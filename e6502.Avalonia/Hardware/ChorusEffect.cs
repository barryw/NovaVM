using System;

namespace e6502.Avalonia.Hardware;

public sealed class ChorusEffect
{
    private const float LfoRate = 0.5f;

    private readonly float[] _bufferL, _bufferR;
    private readonly int _bufferLen;
    private readonly int _centerDelay;
    private readonly float _modDepth;
    private readonly float _lfoInc;

    private int _writeIndex;
    private float _lfoPhaseL;
    private float _lfoPhaseR;

    public ChorusEffect(int sampleRate)
    {
        // ~30ms max delay
        _bufferLen = (int)(sampleRate * 0.030f);
        _bufferL = new float[_bufferLen];
        _bufferR = new float[_bufferLen];

        // ~15ms center delay
        _centerDelay = (int)(sampleRate * 0.015f);

        // ±1ms modulation depth
        _modDepth = sampleRate * 0.001f;

        _lfoInc = 2f * MathF.PI * LfoRate / sampleRate;
        _lfoPhaseL = 0f;
        _lfoPhaseR = MathF.PI * 0.5f; // 90-degree offset for stereo
    }

    public void Reset()
    {
        Array.Clear(_bufferL);
        Array.Clear(_bufferR);
        _writeIndex = 0;
        _lfoPhaseL = 0f;
        _lfoPhaseR = MathF.PI * 0.5f;
    }

    public void Process(float[] left, float[] right)
    {
        int len = Math.Min(left.Length, right.Length);

        for (int i = 0; i < len; i++)
        {
            _bufferL[_writeIndex] = left[i];
            _bufferR[_writeIndex] = right[i];

            // LFO-modulated read positions
            float delayL = _centerDelay + MathF.Sin(_lfoPhaseL) * _modDepth;
            float delayR = _centerDelay + MathF.Sin(_lfoPhaseR) * _modDepth;

            // Return wet-only signal; caller handles dry/wet mixing
            left[i] = ReadInterpolated(_bufferL, delayL);
            right[i] = ReadInterpolated(_bufferR, delayR);

            _lfoPhaseL += _lfoInc;
            _lfoPhaseR += _lfoInc;

            _writeIndex = (_writeIndex + 1) % _bufferLen;
        }

        // Keep LFO phases from growing unbounded
        if (_lfoPhaseL > 2f * MathF.PI) _lfoPhaseL -= 2f * MathF.PI;
        if (_lfoPhaseR > 2f * MathF.PI) _lfoPhaseR -= 2f * MathF.PI;
    }

    private float ReadInterpolated(float[] buffer, float delay)
    {
        float readPos = _writeIndex - delay;
        if (readPos < 0) readPos += _bufferLen;

        int idx0 = (int)readPos;
        int idx1 = (idx0 + 1) % _bufferLen;
        float frac = readPos - idx0;

        return buffer[idx0 % _bufferLen] * (1f - frac) + buffer[idx1] * frac;
    }
}
