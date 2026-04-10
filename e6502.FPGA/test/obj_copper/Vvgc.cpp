// Verilated -*- C++ -*-
// DESCRIPTION: Verilator output: Model implementation (design independent parts)

#include "Vvgc__pch.h"

//============================================================
// Constructors

Vvgc::Vvgc(VerilatedContext* _vcontextp__, const char* _vcname__)
    : VerilatedModel{*_vcontextp__}
    , vlSymsp{new Vvgc__Syms(contextp(), _vcname__, this)}
    , clk{vlSymsp->TOP.clk}
    , rst{vlSymsp->TOP.rst}
    , cpu_ce{vlSymsp->TOP.cpu_ce}
    , cpu_wdata{vlSymsp->TOP.cpu_wdata}
    , cpu_rdata{vlSymsp->TOP.cpu_rdata}
    , cpu_we{vlSymsp->TOP.cpu_we}
    , cpu_re{vlSymsp->TOP.cpu_re}
    , key_valid{vlSymsp->TOP.key_valid}
    , key_data{vlSymsp->TOP.key_data}
    , vid_r{vlSymsp->TOP.vid_r}
    , vid_g{vlSymsp->TOP.vid_g}
    , vid_b{vlSymsp->TOP.vid_b}
    , vid_hsync{vlSymsp->TOP.vid_hsync}
    , vid_vsync{vlSymsp->TOP.vid_vsync}
    , vid_de{vlSymsp->TOP.vid_de}
    , cpu_addr{vlSymsp->TOP.cpu_addr}
    , rootp{&(vlSymsp->TOP)}
{
    // Register model with the context
    contextp()->addModel(this);
}

Vvgc::Vvgc(const char* _vcname__)
    : Vvgc(Verilated::threadContextp(), _vcname__)
{
}

//============================================================
// Destructor

Vvgc::~Vvgc() {
    delete vlSymsp;
}

//============================================================
// Evaluation function

#ifdef VL_DEBUG
void Vvgc___024root___eval_debug_assertions(Vvgc___024root* vlSelf);
#endif  // VL_DEBUG
void Vvgc___024root___eval_static(Vvgc___024root* vlSelf);
void Vvgc___024root___eval_initial(Vvgc___024root* vlSelf);
void Vvgc___024root___eval_settle(Vvgc___024root* vlSelf);
void Vvgc___024root___eval(Vvgc___024root* vlSelf);

void Vvgc::eval_step() {
    VL_DEBUG_IF(VL_DBG_MSGF("+++++TOP Evaluate Vvgc::eval_step\n"); );
#ifdef VL_DEBUG
    // Debug assertions
    Vvgc___024root___eval_debug_assertions(&(vlSymsp->TOP));
#endif  // VL_DEBUG
    vlSymsp->__Vm_deleter.deleteAll();
    if (VL_UNLIKELY(!vlSymsp->__Vm_didInit)) {
        VL_DEBUG_IF(VL_DBG_MSGF("+ Initial\n"););
        Vvgc___024root___eval_static(&(vlSymsp->TOP));
        Vvgc___024root___eval_initial(&(vlSymsp->TOP));
        Vvgc___024root___eval_settle(&(vlSymsp->TOP));
        vlSymsp->__Vm_didInit = true;
    }
    VL_DEBUG_IF(VL_DBG_MSGF("+ Eval\n"););
    Vvgc___024root___eval(&(vlSymsp->TOP));
    // Evaluate cleanup
    Verilated::endOfEval(vlSymsp->__Vm_evalMsgQp);
}

//============================================================
// Events and timing
bool Vvgc::eventsPending() { return false; }

uint64_t Vvgc::nextTimeSlot() {
    VL_FATAL_MT(__FILE__, __LINE__, "", "No delays in the design");
    return 0;
}

//============================================================
// Utilities

const char* Vvgc::name() const {
    return vlSymsp->name();
}

//============================================================
// Invoke final blocks

void Vvgc___024root___eval_final(Vvgc___024root* vlSelf);

VL_ATTR_COLD void Vvgc::final() {
    Vvgc___024root___eval_final(&(vlSymsp->TOP));
}

//============================================================
// Implementations of abstract methods from VerilatedModel

const char* Vvgc::hierName() const { return vlSymsp->name(); }
const char* Vvgc::modelName() const { return "Vvgc"; }
unsigned Vvgc::threads() const { return 1; }
void Vvgc::prepareClone() const { contextp()->prepareClone(); }
void Vvgc::atClone() const {
    contextp()->threadPoolpOnClone();
}
