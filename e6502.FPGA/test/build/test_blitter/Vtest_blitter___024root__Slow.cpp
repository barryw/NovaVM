// Verilated -*- C++ -*-
// DESCRIPTION: Verilator output: Design implementation internals
// See Vtest_blitter.h for the primary calling header

#include "Vtest_blitter__pch.h"

void Vtest_blitter___024root___ctor_var_reset(Vtest_blitter___024root* vlSelf);

Vtest_blitter___024root::Vtest_blitter___024root(Vtest_blitter__Syms* symsp, const char* namep)
    : __VdlySched{*symsp->_vm_contextp__}
 {
    vlSymsp = symsp;
    vlNamep = strdup(namep);
    // Reset structure values
    Vtest_blitter___024root___ctor_var_reset(this);
}

void Vtest_blitter___024root::__Vconfigure(bool first) {
    (void)first;  // Prevent unused variable warning
}

Vtest_blitter___024root::~Vtest_blitter___024root() {
    VL_DO_DANGLING(std::free(const_cast<char*>(vlNamep)), vlNamep);
}
