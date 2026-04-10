// Verilated -*- C++ -*-
// DESCRIPTION: Verilator output: Model implementation (design independent parts)

#include "Vtest_blitter__pch.h"

//============================================================
// Constructors

Vtest_blitter::Vtest_blitter(VerilatedContext* _vcontextp__, const char* _vcname__)
    : VerilatedModel{*_vcontextp__}
    , vlSymsp{new Vtest_blitter__Syms(contextp(), _vcname__, this)}
    , rootp{&(vlSymsp->TOP)}
{
    // Register model with the context
    contextp()->addModel(this);
}

Vtest_blitter::Vtest_blitter(const char* _vcname__)
    : Vtest_blitter(Verilated::threadContextp(), _vcname__)
{
}

//============================================================
// Destructor

Vtest_blitter::~Vtest_blitter() {
    delete vlSymsp;
}

//============================================================
// Evaluation function

#ifdef VL_DEBUG
void Vtest_blitter___024root___eval_debug_assertions(Vtest_blitter___024root* vlSelf);
#endif  // VL_DEBUG
void Vtest_blitter___024root___eval_static(Vtest_blitter___024root* vlSelf);
void Vtest_blitter___024root___eval_initial(Vtest_blitter___024root* vlSelf);
void Vtest_blitter___024root___eval_settle(Vtest_blitter___024root* vlSelf);
void Vtest_blitter___024root___eval(Vtest_blitter___024root* vlSelf);

void Vtest_blitter::eval_step() {
    VL_DEBUG_IF(VL_DBG_MSGF("+++++TOP Evaluate Vtest_blitter::eval_step\n"); );
#ifdef VL_DEBUG
    // Debug assertions
    Vtest_blitter___024root___eval_debug_assertions(&(vlSymsp->TOP));
#endif  // VL_DEBUG
    vlSymsp->__Vm_deleter.deleteAll();
    if (VL_UNLIKELY(!vlSymsp->__Vm_didInit)) {
        VL_DEBUG_IF(VL_DBG_MSGF("+ Initial\n"););
        Vtest_blitter___024root___eval_static(&(vlSymsp->TOP));
        Vtest_blitter___024root___eval_initial(&(vlSymsp->TOP));
        Vtest_blitter___024root___eval_settle(&(vlSymsp->TOP));
        vlSymsp->__Vm_didInit = true;
    }
    VL_DEBUG_IF(VL_DBG_MSGF("+ Eval\n"););
    Vtest_blitter___024root___eval(&(vlSymsp->TOP));
    // Evaluate cleanup
    Verilated::endOfEval(vlSymsp->__Vm_evalMsgQp);
}

//============================================================
// Events and timing
bool Vtest_blitter::eventsPending() { return !vlSymsp->TOP.__VdlySched.empty() && !contextp()->gotFinish(); }

uint64_t Vtest_blitter::nextTimeSlot() { return vlSymsp->TOP.__VdlySched.nextTimeSlot(); }

//============================================================
// Utilities

const char* Vtest_blitter::name() const {
    return vlSymsp->name();
}

//============================================================
// Invoke final blocks

void Vtest_blitter___024root___eval_final(Vtest_blitter___024root* vlSelf);

VL_ATTR_COLD void Vtest_blitter::final() {
    Vtest_blitter___024root___eval_final(&(vlSymsp->TOP));
}

//============================================================
// Implementations of abstract methods from VerilatedModel

const char* Vtest_blitter::hierName() const { return vlSymsp->name(); }
const char* Vtest_blitter::modelName() const { return "Vtest_blitter"; }
unsigned Vtest_blitter::threads() const { return 1; }
void Vtest_blitter::prepareClone() const { contextp()->prepareClone(); }
void Vtest_blitter::atClone() const {
    contextp()->threadPoolpOnClone();
}
