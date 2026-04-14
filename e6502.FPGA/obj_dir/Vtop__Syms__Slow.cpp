// Verilated -*- C++ -*-
// DESCRIPTION: Verilator output: Symbol table implementation internals

#include "Vtop__pch.h"

Vtop__Syms::Vtop__Syms(VerilatedContext* contextp, const char* namep, Vtop* modelp)
    : VerilatedSyms{contextp}
    // Setup internal state of the Syms class
    , __Vm_modelp{modelp}
    // Setup top module instance
    , TOP{this, namep}
{
    // Check resources
    Verilated::stackCheck(2844);
    // Setup sub module instances
    TOP__top__DOT__sid1_inst.ctor(this, "top.sid1_inst");
    TOP__top__DOT__sid2_inst.ctor(this, "top.sid2_inst");
    // Configure time unit / time precision
    _vm_contextp__->timeunit(-12);
    _vm_contextp__->timeprecision(-12);
    // Setup each module's pointers to their submodules
    TOP.__PVT__top__DOT__sid1_inst = &TOP__top__DOT__sid1_inst;
    TOP.__PVT__top__DOT__sid2_inst = &TOP__top__DOT__sid2_inst;
    // Setup each module's pointer back to symbol table (for public functions)
    TOP.__Vconfigure(true);
    TOP__top__DOT__sid1_inst.__Vconfigure(true);
    TOP__top__DOT__sid2_inst.__Vconfigure(false);
    // Setup scopes
}

Vtop__Syms::~Vtop__Syms() {
    // Tear down scopes
    // Tear down sub module instances
    TOP__top__DOT__sid2_inst.dtor();
    TOP__top__DOT__sid1_inst.dtor();
}
