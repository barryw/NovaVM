// Verilated -*- C++ -*-
// DESCRIPTION: Verilator output: Design implementation internals
// See Vtest_gtext.h for the primary calling header

#include "Vtest_gtext__pch.h"

void Vtest_gtext___024root___ctor_var_reset(Vtest_gtext___024root* vlSelf);

Vtest_gtext___024root::Vtest_gtext___024root(Vtest_gtext__Syms* symsp, const char* namep)
    : __VdlySched{*symsp->_vm_contextp__}
 {
    vlSymsp = symsp;
    vlNamep = strdup(namep);
    // Reset structure values
    Vtest_gtext___024root___ctor_var_reset(this);
}

void Vtest_gtext___024root::__Vconfigure(bool first) {
    (void)first;  // Prevent unused variable warning
}

Vtest_gtext___024root::~Vtest_gtext___024root() {
    VL_DO_DANGLING(std::free(const_cast<char*>(vlNamep)), vlNamep);
}
