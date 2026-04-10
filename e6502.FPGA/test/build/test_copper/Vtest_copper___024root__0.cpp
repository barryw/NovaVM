// Verilated -*- C++ -*-
// DESCRIPTION: Verilator output: Design implementation internals
// See Vtest_copper.h for the primary calling header

#include "Vtest_copper__pch.h"

VL_ATTR_COLD void Vtest_copper___024root___eval_initial__TOP(Vtest_copper___024root* vlSelf);
VlCoroutine Vtest_copper___024root___eval_initial__TOP__Vtiming__0(Vtest_copper___024root* vlSelf);
VlCoroutine Vtest_copper___024root___eval_initial__TOP__Vtiming__1(Vtest_copper___024root* vlSelf);

void Vtest_copper___024root___eval_initial(Vtest_copper___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vtest_copper___024root___eval_initial\n"); );
    Vtest_copper__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
    // Body
    Vtest_copper___024root___eval_initial__TOP(vlSelf);
    Vtest_copper___024root___eval_initial__TOP__Vtiming__0(vlSelf);
    Vtest_copper___024root___eval_initial__TOP__Vtiming__1(vlSelf);
}

void Vtest_copper___024root____VbeforeTrig_hbd4d80cf__0(Vtest_copper___024root* vlSelf, const char* __VeventDescription);

VlCoroutine Vtest_copper___024root___eval_initial__TOP__Vtiming__0(Vtest_copper___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vtest_copper___024root___eval_initial__TOP__Vtiming__0\n"); );
    Vtest_copper__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
    // Locals
    IData/*31:0*/ __Vtask_test_copper__DOT__run_clocks__0__test_copper__DOT__unnamedblk1_1__DOT____Vrepeat0;
    __Vtask_test_copper__DOT__run_clocks__0__test_copper__DOT__unnamedblk1_1__DOT____Vrepeat0 = 0;
    IData/*31:0*/ __Vtask_test_copper__DOT__run_clocks__1__n;
    __Vtask_test_copper__DOT__run_clocks__1__n = 0;
    IData/*31:0*/ __Vtask_test_copper__DOT__run_clocks__1__test_copper__DOT__unnamedblk1_1__DOT____Vrepeat0;
    __Vtask_test_copper__DOT__run_clocks__1__test_copper__DOT__unnamedblk1_1__DOT____Vrepeat0 = 0;
    CData/*0:0*/ __Vtask_test_copper__DOT__check__2__condition;
    __Vtask_test_copper__DOT__check__2__condition = 0;
    CData/*0:0*/ __Vtask_test_copper__DOT__check__3__condition;
    __Vtask_test_copper__DOT__check__3__condition = 0;
    IData/*31:0*/ __Vtask_test_copper__DOT__copper_add__4__x;
    __Vtask_test_copper__DOT__copper_add__4__x = 0;
    IData/*31:0*/ __Vtask_test_copper__DOT__copper_add__4__y;
    __Vtask_test_copper__DOT__copper_add__4__y = 0;
    IData/*31:0*/ __Vtask_test_copper__DOT__copper_add__4__reg_idx;
    __Vtask_test_copper__DOT__copper_add__4__reg_idx = 0;
    CData/*7:0*/ __Vtask_test_copper__DOT__copper_add__4__value;
    __Vtask_test_copper__DOT__copper_add__4__value = 0;
    IData/*31:0*/ __Vtask_test_copper__DOT__write_param__5__idx;
    __Vtask_test_copper__DOT__write_param__5__idx = 0;
    CData/*7:0*/ __Vtask_test_copper__DOT__write_param__5__data;
    __Vtask_test_copper__DOT__write_param__5__data = 0;
    SData/*15:0*/ __Vtask_test_copper__DOT__write_reg__6__addr;
    __Vtask_test_copper__DOT__write_reg__6__addr = 0;
    CData/*7:0*/ __Vtask_test_copper__DOT__write_reg__6__data;
    __Vtask_test_copper__DOT__write_reg__6__data = 0;
    IData/*31:0*/ __Vtask_test_copper__DOT__write_param__7__idx;
    __Vtask_test_copper__DOT__write_param__7__idx = 0;
    CData/*7:0*/ __Vtask_test_copper__DOT__write_param__7__data;
    __Vtask_test_copper__DOT__write_param__7__data = 0;
    SData/*15:0*/ __Vtask_test_copper__DOT__write_reg__8__addr;
    __Vtask_test_copper__DOT__write_reg__8__addr = 0;
    CData/*7:0*/ __Vtask_test_copper__DOT__write_reg__8__data;
    __Vtask_test_copper__DOT__write_reg__8__data = 0;
    IData/*31:0*/ __Vtask_test_copper__DOT__write_param__9__idx;
    __Vtask_test_copper__DOT__write_param__9__idx = 0;
    CData/*7:0*/ __Vtask_test_copper__DOT__write_param__9__data;
    __Vtask_test_copper__DOT__write_param__9__data = 0;
    SData/*15:0*/ __Vtask_test_copper__DOT__write_reg__10__addr;
    __Vtask_test_copper__DOT__write_reg__10__addr = 0;
    CData/*7:0*/ __Vtask_test_copper__DOT__write_reg__10__data;
    __Vtask_test_copper__DOT__write_reg__10__data = 0;
    IData/*31:0*/ __Vtask_test_copper__DOT__write_param__11__idx;
    __Vtask_test_copper__DOT__write_param__11__idx = 0;
    CData/*7:0*/ __Vtask_test_copper__DOT__write_param__11__data;
    __Vtask_test_copper__DOT__write_param__11__data = 0;
    SData/*15:0*/ __Vtask_test_copper__DOT__write_reg__12__addr;
    __Vtask_test_copper__DOT__write_reg__12__addr = 0;
    CData/*7:0*/ __Vtask_test_copper__DOT__write_reg__12__data;
    __Vtask_test_copper__DOT__write_reg__12__data = 0;
    IData/*31:0*/ __Vtask_test_copper__DOT__write_param__13__idx;
    __Vtask_test_copper__DOT__write_param__13__idx = 0;
    CData/*7:0*/ __Vtask_test_copper__DOT__write_param__13__data;
    __Vtask_test_copper__DOT__write_param__13__data = 0;
    SData/*15:0*/ __Vtask_test_copper__DOT__write_reg__14__addr;
    __Vtask_test_copper__DOT__write_reg__14__addr = 0;
    CData/*7:0*/ __Vtask_test_copper__DOT__write_reg__14__data;
    __Vtask_test_copper__DOT__write_reg__14__data = 0;
    IData/*31:0*/ __Vtask_test_copper__DOT__write_param__15__idx;
    __Vtask_test_copper__DOT__write_param__15__idx = 0;
    CData/*7:0*/ __Vtask_test_copper__DOT__write_param__15__data;
    __Vtask_test_copper__DOT__write_param__15__data = 0;
    SData/*15:0*/ __Vtask_test_copper__DOT__write_reg__16__addr;
    __Vtask_test_copper__DOT__write_reg__16__addr = 0;
    CData/*7:0*/ __Vtask_test_copper__DOT__write_reg__16__data;
    __Vtask_test_copper__DOT__write_reg__16__data = 0;
    CData/*7:0*/ __Vtask_test_copper__DOT__write_cmd__17__cmd;
    __Vtask_test_copper__DOT__write_cmd__17__cmd = 0;
    SData/*15:0*/ __Vtask_test_copper__DOT__write_reg__18__addr;
    __Vtask_test_copper__DOT__write_reg__18__addr = 0;
    CData/*7:0*/ __Vtask_test_copper__DOT__write_reg__18__data;
    __Vtask_test_copper__DOT__write_reg__18__data = 0;
    CData/*7:0*/ __Vtask_test_copper__DOT__write_cmd__20__cmd;
    __Vtask_test_copper__DOT__write_cmd__20__cmd = 0;
    SData/*15:0*/ __Vtask_test_copper__DOT__write_reg__21__addr;
    __Vtask_test_copper__DOT__write_reg__21__addr = 0;
    CData/*7:0*/ __Vtask_test_copper__DOT__write_reg__21__data;
    __Vtask_test_copper__DOT__write_reg__21__data = 0;
    IData/*31:0*/ __Vtask_test_copper__DOT__run_clocks__22__n;
    __Vtask_test_copper__DOT__run_clocks__22__n = 0;
    IData/*31:0*/ __Vtask_test_copper__DOT__run_clocks__22__test_copper__DOT__unnamedblk1_1__DOT____Vrepeat0;
    __Vtask_test_copper__DOT__run_clocks__22__test_copper__DOT__unnamedblk1_1__DOT____Vrepeat0 = 0;
    CData/*0:0*/ __Vtask_test_copper__DOT__check__23__condition;
    __Vtask_test_copper__DOT__check__23__condition = 0;
    IData/*31:0*/ __Vtask_test_copper__DOT__run_to_scanline__24__target;
    __Vtask_test_copper__DOT__run_to_scanline__24__target = 0;
    SData/*15:0*/ __Vtask_test_copper__DOT__write_reg__25__addr;
    __Vtask_test_copper__DOT__write_reg__25__addr = 0;
    CData/*7:0*/ __Vtask_test_copper__DOT__write_reg__25__data;
    __Vtask_test_copper__DOT__write_reg__25__data = 0;
    IData/*31:0*/ __Vtask_test_copper__DOT__run_clocks__26__n;
    __Vtask_test_copper__DOT__run_clocks__26__n = 0;
    IData/*31:0*/ __Vtask_test_copper__DOT__run_clocks__26__test_copper__DOT__unnamedblk1_1__DOT____Vrepeat0;
    __Vtask_test_copper__DOT__run_clocks__26__test_copper__DOT__unnamedblk1_1__DOT____Vrepeat0 = 0;
    IData/*31:0*/ __Vtask_test_copper__DOT__run_to_scanline__27__target;
    __Vtask_test_copper__DOT__run_to_scanline__27__target = 0;
    IData/*31:0*/ __Vtask_test_copper__DOT__run_to_scanline__28__target;
    __Vtask_test_copper__DOT__run_to_scanline__28__target = 0;
    CData/*0:0*/ __Vtask_test_copper__DOT__check__29__condition;
    __Vtask_test_copper__DOT__check__29__condition = 0;
    IData/*31:0*/ __Vtask_test_copper__DOT__run_to_scanline__30__target;
    __Vtask_test_copper__DOT__run_to_scanline__30__target = 0;
    CData/*0:0*/ __Vtask_test_copper__DOT__check__31__condition;
    __Vtask_test_copper__DOT__check__31__condition = 0;
    SData/*15:0*/ __Vtask_test_copper__DOT__write_reg__32__addr;
    __Vtask_test_copper__DOT__write_reg__32__addr = 0;
    CData/*7:0*/ __Vtask_test_copper__DOT__write_reg__32__data;
    __Vtask_test_copper__DOT__write_reg__32__data = 0;
    IData/*31:0*/ __Vtask_test_copper__DOT__run_clocks__33__n;
    __Vtask_test_copper__DOT__run_clocks__33__n = 0;
    IData/*31:0*/ __Vtask_test_copper__DOT__run_clocks__33__test_copper__DOT__unnamedblk1_1__DOT____Vrepeat0;
    __Vtask_test_copper__DOT__run_clocks__33__test_copper__DOT__unnamedblk1_1__DOT____Vrepeat0 = 0;
    CData/*0:0*/ __Vtask_test_copper__DOT__check__34__condition;
    __Vtask_test_copper__DOT__check__34__condition = 0;
    CData/*7:0*/ __Vtask_test_copper__DOT__write_cmd__36__cmd;
    __Vtask_test_copper__DOT__write_cmd__36__cmd = 0;
    SData/*15:0*/ __Vtask_test_copper__DOT__write_reg__37__addr;
    __Vtask_test_copper__DOT__write_reg__37__addr = 0;
    CData/*7:0*/ __Vtask_test_copper__DOT__write_reg__37__data;
    __Vtask_test_copper__DOT__write_reg__37__data = 0;
    IData/*31:0*/ __Vtask_test_copper__DOT__run_clocks__38__n;
    __Vtask_test_copper__DOT__run_clocks__38__n = 0;
    IData/*31:0*/ __Vtask_test_copper__DOT__run_clocks__38__test_copper__DOT__unnamedblk1_1__DOT____Vrepeat0;
    __Vtask_test_copper__DOT__run_clocks__38__test_copper__DOT__unnamedblk1_1__DOT____Vrepeat0 = 0;
    IData/*31:0*/ __Vtask_test_copper__DOT__run_clocks__40__n;
    __Vtask_test_copper__DOT__run_clocks__40__n = 0;
    IData/*31:0*/ __Vtask_test_copper__DOT__run_clocks__40__test_copper__DOT__unnamedblk1_1__DOT____Vrepeat0;
    __Vtask_test_copper__DOT__run_clocks__40__test_copper__DOT__unnamedblk1_1__DOT____Vrepeat0 = 0;
    IData/*31:0*/ __Vtask_test_copper__DOT__run_to_scanline__41__target;
    __Vtask_test_copper__DOT__run_to_scanline__41__target = 0;
    CData/*0:0*/ __Vtask_test_copper__DOT__check__42__condition;
    __Vtask_test_copper__DOT__check__42__condition = 0;
    CData/*7:0*/ __Vtask_test_copper__DOT__write_cmd__44__cmd;
    __Vtask_test_copper__DOT__write_cmd__44__cmd = 0;
    SData/*15:0*/ __Vtask_test_copper__DOT__write_reg__45__addr;
    __Vtask_test_copper__DOT__write_reg__45__addr = 0;
    CData/*7:0*/ __Vtask_test_copper__DOT__write_reg__45__data;
    __Vtask_test_copper__DOT__write_reg__45__data = 0;
    SData/*15:0*/ __Vtask_test_copper__DOT__write_reg__46__addr;
    __Vtask_test_copper__DOT__write_reg__46__addr = 0;
    CData/*7:0*/ __Vtask_test_copper__DOT__write_reg__46__data;
    __Vtask_test_copper__DOT__write_reg__46__data = 0;
    IData/*31:0*/ __Vtask_test_copper__DOT__run_clocks__47__n;
    __Vtask_test_copper__DOT__run_clocks__47__n = 0;
    IData/*31:0*/ __Vtask_test_copper__DOT__run_clocks__47__test_copper__DOT__unnamedblk1_1__DOT____Vrepeat0;
    __Vtask_test_copper__DOT__run_clocks__47__test_copper__DOT__unnamedblk1_1__DOT____Vrepeat0 = 0;
    CData/*7:0*/ __Vtask_test_copper__DOT__write_cmd__49__cmd;
    __Vtask_test_copper__DOT__write_cmd__49__cmd = 0;
    SData/*15:0*/ __Vtask_test_copper__DOT__write_reg__50__addr;
    __Vtask_test_copper__DOT__write_reg__50__addr = 0;
    CData/*7:0*/ __Vtask_test_copper__DOT__write_reg__50__data;
    __Vtask_test_copper__DOT__write_reg__50__data = 0;
    IData/*31:0*/ __Vtask_test_copper__DOT__run_clocks__52__n;
    __Vtask_test_copper__DOT__run_clocks__52__n = 0;
    IData/*31:0*/ __Vtask_test_copper__DOT__run_clocks__52__test_copper__DOT__unnamedblk1_1__DOT____Vrepeat0;
    __Vtask_test_copper__DOT__run_clocks__52__test_copper__DOT__unnamedblk1_1__DOT____Vrepeat0 = 0;
    IData/*31:0*/ __Vtask_test_copper__DOT__run_to_scanline__53__target;
    __Vtask_test_copper__DOT__run_to_scanline__53__target = 0;
    CData/*0:0*/ __Vtask_test_copper__DOT__check__54__condition;
    __Vtask_test_copper__DOT__check__54__condition = 0;
    IData/*31:0*/ __Vtask_test_copper__DOT__copper_add__55__x;
    __Vtask_test_copper__DOT__copper_add__55__x = 0;
    IData/*31:0*/ __Vtask_test_copper__DOT__copper_add__55__y;
    __Vtask_test_copper__DOT__copper_add__55__y = 0;
    IData/*31:0*/ __Vtask_test_copper__DOT__copper_add__55__reg_idx;
    __Vtask_test_copper__DOT__copper_add__55__reg_idx = 0;
    CData/*7:0*/ __Vtask_test_copper__DOT__copper_add__55__value;
    __Vtask_test_copper__DOT__copper_add__55__value = 0;
    IData/*31:0*/ __Vtask_test_copper__DOT__write_param__56__idx;
    __Vtask_test_copper__DOT__write_param__56__idx = 0;
    CData/*7:0*/ __Vtask_test_copper__DOT__write_param__56__data;
    __Vtask_test_copper__DOT__write_param__56__data = 0;
    SData/*15:0*/ __Vtask_test_copper__DOT__write_reg__57__addr;
    __Vtask_test_copper__DOT__write_reg__57__addr = 0;
    CData/*7:0*/ __Vtask_test_copper__DOT__write_reg__57__data;
    __Vtask_test_copper__DOT__write_reg__57__data = 0;
    IData/*31:0*/ __Vtask_test_copper__DOT__write_param__58__idx;
    __Vtask_test_copper__DOT__write_param__58__idx = 0;
    CData/*7:0*/ __Vtask_test_copper__DOT__write_param__58__data;
    __Vtask_test_copper__DOT__write_param__58__data = 0;
    SData/*15:0*/ __Vtask_test_copper__DOT__write_reg__59__addr;
    __Vtask_test_copper__DOT__write_reg__59__addr = 0;
    CData/*7:0*/ __Vtask_test_copper__DOT__write_reg__59__data;
    __Vtask_test_copper__DOT__write_reg__59__data = 0;
    IData/*31:0*/ __Vtask_test_copper__DOT__write_param__60__idx;
    __Vtask_test_copper__DOT__write_param__60__idx = 0;
    CData/*7:0*/ __Vtask_test_copper__DOT__write_param__60__data;
    __Vtask_test_copper__DOT__write_param__60__data = 0;
    SData/*15:0*/ __Vtask_test_copper__DOT__write_reg__61__addr;
    __Vtask_test_copper__DOT__write_reg__61__addr = 0;
    CData/*7:0*/ __Vtask_test_copper__DOT__write_reg__61__data;
    __Vtask_test_copper__DOT__write_reg__61__data = 0;
    IData/*31:0*/ __Vtask_test_copper__DOT__write_param__62__idx;
    __Vtask_test_copper__DOT__write_param__62__idx = 0;
    CData/*7:0*/ __Vtask_test_copper__DOT__write_param__62__data;
    __Vtask_test_copper__DOT__write_param__62__data = 0;
    SData/*15:0*/ __Vtask_test_copper__DOT__write_reg__63__addr;
    __Vtask_test_copper__DOT__write_reg__63__addr = 0;
    CData/*7:0*/ __Vtask_test_copper__DOT__write_reg__63__data;
    __Vtask_test_copper__DOT__write_reg__63__data = 0;
    IData/*31:0*/ __Vtask_test_copper__DOT__write_param__64__idx;
    __Vtask_test_copper__DOT__write_param__64__idx = 0;
    CData/*7:0*/ __Vtask_test_copper__DOT__write_param__64__data;
    __Vtask_test_copper__DOT__write_param__64__data = 0;
    SData/*15:0*/ __Vtask_test_copper__DOT__write_reg__65__addr;
    __Vtask_test_copper__DOT__write_reg__65__addr = 0;
    CData/*7:0*/ __Vtask_test_copper__DOT__write_reg__65__data;
    __Vtask_test_copper__DOT__write_reg__65__data = 0;
    IData/*31:0*/ __Vtask_test_copper__DOT__write_param__66__idx;
    __Vtask_test_copper__DOT__write_param__66__idx = 0;
    CData/*7:0*/ __Vtask_test_copper__DOT__write_param__66__data;
    __Vtask_test_copper__DOT__write_param__66__data = 0;
    SData/*15:0*/ __Vtask_test_copper__DOT__write_reg__67__addr;
    __Vtask_test_copper__DOT__write_reg__67__addr = 0;
    CData/*7:0*/ __Vtask_test_copper__DOT__write_reg__67__data;
    __Vtask_test_copper__DOT__write_reg__67__data = 0;
    CData/*7:0*/ __Vtask_test_copper__DOT__write_cmd__68__cmd;
    __Vtask_test_copper__DOT__write_cmd__68__cmd = 0;
    SData/*15:0*/ __Vtask_test_copper__DOT__write_reg__69__addr;
    __Vtask_test_copper__DOT__write_reg__69__addr = 0;
    CData/*7:0*/ __Vtask_test_copper__DOT__write_reg__69__data;
    __Vtask_test_copper__DOT__write_reg__69__data = 0;
    IData/*31:0*/ __Vtask_test_copper__DOT__copper_add__70__x;
    __Vtask_test_copper__DOT__copper_add__70__x = 0;
    IData/*31:0*/ __Vtask_test_copper__DOT__copper_add__70__y;
    __Vtask_test_copper__DOT__copper_add__70__y = 0;
    IData/*31:0*/ __Vtask_test_copper__DOT__copper_add__70__reg_idx;
    __Vtask_test_copper__DOT__copper_add__70__reg_idx = 0;
    CData/*7:0*/ __Vtask_test_copper__DOT__copper_add__70__value;
    __Vtask_test_copper__DOT__copper_add__70__value = 0;
    IData/*31:0*/ __Vtask_test_copper__DOT__write_param__71__idx;
    __Vtask_test_copper__DOT__write_param__71__idx = 0;
    CData/*7:0*/ __Vtask_test_copper__DOT__write_param__71__data;
    __Vtask_test_copper__DOT__write_param__71__data = 0;
    SData/*15:0*/ __Vtask_test_copper__DOT__write_reg__72__addr;
    __Vtask_test_copper__DOT__write_reg__72__addr = 0;
    CData/*7:0*/ __Vtask_test_copper__DOT__write_reg__72__data;
    __Vtask_test_copper__DOT__write_reg__72__data = 0;
    IData/*31:0*/ __Vtask_test_copper__DOT__write_param__73__idx;
    __Vtask_test_copper__DOT__write_param__73__idx = 0;
    CData/*7:0*/ __Vtask_test_copper__DOT__write_param__73__data;
    __Vtask_test_copper__DOT__write_param__73__data = 0;
    SData/*15:0*/ __Vtask_test_copper__DOT__write_reg__74__addr;
    __Vtask_test_copper__DOT__write_reg__74__addr = 0;
    CData/*7:0*/ __Vtask_test_copper__DOT__write_reg__74__data;
    __Vtask_test_copper__DOT__write_reg__74__data = 0;
    IData/*31:0*/ __Vtask_test_copper__DOT__write_param__75__idx;
    __Vtask_test_copper__DOT__write_param__75__idx = 0;
    CData/*7:0*/ __Vtask_test_copper__DOT__write_param__75__data;
    __Vtask_test_copper__DOT__write_param__75__data = 0;
    SData/*15:0*/ __Vtask_test_copper__DOT__write_reg__76__addr;
    __Vtask_test_copper__DOT__write_reg__76__addr = 0;
    CData/*7:0*/ __Vtask_test_copper__DOT__write_reg__76__data;
    __Vtask_test_copper__DOT__write_reg__76__data = 0;
    IData/*31:0*/ __Vtask_test_copper__DOT__write_param__77__idx;
    __Vtask_test_copper__DOT__write_param__77__idx = 0;
    CData/*7:0*/ __Vtask_test_copper__DOT__write_param__77__data;
    __Vtask_test_copper__DOT__write_param__77__data = 0;
    SData/*15:0*/ __Vtask_test_copper__DOT__write_reg__78__addr;
    __Vtask_test_copper__DOT__write_reg__78__addr = 0;
    CData/*7:0*/ __Vtask_test_copper__DOT__write_reg__78__data;
    __Vtask_test_copper__DOT__write_reg__78__data = 0;
    IData/*31:0*/ __Vtask_test_copper__DOT__write_param__79__idx;
    __Vtask_test_copper__DOT__write_param__79__idx = 0;
    CData/*7:0*/ __Vtask_test_copper__DOT__write_param__79__data;
    __Vtask_test_copper__DOT__write_param__79__data = 0;
    SData/*15:0*/ __Vtask_test_copper__DOT__write_reg__80__addr;
    __Vtask_test_copper__DOT__write_reg__80__addr = 0;
    CData/*7:0*/ __Vtask_test_copper__DOT__write_reg__80__data;
    __Vtask_test_copper__DOT__write_reg__80__data = 0;
    IData/*31:0*/ __Vtask_test_copper__DOT__write_param__81__idx;
    __Vtask_test_copper__DOT__write_param__81__idx = 0;
    CData/*7:0*/ __Vtask_test_copper__DOT__write_param__81__data;
    __Vtask_test_copper__DOT__write_param__81__data = 0;
    SData/*15:0*/ __Vtask_test_copper__DOT__write_reg__82__addr;
    __Vtask_test_copper__DOT__write_reg__82__addr = 0;
    CData/*7:0*/ __Vtask_test_copper__DOT__write_reg__82__data;
    __Vtask_test_copper__DOT__write_reg__82__data = 0;
    CData/*7:0*/ __Vtask_test_copper__DOT__write_cmd__83__cmd;
    __Vtask_test_copper__DOT__write_cmd__83__cmd = 0;
    SData/*15:0*/ __Vtask_test_copper__DOT__write_reg__84__addr;
    __Vtask_test_copper__DOT__write_reg__84__addr = 0;
    CData/*7:0*/ __Vtask_test_copper__DOT__write_reg__84__data;
    __Vtask_test_copper__DOT__write_reg__84__data = 0;
    IData/*31:0*/ __Vtask_test_copper__DOT__copper_add__85__x;
    __Vtask_test_copper__DOT__copper_add__85__x = 0;
    IData/*31:0*/ __Vtask_test_copper__DOT__copper_add__85__y;
    __Vtask_test_copper__DOT__copper_add__85__y = 0;
    IData/*31:0*/ __Vtask_test_copper__DOT__copper_add__85__reg_idx;
    __Vtask_test_copper__DOT__copper_add__85__reg_idx = 0;
    CData/*7:0*/ __Vtask_test_copper__DOT__copper_add__85__value;
    __Vtask_test_copper__DOT__copper_add__85__value = 0;
    IData/*31:0*/ __Vtask_test_copper__DOT__write_param__86__idx;
    __Vtask_test_copper__DOT__write_param__86__idx = 0;
    CData/*7:0*/ __Vtask_test_copper__DOT__write_param__86__data;
    __Vtask_test_copper__DOT__write_param__86__data = 0;
    SData/*15:0*/ __Vtask_test_copper__DOT__write_reg__87__addr;
    __Vtask_test_copper__DOT__write_reg__87__addr = 0;
    CData/*7:0*/ __Vtask_test_copper__DOT__write_reg__87__data;
    __Vtask_test_copper__DOT__write_reg__87__data = 0;
    IData/*31:0*/ __Vtask_test_copper__DOT__write_param__88__idx;
    __Vtask_test_copper__DOT__write_param__88__idx = 0;
    CData/*7:0*/ __Vtask_test_copper__DOT__write_param__88__data;
    __Vtask_test_copper__DOT__write_param__88__data = 0;
    SData/*15:0*/ __Vtask_test_copper__DOT__write_reg__89__addr;
    __Vtask_test_copper__DOT__write_reg__89__addr = 0;
    CData/*7:0*/ __Vtask_test_copper__DOT__write_reg__89__data;
    __Vtask_test_copper__DOT__write_reg__89__data = 0;
    IData/*31:0*/ __Vtask_test_copper__DOT__write_param__90__idx;
    __Vtask_test_copper__DOT__write_param__90__idx = 0;
    CData/*7:0*/ __Vtask_test_copper__DOT__write_param__90__data;
    __Vtask_test_copper__DOT__write_param__90__data = 0;
    SData/*15:0*/ __Vtask_test_copper__DOT__write_reg__91__addr;
    __Vtask_test_copper__DOT__write_reg__91__addr = 0;
    CData/*7:0*/ __Vtask_test_copper__DOT__write_reg__91__data;
    __Vtask_test_copper__DOT__write_reg__91__data = 0;
    IData/*31:0*/ __Vtask_test_copper__DOT__write_param__92__idx;
    __Vtask_test_copper__DOT__write_param__92__idx = 0;
    CData/*7:0*/ __Vtask_test_copper__DOT__write_param__92__data;
    __Vtask_test_copper__DOT__write_param__92__data = 0;
    SData/*15:0*/ __Vtask_test_copper__DOT__write_reg__93__addr;
    __Vtask_test_copper__DOT__write_reg__93__addr = 0;
    CData/*7:0*/ __Vtask_test_copper__DOT__write_reg__93__data;
    __Vtask_test_copper__DOT__write_reg__93__data = 0;
    IData/*31:0*/ __Vtask_test_copper__DOT__write_param__94__idx;
    __Vtask_test_copper__DOT__write_param__94__idx = 0;
    CData/*7:0*/ __Vtask_test_copper__DOT__write_param__94__data;
    __Vtask_test_copper__DOT__write_param__94__data = 0;
    SData/*15:0*/ __Vtask_test_copper__DOT__write_reg__95__addr;
    __Vtask_test_copper__DOT__write_reg__95__addr = 0;
    CData/*7:0*/ __Vtask_test_copper__DOT__write_reg__95__data;
    __Vtask_test_copper__DOT__write_reg__95__data = 0;
    IData/*31:0*/ __Vtask_test_copper__DOT__write_param__96__idx;
    __Vtask_test_copper__DOT__write_param__96__idx = 0;
    CData/*7:0*/ __Vtask_test_copper__DOT__write_param__96__data;
    __Vtask_test_copper__DOT__write_param__96__data = 0;
    SData/*15:0*/ __Vtask_test_copper__DOT__write_reg__97__addr;
    __Vtask_test_copper__DOT__write_reg__97__addr = 0;
    CData/*7:0*/ __Vtask_test_copper__DOT__write_reg__97__data;
    __Vtask_test_copper__DOT__write_reg__97__data = 0;
    CData/*7:0*/ __Vtask_test_copper__DOT__write_cmd__98__cmd;
    __Vtask_test_copper__DOT__write_cmd__98__cmd = 0;
    SData/*15:0*/ __Vtask_test_copper__DOT__write_reg__99__addr;
    __Vtask_test_copper__DOT__write_reg__99__addr = 0;
    CData/*7:0*/ __Vtask_test_copper__DOT__write_reg__99__data;
    __Vtask_test_copper__DOT__write_reg__99__data = 0;
    SData/*15:0*/ __Vtask_test_copper__DOT__write_reg__100__addr;
    __Vtask_test_copper__DOT__write_reg__100__addr = 0;
    CData/*7:0*/ __Vtask_test_copper__DOT__write_reg__100__data;
    __Vtask_test_copper__DOT__write_reg__100__data = 0;
    IData/*31:0*/ __Vtask_test_copper__DOT__run_clocks__101__n;
    __Vtask_test_copper__DOT__run_clocks__101__n = 0;
    IData/*31:0*/ __Vtask_test_copper__DOT__run_clocks__101__test_copper__DOT__unnamedblk1_1__DOT____Vrepeat0;
    __Vtask_test_copper__DOT__run_clocks__101__test_copper__DOT__unnamedblk1_1__DOT____Vrepeat0 = 0;
    IData/*31:0*/ __Vtask_test_copper__DOT__run_to_scanline__102__target;
    __Vtask_test_copper__DOT__run_to_scanline__102__target = 0;
    IData/*31:0*/ __Vtask_test_copper__DOT__run_to_scanline__103__target;
    __Vtask_test_copper__DOT__run_to_scanline__103__target = 0;
    IData/*31:0*/ __Vtask_test_copper__DOT__run_to_scanline__104__target;
    __Vtask_test_copper__DOT__run_to_scanline__104__target = 0;
    CData/*0:0*/ __Vtask_test_copper__DOT__check__105__condition;
    __Vtask_test_copper__DOT__check__105__condition = 0;
    IData/*31:0*/ __Vtask_test_copper__DOT__run_to_scanline__106__target;
    __Vtask_test_copper__DOT__run_to_scanline__106__target = 0;
    CData/*0:0*/ __Vtask_test_copper__DOT__check__107__condition;
    __Vtask_test_copper__DOT__check__107__condition = 0;
    IData/*31:0*/ __Vtask_test_copper__DOT__run_to_scanline__108__target;
    __Vtask_test_copper__DOT__run_to_scanline__108__target = 0;
    CData/*0:0*/ __Vtask_test_copper__DOT__check__109__condition;
    __Vtask_test_copper__DOT__check__109__condition = 0;
    // Body
    VL_WRITEF_NX("=== Copper Unit Tests ===\n\n",0);
    vlSelfRef.test_copper__DOT__rst = 1U;
    vlSelfRef.test_copper__DOT__cpu_we = 0U;
    vlSelfRef.test_copper__DOT__cpu_addr = 0U;
    vlSelfRef.test_copper__DOT__cpu_wdata = 0U;
    __Vtask_test_copper__DOT__run_clocks__0__test_copper__DOT__unnamedblk1_1__DOT____Vrepeat0 = 0x00000032U;
    while (VL_LTS_III(32, 0U, __Vtask_test_copper__DOT__run_clocks__0__test_copper__DOT__unnamedblk1_1__DOT____Vrepeat0)) {
        Vtest_copper___024root____VbeforeTrig_hbd4d80cf__0(vlSelf, 
                                                           "@(posedge test_copper.clk)");
        co_await vlSelfRef.__VtrigSched_hbd4d80cf__0.trigger(0U, 
                                                             nullptr, 
                                                             "@(posedge test_copper.clk)", 
                                                             "test_copper.sv", 
                                                             101);
        __Vtask_test_copper__DOT__run_clocks__0__test_copper__DOT__unnamedblk1_1__DOT____Vrepeat0 
            = (__Vtask_test_copper__DOT__run_clocks__0__test_copper__DOT__unnamedblk1_1__DOT____Vrepeat0 
               - (IData)(1U));
    }
    vlSelfRef.test_copper__DOT__rst = 0U;
    __Vtask_test_copper__DOT__run_clocks__1__n = 0x0000000aU;
    __Vtask_test_copper__DOT__run_clocks__1__test_copper__DOT__unnamedblk1_1__DOT____Vrepeat0 = 0;
    __Vtask_test_copper__DOT__run_clocks__1__test_copper__DOT__unnamedblk1_1__DOT____Vrepeat0 
        = __Vtask_test_copper__DOT__run_clocks__1__n;
    while (VL_LTS_III(32, 0U, __Vtask_test_copper__DOT__run_clocks__1__test_copper__DOT__unnamedblk1_1__DOT____Vrepeat0)) {
        Vtest_copper___024root____VbeforeTrig_hbd4d80cf__0(vlSelf, 
                                                           "@(posedge test_copper.clk)");
        co_await vlSelfRef.__VtrigSched_hbd4d80cf__0.trigger(0U, 
                                                             nullptr, 
                                                             "@(posedge test_copper.clk)", 
                                                             "test_copper.sv", 
                                                             101);
        __Vtask_test_copper__DOT__run_clocks__1__test_copper__DOT__unnamedblk1_1__DOT____Vrepeat0 
            = (__Vtask_test_copper__DOT__run_clocks__1__test_copper__DOT__unnamedblk1_1__DOT____Vrepeat0 
               - (IData)(1U));
    }
    VL_WRITEF_NX("Test: Initial state\n",0);
    __Vtask_test_copper__DOT__check__2__condition = 
        (6U == (IData)(vlSelfRef.test_copper__DOT__dut__DOT__bg_color));
    vlSelfRef.__Vtask_test_copper__DOT__check__2__name = "bg_color starts as 6 (blue)"s;
    vlSelfRef.test_copper__DOT__test_num = ((IData)(1U) 
                                            + vlSelfRef.test_copper__DOT__test_num);
    if (__Vtask_test_copper__DOT__check__2__condition) {
        VL_WRITEF_NX("  PASS [%0d] %@\n",0,32,vlSelfRef.test_copper__DOT__test_num,
                     -1,&(vlSelfRef.__Vtask_test_copper__DOT__check__2__name));
        vlSelfRef.test_copper__DOT__pass_count = ((IData)(1U) 
                                                  + vlSelfRef.test_copper__DOT__pass_count);
    } else {
        VL_WRITEF_NX("  FAIL [%0d] %@\n",0,32,vlSelfRef.test_copper__DOT__test_num,
                     -1,&(vlSelfRef.__Vtask_test_copper__DOT__check__2__name));
        vlSelfRef.test_copper__DOT__fail_count = ((IData)(1U) 
                                                  + vlSelfRef.test_copper__DOT__fail_count);
    }
    __Vtask_test_copper__DOT__check__3__condition = 
        (1U & (~ (IData)(vlSelfRef.test_copper__DOT__dut__DOT__copper_enabled)));
    vlSelfRef.__Vtask_test_copper__DOT__check__3__name = "copper disabled by default"s;
    vlSelfRef.test_copper__DOT__test_num = ((IData)(1U) 
                                            + vlSelfRef.test_copper__DOT__test_num);
    if (__Vtask_test_copper__DOT__check__3__condition) {
        VL_WRITEF_NX("  PASS [%0d] %@\n",0,32,vlSelfRef.test_copper__DOT__test_num,
                     -1,&(vlSelfRef.__Vtask_test_copper__DOT__check__3__name));
        vlSelfRef.test_copper__DOT__pass_count = ((IData)(1U) 
                                                  + vlSelfRef.test_copper__DOT__pass_count);
    } else {
        VL_WRITEF_NX("  FAIL [%0d] %@\n",0,32,vlSelfRef.test_copper__DOT__test_num,
                     -1,&(vlSelfRef.__Vtask_test_copper__DOT__check__3__name));
        vlSelfRef.test_copper__DOT__fail_count = ((IData)(1U) 
                                                  + vlSelfRef.test_copper__DOT__fail_count);
    }
    VL_WRITEF_NX("Test: Copper add + enable\n",0);
    __Vtask_test_copper__DOT__copper_add__4__value = 2U;
    __Vtask_test_copper__DOT__copper_add__4__reg_idx = 1U;
    __Vtask_test_copper__DOT__copper_add__4__y = 0x00000032U;
    __Vtask_test_copper__DOT__copper_add__4__x = 0U;
    __Vtask_test_copper__DOT__write_param__5__data 
        = (0x000000ffU & __Vtask_test_copper__DOT__copper_add__4__x);
    __Vtask_test_copper__DOT__write_param__5__idx = 0U;
    __Vtask_test_copper__DOT__write_reg__6__data = __Vtask_test_copper__DOT__write_param__5__data;
    __Vtask_test_copper__DOT__write_reg__6__addr = 
        (0x0000ffffU & ((IData)(0xa011U) + __Vtask_test_copper__DOT__write_param__5__idx));
    Vtest_copper___024root____VbeforeTrig_hbd4d80cf__0(vlSelf, 
                                                       "@(posedge test_copper.clk)");
    co_await vlSelfRef.__VtrigSched_hbd4d80cf__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge test_copper.clk)", 
                                                         "test_copper.sv", 
                                                         55);
    vlSelfRef.test_copper__DOT__cpu_addr = __Vtask_test_copper__DOT__write_reg__6__addr;
    vlSelfRef.test_copper__DOT__cpu_wdata = __Vtask_test_copper__DOT__write_reg__6__data;
    vlSelfRef.test_copper__DOT__cpu_we = 1U;
    Vtest_copper___024root____VbeforeTrig_hbd4d80cf__0(vlSelf, 
                                                       "@(posedge test_copper.clk)");
    co_await vlSelfRef.__VtrigSched_hbd4d80cf__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge test_copper.clk)", 
                                                         "test_copper.sv", 
                                                         60);
    vlSelfRef.test_copper__DOT__cpu_we = 0U;
    __Vtask_test_copper__DOT__write_param__7__data 
        = (0x000000ffU & (__Vtask_test_copper__DOT__copper_add__4__x 
                          >> 8U));
    __Vtask_test_copper__DOT__write_param__7__idx = 1U;
    __Vtask_test_copper__DOT__write_reg__8__data = __Vtask_test_copper__DOT__write_param__7__data;
    __Vtask_test_copper__DOT__write_reg__8__addr = 
        (0x0000ffffU & ((IData)(0xa011U) + __Vtask_test_copper__DOT__write_param__7__idx));
    Vtest_copper___024root____VbeforeTrig_hbd4d80cf__0(vlSelf, 
                                                       "@(posedge test_copper.clk)");
    co_await vlSelfRef.__VtrigSched_hbd4d80cf__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge test_copper.clk)", 
                                                         "test_copper.sv", 
                                                         55);
    vlSelfRef.test_copper__DOT__cpu_addr = __Vtask_test_copper__DOT__write_reg__8__addr;
    vlSelfRef.test_copper__DOT__cpu_wdata = __Vtask_test_copper__DOT__write_reg__8__data;
    vlSelfRef.test_copper__DOT__cpu_we = 1U;
    Vtest_copper___024root____VbeforeTrig_hbd4d80cf__0(vlSelf, 
                                                       "@(posedge test_copper.clk)");
    co_await vlSelfRef.__VtrigSched_hbd4d80cf__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge test_copper.clk)", 
                                                         "test_copper.sv", 
                                                         60);
    vlSelfRef.test_copper__DOT__cpu_we = 0U;
    __Vtask_test_copper__DOT__write_param__9__data 
        = (0x000000ffU & __Vtask_test_copper__DOT__copper_add__4__y);
    __Vtask_test_copper__DOT__write_param__9__idx = 2U;
    __Vtask_test_copper__DOT__write_reg__10__data = __Vtask_test_copper__DOT__write_param__9__data;
    __Vtask_test_copper__DOT__write_reg__10__addr = 
        (0x0000ffffU & ((IData)(0xa011U) + __Vtask_test_copper__DOT__write_param__9__idx));
    Vtest_copper___024root____VbeforeTrig_hbd4d80cf__0(vlSelf, 
                                                       "@(posedge test_copper.clk)");
    co_await vlSelfRef.__VtrigSched_hbd4d80cf__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge test_copper.clk)", 
                                                         "test_copper.sv", 
                                                         55);
    vlSelfRef.test_copper__DOT__cpu_addr = __Vtask_test_copper__DOT__write_reg__10__addr;
    vlSelfRef.test_copper__DOT__cpu_wdata = __Vtask_test_copper__DOT__write_reg__10__data;
    vlSelfRef.test_copper__DOT__cpu_we = 1U;
    Vtest_copper___024root____VbeforeTrig_hbd4d80cf__0(vlSelf, 
                                                       "@(posedge test_copper.clk)");
    co_await vlSelfRef.__VtrigSched_hbd4d80cf__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge test_copper.clk)", 
                                                         "test_copper.sv", 
                                                         60);
    vlSelfRef.test_copper__DOT__cpu_we = 0U;
    __Vtask_test_copper__DOT__write_param__11__data 
        = (0x000000ffU & __Vtask_test_copper__DOT__copper_add__4__reg_idx);
    __Vtask_test_copper__DOT__write_param__11__idx = 3U;
    __Vtask_test_copper__DOT__write_reg__12__data = __Vtask_test_copper__DOT__write_param__11__data;
    __Vtask_test_copper__DOT__write_reg__12__addr = 
        (0x0000ffffU & ((IData)(0xa011U) + __Vtask_test_copper__DOT__write_param__11__idx));
    Vtest_copper___024root____VbeforeTrig_hbd4d80cf__0(vlSelf, 
                                                       "@(posedge test_copper.clk)");
    co_await vlSelfRef.__VtrigSched_hbd4d80cf__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge test_copper.clk)", 
                                                         "test_copper.sv", 
                                                         55);
    vlSelfRef.test_copper__DOT__cpu_addr = __Vtask_test_copper__DOT__write_reg__12__addr;
    vlSelfRef.test_copper__DOT__cpu_wdata = __Vtask_test_copper__DOT__write_reg__12__data;
    vlSelfRef.test_copper__DOT__cpu_we = 1U;
    Vtest_copper___024root____VbeforeTrig_hbd4d80cf__0(vlSelf, 
                                                       "@(posedge test_copper.clk)");
    co_await vlSelfRef.__VtrigSched_hbd4d80cf__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge test_copper.clk)", 
                                                         "test_copper.sv", 
                                                         60);
    vlSelfRef.test_copper__DOT__cpu_we = 0U;
    __Vtask_test_copper__DOT__write_param__13__data 
        = (0x000000ffU & (__Vtask_test_copper__DOT__copper_add__4__reg_idx 
                          >> 8U));
    __Vtask_test_copper__DOT__write_param__13__idx = 4U;
    __Vtask_test_copper__DOT__write_reg__14__data = __Vtask_test_copper__DOT__write_param__13__data;
    __Vtask_test_copper__DOT__write_reg__14__addr = 
        (0x0000ffffU & ((IData)(0xa011U) + __Vtask_test_copper__DOT__write_param__13__idx));
    Vtest_copper___024root____VbeforeTrig_hbd4d80cf__0(vlSelf, 
                                                       "@(posedge test_copper.clk)");
    co_await vlSelfRef.__VtrigSched_hbd4d80cf__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge test_copper.clk)", 
                                                         "test_copper.sv", 
                                                         55);
    vlSelfRef.test_copper__DOT__cpu_addr = __Vtask_test_copper__DOT__write_reg__14__addr;
    vlSelfRef.test_copper__DOT__cpu_wdata = __Vtask_test_copper__DOT__write_reg__14__data;
    vlSelfRef.test_copper__DOT__cpu_we = 1U;
    Vtest_copper___024root____VbeforeTrig_hbd4d80cf__0(vlSelf, 
                                                       "@(posedge test_copper.clk)");
    co_await vlSelfRef.__VtrigSched_hbd4d80cf__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge test_copper.clk)", 
                                                         "test_copper.sv", 
                                                         60);
    vlSelfRef.test_copper__DOT__cpu_we = 0U;
    __Vtask_test_copper__DOT__write_param__15__data 
        = __Vtask_test_copper__DOT__copper_add__4__value;
    __Vtask_test_copper__DOT__write_param__15__idx = 5U;
    __Vtask_test_copper__DOT__write_reg__16__data = __Vtask_test_copper__DOT__write_param__15__data;
    __Vtask_test_copper__DOT__write_reg__16__addr = 
        (0x0000ffffU & ((IData)(0xa011U) + __Vtask_test_copper__DOT__write_param__15__idx));
    Vtest_copper___024root____VbeforeTrig_hbd4d80cf__0(vlSelf, 
                                                       "@(posedge test_copper.clk)");
    co_await vlSelfRef.__VtrigSched_hbd4d80cf__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge test_copper.clk)", 
                                                         "test_copper.sv", 
                                                         55);
    vlSelfRef.test_copper__DOT__cpu_addr = __Vtask_test_copper__DOT__write_reg__16__addr;
    vlSelfRef.test_copper__DOT__cpu_wdata = __Vtask_test_copper__DOT__write_reg__16__data;
    vlSelfRef.test_copper__DOT__cpu_we = 1U;
    Vtest_copper___024root____VbeforeTrig_hbd4d80cf__0(vlSelf, 
                                                       "@(posedge test_copper.clk)");
    co_await vlSelfRef.__VtrigSched_hbd4d80cf__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge test_copper.clk)", 
                                                         "test_copper.sv", 
                                                         60);
    vlSelfRef.test_copper__DOT__cpu_we = 0U;
    __Vtask_test_copper__DOT__write_cmd__17__cmd = 0x1bU;
    __Vtask_test_copper__DOT__write_reg__18__data = __Vtask_test_copper__DOT__write_cmd__17__cmd;
    __Vtask_test_copper__DOT__write_reg__18__addr = 0xa010U;
    Vtest_copper___024root____VbeforeTrig_hbd4d80cf__0(vlSelf, 
                                                       "@(posedge test_copper.clk)");
    co_await vlSelfRef.__VtrigSched_hbd4d80cf__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge test_copper.clk)", 
                                                         "test_copper.sv", 
                                                         55);
    vlSelfRef.test_copper__DOT__cpu_addr = __Vtask_test_copper__DOT__write_reg__18__addr;
    vlSelfRef.test_copper__DOT__cpu_wdata = __Vtask_test_copper__DOT__write_reg__18__data;
    vlSelfRef.test_copper__DOT__cpu_we = 1U;
    Vtest_copper___024root____VbeforeTrig_hbd4d80cf__0(vlSelf, 
                                                       "@(posedge test_copper.clk)");
    co_await vlSelfRef.__VtrigSched_hbd4d80cf__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge test_copper.clk)", 
                                                         "test_copper.sv", 
                                                         60);
    vlSelfRef.test_copper__DOT__cpu_we = 0U;
    __Vtask_test_copper__DOT__write_cmd__20__cmd = 0x1dU;
    __Vtask_test_copper__DOT__write_reg__21__data = __Vtask_test_copper__DOT__write_cmd__20__cmd;
    __Vtask_test_copper__DOT__write_reg__21__addr = 0xa010U;
    Vtest_copper___024root____VbeforeTrig_hbd4d80cf__0(vlSelf, 
                                                       "@(posedge test_copper.clk)");
    co_await vlSelfRef.__VtrigSched_hbd4d80cf__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge test_copper.clk)", 
                                                         "test_copper.sv", 
                                                         55);
    vlSelfRef.test_copper__DOT__cpu_addr = __Vtask_test_copper__DOT__write_reg__21__addr;
    vlSelfRef.test_copper__DOT__cpu_wdata = __Vtask_test_copper__DOT__write_reg__21__data;
    vlSelfRef.test_copper__DOT__cpu_we = 1U;
    Vtest_copper___024root____VbeforeTrig_hbd4d80cf__0(vlSelf, 
                                                       "@(posedge test_copper.clk)");
    co_await vlSelfRef.__VtrigSched_hbd4d80cf__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge test_copper.clk)", 
                                                         "test_copper.sv", 
                                                         60);
    vlSelfRef.test_copper__DOT__cpu_we = 0U;
    __Vtask_test_copper__DOT__run_clocks__22__n = 2U;
    __Vtask_test_copper__DOT__run_clocks__22__test_copper__DOT__unnamedblk1_1__DOT____Vrepeat0 = 0;
    __Vtask_test_copper__DOT__run_clocks__22__test_copper__DOT__unnamedblk1_1__DOT____Vrepeat0 
        = __Vtask_test_copper__DOT__run_clocks__22__n;
    while (VL_LTS_III(32, 0U, __Vtask_test_copper__DOT__run_clocks__22__test_copper__DOT__unnamedblk1_1__DOT____Vrepeat0)) {
        Vtest_copper___024root____VbeforeTrig_hbd4d80cf__0(vlSelf, 
                                                           "@(posedge test_copper.clk)");
        co_await vlSelfRef.__VtrigSched_hbd4d80cf__0.trigger(0U, 
                                                             nullptr, 
                                                             "@(posedge test_copper.clk)", 
                                                             "test_copper.sv", 
                                                             101);
        __Vtask_test_copper__DOT__run_clocks__22__test_copper__DOT__unnamedblk1_1__DOT____Vrepeat0 
            = (__Vtask_test_copper__DOT__run_clocks__22__test_copper__DOT__unnamedblk1_1__DOT____Vrepeat0 
               - (IData)(1U));
    }
    __Vtask_test_copper__DOT__check__23__condition 
        = vlSelfRef.test_copper__DOT__dut__DOT__copper_enabled;
    vlSelfRef.__Vtask_test_copper__DOT__check__23__name = "copper enabled after command"s;
    vlSelfRef.test_copper__DOT__test_num = ((IData)(1U) 
                                            + vlSelfRef.test_copper__DOT__test_num);
    if (__Vtask_test_copper__DOT__check__23__condition) {
        VL_WRITEF_NX("  PASS [%0d] %@\n",0,32,vlSelfRef.test_copper__DOT__test_num,
                     -1,&(vlSelfRef.__Vtask_test_copper__DOT__check__23__name));
        vlSelfRef.test_copper__DOT__pass_count = ((IData)(1U) 
                                                  + vlSelfRef.test_copper__DOT__pass_count);
    } else {
        VL_WRITEF_NX("  FAIL [%0d] %@\n",0,32,vlSelfRef.test_copper__DOT__test_num,
                     -1,&(vlSelfRef.__Vtask_test_copper__DOT__check__23__name));
        vlSelfRef.test_copper__DOT__fail_count = ((IData)(1U) 
                                                  + vlSelfRef.test_copper__DOT__fail_count);
    }
    VL_WRITEF_NX("Test: Copper trigger timing\n",0);
    __Vtask_test_copper__DOT__run_to_scanline__24__target = 0x000001e0U;
    while ((((IData)(vlSelfRef.test_copper__DOT__dut__DOT__v_count) 
             != __Vtask_test_copper__DOT__run_to_scanline__24__target) 
            | (0U != (IData)(vlSelfRef.test_copper__DOT__dut__DOT__h_count)))) {
        Vtest_copper___024root____VbeforeTrig_hbd4d80cf__0(vlSelf, 
                                                           "@(posedge test_copper.clk)");
        co_await vlSelfRef.__VtrigSched_hbd4d80cf__0.trigger(0U, 
                                                             nullptr, 
                                                             "@(posedge test_copper.clk)", 
                                                             "test_copper.sv", 
                                                             107);
    }
    __Vtask_test_copper__DOT__write_reg__25__data = 6U;
    __Vtask_test_copper__DOT__write_reg__25__addr = 0xa001U;
    Vtest_copper___024root____VbeforeTrig_hbd4d80cf__0(vlSelf, 
                                                       "@(posedge test_copper.clk)");
    co_await vlSelfRef.__VtrigSched_hbd4d80cf__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge test_copper.clk)", 
                                                         "test_copper.sv", 
                                                         55);
    vlSelfRef.test_copper__DOT__cpu_addr = __Vtask_test_copper__DOT__write_reg__25__addr;
    vlSelfRef.test_copper__DOT__cpu_wdata = __Vtask_test_copper__DOT__write_reg__25__data;
    vlSelfRef.test_copper__DOT__cpu_we = 1U;
    Vtest_copper___024root____VbeforeTrig_hbd4d80cf__0(vlSelf, 
                                                       "@(posedge test_copper.clk)");
    co_await vlSelfRef.__VtrigSched_hbd4d80cf__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge test_copper.clk)", 
                                                         "test_copper.sv", 
                                                         60);
    vlSelfRef.test_copper__DOT__cpu_we = 0U;
    __Vtask_test_copper__DOT__run_clocks__26__n = 2U;
    __Vtask_test_copper__DOT__run_clocks__26__test_copper__DOT__unnamedblk1_1__DOT____Vrepeat0 = 0;
    __Vtask_test_copper__DOT__run_clocks__26__test_copper__DOT__unnamedblk1_1__DOT____Vrepeat0 
        = __Vtask_test_copper__DOT__run_clocks__26__n;
    while (VL_LTS_III(32, 0U, __Vtask_test_copper__DOT__run_clocks__26__test_copper__DOT__unnamedblk1_1__DOT____Vrepeat0)) {
        Vtest_copper___024root____VbeforeTrig_hbd4d80cf__0(vlSelf, 
                                                           "@(posedge test_copper.clk)");
        co_await vlSelfRef.__VtrigSched_hbd4d80cf__0.trigger(0U, 
                                                             nullptr, 
                                                             "@(posedge test_copper.clk)", 
                                                             "test_copper.sv", 
                                                             101);
        __Vtask_test_copper__DOT__run_clocks__26__test_copper__DOT__unnamedblk1_1__DOT____Vrepeat0 
            = (__Vtask_test_copper__DOT__run_clocks__26__test_copper__DOT__unnamedblk1_1__DOT____Vrepeat0 
               - (IData)(1U));
    }
    __Vtask_test_copper__DOT__run_to_scanline__27__target = 0U;
    while ((((IData)(vlSelfRef.test_copper__DOT__dut__DOT__v_count) 
             != __Vtask_test_copper__DOT__run_to_scanline__27__target) 
            | (0U != (IData)(vlSelfRef.test_copper__DOT__dut__DOT__h_count)))) {
        Vtest_copper___024root____VbeforeTrig_hbd4d80cf__0(vlSelf, 
                                                           "@(posedge test_copper.clk)");
        co_await vlSelfRef.__VtrigSched_hbd4d80cf__0.trigger(0U, 
                                                             nullptr, 
                                                             "@(posedge test_copper.clk)", 
                                                             "test_copper.sv", 
                                                             107);
    }
    __Vtask_test_copper__DOT__run_to_scanline__28__target = 0x0000008bU;
    while ((((IData)(vlSelfRef.test_copper__DOT__dut__DOT__v_count) 
             != __Vtask_test_copper__DOT__run_to_scanline__28__target) 
            | (0U != (IData)(vlSelfRef.test_copper__DOT__dut__DOT__h_count)))) {
        Vtest_copper___024root____VbeforeTrig_hbd4d80cf__0(vlSelf, 
                                                           "@(posedge test_copper.clk)");
        co_await vlSelfRef.__VtrigSched_hbd4d80cf__0.trigger(0U, 
                                                             nullptr, 
                                                             "@(posedge test_copper.clk)", 
                                                             "test_copper.sv", 
                                                             107);
    }
    __Vtask_test_copper__DOT__check__29__condition 
        = (6U == (IData)(vlSelfRef.test_copper__DOT__dut__DOT__bg_color));
    vlSelfRef.__Vtask_test_copper__DOT__check__29__name = "bg_color still 6 before trigger"s;
    vlSelfRef.test_copper__DOT__test_num = ((IData)(1U) 
                                            + vlSelfRef.test_copper__DOT__test_num);
    if (__Vtask_test_copper__DOT__check__29__condition) {
        VL_WRITEF_NX("  PASS [%0d] %@\n",0,32,vlSelfRef.test_copper__DOT__test_num,
                     -1,&(vlSelfRef.__Vtask_test_copper__DOT__check__29__name));
        vlSelfRef.test_copper__DOT__pass_count = ((IData)(1U) 
                                                  + vlSelfRef.test_copper__DOT__pass_count);
    } else {
        VL_WRITEF_NX("  FAIL [%0d] %@\n",0,32,vlSelfRef.test_copper__DOT__test_num,
                     -1,&(vlSelfRef.__Vtask_test_copper__DOT__check__29__name));
        vlSelfRef.test_copper__DOT__fail_count = ((IData)(1U) 
                                                  + vlSelfRef.test_copper__DOT__fail_count);
    }
    __Vtask_test_copper__DOT__run_to_scanline__30__target = 0x0000008dU;
    while ((((IData)(vlSelfRef.test_copper__DOT__dut__DOT__v_count) 
             != __Vtask_test_copper__DOT__run_to_scanline__30__target) 
            | (0U != (IData)(vlSelfRef.test_copper__DOT__dut__DOT__h_count)))) {
        Vtest_copper___024root____VbeforeTrig_hbd4d80cf__0(vlSelf, 
                                                           "@(posedge test_copper.clk)");
        co_await vlSelfRef.__VtrigSched_hbd4d80cf__0.trigger(0U, 
                                                             nullptr, 
                                                             "@(posedge test_copper.clk)", 
                                                             "test_copper.sv", 
                                                             107);
    }
    __Vtask_test_copper__DOT__check__31__condition 
        = (2U == (IData)(vlSelfRef.test_copper__DOT__dut__DOT__bg_color));
    vlSelfRef.__Vtask_test_copper__DOT__check__31__name = "bg_color changed to 2 after trigger"s;
    vlSelfRef.test_copper__DOT__test_num = ((IData)(1U) 
                                            + vlSelfRef.test_copper__DOT__test_num);
    if (__Vtask_test_copper__DOT__check__31__condition) {
        VL_WRITEF_NX("  PASS [%0d] %@\n",0,32,vlSelfRef.test_copper__DOT__test_num,
                     -1,&(vlSelfRef.__Vtask_test_copper__DOT__check__31__name));
        vlSelfRef.test_copper__DOT__pass_count = ((IData)(1U) 
                                                  + vlSelfRef.test_copper__DOT__pass_count);
    } else {
        VL_WRITEF_NX("  FAIL [%0d] %@\n",0,32,vlSelfRef.test_copper__DOT__test_num,
                     -1,&(vlSelfRef.__Vtask_test_copper__DOT__check__31__name));
        vlSelfRef.test_copper__DOT__fail_count = ((IData)(1U) 
                                                  + vlSelfRef.test_copper__DOT__fail_count);
    }
    VL_WRITEF_NX("Test: Copper disable\n",0);
    __Vtask_test_copper__DOT__write_reg__32__data = 6U;
    __Vtask_test_copper__DOT__write_reg__32__addr = 0xa001U;
    Vtest_copper___024root____VbeforeTrig_hbd4d80cf__0(vlSelf, 
                                                       "@(posedge test_copper.clk)");
    co_await vlSelfRef.__VtrigSched_hbd4d80cf__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge test_copper.clk)", 
                                                         "test_copper.sv", 
                                                         55);
    vlSelfRef.test_copper__DOT__cpu_addr = __Vtask_test_copper__DOT__write_reg__32__addr;
    vlSelfRef.test_copper__DOT__cpu_wdata = __Vtask_test_copper__DOT__write_reg__32__data;
    vlSelfRef.test_copper__DOT__cpu_we = 1U;
    Vtest_copper___024root____VbeforeTrig_hbd4d80cf__0(vlSelf, 
                                                       "@(posedge test_copper.clk)");
    co_await vlSelfRef.__VtrigSched_hbd4d80cf__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge test_copper.clk)", 
                                                         "test_copper.sv", 
                                                         60);
    vlSelfRef.test_copper__DOT__cpu_we = 0U;
    __Vtask_test_copper__DOT__run_clocks__33__n = 2U;
    __Vtask_test_copper__DOT__run_clocks__33__test_copper__DOT__unnamedblk1_1__DOT____Vrepeat0 = 0;
    __Vtask_test_copper__DOT__run_clocks__33__test_copper__DOT__unnamedblk1_1__DOT____Vrepeat0 
        = __Vtask_test_copper__DOT__run_clocks__33__n;
    while (VL_LTS_III(32, 0U, __Vtask_test_copper__DOT__run_clocks__33__test_copper__DOT__unnamedblk1_1__DOT____Vrepeat0)) {
        Vtest_copper___024root____VbeforeTrig_hbd4d80cf__0(vlSelf, 
                                                           "@(posedge test_copper.clk)");
        co_await vlSelfRef.__VtrigSched_hbd4d80cf__0.trigger(0U, 
                                                             nullptr, 
                                                             "@(posedge test_copper.clk)", 
                                                             "test_copper.sv", 
                                                             101);
        __Vtask_test_copper__DOT__run_clocks__33__test_copper__DOT__unnamedblk1_1__DOT____Vrepeat0 
            = (__Vtask_test_copper__DOT__run_clocks__33__test_copper__DOT__unnamedblk1_1__DOT____Vrepeat0 
               - (IData)(1U));
    }
    __Vtask_test_copper__DOT__check__34__condition 
        = (6U == (IData)(vlSelfRef.test_copper__DOT__dut__DOT__bg_color));
    vlSelfRef.__Vtask_test_copper__DOT__check__34__name = "bg_color reset to 6"s;
    vlSelfRef.test_copper__DOT__test_num = ((IData)(1U) 
                                            + vlSelfRef.test_copper__DOT__test_num);
    if (__Vtask_test_copper__DOT__check__34__condition) {
        VL_WRITEF_NX("  PASS [%0d] %@\n",0,32,vlSelfRef.test_copper__DOT__test_num,
                     -1,&(vlSelfRef.__Vtask_test_copper__DOT__check__34__name));
        vlSelfRef.test_copper__DOT__pass_count = ((IData)(1U) 
                                                  + vlSelfRef.test_copper__DOT__pass_count);
    } else {
        VL_WRITEF_NX("  FAIL [%0d] %@\n",0,32,vlSelfRef.test_copper__DOT__test_num,
                     -1,&(vlSelfRef.__Vtask_test_copper__DOT__check__34__name));
        vlSelfRef.test_copper__DOT__fail_count = ((IData)(1U) 
                                                  + vlSelfRef.test_copper__DOT__fail_count);
    }
    __Vtask_test_copper__DOT__write_cmd__36__cmd = 0x1eU;
    __Vtask_test_copper__DOT__write_reg__37__data = __Vtask_test_copper__DOT__write_cmd__36__cmd;
    __Vtask_test_copper__DOT__write_reg__37__addr = 0xa010U;
    Vtest_copper___024root____VbeforeTrig_hbd4d80cf__0(vlSelf, 
                                                       "@(posedge test_copper.clk)");
    co_await vlSelfRef.__VtrigSched_hbd4d80cf__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge test_copper.clk)", 
                                                         "test_copper.sv", 
                                                         55);
    vlSelfRef.test_copper__DOT__cpu_addr = __Vtask_test_copper__DOT__write_reg__37__addr;
    vlSelfRef.test_copper__DOT__cpu_wdata = __Vtask_test_copper__DOT__write_reg__37__data;
    vlSelfRef.test_copper__DOT__cpu_we = 1U;
    Vtest_copper___024root____VbeforeTrig_hbd4d80cf__0(vlSelf, 
                                                       "@(posedge test_copper.clk)");
    co_await vlSelfRef.__VtrigSched_hbd4d80cf__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge test_copper.clk)", 
                                                         "test_copper.sv", 
                                                         60);
    vlSelfRef.test_copper__DOT__cpu_we = 0U;
    __Vtask_test_copper__DOT__run_clocks__38__n = 2U;
    __Vtask_test_copper__DOT__run_clocks__38__test_copper__DOT__unnamedblk1_1__DOT____Vrepeat0 = 0;
    __Vtask_test_copper__DOT__run_clocks__38__test_copper__DOT__unnamedblk1_1__DOT____Vrepeat0 
        = __Vtask_test_copper__DOT__run_clocks__38__n;
    while (VL_LTS_III(32, 0U, __Vtask_test_copper__DOT__run_clocks__38__test_copper__DOT__unnamedblk1_1__DOT____Vrepeat0)) {
        Vtest_copper___024root____VbeforeTrig_hbd4d80cf__0(vlSelf, 
                                                           "@(posedge test_copper.clk)");
        co_await vlSelfRef.__VtrigSched_hbd4d80cf__0.trigger(0U, 
                                                             nullptr, 
                                                             "@(posedge test_copper.clk)", 
                                                             "test_copper.sv", 
                                                             101);
        __Vtask_test_copper__DOT__run_clocks__38__test_copper__DOT__unnamedblk1_1__DOT____Vrepeat0 
            = (__Vtask_test_copper__DOT__run_clocks__38__test_copper__DOT__unnamedblk1_1__DOT____Vrepeat0 
               - (IData)(1U));
    }
    __Vtask_test_copper__DOT__run_clocks__40__n = 0x000668a0U;
    __Vtask_test_copper__DOT__run_clocks__40__test_copper__DOT__unnamedblk1_1__DOT____Vrepeat0 = 0;
    __Vtask_test_copper__DOT__run_clocks__40__test_copper__DOT__unnamedblk1_1__DOT____Vrepeat0 
        = __Vtask_test_copper__DOT__run_clocks__40__n;
    while (VL_LTS_III(32, 0U, __Vtask_test_copper__DOT__run_clocks__40__test_copper__DOT__unnamedblk1_1__DOT____Vrepeat0)) {
        Vtest_copper___024root____VbeforeTrig_hbd4d80cf__0(vlSelf, 
                                                           "@(posedge test_copper.clk)");
        co_await vlSelfRef.__VtrigSched_hbd4d80cf__0.trigger(0U, 
                                                             nullptr, 
                                                             "@(posedge test_copper.clk)", 
                                                             "test_copper.sv", 
                                                             101);
        __Vtask_test_copper__DOT__run_clocks__40__test_copper__DOT__unnamedblk1_1__DOT____Vrepeat0 
            = (__Vtask_test_copper__DOT__run_clocks__40__test_copper__DOT__unnamedblk1_1__DOT____Vrepeat0 
               - (IData)(1U));
    }
    __Vtask_test_copper__DOT__run_to_scanline__41__target = 0x0000008dU;
    while ((((IData)(vlSelfRef.test_copper__DOT__dut__DOT__v_count) 
             != __Vtask_test_copper__DOT__run_to_scanline__41__target) 
            | (0U != (IData)(vlSelfRef.test_copper__DOT__dut__DOT__h_count)))) {
        Vtest_copper___024root____VbeforeTrig_hbd4d80cf__0(vlSelf, 
                                                           "@(posedge test_copper.clk)");
        co_await vlSelfRef.__VtrigSched_hbd4d80cf__0.trigger(0U, 
                                                             nullptr, 
                                                             "@(posedge test_copper.clk)", 
                                                             "test_copper.sv", 
                                                             107);
    }
    __Vtask_test_copper__DOT__check__42__condition 
        = (6U == (IData)(vlSelfRef.test_copper__DOT__dut__DOT__bg_color));
    vlSelfRef.__Vtask_test_copper__DOT__check__42__name = "bg_color stays 6 when copper disabled"s;
    vlSelfRef.test_copper__DOT__test_num = ((IData)(1U) 
                                            + vlSelfRef.test_copper__DOT__test_num);
    if (__Vtask_test_copper__DOT__check__42__condition) {
        VL_WRITEF_NX("  PASS [%0d] %@\n",0,32,vlSelfRef.test_copper__DOT__test_num,
                     -1,&(vlSelfRef.__Vtask_test_copper__DOT__check__42__name));
        vlSelfRef.test_copper__DOT__pass_count = ((IData)(1U) 
                                                  + vlSelfRef.test_copper__DOT__pass_count);
    } else {
        VL_WRITEF_NX("  FAIL [%0d] %@\n",0,32,vlSelfRef.test_copper__DOT__test_num,
                     -1,&(vlSelfRef.__Vtask_test_copper__DOT__check__42__name));
        vlSelfRef.test_copper__DOT__fail_count = ((IData)(1U) 
                                                  + vlSelfRef.test_copper__DOT__fail_count);
    }
    VL_WRITEF_NX("Test: Copper clear\n",0);
    __Vtask_test_copper__DOT__write_cmd__44__cmd = 0x1cU;
    __Vtask_test_copper__DOT__write_reg__45__data = __Vtask_test_copper__DOT__write_cmd__44__cmd;
    __Vtask_test_copper__DOT__write_reg__45__addr = 0xa010U;
    Vtest_copper___024root____VbeforeTrig_hbd4d80cf__0(vlSelf, 
                                                       "@(posedge test_copper.clk)");
    co_await vlSelfRef.__VtrigSched_hbd4d80cf__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge test_copper.clk)", 
                                                         "test_copper.sv", 
                                                         55);
    vlSelfRef.test_copper__DOT__cpu_addr = __Vtask_test_copper__DOT__write_reg__45__addr;
    vlSelfRef.test_copper__DOT__cpu_wdata = __Vtask_test_copper__DOT__write_reg__45__data;
    vlSelfRef.test_copper__DOT__cpu_we = 1U;
    Vtest_copper___024root____VbeforeTrig_hbd4d80cf__0(vlSelf, 
                                                       "@(posedge test_copper.clk)");
    co_await vlSelfRef.__VtrigSched_hbd4d80cf__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge test_copper.clk)", 
                                                         "test_copper.sv", 
                                                         60);
    vlSelfRef.test_copper__DOT__cpu_we = 0U;
    __Vtask_test_copper__DOT__write_reg__46__data = 6U;
    __Vtask_test_copper__DOT__write_reg__46__addr = 0xa001U;
    Vtest_copper___024root____VbeforeTrig_hbd4d80cf__0(vlSelf, 
                                                       "@(posedge test_copper.clk)");
    co_await vlSelfRef.__VtrigSched_hbd4d80cf__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge test_copper.clk)", 
                                                         "test_copper.sv", 
                                                         55);
    vlSelfRef.test_copper__DOT__cpu_addr = __Vtask_test_copper__DOT__write_reg__46__addr;
    vlSelfRef.test_copper__DOT__cpu_wdata = __Vtask_test_copper__DOT__write_reg__46__data;
    vlSelfRef.test_copper__DOT__cpu_we = 1U;
    Vtest_copper___024root____VbeforeTrig_hbd4d80cf__0(vlSelf, 
                                                       "@(posedge test_copper.clk)");
    co_await vlSelfRef.__VtrigSched_hbd4d80cf__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge test_copper.clk)", 
                                                         "test_copper.sv", 
                                                         60);
    vlSelfRef.test_copper__DOT__cpu_we = 0U;
    __Vtask_test_copper__DOT__run_clocks__47__n = 2U;
    __Vtask_test_copper__DOT__run_clocks__47__test_copper__DOT__unnamedblk1_1__DOT____Vrepeat0 = 0;
    __Vtask_test_copper__DOT__run_clocks__47__test_copper__DOT__unnamedblk1_1__DOT____Vrepeat0 
        = __Vtask_test_copper__DOT__run_clocks__47__n;
    while (VL_LTS_III(32, 0U, __Vtask_test_copper__DOT__run_clocks__47__test_copper__DOT__unnamedblk1_1__DOT____Vrepeat0)) {
        Vtest_copper___024root____VbeforeTrig_hbd4d80cf__0(vlSelf, 
                                                           "@(posedge test_copper.clk)");
        co_await vlSelfRef.__VtrigSched_hbd4d80cf__0.trigger(0U, 
                                                             nullptr, 
                                                             "@(posedge test_copper.clk)", 
                                                             "test_copper.sv", 
                                                             101);
        __Vtask_test_copper__DOT__run_clocks__47__test_copper__DOT__unnamedblk1_1__DOT____Vrepeat0 
            = (__Vtask_test_copper__DOT__run_clocks__47__test_copper__DOT__unnamedblk1_1__DOT____Vrepeat0 
               - (IData)(1U));
    }
    __Vtask_test_copper__DOT__write_cmd__49__cmd = 0x1dU;
    __Vtask_test_copper__DOT__write_reg__50__data = __Vtask_test_copper__DOT__write_cmd__49__cmd;
    __Vtask_test_copper__DOT__write_reg__50__addr = 0xa010U;
    Vtest_copper___024root____VbeforeTrig_hbd4d80cf__0(vlSelf, 
                                                       "@(posedge test_copper.clk)");
    co_await vlSelfRef.__VtrigSched_hbd4d80cf__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge test_copper.clk)", 
                                                         "test_copper.sv", 
                                                         55);
    vlSelfRef.test_copper__DOT__cpu_addr = __Vtask_test_copper__DOT__write_reg__50__addr;
    vlSelfRef.test_copper__DOT__cpu_wdata = __Vtask_test_copper__DOT__write_reg__50__data;
    vlSelfRef.test_copper__DOT__cpu_we = 1U;
    Vtest_copper___024root____VbeforeTrig_hbd4d80cf__0(vlSelf, 
                                                       "@(posedge test_copper.clk)");
    co_await vlSelfRef.__VtrigSched_hbd4d80cf__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge test_copper.clk)", 
                                                         "test_copper.sv", 
                                                         60);
    vlSelfRef.test_copper__DOT__cpu_we = 0U;
    __Vtask_test_copper__DOT__run_clocks__52__n = 0x000668a0U;
    __Vtask_test_copper__DOT__run_clocks__52__test_copper__DOT__unnamedblk1_1__DOT____Vrepeat0 = 0;
    __Vtask_test_copper__DOT__run_clocks__52__test_copper__DOT__unnamedblk1_1__DOT____Vrepeat0 
        = __Vtask_test_copper__DOT__run_clocks__52__n;
    while (VL_LTS_III(32, 0U, __Vtask_test_copper__DOT__run_clocks__52__test_copper__DOT__unnamedblk1_1__DOT____Vrepeat0)) {
        Vtest_copper___024root____VbeforeTrig_hbd4d80cf__0(vlSelf, 
                                                           "@(posedge test_copper.clk)");
        co_await vlSelfRef.__VtrigSched_hbd4d80cf__0.trigger(0U, 
                                                             nullptr, 
                                                             "@(posedge test_copper.clk)", 
                                                             "test_copper.sv", 
                                                             101);
        __Vtask_test_copper__DOT__run_clocks__52__test_copper__DOT__unnamedblk1_1__DOT____Vrepeat0 
            = (__Vtask_test_copper__DOT__run_clocks__52__test_copper__DOT__unnamedblk1_1__DOT____Vrepeat0 
               - (IData)(1U));
    }
    __Vtask_test_copper__DOT__run_to_scanline__53__target = 0x0000008dU;
    while ((((IData)(vlSelfRef.test_copper__DOT__dut__DOT__v_count) 
             != __Vtask_test_copper__DOT__run_to_scanline__53__target) 
            | (0U != (IData)(vlSelfRef.test_copper__DOT__dut__DOT__h_count)))) {
        Vtest_copper___024root____VbeforeTrig_hbd4d80cf__0(vlSelf, 
                                                           "@(posedge test_copper.clk)");
        co_await vlSelfRef.__VtrigSched_hbd4d80cf__0.trigger(0U, 
                                                             nullptr, 
                                                             "@(posedge test_copper.clk)", 
                                                             "test_copper.sv", 
                                                             107);
    }
    __Vtask_test_copper__DOT__check__54__condition 
        = (6U == (IData)(vlSelfRef.test_copper__DOT__dut__DOT__bg_color));
    vlSelfRef.__Vtask_test_copper__DOT__check__54__name = "bg_color stays 6 after copper clear"s;
    vlSelfRef.test_copper__DOT__test_num = ((IData)(1U) 
                                            + vlSelfRef.test_copper__DOT__test_num);
    if (__Vtask_test_copper__DOT__check__54__condition) {
        VL_WRITEF_NX("  PASS [%0d] %@\n",0,32,vlSelfRef.test_copper__DOT__test_num,
                     -1,&(vlSelfRef.__Vtask_test_copper__DOT__check__54__name));
        vlSelfRef.test_copper__DOT__pass_count = ((IData)(1U) 
                                                  + vlSelfRef.test_copper__DOT__pass_count);
    } else {
        VL_WRITEF_NX("  FAIL [%0d] %@\n",0,32,vlSelfRef.test_copper__DOT__test_num,
                     -1,&(vlSelfRef.__Vtask_test_copper__DOT__check__54__name));
        vlSelfRef.test_copper__DOT__fail_count = ((IData)(1U) 
                                                  + vlSelfRef.test_copper__DOT__fail_count);
    }
    VL_WRITEF_NX("Test: Multiple entries\n",0);
    __Vtask_test_copper__DOT__copper_add__55__value = 5U;
    __Vtask_test_copper__DOT__copper_add__55__reg_idx = 1U;
    __Vtask_test_copper__DOT__copper_add__55__y = 0x0000000aU;
    __Vtask_test_copper__DOT__copper_add__55__x = 0U;
    __Vtask_test_copper__DOT__write_param__56__data 
        = (0x000000ffU & __Vtask_test_copper__DOT__copper_add__55__x);
    __Vtask_test_copper__DOT__write_param__56__idx = 0U;
    __Vtask_test_copper__DOT__write_reg__57__data = __Vtask_test_copper__DOT__write_param__56__data;
    __Vtask_test_copper__DOT__write_reg__57__addr = 
        (0x0000ffffU & ((IData)(0xa011U) + __Vtask_test_copper__DOT__write_param__56__idx));
    Vtest_copper___024root____VbeforeTrig_hbd4d80cf__0(vlSelf, 
                                                       "@(posedge test_copper.clk)");
    co_await vlSelfRef.__VtrigSched_hbd4d80cf__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge test_copper.clk)", 
                                                         "test_copper.sv", 
                                                         55);
    vlSelfRef.test_copper__DOT__cpu_addr = __Vtask_test_copper__DOT__write_reg__57__addr;
    vlSelfRef.test_copper__DOT__cpu_wdata = __Vtask_test_copper__DOT__write_reg__57__data;
    vlSelfRef.test_copper__DOT__cpu_we = 1U;
    Vtest_copper___024root____VbeforeTrig_hbd4d80cf__0(vlSelf, 
                                                       "@(posedge test_copper.clk)");
    co_await vlSelfRef.__VtrigSched_hbd4d80cf__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge test_copper.clk)", 
                                                         "test_copper.sv", 
                                                         60);
    vlSelfRef.test_copper__DOT__cpu_we = 0U;
    __Vtask_test_copper__DOT__write_param__58__data 
        = (0x000000ffU & (__Vtask_test_copper__DOT__copper_add__55__x 
                          >> 8U));
    __Vtask_test_copper__DOT__write_param__58__idx = 1U;
    __Vtask_test_copper__DOT__write_reg__59__data = __Vtask_test_copper__DOT__write_param__58__data;
    __Vtask_test_copper__DOT__write_reg__59__addr = 
        (0x0000ffffU & ((IData)(0xa011U) + __Vtask_test_copper__DOT__write_param__58__idx));
    Vtest_copper___024root____VbeforeTrig_hbd4d80cf__0(vlSelf, 
                                                       "@(posedge test_copper.clk)");
    co_await vlSelfRef.__VtrigSched_hbd4d80cf__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge test_copper.clk)", 
                                                         "test_copper.sv", 
                                                         55);
    vlSelfRef.test_copper__DOT__cpu_addr = __Vtask_test_copper__DOT__write_reg__59__addr;
    vlSelfRef.test_copper__DOT__cpu_wdata = __Vtask_test_copper__DOT__write_reg__59__data;
    vlSelfRef.test_copper__DOT__cpu_we = 1U;
    Vtest_copper___024root____VbeforeTrig_hbd4d80cf__0(vlSelf, 
                                                       "@(posedge test_copper.clk)");
    co_await vlSelfRef.__VtrigSched_hbd4d80cf__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge test_copper.clk)", 
                                                         "test_copper.sv", 
                                                         60);
    vlSelfRef.test_copper__DOT__cpu_we = 0U;
    __Vtask_test_copper__DOT__write_param__60__data 
        = (0x000000ffU & __Vtask_test_copper__DOT__copper_add__55__y);
    __Vtask_test_copper__DOT__write_param__60__idx = 2U;
    __Vtask_test_copper__DOT__write_reg__61__data = __Vtask_test_copper__DOT__write_param__60__data;
    __Vtask_test_copper__DOT__write_reg__61__addr = 
        (0x0000ffffU & ((IData)(0xa011U) + __Vtask_test_copper__DOT__write_param__60__idx));
    Vtest_copper___024root____VbeforeTrig_hbd4d80cf__0(vlSelf, 
                                                       "@(posedge test_copper.clk)");
    co_await vlSelfRef.__VtrigSched_hbd4d80cf__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge test_copper.clk)", 
                                                         "test_copper.sv", 
                                                         55);
    vlSelfRef.test_copper__DOT__cpu_addr = __Vtask_test_copper__DOT__write_reg__61__addr;
    vlSelfRef.test_copper__DOT__cpu_wdata = __Vtask_test_copper__DOT__write_reg__61__data;
    vlSelfRef.test_copper__DOT__cpu_we = 1U;
    Vtest_copper___024root____VbeforeTrig_hbd4d80cf__0(vlSelf, 
                                                       "@(posedge test_copper.clk)");
    co_await vlSelfRef.__VtrigSched_hbd4d80cf__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge test_copper.clk)", 
                                                         "test_copper.sv", 
                                                         60);
    vlSelfRef.test_copper__DOT__cpu_we = 0U;
    __Vtask_test_copper__DOT__write_param__62__data 
        = (0x000000ffU & __Vtask_test_copper__DOT__copper_add__55__reg_idx);
    __Vtask_test_copper__DOT__write_param__62__idx = 3U;
    __Vtask_test_copper__DOT__write_reg__63__data = __Vtask_test_copper__DOT__write_param__62__data;
    __Vtask_test_copper__DOT__write_reg__63__addr = 
        (0x0000ffffU & ((IData)(0xa011U) + __Vtask_test_copper__DOT__write_param__62__idx));
    Vtest_copper___024root____VbeforeTrig_hbd4d80cf__0(vlSelf, 
                                                       "@(posedge test_copper.clk)");
    co_await vlSelfRef.__VtrigSched_hbd4d80cf__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge test_copper.clk)", 
                                                         "test_copper.sv", 
                                                         55);
    vlSelfRef.test_copper__DOT__cpu_addr = __Vtask_test_copper__DOT__write_reg__63__addr;
    vlSelfRef.test_copper__DOT__cpu_wdata = __Vtask_test_copper__DOT__write_reg__63__data;
    vlSelfRef.test_copper__DOT__cpu_we = 1U;
    Vtest_copper___024root____VbeforeTrig_hbd4d80cf__0(vlSelf, 
                                                       "@(posedge test_copper.clk)");
    co_await vlSelfRef.__VtrigSched_hbd4d80cf__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge test_copper.clk)", 
                                                         "test_copper.sv", 
                                                         60);
    vlSelfRef.test_copper__DOT__cpu_we = 0U;
    __Vtask_test_copper__DOT__write_param__64__data 
        = (0x000000ffU & (__Vtask_test_copper__DOT__copper_add__55__reg_idx 
                          >> 8U));
    __Vtask_test_copper__DOT__write_param__64__idx = 4U;
    __Vtask_test_copper__DOT__write_reg__65__data = __Vtask_test_copper__DOT__write_param__64__data;
    __Vtask_test_copper__DOT__write_reg__65__addr = 
        (0x0000ffffU & ((IData)(0xa011U) + __Vtask_test_copper__DOT__write_param__64__idx));
    Vtest_copper___024root____VbeforeTrig_hbd4d80cf__0(vlSelf, 
                                                       "@(posedge test_copper.clk)");
    co_await vlSelfRef.__VtrigSched_hbd4d80cf__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge test_copper.clk)", 
                                                         "test_copper.sv", 
                                                         55);
    vlSelfRef.test_copper__DOT__cpu_addr = __Vtask_test_copper__DOT__write_reg__65__addr;
    vlSelfRef.test_copper__DOT__cpu_wdata = __Vtask_test_copper__DOT__write_reg__65__data;
    vlSelfRef.test_copper__DOT__cpu_we = 1U;
    Vtest_copper___024root____VbeforeTrig_hbd4d80cf__0(vlSelf, 
                                                       "@(posedge test_copper.clk)");
    co_await vlSelfRef.__VtrigSched_hbd4d80cf__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge test_copper.clk)", 
                                                         "test_copper.sv", 
                                                         60);
    vlSelfRef.test_copper__DOT__cpu_we = 0U;
    __Vtask_test_copper__DOT__write_param__66__data 
        = __Vtask_test_copper__DOT__copper_add__55__value;
    __Vtask_test_copper__DOT__write_param__66__idx = 5U;
    __Vtask_test_copper__DOT__write_reg__67__data = __Vtask_test_copper__DOT__write_param__66__data;
    __Vtask_test_copper__DOT__write_reg__67__addr = 
        (0x0000ffffU & ((IData)(0xa011U) + __Vtask_test_copper__DOT__write_param__66__idx));
    Vtest_copper___024root____VbeforeTrig_hbd4d80cf__0(vlSelf, 
                                                       "@(posedge test_copper.clk)");
    co_await vlSelfRef.__VtrigSched_hbd4d80cf__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge test_copper.clk)", 
                                                         "test_copper.sv", 
                                                         55);
    vlSelfRef.test_copper__DOT__cpu_addr = __Vtask_test_copper__DOT__write_reg__67__addr;
    vlSelfRef.test_copper__DOT__cpu_wdata = __Vtask_test_copper__DOT__write_reg__67__data;
    vlSelfRef.test_copper__DOT__cpu_we = 1U;
    Vtest_copper___024root____VbeforeTrig_hbd4d80cf__0(vlSelf, 
                                                       "@(posedge test_copper.clk)");
    co_await vlSelfRef.__VtrigSched_hbd4d80cf__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge test_copper.clk)", 
                                                         "test_copper.sv", 
                                                         60);
    vlSelfRef.test_copper__DOT__cpu_we = 0U;
    __Vtask_test_copper__DOT__write_cmd__68__cmd = 0x1bU;
    __Vtask_test_copper__DOT__write_reg__69__data = __Vtask_test_copper__DOT__write_cmd__68__cmd;
    __Vtask_test_copper__DOT__write_reg__69__addr = 0xa010U;
    Vtest_copper___024root____VbeforeTrig_hbd4d80cf__0(vlSelf, 
                                                       "@(posedge test_copper.clk)");
    co_await vlSelfRef.__VtrigSched_hbd4d80cf__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge test_copper.clk)", 
                                                         "test_copper.sv", 
                                                         55);
    vlSelfRef.test_copper__DOT__cpu_addr = __Vtask_test_copper__DOT__write_reg__69__addr;
    vlSelfRef.test_copper__DOT__cpu_wdata = __Vtask_test_copper__DOT__write_reg__69__data;
    vlSelfRef.test_copper__DOT__cpu_we = 1U;
    Vtest_copper___024root____VbeforeTrig_hbd4d80cf__0(vlSelf, 
                                                       "@(posedge test_copper.clk)");
    co_await vlSelfRef.__VtrigSched_hbd4d80cf__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge test_copper.clk)", 
                                                         "test_copper.sv", 
                                                         60);
    vlSelfRef.test_copper__DOT__cpu_we = 0U;
    __Vtask_test_copper__DOT__copper_add__70__value = 2U;
    __Vtask_test_copper__DOT__copper_add__70__reg_idx = 1U;
    __Vtask_test_copper__DOT__copper_add__70__y = 0x00000064U;
    __Vtask_test_copper__DOT__copper_add__70__x = 0U;
    __Vtask_test_copper__DOT__write_param__71__data 
        = (0x000000ffU & __Vtask_test_copper__DOT__copper_add__70__x);
    __Vtask_test_copper__DOT__write_param__71__idx = 0U;
    __Vtask_test_copper__DOT__write_reg__72__data = __Vtask_test_copper__DOT__write_param__71__data;
    __Vtask_test_copper__DOT__write_reg__72__addr = 
        (0x0000ffffU & ((IData)(0xa011U) + __Vtask_test_copper__DOT__write_param__71__idx));
    Vtest_copper___024root____VbeforeTrig_hbd4d80cf__0(vlSelf, 
                                                       "@(posedge test_copper.clk)");
    co_await vlSelfRef.__VtrigSched_hbd4d80cf__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge test_copper.clk)", 
                                                         "test_copper.sv", 
                                                         55);
    vlSelfRef.test_copper__DOT__cpu_addr = __Vtask_test_copper__DOT__write_reg__72__addr;
    vlSelfRef.test_copper__DOT__cpu_wdata = __Vtask_test_copper__DOT__write_reg__72__data;
    vlSelfRef.test_copper__DOT__cpu_we = 1U;
    Vtest_copper___024root____VbeforeTrig_hbd4d80cf__0(vlSelf, 
                                                       "@(posedge test_copper.clk)");
    co_await vlSelfRef.__VtrigSched_hbd4d80cf__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge test_copper.clk)", 
                                                         "test_copper.sv", 
                                                         60);
    vlSelfRef.test_copper__DOT__cpu_we = 0U;
    __Vtask_test_copper__DOT__write_param__73__data 
        = (0x000000ffU & (__Vtask_test_copper__DOT__copper_add__70__x 
                          >> 8U));
    __Vtask_test_copper__DOT__write_param__73__idx = 1U;
    __Vtask_test_copper__DOT__write_reg__74__data = __Vtask_test_copper__DOT__write_param__73__data;
    __Vtask_test_copper__DOT__write_reg__74__addr = 
        (0x0000ffffU & ((IData)(0xa011U) + __Vtask_test_copper__DOT__write_param__73__idx));
    Vtest_copper___024root____VbeforeTrig_hbd4d80cf__0(vlSelf, 
                                                       "@(posedge test_copper.clk)");
    co_await vlSelfRef.__VtrigSched_hbd4d80cf__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge test_copper.clk)", 
                                                         "test_copper.sv", 
                                                         55);
    vlSelfRef.test_copper__DOT__cpu_addr = __Vtask_test_copper__DOT__write_reg__74__addr;
    vlSelfRef.test_copper__DOT__cpu_wdata = __Vtask_test_copper__DOT__write_reg__74__data;
    vlSelfRef.test_copper__DOT__cpu_we = 1U;
    Vtest_copper___024root____VbeforeTrig_hbd4d80cf__0(vlSelf, 
                                                       "@(posedge test_copper.clk)");
    co_await vlSelfRef.__VtrigSched_hbd4d80cf__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge test_copper.clk)", 
                                                         "test_copper.sv", 
                                                         60);
    vlSelfRef.test_copper__DOT__cpu_we = 0U;
    __Vtask_test_copper__DOT__write_param__75__data 
        = (0x000000ffU & __Vtask_test_copper__DOT__copper_add__70__y);
    __Vtask_test_copper__DOT__write_param__75__idx = 2U;
    __Vtask_test_copper__DOT__write_reg__76__data = __Vtask_test_copper__DOT__write_param__75__data;
    __Vtask_test_copper__DOT__write_reg__76__addr = 
        (0x0000ffffU & ((IData)(0xa011U) + __Vtask_test_copper__DOT__write_param__75__idx));
    Vtest_copper___024root____VbeforeTrig_hbd4d80cf__0(vlSelf, 
                                                       "@(posedge test_copper.clk)");
    co_await vlSelfRef.__VtrigSched_hbd4d80cf__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge test_copper.clk)", 
                                                         "test_copper.sv", 
                                                         55);
    vlSelfRef.test_copper__DOT__cpu_addr = __Vtask_test_copper__DOT__write_reg__76__addr;
    vlSelfRef.test_copper__DOT__cpu_wdata = __Vtask_test_copper__DOT__write_reg__76__data;
    vlSelfRef.test_copper__DOT__cpu_we = 1U;
    Vtest_copper___024root____VbeforeTrig_hbd4d80cf__0(vlSelf, 
                                                       "@(posedge test_copper.clk)");
    co_await vlSelfRef.__VtrigSched_hbd4d80cf__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge test_copper.clk)", 
                                                         "test_copper.sv", 
                                                         60);
    vlSelfRef.test_copper__DOT__cpu_we = 0U;
    __Vtask_test_copper__DOT__write_param__77__data 
        = (0x000000ffU & __Vtask_test_copper__DOT__copper_add__70__reg_idx);
    __Vtask_test_copper__DOT__write_param__77__idx = 3U;
    __Vtask_test_copper__DOT__write_reg__78__data = __Vtask_test_copper__DOT__write_param__77__data;
    __Vtask_test_copper__DOT__write_reg__78__addr = 
        (0x0000ffffU & ((IData)(0xa011U) + __Vtask_test_copper__DOT__write_param__77__idx));
    Vtest_copper___024root____VbeforeTrig_hbd4d80cf__0(vlSelf, 
                                                       "@(posedge test_copper.clk)");
    co_await vlSelfRef.__VtrigSched_hbd4d80cf__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge test_copper.clk)", 
                                                         "test_copper.sv", 
                                                         55);
    vlSelfRef.test_copper__DOT__cpu_addr = __Vtask_test_copper__DOT__write_reg__78__addr;
    vlSelfRef.test_copper__DOT__cpu_wdata = __Vtask_test_copper__DOT__write_reg__78__data;
    vlSelfRef.test_copper__DOT__cpu_we = 1U;
    Vtest_copper___024root____VbeforeTrig_hbd4d80cf__0(vlSelf, 
                                                       "@(posedge test_copper.clk)");
    co_await vlSelfRef.__VtrigSched_hbd4d80cf__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge test_copper.clk)", 
                                                         "test_copper.sv", 
                                                         60);
    vlSelfRef.test_copper__DOT__cpu_we = 0U;
    __Vtask_test_copper__DOT__write_param__79__data 
        = (0x000000ffU & (__Vtask_test_copper__DOT__copper_add__70__reg_idx 
                          >> 8U));
    __Vtask_test_copper__DOT__write_param__79__idx = 4U;
    __Vtask_test_copper__DOT__write_reg__80__data = __Vtask_test_copper__DOT__write_param__79__data;
    __Vtask_test_copper__DOT__write_reg__80__addr = 
        (0x0000ffffU & ((IData)(0xa011U) + __Vtask_test_copper__DOT__write_param__79__idx));
    Vtest_copper___024root____VbeforeTrig_hbd4d80cf__0(vlSelf, 
                                                       "@(posedge test_copper.clk)");
    co_await vlSelfRef.__VtrigSched_hbd4d80cf__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge test_copper.clk)", 
                                                         "test_copper.sv", 
                                                         55);
    vlSelfRef.test_copper__DOT__cpu_addr = __Vtask_test_copper__DOT__write_reg__80__addr;
    vlSelfRef.test_copper__DOT__cpu_wdata = __Vtask_test_copper__DOT__write_reg__80__data;
    vlSelfRef.test_copper__DOT__cpu_we = 1U;
    Vtest_copper___024root____VbeforeTrig_hbd4d80cf__0(vlSelf, 
                                                       "@(posedge test_copper.clk)");
    co_await vlSelfRef.__VtrigSched_hbd4d80cf__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge test_copper.clk)", 
                                                         "test_copper.sv", 
                                                         60);
    vlSelfRef.test_copper__DOT__cpu_we = 0U;
    __Vtask_test_copper__DOT__write_param__81__data 
        = __Vtask_test_copper__DOT__copper_add__70__value;
    __Vtask_test_copper__DOT__write_param__81__idx = 5U;
    __Vtask_test_copper__DOT__write_reg__82__data = __Vtask_test_copper__DOT__write_param__81__data;
    __Vtask_test_copper__DOT__write_reg__82__addr = 
        (0x0000ffffU & ((IData)(0xa011U) + __Vtask_test_copper__DOT__write_param__81__idx));
    Vtest_copper___024root____VbeforeTrig_hbd4d80cf__0(vlSelf, 
                                                       "@(posedge test_copper.clk)");
    co_await vlSelfRef.__VtrigSched_hbd4d80cf__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge test_copper.clk)", 
                                                         "test_copper.sv", 
                                                         55);
    vlSelfRef.test_copper__DOT__cpu_addr = __Vtask_test_copper__DOT__write_reg__82__addr;
    vlSelfRef.test_copper__DOT__cpu_wdata = __Vtask_test_copper__DOT__write_reg__82__data;
    vlSelfRef.test_copper__DOT__cpu_we = 1U;
    Vtest_copper___024root____VbeforeTrig_hbd4d80cf__0(vlSelf, 
                                                       "@(posedge test_copper.clk)");
    co_await vlSelfRef.__VtrigSched_hbd4d80cf__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge test_copper.clk)", 
                                                         "test_copper.sv", 
                                                         60);
    vlSelfRef.test_copper__DOT__cpu_we = 0U;
    __Vtask_test_copper__DOT__write_cmd__83__cmd = 0x1bU;
    __Vtask_test_copper__DOT__write_reg__84__data = __Vtask_test_copper__DOT__write_cmd__83__cmd;
    __Vtask_test_copper__DOT__write_reg__84__addr = 0xa010U;
    Vtest_copper___024root____VbeforeTrig_hbd4d80cf__0(vlSelf, 
                                                       "@(posedge test_copper.clk)");
    co_await vlSelfRef.__VtrigSched_hbd4d80cf__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge test_copper.clk)", 
                                                         "test_copper.sv", 
                                                         55);
    vlSelfRef.test_copper__DOT__cpu_addr = __Vtask_test_copper__DOT__write_reg__84__addr;
    vlSelfRef.test_copper__DOT__cpu_wdata = __Vtask_test_copper__DOT__write_reg__84__data;
    vlSelfRef.test_copper__DOT__cpu_we = 1U;
    Vtest_copper___024root____VbeforeTrig_hbd4d80cf__0(vlSelf, 
                                                       "@(posedge test_copper.clk)");
    co_await vlSelfRef.__VtrigSched_hbd4d80cf__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge test_copper.clk)", 
                                                         "test_copper.sv", 
                                                         60);
    vlSelfRef.test_copper__DOT__cpu_we = 0U;
    __Vtask_test_copper__DOT__copper_add__85__value = 7U;
    __Vtask_test_copper__DOT__copper_add__85__reg_idx = 1U;
    __Vtask_test_copper__DOT__copper_add__85__y = 0x00000096U;
    __Vtask_test_copper__DOT__copper_add__85__x = 0U;
    __Vtask_test_copper__DOT__write_param__86__data 
        = (0x000000ffU & __Vtask_test_copper__DOT__copper_add__85__x);
    __Vtask_test_copper__DOT__write_param__86__idx = 0U;
    __Vtask_test_copper__DOT__write_reg__87__data = __Vtask_test_copper__DOT__write_param__86__data;
    __Vtask_test_copper__DOT__write_reg__87__addr = 
        (0x0000ffffU & ((IData)(0xa011U) + __Vtask_test_copper__DOT__write_param__86__idx));
    Vtest_copper___024root____VbeforeTrig_hbd4d80cf__0(vlSelf, 
                                                       "@(posedge test_copper.clk)");
    co_await vlSelfRef.__VtrigSched_hbd4d80cf__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge test_copper.clk)", 
                                                         "test_copper.sv", 
                                                         55);
    vlSelfRef.test_copper__DOT__cpu_addr = __Vtask_test_copper__DOT__write_reg__87__addr;
    vlSelfRef.test_copper__DOT__cpu_wdata = __Vtask_test_copper__DOT__write_reg__87__data;
    vlSelfRef.test_copper__DOT__cpu_we = 1U;
    Vtest_copper___024root____VbeforeTrig_hbd4d80cf__0(vlSelf, 
                                                       "@(posedge test_copper.clk)");
    co_await vlSelfRef.__VtrigSched_hbd4d80cf__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge test_copper.clk)", 
                                                         "test_copper.sv", 
                                                         60);
    vlSelfRef.test_copper__DOT__cpu_we = 0U;
    __Vtask_test_copper__DOT__write_param__88__data 
        = (0x000000ffU & (__Vtask_test_copper__DOT__copper_add__85__x 
                          >> 8U));
    __Vtask_test_copper__DOT__write_param__88__idx = 1U;
    __Vtask_test_copper__DOT__write_reg__89__data = __Vtask_test_copper__DOT__write_param__88__data;
    __Vtask_test_copper__DOT__write_reg__89__addr = 
        (0x0000ffffU & ((IData)(0xa011U) + __Vtask_test_copper__DOT__write_param__88__idx));
    Vtest_copper___024root____VbeforeTrig_hbd4d80cf__0(vlSelf, 
                                                       "@(posedge test_copper.clk)");
    co_await vlSelfRef.__VtrigSched_hbd4d80cf__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge test_copper.clk)", 
                                                         "test_copper.sv", 
                                                         55);
    vlSelfRef.test_copper__DOT__cpu_addr = __Vtask_test_copper__DOT__write_reg__89__addr;
    vlSelfRef.test_copper__DOT__cpu_wdata = __Vtask_test_copper__DOT__write_reg__89__data;
    vlSelfRef.test_copper__DOT__cpu_we = 1U;
    Vtest_copper___024root____VbeforeTrig_hbd4d80cf__0(vlSelf, 
                                                       "@(posedge test_copper.clk)");
    co_await vlSelfRef.__VtrigSched_hbd4d80cf__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge test_copper.clk)", 
                                                         "test_copper.sv", 
                                                         60);
    vlSelfRef.test_copper__DOT__cpu_we = 0U;
    __Vtask_test_copper__DOT__write_param__90__data 
        = (0x000000ffU & __Vtask_test_copper__DOT__copper_add__85__y);
    __Vtask_test_copper__DOT__write_param__90__idx = 2U;
    __Vtask_test_copper__DOT__write_reg__91__data = __Vtask_test_copper__DOT__write_param__90__data;
    __Vtask_test_copper__DOT__write_reg__91__addr = 
        (0x0000ffffU & ((IData)(0xa011U) + __Vtask_test_copper__DOT__write_param__90__idx));
    Vtest_copper___024root____VbeforeTrig_hbd4d80cf__0(vlSelf, 
                                                       "@(posedge test_copper.clk)");
    co_await vlSelfRef.__VtrigSched_hbd4d80cf__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge test_copper.clk)", 
                                                         "test_copper.sv", 
                                                         55);
    vlSelfRef.test_copper__DOT__cpu_addr = __Vtask_test_copper__DOT__write_reg__91__addr;
    vlSelfRef.test_copper__DOT__cpu_wdata = __Vtask_test_copper__DOT__write_reg__91__data;
    vlSelfRef.test_copper__DOT__cpu_we = 1U;
    Vtest_copper___024root____VbeforeTrig_hbd4d80cf__0(vlSelf, 
                                                       "@(posedge test_copper.clk)");
    co_await vlSelfRef.__VtrigSched_hbd4d80cf__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge test_copper.clk)", 
                                                         "test_copper.sv", 
                                                         60);
    vlSelfRef.test_copper__DOT__cpu_we = 0U;
    __Vtask_test_copper__DOT__write_param__92__data 
        = (0x000000ffU & __Vtask_test_copper__DOT__copper_add__85__reg_idx);
    __Vtask_test_copper__DOT__write_param__92__idx = 3U;
    __Vtask_test_copper__DOT__write_reg__93__data = __Vtask_test_copper__DOT__write_param__92__data;
    __Vtask_test_copper__DOT__write_reg__93__addr = 
        (0x0000ffffU & ((IData)(0xa011U) + __Vtask_test_copper__DOT__write_param__92__idx));
    Vtest_copper___024root____VbeforeTrig_hbd4d80cf__0(vlSelf, 
                                                       "@(posedge test_copper.clk)");
    co_await vlSelfRef.__VtrigSched_hbd4d80cf__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge test_copper.clk)", 
                                                         "test_copper.sv", 
                                                         55);
    vlSelfRef.test_copper__DOT__cpu_addr = __Vtask_test_copper__DOT__write_reg__93__addr;
    vlSelfRef.test_copper__DOT__cpu_wdata = __Vtask_test_copper__DOT__write_reg__93__data;
    vlSelfRef.test_copper__DOT__cpu_we = 1U;
    Vtest_copper___024root____VbeforeTrig_hbd4d80cf__0(vlSelf, 
                                                       "@(posedge test_copper.clk)");
    co_await vlSelfRef.__VtrigSched_hbd4d80cf__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge test_copper.clk)", 
                                                         "test_copper.sv", 
                                                         60);
    vlSelfRef.test_copper__DOT__cpu_we = 0U;
    __Vtask_test_copper__DOT__write_param__94__data 
        = (0x000000ffU & (__Vtask_test_copper__DOT__copper_add__85__reg_idx 
                          >> 8U));
    __Vtask_test_copper__DOT__write_param__94__idx = 4U;
    __Vtask_test_copper__DOT__write_reg__95__data = __Vtask_test_copper__DOT__write_param__94__data;
    __Vtask_test_copper__DOT__write_reg__95__addr = 
        (0x0000ffffU & ((IData)(0xa011U) + __Vtask_test_copper__DOT__write_param__94__idx));
    Vtest_copper___024root____VbeforeTrig_hbd4d80cf__0(vlSelf, 
                                                       "@(posedge test_copper.clk)");
    co_await vlSelfRef.__VtrigSched_hbd4d80cf__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge test_copper.clk)", 
                                                         "test_copper.sv", 
                                                         55);
    vlSelfRef.test_copper__DOT__cpu_addr = __Vtask_test_copper__DOT__write_reg__95__addr;
    vlSelfRef.test_copper__DOT__cpu_wdata = __Vtask_test_copper__DOT__write_reg__95__data;
    vlSelfRef.test_copper__DOT__cpu_we = 1U;
    Vtest_copper___024root____VbeforeTrig_hbd4d80cf__0(vlSelf, 
                                                       "@(posedge test_copper.clk)");
    co_await vlSelfRef.__VtrigSched_hbd4d80cf__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge test_copper.clk)", 
                                                         "test_copper.sv", 
                                                         60);
    vlSelfRef.test_copper__DOT__cpu_we = 0U;
    __Vtask_test_copper__DOT__write_param__96__data 
        = __Vtask_test_copper__DOT__copper_add__85__value;
    __Vtask_test_copper__DOT__write_param__96__idx = 5U;
    __Vtask_test_copper__DOT__write_reg__97__data = __Vtask_test_copper__DOT__write_param__96__data;
    __Vtask_test_copper__DOT__write_reg__97__addr = 
        (0x0000ffffU & ((IData)(0xa011U) + __Vtask_test_copper__DOT__write_param__96__idx));
    Vtest_copper___024root____VbeforeTrig_hbd4d80cf__0(vlSelf, 
                                                       "@(posedge test_copper.clk)");
    co_await vlSelfRef.__VtrigSched_hbd4d80cf__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge test_copper.clk)", 
                                                         "test_copper.sv", 
                                                         55);
    vlSelfRef.test_copper__DOT__cpu_addr = __Vtask_test_copper__DOT__write_reg__97__addr;
    vlSelfRef.test_copper__DOT__cpu_wdata = __Vtask_test_copper__DOT__write_reg__97__data;
    vlSelfRef.test_copper__DOT__cpu_we = 1U;
    Vtest_copper___024root____VbeforeTrig_hbd4d80cf__0(vlSelf, 
                                                       "@(posedge test_copper.clk)");
    co_await vlSelfRef.__VtrigSched_hbd4d80cf__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge test_copper.clk)", 
                                                         "test_copper.sv", 
                                                         60);
    vlSelfRef.test_copper__DOT__cpu_we = 0U;
    __Vtask_test_copper__DOT__write_cmd__98__cmd = 0x1bU;
    __Vtask_test_copper__DOT__write_reg__99__data = __Vtask_test_copper__DOT__write_cmd__98__cmd;
    __Vtask_test_copper__DOT__write_reg__99__addr = 0xa010U;
    Vtest_copper___024root____VbeforeTrig_hbd4d80cf__0(vlSelf, 
                                                       "@(posedge test_copper.clk)");
    co_await vlSelfRef.__VtrigSched_hbd4d80cf__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge test_copper.clk)", 
                                                         "test_copper.sv", 
                                                         55);
    vlSelfRef.test_copper__DOT__cpu_addr = __Vtask_test_copper__DOT__write_reg__99__addr;
    vlSelfRef.test_copper__DOT__cpu_wdata = __Vtask_test_copper__DOT__write_reg__99__data;
    vlSelfRef.test_copper__DOT__cpu_we = 1U;
    Vtest_copper___024root____VbeforeTrig_hbd4d80cf__0(vlSelf, 
                                                       "@(posedge test_copper.clk)");
    co_await vlSelfRef.__VtrigSched_hbd4d80cf__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge test_copper.clk)", 
                                                         "test_copper.sv", 
                                                         60);
    vlSelfRef.test_copper__DOT__cpu_we = 0U;
    __Vtask_test_copper__DOT__write_reg__100__data = 6U;
    __Vtask_test_copper__DOT__write_reg__100__addr = 0xa001U;
    Vtest_copper___024root____VbeforeTrig_hbd4d80cf__0(vlSelf, 
                                                       "@(posedge test_copper.clk)");
    co_await vlSelfRef.__VtrigSched_hbd4d80cf__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge test_copper.clk)", 
                                                         "test_copper.sv", 
                                                         55);
    vlSelfRef.test_copper__DOT__cpu_addr = __Vtask_test_copper__DOT__write_reg__100__addr;
    vlSelfRef.test_copper__DOT__cpu_wdata = __Vtask_test_copper__DOT__write_reg__100__data;
    vlSelfRef.test_copper__DOT__cpu_we = 1U;
    Vtest_copper___024root____VbeforeTrig_hbd4d80cf__0(vlSelf, 
                                                       "@(posedge test_copper.clk)");
    co_await vlSelfRef.__VtrigSched_hbd4d80cf__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge test_copper.clk)", 
                                                         "test_copper.sv", 
                                                         60);
    vlSelfRef.test_copper__DOT__cpu_we = 0U;
    __Vtask_test_copper__DOT__run_clocks__101__n = 2U;
    __Vtask_test_copper__DOT__run_clocks__101__test_copper__DOT__unnamedblk1_1__DOT____Vrepeat0 = 0;
    __Vtask_test_copper__DOT__run_clocks__101__test_copper__DOT__unnamedblk1_1__DOT____Vrepeat0 
        = __Vtask_test_copper__DOT__run_clocks__101__n;
    while (VL_LTS_III(32, 0U, __Vtask_test_copper__DOT__run_clocks__101__test_copper__DOT__unnamedblk1_1__DOT____Vrepeat0)) {
        Vtest_copper___024root____VbeforeTrig_hbd4d80cf__0(vlSelf, 
                                                           "@(posedge test_copper.clk)");
        co_await vlSelfRef.__VtrigSched_hbd4d80cf__0.trigger(0U, 
                                                             nullptr, 
                                                             "@(posedge test_copper.clk)", 
                                                             "test_copper.sv", 
                                                             101);
        __Vtask_test_copper__DOT__run_clocks__101__test_copper__DOT__unnamedblk1_1__DOT____Vrepeat0 
            = (__Vtask_test_copper__DOT__run_clocks__101__test_copper__DOT__unnamedblk1_1__DOT____Vrepeat0 
               - (IData)(1U));
    }
    __Vtask_test_copper__DOT__run_to_scanline__102__target = 0x000001e0U;
    while ((((IData)(vlSelfRef.test_copper__DOT__dut__DOT__v_count) 
             != __Vtask_test_copper__DOT__run_to_scanline__102__target) 
            | (0U != (IData)(vlSelfRef.test_copper__DOT__dut__DOT__h_count)))) {
        Vtest_copper___024root____VbeforeTrig_hbd4d80cf__0(vlSelf, 
                                                           "@(posedge test_copper.clk)");
        co_await vlSelfRef.__VtrigSched_hbd4d80cf__0.trigger(0U, 
                                                             nullptr, 
                                                             "@(posedge test_copper.clk)", 
                                                             "test_copper.sv", 
                                                             107);
    }
    __Vtask_test_copper__DOT__run_to_scanline__103__target = 0U;
    while ((((IData)(vlSelfRef.test_copper__DOT__dut__DOT__v_count) 
             != __Vtask_test_copper__DOT__run_to_scanline__103__target) 
            | (0U != (IData)(vlSelfRef.test_copper__DOT__dut__DOT__h_count)))) {
        Vtest_copper___024root____VbeforeTrig_hbd4d80cf__0(vlSelf, 
                                                           "@(posedge test_copper.clk)");
        co_await vlSelfRef.__VtrigSched_hbd4d80cf__0.trigger(0U, 
                                                             nullptr, 
                                                             "@(posedge test_copper.clk)", 
                                                             "test_copper.sv", 
                                                             107);
    }
    __Vtask_test_copper__DOT__run_to_scanline__104__target = 0x0000003dU;
    while ((((IData)(vlSelfRef.test_copper__DOT__dut__DOT__v_count) 
             != __Vtask_test_copper__DOT__run_to_scanline__104__target) 
            | (0U != (IData)(vlSelfRef.test_copper__DOT__dut__DOT__h_count)))) {
        Vtest_copper___024root____VbeforeTrig_hbd4d80cf__0(vlSelf, 
                                                           "@(posedge test_copper.clk)");
        co_await vlSelfRef.__VtrigSched_hbd4d80cf__0.trigger(0U, 
                                                             nullptr, 
                                                             "@(posedge test_copper.clk)", 
                                                             "test_copper.sv", 
                                                             107);
    }
    __Vtask_test_copper__DOT__check__105__condition 
        = (5U == (IData)(vlSelfRef.test_copper__DOT__dut__DOT__bg_color));
    vlSelfRef.__Vtask_test_copper__DOT__check__105__name = "bg=5 (green) after line 10"s;
    vlSelfRef.test_copper__DOT__test_num = ((IData)(1U) 
                                            + vlSelfRef.test_copper__DOT__test_num);
    if (__Vtask_test_copper__DOT__check__105__condition) {
        VL_WRITEF_NX("  PASS [%0d] %@\n",0,32,vlSelfRef.test_copper__DOT__test_num,
                     -1,&(vlSelfRef.__Vtask_test_copper__DOT__check__105__name));
        vlSelfRef.test_copper__DOT__pass_count = ((IData)(1U) 
                                                  + vlSelfRef.test_copper__DOT__pass_count);
    } else {
        VL_WRITEF_NX("  FAIL [%0d] %@\n",0,32,vlSelfRef.test_copper__DOT__test_num,
                     -1,&(vlSelfRef.__Vtask_test_copper__DOT__check__105__name));
        vlSelfRef.test_copper__DOT__fail_count = ((IData)(1U) 
                                                  + vlSelfRef.test_copper__DOT__fail_count);
    }
    __Vtask_test_copper__DOT__run_to_scanline__106__target = 0x000000f1U;
    while ((((IData)(vlSelfRef.test_copper__DOT__dut__DOT__v_count) 
             != __Vtask_test_copper__DOT__run_to_scanline__106__target) 
            | (0U != (IData)(vlSelfRef.test_copper__DOT__dut__DOT__h_count)))) {
        Vtest_copper___024root____VbeforeTrig_hbd4d80cf__0(vlSelf, 
                                                           "@(posedge test_copper.clk)");
        co_await vlSelfRef.__VtrigSched_hbd4d80cf__0.trigger(0U, 
                                                             nullptr, 
                                                             "@(posedge test_copper.clk)", 
                                                             "test_copper.sv", 
                                                             107);
    }
    __Vtask_test_copper__DOT__check__107__condition 
        = (2U == (IData)(vlSelfRef.test_copper__DOT__dut__DOT__bg_color));
    vlSelfRef.__Vtask_test_copper__DOT__check__107__name = "bg=2 (red) after line 100"s;
    vlSelfRef.test_copper__DOT__test_num = ((IData)(1U) 
                                            + vlSelfRef.test_copper__DOT__test_num);
    if (__Vtask_test_copper__DOT__check__107__condition) {
        VL_WRITEF_NX("  PASS [%0d] %@\n",0,32,vlSelfRef.test_copper__DOT__test_num,
                     -1,&(vlSelfRef.__Vtask_test_copper__DOT__check__107__name));
        vlSelfRef.test_copper__DOT__pass_count = ((IData)(1U) 
                                                  + vlSelfRef.test_copper__DOT__pass_count);
    } else {
        VL_WRITEF_NX("  FAIL [%0d] %@\n",0,32,vlSelfRef.test_copper__DOT__test_num,
                     -1,&(vlSelfRef.__Vtask_test_copper__DOT__check__107__name));
        vlSelfRef.test_copper__DOT__fail_count = ((IData)(1U) 
                                                  + vlSelfRef.test_copper__DOT__fail_count);
    }
    __Vtask_test_copper__DOT__run_to_scanline__108__target = 0x00000155U;
    while ((((IData)(vlSelfRef.test_copper__DOT__dut__DOT__v_count) 
             != __Vtask_test_copper__DOT__run_to_scanline__108__target) 
            | (0U != (IData)(vlSelfRef.test_copper__DOT__dut__DOT__h_count)))) {
        Vtest_copper___024root____VbeforeTrig_hbd4d80cf__0(vlSelf, 
                                                           "@(posedge test_copper.clk)");
        co_await vlSelfRef.__VtrigSched_hbd4d80cf__0.trigger(0U, 
                                                             nullptr, 
                                                             "@(posedge test_copper.clk)", 
                                                             "test_copper.sv", 
                                                             107);
    }
    __Vtask_test_copper__DOT__check__109__condition 
        = (7U == (IData)(vlSelfRef.test_copper__DOT__dut__DOT__bg_color));
    vlSelfRef.__Vtask_test_copper__DOT__check__109__name = "bg=7 (yellow) after line 150"s;
    vlSelfRef.test_copper__DOT__test_num = ((IData)(1U) 
                                            + vlSelfRef.test_copper__DOT__test_num);
    if (__Vtask_test_copper__DOT__check__109__condition) {
        VL_WRITEF_NX("  PASS [%0d] %@\n",0,32,vlSelfRef.test_copper__DOT__test_num,
                     -1,&(vlSelfRef.__Vtask_test_copper__DOT__check__109__name));
        vlSelfRef.test_copper__DOT__pass_count = ((IData)(1U) 
                                                  + vlSelfRef.test_copper__DOT__pass_count);
    } else {
        VL_WRITEF_NX("  FAIL [%0d] %@\n",0,32,vlSelfRef.test_copper__DOT__test_num,
                     -1,&(vlSelfRef.__Vtask_test_copper__DOT__check__109__name));
        vlSelfRef.test_copper__DOT__fail_count = ((IData)(1U) 
                                                  + vlSelfRef.test_copper__DOT__fail_count);
    }
    VL_WRITEF_NX("\n=== Results: %0d passed, %0d failed ===\n",0,
                 32,vlSelfRef.test_copper__DOT__pass_count,
                 32,vlSelfRef.test_copper__DOT__fail_count);
    if ((0U == vlSelfRef.test_copper__DOT__fail_count)) {
        VL_WRITEF_NX("ALL TESTS PASSED\n",0);
    } else {
        VL_WRITEF_NX("SOME TESTS FAILED\n",0);
    }
    VL_FINISH_MT("test_copper.sv", 236, "");
    co_return;
}

VlCoroutine Vtest_copper___024root___eval_initial__TOP__Vtiming__1(Vtest_copper___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vtest_copper___024root___eval_initial__TOP__Vtiming__1\n"); );
    Vtest_copper__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
    // Body
    while (VL_LIKELY(!vlSymsp->_vm_contextp__->gotFinish())) {
        co_await vlSelfRef.__VdlySched.delay(0x0000000000004e20ULL, 
                                             nullptr, 
                                             "test_copper.sv", 
                                             10);
        vlSelfRef.test_copper__DOT__clk = (1U & (~ (IData)(vlSelfRef.test_copper__DOT__clk)));
    }
    co_return;
}

void Vtest_copper___024root___eval_triggers_vec__act(Vtest_copper___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vtest_copper___024root___eval_triggers_vec__act\n"); );
    Vtest_copper__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
    // Body
    vlSelfRef.__VactTriggered[0U] = (QData)((IData)(
                                                    ((vlSelfRef.__VdlySched.awaitingCurrentTime() 
                                                      << 1U) 
                                                     | ((IData)(vlSelfRef.test_copper__DOT__clk) 
                                                        & (~ (IData)(vlSelfRef.__Vtrigprevexpr___TOP__test_copper__DOT__clk__0))))));
    vlSelfRef.__Vtrigprevexpr___TOP__test_copper__DOT__clk__0 
        = vlSelfRef.test_copper__DOT__clk;
}

bool Vtest_copper___024root___trigger_anySet__act(const VlUnpacked<QData/*63:0*/, 1> &in) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vtest_copper___024root___trigger_anySet__act\n"); );
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

void Vtest_copper___024root___act_sequent__TOP__0(Vtest_copper___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vtest_copper___024root___act_sequent__TOP__0\n"); );
    Vtest_copper__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
    // Body
    vlSelfRef.test_copper__DOT__dut__DOT__vgc_reg_sel 
        = ((0xa000U <= (IData)(vlSelfRef.test_copper__DOT__cpu_addr)) 
           & (0xa01fU >= (IData)(vlSelfRef.test_copper__DOT__cpu_addr)));
    vlSelfRef.test_copper__DOT__dut__DOT__char_ram_sel 
        = ((0xaa00U <= (IData)(vlSelfRef.test_copper__DOT__cpu_addr)) 
           & (0xb1cfU >= (IData)(vlSelfRef.test_copper__DOT__cpu_addr)));
    vlSelfRef.test_copper__DOT__dut__DOT__color_ram_sel 
        = ((0xb1d0U <= (IData)(vlSelfRef.test_copper__DOT__cpu_addr)) 
           & (0xb99fU >= (IData)(vlSelfRef.test_copper__DOT__cpu_addr)));
    vlSelfRef.test_copper__DOT__dut__DOT__tile_reg_sel 
        = ((0xa0c0U <= (IData)(vlSelfRef.test_copper__DOT__cpu_addr)) 
           & (0xa0dfU >= (IData)(vlSelfRef.test_copper__DOT__cpu_addr)));
}

void Vtest_copper___024root___eval_act(Vtest_copper___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vtest_copper___024root___eval_act\n"); );
    Vtest_copper__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
    // Body
    if ((1ULL & vlSelfRef.__VactTriggered[0U])) {
        vlSelfRef.test_copper__DOT__dut__DOT__vgc_reg_sel 
            = ((0xa000U <= (IData)(vlSelfRef.test_copper__DOT__cpu_addr)) 
               & (0xa01fU >= (IData)(vlSelfRef.test_copper__DOT__cpu_addr)));
        vlSelfRef.test_copper__DOT__dut__DOT__char_ram_sel 
            = ((0xaa00U <= (IData)(vlSelfRef.test_copper__DOT__cpu_addr)) 
               & (0xb1cfU >= (IData)(vlSelfRef.test_copper__DOT__cpu_addr)));
        vlSelfRef.test_copper__DOT__dut__DOT__color_ram_sel 
            = ((0xb1d0U <= (IData)(vlSelfRef.test_copper__DOT__cpu_addr)) 
               & (0xb99fU >= (IData)(vlSelfRef.test_copper__DOT__cpu_addr)));
        vlSelfRef.test_copper__DOT__dut__DOT__tile_reg_sel 
            = ((0xa0c0U <= (IData)(vlSelfRef.test_copper__DOT__cpu_addr)) 
               & (0xa0dfU >= (IData)(vlSelfRef.test_copper__DOT__cpu_addr)));
    }
}

void Vtest_copper___024root___nba_sequent__TOP__0(Vtest_copper___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vtest_copper___024root___nba_sequent__TOP__0\n"); );
    Vtest_copper__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
    // Locals
    CData/*0:0*/ test_copper__DOT__dut__DOT__visible;
    test_copper__DOT__dut__DOT__visible = 0;
    VlUnpacked<SData/*8:0*/, 16> test_copper__DOT__dut__DOT__spr_x;
    for (int __Vi0 = 0; __Vi0 < 16; ++__Vi0) {
        test_copper__DOT__dut__DOT__spr_x[__Vi0] = 0;
    }
    VlUnpacked<CData/*7:0*/, 16> test_copper__DOT__dut__DOT__spr_y;
    for (int __Vi0 = 0; __Vi0 < 16; ++__Vi0) {
        test_copper__DOT__dut__DOT__spr_y[__Vi0] = 0;
    }
    VlUnpacked<CData/*0:0*/, 16> test_copper__DOT__dut__DOT__spr_enable;
    for (int __Vi0 = 0; __Vi0 < 16; ++__Vi0) {
        test_copper__DOT__dut__DOT__spr_enable[__Vi0] = 0;
    }
    VlUnpacked<CData/*0:0*/, 16> test_copper__DOT__dut__DOT__spr_flip_h;
    for (int __Vi0 = 0; __Vi0 < 16; ++__Vi0) {
        test_copper__DOT__dut__DOT__spr_flip_h[__Vi0] = 0;
    }
    VlUnpacked<CData/*0:0*/, 16> test_copper__DOT__dut__DOT__spr_flip_v;
    for (int __Vi0 = 0; __Vi0 < 16; ++__Vi0) {
        test_copper__DOT__dut__DOT__spr_flip_v[__Vi0] = 0;
    }
    VlUnpacked<CData/*1:0*/, 16> test_copper__DOT__dut__DOT__spr_pri;
    for (int __Vi0 = 0; __Vi0 < 16; ++__Vi0) {
        test_copper__DOT__dut__DOT__spr_pri[__Vi0] = 0;
    }
    VlUnpacked<CData/*3:0*/, 16> test_copper__DOT__dut__DOT__spr_shape;
    for (int __Vi0 = 0; __Vi0 < 16; ++__Vi0) {
        test_copper__DOT__dut__DOT__spr_shape[__Vi0] = 0;
    }
    VlUnpacked<CData/*3:0*/, 16> test_copper__DOT__dut__DOT__spr_trans;
    for (int __Vi0 = 0; __Vi0 < 16; ++__Vi0) {
        test_copper__DOT__dut__DOT__spr_trans[__Vi0] = 0;
    }
    CData/*4:0*/ test_copper__DOT__dut__DOT__text_row;
    test_copper__DOT__dut__DOT__text_row = 0;
    CData/*4:0*/ test_copper__DOT__dut__DOT__real_row;
    test_copper__DOT__dut__DOT__real_row = 0;
    SData/*9:0*/ test_copper__DOT__dut__DOT__text_line;
    test_copper__DOT__dut__DOT__text_line = 0;
    VlUnpacked<SData/*11:0*/, 256> test_copper__DOT__dut__DOT__tile_inst__DOT__pal_ram;
    for (int __Vi0 = 0; __Vi0 < 256; ++__Vi0) {
        test_copper__DOT__dut__DOT__tile_inst__DOT__pal_ram[__Vi0] = 0;
    }
    SData/*10:0*/ __Vfunc_test_copper__DOT__dut__DOT__screen_addr__110__Vfuncout;
    __Vfunc_test_copper__DOT__dut__DOT__screen_addr__110__Vfuncout = 0;
    CData/*6:0*/ __Vfunc_test_copper__DOT__dut__DOT__screen_addr__110__col;
    __Vfunc_test_copper__DOT__dut__DOT__screen_addr__110__col = 0;
    CData/*4:0*/ __Vfunc_test_copper__DOT__dut__DOT__screen_addr__110__rr;
    __Vfunc_test_copper__DOT__dut__DOT__screen_addr__110__rr = 0;
    SData/*10:0*/ __Vfunc_test_copper__DOT__dut__DOT__screen_addr__111__Vfuncout;
    __Vfunc_test_copper__DOT__dut__DOT__screen_addr__111__Vfuncout = 0;
    CData/*6:0*/ __Vfunc_test_copper__DOT__dut__DOT__screen_addr__111__col;
    __Vfunc_test_copper__DOT__dut__DOT__screen_addr__111__col = 0;
    CData/*4:0*/ __Vfunc_test_copper__DOT__dut__DOT__screen_addr__111__rr;
    __Vfunc_test_copper__DOT__dut__DOT__screen_addr__111__rr = 0;
    SData/*10:0*/ __Vfunc_test_copper__DOT__dut__DOT__screen_addr__112__Vfuncout;
    __Vfunc_test_copper__DOT__dut__DOT__screen_addr__112__Vfuncout = 0;
    CData/*6:0*/ __Vfunc_test_copper__DOT__dut__DOT__screen_addr__112__col;
    __Vfunc_test_copper__DOT__dut__DOT__screen_addr__112__col = 0;
    CData/*4:0*/ __Vfunc_test_copper__DOT__dut__DOT__screen_addr__112__rr;
    __Vfunc_test_copper__DOT__dut__DOT__screen_addr__112__rr = 0;
    SData/*10:0*/ __Vfunc_test_copper__DOT__dut__DOT__screen_addr__113__Vfuncout;
    __Vfunc_test_copper__DOT__dut__DOT__screen_addr__113__Vfuncout = 0;
    CData/*6:0*/ __Vfunc_test_copper__DOT__dut__DOT__screen_addr__113__col;
    __Vfunc_test_copper__DOT__dut__DOT__screen_addr__113__col = 0;
    CData/*4:0*/ __Vfunc_test_copper__DOT__dut__DOT__screen_addr__113__rr;
    __Vfunc_test_copper__DOT__dut__DOT__screen_addr__113__rr = 0;
    SData/*10:0*/ __Vfunc_test_copper__DOT__dut__DOT__screen_addr__114__Vfuncout;
    __Vfunc_test_copper__DOT__dut__DOT__screen_addr__114__Vfuncout = 0;
    CData/*6:0*/ __Vfunc_test_copper__DOT__dut__DOT__screen_addr__114__col;
    __Vfunc_test_copper__DOT__dut__DOT__screen_addr__114__col = 0;
    CData/*4:0*/ __Vfunc_test_copper__DOT__dut__DOT__screen_addr__114__row;
    __Vfunc_test_copper__DOT__dut__DOT__screen_addr__114__row = 0;
    CData/*4:0*/ __Vfunc_test_copper__DOT__dut__DOT__screen_addr__114__rr;
    __Vfunc_test_copper__DOT__dut__DOT__screen_addr__114__rr = 0;
    SData/*10:0*/ __Vfunc_test_copper__DOT__dut__DOT__screen_addr__115__Vfuncout;
    __Vfunc_test_copper__DOT__dut__DOT__screen_addr__115__Vfuncout = 0;
    CData/*6:0*/ __Vfunc_test_copper__DOT__dut__DOT__screen_addr__115__col;
    __Vfunc_test_copper__DOT__dut__DOT__screen_addr__115__col = 0;
    CData/*4:0*/ __Vfunc_test_copper__DOT__dut__DOT__screen_addr__115__row;
    __Vfunc_test_copper__DOT__dut__DOT__screen_addr__115__row = 0;
    CData/*4:0*/ __Vfunc_test_copper__DOT__dut__DOT__screen_addr__115__rr;
    __Vfunc_test_copper__DOT__dut__DOT__screen_addr__115__rr = 0;
    SData/*10:0*/ __Vfunc_test_copper__DOT__dut__DOT__screen_addr__116__Vfuncout;
    __Vfunc_test_copper__DOT__dut__DOT__screen_addr__116__Vfuncout = 0;
    CData/*6:0*/ __Vfunc_test_copper__DOT__dut__DOT__screen_addr__116__col;
    __Vfunc_test_copper__DOT__dut__DOT__screen_addr__116__col = 0;
    CData/*4:0*/ __Vfunc_test_copper__DOT__dut__DOT__screen_addr__116__row;
    __Vfunc_test_copper__DOT__dut__DOT__screen_addr__116__row = 0;
    CData/*4:0*/ __Vfunc_test_copper__DOT__dut__DOT__screen_addr__116__rr;
    __Vfunc_test_copper__DOT__dut__DOT__screen_addr__116__rr = 0;
    SData/*10:0*/ __Vfunc_test_copper__DOT__dut__DOT__screen_addr__117__Vfuncout;
    __Vfunc_test_copper__DOT__dut__DOT__screen_addr__117__Vfuncout = 0;
    CData/*6:0*/ __Vfunc_test_copper__DOT__dut__DOT__screen_addr__117__col;
    __Vfunc_test_copper__DOT__dut__DOT__screen_addr__117__col = 0;
    CData/*4:0*/ __Vfunc_test_copper__DOT__dut__DOT__screen_addr__117__row;
    __Vfunc_test_copper__DOT__dut__DOT__screen_addr__117__row = 0;
    CData/*4:0*/ __Vfunc_test_copper__DOT__dut__DOT__screen_addr__117__rr;
    __Vfunc_test_copper__DOT__dut__DOT__screen_addr__117__rr = 0;
    SData/*10:0*/ __Vfunc_test_copper__DOT__dut__DOT__screen_addr__118__Vfuncout;
    __Vfunc_test_copper__DOT__dut__DOT__screen_addr__118__Vfuncout = 0;
    CData/*6:0*/ __Vfunc_test_copper__DOT__dut__DOT__screen_addr__118__col;
    __Vfunc_test_copper__DOT__dut__DOT__screen_addr__118__col = 0;
    CData/*4:0*/ __Vfunc_test_copper__DOT__dut__DOT__screen_addr__118__row;
    __Vfunc_test_copper__DOT__dut__DOT__screen_addr__118__row = 0;
    CData/*4:0*/ __Vfunc_test_copper__DOT__dut__DOT__screen_addr__118__rr;
    __Vfunc_test_copper__DOT__dut__DOT__screen_addr__118__rr = 0;
    SData/*10:0*/ __Vfunc_test_copper__DOT__dut__DOT__screen_addr__119__Vfuncout;
    __Vfunc_test_copper__DOT__dut__DOT__screen_addr__119__Vfuncout = 0;
    CData/*6:0*/ __Vfunc_test_copper__DOT__dut__DOT__screen_addr__119__col;
    __Vfunc_test_copper__DOT__dut__DOT__screen_addr__119__col = 0;
    CData/*4:0*/ __Vfunc_test_copper__DOT__dut__DOT__screen_addr__119__row;
    __Vfunc_test_copper__DOT__dut__DOT__screen_addr__119__row = 0;
    CData/*4:0*/ __Vfunc_test_copper__DOT__dut__DOT__screen_addr__119__rr;
    __Vfunc_test_copper__DOT__dut__DOT__screen_addr__119__rr = 0;
    SData/*9:0*/ __Vdly__test_copper__DOT__dut__DOT__h_count;
    __Vdly__test_copper__DOT__dut__DOT__h_count = 0;
    SData/*9:0*/ __Vdly__test_copper__DOT__dut__DOT__v_count;
    __Vdly__test_copper__DOT__dut__DOT__v_count = 0;
    CData/*6:0*/ __Vdly__test_copper__DOT__dut__DOT__cursor_x;
    __Vdly__test_copper__DOT__dut__DOT__cursor_x = 0;
    CData/*4:0*/ __Vdly__test_copper__DOT__dut__DOT__cursor_y;
    __Vdly__test_copper__DOT__dut__DOT__cursor_y = 0;
    CData/*3:0*/ __Vdly__test_copper__DOT__dut__DOT__fg_color;
    __Vdly__test_copper__DOT__dut__DOT__fg_color = 0;
    CData/*3:0*/ __Vdly__test_copper__DOT__dut__DOT__gfx_color;
    __Vdly__test_copper__DOT__dut__DOT__gfx_color = 0;
    CData/*4:0*/ __Vdly__test_copper__DOT__dut__DOT__scroll_offset;
    __Vdly__test_copper__DOT__dut__DOT__scroll_offset = 0;
    CData/*0:0*/ __Vdly__test_copper__DOT__dut__DOT__scroll_pending;
    __Vdly__test_copper__DOT__dut__DOT__scroll_pending = 0;
    CData/*6:0*/ __Vdly__test_copper__DOT__dut__DOT__scroll_col;
    __Vdly__test_copper__DOT__dut__DOT__scroll_col = 0;
    CData/*0:0*/ __Vdly__test_copper__DOT__dut__DOT__cmd_busy;
    __Vdly__test_copper__DOT__dut__DOT__cmd_busy = 0;
    SData/*8:0*/ __Vdly__test_copper__DOT__dut__DOT__copper_count;
    __Vdly__test_copper__DOT__dut__DOT__copper_count = 0;
    SData/*8:0*/ __Vdly__test_copper__DOT__dut__DOT__copper_index;
    __Vdly__test_copper__DOT__dut__DOT__copper_index = 0;
    CData/*6:0*/ __Vdly__test_copper__DOT__dut__DOT__copper_target_list;
    __Vdly__test_copper__DOT__dut__DOT__copper_target_list = 0;
    CData/*6:0*/ __Vdly__test_copper__DOT__dut__DOT__copper_pending_list;
    __Vdly__test_copper__DOT__dut__DOT__copper_pending_list = 0;
    CData/*0:0*/ __Vdly__test_copper__DOT__dut__DOT__copper_loading;
    __Vdly__test_copper__DOT__dut__DOT__copper_loading = 0;
    SData/*9:0*/ __Vdly__test_copper__DOT__dut__DOT__fill_sp;
    __Vdly__test_copper__DOT__dut__DOT__fill_sp = 0;
    SData/*8:0*/ __Vdly__test_copper__DOT__dut__DOT__cmd_cx;
    __Vdly__test_copper__DOT__dut__DOT__cmd_cx = 0;
    CData/*7:0*/ __Vdly__test_copper__DOT__dut__DOT__cmd_cy;
    __Vdly__test_copper__DOT__dut__DOT__cmd_cy = 0;
    CData/*2:0*/ __Vdly__test_copper__DOT__dut__DOT__gt_font_row;
    __Vdly__test_copper__DOT__dut__DOT__gt_font_row = 0;
    CData/*5:0*/ __Vdly__test_copper__DOT__dut__DOT__gt_char_idx;
    __Vdly__test_copper__DOT__dut__DOT__gt_char_idx = 0;
    SData/*9:0*/ __Vdly__test_copper__DOT__dut__DOT__cmd_dx;
    __Vdly__test_copper__DOT__dut__DOT__cmd_dx = 0;
    SData/*9:0*/ __Vdly__test_copper__DOT__dut__DOT__cmd_err;
    __Vdly__test_copper__DOT__dut__DOT__cmd_err = 0;
    SData/*9:0*/ __Vdly__test_copper__DOT__dut__DOT__cmd_dy;
    __Vdly__test_copper__DOT__dut__DOT__cmd_dy = 0;
    SData/*9:0*/ __Vdly__test_copper__DOT__dut__DOT__cmd_x;
    __Vdly__test_copper__DOT__dut__DOT__cmd_x = 0;
    SData/*9:0*/ __Vdly__test_copper__DOT__dut__DOT__cmd_y;
    __Vdly__test_copper__DOT__dut__DOT__cmd_y = 0;
    CData/*6:0*/ __Vdly__test_copper__DOT__dut__DOT__copper_active_list;
    __Vdly__test_copper__DOT__dut__DOT__copper_active_list = 0;
    CData/*3:0*/ __Vdly__test_copper__DOT__dut__DOT__tile_inst__DOT__dma_state;
    __Vdly__test_copper__DOT__dut__DOT__tile_inst__DOT__dma_state = 0;
    CData/*0:0*/ __Vdly__test_copper__DOT__dut__DOT__tile_inst__DOT__dma_data_valid;
    __Vdly__test_copper__DOT__dut__DOT__tile_inst__DOT__dma_data_valid = 0;
    SData/*15:0*/ __Vdly__test_copper__DOT__dut__DOT__tile_inst__DOT__dma_count;
    __Vdly__test_copper__DOT__dut__DOT__tile_inst__DOT__dma_count = 0;
    CData/*4:0*/ __Vdly__test_copper__DOT__dut__DOT__tile_inst__DOT__dma_col_idx;
    __Vdly__test_copper__DOT__dut__DOT__tile_inst__DOT__dma_col_idx = 0;
    SData/*15:0*/ __Vdly__test_copper__DOT__dut__DOT__tile_inst__DOT__dma_src;
    __Vdly__test_copper__DOT__dut__DOT__tile_inst__DOT__dma_src = 0;
    SData/*15:0*/ __Vdly__test_copper__DOT__dut__DOT__tile_inst__DOT__dma_dst;
    __Vdly__test_copper__DOT__dut__DOT__tile_inst__DOT__dma_dst = 0;
    CData/*7:0*/ __Vdly__test_copper__DOT__dut__DOT__tile_inst__DOT__dma_pal_r;
    __Vdly__test_copper__DOT__dut__DOT__tile_inst__DOT__dma_pal_r = 0;
    CData/*7:0*/ __Vdly__test_copper__DOT__dut__DOT__tile_inst__DOT__dma_pal_phase;
    __Vdly__test_copper__DOT__dut__DOT__tile_inst__DOT__dma_pal_phase = 0;
    CData/*7:0*/ __Vdly__test_copper__DOT__dut__DOT__tile_inst__DOT__dma_pal_g;
    __Vdly__test_copper__DOT__dut__DOT__tile_inst__DOT__dma_pal_g = 0;
    CData/*0:0*/ __VdlySet__test_copper__DOT__dut__DOT__regs__v0;
    __VdlySet__test_copper__DOT__dut__DOT__regs__v0 = 0;
    CData/*7:0*/ __VdlyVal__test_copper__DOT__dut__DOT__char_ram__v0;
    __VdlyVal__test_copper__DOT__dut__DOT__char_ram__v0 = 0;
    SData/*10:0*/ __VdlyDim0__test_copper__DOT__dut__DOT__char_ram__v0;
    __VdlyDim0__test_copper__DOT__dut__DOT__char_ram__v0 = 0;
    CData/*7:0*/ __VdlyVal__test_copper__DOT__dut__DOT__color_ram__v0;
    __VdlyVal__test_copper__DOT__dut__DOT__color_ram__v0 = 0;
    SData/*10:0*/ __VdlyDim0__test_copper__DOT__dut__DOT__color_ram__v0;
    __VdlyDim0__test_copper__DOT__dut__DOT__color_ram__v0 = 0;
    CData/*3:0*/ __VdlyVal__test_copper__DOT__dut__DOT__gfx_ram__v0;
    __VdlyVal__test_copper__DOT__dut__DOT__gfx_ram__v0 = 0;
    SData/*15:0*/ __VdlyDim0__test_copper__DOT__dut__DOT__gfx_ram__v0;
    __VdlyDim0__test_copper__DOT__dut__DOT__gfx_ram__v0 = 0;
    SData/*10:0*/ __VdlyDim0__test_copper__DOT__dut__DOT__sprite_shapes__v0;
    __VdlyDim0__test_copper__DOT__dut__DOT__sprite_shapes__v0 = 0;
    CData/*0:0*/ __VdlySet__test_copper__DOT__dut__DOT__regs__v1;
    __VdlySet__test_copper__DOT__dut__DOT__regs__v1 = 0;
    CData/*7:0*/ __VdlyVal__test_copper__DOT__dut__DOT__char_ram__v1;
    __VdlyVal__test_copper__DOT__dut__DOT__char_ram__v1 = 0;
    SData/*10:0*/ __VdlyDim0__test_copper__DOT__dut__DOT__char_ram__v1;
    __VdlyDim0__test_copper__DOT__dut__DOT__char_ram__v1 = 0;
    CData/*7:0*/ __VdlyVal__test_copper__DOT__dut__DOT__color_ram__v1;
    __VdlyVal__test_copper__DOT__dut__DOT__color_ram__v1 = 0;
    SData/*10:0*/ __VdlyDim0__test_copper__DOT__dut__DOT__color_ram__v1;
    __VdlyDim0__test_copper__DOT__dut__DOT__color_ram__v1 = 0;
    CData/*7:0*/ __VdlyVal__test_copper__DOT__dut__DOT__char_ram__v2;
    __VdlyVal__test_copper__DOT__dut__DOT__char_ram__v2 = 0;
    SData/*10:0*/ __VdlyDim0__test_copper__DOT__dut__DOT__char_ram__v2;
    __VdlyDim0__test_copper__DOT__dut__DOT__char_ram__v2 = 0;
    CData/*7:0*/ __VdlyVal__test_copper__DOT__dut__DOT__color_ram__v2;
    __VdlyVal__test_copper__DOT__dut__DOT__color_ram__v2 = 0;
    SData/*10:0*/ __VdlyDim0__test_copper__DOT__dut__DOT__color_ram__v2;
    __VdlyDim0__test_copper__DOT__dut__DOT__color_ram__v2 = 0;
    CData/*7:0*/ __VdlyVal__test_copper__DOT__dut__DOT__sprite_shapes__v1;
    __VdlyVal__test_copper__DOT__dut__DOT__sprite_shapes__v1 = 0;
    SData/*10:0*/ __VdlyDim0__test_copper__DOT__dut__DOT__sprite_shapes__v1;
    __VdlyDim0__test_copper__DOT__dut__DOT__sprite_shapes__v1 = 0;
    SData/*10:0*/ __VdlyDim0__test_copper__DOT__dut__DOT__sprite_shapes__v2;
    __VdlyDim0__test_copper__DOT__dut__DOT__sprite_shapes__v2 = 0;
    CData/*3:0*/ __VdlyVal__test_copper__DOT__dut__DOT__gfx_ram__v1;
    __VdlyVal__test_copper__DOT__dut__DOT__gfx_ram__v1 = 0;
    SData/*15:0*/ __VdlyDim0__test_copper__DOT__dut__DOT__gfx_ram__v1;
    __VdlyDim0__test_copper__DOT__dut__DOT__gfx_ram__v1 = 0;
    CData/*3:0*/ __VdlyVal__test_copper__DOT__dut__DOT__gfx_ram__v2;
    __VdlyVal__test_copper__DOT__dut__DOT__gfx_ram__v2 = 0;
    SData/*15:0*/ __VdlyDim0__test_copper__DOT__dut__DOT__gfx_ram__v2;
    __VdlyDim0__test_copper__DOT__dut__DOT__gfx_ram__v2 = 0;
    IData/*16:0*/ __VdlyVal__test_copper__DOT__dut__DOT__fill_stack__v0;
    __VdlyVal__test_copper__DOT__dut__DOT__fill_stack__v0 = 0;
    SData/*8:0*/ __VdlyDim0__test_copper__DOT__dut__DOT__fill_stack__v0;
    __VdlyDim0__test_copper__DOT__dut__DOT__fill_stack__v0 = 0;
    CData/*0:0*/ __VdlySet__test_copper__DOT__dut__DOT__fill_stack__v0;
    __VdlySet__test_copper__DOT__dut__DOT__fill_stack__v0 = 0;
    IData/*16:0*/ __VdlyVal__test_copper__DOT__dut__DOT__fill_stack__v1;
    __VdlyVal__test_copper__DOT__dut__DOT__fill_stack__v1 = 0;
    SData/*8:0*/ __VdlyDim0__test_copper__DOT__dut__DOT__fill_stack__v1;
    __VdlyDim0__test_copper__DOT__dut__DOT__fill_stack__v1 = 0;
    IData/*16:0*/ __VdlyVal__test_copper__DOT__dut__DOT__fill_stack__v2;
    __VdlyVal__test_copper__DOT__dut__DOT__fill_stack__v2 = 0;
    SData/*8:0*/ __VdlyDim0__test_copper__DOT__dut__DOT__fill_stack__v2;
    __VdlyDim0__test_copper__DOT__dut__DOT__fill_stack__v2 = 0;
    IData/*16:0*/ __VdlyVal__test_copper__DOT__dut__DOT__fill_stack__v3;
    __VdlyVal__test_copper__DOT__dut__DOT__fill_stack__v3 = 0;
    SData/*8:0*/ __VdlyDim0__test_copper__DOT__dut__DOT__fill_stack__v3;
    __VdlyDim0__test_copper__DOT__dut__DOT__fill_stack__v3 = 0;
    CData/*3:0*/ __VdlyVal__test_copper__DOT__dut__DOT__gfx_ram__v3;
    __VdlyVal__test_copper__DOT__dut__DOT__gfx_ram__v3 = 0;
    SData/*15:0*/ __VdlyDim0__test_copper__DOT__dut__DOT__gfx_ram__v3;
    __VdlyDim0__test_copper__DOT__dut__DOT__gfx_ram__v3 = 0;
    CData/*3:0*/ __VdlyVal__test_copper__DOT__dut__DOT__gfx_ram__v4;
    __VdlyVal__test_copper__DOT__dut__DOT__gfx_ram__v4 = 0;
    SData/*15:0*/ __VdlyDim0__test_copper__DOT__dut__DOT__gfx_ram__v4;
    __VdlyDim0__test_copper__DOT__dut__DOT__gfx_ram__v4 = 0;
    CData/*3:0*/ __VdlyVal__test_copper__DOT__dut__DOT__gfx_ram__v5;
    __VdlyVal__test_copper__DOT__dut__DOT__gfx_ram__v5 = 0;
    SData/*15:0*/ __VdlyDim0__test_copper__DOT__dut__DOT__gfx_ram__v5;
    __VdlyDim0__test_copper__DOT__dut__DOT__gfx_ram__v5 = 0;
    CData/*3:0*/ __VdlyVal__test_copper__DOT__dut__DOT__gfx_ram__v6;
    __VdlyVal__test_copper__DOT__dut__DOT__gfx_ram__v6 = 0;
    SData/*15:0*/ __VdlyDim0__test_copper__DOT__dut__DOT__gfx_ram__v6;
    __VdlyDim0__test_copper__DOT__dut__DOT__gfx_ram__v6 = 0;
    CData/*3:0*/ __VdlyVal__test_copper__DOT__dut__DOT__gfx_ram__v7;
    __VdlyVal__test_copper__DOT__dut__DOT__gfx_ram__v7 = 0;
    SData/*15:0*/ __VdlyDim0__test_copper__DOT__dut__DOT__gfx_ram__v7;
    __VdlyDim0__test_copper__DOT__dut__DOT__gfx_ram__v7 = 0;
    CData/*3:0*/ __VdlyVal__test_copper__DOT__dut__DOT__gfx_ram__v8;
    __VdlyVal__test_copper__DOT__dut__DOT__gfx_ram__v8 = 0;
    SData/*15:0*/ __VdlyDim0__test_copper__DOT__dut__DOT__gfx_ram__v8;
    __VdlyDim0__test_copper__DOT__dut__DOT__gfx_ram__v8 = 0;
    CData/*3:0*/ __VdlyVal__test_copper__DOT__dut__DOT__gfx_ram__v9;
    __VdlyVal__test_copper__DOT__dut__DOT__gfx_ram__v9 = 0;
    SData/*15:0*/ __VdlyDim0__test_copper__DOT__dut__DOT__gfx_ram__v9;
    __VdlyDim0__test_copper__DOT__dut__DOT__gfx_ram__v9 = 0;
    CData/*3:0*/ __VdlyVal__test_copper__DOT__dut__DOT__gfx_ram__v10;
    __VdlyVal__test_copper__DOT__dut__DOT__gfx_ram__v10 = 0;
    SData/*15:0*/ __VdlyDim0__test_copper__DOT__dut__DOT__gfx_ram__v10;
    __VdlyDim0__test_copper__DOT__dut__DOT__gfx_ram__v10 = 0;
    CData/*3:0*/ __VdlyVal__test_copper__DOT__dut__DOT__gfx_ram__v11;
    __VdlyVal__test_copper__DOT__dut__DOT__gfx_ram__v11 = 0;
    SData/*15:0*/ __VdlyDim0__test_copper__DOT__dut__DOT__gfx_ram__v11;
    __VdlyDim0__test_copper__DOT__dut__DOT__gfx_ram__v11 = 0;
    CData/*3:0*/ __VdlyVal__test_copper__DOT__dut__DOT__gfx_ram__v12;
    __VdlyVal__test_copper__DOT__dut__DOT__gfx_ram__v12 = 0;
    SData/*15:0*/ __VdlyDim0__test_copper__DOT__dut__DOT__gfx_ram__v12;
    __VdlyDim0__test_copper__DOT__dut__DOT__gfx_ram__v12 = 0;
    CData/*3:0*/ __VdlyVal__test_copper__DOT__dut__DOT__gfx_ram__v13;
    __VdlyVal__test_copper__DOT__dut__DOT__gfx_ram__v13 = 0;
    SData/*15:0*/ __VdlyDim0__test_copper__DOT__dut__DOT__gfx_ram__v13;
    __VdlyDim0__test_copper__DOT__dut__DOT__gfx_ram__v13 = 0;
    CData/*3:0*/ __VdlyVal__test_copper__DOT__dut__DOT__gfx_ram__v14;
    __VdlyVal__test_copper__DOT__dut__DOT__gfx_ram__v14 = 0;
    SData/*15:0*/ __VdlyDim0__test_copper__DOT__dut__DOT__gfx_ram__v14;
    __VdlyDim0__test_copper__DOT__dut__DOT__gfx_ram__v14 = 0;
    IData/*16:0*/ __VdlyVal__test_copper__DOT__dut__DOT__copper_pos__v0;
    __VdlyVal__test_copper__DOT__dut__DOT__copper_pos__v0 = 0;
    CData/*4:0*/ __VdlyDim0__test_copper__DOT__dut__DOT__copper_pos__v0;
    __VdlyDim0__test_copper__DOT__dut__DOT__copper_pos__v0 = 0;
    CData/*0:0*/ __VdlySet__test_copper__DOT__dut__DOT__copper_pos__v0;
    __VdlySet__test_copper__DOT__dut__DOT__copper_pos__v0 = 0;
    CData/*7:0*/ __VdlyVal__test_copper__DOT__dut__DOT__copper_reg__v0;
    __VdlyVal__test_copper__DOT__dut__DOT__copper_reg__v0 = 0;
    CData/*4:0*/ __VdlyDim0__test_copper__DOT__dut__DOT__copper_reg__v0;
    __VdlyDim0__test_copper__DOT__dut__DOT__copper_reg__v0 = 0;
    CData/*7:0*/ __VdlyVal__test_copper__DOT__dut__DOT__copper_val__v0;
    __VdlyVal__test_copper__DOT__dut__DOT__copper_val__v0 = 0;
    CData/*4:0*/ __VdlyDim0__test_copper__DOT__dut__DOT__copper_val__v0;
    __VdlyDim0__test_copper__DOT__dut__DOT__copper_val__v0 = 0;
    CData/*7:0*/ __VdlyVal__test_copper__DOT__dut__DOT__char_ram__v3;
    __VdlyVal__test_copper__DOT__dut__DOT__char_ram__v3 = 0;
    SData/*10:0*/ __VdlyDim0__test_copper__DOT__dut__DOT__char_ram__v3;
    __VdlyDim0__test_copper__DOT__dut__DOT__char_ram__v3 = 0;
    CData/*7:0*/ __VdlyVal__test_copper__DOT__dut__DOT__char_ram__v4;
    __VdlyVal__test_copper__DOT__dut__DOT__char_ram__v4 = 0;
    SData/*10:0*/ __VdlyDim0__test_copper__DOT__dut__DOT__char_ram__v4;
    __VdlyDim0__test_copper__DOT__dut__DOT__char_ram__v4 = 0;
    CData/*7:0*/ __VdlyVal__test_copper__DOT__dut__DOT__color_ram__v3;
    __VdlyVal__test_copper__DOT__dut__DOT__color_ram__v3 = 0;
    SData/*10:0*/ __VdlyDim0__test_copper__DOT__dut__DOT__color_ram__v3;
    __VdlyDim0__test_copper__DOT__dut__DOT__color_ram__v3 = 0;
    CData/*6:0*/ __VdlyDim0__test_copper__DOT__dut__DOT__copper_list_count__v0;
    __VdlyDim0__test_copper__DOT__dut__DOT__copper_list_count__v0 = 0;
    CData/*6:0*/ __VdlyDim0__test_copper__DOT__dut__DOT__copper_list_count__v1;
    __VdlyDim0__test_copper__DOT__dut__DOT__copper_list_count__v1 = 0;
    IData/*16:0*/ __VdlyVal__test_copper__DOT__dut__DOT__copper_list_pos__v0;
    __VdlyVal__test_copper__DOT__dut__DOT__copper_list_pos__v0 = 0;
    SData/*11:0*/ __VdlyDim0__test_copper__DOT__dut__DOT__copper_list_pos__v0;
    __VdlyDim0__test_copper__DOT__dut__DOT__copper_list_pos__v0 = 0;
    CData/*0:0*/ __VdlySet__test_copper__DOT__dut__DOT__copper_list_pos__v0;
    __VdlySet__test_copper__DOT__dut__DOT__copper_list_pos__v0 = 0;
    CData/*7:0*/ __VdlyVal__test_copper__DOT__dut__DOT__copper_list_reg__v0;
    __VdlyVal__test_copper__DOT__dut__DOT__copper_list_reg__v0 = 0;
    SData/*11:0*/ __VdlyDim0__test_copper__DOT__dut__DOT__copper_list_reg__v0;
    __VdlyDim0__test_copper__DOT__dut__DOT__copper_list_reg__v0 = 0;
    CData/*7:0*/ __VdlyVal__test_copper__DOT__dut__DOT__copper_list_val__v0;
    __VdlyVal__test_copper__DOT__dut__DOT__copper_list_val__v0 = 0;
    SData/*11:0*/ __VdlyDim0__test_copper__DOT__dut__DOT__copper_list_val__v0;
    __VdlyDim0__test_copper__DOT__dut__DOT__copper_list_val__v0 = 0;
    SData/*8:0*/ __VdlyVal__test_copper__DOT__dut__DOT__copper_list_count__v2;
    __VdlyVal__test_copper__DOT__dut__DOT__copper_list_count__v2 = 0;
    CData/*6:0*/ __VdlyDim0__test_copper__DOT__dut__DOT__copper_list_count__v2;
    __VdlyDim0__test_copper__DOT__dut__DOT__copper_list_count__v2 = 0;
    IData/*16:0*/ __VdlyVal__test_copper__DOT__dut__DOT__copper_pos__v1;
    __VdlyVal__test_copper__DOT__dut__DOT__copper_pos__v1 = 0;
    CData/*4:0*/ __VdlyDim0__test_copper__DOT__dut__DOT__copper_pos__v1;
    __VdlyDim0__test_copper__DOT__dut__DOT__copper_pos__v1 = 0;
    CData/*0:0*/ __VdlySet__test_copper__DOT__dut__DOT__copper_pos__v1;
    __VdlySet__test_copper__DOT__dut__DOT__copper_pos__v1 = 0;
    CData/*7:0*/ __VdlyVal__test_copper__DOT__dut__DOT__copper_reg__v1;
    __VdlyVal__test_copper__DOT__dut__DOT__copper_reg__v1 = 0;
    CData/*4:0*/ __VdlyDim0__test_copper__DOT__dut__DOT__copper_reg__v1;
    __VdlyDim0__test_copper__DOT__dut__DOT__copper_reg__v1 = 0;
    CData/*7:0*/ __VdlyVal__test_copper__DOT__dut__DOT__copper_val__v1;
    __VdlyVal__test_copper__DOT__dut__DOT__copper_val__v1 = 0;
    CData/*4:0*/ __VdlyDim0__test_copper__DOT__dut__DOT__copper_val__v1;
    __VdlyDim0__test_copper__DOT__dut__DOT__copper_val__v1 = 0;
    CData/*7:0*/ __VdlyVal__test_copper__DOT__dut__DOT__char_ram__v5;
    __VdlyVal__test_copper__DOT__dut__DOT__char_ram__v5 = 0;
    SData/*10:0*/ __VdlyDim0__test_copper__DOT__dut__DOT__char_ram__v5;
    __VdlyDim0__test_copper__DOT__dut__DOT__char_ram__v5 = 0;
    CData/*7:0*/ __VdlyVal__test_copper__DOT__dut__DOT__color_ram__v4;
    __VdlyVal__test_copper__DOT__dut__DOT__color_ram__v4 = 0;
    SData/*10:0*/ __VdlyDim0__test_copper__DOT__dut__DOT__color_ram__v4;
    __VdlyDim0__test_copper__DOT__dut__DOT__color_ram__v4 = 0;
    CData/*3:0*/ __VdlyVal__test_copper__DOT__dut__DOT__gfx_ram__v15;
    __VdlyVal__test_copper__DOT__dut__DOT__gfx_ram__v15 = 0;
    SData/*15:0*/ __VdlyDim0__test_copper__DOT__dut__DOT__gfx_ram__v15;
    __VdlyDim0__test_copper__DOT__dut__DOT__gfx_ram__v15 = 0;
    CData/*7:0*/ __VdlyVal__test_copper__DOT__dut__DOT__sprite_shapes__v3;
    __VdlyVal__test_copper__DOT__dut__DOT__sprite_shapes__v3 = 0;
    SData/*10:0*/ __VdlyDim0__test_copper__DOT__dut__DOT__sprite_shapes__v3;
    __VdlyDim0__test_copper__DOT__dut__DOT__sprite_shapes__v3 = 0;
    CData/*7:0*/ __VdlyVal__test_copper__DOT__dut__DOT__tile_inst__DOT__tile_data__v0;
    __VdlyVal__test_copper__DOT__dut__DOT__tile_inst__DOT__tile_data__v0 = 0;
    SData/*14:0*/ __VdlyDim0__test_copper__DOT__dut__DOT__tile_inst__DOT__tile_data__v0;
    __VdlyDim0__test_copper__DOT__dut__DOT__tile_inst__DOT__tile_data__v0 = 0;
    CData/*0:0*/ __VdlySet__test_copper__DOT__dut__DOT__tile_inst__DOT__tile_data__v0;
    __VdlySet__test_copper__DOT__dut__DOT__tile_inst__DOT__tile_data__v0 = 0;
    CData/*7:0*/ __VdlyVal__test_copper__DOT__dut__DOT__regs__v32;
    __VdlyVal__test_copper__DOT__dut__DOT__regs__v32 = 0;
    CData/*0:0*/ __VdlySet__test_copper__DOT__dut__DOT__regs__v32;
    __VdlySet__test_copper__DOT__dut__DOT__regs__v32 = 0;
    CData/*7:0*/ __VdlyVal__test_copper__DOT__dut__DOT__regs__v33;
    __VdlyVal__test_copper__DOT__dut__DOT__regs__v33 = 0;
    CData/*0:0*/ __VdlySet__test_copper__DOT__dut__DOT__regs__v33;
    __VdlySet__test_copper__DOT__dut__DOT__regs__v33 = 0;
    CData/*7:0*/ __VdlyVal__test_copper__DOT__dut__DOT__regs__v34;
    __VdlyVal__test_copper__DOT__dut__DOT__regs__v34 = 0;
    CData/*0:0*/ __VdlySet__test_copper__DOT__dut__DOT__regs__v34;
    __VdlySet__test_copper__DOT__dut__DOT__regs__v34 = 0;
    CData/*7:0*/ __VdlyVal__test_copper__DOT__dut__DOT__regs__v35;
    __VdlyVal__test_copper__DOT__dut__DOT__regs__v35 = 0;
    CData/*0:0*/ __VdlySet__test_copper__DOT__dut__DOT__regs__v35;
    __VdlySet__test_copper__DOT__dut__DOT__regs__v35 = 0;
    CData/*7:0*/ __VdlyVal__test_copper__DOT__dut__DOT__regs__v36;
    __VdlyVal__test_copper__DOT__dut__DOT__regs__v36 = 0;
    CData/*0:0*/ __VdlySet__test_copper__DOT__dut__DOT__regs__v36;
    __VdlySet__test_copper__DOT__dut__DOT__regs__v36 = 0;
    CData/*1:0*/ __VdlyVal__test_copper__DOT__dut__DOT__spr_pri__v16;
    __VdlyVal__test_copper__DOT__dut__DOT__spr_pri__v16 = 0;
    CData/*3:0*/ __VdlyDim0__test_copper__DOT__dut__DOT__spr_pri__v16;
    __VdlyDim0__test_copper__DOT__dut__DOT__spr_pri__v16 = 0;
    CData/*0:0*/ __VdlySet__test_copper__DOT__dut__DOT__spr_pri__v16;
    __VdlySet__test_copper__DOT__dut__DOT__spr_pri__v16 = 0;
    CData/*0:0*/ __VdlyVal__test_copper__DOT__dut__DOT__spr_flip_h__v16;
    __VdlyVal__test_copper__DOT__dut__DOT__spr_flip_h__v16 = 0;
    CData/*3:0*/ __VdlyDim0__test_copper__DOT__dut__DOT__spr_flip_h__v16;
    __VdlyDim0__test_copper__DOT__dut__DOT__spr_flip_h__v16 = 0;
    CData/*0:0*/ __VdlySet__test_copper__DOT__dut__DOT__spr_flip_h__v16;
    __VdlySet__test_copper__DOT__dut__DOT__spr_flip_h__v16 = 0;
    CData/*0:0*/ __VdlyVal__test_copper__DOT__dut__DOT__spr_flip_v__v16;
    __VdlyVal__test_copper__DOT__dut__DOT__spr_flip_v__v16 = 0;
    CData/*3:0*/ __VdlyDim0__test_copper__DOT__dut__DOT__spr_flip_v__v16;
    __VdlyDim0__test_copper__DOT__dut__DOT__spr_flip_v__v16 = 0;
    CData/*3:0*/ __VdlyDim0__test_copper__DOT__dut__DOT__spr_enable__v16;
    __VdlyDim0__test_copper__DOT__dut__DOT__spr_enable__v16 = 0;
    CData/*0:0*/ __VdlySet__test_copper__DOT__dut__DOT__spr_enable__v16;
    __VdlySet__test_copper__DOT__dut__DOT__spr_enable__v16 = 0;
    CData/*3:0*/ __VdlyDim0__test_copper__DOT__dut__DOT__spr_enable__v17;
    __VdlyDim0__test_copper__DOT__dut__DOT__spr_enable__v17 = 0;
    CData/*0:0*/ __VdlySet__test_copper__DOT__dut__DOT__spr_enable__v17;
    __VdlySet__test_copper__DOT__dut__DOT__spr_enable__v17 = 0;
    SData/*8:0*/ __VdlyVal__test_copper__DOT__dut__DOT__spr_x__v16;
    __VdlyVal__test_copper__DOT__dut__DOT__spr_x__v16 = 0;
    CData/*3:0*/ __VdlyDim0__test_copper__DOT__dut__DOT__spr_x__v16;
    __VdlyDim0__test_copper__DOT__dut__DOT__spr_x__v16 = 0;
    CData/*0:0*/ __VdlySet__test_copper__DOT__dut__DOT__spr_x__v16;
    __VdlySet__test_copper__DOT__dut__DOT__spr_x__v16 = 0;
    CData/*7:0*/ __VdlyVal__test_copper__DOT__dut__DOT__spr_y__v16;
    __VdlyVal__test_copper__DOT__dut__DOT__spr_y__v16 = 0;
    CData/*3:0*/ __VdlyDim0__test_copper__DOT__dut__DOT__spr_y__v16;
    __VdlyDim0__test_copper__DOT__dut__DOT__spr_y__v16 = 0;
    CData/*7:0*/ __VdlyVal__test_copper__DOT__dut__DOT__sprite_shapes__v4;
    __VdlyVal__test_copper__DOT__dut__DOT__sprite_shapes__v4 = 0;
    SData/*10:0*/ __VdlyDim0__test_copper__DOT__dut__DOT__sprite_shapes__v4;
    __VdlyDim0__test_copper__DOT__dut__DOT__sprite_shapes__v4 = 0;
    CData/*7:0*/ __VdlyVal__test_copper__DOT__dut__DOT__sprite_shapes__v5;
    __VdlyVal__test_copper__DOT__dut__DOT__sprite_shapes__v5 = 0;
    SData/*10:0*/ __VdlyDim0__test_copper__DOT__dut__DOT__sprite_shapes__v5;
    __VdlyDim0__test_copper__DOT__dut__DOT__sprite_shapes__v5 = 0;
    CData/*7:0*/ __VdlyVal__test_copper__DOT__dut__DOT__sprite_shapes__v6;
    __VdlyVal__test_copper__DOT__dut__DOT__sprite_shapes__v6 = 0;
    SData/*10:0*/ __VdlyDim0__test_copper__DOT__dut__DOT__sprite_shapes__v6;
    __VdlyDim0__test_copper__DOT__dut__DOT__sprite_shapes__v6 = 0;
    CData/*7:0*/ __VdlyVal__test_copper__DOT__dut__DOT__sprite_shapes__v7;
    __VdlyVal__test_copper__DOT__dut__DOT__sprite_shapes__v7 = 0;
    SData/*10:0*/ __VdlyDim0__test_copper__DOT__dut__DOT__sprite_shapes__v7;
    __VdlyDim0__test_copper__DOT__dut__DOT__sprite_shapes__v7 = 0;
    CData/*7:0*/ __VdlyVal__test_copper__DOT__dut__DOT__sprite_shapes__v8;
    __VdlyVal__test_copper__DOT__dut__DOT__sprite_shapes__v8 = 0;
    SData/*10:0*/ __VdlyDim0__test_copper__DOT__dut__DOT__sprite_shapes__v8;
    __VdlyDim0__test_copper__DOT__dut__DOT__sprite_shapes__v8 = 0;
    CData/*7:0*/ __VdlyVal__test_copper__DOT__dut__DOT__sprite_shapes__v9;
    __VdlyVal__test_copper__DOT__dut__DOT__sprite_shapes__v9 = 0;
    SData/*10:0*/ __VdlyDim0__test_copper__DOT__dut__DOT__sprite_shapes__v9;
    __VdlyDim0__test_copper__DOT__dut__DOT__sprite_shapes__v9 = 0;
    CData/*7:0*/ __VdlyVal__test_copper__DOT__dut__DOT__sprite_shapes__v10;
    __VdlyVal__test_copper__DOT__dut__DOT__sprite_shapes__v10 = 0;
    SData/*10:0*/ __VdlyDim0__test_copper__DOT__dut__DOT__sprite_shapes__v10;
    __VdlyDim0__test_copper__DOT__dut__DOT__sprite_shapes__v10 = 0;
    CData/*7:0*/ __VdlyVal__test_copper__DOT__dut__DOT__sprite_shapes__v11;
    __VdlyVal__test_copper__DOT__dut__DOT__sprite_shapes__v11 = 0;
    SData/*10:0*/ __VdlyDim0__test_copper__DOT__dut__DOT__sprite_shapes__v11;
    __VdlyDim0__test_copper__DOT__dut__DOT__sprite_shapes__v11 = 0;
    CData/*7:0*/ __VdlyVal__test_copper__DOT__dut__DOT__sprite_shapes__v12;
    __VdlyVal__test_copper__DOT__dut__DOT__sprite_shapes__v12 = 0;
    SData/*10:0*/ __VdlyDim0__test_copper__DOT__dut__DOT__sprite_shapes__v12;
    __VdlyDim0__test_copper__DOT__dut__DOT__sprite_shapes__v12 = 0;
    IData/*16:0*/ __VdlyVal__test_copper__DOT__dut__DOT__fill_stack__v4;
    __VdlyVal__test_copper__DOT__dut__DOT__fill_stack__v4 = 0;
    CData/*0:0*/ __VdlySet__test_copper__DOT__dut__DOT__fill_stack__v4;
    __VdlySet__test_copper__DOT__dut__DOT__fill_stack__v4 = 0;
    CData/*3:0*/ __VdlyVal__test_copper__DOT__dut__DOT__gfx_ram__v16;
    __VdlyVal__test_copper__DOT__dut__DOT__gfx_ram__v16 = 0;
    SData/*15:0*/ __VdlyDim0__test_copper__DOT__dut__DOT__gfx_ram__v16;
    __VdlyDim0__test_copper__DOT__dut__DOT__gfx_ram__v16 = 0;
    CData/*3:0*/ __VdlyVal__test_copper__DOT__dut__DOT__gfx_ram__v17;
    __VdlyVal__test_copper__DOT__dut__DOT__gfx_ram__v17 = 0;
    SData/*15:0*/ __VdlyDim0__test_copper__DOT__dut__DOT__gfx_ram__v17;
    __VdlyDim0__test_copper__DOT__dut__DOT__gfx_ram__v17 = 0;
    CData/*7:0*/ __VdlyVal__test_copper__DOT__dut__DOT__regs__v37;
    __VdlyVal__test_copper__DOT__dut__DOT__regs__v37 = 0;
    CData/*4:0*/ __VdlyDim0__test_copper__DOT__dut__DOT__regs__v37;
    __VdlyDim0__test_copper__DOT__dut__DOT__regs__v37 = 0;
    CData/*0:0*/ __VdlySet__test_copper__DOT__dut__DOT__regs__v37;
    __VdlySet__test_copper__DOT__dut__DOT__regs__v37 = 0;
    CData/*7:0*/ __VdlyVal__test_copper__DOT__dut__DOT__char_ram__v6;
    __VdlyVal__test_copper__DOT__dut__DOT__char_ram__v6 = 0;
    SData/*10:0*/ __VdlyDim0__test_copper__DOT__dut__DOT__char_ram__v6;
    __VdlyDim0__test_copper__DOT__dut__DOT__char_ram__v6 = 0;
    CData/*7:0*/ __VdlyVal__test_copper__DOT__dut__DOT__fio_name__v0;
    __VdlyVal__test_copper__DOT__dut__DOT__fio_name__v0 = 0;
    CData/*5:0*/ __VdlyDim0__test_copper__DOT__dut__DOT__fio_name__v0;
    __VdlyDim0__test_copper__DOT__dut__DOT__fio_name__v0 = 0;
    CData/*0:0*/ __VdlySet__test_copper__DOT__dut__DOT__fio_name__v0;
    __VdlySet__test_copper__DOT__dut__DOT__fio_name__v0 = 0;
    CData/*7:0*/ __VdlyVal__test_copper__DOT__dut__DOT__color_ram__v5;
    __VdlyVal__test_copper__DOT__dut__DOT__color_ram__v5 = 0;
    SData/*10:0*/ __VdlyDim0__test_copper__DOT__dut__DOT__color_ram__v5;
    __VdlyDim0__test_copper__DOT__dut__DOT__color_ram__v5 = 0;
    CData/*3:0*/ __VdlyVal__test_copper__DOT__dut__DOT__spr_trans__v16;
    __VdlyVal__test_copper__DOT__dut__DOT__spr_trans__v16 = 0;
    CData/*3:0*/ __VdlyDim0__test_copper__DOT__dut__DOT__spr_trans__v16;
    __VdlyDim0__test_copper__DOT__dut__DOT__spr_trans__v16 = 0;
    CData/*0:0*/ __VdlySet__test_copper__DOT__dut__DOT__spr_trans__v16;
    __VdlySet__test_copper__DOT__dut__DOT__spr_trans__v16 = 0;
    CData/*1:0*/ __VdlyVal__test_copper__DOT__dut__DOT__spr_pri__v17;
    __VdlyVal__test_copper__DOT__dut__DOT__spr_pri__v17 = 0;
    CData/*3:0*/ __VdlyDim0__test_copper__DOT__dut__DOT__spr_pri__v17;
    __VdlyDim0__test_copper__DOT__dut__DOT__spr_pri__v17 = 0;
    CData/*0:0*/ __VdlySet__test_copper__DOT__dut__DOT__spr_pri__v17;
    __VdlySet__test_copper__DOT__dut__DOT__spr_pri__v17 = 0;
    CData/*0:0*/ __VdlyVal__test_copper__DOT__dut__DOT__spr_flip_h__v17;
    __VdlyVal__test_copper__DOT__dut__DOT__spr_flip_h__v17 = 0;
    CData/*3:0*/ __VdlyDim0__test_copper__DOT__dut__DOT__spr_flip_h__v17;
    __VdlyDim0__test_copper__DOT__dut__DOT__spr_flip_h__v17 = 0;
    CData/*0:0*/ __VdlySet__test_copper__DOT__dut__DOT__spr_flip_h__v17;
    __VdlySet__test_copper__DOT__dut__DOT__spr_flip_h__v17 = 0;
    CData/*0:0*/ __VdlyVal__test_copper__DOT__dut__DOT__spr_flip_v__v17;
    __VdlyVal__test_copper__DOT__dut__DOT__spr_flip_v__v17 = 0;
    CData/*3:0*/ __VdlyDim0__test_copper__DOT__dut__DOT__spr_flip_v__v17;
    __VdlyDim0__test_copper__DOT__dut__DOT__spr_flip_v__v17 = 0;
    CData/*0:0*/ __VdlyVal__test_copper__DOT__dut__DOT__spr_enable__v18;
    __VdlyVal__test_copper__DOT__dut__DOT__spr_enable__v18 = 0;
    CData/*3:0*/ __VdlyDim0__test_copper__DOT__dut__DOT__spr_enable__v18;
    __VdlyDim0__test_copper__DOT__dut__DOT__spr_enable__v18 = 0;
    CData/*3:0*/ __VdlyVal__test_copper__DOT__dut__DOT__spr_shape__v16;
    __VdlyVal__test_copper__DOT__dut__DOT__spr_shape__v16 = 0;
    CData/*3:0*/ __VdlyDim0__test_copper__DOT__dut__DOT__spr_shape__v16;
    __VdlyDim0__test_copper__DOT__dut__DOT__spr_shape__v16 = 0;
    CData/*0:0*/ __VdlySet__test_copper__DOT__dut__DOT__spr_shape__v16;
    __VdlySet__test_copper__DOT__dut__DOT__spr_shape__v16 = 0;
    CData/*7:0*/ __VdlyVal__test_copper__DOT__dut__DOT__spr_y__v17;
    __VdlyVal__test_copper__DOT__dut__DOT__spr_y__v17 = 0;
    CData/*3:0*/ __VdlyDim0__test_copper__DOT__dut__DOT__spr_y__v17;
    __VdlyDim0__test_copper__DOT__dut__DOT__spr_y__v17 = 0;
    CData/*0:0*/ __VdlySet__test_copper__DOT__dut__DOT__spr_y__v17;
    __VdlySet__test_copper__DOT__dut__DOT__spr_y__v17 = 0;
    CData/*0:0*/ __VdlyVal__test_copper__DOT__dut__DOT__spr_x__v17;
    __VdlyVal__test_copper__DOT__dut__DOT__spr_x__v17 = 0;
    CData/*3:0*/ __VdlyDim0__test_copper__DOT__dut__DOT__spr_x__v17;
    __VdlyDim0__test_copper__DOT__dut__DOT__spr_x__v17 = 0;
    CData/*0:0*/ __VdlySet__test_copper__DOT__dut__DOT__spr_x__v17;
    __VdlySet__test_copper__DOT__dut__DOT__spr_x__v17 = 0;
    CData/*7:0*/ __VdlyVal__test_copper__DOT__dut__DOT__spr_x__v18;
    __VdlyVal__test_copper__DOT__dut__DOT__spr_x__v18 = 0;
    CData/*3:0*/ __VdlyDim0__test_copper__DOT__dut__DOT__spr_x__v18;
    __VdlyDim0__test_copper__DOT__dut__DOT__spr_x__v18 = 0;
    CData/*0:0*/ __VdlySet__test_copper__DOT__dut__DOT__spr_x__v18;
    __VdlySet__test_copper__DOT__dut__DOT__spr_x__v18 = 0;
    CData/*7:0*/ __VdlyVal__test_copper__DOT__dut__DOT__regs__v38;
    __VdlyVal__test_copper__DOT__dut__DOT__regs__v38 = 0;
    CData/*4:0*/ __VdlyDim0__test_copper__DOT__dut__DOT__regs__v38;
    __VdlyDim0__test_copper__DOT__dut__DOT__regs__v38 = 0;
    CData/*0:0*/ __VdlySet__test_copper__DOT__dut__DOT__regs__v38;
    __VdlySet__test_copper__DOT__dut__DOT__regs__v38 = 0;
    CData/*7:0*/ __VdlyVal__test_copper__DOT__dut__DOT__tile_inst__DOT__nametable__v0;
    __VdlyVal__test_copper__DOT__dut__DOT__tile_inst__DOT__nametable__v0 = 0;
    SData/*11:0*/ __VdlyDim0__test_copper__DOT__dut__DOT__tile_inst__DOT__nametable__v0;
    __VdlyDim0__test_copper__DOT__dut__DOT__tile_inst__DOT__nametable__v0 = 0;
    CData/*7:0*/ __VdlyVal__test_copper__DOT__dut__DOT__tile_inst__DOT__attr_table__v0;
    __VdlyVal__test_copper__DOT__dut__DOT__tile_inst__DOT__attr_table__v0 = 0;
    SData/*11:0*/ __VdlyDim0__test_copper__DOT__dut__DOT__tile_inst__DOT__attr_table__v0;
    __VdlyDim0__test_copper__DOT__dut__DOT__tile_inst__DOT__attr_table__v0 = 0;
    CData/*0:0*/ __VdlySet__test_copper__DOT__dut__DOT__tile_inst__DOT__attr_table__v0;
    __VdlySet__test_copper__DOT__dut__DOT__tile_inst__DOT__attr_table__v0 = 0;
    CData/*7:0*/ __VdlyVal__test_copper__DOT__dut__DOT__tile_inst__DOT__attr_table__v1;
    __VdlyVal__test_copper__DOT__dut__DOT__tile_inst__DOT__attr_table__v1 = 0;
    SData/*11:0*/ __VdlyDim0__test_copper__DOT__dut__DOT__tile_inst__DOT__attr_table__v1;
    __VdlyDim0__test_copper__DOT__dut__DOT__tile_inst__DOT__attr_table__v1 = 0;
    CData/*0:0*/ __VdlySet__test_copper__DOT__dut__DOT__tile_inst__DOT__attr_table__v1;
    __VdlySet__test_copper__DOT__dut__DOT__tile_inst__DOT__attr_table__v1 = 0;
    CData/*7:0*/ __VdlyVal__test_copper__DOT__dut__DOT__tile_inst__DOT__attr_table__v2;
    __VdlyVal__test_copper__DOT__dut__DOT__tile_inst__DOT__attr_table__v2 = 0;
    SData/*11:0*/ __VdlyDim0__test_copper__DOT__dut__DOT__tile_inst__DOT__attr_table__v2;
    __VdlyDim0__test_copper__DOT__dut__DOT__tile_inst__DOT__attr_table__v2 = 0;
    CData/*0:0*/ __VdlySet__test_copper__DOT__dut__DOT__tile_inst__DOT__attr_table__v2;
    __VdlySet__test_copper__DOT__dut__DOT__tile_inst__DOT__attr_table__v2 = 0;
    CData/*7:0*/ __VdlyVal__test_copper__DOT__dut__DOT__tile_inst__DOT__nametable__v1;
    __VdlyVal__test_copper__DOT__dut__DOT__tile_inst__DOT__nametable__v1 = 0;
    SData/*11:0*/ __VdlyDim0__test_copper__DOT__dut__DOT__tile_inst__DOT__nametable__v1;
    __VdlyDim0__test_copper__DOT__dut__DOT__tile_inst__DOT__nametable__v1 = 0;
    CData/*7:0*/ __VdlyVal__test_copper__DOT__dut__DOT__tile_inst__DOT__nametable__v2;
    __VdlyVal__test_copper__DOT__dut__DOT__tile_inst__DOT__nametable__v2 = 0;
    SData/*11:0*/ __VdlyDim0__test_copper__DOT__dut__DOT__tile_inst__DOT__nametable__v2;
    __VdlyDim0__test_copper__DOT__dut__DOT__tile_inst__DOT__nametable__v2 = 0;
    SData/*11:0*/ __VdlyVal__test_copper__DOT__dut__DOT__tile_inst__DOT__pal_ram__v0;
    __VdlyVal__test_copper__DOT__dut__DOT__tile_inst__DOT__pal_ram__v0 = 0;
    CData/*7:0*/ __VdlyDim0__test_copper__DOT__dut__DOT__tile_inst__DOT__pal_ram__v0;
    __VdlyDim0__test_copper__DOT__dut__DOT__tile_inst__DOT__pal_ram__v0 = 0;
    CData/*0:0*/ __VdlySet__test_copper__DOT__dut__DOT__tile_inst__DOT__pal_ram__v0;
    __VdlySet__test_copper__DOT__dut__DOT__tile_inst__DOT__pal_ram__v0 = 0;
    CData/*7:0*/ __VdlyVal__test_copper__DOT__dut__DOT__tile_inst__DOT__attr_table__v3;
    __VdlyVal__test_copper__DOT__dut__DOT__tile_inst__DOT__attr_table__v3 = 0;
    SData/*11:0*/ __VdlyDim0__test_copper__DOT__dut__DOT__tile_inst__DOT__attr_table__v3;
    __VdlyDim0__test_copper__DOT__dut__DOT__tile_inst__DOT__attr_table__v3 = 0;
    CData/*0:0*/ __VdlySet__test_copper__DOT__dut__DOT__tile_inst__DOT__attr_table__v3;
    __VdlySet__test_copper__DOT__dut__DOT__tile_inst__DOT__attr_table__v3 = 0;
    CData/*7:0*/ __VdlyVal__test_copper__DOT__dut__DOT__tile_inst__DOT__nametable__v3;
    __VdlyVal__test_copper__DOT__dut__DOT__tile_inst__DOT__nametable__v3 = 0;
    SData/*11:0*/ __VdlyDim0__test_copper__DOT__dut__DOT__tile_inst__DOT__nametable__v3;
    __VdlyDim0__test_copper__DOT__dut__DOT__tile_inst__DOT__nametable__v3 = 0;
    SData/*14:0*/ __VdlyDim0__test_copper__DOT__dut__DOT__tile_inst__DOT__tile_data__v1;
    __VdlyDim0__test_copper__DOT__dut__DOT__tile_inst__DOT__tile_data__v1 = 0;
    CData/*0:0*/ __VdlySet__test_copper__DOT__dut__DOT__tile_inst__DOT__tile_data__v1;
    __VdlySet__test_copper__DOT__dut__DOT__tile_inst__DOT__tile_data__v1 = 0;
    CData/*7:0*/ __VdlyVal__test_copper__DOT__dut__DOT__tile_inst__DOT__nametable__v4;
    __VdlyVal__test_copper__DOT__dut__DOT__tile_inst__DOT__nametable__v4 = 0;
    SData/*11:0*/ __VdlyDim0__test_copper__DOT__dut__DOT__tile_inst__DOT__nametable__v4;
    __VdlyDim0__test_copper__DOT__dut__DOT__tile_inst__DOT__nametable__v4 = 0;
    CData/*7:0*/ __VdlyVal__test_copper__DOT__dut__DOT__tile_inst__DOT__nametable__v5;
    __VdlyVal__test_copper__DOT__dut__DOT__tile_inst__DOT__nametable__v5 = 0;
    SData/*11:0*/ __VdlyDim0__test_copper__DOT__dut__DOT__tile_inst__DOT__nametable__v5;
    __VdlyDim0__test_copper__DOT__dut__DOT__tile_inst__DOT__nametable__v5 = 0;
    CData/*7:0*/ __VdlyVal__test_copper__DOT__dut__DOT__tile_inst__DOT__nametable__v6;
    __VdlyVal__test_copper__DOT__dut__DOT__tile_inst__DOT__nametable__v6 = 0;
    SData/*11:0*/ __VdlyDim0__test_copper__DOT__dut__DOT__tile_inst__DOT__nametable__v6;
    __VdlyDim0__test_copper__DOT__dut__DOT__tile_inst__DOT__nametable__v6 = 0;
    CData/*7:0*/ __VdlyVal__test_copper__DOT__dut__DOT__tile_inst__DOT__nametable__v7;
    __VdlyVal__test_copper__DOT__dut__DOT__tile_inst__DOT__nametable__v7 = 0;
    SData/*11:0*/ __VdlyDim0__test_copper__DOT__dut__DOT__tile_inst__DOT__nametable__v7;
    __VdlyDim0__test_copper__DOT__dut__DOT__tile_inst__DOT__nametable__v7 = 0;
    CData/*7:0*/ __VdlyVal__test_copper__DOT__dut__DOT__tile_inst__DOT__nametable__v8;
    __VdlyVal__test_copper__DOT__dut__DOT__tile_inst__DOT__nametable__v8 = 0;
    SData/*11:0*/ __VdlyDim0__test_copper__DOT__dut__DOT__tile_inst__DOT__nametable__v8;
    __VdlyDim0__test_copper__DOT__dut__DOT__tile_inst__DOT__nametable__v8 = 0;
    CData/*7:0*/ __VdlyVal__test_copper__DOT__dut__DOT__tile_inst__DOT__nametable__v9;
    __VdlyVal__test_copper__DOT__dut__DOT__tile_inst__DOT__nametable__v9 = 0;
    SData/*11:0*/ __VdlyDim0__test_copper__DOT__dut__DOT__tile_inst__DOT__nametable__v9;
    __VdlyDim0__test_copper__DOT__dut__DOT__tile_inst__DOT__nametable__v9 = 0;
    CData/*7:0*/ __VdlyVal__test_copper__DOT__dut__DOT__tile_inst__DOT__nametable__v10;
    __VdlyVal__test_copper__DOT__dut__DOT__tile_inst__DOT__nametable__v10 = 0;
    SData/*11:0*/ __VdlyDim0__test_copper__DOT__dut__DOT__tile_inst__DOT__nametable__v10;
    __VdlyDim0__test_copper__DOT__dut__DOT__tile_inst__DOT__nametable__v10 = 0;
    CData/*7:0*/ __VdlyVal__test_copper__DOT__dut__DOT__tile_inst__DOT__nametable__v11;
    __VdlyVal__test_copper__DOT__dut__DOT__tile_inst__DOT__nametable__v11 = 0;
    SData/*11:0*/ __VdlyDim0__test_copper__DOT__dut__DOT__tile_inst__DOT__nametable__v11;
    __VdlyDim0__test_copper__DOT__dut__DOT__tile_inst__DOT__nametable__v11 = 0;
    CData/*7:0*/ __VdlyVal__test_copper__DOT__dut__DOT__tile_inst__DOT__nametable__v12;
    __VdlyVal__test_copper__DOT__dut__DOT__tile_inst__DOT__nametable__v12 = 0;
    SData/*11:0*/ __VdlyDim0__test_copper__DOT__dut__DOT__tile_inst__DOT__nametable__v12;
    __VdlyDim0__test_copper__DOT__dut__DOT__tile_inst__DOT__nametable__v12 = 0;
    CData/*7:0*/ __VdlyVal__test_copper__DOT__dut__DOT__tile_inst__DOT__nametable__v13;
    __VdlyVal__test_copper__DOT__dut__DOT__tile_inst__DOT__nametable__v13 = 0;
    SData/*11:0*/ __VdlyDim0__test_copper__DOT__dut__DOT__tile_inst__DOT__nametable__v13;
    __VdlyDim0__test_copper__DOT__dut__DOT__tile_inst__DOT__nametable__v13 = 0;
    CData/*7:0*/ __VdlyVal__test_copper__DOT__dut__DOT__tile_inst__DOT__nametable__v14;
    __VdlyVal__test_copper__DOT__dut__DOT__tile_inst__DOT__nametable__v14 = 0;
    SData/*11:0*/ __VdlyDim0__test_copper__DOT__dut__DOT__tile_inst__DOT__nametable__v14;
    __VdlyDim0__test_copper__DOT__dut__DOT__tile_inst__DOT__nametable__v14 = 0;
    CData/*7:0*/ __VdlyVal__test_copper__DOT__dut__DOT__tile_inst__DOT__nametable__v15;
    __VdlyVal__test_copper__DOT__dut__DOT__tile_inst__DOT__nametable__v15 = 0;
    SData/*11:0*/ __VdlyDim0__test_copper__DOT__dut__DOT__tile_inst__DOT__nametable__v15;
    __VdlyDim0__test_copper__DOT__dut__DOT__tile_inst__DOT__nametable__v15 = 0;
    CData/*7:0*/ __VdlyVal__test_copper__DOT__dut__DOT__tile_inst__DOT__nametable__v16;
    __VdlyVal__test_copper__DOT__dut__DOT__tile_inst__DOT__nametable__v16 = 0;
    SData/*11:0*/ __VdlyDim0__test_copper__DOT__dut__DOT__tile_inst__DOT__nametable__v16;
    __VdlyDim0__test_copper__DOT__dut__DOT__tile_inst__DOT__nametable__v16 = 0;
    CData/*7:0*/ __VdlyVal__test_copper__DOT__dut__DOT__tile_inst__DOT__nametable__v17;
    __VdlyVal__test_copper__DOT__dut__DOT__tile_inst__DOT__nametable__v17 = 0;
    SData/*11:0*/ __VdlyDim0__test_copper__DOT__dut__DOT__tile_inst__DOT__nametable__v17;
    __VdlyDim0__test_copper__DOT__dut__DOT__tile_inst__DOT__nametable__v17 = 0;
    CData/*7:0*/ __VdlyVal__test_copper__DOT__dut__DOT__tile_inst__DOT__nametable__v18;
    __VdlyVal__test_copper__DOT__dut__DOT__tile_inst__DOT__nametable__v18 = 0;
    SData/*11:0*/ __VdlyDim0__test_copper__DOT__dut__DOT__tile_inst__DOT__nametable__v18;
    __VdlyDim0__test_copper__DOT__dut__DOT__tile_inst__DOT__nametable__v18 = 0;
    CData/*7:0*/ __VdlyVal__test_copper__DOT__dut__DOT__tile_inst__DOT__nametable__v19;
    __VdlyVal__test_copper__DOT__dut__DOT__tile_inst__DOT__nametable__v19 = 0;
    SData/*11:0*/ __VdlyDim0__test_copper__DOT__dut__DOT__tile_inst__DOT__nametable__v19;
    __VdlyDim0__test_copper__DOT__dut__DOT__tile_inst__DOT__nametable__v19 = 0;
    CData/*7:0*/ __VdlyVal__test_copper__DOT__dut__DOT__tile_inst__DOT__nametable__v20;
    __VdlyVal__test_copper__DOT__dut__DOT__tile_inst__DOT__nametable__v20 = 0;
    SData/*11:0*/ __VdlyDim0__test_copper__DOT__dut__DOT__tile_inst__DOT__nametable__v20;
    __VdlyDim0__test_copper__DOT__dut__DOT__tile_inst__DOT__nametable__v20 = 0;
    CData/*7:0*/ __VdlyVal__test_copper__DOT__dut__DOT__tile_inst__DOT__nametable__v21;
    __VdlyVal__test_copper__DOT__dut__DOT__tile_inst__DOT__nametable__v21 = 0;
    SData/*11:0*/ __VdlyDim0__test_copper__DOT__dut__DOT__tile_inst__DOT__nametable__v21;
    __VdlyDim0__test_copper__DOT__dut__DOT__tile_inst__DOT__nametable__v21 = 0;
    CData/*7:0*/ __VdlyVal__test_copper__DOT__dut__DOT__tile_inst__DOT__nametable__v22;
    __VdlyVal__test_copper__DOT__dut__DOT__tile_inst__DOT__nametable__v22 = 0;
    SData/*11:0*/ __VdlyDim0__test_copper__DOT__dut__DOT__tile_inst__DOT__nametable__v22;
    __VdlyDim0__test_copper__DOT__dut__DOT__tile_inst__DOT__nametable__v22 = 0;
    CData/*7:0*/ __VdlyVal__test_copper__DOT__dut__DOT__tile_inst__DOT__nametable__v23;
    __VdlyVal__test_copper__DOT__dut__DOT__tile_inst__DOT__nametable__v23 = 0;
    SData/*11:0*/ __VdlyDim0__test_copper__DOT__dut__DOT__tile_inst__DOT__nametable__v23;
    __VdlyDim0__test_copper__DOT__dut__DOT__tile_inst__DOT__nametable__v23 = 0;
    CData/*7:0*/ __VdlyVal__test_copper__DOT__dut__DOT__tile_inst__DOT__nametable__v24;
    __VdlyVal__test_copper__DOT__dut__DOT__tile_inst__DOT__nametable__v24 = 0;
    SData/*11:0*/ __VdlyDim0__test_copper__DOT__dut__DOT__tile_inst__DOT__nametable__v24;
    __VdlyDim0__test_copper__DOT__dut__DOT__tile_inst__DOT__nametable__v24 = 0;
    CData/*7:0*/ __VdlyVal__test_copper__DOT__dut__DOT__tile_inst__DOT__nametable__v25;
    __VdlyVal__test_copper__DOT__dut__DOT__tile_inst__DOT__nametable__v25 = 0;
    SData/*11:0*/ __VdlyDim0__test_copper__DOT__dut__DOT__tile_inst__DOT__nametable__v25;
    __VdlyDim0__test_copper__DOT__dut__DOT__tile_inst__DOT__nametable__v25 = 0;
    CData/*7:0*/ __VdlyVal__test_copper__DOT__dut__DOT__tile_inst__DOT__nametable__v26;
    __VdlyVal__test_copper__DOT__dut__DOT__tile_inst__DOT__nametable__v26 = 0;
    SData/*11:0*/ __VdlyDim0__test_copper__DOT__dut__DOT__tile_inst__DOT__nametable__v26;
    __VdlyDim0__test_copper__DOT__dut__DOT__tile_inst__DOT__nametable__v26 = 0;
    CData/*7:0*/ __VdlyVal__test_copper__DOT__dut__DOT__tile_inst__DOT__nametable__v27;
    __VdlyVal__test_copper__DOT__dut__DOT__tile_inst__DOT__nametable__v27 = 0;
    SData/*11:0*/ __VdlyDim0__test_copper__DOT__dut__DOT__tile_inst__DOT__nametable__v27;
    __VdlyDim0__test_copper__DOT__dut__DOT__tile_inst__DOT__nametable__v27 = 0;
    CData/*7:0*/ __VdlyVal__test_copper__DOT__dut__DOT__tile_inst__DOT__nametable__v28;
    __VdlyVal__test_copper__DOT__dut__DOT__tile_inst__DOT__nametable__v28 = 0;
    SData/*11:0*/ __VdlyDim0__test_copper__DOT__dut__DOT__tile_inst__DOT__nametable__v28;
    __VdlyDim0__test_copper__DOT__dut__DOT__tile_inst__DOT__nametable__v28 = 0;
    CData/*7:0*/ __VdlyVal__test_copper__DOT__dut__DOT__tile_inst__DOT__col_buffer__v0;
    __VdlyVal__test_copper__DOT__dut__DOT__tile_inst__DOT__col_buffer__v0 = 0;
    CData/*0:0*/ __VdlySet__test_copper__DOT__dut__DOT__tile_inst__DOT__col_buffer__v0;
    __VdlySet__test_copper__DOT__dut__DOT__tile_inst__DOT__col_buffer__v0 = 0;
    CData/*7:0*/ __VdlyVal__test_copper__DOT__dut__DOT__tile_inst__DOT__col_buffer__v1;
    __VdlyVal__test_copper__DOT__dut__DOT__tile_inst__DOT__col_buffer__v1 = 0;
    CData/*0:0*/ __VdlySet__test_copper__DOT__dut__DOT__tile_inst__DOT__col_buffer__v1;
    __VdlySet__test_copper__DOT__dut__DOT__tile_inst__DOT__col_buffer__v1 = 0;
    CData/*7:0*/ __VdlyVal__test_copper__DOT__dut__DOT__tile_inst__DOT__col_buffer__v2;
    __VdlyVal__test_copper__DOT__dut__DOT__tile_inst__DOT__col_buffer__v2 = 0;
    CData/*0:0*/ __VdlySet__test_copper__DOT__dut__DOT__tile_inst__DOT__col_buffer__v2;
    __VdlySet__test_copper__DOT__dut__DOT__tile_inst__DOT__col_buffer__v2 = 0;
    CData/*7:0*/ __VdlyVal__test_copper__DOT__dut__DOT__tile_inst__DOT__col_buffer__v3;
    __VdlyVal__test_copper__DOT__dut__DOT__tile_inst__DOT__col_buffer__v3 = 0;
    CData/*0:0*/ __VdlySet__test_copper__DOT__dut__DOT__tile_inst__DOT__col_buffer__v3;
    __VdlySet__test_copper__DOT__dut__DOT__tile_inst__DOT__col_buffer__v3 = 0;
    CData/*7:0*/ __VdlyVal__test_copper__DOT__dut__DOT__tile_inst__DOT__col_buffer__v4;
    __VdlyVal__test_copper__DOT__dut__DOT__tile_inst__DOT__col_buffer__v4 = 0;
    CData/*0:0*/ __VdlySet__test_copper__DOT__dut__DOT__tile_inst__DOT__col_buffer__v4;
    __VdlySet__test_copper__DOT__dut__DOT__tile_inst__DOT__col_buffer__v4 = 0;
    CData/*7:0*/ __VdlyVal__test_copper__DOT__dut__DOT__tile_inst__DOT__col_buffer__v5;
    __VdlyVal__test_copper__DOT__dut__DOT__tile_inst__DOT__col_buffer__v5 = 0;
    CData/*0:0*/ __VdlySet__test_copper__DOT__dut__DOT__tile_inst__DOT__col_buffer__v5;
    __VdlySet__test_copper__DOT__dut__DOT__tile_inst__DOT__col_buffer__v5 = 0;
    CData/*7:0*/ __VdlyVal__test_copper__DOT__dut__DOT__tile_inst__DOT__col_buffer__v6;
    __VdlyVal__test_copper__DOT__dut__DOT__tile_inst__DOT__col_buffer__v6 = 0;
    CData/*0:0*/ __VdlySet__test_copper__DOT__dut__DOT__tile_inst__DOT__col_buffer__v6;
    __VdlySet__test_copper__DOT__dut__DOT__tile_inst__DOT__col_buffer__v6 = 0;
    CData/*7:0*/ __VdlyVal__test_copper__DOT__dut__DOT__tile_inst__DOT__col_buffer__v7;
    __VdlyVal__test_copper__DOT__dut__DOT__tile_inst__DOT__col_buffer__v7 = 0;
    CData/*0:0*/ __VdlySet__test_copper__DOT__dut__DOT__tile_inst__DOT__col_buffer__v7;
    __VdlySet__test_copper__DOT__dut__DOT__tile_inst__DOT__col_buffer__v7 = 0;
    CData/*7:0*/ __VdlyVal__test_copper__DOT__dut__DOT__tile_inst__DOT__col_buffer__v8;
    __VdlyVal__test_copper__DOT__dut__DOT__tile_inst__DOT__col_buffer__v8 = 0;
    CData/*0:0*/ __VdlySet__test_copper__DOT__dut__DOT__tile_inst__DOT__col_buffer__v8;
    __VdlySet__test_copper__DOT__dut__DOT__tile_inst__DOT__col_buffer__v8 = 0;
    CData/*7:0*/ __VdlyVal__test_copper__DOT__dut__DOT__tile_inst__DOT__col_buffer__v9;
    __VdlyVal__test_copper__DOT__dut__DOT__tile_inst__DOT__col_buffer__v9 = 0;
    CData/*0:0*/ __VdlySet__test_copper__DOT__dut__DOT__tile_inst__DOT__col_buffer__v9;
    __VdlySet__test_copper__DOT__dut__DOT__tile_inst__DOT__col_buffer__v9 = 0;
    CData/*7:0*/ __VdlyVal__test_copper__DOT__dut__DOT__tile_inst__DOT__col_buffer__v10;
    __VdlyVal__test_copper__DOT__dut__DOT__tile_inst__DOT__col_buffer__v10 = 0;
    CData/*0:0*/ __VdlySet__test_copper__DOT__dut__DOT__tile_inst__DOT__col_buffer__v10;
    __VdlySet__test_copper__DOT__dut__DOT__tile_inst__DOT__col_buffer__v10 = 0;
    CData/*7:0*/ __VdlyVal__test_copper__DOT__dut__DOT__tile_inst__DOT__col_buffer__v11;
    __VdlyVal__test_copper__DOT__dut__DOT__tile_inst__DOT__col_buffer__v11 = 0;
    CData/*0:0*/ __VdlySet__test_copper__DOT__dut__DOT__tile_inst__DOT__col_buffer__v11;
    __VdlySet__test_copper__DOT__dut__DOT__tile_inst__DOT__col_buffer__v11 = 0;
    CData/*7:0*/ __VdlyVal__test_copper__DOT__dut__DOT__tile_inst__DOT__col_buffer__v12;
    __VdlyVal__test_copper__DOT__dut__DOT__tile_inst__DOT__col_buffer__v12 = 0;
    CData/*0:0*/ __VdlySet__test_copper__DOT__dut__DOT__tile_inst__DOT__col_buffer__v12;
    __VdlySet__test_copper__DOT__dut__DOT__tile_inst__DOT__col_buffer__v12 = 0;
    CData/*7:0*/ __VdlyVal__test_copper__DOT__dut__DOT__tile_inst__DOT__col_buffer__v13;
    __VdlyVal__test_copper__DOT__dut__DOT__tile_inst__DOT__col_buffer__v13 = 0;
    CData/*0:0*/ __VdlySet__test_copper__DOT__dut__DOT__tile_inst__DOT__col_buffer__v13;
    __VdlySet__test_copper__DOT__dut__DOT__tile_inst__DOT__col_buffer__v13 = 0;
    CData/*7:0*/ __VdlyVal__test_copper__DOT__dut__DOT__tile_inst__DOT__col_buffer__v14;
    __VdlyVal__test_copper__DOT__dut__DOT__tile_inst__DOT__col_buffer__v14 = 0;
    CData/*0:0*/ __VdlySet__test_copper__DOT__dut__DOT__tile_inst__DOT__col_buffer__v14;
    __VdlySet__test_copper__DOT__dut__DOT__tile_inst__DOT__col_buffer__v14 = 0;
    CData/*7:0*/ __VdlyVal__test_copper__DOT__dut__DOT__tile_inst__DOT__col_buffer__v15;
    __VdlyVal__test_copper__DOT__dut__DOT__tile_inst__DOT__col_buffer__v15 = 0;
    CData/*0:0*/ __VdlySet__test_copper__DOT__dut__DOT__tile_inst__DOT__col_buffer__v15;
    __VdlySet__test_copper__DOT__dut__DOT__tile_inst__DOT__col_buffer__v15 = 0;
    CData/*7:0*/ __VdlyVal__test_copper__DOT__dut__DOT__tile_inst__DOT__col_buffer__v16;
    __VdlyVal__test_copper__DOT__dut__DOT__tile_inst__DOT__col_buffer__v16 = 0;
    CData/*0:0*/ __VdlySet__test_copper__DOT__dut__DOT__tile_inst__DOT__col_buffer__v16;
    __VdlySet__test_copper__DOT__dut__DOT__tile_inst__DOT__col_buffer__v16 = 0;
    CData/*7:0*/ __VdlyVal__test_copper__DOT__dut__DOT__tile_inst__DOT__col_buffer__v17;
    __VdlyVal__test_copper__DOT__dut__DOT__tile_inst__DOT__col_buffer__v17 = 0;
    CData/*0:0*/ __VdlySet__test_copper__DOT__dut__DOT__tile_inst__DOT__col_buffer__v17;
    __VdlySet__test_copper__DOT__dut__DOT__tile_inst__DOT__col_buffer__v17 = 0;
    CData/*7:0*/ __VdlyVal__test_copper__DOT__dut__DOT__tile_inst__DOT__col_buffer__v18;
    __VdlyVal__test_copper__DOT__dut__DOT__tile_inst__DOT__col_buffer__v18 = 0;
    CData/*0:0*/ __VdlySet__test_copper__DOT__dut__DOT__tile_inst__DOT__col_buffer__v18;
    __VdlySet__test_copper__DOT__dut__DOT__tile_inst__DOT__col_buffer__v18 = 0;
    CData/*7:0*/ __VdlyVal__test_copper__DOT__dut__DOT__tile_inst__DOT__col_buffer__v19;
    __VdlyVal__test_copper__DOT__dut__DOT__tile_inst__DOT__col_buffer__v19 = 0;
    CData/*0:0*/ __VdlySet__test_copper__DOT__dut__DOT__tile_inst__DOT__col_buffer__v19;
    __VdlySet__test_copper__DOT__dut__DOT__tile_inst__DOT__col_buffer__v19 = 0;
    CData/*7:0*/ __VdlyVal__test_copper__DOT__dut__DOT__tile_inst__DOT__col_buffer__v20;
    __VdlyVal__test_copper__DOT__dut__DOT__tile_inst__DOT__col_buffer__v20 = 0;
    CData/*0:0*/ __VdlySet__test_copper__DOT__dut__DOT__tile_inst__DOT__col_buffer__v20;
    __VdlySet__test_copper__DOT__dut__DOT__tile_inst__DOT__col_buffer__v20 = 0;
    CData/*7:0*/ __VdlyVal__test_copper__DOT__dut__DOT__tile_inst__DOT__col_buffer__v21;
    __VdlyVal__test_copper__DOT__dut__DOT__tile_inst__DOT__col_buffer__v21 = 0;
    CData/*0:0*/ __VdlySet__test_copper__DOT__dut__DOT__tile_inst__DOT__col_buffer__v21;
    __VdlySet__test_copper__DOT__dut__DOT__tile_inst__DOT__col_buffer__v21 = 0;
    CData/*7:0*/ __VdlyVal__test_copper__DOT__dut__DOT__tile_inst__DOT__col_buffer__v22;
    __VdlyVal__test_copper__DOT__dut__DOT__tile_inst__DOT__col_buffer__v22 = 0;
    CData/*0:0*/ __VdlySet__test_copper__DOT__dut__DOT__tile_inst__DOT__col_buffer__v22;
    __VdlySet__test_copper__DOT__dut__DOT__tile_inst__DOT__col_buffer__v22 = 0;
    CData/*7:0*/ __VdlyVal__test_copper__DOT__dut__DOT__tile_inst__DOT__col_buffer__v23;
    __VdlyVal__test_copper__DOT__dut__DOT__tile_inst__DOT__col_buffer__v23 = 0;
    CData/*0:0*/ __VdlySet__test_copper__DOT__dut__DOT__tile_inst__DOT__col_buffer__v23;
    __VdlySet__test_copper__DOT__dut__DOT__tile_inst__DOT__col_buffer__v23 = 0;
    CData/*7:0*/ __VdlyVal__test_copper__DOT__dut__DOT__tile_inst__DOT__col_buffer__v24;
    __VdlyVal__test_copper__DOT__dut__DOT__tile_inst__DOT__col_buffer__v24 = 0;
    CData/*0:0*/ __VdlySet__test_copper__DOT__dut__DOT__tile_inst__DOT__col_buffer__v24;
    __VdlySet__test_copper__DOT__dut__DOT__tile_inst__DOT__col_buffer__v24 = 0;
    CData/*7:0*/ __VdlyVal__test_copper__DOT__dut__DOT__tile_inst__DOT__col_buffer__v25;
    __VdlyVal__test_copper__DOT__dut__DOT__tile_inst__DOT__col_buffer__v25 = 0;
    CData/*4:0*/ __VdlyDim0__test_copper__DOT__dut__DOT__tile_inst__DOT__col_buffer__v25;
    __VdlyDim0__test_copper__DOT__dut__DOT__tile_inst__DOT__col_buffer__v25 = 0;
    CData/*0:0*/ __VdlySet__test_copper__DOT__dut__DOT__tile_inst__DOT__col_buffer__v25;
    __VdlySet__test_copper__DOT__dut__DOT__tile_inst__DOT__col_buffer__v25 = 0;
    CData/*7:0*/ __VdlyVal__test_copper__DOT__dut__DOT__tile_inst__DOT__col_buffer__v26;
    __VdlyVal__test_copper__DOT__dut__DOT__tile_inst__DOT__col_buffer__v26 = 0;
    CData/*0:0*/ __VdlySet__test_copper__DOT__dut__DOT__tile_inst__DOT__col_buffer__v26;
    __VdlySet__test_copper__DOT__dut__DOT__tile_inst__DOT__col_buffer__v26 = 0;
    CData/*7:0*/ __VdlyVal__test_copper__DOT__dut__DOT__tile_inst__DOT__col_buffer__v27;
    __VdlyVal__test_copper__DOT__dut__DOT__tile_inst__DOT__col_buffer__v27 = 0;
    CData/*0:0*/ __VdlySet__test_copper__DOT__dut__DOT__tile_inst__DOT__col_buffer__v27;
    __VdlySet__test_copper__DOT__dut__DOT__tile_inst__DOT__col_buffer__v27 = 0;
    CData/*7:0*/ __VdlyVal__test_copper__DOT__dut__DOT__tile_inst__DOT__col_buffer__v28;
    __VdlyVal__test_copper__DOT__dut__DOT__tile_inst__DOT__col_buffer__v28 = 0;
    CData/*0:0*/ __VdlySet__test_copper__DOT__dut__DOT__tile_inst__DOT__col_buffer__v28;
    __VdlySet__test_copper__DOT__dut__DOT__tile_inst__DOT__col_buffer__v28 = 0;
    CData/*7:0*/ __VdlyVal__test_copper__DOT__dut__DOT__tile_inst__DOT__col_buffer__v29;
    __VdlyVal__test_copper__DOT__dut__DOT__tile_inst__DOT__col_buffer__v29 = 0;
    CData/*0:0*/ __VdlySet__test_copper__DOT__dut__DOT__tile_inst__DOT__col_buffer__v29;
    __VdlySet__test_copper__DOT__dut__DOT__tile_inst__DOT__col_buffer__v29 = 0;
    CData/*7:0*/ __VdlyVal__test_copper__DOT__dut__DOT__tile_inst__DOT__col_buffer__v30;
    __VdlyVal__test_copper__DOT__dut__DOT__tile_inst__DOT__col_buffer__v30 = 0;
    CData/*0:0*/ __VdlySet__test_copper__DOT__dut__DOT__tile_inst__DOT__col_buffer__v30;
    __VdlySet__test_copper__DOT__dut__DOT__tile_inst__DOT__col_buffer__v30 = 0;
    CData/*7:0*/ __VdlyVal__test_copper__DOT__dut__DOT__tile_inst__DOT__col_buffer__v31;
    __VdlyVal__test_copper__DOT__dut__DOT__tile_inst__DOT__col_buffer__v31 = 0;
    CData/*0:0*/ __VdlySet__test_copper__DOT__dut__DOT__tile_inst__DOT__col_buffer__v31;
    __VdlySet__test_copper__DOT__dut__DOT__tile_inst__DOT__col_buffer__v31 = 0;
    CData/*7:0*/ __VdlyVal__test_copper__DOT__dut__DOT__tile_inst__DOT__col_buffer__v32;
    __VdlyVal__test_copper__DOT__dut__DOT__tile_inst__DOT__col_buffer__v32 = 0;
    CData/*0:0*/ __VdlySet__test_copper__DOT__dut__DOT__tile_inst__DOT__col_buffer__v32;
    __VdlySet__test_copper__DOT__dut__DOT__tile_inst__DOT__col_buffer__v32 = 0;
    CData/*7:0*/ __VdlyVal__test_copper__DOT__dut__DOT__tile_inst__DOT__col_buffer__v33;
    __VdlyVal__test_copper__DOT__dut__DOT__tile_inst__DOT__col_buffer__v33 = 0;
    CData/*0:0*/ __VdlySet__test_copper__DOT__dut__DOT__tile_inst__DOT__col_buffer__v33;
    __VdlySet__test_copper__DOT__dut__DOT__tile_inst__DOT__col_buffer__v33 = 0;
    CData/*7:0*/ __VdlyVal__test_copper__DOT__dut__DOT__tile_inst__DOT__col_buffer__v34;
    __VdlyVal__test_copper__DOT__dut__DOT__tile_inst__DOT__col_buffer__v34 = 0;
    CData/*0:0*/ __VdlySet__test_copper__DOT__dut__DOT__tile_inst__DOT__col_buffer__v34;
    __VdlySet__test_copper__DOT__dut__DOT__tile_inst__DOT__col_buffer__v34 = 0;
    CData/*7:0*/ __VdlyVal__test_copper__DOT__dut__DOT__tile_inst__DOT__col_buffer__v35;
    __VdlyVal__test_copper__DOT__dut__DOT__tile_inst__DOT__col_buffer__v35 = 0;
    CData/*0:0*/ __VdlySet__test_copper__DOT__dut__DOT__tile_inst__DOT__col_buffer__v35;
    __VdlySet__test_copper__DOT__dut__DOT__tile_inst__DOT__col_buffer__v35 = 0;
    CData/*7:0*/ __VdlyVal__test_copper__DOT__dut__DOT__tile_inst__DOT__col_buffer__v36;
    __VdlyVal__test_copper__DOT__dut__DOT__tile_inst__DOT__col_buffer__v36 = 0;
    CData/*0:0*/ __VdlySet__test_copper__DOT__dut__DOT__tile_inst__DOT__col_buffer__v36;
    __VdlySet__test_copper__DOT__dut__DOT__tile_inst__DOT__col_buffer__v36 = 0;
    CData/*7:0*/ __VdlyVal__test_copper__DOT__dut__DOT__tile_inst__DOT__col_buffer__v37;
    __VdlyVal__test_copper__DOT__dut__DOT__tile_inst__DOT__col_buffer__v37 = 0;
    CData/*0:0*/ __VdlySet__test_copper__DOT__dut__DOT__tile_inst__DOT__col_buffer__v37;
    __VdlySet__test_copper__DOT__dut__DOT__tile_inst__DOT__col_buffer__v37 = 0;
    CData/*7:0*/ __VdlyVal__test_copper__DOT__dut__DOT__tile_inst__DOT__col_buffer__v38;
    __VdlyVal__test_copper__DOT__dut__DOT__tile_inst__DOT__col_buffer__v38 = 0;
    CData/*0:0*/ __VdlySet__test_copper__DOT__dut__DOT__tile_inst__DOT__col_buffer__v38;
    __VdlySet__test_copper__DOT__dut__DOT__tile_inst__DOT__col_buffer__v38 = 0;
    CData/*7:0*/ __VdlyVal__test_copper__DOT__dut__DOT__tile_inst__DOT__col_buffer__v39;
    __VdlyVal__test_copper__DOT__dut__DOT__tile_inst__DOT__col_buffer__v39 = 0;
    CData/*0:0*/ __VdlySet__test_copper__DOT__dut__DOT__tile_inst__DOT__col_buffer__v39;
    __VdlySet__test_copper__DOT__dut__DOT__tile_inst__DOT__col_buffer__v39 = 0;
    CData/*7:0*/ __VdlyVal__test_copper__DOT__dut__DOT__tile_inst__DOT__col_buffer__v40;
    __VdlyVal__test_copper__DOT__dut__DOT__tile_inst__DOT__col_buffer__v40 = 0;
    CData/*0:0*/ __VdlySet__test_copper__DOT__dut__DOT__tile_inst__DOT__col_buffer__v40;
    __VdlySet__test_copper__DOT__dut__DOT__tile_inst__DOT__col_buffer__v40 = 0;
    CData/*7:0*/ __VdlyVal__test_copper__DOT__dut__DOT__tile_inst__DOT__col_buffer__v41;
    __VdlyVal__test_copper__DOT__dut__DOT__tile_inst__DOT__col_buffer__v41 = 0;
    CData/*0:0*/ __VdlySet__test_copper__DOT__dut__DOT__tile_inst__DOT__col_buffer__v41;
    __VdlySet__test_copper__DOT__dut__DOT__tile_inst__DOT__col_buffer__v41 = 0;
    CData/*7:0*/ __VdlyVal__test_copper__DOT__dut__DOT__tile_inst__DOT__col_buffer__v42;
    __VdlyVal__test_copper__DOT__dut__DOT__tile_inst__DOT__col_buffer__v42 = 0;
    CData/*0:0*/ __VdlySet__test_copper__DOT__dut__DOT__tile_inst__DOT__col_buffer__v42;
    __VdlySet__test_copper__DOT__dut__DOT__tile_inst__DOT__col_buffer__v42 = 0;
    CData/*7:0*/ __VdlyVal__test_copper__DOT__dut__DOT__tile_inst__DOT__col_buffer__v43;
    __VdlyVal__test_copper__DOT__dut__DOT__tile_inst__DOT__col_buffer__v43 = 0;
    CData/*0:0*/ __VdlySet__test_copper__DOT__dut__DOT__tile_inst__DOT__col_buffer__v43;
    __VdlySet__test_copper__DOT__dut__DOT__tile_inst__DOT__col_buffer__v43 = 0;
    CData/*7:0*/ __VdlyVal__test_copper__DOT__dut__DOT__tile_inst__DOT__col_buffer__v44;
    __VdlyVal__test_copper__DOT__dut__DOT__tile_inst__DOT__col_buffer__v44 = 0;
    CData/*0:0*/ __VdlySet__test_copper__DOT__dut__DOT__tile_inst__DOT__col_buffer__v44;
    __VdlySet__test_copper__DOT__dut__DOT__tile_inst__DOT__col_buffer__v44 = 0;
    CData/*7:0*/ __VdlyVal__test_copper__DOT__dut__DOT__tile_inst__DOT__col_buffer__v45;
    __VdlyVal__test_copper__DOT__dut__DOT__tile_inst__DOT__col_buffer__v45 = 0;
    CData/*0:0*/ __VdlySet__test_copper__DOT__dut__DOT__tile_inst__DOT__col_buffer__v45;
    __VdlySet__test_copper__DOT__dut__DOT__tile_inst__DOT__col_buffer__v45 = 0;
    CData/*7:0*/ __VdlyVal__test_copper__DOT__dut__DOT__tile_inst__DOT__col_buffer__v46;
    __VdlyVal__test_copper__DOT__dut__DOT__tile_inst__DOT__col_buffer__v46 = 0;
    CData/*0:0*/ __VdlySet__test_copper__DOT__dut__DOT__tile_inst__DOT__col_buffer__v46;
    __VdlySet__test_copper__DOT__dut__DOT__tile_inst__DOT__col_buffer__v46 = 0;
    CData/*7:0*/ __VdlyVal__test_copper__DOT__dut__DOT__tile_inst__DOT__col_buffer__v47;
    __VdlyVal__test_copper__DOT__dut__DOT__tile_inst__DOT__col_buffer__v47 = 0;
    CData/*0:0*/ __VdlySet__test_copper__DOT__dut__DOT__tile_inst__DOT__col_buffer__v47;
    __VdlySet__test_copper__DOT__dut__DOT__tile_inst__DOT__col_buffer__v47 = 0;
    CData/*7:0*/ __VdlyVal__test_copper__DOT__dut__DOT__tile_inst__DOT__col_buffer__v48;
    __VdlyVal__test_copper__DOT__dut__DOT__tile_inst__DOT__col_buffer__v48 = 0;
    CData/*0:0*/ __VdlySet__test_copper__DOT__dut__DOT__tile_inst__DOT__col_buffer__v48;
    __VdlySet__test_copper__DOT__dut__DOT__tile_inst__DOT__col_buffer__v48 = 0;
    CData/*7:0*/ __VdlyVal__test_copper__DOT__dut__DOT__tile_inst__DOT__col_buffer__v49;
    __VdlyVal__test_copper__DOT__dut__DOT__tile_inst__DOT__col_buffer__v49 = 0;
    CData/*0:0*/ __VdlySet__test_copper__DOT__dut__DOT__tile_inst__DOT__col_buffer__v49;
    __VdlySet__test_copper__DOT__dut__DOT__tile_inst__DOT__col_buffer__v49 = 0;
    CData/*7:0*/ __VdlyVal__test_copper__DOT__dut__DOT__tile_inst__DOT__col_buffer__v50;
    __VdlyVal__test_copper__DOT__dut__DOT__tile_inst__DOT__col_buffer__v50 = 0;
    CData/*0:0*/ __VdlySet__test_copper__DOT__dut__DOT__tile_inst__DOT__col_buffer__v50;
    __VdlySet__test_copper__DOT__dut__DOT__tile_inst__DOT__col_buffer__v50 = 0;
    SData/*11:0*/ __VdlyVal__test_copper__DOT__dut__DOT__tile_inst__DOT__pal_ram__v1;
    __VdlyVal__test_copper__DOT__dut__DOT__tile_inst__DOT__pal_ram__v1 = 0;
    CData/*7:0*/ __VdlyDim0__test_copper__DOT__dut__DOT__tile_inst__DOT__pal_ram__v1;
    __VdlyDim0__test_copper__DOT__dut__DOT__tile_inst__DOT__pal_ram__v1 = 0;
    CData/*0:0*/ __VdlySet__test_copper__DOT__dut__DOT__tile_inst__DOT__pal_ram__v1;
    __VdlySet__test_copper__DOT__dut__DOT__tile_inst__DOT__pal_ram__v1 = 0;
    CData/*7:0*/ __VdlyVal__test_copper__DOT__dut__DOT__tile_inst__DOT__nametable__v29;
    __VdlyVal__test_copper__DOT__dut__DOT__tile_inst__DOT__nametable__v29 = 0;
    SData/*11:0*/ __VdlyDim0__test_copper__DOT__dut__DOT__tile_inst__DOT__nametable__v29;
    __VdlyDim0__test_copper__DOT__dut__DOT__tile_inst__DOT__nametable__v29 = 0;
    CData/*7:0*/ __VdlyVal__test_copper__DOT__dut__DOT__tile_inst__DOT__attr_table__v4;
    __VdlyVal__test_copper__DOT__dut__DOT__tile_inst__DOT__attr_table__v4 = 0;
    SData/*11:0*/ __VdlyDim0__test_copper__DOT__dut__DOT__tile_inst__DOT__attr_table__v4;
    __VdlyDim0__test_copper__DOT__dut__DOT__tile_inst__DOT__attr_table__v4 = 0;
    CData/*0:0*/ __VdlySet__test_copper__DOT__dut__DOT__tile_inst__DOT__attr_table__v4;
    __VdlySet__test_copper__DOT__dut__DOT__tile_inst__DOT__attr_table__v4 = 0;
    CData/*7:0*/ __VdlyVal__test_copper__DOT__dut__DOT__tile_inst__DOT__nametable__v30;
    __VdlyVal__test_copper__DOT__dut__DOT__tile_inst__DOT__nametable__v30 = 0;
    SData/*11:0*/ __VdlyDim0__test_copper__DOT__dut__DOT__tile_inst__DOT__nametable__v30;
    __VdlyDim0__test_copper__DOT__dut__DOT__tile_inst__DOT__nametable__v30 = 0;
    CData/*7:0*/ __VdlyVal__test_copper__DOT__dut__DOT__tile_inst__DOT__tregs__v0;
    __VdlyVal__test_copper__DOT__dut__DOT__tile_inst__DOT__tregs__v0 = 0;
    CData/*4:0*/ __VdlyDim0__test_copper__DOT__dut__DOT__tile_inst__DOT__tregs__v0;
    __VdlyDim0__test_copper__DOT__dut__DOT__tile_inst__DOT__tregs__v0 = 0;
    CData/*0:0*/ __VdlySet__test_copper__DOT__dut__DOT__tile_inst__DOT__tregs__v0;
    __VdlySet__test_copper__DOT__dut__DOT__tile_inst__DOT__tregs__v0 = 0;
    // Body
    __VdlySet__test_copper__DOT__dut__DOT__tile_inst__DOT__tregs__v0 = 0U;
    __Vdly__test_copper__DOT__dut__DOT__h_count = vlSelfRef.test_copper__DOT__dut__DOT__h_count;
    __Vdly__test_copper__DOT__dut__DOT__v_count = vlSelfRef.test_copper__DOT__dut__DOT__v_count;
    __Vdly__test_copper__DOT__dut__DOT__cursor_x = vlSelfRef.test_copper__DOT__dut__DOT__cursor_x;
    __Vdly__test_copper__DOT__dut__DOT__cursor_y = vlSelfRef.test_copper__DOT__dut__DOT__cursor_y;
    __Vdly__test_copper__DOT__dut__DOT__gfx_color = vlSelfRef.test_copper__DOT__dut__DOT__gfx_color;
    __Vdly__test_copper__DOT__dut__DOT__scroll_pending 
        = vlSelfRef.test_copper__DOT__dut__DOT__scroll_pending;
    __Vdly__test_copper__DOT__dut__DOT__scroll_col 
        = vlSelfRef.test_copper__DOT__dut__DOT__scroll_col;
    __Vdly__test_copper__DOT__dut__DOT__cmd_busy = vlSelfRef.test_copper__DOT__dut__DOT__cmd_busy;
    __Vdly__test_copper__DOT__dut__DOT__copper_count 
        = vlSelfRef.test_copper__DOT__dut__DOT__copper_count;
    __Vdly__test_copper__DOT__dut__DOT__copper_target_list 
        = vlSelfRef.test_copper__DOT__dut__DOT__copper_target_list;
    __Vdly__test_copper__DOT__dut__DOT__copper_pending_list 
        = vlSelfRef.test_copper__DOT__dut__DOT__copper_pending_list;
    __Vdly__test_copper__DOT__dut__DOT__fill_sp = vlSelfRef.test_copper__DOT__dut__DOT__fill_sp;
    __Vdly__test_copper__DOT__dut__DOT__cmd_cx = vlSelfRef.test_copper__DOT__dut__DOT__cmd_cx;
    __Vdly__test_copper__DOT__dut__DOT__cmd_cy = vlSelfRef.test_copper__DOT__dut__DOT__cmd_cy;
    __Vdly__test_copper__DOT__dut__DOT__gt_font_row 
        = vlSelfRef.test_copper__DOT__dut__DOT__gt_font_row;
    __Vdly__test_copper__DOT__dut__DOT__gt_char_idx 
        = vlSelfRef.test_copper__DOT__dut__DOT__gt_char_idx;
    __Vdly__test_copper__DOT__dut__DOT__cmd_dx = vlSelfRef.test_copper__DOT__dut__DOT__cmd_dx;
    __Vdly__test_copper__DOT__dut__DOT__cmd_err = vlSelfRef.test_copper__DOT__dut__DOT__cmd_err;
    __Vdly__test_copper__DOT__dut__DOT__cmd_dy = vlSelfRef.test_copper__DOT__dut__DOT__cmd_dy;
    __Vdly__test_copper__DOT__dut__DOT__cmd_x = vlSelfRef.test_copper__DOT__dut__DOT__cmd_x;
    __Vdly__test_copper__DOT__dut__DOT__cmd_y = vlSelfRef.test_copper__DOT__dut__DOT__cmd_y;
    __VdlySet__test_copper__DOT__dut__DOT__fio_name__v0 = 0U;
    __VdlySet__test_copper__DOT__dut__DOT__spr_shape__v16 = 0U;
    __VdlySet__test_copper__DOT__dut__DOT__spr_trans__v16 = 0U;
    __VdlySet__test_copper__DOT__dut__DOT__tile_inst__DOT__tile_data__v0 = 0U;
    __VdlySet__test_copper__DOT__dut__DOT__spr_y__v17 = 0U;
    __VdlySet__test_copper__DOT__dut__DOT__spr_pri__v16 = 0U;
    __VdlySet__test_copper__DOT__dut__DOT__spr_pri__v17 = 0U;
    __VdlySet__test_copper__DOT__dut__DOT__spr_enable__v16 = 0U;
    __VdlySet__test_copper__DOT__dut__DOT__spr_enable__v17 = 0U;
    __Vdly__test_copper__DOT__dut__DOT__scroll_offset 
        = vlSelfRef.test_copper__DOT__dut__DOT__scroll_offset;
    __VdlySet__test_copper__DOT__dut__DOT__spr_x__v17 = 0U;
    __VdlySet__test_copper__DOT__dut__DOT__spr_x__v18 = 0U;
    __VdlySet__test_copper__DOT__dut__DOT__fill_stack__v0 = 0U;
    __VdlySet__test_copper__DOT__dut__DOT__fill_stack__v4 = 0U;
    __VdlySet__test_copper__DOT__dut__DOT__copper_list_pos__v0 = 0U;
    __VdlySet__test_copper__DOT__dut__DOT__regs__v32 = 0U;
    __VdlySet__test_copper__DOT__dut__DOT__regs__v33 = 0U;
    __VdlySet__test_copper__DOT__dut__DOT__regs__v34 = 0U;
    __VdlySet__test_copper__DOT__dut__DOT__regs__v35 = 0U;
    __VdlySet__test_copper__DOT__dut__DOT__regs__v36 = 0U;
    __VdlySet__test_copper__DOT__dut__DOT__regs__v37 = 0U;
    __VdlySet__test_copper__DOT__dut__DOT__spr_flip_h__v16 = 0U;
    __VdlySet__test_copper__DOT__dut__DOT__spr_x__v16 = 0U;
    __VdlySet__test_copper__DOT__dut__DOT__copper_pos__v0 = 0U;
    __VdlySet__test_copper__DOT__dut__DOT__copper_pos__v1 = 0U;
    __VdlySet__test_copper__DOT__dut__DOT__spr_flip_h__v17 = 0U;
    __VdlySet__test_copper__DOT__dut__DOT__regs__v0 = 0U;
    __VdlySet__test_copper__DOT__dut__DOT__regs__v1 = 0U;
    __Vdly__test_copper__DOT__dut__DOT__copper_loading 
        = vlSelfRef.test_copper__DOT__dut__DOT__copper_loading;
    __Vdly__test_copper__DOT__dut__DOT__copper_active_list 
        = vlSelfRef.test_copper__DOT__dut__DOT__copper_active_list;
    __VdlySet__test_copper__DOT__dut__DOT__regs__v38 = 0U;
    __Vdly__test_copper__DOT__dut__DOT__fg_color = vlSelfRef.test_copper__DOT__dut__DOT__fg_color;
    __Vdly__test_copper__DOT__dut__DOT__copper_index 
        = vlSelfRef.test_copper__DOT__dut__DOT__copper_index;
    __Vdly__test_copper__DOT__dut__DOT__tile_inst__DOT__dma_state 
        = vlSelfRef.test_copper__DOT__dut__DOT__tile_inst__DOT__dma_state;
    __Vdly__test_copper__DOT__dut__DOT__tile_inst__DOT__dma_data_valid 
        = vlSelfRef.test_copper__DOT__dut__DOT__tile_inst__DOT__dma_data_valid;
    __Vdly__test_copper__DOT__dut__DOT__tile_inst__DOT__dma_count 
        = vlSelfRef.test_copper__DOT__dut__DOT__tile_inst__DOT__dma_count;
    __Vdly__test_copper__DOT__dut__DOT__tile_inst__DOT__dma_col_idx 
        = vlSelfRef.test_copper__DOT__dut__DOT__tile_inst__DOT__dma_col_idx;
    __Vdly__test_copper__DOT__dut__DOT__tile_inst__DOT__dma_src 
        = vlSelfRef.test_copper__DOT__dut__DOT__tile_inst__DOT__dma_src;
    __Vdly__test_copper__DOT__dut__DOT__tile_inst__DOT__dma_dst 
        = vlSelfRef.test_copper__DOT__dut__DOT__tile_inst__DOT__dma_dst;
    __Vdly__test_copper__DOT__dut__DOT__tile_inst__DOT__dma_pal_r 
        = vlSelfRef.test_copper__DOT__dut__DOT__tile_inst__DOT__dma_pal_r;
    __Vdly__test_copper__DOT__dut__DOT__tile_inst__DOT__dma_pal_phase 
        = vlSelfRef.test_copper__DOT__dut__DOT__tile_inst__DOT__dma_pal_phase;
    __Vdly__test_copper__DOT__dut__DOT__tile_inst__DOT__dma_pal_g 
        = vlSelfRef.test_copper__DOT__dut__DOT__tile_inst__DOT__dma_pal_g;
    __VdlySet__test_copper__DOT__dut__DOT__tile_inst__DOT__tile_data__v1 = 0U;
    __VdlySet__test_copper__DOT__dut__DOT__tile_inst__DOT__pal_ram__v0 = 0U;
    __VdlySet__test_copper__DOT__dut__DOT__tile_inst__DOT__pal_ram__v1 = 0U;
    __VdlySet__test_copper__DOT__dut__DOT__tile_inst__DOT__attr_table__v0 = 0U;
    __VdlySet__test_copper__DOT__dut__DOT__tile_inst__DOT__attr_table__v1 = 0U;
    __VdlySet__test_copper__DOT__dut__DOT__tile_inst__DOT__attr_table__v2 = 0U;
    __VdlySet__test_copper__DOT__dut__DOT__tile_inst__DOT__attr_table__v3 = 0U;
    __VdlySet__test_copper__DOT__dut__DOT__tile_inst__DOT__attr_table__v4 = 0U;
    __VdlySet__test_copper__DOT__dut__DOT__tile_inst__DOT__col_buffer__v0 = 0U;
    __VdlySet__test_copper__DOT__dut__DOT__tile_inst__DOT__col_buffer__v1 = 0U;
    __VdlySet__test_copper__DOT__dut__DOT__tile_inst__DOT__col_buffer__v2 = 0U;
    __VdlySet__test_copper__DOT__dut__DOT__tile_inst__DOT__col_buffer__v3 = 0U;
    __VdlySet__test_copper__DOT__dut__DOT__tile_inst__DOT__col_buffer__v4 = 0U;
    __VdlySet__test_copper__DOT__dut__DOT__tile_inst__DOT__col_buffer__v5 = 0U;
    __VdlySet__test_copper__DOT__dut__DOT__tile_inst__DOT__col_buffer__v6 = 0U;
    __VdlySet__test_copper__DOT__dut__DOT__tile_inst__DOT__col_buffer__v7 = 0U;
    __VdlySet__test_copper__DOT__dut__DOT__tile_inst__DOT__col_buffer__v8 = 0U;
    __VdlySet__test_copper__DOT__dut__DOT__tile_inst__DOT__col_buffer__v9 = 0U;
    __VdlySet__test_copper__DOT__dut__DOT__tile_inst__DOT__col_buffer__v10 = 0U;
    __VdlySet__test_copper__DOT__dut__DOT__tile_inst__DOT__col_buffer__v11 = 0U;
    __VdlySet__test_copper__DOT__dut__DOT__tile_inst__DOT__col_buffer__v12 = 0U;
    __VdlySet__test_copper__DOT__dut__DOT__tile_inst__DOT__col_buffer__v13 = 0U;
    __VdlySet__test_copper__DOT__dut__DOT__tile_inst__DOT__col_buffer__v14 = 0U;
    __VdlySet__test_copper__DOT__dut__DOT__tile_inst__DOT__col_buffer__v15 = 0U;
    __VdlySet__test_copper__DOT__dut__DOT__tile_inst__DOT__col_buffer__v16 = 0U;
    __VdlySet__test_copper__DOT__dut__DOT__tile_inst__DOT__col_buffer__v17 = 0U;
    __VdlySet__test_copper__DOT__dut__DOT__tile_inst__DOT__col_buffer__v18 = 0U;
    __VdlySet__test_copper__DOT__dut__DOT__tile_inst__DOT__col_buffer__v19 = 0U;
    __VdlySet__test_copper__DOT__dut__DOT__tile_inst__DOT__col_buffer__v20 = 0U;
    __VdlySet__test_copper__DOT__dut__DOT__tile_inst__DOT__col_buffer__v21 = 0U;
    __VdlySet__test_copper__DOT__dut__DOT__tile_inst__DOT__col_buffer__v22 = 0U;
    __VdlySet__test_copper__DOT__dut__DOT__tile_inst__DOT__col_buffer__v23 = 0U;
    __VdlySet__test_copper__DOT__dut__DOT__tile_inst__DOT__col_buffer__v24 = 0U;
    __VdlySet__test_copper__DOT__dut__DOT__tile_inst__DOT__col_buffer__v25 = 0U;
    __VdlySet__test_copper__DOT__dut__DOT__tile_inst__DOT__col_buffer__v26 = 0U;
    __VdlySet__test_copper__DOT__dut__DOT__tile_inst__DOT__col_buffer__v27 = 0U;
    __VdlySet__test_copper__DOT__dut__DOT__tile_inst__DOT__col_buffer__v28 = 0U;
    __VdlySet__test_copper__DOT__dut__DOT__tile_inst__DOT__col_buffer__v29 = 0U;
    __VdlySet__test_copper__DOT__dut__DOT__tile_inst__DOT__col_buffer__v30 = 0U;
    __VdlySet__test_copper__DOT__dut__DOT__tile_inst__DOT__col_buffer__v31 = 0U;
    __VdlySet__test_copper__DOT__dut__DOT__tile_inst__DOT__col_buffer__v32 = 0U;
    __VdlySet__test_copper__DOT__dut__DOT__tile_inst__DOT__col_buffer__v33 = 0U;
    __VdlySet__test_copper__DOT__dut__DOT__tile_inst__DOT__col_buffer__v34 = 0U;
    __VdlySet__test_copper__DOT__dut__DOT__tile_inst__DOT__col_buffer__v35 = 0U;
    __VdlySet__test_copper__DOT__dut__DOT__tile_inst__DOT__col_buffer__v36 = 0U;
    __VdlySet__test_copper__DOT__dut__DOT__tile_inst__DOT__col_buffer__v37 = 0U;
    __VdlySet__test_copper__DOT__dut__DOT__tile_inst__DOT__col_buffer__v38 = 0U;
    __VdlySet__test_copper__DOT__dut__DOT__tile_inst__DOT__col_buffer__v39 = 0U;
    __VdlySet__test_copper__DOT__dut__DOT__tile_inst__DOT__col_buffer__v40 = 0U;
    __VdlySet__test_copper__DOT__dut__DOT__tile_inst__DOT__col_buffer__v41 = 0U;
    __VdlySet__test_copper__DOT__dut__DOT__tile_inst__DOT__col_buffer__v42 = 0U;
    __VdlySet__test_copper__DOT__dut__DOT__tile_inst__DOT__col_buffer__v43 = 0U;
    __VdlySet__test_copper__DOT__dut__DOT__tile_inst__DOT__col_buffer__v44 = 0U;
    __VdlySet__test_copper__DOT__dut__DOT__tile_inst__DOT__col_buffer__v45 = 0U;
    __VdlySet__test_copper__DOT__dut__DOT__tile_inst__DOT__col_buffer__v46 = 0U;
    __VdlySet__test_copper__DOT__dut__DOT__tile_inst__DOT__col_buffer__v47 = 0U;
    __VdlySet__test_copper__DOT__dut__DOT__tile_inst__DOT__col_buffer__v48 = 0U;
    __VdlySet__test_copper__DOT__dut__DOT__tile_inst__DOT__col_buffer__v49 = 0U;
    __VdlySet__test_copper__DOT__dut__DOT__tile_inst__DOT__col_buffer__v50 = 0U;
    if ((1U & (~ (IData)(vlSelfRef.test_copper__DOT__rst)))) {
        if (((IData)(vlSelfRef.test_copper__DOT__cpu_we) 
             & (IData)(vlSelfRef.test_copper__DOT__dut__DOT__tile_reg_sel))) {
            __VdlyVal__test_copper__DOT__dut__DOT__tile_inst__DOT__tregs__v0 
                = vlSelfRef.test_copper__DOT__cpu_wdata;
            __VdlyDim0__test_copper__DOT__dut__DOT__tile_inst__DOT__tregs__v0 
                = (0x0000001fU & (IData)(vlSelfRef.test_copper__DOT__cpu_addr));
            __VdlySet__test_copper__DOT__dut__DOT__tile_inst__DOT__tregs__v0 = 1U;
        }
    }
    if (vlSelfRef.test_copper__DOT__rst) {
        __Vdly__test_copper__DOT__dut__DOT__h_count = 0U;
        __Vdly__test_copper__DOT__dut__DOT__v_count = 0U;
    } else if ((0x031fU == (IData)(vlSelfRef.test_copper__DOT__dut__DOT__h_count))) {
        __Vdly__test_copper__DOT__dut__DOT__v_count 
            = ((0x020cU == (IData)(vlSelfRef.test_copper__DOT__dut__DOT__v_count))
                ? 0U : (0x000003ffU & ((IData)(1U) 
                                       + (IData)(vlSelfRef.test_copper__DOT__dut__DOT__v_count))));
        __Vdly__test_copper__DOT__dut__DOT__h_count = 0U;
    } else {
        __Vdly__test_copper__DOT__dut__DOT__h_count 
            = (0x000003ffU & ((IData)(1U) + (IData)(vlSelfRef.test_copper__DOT__dut__DOT__h_count)));
    }
    if (vlSelfRef.test_copper__DOT__rst) {
        __Vdly__test_copper__DOT__dut__DOT__tile_inst__DOT__dma_state = 0U;
        __Vdly__test_copper__DOT__dut__DOT__tile_inst__DOT__dma_data_valid = 0U;
    } else {
        __Vdly__test_copper__DOT__dut__DOT__tile_inst__DOT__dma_data_valid = 0U;
        if ((8U & (IData)(vlSelfRef.test_copper__DOT__dut__DOT__tile_inst__DOT__dma_state))) {
            if ((4U & (IData)(vlSelfRef.test_copper__DOT__dut__DOT__tile_inst__DOT__dma_state))) {
                __Vdly__test_copper__DOT__dut__DOT__tile_inst__DOT__dma_state = 0U;
            } else if ((2U & (IData)(vlSelfRef.test_copper__DOT__dut__DOT__tile_inst__DOT__dma_state))) {
                __Vdly__test_copper__DOT__dut__DOT__tile_inst__DOT__dma_state = 0U;
            } else if ((1U & (IData)(vlSelfRef.test_copper__DOT__dut__DOT__tile_inst__DOT__dma_state))) {
                if ((0x0fa0U > (IData)(vlSelfRef.test_copper__DOT__dut__DOT__tile_inst__DOT__dma_count))) {
                    vlSelfRef.test_copper__DOT__dut__DOT__tile_inst__DOT____Vlvbound_hf26d05a2__0 = 0U;
                    if ((0x0f9fU >= (0x00000fffU & (IData)(vlSelfRef.test_copper__DOT__dut__DOT__tile_inst__DOT__dma_count)))) {
                        __VdlyVal__test_copper__DOT__dut__DOT__tile_inst__DOT__nametable__v0 
                            = vlSelfRef.test_copper__DOT__dut__DOT__tile_inst__DOT____Vlvbound_hf26d05a2__0;
                        __VdlyDim0__test_copper__DOT__dut__DOT__tile_inst__DOT__nametable__v0 
                            = (0x00000fffU & (IData)(vlSelfRef.test_copper__DOT__dut__DOT__tile_inst__DOT__dma_count));
                        vlSelfRef.__VdlyCommitQueuetest_copper__DOT__dut__DOT__tile_inst__DOT__nametable.enqueue(__VdlyVal__test_copper__DOT__dut__DOT__tile_inst__DOT__nametable__v0, (IData)(__VdlyDim0__test_copper__DOT__dut__DOT__tile_inst__DOT__nametable__v0));
                    }
                    vlSelfRef.test_copper__DOT__dut__DOT__tile_inst__DOT____Vlvbound_hf7076230__0 = 0U;
                    if ((0x0f9fU >= (0x00000fffU & (IData)(vlSelfRef.test_copper__DOT__dut__DOT__tile_inst__DOT__dma_count)))) {
                        __VdlyVal__test_copper__DOT__dut__DOT__tile_inst__DOT__attr_table__v0 
                            = vlSelfRef.test_copper__DOT__dut__DOT__tile_inst__DOT____Vlvbound_hf7076230__0;
                        __VdlyDim0__test_copper__DOT__dut__DOT__tile_inst__DOT__attr_table__v0 
                            = (0x00000fffU & (IData)(vlSelfRef.test_copper__DOT__dut__DOT__tile_inst__DOT__dma_count));
                        __VdlySet__test_copper__DOT__dut__DOT__tile_inst__DOT__attr_table__v0 = 1U;
                    }
                    __Vdly__test_copper__DOT__dut__DOT__tile_inst__DOT__dma_count 
                        = (0x0000ffffU & ((IData)(1U) 
                                          + (IData)(vlSelfRef.test_copper__DOT__dut__DOT__tile_inst__DOT__dma_count)));
                } else {
                    __Vdly__test_copper__DOT__dut__DOT__tile_inst__DOT__dma_state = 0U;
                }
            } else {
                if (vlSelfRef.test_copper__DOT__dut__DOT__tile_inst__DOT__dma_data_valid) {
                    vlSelfRef.test_copper__DOT__dut__DOT__tile_inst__DOT____Vlvbound_h8c784627__0 = 0U;
                    __Vdly__test_copper__DOT__dut__DOT__tile_inst__DOT__dma_count 
                        = (0x0000ffffU & ((IData)(vlSelfRef.test_copper__DOT__dut__DOT__tile_inst__DOT__dma_count) 
                                          - (IData)(1U)));
                    if ((0x0f9fU >= (0x00000fffU & 
                                     ((((IData)(0x000003e8U) 
                                        * (IData)(vlSelfRef.test_copper__DOT__dut__DOT__tile_inst__DOT__dma_nt)) 
                                       + ((IData)(0x00000028U) 
                                          * (IData)(vlSelfRef.test_copper__DOT__dut__DOT__tile_inst__DOT__dma_col_idx))) 
                                      + (IData)(vlSelfRef.test_copper__DOT__dut__DOT__tile_inst__DOT__dma_col_row))))) {
                        __VdlyVal__test_copper__DOT__dut__DOT__tile_inst__DOT__attr_table__v1 
                            = vlSelfRef.test_copper__DOT__dut__DOT__tile_inst__DOT____Vlvbound_h8c784627__0;
                        __VdlyDim0__test_copper__DOT__dut__DOT__tile_inst__DOT__attr_table__v1 
                            = (0x00000fffU & ((((IData)(0x000003e8U) 
                                                * (IData)(vlSelfRef.test_copper__DOT__dut__DOT__tile_inst__DOT__dma_nt)) 
                                               + ((IData)(0x00000028U) 
                                                  * (IData)(vlSelfRef.test_copper__DOT__dut__DOT__tile_inst__DOT__dma_col_idx))) 
                                              + (IData)(vlSelfRef.test_copper__DOT__dut__DOT__tile_inst__DOT__dma_col_row)));
                        __VdlySet__test_copper__DOT__dut__DOT__tile_inst__DOT__attr_table__v1 = 1U;
                    }
                    __Vdly__test_copper__DOT__dut__DOT__tile_inst__DOT__dma_col_idx 
                        = (0x0000001fU & ((IData)(1U) 
                                          + (IData)(vlSelfRef.test_copper__DOT__dut__DOT__tile_inst__DOT__dma_col_idx)));
                }
                if (((0U == (IData)(vlSelfRef.test_copper__DOT__dut__DOT__tile_inst__DOT__dma_count)) 
                     & (~ (IData)(vlSelfRef.test_copper__DOT__dut__DOT__tile_inst__DOT__dma_data_valid)))) {
                    __Vdly__test_copper__DOT__dut__DOT__tile_inst__DOT__dma_state = 0U;
                } else if ((1U & ((~ (IData)(vlSelfRef.test_copper__DOT__dut__DOT__tile_inst__DOT__dma_data_valid)) 
                                  | (1U < (IData)(vlSelfRef.test_copper__DOT__dut__DOT__tile_inst__DOT__dma_count))))) {
                    __Vdly__test_copper__DOT__dut__DOT__tile_inst__DOT__dma_src 
                        = (0x0000ffffU & ((IData)(1U) 
                                          + (IData)(vlSelfRef.test_copper__DOT__dut__DOT__tile_inst__DOT__dma_src)));
                    __Vdly__test_copper__DOT__dut__DOT__tile_inst__DOT__dma_data_valid = 1U;
                }
            }
        } else if ((4U & (IData)(vlSelfRef.test_copper__DOT__dut__DOT__tile_inst__DOT__dma_state))) {
            if ((2U & (IData)(vlSelfRef.test_copper__DOT__dut__DOT__tile_inst__DOT__dma_state))) {
                if ((1U & (IData)(vlSelfRef.test_copper__DOT__dut__DOT__tile_inst__DOT__dma_state))) {
                    if (vlSelfRef.test_copper__DOT__dut__DOT__tile_inst__DOT__dma_data_valid) {
                        vlSelfRef.test_copper__DOT__dut__DOT__tile_inst__DOT____Vlvbound_h02aa0280__0 = 0U;
                        __Vdly__test_copper__DOT__dut__DOT__tile_inst__DOT__dma_count 
                            = (0x0000ffffU & ((IData)(vlSelfRef.test_copper__DOT__dut__DOT__tile_inst__DOT__dma_count) 
                                              - (IData)(1U)));
                        if ((0x0f9fU >= (0x00000fffU 
                                         & ((((IData)(0x000003e8U) 
                                              * (IData)(vlSelfRef.test_copper__DOT__dut__DOT__tile_inst__DOT__dma_nt)) 
                                             + ((IData)(0x00000028U) 
                                                * (IData)(vlSelfRef.test_copper__DOT__dut__DOT__tile_inst__DOT__dma_col_row))) 
                                            + (0x0000003fU 
                                               & (IData)(vlSelfRef.test_copper__DOT__dut__DOT__tile_inst__DOT__dma_dst)))))) {
                            __VdlyVal__test_copper__DOT__dut__DOT__tile_inst__DOT__attr_table__v2 
                                = vlSelfRef.test_copper__DOT__dut__DOT__tile_inst__DOT____Vlvbound_h02aa0280__0;
                            __VdlyDim0__test_copper__DOT__dut__DOT__tile_inst__DOT__attr_table__v2 
                                = (0x00000fffU & ((
                                                   ((IData)(0x000003e8U) 
                                                    * (IData)(vlSelfRef.test_copper__DOT__dut__DOT__tile_inst__DOT__dma_nt)) 
                                                   + 
                                                   ((IData)(0x00000028U) 
                                                    * (IData)(vlSelfRef.test_copper__DOT__dut__DOT__tile_inst__DOT__dma_col_row))) 
                                                  + 
                                                  (0x0000003fU 
                                                   & (IData)(vlSelfRef.test_copper__DOT__dut__DOT__tile_inst__DOT__dma_dst))));
                            __VdlySet__test_copper__DOT__dut__DOT__tile_inst__DOT__attr_table__v2 = 1U;
                        }
                        __Vdly__test_copper__DOT__dut__DOT__tile_inst__DOT__dma_dst 
                            = (0x0000ffffU & ((IData)(1U) 
                                              + (IData)(vlSelfRef.test_copper__DOT__dut__DOT__tile_inst__DOT__dma_dst)));
                    }
                    if (((0U == (IData)(vlSelfRef.test_copper__DOT__dut__DOT__tile_inst__DOT__dma_count)) 
                         & (~ (IData)(vlSelfRef.test_copper__DOT__dut__DOT__tile_inst__DOT__dma_data_valid)))) {
                        __Vdly__test_copper__DOT__dut__DOT__tile_inst__DOT__dma_state = 0U;
                    } else if ((1U & ((~ (IData)(vlSelfRef.test_copper__DOT__dut__DOT__tile_inst__DOT__dma_data_valid)) 
                                      | (1U < (IData)(vlSelfRef.test_copper__DOT__dut__DOT__tile_inst__DOT__dma_count))))) {
                        __Vdly__test_copper__DOT__dut__DOT__tile_inst__DOT__dma_src 
                            = (0x0000ffffU & ((IData)(1U) 
                                              + (IData)(vlSelfRef.test_copper__DOT__dut__DOT__tile_inst__DOT__dma_src)));
                        __Vdly__test_copper__DOT__dut__DOT__tile_inst__DOT__dma_data_valid = 1U;
                    }
                } else {
                    if (vlSelfRef.test_copper__DOT__dut__DOT__tile_inst__DOT__dma_data_valid) {
                        vlSelfRef.test_copper__DOT__dut__DOT__tile_inst__DOT____Vlvbound_h21da2949__0 = 0U;
                        __Vdly__test_copper__DOT__dut__DOT__tile_inst__DOT__dma_count 
                            = (0x0000ffffU & ((IData)(vlSelfRef.test_copper__DOT__dut__DOT__tile_inst__DOT__dma_count) 
                                              - (IData)(1U)));
                        if ((0x0f9fU >= (0x00000fffU 
                                         & ((((IData)(0x000003e8U) 
                                              * (IData)(vlSelfRef.test_copper__DOT__dut__DOT__tile_inst__DOT__dma_nt)) 
                                             + ((IData)(0x00000028U) 
                                                * (IData)(vlSelfRef.test_copper__DOT__dut__DOT__tile_inst__DOT__dma_col_idx))) 
                                            + (IData)(vlSelfRef.test_copper__DOT__dut__DOT__tile_inst__DOT__dma_col_row))))) {
                            __VdlyVal__test_copper__DOT__dut__DOT__tile_inst__DOT__nametable__v1 
                                = vlSelfRef.test_copper__DOT__dut__DOT__tile_inst__DOT____Vlvbound_h21da2949__0;
                            __VdlyDim0__test_copper__DOT__dut__DOT__tile_inst__DOT__nametable__v1 
                                = (0x00000fffU & ((
                                                   ((IData)(0x000003e8U) 
                                                    * (IData)(vlSelfRef.test_copper__DOT__dut__DOT__tile_inst__DOT__dma_nt)) 
                                                   + 
                                                   ((IData)(0x00000028U) 
                                                    * (IData)(vlSelfRef.test_copper__DOT__dut__DOT__tile_inst__DOT__dma_col_idx))) 
                                                  + (IData)(vlSelfRef.test_copper__DOT__dut__DOT__tile_inst__DOT__dma_col_row)));
                            vlSelfRef.__VdlyCommitQueuetest_copper__DOT__dut__DOT__tile_inst__DOT__nametable.enqueue(__VdlyVal__test_copper__DOT__dut__DOT__tile_inst__DOT__nametable__v1, (IData)(__VdlyDim0__test_copper__DOT__dut__DOT__tile_inst__DOT__nametable__v1));
                        }
                        __Vdly__test_copper__DOT__dut__DOT__tile_inst__DOT__dma_col_idx 
                            = (0x0000001fU & ((IData)(1U) 
                                              + (IData)(vlSelfRef.test_copper__DOT__dut__DOT__tile_inst__DOT__dma_col_idx)));
                    }
                    if (((0U == (IData)(vlSelfRef.test_copper__DOT__dut__DOT__tile_inst__DOT__dma_count)) 
                         & (~ (IData)(vlSelfRef.test_copper__DOT__dut__DOT__tile_inst__DOT__dma_data_valid)))) {
                        __Vdly__test_copper__DOT__dut__DOT__tile_inst__DOT__dma_state = 0U;
                    } else if ((1U & ((~ (IData)(vlSelfRef.test_copper__DOT__dut__DOT__tile_inst__DOT__dma_data_valid)) 
                                      | (1U < (IData)(vlSelfRef.test_copper__DOT__dut__DOT__tile_inst__DOT__dma_count))))) {
                        __Vdly__test_copper__DOT__dut__DOT__tile_inst__DOT__dma_src 
                            = (0x0000ffffU & ((IData)(1U) 
                                              + (IData)(vlSelfRef.test_copper__DOT__dut__DOT__tile_inst__DOT__dma_src)));
                        __Vdly__test_copper__DOT__dut__DOT__tile_inst__DOT__dma_data_valid = 1U;
                    }
                }
            } else if ((1U & (IData)(vlSelfRef.test_copper__DOT__dut__DOT__tile_inst__DOT__dma_state))) {
                if (vlSelfRef.test_copper__DOT__dut__DOT__tile_inst__DOT__dma_data_valid) {
                    vlSelfRef.test_copper__DOT__dut__DOT__tile_inst__DOT____Vlvbound_h17e6e432__0 = 0U;
                    __Vdly__test_copper__DOT__dut__DOT__tile_inst__DOT__dma_count 
                        = (0x0000ffffU & ((IData)(vlSelfRef.test_copper__DOT__dut__DOT__tile_inst__DOT__dma_count) 
                                          - (IData)(1U)));
                    if ((0x0f9fU >= (0x00000fffU & 
                                     ((((IData)(0x000003e8U) 
                                        * (IData)(vlSelfRef.test_copper__DOT__dut__DOT__tile_inst__DOT__dma_nt)) 
                                       + ((IData)(0x00000028U) 
                                          * (IData)(vlSelfRef.test_copper__DOT__dut__DOT__tile_inst__DOT__dma_col_row))) 
                                      + (0x0000003fU 
                                         & (IData)(vlSelfRef.test_copper__DOT__dut__DOT__tile_inst__DOT__dma_dst)))))) {
                        __VdlyVal__test_copper__DOT__dut__DOT__tile_inst__DOT__nametable__v2 
                            = vlSelfRef.test_copper__DOT__dut__DOT__tile_inst__DOT____Vlvbound_h17e6e432__0;
                        __VdlyDim0__test_copper__DOT__dut__DOT__tile_inst__DOT__nametable__v2 
                            = (0x00000fffU & ((((IData)(0x000003e8U) 
                                                * (IData)(vlSelfRef.test_copper__DOT__dut__DOT__tile_inst__DOT__dma_nt)) 
                                               + ((IData)(0x00000028U) 
                                                  * (IData)(vlSelfRef.test_copper__DOT__dut__DOT__tile_inst__DOT__dma_col_row))) 
                                              + (0x0000003fU 
                                                 & (IData)(vlSelfRef.test_copper__DOT__dut__DOT__tile_inst__DOT__dma_dst))));
                        vlSelfRef.__VdlyCommitQueuetest_copper__DOT__dut__DOT__tile_inst__DOT__nametable.enqueue(__VdlyVal__test_copper__DOT__dut__DOT__tile_inst__DOT__nametable__v2, (IData)(__VdlyDim0__test_copper__DOT__dut__DOT__tile_inst__DOT__nametable__v2));
                    }
                    __Vdly__test_copper__DOT__dut__DOT__tile_inst__DOT__dma_dst 
                        = (0x0000ffffU & ((IData)(1U) 
                                          + (IData)(vlSelfRef.test_copper__DOT__dut__DOT__tile_inst__DOT__dma_dst)));
                }
                if (((0U == (IData)(vlSelfRef.test_copper__DOT__dut__DOT__tile_inst__DOT__dma_count)) 
                     & (~ (IData)(vlSelfRef.test_copper__DOT__dut__DOT__tile_inst__DOT__dma_data_valid)))) {
                    __Vdly__test_copper__DOT__dut__DOT__tile_inst__DOT__dma_state = 0U;
                } else if ((1U & ((~ (IData)(vlSelfRef.test_copper__DOT__dut__DOT__tile_inst__DOT__dma_data_valid)) 
                                  | (1U < (IData)(vlSelfRef.test_copper__DOT__dut__DOT__tile_inst__DOT__dma_count))))) {
                    __Vdly__test_copper__DOT__dut__DOT__tile_inst__DOT__dma_src 
                        = (0x0000ffffU & ((IData)(1U) 
                                          + (IData)(vlSelfRef.test_copper__DOT__dut__DOT__tile_inst__DOT__dma_src)));
                    __Vdly__test_copper__DOT__dut__DOT__tile_inst__DOT__dma_data_valid = 1U;
                }
            } else {
                if (vlSelfRef.test_copper__DOT__dut__DOT__tile_inst__DOT__dma_data_valid) {
                    if ((0U == (IData)(vlSelfRef.test_copper__DOT__dut__DOT__tile_inst__DOT__dma_pal_phase))) {
                        __Vdly__test_copper__DOT__dut__DOT__tile_inst__DOT__dma_pal_r = 0U;
                        __Vdly__test_copper__DOT__dut__DOT__tile_inst__DOT__dma_pal_phase = 1U;
                    } else if ((1U == (IData)(vlSelfRef.test_copper__DOT__dut__DOT__tile_inst__DOT__dma_pal_phase))) {
                        __Vdly__test_copper__DOT__dut__DOT__tile_inst__DOT__dma_pal_g = 0U;
                        __Vdly__test_copper__DOT__dut__DOT__tile_inst__DOT__dma_pal_phase = 2U;
                    } else if ((2U == (IData)(vlSelfRef.test_copper__DOT__dut__DOT__tile_inst__DOT__dma_pal_phase))) {
                        __VdlyVal__test_copper__DOT__dut__DOT__tile_inst__DOT__pal_ram__v0 
                            = ((0x00000f00U & ((IData)(vlSelfRef.test_copper__DOT__dut__DOT__tile_inst__DOT__dma_pal_r) 
                                               << 4U)) 
                               | (0x000000f0U & (IData)(vlSelfRef.test_copper__DOT__dut__DOT__tile_inst__DOT__dma_pal_g)));
                        __VdlyDim0__test_copper__DOT__dut__DOT__tile_inst__DOT__pal_ram__v0 
                            = (((IData)(vlSelfRef.test_copper__DOT__dut__DOT__tile_inst__DOT__dma_pal_sub) 
                                << 4U) | (IData)(vlSelfRef.test_copper__DOT__dut__DOT__tile_inst__DOT__dma_pal_cidx));
                        __VdlySet__test_copper__DOT__dut__DOT__tile_inst__DOT__pal_ram__v0 = 1U;
                        __Vdly__test_copper__DOT__dut__DOT__tile_inst__DOT__dma_pal_phase = 0U;
                        if ((0x0fU == (IData)(vlSelfRef.test_copper__DOT__dut__DOT__tile_inst__DOT__dma_pal_cidx))) {
                            __Vdly__test_copper__DOT__dut__DOT__tile_inst__DOT__dma_count = 0U;
                        } else {
                            vlSelfRef.test_copper__DOT__dut__DOT__tile_inst__DOT__dma_pal_cidx 
                                = (0x0000000fU & ((IData)(1U) 
                                                  + (IData)(vlSelfRef.test_copper__DOT__dut__DOT__tile_inst__DOT__dma_pal_cidx)));
                        }
                    } else {
                        __Vdly__test_copper__DOT__dut__DOT__tile_inst__DOT__dma_pal_phase = 0U;
                    }
                }
                if (((0U == (IData)(vlSelfRef.test_copper__DOT__dut__DOT__tile_inst__DOT__dma_count)) 
                     & (~ (IData)(vlSelfRef.test_copper__DOT__dut__DOT__tile_inst__DOT__dma_data_valid)))) {
                    __Vdly__test_copper__DOT__dut__DOT__tile_inst__DOT__dma_state = 0U;
                } else if ((1U & ((~ (IData)(vlSelfRef.test_copper__DOT__dut__DOT__tile_inst__DOT__dma_data_valid)) 
                                  | (0U < (IData)(vlSelfRef.test_copper__DOT__dut__DOT__tile_inst__DOT__dma_count))))) {
                    __Vdly__test_copper__DOT__dut__DOT__tile_inst__DOT__dma_src 
                        = (0x0000ffffU & ((IData)(1U) 
                                          + (IData)(vlSelfRef.test_copper__DOT__dut__DOT__tile_inst__DOT__dma_src)));
                    __Vdly__test_copper__DOT__dut__DOT__tile_inst__DOT__dma_data_valid = 1U;
                }
            }
        } else if ((2U & (IData)(vlSelfRef.test_copper__DOT__dut__DOT__tile_inst__DOT__dma_state))) {
            if ((1U & (IData)(vlSelfRef.test_copper__DOT__dut__DOT__tile_inst__DOT__dma_state))) {
                if (vlSelfRef.test_copper__DOT__dut__DOT__tile_inst__DOT__dma_data_valid) {
                    vlSelfRef.test_copper__DOT__dut__DOT__tile_inst__DOT____Vlvbound_h3335cc4a__0 = 0U;
                    __Vdly__test_copper__DOT__dut__DOT__tile_inst__DOT__dma_count 
                        = (0x0000ffffU & ((IData)(vlSelfRef.test_copper__DOT__dut__DOT__tile_inst__DOT__dma_count) 
                                          - (IData)(1U)));
                    if ((0x0f9fU >= (0x00000fffU & 
                                     (((IData)(0x000003e8U) 
                                       * (IData)(vlSelfRef.test_copper__DOT__dut__DOT__tile_inst__DOT__dma_nt)) 
                                      + (0x000003ffU 
                                         & (IData)(vlSelfRef.test_copper__DOT__dut__DOT__tile_inst__DOT__dma_dst)))))) {
                        __VdlyVal__test_copper__DOT__dut__DOT__tile_inst__DOT__attr_table__v3 
                            = vlSelfRef.test_copper__DOT__dut__DOT__tile_inst__DOT____Vlvbound_h3335cc4a__0;
                        __VdlyDim0__test_copper__DOT__dut__DOT__tile_inst__DOT__attr_table__v3 
                            = (0x00000fffU & (((IData)(0x000003e8U) 
                                               * (IData)(vlSelfRef.test_copper__DOT__dut__DOT__tile_inst__DOT__dma_nt)) 
                                              + (0x000003ffU 
                                                 & (IData)(vlSelfRef.test_copper__DOT__dut__DOT__tile_inst__DOT__dma_dst))));
                        __VdlySet__test_copper__DOT__dut__DOT__tile_inst__DOT__attr_table__v3 = 1U;
                    }
                    __Vdly__test_copper__DOT__dut__DOT__tile_inst__DOT__dma_dst 
                        = (0x0000ffffU & ((IData)(1U) 
                                          + (IData)(vlSelfRef.test_copper__DOT__dut__DOT__tile_inst__DOT__dma_dst)));
                }
                if (((0U == (IData)(vlSelfRef.test_copper__DOT__dut__DOT__tile_inst__DOT__dma_count)) 
                     & (~ (IData)(vlSelfRef.test_copper__DOT__dut__DOT__tile_inst__DOT__dma_data_valid)))) {
                    __Vdly__test_copper__DOT__dut__DOT__tile_inst__DOT__dma_state = 0U;
                } else if ((1U & ((~ (IData)(vlSelfRef.test_copper__DOT__dut__DOT__tile_inst__DOT__dma_data_valid)) 
                                  | (1U < (IData)(vlSelfRef.test_copper__DOT__dut__DOT__tile_inst__DOT__dma_count))))) {
                    __Vdly__test_copper__DOT__dut__DOT__tile_inst__DOT__dma_src 
                        = (0x0000ffffU & ((IData)(1U) 
                                          + (IData)(vlSelfRef.test_copper__DOT__dut__DOT__tile_inst__DOT__dma_src)));
                    __Vdly__test_copper__DOT__dut__DOT__tile_inst__DOT__dma_data_valid = 1U;
                }
            } else {
                if (vlSelfRef.test_copper__DOT__dut__DOT__tile_inst__DOT__dma_data_valid) {
                    vlSelfRef.test_copper__DOT__dut__DOT__tile_inst__DOT____Vlvbound_ha65a9a44__0 = 0U;
                    __Vdly__test_copper__DOT__dut__DOT__tile_inst__DOT__dma_count 
                        = (0x0000ffffU & ((IData)(vlSelfRef.test_copper__DOT__dut__DOT__tile_inst__DOT__dma_count) 
                                          - (IData)(1U)));
                    if ((0x0f9fU >= (0x00000fffU & 
                                     (((IData)(0x000003e8U) 
                                       * (IData)(vlSelfRef.test_copper__DOT__dut__DOT__tile_inst__DOT__dma_nt)) 
                                      + (0x000003ffU 
                                         & (IData)(vlSelfRef.test_copper__DOT__dut__DOT__tile_inst__DOT__dma_dst)))))) {
                        __VdlyVal__test_copper__DOT__dut__DOT__tile_inst__DOT__nametable__v3 
                            = vlSelfRef.test_copper__DOT__dut__DOT__tile_inst__DOT____Vlvbound_ha65a9a44__0;
                        __VdlyDim0__test_copper__DOT__dut__DOT__tile_inst__DOT__nametable__v3 
                            = (0x00000fffU & (((IData)(0x000003e8U) 
                                               * (IData)(vlSelfRef.test_copper__DOT__dut__DOT__tile_inst__DOT__dma_nt)) 
                                              + (0x000003ffU 
                                                 & (IData)(vlSelfRef.test_copper__DOT__dut__DOT__tile_inst__DOT__dma_dst))));
                        vlSelfRef.__VdlyCommitQueuetest_copper__DOT__dut__DOT__tile_inst__DOT__nametable.enqueue(__VdlyVal__test_copper__DOT__dut__DOT__tile_inst__DOT__nametable__v3, (IData)(__VdlyDim0__test_copper__DOT__dut__DOT__tile_inst__DOT__nametable__v3));
                    }
                    __Vdly__test_copper__DOT__dut__DOT__tile_inst__DOT__dma_dst 
                        = (0x0000ffffU & ((IData)(1U) 
                                          + (IData)(vlSelfRef.test_copper__DOT__dut__DOT__tile_inst__DOT__dma_dst)));
                }
                if (((0U == (IData)(vlSelfRef.test_copper__DOT__dut__DOT__tile_inst__DOT__dma_count)) 
                     & (~ (IData)(vlSelfRef.test_copper__DOT__dut__DOT__tile_inst__DOT__dma_data_valid)))) {
                    __Vdly__test_copper__DOT__dut__DOT__tile_inst__DOT__dma_state = 0U;
                } else if ((1U & ((~ (IData)(vlSelfRef.test_copper__DOT__dut__DOT__tile_inst__DOT__dma_data_valid)) 
                                  | (1U < (IData)(vlSelfRef.test_copper__DOT__dut__DOT__tile_inst__DOT__dma_count))))) {
                    __Vdly__test_copper__DOT__dut__DOT__tile_inst__DOT__dma_src 
                        = (0x0000ffffU & ((IData)(1U) 
                                          + (IData)(vlSelfRef.test_copper__DOT__dut__DOT__tile_inst__DOT__dma_src)));
                    __Vdly__test_copper__DOT__dut__DOT__tile_inst__DOT__dma_data_valid = 1U;
                }
            }
        } else if ((1U & (IData)(vlSelfRef.test_copper__DOT__dut__DOT__tile_inst__DOT__dma_state))) {
            if (vlSelfRef.test_copper__DOT__dut__DOT__tile_inst__DOT__dma_data_valid) {
                __Vdly__test_copper__DOT__dut__DOT__tile_inst__DOT__dma_count 
                    = (0x0000ffffU & ((IData)(vlSelfRef.test_copper__DOT__dut__DOT__tile_inst__DOT__dma_count) 
                                      - (IData)(1U)));
                __VdlyDim0__test_copper__DOT__dut__DOT__tile_inst__DOT__tile_data__v1 
                    = (0x00007fffU & (IData)(vlSelfRef.test_copper__DOT__dut__DOT__tile_inst__DOT__dma_dst));
                __VdlySet__test_copper__DOT__dut__DOT__tile_inst__DOT__tile_data__v1 = 1U;
                __Vdly__test_copper__DOT__dut__DOT__tile_inst__DOT__dma_dst 
                    = (0x0000ffffU & ((IData)(1U) + (IData)(vlSelfRef.test_copper__DOT__dut__DOT__tile_inst__DOT__dma_dst)));
            }
            if (((0U == (IData)(vlSelfRef.test_copper__DOT__dut__DOT__tile_inst__DOT__dma_count)) 
                 & (~ (IData)(vlSelfRef.test_copper__DOT__dut__DOT__tile_inst__DOT__dma_data_valid)))) {
                __Vdly__test_copper__DOT__dut__DOT__tile_inst__DOT__dma_state = 0U;
            } else if ((1U & ((~ (IData)(vlSelfRef.test_copper__DOT__dut__DOT__tile_inst__DOT__dma_data_valid)) 
                              | (1U < (IData)(vlSelfRef.test_copper__DOT__dut__DOT__tile_inst__DOT__dma_count))))) {
                __Vdly__test_copper__DOT__dut__DOT__tile_inst__DOT__dma_src 
                    = (0x0000ffffU & ((IData)(1U) + (IData)(vlSelfRef.test_copper__DOT__dut__DOT__tile_inst__DOT__dma_src)));
                __Vdly__test_copper__DOT__dut__DOT__tile_inst__DOT__dma_data_valid = 1U;
            }
        }
        if (((IData)(vlSelfRef.test_copper__DOT__cpu_we) 
             & (IData)(vlSelfRef.test_copper__DOT__dut__DOT__tile_reg_sel))) {
            if ((0U != (0x0000001fU & (IData)(vlSelfRef.test_copper__DOT__cpu_addr)))) {
                if ((1U != (0x0000001fU & (IData)(vlSelfRef.test_copper__DOT__cpu_addr)))) {
                    if ((2U != (0x0000001fU & (IData)(vlSelfRef.test_copper__DOT__cpu_addr)))) {
                        if ((3U != (0x0000001fU & (IData)(vlSelfRef.test_copper__DOT__cpu_addr)))) {
                            if ((4U != (0x0000001fU 
                                        & (IData)(vlSelfRef.test_copper__DOT__cpu_addr)))) {
                                if ((5U != (0x0000001fU 
                                            & (IData)(vlSelfRef.test_copper__DOT__cpu_addr)))) {
                                    if ((7U == (0x0000001fU 
                                                & (IData)(vlSelfRef.test_copper__DOT__cpu_addr)))) {
                                        if ((0U == (IData)(vlSelfRef.test_copper__DOT__dut__DOT__tile_inst__DOT__dma_state))) {
                                            if ((1U 
                                                 & (~ 
                                                    ((IData)(vlSelfRef.test_copper__DOT__cpu_wdata) 
                                                     >> 7U)))) {
                                                if (
                                                    (1U 
                                                     & (~ 
                                                        ((IData)(vlSelfRef.test_copper__DOT__cpu_wdata) 
                                                         >> 6U)))) {
                                                    if (
                                                        (1U 
                                                         & (~ 
                                                            ((IData)(vlSelfRef.test_copper__DOT__cpu_wdata) 
                                                             >> 5U)))) {
                                                        if (
                                                            (0x00000010U 
                                                             & (IData)(vlSelfRef.test_copper__DOT__cpu_wdata))) {
                                                            if (
                                                                (1U 
                                                                 & (~ 
                                                                    ((IData)(vlSelfRef.test_copper__DOT__cpu_wdata) 
                                                                     >> 3U)))) {
                                                                if (
                                                                    (1U 
                                                                     & (~ 
                                                                        ((IData)(vlSelfRef.test_copper__DOT__cpu_wdata) 
                                                                         >> 2U)))) {
                                                                    if (
                                                                        (2U 
                                                                         & (IData)(vlSelfRef.test_copper__DOT__cpu_wdata))) {
                                                                        if (
                                                                            (1U 
                                                                             & (IData)(vlSelfRef.test_copper__DOT__cpu_wdata))) {
                                                                            if (
                                                                                ((4U 
                                                                                > vlSelfRef.test_copper__DOT__dut__DOT__tile_inst__DOT__tregs[8U]) 
                                                                                & (0x28U 
                                                                                > vlSelfRef.test_copper__DOT__dut__DOT__tile_inst__DOT__tregs[9U]))) {
                                                                                vlSelfRef.test_copper__DOT__dut__DOT__tile_inst__DOT____Vlvbound_h021f5fae__0 
                                                                                = vlSelfRef.test_copper__DOT__dut__DOT__tile_inst__DOT__col_buffer[0U];
                                                                                if (
                                                                                (0x0f9fU 
                                                                                >= 
                                                                                (0x00000fffU 
                                                                                & (((IData)(0x000003e8U) 
                                                                                * vlSelfRef.test_copper__DOT__dut__DOT__tile_inst__DOT__tregs[8U]) 
                                                                                + vlSelfRef.test_copper__DOT__dut__DOT__tile_inst__DOT__tregs[9U])))) {
                                                                                __VdlyVal__test_copper__DOT__dut__DOT__tile_inst__DOT__nametable__v4 
                                                                                = vlSelfRef.test_copper__DOT__dut__DOT__tile_inst__DOT____Vlvbound_h021f5fae__0;
                                                                                __VdlyDim0__test_copper__DOT__dut__DOT__tile_inst__DOT__nametable__v4 
                                                                                = 
                                                                                (0x00000fffU 
                                                                                & (((IData)(0x000003e8U) 
                                                                                * vlSelfRef.test_copper__DOT__dut__DOT__tile_inst__DOT__tregs[8U]) 
                                                                                + vlSelfRef.test_copper__DOT__dut__DOT__tile_inst__DOT__tregs[9U]));
                                                                                vlSelfRef.__VdlyCommitQueuetest_copper__DOT__dut__DOT__tile_inst__DOT__nametable.enqueue(__VdlyVal__test_copper__DOT__dut__DOT__tile_inst__DOT__nametable__v4, (IData)(__VdlyDim0__test_copper__DOT__dut__DOT__tile_inst__DOT__nametable__v4));
                                                                                }
                                                                                vlSelfRef.test_copper__DOT__dut__DOT__tile_inst__DOT____Vlvbound_h021f5fae__0 
                                                                                = vlSelfRef.test_copper__DOT__dut__DOT__tile_inst__DOT__col_buffer[1U];
                                                                                if (
                                                                                (0x0f9fU 
                                                                                >= 
                                                                                (0x00000fffU 
                                                                                & ((IData)(0x0028U) 
                                                                                + 
                                                                                (((IData)(0x000003e8U) 
                                                                                * vlSelfRef.test_copper__DOT__dut__DOT__tile_inst__DOT__tregs[8U]) 
                                                                                + vlSelfRef.test_copper__DOT__dut__DOT__tile_inst__DOT__tregs[9U]))))) {
                                                                                __VdlyVal__test_copper__DOT__dut__DOT__tile_inst__DOT__nametable__v5 
                                                                                = vlSelfRef.test_copper__DOT__dut__DOT__tile_inst__DOT____Vlvbound_h021f5fae__0;
                                                                                __VdlyDim0__test_copper__DOT__dut__DOT__tile_inst__DOT__nametable__v5 
                                                                                = 
                                                                                (0x00000fffU 
                                                                                & ((IData)(0x0028U) 
                                                                                + 
                                                                                (((IData)(0x000003e8U) 
                                                                                * vlSelfRef.test_copper__DOT__dut__DOT__tile_inst__DOT__tregs[8U]) 
                                                                                + vlSelfRef.test_copper__DOT__dut__DOT__tile_inst__DOT__tregs[9U])));
                                                                                vlSelfRef.__VdlyCommitQueuetest_copper__DOT__dut__DOT__tile_inst__DOT__nametable.enqueue(__VdlyVal__test_copper__DOT__dut__DOT__tile_inst__DOT__nametable__v5, (IData)(__VdlyDim0__test_copper__DOT__dut__DOT__tile_inst__DOT__nametable__v5));
                                                                                }
                                                                                vlSelfRef.test_copper__DOT__dut__DOT__tile_inst__DOT____Vlvbound_h021f5fae__0 
                                                                                = vlSelfRef.test_copper__DOT__dut__DOT__tile_inst__DOT__col_buffer[2U];
                                                                                if (
                                                                                (0x0f9fU 
                                                                                >= 
                                                                                (0x00000fffU 
                                                                                & ((IData)(0x0050U) 
                                                                                + 
                                                                                (((IData)(0x000003e8U) 
                                                                                * vlSelfRef.test_copper__DOT__dut__DOT__tile_inst__DOT__tregs[8U]) 
                                                                                + vlSelfRef.test_copper__DOT__dut__DOT__tile_inst__DOT__tregs[9U]))))) {
                                                                                __VdlyVal__test_copper__DOT__dut__DOT__tile_inst__DOT__nametable__v6 
                                                                                = vlSelfRef.test_copper__DOT__dut__DOT__tile_inst__DOT____Vlvbound_h021f5fae__0;
                                                                                __VdlyDim0__test_copper__DOT__dut__DOT__tile_inst__DOT__nametable__v6 
                                                                                = 
                                                                                (0x00000fffU 
                                                                                & ((IData)(0x0050U) 
                                                                                + 
                                                                                (((IData)(0x000003e8U) 
                                                                                * vlSelfRef.test_copper__DOT__dut__DOT__tile_inst__DOT__tregs[8U]) 
                                                                                + vlSelfRef.test_copper__DOT__dut__DOT__tile_inst__DOT__tregs[9U])));
                                                                                vlSelfRef.__VdlyCommitQueuetest_copper__DOT__dut__DOT__tile_inst__DOT__nametable.enqueue(__VdlyVal__test_copper__DOT__dut__DOT__tile_inst__DOT__nametable__v6, (IData)(__VdlyDim0__test_copper__DOT__dut__DOT__tile_inst__DOT__nametable__v6));
                                                                                }
                                                                                vlSelfRef.test_copper__DOT__dut__DOT__tile_inst__DOT____Vlvbound_h021f5fae__0 
                                                                                = vlSelfRef.test_copper__DOT__dut__DOT__tile_inst__DOT__col_buffer[3U];
                                                                                if (
                                                                                (0x0f9fU 
                                                                                >= 
                                                                                (0x00000fffU 
                                                                                & ((IData)(0x0078U) 
                                                                                + 
                                                                                (((IData)(0x000003e8U) 
                                                                                * vlSelfRef.test_copper__DOT__dut__DOT__tile_inst__DOT__tregs[8U]) 
                                                                                + vlSelfRef.test_copper__DOT__dut__DOT__tile_inst__DOT__tregs[9U]))))) {
                                                                                __VdlyVal__test_copper__DOT__dut__DOT__tile_inst__DOT__nametable__v7 
                                                                                = vlSelfRef.test_copper__DOT__dut__DOT__tile_inst__DOT____Vlvbound_h021f5fae__0;
                                                                                __VdlyDim0__test_copper__DOT__dut__DOT__tile_inst__DOT__nametable__v7 
                                                                                = 
                                                                                (0x00000fffU 
                                                                                & ((IData)(0x0078U) 
                                                                                + 
                                                                                (((IData)(0x000003e8U) 
                                                                                * vlSelfRef.test_copper__DOT__dut__DOT__tile_inst__DOT__tregs[8U]) 
                                                                                + vlSelfRef.test_copper__DOT__dut__DOT__tile_inst__DOT__tregs[9U])));
                                                                                vlSelfRef.__VdlyCommitQueuetest_copper__DOT__dut__DOT__tile_inst__DOT__nametable.enqueue(__VdlyVal__test_copper__DOT__dut__DOT__tile_inst__DOT__nametable__v7, (IData)(__VdlyDim0__test_copper__DOT__dut__DOT__tile_inst__DOT__nametable__v7));
                                                                                }
                                                                                vlSelfRef.test_copper__DOT__dut__DOT__tile_inst__DOT____Vlvbound_h021f5fae__0 
                                                                                = vlSelfRef.test_copper__DOT__dut__DOT__tile_inst__DOT__col_buffer[4U];
                                                                                if (
                                                                                (0x0f9fU 
                                                                                >= 
                                                                                (0x00000fffU 
                                                                                & ((IData)(0x00a0U) 
                                                                                + 
                                                                                (((IData)(0x000003e8U) 
                                                                                * vlSelfRef.test_copper__DOT__dut__DOT__tile_inst__DOT__tregs[8U]) 
                                                                                + vlSelfRef.test_copper__DOT__dut__DOT__tile_inst__DOT__tregs[9U]))))) {
                                                                                __VdlyVal__test_copper__DOT__dut__DOT__tile_inst__DOT__nametable__v8 
                                                                                = vlSelfRef.test_copper__DOT__dut__DOT__tile_inst__DOT____Vlvbound_h021f5fae__0;
                                                                                __VdlyDim0__test_copper__DOT__dut__DOT__tile_inst__DOT__nametable__v8 
                                                                                = 
                                                                                (0x00000fffU 
                                                                                & ((IData)(0x00a0U) 
                                                                                + 
                                                                                (((IData)(0x000003e8U) 
                                                                                * vlSelfRef.test_copper__DOT__dut__DOT__tile_inst__DOT__tregs[8U]) 
                                                                                + vlSelfRef.test_copper__DOT__dut__DOT__tile_inst__DOT__tregs[9U])));
                                                                                vlSelfRef.__VdlyCommitQueuetest_copper__DOT__dut__DOT__tile_inst__DOT__nametable.enqueue(__VdlyVal__test_copper__DOT__dut__DOT__tile_inst__DOT__nametable__v8, (IData)(__VdlyDim0__test_copper__DOT__dut__DOT__tile_inst__DOT__nametable__v8));
                                                                                }
                                                                                vlSelfRef.test_copper__DOT__dut__DOT__tile_inst__DOT____Vlvbound_h021f5fae__0 
                                                                                = vlSelfRef.test_copper__DOT__dut__DOT__tile_inst__DOT__col_buffer[5U];
                                                                                if (
                                                                                (0x0f9fU 
                                                                                >= 
                                                                                (0x00000fffU 
                                                                                & ((IData)(0x00c8U) 
                                                                                + 
                                                                                (((IData)(0x000003e8U) 
                                                                                * vlSelfRef.test_copper__DOT__dut__DOT__tile_inst__DOT__tregs[8U]) 
                                                                                + vlSelfRef.test_copper__DOT__dut__DOT__tile_inst__DOT__tregs[9U]))))) {
                                                                                __VdlyVal__test_copper__DOT__dut__DOT__tile_inst__DOT__nametable__v9 
                                                                                = vlSelfRef.test_copper__DOT__dut__DOT__tile_inst__DOT____Vlvbound_h021f5fae__0;
                                                                                __VdlyDim0__test_copper__DOT__dut__DOT__tile_inst__DOT__nametable__v9 
                                                                                = 
                                                                                (0x00000fffU 
                                                                                & ((IData)(0x00c8U) 
                                                                                + 
                                                                                (((IData)(0x000003e8U) 
                                                                                * vlSelfRef.test_copper__DOT__dut__DOT__tile_inst__DOT__tregs[8U]) 
                                                                                + vlSelfRef.test_copper__DOT__dut__DOT__tile_inst__DOT__tregs[9U])));
                                                                                vlSelfRef.__VdlyCommitQueuetest_copper__DOT__dut__DOT__tile_inst__DOT__nametable.enqueue(__VdlyVal__test_copper__DOT__dut__DOT__tile_inst__DOT__nametable__v9, (IData)(__VdlyDim0__test_copper__DOT__dut__DOT__tile_inst__DOT__nametable__v9));
                                                                                }
                                                                                vlSelfRef.test_copper__DOT__dut__DOT__tile_inst__DOT____Vlvbound_h021f5fae__0 
                                                                                = vlSelfRef.test_copper__DOT__dut__DOT__tile_inst__DOT__col_buffer[6U];
                                                                                if (
                                                                                (0x0f9fU 
                                                                                >= 
                                                                                (0x00000fffU 
                                                                                & ((IData)(0x00f0U) 
                                                                                + 
                                                                                (((IData)(0x000003e8U) 
                                                                                * vlSelfRef.test_copper__DOT__dut__DOT__tile_inst__DOT__tregs[8U]) 
                                                                                + vlSelfRef.test_copper__DOT__dut__DOT__tile_inst__DOT__tregs[9U]))))) {
                                                                                __VdlyVal__test_copper__DOT__dut__DOT__tile_inst__DOT__nametable__v10 
                                                                                = vlSelfRef.test_copper__DOT__dut__DOT__tile_inst__DOT____Vlvbound_h021f5fae__0;
                                                                                __VdlyDim0__test_copper__DOT__dut__DOT__tile_inst__DOT__nametable__v10 
                                                                                = 
                                                                                (0x00000fffU 
                                                                                & ((IData)(0x00f0U) 
                                                                                + 
                                                                                (((IData)(0x000003e8U) 
                                                                                * vlSelfRef.test_copper__DOT__dut__DOT__tile_inst__DOT__tregs[8U]) 
                                                                                + vlSelfRef.test_copper__DOT__dut__DOT__tile_inst__DOT__tregs[9U])));
                                                                                vlSelfRef.__VdlyCommitQueuetest_copper__DOT__dut__DOT__tile_inst__DOT__nametable.enqueue(__VdlyVal__test_copper__DOT__dut__DOT__tile_inst__DOT__nametable__v10, (IData)(__VdlyDim0__test_copper__DOT__dut__DOT__tile_inst__DOT__nametable__v10));
                                                                                }
                                                                                vlSelfRef.test_copper__DOT__dut__DOT__tile_inst__DOT____Vlvbound_h021f5fae__0 
                                                                                = vlSelfRef.test_copper__DOT__dut__DOT__tile_inst__DOT__col_buffer[7U];
                                                                                if (
                                                                                (0x0f9fU 
                                                                                >= 
                                                                                (0x00000fffU 
                                                                                & ((IData)(0x0118U) 
                                                                                + 
                                                                                (((IData)(0x000003e8U) 
                                                                                * vlSelfRef.test_copper__DOT__dut__DOT__tile_inst__DOT__tregs[8U]) 
                                                                                + vlSelfRef.test_copper__DOT__dut__DOT__tile_inst__DOT__tregs[9U]))))) {
                                                                                __VdlyVal__test_copper__DOT__dut__DOT__tile_inst__DOT__nametable__v11 
                                                                                = vlSelfRef.test_copper__DOT__dut__DOT__tile_inst__DOT____Vlvbound_h021f5fae__0;
                                                                                __VdlyDim0__test_copper__DOT__dut__DOT__tile_inst__DOT__nametable__v11 
                                                                                = 
                                                                                (0x00000fffU 
                                                                                & ((IData)(0x0118U) 
                                                                                + 
                                                                                (((IData)(0x000003e8U) 
                                                                                * vlSelfRef.test_copper__DOT__dut__DOT__tile_inst__DOT__tregs[8U]) 
                                                                                + vlSelfRef.test_copper__DOT__dut__DOT__tile_inst__DOT__tregs[9U])));
                                                                                vlSelfRef.__VdlyCommitQueuetest_copper__DOT__dut__DOT__tile_inst__DOT__nametable.enqueue(__VdlyVal__test_copper__DOT__dut__DOT__tile_inst__DOT__nametable__v11, (IData)(__VdlyDim0__test_copper__DOT__dut__DOT__tile_inst__DOT__nametable__v11));
                                                                                }
                                                                                vlSelfRef.test_copper__DOT__dut__DOT__tile_inst__DOT____Vlvbound_h021f5fae__0 
                                                                                = vlSelfRef.test_copper__DOT__dut__DOT__tile_inst__DOT__col_buffer[8U];
                                                                                if (
                                                                                (0x0f9fU 
                                                                                >= 
                                                                                (0x00000fffU 
                                                                                & ((IData)(0x0140U) 
                                                                                + 
                                                                                (((IData)(0x000003e8U) 
                                                                                * vlSelfRef.test_copper__DOT__dut__DOT__tile_inst__DOT__tregs[8U]) 
                                                                                + vlSelfRef.test_copper__DOT__dut__DOT__tile_inst__DOT__tregs[9U]))))) {
                                                                                __VdlyVal__test_copper__DOT__dut__DOT__tile_inst__DOT__nametable__v12 
                                                                                = vlSelfRef.test_copper__DOT__dut__DOT__tile_inst__DOT____Vlvbound_h021f5fae__0;
                                                                                __VdlyDim0__test_copper__DOT__dut__DOT__tile_inst__DOT__nametable__v12 
                                                                                = 
                                                                                (0x00000fffU 
                                                                                & ((IData)(0x0140U) 
                                                                                + 
                                                                                (((IData)(0x000003e8U) 
                                                                                * vlSelfRef.test_copper__DOT__dut__DOT__tile_inst__DOT__tregs[8U]) 
                                                                                + vlSelfRef.test_copper__DOT__dut__DOT__tile_inst__DOT__tregs[9U])));
                                                                                vlSelfRef.__VdlyCommitQueuetest_copper__DOT__dut__DOT__tile_inst__DOT__nametable.enqueue(__VdlyVal__test_copper__DOT__dut__DOT__tile_inst__DOT__nametable__v12, (IData)(__VdlyDim0__test_copper__DOT__dut__DOT__tile_inst__DOT__nametable__v12));
                                                                                }
                                                                                vlSelfRef.test_copper__DOT__dut__DOT__tile_inst__DOT____Vlvbound_h021f5fae__0 
                                                                                = vlSelfRef.test_copper__DOT__dut__DOT__tile_inst__DOT__col_buffer[9U];
                                                                                if (
                                                                                (0x0f9fU 
                                                                                >= 
                                                                                (0x00000fffU 
                                                                                & ((IData)(0x0168U) 
                                                                                + 
                                                                                (((IData)(0x000003e8U) 
                                                                                * vlSelfRef.test_copper__DOT__dut__DOT__tile_inst__DOT__tregs[8U]) 
                                                                                + vlSelfRef.test_copper__DOT__dut__DOT__tile_inst__DOT__tregs[9U]))))) {
                                                                                __VdlyVal__test_copper__DOT__dut__DOT__tile_inst__DOT__nametable__v13 
                                                                                = vlSelfRef.test_copper__DOT__dut__DOT__tile_inst__DOT____Vlvbound_h021f5fae__0;
                                                                                __VdlyDim0__test_copper__DOT__dut__DOT__tile_inst__DOT__nametable__v13 
                                                                                = 
                                                                                (0x00000fffU 
                                                                                & ((IData)(0x0168U) 
                                                                                + 
                                                                                (((IData)(0x000003e8U) 
                                                                                * vlSelfRef.test_copper__DOT__dut__DOT__tile_inst__DOT__tregs[8U]) 
                                                                                + vlSelfRef.test_copper__DOT__dut__DOT__tile_inst__DOT__tregs[9U])));
                                                                                vlSelfRef.__VdlyCommitQueuetest_copper__DOT__dut__DOT__tile_inst__DOT__nametable.enqueue(__VdlyVal__test_copper__DOT__dut__DOT__tile_inst__DOT__nametable__v13, (IData)(__VdlyDim0__test_copper__DOT__dut__DOT__tile_inst__DOT__nametable__v13));
                                                                                }
                                                                                vlSelfRef.test_copper__DOT__dut__DOT__tile_inst__DOT____Vlvbound_h021f5fae__0 
                                                                                = vlSelfRef.test_copper__DOT__dut__DOT__tile_inst__DOT__col_buffer[10U];
                                                                                if (
                                                                                (0x0f9fU 
                                                                                >= 
                                                                                (0x00000fffU 
                                                                                & ((IData)(0x0190U) 
                                                                                + 
                                                                                (((IData)(0x000003e8U) 
                                                                                * vlSelfRef.test_copper__DOT__dut__DOT__tile_inst__DOT__tregs[8U]) 
                                                                                + vlSelfRef.test_copper__DOT__dut__DOT__tile_inst__DOT__tregs[9U]))))) {
                                                                                __VdlyVal__test_copper__DOT__dut__DOT__tile_inst__DOT__nametable__v14 
                                                                                = vlSelfRef.test_copper__DOT__dut__DOT__tile_inst__DOT____Vlvbound_h021f5fae__0;
                                                                                __VdlyDim0__test_copper__DOT__dut__DOT__tile_inst__DOT__nametable__v14 
                                                                                = 
                                                                                (0x00000fffU 
                                                                                & ((IData)(0x0190U) 
                                                                                + 
                                                                                (((IData)(0x000003e8U) 
                                                                                * vlSelfRef.test_copper__DOT__dut__DOT__tile_inst__DOT__tregs[8U]) 
                                                                                + vlSelfRef.test_copper__DOT__dut__DOT__tile_inst__DOT__tregs[9U])));
                                                                                vlSelfRef.__VdlyCommitQueuetest_copper__DOT__dut__DOT__tile_inst__DOT__nametable.enqueue(__VdlyVal__test_copper__DOT__dut__DOT__tile_inst__DOT__nametable__v14, (IData)(__VdlyDim0__test_copper__DOT__dut__DOT__tile_inst__DOT__nametable__v14));
                                                                                }
                                                                                vlSelfRef.test_copper__DOT__dut__DOT__tile_inst__DOT____Vlvbound_h021f5fae__0 
                                                                                = vlSelfRef.test_copper__DOT__dut__DOT__tile_inst__DOT__col_buffer[11U];
                                                                                if (
                                                                                (0x0f9fU 
                                                                                >= 
                                                                                (0x00000fffU 
                                                                                & ((IData)(0x01b8U) 
                                                                                + 
                                                                                (((IData)(0x000003e8U) 
                                                                                * vlSelfRef.test_copper__DOT__dut__DOT__tile_inst__DOT__tregs[8U]) 
                                                                                + vlSelfRef.test_copper__DOT__dut__DOT__tile_inst__DOT__tregs[9U]))))) {
                                                                                __VdlyVal__test_copper__DOT__dut__DOT__tile_inst__DOT__nametable__v15 
                                                                                = vlSelfRef.test_copper__DOT__dut__DOT__tile_inst__DOT____Vlvbound_h021f5fae__0;
                                                                                __VdlyDim0__test_copper__DOT__dut__DOT__tile_inst__DOT__nametable__v15 
                                                                                = 
                                                                                (0x00000fffU 
                                                                                & ((IData)(0x01b8U) 
                                                                                + 
                                                                                (((IData)(0x000003e8U) 
                                                                                * vlSelfRef.test_copper__DOT__dut__DOT__tile_inst__DOT__tregs[8U]) 
                                                                                + vlSelfRef.test_copper__DOT__dut__DOT__tile_inst__DOT__tregs[9U])));
                                                                                vlSelfRef.__VdlyCommitQueuetest_copper__DOT__dut__DOT__tile_inst__DOT__nametable.enqueue(__VdlyVal__test_copper__DOT__dut__DOT__tile_inst__DOT__nametable__v15, (IData)(__VdlyDim0__test_copper__DOT__dut__DOT__tile_inst__DOT__nametable__v15));
                                                                                }
                                                                                vlSelfRef.test_copper__DOT__dut__DOT__tile_inst__DOT____Vlvbound_h021f5fae__0 
                                                                                = vlSelfRef.test_copper__DOT__dut__DOT__tile_inst__DOT__col_buffer[12U];
                                                                                if (
                                                                                (0x0f9fU 
                                                                                >= 
                                                                                (0x00000fffU 
                                                                                & ((IData)(0x01e0U) 
                                                                                + 
                                                                                (((IData)(0x000003e8U) 
                                                                                * vlSelfRef.test_copper__DOT__dut__DOT__tile_inst__DOT__tregs[8U]) 
                                                                                + vlSelfRef.test_copper__DOT__dut__DOT__tile_inst__DOT__tregs[9U]))))) {
                                                                                __VdlyVal__test_copper__DOT__dut__DOT__tile_inst__DOT__nametable__v16 
                                                                                = vlSelfRef.test_copper__DOT__dut__DOT__tile_inst__DOT____Vlvbound_h021f5fae__0;
                                                                                __VdlyDim0__test_copper__DOT__dut__DOT__tile_inst__DOT__nametable__v16 
                                                                                = 
                                                                                (0x00000fffU 
                                                                                & ((IData)(0x01e0U) 
                                                                                + 
                                                                                (((IData)(0x000003e8U) 
                                                                                * vlSelfRef.test_copper__DOT__dut__DOT__tile_inst__DOT__tregs[8U]) 
                                                                                + vlSelfRef.test_copper__DOT__dut__DOT__tile_inst__DOT__tregs[9U])));
                                                                                vlSelfRef.__VdlyCommitQueuetest_copper__DOT__dut__DOT__tile_inst__DOT__nametable.enqueue(__VdlyVal__test_copper__DOT__dut__DOT__tile_inst__DOT__nametable__v16, (IData)(__VdlyDim0__test_copper__DOT__dut__DOT__tile_inst__DOT__nametable__v16));
                                                                                }
                                                                                vlSelfRef.test_copper__DOT__dut__DOT__tile_inst__DOT____Vlvbound_h021f5fae__0 
                                                                                = vlSelfRef.test_copper__DOT__dut__DOT__tile_inst__DOT__col_buffer[13U];
                                                                                if (
                                                                                (0x0f9fU 
                                                                                >= 
                                                                                (0x00000fffU 
                                                                                & ((IData)(0x0208U) 
                                                                                + 
                                                                                (((IData)(0x000003e8U) 
                                                                                * vlSelfRef.test_copper__DOT__dut__DOT__tile_inst__DOT__tregs[8U]) 
                                                                                + vlSelfRef.test_copper__DOT__dut__DOT__tile_inst__DOT__tregs[9U]))))) {
                                                                                __VdlyVal__test_copper__DOT__dut__DOT__tile_inst__DOT__nametable__v17 
                                                                                = vlSelfRef.test_copper__DOT__dut__DOT__tile_inst__DOT____Vlvbound_h021f5fae__0;
                                                                                __VdlyDim0__test_copper__DOT__dut__DOT__tile_inst__DOT__nametable__v17 
                                                                                = 
                                                                                (0x00000fffU 
                                                                                & ((IData)(0x0208U) 
                                                                                + 
                                                                                (((IData)(0x000003e8U) 
                                                                                * vlSelfRef.test_copper__DOT__dut__DOT__tile_inst__DOT__tregs[8U]) 
                                                                                + vlSelfRef.test_copper__DOT__dut__DOT__tile_inst__DOT__tregs[9U])));
                                                                                vlSelfRef.__VdlyCommitQueuetest_copper__DOT__dut__DOT__tile_inst__DOT__nametable.enqueue(__VdlyVal__test_copper__DOT__dut__DOT__tile_inst__DOT__nametable__v17, (IData)(__VdlyDim0__test_copper__DOT__dut__DOT__tile_inst__DOT__nametable__v17));
                                                                                }
                                                                                vlSelfRef.test_copper__DOT__dut__DOT__tile_inst__DOT____Vlvbound_h021f5fae__0 
                                                                                = vlSelfRef.test_copper__DOT__dut__DOT__tile_inst__DOT__col_buffer[14U];
                                                                                if (
                                                                                (0x0f9fU 
                                                                                >= 
                                                                                (0x00000fffU 
                                                                                & ((IData)(0x0230U) 
                                                                                + 
                                                                                (((IData)(0x000003e8U) 
                                                                                * vlSelfRef.test_copper__DOT__dut__DOT__tile_inst__DOT__tregs[8U]) 
                                                                                + vlSelfRef.test_copper__DOT__dut__DOT__tile_inst__DOT__tregs[9U]))))) {
                                                                                __VdlyVal__test_copper__DOT__dut__DOT__tile_inst__DOT__nametable__v18 
                                                                                = vlSelfRef.test_copper__DOT__dut__DOT__tile_inst__DOT____Vlvbound_h021f5fae__0;
                                                                                __VdlyDim0__test_copper__DOT__dut__DOT__tile_inst__DOT__nametable__v18 
                                                                                = 
                                                                                (0x00000fffU 
                                                                                & ((IData)(0x0230U) 
                                                                                + 
                                                                                (((IData)(0x000003e8U) 
                                                                                * vlSelfRef.test_copper__DOT__dut__DOT__tile_inst__DOT__tregs[8U]) 
                                                                                + vlSelfRef.test_copper__DOT__dut__DOT__tile_inst__DOT__tregs[9U])));
                                                                                vlSelfRef.__VdlyCommitQueuetest_copper__DOT__dut__DOT__tile_inst__DOT__nametable.enqueue(__VdlyVal__test_copper__DOT__dut__DOT__tile_inst__DOT__nametable__v18, (IData)(__VdlyDim0__test_copper__DOT__dut__DOT__tile_inst__DOT__nametable__v18));
                                                                                }
                                                                                vlSelfRef.test_copper__DOT__dut__DOT__tile_inst__DOT____Vlvbound_h021f5fae__0 
                                                                                = vlSelfRef.test_copper__DOT__dut__DOT__tile_inst__DOT__col_buffer[15U];
                                                                                if (
                                                                                (0x0f9fU 
                                                                                >= 
                                                                                (0x00000fffU 
                                                                                & ((IData)(0x0258U) 
                                                                                + 
                                                                                (((IData)(0x000003e8U) 
                                                                                * vlSelfRef.test_copper__DOT__dut__DOT__tile_inst__DOT__tregs[8U]) 
                                                                                + vlSelfRef.test_copper__DOT__dut__DOT__tile_inst__DOT__tregs[9U]))))) {
                                                                                __VdlyVal__test_copper__DOT__dut__DOT__tile_inst__DOT__nametable__v19 
                                                                                = vlSelfRef.test_copper__DOT__dut__DOT__tile_inst__DOT____Vlvbound_h021f5fae__0;
                                                                                __VdlyDim0__test_copper__DOT__dut__DOT__tile_inst__DOT__nametable__v19 
                                                                                = 
                                                                                (0x00000fffU 
                                                                                & ((IData)(0x0258U) 
                                                                                + 
                                                                                (((IData)(0x000003e8U) 
                                                                                * vlSelfRef.test_copper__DOT__dut__DOT__tile_inst__DOT__tregs[8U]) 
                                                                                + vlSelfRef.test_copper__DOT__dut__DOT__tile_inst__DOT__tregs[9U])));
                                                                                vlSelfRef.__VdlyCommitQueuetest_copper__DOT__dut__DOT__tile_inst__DOT__nametable.enqueue(__VdlyVal__test_copper__DOT__dut__DOT__tile_inst__DOT__nametable__v19, (IData)(__VdlyDim0__test_copper__DOT__dut__DOT__tile_inst__DOT__nametable__v19));
                                                                                }
                                                                                vlSelfRef.test_copper__DOT__dut__DOT__tile_inst__DOT____Vlvbound_h021f5fae__0 
                                                                                = vlSelfRef.test_copper__DOT__dut__DOT__tile_inst__DOT__col_buffer[16U];
                                                                                if (
                                                                                (0x0f9fU 
                                                                                >= 
                                                                                (0x00000fffU 
                                                                                & ((IData)(0x0280U) 
                                                                                + 
                                                                                (((IData)(0x000003e8U) 
                                                                                * vlSelfRef.test_copper__DOT__dut__DOT__tile_inst__DOT__tregs[8U]) 
                                                                                + vlSelfRef.test_copper__DOT__dut__DOT__tile_inst__DOT__tregs[9U]))))) {
                                                                                __VdlyVal__test_copper__DOT__dut__DOT__tile_inst__DOT__nametable__v20 
                                                                                = vlSelfRef.test_copper__DOT__dut__DOT__tile_inst__DOT____Vlvbound_h021f5fae__0;
                                                                                __VdlyDim0__test_copper__DOT__dut__DOT__tile_inst__DOT__nametable__v20 
                                                                                = 
                                                                                (0x00000fffU 
                                                                                & ((IData)(0x0280U) 
                                                                                + 
                                                                                (((IData)(0x000003e8U) 
                                                                                * vlSelfRef.test_copper__DOT__dut__DOT__tile_inst__DOT__tregs[8U]) 
                                                                                + vlSelfRef.test_copper__DOT__dut__DOT__tile_inst__DOT__tregs[9U])));
                                                                                vlSelfRef.__VdlyCommitQueuetest_copper__DOT__dut__DOT__tile_inst__DOT__nametable.enqueue(__VdlyVal__test_copper__DOT__dut__DOT__tile_inst__DOT__nametable__v20, (IData)(__VdlyDim0__test_copper__DOT__dut__DOT__tile_inst__DOT__nametable__v20));
                                                                                }
                                                                                vlSelfRef.test_copper__DOT__dut__DOT__tile_inst__DOT____Vlvbound_h021f5fae__0 
                                                                                = vlSelfRef.test_copper__DOT__dut__DOT__tile_inst__DOT__col_buffer[17U];
                                                                                if (
                                                                                (0x0f9fU 
                                                                                >= 
                                                                                (0x00000fffU 
                                                                                & ((IData)(0x02a8U) 
                                                                                + 
                                                                                (((IData)(0x000003e8U) 
                                                                                * vlSelfRef.test_copper__DOT__dut__DOT__tile_inst__DOT__tregs[8U]) 
                                                                                + vlSelfRef.test_copper__DOT__dut__DOT__tile_inst__DOT__tregs[9U]))))) {
                                                                                __VdlyVal__test_copper__DOT__dut__DOT__tile_inst__DOT__nametable__v21 
                                                                                = vlSelfRef.test_copper__DOT__dut__DOT__tile_inst__DOT____Vlvbound_h021f5fae__0;
                                                                                __VdlyDim0__test_copper__DOT__dut__DOT__tile_inst__DOT__nametable__v21 
                                                                                = 
                                                                                (0x00000fffU 
                                                                                & ((IData)(0x02a8U) 
                                                                                + 
                                                                                (((IData)(0x000003e8U) 
                                                                                * vlSelfRef.test_copper__DOT__dut__DOT__tile_inst__DOT__tregs[8U]) 
                                                                                + vlSelfRef.test_copper__DOT__dut__DOT__tile_inst__DOT__tregs[9U])));
                                                                                vlSelfRef.__VdlyCommitQueuetest_copper__DOT__dut__DOT__tile_inst__DOT__nametable.enqueue(__VdlyVal__test_copper__DOT__dut__DOT__tile_inst__DOT__nametable__v21, (IData)(__VdlyDim0__test_copper__DOT__dut__DOT__tile_inst__DOT__nametable__v21));
                                                                                }
                                                                                vlSelfRef.test_copper__DOT__dut__DOT__tile_inst__DOT____Vlvbound_h021f5fae__0 
                                                                                = vlSelfRef.test_copper__DOT__dut__DOT__tile_inst__DOT__col_buffer[18U];
                                                                                if (
                                                                                (0x0f9fU 
                                                                                >= 
                                                                                (0x00000fffU 
                                                                                & ((IData)(0x02d0U) 
                                                                                + 
                                                                                (((IData)(0x000003e8U) 
                                                                                * vlSelfRef.test_copper__DOT__dut__DOT__tile_inst__DOT__tregs[8U]) 
                                                                                + vlSelfRef.test_copper__DOT__dut__DOT__tile_inst__DOT__tregs[9U]))))) {
                                                                                __VdlyVal__test_copper__DOT__dut__DOT__tile_inst__DOT__nametable__v22 
                                                                                = vlSelfRef.test_copper__DOT__dut__DOT__tile_inst__DOT____Vlvbound_h021f5fae__0;
                                                                                __VdlyDim0__test_copper__DOT__dut__DOT__tile_inst__DOT__nametable__v22 
                                                                                = 
                                                                                (0x00000fffU 
                                                                                & ((IData)(0x02d0U) 
                                                                                + 
                                                                                (((IData)(0x000003e8U) 
                                                                                * vlSelfRef.test_copper__DOT__dut__DOT__tile_inst__DOT__tregs[8U]) 
                                                                                + vlSelfRef.test_copper__DOT__dut__DOT__tile_inst__DOT__tregs[9U])));
                                                                                vlSelfRef.__VdlyCommitQueuetest_copper__DOT__dut__DOT__tile_inst__DOT__nametable.enqueue(__VdlyVal__test_copper__DOT__dut__DOT__tile_inst__DOT__nametable__v22, (IData)(__VdlyDim0__test_copper__DOT__dut__DOT__tile_inst__DOT__nametable__v22));
                                                                                }
                                                                                vlSelfRef.test_copper__DOT__dut__DOT__tile_inst__DOT____Vlvbound_h021f5fae__0 
                                                                                = vlSelfRef.test_copper__DOT__dut__DOT__tile_inst__DOT__col_buffer[19U];
                                                                                if (
                                                                                (0x0f9fU 
                                                                                >= 
                                                                                (0x00000fffU 
                                                                                & ((IData)(0x02f8U) 
                                                                                + 
                                                                                (((IData)(0x000003e8U) 
                                                                                * vlSelfRef.test_copper__DOT__dut__DOT__tile_inst__DOT__tregs[8U]) 
                                                                                + vlSelfRef.test_copper__DOT__dut__DOT__tile_inst__DOT__tregs[9U]))))) {
                                                                                __VdlyVal__test_copper__DOT__dut__DOT__tile_inst__DOT__nametable__v23 
                                                                                = vlSelfRef.test_copper__DOT__dut__DOT__tile_inst__DOT____Vlvbound_h021f5fae__0;
                                                                                __VdlyDim0__test_copper__DOT__dut__DOT__tile_inst__DOT__nametable__v23 
                                                                                = 
                                                                                (0x00000fffU 
                                                                                & ((IData)(0x02f8U) 
                                                                                + 
                                                                                (((IData)(0x000003e8U) 
                                                                                * vlSelfRef.test_copper__DOT__dut__DOT__tile_inst__DOT__tregs[8U]) 
                                                                                + vlSelfRef.test_copper__DOT__dut__DOT__tile_inst__DOT__tregs[9U])));
                                                                                vlSelfRef.__VdlyCommitQueuetest_copper__DOT__dut__DOT__tile_inst__DOT__nametable.enqueue(__VdlyVal__test_copper__DOT__dut__DOT__tile_inst__DOT__nametable__v23, (IData)(__VdlyDim0__test_copper__DOT__dut__DOT__tile_inst__DOT__nametable__v23));
                                                                                }
                                                                                vlSelfRef.test_copper__DOT__dut__DOT__tile_inst__DOT____Vlvbound_h021f5fae__0 
                                                                                = vlSelfRef.test_copper__DOT__dut__DOT__tile_inst__DOT__col_buffer[20U];
                                                                                if (
                                                                                (0x0f9fU 
                                                                                >= 
                                                                                (0x00000fffU 
                                                                                & ((IData)(0x0320U) 
                                                                                + 
                                                                                (((IData)(0x000003e8U) 
                                                                                * vlSelfRef.test_copper__DOT__dut__DOT__tile_inst__DOT__tregs[8U]) 
                                                                                + vlSelfRef.test_copper__DOT__dut__DOT__tile_inst__DOT__tregs[9U]))))) {
                                                                                __VdlyVal__test_copper__DOT__dut__DOT__tile_inst__DOT__nametable__v24 
                                                                                = vlSelfRef.test_copper__DOT__dut__DOT__tile_inst__DOT____Vlvbound_h021f5fae__0;
                                                                                __VdlyDim0__test_copper__DOT__dut__DOT__tile_inst__DOT__nametable__v24 
                                                                                = 
                                                                                (0x00000fffU 
                                                                                & ((IData)(0x0320U) 
                                                                                + 
                                                                                (((IData)(0x000003e8U) 
                                                                                * vlSelfRef.test_copper__DOT__dut__DOT__tile_inst__DOT__tregs[8U]) 
                                                                                + vlSelfRef.test_copper__DOT__dut__DOT__tile_inst__DOT__tregs[9U])));
                                                                                vlSelfRef.__VdlyCommitQueuetest_copper__DOT__dut__DOT__tile_inst__DOT__nametable.enqueue(__VdlyVal__test_copper__DOT__dut__DOT__tile_inst__DOT__nametable__v24, (IData)(__VdlyDim0__test_copper__DOT__dut__DOT__tile_inst__DOT__nametable__v24));
                                                                                }
                                                                                vlSelfRef.test_copper__DOT__dut__DOT__tile_inst__DOT____Vlvbound_h021f5fae__0 
                                                                                = vlSelfRef.test_copper__DOT__dut__DOT__tile_inst__DOT__col_buffer[21U];
                                                                                if (
                                                                                (0x0f9fU 
                                                                                >= 
                                                                                (0x00000fffU 
                                                                                & ((IData)(0x0348U) 
                                                                                + 
                                                                                (((IData)(0x000003e8U) 
                                                                                * vlSelfRef.test_copper__DOT__dut__DOT__tile_inst__DOT__tregs[8U]) 
                                                                                + vlSelfRef.test_copper__DOT__dut__DOT__tile_inst__DOT__tregs[9U]))))) {
                                                                                __VdlyVal__test_copper__DOT__dut__DOT__tile_inst__DOT__nametable__v25 
                                                                                = vlSelfRef.test_copper__DOT__dut__DOT__tile_inst__DOT____Vlvbound_h021f5fae__0;
                                                                                __VdlyDim0__test_copper__DOT__dut__DOT__tile_inst__DOT__nametable__v25 
                                                                                = 
                                                                                (0x00000fffU 
                                                                                & ((IData)(0x0348U) 
                                                                                + 
                                                                                (((IData)(0x000003e8U) 
                                                                                * vlSelfRef.test_copper__DOT__dut__DOT__tile_inst__DOT__tregs[8U]) 
                                                                                + vlSelfRef.test_copper__DOT__dut__DOT__tile_inst__DOT__tregs[9U])));
                                                                                vlSelfRef.__VdlyCommitQueuetest_copper__DOT__dut__DOT__tile_inst__DOT__nametable.enqueue(__VdlyVal__test_copper__DOT__dut__DOT__tile_inst__DOT__nametable__v25, (IData)(__VdlyDim0__test_copper__DOT__dut__DOT__tile_inst__DOT__nametable__v25));
                                                                                }
                                                                                vlSelfRef.test_copper__DOT__dut__DOT__tile_inst__DOT____Vlvbound_h021f5fae__0 
                                                                                = vlSelfRef.test_copper__DOT__dut__DOT__tile_inst__DOT__col_buffer[22U];
                                                                                if (
                                                                                (0x0f9fU 
                                                                                >= 
                                                                                (0x00000fffU 
                                                                                & ((IData)(0x0370U) 
                                                                                + 
                                                                                (((IData)(0x000003e8U) 
                                                                                * vlSelfRef.test_copper__DOT__dut__DOT__tile_inst__DOT__tregs[8U]) 
                                                                                + vlSelfRef.test_copper__DOT__dut__DOT__tile_inst__DOT__tregs[9U]))))) {
                                                                                __VdlyVal__test_copper__DOT__dut__DOT__tile_inst__DOT__nametable__v26 
                                                                                = vlSelfRef.test_copper__DOT__dut__DOT__tile_inst__DOT____Vlvbound_h021f5fae__0;
                                                                                __VdlyDim0__test_copper__DOT__dut__DOT__tile_inst__DOT__nametable__v26 
                                                                                = 
                                                                                (0x00000fffU 
                                                                                & ((IData)(0x0370U) 
                                                                                + 
                                                                                (((IData)(0x000003e8U) 
                                                                                * vlSelfRef.test_copper__DOT__dut__DOT__tile_inst__DOT__tregs[8U]) 
                                                                                + vlSelfRef.test_copper__DOT__dut__DOT__tile_inst__DOT__tregs[9U])));
                                                                                vlSelfRef.__VdlyCommitQueuetest_copper__DOT__dut__DOT__tile_inst__DOT__nametable.enqueue(__VdlyVal__test_copper__DOT__dut__DOT__tile_inst__DOT__nametable__v26, (IData)(__VdlyDim0__test_copper__DOT__dut__DOT__tile_inst__DOT__nametable__v26));
                                                                                }
                                                                                vlSelfRef.test_copper__DOT__dut__DOT__tile_inst__DOT____Vlvbound_h021f5fae__0 
                                                                                = vlSelfRef.test_copper__DOT__dut__DOT__tile_inst__DOT__col_buffer[23U];
                                                                                if (
                                                                                (0x0f9fU 
                                                                                >= 
                                                                                (0x00000fffU 
                                                                                & ((IData)(0x0398U) 
                                                                                + 
                                                                                (((IData)(0x000003e8U) 
                                                                                * vlSelfRef.test_copper__DOT__dut__DOT__tile_inst__DOT__tregs[8U]) 
                                                                                + vlSelfRef.test_copper__DOT__dut__DOT__tile_inst__DOT__tregs[9U]))))) {
                                                                                __VdlyVal__test_copper__DOT__dut__DOT__tile_inst__DOT__nametable__v27 
                                                                                = vlSelfRef.test_copper__DOT__dut__DOT__tile_inst__DOT____Vlvbound_h021f5fae__0;
                                                                                __VdlyDim0__test_copper__DOT__dut__DOT__tile_inst__DOT__nametable__v27 
                                                                                = 
                                                                                (0x00000fffU 
                                                                                & ((IData)(0x0398U) 
                                                                                + 
                                                                                (((IData)(0x000003e8U) 
                                                                                * vlSelfRef.test_copper__DOT__dut__DOT__tile_inst__DOT__tregs[8U]) 
                                                                                + vlSelfRef.test_copper__DOT__dut__DOT__tile_inst__DOT__tregs[9U])));
                                                                                vlSelfRef.__VdlyCommitQueuetest_copper__DOT__dut__DOT__tile_inst__DOT__nametable.enqueue(__VdlyVal__test_copper__DOT__dut__DOT__tile_inst__DOT__nametable__v27, (IData)(__VdlyDim0__test_copper__DOT__dut__DOT__tile_inst__DOT__nametable__v27));
                                                                                }
                                                                                vlSelfRef.test_copper__DOT__dut__DOT__tile_inst__DOT____Vlvbound_h021f5fae__0 
                                                                                = vlSelfRef.test_copper__DOT__dut__DOT__tile_inst__DOT__col_buffer[24U];
                                                                                if (
                                                                                (0x0f9fU 
                                                                                >= 
                                                                                (0x00000fffU 
                                                                                & ((IData)(0x03c0U) 
                                                                                + 
                                                                                (((IData)(0x000003e8U) 
                                                                                * vlSelfRef.test_copper__DOT__dut__DOT__tile_inst__DOT__tregs[8U]) 
                                                                                + vlSelfRef.test_copper__DOT__dut__DOT__tile_inst__DOT__tregs[9U]))))) {
                                                                                __VdlyVal__test_copper__DOT__dut__DOT__tile_inst__DOT__nametable__v28 
                                                                                = vlSelfRef.test_copper__DOT__dut__DOT__tile_inst__DOT____Vlvbound_h021f5fae__0;
                                                                                __VdlyDim0__test_copper__DOT__dut__DOT__tile_inst__DOT__nametable__v28 
                                                                                = 
                                                                                (0x00000fffU 
                                                                                & ((IData)(0x03c0U) 
                                                                                + 
                                                                                (((IData)(0x000003e8U) 
                                                                                * vlSelfRef.test_copper__DOT__dut__DOT__tile_inst__DOT__tregs[8U]) 
                                                                                + vlSelfRef.test_copper__DOT__dut__DOT__tile_inst__DOT__tregs[9U])));
                                                                                vlSelfRef.__VdlyCommitQueuetest_copper__DOT__dut__DOT__tile_inst__DOT__nametable.enqueue(__VdlyVal__test_copper__DOT__dut__DOT__tile_inst__DOT__nametable__v28, (IData)(__VdlyDim0__test_copper__DOT__dut__DOT__tile_inst__DOT__nametable__v28));
                                                                                }
                                                                            }
                                                                        } else {
                                                                            if (
                                                                                (0U 
                                                                                >= vlSelfRef.test_copper__DOT__dut__DOT__tile_inst__DOT__tregs[8U])) {
                                                                                vlSelfRef.test_copper__DOT__dut__DOT__tile_inst__DOT____Vlvbound_h02fc01d7__0 
                                                                                = vlSelfRef.test_copper__DOT__dut__DOT__tile_inst__DOT__tregs[10U];
                                                                                __VdlyVal__test_copper__DOT__dut__DOT__tile_inst__DOT__col_buffer__v0 
                                                                                = vlSelfRef.test_copper__DOT__dut__DOT__tile_inst__DOT____Vlvbound_h02fc01d7__0;
                                                                                __VdlySet__test_copper__DOT__dut__DOT__tile_inst__DOT__col_buffer__v0 = 1U;
                                                                            }
                                                                            if (
                                                                                ((1U 
                                                                                >= vlSelfRef.test_copper__DOT__dut__DOT__tile_inst__DOT__tregs[8U]) 
                                                                                & (1U 
                                                                                <= vlSelfRef.test_copper__DOT__dut__DOT__tile_inst__DOT__tregs[9U]))) {
                                                                                vlSelfRef.test_copper__DOT__dut__DOT__tile_inst__DOT____Vlvbound_h02fc01d7__0 
                                                                                = vlSelfRef.test_copper__DOT__dut__DOT__tile_inst__DOT__tregs[10U];
                                                                                __VdlyVal__test_copper__DOT__dut__DOT__tile_inst__DOT__col_buffer__v1 
                                                                                = vlSelfRef.test_copper__DOT__dut__DOT__tile_inst__DOT____Vlvbound_h02fc01d7__0;
                                                                                __VdlySet__test_copper__DOT__dut__DOT__tile_inst__DOT__col_buffer__v1 = 1U;
                                                                            }
                                                                            if (
                                                                                ((2U 
                                                                                >= vlSelfRef.test_copper__DOT__dut__DOT__tile_inst__DOT__tregs[8U]) 
                                                                                & (2U 
                                                                                <= vlSelfRef.test_copper__DOT__dut__DOT__tile_inst__DOT__tregs[9U]))) {
                                                                                vlSelfRef.test_copper__DOT__dut__DOT__tile_inst__DOT____Vlvbound_h02fc01d7__0 
                                                                                = vlSelfRef.test_copper__DOT__dut__DOT__tile_inst__DOT__tregs[10U];
                                                                                __VdlyVal__test_copper__DOT__dut__DOT__tile_inst__DOT__col_buffer__v2 
                                                                                = vlSelfRef.test_copper__DOT__dut__DOT__tile_inst__DOT____Vlvbound_h02fc01d7__0;
                                                                                __VdlySet__test_copper__DOT__dut__DOT__tile_inst__DOT__col_buffer__v2 = 1U;
                                                                            }
                                                                            if (
                                                                                ((3U 
                                                                                >= vlSelfRef.test_copper__DOT__dut__DOT__tile_inst__DOT__tregs[8U]) 
                                                                                & (3U 
                                                                                <= vlSelfRef.test_copper__DOT__dut__DOT__tile_inst__DOT__tregs[9U]))) {
                                                                                vlSelfRef.test_copper__DOT__dut__DOT__tile_inst__DOT____Vlvbound_h02fc01d7__0 
                                                                                = vlSelfRef.test_copper__DOT__dut__DOT__tile_inst__DOT__tregs[10U];
                                                                                __VdlyVal__test_copper__DOT__dut__DOT__tile_inst__DOT__col_buffer__v3 
                                                                                = vlSelfRef.test_copper__DOT__dut__DOT__tile_inst__DOT____Vlvbound_h02fc01d7__0;
                                                                                __VdlySet__test_copper__DOT__dut__DOT__tile_inst__DOT__col_buffer__v3 = 1U;
                                                                            }
                                                                            if (
                                                                                ((4U 
                                                                                >= vlSelfRef.test_copper__DOT__dut__DOT__tile_inst__DOT__tregs[8U]) 
                                                                                & (4U 
                                                                                <= vlSelfRef.test_copper__DOT__dut__DOT__tile_inst__DOT__tregs[9U]))) {
                                                                                vlSelfRef.test_copper__DOT__dut__DOT__tile_inst__DOT____Vlvbound_h02fc01d7__0 
                                                                                = vlSelfRef.test_copper__DOT__dut__DOT__tile_inst__DOT__tregs[10U];
                                                                                __VdlyVal__test_copper__DOT__dut__DOT__tile_inst__DOT__col_buffer__v4 
                                                                                = vlSelfRef.test_copper__DOT__dut__DOT__tile_inst__DOT____Vlvbound_h02fc01d7__0;
                                                                                __VdlySet__test_copper__DOT__dut__DOT__tile_inst__DOT__col_buffer__v4 = 1U;
                                                                            }
                                                                            if (
                                                                                ((5U 
                                                                                >= vlSelfRef.test_copper__DOT__dut__DOT__tile_inst__DOT__tregs[8U]) 
                                                                                & (5U 
                                                                                <= vlSelfRef.test_copper__DOT__dut__DOT__tile_inst__DOT__tregs[9U]))) {
                                                                                vlSelfRef.test_copper__DOT__dut__DOT__tile_inst__DOT____Vlvbound_h02fc01d7__0 
                                                                                = vlSelfRef.test_copper__DOT__dut__DOT__tile_inst__DOT__tregs[10U];
                                                                                __VdlyVal__test_copper__DOT__dut__DOT__tile_inst__DOT__col_buffer__v5 
                                                                                = vlSelfRef.test_copper__DOT__dut__DOT__tile_inst__DOT____Vlvbound_h02fc01d7__0;
                                                                                __VdlySet__test_copper__DOT__dut__DOT__tile_inst__DOT__col_buffer__v5 = 1U;
                                                                            }
                                                                            if (
                                                                                ((6U 
                                                                                >= vlSelfRef.test_copper__DOT__dut__DOT__tile_inst__DOT__tregs[8U]) 
                                                                                & (6U 
                                                                                <= vlSelfRef.test_copper__DOT__dut__DOT__tile_inst__DOT__tregs[9U]))) {
                                                                                vlSelfRef.test_copper__DOT__dut__DOT__tile_inst__DOT____Vlvbound_h02fc01d7__0 
                                                                                = vlSelfRef.test_copper__DOT__dut__DOT__tile_inst__DOT__tregs[10U];
                                                                                __VdlyVal__test_copper__DOT__dut__DOT__tile_inst__DOT__col_buffer__v6 
                                                                                = vlSelfRef.test_copper__DOT__dut__DOT__tile_inst__DOT____Vlvbound_h02fc01d7__0;
                                                                                __VdlySet__test_copper__DOT__dut__DOT__tile_inst__DOT__col_buffer__v6 = 1U;
                                                                            }
                                                                            if (
                                                                                ((7U 
                                                                                >= vlSelfRef.test_copper__DOT__dut__DOT__tile_inst__DOT__tregs[8U]) 
                                                                                & (7U 
                                                                                <= vlSelfRef.test_copper__DOT__dut__DOT__tile_inst__DOT__tregs[9U]))) {
                                                                                vlSelfRef.test_copper__DOT__dut__DOT__tile_inst__DOT____Vlvbound_h02fc01d7__0 
                                                                                = vlSelfRef.test_copper__DOT__dut__DOT__tile_inst__DOT__tregs[10U];
                                                                                __VdlyVal__test_copper__DOT__dut__DOT__tile_inst__DOT__col_buffer__v7 
                                                                                = vlSelfRef.test_copper__DOT__dut__DOT__tile_inst__DOT____Vlvbound_h02fc01d7__0;
                                                                                __VdlySet__test_copper__DOT__dut__DOT__tile_inst__DOT__col_buffer__v7 = 1U;
                                                                            }
                                                                            if (
                                                                                ((8U 
                                                                                >= vlSelfRef.test_copper__DOT__dut__DOT__tile_inst__DOT__tregs[8U]) 
                                                                                & (8U 
                                                                                <= vlSelfRef.test_copper__DOT__dut__DOT__tile_inst__DOT__tregs[9U]))) {
                                                                                vlSelfRef.test_copper__DOT__dut__DOT__tile_inst__DOT____Vlvbound_h02fc01d7__0 
                                                                                = vlSelfRef.test_copper__DOT__dut__DOT__tile_inst__DOT__tregs[10U];
                                                                                __VdlyVal__test_copper__DOT__dut__DOT__tile_inst__DOT__col_buffer__v8 
                                                                                = vlSelfRef.test_copper__DOT__dut__DOT__tile_inst__DOT____Vlvbound_h02fc01d7__0;
                                                                                __VdlySet__test_copper__DOT__dut__DOT__tile_inst__DOT__col_buffer__v8 = 1U;
                                                                            }
                                                                            if (
                                                                                ((9U 
                                                                                >= vlSelfRef.test_copper__DOT__dut__DOT__tile_inst__DOT__tregs[8U]) 
                                                                                & (9U 
                                                                                <= vlSelfRef.test_copper__DOT__dut__DOT__tile_inst__DOT__tregs[9U]))) {
                                                                                vlSelfRef.test_copper__DOT__dut__DOT__tile_inst__DOT____Vlvbound_h02fc01d7__0 
                                                                                = vlSelfRef.test_copper__DOT__dut__DOT__tile_inst__DOT__tregs[10U];
                                                                                __VdlyVal__test_copper__DOT__dut__DOT__tile_inst__DOT__col_buffer__v9 
                                                                                = vlSelfRef.test_copper__DOT__dut__DOT__tile_inst__DOT____Vlvbound_h02fc01d7__0;
                                                                                __VdlySet__test_copper__DOT__dut__DOT__tile_inst__DOT__col_buffer__v9 = 1U;
                                                                            }
                                                                            if (
                                                                                ((0x0aU 
                                                                                >= vlSelfRef.test_copper__DOT__dut__DOT__tile_inst__DOT__tregs[8U]) 
                                                                                & (0x0aU 
                                                                                <= vlSelfRef.test_copper__DOT__dut__DOT__tile_inst__DOT__tregs[9U]))) {
                                                                                vlSelfRef.test_copper__DOT__dut__DOT__tile_inst__DOT____Vlvbound_h02fc01d7__0 
                                                                                = vlSelfRef.test_copper__DOT__dut__DOT__tile_inst__DOT__tregs[10U];
                                                                                __VdlyVal__test_copper__DOT__dut__DOT__tile_inst__DOT__col_buffer__v10 
                                                                                = vlSelfRef.test_copper__DOT__dut__DOT__tile_inst__DOT____Vlvbound_h02fc01d7__0;
                                                                                __VdlySet__test_copper__DOT__dut__DOT__tile_inst__DOT__col_buffer__v10 = 1U;
                                                                            }
                                                                            if (
                                                                                ((0x0bU 
                                                                                >= vlSelfRef.test_copper__DOT__dut__DOT__tile_inst__DOT__tregs[8U]) 
                                                                                & (0x0bU 
                                                                                <= vlSelfRef.test_copper__DOT__dut__DOT__tile_inst__DOT__tregs[9U]))) {
                                                                                vlSelfRef.test_copper__DOT__dut__DOT__tile_inst__DOT____Vlvbound_h02fc01d7__0 
                                                                                = vlSelfRef.test_copper__DOT__dut__DOT__tile_inst__DOT__tregs[10U];
                                                                                __VdlyVal__test_copper__DOT__dut__DOT__tile_inst__DOT__col_buffer__v11 
                                                                                = vlSelfRef.test_copper__DOT__dut__DOT__tile_inst__DOT____Vlvbound_h02fc01d7__0;
                                                                                __VdlySet__test_copper__DOT__dut__DOT__tile_inst__DOT__col_buffer__v11 = 1U;
                                                                            }
                                                                            if (
                                                                                ((0x0cU 
                                                                                >= vlSelfRef.test_copper__DOT__dut__DOT__tile_inst__DOT__tregs[8U]) 
                                                                                & (0x0cU 
                                                                                <= vlSelfRef.test_copper__DOT__dut__DOT__tile_inst__DOT__tregs[9U]))) {
                                                                                vlSelfRef.test_copper__DOT__dut__DOT__tile_inst__DOT____Vlvbound_h02fc01d7__0 
                                                                                = vlSelfRef.test_copper__DOT__dut__DOT__tile_inst__DOT__tregs[10U];
                                                                                __VdlyVal__test_copper__DOT__dut__DOT__tile_inst__DOT__col_buffer__v12 
                                                                                = vlSelfRef.test_copper__DOT__dut__DOT__tile_inst__DOT____Vlvbound_h02fc01d7__0;
                                                                                __VdlySet__test_copper__DOT__dut__DOT__tile_inst__DOT__col_buffer__v12 = 1U;
                                                                            }
                                                                            if (
                                                                                ((0x0dU 
                                                                                >= vlSelfRef.test_copper__DOT__dut__DOT__tile_inst__DOT__tregs[8U]) 
                                                                                & (0x0dU 
                                                                                <= vlSelfRef.test_copper__DOT__dut__DOT__tile_inst__DOT__tregs[9U]))) {
                                                                                vlSelfRef.test_copper__DOT__dut__DOT__tile_inst__DOT____Vlvbound_h02fc01d7__0 
                                                                                = vlSelfRef.test_copper__DOT__dut__DOT__tile_inst__DOT__tregs[10U];
                                                                                __VdlyVal__test_copper__DOT__dut__DOT__tile_inst__DOT__col_buffer__v13 
                                                                                = vlSelfRef.test_copper__DOT__dut__DOT__tile_inst__DOT____Vlvbound_h02fc01d7__0;
                                                                                __VdlySet__test_copper__DOT__dut__DOT__tile_inst__DOT__col_buffer__v13 = 1U;
                                                                            }
                                                                            if (
                                                                                ((0x0eU 
                                                                                >= vlSelfRef.test_copper__DOT__dut__DOT__tile_inst__DOT__tregs[8U]) 
                                                                                & (0x0eU 
                                                                                <= vlSelfRef.test_copper__DOT__dut__DOT__tile_inst__DOT__tregs[9U]))) {
                                                                                vlSelfRef.test_copper__DOT__dut__DOT__tile_inst__DOT____Vlvbound_h02fc01d7__0 
                                                                                = vlSelfRef.test_copper__DOT__dut__DOT__tile_inst__DOT__tregs[10U];
                                                                                __VdlyVal__test_copper__DOT__dut__DOT__tile_inst__DOT__col_buffer__v14 
                                                                                = vlSelfRef.test_copper__DOT__dut__DOT__tile_inst__DOT____Vlvbound_h02fc01d7__0;
                                                                                __VdlySet__test_copper__DOT__dut__DOT__tile_inst__DOT__col_buffer__v14 = 1U;
                                                                            }
                                                                            if (
                                                                                ((0x0fU 
                                                                                >= vlSelfRef.test_copper__DOT__dut__DOT__tile_inst__DOT__tregs[8U]) 
                                                                                & (0x0fU 
                                                                                <= vlSelfRef.test_copper__DOT__dut__DOT__tile_inst__DOT__tregs[9U]))) {
                                                                                vlSelfRef.test_copper__DOT__dut__DOT__tile_inst__DOT____Vlvbound_h02fc01d7__0 
                                                                                = vlSelfRef.test_copper__DOT__dut__DOT__tile_inst__DOT__tregs[10U];
                                                                                __VdlyVal__test_copper__DOT__dut__DOT__tile_inst__DOT__col_buffer__v15 
                                                                                = vlSelfRef.test_copper__DOT__dut__DOT__tile_inst__DOT____Vlvbound_h02fc01d7__0;
                                                                                __VdlySet__test_copper__DOT__dut__DOT__tile_inst__DOT__col_buffer__v15 = 1U;
                                                                            }
                                                                            if (
                                                                                ((0x10U 
                                                                                >= vlSelfRef.test_copper__DOT__dut__DOT__tile_inst__DOT__tregs[8U]) 
                                                                                & (0x10U 
                                                                                <= vlSelfRef.test_copper__DOT__dut__DOT__tile_inst__DOT__tregs[9U]))) {
                                                                                vlSelfRef.test_copper__DOT__dut__DOT__tile_inst__DOT____Vlvbound_h02fc01d7__0 
                                                                                = vlSelfRef.test_copper__DOT__dut__DOT__tile_inst__DOT__tregs[10U];
                                                                                __VdlyVal__test_copper__DOT__dut__DOT__tile_inst__DOT__col_buffer__v16 
                                                                                = vlSelfRef.test_copper__DOT__dut__DOT__tile_inst__DOT____Vlvbound_h02fc01d7__0;
                                                                                __VdlySet__test_copper__DOT__dut__DOT__tile_inst__DOT__col_buffer__v16 = 1U;
                                                                            }
                                                                            if (
                                                                                ((0x11U 
                                                                                >= vlSelfRef.test_copper__DOT__dut__DOT__tile_inst__DOT__tregs[8U]) 
                                                                                & (0x11U 
                                                                                <= vlSelfRef.test_copper__DOT__dut__DOT__tile_inst__DOT__tregs[9U]))) {
                                                                                vlSelfRef.test_copper__DOT__dut__DOT__tile_inst__DOT____Vlvbound_h02fc01d7__0 
                                                                                = vlSelfRef.test_copper__DOT__dut__DOT__tile_inst__DOT__tregs[10U];
                                                                                __VdlyVal__test_copper__DOT__dut__DOT__tile_inst__DOT__col_buffer__v17 
                                                                                = vlSelfRef.test_copper__DOT__dut__DOT__tile_inst__DOT____Vlvbound_h02fc01d7__0;
                                                                                __VdlySet__test_copper__DOT__dut__DOT__tile_inst__DOT__col_buffer__v17 = 1U;
                                                                            }
                                                                            if (
                                                                                ((0x12U 
                                                                                >= vlSelfRef.test_copper__DOT__dut__DOT__tile_inst__DOT__tregs[8U]) 
                                                                                & (0x12U 
                                                                                <= vlSelfRef.test_copper__DOT__dut__DOT__tile_inst__DOT__tregs[9U]))) {
                                                                                vlSelfRef.test_copper__DOT__dut__DOT__tile_inst__DOT____Vlvbound_h02fc01d7__0 
                                                                                = vlSelfRef.test_copper__DOT__dut__DOT__tile_inst__DOT__tregs[10U];
                                                                                __VdlyVal__test_copper__DOT__dut__DOT__tile_inst__DOT__col_buffer__v18 
                                                                                = vlSelfRef.test_copper__DOT__dut__DOT__tile_inst__DOT____Vlvbound_h02fc01d7__0;
                                                                                __VdlySet__test_copper__DOT__dut__DOT__tile_inst__DOT__col_buffer__v18 = 1U;
                                                                            }
                                                                            if (
                                                                                ((0x13U 
                                                                                >= vlSelfRef.test_copper__DOT__dut__DOT__tile_inst__DOT__tregs[8U]) 
                                                                                & (0x13U 
                                                                                <= vlSelfRef.test_copper__DOT__dut__DOT__tile_inst__DOT__tregs[9U]))) {
                                                                                vlSelfRef.test_copper__DOT__dut__DOT__tile_inst__DOT____Vlvbound_h02fc01d7__0 
                                                                                = vlSelfRef.test_copper__DOT__dut__DOT__tile_inst__DOT__tregs[10U];
                                                                                __VdlyVal__test_copper__DOT__dut__DOT__tile_inst__DOT__col_buffer__v19 
                                                                                = vlSelfRef.test_copper__DOT__dut__DOT__tile_inst__DOT____Vlvbound_h02fc01d7__0;
                                                                                __VdlySet__test_copper__DOT__dut__DOT__tile_inst__DOT__col_buffer__v19 = 1U;
                                                                            }
                                                                            if (
                                                                                ((0x14U 
                                                                                >= vlSelfRef.test_copper__DOT__dut__DOT__tile_inst__DOT__tregs[8U]) 
                                                                                & (0x14U 
                                                                                <= vlSelfRef.test_copper__DOT__dut__DOT__tile_inst__DOT__tregs[9U]))) {
                                                                                vlSelfRef.test_copper__DOT__dut__DOT__tile_inst__DOT____Vlvbound_h02fc01d7__0 
                                                                                = vlSelfRef.test_copper__DOT__dut__DOT__tile_inst__DOT__tregs[10U];
                                                                                __VdlyVal__test_copper__DOT__dut__DOT__tile_inst__DOT__col_buffer__v20 
                                                                                = vlSelfRef.test_copper__DOT__dut__DOT__tile_inst__DOT____Vlvbound_h02fc01d7__0;
                                                                                __VdlySet__test_copper__DOT__dut__DOT__tile_inst__DOT__col_buffer__v20 = 1U;
                                                                            }
                                                                            if (
                                                                                ((0x15U 
                                                                                >= vlSelfRef.test_copper__DOT__dut__DOT__tile_inst__DOT__tregs[8U]) 
                                                                                & (0x15U 
                                                                                <= vlSelfRef.test_copper__DOT__dut__DOT__tile_inst__DOT__tregs[9U]))) {
                                                                                vlSelfRef.test_copper__DOT__dut__DOT__tile_inst__DOT____Vlvbound_h02fc01d7__0 
                                                                                = vlSelfRef.test_copper__DOT__dut__DOT__tile_inst__DOT__tregs[10U];
                                                                                __VdlyVal__test_copper__DOT__dut__DOT__tile_inst__DOT__col_buffer__v21 
                                                                                = vlSelfRef.test_copper__DOT__dut__DOT__tile_inst__DOT____Vlvbound_h02fc01d7__0;
                                                                                __VdlySet__test_copper__DOT__dut__DOT__tile_inst__DOT__col_buffer__v21 = 1U;
                                                                            }
                                                                            if (
                                                                                ((0x16U 
                                                                                >= vlSelfRef.test_copper__DOT__dut__DOT__tile_inst__DOT__tregs[8U]) 
                                                                                & (0x16U 
                                                                                <= vlSelfRef.test_copper__DOT__dut__DOT__tile_inst__DOT__tregs[9U]))) {
                                                                                vlSelfRef.test_copper__DOT__dut__DOT__tile_inst__DOT____Vlvbound_h02fc01d7__0 
                                                                                = vlSelfRef.test_copper__DOT__dut__DOT__tile_inst__DOT__tregs[10U];
                                                                                __VdlyVal__test_copper__DOT__dut__DOT__tile_inst__DOT__col_buffer__v22 
                                                                                = vlSelfRef.test_copper__DOT__dut__DOT__tile_inst__DOT____Vlvbound_h02fc01d7__0;
                                                                                __VdlySet__test_copper__DOT__dut__DOT__tile_inst__DOT__col_buffer__v22 = 1U;
                                                                            }
                                                                            if (
                                                                                ((0x17U 
                                                                                >= vlSelfRef.test_copper__DOT__dut__DOT__tile_inst__DOT__tregs[8U]) 
                                                                                & (0x17U 
                                                                                <= vlSelfRef.test_copper__DOT__dut__DOT__tile_inst__DOT__tregs[9U]))) {
                                                                                vlSelfRef.test_copper__DOT__dut__DOT__tile_inst__DOT____Vlvbound_h02fc01d7__0 
                                                                                = vlSelfRef.test_copper__DOT__dut__DOT__tile_inst__DOT__tregs[10U];
                                                                                __VdlyVal__test_copper__DOT__dut__DOT__tile_inst__DOT__col_buffer__v23 
                                                                                = vlSelfRef.test_copper__DOT__dut__DOT__tile_inst__DOT____Vlvbound_h02fc01d7__0;
                                                                                __VdlySet__test_copper__DOT__dut__DOT__tile_inst__DOT__col_buffer__v23 = 1U;
                                                                            }
                                                                            if (
                                                                                ((0x18U 
                                                                                >= vlSelfRef.test_copper__DOT__dut__DOT__tile_inst__DOT__tregs[8U]) 
                                                                                & (0x18U 
                                                                                <= vlSelfRef.test_copper__DOT__dut__DOT__tile_inst__DOT__tregs[9U]))) {
                                                                                vlSelfRef.test_copper__DOT__dut__DOT__tile_inst__DOT____Vlvbound_h02fc01d7__0 
                                                                                = vlSelfRef.test_copper__DOT__dut__DOT__tile_inst__DOT__tregs[10U];
                                                                                __VdlyVal__test_copper__DOT__dut__DOT__tile_inst__DOT__col_buffer__v24 
                                                                                = vlSelfRef.test_copper__DOT__dut__DOT__tile_inst__DOT____Vlvbound_h02fc01d7__0;
                                                                                __VdlySet__test_copper__DOT__dut__DOT__tile_inst__DOT__col_buffer__v24 = 1U;
                                                                            }
                                                                        }
                                                                    } else if (
                                                                               (1U 
                                                                                & (IData)(vlSelfRef.test_copper__DOT__cpu_wdata))) {
                                                                        if (
                                                                            (0x19U 
                                                                             > vlSelfRef.test_copper__DOT__dut__DOT__tile_inst__DOT__tregs[8U])) {
                                                                            vlSelfRef.test_copper__DOT__dut__DOT__tile_inst__DOT____Vlvbound_h67f0dfac__0 
                                                                                = vlSelfRef.test_copper__DOT__dut__DOT__tile_inst__DOT__tregs[9U];
                                                                            if (
                                                                                (0x18U 
                                                                                >= 
                                                                                (0x0000001fU 
                                                                                & vlSelfRef.test_copper__DOT__dut__DOT__tile_inst__DOT__tregs[8U]))) {
                                                                                __VdlyVal__test_copper__DOT__dut__DOT__tile_inst__DOT__col_buffer__v25 
                                                                                = vlSelfRef.test_copper__DOT__dut__DOT__tile_inst__DOT____Vlvbound_h67f0dfac__0;
                                                                                __VdlyDim0__test_copper__DOT__dut__DOT__tile_inst__DOT__col_buffer__v25 
                                                                                = 
                                                                                (0x0000001fU 
                                                                                & vlSelfRef.test_copper__DOT__dut__DOT__tile_inst__DOT__tregs[8U]);
                                                                                __VdlySet__test_copper__DOT__dut__DOT__tile_inst__DOT__col_buffer__v25 = 1U;
                                                                            }
                                                                        }
                                                                    } else {
                                                                        vlSelfRef.test_copper__DOT__dut__DOT__tile_inst__DOT____Vlvbound_ha057723b__0 
                                                                            = vlSelfRef.test_copper__DOT__dut__DOT__tile_inst__DOT__tregs[8U];
                                                                        __VdlyVal__test_copper__DOT__dut__DOT__tile_inst__DOT__col_buffer__v26 
                                                                            = vlSelfRef.test_copper__DOT__dut__DOT__tile_inst__DOT____Vlvbound_ha057723b__0;
                                                                        __VdlySet__test_copper__DOT__dut__DOT__tile_inst__DOT__col_buffer__v26 = 1U;
                                                                        vlSelfRef.test_copper__DOT__dut__DOT__tile_inst__DOT____Vlvbound_ha057723b__0 
                                                                            = vlSelfRef.test_copper__DOT__dut__DOT__tile_inst__DOT__tregs[8U];
                                                                        __VdlyVal__test_copper__DOT__dut__DOT__tile_inst__DOT__col_buffer__v27 
                                                                            = vlSelfRef.test_copper__DOT__dut__DOT__tile_inst__DOT____Vlvbound_ha057723b__0;
                                                                        __VdlySet__test_copper__DOT__dut__DOT__tile_inst__DOT__col_buffer__v27 = 1U;
                                                                        vlSelfRef.test_copper__DOT__dut__DOT__tile_inst__DOT____Vlvbound_ha057723b__0 
                                                                            = vlSelfRef.test_copper__DOT__dut__DOT__tile_inst__DOT__tregs[8U];
                                                                        __VdlyVal__test_copper__DOT__dut__DOT__tile_inst__DOT__col_buffer__v28 
                                                                            = vlSelfRef.test_copper__DOT__dut__DOT__tile_inst__DOT____Vlvbound_ha057723b__0;
                                                                        __VdlySet__test_copper__DOT__dut__DOT__tile_inst__DOT__col_buffer__v28 = 1U;
                                                                        vlSelfRef.test_copper__DOT__dut__DOT__tile_inst__DOT____Vlvbound_ha057723b__0 
                                                                            = vlSelfRef.test_copper__DOT__dut__DOT__tile_inst__DOT__tregs[8U];
                                                                        __VdlyVal__test_copper__DOT__dut__DOT__tile_inst__DOT__col_buffer__v29 
                                                                            = vlSelfRef.test_copper__DOT__dut__DOT__tile_inst__DOT____Vlvbound_ha057723b__0;
                                                                        __VdlySet__test_copper__DOT__dut__DOT__tile_inst__DOT__col_buffer__v29 = 1U;
                                                                        vlSelfRef.test_copper__DOT__dut__DOT__tile_inst__DOT____Vlvbound_ha057723b__0 
                                                                            = vlSelfRef.test_copper__DOT__dut__DOT__tile_inst__DOT__tregs[8U];
                                                                        __VdlyVal__test_copper__DOT__dut__DOT__tile_inst__DOT__col_buffer__v30 
                                                                            = vlSelfRef.test_copper__DOT__dut__DOT__tile_inst__DOT____Vlvbound_ha057723b__0;
                                                                        __VdlySet__test_copper__DOT__dut__DOT__tile_inst__DOT__col_buffer__v30 = 1U;
                                                                        vlSelfRef.test_copper__DOT__dut__DOT__tile_inst__DOT____Vlvbound_ha057723b__0 
                                                                            = vlSelfRef.test_copper__DOT__dut__DOT__tile_inst__DOT__tregs[8U];
                                                                        __VdlyVal__test_copper__DOT__dut__DOT__tile_inst__DOT__col_buffer__v31 
                                                                            = vlSelfRef.test_copper__DOT__dut__DOT__tile_inst__DOT____Vlvbound_ha057723b__0;
                                                                        __VdlySet__test_copper__DOT__dut__DOT__tile_inst__DOT__col_buffer__v31 = 1U;
                                                                        vlSelfRef.test_copper__DOT__dut__DOT__tile_inst__DOT____Vlvbound_ha057723b__0 
                                                                            = vlSelfRef.test_copper__DOT__dut__DOT__tile_inst__DOT__tregs[8U];
                                                                        __VdlyVal__test_copper__DOT__dut__DOT__tile_inst__DOT__col_buffer__v32 
                                                                            = vlSelfRef.test_copper__DOT__dut__DOT__tile_inst__DOT____Vlvbound_ha057723b__0;
                                                                        __VdlySet__test_copper__DOT__dut__DOT__tile_inst__DOT__col_buffer__v32 = 1U;
                                                                        vlSelfRef.test_copper__DOT__dut__DOT__tile_inst__DOT____Vlvbound_ha057723b__0 
                                                                            = vlSelfRef.test_copper__DOT__dut__DOT__tile_inst__DOT__tregs[8U];
                                                                        __VdlyVal__test_copper__DOT__dut__DOT__tile_inst__DOT__col_buffer__v33 
                                                                            = vlSelfRef.test_copper__DOT__dut__DOT__tile_inst__DOT____Vlvbound_ha057723b__0;
                                                                        __VdlySet__test_copper__DOT__dut__DOT__tile_inst__DOT__col_buffer__v33 = 1U;
                                                                        vlSelfRef.test_copper__DOT__dut__DOT__tile_inst__DOT____Vlvbound_ha057723b__0 
                                                                            = vlSelfRef.test_copper__DOT__dut__DOT__tile_inst__DOT__tregs[8U];
                                                                        __VdlyVal__test_copper__DOT__dut__DOT__tile_inst__DOT__col_buffer__v34 
                                                                            = vlSelfRef.test_copper__DOT__dut__DOT__tile_inst__DOT____Vlvbound_ha057723b__0;
                                                                        __VdlySet__test_copper__DOT__dut__DOT__tile_inst__DOT__col_buffer__v34 = 1U;
                                                                        vlSelfRef.test_copper__DOT__dut__DOT__tile_inst__DOT____Vlvbound_ha057723b__0 
                                                                            = vlSelfRef.test_copper__DOT__dut__DOT__tile_inst__DOT__tregs[8U];
                                                                        __VdlyVal__test_copper__DOT__dut__DOT__tile_inst__DOT__col_buffer__v35 
                                                                            = vlSelfRef.test_copper__DOT__dut__DOT__tile_inst__DOT____Vlvbound_ha057723b__0;
                                                                        __VdlySet__test_copper__DOT__dut__DOT__tile_inst__DOT__col_buffer__v35 = 1U;
                                                                        vlSelfRef.test_copper__DOT__dut__DOT__tile_inst__DOT____Vlvbound_ha057723b__0 
                                                                            = vlSelfRef.test_copper__DOT__dut__DOT__tile_inst__DOT__tregs[8U];
                                                                        __VdlyVal__test_copper__DOT__dut__DOT__tile_inst__DOT__col_buffer__v36 
                                                                            = vlSelfRef.test_copper__DOT__dut__DOT__tile_inst__DOT____Vlvbound_ha057723b__0;
                                                                        __VdlySet__test_copper__DOT__dut__DOT__tile_inst__DOT__col_buffer__v36 = 1U;
                                                                        vlSelfRef.test_copper__DOT__dut__DOT__tile_inst__DOT____Vlvbound_ha057723b__0 
                                                                            = vlSelfRef.test_copper__DOT__dut__DOT__tile_inst__DOT__tregs[8U];
                                                                        __VdlyVal__test_copper__DOT__dut__DOT__tile_inst__DOT__col_buffer__v37 
                                                                            = vlSelfRef.test_copper__DOT__dut__DOT__tile_inst__DOT____Vlvbound_ha057723b__0;
                                                                        __VdlySet__test_copper__DOT__dut__DOT__tile_inst__DOT__col_buffer__v37 = 1U;
                                                                        vlSelfRef.test_copper__DOT__dut__DOT__tile_inst__DOT____Vlvbound_ha057723b__0 
                                                                            = vlSelfRef.test_copper__DOT__dut__DOT__tile_inst__DOT__tregs[8U];
                                                                        __VdlyVal__test_copper__DOT__dut__DOT__tile_inst__DOT__col_buffer__v38 
                                                                            = vlSelfRef.test_copper__DOT__dut__DOT__tile_inst__DOT____Vlvbound_ha057723b__0;
                                                                        __VdlySet__test_copper__DOT__dut__DOT__tile_inst__DOT__col_buffer__v38 = 1U;
                                                                        vlSelfRef.test_copper__DOT__dut__DOT__tile_inst__DOT____Vlvbound_ha057723b__0 
                                                                            = vlSelfRef.test_copper__DOT__dut__DOT__tile_inst__DOT__tregs[8U];
                                                                        __VdlyVal__test_copper__DOT__dut__DOT__tile_inst__DOT__col_buffer__v39 
                                                                            = vlSelfRef.test_copper__DOT__dut__DOT__tile_inst__DOT____Vlvbound_ha057723b__0;
                                                                        __VdlySet__test_copper__DOT__dut__DOT__tile_inst__DOT__col_buffer__v39 = 1U;
                                                                        vlSelfRef.test_copper__DOT__dut__DOT__tile_inst__DOT____Vlvbound_ha057723b__0 
                                                                            = vlSelfRef.test_copper__DOT__dut__DOT__tile_inst__DOT__tregs[8U];
                                                                        __VdlyVal__test_copper__DOT__dut__DOT__tile_inst__DOT__col_buffer__v40 
                                                                            = vlSelfRef.test_copper__DOT__dut__DOT__tile_inst__DOT____Vlvbound_ha057723b__0;
                                                                        __VdlySet__test_copper__DOT__dut__DOT__tile_inst__DOT__col_buffer__v40 = 1U;
                                                                        vlSelfRef.test_copper__DOT__dut__DOT__tile_inst__DOT____Vlvbound_ha057723b__0 
                                                                            = vlSelfRef.test_copper__DOT__dut__DOT__tile_inst__DOT__tregs[8U];
                                                                        __VdlyVal__test_copper__DOT__dut__DOT__tile_inst__DOT__col_buffer__v41 
                                                                            = vlSelfRef.test_copper__DOT__dut__DOT__tile_inst__DOT____Vlvbound_ha057723b__0;
                                                                        __VdlySet__test_copper__DOT__dut__DOT__tile_inst__DOT__col_buffer__v41 = 1U;
                                                                        vlSelfRef.test_copper__DOT__dut__DOT__tile_inst__DOT____Vlvbound_ha057723b__0 
                                                                            = vlSelfRef.test_copper__DOT__dut__DOT__tile_inst__DOT__tregs[8U];
                                                                        __VdlyVal__test_copper__DOT__dut__DOT__tile_inst__DOT__col_buffer__v42 
                                                                            = vlSelfRef.test_copper__DOT__dut__DOT__tile_inst__DOT____Vlvbound_ha057723b__0;
                                                                        __VdlySet__test_copper__DOT__dut__DOT__tile_inst__DOT__col_buffer__v42 = 1U;
                                                                        vlSelfRef.test_copper__DOT__dut__DOT__tile_inst__DOT____Vlvbound_ha057723b__0 
                                                                            = vlSelfRef.test_copper__DOT__dut__DOT__tile_inst__DOT__tregs[8U];
                                                                        __VdlyVal__test_copper__DOT__dut__DOT__tile_inst__DOT__col_buffer__v43 
                                                                            = vlSelfRef.test_copper__DOT__dut__DOT__tile_inst__DOT____Vlvbound_ha057723b__0;
                                                                        __VdlySet__test_copper__DOT__dut__DOT__tile_inst__DOT__col_buffer__v43 = 1U;
                                                                        vlSelfRef.test_copper__DOT__dut__DOT__tile_inst__DOT____Vlvbound_ha057723b__0 
                                                                            = vlSelfRef.test_copper__DOT__dut__DOT__tile_inst__DOT__tregs[8U];
                                                                        __VdlyVal__test_copper__DOT__dut__DOT__tile_inst__DOT__col_buffer__v44 
                                                                            = vlSelfRef.test_copper__DOT__dut__DOT__tile_inst__DOT____Vlvbound_ha057723b__0;
                                                                        __VdlySet__test_copper__DOT__dut__DOT__tile_inst__DOT__col_buffer__v44 = 1U;
                                                                        vlSelfRef.test_copper__DOT__dut__DOT__tile_inst__DOT____Vlvbound_ha057723b__0 
                                                                            = vlSelfRef.test_copper__DOT__dut__DOT__tile_inst__DOT__tregs[8U];
                                                                        __VdlyVal__test_copper__DOT__dut__DOT__tile_inst__DOT__col_buffer__v45 
                                                                            = vlSelfRef.test_copper__DOT__dut__DOT__tile_inst__DOT____Vlvbound_ha057723b__0;
                                                                        __VdlySet__test_copper__DOT__dut__DOT__tile_inst__DOT__col_buffer__v45 = 1U;
                                                                        vlSelfRef.test_copper__DOT__dut__DOT__tile_inst__DOT____Vlvbound_ha057723b__0 
                                                                            = vlSelfRef.test_copper__DOT__dut__DOT__tile_inst__DOT__tregs[8U];
                                                                        __VdlyVal__test_copper__DOT__dut__DOT__tile_inst__DOT__col_buffer__v46 
                                                                            = vlSelfRef.test_copper__DOT__dut__DOT__tile_inst__DOT____Vlvbound_ha057723b__0;
                                                                        __VdlySet__test_copper__DOT__dut__DOT__tile_inst__DOT__col_buffer__v46 = 1U;
                                                                        vlSelfRef.test_copper__DOT__dut__DOT__tile_inst__DOT____Vlvbound_ha057723b__0 
                                                                            = vlSelfRef.test_copper__DOT__dut__DOT__tile_inst__DOT__tregs[8U];
                                                                        __VdlyVal__test_copper__DOT__dut__DOT__tile_inst__DOT__col_buffer__v47 
                                                                            = vlSelfRef.test_copper__DOT__dut__DOT__tile_inst__DOT____Vlvbound_ha057723b__0;
                                                                        __VdlySet__test_copper__DOT__dut__DOT__tile_inst__DOT__col_buffer__v47 = 1U;
                                                                        vlSelfRef.test_copper__DOT__dut__DOT__tile_inst__DOT____Vlvbound_ha057723b__0 
                                                                            = vlSelfRef.test_copper__DOT__dut__DOT__tile_inst__DOT__tregs[8U];
                                                                        __VdlyVal__test_copper__DOT__dut__DOT__tile_inst__DOT__col_buffer__v48 
                                                                            = vlSelfRef.test_copper__DOT__dut__DOT__tile_inst__DOT____Vlvbound_ha057723b__0;
                                                                        __VdlySet__test_copper__DOT__dut__DOT__tile_inst__DOT__col_buffer__v48 = 1U;
                                                                        vlSelfRef.test_copper__DOT__dut__DOT__tile_inst__DOT____Vlvbound_ha057723b__0 
                                                                            = vlSelfRef.test_copper__DOT__dut__DOT__tile_inst__DOT__tregs[8U];
                                                                        __VdlyVal__test_copper__DOT__dut__DOT__tile_inst__DOT__col_buffer__v49 
                                                                            = vlSelfRef.test_copper__DOT__dut__DOT__tile_inst__DOT____Vlvbound_ha057723b__0;
                                                                        __VdlySet__test_copper__DOT__dut__DOT__tile_inst__DOT__col_buffer__v49 = 1U;
                                                                        vlSelfRef.test_copper__DOT__dut__DOT__tile_inst__DOT____Vlvbound_ha057723b__0 
                                                                            = vlSelfRef.test_copper__DOT__dut__DOT__tile_inst__DOT__tregs[8U];
                                                                        __VdlyVal__test_copper__DOT__dut__DOT__tile_inst__DOT__col_buffer__v50 
                                                                            = vlSelfRef.test_copper__DOT__dut__DOT__tile_inst__DOT____Vlvbound_ha057723b__0;
                                                                        __VdlySet__test_copper__DOT__dut__DOT__tile_inst__DOT__col_buffer__v50 = 1U;
                                                                    }
                                                                }
                                                            }
                                                        } else if (
                                                                   (8U 
                                                                    & (IData)(vlSelfRef.test_copper__DOT__cpu_wdata))) {
                                                            if (
                                                                (4U 
                                                                 & (IData)(vlSelfRef.test_copper__DOT__cpu_wdata))) {
                                                                if (
                                                                    (2U 
                                                                     & (IData)(vlSelfRef.test_copper__DOT__cpu_wdata))) {
                                                                    if (
                                                                        (1U 
                                                                         & (IData)(vlSelfRef.test_copper__DOT__cpu_wdata))) {
                                                                        __Vdly__test_copper__DOT__dut__DOT__tile_inst__DOT__dma_count = 0U;
                                                                        __Vdly__test_copper__DOT__dut__DOT__tile_inst__DOT__dma_state = 9U;
                                                                    } else {
                                                                        __Vdly__test_copper__DOT__dut__DOT__tile_inst__DOT__dma_src 
                                                                            = 
                                                                            ((vlSelfRef.test_copper__DOT__dut__DOT__tile_inst__DOT__tregs[13U] 
                                                                              << 8U) 
                                                                             | vlSelfRef.test_copper__DOT__dut__DOT__tile_inst__DOT__tregs[12U]);
                                                                        vlSelfRef.test_copper__DOT__dut__DOT__tile_inst__DOT__dma_nt 
                                                                            = 
                                                                            (3U 
                                                                             & vlSelfRef.test_copper__DOT__dut__DOT__tile_inst__DOT__tregs[8U]);
                                                                        vlSelfRef.test_copper__DOT__dut__DOT__tile_inst__DOT__dma_col_row 
                                                                            = 
                                                                            (0x0000003fU 
                                                                             & vlSelfRef.test_copper__DOT__dut__DOT__tile_inst__DOT__tregs[9U]);
                                                                        __Vdly__test_copper__DOT__dut__DOT__tile_inst__DOT__dma_col_idx = 0U;
                                                                        __Vdly__test_copper__DOT__dut__DOT__tile_inst__DOT__dma_count = 0x0019U;
                                                                        __Vdly__test_copper__DOT__dut__DOT__tile_inst__DOT__dma_state = 8U;
                                                                        __Vdly__test_copper__DOT__dut__DOT__tile_inst__DOT__dma_data_valid = 0U;
                                                                    }
                                                                } else if (
                                                                           (1U 
                                                                            & (IData)(vlSelfRef.test_copper__DOT__cpu_wdata))) {
                                                                    __Vdly__test_copper__DOT__dut__DOT__tile_inst__DOT__dma_src 
                                                                        = 
                                                                        ((vlSelfRef.test_copper__DOT__dut__DOT__tile_inst__DOT__tregs[13U] 
                                                                          << 8U) 
                                                                         | vlSelfRef.test_copper__DOT__dut__DOT__tile_inst__DOT__tregs[12U]);
                                                                    __Vdly__test_copper__DOT__dut__DOT__tile_inst__DOT__dma_dst = 0U;
                                                                    vlSelfRef.test_copper__DOT__dut__DOT__tile_inst__DOT__dma_nt 
                                                                        = 
                                                                        (3U 
                                                                         & vlSelfRef.test_copper__DOT__dut__DOT__tile_inst__DOT__tregs[8U]);
                                                                    vlSelfRef.test_copper__DOT__dut__DOT__tile_inst__DOT__dma_col_row 
                                                                        = 
                                                                        (0x0000003fU 
                                                                         & vlSelfRef.test_copper__DOT__dut__DOT__tile_inst__DOT__tregs[9U]);
                                                                    __Vdly__test_copper__DOT__dut__DOT__tile_inst__DOT__dma_count = 0x0028U;
                                                                    __Vdly__test_copper__DOT__dut__DOT__tile_inst__DOT__dma_state = 7U;
                                                                    __Vdly__test_copper__DOT__dut__DOT__tile_inst__DOT__dma_data_valid = 0U;
                                                                }
                                                            } else if (
                                                                       (2U 
                                                                        & (IData)(vlSelfRef.test_copper__DOT__cpu_wdata))) {
                                                                if (
                                                                    (1U 
                                                                     & (IData)(vlSelfRef.test_copper__DOT__cpu_wdata))) {
                                                                    __VdlyVal__test_copper__DOT__dut__DOT__tile_inst__DOT__pal_ram__v1 
                                                                        = 
                                                                        ((0x00000f00U 
                                                                          & (vlSelfRef.test_copper__DOT__dut__DOT__tile_inst__DOT__tregs[8U] 
                                                                             << 4U)) 
                                                                         | ((0x000000f0U 
                                                                             & vlSelfRef.test_copper__DOT__dut__DOT__tile_inst__DOT__tregs[9U]) 
                                                                            | (0x0000000fU 
                                                                               & (vlSelfRef.test_copper__DOT__dut__DOT__tile_inst__DOT__tregs[10U] 
                                                                                >> 4U))));
                                                                    __VdlyDim0__test_copper__DOT__dut__DOT__tile_inst__DOT__pal_ram__v1 
                                                                        = 
                                                                        ((0x000000f0U 
                                                                          & (vlSelfRef.test_copper__DOT__dut__DOT__tile_inst__DOT__tregs[14U] 
                                                                             << 4U)) 
                                                                         | (0x0000000fU 
                                                                            & vlSelfRef.test_copper__DOT__dut__DOT__tile_inst__DOT__tregs[15U]));
                                                                    __VdlySet__test_copper__DOT__dut__DOT__tile_inst__DOT__pal_ram__v1 = 1U;
                                                                } else {
                                                                    __Vdly__test_copper__DOT__dut__DOT__tile_inst__DOT__dma_src 
                                                                        = 
                                                                        ((vlSelfRef.test_copper__DOT__dut__DOT__tile_inst__DOT__tregs[13U] 
                                                                          << 8U) 
                                                                         | vlSelfRef.test_copper__DOT__dut__DOT__tile_inst__DOT__tregs[12U]);
                                                                    vlSelfRef.test_copper__DOT__dut__DOT__tile_inst__DOT__dma_pal_cidx = 0U;
                                                                    vlSelfRef.test_copper__DOT__dut__DOT__tile_inst__DOT__dma_pal_sub 
                                                                        = 
                                                                        (0x0000000fU 
                                                                         & vlSelfRef.test_copper__DOT__dut__DOT__tile_inst__DOT__tregs[14U]);
                                                                    __Vdly__test_copper__DOT__dut__DOT__tile_inst__DOT__dma_pal_phase = 0U;
                                                                    __Vdly__test_copper__DOT__dut__DOT__tile_inst__DOT__dma_count = 0x0030U;
                                                                    __Vdly__test_copper__DOT__dut__DOT__tile_inst__DOT__dma_state = 4U;
                                                                    __Vdly__test_copper__DOT__dut__DOT__tile_inst__DOT__dma_data_valid = 0U;
                                                                }
                                                            } else if (
                                                                       (1U 
                                                                        & (IData)(vlSelfRef.test_copper__DOT__cpu_wdata))) {
                                                                __Vdly__test_copper__DOT__dut__DOT__tile_inst__DOT__dma_src 
                                                                    = 
                                                                    ((vlSelfRef.test_copper__DOT__dut__DOT__tile_inst__DOT__tregs[13U] 
                                                                      << 8U) 
                                                                     | vlSelfRef.test_copper__DOT__dut__DOT__tile_inst__DOT__tregs[12U]);
                                                                __Vdly__test_copper__DOT__dut__DOT__tile_inst__DOT__dma_dst = 0U;
                                                                vlSelfRef.test_copper__DOT__dut__DOT__tile_inst__DOT__dma_nt 
                                                                    = 
                                                                    (3U 
                                                                     & vlSelfRef.test_copper__DOT__dut__DOT__tile_inst__DOT__tregs[8U]);
                                                                __Vdly__test_copper__DOT__dut__DOT__tile_inst__DOT__dma_count = 0x03e8U;
                                                                __Vdly__test_copper__DOT__dut__DOT__tile_inst__DOT__dma_state = 3U;
                                                                __Vdly__test_copper__DOT__dut__DOT__tile_inst__DOT__dma_data_valid = 0U;
                                                            } else {
                                                                __Vdly__test_copper__DOT__dut__DOT__tile_inst__DOT__dma_src 
                                                                    = 
                                                                    ((vlSelfRef.test_copper__DOT__dut__DOT__tile_inst__DOT__tregs[13U] 
                                                                      << 8U) 
                                                                     | vlSelfRef.test_copper__DOT__dut__DOT__tile_inst__DOT__tregs[12U]);
                                                                __Vdly__test_copper__DOT__dut__DOT__tile_inst__DOT__dma_dst = 0U;
                                                                vlSelfRef.test_copper__DOT__dut__DOT__tile_inst__DOT__dma_nt 
                                                                    = 
                                                                    (3U 
                                                                     & vlSelfRef.test_copper__DOT__dut__DOT__tile_inst__DOT__tregs[8U]);
                                                                __Vdly__test_copper__DOT__dut__DOT__tile_inst__DOT__dma_count = 0x03e8U;
                                                                __Vdly__test_copper__DOT__dut__DOT__tile_inst__DOT__dma_state = 2U;
                                                                __Vdly__test_copper__DOT__dut__DOT__tile_inst__DOT__dma_data_valid = 0U;
                                                            }
                                                        } else if (
                                                                   (4U 
                                                                    & (IData)(vlSelfRef.test_copper__DOT__cpu_wdata))) {
                                                            if (
                                                                (2U 
                                                                 & (IData)(vlSelfRef.test_copper__DOT__cpu_wdata))) {
                                                                if (
                                                                    (1U 
                                                                     & (IData)(vlSelfRef.test_copper__DOT__cpu_wdata))) {
                                                                    __Vdly__test_copper__DOT__dut__DOT__tile_inst__DOT__dma_src 
                                                                        = 
                                                                        ((vlSelfRef.test_copper__DOT__dut__DOT__tile_inst__DOT__tregs[13U] 
                                                                          << 8U) 
                                                                         | vlSelfRef.test_copper__DOT__dut__DOT__tile_inst__DOT__tregs[12U]);
                                                                    vlSelfRef.test_copper__DOT__dut__DOT__tile_inst__DOT__dma_nt 
                                                                        = 
                                                                        (3U 
                                                                         & vlSelfRef.test_copper__DOT__dut__DOT__tile_inst__DOT__tregs[8U]);
                                                                    vlSelfRef.test_copper__DOT__dut__DOT__tile_inst__DOT__dma_col_row 
                                                                        = 
                                                                        (0x0000003fU 
                                                                         & vlSelfRef.test_copper__DOT__dut__DOT__tile_inst__DOT__tregs[9U]);
                                                                    __Vdly__test_copper__DOT__dut__DOT__tile_inst__DOT__dma_col_idx = 0U;
                                                                    __Vdly__test_copper__DOT__dut__DOT__tile_inst__DOT__dma_count = 0x0019U;
                                                                    __Vdly__test_copper__DOT__dut__DOT__tile_inst__DOT__dma_state = 6U;
                                                                    __Vdly__test_copper__DOT__dut__DOT__tile_inst__DOT__dma_data_valid = 0U;
                                                                } else {
                                                                    __Vdly__test_copper__DOT__dut__DOT__tile_inst__DOT__dma_src 
                                                                        = 
                                                                        ((vlSelfRef.test_copper__DOT__dut__DOT__tile_inst__DOT__tregs[13U] 
                                                                          << 8U) 
                                                                         | vlSelfRef.test_copper__DOT__dut__DOT__tile_inst__DOT__tregs[12U]);
                                                                    __Vdly__test_copper__DOT__dut__DOT__tile_inst__DOT__dma_dst = 0U;
                                                                    vlSelfRef.test_copper__DOT__dut__DOT__tile_inst__DOT__dma_nt 
                                                                        = 
                                                                        (3U 
                                                                         & vlSelfRef.test_copper__DOT__dut__DOT__tile_inst__DOT__tregs[8U]);
                                                                    vlSelfRef.test_copper__DOT__dut__DOT__tile_inst__DOT__dma_col_row 
                                                                        = 
                                                                        (0x0000003fU 
                                                                         & vlSelfRef.test_copper__DOT__dut__DOT__tile_inst__DOT__tregs[9U]);
                                                                    __Vdly__test_copper__DOT__dut__DOT__tile_inst__DOT__dma_count = 0x0028U;
                                                                    __Vdly__test_copper__DOT__dut__DOT__tile_inst__DOT__dma_state = 5U;
                                                                    __Vdly__test_copper__DOT__dut__DOT__tile_inst__DOT__dma_data_valid = 0U;
                                                                }
                                                            } else if (
                                                                       (1U 
                                                                        & (IData)(vlSelfRef.test_copper__DOT__cpu_wdata))) {
                                                                if (
                                                                    (4U 
                                                                     > vlSelfRef.test_copper__DOT__dut__DOT__tile_inst__DOT__tregs[8U])) {
                                                                    vlSelfRef.test_copper__DOT__dut__DOT__tile_inst__DOT__unnamedblk6__DOT__i = 0U;
                                                                    while (
                                                                           VL_GTS_III(32, 0x000003e8U, vlSelfRef.test_copper__DOT__dut__DOT__tile_inst__DOT__unnamedblk6__DOT__i)) {
                                                                        vlSelfRef.test_copper__DOT__dut__DOT__tile_inst__DOT____Vlvbound_h8898988b__0 
                                                                            = vlSelfRef.test_copper__DOT__dut__DOT__tile_inst__DOT__tregs[9U];
                                                                        if (VL_LIKELY((
                                                                                (0x0f9fU 
                                                                                >= 
                                                                                (0x00000fffU 
                                                                                & (((IData)(0x000003e8U) 
                                                                                * vlSelfRef.test_copper__DOT__dut__DOT__tile_inst__DOT__tregs[8U]) 
                                                                                + vlSelfRef.test_copper__DOT__dut__DOT__tile_inst__DOT__unnamedblk6__DOT__i)))))) {
                                                                            __VdlyVal__test_copper__DOT__dut__DOT__tile_inst__DOT__nametable__v29 
                                                                                = vlSelfRef.test_copper__DOT__dut__DOT__tile_inst__DOT____Vlvbound_h8898988b__0;
                                                                            __VdlyDim0__test_copper__DOT__dut__DOT__tile_inst__DOT__nametable__v29 
                                                                                = 
                                                                                (0x00000fffU 
                                                                                & (((IData)(0x000003e8U) 
                                                                                * vlSelfRef.test_copper__DOT__dut__DOT__tile_inst__DOT__tregs[8U]) 
                                                                                + vlSelfRef.test_copper__DOT__dut__DOT__tile_inst__DOT__unnamedblk6__DOT__i));
                                                                            vlSelfRef.__VdlyCommitQueuetest_copper__DOT__dut__DOT__tile_inst__DOT__nametable.enqueue(__VdlyVal__test_copper__DOT__dut__DOT__tile_inst__DOT__nametable__v29, (IData)(__VdlyDim0__test_copper__DOT__dut__DOT__tile_inst__DOT__nametable__v29));
                                                                        }
                                                                        vlSelfRef.test_copper__DOT__dut__DOT__tile_inst__DOT__unnamedblk6__DOT__i 
                                                                            = 
                                                                            ((IData)(1U) 
                                                                             + vlSelfRef.test_copper__DOT__dut__DOT__tile_inst__DOT__unnamedblk6__DOT__i);
                                                                    }
                                                                }
                                                            } else if (
                                                                       (((4U 
                                                                          > vlSelfRef.test_copper__DOT__dut__DOT__tile_inst__DOT__tregs[8U]) 
                                                                         & (0x28U 
                                                                            > vlSelfRef.test_copper__DOT__dut__DOT__tile_inst__DOT__tregs[9U])) 
                                                                        & (0x19U 
                                                                           > vlSelfRef.test_copper__DOT__dut__DOT__tile_inst__DOT__tregs[10U]))) {
                                                                vlSelfRef.test_copper__DOT__dut__DOT__tile_inst__DOT____Vlvbound_ha3d87205__0 
                                                                    = vlSelfRef.test_copper__DOT__dut__DOT__tile_inst__DOT__tregs[11U];
                                                                if (
                                                                    (0x0f9fU 
                                                                     >= 
                                                                     (0x00000fffU 
                                                                      & ((((IData)(0x000003e8U) 
                                                                           * vlSelfRef.test_copper__DOT__dut__DOT__tile_inst__DOT__tregs[8U]) 
                                                                          + 
                                                                          ((IData)(0x00000028U) 
                                                                           * vlSelfRef.test_copper__DOT__dut__DOT__tile_inst__DOT__tregs[10U])) 
                                                                         + vlSelfRef.test_copper__DOT__dut__DOT__tile_inst__DOT__tregs[9U])))) {
                                                                    __VdlyVal__test_copper__DOT__dut__DOT__tile_inst__DOT__attr_table__v4 
                                                                        = vlSelfRef.test_copper__DOT__dut__DOT__tile_inst__DOT____Vlvbound_ha3d87205__0;
                                                                    __VdlyDim0__test_copper__DOT__dut__DOT__tile_inst__DOT__attr_table__v4 
                                                                        = 
                                                                        (0x00000fffU 
                                                                         & ((((IData)(0x000003e8U) 
                                                                              * vlSelfRef.test_copper__DOT__dut__DOT__tile_inst__DOT__tregs[8U]) 
                                                                             + 
                                                                             ((IData)(0x00000028U) 
                                                                              * vlSelfRef.test_copper__DOT__dut__DOT__tile_inst__DOT__tregs[10U])) 
                                                                            + vlSelfRef.test_copper__DOT__dut__DOT__tile_inst__DOT__tregs[9U]));
                                                                    __VdlySet__test_copper__DOT__dut__DOT__tile_inst__DOT__attr_table__v4 = 1U;
                                                                }
                                                            }
                                                        } else if (
                                                                   (2U 
                                                                    & (IData)(vlSelfRef.test_copper__DOT__cpu_wdata))) {
                                                            if (
                                                                (1U 
                                                                 & (IData)(vlSelfRef.test_copper__DOT__cpu_wdata))) {
                                                                if (
                                                                    (((4U 
                                                                       > vlSelfRef.test_copper__DOT__dut__DOT__tile_inst__DOT__tregs[8U]) 
                                                                      & (0x28U 
                                                                         > vlSelfRef.test_copper__DOT__dut__DOT__tile_inst__DOT__tregs[9U])) 
                                                                     & (0x19U 
                                                                        > vlSelfRef.test_copper__DOT__dut__DOT__tile_inst__DOT__tregs[10U]))) {
                                                                    vlSelfRef.test_copper__DOT__dut__DOT__tile_inst__DOT____Vlvbound_h353a15af__0 
                                                                        = vlSelfRef.test_copper__DOT__dut__DOT__tile_inst__DOT__tregs[11U];
                                                                    if (
                                                                        (0x0f9fU 
                                                                         >= 
                                                                         (0x00000fffU 
                                                                          & ((((IData)(0x000003e8U) 
                                                                               * vlSelfRef.test_copper__DOT__dut__DOT__tile_inst__DOT__tregs[8U]) 
                                                                              + 
                                                                              ((IData)(0x00000028U) 
                                                                               * vlSelfRef.test_copper__DOT__dut__DOT__tile_inst__DOT__tregs[10U])) 
                                                                             + vlSelfRef.test_copper__DOT__dut__DOT__tile_inst__DOT__tregs[9U])))) {
                                                                        __VdlyVal__test_copper__DOT__dut__DOT__tile_inst__DOT__nametable__v30 
                                                                            = vlSelfRef.test_copper__DOT__dut__DOT__tile_inst__DOT____Vlvbound_h353a15af__0;
                                                                        __VdlyDim0__test_copper__DOT__dut__DOT__tile_inst__DOT__nametable__v30 
                                                                            = 
                                                                            (0x00000fffU 
                                                                             & ((((IData)(0x000003e8U) 
                                                                                * vlSelfRef.test_copper__DOT__dut__DOT__tile_inst__DOT__tregs[8U]) 
                                                                                + 
                                                                                ((IData)(0x00000028U) 
                                                                                * vlSelfRef.test_copper__DOT__dut__DOT__tile_inst__DOT__tregs[10U])) 
                                                                                + vlSelfRef.test_copper__DOT__dut__DOT__tile_inst__DOT__tregs[9U]));
                                                                        vlSelfRef.__VdlyCommitQueuetest_copper__DOT__dut__DOT__tile_inst__DOT__nametable.enqueue(__VdlyVal__test_copper__DOT__dut__DOT__tile_inst__DOT__nametable__v30, (IData)(__VdlyDim0__test_copper__DOT__dut__DOT__tile_inst__DOT__nametable__v30));
                                                                    }
                                                                }
                                                            } else {
                                                                __Vdly__test_copper__DOT__dut__DOT__tile_inst__DOT__dma_src 
                                                                    = 
                                                                    ((vlSelfRef.test_copper__DOT__dut__DOT__tile_inst__DOT__tregs[13U] 
                                                                      << 8U) 
                                                                     | vlSelfRef.test_copper__DOT__dut__DOT__tile_inst__DOT__tregs[12U]);
                                                                if (vlSelfRef.test_copper__DOT__dut__DOT__tile_inst__DOT__tile_size16) {
                                                                    __Vdly__test_copper__DOT__dut__DOT__tile_inst__DOT__dma_dst 
                                                                        = 
                                                                        (vlSelfRef.test_copper__DOT__dut__DOT__tile_inst__DOT__tregs[8U] 
                                                                         << 7U);
                                                                    __Vdly__test_copper__DOT__dut__DOT__tile_inst__DOT__dma_count 
                                                                        = 
                                                                        (0x0000ffffU 
                                                                         & VL_SHIFTL_III(32,32,32, vlSelfRef.test_copper__DOT__dut__DOT__tile_inst__DOT__tregs[9U], 7U));
                                                                } else {
                                                                    __Vdly__test_copper__DOT__dut__DOT__tile_inst__DOT__dma_dst 
                                                                        = 
                                                                        (vlSelfRef.test_copper__DOT__dut__DOT__tile_inst__DOT__tregs[8U] 
                                                                         << 5U);
                                                                    __Vdly__test_copper__DOT__dut__DOT__tile_inst__DOT__dma_count 
                                                                        = 
                                                                        (0x0000ffffU 
                                                                         & VL_SHIFTL_III(32,32,32, vlSelfRef.test_copper__DOT__dut__DOT__tile_inst__DOT__tregs[9U], 5U));
                                                                }
                                                                __Vdly__test_copper__DOT__dut__DOT__tile_inst__DOT__dma_state = 1U;
                                                                __Vdly__test_copper__DOT__dut__DOT__tile_inst__DOT__dma_data_valid = 0U;
                                                            }
                                                        } else if (
                                                                   (1U 
                                                                    & (IData)(vlSelfRef.test_copper__DOT__cpu_wdata))) {
                                                            __Vdly__test_copper__DOT__dut__DOT__tile_inst__DOT__dma_src 
                                                                = 
                                                                ((vlSelfRef.test_copper__DOT__dut__DOT__tile_inst__DOT__tregs[13U] 
                                                                  << 8U) 
                                                                 | vlSelfRef.test_copper__DOT__dut__DOT__tile_inst__DOT__tregs[12U]);
                                                            if (vlSelfRef.test_copper__DOT__dut__DOT__tile_inst__DOT__tile_size16) {
                                                                __Vdly__test_copper__DOT__dut__DOT__tile_inst__DOT__dma_dst 
                                                                    = 
                                                                    (vlSelfRef.test_copper__DOT__dut__DOT__tile_inst__DOT__tregs[8U] 
                                                                     << 7U);
                                                                __Vdly__test_copper__DOT__dut__DOT__tile_inst__DOT__dma_count = 0x0080U;
                                                            } else {
                                                                __Vdly__test_copper__DOT__dut__DOT__tile_inst__DOT__dma_dst 
                                                                    = 
                                                                    (vlSelfRef.test_copper__DOT__dut__DOT__tile_inst__DOT__tregs[8U] 
                                                                     << 5U);
                                                                __Vdly__test_copper__DOT__dut__DOT__tile_inst__DOT__dma_count = 0x0020U;
                                                            }
                                                            __Vdly__test_copper__DOT__dut__DOT__tile_inst__DOT__dma_state = 1U;
                                                            __Vdly__test_copper__DOT__dut__DOT__tile_inst__DOT__dma_data_valid = 0U;
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
    vlSelfRef.test_copper__DOT__dut__DOT__tile_inst__DOT__dma_state 
        = __Vdly__test_copper__DOT__dut__DOT__tile_inst__DOT__dma_state;
    vlSelfRef.test_copper__DOT__dut__DOT__tile_inst__DOT__dma_data_valid 
        = __Vdly__test_copper__DOT__dut__DOT__tile_inst__DOT__dma_data_valid;
    if (vlSelfRef.test_copper__DOT__rst) {
        __Vdly__test_copper__DOT__dut__DOT__copper_index = 0U;
    } else {
        if (((0U == (IData)(vlSelfRef.test_copper__DOT__dut__DOT__h_count)) 
             & (0x01e0U == (IData)(vlSelfRef.test_copper__DOT__dut__DOT__v_count)))) {
            __Vdly__test_copper__DOT__dut__DOT__copper_index = 0U;
        }
        if (((((IData)(vlSelfRef.test_copper__DOT__dut__DOT__copper_enabled) 
               & (IData)(vlSelfRef.test_copper__DOT__dut__DOT__in_text_area)) 
              & ((IData)(vlSelfRef.test_copper__DOT__dut__DOT__copper_index) 
                 < (IData)(vlSelfRef.test_copper__DOT__dut__DOT__copper_count))) 
             & ((0x0001ffffU & (((IData)(0x00000140U) 
                                 * (IData)(vlSelfRef.test_copper__DOT__dut__DOT__gfx_y)) 
                                + (0x000001ffU & ((IData)(vlSelfRef.test_copper__DOT__dut__DOT__h_count) 
                                                  >> 1U)))) 
                >= vlSelfRef.test_copper__DOT__dut__DOT__copper_pos
                [(0x0000001fU & (IData)(vlSelfRef.test_copper__DOT__dut__DOT__copper_index))]))) {
            if ((1U == vlSelfRef.test_copper__DOT__dut__DOT__copper_reg
                 [(0x0000001fU & (IData)(vlSelfRef.test_copper__DOT__dut__DOT__copper_index))])) {
                vlSelfRef.test_copper__DOT__dut__DOT__bg_color 
                    = (0x0000000fU & vlSelfRef.test_copper__DOT__dut__DOT__copper_val
                       [(0x0000001fU & (IData)(vlSelfRef.test_copper__DOT__dut__DOT__copper_index))]);
            } else if ((2U == vlSelfRef.test_copper__DOT__dut__DOT__copper_reg
                        [(0x0000001fU & (IData)(vlSelfRef.test_copper__DOT__dut__DOT__copper_index))])) {
                __Vdly__test_copper__DOT__dut__DOT__fg_color 
                    = (0x0000000fU & vlSelfRef.test_copper__DOT__dut__DOT__copper_val
                       [(0x0000001fU & (IData)(vlSelfRef.test_copper__DOT__dut__DOT__copper_index))]);
            } else if ((0x0dU != vlSelfRef.test_copper__DOT__dut__DOT__copper_reg
                        [(0x0000001fU & (IData)(vlSelfRef.test_copper__DOT__dut__DOT__copper_index))])) {
                __VdlyVal__test_copper__DOT__dut__DOT__regs__v38 
                    = vlSelfRef.test_copper__DOT__dut__DOT__copper_val
                    [(0x0000001fU & (IData)(vlSelfRef.test_copper__DOT__dut__DOT__copper_index))];
                __VdlyDim0__test_copper__DOT__dut__DOT__regs__v38 
                    = (0x0000001fU & vlSelfRef.test_copper__DOT__dut__DOT__copper_reg
                       [(0x0000001fU & (IData)(vlSelfRef.test_copper__DOT__dut__DOT__copper_index))]);
                __VdlySet__test_copper__DOT__dut__DOT__regs__v38 = 1U;
            }
            __Vdly__test_copper__DOT__dut__DOT__copper_index 
                = (0x000001ffU & ((IData)(1U) + (IData)(vlSelfRef.test_copper__DOT__dut__DOT__copper_index)));
        }
    }
    vlSelfRef.__VdlyCommitQueuetest_copper__DOT__dut__DOT__tile_inst__DOT__nametable.commit(vlSelfRef.test_copper__DOT__dut__DOT__tile_inst__DOT__nametable);
    vlSelfRef.test_copper__DOT__dut__DOT__tile_inst__DOT__dma_count 
        = __Vdly__test_copper__DOT__dut__DOT__tile_inst__DOT__dma_count;
    vlSelfRef.test_copper__DOT__dut__DOT__tile_inst__DOT__dma_col_idx 
        = __Vdly__test_copper__DOT__dut__DOT__tile_inst__DOT__dma_col_idx;
    vlSelfRef.test_copper__DOT__dut__DOT__tile_inst__DOT__dma_src 
        = __Vdly__test_copper__DOT__dut__DOT__tile_inst__DOT__dma_src;
    vlSelfRef.test_copper__DOT__dut__DOT__tile_inst__DOT__dma_dst 
        = __Vdly__test_copper__DOT__dut__DOT__tile_inst__DOT__dma_dst;
    vlSelfRef.test_copper__DOT__dut__DOT__tile_inst__DOT__dma_pal_r 
        = __Vdly__test_copper__DOT__dut__DOT__tile_inst__DOT__dma_pal_r;
    vlSelfRef.test_copper__DOT__dut__DOT__tile_inst__DOT__dma_pal_phase 
        = __Vdly__test_copper__DOT__dut__DOT__tile_inst__DOT__dma_pal_phase;
    vlSelfRef.test_copper__DOT__dut__DOT__tile_inst__DOT__dma_pal_g 
        = __Vdly__test_copper__DOT__dut__DOT__tile_inst__DOT__dma_pal_g;
    if (__VdlySet__test_copper__DOT__dut__DOT__tile_inst__DOT__tregs__v0) {
        vlSelfRef.test_copper__DOT__dut__DOT__tile_inst__DOT__tregs[__VdlyDim0__test_copper__DOT__dut__DOT__tile_inst__DOT__tregs__v0] 
            = __VdlyVal__test_copper__DOT__dut__DOT__tile_inst__DOT__tregs__v0;
    }
    if (__VdlySet__test_copper__DOT__dut__DOT__tile_inst__DOT__pal_ram__v0) {
        test_copper__DOT__dut__DOT__tile_inst__DOT__pal_ram[__VdlyDim0__test_copper__DOT__dut__DOT__tile_inst__DOT__pal_ram__v0] 
            = __VdlyVal__test_copper__DOT__dut__DOT__tile_inst__DOT__pal_ram__v0;
    }
    if (__VdlySet__test_copper__DOT__dut__DOT__tile_inst__DOT__pal_ram__v1) {
        test_copper__DOT__dut__DOT__tile_inst__DOT__pal_ram[__VdlyDim0__test_copper__DOT__dut__DOT__tile_inst__DOT__pal_ram__v1] 
            = __VdlyVal__test_copper__DOT__dut__DOT__tile_inst__DOT__pal_ram__v1;
    }
    if (__VdlySet__test_copper__DOT__dut__DOT__tile_inst__DOT__attr_table__v0) {
        vlSelfRef.test_copper__DOT__dut__DOT__tile_inst__DOT__attr_table[__VdlyDim0__test_copper__DOT__dut__DOT__tile_inst__DOT__attr_table__v0] 
            = __VdlyVal__test_copper__DOT__dut__DOT__tile_inst__DOT__attr_table__v0;
    }
    if (__VdlySet__test_copper__DOT__dut__DOT__tile_inst__DOT__attr_table__v1) {
        vlSelfRef.test_copper__DOT__dut__DOT__tile_inst__DOT__attr_table[__VdlyDim0__test_copper__DOT__dut__DOT__tile_inst__DOT__attr_table__v1] 
            = __VdlyVal__test_copper__DOT__dut__DOT__tile_inst__DOT__attr_table__v1;
    }
    if (__VdlySet__test_copper__DOT__dut__DOT__tile_inst__DOT__attr_table__v2) {
        vlSelfRef.test_copper__DOT__dut__DOT__tile_inst__DOT__attr_table[__VdlyDim0__test_copper__DOT__dut__DOT__tile_inst__DOT__attr_table__v2] 
            = __VdlyVal__test_copper__DOT__dut__DOT__tile_inst__DOT__attr_table__v2;
    }
    if (__VdlySet__test_copper__DOT__dut__DOT__tile_inst__DOT__attr_table__v3) {
        vlSelfRef.test_copper__DOT__dut__DOT__tile_inst__DOT__attr_table[__VdlyDim0__test_copper__DOT__dut__DOT__tile_inst__DOT__attr_table__v3] 
            = __VdlyVal__test_copper__DOT__dut__DOT__tile_inst__DOT__attr_table__v3;
    }
    if (__VdlySet__test_copper__DOT__dut__DOT__tile_inst__DOT__attr_table__v4) {
        vlSelfRef.test_copper__DOT__dut__DOT__tile_inst__DOT__attr_table[__VdlyDim0__test_copper__DOT__dut__DOT__tile_inst__DOT__attr_table__v4] 
            = __VdlyVal__test_copper__DOT__dut__DOT__tile_inst__DOT__attr_table__v4;
    }
    if (__VdlySet__test_copper__DOT__dut__DOT__tile_inst__DOT__col_buffer__v0) {
        vlSelfRef.test_copper__DOT__dut__DOT__tile_inst__DOT__col_buffer[0U] 
            = __VdlyVal__test_copper__DOT__dut__DOT__tile_inst__DOT__col_buffer__v0;
    }
    if (__VdlySet__test_copper__DOT__dut__DOT__tile_inst__DOT__col_buffer__v1) {
        vlSelfRef.test_copper__DOT__dut__DOT__tile_inst__DOT__col_buffer[1U] 
            = __VdlyVal__test_copper__DOT__dut__DOT__tile_inst__DOT__col_buffer__v1;
    }
    if (__VdlySet__test_copper__DOT__dut__DOT__tile_inst__DOT__col_buffer__v2) {
        vlSelfRef.test_copper__DOT__dut__DOT__tile_inst__DOT__col_buffer[2U] 
            = __VdlyVal__test_copper__DOT__dut__DOT__tile_inst__DOT__col_buffer__v2;
    }
    if (__VdlySet__test_copper__DOT__dut__DOT__tile_inst__DOT__col_buffer__v3) {
        vlSelfRef.test_copper__DOT__dut__DOT__tile_inst__DOT__col_buffer[3U] 
            = __VdlyVal__test_copper__DOT__dut__DOT__tile_inst__DOT__col_buffer__v3;
    }
    if (__VdlySet__test_copper__DOT__dut__DOT__tile_inst__DOT__col_buffer__v4) {
        vlSelfRef.test_copper__DOT__dut__DOT__tile_inst__DOT__col_buffer[4U] 
            = __VdlyVal__test_copper__DOT__dut__DOT__tile_inst__DOT__col_buffer__v4;
    }
    if (__VdlySet__test_copper__DOT__dut__DOT__tile_inst__DOT__col_buffer__v5) {
        vlSelfRef.test_copper__DOT__dut__DOT__tile_inst__DOT__col_buffer[5U] 
            = __VdlyVal__test_copper__DOT__dut__DOT__tile_inst__DOT__col_buffer__v5;
    }
    if (__VdlySet__test_copper__DOT__dut__DOT__tile_inst__DOT__col_buffer__v6) {
        vlSelfRef.test_copper__DOT__dut__DOT__tile_inst__DOT__col_buffer[6U] 
            = __VdlyVal__test_copper__DOT__dut__DOT__tile_inst__DOT__col_buffer__v6;
    }
    if (__VdlySet__test_copper__DOT__dut__DOT__tile_inst__DOT__col_buffer__v7) {
        vlSelfRef.test_copper__DOT__dut__DOT__tile_inst__DOT__col_buffer[7U] 
            = __VdlyVal__test_copper__DOT__dut__DOT__tile_inst__DOT__col_buffer__v7;
    }
    if (__VdlySet__test_copper__DOT__dut__DOT__tile_inst__DOT__col_buffer__v8) {
        vlSelfRef.test_copper__DOT__dut__DOT__tile_inst__DOT__col_buffer[8U] 
            = __VdlyVal__test_copper__DOT__dut__DOT__tile_inst__DOT__col_buffer__v8;
    }
    if (__VdlySet__test_copper__DOT__dut__DOT__tile_inst__DOT__col_buffer__v9) {
        vlSelfRef.test_copper__DOT__dut__DOT__tile_inst__DOT__col_buffer[9U] 
            = __VdlyVal__test_copper__DOT__dut__DOT__tile_inst__DOT__col_buffer__v9;
    }
    if (__VdlySet__test_copper__DOT__dut__DOT__tile_inst__DOT__col_buffer__v10) {
        vlSelfRef.test_copper__DOT__dut__DOT__tile_inst__DOT__col_buffer[10U] 
            = __VdlyVal__test_copper__DOT__dut__DOT__tile_inst__DOT__col_buffer__v10;
    }
    if (__VdlySet__test_copper__DOT__dut__DOT__tile_inst__DOT__col_buffer__v11) {
        vlSelfRef.test_copper__DOT__dut__DOT__tile_inst__DOT__col_buffer[11U] 
            = __VdlyVal__test_copper__DOT__dut__DOT__tile_inst__DOT__col_buffer__v11;
    }
    if (__VdlySet__test_copper__DOT__dut__DOT__tile_inst__DOT__col_buffer__v12) {
        vlSelfRef.test_copper__DOT__dut__DOT__tile_inst__DOT__col_buffer[12U] 
            = __VdlyVal__test_copper__DOT__dut__DOT__tile_inst__DOT__col_buffer__v12;
    }
    if (__VdlySet__test_copper__DOT__dut__DOT__tile_inst__DOT__col_buffer__v13) {
        vlSelfRef.test_copper__DOT__dut__DOT__tile_inst__DOT__col_buffer[13U] 
            = __VdlyVal__test_copper__DOT__dut__DOT__tile_inst__DOT__col_buffer__v13;
    }
    if (__VdlySet__test_copper__DOT__dut__DOT__tile_inst__DOT__col_buffer__v14) {
        vlSelfRef.test_copper__DOT__dut__DOT__tile_inst__DOT__col_buffer[14U] 
            = __VdlyVal__test_copper__DOT__dut__DOT__tile_inst__DOT__col_buffer__v14;
    }
    if (__VdlySet__test_copper__DOT__dut__DOT__tile_inst__DOT__col_buffer__v15) {
        vlSelfRef.test_copper__DOT__dut__DOT__tile_inst__DOT__col_buffer[15U] 
            = __VdlyVal__test_copper__DOT__dut__DOT__tile_inst__DOT__col_buffer__v15;
    }
    if (__VdlySet__test_copper__DOT__dut__DOT__tile_inst__DOT__col_buffer__v16) {
        vlSelfRef.test_copper__DOT__dut__DOT__tile_inst__DOT__col_buffer[16U] 
            = __VdlyVal__test_copper__DOT__dut__DOT__tile_inst__DOT__col_buffer__v16;
    }
    if (__VdlySet__test_copper__DOT__dut__DOT__tile_inst__DOT__col_buffer__v17) {
        vlSelfRef.test_copper__DOT__dut__DOT__tile_inst__DOT__col_buffer[17U] 
            = __VdlyVal__test_copper__DOT__dut__DOT__tile_inst__DOT__col_buffer__v17;
    }
    if (__VdlySet__test_copper__DOT__dut__DOT__tile_inst__DOT__col_buffer__v18) {
        vlSelfRef.test_copper__DOT__dut__DOT__tile_inst__DOT__col_buffer[18U] 
            = __VdlyVal__test_copper__DOT__dut__DOT__tile_inst__DOT__col_buffer__v18;
    }
    if (__VdlySet__test_copper__DOT__dut__DOT__tile_inst__DOT__col_buffer__v19) {
        vlSelfRef.test_copper__DOT__dut__DOT__tile_inst__DOT__col_buffer[19U] 
            = __VdlyVal__test_copper__DOT__dut__DOT__tile_inst__DOT__col_buffer__v19;
    }
    if (__VdlySet__test_copper__DOT__dut__DOT__tile_inst__DOT__col_buffer__v20) {
        vlSelfRef.test_copper__DOT__dut__DOT__tile_inst__DOT__col_buffer[20U] 
            = __VdlyVal__test_copper__DOT__dut__DOT__tile_inst__DOT__col_buffer__v20;
    }
    if (__VdlySet__test_copper__DOT__dut__DOT__tile_inst__DOT__col_buffer__v21) {
        vlSelfRef.test_copper__DOT__dut__DOT__tile_inst__DOT__col_buffer[21U] 
            = __VdlyVal__test_copper__DOT__dut__DOT__tile_inst__DOT__col_buffer__v21;
    }
    if (__VdlySet__test_copper__DOT__dut__DOT__tile_inst__DOT__col_buffer__v22) {
        vlSelfRef.test_copper__DOT__dut__DOT__tile_inst__DOT__col_buffer[22U] 
            = __VdlyVal__test_copper__DOT__dut__DOT__tile_inst__DOT__col_buffer__v22;
    }
    if (__VdlySet__test_copper__DOT__dut__DOT__tile_inst__DOT__col_buffer__v23) {
        vlSelfRef.test_copper__DOT__dut__DOT__tile_inst__DOT__col_buffer[23U] 
            = __VdlyVal__test_copper__DOT__dut__DOT__tile_inst__DOT__col_buffer__v23;
    }
    if (__VdlySet__test_copper__DOT__dut__DOT__tile_inst__DOT__col_buffer__v24) {
        vlSelfRef.test_copper__DOT__dut__DOT__tile_inst__DOT__col_buffer[24U] 
            = __VdlyVal__test_copper__DOT__dut__DOT__tile_inst__DOT__col_buffer__v24;
    }
    if (__VdlySet__test_copper__DOT__dut__DOT__tile_inst__DOT__col_buffer__v25) {
        vlSelfRef.test_copper__DOT__dut__DOT__tile_inst__DOT__col_buffer[__VdlyDim0__test_copper__DOT__dut__DOT__tile_inst__DOT__col_buffer__v25] 
            = __VdlyVal__test_copper__DOT__dut__DOT__tile_inst__DOT__col_buffer__v25;
    }
    if (__VdlySet__test_copper__DOT__dut__DOT__tile_inst__DOT__col_buffer__v26) {
        vlSelfRef.test_copper__DOT__dut__DOT__tile_inst__DOT__col_buffer[0U] 
            = __VdlyVal__test_copper__DOT__dut__DOT__tile_inst__DOT__col_buffer__v26;
    }
    if (__VdlySet__test_copper__DOT__dut__DOT__tile_inst__DOT__col_buffer__v27) {
        vlSelfRef.test_copper__DOT__dut__DOT__tile_inst__DOT__col_buffer[1U] 
            = __VdlyVal__test_copper__DOT__dut__DOT__tile_inst__DOT__col_buffer__v27;
    }
    if (__VdlySet__test_copper__DOT__dut__DOT__tile_inst__DOT__col_buffer__v28) {
        vlSelfRef.test_copper__DOT__dut__DOT__tile_inst__DOT__col_buffer[2U] 
            = __VdlyVal__test_copper__DOT__dut__DOT__tile_inst__DOT__col_buffer__v28;
    }
    if (__VdlySet__test_copper__DOT__dut__DOT__tile_inst__DOT__col_buffer__v29) {
        vlSelfRef.test_copper__DOT__dut__DOT__tile_inst__DOT__col_buffer[3U] 
            = __VdlyVal__test_copper__DOT__dut__DOT__tile_inst__DOT__col_buffer__v29;
    }
    if (__VdlySet__test_copper__DOT__dut__DOT__tile_inst__DOT__col_buffer__v30) {
        vlSelfRef.test_copper__DOT__dut__DOT__tile_inst__DOT__col_buffer[4U] 
            = __VdlyVal__test_copper__DOT__dut__DOT__tile_inst__DOT__col_buffer__v30;
    }
    if (__VdlySet__test_copper__DOT__dut__DOT__tile_inst__DOT__col_buffer__v31) {
        vlSelfRef.test_copper__DOT__dut__DOT__tile_inst__DOT__col_buffer[5U] 
            = __VdlyVal__test_copper__DOT__dut__DOT__tile_inst__DOT__col_buffer__v31;
    }
    if (__VdlySet__test_copper__DOT__dut__DOT__tile_inst__DOT__col_buffer__v32) {
        vlSelfRef.test_copper__DOT__dut__DOT__tile_inst__DOT__col_buffer[6U] 
            = __VdlyVal__test_copper__DOT__dut__DOT__tile_inst__DOT__col_buffer__v32;
    }
    if (__VdlySet__test_copper__DOT__dut__DOT__tile_inst__DOT__col_buffer__v33) {
        vlSelfRef.test_copper__DOT__dut__DOT__tile_inst__DOT__col_buffer[7U] 
            = __VdlyVal__test_copper__DOT__dut__DOT__tile_inst__DOT__col_buffer__v33;
    }
    if (__VdlySet__test_copper__DOT__dut__DOT__tile_inst__DOT__col_buffer__v34) {
        vlSelfRef.test_copper__DOT__dut__DOT__tile_inst__DOT__col_buffer[8U] 
            = __VdlyVal__test_copper__DOT__dut__DOT__tile_inst__DOT__col_buffer__v34;
    }
    if (__VdlySet__test_copper__DOT__dut__DOT__tile_inst__DOT__col_buffer__v35) {
        vlSelfRef.test_copper__DOT__dut__DOT__tile_inst__DOT__col_buffer[9U] 
            = __VdlyVal__test_copper__DOT__dut__DOT__tile_inst__DOT__col_buffer__v35;
    }
    if (__VdlySet__test_copper__DOT__dut__DOT__tile_inst__DOT__col_buffer__v36) {
        vlSelfRef.test_copper__DOT__dut__DOT__tile_inst__DOT__col_buffer[10U] 
            = __VdlyVal__test_copper__DOT__dut__DOT__tile_inst__DOT__col_buffer__v36;
    }
    if (__VdlySet__test_copper__DOT__dut__DOT__tile_inst__DOT__col_buffer__v37) {
        vlSelfRef.test_copper__DOT__dut__DOT__tile_inst__DOT__col_buffer[11U] 
            = __VdlyVal__test_copper__DOT__dut__DOT__tile_inst__DOT__col_buffer__v37;
    }
    if (__VdlySet__test_copper__DOT__dut__DOT__tile_inst__DOT__col_buffer__v38) {
        vlSelfRef.test_copper__DOT__dut__DOT__tile_inst__DOT__col_buffer[12U] 
            = __VdlyVal__test_copper__DOT__dut__DOT__tile_inst__DOT__col_buffer__v38;
    }
    if (__VdlySet__test_copper__DOT__dut__DOT__tile_inst__DOT__col_buffer__v39) {
        vlSelfRef.test_copper__DOT__dut__DOT__tile_inst__DOT__col_buffer[13U] 
            = __VdlyVal__test_copper__DOT__dut__DOT__tile_inst__DOT__col_buffer__v39;
    }
    if (__VdlySet__test_copper__DOT__dut__DOT__tile_inst__DOT__col_buffer__v40) {
        vlSelfRef.test_copper__DOT__dut__DOT__tile_inst__DOT__col_buffer[14U] 
            = __VdlyVal__test_copper__DOT__dut__DOT__tile_inst__DOT__col_buffer__v40;
    }
    if (__VdlySet__test_copper__DOT__dut__DOT__tile_inst__DOT__col_buffer__v41) {
        vlSelfRef.test_copper__DOT__dut__DOT__tile_inst__DOT__col_buffer[15U] 
            = __VdlyVal__test_copper__DOT__dut__DOT__tile_inst__DOT__col_buffer__v41;
    }
    if (__VdlySet__test_copper__DOT__dut__DOT__tile_inst__DOT__col_buffer__v42) {
        vlSelfRef.test_copper__DOT__dut__DOT__tile_inst__DOT__col_buffer[16U] 
            = __VdlyVal__test_copper__DOT__dut__DOT__tile_inst__DOT__col_buffer__v42;
    }
    if (__VdlySet__test_copper__DOT__dut__DOT__tile_inst__DOT__col_buffer__v43) {
        vlSelfRef.test_copper__DOT__dut__DOT__tile_inst__DOT__col_buffer[17U] 
            = __VdlyVal__test_copper__DOT__dut__DOT__tile_inst__DOT__col_buffer__v43;
    }
    if (__VdlySet__test_copper__DOT__dut__DOT__tile_inst__DOT__col_buffer__v44) {
        vlSelfRef.test_copper__DOT__dut__DOT__tile_inst__DOT__col_buffer[18U] 
            = __VdlyVal__test_copper__DOT__dut__DOT__tile_inst__DOT__col_buffer__v44;
    }
    if (__VdlySet__test_copper__DOT__dut__DOT__tile_inst__DOT__col_buffer__v45) {
        vlSelfRef.test_copper__DOT__dut__DOT__tile_inst__DOT__col_buffer[19U] 
            = __VdlyVal__test_copper__DOT__dut__DOT__tile_inst__DOT__col_buffer__v45;
    }
    if (__VdlySet__test_copper__DOT__dut__DOT__tile_inst__DOT__col_buffer__v46) {
        vlSelfRef.test_copper__DOT__dut__DOT__tile_inst__DOT__col_buffer[20U] 
            = __VdlyVal__test_copper__DOT__dut__DOT__tile_inst__DOT__col_buffer__v46;
    }
    if (__VdlySet__test_copper__DOT__dut__DOT__tile_inst__DOT__col_buffer__v47) {
        vlSelfRef.test_copper__DOT__dut__DOT__tile_inst__DOT__col_buffer[21U] 
            = __VdlyVal__test_copper__DOT__dut__DOT__tile_inst__DOT__col_buffer__v47;
    }
    if (__VdlySet__test_copper__DOT__dut__DOT__tile_inst__DOT__col_buffer__v48) {
        vlSelfRef.test_copper__DOT__dut__DOT__tile_inst__DOT__col_buffer[22U] 
            = __VdlyVal__test_copper__DOT__dut__DOT__tile_inst__DOT__col_buffer__v48;
    }
    if (__VdlySet__test_copper__DOT__dut__DOT__tile_inst__DOT__col_buffer__v49) {
        vlSelfRef.test_copper__DOT__dut__DOT__tile_inst__DOT__col_buffer[23U] 
            = __VdlyVal__test_copper__DOT__dut__DOT__tile_inst__DOT__col_buffer__v49;
    }
    if (__VdlySet__test_copper__DOT__dut__DOT__tile_inst__DOT__col_buffer__v50) {
        vlSelfRef.test_copper__DOT__dut__DOT__tile_inst__DOT__col_buffer[24U] 
            = __VdlyVal__test_copper__DOT__dut__DOT__tile_inst__DOT__col_buffer__v50;
    }
    if (vlSelfRef.test_copper__DOT__rst) {
        vlSelfRef.test_copper__DOT__dut__DOT__tile_inst__DOT__tile_size16 = 0U;
    } else if (((IData)(vlSelfRef.test_copper__DOT__cpu_we) 
                & (IData)(vlSelfRef.test_copper__DOT__dut__DOT__tile_reg_sel))) {
        if ((0U == (0x0000001fU & (IData)(vlSelfRef.test_copper__DOT__cpu_addr)))) {
            vlSelfRef.test_copper__DOT__dut__DOT__tile_inst__DOT__tile_size16 
                = (1U & (IData)(vlSelfRef.test_copper__DOT__cpu_wdata));
        }
    }
    if (vlSelfRef.test_copper__DOT__rst) {
        vlSelfRef.test_copper__DOT__dut__DOT__unnamedblk14__DOT__i = 0U;
        vlSelfRef.test_copper__DOT__dut__DOT__unnamedblk15__DOT__i = 0U;
        vlSelfRef.test_copper__DOT__dut__DOT__unnamedblk16__DOT__i = 0U;
        __Vdly__test_copper__DOT__dut__DOT__cursor_x = 0U;
        __Vdly__test_copper__DOT__dut__DOT__cursor_y = 0U;
        __Vdly__test_copper__DOT__dut__DOT__fg_color = 1U;
        vlSelfRef.test_copper__DOT__dut__DOT__bg_color = 6U;
        __Vdly__test_copper__DOT__dut__DOT__gfx_color = 1U;
        __Vdly__test_copper__DOT__dut__DOT__scroll_offset = 0U;
        __Vdly__test_copper__DOT__dut__DOT__scroll_pending = 0U;
        __Vdly__test_copper__DOT__dut__DOT__scroll_col = 0U;
        __Vdly__test_copper__DOT__dut__DOT__cmd_busy = 0U;
        vlSelfRef.test_copper__DOT__dut__DOT__copper_enabled = 0U;
        __Vdly__test_copper__DOT__dut__DOT__copper_count = 0U;
        __Vdly__test_copper__DOT__dut__DOT__copper_index = 0U;
        __Vdly__test_copper__DOT__dut__DOT__copper_target_list = 0U;
        __Vdly__test_copper__DOT__dut__DOT__copper_pending_list = 0U;
        __Vdly__test_copper__DOT__dut__DOT__copper_loading = 0U;
        __Vdly__test_copper__DOT__dut__DOT__fill_sp = 0U;
        __VdlySet__test_copper__DOT__dut__DOT__regs__v0 = 1U;
        while (VL_GTS_III(32, 0x000007d0U, vlSelfRef.test_copper__DOT__dut__DOT__unnamedblk14__DOT__i)) {
            vlSelfRef.test_copper__DOT__dut__DOT____Vlvbound_h9d4d7d6f__0 = 0x20U;
            if (VL_LIKELY(((0x07cfU >= (0x000007ffU 
                                        & vlSelfRef.test_copper__DOT__dut__DOT__unnamedblk14__DOT__i))))) {
                __VdlyVal__test_copper__DOT__dut__DOT__char_ram__v0 
                    = vlSelfRef.test_copper__DOT__dut__DOT____Vlvbound_h9d4d7d6f__0;
                __VdlyDim0__test_copper__DOT__dut__DOT__char_ram__v0 
                    = (0x000007ffU & vlSelfRef.test_copper__DOT__dut__DOT__unnamedblk14__DOT__i);
                vlSelfRef.__VdlyCommitQueuetest_copper__DOT__dut__DOT__char_ram.enqueue(__VdlyVal__test_copper__DOT__dut__DOT__char_ram__v0, (IData)(__VdlyDim0__test_copper__DOT__dut__DOT__char_ram__v0));
            }
            vlSelfRef.test_copper__DOT__dut__DOT____Vlvbound_hef6c2195__0 = 1U;
            if (VL_LIKELY(((0x07cfU >= (0x000007ffU 
                                        & vlSelfRef.test_copper__DOT__dut__DOT__unnamedblk14__DOT__i))))) {
                __VdlyVal__test_copper__DOT__dut__DOT__color_ram__v0 
                    = vlSelfRef.test_copper__DOT__dut__DOT____Vlvbound_hef6c2195__0;
                __VdlyDim0__test_copper__DOT__dut__DOT__color_ram__v0 
                    = (0x000007ffU & vlSelfRef.test_copper__DOT__dut__DOT__unnamedblk14__DOT__i);
                vlSelfRef.__VdlyCommitQueuetest_copper__DOT__dut__DOT__color_ram.enqueue(__VdlyVal__test_copper__DOT__dut__DOT__color_ram__v0, (IData)(__VdlyDim0__test_copper__DOT__dut__DOT__color_ram__v0));
            }
            vlSelfRef.test_copper__DOT__dut__DOT__unnamedblk14__DOT__i 
                = ((IData)(1U) + vlSelfRef.test_copper__DOT__dut__DOT__unnamedblk14__DOT__i);
        }
        while (VL_GTS_III(32, 0x0000fa00U, vlSelfRef.test_copper__DOT__dut__DOT__unnamedblk15__DOT__i)) {
            vlSelfRef.test_copper__DOT__dut__DOT____Vlvbound_h9d27345d__0 = 0U;
            if (VL_LIKELY(((0xf9ffU >= (0x0000ffffU 
                                        & vlSelfRef.test_copper__DOT__dut__DOT__unnamedblk15__DOT__i))))) {
                __VdlyVal__test_copper__DOT__dut__DOT__gfx_ram__v0 
                    = vlSelfRef.test_copper__DOT__dut__DOT____Vlvbound_h9d27345d__0;
                __VdlyDim0__test_copper__DOT__dut__DOT__gfx_ram__v0 
                    = (0x0000ffffU & vlSelfRef.test_copper__DOT__dut__DOT__unnamedblk15__DOT__i);
                vlSelfRef.__VdlyCommitQueuetest_copper__DOT__dut__DOT__gfx_ram.enqueue(__VdlyVal__test_copper__DOT__dut__DOT__gfx_ram__v0, (IData)(__VdlyDim0__test_copper__DOT__dut__DOT__gfx_ram__v0));
            }
            vlSelfRef.test_copper__DOT__dut__DOT__unnamedblk15__DOT__i 
                = ((IData)(1U) + vlSelfRef.test_copper__DOT__dut__DOT__unnamedblk15__DOT__i);
        }
        while (VL_GTS_III(32, 0x00000800U, vlSelfRef.test_copper__DOT__dut__DOT__unnamedblk16__DOT__i)) {
            __VdlyDim0__test_copper__DOT__dut__DOT__sprite_shapes__v0 
                = (0x000007ffU & vlSelfRef.test_copper__DOT__dut__DOT__unnamedblk16__DOT__i);
            vlSelfRef.__VdlyCommitQueuetest_copper__DOT__dut__DOT__sprite_shapes.enqueue(0U, (IData)(__VdlyDim0__test_copper__DOT__dut__DOT__sprite_shapes__v0));
            vlSelfRef.test_copper__DOT__dut__DOT__unnamedblk16__DOT__i 
                = ((IData)(1U) + vlSelfRef.test_copper__DOT__dut__DOT__unnamedblk16__DOT__i);
        }
        __VdlySet__test_copper__DOT__dut__DOT__regs__v1 = 1U;
    } else {
        if (vlSelfRef.test_copper__DOT__dut__DOT__scroll_pending) {
            vlSelfRef.test_copper__DOT__dut__DOT____Vlvbound_h902d5c6a__0 = 0x20U;
            if (VL_LIKELY(((0x07cfU >= ([&]() {
                                    __Vfunc_test_copper__DOT__dut__DOT__screen_addr__110__col 
                                        = vlSelfRef.test_copper__DOT__dut__DOT__scroll_col;
                                    __Vfunc_test_copper__DOT__dut__DOT__screen_addr__110__rr 
                                        = (0x0000001fU 
                                           & ((IData)(0x18U) 
                                              + (IData)(vlSelfRef.test_copper__DOT__dut__DOT__scroll_offset)));
                                    if ((0x19U <= (IData)(__Vfunc_test_copper__DOT__dut__DOT__screen_addr__110__rr))) {
                                        __Vfunc_test_copper__DOT__dut__DOT__screen_addr__110__rr 
                                            = (0x0000001fU 
                                               & ((IData)(__Vfunc_test_copper__DOT__dut__DOT__screen_addr__110__rr) 
                                                  - (IData)(0x19U)));
                                    }
                                    __Vfunc_test_copper__DOT__dut__DOT__screen_addr__110__Vfuncout 
                                        = (0x000007ffU 
                                           & (((IData)(0x00000050U) 
                                               * (IData)(__Vfunc_test_copper__DOT__dut__DOT__screen_addr__110__rr)) 
                                              + (IData)(__Vfunc_test_copper__DOT__dut__DOT__screen_addr__110__col)));
                                }(), (IData)(__Vfunc_test_copper__DOT__dut__DOT__screen_addr__110__Vfuncout)))))) {
                __Vfunc_test_copper__DOT__dut__DOT__screen_addr__111__col 
                    = vlSelfRef.test_copper__DOT__dut__DOT__scroll_col;
                __Vfunc_test_copper__DOT__dut__DOT__screen_addr__111__rr 
                    = (0x0000001fU & ((IData)(0x18U) 
                                      + (IData)(vlSelfRef.test_copper__DOT__dut__DOT__scroll_offset)));
                if ((0x19U <= (IData)(__Vfunc_test_copper__DOT__dut__DOT__screen_addr__111__rr))) {
                    __Vfunc_test_copper__DOT__dut__DOT__screen_addr__111__rr 
                        = (0x0000001fU & ((IData)(__Vfunc_test_copper__DOT__dut__DOT__screen_addr__111__rr) 
                                          - (IData)(0x19U)));
                }
                __Vfunc_test_copper__DOT__dut__DOT__screen_addr__111__Vfuncout 
                    = (0x000007ffU & (((IData)(0x00000050U) 
                                       * (IData)(__Vfunc_test_copper__DOT__dut__DOT__screen_addr__111__rr)) 
                                      + (IData)(__Vfunc_test_copper__DOT__dut__DOT__screen_addr__111__col)));
                __VdlyVal__test_copper__DOT__dut__DOT__char_ram__v1 
                    = vlSelfRef.test_copper__DOT__dut__DOT____Vlvbound_h902d5c6a__0;
                __VdlyDim0__test_copper__DOT__dut__DOT__char_ram__v1 
                    = __Vfunc_test_copper__DOT__dut__DOT__screen_addr__111__Vfuncout;
                vlSelfRef.__VdlyCommitQueuetest_copper__DOT__dut__DOT__char_ram.enqueue(__VdlyVal__test_copper__DOT__dut__DOT__char_ram__v1, (IData)(__VdlyDim0__test_copper__DOT__dut__DOT__char_ram__v1));
            }
            vlSelfRef.test_copper__DOT__dut__DOT____Vlvbound_hdecc4094__0 
                = vlSelfRef.test_copper__DOT__dut__DOT__fg_color;
            if (VL_LIKELY(((0x07cfU >= ([&]() {
                                    __Vfunc_test_copper__DOT__dut__DOT__screen_addr__112__col 
                                        = vlSelfRef.test_copper__DOT__dut__DOT__scroll_col;
                                    __Vfunc_test_copper__DOT__dut__DOT__screen_addr__112__rr 
                                        = (0x0000001fU 
                                           & ((IData)(0x18U) 
                                              + (IData)(vlSelfRef.test_copper__DOT__dut__DOT__scroll_offset)));
                                    if ((0x19U <= (IData)(__Vfunc_test_copper__DOT__dut__DOT__screen_addr__112__rr))) {
                                        __Vfunc_test_copper__DOT__dut__DOT__screen_addr__112__rr 
                                            = (0x0000001fU 
                                               & ((IData)(__Vfunc_test_copper__DOT__dut__DOT__screen_addr__112__rr) 
                                                  - (IData)(0x19U)));
                                    }
                                    __Vfunc_test_copper__DOT__dut__DOT__screen_addr__112__Vfuncout 
                                        = (0x000007ffU 
                                           & (((IData)(0x00000050U) 
                                               * (IData)(__Vfunc_test_copper__DOT__dut__DOT__screen_addr__112__rr)) 
                                              + (IData)(__Vfunc_test_copper__DOT__dut__DOT__screen_addr__112__col)));
                                }(), (IData)(__Vfunc_test_copper__DOT__dut__DOT__screen_addr__112__Vfuncout)))))) {
                __Vfunc_test_copper__DOT__dut__DOT__screen_addr__113__col 
                    = vlSelfRef.test_copper__DOT__dut__DOT__scroll_col;
                __Vfunc_test_copper__DOT__dut__DOT__screen_addr__113__rr 
                    = (0x0000001fU & ((IData)(0x18U) 
                                      + (IData)(vlSelfRef.test_copper__DOT__dut__DOT__scroll_offset)));
                if ((0x19U <= (IData)(__Vfunc_test_copper__DOT__dut__DOT__screen_addr__113__rr))) {
                    __Vfunc_test_copper__DOT__dut__DOT__screen_addr__113__rr 
                        = (0x0000001fU & ((IData)(__Vfunc_test_copper__DOT__dut__DOT__screen_addr__113__rr) 
                                          - (IData)(0x19U)));
                }
                __Vfunc_test_copper__DOT__dut__DOT__screen_addr__113__Vfuncout 
                    = (0x000007ffU & (((IData)(0x00000050U) 
                                       * (IData)(__Vfunc_test_copper__DOT__dut__DOT__screen_addr__113__rr)) 
                                      + (IData)(__Vfunc_test_copper__DOT__dut__DOT__screen_addr__113__col)));
                __VdlyVal__test_copper__DOT__dut__DOT__color_ram__v1 
                    = vlSelfRef.test_copper__DOT__dut__DOT____Vlvbound_hdecc4094__0;
                __VdlyDim0__test_copper__DOT__dut__DOT__color_ram__v1 
                    = __Vfunc_test_copper__DOT__dut__DOT__screen_addr__113__Vfuncout;
                vlSelfRef.__VdlyCommitQueuetest_copper__DOT__dut__DOT__color_ram.enqueue(__VdlyVal__test_copper__DOT__dut__DOT__color_ram__v1, (IData)(__VdlyDim0__test_copper__DOT__dut__DOT__color_ram__v1));
            }
            if ((0x4fU == (IData)(vlSelfRef.test_copper__DOT__dut__DOT__scroll_col))) {
                __Vdly__test_copper__DOT__dut__DOT__scroll_pending = 0U;
                __Vdly__test_copper__DOT__dut__DOT__scroll_col = 0U;
            } else {
                __Vdly__test_copper__DOT__dut__DOT__scroll_col 
                    = (0x0000007fU & ((IData)(1U) + (IData)(vlSelfRef.test_copper__DOT__dut__DOT__scroll_col)));
            }
        }
        if (vlSelfRef.test_copper__DOT__dut__DOT__cmd_busy) {
            if ((0x00000080U & (IData)(vlSelfRef.test_copper__DOT__dut__DOT__cmd_op))) {
                if ((0x00000040U & (IData)(vlSelfRef.test_copper__DOT__dut__DOT__cmd_op))) {
                    if ((0x00000020U & (IData)(vlSelfRef.test_copper__DOT__dut__DOT__cmd_op))) {
                        if ((0x00000010U & (IData)(vlSelfRef.test_copper__DOT__dut__DOT__cmd_op))) {
                            if ((8U & (IData)(vlSelfRef.test_copper__DOT__dut__DOT__cmd_op))) {
                                __Vdly__test_copper__DOT__dut__DOT__cmd_busy = 0U;
                            } else if ((4U & (IData)(vlSelfRef.test_copper__DOT__dut__DOT__cmd_op))) {
                                __Vdly__test_copper__DOT__dut__DOT__cmd_busy = 0U;
                            } else if ((2U & (IData)(vlSelfRef.test_copper__DOT__dut__DOT__cmd_op))) {
                                __Vdly__test_copper__DOT__dut__DOT__cmd_busy = 0U;
                            } else if ((1U & (IData)(vlSelfRef.test_copper__DOT__dut__DOT__cmd_op))) {
                                __Vdly__test_copper__DOT__dut__DOT__cmd_busy = 0U;
                            } else {
                                vlSelfRef.test_copper__DOT__dut__DOT____Vlvbound_h6e0ff81d__0 = 0x20U;
                                if (VL_LIKELY(((0x07cfU 
                                                >= 
                                                (0x000007ffU 
                                                 & (((IData)(0x00000050U) 
                                                     * 
                                                     (0x0000001fU 
                                                      & (IData)(vlSelfRef.test_copper__DOT__dut__DOT__cmd_cy))) 
                                                    + 
                                                    (0x0000007fU 
                                                     & (IData)(vlSelfRef.test_copper__DOT__dut__DOT__cmd_cx)))))))) {
                                    __VdlyVal__test_copper__DOT__dut__DOT__char_ram__v2 
                                        = vlSelfRef.test_copper__DOT__dut__DOT____Vlvbound_h6e0ff81d__0;
                                    __VdlyDim0__test_copper__DOT__dut__DOT__char_ram__v2 
                                        = (0x000007ffU 
                                           & (((IData)(0x00000050U) 
                                               * (0x0000001fU 
                                                  & (IData)(vlSelfRef.test_copper__DOT__dut__DOT__cmd_cy))) 
                                              + (0x0000007fU 
                                                 & (IData)(vlSelfRef.test_copper__DOT__dut__DOT__cmd_cx))));
                                    vlSelfRef.__VdlyCommitQueuetest_copper__DOT__dut__DOT__char_ram.enqueue(__VdlyVal__test_copper__DOT__dut__DOT__char_ram__v2, (IData)(__VdlyDim0__test_copper__DOT__dut__DOT__char_ram__v2));
                                }
                                vlSelfRef.test_copper__DOT__dut__DOT____Vlvbound_h1cc62cc7__0 
                                    = vlSelfRef.test_copper__DOT__dut__DOT__fg_color;
                                if (VL_LIKELY(((0x07cfU 
                                                >= 
                                                (0x000007ffU 
                                                 & (((IData)(0x00000050U) 
                                                     * 
                                                     (0x0000001fU 
                                                      & (IData)(vlSelfRef.test_copper__DOT__dut__DOT__cmd_cy))) 
                                                    + 
                                                    (0x0000007fU 
                                                     & (IData)(vlSelfRef.test_copper__DOT__dut__DOT__cmd_cx)))))))) {
                                    __VdlyVal__test_copper__DOT__dut__DOT__color_ram__v2 
                                        = vlSelfRef.test_copper__DOT__dut__DOT____Vlvbound_h1cc62cc7__0;
                                    __VdlyDim0__test_copper__DOT__dut__DOT__color_ram__v2 
                                        = (0x000007ffU 
                                           & (((IData)(0x00000050U) 
                                               * (0x0000001fU 
                                                  & (IData)(vlSelfRef.test_copper__DOT__dut__DOT__cmd_cy))) 
                                              + (0x0000007fU 
                                                 & (IData)(vlSelfRef.test_copper__DOT__dut__DOT__cmd_cx))));
                                    vlSelfRef.__VdlyCommitQueuetest_copper__DOT__dut__DOT__color_ram.enqueue(__VdlyVal__test_copper__DOT__dut__DOT__color_ram__v2, (IData)(__VdlyDim0__test_copper__DOT__dut__DOT__color_ram__v2));
                                }
                                if ((0x004fU == (IData)(vlSelfRef.test_copper__DOT__dut__DOT__cmd_cx))) {
                                    __Vdly__test_copper__DOT__dut__DOT__cmd_cx = 0U;
                                    if ((0x18U == (IData)(vlSelfRef.test_copper__DOT__dut__DOT__cmd_cy))) {
                                        __Vdly__test_copper__DOT__dut__DOT__cmd_busy = 0U;
                                    } else {
                                        __Vdly__test_copper__DOT__dut__DOT__cmd_cy 
                                            = (0x000000ffU 
                                               & ((IData)(1U) 
                                                  + (IData)(vlSelfRef.test_copper__DOT__dut__DOT__cmd_cy)));
                                    }
                                } else {
                                    __Vdly__test_copper__DOT__dut__DOT__cmd_cx 
                                        = (0x000001ffU 
                                           & ((IData)(1U) 
                                              + (IData)(vlSelfRef.test_copper__DOT__dut__DOT__cmd_cx)));
                                }
                            }
                        } else {
                            __Vdly__test_copper__DOT__dut__DOT__cmd_busy = 0U;
                        }
                    } else {
                        __Vdly__test_copper__DOT__dut__DOT__cmd_busy = 0U;
                    }
                } else {
                    __Vdly__test_copper__DOT__dut__DOT__cmd_busy = 0U;
                }
            } else if ((0x00000040U & (IData)(vlSelfRef.test_copper__DOT__dut__DOT__cmd_op))) {
                __Vdly__test_copper__DOT__dut__DOT__cmd_busy = 0U;
            } else if ((0x00000020U & (IData)(vlSelfRef.test_copper__DOT__dut__DOT__cmd_op))) {
                __Vdly__test_copper__DOT__dut__DOT__cmd_busy = 0U;
            } else if ((0x00000010U & (IData)(vlSelfRef.test_copper__DOT__dut__DOT__cmd_op))) {
                if ((8U & (IData)(vlSelfRef.test_copper__DOT__dut__DOT__cmd_op))) {
                    __Vdly__test_copper__DOT__dut__DOT__cmd_busy = 0U;
                } else if ((4U & (IData)(vlSelfRef.test_copper__DOT__dut__DOT__cmd_op))) {
                    __Vdly__test_copper__DOT__dut__DOT__cmd_busy = 0U;
                } else if ((2U & (IData)(vlSelfRef.test_copper__DOT__dut__DOT__cmd_op))) {
                    if ((1U & (IData)(vlSelfRef.test_copper__DOT__dut__DOT__cmd_op))) {
                        __VdlyVal__test_copper__DOT__dut__DOT__sprite_shapes__v1 
                            = vlSelfRef.test_copper__DOT__dut__DOT__sprite_shapes
                            [((0x00000780U & ((IData)(vlSelfRef.test_copper__DOT__dut__DOT__cmd_y) 
                                              << 7U)) 
                              | ((0x00000078U & ((IData)(vlSelfRef.test_copper__DOT__dut__DOT__cmd_cy) 
                                                 << 3U)) 
                                 | (7U & (IData)(vlSelfRef.test_copper__DOT__dut__DOT__cmd_cx))))];
                        __VdlyDim0__test_copper__DOT__dut__DOT__sprite_shapes__v1 
                            = ((0x00000780U & ((IData)(vlSelfRef.test_copper__DOT__dut__DOT__cmd_x) 
                                               << 7U)) 
                               | ((0x00000078U & ((IData)(vlSelfRef.test_copper__DOT__dut__DOT__cmd_cy) 
                                                  << 3U)) 
                                  | (7U & (IData)(vlSelfRef.test_copper__DOT__dut__DOT__cmd_cx))));
                        vlSelfRef.__VdlyCommitQueuetest_copper__DOT__dut__DOT__sprite_shapes.enqueue(__VdlyVal__test_copper__DOT__dut__DOT__sprite_shapes__v1, (IData)(__VdlyDim0__test_copper__DOT__dut__DOT__sprite_shapes__v1));
                        if ((7U == (IData)(vlSelfRef.test_copper__DOT__dut__DOT__cmd_cx))) {
                            __Vdly__test_copper__DOT__dut__DOT__cmd_cx = 0U;
                            if ((0x0fU == (IData)(vlSelfRef.test_copper__DOT__dut__DOT__cmd_cy))) {
                                __Vdly__test_copper__DOT__dut__DOT__cmd_busy = 0U;
                            } else {
                                __Vdly__test_copper__DOT__dut__DOT__cmd_cy 
                                    = (0x000000ffU 
                                       & ((IData)(1U) 
                                          + (IData)(vlSelfRef.test_copper__DOT__dut__DOT__cmd_cy)));
                            }
                        } else {
                            __Vdly__test_copper__DOT__dut__DOT__cmd_cx 
                                = (0x000001ffU & ((IData)(1U) 
                                                  + (IData)(vlSelfRef.test_copper__DOT__dut__DOT__cmd_cx)));
                        }
                    } else {
                        __VdlyDim0__test_copper__DOT__dut__DOT__sprite_shapes__v2 
                            = ((0x00000780U & ((IData)(vlSelfRef.test_copper__DOT__dut__DOT__cmd_x) 
                                               << 6U)) 
                               | ((0x00000078U & ((IData)(vlSelfRef.test_copper__DOT__dut__DOT__cmd_cy) 
                                                  << 3U)) 
                                  | (7U & (IData)(vlSelfRef.test_copper__DOT__dut__DOT__cmd_cx))));
                        vlSelfRef.__VdlyCommitQueuetest_copper__DOT__dut__DOT__sprite_shapes.enqueue(0U, (IData)(__VdlyDim0__test_copper__DOT__dut__DOT__sprite_shapes__v2));
                        if ((7U == (IData)(vlSelfRef.test_copper__DOT__dut__DOT__cmd_cx))) {
                            __Vdly__test_copper__DOT__dut__DOT__cmd_cx = 0U;
                            if ((0x0fU == (IData)(vlSelfRef.test_copper__DOT__dut__DOT__cmd_cy))) {
                                __Vdly__test_copper__DOT__dut__DOT__cmd_busy = 0U;
                            } else {
                                __Vdly__test_copper__DOT__dut__DOT__cmd_cy 
                                    = (0x000000ffU 
                                       & ((IData)(1U) 
                                          + (IData)(vlSelfRef.test_copper__DOT__dut__DOT__cmd_cy)));
                            }
                        } else {
                            __Vdly__test_copper__DOT__dut__DOT__cmd_cx 
                                = (0x000001ffU & ((IData)(1U) 
                                                  + (IData)(vlSelfRef.test_copper__DOT__dut__DOT__cmd_cx)));
                        }
                    }
                } else {
                    __Vdly__test_copper__DOT__dut__DOT__cmd_busy = 0U;
                }
            } else if ((8U & (IData)(vlSelfRef.test_copper__DOT__dut__DOT__cmd_op))) {
                if ((4U & (IData)(vlSelfRef.test_copper__DOT__dut__DOT__cmd_op))) {
                    __Vdly__test_copper__DOT__dut__DOT__cmd_busy = 0U;
                } else if ((2U & (IData)(vlSelfRef.test_copper__DOT__dut__DOT__cmd_op))) {
                    if ((1U & (IData)(vlSelfRef.test_copper__DOT__dut__DOT__cmd_op))) {
                        __Vdly__test_copper__DOT__dut__DOT__cmd_busy = 0U;
                    } else if (((IData)(vlSelfRef.test_copper__DOT__dut__DOT__gt_char_idx) 
                                < (IData)(vlSelfRef.test_copper__DOT__dut__DOT__gt_char_len))) {
                        if ((1U & ((IData)(vlSelfRef.test_copper__DOT__dut__DOT__gt_font_byte) 
                                   >> (7U & ((IData)(7U) 
                                             - (IData)(vlSelfRef.test_copper__DOT__dut__DOT__gt_font_col)))))) {
                            if (((0x00000140U > (((IData)(vlSelfRef.test_copper__DOT__dut__DOT__gt_pen_x) 
                                                  + 
                                                  ((IData)(vlSelfRef.test_copper__DOT__dut__DOT__gt_font_col) 
                                                   * (IData)(vlSelfRef.test_copper__DOT__dut__DOT__gt_scale))) 
                                                 + (IData)(vlSelfRef.test_copper__DOT__dut__DOT__gt_scale_x))) 
                                 & (0x000000c8U > (
                                                   ((IData)(vlSelfRef.test_copper__DOT__dut__DOT__gt_start_y) 
                                                    + 
                                                    ((IData)(vlSelfRef.test_copper__DOT__dut__DOT__gt_font_row) 
                                                     * (IData)(vlSelfRef.test_copper__DOT__dut__DOT__gt_scale))) 
                                                   + (IData)(vlSelfRef.test_copper__DOT__dut__DOT__gt_scale_y))))) {
                                vlSelfRef.test_copper__DOT__dut__DOT____Vlvbound_he1a62a70__0 
                                    = vlSelfRef.test_copper__DOT__dut__DOT__gfx_color;
                                if (VL_LIKELY(((0xf9ffU 
                                                >= 
                                                (0x0000ffffU 
                                                 & (((IData)(0x00000140U) 
                                                     * 
                                                     (((IData)(vlSelfRef.test_copper__DOT__dut__DOT__gt_start_y) 
                                                       + 
                                                       ((IData)(vlSelfRef.test_copper__DOT__dut__DOT__gt_font_row) 
                                                        * (IData)(vlSelfRef.test_copper__DOT__dut__DOT__gt_scale))) 
                                                      + (IData)(vlSelfRef.test_copper__DOT__dut__DOT__gt_scale_y))) 
                                                    + 
                                                    (((IData)(vlSelfRef.test_copper__DOT__dut__DOT__gt_pen_x) 
                                                      + 
                                                      ((IData)(vlSelfRef.test_copper__DOT__dut__DOT__gt_font_col) 
                                                       * (IData)(vlSelfRef.test_copper__DOT__dut__DOT__gt_scale))) 
                                                     + (IData)(vlSelfRef.test_copper__DOT__dut__DOT__gt_scale_x)))))))) {
                                    __VdlyVal__test_copper__DOT__dut__DOT__gfx_ram__v1 
                                        = vlSelfRef.test_copper__DOT__dut__DOT____Vlvbound_he1a62a70__0;
                                    __VdlyDim0__test_copper__DOT__dut__DOT__gfx_ram__v1 
                                        = (0x0000ffffU 
                                           & (((IData)(0x00000140U) 
                                               * (((IData)(vlSelfRef.test_copper__DOT__dut__DOT__gt_start_y) 
                                                   + 
                                                   ((IData)(vlSelfRef.test_copper__DOT__dut__DOT__gt_font_row) 
                                                    * (IData)(vlSelfRef.test_copper__DOT__dut__DOT__gt_scale))) 
                                                  + (IData)(vlSelfRef.test_copper__DOT__dut__DOT__gt_scale_y))) 
                                              + (((IData)(vlSelfRef.test_copper__DOT__dut__DOT__gt_pen_x) 
                                                  + 
                                                  ((IData)(vlSelfRef.test_copper__DOT__dut__DOT__gt_font_col) 
                                                   * (IData)(vlSelfRef.test_copper__DOT__dut__DOT__gt_scale))) 
                                                 + (IData)(vlSelfRef.test_copper__DOT__dut__DOT__gt_scale_x))));
                                    vlSelfRef.__VdlyCommitQueuetest_copper__DOT__dut__DOT__gfx_ram.enqueue(__VdlyVal__test_copper__DOT__dut__DOT__gfx_ram__v1, (IData)(__VdlyDim0__test_copper__DOT__dut__DOT__gfx_ram__v1));
                                }
                            }
                        }
                        if (((0x000000ffU & ((IData)(1U) 
                                             + (IData)(vlSelfRef.test_copper__DOT__dut__DOT__gt_scale_x))) 
                             < (IData)(vlSelfRef.test_copper__DOT__dut__DOT__gt_scale))) {
                            vlSelfRef.test_copper__DOT__dut__DOT__gt_scale_x 
                                = (0x000000ffU & ((IData)(1U) 
                                                  + (IData)(vlSelfRef.test_copper__DOT__dut__DOT__gt_scale_x)));
                        } else {
                            vlSelfRef.test_copper__DOT__dut__DOT__gt_scale_x = 0U;
                            if ((7U > (IData)(vlSelfRef.test_copper__DOT__dut__DOT__gt_font_col))) {
                                vlSelfRef.test_copper__DOT__dut__DOT__gt_font_col 
                                    = (7U & ((IData)(1U) 
                                             + (IData)(vlSelfRef.test_copper__DOT__dut__DOT__gt_font_col)));
                            } else {
                                vlSelfRef.test_copper__DOT__dut__DOT__gt_font_col = 0U;
                                if (((0x000000ffU & 
                                      ((IData)(1U) 
                                       + (IData)(vlSelfRef.test_copper__DOT__dut__DOT__gt_scale_y))) 
                                     < (IData)(vlSelfRef.test_copper__DOT__dut__DOT__gt_scale))) {
                                    vlSelfRef.test_copper__DOT__dut__DOT__gt_scale_y 
                                        = (0x000000ffU 
                                           & ((IData)(1U) 
                                              + (IData)(vlSelfRef.test_copper__DOT__dut__DOT__gt_scale_y)));
                                } else {
                                    vlSelfRef.test_copper__DOT__dut__DOT__gt_scale_y = 0U;
                                    if ((7U > (IData)(vlSelfRef.test_copper__DOT__dut__DOT__gt_font_row))) {
                                        __Vdly__test_copper__DOT__dut__DOT__gt_font_row 
                                            = (7U & 
                                               ((IData)(1U) 
                                                + (IData)(vlSelfRef.test_copper__DOT__dut__DOT__gt_font_row)));
                                        vlSelfRef.test_copper__DOT__dut__DOT__gt_font_byte 
                                            = vlSelfRef.test_copper__DOT__dut__DOT__font_rom
                                            [(((IData)(vlSelfRef.test_copper__DOT__dut__DOT__gt_cur_char) 
                                               << 3U) 
                                              | (7U 
                                                 & ((IData)(1U) 
                                                    + (IData)(vlSelfRef.test_copper__DOT__dut__DOT__gt_font_row))))];
                                    } else {
                                        __Vdly__test_copper__DOT__dut__DOT__gt_char_idx 
                                            = (0x0000003fU 
                                               & ((IData)(1U) 
                                                  + (IData)(vlSelfRef.test_copper__DOT__dut__DOT__gt_char_idx)));
                                        vlSelfRef.test_copper__DOT__dut__DOT__gt_pen_x 
                                            = (0x000001ffU 
                                               & ((IData)(vlSelfRef.test_copper__DOT__dut__DOT__gt_pen_x) 
                                                  + 
                                                  (0x0000ffffU 
                                                   & VL_SHIFTL_III(16,16,32, (IData)(vlSelfRef.test_copper__DOT__dut__DOT__gt_scale), 3U))));
                                        __Vdly__test_copper__DOT__dut__DOT__gt_font_row = 0U;
                                        if ((((IData)(1U) 
                                              + (IData)(vlSelfRef.test_copper__DOT__dut__DOT__gt_char_idx)) 
                                             < (IData)(vlSelfRef.test_copper__DOT__dut__DOT__gt_char_len))) {
                                            vlSelfRef.test_copper__DOT__dut__DOT__gt_cur_char 
                                                = vlSelfRef.test_copper__DOT__dut__DOT__fio_name
                                                [(0x0000003fU 
                                                  & ((IData)(1U) 
                                                     + (IData)(vlSelfRef.test_copper__DOT__dut__DOT__gt_char_idx)))];
                                            vlSelfRef.test_copper__DOT__dut__DOT__gt_font_byte 
                                                = vlSelfRef.test_copper__DOT__dut__DOT__font_rom
                                                [(vlSelfRef.test_copper__DOT__dut__DOT__fio_name
                                                  [
                                                  (0x0000003fU 
                                                   & ((IData)(1U) 
                                                      + (IData)(vlSelfRef.test_copper__DOT__dut__DOT__gt_char_idx)))] 
                                                  << 3U)];
                                        }
                                    }
                                }
                            }
                        }
                    } else {
                        __Vdly__test_copper__DOT__dut__DOT__cmd_busy = 0U;
                    }
                } else if ((1U & (IData)(vlSelfRef.test_copper__DOT__dut__DOT__cmd_op))) {
                    if (((0U < (IData)(vlSelfRef.test_copper__DOT__dut__DOT__fill_sp)) 
                         & (0x0200U > (IData)(vlSelfRef.test_copper__DOT__dut__DOT__fill_sp)))) {
                        __Vdly__test_copper__DOT__dut__DOT__fill_sp 
                            = (0x000003ffU & ((IData)(vlSelfRef.test_copper__DOT__dut__DOT__fill_sp) 
                                              - (IData)(1U)));
                        __Vdly__test_copper__DOT__dut__DOT__cmd_cx 
                            = (0x000001ffU & vlSelfRef.test_copper__DOT__dut__DOT__fill_stack
                               [(0x000001ffU & ((IData)(vlSelfRef.test_copper__DOT__dut__DOT__fill_sp) 
                                                - (IData)(1U)))]);
                        __Vdly__test_copper__DOT__dut__DOT__cmd_cy 
                            = (0x000000ffU & (vlSelfRef.test_copper__DOT__dut__DOT__fill_stack
                                              [(0x000001ffU 
                                                & ((IData)(vlSelfRef.test_copper__DOT__dut__DOT__fill_sp) 
                                                   - (IData)(1U)))] 
                                              >> 9U));
                    } else if ((0U == (IData)(vlSelfRef.test_copper__DOT__dut__DOT__fill_sp))) {
                        __Vdly__test_copper__DOT__dut__DOT__cmd_busy = 0U;
                    } else if (((((0x0140U > (IData)(vlSelfRef.test_copper__DOT__dut__DOT__cmd_cx)) 
                                  & (0xc8U > (IData)(vlSelfRef.test_copper__DOT__dut__DOT__cmd_cy))) 
                                 & (((0xf9ffU >= (0x0000ffffU 
                                                  & (((IData)(0x00000140U) 
                                                      * (IData)(vlSelfRef.test_copper__DOT__dut__DOT__cmd_cy)) 
                                                     + (IData)(vlSelfRef.test_copper__DOT__dut__DOT__cmd_cx))))
                                      ? vlSelfRef.test_copper__DOT__dut__DOT__gfx_ram
                                     [(0x0000ffffU 
                                       & (((IData)(0x00000140U) 
                                           * (IData)(vlSelfRef.test_copper__DOT__dut__DOT__cmd_cy)) 
                                          + (IData)(vlSelfRef.test_copper__DOT__dut__DOT__cmd_cx)))]
                                      : 0U) == (IData)(vlSelfRef.test_copper__DOT__dut__DOT__fill_target))) 
                                & ((IData)(vlSelfRef.test_copper__DOT__dut__DOT__fill_target) 
                                   != (IData)(vlSelfRef.test_copper__DOT__dut__DOT__gfx_color)))) {
                        vlSelfRef.test_copper__DOT__dut__DOT____Vlvbound_h3f27e46b__3 
                            = vlSelfRef.test_copper__DOT__dut__DOT__gfx_color;
                        if (VL_LIKELY(((0xf9ffU >= 
                                        (0x0000ffffU 
                                         & (((IData)(0x00000140U) 
                                             * (IData)(vlSelfRef.test_copper__DOT__dut__DOT__cmd_cy)) 
                                            + (IData)(vlSelfRef.test_copper__DOT__dut__DOT__cmd_cx))))))) {
                            __VdlyVal__test_copper__DOT__dut__DOT__gfx_ram__v2 
                                = vlSelfRef.test_copper__DOT__dut__DOT____Vlvbound_h3f27e46b__3;
                            __VdlyDim0__test_copper__DOT__dut__DOT__gfx_ram__v2 
                                = (0x0000ffffU & (((IData)(0x00000140U) 
                                                   * (IData)(vlSelfRef.test_copper__DOT__dut__DOT__cmd_cy)) 
                                                  + (IData)(vlSelfRef.test_copper__DOT__dut__DOT__cmd_cx)));
                            vlSelfRef.__VdlyCommitQueuetest_copper__DOT__dut__DOT__gfx_ram.enqueue(__VdlyVal__test_copper__DOT__dut__DOT__gfx_ram__v2, (IData)(__VdlyDim0__test_copper__DOT__dut__DOT__gfx_ram__v2));
                        }
                        if ((0x00000200U > ((IData)(3U) 
                                            + (IData)(vlSelfRef.test_copper__DOT__dut__DOT__fill_sp)))) {
                            __VdlyVal__test_copper__DOT__dut__DOT__fill_stack__v0 
                                = (((IData)(vlSelfRef.test_copper__DOT__dut__DOT__cmd_cy) 
                                    << 9U) | (0x000001ffU 
                                              & ((IData)(1U) 
                                                 + (IData)(vlSelfRef.test_copper__DOT__dut__DOT__cmd_cx))));
                            __VdlyDim0__test_copper__DOT__dut__DOT__fill_stack__v0 
                                = (0x000001ffU & (IData)(vlSelfRef.test_copper__DOT__dut__DOT__fill_sp));
                            __VdlySet__test_copper__DOT__dut__DOT__fill_stack__v0 = 1U;
                            __VdlyVal__test_copper__DOT__dut__DOT__fill_stack__v1 
                                = (((IData)(vlSelfRef.test_copper__DOT__dut__DOT__cmd_cy) 
                                    << 9U) | (0x000001ffU 
                                              & ((IData)(vlSelfRef.test_copper__DOT__dut__DOT__cmd_cx) 
                                                 - (IData)(1U))));
                            __VdlyDim0__test_copper__DOT__dut__DOT__fill_stack__v1 
                                = (0x000001ffU & ((IData)(1U) 
                                                  + (IData)(vlSelfRef.test_copper__DOT__dut__DOT__fill_sp)));
                            __VdlyVal__test_copper__DOT__dut__DOT__fill_stack__v2 
                                = ((0x0001fe00U & (
                                                   ((IData)(1U) 
                                                    + (IData)(vlSelfRef.test_copper__DOT__dut__DOT__cmd_cy)) 
                                                   << 9U)) 
                                   | (IData)(vlSelfRef.test_copper__DOT__dut__DOT__cmd_cx));
                            __VdlyDim0__test_copper__DOT__dut__DOT__fill_stack__v2 
                                = (0x000001ffU & ((IData)(2U) 
                                                  + (IData)(vlSelfRef.test_copper__DOT__dut__DOT__fill_sp)));
                            __VdlyVal__test_copper__DOT__dut__DOT__fill_stack__v3 
                                = ((0x0001fe00U & (
                                                   ((IData)(vlSelfRef.test_copper__DOT__dut__DOT__cmd_cy) 
                                                    - (IData)(1U)) 
                                                   << 9U)) 
                                   | (IData)(vlSelfRef.test_copper__DOT__dut__DOT__cmd_cx));
                            __VdlyDim0__test_copper__DOT__dut__DOT__fill_stack__v3 
                                = (0x000001ffU & ((IData)(3U) 
                                                  + (IData)(vlSelfRef.test_copper__DOT__dut__DOT__fill_sp)));
                            __Vdly__test_copper__DOT__dut__DOT__fill_sp 
                                = (0x000003ffU & ((IData)(4U) 
                                                  + (IData)(vlSelfRef.test_copper__DOT__dut__DOT__fill_sp)));
                        }
                    }
                } else {
                    __Vdly__test_copper__DOT__dut__DOT__cmd_busy = 0U;
                }
            } else if ((4U & (IData)(vlSelfRef.test_copper__DOT__dut__DOT__cmd_op))) {
                if ((2U & (IData)(vlSelfRef.test_copper__DOT__dut__DOT__cmd_op))) {
                    if ((1U & (IData)(vlSelfRef.test_copper__DOT__dut__DOT__cmd_op))) {
                        vlSelfRef.test_copper__DOT__dut__DOT____Vlvbound_h3f27e46b__2 = 0U;
                        if (VL_LIKELY(((0xf9ffU >= 
                                        (0x0000ffffU 
                                         & (((IData)(0x00000140U) 
                                             * (IData)(vlSelfRef.test_copper__DOT__dut__DOT__cmd_cy)) 
                                            + (IData)(vlSelfRef.test_copper__DOT__dut__DOT__cmd_cx))))))) {
                            __VdlyVal__test_copper__DOT__dut__DOT__gfx_ram__v3 
                                = vlSelfRef.test_copper__DOT__dut__DOT____Vlvbound_h3f27e46b__2;
                            __VdlyDim0__test_copper__DOT__dut__DOT__gfx_ram__v3 
                                = (0x0000ffffU & (((IData)(0x00000140U) 
                                                   * (IData)(vlSelfRef.test_copper__DOT__dut__DOT__cmd_cy)) 
                                                  + (IData)(vlSelfRef.test_copper__DOT__dut__DOT__cmd_cx)));
                            vlSelfRef.__VdlyCommitQueuetest_copper__DOT__dut__DOT__gfx_ram.enqueue(__VdlyVal__test_copper__DOT__dut__DOT__gfx_ram__v3, (IData)(__VdlyDim0__test_copper__DOT__dut__DOT__gfx_ram__v3));
                        }
                        if ((0x013fU == (IData)(vlSelfRef.test_copper__DOT__dut__DOT__cmd_cx))) {
                            __Vdly__test_copper__DOT__dut__DOT__cmd_cx = 0U;
                            if ((0xc7U == (IData)(vlSelfRef.test_copper__DOT__dut__DOT__cmd_cy))) {
                                __Vdly__test_copper__DOT__dut__DOT__cmd_busy = 0U;
                            } else {
                                __Vdly__test_copper__DOT__dut__DOT__cmd_cy 
                                    = (0x000000ffU 
                                       & ((IData)(1U) 
                                          + (IData)(vlSelfRef.test_copper__DOT__dut__DOT__cmd_cy)));
                            }
                        } else {
                            __Vdly__test_copper__DOT__dut__DOT__cmd_cx 
                                = (0x000001ffU & ((IData)(1U) 
                                                  + (IData)(vlSelfRef.test_copper__DOT__dut__DOT__cmd_cx)));
                        }
                    } else {
                        if (((0x0140U > (IData)(vlSelfRef.test_copper__DOT__dut__DOT__cmd_cx)) 
                             & (0xc8U > (IData)(vlSelfRef.test_copper__DOT__dut__DOT__cmd_cy)))) {
                            vlSelfRef.test_copper__DOT__dut__DOT____Vlvbound_h3f27e46b__1 
                                = vlSelfRef.test_copper__DOT__dut__DOT__gfx_color;
                            if (VL_LIKELY(((0xf9ffU 
                                            >= (0x0000ffffU 
                                                & (((IData)(0x00000140U) 
                                                    * (IData)(vlSelfRef.test_copper__DOT__dut__DOT__cmd_cy)) 
                                                   + (IData)(vlSelfRef.test_copper__DOT__dut__DOT__cmd_cx))))))) {
                                __VdlyVal__test_copper__DOT__dut__DOT__gfx_ram__v4 
                                    = vlSelfRef.test_copper__DOT__dut__DOT____Vlvbound_h3f27e46b__1;
                                __VdlyDim0__test_copper__DOT__dut__DOT__gfx_ram__v4 
                                    = (0x0000ffffU 
                                       & (((IData)(0x00000140U) 
                                           * (IData)(vlSelfRef.test_copper__DOT__dut__DOT__cmd_cy)) 
                                          + (IData)(vlSelfRef.test_copper__DOT__dut__DOT__cmd_cx)));
                                vlSelfRef.__VdlyCommitQueuetest_copper__DOT__dut__DOT__gfx_ram.enqueue(__VdlyVal__test_copper__DOT__dut__DOT__gfx_ram__v4, (IData)(__VdlyDim0__test_copper__DOT__dut__DOT__gfx_ram__v4));
                            }
                        }
                        if (((IData)(vlSelfRef.test_copper__DOT__dut__DOT__cmd_cx) 
                             >= (0x000001ffU & (IData)(vlSelfRef.test_copper__DOT__dut__DOT__cmd_x2)))) {
                            __Vdly__test_copper__DOT__dut__DOT__cmd_cx 
                                = (0x000001ffU & (IData)(vlSelfRef.test_copper__DOT__dut__DOT__cmd_x));
                            if (((IData)(vlSelfRef.test_copper__DOT__dut__DOT__cmd_cy) 
                                 >= (0x000000ffU & (IData)(vlSelfRef.test_copper__DOT__dut__DOT__cmd_y2)))) {
                                __Vdly__test_copper__DOT__dut__DOT__cmd_busy = 0U;
                            } else {
                                __Vdly__test_copper__DOT__dut__DOT__cmd_cy 
                                    = (0x000000ffU 
                                       & ((IData)(1U) 
                                          + (IData)(vlSelfRef.test_copper__DOT__dut__DOT__cmd_cy)));
                            }
                        } else {
                            __Vdly__test_copper__DOT__dut__DOT__cmd_cx 
                                = (0x000001ffU & ((IData)(1U) 
                                                  + (IData)(vlSelfRef.test_copper__DOT__dut__DOT__cmd_cx)));
                        }
                    }
                } else if ((1U & (IData)(vlSelfRef.test_copper__DOT__dut__DOT__cmd_op))) {
                    if (((0x0140U > (IData)(vlSelfRef.test_copper__DOT__dut__DOT__cmd_cx)) 
                         & (0xc8U > (IData)(vlSelfRef.test_copper__DOT__dut__DOT__cmd_cy)))) {
                        if ((((((IData)(vlSelfRef.test_copper__DOT__dut__DOT__cmd_cy) 
                                == (0x000000ffU & (IData)(vlSelfRef.test_copper__DOT__dut__DOT__cmd_y))) 
                               | ((IData)(vlSelfRef.test_copper__DOT__dut__DOT__cmd_cy) 
                                  == (0x000000ffU & (IData)(vlSelfRef.test_copper__DOT__dut__DOT__cmd_y2)))) 
                              | ((IData)(vlSelfRef.test_copper__DOT__dut__DOT__cmd_cx) 
                                 == (0x000001ffU & (IData)(vlSelfRef.test_copper__DOT__dut__DOT__cmd_x)))) 
                             | ((IData)(vlSelfRef.test_copper__DOT__dut__DOT__cmd_cx) 
                                == (0x000001ffU & (IData)(vlSelfRef.test_copper__DOT__dut__DOT__cmd_x2))))) {
                            vlSelfRef.test_copper__DOT__dut__DOT____Vlvbound_h3f27e46b__0 
                                = vlSelfRef.test_copper__DOT__dut__DOT__gfx_color;
                            if (VL_LIKELY(((0xf9ffU 
                                            >= (0x0000ffffU 
                                                & (((IData)(0x00000140U) 
                                                    * (IData)(vlSelfRef.test_copper__DOT__dut__DOT__cmd_cy)) 
                                                   + (IData)(vlSelfRef.test_copper__DOT__dut__DOT__cmd_cx))))))) {
                                __VdlyVal__test_copper__DOT__dut__DOT__gfx_ram__v5 
                                    = vlSelfRef.test_copper__DOT__dut__DOT____Vlvbound_h3f27e46b__0;
                                __VdlyDim0__test_copper__DOT__dut__DOT__gfx_ram__v5 
                                    = (0x0000ffffU 
                                       & (((IData)(0x00000140U) 
                                           * (IData)(vlSelfRef.test_copper__DOT__dut__DOT__cmd_cy)) 
                                          + (IData)(vlSelfRef.test_copper__DOT__dut__DOT__cmd_cx)));
                                vlSelfRef.__VdlyCommitQueuetest_copper__DOT__dut__DOT__gfx_ram.enqueue(__VdlyVal__test_copper__DOT__dut__DOT__gfx_ram__v5, (IData)(__VdlyDim0__test_copper__DOT__dut__DOT__gfx_ram__v5));
                            }
                        }
                    }
                    if (((IData)(vlSelfRef.test_copper__DOT__dut__DOT__cmd_cx) 
                         >= (0x000001ffU & (IData)(vlSelfRef.test_copper__DOT__dut__DOT__cmd_x2)))) {
                        __Vdly__test_copper__DOT__dut__DOT__cmd_cx 
                            = (0x000001ffU & (IData)(vlSelfRef.test_copper__DOT__dut__DOT__cmd_x));
                        if (((IData)(vlSelfRef.test_copper__DOT__dut__DOT__cmd_cy) 
                             >= (0x000000ffU & (IData)(vlSelfRef.test_copper__DOT__dut__DOT__cmd_y2)))) {
                            __Vdly__test_copper__DOT__dut__DOT__cmd_busy = 0U;
                        } else {
                            __Vdly__test_copper__DOT__dut__DOT__cmd_cy 
                                = (0x000000ffU & ((IData)(1U) 
                                                  + (IData)(vlSelfRef.test_copper__DOT__dut__DOT__cmd_cy)));
                        }
                    } else {
                        __Vdly__test_copper__DOT__dut__DOT__cmd_cx 
                            = (0x000001ffU & ((IData)(1U) 
                                              + (IData)(vlSelfRef.test_copper__DOT__dut__DOT__cmd_cx)));
                    }
                } else if (VL_LTES_III(10, (IData)(vlSelfRef.test_copper__DOT__dut__DOT__cmd_dx), (IData)(vlSelfRef.test_copper__DOT__dut__DOT__cmd_dy))) {
                    if ((((VL_LTES_III(32, 0U, (VL_EXTENDS_II(32,10, (IData)(vlSelfRef.test_copper__DOT__dut__DOT__cmd_x)) 
                                                + VL_EXTENDS_II(32,10, (IData)(vlSelfRef.test_copper__DOT__dut__DOT__cmd_dx)))) 
                           & VL_GTS_III(32, 0x00000140U, 
                                        (VL_EXTENDS_II(32,10, (IData)(vlSelfRef.test_copper__DOT__dut__DOT__cmd_x)) 
                                         + VL_EXTENDS_II(32,10, (IData)(vlSelfRef.test_copper__DOT__dut__DOT__cmd_dx))))) 
                          & VL_LTES_III(32, 0U, (VL_EXTENDS_II(32,10, (IData)(vlSelfRef.test_copper__DOT__dut__DOT__cmd_y)) 
                                                 + 
                                                 VL_EXTENDS_II(32,10, (IData)(vlSelfRef.test_copper__DOT__dut__DOT__cmd_dy))))) 
                         & VL_GTS_III(32, 0x000000c8U, 
                                      (VL_EXTENDS_II(32,10, (IData)(vlSelfRef.test_copper__DOT__dut__DOT__cmd_y)) 
                                       + VL_EXTENDS_II(32,10, (IData)(vlSelfRef.test_copper__DOT__dut__DOT__cmd_dy)))))) {
                        vlSelfRef.test_copper__DOT__dut__DOT____Vlvbound_hafb21497__0 
                            = vlSelfRef.test_copper__DOT__dut__DOT__gfx_color;
                        if (VL_LIKELY(((0xf9ffU >= 
                                        (0x0000ffffU 
                                         & (VL_MULS_III(32, (IData)(0x00000140U), 
                                                        (VL_EXTENDS_II(32,10, (IData)(vlSelfRef.test_copper__DOT__dut__DOT__cmd_y)) 
                                                         + 
                                                         VL_EXTENDS_II(32,10, (IData)(vlSelfRef.test_copper__DOT__dut__DOT__cmd_dy)))) 
                                            + (VL_EXTENDS_II(16,10, (IData)(vlSelfRef.test_copper__DOT__dut__DOT__cmd_x)) 
                                               + VL_EXTENDS_II(16,10, (IData)(vlSelfRef.test_copper__DOT__dut__DOT__cmd_dx))))))))) {
                            __VdlyVal__test_copper__DOT__dut__DOT__gfx_ram__v6 
                                = vlSelfRef.test_copper__DOT__dut__DOT____Vlvbound_hafb21497__0;
                            __VdlyDim0__test_copper__DOT__dut__DOT__gfx_ram__v6 
                                = (0x0000ffffU & (VL_MULS_III(32, (IData)(0x00000140U), 
                                                              (VL_EXTENDS_II(32,10, (IData)(vlSelfRef.test_copper__DOT__dut__DOT__cmd_y)) 
                                                               + 
                                                               VL_EXTENDS_II(32,10, (IData)(vlSelfRef.test_copper__DOT__dut__DOT__cmd_dy)))) 
                                                  + 
                                                  (VL_EXTENDS_II(16,10, (IData)(vlSelfRef.test_copper__DOT__dut__DOT__cmd_x)) 
                                                   + 
                                                   VL_EXTENDS_II(16,10, (IData)(vlSelfRef.test_copper__DOT__dut__DOT__cmd_dx)))));
                            vlSelfRef.__VdlyCommitQueuetest_copper__DOT__dut__DOT__gfx_ram.enqueue(__VdlyVal__test_copper__DOT__dut__DOT__gfx_ram__v6, (IData)(__VdlyDim0__test_copper__DOT__dut__DOT__gfx_ram__v6));
                        }
                    }
                    if ((((VL_LTES_III(32, 0U, (VL_EXTENDS_II(32,10, (IData)(vlSelfRef.test_copper__DOT__dut__DOT__cmd_x)) 
                                                - VL_EXTENDS_II(32,10, (IData)(vlSelfRef.test_copper__DOT__dut__DOT__cmd_dx)))) 
                           & VL_GTS_III(32, 0x00000140U, 
                                        (VL_EXTENDS_II(32,10, (IData)(vlSelfRef.test_copper__DOT__dut__DOT__cmd_x)) 
                                         - VL_EXTENDS_II(32,10, (IData)(vlSelfRef.test_copper__DOT__dut__DOT__cmd_dx))))) 
                          & VL_LTES_III(32, 0U, (VL_EXTENDS_II(32,10, (IData)(vlSelfRef.test_copper__DOT__dut__DOT__cmd_y)) 
                                                 + 
                                                 VL_EXTENDS_II(32,10, (IData)(vlSelfRef.test_copper__DOT__dut__DOT__cmd_dy))))) 
                         & VL_GTS_III(32, 0x000000c8U, 
                                      (VL_EXTENDS_II(32,10, (IData)(vlSelfRef.test_copper__DOT__dut__DOT__cmd_y)) 
                                       + VL_EXTENDS_II(32,10, (IData)(vlSelfRef.test_copper__DOT__dut__DOT__cmd_dy)))))) {
                        vlSelfRef.test_copper__DOT__dut__DOT____Vlvbound_haf1931bd__0 
                            = vlSelfRef.test_copper__DOT__dut__DOT__gfx_color;
                        if (VL_LIKELY(((0xf9ffU >= 
                                        (0x0000ffffU 
                                         & (VL_MULS_III(32, (IData)(0x00000140U), 
                                                        (VL_EXTENDS_II(32,10, (IData)(vlSelfRef.test_copper__DOT__dut__DOT__cmd_y)) 
                                                         + 
                                                         VL_EXTENDS_II(32,10, (IData)(vlSelfRef.test_copper__DOT__dut__DOT__cmd_dy)))) 
                                            + (VL_EXTENDS_II(16,10, (IData)(vlSelfRef.test_copper__DOT__dut__DOT__cmd_x)) 
                                               - VL_EXTENDS_II(16,10, (IData)(vlSelfRef.test_copper__DOT__dut__DOT__cmd_dx))))))))) {
                            __VdlyVal__test_copper__DOT__dut__DOT__gfx_ram__v7 
                                = vlSelfRef.test_copper__DOT__dut__DOT____Vlvbound_haf1931bd__0;
                            __VdlyDim0__test_copper__DOT__dut__DOT__gfx_ram__v7 
                                = (0x0000ffffU & (VL_MULS_III(32, (IData)(0x00000140U), 
                                                              (VL_EXTENDS_II(32,10, (IData)(vlSelfRef.test_copper__DOT__dut__DOT__cmd_y)) 
                                                               + 
                                                               VL_EXTENDS_II(32,10, (IData)(vlSelfRef.test_copper__DOT__dut__DOT__cmd_dy)))) 
                                                  + 
                                                  (VL_EXTENDS_II(16,10, (IData)(vlSelfRef.test_copper__DOT__dut__DOT__cmd_x)) 
                                                   - 
                                                   VL_EXTENDS_II(16,10, (IData)(vlSelfRef.test_copper__DOT__dut__DOT__cmd_dx)))));
                            vlSelfRef.__VdlyCommitQueuetest_copper__DOT__dut__DOT__gfx_ram.enqueue(__VdlyVal__test_copper__DOT__dut__DOT__gfx_ram__v7, (IData)(__VdlyDim0__test_copper__DOT__dut__DOT__gfx_ram__v7));
                        }
                    }
                    if ((((VL_LTES_III(32, 0U, (VL_EXTENDS_II(32,10, (IData)(vlSelfRef.test_copper__DOT__dut__DOT__cmd_x)) 
                                                + VL_EXTENDS_II(32,10, (IData)(vlSelfRef.test_copper__DOT__dut__DOT__cmd_dx)))) 
                           & VL_GTS_III(32, 0x00000140U, 
                                        (VL_EXTENDS_II(32,10, (IData)(vlSelfRef.test_copper__DOT__dut__DOT__cmd_x)) 
                                         + VL_EXTENDS_II(32,10, (IData)(vlSelfRef.test_copper__DOT__dut__DOT__cmd_dx))))) 
                          & VL_LTES_III(32, 0U, (VL_EXTENDS_II(32,10, (IData)(vlSelfRef.test_copper__DOT__dut__DOT__cmd_y)) 
                                                 - 
                                                 VL_EXTENDS_II(32,10, (IData)(vlSelfRef.test_copper__DOT__dut__DOT__cmd_dy))))) 
                         & VL_GTS_III(32, 0x000000c8U, 
                                      (VL_EXTENDS_II(32,10, (IData)(vlSelfRef.test_copper__DOT__dut__DOT__cmd_y)) 
                                       - VL_EXTENDS_II(32,10, (IData)(vlSelfRef.test_copper__DOT__dut__DOT__cmd_dy)))))) {
                        vlSelfRef.test_copper__DOT__dut__DOT____Vlvbound_hc84a7865__0 
                            = vlSelfRef.test_copper__DOT__dut__DOT__gfx_color;
                        if (VL_LIKELY(((0xf9ffU >= 
                                        (0x0000ffffU 
                                         & (VL_MULS_III(32, (IData)(0x00000140U), 
                                                        (VL_EXTENDS_II(32,10, (IData)(vlSelfRef.test_copper__DOT__dut__DOT__cmd_y)) 
                                                         - 
                                                         VL_EXTENDS_II(32,10, (IData)(vlSelfRef.test_copper__DOT__dut__DOT__cmd_dy)))) 
                                            + (VL_EXTENDS_II(16,10, (IData)(vlSelfRef.test_copper__DOT__dut__DOT__cmd_x)) 
                                               + VL_EXTENDS_II(16,10, (IData)(vlSelfRef.test_copper__DOT__dut__DOT__cmd_dx))))))))) {
                            __VdlyVal__test_copper__DOT__dut__DOT__gfx_ram__v8 
                                = vlSelfRef.test_copper__DOT__dut__DOT____Vlvbound_hc84a7865__0;
                            __VdlyDim0__test_copper__DOT__dut__DOT__gfx_ram__v8 
                                = (0x0000ffffU & (VL_MULS_III(32, (IData)(0x00000140U), 
                                                              (VL_EXTENDS_II(32,10, (IData)(vlSelfRef.test_copper__DOT__dut__DOT__cmd_y)) 
                                                               - 
                                                               VL_EXTENDS_II(32,10, (IData)(vlSelfRef.test_copper__DOT__dut__DOT__cmd_dy)))) 
                                                  + 
                                                  (VL_EXTENDS_II(16,10, (IData)(vlSelfRef.test_copper__DOT__dut__DOT__cmd_x)) 
                                                   + 
                                                   VL_EXTENDS_II(16,10, (IData)(vlSelfRef.test_copper__DOT__dut__DOT__cmd_dx)))));
                            vlSelfRef.__VdlyCommitQueuetest_copper__DOT__dut__DOT__gfx_ram.enqueue(__VdlyVal__test_copper__DOT__dut__DOT__gfx_ram__v8, (IData)(__VdlyDim0__test_copper__DOT__dut__DOT__gfx_ram__v8));
                        }
                    }
                    if ((((VL_LTES_III(32, 0U, (VL_EXTENDS_II(32,10, (IData)(vlSelfRef.test_copper__DOT__dut__DOT__cmd_x)) 
                                                - VL_EXTENDS_II(32,10, (IData)(vlSelfRef.test_copper__DOT__dut__DOT__cmd_dx)))) 
                           & VL_GTS_III(32, 0x00000140U, 
                                        (VL_EXTENDS_II(32,10, (IData)(vlSelfRef.test_copper__DOT__dut__DOT__cmd_x)) 
                                         - VL_EXTENDS_II(32,10, (IData)(vlSelfRef.test_copper__DOT__dut__DOT__cmd_dx))))) 
                          & VL_LTES_III(32, 0U, (VL_EXTENDS_II(32,10, (IData)(vlSelfRef.test_copper__DOT__dut__DOT__cmd_y)) 
                                                 - 
                                                 VL_EXTENDS_II(32,10, (IData)(vlSelfRef.test_copper__DOT__dut__DOT__cmd_dy))))) 
                         & VL_GTS_III(32, 0x000000c8U, 
                                      (VL_EXTENDS_II(32,10, (IData)(vlSelfRef.test_copper__DOT__dut__DOT__cmd_y)) 
                                       - VL_EXTENDS_II(32,10, (IData)(vlSelfRef.test_copper__DOT__dut__DOT__cmd_dy)))))) {
                        vlSelfRef.test_copper__DOT__dut__DOT____Vlvbound_hed509973__0 
                            = vlSelfRef.test_copper__DOT__dut__DOT__gfx_color;
                        if (VL_LIKELY(((0xf9ffU >= 
                                        (0x0000ffffU 
                                         & (VL_MULS_III(32, (IData)(0x00000140U), 
                                                        (VL_EXTENDS_II(32,10, (IData)(vlSelfRef.test_copper__DOT__dut__DOT__cmd_y)) 
                                                         - 
                                                         VL_EXTENDS_II(32,10, (IData)(vlSelfRef.test_copper__DOT__dut__DOT__cmd_dy)))) 
                                            + (VL_EXTENDS_II(16,10, (IData)(vlSelfRef.test_copper__DOT__dut__DOT__cmd_x)) 
                                               - VL_EXTENDS_II(16,10, (IData)(vlSelfRef.test_copper__DOT__dut__DOT__cmd_dx))))))))) {
                            __VdlyVal__test_copper__DOT__dut__DOT__gfx_ram__v9 
                                = vlSelfRef.test_copper__DOT__dut__DOT____Vlvbound_hed509973__0;
                            __VdlyDim0__test_copper__DOT__dut__DOT__gfx_ram__v9 
                                = (0x0000ffffU & (VL_MULS_III(32, (IData)(0x00000140U), 
                                                              (VL_EXTENDS_II(32,10, (IData)(vlSelfRef.test_copper__DOT__dut__DOT__cmd_y)) 
                                                               - 
                                                               VL_EXTENDS_II(32,10, (IData)(vlSelfRef.test_copper__DOT__dut__DOT__cmd_dy)))) 
                                                  + 
                                                  (VL_EXTENDS_II(16,10, (IData)(vlSelfRef.test_copper__DOT__dut__DOT__cmd_x)) 
                                                   - 
                                                   VL_EXTENDS_II(16,10, (IData)(vlSelfRef.test_copper__DOT__dut__DOT__cmd_dx)))));
                            vlSelfRef.__VdlyCommitQueuetest_copper__DOT__dut__DOT__gfx_ram.enqueue(__VdlyVal__test_copper__DOT__dut__DOT__gfx_ram__v9, (IData)(__VdlyDim0__test_copper__DOT__dut__DOT__gfx_ram__v9));
                        }
                    }
                    if ((((VL_LTES_III(32, 0U, (VL_EXTENDS_II(32,10, (IData)(vlSelfRef.test_copper__DOT__dut__DOT__cmd_x)) 
                                                + VL_EXTENDS_II(32,10, (IData)(vlSelfRef.test_copper__DOT__dut__DOT__cmd_dy)))) 
                           & VL_GTS_III(32, 0x00000140U, 
                                        (VL_EXTENDS_II(32,10, (IData)(vlSelfRef.test_copper__DOT__dut__DOT__cmd_x)) 
                                         + VL_EXTENDS_II(32,10, (IData)(vlSelfRef.test_copper__DOT__dut__DOT__cmd_dy))))) 
                          & VL_LTES_III(32, 0U, (VL_EXTENDS_II(32,10, (IData)(vlSelfRef.test_copper__DOT__dut__DOT__cmd_y)) 
                                                 + 
                                                 VL_EXTENDS_II(32,10, (IData)(vlSelfRef.test_copper__DOT__dut__DOT__cmd_dx))))) 
                         & VL_GTS_III(32, 0x000000c8U, 
                                      (VL_EXTENDS_II(32,10, (IData)(vlSelfRef.test_copper__DOT__dut__DOT__cmd_y)) 
                                       + VL_EXTENDS_II(32,10, (IData)(vlSelfRef.test_copper__DOT__dut__DOT__cmd_dx)))))) {
                        vlSelfRef.test_copper__DOT__dut__DOT____Vlvbound_hd5133de6__0 
                            = vlSelfRef.test_copper__DOT__dut__DOT__gfx_color;
                        if (VL_LIKELY(((0xf9ffU >= 
                                        (0x0000ffffU 
                                         & (VL_MULS_III(32, (IData)(0x00000140U), 
                                                        (VL_EXTENDS_II(32,10, (IData)(vlSelfRef.test_copper__DOT__dut__DOT__cmd_y)) 
                                                         + 
                                                         VL_EXTENDS_II(32,10, (IData)(vlSelfRef.test_copper__DOT__dut__DOT__cmd_dx)))) 
                                            + (VL_EXTENDS_II(16,10, (IData)(vlSelfRef.test_copper__DOT__dut__DOT__cmd_x)) 
                                               + VL_EXTENDS_II(16,10, (IData)(vlSelfRef.test_copper__DOT__dut__DOT__cmd_dy))))))))) {
                            __VdlyVal__test_copper__DOT__dut__DOT__gfx_ram__v10 
                                = vlSelfRef.test_copper__DOT__dut__DOT____Vlvbound_hd5133de6__0;
                            __VdlyDim0__test_copper__DOT__dut__DOT__gfx_ram__v10 
                                = (0x0000ffffU & (VL_MULS_III(32, (IData)(0x00000140U), 
                                                              (VL_EXTENDS_II(32,10, (IData)(vlSelfRef.test_copper__DOT__dut__DOT__cmd_y)) 
                                                               + 
                                                               VL_EXTENDS_II(32,10, (IData)(vlSelfRef.test_copper__DOT__dut__DOT__cmd_dx)))) 
                                                  + 
                                                  (VL_EXTENDS_II(16,10, (IData)(vlSelfRef.test_copper__DOT__dut__DOT__cmd_x)) 
                                                   + 
                                                   VL_EXTENDS_II(16,10, (IData)(vlSelfRef.test_copper__DOT__dut__DOT__cmd_dy)))));
                            vlSelfRef.__VdlyCommitQueuetest_copper__DOT__dut__DOT__gfx_ram.enqueue(__VdlyVal__test_copper__DOT__dut__DOT__gfx_ram__v10, (IData)(__VdlyDim0__test_copper__DOT__dut__DOT__gfx_ram__v10));
                        }
                    }
                    if ((((VL_LTES_III(32, 0U, (VL_EXTENDS_II(32,10, (IData)(vlSelfRef.test_copper__DOT__dut__DOT__cmd_x)) 
                                                - VL_EXTENDS_II(32,10, (IData)(vlSelfRef.test_copper__DOT__dut__DOT__cmd_dy)))) 
                           & VL_GTS_III(32, 0x00000140U, 
                                        (VL_EXTENDS_II(32,10, (IData)(vlSelfRef.test_copper__DOT__dut__DOT__cmd_x)) 
                                         - VL_EXTENDS_II(32,10, (IData)(vlSelfRef.test_copper__DOT__dut__DOT__cmd_dy))))) 
                          & VL_LTES_III(32, 0U, (VL_EXTENDS_II(32,10, (IData)(vlSelfRef.test_copper__DOT__dut__DOT__cmd_y)) 
                                                 + 
                                                 VL_EXTENDS_II(32,10, (IData)(vlSelfRef.test_copper__DOT__dut__DOT__cmd_dx))))) 
                         & VL_GTS_III(32, 0x000000c8U, 
                                      (VL_EXTENDS_II(32,10, (IData)(vlSelfRef.test_copper__DOT__dut__DOT__cmd_y)) 
                                       + VL_EXTENDS_II(32,10, (IData)(vlSelfRef.test_copper__DOT__dut__DOT__cmd_dx)))))) {
                        vlSelfRef.test_copper__DOT__dut__DOT____Vlvbound_hd5fa5e14__0 
                            = vlSelfRef.test_copper__DOT__dut__DOT__gfx_color;
                        if (VL_LIKELY(((0xf9ffU >= 
                                        (0x0000ffffU 
                                         & (VL_MULS_III(32, (IData)(0x00000140U), 
                                                        (VL_EXTENDS_II(32,10, (IData)(vlSelfRef.test_copper__DOT__dut__DOT__cmd_y)) 
                                                         + 
                                                         VL_EXTENDS_II(32,10, (IData)(vlSelfRef.test_copper__DOT__dut__DOT__cmd_dx)))) 
                                            + (VL_EXTENDS_II(16,10, (IData)(vlSelfRef.test_copper__DOT__dut__DOT__cmd_x)) 
                                               - VL_EXTENDS_II(16,10, (IData)(vlSelfRef.test_copper__DOT__dut__DOT__cmd_dy))))))))) {
                            __VdlyVal__test_copper__DOT__dut__DOT__gfx_ram__v11 
                                = vlSelfRef.test_copper__DOT__dut__DOT____Vlvbound_hd5fa5e14__0;
                            __VdlyDim0__test_copper__DOT__dut__DOT__gfx_ram__v11 
                                = (0x0000ffffU & (VL_MULS_III(32, (IData)(0x00000140U), 
                                                              (VL_EXTENDS_II(32,10, (IData)(vlSelfRef.test_copper__DOT__dut__DOT__cmd_y)) 
                                                               + 
                                                               VL_EXTENDS_II(32,10, (IData)(vlSelfRef.test_copper__DOT__dut__DOT__cmd_dx)))) 
                                                  + 
                                                  (VL_EXTENDS_II(16,10, (IData)(vlSelfRef.test_copper__DOT__dut__DOT__cmd_x)) 
                                                   - 
                                                   VL_EXTENDS_II(16,10, (IData)(vlSelfRef.test_copper__DOT__dut__DOT__cmd_dy)))));
                            vlSelfRef.__VdlyCommitQueuetest_copper__DOT__dut__DOT__gfx_ram.enqueue(__VdlyVal__test_copper__DOT__dut__DOT__gfx_ram__v11, (IData)(__VdlyDim0__test_copper__DOT__dut__DOT__gfx_ram__v11));
                        }
                    }
                    if ((((VL_LTES_III(32, 0U, (VL_EXTENDS_II(32,10, (IData)(vlSelfRef.test_copper__DOT__dut__DOT__cmd_x)) 
                                                + VL_EXTENDS_II(32,10, (IData)(vlSelfRef.test_copper__DOT__dut__DOT__cmd_dy)))) 
                           & VL_GTS_III(32, 0x00000140U, 
                                        (VL_EXTENDS_II(32,10, (IData)(vlSelfRef.test_copper__DOT__dut__DOT__cmd_x)) 
                                         + VL_EXTENDS_II(32,10, (IData)(vlSelfRef.test_copper__DOT__dut__DOT__cmd_dy))))) 
                          & VL_LTES_III(32, 0U, (VL_EXTENDS_II(32,10, (IData)(vlSelfRef.test_copper__DOT__dut__DOT__cmd_y)) 
                                                 - 
                                                 VL_EXTENDS_II(32,10, (IData)(vlSelfRef.test_copper__DOT__dut__DOT__cmd_dx))))) 
                         & VL_GTS_III(32, 0x000000c8U, 
                                      (VL_EXTENDS_II(32,10, (IData)(vlSelfRef.test_copper__DOT__dut__DOT__cmd_y)) 
                                       - VL_EXTENDS_II(32,10, (IData)(vlSelfRef.test_copper__DOT__dut__DOT__cmd_dx)))))) {
                        vlSelfRef.test_copper__DOT__dut__DOT____Vlvbound_haa426699__0 
                            = vlSelfRef.test_copper__DOT__dut__DOT__gfx_color;
                        if (VL_LIKELY(((0xf9ffU >= 
                                        (0x0000ffffU 
                                         & (VL_MULS_III(32, (IData)(0x00000140U), 
                                                        (VL_EXTENDS_II(32,10, (IData)(vlSelfRef.test_copper__DOT__dut__DOT__cmd_y)) 
                                                         - 
                                                         VL_EXTENDS_II(32,10, (IData)(vlSelfRef.test_copper__DOT__dut__DOT__cmd_dx)))) 
                                            + (VL_EXTENDS_II(16,10, (IData)(vlSelfRef.test_copper__DOT__dut__DOT__cmd_x)) 
                                               + VL_EXTENDS_II(16,10, (IData)(vlSelfRef.test_copper__DOT__dut__DOT__cmd_dy))))))))) {
                            __VdlyVal__test_copper__DOT__dut__DOT__gfx_ram__v12 
                                = vlSelfRef.test_copper__DOT__dut__DOT____Vlvbound_haa426699__0;
                            __VdlyDim0__test_copper__DOT__dut__DOT__gfx_ram__v12 
                                = (0x0000ffffU & (VL_MULS_III(32, (IData)(0x00000140U), 
                                                              (VL_EXTENDS_II(32,10, (IData)(vlSelfRef.test_copper__DOT__dut__DOT__cmd_y)) 
                                                               - 
                                                               VL_EXTENDS_II(32,10, (IData)(vlSelfRef.test_copper__DOT__dut__DOT__cmd_dx)))) 
                                                  + 
                                                  (VL_EXTENDS_II(16,10, (IData)(vlSelfRef.test_copper__DOT__dut__DOT__cmd_x)) 
                                                   + 
                                                   VL_EXTENDS_II(16,10, (IData)(vlSelfRef.test_copper__DOT__dut__DOT__cmd_dy)))));
                            vlSelfRef.__VdlyCommitQueuetest_copper__DOT__dut__DOT__gfx_ram.enqueue(__VdlyVal__test_copper__DOT__dut__DOT__gfx_ram__v12, (IData)(__VdlyDim0__test_copper__DOT__dut__DOT__gfx_ram__v12));
                        }
                    }
                    if ((((VL_LTES_III(32, 0U, (VL_EXTENDS_II(32,10, (IData)(vlSelfRef.test_copper__DOT__dut__DOT__cmd_x)) 
                                                - VL_EXTENDS_II(32,10, (IData)(vlSelfRef.test_copper__DOT__dut__DOT__cmd_dy)))) 
                           & VL_GTS_III(32, 0x00000140U, 
                                        (VL_EXTENDS_II(32,10, (IData)(vlSelfRef.test_copper__DOT__dut__DOT__cmd_x)) 
                                         - VL_EXTENDS_II(32,10, (IData)(vlSelfRef.test_copper__DOT__dut__DOT__cmd_dy))))) 
                          & VL_LTES_III(32, 0U, (VL_EXTENDS_II(32,10, (IData)(vlSelfRef.test_copper__DOT__dut__DOT__cmd_y)) 
                                                 - 
                                                 VL_EXTENDS_II(32,10, (IData)(vlSelfRef.test_copper__DOT__dut__DOT__cmd_dx))))) 
                         & VL_GTS_III(32, 0x000000c8U, 
                                      (VL_EXTENDS_II(32,10, (IData)(vlSelfRef.test_copper__DOT__dut__DOT__cmd_y)) 
                                       - VL_EXTENDS_II(32,10, (IData)(vlSelfRef.test_copper__DOT__dut__DOT__cmd_dx)))))) {
                        vlSelfRef.test_copper__DOT__dut__DOT____Vlvbound_h9f4406d7__0 
                            = vlSelfRef.test_copper__DOT__dut__DOT__gfx_color;
                        if (VL_LIKELY(((0xf9ffU >= 
                                        (0x0000ffffU 
                                         & (VL_MULS_III(32, (IData)(0x00000140U), 
                                                        (VL_EXTENDS_II(32,10, (IData)(vlSelfRef.test_copper__DOT__dut__DOT__cmd_y)) 
                                                         - 
                                                         VL_EXTENDS_II(32,10, (IData)(vlSelfRef.test_copper__DOT__dut__DOT__cmd_dx)))) 
                                            + (VL_EXTENDS_II(16,10, (IData)(vlSelfRef.test_copper__DOT__dut__DOT__cmd_x)) 
                                               - VL_EXTENDS_II(16,10, (IData)(vlSelfRef.test_copper__DOT__dut__DOT__cmd_dy))))))))) {
                            __VdlyVal__test_copper__DOT__dut__DOT__gfx_ram__v13 
                                = vlSelfRef.test_copper__DOT__dut__DOT____Vlvbound_h9f4406d7__0;
                            __VdlyDim0__test_copper__DOT__dut__DOT__gfx_ram__v13 
                                = (0x0000ffffU & (VL_MULS_III(32, (IData)(0x00000140U), 
                                                              (VL_EXTENDS_II(32,10, (IData)(vlSelfRef.test_copper__DOT__dut__DOT__cmd_y)) 
                                                               - 
                                                               VL_EXTENDS_II(32,10, (IData)(vlSelfRef.test_copper__DOT__dut__DOT__cmd_dx)))) 
                                                  + 
                                                  (VL_EXTENDS_II(16,10, (IData)(vlSelfRef.test_copper__DOT__dut__DOT__cmd_x)) 
                                                   - 
                                                   VL_EXTENDS_II(16,10, (IData)(vlSelfRef.test_copper__DOT__dut__DOT__cmd_dy)))));
                            vlSelfRef.__VdlyCommitQueuetest_copper__DOT__dut__DOT__gfx_ram.enqueue(__VdlyVal__test_copper__DOT__dut__DOT__gfx_ram__v13, (IData)(__VdlyDim0__test_copper__DOT__dut__DOT__gfx_ram__v13));
                        }
                    }
                    __Vdly__test_copper__DOT__dut__DOT__cmd_dx 
                        = (0x000003ffU & ((IData)(1U) 
                                          + VL_EXTENDS_II(10,10, (IData)(vlSelfRef.test_copper__DOT__dut__DOT__cmd_dx))));
                    if (VL_GTS_III(32, 0U, VL_EXTENDS_II(32,10, (IData)(vlSelfRef.test_copper__DOT__dut__DOT__cmd_err)))) {
                        __Vdly__test_copper__DOT__dut__DOT__cmd_err 
                            = (0x000003ffU & ((IData)(3U) 
                                              + (VL_EXTENDS_II(10,10, (IData)(vlSelfRef.test_copper__DOT__dut__DOT__cmd_err)) 
                                                 + 
                                                 VL_MULS_III(32, (IData)(2U), 
                                                             VL_EXTENDS_II(32,10, (IData)(vlSelfRef.test_copper__DOT__dut__DOT__cmd_dx))))));
                    } else {
                        __Vdly__test_copper__DOT__dut__DOT__cmd_err 
                            = (0x000003ffU & ((IData)(5U) 
                                              + (VL_EXTENDS_II(10,10, (IData)(vlSelfRef.test_copper__DOT__dut__DOT__cmd_err)) 
                                                 + 
                                                 VL_MULS_III(32, (IData)(2U), 
                                                             (VL_EXTENDS_II(32,10, (IData)(vlSelfRef.test_copper__DOT__dut__DOT__cmd_dx)) 
                                                              - 
                                                              VL_EXTENDS_II(32,10, (IData)(vlSelfRef.test_copper__DOT__dut__DOT__cmd_dy)))))));
                        __Vdly__test_copper__DOT__dut__DOT__cmd_dy 
                            = (0x000003ffU & (VL_EXTENDS_II(10,10, (IData)(vlSelfRef.test_copper__DOT__dut__DOT__cmd_dy)) 
                                              - (IData)(1U)));
                    }
                } else {
                    __Vdly__test_copper__DOT__dut__DOT__cmd_busy = 0U;
                }
            } else if ((2U & (IData)(vlSelfRef.test_copper__DOT__dut__DOT__cmd_op))) {
                if ((1U & (IData)(vlSelfRef.test_copper__DOT__dut__DOT__cmd_op))) {
                    if ((((VL_LTES_III(32, 0U, VL_EXTENDS_II(32,10, (IData)(vlSelfRef.test_copper__DOT__dut__DOT__cmd_x))) 
                           & VL_GTS_III(32, 0x00000140U, 
                                        VL_EXTENDS_II(32,10, (IData)(vlSelfRef.test_copper__DOT__dut__DOT__cmd_x)))) 
                          & VL_LTES_III(32, 0U, VL_EXTENDS_II(32,10, (IData)(vlSelfRef.test_copper__DOT__dut__DOT__cmd_y)))) 
                         & VL_GTS_III(32, 0x000000c8U, 
                                      VL_EXTENDS_II(32,10, (IData)(vlSelfRef.test_copper__DOT__dut__DOT__cmd_y))))) {
                        vlSelfRef.test_copper__DOT__dut__DOT____Vlvbound_hc0a966b6__0 
                            = vlSelfRef.test_copper__DOT__dut__DOT__gfx_color;
                        if (VL_LIKELY(((0xf9ffU >= 
                                        (0x0000ffffU 
                                         & (VL_MULS_III(32, (IData)(0x00000140U), 
                                                        VL_EXTENDS_II(32,10, (IData)(vlSelfRef.test_copper__DOT__dut__DOT__cmd_y))) 
                                            + VL_EXTENDS_II(16,10, (IData)(vlSelfRef.test_copper__DOT__dut__DOT__cmd_x)))))))) {
                            __VdlyVal__test_copper__DOT__dut__DOT__gfx_ram__v14 
                                = vlSelfRef.test_copper__DOT__dut__DOT____Vlvbound_hc0a966b6__0;
                            __VdlyDim0__test_copper__DOT__dut__DOT__gfx_ram__v14 
                                = (0x0000ffffU & (VL_MULS_III(32, (IData)(0x00000140U), 
                                                              VL_EXTENDS_II(32,10, (IData)(vlSelfRef.test_copper__DOT__dut__DOT__cmd_y))) 
                                                  + 
                                                  VL_EXTENDS_II(16,10, (IData)(vlSelfRef.test_copper__DOT__dut__DOT__cmd_x))));
                            vlSelfRef.__VdlyCommitQueuetest_copper__DOT__dut__DOT__gfx_ram.enqueue(__VdlyVal__test_copper__DOT__dut__DOT__gfx_ram__v14, (IData)(__VdlyDim0__test_copper__DOT__dut__DOT__gfx_ram__v14));
                        }
                    }
                    if ((((((((IData)(vlSelfRef.test_copper__DOT__dut__DOT__cmd_x) 
                              == (IData)(vlSelfRef.test_copper__DOT__dut__DOT__cmd_x2)) 
                             & ((IData)(vlSelfRef.test_copper__DOT__dut__DOT__cmd_y) 
                                == (IData)(vlSelfRef.test_copper__DOT__dut__DOT__cmd_y2))) 
                            | VL_GTS_III(32, 0xfffffff0U, 
                                         VL_EXTENDS_II(32,10, (IData)(vlSelfRef.test_copper__DOT__dut__DOT__cmd_x)))) 
                           | VL_LTS_III(32, 0x00000150U, 
                                        VL_EXTENDS_II(32,10, (IData)(vlSelfRef.test_copper__DOT__dut__DOT__cmd_x)))) 
                          | VL_GTS_III(32, 0xfffffff0U, 
                                       VL_EXTENDS_II(32,10, (IData)(vlSelfRef.test_copper__DOT__dut__DOT__cmd_y)))) 
                         | VL_LTS_III(32, 0x000000d8U, 
                                      VL_EXTENDS_II(32,10, (IData)(vlSelfRef.test_copper__DOT__dut__DOT__cmd_y))))) {
                        __Vdly__test_copper__DOT__dut__DOT__cmd_busy = 0U;
                    } else if ((VL_GTES_III(32, VL_MULS_III(32, (IData)(2U), 
                                                            VL_EXTENDS_II(32,10, (IData)(vlSelfRef.test_copper__DOT__dut__DOT__cmd_err))), 
                                            VL_EXTENDS_II(32,10, (IData)(vlSelfRef.test_copper__DOT__dut__DOT__cmd_dy))) 
                                & VL_LTES_III(32, VL_MULS_III(32, (IData)(2U), 
                                                              VL_EXTENDS_II(32,10, (IData)(vlSelfRef.test_copper__DOT__dut__DOT__cmd_err))), 
                                              VL_EXTENDS_II(32,10, (IData)(vlSelfRef.test_copper__DOT__dut__DOT__cmd_dx))))) {
                        __Vdly__test_copper__DOT__dut__DOT__cmd_err 
                            = (0x000003ffU & (((IData)(vlSelfRef.test_copper__DOT__dut__DOT__cmd_err) 
                                               + (IData)(vlSelfRef.test_copper__DOT__dut__DOT__cmd_dy)) 
                                              + (IData)(vlSelfRef.test_copper__DOT__dut__DOT__cmd_dx)));
                        __Vdly__test_copper__DOT__dut__DOT__cmd_x 
                            = (0x000003ffU & ((IData)(vlSelfRef.test_copper__DOT__dut__DOT__cmd_x) 
                                              + (IData)(vlSelfRef.test_copper__DOT__dut__DOT__cmd_sx)));
                        __Vdly__test_copper__DOT__dut__DOT__cmd_y 
                            = (0x000003ffU & ((IData)(vlSelfRef.test_copper__DOT__dut__DOT__cmd_y) 
                                              + (IData)(vlSelfRef.test_copper__DOT__dut__DOT__cmd_sy)));
                    } else if (VL_GTES_III(32, VL_MULS_III(32, (IData)(2U), 
                                                           VL_EXTENDS_II(32,10, (IData)(vlSelfRef.test_copper__DOT__dut__DOT__cmd_err))), 
                                           VL_EXTENDS_II(32,10, (IData)(vlSelfRef.test_copper__DOT__dut__DOT__cmd_dy)))) {
                        __Vdly__test_copper__DOT__dut__DOT__cmd_err 
                            = (0x000003ffU & ((IData)(vlSelfRef.test_copper__DOT__dut__DOT__cmd_err) 
                                              + (IData)(vlSelfRef.test_copper__DOT__dut__DOT__cmd_dy)));
                        __Vdly__test_copper__DOT__dut__DOT__cmd_x 
                            = (0x000003ffU & ((IData)(vlSelfRef.test_copper__DOT__dut__DOT__cmd_x) 
                                              + (IData)(vlSelfRef.test_copper__DOT__dut__DOT__cmd_sx)));
                    } else if (VL_LTES_III(32, VL_MULS_III(32, (IData)(2U), 
                                                           VL_EXTENDS_II(32,10, (IData)(vlSelfRef.test_copper__DOT__dut__DOT__cmd_err))), 
                                           VL_EXTENDS_II(32,10, (IData)(vlSelfRef.test_copper__DOT__dut__DOT__cmd_dx)))) {
                        __Vdly__test_copper__DOT__dut__DOT__cmd_err 
                            = (0x000003ffU & ((IData)(vlSelfRef.test_copper__DOT__dut__DOT__cmd_err) 
                                              + (IData)(vlSelfRef.test_copper__DOT__dut__DOT__cmd_dx)));
                        __Vdly__test_copper__DOT__dut__DOT__cmd_y 
                            = (0x000003ffU & ((IData)(vlSelfRef.test_copper__DOT__dut__DOT__cmd_y) 
                                              + (IData)(vlSelfRef.test_copper__DOT__dut__DOT__cmd_sy)));
                    }
                } else {
                    __Vdly__test_copper__DOT__dut__DOT__cmd_busy = 0U;
                }
            } else {
                __Vdly__test_copper__DOT__dut__DOT__cmd_busy = 0U;
            }
        }
        if (vlSelfRef.test_copper__DOT__dut__DOT__copper_loading) {
            if (((IData)(vlSelfRef.test_copper__DOT__dut__DOT__copper_load_idx) 
                 < vlSelfRef.test_copper__DOT__dut__DOT__copper_list_count
                 [vlSelfRef.test_copper__DOT__dut__DOT__copper_load_src])) {
                __VdlyVal__test_copper__DOT__dut__DOT__copper_pos__v0 
                    = vlSelfRef.test_copper__DOT__dut__DOT__copper_list_pos
                    [(((IData)(vlSelfRef.test_copper__DOT__dut__DOT__copper_load_src) 
                       << 5U) | (0x0000001fU & (IData)(vlSelfRef.test_copper__DOT__dut__DOT__copper_load_idx)))];
                __VdlyDim0__test_copper__DOT__dut__DOT__copper_pos__v0 
                    = (0x0000001fU & (IData)(vlSelfRef.test_copper__DOT__dut__DOT__copper_load_idx));
                __VdlySet__test_copper__DOT__dut__DOT__copper_pos__v0 = 1U;
                __VdlyVal__test_copper__DOT__dut__DOT__copper_reg__v0 
                    = vlSelfRef.test_copper__DOT__dut__DOT__copper_list_reg
                    [(((IData)(vlSelfRef.test_copper__DOT__dut__DOT__copper_load_src) 
                       << 5U) | (0x0000001fU & (IData)(vlSelfRef.test_copper__DOT__dut__DOT__copper_load_idx)))];
                __VdlyDim0__test_copper__DOT__dut__DOT__copper_reg__v0 
                    = (0x0000001fU & (IData)(vlSelfRef.test_copper__DOT__dut__DOT__copper_load_idx));
                __VdlyVal__test_copper__DOT__dut__DOT__copper_val__v0 
                    = vlSelfRef.test_copper__DOT__dut__DOT__copper_list_val
                    [(((IData)(vlSelfRef.test_copper__DOT__dut__DOT__copper_load_src) 
                       << 5U) | (0x0000001fU & (IData)(vlSelfRef.test_copper__DOT__dut__DOT__copper_load_idx)))];
                __VdlyDim0__test_copper__DOT__dut__DOT__copper_val__v0 
                    = (0x0000001fU & (IData)(vlSelfRef.test_copper__DOT__dut__DOT__copper_load_idx));
                vlSelfRef.test_copper__DOT__dut__DOT__copper_load_idx 
                    = (0x000001ffU & ((IData)(1U) + (IData)(vlSelfRef.test_copper__DOT__dut__DOT__copper_load_idx)));
            } else {
                __Vdly__test_copper__DOT__dut__DOT__copper_count 
                    = vlSelfRef.test_copper__DOT__dut__DOT__copper_list_count
                    [vlSelfRef.test_copper__DOT__dut__DOT__copper_load_src];
                __Vdly__test_copper__DOT__dut__DOT__copper_loading = 0U;
            }
        }
        if (vlSelfRef.test_copper__DOT__cpu_we) {
            if (vlSelfRef.test_copper__DOT__dut__DOT__vgc_reg_sel) {
                if (((((((((0U == (0x0000001fU & (IData)(vlSelfRef.test_copper__DOT__cpu_addr))) 
                           | (1U == (0x0000001fU & (IData)(vlSelfRef.test_copper__DOT__cpu_addr)))) 
                          | (2U == (0x0000001fU & (IData)(vlSelfRef.test_copper__DOT__cpu_addr)))) 
                         | (3U == (0x0000001fU & (IData)(vlSelfRef.test_copper__DOT__cpu_addr)))) 
                        | (4U == (0x0000001fU & (IData)(vlSelfRef.test_copper__DOT__cpu_addr)))) 
                       | (5U == (0x0000001fU & (IData)(vlSelfRef.test_copper__DOT__cpu_addr)))) 
                      | (6U == (0x0000001fU & (IData)(vlSelfRef.test_copper__DOT__cpu_addr)))) 
                     | (7U == (0x0000001fU & (IData)(vlSelfRef.test_copper__DOT__cpu_addr))))) {
                    if ((0U != (0x0000001fU & (IData)(vlSelfRef.test_copper__DOT__cpu_addr)))) {
                        if ((1U == (0x0000001fU & (IData)(vlSelfRef.test_copper__DOT__cpu_addr)))) {
                            vlSelfRef.test_copper__DOT__dut__DOT__bg_color 
                                = (0x0000000fU & (IData)(vlSelfRef.test_copper__DOT__cpu_wdata));
                        } else if ((2U == (0x0000001fU 
                                           & (IData)(vlSelfRef.test_copper__DOT__cpu_addr)))) {
                            __Vdly__test_copper__DOT__dut__DOT__fg_color 
                                = (0x0000000fU & (IData)(vlSelfRef.test_copper__DOT__cpu_wdata));
                        } else if ((3U == (0x0000001fU 
                                           & (IData)(vlSelfRef.test_copper__DOT__cpu_addr)))) {
                            __Vdly__test_copper__DOT__dut__DOT__cursor_x 
                                = (0x0000007fU & (IData)(vlSelfRef.test_copper__DOT__cpu_wdata));
                        } else if ((4U == (0x0000001fU 
                                           & (IData)(vlSelfRef.test_copper__DOT__cpu_addr)))) {
                            __Vdly__test_copper__DOT__dut__DOT__cursor_y 
                                = (0x0000001fU & (IData)(vlSelfRef.test_copper__DOT__cpu_wdata));
                        }
                    }
                } else if (((((((((8U == (0x0000001fU 
                                          & (IData)(vlSelfRef.test_copper__DOT__cpu_addr))) 
                                  | (0x0aU == (0x0000001fU 
                                               & (IData)(vlSelfRef.test_copper__DOT__cpu_addr)))) 
                                 | (0x0bU == (0x0000001fU 
                                              & (IData)(vlSelfRef.test_copper__DOT__cpu_addr)))) 
                                | (0x0cU == (0x0000001fU 
                                             & (IData)(vlSelfRef.test_copper__DOT__cpu_addr)))) 
                               | (0x0dU == (0x0000001fU 
                                            & (IData)(vlSelfRef.test_copper__DOT__cpu_addr)))) 
                              | (0x1fU == (0x0000001fU 
                                           & (IData)(vlSelfRef.test_copper__DOT__cpu_addr)))) 
                             | (0x0eU == (0x0000001fU 
                                          & (IData)(vlSelfRef.test_copper__DOT__cpu_addr)))) 
                            | (0x10U == (0x0000001fU 
                                         & (IData)(vlSelfRef.test_copper__DOT__cpu_addr))))) {
                    if ((8U == (0x0000001fU & (IData)(vlSelfRef.test_copper__DOT__cpu_addr)))) {
                        __Vdly__test_copper__DOT__dut__DOT__gfx_color 
                            = (0x0000000fU & (IData)(vlSelfRef.test_copper__DOT__cpu_wdata));
                    } else if ((0x0aU != (0x0000001fU 
                                          & (IData)(vlSelfRef.test_copper__DOT__cpu_addr)))) {
                        if ((0x0bU != (0x0000001fU 
                                       & (IData)(vlSelfRef.test_copper__DOT__cpu_addr)))) {
                            if ((0x0cU != (0x0000001fU 
                                           & (IData)(vlSelfRef.test_copper__DOT__cpu_addr)))) {
                                if ((0x0dU != (0x0000001fU 
                                               & (IData)(vlSelfRef.test_copper__DOT__cpu_addr)))) {
                                    if ((0x1fU != (0x0000001fU 
                                                   & (IData)(vlSelfRef.test_copper__DOT__cpu_addr)))) {
                                        if ((0x0eU 
                                             == (0x0000001fU 
                                                 & (IData)(vlSelfRef.test_copper__DOT__cpu_addr)))) {
                                            if ((8U 
                                                 == (IData)(vlSelfRef.test_copper__DOT__cpu_wdata))) {
                                                if (
                                                    (0U 
                                                     < (IData)(vlSelfRef.test_copper__DOT__dut__DOT__cursor_x))) {
                                                    __Vdly__test_copper__DOT__dut__DOT__cursor_x 
                                                        = 
                                                        (0x0000007fU 
                                                         & ((IData)(vlSelfRef.test_copper__DOT__dut__DOT__cursor_x) 
                                                            - (IData)(1U)));
                                                }
                                                vlSelfRef.test_copper__DOT__dut__DOT____Vlvbound_hb8130fd5__0 = 0x20U;
                                                if (VL_LIKELY((
                                                               (0x07cfU 
                                                                >= 
                                                                ([&]() {
                                                                        __Vfunc_test_copper__DOT__dut__DOT__screen_addr__114__row 
                                                                            = vlSelfRef.test_copper__DOT__dut__DOT__cursor_y;
                                                                        __Vfunc_test_copper__DOT__dut__DOT__screen_addr__114__col 
                                                                            = 
                                                                            ((0U 
                                                                              < (IData)(vlSelfRef.test_copper__DOT__dut__DOT__cursor_x))
                                                                              ? 
                                                                             (0x0000007fU 
                                                                              & ((IData)(vlSelfRef.test_copper__DOT__dut__DOT__cursor_x) 
                                                                                - (IData)(1U)))
                                                                              : 0U);
                                                                        __Vfunc_test_copper__DOT__dut__DOT__screen_addr__114__rr 
                                                                            = 
                                                                            (0x0000001fU 
                                                                             & ((IData)(__Vfunc_test_copper__DOT__dut__DOT__screen_addr__114__row) 
                                                                                + (IData)(vlSelfRef.test_copper__DOT__dut__DOT__scroll_offset)));
                                                                        if (
                                                                            (0x19U 
                                                                             <= (IData)(__Vfunc_test_copper__DOT__dut__DOT__screen_addr__114__rr))) {
                                                                            __Vfunc_test_copper__DOT__dut__DOT__screen_addr__114__rr 
                                                                                = 
                                                                                (0x0000001fU 
                                                                                & ((IData)(__Vfunc_test_copper__DOT__dut__DOT__screen_addr__114__rr) 
                                                                                - (IData)(0x19U)));
                                                                        }
                                                                        __Vfunc_test_copper__DOT__dut__DOT__screen_addr__114__Vfuncout 
                                                                            = 
                                                                            (0x000007ffU 
                                                                             & (((IData)(0x00000050U) 
                                                                                * (IData)(__Vfunc_test_copper__DOT__dut__DOT__screen_addr__114__rr)) 
                                                                                + (IData)(__Vfunc_test_copper__DOT__dut__DOT__screen_addr__114__col)));
                                                                    }(), (IData)(__Vfunc_test_copper__DOT__dut__DOT__screen_addr__114__Vfuncout)))))) {
                                                    __Vfunc_test_copper__DOT__dut__DOT__screen_addr__115__row 
                                                        = vlSelfRef.test_copper__DOT__dut__DOT__cursor_y;
                                                    __Vfunc_test_copper__DOT__dut__DOT__screen_addr__115__col 
                                                        = 
                                                        ((0U 
                                                          < (IData)(vlSelfRef.test_copper__DOT__dut__DOT__cursor_x))
                                                          ? 
                                                         (0x0000007fU 
                                                          & ((IData)(vlSelfRef.test_copper__DOT__dut__DOT__cursor_x) 
                                                             - (IData)(1U)))
                                                          : 0U);
                                                    __Vfunc_test_copper__DOT__dut__DOT__screen_addr__115__rr 
                                                        = 
                                                        (0x0000001fU 
                                                         & ((IData)(__Vfunc_test_copper__DOT__dut__DOT__screen_addr__115__row) 
                                                            + (IData)(vlSelfRef.test_copper__DOT__dut__DOT__scroll_offset)));
                                                    if (
                                                        (0x19U 
                                                         <= (IData)(__Vfunc_test_copper__DOT__dut__DOT__screen_addr__115__rr))) {
                                                        __Vfunc_test_copper__DOT__dut__DOT__screen_addr__115__rr 
                                                            = 
                                                            (0x0000001fU 
                                                             & ((IData)(__Vfunc_test_copper__DOT__dut__DOT__screen_addr__115__rr) 
                                                                - (IData)(0x19U)));
                                                    }
                                                    __Vfunc_test_copper__DOT__dut__DOT__screen_addr__115__Vfuncout 
                                                        = 
                                                        (0x000007ffU 
                                                         & (((IData)(0x00000050U) 
                                                             * (IData)(__Vfunc_test_copper__DOT__dut__DOT__screen_addr__115__rr)) 
                                                            + (IData)(__Vfunc_test_copper__DOT__dut__DOT__screen_addr__115__col)));
                                                    __VdlyVal__test_copper__DOT__dut__DOT__char_ram__v3 
                                                        = vlSelfRef.test_copper__DOT__dut__DOT____Vlvbound_hb8130fd5__0;
                                                    __VdlyDim0__test_copper__DOT__dut__DOT__char_ram__v3 
                                                        = __Vfunc_test_copper__DOT__dut__DOT__screen_addr__115__Vfuncout;
                                                    vlSelfRef.__VdlyCommitQueuetest_copper__DOT__dut__DOT__char_ram.enqueue(__VdlyVal__test_copper__DOT__dut__DOT__char_ram__v3, (IData)(__VdlyDim0__test_copper__DOT__dut__DOT__char_ram__v3));
                                                }
                                            } else if (
                                                       (0x0aU 
                                                        == (IData)(vlSelfRef.test_copper__DOT__cpu_wdata))) {
                                                if (
                                                    (0x18U 
                                                     <= (IData)(vlSelfRef.test_copper__DOT__dut__DOT__cursor_y))) {
                                                    __Vdly__test_copper__DOT__dut__DOT__scroll_offset 
                                                        = 
                                                        ((0x18U 
                                                          <= (IData)(vlSelfRef.test_copper__DOT__dut__DOT__scroll_offset))
                                                          ? 0U
                                                          : 
                                                         (0x0000001fU 
                                                          & ((IData)(1U) 
                                                             + (IData)(vlSelfRef.test_copper__DOT__dut__DOT__scroll_offset))));
                                                    __Vdly__test_copper__DOT__dut__DOT__scroll_pending = 1U;
                                                    __Vdly__test_copper__DOT__dut__DOT__scroll_col = 0U;
                                                } else {
                                                    __Vdly__test_copper__DOT__dut__DOT__cursor_y 
                                                        = 
                                                        (0x0000001fU 
                                                         & ((IData)(1U) 
                                                            + (IData)(vlSelfRef.test_copper__DOT__dut__DOT__cursor_y)));
                                                }
                                            } else if (
                                                       (0x0cU 
                                                        == (IData)(vlSelfRef.test_copper__DOT__cpu_wdata))) {
                                                __Vdly__test_copper__DOT__dut__DOT__cmd_cy = 0U;
                                                __Vdly__test_copper__DOT__dut__DOT__cursor_x = 0U;
                                                __Vdly__test_copper__DOT__dut__DOT__cursor_y = 0U;
                                                __Vdly__test_copper__DOT__dut__DOT__scroll_offset = 0U;
                                                __Vdly__test_copper__DOT__dut__DOT__cmd_cx = 0U;
                                                __Vdly__test_copper__DOT__dut__DOT__cmd_busy = 1U;
                                                vlSelfRef.test_copper__DOT__dut__DOT__cmd_op = 0xf0U;
                                            } else if (
                                                       (0x0dU 
                                                        == (IData)(vlSelfRef.test_copper__DOT__cpu_wdata))) {
                                                __Vdly__test_copper__DOT__dut__DOT__cursor_x = 0U;
                                            } else if (
                                                       (0x13U 
                                                        == (IData)(vlSelfRef.test_copper__DOT__cpu_wdata))) {
                                                __Vdly__test_copper__DOT__dut__DOT__cursor_x = 0U;
                                                __Vdly__test_copper__DOT__dut__DOT__cursor_y = 0U;
                                            } else if (
                                                       (0x20U 
                                                        <= (IData)(vlSelfRef.test_copper__DOT__cpu_wdata))) {
                                                vlSelfRef.test_copper__DOT__dut__DOT____Vlvbound_h2b480197__0 
                                                    = vlSelfRef.test_copper__DOT__cpu_wdata;
                                                if (VL_LIKELY((
                                                               (0x07cfU 
                                                                >= 
                                                                ([&]() {
                                                                        __Vfunc_test_copper__DOT__dut__DOT__screen_addr__116__row 
                                                                            = vlSelfRef.test_copper__DOT__dut__DOT__cursor_y;
                                                                        __Vfunc_test_copper__DOT__dut__DOT__screen_addr__116__col 
                                                                            = vlSelfRef.test_copper__DOT__dut__DOT__cursor_x;
                                                                        __Vfunc_test_copper__DOT__dut__DOT__screen_addr__116__rr 
                                                                            = 
                                                                            (0x0000001fU 
                                                                             & ((IData)(__Vfunc_test_copper__DOT__dut__DOT__screen_addr__116__row) 
                                                                                + (IData)(vlSelfRef.test_copper__DOT__dut__DOT__scroll_offset)));
                                                                        if (
                                                                            (0x19U 
                                                                             <= (IData)(__Vfunc_test_copper__DOT__dut__DOT__screen_addr__116__rr))) {
                                                                            __Vfunc_test_copper__DOT__dut__DOT__screen_addr__116__rr 
                                                                                = 
                                                                                (0x0000001fU 
                                                                                & ((IData)(__Vfunc_test_copper__DOT__dut__DOT__screen_addr__116__rr) 
                                                                                - (IData)(0x19U)));
                                                                        }
                                                                        __Vfunc_test_copper__DOT__dut__DOT__screen_addr__116__Vfuncout 
                                                                            = 
                                                                            (0x000007ffU 
                                                                             & (((IData)(0x00000050U) 
                                                                                * (IData)(__Vfunc_test_copper__DOT__dut__DOT__screen_addr__116__rr)) 
                                                                                + (IData)(__Vfunc_test_copper__DOT__dut__DOT__screen_addr__116__col)));
                                                                    }(), (IData)(__Vfunc_test_copper__DOT__dut__DOT__screen_addr__116__Vfuncout)))))) {
                                                    __Vfunc_test_copper__DOT__dut__DOT__screen_addr__117__row 
                                                        = vlSelfRef.test_copper__DOT__dut__DOT__cursor_y;
                                                    __Vfunc_test_copper__DOT__dut__DOT__screen_addr__117__col 
                                                        = vlSelfRef.test_copper__DOT__dut__DOT__cursor_x;
                                                    __Vfunc_test_copper__DOT__dut__DOT__screen_addr__117__rr 
                                                        = 
                                                        (0x0000001fU 
                                                         & ((IData)(__Vfunc_test_copper__DOT__dut__DOT__screen_addr__117__row) 
                                                            + (IData)(vlSelfRef.test_copper__DOT__dut__DOT__scroll_offset)));
                                                    if (
                                                        (0x19U 
                                                         <= (IData)(__Vfunc_test_copper__DOT__dut__DOT__screen_addr__117__rr))) {
                                                        __Vfunc_test_copper__DOT__dut__DOT__screen_addr__117__rr 
                                                            = 
                                                            (0x0000001fU 
                                                             & ((IData)(__Vfunc_test_copper__DOT__dut__DOT__screen_addr__117__rr) 
                                                                - (IData)(0x19U)));
                                                    }
                                                    __Vfunc_test_copper__DOT__dut__DOT__screen_addr__117__Vfuncout 
                                                        = 
                                                        (0x000007ffU 
                                                         & (((IData)(0x00000050U) 
                                                             * (IData)(__Vfunc_test_copper__DOT__dut__DOT__screen_addr__117__rr)) 
                                                            + (IData)(__Vfunc_test_copper__DOT__dut__DOT__screen_addr__117__col)));
                                                    __VdlyVal__test_copper__DOT__dut__DOT__char_ram__v4 
                                                        = vlSelfRef.test_copper__DOT__dut__DOT____Vlvbound_h2b480197__0;
                                                    __VdlyDim0__test_copper__DOT__dut__DOT__char_ram__v4 
                                                        = __Vfunc_test_copper__DOT__dut__DOT__screen_addr__117__Vfuncout;
                                                    vlSelfRef.__VdlyCommitQueuetest_copper__DOT__dut__DOT__char_ram.enqueue(__VdlyVal__test_copper__DOT__dut__DOT__char_ram__v4, (IData)(__VdlyDim0__test_copper__DOT__dut__DOT__char_ram__v4));
                                                }
                                                vlSelfRef.test_copper__DOT__dut__DOT____Vlvbound_h5d6d955d__0 
                                                    = vlSelfRef.test_copper__DOT__dut__DOT__fg_color;
                                                if (VL_LIKELY((
                                                               (0x07cfU 
                                                                >= 
                                                                ([&]() {
                                                                        __Vfunc_test_copper__DOT__dut__DOT__screen_addr__118__row 
                                                                            = vlSelfRef.test_copper__DOT__dut__DOT__cursor_y;
                                                                        __Vfunc_test_copper__DOT__dut__DOT__screen_addr__118__col 
                                                                            = vlSelfRef.test_copper__DOT__dut__DOT__cursor_x;
                                                                        __Vfunc_test_copper__DOT__dut__DOT__screen_addr__118__rr 
                                                                            = 
                                                                            (0x0000001fU 
                                                                             & ((IData)(__Vfunc_test_copper__DOT__dut__DOT__screen_addr__118__row) 
                                                                                + (IData)(vlSelfRef.test_copper__DOT__dut__DOT__scroll_offset)));
                                                                        if (
                                                                            (0x19U 
                                                                             <= (IData)(__Vfunc_test_copper__DOT__dut__DOT__screen_addr__118__rr))) {
                                                                            __Vfunc_test_copper__DOT__dut__DOT__screen_addr__118__rr 
                                                                                = 
                                                                                (0x0000001fU 
                                                                                & ((IData)(__Vfunc_test_copper__DOT__dut__DOT__screen_addr__118__rr) 
                                                                                - (IData)(0x19U)));
                                                                        }
                                                                        __Vfunc_test_copper__DOT__dut__DOT__screen_addr__118__Vfuncout 
                                                                            = 
                                                                            (0x000007ffU 
                                                                             & (((IData)(0x00000050U) 
                                                                                * (IData)(__Vfunc_test_copper__DOT__dut__DOT__screen_addr__118__rr)) 
                                                                                + (IData)(__Vfunc_test_copper__DOT__dut__DOT__screen_addr__118__col)));
                                                                    }(), (IData)(__Vfunc_test_copper__DOT__dut__DOT__screen_addr__118__Vfuncout)))))) {
                                                    __Vfunc_test_copper__DOT__dut__DOT__screen_addr__119__row 
                                                        = vlSelfRef.test_copper__DOT__dut__DOT__cursor_y;
                                                    __Vfunc_test_copper__DOT__dut__DOT__screen_addr__119__col 
                                                        = vlSelfRef.test_copper__DOT__dut__DOT__cursor_x;
                                                    __Vfunc_test_copper__DOT__dut__DOT__screen_addr__119__rr 
                                                        = 
                                                        (0x0000001fU 
                                                         & ((IData)(__Vfunc_test_copper__DOT__dut__DOT__screen_addr__119__row) 
                                                            + (IData)(vlSelfRef.test_copper__DOT__dut__DOT__scroll_offset)));
                                                    if (
                                                        (0x19U 
                                                         <= (IData)(__Vfunc_test_copper__DOT__dut__DOT__screen_addr__119__rr))) {
                                                        __Vfunc_test_copper__DOT__dut__DOT__screen_addr__119__rr 
                                                            = 
                                                            (0x0000001fU 
                                                             & ((IData)(__Vfunc_test_copper__DOT__dut__DOT__screen_addr__119__rr) 
                                                                - (IData)(0x19U)));
                                                    }
                                                    __Vfunc_test_copper__DOT__dut__DOT__screen_addr__119__Vfuncout 
                                                        = 
                                                        (0x000007ffU 
                                                         & (((IData)(0x00000050U) 
                                                             * (IData)(__Vfunc_test_copper__DOT__dut__DOT__screen_addr__119__rr)) 
                                                            + (IData)(__Vfunc_test_copper__DOT__dut__DOT__screen_addr__119__col)));
                                                    __VdlyVal__test_copper__DOT__dut__DOT__color_ram__v3 
                                                        = vlSelfRef.test_copper__DOT__dut__DOT____Vlvbound_h5d6d955d__0;
                                                    __VdlyDim0__test_copper__DOT__dut__DOT__color_ram__v3 
                                                        = __Vfunc_test_copper__DOT__dut__DOT__screen_addr__119__Vfuncout;
                                                    vlSelfRef.__VdlyCommitQueuetest_copper__DOT__dut__DOT__color_ram.enqueue(__VdlyVal__test_copper__DOT__dut__DOT__color_ram__v3, (IData)(__VdlyDim0__test_copper__DOT__dut__DOT__color_ram__v3));
                                                }
                                                if (
                                                    (0x4fU 
                                                     <= (IData)(vlSelfRef.test_copper__DOT__dut__DOT__cursor_x))) {
                                                    __Vdly__test_copper__DOT__dut__DOT__cursor_x = 0U;
                                                    if (
                                                        (0x18U 
                                                         <= (IData)(vlSelfRef.test_copper__DOT__dut__DOT__cursor_y))) {
                                                        __Vdly__test_copper__DOT__dut__DOT__scroll_offset 
                                                            = 
                                                            ((0x18U 
                                                              <= (IData)(vlSelfRef.test_copper__DOT__dut__DOT__scroll_offset))
                                                              ? 0U
                                                              : 
                                                             (0x0000001fU 
                                                              & ((IData)(1U) 
                                                                 + (IData)(vlSelfRef.test_copper__DOT__dut__DOT__scroll_offset))));
                                                        __Vdly__test_copper__DOT__dut__DOT__scroll_pending = 1U;
                                                        __Vdly__test_copper__DOT__dut__DOT__scroll_col = 0U;
                                                    } else {
                                                        __Vdly__test_copper__DOT__dut__DOT__cursor_y 
                                                            = 
                                                            (0x0000001fU 
                                                             & ((IData)(1U) 
                                                                + (IData)(vlSelfRef.test_copper__DOT__dut__DOT__cursor_y)));
                                                    }
                                                } else {
                                                    __Vdly__test_copper__DOT__dut__DOT__cursor_x 
                                                        = 
                                                        (0x0000007fU 
                                                         & ((IData)(1U) 
                                                            + (IData)(vlSelfRef.test_copper__DOT__dut__DOT__cursor_x)));
                                                }
                                            }
                                        } else if (
                                                   (1U 
                                                    & (~ (IData)(vlSelfRef.test_copper__DOT__dut__DOT__cmd_busy)))) {
                                            __Vdly__test_copper__DOT__dut__DOT__cmd_x 
                                                = (
                                                   (0x00000300U 
                                                    & (vlSelfRef.test_copper__DOT__dut__DOT__regs[18U] 
                                                       << 8U)) 
                                                   | vlSelfRef.test_copper__DOT__dut__DOT__regs[17U]);
                                            __Vdly__test_copper__DOT__dut__DOT__cmd_y 
                                                = (
                                                   (0x00000300U 
                                                    & (vlSelfRef.test_copper__DOT__dut__DOT__regs[20U] 
                                                       << 8U)) 
                                                   | vlSelfRef.test_copper__DOT__dut__DOT__regs[19U]);
                                            vlSelfRef.test_copper__DOT__dut__DOT__cmd_x2 
                                                = (
                                                   (0x00000300U 
                                                    & (vlSelfRef.test_copper__DOT__dut__DOT__regs[22U] 
                                                       << 8U)) 
                                                   | vlSelfRef.test_copper__DOT__dut__DOT__regs[21U]);
                                            vlSelfRef.test_copper__DOT__dut__DOT__cmd_y2 
                                                = (
                                                   (0x00000300U 
                                                    & (vlSelfRef.test_copper__DOT__dut__DOT__regs[24U] 
                                                       << 8U)) 
                                                   | vlSelfRef.test_copper__DOT__dut__DOT__regs[23U]);
                                            if ((1U 
                                                 & (~ 
                                                    ((IData)(vlSelfRef.test_copper__DOT__cpu_wdata) 
                                                     >> 7U)))) {
                                                if (
                                                    (1U 
                                                     & (~ 
                                                        ((IData)(vlSelfRef.test_copper__DOT__cpu_wdata) 
                                                         >> 6U)))) {
                                                    if (
                                                        (0x00000020U 
                                                         & (IData)(vlSelfRef.test_copper__DOT__cpu_wdata))) {
                                                        if (
                                                            (1U 
                                                             & (~ 
                                                                ((IData)(vlSelfRef.test_copper__DOT__cpu_wdata) 
                                                                 >> 4U)))) {
                                                            if (
                                                                (1U 
                                                                 & (~ 
                                                                    ((IData)(vlSelfRef.test_copper__DOT__cpu_wdata) 
                                                                     >> 3U)))) {
                                                                if (
                                                                    (1U 
                                                                     & (~ 
                                                                        ((IData)(vlSelfRef.test_copper__DOT__cpu_wdata) 
                                                                         >> 2U)))) {
                                                                    if (
                                                                        (2U 
                                                                         & (IData)(vlSelfRef.test_copper__DOT__cpu_wdata))) {
                                                                        if (
                                                                            (1U 
                                                                             & (~ (IData)(vlSelfRef.test_copper__DOT__cpu_wdata)))) {
                                                                            __Vdly__test_copper__DOT__dut__DOT__copper_target_list 
                                                                                = vlSelfRef.test_copper__DOT__dut__DOT__copper_active_list;
                                                                        }
                                                                    } else if (
                                                                               (1U 
                                                                                & (IData)(vlSelfRef.test_copper__DOT__cpu_wdata))) {
                                                                        __Vdly__test_copper__DOT__dut__DOT__copper_pending_list 
                                                                            = 
                                                                            (0x0000007fU 
                                                                             & vlSelfRef.test_copper__DOT__dut__DOT__regs[17U]);
                                                                    } else {
                                                                        __Vdly__test_copper__DOT__dut__DOT__copper_target_list 
                                                                            = 
                                                                            (0x0000007fU 
                                                                             & vlSelfRef.test_copper__DOT__dut__DOT__regs[17U]);
                                                                    }
                                                                }
                                                            }
                                                        }
                                                    } else if (
                                                               (0x00000010U 
                                                                & (IData)(vlSelfRef.test_copper__DOT__cpu_wdata))) {
                                                        if (
                                                            (8U 
                                                             & (IData)(vlSelfRef.test_copper__DOT__cpu_wdata))) {
                                                            if (
                                                                (4U 
                                                                 & (IData)(vlSelfRef.test_copper__DOT__cpu_wdata))) {
                                                                if (
                                                                    (2U 
                                                                     & (IData)(vlSelfRef.test_copper__DOT__cpu_wdata))) {
                                                                    if (
                                                                        (1U 
                                                                         & (IData)(vlSelfRef.test_copper__DOT__cpu_wdata))) {
                                                                        vlSelfRef.test_copper__DOT__dut__DOT__unnamedblk17__DOT__i = 0U;
                                                                        __Vdly__test_copper__DOT__dut__DOT__cursor_x = 0U;
                                                                        __Vdly__test_copper__DOT__dut__DOT__cursor_y = 0U;
                                                                        __Vdly__test_copper__DOT__dut__DOT__fg_color = 1U;
                                                                        vlSelfRef.test_copper__DOT__dut__DOT__bg_color = 6U;
                                                                        __Vdly__test_copper__DOT__dut__DOT__gfx_color = 1U;
                                                                        __Vdly__test_copper__DOT__dut__DOT__scroll_offset = 0U;
                                                                        vlSelfRef.test_copper__DOT__dut__DOT__copper_enabled = 0U;
                                                                        __Vdly__test_copper__DOT__dut__DOT__copper_count = 0U;
                                                                        __Vdly__test_copper__DOT__dut__DOT__copper_index = 0U;
                                                                        __Vdly__test_copper__DOT__dut__DOT__copper_target_list = 0U;
                                                                        __Vdly__test_copper__DOT__dut__DOT__copper_active_list = 0U;
                                                                        __Vdly__test_copper__DOT__dut__DOT__copper_pending_list = 0U;
                                                                        __Vdly__test_copper__DOT__dut__DOT__copper_loading = 0U;
                                                                        while (
                                                                               VL_GTS_III(32, 0x00000080U, vlSelfRef.test_copper__DOT__dut__DOT__unnamedblk17__DOT__i)) {
                                                                            __VdlyDim0__test_copper__DOT__dut__DOT__copper_list_count__v0 
                                                                                = 
                                                                                (0x0000007fU 
                                                                                & vlSelfRef.test_copper__DOT__dut__DOT__unnamedblk17__DOT__i);
                                                                            vlSelfRef.__VdlyCommitQueuetest_copper__DOT__dut__DOT__copper_list_count.enqueue(0U, (IData)(__VdlyDim0__test_copper__DOT__dut__DOT__copper_list_count__v0));
                                                                            vlSelfRef.test_copper__DOT__dut__DOT__unnamedblk17__DOT__i 
                                                                                = 
                                                                                ((IData)(1U) 
                                                                                + vlSelfRef.test_copper__DOT__dut__DOT__unnamedblk17__DOT__i);
                                                                        }
                                                                    } else {
                                                                        vlSelfRef.test_copper__DOT__dut__DOT__copper_enabled = 0U;
                                                                    }
                                                                } else if (
                                                                           (1U 
                                                                            & (IData)(vlSelfRef.test_copper__DOT__cpu_wdata))) {
                                                                    vlSelfRef.test_copper__DOT__dut__DOT__copper_enabled = 1U;
                                                                } else {
                                                                    __VdlyDim0__test_copper__DOT__dut__DOT__copper_list_count__v1 
                                                                        = vlSelfRef.test_copper__DOT__dut__DOT__copper_target_list;
                                                                    vlSelfRef.__VdlyCommitQueuetest_copper__DOT__dut__DOT__copper_list_count.enqueue(0U, (IData)(__VdlyDim0__test_copper__DOT__dut__DOT__copper_list_count__v1));
                                                                    if (
                                                                        ((IData)(vlSelfRef.test_copper__DOT__dut__DOT__copper_target_list) 
                                                                         == (IData)(vlSelfRef.test_copper__DOT__dut__DOT__copper_active_list))) {
                                                                        __Vdly__test_copper__DOT__dut__DOT__copper_count = 0U;
                                                                        __Vdly__test_copper__DOT__dut__DOT__copper_index = 0U;
                                                                    }
                                                                }
                                                            } else if (
                                                                       (2U 
                                                                        & (IData)(vlSelfRef.test_copper__DOT__cpu_wdata))) {
                                                                if (
                                                                    (1U 
                                                                     & (IData)(vlSelfRef.test_copper__DOT__cpu_wdata))) {
                                                                    if (
                                                                        (0x0020U 
                                                                         > vlSelfRef.test_copper__DOT__dut__DOT__copper_list_count
                                                                         [vlSelfRef.test_copper__DOT__dut__DOT__copper_target_list])) {
                                                                        __VdlyVal__test_copper__DOT__dut__DOT__copper_list_pos__v0 
                                                                            = 
                                                                            (0x0001ffffU 
                                                                             & (((IData)(0x00000140U) 
                                                                                * vlSelfRef.test_copper__DOT__dut__DOT__regs[19U]) 
                                                                                + 
                                                                                ((0x00000100U 
                                                                                & (vlSelfRef.test_copper__DOT__dut__DOT__regs[18U] 
                                                                                << 8U)) 
                                                                                | vlSelfRef.test_copper__DOT__dut__DOT__regs[17U])));
                                                                        __VdlyDim0__test_copper__DOT__dut__DOT__copper_list_pos__v0 
                                                                            = 
                                                                            (((IData)(vlSelfRef.test_copper__DOT__dut__DOT__copper_target_list) 
                                                                              << 5U) 
                                                                             | (0x0000001fU 
                                                                                & vlSelfRef.test_copper__DOT__dut__DOT__copper_list_count
                                                                                [vlSelfRef.test_copper__DOT__dut__DOT__copper_target_list]));
                                                                        __VdlySet__test_copper__DOT__dut__DOT__copper_list_pos__v0 = 1U;
                                                                        __VdlyVal__test_copper__DOT__dut__DOT__copper_list_reg__v0 
                                                                            = vlSelfRef.test_copper__DOT__dut__DOT__regs[20U];
                                                                        __VdlyDim0__test_copper__DOT__dut__DOT__copper_list_reg__v0 
                                                                            = 
                                                                            (((IData)(vlSelfRef.test_copper__DOT__dut__DOT__copper_target_list) 
                                                                              << 5U) 
                                                                             | (0x0000001fU 
                                                                                & vlSelfRef.test_copper__DOT__dut__DOT__copper_list_count
                                                                                [vlSelfRef.test_copper__DOT__dut__DOT__copper_target_list]));
                                                                        __VdlyVal__test_copper__DOT__dut__DOT__copper_list_val__v0 
                                                                            = vlSelfRef.test_copper__DOT__dut__DOT__regs[22U];
                                                                        __VdlyDim0__test_copper__DOT__dut__DOT__copper_list_val__v0 
                                                                            = 
                                                                            (((IData)(vlSelfRef.test_copper__DOT__dut__DOT__copper_target_list) 
                                                                              << 5U) 
                                                                             | (0x0000001fU 
                                                                                & vlSelfRef.test_copper__DOT__dut__DOT__copper_list_count
                                                                                [vlSelfRef.test_copper__DOT__dut__DOT__copper_target_list]));
                                                                        __VdlyVal__test_copper__DOT__dut__DOT__copper_list_count__v2 
                                                                            = 
                                                                            (0x000001ffU 
                                                                             & ((IData)(1U) 
                                                                                + vlSelfRef.test_copper__DOT__dut__DOT__copper_list_count
                                                                                [vlSelfRef.test_copper__DOT__dut__DOT__copper_target_list]));
                                                                        __VdlyDim0__test_copper__DOT__dut__DOT__copper_list_count__v2 
                                                                            = vlSelfRef.test_copper__DOT__dut__DOT__copper_target_list;
                                                                        vlSelfRef.__VdlyCommitQueuetest_copper__DOT__dut__DOT__copper_list_count.enqueue(__VdlyVal__test_copper__DOT__dut__DOT__copper_list_count__v2, (IData)(__VdlyDim0__test_copper__DOT__dut__DOT__copper_list_count__v2));
                                                                        if (
                                                                            ((IData)(vlSelfRef.test_copper__DOT__dut__DOT__copper_target_list) 
                                                                             == (IData)(vlSelfRef.test_copper__DOT__dut__DOT__copper_active_list))) {
                                                                            __VdlyVal__test_copper__DOT__dut__DOT__copper_pos__v1 
                                                                                = 
                                                                                (0x0001ffffU 
                                                                                & (((IData)(0x00000140U) 
                                                                                * vlSelfRef.test_copper__DOT__dut__DOT__regs[19U]) 
                                                                                + 
                                                                                ((0x00000100U 
                                                                                & (vlSelfRef.test_copper__DOT__dut__DOT__regs[18U] 
                                                                                << 8U)) 
                                                                                | vlSelfRef.test_copper__DOT__dut__DOT__regs[17U])));
                                                                            __VdlyDim0__test_copper__DOT__dut__DOT__copper_pos__v1 
                                                                                = 
                                                                                (0x0000001fU 
                                                                                & (IData)(vlSelfRef.test_copper__DOT__dut__DOT__copper_count));
                                                                            __VdlySet__test_copper__DOT__dut__DOT__copper_pos__v1 = 1U;
                                                                            __VdlyVal__test_copper__DOT__dut__DOT__copper_reg__v1 
                                                                                = vlSelfRef.test_copper__DOT__dut__DOT__regs[20U];
                                                                            __VdlyDim0__test_copper__DOT__dut__DOT__copper_reg__v1 
                                                                                = 
                                                                                (0x0000001fU 
                                                                                & (IData)(vlSelfRef.test_copper__DOT__dut__DOT__copper_count));
                                                                            __VdlyVal__test_copper__DOT__dut__DOT__copper_val__v1 
                                                                                = vlSelfRef.test_copper__DOT__dut__DOT__regs[22U];
                                                                            __VdlyDim0__test_copper__DOT__dut__DOT__copper_val__v1 
                                                                                = 
                                                                                (0x0000001fU 
                                                                                & (IData)(vlSelfRef.test_copper__DOT__dut__DOT__copper_count));
                                                                            __Vdly__test_copper__DOT__dut__DOT__copper_count 
                                                                                = 
                                                                                (0x000001ffU 
                                                                                & ((IData)(1U) 
                                                                                + (IData)(vlSelfRef.test_copper__DOT__dut__DOT__copper_count)));
                                                                        }
                                                                    }
                                                                } else if (
                                                                           (0U 
                                                                            == vlSelfRef.test_copper__DOT__dut__DOT__regs[17U])) {
                                                                    vlSelfRef.test_copper__DOT__dut__DOT____Vlvbound_ha13cfb6b__0 
                                                                        = vlSelfRef.test_copper__DOT__dut__DOT__regs[20U];
                                                                    if (VL_LIKELY((
                                                                                (0x07cfU 
                                                                                >= 
                                                                                ((0x00000700U 
                                                                                & (vlSelfRef.test_copper__DOT__dut__DOT__regs[19U] 
                                                                                << 8U)) 
                                                                                | vlSelfRef.test_copper__DOT__dut__DOT__regs[18U]))))) {
                                                                        __VdlyVal__test_copper__DOT__dut__DOT__char_ram__v5 
                                                                            = vlSelfRef.test_copper__DOT__dut__DOT____Vlvbound_ha13cfb6b__0;
                                                                        __VdlyDim0__test_copper__DOT__dut__DOT__char_ram__v5 
                                                                            = 
                                                                            ((0x00000700U 
                                                                              & (vlSelfRef.test_copper__DOT__dut__DOT__regs[19U] 
                                                                                << 8U)) 
                                                                             | vlSelfRef.test_copper__DOT__dut__DOT__regs[18U]);
                                                                        vlSelfRef.__VdlyCommitQueuetest_copper__DOT__dut__DOT__char_ram.enqueue(__VdlyVal__test_copper__DOT__dut__DOT__char_ram__v5, (IData)(__VdlyDim0__test_copper__DOT__dut__DOT__char_ram__v5));
                                                                    }
                                                                } else if (
                                                                           (1U 
                                                                            == vlSelfRef.test_copper__DOT__dut__DOT__regs[17U])) {
                                                                    vlSelfRef.test_copper__DOT__dut__DOT____Vlvbound_hd4129f91__0 
                                                                        = vlSelfRef.test_copper__DOT__dut__DOT__regs[20U];
                                                                    if (VL_LIKELY((
                                                                                (0x07cfU 
                                                                                >= 
                                                                                ((0x00000700U 
                                                                                & (vlSelfRef.test_copper__DOT__dut__DOT__regs[19U] 
                                                                                << 8U)) 
                                                                                | vlSelfRef.test_copper__DOT__dut__DOT__regs[18U]))))) {
                                                                        __VdlyVal__test_copper__DOT__dut__DOT__color_ram__v4 
                                                                            = vlSelfRef.test_copper__DOT__dut__DOT____Vlvbound_hd4129f91__0;
                                                                        __VdlyDim0__test_copper__DOT__dut__DOT__color_ram__v4 
                                                                            = 
                                                                            ((0x00000700U 
                                                                              & (vlSelfRef.test_copper__DOT__dut__DOT__regs[19U] 
                                                                                << 8U)) 
                                                                             | vlSelfRef.test_copper__DOT__dut__DOT__regs[18U]);
                                                                        vlSelfRef.__VdlyCommitQueuetest_copper__DOT__dut__DOT__color_ram.enqueue(__VdlyVal__test_copper__DOT__dut__DOT__color_ram__v4, (IData)(__VdlyDim0__test_copper__DOT__dut__DOT__color_ram__v4));
                                                                    }
                                                                } else if (
                                                                           (2U 
                                                                            == vlSelfRef.test_copper__DOT__dut__DOT__regs[17U])) {
                                                                    vlSelfRef.test_copper__DOT__dut__DOT____Vlvbound_h45978041__0 
                                                                        = 
                                                                        (0x0000000fU 
                                                                         & vlSelfRef.test_copper__DOT__dut__DOT__regs[20U]);
                                                                    if (VL_LIKELY((
                                                                                (0xf9ffU 
                                                                                >= 
                                                                                ((vlSelfRef.test_copper__DOT__dut__DOT__regs[19U] 
                                                                                << 8U) 
                                                                                | vlSelfRef.test_copper__DOT__dut__DOT__regs[18U]))))) {
                                                                        __VdlyVal__test_copper__DOT__dut__DOT__gfx_ram__v15 
                                                                            = vlSelfRef.test_copper__DOT__dut__DOT____Vlvbound_h45978041__0;
                                                                        __VdlyDim0__test_copper__DOT__dut__DOT__gfx_ram__v15 
                                                                            = 
                                                                            ((vlSelfRef.test_copper__DOT__dut__DOT__regs[19U] 
                                                                              << 8U) 
                                                                             | vlSelfRef.test_copper__DOT__dut__DOT__regs[18U]);
                                                                        vlSelfRef.__VdlyCommitQueuetest_copper__DOT__dut__DOT__gfx_ram.enqueue(__VdlyVal__test_copper__DOT__dut__DOT__gfx_ram__v15, (IData)(__VdlyDim0__test_copper__DOT__dut__DOT__gfx_ram__v15));
                                                                    }
                                                                } else if (
                                                                           (3U 
                                                                            == vlSelfRef.test_copper__DOT__dut__DOT__regs[17U])) {
                                                                    __VdlyVal__test_copper__DOT__dut__DOT__sprite_shapes__v3 
                                                                        = vlSelfRef.test_copper__DOT__dut__DOT__regs[20U];
                                                                    __VdlyDim0__test_copper__DOT__dut__DOT__sprite_shapes__v3 
                                                                        = 
                                                                        ((0x00000700U 
                                                                          & (vlSelfRef.test_copper__DOT__dut__DOT__regs[19U] 
                                                                             << 8U)) 
                                                                         | vlSelfRef.test_copper__DOT__dut__DOT__regs[18U]);
                                                                    vlSelfRef.__VdlyCommitQueuetest_copper__DOT__dut__DOT__sprite_shapes.enqueue(__VdlyVal__test_copper__DOT__dut__DOT__sprite_shapes__v3, (IData)(__VdlyDim0__test_copper__DOT__dut__DOT__sprite_shapes__v3));
                                                                } else if (
                                                                           (4U 
                                                                            == vlSelfRef.test_copper__DOT__dut__DOT__regs[17U])) {
                                                                    __VdlyVal__test_copper__DOT__dut__DOT__tile_inst__DOT__tile_data__v0 
                                                                        = vlSelfRef.test_copper__DOT__dut__DOT__regs[20U];
                                                                    __VdlyDim0__test_copper__DOT__dut__DOT__tile_inst__DOT__tile_data__v0 
                                                                        = 
                                                                        ((0x00007f00U 
                                                                          & (vlSelfRef.test_copper__DOT__dut__DOT__regs[19U] 
                                                                             << 8U)) 
                                                                         | vlSelfRef.test_copper__DOT__dut__DOT__regs[18U]);
                                                                    __VdlySet__test_copper__DOT__dut__DOT__tile_inst__DOT__tile_data__v0 = 1U;
                                                                }
                                                            } else if (
                                                                       (1U 
                                                                        & (IData)(vlSelfRef.test_copper__DOT__cpu_wdata))) {
                                                                if (
                                                                    (0U 
                                                                     == vlSelfRef.test_copper__DOT__dut__DOT__regs[17U])) {
                                                                    __VdlyVal__test_copper__DOT__dut__DOT__regs__v32 
                                                                        = 
                                                                        ((0x07cfU 
                                                                          >= 
                                                                          ((0x00000700U 
                                                                            & (vlSelfRef.test_copper__DOT__dut__DOT__regs[19U] 
                                                                               << 8U)) 
                                                                           | vlSelfRef.test_copper__DOT__dut__DOT__regs[18U]))
                                                                          ? vlSelfRef.test_copper__DOT__dut__DOT__char_ram
                                                                         [
                                                                         ((0x00000700U 
                                                                           & (vlSelfRef.test_copper__DOT__dut__DOT__regs[19U] 
                                                                              << 8U)) 
                                                                          | vlSelfRef.test_copper__DOT__dut__DOT__regs[18U])]
                                                                          : 0U);
                                                                    __VdlySet__test_copper__DOT__dut__DOT__regs__v32 = 1U;
                                                                } else if (
                                                                           (1U 
                                                                            == vlSelfRef.test_copper__DOT__dut__DOT__regs[17U])) {
                                                                    __VdlyVal__test_copper__DOT__dut__DOT__regs__v33 
                                                                        = 
                                                                        ((0x07cfU 
                                                                          >= 
                                                                          ((0x00000700U 
                                                                            & (vlSelfRef.test_copper__DOT__dut__DOT__regs[19U] 
                                                                               << 8U)) 
                                                                           | vlSelfRef.test_copper__DOT__dut__DOT__regs[18U]))
                                                                          ? vlSelfRef.test_copper__DOT__dut__DOT__color_ram
                                                                         [
                                                                         ((0x00000700U 
                                                                           & (vlSelfRef.test_copper__DOT__dut__DOT__regs[19U] 
                                                                              << 8U)) 
                                                                          | vlSelfRef.test_copper__DOT__dut__DOT__regs[18U])]
                                                                          : 0U);
                                                                    __VdlySet__test_copper__DOT__dut__DOT__regs__v33 = 1U;
                                                                } else if (
                                                                           (2U 
                                                                            == vlSelfRef.test_copper__DOT__dut__DOT__regs[17U])) {
                                                                    __VdlyVal__test_copper__DOT__dut__DOT__regs__v34 
                                                                        = 
                                                                        ((0xf9ffU 
                                                                          >= 
                                                                          ((vlSelfRef.test_copper__DOT__dut__DOT__regs[19U] 
                                                                            << 8U) 
                                                                           | vlSelfRef.test_copper__DOT__dut__DOT__regs[18U]))
                                                                          ? vlSelfRef.test_copper__DOT__dut__DOT__gfx_ram
                                                                         [
                                                                         ((vlSelfRef.test_copper__DOT__dut__DOT__regs[19U] 
                                                                           << 8U) 
                                                                          | vlSelfRef.test_copper__DOT__dut__DOT__regs[18U])]
                                                                          : 0U);
                                                                    __VdlySet__test_copper__DOT__dut__DOT__regs__v34 = 1U;
                                                                } else if (
                                                                           (3U 
                                                                            == vlSelfRef.test_copper__DOT__dut__DOT__regs[17U])) {
                                                                    __VdlyVal__test_copper__DOT__dut__DOT__regs__v35 
                                                                        = vlSelfRef.test_copper__DOT__dut__DOT__sprite_shapes
                                                                        [
                                                                        ((0x00000700U 
                                                                          & (vlSelfRef.test_copper__DOT__dut__DOT__regs[19U] 
                                                                             << 8U)) 
                                                                         | vlSelfRef.test_copper__DOT__dut__DOT__regs[18U])];
                                                                    __VdlySet__test_copper__DOT__dut__DOT__regs__v35 = 1U;
                                                                } else if (
                                                                           (4U 
                                                                            == vlSelfRef.test_copper__DOT__dut__DOT__regs[17U])) {
                                                                    __VdlyVal__test_copper__DOT__dut__DOT__regs__v36 
                                                                        = vlSelfRef.test_copper__DOT__dut__DOT__tile_inst__DOT__tile_data
                                                                        [
                                                                        ((0x00007f00U 
                                                                          & (vlSelfRef.test_copper__DOT__dut__DOT__regs[19U] 
                                                                             << 8U)) 
                                                                         | vlSelfRef.test_copper__DOT__dut__DOT__regs[18U])];
                                                                    __VdlySet__test_copper__DOT__dut__DOT__regs__v36 = 1U;
                                                                }
                                                            } else if (
                                                                       (0x10U 
                                                                        > vlSelfRef.test_copper__DOT__dut__DOT__regs[17U])) {
                                                                __VdlyVal__test_copper__DOT__dut__DOT__spr_pri__v16 
                                                                    = 
                                                                    (3U 
                                                                     & vlSelfRef.test_copper__DOT__dut__DOT__regs[18U]);
                                                                __VdlyDim0__test_copper__DOT__dut__DOT__spr_pri__v16 
                                                                    = 
                                                                    (0x0000000fU 
                                                                     & vlSelfRef.test_copper__DOT__dut__DOT__regs[17U]);
                                                                __VdlySet__test_copper__DOT__dut__DOT__spr_pri__v16 = 1U;
                                                            }
                                                        } else if (
                                                                   (4U 
                                                                    & (IData)(vlSelfRef.test_copper__DOT__cpu_wdata))) {
                                                            if (
                                                                (2U 
                                                                 & (IData)(vlSelfRef.test_copper__DOT__cpu_wdata))) {
                                                                if (
                                                                    (1U 
                                                                     & (IData)(vlSelfRef.test_copper__DOT__cpu_wdata))) {
                                                                    if (
                                                                        (0x10U 
                                                                         > vlSelfRef.test_copper__DOT__dut__DOT__regs[17U])) {
                                                                        __VdlyVal__test_copper__DOT__dut__DOT__spr_flip_h__v16 
                                                                            = 
                                                                            (1U 
                                                                             & vlSelfRef.test_copper__DOT__dut__DOT__regs[18U]);
                                                                        __VdlyDim0__test_copper__DOT__dut__DOT__spr_flip_h__v16 
                                                                            = 
                                                                            (0x0000000fU 
                                                                             & vlSelfRef.test_copper__DOT__dut__DOT__regs[17U]);
                                                                        __VdlySet__test_copper__DOT__dut__DOT__spr_flip_h__v16 = 1U;
                                                                        __VdlyVal__test_copper__DOT__dut__DOT__spr_flip_v__v16 
                                                                            = 
                                                                            (1U 
                                                                             & (vlSelfRef.test_copper__DOT__dut__DOT__regs[18U] 
                                                                                >> 1U));
                                                                        __VdlyDim0__test_copper__DOT__dut__DOT__spr_flip_v__v16 
                                                                            = 
                                                                            (0x0000000fU 
                                                                             & vlSelfRef.test_copper__DOT__dut__DOT__regs[17U]);
                                                                    }
                                                                } else if (
                                                                           (0x10U 
                                                                            > vlSelfRef.test_copper__DOT__dut__DOT__regs[17U])) {
                                                                    __VdlyDim0__test_copper__DOT__dut__DOT__spr_enable__v16 
                                                                        = 
                                                                        (0x0000000fU 
                                                                         & vlSelfRef.test_copper__DOT__dut__DOT__regs[17U]);
                                                                    __VdlySet__test_copper__DOT__dut__DOT__spr_enable__v16 = 1U;
                                                                }
                                                            } else if (
                                                                       (1U 
                                                                        & (IData)(vlSelfRef.test_copper__DOT__cpu_wdata))) {
                                                                if (
                                                                    (0x10U 
                                                                     > vlSelfRef.test_copper__DOT__dut__DOT__regs[17U])) {
                                                                    __VdlyDim0__test_copper__DOT__dut__DOT__spr_enable__v17 
                                                                        = 
                                                                        (0x0000000fU 
                                                                         & vlSelfRef.test_copper__DOT__dut__DOT__regs[17U]);
                                                                    __VdlySet__test_copper__DOT__dut__DOT__spr_enable__v17 = 1U;
                                                                }
                                                            } else if (
                                                                       (0x10U 
                                                                        > vlSelfRef.test_copper__DOT__dut__DOT__regs[17U])) {
                                                                __VdlyVal__test_copper__DOT__dut__DOT__spr_x__v16 
                                                                    = 
                                                                    ((0x00000100U 
                                                                      & (vlSelfRef.test_copper__DOT__dut__DOT__regs[19U] 
                                                                         << 8U)) 
                                                                     | vlSelfRef.test_copper__DOT__dut__DOT__regs[18U]);
                                                                __VdlyDim0__test_copper__DOT__dut__DOT__spr_x__v16 
                                                                    = 
                                                                    (0x0000000fU 
                                                                     & vlSelfRef.test_copper__DOT__dut__DOT__regs[17U]);
                                                                __VdlySet__test_copper__DOT__dut__DOT__spr_x__v16 = 1U;
                                                                __VdlyVal__test_copper__DOT__dut__DOT__spr_y__v16 
                                                                    = vlSelfRef.test_copper__DOT__dut__DOT__regs[20U];
                                                                __VdlyDim0__test_copper__DOT__dut__DOT__spr_y__v16 
                                                                    = 
                                                                    (0x0000000fU 
                                                                     & vlSelfRef.test_copper__DOT__dut__DOT__regs[17U]);
                                                            }
                                                        } else if (
                                                                   (2U 
                                                                    & (IData)(vlSelfRef.test_copper__DOT__cpu_wdata))) {
                                                            if (
                                                                (1U 
                                                                 & (IData)(vlSelfRef.test_copper__DOT__cpu_wdata))) {
                                                                if (
                                                                    ((0x10U 
                                                                      > vlSelfRef.test_copper__DOT__dut__DOT__regs[17U]) 
                                                                     & (0x10U 
                                                                        > vlSelfRef.test_copper__DOT__dut__DOT__regs[18U]))) {
                                                                    __Vdly__test_copper__DOT__dut__DOT__cmd_x 
                                                                        = 
                                                                        (0x0000000fU 
                                                                         & vlSelfRef.test_copper__DOT__dut__DOT__regs[17U]);
                                                                    __Vdly__test_copper__DOT__dut__DOT__cmd_cy = 0U;
                                                                    __Vdly__test_copper__DOT__dut__DOT__cmd_y 
                                                                        = 
                                                                        (0x0000000fU 
                                                                         & vlSelfRef.test_copper__DOT__dut__DOT__regs[18U]);
                                                                    __Vdly__test_copper__DOT__dut__DOT__cmd_cx = 0U;
                                                                    __Vdly__test_copper__DOT__dut__DOT__cmd_busy = 1U;
                                                                    vlSelfRef.test_copper__DOT__dut__DOT__cmd_op = 0x13U;
                                                                }
                                                            } else if (
                                                                       (0x10U 
                                                                        > vlSelfRef.test_copper__DOT__dut__DOT__regs[17U])) {
                                                                __Vdly__test_copper__DOT__dut__DOT__cmd_cy = 0U;
                                                                __Vdly__test_copper__DOT__dut__DOT__cmd_x 
                                                                    = 
                                                                    (0x0000001eU 
                                                                     & (vlSelfRef.test_copper__DOT__dut__DOT__regs[17U] 
                                                                        << 1U));
                                                                __Vdly__test_copper__DOT__dut__DOT__cmd_cx = 0U;
                                                                __Vdly__test_copper__DOT__dut__DOT__cmd_busy = 1U;
                                                                vlSelfRef.test_copper__DOT__dut__DOT__cmd_op = 0x12U;
                                                            }
                                                        } else if (
                                                                   (1U 
                                                                    & (IData)(vlSelfRef.test_copper__DOT__cpu_wdata))) {
                                                            if (
                                                                ((0x10U 
                                                                  > vlSelfRef.test_copper__DOT__dut__DOT__regs[17U]) 
                                                                 & (0x10U 
                                                                    > vlSelfRef.test_copper__DOT__dut__DOT__regs[18U]))) {
                                                                __VdlyVal__test_copper__DOT__dut__DOT__sprite_shapes__v4 
                                                                    = vlSelfRef.test_copper__DOT__dut__DOT__regs[19U];
                                                                __VdlyDim0__test_copper__DOT__dut__DOT__sprite_shapes__v4 
                                                                    = 
                                                                    ((0x00000780U 
                                                                      & (vlSelfRef.test_copper__DOT__dut__DOT__regs[17U] 
                                                                         << 7U)) 
                                                                     | (0x00000078U 
                                                                        & (vlSelfRef.test_copper__DOT__dut__DOT__regs[18U] 
                                                                           << 3U)));
                                                                vlSelfRef.__VdlyCommitQueuetest_copper__DOT__dut__DOT__sprite_shapes.enqueue(__VdlyVal__test_copper__DOT__dut__DOT__sprite_shapes__v4, (IData)(__VdlyDim0__test_copper__DOT__dut__DOT__sprite_shapes__v4));
                                                                __VdlyVal__test_copper__DOT__dut__DOT__sprite_shapes__v5 
                                                                    = vlSelfRef.test_copper__DOT__dut__DOT__regs[20U];
                                                                __VdlyDim0__test_copper__DOT__dut__DOT__sprite_shapes__v5 
                                                                    = 
                                                                    (1U 
                                                                     | ((0x00000780U 
                                                                         & (vlSelfRef.test_copper__DOT__dut__DOT__regs[17U] 
                                                                            << 7U)) 
                                                                        | (0x00000078U 
                                                                           & (vlSelfRef.test_copper__DOT__dut__DOT__regs[18U] 
                                                                              << 3U))));
                                                                vlSelfRef.__VdlyCommitQueuetest_copper__DOT__dut__DOT__sprite_shapes.enqueue(__VdlyVal__test_copper__DOT__dut__DOT__sprite_shapes__v5, (IData)(__VdlyDim0__test_copper__DOT__dut__DOT__sprite_shapes__v5));
                                                                __VdlyVal__test_copper__DOT__dut__DOT__sprite_shapes__v6 
                                                                    = vlSelfRef.test_copper__DOT__dut__DOT__regs[21U];
                                                                __VdlyDim0__test_copper__DOT__dut__DOT__sprite_shapes__v6 
                                                                    = 
                                                                    (2U 
                                                                     | ((0x00000780U 
                                                                         & (vlSelfRef.test_copper__DOT__dut__DOT__regs[17U] 
                                                                            << 7U)) 
                                                                        | (0x00000078U 
                                                                           & (vlSelfRef.test_copper__DOT__dut__DOT__regs[18U] 
                                                                              << 3U))));
                                                                vlSelfRef.__VdlyCommitQueuetest_copper__DOT__dut__DOT__sprite_shapes.enqueue(__VdlyVal__test_copper__DOT__dut__DOT__sprite_shapes__v6, (IData)(__VdlyDim0__test_copper__DOT__dut__DOT__sprite_shapes__v6));
                                                                __VdlyVal__test_copper__DOT__dut__DOT__sprite_shapes__v7 
                                                                    = vlSelfRef.test_copper__DOT__dut__DOT__regs[22U];
                                                                __VdlyDim0__test_copper__DOT__dut__DOT__sprite_shapes__v7 
                                                                    = 
                                                                    (3U 
                                                                     | ((0x00000780U 
                                                                         & (vlSelfRef.test_copper__DOT__dut__DOT__regs[17U] 
                                                                            << 7U)) 
                                                                        | (0x00000078U 
                                                                           & (vlSelfRef.test_copper__DOT__dut__DOT__regs[18U] 
                                                                              << 3U))));
                                                                vlSelfRef.__VdlyCommitQueuetest_copper__DOT__dut__DOT__sprite_shapes.enqueue(__VdlyVal__test_copper__DOT__dut__DOT__sprite_shapes__v7, (IData)(__VdlyDim0__test_copper__DOT__dut__DOT__sprite_shapes__v7));
                                                                __VdlyVal__test_copper__DOT__dut__DOT__sprite_shapes__v8 
                                                                    = vlSelfRef.test_copper__DOT__dut__DOT__regs[23U];
                                                                __VdlyDim0__test_copper__DOT__dut__DOT__sprite_shapes__v8 
                                                                    = 
                                                                    (4U 
                                                                     | ((0x00000780U 
                                                                         & (vlSelfRef.test_copper__DOT__dut__DOT__regs[17U] 
                                                                            << 7U)) 
                                                                        | (0x00000078U 
                                                                           & (vlSelfRef.test_copper__DOT__dut__DOT__regs[18U] 
                                                                              << 3U))));
                                                                vlSelfRef.__VdlyCommitQueuetest_copper__DOT__dut__DOT__sprite_shapes.enqueue(__VdlyVal__test_copper__DOT__dut__DOT__sprite_shapes__v8, (IData)(__VdlyDim0__test_copper__DOT__dut__DOT__sprite_shapes__v8));
                                                                __VdlyVal__test_copper__DOT__dut__DOT__sprite_shapes__v9 
                                                                    = vlSelfRef.test_copper__DOT__dut__DOT__regs[24U];
                                                                __VdlyDim0__test_copper__DOT__dut__DOT__sprite_shapes__v9 
                                                                    = 
                                                                    (5U 
                                                                     | ((0x00000780U 
                                                                         & (vlSelfRef.test_copper__DOT__dut__DOT__regs[17U] 
                                                                            << 7U)) 
                                                                        | (0x00000078U 
                                                                           & (vlSelfRef.test_copper__DOT__dut__DOT__regs[18U] 
                                                                              << 3U))));
                                                                vlSelfRef.__VdlyCommitQueuetest_copper__DOT__dut__DOT__sprite_shapes.enqueue(__VdlyVal__test_copper__DOT__dut__DOT__sprite_shapes__v9, (IData)(__VdlyDim0__test_copper__DOT__dut__DOT__sprite_shapes__v9));
                                                                __VdlyVal__test_copper__DOT__dut__DOT__sprite_shapes__v10 
                                                                    = vlSelfRef.test_copper__DOT__dut__DOT__regs[25U];
                                                                __VdlyDim0__test_copper__DOT__dut__DOT__sprite_shapes__v10 
                                                                    = 
                                                                    (6U 
                                                                     | ((0x00000780U 
                                                                         & (vlSelfRef.test_copper__DOT__dut__DOT__regs[17U] 
                                                                            << 7U)) 
                                                                        | (0x00000078U 
                                                                           & (vlSelfRef.test_copper__DOT__dut__DOT__regs[18U] 
                                                                              << 3U))));
                                                                vlSelfRef.__VdlyCommitQueuetest_copper__DOT__dut__DOT__sprite_shapes.enqueue(__VdlyVal__test_copper__DOT__dut__DOT__sprite_shapes__v10, (IData)(__VdlyDim0__test_copper__DOT__dut__DOT__sprite_shapes__v10));
                                                                __VdlyVal__test_copper__DOT__dut__DOT__sprite_shapes__v11 
                                                                    = vlSelfRef.test_copper__DOT__dut__DOT__regs[26U];
                                                                __VdlyDim0__test_copper__DOT__dut__DOT__sprite_shapes__v11 
                                                                    = 
                                                                    (7U 
                                                                     | ((0x00000780U 
                                                                         & (vlSelfRef.test_copper__DOT__dut__DOT__regs[17U] 
                                                                            << 7U)) 
                                                                        | (0x00000078U 
                                                                           & (vlSelfRef.test_copper__DOT__dut__DOT__regs[18U] 
                                                                              << 3U))));
                                                                vlSelfRef.__VdlyCommitQueuetest_copper__DOT__dut__DOT__sprite_shapes.enqueue(__VdlyVal__test_copper__DOT__dut__DOT__sprite_shapes__v11, (IData)(__VdlyDim0__test_copper__DOT__dut__DOT__sprite_shapes__v11));
                                                            }
                                                        } else if (
                                                                   (((0x10U 
                                                                      > vlSelfRef.test_copper__DOT__dut__DOT__regs[17U]) 
                                                                     & (0x10U 
                                                                        > vlSelfRef.test_copper__DOT__dut__DOT__regs[18U])) 
                                                                    & (0x10U 
                                                                       > vlSelfRef.test_copper__DOT__dut__DOT__regs[19U]))) {
                                                            __VdlyVal__test_copper__DOT__dut__DOT__sprite_shapes__v12 
                                                                = 
                                                                ((1U 
                                                                  & vlSelfRef.test_copper__DOT__dut__DOT__regs[18U])
                                                                  ? 
                                                                 ((0x000000f0U 
                                                                   & vlSelfRef.test_copper__DOT__dut__DOT__sprite_shapes
                                                                   [
                                                                   ((0x00000780U 
                                                                     & (vlSelfRef.test_copper__DOT__dut__DOT__regs[17U] 
                                                                        << 7U)) 
                                                                    | ((0x00000078U 
                                                                        & (vlSelfRef.test_copper__DOT__dut__DOT__regs[19U] 
                                                                           << 3U)) 
                                                                       | (7U 
                                                                          & (vlSelfRef.test_copper__DOT__dut__DOT__regs[18U] 
                                                                             >> 1U))))]) 
                                                                  | (0x0000000fU 
                                                                     & vlSelfRef.test_copper__DOT__dut__DOT__regs[20U]))
                                                                  : 
                                                                 ((0x000000f0U 
                                                                   & (vlSelfRef.test_copper__DOT__dut__DOT__regs[20U] 
                                                                      << 4U)) 
                                                                  | (0x0000000fU 
                                                                     & vlSelfRef.test_copper__DOT__dut__DOT__sprite_shapes
                                                                     [
                                                                     ((0x00000780U 
                                                                       & (vlSelfRef.test_copper__DOT__dut__DOT__regs[17U] 
                                                                          << 7U)) 
                                                                      | ((0x00000078U 
                                                                          & (vlSelfRef.test_copper__DOT__dut__DOT__regs[19U] 
                                                                             << 3U)) 
                                                                         | (7U 
                                                                            & (vlSelfRef.test_copper__DOT__dut__DOT__regs[18U] 
                                                                               >> 1U))))])));
                                                            __VdlyDim0__test_copper__DOT__dut__DOT__sprite_shapes__v12 
                                                                = 
                                                                ((0x00000780U 
                                                                  & (vlSelfRef.test_copper__DOT__dut__DOT__regs[17U] 
                                                                     << 7U)) 
                                                                 | ((0x00000078U 
                                                                     & (vlSelfRef.test_copper__DOT__dut__DOT__regs[19U] 
                                                                        << 3U)) 
                                                                    | (7U 
                                                                       & (vlSelfRef.test_copper__DOT__dut__DOT__regs[18U] 
                                                                          >> 1U))));
                                                            vlSelfRef.__VdlyCommitQueuetest_copper__DOT__dut__DOT__sprite_shapes.enqueue(__VdlyVal__test_copper__DOT__dut__DOT__sprite_shapes__v12, (IData)(__VdlyDim0__test_copper__DOT__dut__DOT__sprite_shapes__v12));
                                                        }
                                                    } else if (
                                                               (8U 
                                                                & (IData)(vlSelfRef.test_copper__DOT__cpu_wdata))) {
                                                        if (
                                                            (1U 
                                                             & (~ 
                                                                ((IData)(vlSelfRef.test_copper__DOT__cpu_wdata) 
                                                                 >> 2U)))) {
                                                            if (
                                                                (2U 
                                                                 & (IData)(vlSelfRef.test_copper__DOT__cpu_wdata))) {
                                                                if (
                                                                    (1U 
                                                                     & (~ (IData)(vlSelfRef.test_copper__DOT__cpu_wdata)))) {
                                                                    if (
                                                                        (0U 
                                                                         < (IData)(vlSelfRef.test_copper__DOT__dut__DOT__fio_name_len))) {
                                                                        vlSelfRef.test_copper__DOT__dut__DOT__gt_pen_x 
                                                                            = 
                                                                            ((0x00000100U 
                                                                              & (vlSelfRef.test_copper__DOT__dut__DOT__regs[18U] 
                                                                                << 8U)) 
                                                                             | vlSelfRef.test_copper__DOT__dut__DOT__regs[17U]);
                                                                        vlSelfRef.test_copper__DOT__dut__DOT__gt_start_y 
                                                                            = vlSelfRef.test_copper__DOT__dut__DOT__regs[19U];
                                                                        __Vdly__test_copper__DOT__dut__DOT__gt_char_idx = 0U;
                                                                        vlSelfRef.test_copper__DOT__dut__DOT__gt_char_len 
                                                                            = vlSelfRef.test_copper__DOT__dut__DOT__fio_name_len;
                                                                        __Vdly__test_copper__DOT__dut__DOT__gt_font_row = 0U;
                                                                        vlSelfRef.test_copper__DOT__dut__DOT__gt_font_col = 0U;
                                                                        vlSelfRef.test_copper__DOT__dut__DOT__gt_scale_x = 0U;
                                                                        vlSelfRef.test_copper__DOT__dut__DOT__gt_scale_y = 0U;
                                                                        vlSelfRef.test_copper__DOT__dut__DOT__gt_cur_char 
                                                                            = vlSelfRef.test_copper__DOT__dut__DOT__fio_name[0U];
                                                                        vlSelfRef.test_copper__DOT__dut__DOT__gt_font_byte 
                                                                            = vlSelfRef.test_copper__DOT__dut__DOT__font_rom
                                                                            [
                                                                            (vlSelfRef.test_copper__DOT__dut__DOT__fio_name[0U] 
                                                                             << 3U)];
                                                                        __Vdly__test_copper__DOT__dut__DOT__cmd_busy = 1U;
                                                                        vlSelfRef.test_copper__DOT__dut__DOT__cmd_op = 0x0aU;
                                                                        vlSelfRef.test_copper__DOT__dut__DOT__gt_scale 
                                                                            = 
                                                                            ((1U 
                                                                              > vlSelfRef.test_copper__DOT__dut__DOT__regs[22U])
                                                                              ? 1U
                                                                              : vlSelfRef.test_copper__DOT__dut__DOT__regs[22U]);
                                                                    }
                                                                }
                                                            } else if (
                                                                       (1U 
                                                                        & (IData)(vlSelfRef.test_copper__DOT__cpu_wdata))) {
                                                                __Vdly__test_copper__DOT__dut__DOT__cmd_x 
                                                                    = 
                                                                    ((0x00000300U 
                                                                      & (vlSelfRef.test_copper__DOT__dut__DOT__regs[18U] 
                                                                         << 8U)) 
                                                                     | vlSelfRef.test_copper__DOT__dut__DOT__regs[17U]);
                                                                __Vdly__test_copper__DOT__dut__DOT__cmd_y 
                                                                    = 
                                                                    ((0x00000300U 
                                                                      & (vlSelfRef.test_copper__DOT__dut__DOT__regs[20U] 
                                                                         << 8U)) 
                                                                     | vlSelfRef.test_copper__DOT__dut__DOT__regs[19U]);
                                                                if (
                                                                    ((0x0140U 
                                                                      > 
                                                                      ((0x00000100U 
                                                                        & (vlSelfRef.test_copper__DOT__dut__DOT__regs[18U] 
                                                                           << 8U)) 
                                                                       | vlSelfRef.test_copper__DOT__dut__DOT__regs[17U])) 
                                                                     & (0xc8U 
                                                                        > vlSelfRef.test_copper__DOT__dut__DOT__regs[19U]))) {
                                                                    vlSelfRef.test_copper__DOT__dut__DOT__fill_target 
                                                                        = 
                                                                        ((0xf9ffU 
                                                                          >= 
                                                                          (0x0000ffffU 
                                                                           & (((IData)(0x00000140U) 
                                                                               * vlSelfRef.test_copper__DOT__dut__DOT__regs[19U]) 
                                                                              + 
                                                                              ((0x00000100U 
                                                                                & (vlSelfRef.test_copper__DOT__dut__DOT__regs[18U] 
                                                                                << 8U)) 
                                                                               | vlSelfRef.test_copper__DOT__dut__DOT__regs[17U]))))
                                                                          ? vlSelfRef.test_copper__DOT__dut__DOT__gfx_ram
                                                                         [
                                                                         (0x0000ffffU 
                                                                          & (((IData)(0x00000140U) 
                                                                              * vlSelfRef.test_copper__DOT__dut__DOT__regs[19U]) 
                                                                             + 
                                                                             ((0x00000100U 
                                                                               & (vlSelfRef.test_copper__DOT__dut__DOT__regs[18U] 
                                                                                << 8U)) 
                                                                              | vlSelfRef.test_copper__DOT__dut__DOT__regs[17U])))]
                                                                          : 0U);
                                                                    __Vdly__test_copper__DOT__dut__DOT__fill_sp = 1U;
                                                                    __Vdly__test_copper__DOT__dut__DOT__cmd_busy = 1U;
                                                                    vlSelfRef.test_copper__DOT__dut__DOT__cmd_op = 9U;
                                                                    __VdlyVal__test_copper__DOT__dut__DOT__fill_stack__v4 
                                                                        = 
                                                                        ((vlSelfRef.test_copper__DOT__dut__DOT__regs[19U] 
                                                                          << 9U) 
                                                                         | ((0x00000100U 
                                                                             & (vlSelfRef.test_copper__DOT__dut__DOT__regs[18U] 
                                                                                << 8U)) 
                                                                            | vlSelfRef.test_copper__DOT__dut__DOT__regs[17U]));
                                                                    __VdlySet__test_copper__DOT__dut__DOT__fill_stack__v4 = 1U;
                                                                }
                                                            } else {
                                                                __Vdly__test_copper__DOT__dut__DOT__gfx_color 
                                                                    = 
                                                                    (0x0000000fU 
                                                                     & vlSelfRef.test_copper__DOT__dut__DOT__regs[17U]);
                                                            }
                                                        }
                                                    } else if (
                                                               (4U 
                                                                & (IData)(vlSelfRef.test_copper__DOT__cpu_wdata))) {
                                                        if (
                                                            (2U 
                                                             & (IData)(vlSelfRef.test_copper__DOT__cpu_wdata))) {
                                                            if (
                                                                (1U 
                                                                 & (IData)(vlSelfRef.test_copper__DOT__cpu_wdata))) {
                                                                __Vdly__test_copper__DOT__dut__DOT__cmd_cy = 0U;
                                                                __Vdly__test_copper__DOT__dut__DOT__cmd_cx = 0U;
                                                                __Vdly__test_copper__DOT__dut__DOT__cmd_busy = 1U;
                                                                vlSelfRef.test_copper__DOT__dut__DOT__cmd_op = 7U;
                                                            } else {
                                                                __Vdly__test_copper__DOT__dut__DOT__cmd_cx 
                                                                    = 
                                                                    ((0x00000100U 
                                                                      & (vlSelfRef.test_copper__DOT__dut__DOT__regs[18U] 
                                                                         << 8U)) 
                                                                     | vlSelfRef.test_copper__DOT__dut__DOT__regs[17U]);
                                                                __Vdly__test_copper__DOT__dut__DOT__cmd_busy = 1U;
                                                                vlSelfRef.test_copper__DOT__dut__DOT__cmd_op 
                                                                    = vlSelfRef.test_copper__DOT__cpu_wdata;
                                                                __Vdly__test_copper__DOT__dut__DOT__cmd_cy 
                                                                    = vlSelfRef.test_copper__DOT__dut__DOT__regs[19U];
                                                            }
                                                        } else if (
                                                                   (1U 
                                                                    & (IData)(vlSelfRef.test_copper__DOT__cpu_wdata))) {
                                                            __Vdly__test_copper__DOT__dut__DOT__cmd_cx 
                                                                = 
                                                                ((0x00000100U 
                                                                  & (vlSelfRef.test_copper__DOT__dut__DOT__regs[18U] 
                                                                     << 8U)) 
                                                                 | vlSelfRef.test_copper__DOT__dut__DOT__regs[17U]);
                                                            __Vdly__test_copper__DOT__dut__DOT__cmd_busy = 1U;
                                                            vlSelfRef.test_copper__DOT__dut__DOT__cmd_op 
                                                                = vlSelfRef.test_copper__DOT__cpu_wdata;
                                                            __Vdly__test_copper__DOT__dut__DOT__cmd_cy 
                                                                = vlSelfRef.test_copper__DOT__dut__DOT__regs[19U];
                                                        } else {
                                                            __Vdly__test_copper__DOT__dut__DOT__cmd_x 
                                                                = 
                                                                ((0x00000300U 
                                                                  & (vlSelfRef.test_copper__DOT__dut__DOT__regs[18U] 
                                                                     << 8U)) 
                                                                 | vlSelfRef.test_copper__DOT__dut__DOT__regs[17U]);
                                                            __Vdly__test_copper__DOT__dut__DOT__cmd_y 
                                                                = 
                                                                ((0x00000300U 
                                                                  & (vlSelfRef.test_copper__DOT__dut__DOT__regs[20U] 
                                                                     << 8U)) 
                                                                 | vlSelfRef.test_copper__DOT__dut__DOT__regs[19U]);
                                                            __Vdly__test_copper__DOT__dut__DOT__cmd_dx = 0U;
                                                            __Vdly__test_copper__DOT__dut__DOT__cmd_busy = 1U;
                                                            vlSelfRef.test_copper__DOT__dut__DOT__cmd_op = 4U;
                                                            vlSelfRef.test_copper__DOT__dut__DOT__cmd_x2 
                                                                = 
                                                                ((0x00000300U 
                                                                  & (vlSelfRef.test_copper__DOT__dut__DOT__regs[22U] 
                                                                     << 8U)) 
                                                                 | vlSelfRef.test_copper__DOT__dut__DOT__regs[21U]);
                                                            __Vdly__test_copper__DOT__dut__DOT__cmd_dy 
                                                                = 
                                                                ((0x00000300U 
                                                                  & (vlSelfRef.test_copper__DOT__dut__DOT__regs[22U] 
                                                                     << 8U)) 
                                                                 | vlSelfRef.test_copper__DOT__dut__DOT__regs[21U]);
                                                            __Vdly__test_copper__DOT__dut__DOT__cmd_err 
                                                                = 
                                                                (0x000003ffU 
                                                                 & ((IData)(1U) 
                                                                    - 
                                                                    ((0x00000300U 
                                                                      & (vlSelfRef.test_copper__DOT__dut__DOT__regs[22U] 
                                                                         << 8U)) 
                                                                     | vlSelfRef.test_copper__DOT__dut__DOT__regs[21U])));
                                                        }
                                                    } else if (
                                                               (2U 
                                                                & (IData)(vlSelfRef.test_copper__DOT__cpu_wdata))) {
                                                        if (
                                                            (1U 
                                                             & (IData)(vlSelfRef.test_copper__DOT__cpu_wdata))) {
                                                            if (
                                                                (((0x00000100U 
                                                                   & (vlSelfRef.test_copper__DOT__dut__DOT__regs[22U] 
                                                                      << 8U)) 
                                                                  | vlSelfRef.test_copper__DOT__dut__DOT__regs[21U]) 
                                                                 >= 
                                                                 ((0x00000100U 
                                                                   & (vlSelfRef.test_copper__DOT__dut__DOT__regs[18U] 
                                                                      << 8U)) 
                                                                  | vlSelfRef.test_copper__DOT__dut__DOT__regs[17U]))) {
                                                                __Vdly__test_copper__DOT__dut__DOT__cmd_dx 
                                                                    = 
                                                                    (0x000003ffU 
                                                                     & (((0x00000100U 
                                                                          & (vlSelfRef.test_copper__DOT__dut__DOT__regs[22U] 
                                                                             << 8U)) 
                                                                         | vlSelfRef.test_copper__DOT__dut__DOT__regs[21U]) 
                                                                        - 
                                                                        ((0x00000100U 
                                                                          & (vlSelfRef.test_copper__DOT__dut__DOT__regs[18U] 
                                                                             << 8U)) 
                                                                         | vlSelfRef.test_copper__DOT__dut__DOT__regs[17U])));
                                                                vlSelfRef.test_copper__DOT__dut__DOT__cmd_sx = 1U;
                                                            } else {
                                                                __Vdly__test_copper__DOT__dut__DOT__cmd_dx 
                                                                    = 
                                                                    (0x000003ffU 
                                                                     & (((0x00000100U 
                                                                          & (vlSelfRef.test_copper__DOT__dut__DOT__regs[18U] 
                                                                             << 8U)) 
                                                                         | vlSelfRef.test_copper__DOT__dut__DOT__regs[17U]) 
                                                                        - 
                                                                        ((0x00000100U 
                                                                          & (vlSelfRef.test_copper__DOT__dut__DOT__regs[22U] 
                                                                             << 8U)) 
                                                                         | vlSelfRef.test_copper__DOT__dut__DOT__regs[21U])));
                                                                vlSelfRef.test_copper__DOT__dut__DOT__cmd_sx = 0x03ffU;
                                                            }
                                                            __Vdly__test_copper__DOT__dut__DOT__cmd_busy = 1U;
                                                            vlSelfRef.test_copper__DOT__dut__DOT__cmd_op = 3U;
                                                            if (
                                                                (vlSelfRef.test_copper__DOT__dut__DOT__regs[23U] 
                                                                 >= vlSelfRef.test_copper__DOT__dut__DOT__regs[19U])) {
                                                                __Vdly__test_copper__DOT__dut__DOT__cmd_dy 
                                                                    = 
                                                                    (0x000003ffU 
                                                                     & (- 
                                                                        (vlSelfRef.test_copper__DOT__dut__DOT__regs[23U] 
                                                                         - vlSelfRef.test_copper__DOT__dut__DOT__regs[19U])));
                                                                vlSelfRef.test_copper__DOT__dut__DOT__cmd_sy = 1U;
                                                            } else {
                                                                __Vdly__test_copper__DOT__dut__DOT__cmd_dy 
                                                                    = 
                                                                    (0x000003ffU 
                                                                     & (vlSelfRef.test_copper__DOT__dut__DOT__regs[19U] 
                                                                        - vlSelfRef.test_copper__DOT__dut__DOT__regs[23U]));
                                                                vlSelfRef.test_copper__DOT__dut__DOT__cmd_sy = 0x03ffU;
                                                            }
                                                            __Vdly__test_copper__DOT__dut__DOT__cmd_err 
                                                                = 
                                                                (0x000003ffU 
                                                                 & (((((0x00000100U 
                                                                        & (vlSelfRef.test_copper__DOT__dut__DOT__regs[22U] 
                                                                           << 8U)) 
                                                                       | vlSelfRef.test_copper__DOT__dut__DOT__regs[21U]) 
                                                                      >= 
                                                                      ((0x00000100U 
                                                                        & (vlSelfRef.test_copper__DOT__dut__DOT__regs[18U] 
                                                                           << 8U)) 
                                                                       | vlSelfRef.test_copper__DOT__dut__DOT__regs[17U]))
                                                                      ? 
                                                                     (((0x00000100U 
                                                                        & (vlSelfRef.test_copper__DOT__dut__DOT__regs[22U] 
                                                                           << 8U)) 
                                                                       | vlSelfRef.test_copper__DOT__dut__DOT__regs[21U]) 
                                                                      - 
                                                                      ((0x00000100U 
                                                                        & (vlSelfRef.test_copper__DOT__dut__DOT__regs[18U] 
                                                                           << 8U)) 
                                                                       | vlSelfRef.test_copper__DOT__dut__DOT__regs[17U]))
                                                                      : 
                                                                     (((0x00000100U 
                                                                        & (vlSelfRef.test_copper__DOT__dut__DOT__regs[18U] 
                                                                           << 8U)) 
                                                                       | vlSelfRef.test_copper__DOT__dut__DOT__regs[17U]) 
                                                                      - 
                                                                      ((0x00000100U 
                                                                        & (vlSelfRef.test_copper__DOT__dut__DOT__regs[22U] 
                                                                           << 8U)) 
                                                                       | vlSelfRef.test_copper__DOT__dut__DOT__regs[21U]))) 
                                                                    + 
                                                                    ((vlSelfRef.test_copper__DOT__dut__DOT__regs[23U] 
                                                                      >= vlSelfRef.test_copper__DOT__dut__DOT__regs[19U])
                                                                      ? 
                                                                     (- 
                                                                      (vlSelfRef.test_copper__DOT__dut__DOT__regs[23U] 
                                                                       - vlSelfRef.test_copper__DOT__dut__DOT__regs[19U]))
                                                                      : 
                                                                     (vlSelfRef.test_copper__DOT__dut__DOT__regs[19U] 
                                                                      - vlSelfRef.test_copper__DOT__dut__DOT__regs[23U]))));
                                                        } else if (
                                                                   ((0x0140U 
                                                                     > 
                                                                     ((0x00000100U 
                                                                       & (vlSelfRef.test_copper__DOT__dut__DOT__regs[18U] 
                                                                          << 8U)) 
                                                                      | vlSelfRef.test_copper__DOT__dut__DOT__regs[17U])) 
                                                                    & (0xc8U 
                                                                       > vlSelfRef.test_copper__DOT__dut__DOT__regs[19U]))) {
                                                            vlSelfRef.test_copper__DOT__dut__DOT____Vlvbound_hde0d773d__1 = 0U;
                                                            if (VL_LIKELY((
                                                                           (0xf9ffU 
                                                                            >= 
                                                                            (0x0000ffffU 
                                                                             & (((IData)(0x00000140U) 
                                                                                * vlSelfRef.test_copper__DOT__dut__DOT__regs[19U]) 
                                                                                + 
                                                                                ((0x00000100U 
                                                                                & (vlSelfRef.test_copper__DOT__dut__DOT__regs[18U] 
                                                                                << 8U)) 
                                                                                | vlSelfRef.test_copper__DOT__dut__DOT__regs[17U]))))))) {
                                                                __VdlyVal__test_copper__DOT__dut__DOT__gfx_ram__v16 
                                                                    = vlSelfRef.test_copper__DOT__dut__DOT____Vlvbound_hde0d773d__1;
                                                                __VdlyDim0__test_copper__DOT__dut__DOT__gfx_ram__v16 
                                                                    = 
                                                                    (0x0000ffffU 
                                                                     & (((IData)(0x00000140U) 
                                                                         * vlSelfRef.test_copper__DOT__dut__DOT__regs[19U]) 
                                                                        + 
                                                                        ((0x00000100U 
                                                                          & (vlSelfRef.test_copper__DOT__dut__DOT__regs[18U] 
                                                                             << 8U)) 
                                                                         | vlSelfRef.test_copper__DOT__dut__DOT__regs[17U])));
                                                                vlSelfRef.__VdlyCommitQueuetest_copper__DOT__dut__DOT__gfx_ram.enqueue(__VdlyVal__test_copper__DOT__dut__DOT__gfx_ram__v16, (IData)(__VdlyDim0__test_copper__DOT__dut__DOT__gfx_ram__v16));
                                                            }
                                                        }
                                                    } else if (
                                                               (1U 
                                                                & (IData)(vlSelfRef.test_copper__DOT__cpu_wdata))) {
                                                        if (
                                                            ((0x0140U 
                                                              > 
                                                              ((0x00000100U 
                                                                & (vlSelfRef.test_copper__DOT__dut__DOT__regs[18U] 
                                                                   << 8U)) 
                                                               | vlSelfRef.test_copper__DOT__dut__DOT__regs[17U])) 
                                                             & (0xc8U 
                                                                > vlSelfRef.test_copper__DOT__dut__DOT__regs[19U]))) {
                                                            vlSelfRef.test_copper__DOT__dut__DOT____Vlvbound_hde0d773d__0 
                                                                = vlSelfRef.test_copper__DOT__dut__DOT__gfx_color;
                                                            if (VL_LIKELY((
                                                                           (0xf9ffU 
                                                                            >= 
                                                                            (0x0000ffffU 
                                                                             & (((IData)(0x00000140U) 
                                                                                * vlSelfRef.test_copper__DOT__dut__DOT__regs[19U]) 
                                                                                + 
                                                                                ((0x00000100U 
                                                                                & (vlSelfRef.test_copper__DOT__dut__DOT__regs[18U] 
                                                                                << 8U)) 
                                                                                | vlSelfRef.test_copper__DOT__dut__DOT__regs[17U]))))))) {
                                                                __VdlyVal__test_copper__DOT__dut__DOT__gfx_ram__v17 
                                                                    = vlSelfRef.test_copper__DOT__dut__DOT____Vlvbound_hde0d773d__0;
                                                                __VdlyDim0__test_copper__DOT__dut__DOT__gfx_ram__v17 
                                                                    = 
                                                                    (0x0000ffffU 
                                                                     & (((IData)(0x00000140U) 
                                                                         * vlSelfRef.test_copper__DOT__dut__DOT__regs[19U]) 
                                                                        + 
                                                                        ((0x00000100U 
                                                                          & (vlSelfRef.test_copper__DOT__dut__DOT__regs[18U] 
                                                                             << 8U)) 
                                                                         | vlSelfRef.test_copper__DOT__dut__DOT__regs[17U])));
                                                                vlSelfRef.__VdlyCommitQueuetest_copper__DOT__dut__DOT__gfx_ram.enqueue(__VdlyVal__test_copper__DOT__dut__DOT__gfx_ram__v17, (IData)(__VdlyDim0__test_copper__DOT__dut__DOT__gfx_ram__v17));
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
                } else {
                    __VdlyVal__test_copper__DOT__dut__DOT__regs__v37 
                        = vlSelfRef.test_copper__DOT__cpu_wdata;
                    __VdlyDim0__test_copper__DOT__dut__DOT__regs__v37 
                        = (0x0000001fU & (IData)(vlSelfRef.test_copper__DOT__cpu_addr));
                    __VdlySet__test_copper__DOT__dut__DOT__regs__v37 = 1U;
                }
            }
            if (vlSelfRef.test_copper__DOT__dut__DOT__char_ram_sel) {
                vlSelfRef.test_copper__DOT__dut__DOT____Vlvbound_h87c37a99__0 
                    = vlSelfRef.test_copper__DOT__cpu_wdata;
                if (VL_LIKELY(((0x07cfU >= (0x000007ffU 
                                            & ((IData)(vlSelfRef.test_copper__DOT__cpu_addr) 
                                               - (IData)(0x0200U))))))) {
                    __VdlyVal__test_copper__DOT__dut__DOT__char_ram__v6 
                        = vlSelfRef.test_copper__DOT__dut__DOT____Vlvbound_h87c37a99__0;
                    __VdlyDim0__test_copper__DOT__dut__DOT__char_ram__v6 
                        = (0x000007ffU & ((IData)(vlSelfRef.test_copper__DOT__cpu_addr) 
                                          - (IData)(0x0200U)));
                    vlSelfRef.__VdlyCommitQueuetest_copper__DOT__dut__DOT__char_ram.enqueue(__VdlyVal__test_copper__DOT__dut__DOT__char_ram__v6, (IData)(__VdlyDim0__test_copper__DOT__dut__DOT__char_ram__v6));
                }
            }
            if ((0xb9a3U == (IData)(vlSelfRef.test_copper__DOT__cpu_addr))) {
                vlSelfRef.test_copper__DOT__dut__DOT__fio_name_len 
                    = (0x0000003fU & (IData)(vlSelfRef.test_copper__DOT__cpu_wdata));
            }
            if (((0xb9b0U <= (IData)(vlSelfRef.test_copper__DOT__cpu_addr)) 
                 & (0xb9efU >= (IData)(vlSelfRef.test_copper__DOT__cpu_addr)))) {
                __VdlyVal__test_copper__DOT__dut__DOT__fio_name__v0 
                    = vlSelfRef.test_copper__DOT__cpu_wdata;
                __VdlyDim0__test_copper__DOT__dut__DOT__fio_name__v0 
                    = (0x0000003fU & ((IData)(vlSelfRef.test_copper__DOT__cpu_addr) 
                                      - (IData)(0x30U)));
                __VdlySet__test_copper__DOT__dut__DOT__fio_name__v0 = 1U;
            }
            if (vlSelfRef.test_copper__DOT__dut__DOT__color_ram_sel) {
                vlSelfRef.test_copper__DOT__dut__DOT____Vlvbound_hced7a248__0 
                    = vlSelfRef.test_copper__DOT__cpu_wdata;
                if (VL_LIKELY(((0x07cfU >= (0x000007ffU 
                                            & ((IData)(vlSelfRef.test_copper__DOT__cpu_addr) 
                                               - (IData)(0x01d0U))))))) {
                    __VdlyVal__test_copper__DOT__dut__DOT__color_ram__v5 
                        = vlSelfRef.test_copper__DOT__dut__DOT____Vlvbound_hced7a248__0;
                    __VdlyDim0__test_copper__DOT__dut__DOT__color_ram__v5 
                        = (0x000007ffU & ((IData)(vlSelfRef.test_copper__DOT__cpu_addr) 
                                          - (IData)(0x01d0U)));
                    vlSelfRef.__VdlyCommitQueuetest_copper__DOT__dut__DOT__color_ram.enqueue(__VdlyVal__test_copper__DOT__dut__DOT__color_ram__v5, (IData)(__VdlyDim0__test_copper__DOT__dut__DOT__color_ram__v5));
                }
            }
            if (((0xa040U <= (IData)(vlSelfRef.test_copper__DOT__cpu_addr)) 
                 & (0xa0bfU >= (IData)(vlSelfRef.test_copper__DOT__cpu_addr)))) {
                if ((4U & (IData)(vlSelfRef.test_copper__DOT__cpu_addr))) {
                    if ((2U & (IData)(vlSelfRef.test_copper__DOT__cpu_addr))) {
                        if ((1U & (IData)(vlSelfRef.test_copper__DOT__cpu_addr))) {
                            __VdlyVal__test_copper__DOT__dut__DOT__spr_trans__v16 
                                = (0x0000000fU & (IData)(vlSelfRef.test_copper__DOT__cpu_wdata));
                            __VdlyDim0__test_copper__DOT__dut__DOT__spr_trans__v16 
                                = (0x0000000fU & ((IData)(vlSelfRef.test_copper__DOT__cpu_addr) 
                                                  >> 3U));
                            __VdlySet__test_copper__DOT__dut__DOT__spr_trans__v16 = 1U;
                        } else {
                            __VdlyVal__test_copper__DOT__dut__DOT__spr_pri__v17 
                                = (3U & (IData)(vlSelfRef.test_copper__DOT__cpu_wdata));
                            __VdlyDim0__test_copper__DOT__dut__DOT__spr_pri__v17 
                                = (0x0000000fU & ((IData)(vlSelfRef.test_copper__DOT__cpu_addr) 
                                                  >> 3U));
                            __VdlySet__test_copper__DOT__dut__DOT__spr_pri__v17 = 1U;
                        }
                    } else if ((1U & (IData)(vlSelfRef.test_copper__DOT__cpu_addr))) {
                        __VdlyVal__test_copper__DOT__dut__DOT__spr_flip_h__v17 
                            = (1U & (IData)(vlSelfRef.test_copper__DOT__cpu_wdata));
                        __VdlyDim0__test_copper__DOT__dut__DOT__spr_flip_h__v17 
                            = (0x0000000fU & ((IData)(vlSelfRef.test_copper__DOT__cpu_addr) 
                                              >> 3U));
                        __VdlySet__test_copper__DOT__dut__DOT__spr_flip_h__v17 = 1U;
                        __VdlyVal__test_copper__DOT__dut__DOT__spr_flip_v__v17 
                            = (1U & ((IData)(vlSelfRef.test_copper__DOT__cpu_wdata) 
                                     >> 1U));
                        __VdlyDim0__test_copper__DOT__dut__DOT__spr_flip_v__v17 
                            = (0x0000000fU & ((IData)(vlSelfRef.test_copper__DOT__cpu_addr) 
                                              >> 3U));
                        __VdlyVal__test_copper__DOT__dut__DOT__spr_enable__v18 
                            = (1U & ((IData)(vlSelfRef.test_copper__DOT__cpu_wdata) 
                                     >> 7U));
                        __VdlyDim0__test_copper__DOT__dut__DOT__spr_enable__v18 
                            = (0x0000000fU & ((IData)(vlSelfRef.test_copper__DOT__cpu_addr) 
                                              >> 3U));
                    } else {
                        __VdlyVal__test_copper__DOT__dut__DOT__spr_shape__v16 
                            = (0x0000000fU & (IData)(vlSelfRef.test_copper__DOT__cpu_wdata));
                        __VdlyDim0__test_copper__DOT__dut__DOT__spr_shape__v16 
                            = (0x0000000fU & ((IData)(vlSelfRef.test_copper__DOT__cpu_addr) 
                                              >> 3U));
                        __VdlySet__test_copper__DOT__dut__DOT__spr_shape__v16 = 1U;
                    }
                } else if ((2U & (IData)(vlSelfRef.test_copper__DOT__cpu_addr))) {
                    if ((1U & (~ (IData)(vlSelfRef.test_copper__DOT__cpu_addr)))) {
                        __VdlyVal__test_copper__DOT__dut__DOT__spr_y__v17 
                            = vlSelfRef.test_copper__DOT__cpu_wdata;
                        __VdlyDim0__test_copper__DOT__dut__DOT__spr_y__v17 
                            = (0x0000000fU & ((IData)(vlSelfRef.test_copper__DOT__cpu_addr) 
                                              >> 3U));
                        __VdlySet__test_copper__DOT__dut__DOT__spr_y__v17 = 1U;
                    }
                } else if ((1U & (IData)(vlSelfRef.test_copper__DOT__cpu_addr))) {
                    __VdlyVal__test_copper__DOT__dut__DOT__spr_x__v17 
                        = (1U & (IData)(vlSelfRef.test_copper__DOT__cpu_wdata));
                    __VdlyDim0__test_copper__DOT__dut__DOT__spr_x__v17 
                        = (0x0000000fU & ((IData)(vlSelfRef.test_copper__DOT__cpu_addr) 
                                          >> 3U));
                    __VdlySet__test_copper__DOT__dut__DOT__spr_x__v17 = 1U;
                } else {
                    __VdlyVal__test_copper__DOT__dut__DOT__spr_x__v18 
                        = vlSelfRef.test_copper__DOT__cpu_wdata;
                    __VdlyDim0__test_copper__DOT__dut__DOT__spr_x__v18 
                        = (0x0000000fU & ((IData)(vlSelfRef.test_copper__DOT__cpu_addr) 
                                          >> 3U));
                    __VdlySet__test_copper__DOT__dut__DOT__spr_x__v18 = 1U;
                }
            }
        }
    }
    vlSelfRef.test_copper__DOT__dut__DOT__tile_reg_sel 
        = ((0xa0c0U <= (IData)(vlSelfRef.test_copper__DOT__cpu_addr)) 
           & (0xa0dfU >= (IData)(vlSelfRef.test_copper__DOT__cpu_addr)));
    vlSelfRef.test_copper__DOT__dut__DOT__vgc_reg_sel 
        = ((0xa000U <= (IData)(vlSelfRef.test_copper__DOT__cpu_addr)) 
           & (0xa01fU >= (IData)(vlSelfRef.test_copper__DOT__cpu_addr)));
    vlSelfRef.test_copper__DOT__dut__DOT__char_ram_sel 
        = ((0xaa00U <= (IData)(vlSelfRef.test_copper__DOT__cpu_addr)) 
           & (0xb1cfU >= (IData)(vlSelfRef.test_copper__DOT__cpu_addr)));
    vlSelfRef.test_copper__DOT__dut__DOT__color_ram_sel 
        = ((0xb1d0U <= (IData)(vlSelfRef.test_copper__DOT__cpu_addr)) 
           & (0xb99fU >= (IData)(vlSelfRef.test_copper__DOT__cpu_addr)));
    vlSelfRef.test_copper__DOT__dut__DOT__cursor_x 
        = __Vdly__test_copper__DOT__dut__DOT__cursor_x;
    vlSelfRef.test_copper__DOT__dut__DOT__cursor_y 
        = __Vdly__test_copper__DOT__dut__DOT__cursor_y;
    vlSelfRef.test_copper__DOT__dut__DOT__gfx_color 
        = __Vdly__test_copper__DOT__dut__DOT__gfx_color;
    vlSelfRef.test_copper__DOT__dut__DOT__scroll_pending 
        = __Vdly__test_copper__DOT__dut__DOT__scroll_pending;
    vlSelfRef.test_copper__DOT__dut__DOT__scroll_col 
        = __Vdly__test_copper__DOT__dut__DOT__scroll_col;
    vlSelfRef.test_copper__DOT__dut__DOT__cmd_busy 
        = __Vdly__test_copper__DOT__dut__DOT__cmd_busy;
    vlSelfRef.test_copper__DOT__dut__DOT__copper_target_list 
        = __Vdly__test_copper__DOT__dut__DOT__copper_target_list;
    vlSelfRef.test_copper__DOT__dut__DOT__fill_sp = __Vdly__test_copper__DOT__dut__DOT__fill_sp;
    vlSelfRef.test_copper__DOT__dut__DOT__cmd_cx = __Vdly__test_copper__DOT__dut__DOT__cmd_cx;
    vlSelfRef.test_copper__DOT__dut__DOT__cmd_cy = __Vdly__test_copper__DOT__dut__DOT__cmd_cy;
    vlSelfRef.__VdlyCommitQueuetest_copper__DOT__dut__DOT__sprite_shapes.commit(vlSelfRef.test_copper__DOT__dut__DOT__sprite_shapes);
    vlSelfRef.test_copper__DOT__dut__DOT__gt_font_row 
        = __Vdly__test_copper__DOT__dut__DOT__gt_font_row;
    vlSelfRef.test_copper__DOT__dut__DOT__gt_char_idx 
        = __Vdly__test_copper__DOT__dut__DOT__gt_char_idx;
    vlSelfRef.__VdlyCommitQueuetest_copper__DOT__dut__DOT__gfx_ram.commit(vlSelfRef.test_copper__DOT__dut__DOT__gfx_ram);
    vlSelfRef.test_copper__DOT__dut__DOT__cmd_dx = __Vdly__test_copper__DOT__dut__DOT__cmd_dx;
    vlSelfRef.test_copper__DOT__dut__DOT__cmd_err = __Vdly__test_copper__DOT__dut__DOT__cmd_err;
    vlSelfRef.test_copper__DOT__dut__DOT__cmd_dy = __Vdly__test_copper__DOT__dut__DOT__cmd_dy;
    vlSelfRef.test_copper__DOT__dut__DOT__cmd_x = __Vdly__test_copper__DOT__dut__DOT__cmd_x;
    vlSelfRef.test_copper__DOT__dut__DOT__cmd_y = __Vdly__test_copper__DOT__dut__DOT__cmd_y;
    vlSelfRef.__VdlyCommitQueuetest_copper__DOT__dut__DOT__copper_list_count.commit(vlSelfRef.test_copper__DOT__dut__DOT__copper_list_count);
    vlSelfRef.test_copper__DOT__dut__DOT__copper_count 
        = __Vdly__test_copper__DOT__dut__DOT__copper_count;
    vlSelfRef.__VdlyCommitQueuetest_copper__DOT__dut__DOT__char_ram.commit(vlSelfRef.test_copper__DOT__dut__DOT__char_ram);
    vlSelfRef.__VdlyCommitQueuetest_copper__DOT__dut__DOT__color_ram.commit(vlSelfRef.test_copper__DOT__dut__DOT__color_ram);
    if (vlSelfRef.test_copper__DOT__rst) {
        __Vdly__test_copper__DOT__dut__DOT__copper_active_list = 0U;
    } else if (((0U == (IData)(vlSelfRef.test_copper__DOT__dut__DOT__h_count)) 
                & (0x01e0U == (IData)(vlSelfRef.test_copper__DOT__dut__DOT__v_count)))) {
        if ((((IData)(vlSelfRef.test_copper__DOT__dut__DOT__copper_pending_list) 
              != (IData)(vlSelfRef.test_copper__DOT__dut__DOT__copper_active_list)) 
             & (~ (IData)(vlSelfRef.test_copper__DOT__dut__DOT__copper_loading)))) {
            __Vdly__test_copper__DOT__dut__DOT__copper_active_list 
                = vlSelfRef.test_copper__DOT__dut__DOT__copper_pending_list;
            __Vdly__test_copper__DOT__dut__DOT__copper_loading = 1U;
            vlSelfRef.test_copper__DOT__dut__DOT__copper_load_idx = 0U;
            vlSelfRef.test_copper__DOT__dut__DOT__copper_load_src 
                = vlSelfRef.test_copper__DOT__dut__DOT__copper_pending_list;
        }
    }
    vlSelfRef.test_copper__DOT__dut__DOT__fg_color 
        = __Vdly__test_copper__DOT__dut__DOT__fg_color;
    vlSelfRef.test_copper__DOT__dut__DOT__copper_index 
        = __Vdly__test_copper__DOT__dut__DOT__copper_index;
    if (__VdlySet__test_copper__DOT__dut__DOT__fio_name__v0) {
        vlSelfRef.test_copper__DOT__dut__DOT__fio_name[__VdlyDim0__test_copper__DOT__dut__DOT__fio_name__v0] 
            = __VdlyVal__test_copper__DOT__dut__DOT__fio_name__v0;
    }
    if (__VdlySet__test_copper__DOT__dut__DOT__tile_inst__DOT__tile_data__v0) {
        vlSelfRef.test_copper__DOT__dut__DOT__tile_inst__DOT__tile_data[__VdlyDim0__test_copper__DOT__dut__DOT__tile_inst__DOT__tile_data__v0] 
            = __VdlyVal__test_copper__DOT__dut__DOT__tile_inst__DOT__tile_data__v0;
    }
    if (__VdlySet__test_copper__DOT__dut__DOT__tile_inst__DOT__tile_data__v1) {
        vlSelfRef.test_copper__DOT__dut__DOT__tile_inst__DOT__tile_data[__VdlyDim0__test_copper__DOT__dut__DOT__tile_inst__DOT__tile_data__v1] = 0U;
    }
    vlSelfRef.test_copper__DOT__dut__DOT__scroll_offset 
        = __Vdly__test_copper__DOT__dut__DOT__scroll_offset;
    if (__VdlySet__test_copper__DOT__dut__DOT__fill_stack__v0) {
        vlSelfRef.test_copper__DOT__dut__DOT__fill_stack[__VdlyDim0__test_copper__DOT__dut__DOT__fill_stack__v0] 
            = __VdlyVal__test_copper__DOT__dut__DOT__fill_stack__v0;
        vlSelfRef.test_copper__DOT__dut__DOT__fill_stack[__VdlyDim0__test_copper__DOT__dut__DOT__fill_stack__v1] 
            = __VdlyVal__test_copper__DOT__dut__DOT__fill_stack__v1;
        vlSelfRef.test_copper__DOT__dut__DOT__fill_stack[__VdlyDim0__test_copper__DOT__dut__DOT__fill_stack__v2] 
            = __VdlyVal__test_copper__DOT__dut__DOT__fill_stack__v2;
        vlSelfRef.test_copper__DOT__dut__DOT__fill_stack[__VdlyDim0__test_copper__DOT__dut__DOT__fill_stack__v3] 
            = __VdlyVal__test_copper__DOT__dut__DOT__fill_stack__v3;
    }
    if (__VdlySet__test_copper__DOT__dut__DOT__fill_stack__v4) {
        vlSelfRef.test_copper__DOT__dut__DOT__fill_stack[0U] 
            = __VdlyVal__test_copper__DOT__dut__DOT__fill_stack__v4;
    }
    if (__VdlySet__test_copper__DOT__dut__DOT__copper_list_pos__v0) {
        vlSelfRef.test_copper__DOT__dut__DOT__copper_list_pos[__VdlyDim0__test_copper__DOT__dut__DOT__copper_list_pos__v0] 
            = __VdlyVal__test_copper__DOT__dut__DOT__copper_list_pos__v0;
        vlSelfRef.test_copper__DOT__dut__DOT__copper_list_reg[__VdlyDim0__test_copper__DOT__dut__DOT__copper_list_reg__v0] 
            = __VdlyVal__test_copper__DOT__dut__DOT__copper_list_reg__v0;
        vlSelfRef.test_copper__DOT__dut__DOT__copper_list_val[__VdlyDim0__test_copper__DOT__dut__DOT__copper_list_val__v0] 
            = __VdlyVal__test_copper__DOT__dut__DOT__copper_list_val__v0;
    }
    if (__VdlySet__test_copper__DOT__dut__DOT__copper_pos__v0) {
        vlSelfRef.test_copper__DOT__dut__DOT__copper_pos[__VdlyDim0__test_copper__DOT__dut__DOT__copper_pos__v0] 
            = __VdlyVal__test_copper__DOT__dut__DOT__copper_pos__v0;
        vlSelfRef.test_copper__DOT__dut__DOT__copper_reg[__VdlyDim0__test_copper__DOT__dut__DOT__copper_reg__v0] 
            = __VdlyVal__test_copper__DOT__dut__DOT__copper_reg__v0;
        vlSelfRef.test_copper__DOT__dut__DOT__copper_val[__VdlyDim0__test_copper__DOT__dut__DOT__copper_val__v0] 
            = __VdlyVal__test_copper__DOT__dut__DOT__copper_val__v0;
    }
    if (__VdlySet__test_copper__DOT__dut__DOT__copper_pos__v1) {
        vlSelfRef.test_copper__DOT__dut__DOT__copper_pos[__VdlyDim0__test_copper__DOT__dut__DOT__copper_pos__v1] 
            = __VdlyVal__test_copper__DOT__dut__DOT__copper_pos__v1;
        vlSelfRef.test_copper__DOT__dut__DOT__copper_reg[__VdlyDim0__test_copper__DOT__dut__DOT__copper_reg__v1] 
            = __VdlyVal__test_copper__DOT__dut__DOT__copper_reg__v1;
        vlSelfRef.test_copper__DOT__dut__DOT__copper_val[__VdlyDim0__test_copper__DOT__dut__DOT__copper_val__v1] 
            = __VdlyVal__test_copper__DOT__dut__DOT__copper_val__v1;
    }
    if (__VdlySet__test_copper__DOT__dut__DOT__regs__v0) {
        test_copper__DOT__dut__DOT__spr_shape[0U] = 0U;
        test_copper__DOT__dut__DOT__spr_trans[0U] = 0U;
        test_copper__DOT__dut__DOT__spr_y[0U] = 0U;
        test_copper__DOT__dut__DOT__spr_flip_h[0U] = 0U;
        test_copper__DOT__dut__DOT__spr_flip_v[0U] = 0U;
        test_copper__DOT__dut__DOT__spr_pri[0U] = 0U;
        test_copper__DOT__dut__DOT__spr_enable[0U] = 0U;
        test_copper__DOT__dut__DOT__spr_x[0U] = 0U;
        vlSelfRef.test_copper__DOT__dut__DOT__regs[0U] = 0U;
    }
    if (__VdlySet__test_copper__DOT__dut__DOT__regs__v1) {
        test_copper__DOT__dut__DOT__spr_shape[1U] = 0U;
        test_copper__DOT__dut__DOT__spr_shape[2U] = 0U;
        test_copper__DOT__dut__DOT__spr_shape[3U] = 0U;
        test_copper__DOT__dut__DOT__spr_shape[4U] = 0U;
        test_copper__DOT__dut__DOT__spr_shape[5U] = 0U;
        test_copper__DOT__dut__DOT__spr_shape[6U] = 0U;
        test_copper__DOT__dut__DOT__spr_shape[7U] = 0U;
        test_copper__DOT__dut__DOT__spr_shape[8U] = 0U;
        test_copper__DOT__dut__DOT__spr_shape[9U] = 0U;
        test_copper__DOT__dut__DOT__spr_shape[10U] = 0U;
        test_copper__DOT__dut__DOT__spr_shape[11U] = 0U;
        test_copper__DOT__dut__DOT__spr_shape[12U] = 0U;
        test_copper__DOT__dut__DOT__spr_shape[13U] = 0U;
        test_copper__DOT__dut__DOT__spr_shape[14U] = 0U;
        test_copper__DOT__dut__DOT__spr_shape[15U] = 0U;
        test_copper__DOT__dut__DOT__spr_trans[1U] = 0U;
        test_copper__DOT__dut__DOT__spr_trans[2U] = 0U;
        test_copper__DOT__dut__DOT__spr_trans[3U] = 0U;
        test_copper__DOT__dut__DOT__spr_trans[4U] = 0U;
        test_copper__DOT__dut__DOT__spr_trans[5U] = 0U;
        test_copper__DOT__dut__DOT__spr_trans[6U] = 0U;
        test_copper__DOT__dut__DOT__spr_trans[7U] = 0U;
        test_copper__DOT__dut__DOT__spr_trans[8U] = 0U;
        test_copper__DOT__dut__DOT__spr_trans[9U] = 0U;
        test_copper__DOT__dut__DOT__spr_trans[10U] = 0U;
        test_copper__DOT__dut__DOT__spr_trans[11U] = 0U;
        test_copper__DOT__dut__DOT__spr_trans[12U] = 0U;
        test_copper__DOT__dut__DOT__spr_trans[13U] = 0U;
        test_copper__DOT__dut__DOT__spr_trans[14U] = 0U;
        test_copper__DOT__dut__DOT__spr_trans[15U] = 0U;
        test_copper__DOT__dut__DOT__spr_y[1U] = 0U;
        test_copper__DOT__dut__DOT__spr_y[2U] = 0U;
        test_copper__DOT__dut__DOT__spr_y[3U] = 0U;
        test_copper__DOT__dut__DOT__spr_y[4U] = 0U;
        test_copper__DOT__dut__DOT__spr_y[5U] = 0U;
        test_copper__DOT__dut__DOT__spr_y[6U] = 0U;
        test_copper__DOT__dut__DOT__spr_y[7U] = 0U;
        test_copper__DOT__dut__DOT__spr_y[8U] = 0U;
        test_copper__DOT__dut__DOT__spr_y[9U] = 0U;
        test_copper__DOT__dut__DOT__spr_y[10U] = 0U;
        test_copper__DOT__dut__DOT__spr_y[11U] = 0U;
        test_copper__DOT__dut__DOT__spr_y[12U] = 0U;
        test_copper__DOT__dut__DOT__spr_y[13U] = 0U;
        test_copper__DOT__dut__DOT__spr_y[14U] = 0U;
        test_copper__DOT__dut__DOT__spr_y[15U] = 0U;
        test_copper__DOT__dut__DOT__spr_flip_h[1U] = 0U;
        test_copper__DOT__dut__DOT__spr_flip_h[2U] = 0U;
        test_copper__DOT__dut__DOT__spr_flip_h[3U] = 0U;
        test_copper__DOT__dut__DOT__spr_flip_h[4U] = 0U;
        test_copper__DOT__dut__DOT__spr_flip_h[5U] = 0U;
        test_copper__DOT__dut__DOT__spr_flip_h[6U] = 0U;
        test_copper__DOT__dut__DOT__spr_flip_h[7U] = 0U;
        test_copper__DOT__dut__DOT__spr_flip_h[8U] = 0U;
        test_copper__DOT__dut__DOT__spr_flip_h[9U] = 0U;
        test_copper__DOT__dut__DOT__spr_flip_h[10U] = 0U;
        test_copper__DOT__dut__DOT__spr_flip_h[11U] = 0U;
        test_copper__DOT__dut__DOT__spr_flip_h[12U] = 0U;
        test_copper__DOT__dut__DOT__spr_flip_h[13U] = 0U;
        test_copper__DOT__dut__DOT__spr_flip_h[14U] = 0U;
        test_copper__DOT__dut__DOT__spr_flip_h[15U] = 0U;
        test_copper__DOT__dut__DOT__spr_flip_v[1U] = 0U;
        test_copper__DOT__dut__DOT__spr_flip_v[2U] = 0U;
        test_copper__DOT__dut__DOT__spr_flip_v[3U] = 0U;
        test_copper__DOT__dut__DOT__spr_flip_v[4U] = 0U;
        test_copper__DOT__dut__DOT__spr_flip_v[5U] = 0U;
        test_copper__DOT__dut__DOT__spr_flip_v[6U] = 0U;
        test_copper__DOT__dut__DOT__spr_flip_v[7U] = 0U;
        test_copper__DOT__dut__DOT__spr_flip_v[8U] = 0U;
        test_copper__DOT__dut__DOT__spr_flip_v[9U] = 0U;
        test_copper__DOT__dut__DOT__spr_flip_v[10U] = 0U;
        test_copper__DOT__dut__DOT__spr_flip_v[11U] = 0U;
        test_copper__DOT__dut__DOT__spr_flip_v[12U] = 0U;
        test_copper__DOT__dut__DOT__spr_flip_v[13U] = 0U;
        test_copper__DOT__dut__DOT__spr_flip_v[14U] = 0U;
        test_copper__DOT__dut__DOT__spr_flip_v[15U] = 0U;
        test_copper__DOT__dut__DOT__spr_pri[1U] = 0U;
        test_copper__DOT__dut__DOT__spr_pri[2U] = 0U;
        test_copper__DOT__dut__DOT__spr_pri[3U] = 0U;
        test_copper__DOT__dut__DOT__spr_pri[4U] = 0U;
        test_copper__DOT__dut__DOT__spr_pri[5U] = 0U;
        test_copper__DOT__dut__DOT__spr_pri[6U] = 0U;
        test_copper__DOT__dut__DOT__spr_pri[7U] = 0U;
        test_copper__DOT__dut__DOT__spr_pri[8U] = 0U;
        test_copper__DOT__dut__DOT__spr_pri[9U] = 0U;
        test_copper__DOT__dut__DOT__spr_pri[10U] = 0U;
        test_copper__DOT__dut__DOT__spr_pri[11U] = 0U;
        test_copper__DOT__dut__DOT__spr_pri[12U] = 0U;
        test_copper__DOT__dut__DOT__spr_pri[13U] = 0U;
        test_copper__DOT__dut__DOT__spr_pri[14U] = 0U;
        test_copper__DOT__dut__DOT__spr_pri[15U] = 0U;
        test_copper__DOT__dut__DOT__spr_enable[1U] = 0U;
        test_copper__DOT__dut__DOT__spr_enable[2U] = 0U;
        test_copper__DOT__dut__DOT__spr_enable[3U] = 0U;
        test_copper__DOT__dut__DOT__spr_enable[4U] = 0U;
        test_copper__DOT__dut__DOT__spr_enable[5U] = 0U;
        test_copper__DOT__dut__DOT__spr_enable[6U] = 0U;
        test_copper__DOT__dut__DOT__spr_enable[7U] = 0U;
        test_copper__DOT__dut__DOT__spr_enable[8U] = 0U;
        test_copper__DOT__dut__DOT__spr_enable[9U] = 0U;
        test_copper__DOT__dut__DOT__spr_enable[10U] = 0U;
        test_copper__DOT__dut__DOT__spr_enable[11U] = 0U;
        test_copper__DOT__dut__DOT__spr_enable[12U] = 0U;
        test_copper__DOT__dut__DOT__spr_enable[13U] = 0U;
        test_copper__DOT__dut__DOT__spr_enable[14U] = 0U;
        test_copper__DOT__dut__DOT__spr_enable[15U] = 0U;
        test_copper__DOT__dut__DOT__spr_x[1U] = 0U;
        test_copper__DOT__dut__DOT__spr_x[2U] = 0U;
        test_copper__DOT__dut__DOT__spr_x[3U] = 0U;
        test_copper__DOT__dut__DOT__spr_x[4U] = 0U;
        test_copper__DOT__dut__DOT__spr_x[5U] = 0U;
        test_copper__DOT__dut__DOT__spr_x[6U] = 0U;
        test_copper__DOT__dut__DOT__spr_x[7U] = 0U;
        test_copper__DOT__dut__DOT__spr_x[8U] = 0U;
        test_copper__DOT__dut__DOT__spr_x[9U] = 0U;
        test_copper__DOT__dut__DOT__spr_x[10U] = 0U;
        test_copper__DOT__dut__DOT__spr_x[11U] = 0U;
        test_copper__DOT__dut__DOT__spr_x[12U] = 0U;
        test_copper__DOT__dut__DOT__spr_x[13U] = 0U;
        test_copper__DOT__dut__DOT__spr_x[14U] = 0U;
        test_copper__DOT__dut__DOT__spr_x[15U] = 0U;
        vlSelfRef.test_copper__DOT__dut__DOT__regs[1U] = 0U;
        vlSelfRef.test_copper__DOT__dut__DOT__regs[2U] = 0U;
        vlSelfRef.test_copper__DOT__dut__DOT__regs[3U] = 0U;
        vlSelfRef.test_copper__DOT__dut__DOT__regs[4U] = 0U;
        vlSelfRef.test_copper__DOT__dut__DOT__regs[5U] = 0U;
        vlSelfRef.test_copper__DOT__dut__DOT__regs[6U] = 0U;
        vlSelfRef.test_copper__DOT__dut__DOT__regs[7U] = 0U;
        vlSelfRef.test_copper__DOT__dut__DOT__regs[8U] = 0U;
        vlSelfRef.test_copper__DOT__dut__DOT__regs[9U] = 0U;
        vlSelfRef.test_copper__DOT__dut__DOT__regs[10U] = 0U;
        vlSelfRef.test_copper__DOT__dut__DOT__regs[11U] = 0U;
        vlSelfRef.test_copper__DOT__dut__DOT__regs[12U] = 0U;
        vlSelfRef.test_copper__DOT__dut__DOT__regs[13U] = 0U;
        vlSelfRef.test_copper__DOT__dut__DOT__regs[14U] = 0U;
        vlSelfRef.test_copper__DOT__dut__DOT__regs[15U] = 0U;
        vlSelfRef.test_copper__DOT__dut__DOT__regs[16U] = 0U;
        vlSelfRef.test_copper__DOT__dut__DOT__regs[17U] = 0U;
        vlSelfRef.test_copper__DOT__dut__DOT__regs[18U] = 0U;
        vlSelfRef.test_copper__DOT__dut__DOT__regs[19U] = 0U;
        vlSelfRef.test_copper__DOT__dut__DOT__regs[20U] = 0U;
        vlSelfRef.test_copper__DOT__dut__DOT__regs[21U] = 0U;
        vlSelfRef.test_copper__DOT__dut__DOT__regs[22U] = 0U;
        vlSelfRef.test_copper__DOT__dut__DOT__regs[23U] = 0U;
        vlSelfRef.test_copper__DOT__dut__DOT__regs[24U] = 0U;
        vlSelfRef.test_copper__DOT__dut__DOT__regs[25U] = 0U;
        vlSelfRef.test_copper__DOT__dut__DOT__regs[26U] = 0U;
        vlSelfRef.test_copper__DOT__dut__DOT__regs[27U] = 0U;
        vlSelfRef.test_copper__DOT__dut__DOT__regs[28U] = 0U;
        vlSelfRef.test_copper__DOT__dut__DOT__regs[29U] = 0U;
        vlSelfRef.test_copper__DOT__dut__DOT__regs[30U] = 0U;
        vlSelfRef.test_copper__DOT__dut__DOT__regs[31U] = 0U;
    }
    if (__VdlySet__test_copper__DOT__dut__DOT__spr_shape__v16) {
        test_copper__DOT__dut__DOT__spr_shape[__VdlyDim0__test_copper__DOT__dut__DOT__spr_shape__v16] 
            = __VdlyVal__test_copper__DOT__dut__DOT__spr_shape__v16;
    }
    if (__VdlySet__test_copper__DOT__dut__DOT__spr_trans__v16) {
        test_copper__DOT__dut__DOT__spr_trans[__VdlyDim0__test_copper__DOT__dut__DOT__spr_trans__v16] 
            = __VdlyVal__test_copper__DOT__dut__DOT__spr_trans__v16;
    }
    if (__VdlySet__test_copper__DOT__dut__DOT__spr_x__v16) {
        test_copper__DOT__dut__DOT__spr_y[__VdlyDim0__test_copper__DOT__dut__DOT__spr_y__v16] 
            = __VdlyVal__test_copper__DOT__dut__DOT__spr_y__v16;
        test_copper__DOT__dut__DOT__spr_x[__VdlyDim0__test_copper__DOT__dut__DOT__spr_x__v16] 
            = __VdlyVal__test_copper__DOT__dut__DOT__spr_x__v16;
    }
    if (__VdlySet__test_copper__DOT__dut__DOT__spr_y__v17) {
        test_copper__DOT__dut__DOT__spr_y[__VdlyDim0__test_copper__DOT__dut__DOT__spr_y__v17] 
            = __VdlyVal__test_copper__DOT__dut__DOT__spr_y__v17;
    }
    if (__VdlySet__test_copper__DOT__dut__DOT__spr_flip_h__v16) {
        test_copper__DOT__dut__DOT__spr_flip_h[__VdlyDim0__test_copper__DOT__dut__DOT__spr_flip_h__v16] 
            = __VdlyVal__test_copper__DOT__dut__DOT__spr_flip_h__v16;
        test_copper__DOT__dut__DOT__spr_flip_v[__VdlyDim0__test_copper__DOT__dut__DOT__spr_flip_v__v16] 
            = __VdlyVal__test_copper__DOT__dut__DOT__spr_flip_v__v16;
    }
    if (__VdlySet__test_copper__DOT__dut__DOT__spr_pri__v16) {
        test_copper__DOT__dut__DOT__spr_pri[__VdlyDim0__test_copper__DOT__dut__DOT__spr_pri__v16] 
            = __VdlyVal__test_copper__DOT__dut__DOT__spr_pri__v16;
    }
    if (__VdlySet__test_copper__DOT__dut__DOT__spr_pri__v17) {
        test_copper__DOT__dut__DOT__spr_pri[__VdlyDim0__test_copper__DOT__dut__DOT__spr_pri__v17] 
            = __VdlyVal__test_copper__DOT__dut__DOT__spr_pri__v17;
    }
    if (__VdlySet__test_copper__DOT__dut__DOT__spr_enable__v16) {
        test_copper__DOT__dut__DOT__spr_enable[__VdlyDim0__test_copper__DOT__dut__DOT__spr_enable__v16] = 0U;
    }
    if (__VdlySet__test_copper__DOT__dut__DOT__spr_enable__v17) {
        test_copper__DOT__dut__DOT__spr_enable[__VdlyDim0__test_copper__DOT__dut__DOT__spr_enable__v17] = 1U;
    }
    if (__VdlySet__test_copper__DOT__dut__DOT__spr_flip_h__v17) {
        test_copper__DOT__dut__DOT__spr_flip_h[__VdlyDim0__test_copper__DOT__dut__DOT__spr_flip_h__v17] 
            = __VdlyVal__test_copper__DOT__dut__DOT__spr_flip_h__v17;
        test_copper__DOT__dut__DOT__spr_flip_v[__VdlyDim0__test_copper__DOT__dut__DOT__spr_flip_v__v17] 
            = __VdlyVal__test_copper__DOT__dut__DOT__spr_flip_v__v17;
        test_copper__DOT__dut__DOT__spr_enable[__VdlyDim0__test_copper__DOT__dut__DOT__spr_enable__v18] 
            = __VdlyVal__test_copper__DOT__dut__DOT__spr_enable__v18;
    }
    if (__VdlySet__test_copper__DOT__dut__DOT__spr_x__v17) {
        test_copper__DOT__dut__DOT__spr_x[__VdlyDim0__test_copper__DOT__dut__DOT__spr_x__v17] 
            = ((0x00ffU & test_copper__DOT__dut__DOT__spr_x
                [__VdlyDim0__test_copper__DOT__dut__DOT__spr_x__v17]) 
               | ((IData)(__VdlyVal__test_copper__DOT__dut__DOT__spr_x__v17) 
                  << 8U));
    }
    if (__VdlySet__test_copper__DOT__dut__DOT__spr_x__v18) {
        test_copper__DOT__dut__DOT__spr_x[__VdlyDim0__test_copper__DOT__dut__DOT__spr_x__v18] 
            = ((0x0100U & test_copper__DOT__dut__DOT__spr_x
                [__VdlyDim0__test_copper__DOT__dut__DOT__spr_x__v18]) 
               | (IData)(__VdlyVal__test_copper__DOT__dut__DOT__spr_x__v18));
    }
    if (__VdlySet__test_copper__DOT__dut__DOT__regs__v32) {
        vlSelfRef.test_copper__DOT__dut__DOT__regs[20U] 
            = __VdlyVal__test_copper__DOT__dut__DOT__regs__v32;
    }
    if (__VdlySet__test_copper__DOT__dut__DOT__regs__v33) {
        vlSelfRef.test_copper__DOT__dut__DOT__regs[20U] 
            = __VdlyVal__test_copper__DOT__dut__DOT__regs__v33;
    }
    if (__VdlySet__test_copper__DOT__dut__DOT__regs__v34) {
        vlSelfRef.test_copper__DOT__dut__DOT__regs[20U] 
            = __VdlyVal__test_copper__DOT__dut__DOT__regs__v34;
    }
    if (__VdlySet__test_copper__DOT__dut__DOT__regs__v35) {
        vlSelfRef.test_copper__DOT__dut__DOT__regs[20U] 
            = __VdlyVal__test_copper__DOT__dut__DOT__regs__v35;
    }
    if (__VdlySet__test_copper__DOT__dut__DOT__regs__v36) {
        vlSelfRef.test_copper__DOT__dut__DOT__regs[20U] 
            = __VdlyVal__test_copper__DOT__dut__DOT__regs__v36;
    }
    if (__VdlySet__test_copper__DOT__dut__DOT__regs__v37) {
        vlSelfRef.test_copper__DOT__dut__DOT__regs[__VdlyDim0__test_copper__DOT__dut__DOT__regs__v37] 
            = __VdlyVal__test_copper__DOT__dut__DOT__regs__v37;
    }
    if (__VdlySet__test_copper__DOT__dut__DOT__regs__v38) {
        vlSelfRef.test_copper__DOT__dut__DOT__regs[__VdlyDim0__test_copper__DOT__dut__DOT__regs__v38] 
            = __VdlyVal__test_copper__DOT__dut__DOT__regs__v38;
    }
    vlSelfRef.test_copper__DOT__dut__DOT__copper_pending_list 
        = __Vdly__test_copper__DOT__dut__DOT__copper_pending_list;
    vlSelfRef.test_copper__DOT__dut__DOT__copper_active_list 
        = __Vdly__test_copper__DOT__dut__DOT__copper_active_list;
    vlSelfRef.test_copper__DOT__dut__DOT__copper_loading 
        = __Vdly__test_copper__DOT__dut__DOT__copper_loading;
    vlSelfRef.test_copper__DOT__dut__DOT__h_count = __Vdly__test_copper__DOT__dut__DOT__h_count;
    vlSelfRef.test_copper__DOT__dut__DOT__v_count = __Vdly__test_copper__DOT__dut__DOT__v_count;
    test_copper__DOT__dut__DOT__visible = ((0x0280U 
                                            > (IData)(vlSelfRef.test_copper__DOT__dut__DOT__h_count)) 
                                           & (0x01e0U 
                                              > (IData)(vlSelfRef.test_copper__DOT__dut__DOT__v_count)));
    test_copper__DOT__dut__DOT__text_line = (0x000003ffU 
                                             & (((IData)(vlSelfRef.test_copper__DOT__dut__DOT__v_count) 
                                                 - (IData)(0x00000028U)) 
                                                >> 1U));
    test_copper__DOT__dut__DOT__text_row = (0x0000001fU 
                                            & ((IData)(test_copper__DOT__dut__DOT__text_line) 
                                               >> 3U));
    test_copper__DOT__dut__DOT__real_row = (0x0000001fU 
                                            & ((IData)(test_copper__DOT__dut__DOT__text_row) 
                                               + (IData)(vlSelfRef.test_copper__DOT__dut__DOT__scroll_offset)));
    if ((0x19U <= (IData)(test_copper__DOT__dut__DOT__real_row))) {
        test_copper__DOT__dut__DOT__real_row = (0x0000001fU 
                                                & ((IData)(test_copper__DOT__dut__DOT__real_row) 
                                                   - (IData)(0x19U)));
    }
    vlSelfRef.test_copper__DOT__dut__DOT__gfx_y = (0x000000ffU 
                                                   & (IData)(test_copper__DOT__dut__DOT__text_line));
    vlSelfRef.test_copper__DOT__dut__DOT__in_text_area 
        = ((IData)(test_copper__DOT__dut__DOT__visible) 
           & ((0x0028U <= (IData)(vlSelfRef.test_copper__DOT__dut__DOT__v_count)) 
              & (0x01b8U > (IData)(vlSelfRef.test_copper__DOT__dut__DOT__v_count))));
}

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

#ifdef VL_DEBUG
VL_ATTR_COLD void Vtest_copper___024root___dump_triggers__act(const VlUnpacked<QData/*63:0*/, 1> &triggers, const std::string &tag);
#endif  // VL_DEBUG

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
