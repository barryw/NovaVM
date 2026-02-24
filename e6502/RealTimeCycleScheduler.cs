using System.Diagnostics;

namespace KDS.e6502
{
    /// <summary>
    /// Converts wall-clock time into a cycle budget for deterministic CPU pacing.
    /// </summary>
    public sealed class RealTimeCycleScheduler
    {
        private readonly Func<long> _timestampProvider;
        private readonly long _timestampFrequency;
        private readonly int _targetCyclesPerSecond;
        private readonly int _maxBacklogCycles;

        private bool _initialized;
        private long _lastTimestamp;
        private double _pendingCycles;

        public RealTimeCycleScheduler(
            int targetCyclesPerSecond,
            int maxBacklogCycles,
            Func<long>? timestampProvider = null,
            long timestampFrequency = 0)
        {
            _targetCyclesPerSecond = targetCyclesPerSecond > 0 ? targetCyclesPerSecond : throw new ArgumentOutOfRangeException(nameof(targetCyclesPerSecond));
            _maxBacklogCycles = maxBacklogCycles > 0 ? maxBacklogCycles : throw new ArgumentOutOfRangeException(nameof(maxBacklogCycles));
            _timestampProvider = timestampProvider ?? Stopwatch.GetTimestamp;
            _timestampFrequency = timestampFrequency > 0 ? timestampFrequency : Stopwatch.Frequency;
        }

        public int TargetCyclesPerSecond => _targetCyclesPerSecond;
        public int MaxBacklogCycles => _maxBacklogCycles;
        public double PendingCycles => _pendingCycles;

        public void Reset()
        {
            _initialized = false;
            _lastTimestamp = 0;
            _pendingCycles = 0;
        }

        public int TakeCycleBudget(int maxCycles = int.MaxValue)
        {
            if (maxCycles <= 0)
                return 0;

            long now = _timestampProvider();
            if (!_initialized)
            {
                _initialized = true;
                _lastTimestamp = now;
                return 0;
            }

            long deltaTicks = now - _lastTimestamp;
            _lastTimestamp = now;
            if (deltaTicks > 0)
            {
                _pendingCycles += (double)deltaTicks * _targetCyclesPerSecond / _timestampFrequency;
                if (_pendingCycles > _maxBacklogCycles)
                    _pendingCycles = _maxBacklogCycles;
            }

            int budget = (int)Math.Min(_pendingCycles, maxCycles);
            _pendingCycles -= budget;
            return budget;
        }
    }
}
