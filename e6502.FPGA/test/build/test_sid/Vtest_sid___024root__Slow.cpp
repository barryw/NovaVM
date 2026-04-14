// Verilated -*- C++ -*-
// DESCRIPTION: Verilator output: Design implementation internals
// See Vtest_sid.h for the primary calling header

#include "Vtest_sid__pch.h"

void Vtest_sid___024root___ctor_var_reset(Vtest_sid___024root* vlSelf);

Vtest_sid___024root::Vtest_sid___024root(Vtest_sid__Syms* symsp, const char* namep)
    : __VdlySched{*symsp->_vm_contextp__}
 {
    vlSymsp = symsp;
    vlNamep = strdup(namep);
    // Reset structure values
    Vtest_sid___024root___ctor_var_reset(this);
}

void Vtest_sid___024root::__Vconfigure(bool first) {
    (void)first;  // Prevent unused variable warning
}

Vtest_sid___024root::~Vtest_sid___024root() {
    VL_DO_DANGLING(std::free(const_cast<char*>(vlNamep)), vlNamep);
}
