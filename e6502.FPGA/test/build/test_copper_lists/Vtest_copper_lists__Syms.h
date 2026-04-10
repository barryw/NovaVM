// Verilated -*- C++ -*-
// DESCRIPTION: Verilator output: Symbol table internal header
//
// Internal details; most calling programs do not need this header,
// unless using verilator public meta comments.

#ifndef VERILATED_VTEST_COPPER_LISTS__SYMS_H_
#define VERILATED_VTEST_COPPER_LISTS__SYMS_H_  // guard

#include "verilated.h"

// INCLUDE MODEL CLASS

#include "Vtest_copper_lists.h"

// INCLUDE MODULE CLASSES
#include "Vtest_copper_lists___024root.h"

// SYMS CLASS (contains all model state)
class alignas(VL_CACHE_LINE_BYTES) Vtest_copper_lists__Syms final : public VerilatedSyms {
  public:
    // INTERNAL STATE
    Vtest_copper_lists* const __Vm_modelp;
    VlDeleter __Vm_deleter;
    bool __Vm_didInit = false;

    // MODULE INSTANCE STATE
    Vtest_copper_lists___024root   TOP;

    // CONSTRUCTORS
    Vtest_copper_lists__Syms(VerilatedContext* contextp, const char* namep, Vtest_copper_lists* modelp);
    ~Vtest_copper_lists__Syms();

    // METHODS
    const char* name() const { return TOP.vlNamep; }
};

#endif  // guard
