// Verilated -*- C++ -*-
// DESCRIPTION: Verilator output: Model implementation (design independent parts)

#include "Vtest_tile_engine__pch.h"

//============================================================
// Constructors

Vtest_tile_engine::Vtest_tile_engine(VerilatedContext* _vcontextp__, const char* _vcname__)
    : VerilatedModel{*_vcontextp__}
    , vlSymsp{new Vtest_tile_engine__Syms(contextp(), _vcname__, this)}
    , rootp{&(vlSymsp->TOP)}
{
    // Register model with the context
    contextp()->addModel(this);
}

Vtest_tile_engine::Vtest_tile_engine(const char* _vcname__)
    : Vtest_tile_engine(Verilated::threadContextp(), _vcname__)
{
}

//============================================================
// Destructor

Vtest_tile_engine::~Vtest_tile_engine() {
    delete vlSymsp;
}

//============================================================
// Evaluation function

#ifdef VL_DEBUG
void Vtest_tile_engine___024root___eval_debug_assertions(Vtest_tile_engine___024root* vlSelf);
#endif  // VL_DEBUG
void Vtest_tile_engine___024root___eval_static(Vtest_tile_engine___024root* vlSelf);
void Vtest_tile_engine___024root___eval_initial(Vtest_tile_engine___024root* vlSelf);
void Vtest_tile_engine___024root___eval_settle(Vtest_tile_engine___024root* vlSelf);
void Vtest_tile_engine___024root___eval(Vtest_tile_engine___024root* vlSelf);

void Vtest_tile_engine::eval_step() {
    VL_DEBUG_IF(VL_DBG_MSGF("+++++TOP Evaluate Vtest_tile_engine::eval_step\n"); );
#ifdef VL_DEBUG
    // Debug assertions
    Vtest_tile_engine___024root___eval_debug_assertions(&(vlSymsp->TOP));
#endif  // VL_DEBUG
    vlSymsp->__Vm_deleter.deleteAll();
    if (VL_UNLIKELY(!vlSymsp->__Vm_didInit)) {
        VL_DEBUG_IF(VL_DBG_MSGF("+ Initial\n"););
        Vtest_tile_engine___024root___eval_static(&(vlSymsp->TOP));
        Vtest_tile_engine___024root___eval_initial(&(vlSymsp->TOP));
        Vtest_tile_engine___024root___eval_settle(&(vlSymsp->TOP));
        vlSymsp->__Vm_didInit = true;
    }
    VL_DEBUG_IF(VL_DBG_MSGF("+ Eval\n"););
    Vtest_tile_engine___024root___eval(&(vlSymsp->TOP));
    // Evaluate cleanup
    Verilated::endOfEval(vlSymsp->__Vm_evalMsgQp);
}

//============================================================
// Events and timing
bool Vtest_tile_engine::eventsPending() { return !vlSymsp->TOP.__VdlySched.empty() && !contextp()->gotFinish(); }

uint64_t Vtest_tile_engine::nextTimeSlot() { return vlSymsp->TOP.__VdlySched.nextTimeSlot(); }

//============================================================
// Utilities

const char* Vtest_tile_engine::name() const {
    return vlSymsp->name();
}

//============================================================
// Invoke final blocks

void Vtest_tile_engine___024root___eval_final(Vtest_tile_engine___024root* vlSelf);

VL_ATTR_COLD void Vtest_tile_engine::final() {
    Vtest_tile_engine___024root___eval_final(&(vlSymsp->TOP));
}

//============================================================
// Implementations of abstract methods from VerilatedModel

const char* Vtest_tile_engine::hierName() const { return vlSymsp->name(); }
const char* Vtest_tile_engine::modelName() const { return "Vtest_tile_engine"; }
unsigned Vtest_tile_engine::threads() const { return 1; }
void Vtest_tile_engine::prepareClone() const { contextp()->prepareClone(); }
void Vtest_tile_engine::atClone() const {
    contextp()->threadPoolpOnClone();
}
