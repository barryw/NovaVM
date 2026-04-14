// Verilated -*- C++ -*-
// DESCRIPTION: Verilator output: Model implementation (design independent parts)

#include "Vtest_sid__pch.h"

//============================================================
// Constructors

Vtest_sid::Vtest_sid(VerilatedContext* _vcontextp__, const char* _vcname__)
    : VerilatedModel{*_vcontextp__}
    , vlSymsp{new Vtest_sid__Syms(contextp(), _vcname__, this)}
    , rootp{&(vlSymsp->TOP)}
{
    // Register model with the context
    contextp()->addModel(this);
}

Vtest_sid::Vtest_sid(const char* _vcname__)
    : Vtest_sid(Verilated::threadContextp(), _vcname__)
{
}

//============================================================
// Destructor

Vtest_sid::~Vtest_sid() {
    delete vlSymsp;
}

//============================================================
// Evaluation function

#ifdef VL_DEBUG
void Vtest_sid___024root___eval_debug_assertions(Vtest_sid___024root* vlSelf);
#endif  // VL_DEBUG
void Vtest_sid___024root___eval_static(Vtest_sid___024root* vlSelf);
void Vtest_sid___024root___eval_initial(Vtest_sid___024root* vlSelf);
void Vtest_sid___024root___eval_settle(Vtest_sid___024root* vlSelf);
void Vtest_sid___024root___eval(Vtest_sid___024root* vlSelf);

void Vtest_sid::eval_step() {
    VL_DEBUG_IF(VL_DBG_MSGF("+++++TOP Evaluate Vtest_sid::eval_step\n"); );
#ifdef VL_DEBUG
    // Debug assertions
    Vtest_sid___024root___eval_debug_assertions(&(vlSymsp->TOP));
#endif  // VL_DEBUG
    vlSymsp->__Vm_deleter.deleteAll();
    if (VL_UNLIKELY(!vlSymsp->__Vm_didInit)) {
        VL_DEBUG_IF(VL_DBG_MSGF("+ Initial\n"););
        Vtest_sid___024root___eval_static(&(vlSymsp->TOP));
        Vtest_sid___024root___eval_initial(&(vlSymsp->TOP));
        Vtest_sid___024root___eval_settle(&(vlSymsp->TOP));
        vlSymsp->__Vm_didInit = true;
    }
    VL_DEBUG_IF(VL_DBG_MSGF("+ Eval\n"););
    Vtest_sid___024root___eval(&(vlSymsp->TOP));
    // Evaluate cleanup
    Verilated::endOfEval(vlSymsp->__Vm_evalMsgQp);
}

//============================================================
// Events and timing
bool Vtest_sid::eventsPending() { return !vlSymsp->TOP.__VdlySched.empty() && !contextp()->gotFinish(); }

uint64_t Vtest_sid::nextTimeSlot() { return vlSymsp->TOP.__VdlySched.nextTimeSlot(); }

//============================================================
// Utilities

const char* Vtest_sid::name() const {
    return vlSymsp->name();
}

//============================================================
// Invoke final blocks

void Vtest_sid___024root___eval_final(Vtest_sid___024root* vlSelf);

VL_ATTR_COLD void Vtest_sid::final() {
    Vtest_sid___024root___eval_final(&(vlSymsp->TOP));
}

//============================================================
// Implementations of abstract methods from VerilatedModel

const char* Vtest_sid::hierName() const { return vlSymsp->name(); }
const char* Vtest_sid::modelName() const { return "Vtest_sid"; }
unsigned Vtest_sid::threads() const { return 1; }
void Vtest_sid::prepareClone() const { contextp()->prepareClone(); }
void Vtest_sid::atClone() const {
    contextp()->threadPoolpOnClone();
}
