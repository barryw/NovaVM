// Verilated -*- C++ -*-
// DESCRIPTION: Verilator output: Model implementation (design independent parts)

#include "Vtest_copper_lists__pch.h"

//============================================================
// Constructors

Vtest_copper_lists::Vtest_copper_lists(VerilatedContext* _vcontextp__, const char* _vcname__)
    : VerilatedModel{*_vcontextp__}
    , vlSymsp{new Vtest_copper_lists__Syms(contextp(), _vcname__, this)}
    , rootp{&(vlSymsp->TOP)}
{
    // Register model with the context
    contextp()->addModel(this);
}

Vtest_copper_lists::Vtest_copper_lists(const char* _vcname__)
    : Vtest_copper_lists(Verilated::threadContextp(), _vcname__)
{
}

//============================================================
// Destructor

Vtest_copper_lists::~Vtest_copper_lists() {
    delete vlSymsp;
}

//============================================================
// Evaluation function

#ifdef VL_DEBUG
void Vtest_copper_lists___024root___eval_debug_assertions(Vtest_copper_lists___024root* vlSelf);
#endif  // VL_DEBUG
void Vtest_copper_lists___024root___eval_static(Vtest_copper_lists___024root* vlSelf);
void Vtest_copper_lists___024root___eval_initial(Vtest_copper_lists___024root* vlSelf);
void Vtest_copper_lists___024root___eval_settle(Vtest_copper_lists___024root* vlSelf);
void Vtest_copper_lists___024root___eval(Vtest_copper_lists___024root* vlSelf);

void Vtest_copper_lists::eval_step() {
    VL_DEBUG_IF(VL_DBG_MSGF("+++++TOP Evaluate Vtest_copper_lists::eval_step\n"); );
#ifdef VL_DEBUG
    // Debug assertions
    Vtest_copper_lists___024root___eval_debug_assertions(&(vlSymsp->TOP));
#endif  // VL_DEBUG
    vlSymsp->__Vm_deleter.deleteAll();
    if (VL_UNLIKELY(!vlSymsp->__Vm_didInit)) {
        VL_DEBUG_IF(VL_DBG_MSGF("+ Initial\n"););
        Vtest_copper_lists___024root___eval_static(&(vlSymsp->TOP));
        Vtest_copper_lists___024root___eval_initial(&(vlSymsp->TOP));
        Vtest_copper_lists___024root___eval_settle(&(vlSymsp->TOP));
        vlSymsp->__Vm_didInit = true;
    }
    VL_DEBUG_IF(VL_DBG_MSGF("+ Eval\n"););
    Vtest_copper_lists___024root___eval(&(vlSymsp->TOP));
    // Evaluate cleanup
    Verilated::endOfEval(vlSymsp->__Vm_evalMsgQp);
}

//============================================================
// Events and timing
bool Vtest_copper_lists::eventsPending() { return !vlSymsp->TOP.__VdlySched.empty() && !contextp()->gotFinish(); }

uint64_t Vtest_copper_lists::nextTimeSlot() { return vlSymsp->TOP.__VdlySched.nextTimeSlot(); }

//============================================================
// Utilities

const char* Vtest_copper_lists::name() const {
    return vlSymsp->name();
}

//============================================================
// Invoke final blocks

void Vtest_copper_lists___024root___eval_final(Vtest_copper_lists___024root* vlSelf);

VL_ATTR_COLD void Vtest_copper_lists::final() {
    Vtest_copper_lists___024root___eval_final(&(vlSymsp->TOP));
}

//============================================================
// Implementations of abstract methods from VerilatedModel

const char* Vtest_copper_lists::hierName() const { return vlSymsp->name(); }
const char* Vtest_copper_lists::modelName() const { return "Vtest_copper_lists"; }
unsigned Vtest_copper_lists::threads() const { return 1; }
void Vtest_copper_lists::prepareClone() const { contextp()->prepareClone(); }
void Vtest_copper_lists::atClone() const {
    contextp()->threadPoolpOnClone();
}
