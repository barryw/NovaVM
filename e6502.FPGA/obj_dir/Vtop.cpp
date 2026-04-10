// Verilated -*- C++ -*-
// DESCRIPTION: Verilator output: Model implementation (design independent parts)

#include "Vtop__pch.h"

//============================================================
// Constructors

Vtop::Vtop(VerilatedContext* _vcontextp__, const char* _vcname__)
    : VerilatedModel{*_vcontextp__}
    , vlSymsp{new Vtop__Syms(contextp(), _vcname__, this)}
    , clk{vlSymsp->TOP.clk}
    , rst{vlSymsp->TOP.rst}
    , key_valid{vlSymsp->TOP.key_valid}
    , key_data{vlSymsp->TOP.key_data}
    , irq_n{vlSymsp->TOP.irq_n}
    , nmi_n{vlSymsp->TOP.nmi_n}
    , vid_r{vlSymsp->TOP.vid_r}
    , vid_g{vlSymsp->TOP.vid_g}
    , vid_b{vlSymsp->TOP.vid_b}
    , vid_hsync{vlSymsp->TOP.vid_hsync}
    , vid_vsync{vlSymsp->TOP.vid_vsync}
    , vid_de{vlSymsp->TOP.vid_de}
    , dbg_peek_en{vlSymsp->TOP.dbg_peek_en}
    , dbg_peek_data{vlSymsp->TOP.dbg_peek_data}
    , dbg_poke_en{vlSymsp->TOP.dbg_poke_en}
    , dbg_poke_data{vlSymsp->TOP.dbg_poke_data}
    , dbg_pause{vlSymsp->TOP.dbg_pause}
    , dbg_cpu_a{vlSymsp->TOP.dbg_cpu_a}
    , dbg_cpu_x{vlSymsp->TOP.dbg_cpu_x}
    , dbg_cpu_y{vlSymsp->TOP.dbg_cpu_y}
    , dbg_cpu_sp{vlSymsp->TOP.dbg_cpu_sp}
    , dbg_cpu_flags{vlSymsp->TOP.dbg_cpu_flags}
    , dbg_peek_addr{vlSymsp->TOP.dbg_peek_addr}
    , dbg_poke_addr{vlSymsp->TOP.dbg_poke_addr}
    , dbg_cpu_pc{vlSymsp->TOP.dbg_cpu_pc}
    , rootp{&(vlSymsp->TOP)}
{
    // Register model with the context
    contextp()->addModel(this);
}

Vtop::Vtop(const char* _vcname__)
    : Vtop(Verilated::threadContextp(), _vcname__)
{
}

//============================================================
// Destructor

Vtop::~Vtop() {
    delete vlSymsp;
}

//============================================================
// Evaluation function

#ifdef VL_DEBUG
void Vtop___024root___eval_debug_assertions(Vtop___024root* vlSelf);
#endif  // VL_DEBUG
void Vtop___024root___eval_static(Vtop___024root* vlSelf);
void Vtop___024root___eval_initial(Vtop___024root* vlSelf);
void Vtop___024root___eval_settle(Vtop___024root* vlSelf);
void Vtop___024root___eval(Vtop___024root* vlSelf);

void Vtop::eval_step() {
    VL_DEBUG_IF(VL_DBG_MSGF("+++++TOP Evaluate Vtop::eval_step\n"); );
#ifdef VL_DEBUG
    // Debug assertions
    Vtop___024root___eval_debug_assertions(&(vlSymsp->TOP));
#endif  // VL_DEBUG
    vlSymsp->__Vm_deleter.deleteAll();
    if (VL_UNLIKELY(!vlSymsp->__Vm_didInit)) {
        VL_DEBUG_IF(VL_DBG_MSGF("+ Initial\n"););
        Vtop___024root___eval_static(&(vlSymsp->TOP));
        Vtop___024root___eval_initial(&(vlSymsp->TOP));
        Vtop___024root___eval_settle(&(vlSymsp->TOP));
        vlSymsp->__Vm_didInit = true;
    }
    VL_DEBUG_IF(VL_DBG_MSGF("+ Eval\n"););
    Vtop___024root___eval(&(vlSymsp->TOP));
    // Evaluate cleanup
    Verilated::endOfEval(vlSymsp->__Vm_evalMsgQp);
}

//============================================================
// Events and timing
bool Vtop::eventsPending() { return false; }

uint64_t Vtop::nextTimeSlot() {
    VL_FATAL_MT(__FILE__, __LINE__, "", "No delays in the design");
    return 0;
}

//============================================================
// Utilities

const char* Vtop::name() const {
    return vlSymsp->name();
}

//============================================================
// Invoke final blocks

void Vtop___024root___eval_final(Vtop___024root* vlSelf);

VL_ATTR_COLD void Vtop::final() {
    Vtop___024root___eval_final(&(vlSymsp->TOP));
}

//============================================================
// Implementations of abstract methods from VerilatedModel

const char* Vtop::hierName() const { return vlSymsp->name(); }
const char* Vtop::modelName() const { return "Vtop"; }
unsigned Vtop::threads() const { return 1; }
void Vtop::prepareClone() const { contextp()->prepareClone(); }
void Vtop::atClone() const {
    contextp()->threadPoolpOnClone();
}
