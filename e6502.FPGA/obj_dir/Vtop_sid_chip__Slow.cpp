// Verilated -*- C++ -*-
// DESCRIPTION: Verilator output: Design implementation internals
// See Vtop.h for the primary calling header

#include "Vtop__pch.h"

void Vtop_sid_chip___ctor_var_reset(Vtop_sid_chip* vlSelf);

Vtop_sid_chip::Vtop_sid_chip() = default;
Vtop_sid_chip::~Vtop_sid_chip() = default;

void Vtop_sid_chip::ctor(Vtop__Syms* symsp, const char* namep) {
    vlSymsp = symsp;
    vlNamep = strdup(Verilated::catName(vlSymsp->name(), namep));
    // Reset structure values
    Vtop_sid_chip___ctor_var_reset(this);
}

void Vtop_sid_chip::__Vconfigure(bool first) {
    (void)first;  // Prevent unused variable warning
}

void Vtop_sid_chip::dtor() {
    VL_DO_DANGLING(std::free(const_cast<char*>(vlNamep)), vlNamep);
}
