// Verilated -*- C++ -*-
// DESCRIPTION: Verilator output: Symbol table internal header
//
// Internal details; most calling programs do not need this header,
// unless using verilator public meta comments.

#ifndef VERILATED_VVGC__SYMS_H_
#define VERILATED_VVGC__SYMS_H_  // guard

#include "verilated.h"

// INCLUDE MODEL CLASS

#include "Vvgc.h"

// INCLUDE MODULE CLASSES
#include "Vvgc___024root.h"

// DPI TYPES for DPI Export callbacks (Internal use)

// SYMS CLASS (contains all model state)
class alignas(VL_CACHE_LINE_BYTES) Vvgc__Syms final : public VerilatedSyms {
  public:
    // INTERNAL STATE
    Vvgc* const __Vm_modelp;
    VlDeleter __Vm_deleter;
    bool __Vm_didInit = false;

    // MODULE INSTANCE STATE
    Vvgc___024root                 TOP;

    // SCOPE NAMES
    VerilatedScope* __Vscopep_TOP;
    VerilatedScope* __Vscopep_vgc;
    VerilatedScope* __Vscopep_vgc__unnamedblk1;
    VerilatedScope* __Vscopep_vgc__unnamedblk2;
    VerilatedScope* __Vscopep_vgc__unnamedblk3;
    VerilatedScope* __Vscopep_vgc__unnamedblk4;
    VerilatedScope* __Vscopep_vgc__unnamedblk5;
    VerilatedScope* __Vscopep_vgc__unnamedblk6;
    VerilatedScope* __Vscopep_vgc__unnamedblk7;
    VerilatedScope* __Vscopep_vgc__unnamedblk8;

    // CONSTRUCTORS
    Vvgc__Syms(VerilatedContext* contextp, const char* namep, Vvgc* modelp);
    ~Vvgc__Syms();

    // METHODS
    const char* name() const { return TOP.vlNamep; }
};

#endif  // guard
