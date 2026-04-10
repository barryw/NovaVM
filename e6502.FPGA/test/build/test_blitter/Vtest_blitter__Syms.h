// Verilated -*- C++ -*-
// DESCRIPTION: Verilator output: Symbol table internal header
//
// Internal details; most calling programs do not need this header,
// unless using verilator public meta comments.

#ifndef VERILATED_VTEST_BLITTER__SYMS_H_
#define VERILATED_VTEST_BLITTER__SYMS_H_  // guard

#include "verilated.h"

// INCLUDE MODEL CLASS

#include "Vtest_blitter.h"

// INCLUDE MODULE CLASSES
#include "Vtest_blitter___024root.h"

// SYMS CLASS (contains all model state)
class alignas(VL_CACHE_LINE_BYTES) Vtest_blitter__Syms final : public VerilatedSyms {
  public:
    // INTERNAL STATE
    Vtest_blitter* const __Vm_modelp;
    VlDeleter __Vm_deleter;
    bool __Vm_didInit = false;

    // MODULE INSTANCE STATE
    Vtest_blitter___024root        TOP;

    // CONSTRUCTORS
    Vtest_blitter__Syms(VerilatedContext* contextp, const char* namep, Vtest_blitter* modelp);
    ~Vtest_blitter__Syms();

    // METHODS
    const char* name() const { return TOP.vlNamep; }
};

#endif  // guard
