// Verilated -*- C++ -*-
// DESCRIPTION: Verilator output: Design implementation internals
// See Vtest_copper.h for the primary calling header

#include "Vtest_copper__pch.h"

void Vtest_copper___024root___ctor_var_reset(Vtest_copper___024root* vlSelf);

Vtest_copper___024root::Vtest_copper___024root(Vtest_copper__Syms* symsp, const char* namep)
    : __VdlySched{*symsp->_vm_contextp__}
 {
    vlSymsp = symsp;
    vlNamep = strdup(namep);
    // Reset structure values
    Vtest_copper___024root___ctor_var_reset(this);
}

void Vtest_copper___024root::__Vconfigure(bool first) {
    (void)first;  // Prevent unused variable warning
}

Vtest_copper___024root::~Vtest_copper___024root() {
    VL_DO_DANGLING(std::free(const_cast<char*>(vlNamep)), vlNamep);
}
