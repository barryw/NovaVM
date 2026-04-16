// Verilated -*- C++ -*-
// DESCRIPTION: Verilator output: Design implementation internals
// See Vtest_copper_lists.h for the primary calling header

#include "Vtest_copper_lists__pch.h"

VL_ATTR_COLD void Vtest_copper_lists___024root___eval_initial__TOP(Vtest_copper_lists___024root* vlSelf);
VlCoroutine Vtest_copper_lists___024root___eval_initial__TOP__Vtiming__0(Vtest_copper_lists___024root* vlSelf);
VlCoroutine Vtest_copper_lists___024root___eval_initial__TOP__Vtiming__1(Vtest_copper_lists___024root* vlSelf);

void Vtest_copper_lists___024root___eval_initial(Vtest_copper_lists___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vtest_copper_lists___024root___eval_initial\n"); );
    Vtest_copper_lists__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
    // Body
    Vtest_copper_lists___024root___eval_initial__TOP(vlSelf);
    Vtest_copper_lists___024root___eval_initial__TOP__Vtiming__0(vlSelf);
    Vtest_copper_lists___024root___eval_initial__TOP__Vtiming__1(vlSelf);
}

void Vtest_copper_lists___024root____VbeforeTrig_h76281f29__0(Vtest_copper_lists___024root* vlSelf, const char* __VeventDescription);

VlCoroutine Vtest_copper_lists___024root___eval_initial__TOP__Vtiming__0(Vtest_copper_lists___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vtest_copper_lists___024root___eval_initial__TOP__Vtiming__0\n"); );
    Vtest_copper_lists__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
    // Locals
    IData/*31:0*/ __Vtask_test_copper_lists__DOT__run_clocks__0__test_copper_lists__DOT__unnamedblk1_1__DOT____Vrepeat0;
    __Vtask_test_copper_lists__DOT__run_clocks__0__test_copper_lists__DOT__unnamedblk1_1__DOT____Vrepeat0 = 0;
    IData/*31:0*/ __Vtask_test_copper_lists__DOT__run_clocks__1__n;
    __Vtask_test_copper_lists__DOT__run_clocks__1__n = 0;
    IData/*31:0*/ __Vtask_test_copper_lists__DOT__run_clocks__1__test_copper_lists__DOT__unnamedblk1_1__DOT____Vrepeat0;
    __Vtask_test_copper_lists__DOT__run_clocks__1__test_copper_lists__DOT__unnamedblk1_1__DOT____Vrepeat0 = 0;
    CData/*0:0*/ __Vtask_test_copper_lists__DOT__check__2__condition;
    __Vtask_test_copper_lists__DOT__check__2__condition = 0;
    CData/*0:0*/ __Vtask_test_copper_lists__DOT__check__3__condition;
    __Vtask_test_copper_lists__DOT__check__3__condition = 0;
    CData/*0:0*/ __Vtask_test_copper_lists__DOT__check__4__condition;
    __Vtask_test_copper_lists__DOT__check__4__condition = 0;
    IData/*31:0*/ __Vtask_test_copper_lists__DOT__copper_list__5__idx;
    __Vtask_test_copper_lists__DOT__copper_list__5__idx = 0;
    IData/*31:0*/ __Vtask_test_copper_lists__DOT__write_param__6__idx;
    __Vtask_test_copper_lists__DOT__write_param__6__idx = 0;
    CData/*7:0*/ __Vtask_test_copper_lists__DOT__write_param__6__data;
    __Vtask_test_copper_lists__DOT__write_param__6__data = 0;
    SData/*15:0*/ __Vtask_test_copper_lists__DOT__write_reg__7__addr;
    __Vtask_test_copper_lists__DOT__write_reg__7__addr = 0;
    CData/*7:0*/ __Vtask_test_copper_lists__DOT__write_reg__7__data;
    __Vtask_test_copper_lists__DOT__write_reg__7__data = 0;
    CData/*7:0*/ __Vtask_test_copper_lists__DOT__write_cmd__8__cmd;
    __Vtask_test_copper_lists__DOT__write_cmd__8__cmd = 0;
    SData/*15:0*/ __Vtask_test_copper_lists__DOT__write_reg__9__addr;
    __Vtask_test_copper_lists__DOT__write_reg__9__addr = 0;
    CData/*7:0*/ __Vtask_test_copper_lists__DOT__write_reg__9__data;
    __Vtask_test_copper_lists__DOT__write_reg__9__data = 0;
    IData/*31:0*/ __Vtask_test_copper_lists__DOT__run_clocks__10__n;
    __Vtask_test_copper_lists__DOT__run_clocks__10__n = 0;
    IData/*31:0*/ __Vtask_test_copper_lists__DOT__run_clocks__10__test_copper_lists__DOT__unnamedblk1_1__DOT____Vrepeat0;
    __Vtask_test_copper_lists__DOT__run_clocks__10__test_copper_lists__DOT__unnamedblk1_1__DOT____Vrepeat0 = 0;
    CData/*0:0*/ __Vtask_test_copper_lists__DOT__check__11__condition;
    __Vtask_test_copper_lists__DOT__check__11__condition = 0;
    CData/*0:0*/ __Vtask_test_copper_lists__DOT__check__12__condition;
    __Vtask_test_copper_lists__DOT__check__12__condition = 0;
    IData/*31:0*/ __Vtask_test_copper_lists__DOT__copper_add__13__x;
    __Vtask_test_copper_lists__DOT__copper_add__13__x = 0;
    IData/*31:0*/ __Vtask_test_copper_lists__DOT__copper_add__13__y;
    __Vtask_test_copper_lists__DOT__copper_add__13__y = 0;
    IData/*31:0*/ __Vtask_test_copper_lists__DOT__copper_add__13__reg_idx;
    __Vtask_test_copper_lists__DOT__copper_add__13__reg_idx = 0;
    CData/*7:0*/ __Vtask_test_copper_lists__DOT__copper_add__13__value;
    __Vtask_test_copper_lists__DOT__copper_add__13__value = 0;
    IData/*31:0*/ __Vtask_test_copper_lists__DOT__write_param__14__idx;
    __Vtask_test_copper_lists__DOT__write_param__14__idx = 0;
    CData/*7:0*/ __Vtask_test_copper_lists__DOT__write_param__14__data;
    __Vtask_test_copper_lists__DOT__write_param__14__data = 0;
    SData/*15:0*/ __Vtask_test_copper_lists__DOT__write_reg__15__addr;
    __Vtask_test_copper_lists__DOT__write_reg__15__addr = 0;
    CData/*7:0*/ __Vtask_test_copper_lists__DOT__write_reg__15__data;
    __Vtask_test_copper_lists__DOT__write_reg__15__data = 0;
    IData/*31:0*/ __Vtask_test_copper_lists__DOT__write_param__16__idx;
    __Vtask_test_copper_lists__DOT__write_param__16__idx = 0;
    CData/*7:0*/ __Vtask_test_copper_lists__DOT__write_param__16__data;
    __Vtask_test_copper_lists__DOT__write_param__16__data = 0;
    SData/*15:0*/ __Vtask_test_copper_lists__DOT__write_reg__17__addr;
    __Vtask_test_copper_lists__DOT__write_reg__17__addr = 0;
    CData/*7:0*/ __Vtask_test_copper_lists__DOT__write_reg__17__data;
    __Vtask_test_copper_lists__DOT__write_reg__17__data = 0;
    IData/*31:0*/ __Vtask_test_copper_lists__DOT__write_param__18__idx;
    __Vtask_test_copper_lists__DOT__write_param__18__idx = 0;
    CData/*7:0*/ __Vtask_test_copper_lists__DOT__write_param__18__data;
    __Vtask_test_copper_lists__DOT__write_param__18__data = 0;
    SData/*15:0*/ __Vtask_test_copper_lists__DOT__write_reg__19__addr;
    __Vtask_test_copper_lists__DOT__write_reg__19__addr = 0;
    CData/*7:0*/ __Vtask_test_copper_lists__DOT__write_reg__19__data;
    __Vtask_test_copper_lists__DOT__write_reg__19__data = 0;
    IData/*31:0*/ __Vtask_test_copper_lists__DOT__write_param__20__idx;
    __Vtask_test_copper_lists__DOT__write_param__20__idx = 0;
    CData/*7:0*/ __Vtask_test_copper_lists__DOT__write_param__20__data;
    __Vtask_test_copper_lists__DOT__write_param__20__data = 0;
    SData/*15:0*/ __Vtask_test_copper_lists__DOT__write_reg__21__addr;
    __Vtask_test_copper_lists__DOT__write_reg__21__addr = 0;
    CData/*7:0*/ __Vtask_test_copper_lists__DOT__write_reg__21__data;
    __Vtask_test_copper_lists__DOT__write_reg__21__data = 0;
    IData/*31:0*/ __Vtask_test_copper_lists__DOT__write_param__22__idx;
    __Vtask_test_copper_lists__DOT__write_param__22__idx = 0;
    CData/*7:0*/ __Vtask_test_copper_lists__DOT__write_param__22__data;
    __Vtask_test_copper_lists__DOT__write_param__22__data = 0;
    SData/*15:0*/ __Vtask_test_copper_lists__DOT__write_reg__23__addr;
    __Vtask_test_copper_lists__DOT__write_reg__23__addr = 0;
    CData/*7:0*/ __Vtask_test_copper_lists__DOT__write_reg__23__data;
    __Vtask_test_copper_lists__DOT__write_reg__23__data = 0;
    IData/*31:0*/ __Vtask_test_copper_lists__DOT__write_param__24__idx;
    __Vtask_test_copper_lists__DOT__write_param__24__idx = 0;
    CData/*7:0*/ __Vtask_test_copper_lists__DOT__write_param__24__data;
    __Vtask_test_copper_lists__DOT__write_param__24__data = 0;
    SData/*15:0*/ __Vtask_test_copper_lists__DOT__write_reg__25__addr;
    __Vtask_test_copper_lists__DOT__write_reg__25__addr = 0;
    CData/*7:0*/ __Vtask_test_copper_lists__DOT__write_reg__25__data;
    __Vtask_test_copper_lists__DOT__write_reg__25__data = 0;
    CData/*7:0*/ __Vtask_test_copper_lists__DOT__write_cmd__26__cmd;
    __Vtask_test_copper_lists__DOT__write_cmd__26__cmd = 0;
    SData/*15:0*/ __Vtask_test_copper_lists__DOT__write_reg__27__addr;
    __Vtask_test_copper_lists__DOT__write_reg__27__addr = 0;
    CData/*7:0*/ __Vtask_test_copper_lists__DOT__write_reg__27__data;
    __Vtask_test_copper_lists__DOT__write_reg__27__data = 0;
    IData/*31:0*/ __Vtask_test_copper_lists__DOT__run_clocks__28__n;
    __Vtask_test_copper_lists__DOT__run_clocks__28__n = 0;
    IData/*31:0*/ __Vtask_test_copper_lists__DOT__run_clocks__28__test_copper_lists__DOT__unnamedblk1_1__DOT____Vrepeat0;
    __Vtask_test_copper_lists__DOT__run_clocks__28__test_copper_lists__DOT__unnamedblk1_1__DOT____Vrepeat0 = 0;
    CData/*0:0*/ __Vtask_test_copper_lists__DOT__check__29__condition;
    __Vtask_test_copper_lists__DOT__check__29__condition = 0;
    CData/*0:0*/ __Vtask_test_copper_lists__DOT__check__30__condition;
    __Vtask_test_copper_lists__DOT__check__30__condition = 0;
    IData/*31:0*/ __Vtask_test_copper_lists__DOT__copper_list__31__idx;
    __Vtask_test_copper_lists__DOT__copper_list__31__idx = 0;
    IData/*31:0*/ __Vtask_test_copper_lists__DOT__write_param__32__idx;
    __Vtask_test_copper_lists__DOT__write_param__32__idx = 0;
    CData/*7:0*/ __Vtask_test_copper_lists__DOT__write_param__32__data;
    __Vtask_test_copper_lists__DOT__write_param__32__data = 0;
    SData/*15:0*/ __Vtask_test_copper_lists__DOT__write_reg__33__addr;
    __Vtask_test_copper_lists__DOT__write_reg__33__addr = 0;
    CData/*7:0*/ __Vtask_test_copper_lists__DOT__write_reg__33__data;
    __Vtask_test_copper_lists__DOT__write_reg__33__data = 0;
    CData/*7:0*/ __Vtask_test_copper_lists__DOT__write_cmd__34__cmd;
    __Vtask_test_copper_lists__DOT__write_cmd__34__cmd = 0;
    SData/*15:0*/ __Vtask_test_copper_lists__DOT__write_reg__35__addr;
    __Vtask_test_copper_lists__DOT__write_reg__35__addr = 0;
    CData/*7:0*/ __Vtask_test_copper_lists__DOT__write_reg__35__data;
    __Vtask_test_copper_lists__DOT__write_reg__35__data = 0;
    IData/*31:0*/ __Vtask_test_copper_lists__DOT__run_clocks__36__n;
    __Vtask_test_copper_lists__DOT__run_clocks__36__n = 0;
    IData/*31:0*/ __Vtask_test_copper_lists__DOT__run_clocks__36__test_copper_lists__DOT__unnamedblk1_1__DOT____Vrepeat0;
    __Vtask_test_copper_lists__DOT__run_clocks__36__test_copper_lists__DOT__unnamedblk1_1__DOT____Vrepeat0 = 0;
    IData/*31:0*/ __Vtask_test_copper_lists__DOT__copper_add__37__x;
    __Vtask_test_copper_lists__DOT__copper_add__37__x = 0;
    IData/*31:0*/ __Vtask_test_copper_lists__DOT__copper_add__37__y;
    __Vtask_test_copper_lists__DOT__copper_add__37__y = 0;
    IData/*31:0*/ __Vtask_test_copper_lists__DOT__copper_add__37__reg_idx;
    __Vtask_test_copper_lists__DOT__copper_add__37__reg_idx = 0;
    CData/*7:0*/ __Vtask_test_copper_lists__DOT__copper_add__37__value;
    __Vtask_test_copper_lists__DOT__copper_add__37__value = 0;
    IData/*31:0*/ __Vtask_test_copper_lists__DOT__write_param__38__idx;
    __Vtask_test_copper_lists__DOT__write_param__38__idx = 0;
    CData/*7:0*/ __Vtask_test_copper_lists__DOT__write_param__38__data;
    __Vtask_test_copper_lists__DOT__write_param__38__data = 0;
    SData/*15:0*/ __Vtask_test_copper_lists__DOT__write_reg__39__addr;
    __Vtask_test_copper_lists__DOT__write_reg__39__addr = 0;
    CData/*7:0*/ __Vtask_test_copper_lists__DOT__write_reg__39__data;
    __Vtask_test_copper_lists__DOT__write_reg__39__data = 0;
    IData/*31:0*/ __Vtask_test_copper_lists__DOT__write_param__40__idx;
    __Vtask_test_copper_lists__DOT__write_param__40__idx = 0;
    CData/*7:0*/ __Vtask_test_copper_lists__DOT__write_param__40__data;
    __Vtask_test_copper_lists__DOT__write_param__40__data = 0;
    SData/*15:0*/ __Vtask_test_copper_lists__DOT__write_reg__41__addr;
    __Vtask_test_copper_lists__DOT__write_reg__41__addr = 0;
    CData/*7:0*/ __Vtask_test_copper_lists__DOT__write_reg__41__data;
    __Vtask_test_copper_lists__DOT__write_reg__41__data = 0;
    IData/*31:0*/ __Vtask_test_copper_lists__DOT__write_param__42__idx;
    __Vtask_test_copper_lists__DOT__write_param__42__idx = 0;
    CData/*7:0*/ __Vtask_test_copper_lists__DOT__write_param__42__data;
    __Vtask_test_copper_lists__DOT__write_param__42__data = 0;
    SData/*15:0*/ __Vtask_test_copper_lists__DOT__write_reg__43__addr;
    __Vtask_test_copper_lists__DOT__write_reg__43__addr = 0;
    CData/*7:0*/ __Vtask_test_copper_lists__DOT__write_reg__43__data;
    __Vtask_test_copper_lists__DOT__write_reg__43__data = 0;
    IData/*31:0*/ __Vtask_test_copper_lists__DOT__write_param__44__idx;
    __Vtask_test_copper_lists__DOT__write_param__44__idx = 0;
    CData/*7:0*/ __Vtask_test_copper_lists__DOT__write_param__44__data;
    __Vtask_test_copper_lists__DOT__write_param__44__data = 0;
    SData/*15:0*/ __Vtask_test_copper_lists__DOT__write_reg__45__addr;
    __Vtask_test_copper_lists__DOT__write_reg__45__addr = 0;
    CData/*7:0*/ __Vtask_test_copper_lists__DOT__write_reg__45__data;
    __Vtask_test_copper_lists__DOT__write_reg__45__data = 0;
    IData/*31:0*/ __Vtask_test_copper_lists__DOT__write_param__46__idx;
    __Vtask_test_copper_lists__DOT__write_param__46__idx = 0;
    CData/*7:0*/ __Vtask_test_copper_lists__DOT__write_param__46__data;
    __Vtask_test_copper_lists__DOT__write_param__46__data = 0;
    SData/*15:0*/ __Vtask_test_copper_lists__DOT__write_reg__47__addr;
    __Vtask_test_copper_lists__DOT__write_reg__47__addr = 0;
    CData/*7:0*/ __Vtask_test_copper_lists__DOT__write_reg__47__data;
    __Vtask_test_copper_lists__DOT__write_reg__47__data = 0;
    IData/*31:0*/ __Vtask_test_copper_lists__DOT__write_param__48__idx;
    __Vtask_test_copper_lists__DOT__write_param__48__idx = 0;
    CData/*7:0*/ __Vtask_test_copper_lists__DOT__write_param__48__data;
    __Vtask_test_copper_lists__DOT__write_param__48__data = 0;
    SData/*15:0*/ __Vtask_test_copper_lists__DOT__write_reg__49__addr;
    __Vtask_test_copper_lists__DOT__write_reg__49__addr = 0;
    CData/*7:0*/ __Vtask_test_copper_lists__DOT__write_reg__49__data;
    __Vtask_test_copper_lists__DOT__write_reg__49__data = 0;
    CData/*7:0*/ __Vtask_test_copper_lists__DOT__write_cmd__50__cmd;
    __Vtask_test_copper_lists__DOT__write_cmd__50__cmd = 0;
    SData/*15:0*/ __Vtask_test_copper_lists__DOT__write_reg__51__addr;
    __Vtask_test_copper_lists__DOT__write_reg__51__addr = 0;
    CData/*7:0*/ __Vtask_test_copper_lists__DOT__write_reg__51__data;
    __Vtask_test_copper_lists__DOT__write_reg__51__data = 0;
    IData/*31:0*/ __Vtask_test_copper_lists__DOT__run_clocks__52__n;
    __Vtask_test_copper_lists__DOT__run_clocks__52__n = 0;
    IData/*31:0*/ __Vtask_test_copper_lists__DOT__run_clocks__52__test_copper_lists__DOT__unnamedblk1_1__DOT____Vrepeat0;
    __Vtask_test_copper_lists__DOT__run_clocks__52__test_copper_lists__DOT__unnamedblk1_1__DOT____Vrepeat0 = 0;
    CData/*0:0*/ __Vtask_test_copper_lists__DOT__check__53__condition;
    __Vtask_test_copper_lists__DOT__check__53__condition = 0;
    CData/*0:0*/ __Vtask_test_copper_lists__DOT__check__54__condition;
    __Vtask_test_copper_lists__DOT__check__54__condition = 0;
    IData/*31:0*/ __Vtask_test_copper_lists__DOT__copper_use__55__idx;
    __Vtask_test_copper_lists__DOT__copper_use__55__idx = 0;
    IData/*31:0*/ __Vtask_test_copper_lists__DOT__write_param__56__idx;
    __Vtask_test_copper_lists__DOT__write_param__56__idx = 0;
    CData/*7:0*/ __Vtask_test_copper_lists__DOT__write_param__56__data;
    __Vtask_test_copper_lists__DOT__write_param__56__data = 0;
    SData/*15:0*/ __Vtask_test_copper_lists__DOT__write_reg__57__addr;
    __Vtask_test_copper_lists__DOT__write_reg__57__addr = 0;
    CData/*7:0*/ __Vtask_test_copper_lists__DOT__write_reg__57__data;
    __Vtask_test_copper_lists__DOT__write_reg__57__data = 0;
    CData/*7:0*/ __Vtask_test_copper_lists__DOT__write_cmd__58__cmd;
    __Vtask_test_copper_lists__DOT__write_cmd__58__cmd = 0;
    SData/*15:0*/ __Vtask_test_copper_lists__DOT__write_reg__59__addr;
    __Vtask_test_copper_lists__DOT__write_reg__59__addr = 0;
    CData/*7:0*/ __Vtask_test_copper_lists__DOT__write_reg__59__data;
    __Vtask_test_copper_lists__DOT__write_reg__59__data = 0;
    IData/*31:0*/ __Vtask_test_copper_lists__DOT__run_clocks__60__n;
    __Vtask_test_copper_lists__DOT__run_clocks__60__n = 0;
    IData/*31:0*/ __Vtask_test_copper_lists__DOT__run_clocks__60__test_copper_lists__DOT__unnamedblk1_1__DOT____Vrepeat0;
    __Vtask_test_copper_lists__DOT__run_clocks__60__test_copper_lists__DOT__unnamedblk1_1__DOT____Vrepeat0 = 0;
    CData/*0:0*/ __Vtask_test_copper_lists__DOT__check__61__condition;
    __Vtask_test_copper_lists__DOT__check__61__condition = 0;
    CData/*0:0*/ __Vtask_test_copper_lists__DOT__check__62__condition;
    __Vtask_test_copper_lists__DOT__check__62__condition = 0;
    CData/*7:0*/ __Vtask_test_copper_lists__DOT__write_cmd__64__cmd;
    __Vtask_test_copper_lists__DOT__write_cmd__64__cmd = 0;
    SData/*15:0*/ __Vtask_test_copper_lists__DOT__write_reg__65__addr;
    __Vtask_test_copper_lists__DOT__write_reg__65__addr = 0;
    CData/*7:0*/ __Vtask_test_copper_lists__DOT__write_reg__65__data;
    __Vtask_test_copper_lists__DOT__write_reg__65__data = 0;
    IData/*31:0*/ __Vtask_test_copper_lists__DOT__run_clocks__66__n;
    __Vtask_test_copper_lists__DOT__run_clocks__66__n = 0;
    IData/*31:0*/ __Vtask_test_copper_lists__DOT__run_clocks__66__test_copper_lists__DOT__unnamedblk1_1__DOT____Vrepeat0;
    __Vtask_test_copper_lists__DOT__run_clocks__66__test_copper_lists__DOT__unnamedblk1_1__DOT____Vrepeat0 = 0;
    SData/*15:0*/ __Vtask_test_copper_lists__DOT__write_reg__67__addr;
    __Vtask_test_copper_lists__DOT__write_reg__67__addr = 0;
    CData/*7:0*/ __Vtask_test_copper_lists__DOT__write_reg__67__data;
    __Vtask_test_copper_lists__DOT__write_reg__67__data = 0;
    IData/*31:0*/ __Vtask_test_copper_lists__DOT__run_clocks__68__n;
    __Vtask_test_copper_lists__DOT__run_clocks__68__n = 0;
    IData/*31:0*/ __Vtask_test_copper_lists__DOT__run_clocks__68__test_copper_lists__DOT__unnamedblk1_1__DOT____Vrepeat0;
    __Vtask_test_copper_lists__DOT__run_clocks__68__test_copper_lists__DOT__unnamedblk1_1__DOT____Vrepeat0 = 0;
    IData/*31:0*/ __Vtask_test_copper_lists__DOT__run_to_scanline__69__target;
    __Vtask_test_copper_lists__DOT__run_to_scanline__69__target = 0;
    IData/*31:0*/ __Vtask_test_copper_lists__DOT__run_clocks__70__n;
    __Vtask_test_copper_lists__DOT__run_clocks__70__n = 0;
    IData/*31:0*/ __Vtask_test_copper_lists__DOT__run_clocks__70__test_copper_lists__DOT__unnamedblk1_1__DOT____Vrepeat0;
    __Vtask_test_copper_lists__DOT__run_clocks__70__test_copper_lists__DOT__unnamedblk1_1__DOT____Vrepeat0 = 0;
    CData/*0:0*/ __Vtask_test_copper_lists__DOT__check__71__condition;
    __Vtask_test_copper_lists__DOT__check__71__condition = 0;
    IData/*31:0*/ __Vtask_test_copper_lists__DOT__run_clocks__72__n;
    __Vtask_test_copper_lists__DOT__run_clocks__72__n = 0;
    IData/*31:0*/ __Vtask_test_copper_lists__DOT__run_clocks__72__test_copper_lists__DOT__unnamedblk1_1__DOT____Vrepeat0;
    __Vtask_test_copper_lists__DOT__run_clocks__72__test_copper_lists__DOT__unnamedblk1_1__DOT____Vrepeat0 = 0;
    CData/*0:0*/ __Vtask_test_copper_lists__DOT__check__73__condition;
    __Vtask_test_copper_lists__DOT__check__73__condition = 0;
    SData/*15:0*/ __Vtask_test_copper_lists__DOT__write_reg__74__addr;
    __Vtask_test_copper_lists__DOT__write_reg__74__addr = 0;
    CData/*7:0*/ __Vtask_test_copper_lists__DOT__write_reg__74__data;
    __Vtask_test_copper_lists__DOT__write_reg__74__data = 0;
    IData/*31:0*/ __Vtask_test_copper_lists__DOT__run_clocks__75__n;
    __Vtask_test_copper_lists__DOT__run_clocks__75__n = 0;
    IData/*31:0*/ __Vtask_test_copper_lists__DOT__run_clocks__75__test_copper_lists__DOT__unnamedblk1_1__DOT____Vrepeat0;
    __Vtask_test_copper_lists__DOT__run_clocks__75__test_copper_lists__DOT__unnamedblk1_1__DOT____Vrepeat0 = 0;
    IData/*31:0*/ __Vtask_test_copper_lists__DOT__run_to_scanline__76__target;
    __Vtask_test_copper_lists__DOT__run_to_scanline__76__target = 0;
    IData/*31:0*/ __Vtask_test_copper_lists__DOT__run_to_scanline__77__target;
    __Vtask_test_copper_lists__DOT__run_to_scanline__77__target = 0;
    CData/*0:0*/ __Vtask_test_copper_lists__DOT__check__78__condition;
    __Vtask_test_copper_lists__DOT__check__78__condition = 0;
    IData/*31:0*/ __Vtask_test_copper_lists__DOT__copper_list__79__idx;
    __Vtask_test_copper_lists__DOT__copper_list__79__idx = 0;
    IData/*31:0*/ __Vtask_test_copper_lists__DOT__write_param__80__idx;
    __Vtask_test_copper_lists__DOT__write_param__80__idx = 0;
    CData/*7:0*/ __Vtask_test_copper_lists__DOT__write_param__80__data;
    __Vtask_test_copper_lists__DOT__write_param__80__data = 0;
    SData/*15:0*/ __Vtask_test_copper_lists__DOT__write_reg__81__addr;
    __Vtask_test_copper_lists__DOT__write_reg__81__addr = 0;
    CData/*7:0*/ __Vtask_test_copper_lists__DOT__write_reg__81__data;
    __Vtask_test_copper_lists__DOT__write_reg__81__data = 0;
    CData/*7:0*/ __Vtask_test_copper_lists__DOT__write_cmd__82__cmd;
    __Vtask_test_copper_lists__DOT__write_cmd__82__cmd = 0;
    SData/*15:0*/ __Vtask_test_copper_lists__DOT__write_reg__83__addr;
    __Vtask_test_copper_lists__DOT__write_reg__83__addr = 0;
    CData/*7:0*/ __Vtask_test_copper_lists__DOT__write_reg__83__data;
    __Vtask_test_copper_lists__DOT__write_reg__83__data = 0;
    IData/*31:0*/ __Vtask_test_copper_lists__DOT__run_clocks__84__n;
    __Vtask_test_copper_lists__DOT__run_clocks__84__n = 0;
    IData/*31:0*/ __Vtask_test_copper_lists__DOT__run_clocks__84__test_copper_lists__DOT__unnamedblk1_1__DOT____Vrepeat0;
    __Vtask_test_copper_lists__DOT__run_clocks__84__test_copper_lists__DOT__unnamedblk1_1__DOT____Vrepeat0 = 0;
    CData/*0:0*/ __Vtask_test_copper_lists__DOT__check__85__condition;
    __Vtask_test_copper_lists__DOT__check__85__condition = 0;
    CData/*7:0*/ __Vtask_test_copper_lists__DOT__write_cmd__87__cmd;
    __Vtask_test_copper_lists__DOT__write_cmd__87__cmd = 0;
    SData/*15:0*/ __Vtask_test_copper_lists__DOT__write_reg__88__addr;
    __Vtask_test_copper_lists__DOT__write_reg__88__addr = 0;
    CData/*7:0*/ __Vtask_test_copper_lists__DOT__write_reg__88__data;
    __Vtask_test_copper_lists__DOT__write_reg__88__data = 0;
    IData/*31:0*/ __Vtask_test_copper_lists__DOT__run_clocks__89__n;
    __Vtask_test_copper_lists__DOT__run_clocks__89__n = 0;
    IData/*31:0*/ __Vtask_test_copper_lists__DOT__run_clocks__89__test_copper_lists__DOT__unnamedblk1_1__DOT____Vrepeat0;
    __Vtask_test_copper_lists__DOT__run_clocks__89__test_copper_lists__DOT__unnamedblk1_1__DOT____Vrepeat0 = 0;
    CData/*0:0*/ __Vtask_test_copper_lists__DOT__check__90__condition;
    __Vtask_test_copper_lists__DOT__check__90__condition = 0;
    IData/*31:0*/ __Vtask_test_copper_lists__DOT__copper_list__91__idx;
    __Vtask_test_copper_lists__DOT__copper_list__91__idx = 0;
    IData/*31:0*/ __Vtask_test_copper_lists__DOT__write_param__92__idx;
    __Vtask_test_copper_lists__DOT__write_param__92__idx = 0;
    CData/*7:0*/ __Vtask_test_copper_lists__DOT__write_param__92__data;
    __Vtask_test_copper_lists__DOT__write_param__92__data = 0;
    SData/*15:0*/ __Vtask_test_copper_lists__DOT__write_reg__93__addr;
    __Vtask_test_copper_lists__DOT__write_reg__93__addr = 0;
    CData/*7:0*/ __Vtask_test_copper_lists__DOT__write_reg__93__data;
    __Vtask_test_copper_lists__DOT__write_reg__93__data = 0;
    CData/*7:0*/ __Vtask_test_copper_lists__DOT__write_cmd__94__cmd;
    __Vtask_test_copper_lists__DOT__write_cmd__94__cmd = 0;
    SData/*15:0*/ __Vtask_test_copper_lists__DOT__write_reg__95__addr;
    __Vtask_test_copper_lists__DOT__write_reg__95__addr = 0;
    CData/*7:0*/ __Vtask_test_copper_lists__DOT__write_reg__95__data;
    __Vtask_test_copper_lists__DOT__write_reg__95__data = 0;
    IData/*31:0*/ __Vtask_test_copper_lists__DOT__run_clocks__96__n;
    __Vtask_test_copper_lists__DOT__run_clocks__96__n = 0;
    IData/*31:0*/ __Vtask_test_copper_lists__DOT__run_clocks__96__test_copper_lists__DOT__unnamedblk1_1__DOT____Vrepeat0;
    __Vtask_test_copper_lists__DOT__run_clocks__96__test_copper_lists__DOT__unnamedblk1_1__DOT____Vrepeat0 = 0;
    CData/*0:0*/ __Vtask_test_copper_lists__DOT__check__97__condition;
    __Vtask_test_copper_lists__DOT__check__97__condition = 0;
    CData/*7:0*/ __Vtask_test_copper_lists__DOT__write_cmd__99__cmd;
    __Vtask_test_copper_lists__DOT__write_cmd__99__cmd = 0;
    SData/*15:0*/ __Vtask_test_copper_lists__DOT__write_reg__100__addr;
    __Vtask_test_copper_lists__DOT__write_reg__100__addr = 0;
    CData/*7:0*/ __Vtask_test_copper_lists__DOT__write_reg__100__data;
    __Vtask_test_copper_lists__DOT__write_reg__100__data = 0;
    IData/*31:0*/ __Vtask_test_copper_lists__DOT__run_clocks__101__n;
    __Vtask_test_copper_lists__DOT__run_clocks__101__n = 0;
    IData/*31:0*/ __Vtask_test_copper_lists__DOT__run_clocks__101__test_copper_lists__DOT__unnamedblk1_1__DOT____Vrepeat0;
    __Vtask_test_copper_lists__DOT__run_clocks__101__test_copper_lists__DOT__unnamedblk1_1__DOT____Vrepeat0 = 0;
    CData/*0:0*/ __Vtask_test_copper_lists__DOT__check__102__condition;
    __Vtask_test_copper_lists__DOT__check__102__condition = 0;
    CData/*0:0*/ __Vtask_test_copper_lists__DOT__check__103__condition;
    __Vtask_test_copper_lists__DOT__check__103__condition = 0;
    IData/*31:0*/ __Vtask_test_copper_lists__DOT__copper_list__104__idx;
    __Vtask_test_copper_lists__DOT__copper_list__104__idx = 0;
    IData/*31:0*/ __Vtask_test_copper_lists__DOT__write_param__105__idx;
    __Vtask_test_copper_lists__DOT__write_param__105__idx = 0;
    CData/*7:0*/ __Vtask_test_copper_lists__DOT__write_param__105__data;
    __Vtask_test_copper_lists__DOT__write_param__105__data = 0;
    SData/*15:0*/ __Vtask_test_copper_lists__DOT__write_reg__106__addr;
    __Vtask_test_copper_lists__DOT__write_reg__106__addr = 0;
    CData/*7:0*/ __Vtask_test_copper_lists__DOT__write_reg__106__data;
    __Vtask_test_copper_lists__DOT__write_reg__106__data = 0;
    CData/*7:0*/ __Vtask_test_copper_lists__DOT__write_cmd__107__cmd;
    __Vtask_test_copper_lists__DOT__write_cmd__107__cmd = 0;
    SData/*15:0*/ __Vtask_test_copper_lists__DOT__write_reg__108__addr;
    __Vtask_test_copper_lists__DOT__write_reg__108__addr = 0;
    CData/*7:0*/ __Vtask_test_copper_lists__DOT__write_reg__108__data;
    __Vtask_test_copper_lists__DOT__write_reg__108__data = 0;
    IData/*31:0*/ __Vtask_test_copper_lists__DOT__run_clocks__109__n;
    __Vtask_test_copper_lists__DOT__run_clocks__109__n = 0;
    IData/*31:0*/ __Vtask_test_copper_lists__DOT__run_clocks__109__test_copper_lists__DOT__unnamedblk1_1__DOT____Vrepeat0;
    __Vtask_test_copper_lists__DOT__run_clocks__109__test_copper_lists__DOT__unnamedblk1_1__DOT____Vrepeat0 = 0;
    CData/*7:0*/ __Vtask_test_copper_lists__DOT__write_cmd__111__cmd;
    __Vtask_test_copper_lists__DOT__write_cmd__111__cmd = 0;
    SData/*15:0*/ __Vtask_test_copper_lists__DOT__write_reg__112__addr;
    __Vtask_test_copper_lists__DOT__write_reg__112__addr = 0;
    CData/*7:0*/ __Vtask_test_copper_lists__DOT__write_reg__112__data;
    __Vtask_test_copper_lists__DOT__write_reg__112__data = 0;
    IData/*31:0*/ __Vtask_test_copper_lists__DOT__run_clocks__113__n;
    __Vtask_test_copper_lists__DOT__run_clocks__113__n = 0;
    IData/*31:0*/ __Vtask_test_copper_lists__DOT__run_clocks__113__test_copper_lists__DOT__unnamedblk1_1__DOT____Vrepeat0;
    __Vtask_test_copper_lists__DOT__run_clocks__113__test_copper_lists__DOT__unnamedblk1_1__DOT____Vrepeat0 = 0;
    IData/*31:0*/ __Vtask_test_copper_lists__DOT__copper_add__114__x;
    __Vtask_test_copper_lists__DOT__copper_add__114__x = 0;
    IData/*31:0*/ __Vtask_test_copper_lists__DOT__copper_add__114__y;
    __Vtask_test_copper_lists__DOT__copper_add__114__y = 0;
    IData/*31:0*/ __Vtask_test_copper_lists__DOT__copper_add__114__reg_idx;
    __Vtask_test_copper_lists__DOT__copper_add__114__reg_idx = 0;
    CData/*7:0*/ __Vtask_test_copper_lists__DOT__copper_add__114__value;
    __Vtask_test_copper_lists__DOT__copper_add__114__value = 0;
    IData/*31:0*/ __Vtask_test_copper_lists__DOT__write_param__115__idx;
    __Vtask_test_copper_lists__DOT__write_param__115__idx = 0;
    CData/*7:0*/ __Vtask_test_copper_lists__DOT__write_param__115__data;
    __Vtask_test_copper_lists__DOT__write_param__115__data = 0;
    SData/*15:0*/ __Vtask_test_copper_lists__DOT__write_reg__116__addr;
    __Vtask_test_copper_lists__DOT__write_reg__116__addr = 0;
    CData/*7:0*/ __Vtask_test_copper_lists__DOT__write_reg__116__data;
    __Vtask_test_copper_lists__DOT__write_reg__116__data = 0;
    IData/*31:0*/ __Vtask_test_copper_lists__DOT__write_param__117__idx;
    __Vtask_test_copper_lists__DOT__write_param__117__idx = 0;
    CData/*7:0*/ __Vtask_test_copper_lists__DOT__write_param__117__data;
    __Vtask_test_copper_lists__DOT__write_param__117__data = 0;
    SData/*15:0*/ __Vtask_test_copper_lists__DOT__write_reg__118__addr;
    __Vtask_test_copper_lists__DOT__write_reg__118__addr = 0;
    CData/*7:0*/ __Vtask_test_copper_lists__DOT__write_reg__118__data;
    __Vtask_test_copper_lists__DOT__write_reg__118__data = 0;
    IData/*31:0*/ __Vtask_test_copper_lists__DOT__write_param__119__idx;
    __Vtask_test_copper_lists__DOT__write_param__119__idx = 0;
    CData/*7:0*/ __Vtask_test_copper_lists__DOT__write_param__119__data;
    __Vtask_test_copper_lists__DOT__write_param__119__data = 0;
    SData/*15:0*/ __Vtask_test_copper_lists__DOT__write_reg__120__addr;
    __Vtask_test_copper_lists__DOT__write_reg__120__addr = 0;
    CData/*7:0*/ __Vtask_test_copper_lists__DOT__write_reg__120__data;
    __Vtask_test_copper_lists__DOT__write_reg__120__data = 0;
    IData/*31:0*/ __Vtask_test_copper_lists__DOT__write_param__121__idx;
    __Vtask_test_copper_lists__DOT__write_param__121__idx = 0;
    CData/*7:0*/ __Vtask_test_copper_lists__DOT__write_param__121__data;
    __Vtask_test_copper_lists__DOT__write_param__121__data = 0;
    SData/*15:0*/ __Vtask_test_copper_lists__DOT__write_reg__122__addr;
    __Vtask_test_copper_lists__DOT__write_reg__122__addr = 0;
    CData/*7:0*/ __Vtask_test_copper_lists__DOT__write_reg__122__data;
    __Vtask_test_copper_lists__DOT__write_reg__122__data = 0;
    IData/*31:0*/ __Vtask_test_copper_lists__DOT__write_param__123__idx;
    __Vtask_test_copper_lists__DOT__write_param__123__idx = 0;
    CData/*7:0*/ __Vtask_test_copper_lists__DOT__write_param__123__data;
    __Vtask_test_copper_lists__DOT__write_param__123__data = 0;
    SData/*15:0*/ __Vtask_test_copper_lists__DOT__write_reg__124__addr;
    __Vtask_test_copper_lists__DOT__write_reg__124__addr = 0;
    CData/*7:0*/ __Vtask_test_copper_lists__DOT__write_reg__124__data;
    __Vtask_test_copper_lists__DOT__write_reg__124__data = 0;
    IData/*31:0*/ __Vtask_test_copper_lists__DOT__write_param__125__idx;
    __Vtask_test_copper_lists__DOT__write_param__125__idx = 0;
    CData/*7:0*/ __Vtask_test_copper_lists__DOT__write_param__125__data;
    __Vtask_test_copper_lists__DOT__write_param__125__data = 0;
    SData/*15:0*/ __Vtask_test_copper_lists__DOT__write_reg__126__addr;
    __Vtask_test_copper_lists__DOT__write_reg__126__addr = 0;
    CData/*7:0*/ __Vtask_test_copper_lists__DOT__write_reg__126__data;
    __Vtask_test_copper_lists__DOT__write_reg__126__data = 0;
    CData/*7:0*/ __Vtask_test_copper_lists__DOT__write_cmd__127__cmd;
    __Vtask_test_copper_lists__DOT__write_cmd__127__cmd = 0;
    SData/*15:0*/ __Vtask_test_copper_lists__DOT__write_reg__128__addr;
    __Vtask_test_copper_lists__DOT__write_reg__128__addr = 0;
    CData/*7:0*/ __Vtask_test_copper_lists__DOT__write_reg__128__data;
    __Vtask_test_copper_lists__DOT__write_reg__128__data = 0;
    IData/*31:0*/ __Vtask_test_copper_lists__DOT__copper_add__129__x;
    __Vtask_test_copper_lists__DOT__copper_add__129__x = 0;
    IData/*31:0*/ __Vtask_test_copper_lists__DOT__copper_add__129__y;
    __Vtask_test_copper_lists__DOT__copper_add__129__y = 0;
    IData/*31:0*/ __Vtask_test_copper_lists__DOT__copper_add__129__reg_idx;
    __Vtask_test_copper_lists__DOT__copper_add__129__reg_idx = 0;
    CData/*7:0*/ __Vtask_test_copper_lists__DOT__copper_add__129__value;
    __Vtask_test_copper_lists__DOT__copper_add__129__value = 0;
    IData/*31:0*/ __Vtask_test_copper_lists__DOT__write_param__130__idx;
    __Vtask_test_copper_lists__DOT__write_param__130__idx = 0;
    CData/*7:0*/ __Vtask_test_copper_lists__DOT__write_param__130__data;
    __Vtask_test_copper_lists__DOT__write_param__130__data = 0;
    SData/*15:0*/ __Vtask_test_copper_lists__DOT__write_reg__131__addr;
    __Vtask_test_copper_lists__DOT__write_reg__131__addr = 0;
    CData/*7:0*/ __Vtask_test_copper_lists__DOT__write_reg__131__data;
    __Vtask_test_copper_lists__DOT__write_reg__131__data = 0;
    IData/*31:0*/ __Vtask_test_copper_lists__DOT__write_param__132__idx;
    __Vtask_test_copper_lists__DOT__write_param__132__idx = 0;
    CData/*7:0*/ __Vtask_test_copper_lists__DOT__write_param__132__data;
    __Vtask_test_copper_lists__DOT__write_param__132__data = 0;
    SData/*15:0*/ __Vtask_test_copper_lists__DOT__write_reg__133__addr;
    __Vtask_test_copper_lists__DOT__write_reg__133__addr = 0;
    CData/*7:0*/ __Vtask_test_copper_lists__DOT__write_reg__133__data;
    __Vtask_test_copper_lists__DOT__write_reg__133__data = 0;
    IData/*31:0*/ __Vtask_test_copper_lists__DOT__write_param__134__idx;
    __Vtask_test_copper_lists__DOT__write_param__134__idx = 0;
    CData/*7:0*/ __Vtask_test_copper_lists__DOT__write_param__134__data;
    __Vtask_test_copper_lists__DOT__write_param__134__data = 0;
    SData/*15:0*/ __Vtask_test_copper_lists__DOT__write_reg__135__addr;
    __Vtask_test_copper_lists__DOT__write_reg__135__addr = 0;
    CData/*7:0*/ __Vtask_test_copper_lists__DOT__write_reg__135__data;
    __Vtask_test_copper_lists__DOT__write_reg__135__data = 0;
    IData/*31:0*/ __Vtask_test_copper_lists__DOT__write_param__136__idx;
    __Vtask_test_copper_lists__DOT__write_param__136__idx = 0;
    CData/*7:0*/ __Vtask_test_copper_lists__DOT__write_param__136__data;
    __Vtask_test_copper_lists__DOT__write_param__136__data = 0;
    SData/*15:0*/ __Vtask_test_copper_lists__DOT__write_reg__137__addr;
    __Vtask_test_copper_lists__DOT__write_reg__137__addr = 0;
    CData/*7:0*/ __Vtask_test_copper_lists__DOT__write_reg__137__data;
    __Vtask_test_copper_lists__DOT__write_reg__137__data = 0;
    IData/*31:0*/ __Vtask_test_copper_lists__DOT__write_param__138__idx;
    __Vtask_test_copper_lists__DOT__write_param__138__idx = 0;
    CData/*7:0*/ __Vtask_test_copper_lists__DOT__write_param__138__data;
    __Vtask_test_copper_lists__DOT__write_param__138__data = 0;
    SData/*15:0*/ __Vtask_test_copper_lists__DOT__write_reg__139__addr;
    __Vtask_test_copper_lists__DOT__write_reg__139__addr = 0;
    CData/*7:0*/ __Vtask_test_copper_lists__DOT__write_reg__139__data;
    __Vtask_test_copper_lists__DOT__write_reg__139__data = 0;
    IData/*31:0*/ __Vtask_test_copper_lists__DOT__write_param__140__idx;
    __Vtask_test_copper_lists__DOT__write_param__140__idx = 0;
    CData/*7:0*/ __Vtask_test_copper_lists__DOT__write_param__140__data;
    __Vtask_test_copper_lists__DOT__write_param__140__data = 0;
    SData/*15:0*/ __Vtask_test_copper_lists__DOT__write_reg__141__addr;
    __Vtask_test_copper_lists__DOT__write_reg__141__addr = 0;
    CData/*7:0*/ __Vtask_test_copper_lists__DOT__write_reg__141__data;
    __Vtask_test_copper_lists__DOT__write_reg__141__data = 0;
    CData/*7:0*/ __Vtask_test_copper_lists__DOT__write_cmd__142__cmd;
    __Vtask_test_copper_lists__DOT__write_cmd__142__cmd = 0;
    SData/*15:0*/ __Vtask_test_copper_lists__DOT__write_reg__143__addr;
    __Vtask_test_copper_lists__DOT__write_reg__143__addr = 0;
    CData/*7:0*/ __Vtask_test_copper_lists__DOT__write_reg__143__data;
    __Vtask_test_copper_lists__DOT__write_reg__143__data = 0;
    IData/*31:0*/ __Vtask_test_copper_lists__DOT__run_clocks__144__n;
    __Vtask_test_copper_lists__DOT__run_clocks__144__n = 0;
    IData/*31:0*/ __Vtask_test_copper_lists__DOT__run_clocks__144__test_copper_lists__DOT__unnamedblk1_1__DOT____Vrepeat0;
    __Vtask_test_copper_lists__DOT__run_clocks__144__test_copper_lists__DOT__unnamedblk1_1__DOT____Vrepeat0 = 0;
    CData/*0:0*/ __Vtask_test_copper_lists__DOT__check__145__condition;
    __Vtask_test_copper_lists__DOT__check__145__condition = 0;
    IData/*31:0*/ __Vtask_test_copper_lists__DOT__copper_use__146__idx;
    __Vtask_test_copper_lists__DOT__copper_use__146__idx = 0;
    IData/*31:0*/ __Vtask_test_copper_lists__DOT__write_param__147__idx;
    __Vtask_test_copper_lists__DOT__write_param__147__idx = 0;
    CData/*7:0*/ __Vtask_test_copper_lists__DOT__write_param__147__data;
    __Vtask_test_copper_lists__DOT__write_param__147__data = 0;
    SData/*15:0*/ __Vtask_test_copper_lists__DOT__write_reg__148__addr;
    __Vtask_test_copper_lists__DOT__write_reg__148__addr = 0;
    CData/*7:0*/ __Vtask_test_copper_lists__DOT__write_reg__148__data;
    __Vtask_test_copper_lists__DOT__write_reg__148__data = 0;
    CData/*7:0*/ __Vtask_test_copper_lists__DOT__write_cmd__149__cmd;
    __Vtask_test_copper_lists__DOT__write_cmd__149__cmd = 0;
    SData/*15:0*/ __Vtask_test_copper_lists__DOT__write_reg__150__addr;
    __Vtask_test_copper_lists__DOT__write_reg__150__addr = 0;
    CData/*7:0*/ __Vtask_test_copper_lists__DOT__write_reg__150__data;
    __Vtask_test_copper_lists__DOT__write_reg__150__data = 0;
    IData/*31:0*/ __Vtask_test_copper_lists__DOT__run_clocks__151__n;
    __Vtask_test_copper_lists__DOT__run_clocks__151__n = 0;
    IData/*31:0*/ __Vtask_test_copper_lists__DOT__run_clocks__151__test_copper_lists__DOT__unnamedblk1_1__DOT____Vrepeat0;
    __Vtask_test_copper_lists__DOT__run_clocks__151__test_copper_lists__DOT__unnamedblk1_1__DOT____Vrepeat0 = 0;
    IData/*31:0*/ __Vtask_test_copper_lists__DOT__run_to_scanline__152__target;
    __Vtask_test_copper_lists__DOT__run_to_scanline__152__target = 0;
    IData/*31:0*/ __Vtask_test_copper_lists__DOT__run_clocks__153__n;
    __Vtask_test_copper_lists__DOT__run_clocks__153__n = 0;
    IData/*31:0*/ __Vtask_test_copper_lists__DOT__run_clocks__153__test_copper_lists__DOT__unnamedblk1_1__DOT____Vrepeat0;
    __Vtask_test_copper_lists__DOT__run_clocks__153__test_copper_lists__DOT__unnamedblk1_1__DOT____Vrepeat0 = 0;
    IData/*31:0*/ __Vtask_test_copper_lists__DOT__run_clocks__154__n;
    __Vtask_test_copper_lists__DOT__run_clocks__154__n = 0;
    IData/*31:0*/ __Vtask_test_copper_lists__DOT__run_clocks__154__test_copper_lists__DOT__unnamedblk1_1__DOT____Vrepeat0;
    __Vtask_test_copper_lists__DOT__run_clocks__154__test_copper_lists__DOT__unnamedblk1_1__DOT____Vrepeat0 = 0;
    CData/*0:0*/ __Vtask_test_copper_lists__DOT__check__155__condition;
    __Vtask_test_copper_lists__DOT__check__155__condition = 0;
    CData/*0:0*/ __Vtask_test_copper_lists__DOT__check__156__condition;
    __Vtask_test_copper_lists__DOT__check__156__condition = 0;
    SData/*15:0*/ __Vtask_test_copper_lists__DOT__write_reg__157__addr;
    __Vtask_test_copper_lists__DOT__write_reg__157__addr = 0;
    CData/*7:0*/ __Vtask_test_copper_lists__DOT__write_reg__157__data;
    __Vtask_test_copper_lists__DOT__write_reg__157__data = 0;
    IData/*31:0*/ __Vtask_test_copper_lists__DOT__run_clocks__158__n;
    __Vtask_test_copper_lists__DOT__run_clocks__158__n = 0;
    IData/*31:0*/ __Vtask_test_copper_lists__DOT__run_clocks__158__test_copper_lists__DOT__unnamedblk1_1__DOT____Vrepeat0;
    __Vtask_test_copper_lists__DOT__run_clocks__158__test_copper_lists__DOT__unnamedblk1_1__DOT____Vrepeat0 = 0;
    IData/*31:0*/ __Vtask_test_copper_lists__DOT__run_to_scanline__159__target;
    __Vtask_test_copper_lists__DOT__run_to_scanline__159__target = 0;
    IData/*31:0*/ __Vtask_test_copper_lists__DOT__run_to_scanline__160__target;
    __Vtask_test_copper_lists__DOT__run_to_scanline__160__target = 0;
    CData/*0:0*/ __Vtask_test_copper_lists__DOT__check__161__condition;
    __Vtask_test_copper_lists__DOT__check__161__condition = 0;
    IData/*31:0*/ __Vtask_test_copper_lists__DOT__run_to_scanline__162__target;
    __Vtask_test_copper_lists__DOT__run_to_scanline__162__target = 0;
    CData/*0:0*/ __Vtask_test_copper_lists__DOT__check__163__condition;
    __Vtask_test_copper_lists__DOT__check__163__condition = 0;
    CData/*7:0*/ __Vtask_test_copper_lists__DOT__write_cmd__164__cmd;
    __Vtask_test_copper_lists__DOT__write_cmd__164__cmd = 0;
    SData/*15:0*/ __Vtask_test_copper_lists__DOT__write_reg__165__addr;
    __Vtask_test_copper_lists__DOT__write_reg__165__addr = 0;
    CData/*7:0*/ __Vtask_test_copper_lists__DOT__write_reg__165__data;
    __Vtask_test_copper_lists__DOT__write_reg__165__data = 0;
    IData/*31:0*/ __Vtask_test_copper_lists__DOT__run_clocks__166__n;
    __Vtask_test_copper_lists__DOT__run_clocks__166__n = 0;
    IData/*31:0*/ __Vtask_test_copper_lists__DOT__run_clocks__166__test_copper_lists__DOT__unnamedblk1_1__DOT____Vrepeat0;
    __Vtask_test_copper_lists__DOT__run_clocks__166__test_copper_lists__DOT__unnamedblk1_1__DOT____Vrepeat0 = 0;
    CData/*0:0*/ __Vtask_test_copper_lists__DOT__check__167__condition;
    __Vtask_test_copper_lists__DOT__check__167__condition = 0;
    CData/*0:0*/ __Vtask_test_copper_lists__DOT__check__168__condition;
    __Vtask_test_copper_lists__DOT__check__168__condition = 0;
    CData/*0:0*/ __Vtask_test_copper_lists__DOT__check__169__condition;
    __Vtask_test_copper_lists__DOT__check__169__condition = 0;
    CData/*0:0*/ __Vtask_test_copper_lists__DOT__check__170__condition;
    __Vtask_test_copper_lists__DOT__check__170__condition = 0;
    // Body
    VL_WRITEF_NX("=== Advanced Copper List Unit Tests ===\n\n",0);
    vlSelfRef.test_copper_lists__DOT__rst = 1U;
    vlSelfRef.test_copper_lists__DOT__cpu_we = 0U;
    vlSelfRef.test_copper_lists__DOT__cpu_re = 0U;
    vlSelfRef.test_copper_lists__DOT__cpu_ce = 0U;
    vlSelfRef.test_copper_lists__DOT__cpu_addr = 0U;
    vlSelfRef.test_copper_lists__DOT__cpu_wdata = 0U;
    __Vtask_test_copper_lists__DOT__run_clocks__0__test_copper_lists__DOT__unnamedblk1_1__DOT____Vrepeat0 = 0x00000032U;
    while (VL_LTS_III(32, 0U, __Vtask_test_copper_lists__DOT__run_clocks__0__test_copper_lists__DOT__unnamedblk1_1__DOT____Vrepeat0)) {
        Vtest_copper_lists___024root____VbeforeTrig_h76281f29__0(vlSelf, 
                                                                 "@(posedge test_copper_lists.clk)");
        co_await vlSelfRef.__VtrigSched_h76281f29__0.trigger(0U, 
                                                             nullptr, 
                                                             "@(posedge test_copper_lists.clk)", 
                                                             "test_copper_lists.sv", 
                                                             88);
        __Vtask_test_copper_lists__DOT__run_clocks__0__test_copper_lists__DOT__unnamedblk1_1__DOT____Vrepeat0 
            = (__Vtask_test_copper_lists__DOT__run_clocks__0__test_copper_lists__DOT__unnamedblk1_1__DOT____Vrepeat0 
               - (IData)(1U));
    }
    vlSelfRef.test_copper_lists__DOT__rst = 0U;
    __Vtask_test_copper_lists__DOT__run_clocks__1__n = 0x0000000aU;
    __Vtask_test_copper_lists__DOT__run_clocks__1__test_copper_lists__DOT__unnamedblk1_1__DOT____Vrepeat0 = 0;
    __Vtask_test_copper_lists__DOT__run_clocks__1__test_copper_lists__DOT__unnamedblk1_1__DOT____Vrepeat0 
        = __Vtask_test_copper_lists__DOT__run_clocks__1__n;
    while (VL_LTS_III(32, 0U, __Vtask_test_copper_lists__DOT__run_clocks__1__test_copper_lists__DOT__unnamedblk1_1__DOT____Vrepeat0)) {
        Vtest_copper_lists___024root____VbeforeTrig_h76281f29__0(vlSelf, 
                                                                 "@(posedge test_copper_lists.clk)");
        co_await vlSelfRef.__VtrigSched_h76281f29__0.trigger(0U, 
                                                             nullptr, 
                                                             "@(posedge test_copper_lists.clk)", 
                                                             "test_copper_lists.sv", 
                                                             88);
        __Vtask_test_copper_lists__DOT__run_clocks__1__test_copper_lists__DOT__unnamedblk1_1__DOT____Vrepeat0 
            = (__Vtask_test_copper_lists__DOT__run_clocks__1__test_copper_lists__DOT__unnamedblk1_1__DOT____Vrepeat0 
               - (IData)(1U));
    }
    VL_WRITEF_NX("Test: Initial multi-list state\n",0);
    __Vtask_test_copper_lists__DOT__check__2__condition 
        = (0U == (IData)(vlSelfRef.test_copper_lists__DOT__dut__DOT__copper_target_list));
    vlSelfRef.__Vtask_test_copper_lists__DOT__check__2__name = "target list starts at 0"s;
    vlSelfRef.test_copper_lists__DOT__test_num = ((IData)(1U) 
                                                  + vlSelfRef.test_copper_lists__DOT__test_num);
    if (__Vtask_test_copper_lists__DOT__check__2__condition) {
        VL_WRITEF_NX("  PASS [%0d] %@\n",0,32,vlSelfRef.test_copper_lists__DOT__test_num,
                     -1,&(vlSelfRef.__Vtask_test_copper_lists__DOT__check__2__name));
        vlSelfRef.test_copper_lists__DOT__pass_count 
            = ((IData)(1U) + vlSelfRef.test_copper_lists__DOT__pass_count);
    } else {
        VL_WRITEF_NX("  FAIL [%0d] %@\n",0,32,vlSelfRef.test_copper_lists__DOT__test_num,
                     -1,&(vlSelfRef.__Vtask_test_copper_lists__DOT__check__2__name));
        vlSelfRef.test_copper_lists__DOT__fail_count 
            = ((IData)(1U) + vlSelfRef.test_copper_lists__DOT__fail_count);
    }
    __Vtask_test_copper_lists__DOT__check__3__condition 
        = (0U == (IData)(vlSelfRef.test_copper_lists__DOT__dut__DOT__copper_active_list));
    vlSelfRef.__Vtask_test_copper_lists__DOT__check__3__name = "active list starts at 0"s;
    vlSelfRef.test_copper_lists__DOT__test_num = ((IData)(1U) 
                                                  + vlSelfRef.test_copper_lists__DOT__test_num);
    if (__Vtask_test_copper_lists__DOT__check__3__condition) {
        VL_WRITEF_NX("  PASS [%0d] %@\n",0,32,vlSelfRef.test_copper_lists__DOT__test_num,
                     -1,&(vlSelfRef.__Vtask_test_copper_lists__DOT__check__3__name));
        vlSelfRef.test_copper_lists__DOT__pass_count 
            = ((IData)(1U) + vlSelfRef.test_copper_lists__DOT__pass_count);
    } else {
        VL_WRITEF_NX("  FAIL [%0d] %@\n",0,32,vlSelfRef.test_copper_lists__DOT__test_num,
                     -1,&(vlSelfRef.__Vtask_test_copper_lists__DOT__check__3__name));
        vlSelfRef.test_copper_lists__DOT__fail_count 
            = ((IData)(1U) + vlSelfRef.test_copper_lists__DOT__fail_count);
    }
    __Vtask_test_copper_lists__DOT__check__4__condition 
        = (0U == (IData)(vlSelfRef.test_copper_lists__DOT__dut__DOT__copper_pending_list));
    vlSelfRef.__Vtask_test_copper_lists__DOT__check__4__name = "pending list starts at 0"s;
    vlSelfRef.test_copper_lists__DOT__test_num = ((IData)(1U) 
                                                  + vlSelfRef.test_copper_lists__DOT__test_num);
    if (__Vtask_test_copper_lists__DOT__check__4__condition) {
        VL_WRITEF_NX("  PASS [%0d] %@\n",0,32,vlSelfRef.test_copper_lists__DOT__test_num,
                     -1,&(vlSelfRef.__Vtask_test_copper_lists__DOT__check__4__name));
        vlSelfRef.test_copper_lists__DOT__pass_count 
            = ((IData)(1U) + vlSelfRef.test_copper_lists__DOT__pass_count);
    } else {
        VL_WRITEF_NX("  FAIL [%0d] %@\n",0,32,vlSelfRef.test_copper_lists__DOT__test_num,
                     -1,&(vlSelfRef.__Vtask_test_copper_lists__DOT__check__4__name));
        vlSelfRef.test_copper_lists__DOT__fail_count 
            = ((IData)(1U) + vlSelfRef.test_copper_lists__DOT__fail_count);
    }
    VL_WRITEF_NX("Test: CopperList command\n",0);
    __Vtask_test_copper_lists__DOT__copper_list__5__idx = 3U;
    __Vtask_test_copper_lists__DOT__write_param__6__data 
        = (0x000000ffU & __Vtask_test_copper_lists__DOT__copper_list__5__idx);
    __Vtask_test_copper_lists__DOT__write_param__6__idx = 0U;
    __Vtask_test_copper_lists__DOT__write_reg__7__data 
        = __Vtask_test_copper_lists__DOT__write_param__6__data;
    __Vtask_test_copper_lists__DOT__write_reg__7__addr 
        = (0x0000ffffU & ((IData)(0xa011U) + __Vtask_test_copper_lists__DOT__write_param__6__idx));
    Vtest_copper_lists___024root____VbeforeTrig_h76281f29__0(vlSelf, 
                                                             "@(posedge test_copper_lists.clk)");
    co_await vlSelfRef.__VtrigSched_h76281f29__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge test_copper_lists.clk)", 
                                                         "test_copper_lists.sv", 
                                                         44);
    vlSelfRef.test_copper_lists__DOT__cpu_addr = __Vtask_test_copper_lists__DOT__write_reg__7__addr;
    vlSelfRef.test_copper_lists__DOT__cpu_wdata = __Vtask_test_copper_lists__DOT__write_reg__7__data;
    vlSelfRef.test_copper_lists__DOT__cpu_we = 1U;
    vlSelfRef.test_copper_lists__DOT__cpu_ce = 1U;
    Vtest_copper_lists___024root____VbeforeTrig_h76281f29__0(vlSelf, 
                                                             "@(posedge test_copper_lists.clk)");
    co_await vlSelfRef.__VtrigSched_h76281f29__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge test_copper_lists.clk)", 
                                                         "test_copper_lists.sv", 
                                                         46);
    vlSelfRef.test_copper_lists__DOT__cpu_we = 0U;
    vlSelfRef.test_copper_lists__DOT__cpu_ce = 0U;
    __Vtask_test_copper_lists__DOT__write_cmd__8__cmd = 0x20U;
    __Vtask_test_copper_lists__DOT__write_reg__9__data 
        = __Vtask_test_copper_lists__DOT__write_cmd__8__cmd;
    __Vtask_test_copper_lists__DOT__write_reg__9__addr = 0xa010U;
    Vtest_copper_lists___024root____VbeforeTrig_h76281f29__0(vlSelf, 
                                                             "@(posedge test_copper_lists.clk)");
    co_await vlSelfRef.__VtrigSched_h76281f29__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge test_copper_lists.clk)", 
                                                         "test_copper_lists.sv", 
                                                         44);
    vlSelfRef.test_copper_lists__DOT__cpu_addr = __Vtask_test_copper_lists__DOT__write_reg__9__addr;
    vlSelfRef.test_copper_lists__DOT__cpu_wdata = __Vtask_test_copper_lists__DOT__write_reg__9__data;
    vlSelfRef.test_copper_lists__DOT__cpu_we = 1U;
    vlSelfRef.test_copper_lists__DOT__cpu_ce = 1U;
    Vtest_copper_lists___024root____VbeforeTrig_h76281f29__0(vlSelf, 
                                                             "@(posedge test_copper_lists.clk)");
    co_await vlSelfRef.__VtrigSched_h76281f29__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge test_copper_lists.clk)", 
                                                         "test_copper_lists.sv", 
                                                         46);
    vlSelfRef.test_copper_lists__DOT__cpu_we = 0U;
    vlSelfRef.test_copper_lists__DOT__cpu_ce = 0U;
    __Vtask_test_copper_lists__DOT__run_clocks__10__n = 2U;
    __Vtask_test_copper_lists__DOT__run_clocks__10__test_copper_lists__DOT__unnamedblk1_1__DOT____Vrepeat0 = 0;
    __Vtask_test_copper_lists__DOT__run_clocks__10__test_copper_lists__DOT__unnamedblk1_1__DOT____Vrepeat0 
        = __Vtask_test_copper_lists__DOT__run_clocks__10__n;
    while (VL_LTS_III(32, 0U, __Vtask_test_copper_lists__DOT__run_clocks__10__test_copper_lists__DOT__unnamedblk1_1__DOT____Vrepeat0)) {
        Vtest_copper_lists___024root____VbeforeTrig_h76281f29__0(vlSelf, 
                                                                 "@(posedge test_copper_lists.clk)");
        co_await vlSelfRef.__VtrigSched_h76281f29__0.trigger(0U, 
                                                             nullptr, 
                                                             "@(posedge test_copper_lists.clk)", 
                                                             "test_copper_lists.sv", 
                                                             88);
        __Vtask_test_copper_lists__DOT__run_clocks__10__test_copper_lists__DOT__unnamedblk1_1__DOT____Vrepeat0 
            = (__Vtask_test_copper_lists__DOT__run_clocks__10__test_copper_lists__DOT__unnamedblk1_1__DOT____Vrepeat0 
               - (IData)(1U));
    }
    __Vtask_test_copper_lists__DOT__check__11__condition 
        = (3U == (IData)(vlSelfRef.test_copper_lists__DOT__dut__DOT__copper_target_list));
    vlSelfRef.__Vtask_test_copper_lists__DOT__check__11__name = "target list set to 3"s;
    vlSelfRef.test_copper_lists__DOT__test_num = ((IData)(1U) 
                                                  + vlSelfRef.test_copper_lists__DOT__test_num);
    if (__Vtask_test_copper_lists__DOT__check__11__condition) {
        VL_WRITEF_NX("  PASS [%0d] %@\n",0,32,vlSelfRef.test_copper_lists__DOT__test_num,
                     -1,&(vlSelfRef.__Vtask_test_copper_lists__DOT__check__11__name));
        vlSelfRef.test_copper_lists__DOT__pass_count 
            = ((IData)(1U) + vlSelfRef.test_copper_lists__DOT__pass_count);
    } else {
        VL_WRITEF_NX("  FAIL [%0d] %@\n",0,32,vlSelfRef.test_copper_lists__DOT__test_num,
                     -1,&(vlSelfRef.__Vtask_test_copper_lists__DOT__check__11__name));
        vlSelfRef.test_copper_lists__DOT__fail_count 
            = ((IData)(1U) + vlSelfRef.test_copper_lists__DOT__fail_count);
    }
    __Vtask_test_copper_lists__DOT__check__12__condition 
        = (0U == (IData)(vlSelfRef.test_copper_lists__DOT__dut__DOT__copper_active_list));
    vlSelfRef.__Vtask_test_copper_lists__DOT__check__12__name = "active list unchanged"s;
    vlSelfRef.test_copper_lists__DOT__test_num = ((IData)(1U) 
                                                  + vlSelfRef.test_copper_lists__DOT__test_num);
    if (__Vtask_test_copper_lists__DOT__check__12__condition) {
        VL_WRITEF_NX("  PASS [%0d] %@\n",0,32,vlSelfRef.test_copper_lists__DOT__test_num,
                     -1,&(vlSelfRef.__Vtask_test_copper_lists__DOT__check__12__name));
        vlSelfRef.test_copper_lists__DOT__pass_count 
            = ((IData)(1U) + vlSelfRef.test_copper_lists__DOT__pass_count);
    } else {
        VL_WRITEF_NX("  FAIL [%0d] %@\n",0,32,vlSelfRef.test_copper_lists__DOT__test_num,
                     -1,&(vlSelfRef.__Vtask_test_copper_lists__DOT__check__12__name));
        vlSelfRef.test_copper_lists__DOT__fail_count 
            = ((IData)(1U) + vlSelfRef.test_copper_lists__DOT__fail_count);
    }
    VL_WRITEF_NX("Test: Add entries to non-active list\n",0);
    __Vtask_test_copper_lists__DOT__copper_add__13__value = 5U;
    __Vtask_test_copper_lists__DOT__copper_add__13__reg_idx = 1U;
    __Vtask_test_copper_lists__DOT__copper_add__13__y = 0x00000032U;
    __Vtask_test_copper_lists__DOT__copper_add__13__x = 0U;
    __Vtask_test_copper_lists__DOT__write_param__14__data 
        = (0x000000ffU & __Vtask_test_copper_lists__DOT__copper_add__13__x);
    __Vtask_test_copper_lists__DOT__write_param__14__idx = 0U;
    __Vtask_test_copper_lists__DOT__write_reg__15__data 
        = __Vtask_test_copper_lists__DOT__write_param__14__data;
    __Vtask_test_copper_lists__DOT__write_reg__15__addr 
        = (0x0000ffffU & ((IData)(0xa011U) + __Vtask_test_copper_lists__DOT__write_param__14__idx));
    Vtest_copper_lists___024root____VbeforeTrig_h76281f29__0(vlSelf, 
                                                             "@(posedge test_copper_lists.clk)");
    co_await vlSelfRef.__VtrigSched_h76281f29__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge test_copper_lists.clk)", 
                                                         "test_copper_lists.sv", 
                                                         44);
    vlSelfRef.test_copper_lists__DOT__cpu_addr = __Vtask_test_copper_lists__DOT__write_reg__15__addr;
    vlSelfRef.test_copper_lists__DOT__cpu_wdata = __Vtask_test_copper_lists__DOT__write_reg__15__data;
    vlSelfRef.test_copper_lists__DOT__cpu_we = 1U;
    vlSelfRef.test_copper_lists__DOT__cpu_ce = 1U;
    Vtest_copper_lists___024root____VbeforeTrig_h76281f29__0(vlSelf, 
                                                             "@(posedge test_copper_lists.clk)");
    co_await vlSelfRef.__VtrigSched_h76281f29__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge test_copper_lists.clk)", 
                                                         "test_copper_lists.sv", 
                                                         46);
    vlSelfRef.test_copper_lists__DOT__cpu_we = 0U;
    vlSelfRef.test_copper_lists__DOT__cpu_ce = 0U;
    __Vtask_test_copper_lists__DOT__write_param__16__data 
        = (0x000000ffU & (__Vtask_test_copper_lists__DOT__copper_add__13__x 
                          >> 8U));
    __Vtask_test_copper_lists__DOT__write_param__16__idx = 1U;
    __Vtask_test_copper_lists__DOT__write_reg__17__data 
        = __Vtask_test_copper_lists__DOT__write_param__16__data;
    __Vtask_test_copper_lists__DOT__write_reg__17__addr 
        = (0x0000ffffU & ((IData)(0xa011U) + __Vtask_test_copper_lists__DOT__write_param__16__idx));
    Vtest_copper_lists___024root____VbeforeTrig_h76281f29__0(vlSelf, 
                                                             "@(posedge test_copper_lists.clk)");
    co_await vlSelfRef.__VtrigSched_h76281f29__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge test_copper_lists.clk)", 
                                                         "test_copper_lists.sv", 
                                                         44);
    vlSelfRef.test_copper_lists__DOT__cpu_addr = __Vtask_test_copper_lists__DOT__write_reg__17__addr;
    vlSelfRef.test_copper_lists__DOT__cpu_wdata = __Vtask_test_copper_lists__DOT__write_reg__17__data;
    vlSelfRef.test_copper_lists__DOT__cpu_we = 1U;
    vlSelfRef.test_copper_lists__DOT__cpu_ce = 1U;
    Vtest_copper_lists___024root____VbeforeTrig_h76281f29__0(vlSelf, 
                                                             "@(posedge test_copper_lists.clk)");
    co_await vlSelfRef.__VtrigSched_h76281f29__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge test_copper_lists.clk)", 
                                                         "test_copper_lists.sv", 
                                                         46);
    vlSelfRef.test_copper_lists__DOT__cpu_we = 0U;
    vlSelfRef.test_copper_lists__DOT__cpu_ce = 0U;
    __Vtask_test_copper_lists__DOT__write_param__18__data 
        = (0x000000ffU & __Vtask_test_copper_lists__DOT__copper_add__13__y);
    __Vtask_test_copper_lists__DOT__write_param__18__idx = 2U;
    __Vtask_test_copper_lists__DOT__write_reg__19__data 
        = __Vtask_test_copper_lists__DOT__write_param__18__data;
    __Vtask_test_copper_lists__DOT__write_reg__19__addr 
        = (0x0000ffffU & ((IData)(0xa011U) + __Vtask_test_copper_lists__DOT__write_param__18__idx));
    Vtest_copper_lists___024root____VbeforeTrig_h76281f29__0(vlSelf, 
                                                             "@(posedge test_copper_lists.clk)");
    co_await vlSelfRef.__VtrigSched_h76281f29__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge test_copper_lists.clk)", 
                                                         "test_copper_lists.sv", 
                                                         44);
    vlSelfRef.test_copper_lists__DOT__cpu_addr = __Vtask_test_copper_lists__DOT__write_reg__19__addr;
    vlSelfRef.test_copper_lists__DOT__cpu_wdata = __Vtask_test_copper_lists__DOT__write_reg__19__data;
    vlSelfRef.test_copper_lists__DOT__cpu_we = 1U;
    vlSelfRef.test_copper_lists__DOT__cpu_ce = 1U;
    Vtest_copper_lists___024root____VbeforeTrig_h76281f29__0(vlSelf, 
                                                             "@(posedge test_copper_lists.clk)");
    co_await vlSelfRef.__VtrigSched_h76281f29__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge test_copper_lists.clk)", 
                                                         "test_copper_lists.sv", 
                                                         46);
    vlSelfRef.test_copper_lists__DOT__cpu_we = 0U;
    vlSelfRef.test_copper_lists__DOT__cpu_ce = 0U;
    __Vtask_test_copper_lists__DOT__write_param__20__data 
        = (0x000000ffU & __Vtask_test_copper_lists__DOT__copper_add__13__reg_idx);
    __Vtask_test_copper_lists__DOT__write_param__20__idx = 3U;
    __Vtask_test_copper_lists__DOT__write_reg__21__data 
        = __Vtask_test_copper_lists__DOT__write_param__20__data;
    __Vtask_test_copper_lists__DOT__write_reg__21__addr 
        = (0x0000ffffU & ((IData)(0xa011U) + __Vtask_test_copper_lists__DOT__write_param__20__idx));
    Vtest_copper_lists___024root____VbeforeTrig_h76281f29__0(vlSelf, 
                                                             "@(posedge test_copper_lists.clk)");
    co_await vlSelfRef.__VtrigSched_h76281f29__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge test_copper_lists.clk)", 
                                                         "test_copper_lists.sv", 
                                                         44);
    vlSelfRef.test_copper_lists__DOT__cpu_addr = __Vtask_test_copper_lists__DOT__write_reg__21__addr;
    vlSelfRef.test_copper_lists__DOT__cpu_wdata = __Vtask_test_copper_lists__DOT__write_reg__21__data;
    vlSelfRef.test_copper_lists__DOT__cpu_we = 1U;
    vlSelfRef.test_copper_lists__DOT__cpu_ce = 1U;
    Vtest_copper_lists___024root____VbeforeTrig_h76281f29__0(vlSelf, 
                                                             "@(posedge test_copper_lists.clk)");
    co_await vlSelfRef.__VtrigSched_h76281f29__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge test_copper_lists.clk)", 
                                                         "test_copper_lists.sv", 
                                                         46);
    vlSelfRef.test_copper_lists__DOT__cpu_we = 0U;
    vlSelfRef.test_copper_lists__DOT__cpu_ce = 0U;
    __Vtask_test_copper_lists__DOT__write_param__22__data 
        = (0x000000ffU & (__Vtask_test_copper_lists__DOT__copper_add__13__reg_idx 
                          >> 8U));
    __Vtask_test_copper_lists__DOT__write_param__22__idx = 4U;
    __Vtask_test_copper_lists__DOT__write_reg__23__data 
        = __Vtask_test_copper_lists__DOT__write_param__22__data;
    __Vtask_test_copper_lists__DOT__write_reg__23__addr 
        = (0x0000ffffU & ((IData)(0xa011U) + __Vtask_test_copper_lists__DOT__write_param__22__idx));
    Vtest_copper_lists___024root____VbeforeTrig_h76281f29__0(vlSelf, 
                                                             "@(posedge test_copper_lists.clk)");
    co_await vlSelfRef.__VtrigSched_h76281f29__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge test_copper_lists.clk)", 
                                                         "test_copper_lists.sv", 
                                                         44);
    vlSelfRef.test_copper_lists__DOT__cpu_addr = __Vtask_test_copper_lists__DOT__write_reg__23__addr;
    vlSelfRef.test_copper_lists__DOT__cpu_wdata = __Vtask_test_copper_lists__DOT__write_reg__23__data;
    vlSelfRef.test_copper_lists__DOT__cpu_we = 1U;
    vlSelfRef.test_copper_lists__DOT__cpu_ce = 1U;
    Vtest_copper_lists___024root____VbeforeTrig_h76281f29__0(vlSelf, 
                                                             "@(posedge test_copper_lists.clk)");
    co_await vlSelfRef.__VtrigSched_h76281f29__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge test_copper_lists.clk)", 
                                                         "test_copper_lists.sv", 
                                                         46);
    vlSelfRef.test_copper_lists__DOT__cpu_we = 0U;
    vlSelfRef.test_copper_lists__DOT__cpu_ce = 0U;
    __Vtask_test_copper_lists__DOT__write_param__24__data 
        = __Vtask_test_copper_lists__DOT__copper_add__13__value;
    __Vtask_test_copper_lists__DOT__write_param__24__idx = 5U;
    __Vtask_test_copper_lists__DOT__write_reg__25__data 
        = __Vtask_test_copper_lists__DOT__write_param__24__data;
    __Vtask_test_copper_lists__DOT__write_reg__25__addr 
        = (0x0000ffffU & ((IData)(0xa011U) + __Vtask_test_copper_lists__DOT__write_param__24__idx));
    Vtest_copper_lists___024root____VbeforeTrig_h76281f29__0(vlSelf, 
                                                             "@(posedge test_copper_lists.clk)");
    co_await vlSelfRef.__VtrigSched_h76281f29__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge test_copper_lists.clk)", 
                                                         "test_copper_lists.sv", 
                                                         44);
    vlSelfRef.test_copper_lists__DOT__cpu_addr = __Vtask_test_copper_lists__DOT__write_reg__25__addr;
    vlSelfRef.test_copper_lists__DOT__cpu_wdata = __Vtask_test_copper_lists__DOT__write_reg__25__data;
    vlSelfRef.test_copper_lists__DOT__cpu_we = 1U;
    vlSelfRef.test_copper_lists__DOT__cpu_ce = 1U;
    Vtest_copper_lists___024root____VbeforeTrig_h76281f29__0(vlSelf, 
                                                             "@(posedge test_copper_lists.clk)");
    co_await vlSelfRef.__VtrigSched_h76281f29__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge test_copper_lists.clk)", 
                                                         "test_copper_lists.sv", 
                                                         46);
    vlSelfRef.test_copper_lists__DOT__cpu_we = 0U;
    vlSelfRef.test_copper_lists__DOT__cpu_ce = 0U;
    __Vtask_test_copper_lists__DOT__write_cmd__26__cmd = 0x1bU;
    __Vtask_test_copper_lists__DOT__write_reg__27__data 
        = __Vtask_test_copper_lists__DOT__write_cmd__26__cmd;
    __Vtask_test_copper_lists__DOT__write_reg__27__addr = 0xa010U;
    Vtest_copper_lists___024root____VbeforeTrig_h76281f29__0(vlSelf, 
                                                             "@(posedge test_copper_lists.clk)");
    co_await vlSelfRef.__VtrigSched_h76281f29__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge test_copper_lists.clk)", 
                                                         "test_copper_lists.sv", 
                                                         44);
    vlSelfRef.test_copper_lists__DOT__cpu_addr = __Vtask_test_copper_lists__DOT__write_reg__27__addr;
    vlSelfRef.test_copper_lists__DOT__cpu_wdata = __Vtask_test_copper_lists__DOT__write_reg__27__data;
    vlSelfRef.test_copper_lists__DOT__cpu_we = 1U;
    vlSelfRef.test_copper_lists__DOT__cpu_ce = 1U;
    Vtest_copper_lists___024root____VbeforeTrig_h76281f29__0(vlSelf, 
                                                             "@(posedge test_copper_lists.clk)");
    co_await vlSelfRef.__VtrigSched_h76281f29__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge test_copper_lists.clk)", 
                                                         "test_copper_lists.sv", 
                                                         46);
    vlSelfRef.test_copper_lists__DOT__cpu_we = 0U;
    vlSelfRef.test_copper_lists__DOT__cpu_ce = 0U;
    __Vtask_test_copper_lists__DOT__run_clocks__28__n = 2U;
    __Vtask_test_copper_lists__DOT__run_clocks__28__test_copper_lists__DOT__unnamedblk1_1__DOT____Vrepeat0 = 0;
    __Vtask_test_copper_lists__DOT__run_clocks__28__test_copper_lists__DOT__unnamedblk1_1__DOT____Vrepeat0 
        = __Vtask_test_copper_lists__DOT__run_clocks__28__n;
    while (VL_LTS_III(32, 0U, __Vtask_test_copper_lists__DOT__run_clocks__28__test_copper_lists__DOT__unnamedblk1_1__DOT____Vrepeat0)) {
        Vtest_copper_lists___024root____VbeforeTrig_h76281f29__0(vlSelf, 
                                                                 "@(posedge test_copper_lists.clk)");
        co_await vlSelfRef.__VtrigSched_h76281f29__0.trigger(0U, 
                                                             nullptr, 
                                                             "@(posedge test_copper_lists.clk)", 
                                                             "test_copper_lists.sv", 
                                                             88);
        __Vtask_test_copper_lists__DOT__run_clocks__28__test_copper_lists__DOT__unnamedblk1_1__DOT____Vrepeat0 
            = (__Vtask_test_copper_lists__DOT__run_clocks__28__test_copper_lists__DOT__unnamedblk1_1__DOT____Vrepeat0 
               - (IData)(1U));
    }
    __Vtask_test_copper_lists__DOT__check__29__condition 
        = (1U == vlSelfRef.test_copper_lists__DOT__dut__DOT__copper_list_count[3U]);
    vlSelfRef.__Vtask_test_copper_lists__DOT__check__29__name = "list 3 count is 1"s;
    vlSelfRef.test_copper_lists__DOT__test_num = ((IData)(1U) 
                                                  + vlSelfRef.test_copper_lists__DOT__test_num);
    if (__Vtask_test_copper_lists__DOT__check__29__condition) {
        VL_WRITEF_NX("  PASS [%0d] %@\n",0,32,vlSelfRef.test_copper_lists__DOT__test_num,
                     -1,&(vlSelfRef.__Vtask_test_copper_lists__DOT__check__29__name));
        vlSelfRef.test_copper_lists__DOT__pass_count 
            = ((IData)(1U) + vlSelfRef.test_copper_lists__DOT__pass_count);
    } else {
        VL_WRITEF_NX("  FAIL [%0d] %@\n",0,32,vlSelfRef.test_copper_lists__DOT__test_num,
                     -1,&(vlSelfRef.__Vtask_test_copper_lists__DOT__check__29__name));
        vlSelfRef.test_copper_lists__DOT__fail_count 
            = ((IData)(1U) + vlSelfRef.test_copper_lists__DOT__fail_count);
    }
    __Vtask_test_copper_lists__DOT__check__30__condition 
        = (0U == (IData)(vlSelfRef.test_copper_lists__DOT__dut__DOT__copper_count));
    vlSelfRef.__Vtask_test_copper_lists__DOT__check__30__name = "active list count unchanged"s;
    vlSelfRef.test_copper_lists__DOT__test_num = ((IData)(1U) 
                                                  + vlSelfRef.test_copper_lists__DOT__test_num);
    if (__Vtask_test_copper_lists__DOT__check__30__condition) {
        VL_WRITEF_NX("  PASS [%0d] %@\n",0,32,vlSelfRef.test_copper_lists__DOT__test_num,
                     -1,&(vlSelfRef.__Vtask_test_copper_lists__DOT__check__30__name));
        vlSelfRef.test_copper_lists__DOT__pass_count 
            = ((IData)(1U) + vlSelfRef.test_copper_lists__DOT__pass_count);
    } else {
        VL_WRITEF_NX("  FAIL [%0d] %@\n",0,32,vlSelfRef.test_copper_lists__DOT__test_num,
                     -1,&(vlSelfRef.__Vtask_test_copper_lists__DOT__check__30__name));
        vlSelfRef.test_copper_lists__DOT__fail_count 
            = ((IData)(1U) + vlSelfRef.test_copper_lists__DOT__fail_count);
    }
    VL_WRITEF_NX("Test: Add entries to active list (0)\n",0);
    __Vtask_test_copper_lists__DOT__copper_list__31__idx = 0U;
    __Vtask_test_copper_lists__DOT__write_param__32__data 
        = (0x000000ffU & __Vtask_test_copper_lists__DOT__copper_list__31__idx);
    __Vtask_test_copper_lists__DOT__write_param__32__idx = 0U;
    __Vtask_test_copper_lists__DOT__write_reg__33__data 
        = __Vtask_test_copper_lists__DOT__write_param__32__data;
    __Vtask_test_copper_lists__DOT__write_reg__33__addr 
        = (0x0000ffffU & ((IData)(0xa011U) + __Vtask_test_copper_lists__DOT__write_param__32__idx));
    Vtest_copper_lists___024root____VbeforeTrig_h76281f29__0(vlSelf, 
                                                             "@(posedge test_copper_lists.clk)");
    co_await vlSelfRef.__VtrigSched_h76281f29__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge test_copper_lists.clk)", 
                                                         "test_copper_lists.sv", 
                                                         44);
    vlSelfRef.test_copper_lists__DOT__cpu_addr = __Vtask_test_copper_lists__DOT__write_reg__33__addr;
    vlSelfRef.test_copper_lists__DOT__cpu_wdata = __Vtask_test_copper_lists__DOT__write_reg__33__data;
    vlSelfRef.test_copper_lists__DOT__cpu_we = 1U;
    vlSelfRef.test_copper_lists__DOT__cpu_ce = 1U;
    Vtest_copper_lists___024root____VbeforeTrig_h76281f29__0(vlSelf, 
                                                             "@(posedge test_copper_lists.clk)");
    co_await vlSelfRef.__VtrigSched_h76281f29__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge test_copper_lists.clk)", 
                                                         "test_copper_lists.sv", 
                                                         46);
    vlSelfRef.test_copper_lists__DOT__cpu_we = 0U;
    vlSelfRef.test_copper_lists__DOT__cpu_ce = 0U;
    __Vtask_test_copper_lists__DOT__write_cmd__34__cmd = 0x20U;
    __Vtask_test_copper_lists__DOT__write_reg__35__data 
        = __Vtask_test_copper_lists__DOT__write_cmd__34__cmd;
    __Vtask_test_copper_lists__DOT__write_reg__35__addr = 0xa010U;
    Vtest_copper_lists___024root____VbeforeTrig_h76281f29__0(vlSelf, 
                                                             "@(posedge test_copper_lists.clk)");
    co_await vlSelfRef.__VtrigSched_h76281f29__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge test_copper_lists.clk)", 
                                                         "test_copper_lists.sv", 
                                                         44);
    vlSelfRef.test_copper_lists__DOT__cpu_addr = __Vtask_test_copper_lists__DOT__write_reg__35__addr;
    vlSelfRef.test_copper_lists__DOT__cpu_wdata = __Vtask_test_copper_lists__DOT__write_reg__35__data;
    vlSelfRef.test_copper_lists__DOT__cpu_we = 1U;
    vlSelfRef.test_copper_lists__DOT__cpu_ce = 1U;
    Vtest_copper_lists___024root____VbeforeTrig_h76281f29__0(vlSelf, 
                                                             "@(posedge test_copper_lists.clk)");
    co_await vlSelfRef.__VtrigSched_h76281f29__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge test_copper_lists.clk)", 
                                                         "test_copper_lists.sv", 
                                                         46);
    vlSelfRef.test_copper_lists__DOT__cpu_we = 0U;
    vlSelfRef.test_copper_lists__DOT__cpu_ce = 0U;
    __Vtask_test_copper_lists__DOT__run_clocks__36__n = 2U;
    __Vtask_test_copper_lists__DOT__run_clocks__36__test_copper_lists__DOT__unnamedblk1_1__DOT____Vrepeat0 = 0;
    __Vtask_test_copper_lists__DOT__run_clocks__36__test_copper_lists__DOT__unnamedblk1_1__DOT____Vrepeat0 
        = __Vtask_test_copper_lists__DOT__run_clocks__36__n;
    while (VL_LTS_III(32, 0U, __Vtask_test_copper_lists__DOT__run_clocks__36__test_copper_lists__DOT__unnamedblk1_1__DOT____Vrepeat0)) {
        Vtest_copper_lists___024root____VbeforeTrig_h76281f29__0(vlSelf, 
                                                                 "@(posedge test_copper_lists.clk)");
        co_await vlSelfRef.__VtrigSched_h76281f29__0.trigger(0U, 
                                                             nullptr, 
                                                             "@(posedge test_copper_lists.clk)", 
                                                             "test_copper_lists.sv", 
                                                             88);
        __Vtask_test_copper_lists__DOT__run_clocks__36__test_copper_lists__DOT__unnamedblk1_1__DOT____Vrepeat0 
            = (__Vtask_test_copper_lists__DOT__run_clocks__36__test_copper_lists__DOT__unnamedblk1_1__DOT____Vrepeat0 
               - (IData)(1U));
    }
    __Vtask_test_copper_lists__DOT__copper_add__37__value = 2U;
    __Vtask_test_copper_lists__DOT__copper_add__37__reg_idx = 1U;
    __Vtask_test_copper_lists__DOT__copper_add__37__y = 0x0000001eU;
    __Vtask_test_copper_lists__DOT__copper_add__37__x = 0U;
    __Vtask_test_copper_lists__DOT__write_param__38__data 
        = (0x000000ffU & __Vtask_test_copper_lists__DOT__copper_add__37__x);
    __Vtask_test_copper_lists__DOT__write_param__38__idx = 0U;
    __Vtask_test_copper_lists__DOT__write_reg__39__data 
        = __Vtask_test_copper_lists__DOT__write_param__38__data;
    __Vtask_test_copper_lists__DOT__write_reg__39__addr 
        = (0x0000ffffU & ((IData)(0xa011U) + __Vtask_test_copper_lists__DOT__write_param__38__idx));
    Vtest_copper_lists___024root____VbeforeTrig_h76281f29__0(vlSelf, 
                                                             "@(posedge test_copper_lists.clk)");
    co_await vlSelfRef.__VtrigSched_h76281f29__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge test_copper_lists.clk)", 
                                                         "test_copper_lists.sv", 
                                                         44);
    vlSelfRef.test_copper_lists__DOT__cpu_addr = __Vtask_test_copper_lists__DOT__write_reg__39__addr;
    vlSelfRef.test_copper_lists__DOT__cpu_wdata = __Vtask_test_copper_lists__DOT__write_reg__39__data;
    vlSelfRef.test_copper_lists__DOT__cpu_we = 1U;
    vlSelfRef.test_copper_lists__DOT__cpu_ce = 1U;
    Vtest_copper_lists___024root____VbeforeTrig_h76281f29__0(vlSelf, 
                                                             "@(posedge test_copper_lists.clk)");
    co_await vlSelfRef.__VtrigSched_h76281f29__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge test_copper_lists.clk)", 
                                                         "test_copper_lists.sv", 
                                                         46);
    vlSelfRef.test_copper_lists__DOT__cpu_we = 0U;
    vlSelfRef.test_copper_lists__DOT__cpu_ce = 0U;
    __Vtask_test_copper_lists__DOT__write_param__40__data 
        = (0x000000ffU & (__Vtask_test_copper_lists__DOT__copper_add__37__x 
                          >> 8U));
    __Vtask_test_copper_lists__DOT__write_param__40__idx = 1U;
    __Vtask_test_copper_lists__DOT__write_reg__41__data 
        = __Vtask_test_copper_lists__DOT__write_param__40__data;
    __Vtask_test_copper_lists__DOT__write_reg__41__addr 
        = (0x0000ffffU & ((IData)(0xa011U) + __Vtask_test_copper_lists__DOT__write_param__40__idx));
    Vtest_copper_lists___024root____VbeforeTrig_h76281f29__0(vlSelf, 
                                                             "@(posedge test_copper_lists.clk)");
    co_await vlSelfRef.__VtrigSched_h76281f29__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge test_copper_lists.clk)", 
                                                         "test_copper_lists.sv", 
                                                         44);
    vlSelfRef.test_copper_lists__DOT__cpu_addr = __Vtask_test_copper_lists__DOT__write_reg__41__addr;
    vlSelfRef.test_copper_lists__DOT__cpu_wdata = __Vtask_test_copper_lists__DOT__write_reg__41__data;
    vlSelfRef.test_copper_lists__DOT__cpu_we = 1U;
    vlSelfRef.test_copper_lists__DOT__cpu_ce = 1U;
    Vtest_copper_lists___024root____VbeforeTrig_h76281f29__0(vlSelf, 
                                                             "@(posedge test_copper_lists.clk)");
    co_await vlSelfRef.__VtrigSched_h76281f29__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge test_copper_lists.clk)", 
                                                         "test_copper_lists.sv", 
                                                         46);
    vlSelfRef.test_copper_lists__DOT__cpu_we = 0U;
    vlSelfRef.test_copper_lists__DOT__cpu_ce = 0U;
    __Vtask_test_copper_lists__DOT__write_param__42__data 
        = (0x000000ffU & __Vtask_test_copper_lists__DOT__copper_add__37__y);
    __Vtask_test_copper_lists__DOT__write_param__42__idx = 2U;
    __Vtask_test_copper_lists__DOT__write_reg__43__data 
        = __Vtask_test_copper_lists__DOT__write_param__42__data;
    __Vtask_test_copper_lists__DOT__write_reg__43__addr 
        = (0x0000ffffU & ((IData)(0xa011U) + __Vtask_test_copper_lists__DOT__write_param__42__idx));
    Vtest_copper_lists___024root____VbeforeTrig_h76281f29__0(vlSelf, 
                                                             "@(posedge test_copper_lists.clk)");
    co_await vlSelfRef.__VtrigSched_h76281f29__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge test_copper_lists.clk)", 
                                                         "test_copper_lists.sv", 
                                                         44);
    vlSelfRef.test_copper_lists__DOT__cpu_addr = __Vtask_test_copper_lists__DOT__write_reg__43__addr;
    vlSelfRef.test_copper_lists__DOT__cpu_wdata = __Vtask_test_copper_lists__DOT__write_reg__43__data;
    vlSelfRef.test_copper_lists__DOT__cpu_we = 1U;
    vlSelfRef.test_copper_lists__DOT__cpu_ce = 1U;
    Vtest_copper_lists___024root____VbeforeTrig_h76281f29__0(vlSelf, 
                                                             "@(posedge test_copper_lists.clk)");
    co_await vlSelfRef.__VtrigSched_h76281f29__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge test_copper_lists.clk)", 
                                                         "test_copper_lists.sv", 
                                                         46);
    vlSelfRef.test_copper_lists__DOT__cpu_we = 0U;
    vlSelfRef.test_copper_lists__DOT__cpu_ce = 0U;
    __Vtask_test_copper_lists__DOT__write_param__44__data 
        = (0x000000ffU & __Vtask_test_copper_lists__DOT__copper_add__37__reg_idx);
    __Vtask_test_copper_lists__DOT__write_param__44__idx = 3U;
    __Vtask_test_copper_lists__DOT__write_reg__45__data 
        = __Vtask_test_copper_lists__DOT__write_param__44__data;
    __Vtask_test_copper_lists__DOT__write_reg__45__addr 
        = (0x0000ffffU & ((IData)(0xa011U) + __Vtask_test_copper_lists__DOT__write_param__44__idx));
    Vtest_copper_lists___024root____VbeforeTrig_h76281f29__0(vlSelf, 
                                                             "@(posedge test_copper_lists.clk)");
    co_await vlSelfRef.__VtrigSched_h76281f29__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge test_copper_lists.clk)", 
                                                         "test_copper_lists.sv", 
                                                         44);
    vlSelfRef.test_copper_lists__DOT__cpu_addr = __Vtask_test_copper_lists__DOT__write_reg__45__addr;
    vlSelfRef.test_copper_lists__DOT__cpu_wdata = __Vtask_test_copper_lists__DOT__write_reg__45__data;
    vlSelfRef.test_copper_lists__DOT__cpu_we = 1U;
    vlSelfRef.test_copper_lists__DOT__cpu_ce = 1U;
    Vtest_copper_lists___024root____VbeforeTrig_h76281f29__0(vlSelf, 
                                                             "@(posedge test_copper_lists.clk)");
    co_await vlSelfRef.__VtrigSched_h76281f29__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge test_copper_lists.clk)", 
                                                         "test_copper_lists.sv", 
                                                         46);
    vlSelfRef.test_copper_lists__DOT__cpu_we = 0U;
    vlSelfRef.test_copper_lists__DOT__cpu_ce = 0U;
    __Vtask_test_copper_lists__DOT__write_param__46__data 
        = (0x000000ffU & (__Vtask_test_copper_lists__DOT__copper_add__37__reg_idx 
                          >> 8U));
    __Vtask_test_copper_lists__DOT__write_param__46__idx = 4U;
    __Vtask_test_copper_lists__DOT__write_reg__47__data 
        = __Vtask_test_copper_lists__DOT__write_param__46__data;
    __Vtask_test_copper_lists__DOT__write_reg__47__addr 
        = (0x0000ffffU & ((IData)(0xa011U) + __Vtask_test_copper_lists__DOT__write_param__46__idx));
    Vtest_copper_lists___024root____VbeforeTrig_h76281f29__0(vlSelf, 
                                                             "@(posedge test_copper_lists.clk)");
    co_await vlSelfRef.__VtrigSched_h76281f29__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge test_copper_lists.clk)", 
                                                         "test_copper_lists.sv", 
                                                         44);
    vlSelfRef.test_copper_lists__DOT__cpu_addr = __Vtask_test_copper_lists__DOT__write_reg__47__addr;
    vlSelfRef.test_copper_lists__DOT__cpu_wdata = __Vtask_test_copper_lists__DOT__write_reg__47__data;
    vlSelfRef.test_copper_lists__DOT__cpu_we = 1U;
    vlSelfRef.test_copper_lists__DOT__cpu_ce = 1U;
    Vtest_copper_lists___024root____VbeforeTrig_h76281f29__0(vlSelf, 
                                                             "@(posedge test_copper_lists.clk)");
    co_await vlSelfRef.__VtrigSched_h76281f29__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge test_copper_lists.clk)", 
                                                         "test_copper_lists.sv", 
                                                         46);
    vlSelfRef.test_copper_lists__DOT__cpu_we = 0U;
    vlSelfRef.test_copper_lists__DOT__cpu_ce = 0U;
    __Vtask_test_copper_lists__DOT__write_param__48__data 
        = __Vtask_test_copper_lists__DOT__copper_add__37__value;
    __Vtask_test_copper_lists__DOT__write_param__48__idx = 5U;
    __Vtask_test_copper_lists__DOT__write_reg__49__data 
        = __Vtask_test_copper_lists__DOT__write_param__48__data;
    __Vtask_test_copper_lists__DOT__write_reg__49__addr 
        = (0x0000ffffU & ((IData)(0xa011U) + __Vtask_test_copper_lists__DOT__write_param__48__idx));
    Vtest_copper_lists___024root____VbeforeTrig_h76281f29__0(vlSelf, 
                                                             "@(posedge test_copper_lists.clk)");
    co_await vlSelfRef.__VtrigSched_h76281f29__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge test_copper_lists.clk)", 
                                                         "test_copper_lists.sv", 
                                                         44);
    vlSelfRef.test_copper_lists__DOT__cpu_addr = __Vtask_test_copper_lists__DOT__write_reg__49__addr;
    vlSelfRef.test_copper_lists__DOT__cpu_wdata = __Vtask_test_copper_lists__DOT__write_reg__49__data;
    vlSelfRef.test_copper_lists__DOT__cpu_we = 1U;
    vlSelfRef.test_copper_lists__DOT__cpu_ce = 1U;
    Vtest_copper_lists___024root____VbeforeTrig_h76281f29__0(vlSelf, 
                                                             "@(posedge test_copper_lists.clk)");
    co_await vlSelfRef.__VtrigSched_h76281f29__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge test_copper_lists.clk)", 
                                                         "test_copper_lists.sv", 
                                                         46);
    vlSelfRef.test_copper_lists__DOT__cpu_we = 0U;
    vlSelfRef.test_copper_lists__DOT__cpu_ce = 0U;
    __Vtask_test_copper_lists__DOT__write_cmd__50__cmd = 0x1bU;
    __Vtask_test_copper_lists__DOT__write_reg__51__data 
        = __Vtask_test_copper_lists__DOT__write_cmd__50__cmd;
    __Vtask_test_copper_lists__DOT__write_reg__51__addr = 0xa010U;
    Vtest_copper_lists___024root____VbeforeTrig_h76281f29__0(vlSelf, 
                                                             "@(posedge test_copper_lists.clk)");
    co_await vlSelfRef.__VtrigSched_h76281f29__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge test_copper_lists.clk)", 
                                                         "test_copper_lists.sv", 
                                                         44);
    vlSelfRef.test_copper_lists__DOT__cpu_addr = __Vtask_test_copper_lists__DOT__write_reg__51__addr;
    vlSelfRef.test_copper_lists__DOT__cpu_wdata = __Vtask_test_copper_lists__DOT__write_reg__51__data;
    vlSelfRef.test_copper_lists__DOT__cpu_we = 1U;
    vlSelfRef.test_copper_lists__DOT__cpu_ce = 1U;
    Vtest_copper_lists___024root____VbeforeTrig_h76281f29__0(vlSelf, 
                                                             "@(posedge test_copper_lists.clk)");
    co_await vlSelfRef.__VtrigSched_h76281f29__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge test_copper_lists.clk)", 
                                                         "test_copper_lists.sv", 
                                                         46);
    vlSelfRef.test_copper_lists__DOT__cpu_we = 0U;
    vlSelfRef.test_copper_lists__DOT__cpu_ce = 0U;
    __Vtask_test_copper_lists__DOT__run_clocks__52__n = 2U;
    __Vtask_test_copper_lists__DOT__run_clocks__52__test_copper_lists__DOT__unnamedblk1_1__DOT____Vrepeat0 = 0;
    __Vtask_test_copper_lists__DOT__run_clocks__52__test_copper_lists__DOT__unnamedblk1_1__DOT____Vrepeat0 
        = __Vtask_test_copper_lists__DOT__run_clocks__52__n;
    while (VL_LTS_III(32, 0U, __Vtask_test_copper_lists__DOT__run_clocks__52__test_copper_lists__DOT__unnamedblk1_1__DOT____Vrepeat0)) {
        Vtest_copper_lists___024root____VbeforeTrig_h76281f29__0(vlSelf, 
                                                                 "@(posedge test_copper_lists.clk)");
        co_await vlSelfRef.__VtrigSched_h76281f29__0.trigger(0U, 
                                                             nullptr, 
                                                             "@(posedge test_copper_lists.clk)", 
                                                             "test_copper_lists.sv", 
                                                             88);
        __Vtask_test_copper_lists__DOT__run_clocks__52__test_copper_lists__DOT__unnamedblk1_1__DOT____Vrepeat0 
            = (__Vtask_test_copper_lists__DOT__run_clocks__52__test_copper_lists__DOT__unnamedblk1_1__DOT____Vrepeat0 
               - (IData)(1U));
    }
    __Vtask_test_copper_lists__DOT__check__53__condition 
        = (1U == vlSelfRef.test_copper_lists__DOT__dut__DOT__copper_list_count[0U]);
    vlSelfRef.__Vtask_test_copper_lists__DOT__check__53__name = "list 0 count is 1"s;
    vlSelfRef.test_copper_lists__DOT__test_num = ((IData)(1U) 
                                                  + vlSelfRef.test_copper_lists__DOT__test_num);
    if (__Vtask_test_copper_lists__DOT__check__53__condition) {
        VL_WRITEF_NX("  PASS [%0d] %@\n",0,32,vlSelfRef.test_copper_lists__DOT__test_num,
                     -1,&(vlSelfRef.__Vtask_test_copper_lists__DOT__check__53__name));
        vlSelfRef.test_copper_lists__DOT__pass_count 
            = ((IData)(1U) + vlSelfRef.test_copper_lists__DOT__pass_count);
    } else {
        VL_WRITEF_NX("  FAIL [%0d] %@\n",0,32,vlSelfRef.test_copper_lists__DOT__test_num,
                     -1,&(vlSelfRef.__Vtask_test_copper_lists__DOT__check__53__name));
        vlSelfRef.test_copper_lists__DOT__fail_count 
            = ((IData)(1U) + vlSelfRef.test_copper_lists__DOT__fail_count);
    }
    __Vtask_test_copper_lists__DOT__check__54__condition 
        = (1U == (IData)(vlSelfRef.test_copper_lists__DOT__dut__DOT__copper_count));
    vlSelfRef.__Vtask_test_copper_lists__DOT__check__54__name = "active arrays also updated"s;
    vlSelfRef.test_copper_lists__DOT__test_num = ((IData)(1U) 
                                                  + vlSelfRef.test_copper_lists__DOT__test_num);
    if (__Vtask_test_copper_lists__DOT__check__54__condition) {
        VL_WRITEF_NX("  PASS [%0d] %@\n",0,32,vlSelfRef.test_copper_lists__DOT__test_num,
                     -1,&(vlSelfRef.__Vtask_test_copper_lists__DOT__check__54__name));
        vlSelfRef.test_copper_lists__DOT__pass_count 
            = ((IData)(1U) + vlSelfRef.test_copper_lists__DOT__pass_count);
    } else {
        VL_WRITEF_NX("  FAIL [%0d] %@\n",0,32,vlSelfRef.test_copper_lists__DOT__test_num,
                     -1,&(vlSelfRef.__Vtask_test_copper_lists__DOT__check__54__name));
        vlSelfRef.test_copper_lists__DOT__fail_count 
            = ((IData)(1U) + vlSelfRef.test_copper_lists__DOT__fail_count);
    }
    VL_WRITEF_NX("Test: CopperUse command\n",0);
    __Vtask_test_copper_lists__DOT__copper_use__55__idx = 3U;
    __Vtask_test_copper_lists__DOT__write_param__56__data 
        = (0x000000ffU & __Vtask_test_copper_lists__DOT__copper_use__55__idx);
    __Vtask_test_copper_lists__DOT__write_param__56__idx = 0U;
    __Vtask_test_copper_lists__DOT__write_reg__57__data 
        = __Vtask_test_copper_lists__DOT__write_param__56__data;
    __Vtask_test_copper_lists__DOT__write_reg__57__addr 
        = (0x0000ffffU & ((IData)(0xa011U) + __Vtask_test_copper_lists__DOT__write_param__56__idx));
    Vtest_copper_lists___024root____VbeforeTrig_h76281f29__0(vlSelf, 
                                                             "@(posedge test_copper_lists.clk)");
    co_await vlSelfRef.__VtrigSched_h76281f29__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge test_copper_lists.clk)", 
                                                         "test_copper_lists.sv", 
                                                         44);
    vlSelfRef.test_copper_lists__DOT__cpu_addr = __Vtask_test_copper_lists__DOT__write_reg__57__addr;
    vlSelfRef.test_copper_lists__DOT__cpu_wdata = __Vtask_test_copper_lists__DOT__write_reg__57__data;
    vlSelfRef.test_copper_lists__DOT__cpu_we = 1U;
    vlSelfRef.test_copper_lists__DOT__cpu_ce = 1U;
    Vtest_copper_lists___024root____VbeforeTrig_h76281f29__0(vlSelf, 
                                                             "@(posedge test_copper_lists.clk)");
    co_await vlSelfRef.__VtrigSched_h76281f29__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge test_copper_lists.clk)", 
                                                         "test_copper_lists.sv", 
                                                         46);
    vlSelfRef.test_copper_lists__DOT__cpu_we = 0U;
    vlSelfRef.test_copper_lists__DOT__cpu_ce = 0U;
    __Vtask_test_copper_lists__DOT__write_cmd__58__cmd = 0x21U;
    __Vtask_test_copper_lists__DOT__write_reg__59__data 
        = __Vtask_test_copper_lists__DOT__write_cmd__58__cmd;
    __Vtask_test_copper_lists__DOT__write_reg__59__addr = 0xa010U;
    Vtest_copper_lists___024root____VbeforeTrig_h76281f29__0(vlSelf, 
                                                             "@(posedge test_copper_lists.clk)");
    co_await vlSelfRef.__VtrigSched_h76281f29__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge test_copper_lists.clk)", 
                                                         "test_copper_lists.sv", 
                                                         44);
    vlSelfRef.test_copper_lists__DOT__cpu_addr = __Vtask_test_copper_lists__DOT__write_reg__59__addr;
    vlSelfRef.test_copper_lists__DOT__cpu_wdata = __Vtask_test_copper_lists__DOT__write_reg__59__data;
    vlSelfRef.test_copper_lists__DOT__cpu_we = 1U;
    vlSelfRef.test_copper_lists__DOT__cpu_ce = 1U;
    Vtest_copper_lists___024root____VbeforeTrig_h76281f29__0(vlSelf, 
                                                             "@(posedge test_copper_lists.clk)");
    co_await vlSelfRef.__VtrigSched_h76281f29__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge test_copper_lists.clk)", 
                                                         "test_copper_lists.sv", 
                                                         46);
    vlSelfRef.test_copper_lists__DOT__cpu_we = 0U;
    vlSelfRef.test_copper_lists__DOT__cpu_ce = 0U;
    __Vtask_test_copper_lists__DOT__run_clocks__60__n = 2U;
    __Vtask_test_copper_lists__DOT__run_clocks__60__test_copper_lists__DOT__unnamedblk1_1__DOT____Vrepeat0 = 0;
    __Vtask_test_copper_lists__DOT__run_clocks__60__test_copper_lists__DOT__unnamedblk1_1__DOT____Vrepeat0 
        = __Vtask_test_copper_lists__DOT__run_clocks__60__n;
    while (VL_LTS_III(32, 0U, __Vtask_test_copper_lists__DOT__run_clocks__60__test_copper_lists__DOT__unnamedblk1_1__DOT____Vrepeat0)) {
        Vtest_copper_lists___024root____VbeforeTrig_h76281f29__0(vlSelf, 
                                                                 "@(posedge test_copper_lists.clk)");
        co_await vlSelfRef.__VtrigSched_h76281f29__0.trigger(0U, 
                                                             nullptr, 
                                                             "@(posedge test_copper_lists.clk)", 
                                                             "test_copper_lists.sv", 
                                                             88);
        __Vtask_test_copper_lists__DOT__run_clocks__60__test_copper_lists__DOT__unnamedblk1_1__DOT____Vrepeat0 
            = (__Vtask_test_copper_lists__DOT__run_clocks__60__test_copper_lists__DOT__unnamedblk1_1__DOT____Vrepeat0 
               - (IData)(1U));
    }
    __Vtask_test_copper_lists__DOT__check__61__condition 
        = (3U == (IData)(vlSelfRef.test_copper_lists__DOT__dut__DOT__copper_pending_list));
    vlSelfRef.__Vtask_test_copper_lists__DOT__check__61__name = "pending list set to 3"s;
    vlSelfRef.test_copper_lists__DOT__test_num = ((IData)(1U) 
                                                  + vlSelfRef.test_copper_lists__DOT__test_num);
    if (__Vtask_test_copper_lists__DOT__check__61__condition) {
        VL_WRITEF_NX("  PASS [%0d] %@\n",0,32,vlSelfRef.test_copper_lists__DOT__test_num,
                     -1,&(vlSelfRef.__Vtask_test_copper_lists__DOT__check__61__name));
        vlSelfRef.test_copper_lists__DOT__pass_count 
            = ((IData)(1U) + vlSelfRef.test_copper_lists__DOT__pass_count);
    } else {
        VL_WRITEF_NX("  FAIL [%0d] %@\n",0,32,vlSelfRef.test_copper_lists__DOT__test_num,
                     -1,&(vlSelfRef.__Vtask_test_copper_lists__DOT__check__61__name));
        vlSelfRef.test_copper_lists__DOT__fail_count 
            = ((IData)(1U) + vlSelfRef.test_copper_lists__DOT__fail_count);
    }
    __Vtask_test_copper_lists__DOT__check__62__condition 
        = (0U == (IData)(vlSelfRef.test_copper_lists__DOT__dut__DOT__copper_active_list));
    vlSelfRef.__Vtask_test_copper_lists__DOT__check__62__name = "active list NOT yet changed"s;
    vlSelfRef.test_copper_lists__DOT__test_num = ((IData)(1U) 
                                                  + vlSelfRef.test_copper_lists__DOT__test_num);
    if (__Vtask_test_copper_lists__DOT__check__62__condition) {
        VL_WRITEF_NX("  PASS [%0d] %@\n",0,32,vlSelfRef.test_copper_lists__DOT__test_num,
                     -1,&(vlSelfRef.__Vtask_test_copper_lists__DOT__check__62__name));
        vlSelfRef.test_copper_lists__DOT__pass_count 
            = ((IData)(1U) + vlSelfRef.test_copper_lists__DOT__pass_count);
    } else {
        VL_WRITEF_NX("  FAIL [%0d] %@\n",0,32,vlSelfRef.test_copper_lists__DOT__test_num,
                     -1,&(vlSelfRef.__Vtask_test_copper_lists__DOT__check__62__name));
        vlSelfRef.test_copper_lists__DOT__fail_count 
            = ((IData)(1U) + vlSelfRef.test_copper_lists__DOT__fail_count);
    }
    VL_WRITEF_NX("Test: Vblank list swap\n",0);
    __Vtask_test_copper_lists__DOT__write_cmd__64__cmd = 0x1dU;
    __Vtask_test_copper_lists__DOT__write_reg__65__data 
        = __Vtask_test_copper_lists__DOT__write_cmd__64__cmd;
    __Vtask_test_copper_lists__DOT__write_reg__65__addr = 0xa010U;
    Vtest_copper_lists___024root____VbeforeTrig_h76281f29__0(vlSelf, 
                                                             "@(posedge test_copper_lists.clk)");
    co_await vlSelfRef.__VtrigSched_h76281f29__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge test_copper_lists.clk)", 
                                                         "test_copper_lists.sv", 
                                                         44);
    vlSelfRef.test_copper_lists__DOT__cpu_addr = __Vtask_test_copper_lists__DOT__write_reg__65__addr;
    vlSelfRef.test_copper_lists__DOT__cpu_wdata = __Vtask_test_copper_lists__DOT__write_reg__65__data;
    vlSelfRef.test_copper_lists__DOT__cpu_we = 1U;
    vlSelfRef.test_copper_lists__DOT__cpu_ce = 1U;
    Vtest_copper_lists___024root____VbeforeTrig_h76281f29__0(vlSelf, 
                                                             "@(posedge test_copper_lists.clk)");
    co_await vlSelfRef.__VtrigSched_h76281f29__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge test_copper_lists.clk)", 
                                                         "test_copper_lists.sv", 
                                                         46);
    vlSelfRef.test_copper_lists__DOT__cpu_we = 0U;
    vlSelfRef.test_copper_lists__DOT__cpu_ce = 0U;
    __Vtask_test_copper_lists__DOT__run_clocks__66__n = 2U;
    __Vtask_test_copper_lists__DOT__run_clocks__66__test_copper_lists__DOT__unnamedblk1_1__DOT____Vrepeat0 = 0;
    __Vtask_test_copper_lists__DOT__run_clocks__66__test_copper_lists__DOT__unnamedblk1_1__DOT____Vrepeat0 
        = __Vtask_test_copper_lists__DOT__run_clocks__66__n;
    while (VL_LTS_III(32, 0U, __Vtask_test_copper_lists__DOT__run_clocks__66__test_copper_lists__DOT__unnamedblk1_1__DOT____Vrepeat0)) {
        Vtest_copper_lists___024root____VbeforeTrig_h76281f29__0(vlSelf, 
                                                                 "@(posedge test_copper_lists.clk)");
        co_await vlSelfRef.__VtrigSched_h76281f29__0.trigger(0U, 
                                                             nullptr, 
                                                             "@(posedge test_copper_lists.clk)", 
                                                             "test_copper_lists.sv", 
                                                             88);
        __Vtask_test_copper_lists__DOT__run_clocks__66__test_copper_lists__DOT__unnamedblk1_1__DOT____Vrepeat0 
            = (__Vtask_test_copper_lists__DOT__run_clocks__66__test_copper_lists__DOT__unnamedblk1_1__DOT____Vrepeat0 
               - (IData)(1U));
    }
    __Vtask_test_copper_lists__DOT__write_reg__67__data = 6U;
    __Vtask_test_copper_lists__DOT__write_reg__67__addr = 0xa001U;
    Vtest_copper_lists___024root____VbeforeTrig_h76281f29__0(vlSelf, 
                                                             "@(posedge test_copper_lists.clk)");
    co_await vlSelfRef.__VtrigSched_h76281f29__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge test_copper_lists.clk)", 
                                                         "test_copper_lists.sv", 
                                                         44);
    vlSelfRef.test_copper_lists__DOT__cpu_addr = __Vtask_test_copper_lists__DOT__write_reg__67__addr;
    vlSelfRef.test_copper_lists__DOT__cpu_wdata = __Vtask_test_copper_lists__DOT__write_reg__67__data;
    vlSelfRef.test_copper_lists__DOT__cpu_we = 1U;
    vlSelfRef.test_copper_lists__DOT__cpu_ce = 1U;
    Vtest_copper_lists___024root____VbeforeTrig_h76281f29__0(vlSelf, 
                                                             "@(posedge test_copper_lists.clk)");
    co_await vlSelfRef.__VtrigSched_h76281f29__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge test_copper_lists.clk)", 
                                                         "test_copper_lists.sv", 
                                                         46);
    vlSelfRef.test_copper_lists__DOT__cpu_we = 0U;
    vlSelfRef.test_copper_lists__DOT__cpu_ce = 0U;
    __Vtask_test_copper_lists__DOT__run_clocks__68__n = 2U;
    __Vtask_test_copper_lists__DOT__run_clocks__68__test_copper_lists__DOT__unnamedblk1_1__DOT____Vrepeat0 = 0;
    __Vtask_test_copper_lists__DOT__run_clocks__68__test_copper_lists__DOT__unnamedblk1_1__DOT____Vrepeat0 
        = __Vtask_test_copper_lists__DOT__run_clocks__68__n;
    while (VL_LTS_III(32, 0U, __Vtask_test_copper_lists__DOT__run_clocks__68__test_copper_lists__DOT__unnamedblk1_1__DOT____Vrepeat0)) {
        Vtest_copper_lists___024root____VbeforeTrig_h76281f29__0(vlSelf, 
                                                                 "@(posedge test_copper_lists.clk)");
        co_await vlSelfRef.__VtrigSched_h76281f29__0.trigger(0U, 
                                                             nullptr, 
                                                             "@(posedge test_copper_lists.clk)", 
                                                             "test_copper_lists.sv", 
                                                             88);
        __Vtask_test_copper_lists__DOT__run_clocks__68__test_copper_lists__DOT__unnamedblk1_1__DOT____Vrepeat0 
            = (__Vtask_test_copper_lists__DOT__run_clocks__68__test_copper_lists__DOT__unnamedblk1_1__DOT____Vrepeat0 
               - (IData)(1U));
    }
    __Vtask_test_copper_lists__DOT__run_to_scanline__69__target = 0x000001e0U;
    while ((((IData)(vlSelfRef.test_copper_lists__DOT__dut__DOT__v_count) 
             != __Vtask_test_copper_lists__DOT__run_to_scanline__69__target) 
            | (0U != (IData)(vlSelfRef.test_copper_lists__DOT__dut__DOT__h_count)))) {
        Vtest_copper_lists___024root____VbeforeTrig_h76281f29__0(vlSelf, 
                                                                 "@(posedge test_copper_lists.clk)");
        co_await vlSelfRef.__VtrigSched_h76281f29__0.trigger(0U, 
                                                             nullptr, 
                                                             "@(posedge test_copper_lists.clk)", 
                                                             "test_copper_lists.sv", 
                                                             93);
    }
    __Vtask_test_copper_lists__DOT__run_clocks__70__n = 0x0000000aU;
    __Vtask_test_copper_lists__DOT__run_clocks__70__test_copper_lists__DOT__unnamedblk1_1__DOT____Vrepeat0 = 0;
    __Vtask_test_copper_lists__DOT__run_clocks__70__test_copper_lists__DOT__unnamedblk1_1__DOT____Vrepeat0 
        = __Vtask_test_copper_lists__DOT__run_clocks__70__n;
    while (VL_LTS_III(32, 0U, __Vtask_test_copper_lists__DOT__run_clocks__70__test_copper_lists__DOT__unnamedblk1_1__DOT____Vrepeat0)) {
        Vtest_copper_lists___024root____VbeforeTrig_h76281f29__0(vlSelf, 
                                                                 "@(posedge test_copper_lists.clk)");
        co_await vlSelfRef.__VtrigSched_h76281f29__0.trigger(0U, 
                                                             nullptr, 
                                                             "@(posedge test_copper_lists.clk)", 
                                                             "test_copper_lists.sv", 
                                                             88);
        __Vtask_test_copper_lists__DOT__run_clocks__70__test_copper_lists__DOT__unnamedblk1_1__DOT____Vrepeat0 
            = (__Vtask_test_copper_lists__DOT__run_clocks__70__test_copper_lists__DOT__unnamedblk1_1__DOT____Vrepeat0 
               - (IData)(1U));
    }
    __Vtask_test_copper_lists__DOT__check__71__condition 
        = (3U == (IData)(vlSelfRef.test_copper_lists__DOT__dut__DOT__copper_active_list));
    vlSelfRef.__Vtask_test_copper_lists__DOT__check__71__name = "active list swapped to 3 at vblank"s;
    vlSelfRef.test_copper_lists__DOT__test_num = ((IData)(1U) 
                                                  + vlSelfRef.test_copper_lists__DOT__test_num);
    if (__Vtask_test_copper_lists__DOT__check__71__condition) {
        VL_WRITEF_NX("  PASS [%0d] %@\n",0,32,vlSelfRef.test_copper_lists__DOT__test_num,
                     -1,&(vlSelfRef.__Vtask_test_copper_lists__DOT__check__71__name));
        vlSelfRef.test_copper_lists__DOT__pass_count 
            = ((IData)(1U) + vlSelfRef.test_copper_lists__DOT__pass_count);
    } else {
        VL_WRITEF_NX("  FAIL [%0d] %@\n",0,32,vlSelfRef.test_copper_lists__DOT__test_num,
                     -1,&(vlSelfRef.__Vtask_test_copper_lists__DOT__check__71__name));
        vlSelfRef.test_copper_lists__DOT__fail_count 
            = ((IData)(1U) + vlSelfRef.test_copper_lists__DOT__fail_count);
    }
    while (vlSelfRef.test_copper_lists__DOT__dut__DOT__copper_loading) {
        Vtest_copper_lists___024root____VbeforeTrig_h76281f29__0(vlSelf, 
                                                                 "@(posedge test_copper_lists.clk)");
        co_await vlSelfRef.__VtrigSched_h76281f29__0.trigger(0U, 
                                                             nullptr, 
                                                             "@(posedge test_copper_lists.clk)", 
                                                             "test_copper_lists.sv", 
                                                             178);
    }
    __Vtask_test_copper_lists__DOT__run_clocks__72__n = 5U;
    __Vtask_test_copper_lists__DOT__run_clocks__72__test_copper_lists__DOT__unnamedblk1_1__DOT____Vrepeat0 = 0;
    __Vtask_test_copper_lists__DOT__run_clocks__72__test_copper_lists__DOT__unnamedblk1_1__DOT____Vrepeat0 
        = __Vtask_test_copper_lists__DOT__run_clocks__72__n;
    while (VL_LTS_III(32, 0U, __Vtask_test_copper_lists__DOT__run_clocks__72__test_copper_lists__DOT__unnamedblk1_1__DOT____Vrepeat0)) {
        Vtest_copper_lists___024root____VbeforeTrig_h76281f29__0(vlSelf, 
                                                                 "@(posedge test_copper_lists.clk)");
        co_await vlSelfRef.__VtrigSched_h76281f29__0.trigger(0U, 
                                                             nullptr, 
                                                             "@(posedge test_copper_lists.clk)", 
                                                             "test_copper_lists.sv", 
                                                             88);
        __Vtask_test_copper_lists__DOT__run_clocks__72__test_copper_lists__DOT__unnamedblk1_1__DOT____Vrepeat0 
            = (__Vtask_test_copper_lists__DOT__run_clocks__72__test_copper_lists__DOT__unnamedblk1_1__DOT____Vrepeat0 
               - (IData)(1U));
    }
    __Vtask_test_copper_lists__DOT__check__73__condition 
        = ((IData)(vlSelfRef.test_copper_lists__DOT__dut__DOT__copper_count) 
           == vlSelfRef.test_copper_lists__DOT__dut__DOT__copper_list_count[3U]);
    vlSelfRef.__Vtask_test_copper_lists__DOT__check__73__name = "active count matches list 3"s;
    vlSelfRef.test_copper_lists__DOT__test_num = ((IData)(1U) 
                                                  + vlSelfRef.test_copper_lists__DOT__test_num);
    if (__Vtask_test_copper_lists__DOT__check__73__condition) {
        VL_WRITEF_NX("  PASS [%0d] %@\n",0,32,vlSelfRef.test_copper_lists__DOT__test_num,
                     -1,&(vlSelfRef.__Vtask_test_copper_lists__DOT__check__73__name));
        vlSelfRef.test_copper_lists__DOT__pass_count 
            = ((IData)(1U) + vlSelfRef.test_copper_lists__DOT__pass_count);
    } else {
        VL_WRITEF_NX("  FAIL [%0d] %@\n",0,32,vlSelfRef.test_copper_lists__DOT__test_num,
                     -1,&(vlSelfRef.__Vtask_test_copper_lists__DOT__check__73__name));
        vlSelfRef.test_copper_lists__DOT__fail_count 
            = ((IData)(1U) + vlSelfRef.test_copper_lists__DOT__fail_count);
    }
    VL_WRITEF_NX("Test: Swapped list fires\n",0);
    __Vtask_test_copper_lists__DOT__write_reg__74__data = 6U;
    __Vtask_test_copper_lists__DOT__write_reg__74__addr = 0xa001U;
    Vtest_copper_lists___024root____VbeforeTrig_h76281f29__0(vlSelf, 
                                                             "@(posedge test_copper_lists.clk)");
    co_await vlSelfRef.__VtrigSched_h76281f29__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge test_copper_lists.clk)", 
                                                         "test_copper_lists.sv", 
                                                         44);
    vlSelfRef.test_copper_lists__DOT__cpu_addr = __Vtask_test_copper_lists__DOT__write_reg__74__addr;
    vlSelfRef.test_copper_lists__DOT__cpu_wdata = __Vtask_test_copper_lists__DOT__write_reg__74__data;
    vlSelfRef.test_copper_lists__DOT__cpu_we = 1U;
    vlSelfRef.test_copper_lists__DOT__cpu_ce = 1U;
    Vtest_copper_lists___024root____VbeforeTrig_h76281f29__0(vlSelf, 
                                                             "@(posedge test_copper_lists.clk)");
    co_await vlSelfRef.__VtrigSched_h76281f29__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge test_copper_lists.clk)", 
                                                         "test_copper_lists.sv", 
                                                         46);
    vlSelfRef.test_copper_lists__DOT__cpu_we = 0U;
    vlSelfRef.test_copper_lists__DOT__cpu_ce = 0U;
    __Vtask_test_copper_lists__DOT__run_clocks__75__n = 2U;
    __Vtask_test_copper_lists__DOT__run_clocks__75__test_copper_lists__DOT__unnamedblk1_1__DOT____Vrepeat0 = 0;
    __Vtask_test_copper_lists__DOT__run_clocks__75__test_copper_lists__DOT__unnamedblk1_1__DOT____Vrepeat0 
        = __Vtask_test_copper_lists__DOT__run_clocks__75__n;
    while (VL_LTS_III(32, 0U, __Vtask_test_copper_lists__DOT__run_clocks__75__test_copper_lists__DOT__unnamedblk1_1__DOT____Vrepeat0)) {
        Vtest_copper_lists___024root____VbeforeTrig_h76281f29__0(vlSelf, 
                                                                 "@(posedge test_copper_lists.clk)");
        co_await vlSelfRef.__VtrigSched_h76281f29__0.trigger(0U, 
                                                             nullptr, 
                                                             "@(posedge test_copper_lists.clk)", 
                                                             "test_copper_lists.sv", 
                                                             88);
        __Vtask_test_copper_lists__DOT__run_clocks__75__test_copper_lists__DOT__unnamedblk1_1__DOT____Vrepeat0 
            = (__Vtask_test_copper_lists__DOT__run_clocks__75__test_copper_lists__DOT__unnamedblk1_1__DOT____Vrepeat0 
               - (IData)(1U));
    }
    __Vtask_test_copper_lists__DOT__run_to_scanline__76__target = 0U;
    while ((((IData)(vlSelfRef.test_copper_lists__DOT__dut__DOT__v_count) 
             != __Vtask_test_copper_lists__DOT__run_to_scanline__76__target) 
            | (0U != (IData)(vlSelfRef.test_copper_lists__DOT__dut__DOT__h_count)))) {
        Vtest_copper_lists___024root____VbeforeTrig_h76281f29__0(vlSelf, 
                                                                 "@(posedge test_copper_lists.clk)");
        co_await vlSelfRef.__VtrigSched_h76281f29__0.trigger(0U, 
                                                             nullptr, 
                                                             "@(posedge test_copper_lists.clk)", 
                                                             "test_copper_lists.sv", 
                                                             93);
    }
    __Vtask_test_copper_lists__DOT__run_to_scanline__77__target = 0x0000008dU;
    while ((((IData)(vlSelfRef.test_copper_lists__DOT__dut__DOT__v_count) 
             != __Vtask_test_copper_lists__DOT__run_to_scanline__77__target) 
            | (0U != (IData)(vlSelfRef.test_copper_lists__DOT__dut__DOT__h_count)))) {
        Vtest_copper_lists___024root____VbeforeTrig_h76281f29__0(vlSelf, 
                                                                 "@(posedge test_copper_lists.clk)");
        co_await vlSelfRef.__VtrigSched_h76281f29__0.trigger(0U, 
                                                             nullptr, 
                                                             "@(posedge test_copper_lists.clk)", 
                                                             "test_copper_lists.sv", 
                                                             93);
    }
    __Vtask_test_copper_lists__DOT__check__78__condition 
        = (5U == (IData)(vlSelfRef.test_copper_lists__DOT__dut__DOT__bg_color));
    vlSelfRef.__Vtask_test_copper_lists__DOT__check__78__name = "bg changed to 5 (green) from list 3"s;
    vlSelfRef.test_copper_lists__DOT__test_num = ((IData)(1U) 
                                                  + vlSelfRef.test_copper_lists__DOT__test_num);
    if (__Vtask_test_copper_lists__DOT__check__78__condition) {
        VL_WRITEF_NX("  PASS [%0d] %@\n",0,32,vlSelfRef.test_copper_lists__DOT__test_num,
                     -1,&(vlSelfRef.__Vtask_test_copper_lists__DOT__check__78__name));
        vlSelfRef.test_copper_lists__DOT__pass_count 
            = ((IData)(1U) + vlSelfRef.test_copper_lists__DOT__pass_count);
    } else {
        VL_WRITEF_NX("  FAIL [%0d] %@\n",0,32,vlSelfRef.test_copper_lists__DOT__test_num,
                     -1,&(vlSelfRef.__Vtask_test_copper_lists__DOT__check__78__name));
        vlSelfRef.test_copper_lists__DOT__fail_count 
            = ((IData)(1U) + vlSelfRef.test_copper_lists__DOT__fail_count);
    }
    VL_WRITEF_NX("Test: CopperListEnd\n",0);
    __Vtask_test_copper_lists__DOT__copper_list__79__idx = 7U;
    __Vtask_test_copper_lists__DOT__write_param__80__data 
        = (0x000000ffU & __Vtask_test_copper_lists__DOT__copper_list__79__idx);
    __Vtask_test_copper_lists__DOT__write_param__80__idx = 0U;
    __Vtask_test_copper_lists__DOT__write_reg__81__data 
        = __Vtask_test_copper_lists__DOT__write_param__80__data;
    __Vtask_test_copper_lists__DOT__write_reg__81__addr 
        = (0x0000ffffU & ((IData)(0xa011U) + __Vtask_test_copper_lists__DOT__write_param__80__idx));
    Vtest_copper_lists___024root____VbeforeTrig_h76281f29__0(vlSelf, 
                                                             "@(posedge test_copper_lists.clk)");
    co_await vlSelfRef.__VtrigSched_h76281f29__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge test_copper_lists.clk)", 
                                                         "test_copper_lists.sv", 
                                                         44);
    vlSelfRef.test_copper_lists__DOT__cpu_addr = __Vtask_test_copper_lists__DOT__write_reg__81__addr;
    vlSelfRef.test_copper_lists__DOT__cpu_wdata = __Vtask_test_copper_lists__DOT__write_reg__81__data;
    vlSelfRef.test_copper_lists__DOT__cpu_we = 1U;
    vlSelfRef.test_copper_lists__DOT__cpu_ce = 1U;
    Vtest_copper_lists___024root____VbeforeTrig_h76281f29__0(vlSelf, 
                                                             "@(posedge test_copper_lists.clk)");
    co_await vlSelfRef.__VtrigSched_h76281f29__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge test_copper_lists.clk)", 
                                                         "test_copper_lists.sv", 
                                                         46);
    vlSelfRef.test_copper_lists__DOT__cpu_we = 0U;
    vlSelfRef.test_copper_lists__DOT__cpu_ce = 0U;
    __Vtask_test_copper_lists__DOT__write_cmd__82__cmd = 0x20U;
    __Vtask_test_copper_lists__DOT__write_reg__83__data 
        = __Vtask_test_copper_lists__DOT__write_cmd__82__cmd;
    __Vtask_test_copper_lists__DOT__write_reg__83__addr = 0xa010U;
    Vtest_copper_lists___024root____VbeforeTrig_h76281f29__0(vlSelf, 
                                                             "@(posedge test_copper_lists.clk)");
    co_await vlSelfRef.__VtrigSched_h76281f29__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge test_copper_lists.clk)", 
                                                         "test_copper_lists.sv", 
                                                         44);
    vlSelfRef.test_copper_lists__DOT__cpu_addr = __Vtask_test_copper_lists__DOT__write_reg__83__addr;
    vlSelfRef.test_copper_lists__DOT__cpu_wdata = __Vtask_test_copper_lists__DOT__write_reg__83__data;
    vlSelfRef.test_copper_lists__DOT__cpu_we = 1U;
    vlSelfRef.test_copper_lists__DOT__cpu_ce = 1U;
    Vtest_copper_lists___024root____VbeforeTrig_h76281f29__0(vlSelf, 
                                                             "@(posedge test_copper_lists.clk)");
    co_await vlSelfRef.__VtrigSched_h76281f29__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge test_copper_lists.clk)", 
                                                         "test_copper_lists.sv", 
                                                         46);
    vlSelfRef.test_copper_lists__DOT__cpu_we = 0U;
    vlSelfRef.test_copper_lists__DOT__cpu_ce = 0U;
    __Vtask_test_copper_lists__DOT__run_clocks__84__n = 2U;
    __Vtask_test_copper_lists__DOT__run_clocks__84__test_copper_lists__DOT__unnamedblk1_1__DOT____Vrepeat0 = 0;
    __Vtask_test_copper_lists__DOT__run_clocks__84__test_copper_lists__DOT__unnamedblk1_1__DOT____Vrepeat0 
        = __Vtask_test_copper_lists__DOT__run_clocks__84__n;
    while (VL_LTS_III(32, 0U, __Vtask_test_copper_lists__DOT__run_clocks__84__test_copper_lists__DOT__unnamedblk1_1__DOT____Vrepeat0)) {
        Vtest_copper_lists___024root____VbeforeTrig_h76281f29__0(vlSelf, 
                                                                 "@(posedge test_copper_lists.clk)");
        co_await vlSelfRef.__VtrigSched_h76281f29__0.trigger(0U, 
                                                             nullptr, 
                                                             "@(posedge test_copper_lists.clk)", 
                                                             "test_copper_lists.sv", 
                                                             88);
        __Vtask_test_copper_lists__DOT__run_clocks__84__test_copper_lists__DOT__unnamedblk1_1__DOT____Vrepeat0 
            = (__Vtask_test_copper_lists__DOT__run_clocks__84__test_copper_lists__DOT__unnamedblk1_1__DOT____Vrepeat0 
               - (IData)(1U));
    }
    __Vtask_test_copper_lists__DOT__check__85__condition 
        = (7U == (IData)(vlSelfRef.test_copper_lists__DOT__dut__DOT__copper_target_list));
    vlSelfRef.__Vtask_test_copper_lists__DOT__check__85__name = "target is 7"s;
    vlSelfRef.test_copper_lists__DOT__test_num = ((IData)(1U) 
                                                  + vlSelfRef.test_copper_lists__DOT__test_num);
    if (__Vtask_test_copper_lists__DOT__check__85__condition) {
        VL_WRITEF_NX("  PASS [%0d] %@\n",0,32,vlSelfRef.test_copper_lists__DOT__test_num,
                     -1,&(vlSelfRef.__Vtask_test_copper_lists__DOT__check__85__name));
        vlSelfRef.test_copper_lists__DOT__pass_count 
            = ((IData)(1U) + vlSelfRef.test_copper_lists__DOT__pass_count);
    } else {
        VL_WRITEF_NX("  FAIL [%0d] %@\n",0,32,vlSelfRef.test_copper_lists__DOT__test_num,
                     -1,&(vlSelfRef.__Vtask_test_copper_lists__DOT__check__85__name));
        vlSelfRef.test_copper_lists__DOT__fail_count 
            = ((IData)(1U) + vlSelfRef.test_copper_lists__DOT__fail_count);
    }
    __Vtask_test_copper_lists__DOT__write_cmd__87__cmd = 0x22U;
    __Vtask_test_copper_lists__DOT__write_reg__88__data 
        = __Vtask_test_copper_lists__DOT__write_cmd__87__cmd;
    __Vtask_test_copper_lists__DOT__write_reg__88__addr = 0xa010U;
    Vtest_copper_lists___024root____VbeforeTrig_h76281f29__0(vlSelf, 
                                                             "@(posedge test_copper_lists.clk)");
    co_await vlSelfRef.__VtrigSched_h76281f29__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge test_copper_lists.clk)", 
                                                         "test_copper_lists.sv", 
                                                         44);
    vlSelfRef.test_copper_lists__DOT__cpu_addr = __Vtask_test_copper_lists__DOT__write_reg__88__addr;
    vlSelfRef.test_copper_lists__DOT__cpu_wdata = __Vtask_test_copper_lists__DOT__write_reg__88__data;
    vlSelfRef.test_copper_lists__DOT__cpu_we = 1U;
    vlSelfRef.test_copper_lists__DOT__cpu_ce = 1U;
    Vtest_copper_lists___024root____VbeforeTrig_h76281f29__0(vlSelf, 
                                                             "@(posedge test_copper_lists.clk)");
    co_await vlSelfRef.__VtrigSched_h76281f29__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge test_copper_lists.clk)", 
                                                         "test_copper_lists.sv", 
                                                         46);
    vlSelfRef.test_copper_lists__DOT__cpu_we = 0U;
    vlSelfRef.test_copper_lists__DOT__cpu_ce = 0U;
    __Vtask_test_copper_lists__DOT__run_clocks__89__n = 2U;
    __Vtask_test_copper_lists__DOT__run_clocks__89__test_copper_lists__DOT__unnamedblk1_1__DOT____Vrepeat0 = 0;
    __Vtask_test_copper_lists__DOT__run_clocks__89__test_copper_lists__DOT__unnamedblk1_1__DOT____Vrepeat0 
        = __Vtask_test_copper_lists__DOT__run_clocks__89__n;
    while (VL_LTS_III(32, 0U, __Vtask_test_copper_lists__DOT__run_clocks__89__test_copper_lists__DOT__unnamedblk1_1__DOT____Vrepeat0)) {
        Vtest_copper_lists___024root____VbeforeTrig_h76281f29__0(vlSelf, 
                                                                 "@(posedge test_copper_lists.clk)");
        co_await vlSelfRef.__VtrigSched_h76281f29__0.trigger(0U, 
                                                             nullptr, 
                                                             "@(posedge test_copper_lists.clk)", 
                                                             "test_copper_lists.sv", 
                                                             88);
        __Vtask_test_copper_lists__DOT__run_clocks__89__test_copper_lists__DOT__unnamedblk1_1__DOT____Vrepeat0 
            = (__Vtask_test_copper_lists__DOT__run_clocks__89__test_copper_lists__DOT__unnamedblk1_1__DOT____Vrepeat0 
               - (IData)(1U));
    }
    __Vtask_test_copper_lists__DOT__check__90__condition 
        = ((IData)(vlSelfRef.test_copper_lists__DOT__dut__DOT__copper_target_list) 
           == (IData)(vlSelfRef.test_copper_lists__DOT__dut__DOT__copper_active_list));
    vlSelfRef.__Vtask_test_copper_lists__DOT__check__90__name = "target reset to active (3)"s;
    vlSelfRef.test_copper_lists__DOT__test_num = ((IData)(1U) 
                                                  + vlSelfRef.test_copper_lists__DOT__test_num);
    if (__Vtask_test_copper_lists__DOT__check__90__condition) {
        VL_WRITEF_NX("  PASS [%0d] %@\n",0,32,vlSelfRef.test_copper_lists__DOT__test_num,
                     -1,&(vlSelfRef.__Vtask_test_copper_lists__DOT__check__90__name));
        vlSelfRef.test_copper_lists__DOT__pass_count 
            = ((IData)(1U) + vlSelfRef.test_copper_lists__DOT__pass_count);
    } else {
        VL_WRITEF_NX("  FAIL [%0d] %@\n",0,32,vlSelfRef.test_copper_lists__DOT__test_num,
                     -1,&(vlSelfRef.__Vtask_test_copper_lists__DOT__check__90__name));
        vlSelfRef.test_copper_lists__DOT__fail_count 
            = ((IData)(1U) + vlSelfRef.test_copper_lists__DOT__fail_count);
    }
    VL_WRITEF_NX("Test: Independent list clear\n",0);
    __Vtask_test_copper_lists__DOT__copper_list__91__idx = 3U;
    __Vtask_test_copper_lists__DOT__write_param__92__data 
        = (0x000000ffU & __Vtask_test_copper_lists__DOT__copper_list__91__idx);
    __Vtask_test_copper_lists__DOT__write_param__92__idx = 0U;
    __Vtask_test_copper_lists__DOT__write_reg__93__data 
        = __Vtask_test_copper_lists__DOT__write_param__92__data;
    __Vtask_test_copper_lists__DOT__write_reg__93__addr 
        = (0x0000ffffU & ((IData)(0xa011U) + __Vtask_test_copper_lists__DOT__write_param__92__idx));
    Vtest_copper_lists___024root____VbeforeTrig_h76281f29__0(vlSelf, 
                                                             "@(posedge test_copper_lists.clk)");
    co_await vlSelfRef.__VtrigSched_h76281f29__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge test_copper_lists.clk)", 
                                                         "test_copper_lists.sv", 
                                                         44);
    vlSelfRef.test_copper_lists__DOT__cpu_addr = __Vtask_test_copper_lists__DOT__write_reg__93__addr;
    vlSelfRef.test_copper_lists__DOT__cpu_wdata = __Vtask_test_copper_lists__DOT__write_reg__93__data;
    vlSelfRef.test_copper_lists__DOT__cpu_we = 1U;
    vlSelfRef.test_copper_lists__DOT__cpu_ce = 1U;
    Vtest_copper_lists___024root____VbeforeTrig_h76281f29__0(vlSelf, 
                                                             "@(posedge test_copper_lists.clk)");
    co_await vlSelfRef.__VtrigSched_h76281f29__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge test_copper_lists.clk)", 
                                                         "test_copper_lists.sv", 
                                                         46);
    vlSelfRef.test_copper_lists__DOT__cpu_we = 0U;
    vlSelfRef.test_copper_lists__DOT__cpu_ce = 0U;
    __Vtask_test_copper_lists__DOT__write_cmd__94__cmd = 0x20U;
    __Vtask_test_copper_lists__DOT__write_reg__95__data 
        = __Vtask_test_copper_lists__DOT__write_cmd__94__cmd;
    __Vtask_test_copper_lists__DOT__write_reg__95__addr = 0xa010U;
    Vtest_copper_lists___024root____VbeforeTrig_h76281f29__0(vlSelf, 
                                                             "@(posedge test_copper_lists.clk)");
    co_await vlSelfRef.__VtrigSched_h76281f29__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge test_copper_lists.clk)", 
                                                         "test_copper_lists.sv", 
                                                         44);
    vlSelfRef.test_copper_lists__DOT__cpu_addr = __Vtask_test_copper_lists__DOT__write_reg__95__addr;
    vlSelfRef.test_copper_lists__DOT__cpu_wdata = __Vtask_test_copper_lists__DOT__write_reg__95__data;
    vlSelfRef.test_copper_lists__DOT__cpu_we = 1U;
    vlSelfRef.test_copper_lists__DOT__cpu_ce = 1U;
    Vtest_copper_lists___024root____VbeforeTrig_h76281f29__0(vlSelf, 
                                                             "@(posedge test_copper_lists.clk)");
    co_await vlSelfRef.__VtrigSched_h76281f29__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge test_copper_lists.clk)", 
                                                         "test_copper_lists.sv", 
                                                         46);
    vlSelfRef.test_copper_lists__DOT__cpu_we = 0U;
    vlSelfRef.test_copper_lists__DOT__cpu_ce = 0U;
    __Vtask_test_copper_lists__DOT__run_clocks__96__n = 2U;
    __Vtask_test_copper_lists__DOT__run_clocks__96__test_copper_lists__DOT__unnamedblk1_1__DOT____Vrepeat0 = 0;
    __Vtask_test_copper_lists__DOT__run_clocks__96__test_copper_lists__DOT__unnamedblk1_1__DOT____Vrepeat0 
        = __Vtask_test_copper_lists__DOT__run_clocks__96__n;
    while (VL_LTS_III(32, 0U, __Vtask_test_copper_lists__DOT__run_clocks__96__test_copper_lists__DOT__unnamedblk1_1__DOT____Vrepeat0)) {
        Vtest_copper_lists___024root____VbeforeTrig_h76281f29__0(vlSelf, 
                                                                 "@(posedge test_copper_lists.clk)");
        co_await vlSelfRef.__VtrigSched_h76281f29__0.trigger(0U, 
                                                             nullptr, 
                                                             "@(posedge test_copper_lists.clk)", 
                                                             "test_copper_lists.sv", 
                                                             88);
        __Vtask_test_copper_lists__DOT__run_clocks__96__test_copper_lists__DOT__unnamedblk1_1__DOT____Vrepeat0 
            = (__Vtask_test_copper_lists__DOT__run_clocks__96__test_copper_lists__DOT__unnamedblk1_1__DOT____Vrepeat0 
               - (IData)(1U));
    }
    __Vtask_test_copper_lists__DOT__check__97__condition 
        = (0U < vlSelfRef.test_copper_lists__DOT__dut__DOT__copper_list_count[3U]);
    vlSelfRef.__Vtask_test_copper_lists__DOT__check__97__name = "list 3 has entries before clear"s;
    vlSelfRef.test_copper_lists__DOT__test_num = ((IData)(1U) 
                                                  + vlSelfRef.test_copper_lists__DOT__test_num);
    if (__Vtask_test_copper_lists__DOT__check__97__condition) {
        VL_WRITEF_NX("  PASS [%0d] %@\n",0,32,vlSelfRef.test_copper_lists__DOT__test_num,
                     -1,&(vlSelfRef.__Vtask_test_copper_lists__DOT__check__97__name));
        vlSelfRef.test_copper_lists__DOT__pass_count 
            = ((IData)(1U) + vlSelfRef.test_copper_lists__DOT__pass_count);
    } else {
        VL_WRITEF_NX("  FAIL [%0d] %@\n",0,32,vlSelfRef.test_copper_lists__DOT__test_num,
                     -1,&(vlSelfRef.__Vtask_test_copper_lists__DOT__check__97__name));
        vlSelfRef.test_copper_lists__DOT__fail_count 
            = ((IData)(1U) + vlSelfRef.test_copper_lists__DOT__fail_count);
    }
    __Vtask_test_copper_lists__DOT__write_cmd__99__cmd = 0x1cU;
    __Vtask_test_copper_lists__DOT__write_reg__100__data 
        = __Vtask_test_copper_lists__DOT__write_cmd__99__cmd;
    __Vtask_test_copper_lists__DOT__write_reg__100__addr = 0xa010U;
    Vtest_copper_lists___024root____VbeforeTrig_h76281f29__0(vlSelf, 
                                                             "@(posedge test_copper_lists.clk)");
    co_await vlSelfRef.__VtrigSched_h76281f29__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge test_copper_lists.clk)", 
                                                         "test_copper_lists.sv", 
                                                         44);
    vlSelfRef.test_copper_lists__DOT__cpu_addr = __Vtask_test_copper_lists__DOT__write_reg__100__addr;
    vlSelfRef.test_copper_lists__DOT__cpu_wdata = __Vtask_test_copper_lists__DOT__write_reg__100__data;
    vlSelfRef.test_copper_lists__DOT__cpu_we = 1U;
    vlSelfRef.test_copper_lists__DOT__cpu_ce = 1U;
    Vtest_copper_lists___024root____VbeforeTrig_h76281f29__0(vlSelf, 
                                                             "@(posedge test_copper_lists.clk)");
    co_await vlSelfRef.__VtrigSched_h76281f29__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge test_copper_lists.clk)", 
                                                         "test_copper_lists.sv", 
                                                         46);
    vlSelfRef.test_copper_lists__DOT__cpu_we = 0U;
    vlSelfRef.test_copper_lists__DOT__cpu_ce = 0U;
    __Vtask_test_copper_lists__DOT__run_clocks__101__n = 2U;
    __Vtask_test_copper_lists__DOT__run_clocks__101__test_copper_lists__DOT__unnamedblk1_1__DOT____Vrepeat0 = 0;
    __Vtask_test_copper_lists__DOT__run_clocks__101__test_copper_lists__DOT__unnamedblk1_1__DOT____Vrepeat0 
        = __Vtask_test_copper_lists__DOT__run_clocks__101__n;
    while (VL_LTS_III(32, 0U, __Vtask_test_copper_lists__DOT__run_clocks__101__test_copper_lists__DOT__unnamedblk1_1__DOT____Vrepeat0)) {
        Vtest_copper_lists___024root____VbeforeTrig_h76281f29__0(vlSelf, 
                                                                 "@(posedge test_copper_lists.clk)");
        co_await vlSelfRef.__VtrigSched_h76281f29__0.trigger(0U, 
                                                             nullptr, 
                                                             "@(posedge test_copper_lists.clk)", 
                                                             "test_copper_lists.sv", 
                                                             88);
        __Vtask_test_copper_lists__DOT__run_clocks__101__test_copper_lists__DOT__unnamedblk1_1__DOT____Vrepeat0 
            = (__Vtask_test_copper_lists__DOT__run_clocks__101__test_copper_lists__DOT__unnamedblk1_1__DOT____Vrepeat0 
               - (IData)(1U));
    }
    __Vtask_test_copper_lists__DOT__check__102__condition 
        = (0U == vlSelfRef.test_copper_lists__DOT__dut__DOT__copper_list_count[3U]);
    vlSelfRef.__Vtask_test_copper_lists__DOT__check__102__name = "list 3 cleared to 0"s;
    vlSelfRef.test_copper_lists__DOT__test_num = ((IData)(1U) 
                                                  + vlSelfRef.test_copper_lists__DOT__test_num);
    if (__Vtask_test_copper_lists__DOT__check__102__condition) {
        VL_WRITEF_NX("  PASS [%0d] %@\n",0,32,vlSelfRef.test_copper_lists__DOT__test_num,
                     -1,&(vlSelfRef.__Vtask_test_copper_lists__DOT__check__102__name));
        vlSelfRef.test_copper_lists__DOT__pass_count 
            = ((IData)(1U) + vlSelfRef.test_copper_lists__DOT__pass_count);
    } else {
        VL_WRITEF_NX("  FAIL [%0d] %@\n",0,32,vlSelfRef.test_copper_lists__DOT__test_num,
                     -1,&(vlSelfRef.__Vtask_test_copper_lists__DOT__check__102__name));
        vlSelfRef.test_copper_lists__DOT__fail_count 
            = ((IData)(1U) + vlSelfRef.test_copper_lists__DOT__fail_count);
    }
    __Vtask_test_copper_lists__DOT__check__103__condition 
        = (0U == (IData)(vlSelfRef.test_copper_lists__DOT__dut__DOT__copper_count));
    vlSelfRef.__Vtask_test_copper_lists__DOT__check__103__name = "active arrays cleared too"s;
    vlSelfRef.test_copper_lists__DOT__test_num = ((IData)(1U) 
                                                  + vlSelfRef.test_copper_lists__DOT__test_num);
    if (__Vtask_test_copper_lists__DOT__check__103__condition) {
        VL_WRITEF_NX("  PASS [%0d] %@\n",0,32,vlSelfRef.test_copper_lists__DOT__test_num,
                     -1,&(vlSelfRef.__Vtask_test_copper_lists__DOT__check__103__name));
        vlSelfRef.test_copper_lists__DOT__pass_count 
            = ((IData)(1U) + vlSelfRef.test_copper_lists__DOT__pass_count);
    } else {
        VL_WRITEF_NX("  FAIL [%0d] %@\n",0,32,vlSelfRef.test_copper_lists__DOT__test_num,
                     -1,&(vlSelfRef.__Vtask_test_copper_lists__DOT__check__103__name));
        vlSelfRef.test_copper_lists__DOT__fail_count 
            = ((IData)(1U) + vlSelfRef.test_copper_lists__DOT__fail_count);
    }
    VL_WRITEF_NX("Test: Double buffering workflow\n",0);
    __Vtask_test_copper_lists__DOT__copper_list__104__idx = 0U;
    __Vtask_test_copper_lists__DOT__write_param__105__data 
        = (0x000000ffU & __Vtask_test_copper_lists__DOT__copper_list__104__idx);
    __Vtask_test_copper_lists__DOT__write_param__105__idx = 0U;
    __Vtask_test_copper_lists__DOT__write_reg__106__data 
        = __Vtask_test_copper_lists__DOT__write_param__105__data;
    __Vtask_test_copper_lists__DOT__write_reg__106__addr 
        = (0x0000ffffU & ((IData)(0xa011U) + __Vtask_test_copper_lists__DOT__write_param__105__idx));
    Vtest_copper_lists___024root____VbeforeTrig_h76281f29__0(vlSelf, 
                                                             "@(posedge test_copper_lists.clk)");
    co_await vlSelfRef.__VtrigSched_h76281f29__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge test_copper_lists.clk)", 
                                                         "test_copper_lists.sv", 
                                                         44);
    vlSelfRef.test_copper_lists__DOT__cpu_addr = __Vtask_test_copper_lists__DOT__write_reg__106__addr;
    vlSelfRef.test_copper_lists__DOT__cpu_wdata = __Vtask_test_copper_lists__DOT__write_reg__106__data;
    vlSelfRef.test_copper_lists__DOT__cpu_we = 1U;
    vlSelfRef.test_copper_lists__DOT__cpu_ce = 1U;
    Vtest_copper_lists___024root____VbeforeTrig_h76281f29__0(vlSelf, 
                                                             "@(posedge test_copper_lists.clk)");
    co_await vlSelfRef.__VtrigSched_h76281f29__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge test_copper_lists.clk)", 
                                                         "test_copper_lists.sv", 
                                                         46);
    vlSelfRef.test_copper_lists__DOT__cpu_we = 0U;
    vlSelfRef.test_copper_lists__DOT__cpu_ce = 0U;
    __Vtask_test_copper_lists__DOT__write_cmd__107__cmd = 0x20U;
    __Vtask_test_copper_lists__DOT__write_reg__108__data 
        = __Vtask_test_copper_lists__DOT__write_cmd__107__cmd;
    __Vtask_test_copper_lists__DOT__write_reg__108__addr = 0xa010U;
    Vtest_copper_lists___024root____VbeforeTrig_h76281f29__0(vlSelf, 
                                                             "@(posedge test_copper_lists.clk)");
    co_await vlSelfRef.__VtrigSched_h76281f29__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge test_copper_lists.clk)", 
                                                         "test_copper_lists.sv", 
                                                         44);
    vlSelfRef.test_copper_lists__DOT__cpu_addr = __Vtask_test_copper_lists__DOT__write_reg__108__addr;
    vlSelfRef.test_copper_lists__DOT__cpu_wdata = __Vtask_test_copper_lists__DOT__write_reg__108__data;
    vlSelfRef.test_copper_lists__DOT__cpu_we = 1U;
    vlSelfRef.test_copper_lists__DOT__cpu_ce = 1U;
    Vtest_copper_lists___024root____VbeforeTrig_h76281f29__0(vlSelf, 
                                                             "@(posedge test_copper_lists.clk)");
    co_await vlSelfRef.__VtrigSched_h76281f29__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge test_copper_lists.clk)", 
                                                         "test_copper_lists.sv", 
                                                         46);
    vlSelfRef.test_copper_lists__DOT__cpu_we = 0U;
    vlSelfRef.test_copper_lists__DOT__cpu_ce = 0U;
    __Vtask_test_copper_lists__DOT__run_clocks__109__n = 2U;
    __Vtask_test_copper_lists__DOT__run_clocks__109__test_copper_lists__DOT__unnamedblk1_1__DOT____Vrepeat0 = 0;
    __Vtask_test_copper_lists__DOT__run_clocks__109__test_copper_lists__DOT__unnamedblk1_1__DOT____Vrepeat0 
        = __Vtask_test_copper_lists__DOT__run_clocks__109__n;
    while (VL_LTS_III(32, 0U, __Vtask_test_copper_lists__DOT__run_clocks__109__test_copper_lists__DOT__unnamedblk1_1__DOT____Vrepeat0)) {
        Vtest_copper_lists___024root____VbeforeTrig_h76281f29__0(vlSelf, 
                                                                 "@(posedge test_copper_lists.clk)");
        co_await vlSelfRef.__VtrigSched_h76281f29__0.trigger(0U, 
                                                             nullptr, 
                                                             "@(posedge test_copper_lists.clk)", 
                                                             "test_copper_lists.sv", 
                                                             88);
        __Vtask_test_copper_lists__DOT__run_clocks__109__test_copper_lists__DOT__unnamedblk1_1__DOT____Vrepeat0 
            = (__Vtask_test_copper_lists__DOT__run_clocks__109__test_copper_lists__DOT__unnamedblk1_1__DOT____Vrepeat0 
               - (IData)(1U));
    }
    __Vtask_test_copper_lists__DOT__write_cmd__111__cmd = 0x1cU;
    __Vtask_test_copper_lists__DOT__write_reg__112__data 
        = __Vtask_test_copper_lists__DOT__write_cmd__111__cmd;
    __Vtask_test_copper_lists__DOT__write_reg__112__addr = 0xa010U;
    Vtest_copper_lists___024root____VbeforeTrig_h76281f29__0(vlSelf, 
                                                             "@(posedge test_copper_lists.clk)");
    co_await vlSelfRef.__VtrigSched_h76281f29__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge test_copper_lists.clk)", 
                                                         "test_copper_lists.sv", 
                                                         44);
    vlSelfRef.test_copper_lists__DOT__cpu_addr = __Vtask_test_copper_lists__DOT__write_reg__112__addr;
    vlSelfRef.test_copper_lists__DOT__cpu_wdata = __Vtask_test_copper_lists__DOT__write_reg__112__data;
    vlSelfRef.test_copper_lists__DOT__cpu_we = 1U;
    vlSelfRef.test_copper_lists__DOT__cpu_ce = 1U;
    Vtest_copper_lists___024root____VbeforeTrig_h76281f29__0(vlSelf, 
                                                             "@(posedge test_copper_lists.clk)");
    co_await vlSelfRef.__VtrigSched_h76281f29__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge test_copper_lists.clk)", 
                                                         "test_copper_lists.sv", 
                                                         46);
    vlSelfRef.test_copper_lists__DOT__cpu_we = 0U;
    vlSelfRef.test_copper_lists__DOT__cpu_ce = 0U;
    __Vtask_test_copper_lists__DOT__run_clocks__113__n = 2U;
    __Vtask_test_copper_lists__DOT__run_clocks__113__test_copper_lists__DOT__unnamedblk1_1__DOT____Vrepeat0 = 0;
    __Vtask_test_copper_lists__DOT__run_clocks__113__test_copper_lists__DOT__unnamedblk1_1__DOT____Vrepeat0 
        = __Vtask_test_copper_lists__DOT__run_clocks__113__n;
    while (VL_LTS_III(32, 0U, __Vtask_test_copper_lists__DOT__run_clocks__113__test_copper_lists__DOT__unnamedblk1_1__DOT____Vrepeat0)) {
        Vtest_copper_lists___024root____VbeforeTrig_h76281f29__0(vlSelf, 
                                                                 "@(posedge test_copper_lists.clk)");
        co_await vlSelfRef.__VtrigSched_h76281f29__0.trigger(0U, 
                                                             nullptr, 
                                                             "@(posedge test_copper_lists.clk)", 
                                                             "test_copper_lists.sv", 
                                                             88);
        __Vtask_test_copper_lists__DOT__run_clocks__113__test_copper_lists__DOT__unnamedblk1_1__DOT____Vrepeat0 
            = (__Vtask_test_copper_lists__DOT__run_clocks__113__test_copper_lists__DOT__unnamedblk1_1__DOT____Vrepeat0 
               - (IData)(1U));
    }
    __Vtask_test_copper_lists__DOT__copper_add__114__value = 7U;
    __Vtask_test_copper_lists__DOT__copper_add__114__reg_idx = 1U;
    __Vtask_test_copper_lists__DOT__copper_add__114__y = 0x00000014U;
    __Vtask_test_copper_lists__DOT__copper_add__114__x = 0U;
    __Vtask_test_copper_lists__DOT__write_param__115__data 
        = (0x000000ffU & __Vtask_test_copper_lists__DOT__copper_add__114__x);
    __Vtask_test_copper_lists__DOT__write_param__115__idx = 0U;
    __Vtask_test_copper_lists__DOT__write_reg__116__data 
        = __Vtask_test_copper_lists__DOT__write_param__115__data;
    __Vtask_test_copper_lists__DOT__write_reg__116__addr 
        = (0x0000ffffU & ((IData)(0xa011U) + __Vtask_test_copper_lists__DOT__write_param__115__idx));
    Vtest_copper_lists___024root____VbeforeTrig_h76281f29__0(vlSelf, 
                                                             "@(posedge test_copper_lists.clk)");
    co_await vlSelfRef.__VtrigSched_h76281f29__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge test_copper_lists.clk)", 
                                                         "test_copper_lists.sv", 
                                                         44);
    vlSelfRef.test_copper_lists__DOT__cpu_addr = __Vtask_test_copper_lists__DOT__write_reg__116__addr;
    vlSelfRef.test_copper_lists__DOT__cpu_wdata = __Vtask_test_copper_lists__DOT__write_reg__116__data;
    vlSelfRef.test_copper_lists__DOT__cpu_we = 1U;
    vlSelfRef.test_copper_lists__DOT__cpu_ce = 1U;
    Vtest_copper_lists___024root____VbeforeTrig_h76281f29__0(vlSelf, 
                                                             "@(posedge test_copper_lists.clk)");
    co_await vlSelfRef.__VtrigSched_h76281f29__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge test_copper_lists.clk)", 
                                                         "test_copper_lists.sv", 
                                                         46);
    vlSelfRef.test_copper_lists__DOT__cpu_we = 0U;
    vlSelfRef.test_copper_lists__DOT__cpu_ce = 0U;
    __Vtask_test_copper_lists__DOT__write_param__117__data 
        = (0x000000ffU & (__Vtask_test_copper_lists__DOT__copper_add__114__x 
                          >> 8U));
    __Vtask_test_copper_lists__DOT__write_param__117__idx = 1U;
    __Vtask_test_copper_lists__DOT__write_reg__118__data 
        = __Vtask_test_copper_lists__DOT__write_param__117__data;
    __Vtask_test_copper_lists__DOT__write_reg__118__addr 
        = (0x0000ffffU & ((IData)(0xa011U) + __Vtask_test_copper_lists__DOT__write_param__117__idx));
    Vtest_copper_lists___024root____VbeforeTrig_h76281f29__0(vlSelf, 
                                                             "@(posedge test_copper_lists.clk)");
    co_await vlSelfRef.__VtrigSched_h76281f29__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge test_copper_lists.clk)", 
                                                         "test_copper_lists.sv", 
                                                         44);
    vlSelfRef.test_copper_lists__DOT__cpu_addr = __Vtask_test_copper_lists__DOT__write_reg__118__addr;
    vlSelfRef.test_copper_lists__DOT__cpu_wdata = __Vtask_test_copper_lists__DOT__write_reg__118__data;
    vlSelfRef.test_copper_lists__DOT__cpu_we = 1U;
    vlSelfRef.test_copper_lists__DOT__cpu_ce = 1U;
    Vtest_copper_lists___024root____VbeforeTrig_h76281f29__0(vlSelf, 
                                                             "@(posedge test_copper_lists.clk)");
    co_await vlSelfRef.__VtrigSched_h76281f29__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge test_copper_lists.clk)", 
                                                         "test_copper_lists.sv", 
                                                         46);
    vlSelfRef.test_copper_lists__DOT__cpu_we = 0U;
    vlSelfRef.test_copper_lists__DOT__cpu_ce = 0U;
    __Vtask_test_copper_lists__DOT__write_param__119__data 
        = (0x000000ffU & __Vtask_test_copper_lists__DOT__copper_add__114__y);
    __Vtask_test_copper_lists__DOT__write_param__119__idx = 2U;
    __Vtask_test_copper_lists__DOT__write_reg__120__data 
        = __Vtask_test_copper_lists__DOT__write_param__119__data;
    __Vtask_test_copper_lists__DOT__write_reg__120__addr 
        = (0x0000ffffU & ((IData)(0xa011U) + __Vtask_test_copper_lists__DOT__write_param__119__idx));
    Vtest_copper_lists___024root____VbeforeTrig_h76281f29__0(vlSelf, 
                                                             "@(posedge test_copper_lists.clk)");
    co_await vlSelfRef.__VtrigSched_h76281f29__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge test_copper_lists.clk)", 
                                                         "test_copper_lists.sv", 
                                                         44);
    vlSelfRef.test_copper_lists__DOT__cpu_addr = __Vtask_test_copper_lists__DOT__write_reg__120__addr;
    vlSelfRef.test_copper_lists__DOT__cpu_wdata = __Vtask_test_copper_lists__DOT__write_reg__120__data;
    vlSelfRef.test_copper_lists__DOT__cpu_we = 1U;
    vlSelfRef.test_copper_lists__DOT__cpu_ce = 1U;
    Vtest_copper_lists___024root____VbeforeTrig_h76281f29__0(vlSelf, 
                                                             "@(posedge test_copper_lists.clk)");
    co_await vlSelfRef.__VtrigSched_h76281f29__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge test_copper_lists.clk)", 
                                                         "test_copper_lists.sv", 
                                                         46);
    vlSelfRef.test_copper_lists__DOT__cpu_we = 0U;
    vlSelfRef.test_copper_lists__DOT__cpu_ce = 0U;
    __Vtask_test_copper_lists__DOT__write_param__121__data 
        = (0x000000ffU & __Vtask_test_copper_lists__DOT__copper_add__114__reg_idx);
    __Vtask_test_copper_lists__DOT__write_param__121__idx = 3U;
    __Vtask_test_copper_lists__DOT__write_reg__122__data 
        = __Vtask_test_copper_lists__DOT__write_param__121__data;
    __Vtask_test_copper_lists__DOT__write_reg__122__addr 
        = (0x0000ffffU & ((IData)(0xa011U) + __Vtask_test_copper_lists__DOT__write_param__121__idx));
    Vtest_copper_lists___024root____VbeforeTrig_h76281f29__0(vlSelf, 
                                                             "@(posedge test_copper_lists.clk)");
    co_await vlSelfRef.__VtrigSched_h76281f29__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge test_copper_lists.clk)", 
                                                         "test_copper_lists.sv", 
                                                         44);
    vlSelfRef.test_copper_lists__DOT__cpu_addr = __Vtask_test_copper_lists__DOT__write_reg__122__addr;
    vlSelfRef.test_copper_lists__DOT__cpu_wdata = __Vtask_test_copper_lists__DOT__write_reg__122__data;
    vlSelfRef.test_copper_lists__DOT__cpu_we = 1U;
    vlSelfRef.test_copper_lists__DOT__cpu_ce = 1U;
    Vtest_copper_lists___024root____VbeforeTrig_h76281f29__0(vlSelf, 
                                                             "@(posedge test_copper_lists.clk)");
    co_await vlSelfRef.__VtrigSched_h76281f29__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge test_copper_lists.clk)", 
                                                         "test_copper_lists.sv", 
                                                         46);
    vlSelfRef.test_copper_lists__DOT__cpu_we = 0U;
    vlSelfRef.test_copper_lists__DOT__cpu_ce = 0U;
    __Vtask_test_copper_lists__DOT__write_param__123__data 
        = (0x000000ffU & (__Vtask_test_copper_lists__DOT__copper_add__114__reg_idx 
                          >> 8U));
    __Vtask_test_copper_lists__DOT__write_param__123__idx = 4U;
    __Vtask_test_copper_lists__DOT__write_reg__124__data 
        = __Vtask_test_copper_lists__DOT__write_param__123__data;
    __Vtask_test_copper_lists__DOT__write_reg__124__addr 
        = (0x0000ffffU & ((IData)(0xa011U) + __Vtask_test_copper_lists__DOT__write_param__123__idx));
    Vtest_copper_lists___024root____VbeforeTrig_h76281f29__0(vlSelf, 
                                                             "@(posedge test_copper_lists.clk)");
    co_await vlSelfRef.__VtrigSched_h76281f29__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge test_copper_lists.clk)", 
                                                         "test_copper_lists.sv", 
                                                         44);
    vlSelfRef.test_copper_lists__DOT__cpu_addr = __Vtask_test_copper_lists__DOT__write_reg__124__addr;
    vlSelfRef.test_copper_lists__DOT__cpu_wdata = __Vtask_test_copper_lists__DOT__write_reg__124__data;
    vlSelfRef.test_copper_lists__DOT__cpu_we = 1U;
    vlSelfRef.test_copper_lists__DOT__cpu_ce = 1U;
    Vtest_copper_lists___024root____VbeforeTrig_h76281f29__0(vlSelf, 
                                                             "@(posedge test_copper_lists.clk)");
    co_await vlSelfRef.__VtrigSched_h76281f29__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge test_copper_lists.clk)", 
                                                         "test_copper_lists.sv", 
                                                         46);
    vlSelfRef.test_copper_lists__DOT__cpu_we = 0U;
    vlSelfRef.test_copper_lists__DOT__cpu_ce = 0U;
    __Vtask_test_copper_lists__DOT__write_param__125__data 
        = __Vtask_test_copper_lists__DOT__copper_add__114__value;
    __Vtask_test_copper_lists__DOT__write_param__125__idx = 5U;
    __Vtask_test_copper_lists__DOT__write_reg__126__data 
        = __Vtask_test_copper_lists__DOT__write_param__125__data;
    __Vtask_test_copper_lists__DOT__write_reg__126__addr 
        = (0x0000ffffU & ((IData)(0xa011U) + __Vtask_test_copper_lists__DOT__write_param__125__idx));
    Vtest_copper_lists___024root____VbeforeTrig_h76281f29__0(vlSelf, 
                                                             "@(posedge test_copper_lists.clk)");
    co_await vlSelfRef.__VtrigSched_h76281f29__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge test_copper_lists.clk)", 
                                                         "test_copper_lists.sv", 
                                                         44);
    vlSelfRef.test_copper_lists__DOT__cpu_addr = __Vtask_test_copper_lists__DOT__write_reg__126__addr;
    vlSelfRef.test_copper_lists__DOT__cpu_wdata = __Vtask_test_copper_lists__DOT__write_reg__126__data;
    vlSelfRef.test_copper_lists__DOT__cpu_we = 1U;
    vlSelfRef.test_copper_lists__DOT__cpu_ce = 1U;
    Vtest_copper_lists___024root____VbeforeTrig_h76281f29__0(vlSelf, 
                                                             "@(posedge test_copper_lists.clk)");
    co_await vlSelfRef.__VtrigSched_h76281f29__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge test_copper_lists.clk)", 
                                                         "test_copper_lists.sv", 
                                                         46);
    vlSelfRef.test_copper_lists__DOT__cpu_we = 0U;
    vlSelfRef.test_copper_lists__DOT__cpu_ce = 0U;
    __Vtask_test_copper_lists__DOT__write_cmd__127__cmd = 0x1bU;
    __Vtask_test_copper_lists__DOT__write_reg__128__data 
        = __Vtask_test_copper_lists__DOT__write_cmd__127__cmd;
    __Vtask_test_copper_lists__DOT__write_reg__128__addr = 0xa010U;
    Vtest_copper_lists___024root____VbeforeTrig_h76281f29__0(vlSelf, 
                                                             "@(posedge test_copper_lists.clk)");
    co_await vlSelfRef.__VtrigSched_h76281f29__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge test_copper_lists.clk)", 
                                                         "test_copper_lists.sv", 
                                                         44);
    vlSelfRef.test_copper_lists__DOT__cpu_addr = __Vtask_test_copper_lists__DOT__write_reg__128__addr;
    vlSelfRef.test_copper_lists__DOT__cpu_wdata = __Vtask_test_copper_lists__DOT__write_reg__128__data;
    vlSelfRef.test_copper_lists__DOT__cpu_we = 1U;
    vlSelfRef.test_copper_lists__DOT__cpu_ce = 1U;
    Vtest_copper_lists___024root____VbeforeTrig_h76281f29__0(vlSelf, 
                                                             "@(posedge test_copper_lists.clk)");
    co_await vlSelfRef.__VtrigSched_h76281f29__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge test_copper_lists.clk)", 
                                                         "test_copper_lists.sv", 
                                                         46);
    vlSelfRef.test_copper_lists__DOT__cpu_we = 0U;
    vlSelfRef.test_copper_lists__DOT__cpu_ce = 0U;
    __Vtask_test_copper_lists__DOT__copper_add__129__value = 2U;
    __Vtask_test_copper_lists__DOT__copper_add__129__reg_idx = 1U;
    __Vtask_test_copper_lists__DOT__copper_add__129__y = 0x00000050U;
    __Vtask_test_copper_lists__DOT__copper_add__129__x = 0U;
    __Vtask_test_copper_lists__DOT__write_param__130__data 
        = (0x000000ffU & __Vtask_test_copper_lists__DOT__copper_add__129__x);
    __Vtask_test_copper_lists__DOT__write_param__130__idx = 0U;
    __Vtask_test_copper_lists__DOT__write_reg__131__data 
        = __Vtask_test_copper_lists__DOT__write_param__130__data;
    __Vtask_test_copper_lists__DOT__write_reg__131__addr 
        = (0x0000ffffU & ((IData)(0xa011U) + __Vtask_test_copper_lists__DOT__write_param__130__idx));
    Vtest_copper_lists___024root____VbeforeTrig_h76281f29__0(vlSelf, 
                                                             "@(posedge test_copper_lists.clk)");
    co_await vlSelfRef.__VtrigSched_h76281f29__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge test_copper_lists.clk)", 
                                                         "test_copper_lists.sv", 
                                                         44);
    vlSelfRef.test_copper_lists__DOT__cpu_addr = __Vtask_test_copper_lists__DOT__write_reg__131__addr;
    vlSelfRef.test_copper_lists__DOT__cpu_wdata = __Vtask_test_copper_lists__DOT__write_reg__131__data;
    vlSelfRef.test_copper_lists__DOT__cpu_we = 1U;
    vlSelfRef.test_copper_lists__DOT__cpu_ce = 1U;
    Vtest_copper_lists___024root____VbeforeTrig_h76281f29__0(vlSelf, 
                                                             "@(posedge test_copper_lists.clk)");
    co_await vlSelfRef.__VtrigSched_h76281f29__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge test_copper_lists.clk)", 
                                                         "test_copper_lists.sv", 
                                                         46);
    vlSelfRef.test_copper_lists__DOT__cpu_we = 0U;
    vlSelfRef.test_copper_lists__DOT__cpu_ce = 0U;
    __Vtask_test_copper_lists__DOT__write_param__132__data 
        = (0x000000ffU & (__Vtask_test_copper_lists__DOT__copper_add__129__x 
                          >> 8U));
    __Vtask_test_copper_lists__DOT__write_param__132__idx = 1U;
    __Vtask_test_copper_lists__DOT__write_reg__133__data 
        = __Vtask_test_copper_lists__DOT__write_param__132__data;
    __Vtask_test_copper_lists__DOT__write_reg__133__addr 
        = (0x0000ffffU & ((IData)(0xa011U) + __Vtask_test_copper_lists__DOT__write_param__132__idx));
    Vtest_copper_lists___024root____VbeforeTrig_h76281f29__0(vlSelf, 
                                                             "@(posedge test_copper_lists.clk)");
    co_await vlSelfRef.__VtrigSched_h76281f29__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge test_copper_lists.clk)", 
                                                         "test_copper_lists.sv", 
                                                         44);
    vlSelfRef.test_copper_lists__DOT__cpu_addr = __Vtask_test_copper_lists__DOT__write_reg__133__addr;
    vlSelfRef.test_copper_lists__DOT__cpu_wdata = __Vtask_test_copper_lists__DOT__write_reg__133__data;
    vlSelfRef.test_copper_lists__DOT__cpu_we = 1U;
    vlSelfRef.test_copper_lists__DOT__cpu_ce = 1U;
    Vtest_copper_lists___024root____VbeforeTrig_h76281f29__0(vlSelf, 
                                                             "@(posedge test_copper_lists.clk)");
    co_await vlSelfRef.__VtrigSched_h76281f29__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge test_copper_lists.clk)", 
                                                         "test_copper_lists.sv", 
                                                         46);
    vlSelfRef.test_copper_lists__DOT__cpu_we = 0U;
    vlSelfRef.test_copper_lists__DOT__cpu_ce = 0U;
    __Vtask_test_copper_lists__DOT__write_param__134__data 
        = (0x000000ffU & __Vtask_test_copper_lists__DOT__copper_add__129__y);
    __Vtask_test_copper_lists__DOT__write_param__134__idx = 2U;
    __Vtask_test_copper_lists__DOT__write_reg__135__data 
        = __Vtask_test_copper_lists__DOT__write_param__134__data;
    __Vtask_test_copper_lists__DOT__write_reg__135__addr 
        = (0x0000ffffU & ((IData)(0xa011U) + __Vtask_test_copper_lists__DOT__write_param__134__idx));
    Vtest_copper_lists___024root____VbeforeTrig_h76281f29__0(vlSelf, 
                                                             "@(posedge test_copper_lists.clk)");
    co_await vlSelfRef.__VtrigSched_h76281f29__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge test_copper_lists.clk)", 
                                                         "test_copper_lists.sv", 
                                                         44);
    vlSelfRef.test_copper_lists__DOT__cpu_addr = __Vtask_test_copper_lists__DOT__write_reg__135__addr;
    vlSelfRef.test_copper_lists__DOT__cpu_wdata = __Vtask_test_copper_lists__DOT__write_reg__135__data;
    vlSelfRef.test_copper_lists__DOT__cpu_we = 1U;
    vlSelfRef.test_copper_lists__DOT__cpu_ce = 1U;
    Vtest_copper_lists___024root____VbeforeTrig_h76281f29__0(vlSelf, 
                                                             "@(posedge test_copper_lists.clk)");
    co_await vlSelfRef.__VtrigSched_h76281f29__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge test_copper_lists.clk)", 
                                                         "test_copper_lists.sv", 
                                                         46);
    vlSelfRef.test_copper_lists__DOT__cpu_we = 0U;
    vlSelfRef.test_copper_lists__DOT__cpu_ce = 0U;
    __Vtask_test_copper_lists__DOT__write_param__136__data 
        = (0x000000ffU & __Vtask_test_copper_lists__DOT__copper_add__129__reg_idx);
    __Vtask_test_copper_lists__DOT__write_param__136__idx = 3U;
    __Vtask_test_copper_lists__DOT__write_reg__137__data 
        = __Vtask_test_copper_lists__DOT__write_param__136__data;
    __Vtask_test_copper_lists__DOT__write_reg__137__addr 
        = (0x0000ffffU & ((IData)(0xa011U) + __Vtask_test_copper_lists__DOT__write_param__136__idx));
    Vtest_copper_lists___024root____VbeforeTrig_h76281f29__0(vlSelf, 
                                                             "@(posedge test_copper_lists.clk)");
    co_await vlSelfRef.__VtrigSched_h76281f29__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge test_copper_lists.clk)", 
                                                         "test_copper_lists.sv", 
                                                         44);
    vlSelfRef.test_copper_lists__DOT__cpu_addr = __Vtask_test_copper_lists__DOT__write_reg__137__addr;
    vlSelfRef.test_copper_lists__DOT__cpu_wdata = __Vtask_test_copper_lists__DOT__write_reg__137__data;
    vlSelfRef.test_copper_lists__DOT__cpu_we = 1U;
    vlSelfRef.test_copper_lists__DOT__cpu_ce = 1U;
    Vtest_copper_lists___024root____VbeforeTrig_h76281f29__0(vlSelf, 
                                                             "@(posedge test_copper_lists.clk)");
    co_await vlSelfRef.__VtrigSched_h76281f29__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge test_copper_lists.clk)", 
                                                         "test_copper_lists.sv", 
                                                         46);
    vlSelfRef.test_copper_lists__DOT__cpu_we = 0U;
    vlSelfRef.test_copper_lists__DOT__cpu_ce = 0U;
    __Vtask_test_copper_lists__DOT__write_param__138__data 
        = (0x000000ffU & (__Vtask_test_copper_lists__DOT__copper_add__129__reg_idx 
                          >> 8U));
    __Vtask_test_copper_lists__DOT__write_param__138__idx = 4U;
    __Vtask_test_copper_lists__DOT__write_reg__139__data 
        = __Vtask_test_copper_lists__DOT__write_param__138__data;
    __Vtask_test_copper_lists__DOT__write_reg__139__addr 
        = (0x0000ffffU & ((IData)(0xa011U) + __Vtask_test_copper_lists__DOT__write_param__138__idx));
    Vtest_copper_lists___024root____VbeforeTrig_h76281f29__0(vlSelf, 
                                                             "@(posedge test_copper_lists.clk)");
    co_await vlSelfRef.__VtrigSched_h76281f29__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge test_copper_lists.clk)", 
                                                         "test_copper_lists.sv", 
                                                         44);
    vlSelfRef.test_copper_lists__DOT__cpu_addr = __Vtask_test_copper_lists__DOT__write_reg__139__addr;
    vlSelfRef.test_copper_lists__DOT__cpu_wdata = __Vtask_test_copper_lists__DOT__write_reg__139__data;
    vlSelfRef.test_copper_lists__DOT__cpu_we = 1U;
    vlSelfRef.test_copper_lists__DOT__cpu_ce = 1U;
    Vtest_copper_lists___024root____VbeforeTrig_h76281f29__0(vlSelf, 
                                                             "@(posedge test_copper_lists.clk)");
    co_await vlSelfRef.__VtrigSched_h76281f29__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge test_copper_lists.clk)", 
                                                         "test_copper_lists.sv", 
                                                         46);
    vlSelfRef.test_copper_lists__DOT__cpu_we = 0U;
    vlSelfRef.test_copper_lists__DOT__cpu_ce = 0U;
    __Vtask_test_copper_lists__DOT__write_param__140__data 
        = __Vtask_test_copper_lists__DOT__copper_add__129__value;
    __Vtask_test_copper_lists__DOT__write_param__140__idx = 5U;
    __Vtask_test_copper_lists__DOT__write_reg__141__data 
        = __Vtask_test_copper_lists__DOT__write_param__140__data;
    __Vtask_test_copper_lists__DOT__write_reg__141__addr 
        = (0x0000ffffU & ((IData)(0xa011U) + __Vtask_test_copper_lists__DOT__write_param__140__idx));
    Vtest_copper_lists___024root____VbeforeTrig_h76281f29__0(vlSelf, 
                                                             "@(posedge test_copper_lists.clk)");
    co_await vlSelfRef.__VtrigSched_h76281f29__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge test_copper_lists.clk)", 
                                                         "test_copper_lists.sv", 
                                                         44);
    vlSelfRef.test_copper_lists__DOT__cpu_addr = __Vtask_test_copper_lists__DOT__write_reg__141__addr;
    vlSelfRef.test_copper_lists__DOT__cpu_wdata = __Vtask_test_copper_lists__DOT__write_reg__141__data;
    vlSelfRef.test_copper_lists__DOT__cpu_we = 1U;
    vlSelfRef.test_copper_lists__DOT__cpu_ce = 1U;
    Vtest_copper_lists___024root____VbeforeTrig_h76281f29__0(vlSelf, 
                                                             "@(posedge test_copper_lists.clk)");
    co_await vlSelfRef.__VtrigSched_h76281f29__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge test_copper_lists.clk)", 
                                                         "test_copper_lists.sv", 
                                                         46);
    vlSelfRef.test_copper_lists__DOT__cpu_we = 0U;
    vlSelfRef.test_copper_lists__DOT__cpu_ce = 0U;
    __Vtask_test_copper_lists__DOT__write_cmd__142__cmd = 0x1bU;
    __Vtask_test_copper_lists__DOT__write_reg__143__data 
        = __Vtask_test_copper_lists__DOT__write_cmd__142__cmd;
    __Vtask_test_copper_lists__DOT__write_reg__143__addr = 0xa010U;
    Vtest_copper_lists___024root____VbeforeTrig_h76281f29__0(vlSelf, 
                                                             "@(posedge test_copper_lists.clk)");
    co_await vlSelfRef.__VtrigSched_h76281f29__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge test_copper_lists.clk)", 
                                                         "test_copper_lists.sv", 
                                                         44);
    vlSelfRef.test_copper_lists__DOT__cpu_addr = __Vtask_test_copper_lists__DOT__write_reg__143__addr;
    vlSelfRef.test_copper_lists__DOT__cpu_wdata = __Vtask_test_copper_lists__DOT__write_reg__143__data;
    vlSelfRef.test_copper_lists__DOT__cpu_we = 1U;
    vlSelfRef.test_copper_lists__DOT__cpu_ce = 1U;
    Vtest_copper_lists___024root____VbeforeTrig_h76281f29__0(vlSelf, 
                                                             "@(posedge test_copper_lists.clk)");
    co_await vlSelfRef.__VtrigSched_h76281f29__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge test_copper_lists.clk)", 
                                                         "test_copper_lists.sv", 
                                                         46);
    vlSelfRef.test_copper_lists__DOT__cpu_we = 0U;
    vlSelfRef.test_copper_lists__DOT__cpu_ce = 0U;
    __Vtask_test_copper_lists__DOT__run_clocks__144__n = 2U;
    __Vtask_test_copper_lists__DOT__run_clocks__144__test_copper_lists__DOT__unnamedblk1_1__DOT____Vrepeat0 = 0;
    __Vtask_test_copper_lists__DOT__run_clocks__144__test_copper_lists__DOT__unnamedblk1_1__DOT____Vrepeat0 
        = __Vtask_test_copper_lists__DOT__run_clocks__144__n;
    while (VL_LTS_III(32, 0U, __Vtask_test_copper_lists__DOT__run_clocks__144__test_copper_lists__DOT__unnamedblk1_1__DOT____Vrepeat0)) {
        Vtest_copper_lists___024root____VbeforeTrig_h76281f29__0(vlSelf, 
                                                                 "@(posedge test_copper_lists.clk)");
        co_await vlSelfRef.__VtrigSched_h76281f29__0.trigger(0U, 
                                                             nullptr, 
                                                             "@(posedge test_copper_lists.clk)", 
                                                             "test_copper_lists.sv", 
                                                             88);
        __Vtask_test_copper_lists__DOT__run_clocks__144__test_copper_lists__DOT__unnamedblk1_1__DOT____Vrepeat0 
            = (__Vtask_test_copper_lists__DOT__run_clocks__144__test_copper_lists__DOT__unnamedblk1_1__DOT____Vrepeat0 
               - (IData)(1U));
    }
    __Vtask_test_copper_lists__DOT__check__145__condition 
        = (2U == vlSelfRef.test_copper_lists__DOT__dut__DOT__copper_list_count[0U]);
    vlSelfRef.__Vtask_test_copper_lists__DOT__check__145__name = "list 0 has 2 entries"s;
    vlSelfRef.test_copper_lists__DOT__test_num = ((IData)(1U) 
                                                  + vlSelfRef.test_copper_lists__DOT__test_num);
    if (__Vtask_test_copper_lists__DOT__check__145__condition) {
        VL_WRITEF_NX("  PASS [%0d] %@\n",0,32,vlSelfRef.test_copper_lists__DOT__test_num,
                     -1,&(vlSelfRef.__Vtask_test_copper_lists__DOT__check__145__name));
        vlSelfRef.test_copper_lists__DOT__pass_count 
            = ((IData)(1U) + vlSelfRef.test_copper_lists__DOT__pass_count);
    } else {
        VL_WRITEF_NX("  FAIL [%0d] %@\n",0,32,vlSelfRef.test_copper_lists__DOT__test_num,
                     -1,&(vlSelfRef.__Vtask_test_copper_lists__DOT__check__145__name));
        vlSelfRef.test_copper_lists__DOT__fail_count 
            = ((IData)(1U) + vlSelfRef.test_copper_lists__DOT__fail_count);
    }
    __Vtask_test_copper_lists__DOT__copper_use__146__idx = 0U;
    __Vtask_test_copper_lists__DOT__write_param__147__data 
        = (0x000000ffU & __Vtask_test_copper_lists__DOT__copper_use__146__idx);
    __Vtask_test_copper_lists__DOT__write_param__147__idx = 0U;
    __Vtask_test_copper_lists__DOT__write_reg__148__data 
        = __Vtask_test_copper_lists__DOT__write_param__147__data;
    __Vtask_test_copper_lists__DOT__write_reg__148__addr 
        = (0x0000ffffU & ((IData)(0xa011U) + __Vtask_test_copper_lists__DOT__write_param__147__idx));
    Vtest_copper_lists___024root____VbeforeTrig_h76281f29__0(vlSelf, 
                                                             "@(posedge test_copper_lists.clk)");
    co_await vlSelfRef.__VtrigSched_h76281f29__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge test_copper_lists.clk)", 
                                                         "test_copper_lists.sv", 
                                                         44);
    vlSelfRef.test_copper_lists__DOT__cpu_addr = __Vtask_test_copper_lists__DOT__write_reg__148__addr;
    vlSelfRef.test_copper_lists__DOT__cpu_wdata = __Vtask_test_copper_lists__DOT__write_reg__148__data;
    vlSelfRef.test_copper_lists__DOT__cpu_we = 1U;
    vlSelfRef.test_copper_lists__DOT__cpu_ce = 1U;
    Vtest_copper_lists___024root____VbeforeTrig_h76281f29__0(vlSelf, 
                                                             "@(posedge test_copper_lists.clk)");
    co_await vlSelfRef.__VtrigSched_h76281f29__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge test_copper_lists.clk)", 
                                                         "test_copper_lists.sv", 
                                                         46);
    vlSelfRef.test_copper_lists__DOT__cpu_we = 0U;
    vlSelfRef.test_copper_lists__DOT__cpu_ce = 0U;
    __Vtask_test_copper_lists__DOT__write_cmd__149__cmd = 0x21U;
    __Vtask_test_copper_lists__DOT__write_reg__150__data 
        = __Vtask_test_copper_lists__DOT__write_cmd__149__cmd;
    __Vtask_test_copper_lists__DOT__write_reg__150__addr = 0xa010U;
    Vtest_copper_lists___024root____VbeforeTrig_h76281f29__0(vlSelf, 
                                                             "@(posedge test_copper_lists.clk)");
    co_await vlSelfRef.__VtrigSched_h76281f29__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge test_copper_lists.clk)", 
                                                         "test_copper_lists.sv", 
                                                         44);
    vlSelfRef.test_copper_lists__DOT__cpu_addr = __Vtask_test_copper_lists__DOT__write_reg__150__addr;
    vlSelfRef.test_copper_lists__DOT__cpu_wdata = __Vtask_test_copper_lists__DOT__write_reg__150__data;
    vlSelfRef.test_copper_lists__DOT__cpu_we = 1U;
    vlSelfRef.test_copper_lists__DOT__cpu_ce = 1U;
    Vtest_copper_lists___024root____VbeforeTrig_h76281f29__0(vlSelf, 
                                                             "@(posedge test_copper_lists.clk)");
    co_await vlSelfRef.__VtrigSched_h76281f29__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge test_copper_lists.clk)", 
                                                         "test_copper_lists.sv", 
                                                         46);
    vlSelfRef.test_copper_lists__DOT__cpu_we = 0U;
    vlSelfRef.test_copper_lists__DOT__cpu_ce = 0U;
    __Vtask_test_copper_lists__DOT__run_clocks__151__n = 2U;
    __Vtask_test_copper_lists__DOT__run_clocks__151__test_copper_lists__DOT__unnamedblk1_1__DOT____Vrepeat0 = 0;
    __Vtask_test_copper_lists__DOT__run_clocks__151__test_copper_lists__DOT__unnamedblk1_1__DOT____Vrepeat0 
        = __Vtask_test_copper_lists__DOT__run_clocks__151__n;
    while (VL_LTS_III(32, 0U, __Vtask_test_copper_lists__DOT__run_clocks__151__test_copper_lists__DOT__unnamedblk1_1__DOT____Vrepeat0)) {
        Vtest_copper_lists___024root____VbeforeTrig_h76281f29__0(vlSelf, 
                                                                 "@(posedge test_copper_lists.clk)");
        co_await vlSelfRef.__VtrigSched_h76281f29__0.trigger(0U, 
                                                             nullptr, 
                                                             "@(posedge test_copper_lists.clk)", 
                                                             "test_copper_lists.sv", 
                                                             88);
        __Vtask_test_copper_lists__DOT__run_clocks__151__test_copper_lists__DOT__unnamedblk1_1__DOT____Vrepeat0 
            = (__Vtask_test_copper_lists__DOT__run_clocks__151__test_copper_lists__DOT__unnamedblk1_1__DOT____Vrepeat0 
               - (IData)(1U));
    }
    __Vtask_test_copper_lists__DOT__run_to_scanline__152__target = 0x000001e0U;
    while ((((IData)(vlSelfRef.test_copper_lists__DOT__dut__DOT__v_count) 
             != __Vtask_test_copper_lists__DOT__run_to_scanline__152__target) 
            | (0U != (IData)(vlSelfRef.test_copper_lists__DOT__dut__DOT__h_count)))) {
        Vtest_copper_lists___024root____VbeforeTrig_h76281f29__0(vlSelf, 
                                                                 "@(posedge test_copper_lists.clk)");
        co_await vlSelfRef.__VtrigSched_h76281f29__0.trigger(0U, 
                                                             nullptr, 
                                                             "@(posedge test_copper_lists.clk)", 
                                                             "test_copper_lists.sv", 
                                                             93);
    }
    __Vtask_test_copper_lists__DOT__run_clocks__153__n = 0x0000000aU;
    __Vtask_test_copper_lists__DOT__run_clocks__153__test_copper_lists__DOT__unnamedblk1_1__DOT____Vrepeat0 = 0;
    __Vtask_test_copper_lists__DOT__run_clocks__153__test_copper_lists__DOT__unnamedblk1_1__DOT____Vrepeat0 
        = __Vtask_test_copper_lists__DOT__run_clocks__153__n;
    while (VL_LTS_III(32, 0U, __Vtask_test_copper_lists__DOT__run_clocks__153__test_copper_lists__DOT__unnamedblk1_1__DOT____Vrepeat0)) {
        Vtest_copper_lists___024root____VbeforeTrig_h76281f29__0(vlSelf, 
                                                                 "@(posedge test_copper_lists.clk)");
        co_await vlSelfRef.__VtrigSched_h76281f29__0.trigger(0U, 
                                                             nullptr, 
                                                             "@(posedge test_copper_lists.clk)", 
                                                             "test_copper_lists.sv", 
                                                             88);
        __Vtask_test_copper_lists__DOT__run_clocks__153__test_copper_lists__DOT__unnamedblk1_1__DOT____Vrepeat0 
            = (__Vtask_test_copper_lists__DOT__run_clocks__153__test_copper_lists__DOT__unnamedblk1_1__DOT____Vrepeat0 
               - (IData)(1U));
    }
    while (vlSelfRef.test_copper_lists__DOT__dut__DOT__copper_loading) {
        Vtest_copper_lists___024root____VbeforeTrig_h76281f29__0(vlSelf, 
                                                                 "@(posedge test_copper_lists.clk)");
        co_await vlSelfRef.__VtrigSched_h76281f29__0.trigger(0U, 
                                                             nullptr, 
                                                             "@(posedge test_copper_lists.clk)", 
                                                             "test_copper_lists.sv", 
                                                             235);
    }
    __Vtask_test_copper_lists__DOT__run_clocks__154__n = 5U;
    __Vtask_test_copper_lists__DOT__run_clocks__154__test_copper_lists__DOT__unnamedblk1_1__DOT____Vrepeat0 = 0;
    __Vtask_test_copper_lists__DOT__run_clocks__154__test_copper_lists__DOT__unnamedblk1_1__DOT____Vrepeat0 
        = __Vtask_test_copper_lists__DOT__run_clocks__154__n;
    while (VL_LTS_III(32, 0U, __Vtask_test_copper_lists__DOT__run_clocks__154__test_copper_lists__DOT__unnamedblk1_1__DOT____Vrepeat0)) {
        Vtest_copper_lists___024root____VbeforeTrig_h76281f29__0(vlSelf, 
                                                                 "@(posedge test_copper_lists.clk)");
        co_await vlSelfRef.__VtrigSched_h76281f29__0.trigger(0U, 
                                                             nullptr, 
                                                             "@(posedge test_copper_lists.clk)", 
                                                             "test_copper_lists.sv", 
                                                             88);
        __Vtask_test_copper_lists__DOT__run_clocks__154__test_copper_lists__DOT__unnamedblk1_1__DOT____Vrepeat0 
            = (__Vtask_test_copper_lists__DOT__run_clocks__154__test_copper_lists__DOT__unnamedblk1_1__DOT____Vrepeat0 
               - (IData)(1U));
    }
    __Vtask_test_copper_lists__DOT__check__155__condition 
        = (0U == (IData)(vlSelfRef.test_copper_lists__DOT__dut__DOT__copper_active_list));
    vlSelfRef.__Vtask_test_copper_lists__DOT__check__155__name = "active swapped to list 0"s;
    vlSelfRef.test_copper_lists__DOT__test_num = ((IData)(1U) 
                                                  + vlSelfRef.test_copper_lists__DOT__test_num);
    if (__Vtask_test_copper_lists__DOT__check__155__condition) {
        VL_WRITEF_NX("  PASS [%0d] %@\n",0,32,vlSelfRef.test_copper_lists__DOT__test_num,
                     -1,&(vlSelfRef.__Vtask_test_copper_lists__DOT__check__155__name));
        vlSelfRef.test_copper_lists__DOT__pass_count 
            = ((IData)(1U) + vlSelfRef.test_copper_lists__DOT__pass_count);
    } else {
        VL_WRITEF_NX("  FAIL [%0d] %@\n",0,32,vlSelfRef.test_copper_lists__DOT__test_num,
                     -1,&(vlSelfRef.__Vtask_test_copper_lists__DOT__check__155__name));
        vlSelfRef.test_copper_lists__DOT__fail_count 
            = ((IData)(1U) + vlSelfRef.test_copper_lists__DOT__fail_count);
    }
    __Vtask_test_copper_lists__DOT__check__156__condition 
        = (2U == (IData)(vlSelfRef.test_copper_lists__DOT__dut__DOT__copper_count));
    vlSelfRef.__Vtask_test_copper_lists__DOT__check__156__name = "active count is 2"s;
    vlSelfRef.test_copper_lists__DOT__test_num = ((IData)(1U) 
                                                  + vlSelfRef.test_copper_lists__DOT__test_num);
    if (__Vtask_test_copper_lists__DOT__check__156__condition) {
        VL_WRITEF_NX("  PASS [%0d] %@\n",0,32,vlSelfRef.test_copper_lists__DOT__test_num,
                     -1,&(vlSelfRef.__Vtask_test_copper_lists__DOT__check__156__name));
        vlSelfRef.test_copper_lists__DOT__pass_count 
            = ((IData)(1U) + vlSelfRef.test_copper_lists__DOT__pass_count);
    } else {
        VL_WRITEF_NX("  FAIL [%0d] %@\n",0,32,vlSelfRef.test_copper_lists__DOT__test_num,
                     -1,&(vlSelfRef.__Vtask_test_copper_lists__DOT__check__156__name));
        vlSelfRef.test_copper_lists__DOT__fail_count 
            = ((IData)(1U) + vlSelfRef.test_copper_lists__DOT__fail_count);
    }
    __Vtask_test_copper_lists__DOT__write_reg__157__data = 6U;
    __Vtask_test_copper_lists__DOT__write_reg__157__addr = 0xa001U;
    Vtest_copper_lists___024root____VbeforeTrig_h76281f29__0(vlSelf, 
                                                             "@(posedge test_copper_lists.clk)");
    co_await vlSelfRef.__VtrigSched_h76281f29__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge test_copper_lists.clk)", 
                                                         "test_copper_lists.sv", 
                                                         44);
    vlSelfRef.test_copper_lists__DOT__cpu_addr = __Vtask_test_copper_lists__DOT__write_reg__157__addr;
    vlSelfRef.test_copper_lists__DOT__cpu_wdata = __Vtask_test_copper_lists__DOT__write_reg__157__data;
    vlSelfRef.test_copper_lists__DOT__cpu_we = 1U;
    vlSelfRef.test_copper_lists__DOT__cpu_ce = 1U;
    Vtest_copper_lists___024root____VbeforeTrig_h76281f29__0(vlSelf, 
                                                             "@(posedge test_copper_lists.clk)");
    co_await vlSelfRef.__VtrigSched_h76281f29__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge test_copper_lists.clk)", 
                                                         "test_copper_lists.sv", 
                                                         46);
    vlSelfRef.test_copper_lists__DOT__cpu_we = 0U;
    vlSelfRef.test_copper_lists__DOT__cpu_ce = 0U;
    __Vtask_test_copper_lists__DOT__run_clocks__158__n = 2U;
    __Vtask_test_copper_lists__DOT__run_clocks__158__test_copper_lists__DOT__unnamedblk1_1__DOT____Vrepeat0 = 0;
    __Vtask_test_copper_lists__DOT__run_clocks__158__test_copper_lists__DOT__unnamedblk1_1__DOT____Vrepeat0 
        = __Vtask_test_copper_lists__DOT__run_clocks__158__n;
    while (VL_LTS_III(32, 0U, __Vtask_test_copper_lists__DOT__run_clocks__158__test_copper_lists__DOT__unnamedblk1_1__DOT____Vrepeat0)) {
        Vtest_copper_lists___024root____VbeforeTrig_h76281f29__0(vlSelf, 
                                                                 "@(posedge test_copper_lists.clk)");
        co_await vlSelfRef.__VtrigSched_h76281f29__0.trigger(0U, 
                                                             nullptr, 
                                                             "@(posedge test_copper_lists.clk)", 
                                                             "test_copper_lists.sv", 
                                                             88);
        __Vtask_test_copper_lists__DOT__run_clocks__158__test_copper_lists__DOT__unnamedblk1_1__DOT____Vrepeat0 
            = (__Vtask_test_copper_lists__DOT__run_clocks__158__test_copper_lists__DOT__unnamedblk1_1__DOT____Vrepeat0 
               - (IData)(1U));
    }
    __Vtask_test_copper_lists__DOT__run_to_scanline__159__target = 0U;
    while ((((IData)(vlSelfRef.test_copper_lists__DOT__dut__DOT__v_count) 
             != __Vtask_test_copper_lists__DOT__run_to_scanline__159__target) 
            | (0U != (IData)(vlSelfRef.test_copper_lists__DOT__dut__DOT__h_count)))) {
        Vtest_copper_lists___024root____VbeforeTrig_h76281f29__0(vlSelf, 
                                                                 "@(posedge test_copper_lists.clk)");
        co_await vlSelfRef.__VtrigSched_h76281f29__0.trigger(0U, 
                                                             nullptr, 
                                                             "@(posedge test_copper_lists.clk)", 
                                                             "test_copper_lists.sv", 
                                                             93);
    }
    __Vtask_test_copper_lists__DOT__run_to_scanline__160__target = 0x00000051U;
    while ((((IData)(vlSelfRef.test_copper_lists__DOT__dut__DOT__v_count) 
             != __Vtask_test_copper_lists__DOT__run_to_scanline__160__target) 
            | (0U != (IData)(vlSelfRef.test_copper_lists__DOT__dut__DOT__h_count)))) {
        Vtest_copper_lists___024root____VbeforeTrig_h76281f29__0(vlSelf, 
                                                                 "@(posedge test_copper_lists.clk)");
        co_await vlSelfRef.__VtrigSched_h76281f29__0.trigger(0U, 
                                                             nullptr, 
                                                             "@(posedge test_copper_lists.clk)", 
                                                             "test_copper_lists.sv", 
                                                             93);
    }
    __Vtask_test_copper_lists__DOT__check__161__condition 
        = (7U == (IData)(vlSelfRef.test_copper_lists__DOT__dut__DOT__bg_color));
    vlSelfRef.__Vtask_test_copper_lists__DOT__check__161__name = "bg=7 after line 20"s;
    vlSelfRef.test_copper_lists__DOT__test_num = ((IData)(1U) 
                                                  + vlSelfRef.test_copper_lists__DOT__test_num);
    if (__Vtask_test_copper_lists__DOT__check__161__condition) {
        VL_WRITEF_NX("  PASS [%0d] %@\n",0,32,vlSelfRef.test_copper_lists__DOT__test_num,
                     -1,&(vlSelfRef.__Vtask_test_copper_lists__DOT__check__161__name));
        vlSelfRef.test_copper_lists__DOT__pass_count 
            = ((IData)(1U) + vlSelfRef.test_copper_lists__DOT__pass_count);
    } else {
        VL_WRITEF_NX("  FAIL [%0d] %@\n",0,32,vlSelfRef.test_copper_lists__DOT__test_num,
                     -1,&(vlSelfRef.__Vtask_test_copper_lists__DOT__check__161__name));
        vlSelfRef.test_copper_lists__DOT__fail_count 
            = ((IData)(1U) + vlSelfRef.test_copper_lists__DOT__fail_count);
    }
    __Vtask_test_copper_lists__DOT__run_to_scanline__162__target = 0x000000c9U;
    while ((((IData)(vlSelfRef.test_copper_lists__DOT__dut__DOT__v_count) 
             != __Vtask_test_copper_lists__DOT__run_to_scanline__162__target) 
            | (0U != (IData)(vlSelfRef.test_copper_lists__DOT__dut__DOT__h_count)))) {
        Vtest_copper_lists___024root____VbeforeTrig_h76281f29__0(vlSelf, 
                                                                 "@(posedge test_copper_lists.clk)");
        co_await vlSelfRef.__VtrigSched_h76281f29__0.trigger(0U, 
                                                             nullptr, 
                                                             "@(posedge test_copper_lists.clk)", 
                                                             "test_copper_lists.sv", 
                                                             93);
    }
    __Vtask_test_copper_lists__DOT__check__163__condition 
        = (2U == (IData)(vlSelfRef.test_copper_lists__DOT__dut__DOT__bg_color));
    vlSelfRef.__Vtask_test_copper_lists__DOT__check__163__name = "bg=2 after line 80"s;
    vlSelfRef.test_copper_lists__DOT__test_num = ((IData)(1U) 
                                                  + vlSelfRef.test_copper_lists__DOT__test_num);
    if (__Vtask_test_copper_lists__DOT__check__163__condition) {
        VL_WRITEF_NX("  PASS [%0d] %@\n",0,32,vlSelfRef.test_copper_lists__DOT__test_num,
                     -1,&(vlSelfRef.__Vtask_test_copper_lists__DOT__check__163__name));
        vlSelfRef.test_copper_lists__DOT__pass_count 
            = ((IData)(1U) + vlSelfRef.test_copper_lists__DOT__pass_count);
    } else {
        VL_WRITEF_NX("  FAIL [%0d] %@\n",0,32,vlSelfRef.test_copper_lists__DOT__test_num,
                     -1,&(vlSelfRef.__Vtask_test_copper_lists__DOT__check__163__name));
        vlSelfRef.test_copper_lists__DOT__fail_count 
            = ((IData)(1U) + vlSelfRef.test_copper_lists__DOT__fail_count);
    }
    VL_WRITEF_NX("Test: SysReset\n",0);
    __Vtask_test_copper_lists__DOT__write_cmd__164__cmd = 0x1fU;
    __Vtask_test_copper_lists__DOT__write_reg__165__data 
        = __Vtask_test_copper_lists__DOT__write_cmd__164__cmd;
    __Vtask_test_copper_lists__DOT__write_reg__165__addr = 0xa010U;
    Vtest_copper_lists___024root____VbeforeTrig_h76281f29__0(vlSelf, 
                                                             "@(posedge test_copper_lists.clk)");
    co_await vlSelfRef.__VtrigSched_h76281f29__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge test_copper_lists.clk)", 
                                                         "test_copper_lists.sv", 
                                                         44);
    vlSelfRef.test_copper_lists__DOT__cpu_addr = __Vtask_test_copper_lists__DOT__write_reg__165__addr;
    vlSelfRef.test_copper_lists__DOT__cpu_wdata = __Vtask_test_copper_lists__DOT__write_reg__165__data;
    vlSelfRef.test_copper_lists__DOT__cpu_we = 1U;
    vlSelfRef.test_copper_lists__DOT__cpu_ce = 1U;
    Vtest_copper_lists___024root____VbeforeTrig_h76281f29__0(vlSelf, 
                                                             "@(posedge test_copper_lists.clk)");
    co_await vlSelfRef.__VtrigSched_h76281f29__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge test_copper_lists.clk)", 
                                                         "test_copper_lists.sv", 
                                                         46);
    vlSelfRef.test_copper_lists__DOT__cpu_we = 0U;
    vlSelfRef.test_copper_lists__DOT__cpu_ce = 0U;
    __Vtask_test_copper_lists__DOT__run_clocks__166__n = 5U;
    __Vtask_test_copper_lists__DOT__run_clocks__166__test_copper_lists__DOT__unnamedblk1_1__DOT____Vrepeat0 = 0;
    __Vtask_test_copper_lists__DOT__run_clocks__166__test_copper_lists__DOT__unnamedblk1_1__DOT____Vrepeat0 
        = __Vtask_test_copper_lists__DOT__run_clocks__166__n;
    while (VL_LTS_III(32, 0U, __Vtask_test_copper_lists__DOT__run_clocks__166__test_copper_lists__DOT__unnamedblk1_1__DOT____Vrepeat0)) {
        Vtest_copper_lists___024root____VbeforeTrig_h76281f29__0(vlSelf, 
                                                                 "@(posedge test_copper_lists.clk)");
        co_await vlSelfRef.__VtrigSched_h76281f29__0.trigger(0U, 
                                                             nullptr, 
                                                             "@(posedge test_copper_lists.clk)", 
                                                             "test_copper_lists.sv", 
                                                             88);
        __Vtask_test_copper_lists__DOT__run_clocks__166__test_copper_lists__DOT__unnamedblk1_1__DOT____Vrepeat0 
            = (__Vtask_test_copper_lists__DOT__run_clocks__166__test_copper_lists__DOT__unnamedblk1_1__DOT____Vrepeat0 
               - (IData)(1U));
    }
    __Vtask_test_copper_lists__DOT__check__167__condition 
        = (0U == (IData)(vlSelfRef.test_copper_lists__DOT__dut__DOT__copper_target_list));
    vlSelfRef.__Vtask_test_copper_lists__DOT__check__167__name = "target reset to 0"s;
    vlSelfRef.test_copper_lists__DOT__test_num = ((IData)(1U) 
                                                  + vlSelfRef.test_copper_lists__DOT__test_num);
    if (__Vtask_test_copper_lists__DOT__check__167__condition) {
        VL_WRITEF_NX("  PASS [%0d] %@\n",0,32,vlSelfRef.test_copper_lists__DOT__test_num,
                     -1,&(vlSelfRef.__Vtask_test_copper_lists__DOT__check__167__name));
        vlSelfRef.test_copper_lists__DOT__pass_count 
            = ((IData)(1U) + vlSelfRef.test_copper_lists__DOT__pass_count);
    } else {
        VL_WRITEF_NX("  FAIL [%0d] %@\n",0,32,vlSelfRef.test_copper_lists__DOT__test_num,
                     -1,&(vlSelfRef.__Vtask_test_copper_lists__DOT__check__167__name));
        vlSelfRef.test_copper_lists__DOT__fail_count 
            = ((IData)(1U) + vlSelfRef.test_copper_lists__DOT__fail_count);
    }
    __Vtask_test_copper_lists__DOT__check__168__condition 
        = (0U == (IData)(vlSelfRef.test_copper_lists__DOT__dut__DOT__copper_active_list));
    vlSelfRef.__Vtask_test_copper_lists__DOT__check__168__name = "active reset to 0"s;
    vlSelfRef.test_copper_lists__DOT__test_num = ((IData)(1U) 
                                                  + vlSelfRef.test_copper_lists__DOT__test_num);
    if (__Vtask_test_copper_lists__DOT__check__168__condition) {
        VL_WRITEF_NX("  PASS [%0d] %@\n",0,32,vlSelfRef.test_copper_lists__DOT__test_num,
                     -1,&(vlSelfRef.__Vtask_test_copper_lists__DOT__check__168__name));
        vlSelfRef.test_copper_lists__DOT__pass_count 
            = ((IData)(1U) + vlSelfRef.test_copper_lists__DOT__pass_count);
    } else {
        VL_WRITEF_NX("  FAIL [%0d] %@\n",0,32,vlSelfRef.test_copper_lists__DOT__test_num,
                     -1,&(vlSelfRef.__Vtask_test_copper_lists__DOT__check__168__name));
        vlSelfRef.test_copper_lists__DOT__fail_count 
            = ((IData)(1U) + vlSelfRef.test_copper_lists__DOT__fail_count);
    }
    __Vtask_test_copper_lists__DOT__check__169__condition 
        = (0U == (IData)(vlSelfRef.test_copper_lists__DOT__dut__DOT__copper_pending_list));
    vlSelfRef.__Vtask_test_copper_lists__DOT__check__169__name = "pending reset to 0"s;
    vlSelfRef.test_copper_lists__DOT__test_num = ((IData)(1U) 
                                                  + vlSelfRef.test_copper_lists__DOT__test_num);
    if (__Vtask_test_copper_lists__DOT__check__169__condition) {
        VL_WRITEF_NX("  PASS [%0d] %@\n",0,32,vlSelfRef.test_copper_lists__DOT__test_num,
                     -1,&(vlSelfRef.__Vtask_test_copper_lists__DOT__check__169__name));
        vlSelfRef.test_copper_lists__DOT__pass_count 
            = ((IData)(1U) + vlSelfRef.test_copper_lists__DOT__pass_count);
    } else {
        VL_WRITEF_NX("  FAIL [%0d] %@\n",0,32,vlSelfRef.test_copper_lists__DOT__test_num,
                     -1,&(vlSelfRef.__Vtask_test_copper_lists__DOT__check__169__name));
        vlSelfRef.test_copper_lists__DOT__fail_count 
            = ((IData)(1U) + vlSelfRef.test_copper_lists__DOT__fail_count);
    }
    __Vtask_test_copper_lists__DOT__check__170__condition 
        = (1U & (~ (IData)(vlSelfRef.test_copper_lists__DOT__dut__DOT__copper_enabled)));
    vlSelfRef.__Vtask_test_copper_lists__DOT__check__170__name = "copper disabled after reset"s;
    vlSelfRef.test_copper_lists__DOT__test_num = ((IData)(1U) 
                                                  + vlSelfRef.test_copper_lists__DOT__test_num);
    if (__Vtask_test_copper_lists__DOT__check__170__condition) {
        VL_WRITEF_NX("  PASS [%0d] %@\n",0,32,vlSelfRef.test_copper_lists__DOT__test_num,
                     -1,&(vlSelfRef.__Vtask_test_copper_lists__DOT__check__170__name));
        vlSelfRef.test_copper_lists__DOT__pass_count 
            = ((IData)(1U) + vlSelfRef.test_copper_lists__DOT__pass_count);
    } else {
        VL_WRITEF_NX("  FAIL [%0d] %@\n",0,32,vlSelfRef.test_copper_lists__DOT__test_num,
                     -1,&(vlSelfRef.__Vtask_test_copper_lists__DOT__check__170__name));
        vlSelfRef.test_copper_lists__DOT__fail_count 
            = ((IData)(1U) + vlSelfRef.test_copper_lists__DOT__fail_count);
    }
    VL_WRITEF_NX("\n=== Results: %0d passed, %0d failed ===\n",0,
                 32,vlSelfRef.test_copper_lists__DOT__pass_count,
                 32,vlSelfRef.test_copper_lists__DOT__fail_count);
    if ((0U == vlSelfRef.test_copper_lists__DOT__fail_count)) {
        VL_WRITEF_NX("ALL TESTS PASSED\n",0);
    } else {
        VL_WRITEF_NX("SOME TESTS FAILED\n",0);
    }
    VL_FINISH_MT("test_copper_lists.sv", 265, "");
    co_return;
}

VlCoroutine Vtest_copper_lists___024root___eval_initial__TOP__Vtiming__1(Vtest_copper_lists___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vtest_copper_lists___024root___eval_initial__TOP__Vtiming__1\n"); );
    Vtest_copper_lists__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
    // Body
    while (VL_LIKELY(!vlSymsp->_vm_contextp__->gotFinish())) {
        co_await vlSelfRef.__VdlySched.delay(0x0000000000004e20ULL, 
                                             nullptr, 
                                             "test_copper_lists.sv", 
                                             9);
        vlSelfRef.test_copper_lists__DOT__clk = (1U 
                                                 & (~ (IData)(vlSelfRef.test_copper_lists__DOT__clk)));
    }
    co_return;
}

void Vtest_copper_lists___024root___eval_triggers_vec__act(Vtest_copper_lists___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vtest_copper_lists___024root___eval_triggers_vec__act\n"); );
    Vtest_copper_lists__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
    // Body
    vlSelfRef.__VactTriggered[0U] = (QData)((IData)(
                                                    ((vlSelfRef.__VdlySched.awaitingCurrentTime() 
                                                      << 1U) 
                                                     | ((IData)(vlSelfRef.test_copper_lists__DOT__clk) 
                                                        & (~ (IData)(vlSelfRef.__Vtrigprevexpr___TOP__test_copper_lists__DOT__clk__0))))));
    vlSelfRef.__Vtrigprevexpr___TOP__test_copper_lists__DOT__clk__0 
        = vlSelfRef.test_copper_lists__DOT__clk;
}

bool Vtest_copper_lists___024root___trigger_anySet__act(const VlUnpacked<QData/*63:0*/, 1> &in) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vtest_copper_lists___024root___trigger_anySet__act\n"); );
    // Locals
    IData/*31:0*/ n;
    // Body
    n = 0U;
    do {
        if (in[n]) {
            return (1U);
        }
        n = ((IData)(1U) + n);
    } while ((1U > n));
    return (0U);
}

void Vtest_copper_lists___024root___act_sequent__TOP__0(Vtest_copper_lists___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vtest_copper_lists___024root___act_sequent__TOP__0\n"); );
    Vtest_copper_lists__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
    // Locals
    CData/*0:0*/ __VdfgRegularize_hebeb780c_0_0;
    __VdfgRegularize_hebeb780c_0_0 = 0;
    // Body
    vlSelfRef.test_copper_lists__DOT__dut__DOT__vgc_reg_sel 
        = ((0xa000U <= (IData)(vlSelfRef.test_copper_lists__DOT__cpu_addr)) 
           & (0xa01fU >= (IData)(vlSelfRef.test_copper_lists__DOT__cpu_addr)));
    vlSelfRef.test_copper_lists__DOT__dut__DOT__spr_reg_sel 
        = ((0xa040U <= (IData)(vlSelfRef.test_copper_lists__DOT__cpu_addr)) 
           & (0xa0bfU >= (IData)(vlSelfRef.test_copper_lists__DOT__cpu_addr)));
    vlSelfRef.test_copper_lists__DOT__dut__DOT__tile_reg_sel 
        = ((0xa0c0U <= (IData)(vlSelfRef.test_copper_lists__DOT__cpu_addr)) 
           & (0xa0dfU >= (IData)(vlSelfRef.test_copper_lists__DOT__cpu_addr)));
    vlSelfRef.test_copper_lists__DOT__dut__DOT____Vcellinp__tile_inst__cpu_we 
        = ((IData)(vlSelfRef.test_copper_lists__DOT__cpu_ce) 
           & (IData)(vlSelfRef.test_copper_lists__DOT__cpu_we));
    vlSelfRef.test_copper_lists__DOT__dut__DOT__char_ram_sel 
        = ((0xaa00U <= (IData)(vlSelfRef.test_copper_lists__DOT__cpu_addr)) 
           & (0xb1cfU >= (IData)(vlSelfRef.test_copper_lists__DOT__cpu_addr)));
    vlSelfRef.test_copper_lists__DOT__dut__DOT__color_ram_sel 
        = ((0xb1d0U <= (IData)(vlSelfRef.test_copper_lists__DOT__cpu_addr)) 
           & (0xb99fU >= (IData)(vlSelfRef.test_copper_lists__DOT__cpu_addr)));
    __VdfgRegularize_hebeb780c_0_0 = ((IData)(vlSelfRef.test_copper_lists__DOT__cpu_ce) 
                                      & (IData)(vlSelfRef.test_copper_lists__DOT__cpu_re));
    vlSelfRef.test_copper_lists__DOT__dut__DOT__char_a_addr 
        = (0x000007ffU & ((IData)(vlSelfRef.test_copper_lists__DOT__dut__DOT__cmd_char_we)
                           ? (IData)(vlSelfRef.test_copper_lists__DOT__dut__DOT__cmd_char_addr)
                           : (((IData)(__VdfgRegularize_hebeb780c_0_0) 
                               & (IData)(vlSelfRef.test_copper_lists__DOT__dut__DOT__char_ram_sel))
                               ? ((IData)(vlSelfRef.test_copper_lists__DOT__cpu_addr) 
                                  - (IData)(0x0200U))
                               : ((IData)(vlSelfRef.test_copper_lists__DOT__dut__DOT__dbg_char_sel)
                                   ? ((IData)(vlSelfRef.test_copper_lists__DOT__dut__DOT__dbg_addr) 
                                      - (IData)(0x0200U))
                                   : (IData)(vlSelfRef.test_copper_lists__DOT__dut__DOT__cmd_char_addr)))));
    vlSelfRef.test_copper_lists__DOT__dut__DOT__color_a_addr 
        = (0x000007ffU & ((IData)(vlSelfRef.test_copper_lists__DOT__dut__DOT__cmd_color_we)
                           ? (IData)(vlSelfRef.test_copper_lists__DOT__dut__DOT__cmd_color_addr)
                           : (((IData)(__VdfgRegularize_hebeb780c_0_0) 
                               & (IData)(vlSelfRef.test_copper_lists__DOT__dut__DOT__color_ram_sel))
                               ? ((IData)(vlSelfRef.test_copper_lists__DOT__cpu_addr) 
                                  - (IData)(0x01d0U))
                               : ((IData)(vlSelfRef.test_copper_lists__DOT__dut__DOT__dbg_color_sel)
                                   ? ((IData)(vlSelfRef.test_copper_lists__DOT__dut__DOT__dbg_addr) 
                                      - (IData)(0x01d0U))
                                   : (IData)(vlSelfRef.test_copper_lists__DOT__dut__DOT__cmd_color_addr)))));
}

void Vtest_copper_lists___024root___eval_act(Vtest_copper_lists___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vtest_copper_lists___024root___eval_act\n"); );
    Vtest_copper_lists__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
    // Locals
    CData/*0:0*/ __Vinline__act_sequent__TOP__0___VdfgRegularize_hebeb780c_0_0;
    __Vinline__act_sequent__TOP__0___VdfgRegularize_hebeb780c_0_0 = 0;
    // Body
    if ((1ULL & vlSelfRef.__VactTriggered[0U])) {
        vlSelfRef.test_copper_lists__DOT__dut__DOT__vgc_reg_sel 
            = ((0xa000U <= (IData)(vlSelfRef.test_copper_lists__DOT__cpu_addr)) 
               & (0xa01fU >= (IData)(vlSelfRef.test_copper_lists__DOT__cpu_addr)));
        vlSelfRef.test_copper_lists__DOT__dut__DOT__spr_reg_sel 
            = ((0xa040U <= (IData)(vlSelfRef.test_copper_lists__DOT__cpu_addr)) 
               & (0xa0bfU >= (IData)(vlSelfRef.test_copper_lists__DOT__cpu_addr)));
        vlSelfRef.test_copper_lists__DOT__dut__DOT__tile_reg_sel 
            = ((0xa0c0U <= (IData)(vlSelfRef.test_copper_lists__DOT__cpu_addr)) 
               & (0xa0dfU >= (IData)(vlSelfRef.test_copper_lists__DOT__cpu_addr)));
        vlSelfRef.test_copper_lists__DOT__dut__DOT____Vcellinp__tile_inst__cpu_we 
            = ((IData)(vlSelfRef.test_copper_lists__DOT__cpu_ce) 
               & (IData)(vlSelfRef.test_copper_lists__DOT__cpu_we));
        vlSelfRef.test_copper_lists__DOT__dut__DOT__char_ram_sel 
            = ((0xaa00U <= (IData)(vlSelfRef.test_copper_lists__DOT__cpu_addr)) 
               & (0xb1cfU >= (IData)(vlSelfRef.test_copper_lists__DOT__cpu_addr)));
        vlSelfRef.test_copper_lists__DOT__dut__DOT__color_ram_sel 
            = ((0xb1d0U <= (IData)(vlSelfRef.test_copper_lists__DOT__cpu_addr)) 
               & (0xb99fU >= (IData)(vlSelfRef.test_copper_lists__DOT__cpu_addr)));
        __Vinline__act_sequent__TOP__0___VdfgRegularize_hebeb780c_0_0 
            = ((IData)(vlSelfRef.test_copper_lists__DOT__cpu_ce) 
               & (IData)(vlSelfRef.test_copper_lists__DOT__cpu_re));
        vlSelfRef.test_copper_lists__DOT__dut__DOT__char_a_addr 
            = (0x000007ffU & ((IData)(vlSelfRef.test_copper_lists__DOT__dut__DOT__cmd_char_we)
                               ? (IData)(vlSelfRef.test_copper_lists__DOT__dut__DOT__cmd_char_addr)
                               : ((__Vinline__act_sequent__TOP__0___VdfgRegularize_hebeb780c_0_0 
                                   & (IData)(vlSelfRef.test_copper_lists__DOT__dut__DOT__char_ram_sel))
                                   ? ((IData)(vlSelfRef.test_copper_lists__DOT__cpu_addr) 
                                      - (IData)(0x0200U))
                                   : ((IData)(vlSelfRef.test_copper_lists__DOT__dut__DOT__dbg_char_sel)
                                       ? ((IData)(vlSelfRef.test_copper_lists__DOT__dut__DOT__dbg_addr) 
                                          - (IData)(0x0200U))
                                       : (IData)(vlSelfRef.test_copper_lists__DOT__dut__DOT__cmd_char_addr)))));
        vlSelfRef.test_copper_lists__DOT__dut__DOT__color_a_addr 
            = (0x000007ffU & ((IData)(vlSelfRef.test_copper_lists__DOT__dut__DOT__cmd_color_we)
                               ? (IData)(vlSelfRef.test_copper_lists__DOT__dut__DOT__cmd_color_addr)
                               : ((__Vinline__act_sequent__TOP__0___VdfgRegularize_hebeb780c_0_0 
                                   & (IData)(vlSelfRef.test_copper_lists__DOT__dut__DOT__color_ram_sel))
                                   ? ((IData)(vlSelfRef.test_copper_lists__DOT__cpu_addr) 
                                      - (IData)(0x01d0U))
                                   : ((IData)(vlSelfRef.test_copper_lists__DOT__dut__DOT__dbg_color_sel)
                                       ? ((IData)(vlSelfRef.test_copper_lists__DOT__dut__DOT__dbg_addr) 
                                          - (IData)(0x01d0U))
                                       : (IData)(vlSelfRef.test_copper_lists__DOT__dut__DOT__cmd_color_addr)))));
    }
}

void Vtest_copper_lists___024root___nba_sequent__TOP__0(Vtest_copper_lists___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vtest_copper_lists___024root___nba_sequent__TOP__0\n"); );
    Vtest_copper_lists__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
    // Locals
    IData/*16:0*/ test_copper_lists__DOT__dut__DOT____Vlvbound_h3e9a9ec3__0;
    test_copper_lists__DOT__dut__DOT____Vlvbound_h3e9a9ec3__0 = 0;
    CData/*0:0*/ test_copper_lists__DOT__dut__DOT__visible;
    test_copper_lists__DOT__dut__DOT__visible = 0;
    CData/*4:0*/ test_copper_lists__DOT__dut__DOT__text_row;
    test_copper_lists__DOT__dut__DOT__text_row = 0;
    CData/*4:0*/ test_copper_lists__DOT__dut__DOT__real_row;
    test_copper_lists__DOT__dut__DOT__real_row = 0;
    SData/*9:0*/ test_copper_lists__DOT__dut__DOT__text_line;
    test_copper_lists__DOT__dut__DOT__text_line = 0;
    VlUnpacked<SData/*8:0*/, 16> test_copper_lists__DOT__dut__DOT__spr_x;
    for (int __Vi0 = 0; __Vi0 < 16; ++__Vi0) {
        test_copper_lists__DOT__dut__DOT__spr_x[__Vi0] = 0;
    }
    VlUnpacked<CData/*7:0*/, 16> test_copper_lists__DOT__dut__DOT__spr_y;
    for (int __Vi0 = 0; __Vi0 < 16; ++__Vi0) {
        test_copper_lists__DOT__dut__DOT__spr_y[__Vi0] = 0;
    }
    VlUnpacked<CData/*0:0*/, 16> test_copper_lists__DOT__dut__DOT__spr_enable;
    for (int __Vi0 = 0; __Vi0 < 16; ++__Vi0) {
        test_copper_lists__DOT__dut__DOT__spr_enable[__Vi0] = 0;
    }
    VlUnpacked<CData/*0:0*/, 16> test_copper_lists__DOT__dut__DOT__spr_flip_h;
    for (int __Vi0 = 0; __Vi0 < 16; ++__Vi0) {
        test_copper_lists__DOT__dut__DOT__spr_flip_h[__Vi0] = 0;
    }
    VlUnpacked<CData/*0:0*/, 16> test_copper_lists__DOT__dut__DOT__spr_flip_v;
    for (int __Vi0 = 0; __Vi0 < 16; ++__Vi0) {
        test_copper_lists__DOT__dut__DOT__spr_flip_v[__Vi0] = 0;
    }
    VlUnpacked<CData/*1:0*/, 16> test_copper_lists__DOT__dut__DOT__spr_pri;
    for (int __Vi0 = 0; __Vi0 < 16; ++__Vi0) {
        test_copper_lists__DOT__dut__DOT__spr_pri[__Vi0] = 0;
    }
    SData/*14:0*/ test_copper_lists__DOT__dut__DOT__tile_blt_addr;
    test_copper_lists__DOT__dut__DOT__tile_blt_addr = 0;
    CData/*7:0*/ test_copper_lists__DOT__dut__DOT__tile_blt_wdata;
    test_copper_lists__DOT__dut__DOT__tile_blt_wdata = 0;
    SData/*10:0*/ __Vfunc_test_copper_lists__DOT__dut__DOT__screen_addr__171__Vfuncout;
    __Vfunc_test_copper_lists__DOT__dut__DOT__screen_addr__171__Vfuncout = 0;
    CData/*6:0*/ __Vfunc_test_copper_lists__DOT__dut__DOT__screen_addr__171__col;
    __Vfunc_test_copper_lists__DOT__dut__DOT__screen_addr__171__col = 0;
    CData/*4:0*/ __Vfunc_test_copper_lists__DOT__dut__DOT__screen_addr__171__rr;
    __Vfunc_test_copper_lists__DOT__dut__DOT__screen_addr__171__rr = 0;
    SData/*10:0*/ __Vfunc_test_copper_lists__DOT__dut__DOT__screen_addr__172__Vfuncout;
    __Vfunc_test_copper_lists__DOT__dut__DOT__screen_addr__172__Vfuncout = 0;
    CData/*6:0*/ __Vfunc_test_copper_lists__DOT__dut__DOT__screen_addr__172__col;
    __Vfunc_test_copper_lists__DOT__dut__DOT__screen_addr__172__col = 0;
    CData/*4:0*/ __Vfunc_test_copper_lists__DOT__dut__DOT__screen_addr__172__rr;
    __Vfunc_test_copper_lists__DOT__dut__DOT__screen_addr__172__rr = 0;
    SData/*10:0*/ __Vfunc_test_copper_lists__DOT__dut__DOT__screen_addr__173__Vfuncout;
    __Vfunc_test_copper_lists__DOT__dut__DOT__screen_addr__173__Vfuncout = 0;
    CData/*6:0*/ __Vfunc_test_copper_lists__DOT__dut__DOT__screen_addr__173__col;
    __Vfunc_test_copper_lists__DOT__dut__DOT__screen_addr__173__col = 0;
    CData/*4:0*/ __Vfunc_test_copper_lists__DOT__dut__DOT__screen_addr__173__row;
    __Vfunc_test_copper_lists__DOT__dut__DOT__screen_addr__173__row = 0;
    CData/*4:0*/ __Vfunc_test_copper_lists__DOT__dut__DOT__screen_addr__173__rr;
    __Vfunc_test_copper_lists__DOT__dut__DOT__screen_addr__173__rr = 0;
    SData/*10:0*/ __Vfunc_test_copper_lists__DOT__dut__DOT__screen_addr__174__Vfuncout;
    __Vfunc_test_copper_lists__DOT__dut__DOT__screen_addr__174__Vfuncout = 0;
    CData/*6:0*/ __Vfunc_test_copper_lists__DOT__dut__DOT__screen_addr__174__col;
    __Vfunc_test_copper_lists__DOT__dut__DOT__screen_addr__174__col = 0;
    CData/*4:0*/ __Vfunc_test_copper_lists__DOT__dut__DOT__screen_addr__174__row;
    __Vfunc_test_copper_lists__DOT__dut__DOT__screen_addr__174__row = 0;
    CData/*4:0*/ __Vfunc_test_copper_lists__DOT__dut__DOT__screen_addr__174__rr;
    __Vfunc_test_copper_lists__DOT__dut__DOT__screen_addr__174__rr = 0;
    SData/*10:0*/ __Vfunc_test_copper_lists__DOT__dut__DOT__screen_addr__175__Vfuncout;
    __Vfunc_test_copper_lists__DOT__dut__DOT__screen_addr__175__Vfuncout = 0;
    CData/*6:0*/ __Vfunc_test_copper_lists__DOT__dut__DOT__screen_addr__175__col;
    __Vfunc_test_copper_lists__DOT__dut__DOT__screen_addr__175__col = 0;
    CData/*4:0*/ __Vfunc_test_copper_lists__DOT__dut__DOT__screen_addr__175__row;
    __Vfunc_test_copper_lists__DOT__dut__DOT__screen_addr__175__row = 0;
    CData/*4:0*/ __Vfunc_test_copper_lists__DOT__dut__DOT__screen_addr__175__rr;
    __Vfunc_test_copper_lists__DOT__dut__DOT__screen_addr__175__rr = 0;
    CData/*7:0*/ __Vfunc_test_copper_lists__DOT__dut__DOT__artist_inst__DOT__gt_char__176__Vfuncout;
    __Vfunc_test_copper_lists__DOT__dut__DOT__artist_inst__DOT__gt_char__176__Vfuncout = 0;
    CData/*5:0*/ __Vfunc_test_copper_lists__DOT__dut__DOT__artist_inst__DOT__gt_char__176__idx;
    __Vfunc_test_copper_lists__DOT__dut__DOT__artist_inst__DOT__gt_char__176__idx = 0;
    CData/*7:0*/ __Vfunc_test_copper_lists__DOT__dut__DOT__artist_inst__DOT__gt_char__177__Vfuncout;
    __Vfunc_test_copper_lists__DOT__dut__DOT__artist_inst__DOT__gt_char__177__Vfuncout = 0;
    CData/*5:0*/ __Vfunc_test_copper_lists__DOT__dut__DOT__artist_inst__DOT__gt_char__177__idx;
    __Vfunc_test_copper_lists__DOT__dut__DOT__artist_inst__DOT__gt_char__177__idx = 0;
    CData/*7:0*/ __Vfunc_test_copper_lists__DOT__dut__DOT__artist_inst__DOT__gt_char__178__Vfuncout;
    __Vfunc_test_copper_lists__DOT__dut__DOT__artist_inst__DOT__gt_char__178__Vfuncout = 0;
    CData/*7:0*/ __Vfunc_test_copper_lists__DOT__dut__DOT__artist_inst__DOT__gt_char__179__Vfuncout;
    __Vfunc_test_copper_lists__DOT__dut__DOT__artist_inst__DOT__gt_char__179__Vfuncout = 0;
    CData/*0:0*/ __VdfgRegularize_hebeb780c_0_0;
    __VdfgRegularize_hebeb780c_0_0 = 0;
    CData/*6:0*/ __Vdly__test_copper_lists__DOT__dut__DOT__cursor_x;
    __Vdly__test_copper_lists__DOT__dut__DOT__cursor_x = 0;
    CData/*4:0*/ __Vdly__test_copper_lists__DOT__dut__DOT__cursor_y;
    __Vdly__test_copper_lists__DOT__dut__DOT__cursor_y = 0;
    CData/*3:0*/ __Vdly__test_copper_lists__DOT__dut__DOT__fg_color;
    __Vdly__test_copper_lists__DOT__dut__DOT__fg_color = 0;
    CData/*4:0*/ __Vdly__test_copper_lists__DOT__dut__DOT__scroll_offset;
    __Vdly__test_copper_lists__DOT__dut__DOT__scroll_offset = 0;
    CData/*0:0*/ __Vdly__test_copper_lists__DOT__dut__DOT__scroll_pending;
    __Vdly__test_copper_lists__DOT__dut__DOT__scroll_pending = 0;
    CData/*6:0*/ __Vdly__test_copper_lists__DOT__dut__DOT__scroll_col;
    __Vdly__test_copper_lists__DOT__dut__DOT__scroll_col = 0;
    CData/*0:0*/ __Vdly__test_copper_lists__DOT__dut__DOT__cmd_busy;
    __Vdly__test_copper_lists__DOT__dut__DOT__cmd_busy = 0;
    CData/*0:0*/ __Vdly__test_copper_lists__DOT__dut__DOT__copper_enabled;
    __Vdly__test_copper_lists__DOT__dut__DOT__copper_enabled = 0;
    SData/*8:0*/ __Vdly__test_copper_lists__DOT__dut__DOT__copper_count;
    __Vdly__test_copper_lists__DOT__dut__DOT__copper_count = 0;
    CData/*6:0*/ __Vdly__test_copper_lists__DOT__dut__DOT__copper_target_list;
    __Vdly__test_copper_lists__DOT__dut__DOT__copper_target_list = 0;
    CData/*6:0*/ __Vdly__test_copper_lists__DOT__dut__DOT__copper_pending_list;
    __Vdly__test_copper_lists__DOT__dut__DOT__copper_pending_list = 0;
    CData/*0:0*/ __Vdly__test_copper_lists__DOT__dut__DOT__copper_loading;
    __Vdly__test_copper_lists__DOT__dut__DOT__copper_loading = 0;
    CData/*2:0*/ __Vdly__test_copper_lists__DOT__dut__DOT__sprrow_count;
    __Vdly__test_copper_lists__DOT__dut__DOT__sprrow_count = 0;
    CData/*0:0*/ __Vdly__test_copper_lists__DOT__dut__DOT__sprcopy_phase;
    __Vdly__test_copper_lists__DOT__dut__DOT__sprcopy_phase = 0;
    CData/*0:0*/ __Vdly__test_copper_lists__DOT__dut__DOT__memread_pending;
    __Vdly__test_copper_lists__DOT__dut__DOT__memread_pending = 0;
    SData/*8:0*/ __Vdly__test_copper_lists__DOT__dut__DOT__cmd_cx;
    __Vdly__test_copper_lists__DOT__dut__DOT__cmd_cx = 0;
    CData/*7:0*/ __Vdly__test_copper_lists__DOT__dut__DOT__cmd_cy;
    __Vdly__test_copper_lists__DOT__dut__DOT__cmd_cy = 0;
    CData/*6:0*/ __Vdly__test_copper_lists__DOT__dut__DOT__copper_active_list;
    __Vdly__test_copper_lists__DOT__dut__DOT__copper_active_list = 0;
    CData/*7:0*/ __Vdly__test_copper_lists__DOT__dut__DOT__cmd_op;
    __Vdly__test_copper_lists__DOT__dut__DOT__cmd_op = 0;
    SData/*9:0*/ __Vdly__test_copper_lists__DOT__dut__DOT__h_count;
    __Vdly__test_copper_lists__DOT__dut__DOT__h_count = 0;
    SData/*9:0*/ __Vdly__test_copper_lists__DOT__dut__DOT__v_count;
    __Vdly__test_copper_lists__DOT__dut__DOT__v_count = 0;
    CData/*0:0*/ __Vdly__test_copper_lists__DOT__dut__DOT__artist_busy;
    __Vdly__test_copper_lists__DOT__dut__DOT__artist_busy = 0;
    CData/*7:0*/ __Vdly__test_copper_lists__DOT__dut__DOT__artist_inst__DOT__op;
    __Vdly__test_copper_lists__DOT__dut__DOT__artist_inst__DOT__op = 0;
    SData/*9:0*/ __Vdly__test_copper_lists__DOT__dut__DOT__artist_inst__DOT__fill_sp;
    __Vdly__test_copper_lists__DOT__dut__DOT__artist_inst__DOT__fill_sp = 0;
    CData/*2:0*/ __Vdly__test_copper_lists__DOT__dut__DOT__artist_inst__DOT__octant_count;
    __Vdly__test_copper_lists__DOT__dut__DOT__artist_inst__DOT__octant_count = 0;
    CData/*2:0*/ __Vdly__test_copper_lists__DOT__dut__DOT__artist_inst__DOT__paint_phase;
    __Vdly__test_copper_lists__DOT__dut__DOT__artist_inst__DOT__paint_phase = 0;
    CData/*1:0*/ __Vdly__test_copper_lists__DOT__dut__DOT__artist_inst__DOT__gt_font_pending;
    __Vdly__test_copper_lists__DOT__dut__DOT__artist_inst__DOT__gt_font_pending = 0;
    CData/*0:0*/ __Vdly__test_copper_lists__DOT__dut__DOT__artist_inst__DOT__paint_init;
    __Vdly__test_copper_lists__DOT__dut__DOT__artist_inst__DOT__paint_init = 0;
    CData/*7:0*/ __Vdly__test_copper_lists__DOT__dut__DOT__artist_inst__DOT__gt_font_byte;
    __Vdly__test_copper_lists__DOT__dut__DOT__artist_inst__DOT__gt_font_byte = 0;
    SData/*9:0*/ __Vdly__test_copper_lists__DOT__dut__DOT__artist_inst__DOT__err;
    __Vdly__test_copper_lists__DOT__dut__DOT__artist_inst__DOT__err = 0;
    SData/*9:0*/ __Vdly__test_copper_lists__DOT__dut__DOT__artist_inst__DOT__x;
    __Vdly__test_copper_lists__DOT__dut__DOT__artist_inst__DOT__x = 0;
    SData/*9:0*/ __Vdly__test_copper_lists__DOT__dut__DOT__artist_inst__DOT__y;
    __Vdly__test_copper_lists__DOT__dut__DOT__artist_inst__DOT__y = 0;
    SData/*8:0*/ __Vdly__test_copper_lists__DOT__dut__DOT__artist_inst__DOT__cx;
    __Vdly__test_copper_lists__DOT__dut__DOT__artist_inst__DOT__cx = 0;
    CData/*7:0*/ __Vdly__test_copper_lists__DOT__dut__DOT__artist_inst__DOT__cy;
    __Vdly__test_copper_lists__DOT__dut__DOT__artist_inst__DOT__cy = 0;
    SData/*9:0*/ __Vdly__test_copper_lists__DOT__dut__DOT__artist_inst__DOT__dx;
    __Vdly__test_copper_lists__DOT__dut__DOT__artist_inst__DOT__dx = 0;
    SData/*9:0*/ __Vdly__test_copper_lists__DOT__dut__DOT__artist_inst__DOT__dy;
    __Vdly__test_copper_lists__DOT__dut__DOT__artist_inst__DOT__dy = 0;
    CData/*3:0*/ __Vdly__test_copper_lists__DOT__dut__DOT__artist_inst__DOT__fill_target;
    __Vdly__test_copper_lists__DOT__dut__DOT__artist_inst__DOT__fill_target = 0;
    SData/*8:0*/ __Vdly__test_copper_lists__DOT__dut__DOT__artist_inst__DOT__fs_b_addr;
    __Vdly__test_copper_lists__DOT__dut__DOT__artist_inst__DOT__fs_b_addr = 0;
    CData/*2:0*/ __Vdly__test_copper_lists__DOT__dut__DOT__artist_inst__DOT__gt_font_row;
    __Vdly__test_copper_lists__DOT__dut__DOT__artist_inst__DOT__gt_font_row = 0;
    CData/*5:0*/ __Vdly__test_copper_lists__DOT__dut__DOT__artist_inst__DOT__gt_char_idx;
    __Vdly__test_copper_lists__DOT__dut__DOT__artist_inst__DOT__gt_char_idx = 0;
    SData/*8:0*/ __Vdly__test_copper_lists__DOT__dut__DOT__copper_index;
    __Vdly__test_copper_lists__DOT__dut__DOT__copper_index = 0;
    CData/*4:0*/ __Vdly__test_copper_lists__DOT__dut__DOT__tile_inst__DOT__dma_state;
    __Vdly__test_copper_lists__DOT__dut__DOT__tile_inst__DOT__dma_state = 0;
    CData/*0:0*/ __Vdly__test_copper_lists__DOT__dut__DOT__tile_inst__DOT__dma_data_valid;
    __Vdly__test_copper_lists__DOT__dut__DOT__tile_inst__DOT__dma_data_valid = 0;
    CData/*4:0*/ __Vdly__test_copper_lists__DOT__dut__DOT__tile_inst__DOT__dma_col_idx;
    __Vdly__test_copper_lists__DOT__dut__DOT__tile_inst__DOT__dma_col_idx = 0;
    SData/*15:0*/ __Vdly__test_copper_lists__DOT__dut__DOT__tile_inst__DOT__dma_count;
    __Vdly__test_copper_lists__DOT__dut__DOT__tile_inst__DOT__dma_count = 0;
    SData/*15:0*/ __Vdly__test_copper_lists__DOT__dut__DOT__tile_inst__DOT__dma_src;
    __Vdly__test_copper_lists__DOT__dut__DOT__tile_inst__DOT__dma_src = 0;
    SData/*15:0*/ __Vdly__test_copper_lists__DOT__dut__DOT__tile_inst__DOT__dma_dst;
    __Vdly__test_copper_lists__DOT__dut__DOT__tile_inst__DOT__dma_dst = 0;
    CData/*7:0*/ __Vdly__test_copper_lists__DOT__dut__DOT__tile_inst__DOT__dma_pal_phase;
    __Vdly__test_copper_lists__DOT__dut__DOT__tile_inst__DOT__dma_pal_phase = 0;
    CData/*0:0*/ __VdlySet__test_copper_lists__DOT__dut__DOT__regs__v0;
    __VdlySet__test_copper_lists__DOT__dut__DOT__regs__v0 = 0;
    CData/*7:0*/ __VdlyVal__test_copper_lists__DOT__dut__DOT__regs__v32;
    __VdlyVal__test_copper_lists__DOT__dut__DOT__regs__v32 = 0;
    CData/*0:0*/ __VdlySet__test_copper_lists__DOT__dut__DOT__regs__v32;
    __VdlySet__test_copper_lists__DOT__dut__DOT__regs__v32 = 0;
    CData/*7:0*/ __VdlyVal__test_copper_lists__DOT__dut__DOT__regs__v33;
    __VdlyVal__test_copper_lists__DOT__dut__DOT__regs__v33 = 0;
    CData/*0:0*/ __VdlySet__test_copper_lists__DOT__dut__DOT__regs__v33;
    __VdlySet__test_copper_lists__DOT__dut__DOT__regs__v33 = 0;
    IData/*16:0*/ __VdlyVal__test_copper_lists__DOT__dut__DOT__copper_pos__v0;
    __VdlyVal__test_copper_lists__DOT__dut__DOT__copper_pos__v0 = 0;
    CData/*4:0*/ __VdlyDim0__test_copper_lists__DOT__dut__DOT__copper_pos__v0;
    __VdlyDim0__test_copper_lists__DOT__dut__DOT__copper_pos__v0 = 0;
    CData/*0:0*/ __VdlySet__test_copper_lists__DOT__dut__DOT__copper_pos__v0;
    __VdlySet__test_copper_lists__DOT__dut__DOT__copper_pos__v0 = 0;
    CData/*7:0*/ __VdlyVal__test_copper_lists__DOT__dut__DOT__copper_reg__v0;
    __VdlyVal__test_copper_lists__DOT__dut__DOT__copper_reg__v0 = 0;
    CData/*4:0*/ __VdlyDim0__test_copper_lists__DOT__dut__DOT__copper_reg__v0;
    __VdlyDim0__test_copper_lists__DOT__dut__DOT__copper_reg__v0 = 0;
    CData/*7:0*/ __VdlyVal__test_copper_lists__DOT__dut__DOT__copper_val__v0;
    __VdlyVal__test_copper_lists__DOT__dut__DOT__copper_val__v0 = 0;
    CData/*4:0*/ __VdlyDim0__test_copper_lists__DOT__dut__DOT__copper_val__v0;
    __VdlyDim0__test_copper_lists__DOT__dut__DOT__copper_val__v0 = 0;
    CData/*7:0*/ __VdlyVal__test_copper_lists__DOT__dut__DOT__regs__v34;
    __VdlyVal__test_copper_lists__DOT__dut__DOT__regs__v34 = 0;
    CData/*4:0*/ __VdlyDim0__test_copper_lists__DOT__dut__DOT__regs__v34;
    __VdlyDim0__test_copper_lists__DOT__dut__DOT__regs__v34 = 0;
    CData/*0:0*/ __VdlySet__test_copper_lists__DOT__dut__DOT__regs__v34;
    __VdlySet__test_copper_lists__DOT__dut__DOT__regs__v34 = 0;
    CData/*6:0*/ __VdlyDim0__test_copper_lists__DOT__dut__DOT__copper_list_count__v0;
    __VdlyDim0__test_copper_lists__DOT__dut__DOT__copper_list_count__v0 = 0;
    CData/*6:0*/ __VdlyDim0__test_copper_lists__DOT__dut__DOT__copper_list_count__v1;
    __VdlyDim0__test_copper_lists__DOT__dut__DOT__copper_list_count__v1 = 0;
    IData/*16:0*/ __VdlyVal__test_copper_lists__DOT__dut__DOT__copper_list_pos__v0;
    __VdlyVal__test_copper_lists__DOT__dut__DOT__copper_list_pos__v0 = 0;
    SData/*11:0*/ __VdlyDim0__test_copper_lists__DOT__dut__DOT__copper_list_pos__v0;
    __VdlyDim0__test_copper_lists__DOT__dut__DOT__copper_list_pos__v0 = 0;
    CData/*0:0*/ __VdlySet__test_copper_lists__DOT__dut__DOT__copper_list_pos__v0;
    __VdlySet__test_copper_lists__DOT__dut__DOT__copper_list_pos__v0 = 0;
    CData/*7:0*/ __VdlyVal__test_copper_lists__DOT__dut__DOT__copper_list_reg__v0;
    __VdlyVal__test_copper_lists__DOT__dut__DOT__copper_list_reg__v0 = 0;
    SData/*11:0*/ __VdlyDim0__test_copper_lists__DOT__dut__DOT__copper_list_reg__v0;
    __VdlyDim0__test_copper_lists__DOT__dut__DOT__copper_list_reg__v0 = 0;
    CData/*7:0*/ __VdlyVal__test_copper_lists__DOT__dut__DOT__copper_list_val__v0;
    __VdlyVal__test_copper_lists__DOT__dut__DOT__copper_list_val__v0 = 0;
    SData/*11:0*/ __VdlyDim0__test_copper_lists__DOT__dut__DOT__copper_list_val__v0;
    __VdlyDim0__test_copper_lists__DOT__dut__DOT__copper_list_val__v0 = 0;
    SData/*8:0*/ __VdlyVal__test_copper_lists__DOT__dut__DOT__copper_list_count__v2;
    __VdlyVal__test_copper_lists__DOT__dut__DOT__copper_list_count__v2 = 0;
    CData/*6:0*/ __VdlyDim0__test_copper_lists__DOT__dut__DOT__copper_list_count__v2;
    __VdlyDim0__test_copper_lists__DOT__dut__DOT__copper_list_count__v2 = 0;
    IData/*16:0*/ __VdlyVal__test_copper_lists__DOT__dut__DOT__copper_pos__v1;
    __VdlyVal__test_copper_lists__DOT__dut__DOT__copper_pos__v1 = 0;
    CData/*4:0*/ __VdlyDim0__test_copper_lists__DOT__dut__DOT__copper_pos__v1;
    __VdlyDim0__test_copper_lists__DOT__dut__DOT__copper_pos__v1 = 0;
    CData/*0:0*/ __VdlySet__test_copper_lists__DOT__dut__DOT__copper_pos__v1;
    __VdlySet__test_copper_lists__DOT__dut__DOT__copper_pos__v1 = 0;
    CData/*7:0*/ __VdlyVal__test_copper_lists__DOT__dut__DOT__copper_reg__v1;
    __VdlyVal__test_copper_lists__DOT__dut__DOT__copper_reg__v1 = 0;
    CData/*4:0*/ __VdlyDim0__test_copper_lists__DOT__dut__DOT__copper_reg__v1;
    __VdlyDim0__test_copper_lists__DOT__dut__DOT__copper_reg__v1 = 0;
    CData/*7:0*/ __VdlyVal__test_copper_lists__DOT__dut__DOT__copper_val__v1;
    __VdlyVal__test_copper_lists__DOT__dut__DOT__copper_val__v1 = 0;
    CData/*4:0*/ __VdlyDim0__test_copper_lists__DOT__dut__DOT__copper_val__v1;
    __VdlyDim0__test_copper_lists__DOT__dut__DOT__copper_val__v1 = 0;
    CData/*1:0*/ __VdlyVal__test_copper_lists__DOT__dut__DOT__spr_pri__v16;
    __VdlyVal__test_copper_lists__DOT__dut__DOT__spr_pri__v16 = 0;
    CData/*3:0*/ __VdlyDim0__test_copper_lists__DOT__dut__DOT__spr_pri__v16;
    __VdlyDim0__test_copper_lists__DOT__dut__DOT__spr_pri__v16 = 0;
    CData/*0:0*/ __VdlySet__test_copper_lists__DOT__dut__DOT__spr_pri__v16;
    __VdlySet__test_copper_lists__DOT__dut__DOT__spr_pri__v16 = 0;
    CData/*0:0*/ __VdlyVal__test_copper_lists__DOT__dut__DOT__spr_flip_h__v16;
    __VdlyVal__test_copper_lists__DOT__dut__DOT__spr_flip_h__v16 = 0;
    CData/*3:0*/ __VdlyDim0__test_copper_lists__DOT__dut__DOT__spr_flip_h__v16;
    __VdlyDim0__test_copper_lists__DOT__dut__DOT__spr_flip_h__v16 = 0;
    CData/*0:0*/ __VdlySet__test_copper_lists__DOT__dut__DOT__spr_flip_h__v16;
    __VdlySet__test_copper_lists__DOT__dut__DOT__spr_flip_h__v16 = 0;
    CData/*0:0*/ __VdlyVal__test_copper_lists__DOT__dut__DOT__spr_flip_v__v16;
    __VdlyVal__test_copper_lists__DOT__dut__DOT__spr_flip_v__v16 = 0;
    CData/*3:0*/ __VdlyDim0__test_copper_lists__DOT__dut__DOT__spr_flip_v__v16;
    __VdlyDim0__test_copper_lists__DOT__dut__DOT__spr_flip_v__v16 = 0;
    CData/*3:0*/ __VdlyDim0__test_copper_lists__DOT__dut__DOT__spr_enable__v16;
    __VdlyDim0__test_copper_lists__DOT__dut__DOT__spr_enable__v16 = 0;
    CData/*0:0*/ __VdlySet__test_copper_lists__DOT__dut__DOT__spr_enable__v16;
    __VdlySet__test_copper_lists__DOT__dut__DOT__spr_enable__v16 = 0;
    CData/*3:0*/ __VdlyDim0__test_copper_lists__DOT__dut__DOT__spr_enable__v17;
    __VdlyDim0__test_copper_lists__DOT__dut__DOT__spr_enable__v17 = 0;
    CData/*0:0*/ __VdlySet__test_copper_lists__DOT__dut__DOT__spr_enable__v17;
    __VdlySet__test_copper_lists__DOT__dut__DOT__spr_enable__v17 = 0;
    SData/*8:0*/ __VdlyVal__test_copper_lists__DOT__dut__DOT__spr_x__v16;
    __VdlyVal__test_copper_lists__DOT__dut__DOT__spr_x__v16 = 0;
    CData/*3:0*/ __VdlyDim0__test_copper_lists__DOT__dut__DOT__spr_x__v16;
    __VdlyDim0__test_copper_lists__DOT__dut__DOT__spr_x__v16 = 0;
    CData/*0:0*/ __VdlySet__test_copper_lists__DOT__dut__DOT__spr_x__v16;
    __VdlySet__test_copper_lists__DOT__dut__DOT__spr_x__v16 = 0;
    CData/*7:0*/ __VdlyVal__test_copper_lists__DOT__dut__DOT__spr_y__v16;
    __VdlyVal__test_copper_lists__DOT__dut__DOT__spr_y__v16 = 0;
    CData/*3:0*/ __VdlyDim0__test_copper_lists__DOT__dut__DOT__spr_y__v16;
    __VdlyDim0__test_copper_lists__DOT__dut__DOT__spr_y__v16 = 0;
    CData/*7:0*/ __VdlyVal__test_copper_lists__DOT__dut__DOT__sprrow_data__v0;
    __VdlyVal__test_copper_lists__DOT__dut__DOT__sprrow_data__v0 = 0;
    CData/*0:0*/ __VdlySet__test_copper_lists__DOT__dut__DOT__sprrow_data__v0;
    __VdlySet__test_copper_lists__DOT__dut__DOT__sprrow_data__v0 = 0;
    CData/*7:0*/ __VdlyVal__test_copper_lists__DOT__dut__DOT__sprrow_data__v1;
    __VdlyVal__test_copper_lists__DOT__dut__DOT__sprrow_data__v1 = 0;
    CData/*7:0*/ __VdlyVal__test_copper_lists__DOT__dut__DOT__sprrow_data__v2;
    __VdlyVal__test_copper_lists__DOT__dut__DOT__sprrow_data__v2 = 0;
    CData/*7:0*/ __VdlyVal__test_copper_lists__DOT__dut__DOT__sprrow_data__v3;
    __VdlyVal__test_copper_lists__DOT__dut__DOT__sprrow_data__v3 = 0;
    CData/*7:0*/ __VdlyVal__test_copper_lists__DOT__dut__DOT__sprrow_data__v4;
    __VdlyVal__test_copper_lists__DOT__dut__DOT__sprrow_data__v4 = 0;
    CData/*7:0*/ __VdlyVal__test_copper_lists__DOT__dut__DOT__sprrow_data__v5;
    __VdlyVal__test_copper_lists__DOT__dut__DOT__sprrow_data__v5 = 0;
    CData/*7:0*/ __VdlyVal__test_copper_lists__DOT__dut__DOT__sprrow_data__v6;
    __VdlyVal__test_copper_lists__DOT__dut__DOT__sprrow_data__v6 = 0;
    CData/*7:0*/ __VdlyVal__test_copper_lists__DOT__dut__DOT__sprrow_data__v7;
    __VdlyVal__test_copper_lists__DOT__dut__DOT__sprrow_data__v7 = 0;
    CData/*7:0*/ __VdlyVal__test_copper_lists__DOT__dut__DOT__regs__v35;
    __VdlyVal__test_copper_lists__DOT__dut__DOT__regs__v35 = 0;
    CData/*4:0*/ __VdlyDim0__test_copper_lists__DOT__dut__DOT__regs__v35;
    __VdlyDim0__test_copper_lists__DOT__dut__DOT__regs__v35 = 0;
    CData/*0:0*/ __VdlySet__test_copper_lists__DOT__dut__DOT__regs__v35;
    __VdlySet__test_copper_lists__DOT__dut__DOT__regs__v35 = 0;
    CData/*1:0*/ __VdlyVal__test_copper_lists__DOT__dut__DOT__spr_pri__v17;
    __VdlyVal__test_copper_lists__DOT__dut__DOT__spr_pri__v17 = 0;
    CData/*3:0*/ __VdlyDim0__test_copper_lists__DOT__dut__DOT__spr_pri__v17;
    __VdlyDim0__test_copper_lists__DOT__dut__DOT__spr_pri__v17 = 0;
    CData/*0:0*/ __VdlySet__test_copper_lists__DOT__dut__DOT__spr_pri__v17;
    __VdlySet__test_copper_lists__DOT__dut__DOT__spr_pri__v17 = 0;
    CData/*0:0*/ __VdlyVal__test_copper_lists__DOT__dut__DOT__spr_flip_h__v17;
    __VdlyVal__test_copper_lists__DOT__dut__DOT__spr_flip_h__v17 = 0;
    CData/*3:0*/ __VdlyDim0__test_copper_lists__DOT__dut__DOT__spr_flip_h__v17;
    __VdlyDim0__test_copper_lists__DOT__dut__DOT__spr_flip_h__v17 = 0;
    CData/*0:0*/ __VdlySet__test_copper_lists__DOT__dut__DOT__spr_flip_h__v17;
    __VdlySet__test_copper_lists__DOT__dut__DOT__spr_flip_h__v17 = 0;
    CData/*0:0*/ __VdlyVal__test_copper_lists__DOT__dut__DOT__spr_flip_v__v17;
    __VdlyVal__test_copper_lists__DOT__dut__DOT__spr_flip_v__v17 = 0;
    CData/*3:0*/ __VdlyDim0__test_copper_lists__DOT__dut__DOT__spr_flip_v__v17;
    __VdlyDim0__test_copper_lists__DOT__dut__DOT__spr_flip_v__v17 = 0;
    CData/*0:0*/ __VdlyVal__test_copper_lists__DOT__dut__DOT__spr_enable__v18;
    __VdlyVal__test_copper_lists__DOT__dut__DOT__spr_enable__v18 = 0;
    CData/*3:0*/ __VdlyDim0__test_copper_lists__DOT__dut__DOT__spr_enable__v18;
    __VdlyDim0__test_copper_lists__DOT__dut__DOT__spr_enable__v18 = 0;
    CData/*7:0*/ __VdlyVal__test_copper_lists__DOT__dut__DOT__spr_y__v17;
    __VdlyVal__test_copper_lists__DOT__dut__DOT__spr_y__v17 = 0;
    CData/*3:0*/ __VdlyDim0__test_copper_lists__DOT__dut__DOT__spr_y__v17;
    __VdlyDim0__test_copper_lists__DOT__dut__DOT__spr_y__v17 = 0;
    CData/*0:0*/ __VdlySet__test_copper_lists__DOT__dut__DOT__spr_y__v17;
    __VdlySet__test_copper_lists__DOT__dut__DOT__spr_y__v17 = 0;
    CData/*0:0*/ __VdlyVal__test_copper_lists__DOT__dut__DOT__spr_x__v17;
    __VdlyVal__test_copper_lists__DOT__dut__DOT__spr_x__v17 = 0;
    CData/*3:0*/ __VdlyDim0__test_copper_lists__DOT__dut__DOT__spr_x__v17;
    __VdlyDim0__test_copper_lists__DOT__dut__DOT__spr_x__v17 = 0;
    CData/*0:0*/ __VdlySet__test_copper_lists__DOT__dut__DOT__spr_x__v17;
    __VdlySet__test_copper_lists__DOT__dut__DOT__spr_x__v17 = 0;
    CData/*7:0*/ __VdlyVal__test_copper_lists__DOT__dut__DOT__spr_x__v18;
    __VdlyVal__test_copper_lists__DOT__dut__DOT__spr_x__v18 = 0;
    CData/*3:0*/ __VdlyDim0__test_copper_lists__DOT__dut__DOT__spr_x__v18;
    __VdlyDim0__test_copper_lists__DOT__dut__DOT__spr_x__v18 = 0;
    CData/*0:0*/ __VdlySet__test_copper_lists__DOT__dut__DOT__spr_x__v18;
    __VdlySet__test_copper_lists__DOT__dut__DOT__spr_x__v18 = 0;
    CData/*7:0*/ __VdlyVal__test_copper_lists__DOT__dut__DOT__fio_name__v0;
    __VdlyVal__test_copper_lists__DOT__dut__DOT__fio_name__v0 = 0;
    CData/*5:0*/ __VdlyDim0__test_copper_lists__DOT__dut__DOT__fio_name__v0;
    __VdlyDim0__test_copper_lists__DOT__dut__DOT__fio_name__v0 = 0;
    CData/*0:0*/ __VdlySet__test_copper_lists__DOT__dut__DOT__fio_name__v0;
    __VdlySet__test_copper_lists__DOT__dut__DOT__fio_name__v0 = 0;
    CData/*7:0*/ __VdlyVal__test_copper_lists__DOT__dut__DOT__text_inst__DOT__char_mem__DOT__mem__v0;
    __VdlyVal__test_copper_lists__DOT__dut__DOT__text_inst__DOT__char_mem__DOT__mem__v0 = 0;
    SData/*10:0*/ __VdlyDim0__test_copper_lists__DOT__dut__DOT__text_inst__DOT__char_mem__DOT__mem__v0;
    __VdlyDim0__test_copper_lists__DOT__dut__DOT__text_inst__DOT__char_mem__DOT__mem__v0 = 0;
    CData/*0:0*/ __VdlySet__test_copper_lists__DOT__dut__DOT__text_inst__DOT__char_mem__DOT__mem__v0;
    __VdlySet__test_copper_lists__DOT__dut__DOT__text_inst__DOT__char_mem__DOT__mem__v0 = 0;
    CData/*7:0*/ __VdlyVal__test_copper_lists__DOT__dut__DOT__text_inst__DOT__color_mem__DOT__mem__v0;
    __VdlyVal__test_copper_lists__DOT__dut__DOT__text_inst__DOT__color_mem__DOT__mem__v0 = 0;
    SData/*10:0*/ __VdlyDim0__test_copper_lists__DOT__dut__DOT__text_inst__DOT__color_mem__DOT__mem__v0;
    __VdlyDim0__test_copper_lists__DOT__dut__DOT__text_inst__DOT__color_mem__DOT__mem__v0 = 0;
    CData/*0:0*/ __VdlySet__test_copper_lists__DOT__dut__DOT__text_inst__DOT__color_mem__DOT__mem__v0;
    __VdlySet__test_copper_lists__DOT__dut__DOT__text_inst__DOT__color_mem__DOT__mem__v0 = 0;
    CData/*3:0*/ __VdlyVal__test_copper_lists__DOT__dut__DOT__gfx_inst__DOT__gfx_mem__DOT__mem__v0;
    __VdlyVal__test_copper_lists__DOT__dut__DOT__gfx_inst__DOT__gfx_mem__DOT__mem__v0 = 0;
    SData/*15:0*/ __VdlyDim0__test_copper_lists__DOT__dut__DOT__gfx_inst__DOT__gfx_mem__DOT__mem__v0;
    __VdlyDim0__test_copper_lists__DOT__dut__DOT__gfx_inst__DOT__gfx_mem__DOT__mem__v0 = 0;
    CData/*0:0*/ __VdlySet__test_copper_lists__DOT__dut__DOT__gfx_inst__DOT__gfx_mem__DOT__mem__v0;
    __VdlySet__test_copper_lists__DOT__dut__DOT__gfx_inst__DOT__gfx_mem__DOT__mem__v0 = 0;
    IData/*16:0*/ __VdlyVal__test_copper_lists__DOT__dut__DOT__artist_inst__DOT__push_neighbors__v0;
    __VdlyVal__test_copper_lists__DOT__dut__DOT__artist_inst__DOT__push_neighbors__v0 = 0;
    CData/*0:0*/ __VdlySet__test_copper_lists__DOT__dut__DOT__artist_inst__DOT__push_neighbors__v0;
    __VdlySet__test_copper_lists__DOT__dut__DOT__artist_inst__DOT__push_neighbors__v0 = 0;
    IData/*16:0*/ __VdlyVal__test_copper_lists__DOT__dut__DOT__artist_inst__DOT__push_neighbors__v1;
    __VdlyVal__test_copper_lists__DOT__dut__DOT__artist_inst__DOT__push_neighbors__v1 = 0;
    IData/*16:0*/ __VdlyVal__test_copper_lists__DOT__dut__DOT__artist_inst__DOT__push_neighbors__v2;
    __VdlyVal__test_copper_lists__DOT__dut__DOT__artist_inst__DOT__push_neighbors__v2 = 0;
    IData/*16:0*/ __VdlyVal__test_copper_lists__DOT__dut__DOT__artist_inst__DOT__push_neighbors__v3;
    __VdlyVal__test_copper_lists__DOT__dut__DOT__artist_inst__DOT__push_neighbors__v3 = 0;
    IData/*16:0*/ __VdlyVal__test_copper_lists__DOT__dut__DOT__artist_inst__DOT__fill_stack_mem__DOT__mem__v0;
    __VdlyVal__test_copper_lists__DOT__dut__DOT__artist_inst__DOT__fill_stack_mem__DOT__mem__v0 = 0;
    SData/*8:0*/ __VdlyDim0__test_copper_lists__DOT__dut__DOT__artist_inst__DOT__fill_stack_mem__DOT__mem__v0;
    __VdlyDim0__test_copper_lists__DOT__dut__DOT__artist_inst__DOT__fill_stack_mem__DOT__mem__v0 = 0;
    CData/*0:0*/ __VdlySet__test_copper_lists__DOT__dut__DOT__artist_inst__DOT__fill_stack_mem__DOT__mem__v0;
    __VdlySet__test_copper_lists__DOT__dut__DOT__artist_inst__DOT__fill_stack_mem__DOT__mem__v0 = 0;
    CData/*7:0*/ __VdlyVal__test_copper_lists__DOT__dut__DOT__sprite_inst__DOT__spr_mem__DOT__mem__v0;
    __VdlyVal__test_copper_lists__DOT__dut__DOT__sprite_inst__DOT__spr_mem__DOT__mem__v0 = 0;
    SData/*10:0*/ __VdlyDim0__test_copper_lists__DOT__dut__DOT__sprite_inst__DOT__spr_mem__DOT__mem__v0;
    __VdlyDim0__test_copper_lists__DOT__dut__DOT__sprite_inst__DOT__spr_mem__DOT__mem__v0 = 0;
    CData/*0:0*/ __VdlySet__test_copper_lists__DOT__dut__DOT__sprite_inst__DOT__spr_mem__DOT__mem__v0;
    __VdlySet__test_copper_lists__DOT__dut__DOT__sprite_inst__DOT__spr_mem__DOT__mem__v0 = 0;
    CData/*7:0*/ __VdlyVal__test_copper_lists__DOT__dut__DOT__tile_inst__DOT__col_buffer__v0;
    __VdlyVal__test_copper_lists__DOT__dut__DOT__tile_inst__DOT__col_buffer__v0 = 0;
    CData/*0:0*/ __VdlySet__test_copper_lists__DOT__dut__DOT__tile_inst__DOT__col_buffer__v0;
    __VdlySet__test_copper_lists__DOT__dut__DOT__tile_inst__DOT__col_buffer__v0 = 0;
    CData/*7:0*/ __VdlyVal__test_copper_lists__DOT__dut__DOT__tile_inst__DOT__col_buffer__v1;
    __VdlyVal__test_copper_lists__DOT__dut__DOT__tile_inst__DOT__col_buffer__v1 = 0;
    CData/*0:0*/ __VdlySet__test_copper_lists__DOT__dut__DOT__tile_inst__DOT__col_buffer__v1;
    __VdlySet__test_copper_lists__DOT__dut__DOT__tile_inst__DOT__col_buffer__v1 = 0;
    CData/*7:0*/ __VdlyVal__test_copper_lists__DOT__dut__DOT__tile_inst__DOT__col_buffer__v2;
    __VdlyVal__test_copper_lists__DOT__dut__DOT__tile_inst__DOT__col_buffer__v2 = 0;
    CData/*0:0*/ __VdlySet__test_copper_lists__DOT__dut__DOT__tile_inst__DOT__col_buffer__v2;
    __VdlySet__test_copper_lists__DOT__dut__DOT__tile_inst__DOT__col_buffer__v2 = 0;
    CData/*7:0*/ __VdlyVal__test_copper_lists__DOT__dut__DOT__tile_inst__DOT__col_buffer__v3;
    __VdlyVal__test_copper_lists__DOT__dut__DOT__tile_inst__DOT__col_buffer__v3 = 0;
    CData/*0:0*/ __VdlySet__test_copper_lists__DOT__dut__DOT__tile_inst__DOT__col_buffer__v3;
    __VdlySet__test_copper_lists__DOT__dut__DOT__tile_inst__DOT__col_buffer__v3 = 0;
    CData/*7:0*/ __VdlyVal__test_copper_lists__DOT__dut__DOT__tile_inst__DOT__col_buffer__v4;
    __VdlyVal__test_copper_lists__DOT__dut__DOT__tile_inst__DOT__col_buffer__v4 = 0;
    CData/*0:0*/ __VdlySet__test_copper_lists__DOT__dut__DOT__tile_inst__DOT__col_buffer__v4;
    __VdlySet__test_copper_lists__DOT__dut__DOT__tile_inst__DOT__col_buffer__v4 = 0;
    CData/*7:0*/ __VdlyVal__test_copper_lists__DOT__dut__DOT__tile_inst__DOT__col_buffer__v5;
    __VdlyVal__test_copper_lists__DOT__dut__DOT__tile_inst__DOT__col_buffer__v5 = 0;
    CData/*0:0*/ __VdlySet__test_copper_lists__DOT__dut__DOT__tile_inst__DOT__col_buffer__v5;
    __VdlySet__test_copper_lists__DOT__dut__DOT__tile_inst__DOT__col_buffer__v5 = 0;
    CData/*7:0*/ __VdlyVal__test_copper_lists__DOT__dut__DOT__tile_inst__DOT__col_buffer__v6;
    __VdlyVal__test_copper_lists__DOT__dut__DOT__tile_inst__DOT__col_buffer__v6 = 0;
    CData/*0:0*/ __VdlySet__test_copper_lists__DOT__dut__DOT__tile_inst__DOT__col_buffer__v6;
    __VdlySet__test_copper_lists__DOT__dut__DOT__tile_inst__DOT__col_buffer__v6 = 0;
    CData/*7:0*/ __VdlyVal__test_copper_lists__DOT__dut__DOT__tile_inst__DOT__col_buffer__v7;
    __VdlyVal__test_copper_lists__DOT__dut__DOT__tile_inst__DOT__col_buffer__v7 = 0;
    CData/*0:0*/ __VdlySet__test_copper_lists__DOT__dut__DOT__tile_inst__DOT__col_buffer__v7;
    __VdlySet__test_copper_lists__DOT__dut__DOT__tile_inst__DOT__col_buffer__v7 = 0;
    CData/*7:0*/ __VdlyVal__test_copper_lists__DOT__dut__DOT__tile_inst__DOT__col_buffer__v8;
    __VdlyVal__test_copper_lists__DOT__dut__DOT__tile_inst__DOT__col_buffer__v8 = 0;
    CData/*0:0*/ __VdlySet__test_copper_lists__DOT__dut__DOT__tile_inst__DOT__col_buffer__v8;
    __VdlySet__test_copper_lists__DOT__dut__DOT__tile_inst__DOT__col_buffer__v8 = 0;
    CData/*7:0*/ __VdlyVal__test_copper_lists__DOT__dut__DOT__tile_inst__DOT__col_buffer__v9;
    __VdlyVal__test_copper_lists__DOT__dut__DOT__tile_inst__DOT__col_buffer__v9 = 0;
    CData/*0:0*/ __VdlySet__test_copper_lists__DOT__dut__DOT__tile_inst__DOT__col_buffer__v9;
    __VdlySet__test_copper_lists__DOT__dut__DOT__tile_inst__DOT__col_buffer__v9 = 0;
    CData/*7:0*/ __VdlyVal__test_copper_lists__DOT__dut__DOT__tile_inst__DOT__col_buffer__v10;
    __VdlyVal__test_copper_lists__DOT__dut__DOT__tile_inst__DOT__col_buffer__v10 = 0;
    CData/*0:0*/ __VdlySet__test_copper_lists__DOT__dut__DOT__tile_inst__DOT__col_buffer__v10;
    __VdlySet__test_copper_lists__DOT__dut__DOT__tile_inst__DOT__col_buffer__v10 = 0;
    CData/*7:0*/ __VdlyVal__test_copper_lists__DOT__dut__DOT__tile_inst__DOT__col_buffer__v11;
    __VdlyVal__test_copper_lists__DOT__dut__DOT__tile_inst__DOT__col_buffer__v11 = 0;
    CData/*0:0*/ __VdlySet__test_copper_lists__DOT__dut__DOT__tile_inst__DOT__col_buffer__v11;
    __VdlySet__test_copper_lists__DOT__dut__DOT__tile_inst__DOT__col_buffer__v11 = 0;
    CData/*7:0*/ __VdlyVal__test_copper_lists__DOT__dut__DOT__tile_inst__DOT__col_buffer__v12;
    __VdlyVal__test_copper_lists__DOT__dut__DOT__tile_inst__DOT__col_buffer__v12 = 0;
    CData/*0:0*/ __VdlySet__test_copper_lists__DOT__dut__DOT__tile_inst__DOT__col_buffer__v12;
    __VdlySet__test_copper_lists__DOT__dut__DOT__tile_inst__DOT__col_buffer__v12 = 0;
    CData/*7:0*/ __VdlyVal__test_copper_lists__DOT__dut__DOT__tile_inst__DOT__col_buffer__v13;
    __VdlyVal__test_copper_lists__DOT__dut__DOT__tile_inst__DOT__col_buffer__v13 = 0;
    CData/*0:0*/ __VdlySet__test_copper_lists__DOT__dut__DOT__tile_inst__DOT__col_buffer__v13;
    __VdlySet__test_copper_lists__DOT__dut__DOT__tile_inst__DOT__col_buffer__v13 = 0;
    CData/*7:0*/ __VdlyVal__test_copper_lists__DOT__dut__DOT__tile_inst__DOT__col_buffer__v14;
    __VdlyVal__test_copper_lists__DOT__dut__DOT__tile_inst__DOT__col_buffer__v14 = 0;
    CData/*0:0*/ __VdlySet__test_copper_lists__DOT__dut__DOT__tile_inst__DOT__col_buffer__v14;
    __VdlySet__test_copper_lists__DOT__dut__DOT__tile_inst__DOT__col_buffer__v14 = 0;
    CData/*7:0*/ __VdlyVal__test_copper_lists__DOT__dut__DOT__tile_inst__DOT__col_buffer__v15;
    __VdlyVal__test_copper_lists__DOT__dut__DOT__tile_inst__DOT__col_buffer__v15 = 0;
    CData/*0:0*/ __VdlySet__test_copper_lists__DOT__dut__DOT__tile_inst__DOT__col_buffer__v15;
    __VdlySet__test_copper_lists__DOT__dut__DOT__tile_inst__DOT__col_buffer__v15 = 0;
    CData/*7:0*/ __VdlyVal__test_copper_lists__DOT__dut__DOT__tile_inst__DOT__col_buffer__v16;
    __VdlyVal__test_copper_lists__DOT__dut__DOT__tile_inst__DOT__col_buffer__v16 = 0;
    CData/*0:0*/ __VdlySet__test_copper_lists__DOT__dut__DOT__tile_inst__DOT__col_buffer__v16;
    __VdlySet__test_copper_lists__DOT__dut__DOT__tile_inst__DOT__col_buffer__v16 = 0;
    CData/*7:0*/ __VdlyVal__test_copper_lists__DOT__dut__DOT__tile_inst__DOT__col_buffer__v17;
    __VdlyVal__test_copper_lists__DOT__dut__DOT__tile_inst__DOT__col_buffer__v17 = 0;
    CData/*0:0*/ __VdlySet__test_copper_lists__DOT__dut__DOT__tile_inst__DOT__col_buffer__v17;
    __VdlySet__test_copper_lists__DOT__dut__DOT__tile_inst__DOT__col_buffer__v17 = 0;
    CData/*7:0*/ __VdlyVal__test_copper_lists__DOT__dut__DOT__tile_inst__DOT__col_buffer__v18;
    __VdlyVal__test_copper_lists__DOT__dut__DOT__tile_inst__DOT__col_buffer__v18 = 0;
    CData/*0:0*/ __VdlySet__test_copper_lists__DOT__dut__DOT__tile_inst__DOT__col_buffer__v18;
    __VdlySet__test_copper_lists__DOT__dut__DOT__tile_inst__DOT__col_buffer__v18 = 0;
    CData/*7:0*/ __VdlyVal__test_copper_lists__DOT__dut__DOT__tile_inst__DOT__col_buffer__v19;
    __VdlyVal__test_copper_lists__DOT__dut__DOT__tile_inst__DOT__col_buffer__v19 = 0;
    CData/*0:0*/ __VdlySet__test_copper_lists__DOT__dut__DOT__tile_inst__DOT__col_buffer__v19;
    __VdlySet__test_copper_lists__DOT__dut__DOT__tile_inst__DOT__col_buffer__v19 = 0;
    CData/*7:0*/ __VdlyVal__test_copper_lists__DOT__dut__DOT__tile_inst__DOT__col_buffer__v20;
    __VdlyVal__test_copper_lists__DOT__dut__DOT__tile_inst__DOT__col_buffer__v20 = 0;
    CData/*0:0*/ __VdlySet__test_copper_lists__DOT__dut__DOT__tile_inst__DOT__col_buffer__v20;
    __VdlySet__test_copper_lists__DOT__dut__DOT__tile_inst__DOT__col_buffer__v20 = 0;
    CData/*7:0*/ __VdlyVal__test_copper_lists__DOT__dut__DOT__tile_inst__DOT__col_buffer__v21;
    __VdlyVal__test_copper_lists__DOT__dut__DOT__tile_inst__DOT__col_buffer__v21 = 0;
    CData/*0:0*/ __VdlySet__test_copper_lists__DOT__dut__DOT__tile_inst__DOT__col_buffer__v21;
    __VdlySet__test_copper_lists__DOT__dut__DOT__tile_inst__DOT__col_buffer__v21 = 0;
    CData/*7:0*/ __VdlyVal__test_copper_lists__DOT__dut__DOT__tile_inst__DOT__col_buffer__v22;
    __VdlyVal__test_copper_lists__DOT__dut__DOT__tile_inst__DOT__col_buffer__v22 = 0;
    CData/*0:0*/ __VdlySet__test_copper_lists__DOT__dut__DOT__tile_inst__DOT__col_buffer__v22;
    __VdlySet__test_copper_lists__DOT__dut__DOT__tile_inst__DOT__col_buffer__v22 = 0;
    CData/*7:0*/ __VdlyVal__test_copper_lists__DOT__dut__DOT__tile_inst__DOT__col_buffer__v23;
    __VdlyVal__test_copper_lists__DOT__dut__DOT__tile_inst__DOT__col_buffer__v23 = 0;
    CData/*0:0*/ __VdlySet__test_copper_lists__DOT__dut__DOT__tile_inst__DOT__col_buffer__v23;
    __VdlySet__test_copper_lists__DOT__dut__DOT__tile_inst__DOT__col_buffer__v23 = 0;
    CData/*7:0*/ __VdlyVal__test_copper_lists__DOT__dut__DOT__tile_inst__DOT__col_buffer__v24;
    __VdlyVal__test_copper_lists__DOT__dut__DOT__tile_inst__DOT__col_buffer__v24 = 0;
    CData/*0:0*/ __VdlySet__test_copper_lists__DOT__dut__DOT__tile_inst__DOT__col_buffer__v24;
    __VdlySet__test_copper_lists__DOT__dut__DOT__tile_inst__DOT__col_buffer__v24 = 0;
    CData/*7:0*/ __VdlyVal__test_copper_lists__DOT__dut__DOT__tile_inst__DOT__col_buffer__v25;
    __VdlyVal__test_copper_lists__DOT__dut__DOT__tile_inst__DOT__col_buffer__v25 = 0;
    CData/*4:0*/ __VdlyDim0__test_copper_lists__DOT__dut__DOT__tile_inst__DOT__col_buffer__v25;
    __VdlyDim0__test_copper_lists__DOT__dut__DOT__tile_inst__DOT__col_buffer__v25 = 0;
    CData/*0:0*/ __VdlySet__test_copper_lists__DOT__dut__DOT__tile_inst__DOT__col_buffer__v25;
    __VdlySet__test_copper_lists__DOT__dut__DOT__tile_inst__DOT__col_buffer__v25 = 0;
    CData/*7:0*/ __VdlyVal__test_copper_lists__DOT__dut__DOT__tile_inst__DOT__col_buffer__v26;
    __VdlyVal__test_copper_lists__DOT__dut__DOT__tile_inst__DOT__col_buffer__v26 = 0;
    CData/*0:0*/ __VdlySet__test_copper_lists__DOT__dut__DOT__tile_inst__DOT__col_buffer__v26;
    __VdlySet__test_copper_lists__DOT__dut__DOT__tile_inst__DOT__col_buffer__v26 = 0;
    CData/*7:0*/ __VdlyVal__test_copper_lists__DOT__dut__DOT__tile_inst__DOT__col_buffer__v27;
    __VdlyVal__test_copper_lists__DOT__dut__DOT__tile_inst__DOT__col_buffer__v27 = 0;
    CData/*0:0*/ __VdlySet__test_copper_lists__DOT__dut__DOT__tile_inst__DOT__col_buffer__v27;
    __VdlySet__test_copper_lists__DOT__dut__DOT__tile_inst__DOT__col_buffer__v27 = 0;
    CData/*7:0*/ __VdlyVal__test_copper_lists__DOT__dut__DOT__tile_inst__DOT__col_buffer__v28;
    __VdlyVal__test_copper_lists__DOT__dut__DOT__tile_inst__DOT__col_buffer__v28 = 0;
    CData/*0:0*/ __VdlySet__test_copper_lists__DOT__dut__DOT__tile_inst__DOT__col_buffer__v28;
    __VdlySet__test_copper_lists__DOT__dut__DOT__tile_inst__DOT__col_buffer__v28 = 0;
    CData/*7:0*/ __VdlyVal__test_copper_lists__DOT__dut__DOT__tile_inst__DOT__col_buffer__v29;
    __VdlyVal__test_copper_lists__DOT__dut__DOT__tile_inst__DOT__col_buffer__v29 = 0;
    CData/*0:0*/ __VdlySet__test_copper_lists__DOT__dut__DOT__tile_inst__DOT__col_buffer__v29;
    __VdlySet__test_copper_lists__DOT__dut__DOT__tile_inst__DOT__col_buffer__v29 = 0;
    CData/*7:0*/ __VdlyVal__test_copper_lists__DOT__dut__DOT__tile_inst__DOT__col_buffer__v30;
    __VdlyVal__test_copper_lists__DOT__dut__DOT__tile_inst__DOT__col_buffer__v30 = 0;
    CData/*0:0*/ __VdlySet__test_copper_lists__DOT__dut__DOT__tile_inst__DOT__col_buffer__v30;
    __VdlySet__test_copper_lists__DOT__dut__DOT__tile_inst__DOT__col_buffer__v30 = 0;
    CData/*7:0*/ __VdlyVal__test_copper_lists__DOT__dut__DOT__tile_inst__DOT__col_buffer__v31;
    __VdlyVal__test_copper_lists__DOT__dut__DOT__tile_inst__DOT__col_buffer__v31 = 0;
    CData/*0:0*/ __VdlySet__test_copper_lists__DOT__dut__DOT__tile_inst__DOT__col_buffer__v31;
    __VdlySet__test_copper_lists__DOT__dut__DOT__tile_inst__DOT__col_buffer__v31 = 0;
    CData/*7:0*/ __VdlyVal__test_copper_lists__DOT__dut__DOT__tile_inst__DOT__col_buffer__v32;
    __VdlyVal__test_copper_lists__DOT__dut__DOT__tile_inst__DOT__col_buffer__v32 = 0;
    CData/*0:0*/ __VdlySet__test_copper_lists__DOT__dut__DOT__tile_inst__DOT__col_buffer__v32;
    __VdlySet__test_copper_lists__DOT__dut__DOT__tile_inst__DOT__col_buffer__v32 = 0;
    CData/*7:0*/ __VdlyVal__test_copper_lists__DOT__dut__DOT__tile_inst__DOT__col_buffer__v33;
    __VdlyVal__test_copper_lists__DOT__dut__DOT__tile_inst__DOT__col_buffer__v33 = 0;
    CData/*0:0*/ __VdlySet__test_copper_lists__DOT__dut__DOT__tile_inst__DOT__col_buffer__v33;
    __VdlySet__test_copper_lists__DOT__dut__DOT__tile_inst__DOT__col_buffer__v33 = 0;
    CData/*7:0*/ __VdlyVal__test_copper_lists__DOT__dut__DOT__tile_inst__DOT__col_buffer__v34;
    __VdlyVal__test_copper_lists__DOT__dut__DOT__tile_inst__DOT__col_buffer__v34 = 0;
    CData/*0:0*/ __VdlySet__test_copper_lists__DOT__dut__DOT__tile_inst__DOT__col_buffer__v34;
    __VdlySet__test_copper_lists__DOT__dut__DOT__tile_inst__DOT__col_buffer__v34 = 0;
    CData/*7:0*/ __VdlyVal__test_copper_lists__DOT__dut__DOT__tile_inst__DOT__col_buffer__v35;
    __VdlyVal__test_copper_lists__DOT__dut__DOT__tile_inst__DOT__col_buffer__v35 = 0;
    CData/*0:0*/ __VdlySet__test_copper_lists__DOT__dut__DOT__tile_inst__DOT__col_buffer__v35;
    __VdlySet__test_copper_lists__DOT__dut__DOT__tile_inst__DOT__col_buffer__v35 = 0;
    CData/*7:0*/ __VdlyVal__test_copper_lists__DOT__dut__DOT__tile_inst__DOT__col_buffer__v36;
    __VdlyVal__test_copper_lists__DOT__dut__DOT__tile_inst__DOT__col_buffer__v36 = 0;
    CData/*0:0*/ __VdlySet__test_copper_lists__DOT__dut__DOT__tile_inst__DOT__col_buffer__v36;
    __VdlySet__test_copper_lists__DOT__dut__DOT__tile_inst__DOT__col_buffer__v36 = 0;
    CData/*7:0*/ __VdlyVal__test_copper_lists__DOT__dut__DOT__tile_inst__DOT__col_buffer__v37;
    __VdlyVal__test_copper_lists__DOT__dut__DOT__tile_inst__DOT__col_buffer__v37 = 0;
    CData/*0:0*/ __VdlySet__test_copper_lists__DOT__dut__DOT__tile_inst__DOT__col_buffer__v37;
    __VdlySet__test_copper_lists__DOT__dut__DOT__tile_inst__DOT__col_buffer__v37 = 0;
    CData/*7:0*/ __VdlyVal__test_copper_lists__DOT__dut__DOT__tile_inst__DOT__col_buffer__v38;
    __VdlyVal__test_copper_lists__DOT__dut__DOT__tile_inst__DOT__col_buffer__v38 = 0;
    CData/*0:0*/ __VdlySet__test_copper_lists__DOT__dut__DOT__tile_inst__DOT__col_buffer__v38;
    __VdlySet__test_copper_lists__DOT__dut__DOT__tile_inst__DOT__col_buffer__v38 = 0;
    CData/*7:0*/ __VdlyVal__test_copper_lists__DOT__dut__DOT__tile_inst__DOT__col_buffer__v39;
    __VdlyVal__test_copper_lists__DOT__dut__DOT__tile_inst__DOT__col_buffer__v39 = 0;
    CData/*0:0*/ __VdlySet__test_copper_lists__DOT__dut__DOT__tile_inst__DOT__col_buffer__v39;
    __VdlySet__test_copper_lists__DOT__dut__DOT__tile_inst__DOT__col_buffer__v39 = 0;
    CData/*7:0*/ __VdlyVal__test_copper_lists__DOT__dut__DOT__tile_inst__DOT__col_buffer__v40;
    __VdlyVal__test_copper_lists__DOT__dut__DOT__tile_inst__DOT__col_buffer__v40 = 0;
    CData/*0:0*/ __VdlySet__test_copper_lists__DOT__dut__DOT__tile_inst__DOT__col_buffer__v40;
    __VdlySet__test_copper_lists__DOT__dut__DOT__tile_inst__DOT__col_buffer__v40 = 0;
    CData/*7:0*/ __VdlyVal__test_copper_lists__DOT__dut__DOT__tile_inst__DOT__col_buffer__v41;
    __VdlyVal__test_copper_lists__DOT__dut__DOT__tile_inst__DOT__col_buffer__v41 = 0;
    CData/*0:0*/ __VdlySet__test_copper_lists__DOT__dut__DOT__tile_inst__DOT__col_buffer__v41;
    __VdlySet__test_copper_lists__DOT__dut__DOT__tile_inst__DOT__col_buffer__v41 = 0;
    CData/*7:0*/ __VdlyVal__test_copper_lists__DOT__dut__DOT__tile_inst__DOT__col_buffer__v42;
    __VdlyVal__test_copper_lists__DOT__dut__DOT__tile_inst__DOT__col_buffer__v42 = 0;
    CData/*0:0*/ __VdlySet__test_copper_lists__DOT__dut__DOT__tile_inst__DOT__col_buffer__v42;
    __VdlySet__test_copper_lists__DOT__dut__DOT__tile_inst__DOT__col_buffer__v42 = 0;
    CData/*7:0*/ __VdlyVal__test_copper_lists__DOT__dut__DOT__tile_inst__DOT__col_buffer__v43;
    __VdlyVal__test_copper_lists__DOT__dut__DOT__tile_inst__DOT__col_buffer__v43 = 0;
    CData/*0:0*/ __VdlySet__test_copper_lists__DOT__dut__DOT__tile_inst__DOT__col_buffer__v43;
    __VdlySet__test_copper_lists__DOT__dut__DOT__tile_inst__DOT__col_buffer__v43 = 0;
    CData/*7:0*/ __VdlyVal__test_copper_lists__DOT__dut__DOT__tile_inst__DOT__col_buffer__v44;
    __VdlyVal__test_copper_lists__DOT__dut__DOT__tile_inst__DOT__col_buffer__v44 = 0;
    CData/*0:0*/ __VdlySet__test_copper_lists__DOT__dut__DOT__tile_inst__DOT__col_buffer__v44;
    __VdlySet__test_copper_lists__DOT__dut__DOT__tile_inst__DOT__col_buffer__v44 = 0;
    CData/*7:0*/ __VdlyVal__test_copper_lists__DOT__dut__DOT__tile_inst__DOT__col_buffer__v45;
    __VdlyVal__test_copper_lists__DOT__dut__DOT__tile_inst__DOT__col_buffer__v45 = 0;
    CData/*0:0*/ __VdlySet__test_copper_lists__DOT__dut__DOT__tile_inst__DOT__col_buffer__v45;
    __VdlySet__test_copper_lists__DOT__dut__DOT__tile_inst__DOT__col_buffer__v45 = 0;
    CData/*7:0*/ __VdlyVal__test_copper_lists__DOT__dut__DOT__tile_inst__DOT__col_buffer__v46;
    __VdlyVal__test_copper_lists__DOT__dut__DOT__tile_inst__DOT__col_buffer__v46 = 0;
    CData/*0:0*/ __VdlySet__test_copper_lists__DOT__dut__DOT__tile_inst__DOT__col_buffer__v46;
    __VdlySet__test_copper_lists__DOT__dut__DOT__tile_inst__DOT__col_buffer__v46 = 0;
    CData/*7:0*/ __VdlyVal__test_copper_lists__DOT__dut__DOT__tile_inst__DOT__col_buffer__v47;
    __VdlyVal__test_copper_lists__DOT__dut__DOT__tile_inst__DOT__col_buffer__v47 = 0;
    CData/*0:0*/ __VdlySet__test_copper_lists__DOT__dut__DOT__tile_inst__DOT__col_buffer__v47;
    __VdlySet__test_copper_lists__DOT__dut__DOT__tile_inst__DOT__col_buffer__v47 = 0;
    CData/*7:0*/ __VdlyVal__test_copper_lists__DOT__dut__DOT__tile_inst__DOT__col_buffer__v48;
    __VdlyVal__test_copper_lists__DOT__dut__DOT__tile_inst__DOT__col_buffer__v48 = 0;
    CData/*0:0*/ __VdlySet__test_copper_lists__DOT__dut__DOT__tile_inst__DOT__col_buffer__v48;
    __VdlySet__test_copper_lists__DOT__dut__DOT__tile_inst__DOT__col_buffer__v48 = 0;
    CData/*7:0*/ __VdlyVal__test_copper_lists__DOT__dut__DOT__tile_inst__DOT__col_buffer__v49;
    __VdlyVal__test_copper_lists__DOT__dut__DOT__tile_inst__DOT__col_buffer__v49 = 0;
    CData/*0:0*/ __VdlySet__test_copper_lists__DOT__dut__DOT__tile_inst__DOT__col_buffer__v49;
    __VdlySet__test_copper_lists__DOT__dut__DOT__tile_inst__DOT__col_buffer__v49 = 0;
    CData/*7:0*/ __VdlyVal__test_copper_lists__DOT__dut__DOT__tile_inst__DOT__col_buffer__v50;
    __VdlyVal__test_copper_lists__DOT__dut__DOT__tile_inst__DOT__col_buffer__v50 = 0;
    CData/*0:0*/ __VdlySet__test_copper_lists__DOT__dut__DOT__tile_inst__DOT__col_buffer__v50;
    __VdlySet__test_copper_lists__DOT__dut__DOT__tile_inst__DOT__col_buffer__v50 = 0;
    CData/*7:0*/ __VdlyVal__test_copper_lists__DOT__dut__DOT__tile_inst__DOT__tregs__v0;
    __VdlyVal__test_copper_lists__DOT__dut__DOT__tile_inst__DOT__tregs__v0 = 0;
    CData/*4:0*/ __VdlyDim0__test_copper_lists__DOT__dut__DOT__tile_inst__DOT__tregs__v0;
    __VdlyDim0__test_copper_lists__DOT__dut__DOT__tile_inst__DOT__tregs__v0 = 0;
    CData/*0:0*/ __VdlySet__test_copper_lists__DOT__dut__DOT__tile_inst__DOT__tregs__v0;
    __VdlySet__test_copper_lists__DOT__dut__DOT__tile_inst__DOT__tregs__v0 = 0;
    CData/*7:0*/ __VdlyVal__test_copper_lists__DOT__dut__DOT__tile_inst__DOT__tile_data_ram__DOT__mem__v0;
    __VdlyVal__test_copper_lists__DOT__dut__DOT__tile_inst__DOT__tile_data_ram__DOT__mem__v0 = 0;
    SData/*14:0*/ __VdlyDim0__test_copper_lists__DOT__dut__DOT__tile_inst__DOT__tile_data_ram__DOT__mem__v0;
    __VdlyDim0__test_copper_lists__DOT__dut__DOT__tile_inst__DOT__tile_data_ram__DOT__mem__v0 = 0;
    CData/*0:0*/ __VdlySet__test_copper_lists__DOT__dut__DOT__tile_inst__DOT__tile_data_ram__DOT__mem__v0;
    __VdlySet__test_copper_lists__DOT__dut__DOT__tile_inst__DOT__tile_data_ram__DOT__mem__v0 = 0;
    CData/*7:0*/ __VdlyVal__test_copper_lists__DOT__dut__DOT__tile_inst__DOT__nametable_ram__DOT__mem__v0;
    __VdlyVal__test_copper_lists__DOT__dut__DOT__tile_inst__DOT__nametable_ram__DOT__mem__v0 = 0;
    SData/*11:0*/ __VdlyDim0__test_copper_lists__DOT__dut__DOT__tile_inst__DOT__nametable_ram__DOT__mem__v0;
    __VdlyDim0__test_copper_lists__DOT__dut__DOT__tile_inst__DOT__nametable_ram__DOT__mem__v0 = 0;
    CData/*0:0*/ __VdlySet__test_copper_lists__DOT__dut__DOT__tile_inst__DOT__nametable_ram__DOT__mem__v0;
    __VdlySet__test_copper_lists__DOT__dut__DOT__tile_inst__DOT__nametable_ram__DOT__mem__v0 = 0;
    CData/*7:0*/ __VdlyVal__test_copper_lists__DOT__dut__DOT__tile_inst__DOT__attr_table_ram__DOT__mem__v0;
    __VdlyVal__test_copper_lists__DOT__dut__DOT__tile_inst__DOT__attr_table_ram__DOT__mem__v0 = 0;
    SData/*11:0*/ __VdlyDim0__test_copper_lists__DOT__dut__DOT__tile_inst__DOT__attr_table_ram__DOT__mem__v0;
    __VdlyDim0__test_copper_lists__DOT__dut__DOT__tile_inst__DOT__attr_table_ram__DOT__mem__v0 = 0;
    CData/*0:0*/ __VdlySet__test_copper_lists__DOT__dut__DOT__tile_inst__DOT__attr_table_ram__DOT__mem__v0;
    __VdlySet__test_copper_lists__DOT__dut__DOT__tile_inst__DOT__attr_table_ram__DOT__mem__v0 = 0;
    // Body
    __VdlySet__test_copper_lists__DOT__dut__DOT__tile_inst__DOT__tile_data_ram__DOT__mem__v0 = 0U;
    __VdlySet__test_copper_lists__DOT__dut__DOT__gfx_inst__DOT__gfx_mem__DOT__mem__v0 = 0U;
    __VdlySet__test_copper_lists__DOT__dut__DOT__tile_inst__DOT__attr_table_ram__DOT__mem__v0 = 0U;
    __VdlySet__test_copper_lists__DOT__dut__DOT__tile_inst__DOT__nametable_ram__DOT__mem__v0 = 0U;
    __VdlySet__test_copper_lists__DOT__dut__DOT__tile_inst__DOT__tregs__v0 = 0U;
    __VdlySet__test_copper_lists__DOT__dut__DOT__fio_name__v0 = 0U;
    __Vdly__test_copper_lists__DOT__dut__DOT__h_count 
        = vlSelfRef.test_copper_lists__DOT__dut__DOT__h_count;
    __Vdly__test_copper_lists__DOT__dut__DOT__v_count 
        = vlSelfRef.test_copper_lists__DOT__dut__DOT__v_count;
    __Vdly__test_copper_lists__DOT__dut__DOT__copper_index 
        = vlSelfRef.test_copper_lists__DOT__dut__DOT__copper_index;
    __Vdly__test_copper_lists__DOT__dut__DOT__cursor_x 
        = vlSelfRef.test_copper_lists__DOT__dut__DOT__cursor_x;
    __Vdly__test_copper_lists__DOT__dut__DOT__cursor_y 
        = vlSelfRef.test_copper_lists__DOT__dut__DOT__cursor_y;
    __Vdly__test_copper_lists__DOT__dut__DOT__fg_color 
        = vlSelfRef.test_copper_lists__DOT__dut__DOT__fg_color;
    __Vdly__test_copper_lists__DOT__dut__DOT__scroll_pending 
        = vlSelfRef.test_copper_lists__DOT__dut__DOT__scroll_pending;
    __Vdly__test_copper_lists__DOT__dut__DOT__scroll_col 
        = vlSelfRef.test_copper_lists__DOT__dut__DOT__scroll_col;
    __Vdly__test_copper_lists__DOT__dut__DOT__cmd_busy 
        = vlSelfRef.test_copper_lists__DOT__dut__DOT__cmd_busy;
    __Vdly__test_copper_lists__DOT__dut__DOT__copper_enabled 
        = vlSelfRef.test_copper_lists__DOT__dut__DOT__copper_enabled;
    __Vdly__test_copper_lists__DOT__dut__DOT__copper_count 
        = vlSelfRef.test_copper_lists__DOT__dut__DOT__copper_count;
    __Vdly__test_copper_lists__DOT__dut__DOT__copper_target_list 
        = vlSelfRef.test_copper_lists__DOT__dut__DOT__copper_target_list;
    __Vdly__test_copper_lists__DOT__dut__DOT__copper_pending_list 
        = vlSelfRef.test_copper_lists__DOT__dut__DOT__copper_pending_list;
    __Vdly__test_copper_lists__DOT__dut__DOT__copper_loading 
        = vlSelfRef.test_copper_lists__DOT__dut__DOT__copper_loading;
    __Vdly__test_copper_lists__DOT__dut__DOT__sprrow_count 
        = vlSelfRef.test_copper_lists__DOT__dut__DOT__sprrow_count;
    __Vdly__test_copper_lists__DOT__dut__DOT__sprcopy_phase 
        = vlSelfRef.test_copper_lists__DOT__dut__DOT__sprcopy_phase;
    __Vdly__test_copper_lists__DOT__dut__DOT__memread_pending 
        = vlSelfRef.test_copper_lists__DOT__dut__DOT__memread_pending;
    __Vdly__test_copper_lists__DOT__dut__DOT__cmd_cx 
        = vlSelfRef.test_copper_lists__DOT__dut__DOT__cmd_cx;
    __Vdly__test_copper_lists__DOT__dut__DOT__cmd_cy 
        = vlSelfRef.test_copper_lists__DOT__dut__DOT__cmd_cy;
    __Vdly__test_copper_lists__DOT__dut__DOT__copper_active_list 
        = vlSelfRef.test_copper_lists__DOT__dut__DOT__copper_active_list;
    __Vdly__test_copper_lists__DOT__dut__DOT__cmd_op 
        = vlSelfRef.test_copper_lists__DOT__dut__DOT__cmd_op;
    __VdlySet__test_copper_lists__DOT__dut__DOT__spr_y__v17 = 0U;
    __VdlySet__test_copper_lists__DOT__dut__DOT__spr_pri__v16 = 0U;
    __VdlySet__test_copper_lists__DOT__dut__DOT__spr_pri__v17 = 0U;
    __VdlySet__test_copper_lists__DOT__dut__DOT__spr_enable__v16 = 0U;
    __VdlySet__test_copper_lists__DOT__dut__DOT__spr_enable__v17 = 0U;
    __VdlySet__test_copper_lists__DOT__dut__DOT__sprrow_data__v0 = 0U;
    __VdlySet__test_copper_lists__DOT__dut__DOT__spr_x__v17 = 0U;
    __VdlySet__test_copper_lists__DOT__dut__DOT__spr_x__v18 = 0U;
    __Vdly__test_copper_lists__DOT__dut__DOT__scroll_offset 
        = vlSelfRef.test_copper_lists__DOT__dut__DOT__scroll_offset;
    __VdlySet__test_copper_lists__DOT__dut__DOT__copper_list_pos__v0 = 0U;
    __VdlySet__test_copper_lists__DOT__dut__DOT__spr_flip_h__v16 = 0U;
    __VdlySet__test_copper_lists__DOT__dut__DOT__regs__v32 = 0U;
    __VdlySet__test_copper_lists__DOT__dut__DOT__regs__v33 = 0U;
    __VdlySet__test_copper_lists__DOT__dut__DOT__regs__v34 = 0U;
    __VdlySet__test_copper_lists__DOT__dut__DOT__regs__v35 = 0U;
    __VdlySet__test_copper_lists__DOT__dut__DOT__spr_x__v16 = 0U;
    __VdlySet__test_copper_lists__DOT__dut__DOT__spr_flip_h__v17 = 0U;
    __VdlySet__test_copper_lists__DOT__dut__DOT__copper_pos__v0 = 0U;
    __VdlySet__test_copper_lists__DOT__dut__DOT__copper_pos__v1 = 0U;
    __VdlySet__test_copper_lists__DOT__dut__DOT__regs__v0 = 0U;
    __VdlySet__test_copper_lists__DOT__dut__DOT__sprite_inst__DOT__spr_mem__DOT__mem__v0 = 0U;
    __VdlySet__test_copper_lists__DOT__dut__DOT__text_inst__DOT__char_mem__DOT__mem__v0 = 0U;
    __VdlySet__test_copper_lists__DOT__dut__DOT__text_inst__DOT__color_mem__DOT__mem__v0 = 0U;
    __Vdly__test_copper_lists__DOT__dut__DOT__artist_busy 
        = vlSelfRef.test_copper_lists__DOT__dut__DOT__artist_busy;
    __Vdly__test_copper_lists__DOT__dut__DOT__artist_inst__DOT__op 
        = vlSelfRef.test_copper_lists__DOT__dut__DOT__artist_inst__DOT__op;
    __Vdly__test_copper_lists__DOT__dut__DOT__artist_inst__DOT__fill_sp 
        = vlSelfRef.test_copper_lists__DOT__dut__DOT__artist_inst__DOT__fill_sp;
    __Vdly__test_copper_lists__DOT__dut__DOT__artist_inst__DOT__octant_count 
        = vlSelfRef.test_copper_lists__DOT__dut__DOT__artist_inst__DOT__octant_count;
    __Vdly__test_copper_lists__DOT__dut__DOT__artist_inst__DOT__paint_phase 
        = vlSelfRef.test_copper_lists__DOT__dut__DOT__artist_inst__DOT__paint_phase;
    __Vdly__test_copper_lists__DOT__dut__DOT__artist_inst__DOT__gt_font_pending 
        = vlSelfRef.test_copper_lists__DOT__dut__DOT__artist_inst__DOT__gt_font_pending;
    __Vdly__test_copper_lists__DOT__dut__DOT__artist_inst__DOT__paint_init 
        = vlSelfRef.test_copper_lists__DOT__dut__DOT__artist_inst__DOT__paint_init;
    __Vdly__test_copper_lists__DOT__dut__DOT__artist_inst__DOT__gt_font_byte 
        = vlSelfRef.test_copper_lists__DOT__dut__DOT__artist_inst__DOT__gt_font_byte;
    __Vdly__test_copper_lists__DOT__dut__DOT__artist_inst__DOT__err 
        = vlSelfRef.test_copper_lists__DOT__dut__DOT__artist_inst__DOT__err;
    __Vdly__test_copper_lists__DOT__dut__DOT__artist_inst__DOT__x 
        = vlSelfRef.test_copper_lists__DOT__dut__DOT__artist_inst__DOT__x;
    __Vdly__test_copper_lists__DOT__dut__DOT__artist_inst__DOT__y 
        = vlSelfRef.test_copper_lists__DOT__dut__DOT__artist_inst__DOT__y;
    __Vdly__test_copper_lists__DOT__dut__DOT__artist_inst__DOT__cx 
        = vlSelfRef.test_copper_lists__DOT__dut__DOT__artist_inst__DOT__cx;
    __Vdly__test_copper_lists__DOT__dut__DOT__artist_inst__DOT__cy 
        = vlSelfRef.test_copper_lists__DOT__dut__DOT__artist_inst__DOT__cy;
    __Vdly__test_copper_lists__DOT__dut__DOT__artist_inst__DOT__dx 
        = vlSelfRef.test_copper_lists__DOT__dut__DOT__artist_inst__DOT__dx;
    __Vdly__test_copper_lists__DOT__dut__DOT__artist_inst__DOT__dy 
        = vlSelfRef.test_copper_lists__DOT__dut__DOT__artist_inst__DOT__dy;
    __Vdly__test_copper_lists__DOT__dut__DOT__artist_inst__DOT__fill_target 
        = vlSelfRef.test_copper_lists__DOT__dut__DOT__artist_inst__DOT__fill_target;
    __Vdly__test_copper_lists__DOT__dut__DOT__artist_inst__DOT__fs_b_addr 
        = vlSelfRef.test_copper_lists__DOT__dut__DOT__artist_inst__DOT__fs_b_addr;
    __Vdly__test_copper_lists__DOT__dut__DOT__artist_inst__DOT__gt_font_row 
        = vlSelfRef.test_copper_lists__DOT__dut__DOT__artist_inst__DOT__gt_font_row;
    __Vdly__test_copper_lists__DOT__dut__DOT__artist_inst__DOT__gt_char_idx 
        = vlSelfRef.test_copper_lists__DOT__dut__DOT__artist_inst__DOT__gt_char_idx;
    __VdlySet__test_copper_lists__DOT__dut__DOT__artist_inst__DOT__push_neighbors__v0 = 0U;
    __Vdly__test_copper_lists__DOT__dut__DOT__tile_inst__DOT__dma_src 
        = vlSelfRef.test_copper_lists__DOT__dut__DOT__tile_inst__DOT__dma_src;
    __Vdly__test_copper_lists__DOT__dut__DOT__tile_inst__DOT__dma_pal_phase 
        = vlSelfRef.test_copper_lists__DOT__dut__DOT__tile_inst__DOT__dma_pal_phase;
    __Vdly__test_copper_lists__DOT__dut__DOT__tile_inst__DOT__dma_count 
        = vlSelfRef.test_copper_lists__DOT__dut__DOT__tile_inst__DOT__dma_count;
    __Vdly__test_copper_lists__DOT__dut__DOT__tile_inst__DOT__dma_dst 
        = vlSelfRef.test_copper_lists__DOT__dut__DOT__tile_inst__DOT__dma_dst;
    __Vdly__test_copper_lists__DOT__dut__DOT__tile_inst__DOT__dma_col_idx 
        = vlSelfRef.test_copper_lists__DOT__dut__DOT__tile_inst__DOT__dma_col_idx;
    __Vdly__test_copper_lists__DOT__dut__DOT__tile_inst__DOT__dma_state 
        = vlSelfRef.test_copper_lists__DOT__dut__DOT__tile_inst__DOT__dma_state;
    __Vdly__test_copper_lists__DOT__dut__DOT__tile_inst__DOT__dma_data_valid 
        = vlSelfRef.test_copper_lists__DOT__dut__DOT__tile_inst__DOT__dma_data_valid;
    __VdlySet__test_copper_lists__DOT__dut__DOT__tile_inst__DOT__col_buffer__v0 = 0U;
    __VdlySet__test_copper_lists__DOT__dut__DOT__tile_inst__DOT__col_buffer__v1 = 0U;
    __VdlySet__test_copper_lists__DOT__dut__DOT__tile_inst__DOT__col_buffer__v2 = 0U;
    __VdlySet__test_copper_lists__DOT__dut__DOT__tile_inst__DOT__col_buffer__v3 = 0U;
    __VdlySet__test_copper_lists__DOT__dut__DOT__tile_inst__DOT__col_buffer__v4 = 0U;
    __VdlySet__test_copper_lists__DOT__dut__DOT__tile_inst__DOT__col_buffer__v5 = 0U;
    __VdlySet__test_copper_lists__DOT__dut__DOT__tile_inst__DOT__col_buffer__v6 = 0U;
    __VdlySet__test_copper_lists__DOT__dut__DOT__tile_inst__DOT__col_buffer__v7 = 0U;
    __VdlySet__test_copper_lists__DOT__dut__DOT__tile_inst__DOT__col_buffer__v8 = 0U;
    __VdlySet__test_copper_lists__DOT__dut__DOT__tile_inst__DOT__col_buffer__v9 = 0U;
    __VdlySet__test_copper_lists__DOT__dut__DOT__tile_inst__DOT__col_buffer__v10 = 0U;
    __VdlySet__test_copper_lists__DOT__dut__DOT__tile_inst__DOT__col_buffer__v11 = 0U;
    __VdlySet__test_copper_lists__DOT__dut__DOT__tile_inst__DOT__col_buffer__v12 = 0U;
    __VdlySet__test_copper_lists__DOT__dut__DOT__tile_inst__DOT__col_buffer__v13 = 0U;
    __VdlySet__test_copper_lists__DOT__dut__DOT__tile_inst__DOT__col_buffer__v14 = 0U;
    __VdlySet__test_copper_lists__DOT__dut__DOT__tile_inst__DOT__col_buffer__v15 = 0U;
    __VdlySet__test_copper_lists__DOT__dut__DOT__tile_inst__DOT__col_buffer__v16 = 0U;
    __VdlySet__test_copper_lists__DOT__dut__DOT__tile_inst__DOT__col_buffer__v17 = 0U;
    __VdlySet__test_copper_lists__DOT__dut__DOT__tile_inst__DOT__col_buffer__v18 = 0U;
    __VdlySet__test_copper_lists__DOT__dut__DOT__tile_inst__DOT__col_buffer__v19 = 0U;
    __VdlySet__test_copper_lists__DOT__dut__DOT__tile_inst__DOT__col_buffer__v20 = 0U;
    __VdlySet__test_copper_lists__DOT__dut__DOT__tile_inst__DOT__col_buffer__v21 = 0U;
    __VdlySet__test_copper_lists__DOT__dut__DOT__tile_inst__DOT__col_buffer__v22 = 0U;
    __VdlySet__test_copper_lists__DOT__dut__DOT__tile_inst__DOT__col_buffer__v23 = 0U;
    __VdlySet__test_copper_lists__DOT__dut__DOT__tile_inst__DOT__col_buffer__v24 = 0U;
    __VdlySet__test_copper_lists__DOT__dut__DOT__tile_inst__DOT__col_buffer__v25 = 0U;
    __VdlySet__test_copper_lists__DOT__dut__DOT__tile_inst__DOT__col_buffer__v26 = 0U;
    __VdlySet__test_copper_lists__DOT__dut__DOT__tile_inst__DOT__col_buffer__v27 = 0U;
    __VdlySet__test_copper_lists__DOT__dut__DOT__tile_inst__DOT__col_buffer__v28 = 0U;
    __VdlySet__test_copper_lists__DOT__dut__DOT__tile_inst__DOT__col_buffer__v29 = 0U;
    __VdlySet__test_copper_lists__DOT__dut__DOT__tile_inst__DOT__col_buffer__v30 = 0U;
    __VdlySet__test_copper_lists__DOT__dut__DOT__tile_inst__DOT__col_buffer__v31 = 0U;
    __VdlySet__test_copper_lists__DOT__dut__DOT__tile_inst__DOT__col_buffer__v32 = 0U;
    __VdlySet__test_copper_lists__DOT__dut__DOT__tile_inst__DOT__col_buffer__v33 = 0U;
    __VdlySet__test_copper_lists__DOT__dut__DOT__tile_inst__DOT__col_buffer__v34 = 0U;
    __VdlySet__test_copper_lists__DOT__dut__DOT__tile_inst__DOT__col_buffer__v35 = 0U;
    __VdlySet__test_copper_lists__DOT__dut__DOT__tile_inst__DOT__col_buffer__v36 = 0U;
    __VdlySet__test_copper_lists__DOT__dut__DOT__tile_inst__DOT__col_buffer__v37 = 0U;
    __VdlySet__test_copper_lists__DOT__dut__DOT__tile_inst__DOT__col_buffer__v38 = 0U;
    __VdlySet__test_copper_lists__DOT__dut__DOT__tile_inst__DOT__col_buffer__v39 = 0U;
    __VdlySet__test_copper_lists__DOT__dut__DOT__tile_inst__DOT__col_buffer__v40 = 0U;
    __VdlySet__test_copper_lists__DOT__dut__DOT__tile_inst__DOT__col_buffer__v41 = 0U;
    __VdlySet__test_copper_lists__DOT__dut__DOT__tile_inst__DOT__col_buffer__v42 = 0U;
    __VdlySet__test_copper_lists__DOT__dut__DOT__tile_inst__DOT__col_buffer__v43 = 0U;
    __VdlySet__test_copper_lists__DOT__dut__DOT__tile_inst__DOT__col_buffer__v44 = 0U;
    __VdlySet__test_copper_lists__DOT__dut__DOT__tile_inst__DOT__col_buffer__v45 = 0U;
    __VdlySet__test_copper_lists__DOT__dut__DOT__tile_inst__DOT__col_buffer__v46 = 0U;
    __VdlySet__test_copper_lists__DOT__dut__DOT__tile_inst__DOT__col_buffer__v47 = 0U;
    __VdlySet__test_copper_lists__DOT__dut__DOT__tile_inst__DOT__col_buffer__v48 = 0U;
    __VdlySet__test_copper_lists__DOT__dut__DOT__tile_inst__DOT__col_buffer__v49 = 0U;
    __VdlySet__test_copper_lists__DOT__dut__DOT__tile_inst__DOT__col_buffer__v50 = 0U;
    __VdlySet__test_copper_lists__DOT__dut__DOT__artist_inst__DOT__fill_stack_mem__DOT__mem__v0 = 0U;
    __VdfgRegularize_hebeb780c_0_0 = ((IData)(vlSelfRef.test_copper_lists__DOT__cpu_ce) 
                                      & (IData)(vlSelfRef.test_copper_lists__DOT__cpu_re));
    if (vlSelfRef.test_copper_lists__DOT__dut__DOT__tile_inst__DOT__td_we_a) {
        __VdlyVal__test_copper_lists__DOT__dut__DOT__tile_inst__DOT__tile_data_ram__DOT__mem__v0 
            = vlSelfRef.test_copper_lists__DOT__dut__DOT__tile_inst__DOT__td_din_a;
        __VdlyDim0__test_copper_lists__DOT__dut__DOT__tile_inst__DOT__tile_data_ram__DOT__mem__v0 
            = vlSelfRef.test_copper_lists__DOT__dut__DOT__tile_inst__DOT__td_addr_a;
        __VdlySet__test_copper_lists__DOT__dut__DOT__tile_inst__DOT__tile_data_ram__DOT__mem__v0 = 1U;
    }
    if (vlSelfRef.test_copper_lists__DOT__dut__DOT__gfx_a_we) {
        vlSelfRef.test_copper_lists__DOT__dut__DOT__gfx_inst__DOT__gfx_mem__DOT____Vlvbound_h0a4ce44c__0 
            = vlSelfRef.test_copper_lists__DOT__dut__DOT__gfx_a_din;
        if ((0xf9ffU >= (IData)(vlSelfRef.test_copper_lists__DOT__dut__DOT__gfx_a_addr))) {
            __VdlyVal__test_copper_lists__DOT__dut__DOT__gfx_inst__DOT__gfx_mem__DOT__mem__v0 
                = vlSelfRef.test_copper_lists__DOT__dut__DOT__gfx_inst__DOT__gfx_mem__DOT____Vlvbound_h0a4ce44c__0;
            __VdlyDim0__test_copper_lists__DOT__dut__DOT__gfx_inst__DOT__gfx_mem__DOT__mem__v0 
                = vlSelfRef.test_copper_lists__DOT__dut__DOT__gfx_a_addr;
            __VdlySet__test_copper_lists__DOT__dut__DOT__gfx_inst__DOT__gfx_mem__DOT__mem__v0 = 1U;
        }
    }
    if (vlSelfRef.test_copper_lists__DOT__dut__DOT__tile_inst__DOT__at_we_a) {
        __VdlyVal__test_copper_lists__DOT__dut__DOT__tile_inst__DOT__attr_table_ram__DOT__mem__v0 
            = vlSelfRef.test_copper_lists__DOT__dut__DOT__tile_inst__DOT__at_din_a;
        __VdlyDim0__test_copper_lists__DOT__dut__DOT__tile_inst__DOT__attr_table_ram__DOT__mem__v0 
            = vlSelfRef.test_copper_lists__DOT__dut__DOT__tile_inst__DOT__at_addr_a;
        __VdlySet__test_copper_lists__DOT__dut__DOT__tile_inst__DOT__attr_table_ram__DOT__mem__v0 = 1U;
    }
    if (vlSelfRef.test_copper_lists__DOT__dut__DOT__tile_inst__DOT__nt_we_a) {
        __VdlyVal__test_copper_lists__DOT__dut__DOT__tile_inst__DOT__nametable_ram__DOT__mem__v0 
            = vlSelfRef.test_copper_lists__DOT__dut__DOT__tile_inst__DOT__nt_din_a;
        __VdlyDim0__test_copper_lists__DOT__dut__DOT__tile_inst__DOT__nametable_ram__DOT__mem__v0 
            = vlSelfRef.test_copper_lists__DOT__dut__DOT__tile_inst__DOT__nt_addr_a;
        __VdlySet__test_copper_lists__DOT__dut__DOT__tile_inst__DOT__nametable_ram__DOT__mem__v0 = 1U;
    }
    if (vlSelfRef.test_copper_lists__DOT__dut__DOT__artist_inst__DOT__fs_a_we) {
        __VdlyVal__test_copper_lists__DOT__dut__DOT__artist_inst__DOT__fill_stack_mem__DOT__mem__v0 
            = vlSelfRef.test_copper_lists__DOT__dut__DOT__artist_inst__DOT__fs_a_din;
        __VdlyDim0__test_copper_lists__DOT__dut__DOT__artist_inst__DOT__fill_stack_mem__DOT__mem__v0 
            = vlSelfRef.test_copper_lists__DOT__dut__DOT__artist_inst__DOT__fs_a_addr;
        __VdlySet__test_copper_lists__DOT__dut__DOT__artist_inst__DOT__fill_stack_mem__DOT__mem__v0 = 1U;
    }
    vlSelfRef.test_copper_lists__DOT__dut__DOT__tile_inst__DOT__cmd_nt_we = 0U;
    vlSelfRef.test_copper_lists__DOT__dut__DOT__tile_inst__DOT__cmd_at_we = 0U;
    if ((1U & (~ (IData)(vlSelfRef.test_copper_lists__DOT__rst)))) {
        if (((IData)(vlSelfRef.test_copper_lists__DOT__dut__DOT____Vcellinp__tile_inst__cpu_we) 
             & (IData)(vlSelfRef.test_copper_lists__DOT__dut__DOT__tile_reg_sel))) {
            __VdlyVal__test_copper_lists__DOT__dut__DOT__tile_inst__DOT__tregs__v0 
                = vlSelfRef.test_copper_lists__DOT__cpu_wdata;
            __VdlyDim0__test_copper_lists__DOT__dut__DOT__tile_inst__DOT__tregs__v0 
                = (0x0000001fU & (IData)(vlSelfRef.test_copper_lists__DOT__cpu_addr));
            __VdlySet__test_copper_lists__DOT__dut__DOT__tile_inst__DOT__tregs__v0 = 1U;
        }
        if (((IData)(vlSelfRef.test_copper_lists__DOT__cpu_we) 
             & (IData)(vlSelfRef.test_copper_lists__DOT__cpu_ce))) {
            if (((0xb9b0U <= (IData)(vlSelfRef.test_copper_lists__DOT__cpu_addr)) 
                 & (0xb9efU >= (IData)(vlSelfRef.test_copper_lists__DOT__cpu_addr)))) {
                __VdlyVal__test_copper_lists__DOT__dut__DOT__fio_name__v0 
                    = vlSelfRef.test_copper_lists__DOT__cpu_wdata;
                __VdlyDim0__test_copper_lists__DOT__dut__DOT__fio_name__v0 
                    = (0x0000003fU & ((IData)(vlSelfRef.test_copper_lists__DOT__cpu_addr) 
                                      - (IData)(0x30U)));
                __VdlySet__test_copper_lists__DOT__dut__DOT__fio_name__v0 = 1U;
            }
        }
    }
    if (__VdlySet__test_copper_lists__DOT__dut__DOT__fio_name__v0) {
        vlSelfRef.test_copper_lists__DOT__dut__DOT__fio_name[__VdlyDim0__test_copper_lists__DOT__dut__DOT__fio_name__v0] 
            = __VdlyVal__test_copper_lists__DOT__dut__DOT__fio_name__v0;
    }
    if (__VdlySet__test_copper_lists__DOT__dut__DOT__tile_inst__DOT__attr_table_ram__DOT__mem__v0) {
        vlSelfRef.test_copper_lists__DOT__dut__DOT__tile_inst__DOT__attr_table_ram__DOT__mem[__VdlyDim0__test_copper_lists__DOT__dut__DOT__tile_inst__DOT__attr_table_ram__DOT__mem__v0] 
            = __VdlyVal__test_copper_lists__DOT__dut__DOT__tile_inst__DOT__attr_table_ram__DOT__mem__v0;
    }
    if (__VdlySet__test_copper_lists__DOT__dut__DOT__tile_inst__DOT__nametable_ram__DOT__mem__v0) {
        vlSelfRef.test_copper_lists__DOT__dut__DOT__tile_inst__DOT__nametable_ram__DOT__mem[__VdlyDim0__test_copper_lists__DOT__dut__DOT__tile_inst__DOT__nametable_ram__DOT__mem__v0] 
            = __VdlyVal__test_copper_lists__DOT__dut__DOT__tile_inst__DOT__nametable_ram__DOT__mem__v0;
    }
    if (vlSelfRef.test_copper_lists__DOT__rst) {
        __Vdly__test_copper_lists__DOT__dut__DOT__h_count = 0U;
        __Vdly__test_copper_lists__DOT__dut__DOT__v_count = 0U;
        __Vdly__test_copper_lists__DOT__dut__DOT__tile_inst__DOT__dma_state = 0U;
        __Vdly__test_copper_lists__DOT__dut__DOT__tile_inst__DOT__dma_data_valid = 0U;
    } else {
        if ((0x031fU == (IData)(vlSelfRef.test_copper_lists__DOT__dut__DOT__h_count))) {
            __Vdly__test_copper_lists__DOT__dut__DOT__v_count 
                = ((0x020cU == (IData)(vlSelfRef.test_copper_lists__DOT__dut__DOT__v_count))
                    ? 0U : (0x000003ffU & ((IData)(1U) 
                                           + (IData)(vlSelfRef.test_copper_lists__DOT__dut__DOT__v_count))));
            __Vdly__test_copper_lists__DOT__dut__DOT__h_count = 0U;
        } else {
            __Vdly__test_copper_lists__DOT__dut__DOT__h_count 
                = (0x000003ffU & ((IData)(1U) + (IData)(vlSelfRef.test_copper_lists__DOT__dut__DOT__h_count)));
        }
        __Vdly__test_copper_lists__DOT__dut__DOT__tile_inst__DOT__dma_data_valid = 0U;
        if ((0x00000010U & (IData)(vlSelfRef.test_copper_lists__DOT__dut__DOT__tile_inst__DOT__dma_state))) {
            __Vdly__test_copper_lists__DOT__dut__DOT__tile_inst__DOT__dma_state = 0U;
        } else if ((8U & (IData)(vlSelfRef.test_copper_lists__DOT__dut__DOT__tile_inst__DOT__dma_state))) {
            if ((4U & (IData)(vlSelfRef.test_copper_lists__DOT__dut__DOT__tile_inst__DOT__dma_state))) {
                __Vdly__test_copper_lists__DOT__dut__DOT__tile_inst__DOT__dma_state 
                    = ((2U & (IData)(vlSelfRef.test_copper_lists__DOT__dut__DOT__tile_inst__DOT__dma_state))
                        ? 0U : ((1U & (IData)(vlSelfRef.test_copper_lists__DOT__dut__DOT__tile_inst__DOT__dma_state))
                                 ? 0x0eU : 0x0dU));
            } else if ((2U & (IData)(vlSelfRef.test_copper_lists__DOT__dut__DOT__tile_inst__DOT__dma_state))) {
                if ((1U & (IData)(vlSelfRef.test_copper_lists__DOT__dut__DOT__tile_inst__DOT__dma_state))) {
                    if ((0x18U > (IData)(vlSelfRef.test_copper_lists__DOT__dut__DOT__tile_inst__DOT__dma_col_idx))) {
                        __Vdly__test_copper_lists__DOT__dut__DOT__tile_inst__DOT__dma_col_idx 
                            = (0x0000001fU & ((IData)(1U) 
                                              + (IData)(vlSelfRef.test_copper_lists__DOT__dut__DOT__tile_inst__DOT__dma_col_idx)));
                    } else {
                        __Vdly__test_copper_lists__DOT__dut__DOT__tile_inst__DOT__dma_state = 0U;
                    }
                } else if ((0x03e7U > (IData)(vlSelfRef.test_copper_lists__DOT__dut__DOT__tile_inst__DOT__dma_count))) {
                    __Vdly__test_copper_lists__DOT__dut__DOT__tile_inst__DOT__dma_count 
                        = (0x0000ffffU & ((IData)(1U) 
                                          + (IData)(vlSelfRef.test_copper_lists__DOT__dut__DOT__tile_inst__DOT__dma_count)));
                } else {
                    __Vdly__test_copper_lists__DOT__dut__DOT__tile_inst__DOT__dma_state = 0U;
                }
            } else if ((1U & (IData)(vlSelfRef.test_copper_lists__DOT__dut__DOT__tile_inst__DOT__dma_state))) {
                if ((0x0fa0U > (IData)(vlSelfRef.test_copper_lists__DOT__dut__DOT__tile_inst__DOT__dma_fill_addr))) {
                    vlSelfRef.test_copper_lists__DOT__dut__DOT__tile_inst__DOT__dma_fill_addr 
                        = (0x00000fffU & ((IData)(1U) 
                                          + (IData)(vlSelfRef.test_copper_lists__DOT__dut__DOT__tile_inst__DOT__dma_fill_addr)));
                } else {
                    __Vdly__test_copper_lists__DOT__dut__DOT__tile_inst__DOT__dma_state = 0U;
                }
            } else {
                if (vlSelfRef.test_copper_lists__DOT__dut__DOT__tile_inst__DOT__dma_data_valid) {
                    __Vdly__test_copper_lists__DOT__dut__DOT__tile_inst__DOT__dma_col_idx 
                        = (0x0000001fU & ((IData)(1U) 
                                          + (IData)(vlSelfRef.test_copper_lists__DOT__dut__DOT__tile_inst__DOT__dma_col_idx)));
                    __Vdly__test_copper_lists__DOT__dut__DOT__tile_inst__DOT__dma_count 
                        = (0x0000ffffU & ((IData)(vlSelfRef.test_copper_lists__DOT__dut__DOT__tile_inst__DOT__dma_count) 
                                          - (IData)(1U)));
                }
                if (((0U == (IData)(vlSelfRef.test_copper_lists__DOT__dut__DOT__tile_inst__DOT__dma_count)) 
                     & (~ (IData)(vlSelfRef.test_copper_lists__DOT__dut__DOT__tile_inst__DOT__dma_data_valid)))) {
                    __Vdly__test_copper_lists__DOT__dut__DOT__tile_inst__DOT__dma_state = 0U;
                } else if ((1U & ((~ (IData)(vlSelfRef.test_copper_lists__DOT__dut__DOT__tile_inst__DOT__dma_data_valid)) 
                                  | (1U < (IData)(vlSelfRef.test_copper_lists__DOT__dut__DOT__tile_inst__DOT__dma_count))))) {
                    __Vdly__test_copper_lists__DOT__dut__DOT__tile_inst__DOT__dma_src 
                        = (0x0000ffffU & ((IData)(1U) 
                                          + (IData)(vlSelfRef.test_copper_lists__DOT__dut__DOT__tile_inst__DOT__dma_src)));
                    __Vdly__test_copper_lists__DOT__dut__DOT__tile_inst__DOT__dma_data_valid = 1U;
                }
            }
        } else if ((4U & (IData)(vlSelfRef.test_copper_lists__DOT__dut__DOT__tile_inst__DOT__dma_state))) {
            if ((2U & (IData)(vlSelfRef.test_copper_lists__DOT__dut__DOT__tile_inst__DOT__dma_state))) {
                if ((1U & (IData)(vlSelfRef.test_copper_lists__DOT__dut__DOT__tile_inst__DOT__dma_state))) {
                    if (vlSelfRef.test_copper_lists__DOT__dut__DOT__tile_inst__DOT__dma_data_valid) {
                        __Vdly__test_copper_lists__DOT__dut__DOT__tile_inst__DOT__dma_count 
                            = (0x0000ffffU & ((IData)(vlSelfRef.test_copper_lists__DOT__dut__DOT__tile_inst__DOT__dma_count) 
                                              - (IData)(1U)));
                        __Vdly__test_copper_lists__DOT__dut__DOT__tile_inst__DOT__dma_dst 
                            = (0x0000ffffU & ((IData)(1U) 
                                              + (IData)(vlSelfRef.test_copper_lists__DOT__dut__DOT__tile_inst__DOT__dma_dst)));
                    }
                    if (((0U == (IData)(vlSelfRef.test_copper_lists__DOT__dut__DOT__tile_inst__DOT__dma_count)) 
                         & (~ (IData)(vlSelfRef.test_copper_lists__DOT__dut__DOT__tile_inst__DOT__dma_data_valid)))) {
                        __Vdly__test_copper_lists__DOT__dut__DOT__tile_inst__DOT__dma_state = 0U;
                    } else if ((1U & ((~ (IData)(vlSelfRef.test_copper_lists__DOT__dut__DOT__tile_inst__DOT__dma_data_valid)) 
                                      | (1U < (IData)(vlSelfRef.test_copper_lists__DOT__dut__DOT__tile_inst__DOT__dma_count))))) {
                        __Vdly__test_copper_lists__DOT__dut__DOT__tile_inst__DOT__dma_src 
                            = (0x0000ffffU & ((IData)(1U) 
                                              + (IData)(vlSelfRef.test_copper_lists__DOT__dut__DOT__tile_inst__DOT__dma_src)));
                        __Vdly__test_copper_lists__DOT__dut__DOT__tile_inst__DOT__dma_data_valid = 1U;
                    }
                } else {
                    if (vlSelfRef.test_copper_lists__DOT__dut__DOT__tile_inst__DOT__dma_data_valid) {
                        __Vdly__test_copper_lists__DOT__dut__DOT__tile_inst__DOT__dma_col_idx 
                            = (0x0000001fU & ((IData)(1U) 
                                              + (IData)(vlSelfRef.test_copper_lists__DOT__dut__DOT__tile_inst__DOT__dma_col_idx)));
                        __Vdly__test_copper_lists__DOT__dut__DOT__tile_inst__DOT__dma_count 
                            = (0x0000ffffU & ((IData)(vlSelfRef.test_copper_lists__DOT__dut__DOT__tile_inst__DOT__dma_count) 
                                              - (IData)(1U)));
                    }
                    if (((0U == (IData)(vlSelfRef.test_copper_lists__DOT__dut__DOT__tile_inst__DOT__dma_count)) 
                         & (~ (IData)(vlSelfRef.test_copper_lists__DOT__dut__DOT__tile_inst__DOT__dma_data_valid)))) {
                        __Vdly__test_copper_lists__DOT__dut__DOT__tile_inst__DOT__dma_state = 0U;
                    } else if ((1U & ((~ (IData)(vlSelfRef.test_copper_lists__DOT__dut__DOT__tile_inst__DOT__dma_data_valid)) 
                                      | (1U < (IData)(vlSelfRef.test_copper_lists__DOT__dut__DOT__tile_inst__DOT__dma_count))))) {
                        __Vdly__test_copper_lists__DOT__dut__DOT__tile_inst__DOT__dma_src 
                            = (0x0000ffffU & ((IData)(1U) 
                                              + (IData)(vlSelfRef.test_copper_lists__DOT__dut__DOT__tile_inst__DOT__dma_src)));
                        __Vdly__test_copper_lists__DOT__dut__DOT__tile_inst__DOT__dma_data_valid = 1U;
                    }
                }
            } else if ((1U & (IData)(vlSelfRef.test_copper_lists__DOT__dut__DOT__tile_inst__DOT__dma_state))) {
                if (vlSelfRef.test_copper_lists__DOT__dut__DOT__tile_inst__DOT__dma_data_valid) {
                    __Vdly__test_copper_lists__DOT__dut__DOT__tile_inst__DOT__dma_count 
                        = (0x0000ffffU & ((IData)(vlSelfRef.test_copper_lists__DOT__dut__DOT__tile_inst__DOT__dma_count) 
                                          - (IData)(1U)));
                    __Vdly__test_copper_lists__DOT__dut__DOT__tile_inst__DOT__dma_dst 
                        = (0x0000ffffU & ((IData)(1U) 
                                          + (IData)(vlSelfRef.test_copper_lists__DOT__dut__DOT__tile_inst__DOT__dma_dst)));
                }
                if (((0U == (IData)(vlSelfRef.test_copper_lists__DOT__dut__DOT__tile_inst__DOT__dma_count)) 
                     & (~ (IData)(vlSelfRef.test_copper_lists__DOT__dut__DOT__tile_inst__DOT__dma_data_valid)))) {
                    __Vdly__test_copper_lists__DOT__dut__DOT__tile_inst__DOT__dma_state = 0U;
                } else if ((1U & ((~ (IData)(vlSelfRef.test_copper_lists__DOT__dut__DOT__tile_inst__DOT__dma_data_valid)) 
                                  | (1U < (IData)(vlSelfRef.test_copper_lists__DOT__dut__DOT__tile_inst__DOT__dma_count))))) {
                    __Vdly__test_copper_lists__DOT__dut__DOT__tile_inst__DOT__dma_src 
                        = (0x0000ffffU & ((IData)(1U) 
                                          + (IData)(vlSelfRef.test_copper_lists__DOT__dut__DOT__tile_inst__DOT__dma_src)));
                    __Vdly__test_copper_lists__DOT__dut__DOT__tile_inst__DOT__dma_data_valid = 1U;
                }
            } else {
                if (vlSelfRef.test_copper_lists__DOT__dut__DOT__tile_inst__DOT__dma_data_valid) {
                    if ((0U == (IData)(vlSelfRef.test_copper_lists__DOT__dut__DOT__tile_inst__DOT__dma_pal_phase))) {
                        __Vdly__test_copper_lists__DOT__dut__DOT__tile_inst__DOT__dma_pal_phase = 1U;
                    } else if ((1U == (IData)(vlSelfRef.test_copper_lists__DOT__dut__DOT__tile_inst__DOT__dma_pal_phase))) {
                        __Vdly__test_copper_lists__DOT__dut__DOT__tile_inst__DOT__dma_pal_phase = 2U;
                    } else if ((2U == (IData)(vlSelfRef.test_copper_lists__DOT__dut__DOT__tile_inst__DOT__dma_pal_phase))) {
                        __Vdly__test_copper_lists__DOT__dut__DOT__tile_inst__DOT__dma_pal_phase = 0U;
                        if ((0x0fU == (IData)(vlSelfRef.test_copper_lists__DOT__dut__DOT__tile_inst__DOT__dma_pal_cidx))) {
                            __Vdly__test_copper_lists__DOT__dut__DOT__tile_inst__DOT__dma_count = 0U;
                        } else {
                            vlSelfRef.test_copper_lists__DOT__dut__DOT__tile_inst__DOT__dma_pal_cidx 
                                = (0x0000000fU & ((IData)(1U) 
                                                  + (IData)(vlSelfRef.test_copper_lists__DOT__dut__DOT__tile_inst__DOT__dma_pal_cidx)));
                        }
                    } else {
                        __Vdly__test_copper_lists__DOT__dut__DOT__tile_inst__DOT__dma_pal_phase = 0U;
                    }
                }
                if (((0U == (IData)(vlSelfRef.test_copper_lists__DOT__dut__DOT__tile_inst__DOT__dma_count)) 
                     & (~ (IData)(vlSelfRef.test_copper_lists__DOT__dut__DOT__tile_inst__DOT__dma_data_valid)))) {
                    __Vdly__test_copper_lists__DOT__dut__DOT__tile_inst__DOT__dma_state = 0U;
                } else if ((1U & ((~ (IData)(vlSelfRef.test_copper_lists__DOT__dut__DOT__tile_inst__DOT__dma_data_valid)) 
                                  | (0U < (IData)(vlSelfRef.test_copper_lists__DOT__dut__DOT__tile_inst__DOT__dma_count))))) {
                    __Vdly__test_copper_lists__DOT__dut__DOT__tile_inst__DOT__dma_src 
                        = (0x0000ffffU & ((IData)(1U) 
                                          + (IData)(vlSelfRef.test_copper_lists__DOT__dut__DOT__tile_inst__DOT__dma_src)));
                    __Vdly__test_copper_lists__DOT__dut__DOT__tile_inst__DOT__dma_data_valid = 1U;
                }
            }
        } else if ((2U & (IData)(vlSelfRef.test_copper_lists__DOT__dut__DOT__tile_inst__DOT__dma_state))) {
            if ((1U & (IData)(vlSelfRef.test_copper_lists__DOT__dut__DOT__tile_inst__DOT__dma_state))) {
                if (vlSelfRef.test_copper_lists__DOT__dut__DOT__tile_inst__DOT__dma_data_valid) {
                    __Vdly__test_copper_lists__DOT__dut__DOT__tile_inst__DOT__dma_count 
                        = (0x0000ffffU & ((IData)(vlSelfRef.test_copper_lists__DOT__dut__DOT__tile_inst__DOT__dma_count) 
                                          - (IData)(1U)));
                    __Vdly__test_copper_lists__DOT__dut__DOT__tile_inst__DOT__dma_dst 
                        = (0x0000ffffU & ((IData)(1U) 
                                          + (IData)(vlSelfRef.test_copper_lists__DOT__dut__DOT__tile_inst__DOT__dma_dst)));
                }
                if (((0U == (IData)(vlSelfRef.test_copper_lists__DOT__dut__DOT__tile_inst__DOT__dma_count)) 
                     & (~ (IData)(vlSelfRef.test_copper_lists__DOT__dut__DOT__tile_inst__DOT__dma_data_valid)))) {
                    __Vdly__test_copper_lists__DOT__dut__DOT__tile_inst__DOT__dma_state = 0U;
                } else if ((1U & ((~ (IData)(vlSelfRef.test_copper_lists__DOT__dut__DOT__tile_inst__DOT__dma_data_valid)) 
                                  | (1U < (IData)(vlSelfRef.test_copper_lists__DOT__dut__DOT__tile_inst__DOT__dma_count))))) {
                    __Vdly__test_copper_lists__DOT__dut__DOT__tile_inst__DOT__dma_src 
                        = (0x0000ffffU & ((IData)(1U) 
                                          + (IData)(vlSelfRef.test_copper_lists__DOT__dut__DOT__tile_inst__DOT__dma_src)));
                    __Vdly__test_copper_lists__DOT__dut__DOT__tile_inst__DOT__dma_data_valid = 1U;
                }
            } else {
                if (vlSelfRef.test_copper_lists__DOT__dut__DOT__tile_inst__DOT__dma_data_valid) {
                    __Vdly__test_copper_lists__DOT__dut__DOT__tile_inst__DOT__dma_count 
                        = (0x0000ffffU & ((IData)(vlSelfRef.test_copper_lists__DOT__dut__DOT__tile_inst__DOT__dma_count) 
                                          - (IData)(1U)));
                    __Vdly__test_copper_lists__DOT__dut__DOT__tile_inst__DOT__dma_dst 
                        = (0x0000ffffU & ((IData)(1U) 
                                          + (IData)(vlSelfRef.test_copper_lists__DOT__dut__DOT__tile_inst__DOT__dma_dst)));
                }
                if (((0U == (IData)(vlSelfRef.test_copper_lists__DOT__dut__DOT__tile_inst__DOT__dma_count)) 
                     & (~ (IData)(vlSelfRef.test_copper_lists__DOT__dut__DOT__tile_inst__DOT__dma_data_valid)))) {
                    __Vdly__test_copper_lists__DOT__dut__DOT__tile_inst__DOT__dma_state = 0U;
                } else if ((1U & ((~ (IData)(vlSelfRef.test_copper_lists__DOT__dut__DOT__tile_inst__DOT__dma_data_valid)) 
                                  | (1U < (IData)(vlSelfRef.test_copper_lists__DOT__dut__DOT__tile_inst__DOT__dma_count))))) {
                    __Vdly__test_copper_lists__DOT__dut__DOT__tile_inst__DOT__dma_src 
                        = (0x0000ffffU & ((IData)(1U) 
                                          + (IData)(vlSelfRef.test_copper_lists__DOT__dut__DOT__tile_inst__DOT__dma_src)));
                    __Vdly__test_copper_lists__DOT__dut__DOT__tile_inst__DOT__dma_data_valid = 1U;
                }
            }
        } else if ((1U & (IData)(vlSelfRef.test_copper_lists__DOT__dut__DOT__tile_inst__DOT__dma_state))) {
            if (vlSelfRef.test_copper_lists__DOT__dut__DOT__tile_inst__DOT__dma_data_valid) {
                __Vdly__test_copper_lists__DOT__dut__DOT__tile_inst__DOT__dma_count 
                    = (0x0000ffffU & ((IData)(vlSelfRef.test_copper_lists__DOT__dut__DOT__tile_inst__DOT__dma_count) 
                                      - (IData)(1U)));
                __Vdly__test_copper_lists__DOT__dut__DOT__tile_inst__DOT__dma_dst 
                    = (0x0000ffffU & ((IData)(1U) + (IData)(vlSelfRef.test_copper_lists__DOT__dut__DOT__tile_inst__DOT__dma_dst)));
            }
            if (((0U == (IData)(vlSelfRef.test_copper_lists__DOT__dut__DOT__tile_inst__DOT__dma_count)) 
                 & (~ (IData)(vlSelfRef.test_copper_lists__DOT__dut__DOT__tile_inst__DOT__dma_data_valid)))) {
                __Vdly__test_copper_lists__DOT__dut__DOT__tile_inst__DOT__dma_state = 0U;
            } else if ((1U & ((~ (IData)(vlSelfRef.test_copper_lists__DOT__dut__DOT__tile_inst__DOT__dma_data_valid)) 
                              | (1U < (IData)(vlSelfRef.test_copper_lists__DOT__dut__DOT__tile_inst__DOT__dma_count))))) {
                __Vdly__test_copper_lists__DOT__dut__DOT__tile_inst__DOT__dma_src 
                    = (0x0000ffffU & ((IData)(1U) + (IData)(vlSelfRef.test_copper_lists__DOT__dut__DOT__tile_inst__DOT__dma_src)));
                __Vdly__test_copper_lists__DOT__dut__DOT__tile_inst__DOT__dma_data_valid = 1U;
            }
        }
        if (((IData)(vlSelfRef.test_copper_lists__DOT__dut__DOT____Vcellinp__tile_inst__cpu_we) 
             & (IData)(vlSelfRef.test_copper_lists__DOT__dut__DOT__tile_reg_sel))) {
            if ((0U != (0x0000001fU & (IData)(vlSelfRef.test_copper_lists__DOT__cpu_addr)))) {
                if ((1U != (0x0000001fU & (IData)(vlSelfRef.test_copper_lists__DOT__cpu_addr)))) {
                    if ((2U != (0x0000001fU & (IData)(vlSelfRef.test_copper_lists__DOT__cpu_addr)))) {
                        if ((3U != (0x0000001fU & (IData)(vlSelfRef.test_copper_lists__DOT__cpu_addr)))) {
                            if ((4U != (0x0000001fU 
                                        & (IData)(vlSelfRef.test_copper_lists__DOT__cpu_addr)))) {
                                if ((5U != (0x0000001fU 
                                            & (IData)(vlSelfRef.test_copper_lists__DOT__cpu_addr)))) {
                                    if ((7U == (0x0000001fU 
                                                & (IData)(vlSelfRef.test_copper_lists__DOT__cpu_addr)))) {
                                        if ((0U == (IData)(vlSelfRef.test_copper_lists__DOT__dut__DOT__tile_inst__DOT__dma_state))) {
                                            if ((1U 
                                                 & (~ 
                                                    ((IData)(vlSelfRef.test_copper_lists__DOT__cpu_wdata) 
                                                     >> 7U)))) {
                                                if (
                                                    (1U 
                                                     & (~ 
                                                        ((IData)(vlSelfRef.test_copper_lists__DOT__cpu_wdata) 
                                                         >> 6U)))) {
                                                    if (
                                                        (1U 
                                                         & (~ 
                                                            ((IData)(vlSelfRef.test_copper_lists__DOT__cpu_wdata) 
                                                             >> 5U)))) {
                                                        if (
                                                            (0x00000010U 
                                                             & (IData)(vlSelfRef.test_copper_lists__DOT__cpu_wdata))) {
                                                            if (
                                                                (1U 
                                                                 & (~ 
                                                                    ((IData)(vlSelfRef.test_copper_lists__DOT__cpu_wdata) 
                                                                     >> 3U)))) {
                                                                if (
                                                                    (1U 
                                                                     & (~ 
                                                                        ((IData)(vlSelfRef.test_copper_lists__DOT__cpu_wdata) 
                                                                         >> 2U)))) {
                                                                    if (
                                                                        (2U 
                                                                         & (IData)(vlSelfRef.test_copper_lists__DOT__cpu_wdata))) {
                                                                        if (
                                                                            (1U 
                                                                             & (IData)(vlSelfRef.test_copper_lists__DOT__cpu_wdata))) {
                                                                            if (
                                                                                ((4U 
                                                                                > vlSelfRef.test_copper_lists__DOT__dut__DOT__tile_inst__DOT__tregs[8U]) 
                                                                                & (0x28U 
                                                                                > vlSelfRef.test_copper_lists__DOT__dut__DOT__tile_inst__DOT__tregs[9U]))) {
                                                                                vlSelfRef.test_copper_lists__DOT__dut__DOT__tile_inst__DOT__dma_nt 
                                                                                = 
                                                                                (3U 
                                                                                & vlSelfRef.test_copper_lists__DOT__dut__DOT__tile_inst__DOT__tregs[8U]);
                                                                                vlSelfRef.test_copper_lists__DOT__dut__DOT__tile_inst__DOT__dma_col_row 
                                                                                = 
                                                                                (0x0000003fU 
                                                                                & vlSelfRef.test_copper_lists__DOT__dut__DOT__tile_inst__DOT__tregs[9U]);
                                                                                __Vdly__test_copper_lists__DOT__dut__DOT__tile_inst__DOT__dma_col_idx = 0U;
                                                                                __Vdly__test_copper_lists__DOT__dut__DOT__tile_inst__DOT__dma_state = 0x0bU;
                                                                            }
                                                                        } else {
                                                                            if (
                                                                                (0U 
                                                                                >= vlSelfRef.test_copper_lists__DOT__dut__DOT__tile_inst__DOT__tregs[8U])) {
                                                                                vlSelfRef.test_copper_lists__DOT__dut__DOT__tile_inst__DOT____Vlvbound_ha768cd16__0 
                                                                                = vlSelfRef.test_copper_lists__DOT__dut__DOT__tile_inst__DOT__tregs[10U];
                                                                                __VdlyVal__test_copper_lists__DOT__dut__DOT__tile_inst__DOT__col_buffer__v0 
                                                                                = vlSelfRef.test_copper_lists__DOT__dut__DOT__tile_inst__DOT____Vlvbound_ha768cd16__0;
                                                                                __VdlySet__test_copper_lists__DOT__dut__DOT__tile_inst__DOT__col_buffer__v0 = 1U;
                                                                            }
                                                                            if (
                                                                                ((1U 
                                                                                >= vlSelfRef.test_copper_lists__DOT__dut__DOT__tile_inst__DOT__tregs[8U]) 
                                                                                & (1U 
                                                                                <= vlSelfRef.test_copper_lists__DOT__dut__DOT__tile_inst__DOT__tregs[9U]))) {
                                                                                vlSelfRef.test_copper_lists__DOT__dut__DOT__tile_inst__DOT____Vlvbound_ha768cd16__0 
                                                                                = vlSelfRef.test_copper_lists__DOT__dut__DOT__tile_inst__DOT__tregs[10U];
                                                                                __VdlyVal__test_copper_lists__DOT__dut__DOT__tile_inst__DOT__col_buffer__v1 
                                                                                = vlSelfRef.test_copper_lists__DOT__dut__DOT__tile_inst__DOT____Vlvbound_ha768cd16__0;
                                                                                __VdlySet__test_copper_lists__DOT__dut__DOT__tile_inst__DOT__col_buffer__v1 = 1U;
                                                                            }
                                                                            if (
                                                                                ((2U 
                                                                                >= vlSelfRef.test_copper_lists__DOT__dut__DOT__tile_inst__DOT__tregs[8U]) 
                                                                                & (2U 
                                                                                <= vlSelfRef.test_copper_lists__DOT__dut__DOT__tile_inst__DOT__tregs[9U]))) {
                                                                                vlSelfRef.test_copper_lists__DOT__dut__DOT__tile_inst__DOT____Vlvbound_ha768cd16__0 
                                                                                = vlSelfRef.test_copper_lists__DOT__dut__DOT__tile_inst__DOT__tregs[10U];
                                                                                __VdlyVal__test_copper_lists__DOT__dut__DOT__tile_inst__DOT__col_buffer__v2 
                                                                                = vlSelfRef.test_copper_lists__DOT__dut__DOT__tile_inst__DOT____Vlvbound_ha768cd16__0;
                                                                                __VdlySet__test_copper_lists__DOT__dut__DOT__tile_inst__DOT__col_buffer__v2 = 1U;
                                                                            }
                                                                            if (
                                                                                ((3U 
                                                                                >= vlSelfRef.test_copper_lists__DOT__dut__DOT__tile_inst__DOT__tregs[8U]) 
                                                                                & (3U 
                                                                                <= vlSelfRef.test_copper_lists__DOT__dut__DOT__tile_inst__DOT__tregs[9U]))) {
                                                                                vlSelfRef.test_copper_lists__DOT__dut__DOT__tile_inst__DOT____Vlvbound_ha768cd16__0 
                                                                                = vlSelfRef.test_copper_lists__DOT__dut__DOT__tile_inst__DOT__tregs[10U];
                                                                                __VdlyVal__test_copper_lists__DOT__dut__DOT__tile_inst__DOT__col_buffer__v3 
                                                                                = vlSelfRef.test_copper_lists__DOT__dut__DOT__tile_inst__DOT____Vlvbound_ha768cd16__0;
                                                                                __VdlySet__test_copper_lists__DOT__dut__DOT__tile_inst__DOT__col_buffer__v3 = 1U;
                                                                            }
                                                                            if (
                                                                                ((4U 
                                                                                >= vlSelfRef.test_copper_lists__DOT__dut__DOT__tile_inst__DOT__tregs[8U]) 
                                                                                & (4U 
                                                                                <= vlSelfRef.test_copper_lists__DOT__dut__DOT__tile_inst__DOT__tregs[9U]))) {
                                                                                vlSelfRef.test_copper_lists__DOT__dut__DOT__tile_inst__DOT____Vlvbound_ha768cd16__0 
                                                                                = vlSelfRef.test_copper_lists__DOT__dut__DOT__tile_inst__DOT__tregs[10U];
                                                                                __VdlyVal__test_copper_lists__DOT__dut__DOT__tile_inst__DOT__col_buffer__v4 
                                                                                = vlSelfRef.test_copper_lists__DOT__dut__DOT__tile_inst__DOT____Vlvbound_ha768cd16__0;
                                                                                __VdlySet__test_copper_lists__DOT__dut__DOT__tile_inst__DOT__col_buffer__v4 = 1U;
                                                                            }
                                                                            if (
                                                                                ((5U 
                                                                                >= vlSelfRef.test_copper_lists__DOT__dut__DOT__tile_inst__DOT__tregs[8U]) 
                                                                                & (5U 
                                                                                <= vlSelfRef.test_copper_lists__DOT__dut__DOT__tile_inst__DOT__tregs[9U]))) {
                                                                                vlSelfRef.test_copper_lists__DOT__dut__DOT__tile_inst__DOT____Vlvbound_ha768cd16__0 
                                                                                = vlSelfRef.test_copper_lists__DOT__dut__DOT__tile_inst__DOT__tregs[10U];
                                                                                __VdlyVal__test_copper_lists__DOT__dut__DOT__tile_inst__DOT__col_buffer__v5 
                                                                                = vlSelfRef.test_copper_lists__DOT__dut__DOT__tile_inst__DOT____Vlvbound_ha768cd16__0;
                                                                                __VdlySet__test_copper_lists__DOT__dut__DOT__tile_inst__DOT__col_buffer__v5 = 1U;
                                                                            }
                                                                            if (
                                                                                ((6U 
                                                                                >= vlSelfRef.test_copper_lists__DOT__dut__DOT__tile_inst__DOT__tregs[8U]) 
                                                                                & (6U 
                                                                                <= vlSelfRef.test_copper_lists__DOT__dut__DOT__tile_inst__DOT__tregs[9U]))) {
                                                                                vlSelfRef.test_copper_lists__DOT__dut__DOT__tile_inst__DOT____Vlvbound_ha768cd16__0 
                                                                                = vlSelfRef.test_copper_lists__DOT__dut__DOT__tile_inst__DOT__tregs[10U];
                                                                                __VdlyVal__test_copper_lists__DOT__dut__DOT__tile_inst__DOT__col_buffer__v6 
                                                                                = vlSelfRef.test_copper_lists__DOT__dut__DOT__tile_inst__DOT____Vlvbound_ha768cd16__0;
                                                                                __VdlySet__test_copper_lists__DOT__dut__DOT__tile_inst__DOT__col_buffer__v6 = 1U;
                                                                            }
                                                                            if (
                                                                                ((7U 
                                                                                >= vlSelfRef.test_copper_lists__DOT__dut__DOT__tile_inst__DOT__tregs[8U]) 
                                                                                & (7U 
                                                                                <= vlSelfRef.test_copper_lists__DOT__dut__DOT__tile_inst__DOT__tregs[9U]))) {
                                                                                vlSelfRef.test_copper_lists__DOT__dut__DOT__tile_inst__DOT____Vlvbound_ha768cd16__0 
                                                                                = vlSelfRef.test_copper_lists__DOT__dut__DOT__tile_inst__DOT__tregs[10U];
                                                                                __VdlyVal__test_copper_lists__DOT__dut__DOT__tile_inst__DOT__col_buffer__v7 
                                                                                = vlSelfRef.test_copper_lists__DOT__dut__DOT__tile_inst__DOT____Vlvbound_ha768cd16__0;
                                                                                __VdlySet__test_copper_lists__DOT__dut__DOT__tile_inst__DOT__col_buffer__v7 = 1U;
                                                                            }
                                                                            if (
                                                                                ((8U 
                                                                                >= vlSelfRef.test_copper_lists__DOT__dut__DOT__tile_inst__DOT__tregs[8U]) 
                                                                                & (8U 
                                                                                <= vlSelfRef.test_copper_lists__DOT__dut__DOT__tile_inst__DOT__tregs[9U]))) {
                                                                                vlSelfRef.test_copper_lists__DOT__dut__DOT__tile_inst__DOT____Vlvbound_ha768cd16__0 
                                                                                = vlSelfRef.test_copper_lists__DOT__dut__DOT__tile_inst__DOT__tregs[10U];
                                                                                __VdlyVal__test_copper_lists__DOT__dut__DOT__tile_inst__DOT__col_buffer__v8 
                                                                                = vlSelfRef.test_copper_lists__DOT__dut__DOT__tile_inst__DOT____Vlvbound_ha768cd16__0;
                                                                                __VdlySet__test_copper_lists__DOT__dut__DOT__tile_inst__DOT__col_buffer__v8 = 1U;
                                                                            }
                                                                            if (
                                                                                ((9U 
                                                                                >= vlSelfRef.test_copper_lists__DOT__dut__DOT__tile_inst__DOT__tregs[8U]) 
                                                                                & (9U 
                                                                                <= vlSelfRef.test_copper_lists__DOT__dut__DOT__tile_inst__DOT__tregs[9U]))) {
                                                                                vlSelfRef.test_copper_lists__DOT__dut__DOT__tile_inst__DOT____Vlvbound_ha768cd16__0 
                                                                                = vlSelfRef.test_copper_lists__DOT__dut__DOT__tile_inst__DOT__tregs[10U];
                                                                                __VdlyVal__test_copper_lists__DOT__dut__DOT__tile_inst__DOT__col_buffer__v9 
                                                                                = vlSelfRef.test_copper_lists__DOT__dut__DOT__tile_inst__DOT____Vlvbound_ha768cd16__0;
                                                                                __VdlySet__test_copper_lists__DOT__dut__DOT__tile_inst__DOT__col_buffer__v9 = 1U;
                                                                            }
                                                                            if (
                                                                                ((0x0aU 
                                                                                >= vlSelfRef.test_copper_lists__DOT__dut__DOT__tile_inst__DOT__tregs[8U]) 
                                                                                & (0x0aU 
                                                                                <= vlSelfRef.test_copper_lists__DOT__dut__DOT__tile_inst__DOT__tregs[9U]))) {
                                                                                vlSelfRef.test_copper_lists__DOT__dut__DOT__tile_inst__DOT____Vlvbound_ha768cd16__0 
                                                                                = vlSelfRef.test_copper_lists__DOT__dut__DOT__tile_inst__DOT__tregs[10U];
                                                                                __VdlyVal__test_copper_lists__DOT__dut__DOT__tile_inst__DOT__col_buffer__v10 
                                                                                = vlSelfRef.test_copper_lists__DOT__dut__DOT__tile_inst__DOT____Vlvbound_ha768cd16__0;
                                                                                __VdlySet__test_copper_lists__DOT__dut__DOT__tile_inst__DOT__col_buffer__v10 = 1U;
                                                                            }
                                                                            if (
                                                                                ((0x0bU 
                                                                                >= vlSelfRef.test_copper_lists__DOT__dut__DOT__tile_inst__DOT__tregs[8U]) 
                                                                                & (0x0bU 
                                                                                <= vlSelfRef.test_copper_lists__DOT__dut__DOT__tile_inst__DOT__tregs[9U]))) {
                                                                                vlSelfRef.test_copper_lists__DOT__dut__DOT__tile_inst__DOT____Vlvbound_ha768cd16__0 
                                                                                = vlSelfRef.test_copper_lists__DOT__dut__DOT__tile_inst__DOT__tregs[10U];
                                                                                __VdlyVal__test_copper_lists__DOT__dut__DOT__tile_inst__DOT__col_buffer__v11 
                                                                                = vlSelfRef.test_copper_lists__DOT__dut__DOT__tile_inst__DOT____Vlvbound_ha768cd16__0;
                                                                                __VdlySet__test_copper_lists__DOT__dut__DOT__tile_inst__DOT__col_buffer__v11 = 1U;
                                                                            }
                                                                            if (
                                                                                ((0x0cU 
                                                                                >= vlSelfRef.test_copper_lists__DOT__dut__DOT__tile_inst__DOT__tregs[8U]) 
                                                                                & (0x0cU 
                                                                                <= vlSelfRef.test_copper_lists__DOT__dut__DOT__tile_inst__DOT__tregs[9U]))) {
                                                                                vlSelfRef.test_copper_lists__DOT__dut__DOT__tile_inst__DOT____Vlvbound_ha768cd16__0 
                                                                                = vlSelfRef.test_copper_lists__DOT__dut__DOT__tile_inst__DOT__tregs[10U];
                                                                                __VdlyVal__test_copper_lists__DOT__dut__DOT__tile_inst__DOT__col_buffer__v12 
                                                                                = vlSelfRef.test_copper_lists__DOT__dut__DOT__tile_inst__DOT____Vlvbound_ha768cd16__0;
                                                                                __VdlySet__test_copper_lists__DOT__dut__DOT__tile_inst__DOT__col_buffer__v12 = 1U;
                                                                            }
                                                                            if (
                                                                                ((0x0dU 
                                                                                >= vlSelfRef.test_copper_lists__DOT__dut__DOT__tile_inst__DOT__tregs[8U]) 
                                                                                & (0x0dU 
                                                                                <= vlSelfRef.test_copper_lists__DOT__dut__DOT__tile_inst__DOT__tregs[9U]))) {
                                                                                vlSelfRef.test_copper_lists__DOT__dut__DOT__tile_inst__DOT____Vlvbound_ha768cd16__0 
                                                                                = vlSelfRef.test_copper_lists__DOT__dut__DOT__tile_inst__DOT__tregs[10U];
                                                                                __VdlyVal__test_copper_lists__DOT__dut__DOT__tile_inst__DOT__col_buffer__v13 
                                                                                = vlSelfRef.test_copper_lists__DOT__dut__DOT__tile_inst__DOT____Vlvbound_ha768cd16__0;
                                                                                __VdlySet__test_copper_lists__DOT__dut__DOT__tile_inst__DOT__col_buffer__v13 = 1U;
                                                                            }
                                                                            if (
                                                                                ((0x0eU 
                                                                                >= vlSelfRef.test_copper_lists__DOT__dut__DOT__tile_inst__DOT__tregs[8U]) 
                                                                                & (0x0eU 
                                                                                <= vlSelfRef.test_copper_lists__DOT__dut__DOT__tile_inst__DOT__tregs[9U]))) {
                                                                                vlSelfRef.test_copper_lists__DOT__dut__DOT__tile_inst__DOT____Vlvbound_ha768cd16__0 
                                                                                = vlSelfRef.test_copper_lists__DOT__dut__DOT__tile_inst__DOT__tregs[10U];
                                                                                __VdlyVal__test_copper_lists__DOT__dut__DOT__tile_inst__DOT__col_buffer__v14 
                                                                                = vlSelfRef.test_copper_lists__DOT__dut__DOT__tile_inst__DOT____Vlvbound_ha768cd16__0;
                                                                                __VdlySet__test_copper_lists__DOT__dut__DOT__tile_inst__DOT__col_buffer__v14 = 1U;
                                                                            }
                                                                            if (
                                                                                ((0x0fU 
                                                                                >= vlSelfRef.test_copper_lists__DOT__dut__DOT__tile_inst__DOT__tregs[8U]) 
                                                                                & (0x0fU 
                                                                                <= vlSelfRef.test_copper_lists__DOT__dut__DOT__tile_inst__DOT__tregs[9U]))) {
                                                                                vlSelfRef.test_copper_lists__DOT__dut__DOT__tile_inst__DOT____Vlvbound_ha768cd16__0 
                                                                                = vlSelfRef.test_copper_lists__DOT__dut__DOT__tile_inst__DOT__tregs[10U];
                                                                                __VdlyVal__test_copper_lists__DOT__dut__DOT__tile_inst__DOT__col_buffer__v15 
                                                                                = vlSelfRef.test_copper_lists__DOT__dut__DOT__tile_inst__DOT____Vlvbound_ha768cd16__0;
                                                                                __VdlySet__test_copper_lists__DOT__dut__DOT__tile_inst__DOT__col_buffer__v15 = 1U;
                                                                            }
                                                                            if (
                                                                                ((0x10U 
                                                                                >= vlSelfRef.test_copper_lists__DOT__dut__DOT__tile_inst__DOT__tregs[8U]) 
                                                                                & (0x10U 
                                                                                <= vlSelfRef.test_copper_lists__DOT__dut__DOT__tile_inst__DOT__tregs[9U]))) {
                                                                                vlSelfRef.test_copper_lists__DOT__dut__DOT__tile_inst__DOT____Vlvbound_ha768cd16__0 
                                                                                = vlSelfRef.test_copper_lists__DOT__dut__DOT__tile_inst__DOT__tregs[10U];
                                                                                __VdlyVal__test_copper_lists__DOT__dut__DOT__tile_inst__DOT__col_buffer__v16 
                                                                                = vlSelfRef.test_copper_lists__DOT__dut__DOT__tile_inst__DOT____Vlvbound_ha768cd16__0;
                                                                                __VdlySet__test_copper_lists__DOT__dut__DOT__tile_inst__DOT__col_buffer__v16 = 1U;
                                                                            }
                                                                            if (
                                                                                ((0x11U 
                                                                                >= vlSelfRef.test_copper_lists__DOT__dut__DOT__tile_inst__DOT__tregs[8U]) 
                                                                                & (0x11U 
                                                                                <= vlSelfRef.test_copper_lists__DOT__dut__DOT__tile_inst__DOT__tregs[9U]))) {
                                                                                vlSelfRef.test_copper_lists__DOT__dut__DOT__tile_inst__DOT____Vlvbound_ha768cd16__0 
                                                                                = vlSelfRef.test_copper_lists__DOT__dut__DOT__tile_inst__DOT__tregs[10U];
                                                                                __VdlyVal__test_copper_lists__DOT__dut__DOT__tile_inst__DOT__col_buffer__v17 
                                                                                = vlSelfRef.test_copper_lists__DOT__dut__DOT__tile_inst__DOT____Vlvbound_ha768cd16__0;
                                                                                __VdlySet__test_copper_lists__DOT__dut__DOT__tile_inst__DOT__col_buffer__v17 = 1U;
                                                                            }
                                                                            if (
                                                                                ((0x12U 
                                                                                >= vlSelfRef.test_copper_lists__DOT__dut__DOT__tile_inst__DOT__tregs[8U]) 
                                                                                & (0x12U 
                                                                                <= vlSelfRef.test_copper_lists__DOT__dut__DOT__tile_inst__DOT__tregs[9U]))) {
                                                                                vlSelfRef.test_copper_lists__DOT__dut__DOT__tile_inst__DOT____Vlvbound_ha768cd16__0 
                                                                                = vlSelfRef.test_copper_lists__DOT__dut__DOT__tile_inst__DOT__tregs[10U];
                                                                                __VdlyVal__test_copper_lists__DOT__dut__DOT__tile_inst__DOT__col_buffer__v18 
                                                                                = vlSelfRef.test_copper_lists__DOT__dut__DOT__tile_inst__DOT____Vlvbound_ha768cd16__0;
                                                                                __VdlySet__test_copper_lists__DOT__dut__DOT__tile_inst__DOT__col_buffer__v18 = 1U;
                                                                            }
                                                                            if (
                                                                                ((0x13U 
                                                                                >= vlSelfRef.test_copper_lists__DOT__dut__DOT__tile_inst__DOT__tregs[8U]) 
                                                                                & (0x13U 
                                                                                <= vlSelfRef.test_copper_lists__DOT__dut__DOT__tile_inst__DOT__tregs[9U]))) {
                                                                                vlSelfRef.test_copper_lists__DOT__dut__DOT__tile_inst__DOT____Vlvbound_ha768cd16__0 
                                                                                = vlSelfRef.test_copper_lists__DOT__dut__DOT__tile_inst__DOT__tregs[10U];
                                                                                __VdlyVal__test_copper_lists__DOT__dut__DOT__tile_inst__DOT__col_buffer__v19 
                                                                                = vlSelfRef.test_copper_lists__DOT__dut__DOT__tile_inst__DOT____Vlvbound_ha768cd16__0;
                                                                                __VdlySet__test_copper_lists__DOT__dut__DOT__tile_inst__DOT__col_buffer__v19 = 1U;
                                                                            }
                                                                            if (
                                                                                ((0x14U 
                                                                                >= vlSelfRef.test_copper_lists__DOT__dut__DOT__tile_inst__DOT__tregs[8U]) 
                                                                                & (0x14U 
                                                                                <= vlSelfRef.test_copper_lists__DOT__dut__DOT__tile_inst__DOT__tregs[9U]))) {
                                                                                vlSelfRef.test_copper_lists__DOT__dut__DOT__tile_inst__DOT____Vlvbound_ha768cd16__0 
                                                                                = vlSelfRef.test_copper_lists__DOT__dut__DOT__tile_inst__DOT__tregs[10U];
                                                                                __VdlyVal__test_copper_lists__DOT__dut__DOT__tile_inst__DOT__col_buffer__v20 
                                                                                = vlSelfRef.test_copper_lists__DOT__dut__DOT__tile_inst__DOT____Vlvbound_ha768cd16__0;
                                                                                __VdlySet__test_copper_lists__DOT__dut__DOT__tile_inst__DOT__col_buffer__v20 = 1U;
                                                                            }
                                                                            if (
                                                                                ((0x15U 
                                                                                >= vlSelfRef.test_copper_lists__DOT__dut__DOT__tile_inst__DOT__tregs[8U]) 
                                                                                & (0x15U 
                                                                                <= vlSelfRef.test_copper_lists__DOT__dut__DOT__tile_inst__DOT__tregs[9U]))) {
                                                                                vlSelfRef.test_copper_lists__DOT__dut__DOT__tile_inst__DOT____Vlvbound_ha768cd16__0 
                                                                                = vlSelfRef.test_copper_lists__DOT__dut__DOT__tile_inst__DOT__tregs[10U];
                                                                                __VdlyVal__test_copper_lists__DOT__dut__DOT__tile_inst__DOT__col_buffer__v21 
                                                                                = vlSelfRef.test_copper_lists__DOT__dut__DOT__tile_inst__DOT____Vlvbound_ha768cd16__0;
                                                                                __VdlySet__test_copper_lists__DOT__dut__DOT__tile_inst__DOT__col_buffer__v21 = 1U;
                                                                            }
                                                                            if (
                                                                                ((0x16U 
                                                                                >= vlSelfRef.test_copper_lists__DOT__dut__DOT__tile_inst__DOT__tregs[8U]) 
                                                                                & (0x16U 
                                                                                <= vlSelfRef.test_copper_lists__DOT__dut__DOT__tile_inst__DOT__tregs[9U]))) {
                                                                                vlSelfRef.test_copper_lists__DOT__dut__DOT__tile_inst__DOT____Vlvbound_ha768cd16__0 
                                                                                = vlSelfRef.test_copper_lists__DOT__dut__DOT__tile_inst__DOT__tregs[10U];
                                                                                __VdlyVal__test_copper_lists__DOT__dut__DOT__tile_inst__DOT__col_buffer__v22 
                                                                                = vlSelfRef.test_copper_lists__DOT__dut__DOT__tile_inst__DOT____Vlvbound_ha768cd16__0;
                                                                                __VdlySet__test_copper_lists__DOT__dut__DOT__tile_inst__DOT__col_buffer__v22 = 1U;
                                                                            }
                                                                            if (
                                                                                ((0x17U 
                                                                                >= vlSelfRef.test_copper_lists__DOT__dut__DOT__tile_inst__DOT__tregs[8U]) 
                                                                                & (0x17U 
                                                                                <= vlSelfRef.test_copper_lists__DOT__dut__DOT__tile_inst__DOT__tregs[9U]))) {
                                                                                vlSelfRef.test_copper_lists__DOT__dut__DOT__tile_inst__DOT____Vlvbound_ha768cd16__0 
                                                                                = vlSelfRef.test_copper_lists__DOT__dut__DOT__tile_inst__DOT__tregs[10U];
                                                                                __VdlyVal__test_copper_lists__DOT__dut__DOT__tile_inst__DOT__col_buffer__v23 
                                                                                = vlSelfRef.test_copper_lists__DOT__dut__DOT__tile_inst__DOT____Vlvbound_ha768cd16__0;
                                                                                __VdlySet__test_copper_lists__DOT__dut__DOT__tile_inst__DOT__col_buffer__v23 = 1U;
                                                                            }
                                                                            if (
                                                                                ((0x18U 
                                                                                >= vlSelfRef.test_copper_lists__DOT__dut__DOT__tile_inst__DOT__tregs[8U]) 
                                                                                & (0x18U 
                                                                                <= vlSelfRef.test_copper_lists__DOT__dut__DOT__tile_inst__DOT__tregs[9U]))) {
                                                                                vlSelfRef.test_copper_lists__DOT__dut__DOT__tile_inst__DOT____Vlvbound_ha768cd16__0 
                                                                                = vlSelfRef.test_copper_lists__DOT__dut__DOT__tile_inst__DOT__tregs[10U];
                                                                                __VdlyVal__test_copper_lists__DOT__dut__DOT__tile_inst__DOT__col_buffer__v24 
                                                                                = vlSelfRef.test_copper_lists__DOT__dut__DOT__tile_inst__DOT____Vlvbound_ha768cd16__0;
                                                                                __VdlySet__test_copper_lists__DOT__dut__DOT__tile_inst__DOT__col_buffer__v24 = 1U;
                                                                            }
                                                                        }
                                                                    } else if (
                                                                               (1U 
                                                                                & (IData)(vlSelfRef.test_copper_lists__DOT__cpu_wdata))) {
                                                                        if (
                                                                            (0x19U 
                                                                             > vlSelfRef.test_copper_lists__DOT__dut__DOT__tile_inst__DOT__tregs[8U])) {
                                                                            vlSelfRef.test_copper_lists__DOT__dut__DOT__tile_inst__DOT____Vlvbound_h67f0dfac__0 
                                                                                = vlSelfRef.test_copper_lists__DOT__dut__DOT__tile_inst__DOT__tregs[9U];
                                                                            if (
                                                                                (0x18U 
                                                                                >= 
                                                                                (0x0000001fU 
                                                                                & vlSelfRef.test_copper_lists__DOT__dut__DOT__tile_inst__DOT__tregs[8U]))) {
                                                                                __VdlyVal__test_copper_lists__DOT__dut__DOT__tile_inst__DOT__col_buffer__v25 
                                                                                = vlSelfRef.test_copper_lists__DOT__dut__DOT__tile_inst__DOT____Vlvbound_h67f0dfac__0;
                                                                                __VdlyDim0__test_copper_lists__DOT__dut__DOT__tile_inst__DOT__col_buffer__v25 
                                                                                = 
                                                                                (0x0000001fU 
                                                                                & vlSelfRef.test_copper_lists__DOT__dut__DOT__tile_inst__DOT__tregs[8U]);
                                                                                __VdlySet__test_copper_lists__DOT__dut__DOT__tile_inst__DOT__col_buffer__v25 = 1U;
                                                                            }
                                                                        }
                                                                    } else {
                                                                        vlSelfRef.test_copper_lists__DOT__dut__DOT__tile_inst__DOT____Vlvbound_hbbfa6b2d__0 
                                                                            = vlSelfRef.test_copper_lists__DOT__dut__DOT__tile_inst__DOT__tregs[8U];
                                                                        __VdlyVal__test_copper_lists__DOT__dut__DOT__tile_inst__DOT__col_buffer__v26 
                                                                            = vlSelfRef.test_copper_lists__DOT__dut__DOT__tile_inst__DOT____Vlvbound_hbbfa6b2d__0;
                                                                        __VdlySet__test_copper_lists__DOT__dut__DOT__tile_inst__DOT__col_buffer__v26 = 1U;
                                                                        vlSelfRef.test_copper_lists__DOT__dut__DOT__tile_inst__DOT____Vlvbound_hbbfa6b2d__0 
                                                                            = vlSelfRef.test_copper_lists__DOT__dut__DOT__tile_inst__DOT__tregs[8U];
                                                                        __VdlyVal__test_copper_lists__DOT__dut__DOT__tile_inst__DOT__col_buffer__v27 
                                                                            = vlSelfRef.test_copper_lists__DOT__dut__DOT__tile_inst__DOT____Vlvbound_hbbfa6b2d__0;
                                                                        __VdlySet__test_copper_lists__DOT__dut__DOT__tile_inst__DOT__col_buffer__v27 = 1U;
                                                                        vlSelfRef.test_copper_lists__DOT__dut__DOT__tile_inst__DOT____Vlvbound_hbbfa6b2d__0 
                                                                            = vlSelfRef.test_copper_lists__DOT__dut__DOT__tile_inst__DOT__tregs[8U];
                                                                        __VdlyVal__test_copper_lists__DOT__dut__DOT__tile_inst__DOT__col_buffer__v28 
                                                                            = vlSelfRef.test_copper_lists__DOT__dut__DOT__tile_inst__DOT____Vlvbound_hbbfa6b2d__0;
                                                                        __VdlySet__test_copper_lists__DOT__dut__DOT__tile_inst__DOT__col_buffer__v28 = 1U;
                                                                        vlSelfRef.test_copper_lists__DOT__dut__DOT__tile_inst__DOT____Vlvbound_hbbfa6b2d__0 
                                                                            = vlSelfRef.test_copper_lists__DOT__dut__DOT__tile_inst__DOT__tregs[8U];
                                                                        __VdlyVal__test_copper_lists__DOT__dut__DOT__tile_inst__DOT__col_buffer__v29 
                                                                            = vlSelfRef.test_copper_lists__DOT__dut__DOT__tile_inst__DOT____Vlvbound_hbbfa6b2d__0;
                                                                        __VdlySet__test_copper_lists__DOT__dut__DOT__tile_inst__DOT__col_buffer__v29 = 1U;
                                                                        vlSelfRef.test_copper_lists__DOT__dut__DOT__tile_inst__DOT____Vlvbound_hbbfa6b2d__0 
                                                                            = vlSelfRef.test_copper_lists__DOT__dut__DOT__tile_inst__DOT__tregs[8U];
                                                                        __VdlyVal__test_copper_lists__DOT__dut__DOT__tile_inst__DOT__col_buffer__v30 
                                                                            = vlSelfRef.test_copper_lists__DOT__dut__DOT__tile_inst__DOT____Vlvbound_hbbfa6b2d__0;
                                                                        __VdlySet__test_copper_lists__DOT__dut__DOT__tile_inst__DOT__col_buffer__v30 = 1U;
                                                                        vlSelfRef.test_copper_lists__DOT__dut__DOT__tile_inst__DOT____Vlvbound_hbbfa6b2d__0 
                                                                            = vlSelfRef.test_copper_lists__DOT__dut__DOT__tile_inst__DOT__tregs[8U];
                                                                        __VdlyVal__test_copper_lists__DOT__dut__DOT__tile_inst__DOT__col_buffer__v31 
                                                                            = vlSelfRef.test_copper_lists__DOT__dut__DOT__tile_inst__DOT____Vlvbound_hbbfa6b2d__0;
                                                                        __VdlySet__test_copper_lists__DOT__dut__DOT__tile_inst__DOT__col_buffer__v31 = 1U;
                                                                        vlSelfRef.test_copper_lists__DOT__dut__DOT__tile_inst__DOT____Vlvbound_hbbfa6b2d__0 
                                                                            = vlSelfRef.test_copper_lists__DOT__dut__DOT__tile_inst__DOT__tregs[8U];
                                                                        __VdlyVal__test_copper_lists__DOT__dut__DOT__tile_inst__DOT__col_buffer__v32 
                                                                            = vlSelfRef.test_copper_lists__DOT__dut__DOT__tile_inst__DOT____Vlvbound_hbbfa6b2d__0;
                                                                        __VdlySet__test_copper_lists__DOT__dut__DOT__tile_inst__DOT__col_buffer__v32 = 1U;
                                                                        vlSelfRef.test_copper_lists__DOT__dut__DOT__tile_inst__DOT____Vlvbound_hbbfa6b2d__0 
                                                                            = vlSelfRef.test_copper_lists__DOT__dut__DOT__tile_inst__DOT__tregs[8U];
                                                                        __VdlyVal__test_copper_lists__DOT__dut__DOT__tile_inst__DOT__col_buffer__v33 
                                                                            = vlSelfRef.test_copper_lists__DOT__dut__DOT__tile_inst__DOT____Vlvbound_hbbfa6b2d__0;
                                                                        __VdlySet__test_copper_lists__DOT__dut__DOT__tile_inst__DOT__col_buffer__v33 = 1U;
                                                                        vlSelfRef.test_copper_lists__DOT__dut__DOT__tile_inst__DOT____Vlvbound_hbbfa6b2d__0 
                                                                            = vlSelfRef.test_copper_lists__DOT__dut__DOT__tile_inst__DOT__tregs[8U];
                                                                        __VdlyVal__test_copper_lists__DOT__dut__DOT__tile_inst__DOT__col_buffer__v34 
                                                                            = vlSelfRef.test_copper_lists__DOT__dut__DOT__tile_inst__DOT____Vlvbound_hbbfa6b2d__0;
                                                                        __VdlySet__test_copper_lists__DOT__dut__DOT__tile_inst__DOT__col_buffer__v34 = 1U;
                                                                        vlSelfRef.test_copper_lists__DOT__dut__DOT__tile_inst__DOT____Vlvbound_hbbfa6b2d__0 
                                                                            = vlSelfRef.test_copper_lists__DOT__dut__DOT__tile_inst__DOT__tregs[8U];
                                                                        __VdlyVal__test_copper_lists__DOT__dut__DOT__tile_inst__DOT__col_buffer__v35 
                                                                            = vlSelfRef.test_copper_lists__DOT__dut__DOT__tile_inst__DOT____Vlvbound_hbbfa6b2d__0;
                                                                        __VdlySet__test_copper_lists__DOT__dut__DOT__tile_inst__DOT__col_buffer__v35 = 1U;
                                                                        vlSelfRef.test_copper_lists__DOT__dut__DOT__tile_inst__DOT____Vlvbound_hbbfa6b2d__0 
                                                                            = vlSelfRef.test_copper_lists__DOT__dut__DOT__tile_inst__DOT__tregs[8U];
                                                                        __VdlyVal__test_copper_lists__DOT__dut__DOT__tile_inst__DOT__col_buffer__v36 
                                                                            = vlSelfRef.test_copper_lists__DOT__dut__DOT__tile_inst__DOT____Vlvbound_hbbfa6b2d__0;
                                                                        __VdlySet__test_copper_lists__DOT__dut__DOT__tile_inst__DOT__col_buffer__v36 = 1U;
                                                                        vlSelfRef.test_copper_lists__DOT__dut__DOT__tile_inst__DOT____Vlvbound_hbbfa6b2d__0 
                                                                            = vlSelfRef.test_copper_lists__DOT__dut__DOT__tile_inst__DOT__tregs[8U];
                                                                        __VdlyVal__test_copper_lists__DOT__dut__DOT__tile_inst__DOT__col_buffer__v37 
                                                                            = vlSelfRef.test_copper_lists__DOT__dut__DOT__tile_inst__DOT____Vlvbound_hbbfa6b2d__0;
                                                                        __VdlySet__test_copper_lists__DOT__dut__DOT__tile_inst__DOT__col_buffer__v37 = 1U;
                                                                        vlSelfRef.test_copper_lists__DOT__dut__DOT__tile_inst__DOT____Vlvbound_hbbfa6b2d__0 
                                                                            = vlSelfRef.test_copper_lists__DOT__dut__DOT__tile_inst__DOT__tregs[8U];
                                                                        __VdlyVal__test_copper_lists__DOT__dut__DOT__tile_inst__DOT__col_buffer__v38 
                                                                            = vlSelfRef.test_copper_lists__DOT__dut__DOT__tile_inst__DOT____Vlvbound_hbbfa6b2d__0;
                                                                        __VdlySet__test_copper_lists__DOT__dut__DOT__tile_inst__DOT__col_buffer__v38 = 1U;
                                                                        vlSelfRef.test_copper_lists__DOT__dut__DOT__tile_inst__DOT____Vlvbound_hbbfa6b2d__0 
                                                                            = vlSelfRef.test_copper_lists__DOT__dut__DOT__tile_inst__DOT__tregs[8U];
                                                                        __VdlyVal__test_copper_lists__DOT__dut__DOT__tile_inst__DOT__col_buffer__v39 
                                                                            = vlSelfRef.test_copper_lists__DOT__dut__DOT__tile_inst__DOT____Vlvbound_hbbfa6b2d__0;
                                                                        __VdlySet__test_copper_lists__DOT__dut__DOT__tile_inst__DOT__col_buffer__v39 = 1U;
                                                                        vlSelfRef.test_copper_lists__DOT__dut__DOT__tile_inst__DOT____Vlvbound_hbbfa6b2d__0 
                                                                            = vlSelfRef.test_copper_lists__DOT__dut__DOT__tile_inst__DOT__tregs[8U];
                                                                        __VdlyVal__test_copper_lists__DOT__dut__DOT__tile_inst__DOT__col_buffer__v40 
                                                                            = vlSelfRef.test_copper_lists__DOT__dut__DOT__tile_inst__DOT____Vlvbound_hbbfa6b2d__0;
                                                                        __VdlySet__test_copper_lists__DOT__dut__DOT__tile_inst__DOT__col_buffer__v40 = 1U;
                                                                        vlSelfRef.test_copper_lists__DOT__dut__DOT__tile_inst__DOT____Vlvbound_hbbfa6b2d__0 
                                                                            = vlSelfRef.test_copper_lists__DOT__dut__DOT__tile_inst__DOT__tregs[8U];
                                                                        __VdlyVal__test_copper_lists__DOT__dut__DOT__tile_inst__DOT__col_buffer__v41 
                                                                            = vlSelfRef.test_copper_lists__DOT__dut__DOT__tile_inst__DOT____Vlvbound_hbbfa6b2d__0;
                                                                        __VdlySet__test_copper_lists__DOT__dut__DOT__tile_inst__DOT__col_buffer__v41 = 1U;
                                                                        vlSelfRef.test_copper_lists__DOT__dut__DOT__tile_inst__DOT____Vlvbound_hbbfa6b2d__0 
                                                                            = vlSelfRef.test_copper_lists__DOT__dut__DOT__tile_inst__DOT__tregs[8U];
                                                                        __VdlyVal__test_copper_lists__DOT__dut__DOT__tile_inst__DOT__col_buffer__v42 
                                                                            = vlSelfRef.test_copper_lists__DOT__dut__DOT__tile_inst__DOT____Vlvbound_hbbfa6b2d__0;
                                                                        __VdlySet__test_copper_lists__DOT__dut__DOT__tile_inst__DOT__col_buffer__v42 = 1U;
                                                                        vlSelfRef.test_copper_lists__DOT__dut__DOT__tile_inst__DOT____Vlvbound_hbbfa6b2d__0 
                                                                            = vlSelfRef.test_copper_lists__DOT__dut__DOT__tile_inst__DOT__tregs[8U];
                                                                        __VdlyVal__test_copper_lists__DOT__dut__DOT__tile_inst__DOT__col_buffer__v43 
                                                                            = vlSelfRef.test_copper_lists__DOT__dut__DOT__tile_inst__DOT____Vlvbound_hbbfa6b2d__0;
                                                                        __VdlySet__test_copper_lists__DOT__dut__DOT__tile_inst__DOT__col_buffer__v43 = 1U;
                                                                        vlSelfRef.test_copper_lists__DOT__dut__DOT__tile_inst__DOT____Vlvbound_hbbfa6b2d__0 
                                                                            = vlSelfRef.test_copper_lists__DOT__dut__DOT__tile_inst__DOT__tregs[8U];
                                                                        __VdlyVal__test_copper_lists__DOT__dut__DOT__tile_inst__DOT__col_buffer__v44 
                                                                            = vlSelfRef.test_copper_lists__DOT__dut__DOT__tile_inst__DOT____Vlvbound_hbbfa6b2d__0;
                                                                        __VdlySet__test_copper_lists__DOT__dut__DOT__tile_inst__DOT__col_buffer__v44 = 1U;
                                                                        vlSelfRef.test_copper_lists__DOT__dut__DOT__tile_inst__DOT____Vlvbound_hbbfa6b2d__0 
                                                                            = vlSelfRef.test_copper_lists__DOT__dut__DOT__tile_inst__DOT__tregs[8U];
                                                                        __VdlyVal__test_copper_lists__DOT__dut__DOT__tile_inst__DOT__col_buffer__v45 
                                                                            = vlSelfRef.test_copper_lists__DOT__dut__DOT__tile_inst__DOT____Vlvbound_hbbfa6b2d__0;
                                                                        __VdlySet__test_copper_lists__DOT__dut__DOT__tile_inst__DOT__col_buffer__v45 = 1U;
                                                                        vlSelfRef.test_copper_lists__DOT__dut__DOT__tile_inst__DOT____Vlvbound_hbbfa6b2d__0 
                                                                            = vlSelfRef.test_copper_lists__DOT__dut__DOT__tile_inst__DOT__tregs[8U];
                                                                        __VdlyVal__test_copper_lists__DOT__dut__DOT__tile_inst__DOT__col_buffer__v46 
                                                                            = vlSelfRef.test_copper_lists__DOT__dut__DOT__tile_inst__DOT____Vlvbound_hbbfa6b2d__0;
                                                                        __VdlySet__test_copper_lists__DOT__dut__DOT__tile_inst__DOT__col_buffer__v46 = 1U;
                                                                        vlSelfRef.test_copper_lists__DOT__dut__DOT__tile_inst__DOT____Vlvbound_hbbfa6b2d__0 
                                                                            = vlSelfRef.test_copper_lists__DOT__dut__DOT__tile_inst__DOT__tregs[8U];
                                                                        __VdlyVal__test_copper_lists__DOT__dut__DOT__tile_inst__DOT__col_buffer__v47 
                                                                            = vlSelfRef.test_copper_lists__DOT__dut__DOT__tile_inst__DOT____Vlvbound_hbbfa6b2d__0;
                                                                        __VdlySet__test_copper_lists__DOT__dut__DOT__tile_inst__DOT__col_buffer__v47 = 1U;
                                                                        vlSelfRef.test_copper_lists__DOT__dut__DOT__tile_inst__DOT____Vlvbound_hbbfa6b2d__0 
                                                                            = vlSelfRef.test_copper_lists__DOT__dut__DOT__tile_inst__DOT__tregs[8U];
                                                                        __VdlyVal__test_copper_lists__DOT__dut__DOT__tile_inst__DOT__col_buffer__v48 
                                                                            = vlSelfRef.test_copper_lists__DOT__dut__DOT__tile_inst__DOT____Vlvbound_hbbfa6b2d__0;
                                                                        __VdlySet__test_copper_lists__DOT__dut__DOT__tile_inst__DOT__col_buffer__v48 = 1U;
                                                                        vlSelfRef.test_copper_lists__DOT__dut__DOT__tile_inst__DOT____Vlvbound_hbbfa6b2d__0 
                                                                            = vlSelfRef.test_copper_lists__DOT__dut__DOT__tile_inst__DOT__tregs[8U];
                                                                        __VdlyVal__test_copper_lists__DOT__dut__DOT__tile_inst__DOT__col_buffer__v49 
                                                                            = vlSelfRef.test_copper_lists__DOT__dut__DOT__tile_inst__DOT____Vlvbound_hbbfa6b2d__0;
                                                                        __VdlySet__test_copper_lists__DOT__dut__DOT__tile_inst__DOT__col_buffer__v49 = 1U;
                                                                        vlSelfRef.test_copper_lists__DOT__dut__DOT__tile_inst__DOT____Vlvbound_hbbfa6b2d__0 
                                                                            = vlSelfRef.test_copper_lists__DOT__dut__DOT__tile_inst__DOT__tregs[8U];
                                                                        __VdlyVal__test_copper_lists__DOT__dut__DOT__tile_inst__DOT__col_buffer__v50 
                                                                            = vlSelfRef.test_copper_lists__DOT__dut__DOT__tile_inst__DOT____Vlvbound_hbbfa6b2d__0;
                                                                        __VdlySet__test_copper_lists__DOT__dut__DOT__tile_inst__DOT__col_buffer__v50 = 1U;
                                                                    }
                                                                }
                                                            }
                                                        } else if (
                                                                   (8U 
                                                                    & (IData)(vlSelfRef.test_copper_lists__DOT__cpu_wdata))) {
                                                            if (
                                                                (4U 
                                                                 & (IData)(vlSelfRef.test_copper_lists__DOT__cpu_wdata))) {
                                                                if (
                                                                    (2U 
                                                                     & (IData)(vlSelfRef.test_copper_lists__DOT__cpu_wdata))) {
                                                                    if (
                                                                        (1U 
                                                                         & (IData)(vlSelfRef.test_copper_lists__DOT__cpu_wdata))) {
                                                                        vlSelfRef.test_copper_lists__DOT__dut__DOT__tile_inst__DOT__dma_fill_addr = 0U;
                                                                        __Vdly__test_copper_lists__DOT__dut__DOT__tile_inst__DOT__dma_state = 9U;
                                                                    } else {
                                                                        __Vdly__test_copper_lists__DOT__dut__DOT__tile_inst__DOT__dma_src 
                                                                            = 
                                                                            ((vlSelfRef.test_copper_lists__DOT__dut__DOT__tile_inst__DOT__tregs[13U] 
                                                                              << 8U) 
                                                                             | vlSelfRef.test_copper_lists__DOT__dut__DOT__tile_inst__DOT__tregs[12U]);
                                                                        vlSelfRef.test_copper_lists__DOT__dut__DOT__tile_inst__DOT__dma_nt 
                                                                            = 
                                                                            (3U 
                                                                             & vlSelfRef.test_copper_lists__DOT__dut__DOT__tile_inst__DOT__tregs[8U]);
                                                                        vlSelfRef.test_copper_lists__DOT__dut__DOT__tile_inst__DOT__dma_col_row 
                                                                            = 
                                                                            (0x0000003fU 
                                                                             & vlSelfRef.test_copper_lists__DOT__dut__DOT__tile_inst__DOT__tregs[9U]);
                                                                        __Vdly__test_copper_lists__DOT__dut__DOT__tile_inst__DOT__dma_col_idx = 0U;
                                                                        __Vdly__test_copper_lists__DOT__dut__DOT__tile_inst__DOT__dma_count = 0x0019U;
                                                                        __Vdly__test_copper_lists__DOT__dut__DOT__tile_inst__DOT__dma_state = 8U;
                                                                        __Vdly__test_copper_lists__DOT__dut__DOT__tile_inst__DOT__dma_data_valid = 0U;
                                                                    }
                                                                } else if (
                                                                           (1U 
                                                                            & (IData)(vlSelfRef.test_copper_lists__DOT__cpu_wdata))) {
                                                                    __Vdly__test_copper_lists__DOT__dut__DOT__tile_inst__DOT__dma_src 
                                                                        = 
                                                                        ((vlSelfRef.test_copper_lists__DOT__dut__DOT__tile_inst__DOT__tregs[13U] 
                                                                          << 8U) 
                                                                         | vlSelfRef.test_copper_lists__DOT__dut__DOT__tile_inst__DOT__tregs[12U]);
                                                                    __Vdly__test_copper_lists__DOT__dut__DOT__tile_inst__DOT__dma_dst = 0U;
                                                                    vlSelfRef.test_copper_lists__DOT__dut__DOT__tile_inst__DOT__dma_nt 
                                                                        = 
                                                                        (3U 
                                                                         & vlSelfRef.test_copper_lists__DOT__dut__DOT__tile_inst__DOT__tregs[8U]);
                                                                    vlSelfRef.test_copper_lists__DOT__dut__DOT__tile_inst__DOT__dma_col_row 
                                                                        = 
                                                                        (0x0000003fU 
                                                                         & vlSelfRef.test_copper_lists__DOT__dut__DOT__tile_inst__DOT__tregs[9U]);
                                                                    __Vdly__test_copper_lists__DOT__dut__DOT__tile_inst__DOT__dma_count = 0x0028U;
                                                                    __Vdly__test_copper_lists__DOT__dut__DOT__tile_inst__DOT__dma_state = 7U;
                                                                    __Vdly__test_copper_lists__DOT__dut__DOT__tile_inst__DOT__dma_data_valid = 0U;
                                                                } else if (
                                                                           (((4U 
                                                                              > vlSelfRef.test_copper_lists__DOT__dut__DOT__tile_inst__DOT__tregs[8U]) 
                                                                             & (0x28U 
                                                                                > vlSelfRef.test_copper_lists__DOT__dut__DOT__tile_inst__DOT__tregs[9U])) 
                                                                            & (0x19U 
                                                                               > vlSelfRef.test_copper_lists__DOT__dut__DOT__tile_inst__DOT__tregs[10U]))) {
                                                                    vlSelfRef.test_copper_lists__DOT__dut__DOT__tile_inst__DOT__cmd_nt_addr 
                                                                        = 
                                                                        (0x00000fffU 
                                                                         & ((((IData)(0x000003e8U) 
                                                                              * vlSelfRef.test_copper_lists__DOT__dut__DOT__tile_inst__DOT__tregs[8U]) 
                                                                             + 
                                                                             ((IData)(0x00000028U) 
                                                                              * vlSelfRef.test_copper_lists__DOT__dut__DOT__tile_inst__DOT__tregs[10U])) 
                                                                            + vlSelfRef.test_copper_lists__DOT__dut__DOT__tile_inst__DOT__tregs[9U]));
                                                                    vlSelfRef.test_copper_lists__DOT__dut__DOT__tile_inst__DOT__cmd_at_addr 
                                                                        = 
                                                                        (0x00000fffU 
                                                                         & ((((IData)(0x000003e8U) 
                                                                              * vlSelfRef.test_copper_lists__DOT__dut__DOT__tile_inst__DOT__tregs[8U]) 
                                                                             + 
                                                                             ((IData)(0x00000028U) 
                                                                              * vlSelfRef.test_copper_lists__DOT__dut__DOT__tile_inst__DOT__tregs[10U])) 
                                                                            + vlSelfRef.test_copper_lists__DOT__dut__DOT__tile_inst__DOT__tregs[9U]));
                                                                    __Vdly__test_copper_lists__DOT__dut__DOT__tile_inst__DOT__dma_state = 0x0cU;
                                                                }
                                                            } else if (
                                                                       (2U 
                                                                        & (IData)(vlSelfRef.test_copper_lists__DOT__cpu_wdata))) {
                                                                if (
                                                                    (1U 
                                                                     & (~ (IData)(vlSelfRef.test_copper_lists__DOT__cpu_wdata)))) {
                                                                    __Vdly__test_copper_lists__DOT__dut__DOT__tile_inst__DOT__dma_src 
                                                                        = 
                                                                        ((vlSelfRef.test_copper_lists__DOT__dut__DOT__tile_inst__DOT__tregs[13U] 
                                                                          << 8U) 
                                                                         | vlSelfRef.test_copper_lists__DOT__dut__DOT__tile_inst__DOT__tregs[12U]);
                                                                    vlSelfRef.test_copper_lists__DOT__dut__DOT__tile_inst__DOT__dma_pal_cidx = 0U;
                                                                    __Vdly__test_copper_lists__DOT__dut__DOT__tile_inst__DOT__dma_pal_phase = 0U;
                                                                    __Vdly__test_copper_lists__DOT__dut__DOT__tile_inst__DOT__dma_count = 0x0030U;
                                                                    __Vdly__test_copper_lists__DOT__dut__DOT__tile_inst__DOT__dma_state = 4U;
                                                                    __Vdly__test_copper_lists__DOT__dut__DOT__tile_inst__DOT__dma_data_valid = 0U;
                                                                }
                                                            } else if (
                                                                       (1U 
                                                                        & (IData)(vlSelfRef.test_copper_lists__DOT__cpu_wdata))) {
                                                                __Vdly__test_copper_lists__DOT__dut__DOT__tile_inst__DOT__dma_src 
                                                                    = 
                                                                    ((vlSelfRef.test_copper_lists__DOT__dut__DOT__tile_inst__DOT__tregs[13U] 
                                                                      << 8U) 
                                                                     | vlSelfRef.test_copper_lists__DOT__dut__DOT__tile_inst__DOT__tregs[12U]);
                                                                __Vdly__test_copper_lists__DOT__dut__DOT__tile_inst__DOT__dma_dst = 0U;
                                                                vlSelfRef.test_copper_lists__DOT__dut__DOT__tile_inst__DOT__dma_nt 
                                                                    = 
                                                                    (3U 
                                                                     & vlSelfRef.test_copper_lists__DOT__dut__DOT__tile_inst__DOT__tregs[8U]);
                                                                __Vdly__test_copper_lists__DOT__dut__DOT__tile_inst__DOT__dma_count = 0x03e8U;
                                                                __Vdly__test_copper_lists__DOT__dut__DOT__tile_inst__DOT__dma_state = 3U;
                                                                __Vdly__test_copper_lists__DOT__dut__DOT__tile_inst__DOT__dma_data_valid = 0U;
                                                            } else {
                                                                __Vdly__test_copper_lists__DOT__dut__DOT__tile_inst__DOT__dma_src 
                                                                    = 
                                                                    ((vlSelfRef.test_copper_lists__DOT__dut__DOT__tile_inst__DOT__tregs[13U] 
                                                                      << 8U) 
                                                                     | vlSelfRef.test_copper_lists__DOT__dut__DOT__tile_inst__DOT__tregs[12U]);
                                                                __Vdly__test_copper_lists__DOT__dut__DOT__tile_inst__DOT__dma_dst = 0U;
                                                                vlSelfRef.test_copper_lists__DOT__dut__DOT__tile_inst__DOT__dma_nt 
                                                                    = 
                                                                    (3U 
                                                                     & vlSelfRef.test_copper_lists__DOT__dut__DOT__tile_inst__DOT__tregs[8U]);
                                                                __Vdly__test_copper_lists__DOT__dut__DOT__tile_inst__DOT__dma_count = 0x03e8U;
                                                                __Vdly__test_copper_lists__DOT__dut__DOT__tile_inst__DOT__dma_state = 2U;
                                                                __Vdly__test_copper_lists__DOT__dut__DOT__tile_inst__DOT__dma_data_valid = 0U;
                                                            }
                                                        } else if (
                                                                   (4U 
                                                                    & (IData)(vlSelfRef.test_copper_lists__DOT__cpu_wdata))) {
                                                            if (
                                                                (2U 
                                                                 & (IData)(vlSelfRef.test_copper_lists__DOT__cpu_wdata))) {
                                                                if (
                                                                    (1U 
                                                                     & (IData)(vlSelfRef.test_copper_lists__DOT__cpu_wdata))) {
                                                                    __Vdly__test_copper_lists__DOT__dut__DOT__tile_inst__DOT__dma_src 
                                                                        = 
                                                                        ((vlSelfRef.test_copper_lists__DOT__dut__DOT__tile_inst__DOT__tregs[13U] 
                                                                          << 8U) 
                                                                         | vlSelfRef.test_copper_lists__DOT__dut__DOT__tile_inst__DOT__tregs[12U]);
                                                                    vlSelfRef.test_copper_lists__DOT__dut__DOT__tile_inst__DOT__dma_nt 
                                                                        = 
                                                                        (3U 
                                                                         & vlSelfRef.test_copper_lists__DOT__dut__DOT__tile_inst__DOT__tregs[8U]);
                                                                    vlSelfRef.test_copper_lists__DOT__dut__DOT__tile_inst__DOT__dma_col_row 
                                                                        = 
                                                                        (0x0000003fU 
                                                                         & vlSelfRef.test_copper_lists__DOT__dut__DOT__tile_inst__DOT__tregs[9U]);
                                                                    __Vdly__test_copper_lists__DOT__dut__DOT__tile_inst__DOT__dma_col_idx = 0U;
                                                                    __Vdly__test_copper_lists__DOT__dut__DOT__tile_inst__DOT__dma_count = 0x0019U;
                                                                    __Vdly__test_copper_lists__DOT__dut__DOT__tile_inst__DOT__dma_state = 6U;
                                                                    __Vdly__test_copper_lists__DOT__dut__DOT__tile_inst__DOT__dma_data_valid = 0U;
                                                                } else {
                                                                    __Vdly__test_copper_lists__DOT__dut__DOT__tile_inst__DOT__dma_src 
                                                                        = 
                                                                        ((vlSelfRef.test_copper_lists__DOT__dut__DOT__tile_inst__DOT__tregs[13U] 
                                                                          << 8U) 
                                                                         | vlSelfRef.test_copper_lists__DOT__dut__DOT__tile_inst__DOT__tregs[12U]);
                                                                    __Vdly__test_copper_lists__DOT__dut__DOT__tile_inst__DOT__dma_dst = 0U;
                                                                    vlSelfRef.test_copper_lists__DOT__dut__DOT__tile_inst__DOT__dma_nt 
                                                                        = 
                                                                        (3U 
                                                                         & vlSelfRef.test_copper_lists__DOT__dut__DOT__tile_inst__DOT__tregs[8U]);
                                                                    vlSelfRef.test_copper_lists__DOT__dut__DOT__tile_inst__DOT__dma_col_row 
                                                                        = 
                                                                        (0x0000003fU 
                                                                         & vlSelfRef.test_copper_lists__DOT__dut__DOT__tile_inst__DOT__tregs[9U]);
                                                                    __Vdly__test_copper_lists__DOT__dut__DOT__tile_inst__DOT__dma_count = 0x0028U;
                                                                    __Vdly__test_copper_lists__DOT__dut__DOT__tile_inst__DOT__dma_state = 5U;
                                                                    __Vdly__test_copper_lists__DOT__dut__DOT__tile_inst__DOT__dma_data_valid = 0U;
                                                                }
                                                            } else if (
                                                                       (1U 
                                                                        & (IData)(vlSelfRef.test_copper_lists__DOT__cpu_wdata))) {
                                                                if (
                                                                    (4U 
                                                                     > vlSelfRef.test_copper_lists__DOT__dut__DOT__tile_inst__DOT__tregs[8U])) {
                                                                    vlSelfRef.test_copper_lists__DOT__dut__DOT__tile_inst__DOT__dma_nt 
                                                                        = 
                                                                        (3U 
                                                                         & vlSelfRef.test_copper_lists__DOT__dut__DOT__tile_inst__DOT__tregs[8U]);
                                                                    vlSelfRef.test_copper_lists__DOT__dut__DOT__tile_inst__DOT__dma_fill_val 
                                                                        = vlSelfRef.test_copper_lists__DOT__dut__DOT__tile_inst__DOT__tregs[9U];
                                                                    __Vdly__test_copper_lists__DOT__dut__DOT__tile_inst__DOT__dma_count = 0U;
                                                                    __Vdly__test_copper_lists__DOT__dut__DOT__tile_inst__DOT__dma_state = 0x0aU;
                                                                }
                                                            } else if (
                                                                       (((4U 
                                                                          > vlSelfRef.test_copper_lists__DOT__dut__DOT__tile_inst__DOT__tregs[8U]) 
                                                                         & (0x28U 
                                                                            > vlSelfRef.test_copper_lists__DOT__dut__DOT__tile_inst__DOT__tregs[9U])) 
                                                                        & (0x19U 
                                                                           > vlSelfRef.test_copper_lists__DOT__dut__DOT__tile_inst__DOT__tregs[10U]))) {
                                                                vlSelfRef.test_copper_lists__DOT__dut__DOT__tile_inst__DOT__cmd_at_addr 
                                                                    = 
                                                                    (0x00000fffU 
                                                                     & ((((IData)(0x000003e8U) 
                                                                          * vlSelfRef.test_copper_lists__DOT__dut__DOT__tile_inst__DOT__tregs[8U]) 
                                                                         + 
                                                                         ((IData)(0x00000028U) 
                                                                          * vlSelfRef.test_copper_lists__DOT__dut__DOT__tile_inst__DOT__tregs[10U])) 
                                                                        + vlSelfRef.test_copper_lists__DOT__dut__DOT__tile_inst__DOT__tregs[9U]));
                                                                vlSelfRef.test_copper_lists__DOT__dut__DOT__tile_inst__DOT__cmd_at_din 
                                                                    = vlSelfRef.test_copper_lists__DOT__dut__DOT__tile_inst__DOT__tregs[11U];
                                                                vlSelfRef.test_copper_lists__DOT__dut__DOT__tile_inst__DOT__cmd_at_we = 1U;
                                                            }
                                                        } else if (
                                                                   (2U 
                                                                    & (IData)(vlSelfRef.test_copper_lists__DOT__cpu_wdata))) {
                                                            if (
                                                                (1U 
                                                                 & (IData)(vlSelfRef.test_copper_lists__DOT__cpu_wdata))) {
                                                                if (
                                                                    (((4U 
                                                                       > vlSelfRef.test_copper_lists__DOT__dut__DOT__tile_inst__DOT__tregs[8U]) 
                                                                      & (0x28U 
                                                                         > vlSelfRef.test_copper_lists__DOT__dut__DOT__tile_inst__DOT__tregs[9U])) 
                                                                     & (0x19U 
                                                                        > vlSelfRef.test_copper_lists__DOT__dut__DOT__tile_inst__DOT__tregs[10U]))) {
                                                                    vlSelfRef.test_copper_lists__DOT__dut__DOT__tile_inst__DOT__cmd_nt_addr 
                                                                        = 
                                                                        (0x00000fffU 
                                                                         & ((((IData)(0x000003e8U) 
                                                                              * vlSelfRef.test_copper_lists__DOT__dut__DOT__tile_inst__DOT__tregs[8U]) 
                                                                             + 
                                                                             ((IData)(0x00000028U) 
                                                                              * vlSelfRef.test_copper_lists__DOT__dut__DOT__tile_inst__DOT__tregs[10U])) 
                                                                            + vlSelfRef.test_copper_lists__DOT__dut__DOT__tile_inst__DOT__tregs[9U]));
                                                                    vlSelfRef.test_copper_lists__DOT__dut__DOT__tile_inst__DOT__cmd_nt_din 
                                                                        = vlSelfRef.test_copper_lists__DOT__dut__DOT__tile_inst__DOT__tregs[11U];
                                                                    vlSelfRef.test_copper_lists__DOT__dut__DOT__tile_inst__DOT__cmd_nt_we = 1U;
                                                                }
                                                            } else {
                                                                __Vdly__test_copper_lists__DOT__dut__DOT__tile_inst__DOT__dma_src 
                                                                    = 
                                                                    ((vlSelfRef.test_copper_lists__DOT__dut__DOT__tile_inst__DOT__tregs[13U] 
                                                                      << 8U) 
                                                                     | vlSelfRef.test_copper_lists__DOT__dut__DOT__tile_inst__DOT__tregs[12U]);
                                                                if (vlSelfRef.test_copper_lists__DOT__dut__DOT__tile_inst__DOT__tile_size16) {
                                                                    __Vdly__test_copper_lists__DOT__dut__DOT__tile_inst__DOT__dma_dst 
                                                                        = 
                                                                        (vlSelfRef.test_copper_lists__DOT__dut__DOT__tile_inst__DOT__tregs[8U] 
                                                                         << 7U);
                                                                    __Vdly__test_copper_lists__DOT__dut__DOT__tile_inst__DOT__dma_count 
                                                                        = 
                                                                        (0x0000ffffU 
                                                                         & VL_SHIFTL_III(32,32,32, vlSelfRef.test_copper_lists__DOT__dut__DOT__tile_inst__DOT__tregs[9U], 7U));
                                                                } else {
                                                                    __Vdly__test_copper_lists__DOT__dut__DOT__tile_inst__DOT__dma_dst 
                                                                        = 
                                                                        (vlSelfRef.test_copper_lists__DOT__dut__DOT__tile_inst__DOT__tregs[8U] 
                                                                         << 5U);
                                                                    __Vdly__test_copper_lists__DOT__dut__DOT__tile_inst__DOT__dma_count 
                                                                        = 
                                                                        (0x0000ffffU 
                                                                         & VL_SHIFTL_III(32,32,32, vlSelfRef.test_copper_lists__DOT__dut__DOT__tile_inst__DOT__tregs[9U], 5U));
                                                                }
                                                                __Vdly__test_copper_lists__DOT__dut__DOT__tile_inst__DOT__dma_state = 1U;
                                                                __Vdly__test_copper_lists__DOT__dut__DOT__tile_inst__DOT__dma_data_valid = 0U;
                                                            }
                                                        } else if (
                                                                   (1U 
                                                                    & (IData)(vlSelfRef.test_copper_lists__DOT__cpu_wdata))) {
                                                            __Vdly__test_copper_lists__DOT__dut__DOT__tile_inst__DOT__dma_src 
                                                                = 
                                                                ((vlSelfRef.test_copper_lists__DOT__dut__DOT__tile_inst__DOT__tregs[13U] 
                                                                  << 8U) 
                                                                 | vlSelfRef.test_copper_lists__DOT__dut__DOT__tile_inst__DOT__tregs[12U]);
                                                            if (vlSelfRef.test_copper_lists__DOT__dut__DOT__tile_inst__DOT__tile_size16) {
                                                                __Vdly__test_copper_lists__DOT__dut__DOT__tile_inst__DOT__dma_dst 
                                                                    = 
                                                                    (vlSelfRef.test_copper_lists__DOT__dut__DOT__tile_inst__DOT__tregs[8U] 
                                                                     << 7U);
                                                                __Vdly__test_copper_lists__DOT__dut__DOT__tile_inst__DOT__dma_count = 0x0080U;
                                                            } else {
                                                                __Vdly__test_copper_lists__DOT__dut__DOT__tile_inst__DOT__dma_dst 
                                                                    = 
                                                                    (vlSelfRef.test_copper_lists__DOT__dut__DOT__tile_inst__DOT__tregs[8U] 
                                                                     << 5U);
                                                                __Vdly__test_copper_lists__DOT__dut__DOT__tile_inst__DOT__dma_count = 0x0020U;
                                                            }
                                                            __Vdly__test_copper_lists__DOT__dut__DOT__tile_inst__DOT__dma_state = 1U;
                                                            __Vdly__test_copper_lists__DOT__dut__DOT__tile_inst__DOT__dma_data_valid = 0U;
                                                        }
                                                    }
                                                }
                                            }
                                        }
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }
    }
    vlSelfRef.test_copper_lists__DOT__dut__DOT__tile_inst__DOT__dma_src 
        = __Vdly__test_copper_lists__DOT__dut__DOT__tile_inst__DOT__dma_src;
    vlSelfRef.test_copper_lists__DOT__dut__DOT__tile_inst__DOT__dma_pal_phase 
        = __Vdly__test_copper_lists__DOT__dut__DOT__tile_inst__DOT__dma_pal_phase;
    if (__VdlySet__test_copper_lists__DOT__dut__DOT__tile_inst__DOT__tregs__v0) {
        vlSelfRef.test_copper_lists__DOT__dut__DOT__tile_inst__DOT__tregs[__VdlyDim0__test_copper_lists__DOT__dut__DOT__tile_inst__DOT__tregs__v0] 
            = __VdlyVal__test_copper_lists__DOT__dut__DOT__tile_inst__DOT__tregs__v0;
    }
    vlSelfRef.test_copper_lists__DOT__dut__DOT__tile_inst__DOT__dma_count 
        = __Vdly__test_copper_lists__DOT__dut__DOT__tile_inst__DOT__dma_count;
    vlSelfRef.test_copper_lists__DOT__dut__DOT__tile_inst__DOT__dma_dst 
        = __Vdly__test_copper_lists__DOT__dut__DOT__tile_inst__DOT__dma_dst;
    vlSelfRef.test_copper_lists__DOT__dut__DOT__tile_inst__DOT__dma_col_idx 
        = __Vdly__test_copper_lists__DOT__dut__DOT__tile_inst__DOT__dma_col_idx;
    vlSelfRef.test_copper_lists__DOT__dut__DOT__tile_inst__DOT__dma_data_valid 
        = __Vdly__test_copper_lists__DOT__dut__DOT__tile_inst__DOT__dma_data_valid;
    if (__VdlySet__test_copper_lists__DOT__dut__DOT__tile_inst__DOT__col_buffer__v0) {
        vlSelfRef.test_copper_lists__DOT__dut__DOT__tile_inst__DOT__col_buffer[0U] 
            = __VdlyVal__test_copper_lists__DOT__dut__DOT__tile_inst__DOT__col_buffer__v0;
    }
    if (__VdlySet__test_copper_lists__DOT__dut__DOT__tile_inst__DOT__col_buffer__v1) {
        vlSelfRef.test_copper_lists__DOT__dut__DOT__tile_inst__DOT__col_buffer[1U] 
            = __VdlyVal__test_copper_lists__DOT__dut__DOT__tile_inst__DOT__col_buffer__v1;
    }
    if (__VdlySet__test_copper_lists__DOT__dut__DOT__tile_inst__DOT__col_buffer__v2) {
        vlSelfRef.test_copper_lists__DOT__dut__DOT__tile_inst__DOT__col_buffer[2U] 
            = __VdlyVal__test_copper_lists__DOT__dut__DOT__tile_inst__DOT__col_buffer__v2;
    }
    if (__VdlySet__test_copper_lists__DOT__dut__DOT__tile_inst__DOT__col_buffer__v3) {
        vlSelfRef.test_copper_lists__DOT__dut__DOT__tile_inst__DOT__col_buffer[3U] 
            = __VdlyVal__test_copper_lists__DOT__dut__DOT__tile_inst__DOT__col_buffer__v3;
    }
    if (__VdlySet__test_copper_lists__DOT__dut__DOT__tile_inst__DOT__col_buffer__v4) {
        vlSelfRef.test_copper_lists__DOT__dut__DOT__tile_inst__DOT__col_buffer[4U] 
            = __VdlyVal__test_copper_lists__DOT__dut__DOT__tile_inst__DOT__col_buffer__v4;
    }
    if (__VdlySet__test_copper_lists__DOT__dut__DOT__tile_inst__DOT__col_buffer__v5) {
        vlSelfRef.test_copper_lists__DOT__dut__DOT__tile_inst__DOT__col_buffer[5U] 
            = __VdlyVal__test_copper_lists__DOT__dut__DOT__tile_inst__DOT__col_buffer__v5;
    }
    if (__VdlySet__test_copper_lists__DOT__dut__DOT__tile_inst__DOT__col_buffer__v6) {
        vlSelfRef.test_copper_lists__DOT__dut__DOT__tile_inst__DOT__col_buffer[6U] 
            = __VdlyVal__test_copper_lists__DOT__dut__DOT__tile_inst__DOT__col_buffer__v6;
    }
    if (__VdlySet__test_copper_lists__DOT__dut__DOT__tile_inst__DOT__col_buffer__v7) {
        vlSelfRef.test_copper_lists__DOT__dut__DOT__tile_inst__DOT__col_buffer[7U] 
            = __VdlyVal__test_copper_lists__DOT__dut__DOT__tile_inst__DOT__col_buffer__v7;
    }
    if (__VdlySet__test_copper_lists__DOT__dut__DOT__tile_inst__DOT__col_buffer__v8) {
        vlSelfRef.test_copper_lists__DOT__dut__DOT__tile_inst__DOT__col_buffer[8U] 
            = __VdlyVal__test_copper_lists__DOT__dut__DOT__tile_inst__DOT__col_buffer__v8;
    }
    if (__VdlySet__test_copper_lists__DOT__dut__DOT__tile_inst__DOT__col_buffer__v9) {
        vlSelfRef.test_copper_lists__DOT__dut__DOT__tile_inst__DOT__col_buffer[9U] 
            = __VdlyVal__test_copper_lists__DOT__dut__DOT__tile_inst__DOT__col_buffer__v9;
    }
    if (__VdlySet__test_copper_lists__DOT__dut__DOT__tile_inst__DOT__col_buffer__v10) {
        vlSelfRef.test_copper_lists__DOT__dut__DOT__tile_inst__DOT__col_buffer[10U] 
            = __VdlyVal__test_copper_lists__DOT__dut__DOT__tile_inst__DOT__col_buffer__v10;
    }
    if (__VdlySet__test_copper_lists__DOT__dut__DOT__tile_inst__DOT__col_buffer__v11) {
        vlSelfRef.test_copper_lists__DOT__dut__DOT__tile_inst__DOT__col_buffer[11U] 
            = __VdlyVal__test_copper_lists__DOT__dut__DOT__tile_inst__DOT__col_buffer__v11;
    }
    if (__VdlySet__test_copper_lists__DOT__dut__DOT__tile_inst__DOT__col_buffer__v12) {
        vlSelfRef.test_copper_lists__DOT__dut__DOT__tile_inst__DOT__col_buffer[12U] 
            = __VdlyVal__test_copper_lists__DOT__dut__DOT__tile_inst__DOT__col_buffer__v12;
    }
    if (__VdlySet__test_copper_lists__DOT__dut__DOT__tile_inst__DOT__col_buffer__v13) {
        vlSelfRef.test_copper_lists__DOT__dut__DOT__tile_inst__DOT__col_buffer[13U] 
            = __VdlyVal__test_copper_lists__DOT__dut__DOT__tile_inst__DOT__col_buffer__v13;
    }
    if (__VdlySet__test_copper_lists__DOT__dut__DOT__tile_inst__DOT__col_buffer__v14) {
        vlSelfRef.test_copper_lists__DOT__dut__DOT__tile_inst__DOT__col_buffer[14U] 
            = __VdlyVal__test_copper_lists__DOT__dut__DOT__tile_inst__DOT__col_buffer__v14;
    }
    if (__VdlySet__test_copper_lists__DOT__dut__DOT__tile_inst__DOT__col_buffer__v15) {
        vlSelfRef.test_copper_lists__DOT__dut__DOT__tile_inst__DOT__col_buffer[15U] 
            = __VdlyVal__test_copper_lists__DOT__dut__DOT__tile_inst__DOT__col_buffer__v15;
    }
    if (__VdlySet__test_copper_lists__DOT__dut__DOT__tile_inst__DOT__col_buffer__v16) {
        vlSelfRef.test_copper_lists__DOT__dut__DOT__tile_inst__DOT__col_buffer[16U] 
            = __VdlyVal__test_copper_lists__DOT__dut__DOT__tile_inst__DOT__col_buffer__v16;
    }
    if (__VdlySet__test_copper_lists__DOT__dut__DOT__tile_inst__DOT__col_buffer__v17) {
        vlSelfRef.test_copper_lists__DOT__dut__DOT__tile_inst__DOT__col_buffer[17U] 
            = __VdlyVal__test_copper_lists__DOT__dut__DOT__tile_inst__DOT__col_buffer__v17;
    }
    if (__VdlySet__test_copper_lists__DOT__dut__DOT__tile_inst__DOT__col_buffer__v18) {
        vlSelfRef.test_copper_lists__DOT__dut__DOT__tile_inst__DOT__col_buffer[18U] 
            = __VdlyVal__test_copper_lists__DOT__dut__DOT__tile_inst__DOT__col_buffer__v18;
    }
    if (__VdlySet__test_copper_lists__DOT__dut__DOT__tile_inst__DOT__col_buffer__v19) {
        vlSelfRef.test_copper_lists__DOT__dut__DOT__tile_inst__DOT__col_buffer[19U] 
            = __VdlyVal__test_copper_lists__DOT__dut__DOT__tile_inst__DOT__col_buffer__v19;
    }
    if (__VdlySet__test_copper_lists__DOT__dut__DOT__tile_inst__DOT__col_buffer__v20) {
        vlSelfRef.test_copper_lists__DOT__dut__DOT__tile_inst__DOT__col_buffer[20U] 
            = __VdlyVal__test_copper_lists__DOT__dut__DOT__tile_inst__DOT__col_buffer__v20;
    }
    if (__VdlySet__test_copper_lists__DOT__dut__DOT__tile_inst__DOT__col_buffer__v21) {
        vlSelfRef.test_copper_lists__DOT__dut__DOT__tile_inst__DOT__col_buffer[21U] 
            = __VdlyVal__test_copper_lists__DOT__dut__DOT__tile_inst__DOT__col_buffer__v21;
    }
    if (__VdlySet__test_copper_lists__DOT__dut__DOT__tile_inst__DOT__col_buffer__v22) {
        vlSelfRef.test_copper_lists__DOT__dut__DOT__tile_inst__DOT__col_buffer[22U] 
            = __VdlyVal__test_copper_lists__DOT__dut__DOT__tile_inst__DOT__col_buffer__v22;
    }
    if (__VdlySet__test_copper_lists__DOT__dut__DOT__tile_inst__DOT__col_buffer__v23) {
        vlSelfRef.test_copper_lists__DOT__dut__DOT__tile_inst__DOT__col_buffer[23U] 
            = __VdlyVal__test_copper_lists__DOT__dut__DOT__tile_inst__DOT__col_buffer__v23;
    }
    if (__VdlySet__test_copper_lists__DOT__dut__DOT__tile_inst__DOT__col_buffer__v24) {
        vlSelfRef.test_copper_lists__DOT__dut__DOT__tile_inst__DOT__col_buffer[24U] 
            = __VdlyVal__test_copper_lists__DOT__dut__DOT__tile_inst__DOT__col_buffer__v24;
    }
    if (__VdlySet__test_copper_lists__DOT__dut__DOT__tile_inst__DOT__col_buffer__v25) {
        vlSelfRef.test_copper_lists__DOT__dut__DOT__tile_inst__DOT__col_buffer[__VdlyDim0__test_copper_lists__DOT__dut__DOT__tile_inst__DOT__col_buffer__v25] 
            = __VdlyVal__test_copper_lists__DOT__dut__DOT__tile_inst__DOT__col_buffer__v25;
    }
    if (__VdlySet__test_copper_lists__DOT__dut__DOT__tile_inst__DOT__col_buffer__v26) {
        vlSelfRef.test_copper_lists__DOT__dut__DOT__tile_inst__DOT__col_buffer[0U] 
            = __VdlyVal__test_copper_lists__DOT__dut__DOT__tile_inst__DOT__col_buffer__v26;
    }
    if (__VdlySet__test_copper_lists__DOT__dut__DOT__tile_inst__DOT__col_buffer__v27) {
        vlSelfRef.test_copper_lists__DOT__dut__DOT__tile_inst__DOT__col_buffer[1U] 
            = __VdlyVal__test_copper_lists__DOT__dut__DOT__tile_inst__DOT__col_buffer__v27;
    }
    if (__VdlySet__test_copper_lists__DOT__dut__DOT__tile_inst__DOT__col_buffer__v28) {
        vlSelfRef.test_copper_lists__DOT__dut__DOT__tile_inst__DOT__col_buffer[2U] 
            = __VdlyVal__test_copper_lists__DOT__dut__DOT__tile_inst__DOT__col_buffer__v28;
    }
    if (__VdlySet__test_copper_lists__DOT__dut__DOT__tile_inst__DOT__col_buffer__v29) {
        vlSelfRef.test_copper_lists__DOT__dut__DOT__tile_inst__DOT__col_buffer[3U] 
            = __VdlyVal__test_copper_lists__DOT__dut__DOT__tile_inst__DOT__col_buffer__v29;
    }
    if (__VdlySet__test_copper_lists__DOT__dut__DOT__tile_inst__DOT__col_buffer__v30) {
        vlSelfRef.test_copper_lists__DOT__dut__DOT__tile_inst__DOT__col_buffer[4U] 
            = __VdlyVal__test_copper_lists__DOT__dut__DOT__tile_inst__DOT__col_buffer__v30;
    }
    if (__VdlySet__test_copper_lists__DOT__dut__DOT__tile_inst__DOT__col_buffer__v31) {
        vlSelfRef.test_copper_lists__DOT__dut__DOT__tile_inst__DOT__col_buffer[5U] 
            = __VdlyVal__test_copper_lists__DOT__dut__DOT__tile_inst__DOT__col_buffer__v31;
    }
    if (__VdlySet__test_copper_lists__DOT__dut__DOT__tile_inst__DOT__col_buffer__v32) {
        vlSelfRef.test_copper_lists__DOT__dut__DOT__tile_inst__DOT__col_buffer[6U] 
            = __VdlyVal__test_copper_lists__DOT__dut__DOT__tile_inst__DOT__col_buffer__v32;
    }
    if (__VdlySet__test_copper_lists__DOT__dut__DOT__tile_inst__DOT__col_buffer__v33) {
        vlSelfRef.test_copper_lists__DOT__dut__DOT__tile_inst__DOT__col_buffer[7U] 
            = __VdlyVal__test_copper_lists__DOT__dut__DOT__tile_inst__DOT__col_buffer__v33;
    }
    if (__VdlySet__test_copper_lists__DOT__dut__DOT__tile_inst__DOT__col_buffer__v34) {
        vlSelfRef.test_copper_lists__DOT__dut__DOT__tile_inst__DOT__col_buffer[8U] 
            = __VdlyVal__test_copper_lists__DOT__dut__DOT__tile_inst__DOT__col_buffer__v34;
    }
    if (__VdlySet__test_copper_lists__DOT__dut__DOT__tile_inst__DOT__col_buffer__v35) {
        vlSelfRef.test_copper_lists__DOT__dut__DOT__tile_inst__DOT__col_buffer[9U] 
            = __VdlyVal__test_copper_lists__DOT__dut__DOT__tile_inst__DOT__col_buffer__v35;
    }
    if (__VdlySet__test_copper_lists__DOT__dut__DOT__tile_inst__DOT__col_buffer__v36) {
        vlSelfRef.test_copper_lists__DOT__dut__DOT__tile_inst__DOT__col_buffer[10U] 
            = __VdlyVal__test_copper_lists__DOT__dut__DOT__tile_inst__DOT__col_buffer__v36;
    }
    if (__VdlySet__test_copper_lists__DOT__dut__DOT__tile_inst__DOT__col_buffer__v37) {
        vlSelfRef.test_copper_lists__DOT__dut__DOT__tile_inst__DOT__col_buffer[11U] 
            = __VdlyVal__test_copper_lists__DOT__dut__DOT__tile_inst__DOT__col_buffer__v37;
    }
    if (__VdlySet__test_copper_lists__DOT__dut__DOT__tile_inst__DOT__col_buffer__v38) {
        vlSelfRef.test_copper_lists__DOT__dut__DOT__tile_inst__DOT__col_buffer[12U] 
            = __VdlyVal__test_copper_lists__DOT__dut__DOT__tile_inst__DOT__col_buffer__v38;
    }
    if (__VdlySet__test_copper_lists__DOT__dut__DOT__tile_inst__DOT__col_buffer__v39) {
        vlSelfRef.test_copper_lists__DOT__dut__DOT__tile_inst__DOT__col_buffer[13U] 
            = __VdlyVal__test_copper_lists__DOT__dut__DOT__tile_inst__DOT__col_buffer__v39;
    }
    if (__VdlySet__test_copper_lists__DOT__dut__DOT__tile_inst__DOT__col_buffer__v40) {
        vlSelfRef.test_copper_lists__DOT__dut__DOT__tile_inst__DOT__col_buffer[14U] 
            = __VdlyVal__test_copper_lists__DOT__dut__DOT__tile_inst__DOT__col_buffer__v40;
    }
    if (__VdlySet__test_copper_lists__DOT__dut__DOT__tile_inst__DOT__col_buffer__v41) {
        vlSelfRef.test_copper_lists__DOT__dut__DOT__tile_inst__DOT__col_buffer[15U] 
            = __VdlyVal__test_copper_lists__DOT__dut__DOT__tile_inst__DOT__col_buffer__v41;
    }
    if (__VdlySet__test_copper_lists__DOT__dut__DOT__tile_inst__DOT__col_buffer__v42) {
        vlSelfRef.test_copper_lists__DOT__dut__DOT__tile_inst__DOT__col_buffer[16U] 
            = __VdlyVal__test_copper_lists__DOT__dut__DOT__tile_inst__DOT__col_buffer__v42;
    }
    if (__VdlySet__test_copper_lists__DOT__dut__DOT__tile_inst__DOT__col_buffer__v43) {
        vlSelfRef.test_copper_lists__DOT__dut__DOT__tile_inst__DOT__col_buffer[17U] 
            = __VdlyVal__test_copper_lists__DOT__dut__DOT__tile_inst__DOT__col_buffer__v43;
    }
    if (__VdlySet__test_copper_lists__DOT__dut__DOT__tile_inst__DOT__col_buffer__v44) {
        vlSelfRef.test_copper_lists__DOT__dut__DOT__tile_inst__DOT__col_buffer[18U] 
            = __VdlyVal__test_copper_lists__DOT__dut__DOT__tile_inst__DOT__col_buffer__v44;
    }
    if (__VdlySet__test_copper_lists__DOT__dut__DOT__tile_inst__DOT__col_buffer__v45) {
        vlSelfRef.test_copper_lists__DOT__dut__DOT__tile_inst__DOT__col_buffer[19U] 
            = __VdlyVal__test_copper_lists__DOT__dut__DOT__tile_inst__DOT__col_buffer__v45;
    }
    if (__VdlySet__test_copper_lists__DOT__dut__DOT__tile_inst__DOT__col_buffer__v46) {
        vlSelfRef.test_copper_lists__DOT__dut__DOT__tile_inst__DOT__col_buffer[20U] 
            = __VdlyVal__test_copper_lists__DOT__dut__DOT__tile_inst__DOT__col_buffer__v46;
    }
    if (__VdlySet__test_copper_lists__DOT__dut__DOT__tile_inst__DOT__col_buffer__v47) {
        vlSelfRef.test_copper_lists__DOT__dut__DOT__tile_inst__DOT__col_buffer[21U] 
            = __VdlyVal__test_copper_lists__DOT__dut__DOT__tile_inst__DOT__col_buffer__v47;
    }
    if (__VdlySet__test_copper_lists__DOT__dut__DOT__tile_inst__DOT__col_buffer__v48) {
        vlSelfRef.test_copper_lists__DOT__dut__DOT__tile_inst__DOT__col_buffer[22U] 
            = __VdlyVal__test_copper_lists__DOT__dut__DOT__tile_inst__DOT__col_buffer__v48;
    }
    if (__VdlySet__test_copper_lists__DOT__dut__DOT__tile_inst__DOT__col_buffer__v49) {
        vlSelfRef.test_copper_lists__DOT__dut__DOT__tile_inst__DOT__col_buffer[23U] 
            = __VdlyVal__test_copper_lists__DOT__dut__DOT__tile_inst__DOT__col_buffer__v49;
    }
    if (__VdlySet__test_copper_lists__DOT__dut__DOT__tile_inst__DOT__col_buffer__v50) {
        vlSelfRef.test_copper_lists__DOT__dut__DOT__tile_inst__DOT__col_buffer[24U] 
            = __VdlyVal__test_copper_lists__DOT__dut__DOT__tile_inst__DOT__col_buffer__v50;
    }
    if (vlSelfRef.test_copper_lists__DOT__dut__DOT__tile_inst__DOT__blt_tile_rd_pending) {
        vlSelfRef.test_copper_lists__DOT__dut__DOT__tile_inst__DOT__blt_tile_rd_latch 
            = vlSelfRef.test_copper_lists__DOT__dut__DOT__tile_inst__DOT__td_dout_a;
    }
    if (vlSelfRef.test_copper_lists__DOT__dut__DOT__cmd_spr_we) {
        __VdlyVal__test_copper_lists__DOT__dut__DOT__sprite_inst__DOT__spr_mem__DOT__mem__v0 
            = ((IData)(vlSelfRef.test_copper_lists__DOT__dut__DOT__cmd_spr_we)
                ? (IData)(vlSelfRef.test_copper_lists__DOT__dut__DOT__cmd_spr_din)
                : 0U);
        __VdlyDim0__test_copper_lists__DOT__dut__DOT__sprite_inst__DOT__spr_mem__DOT__mem__v0 
            = vlSelfRef.test_copper_lists__DOT__dut__DOT__spr_a_addr;
        __VdlySet__test_copper_lists__DOT__dut__DOT__sprite_inst__DOT__spr_mem__DOT__mem__v0 = 1U;
    }
    if (vlSelfRef.test_copper_lists__DOT__dut__DOT__cmd_color_we) {
        vlSelfRef.test_copper_lists__DOT__dut__DOT__text_inst__DOT__color_mem__DOT____Vlvbound_hcad4ec09__0 
            = ((IData)(vlSelfRef.test_copper_lists__DOT__dut__DOT__cmd_color_we)
                ? (IData)(vlSelfRef.test_copper_lists__DOT__dut__DOT__cmd_color_din)
                : 0U);
        if ((0x07cfU >= (IData)(vlSelfRef.test_copper_lists__DOT__dut__DOT__color_a_addr))) {
            __VdlyVal__test_copper_lists__DOT__dut__DOT__text_inst__DOT__color_mem__DOT__mem__v0 
                = vlSelfRef.test_copper_lists__DOT__dut__DOT__text_inst__DOT__color_mem__DOT____Vlvbound_hcad4ec09__0;
            __VdlyDim0__test_copper_lists__DOT__dut__DOT__text_inst__DOT__color_mem__DOT__mem__v0 
                = vlSelfRef.test_copper_lists__DOT__dut__DOT__color_a_addr;
            __VdlySet__test_copper_lists__DOT__dut__DOT__text_inst__DOT__color_mem__DOT__mem__v0 = 1U;
        }
    }
    if (vlSelfRef.test_copper_lists__DOT__dut__DOT__cmd_char_we) {
        vlSelfRef.test_copper_lists__DOT__dut__DOT__text_inst__DOT__char_mem__DOT____Vlvbound_hcad4ec09__0 
            = ((IData)(vlSelfRef.test_copper_lists__DOT__dut__DOT__cmd_char_we)
                ? (IData)(vlSelfRef.test_copper_lists__DOT__dut__DOT__cmd_char_din)
                : 0U);
        if ((0x07cfU >= (IData)(vlSelfRef.test_copper_lists__DOT__dut__DOT__char_a_addr))) {
            __VdlyVal__test_copper_lists__DOT__dut__DOT__text_inst__DOT__char_mem__DOT__mem__v0 
                = vlSelfRef.test_copper_lists__DOT__dut__DOT__text_inst__DOT__char_mem__DOT____Vlvbound_hcad4ec09__0;
            __VdlyDim0__test_copper_lists__DOT__dut__DOT__text_inst__DOT__char_mem__DOT__mem__v0 
                = vlSelfRef.test_copper_lists__DOT__dut__DOT__char_a_addr;
            __VdlySet__test_copper_lists__DOT__dut__DOT__text_inst__DOT__char_mem__DOT__mem__v0 = 1U;
        }
    }
    if (vlSelfRef.test_copper_lists__DOT__rst) {
        __Vdly__test_copper_lists__DOT__dut__DOT__artist_busy = 0U;
        __Vdly__test_copper_lists__DOT__dut__DOT__artist_inst__DOT__op = 0U;
        __Vdly__test_copper_lists__DOT__dut__DOT__artist_inst__DOT__fill_sp = 0U;
        __Vdly__test_copper_lists__DOT__dut__DOT__artist_inst__DOT__octant_count = 0U;
        __Vdly__test_copper_lists__DOT__dut__DOT__artist_inst__DOT__paint_phase = 0U;
        __Vdly__test_copper_lists__DOT__dut__DOT__artist_inst__DOT__gt_font_pending = 0U;
        __Vdly__test_copper_lists__DOT__dut__DOT__artist_inst__DOT__paint_init = 0U;
        vlSelfRef.test_copper_lists__DOT__dut__DOT__artist_gfx_we = 0U;
        vlSelfRef.test_copper_lists__DOT__dut__DOT__artist_gfx_re = 0U;
        vlSelfRef.test_copper_lists__DOT__dut__DOT__artist_font_re = 0U;
        vlSelfRef.test_copper_lists__DOT__dut__DOT__artist_inst__DOT__fs_a_we = 0U;
    } else {
        vlSelfRef.test_copper_lists__DOT__dut__DOT__artist_gfx_we = 0U;
        vlSelfRef.test_copper_lists__DOT__dut__DOT__artist_gfx_re = 0U;
        vlSelfRef.test_copper_lists__DOT__dut__DOT__artist_font_re = 0U;
        vlSelfRef.test_copper_lists__DOT__dut__DOT__artist_inst__DOT__fs_a_we = 0U;
        if ((1U == (IData)(vlSelfRef.test_copper_lists__DOT__dut__DOT__artist_inst__DOT__gt_font_pending))) {
            __Vdly__test_copper_lists__DOT__dut__DOT__artist_inst__DOT__gt_font_pending = 2U;
        } else if ((2U == (IData)(vlSelfRef.test_copper_lists__DOT__dut__DOT__artist_inst__DOT__gt_font_pending))) {
            __Vdly__test_copper_lists__DOT__dut__DOT__artist_inst__DOT__gt_font_byte 
                = vlSelfRef.test_copper_lists__DOT__dut__DOT__font_a_dout;
            __Vdly__test_copper_lists__DOT__dut__DOT__artist_inst__DOT__gt_font_pending = 0U;
        }
        if (vlSelfRef.test_copper_lists__DOT__dut__DOT__artist_busy) {
            if ((3U == (IData)(vlSelfRef.test_copper_lists__DOT__dut__DOT__artist_inst__DOT__op))) {
                if ((((VL_LTES_III(32, 0U, VL_EXTENDS_II(32,10, (IData)(vlSelfRef.test_copper_lists__DOT__dut__DOT__artist_inst__DOT__x))) 
                       & VL_GTS_III(32, 0x00000140U, 
                                    VL_EXTENDS_II(32,10, (IData)(vlSelfRef.test_copper_lists__DOT__dut__DOT__artist_inst__DOT__x)))) 
                      & VL_LTES_III(32, 0U, VL_EXTENDS_II(32,10, (IData)(vlSelfRef.test_copper_lists__DOT__dut__DOT__artist_inst__DOT__y)))) 
                     & VL_GTS_III(32, 0x000000c8U, 
                                  VL_EXTENDS_II(32,10, (IData)(vlSelfRef.test_copper_lists__DOT__dut__DOT__artist_inst__DOT__y))))) {
                    vlSelfRef.test_copper_lists__DOT__dut__DOT__artist_gfx_addr 
                        = (0x0000ffffU & (VL_MULS_III(32, (IData)(0x00000140U), 
                                                      VL_EXTENDS_II(32,10, (IData)(vlSelfRef.test_copper_lists__DOT__dut__DOT__artist_inst__DOT__y))) 
                                          + VL_EXTENDS_II(16,10, (IData)(vlSelfRef.test_copper_lists__DOT__dut__DOT__artist_inst__DOT__x))));
                    vlSelfRef.test_copper_lists__DOT__dut__DOT__artist_gfx_wdata 
                        = vlSelfRef.test_copper_lists__DOT__dut__DOT__artist_inst__DOT__color;
                    vlSelfRef.test_copper_lists__DOT__dut__DOT__artist_gfx_we = 1U;
                }
                if ((((((((IData)(vlSelfRef.test_copper_lists__DOT__dut__DOT__artist_inst__DOT__x) 
                          == (IData)(vlSelfRef.test_copper_lists__DOT__dut__DOT__artist_inst__DOT__x2)) 
                         & ((IData)(vlSelfRef.test_copper_lists__DOT__dut__DOT__artist_inst__DOT__y) 
                            == (IData)(vlSelfRef.test_copper_lists__DOT__dut__DOT__artist_inst__DOT__y2))) 
                        | VL_GTS_III(32, 0xfffffff0U, 
                                     VL_EXTENDS_II(32,10, (IData)(vlSelfRef.test_copper_lists__DOT__dut__DOT__artist_inst__DOT__x)))) 
                       | VL_LTS_III(32, 0x00000150U, 
                                    VL_EXTENDS_II(32,10, (IData)(vlSelfRef.test_copper_lists__DOT__dut__DOT__artist_inst__DOT__x)))) 
                      | VL_GTS_III(32, 0xfffffff0U, 
                                   VL_EXTENDS_II(32,10, (IData)(vlSelfRef.test_copper_lists__DOT__dut__DOT__artist_inst__DOT__y)))) 
                     | VL_LTS_III(32, 0x000000d8U, 
                                  VL_EXTENDS_II(32,10, (IData)(vlSelfRef.test_copper_lists__DOT__dut__DOT__artist_inst__DOT__y))))) {
                    __Vdly__test_copper_lists__DOT__dut__DOT__artist_busy = 0U;
                } else if ((VL_GTES_III(32, VL_MULS_III(32, (IData)(2U), 
                                                        VL_EXTENDS_II(32,10, (IData)(vlSelfRef.test_copper_lists__DOT__dut__DOT__artist_inst__DOT__err))), 
                                        VL_EXTENDS_II(32,10, (IData)(vlSelfRef.test_copper_lists__DOT__dut__DOT__artist_inst__DOT__dy))) 
                            & VL_LTES_III(32, VL_MULS_III(32, (IData)(2U), 
                                                          VL_EXTENDS_II(32,10, (IData)(vlSelfRef.test_copper_lists__DOT__dut__DOT__artist_inst__DOT__err))), 
                                          VL_EXTENDS_II(32,10, (IData)(vlSelfRef.test_copper_lists__DOT__dut__DOT__artist_inst__DOT__dx))))) {
                    __Vdly__test_copper_lists__DOT__dut__DOT__artist_inst__DOT__err 
                        = (0x000003ffU & (((IData)(vlSelfRef.test_copper_lists__DOT__dut__DOT__artist_inst__DOT__err) 
                                           + (IData)(vlSelfRef.test_copper_lists__DOT__dut__DOT__artist_inst__DOT__dy)) 
                                          + (IData)(vlSelfRef.test_copper_lists__DOT__dut__DOT__artist_inst__DOT__dx)));
                    __Vdly__test_copper_lists__DOT__dut__DOT__artist_inst__DOT__x 
                        = (0x000003ffU & ((IData)(vlSelfRef.test_copper_lists__DOT__dut__DOT__artist_inst__DOT__x) 
                                          + (IData)(vlSelfRef.test_copper_lists__DOT__dut__DOT__artist_inst__DOT__sx)));
                    __Vdly__test_copper_lists__DOT__dut__DOT__artist_inst__DOT__y 
                        = (0x000003ffU & ((IData)(vlSelfRef.test_copper_lists__DOT__dut__DOT__artist_inst__DOT__y) 
                                          + (IData)(vlSelfRef.test_copper_lists__DOT__dut__DOT__artist_inst__DOT__sy)));
                } else if (VL_GTES_III(32, VL_MULS_III(32, (IData)(2U), 
                                                       VL_EXTENDS_II(32,10, (IData)(vlSelfRef.test_copper_lists__DOT__dut__DOT__artist_inst__DOT__err))), 
                                       VL_EXTENDS_II(32,10, (IData)(vlSelfRef.test_copper_lists__DOT__dut__DOT__artist_inst__DOT__dy)))) {
                    __Vdly__test_copper_lists__DOT__dut__DOT__artist_inst__DOT__err 
                        = (0x000003ffU & ((IData)(vlSelfRef.test_copper_lists__DOT__dut__DOT__artist_inst__DOT__err) 
                                          + (IData)(vlSelfRef.test_copper_lists__DOT__dut__DOT__artist_inst__DOT__dy)));
                    __Vdly__test_copper_lists__DOT__dut__DOT__artist_inst__DOT__x 
                        = (0x000003ffU & ((IData)(vlSelfRef.test_copper_lists__DOT__dut__DOT__artist_inst__DOT__x) 
                                          + (IData)(vlSelfRef.test_copper_lists__DOT__dut__DOT__artist_inst__DOT__sx)));
                } else if (VL_LTES_III(32, VL_MULS_III(32, (IData)(2U), 
                                                       VL_EXTENDS_II(32,10, (IData)(vlSelfRef.test_copper_lists__DOT__dut__DOT__artist_inst__DOT__err))), 
                                       VL_EXTENDS_II(32,10, (IData)(vlSelfRef.test_copper_lists__DOT__dut__DOT__artist_inst__DOT__dx)))) {
                    __Vdly__test_copper_lists__DOT__dut__DOT__artist_inst__DOT__err 
                        = (0x000003ffU & ((IData)(vlSelfRef.test_copper_lists__DOT__dut__DOT__artist_inst__DOT__err) 
                                          + (IData)(vlSelfRef.test_copper_lists__DOT__dut__DOT__artist_inst__DOT__dx)));
                    __Vdly__test_copper_lists__DOT__dut__DOT__artist_inst__DOT__y 
                        = (0x000003ffU & ((IData)(vlSelfRef.test_copper_lists__DOT__dut__DOT__artist_inst__DOT__y) 
                                          + (IData)(vlSelfRef.test_copper_lists__DOT__dut__DOT__artist_inst__DOT__sy)));
                }
            } else if ((5U == (IData)(vlSelfRef.test_copper_lists__DOT__dut__DOT__artist_inst__DOT__op))) {
                if (((0x0140U > (IData)(vlSelfRef.test_copper_lists__DOT__dut__DOT__artist_inst__DOT__cx)) 
                     & (0xc8U > (IData)(vlSelfRef.test_copper_lists__DOT__dut__DOT__artist_inst__DOT__cy)))) {
                    if ((((((IData)(vlSelfRef.test_copper_lists__DOT__dut__DOT__artist_inst__DOT__cy) 
                            == (0x000000ffU & (IData)(vlSelfRef.test_copper_lists__DOT__dut__DOT__artist_inst__DOT__y))) 
                           | ((IData)(vlSelfRef.test_copper_lists__DOT__dut__DOT__artist_inst__DOT__cy) 
                              == (0x000000ffU & (IData)(vlSelfRef.test_copper_lists__DOT__dut__DOT__artist_inst__DOT__y2)))) 
                          | ((IData)(vlSelfRef.test_copper_lists__DOT__dut__DOT__artist_inst__DOT__cx) 
                             == (0x000001ffU & (IData)(vlSelfRef.test_copper_lists__DOT__dut__DOT__artist_inst__DOT__x)))) 
                         | ((IData)(vlSelfRef.test_copper_lists__DOT__dut__DOT__artist_inst__DOT__cx) 
                            == (0x000001ffU & (IData)(vlSelfRef.test_copper_lists__DOT__dut__DOT__artist_inst__DOT__x2))))) {
                        vlSelfRef.test_copper_lists__DOT__dut__DOT__artist_gfx_addr 
                            = (0x0000ffffU & (((IData)(0x00000140U) 
                                               * (IData)(vlSelfRef.test_copper_lists__DOT__dut__DOT__artist_inst__DOT__cy)) 
                                              + (IData)(vlSelfRef.test_copper_lists__DOT__dut__DOT__artist_inst__DOT__cx)));
                        vlSelfRef.test_copper_lists__DOT__dut__DOT__artist_gfx_wdata 
                            = vlSelfRef.test_copper_lists__DOT__dut__DOT__artist_inst__DOT__color;
                        vlSelfRef.test_copper_lists__DOT__dut__DOT__artist_gfx_we = 1U;
                    }
                }
                if (((IData)(vlSelfRef.test_copper_lists__DOT__dut__DOT__artist_inst__DOT__cx) 
                     >= (0x000001ffU & (IData)(vlSelfRef.test_copper_lists__DOT__dut__DOT__artist_inst__DOT__x2)))) {
                    __Vdly__test_copper_lists__DOT__dut__DOT__artist_inst__DOT__cx 
                        = (0x000001ffU & (IData)(vlSelfRef.test_copper_lists__DOT__dut__DOT__artist_inst__DOT__x));
                    if (((IData)(vlSelfRef.test_copper_lists__DOT__dut__DOT__artist_inst__DOT__cy) 
                         >= (0x000000ffU & (IData)(vlSelfRef.test_copper_lists__DOT__dut__DOT__artist_inst__DOT__y2)))) {
                        __Vdly__test_copper_lists__DOT__dut__DOT__artist_busy = 0U;
                    } else {
                        __Vdly__test_copper_lists__DOT__dut__DOT__artist_inst__DOT__cy 
                            = (0x000000ffU & ((IData)(1U) 
                                              + (IData)(vlSelfRef.test_copper_lists__DOT__dut__DOT__artist_inst__DOT__cy)));
                    }
                } else {
                    __Vdly__test_copper_lists__DOT__dut__DOT__artist_inst__DOT__cx 
                        = (0x000001ffU & ((IData)(1U) 
                                          + (IData)(vlSelfRef.test_copper_lists__DOT__dut__DOT__artist_inst__DOT__cx)));
                }
            } else if ((6U == (IData)(vlSelfRef.test_copper_lists__DOT__dut__DOT__artist_inst__DOT__op))) {
                if (((0x0140U > (IData)(vlSelfRef.test_copper_lists__DOT__dut__DOT__artist_inst__DOT__cx)) 
                     & (0xc8U > (IData)(vlSelfRef.test_copper_lists__DOT__dut__DOT__artist_inst__DOT__cy)))) {
                    vlSelfRef.test_copper_lists__DOT__dut__DOT__artist_gfx_addr 
                        = (0x0000ffffU & (((IData)(0x00000140U) 
                                           * (IData)(vlSelfRef.test_copper_lists__DOT__dut__DOT__artist_inst__DOT__cy)) 
                                          + (IData)(vlSelfRef.test_copper_lists__DOT__dut__DOT__artist_inst__DOT__cx)));
                    vlSelfRef.test_copper_lists__DOT__dut__DOT__artist_gfx_wdata 
                        = vlSelfRef.test_copper_lists__DOT__dut__DOT__artist_inst__DOT__color;
                    vlSelfRef.test_copper_lists__DOT__dut__DOT__artist_gfx_we = 1U;
                }
                if (((IData)(vlSelfRef.test_copper_lists__DOT__dut__DOT__artist_inst__DOT__cx) 
                     >= (0x000001ffU & (IData)(vlSelfRef.test_copper_lists__DOT__dut__DOT__artist_inst__DOT__x2)))) {
                    __Vdly__test_copper_lists__DOT__dut__DOT__artist_inst__DOT__cx 
                        = (0x000001ffU & (IData)(vlSelfRef.test_copper_lists__DOT__dut__DOT__artist_inst__DOT__x));
                    if (((IData)(vlSelfRef.test_copper_lists__DOT__dut__DOT__artist_inst__DOT__cy) 
                         >= (0x000000ffU & (IData)(vlSelfRef.test_copper_lists__DOT__dut__DOT__artist_inst__DOT__y2)))) {
                        __Vdly__test_copper_lists__DOT__dut__DOT__artist_busy = 0U;
                    } else {
                        __Vdly__test_copper_lists__DOT__dut__DOT__artist_inst__DOT__cy 
                            = (0x000000ffU & ((IData)(1U) 
                                              + (IData)(vlSelfRef.test_copper_lists__DOT__dut__DOT__artist_inst__DOT__cy)));
                    }
                } else {
                    __Vdly__test_copper_lists__DOT__dut__DOT__artist_inst__DOT__cx 
                        = (0x000001ffU & ((IData)(1U) 
                                          + (IData)(vlSelfRef.test_copper_lists__DOT__dut__DOT__artist_inst__DOT__cx)));
                }
            } else if ((7U == (IData)(vlSelfRef.test_copper_lists__DOT__dut__DOT__artist_inst__DOT__op))) {
                vlSelfRef.test_copper_lists__DOT__dut__DOT__artist_gfx_addr 
                    = (0x0000ffffU & (((IData)(0x00000140U) 
                                       * (IData)(vlSelfRef.test_copper_lists__DOT__dut__DOT__artist_inst__DOT__cy)) 
                                      + (IData)(vlSelfRef.test_copper_lists__DOT__dut__DOT__artist_inst__DOT__cx)));
                vlSelfRef.test_copper_lists__DOT__dut__DOT__artist_gfx_wdata = 0U;
                vlSelfRef.test_copper_lists__DOT__dut__DOT__artist_gfx_we = 1U;
                if ((0x013fU == (IData)(vlSelfRef.test_copper_lists__DOT__dut__DOT__artist_inst__DOT__cx))) {
                    __Vdly__test_copper_lists__DOT__dut__DOT__artist_inst__DOT__cx = 0U;
                    if ((0xc7U == (IData)(vlSelfRef.test_copper_lists__DOT__dut__DOT__artist_inst__DOT__cy))) {
                        __Vdly__test_copper_lists__DOT__dut__DOT__artist_busy = 0U;
                    } else {
                        __Vdly__test_copper_lists__DOT__dut__DOT__artist_inst__DOT__cy 
                            = (0x000000ffU & ((IData)(1U) 
                                              + (IData)(vlSelfRef.test_copper_lists__DOT__dut__DOT__artist_inst__DOT__cy)));
                    }
                } else {
                    __Vdly__test_copper_lists__DOT__dut__DOT__artist_inst__DOT__cx 
                        = (0x000001ffU & ((IData)(1U) 
                                          + (IData)(vlSelfRef.test_copper_lists__DOT__dut__DOT__artist_inst__DOT__cx)));
                }
            } else if ((4U == (IData)(vlSelfRef.test_copper_lists__DOT__dut__DOT__artist_inst__DOT__op))) {
                if (VL_LTES_III(10, (IData)(vlSelfRef.test_copper_lists__DOT__dut__DOT__artist_inst__DOT__dx), (IData)(vlSelfRef.test_copper_lists__DOT__dut__DOT__artist_inst__DOT__dy))) {
                    if ((4U & (IData)(vlSelfRef.test_copper_lists__DOT__dut__DOT__artist_inst__DOT__octant_count))) {
                        if ((2U & (IData)(vlSelfRef.test_copper_lists__DOT__dut__DOT__artist_inst__DOT__octant_count))) {
                            if ((1U & (IData)(vlSelfRef.test_copper_lists__DOT__dut__DOT__artist_inst__DOT__octant_count))) {
                                if ((((VL_LTES_III(32, 0U, 
                                                   (VL_EXTENDS_II(32,10, (IData)(vlSelfRef.test_copper_lists__DOT__dut__DOT__artist_inst__DOT__x)) 
                                                    - 
                                                    VL_EXTENDS_II(32,10, (IData)(vlSelfRef.test_copper_lists__DOT__dut__DOT__artist_inst__DOT__dy)))) 
                                       & VL_GTS_III(32, 0x00000140U, 
                                                    (VL_EXTENDS_II(32,10, (IData)(vlSelfRef.test_copper_lists__DOT__dut__DOT__artist_inst__DOT__x)) 
                                                     - 
                                                     VL_EXTENDS_II(32,10, (IData)(vlSelfRef.test_copper_lists__DOT__dut__DOT__artist_inst__DOT__dy))))) 
                                      & VL_LTES_III(32, 0U, 
                                                    (VL_EXTENDS_II(32,10, (IData)(vlSelfRef.test_copper_lists__DOT__dut__DOT__artist_inst__DOT__y)) 
                                                     - 
                                                     VL_EXTENDS_II(32,10, (IData)(vlSelfRef.test_copper_lists__DOT__dut__DOT__artist_inst__DOT__dx))))) 
                                     & VL_GTS_III(32, 0x000000c8U, 
                                                  (VL_EXTENDS_II(32,10, (IData)(vlSelfRef.test_copper_lists__DOT__dut__DOT__artist_inst__DOT__y)) 
                                                   - 
                                                   VL_EXTENDS_II(32,10, (IData)(vlSelfRef.test_copper_lists__DOT__dut__DOT__artist_inst__DOT__dx)))))) {
                                    vlSelfRef.test_copper_lists__DOT__dut__DOT__artist_gfx_addr 
                                        = (0x0000ffffU 
                                           & (VL_MULS_III(32, (IData)(0x00000140U), 
                                                          (VL_EXTENDS_II(32,10, (IData)(vlSelfRef.test_copper_lists__DOT__dut__DOT__artist_inst__DOT__y)) 
                                                           - 
                                                           VL_EXTENDS_II(32,10, (IData)(vlSelfRef.test_copper_lists__DOT__dut__DOT__artist_inst__DOT__dx)))) 
                                              + (VL_EXTENDS_II(16,10, (IData)(vlSelfRef.test_copper_lists__DOT__dut__DOT__artist_inst__DOT__x)) 
                                                 - 
                                                 VL_EXTENDS_II(16,10, (IData)(vlSelfRef.test_copper_lists__DOT__dut__DOT__artist_inst__DOT__dy)))));
                                    vlSelfRef.test_copper_lists__DOT__dut__DOT__artist_gfx_wdata 
                                        = vlSelfRef.test_copper_lists__DOT__dut__DOT__artist_inst__DOT__color;
                                    vlSelfRef.test_copper_lists__DOT__dut__DOT__artist_gfx_we = 1U;
                                }
                                __Vdly__test_copper_lists__DOT__dut__DOT__artist_inst__DOT__octant_count = 0U;
                                __Vdly__test_copper_lists__DOT__dut__DOT__artist_inst__DOT__dx 
                                    = (0x000003ffU 
                                       & ((IData)(1U) 
                                          + VL_EXTENDS_II(10,10, (IData)(vlSelfRef.test_copper_lists__DOT__dut__DOT__artist_inst__DOT__dx))));
                                if (VL_GTS_III(32, 0U, 
                                               VL_EXTENDS_II(32,10, (IData)(vlSelfRef.test_copper_lists__DOT__dut__DOT__artist_inst__DOT__err)))) {
                                    __Vdly__test_copper_lists__DOT__dut__DOT__artist_inst__DOT__err 
                                        = (0x000003ffU 
                                           & ((IData)(3U) 
                                              + (VL_EXTENDS_II(10,10, (IData)(vlSelfRef.test_copper_lists__DOT__dut__DOT__artist_inst__DOT__err)) 
                                                 + 
                                                 VL_MULS_III(32, (IData)(2U), 
                                                             VL_EXTENDS_II(32,10, (IData)(vlSelfRef.test_copper_lists__DOT__dut__DOT__artist_inst__DOT__dx))))));
                                } else {
                                    __Vdly__test_copper_lists__DOT__dut__DOT__artist_inst__DOT__err 
                                        = (0x000003ffU 
                                           & ((IData)(5U) 
                                              + (VL_EXTENDS_II(10,10, (IData)(vlSelfRef.test_copper_lists__DOT__dut__DOT__artist_inst__DOT__err)) 
                                                 + 
                                                 VL_MULS_III(32, (IData)(2U), 
                                                             (VL_EXTENDS_II(32,10, (IData)(vlSelfRef.test_copper_lists__DOT__dut__DOT__artist_inst__DOT__dx)) 
                                                              - 
                                                              VL_EXTENDS_II(32,10, (IData)(vlSelfRef.test_copper_lists__DOT__dut__DOT__artist_inst__DOT__dy)))))));
                                    __Vdly__test_copper_lists__DOT__dut__DOT__artist_inst__DOT__dy 
                                        = (0x000003ffU 
                                           & (VL_EXTENDS_II(10,10, (IData)(vlSelfRef.test_copper_lists__DOT__dut__DOT__artist_inst__DOT__dy)) 
                                              - (IData)(1U)));
                                }
                            } else {
                                if ((((VL_LTES_III(32, 0U, 
                                                   (VL_EXTENDS_II(32,10, (IData)(vlSelfRef.test_copper_lists__DOT__dut__DOT__artist_inst__DOT__x)) 
                                                    + 
                                                    VL_EXTENDS_II(32,10, (IData)(vlSelfRef.test_copper_lists__DOT__dut__DOT__artist_inst__DOT__dy)))) 
                                       & VL_GTS_III(32, 0x00000140U, 
                                                    (VL_EXTENDS_II(32,10, (IData)(vlSelfRef.test_copper_lists__DOT__dut__DOT__artist_inst__DOT__x)) 
                                                     + 
                                                     VL_EXTENDS_II(32,10, (IData)(vlSelfRef.test_copper_lists__DOT__dut__DOT__artist_inst__DOT__dy))))) 
                                      & VL_LTES_III(32, 0U, 
                                                    (VL_EXTENDS_II(32,10, (IData)(vlSelfRef.test_copper_lists__DOT__dut__DOT__artist_inst__DOT__y)) 
                                                     - 
                                                     VL_EXTENDS_II(32,10, (IData)(vlSelfRef.test_copper_lists__DOT__dut__DOT__artist_inst__DOT__dx))))) 
                                     & VL_GTS_III(32, 0x000000c8U, 
                                                  (VL_EXTENDS_II(32,10, (IData)(vlSelfRef.test_copper_lists__DOT__dut__DOT__artist_inst__DOT__y)) 
                                                   - 
                                                   VL_EXTENDS_II(32,10, (IData)(vlSelfRef.test_copper_lists__DOT__dut__DOT__artist_inst__DOT__dx)))))) {
                                    vlSelfRef.test_copper_lists__DOT__dut__DOT__artist_gfx_addr 
                                        = (0x0000ffffU 
                                           & (VL_MULS_III(32, (IData)(0x00000140U), 
                                                          (VL_EXTENDS_II(32,10, (IData)(vlSelfRef.test_copper_lists__DOT__dut__DOT__artist_inst__DOT__y)) 
                                                           - 
                                                           VL_EXTENDS_II(32,10, (IData)(vlSelfRef.test_copper_lists__DOT__dut__DOT__artist_inst__DOT__dx)))) 
                                              + (VL_EXTENDS_II(16,10, (IData)(vlSelfRef.test_copper_lists__DOT__dut__DOT__artist_inst__DOT__x)) 
                                                 + 
                                                 VL_EXTENDS_II(16,10, (IData)(vlSelfRef.test_copper_lists__DOT__dut__DOT__artist_inst__DOT__dy)))));
                                    vlSelfRef.test_copper_lists__DOT__dut__DOT__artist_gfx_wdata 
                                        = vlSelfRef.test_copper_lists__DOT__dut__DOT__artist_inst__DOT__color;
                                    vlSelfRef.test_copper_lists__DOT__dut__DOT__artist_gfx_we = 1U;
                                }
                                __Vdly__test_copper_lists__DOT__dut__DOT__artist_inst__DOT__octant_count = 7U;
                            }
                        } else if ((1U & (IData)(vlSelfRef.test_copper_lists__DOT__dut__DOT__artist_inst__DOT__octant_count))) {
                            if ((((VL_LTES_III(32, 0U, 
                                               (VL_EXTENDS_II(32,10, (IData)(vlSelfRef.test_copper_lists__DOT__dut__DOT__artist_inst__DOT__x)) 
                                                - VL_EXTENDS_II(32,10, (IData)(vlSelfRef.test_copper_lists__DOT__dut__DOT__artist_inst__DOT__dy)))) 
                                   & VL_GTS_III(32, 0x00000140U, 
                                                (VL_EXTENDS_II(32,10, (IData)(vlSelfRef.test_copper_lists__DOT__dut__DOT__artist_inst__DOT__x)) 
                                                 - 
                                                 VL_EXTENDS_II(32,10, (IData)(vlSelfRef.test_copper_lists__DOT__dut__DOT__artist_inst__DOT__dy))))) 
                                  & VL_LTES_III(32, 0U, 
                                                (VL_EXTENDS_II(32,10, (IData)(vlSelfRef.test_copper_lists__DOT__dut__DOT__artist_inst__DOT__y)) 
                                                 + 
                                                 VL_EXTENDS_II(32,10, (IData)(vlSelfRef.test_copper_lists__DOT__dut__DOT__artist_inst__DOT__dx))))) 
                                 & VL_GTS_III(32, 0x000000c8U, 
                                              (VL_EXTENDS_II(32,10, (IData)(vlSelfRef.test_copper_lists__DOT__dut__DOT__artist_inst__DOT__y)) 
                                               + VL_EXTENDS_II(32,10, (IData)(vlSelfRef.test_copper_lists__DOT__dut__DOT__artist_inst__DOT__dx)))))) {
                                vlSelfRef.test_copper_lists__DOT__dut__DOT__artist_gfx_addr 
                                    = (0x0000ffffU 
                                       & (VL_MULS_III(32, (IData)(0x00000140U), 
                                                      (VL_EXTENDS_II(32,10, (IData)(vlSelfRef.test_copper_lists__DOT__dut__DOT__artist_inst__DOT__y)) 
                                                       + 
                                                       VL_EXTENDS_II(32,10, (IData)(vlSelfRef.test_copper_lists__DOT__dut__DOT__artist_inst__DOT__dx)))) 
                                          + (VL_EXTENDS_II(16,10, (IData)(vlSelfRef.test_copper_lists__DOT__dut__DOT__artist_inst__DOT__x)) 
                                             - VL_EXTENDS_II(16,10, (IData)(vlSelfRef.test_copper_lists__DOT__dut__DOT__artist_inst__DOT__dy)))));
                                vlSelfRef.test_copper_lists__DOT__dut__DOT__artist_gfx_wdata 
                                    = vlSelfRef.test_copper_lists__DOT__dut__DOT__artist_inst__DOT__color;
                                vlSelfRef.test_copper_lists__DOT__dut__DOT__artist_gfx_we = 1U;
                            }
                            __Vdly__test_copper_lists__DOT__dut__DOT__artist_inst__DOT__octant_count = 6U;
                        } else {
                            if ((((VL_LTES_III(32, 0U, 
                                               (VL_EXTENDS_II(32,10, (IData)(vlSelfRef.test_copper_lists__DOT__dut__DOT__artist_inst__DOT__x)) 
                                                + VL_EXTENDS_II(32,10, (IData)(vlSelfRef.test_copper_lists__DOT__dut__DOT__artist_inst__DOT__dy)))) 
                                   & VL_GTS_III(32, 0x00000140U, 
                                                (VL_EXTENDS_II(32,10, (IData)(vlSelfRef.test_copper_lists__DOT__dut__DOT__artist_inst__DOT__x)) 
                                                 + 
                                                 VL_EXTENDS_II(32,10, (IData)(vlSelfRef.test_copper_lists__DOT__dut__DOT__artist_inst__DOT__dy))))) 
                                  & VL_LTES_III(32, 0U, 
                                                (VL_EXTENDS_II(32,10, (IData)(vlSelfRef.test_copper_lists__DOT__dut__DOT__artist_inst__DOT__y)) 
                                                 + 
                                                 VL_EXTENDS_II(32,10, (IData)(vlSelfRef.test_copper_lists__DOT__dut__DOT__artist_inst__DOT__dx))))) 
                                 & VL_GTS_III(32, 0x000000c8U, 
                                              (VL_EXTENDS_II(32,10, (IData)(vlSelfRef.test_copper_lists__DOT__dut__DOT__artist_inst__DOT__y)) 
                                               + VL_EXTENDS_II(32,10, (IData)(vlSelfRef.test_copper_lists__DOT__dut__DOT__artist_inst__DOT__dx)))))) {
                                vlSelfRef.test_copper_lists__DOT__dut__DOT__artist_gfx_addr 
                                    = (0x0000ffffU 
                                       & (VL_MULS_III(32, (IData)(0x00000140U), 
                                                      (VL_EXTENDS_II(32,10, (IData)(vlSelfRef.test_copper_lists__DOT__dut__DOT__artist_inst__DOT__y)) 
                                                       + 
                                                       VL_EXTENDS_II(32,10, (IData)(vlSelfRef.test_copper_lists__DOT__dut__DOT__artist_inst__DOT__dx)))) 
                                          + (VL_EXTENDS_II(16,10, (IData)(vlSelfRef.test_copper_lists__DOT__dut__DOT__artist_inst__DOT__x)) 
                                             + VL_EXTENDS_II(16,10, (IData)(vlSelfRef.test_copper_lists__DOT__dut__DOT__artist_inst__DOT__dy)))));
                                vlSelfRef.test_copper_lists__DOT__dut__DOT__artist_gfx_wdata 
                                    = vlSelfRef.test_copper_lists__DOT__dut__DOT__artist_inst__DOT__color;
                                vlSelfRef.test_copper_lists__DOT__dut__DOT__artist_gfx_we = 1U;
                            }
                            __Vdly__test_copper_lists__DOT__dut__DOT__artist_inst__DOT__octant_count = 5U;
                        }
                    } else if ((2U & (IData)(vlSelfRef.test_copper_lists__DOT__dut__DOT__artist_inst__DOT__octant_count))) {
                        if ((1U & (IData)(vlSelfRef.test_copper_lists__DOT__dut__DOT__artist_inst__DOT__octant_count))) {
                            if ((((VL_LTES_III(32, 0U, 
                                               (VL_EXTENDS_II(32,10, (IData)(vlSelfRef.test_copper_lists__DOT__dut__DOT__artist_inst__DOT__x)) 
                                                - VL_EXTENDS_II(32,10, (IData)(vlSelfRef.test_copper_lists__DOT__dut__DOT__artist_inst__DOT__dx)))) 
                                   & VL_GTS_III(32, 0x00000140U, 
                                                (VL_EXTENDS_II(32,10, (IData)(vlSelfRef.test_copper_lists__DOT__dut__DOT__artist_inst__DOT__x)) 
                                                 - 
                                                 VL_EXTENDS_II(32,10, (IData)(vlSelfRef.test_copper_lists__DOT__dut__DOT__artist_inst__DOT__dx))))) 
                                  & VL_LTES_III(32, 0U, 
                                                (VL_EXTENDS_II(32,10, (IData)(vlSelfRef.test_copper_lists__DOT__dut__DOT__artist_inst__DOT__y)) 
                                                 - 
                                                 VL_EXTENDS_II(32,10, (IData)(vlSelfRef.test_copper_lists__DOT__dut__DOT__artist_inst__DOT__dy))))) 
                                 & VL_GTS_III(32, 0x000000c8U, 
                                              (VL_EXTENDS_II(32,10, (IData)(vlSelfRef.test_copper_lists__DOT__dut__DOT__artist_inst__DOT__y)) 
                                               - VL_EXTENDS_II(32,10, (IData)(vlSelfRef.test_copper_lists__DOT__dut__DOT__artist_inst__DOT__dy)))))) {
                                vlSelfRef.test_copper_lists__DOT__dut__DOT__artist_gfx_addr 
                                    = (0x0000ffffU 
                                       & (VL_MULS_III(32, (IData)(0x00000140U), 
                                                      (VL_EXTENDS_II(32,10, (IData)(vlSelfRef.test_copper_lists__DOT__dut__DOT__artist_inst__DOT__y)) 
                                                       - 
                                                       VL_EXTENDS_II(32,10, (IData)(vlSelfRef.test_copper_lists__DOT__dut__DOT__artist_inst__DOT__dy)))) 
                                          + (VL_EXTENDS_II(16,10, (IData)(vlSelfRef.test_copper_lists__DOT__dut__DOT__artist_inst__DOT__x)) 
                                             - VL_EXTENDS_II(16,10, (IData)(vlSelfRef.test_copper_lists__DOT__dut__DOT__artist_inst__DOT__dx)))));
                                vlSelfRef.test_copper_lists__DOT__dut__DOT__artist_gfx_wdata 
                                    = vlSelfRef.test_copper_lists__DOT__dut__DOT__artist_inst__DOT__color;
                                vlSelfRef.test_copper_lists__DOT__dut__DOT__artist_gfx_we = 1U;
                            }
                            __Vdly__test_copper_lists__DOT__dut__DOT__artist_inst__DOT__octant_count = 4U;
                        } else {
                            if ((((VL_LTES_III(32, 0U, 
                                               (VL_EXTENDS_II(32,10, (IData)(vlSelfRef.test_copper_lists__DOT__dut__DOT__artist_inst__DOT__x)) 
                                                + VL_EXTENDS_II(32,10, (IData)(vlSelfRef.test_copper_lists__DOT__dut__DOT__artist_inst__DOT__dx)))) 
                                   & VL_GTS_III(32, 0x00000140U, 
                                                (VL_EXTENDS_II(32,10, (IData)(vlSelfRef.test_copper_lists__DOT__dut__DOT__artist_inst__DOT__x)) 
                                                 + 
                                                 VL_EXTENDS_II(32,10, (IData)(vlSelfRef.test_copper_lists__DOT__dut__DOT__artist_inst__DOT__dx))))) 
                                  & VL_LTES_III(32, 0U, 
                                                (VL_EXTENDS_II(32,10, (IData)(vlSelfRef.test_copper_lists__DOT__dut__DOT__artist_inst__DOT__y)) 
                                                 - 
                                                 VL_EXTENDS_II(32,10, (IData)(vlSelfRef.test_copper_lists__DOT__dut__DOT__artist_inst__DOT__dy))))) 
                                 & VL_GTS_III(32, 0x000000c8U, 
                                              (VL_EXTENDS_II(32,10, (IData)(vlSelfRef.test_copper_lists__DOT__dut__DOT__artist_inst__DOT__y)) 
                                               - VL_EXTENDS_II(32,10, (IData)(vlSelfRef.test_copper_lists__DOT__dut__DOT__artist_inst__DOT__dy)))))) {
                                vlSelfRef.test_copper_lists__DOT__dut__DOT__artist_gfx_addr 
                                    = (0x0000ffffU 
                                       & (VL_MULS_III(32, (IData)(0x00000140U), 
                                                      (VL_EXTENDS_II(32,10, (IData)(vlSelfRef.test_copper_lists__DOT__dut__DOT__artist_inst__DOT__y)) 
                                                       - 
                                                       VL_EXTENDS_II(32,10, (IData)(vlSelfRef.test_copper_lists__DOT__dut__DOT__artist_inst__DOT__dy)))) 
                                          + (VL_EXTENDS_II(16,10, (IData)(vlSelfRef.test_copper_lists__DOT__dut__DOT__artist_inst__DOT__x)) 
                                             + VL_EXTENDS_II(16,10, (IData)(vlSelfRef.test_copper_lists__DOT__dut__DOT__artist_inst__DOT__dx)))));
                                vlSelfRef.test_copper_lists__DOT__dut__DOT__artist_gfx_wdata 
                                    = vlSelfRef.test_copper_lists__DOT__dut__DOT__artist_inst__DOT__color;
                                vlSelfRef.test_copper_lists__DOT__dut__DOT__artist_gfx_we = 1U;
                            }
                            __Vdly__test_copper_lists__DOT__dut__DOT__artist_inst__DOT__octant_count = 3U;
                        }
                    } else if ((1U & (IData)(vlSelfRef.test_copper_lists__DOT__dut__DOT__artist_inst__DOT__octant_count))) {
                        if ((((VL_LTES_III(32, 0U, 
                                           (VL_EXTENDS_II(32,10, (IData)(vlSelfRef.test_copper_lists__DOT__dut__DOT__artist_inst__DOT__x)) 
                                            - VL_EXTENDS_II(32,10, (IData)(vlSelfRef.test_copper_lists__DOT__dut__DOT__artist_inst__DOT__dx)))) 
                               & VL_GTS_III(32, 0x00000140U, 
                                            (VL_EXTENDS_II(32,10, (IData)(vlSelfRef.test_copper_lists__DOT__dut__DOT__artist_inst__DOT__x)) 
                                             - VL_EXTENDS_II(32,10, (IData)(vlSelfRef.test_copper_lists__DOT__dut__DOT__artist_inst__DOT__dx))))) 
                              & VL_LTES_III(32, 0U, 
                                            (VL_EXTENDS_II(32,10, (IData)(vlSelfRef.test_copper_lists__DOT__dut__DOT__artist_inst__DOT__y)) 
                                             + VL_EXTENDS_II(32,10, (IData)(vlSelfRef.test_copper_lists__DOT__dut__DOT__artist_inst__DOT__dy))))) 
                             & VL_GTS_III(32, 0x000000c8U, 
                                          (VL_EXTENDS_II(32,10, (IData)(vlSelfRef.test_copper_lists__DOT__dut__DOT__artist_inst__DOT__y)) 
                                           + VL_EXTENDS_II(32,10, (IData)(vlSelfRef.test_copper_lists__DOT__dut__DOT__artist_inst__DOT__dy)))))) {
                            vlSelfRef.test_copper_lists__DOT__dut__DOT__artist_gfx_addr 
                                = (0x0000ffffU & (VL_MULS_III(32, (IData)(0x00000140U), 
                                                              (VL_EXTENDS_II(32,10, (IData)(vlSelfRef.test_copper_lists__DOT__dut__DOT__artist_inst__DOT__y)) 
                                                               + 
                                                               VL_EXTENDS_II(32,10, (IData)(vlSelfRef.test_copper_lists__DOT__dut__DOT__artist_inst__DOT__dy)))) 
                                                  + 
                                                  (VL_EXTENDS_II(16,10, (IData)(vlSelfRef.test_copper_lists__DOT__dut__DOT__artist_inst__DOT__x)) 
                                                   - 
                                                   VL_EXTENDS_II(16,10, (IData)(vlSelfRef.test_copper_lists__DOT__dut__DOT__artist_inst__DOT__dx)))));
                            vlSelfRef.test_copper_lists__DOT__dut__DOT__artist_gfx_wdata 
                                = vlSelfRef.test_copper_lists__DOT__dut__DOT__artist_inst__DOT__color;
                            vlSelfRef.test_copper_lists__DOT__dut__DOT__artist_gfx_we = 1U;
                        }
                        __Vdly__test_copper_lists__DOT__dut__DOT__artist_inst__DOT__octant_count = 2U;
                    } else {
                        if ((((VL_LTES_III(32, 0U, 
                                           (VL_EXTENDS_II(32,10, (IData)(vlSelfRef.test_copper_lists__DOT__dut__DOT__artist_inst__DOT__x)) 
                                            + VL_EXTENDS_II(32,10, (IData)(vlSelfRef.test_copper_lists__DOT__dut__DOT__artist_inst__DOT__dx)))) 
                               & VL_GTS_III(32, 0x00000140U, 
                                            (VL_EXTENDS_II(32,10, (IData)(vlSelfRef.test_copper_lists__DOT__dut__DOT__artist_inst__DOT__x)) 
                                             + VL_EXTENDS_II(32,10, (IData)(vlSelfRef.test_copper_lists__DOT__dut__DOT__artist_inst__DOT__dx))))) 
                              & VL_LTES_III(32, 0U, 
                                            (VL_EXTENDS_II(32,10, (IData)(vlSelfRef.test_copper_lists__DOT__dut__DOT__artist_inst__DOT__y)) 
                                             + VL_EXTENDS_II(32,10, (IData)(vlSelfRef.test_copper_lists__DOT__dut__DOT__artist_inst__DOT__dy))))) 
                             & VL_GTS_III(32, 0x000000c8U, 
                                          (VL_EXTENDS_II(32,10, (IData)(vlSelfRef.test_copper_lists__DOT__dut__DOT__artist_inst__DOT__y)) 
                                           + VL_EXTENDS_II(32,10, (IData)(vlSelfRef.test_copper_lists__DOT__dut__DOT__artist_inst__DOT__dy)))))) {
                            vlSelfRef.test_copper_lists__DOT__dut__DOT__artist_gfx_addr 
                                = (0x0000ffffU & (VL_MULS_III(32, (IData)(0x00000140U), 
                                                              (VL_EXTENDS_II(32,10, (IData)(vlSelfRef.test_copper_lists__DOT__dut__DOT__artist_inst__DOT__y)) 
                                                               + 
                                                               VL_EXTENDS_II(32,10, (IData)(vlSelfRef.test_copper_lists__DOT__dut__DOT__artist_inst__DOT__dy)))) 
                                                  + 
                                                  (VL_EXTENDS_II(16,10, (IData)(vlSelfRef.test_copper_lists__DOT__dut__DOT__artist_inst__DOT__x)) 
                                                   + 
                                                   VL_EXTENDS_II(16,10, (IData)(vlSelfRef.test_copper_lists__DOT__dut__DOT__artist_inst__DOT__dx)))));
                            vlSelfRef.test_copper_lists__DOT__dut__DOT__artist_gfx_wdata 
                                = vlSelfRef.test_copper_lists__DOT__dut__DOT__artist_inst__DOT__color;
                            vlSelfRef.test_copper_lists__DOT__dut__DOT__artist_gfx_we = 1U;
                        }
                        __Vdly__test_copper_lists__DOT__dut__DOT__artist_inst__DOT__octant_count = 1U;
                    }
                } else {
                    __Vdly__test_copper_lists__DOT__dut__DOT__artist_busy = 0U;
                }
            } else if ((9U == (IData)(vlSelfRef.test_copper_lists__DOT__dut__DOT__artist_inst__DOT__op))) {
                if (vlSelfRef.test_copper_lists__DOT__dut__DOT__artist_inst__DOT__paint_init) {
                    __Vdly__test_copper_lists__DOT__dut__DOT__artist_inst__DOT__fill_target 
                        = vlSelfRef.test_copper_lists__DOT__dut__DOT__gfx_a_dout;
                    __Vdly__test_copper_lists__DOT__dut__DOT__artist_inst__DOT__paint_init = 0U;
                    __Vdly__test_copper_lists__DOT__dut__DOT__artist_inst__DOT__paint_phase = 0U;
                } else if ((4U & (IData)(vlSelfRef.test_copper_lists__DOT__dut__DOT__artist_inst__DOT__paint_phase))) {
                    if ((2U & (IData)(vlSelfRef.test_copper_lists__DOT__dut__DOT__artist_inst__DOT__paint_phase))) {
                        __Vdly__test_copper_lists__DOT__dut__DOT__artist_inst__DOT__paint_phase = 0U;
                    } else if ((1U & (IData)(vlSelfRef.test_copper_lists__DOT__dut__DOT__artist_inst__DOT__paint_phase))) {
                        if ((0x0200U > (IData)(vlSelfRef.test_copper_lists__DOT__dut__DOT__artist_inst__DOT__fill_sp))) {
                            vlSelfRef.test_copper_lists__DOT__dut__DOT__artist_inst__DOT__fs_a_addr 
                                = (0x000001ffU & (IData)(vlSelfRef.test_copper_lists__DOT__dut__DOT__artist_inst__DOT__fill_sp));
                            vlSelfRef.test_copper_lists__DOT__dut__DOT__artist_inst__DOT__fs_a_din 
                                = vlSelfRef.test_copper_lists__DOT__dut__DOT__artist_inst__DOT__push_neighbors
                                [vlSelfRef.test_copper_lists__DOT__dut__DOT__artist_inst__DOT__push_count];
                            vlSelfRef.test_copper_lists__DOT__dut__DOT__artist_inst__DOT__fs_a_we = 1U;
                            __Vdly__test_copper_lists__DOT__dut__DOT__artist_inst__DOT__fill_sp 
                                = (0x000003ffU & ((IData)(1U) 
                                                  + (IData)(vlSelfRef.test_copper_lists__DOT__dut__DOT__artist_inst__DOT__fill_sp)));
                        }
                        if ((3U == (IData)(vlSelfRef.test_copper_lists__DOT__dut__DOT__artist_inst__DOT__push_count))) {
                            __Vdly__test_copper_lists__DOT__dut__DOT__artist_inst__DOT__paint_phase = 0U;
                        } else {
                            vlSelfRef.test_copper_lists__DOT__dut__DOT__artist_inst__DOT__push_count 
                                = (3U & ((IData)(1U) 
                                         + (IData)(vlSelfRef.test_copper_lists__DOT__dut__DOT__artist_inst__DOT__push_count)));
                        }
                    } else if ((((IData)(vlSelfRef.test_copper_lists__DOT__dut__DOT__gfx_a_dout) 
                                 == (IData)(vlSelfRef.test_copper_lists__DOT__dut__DOT__artist_inst__DOT__fill_target)) 
                                & ((IData)(vlSelfRef.test_copper_lists__DOT__dut__DOT__artist_inst__DOT__fill_target) 
                                   != (IData)(vlSelfRef.test_copper_lists__DOT__dut__DOT__artist_inst__DOT__color)))) {
                        vlSelfRef.test_copper_lists__DOT__dut__DOT__artist_gfx_addr 
                            = (0x0000ffffU & (((IData)(0x00000140U) 
                                               * (IData)(vlSelfRef.test_copper_lists__DOT__dut__DOT__artist_inst__DOT__cy)) 
                                              + (IData)(vlSelfRef.test_copper_lists__DOT__dut__DOT__artist_inst__DOT__cx)));
                        vlSelfRef.test_copper_lists__DOT__dut__DOT__artist_gfx_wdata 
                            = vlSelfRef.test_copper_lists__DOT__dut__DOT__artist_inst__DOT__color;
                        vlSelfRef.test_copper_lists__DOT__dut__DOT__artist_gfx_we = 1U;
                        vlSelfRef.test_copper_lists__DOT__dut__DOT__artist_inst__DOT__push_count = 0U;
                        __Vdly__test_copper_lists__DOT__dut__DOT__artist_inst__DOT__paint_phase = 5U;
                        __VdlyVal__test_copper_lists__DOT__dut__DOT__artist_inst__DOT__push_neighbors__v0 
                            = (((IData)(vlSelfRef.test_copper_lists__DOT__dut__DOT__artist_inst__DOT__cy) 
                                << 9U) | (0x000001ffU 
                                          & ((IData)(1U) 
                                             + (IData)(vlSelfRef.test_copper_lists__DOT__dut__DOT__artist_inst__DOT__cx))));
                        __VdlySet__test_copper_lists__DOT__dut__DOT__artist_inst__DOT__push_neighbors__v0 = 1U;
                        __VdlyVal__test_copper_lists__DOT__dut__DOT__artist_inst__DOT__push_neighbors__v1 
                            = (((IData)(vlSelfRef.test_copper_lists__DOT__dut__DOT__artist_inst__DOT__cy) 
                                << 9U) | (0x000001ffU 
                                          & ((IData)(vlSelfRef.test_copper_lists__DOT__dut__DOT__artist_inst__DOT__cx) 
                                             - (IData)(1U))));
                        __VdlyVal__test_copper_lists__DOT__dut__DOT__artist_inst__DOT__push_neighbors__v2 
                            = ((0x0001fe00U & (((IData)(1U) 
                                                + (IData)(vlSelfRef.test_copper_lists__DOT__dut__DOT__artist_inst__DOT__cy)) 
                                               << 9U)) 
                               | (IData)(vlSelfRef.test_copper_lists__DOT__dut__DOT__artist_inst__DOT__cx));
                        __VdlyVal__test_copper_lists__DOT__dut__DOT__artist_inst__DOT__push_neighbors__v3 
                            = ((0x0001fe00U & (((IData)(vlSelfRef.test_copper_lists__DOT__dut__DOT__artist_inst__DOT__cy) 
                                                - (IData)(1U)) 
                                               << 9U)) 
                               | (IData)(vlSelfRef.test_copper_lists__DOT__dut__DOT__artist_inst__DOT__cx));
                    } else {
                        __Vdly__test_copper_lists__DOT__dut__DOT__artist_inst__DOT__paint_phase = 0U;
                    }
                } else if ((2U & (IData)(vlSelfRef.test_copper_lists__DOT__dut__DOT__artist_inst__DOT__paint_phase))) {
                    if ((1U & (IData)(vlSelfRef.test_copper_lists__DOT__dut__DOT__artist_inst__DOT__paint_phase))) {
                        __Vdly__test_copper_lists__DOT__dut__DOT__artist_inst__DOT__paint_phase = 4U;
                    } else {
                        __Vdly__test_copper_lists__DOT__dut__DOT__artist_inst__DOT__cx 
                            = (0x000001ffU & vlSelfRef.test_copper_lists__DOT__dut__DOT__artist_inst__DOT__fs_b_dout);
                        __Vdly__test_copper_lists__DOT__dut__DOT__artist_inst__DOT__cy 
                            = (0x000000ffU & (vlSelfRef.test_copper_lists__DOT__dut__DOT__artist_inst__DOT__fs_b_dout 
                                              >> 9U));
                        if (((0x0140U > (0x000001ffU 
                                         & vlSelfRef.test_copper_lists__DOT__dut__DOT__artist_inst__DOT__fs_b_dout)) 
                             & (0xc8U > (0x000000ffU 
                                         & (vlSelfRef.test_copper_lists__DOT__dut__DOT__artist_inst__DOT__fs_b_dout 
                                            >> 9U))))) {
                            vlSelfRef.test_copper_lists__DOT__dut__DOT__artist_gfx_raddr 
                                = (0x0000ffffU & (((IData)(0x00000140U) 
                                                   * 
                                                   (0x000000ffU 
                                                    & (vlSelfRef.test_copper_lists__DOT__dut__DOT__artist_inst__DOT__fs_b_dout 
                                                       >> 9U))) 
                                                  + 
                                                  (0x000001ffU 
                                                   & vlSelfRef.test_copper_lists__DOT__dut__DOT__artist_inst__DOT__fs_b_dout)));
                            vlSelfRef.test_copper_lists__DOT__dut__DOT__artist_gfx_re = 1U;
                            __Vdly__test_copper_lists__DOT__dut__DOT__artist_inst__DOT__paint_phase = 3U;
                        } else {
                            __Vdly__test_copper_lists__DOT__dut__DOT__artist_inst__DOT__paint_phase = 0U;
                        }
                    }
                } else if ((1U & (IData)(vlSelfRef.test_copper_lists__DOT__dut__DOT__artist_inst__DOT__paint_phase))) {
                    __Vdly__test_copper_lists__DOT__dut__DOT__artist_inst__DOT__paint_phase = 2U;
                } else if ((0U < (IData)(vlSelfRef.test_copper_lists__DOT__dut__DOT__artist_inst__DOT__fill_sp))) {
                    __Vdly__test_copper_lists__DOT__dut__DOT__artist_inst__DOT__fs_b_addr 
                        = (0x000001ffU & ((IData)(vlSelfRef.test_copper_lists__DOT__dut__DOT__artist_inst__DOT__fill_sp) 
                                          - (IData)(1U)));
                    __Vdly__test_copper_lists__DOT__dut__DOT__artist_inst__DOT__paint_phase = 1U;
                    __Vdly__test_copper_lists__DOT__dut__DOT__artist_inst__DOT__fill_sp 
                        = (0x000003ffU & ((IData)(vlSelfRef.test_copper_lists__DOT__dut__DOT__artist_inst__DOT__fill_sp) 
                                          - (IData)(1U)));
                } else {
                    __Vdly__test_copper_lists__DOT__dut__DOT__artist_busy = 0U;
                }
            } else if ((0x0aU == (IData)(vlSelfRef.test_copper_lists__DOT__dut__DOT__artist_inst__DOT__op))) {
                if ((0U == (IData)(vlSelfRef.test_copper_lists__DOT__dut__DOT__artist_inst__DOT__gt_font_pending))) {
                    if (((IData)(vlSelfRef.test_copper_lists__DOT__dut__DOT__artist_inst__DOT__gt_char_idx) 
                         < (IData)(vlSelfRef.test_copper_lists__DOT__dut__DOT__artist_inst__DOT__gt_len))) {
                        if ((1U & ((IData)(vlSelfRef.test_copper_lists__DOT__dut__DOT__artist_inst__DOT__gt_font_byte) 
                                   >> (7U & ((IData)(7U) 
                                             - (IData)(vlSelfRef.test_copper_lists__DOT__dut__DOT__artist_inst__DOT__gt_font_col)))))) {
                            if (((0x00000140U > (((IData)(vlSelfRef.test_copper_lists__DOT__dut__DOT__artist_inst__DOT__gt_pen_x) 
                                                  + 
                                                  ((IData)(vlSelfRef.test_copper_lists__DOT__dut__DOT__artist_inst__DOT__gt_font_col) 
                                                   * (IData)(vlSelfRef.test_copper_lists__DOT__dut__DOT__artist_inst__DOT__gt_scale))) 
                                                 + (IData)(vlSelfRef.test_copper_lists__DOT__dut__DOT__artist_inst__DOT__gt_scale_x))) 
                                 & (0x000000c8U > (
                                                   ((IData)(vlSelfRef.test_copper_lists__DOT__dut__DOT__artist_inst__DOT__gt_start_y) 
                                                    + 
                                                    ((IData)(vlSelfRef.test_copper_lists__DOT__dut__DOT__artist_inst__DOT__gt_font_row) 
                                                     * (IData)(vlSelfRef.test_copper_lists__DOT__dut__DOT__artist_inst__DOT__gt_scale))) 
                                                   + (IData)(vlSelfRef.test_copper_lists__DOT__dut__DOT__artist_inst__DOT__gt_scale_y))))) {
                                vlSelfRef.test_copper_lists__DOT__dut__DOT__artist_gfx_addr 
                                    = (0x0000ffffU 
                                       & (((IData)(0x00000140U) 
                                           * (((IData)(vlSelfRef.test_copper_lists__DOT__dut__DOT__artist_inst__DOT__gt_start_y) 
                                               + ((IData)(vlSelfRef.test_copper_lists__DOT__dut__DOT__artist_inst__DOT__gt_font_row) 
                                                  * (IData)(vlSelfRef.test_copper_lists__DOT__dut__DOT__artist_inst__DOT__gt_scale))) 
                                              + (IData)(vlSelfRef.test_copper_lists__DOT__dut__DOT__artist_inst__DOT__gt_scale_y))) 
                                          + (((IData)(vlSelfRef.test_copper_lists__DOT__dut__DOT__artist_inst__DOT__gt_pen_x) 
                                              + ((IData)(vlSelfRef.test_copper_lists__DOT__dut__DOT__artist_inst__DOT__gt_font_col) 
                                                 * (IData)(vlSelfRef.test_copper_lists__DOT__dut__DOT__artist_inst__DOT__gt_scale))) 
                                             + (IData)(vlSelfRef.test_copper_lists__DOT__dut__DOT__artist_inst__DOT__gt_scale_x))));
                                vlSelfRef.test_copper_lists__DOT__dut__DOT__artist_gfx_wdata 
                                    = vlSelfRef.test_copper_lists__DOT__dut__DOT__artist_inst__DOT__color;
                                vlSelfRef.test_copper_lists__DOT__dut__DOT__artist_gfx_we = 1U;
                            }
                        }
                        if (((0x000000ffU & ((IData)(1U) 
                                             + (IData)(vlSelfRef.test_copper_lists__DOT__dut__DOT__artist_inst__DOT__gt_scale_x))) 
                             < (IData)(vlSelfRef.test_copper_lists__DOT__dut__DOT__artist_inst__DOT__gt_scale))) {
                            vlSelfRef.test_copper_lists__DOT__dut__DOT__artist_inst__DOT__gt_scale_x 
                                = (0x000000ffU & ((IData)(1U) 
                                                  + (IData)(vlSelfRef.test_copper_lists__DOT__dut__DOT__artist_inst__DOT__gt_scale_x)));
                        } else {
                            vlSelfRef.test_copper_lists__DOT__dut__DOT__artist_inst__DOT__gt_scale_x = 0U;
                            if ((7U > (IData)(vlSelfRef.test_copper_lists__DOT__dut__DOT__artist_inst__DOT__gt_font_col))) {
                                vlSelfRef.test_copper_lists__DOT__dut__DOT__artist_inst__DOT__gt_font_col 
                                    = (7U & ((IData)(1U) 
                                             + (IData)(vlSelfRef.test_copper_lists__DOT__dut__DOT__artist_inst__DOT__gt_font_col)));
                            } else {
                                vlSelfRef.test_copper_lists__DOT__dut__DOT__artist_inst__DOT__gt_font_col = 0U;
                                if (((0x000000ffU & 
                                      ((IData)(1U) 
                                       + (IData)(vlSelfRef.test_copper_lists__DOT__dut__DOT__artist_inst__DOT__gt_scale_y))) 
                                     < (IData)(vlSelfRef.test_copper_lists__DOT__dut__DOT__artist_inst__DOT__gt_scale))) {
                                    vlSelfRef.test_copper_lists__DOT__dut__DOT__artist_inst__DOT__gt_scale_y 
                                        = (0x000000ffU 
                                           & ((IData)(1U) 
                                              + (IData)(vlSelfRef.test_copper_lists__DOT__dut__DOT__artist_inst__DOT__gt_scale_y)));
                                } else {
                                    vlSelfRef.test_copper_lists__DOT__dut__DOT__artist_inst__DOT__gt_scale_y = 0U;
                                    if ((7U > (IData)(vlSelfRef.test_copper_lists__DOT__dut__DOT__artist_inst__DOT__gt_font_row))) {
                                        __Vdly__test_copper_lists__DOT__dut__DOT__artist_inst__DOT__gt_font_row 
                                            = (7U & 
                                               ((IData)(1U) 
                                                + (IData)(vlSelfRef.test_copper_lists__DOT__dut__DOT__artist_inst__DOT__gt_font_row)));
                                        vlSelfRef.test_copper_lists__DOT__dut__DOT__artist_font_addr 
                                            = (((IData)(vlSelfRef.test_copper_lists__DOT__dut__DOT__artist_inst__DOT__gt_cur_char) 
                                                << 3U) 
                                               | (7U 
                                                  & ((IData)(1U) 
                                                     + (IData)(vlSelfRef.test_copper_lists__DOT__dut__DOT__artist_inst__DOT__gt_font_row))));
                                        vlSelfRef.test_copper_lists__DOT__dut__DOT__artist_font_re = 1U;
                                        __Vdly__test_copper_lists__DOT__dut__DOT__artist_inst__DOT__gt_font_pending = 1U;
                                    } else {
                                        __Vdly__test_copper_lists__DOT__dut__DOT__artist_inst__DOT__gt_char_idx 
                                            = (0x0000003fU 
                                               & ((IData)(1U) 
                                                  + (IData)(vlSelfRef.test_copper_lists__DOT__dut__DOT__artist_inst__DOT__gt_char_idx)));
                                        vlSelfRef.test_copper_lists__DOT__dut__DOT__artist_inst__DOT__gt_pen_x 
                                            = (0x000001ffU 
                                               & ((IData)(vlSelfRef.test_copper_lists__DOT__dut__DOT__artist_inst__DOT__gt_pen_x) 
                                                  + 
                                                  (0x0000ffffU 
                                                   & VL_SHIFTL_III(16,16,32, (IData)(vlSelfRef.test_copper_lists__DOT__dut__DOT__artist_inst__DOT__gt_scale), 3U))));
                                        __Vdly__test_copper_lists__DOT__dut__DOT__artist_inst__DOT__gt_font_row = 0U;
                                        if ((((IData)(1U) 
                                              + (IData)(vlSelfRef.test_copper_lists__DOT__dut__DOT__artist_inst__DOT__gt_char_idx)) 
                                             < (IData)(vlSelfRef.test_copper_lists__DOT__dut__DOT__artist_inst__DOT__gt_len))) {
                                            __Vfunc_test_copper_lists__DOT__dut__DOT__artist_inst__DOT__gt_char__176__idx 
                                                = (0x0000003fU 
                                                   & ((IData)(1U) 
                                                      + (IData)(vlSelfRef.test_copper_lists__DOT__dut__DOT__artist_inst__DOT__gt_char_idx)));
                                            __Vfunc_test_copper_lists__DOT__dut__DOT__artist_inst__DOT__gt_char__176__Vfuncout 
                                                = (0x000000ffU 
                                                   & (((0U 
                                                        == 
                                                        (0x0000001fU 
                                                         & VL_SHIFTL_III(9,32,32, (IData)(__Vfunc_test_copper_lists__DOT__dut__DOT__artist_inst__DOT__gt_char__176__idx), 3U)))
                                                        ? 0U
                                                        : 
                                                       (vlSelfRef.test_copper_lists__DOT__dut__DOT__fio_name_flat
                                                        [
                                                        (((IData)(7U) 
                                                          + 
                                                          (0x000001ffU 
                                                           & VL_SHIFTL_III(9,32,32, (IData)(__Vfunc_test_copper_lists__DOT__dut__DOT__artist_inst__DOT__gt_char__176__idx), 3U))) 
                                                         >> 5U)] 
                                                        << 
                                                        ((IData)(0x00000020U) 
                                                         - 
                                                         (0x0000001fU 
                                                          & VL_SHIFTL_III(9,32,32, (IData)(__Vfunc_test_copper_lists__DOT__dut__DOT__artist_inst__DOT__gt_char__176__idx), 3U))))) 
                                                      | (vlSelfRef.test_copper_lists__DOT__dut__DOT__fio_name_flat
                                                         [
                                                         (0x0000000fU 
                                                          & (VL_SHIFTL_III(9,32,32, (IData)(__Vfunc_test_copper_lists__DOT__dut__DOT__artist_inst__DOT__gt_char__176__idx), 3U) 
                                                             >> 5U))] 
                                                         >> 
                                                         (0x0000001fU 
                                                          & VL_SHIFTL_III(9,32,32, (IData)(__Vfunc_test_copper_lists__DOT__dut__DOT__artist_inst__DOT__gt_char__176__idx), 3U)))));
                                            vlSelfRef.test_copper_lists__DOT__dut__DOT__artist_inst__DOT__gt_cur_char 
                                                = __Vfunc_test_copper_lists__DOT__dut__DOT__artist_inst__DOT__gt_char__176__Vfuncout;
                                            vlSelfRef.test_copper_lists__DOT__dut__DOT__artist_font_re = 1U;
                                            __Vdly__test_copper_lists__DOT__dut__DOT__artist_inst__DOT__gt_font_pending = 1U;
                                            vlSelfRef.test_copper_lists__DOT__dut__DOT__artist_font_addr 
                                                = (
                                                   ([&]() {
                                                        __Vfunc_test_copper_lists__DOT__dut__DOT__artist_inst__DOT__gt_char__177__idx 
                                                            = 
                                                            (0x0000003fU 
                                                             & ((IData)(1U) 
                                                                + (IData)(vlSelfRef.test_copper_lists__DOT__dut__DOT__artist_inst__DOT__gt_char_idx)));
                                                        __Vfunc_test_copper_lists__DOT__dut__DOT__artist_inst__DOT__gt_char__177__Vfuncout 
                                                            = 
                                                            (0x000000ffU 
                                                             & (((0U 
                                                                  == 
                                                                  (0x0000001fU 
                                                                   & VL_SHIFTL_III(9,32,32, (IData)(__Vfunc_test_copper_lists__DOT__dut__DOT__artist_inst__DOT__gt_char__177__idx), 3U)))
                                                                  ? 0U
                                                                  : 
                                                                 (vlSelfRef.test_copper_lists__DOT__dut__DOT__fio_name_flat
                                                                  [
                                                                  (((IData)(7U) 
                                                                    + 
                                                                    (0x000001ffU 
                                                                     & VL_SHIFTL_III(9,32,32, (IData)(__Vfunc_test_copper_lists__DOT__dut__DOT__artist_inst__DOT__gt_char__177__idx), 3U))) 
                                                                   >> 5U)] 
                                                                  << 
                                                                  ((IData)(0x00000020U) 
                                                                   - 
                                                                   (0x0000001fU 
                                                                    & VL_SHIFTL_III(9,32,32, (IData)(__Vfunc_test_copper_lists__DOT__dut__DOT__artist_inst__DOT__gt_char__177__idx), 3U))))) 
                                                                | (vlSelfRef.test_copper_lists__DOT__dut__DOT__fio_name_flat
                                                                   [
                                                                   (0x0000000fU 
                                                                    & (VL_SHIFTL_III(9,32,32, (IData)(__Vfunc_test_copper_lists__DOT__dut__DOT__artist_inst__DOT__gt_char__177__idx), 3U) 
                                                                       >> 5U))] 
                                                                   >> 
                                                                   (0x0000001fU 
                                                                    & VL_SHIFTL_III(9,32,32, (IData)(__Vfunc_test_copper_lists__DOT__dut__DOT__artist_inst__DOT__gt_char__177__idx), 3U)))));
                                                    }(), (IData)(__Vfunc_test_copper_lists__DOT__dut__DOT__artist_inst__DOT__gt_char__177__Vfuncout)) 
                                                   << 3U);
                                        }
                                    }
                                }
                            }
                        }
                    } else {
                        __Vdly__test_copper_lists__DOT__dut__DOT__artist_busy = 0U;
                    }
                }
            } else {
                __Vdly__test_copper_lists__DOT__dut__DOT__artist_busy = 0U;
            }
        }
        if (((IData)(vlSelfRef.test_copper_lists__DOT__dut__DOT__artist_cmd_valid) 
             & (~ (IData)(vlSelfRef.test_copper_lists__DOT__dut__DOT__artist_busy)))) {
            vlSelfRef.test_copper_lists__DOT__dut__DOT__artist_inst__DOT__color 
                = vlSelfRef.test_copper_lists__DOT__dut__DOT__gfx_color;
            if (((((((((1U == (IData)(vlSelfRef.test_copper_lists__DOT__dut__DOT__artist_cmd_code)) 
                       | (2U == (IData)(vlSelfRef.test_copper_lists__DOT__dut__DOT__artist_cmd_code))) 
                      | (3U == (IData)(vlSelfRef.test_copper_lists__DOT__dut__DOT__artist_cmd_code))) 
                     | ((5U == (IData)(vlSelfRef.test_copper_lists__DOT__dut__DOT__artist_cmd_code)) 
                        || (6U == (IData)(vlSelfRef.test_copper_lists__DOT__dut__DOT__artist_cmd_code)))) 
                    | (7U == (IData)(vlSelfRef.test_copper_lists__DOT__dut__DOT__artist_cmd_code))) 
                   | (4U == (IData)(vlSelfRef.test_copper_lists__DOT__dut__DOT__artist_cmd_code))) 
                  | (9U == (IData)(vlSelfRef.test_copper_lists__DOT__dut__DOT__artist_cmd_code))) 
                 | (0x0aU == (IData)(vlSelfRef.test_copper_lists__DOT__dut__DOT__artist_cmd_code)))) {
                if ((1U == (IData)(vlSelfRef.test_copper_lists__DOT__dut__DOT__artist_cmd_code))) {
                    if (((0x0140U > (IData)(vlSelfRef.test_copper_lists__DOT__dut__DOT____Vcellinp__artist_inst__cmd_x0)) 
                         & (0xc8U > vlSelfRef.test_copper_lists__DOT__dut__DOT__regs[19U]))) {
                        vlSelfRef.test_copper_lists__DOT__dut__DOT__artist_gfx_addr 
                            = (0x0000ffffU & (((IData)(0x00000140U) 
                                               * vlSelfRef.test_copper_lists__DOT__dut__DOT__regs[19U]) 
                                              + (IData)(vlSelfRef.test_copper_lists__DOT__dut__DOT____Vcellinp__artist_inst__cmd_x0)));
                        vlSelfRef.test_copper_lists__DOT__dut__DOT__artist_gfx_wdata 
                            = vlSelfRef.test_copper_lists__DOT__dut__DOT__gfx_color;
                        vlSelfRef.test_copper_lists__DOT__dut__DOT__artist_gfx_we = 1U;
                    }
                } else if ((2U == (IData)(vlSelfRef.test_copper_lists__DOT__dut__DOT__artist_cmd_code))) {
                    if (((0x0140U > (IData)(vlSelfRef.test_copper_lists__DOT__dut__DOT____Vcellinp__artist_inst__cmd_x0)) 
                         & (0xc8U > vlSelfRef.test_copper_lists__DOT__dut__DOT__regs[19U]))) {
                        vlSelfRef.test_copper_lists__DOT__dut__DOT__artist_gfx_addr 
                            = (0x0000ffffU & (((IData)(0x00000140U) 
                                               * vlSelfRef.test_copper_lists__DOT__dut__DOT__regs[19U]) 
                                              + (IData)(vlSelfRef.test_copper_lists__DOT__dut__DOT____Vcellinp__artist_inst__cmd_x0)));
                        vlSelfRef.test_copper_lists__DOT__dut__DOT__artist_gfx_wdata = 0U;
                        vlSelfRef.test_copper_lists__DOT__dut__DOT__artist_gfx_we = 1U;
                    }
                } else if ((3U == (IData)(vlSelfRef.test_copper_lists__DOT__dut__DOT__artist_cmd_code))) {
                    __Vdly__test_copper_lists__DOT__dut__DOT__artist_inst__DOT__x 
                        = vlSelfRef.test_copper_lists__DOT__dut__DOT____Vcellinp__artist_inst__cmd_x0;
                    __Vdly__test_copper_lists__DOT__dut__DOT__artist_inst__DOT__y 
                        = vlSelfRef.test_copper_lists__DOT__dut__DOT__regs[19U];
                    if ((vlSelfRef.test_copper_lists__DOT__dut__DOT__regs[23U] 
                         >= vlSelfRef.test_copper_lists__DOT__dut__DOT__regs[19U])) {
                        __Vdly__test_copper_lists__DOT__dut__DOT__artist_inst__DOT__dy 
                            = (0x000003ffU & (- (vlSelfRef.test_copper_lists__DOT__dut__DOT__regs[23U] 
                                                 - vlSelfRef.test_copper_lists__DOT__dut__DOT__regs[19U])));
                        vlSelfRef.test_copper_lists__DOT__dut__DOT__artist_inst__DOT__sy = 1U;
                    } else {
                        __Vdly__test_copper_lists__DOT__dut__DOT__artist_inst__DOT__dy 
                            = (0x000003ffU & (vlSelfRef.test_copper_lists__DOT__dut__DOT__regs[19U] 
                                              - vlSelfRef.test_copper_lists__DOT__dut__DOT__regs[23U]));
                        vlSelfRef.test_copper_lists__DOT__dut__DOT__artist_inst__DOT__sy = 0x03ffU;
                    }
                    __Vdly__test_copper_lists__DOT__dut__DOT__artist_inst__DOT__err 
                        = (0x000003ffU & ((((IData)(vlSelfRef.test_copper_lists__DOT__dut__DOT____Vcellinp__artist_inst__cmd_x1) 
                                            >= (IData)(vlSelfRef.test_copper_lists__DOT__dut__DOT____Vcellinp__artist_inst__cmd_x0))
                                            ? ((IData)(vlSelfRef.test_copper_lists__DOT__dut__DOT____Vcellinp__artist_inst__cmd_x1) 
                                               - (IData)(vlSelfRef.test_copper_lists__DOT__dut__DOT____Vcellinp__artist_inst__cmd_x0))
                                            : ((IData)(vlSelfRef.test_copper_lists__DOT__dut__DOT____Vcellinp__artist_inst__cmd_x0) 
                                               - (IData)(vlSelfRef.test_copper_lists__DOT__dut__DOT____Vcellinp__artist_inst__cmd_x1))) 
                                          + ((vlSelfRef.test_copper_lists__DOT__dut__DOT__regs[23U] 
                                              >= vlSelfRef.test_copper_lists__DOT__dut__DOT__regs[19U])
                                              ? (- 
                                                 (vlSelfRef.test_copper_lists__DOT__dut__DOT__regs[23U] 
                                                  - vlSelfRef.test_copper_lists__DOT__dut__DOT__regs[19U]))
                                              : (vlSelfRef.test_copper_lists__DOT__dut__DOT__regs[19U] 
                                                 - vlSelfRef.test_copper_lists__DOT__dut__DOT__regs[23U]))));
                    vlSelfRef.test_copper_lists__DOT__dut__DOT__artist_inst__DOT__x2 
                        = vlSelfRef.test_copper_lists__DOT__dut__DOT____Vcellinp__artist_inst__cmd_x1;
                    vlSelfRef.test_copper_lists__DOT__dut__DOT__artist_inst__DOT__y2 
                        = vlSelfRef.test_copper_lists__DOT__dut__DOT__regs[23U];
                    if (((IData)(vlSelfRef.test_copper_lists__DOT__dut__DOT____Vcellinp__artist_inst__cmd_x1) 
                         >= (IData)(vlSelfRef.test_copper_lists__DOT__dut__DOT____Vcellinp__artist_inst__cmd_x0))) {
                        __Vdly__test_copper_lists__DOT__dut__DOT__artist_inst__DOT__dx 
                            = (0x000003ffU & ((IData)(vlSelfRef.test_copper_lists__DOT__dut__DOT____Vcellinp__artist_inst__cmd_x1) 
                                              - (IData)(vlSelfRef.test_copper_lists__DOT__dut__DOT____Vcellinp__artist_inst__cmd_x0)));
                        vlSelfRef.test_copper_lists__DOT__dut__DOT__artist_inst__DOT__sx = 1U;
                    } else {
                        __Vdly__test_copper_lists__DOT__dut__DOT__artist_inst__DOT__dx 
                            = (0x000003ffU & ((IData)(vlSelfRef.test_copper_lists__DOT__dut__DOT____Vcellinp__artist_inst__cmd_x0) 
                                              - (IData)(vlSelfRef.test_copper_lists__DOT__dut__DOT____Vcellinp__artist_inst__cmd_x1)));
                        vlSelfRef.test_copper_lists__DOT__dut__DOT__artist_inst__DOT__sx = 0x03ffU;
                    }
                    __Vdly__test_copper_lists__DOT__dut__DOT__artist_busy = 1U;
                    __Vdly__test_copper_lists__DOT__dut__DOT__artist_inst__DOT__op = 3U;
                } else if (((5U == (IData)(vlSelfRef.test_copper_lists__DOT__dut__DOT__artist_cmd_code)) 
                            || (6U == (IData)(vlSelfRef.test_copper_lists__DOT__dut__DOT__artist_cmd_code)))) {
                    __Vdly__test_copper_lists__DOT__dut__DOT__artist_inst__DOT__x 
                        = vlSelfRef.test_copper_lists__DOT__dut__DOT____Vcellinp__artist_inst__cmd_x0;
                    __Vdly__test_copper_lists__DOT__dut__DOT__artist_inst__DOT__y 
                        = vlSelfRef.test_copper_lists__DOT__dut__DOT__regs[19U];
                    __Vdly__test_copper_lists__DOT__dut__DOT__artist_inst__DOT__cx 
                        = vlSelfRef.test_copper_lists__DOT__dut__DOT____Vcellinp__artist_inst__cmd_x0;
                    __Vdly__test_copper_lists__DOT__dut__DOT__artist_inst__DOT__cy 
                        = vlSelfRef.test_copper_lists__DOT__dut__DOT__regs[19U];
                    vlSelfRef.test_copper_lists__DOT__dut__DOT__artist_inst__DOT__x2 
                        = vlSelfRef.test_copper_lists__DOT__dut__DOT____Vcellinp__artist_inst__cmd_x1;
                    vlSelfRef.test_copper_lists__DOT__dut__DOT__artist_inst__DOT__y2 
                        = vlSelfRef.test_copper_lists__DOT__dut__DOT__regs[23U];
                    __Vdly__test_copper_lists__DOT__dut__DOT__artist_busy = 1U;
                    __Vdly__test_copper_lists__DOT__dut__DOT__artist_inst__DOT__op 
                        = vlSelfRef.test_copper_lists__DOT__dut__DOT__artist_cmd_code;
                } else if ((7U == (IData)(vlSelfRef.test_copper_lists__DOT__dut__DOT__artist_cmd_code))) {
                    __Vdly__test_copper_lists__DOT__dut__DOT__artist_inst__DOT__cx = 0U;
                    __Vdly__test_copper_lists__DOT__dut__DOT__artist_inst__DOT__cy = 0U;
                    __Vdly__test_copper_lists__DOT__dut__DOT__artist_busy = 1U;
                    __Vdly__test_copper_lists__DOT__dut__DOT__artist_inst__DOT__op = 7U;
                } else if ((4U == (IData)(vlSelfRef.test_copper_lists__DOT__dut__DOT__artist_cmd_code))) {
                    __Vdly__test_copper_lists__DOT__dut__DOT__artist_inst__DOT__x 
                        = vlSelfRef.test_copper_lists__DOT__dut__DOT____Vcellinp__artist_inst__cmd_x0;
                    __Vdly__test_copper_lists__DOT__dut__DOT__artist_inst__DOT__y 
                        = vlSelfRef.test_copper_lists__DOT__dut__DOT__regs[19U];
                    __Vdly__test_copper_lists__DOT__dut__DOT__artist_inst__DOT__dy 
                        = vlSelfRef.test_copper_lists__DOT__dut__DOT____Vcellinp__artist_inst__cmd_x1;
                    __Vdly__test_copper_lists__DOT__dut__DOT__artist_inst__DOT__err 
                        = (0x000003ffU & ((IData)(1U) 
                                          - (IData)(vlSelfRef.test_copper_lists__DOT__dut__DOT____Vcellinp__artist_inst__cmd_x1)));
                    vlSelfRef.test_copper_lists__DOT__dut__DOT__artist_inst__DOT__x2 
                        = vlSelfRef.test_copper_lists__DOT__dut__DOT____Vcellinp__artist_inst__cmd_x1;
                    __Vdly__test_copper_lists__DOT__dut__DOT__artist_inst__DOT__dx = 0U;
                    __Vdly__test_copper_lists__DOT__dut__DOT__artist_inst__DOT__octant_count = 0U;
                    __Vdly__test_copper_lists__DOT__dut__DOT__artist_busy = 1U;
                    __Vdly__test_copper_lists__DOT__dut__DOT__artist_inst__DOT__op = 4U;
                } else if ((9U == (IData)(vlSelfRef.test_copper_lists__DOT__dut__DOT__artist_cmd_code))) {
                    __Vdly__test_copper_lists__DOT__dut__DOT__artist_inst__DOT__x 
                        = vlSelfRef.test_copper_lists__DOT__dut__DOT____Vcellinp__artist_inst__cmd_x0;
                    __Vdly__test_copper_lists__DOT__dut__DOT__artist_inst__DOT__y 
                        = vlSelfRef.test_copper_lists__DOT__dut__DOT__regs[19U];
                    if (((0x0140U > (IData)(vlSelfRef.test_copper_lists__DOT__dut__DOT____Vcellinp__artist_inst__cmd_x0)) 
                         & (0xc8U > vlSelfRef.test_copper_lists__DOT__dut__DOT__regs[19U]))) {
                        vlSelfRef.test_copper_lists__DOT__dut__DOT__artist_gfx_raddr 
                            = (0x0000ffffU & (((IData)(0x00000140U) 
                                               * vlSelfRef.test_copper_lists__DOT__dut__DOT__regs[19U]) 
                                              + (IData)(vlSelfRef.test_copper_lists__DOT__dut__DOT____Vcellinp__artist_inst__cmd_x0)));
                        vlSelfRef.test_copper_lists__DOT__dut__DOT__artist_gfx_re = 1U;
                        vlSelfRef.test_copper_lists__DOT__dut__DOT__artist_inst__DOT__fs_a_addr = 0U;
                        vlSelfRef.test_copper_lists__DOT__dut__DOT__artist_inst__DOT__fs_a_din 
                            = ((vlSelfRef.test_copper_lists__DOT__dut__DOT__regs[19U] 
                                << 9U) | (IData)(vlSelfRef.test_copper_lists__DOT__dut__DOT____Vcellinp__artist_inst__cmd_x0));
                        vlSelfRef.test_copper_lists__DOT__dut__DOT__artist_inst__DOT__fs_a_we = 1U;
                        __Vdly__test_copper_lists__DOT__dut__DOT__artist_inst__DOT__fill_sp = 1U;
                        __Vdly__test_copper_lists__DOT__dut__DOT__artist_inst__DOT__paint_phase = 3U;
                        __Vdly__test_copper_lists__DOT__dut__DOT__artist_inst__DOT__paint_init = 1U;
                        __Vdly__test_copper_lists__DOT__dut__DOT__artist_busy = 1U;
                        __Vdly__test_copper_lists__DOT__dut__DOT__artist_inst__DOT__op = 9U;
                    }
                } else if ((0U < (IData)(vlSelfRef.test_copper_lists__DOT__dut__DOT__fio_name_len))) {
                    __Vfunc_test_copper_lists__DOT__dut__DOT__artist_inst__DOT__gt_char__178__Vfuncout 
                        = (0x000000ffU & vlSelfRef.test_copper_lists__DOT__dut__DOT__fio_name_flat[0U]);
                    vlSelfRef.test_copper_lists__DOT__dut__DOT__artist_inst__DOT__gt_pen_x 
                        = vlSelfRef.test_copper_lists__DOT__dut__DOT____Vcellinp__artist_inst__cmd_x0;
                    vlSelfRef.test_copper_lists__DOT__dut__DOT__artist_inst__DOT__gt_start_y 
                        = vlSelfRef.test_copper_lists__DOT__dut__DOT__regs[19U];
                    vlSelfRef.test_copper_lists__DOT__dut__DOT__artist_inst__DOT__gt_scale 
                        = ((1U > vlSelfRef.test_copper_lists__DOT__dut__DOT__regs[22U])
                            ? 1U : vlSelfRef.test_copper_lists__DOT__dut__DOT__regs[22U]);
                    __Vdly__test_copper_lists__DOT__dut__DOT__artist_inst__DOT__gt_char_idx = 0U;
                    vlSelfRef.test_copper_lists__DOT__dut__DOT__artist_inst__DOT__gt_len 
                        = vlSelfRef.test_copper_lists__DOT__dut__DOT__fio_name_len;
                    __Vdly__test_copper_lists__DOT__dut__DOT__artist_inst__DOT__gt_font_row = 0U;
                    vlSelfRef.test_copper_lists__DOT__dut__DOT__artist_inst__DOT__gt_font_col = 0U;
                    vlSelfRef.test_copper_lists__DOT__dut__DOT__artist_inst__DOT__gt_scale_x = 0U;
                    vlSelfRef.test_copper_lists__DOT__dut__DOT__artist_inst__DOT__gt_scale_y = 0U;
                    vlSelfRef.test_copper_lists__DOT__dut__DOT__artist_inst__DOT__gt_cur_char 
                        = __Vfunc_test_copper_lists__DOT__dut__DOT__artist_inst__DOT__gt_char__178__Vfuncout;
                    vlSelfRef.test_copper_lists__DOT__dut__DOT__artist_font_addr 
                        = (([&]() {
                                __Vfunc_test_copper_lists__DOT__dut__DOT__artist_inst__DOT__gt_char__179__Vfuncout 
                                    = (0x000000ffU 
                                       & vlSelfRef.test_copper_lists__DOT__dut__DOT__fio_name_flat[0U]);
                            }(), (IData)(__Vfunc_test_copper_lists__DOT__dut__DOT__artist_inst__DOT__gt_char__179__Vfuncout)) 
                           << 3U);
                    vlSelfRef.test_copper_lists__DOT__dut__DOT__artist_font_re = 1U;
                    __Vdly__test_copper_lists__DOT__dut__DOT__artist_inst__DOT__gt_font_pending = 1U;
                    __Vdly__test_copper_lists__DOT__dut__DOT__artist_busy = 1U;
                    __Vdly__test_copper_lists__DOT__dut__DOT__artist_inst__DOT__op = 0x0aU;
                }
            }
        }
    }
    if ((1U & (~ (IData)(vlSelfRef.test_copper_lists__DOT__rst)))) {
        if (((IData)(vlSelfRef.test_copper_lists__DOT__cpu_we) 
             & (IData)(vlSelfRef.test_copper_lists__DOT__cpu_ce))) {
            if ((0xb9a3U == (IData)(vlSelfRef.test_copper_lists__DOT__cpu_addr))) {
                vlSelfRef.test_copper_lists__DOT__dut__DOT__fio_name_len 
                    = (0x0000003fU & (IData)(vlSelfRef.test_copper_lists__DOT__cpu_wdata));
            }
        }
    }
    vlSelfRef.test_copper_lists__DOT__dut__DOT__tile_inst__DOT__td_dout_a 
        = vlSelfRef.test_copper_lists__DOT__dut__DOT__tile_inst__DOT__tile_data_ram__DOT__mem
        [vlSelfRef.test_copper_lists__DOT__dut__DOT__tile_inst__DOT__td_addr_a];
    vlSelfRef.test_copper_lists__DOT__dut__DOT__tile_inst__DOT__blt_tile_rd_pending 
        = (((IData)(vlSelfRef.test_copper_lists__DOT__dut__DOT__vgc_tile_re) 
            & (~ (IData)(vlSelfRef.test_copper_lists__DOT__dut__DOT__vgc_tile_we))) 
           & (0U == (IData)(vlSelfRef.test_copper_lists__DOT__dut__DOT__tile_inst__DOT__dma_state)));
    vlSelfRef.test_copper_lists__DOT__dut__DOT__fio_name_flat[0U] 
        = ((0xff000000U & vlSelfRef.test_copper_lists__DOT__dut__DOT__fio_name_flat[0U]) 
           | ((vlSelfRef.test_copper_lists__DOT__dut__DOT__fio_name[2U] 
               << 0x00000010U) | ((vlSelfRef.test_copper_lists__DOT__dut__DOT__fio_name[1U] 
                                   << 8U) | vlSelfRef.test_copper_lists__DOT__dut__DOT__fio_name[0U])));
    vlSelfRef.test_copper_lists__DOT__dut__DOT__fio_name_flat[0U] 
        = ((0x00ffffffU & vlSelfRef.test_copper_lists__DOT__dut__DOT__fio_name_flat[0U]) 
           | (vlSelfRef.test_copper_lists__DOT__dut__DOT__fio_name[3U] 
              << 0x00000018U));
    vlSelfRef.test_copper_lists__DOT__dut__DOT__fio_name_flat[1U] 
        = ((0xffff0000U & vlSelfRef.test_copper_lists__DOT__dut__DOT__fio_name_flat[1U]) 
           | (0x00ffffffU & ((0x00ffff00U & (vlSelfRef.test_copper_lists__DOT__dut__DOT__fio_name[5U] 
                                             << 8U)) 
                             | ((0x00ffffffU & vlSelfRef.test_copper_lists__DOT__dut__DOT__fio_name[4U]) 
                                | (vlSelfRef.test_copper_lists__DOT__dut__DOT__fio_name[3U] 
                                   >> 8U)))));
    vlSelfRef.test_copper_lists__DOT__dut__DOT__fio_name_flat[1U] 
        = ((0x0000ffffU & vlSelfRef.test_copper_lists__DOT__dut__DOT__fio_name_flat[1U]) 
           | (0xffff0000U & ((vlSelfRef.test_copper_lists__DOT__dut__DOT__fio_name[7U] 
                              << 0x00000018U) | (vlSelfRef.test_copper_lists__DOT__dut__DOT__fio_name[6U] 
                                                 << 0x00000010U))));
    vlSelfRef.test_copper_lists__DOT__dut__DOT__fio_name_flat[2U] 
        = ((0xffffff00U & vlSelfRef.test_copper_lists__DOT__dut__DOT__fio_name_flat[2U]) 
           | (0x0000ffffU & ((0x0000ffffU & vlSelfRef.test_copper_lists__DOT__dut__DOT__fio_name[8U]) 
                             | ((0x0000ffffU & (vlSelfRef.test_copper_lists__DOT__dut__DOT__fio_name[7U] 
                                                >> 8U)) 
                                | (vlSelfRef.test_copper_lists__DOT__dut__DOT__fio_name[6U] 
                                   >> 0x00000010U)))));
    vlSelfRef.test_copper_lists__DOT__dut__DOT__fio_name_flat[2U] 
        = ((0x000000ffU & vlSelfRef.test_copper_lists__DOT__dut__DOT__fio_name_flat[2U]) 
           | (0xffffff00U & ((vlSelfRef.test_copper_lists__DOT__dut__DOT__fio_name[11U] 
                              << 0x00000018U) | ((vlSelfRef.test_copper_lists__DOT__dut__DOT__fio_name[10U] 
                                                  << 0x00000010U) 
                                                 | (vlSelfRef.test_copper_lists__DOT__dut__DOT__fio_name[9U] 
                                                    << 8U)))));
    vlSelfRef.test_copper_lists__DOT__dut__DOT__fio_name_flat[3U] 
        = ((0xff000000U & vlSelfRef.test_copper_lists__DOT__dut__DOT__fio_name_flat[3U]) 
           | ((vlSelfRef.test_copper_lists__DOT__dut__DOT__fio_name[14U] 
               << 0x00000010U) | ((vlSelfRef.test_copper_lists__DOT__dut__DOT__fio_name[13U] 
                                   << 8U) | vlSelfRef.test_copper_lists__DOT__dut__DOT__fio_name[12U])));
    vlSelfRef.test_copper_lists__DOT__dut__DOT__fio_name_flat[3U] 
        = ((0x00ffffffU & vlSelfRef.test_copper_lists__DOT__dut__DOT__fio_name_flat[3U]) 
           | (vlSelfRef.test_copper_lists__DOT__dut__DOT__fio_name[15U] 
              << 0x00000018U));
    vlSelfRef.test_copper_lists__DOT__dut__DOT__fio_name_flat[4U] 
        = ((0xffff0000U & vlSelfRef.test_copper_lists__DOT__dut__DOT__fio_name_flat[4U]) 
           | (0x00ffffffU & ((0x00ffff00U & (vlSelfRef.test_copper_lists__DOT__dut__DOT__fio_name[17U] 
                                             << 8U)) 
                             | ((0x00ffffffU & vlSelfRef.test_copper_lists__DOT__dut__DOT__fio_name[16U]) 
                                | (vlSelfRef.test_copper_lists__DOT__dut__DOT__fio_name[15U] 
                                   >> 8U)))));
    vlSelfRef.test_copper_lists__DOT__dut__DOT__fio_name_flat[4U] 
        = ((0x0000ffffU & vlSelfRef.test_copper_lists__DOT__dut__DOT__fio_name_flat[4U]) 
           | (0xffff0000U & ((vlSelfRef.test_copper_lists__DOT__dut__DOT__fio_name[19U] 
                              << 0x00000018U) | (vlSelfRef.test_copper_lists__DOT__dut__DOT__fio_name[18U] 
                                                 << 0x00000010U))));
    vlSelfRef.test_copper_lists__DOT__dut__DOT__fio_name_flat[5U] 
        = ((0xffffff00U & vlSelfRef.test_copper_lists__DOT__dut__DOT__fio_name_flat[5U]) 
           | (0x0000ffffU & ((0x0000ffffU & vlSelfRef.test_copper_lists__DOT__dut__DOT__fio_name[20U]) 
                             | ((0x0000ffffU & (vlSelfRef.test_copper_lists__DOT__dut__DOT__fio_name[19U] 
                                                >> 8U)) 
                                | (vlSelfRef.test_copper_lists__DOT__dut__DOT__fio_name[18U] 
                                   >> 0x00000010U)))));
    vlSelfRef.test_copper_lists__DOT__dut__DOT__fio_name_flat[5U] 
        = ((0x000000ffU & vlSelfRef.test_copper_lists__DOT__dut__DOT__fio_name_flat[5U]) 
           | (0xffffff00U & ((vlSelfRef.test_copper_lists__DOT__dut__DOT__fio_name[23U] 
                              << 0x00000018U) | ((vlSelfRef.test_copper_lists__DOT__dut__DOT__fio_name[22U] 
                                                  << 0x00000010U) 
                                                 | (vlSelfRef.test_copper_lists__DOT__dut__DOT__fio_name[21U] 
                                                    << 8U)))));
    vlSelfRef.test_copper_lists__DOT__dut__DOT__fio_name_flat[6U] 
        = ((0xff000000U & vlSelfRef.test_copper_lists__DOT__dut__DOT__fio_name_flat[6U]) 
           | ((vlSelfRef.test_copper_lists__DOT__dut__DOT__fio_name[26U] 
               << 0x00000010U) | ((vlSelfRef.test_copper_lists__DOT__dut__DOT__fio_name[25U] 
                                   << 8U) | vlSelfRef.test_copper_lists__DOT__dut__DOT__fio_name[24U])));
    vlSelfRef.test_copper_lists__DOT__dut__DOT__fio_name_flat[6U] 
        = ((0x00ffffffU & vlSelfRef.test_copper_lists__DOT__dut__DOT__fio_name_flat[6U]) 
           | (vlSelfRef.test_copper_lists__DOT__dut__DOT__fio_name[27U] 
              << 0x00000018U));
    vlSelfRef.test_copper_lists__DOT__dut__DOT__fio_name_flat[7U] 
        = ((0xffff0000U & vlSelfRef.test_copper_lists__DOT__dut__DOT__fio_name_flat[7U]) 
           | (0x00ffffffU & ((0x00ffff00U & (vlSelfRef.test_copper_lists__DOT__dut__DOT__fio_name[29U] 
                                             << 8U)) 
                             | ((0x00ffffffU & vlSelfRef.test_copper_lists__DOT__dut__DOT__fio_name[28U]) 
                                | (vlSelfRef.test_copper_lists__DOT__dut__DOT__fio_name[27U] 
                                   >> 8U)))));
    vlSelfRef.test_copper_lists__DOT__dut__DOT__fio_name_flat[7U] 
        = ((0x0000ffffU & vlSelfRef.test_copper_lists__DOT__dut__DOT__fio_name_flat[7U]) 
           | (0xffff0000U & ((vlSelfRef.test_copper_lists__DOT__dut__DOT__fio_name[31U] 
                              << 0x00000018U) | (vlSelfRef.test_copper_lists__DOT__dut__DOT__fio_name[30U] 
                                                 << 0x00000010U))));
    vlSelfRef.test_copper_lists__DOT__dut__DOT__fio_name_flat[8U] 
        = ((0xffffff00U & vlSelfRef.test_copper_lists__DOT__dut__DOT__fio_name_flat[8U]) 
           | (0x0000ffffU & ((0x0000ffffU & vlSelfRef.test_copper_lists__DOT__dut__DOT__fio_name[32U]) 
                             | ((0x0000ffffU & (vlSelfRef.test_copper_lists__DOT__dut__DOT__fio_name[31U] 
                                                >> 8U)) 
                                | (vlSelfRef.test_copper_lists__DOT__dut__DOT__fio_name[30U] 
                                   >> 0x00000010U)))));
    vlSelfRef.test_copper_lists__DOT__dut__DOT__fio_name_flat[8U] 
        = ((0x000000ffU & vlSelfRef.test_copper_lists__DOT__dut__DOT__fio_name_flat[8U]) 
           | (0xffffff00U & ((vlSelfRef.test_copper_lists__DOT__dut__DOT__fio_name[35U] 
                              << 0x00000018U) | ((vlSelfRef.test_copper_lists__DOT__dut__DOT__fio_name[34U] 
                                                  << 0x00000010U) 
                                                 | (vlSelfRef.test_copper_lists__DOT__dut__DOT__fio_name[33U] 
                                                    << 8U)))));
    vlSelfRef.test_copper_lists__DOT__dut__DOT__fio_name_flat[9U] 
        = ((0xff000000U & vlSelfRef.test_copper_lists__DOT__dut__DOT__fio_name_flat[9U]) 
           | ((vlSelfRef.test_copper_lists__DOT__dut__DOT__fio_name[38U] 
               << 0x00000010U) | ((vlSelfRef.test_copper_lists__DOT__dut__DOT__fio_name[37U] 
                                   << 8U) | vlSelfRef.test_copper_lists__DOT__dut__DOT__fio_name[36U])));
    vlSelfRef.test_copper_lists__DOT__dut__DOT__fio_name_flat[9U] 
        = ((0x00ffffffU & vlSelfRef.test_copper_lists__DOT__dut__DOT__fio_name_flat[9U]) 
           | (vlSelfRef.test_copper_lists__DOT__dut__DOT__fio_name[39U] 
              << 0x00000018U));
    vlSelfRef.test_copper_lists__DOT__dut__DOT__fio_name_flat[10U] 
        = ((0xffff0000U & vlSelfRef.test_copper_lists__DOT__dut__DOT__fio_name_flat[10U]) 
           | (0x00ffffffU & ((0x00ffff00U & (vlSelfRef.test_copper_lists__DOT__dut__DOT__fio_name[41U] 
                                             << 8U)) 
                             | ((0x00ffffffU & vlSelfRef.test_copper_lists__DOT__dut__DOT__fio_name[40U]) 
                                | (vlSelfRef.test_copper_lists__DOT__dut__DOT__fio_name[39U] 
                                   >> 8U)))));
    vlSelfRef.test_copper_lists__DOT__dut__DOT__fio_name_flat[10U] 
        = ((0x0000ffffU & vlSelfRef.test_copper_lists__DOT__dut__DOT__fio_name_flat[10U]) 
           | (0xffff0000U & ((vlSelfRef.test_copper_lists__DOT__dut__DOT__fio_name[43U] 
                              << 0x00000018U) | (vlSelfRef.test_copper_lists__DOT__dut__DOT__fio_name[42U] 
                                                 << 0x00000010U))));
    vlSelfRef.test_copper_lists__DOT__dut__DOT__fio_name_flat[11U] 
        = ((0xffffff00U & vlSelfRef.test_copper_lists__DOT__dut__DOT__fio_name_flat[11U]) 
           | (0x0000ffffU & ((0x0000ffffU & vlSelfRef.test_copper_lists__DOT__dut__DOT__fio_name[44U]) 
                             | ((0x0000ffffU & (vlSelfRef.test_copper_lists__DOT__dut__DOT__fio_name[43U] 
                                                >> 8U)) 
                                | (vlSelfRef.test_copper_lists__DOT__dut__DOT__fio_name[42U] 
                                   >> 0x00000010U)))));
    vlSelfRef.test_copper_lists__DOT__dut__DOT__fio_name_flat[11U] 
        = ((0x000000ffU & vlSelfRef.test_copper_lists__DOT__dut__DOT__fio_name_flat[11U]) 
           | (0xffffff00U & ((vlSelfRef.test_copper_lists__DOT__dut__DOT__fio_name[47U] 
                              << 0x00000018U) | ((vlSelfRef.test_copper_lists__DOT__dut__DOT__fio_name[46U] 
                                                  << 0x00000010U) 
                                                 | (vlSelfRef.test_copper_lists__DOT__dut__DOT__fio_name[45U] 
                                                    << 8U)))));
    vlSelfRef.test_copper_lists__DOT__dut__DOT__fio_name_flat[12U] 
        = ((0xff000000U & vlSelfRef.test_copper_lists__DOT__dut__DOT__fio_name_flat[12U]) 
           | ((vlSelfRef.test_copper_lists__DOT__dut__DOT__fio_name[50U] 
               << 0x00000010U) | ((vlSelfRef.test_copper_lists__DOT__dut__DOT__fio_name[49U] 
                                   << 8U) | vlSelfRef.test_copper_lists__DOT__dut__DOT__fio_name[48U])));
    vlSelfRef.test_copper_lists__DOT__dut__DOT__fio_name_flat[12U] 
        = ((0x00ffffffU & vlSelfRef.test_copper_lists__DOT__dut__DOT__fio_name_flat[12U]) 
           | (vlSelfRef.test_copper_lists__DOT__dut__DOT__fio_name[51U] 
              << 0x00000018U));
    vlSelfRef.test_copper_lists__DOT__dut__DOT__fio_name_flat[13U] 
        = ((0xffff0000U & vlSelfRef.test_copper_lists__DOT__dut__DOT__fio_name_flat[13U]) 
           | (0x00ffffffU & ((0x00ffff00U & (vlSelfRef.test_copper_lists__DOT__dut__DOT__fio_name[53U] 
                                             << 8U)) 
                             | ((0x00ffffffU & vlSelfRef.test_copper_lists__DOT__dut__DOT__fio_name[52U]) 
                                | (vlSelfRef.test_copper_lists__DOT__dut__DOT__fio_name[51U] 
                                   >> 8U)))));
    vlSelfRef.test_copper_lists__DOT__dut__DOT__fio_name_flat[13U] 
        = ((0x0000ffffU & vlSelfRef.test_copper_lists__DOT__dut__DOT__fio_name_flat[13U]) 
           | (0xffff0000U & ((vlSelfRef.test_copper_lists__DOT__dut__DOT__fio_name[55U] 
                              << 0x00000018U) | (vlSelfRef.test_copper_lists__DOT__dut__DOT__fio_name[54U] 
                                                 << 0x00000010U))));
    vlSelfRef.test_copper_lists__DOT__dut__DOT__fio_name_flat[14U] 
        = ((0xffffff00U & vlSelfRef.test_copper_lists__DOT__dut__DOT__fio_name_flat[14U]) 
           | (0x0000ffffU & ((0x0000ffffU & vlSelfRef.test_copper_lists__DOT__dut__DOT__fio_name[56U]) 
                             | ((0x0000ffffU & (vlSelfRef.test_copper_lists__DOT__dut__DOT__fio_name[55U] 
                                                >> 8U)) 
                                | (vlSelfRef.test_copper_lists__DOT__dut__DOT__fio_name[54U] 
                                   >> 0x00000010U)))));
    vlSelfRef.test_copper_lists__DOT__dut__DOT__fio_name_flat[14U] 
        = ((0x000000ffU & vlSelfRef.test_copper_lists__DOT__dut__DOT__fio_name_flat[14U]) 
           | (0xffffff00U & ((vlSelfRef.test_copper_lists__DOT__dut__DOT__fio_name[59U] 
                              << 0x00000018U) | ((vlSelfRef.test_copper_lists__DOT__dut__DOT__fio_name[58U] 
                                                  << 0x00000010U) 
                                                 | (vlSelfRef.test_copper_lists__DOT__dut__DOT__fio_name[57U] 
                                                    << 8U)))));
    vlSelfRef.test_copper_lists__DOT__dut__DOT__fio_name_flat[15U] 
        = ((0xff000000U & vlSelfRef.test_copper_lists__DOT__dut__DOT__fio_name_flat[15U]) 
           | ((vlSelfRef.test_copper_lists__DOT__dut__DOT__fio_name[62U] 
               << 0x00000010U) | ((vlSelfRef.test_copper_lists__DOT__dut__DOT__fio_name[61U] 
                                   << 8U) | vlSelfRef.test_copper_lists__DOT__dut__DOT__fio_name[60U])));
    vlSelfRef.test_copper_lists__DOT__dut__DOT__fio_name_flat[15U] 
        = ((0x00ffffffU & vlSelfRef.test_copper_lists__DOT__dut__DOT__fio_name_flat[15U]) 
           | (vlSelfRef.test_copper_lists__DOT__dut__DOT__fio_name[63U] 
              << 0x00000018U));
    vlSelfRef.test_copper_lists__DOT__dut__DOT__artist_inst__DOT__op 
        = __Vdly__test_copper_lists__DOT__dut__DOT__artist_inst__DOT__op;
    vlSelfRef.test_copper_lists__DOT__dut__DOT__artist_inst__DOT__fill_sp 
        = __Vdly__test_copper_lists__DOT__dut__DOT__artist_inst__DOT__fill_sp;
    vlSelfRef.test_copper_lists__DOT__dut__DOT__artist_inst__DOT__octant_count 
        = __Vdly__test_copper_lists__DOT__dut__DOT__artist_inst__DOT__octant_count;
    vlSelfRef.test_copper_lists__DOT__dut__DOT__artist_inst__DOT__paint_phase 
        = __Vdly__test_copper_lists__DOT__dut__DOT__artist_inst__DOT__paint_phase;
    vlSelfRef.test_copper_lists__DOT__dut__DOT__artist_inst__DOT__gt_font_pending 
        = __Vdly__test_copper_lists__DOT__dut__DOT__artist_inst__DOT__gt_font_pending;
    vlSelfRef.test_copper_lists__DOT__dut__DOT__artist_inst__DOT__paint_init 
        = __Vdly__test_copper_lists__DOT__dut__DOT__artist_inst__DOT__paint_init;
    vlSelfRef.test_copper_lists__DOT__dut__DOT__artist_inst__DOT__gt_font_byte 
        = __Vdly__test_copper_lists__DOT__dut__DOT__artist_inst__DOT__gt_font_byte;
    vlSelfRef.test_copper_lists__DOT__dut__DOT__artist_inst__DOT__err 
        = __Vdly__test_copper_lists__DOT__dut__DOT__artist_inst__DOT__err;
    vlSelfRef.test_copper_lists__DOT__dut__DOT__artist_inst__DOT__x 
        = __Vdly__test_copper_lists__DOT__dut__DOT__artist_inst__DOT__x;
    vlSelfRef.test_copper_lists__DOT__dut__DOT__artist_inst__DOT__y 
        = __Vdly__test_copper_lists__DOT__dut__DOT__artist_inst__DOT__y;
    vlSelfRef.test_copper_lists__DOT__dut__DOT__artist_inst__DOT__cx 
        = __Vdly__test_copper_lists__DOT__dut__DOT__artist_inst__DOT__cx;
    vlSelfRef.test_copper_lists__DOT__dut__DOT__artist_inst__DOT__cy 
        = __Vdly__test_copper_lists__DOT__dut__DOT__artist_inst__DOT__cy;
    vlSelfRef.test_copper_lists__DOT__dut__DOT__artist_inst__DOT__dx 
        = __Vdly__test_copper_lists__DOT__dut__DOT__artist_inst__DOT__dx;
    vlSelfRef.test_copper_lists__DOT__dut__DOT__artist_inst__DOT__dy 
        = __Vdly__test_copper_lists__DOT__dut__DOT__artist_inst__DOT__dy;
    vlSelfRef.test_copper_lists__DOT__dut__DOT__artist_inst__DOT__fill_target 
        = __Vdly__test_copper_lists__DOT__dut__DOT__artist_inst__DOT__fill_target;
    vlSelfRef.test_copper_lists__DOT__dut__DOT__artist_inst__DOT__gt_font_row 
        = __Vdly__test_copper_lists__DOT__dut__DOT__artist_inst__DOT__gt_font_row;
    vlSelfRef.test_copper_lists__DOT__dut__DOT__artist_inst__DOT__gt_char_idx 
        = __Vdly__test_copper_lists__DOT__dut__DOT__artist_inst__DOT__gt_char_idx;
    if (__VdlySet__test_copper_lists__DOT__dut__DOT__artist_inst__DOT__push_neighbors__v0) {
        vlSelfRef.test_copper_lists__DOT__dut__DOT__artist_inst__DOT__push_neighbors[0U] 
            = __VdlyVal__test_copper_lists__DOT__dut__DOT__artist_inst__DOT__push_neighbors__v0;
        vlSelfRef.test_copper_lists__DOT__dut__DOT__artist_inst__DOT__push_neighbors[1U] 
            = __VdlyVal__test_copper_lists__DOT__dut__DOT__artist_inst__DOT__push_neighbors__v1;
        vlSelfRef.test_copper_lists__DOT__dut__DOT__artist_inst__DOT__push_neighbors[2U] 
            = __VdlyVal__test_copper_lists__DOT__dut__DOT__artist_inst__DOT__push_neighbors__v2;
        vlSelfRef.test_copper_lists__DOT__dut__DOT__artist_inst__DOT__push_neighbors[3U] 
            = __VdlyVal__test_copper_lists__DOT__dut__DOT__artist_inst__DOT__push_neighbors__v3;
    }
    if (__VdlySet__test_copper_lists__DOT__dut__DOT__tile_inst__DOT__tile_data_ram__DOT__mem__v0) {
        vlSelfRef.test_copper_lists__DOT__dut__DOT__tile_inst__DOT__tile_data_ram__DOT__mem[__VdlyDim0__test_copper_lists__DOT__dut__DOT__tile_inst__DOT__tile_data_ram__DOT__mem__v0] 
            = __VdlyVal__test_copper_lists__DOT__dut__DOT__tile_inst__DOT__tile_data_ram__DOT__mem__v0;
    }
    vlSelfRef.test_copper_lists__DOT__dut__DOT__tile_inst__DOT__dma_state 
        = __Vdly__test_copper_lists__DOT__dut__DOT__tile_inst__DOT__dma_state;
    if (vlSelfRef.test_copper_lists__DOT__rst) {
        vlSelfRef.test_copper_lists__DOT__dut__DOT__tile_inst__DOT__tile_size16 = 0U;
    } else if (((IData)(vlSelfRef.test_copper_lists__DOT__dut__DOT____Vcellinp__tile_inst__cpu_we) 
                & (IData)(vlSelfRef.test_copper_lists__DOT__dut__DOT__tile_reg_sel))) {
        if ((0U == (0x0000001fU & (IData)(vlSelfRef.test_copper_lists__DOT__cpu_addr)))) {
            vlSelfRef.test_copper_lists__DOT__dut__DOT__tile_inst__DOT__tile_size16 
                = (1U & (IData)(vlSelfRef.test_copper_lists__DOT__cpu_wdata));
        }
    }
    vlSelfRef.test_copper_lists__DOT__dut__DOT__tile_reg_sel 
        = ((0xa0c0U <= (IData)(vlSelfRef.test_copper_lists__DOT__cpu_addr)) 
           & (0xa0dfU >= (IData)(vlSelfRef.test_copper_lists__DOT__cpu_addr)));
    vlSelfRef.test_copper_lists__DOT__dut__DOT____Vcellinp__tile_inst__cpu_we 
        = ((IData)(vlSelfRef.test_copper_lists__DOT__cpu_ce) 
           & (IData)(vlSelfRef.test_copper_lists__DOT__cpu_we));
    vlSelfRef.test_copper_lists__DOT__dut__DOT__font_a_dout 
        = vlSelfRef.test_copper_lists__DOT__dut__DOT__text_inst__DOT__font_mem__DOT__mem
        [vlSelfRef.test_copper_lists__DOT__dut__DOT__font_a_addr];
    vlSelfRef.test_copper_lists__DOT__dut__DOT__artist_inst__DOT__fs_b_dout 
        = vlSelfRef.test_copper_lists__DOT__dut__DOT__artist_inst__DOT__fill_stack_mem__DOT__mem
        [vlSelfRef.test_copper_lists__DOT__dut__DOT__artist_inst__DOT__fs_b_addr];
    vlSelfRef.test_copper_lists__DOT__dut__DOT__tile_inst__DOT__at_we_a = 0U;
    vlSelfRef.test_copper_lists__DOT__dut__DOT__tile_inst__DOT__at_din_a = 0U;
    vlSelfRef.test_copper_lists__DOT__dut__DOT__tile_inst__DOT__nt_we_a = 0U;
    vlSelfRef.test_copper_lists__DOT__dut__DOT__tile_inst__DOT__nt_din_a = 0U;
    vlSelfRef.test_copper_lists__DOT__dut__DOT__tile_inst__DOT__at_addr_a = 0U;
    vlSelfRef.test_copper_lists__DOT__dut__DOT__tile_inst__DOT__nt_addr_a = 0U;
    if ((0x00000010U & (IData)(vlSelfRef.test_copper_lists__DOT__dut__DOT__tile_inst__DOT__dma_state))) {
        if (vlSelfRef.test_copper_lists__DOT__dut__DOT__tile_inst__DOT__cmd_at_we) {
            vlSelfRef.test_copper_lists__DOT__dut__DOT__tile_inst__DOT__at_we_a = 1U;
            vlSelfRef.test_copper_lists__DOT__dut__DOT__tile_inst__DOT__at_din_a 
                = vlSelfRef.test_copper_lists__DOT__dut__DOT__tile_inst__DOT__cmd_at_din;
            vlSelfRef.test_copper_lists__DOT__dut__DOT__tile_inst__DOT__at_addr_a 
                = vlSelfRef.test_copper_lists__DOT__dut__DOT__tile_inst__DOT__cmd_at_addr;
        }
        if (vlSelfRef.test_copper_lists__DOT__dut__DOT__tile_inst__DOT__cmd_nt_we) {
            vlSelfRef.test_copper_lists__DOT__dut__DOT__tile_inst__DOT__nt_we_a = 1U;
            vlSelfRef.test_copper_lists__DOT__dut__DOT__tile_inst__DOT__nt_din_a 
                = vlSelfRef.test_copper_lists__DOT__dut__DOT__tile_inst__DOT__cmd_nt_din;
            vlSelfRef.test_copper_lists__DOT__dut__DOT__tile_inst__DOT__nt_addr_a 
                = vlSelfRef.test_copper_lists__DOT__dut__DOT__tile_inst__DOT__cmd_nt_addr;
        }
    } else if ((8U & (IData)(vlSelfRef.test_copper_lists__DOT__dut__DOT__tile_inst__DOT__dma_state))) {
        if ((4U & (IData)(vlSelfRef.test_copper_lists__DOT__dut__DOT__tile_inst__DOT__dma_state))) {
            if ((2U & (IData)(vlSelfRef.test_copper_lists__DOT__dut__DOT__tile_inst__DOT__dma_state))) {
                if ((1U & (IData)(vlSelfRef.test_copper_lists__DOT__dut__DOT__tile_inst__DOT__dma_state))) {
                    if (vlSelfRef.test_copper_lists__DOT__dut__DOT__tile_inst__DOT__cmd_at_we) {
                        vlSelfRef.test_copper_lists__DOT__dut__DOT__tile_inst__DOT__at_we_a = 1U;
                        vlSelfRef.test_copper_lists__DOT__dut__DOT__tile_inst__DOT__at_din_a 
                            = vlSelfRef.test_copper_lists__DOT__dut__DOT__tile_inst__DOT__cmd_at_din;
                        vlSelfRef.test_copper_lists__DOT__dut__DOT__tile_inst__DOT__at_addr_a 
                            = vlSelfRef.test_copper_lists__DOT__dut__DOT__tile_inst__DOT__cmd_at_addr;
                    }
                    if (vlSelfRef.test_copper_lists__DOT__dut__DOT__tile_inst__DOT__cmd_nt_we) {
                        vlSelfRef.test_copper_lists__DOT__dut__DOT__tile_inst__DOT__nt_we_a = 1U;
                        vlSelfRef.test_copper_lists__DOT__dut__DOT__tile_inst__DOT__nt_din_a 
                            = vlSelfRef.test_copper_lists__DOT__dut__DOT__tile_inst__DOT__cmd_nt_din;
                        vlSelfRef.test_copper_lists__DOT__dut__DOT__tile_inst__DOT__nt_addr_a 
                            = vlSelfRef.test_copper_lists__DOT__dut__DOT__tile_inst__DOT__cmd_nt_addr;
                    }
                }
            } else {
                vlSelfRef.test_copper_lists__DOT__dut__DOT__tile_inst__DOT__at_addr_a 
                    = vlSelfRef.test_copper_lists__DOT__dut__DOT__tile_inst__DOT__cmd_at_addr;
                vlSelfRef.test_copper_lists__DOT__dut__DOT__tile_inst__DOT__nt_addr_a 
                    = vlSelfRef.test_copper_lists__DOT__dut__DOT__tile_inst__DOT__cmd_nt_addr;
            }
        } else {
            if ((1U & (~ ((IData)(vlSelfRef.test_copper_lists__DOT__dut__DOT__tile_inst__DOT__dma_state) 
                          >> 1U)))) {
                if ((1U & (IData)(vlSelfRef.test_copper_lists__DOT__dut__DOT__tile_inst__DOT__dma_state))) {
                    if ((0x0fa0U > (IData)(vlSelfRef.test_copper_lists__DOT__dut__DOT__tile_inst__DOT__dma_fill_addr))) {
                        vlSelfRef.test_copper_lists__DOT__dut__DOT__tile_inst__DOT__at_we_a = 1U;
                        vlSelfRef.test_copper_lists__DOT__dut__DOT__tile_inst__DOT__at_din_a = 0U;
                        vlSelfRef.test_copper_lists__DOT__dut__DOT__tile_inst__DOT__at_addr_a 
                            = vlSelfRef.test_copper_lists__DOT__dut__DOT__tile_inst__DOT__dma_fill_addr;
                    }
                } else if (vlSelfRef.test_copper_lists__DOT__dut__DOT__tile_inst__DOT__dma_data_valid) {
                    vlSelfRef.test_copper_lists__DOT__dut__DOT__tile_inst__DOT__at_we_a = 1U;
                    vlSelfRef.test_copper_lists__DOT__dut__DOT__tile_inst__DOT__at_din_a = 0U;
                    vlSelfRef.test_copper_lists__DOT__dut__DOT__tile_inst__DOT__at_addr_a 
                        = (0x00000fffU & ((((IData)(0x000003e8U) 
                                            * (IData)(vlSelfRef.test_copper_lists__DOT__dut__DOT__tile_inst__DOT__dma_nt)) 
                                           + ((IData)(0x0028U) 
                                              * (IData)(vlSelfRef.test_copper_lists__DOT__dut__DOT__tile_inst__DOT__dma_col_idx))) 
                                          + (IData)(vlSelfRef.test_copper_lists__DOT__dut__DOT__tile_inst__DOT__dma_col_row)));
                }
            }
            if ((2U & (IData)(vlSelfRef.test_copper_lists__DOT__dut__DOT__tile_inst__DOT__dma_state))) {
                if ((1U & (IData)(vlSelfRef.test_copper_lists__DOT__dut__DOT__tile_inst__DOT__dma_state))) {
                    if ((0x19U > (IData)(vlSelfRef.test_copper_lists__DOT__dut__DOT__tile_inst__DOT__dma_col_idx))) {
                        vlSelfRef.test_copper_lists__DOT__dut__DOT__tile_inst__DOT__nt_we_a = 1U;
                        vlSelfRef.test_copper_lists__DOT__dut__DOT__tile_inst__DOT__nt_din_a 
                            = ((0x18U >= (IData)(vlSelfRef.test_copper_lists__DOT__dut__DOT__tile_inst__DOT__dma_col_idx))
                                ? vlSelfRef.test_copper_lists__DOT__dut__DOT__tile_inst__DOT__col_buffer
                               [vlSelfRef.test_copper_lists__DOT__dut__DOT__tile_inst__DOT__dma_col_idx]
                                : 0U);
                        vlSelfRef.test_copper_lists__DOT__dut__DOT__tile_inst__DOT__nt_addr_a 
                            = (0x00000fffU & ((((IData)(0x000003e8U) 
                                                * (IData)(vlSelfRef.test_copper_lists__DOT__dut__DOT__tile_inst__DOT__dma_nt)) 
                                               + ((IData)(0x0028U) 
                                                  * (IData)(vlSelfRef.test_copper_lists__DOT__dut__DOT__tile_inst__DOT__dma_col_idx))) 
                                              + (IData)(vlSelfRef.test_copper_lists__DOT__dut__DOT__tile_inst__DOT__dma_col_row)));
                    }
                } else if ((0x03e8U > (IData)(vlSelfRef.test_copper_lists__DOT__dut__DOT__tile_inst__DOT__dma_count))) {
                    vlSelfRef.test_copper_lists__DOT__dut__DOT__tile_inst__DOT__nt_we_a = 1U;
                    vlSelfRef.test_copper_lists__DOT__dut__DOT__tile_inst__DOT__nt_din_a 
                        = vlSelfRef.test_copper_lists__DOT__dut__DOT__tile_inst__DOT__dma_fill_val;
                    vlSelfRef.test_copper_lists__DOT__dut__DOT__tile_inst__DOT__nt_addr_a 
                        = (0x00000fffU & (((IData)(0x000003e8U) 
                                           * (IData)(vlSelfRef.test_copper_lists__DOT__dut__DOT__tile_inst__DOT__dma_nt)) 
                                          + (IData)(vlSelfRef.test_copper_lists__DOT__dut__DOT__tile_inst__DOT__dma_count)));
                }
            } else if ((1U & (IData)(vlSelfRef.test_copper_lists__DOT__dut__DOT__tile_inst__DOT__dma_state))) {
                if ((0x0fa0U > (IData)(vlSelfRef.test_copper_lists__DOT__dut__DOT__tile_inst__DOT__dma_fill_addr))) {
                    vlSelfRef.test_copper_lists__DOT__dut__DOT__tile_inst__DOT__nt_we_a = 1U;
                    vlSelfRef.test_copper_lists__DOT__dut__DOT__tile_inst__DOT__nt_din_a = 0U;
                    vlSelfRef.test_copper_lists__DOT__dut__DOT__tile_inst__DOT__nt_addr_a 
                        = vlSelfRef.test_copper_lists__DOT__dut__DOT__tile_inst__DOT__dma_fill_addr;
                }
            }
        }
    } else if ((4U & (IData)(vlSelfRef.test_copper_lists__DOT__dut__DOT__tile_inst__DOT__dma_state))) {
        if ((2U & (IData)(vlSelfRef.test_copper_lists__DOT__dut__DOT__tile_inst__DOT__dma_state))) {
            if ((1U & (IData)(vlSelfRef.test_copper_lists__DOT__dut__DOT__tile_inst__DOT__dma_state))) {
                if (vlSelfRef.test_copper_lists__DOT__dut__DOT__tile_inst__DOT__dma_data_valid) {
                    vlSelfRef.test_copper_lists__DOT__dut__DOT__tile_inst__DOT__at_we_a = 1U;
                    vlSelfRef.test_copper_lists__DOT__dut__DOT__tile_inst__DOT__at_din_a = 0U;
                    vlSelfRef.test_copper_lists__DOT__dut__DOT__tile_inst__DOT__at_addr_a 
                        = (0x00000fffU & ((((IData)(0x000003e8U) 
                                            * (IData)(vlSelfRef.test_copper_lists__DOT__dut__DOT__tile_inst__DOT__dma_nt)) 
                                           + ((IData)(0x00000028U) 
                                              * (IData)(vlSelfRef.test_copper_lists__DOT__dut__DOT__tile_inst__DOT__dma_col_row))) 
                                          + (0x0000003fU 
                                             & (IData)(vlSelfRef.test_copper_lists__DOT__dut__DOT__tile_inst__DOT__dma_dst))));
                }
            }
            if ((1U & (~ (IData)(vlSelfRef.test_copper_lists__DOT__dut__DOT__tile_inst__DOT__dma_state)))) {
                if (vlSelfRef.test_copper_lists__DOT__dut__DOT__tile_inst__DOT__dma_data_valid) {
                    vlSelfRef.test_copper_lists__DOT__dut__DOT__tile_inst__DOT__nt_we_a = 1U;
                    vlSelfRef.test_copper_lists__DOT__dut__DOT__tile_inst__DOT__nt_din_a = 0U;
                    vlSelfRef.test_copper_lists__DOT__dut__DOT__tile_inst__DOT__nt_addr_a 
                        = (0x00000fffU & ((((IData)(0x000003e8U) 
                                            * (IData)(vlSelfRef.test_copper_lists__DOT__dut__DOT__tile_inst__DOT__dma_nt)) 
                                           + ((IData)(0x0028U) 
                                              * (IData)(vlSelfRef.test_copper_lists__DOT__dut__DOT__tile_inst__DOT__dma_col_idx))) 
                                          + (IData)(vlSelfRef.test_copper_lists__DOT__dut__DOT__tile_inst__DOT__dma_col_row)));
                }
            }
        } else if ((1U & (IData)(vlSelfRef.test_copper_lists__DOT__dut__DOT__tile_inst__DOT__dma_state))) {
            if (vlSelfRef.test_copper_lists__DOT__dut__DOT__tile_inst__DOT__dma_data_valid) {
                vlSelfRef.test_copper_lists__DOT__dut__DOT__tile_inst__DOT__nt_we_a = 1U;
                vlSelfRef.test_copper_lists__DOT__dut__DOT__tile_inst__DOT__nt_din_a = 0U;
                vlSelfRef.test_copper_lists__DOT__dut__DOT__tile_inst__DOT__nt_addr_a 
                    = (0x00000fffU & ((((IData)(0x000003e8U) 
                                        * (IData)(vlSelfRef.test_copper_lists__DOT__dut__DOT__tile_inst__DOT__dma_nt)) 
                                       + ((IData)(0x00000028U) 
                                          * (IData)(vlSelfRef.test_copper_lists__DOT__dut__DOT__tile_inst__DOT__dma_col_row))) 
                                      + (0x0000003fU 
                                         & (IData)(vlSelfRef.test_copper_lists__DOT__dut__DOT__tile_inst__DOT__dma_dst))));
            }
        }
    } else if ((2U & (IData)(vlSelfRef.test_copper_lists__DOT__dut__DOT__tile_inst__DOT__dma_state))) {
        if ((1U & (IData)(vlSelfRef.test_copper_lists__DOT__dut__DOT__tile_inst__DOT__dma_state))) {
            if (vlSelfRef.test_copper_lists__DOT__dut__DOT__tile_inst__DOT__dma_data_valid) {
                vlSelfRef.test_copper_lists__DOT__dut__DOT__tile_inst__DOT__at_we_a = 1U;
                vlSelfRef.test_copper_lists__DOT__dut__DOT__tile_inst__DOT__at_din_a = 0U;
                vlSelfRef.test_copper_lists__DOT__dut__DOT__tile_inst__DOT__at_addr_a 
                    = (0x00000fffU & (((IData)(0x000003e8U) 
                                       * (IData)(vlSelfRef.test_copper_lists__DOT__dut__DOT__tile_inst__DOT__dma_nt)) 
                                      + (0x000003ffU 
                                         & (IData)(vlSelfRef.test_copper_lists__DOT__dut__DOT__tile_inst__DOT__dma_dst))));
            }
        }
        if ((1U & (~ (IData)(vlSelfRef.test_copper_lists__DOT__dut__DOT__tile_inst__DOT__dma_state)))) {
            if (vlSelfRef.test_copper_lists__DOT__dut__DOT__tile_inst__DOT__dma_data_valid) {
                vlSelfRef.test_copper_lists__DOT__dut__DOT__tile_inst__DOT__nt_we_a = 1U;
                vlSelfRef.test_copper_lists__DOT__dut__DOT__tile_inst__DOT__nt_din_a = 0U;
                vlSelfRef.test_copper_lists__DOT__dut__DOT__tile_inst__DOT__nt_addr_a 
                    = (0x00000fffU & (((IData)(0x000003e8U) 
                                       * (IData)(vlSelfRef.test_copper_lists__DOT__dut__DOT__tile_inst__DOT__dma_nt)) 
                                      + (0x000003ffU 
                                         & (IData)(vlSelfRef.test_copper_lists__DOT__dut__DOT__tile_inst__DOT__dma_dst))));
            }
        }
    } else if ((1U & (~ (IData)(vlSelfRef.test_copper_lists__DOT__dut__DOT__tile_inst__DOT__dma_state)))) {
        if (vlSelfRef.test_copper_lists__DOT__dut__DOT__tile_inst__DOT__cmd_at_we) {
            vlSelfRef.test_copper_lists__DOT__dut__DOT__tile_inst__DOT__at_we_a = 1U;
            vlSelfRef.test_copper_lists__DOT__dut__DOT__tile_inst__DOT__at_din_a 
                = vlSelfRef.test_copper_lists__DOT__dut__DOT__tile_inst__DOT__cmd_at_din;
            vlSelfRef.test_copper_lists__DOT__dut__DOT__tile_inst__DOT__at_addr_a 
                = vlSelfRef.test_copper_lists__DOT__dut__DOT__tile_inst__DOT__cmd_at_addr;
        }
        if (vlSelfRef.test_copper_lists__DOT__dut__DOT__tile_inst__DOT__cmd_nt_we) {
            vlSelfRef.test_copper_lists__DOT__dut__DOT__tile_inst__DOT__nt_we_a = 1U;
            vlSelfRef.test_copper_lists__DOT__dut__DOT__tile_inst__DOT__nt_din_a 
                = vlSelfRef.test_copper_lists__DOT__dut__DOT__tile_inst__DOT__cmd_nt_din;
            vlSelfRef.test_copper_lists__DOT__dut__DOT__tile_inst__DOT__nt_addr_a 
                = vlSelfRef.test_copper_lists__DOT__dut__DOT__tile_inst__DOT__cmd_nt_addr;
        }
    }
    if (vlSelfRef.test_copper_lists__DOT__rst) {
        __Vdly__test_copper_lists__DOT__dut__DOT__cursor_x = 0U;
        __Vdly__test_copper_lists__DOT__dut__DOT__cursor_y = 0U;
        __Vdly__test_copper_lists__DOT__dut__DOT__fg_color = 1U;
        vlSelfRef.test_copper_lists__DOT__dut__DOT__bg_color = 6U;
        vlSelfRef.test_copper_lists__DOT__dut__DOT__gfx_color = 1U;
        __Vdly__test_copper_lists__DOT__dut__DOT__scroll_offset = 0U;
        __Vdly__test_copper_lists__DOT__dut__DOT__scroll_pending = 0U;
        __Vdly__test_copper_lists__DOT__dut__DOT__scroll_col = 0U;
        __Vdly__test_copper_lists__DOT__dut__DOT__cmd_busy = 0U;
        __Vdly__test_copper_lists__DOT__dut__DOT__copper_enabled = 0U;
        __Vdly__test_copper_lists__DOT__dut__DOT__copper_count = 0U;
        __Vdly__test_copper_lists__DOT__dut__DOT__copper_target_list = 0U;
        __Vdly__test_copper_lists__DOT__dut__DOT__copper_pending_list = 0U;
        __Vdly__test_copper_lists__DOT__dut__DOT__copper_loading = 0U;
        __Vdly__test_copper_lists__DOT__dut__DOT__sprrow_count = 0U;
        __Vdly__test_copper_lists__DOT__dut__DOT__sprcopy_phase = 0U;
        __Vdly__test_copper_lists__DOT__dut__DOT__memread_pending = 0U;
        vlSelfRef.test_copper_lists__DOT__dut__DOT__vgc_tile_we = 0U;
        vlSelfRef.test_copper_lists__DOT__dut__DOT__vgc_tile_re = 0U;
        vlSelfRef.test_copper_lists__DOT__dut__DOT__artist_cmd_valid = 0U;
        __VdlySet__test_copper_lists__DOT__dut__DOT__regs__v0 = 1U;
    } else {
        vlSelfRef.test_copper_lists__DOT__dut__DOT__cmd_char_we = 0U;
        vlSelfRef.test_copper_lists__DOT__dut__DOT__cmd_color_we = 0U;
        vlSelfRef.test_copper_lists__DOT__dut__DOT__cmd_gfx_we = 0U;
        vlSelfRef.test_copper_lists__DOT__dut__DOT__cmd_gfx_re = 0U;
        vlSelfRef.test_copper_lists__DOT__dut__DOT__cmd_spr_we = 0U;
        vlSelfRef.test_copper_lists__DOT__dut__DOT__cmd_spr_re = 0U;
        vlSelfRef.test_copper_lists__DOT__dut__DOT__artist_cmd_valid = 0U;
        if (vlSelfRef.test_copper_lists__DOT__dut__DOT__scroll_pending) {
            __Vfunc_test_copper_lists__DOT__dut__DOT__screen_addr__171__col 
                = vlSelfRef.test_copper_lists__DOT__dut__DOT__scroll_col;
            __Vfunc_test_copper_lists__DOT__dut__DOT__screen_addr__171__rr 
                = (0x0000001fU & ((IData)(0x18U) + (IData)(vlSelfRef.test_copper_lists__DOT__dut__DOT__scroll_offset)));
            if ((0x19U <= (IData)(__Vfunc_test_copper_lists__DOT__dut__DOT__screen_addr__171__rr))) {
                __Vfunc_test_copper_lists__DOT__dut__DOT__screen_addr__171__rr 
                    = (0x0000001fU & ((IData)(__Vfunc_test_copper_lists__DOT__dut__DOT__screen_addr__171__rr) 
                                      - (IData)(0x19U)));
            }
            vlSelfRef.test_copper_lists__DOT__dut__DOT__cmd_char_din = 0x20U;
            vlSelfRef.test_copper_lists__DOT__dut__DOT__cmd_char_we = 1U;
            vlSelfRef.test_copper_lists__DOT__dut__DOT__cmd_color_din 
                = vlSelfRef.test_copper_lists__DOT__dut__DOT__fg_color;
            vlSelfRef.test_copper_lists__DOT__dut__DOT__cmd_color_we = 1U;
            __Vfunc_test_copper_lists__DOT__dut__DOT__screen_addr__171__Vfuncout 
                = (0x000007ffU & (((IData)(0x00000050U) 
                                   * (IData)(__Vfunc_test_copper_lists__DOT__dut__DOT__screen_addr__171__rr)) 
                                  + (IData)(__Vfunc_test_copper_lists__DOT__dut__DOT__screen_addr__171__col)));
            vlSelfRef.test_copper_lists__DOT__dut__DOT__cmd_char_addr 
                = __Vfunc_test_copper_lists__DOT__dut__DOT__screen_addr__171__Vfuncout;
            __Vfunc_test_copper_lists__DOT__dut__DOT__screen_addr__172__col 
                = vlSelfRef.test_copper_lists__DOT__dut__DOT__scroll_col;
            __Vfunc_test_copper_lists__DOT__dut__DOT__screen_addr__172__rr 
                = (0x0000001fU & ((IData)(0x18U) + (IData)(vlSelfRef.test_copper_lists__DOT__dut__DOT__scroll_offset)));
            if ((0x19U <= (IData)(__Vfunc_test_copper_lists__DOT__dut__DOT__screen_addr__172__rr))) {
                __Vfunc_test_copper_lists__DOT__dut__DOT__screen_addr__172__rr 
                    = (0x0000001fU & ((IData)(__Vfunc_test_copper_lists__DOT__dut__DOT__screen_addr__172__rr) 
                                      - (IData)(0x19U)));
            }
            __Vfunc_test_copper_lists__DOT__dut__DOT__screen_addr__172__Vfuncout 
                = (0x000007ffU & (((IData)(0x00000050U) 
                                   * (IData)(__Vfunc_test_copper_lists__DOT__dut__DOT__screen_addr__172__rr)) 
                                  + (IData)(__Vfunc_test_copper_lists__DOT__dut__DOT__screen_addr__172__col)));
            vlSelfRef.test_copper_lists__DOT__dut__DOT__cmd_color_addr 
                = __Vfunc_test_copper_lists__DOT__dut__DOT__screen_addr__172__Vfuncout;
            if ((0x4fU == (IData)(vlSelfRef.test_copper_lists__DOT__dut__DOT__scroll_col))) {
                __Vdly__test_copper_lists__DOT__dut__DOT__scroll_pending = 0U;
                __Vdly__test_copper_lists__DOT__dut__DOT__scroll_col = 0U;
            } else {
                __Vdly__test_copper_lists__DOT__dut__DOT__scroll_col 
                    = (0x0000007fU & ((IData)(1U) + (IData)(vlSelfRef.test_copper_lists__DOT__dut__DOT__scroll_col)));
            }
        }
        vlSelfRef.test_copper_lists__DOT__dut__DOT__vgc_tile_we = 0U;
        vlSelfRef.test_copper_lists__DOT__dut__DOT__vgc_tile_re = 0U;
        if (vlSelfRef.test_copper_lists__DOT__dut__DOT__memread_pending) {
            if ((4U & (IData)(vlSelfRef.test_copper_lists__DOT__dut__DOT__memread_space))) {
                if ((1U & (~ ((IData)(vlSelfRef.test_copper_lists__DOT__dut__DOT__memread_space) 
                              >> 1U)))) {
                    if ((1U & (~ (IData)(vlSelfRef.test_copper_lists__DOT__dut__DOT__memread_space)))) {
                        __VdlyVal__test_copper_lists__DOT__dut__DOT__regs__v32 
                            = vlSelfRef.test_copper_lists__DOT__dut__DOT__tile_blt_rdata;
                        __VdlySet__test_copper_lists__DOT__dut__DOT__regs__v32 = 1U;
                    }
                }
            } else {
                __VdlyVal__test_copper_lists__DOT__dut__DOT__regs__v33 
                    = ((2U & (IData)(vlSelfRef.test_copper_lists__DOT__dut__DOT__memread_space))
                        ? ((1U & (IData)(vlSelfRef.test_copper_lists__DOT__dut__DOT__memread_space))
                            ? (IData)(vlSelfRef.test_copper_lists__DOT__dut__DOT__spr_a_dout)
                            : (IData)(vlSelfRef.test_copper_lists__DOT__dut__DOT__gfx_a_dout))
                        : ((1U & (IData)(vlSelfRef.test_copper_lists__DOT__dut__DOT__memread_space))
                            ? (IData)(vlSelfRef.test_copper_lists__DOT__dut__DOT__color_a_dout)
                            : (IData)(vlSelfRef.test_copper_lists__DOT__dut__DOT__char_a_dout)));
                __VdlySet__test_copper_lists__DOT__dut__DOT__regs__v33 = 1U;
            }
            __Vdly__test_copper_lists__DOT__dut__DOT__memread_pending = 0U;
        }
        if (vlSelfRef.test_copper_lists__DOT__dut__DOT__cmd_busy) {
            if ((0x12U == (IData)(vlSelfRef.test_copper_lists__DOT__dut__DOT__cmd_op))) {
                vlSelfRef.test_copper_lists__DOT__dut__DOT__cmd_spr_addr 
                    = ((0x00000780U & ((IData)(vlSelfRef.test_copper_lists__DOT__dut__DOT__cmd_x) 
                                       << 6U)) | ((0x00000078U 
                                                   & ((IData)(vlSelfRef.test_copper_lists__DOT__dut__DOT__cmd_cy) 
                                                      << 3U)) 
                                                  | (7U 
                                                     & (IData)(vlSelfRef.test_copper_lists__DOT__dut__DOT__cmd_cx))));
                vlSelfRef.test_copper_lists__DOT__dut__DOT__cmd_spr_din = 0U;
                vlSelfRef.test_copper_lists__DOT__dut__DOT__cmd_spr_we = 1U;
                if ((7U == (IData)(vlSelfRef.test_copper_lists__DOT__dut__DOT__cmd_cx))) {
                    __Vdly__test_copper_lists__DOT__dut__DOT__cmd_cx = 0U;
                    if ((0x0fU == (IData)(vlSelfRef.test_copper_lists__DOT__dut__DOT__cmd_cy))) {
                        __Vdly__test_copper_lists__DOT__dut__DOT__cmd_busy = 0U;
                    } else {
                        __Vdly__test_copper_lists__DOT__dut__DOT__cmd_cy 
                            = (0x000000ffU & ((IData)(1U) 
                                              + (IData)(vlSelfRef.test_copper_lists__DOT__dut__DOT__cmd_cy)));
                    }
                } else {
                    __Vdly__test_copper_lists__DOT__dut__DOT__cmd_cx 
                        = (0x000001ffU & ((IData)(1U) 
                                          + (IData)(vlSelfRef.test_copper_lists__DOT__dut__DOT__cmd_cx)));
                }
            } else if ((0xf0U == (IData)(vlSelfRef.test_copper_lists__DOT__dut__DOT__cmd_op))) {
                vlSelfRef.test_copper_lists__DOT__dut__DOT__cmd_char_addr 
                    = (0x000007ffU & (((IData)(0x00000050U) 
                                       * (0x0000001fU 
                                          & (IData)(vlSelfRef.test_copper_lists__DOT__dut__DOT__cmd_cy))) 
                                      + (0x0000007fU 
                                         & (IData)(vlSelfRef.test_copper_lists__DOT__dut__DOT__cmd_cx))));
                vlSelfRef.test_copper_lists__DOT__dut__DOT__cmd_char_din = 0x20U;
                vlSelfRef.test_copper_lists__DOT__dut__DOT__cmd_char_we = 1U;
                vlSelfRef.test_copper_lists__DOT__dut__DOT__cmd_color_din 
                    = vlSelfRef.test_copper_lists__DOT__dut__DOT__fg_color;
                vlSelfRef.test_copper_lists__DOT__dut__DOT__cmd_color_we = 1U;
                vlSelfRef.test_copper_lists__DOT__dut__DOT__cmd_color_addr 
                    = (0x000007ffU & (((IData)(0x00000050U) 
                                       * (0x0000001fU 
                                          & (IData)(vlSelfRef.test_copper_lists__DOT__dut__DOT__cmd_cy))) 
                                      + (0x0000007fU 
                                         & (IData)(vlSelfRef.test_copper_lists__DOT__dut__DOT__cmd_cx))));
                if ((0x004fU == (IData)(vlSelfRef.test_copper_lists__DOT__dut__DOT__cmd_cx))) {
                    __Vdly__test_copper_lists__DOT__dut__DOT__cmd_cx = 0U;
                    if ((0x18U == (IData)(vlSelfRef.test_copper_lists__DOT__dut__DOT__cmd_cy))) {
                        __Vdly__test_copper_lists__DOT__dut__DOT__cmd_busy = 0U;
                    } else {
                        __Vdly__test_copper_lists__DOT__dut__DOT__cmd_cy 
                            = (0x000000ffU & ((IData)(1U) 
                                              + (IData)(vlSelfRef.test_copper_lists__DOT__dut__DOT__cmd_cy)));
                    }
                } else {
                    __Vdly__test_copper_lists__DOT__dut__DOT__cmd_cx 
                        = (0x000001ffU & ((IData)(1U) 
                                          + (IData)(vlSelfRef.test_copper_lists__DOT__dut__DOT__cmd_cx)));
                }
            } else if ((0x13U == (IData)(vlSelfRef.test_copper_lists__DOT__dut__DOT__cmd_op))) {
                if (vlSelfRef.test_copper_lists__DOT__dut__DOT__sprcopy_phase) {
                    vlSelfRef.test_copper_lists__DOT__dut__DOT__cmd_spr_addr 
                        = ((0x00000780U & ((IData)(vlSelfRef.test_copper_lists__DOT__dut__DOT__cmd_x) 
                                           << 7U)) 
                           | ((0x00000078U & ((IData)(vlSelfRef.test_copper_lists__DOT__dut__DOT__cmd_cy) 
                                              << 3U)) 
                              | (7U & (IData)(vlSelfRef.test_copper_lists__DOT__dut__DOT__cmd_cx))));
                    vlSelfRef.test_copper_lists__DOT__dut__DOT__cmd_spr_din 
                        = vlSelfRef.test_copper_lists__DOT__dut__DOT__spr_a_dout;
                    vlSelfRef.test_copper_lists__DOT__dut__DOT__cmd_spr_we = 1U;
                    __Vdly__test_copper_lists__DOT__dut__DOT__sprcopy_phase = 0U;
                    if ((7U == (IData)(vlSelfRef.test_copper_lists__DOT__dut__DOT__cmd_cx))) {
                        __Vdly__test_copper_lists__DOT__dut__DOT__cmd_cx = 0U;
                        if ((0x0fU == (IData)(vlSelfRef.test_copper_lists__DOT__dut__DOT__cmd_cy))) {
                            __Vdly__test_copper_lists__DOT__dut__DOT__cmd_busy = 0U;
                        } else {
                            __Vdly__test_copper_lists__DOT__dut__DOT__cmd_cy 
                                = (0x000000ffU & ((IData)(1U) 
                                                  + (IData)(vlSelfRef.test_copper_lists__DOT__dut__DOT__cmd_cy)));
                        }
                    } else {
                        __Vdly__test_copper_lists__DOT__dut__DOT__cmd_cx 
                            = (0x000001ffU & ((IData)(1U) 
                                              + (IData)(vlSelfRef.test_copper_lists__DOT__dut__DOT__cmd_cx)));
                    }
                } else {
                    vlSelfRef.test_copper_lists__DOT__dut__DOT__cmd_spr_addr 
                        = ((0x00000780U & ((IData)(vlSelfRef.test_copper_lists__DOT__dut__DOT__cmd_y) 
                                           << 7U)) 
                           | ((0x00000078U & ((IData)(vlSelfRef.test_copper_lists__DOT__dut__DOT__cmd_cy) 
                                              << 3U)) 
                              | (7U & (IData)(vlSelfRef.test_copper_lists__DOT__dut__DOT__cmd_cx))));
                    vlSelfRef.test_copper_lists__DOT__dut__DOT__cmd_spr_re = 1U;
                    __Vdly__test_copper_lists__DOT__dut__DOT__sprcopy_phase = 1U;
                }
            } else if ((0x11U == (IData)(vlSelfRef.test_copper_lists__DOT__dut__DOT__cmd_op))) {
                vlSelfRef.test_copper_lists__DOT__dut__DOT__cmd_spr_addr 
                    = (((IData)(vlSelfRef.test_copper_lists__DOT__dut__DOT__sprrow_spr) 
                        << 7U) | (((IData)(vlSelfRef.test_copper_lists__DOT__dut__DOT__sprrow_row) 
                                   << 3U) | (IData)(vlSelfRef.test_copper_lists__DOT__dut__DOT__sprrow_count)));
                vlSelfRef.test_copper_lists__DOT__dut__DOT__cmd_spr_we = 1U;
                vlSelfRef.test_copper_lists__DOT__dut__DOT__cmd_spr_din 
                    = vlSelfRef.test_copper_lists__DOT__dut__DOT__sprrow_data
                    [vlSelfRef.test_copper_lists__DOT__dut__DOT__sprrow_count];
                if ((7U == (IData)(vlSelfRef.test_copper_lists__DOT__dut__DOT__sprrow_count))) {
                    __Vdly__test_copper_lists__DOT__dut__DOT__cmd_busy = 0U;
                } else {
                    __Vdly__test_copper_lists__DOT__dut__DOT__sprrow_count 
                        = (7U & ((IData)(1U) + (IData)(vlSelfRef.test_copper_lists__DOT__dut__DOT__sprrow_count)));
                }
            } else {
                __Vdly__test_copper_lists__DOT__dut__DOT__cmd_busy = 0U;
            }
        }
        if (vlSelfRef.test_copper_lists__DOT__dut__DOT__copper_loading) {
            if (((IData)(vlSelfRef.test_copper_lists__DOT__dut__DOT__copper_load_idx) 
                 < vlSelfRef.test_copper_lists__DOT__dut__DOT__copper_list_count
                 [vlSelfRef.test_copper_lists__DOT__dut__DOT__copper_load_src])) {
                __VdlyVal__test_copper_lists__DOT__dut__DOT__copper_pos__v0 
                    = vlSelfRef.test_copper_lists__DOT__dut__DOT__copper_list_pos
                    [(((IData)(vlSelfRef.test_copper_lists__DOT__dut__DOT__copper_load_src) 
                       << 5U) | (0x0000001fU & (IData)(vlSelfRef.test_copper_lists__DOT__dut__DOT__copper_load_idx)))];
                __VdlyDim0__test_copper_lists__DOT__dut__DOT__copper_pos__v0 
                    = (0x0000001fU & (IData)(vlSelfRef.test_copper_lists__DOT__dut__DOT__copper_load_idx));
                __VdlySet__test_copper_lists__DOT__dut__DOT__copper_pos__v0 = 1U;
                __VdlyVal__test_copper_lists__DOT__dut__DOT__copper_reg__v0 
                    = vlSelfRef.test_copper_lists__DOT__dut__DOT__copper_list_reg
                    [(((IData)(vlSelfRef.test_copper_lists__DOT__dut__DOT__copper_load_src) 
                       << 5U) | (0x0000001fU & (IData)(vlSelfRef.test_copper_lists__DOT__dut__DOT__copper_load_idx)))];
                __VdlyDim0__test_copper_lists__DOT__dut__DOT__copper_reg__v0 
                    = (0x0000001fU & (IData)(vlSelfRef.test_copper_lists__DOT__dut__DOT__copper_load_idx));
                __VdlyVal__test_copper_lists__DOT__dut__DOT__copper_val__v0 
                    = vlSelfRef.test_copper_lists__DOT__dut__DOT__copper_list_val
                    [(((IData)(vlSelfRef.test_copper_lists__DOT__dut__DOT__copper_load_src) 
                       << 5U) | (0x0000001fU & (IData)(vlSelfRef.test_copper_lists__DOT__dut__DOT__copper_load_idx)))];
                __VdlyDim0__test_copper_lists__DOT__dut__DOT__copper_val__v0 
                    = (0x0000001fU & (IData)(vlSelfRef.test_copper_lists__DOT__dut__DOT__copper_load_idx));
                vlSelfRef.test_copper_lists__DOT__dut__DOT__copper_load_idx 
                    = (0x000001ffU & ((IData)(1U) + (IData)(vlSelfRef.test_copper_lists__DOT__dut__DOT__copper_load_idx)));
            } else {
                __Vdly__test_copper_lists__DOT__dut__DOT__copper_count 
                    = vlSelfRef.test_copper_lists__DOT__dut__DOT__copper_list_count
                    [vlSelfRef.test_copper_lists__DOT__dut__DOT__copper_load_src];
                __Vdly__test_copper_lists__DOT__dut__DOT__copper_loading = 0U;
            }
        }
        if (((0U == (IData)(vlSelfRef.test_copper_lists__DOT__dut__DOT__h_count)) 
             & (0x01e0U == (IData)(vlSelfRef.test_copper_lists__DOT__dut__DOT__v_count)))) {
            if ((((IData)(vlSelfRef.test_copper_lists__DOT__dut__DOT__copper_pending_list) 
                  != (IData)(vlSelfRef.test_copper_lists__DOT__dut__DOT__copper_active_list)) 
                 & (~ (IData)(vlSelfRef.test_copper_lists__DOT__dut__DOT__copper_loading)))) {
                vlSelfRef.test_copper_lists__DOT__dut__DOT__copper_load_idx = 0U;
                __Vdly__test_copper_lists__DOT__dut__DOT__copper_active_list 
                    = vlSelfRef.test_copper_lists__DOT__dut__DOT__copper_pending_list;
                __Vdly__test_copper_lists__DOT__dut__DOT__copper_loading = 1U;
                vlSelfRef.test_copper_lists__DOT__dut__DOT__copper_load_src 
                    = vlSelfRef.test_copper_lists__DOT__dut__DOT__copper_pending_list;
            }
        }
        if (vlSelfRef.test_copper_lists__DOT__dut__DOT__copper_fire) {
            if ((1U == (IData)(vlSelfRef.test_copper_lists__DOT__dut__DOT__copper_fire_reg))) {
                vlSelfRef.test_copper_lists__DOT__dut__DOT__bg_color 
                    = (0x0000000fU & (IData)(vlSelfRef.test_copper_lists__DOT__dut__DOT__copper_fire_val));
            } else if ((2U == (IData)(vlSelfRef.test_copper_lists__DOT__dut__DOT__copper_fire_reg))) {
                __Vdly__test_copper_lists__DOT__dut__DOT__fg_color 
                    = (0x0000000fU & (IData)(vlSelfRef.test_copper_lists__DOT__dut__DOT__copper_fire_val));
            } else if ((0x0dU != (IData)(vlSelfRef.test_copper_lists__DOT__dut__DOT__copper_fire_reg))) {
                __VdlyVal__test_copper_lists__DOT__dut__DOT__regs__v34 
                    = vlSelfRef.test_copper_lists__DOT__dut__DOT__copper_fire_val;
                __VdlyDim0__test_copper_lists__DOT__dut__DOT__regs__v34 
                    = (0x0000001fU & (IData)(vlSelfRef.test_copper_lists__DOT__dut__DOT__copper_fire_reg));
                __VdlySet__test_copper_lists__DOT__dut__DOT__regs__v34 = 1U;
            }
        }
        if (((IData)(vlSelfRef.test_copper_lists__DOT__cpu_we) 
             & (IData)(vlSelfRef.test_copper_lists__DOT__cpu_ce))) {
            if (vlSelfRef.test_copper_lists__DOT__dut__DOT__vgc_reg_sel) {
                if (((((((((0U == (0x0000001fU & (IData)(vlSelfRef.test_copper_lists__DOT__cpu_addr))) 
                           | (1U == (0x0000001fU & (IData)(vlSelfRef.test_copper_lists__DOT__cpu_addr)))) 
                          | (2U == (0x0000001fU & (IData)(vlSelfRef.test_copper_lists__DOT__cpu_addr)))) 
                         | (3U == (0x0000001fU & (IData)(vlSelfRef.test_copper_lists__DOT__cpu_addr)))) 
                        | (4U == (0x0000001fU & (IData)(vlSelfRef.test_copper_lists__DOT__cpu_addr)))) 
                       | (5U == (0x0000001fU & (IData)(vlSelfRef.test_copper_lists__DOT__cpu_addr)))) 
                      | (6U == (0x0000001fU & (IData)(vlSelfRef.test_copper_lists__DOT__cpu_addr)))) 
                     | (7U == (0x0000001fU & (IData)(vlSelfRef.test_copper_lists__DOT__cpu_addr))))) {
                    if ((0U != (0x0000001fU & (IData)(vlSelfRef.test_copper_lists__DOT__cpu_addr)))) {
                        if ((1U == (0x0000001fU & (IData)(vlSelfRef.test_copper_lists__DOT__cpu_addr)))) {
                            vlSelfRef.test_copper_lists__DOT__dut__DOT__bg_color 
                                = (0x0000000fU & (IData)(vlSelfRef.test_copper_lists__DOT__cpu_wdata));
                        } else if ((2U == (0x0000001fU 
                                           & (IData)(vlSelfRef.test_copper_lists__DOT__cpu_addr)))) {
                            __Vdly__test_copper_lists__DOT__dut__DOT__fg_color 
                                = (0x0000000fU & (IData)(vlSelfRef.test_copper_lists__DOT__cpu_wdata));
                        } else if ((3U == (0x0000001fU 
                                           & (IData)(vlSelfRef.test_copper_lists__DOT__cpu_addr)))) {
                            __Vdly__test_copper_lists__DOT__dut__DOT__cursor_x 
                                = (0x0000007fU & (IData)(vlSelfRef.test_copper_lists__DOT__cpu_wdata));
                        } else if ((4U == (0x0000001fU 
                                           & (IData)(vlSelfRef.test_copper_lists__DOT__cpu_addr)))) {
                            __Vdly__test_copper_lists__DOT__dut__DOT__cursor_y 
                                = (0x0000001fU & (IData)(vlSelfRef.test_copper_lists__DOT__cpu_wdata));
                        }
                    }
                } else if (((((((((8U == (0x0000001fU 
                                          & (IData)(vlSelfRef.test_copper_lists__DOT__cpu_addr))) 
                                  | (0x0aU == (0x0000001fU 
                                               & (IData)(vlSelfRef.test_copper_lists__DOT__cpu_addr)))) 
                                 | (0x0bU == (0x0000001fU 
                                              & (IData)(vlSelfRef.test_copper_lists__DOT__cpu_addr)))) 
                                | (0x0cU == (0x0000001fU 
                                             & (IData)(vlSelfRef.test_copper_lists__DOT__cpu_addr)))) 
                               | (0x0dU == (0x0000001fU 
                                            & (IData)(vlSelfRef.test_copper_lists__DOT__cpu_addr)))) 
                              | (0x1fU == (0x0000001fU 
                                           & (IData)(vlSelfRef.test_copper_lists__DOT__cpu_addr)))) 
                             | (0x0eU == (0x0000001fU 
                                          & (IData)(vlSelfRef.test_copper_lists__DOT__cpu_addr)))) 
                            | (0x10U == (0x0000001fU 
                                         & (IData)(vlSelfRef.test_copper_lists__DOT__cpu_addr))))) {
                    if ((8U == (0x0000001fU & (IData)(vlSelfRef.test_copper_lists__DOT__cpu_addr)))) {
                        vlSelfRef.test_copper_lists__DOT__dut__DOT__gfx_color 
                            = (0x0000000fU & (IData)(vlSelfRef.test_copper_lists__DOT__cpu_wdata));
                    } else if ((0x0aU != (0x0000001fU 
                                          & (IData)(vlSelfRef.test_copper_lists__DOT__cpu_addr)))) {
                        if ((0x0bU != (0x0000001fU 
                                       & (IData)(vlSelfRef.test_copper_lists__DOT__cpu_addr)))) {
                            if ((0x0cU != (0x0000001fU 
                                           & (IData)(vlSelfRef.test_copper_lists__DOT__cpu_addr)))) {
                                if ((0x0dU != (0x0000001fU 
                                               & (IData)(vlSelfRef.test_copper_lists__DOT__cpu_addr)))) {
                                    if ((0x1fU != (0x0000001fU 
                                                   & (IData)(vlSelfRef.test_copper_lists__DOT__cpu_addr)))) {
                                        if ((0x0eU 
                                             == (0x0000001fU 
                                                 & (IData)(vlSelfRef.test_copper_lists__DOT__cpu_addr)))) {
                                            if ((8U 
                                                 == (IData)(vlSelfRef.test_copper_lists__DOT__cpu_wdata))) {
                                                if (
                                                    (0U 
                                                     < (IData)(vlSelfRef.test_copper_lists__DOT__dut__DOT__cursor_x))) {
                                                    __Vdly__test_copper_lists__DOT__dut__DOT__cursor_x 
                                                        = 
                                                        (0x0000007fU 
                                                         & ((IData)(vlSelfRef.test_copper_lists__DOT__dut__DOT__cursor_x) 
                                                            - (IData)(1U)));
                                                    __Vfunc_test_copper_lists__DOT__dut__DOT__screen_addr__173__col 
                                                        = 
                                                        (0x0000007fU 
                                                         & ((IData)(vlSelfRef.test_copper_lists__DOT__dut__DOT__cursor_x) 
                                                            - (IData)(1U)));
                                                } else {
                                                    __Vfunc_test_copper_lists__DOT__dut__DOT__screen_addr__173__col = 0U;
                                                }
                                                vlSelfRef.test_copper_lists__DOT__dut__DOT__cmd_char_din = 0x20U;
                                                vlSelfRef.test_copper_lists__DOT__dut__DOT__cmd_char_we = 1U;
                                                __Vfunc_test_copper_lists__DOT__dut__DOT__screen_addr__173__row 
                                                    = vlSelfRef.test_copper_lists__DOT__dut__DOT__cursor_y;
                                                __Vfunc_test_copper_lists__DOT__dut__DOT__screen_addr__173__rr 
                                                    = 
                                                    (0x0000001fU 
                                                     & ((IData)(__Vfunc_test_copper_lists__DOT__dut__DOT__screen_addr__173__row) 
                                                        + (IData)(vlSelfRef.test_copper_lists__DOT__dut__DOT__scroll_offset)));
                                                if (
                                                    (0x19U 
                                                     <= (IData)(__Vfunc_test_copper_lists__DOT__dut__DOT__screen_addr__173__rr))) {
                                                    __Vfunc_test_copper_lists__DOT__dut__DOT__screen_addr__173__rr 
                                                        = 
                                                        (0x0000001fU 
                                                         & ((IData)(__Vfunc_test_copper_lists__DOT__dut__DOT__screen_addr__173__rr) 
                                                            - (IData)(0x19U)));
                                                }
                                                __Vfunc_test_copper_lists__DOT__dut__DOT__screen_addr__173__Vfuncout 
                                                    = 
                                                    (0x000007ffU 
                                                     & (((IData)(0x00000050U) 
                                                         * (IData)(__Vfunc_test_copper_lists__DOT__dut__DOT__screen_addr__173__rr)) 
                                                        + (IData)(__Vfunc_test_copper_lists__DOT__dut__DOT__screen_addr__173__col)));
                                                vlSelfRef.test_copper_lists__DOT__dut__DOT__cmd_char_addr 
                                                    = __Vfunc_test_copper_lists__DOT__dut__DOT__screen_addr__173__Vfuncout;
                                            } else if (
                                                       (0x0aU 
                                                        == (IData)(vlSelfRef.test_copper_lists__DOT__cpu_wdata))) {
                                                if (
                                                    (0x18U 
                                                     <= (IData)(vlSelfRef.test_copper_lists__DOT__dut__DOT__cursor_y))) {
                                                    __Vdly__test_copper_lists__DOT__dut__DOT__scroll_offset 
                                                        = 
                                                        ((0x18U 
                                                          <= (IData)(vlSelfRef.test_copper_lists__DOT__dut__DOT__scroll_offset))
                                                          ? 0U
                                                          : 
                                                         (0x0000001fU 
                                                          & ((IData)(1U) 
                                                             + (IData)(vlSelfRef.test_copper_lists__DOT__dut__DOT__scroll_offset))));
                                                    __Vdly__test_copper_lists__DOT__dut__DOT__scroll_pending = 1U;
                                                    __Vdly__test_copper_lists__DOT__dut__DOT__scroll_col = 0U;
                                                } else {
                                                    __Vdly__test_copper_lists__DOT__dut__DOT__cursor_y 
                                                        = 
                                                        (0x0000001fU 
                                                         & ((IData)(1U) 
                                                            + (IData)(vlSelfRef.test_copper_lists__DOT__dut__DOT__cursor_y)));
                                                }
                                            } else if (
                                                       (0x0cU 
                                                        == (IData)(vlSelfRef.test_copper_lists__DOT__cpu_wdata))) {
                                                __Vdly__test_copper_lists__DOT__dut__DOT__cmd_cx = 0U;
                                                __Vdly__test_copper_lists__DOT__dut__DOT__cmd_cy = 0U;
                                                __Vdly__test_copper_lists__DOT__dut__DOT__cursor_x = 0U;
                                                __Vdly__test_copper_lists__DOT__dut__DOT__cursor_y = 0U;
                                                __Vdly__test_copper_lists__DOT__dut__DOT__scroll_offset = 0U;
                                                __Vdly__test_copper_lists__DOT__dut__DOT__cmd_busy = 1U;
                                                __Vdly__test_copper_lists__DOT__dut__DOT__cmd_op = 0xf0U;
                                            } else if (
                                                       (0x0dU 
                                                        == (IData)(vlSelfRef.test_copper_lists__DOT__cpu_wdata))) {
                                                __Vdly__test_copper_lists__DOT__dut__DOT__cursor_x = 0U;
                                            } else if (
                                                       (0x13U 
                                                        == (IData)(vlSelfRef.test_copper_lists__DOT__cpu_wdata))) {
                                                __Vdly__test_copper_lists__DOT__dut__DOT__cursor_x = 0U;
                                                __Vdly__test_copper_lists__DOT__dut__DOT__cursor_y = 0U;
                                            } else if (
                                                       (0x20U 
                                                        <= (IData)(vlSelfRef.test_copper_lists__DOT__cpu_wdata))) {
                                                __Vfunc_test_copper_lists__DOT__dut__DOT__screen_addr__174__row 
                                                    = vlSelfRef.test_copper_lists__DOT__dut__DOT__cursor_y;
                                                __Vfunc_test_copper_lists__DOT__dut__DOT__screen_addr__174__col 
                                                    = vlSelfRef.test_copper_lists__DOT__dut__DOT__cursor_x;
                                                __Vfunc_test_copper_lists__DOT__dut__DOT__screen_addr__174__rr 
                                                    = 
                                                    (0x0000001fU 
                                                     & ((IData)(__Vfunc_test_copper_lists__DOT__dut__DOT__screen_addr__174__row) 
                                                        + (IData)(vlSelfRef.test_copper_lists__DOT__dut__DOT__scroll_offset)));
                                                vlSelfRef.test_copper_lists__DOT__dut__DOT__cmd_char_din 
                                                    = vlSelfRef.test_copper_lists__DOT__cpu_wdata;
                                                vlSelfRef.test_copper_lists__DOT__dut__DOT__cmd_char_we = 1U;
                                                vlSelfRef.test_copper_lists__DOT__dut__DOT__cmd_color_din 
                                                    = vlSelfRef.test_copper_lists__DOT__dut__DOT__fg_color;
                                                vlSelfRef.test_copper_lists__DOT__dut__DOT__cmd_color_we = 1U;
                                                if (
                                                    (0x19U 
                                                     <= (IData)(__Vfunc_test_copper_lists__DOT__dut__DOT__screen_addr__174__rr))) {
                                                    __Vfunc_test_copper_lists__DOT__dut__DOT__screen_addr__174__rr 
                                                        = 
                                                        (0x0000001fU 
                                                         & ((IData)(__Vfunc_test_copper_lists__DOT__dut__DOT__screen_addr__174__rr) 
                                                            - (IData)(0x19U)));
                                                }
                                                __Vfunc_test_copper_lists__DOT__dut__DOT__screen_addr__174__Vfuncout 
                                                    = 
                                                    (0x000007ffU 
                                                     & (((IData)(0x00000050U) 
                                                         * (IData)(__Vfunc_test_copper_lists__DOT__dut__DOT__screen_addr__174__rr)) 
                                                        + (IData)(__Vfunc_test_copper_lists__DOT__dut__DOT__screen_addr__174__col)));
                                                vlSelfRef.test_copper_lists__DOT__dut__DOT__cmd_char_addr 
                                                    = __Vfunc_test_copper_lists__DOT__dut__DOT__screen_addr__174__Vfuncout;
                                                __Vfunc_test_copper_lists__DOT__dut__DOT__screen_addr__175__row 
                                                    = vlSelfRef.test_copper_lists__DOT__dut__DOT__cursor_y;
                                                __Vfunc_test_copper_lists__DOT__dut__DOT__screen_addr__175__col 
                                                    = vlSelfRef.test_copper_lists__DOT__dut__DOT__cursor_x;
                                                __Vfunc_test_copper_lists__DOT__dut__DOT__screen_addr__175__rr 
                                                    = 
                                                    (0x0000001fU 
                                                     & ((IData)(__Vfunc_test_copper_lists__DOT__dut__DOT__screen_addr__175__row) 
                                                        + (IData)(vlSelfRef.test_copper_lists__DOT__dut__DOT__scroll_offset)));
                                                if (
                                                    (0x19U 
                                                     <= (IData)(__Vfunc_test_copper_lists__DOT__dut__DOT__screen_addr__175__rr))) {
                                                    __Vfunc_test_copper_lists__DOT__dut__DOT__screen_addr__175__rr 
                                                        = 
                                                        (0x0000001fU 
                                                         & ((IData)(__Vfunc_test_copper_lists__DOT__dut__DOT__screen_addr__175__rr) 
                                                            - (IData)(0x19U)));
                                                }
                                                __Vfunc_test_copper_lists__DOT__dut__DOT__screen_addr__175__Vfuncout 
                                                    = 
                                                    (0x000007ffU 
                                                     & (((IData)(0x00000050U) 
                                                         * (IData)(__Vfunc_test_copper_lists__DOT__dut__DOT__screen_addr__175__rr)) 
                                                        + (IData)(__Vfunc_test_copper_lists__DOT__dut__DOT__screen_addr__175__col)));
                                                vlSelfRef.test_copper_lists__DOT__dut__DOT__cmd_color_addr 
                                                    = __Vfunc_test_copper_lists__DOT__dut__DOT__screen_addr__175__Vfuncout;
                                                if (
                                                    (0x4fU 
                                                     <= (IData)(vlSelfRef.test_copper_lists__DOT__dut__DOT__cursor_x))) {
                                                    __Vdly__test_copper_lists__DOT__dut__DOT__cursor_x = 0U;
                                                    if (
                                                        (0x18U 
                                                         <= (IData)(vlSelfRef.test_copper_lists__DOT__dut__DOT__cursor_y))) {
                                                        __Vdly__test_copper_lists__DOT__dut__DOT__scroll_offset 
                                                            = 
                                                            ((0x18U 
                                                              <= (IData)(vlSelfRef.test_copper_lists__DOT__dut__DOT__scroll_offset))
                                                              ? 0U
                                                              : 
                                                             (0x0000001fU 
                                                              & ((IData)(1U) 
                                                                 + (IData)(vlSelfRef.test_copper_lists__DOT__dut__DOT__scroll_offset))));
                                                        __Vdly__test_copper_lists__DOT__dut__DOT__scroll_pending = 1U;
                                                        __Vdly__test_copper_lists__DOT__dut__DOT__scroll_col = 0U;
                                                    } else {
                                                        __Vdly__test_copper_lists__DOT__dut__DOT__cursor_y 
                                                            = 
                                                            (0x0000001fU 
                                                             & ((IData)(1U) 
                                                                + (IData)(vlSelfRef.test_copper_lists__DOT__dut__DOT__cursor_y)));
                                                    }
                                                } else {
                                                    __Vdly__test_copper_lists__DOT__dut__DOT__cursor_x 
                                                        = 
                                                        (0x0000007fU 
                                                         & ((IData)(1U) 
                                                            + (IData)(vlSelfRef.test_copper_lists__DOT__dut__DOT__cursor_x)));
                                                }
                                            }
                                        } else if (
                                                   (1U 
                                                    & ((~ (IData)(vlSelfRef.test_copper_lists__DOT__dut__DOT__cmd_busy)) 
                                                       & (~ (IData)(vlSelfRef.test_copper_lists__DOT__dut__DOT__artist_busy))))) {
                                            vlSelfRef.test_copper_lists__DOT__dut__DOT__cmd_x 
                                                = (
                                                   (0x00000300U 
                                                    & (vlSelfRef.test_copper_lists__DOT__dut__DOT__regs[18U] 
                                                       << 8U)) 
                                                   | vlSelfRef.test_copper_lists__DOT__dut__DOT__regs[17U]);
                                            vlSelfRef.test_copper_lists__DOT__dut__DOT__cmd_y 
                                                = (
                                                   (0x00000300U 
                                                    & (vlSelfRef.test_copper_lists__DOT__dut__DOT__regs[20U] 
                                                       << 8U)) 
                                                   | vlSelfRef.test_copper_lists__DOT__dut__DOT__regs[19U]);
                                            if ((1U 
                                                 & (~ 
                                                    ((IData)(vlSelfRef.test_copper_lists__DOT__cpu_wdata) 
                                                     >> 7U)))) {
                                                if (
                                                    (1U 
                                                     & (~ 
                                                        ((IData)(vlSelfRef.test_copper_lists__DOT__cpu_wdata) 
                                                         >> 6U)))) {
                                                    if (
                                                        (0x00000020U 
                                                         & (IData)(vlSelfRef.test_copper_lists__DOT__cpu_wdata))) {
                                                        if (
                                                            (1U 
                                                             & (~ 
                                                                ((IData)(vlSelfRef.test_copper_lists__DOT__cpu_wdata) 
                                                                 >> 4U)))) {
                                                            if (
                                                                (1U 
                                                                 & (~ 
                                                                    ((IData)(vlSelfRef.test_copper_lists__DOT__cpu_wdata) 
                                                                     >> 3U)))) {
                                                                if (
                                                                    (1U 
                                                                     & (~ 
                                                                        ((IData)(vlSelfRef.test_copper_lists__DOT__cpu_wdata) 
                                                                         >> 2U)))) {
                                                                    if (
                                                                        (2U 
                                                                         & (IData)(vlSelfRef.test_copper_lists__DOT__cpu_wdata))) {
                                                                        if (
                                                                            (1U 
                                                                             & (~ (IData)(vlSelfRef.test_copper_lists__DOT__cpu_wdata)))) {
                                                                            __Vdly__test_copper_lists__DOT__dut__DOT__copper_target_list 
                                                                                = vlSelfRef.test_copper_lists__DOT__dut__DOT__copper_active_list;
                                                                        }
                                                                    } else if (
                                                                               (1U 
                                                                                & (IData)(vlSelfRef.test_copper_lists__DOT__cpu_wdata))) {
                                                                        __Vdly__test_copper_lists__DOT__dut__DOT__copper_pending_list 
                                                                            = 
                                                                            (0x0000007fU 
                                                                             & vlSelfRef.test_copper_lists__DOT__dut__DOT__regs[17U]);
                                                                    } else {
                                                                        __Vdly__test_copper_lists__DOT__dut__DOT__copper_target_list 
                                                                            = 
                                                                            (0x0000007fU 
                                                                             & vlSelfRef.test_copper_lists__DOT__dut__DOT__regs[17U]);
                                                                    }
                                                                }
                                                            }
                                                        }
                                                    } else if (
                                                               (0x00000010U 
                                                                & (IData)(vlSelfRef.test_copper_lists__DOT__cpu_wdata))) {
                                                        if (
                                                            (8U 
                                                             & (IData)(vlSelfRef.test_copper_lists__DOT__cpu_wdata))) {
                                                            if (
                                                                (4U 
                                                                 & (IData)(vlSelfRef.test_copper_lists__DOT__cpu_wdata))) {
                                                                if (
                                                                    (2U 
                                                                     & (IData)(vlSelfRef.test_copper_lists__DOT__cpu_wdata))) {
                                                                    if (
                                                                        (1U 
                                                                         & (IData)(vlSelfRef.test_copper_lists__DOT__cpu_wdata))) {
                                                                        vlSelfRef.test_copper_lists__DOT__dut__DOT__unnamedblk14__DOT__i = 0U;
                                                                        __Vdly__test_copper_lists__DOT__dut__DOT__cursor_x = 0U;
                                                                        __Vdly__test_copper_lists__DOT__dut__DOT__cursor_y = 0U;
                                                                        __Vdly__test_copper_lists__DOT__dut__DOT__fg_color = 1U;
                                                                        vlSelfRef.test_copper_lists__DOT__dut__DOT__bg_color = 6U;
                                                                        vlSelfRef.test_copper_lists__DOT__dut__DOT__gfx_color = 1U;
                                                                        __Vdly__test_copper_lists__DOT__dut__DOT__scroll_offset = 0U;
                                                                        __Vdly__test_copper_lists__DOT__dut__DOT__copper_enabled = 0U;
                                                                        __Vdly__test_copper_lists__DOT__dut__DOT__copper_count = 0U;
                                                                        __Vdly__test_copper_lists__DOT__dut__DOT__copper_target_list = 0U;
                                                                        __Vdly__test_copper_lists__DOT__dut__DOT__copper_active_list = 0U;
                                                                        __Vdly__test_copper_lists__DOT__dut__DOT__copper_pending_list = 0U;
                                                                        __Vdly__test_copper_lists__DOT__dut__DOT__copper_loading = 0U;
                                                                        while (
                                                                               VL_GTS_III(32, 0x00000080U, vlSelfRef.test_copper_lists__DOT__dut__DOT__unnamedblk14__DOT__i)) {
                                                                            __VdlyDim0__test_copper_lists__DOT__dut__DOT__copper_list_count__v0 
                                                                                = 
                                                                                (0x0000007fU 
                                                                                & vlSelfRef.test_copper_lists__DOT__dut__DOT__unnamedblk14__DOT__i);
                                                                            vlSelfRef.__VdlyCommitQueuetest_copper_lists__DOT__dut__DOT__copper_list_count.enqueue(0U, (IData)(__VdlyDim0__test_copper_lists__DOT__dut__DOT__copper_list_count__v0));
                                                                            vlSelfRef.test_copper_lists__DOT__dut__DOT__unnamedblk14__DOT__i 
                                                                                = 
                                                                                ((IData)(1U) 
                                                                                + vlSelfRef.test_copper_lists__DOT__dut__DOT__unnamedblk14__DOT__i);
                                                                        }
                                                                    } else {
                                                                        __Vdly__test_copper_lists__DOT__dut__DOT__copper_enabled = 0U;
                                                                    }
                                                                } else if (
                                                                           (1U 
                                                                            & (IData)(vlSelfRef.test_copper_lists__DOT__cpu_wdata))) {
                                                                    __Vdly__test_copper_lists__DOT__dut__DOT__copper_enabled = 1U;
                                                                } else {
                                                                    __VdlyDim0__test_copper_lists__DOT__dut__DOT__copper_list_count__v1 
                                                                        = vlSelfRef.test_copper_lists__DOT__dut__DOT__copper_target_list;
                                                                    vlSelfRef.__VdlyCommitQueuetest_copper_lists__DOT__dut__DOT__copper_list_count.enqueue(0U, (IData)(__VdlyDim0__test_copper_lists__DOT__dut__DOT__copper_list_count__v1));
                                                                    if (
                                                                        ((IData)(vlSelfRef.test_copper_lists__DOT__dut__DOT__copper_target_list) 
                                                                         == (IData)(vlSelfRef.test_copper_lists__DOT__dut__DOT__copper_active_list))) {
                                                                        __Vdly__test_copper_lists__DOT__dut__DOT__copper_count = 0U;
                                                                    }
                                                                }
                                                            } else if (
                                                                       (2U 
                                                                        & (IData)(vlSelfRef.test_copper_lists__DOT__cpu_wdata))) {
                                                                if (
                                                                    (1U 
                                                                     & (IData)(vlSelfRef.test_copper_lists__DOT__cpu_wdata))) {
                                                                    if (
                                                                        (0x0020U 
                                                                         > vlSelfRef.test_copper_lists__DOT__dut__DOT__copper_list_count
                                                                         [vlSelfRef.test_copper_lists__DOT__dut__DOT__copper_target_list])) {
                                                                        __VdlyVal__test_copper_lists__DOT__dut__DOT__copper_list_pos__v0 
                                                                            = 
                                                                            (0x0001ffffU 
                                                                             & (((IData)(0x00000140U) 
                                                                                * vlSelfRef.test_copper_lists__DOT__dut__DOT__regs[19U]) 
                                                                                + 
                                                                                ((0x00000100U 
                                                                                & (vlSelfRef.test_copper_lists__DOT__dut__DOT__regs[18U] 
                                                                                << 8U)) 
                                                                                | vlSelfRef.test_copper_lists__DOT__dut__DOT__regs[17U])));
                                                                        __VdlyDim0__test_copper_lists__DOT__dut__DOT__copper_list_pos__v0 
                                                                            = 
                                                                            (((IData)(vlSelfRef.test_copper_lists__DOT__dut__DOT__copper_target_list) 
                                                                              << 5U) 
                                                                             | (0x0000001fU 
                                                                                & vlSelfRef.test_copper_lists__DOT__dut__DOT__copper_list_count
                                                                                [vlSelfRef.test_copper_lists__DOT__dut__DOT__copper_target_list]));
                                                                        __VdlySet__test_copper_lists__DOT__dut__DOT__copper_list_pos__v0 = 1U;
                                                                        __VdlyVal__test_copper_lists__DOT__dut__DOT__copper_list_reg__v0 
                                                                            = vlSelfRef.test_copper_lists__DOT__dut__DOT__regs[20U];
                                                                        __VdlyDim0__test_copper_lists__DOT__dut__DOT__copper_list_reg__v0 
                                                                            = 
                                                                            (((IData)(vlSelfRef.test_copper_lists__DOT__dut__DOT__copper_target_list) 
                                                                              << 5U) 
                                                                             | (0x0000001fU 
                                                                                & vlSelfRef.test_copper_lists__DOT__dut__DOT__copper_list_count
                                                                                [vlSelfRef.test_copper_lists__DOT__dut__DOT__copper_target_list]));
                                                                        __VdlyVal__test_copper_lists__DOT__dut__DOT__copper_list_val__v0 
                                                                            = vlSelfRef.test_copper_lists__DOT__dut__DOT__regs[22U];
                                                                        __VdlyDim0__test_copper_lists__DOT__dut__DOT__copper_list_val__v0 
                                                                            = 
                                                                            (((IData)(vlSelfRef.test_copper_lists__DOT__dut__DOT__copper_target_list) 
                                                                              << 5U) 
                                                                             | (0x0000001fU 
                                                                                & vlSelfRef.test_copper_lists__DOT__dut__DOT__copper_list_count
                                                                                [vlSelfRef.test_copper_lists__DOT__dut__DOT__copper_target_list]));
                                                                        __VdlyVal__test_copper_lists__DOT__dut__DOT__copper_list_count__v2 
                                                                            = 
                                                                            (0x000001ffU 
                                                                             & ((IData)(1U) 
                                                                                + vlSelfRef.test_copper_lists__DOT__dut__DOT__copper_list_count
                                                                                [vlSelfRef.test_copper_lists__DOT__dut__DOT__copper_target_list]));
                                                                        __VdlyDim0__test_copper_lists__DOT__dut__DOT__copper_list_count__v2 
                                                                            = vlSelfRef.test_copper_lists__DOT__dut__DOT__copper_target_list;
                                                                        vlSelfRef.__VdlyCommitQueuetest_copper_lists__DOT__dut__DOT__copper_list_count.enqueue(__VdlyVal__test_copper_lists__DOT__dut__DOT__copper_list_count__v2, (IData)(__VdlyDim0__test_copper_lists__DOT__dut__DOT__copper_list_count__v2));
                                                                        if (
                                                                            ((IData)(vlSelfRef.test_copper_lists__DOT__dut__DOT__copper_target_list) 
                                                                             == (IData)(vlSelfRef.test_copper_lists__DOT__dut__DOT__copper_active_list))) {
                                                                            __VdlyVal__test_copper_lists__DOT__dut__DOT__copper_pos__v1 
                                                                                = 
                                                                                (0x0001ffffU 
                                                                                & (((IData)(0x00000140U) 
                                                                                * vlSelfRef.test_copper_lists__DOT__dut__DOT__regs[19U]) 
                                                                                + 
                                                                                ((0x00000100U 
                                                                                & (vlSelfRef.test_copper_lists__DOT__dut__DOT__regs[18U] 
                                                                                << 8U)) 
                                                                                | vlSelfRef.test_copper_lists__DOT__dut__DOT__regs[17U])));
                                                                            __VdlyDim0__test_copper_lists__DOT__dut__DOT__copper_pos__v1 
                                                                                = 
                                                                                (0x0000001fU 
                                                                                & (IData)(vlSelfRef.test_copper_lists__DOT__dut__DOT__copper_count));
                                                                            __VdlySet__test_copper_lists__DOT__dut__DOT__copper_pos__v1 = 1U;
                                                                            __VdlyVal__test_copper_lists__DOT__dut__DOT__copper_reg__v1 
                                                                                = vlSelfRef.test_copper_lists__DOT__dut__DOT__regs[20U];
                                                                            __VdlyDim0__test_copper_lists__DOT__dut__DOT__copper_reg__v1 
                                                                                = 
                                                                                (0x0000001fU 
                                                                                & (IData)(vlSelfRef.test_copper_lists__DOT__dut__DOT__copper_count));
                                                                            __VdlyVal__test_copper_lists__DOT__dut__DOT__copper_val__v1 
                                                                                = vlSelfRef.test_copper_lists__DOT__dut__DOT__regs[22U];
                                                                            __VdlyDim0__test_copper_lists__DOT__dut__DOT__copper_val__v1 
                                                                                = 
                                                                                (0x0000001fU 
                                                                                & (IData)(vlSelfRef.test_copper_lists__DOT__dut__DOT__copper_count));
                                                                            __Vdly__test_copper_lists__DOT__dut__DOT__copper_count 
                                                                                = 
                                                                                (0x000001ffU 
                                                                                & ((IData)(1U) 
                                                                                + (IData)(vlSelfRef.test_copper_lists__DOT__dut__DOT__copper_count)));
                                                                        }
                                                                    }
                                                                } else if (
                                                                           (0U 
                                                                            == vlSelfRef.test_copper_lists__DOT__dut__DOT__regs[17U])) {
                                                                    vlSelfRef.test_copper_lists__DOT__dut__DOT__cmd_char_addr 
                                                                        = 
                                                                        ((0x00000700U 
                                                                          & (vlSelfRef.test_copper_lists__DOT__dut__DOT__regs[19U] 
                                                                             << 8U)) 
                                                                         | vlSelfRef.test_copper_lists__DOT__dut__DOT__regs[18U]);
                                                                    vlSelfRef.test_copper_lists__DOT__dut__DOT__cmd_char_we = 1U;
                                                                    vlSelfRef.test_copper_lists__DOT__dut__DOT__cmd_char_din 
                                                                        = vlSelfRef.test_copper_lists__DOT__dut__DOT__regs[20U];
                                                                } else if (
                                                                           (1U 
                                                                            == vlSelfRef.test_copper_lists__DOT__dut__DOT__regs[17U])) {
                                                                    vlSelfRef.test_copper_lists__DOT__dut__DOT__cmd_color_addr 
                                                                        = 
                                                                        ((0x00000700U 
                                                                          & (vlSelfRef.test_copper_lists__DOT__dut__DOT__regs[19U] 
                                                                             << 8U)) 
                                                                         | vlSelfRef.test_copper_lists__DOT__dut__DOT__regs[18U]);
                                                                    vlSelfRef.test_copper_lists__DOT__dut__DOT__cmd_color_we = 1U;
                                                                    vlSelfRef.test_copper_lists__DOT__dut__DOT__cmd_color_din 
                                                                        = vlSelfRef.test_copper_lists__DOT__dut__DOT__regs[20U];
                                                                } else if (
                                                                           (2U 
                                                                            == vlSelfRef.test_copper_lists__DOT__dut__DOT__regs[17U])) {
                                                                    vlSelfRef.test_copper_lists__DOT__dut__DOT__cmd_gfx_addr 
                                                                        = 
                                                                        ((vlSelfRef.test_copper_lists__DOT__dut__DOT__regs[19U] 
                                                                          << 8U) 
                                                                         | vlSelfRef.test_copper_lists__DOT__dut__DOT__regs[18U]);
                                                                    vlSelfRef.test_copper_lists__DOT__dut__DOT__cmd_gfx_we = 1U;
                                                                    vlSelfRef.test_copper_lists__DOT__dut__DOT__cmd_gfx_din 
                                                                        = 
                                                                        (0x0000000fU 
                                                                         & vlSelfRef.test_copper_lists__DOT__dut__DOT__regs[20U]);
                                                                } else if (
                                                                           (3U 
                                                                            == vlSelfRef.test_copper_lists__DOT__dut__DOT__regs[17U])) {
                                                                    vlSelfRef.test_copper_lists__DOT__dut__DOT__cmd_spr_addr 
                                                                        = 
                                                                        ((0x00000700U 
                                                                          & (vlSelfRef.test_copper_lists__DOT__dut__DOT__regs[19U] 
                                                                             << 8U)) 
                                                                         | vlSelfRef.test_copper_lists__DOT__dut__DOT__regs[18U]);
                                                                    vlSelfRef.test_copper_lists__DOT__dut__DOT__cmd_spr_we = 1U;
                                                                    vlSelfRef.test_copper_lists__DOT__dut__DOT__cmd_spr_din 
                                                                        = vlSelfRef.test_copper_lists__DOT__dut__DOT__regs[20U];
                                                                } else if (
                                                                           (4U 
                                                                            == vlSelfRef.test_copper_lists__DOT__dut__DOT__regs[17U])) {
                                                                    vlSelfRef.test_copper_lists__DOT__dut__DOT__vgc_tile_addr 
                                                                        = 
                                                                        ((0x00007f00U 
                                                                          & (vlSelfRef.test_copper_lists__DOT__dut__DOT__regs[19U] 
                                                                             << 8U)) 
                                                                         | vlSelfRef.test_copper_lists__DOT__dut__DOT__regs[18U]);
                                                                    vlSelfRef.test_copper_lists__DOT__dut__DOT__vgc_tile_we = 1U;
                                                                    vlSelfRef.test_copper_lists__DOT__dut__DOT__vgc_tile_wdata 
                                                                        = vlSelfRef.test_copper_lists__DOT__dut__DOT__regs[20U];
                                                                }
                                                            } else if (
                                                                       (1U 
                                                                        & (IData)(vlSelfRef.test_copper_lists__DOT__cpu_wdata))) {
                                                                if (
                                                                    (0U 
                                                                     == vlSelfRef.test_copper_lists__DOT__dut__DOT__regs[17U])) {
                                                                    vlSelfRef.test_copper_lists__DOT__dut__DOT__cmd_char_addr 
                                                                        = 
                                                                        ((0x00000700U 
                                                                          & (vlSelfRef.test_copper_lists__DOT__dut__DOT__regs[19U] 
                                                                             << 8U)) 
                                                                         | vlSelfRef.test_copper_lists__DOT__dut__DOT__regs[18U]);
                                                                    __Vdly__test_copper_lists__DOT__dut__DOT__memread_pending = 1U;
                                                                    vlSelfRef.test_copper_lists__DOT__dut__DOT__memread_space = 0U;
                                                                } else if (
                                                                           (1U 
                                                                            == vlSelfRef.test_copper_lists__DOT__dut__DOT__regs[17U])) {
                                                                    vlSelfRef.test_copper_lists__DOT__dut__DOT__cmd_color_addr 
                                                                        = 
                                                                        ((0x00000700U 
                                                                          & (vlSelfRef.test_copper_lists__DOT__dut__DOT__regs[19U] 
                                                                             << 8U)) 
                                                                         | vlSelfRef.test_copper_lists__DOT__dut__DOT__regs[18U]);
                                                                    __Vdly__test_copper_lists__DOT__dut__DOT__memread_pending = 1U;
                                                                    vlSelfRef.test_copper_lists__DOT__dut__DOT__memread_space = 1U;
                                                                } else if (
                                                                           (2U 
                                                                            == vlSelfRef.test_copper_lists__DOT__dut__DOT__regs[17U])) {
                                                                    vlSelfRef.test_copper_lists__DOT__dut__DOT__cmd_gfx_addr 
                                                                        = 
                                                                        ((vlSelfRef.test_copper_lists__DOT__dut__DOT__regs[19U] 
                                                                          << 8U) 
                                                                         | vlSelfRef.test_copper_lists__DOT__dut__DOT__regs[18U]);
                                                                    vlSelfRef.test_copper_lists__DOT__dut__DOT__cmd_gfx_re = 1U;
                                                                    __Vdly__test_copper_lists__DOT__dut__DOT__memread_pending = 1U;
                                                                    vlSelfRef.test_copper_lists__DOT__dut__DOT__memread_space = 2U;
                                                                } else if (
                                                                           (3U 
                                                                            == vlSelfRef.test_copper_lists__DOT__dut__DOT__regs[17U])) {
                                                                    vlSelfRef.test_copper_lists__DOT__dut__DOT__cmd_spr_addr 
                                                                        = 
                                                                        ((0x00000700U 
                                                                          & (vlSelfRef.test_copper_lists__DOT__dut__DOT__regs[19U] 
                                                                             << 8U)) 
                                                                         | vlSelfRef.test_copper_lists__DOT__dut__DOT__regs[18U]);
                                                                    vlSelfRef.test_copper_lists__DOT__dut__DOT__cmd_spr_re = 1U;
                                                                    __Vdly__test_copper_lists__DOT__dut__DOT__memread_pending = 1U;
                                                                    vlSelfRef.test_copper_lists__DOT__dut__DOT__memread_space = 3U;
                                                                } else if (
                                                                           (4U 
                                                                            == vlSelfRef.test_copper_lists__DOT__dut__DOT__regs[17U])) {
                                                                    vlSelfRef.test_copper_lists__DOT__dut__DOT__vgc_tile_addr 
                                                                        = 
                                                                        ((0x00007f00U 
                                                                          & (vlSelfRef.test_copper_lists__DOT__dut__DOT__regs[19U] 
                                                                             << 8U)) 
                                                                         | vlSelfRef.test_copper_lists__DOT__dut__DOT__regs[18U]);
                                                                    vlSelfRef.test_copper_lists__DOT__dut__DOT__vgc_tile_re = 1U;
                                                                    __Vdly__test_copper_lists__DOT__dut__DOT__memread_pending = 1U;
                                                                    vlSelfRef.test_copper_lists__DOT__dut__DOT__memread_space = 4U;
                                                                }
                                                            } else if (
                                                                       (0x10U 
                                                                        > vlSelfRef.test_copper_lists__DOT__dut__DOT__regs[17U])) {
                                                                __VdlyVal__test_copper_lists__DOT__dut__DOT__spr_pri__v16 
                                                                    = 
                                                                    (3U 
                                                                     & vlSelfRef.test_copper_lists__DOT__dut__DOT__regs[18U]);
                                                                __VdlyDim0__test_copper_lists__DOT__dut__DOT__spr_pri__v16 
                                                                    = 
                                                                    (0x0000000fU 
                                                                     & vlSelfRef.test_copper_lists__DOT__dut__DOT__regs[17U]);
                                                                __VdlySet__test_copper_lists__DOT__dut__DOT__spr_pri__v16 = 1U;
                                                            }
                                                        } else if (
                                                                   (4U 
                                                                    & (IData)(vlSelfRef.test_copper_lists__DOT__cpu_wdata))) {
                                                            if (
                                                                (2U 
                                                                 & (IData)(vlSelfRef.test_copper_lists__DOT__cpu_wdata))) {
                                                                if (
                                                                    (1U 
                                                                     & (IData)(vlSelfRef.test_copper_lists__DOT__cpu_wdata))) {
                                                                    if (
                                                                        (0x10U 
                                                                         > vlSelfRef.test_copper_lists__DOT__dut__DOT__regs[17U])) {
                                                                        __VdlyVal__test_copper_lists__DOT__dut__DOT__spr_flip_h__v16 
                                                                            = 
                                                                            (1U 
                                                                             & vlSelfRef.test_copper_lists__DOT__dut__DOT__regs[18U]);
                                                                        __VdlyDim0__test_copper_lists__DOT__dut__DOT__spr_flip_h__v16 
                                                                            = 
                                                                            (0x0000000fU 
                                                                             & vlSelfRef.test_copper_lists__DOT__dut__DOT__regs[17U]);
                                                                        __VdlySet__test_copper_lists__DOT__dut__DOT__spr_flip_h__v16 = 1U;
                                                                        __VdlyVal__test_copper_lists__DOT__dut__DOT__spr_flip_v__v16 
                                                                            = 
                                                                            (1U 
                                                                             & (vlSelfRef.test_copper_lists__DOT__dut__DOT__regs[18U] 
                                                                                >> 1U));
                                                                        __VdlyDim0__test_copper_lists__DOT__dut__DOT__spr_flip_v__v16 
                                                                            = 
                                                                            (0x0000000fU 
                                                                             & vlSelfRef.test_copper_lists__DOT__dut__DOT__regs[17U]);
                                                                    }
                                                                } else if (
                                                                           (0x10U 
                                                                            > vlSelfRef.test_copper_lists__DOT__dut__DOT__regs[17U])) {
                                                                    __VdlyDim0__test_copper_lists__DOT__dut__DOT__spr_enable__v16 
                                                                        = 
                                                                        (0x0000000fU 
                                                                         & vlSelfRef.test_copper_lists__DOT__dut__DOT__regs[17U]);
                                                                    __VdlySet__test_copper_lists__DOT__dut__DOT__spr_enable__v16 = 1U;
                                                                }
                                                            } else if (
                                                                       (1U 
                                                                        & (IData)(vlSelfRef.test_copper_lists__DOT__cpu_wdata))) {
                                                                if (
                                                                    (0x10U 
                                                                     > vlSelfRef.test_copper_lists__DOT__dut__DOT__regs[17U])) {
                                                                    __VdlyDim0__test_copper_lists__DOT__dut__DOT__spr_enable__v17 
                                                                        = 
                                                                        (0x0000000fU 
                                                                         & vlSelfRef.test_copper_lists__DOT__dut__DOT__regs[17U]);
                                                                    __VdlySet__test_copper_lists__DOT__dut__DOT__spr_enable__v17 = 1U;
                                                                }
                                                            } else if (
                                                                       (0x10U 
                                                                        > vlSelfRef.test_copper_lists__DOT__dut__DOT__regs[17U])) {
                                                                __VdlyVal__test_copper_lists__DOT__dut__DOT__spr_x__v16 
                                                                    = 
                                                                    ((0x00000100U 
                                                                      & (vlSelfRef.test_copper_lists__DOT__dut__DOT__regs[19U] 
                                                                         << 8U)) 
                                                                     | vlSelfRef.test_copper_lists__DOT__dut__DOT__regs[18U]);
                                                                __VdlyDim0__test_copper_lists__DOT__dut__DOT__spr_x__v16 
                                                                    = 
                                                                    (0x0000000fU 
                                                                     & vlSelfRef.test_copper_lists__DOT__dut__DOT__regs[17U]);
                                                                __VdlySet__test_copper_lists__DOT__dut__DOT__spr_x__v16 = 1U;
                                                                __VdlyVal__test_copper_lists__DOT__dut__DOT__spr_y__v16 
                                                                    = vlSelfRef.test_copper_lists__DOT__dut__DOT__regs[20U];
                                                                __VdlyDim0__test_copper_lists__DOT__dut__DOT__spr_y__v16 
                                                                    = 
                                                                    (0x0000000fU 
                                                                     & vlSelfRef.test_copper_lists__DOT__dut__DOT__regs[17U]);
                                                            }
                                                        } else if (
                                                                   (2U 
                                                                    & (IData)(vlSelfRef.test_copper_lists__DOT__cpu_wdata))) {
                                                            if (
                                                                (1U 
                                                                 & (IData)(vlSelfRef.test_copper_lists__DOT__cpu_wdata))) {
                                                                if (
                                                                    ((0x10U 
                                                                      > vlSelfRef.test_copper_lists__DOT__dut__DOT__regs[17U]) 
                                                                     & (0x10U 
                                                                        > vlSelfRef.test_copper_lists__DOT__dut__DOT__regs[18U]))) {
                                                                    __Vdly__test_copper_lists__DOT__dut__DOT__cmd_cx = 0U;
                                                                    __Vdly__test_copper_lists__DOT__dut__DOT__cmd_cy = 0U;
                                                                    vlSelfRef.test_copper_lists__DOT__dut__DOT__cmd_x 
                                                                        = 
                                                                        (0x0000000fU 
                                                                         & vlSelfRef.test_copper_lists__DOT__dut__DOT__regs[17U]);
                                                                    __Vdly__test_copper_lists__DOT__dut__DOT__sprcopy_phase = 0U;
                                                                    __Vdly__test_copper_lists__DOT__dut__DOT__cmd_busy = 1U;
                                                                    __Vdly__test_copper_lists__DOT__dut__DOT__cmd_op = 0x13U;
                                                                    vlSelfRef.test_copper_lists__DOT__dut__DOT__cmd_y 
                                                                        = 
                                                                        (0x0000000fU 
                                                                         & vlSelfRef.test_copper_lists__DOT__dut__DOT__regs[18U]);
                                                                }
                                                            } else if (
                                                                       (0x10U 
                                                                        > vlSelfRef.test_copper_lists__DOT__dut__DOT__regs[17U])) {
                                                                __Vdly__test_copper_lists__DOT__dut__DOT__cmd_cx = 0U;
                                                                __Vdly__test_copper_lists__DOT__dut__DOT__cmd_cy = 0U;
                                                                vlSelfRef.test_copper_lists__DOT__dut__DOT__cmd_x 
                                                                    = 
                                                                    (0x0000001eU 
                                                                     & (vlSelfRef.test_copper_lists__DOT__dut__DOT__regs[17U] 
                                                                        << 1U));
                                                                __Vdly__test_copper_lists__DOT__dut__DOT__cmd_busy = 1U;
                                                                __Vdly__test_copper_lists__DOT__dut__DOT__cmd_op = 0x12U;
                                                            }
                                                        } else if (
                                                                   (1U 
                                                                    & (IData)(vlSelfRef.test_copper_lists__DOT__cpu_wdata))) {
                                                            if (
                                                                ((0x10U 
                                                                  > vlSelfRef.test_copper_lists__DOT__dut__DOT__regs[17U]) 
                                                                 & (0x10U 
                                                                    > vlSelfRef.test_copper_lists__DOT__dut__DOT__regs[18U]))) {
                                                                vlSelfRef.test_copper_lists__DOT__dut__DOT__sprrow_spr 
                                                                    = 
                                                                    (0x0000000fU 
                                                                     & vlSelfRef.test_copper_lists__DOT__dut__DOT__regs[17U]);
                                                                __Vdly__test_copper_lists__DOT__dut__DOT__sprrow_count = 0U;
                                                                __Vdly__test_copper_lists__DOT__dut__DOT__cmd_busy = 1U;
                                                                __Vdly__test_copper_lists__DOT__dut__DOT__cmd_op = 0x11U;
                                                                vlSelfRef.test_copper_lists__DOT__dut__DOT__sprrow_row 
                                                                    = 
                                                                    (0x0000000fU 
                                                                     & vlSelfRef.test_copper_lists__DOT__dut__DOT__regs[18U]);
                                                                __VdlyVal__test_copper_lists__DOT__dut__DOT__sprrow_data__v0 
                                                                    = vlSelfRef.test_copper_lists__DOT__dut__DOT__regs[19U];
                                                                __VdlySet__test_copper_lists__DOT__dut__DOT__sprrow_data__v0 = 1U;
                                                                __VdlyVal__test_copper_lists__DOT__dut__DOT__sprrow_data__v1 
                                                                    = vlSelfRef.test_copper_lists__DOT__dut__DOT__regs[20U];
                                                                __VdlyVal__test_copper_lists__DOT__dut__DOT__sprrow_data__v2 
                                                                    = vlSelfRef.test_copper_lists__DOT__dut__DOT__regs[21U];
                                                                __VdlyVal__test_copper_lists__DOT__dut__DOT__sprrow_data__v3 
                                                                    = vlSelfRef.test_copper_lists__DOT__dut__DOT__regs[22U];
                                                                __VdlyVal__test_copper_lists__DOT__dut__DOT__sprrow_data__v4 
                                                                    = vlSelfRef.test_copper_lists__DOT__dut__DOT__regs[23U];
                                                                __VdlyVal__test_copper_lists__DOT__dut__DOT__sprrow_data__v5 
                                                                    = vlSelfRef.test_copper_lists__DOT__dut__DOT__regs[24U];
                                                                __VdlyVal__test_copper_lists__DOT__dut__DOT__sprrow_data__v6 
                                                                    = vlSelfRef.test_copper_lists__DOT__dut__DOT__regs[25U];
                                                                __VdlyVal__test_copper_lists__DOT__dut__DOT__sprrow_data__v7 
                                                                    = vlSelfRef.test_copper_lists__DOT__dut__DOT__regs[26U];
                                                            }
                                                        } else if (
                                                                   (((0x10U 
                                                                      > vlSelfRef.test_copper_lists__DOT__dut__DOT__regs[17U]) 
                                                                     & (0x10U 
                                                                        > vlSelfRef.test_copper_lists__DOT__dut__DOT__regs[18U])) 
                                                                    & (0x10U 
                                                                       > vlSelfRef.test_copper_lists__DOT__dut__DOT__regs[19U]))) {
                                                            vlSelfRef.test_copper_lists__DOT__dut__DOT__cmd_spr_addr 
                                                                = 
                                                                ((0x00000780U 
                                                                  & (vlSelfRef.test_copper_lists__DOT__dut__DOT__regs[17U] 
                                                                     << 7U)) 
                                                                 | ((0x00000078U 
                                                                     & (vlSelfRef.test_copper_lists__DOT__dut__DOT__regs[19U] 
                                                                        << 3U)) 
                                                                    | (7U 
                                                                       & (vlSelfRef.test_copper_lists__DOT__dut__DOT__regs[18U] 
                                                                          >> 1U))));
                                                            vlSelfRef.test_copper_lists__DOT__dut__DOT__cmd_spr_re = 1U;
                                                            __Vdly__test_copper_lists__DOT__dut__DOT__cmd_busy = 1U;
                                                            __Vdly__test_copper_lists__DOT__dut__DOT__cmd_op = 0x10U;
                                                        }
                                                    } else if (
                                                               (8U 
                                                                & (IData)(vlSelfRef.test_copper_lists__DOT__cpu_wdata))) {
                                                        if (
                                                            (1U 
                                                             & (~ 
                                                                ((IData)(vlSelfRef.test_copper_lists__DOT__cpu_wdata) 
                                                                 >> 2U)))) {
                                                            if (
                                                                (2U 
                                                                 & (IData)(vlSelfRef.test_copper_lists__DOT__cpu_wdata))) {
                                                                if (
                                                                    (1U 
                                                                     & (~ (IData)(vlSelfRef.test_copper_lists__DOT__cpu_wdata)))) {
                                                                    vlSelfRef.test_copper_lists__DOT__dut__DOT__artist_cmd_valid = 1U;
                                                                    vlSelfRef.test_copper_lists__DOT__dut__DOT__artist_cmd_code 
                                                                        = vlSelfRef.test_copper_lists__DOT__cpu_wdata;
                                                                }
                                                            } else if (
                                                                       (1U 
                                                                        & (IData)(vlSelfRef.test_copper_lists__DOT__cpu_wdata))) {
                                                                vlSelfRef.test_copper_lists__DOT__dut__DOT__artist_cmd_valid = 1U;
                                                                vlSelfRef.test_copper_lists__DOT__dut__DOT__artist_cmd_code 
                                                                    = vlSelfRef.test_copper_lists__DOT__cpu_wdata;
                                                            } else {
                                                                vlSelfRef.test_copper_lists__DOT__dut__DOT__gfx_color 
                                                                    = 
                                                                    (0x0000000fU 
                                                                     & vlSelfRef.test_copper_lists__DOT__dut__DOT__regs[17U]);
                                                            }
                                                        }
                                                    } else if (
                                                               (4U 
                                                                & (IData)(vlSelfRef.test_copper_lists__DOT__cpu_wdata))) {
                                                        vlSelfRef.test_copper_lists__DOT__dut__DOT__artist_cmd_valid = 1U;
                                                        vlSelfRef.test_copper_lists__DOT__dut__DOT__artist_cmd_code 
                                                            = vlSelfRef.test_copper_lists__DOT__cpu_wdata;
                                                    } else if (
                                                               (2U 
                                                                & (IData)(vlSelfRef.test_copper_lists__DOT__cpu_wdata))) {
                                                        vlSelfRef.test_copper_lists__DOT__dut__DOT__artist_cmd_valid = 1U;
                                                        vlSelfRef.test_copper_lists__DOT__dut__DOT__artist_cmd_code 
                                                            = vlSelfRef.test_copper_lists__DOT__cpu_wdata;
                                                    } else if (
                                                               (1U 
                                                                & (IData)(vlSelfRef.test_copper_lists__DOT__cpu_wdata))) {
                                                        vlSelfRef.test_copper_lists__DOT__dut__DOT__artist_cmd_valid = 1U;
                                                        vlSelfRef.test_copper_lists__DOT__dut__DOT__artist_cmd_code 
                                                            = vlSelfRef.test_copper_lists__DOT__cpu_wdata;
                                                    }
                                                }
                                            }
                                        }
                                    }
                                }
                            }
                        }
                    }
                } else {
                    __VdlyVal__test_copper_lists__DOT__dut__DOT__regs__v35 
                        = vlSelfRef.test_copper_lists__DOT__cpu_wdata;
                    __VdlyDim0__test_copper_lists__DOT__dut__DOT__regs__v35 
                        = (0x0000001fU & (IData)(vlSelfRef.test_copper_lists__DOT__cpu_addr));
                    __VdlySet__test_copper_lists__DOT__dut__DOT__regs__v35 = 1U;
                }
            }
            if (vlSelfRef.test_copper_lists__DOT__dut__DOT__char_ram_sel) {
                vlSelfRef.test_copper_lists__DOT__dut__DOT__cmd_char_addr 
                    = (0x000007ffU & ((IData)(vlSelfRef.test_copper_lists__DOT__cpu_addr) 
                                      - (IData)(0x0200U)));
                vlSelfRef.test_copper_lists__DOT__dut__DOT__cmd_char_din 
                    = vlSelfRef.test_copper_lists__DOT__cpu_wdata;
                vlSelfRef.test_copper_lists__DOT__dut__DOT__cmd_char_we = 1U;
            }
            if (vlSelfRef.test_copper_lists__DOT__dut__DOT__color_ram_sel) {
                vlSelfRef.test_copper_lists__DOT__dut__DOT__cmd_color_addr 
                    = (0x000007ffU & ((IData)(vlSelfRef.test_copper_lists__DOT__cpu_addr) 
                                      - (IData)(0x01d0U)));
                vlSelfRef.test_copper_lists__DOT__dut__DOT__cmd_color_din 
                    = vlSelfRef.test_copper_lists__DOT__cpu_wdata;
                vlSelfRef.test_copper_lists__DOT__dut__DOT__cmd_color_we = 1U;
            }
            if (vlSelfRef.test_copper_lists__DOT__dut__DOT__spr_reg_sel) {
                if ((4U & (IData)(vlSelfRef.test_copper_lists__DOT__cpu_addr))) {
                    if ((2U & (IData)(vlSelfRef.test_copper_lists__DOT__cpu_addr))) {
                        if ((1U & (~ (IData)(vlSelfRef.test_copper_lists__DOT__cpu_addr)))) {
                            __VdlyVal__test_copper_lists__DOT__dut__DOT__spr_pri__v17 
                                = (3U & (IData)(vlSelfRef.test_copper_lists__DOT__cpu_wdata));
                            __VdlyDim0__test_copper_lists__DOT__dut__DOT__spr_pri__v17 
                                = (0x0000000fU & ((IData)(vlSelfRef.test_copper_lists__DOT__cpu_addr) 
                                                  >> 3U));
                            __VdlySet__test_copper_lists__DOT__dut__DOT__spr_pri__v17 = 1U;
                        }
                    } else if ((1U & (IData)(vlSelfRef.test_copper_lists__DOT__cpu_addr))) {
                        __VdlyVal__test_copper_lists__DOT__dut__DOT__spr_flip_h__v17 
                            = (1U & (IData)(vlSelfRef.test_copper_lists__DOT__cpu_wdata));
                        __VdlyDim0__test_copper_lists__DOT__dut__DOT__spr_flip_h__v17 
                            = (0x0000000fU & ((IData)(vlSelfRef.test_copper_lists__DOT__cpu_addr) 
                                              >> 3U));
                        __VdlySet__test_copper_lists__DOT__dut__DOT__spr_flip_h__v17 = 1U;
                        __VdlyVal__test_copper_lists__DOT__dut__DOT__spr_flip_v__v17 
                            = (1U & ((IData)(vlSelfRef.test_copper_lists__DOT__cpu_wdata) 
                                     >> 1U));
                        __VdlyDim0__test_copper_lists__DOT__dut__DOT__spr_flip_v__v17 
                            = (0x0000000fU & ((IData)(vlSelfRef.test_copper_lists__DOT__cpu_addr) 
                                              >> 3U));
                        __VdlyVal__test_copper_lists__DOT__dut__DOT__spr_enable__v18 
                            = (1U & ((IData)(vlSelfRef.test_copper_lists__DOT__cpu_wdata) 
                                     >> 7U));
                        __VdlyDim0__test_copper_lists__DOT__dut__DOT__spr_enable__v18 
                            = (0x0000000fU & ((IData)(vlSelfRef.test_copper_lists__DOT__cpu_addr) 
                                              >> 3U));
                    }
                } else if ((2U & (IData)(vlSelfRef.test_copper_lists__DOT__cpu_addr))) {
                    if ((1U & (~ (IData)(vlSelfRef.test_copper_lists__DOT__cpu_addr)))) {
                        __VdlyVal__test_copper_lists__DOT__dut__DOT__spr_y__v17 
                            = vlSelfRef.test_copper_lists__DOT__cpu_wdata;
                        __VdlyDim0__test_copper_lists__DOT__dut__DOT__spr_y__v17 
                            = (0x0000000fU & ((IData)(vlSelfRef.test_copper_lists__DOT__cpu_addr) 
                                              >> 3U));
                        __VdlySet__test_copper_lists__DOT__dut__DOT__spr_y__v17 = 1U;
                    }
                } else if ((1U & (IData)(vlSelfRef.test_copper_lists__DOT__cpu_addr))) {
                    __VdlyVal__test_copper_lists__DOT__dut__DOT__spr_x__v17 
                        = (1U & (IData)(vlSelfRef.test_copper_lists__DOT__cpu_wdata));
                    __VdlyDim0__test_copper_lists__DOT__dut__DOT__spr_x__v17 
                        = (0x0000000fU & ((IData)(vlSelfRef.test_copper_lists__DOT__cpu_addr) 
                                          >> 3U));
                    __VdlySet__test_copper_lists__DOT__dut__DOT__spr_x__v17 = 1U;
                } else {
                    __VdlyVal__test_copper_lists__DOT__dut__DOT__spr_x__v18 
                        = vlSelfRef.test_copper_lists__DOT__cpu_wdata;
                    __VdlyDim0__test_copper_lists__DOT__dut__DOT__spr_x__v18 
                        = (0x0000000fU & ((IData)(vlSelfRef.test_copper_lists__DOT__cpu_addr) 
                                          >> 3U));
                    __VdlySet__test_copper_lists__DOT__dut__DOT__spr_x__v18 = 1U;
                }
            }
        }
        if ((((IData)(vlSelfRef.test_copper_lists__DOT__dut__DOT__cmd_busy) 
              & (0x10U == (IData)(vlSelfRef.test_copper_lists__DOT__dut__DOT__cmd_op))) 
             & (~ (IData)(vlSelfRef.test_copper_lists__DOT__dut__DOT__memread_pending)))) {
            vlSelfRef.test_copper_lists__DOT__dut__DOT__cmd_spr_addr 
                = ((0x00000780U & (vlSelfRef.test_copper_lists__DOT__dut__DOT__regs[17U] 
                                   << 7U)) | ((0x00000078U 
                                               & (vlSelfRef.test_copper_lists__DOT__dut__DOT__regs[19U] 
                                                  << 3U)) 
                                              | (7U 
                                                 & (vlSelfRef.test_copper_lists__DOT__dut__DOT__regs[18U] 
                                                    >> 1U))));
            vlSelfRef.test_copper_lists__DOT__dut__DOT__cmd_spr_we = 1U;
            __Vdly__test_copper_lists__DOT__dut__DOT__cmd_busy = 0U;
            vlSelfRef.test_copper_lists__DOT__dut__DOT__cmd_spr_din 
                = ((1U & vlSelfRef.test_copper_lists__DOT__dut__DOT__regs[18U])
                    ? ((0x000000f0U & (IData)(vlSelfRef.test_copper_lists__DOT__dut__DOT__spr_a_dout)) 
                       | (0x0000000fU & vlSelfRef.test_copper_lists__DOT__dut__DOT__regs[20U]))
                    : ((0x000000f0U & (vlSelfRef.test_copper_lists__DOT__dut__DOT__regs[20U] 
                                       << 4U)) | (0x0000000fU 
                                                  & (IData)(vlSelfRef.test_copper_lists__DOT__dut__DOT__spr_a_dout))));
        }
    }
    vlSelfRef.test_copper_lists__DOT__dut__DOT__font_a_addr 
        = ((IData)(vlSelfRef.test_copper_lists__DOT__dut__DOT__artist_font_re)
            ? (IData)(vlSelfRef.test_copper_lists__DOT__dut__DOT__artist_font_addr)
            : 0U);
    vlSelfRef.test_copper_lists__DOT__dut__DOT__artist_inst__DOT__fs_b_addr 
        = __Vdly__test_copper_lists__DOT__dut__DOT__artist_inst__DOT__fs_b_addr;
    if (__VdlySet__test_copper_lists__DOT__dut__DOT__artist_inst__DOT__fill_stack_mem__DOT__mem__v0) {
        vlSelfRef.test_copper_lists__DOT__dut__DOT__artist_inst__DOT__fill_stack_mem__DOT__mem[__VdlyDim0__test_copper_lists__DOT__dut__DOT__artist_inst__DOT__fill_stack_mem__DOT__mem__v0] 
            = __VdlyVal__test_copper_lists__DOT__dut__DOT__artist_inst__DOT__fill_stack_mem__DOT__mem__v0;
    }
    vlSelfRef.test_copper_lists__DOT__dut__DOT__vgc_reg_sel 
        = ((0xa000U <= (IData)(vlSelfRef.test_copper_lists__DOT__cpu_addr)) 
           & (0xa01fU >= (IData)(vlSelfRef.test_copper_lists__DOT__cpu_addr)));
    vlSelfRef.test_copper_lists__DOT__dut__DOT__spr_reg_sel 
        = ((0xa040U <= (IData)(vlSelfRef.test_copper_lists__DOT__cpu_addr)) 
           & (0xa0bfU >= (IData)(vlSelfRef.test_copper_lists__DOT__cpu_addr)));
    vlSelfRef.test_copper_lists__DOT__dut__DOT__cursor_x 
        = __Vdly__test_copper_lists__DOT__dut__DOT__cursor_x;
    vlSelfRef.test_copper_lists__DOT__dut__DOT__cursor_y 
        = __Vdly__test_copper_lists__DOT__dut__DOT__cursor_y;
    vlSelfRef.test_copper_lists__DOT__dut__DOT__fg_color 
        = __Vdly__test_copper_lists__DOT__dut__DOT__fg_color;
    vlSelfRef.test_copper_lists__DOT__dut__DOT__scroll_pending 
        = __Vdly__test_copper_lists__DOT__dut__DOT__scroll_pending;
    vlSelfRef.test_copper_lists__DOT__dut__DOT__scroll_col 
        = __Vdly__test_copper_lists__DOT__dut__DOT__scroll_col;
    vlSelfRef.test_copper_lists__DOT__dut__DOT__cmd_busy 
        = __Vdly__test_copper_lists__DOT__dut__DOT__cmd_busy;
    vlSelfRef.test_copper_lists__DOT__dut__DOT__copper_target_list 
        = __Vdly__test_copper_lists__DOT__dut__DOT__copper_target_list;
    vlSelfRef.test_copper_lists__DOT__dut__DOT__copper_pending_list 
        = __Vdly__test_copper_lists__DOT__dut__DOT__copper_pending_list;
    vlSelfRef.test_copper_lists__DOT__dut__DOT__copper_loading 
        = __Vdly__test_copper_lists__DOT__dut__DOT__copper_loading;
    vlSelfRef.test_copper_lists__DOT__dut__DOT__sprrow_count 
        = __Vdly__test_copper_lists__DOT__dut__DOT__sprrow_count;
    vlSelfRef.test_copper_lists__DOT__dut__DOT__sprcopy_phase 
        = __Vdly__test_copper_lists__DOT__dut__DOT__sprcopy_phase;
    vlSelfRef.test_copper_lists__DOT__dut__DOT__memread_pending 
        = __Vdly__test_copper_lists__DOT__dut__DOT__memread_pending;
    vlSelfRef.test_copper_lists__DOT__dut__DOT__cmd_cx 
        = __Vdly__test_copper_lists__DOT__dut__DOT__cmd_cx;
    vlSelfRef.test_copper_lists__DOT__dut__DOT__cmd_cy 
        = __Vdly__test_copper_lists__DOT__dut__DOT__cmd_cy;
    vlSelfRef.test_copper_lists__DOT__dut__DOT__copper_active_list 
        = __Vdly__test_copper_lists__DOT__dut__DOT__copper_active_list;
    vlSelfRef.test_copper_lists__DOT__dut__DOT__cmd_op 
        = __Vdly__test_copper_lists__DOT__dut__DOT__cmd_op;
    vlSelfRef.test_copper_lists__DOT__dut__DOT__artist_busy 
        = __Vdly__test_copper_lists__DOT__dut__DOT__artist_busy;
    vlSelfRef.__VdlyCommitQueuetest_copper_lists__DOT__dut__DOT__copper_list_count.commit(vlSelfRef.test_copper_lists__DOT__dut__DOT__copper_list_count);
    if (vlSelfRef.test_copper_lists__DOT__rst) {
        __Vdly__test_copper_lists__DOT__dut__DOT__copper_index = 0U;
        vlSelfRef.test_copper_lists__DOT__dut__DOT__copper_fire = 0U;
    } else {
        vlSelfRef.test_copper_lists__DOT__dut__DOT__copper_fire = 0U;
        if (((0U == (IData)(vlSelfRef.test_copper_lists__DOT__dut__DOT__h_count)) 
             & (0x01e0U == (IData)(vlSelfRef.test_copper_lists__DOT__dut__DOT__v_count)))) {
            __Vdly__test_copper_lists__DOT__dut__DOT__copper_index = 0U;
        }
        if (((((IData)(vlSelfRef.test_copper_lists__DOT__dut__DOT__copper_enabled) 
               & (IData)(vlSelfRef.test_copper_lists__DOT__dut__DOT__in_text_area)) 
              & ((IData)(vlSelfRef.test_copper_lists__DOT__dut__DOT__copper_index) 
                 < (IData)(vlSelfRef.test_copper_lists__DOT__dut__DOT__copper_count))) 
             & ((0x0001ffffU & (((IData)(0x00000140U) 
                                 * (IData)(vlSelfRef.test_copper_lists__DOT__dut__DOT__gfx_y)) 
                                + (0x000001ffU & ((IData)(vlSelfRef.test_copper_lists__DOT__dut__DOT__h_count) 
                                                  >> 1U)))) 
                >= vlSelfRef.test_copper_lists__DOT__dut__DOT__copper_inst__DOT__copper_pos
                [(0x0000001fU & (IData)(vlSelfRef.test_copper_lists__DOT__dut__DOT__copper_index))]))) {
            vlSelfRef.test_copper_lists__DOT__dut__DOT__copper_fire = 1U;
            vlSelfRef.test_copper_lists__DOT__dut__DOT__copper_fire_reg 
                = vlSelfRef.test_copper_lists__DOT__dut__DOT__copper_inst__DOT__copper_reg
                [(0x0000001fU & (IData)(vlSelfRef.test_copper_lists__DOT__dut__DOT__copper_index))];
            vlSelfRef.test_copper_lists__DOT__dut__DOT__copper_fire_val 
                = vlSelfRef.test_copper_lists__DOT__dut__DOT__copper_inst__DOT__copper_val
                [(0x0000001fU & (IData)(vlSelfRef.test_copper_lists__DOT__dut__DOT__copper_index))];
            __Vdly__test_copper_lists__DOT__dut__DOT__copper_index 
                = (0x000001ffU & ((IData)(1U) + (IData)(vlSelfRef.test_copper_lists__DOT__dut__DOT__copper_index)));
        }
    }
    vlSelfRef.test_copper_lists__DOT__dut__DOT__tile_blt_rdata 
        = ((IData)(vlSelfRef.test_copper_lists__DOT__dut__DOT__tile_inst__DOT__blt_tile_rd_pending)
            ? (IData)(vlSelfRef.test_copper_lists__DOT__dut__DOT__tile_inst__DOT__td_dout_a)
            : (IData)(vlSelfRef.test_copper_lists__DOT__dut__DOT__tile_inst__DOT__blt_tile_rd_latch));
    if (__VdlySet__test_copper_lists__DOT__dut__DOT__sprrow_data__v0) {
        vlSelfRef.test_copper_lists__DOT__dut__DOT__sprrow_data[0U] 
            = __VdlyVal__test_copper_lists__DOT__dut__DOT__sprrow_data__v0;
        vlSelfRef.test_copper_lists__DOT__dut__DOT__sprrow_data[1U] 
            = __VdlyVal__test_copper_lists__DOT__dut__DOT__sprrow_data__v1;
        vlSelfRef.test_copper_lists__DOT__dut__DOT__sprrow_data[2U] 
            = __VdlyVal__test_copper_lists__DOT__dut__DOT__sprrow_data__v2;
        vlSelfRef.test_copper_lists__DOT__dut__DOT__sprrow_data[3U] 
            = __VdlyVal__test_copper_lists__DOT__dut__DOT__sprrow_data__v3;
        vlSelfRef.test_copper_lists__DOT__dut__DOT__sprrow_data[4U] 
            = __VdlyVal__test_copper_lists__DOT__dut__DOT__sprrow_data__v4;
        vlSelfRef.test_copper_lists__DOT__dut__DOT__sprrow_data[5U] 
            = __VdlyVal__test_copper_lists__DOT__dut__DOT__sprrow_data__v5;
        vlSelfRef.test_copper_lists__DOT__dut__DOT__sprrow_data[6U] 
            = __VdlyVal__test_copper_lists__DOT__dut__DOT__sprrow_data__v6;
        vlSelfRef.test_copper_lists__DOT__dut__DOT__sprrow_data[7U] 
            = __VdlyVal__test_copper_lists__DOT__dut__DOT__sprrow_data__v7;
    }
    vlSelfRef.test_copper_lists__DOT__dut__DOT__char_ram_sel 
        = ((0xaa00U <= (IData)(vlSelfRef.test_copper_lists__DOT__cpu_addr)) 
           & (0xb1cfU >= (IData)(vlSelfRef.test_copper_lists__DOT__cpu_addr)));
    vlSelfRef.test_copper_lists__DOT__dut__DOT__color_ram_sel 
        = ((0xb1d0U <= (IData)(vlSelfRef.test_copper_lists__DOT__cpu_addr)) 
           & (0xb99fU >= (IData)(vlSelfRef.test_copper_lists__DOT__cpu_addr)));
    vlSelfRef.test_copper_lists__DOT__dut__DOT__scroll_offset 
        = __Vdly__test_copper_lists__DOT__dut__DOT__scroll_offset;
    if (__VdlySet__test_copper_lists__DOT__dut__DOT__copper_list_pos__v0) {
        vlSelfRef.test_copper_lists__DOT__dut__DOT__copper_list_pos[__VdlyDim0__test_copper_lists__DOT__dut__DOT__copper_list_pos__v0] 
            = __VdlyVal__test_copper_lists__DOT__dut__DOT__copper_list_pos__v0;
        vlSelfRef.test_copper_lists__DOT__dut__DOT__copper_list_reg[__VdlyDim0__test_copper_lists__DOT__dut__DOT__copper_list_reg__v0] 
            = __VdlyVal__test_copper_lists__DOT__dut__DOT__copper_list_reg__v0;
        vlSelfRef.test_copper_lists__DOT__dut__DOT__copper_list_val[__VdlyDim0__test_copper_lists__DOT__dut__DOT__copper_list_val__v0] 
            = __VdlyVal__test_copper_lists__DOT__dut__DOT__copper_list_val__v0;
    }
    if (__VdlySet__test_copper_lists__DOT__dut__DOT__copper_pos__v0) {
        vlSelfRef.test_copper_lists__DOT__dut__DOT__copper_reg[__VdlyDim0__test_copper_lists__DOT__dut__DOT__copper_reg__v0] 
            = __VdlyVal__test_copper_lists__DOT__dut__DOT__copper_reg__v0;
        vlSelfRef.test_copper_lists__DOT__dut__DOT__copper_val[__VdlyDim0__test_copper_lists__DOT__dut__DOT__copper_val__v0] 
            = __VdlyVal__test_copper_lists__DOT__dut__DOT__copper_val__v0;
        vlSelfRef.test_copper_lists__DOT__dut__DOT__copper_pos[__VdlyDim0__test_copper_lists__DOT__dut__DOT__copper_pos__v0] 
            = __VdlyVal__test_copper_lists__DOT__dut__DOT__copper_pos__v0;
    }
    if (__VdlySet__test_copper_lists__DOT__dut__DOT__copper_pos__v1) {
        vlSelfRef.test_copper_lists__DOT__dut__DOT__copper_reg[__VdlyDim0__test_copper_lists__DOT__dut__DOT__copper_reg__v1] 
            = __VdlyVal__test_copper_lists__DOT__dut__DOT__copper_reg__v1;
        vlSelfRef.test_copper_lists__DOT__dut__DOT__copper_val[__VdlyDim0__test_copper_lists__DOT__dut__DOT__copper_val__v1] 
            = __VdlyVal__test_copper_lists__DOT__dut__DOT__copper_val__v1;
        vlSelfRef.test_copper_lists__DOT__dut__DOT__copper_pos[__VdlyDim0__test_copper_lists__DOT__dut__DOT__copper_pos__v1] 
            = __VdlyVal__test_copper_lists__DOT__dut__DOT__copper_pos__v1;
    }
    if (__VdlySet__test_copper_lists__DOT__dut__DOT__regs__v0) {
        test_copper_lists__DOT__dut__DOT__spr_y[0U] = 0U;
        test_copper_lists__DOT__dut__DOT__spr_y[1U] = 0U;
        test_copper_lists__DOT__dut__DOT__spr_y[2U] = 0U;
        test_copper_lists__DOT__dut__DOT__spr_y[3U] = 0U;
        test_copper_lists__DOT__dut__DOT__spr_y[4U] = 0U;
        test_copper_lists__DOT__dut__DOT__spr_y[5U] = 0U;
        test_copper_lists__DOT__dut__DOT__spr_y[6U] = 0U;
        test_copper_lists__DOT__dut__DOT__spr_y[7U] = 0U;
        test_copper_lists__DOT__dut__DOT__spr_y[8U] = 0U;
        test_copper_lists__DOT__dut__DOT__spr_y[9U] = 0U;
        test_copper_lists__DOT__dut__DOT__spr_y[10U] = 0U;
        test_copper_lists__DOT__dut__DOT__spr_y[11U] = 0U;
        test_copper_lists__DOT__dut__DOT__spr_y[12U] = 0U;
        test_copper_lists__DOT__dut__DOT__spr_y[13U] = 0U;
        test_copper_lists__DOT__dut__DOT__spr_y[14U] = 0U;
        test_copper_lists__DOT__dut__DOT__spr_y[15U] = 0U;
        test_copper_lists__DOT__dut__DOT__spr_flip_h[0U] = 0U;
        test_copper_lists__DOT__dut__DOT__spr_flip_h[1U] = 0U;
        test_copper_lists__DOT__dut__DOT__spr_flip_h[2U] = 0U;
        test_copper_lists__DOT__dut__DOT__spr_flip_h[3U] = 0U;
        test_copper_lists__DOT__dut__DOT__spr_flip_h[4U] = 0U;
        test_copper_lists__DOT__dut__DOT__spr_flip_h[5U] = 0U;
        test_copper_lists__DOT__dut__DOT__spr_flip_h[6U] = 0U;
        test_copper_lists__DOT__dut__DOT__spr_flip_h[7U] = 0U;
        test_copper_lists__DOT__dut__DOT__spr_flip_h[8U] = 0U;
        test_copper_lists__DOT__dut__DOT__spr_flip_h[9U] = 0U;
        test_copper_lists__DOT__dut__DOT__spr_flip_h[10U] = 0U;
        test_copper_lists__DOT__dut__DOT__spr_flip_h[11U] = 0U;
        test_copper_lists__DOT__dut__DOT__spr_flip_h[12U] = 0U;
        test_copper_lists__DOT__dut__DOT__spr_flip_h[13U] = 0U;
        test_copper_lists__DOT__dut__DOT__spr_flip_h[14U] = 0U;
        test_copper_lists__DOT__dut__DOT__spr_flip_h[15U] = 0U;
        test_copper_lists__DOT__dut__DOT__spr_flip_v[0U] = 0U;
        test_copper_lists__DOT__dut__DOT__spr_flip_v[1U] = 0U;
        test_copper_lists__DOT__dut__DOT__spr_flip_v[2U] = 0U;
        test_copper_lists__DOT__dut__DOT__spr_flip_v[3U] = 0U;
        test_copper_lists__DOT__dut__DOT__spr_flip_v[4U] = 0U;
        test_copper_lists__DOT__dut__DOT__spr_flip_v[5U] = 0U;
        test_copper_lists__DOT__dut__DOT__spr_flip_v[6U] = 0U;
        test_copper_lists__DOT__dut__DOT__spr_flip_v[7U] = 0U;
        test_copper_lists__DOT__dut__DOT__spr_flip_v[8U] = 0U;
        test_copper_lists__DOT__dut__DOT__spr_flip_v[9U] = 0U;
        test_copper_lists__DOT__dut__DOT__spr_flip_v[10U] = 0U;
        test_copper_lists__DOT__dut__DOT__spr_flip_v[11U] = 0U;
        test_copper_lists__DOT__dut__DOT__spr_flip_v[12U] = 0U;
        test_copper_lists__DOT__dut__DOT__spr_flip_v[13U] = 0U;
        test_copper_lists__DOT__dut__DOT__spr_flip_v[14U] = 0U;
        test_copper_lists__DOT__dut__DOT__spr_flip_v[15U] = 0U;
        test_copper_lists__DOT__dut__DOT__spr_pri[0U] = 0U;
        test_copper_lists__DOT__dut__DOT__spr_pri[1U] = 0U;
        test_copper_lists__DOT__dut__DOT__spr_pri[2U] = 0U;
        test_copper_lists__DOT__dut__DOT__spr_pri[3U] = 0U;
        test_copper_lists__DOT__dut__DOT__spr_pri[4U] = 0U;
        test_copper_lists__DOT__dut__DOT__spr_pri[5U] = 0U;
        test_copper_lists__DOT__dut__DOT__spr_pri[6U] = 0U;
        test_copper_lists__DOT__dut__DOT__spr_pri[7U] = 0U;
        test_copper_lists__DOT__dut__DOT__spr_pri[8U] = 0U;
        test_copper_lists__DOT__dut__DOT__spr_pri[9U] = 0U;
        test_copper_lists__DOT__dut__DOT__spr_pri[10U] = 0U;
        test_copper_lists__DOT__dut__DOT__spr_pri[11U] = 0U;
        test_copper_lists__DOT__dut__DOT__spr_pri[12U] = 0U;
        test_copper_lists__DOT__dut__DOT__spr_pri[13U] = 0U;
        test_copper_lists__DOT__dut__DOT__spr_pri[14U] = 0U;
        test_copper_lists__DOT__dut__DOT__spr_pri[15U] = 0U;
        test_copper_lists__DOT__dut__DOT__spr_enable[0U] = 0U;
        test_copper_lists__DOT__dut__DOT__spr_enable[1U] = 0U;
        test_copper_lists__DOT__dut__DOT__spr_enable[2U] = 0U;
        test_copper_lists__DOT__dut__DOT__spr_enable[3U] = 0U;
        test_copper_lists__DOT__dut__DOT__spr_enable[4U] = 0U;
        test_copper_lists__DOT__dut__DOT__spr_enable[5U] = 0U;
        test_copper_lists__DOT__dut__DOT__spr_enable[6U] = 0U;
        test_copper_lists__DOT__dut__DOT__spr_enable[7U] = 0U;
        test_copper_lists__DOT__dut__DOT__spr_enable[8U] = 0U;
        test_copper_lists__DOT__dut__DOT__spr_enable[9U] = 0U;
        test_copper_lists__DOT__dut__DOT__spr_enable[10U] = 0U;
        test_copper_lists__DOT__dut__DOT__spr_enable[11U] = 0U;
        test_copper_lists__DOT__dut__DOT__spr_enable[12U] = 0U;
        test_copper_lists__DOT__dut__DOT__spr_enable[13U] = 0U;
        test_copper_lists__DOT__dut__DOT__spr_enable[14U] = 0U;
        test_copper_lists__DOT__dut__DOT__spr_enable[15U] = 0U;
        test_copper_lists__DOT__dut__DOT__spr_x[0U] = 0U;
        test_copper_lists__DOT__dut__DOT__spr_x[1U] = 0U;
        test_copper_lists__DOT__dut__DOT__spr_x[2U] = 0U;
        test_copper_lists__DOT__dut__DOT__spr_x[3U] = 0U;
        test_copper_lists__DOT__dut__DOT__spr_x[4U] = 0U;
        test_copper_lists__DOT__dut__DOT__spr_x[5U] = 0U;
        test_copper_lists__DOT__dut__DOT__spr_x[6U] = 0U;
        test_copper_lists__DOT__dut__DOT__spr_x[7U] = 0U;
        test_copper_lists__DOT__dut__DOT__spr_x[8U] = 0U;
        test_copper_lists__DOT__dut__DOT__spr_x[9U] = 0U;
        test_copper_lists__DOT__dut__DOT__spr_x[10U] = 0U;
        test_copper_lists__DOT__dut__DOT__spr_x[11U] = 0U;
        test_copper_lists__DOT__dut__DOT__spr_x[12U] = 0U;
        test_copper_lists__DOT__dut__DOT__spr_x[13U] = 0U;
        test_copper_lists__DOT__dut__DOT__spr_x[14U] = 0U;
        test_copper_lists__DOT__dut__DOT__spr_x[15U] = 0U;
        vlSelfRef.test_copper_lists__DOT__dut__DOT__regs[0U] = 0U;
        vlSelfRef.test_copper_lists__DOT__dut__DOT__regs[1U] = 0U;
        vlSelfRef.test_copper_lists__DOT__dut__DOT__regs[2U] = 0U;
        vlSelfRef.test_copper_lists__DOT__dut__DOT__regs[3U] = 0U;
        vlSelfRef.test_copper_lists__DOT__dut__DOT__regs[4U] = 0U;
        vlSelfRef.test_copper_lists__DOT__dut__DOT__regs[5U] = 0U;
        vlSelfRef.test_copper_lists__DOT__dut__DOT__regs[6U] = 0U;
        vlSelfRef.test_copper_lists__DOT__dut__DOT__regs[7U] = 0U;
        vlSelfRef.test_copper_lists__DOT__dut__DOT__regs[8U] = 0U;
        vlSelfRef.test_copper_lists__DOT__dut__DOT__regs[9U] = 0U;
        vlSelfRef.test_copper_lists__DOT__dut__DOT__regs[10U] = 0U;
        vlSelfRef.test_copper_lists__DOT__dut__DOT__regs[11U] = 0U;
        vlSelfRef.test_copper_lists__DOT__dut__DOT__regs[12U] = 0U;
        vlSelfRef.test_copper_lists__DOT__dut__DOT__regs[13U] = 0U;
        vlSelfRef.test_copper_lists__DOT__dut__DOT__regs[14U] = 0U;
        vlSelfRef.test_copper_lists__DOT__dut__DOT__regs[15U] = 0U;
        vlSelfRef.test_copper_lists__DOT__dut__DOT__regs[16U] = 0U;
        vlSelfRef.test_copper_lists__DOT__dut__DOT__regs[17U] = 0U;
        vlSelfRef.test_copper_lists__DOT__dut__DOT__regs[18U] = 0U;
        vlSelfRef.test_copper_lists__DOT__dut__DOT__regs[19U] = 0U;
        vlSelfRef.test_copper_lists__DOT__dut__DOT__regs[20U] = 0U;
        vlSelfRef.test_copper_lists__DOT__dut__DOT__regs[21U] = 0U;
        vlSelfRef.test_copper_lists__DOT__dut__DOT__regs[22U] = 0U;
        vlSelfRef.test_copper_lists__DOT__dut__DOT__regs[23U] = 0U;
        vlSelfRef.test_copper_lists__DOT__dut__DOT__regs[24U] = 0U;
        vlSelfRef.test_copper_lists__DOT__dut__DOT__regs[25U] = 0U;
        vlSelfRef.test_copper_lists__DOT__dut__DOT__regs[26U] = 0U;
        vlSelfRef.test_copper_lists__DOT__dut__DOT__regs[27U] = 0U;
        vlSelfRef.test_copper_lists__DOT__dut__DOT__regs[28U] = 0U;
        vlSelfRef.test_copper_lists__DOT__dut__DOT__regs[29U] = 0U;
        vlSelfRef.test_copper_lists__DOT__dut__DOT__regs[30U] = 0U;
        vlSelfRef.test_copper_lists__DOT__dut__DOT__regs[31U] = 0U;
    }
    if (__VdlySet__test_copper_lists__DOT__dut__DOT__spr_x__v16) {
        test_copper_lists__DOT__dut__DOT__spr_y[__VdlyDim0__test_copper_lists__DOT__dut__DOT__spr_y__v16] 
            = __VdlyVal__test_copper_lists__DOT__dut__DOT__spr_y__v16;
        test_copper_lists__DOT__dut__DOT__spr_x[__VdlyDim0__test_copper_lists__DOT__dut__DOT__spr_x__v16] 
            = __VdlyVal__test_copper_lists__DOT__dut__DOT__spr_x__v16;
    }
    if (__VdlySet__test_copper_lists__DOT__dut__DOT__spr_y__v17) {
        test_copper_lists__DOT__dut__DOT__spr_y[__VdlyDim0__test_copper_lists__DOT__dut__DOT__spr_y__v17] 
            = __VdlyVal__test_copper_lists__DOT__dut__DOT__spr_y__v17;
    }
    if (__VdlySet__test_copper_lists__DOT__dut__DOT__spr_flip_h__v16) {
        test_copper_lists__DOT__dut__DOT__spr_flip_h[__VdlyDim0__test_copper_lists__DOT__dut__DOT__spr_flip_h__v16] 
            = __VdlyVal__test_copper_lists__DOT__dut__DOT__spr_flip_h__v16;
        test_copper_lists__DOT__dut__DOT__spr_flip_v[__VdlyDim0__test_copper_lists__DOT__dut__DOT__spr_flip_v__v16] 
            = __VdlyVal__test_copper_lists__DOT__dut__DOT__spr_flip_v__v16;
    }
    if (__VdlySet__test_copper_lists__DOT__dut__DOT__spr_pri__v16) {
        test_copper_lists__DOT__dut__DOT__spr_pri[__VdlyDim0__test_copper_lists__DOT__dut__DOT__spr_pri__v16] 
            = __VdlyVal__test_copper_lists__DOT__dut__DOT__spr_pri__v16;
    }
    if (__VdlySet__test_copper_lists__DOT__dut__DOT__spr_pri__v17) {
        test_copper_lists__DOT__dut__DOT__spr_pri[__VdlyDim0__test_copper_lists__DOT__dut__DOT__spr_pri__v17] 
            = __VdlyVal__test_copper_lists__DOT__dut__DOT__spr_pri__v17;
    }
    if (__VdlySet__test_copper_lists__DOT__dut__DOT__spr_enable__v16) {
        test_copper_lists__DOT__dut__DOT__spr_enable[__VdlyDim0__test_copper_lists__DOT__dut__DOT__spr_enable__v16] = 0U;
    }
    if (__VdlySet__test_copper_lists__DOT__dut__DOT__spr_enable__v17) {
        test_copper_lists__DOT__dut__DOT__spr_enable[__VdlyDim0__test_copper_lists__DOT__dut__DOT__spr_enable__v17] = 1U;
    }
    if (__VdlySet__test_copper_lists__DOT__dut__DOT__spr_flip_h__v17) {
        test_copper_lists__DOT__dut__DOT__spr_flip_h[__VdlyDim0__test_copper_lists__DOT__dut__DOT__spr_flip_h__v17] 
            = __VdlyVal__test_copper_lists__DOT__dut__DOT__spr_flip_h__v17;
        test_copper_lists__DOT__dut__DOT__spr_flip_v[__VdlyDim0__test_copper_lists__DOT__dut__DOT__spr_flip_v__v17] 
            = __VdlyVal__test_copper_lists__DOT__dut__DOT__spr_flip_v__v17;
        test_copper_lists__DOT__dut__DOT__spr_enable[__VdlyDim0__test_copper_lists__DOT__dut__DOT__spr_enable__v18] 
            = __VdlyVal__test_copper_lists__DOT__dut__DOT__spr_enable__v18;
    }
    if (__VdlySet__test_copper_lists__DOT__dut__DOT__spr_x__v17) {
        test_copper_lists__DOT__dut__DOT__spr_x[__VdlyDim0__test_copper_lists__DOT__dut__DOT__spr_x__v17] 
            = ((0x00ffU & test_copper_lists__DOT__dut__DOT__spr_x
                [__VdlyDim0__test_copper_lists__DOT__dut__DOT__spr_x__v17]) 
               | ((IData)(__VdlyVal__test_copper_lists__DOT__dut__DOT__spr_x__v17) 
                  << 8U));
    }
    if (__VdlySet__test_copper_lists__DOT__dut__DOT__spr_x__v18) {
        test_copper_lists__DOT__dut__DOT__spr_x[__VdlyDim0__test_copper_lists__DOT__dut__DOT__spr_x__v18] 
            = ((0x0100U & test_copper_lists__DOT__dut__DOT__spr_x
                [__VdlyDim0__test_copper_lists__DOT__dut__DOT__spr_x__v18]) 
               | (IData)(__VdlyVal__test_copper_lists__DOT__dut__DOT__spr_x__v18));
    }
    if (__VdlySet__test_copper_lists__DOT__dut__DOT__regs__v32) {
        vlSelfRef.test_copper_lists__DOT__dut__DOT__regs[20U] 
            = __VdlyVal__test_copper_lists__DOT__dut__DOT__regs__v32;
    }
    if (__VdlySet__test_copper_lists__DOT__dut__DOT__regs__v33) {
        vlSelfRef.test_copper_lists__DOT__dut__DOT__regs[20U] 
            = __VdlyVal__test_copper_lists__DOT__dut__DOT__regs__v33;
    }
    if (__VdlySet__test_copper_lists__DOT__dut__DOT__regs__v34) {
        vlSelfRef.test_copper_lists__DOT__dut__DOT__regs[__VdlyDim0__test_copper_lists__DOT__dut__DOT__regs__v34] 
            = __VdlyVal__test_copper_lists__DOT__dut__DOT__regs__v34;
    }
    if (__VdlySet__test_copper_lists__DOT__dut__DOT__regs__v35) {
        vlSelfRef.test_copper_lists__DOT__dut__DOT__regs[__VdlyDim0__test_copper_lists__DOT__dut__DOT__regs__v35] 
            = __VdlyVal__test_copper_lists__DOT__dut__DOT__regs__v35;
    }
    vlSelfRef.test_copper_lists__DOT__dut__DOT__gfx_a_we = 0U;
    vlSelfRef.test_copper_lists__DOT__dut__DOT__gfx_a_din = 0U;
    vlSelfRef.test_copper_lists__DOT__dut__DOT__spr_a_dout 
        = vlSelfRef.test_copper_lists__DOT__dut__DOT__sprite_inst__DOT__spr_mem__DOT__mem
        [vlSelfRef.test_copper_lists__DOT__dut__DOT__spr_a_addr];
    vlSelfRef.test_copper_lists__DOT__dut__DOT__color_a_dout 
        = ((0x07cfU >= (IData)(vlSelfRef.test_copper_lists__DOT__dut__DOT__color_a_addr))
            ? vlSelfRef.test_copper_lists__DOT__dut__DOT__text_inst__DOT__color_mem__DOT__mem
           [vlSelfRef.test_copper_lists__DOT__dut__DOT__color_a_addr]
            : 0U);
    vlSelfRef.test_copper_lists__DOT__dut__DOT__color_a_addr 
        = (0x000007ffU & ((IData)(vlSelfRef.test_copper_lists__DOT__dut__DOT__cmd_color_we)
                           ? (IData)(vlSelfRef.test_copper_lists__DOT__dut__DOT__cmd_color_addr)
                           : (((IData)(__VdfgRegularize_hebeb780c_0_0) 
                               & (IData)(vlSelfRef.test_copper_lists__DOT__dut__DOT__color_ram_sel))
                               ? ((IData)(vlSelfRef.test_copper_lists__DOT__cpu_addr) 
                                  - (IData)(0x01d0U))
                               : ((IData)(vlSelfRef.test_copper_lists__DOT__dut__DOT__dbg_color_sel)
                                   ? ((IData)(vlSelfRef.test_copper_lists__DOT__dut__DOT__dbg_addr) 
                                      - (IData)(0x01d0U))
                                   : (IData)(vlSelfRef.test_copper_lists__DOT__dut__DOT__cmd_color_addr)))));
    vlSelfRef.test_copper_lists__DOT__dut__DOT__char_a_dout 
        = ((0x07cfU >= (IData)(vlSelfRef.test_copper_lists__DOT__dut__DOT__char_a_addr))
            ? vlSelfRef.test_copper_lists__DOT__dut__DOT__text_inst__DOT__char_mem__DOT__mem
           [vlSelfRef.test_copper_lists__DOT__dut__DOT__char_a_addr]
            : 0U);
    vlSelfRef.test_copper_lists__DOT__dut__DOT__char_a_addr 
        = (0x000007ffU & ((IData)(vlSelfRef.test_copper_lists__DOT__dut__DOT__cmd_char_we)
                           ? (IData)(vlSelfRef.test_copper_lists__DOT__dut__DOT__cmd_char_addr)
                           : (((IData)(__VdfgRegularize_hebeb780c_0_0) 
                               & (IData)(vlSelfRef.test_copper_lists__DOT__dut__DOT__char_ram_sel))
                               ? ((IData)(vlSelfRef.test_copper_lists__DOT__cpu_addr) 
                                  - (IData)(0x0200U))
                               : ((IData)(vlSelfRef.test_copper_lists__DOT__dut__DOT__dbg_char_sel)
                                   ? ((IData)(vlSelfRef.test_copper_lists__DOT__dut__DOT__dbg_addr) 
                                      - (IData)(0x0200U))
                                   : (IData)(vlSelfRef.test_copper_lists__DOT__dut__DOT__cmd_char_addr)))));
    vlSelfRef.test_copper_lists__DOT__dut__DOT__gfx_a_dout 
        = ((0xf9ffU >= (IData)(vlSelfRef.test_copper_lists__DOT__dut__DOT__gfx_a_addr))
            ? vlSelfRef.test_copper_lists__DOT__dut__DOT__gfx_inst__DOT__gfx_mem__DOT__mem
           [vlSelfRef.test_copper_lists__DOT__dut__DOT__gfx_a_addr]
            : 0U);
    vlSelfRef.test_copper_lists__DOT__dut__DOT__copper_reg_flat[0U] 
        = ((0xff000000U & vlSelfRef.test_copper_lists__DOT__dut__DOT__copper_reg_flat[0U]) 
           | ((vlSelfRef.test_copper_lists__DOT__dut__DOT__copper_reg[2U] 
               << 0x00000010U) | ((vlSelfRef.test_copper_lists__DOT__dut__DOT__copper_reg[1U] 
                                   << 8U) | vlSelfRef.test_copper_lists__DOT__dut__DOT__copper_reg[0U])));
    vlSelfRef.test_copper_lists__DOT__dut__DOT__copper_reg_flat[0U] 
        = ((0x00ffffffU & vlSelfRef.test_copper_lists__DOT__dut__DOT__copper_reg_flat[0U]) 
           | (vlSelfRef.test_copper_lists__DOT__dut__DOT__copper_reg[3U] 
              << 0x00000018U));
    vlSelfRef.test_copper_lists__DOT__dut__DOT__copper_reg_flat[1U] 
        = ((0xffff0000U & vlSelfRef.test_copper_lists__DOT__dut__DOT__copper_reg_flat[1U]) 
           | (0x00ffffffU & ((0x00ffff00U & (vlSelfRef.test_copper_lists__DOT__dut__DOT__copper_reg[5U] 
                                             << 8U)) 
                             | ((0x00ffffffU & vlSelfRef.test_copper_lists__DOT__dut__DOT__copper_reg[4U]) 
                                | (vlSelfRef.test_copper_lists__DOT__dut__DOT__copper_reg[3U] 
                                   >> 8U)))));
    vlSelfRef.test_copper_lists__DOT__dut__DOT__copper_reg_flat[1U] 
        = ((0x0000ffffU & vlSelfRef.test_copper_lists__DOT__dut__DOT__copper_reg_flat[1U]) 
           | (0xffff0000U & ((vlSelfRef.test_copper_lists__DOT__dut__DOT__copper_reg[7U] 
                              << 0x00000018U) | (vlSelfRef.test_copper_lists__DOT__dut__DOT__copper_reg[6U] 
                                                 << 0x00000010U))));
    vlSelfRef.test_copper_lists__DOT__dut__DOT__copper_reg_flat[2U] 
        = ((0xffffff00U & vlSelfRef.test_copper_lists__DOT__dut__DOT__copper_reg_flat[2U]) 
           | (0x0000ffffU & ((0x0000ffffU & vlSelfRef.test_copper_lists__DOT__dut__DOT__copper_reg[8U]) 
                             | ((0x0000ffffU & (vlSelfRef.test_copper_lists__DOT__dut__DOT__copper_reg[7U] 
                                                >> 8U)) 
                                | (vlSelfRef.test_copper_lists__DOT__dut__DOT__copper_reg[6U] 
                                   >> 0x00000010U)))));
    vlSelfRef.test_copper_lists__DOT__dut__DOT__copper_reg_flat[2U] 
        = ((0x000000ffU & vlSelfRef.test_copper_lists__DOT__dut__DOT__copper_reg_flat[2U]) 
           | (0xffffff00U & ((vlSelfRef.test_copper_lists__DOT__dut__DOT__copper_reg[11U] 
                              << 0x00000018U) | ((vlSelfRef.test_copper_lists__DOT__dut__DOT__copper_reg[10U] 
                                                  << 0x00000010U) 
                                                 | (vlSelfRef.test_copper_lists__DOT__dut__DOT__copper_reg[9U] 
                                                    << 8U)))));
    vlSelfRef.test_copper_lists__DOT__dut__DOT__copper_reg_flat[3U] 
        = ((0xff000000U & vlSelfRef.test_copper_lists__DOT__dut__DOT__copper_reg_flat[3U]) 
           | ((vlSelfRef.test_copper_lists__DOT__dut__DOT__copper_reg[14U] 
               << 0x00000010U) | ((vlSelfRef.test_copper_lists__DOT__dut__DOT__copper_reg[13U] 
                                   << 8U) | vlSelfRef.test_copper_lists__DOT__dut__DOT__copper_reg[12U])));
    vlSelfRef.test_copper_lists__DOT__dut__DOT__copper_reg_flat[3U] 
        = ((0x00ffffffU & vlSelfRef.test_copper_lists__DOT__dut__DOT__copper_reg_flat[3U]) 
           | (vlSelfRef.test_copper_lists__DOT__dut__DOT__copper_reg[15U] 
              << 0x00000018U));
    vlSelfRef.test_copper_lists__DOT__dut__DOT__copper_reg_flat[4U] 
        = ((0xffff0000U & vlSelfRef.test_copper_lists__DOT__dut__DOT__copper_reg_flat[4U]) 
           | (0x00ffffffU & ((0x00ffff00U & (vlSelfRef.test_copper_lists__DOT__dut__DOT__copper_reg[17U] 
                                             << 8U)) 
                             | ((0x00ffffffU & vlSelfRef.test_copper_lists__DOT__dut__DOT__copper_reg[16U]) 
                                | (vlSelfRef.test_copper_lists__DOT__dut__DOT__copper_reg[15U] 
                                   >> 8U)))));
    vlSelfRef.test_copper_lists__DOT__dut__DOT__copper_reg_flat[4U] 
        = ((0x0000ffffU & vlSelfRef.test_copper_lists__DOT__dut__DOT__copper_reg_flat[4U]) 
           | (0xffff0000U & ((vlSelfRef.test_copper_lists__DOT__dut__DOT__copper_reg[19U] 
                              << 0x00000018U) | (vlSelfRef.test_copper_lists__DOT__dut__DOT__copper_reg[18U] 
                                                 << 0x00000010U))));
    vlSelfRef.test_copper_lists__DOT__dut__DOT__copper_reg_flat[5U] 
        = ((0xffffff00U & vlSelfRef.test_copper_lists__DOT__dut__DOT__copper_reg_flat[5U]) 
           | (0x0000ffffU & ((0x0000ffffU & vlSelfRef.test_copper_lists__DOT__dut__DOT__copper_reg[20U]) 
                             | ((0x0000ffffU & (vlSelfRef.test_copper_lists__DOT__dut__DOT__copper_reg[19U] 
                                                >> 8U)) 
                                | (vlSelfRef.test_copper_lists__DOT__dut__DOT__copper_reg[18U] 
                                   >> 0x00000010U)))));
    vlSelfRef.test_copper_lists__DOT__dut__DOT__copper_reg_flat[5U] 
        = ((0x000000ffU & vlSelfRef.test_copper_lists__DOT__dut__DOT__copper_reg_flat[5U]) 
           | (0xffffff00U & ((vlSelfRef.test_copper_lists__DOT__dut__DOT__copper_reg[23U] 
                              << 0x00000018U) | ((vlSelfRef.test_copper_lists__DOT__dut__DOT__copper_reg[22U] 
                                                  << 0x00000010U) 
                                                 | (vlSelfRef.test_copper_lists__DOT__dut__DOT__copper_reg[21U] 
                                                    << 8U)))));
    vlSelfRef.test_copper_lists__DOT__dut__DOT__copper_reg_flat[6U] 
        = ((0xff000000U & vlSelfRef.test_copper_lists__DOT__dut__DOT__copper_reg_flat[6U]) 
           | ((vlSelfRef.test_copper_lists__DOT__dut__DOT__copper_reg[26U] 
               << 0x00000010U) | ((vlSelfRef.test_copper_lists__DOT__dut__DOT__copper_reg[25U] 
                                   << 8U) | vlSelfRef.test_copper_lists__DOT__dut__DOT__copper_reg[24U])));
    vlSelfRef.test_copper_lists__DOT__dut__DOT__copper_reg_flat[6U] 
        = ((0x00ffffffU & vlSelfRef.test_copper_lists__DOT__dut__DOT__copper_reg_flat[6U]) 
           | (vlSelfRef.test_copper_lists__DOT__dut__DOT__copper_reg[27U] 
              << 0x00000018U));
    vlSelfRef.test_copper_lists__DOT__dut__DOT__copper_reg_flat[7U] 
        = ((0xffff0000U & vlSelfRef.test_copper_lists__DOT__dut__DOT__copper_reg_flat[7U]) 
           | (0x00ffffffU & ((0x00ffff00U & (vlSelfRef.test_copper_lists__DOT__dut__DOT__copper_reg[29U] 
                                             << 8U)) 
                             | ((0x00ffffffU & vlSelfRef.test_copper_lists__DOT__dut__DOT__copper_reg[28U]) 
                                | (vlSelfRef.test_copper_lists__DOT__dut__DOT__copper_reg[27U] 
                                   >> 8U)))));
    vlSelfRef.test_copper_lists__DOT__dut__DOT__copper_reg_flat[7U] 
        = ((0x0000ffffU & vlSelfRef.test_copper_lists__DOT__dut__DOT__copper_reg_flat[7U]) 
           | (0xffff0000U & ((vlSelfRef.test_copper_lists__DOT__dut__DOT__copper_reg[31U] 
                              << 0x00000018U) | (vlSelfRef.test_copper_lists__DOT__dut__DOT__copper_reg[30U] 
                                                 << 0x00000010U))));
    vlSelfRef.test_copper_lists__DOT__dut__DOT__copper_val_flat[0U] 
        = ((0xff000000U & vlSelfRef.test_copper_lists__DOT__dut__DOT__copper_val_flat[0U]) 
           | ((vlSelfRef.test_copper_lists__DOT__dut__DOT__copper_val[2U] 
               << 0x00000010U) | ((vlSelfRef.test_copper_lists__DOT__dut__DOT__copper_val[1U] 
                                   << 8U) | vlSelfRef.test_copper_lists__DOT__dut__DOT__copper_val[0U])));
    vlSelfRef.test_copper_lists__DOT__dut__DOT__copper_val_flat[0U] 
        = ((0x00ffffffU & vlSelfRef.test_copper_lists__DOT__dut__DOT__copper_val_flat[0U]) 
           | (vlSelfRef.test_copper_lists__DOT__dut__DOT__copper_val[3U] 
              << 0x00000018U));
    vlSelfRef.test_copper_lists__DOT__dut__DOT__copper_val_flat[1U] 
        = ((0xffff0000U & vlSelfRef.test_copper_lists__DOT__dut__DOT__copper_val_flat[1U]) 
           | (0x00ffffffU & ((0x00ffff00U & (vlSelfRef.test_copper_lists__DOT__dut__DOT__copper_val[5U] 
                                             << 8U)) 
                             | ((0x00ffffffU & vlSelfRef.test_copper_lists__DOT__dut__DOT__copper_val[4U]) 
                                | (vlSelfRef.test_copper_lists__DOT__dut__DOT__copper_val[3U] 
                                   >> 8U)))));
    vlSelfRef.test_copper_lists__DOT__dut__DOT__copper_val_flat[1U] 
        = ((0x0000ffffU & vlSelfRef.test_copper_lists__DOT__dut__DOT__copper_val_flat[1U]) 
           | (0xffff0000U & ((vlSelfRef.test_copper_lists__DOT__dut__DOT__copper_val[7U] 
                              << 0x00000018U) | (vlSelfRef.test_copper_lists__DOT__dut__DOT__copper_val[6U] 
                                                 << 0x00000010U))));
    vlSelfRef.test_copper_lists__DOT__dut__DOT__copper_val_flat[2U] 
        = ((0xffffff00U & vlSelfRef.test_copper_lists__DOT__dut__DOT__copper_val_flat[2U]) 
           | (0x0000ffffU & ((0x0000ffffU & vlSelfRef.test_copper_lists__DOT__dut__DOT__copper_val[8U]) 
                             | ((0x0000ffffU & (vlSelfRef.test_copper_lists__DOT__dut__DOT__copper_val[7U] 
                                                >> 8U)) 
                                | (vlSelfRef.test_copper_lists__DOT__dut__DOT__copper_val[6U] 
                                   >> 0x00000010U)))));
    vlSelfRef.test_copper_lists__DOT__dut__DOT__copper_val_flat[2U] 
        = ((0x000000ffU & vlSelfRef.test_copper_lists__DOT__dut__DOT__copper_val_flat[2U]) 
           | (0xffffff00U & ((vlSelfRef.test_copper_lists__DOT__dut__DOT__copper_val[11U] 
                              << 0x00000018U) | ((vlSelfRef.test_copper_lists__DOT__dut__DOT__copper_val[10U] 
                                                  << 0x00000010U) 
                                                 | (vlSelfRef.test_copper_lists__DOT__dut__DOT__copper_val[9U] 
                                                    << 8U)))));
    vlSelfRef.test_copper_lists__DOT__dut__DOT__copper_val_flat[3U] 
        = ((0xff000000U & vlSelfRef.test_copper_lists__DOT__dut__DOT__copper_val_flat[3U]) 
           | ((vlSelfRef.test_copper_lists__DOT__dut__DOT__copper_val[14U] 
               << 0x00000010U) | ((vlSelfRef.test_copper_lists__DOT__dut__DOT__copper_val[13U] 
                                   << 8U) | vlSelfRef.test_copper_lists__DOT__dut__DOT__copper_val[12U])));
    vlSelfRef.test_copper_lists__DOT__dut__DOT__copper_val_flat[3U] 
        = ((0x00ffffffU & vlSelfRef.test_copper_lists__DOT__dut__DOT__copper_val_flat[3U]) 
           | (vlSelfRef.test_copper_lists__DOT__dut__DOT__copper_val[15U] 
              << 0x00000018U));
    vlSelfRef.test_copper_lists__DOT__dut__DOT__copper_val_flat[4U] 
        = ((0xffff0000U & vlSelfRef.test_copper_lists__DOT__dut__DOT__copper_val_flat[4U]) 
           | (0x00ffffffU & ((0x00ffff00U & (vlSelfRef.test_copper_lists__DOT__dut__DOT__copper_val[17U] 
                                             << 8U)) 
                             | ((0x00ffffffU & vlSelfRef.test_copper_lists__DOT__dut__DOT__copper_val[16U]) 
                                | (vlSelfRef.test_copper_lists__DOT__dut__DOT__copper_val[15U] 
                                   >> 8U)))));
    vlSelfRef.test_copper_lists__DOT__dut__DOT__copper_val_flat[4U] 
        = ((0x0000ffffU & vlSelfRef.test_copper_lists__DOT__dut__DOT__copper_val_flat[4U]) 
           | (0xffff0000U & ((vlSelfRef.test_copper_lists__DOT__dut__DOT__copper_val[19U] 
                              << 0x00000018U) | (vlSelfRef.test_copper_lists__DOT__dut__DOT__copper_val[18U] 
                                                 << 0x00000010U))));
    vlSelfRef.test_copper_lists__DOT__dut__DOT__copper_val_flat[5U] 
        = ((0xffffff00U & vlSelfRef.test_copper_lists__DOT__dut__DOT__copper_val_flat[5U]) 
           | (0x0000ffffU & ((0x0000ffffU & vlSelfRef.test_copper_lists__DOT__dut__DOT__copper_val[20U]) 
                             | ((0x0000ffffU & (vlSelfRef.test_copper_lists__DOT__dut__DOT__copper_val[19U] 
                                                >> 8U)) 
                                | (vlSelfRef.test_copper_lists__DOT__dut__DOT__copper_val[18U] 
                                   >> 0x00000010U)))));
    vlSelfRef.test_copper_lists__DOT__dut__DOT__copper_val_flat[5U] 
        = ((0x000000ffU & vlSelfRef.test_copper_lists__DOT__dut__DOT__copper_val_flat[5U]) 
           | (0xffffff00U & ((vlSelfRef.test_copper_lists__DOT__dut__DOT__copper_val[23U] 
                              << 0x00000018U) | ((vlSelfRef.test_copper_lists__DOT__dut__DOT__copper_val[22U] 
                                                  << 0x00000010U) 
                                                 | (vlSelfRef.test_copper_lists__DOT__dut__DOT__copper_val[21U] 
                                                    << 8U)))));
    vlSelfRef.test_copper_lists__DOT__dut__DOT__copper_val_flat[6U] 
        = ((0xff000000U & vlSelfRef.test_copper_lists__DOT__dut__DOT__copper_val_flat[6U]) 
           | ((vlSelfRef.test_copper_lists__DOT__dut__DOT__copper_val[26U] 
               << 0x00000010U) | ((vlSelfRef.test_copper_lists__DOT__dut__DOT__copper_val[25U] 
                                   << 8U) | vlSelfRef.test_copper_lists__DOT__dut__DOT__copper_val[24U])));
    vlSelfRef.test_copper_lists__DOT__dut__DOT__copper_val_flat[6U] 
        = ((0x00ffffffU & vlSelfRef.test_copper_lists__DOT__dut__DOT__copper_val_flat[6U]) 
           | (vlSelfRef.test_copper_lists__DOT__dut__DOT__copper_val[27U] 
              << 0x00000018U));
    vlSelfRef.test_copper_lists__DOT__dut__DOT__copper_val_flat[7U] 
        = ((0xffff0000U & vlSelfRef.test_copper_lists__DOT__dut__DOT__copper_val_flat[7U]) 
           | (0x00ffffffU & ((0x00ffff00U & (vlSelfRef.test_copper_lists__DOT__dut__DOT__copper_val[29U] 
                                             << 8U)) 
                             | ((0x00ffffffU & vlSelfRef.test_copper_lists__DOT__dut__DOT__copper_val[28U]) 
                                | (vlSelfRef.test_copper_lists__DOT__dut__DOT__copper_val[27U] 
                                   >> 8U)))));
    vlSelfRef.test_copper_lists__DOT__dut__DOT__copper_val_flat[7U] 
        = ((0x0000ffffU & vlSelfRef.test_copper_lists__DOT__dut__DOT__copper_val_flat[7U]) 
           | (0xffff0000U & ((vlSelfRef.test_copper_lists__DOT__dut__DOT__copper_val[31U] 
                              << 0x00000018U) | (vlSelfRef.test_copper_lists__DOT__dut__DOT__copper_val[30U] 
                                                 << 0x00000010U))));
    test_copper_lists__DOT__dut__DOT____Vlvbound_h3e9a9ec3__0 
        = vlSelfRef.test_copper_lists__DOT__dut__DOT__copper_pos[0U];
    vlSelfRef.test_copper_lists__DOT__dut__DOT__copper_pos_flat[0U] 
        = ((0xfffe0000U & vlSelfRef.test_copper_lists__DOT__dut__DOT__copper_pos_flat[0U]) 
           | test_copper_lists__DOT__dut__DOT____Vlvbound_h3e9a9ec3__0);
    test_copper_lists__DOT__dut__DOT____Vlvbound_h3e9a9ec3__0 
        = vlSelfRef.test_copper_lists__DOT__dut__DOT__copper_pos[1U];
    vlSelfRef.test_copper_lists__DOT__dut__DOT__copper_pos_flat[0U] 
        = ((0x0001ffffU & vlSelfRef.test_copper_lists__DOT__dut__DOT__copper_pos_flat[0U]) 
           | (test_copper_lists__DOT__dut__DOT____Vlvbound_h3e9a9ec3__0 
              << 0x00000011U));
    vlSelfRef.test_copper_lists__DOT__dut__DOT__copper_pos_flat[1U] 
        = ((0xfffffffcU & vlSelfRef.test_copper_lists__DOT__dut__DOT__copper_pos_flat[1U]) 
           | (test_copper_lists__DOT__dut__DOT____Vlvbound_h3e9a9ec3__0 
              >> 0x0000000fU));
    test_copper_lists__DOT__dut__DOT____Vlvbound_h3e9a9ec3__0 
        = vlSelfRef.test_copper_lists__DOT__dut__DOT__copper_pos[2U];
    vlSelfRef.test_copper_lists__DOT__dut__DOT__copper_pos_flat[1U] 
        = ((0xfff80003U & vlSelfRef.test_copper_lists__DOT__dut__DOT__copper_pos_flat[1U]) 
           | (test_copper_lists__DOT__dut__DOT____Vlvbound_h3e9a9ec3__0 
              << 2U));
    test_copper_lists__DOT__dut__DOT____Vlvbound_h3e9a9ec3__0 
        = vlSelfRef.test_copper_lists__DOT__dut__DOT__copper_pos[3U];
    vlSelfRef.test_copper_lists__DOT__dut__DOT__copper_pos_flat[1U] 
        = ((0x0007ffffU & vlSelfRef.test_copper_lists__DOT__dut__DOT__copper_pos_flat[1U]) 
           | (test_copper_lists__DOT__dut__DOT____Vlvbound_h3e9a9ec3__0 
              << 0x00000013U));
    vlSelfRef.test_copper_lists__DOT__dut__DOT__copper_pos_flat[2U] 
        = ((0xfffffff0U & vlSelfRef.test_copper_lists__DOT__dut__DOT__copper_pos_flat[2U]) 
           | (test_copper_lists__DOT__dut__DOT____Vlvbound_h3e9a9ec3__0 
              >> 0x0000000dU));
    test_copper_lists__DOT__dut__DOT____Vlvbound_h3e9a9ec3__0 
        = vlSelfRef.test_copper_lists__DOT__dut__DOT__copper_pos[4U];
    vlSelfRef.test_copper_lists__DOT__dut__DOT__copper_pos_flat[2U] 
        = ((0xffe0000fU & vlSelfRef.test_copper_lists__DOT__dut__DOT__copper_pos_flat[2U]) 
           | (test_copper_lists__DOT__dut__DOT____Vlvbound_h3e9a9ec3__0 
              << 4U));
    test_copper_lists__DOT__dut__DOT____Vlvbound_h3e9a9ec3__0 
        = vlSelfRef.test_copper_lists__DOT__dut__DOT__copper_pos[5U];
    vlSelfRef.test_copper_lists__DOT__dut__DOT__copper_pos_flat[2U] 
        = ((0x001fffffU & vlSelfRef.test_copper_lists__DOT__dut__DOT__copper_pos_flat[2U]) 
           | (test_copper_lists__DOT__dut__DOT____Vlvbound_h3e9a9ec3__0 
              << 0x00000015U));
    vlSelfRef.test_copper_lists__DOT__dut__DOT__copper_pos_flat[3U] 
        = ((0xffffffc0U & vlSelfRef.test_copper_lists__DOT__dut__DOT__copper_pos_flat[3U]) 
           | (test_copper_lists__DOT__dut__DOT____Vlvbound_h3e9a9ec3__0 
              >> 0x0000000bU));
    test_copper_lists__DOT__dut__DOT____Vlvbound_h3e9a9ec3__0 
        = vlSelfRef.test_copper_lists__DOT__dut__DOT__copper_pos[6U];
    vlSelfRef.test_copper_lists__DOT__dut__DOT__copper_pos_flat[3U] 
        = ((0xff80003fU & vlSelfRef.test_copper_lists__DOT__dut__DOT__copper_pos_flat[3U]) 
           | (test_copper_lists__DOT__dut__DOT____Vlvbound_h3e9a9ec3__0 
              << 6U));
    test_copper_lists__DOT__dut__DOT____Vlvbound_h3e9a9ec3__0 
        = vlSelfRef.test_copper_lists__DOT__dut__DOT__copper_pos[7U];
    vlSelfRef.test_copper_lists__DOT__dut__DOT__copper_pos_flat[3U] 
        = ((0x007fffffU & vlSelfRef.test_copper_lists__DOT__dut__DOT__copper_pos_flat[3U]) 
           | (test_copper_lists__DOT__dut__DOT____Vlvbound_h3e9a9ec3__0 
              << 0x00000017U));
    vlSelfRef.test_copper_lists__DOT__dut__DOT__copper_pos_flat[4U] 
        = ((0xffffff00U & vlSelfRef.test_copper_lists__DOT__dut__DOT__copper_pos_flat[4U]) 
           | (test_copper_lists__DOT__dut__DOT____Vlvbound_h3e9a9ec3__0 
              >> 9U));
    test_copper_lists__DOT__dut__DOT____Vlvbound_h3e9a9ec3__0 
        = vlSelfRef.test_copper_lists__DOT__dut__DOT__copper_pos[8U];
    vlSelfRef.test_copper_lists__DOT__dut__DOT__copper_pos_flat[4U] 
        = ((0xfe0000ffU & vlSelfRef.test_copper_lists__DOT__dut__DOT__copper_pos_flat[4U]) 
           | (test_copper_lists__DOT__dut__DOT____Vlvbound_h3e9a9ec3__0 
              << 8U));
    test_copper_lists__DOT__dut__DOT____Vlvbound_h3e9a9ec3__0 
        = vlSelfRef.test_copper_lists__DOT__dut__DOT__copper_pos[9U];
    vlSelfRef.test_copper_lists__DOT__dut__DOT__copper_pos_flat[4U] 
        = ((0x01ffffffU & vlSelfRef.test_copper_lists__DOT__dut__DOT__copper_pos_flat[4U]) 
           | (test_copper_lists__DOT__dut__DOT____Vlvbound_h3e9a9ec3__0 
              << 0x00000019U));
    vlSelfRef.test_copper_lists__DOT__dut__DOT__copper_pos_flat[5U] 
        = ((0xfffffc00U & vlSelfRef.test_copper_lists__DOT__dut__DOT__copper_pos_flat[5U]) 
           | (test_copper_lists__DOT__dut__DOT____Vlvbound_h3e9a9ec3__0 
              >> 7U));
    test_copper_lists__DOT__dut__DOT____Vlvbound_h3e9a9ec3__0 
        = vlSelfRef.test_copper_lists__DOT__dut__DOT__copper_pos[10U];
    vlSelfRef.test_copper_lists__DOT__dut__DOT__copper_pos_flat[5U] 
        = ((0xf80003ffU & vlSelfRef.test_copper_lists__DOT__dut__DOT__copper_pos_flat[5U]) 
           | (test_copper_lists__DOT__dut__DOT____Vlvbound_h3e9a9ec3__0 
              << 0x0000000aU));
    test_copper_lists__DOT__dut__DOT____Vlvbound_h3e9a9ec3__0 
        = vlSelfRef.test_copper_lists__DOT__dut__DOT__copper_pos[11U];
    vlSelfRef.test_copper_lists__DOT__dut__DOT__copper_pos_flat[5U] 
        = ((0x07ffffffU & vlSelfRef.test_copper_lists__DOT__dut__DOT__copper_pos_flat[5U]) 
           | (test_copper_lists__DOT__dut__DOT____Vlvbound_h3e9a9ec3__0 
              << 0x0000001bU));
    vlSelfRef.test_copper_lists__DOT__dut__DOT__copper_pos_flat[6U] 
        = ((0xfffff000U & vlSelfRef.test_copper_lists__DOT__dut__DOT__copper_pos_flat[6U]) 
           | (test_copper_lists__DOT__dut__DOT____Vlvbound_h3e9a9ec3__0 
              >> 5U));
    test_copper_lists__DOT__dut__DOT____Vlvbound_h3e9a9ec3__0 
        = vlSelfRef.test_copper_lists__DOT__dut__DOT__copper_pos[12U];
    vlSelfRef.test_copper_lists__DOT__dut__DOT__copper_pos_flat[6U] 
        = ((0xe0000fffU & vlSelfRef.test_copper_lists__DOT__dut__DOT__copper_pos_flat[6U]) 
           | (test_copper_lists__DOT__dut__DOT____Vlvbound_h3e9a9ec3__0 
              << 0x0000000cU));
    test_copper_lists__DOT__dut__DOT____Vlvbound_h3e9a9ec3__0 
        = vlSelfRef.test_copper_lists__DOT__dut__DOT__copper_pos[13U];
    vlSelfRef.test_copper_lists__DOT__dut__DOT__copper_pos_flat[6U] 
        = ((0x1fffffffU & vlSelfRef.test_copper_lists__DOT__dut__DOT__copper_pos_flat[6U]) 
           | (test_copper_lists__DOT__dut__DOT____Vlvbound_h3e9a9ec3__0 
              << 0x0000001dU));
    vlSelfRef.test_copper_lists__DOT__dut__DOT__copper_pos_flat[7U] 
        = ((0xffffc000U & vlSelfRef.test_copper_lists__DOT__dut__DOT__copper_pos_flat[7U]) 
           | (test_copper_lists__DOT__dut__DOT____Vlvbound_h3e9a9ec3__0 
              >> 3U));
    test_copper_lists__DOT__dut__DOT____Vlvbound_h3e9a9ec3__0 
        = vlSelfRef.test_copper_lists__DOT__dut__DOT__copper_pos[14U];
    vlSelfRef.test_copper_lists__DOT__dut__DOT__copper_pos_flat[7U] 
        = ((0x80003fffU & vlSelfRef.test_copper_lists__DOT__dut__DOT__copper_pos_flat[7U]) 
           | (test_copper_lists__DOT__dut__DOT____Vlvbound_h3e9a9ec3__0 
              << 0x0000000eU));
    test_copper_lists__DOT__dut__DOT____Vlvbound_h3e9a9ec3__0 
        = vlSelfRef.test_copper_lists__DOT__dut__DOT__copper_pos[15U];
    vlSelfRef.test_copper_lists__DOT__dut__DOT__copper_pos_flat[7U] 
        = ((0x7fffffffU & vlSelfRef.test_copper_lists__DOT__dut__DOT__copper_pos_flat[7U]) 
           | (test_copper_lists__DOT__dut__DOT____Vlvbound_h3e9a9ec3__0 
              << 0x0000001fU));
    vlSelfRef.test_copper_lists__DOT__dut__DOT__copper_pos_flat[8U] 
        = ((0xffff0000U & vlSelfRef.test_copper_lists__DOT__dut__DOT__copper_pos_flat[8U]) 
           | (test_copper_lists__DOT__dut__DOT____Vlvbound_h3e9a9ec3__0 
              >> 1U));
    test_copper_lists__DOT__dut__DOT____Vlvbound_h3e9a9ec3__0 
        = vlSelfRef.test_copper_lists__DOT__dut__DOT__copper_pos[16U];
    vlSelfRef.test_copper_lists__DOT__dut__DOT__copper_pos_flat[8U] 
        = ((0x0000ffffU & vlSelfRef.test_copper_lists__DOT__dut__DOT__copper_pos_flat[8U]) 
           | (test_copper_lists__DOT__dut__DOT____Vlvbound_h3e9a9ec3__0 
              << 0x00000010U));
    vlSelfRef.test_copper_lists__DOT__dut__DOT__copper_pos_flat[9U] 
        = ((0xfffffffeU & vlSelfRef.test_copper_lists__DOT__dut__DOT__copper_pos_flat[9U]) 
           | (test_copper_lists__DOT__dut__DOT____Vlvbound_h3e9a9ec3__0 
              >> 0x00000010U));
    test_copper_lists__DOT__dut__DOT____Vlvbound_h3e9a9ec3__0 
        = vlSelfRef.test_copper_lists__DOT__dut__DOT__copper_pos[17U];
    vlSelfRef.test_copper_lists__DOT__dut__DOT__copper_pos_flat[9U] 
        = ((0xfffc0001U & vlSelfRef.test_copper_lists__DOT__dut__DOT__copper_pos_flat[9U]) 
           | (test_copper_lists__DOT__dut__DOT____Vlvbound_h3e9a9ec3__0 
              << 1U));
    test_copper_lists__DOT__dut__DOT____Vlvbound_h3e9a9ec3__0 
        = vlSelfRef.test_copper_lists__DOT__dut__DOT__copper_pos[18U];
    vlSelfRef.test_copper_lists__DOT__dut__DOT__copper_pos_flat[9U] 
        = ((0x0003ffffU & vlSelfRef.test_copper_lists__DOT__dut__DOT__copper_pos_flat[9U]) 
           | (test_copper_lists__DOT__dut__DOT____Vlvbound_h3e9a9ec3__0 
              << 0x00000012U));
    vlSelfRef.test_copper_lists__DOT__dut__DOT__copper_pos_flat[10U] 
        = ((0xfffffff8U & vlSelfRef.test_copper_lists__DOT__dut__DOT__copper_pos_flat[10U]) 
           | (test_copper_lists__DOT__dut__DOT____Vlvbound_h3e9a9ec3__0 
              >> 0x0000000eU));
    test_copper_lists__DOT__dut__DOT____Vlvbound_h3e9a9ec3__0 
        = vlSelfRef.test_copper_lists__DOT__dut__DOT__copper_pos[19U];
    vlSelfRef.test_copper_lists__DOT__dut__DOT__copper_pos_flat[10U] 
        = ((0xfff00007U & vlSelfRef.test_copper_lists__DOT__dut__DOT__copper_pos_flat[10U]) 
           | (test_copper_lists__DOT__dut__DOT____Vlvbound_h3e9a9ec3__0 
              << 3U));
    test_copper_lists__DOT__dut__DOT____Vlvbound_h3e9a9ec3__0 
        = vlSelfRef.test_copper_lists__DOT__dut__DOT__copper_pos[20U];
    vlSelfRef.test_copper_lists__DOT__dut__DOT__copper_pos_flat[10U] 
        = ((0x000fffffU & vlSelfRef.test_copper_lists__DOT__dut__DOT__copper_pos_flat[10U]) 
           | (test_copper_lists__DOT__dut__DOT____Vlvbound_h3e9a9ec3__0 
              << 0x00000014U));
    vlSelfRef.test_copper_lists__DOT__dut__DOT__copper_pos_flat[11U] 
        = ((0xffffffe0U & vlSelfRef.test_copper_lists__DOT__dut__DOT__copper_pos_flat[11U]) 
           | (test_copper_lists__DOT__dut__DOT____Vlvbound_h3e9a9ec3__0 
              >> 0x0000000cU));
    test_copper_lists__DOT__dut__DOT____Vlvbound_h3e9a9ec3__0 
        = vlSelfRef.test_copper_lists__DOT__dut__DOT__copper_pos[21U];
    vlSelfRef.test_copper_lists__DOT__dut__DOT__copper_pos_flat[11U] 
        = ((0xffc0001fU & vlSelfRef.test_copper_lists__DOT__dut__DOT__copper_pos_flat[11U]) 
           | (test_copper_lists__DOT__dut__DOT____Vlvbound_h3e9a9ec3__0 
              << 5U));
    test_copper_lists__DOT__dut__DOT____Vlvbound_h3e9a9ec3__0 
        = vlSelfRef.test_copper_lists__DOT__dut__DOT__copper_pos[22U];
    vlSelfRef.test_copper_lists__DOT__dut__DOT__copper_pos_flat[11U] 
        = ((0x003fffffU & vlSelfRef.test_copper_lists__DOT__dut__DOT__copper_pos_flat[11U]) 
           | (test_copper_lists__DOT__dut__DOT____Vlvbound_h3e9a9ec3__0 
              << 0x00000016U));
    vlSelfRef.test_copper_lists__DOT__dut__DOT__copper_pos_flat[12U] 
        = ((0xffffff80U & vlSelfRef.test_copper_lists__DOT__dut__DOT__copper_pos_flat[12U]) 
           | (test_copper_lists__DOT__dut__DOT____Vlvbound_h3e9a9ec3__0 
              >> 0x0000000aU));
    test_copper_lists__DOT__dut__DOT____Vlvbound_h3e9a9ec3__0 
        = vlSelfRef.test_copper_lists__DOT__dut__DOT__copper_pos[23U];
    vlSelfRef.test_copper_lists__DOT__dut__DOT__copper_pos_flat[12U] 
        = ((0xff00007fU & vlSelfRef.test_copper_lists__DOT__dut__DOT__copper_pos_flat[12U]) 
           | (test_copper_lists__DOT__dut__DOT____Vlvbound_h3e9a9ec3__0 
              << 7U));
    test_copper_lists__DOT__dut__DOT____Vlvbound_h3e9a9ec3__0 
        = vlSelfRef.test_copper_lists__DOT__dut__DOT__copper_pos[24U];
    vlSelfRef.test_copper_lists__DOT__dut__DOT__copper_pos_flat[12U] 
        = ((0x00ffffffU & vlSelfRef.test_copper_lists__DOT__dut__DOT__copper_pos_flat[12U]) 
           | (test_copper_lists__DOT__dut__DOT____Vlvbound_h3e9a9ec3__0 
              << 0x00000018U));
    vlSelfRef.test_copper_lists__DOT__dut__DOT__copper_pos_flat[13U] 
        = ((0xfffffe00U & vlSelfRef.test_copper_lists__DOT__dut__DOT__copper_pos_flat[13U]) 
           | (test_copper_lists__DOT__dut__DOT____Vlvbound_h3e9a9ec3__0 
              >> 8U));
    test_copper_lists__DOT__dut__DOT____Vlvbound_h3e9a9ec3__0 
        = vlSelfRef.test_copper_lists__DOT__dut__DOT__copper_pos[25U];
    vlSelfRef.test_copper_lists__DOT__dut__DOT__copper_pos_flat[13U] 
        = ((0xfc0001ffU & vlSelfRef.test_copper_lists__DOT__dut__DOT__copper_pos_flat[13U]) 
           | (test_copper_lists__DOT__dut__DOT____Vlvbound_h3e9a9ec3__0 
              << 9U));
    test_copper_lists__DOT__dut__DOT____Vlvbound_h3e9a9ec3__0 
        = vlSelfRef.test_copper_lists__DOT__dut__DOT__copper_pos[26U];
    vlSelfRef.test_copper_lists__DOT__dut__DOT__copper_pos_flat[13U] 
        = ((0x03ffffffU & vlSelfRef.test_copper_lists__DOT__dut__DOT__copper_pos_flat[13U]) 
           | (test_copper_lists__DOT__dut__DOT____Vlvbound_h3e9a9ec3__0 
              << 0x0000001aU));
    vlSelfRef.test_copper_lists__DOT__dut__DOT__copper_pos_flat[14U] 
        = ((0xfffff800U & vlSelfRef.test_copper_lists__DOT__dut__DOT__copper_pos_flat[14U]) 
           | (test_copper_lists__DOT__dut__DOT____Vlvbound_h3e9a9ec3__0 
              >> 6U));
    test_copper_lists__DOT__dut__DOT____Vlvbound_h3e9a9ec3__0 
        = vlSelfRef.test_copper_lists__DOT__dut__DOT__copper_pos[27U];
    vlSelfRef.test_copper_lists__DOT__dut__DOT__copper_pos_flat[14U] 
        = ((0xf00007ffU & vlSelfRef.test_copper_lists__DOT__dut__DOT__copper_pos_flat[14U]) 
           | (test_copper_lists__DOT__dut__DOT____Vlvbound_h3e9a9ec3__0 
              << 0x0000000bU));
    test_copper_lists__DOT__dut__DOT____Vlvbound_h3e9a9ec3__0 
        = vlSelfRef.test_copper_lists__DOT__dut__DOT__copper_pos[28U];
    vlSelfRef.test_copper_lists__DOT__dut__DOT__copper_pos_flat[14U] 
        = ((0x0fffffffU & vlSelfRef.test_copper_lists__DOT__dut__DOT__copper_pos_flat[14U]) 
           | (test_copper_lists__DOT__dut__DOT____Vlvbound_h3e9a9ec3__0 
              << 0x0000001cU));
    vlSelfRef.test_copper_lists__DOT__dut__DOT__copper_pos_flat[15U] 
        = ((0xffffe000U & vlSelfRef.test_copper_lists__DOT__dut__DOT__copper_pos_flat[15U]) 
           | (test_copper_lists__DOT__dut__DOT____Vlvbound_h3e9a9ec3__0 
              >> 4U));
    test_copper_lists__DOT__dut__DOT____Vlvbound_h3e9a9ec3__0 
        = vlSelfRef.test_copper_lists__DOT__dut__DOT__copper_pos[29U];
    vlSelfRef.test_copper_lists__DOT__dut__DOT__copper_pos_flat[15U] 
        = ((0xc0001fffU & vlSelfRef.test_copper_lists__DOT__dut__DOT__copper_pos_flat[15U]) 
           | (test_copper_lists__DOT__dut__DOT____Vlvbound_h3e9a9ec3__0 
              << 0x0000000dU));
    test_copper_lists__DOT__dut__DOT____Vlvbound_h3e9a9ec3__0 
        = vlSelfRef.test_copper_lists__DOT__dut__DOT__copper_pos[30U];
    vlSelfRef.test_copper_lists__DOT__dut__DOT__copper_pos_flat[15U] 
        = ((0x3fffffffU & vlSelfRef.test_copper_lists__DOT__dut__DOT__copper_pos_flat[15U]) 
           | (test_copper_lists__DOT__dut__DOT____Vlvbound_h3e9a9ec3__0 
              << 0x0000001eU));
    vlSelfRef.test_copper_lists__DOT__dut__DOT__copper_pos_flat[16U] 
        = ((0xffff8000U & vlSelfRef.test_copper_lists__DOT__dut__DOT__copper_pos_flat[16U]) 
           | (test_copper_lists__DOT__dut__DOT____Vlvbound_h3e9a9ec3__0 
              >> 2U));
    test_copper_lists__DOT__dut__DOT____Vlvbound_h3e9a9ec3__0 
        = vlSelfRef.test_copper_lists__DOT__dut__DOT__copper_pos[31U];
    vlSelfRef.test_copper_lists__DOT__dut__DOT__copper_pos_flat[16U] 
        = ((0x00007fffU & vlSelfRef.test_copper_lists__DOT__dut__DOT__copper_pos_flat[16U]) 
           | (test_copper_lists__DOT__dut__DOT____Vlvbound_h3e9a9ec3__0 
              << 0x0000000fU));
    vlSelfRef.test_copper_lists__DOT__dut__DOT__tile_inst__DOT__td_we_a = 0U;
    test_copper_lists__DOT__dut__DOT__tile_blt_wdata 
        = ((IData)(vlSelfRef.test_copper_lists__DOT__dut__DOT__vgc_tile_we)
            ? (IData)(vlSelfRef.test_copper_lists__DOT__dut__DOT__vgc_tile_wdata)
            : 0U);
    test_copper_lists__DOT__dut__DOT__tile_blt_addr 
        = (((IData)(vlSelfRef.test_copper_lists__DOT__dut__DOT__vgc_tile_re) 
            | (IData)(vlSelfRef.test_copper_lists__DOT__dut__DOT__vgc_tile_we))
            ? (IData)(vlSelfRef.test_copper_lists__DOT__dut__DOT__vgc_tile_addr)
            : 0U);
    vlSelfRef.test_copper_lists__DOT__dut__DOT____Vcellinp__artist_inst__cmd_x0 
        = ((0x00000100U & (vlSelfRef.test_copper_lists__DOT__dut__DOT__regs[18U] 
                           << 8U)) | vlSelfRef.test_copper_lists__DOT__dut__DOT__regs[17U]);
    vlSelfRef.test_copper_lists__DOT__dut__DOT____Vcellinp__artist_inst__cmd_x1 
        = ((0x00000100U & (vlSelfRef.test_copper_lists__DOT__dut__DOT__regs[22U] 
                           << 8U)) | vlSelfRef.test_copper_lists__DOT__dut__DOT__regs[21U]);
    if (__VdlySet__test_copper_lists__DOT__dut__DOT__sprite_inst__DOT__spr_mem__DOT__mem__v0) {
        vlSelfRef.test_copper_lists__DOT__dut__DOT__sprite_inst__DOT__spr_mem__DOT__mem[__VdlyDim0__test_copper_lists__DOT__dut__DOT__sprite_inst__DOT__spr_mem__DOT__mem__v0] 
            = __VdlyVal__test_copper_lists__DOT__dut__DOT__sprite_inst__DOT__spr_mem__DOT__mem__v0;
    }
    vlSelfRef.test_copper_lists__DOT__dut__DOT__spr_a_addr = 0U;
    if (vlSelfRef.test_copper_lists__DOT__dut__DOT__cmd_spr_we) {
        vlSelfRef.test_copper_lists__DOT__dut__DOT__spr_a_addr 
            = vlSelfRef.test_copper_lists__DOT__dut__DOT__cmd_spr_addr;
    } else if (vlSelfRef.test_copper_lists__DOT__dut__DOT__cmd_spr_re) {
        vlSelfRef.test_copper_lists__DOT__dut__DOT__spr_a_addr 
            = vlSelfRef.test_copper_lists__DOT__dut__DOT__cmd_spr_addr;
    }
    if (__VdlySet__test_copper_lists__DOT__dut__DOT__text_inst__DOT__color_mem__DOT__mem__v0) {
        vlSelfRef.test_copper_lists__DOT__dut__DOT__text_inst__DOT__color_mem__DOT__mem[__VdlyDim0__test_copper_lists__DOT__dut__DOT__text_inst__DOT__color_mem__DOT__mem__v0] 
            = __VdlyVal__test_copper_lists__DOT__dut__DOT__text_inst__DOT__color_mem__DOT__mem__v0;
    }
    if (__VdlySet__test_copper_lists__DOT__dut__DOT__text_inst__DOT__char_mem__DOT__mem__v0) {
        vlSelfRef.test_copper_lists__DOT__dut__DOT__text_inst__DOT__char_mem__DOT__mem[__VdlyDim0__test_copper_lists__DOT__dut__DOT__text_inst__DOT__char_mem__DOT__mem__v0] 
            = __VdlyVal__test_copper_lists__DOT__dut__DOT__text_inst__DOT__char_mem__DOT__mem__v0;
    }
    if (__VdlySet__test_copper_lists__DOT__dut__DOT__gfx_inst__DOT__gfx_mem__DOT__mem__v0) {
        vlSelfRef.test_copper_lists__DOT__dut__DOT__gfx_inst__DOT__gfx_mem__DOT__mem[__VdlyDim0__test_copper_lists__DOT__dut__DOT__gfx_inst__DOT__gfx_mem__DOT__mem__v0] 
            = __VdlyVal__test_copper_lists__DOT__dut__DOT__gfx_inst__DOT__gfx_mem__DOT__mem__v0;
    }
    vlSelfRef.test_copper_lists__DOT__dut__DOT__gfx_a_addr = 0U;
    if (vlSelfRef.test_copper_lists__DOT__dut__DOT__artist_gfx_we) {
        vlSelfRef.test_copper_lists__DOT__dut__DOT__gfx_a_we = 1U;
        vlSelfRef.test_copper_lists__DOT__dut__DOT__gfx_a_din 
            = vlSelfRef.test_copper_lists__DOT__dut__DOT__artist_gfx_wdata;
        vlSelfRef.test_copper_lists__DOT__dut__DOT__gfx_a_addr 
            = vlSelfRef.test_copper_lists__DOT__dut__DOT__artist_gfx_addr;
    } else if (vlSelfRef.test_copper_lists__DOT__dut__DOT__cmd_gfx_we) {
        vlSelfRef.test_copper_lists__DOT__dut__DOT__gfx_a_we = 1U;
        vlSelfRef.test_copper_lists__DOT__dut__DOT__gfx_a_din 
            = vlSelfRef.test_copper_lists__DOT__dut__DOT__cmd_gfx_din;
        vlSelfRef.test_copper_lists__DOT__dut__DOT__gfx_a_addr 
            = vlSelfRef.test_copper_lists__DOT__dut__DOT__cmd_gfx_addr;
    } else if (vlSelfRef.test_copper_lists__DOT__dut__DOT__artist_gfx_re) {
        vlSelfRef.test_copper_lists__DOT__dut__DOT__gfx_a_addr 
            = vlSelfRef.test_copper_lists__DOT__dut__DOT__artist_gfx_raddr;
    } else if (vlSelfRef.test_copper_lists__DOT__dut__DOT__cmd_gfx_re) {
        vlSelfRef.test_copper_lists__DOT__dut__DOT__gfx_a_addr 
            = vlSelfRef.test_copper_lists__DOT__dut__DOT__cmd_gfx_addr;
    }
    vlSelfRef.test_copper_lists__DOT__dut__DOT__tile_inst__DOT__td_din_a = 0U;
    vlSelfRef.test_copper_lists__DOT__dut__DOT__tile_inst__DOT__td_addr_a = 0U;
    if ((0x00000010U & (IData)(vlSelfRef.test_copper_lists__DOT__dut__DOT__tile_inst__DOT__dma_state))) {
        if (vlSelfRef.test_copper_lists__DOT__dut__DOT__vgc_tile_we) {
            vlSelfRef.test_copper_lists__DOT__dut__DOT__tile_inst__DOT__td_we_a = 1U;
            vlSelfRef.test_copper_lists__DOT__dut__DOT__tile_inst__DOT__td_din_a 
                = test_copper_lists__DOT__dut__DOT__tile_blt_wdata;
            vlSelfRef.test_copper_lists__DOT__dut__DOT__tile_inst__DOT__td_addr_a 
                = test_copper_lists__DOT__dut__DOT__tile_blt_addr;
        } else if (vlSelfRef.test_copper_lists__DOT__dut__DOT__vgc_tile_re) {
            vlSelfRef.test_copper_lists__DOT__dut__DOT__tile_inst__DOT__td_addr_a 
                = test_copper_lists__DOT__dut__DOT__tile_blt_addr;
        }
    } else if ((8U & (IData)(vlSelfRef.test_copper_lists__DOT__dut__DOT__tile_inst__DOT__dma_state))) {
        if ((4U & (IData)(vlSelfRef.test_copper_lists__DOT__dut__DOT__tile_inst__DOT__dma_state))) {
            if ((2U & (IData)(vlSelfRef.test_copper_lists__DOT__dut__DOT__tile_inst__DOT__dma_state))) {
                if ((1U & (IData)(vlSelfRef.test_copper_lists__DOT__dut__DOT__tile_inst__DOT__dma_state))) {
                    if (vlSelfRef.test_copper_lists__DOT__dut__DOT__vgc_tile_we) {
                        vlSelfRef.test_copper_lists__DOT__dut__DOT__tile_inst__DOT__td_we_a = 1U;
                        vlSelfRef.test_copper_lists__DOT__dut__DOT__tile_inst__DOT__td_din_a 
                            = test_copper_lists__DOT__dut__DOT__tile_blt_wdata;
                        vlSelfRef.test_copper_lists__DOT__dut__DOT__tile_inst__DOT__td_addr_a 
                            = test_copper_lists__DOT__dut__DOT__tile_blt_addr;
                    } else if (vlSelfRef.test_copper_lists__DOT__dut__DOT__vgc_tile_re) {
                        vlSelfRef.test_copper_lists__DOT__dut__DOT__tile_inst__DOT__td_addr_a 
                            = test_copper_lists__DOT__dut__DOT__tile_blt_addr;
                    }
                }
            }
        }
    } else if ((1U & (~ ((IData)(vlSelfRef.test_copper_lists__DOT__dut__DOT__tile_inst__DOT__dma_state) 
                         >> 2U)))) {
        if ((1U & (~ ((IData)(vlSelfRef.test_copper_lists__DOT__dut__DOT__tile_inst__DOT__dma_state) 
                      >> 1U)))) {
            if ((1U & (IData)(vlSelfRef.test_copper_lists__DOT__dut__DOT__tile_inst__DOT__dma_state))) {
                if (vlSelfRef.test_copper_lists__DOT__dut__DOT__tile_inst__DOT__dma_data_valid) {
                    vlSelfRef.test_copper_lists__DOT__dut__DOT__tile_inst__DOT__td_we_a = 1U;
                    vlSelfRef.test_copper_lists__DOT__dut__DOT__tile_inst__DOT__td_din_a = 0U;
                    vlSelfRef.test_copper_lists__DOT__dut__DOT__tile_inst__DOT__td_addr_a 
                        = (0x00007fffU & (IData)(vlSelfRef.test_copper_lists__DOT__dut__DOT__tile_inst__DOT__dma_dst));
                }
            } else if (vlSelfRef.test_copper_lists__DOT__dut__DOT__vgc_tile_we) {
                vlSelfRef.test_copper_lists__DOT__dut__DOT__tile_inst__DOT__td_we_a = 1U;
                vlSelfRef.test_copper_lists__DOT__dut__DOT__tile_inst__DOT__td_din_a 
                    = test_copper_lists__DOT__dut__DOT__tile_blt_wdata;
                vlSelfRef.test_copper_lists__DOT__dut__DOT__tile_inst__DOT__td_addr_a 
                    = test_copper_lists__DOT__dut__DOT__tile_blt_addr;
            } else if (vlSelfRef.test_copper_lists__DOT__dut__DOT__vgc_tile_re) {
                vlSelfRef.test_copper_lists__DOT__dut__DOT__tile_inst__DOT__td_addr_a 
                    = test_copper_lists__DOT__dut__DOT__tile_blt_addr;
            }
        }
    }
    vlSelfRef.test_copper_lists__DOT__dut__DOT__copper_enabled 
        = __Vdly__test_copper_lists__DOT__dut__DOT__copper_enabled;
    vlSelfRef.test_copper_lists__DOT__dut__DOT__copper_inst__DOT__copper_pos[0U] 
        = (0x0001ffffU & vlSelfRef.test_copper_lists__DOT__dut__DOT__copper_pos_flat[0U]);
    vlSelfRef.test_copper_lists__DOT__dut__DOT__copper_inst__DOT__copper_pos[1U] 
        = (0x0001ffffU & ((vlSelfRef.test_copper_lists__DOT__dut__DOT__copper_pos_flat[1U] 
                           << 0x0000000fU) | (vlSelfRef.test_copper_lists__DOT__dut__DOT__copper_pos_flat[0U] 
                                              >> 0x00000011U)));
    vlSelfRef.test_copper_lists__DOT__dut__DOT__copper_inst__DOT__copper_pos[2U] 
        = (0x0001ffffU & (vlSelfRef.test_copper_lists__DOT__dut__DOT__copper_pos_flat[1U] 
                          >> 2U));
    vlSelfRef.test_copper_lists__DOT__dut__DOT__copper_inst__DOT__copper_pos[3U] 
        = (0x0001ffffU & ((vlSelfRef.test_copper_lists__DOT__dut__DOT__copper_pos_flat[2U] 
                           << 0x0000000dU) | (vlSelfRef.test_copper_lists__DOT__dut__DOT__copper_pos_flat[1U] 
                                              >> 0x00000013U)));
    vlSelfRef.test_copper_lists__DOT__dut__DOT__copper_inst__DOT__copper_pos[4U] 
        = (0x0001ffffU & (vlSelfRef.test_copper_lists__DOT__dut__DOT__copper_pos_flat[2U] 
                          >> 4U));
    vlSelfRef.test_copper_lists__DOT__dut__DOT__copper_inst__DOT__copper_pos[5U] 
        = (0x0001ffffU & ((vlSelfRef.test_copper_lists__DOT__dut__DOT__copper_pos_flat[3U] 
                           << 0x0000000bU) | (vlSelfRef.test_copper_lists__DOT__dut__DOT__copper_pos_flat[2U] 
                                              >> 0x00000015U)));
    vlSelfRef.test_copper_lists__DOT__dut__DOT__copper_inst__DOT__copper_pos[6U] 
        = (0x0001ffffU & (vlSelfRef.test_copper_lists__DOT__dut__DOT__copper_pos_flat[3U] 
                          >> 6U));
    vlSelfRef.test_copper_lists__DOT__dut__DOT__copper_inst__DOT__copper_pos[7U] 
        = (0x0001ffffU & ((vlSelfRef.test_copper_lists__DOT__dut__DOT__copper_pos_flat[4U] 
                           << 9U) | (vlSelfRef.test_copper_lists__DOT__dut__DOT__copper_pos_flat[3U] 
                                     >> 0x00000017U)));
    vlSelfRef.test_copper_lists__DOT__dut__DOT__copper_inst__DOT__copper_pos[8U] 
        = (0x0001ffffU & (vlSelfRef.test_copper_lists__DOT__dut__DOT__copper_pos_flat[4U] 
                          >> 8U));
    vlSelfRef.test_copper_lists__DOT__dut__DOT__copper_inst__DOT__copper_pos[9U] 
        = (0x0001ffffU & ((vlSelfRef.test_copper_lists__DOT__dut__DOT__copper_pos_flat[5U] 
                           << 7U) | (vlSelfRef.test_copper_lists__DOT__dut__DOT__copper_pos_flat[4U] 
                                     >> 0x00000019U)));
    vlSelfRef.test_copper_lists__DOT__dut__DOT__copper_inst__DOT__copper_pos[10U] 
        = (0x0001ffffU & (vlSelfRef.test_copper_lists__DOT__dut__DOT__copper_pos_flat[5U] 
                          >> 0x0000000aU));
    vlSelfRef.test_copper_lists__DOT__dut__DOT__copper_inst__DOT__copper_pos[11U] 
        = (0x0001ffffU & ((vlSelfRef.test_copper_lists__DOT__dut__DOT__copper_pos_flat[6U] 
                           << 5U) | (vlSelfRef.test_copper_lists__DOT__dut__DOT__copper_pos_flat[5U] 
                                     >> 0x0000001bU)));
    vlSelfRef.test_copper_lists__DOT__dut__DOT__copper_inst__DOT__copper_pos[12U] 
        = (0x0001ffffU & (vlSelfRef.test_copper_lists__DOT__dut__DOT__copper_pos_flat[6U] 
                          >> 0x0000000cU));
    vlSelfRef.test_copper_lists__DOT__dut__DOT__copper_inst__DOT__copper_pos[13U] 
        = (0x0001ffffU & ((vlSelfRef.test_copper_lists__DOT__dut__DOT__copper_pos_flat[7U] 
                           << 3U) | (vlSelfRef.test_copper_lists__DOT__dut__DOT__copper_pos_flat[6U] 
                                     >> 0x0000001dU)));
    vlSelfRef.test_copper_lists__DOT__dut__DOT__copper_inst__DOT__copper_pos[14U] 
        = (0x0001ffffU & (vlSelfRef.test_copper_lists__DOT__dut__DOT__copper_pos_flat[7U] 
                          >> 0x0000000eU));
    vlSelfRef.test_copper_lists__DOT__dut__DOT__copper_inst__DOT__copper_pos[15U] 
        = (0x0001ffffU & ((vlSelfRef.test_copper_lists__DOT__dut__DOT__copper_pos_flat[8U] 
                           << 1U) | (vlSelfRef.test_copper_lists__DOT__dut__DOT__copper_pos_flat[7U] 
                                     >> 0x0000001fU)));
    vlSelfRef.test_copper_lists__DOT__dut__DOT__copper_inst__DOT__copper_pos[16U] 
        = (0x0001ffffU & ((vlSelfRef.test_copper_lists__DOT__dut__DOT__copper_pos_flat[9U] 
                           << 0x00000010U) | (vlSelfRef.test_copper_lists__DOT__dut__DOT__copper_pos_flat[8U] 
                                              >> 0x00000010U)));
    vlSelfRef.test_copper_lists__DOT__dut__DOT__copper_inst__DOT__copper_pos[17U] 
        = (0x0001ffffU & (vlSelfRef.test_copper_lists__DOT__dut__DOT__copper_pos_flat[9U] 
                          >> 1U));
    vlSelfRef.test_copper_lists__DOT__dut__DOT__copper_inst__DOT__copper_pos[18U] 
        = (0x0001ffffU & ((vlSelfRef.test_copper_lists__DOT__dut__DOT__copper_pos_flat[10U] 
                           << 0x0000000eU) | (vlSelfRef.test_copper_lists__DOT__dut__DOT__copper_pos_flat[9U] 
                                              >> 0x00000012U)));
    vlSelfRef.test_copper_lists__DOT__dut__DOT__copper_inst__DOT__copper_pos[19U] 
        = (0x0001ffffU & (vlSelfRef.test_copper_lists__DOT__dut__DOT__copper_pos_flat[10U] 
                          >> 3U));
    vlSelfRef.test_copper_lists__DOT__dut__DOT__copper_inst__DOT__copper_pos[20U] 
        = (0x0001ffffU & ((vlSelfRef.test_copper_lists__DOT__dut__DOT__copper_pos_flat[11U] 
                           << 0x0000000cU) | (vlSelfRef.test_copper_lists__DOT__dut__DOT__copper_pos_flat[10U] 
                                              >> 0x00000014U)));
    vlSelfRef.test_copper_lists__DOT__dut__DOT__copper_inst__DOT__copper_pos[21U] 
        = (0x0001ffffU & (vlSelfRef.test_copper_lists__DOT__dut__DOT__copper_pos_flat[11U] 
                          >> 5U));
    vlSelfRef.test_copper_lists__DOT__dut__DOT__copper_inst__DOT__copper_pos[22U] 
        = (0x0001ffffU & ((vlSelfRef.test_copper_lists__DOT__dut__DOT__copper_pos_flat[12U] 
                           << 0x0000000aU) | (vlSelfRef.test_copper_lists__DOT__dut__DOT__copper_pos_flat[11U] 
                                              >> 0x00000016U)));
    vlSelfRef.test_copper_lists__DOT__dut__DOT__copper_inst__DOT__copper_pos[23U] 
        = (0x0001ffffU & (vlSelfRef.test_copper_lists__DOT__dut__DOT__copper_pos_flat[12U] 
                          >> 7U));
    vlSelfRef.test_copper_lists__DOT__dut__DOT__copper_inst__DOT__copper_pos[24U] 
        = (0x0001ffffU & ((vlSelfRef.test_copper_lists__DOT__dut__DOT__copper_pos_flat[13U] 
                           << 8U) | (vlSelfRef.test_copper_lists__DOT__dut__DOT__copper_pos_flat[12U] 
                                     >> 0x00000018U)));
    vlSelfRef.test_copper_lists__DOT__dut__DOT__copper_inst__DOT__copper_pos[25U] 
        = (0x0001ffffU & (vlSelfRef.test_copper_lists__DOT__dut__DOT__copper_pos_flat[13U] 
                          >> 9U));
    vlSelfRef.test_copper_lists__DOT__dut__DOT__copper_inst__DOT__copper_pos[26U] 
        = (0x0001ffffU & ((vlSelfRef.test_copper_lists__DOT__dut__DOT__copper_pos_flat[14U] 
                           << 6U) | (vlSelfRef.test_copper_lists__DOT__dut__DOT__copper_pos_flat[13U] 
                                     >> 0x0000001aU)));
    vlSelfRef.test_copper_lists__DOT__dut__DOT__copper_inst__DOT__copper_pos[27U] 
        = (0x0001ffffU & (vlSelfRef.test_copper_lists__DOT__dut__DOT__copper_pos_flat[14U] 
                          >> 0x0000000bU));
    vlSelfRef.test_copper_lists__DOT__dut__DOT__copper_inst__DOT__copper_pos[28U] 
        = (0x0001ffffU & ((vlSelfRef.test_copper_lists__DOT__dut__DOT__copper_pos_flat[15U] 
                           << 4U) | (vlSelfRef.test_copper_lists__DOT__dut__DOT__copper_pos_flat[14U] 
                                     >> 0x0000001cU)));
    vlSelfRef.test_copper_lists__DOT__dut__DOT__copper_inst__DOT__copper_pos[29U] 
        = (0x0001ffffU & (vlSelfRef.test_copper_lists__DOT__dut__DOT__copper_pos_flat[15U] 
                          >> 0x0000000dU));
    vlSelfRef.test_copper_lists__DOT__dut__DOT__copper_inst__DOT__copper_pos[30U] 
        = (0x0001ffffU & ((vlSelfRef.test_copper_lists__DOT__dut__DOT__copper_pos_flat[16U] 
                           << 2U) | (vlSelfRef.test_copper_lists__DOT__dut__DOT__copper_pos_flat[15U] 
                                     >> 0x0000001eU)));
    vlSelfRef.test_copper_lists__DOT__dut__DOT__copper_inst__DOT__copper_pos[31U] 
        = (vlSelfRef.test_copper_lists__DOT__dut__DOT__copper_pos_flat[16U] 
           >> 0x0000000fU);
    vlSelfRef.test_copper_lists__DOT__dut__DOT__copper_inst__DOT__copper_reg[0U] 
        = (0x000000ffU & vlSelfRef.test_copper_lists__DOT__dut__DOT__copper_reg_flat[0U]);
    vlSelfRef.test_copper_lists__DOT__dut__DOT__copper_inst__DOT__copper_reg[1U] 
        = (0x000000ffU & (vlSelfRef.test_copper_lists__DOT__dut__DOT__copper_reg_flat[0U] 
                          >> 8U));
    vlSelfRef.test_copper_lists__DOT__dut__DOT__copper_inst__DOT__copper_reg[2U] 
        = (0x000000ffU & (vlSelfRef.test_copper_lists__DOT__dut__DOT__copper_reg_flat[0U] 
                          >> 0x00000010U));
    vlSelfRef.test_copper_lists__DOT__dut__DOT__copper_inst__DOT__copper_reg[3U] 
        = (vlSelfRef.test_copper_lists__DOT__dut__DOT__copper_reg_flat[0U] 
           >> 0x00000018U);
    vlSelfRef.test_copper_lists__DOT__dut__DOT__copper_inst__DOT__copper_reg[4U] 
        = (0x000000ffU & vlSelfRef.test_copper_lists__DOT__dut__DOT__copper_reg_flat[1U]);
    vlSelfRef.test_copper_lists__DOT__dut__DOT__copper_inst__DOT__copper_reg[5U] 
        = (0x000000ffU & (vlSelfRef.test_copper_lists__DOT__dut__DOT__copper_reg_flat[1U] 
                          >> 8U));
    vlSelfRef.test_copper_lists__DOT__dut__DOT__copper_inst__DOT__copper_reg[6U] 
        = (0x000000ffU & (vlSelfRef.test_copper_lists__DOT__dut__DOT__copper_reg_flat[1U] 
                          >> 0x00000010U));
    vlSelfRef.test_copper_lists__DOT__dut__DOT__copper_inst__DOT__copper_reg[7U] 
        = (vlSelfRef.test_copper_lists__DOT__dut__DOT__copper_reg_flat[1U] 
           >> 0x00000018U);
    vlSelfRef.test_copper_lists__DOT__dut__DOT__copper_inst__DOT__copper_reg[8U] 
        = (0x000000ffU & vlSelfRef.test_copper_lists__DOT__dut__DOT__copper_reg_flat[2U]);
    vlSelfRef.test_copper_lists__DOT__dut__DOT__copper_inst__DOT__copper_reg[9U] 
        = (0x000000ffU & (vlSelfRef.test_copper_lists__DOT__dut__DOT__copper_reg_flat[2U] 
                          >> 8U));
    vlSelfRef.test_copper_lists__DOT__dut__DOT__copper_inst__DOT__copper_reg[10U] 
        = (0x000000ffU & (vlSelfRef.test_copper_lists__DOT__dut__DOT__copper_reg_flat[2U] 
                          >> 0x00000010U));
    vlSelfRef.test_copper_lists__DOT__dut__DOT__copper_inst__DOT__copper_reg[11U] 
        = (vlSelfRef.test_copper_lists__DOT__dut__DOT__copper_reg_flat[2U] 
           >> 0x00000018U);
    vlSelfRef.test_copper_lists__DOT__dut__DOT__copper_inst__DOT__copper_reg[12U] 
        = (0x000000ffU & vlSelfRef.test_copper_lists__DOT__dut__DOT__copper_reg_flat[3U]);
    vlSelfRef.test_copper_lists__DOT__dut__DOT__copper_inst__DOT__copper_reg[13U] 
        = (0x000000ffU & (vlSelfRef.test_copper_lists__DOT__dut__DOT__copper_reg_flat[3U] 
                          >> 8U));
    vlSelfRef.test_copper_lists__DOT__dut__DOT__copper_inst__DOT__copper_reg[14U] 
        = (0x000000ffU & (vlSelfRef.test_copper_lists__DOT__dut__DOT__copper_reg_flat[3U] 
                          >> 0x00000010U));
    vlSelfRef.test_copper_lists__DOT__dut__DOT__copper_inst__DOT__copper_reg[15U] 
        = (vlSelfRef.test_copper_lists__DOT__dut__DOT__copper_reg_flat[3U] 
           >> 0x00000018U);
    vlSelfRef.test_copper_lists__DOT__dut__DOT__copper_inst__DOT__copper_reg[16U] 
        = (0x000000ffU & vlSelfRef.test_copper_lists__DOT__dut__DOT__copper_reg_flat[4U]);
    vlSelfRef.test_copper_lists__DOT__dut__DOT__copper_inst__DOT__copper_reg[17U] 
        = (0x000000ffU & (vlSelfRef.test_copper_lists__DOT__dut__DOT__copper_reg_flat[4U] 
                          >> 8U));
    vlSelfRef.test_copper_lists__DOT__dut__DOT__copper_inst__DOT__copper_reg[18U] 
        = (0x000000ffU & (vlSelfRef.test_copper_lists__DOT__dut__DOT__copper_reg_flat[4U] 
                          >> 0x00000010U));
    vlSelfRef.test_copper_lists__DOT__dut__DOT__copper_inst__DOT__copper_reg[19U] 
        = (vlSelfRef.test_copper_lists__DOT__dut__DOT__copper_reg_flat[4U] 
           >> 0x00000018U);
    vlSelfRef.test_copper_lists__DOT__dut__DOT__copper_inst__DOT__copper_reg[20U] 
        = (0x000000ffU & vlSelfRef.test_copper_lists__DOT__dut__DOT__copper_reg_flat[5U]);
    vlSelfRef.test_copper_lists__DOT__dut__DOT__copper_inst__DOT__copper_reg[21U] 
        = (0x000000ffU & (vlSelfRef.test_copper_lists__DOT__dut__DOT__copper_reg_flat[5U] 
                          >> 8U));
    vlSelfRef.test_copper_lists__DOT__dut__DOT__copper_inst__DOT__copper_reg[22U] 
        = (0x000000ffU & (vlSelfRef.test_copper_lists__DOT__dut__DOT__copper_reg_flat[5U] 
                          >> 0x00000010U));
    vlSelfRef.test_copper_lists__DOT__dut__DOT__copper_inst__DOT__copper_reg[23U] 
        = (vlSelfRef.test_copper_lists__DOT__dut__DOT__copper_reg_flat[5U] 
           >> 0x00000018U);
    vlSelfRef.test_copper_lists__DOT__dut__DOT__copper_inst__DOT__copper_reg[24U] 
        = (0x000000ffU & vlSelfRef.test_copper_lists__DOT__dut__DOT__copper_reg_flat[6U]);
    vlSelfRef.test_copper_lists__DOT__dut__DOT__copper_inst__DOT__copper_reg[25U] 
        = (0x000000ffU & (vlSelfRef.test_copper_lists__DOT__dut__DOT__copper_reg_flat[6U] 
                          >> 8U));
    vlSelfRef.test_copper_lists__DOT__dut__DOT__copper_inst__DOT__copper_reg[26U] 
        = (0x000000ffU & (vlSelfRef.test_copper_lists__DOT__dut__DOT__copper_reg_flat[6U] 
                          >> 0x00000010U));
    vlSelfRef.test_copper_lists__DOT__dut__DOT__copper_inst__DOT__copper_reg[27U] 
        = (vlSelfRef.test_copper_lists__DOT__dut__DOT__copper_reg_flat[6U] 
           >> 0x00000018U);
    vlSelfRef.test_copper_lists__DOT__dut__DOT__copper_inst__DOT__copper_reg[28U] 
        = (0x000000ffU & vlSelfRef.test_copper_lists__DOT__dut__DOT__copper_reg_flat[7U]);
    vlSelfRef.test_copper_lists__DOT__dut__DOT__copper_inst__DOT__copper_reg[29U] 
        = (0x000000ffU & (vlSelfRef.test_copper_lists__DOT__dut__DOT__copper_reg_flat[7U] 
                          >> 8U));
    vlSelfRef.test_copper_lists__DOT__dut__DOT__copper_inst__DOT__copper_reg[30U] 
        = (0x000000ffU & (vlSelfRef.test_copper_lists__DOT__dut__DOT__copper_reg_flat[7U] 
                          >> 0x00000010U));
    vlSelfRef.test_copper_lists__DOT__dut__DOT__copper_inst__DOT__copper_reg[31U] 
        = (vlSelfRef.test_copper_lists__DOT__dut__DOT__copper_reg_flat[7U] 
           >> 0x00000018U);
    vlSelfRef.test_copper_lists__DOT__dut__DOT__copper_inst__DOT__copper_val[0U] 
        = (0x000000ffU & vlSelfRef.test_copper_lists__DOT__dut__DOT__copper_val_flat[0U]);
    vlSelfRef.test_copper_lists__DOT__dut__DOT__copper_inst__DOT__copper_val[1U] 
        = (0x000000ffU & (vlSelfRef.test_copper_lists__DOT__dut__DOT__copper_val_flat[0U] 
                          >> 8U));
    vlSelfRef.test_copper_lists__DOT__dut__DOT__copper_inst__DOT__copper_val[2U] 
        = (0x000000ffU & (vlSelfRef.test_copper_lists__DOT__dut__DOT__copper_val_flat[0U] 
                          >> 0x00000010U));
    vlSelfRef.test_copper_lists__DOT__dut__DOT__copper_inst__DOT__copper_val[3U] 
        = (vlSelfRef.test_copper_lists__DOT__dut__DOT__copper_val_flat[0U] 
           >> 0x00000018U);
    vlSelfRef.test_copper_lists__DOT__dut__DOT__copper_inst__DOT__copper_val[4U] 
        = (0x000000ffU & vlSelfRef.test_copper_lists__DOT__dut__DOT__copper_val_flat[1U]);
    vlSelfRef.test_copper_lists__DOT__dut__DOT__copper_inst__DOT__copper_val[5U] 
        = (0x000000ffU & (vlSelfRef.test_copper_lists__DOT__dut__DOT__copper_val_flat[1U] 
                          >> 8U));
    vlSelfRef.test_copper_lists__DOT__dut__DOT__copper_inst__DOT__copper_val[6U] 
        = (0x000000ffU & (vlSelfRef.test_copper_lists__DOT__dut__DOT__copper_val_flat[1U] 
                          >> 0x00000010U));
    vlSelfRef.test_copper_lists__DOT__dut__DOT__copper_inst__DOT__copper_val[7U] 
        = (vlSelfRef.test_copper_lists__DOT__dut__DOT__copper_val_flat[1U] 
           >> 0x00000018U);
    vlSelfRef.test_copper_lists__DOT__dut__DOT__copper_inst__DOT__copper_val[8U] 
        = (0x000000ffU & vlSelfRef.test_copper_lists__DOT__dut__DOT__copper_val_flat[2U]);
    vlSelfRef.test_copper_lists__DOT__dut__DOT__copper_inst__DOT__copper_val[9U] 
        = (0x000000ffU & (vlSelfRef.test_copper_lists__DOT__dut__DOT__copper_val_flat[2U] 
                          >> 8U));
    vlSelfRef.test_copper_lists__DOT__dut__DOT__copper_inst__DOT__copper_val[10U] 
        = (0x000000ffU & (vlSelfRef.test_copper_lists__DOT__dut__DOT__copper_val_flat[2U] 
                          >> 0x00000010U));
    vlSelfRef.test_copper_lists__DOT__dut__DOT__copper_inst__DOT__copper_val[11U] 
        = (vlSelfRef.test_copper_lists__DOT__dut__DOT__copper_val_flat[2U] 
           >> 0x00000018U);
    vlSelfRef.test_copper_lists__DOT__dut__DOT__copper_inst__DOT__copper_val[12U] 
        = (0x000000ffU & vlSelfRef.test_copper_lists__DOT__dut__DOT__copper_val_flat[3U]);
    vlSelfRef.test_copper_lists__DOT__dut__DOT__copper_inst__DOT__copper_val[13U] 
        = (0x000000ffU & (vlSelfRef.test_copper_lists__DOT__dut__DOT__copper_val_flat[3U] 
                          >> 8U));
    vlSelfRef.test_copper_lists__DOT__dut__DOT__copper_inst__DOT__copper_val[14U] 
        = (0x000000ffU & (vlSelfRef.test_copper_lists__DOT__dut__DOT__copper_val_flat[3U] 
                          >> 0x00000010U));
    vlSelfRef.test_copper_lists__DOT__dut__DOT__copper_inst__DOT__copper_val[15U] 
        = (vlSelfRef.test_copper_lists__DOT__dut__DOT__copper_val_flat[3U] 
           >> 0x00000018U);
    vlSelfRef.test_copper_lists__DOT__dut__DOT__copper_inst__DOT__copper_val[16U] 
        = (0x000000ffU & vlSelfRef.test_copper_lists__DOT__dut__DOT__copper_val_flat[4U]);
    vlSelfRef.test_copper_lists__DOT__dut__DOT__copper_inst__DOT__copper_val[17U] 
        = (0x000000ffU & (vlSelfRef.test_copper_lists__DOT__dut__DOT__copper_val_flat[4U] 
                          >> 8U));
    vlSelfRef.test_copper_lists__DOT__dut__DOT__copper_inst__DOT__copper_val[18U] 
        = (0x000000ffU & (vlSelfRef.test_copper_lists__DOT__dut__DOT__copper_val_flat[4U] 
                          >> 0x00000010U));
    vlSelfRef.test_copper_lists__DOT__dut__DOT__copper_inst__DOT__copper_val[19U] 
        = (vlSelfRef.test_copper_lists__DOT__dut__DOT__copper_val_flat[4U] 
           >> 0x00000018U);
    vlSelfRef.test_copper_lists__DOT__dut__DOT__copper_inst__DOT__copper_val[20U] 
        = (0x000000ffU & vlSelfRef.test_copper_lists__DOT__dut__DOT__copper_val_flat[5U]);
    vlSelfRef.test_copper_lists__DOT__dut__DOT__copper_inst__DOT__copper_val[21U] 
        = (0x000000ffU & (vlSelfRef.test_copper_lists__DOT__dut__DOT__copper_val_flat[5U] 
                          >> 8U));
    vlSelfRef.test_copper_lists__DOT__dut__DOT__copper_inst__DOT__copper_val[22U] 
        = (0x000000ffU & (vlSelfRef.test_copper_lists__DOT__dut__DOT__copper_val_flat[5U] 
                          >> 0x00000010U));
    vlSelfRef.test_copper_lists__DOT__dut__DOT__copper_inst__DOT__copper_val[23U] 
        = (vlSelfRef.test_copper_lists__DOT__dut__DOT__copper_val_flat[5U] 
           >> 0x00000018U);
    vlSelfRef.test_copper_lists__DOT__dut__DOT__copper_inst__DOT__copper_val[24U] 
        = (0x000000ffU & vlSelfRef.test_copper_lists__DOT__dut__DOT__copper_val_flat[6U]);
    vlSelfRef.test_copper_lists__DOT__dut__DOT__copper_inst__DOT__copper_val[25U] 
        = (0x000000ffU & (vlSelfRef.test_copper_lists__DOT__dut__DOT__copper_val_flat[6U] 
                          >> 8U));
    vlSelfRef.test_copper_lists__DOT__dut__DOT__copper_inst__DOT__copper_val[26U] 
        = (0x000000ffU & (vlSelfRef.test_copper_lists__DOT__dut__DOT__copper_val_flat[6U] 
                          >> 0x00000010U));
    vlSelfRef.test_copper_lists__DOT__dut__DOT__copper_inst__DOT__copper_val[27U] 
        = (vlSelfRef.test_copper_lists__DOT__dut__DOT__copper_val_flat[6U] 
           >> 0x00000018U);
    vlSelfRef.test_copper_lists__DOT__dut__DOT__copper_inst__DOT__copper_val[28U] 
        = (0x000000ffU & vlSelfRef.test_copper_lists__DOT__dut__DOT__copper_val_flat[7U]);
    vlSelfRef.test_copper_lists__DOT__dut__DOT__copper_inst__DOT__copper_val[29U] 
        = (0x000000ffU & (vlSelfRef.test_copper_lists__DOT__dut__DOT__copper_val_flat[7U] 
                          >> 8U));
    vlSelfRef.test_copper_lists__DOT__dut__DOT__copper_inst__DOT__copper_val[30U] 
        = (0x000000ffU & (vlSelfRef.test_copper_lists__DOT__dut__DOT__copper_val_flat[7U] 
                          >> 0x00000010U));
    vlSelfRef.test_copper_lists__DOT__dut__DOT__copper_inst__DOT__copper_val[31U] 
        = (vlSelfRef.test_copper_lists__DOT__dut__DOT__copper_val_flat[7U] 
           >> 0x00000018U);
    vlSelfRef.test_copper_lists__DOT__dut__DOT__copper_index 
        = __Vdly__test_copper_lists__DOT__dut__DOT__copper_index;
    vlSelfRef.test_copper_lists__DOT__dut__DOT__copper_count 
        = __Vdly__test_copper_lists__DOT__dut__DOT__copper_count;
    vlSelfRef.test_copper_lists__DOT__dut__DOT__h_count 
        = __Vdly__test_copper_lists__DOT__dut__DOT__h_count;
    vlSelfRef.test_copper_lists__DOT__dut__DOT__v_count 
        = __Vdly__test_copper_lists__DOT__dut__DOT__v_count;
    test_copper_lists__DOT__dut__DOT__visible = ((0x0280U 
                                                  > (IData)(vlSelfRef.test_copper_lists__DOT__dut__DOT__h_count)) 
                                                 & (0x01e0U 
                                                    > (IData)(vlSelfRef.test_copper_lists__DOT__dut__DOT__v_count)));
    test_copper_lists__DOT__dut__DOT__text_line = (0x000003ffU 
                                                   & (((IData)(vlSelfRef.test_copper_lists__DOT__dut__DOT__v_count) 
                                                       - (IData)(0x00000028U)) 
                                                      >> 1U));
    test_copper_lists__DOT__dut__DOT__text_row = (0x0000001fU 
                                                  & ((IData)(test_copper_lists__DOT__dut__DOT__text_line) 
                                                     >> 3U));
    test_copper_lists__DOT__dut__DOT__real_row = (0x0000001fU 
                                                  & ((IData)(test_copper_lists__DOT__dut__DOT__text_row) 
                                                     + (IData)(vlSelfRef.test_copper_lists__DOT__dut__DOT__scroll_offset)));
    if ((0x19U <= (IData)(test_copper_lists__DOT__dut__DOT__real_row))) {
        test_copper_lists__DOT__dut__DOT__real_row 
            = (0x0000001fU & ((IData)(test_copper_lists__DOT__dut__DOT__real_row) 
                              - (IData)(0x19U)));
    }
    vlSelfRef.test_copper_lists__DOT__dut__DOT__gfx_y 
        = (0x000000ffU & (IData)(test_copper_lists__DOT__dut__DOT__text_line));
    vlSelfRef.test_copper_lists__DOT__dut__DOT__in_text_area 
        = ((IData)(test_copper_lists__DOT__dut__DOT__visible) 
           & ((0x0028U <= (IData)(vlSelfRef.test_copper_lists__DOT__dut__DOT__v_count)) 
              & (0x01b8U > (IData)(vlSelfRef.test_copper_lists__DOT__dut__DOT__v_count))));
}
