// Diagnostic helper: replay a Nova SID rtltrace file through VICE's reSID
// library and emit per-frame raw output metrics.
//
// Build example:
//   c++ -std=c++17 -O2 -I/Users/barry/Git/vice-emu-code/vice/src \
//     -I/Users/barry/Git/vice-emu-code/vice/src/resid \
//     tools/vice_resid_trace_replay.cpp \
//     /Users/barry/Git/vice-emu-code/vice/src/resid/libresid.a \
//     -o /private/tmp/vice_resid_trace_replay

#include <cstdlib>
#include <fstream>
#include <iostream>
#include <limits>
#include <string>

#include "resid/sid.h"

namespace
{
long long Abs(long long value)
{
    return value < 0 ? -value : value;
}

void Usage(const char* argv0)
{
    std::cerr << "Usage: " << argv0
              << " <trace.rtltrace> <metrics.csv> [6581|8580] [frame_ticks]\n";
}
}

int main(int argc, char** argv)
{
    if (argc < 3)
    {
        Usage(argv[0]);
        return 1;
    }

    const std::string tracePath = argv[1];
    const std::string metricsPath = argv[2];
    const std::string mode = argc >= 4 ? argv[3] : "8580";
    const int frameTicks = argc >= 5 ? std::atoi(argv[4]) : 19705;

    std::ifstream trace(tracePath);
    if (!trace)
    {
        std::cerr << "Could not open trace: " << tracePath << "\n";
        return 1;
    }

    std::ofstream metrics(metricsPath);
    if (!metrics)
    {
        std::cerr << "Could not open metrics: " << metricsPath << "\n";
        return 1;
    }

    reSID::SID sid;
    sid.set_chip_model(mode == "8580" ? reSID::MOS8580 : reSID::MOS6581);
    sid.set_voice_mask(0x07);
    sid.enable_filter(true);
    sid.enable_external_filter(true);
    sid.input(0);
    sid.set_sampling_parameters(985248, reSID::SAMPLE_FAST, 48000);
    sid.reset();

    auto captureFrame = [&](int frameNo)
    {
        long long minSample = std::numeric_limits<long long>::max();
        long long maxSample = std::numeric_limits<long long>::min();
        long long sumSample = 0;
        long long sumAbs = 0;

        for (int i = 0; i < frameTicks; i++)
        {
            sid.clock();
            int sample = sid.output();
            if (sample < minSample)
                minSample = sample;
            if (sample > maxSample)
                maxSample = sample;
            sumSample += sample;
            sumAbs += Abs(sample);
        }

        long long peak = Abs(minSample) > Abs(maxSample) ? Abs(minSample) : Abs(maxSample);
        metrics << frameNo << ','
                << mode << ','
                << peak << ','
                << (maxSample - minSample) << ','
                << (sumSample / frameTicks) << ','
                << (sumAbs / frameTicks) << '\n';
    };

    metrics << "frame,mode,peak,ac_range,mean,mean_abs\n";

    int frameNo;
    int chip;
    int regNo;
    int value;
    int currentFrame = std::numeric_limits<int>::min();

    while (trace >> frameNo >> chip >> std::hex >> regNo >> value >> std::dec)
    {
        if (currentFrame == std::numeric_limits<int>::min())
            currentFrame = frameNo;

        while (frameNo != currentFrame)
        {
            captureFrame(currentFrame);
            currentFrame++;
        }

        if (chip == 1)
        {
            sid.write(static_cast<reSID::reg8>(regNo), static_cast<reSID::reg8>(value));
            sid.clock();
        }
    }

    if (currentFrame != std::numeric_limits<int>::min())
        captureFrame(currentFrame);

    return 0;
}
