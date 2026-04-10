// Verilated -*- C++ -*-
// DESCRIPTION: Verilator output: Design implementation internals
// See Vtest_copper_lists.h for the primary calling header

#include "Vtest_copper_lists__pch.h"

void Vtest_copper_lists___024root___ctor_var_reset(Vtest_copper_lists___024root* vlSelf);

Vtest_copper_lists___024root::Vtest_copper_lists___024root(Vtest_copper_lists__Syms* symsp, const char* namep)
    : __VdlySched{*symsp->_vm_contextp__}
 {
    vlSymsp = symsp;
    vlNamep = strdup(namep);
    // Reset structure values
    Vtest_copper_lists___024root___ctor_var_reset(this);
}

void Vtest_copper_lists___024root::__Vconfigure(bool first) {
    (void)first;  // Prevent unused variable warning
}

Vtest_copper_lists___024root::~Vtest_copper_lists___024root() {
    VL_DO_DANGLING(std::free(const_cast<char*>(vlNamep)), vlNamep);
}
