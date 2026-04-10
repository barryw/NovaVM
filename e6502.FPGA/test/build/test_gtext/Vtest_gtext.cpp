// Verilated -*- C++ -*-
// DESCRIPTION: Verilator output: Model implementation (design independent parts)

#include "Vtest_gtext__pch.h"

//============================================================
// Constructors

Vtest_gtext::Vtest_gtext(VerilatedContext* _vcontextp__, const char* _vcname__)
    : VerilatedModel{*_vcontextp__}
    , vlSymsp{new Vtest_gtext__Syms(contextp(), _vcname__, this)}
    , rootp{&(vlSymsp->TOP)}
{
    // Register model with the context
    contextp()->addModel(this);
}

Vtest_gtext::Vtest_gtext(const char* _vcname__)
    : Vtest_gtext(Verilated::threadContextp(), _vcname__)
{
}

//============================================================
// Destructor

Vtest_gtext::~Vtest_gtext() {
    delete vlSymsp;
}

//============================================================
// Evaluation function

#ifdef VL_DEBUG
void Vtest_gtext___024root___eval_debug_assertions(Vtest_gtext___024root* vlSelf);
#endif  // VL_DEBUG
void Vtest_gtext___024root___eval_static(Vtest_gtext___024root* vlSelf);
void Vtest_gtext___024root___eval_initial(Vtest_gtext___024root* vlSelf);
void Vtest_gtext___024root___eval_settle(Vtest_gtext___024root* vlSelf);
void Vtest_gtext___024root___eval(Vtest_gtext___024root* vlSelf);

void Vtest_gtext::eval_step() {
    VL_DEBUG_IF(VL_DBG_MSGF("+++++TOP Evaluate Vtest_gtext::eval_step\n"); );
#ifdef VL_DEBUG
    // Debug assertions
    Vtest_gtext___024root___eval_debug_assertions(&(vlSymsp->TOP));
#endif  // VL_DEBUG
    vlSymsp->__Vm_deleter.deleteAll();
    if (VL_UNLIKELY(!vlSymsp->__Vm_didInit)) {
        VL_DEBUG_IF(VL_DBG_MSGF("+ Initial\n"););
        Vtest_gtext___024root___eval_static(&(vlSymsp->TOP));
        Vtest_gtext___024root___eval_initial(&(vlSymsp->TOP));
        Vtest_gtext___024root___eval_settle(&(vlSymsp->TOP));
        vlSymsp->__Vm_didInit = true;
    }
    VL_DEBUG_IF(VL_DBG_MSGF("+ Eval\n"););
    Vtest_gtext___024root___eval(&(vlSymsp->TOP));
    // Evaluate cleanup
    Verilated::endOfEval(vlSymsp->__Vm_evalMsgQp);
}

//============================================================
// Events and timing
bool Vtest_gtext::eventsPending() { return !vlSymsp->TOP.__VdlySched.empty() && !contextp()->gotFinish(); }

uint64_t Vtest_gtext::nextTimeSlot() { return vlSymsp->TOP.__VdlySched.nextTimeSlot(); }

//============================================================
// Utilities

const char* Vtest_gtext::name() const {
    return vlSymsp->name();
}

//============================================================
// Invoke final blocks

void Vtest_gtext___024root___eval_final(Vtest_gtext___024root* vlSelf);

VL_ATTR_COLD void Vtest_gtext::final() {
    Vtest_gtext___024root___eval_final(&(vlSymsp->TOP));
}

//============================================================
// Implementations of abstract methods from VerilatedModel

const char* Vtest_gtext::hierName() const { return vlSymsp->name(); }
const char* Vtest_gtext::modelName() const { return "Vtest_gtext"; }
unsigned Vtest_gtext::threads() const { return 1; }
void Vtest_gtext::prepareClone() const { contextp()->prepareClone(); }
void Vtest_gtext::atClone() const {
    contextp()->threadPoolpOnClone();
}
