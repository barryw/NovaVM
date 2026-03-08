using e6502.Avalonia.Hardware;
using Microsoft.VisualStudio.TestTools.UnitTesting;
using System;
using System.Linq;

namespace e6502UnitTests;

[TestClass]
public class AudioEffectsTests
{
    [TestMethod]
    public void Reverb_ImpulseProducesTail()
    {
        var reverb = new ReverbEffect(44100);
        float[] left = new float[8820];   // 200ms
        float[] right = new float[8820];
        left[0] = 1.0f;
        right[0] = 1.0f;

        reverb.Process(left, right);

        double tailEnergy = 0;
        for (int i = 4410; i < 8820; i++)
            tailEnergy += left[i] * left[i] + right[i] * right[i];

        Assert.IsTrue(tailEnergy > 0.001, "Reverb should produce a decay tail");
    }

    [TestMethod]
    public void Reverb_SilenceInSilenceOut()
    {
        var reverb = new ReverbEffect(44100);
        float[] left = new float[1024];
        float[] right = new float[1024];
        reverb.Process(left, right);
        Assert.IsTrue(left.All(s => s == 0f));
        Assert.IsTrue(right.All(s => s == 0f));
    }

    [TestMethod]
    public void Chorus_ModulatesSignal()
    {
        var chorus = new ChorusEffect(44100);
        float[] left = new float[4410];
        float[] right = new float[4410];
        for (int i = 0; i < left.Length; i++)
        {
            float v = MathF.Sin(2f * MathF.PI * 440f * i / 44100f);
            left[i] = v;
            right[i] = v;
        }

        float[] origLeft = (float[])left.Clone();
        chorus.Process(left, right);

        double diff = 0;
        for (int i = 0; i < left.Length; i++)
            diff += Math.Abs(left[i] - origLeft[i]);

        Assert.IsTrue(diff > 1.0, "Chorus should modify the signal");
    }

    [TestMethod]
    public void Chorus_SilenceInSilenceOut()
    {
        var chorus = new ChorusEffect(44100);
        float[] left = new float[1024];
        float[] right = new float[1024];
        chorus.Process(left, right);
        Assert.IsTrue(left.All(s => s == 0f));
    }
}
