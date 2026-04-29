// Generic Verilator trace runner for SystemVerilog self-checking tests.
//
// The Makefile uses `--prefix Vtrace_top` when TRACE=1, so every test gets the
// same generated C++ class name even though the Verilog top module changes.

#include "verilated.h"
#include "verilated_fst_c.h"
#include "Vtrace_top.h"

#include <cstdio>
#include <cstdlib>
#include <memory>
#include <string>

#ifndef TRACE_DEPTH
#define TRACE_DEPTH 5
#endif

static std::string tracePath(int argc, char** argv)
{
    const char* env = std::getenv("TRACE_FILE");
    if (env && env[0])
        return std::string(env);

    if (argc > 0 && argv && argv[0] && argv[0][0])
        return std::string(argv[0]) + ".fst";

    return "trace.fst";
}

int main(int argc, char** argv, char**)
{
    Verilated::debug(0);

    const std::unique_ptr<VerilatedContext> contextp{new VerilatedContext};
    contextp->traceEverOn(true);
    contextp->threads(1);
    contextp->commandArgs(argc, argv);

    const std::unique_ptr<Vtrace_top> topp{new Vtrace_top{contextp.get(), ""}};
    const std::unique_ptr<VerilatedFstC> tracep{new VerilatedFstC};

    const std::string path = tracePath(argc, argv);
    topp->trace(tracep.get(), TRACE_DEPTH);
    tracep->open(path.c_str());
    std::fprintf(stderr, "trace: %s\n", path.c_str());

    while (VL_LIKELY(!contextp->gotFinish())) {
        topp->eval();
        tracep->dump(contextp->time());

        if (!topp->eventsPending())
            break;

        contextp->time(topp->nextTimeSlot());
    }

    if (VL_LIKELY(!contextp->gotFinish())) {
        VL_DEBUG_IF(VL_PRINTF("+ Exiting without $finish; no events left\n"););
    }

    topp->final();
    tracep->close();
    contextp->statsPrintSummary();

    return 0;
}
