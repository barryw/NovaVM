namespace e6502.Avalonia.Hardware;

public static class MmlParser
{
    private const int TicksPerQuarter = 96;

    // Indexed by letter A=0..G=6, values are semitone offsets
    private static readonly int[] NoteOffsets = { 9, 11, 0, 2, 4, 5, 7 }; // A B C D E F G

    public static List<MmlEvent> Parse(string mml)
    {
        var events = new List<MmlEvent>();
        mml = mml.ToUpperInvariant();
        int pos = 0;
        int octave = 4;
        int defaultLen = 4;
        bool portamentoNext = false;

        // Expand loops first via recursive helper
        mml = ExpandLoops(mml);

        while (pos < mml.Length)
        {
            char ch = mml[pos];

            if (ch == ' ' || ch == '\t' || ch == '\r' || ch == '\n' || ch == '|')
            {
                pos++;
                continue;
            }

            if (ch >= 'A' && ch <= 'G')
            {
                pos++;
                int semitone = NoteOffsets[ch - 'A'];

                // Sharp / flat
                if (pos < mml.Length && (mml[pos] == '#' || mml[pos] == '+'))
                {
                    semitone++;
                    pos++;
                }
                else if (pos < mml.Length && mml[pos] == '-')
                {
                    semitone--;
                    pos++;
                }

                int midi = (octave + 1) * 12 + semitone;
                int ticks = ReadDuration(mml, ref pos, defaultLen);

                // Tie: merge durations
                while (pos < mml.Length && mml[pos] == '&')
                {
                    pos++; // skip '&'
                    // skip optional repeated note letter
                    if (pos < mml.Length && mml[pos] >= 'A' && mml[pos] <= 'G')
                    {
                        pos++;
                        // skip optional sharp/flat on tied note
                        if (pos < mml.Length && (mml[pos] == '#' || mml[pos] == '+' || mml[pos] == '-'))
                            pos++;
                    }
                    ticks += ReadDuration(mml, ref pos, defaultLen);
                }

                if (portamentoNext)
                {
                    events.Add(new MmlEvent(MmlEventType.Portamento));
                    portamentoNext = false;
                }

                events.Add(new MmlEvent(MmlEventType.NoteOn, midi, ticks));
                continue;
            }

            if (ch == 'R')
            {
                pos++;
                int ticks = ReadDuration(mml, ref pos, defaultLen);
                events.Add(new MmlEvent(MmlEventType.Rest, 0, ticks));
                continue;
            }

            if (ch == 'O')
            {
                pos++;
                int o = ReadInt(mml, ref pos, octave);
                octave = Math.Clamp(o, 0, 7);
                events.Add(new MmlEvent(MmlEventType.SetOctave, octave));
                continue;
            }

            if (ch == '>')
            {
                pos++;
                octave = Math.Min(octave + 1, 7);
                events.Add(new MmlEvent(MmlEventType.OctaveUp));
                continue;
            }

            if (ch == '<')
            {
                pos++;
                octave = Math.Max(octave - 1, 0);
                events.Add(new MmlEvent(MmlEventType.OctaveDown));
                continue;
            }

            if (ch == 'L')
            {
                pos++;
                int len = ReadInt(mml, ref pos, defaultLen);
                defaultLen = len;
                events.Add(new MmlEvent(MmlEventType.SetDefaultLen, len));
                continue;
            }

            if (ch == 'T')
            {
                pos++;
                int bpm = ReadInt(mml, ref pos, 120);
                events.Add(new MmlEvent(MmlEventType.SetTempo, bpm));
                continue;
            }

            if (ch == 'I')
            {
                pos++;
                int inst = ReadInt(mml, ref pos, 0);
                events.Add(new MmlEvent(MmlEventType.SetInstrument, inst));
                continue;
            }

            if (ch == '~')
            {
                pos++;
                int depth = ReadInt(mml, ref pos, 0);
                events.Add(new MmlEvent(MmlEventType.SetVibrato, depth));
                continue;
            }

            if (ch == '/')
            {
                pos++;
                portamentoNext = true;
                continue;
            }

            if (ch == '{')
            {
                pos++; // skip '{'
                var arpNotes = new List<int>();
                while (pos < mml.Length && mml[pos] != '}')
                {
                    char nc = mml[pos];
                    if (nc >= 'A' && nc <= 'G')
                    {
                        pos++;
                        int semitone = NoteOffsets[nc - 'A'];
                        if (pos < mml.Length && (mml[pos] == '#' || mml[pos] == '+'))
                        { semitone++; pos++; }
                        else if (pos < mml.Length && mml[pos] == '-')
                        { semitone--; pos++; }
                        arpNotes.Add((octave + 1) * 12 + semitone);
                    }
                    else
                    {
                        pos++;
                    }
                }
                if (pos < mml.Length) pos++; // skip '}'
                int ticks = ReadDuration(mml, ref pos, defaultLen);
                events.Add(new MmlEvent(MmlEventType.ArpStart, arpNotes.Count, ticks, arpNotes.ToArray()));
                continue;
            }

            if (ch == '@')
            {
                pos++; // skip '@'
                if (pos >= mml.Length) break;

                char sub = mml[pos];

                if (sub == 'P')
                {
                    pos++;
                    if (pos < mml.Length && mml[pos] == 'S')
                    {
                        pos++; // skip 'S'
                        int dir = 0;
                        if (pos < mml.Length && mml[pos] == '+') { dir = 1; pos++; }
                        else if (pos < mml.Length && mml[pos] == '-') { dir = -1; pos++; }
                        else if (pos < mml.Length && mml[pos] == '0') { dir = 0; pos++; }
                        events.Add(new MmlEvent(MmlEventType.PwmSweep, dir));
                    }
                    else
                    {
                        int pw = ReadInt(mml, ref pos, 2048);
                        events.Add(new MmlEvent(MmlEventType.SetPulseWidth, pw));
                    }
                    continue;
                }

                if (sub == 'F')
                {
                    pos++;
                    if (pos >= mml.Length) continue;

                    char fsub = mml[pos];

                    if (fsub == 'L' || fsub == 'B' || fsub == 'H' || fsub == 'O')
                    {
                        pos++;
                        int mode = fsub == 'L' ? 1 : fsub == 'B' ? 2 : fsub == 'H' ? 4 : 0;
                        events.Add(new MmlEvent(MmlEventType.FilterMode, mode));
                    }
                    else if (fsub == 'S')
                    {
                        pos++;
                        int dir = 0;
                        if (pos < mml.Length && mml[pos] == '+') { dir = 1; pos++; }
                        else if (pos < mml.Length && mml[pos] == '-') { dir = -1; pos++; }
                        else if (pos < mml.Length && mml[pos] == '0') { dir = 0; pos++; }
                        events.Add(new MmlEvent(MmlEventType.FilterSweep, dir));
                    }
                    else
                    {
                        // @Fcutoff,resonance
                        int cutoff = ReadInt(mml, ref pos, 0);
                        int resonance = 0;
                        if (pos < mml.Length && mml[pos] == ',')
                        {
                            pos++;
                            resonance = ReadInt(mml, ref pos, 0);
                        }
                        events.Add(new MmlEvent(MmlEventType.SetFilterCutoff, cutoff, resonance));
                    }
                    continue;
                }

                // Unknown @ command — skip
                continue;
            }

            // Sentinel: LoopStart \x01 count \x01
            if (ch == '\x01')
            {
                pos++;
                int count = ReadInt(mml, ref pos, 1);
                if (pos < mml.Length && mml[pos] == '\x01') pos++;
                events.Add(new MmlEvent(MmlEventType.LoopStart, count));
                continue;
            }

            // Sentinel: LoopEnd \x02
            if (ch == '\x02')
            {
                pos++;
                // Find the count from the preceding LoopStart sentinel — we encoded count into LoopStart
                // Actually the count was emitted in LoopStart event; LoopEnd just needs the count too.
                // We stored it in the expansion. Walk back to find last LoopStart.
                int loopCount = FindLastLoopCount(events);
                events.Add(new MmlEvent(MmlEventType.LoopEnd, loopCount));
                continue;
            }

            // Unknown character — skip
            pos++;
        }

        return events;
    }

    private static int FindLastLoopCount(List<MmlEvent> events)
    {
        for (int i = events.Count - 1; i >= 0; i--)
            if (events[i].Type == MmlEventType.LoopStart)
                return events[i].Param1;
        return 1;
    }

    // ExpandLoops handles [...] N times by rewriting the string (no nesting for simplicity)
    private static string ExpandLoops(string mml)
    {
        while (true)
        {
            int open = mml.LastIndexOf('[');
            if (open < 0) break;
            int close = mml.IndexOf(']', open);
            if (close < 0) break;

            string inner = mml.Substring(open + 1, close - open - 1);
            int afterClose = close + 1;

            // Read repeat count
            int start = afterClose;
            int end = start;
            while (end < mml.Length && char.IsDigit(mml[end])) end++;
            int count = end > start ? int.Parse(mml.Substring(start, end - start)) : 1;
            afterClose = end;

            // \x01count\x01 = LoopStart sentinel, \x02 = LoopEnd sentinel
            string expanded = "\x01" + count + "\x01" + string.Concat(Enumerable.Repeat(inner, count)) + "\x02";
            mml = mml.Substring(0, open) + expanded + mml.Substring(afterClose);
        }
        return mml;
    }

    private static int ReadDuration(string mml, ref int pos, int defaultLen)
    {
        int len = ReadInt(mml, ref pos, defaultLen);
        int ticks = TicksPerQuarter * 4 / len; // whole=384, quarter=96, eighth=48
        if (pos < mml.Length && mml[pos] == '.')
        {
            pos++;
            ticks = ticks * 3 / 2;
        }
        return ticks;
    }

    private static int ReadInt(string mml, ref int pos, int defaultVal)
    {
        int start = pos;
        while (pos < mml.Length && char.IsDigit(mml[pos])) pos++;
        if (pos == start) return defaultVal;
        return int.Parse(mml.AsSpan(start, pos - start));
    }
}
