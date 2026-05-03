using System;

namespace e6502.Avalonia.Hardware;

public sealed class ReverbEffect
{
    private const float RoomSize = 0.84f;
    private const float Damp = 0.2f;
    private const float AllpassFeedback = 0.5f;
    private const int StereoSpread = 23;

    private static readonly int[] CombTunings = { 1116, 1188, 1277, 1356, 1422, 1491, 1557, 1617 };
    private static readonly int[] AllpassTunings = { 556, 441, 341, 225 };

    private readonly CombFilter[] _combsL, _combsR;
    private readonly AllpassFilter[] _allpassL, _allpassR;

    public ReverbEffect(int sampleRate)
    {
        double scale = sampleRate / 44100.0;

        _combsL = new CombFilter[CombTunings.Length];
        _combsR = new CombFilter[CombTunings.Length];
        for (int i = 0; i < CombTunings.Length; i++)
        {
            _combsL[i] = new CombFilter((int)(CombTunings[i] * scale));
            _combsR[i] = new CombFilter((int)((CombTunings[i] + StereoSpread) * scale));
        }

        _allpassL = new AllpassFilter[AllpassTunings.Length];
        _allpassR = new AllpassFilter[AllpassTunings.Length];
        for (int i = 0; i < AllpassTunings.Length; i++)
        {
            _allpassL[i] = new AllpassFilter((int)(AllpassTunings[i] * scale));
            _allpassR[i] = new AllpassFilter((int)((AllpassTunings[i] + StereoSpread) * scale));
        }
    }

    public void Reset()
    {
        for (int i = 0; i < _combsL.Length; i++)
        {
            _combsL[i].Reset();
            _combsR[i].Reset();
        }

        for (int i = 0; i < _allpassL.Length; i++)
        {
            _allpassL[i].Reset();
            _allpassR[i].Reset();
        }
    }

    public void Process(float[] left, float[] right)
    {
        int len = Math.Min(left.Length, right.Length);

        for (int i = 0; i < len; i++)
        {
            float inputL = left[i];
            float inputR = right[i];

            float outL = 0, outR = 0;

            for (int c = 0; c < _combsL.Length; c++)
            {
                outL += _combsL[c].Tick(inputL);
                outR += _combsR[c].Tick(inputR);
            }

            // Scale down comb sum to prevent runaway amplification
            const float CombScale = 1f / 8f;
            outL *= CombScale;
            outR *= CombScale;

            for (int a = 0; a < _allpassL.Length; a++)
            {
                outL = _allpassL[a].Tick(outL);
                outR = _allpassR[a].Tick(outR);
            }

            // Return wet-only signal; caller handles dry/wet mixing
            left[i] = outL;
            right[i] = outR;
        }
    }

    private sealed class CombFilter
    {
        private readonly float[] _buffer;
        private int _index;
        private float _filterState;

        public CombFilter(int length)
        {
            _buffer = new float[length];
        }

        public void Reset()
        {
            Array.Clear(_buffer);
            _index = 0;
            _filterState = 0;
        }

        public float Tick(float input)
        {
            float output = _buffer[_index];
            _filterState = output * (1f - Damp) + _filterState * Damp;
            _buffer[_index] = input + _filterState * RoomSize;
            _index = (_index + 1) % _buffer.Length;
            return output;
        }
    }

    private sealed class AllpassFilter
    {
        private readonly float[] _buffer;
        private int _index;

        public AllpassFilter(int length)
        {
            _buffer = new float[length];
        }

        public void Reset()
        {
            Array.Clear(_buffer);
            _index = 0;
        }

        public float Tick(float input)
        {
            float buffered = _buffer[_index];
            float output = -input + buffered;
            _buffer[_index] = input + buffered * AllpassFeedback;
            _index = (_index + 1) % _buffer.Length;
            return output;
        }
    }
}
