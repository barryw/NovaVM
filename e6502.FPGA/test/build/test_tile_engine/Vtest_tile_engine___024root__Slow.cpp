// Verilated -*- C++ -*-
// DESCRIPTION: Verilator output: Design implementation internals
// See Vtest_tile_engine.h for the primary calling header

#include "Vtest_tile_engine__pch.h"

void Vtest_tile_engine___024root___ctor_var_reset(Vtest_tile_engine___024root* vlSelf);

Vtest_tile_engine___024root::Vtest_tile_engine___024root(Vtest_tile_engine__Syms* symsp, const char* namep)
    : __VdlySched{*symsp->_vm_contextp__}
 {
    vlSymsp = symsp;
    vlNamep = strdup(namep);
    // Reset structure values
    Vtest_tile_engine___024root___ctor_var_reset(this);
}

void Vtest_tile_engine___024root::__Vconfigure(bool first) {
    (void)first;  // Prevent unused variable warning
}

Vtest_tile_engine___024root::~Vtest_tile_engine___024root() {
    VL_DO_DANGLING(std::free(const_cast<char*>(vlNamep)), vlNamep);
}
