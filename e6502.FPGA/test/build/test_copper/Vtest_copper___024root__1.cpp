// Verilated -*- C++ -*-
// DESCRIPTION: Verilator output: Design implementation internals
// See Vtest_copper.h for the primary calling header

#include "Vtest_copper__pch.h"

void Vtest_copper___024root___nba_sequent__TOP__0(Vtest_copper___024root* vlSelf);

void Vtest_copper___024root___eval_nba(Vtest_copper___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vtest_copper___024root___eval_nba\n"); );
    Vtest_copper__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
    // Body
    if ((1ULL & vlSelfRef.__VnbaTriggered[0U])) {
        Vtest_copper___024root___nba_sequent__TOP__0(vlSelf);
    }
}

void Vtest_copper___024root___timing_ready(Vtest_copper___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vtest_copper___024root___timing_ready\n"); );
    Vtest_copper__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
    // Body
    if ((1ULL & vlSelfRef.__VactTriggered[0U])) {
        vlSelfRef.__VtrigSched_hbd4d80cf__0.ready("@(posedge test_copper.clk)");
    }
}

void Vtest_copper___024root___timing_resume(Vtest_copper___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vtest_copper___024root___timing_resume\n"); );
    Vtest_copper__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
    // Body
    vlSelfRef.__VtrigSched_hbd4d80cf__0.moveToResumeQueue(
                                                          "@(posedge test_copper.clk)");
    vlSelfRef.__VtrigSched_hbd4d80cf__0.resume("@(posedge test_copper.clk)");
    if ((2ULL & vlSelfRef.__VactTriggered[0U])) {
        vlSelfRef.__VdlySched.resume();
    }
}

void Vtest_copper___024root___trigger_orInto__act_vec_vec(VlUnpacked<QData/*63:0*/, 1> &out, const VlUnpacked<QData/*63:0*/, 1> &in) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vtest_copper___024root___trigger_orInto__act_vec_vec\n"); );
    // Locals
    IData/*31:0*/ n;
    // Body
    n = 0U;
    do {
        out[n] = (out[n] | in[n]);
        n = ((IData)(1U) + n);
    } while ((0U >= n));
}

void Vtest_copper___024root___eval_triggers_vec__act(Vtest_copper___024root* vlSelf);
#ifdef VL_DEBUG
VL_ATTR_COLD void Vtest_copper___024root___dump_triggers__act(const VlUnpacked<QData/*63:0*/, 1> &triggers, const std::string &tag);
#endif  // VL_DEBUG
bool Vtest_copper___024root___trigger_anySet__act(const VlUnpacked<QData/*63:0*/, 1> &in);
void Vtest_copper___024root___eval_act(Vtest_copper___024root* vlSelf);

bool Vtest_copper___024root___eval_phase__act(Vtest_copper___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vtest_copper___024root___eval_phase__act\n"); );
    Vtest_copper__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
    // Locals
    CData/*0:0*/ __VactExecute;
    // Body
    Vtest_copper___024root___eval_triggers_vec__act(vlSelf);
    Vtest_copper___024root___timing_ready(vlSelf);
    Vtest_copper___024root___trigger_orInto__act_vec_vec(vlSelfRef.__VactTriggered, vlSelfRef.__VactTriggeredAcc);
#ifdef VL_DEBUG
    if (VL_UNLIKELY(vlSymsp->_vm_contextp__->debug())) {
        Vtest_copper___024root___dump_triggers__act(vlSelfRef.__VactTriggered, "act"s);
    }
#endif
    Vtest_copper___024root___trigger_orInto__act_vec_vec(vlSelfRef.__VnbaTriggered, vlSelfRef.__VactTriggered);
    __VactExecute = Vtest_copper___024root___trigger_anySet__act(vlSelfRef.__VactTriggered);
    if (__VactExecute) {
        vlSelfRef.__VactTriggeredAcc.fill(0ULL);
        Vtest_copper___024root___timing_resume(vlSelf);
        Vtest_copper___024root___eval_act(vlSelf);
    }
    return (__VactExecute);
}

bool Vtest_copper___024root___eval_phase__inact(Vtest_copper___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vtest_copper___024root___eval_phase__inact\n"); );
    Vtest_copper__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
    // Locals
    CData/*0:0*/ __VinactExecute;
    // Body
    __VinactExecute = vlSelfRef.__VdlySched.awaitingZeroDelay();
    if (__VinactExecute) {
        VL_FATAL_MT("test_copper.sv", 6, "", "ZERODLY: Design Verilated with '--no-sched-zero-delay', but #0 delay executed at runtime");
    }
    return (__VinactExecute);
}

void Vtest_copper___024root___trigger_clear__act(VlUnpacked<QData/*63:0*/, 1> &out) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vtest_copper___024root___trigger_clear__act\n"); );
    // Locals
    IData/*31:0*/ n;
    // Body
    n = 0U;
    do {
        out[n] = 0ULL;
        n = ((IData)(1U) + n);
    } while ((1U > n));
}

bool Vtest_copper___024root___eval_phase__nba(Vtest_copper___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vtest_copper___024root___eval_phase__nba\n"); );
    Vtest_copper__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
    // Locals
    CData/*0:0*/ __VnbaExecute;
    // Body
    __VnbaExecute = Vtest_copper___024root___trigger_anySet__act(vlSelfRef.__VnbaTriggered);
    if (__VnbaExecute) {
        Vtest_copper___024root___eval_nba(vlSelf);
        Vtest_copper___024root___trigger_clear__act(vlSelfRef.__VnbaTriggered);
    }
    return (__VnbaExecute);
}

void Vtest_copper___024root___eval(Vtest_copper___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vtest_copper___024root___eval\n"); );
    Vtest_copper__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
    // Locals
    IData/*31:0*/ __VnbaIterCount;
    // Body
    __VnbaIterCount = 0U;
    do {
        if (VL_UNLIKELY(((0x00000064U < __VnbaIterCount)))) {
#ifdef VL_DEBUG
            Vtest_copper___024root___dump_triggers__act(vlSelfRef.__VnbaTriggered, "nba"s);
#endif
            VL_FATAL_MT("test_copper.sv", 6, "", "DIDNOTCONVERGE: NBA region did not converge after '--converge-limit' of 100 tries");
        }
        __VnbaIterCount = ((IData)(1U) + __VnbaIterCount);
        vlSelfRef.__VinactIterCount = 0U;
        do {
            if (VL_UNLIKELY(((0x00000064U < vlSelfRef.__VinactIterCount)))) {
                VL_FATAL_MT("test_copper.sv", 6, "", "DIDNOTCONVERGE: Inactive region did not converge after '--converge-limit' of 100 tries");
            }
            vlSelfRef.__VinactIterCount = ((IData)(1U) 
                                           + vlSelfRef.__VinactIterCount);
            vlSelfRef.__VactIterCount = 0U;
            do {
                if (VL_UNLIKELY(((0x00000064U < vlSelfRef.__VactIterCount)))) {
#ifdef VL_DEBUG
                    Vtest_copper___024root___dump_triggers__act(vlSelfRef.__VactTriggered, "act"s);
#endif
                    VL_FATAL_MT("test_copper.sv", 6, "", "DIDNOTCONVERGE: Active region did not converge after '--converge-limit' of 100 tries");
                }
                vlSelfRef.__VactIterCount = ((IData)(1U) 
                                             + vlSelfRef.__VactIterCount);
                vlSelfRef.__VactPhaseResult = Vtest_copper___024root___eval_phase__act(vlSelf);
            } while (vlSelfRef.__VactPhaseResult);
            vlSelfRef.__VinactPhaseResult = Vtest_copper___024root___eval_phase__inact(vlSelf);
        } while (vlSelfRef.__VinactPhaseResult);
        vlSelfRef.__VnbaPhaseResult = Vtest_copper___024root___eval_phase__nba(vlSelf);
    } while (vlSelfRef.__VnbaPhaseResult);
}

void Vtest_copper___024root____VbeforeTrig_hbd4d80cf__0(Vtest_copper___024root* vlSelf, const char* __VeventDescription) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vtest_copper___024root____VbeforeTrig_hbd4d80cf__0\n"); );
    Vtest_copper__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
    // Locals
    VlUnpacked<QData/*63:0*/, 1> __VTmp;
    // Body
    __VTmp[0U] = (QData)((IData)(((IData)(vlSelfRef.test_copper__DOT__clk) 
                                  & (~ (IData)(vlSelfRef.__Vtrigprevexpr___TOP__test_copper__DOT__clk__0)))));
    vlSelfRef.__Vtrigprevexpr___TOP__test_copper__DOT__clk__0 
        = vlSelfRef.test_copper__DOT__clk;
    if ((1ULL & __VTmp[0U])) {
        vlSelfRef.__VtrigSched_hbd4d80cf__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_hbd4d80cf__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_hbd4d80cf__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_hbd4d80cf__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_hbd4d80cf__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_hbd4d80cf__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_hbd4d80cf__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_hbd4d80cf__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_hbd4d80cf__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_hbd4d80cf__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_hbd4d80cf__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_hbd4d80cf__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_hbd4d80cf__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_hbd4d80cf__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_hbd4d80cf__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_hbd4d80cf__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_hbd4d80cf__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_hbd4d80cf__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_hbd4d80cf__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_hbd4d80cf__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_hbd4d80cf__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_hbd4d80cf__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_hbd4d80cf__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_hbd4d80cf__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_hbd4d80cf__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_hbd4d80cf__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_hbd4d80cf__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_hbd4d80cf__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_hbd4d80cf__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_hbd4d80cf__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_hbd4d80cf__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_hbd4d80cf__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_hbd4d80cf__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_hbd4d80cf__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_hbd4d80cf__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_hbd4d80cf__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_hbd4d80cf__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_hbd4d80cf__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_hbd4d80cf__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_hbd4d80cf__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_hbd4d80cf__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_hbd4d80cf__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_hbd4d80cf__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_hbd4d80cf__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_hbd4d80cf__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_hbd4d80cf__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_hbd4d80cf__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_hbd4d80cf__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_hbd4d80cf__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_hbd4d80cf__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_hbd4d80cf__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_hbd4d80cf__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_hbd4d80cf__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_hbd4d80cf__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_hbd4d80cf__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_hbd4d80cf__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_hbd4d80cf__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_hbd4d80cf__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_hbd4d80cf__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_hbd4d80cf__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_hbd4d80cf__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_hbd4d80cf__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_hbd4d80cf__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_hbd4d80cf__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_hbd4d80cf__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_hbd4d80cf__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_hbd4d80cf__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_hbd4d80cf__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_hbd4d80cf__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_hbd4d80cf__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_hbd4d80cf__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_hbd4d80cf__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_hbd4d80cf__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_hbd4d80cf__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_hbd4d80cf__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_hbd4d80cf__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_hbd4d80cf__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_hbd4d80cf__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_hbd4d80cf__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_hbd4d80cf__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_hbd4d80cf__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_hbd4d80cf__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_hbd4d80cf__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_hbd4d80cf__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_hbd4d80cf__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_hbd4d80cf__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_hbd4d80cf__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_hbd4d80cf__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_hbd4d80cf__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_hbd4d80cf__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_hbd4d80cf__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_hbd4d80cf__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_hbd4d80cf__0.ready(__VeventDescription);
    }
    vlSelfRef.__VactTriggeredAcc[0U] = (vlSelfRef.__VactTriggeredAcc[0U] 
                                        | __VTmp[0U]);
}

#ifdef VL_DEBUG
void Vtest_copper___024root___eval_debug_assertions(Vtest_copper___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vtest_copper___024root___eval_debug_assertions\n"); );
    Vtest_copper__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
}
#endif  // VL_DEBUG
