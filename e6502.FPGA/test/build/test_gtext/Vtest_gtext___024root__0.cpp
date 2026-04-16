// Verilated -*- C++ -*-
// DESCRIPTION: Verilator output: Design implementation internals
// See Vtest_gtext.h for the primary calling header

#include "Vtest_gtext__pch.h"

VL_ATTR_COLD void Vtest_gtext___024root___eval_initial__TOP(Vtest_gtext___024root* vlSelf);
VlCoroutine Vtest_gtext___024root___eval_initial__TOP__Vtiming__0(Vtest_gtext___024root* vlSelf);
VlCoroutine Vtest_gtext___024root___eval_initial__TOP__Vtiming__1(Vtest_gtext___024root* vlSelf);

void Vtest_gtext___024root___eval_initial(Vtest_gtext___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vtest_gtext___024root___eval_initial\n"); );
    Vtest_gtext__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
    // Body
    Vtest_gtext___024root___eval_initial__TOP(vlSelf);
    Vtest_gtext___024root___eval_initial__TOP__Vtiming__0(vlSelf);
    Vtest_gtext___024root___eval_initial__TOP__Vtiming__1(vlSelf);
}

VlCoroutine Vtest_gtext___024root___eval_initial__TOP__Vtiming__0__0(Vtest_gtext___024root* vlSelf);
VlCoroutine Vtest_gtext___024root___eval_initial__TOP__Vtiming__0__1(Vtest_gtext___024root* vlSelf);

VlCoroutine Vtest_gtext___024root___eval_initial__TOP__Vtiming__0(Vtest_gtext___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vtest_gtext___024root___eval_initial__TOP__Vtiming__0\n"); );
    Vtest_gtext__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
    // Body
    co_await Vtest_gtext___024root___eval_initial__TOP__Vtiming__0__0(vlSelf);
    co_await Vtest_gtext___024root___eval_initial__TOP__Vtiming__0__1(vlSelf);
    co_return;
}

void Vtest_gtext___024root____VbeforeTrig_h8248a36b__0(Vtest_gtext___024root* vlSelf, const char* __VeventDescription);

VlCoroutine Vtest_gtext___024root___eval_initial__TOP__Vtiming__0__0(Vtest_gtext___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vtest_gtext___024root___eval_initial__TOP__Vtiming__0__0\n"); );
    Vtest_gtext__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
    // Locals
    IData/*31:0*/ test_gtext__DOT__unnamedblk1_2__DOT____Vrepeat1;
    test_gtext__DOT__unnamedblk1_2__DOT____Vrepeat1 = 0;
    IData/*31:0*/ test_gtext__DOT__unnamedblk1_3__DOT____Vrepeat2;
    test_gtext__DOT__unnamedblk1_3__DOT____Vrepeat2 = 0;
    IData/*31:0*/ test_gtext__DOT__unnamedblk1_4__DOT____Vrepeat3;
    test_gtext__DOT__unnamedblk1_4__DOT____Vrepeat3 = 0;
    SData/*15:0*/ __Vtask_test_gtext__DOT__write_reg__0__addr;
    __Vtask_test_gtext__DOT__write_reg__0__addr = 0;
    CData/*7:0*/ __Vtask_test_gtext__DOT__write_reg__0__data;
    __Vtask_test_gtext__DOT__write_reg__0__data = 0;
    IData/*31:0*/ __Vtask_test_gtext__DOT__write_param__1__idx;
    __Vtask_test_gtext__DOT__write_param__1__idx = 0;
    CData/*7:0*/ __Vtask_test_gtext__DOT__write_param__1__data;
    __Vtask_test_gtext__DOT__write_param__1__data = 0;
    SData/*15:0*/ __Vtask_test_gtext__DOT__write_reg__2__addr;
    __Vtask_test_gtext__DOT__write_reg__2__addr = 0;
    CData/*7:0*/ __Vtask_test_gtext__DOT__write_reg__2__data;
    __Vtask_test_gtext__DOT__write_reg__2__data = 0;
    IData/*31:0*/ __Vtask_test_gtext__DOT__write_param__3__idx;
    __Vtask_test_gtext__DOT__write_param__3__idx = 0;
    CData/*7:0*/ __Vtask_test_gtext__DOT__write_param__3__data;
    __Vtask_test_gtext__DOT__write_param__3__data = 0;
    SData/*15:0*/ __Vtask_test_gtext__DOT__write_reg__4__addr;
    __Vtask_test_gtext__DOT__write_reg__4__addr = 0;
    CData/*7:0*/ __Vtask_test_gtext__DOT__write_reg__4__data;
    __Vtask_test_gtext__DOT__write_reg__4__data = 0;
    IData/*31:0*/ __Vtask_test_gtext__DOT__write_param__5__idx;
    __Vtask_test_gtext__DOT__write_param__5__idx = 0;
    CData/*7:0*/ __Vtask_test_gtext__DOT__write_param__5__data;
    __Vtask_test_gtext__DOT__write_param__5__data = 0;
    SData/*15:0*/ __Vtask_test_gtext__DOT__write_reg__6__addr;
    __Vtask_test_gtext__DOT__write_reg__6__addr = 0;
    CData/*7:0*/ __Vtask_test_gtext__DOT__write_reg__6__data;
    __Vtask_test_gtext__DOT__write_reg__6__data = 0;
    IData/*31:0*/ __Vtask_test_gtext__DOT__write_param__7__idx;
    __Vtask_test_gtext__DOT__write_param__7__idx = 0;
    CData/*7:0*/ __Vtask_test_gtext__DOT__write_param__7__data;
    __Vtask_test_gtext__DOT__write_param__7__data = 0;
    SData/*15:0*/ __Vtask_test_gtext__DOT__write_reg__8__addr;
    __Vtask_test_gtext__DOT__write_reg__8__addr = 0;
    CData/*7:0*/ __Vtask_test_gtext__DOT__write_reg__8__data;
    __Vtask_test_gtext__DOT__write_reg__8__data = 0;
    IData/*31:0*/ __Vtask_test_gtext__DOT__write_param__9__idx;
    __Vtask_test_gtext__DOT__write_param__9__idx = 0;
    CData/*7:0*/ __Vtask_test_gtext__DOT__write_param__9__data;
    __Vtask_test_gtext__DOT__write_param__9__data = 0;
    SData/*15:0*/ __Vtask_test_gtext__DOT__write_reg__10__addr;
    __Vtask_test_gtext__DOT__write_reg__10__addr = 0;
    CData/*7:0*/ __Vtask_test_gtext__DOT__write_reg__10__data;
    __Vtask_test_gtext__DOT__write_reg__10__data = 0;
    CData/*7:0*/ __Vtask_test_gtext__DOT__write_cmd__11__cmd;
    __Vtask_test_gtext__DOT__write_cmd__11__cmd = 0;
    SData/*15:0*/ __Vtask_test_gtext__DOT__write_reg__12__addr;
    __Vtask_test_gtext__DOT__write_reg__12__addr = 0;
    CData/*7:0*/ __Vtask_test_gtext__DOT__write_reg__12__data;
    __Vtask_test_gtext__DOT__write_reg__12__data = 0;
    CData/*0:0*/ __Vtask_test_gtext__DOT__check__13__condition;
    __Vtask_test_gtext__DOT__check__13__condition = 0;
    CData/*7:0*/ __Vtask_test_gtext__DOT__write_cmd__14__cmd;
    __Vtask_test_gtext__DOT__write_cmd__14__cmd = 0;
    SData/*15:0*/ __Vtask_test_gtext__DOT__write_reg__15__addr;
    __Vtask_test_gtext__DOT__write_reg__15__addr = 0;
    CData/*7:0*/ __Vtask_test_gtext__DOT__write_reg__15__data;
    __Vtask_test_gtext__DOT__write_reg__15__data = 0;
    IData/*31:0*/ __Vtask_test_gtext__DOT__wait_cmd_done__16__test_gtext__DOT__unnamedblk1_1__DOT____Vrepeat0;
    __Vtask_test_gtext__DOT__wait_cmd_done__16__test_gtext__DOT__unnamedblk1_1__DOT____Vrepeat0 = 0;
    IData/*31:0*/ __Vtask_test_gtext__DOT__wait_cmd_done__16__timeout;
    __Vtask_test_gtext__DOT__wait_cmd_done__16__timeout = 0;
    IData/*31:0*/ __Vtask_test_gtext__DOT__write_param__17__idx;
    __Vtask_test_gtext__DOT__write_param__17__idx = 0;
    CData/*7:0*/ __Vtask_test_gtext__DOT__write_param__17__data;
    __Vtask_test_gtext__DOT__write_param__17__data = 0;
    SData/*15:0*/ __Vtask_test_gtext__DOT__write_reg__18__addr;
    __Vtask_test_gtext__DOT__write_reg__18__addr = 0;
    CData/*7:0*/ __Vtask_test_gtext__DOT__write_reg__18__data;
    __Vtask_test_gtext__DOT__write_reg__18__data = 0;
    CData/*7:0*/ __Vtask_test_gtext__DOT__write_cmd__19__cmd;
    __Vtask_test_gtext__DOT__write_cmd__19__cmd = 0;
    SData/*15:0*/ __Vtask_test_gtext__DOT__write_reg__20__addr;
    __Vtask_test_gtext__DOT__write_reg__20__addr = 0;
    CData/*7:0*/ __Vtask_test_gtext__DOT__write_reg__20__data;
    __Vtask_test_gtext__DOT__write_reg__20__data = 0;
    IData/*31:0*/ __Vtask_test_gtext__DOT__set_fio_string__21__i;
    __Vtask_test_gtext__DOT__set_fio_string__21__i = 0;
    SData/*15:0*/ __Vtask_test_gtext__DOT__write_reg__22__addr;
    __Vtask_test_gtext__DOT__write_reg__22__addr = 0;
    CData/*7:0*/ __Vtask_test_gtext__DOT__write_reg__22__data;
    __Vtask_test_gtext__DOT__write_reg__22__data = 0;
    SData/*15:0*/ __Vtask_test_gtext__DOT__write_reg__23__addr;
    __Vtask_test_gtext__DOT__write_reg__23__addr = 0;
    CData/*7:0*/ __Vtask_test_gtext__DOT__write_reg__23__data;
    __Vtask_test_gtext__DOT__write_reg__23__data = 0;
    IData/*31:0*/ __Vtask_test_gtext__DOT__write_param__24__idx;
    __Vtask_test_gtext__DOT__write_param__24__idx = 0;
    CData/*7:0*/ __Vtask_test_gtext__DOT__write_param__24__data;
    __Vtask_test_gtext__DOT__write_param__24__data = 0;
    SData/*15:0*/ __Vtask_test_gtext__DOT__write_reg__25__addr;
    __Vtask_test_gtext__DOT__write_reg__25__addr = 0;
    CData/*7:0*/ __Vtask_test_gtext__DOT__write_reg__25__data;
    __Vtask_test_gtext__DOT__write_reg__25__data = 0;
    IData/*31:0*/ __Vtask_test_gtext__DOT__write_param__26__idx;
    __Vtask_test_gtext__DOT__write_param__26__idx = 0;
    CData/*7:0*/ __Vtask_test_gtext__DOT__write_param__26__data;
    __Vtask_test_gtext__DOT__write_param__26__data = 0;
    SData/*15:0*/ __Vtask_test_gtext__DOT__write_reg__27__addr;
    __Vtask_test_gtext__DOT__write_reg__27__addr = 0;
    CData/*7:0*/ __Vtask_test_gtext__DOT__write_reg__27__data;
    __Vtask_test_gtext__DOT__write_reg__27__data = 0;
    IData/*31:0*/ __Vtask_test_gtext__DOT__write_param__28__idx;
    __Vtask_test_gtext__DOT__write_param__28__idx = 0;
    CData/*7:0*/ __Vtask_test_gtext__DOT__write_param__28__data;
    __Vtask_test_gtext__DOT__write_param__28__data = 0;
    SData/*15:0*/ __Vtask_test_gtext__DOT__write_reg__29__addr;
    __Vtask_test_gtext__DOT__write_reg__29__addr = 0;
    CData/*7:0*/ __Vtask_test_gtext__DOT__write_reg__29__data;
    __Vtask_test_gtext__DOT__write_reg__29__data = 0;
    IData/*31:0*/ __Vtask_test_gtext__DOT__write_param__30__idx;
    __Vtask_test_gtext__DOT__write_param__30__idx = 0;
    CData/*7:0*/ __Vtask_test_gtext__DOT__write_param__30__data;
    __Vtask_test_gtext__DOT__write_param__30__data = 0;
    SData/*15:0*/ __Vtask_test_gtext__DOT__write_reg__31__addr;
    __Vtask_test_gtext__DOT__write_reg__31__addr = 0;
    CData/*7:0*/ __Vtask_test_gtext__DOT__write_reg__31__data;
    __Vtask_test_gtext__DOT__write_reg__31__data = 0;
    IData/*31:0*/ __Vtask_test_gtext__DOT__write_param__32__idx;
    __Vtask_test_gtext__DOT__write_param__32__idx = 0;
    CData/*7:0*/ __Vtask_test_gtext__DOT__write_param__32__data;
    __Vtask_test_gtext__DOT__write_param__32__data = 0;
    SData/*15:0*/ __Vtask_test_gtext__DOT__write_reg__33__addr;
    __Vtask_test_gtext__DOT__write_reg__33__addr = 0;
    CData/*7:0*/ __Vtask_test_gtext__DOT__write_reg__33__data;
    __Vtask_test_gtext__DOT__write_reg__33__data = 0;
    IData/*31:0*/ __Vtask_test_gtext__DOT__write_param__34__idx;
    __Vtask_test_gtext__DOT__write_param__34__idx = 0;
    CData/*7:0*/ __Vtask_test_gtext__DOT__write_param__34__data;
    __Vtask_test_gtext__DOT__write_param__34__data = 0;
    SData/*15:0*/ __Vtask_test_gtext__DOT__write_reg__35__addr;
    __Vtask_test_gtext__DOT__write_reg__35__addr = 0;
    CData/*7:0*/ __Vtask_test_gtext__DOT__write_reg__35__data;
    __Vtask_test_gtext__DOT__write_reg__35__data = 0;
    CData/*7:0*/ __Vtask_test_gtext__DOT__write_cmd__36__cmd;
    __Vtask_test_gtext__DOT__write_cmd__36__cmd = 0;
    SData/*15:0*/ __Vtask_test_gtext__DOT__write_reg__37__addr;
    __Vtask_test_gtext__DOT__write_reg__37__addr = 0;
    CData/*7:0*/ __Vtask_test_gtext__DOT__write_reg__37__data;
    __Vtask_test_gtext__DOT__write_reg__37__data = 0;
    IData/*31:0*/ __Vtask_test_gtext__DOT__wait_cmd_done__38__test_gtext__DOT__unnamedblk1_1__DOT____Vrepeat0;
    __Vtask_test_gtext__DOT__wait_cmd_done__38__test_gtext__DOT__unnamedblk1_1__DOT____Vrepeat0 = 0;
    IData/*31:0*/ __Vtask_test_gtext__DOT__wait_cmd_done__38__timeout;
    __Vtask_test_gtext__DOT__wait_cmd_done__38__timeout = 0;
    CData/*3:0*/ __Vfunc_test_gtext__DOT__gfx_pixel__39__Vfuncout;
    __Vfunc_test_gtext__DOT__gfx_pixel__39__Vfuncout = 0;
    IData/*31:0*/ __Vfunc_test_gtext__DOT__gfx_pixel__39__x;
    __Vfunc_test_gtext__DOT__gfx_pixel__39__x = 0;
    IData/*31:0*/ __Vfunc_test_gtext__DOT__gfx_pixel__39__y;
    __Vfunc_test_gtext__DOT__gfx_pixel__39__y = 0;
    CData/*0:0*/ __Vtask_test_gtext__DOT__check__40__condition;
    __Vtask_test_gtext__DOT__check__40__condition = 0;
    CData/*0:0*/ __Vtask_test_gtext__DOT__check__41__condition;
    __Vtask_test_gtext__DOT__check__41__condition = 0;
    CData/*3:0*/ __Vfunc_test_gtext__DOT__gfx_pixel__42__Vfuncout;
    __Vfunc_test_gtext__DOT__gfx_pixel__42__Vfuncout = 0;
    IData/*31:0*/ __Vfunc_test_gtext__DOT__gfx_pixel__42__x;
    __Vfunc_test_gtext__DOT__gfx_pixel__42__x = 0;
    IData/*31:0*/ __Vfunc_test_gtext__DOT__gfx_pixel__42__y;
    __Vfunc_test_gtext__DOT__gfx_pixel__42__y = 0;
    CData/*3:0*/ __Vfunc_test_gtext__DOT__gfx_pixel__43__Vfuncout;
    __Vfunc_test_gtext__DOT__gfx_pixel__43__Vfuncout = 0;
    IData/*31:0*/ __Vfunc_test_gtext__DOT__gfx_pixel__43__x;
    __Vfunc_test_gtext__DOT__gfx_pixel__43__x = 0;
    IData/*31:0*/ __Vfunc_test_gtext__DOT__gfx_pixel__43__y;
    __Vfunc_test_gtext__DOT__gfx_pixel__43__y = 0;
    CData/*0:0*/ __Vtask_test_gtext__DOT__check__44__condition;
    __Vtask_test_gtext__DOT__check__44__condition = 0;
    CData/*7:0*/ __Vtask_test_gtext__DOT__write_cmd__45__cmd;
    __Vtask_test_gtext__DOT__write_cmd__45__cmd = 0;
    SData/*15:0*/ __Vtask_test_gtext__DOT__write_reg__46__addr;
    __Vtask_test_gtext__DOT__write_reg__46__addr = 0;
    CData/*7:0*/ __Vtask_test_gtext__DOT__write_reg__46__data;
    __Vtask_test_gtext__DOT__write_reg__46__data = 0;
    IData/*31:0*/ __Vtask_test_gtext__DOT__wait_cmd_done__47__test_gtext__DOT__unnamedblk1_1__DOT____Vrepeat0;
    __Vtask_test_gtext__DOT__wait_cmd_done__47__test_gtext__DOT__unnamedblk1_1__DOT____Vrepeat0 = 0;
    IData/*31:0*/ __Vtask_test_gtext__DOT__wait_cmd_done__47__timeout;
    __Vtask_test_gtext__DOT__wait_cmd_done__47__timeout = 0;
    IData/*31:0*/ __Vtask_test_gtext__DOT__set_fio_string__48__i;
    __Vtask_test_gtext__DOT__set_fio_string__48__i = 0;
    SData/*15:0*/ __Vtask_test_gtext__DOT__write_reg__49__addr;
    __Vtask_test_gtext__DOT__write_reg__49__addr = 0;
    CData/*7:0*/ __Vtask_test_gtext__DOT__write_reg__49__data;
    __Vtask_test_gtext__DOT__write_reg__49__data = 0;
    SData/*15:0*/ __Vtask_test_gtext__DOT__write_reg__50__addr;
    __Vtask_test_gtext__DOT__write_reg__50__addr = 0;
    CData/*7:0*/ __Vtask_test_gtext__DOT__write_reg__50__data;
    __Vtask_test_gtext__DOT__write_reg__50__data = 0;
    IData/*31:0*/ __Vtask_test_gtext__DOT__write_param__51__idx;
    __Vtask_test_gtext__DOT__write_param__51__idx = 0;
    CData/*7:0*/ __Vtask_test_gtext__DOT__write_param__51__data;
    __Vtask_test_gtext__DOT__write_param__51__data = 0;
    SData/*15:0*/ __Vtask_test_gtext__DOT__write_reg__52__addr;
    __Vtask_test_gtext__DOT__write_reg__52__addr = 0;
    CData/*7:0*/ __Vtask_test_gtext__DOT__write_reg__52__data;
    __Vtask_test_gtext__DOT__write_reg__52__data = 0;
    IData/*31:0*/ __Vtask_test_gtext__DOT__write_param__53__idx;
    __Vtask_test_gtext__DOT__write_param__53__idx = 0;
    CData/*7:0*/ __Vtask_test_gtext__DOT__write_param__53__data;
    __Vtask_test_gtext__DOT__write_param__53__data = 0;
    SData/*15:0*/ __Vtask_test_gtext__DOT__write_reg__54__addr;
    __Vtask_test_gtext__DOT__write_reg__54__addr = 0;
    CData/*7:0*/ __Vtask_test_gtext__DOT__write_reg__54__data;
    __Vtask_test_gtext__DOT__write_reg__54__data = 0;
    IData/*31:0*/ __Vtask_test_gtext__DOT__write_param__55__idx;
    __Vtask_test_gtext__DOT__write_param__55__idx = 0;
    CData/*7:0*/ __Vtask_test_gtext__DOT__write_param__55__data;
    __Vtask_test_gtext__DOT__write_param__55__data = 0;
    SData/*15:0*/ __Vtask_test_gtext__DOT__write_reg__56__addr;
    __Vtask_test_gtext__DOT__write_reg__56__addr = 0;
    CData/*7:0*/ __Vtask_test_gtext__DOT__write_reg__56__data;
    __Vtask_test_gtext__DOT__write_reg__56__data = 0;
    IData/*31:0*/ __Vtask_test_gtext__DOT__write_param__57__idx;
    __Vtask_test_gtext__DOT__write_param__57__idx = 0;
    CData/*7:0*/ __Vtask_test_gtext__DOT__write_param__57__data;
    __Vtask_test_gtext__DOT__write_param__57__data = 0;
    SData/*15:0*/ __Vtask_test_gtext__DOT__write_reg__58__addr;
    __Vtask_test_gtext__DOT__write_reg__58__addr = 0;
    CData/*7:0*/ __Vtask_test_gtext__DOT__write_reg__58__data;
    __Vtask_test_gtext__DOT__write_reg__58__data = 0;
    IData/*31:0*/ __Vtask_test_gtext__DOT__write_param__59__idx;
    __Vtask_test_gtext__DOT__write_param__59__idx = 0;
    CData/*7:0*/ __Vtask_test_gtext__DOT__write_param__59__data;
    __Vtask_test_gtext__DOT__write_param__59__data = 0;
    SData/*15:0*/ __Vtask_test_gtext__DOT__write_reg__60__addr;
    __Vtask_test_gtext__DOT__write_reg__60__addr = 0;
    CData/*7:0*/ __Vtask_test_gtext__DOT__write_reg__60__data;
    __Vtask_test_gtext__DOT__write_reg__60__data = 0;
    IData/*31:0*/ __Vtask_test_gtext__DOT__write_param__61__idx;
    __Vtask_test_gtext__DOT__write_param__61__idx = 0;
    CData/*7:0*/ __Vtask_test_gtext__DOT__write_param__61__data;
    __Vtask_test_gtext__DOT__write_param__61__data = 0;
    SData/*15:0*/ __Vtask_test_gtext__DOT__write_reg__62__addr;
    __Vtask_test_gtext__DOT__write_reg__62__addr = 0;
    CData/*7:0*/ __Vtask_test_gtext__DOT__write_reg__62__data;
    __Vtask_test_gtext__DOT__write_reg__62__data = 0;
    CData/*7:0*/ __Vtask_test_gtext__DOT__write_cmd__63__cmd;
    __Vtask_test_gtext__DOT__write_cmd__63__cmd = 0;
    SData/*15:0*/ __Vtask_test_gtext__DOT__write_reg__64__addr;
    __Vtask_test_gtext__DOT__write_reg__64__addr = 0;
    CData/*7:0*/ __Vtask_test_gtext__DOT__write_reg__64__data;
    __Vtask_test_gtext__DOT__write_reg__64__data = 0;
    IData/*31:0*/ __Vtask_test_gtext__DOT__wait_cmd_done__65__test_gtext__DOT__unnamedblk1_1__DOT____Vrepeat0;
    __Vtask_test_gtext__DOT__wait_cmd_done__65__test_gtext__DOT__unnamedblk1_1__DOT____Vrepeat0 = 0;
    IData/*31:0*/ __Vtask_test_gtext__DOT__wait_cmd_done__65__timeout;
    __Vtask_test_gtext__DOT__wait_cmd_done__65__timeout = 0;
    CData/*0:0*/ __Vtask_test_gtext__DOT__check__66__condition;
    __Vtask_test_gtext__DOT__check__66__condition = 0;
    CData/*3:0*/ __Vfunc_test_gtext__DOT__gfx_pixel__67__Vfuncout;
    __Vfunc_test_gtext__DOT__gfx_pixel__67__Vfuncout = 0;
    IData/*31:0*/ __Vfunc_test_gtext__DOT__gfx_pixel__67__x;
    __Vfunc_test_gtext__DOT__gfx_pixel__67__x = 0;
    IData/*31:0*/ __Vfunc_test_gtext__DOT__gfx_pixel__67__y;
    __Vfunc_test_gtext__DOT__gfx_pixel__67__y = 0;
    CData/*0:0*/ __Vtask_test_gtext__DOT__check__68__condition;
    __Vtask_test_gtext__DOT__check__68__condition = 0;
    CData/*3:0*/ __Vfunc_test_gtext__DOT__gfx_pixel__69__Vfuncout;
    __Vfunc_test_gtext__DOT__gfx_pixel__69__Vfuncout = 0;
    IData/*31:0*/ __Vfunc_test_gtext__DOT__gfx_pixel__69__x;
    __Vfunc_test_gtext__DOT__gfx_pixel__69__x = 0;
    IData/*31:0*/ __Vfunc_test_gtext__DOT__gfx_pixel__69__y;
    __Vfunc_test_gtext__DOT__gfx_pixel__69__y = 0;
    CData/*3:0*/ __Vfunc_test_gtext__DOT__gfx_pixel__70__Vfuncout;
    __Vfunc_test_gtext__DOT__gfx_pixel__70__Vfuncout = 0;
    IData/*31:0*/ __Vfunc_test_gtext__DOT__gfx_pixel__70__x;
    __Vfunc_test_gtext__DOT__gfx_pixel__70__x = 0;
    IData/*31:0*/ __Vfunc_test_gtext__DOT__gfx_pixel__70__y;
    __Vfunc_test_gtext__DOT__gfx_pixel__70__y = 0;
    CData/*0:0*/ __Vtask_test_gtext__DOT__check__71__condition;
    __Vtask_test_gtext__DOT__check__71__condition = 0;
    CData/*7:0*/ __Vtask_test_gtext__DOT__write_cmd__72__cmd;
    __Vtask_test_gtext__DOT__write_cmd__72__cmd = 0;
    SData/*15:0*/ __Vtask_test_gtext__DOT__write_reg__73__addr;
    __Vtask_test_gtext__DOT__write_reg__73__addr = 0;
    CData/*7:0*/ __Vtask_test_gtext__DOT__write_reg__73__data;
    __Vtask_test_gtext__DOT__write_reg__73__data = 0;
    IData/*31:0*/ __Vtask_test_gtext__DOT__wait_cmd_done__74__test_gtext__DOT__unnamedblk1_1__DOT____Vrepeat0;
    __Vtask_test_gtext__DOT__wait_cmd_done__74__test_gtext__DOT__unnamedblk1_1__DOT____Vrepeat0 = 0;
    IData/*31:0*/ __Vtask_test_gtext__DOT__wait_cmd_done__74__timeout;
    __Vtask_test_gtext__DOT__wait_cmd_done__74__timeout = 0;
    IData/*31:0*/ __Vtask_test_gtext__DOT__set_fio_string__75__i;
    __Vtask_test_gtext__DOT__set_fio_string__75__i = 0;
    SData/*15:0*/ __Vtask_test_gtext__DOT__write_reg__76__addr;
    __Vtask_test_gtext__DOT__write_reg__76__addr = 0;
    CData/*7:0*/ __Vtask_test_gtext__DOT__write_reg__76__data;
    __Vtask_test_gtext__DOT__write_reg__76__data = 0;
    SData/*15:0*/ __Vtask_test_gtext__DOT__write_reg__77__addr;
    __Vtask_test_gtext__DOT__write_reg__77__addr = 0;
    CData/*7:0*/ __Vtask_test_gtext__DOT__write_reg__77__data;
    __Vtask_test_gtext__DOT__write_reg__77__data = 0;
    IData/*31:0*/ __Vtask_test_gtext__DOT__write_param__78__idx;
    __Vtask_test_gtext__DOT__write_param__78__idx = 0;
    CData/*7:0*/ __Vtask_test_gtext__DOT__write_param__78__data;
    __Vtask_test_gtext__DOT__write_param__78__data = 0;
    SData/*15:0*/ __Vtask_test_gtext__DOT__write_reg__79__addr;
    __Vtask_test_gtext__DOT__write_reg__79__addr = 0;
    CData/*7:0*/ __Vtask_test_gtext__DOT__write_reg__79__data;
    __Vtask_test_gtext__DOT__write_reg__79__data = 0;
    IData/*31:0*/ __Vtask_test_gtext__DOT__write_param__80__idx;
    __Vtask_test_gtext__DOT__write_param__80__idx = 0;
    CData/*7:0*/ __Vtask_test_gtext__DOT__write_param__80__data;
    __Vtask_test_gtext__DOT__write_param__80__data = 0;
    SData/*15:0*/ __Vtask_test_gtext__DOT__write_reg__81__addr;
    __Vtask_test_gtext__DOT__write_reg__81__addr = 0;
    CData/*7:0*/ __Vtask_test_gtext__DOT__write_reg__81__data;
    __Vtask_test_gtext__DOT__write_reg__81__data = 0;
    IData/*31:0*/ __Vtask_test_gtext__DOT__write_param__82__idx;
    __Vtask_test_gtext__DOT__write_param__82__idx = 0;
    CData/*7:0*/ __Vtask_test_gtext__DOT__write_param__82__data;
    __Vtask_test_gtext__DOT__write_param__82__data = 0;
    SData/*15:0*/ __Vtask_test_gtext__DOT__write_reg__83__addr;
    __Vtask_test_gtext__DOT__write_reg__83__addr = 0;
    CData/*7:0*/ __Vtask_test_gtext__DOT__write_reg__83__data;
    __Vtask_test_gtext__DOT__write_reg__83__data = 0;
    IData/*31:0*/ __Vtask_test_gtext__DOT__write_param__84__idx;
    __Vtask_test_gtext__DOT__write_param__84__idx = 0;
    CData/*7:0*/ __Vtask_test_gtext__DOT__write_param__84__data;
    __Vtask_test_gtext__DOT__write_param__84__data = 0;
    SData/*15:0*/ __Vtask_test_gtext__DOT__write_reg__85__addr;
    __Vtask_test_gtext__DOT__write_reg__85__addr = 0;
    CData/*7:0*/ __Vtask_test_gtext__DOT__write_reg__85__data;
    __Vtask_test_gtext__DOT__write_reg__85__data = 0;
    IData/*31:0*/ __Vtask_test_gtext__DOT__write_param__86__idx;
    __Vtask_test_gtext__DOT__write_param__86__idx = 0;
    CData/*7:0*/ __Vtask_test_gtext__DOT__write_param__86__data;
    __Vtask_test_gtext__DOT__write_param__86__data = 0;
    SData/*15:0*/ __Vtask_test_gtext__DOT__write_reg__87__addr;
    __Vtask_test_gtext__DOT__write_reg__87__addr = 0;
    CData/*7:0*/ __Vtask_test_gtext__DOT__write_reg__87__data;
    __Vtask_test_gtext__DOT__write_reg__87__data = 0;
    IData/*31:0*/ __Vtask_test_gtext__DOT__write_param__88__idx;
    __Vtask_test_gtext__DOT__write_param__88__idx = 0;
    CData/*7:0*/ __Vtask_test_gtext__DOT__write_param__88__data;
    __Vtask_test_gtext__DOT__write_param__88__data = 0;
    SData/*15:0*/ __Vtask_test_gtext__DOT__write_reg__89__addr;
    __Vtask_test_gtext__DOT__write_reg__89__addr = 0;
    CData/*7:0*/ __Vtask_test_gtext__DOT__write_reg__89__data;
    __Vtask_test_gtext__DOT__write_reg__89__data = 0;
    CData/*7:0*/ __Vtask_test_gtext__DOT__write_cmd__90__cmd;
    __Vtask_test_gtext__DOT__write_cmd__90__cmd = 0;
    SData/*15:0*/ __Vtask_test_gtext__DOT__write_reg__91__addr;
    __Vtask_test_gtext__DOT__write_reg__91__addr = 0;
    CData/*7:0*/ __Vtask_test_gtext__DOT__write_reg__91__data;
    __Vtask_test_gtext__DOT__write_reg__91__data = 0;
    IData/*31:0*/ __Vtask_test_gtext__DOT__wait_cmd_done__92__test_gtext__DOT__unnamedblk1_1__DOT____Vrepeat0;
    __Vtask_test_gtext__DOT__wait_cmd_done__92__test_gtext__DOT__unnamedblk1_1__DOT____Vrepeat0 = 0;
    IData/*31:0*/ __Vtask_test_gtext__DOT__wait_cmd_done__92__timeout;
    __Vtask_test_gtext__DOT__wait_cmd_done__92__timeout = 0;
    // Body
    VL_WRITEF_NX("=== Gtext Unit Tests ===\n\n",0);
    vlSelfRef.test_gtext__DOT__rst = 1U;
    vlSelfRef.test_gtext__DOT__cpu_we = 0U;
    vlSelfRef.test_gtext__DOT__cpu_re = 0U;
    vlSelfRef.test_gtext__DOT__cpu_ce = 0U;
    vlSelfRef.test_gtext__DOT__cpu_addr = 0U;
    vlSelfRef.test_gtext__DOT__cpu_wdata = 0U;
    test_gtext__DOT__unnamedblk1_2__DOT____Vrepeat1 = 0x00000032U;
    while (VL_LTS_III(32, 0U, test_gtext__DOT__unnamedblk1_2__DOT____Vrepeat1)) {
        Vtest_gtext___024root____VbeforeTrig_h8248a36b__0(vlSelf, 
                                                          "@(posedge test_gtext.clk)");
        co_await vlSelfRef.__VtrigSched_h8248a36b__0.trigger(0U, 
                                                             nullptr, 
                                                             "@(posedge test_gtext.clk)", 
                                                             "test_gtext.sv", 
                                                             115);
        test_gtext__DOT__unnamedblk1_2__DOT____Vrepeat1 
            = (test_gtext__DOT__unnamedblk1_2__DOT____Vrepeat1 
               - (IData)(1U));
    }
    vlSelfRef.test_gtext__DOT__rst = 0U;
    test_gtext__DOT__unnamedblk1_3__DOT____Vrepeat2 = 0x0000000aU;
    while (VL_LTS_III(32, 0U, test_gtext__DOT__unnamedblk1_3__DOT____Vrepeat2)) {
        Vtest_gtext___024root____VbeforeTrig_h8248a36b__0(vlSelf, 
                                                          "@(posedge test_gtext.clk)");
        co_await vlSelfRef.__VtrigSched_h8248a36b__0.trigger(0U, 
                                                             nullptr, 
                                                             "@(posedge test_gtext.clk)", 
                                                             "test_gtext.sv", 
                                                             117);
        test_gtext__DOT__unnamedblk1_3__DOT____Vrepeat2 
            = (test_gtext__DOT__unnamedblk1_3__DOT____Vrepeat2 
               - (IData)(1U));
    }
    VL_WRITEF_NX("Test: Empty string\n",0);
    __Vtask_test_gtext__DOT__write_reg__0__data = 0U;
    __Vtask_test_gtext__DOT__write_reg__0__addr = 0xb9a3U;
    Vtest_gtext___024root____VbeforeTrig_h8248a36b__0(vlSelf, 
                                                      "@(posedge test_gtext.clk)");
    co_await vlSelfRef.__VtrigSched_h8248a36b__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge test_gtext.clk)", 
                                                         "test_gtext.sv", 
                                                         49);
    vlSelfRef.test_gtext__DOT__cpu_addr = __Vtask_test_gtext__DOT__write_reg__0__addr;
    vlSelfRef.test_gtext__DOT__cpu_wdata = __Vtask_test_gtext__DOT__write_reg__0__data;
    vlSelfRef.test_gtext__DOT__cpu_we = 1U;
    vlSelfRef.test_gtext__DOT__cpu_ce = 1U;
    Vtest_gtext___024root____VbeforeTrig_h8248a36b__0(vlSelf, 
                                                      "@(posedge test_gtext.clk)");
    co_await vlSelfRef.__VtrigSched_h8248a36b__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge test_gtext.clk)", 
                                                         "test_gtext.sv", 
                                                         54);
    vlSelfRef.test_gtext__DOT__cpu_we = 0U;
    vlSelfRef.test_gtext__DOT__cpu_ce = 0U;
    __Vtask_test_gtext__DOT__write_param__1__data = 0U;
    __Vtask_test_gtext__DOT__write_param__1__idx = 0U;
    __Vtask_test_gtext__DOT__write_reg__2__data = __Vtask_test_gtext__DOT__write_param__1__data;
    __Vtask_test_gtext__DOT__write_reg__2__addr = (0x0000ffffU 
                                                   & ((IData)(0xa011U) 
                                                      + __Vtask_test_gtext__DOT__write_param__1__idx));
    Vtest_gtext___024root____VbeforeTrig_h8248a36b__0(vlSelf, 
                                                      "@(posedge test_gtext.clk)");
    co_await vlSelfRef.__VtrigSched_h8248a36b__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge test_gtext.clk)", 
                                                         "test_gtext.sv", 
                                                         49);
    vlSelfRef.test_gtext__DOT__cpu_addr = __Vtask_test_gtext__DOT__write_reg__2__addr;
    vlSelfRef.test_gtext__DOT__cpu_wdata = __Vtask_test_gtext__DOT__write_reg__2__data;
    vlSelfRef.test_gtext__DOT__cpu_we = 1U;
    vlSelfRef.test_gtext__DOT__cpu_ce = 1U;
    Vtest_gtext___024root____VbeforeTrig_h8248a36b__0(vlSelf, 
                                                      "@(posedge test_gtext.clk)");
    co_await vlSelfRef.__VtrigSched_h8248a36b__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge test_gtext.clk)", 
                                                         "test_gtext.sv", 
                                                         54);
    vlSelfRef.test_gtext__DOT__cpu_we = 0U;
    vlSelfRef.test_gtext__DOT__cpu_ce = 0U;
    __Vtask_test_gtext__DOT__write_param__3__data = 0U;
    __Vtask_test_gtext__DOT__write_param__3__idx = 1U;
    __Vtask_test_gtext__DOT__write_reg__4__data = __Vtask_test_gtext__DOT__write_param__3__data;
    __Vtask_test_gtext__DOT__write_reg__4__addr = (0x0000ffffU 
                                                   & ((IData)(0xa011U) 
                                                      + __Vtask_test_gtext__DOT__write_param__3__idx));
    Vtest_gtext___024root____VbeforeTrig_h8248a36b__0(vlSelf, 
                                                      "@(posedge test_gtext.clk)");
    co_await vlSelfRef.__VtrigSched_h8248a36b__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge test_gtext.clk)", 
                                                         "test_gtext.sv", 
                                                         49);
    vlSelfRef.test_gtext__DOT__cpu_addr = __Vtask_test_gtext__DOT__write_reg__4__addr;
    vlSelfRef.test_gtext__DOT__cpu_wdata = __Vtask_test_gtext__DOT__write_reg__4__data;
    vlSelfRef.test_gtext__DOT__cpu_we = 1U;
    vlSelfRef.test_gtext__DOT__cpu_ce = 1U;
    Vtest_gtext___024root____VbeforeTrig_h8248a36b__0(vlSelf, 
                                                      "@(posedge test_gtext.clk)");
    co_await vlSelfRef.__VtrigSched_h8248a36b__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge test_gtext.clk)", 
                                                         "test_gtext.sv", 
                                                         54);
    vlSelfRef.test_gtext__DOT__cpu_we = 0U;
    vlSelfRef.test_gtext__DOT__cpu_ce = 0U;
    __Vtask_test_gtext__DOT__write_param__5__data = 0U;
    __Vtask_test_gtext__DOT__write_param__5__idx = 2U;
    __Vtask_test_gtext__DOT__write_reg__6__data = __Vtask_test_gtext__DOT__write_param__5__data;
    __Vtask_test_gtext__DOT__write_reg__6__addr = (0x0000ffffU 
                                                   & ((IData)(0xa011U) 
                                                      + __Vtask_test_gtext__DOT__write_param__5__idx));
    Vtest_gtext___024root____VbeforeTrig_h8248a36b__0(vlSelf, 
                                                      "@(posedge test_gtext.clk)");
    co_await vlSelfRef.__VtrigSched_h8248a36b__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge test_gtext.clk)", 
                                                         "test_gtext.sv", 
                                                         49);
    vlSelfRef.test_gtext__DOT__cpu_addr = __Vtask_test_gtext__DOT__write_reg__6__addr;
    vlSelfRef.test_gtext__DOT__cpu_wdata = __Vtask_test_gtext__DOT__write_reg__6__data;
    vlSelfRef.test_gtext__DOT__cpu_we = 1U;
    vlSelfRef.test_gtext__DOT__cpu_ce = 1U;
    Vtest_gtext___024root____VbeforeTrig_h8248a36b__0(vlSelf, 
                                                      "@(posedge test_gtext.clk)");
    co_await vlSelfRef.__VtrigSched_h8248a36b__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge test_gtext.clk)", 
                                                         "test_gtext.sv", 
                                                         54);
    vlSelfRef.test_gtext__DOT__cpu_we = 0U;
    vlSelfRef.test_gtext__DOT__cpu_ce = 0U;
    __Vtask_test_gtext__DOT__write_param__7__data = 0U;
    __Vtask_test_gtext__DOT__write_param__7__idx = 3U;
    __Vtask_test_gtext__DOT__write_reg__8__data = __Vtask_test_gtext__DOT__write_param__7__data;
    __Vtask_test_gtext__DOT__write_reg__8__addr = (0x0000ffffU 
                                                   & ((IData)(0xa011U) 
                                                      + __Vtask_test_gtext__DOT__write_param__7__idx));
    Vtest_gtext___024root____VbeforeTrig_h8248a36b__0(vlSelf, 
                                                      "@(posedge test_gtext.clk)");
    co_await vlSelfRef.__VtrigSched_h8248a36b__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge test_gtext.clk)", 
                                                         "test_gtext.sv", 
                                                         49);
    vlSelfRef.test_gtext__DOT__cpu_addr = __Vtask_test_gtext__DOT__write_reg__8__addr;
    vlSelfRef.test_gtext__DOT__cpu_wdata = __Vtask_test_gtext__DOT__write_reg__8__data;
    vlSelfRef.test_gtext__DOT__cpu_we = 1U;
    vlSelfRef.test_gtext__DOT__cpu_ce = 1U;
    Vtest_gtext___024root____VbeforeTrig_h8248a36b__0(vlSelf, 
                                                      "@(posedge test_gtext.clk)");
    co_await vlSelfRef.__VtrigSched_h8248a36b__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge test_gtext.clk)", 
                                                         "test_gtext.sv", 
                                                         54);
    vlSelfRef.test_gtext__DOT__cpu_we = 0U;
    vlSelfRef.test_gtext__DOT__cpu_ce = 0U;
    __Vtask_test_gtext__DOT__write_param__9__data = 1U;
    __Vtask_test_gtext__DOT__write_param__9__idx = 5U;
    __Vtask_test_gtext__DOT__write_reg__10__data = __Vtask_test_gtext__DOT__write_param__9__data;
    __Vtask_test_gtext__DOT__write_reg__10__addr = 
        (0x0000ffffU & ((IData)(0xa011U) + __Vtask_test_gtext__DOT__write_param__9__idx));
    Vtest_gtext___024root____VbeforeTrig_h8248a36b__0(vlSelf, 
                                                      "@(posedge test_gtext.clk)");
    co_await vlSelfRef.__VtrigSched_h8248a36b__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge test_gtext.clk)", 
                                                         "test_gtext.sv", 
                                                         49);
    vlSelfRef.test_gtext__DOT__cpu_addr = __Vtask_test_gtext__DOT__write_reg__10__addr;
    vlSelfRef.test_gtext__DOT__cpu_wdata = __Vtask_test_gtext__DOT__write_reg__10__data;
    vlSelfRef.test_gtext__DOT__cpu_we = 1U;
    vlSelfRef.test_gtext__DOT__cpu_ce = 1U;
    Vtest_gtext___024root____VbeforeTrig_h8248a36b__0(vlSelf, 
                                                      "@(posedge test_gtext.clk)");
    co_await vlSelfRef.__VtrigSched_h8248a36b__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge test_gtext.clk)", 
                                                         "test_gtext.sv", 
                                                         54);
    vlSelfRef.test_gtext__DOT__cpu_we = 0U;
    vlSelfRef.test_gtext__DOT__cpu_ce = 0U;
    __Vtask_test_gtext__DOT__write_cmd__11__cmd = 0x0aU;
    __Vtask_test_gtext__DOT__write_reg__12__data = __Vtask_test_gtext__DOT__write_cmd__11__cmd;
    __Vtask_test_gtext__DOT__write_reg__12__addr = 0xa010U;
    Vtest_gtext___024root____VbeforeTrig_h8248a36b__0(vlSelf, 
                                                      "@(posedge test_gtext.clk)");
    co_await vlSelfRef.__VtrigSched_h8248a36b__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge test_gtext.clk)", 
                                                         "test_gtext.sv", 
                                                         49);
    vlSelfRef.test_gtext__DOT__cpu_addr = __Vtask_test_gtext__DOT__write_reg__12__addr;
    vlSelfRef.test_gtext__DOT__cpu_wdata = __Vtask_test_gtext__DOT__write_reg__12__data;
    vlSelfRef.test_gtext__DOT__cpu_we = 1U;
    vlSelfRef.test_gtext__DOT__cpu_ce = 1U;
    Vtest_gtext___024root____VbeforeTrig_h8248a36b__0(vlSelf, 
                                                      "@(posedge test_gtext.clk)");
    co_await vlSelfRef.__VtrigSched_h8248a36b__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge test_gtext.clk)", 
                                                         "test_gtext.sv", 
                                                         54);
    vlSelfRef.test_gtext__DOT__cpu_we = 0U;
    vlSelfRef.test_gtext__DOT__cpu_ce = 0U;
    test_gtext__DOT__unnamedblk1_4__DOT____Vrepeat3 = 5U;
    while (VL_LTS_III(32, 0U, test_gtext__DOT__unnamedblk1_4__DOT____Vrepeat3)) {
        Vtest_gtext___024root____VbeforeTrig_h8248a36b__0(vlSelf, 
                                                          "@(posedge test_gtext.clk)");
        co_await vlSelfRef.__VtrigSched_h8248a36b__0.trigger(0U, 
                                                             nullptr, 
                                                             "@(posedge test_gtext.clk)", 
                                                             "test_gtext.sv", 
                                                             126);
        test_gtext__DOT__unnamedblk1_4__DOT____Vrepeat3 
            = (test_gtext__DOT__unnamedblk1_4__DOT____Vrepeat3 
               - (IData)(1U));
    }
    __Vtask_test_gtext__DOT__check__13__condition = 
        (1U & ((~ (IData)(vlSelfRef.test_gtext__DOT__dut__DOT__cmd_busy)) 
               & (~ (IData)(vlSelfRef.test_gtext__DOT__dut__DOT__artist_busy))));
    vlSelfRef.__Vtask_test_gtext__DOT__check__13__name = "empty string: cmd not busy"s;
    vlSelfRef.test_gtext__DOT__test_num = ((IData)(1U) 
                                           + vlSelfRef.test_gtext__DOT__test_num);
    if (__Vtask_test_gtext__DOT__check__13__condition) {
        VL_WRITEF_NX("  PASS [%0d] %@\n",0,32,vlSelfRef.test_gtext__DOT__test_num,
                     -1,&(vlSelfRef.__Vtask_test_gtext__DOT__check__13__name));
        vlSelfRef.test_gtext__DOT__pass_count = ((IData)(1U) 
                                                 + vlSelfRef.test_gtext__DOT__pass_count);
    } else {
        VL_WRITEF_NX("  FAIL [%0d] %@\n",0,32,vlSelfRef.test_gtext__DOT__test_num,
                     -1,&(vlSelfRef.__Vtask_test_gtext__DOT__check__13__name));
        vlSelfRef.test_gtext__DOT__fail_count = ((IData)(1U) 
                                                 + vlSelfRef.test_gtext__DOT__fail_count);
    }
    VL_WRITEF_NX("Test: Single char 'A' at (0,0) scale=1\n",0);
    __Vtask_test_gtext__DOT__write_cmd__14__cmd = 7U;
    __Vtask_test_gtext__DOT__write_reg__15__data = __Vtask_test_gtext__DOT__write_cmd__14__cmd;
    __Vtask_test_gtext__DOT__write_reg__15__addr = 0xa010U;
    Vtest_gtext___024root____VbeforeTrig_h8248a36b__0(vlSelf, 
                                                      "@(posedge test_gtext.clk)");
    co_await vlSelfRef.__VtrigSched_h8248a36b__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge test_gtext.clk)", 
                                                         "test_gtext.sv", 
                                                         49);
    vlSelfRef.test_gtext__DOT__cpu_addr = __Vtask_test_gtext__DOT__write_reg__15__addr;
    vlSelfRef.test_gtext__DOT__cpu_wdata = __Vtask_test_gtext__DOT__write_reg__15__data;
    vlSelfRef.test_gtext__DOT__cpu_we = 1U;
    vlSelfRef.test_gtext__DOT__cpu_ce = 1U;
    Vtest_gtext___024root____VbeforeTrig_h8248a36b__0(vlSelf, 
                                                      "@(posedge test_gtext.clk)");
    co_await vlSelfRef.__VtrigSched_h8248a36b__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge test_gtext.clk)", 
                                                         "test_gtext.sv", 
                                                         54);
    vlSelfRef.test_gtext__DOT__cpu_we = 0U;
    vlSelfRef.test_gtext__DOT__cpu_ce = 0U;
    __Vtask_test_gtext__DOT__wait_cmd_done__16__test_gtext__DOT__unnamedblk1_1__DOT____Vrepeat0 = 0;
    __Vtask_test_gtext__DOT__wait_cmd_done__16__timeout = 0U;
    __Vtask_test_gtext__DOT__wait_cmd_done__16__timeout = 0U;
    __Vtask_test_gtext__DOT__wait_cmd_done__16__test_gtext__DOT__unnamedblk1_1__DOT____Vrepeat0 = 2U;
    while (VL_LTS_III(32, 0U, __Vtask_test_gtext__DOT__wait_cmd_done__16__test_gtext__DOT__unnamedblk1_1__DOT____Vrepeat0)) {
        Vtest_gtext___024root____VbeforeTrig_h8248a36b__0(vlSelf, 
                                                          "@(posedge test_gtext.clk)");
        co_await vlSelfRef.__VtrigSched_h8248a36b__0.trigger(0U, 
                                                             nullptr, 
                                                             "@(posedge test_gtext.clk)", 
                                                             "test_gtext.sv", 
                                                             71);
        __Vtask_test_gtext__DOT__wait_cmd_done__16__test_gtext__DOT__unnamedblk1_1__DOT____Vrepeat0 
            = (__Vtask_test_gtext__DOT__wait_cmd_done__16__test_gtext__DOT__unnamedblk1_1__DOT____Vrepeat0 
               - (IData)(1U));
    }
    while ((((IData)(vlSelfRef.test_gtext__DOT__dut__DOT__cmd_busy) 
             | (IData)(vlSelfRef.test_gtext__DOT__dut__DOT__artist_busy)) 
            & VL_GTS_III(32, 0x0007a120U, __Vtask_test_gtext__DOT__wait_cmd_done__16__timeout))) {
        Vtest_gtext___024root____VbeforeTrig_h8248a36b__0(vlSelf, 
                                                          "@(posedge test_gtext.clk)");
        co_await vlSelfRef.__VtrigSched_h8248a36b__0.trigger(0U, 
                                                             nullptr, 
                                                             "@(posedge test_gtext.clk)", 
                                                             "test_gtext.sv", 
                                                             73);
        __Vtask_test_gtext__DOT__wait_cmd_done__16__timeout 
            = ((IData)(1U) + __Vtask_test_gtext__DOT__wait_cmd_done__16__timeout);
    }
    __Vtask_test_gtext__DOT__write_param__17__data = 1U;
    __Vtask_test_gtext__DOT__write_param__17__idx = 0U;
    __Vtask_test_gtext__DOT__write_reg__18__data = __Vtask_test_gtext__DOT__write_param__17__data;
    __Vtask_test_gtext__DOT__write_reg__18__addr = 
        (0x0000ffffU & ((IData)(0xa011U) + __Vtask_test_gtext__DOT__write_param__17__idx));
    Vtest_gtext___024root____VbeforeTrig_h8248a36b__0(vlSelf, 
                                                      "@(posedge test_gtext.clk)");
    co_await vlSelfRef.__VtrigSched_h8248a36b__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge test_gtext.clk)", 
                                                         "test_gtext.sv", 
                                                         49);
    vlSelfRef.test_gtext__DOT__cpu_addr = __Vtask_test_gtext__DOT__write_reg__18__addr;
    vlSelfRef.test_gtext__DOT__cpu_wdata = __Vtask_test_gtext__DOT__write_reg__18__data;
    vlSelfRef.test_gtext__DOT__cpu_we = 1U;
    vlSelfRef.test_gtext__DOT__cpu_ce = 1U;
    Vtest_gtext___024root____VbeforeTrig_h8248a36b__0(vlSelf, 
                                                      "@(posedge test_gtext.clk)");
    co_await vlSelfRef.__VtrigSched_h8248a36b__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge test_gtext.clk)", 
                                                         "test_gtext.sv", 
                                                         54);
    vlSelfRef.test_gtext__DOT__cpu_we = 0U;
    vlSelfRef.test_gtext__DOT__cpu_ce = 0U;
    __Vtask_test_gtext__DOT__write_cmd__19__cmd = 8U;
    __Vtask_test_gtext__DOT__write_reg__20__data = __Vtask_test_gtext__DOT__write_cmd__19__cmd;
    __Vtask_test_gtext__DOT__write_reg__20__addr = 0xa010U;
    Vtest_gtext___024root____VbeforeTrig_h8248a36b__0(vlSelf, 
                                                      "@(posedge test_gtext.clk)");
    co_await vlSelfRef.__VtrigSched_h8248a36b__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge test_gtext.clk)", 
                                                         "test_gtext.sv", 
                                                         49);
    vlSelfRef.test_gtext__DOT__cpu_addr = __Vtask_test_gtext__DOT__write_reg__20__addr;
    vlSelfRef.test_gtext__DOT__cpu_wdata = __Vtask_test_gtext__DOT__write_reg__20__data;
    vlSelfRef.test_gtext__DOT__cpu_we = 1U;
    vlSelfRef.test_gtext__DOT__cpu_ce = 1U;
    Vtest_gtext___024root____VbeforeTrig_h8248a36b__0(vlSelf, 
                                                      "@(posedge test_gtext.clk)");
    co_await vlSelfRef.__VtrigSched_h8248a36b__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge test_gtext.clk)", 
                                                         "test_gtext.sv", 
                                                         54);
    vlSelfRef.test_gtext__DOT__cpu_we = 0U;
    vlSelfRef.test_gtext__DOT__cpu_ce = 0U;
    vlSelfRef.__Vtask_test_gtext__DOT__set_fio_string__21__s = "A"s;
    __Vtask_test_gtext__DOT__set_fio_string__21__i = 0U;
    __Vtask_test_gtext__DOT__write_reg__22__data = 
        (0x000000ffU & VL_LEN_IN(vlSelfRef.__Vtask_test_gtext__DOT__set_fio_string__21__s));
    __Vtask_test_gtext__DOT__write_reg__22__addr = 0xb9a3U;
    Vtest_gtext___024root____VbeforeTrig_h8248a36b__0(vlSelf, 
                                                      "@(posedge test_gtext.clk)");
    co_await vlSelfRef.__VtrigSched_h8248a36b__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge test_gtext.clk)", 
                                                         "test_gtext.sv", 
                                                         49);
    vlSelfRef.test_gtext__DOT__cpu_addr = __Vtask_test_gtext__DOT__write_reg__22__addr;
    vlSelfRef.test_gtext__DOT__cpu_wdata = __Vtask_test_gtext__DOT__write_reg__22__data;
    vlSelfRef.test_gtext__DOT__cpu_we = 1U;
    vlSelfRef.test_gtext__DOT__cpu_ce = 1U;
    Vtest_gtext___024root____VbeforeTrig_h8248a36b__0(vlSelf, 
                                                      "@(posedge test_gtext.clk)");
    co_await vlSelfRef.__VtrigSched_h8248a36b__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge test_gtext.clk)", 
                                                         "test_gtext.sv", 
                                                         54);
    vlSelfRef.test_gtext__DOT__cpu_we = 0U;
    vlSelfRef.test_gtext__DOT__cpu_ce = 0U;
    __Vtask_test_gtext__DOT__set_fio_string__21__i = 0U;
    while (VL_LTS_III(32, __Vtask_test_gtext__DOT__set_fio_string__21__i, 
                      VL_LEN_IN(vlSelfRef.__Vtask_test_gtext__DOT__set_fio_string__21__s))) {
        __Vtask_test_gtext__DOT__write_reg__23__data 
            = VL_GETC_N(vlSelfRef.__Vtask_test_gtext__DOT__set_fio_string__21__s,__Vtask_test_gtext__DOT__set_fio_string__21__i);
        __Vtask_test_gtext__DOT__write_reg__23__addr 
            = (0x0000ffffU & ((IData)(0xb9b0U) + __Vtask_test_gtext__DOT__set_fio_string__21__i));
        Vtest_gtext___024root____VbeforeTrig_h8248a36b__0(vlSelf, 
                                                          "@(posedge test_gtext.clk)");
        co_await vlSelfRef.__VtrigSched_h8248a36b__0.trigger(0U, 
                                                             nullptr, 
                                                             "@(posedge test_gtext.clk)", 
                                                             "test_gtext.sv", 
                                                             49);
        vlSelfRef.test_gtext__DOT__cpu_addr = __Vtask_test_gtext__DOT__write_reg__23__addr;
        vlSelfRef.test_gtext__DOT__cpu_wdata = __Vtask_test_gtext__DOT__write_reg__23__data;
        vlSelfRef.test_gtext__DOT__cpu_we = 1U;
        vlSelfRef.test_gtext__DOT__cpu_ce = 1U;
        Vtest_gtext___024root____VbeforeTrig_h8248a36b__0(vlSelf, 
                                                          "@(posedge test_gtext.clk)");
        co_await vlSelfRef.__VtrigSched_h8248a36b__0.trigger(0U, 
                                                             nullptr, 
                                                             "@(posedge test_gtext.clk)", 
                                                             "test_gtext.sv", 
                                                             54);
        vlSelfRef.test_gtext__DOT__cpu_we = 0U;
        vlSelfRef.test_gtext__DOT__cpu_ce = 0U;
        __Vtask_test_gtext__DOT__set_fio_string__21__i 
            = ((IData)(1U) + __Vtask_test_gtext__DOT__set_fio_string__21__i);
    }
    __Vtask_test_gtext__DOT__write_param__24__data = 0U;
    __Vtask_test_gtext__DOT__write_param__24__idx = 0U;
    __Vtask_test_gtext__DOT__write_reg__25__data = __Vtask_test_gtext__DOT__write_param__24__data;
    __Vtask_test_gtext__DOT__write_reg__25__addr = 
        (0x0000ffffU & ((IData)(0xa011U) + __Vtask_test_gtext__DOT__write_param__24__idx));
    Vtest_gtext___024root____VbeforeTrig_h8248a36b__0(vlSelf, 
                                                      "@(posedge test_gtext.clk)");
    co_await vlSelfRef.__VtrigSched_h8248a36b__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge test_gtext.clk)", 
                                                         "test_gtext.sv", 
                                                         49);
    vlSelfRef.test_gtext__DOT__cpu_addr = __Vtask_test_gtext__DOT__write_reg__25__addr;
    vlSelfRef.test_gtext__DOT__cpu_wdata = __Vtask_test_gtext__DOT__write_reg__25__data;
    vlSelfRef.test_gtext__DOT__cpu_we = 1U;
    vlSelfRef.test_gtext__DOT__cpu_ce = 1U;
    Vtest_gtext___024root____VbeforeTrig_h8248a36b__0(vlSelf, 
                                                      "@(posedge test_gtext.clk)");
    co_await vlSelfRef.__VtrigSched_h8248a36b__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge test_gtext.clk)", 
                                                         "test_gtext.sv", 
                                                         54);
    vlSelfRef.test_gtext__DOT__cpu_we = 0U;
    vlSelfRef.test_gtext__DOT__cpu_ce = 0U;
    __Vtask_test_gtext__DOT__write_param__26__data = 0U;
    __Vtask_test_gtext__DOT__write_param__26__idx = 1U;
    __Vtask_test_gtext__DOT__write_reg__27__data = __Vtask_test_gtext__DOT__write_param__26__data;
    __Vtask_test_gtext__DOT__write_reg__27__addr = 
        (0x0000ffffU & ((IData)(0xa011U) + __Vtask_test_gtext__DOT__write_param__26__idx));
    Vtest_gtext___024root____VbeforeTrig_h8248a36b__0(vlSelf, 
                                                      "@(posedge test_gtext.clk)");
    co_await vlSelfRef.__VtrigSched_h8248a36b__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge test_gtext.clk)", 
                                                         "test_gtext.sv", 
                                                         49);
    vlSelfRef.test_gtext__DOT__cpu_addr = __Vtask_test_gtext__DOT__write_reg__27__addr;
    vlSelfRef.test_gtext__DOT__cpu_wdata = __Vtask_test_gtext__DOT__write_reg__27__data;
    vlSelfRef.test_gtext__DOT__cpu_we = 1U;
    vlSelfRef.test_gtext__DOT__cpu_ce = 1U;
    Vtest_gtext___024root____VbeforeTrig_h8248a36b__0(vlSelf, 
                                                      "@(posedge test_gtext.clk)");
    co_await vlSelfRef.__VtrigSched_h8248a36b__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge test_gtext.clk)", 
                                                         "test_gtext.sv", 
                                                         54);
    vlSelfRef.test_gtext__DOT__cpu_we = 0U;
    vlSelfRef.test_gtext__DOT__cpu_ce = 0U;
    __Vtask_test_gtext__DOT__write_param__28__data = 0U;
    __Vtask_test_gtext__DOT__write_param__28__idx = 2U;
    __Vtask_test_gtext__DOT__write_reg__29__data = __Vtask_test_gtext__DOT__write_param__28__data;
    __Vtask_test_gtext__DOT__write_reg__29__addr = 
        (0x0000ffffU & ((IData)(0xa011U) + __Vtask_test_gtext__DOT__write_param__28__idx));
    Vtest_gtext___024root____VbeforeTrig_h8248a36b__0(vlSelf, 
                                                      "@(posedge test_gtext.clk)");
    co_await vlSelfRef.__VtrigSched_h8248a36b__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge test_gtext.clk)", 
                                                         "test_gtext.sv", 
                                                         49);
    vlSelfRef.test_gtext__DOT__cpu_addr = __Vtask_test_gtext__DOT__write_reg__29__addr;
    vlSelfRef.test_gtext__DOT__cpu_wdata = __Vtask_test_gtext__DOT__write_reg__29__data;
    vlSelfRef.test_gtext__DOT__cpu_we = 1U;
    vlSelfRef.test_gtext__DOT__cpu_ce = 1U;
    Vtest_gtext___024root____VbeforeTrig_h8248a36b__0(vlSelf, 
                                                      "@(posedge test_gtext.clk)");
    co_await vlSelfRef.__VtrigSched_h8248a36b__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge test_gtext.clk)", 
                                                         "test_gtext.sv", 
                                                         54);
    vlSelfRef.test_gtext__DOT__cpu_we = 0U;
    vlSelfRef.test_gtext__DOT__cpu_ce = 0U;
    __Vtask_test_gtext__DOT__write_param__30__data = 0U;
    __Vtask_test_gtext__DOT__write_param__30__idx = 3U;
    __Vtask_test_gtext__DOT__write_reg__31__data = __Vtask_test_gtext__DOT__write_param__30__data;
    __Vtask_test_gtext__DOT__write_reg__31__addr = 
        (0x0000ffffU & ((IData)(0xa011U) + __Vtask_test_gtext__DOT__write_param__30__idx));
    Vtest_gtext___024root____VbeforeTrig_h8248a36b__0(vlSelf, 
                                                      "@(posedge test_gtext.clk)");
    co_await vlSelfRef.__VtrigSched_h8248a36b__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge test_gtext.clk)", 
                                                         "test_gtext.sv", 
                                                         49);
    vlSelfRef.test_gtext__DOT__cpu_addr = __Vtask_test_gtext__DOT__write_reg__31__addr;
    vlSelfRef.test_gtext__DOT__cpu_wdata = __Vtask_test_gtext__DOT__write_reg__31__data;
    vlSelfRef.test_gtext__DOT__cpu_we = 1U;
    vlSelfRef.test_gtext__DOT__cpu_ce = 1U;
    Vtest_gtext___024root____VbeforeTrig_h8248a36b__0(vlSelf, 
                                                      "@(posedge test_gtext.clk)");
    co_await vlSelfRef.__VtrigSched_h8248a36b__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge test_gtext.clk)", 
                                                         "test_gtext.sv", 
                                                         54);
    vlSelfRef.test_gtext__DOT__cpu_we = 0U;
    vlSelfRef.test_gtext__DOT__cpu_ce = 0U;
    __Vtask_test_gtext__DOT__write_param__32__data = 0U;
    __Vtask_test_gtext__DOT__write_param__32__idx = 4U;
    __Vtask_test_gtext__DOT__write_reg__33__data = __Vtask_test_gtext__DOT__write_param__32__data;
    __Vtask_test_gtext__DOT__write_reg__33__addr = 
        (0x0000ffffU & ((IData)(0xa011U) + __Vtask_test_gtext__DOT__write_param__32__idx));
    Vtest_gtext___024root____VbeforeTrig_h8248a36b__0(vlSelf, 
                                                      "@(posedge test_gtext.clk)");
    co_await vlSelfRef.__VtrigSched_h8248a36b__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge test_gtext.clk)", 
                                                         "test_gtext.sv", 
                                                         49);
    vlSelfRef.test_gtext__DOT__cpu_addr = __Vtask_test_gtext__DOT__write_reg__33__addr;
    vlSelfRef.test_gtext__DOT__cpu_wdata = __Vtask_test_gtext__DOT__write_reg__33__data;
    vlSelfRef.test_gtext__DOT__cpu_we = 1U;
    vlSelfRef.test_gtext__DOT__cpu_ce = 1U;
    Vtest_gtext___024root____VbeforeTrig_h8248a36b__0(vlSelf, 
                                                      "@(posedge test_gtext.clk)");
    co_await vlSelfRef.__VtrigSched_h8248a36b__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge test_gtext.clk)", 
                                                         "test_gtext.sv", 
                                                         54);
    vlSelfRef.test_gtext__DOT__cpu_we = 0U;
    vlSelfRef.test_gtext__DOT__cpu_ce = 0U;
    __Vtask_test_gtext__DOT__write_param__34__data = 1U;
    __Vtask_test_gtext__DOT__write_param__34__idx = 5U;
    __Vtask_test_gtext__DOT__write_reg__35__data = __Vtask_test_gtext__DOT__write_param__34__data;
    __Vtask_test_gtext__DOT__write_reg__35__addr = 
        (0x0000ffffU & ((IData)(0xa011U) + __Vtask_test_gtext__DOT__write_param__34__idx));
    Vtest_gtext___024root____VbeforeTrig_h8248a36b__0(vlSelf, 
                                                      "@(posedge test_gtext.clk)");
    co_await vlSelfRef.__VtrigSched_h8248a36b__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge test_gtext.clk)", 
                                                         "test_gtext.sv", 
                                                         49);
    vlSelfRef.test_gtext__DOT__cpu_addr = __Vtask_test_gtext__DOT__write_reg__35__addr;
    vlSelfRef.test_gtext__DOT__cpu_wdata = __Vtask_test_gtext__DOT__write_reg__35__data;
    vlSelfRef.test_gtext__DOT__cpu_we = 1U;
    vlSelfRef.test_gtext__DOT__cpu_ce = 1U;
    Vtest_gtext___024root____VbeforeTrig_h8248a36b__0(vlSelf, 
                                                      "@(posedge test_gtext.clk)");
    co_await vlSelfRef.__VtrigSched_h8248a36b__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge test_gtext.clk)", 
                                                         "test_gtext.sv", 
                                                         54);
    vlSelfRef.test_gtext__DOT__cpu_we = 0U;
    vlSelfRef.test_gtext__DOT__cpu_ce = 0U;
    __Vtask_test_gtext__DOT__write_cmd__36__cmd = 0x0aU;
    __Vtask_test_gtext__DOT__write_reg__37__data = __Vtask_test_gtext__DOT__write_cmd__36__cmd;
    __Vtask_test_gtext__DOT__write_reg__37__addr = 0xa010U;
    Vtest_gtext___024root____VbeforeTrig_h8248a36b__0(vlSelf, 
                                                      "@(posedge test_gtext.clk)");
    co_await vlSelfRef.__VtrigSched_h8248a36b__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge test_gtext.clk)", 
                                                         "test_gtext.sv", 
                                                         49);
    vlSelfRef.test_gtext__DOT__cpu_addr = __Vtask_test_gtext__DOT__write_reg__37__addr;
    vlSelfRef.test_gtext__DOT__cpu_wdata = __Vtask_test_gtext__DOT__write_reg__37__data;
    vlSelfRef.test_gtext__DOT__cpu_we = 1U;
    vlSelfRef.test_gtext__DOT__cpu_ce = 1U;
    Vtest_gtext___024root____VbeforeTrig_h8248a36b__0(vlSelf, 
                                                      "@(posedge test_gtext.clk)");
    co_await vlSelfRef.__VtrigSched_h8248a36b__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge test_gtext.clk)", 
                                                         "test_gtext.sv", 
                                                         54);
    vlSelfRef.test_gtext__DOT__cpu_we = 0U;
    vlSelfRef.test_gtext__DOT__cpu_ce = 0U;
    __Vtask_test_gtext__DOT__wait_cmd_done__38__test_gtext__DOT__unnamedblk1_1__DOT____Vrepeat0 = 0;
    __Vtask_test_gtext__DOT__wait_cmd_done__38__timeout = 0U;
    __Vtask_test_gtext__DOT__wait_cmd_done__38__timeout = 0U;
    __Vtask_test_gtext__DOT__wait_cmd_done__38__test_gtext__DOT__unnamedblk1_1__DOT____Vrepeat0 = 2U;
    while (VL_LTS_III(32, 0U, __Vtask_test_gtext__DOT__wait_cmd_done__38__test_gtext__DOT__unnamedblk1_1__DOT____Vrepeat0)) {
        Vtest_gtext___024root____VbeforeTrig_h8248a36b__0(vlSelf, 
                                                          "@(posedge test_gtext.clk)");
        co_await vlSelfRef.__VtrigSched_h8248a36b__0.trigger(0U, 
                                                             nullptr, 
                                                             "@(posedge test_gtext.clk)", 
                                                             "test_gtext.sv", 
                                                             71);
        __Vtask_test_gtext__DOT__wait_cmd_done__38__test_gtext__DOT__unnamedblk1_1__DOT____Vrepeat0 
            = (__Vtask_test_gtext__DOT__wait_cmd_done__38__test_gtext__DOT__unnamedblk1_1__DOT____Vrepeat0 
               - (IData)(1U));
    }
    while ((((IData)(vlSelfRef.test_gtext__DOT__dut__DOT__cmd_busy) 
             | (IData)(vlSelfRef.test_gtext__DOT__dut__DOT__artist_busy)) 
            & VL_GTS_III(32, 0x0007a120U, __Vtask_test_gtext__DOT__wait_cmd_done__38__timeout))) {
        Vtest_gtext___024root____VbeforeTrig_h8248a36b__0(vlSelf, 
                                                          "@(posedge test_gtext.clk)");
        co_await vlSelfRef.__VtrigSched_h8248a36b__0.trigger(0U, 
                                                             nullptr, 
                                                             "@(posedge test_gtext.clk)", 
                                                             "test_gtext.sv", 
                                                             73);
        __Vtask_test_gtext__DOT__wait_cmd_done__38__timeout 
            = ((IData)(1U) + __Vtask_test_gtext__DOT__wait_cmd_done__38__timeout);
    }
    vlSelfRef.test_gtext__DOT__unnamedblk1__DOT__pixel_count = 0U;
    if ((0U != ([&]() {
                    __Vfunc_test_gtext__DOT__gfx_pixel__39__y = 0U;
                    __Vfunc_test_gtext__DOT__gfx_pixel__39__x = 0U;
                    __Vfunc_test_gtext__DOT__gfx_pixel__39__Vfuncout = 0;
                    __Vfunc_test_gtext__DOT__gfx_pixel__39__Vfuncout 
                        = ((0xf9ffU >= (0x0000ffffU 
                                        & (VL_MULS_III(32, (IData)(0x00000140U), __Vfunc_test_gtext__DOT__gfx_pixel__39__y) 
                                           + __Vfunc_test_gtext__DOT__gfx_pixel__39__x)))
                            ? vlSelfRef.test_gtext__DOT__dut__DOT__gfx_inst__DOT__gfx_mem__DOT__mem
                           [(0x0000ffffU & (VL_MULS_III(32, (IData)(0x00000140U), __Vfunc_test_gtext__DOT__gfx_pixel__39__y) 
                                            + __Vfunc_test_gtext__DOT__gfx_pixel__39__x))]
                            : 0U);
                }(), (IData)(__Vfunc_test_gtext__DOT__gfx_pixel__39__Vfuncout)))) {
        vlSelfRef.test_gtext__DOT__unnamedblk1__DOT__pixel_count 
            = ((IData)(1U) + vlSelfRef.test_gtext__DOT__unnamedblk1__DOT__pixel_count);
    }
    if ((0U != ([&]() {
                    __Vfunc_test_gtext__DOT__gfx_pixel__39__y = 0U;
                    __Vfunc_test_gtext__DOT__gfx_pixel__39__x = 1U;
                    __Vfunc_test_gtext__DOT__gfx_pixel__39__Vfuncout = 0;
                    __Vfunc_test_gtext__DOT__gfx_pixel__39__Vfuncout 
                        = ((0xf9ffU >= (0x0000ffffU 
                                        & (VL_MULS_III(32, (IData)(0x00000140U), __Vfunc_test_gtext__DOT__gfx_pixel__39__y) 
                                           + __Vfunc_test_gtext__DOT__gfx_pixel__39__x)))
                            ? vlSelfRef.test_gtext__DOT__dut__DOT__gfx_inst__DOT__gfx_mem__DOT__mem
                           [(0x0000ffffU & (VL_MULS_III(32, (IData)(0x00000140U), __Vfunc_test_gtext__DOT__gfx_pixel__39__y) 
                                            + __Vfunc_test_gtext__DOT__gfx_pixel__39__x))]
                            : 0U);
                }(), (IData)(__Vfunc_test_gtext__DOT__gfx_pixel__39__Vfuncout)))) {
        vlSelfRef.test_gtext__DOT__unnamedblk1__DOT__pixel_count 
            = ((IData)(1U) + vlSelfRef.test_gtext__DOT__unnamedblk1__DOT__pixel_count);
    }
    if ((0U != ([&]() {
                    __Vfunc_test_gtext__DOT__gfx_pixel__39__y = 0U;
                    __Vfunc_test_gtext__DOT__gfx_pixel__39__x = 2U;
                    __Vfunc_test_gtext__DOT__gfx_pixel__39__Vfuncout = 0;
                    __Vfunc_test_gtext__DOT__gfx_pixel__39__Vfuncout 
                        = ((0xf9ffU >= (0x0000ffffU 
                                        & (VL_MULS_III(32, (IData)(0x00000140U), __Vfunc_test_gtext__DOT__gfx_pixel__39__y) 
                                           + __Vfunc_test_gtext__DOT__gfx_pixel__39__x)))
                            ? vlSelfRef.test_gtext__DOT__dut__DOT__gfx_inst__DOT__gfx_mem__DOT__mem
                           [(0x0000ffffU & (VL_MULS_III(32, (IData)(0x00000140U), __Vfunc_test_gtext__DOT__gfx_pixel__39__y) 
                                            + __Vfunc_test_gtext__DOT__gfx_pixel__39__x))]
                            : 0U);
                }(), (IData)(__Vfunc_test_gtext__DOT__gfx_pixel__39__Vfuncout)))) {
        vlSelfRef.test_gtext__DOT__unnamedblk1__DOT__pixel_count 
            = ((IData)(1U) + vlSelfRef.test_gtext__DOT__unnamedblk1__DOT__pixel_count);
    }
    if ((0U != ([&]() {
                    __Vfunc_test_gtext__DOT__gfx_pixel__39__y = 0U;
                    __Vfunc_test_gtext__DOT__gfx_pixel__39__x = 3U;
                    __Vfunc_test_gtext__DOT__gfx_pixel__39__Vfuncout = 0;
                    __Vfunc_test_gtext__DOT__gfx_pixel__39__Vfuncout 
                        = ((0xf9ffU >= (0x0000ffffU 
                                        & (VL_MULS_III(32, (IData)(0x00000140U), __Vfunc_test_gtext__DOT__gfx_pixel__39__y) 
                                           + __Vfunc_test_gtext__DOT__gfx_pixel__39__x)))
                            ? vlSelfRef.test_gtext__DOT__dut__DOT__gfx_inst__DOT__gfx_mem__DOT__mem
                           [(0x0000ffffU & (VL_MULS_III(32, (IData)(0x00000140U), __Vfunc_test_gtext__DOT__gfx_pixel__39__y) 
                                            + __Vfunc_test_gtext__DOT__gfx_pixel__39__x))]
                            : 0U);
                }(), (IData)(__Vfunc_test_gtext__DOT__gfx_pixel__39__Vfuncout)))) {
        vlSelfRef.test_gtext__DOT__unnamedblk1__DOT__pixel_count 
            = ((IData)(1U) + vlSelfRef.test_gtext__DOT__unnamedblk1__DOT__pixel_count);
    }
    if ((0U != ([&]() {
                    __Vfunc_test_gtext__DOT__gfx_pixel__39__y = 0U;
                    __Vfunc_test_gtext__DOT__gfx_pixel__39__x = 4U;
                    __Vfunc_test_gtext__DOT__gfx_pixel__39__Vfuncout = 0;
                    __Vfunc_test_gtext__DOT__gfx_pixel__39__Vfuncout 
                        = ((0xf9ffU >= (0x0000ffffU 
                                        & (VL_MULS_III(32, (IData)(0x00000140U), __Vfunc_test_gtext__DOT__gfx_pixel__39__y) 
                                           + __Vfunc_test_gtext__DOT__gfx_pixel__39__x)))
                            ? vlSelfRef.test_gtext__DOT__dut__DOT__gfx_inst__DOT__gfx_mem__DOT__mem
                           [(0x0000ffffU & (VL_MULS_III(32, (IData)(0x00000140U), __Vfunc_test_gtext__DOT__gfx_pixel__39__y) 
                                            + __Vfunc_test_gtext__DOT__gfx_pixel__39__x))]
                            : 0U);
                }(), (IData)(__Vfunc_test_gtext__DOT__gfx_pixel__39__Vfuncout)))) {
        vlSelfRef.test_gtext__DOT__unnamedblk1__DOT__pixel_count 
            = ((IData)(1U) + vlSelfRef.test_gtext__DOT__unnamedblk1__DOT__pixel_count);
    }
    if ((0U != ([&]() {
                    __Vfunc_test_gtext__DOT__gfx_pixel__39__y = 0U;
                    __Vfunc_test_gtext__DOT__gfx_pixel__39__x = 5U;
                    __Vfunc_test_gtext__DOT__gfx_pixel__39__Vfuncout = 0;
                    __Vfunc_test_gtext__DOT__gfx_pixel__39__Vfuncout 
                        = ((0xf9ffU >= (0x0000ffffU 
                                        & (VL_MULS_III(32, (IData)(0x00000140U), __Vfunc_test_gtext__DOT__gfx_pixel__39__y) 
                                           + __Vfunc_test_gtext__DOT__gfx_pixel__39__x)))
                            ? vlSelfRef.test_gtext__DOT__dut__DOT__gfx_inst__DOT__gfx_mem__DOT__mem
                           [(0x0000ffffU & (VL_MULS_III(32, (IData)(0x00000140U), __Vfunc_test_gtext__DOT__gfx_pixel__39__y) 
                                            + __Vfunc_test_gtext__DOT__gfx_pixel__39__x))]
                            : 0U);
                }(), (IData)(__Vfunc_test_gtext__DOT__gfx_pixel__39__Vfuncout)))) {
        vlSelfRef.test_gtext__DOT__unnamedblk1__DOT__pixel_count 
            = ((IData)(1U) + vlSelfRef.test_gtext__DOT__unnamedblk1__DOT__pixel_count);
    }
    if ((0U != ([&]() {
                    __Vfunc_test_gtext__DOT__gfx_pixel__39__y = 0U;
                    __Vfunc_test_gtext__DOT__gfx_pixel__39__x = 6U;
                    __Vfunc_test_gtext__DOT__gfx_pixel__39__Vfuncout = 0;
                    __Vfunc_test_gtext__DOT__gfx_pixel__39__Vfuncout 
                        = ((0xf9ffU >= (0x0000ffffU 
                                        & (VL_MULS_III(32, (IData)(0x00000140U), __Vfunc_test_gtext__DOT__gfx_pixel__39__y) 
                                           + __Vfunc_test_gtext__DOT__gfx_pixel__39__x)))
                            ? vlSelfRef.test_gtext__DOT__dut__DOT__gfx_inst__DOT__gfx_mem__DOT__mem
                           [(0x0000ffffU & (VL_MULS_III(32, (IData)(0x00000140U), __Vfunc_test_gtext__DOT__gfx_pixel__39__y) 
                                            + __Vfunc_test_gtext__DOT__gfx_pixel__39__x))]
                            : 0U);
                }(), (IData)(__Vfunc_test_gtext__DOT__gfx_pixel__39__Vfuncout)))) {
        vlSelfRef.test_gtext__DOT__unnamedblk1__DOT__pixel_count 
            = ((IData)(1U) + vlSelfRef.test_gtext__DOT__unnamedblk1__DOT__pixel_count);
    }
    if ((0U != ([&]() {
                    __Vfunc_test_gtext__DOT__gfx_pixel__39__y = 0U;
                    __Vfunc_test_gtext__DOT__gfx_pixel__39__x = 7U;
                    __Vfunc_test_gtext__DOT__gfx_pixel__39__Vfuncout = 0;
                    __Vfunc_test_gtext__DOT__gfx_pixel__39__Vfuncout 
                        = ((0xf9ffU >= (0x0000ffffU 
                                        & (VL_MULS_III(32, (IData)(0x00000140U), __Vfunc_test_gtext__DOT__gfx_pixel__39__y) 
                                           + __Vfunc_test_gtext__DOT__gfx_pixel__39__x)))
                            ? vlSelfRef.test_gtext__DOT__dut__DOT__gfx_inst__DOT__gfx_mem__DOT__mem
                           [(0x0000ffffU & (VL_MULS_III(32, (IData)(0x00000140U), __Vfunc_test_gtext__DOT__gfx_pixel__39__y) 
                                            + __Vfunc_test_gtext__DOT__gfx_pixel__39__x))]
                            : 0U);
                }(), (IData)(__Vfunc_test_gtext__DOT__gfx_pixel__39__Vfuncout)))) {
        vlSelfRef.test_gtext__DOT__unnamedblk1__DOT__pixel_count 
            = ((IData)(1U) + vlSelfRef.test_gtext__DOT__unnamedblk1__DOT__pixel_count);
    }
    if ((0U != ([&]() {
                    __Vfunc_test_gtext__DOT__gfx_pixel__39__y = 1U;
                    __Vfunc_test_gtext__DOT__gfx_pixel__39__x = 0U;
                    __Vfunc_test_gtext__DOT__gfx_pixel__39__Vfuncout = 0;
                    __Vfunc_test_gtext__DOT__gfx_pixel__39__Vfuncout 
                        = ((0xf9ffU >= (0x0000ffffU 
                                        & (VL_MULS_III(32, (IData)(0x00000140U), __Vfunc_test_gtext__DOT__gfx_pixel__39__y) 
                                           + __Vfunc_test_gtext__DOT__gfx_pixel__39__x)))
                            ? vlSelfRef.test_gtext__DOT__dut__DOT__gfx_inst__DOT__gfx_mem__DOT__mem
                           [(0x0000ffffU & (VL_MULS_III(32, (IData)(0x00000140U), __Vfunc_test_gtext__DOT__gfx_pixel__39__y) 
                                            + __Vfunc_test_gtext__DOT__gfx_pixel__39__x))]
                            : 0U);
                }(), (IData)(__Vfunc_test_gtext__DOT__gfx_pixel__39__Vfuncout)))) {
        vlSelfRef.test_gtext__DOT__unnamedblk1__DOT__pixel_count 
            = ((IData)(1U) + vlSelfRef.test_gtext__DOT__unnamedblk1__DOT__pixel_count);
    }
    if ((0U != ([&]() {
                    __Vfunc_test_gtext__DOT__gfx_pixel__39__y = 1U;
                    __Vfunc_test_gtext__DOT__gfx_pixel__39__x = 1U;
                    __Vfunc_test_gtext__DOT__gfx_pixel__39__Vfuncout = 0;
                    __Vfunc_test_gtext__DOT__gfx_pixel__39__Vfuncout 
                        = ((0xf9ffU >= (0x0000ffffU 
                                        & (VL_MULS_III(32, (IData)(0x00000140U), __Vfunc_test_gtext__DOT__gfx_pixel__39__y) 
                                           + __Vfunc_test_gtext__DOT__gfx_pixel__39__x)))
                            ? vlSelfRef.test_gtext__DOT__dut__DOT__gfx_inst__DOT__gfx_mem__DOT__mem
                           [(0x0000ffffU & (VL_MULS_III(32, (IData)(0x00000140U), __Vfunc_test_gtext__DOT__gfx_pixel__39__y) 
                                            + __Vfunc_test_gtext__DOT__gfx_pixel__39__x))]
                            : 0U);
                }(), (IData)(__Vfunc_test_gtext__DOT__gfx_pixel__39__Vfuncout)))) {
        vlSelfRef.test_gtext__DOT__unnamedblk1__DOT__pixel_count 
            = ((IData)(1U) + vlSelfRef.test_gtext__DOT__unnamedblk1__DOT__pixel_count);
    }
    if ((0U != ([&]() {
                    __Vfunc_test_gtext__DOT__gfx_pixel__39__y = 1U;
                    __Vfunc_test_gtext__DOT__gfx_pixel__39__x = 2U;
                    __Vfunc_test_gtext__DOT__gfx_pixel__39__Vfuncout = 0;
                    __Vfunc_test_gtext__DOT__gfx_pixel__39__Vfuncout 
                        = ((0xf9ffU >= (0x0000ffffU 
                                        & (VL_MULS_III(32, (IData)(0x00000140U), __Vfunc_test_gtext__DOT__gfx_pixel__39__y) 
                                           + __Vfunc_test_gtext__DOT__gfx_pixel__39__x)))
                            ? vlSelfRef.test_gtext__DOT__dut__DOT__gfx_inst__DOT__gfx_mem__DOT__mem
                           [(0x0000ffffU & (VL_MULS_III(32, (IData)(0x00000140U), __Vfunc_test_gtext__DOT__gfx_pixel__39__y) 
                                            + __Vfunc_test_gtext__DOT__gfx_pixel__39__x))]
                            : 0U);
                }(), (IData)(__Vfunc_test_gtext__DOT__gfx_pixel__39__Vfuncout)))) {
        vlSelfRef.test_gtext__DOT__unnamedblk1__DOT__pixel_count 
            = ((IData)(1U) + vlSelfRef.test_gtext__DOT__unnamedblk1__DOT__pixel_count);
    }
    if ((0U != ([&]() {
                    __Vfunc_test_gtext__DOT__gfx_pixel__39__y = 1U;
                    __Vfunc_test_gtext__DOT__gfx_pixel__39__x = 3U;
                    __Vfunc_test_gtext__DOT__gfx_pixel__39__Vfuncout = 0;
                    __Vfunc_test_gtext__DOT__gfx_pixel__39__Vfuncout 
                        = ((0xf9ffU >= (0x0000ffffU 
                                        & (VL_MULS_III(32, (IData)(0x00000140U), __Vfunc_test_gtext__DOT__gfx_pixel__39__y) 
                                           + __Vfunc_test_gtext__DOT__gfx_pixel__39__x)))
                            ? vlSelfRef.test_gtext__DOT__dut__DOT__gfx_inst__DOT__gfx_mem__DOT__mem
                           [(0x0000ffffU & (VL_MULS_III(32, (IData)(0x00000140U), __Vfunc_test_gtext__DOT__gfx_pixel__39__y) 
                                            + __Vfunc_test_gtext__DOT__gfx_pixel__39__x))]
                            : 0U);
                }(), (IData)(__Vfunc_test_gtext__DOT__gfx_pixel__39__Vfuncout)))) {
        vlSelfRef.test_gtext__DOT__unnamedblk1__DOT__pixel_count 
            = ((IData)(1U) + vlSelfRef.test_gtext__DOT__unnamedblk1__DOT__pixel_count);
    }
    if ((0U != ([&]() {
                    __Vfunc_test_gtext__DOT__gfx_pixel__39__y = 1U;
                    __Vfunc_test_gtext__DOT__gfx_pixel__39__x = 4U;
                    __Vfunc_test_gtext__DOT__gfx_pixel__39__Vfuncout = 0;
                    __Vfunc_test_gtext__DOT__gfx_pixel__39__Vfuncout 
                        = ((0xf9ffU >= (0x0000ffffU 
                                        & (VL_MULS_III(32, (IData)(0x00000140U), __Vfunc_test_gtext__DOT__gfx_pixel__39__y) 
                                           + __Vfunc_test_gtext__DOT__gfx_pixel__39__x)))
                            ? vlSelfRef.test_gtext__DOT__dut__DOT__gfx_inst__DOT__gfx_mem__DOT__mem
                           [(0x0000ffffU & (VL_MULS_III(32, (IData)(0x00000140U), __Vfunc_test_gtext__DOT__gfx_pixel__39__y) 
                                            + __Vfunc_test_gtext__DOT__gfx_pixel__39__x))]
                            : 0U);
                }(), (IData)(__Vfunc_test_gtext__DOT__gfx_pixel__39__Vfuncout)))) {
        vlSelfRef.test_gtext__DOT__unnamedblk1__DOT__pixel_count 
            = ((IData)(1U) + vlSelfRef.test_gtext__DOT__unnamedblk1__DOT__pixel_count);
    }
    if ((0U != ([&]() {
                    __Vfunc_test_gtext__DOT__gfx_pixel__39__y = 1U;
                    __Vfunc_test_gtext__DOT__gfx_pixel__39__x = 5U;
                    __Vfunc_test_gtext__DOT__gfx_pixel__39__Vfuncout = 0;
                    __Vfunc_test_gtext__DOT__gfx_pixel__39__Vfuncout 
                        = ((0xf9ffU >= (0x0000ffffU 
                                        & (VL_MULS_III(32, (IData)(0x00000140U), __Vfunc_test_gtext__DOT__gfx_pixel__39__y) 
                                           + __Vfunc_test_gtext__DOT__gfx_pixel__39__x)))
                            ? vlSelfRef.test_gtext__DOT__dut__DOT__gfx_inst__DOT__gfx_mem__DOT__mem
                           [(0x0000ffffU & (VL_MULS_III(32, (IData)(0x00000140U), __Vfunc_test_gtext__DOT__gfx_pixel__39__y) 
                                            + __Vfunc_test_gtext__DOT__gfx_pixel__39__x))]
                            : 0U);
                }(), (IData)(__Vfunc_test_gtext__DOT__gfx_pixel__39__Vfuncout)))) {
        vlSelfRef.test_gtext__DOT__unnamedblk1__DOT__pixel_count 
            = ((IData)(1U) + vlSelfRef.test_gtext__DOT__unnamedblk1__DOT__pixel_count);
    }
    if ((0U != ([&]() {
                    __Vfunc_test_gtext__DOT__gfx_pixel__39__y = 1U;
                    __Vfunc_test_gtext__DOT__gfx_pixel__39__x = 6U;
                    __Vfunc_test_gtext__DOT__gfx_pixel__39__Vfuncout = 0;
                    __Vfunc_test_gtext__DOT__gfx_pixel__39__Vfuncout 
                        = ((0xf9ffU >= (0x0000ffffU 
                                        & (VL_MULS_III(32, (IData)(0x00000140U), __Vfunc_test_gtext__DOT__gfx_pixel__39__y) 
                                           + __Vfunc_test_gtext__DOT__gfx_pixel__39__x)))
                            ? vlSelfRef.test_gtext__DOT__dut__DOT__gfx_inst__DOT__gfx_mem__DOT__mem
                           [(0x0000ffffU & (VL_MULS_III(32, (IData)(0x00000140U), __Vfunc_test_gtext__DOT__gfx_pixel__39__y) 
                                            + __Vfunc_test_gtext__DOT__gfx_pixel__39__x))]
                            : 0U);
                }(), (IData)(__Vfunc_test_gtext__DOT__gfx_pixel__39__Vfuncout)))) {
        vlSelfRef.test_gtext__DOT__unnamedblk1__DOT__pixel_count 
            = ((IData)(1U) + vlSelfRef.test_gtext__DOT__unnamedblk1__DOT__pixel_count);
    }
    if ((0U != ([&]() {
                    __Vfunc_test_gtext__DOT__gfx_pixel__39__y = 1U;
                    __Vfunc_test_gtext__DOT__gfx_pixel__39__x = 7U;
                    __Vfunc_test_gtext__DOT__gfx_pixel__39__Vfuncout = 0;
                    __Vfunc_test_gtext__DOT__gfx_pixel__39__Vfuncout 
                        = ((0xf9ffU >= (0x0000ffffU 
                                        & (VL_MULS_III(32, (IData)(0x00000140U), __Vfunc_test_gtext__DOT__gfx_pixel__39__y) 
                                           + __Vfunc_test_gtext__DOT__gfx_pixel__39__x)))
                            ? vlSelfRef.test_gtext__DOT__dut__DOT__gfx_inst__DOT__gfx_mem__DOT__mem
                           [(0x0000ffffU & (VL_MULS_III(32, (IData)(0x00000140U), __Vfunc_test_gtext__DOT__gfx_pixel__39__y) 
                                            + __Vfunc_test_gtext__DOT__gfx_pixel__39__x))]
                            : 0U);
                }(), (IData)(__Vfunc_test_gtext__DOT__gfx_pixel__39__Vfuncout)))) {
        vlSelfRef.test_gtext__DOT__unnamedblk1__DOT__pixel_count 
            = ((IData)(1U) + vlSelfRef.test_gtext__DOT__unnamedblk1__DOT__pixel_count);
    }
    if ((0U != ([&]() {
                    __Vfunc_test_gtext__DOT__gfx_pixel__39__y = 2U;
                    __Vfunc_test_gtext__DOT__gfx_pixel__39__x = 0U;
                    __Vfunc_test_gtext__DOT__gfx_pixel__39__Vfuncout = 0;
                    __Vfunc_test_gtext__DOT__gfx_pixel__39__Vfuncout 
                        = ((0xf9ffU >= (0x0000ffffU 
                                        & (VL_MULS_III(32, (IData)(0x00000140U), __Vfunc_test_gtext__DOT__gfx_pixel__39__y) 
                                           + __Vfunc_test_gtext__DOT__gfx_pixel__39__x)))
                            ? vlSelfRef.test_gtext__DOT__dut__DOT__gfx_inst__DOT__gfx_mem__DOT__mem
                           [(0x0000ffffU & (VL_MULS_III(32, (IData)(0x00000140U), __Vfunc_test_gtext__DOT__gfx_pixel__39__y) 
                                            + __Vfunc_test_gtext__DOT__gfx_pixel__39__x))]
                            : 0U);
                }(), (IData)(__Vfunc_test_gtext__DOT__gfx_pixel__39__Vfuncout)))) {
        vlSelfRef.test_gtext__DOT__unnamedblk1__DOT__pixel_count 
            = ((IData)(1U) + vlSelfRef.test_gtext__DOT__unnamedblk1__DOT__pixel_count);
    }
    if ((0U != ([&]() {
                    __Vfunc_test_gtext__DOT__gfx_pixel__39__y = 2U;
                    __Vfunc_test_gtext__DOT__gfx_pixel__39__x = 1U;
                    __Vfunc_test_gtext__DOT__gfx_pixel__39__Vfuncout = 0;
                    __Vfunc_test_gtext__DOT__gfx_pixel__39__Vfuncout 
                        = ((0xf9ffU >= (0x0000ffffU 
                                        & (VL_MULS_III(32, (IData)(0x00000140U), __Vfunc_test_gtext__DOT__gfx_pixel__39__y) 
                                           + __Vfunc_test_gtext__DOT__gfx_pixel__39__x)))
                            ? vlSelfRef.test_gtext__DOT__dut__DOT__gfx_inst__DOT__gfx_mem__DOT__mem
                           [(0x0000ffffU & (VL_MULS_III(32, (IData)(0x00000140U), __Vfunc_test_gtext__DOT__gfx_pixel__39__y) 
                                            + __Vfunc_test_gtext__DOT__gfx_pixel__39__x))]
                            : 0U);
                }(), (IData)(__Vfunc_test_gtext__DOT__gfx_pixel__39__Vfuncout)))) {
        vlSelfRef.test_gtext__DOT__unnamedblk1__DOT__pixel_count 
            = ((IData)(1U) + vlSelfRef.test_gtext__DOT__unnamedblk1__DOT__pixel_count);
    }
    if ((0U != ([&]() {
                    __Vfunc_test_gtext__DOT__gfx_pixel__39__y = 2U;
                    __Vfunc_test_gtext__DOT__gfx_pixel__39__x = 2U;
                    __Vfunc_test_gtext__DOT__gfx_pixel__39__Vfuncout = 0;
                    __Vfunc_test_gtext__DOT__gfx_pixel__39__Vfuncout 
                        = ((0xf9ffU >= (0x0000ffffU 
                                        & (VL_MULS_III(32, (IData)(0x00000140U), __Vfunc_test_gtext__DOT__gfx_pixel__39__y) 
                                           + __Vfunc_test_gtext__DOT__gfx_pixel__39__x)))
                            ? vlSelfRef.test_gtext__DOT__dut__DOT__gfx_inst__DOT__gfx_mem__DOT__mem
                           [(0x0000ffffU & (VL_MULS_III(32, (IData)(0x00000140U), __Vfunc_test_gtext__DOT__gfx_pixel__39__y) 
                                            + __Vfunc_test_gtext__DOT__gfx_pixel__39__x))]
                            : 0U);
                }(), (IData)(__Vfunc_test_gtext__DOT__gfx_pixel__39__Vfuncout)))) {
        vlSelfRef.test_gtext__DOT__unnamedblk1__DOT__pixel_count 
            = ((IData)(1U) + vlSelfRef.test_gtext__DOT__unnamedblk1__DOT__pixel_count);
    }
    if ((0U != ([&]() {
                    __Vfunc_test_gtext__DOT__gfx_pixel__39__y = 2U;
                    __Vfunc_test_gtext__DOT__gfx_pixel__39__x = 3U;
                    __Vfunc_test_gtext__DOT__gfx_pixel__39__Vfuncout = 0;
                    __Vfunc_test_gtext__DOT__gfx_pixel__39__Vfuncout 
                        = ((0xf9ffU >= (0x0000ffffU 
                                        & (VL_MULS_III(32, (IData)(0x00000140U), __Vfunc_test_gtext__DOT__gfx_pixel__39__y) 
                                           + __Vfunc_test_gtext__DOT__gfx_pixel__39__x)))
                            ? vlSelfRef.test_gtext__DOT__dut__DOT__gfx_inst__DOT__gfx_mem__DOT__mem
                           [(0x0000ffffU & (VL_MULS_III(32, (IData)(0x00000140U), __Vfunc_test_gtext__DOT__gfx_pixel__39__y) 
                                            + __Vfunc_test_gtext__DOT__gfx_pixel__39__x))]
                            : 0U);
                }(), (IData)(__Vfunc_test_gtext__DOT__gfx_pixel__39__Vfuncout)))) {
        vlSelfRef.test_gtext__DOT__unnamedblk1__DOT__pixel_count 
            = ((IData)(1U) + vlSelfRef.test_gtext__DOT__unnamedblk1__DOT__pixel_count);
    }
    if ((0U != ([&]() {
                    __Vfunc_test_gtext__DOT__gfx_pixel__39__y = 2U;
                    __Vfunc_test_gtext__DOT__gfx_pixel__39__x = 4U;
                    __Vfunc_test_gtext__DOT__gfx_pixel__39__Vfuncout = 0;
                    __Vfunc_test_gtext__DOT__gfx_pixel__39__Vfuncout 
                        = ((0xf9ffU >= (0x0000ffffU 
                                        & (VL_MULS_III(32, (IData)(0x00000140U), __Vfunc_test_gtext__DOT__gfx_pixel__39__y) 
                                           + __Vfunc_test_gtext__DOT__gfx_pixel__39__x)))
                            ? vlSelfRef.test_gtext__DOT__dut__DOT__gfx_inst__DOT__gfx_mem__DOT__mem
                           [(0x0000ffffU & (VL_MULS_III(32, (IData)(0x00000140U), __Vfunc_test_gtext__DOT__gfx_pixel__39__y) 
                                            + __Vfunc_test_gtext__DOT__gfx_pixel__39__x))]
                            : 0U);
                }(), (IData)(__Vfunc_test_gtext__DOT__gfx_pixel__39__Vfuncout)))) {
        vlSelfRef.test_gtext__DOT__unnamedblk1__DOT__pixel_count 
            = ((IData)(1U) + vlSelfRef.test_gtext__DOT__unnamedblk1__DOT__pixel_count);
    }
    if ((0U != ([&]() {
                    __Vfunc_test_gtext__DOT__gfx_pixel__39__y = 2U;
                    __Vfunc_test_gtext__DOT__gfx_pixel__39__x = 5U;
                    __Vfunc_test_gtext__DOT__gfx_pixel__39__Vfuncout = 0;
                    __Vfunc_test_gtext__DOT__gfx_pixel__39__Vfuncout 
                        = ((0xf9ffU >= (0x0000ffffU 
                                        & (VL_MULS_III(32, (IData)(0x00000140U), __Vfunc_test_gtext__DOT__gfx_pixel__39__y) 
                                           + __Vfunc_test_gtext__DOT__gfx_pixel__39__x)))
                            ? vlSelfRef.test_gtext__DOT__dut__DOT__gfx_inst__DOT__gfx_mem__DOT__mem
                           [(0x0000ffffU & (VL_MULS_III(32, (IData)(0x00000140U), __Vfunc_test_gtext__DOT__gfx_pixel__39__y) 
                                            + __Vfunc_test_gtext__DOT__gfx_pixel__39__x))]
                            : 0U);
                }(), (IData)(__Vfunc_test_gtext__DOT__gfx_pixel__39__Vfuncout)))) {
        vlSelfRef.test_gtext__DOT__unnamedblk1__DOT__pixel_count 
            = ((IData)(1U) + vlSelfRef.test_gtext__DOT__unnamedblk1__DOT__pixel_count);
    }
    if ((0U != ([&]() {
                    __Vfunc_test_gtext__DOT__gfx_pixel__39__y = 2U;
                    __Vfunc_test_gtext__DOT__gfx_pixel__39__x = 6U;
                    __Vfunc_test_gtext__DOT__gfx_pixel__39__Vfuncout = 0;
                    __Vfunc_test_gtext__DOT__gfx_pixel__39__Vfuncout 
                        = ((0xf9ffU >= (0x0000ffffU 
                                        & (VL_MULS_III(32, (IData)(0x00000140U), __Vfunc_test_gtext__DOT__gfx_pixel__39__y) 
                                           + __Vfunc_test_gtext__DOT__gfx_pixel__39__x)))
                            ? vlSelfRef.test_gtext__DOT__dut__DOT__gfx_inst__DOT__gfx_mem__DOT__mem
                           [(0x0000ffffU & (VL_MULS_III(32, (IData)(0x00000140U), __Vfunc_test_gtext__DOT__gfx_pixel__39__y) 
                                            + __Vfunc_test_gtext__DOT__gfx_pixel__39__x))]
                            : 0U);
                }(), (IData)(__Vfunc_test_gtext__DOT__gfx_pixel__39__Vfuncout)))) {
        vlSelfRef.test_gtext__DOT__unnamedblk1__DOT__pixel_count 
            = ((IData)(1U) + vlSelfRef.test_gtext__DOT__unnamedblk1__DOT__pixel_count);
    }
    if ((0U != ([&]() {
                    __Vfunc_test_gtext__DOT__gfx_pixel__39__y = 2U;
                    __Vfunc_test_gtext__DOT__gfx_pixel__39__x = 7U;
                    __Vfunc_test_gtext__DOT__gfx_pixel__39__Vfuncout = 0;
                    __Vfunc_test_gtext__DOT__gfx_pixel__39__Vfuncout 
                        = ((0xf9ffU >= (0x0000ffffU 
                                        & (VL_MULS_III(32, (IData)(0x00000140U), __Vfunc_test_gtext__DOT__gfx_pixel__39__y) 
                                           + __Vfunc_test_gtext__DOT__gfx_pixel__39__x)))
                            ? vlSelfRef.test_gtext__DOT__dut__DOT__gfx_inst__DOT__gfx_mem__DOT__mem
                           [(0x0000ffffU & (VL_MULS_III(32, (IData)(0x00000140U), __Vfunc_test_gtext__DOT__gfx_pixel__39__y) 
                                            + __Vfunc_test_gtext__DOT__gfx_pixel__39__x))]
                            : 0U);
                }(), (IData)(__Vfunc_test_gtext__DOT__gfx_pixel__39__Vfuncout)))) {
        vlSelfRef.test_gtext__DOT__unnamedblk1__DOT__pixel_count 
            = ((IData)(1U) + vlSelfRef.test_gtext__DOT__unnamedblk1__DOT__pixel_count);
    }
    if ((0U != ([&]() {
                    __Vfunc_test_gtext__DOT__gfx_pixel__39__y = 3U;
                    __Vfunc_test_gtext__DOT__gfx_pixel__39__x = 0U;
                    __Vfunc_test_gtext__DOT__gfx_pixel__39__Vfuncout = 0;
                    __Vfunc_test_gtext__DOT__gfx_pixel__39__Vfuncout 
                        = ((0xf9ffU >= (0x0000ffffU 
                                        & (VL_MULS_III(32, (IData)(0x00000140U), __Vfunc_test_gtext__DOT__gfx_pixel__39__y) 
                                           + __Vfunc_test_gtext__DOT__gfx_pixel__39__x)))
                            ? vlSelfRef.test_gtext__DOT__dut__DOT__gfx_inst__DOT__gfx_mem__DOT__mem
                           [(0x0000ffffU & (VL_MULS_III(32, (IData)(0x00000140U), __Vfunc_test_gtext__DOT__gfx_pixel__39__y) 
                                            + __Vfunc_test_gtext__DOT__gfx_pixel__39__x))]
                            : 0U);
                }(), (IData)(__Vfunc_test_gtext__DOT__gfx_pixel__39__Vfuncout)))) {
        vlSelfRef.test_gtext__DOT__unnamedblk1__DOT__pixel_count 
            = ((IData)(1U) + vlSelfRef.test_gtext__DOT__unnamedblk1__DOT__pixel_count);
    }
    if ((0U != ([&]() {
                    __Vfunc_test_gtext__DOT__gfx_pixel__39__y = 3U;
                    __Vfunc_test_gtext__DOT__gfx_pixel__39__x = 1U;
                    __Vfunc_test_gtext__DOT__gfx_pixel__39__Vfuncout = 0;
                    __Vfunc_test_gtext__DOT__gfx_pixel__39__Vfuncout 
                        = ((0xf9ffU >= (0x0000ffffU 
                                        & (VL_MULS_III(32, (IData)(0x00000140U), __Vfunc_test_gtext__DOT__gfx_pixel__39__y) 
                                           + __Vfunc_test_gtext__DOT__gfx_pixel__39__x)))
                            ? vlSelfRef.test_gtext__DOT__dut__DOT__gfx_inst__DOT__gfx_mem__DOT__mem
                           [(0x0000ffffU & (VL_MULS_III(32, (IData)(0x00000140U), __Vfunc_test_gtext__DOT__gfx_pixel__39__y) 
                                            + __Vfunc_test_gtext__DOT__gfx_pixel__39__x))]
                            : 0U);
                }(), (IData)(__Vfunc_test_gtext__DOT__gfx_pixel__39__Vfuncout)))) {
        vlSelfRef.test_gtext__DOT__unnamedblk1__DOT__pixel_count 
            = ((IData)(1U) + vlSelfRef.test_gtext__DOT__unnamedblk1__DOT__pixel_count);
    }
    if ((0U != ([&]() {
                    __Vfunc_test_gtext__DOT__gfx_pixel__39__y = 3U;
                    __Vfunc_test_gtext__DOT__gfx_pixel__39__x = 2U;
                    __Vfunc_test_gtext__DOT__gfx_pixel__39__Vfuncout = 0;
                    __Vfunc_test_gtext__DOT__gfx_pixel__39__Vfuncout 
                        = ((0xf9ffU >= (0x0000ffffU 
                                        & (VL_MULS_III(32, (IData)(0x00000140U), __Vfunc_test_gtext__DOT__gfx_pixel__39__y) 
                                           + __Vfunc_test_gtext__DOT__gfx_pixel__39__x)))
                            ? vlSelfRef.test_gtext__DOT__dut__DOT__gfx_inst__DOT__gfx_mem__DOT__mem
                           [(0x0000ffffU & (VL_MULS_III(32, (IData)(0x00000140U), __Vfunc_test_gtext__DOT__gfx_pixel__39__y) 
                                            + __Vfunc_test_gtext__DOT__gfx_pixel__39__x))]
                            : 0U);
                }(), (IData)(__Vfunc_test_gtext__DOT__gfx_pixel__39__Vfuncout)))) {
        vlSelfRef.test_gtext__DOT__unnamedblk1__DOT__pixel_count 
            = ((IData)(1U) + vlSelfRef.test_gtext__DOT__unnamedblk1__DOT__pixel_count);
    }
    if ((0U != ([&]() {
                    __Vfunc_test_gtext__DOT__gfx_pixel__39__y = 3U;
                    __Vfunc_test_gtext__DOT__gfx_pixel__39__x = 3U;
                    __Vfunc_test_gtext__DOT__gfx_pixel__39__Vfuncout = 0;
                    __Vfunc_test_gtext__DOT__gfx_pixel__39__Vfuncout 
                        = ((0xf9ffU >= (0x0000ffffU 
                                        & (VL_MULS_III(32, (IData)(0x00000140U), __Vfunc_test_gtext__DOT__gfx_pixel__39__y) 
                                           + __Vfunc_test_gtext__DOT__gfx_pixel__39__x)))
                            ? vlSelfRef.test_gtext__DOT__dut__DOT__gfx_inst__DOT__gfx_mem__DOT__mem
                           [(0x0000ffffU & (VL_MULS_III(32, (IData)(0x00000140U), __Vfunc_test_gtext__DOT__gfx_pixel__39__y) 
                                            + __Vfunc_test_gtext__DOT__gfx_pixel__39__x))]
                            : 0U);
                }(), (IData)(__Vfunc_test_gtext__DOT__gfx_pixel__39__Vfuncout)))) {
        vlSelfRef.test_gtext__DOT__unnamedblk1__DOT__pixel_count 
            = ((IData)(1U) + vlSelfRef.test_gtext__DOT__unnamedblk1__DOT__pixel_count);
    }
    if ((0U != ([&]() {
                    __Vfunc_test_gtext__DOT__gfx_pixel__39__y = 3U;
                    __Vfunc_test_gtext__DOT__gfx_pixel__39__x = 4U;
                    __Vfunc_test_gtext__DOT__gfx_pixel__39__Vfuncout = 0;
                    __Vfunc_test_gtext__DOT__gfx_pixel__39__Vfuncout 
                        = ((0xf9ffU >= (0x0000ffffU 
                                        & (VL_MULS_III(32, (IData)(0x00000140U), __Vfunc_test_gtext__DOT__gfx_pixel__39__y) 
                                           + __Vfunc_test_gtext__DOT__gfx_pixel__39__x)))
                            ? vlSelfRef.test_gtext__DOT__dut__DOT__gfx_inst__DOT__gfx_mem__DOT__mem
                           [(0x0000ffffU & (VL_MULS_III(32, (IData)(0x00000140U), __Vfunc_test_gtext__DOT__gfx_pixel__39__y) 
                                            + __Vfunc_test_gtext__DOT__gfx_pixel__39__x))]
                            : 0U);
                }(), (IData)(__Vfunc_test_gtext__DOT__gfx_pixel__39__Vfuncout)))) {
        vlSelfRef.test_gtext__DOT__unnamedblk1__DOT__pixel_count 
            = ((IData)(1U) + vlSelfRef.test_gtext__DOT__unnamedblk1__DOT__pixel_count);
    }
    if ((0U != ([&]() {
                    __Vfunc_test_gtext__DOT__gfx_pixel__39__y = 3U;
                    __Vfunc_test_gtext__DOT__gfx_pixel__39__x = 5U;
                    __Vfunc_test_gtext__DOT__gfx_pixel__39__Vfuncout = 0;
                    __Vfunc_test_gtext__DOT__gfx_pixel__39__Vfuncout 
                        = ((0xf9ffU >= (0x0000ffffU 
                                        & (VL_MULS_III(32, (IData)(0x00000140U), __Vfunc_test_gtext__DOT__gfx_pixel__39__y) 
                                           + __Vfunc_test_gtext__DOT__gfx_pixel__39__x)))
                            ? vlSelfRef.test_gtext__DOT__dut__DOT__gfx_inst__DOT__gfx_mem__DOT__mem
                           [(0x0000ffffU & (VL_MULS_III(32, (IData)(0x00000140U), __Vfunc_test_gtext__DOT__gfx_pixel__39__y) 
                                            + __Vfunc_test_gtext__DOT__gfx_pixel__39__x))]
                            : 0U);
                }(), (IData)(__Vfunc_test_gtext__DOT__gfx_pixel__39__Vfuncout)))) {
        vlSelfRef.test_gtext__DOT__unnamedblk1__DOT__pixel_count 
            = ((IData)(1U) + vlSelfRef.test_gtext__DOT__unnamedblk1__DOT__pixel_count);
    }
    if ((0U != ([&]() {
                    __Vfunc_test_gtext__DOT__gfx_pixel__39__y = 3U;
                    __Vfunc_test_gtext__DOT__gfx_pixel__39__x = 6U;
                    __Vfunc_test_gtext__DOT__gfx_pixel__39__Vfuncout = 0;
                    __Vfunc_test_gtext__DOT__gfx_pixel__39__Vfuncout 
                        = ((0xf9ffU >= (0x0000ffffU 
                                        & (VL_MULS_III(32, (IData)(0x00000140U), __Vfunc_test_gtext__DOT__gfx_pixel__39__y) 
                                           + __Vfunc_test_gtext__DOT__gfx_pixel__39__x)))
                            ? vlSelfRef.test_gtext__DOT__dut__DOT__gfx_inst__DOT__gfx_mem__DOT__mem
                           [(0x0000ffffU & (VL_MULS_III(32, (IData)(0x00000140U), __Vfunc_test_gtext__DOT__gfx_pixel__39__y) 
                                            + __Vfunc_test_gtext__DOT__gfx_pixel__39__x))]
                            : 0U);
                }(), (IData)(__Vfunc_test_gtext__DOT__gfx_pixel__39__Vfuncout)))) {
        vlSelfRef.test_gtext__DOT__unnamedblk1__DOT__pixel_count 
            = ((IData)(1U) + vlSelfRef.test_gtext__DOT__unnamedblk1__DOT__pixel_count);
    }
    if ((0U != ([&]() {
                    __Vfunc_test_gtext__DOT__gfx_pixel__39__y = 3U;
                    __Vfunc_test_gtext__DOT__gfx_pixel__39__x = 7U;
                    __Vfunc_test_gtext__DOT__gfx_pixel__39__Vfuncout = 0;
                    __Vfunc_test_gtext__DOT__gfx_pixel__39__Vfuncout 
                        = ((0xf9ffU >= (0x0000ffffU 
                                        & (VL_MULS_III(32, (IData)(0x00000140U), __Vfunc_test_gtext__DOT__gfx_pixel__39__y) 
                                           + __Vfunc_test_gtext__DOT__gfx_pixel__39__x)))
                            ? vlSelfRef.test_gtext__DOT__dut__DOT__gfx_inst__DOT__gfx_mem__DOT__mem
                           [(0x0000ffffU & (VL_MULS_III(32, (IData)(0x00000140U), __Vfunc_test_gtext__DOT__gfx_pixel__39__y) 
                                            + __Vfunc_test_gtext__DOT__gfx_pixel__39__x))]
                            : 0U);
                }(), (IData)(__Vfunc_test_gtext__DOT__gfx_pixel__39__Vfuncout)))) {
        vlSelfRef.test_gtext__DOT__unnamedblk1__DOT__pixel_count 
            = ((IData)(1U) + vlSelfRef.test_gtext__DOT__unnamedblk1__DOT__pixel_count);
    }
    if ((0U != ([&]() {
                    __Vfunc_test_gtext__DOT__gfx_pixel__39__y = 4U;
                    __Vfunc_test_gtext__DOT__gfx_pixel__39__x = 0U;
                    __Vfunc_test_gtext__DOT__gfx_pixel__39__Vfuncout = 0;
                    __Vfunc_test_gtext__DOT__gfx_pixel__39__Vfuncout 
                        = ((0xf9ffU >= (0x0000ffffU 
                                        & (VL_MULS_III(32, (IData)(0x00000140U), __Vfunc_test_gtext__DOT__gfx_pixel__39__y) 
                                           + __Vfunc_test_gtext__DOT__gfx_pixel__39__x)))
                            ? vlSelfRef.test_gtext__DOT__dut__DOT__gfx_inst__DOT__gfx_mem__DOT__mem
                           [(0x0000ffffU & (VL_MULS_III(32, (IData)(0x00000140U), __Vfunc_test_gtext__DOT__gfx_pixel__39__y) 
                                            + __Vfunc_test_gtext__DOT__gfx_pixel__39__x))]
                            : 0U);
                }(), (IData)(__Vfunc_test_gtext__DOT__gfx_pixel__39__Vfuncout)))) {
        vlSelfRef.test_gtext__DOT__unnamedblk1__DOT__pixel_count 
            = ((IData)(1U) + vlSelfRef.test_gtext__DOT__unnamedblk1__DOT__pixel_count);
    }
    if ((0U != ([&]() {
                    __Vfunc_test_gtext__DOT__gfx_pixel__39__y = 4U;
                    __Vfunc_test_gtext__DOT__gfx_pixel__39__x = 1U;
                    __Vfunc_test_gtext__DOT__gfx_pixel__39__Vfuncout = 0;
                    __Vfunc_test_gtext__DOT__gfx_pixel__39__Vfuncout 
                        = ((0xf9ffU >= (0x0000ffffU 
                                        & (VL_MULS_III(32, (IData)(0x00000140U), __Vfunc_test_gtext__DOT__gfx_pixel__39__y) 
                                           + __Vfunc_test_gtext__DOT__gfx_pixel__39__x)))
                            ? vlSelfRef.test_gtext__DOT__dut__DOT__gfx_inst__DOT__gfx_mem__DOT__mem
                           [(0x0000ffffU & (VL_MULS_III(32, (IData)(0x00000140U), __Vfunc_test_gtext__DOT__gfx_pixel__39__y) 
                                            + __Vfunc_test_gtext__DOT__gfx_pixel__39__x))]
                            : 0U);
                }(), (IData)(__Vfunc_test_gtext__DOT__gfx_pixel__39__Vfuncout)))) {
        vlSelfRef.test_gtext__DOT__unnamedblk1__DOT__pixel_count 
            = ((IData)(1U) + vlSelfRef.test_gtext__DOT__unnamedblk1__DOT__pixel_count);
    }
    if ((0U != ([&]() {
                    __Vfunc_test_gtext__DOT__gfx_pixel__39__y = 4U;
                    __Vfunc_test_gtext__DOT__gfx_pixel__39__x = 2U;
                    __Vfunc_test_gtext__DOT__gfx_pixel__39__Vfuncout = 0;
                    __Vfunc_test_gtext__DOT__gfx_pixel__39__Vfuncout 
                        = ((0xf9ffU >= (0x0000ffffU 
                                        & (VL_MULS_III(32, (IData)(0x00000140U), __Vfunc_test_gtext__DOT__gfx_pixel__39__y) 
                                           + __Vfunc_test_gtext__DOT__gfx_pixel__39__x)))
                            ? vlSelfRef.test_gtext__DOT__dut__DOT__gfx_inst__DOT__gfx_mem__DOT__mem
                           [(0x0000ffffU & (VL_MULS_III(32, (IData)(0x00000140U), __Vfunc_test_gtext__DOT__gfx_pixel__39__y) 
                                            + __Vfunc_test_gtext__DOT__gfx_pixel__39__x))]
                            : 0U);
                }(), (IData)(__Vfunc_test_gtext__DOT__gfx_pixel__39__Vfuncout)))) {
        vlSelfRef.test_gtext__DOT__unnamedblk1__DOT__pixel_count 
            = ((IData)(1U) + vlSelfRef.test_gtext__DOT__unnamedblk1__DOT__pixel_count);
    }
    if ((0U != ([&]() {
                    __Vfunc_test_gtext__DOT__gfx_pixel__39__y = 4U;
                    __Vfunc_test_gtext__DOT__gfx_pixel__39__x = 3U;
                    __Vfunc_test_gtext__DOT__gfx_pixel__39__Vfuncout = 0;
                    __Vfunc_test_gtext__DOT__gfx_pixel__39__Vfuncout 
                        = ((0xf9ffU >= (0x0000ffffU 
                                        & (VL_MULS_III(32, (IData)(0x00000140U), __Vfunc_test_gtext__DOT__gfx_pixel__39__y) 
                                           + __Vfunc_test_gtext__DOT__gfx_pixel__39__x)))
                            ? vlSelfRef.test_gtext__DOT__dut__DOT__gfx_inst__DOT__gfx_mem__DOT__mem
                           [(0x0000ffffU & (VL_MULS_III(32, (IData)(0x00000140U), __Vfunc_test_gtext__DOT__gfx_pixel__39__y) 
                                            + __Vfunc_test_gtext__DOT__gfx_pixel__39__x))]
                            : 0U);
                }(), (IData)(__Vfunc_test_gtext__DOT__gfx_pixel__39__Vfuncout)))) {
        vlSelfRef.test_gtext__DOT__unnamedblk1__DOT__pixel_count 
            = ((IData)(1U) + vlSelfRef.test_gtext__DOT__unnamedblk1__DOT__pixel_count);
    }
    if ((0U != ([&]() {
                    __Vfunc_test_gtext__DOT__gfx_pixel__39__y = 4U;
                    __Vfunc_test_gtext__DOT__gfx_pixel__39__x = 4U;
                    __Vfunc_test_gtext__DOT__gfx_pixel__39__Vfuncout = 0;
                    __Vfunc_test_gtext__DOT__gfx_pixel__39__Vfuncout 
                        = ((0xf9ffU >= (0x0000ffffU 
                                        & (VL_MULS_III(32, (IData)(0x00000140U), __Vfunc_test_gtext__DOT__gfx_pixel__39__y) 
                                           + __Vfunc_test_gtext__DOT__gfx_pixel__39__x)))
                            ? vlSelfRef.test_gtext__DOT__dut__DOT__gfx_inst__DOT__gfx_mem__DOT__mem
                           [(0x0000ffffU & (VL_MULS_III(32, (IData)(0x00000140U), __Vfunc_test_gtext__DOT__gfx_pixel__39__y) 
                                            + __Vfunc_test_gtext__DOT__gfx_pixel__39__x))]
                            : 0U);
                }(), (IData)(__Vfunc_test_gtext__DOT__gfx_pixel__39__Vfuncout)))) {
        vlSelfRef.test_gtext__DOT__unnamedblk1__DOT__pixel_count 
            = ((IData)(1U) + vlSelfRef.test_gtext__DOT__unnamedblk1__DOT__pixel_count);
    }
    if ((0U != ([&]() {
                    __Vfunc_test_gtext__DOT__gfx_pixel__39__y = 4U;
                    __Vfunc_test_gtext__DOT__gfx_pixel__39__x = 5U;
                    __Vfunc_test_gtext__DOT__gfx_pixel__39__Vfuncout = 0;
                    __Vfunc_test_gtext__DOT__gfx_pixel__39__Vfuncout 
                        = ((0xf9ffU >= (0x0000ffffU 
                                        & (VL_MULS_III(32, (IData)(0x00000140U), __Vfunc_test_gtext__DOT__gfx_pixel__39__y) 
                                           + __Vfunc_test_gtext__DOT__gfx_pixel__39__x)))
                            ? vlSelfRef.test_gtext__DOT__dut__DOT__gfx_inst__DOT__gfx_mem__DOT__mem
                           [(0x0000ffffU & (VL_MULS_III(32, (IData)(0x00000140U), __Vfunc_test_gtext__DOT__gfx_pixel__39__y) 
                                            + __Vfunc_test_gtext__DOT__gfx_pixel__39__x))]
                            : 0U);
                }(), (IData)(__Vfunc_test_gtext__DOT__gfx_pixel__39__Vfuncout)))) {
        vlSelfRef.test_gtext__DOT__unnamedblk1__DOT__pixel_count 
            = ((IData)(1U) + vlSelfRef.test_gtext__DOT__unnamedblk1__DOT__pixel_count);
    }
    if ((0U != ([&]() {
                    __Vfunc_test_gtext__DOT__gfx_pixel__39__y = 4U;
                    __Vfunc_test_gtext__DOT__gfx_pixel__39__x = 6U;
                    __Vfunc_test_gtext__DOT__gfx_pixel__39__Vfuncout = 0;
                    __Vfunc_test_gtext__DOT__gfx_pixel__39__Vfuncout 
                        = ((0xf9ffU >= (0x0000ffffU 
                                        & (VL_MULS_III(32, (IData)(0x00000140U), __Vfunc_test_gtext__DOT__gfx_pixel__39__y) 
                                           + __Vfunc_test_gtext__DOT__gfx_pixel__39__x)))
                            ? vlSelfRef.test_gtext__DOT__dut__DOT__gfx_inst__DOT__gfx_mem__DOT__mem
                           [(0x0000ffffU & (VL_MULS_III(32, (IData)(0x00000140U), __Vfunc_test_gtext__DOT__gfx_pixel__39__y) 
                                            + __Vfunc_test_gtext__DOT__gfx_pixel__39__x))]
                            : 0U);
                }(), (IData)(__Vfunc_test_gtext__DOT__gfx_pixel__39__Vfuncout)))) {
        vlSelfRef.test_gtext__DOT__unnamedblk1__DOT__pixel_count 
            = ((IData)(1U) + vlSelfRef.test_gtext__DOT__unnamedblk1__DOT__pixel_count);
    }
    if ((0U != ([&]() {
                    __Vfunc_test_gtext__DOT__gfx_pixel__39__y = 4U;
                    __Vfunc_test_gtext__DOT__gfx_pixel__39__x = 7U;
                    __Vfunc_test_gtext__DOT__gfx_pixel__39__Vfuncout = 0;
                    __Vfunc_test_gtext__DOT__gfx_pixel__39__Vfuncout 
                        = ((0xf9ffU >= (0x0000ffffU 
                                        & (VL_MULS_III(32, (IData)(0x00000140U), __Vfunc_test_gtext__DOT__gfx_pixel__39__y) 
                                           + __Vfunc_test_gtext__DOT__gfx_pixel__39__x)))
                            ? vlSelfRef.test_gtext__DOT__dut__DOT__gfx_inst__DOT__gfx_mem__DOT__mem
                           [(0x0000ffffU & (VL_MULS_III(32, (IData)(0x00000140U), __Vfunc_test_gtext__DOT__gfx_pixel__39__y) 
                                            + __Vfunc_test_gtext__DOT__gfx_pixel__39__x))]
                            : 0U);
                }(), (IData)(__Vfunc_test_gtext__DOT__gfx_pixel__39__Vfuncout)))) {
        vlSelfRef.test_gtext__DOT__unnamedblk1__DOT__pixel_count 
            = ((IData)(1U) + vlSelfRef.test_gtext__DOT__unnamedblk1__DOT__pixel_count);
    }
    if ((0U != ([&]() {
                    __Vfunc_test_gtext__DOT__gfx_pixel__39__y = 5U;
                    __Vfunc_test_gtext__DOT__gfx_pixel__39__x = 0U;
                    __Vfunc_test_gtext__DOT__gfx_pixel__39__Vfuncout = 0;
                    __Vfunc_test_gtext__DOT__gfx_pixel__39__Vfuncout 
                        = ((0xf9ffU >= (0x0000ffffU 
                                        & (VL_MULS_III(32, (IData)(0x00000140U), __Vfunc_test_gtext__DOT__gfx_pixel__39__y) 
                                           + __Vfunc_test_gtext__DOT__gfx_pixel__39__x)))
                            ? vlSelfRef.test_gtext__DOT__dut__DOT__gfx_inst__DOT__gfx_mem__DOT__mem
                           [(0x0000ffffU & (VL_MULS_III(32, (IData)(0x00000140U), __Vfunc_test_gtext__DOT__gfx_pixel__39__y) 
                                            + __Vfunc_test_gtext__DOT__gfx_pixel__39__x))]
                            : 0U);
                }(), (IData)(__Vfunc_test_gtext__DOT__gfx_pixel__39__Vfuncout)))) {
        vlSelfRef.test_gtext__DOT__unnamedblk1__DOT__pixel_count 
            = ((IData)(1U) + vlSelfRef.test_gtext__DOT__unnamedblk1__DOT__pixel_count);
    }
    if ((0U != ([&]() {
                    __Vfunc_test_gtext__DOT__gfx_pixel__39__y = 5U;
                    __Vfunc_test_gtext__DOT__gfx_pixel__39__x = 1U;
                    __Vfunc_test_gtext__DOT__gfx_pixel__39__Vfuncout = 0;
                    __Vfunc_test_gtext__DOT__gfx_pixel__39__Vfuncout 
                        = ((0xf9ffU >= (0x0000ffffU 
                                        & (VL_MULS_III(32, (IData)(0x00000140U), __Vfunc_test_gtext__DOT__gfx_pixel__39__y) 
                                           + __Vfunc_test_gtext__DOT__gfx_pixel__39__x)))
                            ? vlSelfRef.test_gtext__DOT__dut__DOT__gfx_inst__DOT__gfx_mem__DOT__mem
                           [(0x0000ffffU & (VL_MULS_III(32, (IData)(0x00000140U), __Vfunc_test_gtext__DOT__gfx_pixel__39__y) 
                                            + __Vfunc_test_gtext__DOT__gfx_pixel__39__x))]
                            : 0U);
                }(), (IData)(__Vfunc_test_gtext__DOT__gfx_pixel__39__Vfuncout)))) {
        vlSelfRef.test_gtext__DOT__unnamedblk1__DOT__pixel_count 
            = ((IData)(1U) + vlSelfRef.test_gtext__DOT__unnamedblk1__DOT__pixel_count);
    }
    if ((0U != ([&]() {
                    __Vfunc_test_gtext__DOT__gfx_pixel__39__y = 5U;
                    __Vfunc_test_gtext__DOT__gfx_pixel__39__x = 2U;
                    __Vfunc_test_gtext__DOT__gfx_pixel__39__Vfuncout = 0;
                    __Vfunc_test_gtext__DOT__gfx_pixel__39__Vfuncout 
                        = ((0xf9ffU >= (0x0000ffffU 
                                        & (VL_MULS_III(32, (IData)(0x00000140U), __Vfunc_test_gtext__DOT__gfx_pixel__39__y) 
                                           + __Vfunc_test_gtext__DOT__gfx_pixel__39__x)))
                            ? vlSelfRef.test_gtext__DOT__dut__DOT__gfx_inst__DOT__gfx_mem__DOT__mem
                           [(0x0000ffffU & (VL_MULS_III(32, (IData)(0x00000140U), __Vfunc_test_gtext__DOT__gfx_pixel__39__y) 
                                            + __Vfunc_test_gtext__DOT__gfx_pixel__39__x))]
                            : 0U);
                }(), (IData)(__Vfunc_test_gtext__DOT__gfx_pixel__39__Vfuncout)))) {
        vlSelfRef.test_gtext__DOT__unnamedblk1__DOT__pixel_count 
            = ((IData)(1U) + vlSelfRef.test_gtext__DOT__unnamedblk1__DOT__pixel_count);
    }
    if ((0U != ([&]() {
                    __Vfunc_test_gtext__DOT__gfx_pixel__39__y = 5U;
                    __Vfunc_test_gtext__DOT__gfx_pixel__39__x = 3U;
                    __Vfunc_test_gtext__DOT__gfx_pixel__39__Vfuncout = 0;
                    __Vfunc_test_gtext__DOT__gfx_pixel__39__Vfuncout 
                        = ((0xf9ffU >= (0x0000ffffU 
                                        & (VL_MULS_III(32, (IData)(0x00000140U), __Vfunc_test_gtext__DOT__gfx_pixel__39__y) 
                                           + __Vfunc_test_gtext__DOT__gfx_pixel__39__x)))
                            ? vlSelfRef.test_gtext__DOT__dut__DOT__gfx_inst__DOT__gfx_mem__DOT__mem
                           [(0x0000ffffU & (VL_MULS_III(32, (IData)(0x00000140U), __Vfunc_test_gtext__DOT__gfx_pixel__39__y) 
                                            + __Vfunc_test_gtext__DOT__gfx_pixel__39__x))]
                            : 0U);
                }(), (IData)(__Vfunc_test_gtext__DOT__gfx_pixel__39__Vfuncout)))) {
        vlSelfRef.test_gtext__DOT__unnamedblk1__DOT__pixel_count 
            = ((IData)(1U) + vlSelfRef.test_gtext__DOT__unnamedblk1__DOT__pixel_count);
    }
    if ((0U != ([&]() {
                    __Vfunc_test_gtext__DOT__gfx_pixel__39__y = 5U;
                    __Vfunc_test_gtext__DOT__gfx_pixel__39__x = 4U;
                    __Vfunc_test_gtext__DOT__gfx_pixel__39__Vfuncout = 0;
                    __Vfunc_test_gtext__DOT__gfx_pixel__39__Vfuncout 
                        = ((0xf9ffU >= (0x0000ffffU 
                                        & (VL_MULS_III(32, (IData)(0x00000140U), __Vfunc_test_gtext__DOT__gfx_pixel__39__y) 
                                           + __Vfunc_test_gtext__DOT__gfx_pixel__39__x)))
                            ? vlSelfRef.test_gtext__DOT__dut__DOT__gfx_inst__DOT__gfx_mem__DOT__mem
                           [(0x0000ffffU & (VL_MULS_III(32, (IData)(0x00000140U), __Vfunc_test_gtext__DOT__gfx_pixel__39__y) 
                                            + __Vfunc_test_gtext__DOT__gfx_pixel__39__x))]
                            : 0U);
                }(), (IData)(__Vfunc_test_gtext__DOT__gfx_pixel__39__Vfuncout)))) {
        vlSelfRef.test_gtext__DOT__unnamedblk1__DOT__pixel_count 
            = ((IData)(1U) + vlSelfRef.test_gtext__DOT__unnamedblk1__DOT__pixel_count);
    }
    if ((0U != ([&]() {
                    __Vfunc_test_gtext__DOT__gfx_pixel__39__y = 5U;
                    __Vfunc_test_gtext__DOT__gfx_pixel__39__x = 5U;
                    __Vfunc_test_gtext__DOT__gfx_pixel__39__Vfuncout = 0;
                    __Vfunc_test_gtext__DOT__gfx_pixel__39__Vfuncout 
                        = ((0xf9ffU >= (0x0000ffffU 
                                        & (VL_MULS_III(32, (IData)(0x00000140U), __Vfunc_test_gtext__DOT__gfx_pixel__39__y) 
                                           + __Vfunc_test_gtext__DOT__gfx_pixel__39__x)))
                            ? vlSelfRef.test_gtext__DOT__dut__DOT__gfx_inst__DOT__gfx_mem__DOT__mem
                           [(0x0000ffffU & (VL_MULS_III(32, (IData)(0x00000140U), __Vfunc_test_gtext__DOT__gfx_pixel__39__y) 
                                            + __Vfunc_test_gtext__DOT__gfx_pixel__39__x))]
                            : 0U);
                }(), (IData)(__Vfunc_test_gtext__DOT__gfx_pixel__39__Vfuncout)))) {
        vlSelfRef.test_gtext__DOT__unnamedblk1__DOT__pixel_count 
            = ((IData)(1U) + vlSelfRef.test_gtext__DOT__unnamedblk1__DOT__pixel_count);
    }
    if ((0U != ([&]() {
                    __Vfunc_test_gtext__DOT__gfx_pixel__39__y = 5U;
                    __Vfunc_test_gtext__DOT__gfx_pixel__39__x = 6U;
                    __Vfunc_test_gtext__DOT__gfx_pixel__39__Vfuncout = 0;
                    __Vfunc_test_gtext__DOT__gfx_pixel__39__Vfuncout 
                        = ((0xf9ffU >= (0x0000ffffU 
                                        & (VL_MULS_III(32, (IData)(0x00000140U), __Vfunc_test_gtext__DOT__gfx_pixel__39__y) 
                                           + __Vfunc_test_gtext__DOT__gfx_pixel__39__x)))
                            ? vlSelfRef.test_gtext__DOT__dut__DOT__gfx_inst__DOT__gfx_mem__DOT__mem
                           [(0x0000ffffU & (VL_MULS_III(32, (IData)(0x00000140U), __Vfunc_test_gtext__DOT__gfx_pixel__39__y) 
                                            + __Vfunc_test_gtext__DOT__gfx_pixel__39__x))]
                            : 0U);
                }(), (IData)(__Vfunc_test_gtext__DOT__gfx_pixel__39__Vfuncout)))) {
        vlSelfRef.test_gtext__DOT__unnamedblk1__DOT__pixel_count 
            = ((IData)(1U) + vlSelfRef.test_gtext__DOT__unnamedblk1__DOT__pixel_count);
    }
    if ((0U != ([&]() {
                    __Vfunc_test_gtext__DOT__gfx_pixel__39__y = 5U;
                    __Vfunc_test_gtext__DOT__gfx_pixel__39__x = 7U;
                    __Vfunc_test_gtext__DOT__gfx_pixel__39__Vfuncout = 0;
                    __Vfunc_test_gtext__DOT__gfx_pixel__39__Vfuncout 
                        = ((0xf9ffU >= (0x0000ffffU 
                                        & (VL_MULS_III(32, (IData)(0x00000140U), __Vfunc_test_gtext__DOT__gfx_pixel__39__y) 
                                           + __Vfunc_test_gtext__DOT__gfx_pixel__39__x)))
                            ? vlSelfRef.test_gtext__DOT__dut__DOT__gfx_inst__DOT__gfx_mem__DOT__mem
                           [(0x0000ffffU & (VL_MULS_III(32, (IData)(0x00000140U), __Vfunc_test_gtext__DOT__gfx_pixel__39__y) 
                                            + __Vfunc_test_gtext__DOT__gfx_pixel__39__x))]
                            : 0U);
                }(), (IData)(__Vfunc_test_gtext__DOT__gfx_pixel__39__Vfuncout)))) {
        vlSelfRef.test_gtext__DOT__unnamedblk1__DOT__pixel_count 
            = ((IData)(1U) + vlSelfRef.test_gtext__DOT__unnamedblk1__DOT__pixel_count);
    }
    if ((0U != ([&]() {
                    __Vfunc_test_gtext__DOT__gfx_pixel__39__y = 6U;
                    __Vfunc_test_gtext__DOT__gfx_pixel__39__x = 0U;
                    __Vfunc_test_gtext__DOT__gfx_pixel__39__Vfuncout = 0;
                    __Vfunc_test_gtext__DOT__gfx_pixel__39__Vfuncout 
                        = ((0xf9ffU >= (0x0000ffffU 
                                        & (VL_MULS_III(32, (IData)(0x00000140U), __Vfunc_test_gtext__DOT__gfx_pixel__39__y) 
                                           + __Vfunc_test_gtext__DOT__gfx_pixel__39__x)))
                            ? vlSelfRef.test_gtext__DOT__dut__DOT__gfx_inst__DOT__gfx_mem__DOT__mem
                           [(0x0000ffffU & (VL_MULS_III(32, (IData)(0x00000140U), __Vfunc_test_gtext__DOT__gfx_pixel__39__y) 
                                            + __Vfunc_test_gtext__DOT__gfx_pixel__39__x))]
                            : 0U);
                }(), (IData)(__Vfunc_test_gtext__DOT__gfx_pixel__39__Vfuncout)))) {
        vlSelfRef.test_gtext__DOT__unnamedblk1__DOT__pixel_count 
            = ((IData)(1U) + vlSelfRef.test_gtext__DOT__unnamedblk1__DOT__pixel_count);
    }
    if ((0U != ([&]() {
                    __Vfunc_test_gtext__DOT__gfx_pixel__39__y = 6U;
                    __Vfunc_test_gtext__DOT__gfx_pixel__39__x = 1U;
                    __Vfunc_test_gtext__DOT__gfx_pixel__39__Vfuncout = 0;
                    __Vfunc_test_gtext__DOT__gfx_pixel__39__Vfuncout 
                        = ((0xf9ffU >= (0x0000ffffU 
                                        & (VL_MULS_III(32, (IData)(0x00000140U), __Vfunc_test_gtext__DOT__gfx_pixel__39__y) 
                                           + __Vfunc_test_gtext__DOT__gfx_pixel__39__x)))
                            ? vlSelfRef.test_gtext__DOT__dut__DOT__gfx_inst__DOT__gfx_mem__DOT__mem
                           [(0x0000ffffU & (VL_MULS_III(32, (IData)(0x00000140U), __Vfunc_test_gtext__DOT__gfx_pixel__39__y) 
                                            + __Vfunc_test_gtext__DOT__gfx_pixel__39__x))]
                            : 0U);
                }(), (IData)(__Vfunc_test_gtext__DOT__gfx_pixel__39__Vfuncout)))) {
        vlSelfRef.test_gtext__DOT__unnamedblk1__DOT__pixel_count 
            = ((IData)(1U) + vlSelfRef.test_gtext__DOT__unnamedblk1__DOT__pixel_count);
    }
    if ((0U != ([&]() {
                    __Vfunc_test_gtext__DOT__gfx_pixel__39__y = 6U;
                    __Vfunc_test_gtext__DOT__gfx_pixel__39__x = 2U;
                    __Vfunc_test_gtext__DOT__gfx_pixel__39__Vfuncout = 0;
                    __Vfunc_test_gtext__DOT__gfx_pixel__39__Vfuncout 
                        = ((0xf9ffU >= (0x0000ffffU 
                                        & (VL_MULS_III(32, (IData)(0x00000140U), __Vfunc_test_gtext__DOT__gfx_pixel__39__y) 
                                           + __Vfunc_test_gtext__DOT__gfx_pixel__39__x)))
                            ? vlSelfRef.test_gtext__DOT__dut__DOT__gfx_inst__DOT__gfx_mem__DOT__mem
                           [(0x0000ffffU & (VL_MULS_III(32, (IData)(0x00000140U), __Vfunc_test_gtext__DOT__gfx_pixel__39__y) 
                                            + __Vfunc_test_gtext__DOT__gfx_pixel__39__x))]
                            : 0U);
                }(), (IData)(__Vfunc_test_gtext__DOT__gfx_pixel__39__Vfuncout)))) {
        vlSelfRef.test_gtext__DOT__unnamedblk1__DOT__pixel_count 
            = ((IData)(1U) + vlSelfRef.test_gtext__DOT__unnamedblk1__DOT__pixel_count);
    }
    if ((0U != ([&]() {
                    __Vfunc_test_gtext__DOT__gfx_pixel__39__y = 6U;
                    __Vfunc_test_gtext__DOT__gfx_pixel__39__x = 3U;
                    __Vfunc_test_gtext__DOT__gfx_pixel__39__Vfuncout = 0;
                    __Vfunc_test_gtext__DOT__gfx_pixel__39__Vfuncout 
                        = ((0xf9ffU >= (0x0000ffffU 
                                        & (VL_MULS_III(32, (IData)(0x00000140U), __Vfunc_test_gtext__DOT__gfx_pixel__39__y) 
                                           + __Vfunc_test_gtext__DOT__gfx_pixel__39__x)))
                            ? vlSelfRef.test_gtext__DOT__dut__DOT__gfx_inst__DOT__gfx_mem__DOT__mem
                           [(0x0000ffffU & (VL_MULS_III(32, (IData)(0x00000140U), __Vfunc_test_gtext__DOT__gfx_pixel__39__y) 
                                            + __Vfunc_test_gtext__DOT__gfx_pixel__39__x))]
                            : 0U);
                }(), (IData)(__Vfunc_test_gtext__DOT__gfx_pixel__39__Vfuncout)))) {
        vlSelfRef.test_gtext__DOT__unnamedblk1__DOT__pixel_count 
            = ((IData)(1U) + vlSelfRef.test_gtext__DOT__unnamedblk1__DOT__pixel_count);
    }
    if ((0U != ([&]() {
                    __Vfunc_test_gtext__DOT__gfx_pixel__39__y = 6U;
                    __Vfunc_test_gtext__DOT__gfx_pixel__39__x = 4U;
                    __Vfunc_test_gtext__DOT__gfx_pixel__39__Vfuncout = 0;
                    __Vfunc_test_gtext__DOT__gfx_pixel__39__Vfuncout 
                        = ((0xf9ffU >= (0x0000ffffU 
                                        & (VL_MULS_III(32, (IData)(0x00000140U), __Vfunc_test_gtext__DOT__gfx_pixel__39__y) 
                                           + __Vfunc_test_gtext__DOT__gfx_pixel__39__x)))
                            ? vlSelfRef.test_gtext__DOT__dut__DOT__gfx_inst__DOT__gfx_mem__DOT__mem
                           [(0x0000ffffU & (VL_MULS_III(32, (IData)(0x00000140U), __Vfunc_test_gtext__DOT__gfx_pixel__39__y) 
                                            + __Vfunc_test_gtext__DOT__gfx_pixel__39__x))]
                            : 0U);
                }(), (IData)(__Vfunc_test_gtext__DOT__gfx_pixel__39__Vfuncout)))) {
        vlSelfRef.test_gtext__DOT__unnamedblk1__DOT__pixel_count 
            = ((IData)(1U) + vlSelfRef.test_gtext__DOT__unnamedblk1__DOT__pixel_count);
    }
    if ((0U != ([&]() {
                    __Vfunc_test_gtext__DOT__gfx_pixel__39__y = 6U;
                    __Vfunc_test_gtext__DOT__gfx_pixel__39__x = 5U;
                    __Vfunc_test_gtext__DOT__gfx_pixel__39__Vfuncout = 0;
                    __Vfunc_test_gtext__DOT__gfx_pixel__39__Vfuncout 
                        = ((0xf9ffU >= (0x0000ffffU 
                                        & (VL_MULS_III(32, (IData)(0x00000140U), __Vfunc_test_gtext__DOT__gfx_pixel__39__y) 
                                           + __Vfunc_test_gtext__DOT__gfx_pixel__39__x)))
                            ? vlSelfRef.test_gtext__DOT__dut__DOT__gfx_inst__DOT__gfx_mem__DOT__mem
                           [(0x0000ffffU & (VL_MULS_III(32, (IData)(0x00000140U), __Vfunc_test_gtext__DOT__gfx_pixel__39__y) 
                                            + __Vfunc_test_gtext__DOT__gfx_pixel__39__x))]
                            : 0U);
                }(), (IData)(__Vfunc_test_gtext__DOT__gfx_pixel__39__Vfuncout)))) {
        vlSelfRef.test_gtext__DOT__unnamedblk1__DOT__pixel_count 
            = ((IData)(1U) + vlSelfRef.test_gtext__DOT__unnamedblk1__DOT__pixel_count);
    }
    if ((0U != ([&]() {
                    __Vfunc_test_gtext__DOT__gfx_pixel__39__y = 6U;
                    __Vfunc_test_gtext__DOT__gfx_pixel__39__x = 6U;
                    __Vfunc_test_gtext__DOT__gfx_pixel__39__Vfuncout = 0;
                    __Vfunc_test_gtext__DOT__gfx_pixel__39__Vfuncout 
                        = ((0xf9ffU >= (0x0000ffffU 
                                        & (VL_MULS_III(32, (IData)(0x00000140U), __Vfunc_test_gtext__DOT__gfx_pixel__39__y) 
                                           + __Vfunc_test_gtext__DOT__gfx_pixel__39__x)))
                            ? vlSelfRef.test_gtext__DOT__dut__DOT__gfx_inst__DOT__gfx_mem__DOT__mem
                           [(0x0000ffffU & (VL_MULS_III(32, (IData)(0x00000140U), __Vfunc_test_gtext__DOT__gfx_pixel__39__y) 
                                            + __Vfunc_test_gtext__DOT__gfx_pixel__39__x))]
                            : 0U);
                }(), (IData)(__Vfunc_test_gtext__DOT__gfx_pixel__39__Vfuncout)))) {
        vlSelfRef.test_gtext__DOT__unnamedblk1__DOT__pixel_count 
            = ((IData)(1U) + vlSelfRef.test_gtext__DOT__unnamedblk1__DOT__pixel_count);
    }
    if ((0U != ([&]() {
                    __Vfunc_test_gtext__DOT__gfx_pixel__39__y = 6U;
                    __Vfunc_test_gtext__DOT__gfx_pixel__39__x = 7U;
                    __Vfunc_test_gtext__DOT__gfx_pixel__39__Vfuncout = 0;
                    __Vfunc_test_gtext__DOT__gfx_pixel__39__Vfuncout 
                        = ((0xf9ffU >= (0x0000ffffU 
                                        & (VL_MULS_III(32, (IData)(0x00000140U), __Vfunc_test_gtext__DOT__gfx_pixel__39__y) 
                                           + __Vfunc_test_gtext__DOT__gfx_pixel__39__x)))
                            ? vlSelfRef.test_gtext__DOT__dut__DOT__gfx_inst__DOT__gfx_mem__DOT__mem
                           [(0x0000ffffU & (VL_MULS_III(32, (IData)(0x00000140U), __Vfunc_test_gtext__DOT__gfx_pixel__39__y) 
                                            + __Vfunc_test_gtext__DOT__gfx_pixel__39__x))]
                            : 0U);
                }(), (IData)(__Vfunc_test_gtext__DOT__gfx_pixel__39__Vfuncout)))) {
        vlSelfRef.test_gtext__DOT__unnamedblk1__DOT__pixel_count 
            = ((IData)(1U) + vlSelfRef.test_gtext__DOT__unnamedblk1__DOT__pixel_count);
    }
    if ((0U != ([&]() {
                    __Vfunc_test_gtext__DOT__gfx_pixel__39__y = 7U;
                    __Vfunc_test_gtext__DOT__gfx_pixel__39__x = 0U;
                    __Vfunc_test_gtext__DOT__gfx_pixel__39__Vfuncout = 0;
                    __Vfunc_test_gtext__DOT__gfx_pixel__39__Vfuncout 
                        = ((0xf9ffU >= (0x0000ffffU 
                                        & (VL_MULS_III(32, (IData)(0x00000140U), __Vfunc_test_gtext__DOT__gfx_pixel__39__y) 
                                           + __Vfunc_test_gtext__DOT__gfx_pixel__39__x)))
                            ? vlSelfRef.test_gtext__DOT__dut__DOT__gfx_inst__DOT__gfx_mem__DOT__mem
                           [(0x0000ffffU & (VL_MULS_III(32, (IData)(0x00000140U), __Vfunc_test_gtext__DOT__gfx_pixel__39__y) 
                                            + __Vfunc_test_gtext__DOT__gfx_pixel__39__x))]
                            : 0U);
                }(), (IData)(__Vfunc_test_gtext__DOT__gfx_pixel__39__Vfuncout)))) {
        vlSelfRef.test_gtext__DOT__unnamedblk1__DOT__pixel_count 
            = ((IData)(1U) + vlSelfRef.test_gtext__DOT__unnamedblk1__DOT__pixel_count);
    }
    if ((0U != ([&]() {
                    __Vfunc_test_gtext__DOT__gfx_pixel__39__y = 7U;
                    __Vfunc_test_gtext__DOT__gfx_pixel__39__x = 1U;
                    __Vfunc_test_gtext__DOT__gfx_pixel__39__Vfuncout = 0;
                    __Vfunc_test_gtext__DOT__gfx_pixel__39__Vfuncout 
                        = ((0xf9ffU >= (0x0000ffffU 
                                        & (VL_MULS_III(32, (IData)(0x00000140U), __Vfunc_test_gtext__DOT__gfx_pixel__39__y) 
                                           + __Vfunc_test_gtext__DOT__gfx_pixel__39__x)))
                            ? vlSelfRef.test_gtext__DOT__dut__DOT__gfx_inst__DOT__gfx_mem__DOT__mem
                           [(0x0000ffffU & (VL_MULS_III(32, (IData)(0x00000140U), __Vfunc_test_gtext__DOT__gfx_pixel__39__y) 
                                            + __Vfunc_test_gtext__DOT__gfx_pixel__39__x))]
                            : 0U);
                }(), (IData)(__Vfunc_test_gtext__DOT__gfx_pixel__39__Vfuncout)))) {
        vlSelfRef.test_gtext__DOT__unnamedblk1__DOT__pixel_count 
            = ((IData)(1U) + vlSelfRef.test_gtext__DOT__unnamedblk1__DOT__pixel_count);
    }
    if ((0U != ([&]() {
                    __Vfunc_test_gtext__DOT__gfx_pixel__39__y = 7U;
                    __Vfunc_test_gtext__DOT__gfx_pixel__39__x = 2U;
                    __Vfunc_test_gtext__DOT__gfx_pixel__39__Vfuncout = 0;
                    __Vfunc_test_gtext__DOT__gfx_pixel__39__Vfuncout 
                        = ((0xf9ffU >= (0x0000ffffU 
                                        & (VL_MULS_III(32, (IData)(0x00000140U), __Vfunc_test_gtext__DOT__gfx_pixel__39__y) 
                                           + __Vfunc_test_gtext__DOT__gfx_pixel__39__x)))
                            ? vlSelfRef.test_gtext__DOT__dut__DOT__gfx_inst__DOT__gfx_mem__DOT__mem
                           [(0x0000ffffU & (VL_MULS_III(32, (IData)(0x00000140U), __Vfunc_test_gtext__DOT__gfx_pixel__39__y) 
                                            + __Vfunc_test_gtext__DOT__gfx_pixel__39__x))]
                            : 0U);
                }(), (IData)(__Vfunc_test_gtext__DOT__gfx_pixel__39__Vfuncout)))) {
        vlSelfRef.test_gtext__DOT__unnamedblk1__DOT__pixel_count 
            = ((IData)(1U) + vlSelfRef.test_gtext__DOT__unnamedblk1__DOT__pixel_count);
    }
    if ((0U != ([&]() {
                    __Vfunc_test_gtext__DOT__gfx_pixel__39__y = 7U;
                    __Vfunc_test_gtext__DOT__gfx_pixel__39__x = 3U;
                    __Vfunc_test_gtext__DOT__gfx_pixel__39__Vfuncout = 0;
                    __Vfunc_test_gtext__DOT__gfx_pixel__39__Vfuncout 
                        = ((0xf9ffU >= (0x0000ffffU 
                                        & (VL_MULS_III(32, (IData)(0x00000140U), __Vfunc_test_gtext__DOT__gfx_pixel__39__y) 
                                           + __Vfunc_test_gtext__DOT__gfx_pixel__39__x)))
                            ? vlSelfRef.test_gtext__DOT__dut__DOT__gfx_inst__DOT__gfx_mem__DOT__mem
                           [(0x0000ffffU & (VL_MULS_III(32, (IData)(0x00000140U), __Vfunc_test_gtext__DOT__gfx_pixel__39__y) 
                                            + __Vfunc_test_gtext__DOT__gfx_pixel__39__x))]
                            : 0U);
                }(), (IData)(__Vfunc_test_gtext__DOT__gfx_pixel__39__Vfuncout)))) {
        vlSelfRef.test_gtext__DOT__unnamedblk1__DOT__pixel_count 
            = ((IData)(1U) + vlSelfRef.test_gtext__DOT__unnamedblk1__DOT__pixel_count);
    }
    if ((0U != ([&]() {
                    __Vfunc_test_gtext__DOT__gfx_pixel__39__y = 7U;
                    __Vfunc_test_gtext__DOT__gfx_pixel__39__x = 4U;
                    __Vfunc_test_gtext__DOT__gfx_pixel__39__Vfuncout = 0;
                    __Vfunc_test_gtext__DOT__gfx_pixel__39__Vfuncout 
                        = ((0xf9ffU >= (0x0000ffffU 
                                        & (VL_MULS_III(32, (IData)(0x00000140U), __Vfunc_test_gtext__DOT__gfx_pixel__39__y) 
                                           + __Vfunc_test_gtext__DOT__gfx_pixel__39__x)))
                            ? vlSelfRef.test_gtext__DOT__dut__DOT__gfx_inst__DOT__gfx_mem__DOT__mem
                           [(0x0000ffffU & (VL_MULS_III(32, (IData)(0x00000140U), __Vfunc_test_gtext__DOT__gfx_pixel__39__y) 
                                            + __Vfunc_test_gtext__DOT__gfx_pixel__39__x))]
                            : 0U);
                }(), (IData)(__Vfunc_test_gtext__DOT__gfx_pixel__39__Vfuncout)))) {
        vlSelfRef.test_gtext__DOT__unnamedblk1__DOT__pixel_count 
            = ((IData)(1U) + vlSelfRef.test_gtext__DOT__unnamedblk1__DOT__pixel_count);
    }
    if ((0U != ([&]() {
                    __Vfunc_test_gtext__DOT__gfx_pixel__39__y = 7U;
                    __Vfunc_test_gtext__DOT__gfx_pixel__39__x = 5U;
                    __Vfunc_test_gtext__DOT__gfx_pixel__39__Vfuncout = 0;
                    __Vfunc_test_gtext__DOT__gfx_pixel__39__Vfuncout 
                        = ((0xf9ffU >= (0x0000ffffU 
                                        & (VL_MULS_III(32, (IData)(0x00000140U), __Vfunc_test_gtext__DOT__gfx_pixel__39__y) 
                                           + __Vfunc_test_gtext__DOT__gfx_pixel__39__x)))
                            ? vlSelfRef.test_gtext__DOT__dut__DOT__gfx_inst__DOT__gfx_mem__DOT__mem
                           [(0x0000ffffU & (VL_MULS_III(32, (IData)(0x00000140U), __Vfunc_test_gtext__DOT__gfx_pixel__39__y) 
                                            + __Vfunc_test_gtext__DOT__gfx_pixel__39__x))]
                            : 0U);
                }(), (IData)(__Vfunc_test_gtext__DOT__gfx_pixel__39__Vfuncout)))) {
        vlSelfRef.test_gtext__DOT__unnamedblk1__DOT__pixel_count 
            = ((IData)(1U) + vlSelfRef.test_gtext__DOT__unnamedblk1__DOT__pixel_count);
    }
    if ((0U != ([&]() {
                    __Vfunc_test_gtext__DOT__gfx_pixel__39__y = 7U;
                    __Vfunc_test_gtext__DOT__gfx_pixel__39__x = 6U;
                    __Vfunc_test_gtext__DOT__gfx_pixel__39__Vfuncout = 0;
                    __Vfunc_test_gtext__DOT__gfx_pixel__39__Vfuncout 
                        = ((0xf9ffU >= (0x0000ffffU 
                                        & (VL_MULS_III(32, (IData)(0x00000140U), __Vfunc_test_gtext__DOT__gfx_pixel__39__y) 
                                           + __Vfunc_test_gtext__DOT__gfx_pixel__39__x)))
                            ? vlSelfRef.test_gtext__DOT__dut__DOT__gfx_inst__DOT__gfx_mem__DOT__mem
                           [(0x0000ffffU & (VL_MULS_III(32, (IData)(0x00000140U), __Vfunc_test_gtext__DOT__gfx_pixel__39__y) 
                                            + __Vfunc_test_gtext__DOT__gfx_pixel__39__x))]
                            : 0U);
                }(), (IData)(__Vfunc_test_gtext__DOT__gfx_pixel__39__Vfuncout)))) {
        vlSelfRef.test_gtext__DOT__unnamedblk1__DOT__pixel_count 
            = ((IData)(1U) + vlSelfRef.test_gtext__DOT__unnamedblk1__DOT__pixel_count);
    }
    if ((0U != ([&]() {
                    __Vfunc_test_gtext__DOT__gfx_pixel__39__y = 7U;
                    __Vfunc_test_gtext__DOT__gfx_pixel__39__x = 7U;
                    __Vfunc_test_gtext__DOT__gfx_pixel__39__Vfuncout = 0;
                    __Vfunc_test_gtext__DOT__gfx_pixel__39__Vfuncout 
                        = ((0xf9ffU >= (0x0000ffffU 
                                        & (VL_MULS_III(32, (IData)(0x00000140U), __Vfunc_test_gtext__DOT__gfx_pixel__39__y) 
                                           + __Vfunc_test_gtext__DOT__gfx_pixel__39__x)))
                            ? vlSelfRef.test_gtext__DOT__dut__DOT__gfx_inst__DOT__gfx_mem__DOT__mem
                           [(0x0000ffffU & (VL_MULS_III(32, (IData)(0x00000140U), __Vfunc_test_gtext__DOT__gfx_pixel__39__y) 
                                            + __Vfunc_test_gtext__DOT__gfx_pixel__39__x))]
                            : 0U);
                }(), (IData)(__Vfunc_test_gtext__DOT__gfx_pixel__39__Vfuncout)))) {
        vlSelfRef.test_gtext__DOT__unnamedblk1__DOT__pixel_count 
            = ((IData)(1U) + vlSelfRef.test_gtext__DOT__unnamedblk1__DOT__pixel_count);
    }
    __Vtask_test_gtext__DOT__check__40__condition = 
        VL_LTS_III(32, 0U, vlSelfRef.test_gtext__DOT__unnamedblk1__DOT__pixel_count);
    vlSelfRef.__Vtask_test_gtext__DOT__check__40__name = "'A' rendered: has set pixels"s;
    vlSelfRef.test_gtext__DOT__test_num = ((IData)(1U) 
                                           + vlSelfRef.test_gtext__DOT__test_num);
    if (__Vtask_test_gtext__DOT__check__40__condition) {
        VL_WRITEF_NX("  PASS [%0d] %@\n",0,32,vlSelfRef.test_gtext__DOT__test_num,
                     -1,&(vlSelfRef.__Vtask_test_gtext__DOT__check__40__name));
        vlSelfRef.test_gtext__DOT__pass_count = ((IData)(1U) 
                                                 + vlSelfRef.test_gtext__DOT__pass_count);
    } else {
        VL_WRITEF_NX("  FAIL [%0d] %@\n",0,32,vlSelfRef.test_gtext__DOT__test_num,
                     -1,&(vlSelfRef.__Vtask_test_gtext__DOT__check__40__name));
        vlSelfRef.test_gtext__DOT__fail_count = ((IData)(1U) 
                                                 + vlSelfRef.test_gtext__DOT__fail_count);
    }
    __Vtask_test_gtext__DOT__check__41__condition = 
        VL_GTS_III(32, 0x00000040U, vlSelfRef.test_gtext__DOT__unnamedblk1__DOT__pixel_count);
    vlSelfRef.__Vtask_test_gtext__DOT__check__41__name = "'A' rendered: not all pixels set"s;
    vlSelfRef.test_gtext__DOT__test_num = ((IData)(1U) 
                                           + vlSelfRef.test_gtext__DOT__test_num);
    if (__Vtask_test_gtext__DOT__check__41__condition) {
        VL_WRITEF_NX("  PASS [%0d] %@\n",0,32,vlSelfRef.test_gtext__DOT__test_num,
                     -1,&(vlSelfRef.__Vtask_test_gtext__DOT__check__41__name));
        vlSelfRef.test_gtext__DOT__pass_count = ((IData)(1U) 
                                                 + vlSelfRef.test_gtext__DOT__pass_count);
    } else {
        VL_WRITEF_NX("  FAIL [%0d] %@\n",0,32,vlSelfRef.test_gtext__DOT__test_num,
                     -1,&(vlSelfRef.__Vtask_test_gtext__DOT__check__41__name));
        vlSelfRef.test_gtext__DOT__fail_count = ((IData)(1U) 
                                                 + vlSelfRef.test_gtext__DOT__fail_count);
    }
    vlSelfRef.test_gtext__DOT__unnamedblk4__DOT__match = 1U;
    vlSelfRef.test_gtext__DOT__unnamedblk4__DOT__unnamedblk5__DOT__unnamedblk6__DOT__fb 
        = vlSelfRef.test_gtext__DOT__dut__DOT__text_inst__DOT__font_mem__DOT__mem[520U];
    vlSelfRef.test_gtext__DOT__unnamedblk4__DOT__unnamedblk5__DOT__unnamedblk6__DOT__unnamedblk7__DOT__unnamedblk8__DOT__expected 
        = (1U & ((IData)(vlSelfRef.test_gtext__DOT__unnamedblk4__DOT__unnamedblk5__DOT__unnamedblk6__DOT__fb) 
                 >> 7U));
    if (((IData)(vlSelfRef.test_gtext__DOT__unnamedblk4__DOT__unnamedblk5__DOT__unnamedblk6__DOT__unnamedblk7__DOT__unnamedblk8__DOT__expected) 
         & (1U != ([&]() {
                        __Vfunc_test_gtext__DOT__gfx_pixel__42__y = 0U;
                        __Vfunc_test_gtext__DOT__gfx_pixel__42__x = 0U;
                        __Vfunc_test_gtext__DOT__gfx_pixel__42__Vfuncout = 0;
                        __Vfunc_test_gtext__DOT__gfx_pixel__42__Vfuncout 
                            = ((0xf9ffU >= (0x0000ffffU 
                                            & (VL_MULS_III(32, (IData)(0x00000140U), __Vfunc_test_gtext__DOT__gfx_pixel__42__y) 
                                               + __Vfunc_test_gtext__DOT__gfx_pixel__42__x)))
                                ? vlSelfRef.test_gtext__DOT__dut__DOT__gfx_inst__DOT__gfx_mem__DOT__mem
                               [(0x0000ffffU & (VL_MULS_III(32, (IData)(0x00000140U), __Vfunc_test_gtext__DOT__gfx_pixel__42__y) 
                                                + __Vfunc_test_gtext__DOT__gfx_pixel__42__x))]
                                : 0U);
                    }(), (IData)(__Vfunc_test_gtext__DOT__gfx_pixel__42__Vfuncout))))) {
        vlSelfRef.test_gtext__DOT__unnamedblk4__DOT__match = 0U;
    }
    if (((~ (IData)(vlSelfRef.test_gtext__DOT__unnamedblk4__DOT__unnamedblk5__DOT__unnamedblk6__DOT__unnamedblk7__DOT__unnamedblk8__DOT__expected)) 
         & (0U != ([&]() {
                        __Vfunc_test_gtext__DOT__gfx_pixel__43__y = 0U;
                        __Vfunc_test_gtext__DOT__gfx_pixel__43__x = 0U;
                        __Vfunc_test_gtext__DOT__gfx_pixel__43__Vfuncout = 0;
                        __Vfunc_test_gtext__DOT__gfx_pixel__43__Vfuncout 
                            = ((0xf9ffU >= (0x0000ffffU 
                                            & (VL_MULS_III(32, (IData)(0x00000140U), __Vfunc_test_gtext__DOT__gfx_pixel__43__y) 
                                               + __Vfunc_test_gtext__DOT__gfx_pixel__43__x)))
                                ? vlSelfRef.test_gtext__DOT__dut__DOT__gfx_inst__DOT__gfx_mem__DOT__mem
                               [(0x0000ffffU & (VL_MULS_III(32, (IData)(0x00000140U), __Vfunc_test_gtext__DOT__gfx_pixel__43__y) 
                                                + __Vfunc_test_gtext__DOT__gfx_pixel__43__x))]
                                : 0U);
                    }(), (IData)(__Vfunc_test_gtext__DOT__gfx_pixel__43__Vfuncout))))) {
        vlSelfRef.test_gtext__DOT__unnamedblk4__DOT__match = 0U;
    }
    vlSelfRef.test_gtext__DOT__unnamedblk4__DOT__unnamedblk5__DOT__unnamedblk6__DOT__unnamedblk7__DOT__unnamedblk8__DOT__expected 
        = (1U & ((IData)(vlSelfRef.test_gtext__DOT__unnamedblk4__DOT__unnamedblk5__DOT__unnamedblk6__DOT__fb) 
                 >> 6U));
    if (((IData)(vlSelfRef.test_gtext__DOT__unnamedblk4__DOT__unnamedblk5__DOT__unnamedblk6__DOT__unnamedblk7__DOT__unnamedblk8__DOT__expected) 
         & (1U != ([&]() {
                        __Vfunc_test_gtext__DOT__gfx_pixel__42__y = 0U;
                        __Vfunc_test_gtext__DOT__gfx_pixel__42__x = 1U;
                        __Vfunc_test_gtext__DOT__gfx_pixel__42__Vfuncout = 0;
                        __Vfunc_test_gtext__DOT__gfx_pixel__42__Vfuncout 
                            = ((0xf9ffU >= (0x0000ffffU 
                                            & (VL_MULS_III(32, (IData)(0x00000140U), __Vfunc_test_gtext__DOT__gfx_pixel__42__y) 
                                               + __Vfunc_test_gtext__DOT__gfx_pixel__42__x)))
                                ? vlSelfRef.test_gtext__DOT__dut__DOT__gfx_inst__DOT__gfx_mem__DOT__mem
                               [(0x0000ffffU & (VL_MULS_III(32, (IData)(0x00000140U), __Vfunc_test_gtext__DOT__gfx_pixel__42__y) 
                                                + __Vfunc_test_gtext__DOT__gfx_pixel__42__x))]
                                : 0U);
                    }(), (IData)(__Vfunc_test_gtext__DOT__gfx_pixel__42__Vfuncout))))) {
        vlSelfRef.test_gtext__DOT__unnamedblk4__DOT__match = 0U;
    }
    if (((~ (IData)(vlSelfRef.test_gtext__DOT__unnamedblk4__DOT__unnamedblk5__DOT__unnamedblk6__DOT__unnamedblk7__DOT__unnamedblk8__DOT__expected)) 
         & (0U != ([&]() {
                        __Vfunc_test_gtext__DOT__gfx_pixel__43__y = 0U;
                        __Vfunc_test_gtext__DOT__gfx_pixel__43__x = 1U;
                        __Vfunc_test_gtext__DOT__gfx_pixel__43__Vfuncout = 0;
                        __Vfunc_test_gtext__DOT__gfx_pixel__43__Vfuncout 
                            = ((0xf9ffU >= (0x0000ffffU 
                                            & (VL_MULS_III(32, (IData)(0x00000140U), __Vfunc_test_gtext__DOT__gfx_pixel__43__y) 
                                               + __Vfunc_test_gtext__DOT__gfx_pixel__43__x)))
                                ? vlSelfRef.test_gtext__DOT__dut__DOT__gfx_inst__DOT__gfx_mem__DOT__mem
                               [(0x0000ffffU & (VL_MULS_III(32, (IData)(0x00000140U), __Vfunc_test_gtext__DOT__gfx_pixel__43__y) 
                                                + __Vfunc_test_gtext__DOT__gfx_pixel__43__x))]
                                : 0U);
                    }(), (IData)(__Vfunc_test_gtext__DOT__gfx_pixel__43__Vfuncout))))) {
        vlSelfRef.test_gtext__DOT__unnamedblk4__DOT__match = 0U;
    }
    vlSelfRef.test_gtext__DOT__unnamedblk4__DOT__unnamedblk5__DOT__unnamedblk6__DOT__unnamedblk7__DOT__unnamedblk8__DOT__expected 
        = (1U & ((IData)(vlSelfRef.test_gtext__DOT__unnamedblk4__DOT__unnamedblk5__DOT__unnamedblk6__DOT__fb) 
                 >> 5U));
    if (((IData)(vlSelfRef.test_gtext__DOT__unnamedblk4__DOT__unnamedblk5__DOT__unnamedblk6__DOT__unnamedblk7__DOT__unnamedblk8__DOT__expected) 
         & (1U != ([&]() {
                        __Vfunc_test_gtext__DOT__gfx_pixel__42__y = 0U;
                        __Vfunc_test_gtext__DOT__gfx_pixel__42__x = 2U;
                        __Vfunc_test_gtext__DOT__gfx_pixel__42__Vfuncout = 0;
                        __Vfunc_test_gtext__DOT__gfx_pixel__42__Vfuncout 
                            = ((0xf9ffU >= (0x0000ffffU 
                                            & (VL_MULS_III(32, (IData)(0x00000140U), __Vfunc_test_gtext__DOT__gfx_pixel__42__y) 
                                               + __Vfunc_test_gtext__DOT__gfx_pixel__42__x)))
                                ? vlSelfRef.test_gtext__DOT__dut__DOT__gfx_inst__DOT__gfx_mem__DOT__mem
                               [(0x0000ffffU & (VL_MULS_III(32, (IData)(0x00000140U), __Vfunc_test_gtext__DOT__gfx_pixel__42__y) 
                                                + __Vfunc_test_gtext__DOT__gfx_pixel__42__x))]
                                : 0U);
                    }(), (IData)(__Vfunc_test_gtext__DOT__gfx_pixel__42__Vfuncout))))) {
        vlSelfRef.test_gtext__DOT__unnamedblk4__DOT__match = 0U;
    }
    if (((~ (IData)(vlSelfRef.test_gtext__DOT__unnamedblk4__DOT__unnamedblk5__DOT__unnamedblk6__DOT__unnamedblk7__DOT__unnamedblk8__DOT__expected)) 
         & (0U != ([&]() {
                        __Vfunc_test_gtext__DOT__gfx_pixel__43__y = 0U;
                        __Vfunc_test_gtext__DOT__gfx_pixel__43__x = 2U;
                        __Vfunc_test_gtext__DOT__gfx_pixel__43__Vfuncout = 0;
                        __Vfunc_test_gtext__DOT__gfx_pixel__43__Vfuncout 
                            = ((0xf9ffU >= (0x0000ffffU 
                                            & (VL_MULS_III(32, (IData)(0x00000140U), __Vfunc_test_gtext__DOT__gfx_pixel__43__y) 
                                               + __Vfunc_test_gtext__DOT__gfx_pixel__43__x)))
                                ? vlSelfRef.test_gtext__DOT__dut__DOT__gfx_inst__DOT__gfx_mem__DOT__mem
                               [(0x0000ffffU & (VL_MULS_III(32, (IData)(0x00000140U), __Vfunc_test_gtext__DOT__gfx_pixel__43__y) 
                                                + __Vfunc_test_gtext__DOT__gfx_pixel__43__x))]
                                : 0U);
                    }(), (IData)(__Vfunc_test_gtext__DOT__gfx_pixel__43__Vfuncout))))) {
        vlSelfRef.test_gtext__DOT__unnamedblk4__DOT__match = 0U;
    }
    vlSelfRef.test_gtext__DOT__unnamedblk4__DOT__unnamedblk5__DOT__unnamedblk6__DOT__unnamedblk7__DOT__unnamedblk8__DOT__expected 
        = (1U & ((IData)(vlSelfRef.test_gtext__DOT__unnamedblk4__DOT__unnamedblk5__DOT__unnamedblk6__DOT__fb) 
                 >> 4U));
    if (((IData)(vlSelfRef.test_gtext__DOT__unnamedblk4__DOT__unnamedblk5__DOT__unnamedblk6__DOT__unnamedblk7__DOT__unnamedblk8__DOT__expected) 
         & (1U != ([&]() {
                        __Vfunc_test_gtext__DOT__gfx_pixel__42__y = 0U;
                        __Vfunc_test_gtext__DOT__gfx_pixel__42__x = 3U;
                        __Vfunc_test_gtext__DOT__gfx_pixel__42__Vfuncout = 0;
                        __Vfunc_test_gtext__DOT__gfx_pixel__42__Vfuncout 
                            = ((0xf9ffU >= (0x0000ffffU 
                                            & (VL_MULS_III(32, (IData)(0x00000140U), __Vfunc_test_gtext__DOT__gfx_pixel__42__y) 
                                               + __Vfunc_test_gtext__DOT__gfx_pixel__42__x)))
                                ? vlSelfRef.test_gtext__DOT__dut__DOT__gfx_inst__DOT__gfx_mem__DOT__mem
                               [(0x0000ffffU & (VL_MULS_III(32, (IData)(0x00000140U), __Vfunc_test_gtext__DOT__gfx_pixel__42__y) 
                                                + __Vfunc_test_gtext__DOT__gfx_pixel__42__x))]
                                : 0U);
                    }(), (IData)(__Vfunc_test_gtext__DOT__gfx_pixel__42__Vfuncout))))) {
        vlSelfRef.test_gtext__DOT__unnamedblk4__DOT__match = 0U;
    }
    if (((~ (IData)(vlSelfRef.test_gtext__DOT__unnamedblk4__DOT__unnamedblk5__DOT__unnamedblk6__DOT__unnamedblk7__DOT__unnamedblk8__DOT__expected)) 
         & (0U != ([&]() {
                        __Vfunc_test_gtext__DOT__gfx_pixel__43__y = 0U;
                        __Vfunc_test_gtext__DOT__gfx_pixel__43__x = 3U;
                        __Vfunc_test_gtext__DOT__gfx_pixel__43__Vfuncout = 0;
                        __Vfunc_test_gtext__DOT__gfx_pixel__43__Vfuncout 
                            = ((0xf9ffU >= (0x0000ffffU 
                                            & (VL_MULS_III(32, (IData)(0x00000140U), __Vfunc_test_gtext__DOT__gfx_pixel__43__y) 
                                               + __Vfunc_test_gtext__DOT__gfx_pixel__43__x)))
                                ? vlSelfRef.test_gtext__DOT__dut__DOT__gfx_inst__DOT__gfx_mem__DOT__mem
                               [(0x0000ffffU & (VL_MULS_III(32, (IData)(0x00000140U), __Vfunc_test_gtext__DOT__gfx_pixel__43__y) 
                                                + __Vfunc_test_gtext__DOT__gfx_pixel__43__x))]
                                : 0U);
                    }(), (IData)(__Vfunc_test_gtext__DOT__gfx_pixel__43__Vfuncout))))) {
        vlSelfRef.test_gtext__DOT__unnamedblk4__DOT__match = 0U;
    }
    vlSelfRef.test_gtext__DOT__unnamedblk4__DOT__unnamedblk5__DOT__unnamedblk6__DOT__unnamedblk7__DOT__unnamedblk8__DOT__expected 
        = (1U & ((IData)(vlSelfRef.test_gtext__DOT__unnamedblk4__DOT__unnamedblk5__DOT__unnamedblk6__DOT__fb) 
                 >> 3U));
    if (((IData)(vlSelfRef.test_gtext__DOT__unnamedblk4__DOT__unnamedblk5__DOT__unnamedblk6__DOT__unnamedblk7__DOT__unnamedblk8__DOT__expected) 
         & (1U != ([&]() {
                        __Vfunc_test_gtext__DOT__gfx_pixel__42__y = 0U;
                        __Vfunc_test_gtext__DOT__gfx_pixel__42__x = 4U;
                        __Vfunc_test_gtext__DOT__gfx_pixel__42__Vfuncout = 0;
                        __Vfunc_test_gtext__DOT__gfx_pixel__42__Vfuncout 
                            = ((0xf9ffU >= (0x0000ffffU 
                                            & (VL_MULS_III(32, (IData)(0x00000140U), __Vfunc_test_gtext__DOT__gfx_pixel__42__y) 
                                               + __Vfunc_test_gtext__DOT__gfx_pixel__42__x)))
                                ? vlSelfRef.test_gtext__DOT__dut__DOT__gfx_inst__DOT__gfx_mem__DOT__mem
                               [(0x0000ffffU & (VL_MULS_III(32, (IData)(0x00000140U), __Vfunc_test_gtext__DOT__gfx_pixel__42__y) 
                                                + __Vfunc_test_gtext__DOT__gfx_pixel__42__x))]
                                : 0U);
                    }(), (IData)(__Vfunc_test_gtext__DOT__gfx_pixel__42__Vfuncout))))) {
        vlSelfRef.test_gtext__DOT__unnamedblk4__DOT__match = 0U;
    }
    if (((~ (IData)(vlSelfRef.test_gtext__DOT__unnamedblk4__DOT__unnamedblk5__DOT__unnamedblk6__DOT__unnamedblk7__DOT__unnamedblk8__DOT__expected)) 
         & (0U != ([&]() {
                        __Vfunc_test_gtext__DOT__gfx_pixel__43__y = 0U;
                        __Vfunc_test_gtext__DOT__gfx_pixel__43__x = 4U;
                        __Vfunc_test_gtext__DOT__gfx_pixel__43__Vfuncout = 0;
                        __Vfunc_test_gtext__DOT__gfx_pixel__43__Vfuncout 
                            = ((0xf9ffU >= (0x0000ffffU 
                                            & (VL_MULS_III(32, (IData)(0x00000140U), __Vfunc_test_gtext__DOT__gfx_pixel__43__y) 
                                               + __Vfunc_test_gtext__DOT__gfx_pixel__43__x)))
                                ? vlSelfRef.test_gtext__DOT__dut__DOT__gfx_inst__DOT__gfx_mem__DOT__mem
                               [(0x0000ffffU & (VL_MULS_III(32, (IData)(0x00000140U), __Vfunc_test_gtext__DOT__gfx_pixel__43__y) 
                                                + __Vfunc_test_gtext__DOT__gfx_pixel__43__x))]
                                : 0U);
                    }(), (IData)(__Vfunc_test_gtext__DOT__gfx_pixel__43__Vfuncout))))) {
        vlSelfRef.test_gtext__DOT__unnamedblk4__DOT__match = 0U;
    }
    vlSelfRef.test_gtext__DOT__unnamedblk4__DOT__unnamedblk5__DOT__unnamedblk6__DOT__unnamedblk7__DOT__unnamedblk8__DOT__expected 
        = (1U & ((IData)(vlSelfRef.test_gtext__DOT__unnamedblk4__DOT__unnamedblk5__DOT__unnamedblk6__DOT__fb) 
                 >> 2U));
    if (((IData)(vlSelfRef.test_gtext__DOT__unnamedblk4__DOT__unnamedblk5__DOT__unnamedblk6__DOT__unnamedblk7__DOT__unnamedblk8__DOT__expected) 
         & (1U != ([&]() {
                        __Vfunc_test_gtext__DOT__gfx_pixel__42__y = 0U;
                        __Vfunc_test_gtext__DOT__gfx_pixel__42__x = 5U;
                        __Vfunc_test_gtext__DOT__gfx_pixel__42__Vfuncout = 0;
                        __Vfunc_test_gtext__DOT__gfx_pixel__42__Vfuncout 
                            = ((0xf9ffU >= (0x0000ffffU 
                                            & (VL_MULS_III(32, (IData)(0x00000140U), __Vfunc_test_gtext__DOT__gfx_pixel__42__y) 
                                               + __Vfunc_test_gtext__DOT__gfx_pixel__42__x)))
                                ? vlSelfRef.test_gtext__DOT__dut__DOT__gfx_inst__DOT__gfx_mem__DOT__mem
                               [(0x0000ffffU & (VL_MULS_III(32, (IData)(0x00000140U), __Vfunc_test_gtext__DOT__gfx_pixel__42__y) 
                                                + __Vfunc_test_gtext__DOT__gfx_pixel__42__x))]
                                : 0U);
                    }(), (IData)(__Vfunc_test_gtext__DOT__gfx_pixel__42__Vfuncout))))) {
        vlSelfRef.test_gtext__DOT__unnamedblk4__DOT__match = 0U;
    }
    if (((~ (IData)(vlSelfRef.test_gtext__DOT__unnamedblk4__DOT__unnamedblk5__DOT__unnamedblk6__DOT__unnamedblk7__DOT__unnamedblk8__DOT__expected)) 
         & (0U != ([&]() {
                        __Vfunc_test_gtext__DOT__gfx_pixel__43__y = 0U;
                        __Vfunc_test_gtext__DOT__gfx_pixel__43__x = 5U;
                        __Vfunc_test_gtext__DOT__gfx_pixel__43__Vfuncout = 0;
                        __Vfunc_test_gtext__DOT__gfx_pixel__43__Vfuncout 
                            = ((0xf9ffU >= (0x0000ffffU 
                                            & (VL_MULS_III(32, (IData)(0x00000140U), __Vfunc_test_gtext__DOT__gfx_pixel__43__y) 
                                               + __Vfunc_test_gtext__DOT__gfx_pixel__43__x)))
                                ? vlSelfRef.test_gtext__DOT__dut__DOT__gfx_inst__DOT__gfx_mem__DOT__mem
                               [(0x0000ffffU & (VL_MULS_III(32, (IData)(0x00000140U), __Vfunc_test_gtext__DOT__gfx_pixel__43__y) 
                                                + __Vfunc_test_gtext__DOT__gfx_pixel__43__x))]
                                : 0U);
                    }(), (IData)(__Vfunc_test_gtext__DOT__gfx_pixel__43__Vfuncout))))) {
        vlSelfRef.test_gtext__DOT__unnamedblk4__DOT__match = 0U;
    }
    vlSelfRef.test_gtext__DOT__unnamedblk4__DOT__unnamedblk5__DOT__unnamedblk6__DOT__unnamedblk7__DOT__unnamedblk8__DOT__expected 
        = (1U & ((IData)(vlSelfRef.test_gtext__DOT__unnamedblk4__DOT__unnamedblk5__DOT__unnamedblk6__DOT__fb) 
                 >> 1U));
    if (((IData)(vlSelfRef.test_gtext__DOT__unnamedblk4__DOT__unnamedblk5__DOT__unnamedblk6__DOT__unnamedblk7__DOT__unnamedblk8__DOT__expected) 
         & (1U != ([&]() {
                        __Vfunc_test_gtext__DOT__gfx_pixel__42__y = 0U;
                        __Vfunc_test_gtext__DOT__gfx_pixel__42__x = 6U;
                        __Vfunc_test_gtext__DOT__gfx_pixel__42__Vfuncout = 0;
                        __Vfunc_test_gtext__DOT__gfx_pixel__42__Vfuncout 
                            = ((0xf9ffU >= (0x0000ffffU 
                                            & (VL_MULS_III(32, (IData)(0x00000140U), __Vfunc_test_gtext__DOT__gfx_pixel__42__y) 
                                               + __Vfunc_test_gtext__DOT__gfx_pixel__42__x)))
                                ? vlSelfRef.test_gtext__DOT__dut__DOT__gfx_inst__DOT__gfx_mem__DOT__mem
                               [(0x0000ffffU & (VL_MULS_III(32, (IData)(0x00000140U), __Vfunc_test_gtext__DOT__gfx_pixel__42__y) 
                                                + __Vfunc_test_gtext__DOT__gfx_pixel__42__x))]
                                : 0U);
                    }(), (IData)(__Vfunc_test_gtext__DOT__gfx_pixel__42__Vfuncout))))) {
        vlSelfRef.test_gtext__DOT__unnamedblk4__DOT__match = 0U;
    }
    if (((~ (IData)(vlSelfRef.test_gtext__DOT__unnamedblk4__DOT__unnamedblk5__DOT__unnamedblk6__DOT__unnamedblk7__DOT__unnamedblk8__DOT__expected)) 
         & (0U != ([&]() {
                        __Vfunc_test_gtext__DOT__gfx_pixel__43__y = 0U;
                        __Vfunc_test_gtext__DOT__gfx_pixel__43__x = 6U;
                        __Vfunc_test_gtext__DOT__gfx_pixel__43__Vfuncout = 0;
                        __Vfunc_test_gtext__DOT__gfx_pixel__43__Vfuncout 
                            = ((0xf9ffU >= (0x0000ffffU 
                                            & (VL_MULS_III(32, (IData)(0x00000140U), __Vfunc_test_gtext__DOT__gfx_pixel__43__y) 
                                               + __Vfunc_test_gtext__DOT__gfx_pixel__43__x)))
                                ? vlSelfRef.test_gtext__DOT__dut__DOT__gfx_inst__DOT__gfx_mem__DOT__mem
                               [(0x0000ffffU & (VL_MULS_III(32, (IData)(0x00000140U), __Vfunc_test_gtext__DOT__gfx_pixel__43__y) 
                                                + __Vfunc_test_gtext__DOT__gfx_pixel__43__x))]
                                : 0U);
                    }(), (IData)(__Vfunc_test_gtext__DOT__gfx_pixel__43__Vfuncout))))) {
        vlSelfRef.test_gtext__DOT__unnamedblk4__DOT__match = 0U;
    }
    vlSelfRef.test_gtext__DOT__unnamedblk4__DOT__unnamedblk5__DOT__unnamedblk6__DOT__unnamedblk7__DOT__unnamedblk8__DOT__expected 
        = (1U & (IData)(vlSelfRef.test_gtext__DOT__unnamedblk4__DOT__unnamedblk5__DOT__unnamedblk6__DOT__fb));
    if (((IData)(vlSelfRef.test_gtext__DOT__unnamedblk4__DOT__unnamedblk5__DOT__unnamedblk6__DOT__unnamedblk7__DOT__unnamedblk8__DOT__expected) 
         & (1U != ([&]() {
                        __Vfunc_test_gtext__DOT__gfx_pixel__42__y = 0U;
                        __Vfunc_test_gtext__DOT__gfx_pixel__42__x = 7U;
                        __Vfunc_test_gtext__DOT__gfx_pixel__42__Vfuncout = 0;
                        __Vfunc_test_gtext__DOT__gfx_pixel__42__Vfuncout 
                            = ((0xf9ffU >= (0x0000ffffU 
                                            & (VL_MULS_III(32, (IData)(0x00000140U), __Vfunc_test_gtext__DOT__gfx_pixel__42__y) 
                                               + __Vfunc_test_gtext__DOT__gfx_pixel__42__x)))
                                ? vlSelfRef.test_gtext__DOT__dut__DOT__gfx_inst__DOT__gfx_mem__DOT__mem
                               [(0x0000ffffU & (VL_MULS_III(32, (IData)(0x00000140U), __Vfunc_test_gtext__DOT__gfx_pixel__42__y) 
                                                + __Vfunc_test_gtext__DOT__gfx_pixel__42__x))]
                                : 0U);
                    }(), (IData)(__Vfunc_test_gtext__DOT__gfx_pixel__42__Vfuncout))))) {
        vlSelfRef.test_gtext__DOT__unnamedblk4__DOT__match = 0U;
    }
    if (((~ (IData)(vlSelfRef.test_gtext__DOT__unnamedblk4__DOT__unnamedblk5__DOT__unnamedblk6__DOT__unnamedblk7__DOT__unnamedblk8__DOT__expected)) 
         & (0U != ([&]() {
                        __Vfunc_test_gtext__DOT__gfx_pixel__43__y = 0U;
                        __Vfunc_test_gtext__DOT__gfx_pixel__43__x = 7U;
                        __Vfunc_test_gtext__DOT__gfx_pixel__43__Vfuncout = 0;
                        __Vfunc_test_gtext__DOT__gfx_pixel__43__Vfuncout 
                            = ((0xf9ffU >= (0x0000ffffU 
                                            & (VL_MULS_III(32, (IData)(0x00000140U), __Vfunc_test_gtext__DOT__gfx_pixel__43__y) 
                                               + __Vfunc_test_gtext__DOT__gfx_pixel__43__x)))
                                ? vlSelfRef.test_gtext__DOT__dut__DOT__gfx_inst__DOT__gfx_mem__DOT__mem
                               [(0x0000ffffU & (VL_MULS_III(32, (IData)(0x00000140U), __Vfunc_test_gtext__DOT__gfx_pixel__43__y) 
                                                + __Vfunc_test_gtext__DOT__gfx_pixel__43__x))]
                                : 0U);
                    }(), (IData)(__Vfunc_test_gtext__DOT__gfx_pixel__43__Vfuncout))))) {
        vlSelfRef.test_gtext__DOT__unnamedblk4__DOT__match = 0U;
    }
    vlSelfRef.test_gtext__DOT__unnamedblk4__DOT__unnamedblk5__DOT__unnamedblk6__DOT__fb 
        = vlSelfRef.test_gtext__DOT__dut__DOT__text_inst__DOT__font_mem__DOT__mem[521U];
    vlSelfRef.test_gtext__DOT__unnamedblk4__DOT__unnamedblk5__DOT__unnamedblk6__DOT__unnamedblk7__DOT__unnamedblk8__DOT__expected 
        = (1U & ((IData)(vlSelfRef.test_gtext__DOT__unnamedblk4__DOT__unnamedblk5__DOT__unnamedblk6__DOT__fb) 
                 >> 7U));
    if (((IData)(vlSelfRef.test_gtext__DOT__unnamedblk4__DOT__unnamedblk5__DOT__unnamedblk6__DOT__unnamedblk7__DOT__unnamedblk8__DOT__expected) 
         & (1U != ([&]() {
                        __Vfunc_test_gtext__DOT__gfx_pixel__42__y = 1U;
                        __Vfunc_test_gtext__DOT__gfx_pixel__42__x = 0U;
                        __Vfunc_test_gtext__DOT__gfx_pixel__42__Vfuncout = 0;
                        __Vfunc_test_gtext__DOT__gfx_pixel__42__Vfuncout 
                            = ((0xf9ffU >= (0x0000ffffU 
                                            & (VL_MULS_III(32, (IData)(0x00000140U), __Vfunc_test_gtext__DOT__gfx_pixel__42__y) 
                                               + __Vfunc_test_gtext__DOT__gfx_pixel__42__x)))
                                ? vlSelfRef.test_gtext__DOT__dut__DOT__gfx_inst__DOT__gfx_mem__DOT__mem
                               [(0x0000ffffU & (VL_MULS_III(32, (IData)(0x00000140U), __Vfunc_test_gtext__DOT__gfx_pixel__42__y) 
                                                + __Vfunc_test_gtext__DOT__gfx_pixel__42__x))]
                                : 0U);
                    }(), (IData)(__Vfunc_test_gtext__DOT__gfx_pixel__42__Vfuncout))))) {
        vlSelfRef.test_gtext__DOT__unnamedblk4__DOT__match = 0U;
    }
    if (((~ (IData)(vlSelfRef.test_gtext__DOT__unnamedblk4__DOT__unnamedblk5__DOT__unnamedblk6__DOT__unnamedblk7__DOT__unnamedblk8__DOT__expected)) 
         & (0U != ([&]() {
                        __Vfunc_test_gtext__DOT__gfx_pixel__43__y = 1U;
                        __Vfunc_test_gtext__DOT__gfx_pixel__43__x = 0U;
                        __Vfunc_test_gtext__DOT__gfx_pixel__43__Vfuncout = 0;
                        __Vfunc_test_gtext__DOT__gfx_pixel__43__Vfuncout 
                            = ((0xf9ffU >= (0x0000ffffU 
                                            & (VL_MULS_III(32, (IData)(0x00000140U), __Vfunc_test_gtext__DOT__gfx_pixel__43__y) 
                                               + __Vfunc_test_gtext__DOT__gfx_pixel__43__x)))
                                ? vlSelfRef.test_gtext__DOT__dut__DOT__gfx_inst__DOT__gfx_mem__DOT__mem
                               [(0x0000ffffU & (VL_MULS_III(32, (IData)(0x00000140U), __Vfunc_test_gtext__DOT__gfx_pixel__43__y) 
                                                + __Vfunc_test_gtext__DOT__gfx_pixel__43__x))]
                                : 0U);
                    }(), (IData)(__Vfunc_test_gtext__DOT__gfx_pixel__43__Vfuncout))))) {
        vlSelfRef.test_gtext__DOT__unnamedblk4__DOT__match = 0U;
    }
    vlSelfRef.test_gtext__DOT__unnamedblk4__DOT__unnamedblk5__DOT__unnamedblk6__DOT__unnamedblk7__DOT__unnamedblk8__DOT__expected 
        = (1U & ((IData)(vlSelfRef.test_gtext__DOT__unnamedblk4__DOT__unnamedblk5__DOT__unnamedblk6__DOT__fb) 
                 >> 6U));
    if (((IData)(vlSelfRef.test_gtext__DOT__unnamedblk4__DOT__unnamedblk5__DOT__unnamedblk6__DOT__unnamedblk7__DOT__unnamedblk8__DOT__expected) 
         & (1U != ([&]() {
                        __Vfunc_test_gtext__DOT__gfx_pixel__42__y = 1U;
                        __Vfunc_test_gtext__DOT__gfx_pixel__42__x = 1U;
                        __Vfunc_test_gtext__DOT__gfx_pixel__42__Vfuncout = 0;
                        __Vfunc_test_gtext__DOT__gfx_pixel__42__Vfuncout 
                            = ((0xf9ffU >= (0x0000ffffU 
                                            & (VL_MULS_III(32, (IData)(0x00000140U), __Vfunc_test_gtext__DOT__gfx_pixel__42__y) 
                                               + __Vfunc_test_gtext__DOT__gfx_pixel__42__x)))
                                ? vlSelfRef.test_gtext__DOT__dut__DOT__gfx_inst__DOT__gfx_mem__DOT__mem
                               [(0x0000ffffU & (VL_MULS_III(32, (IData)(0x00000140U), __Vfunc_test_gtext__DOT__gfx_pixel__42__y) 
                                                + __Vfunc_test_gtext__DOT__gfx_pixel__42__x))]
                                : 0U);
                    }(), (IData)(__Vfunc_test_gtext__DOT__gfx_pixel__42__Vfuncout))))) {
        vlSelfRef.test_gtext__DOT__unnamedblk4__DOT__match = 0U;
    }
    if (((~ (IData)(vlSelfRef.test_gtext__DOT__unnamedblk4__DOT__unnamedblk5__DOT__unnamedblk6__DOT__unnamedblk7__DOT__unnamedblk8__DOT__expected)) 
         & (0U != ([&]() {
                        __Vfunc_test_gtext__DOT__gfx_pixel__43__y = 1U;
                        __Vfunc_test_gtext__DOT__gfx_pixel__43__x = 1U;
                        __Vfunc_test_gtext__DOT__gfx_pixel__43__Vfuncout = 0;
                        __Vfunc_test_gtext__DOT__gfx_pixel__43__Vfuncout 
                            = ((0xf9ffU >= (0x0000ffffU 
                                            & (VL_MULS_III(32, (IData)(0x00000140U), __Vfunc_test_gtext__DOT__gfx_pixel__43__y) 
                                               + __Vfunc_test_gtext__DOT__gfx_pixel__43__x)))
                                ? vlSelfRef.test_gtext__DOT__dut__DOT__gfx_inst__DOT__gfx_mem__DOT__mem
                               [(0x0000ffffU & (VL_MULS_III(32, (IData)(0x00000140U), __Vfunc_test_gtext__DOT__gfx_pixel__43__y) 
                                                + __Vfunc_test_gtext__DOT__gfx_pixel__43__x))]
                                : 0U);
                    }(), (IData)(__Vfunc_test_gtext__DOT__gfx_pixel__43__Vfuncout))))) {
        vlSelfRef.test_gtext__DOT__unnamedblk4__DOT__match = 0U;
    }
    vlSelfRef.test_gtext__DOT__unnamedblk4__DOT__unnamedblk5__DOT__unnamedblk6__DOT__unnamedblk7__DOT__unnamedblk8__DOT__expected 
        = (1U & ((IData)(vlSelfRef.test_gtext__DOT__unnamedblk4__DOT__unnamedblk5__DOT__unnamedblk6__DOT__fb) 
                 >> 5U));
    if (((IData)(vlSelfRef.test_gtext__DOT__unnamedblk4__DOT__unnamedblk5__DOT__unnamedblk6__DOT__unnamedblk7__DOT__unnamedblk8__DOT__expected) 
         & (1U != ([&]() {
                        __Vfunc_test_gtext__DOT__gfx_pixel__42__y = 1U;
                        __Vfunc_test_gtext__DOT__gfx_pixel__42__x = 2U;
                        __Vfunc_test_gtext__DOT__gfx_pixel__42__Vfuncout = 0;
                        __Vfunc_test_gtext__DOT__gfx_pixel__42__Vfuncout 
                            = ((0xf9ffU >= (0x0000ffffU 
                                            & (VL_MULS_III(32, (IData)(0x00000140U), __Vfunc_test_gtext__DOT__gfx_pixel__42__y) 
                                               + __Vfunc_test_gtext__DOT__gfx_pixel__42__x)))
                                ? vlSelfRef.test_gtext__DOT__dut__DOT__gfx_inst__DOT__gfx_mem__DOT__mem
                               [(0x0000ffffU & (VL_MULS_III(32, (IData)(0x00000140U), __Vfunc_test_gtext__DOT__gfx_pixel__42__y) 
                                                + __Vfunc_test_gtext__DOT__gfx_pixel__42__x))]
                                : 0U);
                    }(), (IData)(__Vfunc_test_gtext__DOT__gfx_pixel__42__Vfuncout))))) {
        vlSelfRef.test_gtext__DOT__unnamedblk4__DOT__match = 0U;
    }
    if (((~ (IData)(vlSelfRef.test_gtext__DOT__unnamedblk4__DOT__unnamedblk5__DOT__unnamedblk6__DOT__unnamedblk7__DOT__unnamedblk8__DOT__expected)) 
         & (0U != ([&]() {
                        __Vfunc_test_gtext__DOT__gfx_pixel__43__y = 1U;
                        __Vfunc_test_gtext__DOT__gfx_pixel__43__x = 2U;
                        __Vfunc_test_gtext__DOT__gfx_pixel__43__Vfuncout = 0;
                        __Vfunc_test_gtext__DOT__gfx_pixel__43__Vfuncout 
                            = ((0xf9ffU >= (0x0000ffffU 
                                            & (VL_MULS_III(32, (IData)(0x00000140U), __Vfunc_test_gtext__DOT__gfx_pixel__43__y) 
                                               + __Vfunc_test_gtext__DOT__gfx_pixel__43__x)))
                                ? vlSelfRef.test_gtext__DOT__dut__DOT__gfx_inst__DOT__gfx_mem__DOT__mem
                               [(0x0000ffffU & (VL_MULS_III(32, (IData)(0x00000140U), __Vfunc_test_gtext__DOT__gfx_pixel__43__y) 
                                                + __Vfunc_test_gtext__DOT__gfx_pixel__43__x))]
                                : 0U);
                    }(), (IData)(__Vfunc_test_gtext__DOT__gfx_pixel__43__Vfuncout))))) {
        vlSelfRef.test_gtext__DOT__unnamedblk4__DOT__match = 0U;
    }
    vlSelfRef.test_gtext__DOT__unnamedblk4__DOT__unnamedblk5__DOT__unnamedblk6__DOT__unnamedblk7__DOT__unnamedblk8__DOT__expected 
        = (1U & ((IData)(vlSelfRef.test_gtext__DOT__unnamedblk4__DOT__unnamedblk5__DOT__unnamedblk6__DOT__fb) 
                 >> 4U));
    if (((IData)(vlSelfRef.test_gtext__DOT__unnamedblk4__DOT__unnamedblk5__DOT__unnamedblk6__DOT__unnamedblk7__DOT__unnamedblk8__DOT__expected) 
         & (1U != ([&]() {
                        __Vfunc_test_gtext__DOT__gfx_pixel__42__y = 1U;
                        __Vfunc_test_gtext__DOT__gfx_pixel__42__x = 3U;
                        __Vfunc_test_gtext__DOT__gfx_pixel__42__Vfuncout = 0;
                        __Vfunc_test_gtext__DOT__gfx_pixel__42__Vfuncout 
                            = ((0xf9ffU >= (0x0000ffffU 
                                            & (VL_MULS_III(32, (IData)(0x00000140U), __Vfunc_test_gtext__DOT__gfx_pixel__42__y) 
                                               + __Vfunc_test_gtext__DOT__gfx_pixel__42__x)))
                                ? vlSelfRef.test_gtext__DOT__dut__DOT__gfx_inst__DOT__gfx_mem__DOT__mem
                               [(0x0000ffffU & (VL_MULS_III(32, (IData)(0x00000140U), __Vfunc_test_gtext__DOT__gfx_pixel__42__y) 
                                                + __Vfunc_test_gtext__DOT__gfx_pixel__42__x))]
                                : 0U);
                    }(), (IData)(__Vfunc_test_gtext__DOT__gfx_pixel__42__Vfuncout))))) {
        vlSelfRef.test_gtext__DOT__unnamedblk4__DOT__match = 0U;
    }
    if (((~ (IData)(vlSelfRef.test_gtext__DOT__unnamedblk4__DOT__unnamedblk5__DOT__unnamedblk6__DOT__unnamedblk7__DOT__unnamedblk8__DOT__expected)) 
         & (0U != ([&]() {
                        __Vfunc_test_gtext__DOT__gfx_pixel__43__y = 1U;
                        __Vfunc_test_gtext__DOT__gfx_pixel__43__x = 3U;
                        __Vfunc_test_gtext__DOT__gfx_pixel__43__Vfuncout = 0;
                        __Vfunc_test_gtext__DOT__gfx_pixel__43__Vfuncout 
                            = ((0xf9ffU >= (0x0000ffffU 
                                            & (VL_MULS_III(32, (IData)(0x00000140U), __Vfunc_test_gtext__DOT__gfx_pixel__43__y) 
                                               + __Vfunc_test_gtext__DOT__gfx_pixel__43__x)))
                                ? vlSelfRef.test_gtext__DOT__dut__DOT__gfx_inst__DOT__gfx_mem__DOT__mem
                               [(0x0000ffffU & (VL_MULS_III(32, (IData)(0x00000140U), __Vfunc_test_gtext__DOT__gfx_pixel__43__y) 
                                                + __Vfunc_test_gtext__DOT__gfx_pixel__43__x))]
                                : 0U);
                    }(), (IData)(__Vfunc_test_gtext__DOT__gfx_pixel__43__Vfuncout))))) {
        vlSelfRef.test_gtext__DOT__unnamedblk4__DOT__match = 0U;
    }
    vlSelfRef.test_gtext__DOT__unnamedblk4__DOT__unnamedblk5__DOT__unnamedblk6__DOT__unnamedblk7__DOT__unnamedblk8__DOT__expected 
        = (1U & ((IData)(vlSelfRef.test_gtext__DOT__unnamedblk4__DOT__unnamedblk5__DOT__unnamedblk6__DOT__fb) 
                 >> 3U));
    if (((IData)(vlSelfRef.test_gtext__DOT__unnamedblk4__DOT__unnamedblk5__DOT__unnamedblk6__DOT__unnamedblk7__DOT__unnamedblk8__DOT__expected) 
         & (1U != ([&]() {
                        __Vfunc_test_gtext__DOT__gfx_pixel__42__y = 1U;
                        __Vfunc_test_gtext__DOT__gfx_pixel__42__x = 4U;
                        __Vfunc_test_gtext__DOT__gfx_pixel__42__Vfuncout = 0;
                        __Vfunc_test_gtext__DOT__gfx_pixel__42__Vfuncout 
                            = ((0xf9ffU >= (0x0000ffffU 
                                            & (VL_MULS_III(32, (IData)(0x00000140U), __Vfunc_test_gtext__DOT__gfx_pixel__42__y) 
                                               + __Vfunc_test_gtext__DOT__gfx_pixel__42__x)))
                                ? vlSelfRef.test_gtext__DOT__dut__DOT__gfx_inst__DOT__gfx_mem__DOT__mem
                               [(0x0000ffffU & (VL_MULS_III(32, (IData)(0x00000140U), __Vfunc_test_gtext__DOT__gfx_pixel__42__y) 
                                                + __Vfunc_test_gtext__DOT__gfx_pixel__42__x))]
                                : 0U);
                    }(), (IData)(__Vfunc_test_gtext__DOT__gfx_pixel__42__Vfuncout))))) {
        vlSelfRef.test_gtext__DOT__unnamedblk4__DOT__match = 0U;
    }
    if (((~ (IData)(vlSelfRef.test_gtext__DOT__unnamedblk4__DOT__unnamedblk5__DOT__unnamedblk6__DOT__unnamedblk7__DOT__unnamedblk8__DOT__expected)) 
         & (0U != ([&]() {
                        __Vfunc_test_gtext__DOT__gfx_pixel__43__y = 1U;
                        __Vfunc_test_gtext__DOT__gfx_pixel__43__x = 4U;
                        __Vfunc_test_gtext__DOT__gfx_pixel__43__Vfuncout = 0;
                        __Vfunc_test_gtext__DOT__gfx_pixel__43__Vfuncout 
                            = ((0xf9ffU >= (0x0000ffffU 
                                            & (VL_MULS_III(32, (IData)(0x00000140U), __Vfunc_test_gtext__DOT__gfx_pixel__43__y) 
                                               + __Vfunc_test_gtext__DOT__gfx_pixel__43__x)))
                                ? vlSelfRef.test_gtext__DOT__dut__DOT__gfx_inst__DOT__gfx_mem__DOT__mem
                               [(0x0000ffffU & (VL_MULS_III(32, (IData)(0x00000140U), __Vfunc_test_gtext__DOT__gfx_pixel__43__y) 
                                                + __Vfunc_test_gtext__DOT__gfx_pixel__43__x))]
                                : 0U);
                    }(), (IData)(__Vfunc_test_gtext__DOT__gfx_pixel__43__Vfuncout))))) {
        vlSelfRef.test_gtext__DOT__unnamedblk4__DOT__match = 0U;
    }
    vlSelfRef.test_gtext__DOT__unnamedblk4__DOT__unnamedblk5__DOT__unnamedblk6__DOT__unnamedblk7__DOT__unnamedblk8__DOT__expected 
        = (1U & ((IData)(vlSelfRef.test_gtext__DOT__unnamedblk4__DOT__unnamedblk5__DOT__unnamedblk6__DOT__fb) 
                 >> 2U));
    if (((IData)(vlSelfRef.test_gtext__DOT__unnamedblk4__DOT__unnamedblk5__DOT__unnamedblk6__DOT__unnamedblk7__DOT__unnamedblk8__DOT__expected) 
         & (1U != ([&]() {
                        __Vfunc_test_gtext__DOT__gfx_pixel__42__y = 1U;
                        __Vfunc_test_gtext__DOT__gfx_pixel__42__x = 5U;
                        __Vfunc_test_gtext__DOT__gfx_pixel__42__Vfuncout = 0;
                        __Vfunc_test_gtext__DOT__gfx_pixel__42__Vfuncout 
                            = ((0xf9ffU >= (0x0000ffffU 
                                            & (VL_MULS_III(32, (IData)(0x00000140U), __Vfunc_test_gtext__DOT__gfx_pixel__42__y) 
                                               + __Vfunc_test_gtext__DOT__gfx_pixel__42__x)))
                                ? vlSelfRef.test_gtext__DOT__dut__DOT__gfx_inst__DOT__gfx_mem__DOT__mem
                               [(0x0000ffffU & (VL_MULS_III(32, (IData)(0x00000140U), __Vfunc_test_gtext__DOT__gfx_pixel__42__y) 
                                                + __Vfunc_test_gtext__DOT__gfx_pixel__42__x))]
                                : 0U);
                    }(), (IData)(__Vfunc_test_gtext__DOT__gfx_pixel__42__Vfuncout))))) {
        vlSelfRef.test_gtext__DOT__unnamedblk4__DOT__match = 0U;
    }
    if (((~ (IData)(vlSelfRef.test_gtext__DOT__unnamedblk4__DOT__unnamedblk5__DOT__unnamedblk6__DOT__unnamedblk7__DOT__unnamedblk8__DOT__expected)) 
         & (0U != ([&]() {
                        __Vfunc_test_gtext__DOT__gfx_pixel__43__y = 1U;
                        __Vfunc_test_gtext__DOT__gfx_pixel__43__x = 5U;
                        __Vfunc_test_gtext__DOT__gfx_pixel__43__Vfuncout = 0;
                        __Vfunc_test_gtext__DOT__gfx_pixel__43__Vfuncout 
                            = ((0xf9ffU >= (0x0000ffffU 
                                            & (VL_MULS_III(32, (IData)(0x00000140U), __Vfunc_test_gtext__DOT__gfx_pixel__43__y) 
                                               + __Vfunc_test_gtext__DOT__gfx_pixel__43__x)))
                                ? vlSelfRef.test_gtext__DOT__dut__DOT__gfx_inst__DOT__gfx_mem__DOT__mem
                               [(0x0000ffffU & (VL_MULS_III(32, (IData)(0x00000140U), __Vfunc_test_gtext__DOT__gfx_pixel__43__y) 
                                                + __Vfunc_test_gtext__DOT__gfx_pixel__43__x))]
                                : 0U);
                    }(), (IData)(__Vfunc_test_gtext__DOT__gfx_pixel__43__Vfuncout))))) {
        vlSelfRef.test_gtext__DOT__unnamedblk4__DOT__match = 0U;
    }
    vlSelfRef.test_gtext__DOT__unnamedblk4__DOT__unnamedblk5__DOT__unnamedblk6__DOT__unnamedblk7__DOT__unnamedblk8__DOT__expected 
        = (1U & ((IData)(vlSelfRef.test_gtext__DOT__unnamedblk4__DOT__unnamedblk5__DOT__unnamedblk6__DOT__fb) 
                 >> 1U));
    if (((IData)(vlSelfRef.test_gtext__DOT__unnamedblk4__DOT__unnamedblk5__DOT__unnamedblk6__DOT__unnamedblk7__DOT__unnamedblk8__DOT__expected) 
         & (1U != ([&]() {
                        __Vfunc_test_gtext__DOT__gfx_pixel__42__y = 1U;
                        __Vfunc_test_gtext__DOT__gfx_pixel__42__x = 6U;
                        __Vfunc_test_gtext__DOT__gfx_pixel__42__Vfuncout = 0;
                        __Vfunc_test_gtext__DOT__gfx_pixel__42__Vfuncout 
                            = ((0xf9ffU >= (0x0000ffffU 
                                            & (VL_MULS_III(32, (IData)(0x00000140U), __Vfunc_test_gtext__DOT__gfx_pixel__42__y) 
                                               + __Vfunc_test_gtext__DOT__gfx_pixel__42__x)))
                                ? vlSelfRef.test_gtext__DOT__dut__DOT__gfx_inst__DOT__gfx_mem__DOT__mem
                               [(0x0000ffffU & (VL_MULS_III(32, (IData)(0x00000140U), __Vfunc_test_gtext__DOT__gfx_pixel__42__y) 
                                                + __Vfunc_test_gtext__DOT__gfx_pixel__42__x))]
                                : 0U);
                    }(), (IData)(__Vfunc_test_gtext__DOT__gfx_pixel__42__Vfuncout))))) {
        vlSelfRef.test_gtext__DOT__unnamedblk4__DOT__match = 0U;
    }
    if (((~ (IData)(vlSelfRef.test_gtext__DOT__unnamedblk4__DOT__unnamedblk5__DOT__unnamedblk6__DOT__unnamedblk7__DOT__unnamedblk8__DOT__expected)) 
         & (0U != ([&]() {
                        __Vfunc_test_gtext__DOT__gfx_pixel__43__y = 1U;
                        __Vfunc_test_gtext__DOT__gfx_pixel__43__x = 6U;
                        __Vfunc_test_gtext__DOT__gfx_pixel__43__Vfuncout = 0;
                        __Vfunc_test_gtext__DOT__gfx_pixel__43__Vfuncout 
                            = ((0xf9ffU >= (0x0000ffffU 
                                            & (VL_MULS_III(32, (IData)(0x00000140U), __Vfunc_test_gtext__DOT__gfx_pixel__43__y) 
                                               + __Vfunc_test_gtext__DOT__gfx_pixel__43__x)))
                                ? vlSelfRef.test_gtext__DOT__dut__DOT__gfx_inst__DOT__gfx_mem__DOT__mem
                               [(0x0000ffffU & (VL_MULS_III(32, (IData)(0x00000140U), __Vfunc_test_gtext__DOT__gfx_pixel__43__y) 
                                                + __Vfunc_test_gtext__DOT__gfx_pixel__43__x))]
                                : 0U);
                    }(), (IData)(__Vfunc_test_gtext__DOT__gfx_pixel__43__Vfuncout))))) {
        vlSelfRef.test_gtext__DOT__unnamedblk4__DOT__match = 0U;
    }
    vlSelfRef.test_gtext__DOT__unnamedblk4__DOT__unnamedblk5__DOT__unnamedblk6__DOT__unnamedblk7__DOT__unnamedblk8__DOT__expected 
        = (1U & (IData)(vlSelfRef.test_gtext__DOT__unnamedblk4__DOT__unnamedblk5__DOT__unnamedblk6__DOT__fb));
    if (((IData)(vlSelfRef.test_gtext__DOT__unnamedblk4__DOT__unnamedblk5__DOT__unnamedblk6__DOT__unnamedblk7__DOT__unnamedblk8__DOT__expected) 
         & (1U != ([&]() {
                        __Vfunc_test_gtext__DOT__gfx_pixel__42__y = 1U;
                        __Vfunc_test_gtext__DOT__gfx_pixel__42__x = 7U;
                        __Vfunc_test_gtext__DOT__gfx_pixel__42__Vfuncout = 0;
                        __Vfunc_test_gtext__DOT__gfx_pixel__42__Vfuncout 
                            = ((0xf9ffU >= (0x0000ffffU 
                                            & (VL_MULS_III(32, (IData)(0x00000140U), __Vfunc_test_gtext__DOT__gfx_pixel__42__y) 
                                               + __Vfunc_test_gtext__DOT__gfx_pixel__42__x)))
                                ? vlSelfRef.test_gtext__DOT__dut__DOT__gfx_inst__DOT__gfx_mem__DOT__mem
                               [(0x0000ffffU & (VL_MULS_III(32, (IData)(0x00000140U), __Vfunc_test_gtext__DOT__gfx_pixel__42__y) 
                                                + __Vfunc_test_gtext__DOT__gfx_pixel__42__x))]
                                : 0U);
                    }(), (IData)(__Vfunc_test_gtext__DOT__gfx_pixel__42__Vfuncout))))) {
        vlSelfRef.test_gtext__DOT__unnamedblk4__DOT__match = 0U;
    }
    if (((~ (IData)(vlSelfRef.test_gtext__DOT__unnamedblk4__DOT__unnamedblk5__DOT__unnamedblk6__DOT__unnamedblk7__DOT__unnamedblk8__DOT__expected)) 
         & (0U != ([&]() {
                        __Vfunc_test_gtext__DOT__gfx_pixel__43__y = 1U;
                        __Vfunc_test_gtext__DOT__gfx_pixel__43__x = 7U;
                        __Vfunc_test_gtext__DOT__gfx_pixel__43__Vfuncout = 0;
                        __Vfunc_test_gtext__DOT__gfx_pixel__43__Vfuncout 
                            = ((0xf9ffU >= (0x0000ffffU 
                                            & (VL_MULS_III(32, (IData)(0x00000140U), __Vfunc_test_gtext__DOT__gfx_pixel__43__y) 
                                               + __Vfunc_test_gtext__DOT__gfx_pixel__43__x)))
                                ? vlSelfRef.test_gtext__DOT__dut__DOT__gfx_inst__DOT__gfx_mem__DOT__mem
                               [(0x0000ffffU & (VL_MULS_III(32, (IData)(0x00000140U), __Vfunc_test_gtext__DOT__gfx_pixel__43__y) 
                                                + __Vfunc_test_gtext__DOT__gfx_pixel__43__x))]
                                : 0U);
                    }(), (IData)(__Vfunc_test_gtext__DOT__gfx_pixel__43__Vfuncout))))) {
        vlSelfRef.test_gtext__DOT__unnamedblk4__DOT__match = 0U;
    }
    vlSelfRef.test_gtext__DOT__unnamedblk4__DOT__unnamedblk5__DOT__unnamedblk6__DOT__fb 
        = vlSelfRef.test_gtext__DOT__dut__DOT__text_inst__DOT__font_mem__DOT__mem[522U];
    vlSelfRef.test_gtext__DOT__unnamedblk4__DOT__unnamedblk5__DOT__unnamedblk6__DOT__unnamedblk7__DOT__unnamedblk8__DOT__expected 
        = (1U & ((IData)(vlSelfRef.test_gtext__DOT__unnamedblk4__DOT__unnamedblk5__DOT__unnamedblk6__DOT__fb) 
                 >> 7U));
    if (((IData)(vlSelfRef.test_gtext__DOT__unnamedblk4__DOT__unnamedblk5__DOT__unnamedblk6__DOT__unnamedblk7__DOT__unnamedblk8__DOT__expected) 
         & (1U != ([&]() {
                        __Vfunc_test_gtext__DOT__gfx_pixel__42__y = 2U;
                        __Vfunc_test_gtext__DOT__gfx_pixel__42__x = 0U;
                        __Vfunc_test_gtext__DOT__gfx_pixel__42__Vfuncout = 0;
                        __Vfunc_test_gtext__DOT__gfx_pixel__42__Vfuncout 
                            = ((0xf9ffU >= (0x0000ffffU 
                                            & (VL_MULS_III(32, (IData)(0x00000140U), __Vfunc_test_gtext__DOT__gfx_pixel__42__y) 
                                               + __Vfunc_test_gtext__DOT__gfx_pixel__42__x)))
                                ? vlSelfRef.test_gtext__DOT__dut__DOT__gfx_inst__DOT__gfx_mem__DOT__mem
                               [(0x0000ffffU & (VL_MULS_III(32, (IData)(0x00000140U), __Vfunc_test_gtext__DOT__gfx_pixel__42__y) 
                                                + __Vfunc_test_gtext__DOT__gfx_pixel__42__x))]
                                : 0U);
                    }(), (IData)(__Vfunc_test_gtext__DOT__gfx_pixel__42__Vfuncout))))) {
        vlSelfRef.test_gtext__DOT__unnamedblk4__DOT__match = 0U;
    }
    if (((~ (IData)(vlSelfRef.test_gtext__DOT__unnamedblk4__DOT__unnamedblk5__DOT__unnamedblk6__DOT__unnamedblk7__DOT__unnamedblk8__DOT__expected)) 
         & (0U != ([&]() {
                        __Vfunc_test_gtext__DOT__gfx_pixel__43__y = 2U;
                        __Vfunc_test_gtext__DOT__gfx_pixel__43__x = 0U;
                        __Vfunc_test_gtext__DOT__gfx_pixel__43__Vfuncout = 0;
                        __Vfunc_test_gtext__DOT__gfx_pixel__43__Vfuncout 
                            = ((0xf9ffU >= (0x0000ffffU 
                                            & (VL_MULS_III(32, (IData)(0x00000140U), __Vfunc_test_gtext__DOT__gfx_pixel__43__y) 
                                               + __Vfunc_test_gtext__DOT__gfx_pixel__43__x)))
                                ? vlSelfRef.test_gtext__DOT__dut__DOT__gfx_inst__DOT__gfx_mem__DOT__mem
                               [(0x0000ffffU & (VL_MULS_III(32, (IData)(0x00000140U), __Vfunc_test_gtext__DOT__gfx_pixel__43__y) 
                                                + __Vfunc_test_gtext__DOT__gfx_pixel__43__x))]
                                : 0U);
                    }(), (IData)(__Vfunc_test_gtext__DOT__gfx_pixel__43__Vfuncout))))) {
        vlSelfRef.test_gtext__DOT__unnamedblk4__DOT__match = 0U;
    }
    vlSelfRef.test_gtext__DOT__unnamedblk4__DOT__unnamedblk5__DOT__unnamedblk6__DOT__unnamedblk7__DOT__unnamedblk8__DOT__expected 
        = (1U & ((IData)(vlSelfRef.test_gtext__DOT__unnamedblk4__DOT__unnamedblk5__DOT__unnamedblk6__DOT__fb) 
                 >> 6U));
    if (((IData)(vlSelfRef.test_gtext__DOT__unnamedblk4__DOT__unnamedblk5__DOT__unnamedblk6__DOT__unnamedblk7__DOT__unnamedblk8__DOT__expected) 
         & (1U != ([&]() {
                        __Vfunc_test_gtext__DOT__gfx_pixel__42__y = 2U;
                        __Vfunc_test_gtext__DOT__gfx_pixel__42__x = 1U;
                        __Vfunc_test_gtext__DOT__gfx_pixel__42__Vfuncout = 0;
                        __Vfunc_test_gtext__DOT__gfx_pixel__42__Vfuncout 
                            = ((0xf9ffU >= (0x0000ffffU 
                                            & (VL_MULS_III(32, (IData)(0x00000140U), __Vfunc_test_gtext__DOT__gfx_pixel__42__y) 
                                               + __Vfunc_test_gtext__DOT__gfx_pixel__42__x)))
                                ? vlSelfRef.test_gtext__DOT__dut__DOT__gfx_inst__DOT__gfx_mem__DOT__mem
                               [(0x0000ffffU & (VL_MULS_III(32, (IData)(0x00000140U), __Vfunc_test_gtext__DOT__gfx_pixel__42__y) 
                                                + __Vfunc_test_gtext__DOT__gfx_pixel__42__x))]
                                : 0U);
                    }(), (IData)(__Vfunc_test_gtext__DOT__gfx_pixel__42__Vfuncout))))) {
        vlSelfRef.test_gtext__DOT__unnamedblk4__DOT__match = 0U;
    }
    if (((~ (IData)(vlSelfRef.test_gtext__DOT__unnamedblk4__DOT__unnamedblk5__DOT__unnamedblk6__DOT__unnamedblk7__DOT__unnamedblk8__DOT__expected)) 
         & (0U != ([&]() {
                        __Vfunc_test_gtext__DOT__gfx_pixel__43__y = 2U;
                        __Vfunc_test_gtext__DOT__gfx_pixel__43__x = 1U;
                        __Vfunc_test_gtext__DOT__gfx_pixel__43__Vfuncout = 0;
                        __Vfunc_test_gtext__DOT__gfx_pixel__43__Vfuncout 
                            = ((0xf9ffU >= (0x0000ffffU 
                                            & (VL_MULS_III(32, (IData)(0x00000140U), __Vfunc_test_gtext__DOT__gfx_pixel__43__y) 
                                               + __Vfunc_test_gtext__DOT__gfx_pixel__43__x)))
                                ? vlSelfRef.test_gtext__DOT__dut__DOT__gfx_inst__DOT__gfx_mem__DOT__mem
                               [(0x0000ffffU & (VL_MULS_III(32, (IData)(0x00000140U), __Vfunc_test_gtext__DOT__gfx_pixel__43__y) 
                                                + __Vfunc_test_gtext__DOT__gfx_pixel__43__x))]
                                : 0U);
                    }(), (IData)(__Vfunc_test_gtext__DOT__gfx_pixel__43__Vfuncout))))) {
        vlSelfRef.test_gtext__DOT__unnamedblk4__DOT__match = 0U;
    }
    vlSelfRef.test_gtext__DOT__unnamedblk4__DOT__unnamedblk5__DOT__unnamedblk6__DOT__unnamedblk7__DOT__unnamedblk8__DOT__expected 
        = (1U & ((IData)(vlSelfRef.test_gtext__DOT__unnamedblk4__DOT__unnamedblk5__DOT__unnamedblk6__DOT__fb) 
                 >> 5U));
    if (((IData)(vlSelfRef.test_gtext__DOT__unnamedblk4__DOT__unnamedblk5__DOT__unnamedblk6__DOT__unnamedblk7__DOT__unnamedblk8__DOT__expected) 
         & (1U != ([&]() {
                        __Vfunc_test_gtext__DOT__gfx_pixel__42__y = 2U;
                        __Vfunc_test_gtext__DOT__gfx_pixel__42__x = 2U;
                        __Vfunc_test_gtext__DOT__gfx_pixel__42__Vfuncout = 0;
                        __Vfunc_test_gtext__DOT__gfx_pixel__42__Vfuncout 
                            = ((0xf9ffU >= (0x0000ffffU 
                                            & (VL_MULS_III(32, (IData)(0x00000140U), __Vfunc_test_gtext__DOT__gfx_pixel__42__y) 
                                               + __Vfunc_test_gtext__DOT__gfx_pixel__42__x)))
                                ? vlSelfRef.test_gtext__DOT__dut__DOT__gfx_inst__DOT__gfx_mem__DOT__mem
                               [(0x0000ffffU & (VL_MULS_III(32, (IData)(0x00000140U), __Vfunc_test_gtext__DOT__gfx_pixel__42__y) 
                                                + __Vfunc_test_gtext__DOT__gfx_pixel__42__x))]
                                : 0U);
                    }(), (IData)(__Vfunc_test_gtext__DOT__gfx_pixel__42__Vfuncout))))) {
        vlSelfRef.test_gtext__DOT__unnamedblk4__DOT__match = 0U;
    }
    if (((~ (IData)(vlSelfRef.test_gtext__DOT__unnamedblk4__DOT__unnamedblk5__DOT__unnamedblk6__DOT__unnamedblk7__DOT__unnamedblk8__DOT__expected)) 
         & (0U != ([&]() {
                        __Vfunc_test_gtext__DOT__gfx_pixel__43__y = 2U;
                        __Vfunc_test_gtext__DOT__gfx_pixel__43__x = 2U;
                        __Vfunc_test_gtext__DOT__gfx_pixel__43__Vfuncout = 0;
                        __Vfunc_test_gtext__DOT__gfx_pixel__43__Vfuncout 
                            = ((0xf9ffU >= (0x0000ffffU 
                                            & (VL_MULS_III(32, (IData)(0x00000140U), __Vfunc_test_gtext__DOT__gfx_pixel__43__y) 
                                               + __Vfunc_test_gtext__DOT__gfx_pixel__43__x)))
                                ? vlSelfRef.test_gtext__DOT__dut__DOT__gfx_inst__DOT__gfx_mem__DOT__mem
                               [(0x0000ffffU & (VL_MULS_III(32, (IData)(0x00000140U), __Vfunc_test_gtext__DOT__gfx_pixel__43__y) 
                                                + __Vfunc_test_gtext__DOT__gfx_pixel__43__x))]
                                : 0U);
                    }(), (IData)(__Vfunc_test_gtext__DOT__gfx_pixel__43__Vfuncout))))) {
        vlSelfRef.test_gtext__DOT__unnamedblk4__DOT__match = 0U;
    }
    vlSelfRef.test_gtext__DOT__unnamedblk4__DOT__unnamedblk5__DOT__unnamedblk6__DOT__unnamedblk7__DOT__unnamedblk8__DOT__expected 
        = (1U & ((IData)(vlSelfRef.test_gtext__DOT__unnamedblk4__DOT__unnamedblk5__DOT__unnamedblk6__DOT__fb) 
                 >> 4U));
    if (((IData)(vlSelfRef.test_gtext__DOT__unnamedblk4__DOT__unnamedblk5__DOT__unnamedblk6__DOT__unnamedblk7__DOT__unnamedblk8__DOT__expected) 
         & (1U != ([&]() {
                        __Vfunc_test_gtext__DOT__gfx_pixel__42__y = 2U;
                        __Vfunc_test_gtext__DOT__gfx_pixel__42__x = 3U;
                        __Vfunc_test_gtext__DOT__gfx_pixel__42__Vfuncout = 0;
                        __Vfunc_test_gtext__DOT__gfx_pixel__42__Vfuncout 
                            = ((0xf9ffU >= (0x0000ffffU 
                                            & (VL_MULS_III(32, (IData)(0x00000140U), __Vfunc_test_gtext__DOT__gfx_pixel__42__y) 
                                               + __Vfunc_test_gtext__DOT__gfx_pixel__42__x)))
                                ? vlSelfRef.test_gtext__DOT__dut__DOT__gfx_inst__DOT__gfx_mem__DOT__mem
                               [(0x0000ffffU & (VL_MULS_III(32, (IData)(0x00000140U), __Vfunc_test_gtext__DOT__gfx_pixel__42__y) 
                                                + __Vfunc_test_gtext__DOT__gfx_pixel__42__x))]
                                : 0U);
                    }(), (IData)(__Vfunc_test_gtext__DOT__gfx_pixel__42__Vfuncout))))) {
        vlSelfRef.test_gtext__DOT__unnamedblk4__DOT__match = 0U;
    }
    if (((~ (IData)(vlSelfRef.test_gtext__DOT__unnamedblk4__DOT__unnamedblk5__DOT__unnamedblk6__DOT__unnamedblk7__DOT__unnamedblk8__DOT__expected)) 
         & (0U != ([&]() {
                        __Vfunc_test_gtext__DOT__gfx_pixel__43__y = 2U;
                        __Vfunc_test_gtext__DOT__gfx_pixel__43__x = 3U;
                        __Vfunc_test_gtext__DOT__gfx_pixel__43__Vfuncout = 0;
                        __Vfunc_test_gtext__DOT__gfx_pixel__43__Vfuncout 
                            = ((0xf9ffU >= (0x0000ffffU 
                                            & (VL_MULS_III(32, (IData)(0x00000140U), __Vfunc_test_gtext__DOT__gfx_pixel__43__y) 
                                               + __Vfunc_test_gtext__DOT__gfx_pixel__43__x)))
                                ? vlSelfRef.test_gtext__DOT__dut__DOT__gfx_inst__DOT__gfx_mem__DOT__mem
                               [(0x0000ffffU & (VL_MULS_III(32, (IData)(0x00000140U), __Vfunc_test_gtext__DOT__gfx_pixel__43__y) 
                                                + __Vfunc_test_gtext__DOT__gfx_pixel__43__x))]
                                : 0U);
                    }(), (IData)(__Vfunc_test_gtext__DOT__gfx_pixel__43__Vfuncout))))) {
        vlSelfRef.test_gtext__DOT__unnamedblk4__DOT__match = 0U;
    }
    vlSelfRef.test_gtext__DOT__unnamedblk4__DOT__unnamedblk5__DOT__unnamedblk6__DOT__unnamedblk7__DOT__unnamedblk8__DOT__expected 
        = (1U & ((IData)(vlSelfRef.test_gtext__DOT__unnamedblk4__DOT__unnamedblk5__DOT__unnamedblk6__DOT__fb) 
                 >> 3U));
    if (((IData)(vlSelfRef.test_gtext__DOT__unnamedblk4__DOT__unnamedblk5__DOT__unnamedblk6__DOT__unnamedblk7__DOT__unnamedblk8__DOT__expected) 
         & (1U != ([&]() {
                        __Vfunc_test_gtext__DOT__gfx_pixel__42__y = 2U;
                        __Vfunc_test_gtext__DOT__gfx_pixel__42__x = 4U;
                        __Vfunc_test_gtext__DOT__gfx_pixel__42__Vfuncout = 0;
                        __Vfunc_test_gtext__DOT__gfx_pixel__42__Vfuncout 
                            = ((0xf9ffU >= (0x0000ffffU 
                                            & (VL_MULS_III(32, (IData)(0x00000140U), __Vfunc_test_gtext__DOT__gfx_pixel__42__y) 
                                               + __Vfunc_test_gtext__DOT__gfx_pixel__42__x)))
                                ? vlSelfRef.test_gtext__DOT__dut__DOT__gfx_inst__DOT__gfx_mem__DOT__mem
                               [(0x0000ffffU & (VL_MULS_III(32, (IData)(0x00000140U), __Vfunc_test_gtext__DOT__gfx_pixel__42__y) 
                                                + __Vfunc_test_gtext__DOT__gfx_pixel__42__x))]
                                : 0U);
                    }(), (IData)(__Vfunc_test_gtext__DOT__gfx_pixel__42__Vfuncout))))) {
        vlSelfRef.test_gtext__DOT__unnamedblk4__DOT__match = 0U;
    }
    if (((~ (IData)(vlSelfRef.test_gtext__DOT__unnamedblk4__DOT__unnamedblk5__DOT__unnamedblk6__DOT__unnamedblk7__DOT__unnamedblk8__DOT__expected)) 
         & (0U != ([&]() {
                        __Vfunc_test_gtext__DOT__gfx_pixel__43__y = 2U;
                        __Vfunc_test_gtext__DOT__gfx_pixel__43__x = 4U;
                        __Vfunc_test_gtext__DOT__gfx_pixel__43__Vfuncout = 0;
                        __Vfunc_test_gtext__DOT__gfx_pixel__43__Vfuncout 
                            = ((0xf9ffU >= (0x0000ffffU 
                                            & (VL_MULS_III(32, (IData)(0x00000140U), __Vfunc_test_gtext__DOT__gfx_pixel__43__y) 
                                               + __Vfunc_test_gtext__DOT__gfx_pixel__43__x)))
                                ? vlSelfRef.test_gtext__DOT__dut__DOT__gfx_inst__DOT__gfx_mem__DOT__mem
                               [(0x0000ffffU & (VL_MULS_III(32, (IData)(0x00000140U), __Vfunc_test_gtext__DOT__gfx_pixel__43__y) 
                                                + __Vfunc_test_gtext__DOT__gfx_pixel__43__x))]
                                : 0U);
                    }(), (IData)(__Vfunc_test_gtext__DOT__gfx_pixel__43__Vfuncout))))) {
        vlSelfRef.test_gtext__DOT__unnamedblk4__DOT__match = 0U;
    }
    vlSelfRef.test_gtext__DOT__unnamedblk4__DOT__unnamedblk5__DOT__unnamedblk6__DOT__unnamedblk7__DOT__unnamedblk8__DOT__expected 
        = (1U & ((IData)(vlSelfRef.test_gtext__DOT__unnamedblk4__DOT__unnamedblk5__DOT__unnamedblk6__DOT__fb) 
                 >> 2U));
    if (((IData)(vlSelfRef.test_gtext__DOT__unnamedblk4__DOT__unnamedblk5__DOT__unnamedblk6__DOT__unnamedblk7__DOT__unnamedblk8__DOT__expected) 
         & (1U != ([&]() {
                        __Vfunc_test_gtext__DOT__gfx_pixel__42__y = 2U;
                        __Vfunc_test_gtext__DOT__gfx_pixel__42__x = 5U;
                        __Vfunc_test_gtext__DOT__gfx_pixel__42__Vfuncout = 0;
                        __Vfunc_test_gtext__DOT__gfx_pixel__42__Vfuncout 
                            = ((0xf9ffU >= (0x0000ffffU 
                                            & (VL_MULS_III(32, (IData)(0x00000140U), __Vfunc_test_gtext__DOT__gfx_pixel__42__y) 
                                               + __Vfunc_test_gtext__DOT__gfx_pixel__42__x)))
                                ? vlSelfRef.test_gtext__DOT__dut__DOT__gfx_inst__DOT__gfx_mem__DOT__mem
                               [(0x0000ffffU & (VL_MULS_III(32, (IData)(0x00000140U), __Vfunc_test_gtext__DOT__gfx_pixel__42__y) 
                                                + __Vfunc_test_gtext__DOT__gfx_pixel__42__x))]
                                : 0U);
                    }(), (IData)(__Vfunc_test_gtext__DOT__gfx_pixel__42__Vfuncout))))) {
        vlSelfRef.test_gtext__DOT__unnamedblk4__DOT__match = 0U;
    }
    if (((~ (IData)(vlSelfRef.test_gtext__DOT__unnamedblk4__DOT__unnamedblk5__DOT__unnamedblk6__DOT__unnamedblk7__DOT__unnamedblk8__DOT__expected)) 
         & (0U != ([&]() {
                        __Vfunc_test_gtext__DOT__gfx_pixel__43__y = 2U;
                        __Vfunc_test_gtext__DOT__gfx_pixel__43__x = 5U;
                        __Vfunc_test_gtext__DOT__gfx_pixel__43__Vfuncout = 0;
                        __Vfunc_test_gtext__DOT__gfx_pixel__43__Vfuncout 
                            = ((0xf9ffU >= (0x0000ffffU 
                                            & (VL_MULS_III(32, (IData)(0x00000140U), __Vfunc_test_gtext__DOT__gfx_pixel__43__y) 
                                               + __Vfunc_test_gtext__DOT__gfx_pixel__43__x)))
                                ? vlSelfRef.test_gtext__DOT__dut__DOT__gfx_inst__DOT__gfx_mem__DOT__mem
                               [(0x0000ffffU & (VL_MULS_III(32, (IData)(0x00000140U), __Vfunc_test_gtext__DOT__gfx_pixel__43__y) 
                                                + __Vfunc_test_gtext__DOT__gfx_pixel__43__x))]
                                : 0U);
                    }(), (IData)(__Vfunc_test_gtext__DOT__gfx_pixel__43__Vfuncout))))) {
        vlSelfRef.test_gtext__DOT__unnamedblk4__DOT__match = 0U;
    }
    vlSelfRef.test_gtext__DOT__unnamedblk4__DOT__unnamedblk5__DOT__unnamedblk6__DOT__unnamedblk7__DOT__unnamedblk8__DOT__expected 
        = (1U & ((IData)(vlSelfRef.test_gtext__DOT__unnamedblk4__DOT__unnamedblk5__DOT__unnamedblk6__DOT__fb) 
                 >> 1U));
    if (((IData)(vlSelfRef.test_gtext__DOT__unnamedblk4__DOT__unnamedblk5__DOT__unnamedblk6__DOT__unnamedblk7__DOT__unnamedblk8__DOT__expected) 
         & (1U != ([&]() {
                        __Vfunc_test_gtext__DOT__gfx_pixel__42__y = 2U;
                        __Vfunc_test_gtext__DOT__gfx_pixel__42__x = 6U;
                        __Vfunc_test_gtext__DOT__gfx_pixel__42__Vfuncout = 0;
                        __Vfunc_test_gtext__DOT__gfx_pixel__42__Vfuncout 
                            = ((0xf9ffU >= (0x0000ffffU 
                                            & (VL_MULS_III(32, (IData)(0x00000140U), __Vfunc_test_gtext__DOT__gfx_pixel__42__y) 
                                               + __Vfunc_test_gtext__DOT__gfx_pixel__42__x)))
                                ? vlSelfRef.test_gtext__DOT__dut__DOT__gfx_inst__DOT__gfx_mem__DOT__mem
                               [(0x0000ffffU & (VL_MULS_III(32, (IData)(0x00000140U), __Vfunc_test_gtext__DOT__gfx_pixel__42__y) 
                                                + __Vfunc_test_gtext__DOT__gfx_pixel__42__x))]
                                : 0U);
                    }(), (IData)(__Vfunc_test_gtext__DOT__gfx_pixel__42__Vfuncout))))) {
        vlSelfRef.test_gtext__DOT__unnamedblk4__DOT__match = 0U;
    }
    if (((~ (IData)(vlSelfRef.test_gtext__DOT__unnamedblk4__DOT__unnamedblk5__DOT__unnamedblk6__DOT__unnamedblk7__DOT__unnamedblk8__DOT__expected)) 
         & (0U != ([&]() {
                        __Vfunc_test_gtext__DOT__gfx_pixel__43__y = 2U;
                        __Vfunc_test_gtext__DOT__gfx_pixel__43__x = 6U;
                        __Vfunc_test_gtext__DOT__gfx_pixel__43__Vfuncout = 0;
                        __Vfunc_test_gtext__DOT__gfx_pixel__43__Vfuncout 
                            = ((0xf9ffU >= (0x0000ffffU 
                                            & (VL_MULS_III(32, (IData)(0x00000140U), __Vfunc_test_gtext__DOT__gfx_pixel__43__y) 
                                               + __Vfunc_test_gtext__DOT__gfx_pixel__43__x)))
                                ? vlSelfRef.test_gtext__DOT__dut__DOT__gfx_inst__DOT__gfx_mem__DOT__mem
                               [(0x0000ffffU & (VL_MULS_III(32, (IData)(0x00000140U), __Vfunc_test_gtext__DOT__gfx_pixel__43__y) 
                                                + __Vfunc_test_gtext__DOT__gfx_pixel__43__x))]
                                : 0U);
                    }(), (IData)(__Vfunc_test_gtext__DOT__gfx_pixel__43__Vfuncout))))) {
        vlSelfRef.test_gtext__DOT__unnamedblk4__DOT__match = 0U;
    }
    vlSelfRef.test_gtext__DOT__unnamedblk4__DOT__unnamedblk5__DOT__unnamedblk6__DOT__unnamedblk7__DOT__unnamedblk8__DOT__expected 
        = (1U & (IData)(vlSelfRef.test_gtext__DOT__unnamedblk4__DOT__unnamedblk5__DOT__unnamedblk6__DOT__fb));
    if (((IData)(vlSelfRef.test_gtext__DOT__unnamedblk4__DOT__unnamedblk5__DOT__unnamedblk6__DOT__unnamedblk7__DOT__unnamedblk8__DOT__expected) 
         & (1U != ([&]() {
                        __Vfunc_test_gtext__DOT__gfx_pixel__42__y = 2U;
                        __Vfunc_test_gtext__DOT__gfx_pixel__42__x = 7U;
                        __Vfunc_test_gtext__DOT__gfx_pixel__42__Vfuncout = 0;
                        __Vfunc_test_gtext__DOT__gfx_pixel__42__Vfuncout 
                            = ((0xf9ffU >= (0x0000ffffU 
                                            & (VL_MULS_III(32, (IData)(0x00000140U), __Vfunc_test_gtext__DOT__gfx_pixel__42__y) 
                                               + __Vfunc_test_gtext__DOT__gfx_pixel__42__x)))
                                ? vlSelfRef.test_gtext__DOT__dut__DOT__gfx_inst__DOT__gfx_mem__DOT__mem
                               [(0x0000ffffU & (VL_MULS_III(32, (IData)(0x00000140U), __Vfunc_test_gtext__DOT__gfx_pixel__42__y) 
                                                + __Vfunc_test_gtext__DOT__gfx_pixel__42__x))]
                                : 0U);
                    }(), (IData)(__Vfunc_test_gtext__DOT__gfx_pixel__42__Vfuncout))))) {
        vlSelfRef.test_gtext__DOT__unnamedblk4__DOT__match = 0U;
    }
    if (((~ (IData)(vlSelfRef.test_gtext__DOT__unnamedblk4__DOT__unnamedblk5__DOT__unnamedblk6__DOT__unnamedblk7__DOT__unnamedblk8__DOT__expected)) 
         & (0U != ([&]() {
                        __Vfunc_test_gtext__DOT__gfx_pixel__43__y = 2U;
                        __Vfunc_test_gtext__DOT__gfx_pixel__43__x = 7U;
                        __Vfunc_test_gtext__DOT__gfx_pixel__43__Vfuncout = 0;
                        __Vfunc_test_gtext__DOT__gfx_pixel__43__Vfuncout 
                            = ((0xf9ffU >= (0x0000ffffU 
                                            & (VL_MULS_III(32, (IData)(0x00000140U), __Vfunc_test_gtext__DOT__gfx_pixel__43__y) 
                                               + __Vfunc_test_gtext__DOT__gfx_pixel__43__x)))
                                ? vlSelfRef.test_gtext__DOT__dut__DOT__gfx_inst__DOT__gfx_mem__DOT__mem
                               [(0x0000ffffU & (VL_MULS_III(32, (IData)(0x00000140U), __Vfunc_test_gtext__DOT__gfx_pixel__43__y) 
                                                + __Vfunc_test_gtext__DOT__gfx_pixel__43__x))]
                                : 0U);
                    }(), (IData)(__Vfunc_test_gtext__DOT__gfx_pixel__43__Vfuncout))))) {
        vlSelfRef.test_gtext__DOT__unnamedblk4__DOT__match = 0U;
    }
    vlSelfRef.test_gtext__DOT__unnamedblk4__DOT__unnamedblk5__DOT__unnamedblk6__DOT__fb 
        = vlSelfRef.test_gtext__DOT__dut__DOT__text_inst__DOT__font_mem__DOT__mem[523U];
    vlSelfRef.test_gtext__DOT__unnamedblk4__DOT__unnamedblk5__DOT__unnamedblk6__DOT__unnamedblk7__DOT__unnamedblk8__DOT__expected 
        = (1U & ((IData)(vlSelfRef.test_gtext__DOT__unnamedblk4__DOT__unnamedblk5__DOT__unnamedblk6__DOT__fb) 
                 >> 7U));
    if (((IData)(vlSelfRef.test_gtext__DOT__unnamedblk4__DOT__unnamedblk5__DOT__unnamedblk6__DOT__unnamedblk7__DOT__unnamedblk8__DOT__expected) 
         & (1U != ([&]() {
                        __Vfunc_test_gtext__DOT__gfx_pixel__42__y = 3U;
                        __Vfunc_test_gtext__DOT__gfx_pixel__42__x = 0U;
                        __Vfunc_test_gtext__DOT__gfx_pixel__42__Vfuncout = 0;
                        __Vfunc_test_gtext__DOT__gfx_pixel__42__Vfuncout 
                            = ((0xf9ffU >= (0x0000ffffU 
                                            & (VL_MULS_III(32, (IData)(0x00000140U), __Vfunc_test_gtext__DOT__gfx_pixel__42__y) 
                                               + __Vfunc_test_gtext__DOT__gfx_pixel__42__x)))
                                ? vlSelfRef.test_gtext__DOT__dut__DOT__gfx_inst__DOT__gfx_mem__DOT__mem
                               [(0x0000ffffU & (VL_MULS_III(32, (IData)(0x00000140U), __Vfunc_test_gtext__DOT__gfx_pixel__42__y) 
                                                + __Vfunc_test_gtext__DOT__gfx_pixel__42__x))]
                                : 0U);
                    }(), (IData)(__Vfunc_test_gtext__DOT__gfx_pixel__42__Vfuncout))))) {
        vlSelfRef.test_gtext__DOT__unnamedblk4__DOT__match = 0U;
    }
    if (((~ (IData)(vlSelfRef.test_gtext__DOT__unnamedblk4__DOT__unnamedblk5__DOT__unnamedblk6__DOT__unnamedblk7__DOT__unnamedblk8__DOT__expected)) 
         & (0U != ([&]() {
                        __Vfunc_test_gtext__DOT__gfx_pixel__43__y = 3U;
                        __Vfunc_test_gtext__DOT__gfx_pixel__43__x = 0U;
                        __Vfunc_test_gtext__DOT__gfx_pixel__43__Vfuncout = 0;
                        __Vfunc_test_gtext__DOT__gfx_pixel__43__Vfuncout 
                            = ((0xf9ffU >= (0x0000ffffU 
                                            & (VL_MULS_III(32, (IData)(0x00000140U), __Vfunc_test_gtext__DOT__gfx_pixel__43__y) 
                                               + __Vfunc_test_gtext__DOT__gfx_pixel__43__x)))
                                ? vlSelfRef.test_gtext__DOT__dut__DOT__gfx_inst__DOT__gfx_mem__DOT__mem
                               [(0x0000ffffU & (VL_MULS_III(32, (IData)(0x00000140U), __Vfunc_test_gtext__DOT__gfx_pixel__43__y) 
                                                + __Vfunc_test_gtext__DOT__gfx_pixel__43__x))]
                                : 0U);
                    }(), (IData)(__Vfunc_test_gtext__DOT__gfx_pixel__43__Vfuncout))))) {
        vlSelfRef.test_gtext__DOT__unnamedblk4__DOT__match = 0U;
    }
    vlSelfRef.test_gtext__DOT__unnamedblk4__DOT__unnamedblk5__DOT__unnamedblk6__DOT__unnamedblk7__DOT__unnamedblk8__DOT__expected 
        = (1U & ((IData)(vlSelfRef.test_gtext__DOT__unnamedblk4__DOT__unnamedblk5__DOT__unnamedblk6__DOT__fb) 
                 >> 6U));
    if (((IData)(vlSelfRef.test_gtext__DOT__unnamedblk4__DOT__unnamedblk5__DOT__unnamedblk6__DOT__unnamedblk7__DOT__unnamedblk8__DOT__expected) 
         & (1U != ([&]() {
                        __Vfunc_test_gtext__DOT__gfx_pixel__42__y = 3U;
                        __Vfunc_test_gtext__DOT__gfx_pixel__42__x = 1U;
                        __Vfunc_test_gtext__DOT__gfx_pixel__42__Vfuncout = 0;
                        __Vfunc_test_gtext__DOT__gfx_pixel__42__Vfuncout 
                            = ((0xf9ffU >= (0x0000ffffU 
                                            & (VL_MULS_III(32, (IData)(0x00000140U), __Vfunc_test_gtext__DOT__gfx_pixel__42__y) 
                                               + __Vfunc_test_gtext__DOT__gfx_pixel__42__x)))
                                ? vlSelfRef.test_gtext__DOT__dut__DOT__gfx_inst__DOT__gfx_mem__DOT__mem
                               [(0x0000ffffU & (VL_MULS_III(32, (IData)(0x00000140U), __Vfunc_test_gtext__DOT__gfx_pixel__42__y) 
                                                + __Vfunc_test_gtext__DOT__gfx_pixel__42__x))]
                                : 0U);
                    }(), (IData)(__Vfunc_test_gtext__DOT__gfx_pixel__42__Vfuncout))))) {
        vlSelfRef.test_gtext__DOT__unnamedblk4__DOT__match = 0U;
    }
    if (((~ (IData)(vlSelfRef.test_gtext__DOT__unnamedblk4__DOT__unnamedblk5__DOT__unnamedblk6__DOT__unnamedblk7__DOT__unnamedblk8__DOT__expected)) 
         & (0U != ([&]() {
                        __Vfunc_test_gtext__DOT__gfx_pixel__43__y = 3U;
                        __Vfunc_test_gtext__DOT__gfx_pixel__43__x = 1U;
                        __Vfunc_test_gtext__DOT__gfx_pixel__43__Vfuncout = 0;
                        __Vfunc_test_gtext__DOT__gfx_pixel__43__Vfuncout 
                            = ((0xf9ffU >= (0x0000ffffU 
                                            & (VL_MULS_III(32, (IData)(0x00000140U), __Vfunc_test_gtext__DOT__gfx_pixel__43__y) 
                                               + __Vfunc_test_gtext__DOT__gfx_pixel__43__x)))
                                ? vlSelfRef.test_gtext__DOT__dut__DOT__gfx_inst__DOT__gfx_mem__DOT__mem
                               [(0x0000ffffU & (VL_MULS_III(32, (IData)(0x00000140U), __Vfunc_test_gtext__DOT__gfx_pixel__43__y) 
                                                + __Vfunc_test_gtext__DOT__gfx_pixel__43__x))]
                                : 0U);
                    }(), (IData)(__Vfunc_test_gtext__DOT__gfx_pixel__43__Vfuncout))))) {
        vlSelfRef.test_gtext__DOT__unnamedblk4__DOT__match = 0U;
    }
    vlSelfRef.test_gtext__DOT__unnamedblk4__DOT__unnamedblk5__DOT__unnamedblk6__DOT__unnamedblk7__DOT__unnamedblk8__DOT__expected 
        = (1U & ((IData)(vlSelfRef.test_gtext__DOT__unnamedblk4__DOT__unnamedblk5__DOT__unnamedblk6__DOT__fb) 
                 >> 5U));
    if (((IData)(vlSelfRef.test_gtext__DOT__unnamedblk4__DOT__unnamedblk5__DOT__unnamedblk6__DOT__unnamedblk7__DOT__unnamedblk8__DOT__expected) 
         & (1U != ([&]() {
                        __Vfunc_test_gtext__DOT__gfx_pixel__42__y = 3U;
                        __Vfunc_test_gtext__DOT__gfx_pixel__42__x = 2U;
                        __Vfunc_test_gtext__DOT__gfx_pixel__42__Vfuncout = 0;
                        __Vfunc_test_gtext__DOT__gfx_pixel__42__Vfuncout 
                            = ((0xf9ffU >= (0x0000ffffU 
                                            & (VL_MULS_III(32, (IData)(0x00000140U), __Vfunc_test_gtext__DOT__gfx_pixel__42__y) 
                                               + __Vfunc_test_gtext__DOT__gfx_pixel__42__x)))
                                ? vlSelfRef.test_gtext__DOT__dut__DOT__gfx_inst__DOT__gfx_mem__DOT__mem
                               [(0x0000ffffU & (VL_MULS_III(32, (IData)(0x00000140U), __Vfunc_test_gtext__DOT__gfx_pixel__42__y) 
                                                + __Vfunc_test_gtext__DOT__gfx_pixel__42__x))]
                                : 0U);
                    }(), (IData)(__Vfunc_test_gtext__DOT__gfx_pixel__42__Vfuncout))))) {
        vlSelfRef.test_gtext__DOT__unnamedblk4__DOT__match = 0U;
    }
    if (((~ (IData)(vlSelfRef.test_gtext__DOT__unnamedblk4__DOT__unnamedblk5__DOT__unnamedblk6__DOT__unnamedblk7__DOT__unnamedblk8__DOT__expected)) 
         & (0U != ([&]() {
                        __Vfunc_test_gtext__DOT__gfx_pixel__43__y = 3U;
                        __Vfunc_test_gtext__DOT__gfx_pixel__43__x = 2U;
                        __Vfunc_test_gtext__DOT__gfx_pixel__43__Vfuncout = 0;
                        __Vfunc_test_gtext__DOT__gfx_pixel__43__Vfuncout 
                            = ((0xf9ffU >= (0x0000ffffU 
                                            & (VL_MULS_III(32, (IData)(0x00000140U), __Vfunc_test_gtext__DOT__gfx_pixel__43__y) 
                                               + __Vfunc_test_gtext__DOT__gfx_pixel__43__x)))
                                ? vlSelfRef.test_gtext__DOT__dut__DOT__gfx_inst__DOT__gfx_mem__DOT__mem
                               [(0x0000ffffU & (VL_MULS_III(32, (IData)(0x00000140U), __Vfunc_test_gtext__DOT__gfx_pixel__43__y) 
                                                + __Vfunc_test_gtext__DOT__gfx_pixel__43__x))]
                                : 0U);
                    }(), (IData)(__Vfunc_test_gtext__DOT__gfx_pixel__43__Vfuncout))))) {
        vlSelfRef.test_gtext__DOT__unnamedblk4__DOT__match = 0U;
    }
    vlSelfRef.test_gtext__DOT__unnamedblk4__DOT__unnamedblk5__DOT__unnamedblk6__DOT__unnamedblk7__DOT__unnamedblk8__DOT__expected 
        = (1U & ((IData)(vlSelfRef.test_gtext__DOT__unnamedblk4__DOT__unnamedblk5__DOT__unnamedblk6__DOT__fb) 
                 >> 4U));
    if (((IData)(vlSelfRef.test_gtext__DOT__unnamedblk4__DOT__unnamedblk5__DOT__unnamedblk6__DOT__unnamedblk7__DOT__unnamedblk8__DOT__expected) 
         & (1U != ([&]() {
                        __Vfunc_test_gtext__DOT__gfx_pixel__42__y = 3U;
                        __Vfunc_test_gtext__DOT__gfx_pixel__42__x = 3U;
                        __Vfunc_test_gtext__DOT__gfx_pixel__42__Vfuncout = 0;
                        __Vfunc_test_gtext__DOT__gfx_pixel__42__Vfuncout 
                            = ((0xf9ffU >= (0x0000ffffU 
                                            & (VL_MULS_III(32, (IData)(0x00000140U), __Vfunc_test_gtext__DOT__gfx_pixel__42__y) 
                                               + __Vfunc_test_gtext__DOT__gfx_pixel__42__x)))
                                ? vlSelfRef.test_gtext__DOT__dut__DOT__gfx_inst__DOT__gfx_mem__DOT__mem
                               [(0x0000ffffU & (VL_MULS_III(32, (IData)(0x00000140U), __Vfunc_test_gtext__DOT__gfx_pixel__42__y) 
                                                + __Vfunc_test_gtext__DOT__gfx_pixel__42__x))]
                                : 0U);
                    }(), (IData)(__Vfunc_test_gtext__DOT__gfx_pixel__42__Vfuncout))))) {
        vlSelfRef.test_gtext__DOT__unnamedblk4__DOT__match = 0U;
    }
    if (((~ (IData)(vlSelfRef.test_gtext__DOT__unnamedblk4__DOT__unnamedblk5__DOT__unnamedblk6__DOT__unnamedblk7__DOT__unnamedblk8__DOT__expected)) 
         & (0U != ([&]() {
                        __Vfunc_test_gtext__DOT__gfx_pixel__43__y = 3U;
                        __Vfunc_test_gtext__DOT__gfx_pixel__43__x = 3U;
                        __Vfunc_test_gtext__DOT__gfx_pixel__43__Vfuncout = 0;
                        __Vfunc_test_gtext__DOT__gfx_pixel__43__Vfuncout 
                            = ((0xf9ffU >= (0x0000ffffU 
                                            & (VL_MULS_III(32, (IData)(0x00000140U), __Vfunc_test_gtext__DOT__gfx_pixel__43__y) 
                                               + __Vfunc_test_gtext__DOT__gfx_pixel__43__x)))
                                ? vlSelfRef.test_gtext__DOT__dut__DOT__gfx_inst__DOT__gfx_mem__DOT__mem
                               [(0x0000ffffU & (VL_MULS_III(32, (IData)(0x00000140U), __Vfunc_test_gtext__DOT__gfx_pixel__43__y) 
                                                + __Vfunc_test_gtext__DOT__gfx_pixel__43__x))]
                                : 0U);
                    }(), (IData)(__Vfunc_test_gtext__DOT__gfx_pixel__43__Vfuncout))))) {
        vlSelfRef.test_gtext__DOT__unnamedblk4__DOT__match = 0U;
    }
    vlSelfRef.test_gtext__DOT__unnamedblk4__DOT__unnamedblk5__DOT__unnamedblk6__DOT__unnamedblk7__DOT__unnamedblk8__DOT__expected 
        = (1U & ((IData)(vlSelfRef.test_gtext__DOT__unnamedblk4__DOT__unnamedblk5__DOT__unnamedblk6__DOT__fb) 
                 >> 3U));
    if (((IData)(vlSelfRef.test_gtext__DOT__unnamedblk4__DOT__unnamedblk5__DOT__unnamedblk6__DOT__unnamedblk7__DOT__unnamedblk8__DOT__expected) 
         & (1U != ([&]() {
                        __Vfunc_test_gtext__DOT__gfx_pixel__42__y = 3U;
                        __Vfunc_test_gtext__DOT__gfx_pixel__42__x = 4U;
                        __Vfunc_test_gtext__DOT__gfx_pixel__42__Vfuncout = 0;
                        __Vfunc_test_gtext__DOT__gfx_pixel__42__Vfuncout 
                            = ((0xf9ffU >= (0x0000ffffU 
                                            & (VL_MULS_III(32, (IData)(0x00000140U), __Vfunc_test_gtext__DOT__gfx_pixel__42__y) 
                                               + __Vfunc_test_gtext__DOT__gfx_pixel__42__x)))
                                ? vlSelfRef.test_gtext__DOT__dut__DOT__gfx_inst__DOT__gfx_mem__DOT__mem
                               [(0x0000ffffU & (VL_MULS_III(32, (IData)(0x00000140U), __Vfunc_test_gtext__DOT__gfx_pixel__42__y) 
                                                + __Vfunc_test_gtext__DOT__gfx_pixel__42__x))]
                                : 0U);
                    }(), (IData)(__Vfunc_test_gtext__DOT__gfx_pixel__42__Vfuncout))))) {
        vlSelfRef.test_gtext__DOT__unnamedblk4__DOT__match = 0U;
    }
    if (((~ (IData)(vlSelfRef.test_gtext__DOT__unnamedblk4__DOT__unnamedblk5__DOT__unnamedblk6__DOT__unnamedblk7__DOT__unnamedblk8__DOT__expected)) 
         & (0U != ([&]() {
                        __Vfunc_test_gtext__DOT__gfx_pixel__43__y = 3U;
                        __Vfunc_test_gtext__DOT__gfx_pixel__43__x = 4U;
                        __Vfunc_test_gtext__DOT__gfx_pixel__43__Vfuncout = 0;
                        __Vfunc_test_gtext__DOT__gfx_pixel__43__Vfuncout 
                            = ((0xf9ffU >= (0x0000ffffU 
                                            & (VL_MULS_III(32, (IData)(0x00000140U), __Vfunc_test_gtext__DOT__gfx_pixel__43__y) 
                                               + __Vfunc_test_gtext__DOT__gfx_pixel__43__x)))
                                ? vlSelfRef.test_gtext__DOT__dut__DOT__gfx_inst__DOT__gfx_mem__DOT__mem
                               [(0x0000ffffU & (VL_MULS_III(32, (IData)(0x00000140U), __Vfunc_test_gtext__DOT__gfx_pixel__43__y) 
                                                + __Vfunc_test_gtext__DOT__gfx_pixel__43__x))]
                                : 0U);
                    }(), (IData)(__Vfunc_test_gtext__DOT__gfx_pixel__43__Vfuncout))))) {
        vlSelfRef.test_gtext__DOT__unnamedblk4__DOT__match = 0U;
    }
    vlSelfRef.test_gtext__DOT__unnamedblk4__DOT__unnamedblk5__DOT__unnamedblk6__DOT__unnamedblk7__DOT__unnamedblk8__DOT__expected 
        = (1U & ((IData)(vlSelfRef.test_gtext__DOT__unnamedblk4__DOT__unnamedblk5__DOT__unnamedblk6__DOT__fb) 
                 >> 2U));
    if (((IData)(vlSelfRef.test_gtext__DOT__unnamedblk4__DOT__unnamedblk5__DOT__unnamedblk6__DOT__unnamedblk7__DOT__unnamedblk8__DOT__expected) 
         & (1U != ([&]() {
                        __Vfunc_test_gtext__DOT__gfx_pixel__42__y = 3U;
                        __Vfunc_test_gtext__DOT__gfx_pixel__42__x = 5U;
                        __Vfunc_test_gtext__DOT__gfx_pixel__42__Vfuncout = 0;
                        __Vfunc_test_gtext__DOT__gfx_pixel__42__Vfuncout 
                            = ((0xf9ffU >= (0x0000ffffU 
                                            & (VL_MULS_III(32, (IData)(0x00000140U), __Vfunc_test_gtext__DOT__gfx_pixel__42__y) 
                                               + __Vfunc_test_gtext__DOT__gfx_pixel__42__x)))
                                ? vlSelfRef.test_gtext__DOT__dut__DOT__gfx_inst__DOT__gfx_mem__DOT__mem
                               [(0x0000ffffU & (VL_MULS_III(32, (IData)(0x00000140U), __Vfunc_test_gtext__DOT__gfx_pixel__42__y) 
                                                + __Vfunc_test_gtext__DOT__gfx_pixel__42__x))]
                                : 0U);
                    }(), (IData)(__Vfunc_test_gtext__DOT__gfx_pixel__42__Vfuncout))))) {
        vlSelfRef.test_gtext__DOT__unnamedblk4__DOT__match = 0U;
    }
    if (((~ (IData)(vlSelfRef.test_gtext__DOT__unnamedblk4__DOT__unnamedblk5__DOT__unnamedblk6__DOT__unnamedblk7__DOT__unnamedblk8__DOT__expected)) 
         & (0U != ([&]() {
                        __Vfunc_test_gtext__DOT__gfx_pixel__43__y = 3U;
                        __Vfunc_test_gtext__DOT__gfx_pixel__43__x = 5U;
                        __Vfunc_test_gtext__DOT__gfx_pixel__43__Vfuncout = 0;
                        __Vfunc_test_gtext__DOT__gfx_pixel__43__Vfuncout 
                            = ((0xf9ffU >= (0x0000ffffU 
                                            & (VL_MULS_III(32, (IData)(0x00000140U), __Vfunc_test_gtext__DOT__gfx_pixel__43__y) 
                                               + __Vfunc_test_gtext__DOT__gfx_pixel__43__x)))
                                ? vlSelfRef.test_gtext__DOT__dut__DOT__gfx_inst__DOT__gfx_mem__DOT__mem
                               [(0x0000ffffU & (VL_MULS_III(32, (IData)(0x00000140U), __Vfunc_test_gtext__DOT__gfx_pixel__43__y) 
                                                + __Vfunc_test_gtext__DOT__gfx_pixel__43__x))]
                                : 0U);
                    }(), (IData)(__Vfunc_test_gtext__DOT__gfx_pixel__43__Vfuncout))))) {
        vlSelfRef.test_gtext__DOT__unnamedblk4__DOT__match = 0U;
    }
    vlSelfRef.test_gtext__DOT__unnamedblk4__DOT__unnamedblk5__DOT__unnamedblk6__DOT__unnamedblk7__DOT__unnamedblk8__DOT__expected 
        = (1U & ((IData)(vlSelfRef.test_gtext__DOT__unnamedblk4__DOT__unnamedblk5__DOT__unnamedblk6__DOT__fb) 
                 >> 1U));
    if (((IData)(vlSelfRef.test_gtext__DOT__unnamedblk4__DOT__unnamedblk5__DOT__unnamedblk6__DOT__unnamedblk7__DOT__unnamedblk8__DOT__expected) 
         & (1U != ([&]() {
                        __Vfunc_test_gtext__DOT__gfx_pixel__42__y = 3U;
                        __Vfunc_test_gtext__DOT__gfx_pixel__42__x = 6U;
                        __Vfunc_test_gtext__DOT__gfx_pixel__42__Vfuncout = 0;
                        __Vfunc_test_gtext__DOT__gfx_pixel__42__Vfuncout 
                            = ((0xf9ffU >= (0x0000ffffU 
                                            & (VL_MULS_III(32, (IData)(0x00000140U), __Vfunc_test_gtext__DOT__gfx_pixel__42__y) 
                                               + __Vfunc_test_gtext__DOT__gfx_pixel__42__x)))
                                ? vlSelfRef.test_gtext__DOT__dut__DOT__gfx_inst__DOT__gfx_mem__DOT__mem
                               [(0x0000ffffU & (VL_MULS_III(32, (IData)(0x00000140U), __Vfunc_test_gtext__DOT__gfx_pixel__42__y) 
                                                + __Vfunc_test_gtext__DOT__gfx_pixel__42__x))]
                                : 0U);
                    }(), (IData)(__Vfunc_test_gtext__DOT__gfx_pixel__42__Vfuncout))))) {
        vlSelfRef.test_gtext__DOT__unnamedblk4__DOT__match = 0U;
    }
    if (((~ (IData)(vlSelfRef.test_gtext__DOT__unnamedblk4__DOT__unnamedblk5__DOT__unnamedblk6__DOT__unnamedblk7__DOT__unnamedblk8__DOT__expected)) 
         & (0U != ([&]() {
                        __Vfunc_test_gtext__DOT__gfx_pixel__43__y = 3U;
                        __Vfunc_test_gtext__DOT__gfx_pixel__43__x = 6U;
                        __Vfunc_test_gtext__DOT__gfx_pixel__43__Vfuncout = 0;
                        __Vfunc_test_gtext__DOT__gfx_pixel__43__Vfuncout 
                            = ((0xf9ffU >= (0x0000ffffU 
                                            & (VL_MULS_III(32, (IData)(0x00000140U), __Vfunc_test_gtext__DOT__gfx_pixel__43__y) 
                                               + __Vfunc_test_gtext__DOT__gfx_pixel__43__x)))
                                ? vlSelfRef.test_gtext__DOT__dut__DOT__gfx_inst__DOT__gfx_mem__DOT__mem
                               [(0x0000ffffU & (VL_MULS_III(32, (IData)(0x00000140U), __Vfunc_test_gtext__DOT__gfx_pixel__43__y) 
                                                + __Vfunc_test_gtext__DOT__gfx_pixel__43__x))]
                                : 0U);
                    }(), (IData)(__Vfunc_test_gtext__DOT__gfx_pixel__43__Vfuncout))))) {
        vlSelfRef.test_gtext__DOT__unnamedblk4__DOT__match = 0U;
    }
    vlSelfRef.test_gtext__DOT__unnamedblk4__DOT__unnamedblk5__DOT__unnamedblk6__DOT__unnamedblk7__DOT__unnamedblk8__DOT__expected 
        = (1U & (IData)(vlSelfRef.test_gtext__DOT__unnamedblk4__DOT__unnamedblk5__DOT__unnamedblk6__DOT__fb));
    if (((IData)(vlSelfRef.test_gtext__DOT__unnamedblk4__DOT__unnamedblk5__DOT__unnamedblk6__DOT__unnamedblk7__DOT__unnamedblk8__DOT__expected) 
         & (1U != ([&]() {
                        __Vfunc_test_gtext__DOT__gfx_pixel__42__y = 3U;
                        __Vfunc_test_gtext__DOT__gfx_pixel__42__x = 7U;
                        __Vfunc_test_gtext__DOT__gfx_pixel__42__Vfuncout = 0;
                        __Vfunc_test_gtext__DOT__gfx_pixel__42__Vfuncout 
                            = ((0xf9ffU >= (0x0000ffffU 
                                            & (VL_MULS_III(32, (IData)(0x00000140U), __Vfunc_test_gtext__DOT__gfx_pixel__42__y) 
                                               + __Vfunc_test_gtext__DOT__gfx_pixel__42__x)))
                                ? vlSelfRef.test_gtext__DOT__dut__DOT__gfx_inst__DOT__gfx_mem__DOT__mem
                               [(0x0000ffffU & (VL_MULS_III(32, (IData)(0x00000140U), __Vfunc_test_gtext__DOT__gfx_pixel__42__y) 
                                                + __Vfunc_test_gtext__DOT__gfx_pixel__42__x))]
                                : 0U);
                    }(), (IData)(__Vfunc_test_gtext__DOT__gfx_pixel__42__Vfuncout))))) {
        vlSelfRef.test_gtext__DOT__unnamedblk4__DOT__match = 0U;
    }
    if (((~ (IData)(vlSelfRef.test_gtext__DOT__unnamedblk4__DOT__unnamedblk5__DOT__unnamedblk6__DOT__unnamedblk7__DOT__unnamedblk8__DOT__expected)) 
         & (0U != ([&]() {
                        __Vfunc_test_gtext__DOT__gfx_pixel__43__y = 3U;
                        __Vfunc_test_gtext__DOT__gfx_pixel__43__x = 7U;
                        __Vfunc_test_gtext__DOT__gfx_pixel__43__Vfuncout = 0;
                        __Vfunc_test_gtext__DOT__gfx_pixel__43__Vfuncout 
                            = ((0xf9ffU >= (0x0000ffffU 
                                            & (VL_MULS_III(32, (IData)(0x00000140U), __Vfunc_test_gtext__DOT__gfx_pixel__43__y) 
                                               + __Vfunc_test_gtext__DOT__gfx_pixel__43__x)))
                                ? vlSelfRef.test_gtext__DOT__dut__DOT__gfx_inst__DOT__gfx_mem__DOT__mem
                               [(0x0000ffffU & (VL_MULS_III(32, (IData)(0x00000140U), __Vfunc_test_gtext__DOT__gfx_pixel__43__y) 
                                                + __Vfunc_test_gtext__DOT__gfx_pixel__43__x))]
                                : 0U);
                    }(), (IData)(__Vfunc_test_gtext__DOT__gfx_pixel__43__Vfuncout))))) {
        vlSelfRef.test_gtext__DOT__unnamedblk4__DOT__match = 0U;
    }
    vlSelfRef.test_gtext__DOT__unnamedblk4__DOT__unnamedblk5__DOT__unnamedblk6__DOT__fb 
        = vlSelfRef.test_gtext__DOT__dut__DOT__text_inst__DOT__font_mem__DOT__mem[524U];
    vlSelfRef.test_gtext__DOT__unnamedblk4__DOT__unnamedblk5__DOT__unnamedblk6__DOT__unnamedblk7__DOT__unnamedblk8__DOT__expected 
        = (1U & ((IData)(vlSelfRef.test_gtext__DOT__unnamedblk4__DOT__unnamedblk5__DOT__unnamedblk6__DOT__fb) 
                 >> 7U));
    if (((IData)(vlSelfRef.test_gtext__DOT__unnamedblk4__DOT__unnamedblk5__DOT__unnamedblk6__DOT__unnamedblk7__DOT__unnamedblk8__DOT__expected) 
         & (1U != ([&]() {
                        __Vfunc_test_gtext__DOT__gfx_pixel__42__y = 4U;
                        __Vfunc_test_gtext__DOT__gfx_pixel__42__x = 0U;
                        __Vfunc_test_gtext__DOT__gfx_pixel__42__Vfuncout = 0;
                        __Vfunc_test_gtext__DOT__gfx_pixel__42__Vfuncout 
                            = ((0xf9ffU >= (0x0000ffffU 
                                            & (VL_MULS_III(32, (IData)(0x00000140U), __Vfunc_test_gtext__DOT__gfx_pixel__42__y) 
                                               + __Vfunc_test_gtext__DOT__gfx_pixel__42__x)))
                                ? vlSelfRef.test_gtext__DOT__dut__DOT__gfx_inst__DOT__gfx_mem__DOT__mem
                               [(0x0000ffffU & (VL_MULS_III(32, (IData)(0x00000140U), __Vfunc_test_gtext__DOT__gfx_pixel__42__y) 
                                                + __Vfunc_test_gtext__DOT__gfx_pixel__42__x))]
                                : 0U);
                    }(), (IData)(__Vfunc_test_gtext__DOT__gfx_pixel__42__Vfuncout))))) {
        vlSelfRef.test_gtext__DOT__unnamedblk4__DOT__match = 0U;
    }
    if (((~ (IData)(vlSelfRef.test_gtext__DOT__unnamedblk4__DOT__unnamedblk5__DOT__unnamedblk6__DOT__unnamedblk7__DOT__unnamedblk8__DOT__expected)) 
         & (0U != ([&]() {
                        __Vfunc_test_gtext__DOT__gfx_pixel__43__y = 4U;
                        __Vfunc_test_gtext__DOT__gfx_pixel__43__x = 0U;
                        __Vfunc_test_gtext__DOT__gfx_pixel__43__Vfuncout = 0;
                        __Vfunc_test_gtext__DOT__gfx_pixel__43__Vfuncout 
                            = ((0xf9ffU >= (0x0000ffffU 
                                            & (VL_MULS_III(32, (IData)(0x00000140U), __Vfunc_test_gtext__DOT__gfx_pixel__43__y) 
                                               + __Vfunc_test_gtext__DOT__gfx_pixel__43__x)))
                                ? vlSelfRef.test_gtext__DOT__dut__DOT__gfx_inst__DOT__gfx_mem__DOT__mem
                               [(0x0000ffffU & (VL_MULS_III(32, (IData)(0x00000140U), __Vfunc_test_gtext__DOT__gfx_pixel__43__y) 
                                                + __Vfunc_test_gtext__DOT__gfx_pixel__43__x))]
                                : 0U);
                    }(), (IData)(__Vfunc_test_gtext__DOT__gfx_pixel__43__Vfuncout))))) {
        vlSelfRef.test_gtext__DOT__unnamedblk4__DOT__match = 0U;
    }
    vlSelfRef.test_gtext__DOT__unnamedblk4__DOT__unnamedblk5__DOT__unnamedblk6__DOT__unnamedblk7__DOT__unnamedblk8__DOT__expected 
        = (1U & ((IData)(vlSelfRef.test_gtext__DOT__unnamedblk4__DOT__unnamedblk5__DOT__unnamedblk6__DOT__fb) 
                 >> 6U));
    if (((IData)(vlSelfRef.test_gtext__DOT__unnamedblk4__DOT__unnamedblk5__DOT__unnamedblk6__DOT__unnamedblk7__DOT__unnamedblk8__DOT__expected) 
         & (1U != ([&]() {
                        __Vfunc_test_gtext__DOT__gfx_pixel__42__y = 4U;
                        __Vfunc_test_gtext__DOT__gfx_pixel__42__x = 1U;
                        __Vfunc_test_gtext__DOT__gfx_pixel__42__Vfuncout = 0;
                        __Vfunc_test_gtext__DOT__gfx_pixel__42__Vfuncout 
                            = ((0xf9ffU >= (0x0000ffffU 
                                            & (VL_MULS_III(32, (IData)(0x00000140U), __Vfunc_test_gtext__DOT__gfx_pixel__42__y) 
                                               + __Vfunc_test_gtext__DOT__gfx_pixel__42__x)))
                                ? vlSelfRef.test_gtext__DOT__dut__DOT__gfx_inst__DOT__gfx_mem__DOT__mem
                               [(0x0000ffffU & (VL_MULS_III(32, (IData)(0x00000140U), __Vfunc_test_gtext__DOT__gfx_pixel__42__y) 
                                                + __Vfunc_test_gtext__DOT__gfx_pixel__42__x))]
                                : 0U);
                    }(), (IData)(__Vfunc_test_gtext__DOT__gfx_pixel__42__Vfuncout))))) {
        vlSelfRef.test_gtext__DOT__unnamedblk4__DOT__match = 0U;
    }
    if (((~ (IData)(vlSelfRef.test_gtext__DOT__unnamedblk4__DOT__unnamedblk5__DOT__unnamedblk6__DOT__unnamedblk7__DOT__unnamedblk8__DOT__expected)) 
         & (0U != ([&]() {
                        __Vfunc_test_gtext__DOT__gfx_pixel__43__y = 4U;
                        __Vfunc_test_gtext__DOT__gfx_pixel__43__x = 1U;
                        __Vfunc_test_gtext__DOT__gfx_pixel__43__Vfuncout = 0;
                        __Vfunc_test_gtext__DOT__gfx_pixel__43__Vfuncout 
                            = ((0xf9ffU >= (0x0000ffffU 
                                            & (VL_MULS_III(32, (IData)(0x00000140U), __Vfunc_test_gtext__DOT__gfx_pixel__43__y) 
                                               + __Vfunc_test_gtext__DOT__gfx_pixel__43__x)))
                                ? vlSelfRef.test_gtext__DOT__dut__DOT__gfx_inst__DOT__gfx_mem__DOT__mem
                               [(0x0000ffffU & (VL_MULS_III(32, (IData)(0x00000140U), __Vfunc_test_gtext__DOT__gfx_pixel__43__y) 
                                                + __Vfunc_test_gtext__DOT__gfx_pixel__43__x))]
                                : 0U);
                    }(), (IData)(__Vfunc_test_gtext__DOT__gfx_pixel__43__Vfuncout))))) {
        vlSelfRef.test_gtext__DOT__unnamedblk4__DOT__match = 0U;
    }
    vlSelfRef.test_gtext__DOT__unnamedblk4__DOT__unnamedblk5__DOT__unnamedblk6__DOT__unnamedblk7__DOT__unnamedblk8__DOT__expected 
        = (1U & ((IData)(vlSelfRef.test_gtext__DOT__unnamedblk4__DOT__unnamedblk5__DOT__unnamedblk6__DOT__fb) 
                 >> 5U));
    if (((IData)(vlSelfRef.test_gtext__DOT__unnamedblk4__DOT__unnamedblk5__DOT__unnamedblk6__DOT__unnamedblk7__DOT__unnamedblk8__DOT__expected) 
         & (1U != ([&]() {
                        __Vfunc_test_gtext__DOT__gfx_pixel__42__y = 4U;
                        __Vfunc_test_gtext__DOT__gfx_pixel__42__x = 2U;
                        __Vfunc_test_gtext__DOT__gfx_pixel__42__Vfuncout = 0;
                        __Vfunc_test_gtext__DOT__gfx_pixel__42__Vfuncout 
                            = ((0xf9ffU >= (0x0000ffffU 
                                            & (VL_MULS_III(32, (IData)(0x00000140U), __Vfunc_test_gtext__DOT__gfx_pixel__42__y) 
                                               + __Vfunc_test_gtext__DOT__gfx_pixel__42__x)))
                                ? vlSelfRef.test_gtext__DOT__dut__DOT__gfx_inst__DOT__gfx_mem__DOT__mem
                               [(0x0000ffffU & (VL_MULS_III(32, (IData)(0x00000140U), __Vfunc_test_gtext__DOT__gfx_pixel__42__y) 
                                                + __Vfunc_test_gtext__DOT__gfx_pixel__42__x))]
                                : 0U);
                    }(), (IData)(__Vfunc_test_gtext__DOT__gfx_pixel__42__Vfuncout))))) {
        vlSelfRef.test_gtext__DOT__unnamedblk4__DOT__match = 0U;
    }
    if (((~ (IData)(vlSelfRef.test_gtext__DOT__unnamedblk4__DOT__unnamedblk5__DOT__unnamedblk6__DOT__unnamedblk7__DOT__unnamedblk8__DOT__expected)) 
         & (0U != ([&]() {
                        __Vfunc_test_gtext__DOT__gfx_pixel__43__y = 4U;
                        __Vfunc_test_gtext__DOT__gfx_pixel__43__x = 2U;
                        __Vfunc_test_gtext__DOT__gfx_pixel__43__Vfuncout = 0;
                        __Vfunc_test_gtext__DOT__gfx_pixel__43__Vfuncout 
                            = ((0xf9ffU >= (0x0000ffffU 
                                            & (VL_MULS_III(32, (IData)(0x00000140U), __Vfunc_test_gtext__DOT__gfx_pixel__43__y) 
                                               + __Vfunc_test_gtext__DOT__gfx_pixel__43__x)))
                                ? vlSelfRef.test_gtext__DOT__dut__DOT__gfx_inst__DOT__gfx_mem__DOT__mem
                               [(0x0000ffffU & (VL_MULS_III(32, (IData)(0x00000140U), __Vfunc_test_gtext__DOT__gfx_pixel__43__y) 
                                                + __Vfunc_test_gtext__DOT__gfx_pixel__43__x))]
                                : 0U);
                    }(), (IData)(__Vfunc_test_gtext__DOT__gfx_pixel__43__Vfuncout))))) {
        vlSelfRef.test_gtext__DOT__unnamedblk4__DOT__match = 0U;
    }
    vlSelfRef.test_gtext__DOT__unnamedblk4__DOT__unnamedblk5__DOT__unnamedblk6__DOT__unnamedblk7__DOT__unnamedblk8__DOT__expected 
        = (1U & ((IData)(vlSelfRef.test_gtext__DOT__unnamedblk4__DOT__unnamedblk5__DOT__unnamedblk6__DOT__fb) 
                 >> 4U));
    if (((IData)(vlSelfRef.test_gtext__DOT__unnamedblk4__DOT__unnamedblk5__DOT__unnamedblk6__DOT__unnamedblk7__DOT__unnamedblk8__DOT__expected) 
         & (1U != ([&]() {
                        __Vfunc_test_gtext__DOT__gfx_pixel__42__y = 4U;
                        __Vfunc_test_gtext__DOT__gfx_pixel__42__x = 3U;
                        __Vfunc_test_gtext__DOT__gfx_pixel__42__Vfuncout = 0;
                        __Vfunc_test_gtext__DOT__gfx_pixel__42__Vfuncout 
                            = ((0xf9ffU >= (0x0000ffffU 
                                            & (VL_MULS_III(32, (IData)(0x00000140U), __Vfunc_test_gtext__DOT__gfx_pixel__42__y) 
                                               + __Vfunc_test_gtext__DOT__gfx_pixel__42__x)))
                                ? vlSelfRef.test_gtext__DOT__dut__DOT__gfx_inst__DOT__gfx_mem__DOT__mem
                               [(0x0000ffffU & (VL_MULS_III(32, (IData)(0x00000140U), __Vfunc_test_gtext__DOT__gfx_pixel__42__y) 
                                                + __Vfunc_test_gtext__DOT__gfx_pixel__42__x))]
                                : 0U);
                    }(), (IData)(__Vfunc_test_gtext__DOT__gfx_pixel__42__Vfuncout))))) {
        vlSelfRef.test_gtext__DOT__unnamedblk4__DOT__match = 0U;
    }
    if (((~ (IData)(vlSelfRef.test_gtext__DOT__unnamedblk4__DOT__unnamedblk5__DOT__unnamedblk6__DOT__unnamedblk7__DOT__unnamedblk8__DOT__expected)) 
         & (0U != ([&]() {
                        __Vfunc_test_gtext__DOT__gfx_pixel__43__y = 4U;
                        __Vfunc_test_gtext__DOT__gfx_pixel__43__x = 3U;
                        __Vfunc_test_gtext__DOT__gfx_pixel__43__Vfuncout = 0;
                        __Vfunc_test_gtext__DOT__gfx_pixel__43__Vfuncout 
                            = ((0xf9ffU >= (0x0000ffffU 
                                            & (VL_MULS_III(32, (IData)(0x00000140U), __Vfunc_test_gtext__DOT__gfx_pixel__43__y) 
                                               + __Vfunc_test_gtext__DOT__gfx_pixel__43__x)))
                                ? vlSelfRef.test_gtext__DOT__dut__DOT__gfx_inst__DOT__gfx_mem__DOT__mem
                               [(0x0000ffffU & (VL_MULS_III(32, (IData)(0x00000140U), __Vfunc_test_gtext__DOT__gfx_pixel__43__y) 
                                                + __Vfunc_test_gtext__DOT__gfx_pixel__43__x))]
                                : 0U);
                    }(), (IData)(__Vfunc_test_gtext__DOT__gfx_pixel__43__Vfuncout))))) {
        vlSelfRef.test_gtext__DOT__unnamedblk4__DOT__match = 0U;
    }
    vlSelfRef.test_gtext__DOT__unnamedblk4__DOT__unnamedblk5__DOT__unnamedblk6__DOT__unnamedblk7__DOT__unnamedblk8__DOT__expected 
        = (1U & ((IData)(vlSelfRef.test_gtext__DOT__unnamedblk4__DOT__unnamedblk5__DOT__unnamedblk6__DOT__fb) 
                 >> 3U));
    if (((IData)(vlSelfRef.test_gtext__DOT__unnamedblk4__DOT__unnamedblk5__DOT__unnamedblk6__DOT__unnamedblk7__DOT__unnamedblk8__DOT__expected) 
         & (1U != ([&]() {
                        __Vfunc_test_gtext__DOT__gfx_pixel__42__y = 4U;
                        __Vfunc_test_gtext__DOT__gfx_pixel__42__x = 4U;
                        __Vfunc_test_gtext__DOT__gfx_pixel__42__Vfuncout = 0;
                        __Vfunc_test_gtext__DOT__gfx_pixel__42__Vfuncout 
                            = ((0xf9ffU >= (0x0000ffffU 
                                            & (VL_MULS_III(32, (IData)(0x00000140U), __Vfunc_test_gtext__DOT__gfx_pixel__42__y) 
                                               + __Vfunc_test_gtext__DOT__gfx_pixel__42__x)))
                                ? vlSelfRef.test_gtext__DOT__dut__DOT__gfx_inst__DOT__gfx_mem__DOT__mem
                               [(0x0000ffffU & (VL_MULS_III(32, (IData)(0x00000140U), __Vfunc_test_gtext__DOT__gfx_pixel__42__y) 
                                                + __Vfunc_test_gtext__DOT__gfx_pixel__42__x))]
                                : 0U);
                    }(), (IData)(__Vfunc_test_gtext__DOT__gfx_pixel__42__Vfuncout))))) {
        vlSelfRef.test_gtext__DOT__unnamedblk4__DOT__match = 0U;
    }
    if (((~ (IData)(vlSelfRef.test_gtext__DOT__unnamedblk4__DOT__unnamedblk5__DOT__unnamedblk6__DOT__unnamedblk7__DOT__unnamedblk8__DOT__expected)) 
         & (0U != ([&]() {
                        __Vfunc_test_gtext__DOT__gfx_pixel__43__y = 4U;
                        __Vfunc_test_gtext__DOT__gfx_pixel__43__x = 4U;
                        __Vfunc_test_gtext__DOT__gfx_pixel__43__Vfuncout = 0;
                        __Vfunc_test_gtext__DOT__gfx_pixel__43__Vfuncout 
                            = ((0xf9ffU >= (0x0000ffffU 
                                            & (VL_MULS_III(32, (IData)(0x00000140U), __Vfunc_test_gtext__DOT__gfx_pixel__43__y) 
                                               + __Vfunc_test_gtext__DOT__gfx_pixel__43__x)))
                                ? vlSelfRef.test_gtext__DOT__dut__DOT__gfx_inst__DOT__gfx_mem__DOT__mem
                               [(0x0000ffffU & (VL_MULS_III(32, (IData)(0x00000140U), __Vfunc_test_gtext__DOT__gfx_pixel__43__y) 
                                                + __Vfunc_test_gtext__DOT__gfx_pixel__43__x))]
                                : 0U);
                    }(), (IData)(__Vfunc_test_gtext__DOT__gfx_pixel__43__Vfuncout))))) {
        vlSelfRef.test_gtext__DOT__unnamedblk4__DOT__match = 0U;
    }
    vlSelfRef.test_gtext__DOT__unnamedblk4__DOT__unnamedblk5__DOT__unnamedblk6__DOT__unnamedblk7__DOT__unnamedblk8__DOT__expected 
        = (1U & ((IData)(vlSelfRef.test_gtext__DOT__unnamedblk4__DOT__unnamedblk5__DOT__unnamedblk6__DOT__fb) 
                 >> 2U));
    if (((IData)(vlSelfRef.test_gtext__DOT__unnamedblk4__DOT__unnamedblk5__DOT__unnamedblk6__DOT__unnamedblk7__DOT__unnamedblk8__DOT__expected) 
         & (1U != ([&]() {
                        __Vfunc_test_gtext__DOT__gfx_pixel__42__y = 4U;
                        __Vfunc_test_gtext__DOT__gfx_pixel__42__x = 5U;
                        __Vfunc_test_gtext__DOT__gfx_pixel__42__Vfuncout = 0;
                        __Vfunc_test_gtext__DOT__gfx_pixel__42__Vfuncout 
                            = ((0xf9ffU >= (0x0000ffffU 
                                            & (VL_MULS_III(32, (IData)(0x00000140U), __Vfunc_test_gtext__DOT__gfx_pixel__42__y) 
                                               + __Vfunc_test_gtext__DOT__gfx_pixel__42__x)))
                                ? vlSelfRef.test_gtext__DOT__dut__DOT__gfx_inst__DOT__gfx_mem__DOT__mem
                               [(0x0000ffffU & (VL_MULS_III(32, (IData)(0x00000140U), __Vfunc_test_gtext__DOT__gfx_pixel__42__y) 
                                                + __Vfunc_test_gtext__DOT__gfx_pixel__42__x))]
                                : 0U);
                    }(), (IData)(__Vfunc_test_gtext__DOT__gfx_pixel__42__Vfuncout))))) {
        vlSelfRef.test_gtext__DOT__unnamedblk4__DOT__match = 0U;
    }
    if (((~ (IData)(vlSelfRef.test_gtext__DOT__unnamedblk4__DOT__unnamedblk5__DOT__unnamedblk6__DOT__unnamedblk7__DOT__unnamedblk8__DOT__expected)) 
         & (0U != ([&]() {
                        __Vfunc_test_gtext__DOT__gfx_pixel__43__y = 4U;
                        __Vfunc_test_gtext__DOT__gfx_pixel__43__x = 5U;
                        __Vfunc_test_gtext__DOT__gfx_pixel__43__Vfuncout = 0;
                        __Vfunc_test_gtext__DOT__gfx_pixel__43__Vfuncout 
                            = ((0xf9ffU >= (0x0000ffffU 
                                            & (VL_MULS_III(32, (IData)(0x00000140U), __Vfunc_test_gtext__DOT__gfx_pixel__43__y) 
                                               + __Vfunc_test_gtext__DOT__gfx_pixel__43__x)))
                                ? vlSelfRef.test_gtext__DOT__dut__DOT__gfx_inst__DOT__gfx_mem__DOT__mem
                               [(0x0000ffffU & (VL_MULS_III(32, (IData)(0x00000140U), __Vfunc_test_gtext__DOT__gfx_pixel__43__y) 
                                                + __Vfunc_test_gtext__DOT__gfx_pixel__43__x))]
                                : 0U);
                    }(), (IData)(__Vfunc_test_gtext__DOT__gfx_pixel__43__Vfuncout))))) {
        vlSelfRef.test_gtext__DOT__unnamedblk4__DOT__match = 0U;
    }
    vlSelfRef.test_gtext__DOT__unnamedblk4__DOT__unnamedblk5__DOT__unnamedblk6__DOT__unnamedblk7__DOT__unnamedblk8__DOT__expected 
        = (1U & ((IData)(vlSelfRef.test_gtext__DOT__unnamedblk4__DOT__unnamedblk5__DOT__unnamedblk6__DOT__fb) 
                 >> 1U));
    if (((IData)(vlSelfRef.test_gtext__DOT__unnamedblk4__DOT__unnamedblk5__DOT__unnamedblk6__DOT__unnamedblk7__DOT__unnamedblk8__DOT__expected) 
         & (1U != ([&]() {
                        __Vfunc_test_gtext__DOT__gfx_pixel__42__y = 4U;
                        __Vfunc_test_gtext__DOT__gfx_pixel__42__x = 6U;
                        __Vfunc_test_gtext__DOT__gfx_pixel__42__Vfuncout = 0;
                        __Vfunc_test_gtext__DOT__gfx_pixel__42__Vfuncout 
                            = ((0xf9ffU >= (0x0000ffffU 
                                            & (VL_MULS_III(32, (IData)(0x00000140U), __Vfunc_test_gtext__DOT__gfx_pixel__42__y) 
                                               + __Vfunc_test_gtext__DOT__gfx_pixel__42__x)))
                                ? vlSelfRef.test_gtext__DOT__dut__DOT__gfx_inst__DOT__gfx_mem__DOT__mem
                               [(0x0000ffffU & (VL_MULS_III(32, (IData)(0x00000140U), __Vfunc_test_gtext__DOT__gfx_pixel__42__y) 
                                                + __Vfunc_test_gtext__DOT__gfx_pixel__42__x))]
                                : 0U);
                    }(), (IData)(__Vfunc_test_gtext__DOT__gfx_pixel__42__Vfuncout))))) {
        vlSelfRef.test_gtext__DOT__unnamedblk4__DOT__match = 0U;
    }
    if (((~ (IData)(vlSelfRef.test_gtext__DOT__unnamedblk4__DOT__unnamedblk5__DOT__unnamedblk6__DOT__unnamedblk7__DOT__unnamedblk8__DOT__expected)) 
         & (0U != ([&]() {
                        __Vfunc_test_gtext__DOT__gfx_pixel__43__y = 4U;
                        __Vfunc_test_gtext__DOT__gfx_pixel__43__x = 6U;
                        __Vfunc_test_gtext__DOT__gfx_pixel__43__Vfuncout = 0;
                        __Vfunc_test_gtext__DOT__gfx_pixel__43__Vfuncout 
                            = ((0xf9ffU >= (0x0000ffffU 
                                            & (VL_MULS_III(32, (IData)(0x00000140U), __Vfunc_test_gtext__DOT__gfx_pixel__43__y) 
                                               + __Vfunc_test_gtext__DOT__gfx_pixel__43__x)))
                                ? vlSelfRef.test_gtext__DOT__dut__DOT__gfx_inst__DOT__gfx_mem__DOT__mem
                               [(0x0000ffffU & (VL_MULS_III(32, (IData)(0x00000140U), __Vfunc_test_gtext__DOT__gfx_pixel__43__y) 
                                                + __Vfunc_test_gtext__DOT__gfx_pixel__43__x))]
                                : 0U);
                    }(), (IData)(__Vfunc_test_gtext__DOT__gfx_pixel__43__Vfuncout))))) {
        vlSelfRef.test_gtext__DOT__unnamedblk4__DOT__match = 0U;
    }
    vlSelfRef.test_gtext__DOT__unnamedblk4__DOT__unnamedblk5__DOT__unnamedblk6__DOT__unnamedblk7__DOT__unnamedblk8__DOT__expected 
        = (1U & (IData)(vlSelfRef.test_gtext__DOT__unnamedblk4__DOT__unnamedblk5__DOT__unnamedblk6__DOT__fb));
    if (((IData)(vlSelfRef.test_gtext__DOT__unnamedblk4__DOT__unnamedblk5__DOT__unnamedblk6__DOT__unnamedblk7__DOT__unnamedblk8__DOT__expected) 
         & (1U != ([&]() {
                        __Vfunc_test_gtext__DOT__gfx_pixel__42__y = 4U;
                        __Vfunc_test_gtext__DOT__gfx_pixel__42__x = 7U;
                        __Vfunc_test_gtext__DOT__gfx_pixel__42__Vfuncout = 0;
                        __Vfunc_test_gtext__DOT__gfx_pixel__42__Vfuncout 
                            = ((0xf9ffU >= (0x0000ffffU 
                                            & (VL_MULS_III(32, (IData)(0x00000140U), __Vfunc_test_gtext__DOT__gfx_pixel__42__y) 
                                               + __Vfunc_test_gtext__DOT__gfx_pixel__42__x)))
                                ? vlSelfRef.test_gtext__DOT__dut__DOT__gfx_inst__DOT__gfx_mem__DOT__mem
                               [(0x0000ffffU & (VL_MULS_III(32, (IData)(0x00000140U), __Vfunc_test_gtext__DOT__gfx_pixel__42__y) 
                                                + __Vfunc_test_gtext__DOT__gfx_pixel__42__x))]
                                : 0U);
                    }(), (IData)(__Vfunc_test_gtext__DOT__gfx_pixel__42__Vfuncout))))) {
        vlSelfRef.test_gtext__DOT__unnamedblk4__DOT__match = 0U;
    }
    if (((~ (IData)(vlSelfRef.test_gtext__DOT__unnamedblk4__DOT__unnamedblk5__DOT__unnamedblk6__DOT__unnamedblk7__DOT__unnamedblk8__DOT__expected)) 
         & (0U != ([&]() {
                        __Vfunc_test_gtext__DOT__gfx_pixel__43__y = 4U;
                        __Vfunc_test_gtext__DOT__gfx_pixel__43__x = 7U;
                        __Vfunc_test_gtext__DOT__gfx_pixel__43__Vfuncout = 0;
                        __Vfunc_test_gtext__DOT__gfx_pixel__43__Vfuncout 
                            = ((0xf9ffU >= (0x0000ffffU 
                                            & (VL_MULS_III(32, (IData)(0x00000140U), __Vfunc_test_gtext__DOT__gfx_pixel__43__y) 
                                               + __Vfunc_test_gtext__DOT__gfx_pixel__43__x)))
                                ? vlSelfRef.test_gtext__DOT__dut__DOT__gfx_inst__DOT__gfx_mem__DOT__mem
                               [(0x0000ffffU & (VL_MULS_III(32, (IData)(0x00000140U), __Vfunc_test_gtext__DOT__gfx_pixel__43__y) 
                                                + __Vfunc_test_gtext__DOT__gfx_pixel__43__x))]
                                : 0U);
                    }(), (IData)(__Vfunc_test_gtext__DOT__gfx_pixel__43__Vfuncout))))) {
        vlSelfRef.test_gtext__DOT__unnamedblk4__DOT__match = 0U;
    }
    vlSelfRef.test_gtext__DOT__unnamedblk4__DOT__unnamedblk5__DOT__unnamedblk6__DOT__fb 
        = vlSelfRef.test_gtext__DOT__dut__DOT__text_inst__DOT__font_mem__DOT__mem[525U];
    vlSelfRef.test_gtext__DOT__unnamedblk4__DOT__unnamedblk5__DOT__unnamedblk6__DOT__unnamedblk7__DOT__unnamedblk8__DOT__expected 
        = (1U & ((IData)(vlSelfRef.test_gtext__DOT__unnamedblk4__DOT__unnamedblk5__DOT__unnamedblk6__DOT__fb) 
                 >> 7U));
    if (((IData)(vlSelfRef.test_gtext__DOT__unnamedblk4__DOT__unnamedblk5__DOT__unnamedblk6__DOT__unnamedblk7__DOT__unnamedblk8__DOT__expected) 
         & (1U != ([&]() {
                        __Vfunc_test_gtext__DOT__gfx_pixel__42__y = 5U;
                        __Vfunc_test_gtext__DOT__gfx_pixel__42__x = 0U;
                        __Vfunc_test_gtext__DOT__gfx_pixel__42__Vfuncout = 0;
                        __Vfunc_test_gtext__DOT__gfx_pixel__42__Vfuncout 
                            = ((0xf9ffU >= (0x0000ffffU 
                                            & (VL_MULS_III(32, (IData)(0x00000140U), __Vfunc_test_gtext__DOT__gfx_pixel__42__y) 
                                               + __Vfunc_test_gtext__DOT__gfx_pixel__42__x)))
                                ? vlSelfRef.test_gtext__DOT__dut__DOT__gfx_inst__DOT__gfx_mem__DOT__mem
                               [(0x0000ffffU & (VL_MULS_III(32, (IData)(0x00000140U), __Vfunc_test_gtext__DOT__gfx_pixel__42__y) 
                                                + __Vfunc_test_gtext__DOT__gfx_pixel__42__x))]
                                : 0U);
                    }(), (IData)(__Vfunc_test_gtext__DOT__gfx_pixel__42__Vfuncout))))) {
        vlSelfRef.test_gtext__DOT__unnamedblk4__DOT__match = 0U;
    }
    if (((~ (IData)(vlSelfRef.test_gtext__DOT__unnamedblk4__DOT__unnamedblk5__DOT__unnamedblk6__DOT__unnamedblk7__DOT__unnamedblk8__DOT__expected)) 
         & (0U != ([&]() {
                        __Vfunc_test_gtext__DOT__gfx_pixel__43__y = 5U;
                        __Vfunc_test_gtext__DOT__gfx_pixel__43__x = 0U;
                        __Vfunc_test_gtext__DOT__gfx_pixel__43__Vfuncout = 0;
                        __Vfunc_test_gtext__DOT__gfx_pixel__43__Vfuncout 
                            = ((0xf9ffU >= (0x0000ffffU 
                                            & (VL_MULS_III(32, (IData)(0x00000140U), __Vfunc_test_gtext__DOT__gfx_pixel__43__y) 
                                               + __Vfunc_test_gtext__DOT__gfx_pixel__43__x)))
                                ? vlSelfRef.test_gtext__DOT__dut__DOT__gfx_inst__DOT__gfx_mem__DOT__mem
                               [(0x0000ffffU & (VL_MULS_III(32, (IData)(0x00000140U), __Vfunc_test_gtext__DOT__gfx_pixel__43__y) 
                                                + __Vfunc_test_gtext__DOT__gfx_pixel__43__x))]
                                : 0U);
                    }(), (IData)(__Vfunc_test_gtext__DOT__gfx_pixel__43__Vfuncout))))) {
        vlSelfRef.test_gtext__DOT__unnamedblk4__DOT__match = 0U;
    }
    vlSelfRef.test_gtext__DOT__unnamedblk4__DOT__unnamedblk5__DOT__unnamedblk6__DOT__unnamedblk7__DOT__unnamedblk8__DOT__expected 
        = (1U & ((IData)(vlSelfRef.test_gtext__DOT__unnamedblk4__DOT__unnamedblk5__DOT__unnamedblk6__DOT__fb) 
                 >> 6U));
    if (((IData)(vlSelfRef.test_gtext__DOT__unnamedblk4__DOT__unnamedblk5__DOT__unnamedblk6__DOT__unnamedblk7__DOT__unnamedblk8__DOT__expected) 
         & (1U != ([&]() {
                        __Vfunc_test_gtext__DOT__gfx_pixel__42__y = 5U;
                        __Vfunc_test_gtext__DOT__gfx_pixel__42__x = 1U;
                        __Vfunc_test_gtext__DOT__gfx_pixel__42__Vfuncout = 0;
                        __Vfunc_test_gtext__DOT__gfx_pixel__42__Vfuncout 
                            = ((0xf9ffU >= (0x0000ffffU 
                                            & (VL_MULS_III(32, (IData)(0x00000140U), __Vfunc_test_gtext__DOT__gfx_pixel__42__y) 
                                               + __Vfunc_test_gtext__DOT__gfx_pixel__42__x)))
                                ? vlSelfRef.test_gtext__DOT__dut__DOT__gfx_inst__DOT__gfx_mem__DOT__mem
                               [(0x0000ffffU & (VL_MULS_III(32, (IData)(0x00000140U), __Vfunc_test_gtext__DOT__gfx_pixel__42__y) 
                                                + __Vfunc_test_gtext__DOT__gfx_pixel__42__x))]
                                : 0U);
                    }(), (IData)(__Vfunc_test_gtext__DOT__gfx_pixel__42__Vfuncout))))) {
        vlSelfRef.test_gtext__DOT__unnamedblk4__DOT__match = 0U;
    }
    if (((~ (IData)(vlSelfRef.test_gtext__DOT__unnamedblk4__DOT__unnamedblk5__DOT__unnamedblk6__DOT__unnamedblk7__DOT__unnamedblk8__DOT__expected)) 
         & (0U != ([&]() {
                        __Vfunc_test_gtext__DOT__gfx_pixel__43__y = 5U;
                        __Vfunc_test_gtext__DOT__gfx_pixel__43__x = 1U;
                        __Vfunc_test_gtext__DOT__gfx_pixel__43__Vfuncout = 0;
                        __Vfunc_test_gtext__DOT__gfx_pixel__43__Vfuncout 
                            = ((0xf9ffU >= (0x0000ffffU 
                                            & (VL_MULS_III(32, (IData)(0x00000140U), __Vfunc_test_gtext__DOT__gfx_pixel__43__y) 
                                               + __Vfunc_test_gtext__DOT__gfx_pixel__43__x)))
                                ? vlSelfRef.test_gtext__DOT__dut__DOT__gfx_inst__DOT__gfx_mem__DOT__mem
                               [(0x0000ffffU & (VL_MULS_III(32, (IData)(0x00000140U), __Vfunc_test_gtext__DOT__gfx_pixel__43__y) 
                                                + __Vfunc_test_gtext__DOT__gfx_pixel__43__x))]
                                : 0U);
                    }(), (IData)(__Vfunc_test_gtext__DOT__gfx_pixel__43__Vfuncout))))) {
        vlSelfRef.test_gtext__DOT__unnamedblk4__DOT__match = 0U;
    }
    vlSelfRef.test_gtext__DOT__unnamedblk4__DOT__unnamedblk5__DOT__unnamedblk6__DOT__unnamedblk7__DOT__unnamedblk8__DOT__expected 
        = (1U & ((IData)(vlSelfRef.test_gtext__DOT__unnamedblk4__DOT__unnamedblk5__DOT__unnamedblk6__DOT__fb) 
                 >> 5U));
    if (((IData)(vlSelfRef.test_gtext__DOT__unnamedblk4__DOT__unnamedblk5__DOT__unnamedblk6__DOT__unnamedblk7__DOT__unnamedblk8__DOT__expected) 
         & (1U != ([&]() {
                        __Vfunc_test_gtext__DOT__gfx_pixel__42__y = 5U;
                        __Vfunc_test_gtext__DOT__gfx_pixel__42__x = 2U;
                        __Vfunc_test_gtext__DOT__gfx_pixel__42__Vfuncout = 0;
                        __Vfunc_test_gtext__DOT__gfx_pixel__42__Vfuncout 
                            = ((0xf9ffU >= (0x0000ffffU 
                                            & (VL_MULS_III(32, (IData)(0x00000140U), __Vfunc_test_gtext__DOT__gfx_pixel__42__y) 
                                               + __Vfunc_test_gtext__DOT__gfx_pixel__42__x)))
                                ? vlSelfRef.test_gtext__DOT__dut__DOT__gfx_inst__DOT__gfx_mem__DOT__mem
                               [(0x0000ffffU & (VL_MULS_III(32, (IData)(0x00000140U), __Vfunc_test_gtext__DOT__gfx_pixel__42__y) 
                                                + __Vfunc_test_gtext__DOT__gfx_pixel__42__x))]
                                : 0U);
                    }(), (IData)(__Vfunc_test_gtext__DOT__gfx_pixel__42__Vfuncout))))) {
        vlSelfRef.test_gtext__DOT__unnamedblk4__DOT__match = 0U;
    }
    if (((~ (IData)(vlSelfRef.test_gtext__DOT__unnamedblk4__DOT__unnamedblk5__DOT__unnamedblk6__DOT__unnamedblk7__DOT__unnamedblk8__DOT__expected)) 
         & (0U != ([&]() {
                        __Vfunc_test_gtext__DOT__gfx_pixel__43__y = 5U;
                        __Vfunc_test_gtext__DOT__gfx_pixel__43__x = 2U;
                        __Vfunc_test_gtext__DOT__gfx_pixel__43__Vfuncout = 0;
                        __Vfunc_test_gtext__DOT__gfx_pixel__43__Vfuncout 
                            = ((0xf9ffU >= (0x0000ffffU 
                                            & (VL_MULS_III(32, (IData)(0x00000140U), __Vfunc_test_gtext__DOT__gfx_pixel__43__y) 
                                               + __Vfunc_test_gtext__DOT__gfx_pixel__43__x)))
                                ? vlSelfRef.test_gtext__DOT__dut__DOT__gfx_inst__DOT__gfx_mem__DOT__mem
                               [(0x0000ffffU & (VL_MULS_III(32, (IData)(0x00000140U), __Vfunc_test_gtext__DOT__gfx_pixel__43__y) 
                                                + __Vfunc_test_gtext__DOT__gfx_pixel__43__x))]
                                : 0U);
                    }(), (IData)(__Vfunc_test_gtext__DOT__gfx_pixel__43__Vfuncout))))) {
        vlSelfRef.test_gtext__DOT__unnamedblk4__DOT__match = 0U;
    }
    vlSelfRef.test_gtext__DOT__unnamedblk4__DOT__unnamedblk5__DOT__unnamedblk6__DOT__unnamedblk7__DOT__unnamedblk8__DOT__expected 
        = (1U & ((IData)(vlSelfRef.test_gtext__DOT__unnamedblk4__DOT__unnamedblk5__DOT__unnamedblk6__DOT__fb) 
                 >> 4U));
    if (((IData)(vlSelfRef.test_gtext__DOT__unnamedblk4__DOT__unnamedblk5__DOT__unnamedblk6__DOT__unnamedblk7__DOT__unnamedblk8__DOT__expected) 
         & (1U != ([&]() {
                        __Vfunc_test_gtext__DOT__gfx_pixel__42__y = 5U;
                        __Vfunc_test_gtext__DOT__gfx_pixel__42__x = 3U;
                        __Vfunc_test_gtext__DOT__gfx_pixel__42__Vfuncout = 0;
                        __Vfunc_test_gtext__DOT__gfx_pixel__42__Vfuncout 
                            = ((0xf9ffU >= (0x0000ffffU 
                                            & (VL_MULS_III(32, (IData)(0x00000140U), __Vfunc_test_gtext__DOT__gfx_pixel__42__y) 
                                               + __Vfunc_test_gtext__DOT__gfx_pixel__42__x)))
                                ? vlSelfRef.test_gtext__DOT__dut__DOT__gfx_inst__DOT__gfx_mem__DOT__mem
                               [(0x0000ffffU & (VL_MULS_III(32, (IData)(0x00000140U), __Vfunc_test_gtext__DOT__gfx_pixel__42__y) 
                                                + __Vfunc_test_gtext__DOT__gfx_pixel__42__x))]
                                : 0U);
                    }(), (IData)(__Vfunc_test_gtext__DOT__gfx_pixel__42__Vfuncout))))) {
        vlSelfRef.test_gtext__DOT__unnamedblk4__DOT__match = 0U;
    }
    if (((~ (IData)(vlSelfRef.test_gtext__DOT__unnamedblk4__DOT__unnamedblk5__DOT__unnamedblk6__DOT__unnamedblk7__DOT__unnamedblk8__DOT__expected)) 
         & (0U != ([&]() {
                        __Vfunc_test_gtext__DOT__gfx_pixel__43__y = 5U;
                        __Vfunc_test_gtext__DOT__gfx_pixel__43__x = 3U;
                        __Vfunc_test_gtext__DOT__gfx_pixel__43__Vfuncout = 0;
                        __Vfunc_test_gtext__DOT__gfx_pixel__43__Vfuncout 
                            = ((0xf9ffU >= (0x0000ffffU 
                                            & (VL_MULS_III(32, (IData)(0x00000140U), __Vfunc_test_gtext__DOT__gfx_pixel__43__y) 
                                               + __Vfunc_test_gtext__DOT__gfx_pixel__43__x)))
                                ? vlSelfRef.test_gtext__DOT__dut__DOT__gfx_inst__DOT__gfx_mem__DOT__mem
                               [(0x0000ffffU & (VL_MULS_III(32, (IData)(0x00000140U), __Vfunc_test_gtext__DOT__gfx_pixel__43__y) 
                                                + __Vfunc_test_gtext__DOT__gfx_pixel__43__x))]
                                : 0U);
                    }(), (IData)(__Vfunc_test_gtext__DOT__gfx_pixel__43__Vfuncout))))) {
        vlSelfRef.test_gtext__DOT__unnamedblk4__DOT__match = 0U;
    }
    vlSelfRef.test_gtext__DOT__unnamedblk4__DOT__unnamedblk5__DOT__unnamedblk6__DOT__unnamedblk7__DOT__unnamedblk8__DOT__expected 
        = (1U & ((IData)(vlSelfRef.test_gtext__DOT__unnamedblk4__DOT__unnamedblk5__DOT__unnamedblk6__DOT__fb) 
                 >> 3U));
    if (((IData)(vlSelfRef.test_gtext__DOT__unnamedblk4__DOT__unnamedblk5__DOT__unnamedblk6__DOT__unnamedblk7__DOT__unnamedblk8__DOT__expected) 
         & (1U != ([&]() {
                        __Vfunc_test_gtext__DOT__gfx_pixel__42__y = 5U;
                        __Vfunc_test_gtext__DOT__gfx_pixel__42__x = 4U;
                        __Vfunc_test_gtext__DOT__gfx_pixel__42__Vfuncout = 0;
                        __Vfunc_test_gtext__DOT__gfx_pixel__42__Vfuncout 
                            = ((0xf9ffU >= (0x0000ffffU 
                                            & (VL_MULS_III(32, (IData)(0x00000140U), __Vfunc_test_gtext__DOT__gfx_pixel__42__y) 
                                               + __Vfunc_test_gtext__DOT__gfx_pixel__42__x)))
                                ? vlSelfRef.test_gtext__DOT__dut__DOT__gfx_inst__DOT__gfx_mem__DOT__mem
                               [(0x0000ffffU & (VL_MULS_III(32, (IData)(0x00000140U), __Vfunc_test_gtext__DOT__gfx_pixel__42__y) 
                                                + __Vfunc_test_gtext__DOT__gfx_pixel__42__x))]
                                : 0U);
                    }(), (IData)(__Vfunc_test_gtext__DOT__gfx_pixel__42__Vfuncout))))) {
        vlSelfRef.test_gtext__DOT__unnamedblk4__DOT__match = 0U;
    }
    if (((~ (IData)(vlSelfRef.test_gtext__DOT__unnamedblk4__DOT__unnamedblk5__DOT__unnamedblk6__DOT__unnamedblk7__DOT__unnamedblk8__DOT__expected)) 
         & (0U != ([&]() {
                        __Vfunc_test_gtext__DOT__gfx_pixel__43__y = 5U;
                        __Vfunc_test_gtext__DOT__gfx_pixel__43__x = 4U;
                        __Vfunc_test_gtext__DOT__gfx_pixel__43__Vfuncout = 0;
                        __Vfunc_test_gtext__DOT__gfx_pixel__43__Vfuncout 
                            = ((0xf9ffU >= (0x0000ffffU 
                                            & (VL_MULS_III(32, (IData)(0x00000140U), __Vfunc_test_gtext__DOT__gfx_pixel__43__y) 
                                               + __Vfunc_test_gtext__DOT__gfx_pixel__43__x)))
                                ? vlSelfRef.test_gtext__DOT__dut__DOT__gfx_inst__DOT__gfx_mem__DOT__mem
                               [(0x0000ffffU & (VL_MULS_III(32, (IData)(0x00000140U), __Vfunc_test_gtext__DOT__gfx_pixel__43__y) 
                                                + __Vfunc_test_gtext__DOT__gfx_pixel__43__x))]
                                : 0U);
                    }(), (IData)(__Vfunc_test_gtext__DOT__gfx_pixel__43__Vfuncout))))) {
        vlSelfRef.test_gtext__DOT__unnamedblk4__DOT__match = 0U;
    }
    vlSelfRef.test_gtext__DOT__unnamedblk4__DOT__unnamedblk5__DOT__unnamedblk6__DOT__unnamedblk7__DOT__unnamedblk8__DOT__expected 
        = (1U & ((IData)(vlSelfRef.test_gtext__DOT__unnamedblk4__DOT__unnamedblk5__DOT__unnamedblk6__DOT__fb) 
                 >> 2U));
    if (((IData)(vlSelfRef.test_gtext__DOT__unnamedblk4__DOT__unnamedblk5__DOT__unnamedblk6__DOT__unnamedblk7__DOT__unnamedblk8__DOT__expected) 
         & (1U != ([&]() {
                        __Vfunc_test_gtext__DOT__gfx_pixel__42__y = 5U;
                        __Vfunc_test_gtext__DOT__gfx_pixel__42__x = 5U;
                        __Vfunc_test_gtext__DOT__gfx_pixel__42__Vfuncout = 0;
                        __Vfunc_test_gtext__DOT__gfx_pixel__42__Vfuncout 
                            = ((0xf9ffU >= (0x0000ffffU 
                                            & (VL_MULS_III(32, (IData)(0x00000140U), __Vfunc_test_gtext__DOT__gfx_pixel__42__y) 
                                               + __Vfunc_test_gtext__DOT__gfx_pixel__42__x)))
                                ? vlSelfRef.test_gtext__DOT__dut__DOT__gfx_inst__DOT__gfx_mem__DOT__mem
                               [(0x0000ffffU & (VL_MULS_III(32, (IData)(0x00000140U), __Vfunc_test_gtext__DOT__gfx_pixel__42__y) 
                                                + __Vfunc_test_gtext__DOT__gfx_pixel__42__x))]
                                : 0U);
                    }(), (IData)(__Vfunc_test_gtext__DOT__gfx_pixel__42__Vfuncout))))) {
        vlSelfRef.test_gtext__DOT__unnamedblk4__DOT__match = 0U;
    }
    if (((~ (IData)(vlSelfRef.test_gtext__DOT__unnamedblk4__DOT__unnamedblk5__DOT__unnamedblk6__DOT__unnamedblk7__DOT__unnamedblk8__DOT__expected)) 
         & (0U != ([&]() {
                        __Vfunc_test_gtext__DOT__gfx_pixel__43__y = 5U;
                        __Vfunc_test_gtext__DOT__gfx_pixel__43__x = 5U;
                        __Vfunc_test_gtext__DOT__gfx_pixel__43__Vfuncout = 0;
                        __Vfunc_test_gtext__DOT__gfx_pixel__43__Vfuncout 
                            = ((0xf9ffU >= (0x0000ffffU 
                                            & (VL_MULS_III(32, (IData)(0x00000140U), __Vfunc_test_gtext__DOT__gfx_pixel__43__y) 
                                               + __Vfunc_test_gtext__DOT__gfx_pixel__43__x)))
                                ? vlSelfRef.test_gtext__DOT__dut__DOT__gfx_inst__DOT__gfx_mem__DOT__mem
                               [(0x0000ffffU & (VL_MULS_III(32, (IData)(0x00000140U), __Vfunc_test_gtext__DOT__gfx_pixel__43__y) 
                                                + __Vfunc_test_gtext__DOT__gfx_pixel__43__x))]
                                : 0U);
                    }(), (IData)(__Vfunc_test_gtext__DOT__gfx_pixel__43__Vfuncout))))) {
        vlSelfRef.test_gtext__DOT__unnamedblk4__DOT__match = 0U;
    }
    vlSelfRef.test_gtext__DOT__unnamedblk4__DOT__unnamedblk5__DOT__unnamedblk6__DOT__unnamedblk7__DOT__unnamedblk8__DOT__expected 
        = (1U & ((IData)(vlSelfRef.test_gtext__DOT__unnamedblk4__DOT__unnamedblk5__DOT__unnamedblk6__DOT__fb) 
                 >> 1U));
    if (((IData)(vlSelfRef.test_gtext__DOT__unnamedblk4__DOT__unnamedblk5__DOT__unnamedblk6__DOT__unnamedblk7__DOT__unnamedblk8__DOT__expected) 
         & (1U != ([&]() {
                        __Vfunc_test_gtext__DOT__gfx_pixel__42__y = 5U;
                        __Vfunc_test_gtext__DOT__gfx_pixel__42__x = 6U;
                        __Vfunc_test_gtext__DOT__gfx_pixel__42__Vfuncout = 0;
                        __Vfunc_test_gtext__DOT__gfx_pixel__42__Vfuncout 
                            = ((0xf9ffU >= (0x0000ffffU 
                                            & (VL_MULS_III(32, (IData)(0x00000140U), __Vfunc_test_gtext__DOT__gfx_pixel__42__y) 
                                               + __Vfunc_test_gtext__DOT__gfx_pixel__42__x)))
                                ? vlSelfRef.test_gtext__DOT__dut__DOT__gfx_inst__DOT__gfx_mem__DOT__mem
                               [(0x0000ffffU & (VL_MULS_III(32, (IData)(0x00000140U), __Vfunc_test_gtext__DOT__gfx_pixel__42__y) 
                                                + __Vfunc_test_gtext__DOT__gfx_pixel__42__x))]
                                : 0U);
                    }(), (IData)(__Vfunc_test_gtext__DOT__gfx_pixel__42__Vfuncout))))) {
        vlSelfRef.test_gtext__DOT__unnamedblk4__DOT__match = 0U;
    }
    if (((~ (IData)(vlSelfRef.test_gtext__DOT__unnamedblk4__DOT__unnamedblk5__DOT__unnamedblk6__DOT__unnamedblk7__DOT__unnamedblk8__DOT__expected)) 
         & (0U != ([&]() {
                        __Vfunc_test_gtext__DOT__gfx_pixel__43__y = 5U;
                        __Vfunc_test_gtext__DOT__gfx_pixel__43__x = 6U;
                        __Vfunc_test_gtext__DOT__gfx_pixel__43__Vfuncout = 0;
                        __Vfunc_test_gtext__DOT__gfx_pixel__43__Vfuncout 
                            = ((0xf9ffU >= (0x0000ffffU 
                                            & (VL_MULS_III(32, (IData)(0x00000140U), __Vfunc_test_gtext__DOT__gfx_pixel__43__y) 
                                               + __Vfunc_test_gtext__DOT__gfx_pixel__43__x)))
                                ? vlSelfRef.test_gtext__DOT__dut__DOT__gfx_inst__DOT__gfx_mem__DOT__mem
                               [(0x0000ffffU & (VL_MULS_III(32, (IData)(0x00000140U), __Vfunc_test_gtext__DOT__gfx_pixel__43__y) 
                                                + __Vfunc_test_gtext__DOT__gfx_pixel__43__x))]
                                : 0U);
                    }(), (IData)(__Vfunc_test_gtext__DOT__gfx_pixel__43__Vfuncout))))) {
        vlSelfRef.test_gtext__DOT__unnamedblk4__DOT__match = 0U;
    }
    vlSelfRef.test_gtext__DOT__unnamedblk4__DOT__unnamedblk5__DOT__unnamedblk6__DOT__unnamedblk7__DOT__unnamedblk8__DOT__expected 
        = (1U & (IData)(vlSelfRef.test_gtext__DOT__unnamedblk4__DOT__unnamedblk5__DOT__unnamedblk6__DOT__fb));
    if (((IData)(vlSelfRef.test_gtext__DOT__unnamedblk4__DOT__unnamedblk5__DOT__unnamedblk6__DOT__unnamedblk7__DOT__unnamedblk8__DOT__expected) 
         & (1U != ([&]() {
                        __Vfunc_test_gtext__DOT__gfx_pixel__42__y = 5U;
                        __Vfunc_test_gtext__DOT__gfx_pixel__42__x = 7U;
                        __Vfunc_test_gtext__DOT__gfx_pixel__42__Vfuncout = 0;
                        __Vfunc_test_gtext__DOT__gfx_pixel__42__Vfuncout 
                            = ((0xf9ffU >= (0x0000ffffU 
                                            & (VL_MULS_III(32, (IData)(0x00000140U), __Vfunc_test_gtext__DOT__gfx_pixel__42__y) 
                                               + __Vfunc_test_gtext__DOT__gfx_pixel__42__x)))
                                ? vlSelfRef.test_gtext__DOT__dut__DOT__gfx_inst__DOT__gfx_mem__DOT__mem
                               [(0x0000ffffU & (VL_MULS_III(32, (IData)(0x00000140U), __Vfunc_test_gtext__DOT__gfx_pixel__42__y) 
                                                + __Vfunc_test_gtext__DOT__gfx_pixel__42__x))]
                                : 0U);
                    }(), (IData)(__Vfunc_test_gtext__DOT__gfx_pixel__42__Vfuncout))))) {
        vlSelfRef.test_gtext__DOT__unnamedblk4__DOT__match = 0U;
    }
    if (((~ (IData)(vlSelfRef.test_gtext__DOT__unnamedblk4__DOT__unnamedblk5__DOT__unnamedblk6__DOT__unnamedblk7__DOT__unnamedblk8__DOT__expected)) 
         & (0U != ([&]() {
                        __Vfunc_test_gtext__DOT__gfx_pixel__43__y = 5U;
                        __Vfunc_test_gtext__DOT__gfx_pixel__43__x = 7U;
                        __Vfunc_test_gtext__DOT__gfx_pixel__43__Vfuncout = 0;
                        __Vfunc_test_gtext__DOT__gfx_pixel__43__Vfuncout 
                            = ((0xf9ffU >= (0x0000ffffU 
                                            & (VL_MULS_III(32, (IData)(0x00000140U), __Vfunc_test_gtext__DOT__gfx_pixel__43__y) 
                                               + __Vfunc_test_gtext__DOT__gfx_pixel__43__x)))
                                ? vlSelfRef.test_gtext__DOT__dut__DOT__gfx_inst__DOT__gfx_mem__DOT__mem
                               [(0x0000ffffU & (VL_MULS_III(32, (IData)(0x00000140U), __Vfunc_test_gtext__DOT__gfx_pixel__43__y) 
                                                + __Vfunc_test_gtext__DOT__gfx_pixel__43__x))]
                                : 0U);
                    }(), (IData)(__Vfunc_test_gtext__DOT__gfx_pixel__43__Vfuncout))))) {
        vlSelfRef.test_gtext__DOT__unnamedblk4__DOT__match = 0U;
    }
    vlSelfRef.test_gtext__DOT__unnamedblk4__DOT__unnamedblk5__DOT__unnamedblk6__DOT__fb 
        = vlSelfRef.test_gtext__DOT__dut__DOT__text_inst__DOT__font_mem__DOT__mem[526U];
    vlSelfRef.test_gtext__DOT__unnamedblk4__DOT__unnamedblk5__DOT__unnamedblk6__DOT__unnamedblk7__DOT__unnamedblk8__DOT__expected 
        = (1U & ((IData)(vlSelfRef.test_gtext__DOT__unnamedblk4__DOT__unnamedblk5__DOT__unnamedblk6__DOT__fb) 
                 >> 7U));
    if (((IData)(vlSelfRef.test_gtext__DOT__unnamedblk4__DOT__unnamedblk5__DOT__unnamedblk6__DOT__unnamedblk7__DOT__unnamedblk8__DOT__expected) 
         & (1U != ([&]() {
                        __Vfunc_test_gtext__DOT__gfx_pixel__42__y = 6U;
                        __Vfunc_test_gtext__DOT__gfx_pixel__42__x = 0U;
                        __Vfunc_test_gtext__DOT__gfx_pixel__42__Vfuncout = 0;
                        __Vfunc_test_gtext__DOT__gfx_pixel__42__Vfuncout 
                            = ((0xf9ffU >= (0x0000ffffU 
                                            & (VL_MULS_III(32, (IData)(0x00000140U), __Vfunc_test_gtext__DOT__gfx_pixel__42__y) 
                                               + __Vfunc_test_gtext__DOT__gfx_pixel__42__x)))
                                ? vlSelfRef.test_gtext__DOT__dut__DOT__gfx_inst__DOT__gfx_mem__DOT__mem
                               [(0x0000ffffU & (VL_MULS_III(32, (IData)(0x00000140U), __Vfunc_test_gtext__DOT__gfx_pixel__42__y) 
                                                + __Vfunc_test_gtext__DOT__gfx_pixel__42__x))]
                                : 0U);
                    }(), (IData)(__Vfunc_test_gtext__DOT__gfx_pixel__42__Vfuncout))))) {
        vlSelfRef.test_gtext__DOT__unnamedblk4__DOT__match = 0U;
    }
    if (((~ (IData)(vlSelfRef.test_gtext__DOT__unnamedblk4__DOT__unnamedblk5__DOT__unnamedblk6__DOT__unnamedblk7__DOT__unnamedblk8__DOT__expected)) 
         & (0U != ([&]() {
                        __Vfunc_test_gtext__DOT__gfx_pixel__43__y = 6U;
                        __Vfunc_test_gtext__DOT__gfx_pixel__43__x = 0U;
                        __Vfunc_test_gtext__DOT__gfx_pixel__43__Vfuncout = 0;
                        __Vfunc_test_gtext__DOT__gfx_pixel__43__Vfuncout 
                            = ((0xf9ffU >= (0x0000ffffU 
                                            & (VL_MULS_III(32, (IData)(0x00000140U), __Vfunc_test_gtext__DOT__gfx_pixel__43__y) 
                                               + __Vfunc_test_gtext__DOT__gfx_pixel__43__x)))
                                ? vlSelfRef.test_gtext__DOT__dut__DOT__gfx_inst__DOT__gfx_mem__DOT__mem
                               [(0x0000ffffU & (VL_MULS_III(32, (IData)(0x00000140U), __Vfunc_test_gtext__DOT__gfx_pixel__43__y) 
                                                + __Vfunc_test_gtext__DOT__gfx_pixel__43__x))]
                                : 0U);
                    }(), (IData)(__Vfunc_test_gtext__DOT__gfx_pixel__43__Vfuncout))))) {
        vlSelfRef.test_gtext__DOT__unnamedblk4__DOT__match = 0U;
    }
    vlSelfRef.test_gtext__DOT__unnamedblk4__DOT__unnamedblk5__DOT__unnamedblk6__DOT__unnamedblk7__DOT__unnamedblk8__DOT__expected 
        = (1U & ((IData)(vlSelfRef.test_gtext__DOT__unnamedblk4__DOT__unnamedblk5__DOT__unnamedblk6__DOT__fb) 
                 >> 6U));
    if (((IData)(vlSelfRef.test_gtext__DOT__unnamedblk4__DOT__unnamedblk5__DOT__unnamedblk6__DOT__unnamedblk7__DOT__unnamedblk8__DOT__expected) 
         & (1U != ([&]() {
                        __Vfunc_test_gtext__DOT__gfx_pixel__42__y = 6U;
                        __Vfunc_test_gtext__DOT__gfx_pixel__42__x = 1U;
                        __Vfunc_test_gtext__DOT__gfx_pixel__42__Vfuncout = 0;
                        __Vfunc_test_gtext__DOT__gfx_pixel__42__Vfuncout 
                            = ((0xf9ffU >= (0x0000ffffU 
                                            & (VL_MULS_III(32, (IData)(0x00000140U), __Vfunc_test_gtext__DOT__gfx_pixel__42__y) 
                                               + __Vfunc_test_gtext__DOT__gfx_pixel__42__x)))
                                ? vlSelfRef.test_gtext__DOT__dut__DOT__gfx_inst__DOT__gfx_mem__DOT__mem
                               [(0x0000ffffU & (VL_MULS_III(32, (IData)(0x00000140U), __Vfunc_test_gtext__DOT__gfx_pixel__42__y) 
                                                + __Vfunc_test_gtext__DOT__gfx_pixel__42__x))]
                                : 0U);
                    }(), (IData)(__Vfunc_test_gtext__DOT__gfx_pixel__42__Vfuncout))))) {
        vlSelfRef.test_gtext__DOT__unnamedblk4__DOT__match = 0U;
    }
    if (((~ (IData)(vlSelfRef.test_gtext__DOT__unnamedblk4__DOT__unnamedblk5__DOT__unnamedblk6__DOT__unnamedblk7__DOT__unnamedblk8__DOT__expected)) 
         & (0U != ([&]() {
                        __Vfunc_test_gtext__DOT__gfx_pixel__43__y = 6U;
                        __Vfunc_test_gtext__DOT__gfx_pixel__43__x = 1U;
                        __Vfunc_test_gtext__DOT__gfx_pixel__43__Vfuncout = 0;
                        __Vfunc_test_gtext__DOT__gfx_pixel__43__Vfuncout 
                            = ((0xf9ffU >= (0x0000ffffU 
                                            & (VL_MULS_III(32, (IData)(0x00000140U), __Vfunc_test_gtext__DOT__gfx_pixel__43__y) 
                                               + __Vfunc_test_gtext__DOT__gfx_pixel__43__x)))
                                ? vlSelfRef.test_gtext__DOT__dut__DOT__gfx_inst__DOT__gfx_mem__DOT__mem
                               [(0x0000ffffU & (VL_MULS_III(32, (IData)(0x00000140U), __Vfunc_test_gtext__DOT__gfx_pixel__43__y) 
                                                + __Vfunc_test_gtext__DOT__gfx_pixel__43__x))]
                                : 0U);
                    }(), (IData)(__Vfunc_test_gtext__DOT__gfx_pixel__43__Vfuncout))))) {
        vlSelfRef.test_gtext__DOT__unnamedblk4__DOT__match = 0U;
    }
    vlSelfRef.test_gtext__DOT__unnamedblk4__DOT__unnamedblk5__DOT__unnamedblk6__DOT__unnamedblk7__DOT__unnamedblk8__DOT__expected 
        = (1U & ((IData)(vlSelfRef.test_gtext__DOT__unnamedblk4__DOT__unnamedblk5__DOT__unnamedblk6__DOT__fb) 
                 >> 5U));
    if (((IData)(vlSelfRef.test_gtext__DOT__unnamedblk4__DOT__unnamedblk5__DOT__unnamedblk6__DOT__unnamedblk7__DOT__unnamedblk8__DOT__expected) 
         & (1U != ([&]() {
                        __Vfunc_test_gtext__DOT__gfx_pixel__42__y = 6U;
                        __Vfunc_test_gtext__DOT__gfx_pixel__42__x = 2U;
                        __Vfunc_test_gtext__DOT__gfx_pixel__42__Vfuncout = 0;
                        __Vfunc_test_gtext__DOT__gfx_pixel__42__Vfuncout 
                            = ((0xf9ffU >= (0x0000ffffU 
                                            & (VL_MULS_III(32, (IData)(0x00000140U), __Vfunc_test_gtext__DOT__gfx_pixel__42__y) 
                                               + __Vfunc_test_gtext__DOT__gfx_pixel__42__x)))
                                ? vlSelfRef.test_gtext__DOT__dut__DOT__gfx_inst__DOT__gfx_mem__DOT__mem
                               [(0x0000ffffU & (VL_MULS_III(32, (IData)(0x00000140U), __Vfunc_test_gtext__DOT__gfx_pixel__42__y) 
                                                + __Vfunc_test_gtext__DOT__gfx_pixel__42__x))]
                                : 0U);
                    }(), (IData)(__Vfunc_test_gtext__DOT__gfx_pixel__42__Vfuncout))))) {
        vlSelfRef.test_gtext__DOT__unnamedblk4__DOT__match = 0U;
    }
    if (((~ (IData)(vlSelfRef.test_gtext__DOT__unnamedblk4__DOT__unnamedblk5__DOT__unnamedblk6__DOT__unnamedblk7__DOT__unnamedblk8__DOT__expected)) 
         & (0U != ([&]() {
                        __Vfunc_test_gtext__DOT__gfx_pixel__43__y = 6U;
                        __Vfunc_test_gtext__DOT__gfx_pixel__43__x = 2U;
                        __Vfunc_test_gtext__DOT__gfx_pixel__43__Vfuncout = 0;
                        __Vfunc_test_gtext__DOT__gfx_pixel__43__Vfuncout 
                            = ((0xf9ffU >= (0x0000ffffU 
                                            & (VL_MULS_III(32, (IData)(0x00000140U), __Vfunc_test_gtext__DOT__gfx_pixel__43__y) 
                                               + __Vfunc_test_gtext__DOT__gfx_pixel__43__x)))
                                ? vlSelfRef.test_gtext__DOT__dut__DOT__gfx_inst__DOT__gfx_mem__DOT__mem
                               [(0x0000ffffU & (VL_MULS_III(32, (IData)(0x00000140U), __Vfunc_test_gtext__DOT__gfx_pixel__43__y) 
                                                + __Vfunc_test_gtext__DOT__gfx_pixel__43__x))]
                                : 0U);
                    }(), (IData)(__Vfunc_test_gtext__DOT__gfx_pixel__43__Vfuncout))))) {
        vlSelfRef.test_gtext__DOT__unnamedblk4__DOT__match = 0U;
    }
    vlSelfRef.test_gtext__DOT__unnamedblk4__DOT__unnamedblk5__DOT__unnamedblk6__DOT__unnamedblk7__DOT__unnamedblk8__DOT__expected 
        = (1U & ((IData)(vlSelfRef.test_gtext__DOT__unnamedblk4__DOT__unnamedblk5__DOT__unnamedblk6__DOT__fb) 
                 >> 4U));
    if (((IData)(vlSelfRef.test_gtext__DOT__unnamedblk4__DOT__unnamedblk5__DOT__unnamedblk6__DOT__unnamedblk7__DOT__unnamedblk8__DOT__expected) 
         & (1U != ([&]() {
                        __Vfunc_test_gtext__DOT__gfx_pixel__42__y = 6U;
                        __Vfunc_test_gtext__DOT__gfx_pixel__42__x = 3U;
                        __Vfunc_test_gtext__DOT__gfx_pixel__42__Vfuncout = 0;
                        __Vfunc_test_gtext__DOT__gfx_pixel__42__Vfuncout 
                            = ((0xf9ffU >= (0x0000ffffU 
                                            & (VL_MULS_III(32, (IData)(0x00000140U), __Vfunc_test_gtext__DOT__gfx_pixel__42__y) 
                                               + __Vfunc_test_gtext__DOT__gfx_pixel__42__x)))
                                ? vlSelfRef.test_gtext__DOT__dut__DOT__gfx_inst__DOT__gfx_mem__DOT__mem
                               [(0x0000ffffU & (VL_MULS_III(32, (IData)(0x00000140U), __Vfunc_test_gtext__DOT__gfx_pixel__42__y) 
                                                + __Vfunc_test_gtext__DOT__gfx_pixel__42__x))]
                                : 0U);
                    }(), (IData)(__Vfunc_test_gtext__DOT__gfx_pixel__42__Vfuncout))))) {
        vlSelfRef.test_gtext__DOT__unnamedblk4__DOT__match = 0U;
    }
    if (((~ (IData)(vlSelfRef.test_gtext__DOT__unnamedblk4__DOT__unnamedblk5__DOT__unnamedblk6__DOT__unnamedblk7__DOT__unnamedblk8__DOT__expected)) 
         & (0U != ([&]() {
                        __Vfunc_test_gtext__DOT__gfx_pixel__43__y = 6U;
                        __Vfunc_test_gtext__DOT__gfx_pixel__43__x = 3U;
                        __Vfunc_test_gtext__DOT__gfx_pixel__43__Vfuncout = 0;
                        __Vfunc_test_gtext__DOT__gfx_pixel__43__Vfuncout 
                            = ((0xf9ffU >= (0x0000ffffU 
                                            & (VL_MULS_III(32, (IData)(0x00000140U), __Vfunc_test_gtext__DOT__gfx_pixel__43__y) 
                                               + __Vfunc_test_gtext__DOT__gfx_pixel__43__x)))
                                ? vlSelfRef.test_gtext__DOT__dut__DOT__gfx_inst__DOT__gfx_mem__DOT__mem
                               [(0x0000ffffU & (VL_MULS_III(32, (IData)(0x00000140U), __Vfunc_test_gtext__DOT__gfx_pixel__43__y) 
                                                + __Vfunc_test_gtext__DOT__gfx_pixel__43__x))]
                                : 0U);
                    }(), (IData)(__Vfunc_test_gtext__DOT__gfx_pixel__43__Vfuncout))))) {
        vlSelfRef.test_gtext__DOT__unnamedblk4__DOT__match = 0U;
    }
    vlSelfRef.test_gtext__DOT__unnamedblk4__DOT__unnamedblk5__DOT__unnamedblk6__DOT__unnamedblk7__DOT__unnamedblk8__DOT__expected 
        = (1U & ((IData)(vlSelfRef.test_gtext__DOT__unnamedblk4__DOT__unnamedblk5__DOT__unnamedblk6__DOT__fb) 
                 >> 3U));
    if (((IData)(vlSelfRef.test_gtext__DOT__unnamedblk4__DOT__unnamedblk5__DOT__unnamedblk6__DOT__unnamedblk7__DOT__unnamedblk8__DOT__expected) 
         & (1U != ([&]() {
                        __Vfunc_test_gtext__DOT__gfx_pixel__42__y = 6U;
                        __Vfunc_test_gtext__DOT__gfx_pixel__42__x = 4U;
                        __Vfunc_test_gtext__DOT__gfx_pixel__42__Vfuncout = 0;
                        __Vfunc_test_gtext__DOT__gfx_pixel__42__Vfuncout 
                            = ((0xf9ffU >= (0x0000ffffU 
                                            & (VL_MULS_III(32, (IData)(0x00000140U), __Vfunc_test_gtext__DOT__gfx_pixel__42__y) 
                                               + __Vfunc_test_gtext__DOT__gfx_pixel__42__x)))
                                ? vlSelfRef.test_gtext__DOT__dut__DOT__gfx_inst__DOT__gfx_mem__DOT__mem
                               [(0x0000ffffU & (VL_MULS_III(32, (IData)(0x00000140U), __Vfunc_test_gtext__DOT__gfx_pixel__42__y) 
                                                + __Vfunc_test_gtext__DOT__gfx_pixel__42__x))]
                                : 0U);
                    }(), (IData)(__Vfunc_test_gtext__DOT__gfx_pixel__42__Vfuncout))))) {
        vlSelfRef.test_gtext__DOT__unnamedblk4__DOT__match = 0U;
    }
    if (((~ (IData)(vlSelfRef.test_gtext__DOT__unnamedblk4__DOT__unnamedblk5__DOT__unnamedblk6__DOT__unnamedblk7__DOT__unnamedblk8__DOT__expected)) 
         & (0U != ([&]() {
                        __Vfunc_test_gtext__DOT__gfx_pixel__43__y = 6U;
                        __Vfunc_test_gtext__DOT__gfx_pixel__43__x = 4U;
                        __Vfunc_test_gtext__DOT__gfx_pixel__43__Vfuncout = 0;
                        __Vfunc_test_gtext__DOT__gfx_pixel__43__Vfuncout 
                            = ((0xf9ffU >= (0x0000ffffU 
                                            & (VL_MULS_III(32, (IData)(0x00000140U), __Vfunc_test_gtext__DOT__gfx_pixel__43__y) 
                                               + __Vfunc_test_gtext__DOT__gfx_pixel__43__x)))
                                ? vlSelfRef.test_gtext__DOT__dut__DOT__gfx_inst__DOT__gfx_mem__DOT__mem
                               [(0x0000ffffU & (VL_MULS_III(32, (IData)(0x00000140U), __Vfunc_test_gtext__DOT__gfx_pixel__43__y) 
                                                + __Vfunc_test_gtext__DOT__gfx_pixel__43__x))]
                                : 0U);
                    }(), (IData)(__Vfunc_test_gtext__DOT__gfx_pixel__43__Vfuncout))))) {
        vlSelfRef.test_gtext__DOT__unnamedblk4__DOT__match = 0U;
    }
    vlSelfRef.test_gtext__DOT__unnamedblk4__DOT__unnamedblk5__DOT__unnamedblk6__DOT__unnamedblk7__DOT__unnamedblk8__DOT__expected 
        = (1U & ((IData)(vlSelfRef.test_gtext__DOT__unnamedblk4__DOT__unnamedblk5__DOT__unnamedblk6__DOT__fb) 
                 >> 2U));
    if (((IData)(vlSelfRef.test_gtext__DOT__unnamedblk4__DOT__unnamedblk5__DOT__unnamedblk6__DOT__unnamedblk7__DOT__unnamedblk8__DOT__expected) 
         & (1U != ([&]() {
                        __Vfunc_test_gtext__DOT__gfx_pixel__42__y = 6U;
                        __Vfunc_test_gtext__DOT__gfx_pixel__42__x = 5U;
                        __Vfunc_test_gtext__DOT__gfx_pixel__42__Vfuncout = 0;
                        __Vfunc_test_gtext__DOT__gfx_pixel__42__Vfuncout 
                            = ((0xf9ffU >= (0x0000ffffU 
                                            & (VL_MULS_III(32, (IData)(0x00000140U), __Vfunc_test_gtext__DOT__gfx_pixel__42__y) 
                                               + __Vfunc_test_gtext__DOT__gfx_pixel__42__x)))
                                ? vlSelfRef.test_gtext__DOT__dut__DOT__gfx_inst__DOT__gfx_mem__DOT__mem
                               [(0x0000ffffU & (VL_MULS_III(32, (IData)(0x00000140U), __Vfunc_test_gtext__DOT__gfx_pixel__42__y) 
                                                + __Vfunc_test_gtext__DOT__gfx_pixel__42__x))]
                                : 0U);
                    }(), (IData)(__Vfunc_test_gtext__DOT__gfx_pixel__42__Vfuncout))))) {
        vlSelfRef.test_gtext__DOT__unnamedblk4__DOT__match = 0U;
    }
    if (((~ (IData)(vlSelfRef.test_gtext__DOT__unnamedblk4__DOT__unnamedblk5__DOT__unnamedblk6__DOT__unnamedblk7__DOT__unnamedblk8__DOT__expected)) 
         & (0U != ([&]() {
                        __Vfunc_test_gtext__DOT__gfx_pixel__43__y = 6U;
                        __Vfunc_test_gtext__DOT__gfx_pixel__43__x = 5U;
                        __Vfunc_test_gtext__DOT__gfx_pixel__43__Vfuncout = 0;
                        __Vfunc_test_gtext__DOT__gfx_pixel__43__Vfuncout 
                            = ((0xf9ffU >= (0x0000ffffU 
                                            & (VL_MULS_III(32, (IData)(0x00000140U), __Vfunc_test_gtext__DOT__gfx_pixel__43__y) 
                                               + __Vfunc_test_gtext__DOT__gfx_pixel__43__x)))
                                ? vlSelfRef.test_gtext__DOT__dut__DOT__gfx_inst__DOT__gfx_mem__DOT__mem
                               [(0x0000ffffU & (VL_MULS_III(32, (IData)(0x00000140U), __Vfunc_test_gtext__DOT__gfx_pixel__43__y) 
                                                + __Vfunc_test_gtext__DOT__gfx_pixel__43__x))]
                                : 0U);
                    }(), (IData)(__Vfunc_test_gtext__DOT__gfx_pixel__43__Vfuncout))))) {
        vlSelfRef.test_gtext__DOT__unnamedblk4__DOT__match = 0U;
    }
    vlSelfRef.test_gtext__DOT__unnamedblk4__DOT__unnamedblk5__DOT__unnamedblk6__DOT__unnamedblk7__DOT__unnamedblk8__DOT__expected 
        = (1U & ((IData)(vlSelfRef.test_gtext__DOT__unnamedblk4__DOT__unnamedblk5__DOT__unnamedblk6__DOT__fb) 
                 >> 1U));
    if (((IData)(vlSelfRef.test_gtext__DOT__unnamedblk4__DOT__unnamedblk5__DOT__unnamedblk6__DOT__unnamedblk7__DOT__unnamedblk8__DOT__expected) 
         & (1U != ([&]() {
                        __Vfunc_test_gtext__DOT__gfx_pixel__42__y = 6U;
                        __Vfunc_test_gtext__DOT__gfx_pixel__42__x = 6U;
                        __Vfunc_test_gtext__DOT__gfx_pixel__42__Vfuncout = 0;
                        __Vfunc_test_gtext__DOT__gfx_pixel__42__Vfuncout 
                            = ((0xf9ffU >= (0x0000ffffU 
                                            & (VL_MULS_III(32, (IData)(0x00000140U), __Vfunc_test_gtext__DOT__gfx_pixel__42__y) 
                                               + __Vfunc_test_gtext__DOT__gfx_pixel__42__x)))
                                ? vlSelfRef.test_gtext__DOT__dut__DOT__gfx_inst__DOT__gfx_mem__DOT__mem
                               [(0x0000ffffU & (VL_MULS_III(32, (IData)(0x00000140U), __Vfunc_test_gtext__DOT__gfx_pixel__42__y) 
                                                + __Vfunc_test_gtext__DOT__gfx_pixel__42__x))]
                                : 0U);
                    }(), (IData)(__Vfunc_test_gtext__DOT__gfx_pixel__42__Vfuncout))))) {
        vlSelfRef.test_gtext__DOT__unnamedblk4__DOT__match = 0U;
    }
    if (((~ (IData)(vlSelfRef.test_gtext__DOT__unnamedblk4__DOT__unnamedblk5__DOT__unnamedblk6__DOT__unnamedblk7__DOT__unnamedblk8__DOT__expected)) 
         & (0U != ([&]() {
                        __Vfunc_test_gtext__DOT__gfx_pixel__43__y = 6U;
                        __Vfunc_test_gtext__DOT__gfx_pixel__43__x = 6U;
                        __Vfunc_test_gtext__DOT__gfx_pixel__43__Vfuncout = 0;
                        __Vfunc_test_gtext__DOT__gfx_pixel__43__Vfuncout 
                            = ((0xf9ffU >= (0x0000ffffU 
                                            & (VL_MULS_III(32, (IData)(0x00000140U), __Vfunc_test_gtext__DOT__gfx_pixel__43__y) 
                                               + __Vfunc_test_gtext__DOT__gfx_pixel__43__x)))
                                ? vlSelfRef.test_gtext__DOT__dut__DOT__gfx_inst__DOT__gfx_mem__DOT__mem
                               [(0x0000ffffU & (VL_MULS_III(32, (IData)(0x00000140U), __Vfunc_test_gtext__DOT__gfx_pixel__43__y) 
                                                + __Vfunc_test_gtext__DOT__gfx_pixel__43__x))]
                                : 0U);
                    }(), (IData)(__Vfunc_test_gtext__DOT__gfx_pixel__43__Vfuncout))))) {
        vlSelfRef.test_gtext__DOT__unnamedblk4__DOT__match = 0U;
    }
    vlSelfRef.test_gtext__DOT__unnamedblk4__DOT__unnamedblk5__DOT__unnamedblk6__DOT__unnamedblk7__DOT__unnamedblk8__DOT__expected 
        = (1U & (IData)(vlSelfRef.test_gtext__DOT__unnamedblk4__DOT__unnamedblk5__DOT__unnamedblk6__DOT__fb));
    if (((IData)(vlSelfRef.test_gtext__DOT__unnamedblk4__DOT__unnamedblk5__DOT__unnamedblk6__DOT__unnamedblk7__DOT__unnamedblk8__DOT__expected) 
         & (1U != ([&]() {
                        __Vfunc_test_gtext__DOT__gfx_pixel__42__y = 6U;
                        __Vfunc_test_gtext__DOT__gfx_pixel__42__x = 7U;
                        __Vfunc_test_gtext__DOT__gfx_pixel__42__Vfuncout = 0;
                        __Vfunc_test_gtext__DOT__gfx_pixel__42__Vfuncout 
                            = ((0xf9ffU >= (0x0000ffffU 
                                            & (VL_MULS_III(32, (IData)(0x00000140U), __Vfunc_test_gtext__DOT__gfx_pixel__42__y) 
                                               + __Vfunc_test_gtext__DOT__gfx_pixel__42__x)))
                                ? vlSelfRef.test_gtext__DOT__dut__DOT__gfx_inst__DOT__gfx_mem__DOT__mem
                               [(0x0000ffffU & (VL_MULS_III(32, (IData)(0x00000140U), __Vfunc_test_gtext__DOT__gfx_pixel__42__y) 
                                                + __Vfunc_test_gtext__DOT__gfx_pixel__42__x))]
                                : 0U);
                    }(), (IData)(__Vfunc_test_gtext__DOT__gfx_pixel__42__Vfuncout))))) {
        vlSelfRef.test_gtext__DOT__unnamedblk4__DOT__match = 0U;
    }
    if (((~ (IData)(vlSelfRef.test_gtext__DOT__unnamedblk4__DOT__unnamedblk5__DOT__unnamedblk6__DOT__unnamedblk7__DOT__unnamedblk8__DOT__expected)) 
         & (0U != ([&]() {
                        __Vfunc_test_gtext__DOT__gfx_pixel__43__y = 6U;
                        __Vfunc_test_gtext__DOT__gfx_pixel__43__x = 7U;
                        __Vfunc_test_gtext__DOT__gfx_pixel__43__Vfuncout = 0;
                        __Vfunc_test_gtext__DOT__gfx_pixel__43__Vfuncout 
                            = ((0xf9ffU >= (0x0000ffffU 
                                            & (VL_MULS_III(32, (IData)(0x00000140U), __Vfunc_test_gtext__DOT__gfx_pixel__43__y) 
                                               + __Vfunc_test_gtext__DOT__gfx_pixel__43__x)))
                                ? vlSelfRef.test_gtext__DOT__dut__DOT__gfx_inst__DOT__gfx_mem__DOT__mem
                               [(0x0000ffffU & (VL_MULS_III(32, (IData)(0x00000140U), __Vfunc_test_gtext__DOT__gfx_pixel__43__y) 
                                                + __Vfunc_test_gtext__DOT__gfx_pixel__43__x))]
                                : 0U);
                    }(), (IData)(__Vfunc_test_gtext__DOT__gfx_pixel__43__Vfuncout))))) {
        vlSelfRef.test_gtext__DOT__unnamedblk4__DOT__match = 0U;
    }
    vlSelfRef.test_gtext__DOT__unnamedblk4__DOT__unnamedblk5__DOT__unnamedblk6__DOT__fb 
        = vlSelfRef.test_gtext__DOT__dut__DOT__text_inst__DOT__font_mem__DOT__mem[527U];
    vlSelfRef.test_gtext__DOT__unnamedblk4__DOT__unnamedblk5__DOT__unnamedblk6__DOT__unnamedblk7__DOT__unnamedblk8__DOT__expected 
        = (1U & ((IData)(vlSelfRef.test_gtext__DOT__unnamedblk4__DOT__unnamedblk5__DOT__unnamedblk6__DOT__fb) 
                 >> 7U));
    if (((IData)(vlSelfRef.test_gtext__DOT__unnamedblk4__DOT__unnamedblk5__DOT__unnamedblk6__DOT__unnamedblk7__DOT__unnamedblk8__DOT__expected) 
         & (1U != ([&]() {
                        __Vfunc_test_gtext__DOT__gfx_pixel__42__y = 7U;
                        __Vfunc_test_gtext__DOT__gfx_pixel__42__x = 0U;
                        __Vfunc_test_gtext__DOT__gfx_pixel__42__Vfuncout = 0;
                        __Vfunc_test_gtext__DOT__gfx_pixel__42__Vfuncout 
                            = ((0xf9ffU >= (0x0000ffffU 
                                            & (VL_MULS_III(32, (IData)(0x00000140U), __Vfunc_test_gtext__DOT__gfx_pixel__42__y) 
                                               + __Vfunc_test_gtext__DOT__gfx_pixel__42__x)))
                                ? vlSelfRef.test_gtext__DOT__dut__DOT__gfx_inst__DOT__gfx_mem__DOT__mem
                               [(0x0000ffffU & (VL_MULS_III(32, (IData)(0x00000140U), __Vfunc_test_gtext__DOT__gfx_pixel__42__y) 
                                                + __Vfunc_test_gtext__DOT__gfx_pixel__42__x))]
                                : 0U);
                    }(), (IData)(__Vfunc_test_gtext__DOT__gfx_pixel__42__Vfuncout))))) {
        vlSelfRef.test_gtext__DOT__unnamedblk4__DOT__match = 0U;
    }
    if (((~ (IData)(vlSelfRef.test_gtext__DOT__unnamedblk4__DOT__unnamedblk5__DOT__unnamedblk6__DOT__unnamedblk7__DOT__unnamedblk8__DOT__expected)) 
         & (0U != ([&]() {
                        __Vfunc_test_gtext__DOT__gfx_pixel__43__y = 7U;
                        __Vfunc_test_gtext__DOT__gfx_pixel__43__x = 0U;
                        __Vfunc_test_gtext__DOT__gfx_pixel__43__Vfuncout = 0;
                        __Vfunc_test_gtext__DOT__gfx_pixel__43__Vfuncout 
                            = ((0xf9ffU >= (0x0000ffffU 
                                            & (VL_MULS_III(32, (IData)(0x00000140U), __Vfunc_test_gtext__DOT__gfx_pixel__43__y) 
                                               + __Vfunc_test_gtext__DOT__gfx_pixel__43__x)))
                                ? vlSelfRef.test_gtext__DOT__dut__DOT__gfx_inst__DOT__gfx_mem__DOT__mem
                               [(0x0000ffffU & (VL_MULS_III(32, (IData)(0x00000140U), __Vfunc_test_gtext__DOT__gfx_pixel__43__y) 
                                                + __Vfunc_test_gtext__DOT__gfx_pixel__43__x))]
                                : 0U);
                    }(), (IData)(__Vfunc_test_gtext__DOT__gfx_pixel__43__Vfuncout))))) {
        vlSelfRef.test_gtext__DOT__unnamedblk4__DOT__match = 0U;
    }
    vlSelfRef.test_gtext__DOT__unnamedblk4__DOT__unnamedblk5__DOT__unnamedblk6__DOT__unnamedblk7__DOT__unnamedblk8__DOT__expected 
        = (1U & ((IData)(vlSelfRef.test_gtext__DOT__unnamedblk4__DOT__unnamedblk5__DOT__unnamedblk6__DOT__fb) 
                 >> 6U));
    if (((IData)(vlSelfRef.test_gtext__DOT__unnamedblk4__DOT__unnamedblk5__DOT__unnamedblk6__DOT__unnamedblk7__DOT__unnamedblk8__DOT__expected) 
         & (1U != ([&]() {
                        __Vfunc_test_gtext__DOT__gfx_pixel__42__y = 7U;
                        __Vfunc_test_gtext__DOT__gfx_pixel__42__x = 1U;
                        __Vfunc_test_gtext__DOT__gfx_pixel__42__Vfuncout = 0;
                        __Vfunc_test_gtext__DOT__gfx_pixel__42__Vfuncout 
                            = ((0xf9ffU >= (0x0000ffffU 
                                            & (VL_MULS_III(32, (IData)(0x00000140U), __Vfunc_test_gtext__DOT__gfx_pixel__42__y) 
                                               + __Vfunc_test_gtext__DOT__gfx_pixel__42__x)))
                                ? vlSelfRef.test_gtext__DOT__dut__DOT__gfx_inst__DOT__gfx_mem__DOT__mem
                               [(0x0000ffffU & (VL_MULS_III(32, (IData)(0x00000140U), __Vfunc_test_gtext__DOT__gfx_pixel__42__y) 
                                                + __Vfunc_test_gtext__DOT__gfx_pixel__42__x))]
                                : 0U);
                    }(), (IData)(__Vfunc_test_gtext__DOT__gfx_pixel__42__Vfuncout))))) {
        vlSelfRef.test_gtext__DOT__unnamedblk4__DOT__match = 0U;
    }
    if (((~ (IData)(vlSelfRef.test_gtext__DOT__unnamedblk4__DOT__unnamedblk5__DOT__unnamedblk6__DOT__unnamedblk7__DOT__unnamedblk8__DOT__expected)) 
         & (0U != ([&]() {
                        __Vfunc_test_gtext__DOT__gfx_pixel__43__y = 7U;
                        __Vfunc_test_gtext__DOT__gfx_pixel__43__x = 1U;
                        __Vfunc_test_gtext__DOT__gfx_pixel__43__Vfuncout = 0;
                        __Vfunc_test_gtext__DOT__gfx_pixel__43__Vfuncout 
                            = ((0xf9ffU >= (0x0000ffffU 
                                            & (VL_MULS_III(32, (IData)(0x00000140U), __Vfunc_test_gtext__DOT__gfx_pixel__43__y) 
                                               + __Vfunc_test_gtext__DOT__gfx_pixel__43__x)))
                                ? vlSelfRef.test_gtext__DOT__dut__DOT__gfx_inst__DOT__gfx_mem__DOT__mem
                               [(0x0000ffffU & (VL_MULS_III(32, (IData)(0x00000140U), __Vfunc_test_gtext__DOT__gfx_pixel__43__y) 
                                                + __Vfunc_test_gtext__DOT__gfx_pixel__43__x))]
                                : 0U);
                    }(), (IData)(__Vfunc_test_gtext__DOT__gfx_pixel__43__Vfuncout))))) {
        vlSelfRef.test_gtext__DOT__unnamedblk4__DOT__match = 0U;
    }
    vlSelfRef.test_gtext__DOT__unnamedblk4__DOT__unnamedblk5__DOT__unnamedblk6__DOT__unnamedblk7__DOT__unnamedblk8__DOT__expected 
        = (1U & ((IData)(vlSelfRef.test_gtext__DOT__unnamedblk4__DOT__unnamedblk5__DOT__unnamedblk6__DOT__fb) 
                 >> 5U));
    if (((IData)(vlSelfRef.test_gtext__DOT__unnamedblk4__DOT__unnamedblk5__DOT__unnamedblk6__DOT__unnamedblk7__DOT__unnamedblk8__DOT__expected) 
         & (1U != ([&]() {
                        __Vfunc_test_gtext__DOT__gfx_pixel__42__y = 7U;
                        __Vfunc_test_gtext__DOT__gfx_pixel__42__x = 2U;
                        __Vfunc_test_gtext__DOT__gfx_pixel__42__Vfuncout = 0;
                        __Vfunc_test_gtext__DOT__gfx_pixel__42__Vfuncout 
                            = ((0xf9ffU >= (0x0000ffffU 
                                            & (VL_MULS_III(32, (IData)(0x00000140U), __Vfunc_test_gtext__DOT__gfx_pixel__42__y) 
                                               + __Vfunc_test_gtext__DOT__gfx_pixel__42__x)))
                                ? vlSelfRef.test_gtext__DOT__dut__DOT__gfx_inst__DOT__gfx_mem__DOT__mem
                               [(0x0000ffffU & (VL_MULS_III(32, (IData)(0x00000140U), __Vfunc_test_gtext__DOT__gfx_pixel__42__y) 
                                                + __Vfunc_test_gtext__DOT__gfx_pixel__42__x))]
                                : 0U);
                    }(), (IData)(__Vfunc_test_gtext__DOT__gfx_pixel__42__Vfuncout))))) {
        vlSelfRef.test_gtext__DOT__unnamedblk4__DOT__match = 0U;
    }
    if (((~ (IData)(vlSelfRef.test_gtext__DOT__unnamedblk4__DOT__unnamedblk5__DOT__unnamedblk6__DOT__unnamedblk7__DOT__unnamedblk8__DOT__expected)) 
         & (0U != ([&]() {
                        __Vfunc_test_gtext__DOT__gfx_pixel__43__y = 7U;
                        __Vfunc_test_gtext__DOT__gfx_pixel__43__x = 2U;
                        __Vfunc_test_gtext__DOT__gfx_pixel__43__Vfuncout = 0;
                        __Vfunc_test_gtext__DOT__gfx_pixel__43__Vfuncout 
                            = ((0xf9ffU >= (0x0000ffffU 
                                            & (VL_MULS_III(32, (IData)(0x00000140U), __Vfunc_test_gtext__DOT__gfx_pixel__43__y) 
                                               + __Vfunc_test_gtext__DOT__gfx_pixel__43__x)))
                                ? vlSelfRef.test_gtext__DOT__dut__DOT__gfx_inst__DOT__gfx_mem__DOT__mem
                               [(0x0000ffffU & (VL_MULS_III(32, (IData)(0x00000140U), __Vfunc_test_gtext__DOT__gfx_pixel__43__y) 
                                                + __Vfunc_test_gtext__DOT__gfx_pixel__43__x))]
                                : 0U);
                    }(), (IData)(__Vfunc_test_gtext__DOT__gfx_pixel__43__Vfuncout))))) {
        vlSelfRef.test_gtext__DOT__unnamedblk4__DOT__match = 0U;
    }
    vlSelfRef.test_gtext__DOT__unnamedblk4__DOT__unnamedblk5__DOT__unnamedblk6__DOT__unnamedblk7__DOT__unnamedblk8__DOT__expected 
        = (1U & ((IData)(vlSelfRef.test_gtext__DOT__unnamedblk4__DOT__unnamedblk5__DOT__unnamedblk6__DOT__fb) 
                 >> 4U));
    if (((IData)(vlSelfRef.test_gtext__DOT__unnamedblk4__DOT__unnamedblk5__DOT__unnamedblk6__DOT__unnamedblk7__DOT__unnamedblk8__DOT__expected) 
         & (1U != ([&]() {
                        __Vfunc_test_gtext__DOT__gfx_pixel__42__y = 7U;
                        __Vfunc_test_gtext__DOT__gfx_pixel__42__x = 3U;
                        __Vfunc_test_gtext__DOT__gfx_pixel__42__Vfuncout = 0;
                        __Vfunc_test_gtext__DOT__gfx_pixel__42__Vfuncout 
                            = ((0xf9ffU >= (0x0000ffffU 
                                            & (VL_MULS_III(32, (IData)(0x00000140U), __Vfunc_test_gtext__DOT__gfx_pixel__42__y) 
                                               + __Vfunc_test_gtext__DOT__gfx_pixel__42__x)))
                                ? vlSelfRef.test_gtext__DOT__dut__DOT__gfx_inst__DOT__gfx_mem__DOT__mem
                               [(0x0000ffffU & (VL_MULS_III(32, (IData)(0x00000140U), __Vfunc_test_gtext__DOT__gfx_pixel__42__y) 
                                                + __Vfunc_test_gtext__DOT__gfx_pixel__42__x))]
                                : 0U);
                    }(), (IData)(__Vfunc_test_gtext__DOT__gfx_pixel__42__Vfuncout))))) {
        vlSelfRef.test_gtext__DOT__unnamedblk4__DOT__match = 0U;
    }
    if (((~ (IData)(vlSelfRef.test_gtext__DOT__unnamedblk4__DOT__unnamedblk5__DOT__unnamedblk6__DOT__unnamedblk7__DOT__unnamedblk8__DOT__expected)) 
         & (0U != ([&]() {
                        __Vfunc_test_gtext__DOT__gfx_pixel__43__y = 7U;
                        __Vfunc_test_gtext__DOT__gfx_pixel__43__x = 3U;
                        __Vfunc_test_gtext__DOT__gfx_pixel__43__Vfuncout = 0;
                        __Vfunc_test_gtext__DOT__gfx_pixel__43__Vfuncout 
                            = ((0xf9ffU >= (0x0000ffffU 
                                            & (VL_MULS_III(32, (IData)(0x00000140U), __Vfunc_test_gtext__DOT__gfx_pixel__43__y) 
                                               + __Vfunc_test_gtext__DOT__gfx_pixel__43__x)))
                                ? vlSelfRef.test_gtext__DOT__dut__DOT__gfx_inst__DOT__gfx_mem__DOT__mem
                               [(0x0000ffffU & (VL_MULS_III(32, (IData)(0x00000140U), __Vfunc_test_gtext__DOT__gfx_pixel__43__y) 
                                                + __Vfunc_test_gtext__DOT__gfx_pixel__43__x))]
                                : 0U);
                    }(), (IData)(__Vfunc_test_gtext__DOT__gfx_pixel__43__Vfuncout))))) {
        vlSelfRef.test_gtext__DOT__unnamedblk4__DOT__match = 0U;
    }
    vlSelfRef.test_gtext__DOT__unnamedblk4__DOT__unnamedblk5__DOT__unnamedblk6__DOT__unnamedblk7__DOT__unnamedblk8__DOT__expected 
        = (1U & ((IData)(vlSelfRef.test_gtext__DOT__unnamedblk4__DOT__unnamedblk5__DOT__unnamedblk6__DOT__fb) 
                 >> 3U));
    if (((IData)(vlSelfRef.test_gtext__DOT__unnamedblk4__DOT__unnamedblk5__DOT__unnamedblk6__DOT__unnamedblk7__DOT__unnamedblk8__DOT__expected) 
         & (1U != ([&]() {
                        __Vfunc_test_gtext__DOT__gfx_pixel__42__y = 7U;
                        __Vfunc_test_gtext__DOT__gfx_pixel__42__x = 4U;
                        __Vfunc_test_gtext__DOT__gfx_pixel__42__Vfuncout = 0;
                        __Vfunc_test_gtext__DOT__gfx_pixel__42__Vfuncout 
                            = ((0xf9ffU >= (0x0000ffffU 
                                            & (VL_MULS_III(32, (IData)(0x00000140U), __Vfunc_test_gtext__DOT__gfx_pixel__42__y) 
                                               + __Vfunc_test_gtext__DOT__gfx_pixel__42__x)))
                                ? vlSelfRef.test_gtext__DOT__dut__DOT__gfx_inst__DOT__gfx_mem__DOT__mem
                               [(0x0000ffffU & (VL_MULS_III(32, (IData)(0x00000140U), __Vfunc_test_gtext__DOT__gfx_pixel__42__y) 
                                                + __Vfunc_test_gtext__DOT__gfx_pixel__42__x))]
                                : 0U);
                    }(), (IData)(__Vfunc_test_gtext__DOT__gfx_pixel__42__Vfuncout))))) {
        vlSelfRef.test_gtext__DOT__unnamedblk4__DOT__match = 0U;
    }
    if (((~ (IData)(vlSelfRef.test_gtext__DOT__unnamedblk4__DOT__unnamedblk5__DOT__unnamedblk6__DOT__unnamedblk7__DOT__unnamedblk8__DOT__expected)) 
         & (0U != ([&]() {
                        __Vfunc_test_gtext__DOT__gfx_pixel__43__y = 7U;
                        __Vfunc_test_gtext__DOT__gfx_pixel__43__x = 4U;
                        __Vfunc_test_gtext__DOT__gfx_pixel__43__Vfuncout = 0;
                        __Vfunc_test_gtext__DOT__gfx_pixel__43__Vfuncout 
                            = ((0xf9ffU >= (0x0000ffffU 
                                            & (VL_MULS_III(32, (IData)(0x00000140U), __Vfunc_test_gtext__DOT__gfx_pixel__43__y) 
                                               + __Vfunc_test_gtext__DOT__gfx_pixel__43__x)))
                                ? vlSelfRef.test_gtext__DOT__dut__DOT__gfx_inst__DOT__gfx_mem__DOT__mem
                               [(0x0000ffffU & (VL_MULS_III(32, (IData)(0x00000140U), __Vfunc_test_gtext__DOT__gfx_pixel__43__y) 
                                                + __Vfunc_test_gtext__DOT__gfx_pixel__43__x))]
                                : 0U);
                    }(), (IData)(__Vfunc_test_gtext__DOT__gfx_pixel__43__Vfuncout))))) {
        vlSelfRef.test_gtext__DOT__unnamedblk4__DOT__match = 0U;
    }
    vlSelfRef.test_gtext__DOT__unnamedblk4__DOT__unnamedblk5__DOT__unnamedblk6__DOT__unnamedblk7__DOT__unnamedblk8__DOT__expected 
        = (1U & ((IData)(vlSelfRef.test_gtext__DOT__unnamedblk4__DOT__unnamedblk5__DOT__unnamedblk6__DOT__fb) 
                 >> 2U));
    if (((IData)(vlSelfRef.test_gtext__DOT__unnamedblk4__DOT__unnamedblk5__DOT__unnamedblk6__DOT__unnamedblk7__DOT__unnamedblk8__DOT__expected) 
         & (1U != ([&]() {
                        __Vfunc_test_gtext__DOT__gfx_pixel__42__y = 7U;
                        __Vfunc_test_gtext__DOT__gfx_pixel__42__x = 5U;
                        __Vfunc_test_gtext__DOT__gfx_pixel__42__Vfuncout = 0;
                        __Vfunc_test_gtext__DOT__gfx_pixel__42__Vfuncout 
                            = ((0xf9ffU >= (0x0000ffffU 
                                            & (VL_MULS_III(32, (IData)(0x00000140U), __Vfunc_test_gtext__DOT__gfx_pixel__42__y) 
                                               + __Vfunc_test_gtext__DOT__gfx_pixel__42__x)))
                                ? vlSelfRef.test_gtext__DOT__dut__DOT__gfx_inst__DOT__gfx_mem__DOT__mem
                               [(0x0000ffffU & (VL_MULS_III(32, (IData)(0x00000140U), __Vfunc_test_gtext__DOT__gfx_pixel__42__y) 
                                                + __Vfunc_test_gtext__DOT__gfx_pixel__42__x))]
                                : 0U);
                    }(), (IData)(__Vfunc_test_gtext__DOT__gfx_pixel__42__Vfuncout))))) {
        vlSelfRef.test_gtext__DOT__unnamedblk4__DOT__match = 0U;
    }
    if (((~ (IData)(vlSelfRef.test_gtext__DOT__unnamedblk4__DOT__unnamedblk5__DOT__unnamedblk6__DOT__unnamedblk7__DOT__unnamedblk8__DOT__expected)) 
         & (0U != ([&]() {
                        __Vfunc_test_gtext__DOT__gfx_pixel__43__y = 7U;
                        __Vfunc_test_gtext__DOT__gfx_pixel__43__x = 5U;
                        __Vfunc_test_gtext__DOT__gfx_pixel__43__Vfuncout = 0;
                        __Vfunc_test_gtext__DOT__gfx_pixel__43__Vfuncout 
                            = ((0xf9ffU >= (0x0000ffffU 
                                            & (VL_MULS_III(32, (IData)(0x00000140U), __Vfunc_test_gtext__DOT__gfx_pixel__43__y) 
                                               + __Vfunc_test_gtext__DOT__gfx_pixel__43__x)))
                                ? vlSelfRef.test_gtext__DOT__dut__DOT__gfx_inst__DOT__gfx_mem__DOT__mem
                               [(0x0000ffffU & (VL_MULS_III(32, (IData)(0x00000140U), __Vfunc_test_gtext__DOT__gfx_pixel__43__y) 
                                                + __Vfunc_test_gtext__DOT__gfx_pixel__43__x))]
                                : 0U);
                    }(), (IData)(__Vfunc_test_gtext__DOT__gfx_pixel__43__Vfuncout))))) {
        vlSelfRef.test_gtext__DOT__unnamedblk4__DOT__match = 0U;
    }
    vlSelfRef.test_gtext__DOT__unnamedblk4__DOT__unnamedblk5__DOT__unnamedblk6__DOT__unnamedblk7__DOT__unnamedblk8__DOT__expected 
        = (1U & ((IData)(vlSelfRef.test_gtext__DOT__unnamedblk4__DOT__unnamedblk5__DOT__unnamedblk6__DOT__fb) 
                 >> 1U));
    if (((IData)(vlSelfRef.test_gtext__DOT__unnamedblk4__DOT__unnamedblk5__DOT__unnamedblk6__DOT__unnamedblk7__DOT__unnamedblk8__DOT__expected) 
         & (1U != ([&]() {
                        __Vfunc_test_gtext__DOT__gfx_pixel__42__y = 7U;
                        __Vfunc_test_gtext__DOT__gfx_pixel__42__x = 6U;
                        __Vfunc_test_gtext__DOT__gfx_pixel__42__Vfuncout = 0;
                        __Vfunc_test_gtext__DOT__gfx_pixel__42__Vfuncout 
                            = ((0xf9ffU >= (0x0000ffffU 
                                            & (VL_MULS_III(32, (IData)(0x00000140U), __Vfunc_test_gtext__DOT__gfx_pixel__42__y) 
                                               + __Vfunc_test_gtext__DOT__gfx_pixel__42__x)))
                                ? vlSelfRef.test_gtext__DOT__dut__DOT__gfx_inst__DOT__gfx_mem__DOT__mem
                               [(0x0000ffffU & (VL_MULS_III(32, (IData)(0x00000140U), __Vfunc_test_gtext__DOT__gfx_pixel__42__y) 
                                                + __Vfunc_test_gtext__DOT__gfx_pixel__42__x))]
                                : 0U);
                    }(), (IData)(__Vfunc_test_gtext__DOT__gfx_pixel__42__Vfuncout))))) {
        vlSelfRef.test_gtext__DOT__unnamedblk4__DOT__match = 0U;
    }
    if (((~ (IData)(vlSelfRef.test_gtext__DOT__unnamedblk4__DOT__unnamedblk5__DOT__unnamedblk6__DOT__unnamedblk7__DOT__unnamedblk8__DOT__expected)) 
         & (0U != ([&]() {
                        __Vfunc_test_gtext__DOT__gfx_pixel__43__y = 7U;
                        __Vfunc_test_gtext__DOT__gfx_pixel__43__x = 6U;
                        __Vfunc_test_gtext__DOT__gfx_pixel__43__Vfuncout = 0;
                        __Vfunc_test_gtext__DOT__gfx_pixel__43__Vfuncout 
                            = ((0xf9ffU >= (0x0000ffffU 
                                            & (VL_MULS_III(32, (IData)(0x00000140U), __Vfunc_test_gtext__DOT__gfx_pixel__43__y) 
                                               + __Vfunc_test_gtext__DOT__gfx_pixel__43__x)))
                                ? vlSelfRef.test_gtext__DOT__dut__DOT__gfx_inst__DOT__gfx_mem__DOT__mem
                               [(0x0000ffffU & (VL_MULS_III(32, (IData)(0x00000140U), __Vfunc_test_gtext__DOT__gfx_pixel__43__y) 
                                                + __Vfunc_test_gtext__DOT__gfx_pixel__43__x))]
                                : 0U);
                    }(), (IData)(__Vfunc_test_gtext__DOT__gfx_pixel__43__Vfuncout))))) {
        vlSelfRef.test_gtext__DOT__unnamedblk4__DOT__match = 0U;
    }
    vlSelfRef.test_gtext__DOT__unnamedblk4__DOT__unnamedblk5__DOT__unnamedblk6__DOT__unnamedblk7__DOT__unnamedblk8__DOT__expected 
        = (1U & (IData)(vlSelfRef.test_gtext__DOT__unnamedblk4__DOT__unnamedblk5__DOT__unnamedblk6__DOT__fb));
    if (((IData)(vlSelfRef.test_gtext__DOT__unnamedblk4__DOT__unnamedblk5__DOT__unnamedblk6__DOT__unnamedblk7__DOT__unnamedblk8__DOT__expected) 
         & (1U != ([&]() {
                        __Vfunc_test_gtext__DOT__gfx_pixel__42__y = 7U;
                        __Vfunc_test_gtext__DOT__gfx_pixel__42__x = 7U;
                        __Vfunc_test_gtext__DOT__gfx_pixel__42__Vfuncout = 0;
                        __Vfunc_test_gtext__DOT__gfx_pixel__42__Vfuncout 
                            = ((0xf9ffU >= (0x0000ffffU 
                                            & (VL_MULS_III(32, (IData)(0x00000140U), __Vfunc_test_gtext__DOT__gfx_pixel__42__y) 
                                               + __Vfunc_test_gtext__DOT__gfx_pixel__42__x)))
                                ? vlSelfRef.test_gtext__DOT__dut__DOT__gfx_inst__DOT__gfx_mem__DOT__mem
                               [(0x0000ffffU & (VL_MULS_III(32, (IData)(0x00000140U), __Vfunc_test_gtext__DOT__gfx_pixel__42__y) 
                                                + __Vfunc_test_gtext__DOT__gfx_pixel__42__x))]
                                : 0U);
                    }(), (IData)(__Vfunc_test_gtext__DOT__gfx_pixel__42__Vfuncout))))) {
        vlSelfRef.test_gtext__DOT__unnamedblk4__DOT__match = 0U;
    }
    if (((~ (IData)(vlSelfRef.test_gtext__DOT__unnamedblk4__DOT__unnamedblk5__DOT__unnamedblk6__DOT__unnamedblk7__DOT__unnamedblk8__DOT__expected)) 
         & (0U != ([&]() {
                        __Vfunc_test_gtext__DOT__gfx_pixel__43__y = 7U;
                        __Vfunc_test_gtext__DOT__gfx_pixel__43__x = 7U;
                        __Vfunc_test_gtext__DOT__gfx_pixel__43__Vfuncout = 0;
                        __Vfunc_test_gtext__DOT__gfx_pixel__43__Vfuncout 
                            = ((0xf9ffU >= (0x0000ffffU 
                                            & (VL_MULS_III(32, (IData)(0x00000140U), __Vfunc_test_gtext__DOT__gfx_pixel__43__y) 
                                               + __Vfunc_test_gtext__DOT__gfx_pixel__43__x)))
                                ? vlSelfRef.test_gtext__DOT__dut__DOT__gfx_inst__DOT__gfx_mem__DOT__mem
                               [(0x0000ffffU & (VL_MULS_III(32, (IData)(0x00000140U), __Vfunc_test_gtext__DOT__gfx_pixel__43__y) 
                                                + __Vfunc_test_gtext__DOT__gfx_pixel__43__x))]
                                : 0U);
                    }(), (IData)(__Vfunc_test_gtext__DOT__gfx_pixel__43__Vfuncout))))) {
        vlSelfRef.test_gtext__DOT__unnamedblk4__DOT__match = 0U;
    }
    __Vtask_test_gtext__DOT__check__44__condition = 
        (1U == vlSelfRef.test_gtext__DOT__unnamedblk4__DOT__match);
    vlSelfRef.__Vtask_test_gtext__DOT__check__44__name = "'A' pixels match font ROM exactly"s;
    vlSelfRef.test_gtext__DOT__test_num = ((IData)(1U) 
                                           + vlSelfRef.test_gtext__DOT__test_num);
    if (__Vtask_test_gtext__DOT__check__44__condition) {
        VL_WRITEF_NX("  PASS [%0d] %@\n",0,32,vlSelfRef.test_gtext__DOT__test_num,
                     -1,&(vlSelfRef.__Vtask_test_gtext__DOT__check__44__name));
        vlSelfRef.test_gtext__DOT__pass_count = ((IData)(1U) 
                                                 + vlSelfRef.test_gtext__DOT__pass_count);
    } else {
        VL_WRITEF_NX("  FAIL [%0d] %@\n",0,32,vlSelfRef.test_gtext__DOT__test_num,
                     -1,&(vlSelfRef.__Vtask_test_gtext__DOT__check__44__name));
        vlSelfRef.test_gtext__DOT__fail_count = ((IData)(1U) 
                                                 + vlSelfRef.test_gtext__DOT__fail_count);
    }
    VL_WRITEF_NX("Test: Char at offset position\n",0);
    __Vtask_test_gtext__DOT__write_cmd__45__cmd = 7U;
    __Vtask_test_gtext__DOT__write_reg__46__data = __Vtask_test_gtext__DOT__write_cmd__45__cmd;
    __Vtask_test_gtext__DOT__write_reg__46__addr = 0xa010U;
    Vtest_gtext___024root____VbeforeTrig_h8248a36b__0(vlSelf, 
                                                      "@(posedge test_gtext.clk)");
    co_await vlSelfRef.__VtrigSched_h8248a36b__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge test_gtext.clk)", 
                                                         "test_gtext.sv", 
                                                         49);
    vlSelfRef.test_gtext__DOT__cpu_addr = __Vtask_test_gtext__DOT__write_reg__46__addr;
    vlSelfRef.test_gtext__DOT__cpu_wdata = __Vtask_test_gtext__DOT__write_reg__46__data;
    vlSelfRef.test_gtext__DOT__cpu_we = 1U;
    vlSelfRef.test_gtext__DOT__cpu_ce = 1U;
    Vtest_gtext___024root____VbeforeTrig_h8248a36b__0(vlSelf, 
                                                      "@(posedge test_gtext.clk)");
    co_await vlSelfRef.__VtrigSched_h8248a36b__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge test_gtext.clk)", 
                                                         "test_gtext.sv", 
                                                         54);
    vlSelfRef.test_gtext__DOT__cpu_we = 0U;
    vlSelfRef.test_gtext__DOT__cpu_ce = 0U;
    __Vtask_test_gtext__DOT__wait_cmd_done__47__test_gtext__DOT__unnamedblk1_1__DOT____Vrepeat0 = 0;
    __Vtask_test_gtext__DOT__wait_cmd_done__47__timeout = 0U;
    __Vtask_test_gtext__DOT__wait_cmd_done__47__timeout = 0U;
    __Vtask_test_gtext__DOT__wait_cmd_done__47__test_gtext__DOT__unnamedblk1_1__DOT____Vrepeat0 = 2U;
    while (VL_LTS_III(32, 0U, __Vtask_test_gtext__DOT__wait_cmd_done__47__test_gtext__DOT__unnamedblk1_1__DOT____Vrepeat0)) {
        Vtest_gtext___024root____VbeforeTrig_h8248a36b__0(vlSelf, 
                                                          "@(posedge test_gtext.clk)");
        co_await vlSelfRef.__VtrigSched_h8248a36b__0.trigger(0U, 
                                                             nullptr, 
                                                             "@(posedge test_gtext.clk)", 
                                                             "test_gtext.sv", 
                                                             71);
        __Vtask_test_gtext__DOT__wait_cmd_done__47__test_gtext__DOT__unnamedblk1_1__DOT____Vrepeat0 
            = (__Vtask_test_gtext__DOT__wait_cmd_done__47__test_gtext__DOT__unnamedblk1_1__DOT____Vrepeat0 
               - (IData)(1U));
    }
    while ((((IData)(vlSelfRef.test_gtext__DOT__dut__DOT__cmd_busy) 
             | (IData)(vlSelfRef.test_gtext__DOT__dut__DOT__artist_busy)) 
            & VL_GTS_III(32, 0x0007a120U, __Vtask_test_gtext__DOT__wait_cmd_done__47__timeout))) {
        Vtest_gtext___024root____VbeforeTrig_h8248a36b__0(vlSelf, 
                                                          "@(posedge test_gtext.clk)");
        co_await vlSelfRef.__VtrigSched_h8248a36b__0.trigger(0U, 
                                                             nullptr, 
                                                             "@(posedge test_gtext.clk)", 
                                                             "test_gtext.sv", 
                                                             73);
        __Vtask_test_gtext__DOT__wait_cmd_done__47__timeout 
            = ((IData)(1U) + __Vtask_test_gtext__DOT__wait_cmd_done__47__timeout);
    }
    vlSelfRef.__Vtask_test_gtext__DOT__set_fio_string__48__s = "X"s;
    __Vtask_test_gtext__DOT__set_fio_string__48__i = 0U;
    __Vtask_test_gtext__DOT__write_reg__49__data = 
        (0x000000ffU & VL_LEN_IN(vlSelfRef.__Vtask_test_gtext__DOT__set_fio_string__48__s));
    __Vtask_test_gtext__DOT__write_reg__49__addr = 0xb9a3U;
    Vtest_gtext___024root____VbeforeTrig_h8248a36b__0(vlSelf, 
                                                      "@(posedge test_gtext.clk)");
    co_await vlSelfRef.__VtrigSched_h8248a36b__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge test_gtext.clk)", 
                                                         "test_gtext.sv", 
                                                         49);
    vlSelfRef.test_gtext__DOT__cpu_addr = __Vtask_test_gtext__DOT__write_reg__49__addr;
    vlSelfRef.test_gtext__DOT__cpu_wdata = __Vtask_test_gtext__DOT__write_reg__49__data;
    vlSelfRef.test_gtext__DOT__cpu_we = 1U;
    vlSelfRef.test_gtext__DOT__cpu_ce = 1U;
    Vtest_gtext___024root____VbeforeTrig_h8248a36b__0(vlSelf, 
                                                      "@(posedge test_gtext.clk)");
    co_await vlSelfRef.__VtrigSched_h8248a36b__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge test_gtext.clk)", 
                                                         "test_gtext.sv", 
                                                         54);
    vlSelfRef.test_gtext__DOT__cpu_we = 0U;
    vlSelfRef.test_gtext__DOT__cpu_ce = 0U;
    __Vtask_test_gtext__DOT__set_fio_string__48__i = 0U;
    while (VL_LTS_III(32, __Vtask_test_gtext__DOT__set_fio_string__48__i, 
                      VL_LEN_IN(vlSelfRef.__Vtask_test_gtext__DOT__set_fio_string__48__s))) {
        __Vtask_test_gtext__DOT__write_reg__50__data 
            = VL_GETC_N(vlSelfRef.__Vtask_test_gtext__DOT__set_fio_string__48__s,__Vtask_test_gtext__DOT__set_fio_string__48__i);
        __Vtask_test_gtext__DOT__write_reg__50__addr 
            = (0x0000ffffU & ((IData)(0xb9b0U) + __Vtask_test_gtext__DOT__set_fio_string__48__i));
        Vtest_gtext___024root____VbeforeTrig_h8248a36b__0(vlSelf, 
                                                          "@(posedge test_gtext.clk)");
        co_await vlSelfRef.__VtrigSched_h8248a36b__0.trigger(0U, 
                                                             nullptr, 
                                                             "@(posedge test_gtext.clk)", 
                                                             "test_gtext.sv", 
                                                             49);
        vlSelfRef.test_gtext__DOT__cpu_addr = __Vtask_test_gtext__DOT__write_reg__50__addr;
        vlSelfRef.test_gtext__DOT__cpu_wdata = __Vtask_test_gtext__DOT__write_reg__50__data;
        vlSelfRef.test_gtext__DOT__cpu_we = 1U;
        vlSelfRef.test_gtext__DOT__cpu_ce = 1U;
        Vtest_gtext___024root____VbeforeTrig_h8248a36b__0(vlSelf, 
                                                          "@(posedge test_gtext.clk)");
        co_await vlSelfRef.__VtrigSched_h8248a36b__0.trigger(0U, 
                                                             nullptr, 
                                                             "@(posedge test_gtext.clk)", 
                                                             "test_gtext.sv", 
                                                             54);
        vlSelfRef.test_gtext__DOT__cpu_we = 0U;
        vlSelfRef.test_gtext__DOT__cpu_ce = 0U;
        __Vtask_test_gtext__DOT__set_fio_string__48__i 
            = ((IData)(1U) + __Vtask_test_gtext__DOT__set_fio_string__48__i);
    }
    __Vtask_test_gtext__DOT__write_param__51__data = 0x64U;
    __Vtask_test_gtext__DOT__write_param__51__idx = 0U;
    __Vtask_test_gtext__DOT__write_reg__52__data = __Vtask_test_gtext__DOT__write_param__51__data;
    __Vtask_test_gtext__DOT__write_reg__52__addr = 
        (0x0000ffffU & ((IData)(0xa011U) + __Vtask_test_gtext__DOT__write_param__51__idx));
    Vtest_gtext___024root____VbeforeTrig_h8248a36b__0(vlSelf, 
                                                      "@(posedge test_gtext.clk)");
    co_await vlSelfRef.__VtrigSched_h8248a36b__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge test_gtext.clk)", 
                                                         "test_gtext.sv", 
                                                         49);
    vlSelfRef.test_gtext__DOT__cpu_addr = __Vtask_test_gtext__DOT__write_reg__52__addr;
    vlSelfRef.test_gtext__DOT__cpu_wdata = __Vtask_test_gtext__DOT__write_reg__52__data;
    vlSelfRef.test_gtext__DOT__cpu_we = 1U;
    vlSelfRef.test_gtext__DOT__cpu_ce = 1U;
    Vtest_gtext___024root____VbeforeTrig_h8248a36b__0(vlSelf, 
                                                      "@(posedge test_gtext.clk)");
    co_await vlSelfRef.__VtrigSched_h8248a36b__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge test_gtext.clk)", 
                                                         "test_gtext.sv", 
                                                         54);
    vlSelfRef.test_gtext__DOT__cpu_we = 0U;
    vlSelfRef.test_gtext__DOT__cpu_ce = 0U;
    __Vtask_test_gtext__DOT__write_param__53__data = 0U;
    __Vtask_test_gtext__DOT__write_param__53__idx = 1U;
    __Vtask_test_gtext__DOT__write_reg__54__data = __Vtask_test_gtext__DOT__write_param__53__data;
    __Vtask_test_gtext__DOT__write_reg__54__addr = 
        (0x0000ffffU & ((IData)(0xa011U) + __Vtask_test_gtext__DOT__write_param__53__idx));
    Vtest_gtext___024root____VbeforeTrig_h8248a36b__0(vlSelf, 
                                                      "@(posedge test_gtext.clk)");
    co_await vlSelfRef.__VtrigSched_h8248a36b__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge test_gtext.clk)", 
                                                         "test_gtext.sv", 
                                                         49);
    vlSelfRef.test_gtext__DOT__cpu_addr = __Vtask_test_gtext__DOT__write_reg__54__addr;
    vlSelfRef.test_gtext__DOT__cpu_wdata = __Vtask_test_gtext__DOT__write_reg__54__data;
    vlSelfRef.test_gtext__DOT__cpu_we = 1U;
    vlSelfRef.test_gtext__DOT__cpu_ce = 1U;
    Vtest_gtext___024root____VbeforeTrig_h8248a36b__0(vlSelf, 
                                                      "@(posedge test_gtext.clk)");
    co_await vlSelfRef.__VtrigSched_h8248a36b__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge test_gtext.clk)", 
                                                         "test_gtext.sv", 
                                                         54);
    vlSelfRef.test_gtext__DOT__cpu_we = 0U;
    vlSelfRef.test_gtext__DOT__cpu_ce = 0U;
    __Vtask_test_gtext__DOT__write_param__55__data = 0x32U;
    __Vtask_test_gtext__DOT__write_param__55__idx = 2U;
    __Vtask_test_gtext__DOT__write_reg__56__data = __Vtask_test_gtext__DOT__write_param__55__data;
    __Vtask_test_gtext__DOT__write_reg__56__addr = 
        (0x0000ffffU & ((IData)(0xa011U) + __Vtask_test_gtext__DOT__write_param__55__idx));
    Vtest_gtext___024root____VbeforeTrig_h8248a36b__0(vlSelf, 
                                                      "@(posedge test_gtext.clk)");
    co_await vlSelfRef.__VtrigSched_h8248a36b__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge test_gtext.clk)", 
                                                         "test_gtext.sv", 
                                                         49);
    vlSelfRef.test_gtext__DOT__cpu_addr = __Vtask_test_gtext__DOT__write_reg__56__addr;
    vlSelfRef.test_gtext__DOT__cpu_wdata = __Vtask_test_gtext__DOT__write_reg__56__data;
    vlSelfRef.test_gtext__DOT__cpu_we = 1U;
    vlSelfRef.test_gtext__DOT__cpu_ce = 1U;
    Vtest_gtext___024root____VbeforeTrig_h8248a36b__0(vlSelf, 
                                                      "@(posedge test_gtext.clk)");
    co_await vlSelfRef.__VtrigSched_h8248a36b__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge test_gtext.clk)", 
                                                         "test_gtext.sv", 
                                                         54);
    vlSelfRef.test_gtext__DOT__cpu_we = 0U;
    vlSelfRef.test_gtext__DOT__cpu_ce = 0U;
    __Vtask_test_gtext__DOT__write_param__57__data = 0U;
    __Vtask_test_gtext__DOT__write_param__57__idx = 3U;
    __Vtask_test_gtext__DOT__write_reg__58__data = __Vtask_test_gtext__DOT__write_param__57__data;
    __Vtask_test_gtext__DOT__write_reg__58__addr = 
        (0x0000ffffU & ((IData)(0xa011U) + __Vtask_test_gtext__DOT__write_param__57__idx));
    Vtest_gtext___024root____VbeforeTrig_h8248a36b__0(vlSelf, 
                                                      "@(posedge test_gtext.clk)");
    co_await vlSelfRef.__VtrigSched_h8248a36b__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge test_gtext.clk)", 
                                                         "test_gtext.sv", 
                                                         49);
    vlSelfRef.test_gtext__DOT__cpu_addr = __Vtask_test_gtext__DOT__write_reg__58__addr;
    vlSelfRef.test_gtext__DOT__cpu_wdata = __Vtask_test_gtext__DOT__write_reg__58__data;
    vlSelfRef.test_gtext__DOT__cpu_we = 1U;
    vlSelfRef.test_gtext__DOT__cpu_ce = 1U;
    Vtest_gtext___024root____VbeforeTrig_h8248a36b__0(vlSelf, 
                                                      "@(posedge test_gtext.clk)");
    co_await vlSelfRef.__VtrigSched_h8248a36b__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge test_gtext.clk)", 
                                                         "test_gtext.sv", 
                                                         54);
    vlSelfRef.test_gtext__DOT__cpu_we = 0U;
    vlSelfRef.test_gtext__DOT__cpu_ce = 0U;
    __Vtask_test_gtext__DOT__write_param__59__data = 0U;
    __Vtask_test_gtext__DOT__write_param__59__idx = 4U;
    __Vtask_test_gtext__DOT__write_reg__60__data = __Vtask_test_gtext__DOT__write_param__59__data;
    __Vtask_test_gtext__DOT__write_reg__60__addr = 
        (0x0000ffffU & ((IData)(0xa011U) + __Vtask_test_gtext__DOT__write_param__59__idx));
    Vtest_gtext___024root____VbeforeTrig_h8248a36b__0(vlSelf, 
                                                      "@(posedge test_gtext.clk)");
    co_await vlSelfRef.__VtrigSched_h8248a36b__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge test_gtext.clk)", 
                                                         "test_gtext.sv", 
                                                         49);
    vlSelfRef.test_gtext__DOT__cpu_addr = __Vtask_test_gtext__DOT__write_reg__60__addr;
    vlSelfRef.test_gtext__DOT__cpu_wdata = __Vtask_test_gtext__DOT__write_reg__60__data;
    vlSelfRef.test_gtext__DOT__cpu_we = 1U;
    vlSelfRef.test_gtext__DOT__cpu_ce = 1U;
    Vtest_gtext___024root____VbeforeTrig_h8248a36b__0(vlSelf, 
                                                      "@(posedge test_gtext.clk)");
    co_await vlSelfRef.__VtrigSched_h8248a36b__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge test_gtext.clk)", 
                                                         "test_gtext.sv", 
                                                         54);
    vlSelfRef.test_gtext__DOT__cpu_we = 0U;
    vlSelfRef.test_gtext__DOT__cpu_ce = 0U;
    __Vtask_test_gtext__DOT__write_param__61__data = 1U;
    __Vtask_test_gtext__DOT__write_param__61__idx = 5U;
    __Vtask_test_gtext__DOT__write_reg__62__data = __Vtask_test_gtext__DOT__write_param__61__data;
    __Vtask_test_gtext__DOT__write_reg__62__addr = 
        (0x0000ffffU & ((IData)(0xa011U) + __Vtask_test_gtext__DOT__write_param__61__idx));
    Vtest_gtext___024root____VbeforeTrig_h8248a36b__0(vlSelf, 
                                                      "@(posedge test_gtext.clk)");
    co_await vlSelfRef.__VtrigSched_h8248a36b__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge test_gtext.clk)", 
                                                         "test_gtext.sv", 
                                                         49);
    vlSelfRef.test_gtext__DOT__cpu_addr = __Vtask_test_gtext__DOT__write_reg__62__addr;
    vlSelfRef.test_gtext__DOT__cpu_wdata = __Vtask_test_gtext__DOT__write_reg__62__data;
    vlSelfRef.test_gtext__DOT__cpu_we = 1U;
    vlSelfRef.test_gtext__DOT__cpu_ce = 1U;
    Vtest_gtext___024root____VbeforeTrig_h8248a36b__0(vlSelf, 
                                                      "@(posedge test_gtext.clk)");
    co_await vlSelfRef.__VtrigSched_h8248a36b__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge test_gtext.clk)", 
                                                         "test_gtext.sv", 
                                                         54);
    vlSelfRef.test_gtext__DOT__cpu_we = 0U;
    vlSelfRef.test_gtext__DOT__cpu_ce = 0U;
    __Vtask_test_gtext__DOT__write_cmd__63__cmd = 0x0aU;
    __Vtask_test_gtext__DOT__write_reg__64__data = __Vtask_test_gtext__DOT__write_cmd__63__cmd;
    __Vtask_test_gtext__DOT__write_reg__64__addr = 0xa010U;
    Vtest_gtext___024root____VbeforeTrig_h8248a36b__0(vlSelf, 
                                                      "@(posedge test_gtext.clk)");
    co_await vlSelfRef.__VtrigSched_h8248a36b__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge test_gtext.clk)", 
                                                         "test_gtext.sv", 
                                                         49);
    vlSelfRef.test_gtext__DOT__cpu_addr = __Vtask_test_gtext__DOT__write_reg__64__addr;
    vlSelfRef.test_gtext__DOT__cpu_wdata = __Vtask_test_gtext__DOT__write_reg__64__data;
    vlSelfRef.test_gtext__DOT__cpu_we = 1U;
    vlSelfRef.test_gtext__DOT__cpu_ce = 1U;
    Vtest_gtext___024root____VbeforeTrig_h8248a36b__0(vlSelf, 
                                                      "@(posedge test_gtext.clk)");
    co_await vlSelfRef.__VtrigSched_h8248a36b__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge test_gtext.clk)", 
                                                         "test_gtext.sv", 
                                                         54);
    vlSelfRef.test_gtext__DOT__cpu_we = 0U;
    vlSelfRef.test_gtext__DOT__cpu_ce = 0U;
    __Vtask_test_gtext__DOT__wait_cmd_done__65__test_gtext__DOT__unnamedblk1_1__DOT____Vrepeat0 = 0;
    __Vtask_test_gtext__DOT__wait_cmd_done__65__timeout = 0U;
    __Vtask_test_gtext__DOT__wait_cmd_done__65__timeout = 0U;
    __Vtask_test_gtext__DOT__wait_cmd_done__65__test_gtext__DOT__unnamedblk1_1__DOT____Vrepeat0 = 2U;
    while (VL_LTS_III(32, 0U, __Vtask_test_gtext__DOT__wait_cmd_done__65__test_gtext__DOT__unnamedblk1_1__DOT____Vrepeat0)) {
        Vtest_gtext___024root____VbeforeTrig_h8248a36b__0(vlSelf, 
                                                          "@(posedge test_gtext.clk)");
        co_await vlSelfRef.__VtrigSched_h8248a36b__0.trigger(0U, 
                                                             nullptr, 
                                                             "@(posedge test_gtext.clk)", 
                                                             "test_gtext.sv", 
                                                             71);
        __Vtask_test_gtext__DOT__wait_cmd_done__65__test_gtext__DOT__unnamedblk1_1__DOT____Vrepeat0 
            = (__Vtask_test_gtext__DOT__wait_cmd_done__65__test_gtext__DOT__unnamedblk1_1__DOT____Vrepeat0 
               - (IData)(1U));
    }
    while ((((IData)(vlSelfRef.test_gtext__DOT__dut__DOT__cmd_busy) 
             | (IData)(vlSelfRef.test_gtext__DOT__dut__DOT__artist_busy)) 
            & VL_GTS_III(32, 0x0007a120U, __Vtask_test_gtext__DOT__wait_cmd_done__65__timeout))) {
        Vtest_gtext___024root____VbeforeTrig_h8248a36b__0(vlSelf, 
                                                          "@(posedge test_gtext.clk)");
        co_await vlSelfRef.__VtrigSched_h8248a36b__0.trigger(0U, 
                                                             nullptr, 
                                                             "@(posedge test_gtext.clk)", 
                                                             "test_gtext.sv", 
                                                             73);
        __Vtask_test_gtext__DOT__wait_cmd_done__65__timeout 
            = ((IData)(1U) + __Vtask_test_gtext__DOT__wait_cmd_done__65__timeout);
    }
    __Vtask_test_gtext__DOT__check__66__condition = 
        (0U == ([&]() {
                __Vfunc_test_gtext__DOT__gfx_pixel__67__y = 0U;
                __Vfunc_test_gtext__DOT__gfx_pixel__67__x = 0U;
                __Vfunc_test_gtext__DOT__gfx_pixel__67__Vfuncout = 0;
                __Vfunc_test_gtext__DOT__gfx_pixel__67__Vfuncout 
                    = ((0xf9ffU >= (0x0000ffffU & (
                                                   VL_MULS_III(32, (IData)(0x00000140U), __Vfunc_test_gtext__DOT__gfx_pixel__67__y) 
                                                   + __Vfunc_test_gtext__DOT__gfx_pixel__67__x)))
                        ? vlSelfRef.test_gtext__DOT__dut__DOT__gfx_inst__DOT__gfx_mem__DOT__mem
                       [(0x0000ffffU & (VL_MULS_III(32, (IData)(0x00000140U), __Vfunc_test_gtext__DOT__gfx_pixel__67__y) 
                                        + __Vfunc_test_gtext__DOT__gfx_pixel__67__x))]
                        : 0U);
            }(), (IData)(__Vfunc_test_gtext__DOT__gfx_pixel__67__Vfuncout)));
    vlSelfRef.__Vtask_test_gtext__DOT__check__66__name = "origin pixel clear"s;
    vlSelfRef.test_gtext__DOT__test_num = ((IData)(1U) 
                                           + vlSelfRef.test_gtext__DOT__test_num);
    if (__Vtask_test_gtext__DOT__check__66__condition) {
        VL_WRITEF_NX("  PASS [%0d] %@\n",0,32,vlSelfRef.test_gtext__DOT__test_num,
                     -1,&(vlSelfRef.__Vtask_test_gtext__DOT__check__66__name));
        vlSelfRef.test_gtext__DOT__pass_count = ((IData)(1U) 
                                                 + vlSelfRef.test_gtext__DOT__pass_count);
    } else {
        VL_WRITEF_NX("  FAIL [%0d] %@\n",0,32,vlSelfRef.test_gtext__DOT__test_num,
                     -1,&(vlSelfRef.__Vtask_test_gtext__DOT__check__66__name));
        vlSelfRef.test_gtext__DOT__fail_count = ((IData)(1U) 
                                                 + vlSelfRef.test_gtext__DOT__fail_count);
    }
    __Vtask_test_gtext__DOT__check__68__condition = 
        (0U == ([&]() {
                __Vfunc_test_gtext__DOT__gfx_pixel__69__y = 0x00000019U;
                __Vfunc_test_gtext__DOT__gfx_pixel__69__x = 0x00000032U;
                __Vfunc_test_gtext__DOT__gfx_pixel__69__Vfuncout = 0;
                __Vfunc_test_gtext__DOT__gfx_pixel__69__Vfuncout 
                    = ((0xf9ffU >= (0x0000ffffU & (
                                                   VL_MULS_III(32, (IData)(0x00000140U), __Vfunc_test_gtext__DOT__gfx_pixel__69__y) 
                                                   + __Vfunc_test_gtext__DOT__gfx_pixel__69__x)))
                        ? vlSelfRef.test_gtext__DOT__dut__DOT__gfx_inst__DOT__gfx_mem__DOT__mem
                       [(0x0000ffffU & (VL_MULS_III(32, (IData)(0x00000140U), __Vfunc_test_gtext__DOT__gfx_pixel__69__y) 
                                        + __Vfunc_test_gtext__DOT__gfx_pixel__69__x))]
                        : 0U);
            }(), (IData)(__Vfunc_test_gtext__DOT__gfx_pixel__69__Vfuncout)));
    vlSelfRef.__Vtask_test_gtext__DOT__check__68__name = "pixel at (50,25) clear"s;
    vlSelfRef.test_gtext__DOT__test_num = ((IData)(1U) 
                                           + vlSelfRef.test_gtext__DOT__test_num);
    if (__Vtask_test_gtext__DOT__check__68__condition) {
        VL_WRITEF_NX("  PASS [%0d] %@\n",0,32,vlSelfRef.test_gtext__DOT__test_num,
                     -1,&(vlSelfRef.__Vtask_test_gtext__DOT__check__68__name));
        vlSelfRef.test_gtext__DOT__pass_count = ((IData)(1U) 
                                                 + vlSelfRef.test_gtext__DOT__pass_count);
    } else {
        VL_WRITEF_NX("  FAIL [%0d] %@\n",0,32,vlSelfRef.test_gtext__DOT__test_num,
                     -1,&(vlSelfRef.__Vtask_test_gtext__DOT__check__68__name));
        vlSelfRef.test_gtext__DOT__fail_count = ((IData)(1U) 
                                                 + vlSelfRef.test_gtext__DOT__fail_count);
    }
    vlSelfRef.test_gtext__DOT__unnamedblk9__DOT__pc = 0U;
    if ((0U != ([&]() {
                    __Vfunc_test_gtext__DOT__gfx_pixel__70__y = 0x00000032U;
                    __Vfunc_test_gtext__DOT__gfx_pixel__70__x = 0x00000064U;
                    __Vfunc_test_gtext__DOT__gfx_pixel__70__Vfuncout = 0;
                    __Vfunc_test_gtext__DOT__gfx_pixel__70__Vfuncout 
                        = ((0xf9ffU >= (0x0000ffffU 
                                        & (VL_MULS_III(32, (IData)(0x00000140U), __Vfunc_test_gtext__DOT__gfx_pixel__70__y) 
                                           + __Vfunc_test_gtext__DOT__gfx_pixel__70__x)))
                            ? vlSelfRef.test_gtext__DOT__dut__DOT__gfx_inst__DOT__gfx_mem__DOT__mem
                           [(0x0000ffffU & (VL_MULS_III(32, (IData)(0x00000140U), __Vfunc_test_gtext__DOT__gfx_pixel__70__y) 
                                            + __Vfunc_test_gtext__DOT__gfx_pixel__70__x))]
                            : 0U);
                }(), (IData)(__Vfunc_test_gtext__DOT__gfx_pixel__70__Vfuncout)))) {
        vlSelfRef.test_gtext__DOT__unnamedblk9__DOT__pc 
            = ((IData)(1U) + vlSelfRef.test_gtext__DOT__unnamedblk9__DOT__pc);
    }
    if ((0U != ([&]() {
                    __Vfunc_test_gtext__DOT__gfx_pixel__70__y = 0x00000032U;
                    __Vfunc_test_gtext__DOT__gfx_pixel__70__x = 0x00000065U;
                    __Vfunc_test_gtext__DOT__gfx_pixel__70__Vfuncout = 0;
                    __Vfunc_test_gtext__DOT__gfx_pixel__70__Vfuncout 
                        = ((0xf9ffU >= (0x0000ffffU 
                                        & (VL_MULS_III(32, (IData)(0x00000140U), __Vfunc_test_gtext__DOT__gfx_pixel__70__y) 
                                           + __Vfunc_test_gtext__DOT__gfx_pixel__70__x)))
                            ? vlSelfRef.test_gtext__DOT__dut__DOT__gfx_inst__DOT__gfx_mem__DOT__mem
                           [(0x0000ffffU & (VL_MULS_III(32, (IData)(0x00000140U), __Vfunc_test_gtext__DOT__gfx_pixel__70__y) 
                                            + __Vfunc_test_gtext__DOT__gfx_pixel__70__x))]
                            : 0U);
                }(), (IData)(__Vfunc_test_gtext__DOT__gfx_pixel__70__Vfuncout)))) {
        vlSelfRef.test_gtext__DOT__unnamedblk9__DOT__pc 
            = ((IData)(1U) + vlSelfRef.test_gtext__DOT__unnamedblk9__DOT__pc);
    }
    if ((0U != ([&]() {
                    __Vfunc_test_gtext__DOT__gfx_pixel__70__y = 0x00000032U;
                    __Vfunc_test_gtext__DOT__gfx_pixel__70__x = 0x00000066U;
                    __Vfunc_test_gtext__DOT__gfx_pixel__70__Vfuncout = 0;
                    __Vfunc_test_gtext__DOT__gfx_pixel__70__Vfuncout 
                        = ((0xf9ffU >= (0x0000ffffU 
                                        & (VL_MULS_III(32, (IData)(0x00000140U), __Vfunc_test_gtext__DOT__gfx_pixel__70__y) 
                                           + __Vfunc_test_gtext__DOT__gfx_pixel__70__x)))
                            ? vlSelfRef.test_gtext__DOT__dut__DOT__gfx_inst__DOT__gfx_mem__DOT__mem
                           [(0x0000ffffU & (VL_MULS_III(32, (IData)(0x00000140U), __Vfunc_test_gtext__DOT__gfx_pixel__70__y) 
                                            + __Vfunc_test_gtext__DOT__gfx_pixel__70__x))]
                            : 0U);
                }(), (IData)(__Vfunc_test_gtext__DOT__gfx_pixel__70__Vfuncout)))) {
        vlSelfRef.test_gtext__DOT__unnamedblk9__DOT__pc 
            = ((IData)(1U) + vlSelfRef.test_gtext__DOT__unnamedblk9__DOT__pc);
    }
    if ((0U != ([&]() {
                    __Vfunc_test_gtext__DOT__gfx_pixel__70__y = 0x00000032U;
                    __Vfunc_test_gtext__DOT__gfx_pixel__70__x = 0x00000067U;
                    __Vfunc_test_gtext__DOT__gfx_pixel__70__Vfuncout = 0;
                    __Vfunc_test_gtext__DOT__gfx_pixel__70__Vfuncout 
                        = ((0xf9ffU >= (0x0000ffffU 
                                        & (VL_MULS_III(32, (IData)(0x00000140U), __Vfunc_test_gtext__DOT__gfx_pixel__70__y) 
                                           + __Vfunc_test_gtext__DOT__gfx_pixel__70__x)))
                            ? vlSelfRef.test_gtext__DOT__dut__DOT__gfx_inst__DOT__gfx_mem__DOT__mem
                           [(0x0000ffffU & (VL_MULS_III(32, (IData)(0x00000140U), __Vfunc_test_gtext__DOT__gfx_pixel__70__y) 
                                            + __Vfunc_test_gtext__DOT__gfx_pixel__70__x))]
                            : 0U);
                }(), (IData)(__Vfunc_test_gtext__DOT__gfx_pixel__70__Vfuncout)))) {
        vlSelfRef.test_gtext__DOT__unnamedblk9__DOT__pc 
            = ((IData)(1U) + vlSelfRef.test_gtext__DOT__unnamedblk9__DOT__pc);
    }
    if ((0U != ([&]() {
                    __Vfunc_test_gtext__DOT__gfx_pixel__70__y = 0x00000032U;
                    __Vfunc_test_gtext__DOT__gfx_pixel__70__x = 0x00000068U;
                    __Vfunc_test_gtext__DOT__gfx_pixel__70__Vfuncout = 0;
                    __Vfunc_test_gtext__DOT__gfx_pixel__70__Vfuncout 
                        = ((0xf9ffU >= (0x0000ffffU 
                                        & (VL_MULS_III(32, (IData)(0x00000140U), __Vfunc_test_gtext__DOT__gfx_pixel__70__y) 
                                           + __Vfunc_test_gtext__DOT__gfx_pixel__70__x)))
                            ? vlSelfRef.test_gtext__DOT__dut__DOT__gfx_inst__DOT__gfx_mem__DOT__mem
                           [(0x0000ffffU & (VL_MULS_III(32, (IData)(0x00000140U), __Vfunc_test_gtext__DOT__gfx_pixel__70__y) 
                                            + __Vfunc_test_gtext__DOT__gfx_pixel__70__x))]
                            : 0U);
                }(), (IData)(__Vfunc_test_gtext__DOT__gfx_pixel__70__Vfuncout)))) {
        vlSelfRef.test_gtext__DOT__unnamedblk9__DOT__pc 
            = ((IData)(1U) + vlSelfRef.test_gtext__DOT__unnamedblk9__DOT__pc);
    }
    if ((0U != ([&]() {
                    __Vfunc_test_gtext__DOT__gfx_pixel__70__y = 0x00000032U;
                    __Vfunc_test_gtext__DOT__gfx_pixel__70__x = 0x00000069U;
                    __Vfunc_test_gtext__DOT__gfx_pixel__70__Vfuncout = 0;
                    __Vfunc_test_gtext__DOT__gfx_pixel__70__Vfuncout 
                        = ((0xf9ffU >= (0x0000ffffU 
                                        & (VL_MULS_III(32, (IData)(0x00000140U), __Vfunc_test_gtext__DOT__gfx_pixel__70__y) 
                                           + __Vfunc_test_gtext__DOT__gfx_pixel__70__x)))
                            ? vlSelfRef.test_gtext__DOT__dut__DOT__gfx_inst__DOT__gfx_mem__DOT__mem
                           [(0x0000ffffU & (VL_MULS_III(32, (IData)(0x00000140U), __Vfunc_test_gtext__DOT__gfx_pixel__70__y) 
                                            + __Vfunc_test_gtext__DOT__gfx_pixel__70__x))]
                            : 0U);
                }(), (IData)(__Vfunc_test_gtext__DOT__gfx_pixel__70__Vfuncout)))) {
        vlSelfRef.test_gtext__DOT__unnamedblk9__DOT__pc 
            = ((IData)(1U) + vlSelfRef.test_gtext__DOT__unnamedblk9__DOT__pc);
    }
    if ((0U != ([&]() {
                    __Vfunc_test_gtext__DOT__gfx_pixel__70__y = 0x00000032U;
                    __Vfunc_test_gtext__DOT__gfx_pixel__70__x = 0x0000006aU;
                    __Vfunc_test_gtext__DOT__gfx_pixel__70__Vfuncout = 0;
                    __Vfunc_test_gtext__DOT__gfx_pixel__70__Vfuncout 
                        = ((0xf9ffU >= (0x0000ffffU 
                                        & (VL_MULS_III(32, (IData)(0x00000140U), __Vfunc_test_gtext__DOT__gfx_pixel__70__y) 
                                           + __Vfunc_test_gtext__DOT__gfx_pixel__70__x)))
                            ? vlSelfRef.test_gtext__DOT__dut__DOT__gfx_inst__DOT__gfx_mem__DOT__mem
                           [(0x0000ffffU & (VL_MULS_III(32, (IData)(0x00000140U), __Vfunc_test_gtext__DOT__gfx_pixel__70__y) 
                                            + __Vfunc_test_gtext__DOT__gfx_pixel__70__x))]
                            : 0U);
                }(), (IData)(__Vfunc_test_gtext__DOT__gfx_pixel__70__Vfuncout)))) {
        vlSelfRef.test_gtext__DOT__unnamedblk9__DOT__pc 
            = ((IData)(1U) + vlSelfRef.test_gtext__DOT__unnamedblk9__DOT__pc);
    }
    if ((0U != ([&]() {
                    __Vfunc_test_gtext__DOT__gfx_pixel__70__y = 0x00000032U;
                    __Vfunc_test_gtext__DOT__gfx_pixel__70__x = 0x0000006bU;
                    __Vfunc_test_gtext__DOT__gfx_pixel__70__Vfuncout = 0;
                    __Vfunc_test_gtext__DOT__gfx_pixel__70__Vfuncout 
                        = ((0xf9ffU >= (0x0000ffffU 
                                        & (VL_MULS_III(32, (IData)(0x00000140U), __Vfunc_test_gtext__DOT__gfx_pixel__70__y) 
                                           + __Vfunc_test_gtext__DOT__gfx_pixel__70__x)))
                            ? vlSelfRef.test_gtext__DOT__dut__DOT__gfx_inst__DOT__gfx_mem__DOT__mem
                           [(0x0000ffffU & (VL_MULS_III(32, (IData)(0x00000140U), __Vfunc_test_gtext__DOT__gfx_pixel__70__y) 
                                            + __Vfunc_test_gtext__DOT__gfx_pixel__70__x))]
                            : 0U);
                }(), (IData)(__Vfunc_test_gtext__DOT__gfx_pixel__70__Vfuncout)))) {
        vlSelfRef.test_gtext__DOT__unnamedblk9__DOT__pc 
            = ((IData)(1U) + vlSelfRef.test_gtext__DOT__unnamedblk9__DOT__pc);
    }
    if ((0U != ([&]() {
                    __Vfunc_test_gtext__DOT__gfx_pixel__70__y = 0x00000033U;
                    __Vfunc_test_gtext__DOT__gfx_pixel__70__x = 0x00000064U;
                    __Vfunc_test_gtext__DOT__gfx_pixel__70__Vfuncout = 0;
                    __Vfunc_test_gtext__DOT__gfx_pixel__70__Vfuncout 
                        = ((0xf9ffU >= (0x0000ffffU 
                                        & (VL_MULS_III(32, (IData)(0x00000140U), __Vfunc_test_gtext__DOT__gfx_pixel__70__y) 
                                           + __Vfunc_test_gtext__DOT__gfx_pixel__70__x)))
                            ? vlSelfRef.test_gtext__DOT__dut__DOT__gfx_inst__DOT__gfx_mem__DOT__mem
                           [(0x0000ffffU & (VL_MULS_III(32, (IData)(0x00000140U), __Vfunc_test_gtext__DOT__gfx_pixel__70__y) 
                                            + __Vfunc_test_gtext__DOT__gfx_pixel__70__x))]
                            : 0U);
                }(), (IData)(__Vfunc_test_gtext__DOT__gfx_pixel__70__Vfuncout)))) {
        vlSelfRef.test_gtext__DOT__unnamedblk9__DOT__pc 
            = ((IData)(1U) + vlSelfRef.test_gtext__DOT__unnamedblk9__DOT__pc);
    }
    if ((0U != ([&]() {
                    __Vfunc_test_gtext__DOT__gfx_pixel__70__y = 0x00000033U;
                    __Vfunc_test_gtext__DOT__gfx_pixel__70__x = 0x00000065U;
                    __Vfunc_test_gtext__DOT__gfx_pixel__70__Vfuncout = 0;
                    __Vfunc_test_gtext__DOT__gfx_pixel__70__Vfuncout 
                        = ((0xf9ffU >= (0x0000ffffU 
                                        & (VL_MULS_III(32, (IData)(0x00000140U), __Vfunc_test_gtext__DOT__gfx_pixel__70__y) 
                                           + __Vfunc_test_gtext__DOT__gfx_pixel__70__x)))
                            ? vlSelfRef.test_gtext__DOT__dut__DOT__gfx_inst__DOT__gfx_mem__DOT__mem
                           [(0x0000ffffU & (VL_MULS_III(32, (IData)(0x00000140U), __Vfunc_test_gtext__DOT__gfx_pixel__70__y) 
                                            + __Vfunc_test_gtext__DOT__gfx_pixel__70__x))]
                            : 0U);
                }(), (IData)(__Vfunc_test_gtext__DOT__gfx_pixel__70__Vfuncout)))) {
        vlSelfRef.test_gtext__DOT__unnamedblk9__DOT__pc 
            = ((IData)(1U) + vlSelfRef.test_gtext__DOT__unnamedblk9__DOT__pc);
    }
    if ((0U != ([&]() {
                    __Vfunc_test_gtext__DOT__gfx_pixel__70__y = 0x00000033U;
                    __Vfunc_test_gtext__DOT__gfx_pixel__70__x = 0x00000066U;
                    __Vfunc_test_gtext__DOT__gfx_pixel__70__Vfuncout = 0;
                    __Vfunc_test_gtext__DOT__gfx_pixel__70__Vfuncout 
                        = ((0xf9ffU >= (0x0000ffffU 
                                        & (VL_MULS_III(32, (IData)(0x00000140U), __Vfunc_test_gtext__DOT__gfx_pixel__70__y) 
                                           + __Vfunc_test_gtext__DOT__gfx_pixel__70__x)))
                            ? vlSelfRef.test_gtext__DOT__dut__DOT__gfx_inst__DOT__gfx_mem__DOT__mem
                           [(0x0000ffffU & (VL_MULS_III(32, (IData)(0x00000140U), __Vfunc_test_gtext__DOT__gfx_pixel__70__y) 
                                            + __Vfunc_test_gtext__DOT__gfx_pixel__70__x))]
                            : 0U);
                }(), (IData)(__Vfunc_test_gtext__DOT__gfx_pixel__70__Vfuncout)))) {
        vlSelfRef.test_gtext__DOT__unnamedblk9__DOT__pc 
            = ((IData)(1U) + vlSelfRef.test_gtext__DOT__unnamedblk9__DOT__pc);
    }
    if ((0U != ([&]() {
                    __Vfunc_test_gtext__DOT__gfx_pixel__70__y = 0x00000033U;
                    __Vfunc_test_gtext__DOT__gfx_pixel__70__x = 0x00000067U;
                    __Vfunc_test_gtext__DOT__gfx_pixel__70__Vfuncout = 0;
                    __Vfunc_test_gtext__DOT__gfx_pixel__70__Vfuncout 
                        = ((0xf9ffU >= (0x0000ffffU 
                                        & (VL_MULS_III(32, (IData)(0x00000140U), __Vfunc_test_gtext__DOT__gfx_pixel__70__y) 
                                           + __Vfunc_test_gtext__DOT__gfx_pixel__70__x)))
                            ? vlSelfRef.test_gtext__DOT__dut__DOT__gfx_inst__DOT__gfx_mem__DOT__mem
                           [(0x0000ffffU & (VL_MULS_III(32, (IData)(0x00000140U), __Vfunc_test_gtext__DOT__gfx_pixel__70__y) 
                                            + __Vfunc_test_gtext__DOT__gfx_pixel__70__x))]
                            : 0U);
                }(), (IData)(__Vfunc_test_gtext__DOT__gfx_pixel__70__Vfuncout)))) {
        vlSelfRef.test_gtext__DOT__unnamedblk9__DOT__pc 
            = ((IData)(1U) + vlSelfRef.test_gtext__DOT__unnamedblk9__DOT__pc);
    }
    if ((0U != ([&]() {
                    __Vfunc_test_gtext__DOT__gfx_pixel__70__y = 0x00000033U;
                    __Vfunc_test_gtext__DOT__gfx_pixel__70__x = 0x00000068U;
                    __Vfunc_test_gtext__DOT__gfx_pixel__70__Vfuncout = 0;
                    __Vfunc_test_gtext__DOT__gfx_pixel__70__Vfuncout 
                        = ((0xf9ffU >= (0x0000ffffU 
                                        & (VL_MULS_III(32, (IData)(0x00000140U), __Vfunc_test_gtext__DOT__gfx_pixel__70__y) 
                                           + __Vfunc_test_gtext__DOT__gfx_pixel__70__x)))
                            ? vlSelfRef.test_gtext__DOT__dut__DOT__gfx_inst__DOT__gfx_mem__DOT__mem
                           [(0x0000ffffU & (VL_MULS_III(32, (IData)(0x00000140U), __Vfunc_test_gtext__DOT__gfx_pixel__70__y) 
                                            + __Vfunc_test_gtext__DOT__gfx_pixel__70__x))]
                            : 0U);
                }(), (IData)(__Vfunc_test_gtext__DOT__gfx_pixel__70__Vfuncout)))) {
        vlSelfRef.test_gtext__DOT__unnamedblk9__DOT__pc 
            = ((IData)(1U) + vlSelfRef.test_gtext__DOT__unnamedblk9__DOT__pc);
    }
    if ((0U != ([&]() {
                    __Vfunc_test_gtext__DOT__gfx_pixel__70__y = 0x00000033U;
                    __Vfunc_test_gtext__DOT__gfx_pixel__70__x = 0x00000069U;
                    __Vfunc_test_gtext__DOT__gfx_pixel__70__Vfuncout = 0;
                    __Vfunc_test_gtext__DOT__gfx_pixel__70__Vfuncout 
                        = ((0xf9ffU >= (0x0000ffffU 
                                        & (VL_MULS_III(32, (IData)(0x00000140U), __Vfunc_test_gtext__DOT__gfx_pixel__70__y) 
                                           + __Vfunc_test_gtext__DOT__gfx_pixel__70__x)))
                            ? vlSelfRef.test_gtext__DOT__dut__DOT__gfx_inst__DOT__gfx_mem__DOT__mem
                           [(0x0000ffffU & (VL_MULS_III(32, (IData)(0x00000140U), __Vfunc_test_gtext__DOT__gfx_pixel__70__y) 
                                            + __Vfunc_test_gtext__DOT__gfx_pixel__70__x))]
                            : 0U);
                }(), (IData)(__Vfunc_test_gtext__DOT__gfx_pixel__70__Vfuncout)))) {
        vlSelfRef.test_gtext__DOT__unnamedblk9__DOT__pc 
            = ((IData)(1U) + vlSelfRef.test_gtext__DOT__unnamedblk9__DOT__pc);
    }
    if ((0U != ([&]() {
                    __Vfunc_test_gtext__DOT__gfx_pixel__70__y = 0x00000033U;
                    __Vfunc_test_gtext__DOT__gfx_pixel__70__x = 0x0000006aU;
                    __Vfunc_test_gtext__DOT__gfx_pixel__70__Vfuncout = 0;
                    __Vfunc_test_gtext__DOT__gfx_pixel__70__Vfuncout 
                        = ((0xf9ffU >= (0x0000ffffU 
                                        & (VL_MULS_III(32, (IData)(0x00000140U), __Vfunc_test_gtext__DOT__gfx_pixel__70__y) 
                                           + __Vfunc_test_gtext__DOT__gfx_pixel__70__x)))
                            ? vlSelfRef.test_gtext__DOT__dut__DOT__gfx_inst__DOT__gfx_mem__DOT__mem
                           [(0x0000ffffU & (VL_MULS_III(32, (IData)(0x00000140U), __Vfunc_test_gtext__DOT__gfx_pixel__70__y) 
                                            + __Vfunc_test_gtext__DOT__gfx_pixel__70__x))]
                            : 0U);
                }(), (IData)(__Vfunc_test_gtext__DOT__gfx_pixel__70__Vfuncout)))) {
        vlSelfRef.test_gtext__DOT__unnamedblk9__DOT__pc 
            = ((IData)(1U) + vlSelfRef.test_gtext__DOT__unnamedblk9__DOT__pc);
    }
    if ((0U != ([&]() {
                    __Vfunc_test_gtext__DOT__gfx_pixel__70__y = 0x00000033U;
                    __Vfunc_test_gtext__DOT__gfx_pixel__70__x = 0x0000006bU;
                    __Vfunc_test_gtext__DOT__gfx_pixel__70__Vfuncout = 0;
                    __Vfunc_test_gtext__DOT__gfx_pixel__70__Vfuncout 
                        = ((0xf9ffU >= (0x0000ffffU 
                                        & (VL_MULS_III(32, (IData)(0x00000140U), __Vfunc_test_gtext__DOT__gfx_pixel__70__y) 
                                           + __Vfunc_test_gtext__DOT__gfx_pixel__70__x)))
                            ? vlSelfRef.test_gtext__DOT__dut__DOT__gfx_inst__DOT__gfx_mem__DOT__mem
                           [(0x0000ffffU & (VL_MULS_III(32, (IData)(0x00000140U), __Vfunc_test_gtext__DOT__gfx_pixel__70__y) 
                                            + __Vfunc_test_gtext__DOT__gfx_pixel__70__x))]
                            : 0U);
                }(), (IData)(__Vfunc_test_gtext__DOT__gfx_pixel__70__Vfuncout)))) {
        vlSelfRef.test_gtext__DOT__unnamedblk9__DOT__pc 
            = ((IData)(1U) + vlSelfRef.test_gtext__DOT__unnamedblk9__DOT__pc);
    }
    if ((0U != ([&]() {
                    __Vfunc_test_gtext__DOT__gfx_pixel__70__y = 0x00000034U;
                    __Vfunc_test_gtext__DOT__gfx_pixel__70__x = 0x00000064U;
                    __Vfunc_test_gtext__DOT__gfx_pixel__70__Vfuncout = 0;
                    __Vfunc_test_gtext__DOT__gfx_pixel__70__Vfuncout 
                        = ((0xf9ffU >= (0x0000ffffU 
                                        & (VL_MULS_III(32, (IData)(0x00000140U), __Vfunc_test_gtext__DOT__gfx_pixel__70__y) 
                                           + __Vfunc_test_gtext__DOT__gfx_pixel__70__x)))
                            ? vlSelfRef.test_gtext__DOT__dut__DOT__gfx_inst__DOT__gfx_mem__DOT__mem
                           [(0x0000ffffU & (VL_MULS_III(32, (IData)(0x00000140U), __Vfunc_test_gtext__DOT__gfx_pixel__70__y) 
                                            + __Vfunc_test_gtext__DOT__gfx_pixel__70__x))]
                            : 0U);
                }(), (IData)(__Vfunc_test_gtext__DOT__gfx_pixel__70__Vfuncout)))) {
        vlSelfRef.test_gtext__DOT__unnamedblk9__DOT__pc 
            = ((IData)(1U) + vlSelfRef.test_gtext__DOT__unnamedblk9__DOT__pc);
    }
    if ((0U != ([&]() {
                    __Vfunc_test_gtext__DOT__gfx_pixel__70__y = 0x00000034U;
                    __Vfunc_test_gtext__DOT__gfx_pixel__70__x = 0x00000065U;
                    __Vfunc_test_gtext__DOT__gfx_pixel__70__Vfuncout = 0;
                    __Vfunc_test_gtext__DOT__gfx_pixel__70__Vfuncout 
                        = ((0xf9ffU >= (0x0000ffffU 
                                        & (VL_MULS_III(32, (IData)(0x00000140U), __Vfunc_test_gtext__DOT__gfx_pixel__70__y) 
                                           + __Vfunc_test_gtext__DOT__gfx_pixel__70__x)))
                            ? vlSelfRef.test_gtext__DOT__dut__DOT__gfx_inst__DOT__gfx_mem__DOT__mem
                           [(0x0000ffffU & (VL_MULS_III(32, (IData)(0x00000140U), __Vfunc_test_gtext__DOT__gfx_pixel__70__y) 
                                            + __Vfunc_test_gtext__DOT__gfx_pixel__70__x))]
                            : 0U);
                }(), (IData)(__Vfunc_test_gtext__DOT__gfx_pixel__70__Vfuncout)))) {
        vlSelfRef.test_gtext__DOT__unnamedblk9__DOT__pc 
            = ((IData)(1U) + vlSelfRef.test_gtext__DOT__unnamedblk9__DOT__pc);
    }
    if ((0U != ([&]() {
                    __Vfunc_test_gtext__DOT__gfx_pixel__70__y = 0x00000034U;
                    __Vfunc_test_gtext__DOT__gfx_pixel__70__x = 0x00000066U;
                    __Vfunc_test_gtext__DOT__gfx_pixel__70__Vfuncout = 0;
                    __Vfunc_test_gtext__DOT__gfx_pixel__70__Vfuncout 
                        = ((0xf9ffU >= (0x0000ffffU 
                                        & (VL_MULS_III(32, (IData)(0x00000140U), __Vfunc_test_gtext__DOT__gfx_pixel__70__y) 
                                           + __Vfunc_test_gtext__DOT__gfx_pixel__70__x)))
                            ? vlSelfRef.test_gtext__DOT__dut__DOT__gfx_inst__DOT__gfx_mem__DOT__mem
                           [(0x0000ffffU & (VL_MULS_III(32, (IData)(0x00000140U), __Vfunc_test_gtext__DOT__gfx_pixel__70__y) 
                                            + __Vfunc_test_gtext__DOT__gfx_pixel__70__x))]
                            : 0U);
                }(), (IData)(__Vfunc_test_gtext__DOT__gfx_pixel__70__Vfuncout)))) {
        vlSelfRef.test_gtext__DOT__unnamedblk9__DOT__pc 
            = ((IData)(1U) + vlSelfRef.test_gtext__DOT__unnamedblk9__DOT__pc);
    }
    if ((0U != ([&]() {
                    __Vfunc_test_gtext__DOT__gfx_pixel__70__y = 0x00000034U;
                    __Vfunc_test_gtext__DOT__gfx_pixel__70__x = 0x00000067U;
                    __Vfunc_test_gtext__DOT__gfx_pixel__70__Vfuncout = 0;
                    __Vfunc_test_gtext__DOT__gfx_pixel__70__Vfuncout 
                        = ((0xf9ffU >= (0x0000ffffU 
                                        & (VL_MULS_III(32, (IData)(0x00000140U), __Vfunc_test_gtext__DOT__gfx_pixel__70__y) 
                                           + __Vfunc_test_gtext__DOT__gfx_pixel__70__x)))
                            ? vlSelfRef.test_gtext__DOT__dut__DOT__gfx_inst__DOT__gfx_mem__DOT__mem
                           [(0x0000ffffU & (VL_MULS_III(32, (IData)(0x00000140U), __Vfunc_test_gtext__DOT__gfx_pixel__70__y) 
                                            + __Vfunc_test_gtext__DOT__gfx_pixel__70__x))]
                            : 0U);
                }(), (IData)(__Vfunc_test_gtext__DOT__gfx_pixel__70__Vfuncout)))) {
        vlSelfRef.test_gtext__DOT__unnamedblk9__DOT__pc 
            = ((IData)(1U) + vlSelfRef.test_gtext__DOT__unnamedblk9__DOT__pc);
    }
    if ((0U != ([&]() {
                    __Vfunc_test_gtext__DOT__gfx_pixel__70__y = 0x00000034U;
                    __Vfunc_test_gtext__DOT__gfx_pixel__70__x = 0x00000068U;
                    __Vfunc_test_gtext__DOT__gfx_pixel__70__Vfuncout = 0;
                    __Vfunc_test_gtext__DOT__gfx_pixel__70__Vfuncout 
                        = ((0xf9ffU >= (0x0000ffffU 
                                        & (VL_MULS_III(32, (IData)(0x00000140U), __Vfunc_test_gtext__DOT__gfx_pixel__70__y) 
                                           + __Vfunc_test_gtext__DOT__gfx_pixel__70__x)))
                            ? vlSelfRef.test_gtext__DOT__dut__DOT__gfx_inst__DOT__gfx_mem__DOT__mem
                           [(0x0000ffffU & (VL_MULS_III(32, (IData)(0x00000140U), __Vfunc_test_gtext__DOT__gfx_pixel__70__y) 
                                            + __Vfunc_test_gtext__DOT__gfx_pixel__70__x))]
                            : 0U);
                }(), (IData)(__Vfunc_test_gtext__DOT__gfx_pixel__70__Vfuncout)))) {
        vlSelfRef.test_gtext__DOT__unnamedblk9__DOT__pc 
            = ((IData)(1U) + vlSelfRef.test_gtext__DOT__unnamedblk9__DOT__pc);
    }
    if ((0U != ([&]() {
                    __Vfunc_test_gtext__DOT__gfx_pixel__70__y = 0x00000034U;
                    __Vfunc_test_gtext__DOT__gfx_pixel__70__x = 0x00000069U;
                    __Vfunc_test_gtext__DOT__gfx_pixel__70__Vfuncout = 0;
                    __Vfunc_test_gtext__DOT__gfx_pixel__70__Vfuncout 
                        = ((0xf9ffU >= (0x0000ffffU 
                                        & (VL_MULS_III(32, (IData)(0x00000140U), __Vfunc_test_gtext__DOT__gfx_pixel__70__y) 
                                           + __Vfunc_test_gtext__DOT__gfx_pixel__70__x)))
                            ? vlSelfRef.test_gtext__DOT__dut__DOT__gfx_inst__DOT__gfx_mem__DOT__mem
                           [(0x0000ffffU & (VL_MULS_III(32, (IData)(0x00000140U), __Vfunc_test_gtext__DOT__gfx_pixel__70__y) 
                                            + __Vfunc_test_gtext__DOT__gfx_pixel__70__x))]
                            : 0U);
                }(), (IData)(__Vfunc_test_gtext__DOT__gfx_pixel__70__Vfuncout)))) {
        vlSelfRef.test_gtext__DOT__unnamedblk9__DOT__pc 
            = ((IData)(1U) + vlSelfRef.test_gtext__DOT__unnamedblk9__DOT__pc);
    }
    if ((0U != ([&]() {
                    __Vfunc_test_gtext__DOT__gfx_pixel__70__y = 0x00000034U;
                    __Vfunc_test_gtext__DOT__gfx_pixel__70__x = 0x0000006aU;
                    __Vfunc_test_gtext__DOT__gfx_pixel__70__Vfuncout = 0;
                    __Vfunc_test_gtext__DOT__gfx_pixel__70__Vfuncout 
                        = ((0xf9ffU >= (0x0000ffffU 
                                        & (VL_MULS_III(32, (IData)(0x00000140U), __Vfunc_test_gtext__DOT__gfx_pixel__70__y) 
                                           + __Vfunc_test_gtext__DOT__gfx_pixel__70__x)))
                            ? vlSelfRef.test_gtext__DOT__dut__DOT__gfx_inst__DOT__gfx_mem__DOT__mem
                           [(0x0000ffffU & (VL_MULS_III(32, (IData)(0x00000140U), __Vfunc_test_gtext__DOT__gfx_pixel__70__y) 
                                            + __Vfunc_test_gtext__DOT__gfx_pixel__70__x))]
                            : 0U);
                }(), (IData)(__Vfunc_test_gtext__DOT__gfx_pixel__70__Vfuncout)))) {
        vlSelfRef.test_gtext__DOT__unnamedblk9__DOT__pc 
            = ((IData)(1U) + vlSelfRef.test_gtext__DOT__unnamedblk9__DOT__pc);
    }
    if ((0U != ([&]() {
                    __Vfunc_test_gtext__DOT__gfx_pixel__70__y = 0x00000034U;
                    __Vfunc_test_gtext__DOT__gfx_pixel__70__x = 0x0000006bU;
                    __Vfunc_test_gtext__DOT__gfx_pixel__70__Vfuncout = 0;
                    __Vfunc_test_gtext__DOT__gfx_pixel__70__Vfuncout 
                        = ((0xf9ffU >= (0x0000ffffU 
                                        & (VL_MULS_III(32, (IData)(0x00000140U), __Vfunc_test_gtext__DOT__gfx_pixel__70__y) 
                                           + __Vfunc_test_gtext__DOT__gfx_pixel__70__x)))
                            ? vlSelfRef.test_gtext__DOT__dut__DOT__gfx_inst__DOT__gfx_mem__DOT__mem
                           [(0x0000ffffU & (VL_MULS_III(32, (IData)(0x00000140U), __Vfunc_test_gtext__DOT__gfx_pixel__70__y) 
                                            + __Vfunc_test_gtext__DOT__gfx_pixel__70__x))]
                            : 0U);
                }(), (IData)(__Vfunc_test_gtext__DOT__gfx_pixel__70__Vfuncout)))) {
        vlSelfRef.test_gtext__DOT__unnamedblk9__DOT__pc 
            = ((IData)(1U) + vlSelfRef.test_gtext__DOT__unnamedblk9__DOT__pc);
    }
    if ((0U != ([&]() {
                    __Vfunc_test_gtext__DOT__gfx_pixel__70__y = 0x00000035U;
                    __Vfunc_test_gtext__DOT__gfx_pixel__70__x = 0x00000064U;
                    __Vfunc_test_gtext__DOT__gfx_pixel__70__Vfuncout = 0;
                    __Vfunc_test_gtext__DOT__gfx_pixel__70__Vfuncout 
                        = ((0xf9ffU >= (0x0000ffffU 
                                        & (VL_MULS_III(32, (IData)(0x00000140U), __Vfunc_test_gtext__DOT__gfx_pixel__70__y) 
                                           + __Vfunc_test_gtext__DOT__gfx_pixel__70__x)))
                            ? vlSelfRef.test_gtext__DOT__dut__DOT__gfx_inst__DOT__gfx_mem__DOT__mem
                           [(0x0000ffffU & (VL_MULS_III(32, (IData)(0x00000140U), __Vfunc_test_gtext__DOT__gfx_pixel__70__y) 
                                            + __Vfunc_test_gtext__DOT__gfx_pixel__70__x))]
                            : 0U);
                }(), (IData)(__Vfunc_test_gtext__DOT__gfx_pixel__70__Vfuncout)))) {
        vlSelfRef.test_gtext__DOT__unnamedblk9__DOT__pc 
            = ((IData)(1U) + vlSelfRef.test_gtext__DOT__unnamedblk9__DOT__pc);
    }
    if ((0U != ([&]() {
                    __Vfunc_test_gtext__DOT__gfx_pixel__70__y = 0x00000035U;
                    __Vfunc_test_gtext__DOT__gfx_pixel__70__x = 0x00000065U;
                    __Vfunc_test_gtext__DOT__gfx_pixel__70__Vfuncout = 0;
                    __Vfunc_test_gtext__DOT__gfx_pixel__70__Vfuncout 
                        = ((0xf9ffU >= (0x0000ffffU 
                                        & (VL_MULS_III(32, (IData)(0x00000140U), __Vfunc_test_gtext__DOT__gfx_pixel__70__y) 
                                           + __Vfunc_test_gtext__DOT__gfx_pixel__70__x)))
                            ? vlSelfRef.test_gtext__DOT__dut__DOT__gfx_inst__DOT__gfx_mem__DOT__mem
                           [(0x0000ffffU & (VL_MULS_III(32, (IData)(0x00000140U), __Vfunc_test_gtext__DOT__gfx_pixel__70__y) 
                                            + __Vfunc_test_gtext__DOT__gfx_pixel__70__x))]
                            : 0U);
                }(), (IData)(__Vfunc_test_gtext__DOT__gfx_pixel__70__Vfuncout)))) {
        vlSelfRef.test_gtext__DOT__unnamedblk9__DOT__pc 
            = ((IData)(1U) + vlSelfRef.test_gtext__DOT__unnamedblk9__DOT__pc);
    }
    if ((0U != ([&]() {
                    __Vfunc_test_gtext__DOT__gfx_pixel__70__y = 0x00000035U;
                    __Vfunc_test_gtext__DOT__gfx_pixel__70__x = 0x00000066U;
                    __Vfunc_test_gtext__DOT__gfx_pixel__70__Vfuncout = 0;
                    __Vfunc_test_gtext__DOT__gfx_pixel__70__Vfuncout 
                        = ((0xf9ffU >= (0x0000ffffU 
                                        & (VL_MULS_III(32, (IData)(0x00000140U), __Vfunc_test_gtext__DOT__gfx_pixel__70__y) 
                                           + __Vfunc_test_gtext__DOT__gfx_pixel__70__x)))
                            ? vlSelfRef.test_gtext__DOT__dut__DOT__gfx_inst__DOT__gfx_mem__DOT__mem
                           [(0x0000ffffU & (VL_MULS_III(32, (IData)(0x00000140U), __Vfunc_test_gtext__DOT__gfx_pixel__70__y) 
                                            + __Vfunc_test_gtext__DOT__gfx_pixel__70__x))]
                            : 0U);
                }(), (IData)(__Vfunc_test_gtext__DOT__gfx_pixel__70__Vfuncout)))) {
        vlSelfRef.test_gtext__DOT__unnamedblk9__DOT__pc 
            = ((IData)(1U) + vlSelfRef.test_gtext__DOT__unnamedblk9__DOT__pc);
    }
    if ((0U != ([&]() {
                    __Vfunc_test_gtext__DOT__gfx_pixel__70__y = 0x00000035U;
                    __Vfunc_test_gtext__DOT__gfx_pixel__70__x = 0x00000067U;
                    __Vfunc_test_gtext__DOT__gfx_pixel__70__Vfuncout = 0;
                    __Vfunc_test_gtext__DOT__gfx_pixel__70__Vfuncout 
                        = ((0xf9ffU >= (0x0000ffffU 
                                        & (VL_MULS_III(32, (IData)(0x00000140U), __Vfunc_test_gtext__DOT__gfx_pixel__70__y) 
                                           + __Vfunc_test_gtext__DOT__gfx_pixel__70__x)))
                            ? vlSelfRef.test_gtext__DOT__dut__DOT__gfx_inst__DOT__gfx_mem__DOT__mem
                           [(0x0000ffffU & (VL_MULS_III(32, (IData)(0x00000140U), __Vfunc_test_gtext__DOT__gfx_pixel__70__y) 
                                            + __Vfunc_test_gtext__DOT__gfx_pixel__70__x))]
                            : 0U);
                }(), (IData)(__Vfunc_test_gtext__DOT__gfx_pixel__70__Vfuncout)))) {
        vlSelfRef.test_gtext__DOT__unnamedblk9__DOT__pc 
            = ((IData)(1U) + vlSelfRef.test_gtext__DOT__unnamedblk9__DOT__pc);
    }
    if ((0U != ([&]() {
                    __Vfunc_test_gtext__DOT__gfx_pixel__70__y = 0x00000035U;
                    __Vfunc_test_gtext__DOT__gfx_pixel__70__x = 0x00000068U;
                    __Vfunc_test_gtext__DOT__gfx_pixel__70__Vfuncout = 0;
                    __Vfunc_test_gtext__DOT__gfx_pixel__70__Vfuncout 
                        = ((0xf9ffU >= (0x0000ffffU 
                                        & (VL_MULS_III(32, (IData)(0x00000140U), __Vfunc_test_gtext__DOT__gfx_pixel__70__y) 
                                           + __Vfunc_test_gtext__DOT__gfx_pixel__70__x)))
                            ? vlSelfRef.test_gtext__DOT__dut__DOT__gfx_inst__DOT__gfx_mem__DOT__mem
                           [(0x0000ffffU & (VL_MULS_III(32, (IData)(0x00000140U), __Vfunc_test_gtext__DOT__gfx_pixel__70__y) 
                                            + __Vfunc_test_gtext__DOT__gfx_pixel__70__x))]
                            : 0U);
                }(), (IData)(__Vfunc_test_gtext__DOT__gfx_pixel__70__Vfuncout)))) {
        vlSelfRef.test_gtext__DOT__unnamedblk9__DOT__pc 
            = ((IData)(1U) + vlSelfRef.test_gtext__DOT__unnamedblk9__DOT__pc);
    }
    if ((0U != ([&]() {
                    __Vfunc_test_gtext__DOT__gfx_pixel__70__y = 0x00000035U;
                    __Vfunc_test_gtext__DOT__gfx_pixel__70__x = 0x00000069U;
                    __Vfunc_test_gtext__DOT__gfx_pixel__70__Vfuncout = 0;
                    __Vfunc_test_gtext__DOT__gfx_pixel__70__Vfuncout 
                        = ((0xf9ffU >= (0x0000ffffU 
                                        & (VL_MULS_III(32, (IData)(0x00000140U), __Vfunc_test_gtext__DOT__gfx_pixel__70__y) 
                                           + __Vfunc_test_gtext__DOT__gfx_pixel__70__x)))
                            ? vlSelfRef.test_gtext__DOT__dut__DOT__gfx_inst__DOT__gfx_mem__DOT__mem
                           [(0x0000ffffU & (VL_MULS_III(32, (IData)(0x00000140U), __Vfunc_test_gtext__DOT__gfx_pixel__70__y) 
                                            + __Vfunc_test_gtext__DOT__gfx_pixel__70__x))]
                            : 0U);
                }(), (IData)(__Vfunc_test_gtext__DOT__gfx_pixel__70__Vfuncout)))) {
        vlSelfRef.test_gtext__DOT__unnamedblk9__DOT__pc 
            = ((IData)(1U) + vlSelfRef.test_gtext__DOT__unnamedblk9__DOT__pc);
    }
    if ((0U != ([&]() {
                    __Vfunc_test_gtext__DOT__gfx_pixel__70__y = 0x00000035U;
                    __Vfunc_test_gtext__DOT__gfx_pixel__70__x = 0x0000006aU;
                    __Vfunc_test_gtext__DOT__gfx_pixel__70__Vfuncout = 0;
                    __Vfunc_test_gtext__DOT__gfx_pixel__70__Vfuncout 
                        = ((0xf9ffU >= (0x0000ffffU 
                                        & (VL_MULS_III(32, (IData)(0x00000140U), __Vfunc_test_gtext__DOT__gfx_pixel__70__y) 
                                           + __Vfunc_test_gtext__DOT__gfx_pixel__70__x)))
                            ? vlSelfRef.test_gtext__DOT__dut__DOT__gfx_inst__DOT__gfx_mem__DOT__mem
                           [(0x0000ffffU & (VL_MULS_III(32, (IData)(0x00000140U), __Vfunc_test_gtext__DOT__gfx_pixel__70__y) 
                                            + __Vfunc_test_gtext__DOT__gfx_pixel__70__x))]
                            : 0U);
                }(), (IData)(__Vfunc_test_gtext__DOT__gfx_pixel__70__Vfuncout)))) {
        vlSelfRef.test_gtext__DOT__unnamedblk9__DOT__pc 
            = ((IData)(1U) + vlSelfRef.test_gtext__DOT__unnamedblk9__DOT__pc);
    }
    if ((0U != ([&]() {
                    __Vfunc_test_gtext__DOT__gfx_pixel__70__y = 0x00000035U;
                    __Vfunc_test_gtext__DOT__gfx_pixel__70__x = 0x0000006bU;
                    __Vfunc_test_gtext__DOT__gfx_pixel__70__Vfuncout = 0;
                    __Vfunc_test_gtext__DOT__gfx_pixel__70__Vfuncout 
                        = ((0xf9ffU >= (0x0000ffffU 
                                        & (VL_MULS_III(32, (IData)(0x00000140U), __Vfunc_test_gtext__DOT__gfx_pixel__70__y) 
                                           + __Vfunc_test_gtext__DOT__gfx_pixel__70__x)))
                            ? vlSelfRef.test_gtext__DOT__dut__DOT__gfx_inst__DOT__gfx_mem__DOT__mem
                           [(0x0000ffffU & (VL_MULS_III(32, (IData)(0x00000140U), __Vfunc_test_gtext__DOT__gfx_pixel__70__y) 
                                            + __Vfunc_test_gtext__DOT__gfx_pixel__70__x))]
                            : 0U);
                }(), (IData)(__Vfunc_test_gtext__DOT__gfx_pixel__70__Vfuncout)))) {
        vlSelfRef.test_gtext__DOT__unnamedblk9__DOT__pc 
            = ((IData)(1U) + vlSelfRef.test_gtext__DOT__unnamedblk9__DOT__pc);
    }
    if ((0U != ([&]() {
                    __Vfunc_test_gtext__DOT__gfx_pixel__70__y = 0x00000036U;
                    __Vfunc_test_gtext__DOT__gfx_pixel__70__x = 0x00000064U;
                    __Vfunc_test_gtext__DOT__gfx_pixel__70__Vfuncout = 0;
                    __Vfunc_test_gtext__DOT__gfx_pixel__70__Vfuncout 
                        = ((0xf9ffU >= (0x0000ffffU 
                                        & (VL_MULS_III(32, (IData)(0x00000140U), __Vfunc_test_gtext__DOT__gfx_pixel__70__y) 
                                           + __Vfunc_test_gtext__DOT__gfx_pixel__70__x)))
                            ? vlSelfRef.test_gtext__DOT__dut__DOT__gfx_inst__DOT__gfx_mem__DOT__mem
                           [(0x0000ffffU & (VL_MULS_III(32, (IData)(0x00000140U), __Vfunc_test_gtext__DOT__gfx_pixel__70__y) 
                                            + __Vfunc_test_gtext__DOT__gfx_pixel__70__x))]
                            : 0U);
                }(), (IData)(__Vfunc_test_gtext__DOT__gfx_pixel__70__Vfuncout)))) {
        vlSelfRef.test_gtext__DOT__unnamedblk9__DOT__pc 
            = ((IData)(1U) + vlSelfRef.test_gtext__DOT__unnamedblk9__DOT__pc);
    }
    if ((0U != ([&]() {
                    __Vfunc_test_gtext__DOT__gfx_pixel__70__y = 0x00000036U;
                    __Vfunc_test_gtext__DOT__gfx_pixel__70__x = 0x00000065U;
                    __Vfunc_test_gtext__DOT__gfx_pixel__70__Vfuncout = 0;
                    __Vfunc_test_gtext__DOT__gfx_pixel__70__Vfuncout 
                        = ((0xf9ffU >= (0x0000ffffU 
                                        & (VL_MULS_III(32, (IData)(0x00000140U), __Vfunc_test_gtext__DOT__gfx_pixel__70__y) 
                                           + __Vfunc_test_gtext__DOT__gfx_pixel__70__x)))
                            ? vlSelfRef.test_gtext__DOT__dut__DOT__gfx_inst__DOT__gfx_mem__DOT__mem
                           [(0x0000ffffU & (VL_MULS_III(32, (IData)(0x00000140U), __Vfunc_test_gtext__DOT__gfx_pixel__70__y) 
                                            + __Vfunc_test_gtext__DOT__gfx_pixel__70__x))]
                            : 0U);
                }(), (IData)(__Vfunc_test_gtext__DOT__gfx_pixel__70__Vfuncout)))) {
        vlSelfRef.test_gtext__DOT__unnamedblk9__DOT__pc 
            = ((IData)(1U) + vlSelfRef.test_gtext__DOT__unnamedblk9__DOT__pc);
    }
    if ((0U != ([&]() {
                    __Vfunc_test_gtext__DOT__gfx_pixel__70__y = 0x00000036U;
                    __Vfunc_test_gtext__DOT__gfx_pixel__70__x = 0x00000066U;
                    __Vfunc_test_gtext__DOT__gfx_pixel__70__Vfuncout = 0;
                    __Vfunc_test_gtext__DOT__gfx_pixel__70__Vfuncout 
                        = ((0xf9ffU >= (0x0000ffffU 
                                        & (VL_MULS_III(32, (IData)(0x00000140U), __Vfunc_test_gtext__DOT__gfx_pixel__70__y) 
                                           + __Vfunc_test_gtext__DOT__gfx_pixel__70__x)))
                            ? vlSelfRef.test_gtext__DOT__dut__DOT__gfx_inst__DOT__gfx_mem__DOT__mem
                           [(0x0000ffffU & (VL_MULS_III(32, (IData)(0x00000140U), __Vfunc_test_gtext__DOT__gfx_pixel__70__y) 
                                            + __Vfunc_test_gtext__DOT__gfx_pixel__70__x))]
                            : 0U);
                }(), (IData)(__Vfunc_test_gtext__DOT__gfx_pixel__70__Vfuncout)))) {
        vlSelfRef.test_gtext__DOT__unnamedblk9__DOT__pc 
            = ((IData)(1U) + vlSelfRef.test_gtext__DOT__unnamedblk9__DOT__pc);
    }
    if ((0U != ([&]() {
                    __Vfunc_test_gtext__DOT__gfx_pixel__70__y = 0x00000036U;
                    __Vfunc_test_gtext__DOT__gfx_pixel__70__x = 0x00000067U;
                    __Vfunc_test_gtext__DOT__gfx_pixel__70__Vfuncout = 0;
                    __Vfunc_test_gtext__DOT__gfx_pixel__70__Vfuncout 
                        = ((0xf9ffU >= (0x0000ffffU 
                                        & (VL_MULS_III(32, (IData)(0x00000140U), __Vfunc_test_gtext__DOT__gfx_pixel__70__y) 
                                           + __Vfunc_test_gtext__DOT__gfx_pixel__70__x)))
                            ? vlSelfRef.test_gtext__DOT__dut__DOT__gfx_inst__DOT__gfx_mem__DOT__mem
                           [(0x0000ffffU & (VL_MULS_III(32, (IData)(0x00000140U), __Vfunc_test_gtext__DOT__gfx_pixel__70__y) 
                                            + __Vfunc_test_gtext__DOT__gfx_pixel__70__x))]
                            : 0U);
                }(), (IData)(__Vfunc_test_gtext__DOT__gfx_pixel__70__Vfuncout)))) {
        vlSelfRef.test_gtext__DOT__unnamedblk9__DOT__pc 
            = ((IData)(1U) + vlSelfRef.test_gtext__DOT__unnamedblk9__DOT__pc);
    }
    if ((0U != ([&]() {
                    __Vfunc_test_gtext__DOT__gfx_pixel__70__y = 0x00000036U;
                    __Vfunc_test_gtext__DOT__gfx_pixel__70__x = 0x00000068U;
                    __Vfunc_test_gtext__DOT__gfx_pixel__70__Vfuncout = 0;
                    __Vfunc_test_gtext__DOT__gfx_pixel__70__Vfuncout 
                        = ((0xf9ffU >= (0x0000ffffU 
                                        & (VL_MULS_III(32, (IData)(0x00000140U), __Vfunc_test_gtext__DOT__gfx_pixel__70__y) 
                                           + __Vfunc_test_gtext__DOT__gfx_pixel__70__x)))
                            ? vlSelfRef.test_gtext__DOT__dut__DOT__gfx_inst__DOT__gfx_mem__DOT__mem
                           [(0x0000ffffU & (VL_MULS_III(32, (IData)(0x00000140U), __Vfunc_test_gtext__DOT__gfx_pixel__70__y) 
                                            + __Vfunc_test_gtext__DOT__gfx_pixel__70__x))]
                            : 0U);
                }(), (IData)(__Vfunc_test_gtext__DOT__gfx_pixel__70__Vfuncout)))) {
        vlSelfRef.test_gtext__DOT__unnamedblk9__DOT__pc 
            = ((IData)(1U) + vlSelfRef.test_gtext__DOT__unnamedblk9__DOT__pc);
    }
    if ((0U != ([&]() {
                    __Vfunc_test_gtext__DOT__gfx_pixel__70__y = 0x00000036U;
                    __Vfunc_test_gtext__DOT__gfx_pixel__70__x = 0x00000069U;
                    __Vfunc_test_gtext__DOT__gfx_pixel__70__Vfuncout = 0;
                    __Vfunc_test_gtext__DOT__gfx_pixel__70__Vfuncout 
                        = ((0xf9ffU >= (0x0000ffffU 
                                        & (VL_MULS_III(32, (IData)(0x00000140U), __Vfunc_test_gtext__DOT__gfx_pixel__70__y) 
                                           + __Vfunc_test_gtext__DOT__gfx_pixel__70__x)))
                            ? vlSelfRef.test_gtext__DOT__dut__DOT__gfx_inst__DOT__gfx_mem__DOT__mem
                           [(0x0000ffffU & (VL_MULS_III(32, (IData)(0x00000140U), __Vfunc_test_gtext__DOT__gfx_pixel__70__y) 
                                            + __Vfunc_test_gtext__DOT__gfx_pixel__70__x))]
                            : 0U);
                }(), (IData)(__Vfunc_test_gtext__DOT__gfx_pixel__70__Vfuncout)))) {
        vlSelfRef.test_gtext__DOT__unnamedblk9__DOT__pc 
            = ((IData)(1U) + vlSelfRef.test_gtext__DOT__unnamedblk9__DOT__pc);
    }
    if ((0U != ([&]() {
                    __Vfunc_test_gtext__DOT__gfx_pixel__70__y = 0x00000036U;
                    __Vfunc_test_gtext__DOT__gfx_pixel__70__x = 0x0000006aU;
                    __Vfunc_test_gtext__DOT__gfx_pixel__70__Vfuncout = 0;
                    __Vfunc_test_gtext__DOT__gfx_pixel__70__Vfuncout 
                        = ((0xf9ffU >= (0x0000ffffU 
                                        & (VL_MULS_III(32, (IData)(0x00000140U), __Vfunc_test_gtext__DOT__gfx_pixel__70__y) 
                                           + __Vfunc_test_gtext__DOT__gfx_pixel__70__x)))
                            ? vlSelfRef.test_gtext__DOT__dut__DOT__gfx_inst__DOT__gfx_mem__DOT__mem
                           [(0x0000ffffU & (VL_MULS_III(32, (IData)(0x00000140U), __Vfunc_test_gtext__DOT__gfx_pixel__70__y) 
                                            + __Vfunc_test_gtext__DOT__gfx_pixel__70__x))]
                            : 0U);
                }(), (IData)(__Vfunc_test_gtext__DOT__gfx_pixel__70__Vfuncout)))) {
        vlSelfRef.test_gtext__DOT__unnamedblk9__DOT__pc 
            = ((IData)(1U) + vlSelfRef.test_gtext__DOT__unnamedblk9__DOT__pc);
    }
    if ((0U != ([&]() {
                    __Vfunc_test_gtext__DOT__gfx_pixel__70__y = 0x00000036U;
                    __Vfunc_test_gtext__DOT__gfx_pixel__70__x = 0x0000006bU;
                    __Vfunc_test_gtext__DOT__gfx_pixel__70__Vfuncout = 0;
                    __Vfunc_test_gtext__DOT__gfx_pixel__70__Vfuncout 
                        = ((0xf9ffU >= (0x0000ffffU 
                                        & (VL_MULS_III(32, (IData)(0x00000140U), __Vfunc_test_gtext__DOT__gfx_pixel__70__y) 
                                           + __Vfunc_test_gtext__DOT__gfx_pixel__70__x)))
                            ? vlSelfRef.test_gtext__DOT__dut__DOT__gfx_inst__DOT__gfx_mem__DOT__mem
                           [(0x0000ffffU & (VL_MULS_III(32, (IData)(0x00000140U), __Vfunc_test_gtext__DOT__gfx_pixel__70__y) 
                                            + __Vfunc_test_gtext__DOT__gfx_pixel__70__x))]
                            : 0U);
                }(), (IData)(__Vfunc_test_gtext__DOT__gfx_pixel__70__Vfuncout)))) {
        vlSelfRef.test_gtext__DOT__unnamedblk9__DOT__pc 
            = ((IData)(1U) + vlSelfRef.test_gtext__DOT__unnamedblk9__DOT__pc);
    }
    if ((0U != ([&]() {
                    __Vfunc_test_gtext__DOT__gfx_pixel__70__y = 0x00000037U;
                    __Vfunc_test_gtext__DOT__gfx_pixel__70__x = 0x00000064U;
                    __Vfunc_test_gtext__DOT__gfx_pixel__70__Vfuncout = 0;
                    __Vfunc_test_gtext__DOT__gfx_pixel__70__Vfuncout 
                        = ((0xf9ffU >= (0x0000ffffU 
                                        & (VL_MULS_III(32, (IData)(0x00000140U), __Vfunc_test_gtext__DOT__gfx_pixel__70__y) 
                                           + __Vfunc_test_gtext__DOT__gfx_pixel__70__x)))
                            ? vlSelfRef.test_gtext__DOT__dut__DOT__gfx_inst__DOT__gfx_mem__DOT__mem
                           [(0x0000ffffU & (VL_MULS_III(32, (IData)(0x00000140U), __Vfunc_test_gtext__DOT__gfx_pixel__70__y) 
                                            + __Vfunc_test_gtext__DOT__gfx_pixel__70__x))]
                            : 0U);
                }(), (IData)(__Vfunc_test_gtext__DOT__gfx_pixel__70__Vfuncout)))) {
        vlSelfRef.test_gtext__DOT__unnamedblk9__DOT__pc 
            = ((IData)(1U) + vlSelfRef.test_gtext__DOT__unnamedblk9__DOT__pc);
    }
    if ((0U != ([&]() {
                    __Vfunc_test_gtext__DOT__gfx_pixel__70__y = 0x00000037U;
                    __Vfunc_test_gtext__DOT__gfx_pixel__70__x = 0x00000065U;
                    __Vfunc_test_gtext__DOT__gfx_pixel__70__Vfuncout = 0;
                    __Vfunc_test_gtext__DOT__gfx_pixel__70__Vfuncout 
                        = ((0xf9ffU >= (0x0000ffffU 
                                        & (VL_MULS_III(32, (IData)(0x00000140U), __Vfunc_test_gtext__DOT__gfx_pixel__70__y) 
                                           + __Vfunc_test_gtext__DOT__gfx_pixel__70__x)))
                            ? vlSelfRef.test_gtext__DOT__dut__DOT__gfx_inst__DOT__gfx_mem__DOT__mem
                           [(0x0000ffffU & (VL_MULS_III(32, (IData)(0x00000140U), __Vfunc_test_gtext__DOT__gfx_pixel__70__y) 
                                            + __Vfunc_test_gtext__DOT__gfx_pixel__70__x))]
                            : 0U);
                }(), (IData)(__Vfunc_test_gtext__DOT__gfx_pixel__70__Vfuncout)))) {
        vlSelfRef.test_gtext__DOT__unnamedblk9__DOT__pc 
            = ((IData)(1U) + vlSelfRef.test_gtext__DOT__unnamedblk9__DOT__pc);
    }
    if ((0U != ([&]() {
                    __Vfunc_test_gtext__DOT__gfx_pixel__70__y = 0x00000037U;
                    __Vfunc_test_gtext__DOT__gfx_pixel__70__x = 0x00000066U;
                    __Vfunc_test_gtext__DOT__gfx_pixel__70__Vfuncout = 0;
                    __Vfunc_test_gtext__DOT__gfx_pixel__70__Vfuncout 
                        = ((0xf9ffU >= (0x0000ffffU 
                                        & (VL_MULS_III(32, (IData)(0x00000140U), __Vfunc_test_gtext__DOT__gfx_pixel__70__y) 
                                           + __Vfunc_test_gtext__DOT__gfx_pixel__70__x)))
                            ? vlSelfRef.test_gtext__DOT__dut__DOT__gfx_inst__DOT__gfx_mem__DOT__mem
                           [(0x0000ffffU & (VL_MULS_III(32, (IData)(0x00000140U), __Vfunc_test_gtext__DOT__gfx_pixel__70__y) 
                                            + __Vfunc_test_gtext__DOT__gfx_pixel__70__x))]
                            : 0U);
                }(), (IData)(__Vfunc_test_gtext__DOT__gfx_pixel__70__Vfuncout)))) {
        vlSelfRef.test_gtext__DOT__unnamedblk9__DOT__pc 
            = ((IData)(1U) + vlSelfRef.test_gtext__DOT__unnamedblk9__DOT__pc);
    }
    if ((0U != ([&]() {
                    __Vfunc_test_gtext__DOT__gfx_pixel__70__y = 0x00000037U;
                    __Vfunc_test_gtext__DOT__gfx_pixel__70__x = 0x00000067U;
                    __Vfunc_test_gtext__DOT__gfx_pixel__70__Vfuncout = 0;
                    __Vfunc_test_gtext__DOT__gfx_pixel__70__Vfuncout 
                        = ((0xf9ffU >= (0x0000ffffU 
                                        & (VL_MULS_III(32, (IData)(0x00000140U), __Vfunc_test_gtext__DOT__gfx_pixel__70__y) 
                                           + __Vfunc_test_gtext__DOT__gfx_pixel__70__x)))
                            ? vlSelfRef.test_gtext__DOT__dut__DOT__gfx_inst__DOT__gfx_mem__DOT__mem
                           [(0x0000ffffU & (VL_MULS_III(32, (IData)(0x00000140U), __Vfunc_test_gtext__DOT__gfx_pixel__70__y) 
                                            + __Vfunc_test_gtext__DOT__gfx_pixel__70__x))]
                            : 0U);
                }(), (IData)(__Vfunc_test_gtext__DOT__gfx_pixel__70__Vfuncout)))) {
        vlSelfRef.test_gtext__DOT__unnamedblk9__DOT__pc 
            = ((IData)(1U) + vlSelfRef.test_gtext__DOT__unnamedblk9__DOT__pc);
    }
    if ((0U != ([&]() {
                    __Vfunc_test_gtext__DOT__gfx_pixel__70__y = 0x00000037U;
                    __Vfunc_test_gtext__DOT__gfx_pixel__70__x = 0x00000068U;
                    __Vfunc_test_gtext__DOT__gfx_pixel__70__Vfuncout = 0;
                    __Vfunc_test_gtext__DOT__gfx_pixel__70__Vfuncout 
                        = ((0xf9ffU >= (0x0000ffffU 
                                        & (VL_MULS_III(32, (IData)(0x00000140U), __Vfunc_test_gtext__DOT__gfx_pixel__70__y) 
                                           + __Vfunc_test_gtext__DOT__gfx_pixel__70__x)))
                            ? vlSelfRef.test_gtext__DOT__dut__DOT__gfx_inst__DOT__gfx_mem__DOT__mem
                           [(0x0000ffffU & (VL_MULS_III(32, (IData)(0x00000140U), __Vfunc_test_gtext__DOT__gfx_pixel__70__y) 
                                            + __Vfunc_test_gtext__DOT__gfx_pixel__70__x))]
                            : 0U);
                }(), (IData)(__Vfunc_test_gtext__DOT__gfx_pixel__70__Vfuncout)))) {
        vlSelfRef.test_gtext__DOT__unnamedblk9__DOT__pc 
            = ((IData)(1U) + vlSelfRef.test_gtext__DOT__unnamedblk9__DOT__pc);
    }
    if ((0U != ([&]() {
                    __Vfunc_test_gtext__DOT__gfx_pixel__70__y = 0x00000037U;
                    __Vfunc_test_gtext__DOT__gfx_pixel__70__x = 0x00000069U;
                    __Vfunc_test_gtext__DOT__gfx_pixel__70__Vfuncout = 0;
                    __Vfunc_test_gtext__DOT__gfx_pixel__70__Vfuncout 
                        = ((0xf9ffU >= (0x0000ffffU 
                                        & (VL_MULS_III(32, (IData)(0x00000140U), __Vfunc_test_gtext__DOT__gfx_pixel__70__y) 
                                           + __Vfunc_test_gtext__DOT__gfx_pixel__70__x)))
                            ? vlSelfRef.test_gtext__DOT__dut__DOT__gfx_inst__DOT__gfx_mem__DOT__mem
                           [(0x0000ffffU & (VL_MULS_III(32, (IData)(0x00000140U), __Vfunc_test_gtext__DOT__gfx_pixel__70__y) 
                                            + __Vfunc_test_gtext__DOT__gfx_pixel__70__x))]
                            : 0U);
                }(), (IData)(__Vfunc_test_gtext__DOT__gfx_pixel__70__Vfuncout)))) {
        vlSelfRef.test_gtext__DOT__unnamedblk9__DOT__pc 
            = ((IData)(1U) + vlSelfRef.test_gtext__DOT__unnamedblk9__DOT__pc);
    }
    if ((0U != ([&]() {
                    __Vfunc_test_gtext__DOT__gfx_pixel__70__y = 0x00000037U;
                    __Vfunc_test_gtext__DOT__gfx_pixel__70__x = 0x0000006aU;
                    __Vfunc_test_gtext__DOT__gfx_pixel__70__Vfuncout = 0;
                    __Vfunc_test_gtext__DOT__gfx_pixel__70__Vfuncout 
                        = ((0xf9ffU >= (0x0000ffffU 
                                        & (VL_MULS_III(32, (IData)(0x00000140U), __Vfunc_test_gtext__DOT__gfx_pixel__70__y) 
                                           + __Vfunc_test_gtext__DOT__gfx_pixel__70__x)))
                            ? vlSelfRef.test_gtext__DOT__dut__DOT__gfx_inst__DOT__gfx_mem__DOT__mem
                           [(0x0000ffffU & (VL_MULS_III(32, (IData)(0x00000140U), __Vfunc_test_gtext__DOT__gfx_pixel__70__y) 
                                            + __Vfunc_test_gtext__DOT__gfx_pixel__70__x))]
                            : 0U);
                }(), (IData)(__Vfunc_test_gtext__DOT__gfx_pixel__70__Vfuncout)))) {
        vlSelfRef.test_gtext__DOT__unnamedblk9__DOT__pc 
            = ((IData)(1U) + vlSelfRef.test_gtext__DOT__unnamedblk9__DOT__pc);
    }
    if ((0U != ([&]() {
                    __Vfunc_test_gtext__DOT__gfx_pixel__70__y = 0x00000037U;
                    __Vfunc_test_gtext__DOT__gfx_pixel__70__x = 0x0000006bU;
                    __Vfunc_test_gtext__DOT__gfx_pixel__70__Vfuncout = 0;
                    __Vfunc_test_gtext__DOT__gfx_pixel__70__Vfuncout 
                        = ((0xf9ffU >= (0x0000ffffU 
                                        & (VL_MULS_III(32, (IData)(0x00000140U), __Vfunc_test_gtext__DOT__gfx_pixel__70__y) 
                                           + __Vfunc_test_gtext__DOT__gfx_pixel__70__x)))
                            ? vlSelfRef.test_gtext__DOT__dut__DOT__gfx_inst__DOT__gfx_mem__DOT__mem
                           [(0x0000ffffU & (VL_MULS_III(32, (IData)(0x00000140U), __Vfunc_test_gtext__DOT__gfx_pixel__70__y) 
                                            + __Vfunc_test_gtext__DOT__gfx_pixel__70__x))]
                            : 0U);
                }(), (IData)(__Vfunc_test_gtext__DOT__gfx_pixel__70__Vfuncout)))) {
        vlSelfRef.test_gtext__DOT__unnamedblk9__DOT__pc 
            = ((IData)(1U) + vlSelfRef.test_gtext__DOT__unnamedblk9__DOT__pc);
    }
    if ((0U != ([&]() {
                    __Vfunc_test_gtext__DOT__gfx_pixel__70__y = 0x00000038U;
                    __Vfunc_test_gtext__DOT__gfx_pixel__70__x = 0x00000064U;
                    __Vfunc_test_gtext__DOT__gfx_pixel__70__Vfuncout = 0;
                    __Vfunc_test_gtext__DOT__gfx_pixel__70__Vfuncout 
                        = ((0xf9ffU >= (0x0000ffffU 
                                        & (VL_MULS_III(32, (IData)(0x00000140U), __Vfunc_test_gtext__DOT__gfx_pixel__70__y) 
                                           + __Vfunc_test_gtext__DOT__gfx_pixel__70__x)))
                            ? vlSelfRef.test_gtext__DOT__dut__DOT__gfx_inst__DOT__gfx_mem__DOT__mem
                           [(0x0000ffffU & (VL_MULS_III(32, (IData)(0x00000140U), __Vfunc_test_gtext__DOT__gfx_pixel__70__y) 
                                            + __Vfunc_test_gtext__DOT__gfx_pixel__70__x))]
                            : 0U);
                }(), (IData)(__Vfunc_test_gtext__DOT__gfx_pixel__70__Vfuncout)))) {
        vlSelfRef.test_gtext__DOT__unnamedblk9__DOT__pc 
            = ((IData)(1U) + vlSelfRef.test_gtext__DOT__unnamedblk9__DOT__pc);
    }
    if ((0U != ([&]() {
                    __Vfunc_test_gtext__DOT__gfx_pixel__70__y = 0x00000038U;
                    __Vfunc_test_gtext__DOT__gfx_pixel__70__x = 0x00000065U;
                    __Vfunc_test_gtext__DOT__gfx_pixel__70__Vfuncout = 0;
                    __Vfunc_test_gtext__DOT__gfx_pixel__70__Vfuncout 
                        = ((0xf9ffU >= (0x0000ffffU 
                                        & (VL_MULS_III(32, (IData)(0x00000140U), __Vfunc_test_gtext__DOT__gfx_pixel__70__y) 
                                           + __Vfunc_test_gtext__DOT__gfx_pixel__70__x)))
                            ? vlSelfRef.test_gtext__DOT__dut__DOT__gfx_inst__DOT__gfx_mem__DOT__mem
                           [(0x0000ffffU & (VL_MULS_III(32, (IData)(0x00000140U), __Vfunc_test_gtext__DOT__gfx_pixel__70__y) 
                                            + __Vfunc_test_gtext__DOT__gfx_pixel__70__x))]
                            : 0U);
                }(), (IData)(__Vfunc_test_gtext__DOT__gfx_pixel__70__Vfuncout)))) {
        vlSelfRef.test_gtext__DOT__unnamedblk9__DOT__pc 
            = ((IData)(1U) + vlSelfRef.test_gtext__DOT__unnamedblk9__DOT__pc);
    }
    if ((0U != ([&]() {
                    __Vfunc_test_gtext__DOT__gfx_pixel__70__y = 0x00000038U;
                    __Vfunc_test_gtext__DOT__gfx_pixel__70__x = 0x00000066U;
                    __Vfunc_test_gtext__DOT__gfx_pixel__70__Vfuncout = 0;
                    __Vfunc_test_gtext__DOT__gfx_pixel__70__Vfuncout 
                        = ((0xf9ffU >= (0x0000ffffU 
                                        & (VL_MULS_III(32, (IData)(0x00000140U), __Vfunc_test_gtext__DOT__gfx_pixel__70__y) 
                                           + __Vfunc_test_gtext__DOT__gfx_pixel__70__x)))
                            ? vlSelfRef.test_gtext__DOT__dut__DOT__gfx_inst__DOT__gfx_mem__DOT__mem
                           [(0x0000ffffU & (VL_MULS_III(32, (IData)(0x00000140U), __Vfunc_test_gtext__DOT__gfx_pixel__70__y) 
                                            + __Vfunc_test_gtext__DOT__gfx_pixel__70__x))]
                            : 0U);
                }(), (IData)(__Vfunc_test_gtext__DOT__gfx_pixel__70__Vfuncout)))) {
        vlSelfRef.test_gtext__DOT__unnamedblk9__DOT__pc 
            = ((IData)(1U) + vlSelfRef.test_gtext__DOT__unnamedblk9__DOT__pc);
    }
    if ((0U != ([&]() {
                    __Vfunc_test_gtext__DOT__gfx_pixel__70__y = 0x00000038U;
                    __Vfunc_test_gtext__DOT__gfx_pixel__70__x = 0x00000067U;
                    __Vfunc_test_gtext__DOT__gfx_pixel__70__Vfuncout = 0;
                    __Vfunc_test_gtext__DOT__gfx_pixel__70__Vfuncout 
                        = ((0xf9ffU >= (0x0000ffffU 
                                        & (VL_MULS_III(32, (IData)(0x00000140U), __Vfunc_test_gtext__DOT__gfx_pixel__70__y) 
                                           + __Vfunc_test_gtext__DOT__gfx_pixel__70__x)))
                            ? vlSelfRef.test_gtext__DOT__dut__DOT__gfx_inst__DOT__gfx_mem__DOT__mem
                           [(0x0000ffffU & (VL_MULS_III(32, (IData)(0x00000140U), __Vfunc_test_gtext__DOT__gfx_pixel__70__y) 
                                            + __Vfunc_test_gtext__DOT__gfx_pixel__70__x))]
                            : 0U);
                }(), (IData)(__Vfunc_test_gtext__DOT__gfx_pixel__70__Vfuncout)))) {
        vlSelfRef.test_gtext__DOT__unnamedblk9__DOT__pc 
            = ((IData)(1U) + vlSelfRef.test_gtext__DOT__unnamedblk9__DOT__pc);
    }
    if ((0U != ([&]() {
                    __Vfunc_test_gtext__DOT__gfx_pixel__70__y = 0x00000038U;
                    __Vfunc_test_gtext__DOT__gfx_pixel__70__x = 0x00000068U;
                    __Vfunc_test_gtext__DOT__gfx_pixel__70__Vfuncout = 0;
                    __Vfunc_test_gtext__DOT__gfx_pixel__70__Vfuncout 
                        = ((0xf9ffU >= (0x0000ffffU 
                                        & (VL_MULS_III(32, (IData)(0x00000140U), __Vfunc_test_gtext__DOT__gfx_pixel__70__y) 
                                           + __Vfunc_test_gtext__DOT__gfx_pixel__70__x)))
                            ? vlSelfRef.test_gtext__DOT__dut__DOT__gfx_inst__DOT__gfx_mem__DOT__mem
                           [(0x0000ffffU & (VL_MULS_III(32, (IData)(0x00000140U), __Vfunc_test_gtext__DOT__gfx_pixel__70__y) 
                                            + __Vfunc_test_gtext__DOT__gfx_pixel__70__x))]
                            : 0U);
                }(), (IData)(__Vfunc_test_gtext__DOT__gfx_pixel__70__Vfuncout)))) {
        vlSelfRef.test_gtext__DOT__unnamedblk9__DOT__pc 
            = ((IData)(1U) + vlSelfRef.test_gtext__DOT__unnamedblk9__DOT__pc);
    }
    if ((0U != ([&]() {
                    __Vfunc_test_gtext__DOT__gfx_pixel__70__y = 0x00000038U;
                    __Vfunc_test_gtext__DOT__gfx_pixel__70__x = 0x00000069U;
                    __Vfunc_test_gtext__DOT__gfx_pixel__70__Vfuncout = 0;
                    __Vfunc_test_gtext__DOT__gfx_pixel__70__Vfuncout 
                        = ((0xf9ffU >= (0x0000ffffU 
                                        & (VL_MULS_III(32, (IData)(0x00000140U), __Vfunc_test_gtext__DOT__gfx_pixel__70__y) 
                                           + __Vfunc_test_gtext__DOT__gfx_pixel__70__x)))
                            ? vlSelfRef.test_gtext__DOT__dut__DOT__gfx_inst__DOT__gfx_mem__DOT__mem
                           [(0x0000ffffU & (VL_MULS_III(32, (IData)(0x00000140U), __Vfunc_test_gtext__DOT__gfx_pixel__70__y) 
                                            + __Vfunc_test_gtext__DOT__gfx_pixel__70__x))]
                            : 0U);
                }(), (IData)(__Vfunc_test_gtext__DOT__gfx_pixel__70__Vfuncout)))) {
        vlSelfRef.test_gtext__DOT__unnamedblk9__DOT__pc 
            = ((IData)(1U) + vlSelfRef.test_gtext__DOT__unnamedblk9__DOT__pc);
    }
    if ((0U != ([&]() {
                    __Vfunc_test_gtext__DOT__gfx_pixel__70__y = 0x00000038U;
                    __Vfunc_test_gtext__DOT__gfx_pixel__70__x = 0x0000006aU;
                    __Vfunc_test_gtext__DOT__gfx_pixel__70__Vfuncout = 0;
                    __Vfunc_test_gtext__DOT__gfx_pixel__70__Vfuncout 
                        = ((0xf9ffU >= (0x0000ffffU 
                                        & (VL_MULS_III(32, (IData)(0x00000140U), __Vfunc_test_gtext__DOT__gfx_pixel__70__y) 
                                           + __Vfunc_test_gtext__DOT__gfx_pixel__70__x)))
                            ? vlSelfRef.test_gtext__DOT__dut__DOT__gfx_inst__DOT__gfx_mem__DOT__mem
                           [(0x0000ffffU & (VL_MULS_III(32, (IData)(0x00000140U), __Vfunc_test_gtext__DOT__gfx_pixel__70__y) 
                                            + __Vfunc_test_gtext__DOT__gfx_pixel__70__x))]
                            : 0U);
                }(), (IData)(__Vfunc_test_gtext__DOT__gfx_pixel__70__Vfuncout)))) {
        vlSelfRef.test_gtext__DOT__unnamedblk9__DOT__pc 
            = ((IData)(1U) + vlSelfRef.test_gtext__DOT__unnamedblk9__DOT__pc);
    }
    if ((0U != ([&]() {
                    __Vfunc_test_gtext__DOT__gfx_pixel__70__y = 0x00000038U;
                    __Vfunc_test_gtext__DOT__gfx_pixel__70__x = 0x0000006bU;
                    __Vfunc_test_gtext__DOT__gfx_pixel__70__Vfuncout = 0;
                    __Vfunc_test_gtext__DOT__gfx_pixel__70__Vfuncout 
                        = ((0xf9ffU >= (0x0000ffffU 
                                        & (VL_MULS_III(32, (IData)(0x00000140U), __Vfunc_test_gtext__DOT__gfx_pixel__70__y) 
                                           + __Vfunc_test_gtext__DOT__gfx_pixel__70__x)))
                            ? vlSelfRef.test_gtext__DOT__dut__DOT__gfx_inst__DOT__gfx_mem__DOT__mem
                           [(0x0000ffffU & (VL_MULS_III(32, (IData)(0x00000140U), __Vfunc_test_gtext__DOT__gfx_pixel__70__y) 
                                            + __Vfunc_test_gtext__DOT__gfx_pixel__70__x))]
                            : 0U);
                }(), (IData)(__Vfunc_test_gtext__DOT__gfx_pixel__70__Vfuncout)))) {
        vlSelfRef.test_gtext__DOT__unnamedblk9__DOT__pc 
            = ((IData)(1U) + vlSelfRef.test_gtext__DOT__unnamedblk9__DOT__pc);
    }
    if ((0U != ([&]() {
                    __Vfunc_test_gtext__DOT__gfx_pixel__70__y = 0x00000039U;
                    __Vfunc_test_gtext__DOT__gfx_pixel__70__x = 0x00000064U;
                    __Vfunc_test_gtext__DOT__gfx_pixel__70__Vfuncout = 0;
                    __Vfunc_test_gtext__DOT__gfx_pixel__70__Vfuncout 
                        = ((0xf9ffU >= (0x0000ffffU 
                                        & (VL_MULS_III(32, (IData)(0x00000140U), __Vfunc_test_gtext__DOT__gfx_pixel__70__y) 
                                           + __Vfunc_test_gtext__DOT__gfx_pixel__70__x)))
                            ? vlSelfRef.test_gtext__DOT__dut__DOT__gfx_inst__DOT__gfx_mem__DOT__mem
                           [(0x0000ffffU & (VL_MULS_III(32, (IData)(0x00000140U), __Vfunc_test_gtext__DOT__gfx_pixel__70__y) 
                                            + __Vfunc_test_gtext__DOT__gfx_pixel__70__x))]
                            : 0U);
                }(), (IData)(__Vfunc_test_gtext__DOT__gfx_pixel__70__Vfuncout)))) {
        vlSelfRef.test_gtext__DOT__unnamedblk9__DOT__pc 
            = ((IData)(1U) + vlSelfRef.test_gtext__DOT__unnamedblk9__DOT__pc);
    }
    if ((0U != ([&]() {
                    __Vfunc_test_gtext__DOT__gfx_pixel__70__y = 0x00000039U;
                    __Vfunc_test_gtext__DOT__gfx_pixel__70__x = 0x00000065U;
                    __Vfunc_test_gtext__DOT__gfx_pixel__70__Vfuncout = 0;
                    __Vfunc_test_gtext__DOT__gfx_pixel__70__Vfuncout 
                        = ((0xf9ffU >= (0x0000ffffU 
                                        & (VL_MULS_III(32, (IData)(0x00000140U), __Vfunc_test_gtext__DOT__gfx_pixel__70__y) 
                                           + __Vfunc_test_gtext__DOT__gfx_pixel__70__x)))
                            ? vlSelfRef.test_gtext__DOT__dut__DOT__gfx_inst__DOT__gfx_mem__DOT__mem
                           [(0x0000ffffU & (VL_MULS_III(32, (IData)(0x00000140U), __Vfunc_test_gtext__DOT__gfx_pixel__70__y) 
                                            + __Vfunc_test_gtext__DOT__gfx_pixel__70__x))]
                            : 0U);
                }(), (IData)(__Vfunc_test_gtext__DOT__gfx_pixel__70__Vfuncout)))) {
        vlSelfRef.test_gtext__DOT__unnamedblk9__DOT__pc 
            = ((IData)(1U) + vlSelfRef.test_gtext__DOT__unnamedblk9__DOT__pc);
    }
    if ((0U != ([&]() {
                    __Vfunc_test_gtext__DOT__gfx_pixel__70__y = 0x00000039U;
                    __Vfunc_test_gtext__DOT__gfx_pixel__70__x = 0x00000066U;
                    __Vfunc_test_gtext__DOT__gfx_pixel__70__Vfuncout = 0;
                    __Vfunc_test_gtext__DOT__gfx_pixel__70__Vfuncout 
                        = ((0xf9ffU >= (0x0000ffffU 
                                        & (VL_MULS_III(32, (IData)(0x00000140U), __Vfunc_test_gtext__DOT__gfx_pixel__70__y) 
                                           + __Vfunc_test_gtext__DOT__gfx_pixel__70__x)))
                            ? vlSelfRef.test_gtext__DOT__dut__DOT__gfx_inst__DOT__gfx_mem__DOT__mem
                           [(0x0000ffffU & (VL_MULS_III(32, (IData)(0x00000140U), __Vfunc_test_gtext__DOT__gfx_pixel__70__y) 
                                            + __Vfunc_test_gtext__DOT__gfx_pixel__70__x))]
                            : 0U);
                }(), (IData)(__Vfunc_test_gtext__DOT__gfx_pixel__70__Vfuncout)))) {
        vlSelfRef.test_gtext__DOT__unnamedblk9__DOT__pc 
            = ((IData)(1U) + vlSelfRef.test_gtext__DOT__unnamedblk9__DOT__pc);
    }
    if ((0U != ([&]() {
                    __Vfunc_test_gtext__DOT__gfx_pixel__70__y = 0x00000039U;
                    __Vfunc_test_gtext__DOT__gfx_pixel__70__x = 0x00000067U;
                    __Vfunc_test_gtext__DOT__gfx_pixel__70__Vfuncout = 0;
                    __Vfunc_test_gtext__DOT__gfx_pixel__70__Vfuncout 
                        = ((0xf9ffU >= (0x0000ffffU 
                                        & (VL_MULS_III(32, (IData)(0x00000140U), __Vfunc_test_gtext__DOT__gfx_pixel__70__y) 
                                           + __Vfunc_test_gtext__DOT__gfx_pixel__70__x)))
                            ? vlSelfRef.test_gtext__DOT__dut__DOT__gfx_inst__DOT__gfx_mem__DOT__mem
                           [(0x0000ffffU & (VL_MULS_III(32, (IData)(0x00000140U), __Vfunc_test_gtext__DOT__gfx_pixel__70__y) 
                                            + __Vfunc_test_gtext__DOT__gfx_pixel__70__x))]
                            : 0U);
                }(), (IData)(__Vfunc_test_gtext__DOT__gfx_pixel__70__Vfuncout)))) {
        vlSelfRef.test_gtext__DOT__unnamedblk9__DOT__pc 
            = ((IData)(1U) + vlSelfRef.test_gtext__DOT__unnamedblk9__DOT__pc);
    }
    if ((0U != ([&]() {
                    __Vfunc_test_gtext__DOT__gfx_pixel__70__y = 0x00000039U;
                    __Vfunc_test_gtext__DOT__gfx_pixel__70__x = 0x00000068U;
                    __Vfunc_test_gtext__DOT__gfx_pixel__70__Vfuncout = 0;
                    __Vfunc_test_gtext__DOT__gfx_pixel__70__Vfuncout 
                        = ((0xf9ffU >= (0x0000ffffU 
                                        & (VL_MULS_III(32, (IData)(0x00000140U), __Vfunc_test_gtext__DOT__gfx_pixel__70__y) 
                                           + __Vfunc_test_gtext__DOT__gfx_pixel__70__x)))
                            ? vlSelfRef.test_gtext__DOT__dut__DOT__gfx_inst__DOT__gfx_mem__DOT__mem
                           [(0x0000ffffU & (VL_MULS_III(32, (IData)(0x00000140U), __Vfunc_test_gtext__DOT__gfx_pixel__70__y) 
                                            + __Vfunc_test_gtext__DOT__gfx_pixel__70__x))]
                            : 0U);
                }(), (IData)(__Vfunc_test_gtext__DOT__gfx_pixel__70__Vfuncout)))) {
        vlSelfRef.test_gtext__DOT__unnamedblk9__DOT__pc 
            = ((IData)(1U) + vlSelfRef.test_gtext__DOT__unnamedblk9__DOT__pc);
    }
    if ((0U != ([&]() {
                    __Vfunc_test_gtext__DOT__gfx_pixel__70__y = 0x00000039U;
                    __Vfunc_test_gtext__DOT__gfx_pixel__70__x = 0x00000069U;
                    __Vfunc_test_gtext__DOT__gfx_pixel__70__Vfuncout = 0;
                    __Vfunc_test_gtext__DOT__gfx_pixel__70__Vfuncout 
                        = ((0xf9ffU >= (0x0000ffffU 
                                        & (VL_MULS_III(32, (IData)(0x00000140U), __Vfunc_test_gtext__DOT__gfx_pixel__70__y) 
                                           + __Vfunc_test_gtext__DOT__gfx_pixel__70__x)))
                            ? vlSelfRef.test_gtext__DOT__dut__DOT__gfx_inst__DOT__gfx_mem__DOT__mem
                           [(0x0000ffffU & (VL_MULS_III(32, (IData)(0x00000140U), __Vfunc_test_gtext__DOT__gfx_pixel__70__y) 
                                            + __Vfunc_test_gtext__DOT__gfx_pixel__70__x))]
                            : 0U);
                }(), (IData)(__Vfunc_test_gtext__DOT__gfx_pixel__70__Vfuncout)))) {
        vlSelfRef.test_gtext__DOT__unnamedblk9__DOT__pc 
            = ((IData)(1U) + vlSelfRef.test_gtext__DOT__unnamedblk9__DOT__pc);
    }
    if ((0U != ([&]() {
                    __Vfunc_test_gtext__DOT__gfx_pixel__70__y = 0x00000039U;
                    __Vfunc_test_gtext__DOT__gfx_pixel__70__x = 0x0000006aU;
                    __Vfunc_test_gtext__DOT__gfx_pixel__70__Vfuncout = 0;
                    __Vfunc_test_gtext__DOT__gfx_pixel__70__Vfuncout 
                        = ((0xf9ffU >= (0x0000ffffU 
                                        & (VL_MULS_III(32, (IData)(0x00000140U), __Vfunc_test_gtext__DOT__gfx_pixel__70__y) 
                                           + __Vfunc_test_gtext__DOT__gfx_pixel__70__x)))
                            ? vlSelfRef.test_gtext__DOT__dut__DOT__gfx_inst__DOT__gfx_mem__DOT__mem
                           [(0x0000ffffU & (VL_MULS_III(32, (IData)(0x00000140U), __Vfunc_test_gtext__DOT__gfx_pixel__70__y) 
                                            + __Vfunc_test_gtext__DOT__gfx_pixel__70__x))]
                            : 0U);
                }(), (IData)(__Vfunc_test_gtext__DOT__gfx_pixel__70__Vfuncout)))) {
        vlSelfRef.test_gtext__DOT__unnamedblk9__DOT__pc 
            = ((IData)(1U) + vlSelfRef.test_gtext__DOT__unnamedblk9__DOT__pc);
    }
    if ((0U != ([&]() {
                    __Vfunc_test_gtext__DOT__gfx_pixel__70__y = 0x00000039U;
                    __Vfunc_test_gtext__DOT__gfx_pixel__70__x = 0x0000006bU;
                    __Vfunc_test_gtext__DOT__gfx_pixel__70__Vfuncout = 0;
                    __Vfunc_test_gtext__DOT__gfx_pixel__70__Vfuncout 
                        = ((0xf9ffU >= (0x0000ffffU 
                                        & (VL_MULS_III(32, (IData)(0x00000140U), __Vfunc_test_gtext__DOT__gfx_pixel__70__y) 
                                           + __Vfunc_test_gtext__DOT__gfx_pixel__70__x)))
                            ? vlSelfRef.test_gtext__DOT__dut__DOT__gfx_inst__DOT__gfx_mem__DOT__mem
                           [(0x0000ffffU & (VL_MULS_III(32, (IData)(0x00000140U), __Vfunc_test_gtext__DOT__gfx_pixel__70__y) 
                                            + __Vfunc_test_gtext__DOT__gfx_pixel__70__x))]
                            : 0U);
                }(), (IData)(__Vfunc_test_gtext__DOT__gfx_pixel__70__Vfuncout)))) {
        vlSelfRef.test_gtext__DOT__unnamedblk9__DOT__pc 
            = ((IData)(1U) + vlSelfRef.test_gtext__DOT__unnamedblk9__DOT__pc);
    }
    __Vtask_test_gtext__DOT__check__71__condition = 
        VL_LTS_III(32, 0U, vlSelfRef.test_gtext__DOT__unnamedblk9__DOT__pc);
    vlSelfRef.__Vtask_test_gtext__DOT__check__71__name = "'X' at (100,50) has pixels"s;
    vlSelfRef.test_gtext__DOT__test_num = ((IData)(1U) 
                                           + vlSelfRef.test_gtext__DOT__test_num);
    if (__Vtask_test_gtext__DOT__check__71__condition) {
        VL_WRITEF_NX("  PASS [%0d] %@\n",0,32,vlSelfRef.test_gtext__DOT__test_num,
                     -1,&(vlSelfRef.__Vtask_test_gtext__DOT__check__71__name));
        vlSelfRef.test_gtext__DOT__pass_count = ((IData)(1U) 
                                                 + vlSelfRef.test_gtext__DOT__pass_count);
    } else {
        VL_WRITEF_NX("  FAIL [%0d] %@\n",0,32,vlSelfRef.test_gtext__DOT__test_num,
                     -1,&(vlSelfRef.__Vtask_test_gtext__DOT__check__71__name));
        vlSelfRef.test_gtext__DOT__fail_count = ((IData)(1U) 
                                                 + vlSelfRef.test_gtext__DOT__fail_count);
    }
    VL_WRITEF_NX("Test: Scale factor 2x\n",0);
    __Vtask_test_gtext__DOT__write_cmd__72__cmd = 7U;
    __Vtask_test_gtext__DOT__write_reg__73__data = __Vtask_test_gtext__DOT__write_cmd__72__cmd;
    __Vtask_test_gtext__DOT__write_reg__73__addr = 0xa010U;
    Vtest_gtext___024root____VbeforeTrig_h8248a36b__0(vlSelf, 
                                                      "@(posedge test_gtext.clk)");
    co_await vlSelfRef.__VtrigSched_h8248a36b__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge test_gtext.clk)", 
                                                         "test_gtext.sv", 
                                                         49);
    vlSelfRef.test_gtext__DOT__cpu_addr = __Vtask_test_gtext__DOT__write_reg__73__addr;
    vlSelfRef.test_gtext__DOT__cpu_wdata = __Vtask_test_gtext__DOT__write_reg__73__data;
    vlSelfRef.test_gtext__DOT__cpu_we = 1U;
    vlSelfRef.test_gtext__DOT__cpu_ce = 1U;
    Vtest_gtext___024root____VbeforeTrig_h8248a36b__0(vlSelf, 
                                                      "@(posedge test_gtext.clk)");
    co_await vlSelfRef.__VtrigSched_h8248a36b__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge test_gtext.clk)", 
                                                         "test_gtext.sv", 
                                                         54);
    vlSelfRef.test_gtext__DOT__cpu_we = 0U;
    vlSelfRef.test_gtext__DOT__cpu_ce = 0U;
    __Vtask_test_gtext__DOT__wait_cmd_done__74__test_gtext__DOT__unnamedblk1_1__DOT____Vrepeat0 = 0;
    __Vtask_test_gtext__DOT__wait_cmd_done__74__timeout = 0U;
    __Vtask_test_gtext__DOT__wait_cmd_done__74__timeout = 0U;
    __Vtask_test_gtext__DOT__wait_cmd_done__74__test_gtext__DOT__unnamedblk1_1__DOT____Vrepeat0 = 2U;
    while (VL_LTS_III(32, 0U, __Vtask_test_gtext__DOT__wait_cmd_done__74__test_gtext__DOT__unnamedblk1_1__DOT____Vrepeat0)) {
        Vtest_gtext___024root____VbeforeTrig_h8248a36b__0(vlSelf, 
                                                          "@(posedge test_gtext.clk)");
        co_await vlSelfRef.__VtrigSched_h8248a36b__0.trigger(0U, 
                                                             nullptr, 
                                                             "@(posedge test_gtext.clk)", 
                                                             "test_gtext.sv", 
                                                             71);
        __Vtask_test_gtext__DOT__wait_cmd_done__74__test_gtext__DOT__unnamedblk1_1__DOT____Vrepeat0 
            = (__Vtask_test_gtext__DOT__wait_cmd_done__74__test_gtext__DOT__unnamedblk1_1__DOT____Vrepeat0 
               - (IData)(1U));
    }
    while ((((IData)(vlSelfRef.test_gtext__DOT__dut__DOT__cmd_busy) 
             | (IData)(vlSelfRef.test_gtext__DOT__dut__DOT__artist_busy)) 
            & VL_GTS_III(32, 0x0007a120U, __Vtask_test_gtext__DOT__wait_cmd_done__74__timeout))) {
        Vtest_gtext___024root____VbeforeTrig_h8248a36b__0(vlSelf, 
                                                          "@(posedge test_gtext.clk)");
        co_await vlSelfRef.__VtrigSched_h8248a36b__0.trigger(0U, 
                                                             nullptr, 
                                                             "@(posedge test_gtext.clk)", 
                                                             "test_gtext.sv", 
                                                             73);
        __Vtask_test_gtext__DOT__wait_cmd_done__74__timeout 
            = ((IData)(1U) + __Vtask_test_gtext__DOT__wait_cmd_done__74__timeout);
    }
    vlSelfRef.__Vtask_test_gtext__DOT__set_fio_string__75__s = "I"s;
    __Vtask_test_gtext__DOT__set_fio_string__75__i = 0U;
    __Vtask_test_gtext__DOT__write_reg__76__data = 
        (0x000000ffU & VL_LEN_IN(vlSelfRef.__Vtask_test_gtext__DOT__set_fio_string__75__s));
    __Vtask_test_gtext__DOT__write_reg__76__addr = 0xb9a3U;
    Vtest_gtext___024root____VbeforeTrig_h8248a36b__0(vlSelf, 
                                                      "@(posedge test_gtext.clk)");
    co_await vlSelfRef.__VtrigSched_h8248a36b__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge test_gtext.clk)", 
                                                         "test_gtext.sv", 
                                                         49);
    vlSelfRef.test_gtext__DOT__cpu_addr = __Vtask_test_gtext__DOT__write_reg__76__addr;
    vlSelfRef.test_gtext__DOT__cpu_wdata = __Vtask_test_gtext__DOT__write_reg__76__data;
    vlSelfRef.test_gtext__DOT__cpu_we = 1U;
    vlSelfRef.test_gtext__DOT__cpu_ce = 1U;
    Vtest_gtext___024root____VbeforeTrig_h8248a36b__0(vlSelf, 
                                                      "@(posedge test_gtext.clk)");
    co_await vlSelfRef.__VtrigSched_h8248a36b__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge test_gtext.clk)", 
                                                         "test_gtext.sv", 
                                                         54);
    vlSelfRef.test_gtext__DOT__cpu_we = 0U;
    vlSelfRef.test_gtext__DOT__cpu_ce = 0U;
    __Vtask_test_gtext__DOT__set_fio_string__75__i = 0U;
    while (VL_LTS_III(32, __Vtask_test_gtext__DOT__set_fio_string__75__i, 
                      VL_LEN_IN(vlSelfRef.__Vtask_test_gtext__DOT__set_fio_string__75__s))) {
        __Vtask_test_gtext__DOT__write_reg__77__data 
            = VL_GETC_N(vlSelfRef.__Vtask_test_gtext__DOT__set_fio_string__75__s,__Vtask_test_gtext__DOT__set_fio_string__75__i);
        __Vtask_test_gtext__DOT__write_reg__77__addr 
            = (0x0000ffffU & ((IData)(0xb9b0U) + __Vtask_test_gtext__DOT__set_fio_string__75__i));
        Vtest_gtext___024root____VbeforeTrig_h8248a36b__0(vlSelf, 
                                                          "@(posedge test_gtext.clk)");
        co_await vlSelfRef.__VtrigSched_h8248a36b__0.trigger(0U, 
                                                             nullptr, 
                                                             "@(posedge test_gtext.clk)", 
                                                             "test_gtext.sv", 
                                                             49);
        vlSelfRef.test_gtext__DOT__cpu_addr = __Vtask_test_gtext__DOT__write_reg__77__addr;
        vlSelfRef.test_gtext__DOT__cpu_wdata = __Vtask_test_gtext__DOT__write_reg__77__data;
        vlSelfRef.test_gtext__DOT__cpu_we = 1U;
        vlSelfRef.test_gtext__DOT__cpu_ce = 1U;
        Vtest_gtext___024root____VbeforeTrig_h8248a36b__0(vlSelf, 
                                                          "@(posedge test_gtext.clk)");
        co_await vlSelfRef.__VtrigSched_h8248a36b__0.trigger(0U, 
                                                             nullptr, 
                                                             "@(posedge test_gtext.clk)", 
                                                             "test_gtext.sv", 
                                                             54);
        vlSelfRef.test_gtext__DOT__cpu_we = 0U;
        vlSelfRef.test_gtext__DOT__cpu_ce = 0U;
        __Vtask_test_gtext__DOT__set_fio_string__75__i 
            = ((IData)(1U) + __Vtask_test_gtext__DOT__set_fio_string__75__i);
    }
    __Vtask_test_gtext__DOT__write_param__78__data = 0U;
    __Vtask_test_gtext__DOT__write_param__78__idx = 0U;
    __Vtask_test_gtext__DOT__write_reg__79__data = __Vtask_test_gtext__DOT__write_param__78__data;
    __Vtask_test_gtext__DOT__write_reg__79__addr = 
        (0x0000ffffU & ((IData)(0xa011U) + __Vtask_test_gtext__DOT__write_param__78__idx));
    Vtest_gtext___024root____VbeforeTrig_h8248a36b__0(vlSelf, 
                                                      "@(posedge test_gtext.clk)");
    co_await vlSelfRef.__VtrigSched_h8248a36b__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge test_gtext.clk)", 
                                                         "test_gtext.sv", 
                                                         49);
    vlSelfRef.test_gtext__DOT__cpu_addr = __Vtask_test_gtext__DOT__write_reg__79__addr;
    vlSelfRef.test_gtext__DOT__cpu_wdata = __Vtask_test_gtext__DOT__write_reg__79__data;
    vlSelfRef.test_gtext__DOT__cpu_we = 1U;
    vlSelfRef.test_gtext__DOT__cpu_ce = 1U;
    Vtest_gtext___024root____VbeforeTrig_h8248a36b__0(vlSelf, 
                                                      "@(posedge test_gtext.clk)");
    co_await vlSelfRef.__VtrigSched_h8248a36b__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge test_gtext.clk)", 
                                                         "test_gtext.sv", 
                                                         54);
    vlSelfRef.test_gtext__DOT__cpu_we = 0U;
    vlSelfRef.test_gtext__DOT__cpu_ce = 0U;
    __Vtask_test_gtext__DOT__write_param__80__data = 0U;
    __Vtask_test_gtext__DOT__write_param__80__idx = 1U;
    __Vtask_test_gtext__DOT__write_reg__81__data = __Vtask_test_gtext__DOT__write_param__80__data;
    __Vtask_test_gtext__DOT__write_reg__81__addr = 
        (0x0000ffffU & ((IData)(0xa011U) + __Vtask_test_gtext__DOT__write_param__80__idx));
    Vtest_gtext___024root____VbeforeTrig_h8248a36b__0(vlSelf, 
                                                      "@(posedge test_gtext.clk)");
    co_await vlSelfRef.__VtrigSched_h8248a36b__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge test_gtext.clk)", 
                                                         "test_gtext.sv", 
                                                         49);
    vlSelfRef.test_gtext__DOT__cpu_addr = __Vtask_test_gtext__DOT__write_reg__81__addr;
    vlSelfRef.test_gtext__DOT__cpu_wdata = __Vtask_test_gtext__DOT__write_reg__81__data;
    vlSelfRef.test_gtext__DOT__cpu_we = 1U;
    vlSelfRef.test_gtext__DOT__cpu_ce = 1U;
    Vtest_gtext___024root____VbeforeTrig_h8248a36b__0(vlSelf, 
                                                      "@(posedge test_gtext.clk)");
    co_await vlSelfRef.__VtrigSched_h8248a36b__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge test_gtext.clk)", 
                                                         "test_gtext.sv", 
                                                         54);
    vlSelfRef.test_gtext__DOT__cpu_we = 0U;
    vlSelfRef.test_gtext__DOT__cpu_ce = 0U;
    __Vtask_test_gtext__DOT__write_param__82__data = 0U;
    __Vtask_test_gtext__DOT__write_param__82__idx = 2U;
    __Vtask_test_gtext__DOT__write_reg__83__data = __Vtask_test_gtext__DOT__write_param__82__data;
    __Vtask_test_gtext__DOT__write_reg__83__addr = 
        (0x0000ffffU & ((IData)(0xa011U) + __Vtask_test_gtext__DOT__write_param__82__idx));
    Vtest_gtext___024root____VbeforeTrig_h8248a36b__0(vlSelf, 
                                                      "@(posedge test_gtext.clk)");
    co_await vlSelfRef.__VtrigSched_h8248a36b__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge test_gtext.clk)", 
                                                         "test_gtext.sv", 
                                                         49);
    vlSelfRef.test_gtext__DOT__cpu_addr = __Vtask_test_gtext__DOT__write_reg__83__addr;
    vlSelfRef.test_gtext__DOT__cpu_wdata = __Vtask_test_gtext__DOT__write_reg__83__data;
    vlSelfRef.test_gtext__DOT__cpu_we = 1U;
    vlSelfRef.test_gtext__DOT__cpu_ce = 1U;
    Vtest_gtext___024root____VbeforeTrig_h8248a36b__0(vlSelf, 
                                                      "@(posedge test_gtext.clk)");
    co_await vlSelfRef.__VtrigSched_h8248a36b__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge test_gtext.clk)", 
                                                         "test_gtext.sv", 
                                                         54);
    vlSelfRef.test_gtext__DOT__cpu_we = 0U;
    vlSelfRef.test_gtext__DOT__cpu_ce = 0U;
    __Vtask_test_gtext__DOT__write_param__84__data = 0U;
    __Vtask_test_gtext__DOT__write_param__84__idx = 3U;
    __Vtask_test_gtext__DOT__write_reg__85__data = __Vtask_test_gtext__DOT__write_param__84__data;
    __Vtask_test_gtext__DOT__write_reg__85__addr = 
        (0x0000ffffU & ((IData)(0xa011U) + __Vtask_test_gtext__DOT__write_param__84__idx));
    Vtest_gtext___024root____VbeforeTrig_h8248a36b__0(vlSelf, 
                                                      "@(posedge test_gtext.clk)");
    co_await vlSelfRef.__VtrigSched_h8248a36b__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge test_gtext.clk)", 
                                                         "test_gtext.sv", 
                                                         49);
    vlSelfRef.test_gtext__DOT__cpu_addr = __Vtask_test_gtext__DOT__write_reg__85__addr;
    vlSelfRef.test_gtext__DOT__cpu_wdata = __Vtask_test_gtext__DOT__write_reg__85__data;
    vlSelfRef.test_gtext__DOT__cpu_we = 1U;
    vlSelfRef.test_gtext__DOT__cpu_ce = 1U;
    Vtest_gtext___024root____VbeforeTrig_h8248a36b__0(vlSelf, 
                                                      "@(posedge test_gtext.clk)");
    co_await vlSelfRef.__VtrigSched_h8248a36b__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge test_gtext.clk)", 
                                                         "test_gtext.sv", 
                                                         54);
    vlSelfRef.test_gtext__DOT__cpu_we = 0U;
    vlSelfRef.test_gtext__DOT__cpu_ce = 0U;
    __Vtask_test_gtext__DOT__write_param__86__data = 0U;
    __Vtask_test_gtext__DOT__write_param__86__idx = 4U;
    __Vtask_test_gtext__DOT__write_reg__87__data = __Vtask_test_gtext__DOT__write_param__86__data;
    __Vtask_test_gtext__DOT__write_reg__87__addr = 
        (0x0000ffffU & ((IData)(0xa011U) + __Vtask_test_gtext__DOT__write_param__86__idx));
    Vtest_gtext___024root____VbeforeTrig_h8248a36b__0(vlSelf, 
                                                      "@(posedge test_gtext.clk)");
    co_await vlSelfRef.__VtrigSched_h8248a36b__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge test_gtext.clk)", 
                                                         "test_gtext.sv", 
                                                         49);
    vlSelfRef.test_gtext__DOT__cpu_addr = __Vtask_test_gtext__DOT__write_reg__87__addr;
    vlSelfRef.test_gtext__DOT__cpu_wdata = __Vtask_test_gtext__DOT__write_reg__87__data;
    vlSelfRef.test_gtext__DOT__cpu_we = 1U;
    vlSelfRef.test_gtext__DOT__cpu_ce = 1U;
    Vtest_gtext___024root____VbeforeTrig_h8248a36b__0(vlSelf, 
                                                      "@(posedge test_gtext.clk)");
    co_await vlSelfRef.__VtrigSched_h8248a36b__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge test_gtext.clk)", 
                                                         "test_gtext.sv", 
                                                         54);
    vlSelfRef.test_gtext__DOT__cpu_we = 0U;
    vlSelfRef.test_gtext__DOT__cpu_ce = 0U;
    __Vtask_test_gtext__DOT__write_param__88__data = 2U;
    __Vtask_test_gtext__DOT__write_param__88__idx = 5U;
    __Vtask_test_gtext__DOT__write_reg__89__data = __Vtask_test_gtext__DOT__write_param__88__data;
    __Vtask_test_gtext__DOT__write_reg__89__addr = 
        (0x0000ffffU & ((IData)(0xa011U) + __Vtask_test_gtext__DOT__write_param__88__idx));
    Vtest_gtext___024root____VbeforeTrig_h8248a36b__0(vlSelf, 
                                                      "@(posedge test_gtext.clk)");
    co_await vlSelfRef.__VtrigSched_h8248a36b__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge test_gtext.clk)", 
                                                         "test_gtext.sv", 
                                                         49);
    vlSelfRef.test_gtext__DOT__cpu_addr = __Vtask_test_gtext__DOT__write_reg__89__addr;
    vlSelfRef.test_gtext__DOT__cpu_wdata = __Vtask_test_gtext__DOT__write_reg__89__data;
    vlSelfRef.test_gtext__DOT__cpu_we = 1U;
    vlSelfRef.test_gtext__DOT__cpu_ce = 1U;
    Vtest_gtext___024root____VbeforeTrig_h8248a36b__0(vlSelf, 
                                                      "@(posedge test_gtext.clk)");
    co_await vlSelfRef.__VtrigSched_h8248a36b__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge test_gtext.clk)", 
                                                         "test_gtext.sv", 
                                                         54);
    vlSelfRef.test_gtext__DOT__cpu_we = 0U;
    vlSelfRef.test_gtext__DOT__cpu_ce = 0U;
    __Vtask_test_gtext__DOT__write_cmd__90__cmd = 0x0aU;
    __Vtask_test_gtext__DOT__write_reg__91__data = __Vtask_test_gtext__DOT__write_cmd__90__cmd;
    __Vtask_test_gtext__DOT__write_reg__91__addr = 0xa010U;
    Vtest_gtext___024root____VbeforeTrig_h8248a36b__0(vlSelf, 
                                                      "@(posedge test_gtext.clk)");
    co_await vlSelfRef.__VtrigSched_h8248a36b__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge test_gtext.clk)", 
                                                         "test_gtext.sv", 
                                                         49);
    vlSelfRef.test_gtext__DOT__cpu_addr = __Vtask_test_gtext__DOT__write_reg__91__addr;
    vlSelfRef.test_gtext__DOT__cpu_wdata = __Vtask_test_gtext__DOT__write_reg__91__data;
    vlSelfRef.test_gtext__DOT__cpu_we = 1U;
    vlSelfRef.test_gtext__DOT__cpu_ce = 1U;
    Vtest_gtext___024root____VbeforeTrig_h8248a36b__0(vlSelf, 
                                                      "@(posedge test_gtext.clk)");
    co_await vlSelfRef.__VtrigSched_h8248a36b__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge test_gtext.clk)", 
                                                         "test_gtext.sv", 
                                                         54);
    vlSelfRef.test_gtext__DOT__cpu_we = 0U;
    vlSelfRef.test_gtext__DOT__cpu_ce = 0U;
    __Vtask_test_gtext__DOT__wait_cmd_done__92__test_gtext__DOT__unnamedblk1_1__DOT____Vrepeat0 = 0;
    __Vtask_test_gtext__DOT__wait_cmd_done__92__timeout = 0U;
    __Vtask_test_gtext__DOT__wait_cmd_done__92__timeout = 0U;
    __Vtask_test_gtext__DOT__wait_cmd_done__92__test_gtext__DOT__unnamedblk1_1__DOT____Vrepeat0 = 2U;
    while (VL_LTS_III(32, 0U, __Vtask_test_gtext__DOT__wait_cmd_done__92__test_gtext__DOT__unnamedblk1_1__DOT____Vrepeat0)) {
        Vtest_gtext___024root____VbeforeTrig_h8248a36b__0(vlSelf, 
                                                          "@(posedge test_gtext.clk)");
        co_await vlSelfRef.__VtrigSched_h8248a36b__0.trigger(0U, 
                                                             nullptr, 
                                                             "@(posedge test_gtext.clk)", 
                                                             "test_gtext.sv", 
                                                             71);
        __Vtask_test_gtext__DOT__wait_cmd_done__92__test_gtext__DOT__unnamedblk1_1__DOT____Vrepeat0 
            = (__Vtask_test_gtext__DOT__wait_cmd_done__92__test_gtext__DOT__unnamedblk1_1__DOT____Vrepeat0 
               - (IData)(1U));
    }
    while ((((IData)(vlSelfRef.test_gtext__DOT__dut__DOT__cmd_busy) 
             | (IData)(vlSelfRef.test_gtext__DOT__dut__DOT__artist_busy)) 
            & VL_GTS_III(32, 0x0007a120U, __Vtask_test_gtext__DOT__wait_cmd_done__92__timeout))) {
        Vtest_gtext___024root____VbeforeTrig_h8248a36b__0(vlSelf, 
                                                          "@(posedge test_gtext.clk)");
        co_await vlSelfRef.__VtrigSched_h8248a36b__0.trigger(0U, 
                                                             nullptr, 
                                                             "@(posedge test_gtext.clk)", 
                                                             "test_gtext.sv", 
                                                             73);
        __Vtask_test_gtext__DOT__wait_cmd_done__92__timeout 
            = ((IData)(1U) + __Vtask_test_gtext__DOT__wait_cmd_done__92__timeout);
    }
    vlSelfRef.test_gtext__DOT__unnamedblk12__DOT__pc = 0U;
    if ((0U != ([&]() {
                    vlSelfRef.__Vfunc_test_gtext__DOT__gfx_pixel__93__y = 0U;
                    vlSelfRef.__Vfunc_test_gtext__DOT__gfx_pixel__93__x = 0U;
                    vlSelf->__Vfunc_test_gtext__DOT__gfx_pixel__93__Vfuncout = 0;
                    vlSelfRef.__Vfunc_test_gtext__DOT__gfx_pixel__93__Vfuncout 
                        = ((0xf9ffU >= (0x0000ffffU 
                                        & (VL_MULS_III(32, (IData)(0x00000140U), vlSelfRef.__Vfunc_test_gtext__DOT__gfx_pixel__93__y) 
                                           + vlSelfRef.__Vfunc_test_gtext__DOT__gfx_pixel__93__x)))
                            ? vlSelfRef.test_gtext__DOT__dut__DOT__gfx_inst__DOT__gfx_mem__DOT__mem
                           [(0x0000ffffU & (VL_MULS_III(32, (IData)(0x00000140U), vlSelfRef.__Vfunc_test_gtext__DOT__gfx_pixel__93__y) 
                                            + vlSelfRef.__Vfunc_test_gtext__DOT__gfx_pixel__93__x))]
                            : 0U);
                }(), (IData)(vlSelfRef.__Vfunc_test_gtext__DOT__gfx_pixel__93__Vfuncout)))) {
        vlSelfRef.test_gtext__DOT__unnamedblk12__DOT__pc 
            = ((IData)(1U) + vlSelfRef.test_gtext__DOT__unnamedblk12__DOT__pc);
    }
    if ((0U != ([&]() {
                    vlSelfRef.__Vfunc_test_gtext__DOT__gfx_pixel__93__y = 0U;
                    vlSelfRef.__Vfunc_test_gtext__DOT__gfx_pixel__93__x = 1U;
                    vlSelf->__Vfunc_test_gtext__DOT__gfx_pixel__93__Vfuncout = 0;
                    vlSelfRef.__Vfunc_test_gtext__DOT__gfx_pixel__93__Vfuncout 
                        = ((0xf9ffU >= (0x0000ffffU 
                                        & (VL_MULS_III(32, (IData)(0x00000140U), vlSelfRef.__Vfunc_test_gtext__DOT__gfx_pixel__93__y) 
                                           + vlSelfRef.__Vfunc_test_gtext__DOT__gfx_pixel__93__x)))
                            ? vlSelfRef.test_gtext__DOT__dut__DOT__gfx_inst__DOT__gfx_mem__DOT__mem
                           [(0x0000ffffU & (VL_MULS_III(32, (IData)(0x00000140U), vlSelfRef.__Vfunc_test_gtext__DOT__gfx_pixel__93__y) 
                                            + vlSelfRef.__Vfunc_test_gtext__DOT__gfx_pixel__93__x))]
                            : 0U);
                }(), (IData)(vlSelfRef.__Vfunc_test_gtext__DOT__gfx_pixel__93__Vfuncout)))) {
        vlSelfRef.test_gtext__DOT__unnamedblk12__DOT__pc 
            = ((IData)(1U) + vlSelfRef.test_gtext__DOT__unnamedblk12__DOT__pc);
    }
    if ((0U != ([&]() {
                    vlSelfRef.__Vfunc_test_gtext__DOT__gfx_pixel__93__y = 0U;
                    vlSelfRef.__Vfunc_test_gtext__DOT__gfx_pixel__93__x = 2U;
                    vlSelf->__Vfunc_test_gtext__DOT__gfx_pixel__93__Vfuncout = 0;
                    vlSelfRef.__Vfunc_test_gtext__DOT__gfx_pixel__93__Vfuncout 
                        = ((0xf9ffU >= (0x0000ffffU 
                                        & (VL_MULS_III(32, (IData)(0x00000140U), vlSelfRef.__Vfunc_test_gtext__DOT__gfx_pixel__93__y) 
                                           + vlSelfRef.__Vfunc_test_gtext__DOT__gfx_pixel__93__x)))
                            ? vlSelfRef.test_gtext__DOT__dut__DOT__gfx_inst__DOT__gfx_mem__DOT__mem
                           [(0x0000ffffU & (VL_MULS_III(32, (IData)(0x00000140U), vlSelfRef.__Vfunc_test_gtext__DOT__gfx_pixel__93__y) 
                                            + vlSelfRef.__Vfunc_test_gtext__DOT__gfx_pixel__93__x))]
                            : 0U);
                }(), (IData)(vlSelfRef.__Vfunc_test_gtext__DOT__gfx_pixel__93__Vfuncout)))) {
        vlSelfRef.test_gtext__DOT__unnamedblk12__DOT__pc 
            = ((IData)(1U) + vlSelfRef.test_gtext__DOT__unnamedblk12__DOT__pc);
    }
    if ((0U != ([&]() {
                    vlSelfRef.__Vfunc_test_gtext__DOT__gfx_pixel__93__y = 0U;
                    vlSelfRef.__Vfunc_test_gtext__DOT__gfx_pixel__93__x = 3U;
                    vlSelf->__Vfunc_test_gtext__DOT__gfx_pixel__93__Vfuncout = 0;
                    vlSelfRef.__Vfunc_test_gtext__DOT__gfx_pixel__93__Vfuncout 
                        = ((0xf9ffU >= (0x0000ffffU 
                                        & (VL_MULS_III(32, (IData)(0x00000140U), vlSelfRef.__Vfunc_test_gtext__DOT__gfx_pixel__93__y) 
                                           + vlSelfRef.__Vfunc_test_gtext__DOT__gfx_pixel__93__x)))
                            ? vlSelfRef.test_gtext__DOT__dut__DOT__gfx_inst__DOT__gfx_mem__DOT__mem
                           [(0x0000ffffU & (VL_MULS_III(32, (IData)(0x00000140U), vlSelfRef.__Vfunc_test_gtext__DOT__gfx_pixel__93__y) 
                                            + vlSelfRef.__Vfunc_test_gtext__DOT__gfx_pixel__93__x))]
                            : 0U);
                }(), (IData)(vlSelfRef.__Vfunc_test_gtext__DOT__gfx_pixel__93__Vfuncout)))) {
        vlSelfRef.test_gtext__DOT__unnamedblk12__DOT__pc 
            = ((IData)(1U) + vlSelfRef.test_gtext__DOT__unnamedblk12__DOT__pc);
    }
    if ((0U != ([&]() {
                    vlSelfRef.__Vfunc_test_gtext__DOT__gfx_pixel__93__y = 0U;
                    vlSelfRef.__Vfunc_test_gtext__DOT__gfx_pixel__93__x = 4U;
                    vlSelf->__Vfunc_test_gtext__DOT__gfx_pixel__93__Vfuncout = 0;
                    vlSelfRef.__Vfunc_test_gtext__DOT__gfx_pixel__93__Vfuncout 
                        = ((0xf9ffU >= (0x0000ffffU 
                                        & (VL_MULS_III(32, (IData)(0x00000140U), vlSelfRef.__Vfunc_test_gtext__DOT__gfx_pixel__93__y) 
                                           + vlSelfRef.__Vfunc_test_gtext__DOT__gfx_pixel__93__x)))
                            ? vlSelfRef.test_gtext__DOT__dut__DOT__gfx_inst__DOT__gfx_mem__DOT__mem
                           [(0x0000ffffU & (VL_MULS_III(32, (IData)(0x00000140U), vlSelfRef.__Vfunc_test_gtext__DOT__gfx_pixel__93__y) 
                                            + vlSelfRef.__Vfunc_test_gtext__DOT__gfx_pixel__93__x))]
                            : 0U);
                }(), (IData)(vlSelfRef.__Vfunc_test_gtext__DOT__gfx_pixel__93__Vfuncout)))) {
        vlSelfRef.test_gtext__DOT__unnamedblk12__DOT__pc 
            = ((IData)(1U) + vlSelfRef.test_gtext__DOT__unnamedblk12__DOT__pc);
    }
    if ((0U != ([&]() {
                    vlSelfRef.__Vfunc_test_gtext__DOT__gfx_pixel__93__y = 0U;
                    vlSelfRef.__Vfunc_test_gtext__DOT__gfx_pixel__93__x = 5U;
                    vlSelf->__Vfunc_test_gtext__DOT__gfx_pixel__93__Vfuncout = 0;
                    vlSelfRef.__Vfunc_test_gtext__DOT__gfx_pixel__93__Vfuncout 
                        = ((0xf9ffU >= (0x0000ffffU 
                                        & (VL_MULS_III(32, (IData)(0x00000140U), vlSelfRef.__Vfunc_test_gtext__DOT__gfx_pixel__93__y) 
                                           + vlSelfRef.__Vfunc_test_gtext__DOT__gfx_pixel__93__x)))
                            ? vlSelfRef.test_gtext__DOT__dut__DOT__gfx_inst__DOT__gfx_mem__DOT__mem
                           [(0x0000ffffU & (VL_MULS_III(32, (IData)(0x00000140U), vlSelfRef.__Vfunc_test_gtext__DOT__gfx_pixel__93__y) 
                                            + vlSelfRef.__Vfunc_test_gtext__DOT__gfx_pixel__93__x))]
                            : 0U);
                }(), (IData)(vlSelfRef.__Vfunc_test_gtext__DOT__gfx_pixel__93__Vfuncout)))) {
        vlSelfRef.test_gtext__DOT__unnamedblk12__DOT__pc 
            = ((IData)(1U) + vlSelfRef.test_gtext__DOT__unnamedblk12__DOT__pc);
    }
    if ((0U != ([&]() {
                    vlSelfRef.__Vfunc_test_gtext__DOT__gfx_pixel__93__y = 0U;
                    vlSelfRef.__Vfunc_test_gtext__DOT__gfx_pixel__93__x = 6U;
                    vlSelf->__Vfunc_test_gtext__DOT__gfx_pixel__93__Vfuncout = 0;
                    vlSelfRef.__Vfunc_test_gtext__DOT__gfx_pixel__93__Vfuncout 
                        = ((0xf9ffU >= (0x0000ffffU 
                                        & (VL_MULS_III(32, (IData)(0x00000140U), vlSelfRef.__Vfunc_test_gtext__DOT__gfx_pixel__93__y) 
                                           + vlSelfRef.__Vfunc_test_gtext__DOT__gfx_pixel__93__x)))
                            ? vlSelfRef.test_gtext__DOT__dut__DOT__gfx_inst__DOT__gfx_mem__DOT__mem
                           [(0x0000ffffU & (VL_MULS_III(32, (IData)(0x00000140U), vlSelfRef.__Vfunc_test_gtext__DOT__gfx_pixel__93__y) 
                                            + vlSelfRef.__Vfunc_test_gtext__DOT__gfx_pixel__93__x))]
                            : 0U);
                }(), (IData)(vlSelfRef.__Vfunc_test_gtext__DOT__gfx_pixel__93__Vfuncout)))) {
        vlSelfRef.test_gtext__DOT__unnamedblk12__DOT__pc 
            = ((IData)(1U) + vlSelfRef.test_gtext__DOT__unnamedblk12__DOT__pc);
    }
    if ((0U != ([&]() {
                    vlSelfRef.__Vfunc_test_gtext__DOT__gfx_pixel__93__y = 0U;
                    vlSelfRef.__Vfunc_test_gtext__DOT__gfx_pixel__93__x = 7U;
                    vlSelf->__Vfunc_test_gtext__DOT__gfx_pixel__93__Vfuncout = 0;
                    vlSelfRef.__Vfunc_test_gtext__DOT__gfx_pixel__93__Vfuncout 
                        = ((0xf9ffU >= (0x0000ffffU 
                                        & (VL_MULS_III(32, (IData)(0x00000140U), vlSelfRef.__Vfunc_test_gtext__DOT__gfx_pixel__93__y) 
                                           + vlSelfRef.__Vfunc_test_gtext__DOT__gfx_pixel__93__x)))
                            ? vlSelfRef.test_gtext__DOT__dut__DOT__gfx_inst__DOT__gfx_mem__DOT__mem
                           [(0x0000ffffU & (VL_MULS_III(32, (IData)(0x00000140U), vlSelfRef.__Vfunc_test_gtext__DOT__gfx_pixel__93__y) 
                                            + vlSelfRef.__Vfunc_test_gtext__DOT__gfx_pixel__93__x))]
                            : 0U);
                }(), (IData)(vlSelfRef.__Vfunc_test_gtext__DOT__gfx_pixel__93__Vfuncout)))) {
        vlSelfRef.test_gtext__DOT__unnamedblk12__DOT__pc 
            = ((IData)(1U) + vlSelfRef.test_gtext__DOT__unnamedblk12__DOT__pc);
    }
    if ((0U != ([&]() {
                    vlSelfRef.__Vfunc_test_gtext__DOT__gfx_pixel__93__y = 0U;
                    vlSelfRef.__Vfunc_test_gtext__DOT__gfx_pixel__93__x = 8U;
                    vlSelf->__Vfunc_test_gtext__DOT__gfx_pixel__93__Vfuncout = 0;
                    vlSelfRef.__Vfunc_test_gtext__DOT__gfx_pixel__93__Vfuncout 
                        = ((0xf9ffU >= (0x0000ffffU 
                                        & (VL_MULS_III(32, (IData)(0x00000140U), vlSelfRef.__Vfunc_test_gtext__DOT__gfx_pixel__93__y) 
                                           + vlSelfRef.__Vfunc_test_gtext__DOT__gfx_pixel__93__x)))
                            ? vlSelfRef.test_gtext__DOT__dut__DOT__gfx_inst__DOT__gfx_mem__DOT__mem
                           [(0x0000ffffU & (VL_MULS_III(32, (IData)(0x00000140U), vlSelfRef.__Vfunc_test_gtext__DOT__gfx_pixel__93__y) 
                                            + vlSelfRef.__Vfunc_test_gtext__DOT__gfx_pixel__93__x))]
                            : 0U);
                }(), (IData)(vlSelfRef.__Vfunc_test_gtext__DOT__gfx_pixel__93__Vfuncout)))) {
        vlSelfRef.test_gtext__DOT__unnamedblk12__DOT__pc 
            = ((IData)(1U) + vlSelfRef.test_gtext__DOT__unnamedblk12__DOT__pc);
    }
    if ((0U != ([&]() {
                    vlSelfRef.__Vfunc_test_gtext__DOT__gfx_pixel__93__y = 0U;
                    vlSelfRef.__Vfunc_test_gtext__DOT__gfx_pixel__93__x = 9U;
                    vlSelf->__Vfunc_test_gtext__DOT__gfx_pixel__93__Vfuncout = 0;
                    vlSelfRef.__Vfunc_test_gtext__DOT__gfx_pixel__93__Vfuncout 
                        = ((0xf9ffU >= (0x0000ffffU 
                                        & (VL_MULS_III(32, (IData)(0x00000140U), vlSelfRef.__Vfunc_test_gtext__DOT__gfx_pixel__93__y) 
                                           + vlSelfRef.__Vfunc_test_gtext__DOT__gfx_pixel__93__x)))
                            ? vlSelfRef.test_gtext__DOT__dut__DOT__gfx_inst__DOT__gfx_mem__DOT__mem
                           [(0x0000ffffU & (VL_MULS_III(32, (IData)(0x00000140U), vlSelfRef.__Vfunc_test_gtext__DOT__gfx_pixel__93__y) 
                                            + vlSelfRef.__Vfunc_test_gtext__DOT__gfx_pixel__93__x))]
                            : 0U);
                }(), (IData)(vlSelfRef.__Vfunc_test_gtext__DOT__gfx_pixel__93__Vfuncout)))) {
        vlSelfRef.test_gtext__DOT__unnamedblk12__DOT__pc 
            = ((IData)(1U) + vlSelfRef.test_gtext__DOT__unnamedblk12__DOT__pc);
    }
    if ((0U != ([&]() {
                    vlSelfRef.__Vfunc_test_gtext__DOT__gfx_pixel__93__y = 0U;
                    vlSelfRef.__Vfunc_test_gtext__DOT__gfx_pixel__93__x = 0x0000000aU;
                    vlSelf->__Vfunc_test_gtext__DOT__gfx_pixel__93__Vfuncout = 0;
                    vlSelfRef.__Vfunc_test_gtext__DOT__gfx_pixel__93__Vfuncout 
                        = ((0xf9ffU >= (0x0000ffffU 
                                        & (VL_MULS_III(32, (IData)(0x00000140U), vlSelfRef.__Vfunc_test_gtext__DOT__gfx_pixel__93__y) 
                                           + vlSelfRef.__Vfunc_test_gtext__DOT__gfx_pixel__93__x)))
                            ? vlSelfRef.test_gtext__DOT__dut__DOT__gfx_inst__DOT__gfx_mem__DOT__mem
                           [(0x0000ffffU & (VL_MULS_III(32, (IData)(0x00000140U), vlSelfRef.__Vfunc_test_gtext__DOT__gfx_pixel__93__y) 
                                            + vlSelfRef.__Vfunc_test_gtext__DOT__gfx_pixel__93__x))]
                            : 0U);
                }(), (IData)(vlSelfRef.__Vfunc_test_gtext__DOT__gfx_pixel__93__Vfuncout)))) {
        vlSelfRef.test_gtext__DOT__unnamedblk12__DOT__pc 
            = ((IData)(1U) + vlSelfRef.test_gtext__DOT__unnamedblk12__DOT__pc);
    }
    if ((0U != ([&]() {
                    vlSelfRef.__Vfunc_test_gtext__DOT__gfx_pixel__93__y = 0U;
                    vlSelfRef.__Vfunc_test_gtext__DOT__gfx_pixel__93__x = 0x0000000bU;
                    vlSelf->__Vfunc_test_gtext__DOT__gfx_pixel__93__Vfuncout = 0;
                    vlSelfRef.__Vfunc_test_gtext__DOT__gfx_pixel__93__Vfuncout 
                        = ((0xf9ffU >= (0x0000ffffU 
                                        & (VL_MULS_III(32, (IData)(0x00000140U), vlSelfRef.__Vfunc_test_gtext__DOT__gfx_pixel__93__y) 
                                           + vlSelfRef.__Vfunc_test_gtext__DOT__gfx_pixel__93__x)))
                            ? vlSelfRef.test_gtext__DOT__dut__DOT__gfx_inst__DOT__gfx_mem__DOT__mem
                           [(0x0000ffffU & (VL_MULS_III(32, (IData)(0x00000140U), vlSelfRef.__Vfunc_test_gtext__DOT__gfx_pixel__93__y) 
                                            + vlSelfRef.__Vfunc_test_gtext__DOT__gfx_pixel__93__x))]
                            : 0U);
                }(), (IData)(vlSelfRef.__Vfunc_test_gtext__DOT__gfx_pixel__93__Vfuncout)))) {
        vlSelfRef.test_gtext__DOT__unnamedblk12__DOT__pc 
            = ((IData)(1U) + vlSelfRef.test_gtext__DOT__unnamedblk12__DOT__pc);
    }
    if ((0U != ([&]() {
                    vlSelfRef.__Vfunc_test_gtext__DOT__gfx_pixel__93__y = 0U;
                    vlSelfRef.__Vfunc_test_gtext__DOT__gfx_pixel__93__x = 0x0000000cU;
                    vlSelf->__Vfunc_test_gtext__DOT__gfx_pixel__93__Vfuncout = 0;
                    vlSelfRef.__Vfunc_test_gtext__DOT__gfx_pixel__93__Vfuncout 
                        = ((0xf9ffU >= (0x0000ffffU 
                                        & (VL_MULS_III(32, (IData)(0x00000140U), vlSelfRef.__Vfunc_test_gtext__DOT__gfx_pixel__93__y) 
                                           + vlSelfRef.__Vfunc_test_gtext__DOT__gfx_pixel__93__x)))
                            ? vlSelfRef.test_gtext__DOT__dut__DOT__gfx_inst__DOT__gfx_mem__DOT__mem
                           [(0x0000ffffU & (VL_MULS_III(32, (IData)(0x00000140U), vlSelfRef.__Vfunc_test_gtext__DOT__gfx_pixel__93__y) 
                                            + vlSelfRef.__Vfunc_test_gtext__DOT__gfx_pixel__93__x))]
                            : 0U);
                }(), (IData)(vlSelfRef.__Vfunc_test_gtext__DOT__gfx_pixel__93__Vfuncout)))) {
        vlSelfRef.test_gtext__DOT__unnamedblk12__DOT__pc 
            = ((IData)(1U) + vlSelfRef.test_gtext__DOT__unnamedblk12__DOT__pc);
    }
    if ((0U != ([&]() {
                    vlSelfRef.__Vfunc_test_gtext__DOT__gfx_pixel__93__y = 0U;
                    vlSelfRef.__Vfunc_test_gtext__DOT__gfx_pixel__93__x = 0x0000000dU;
                    vlSelf->__Vfunc_test_gtext__DOT__gfx_pixel__93__Vfuncout = 0;
                    vlSelfRef.__Vfunc_test_gtext__DOT__gfx_pixel__93__Vfuncout 
                        = ((0xf9ffU >= (0x0000ffffU 
                                        & (VL_MULS_III(32, (IData)(0x00000140U), vlSelfRef.__Vfunc_test_gtext__DOT__gfx_pixel__93__y) 
                                           + vlSelfRef.__Vfunc_test_gtext__DOT__gfx_pixel__93__x)))
                            ? vlSelfRef.test_gtext__DOT__dut__DOT__gfx_inst__DOT__gfx_mem__DOT__mem
                           [(0x0000ffffU & (VL_MULS_III(32, (IData)(0x00000140U), vlSelfRef.__Vfunc_test_gtext__DOT__gfx_pixel__93__y) 
                                            + vlSelfRef.__Vfunc_test_gtext__DOT__gfx_pixel__93__x))]
                            : 0U);
                }(), (IData)(vlSelfRef.__Vfunc_test_gtext__DOT__gfx_pixel__93__Vfuncout)))) {
        vlSelfRef.test_gtext__DOT__unnamedblk12__DOT__pc 
            = ((IData)(1U) + vlSelfRef.test_gtext__DOT__unnamedblk12__DOT__pc);
    }
    if ((0U != ([&]() {
                    vlSelfRef.__Vfunc_test_gtext__DOT__gfx_pixel__93__y = 0U;
                    vlSelfRef.__Vfunc_test_gtext__DOT__gfx_pixel__93__x = 0x0000000eU;
                    vlSelf->__Vfunc_test_gtext__DOT__gfx_pixel__93__Vfuncout = 0;
                    vlSelfRef.__Vfunc_test_gtext__DOT__gfx_pixel__93__Vfuncout 
                        = ((0xf9ffU >= (0x0000ffffU 
                                        & (VL_MULS_III(32, (IData)(0x00000140U), vlSelfRef.__Vfunc_test_gtext__DOT__gfx_pixel__93__y) 
                                           + vlSelfRef.__Vfunc_test_gtext__DOT__gfx_pixel__93__x)))
                            ? vlSelfRef.test_gtext__DOT__dut__DOT__gfx_inst__DOT__gfx_mem__DOT__mem
                           [(0x0000ffffU & (VL_MULS_III(32, (IData)(0x00000140U), vlSelfRef.__Vfunc_test_gtext__DOT__gfx_pixel__93__y) 
                                            + vlSelfRef.__Vfunc_test_gtext__DOT__gfx_pixel__93__x))]
                            : 0U);
                }(), (IData)(vlSelfRef.__Vfunc_test_gtext__DOT__gfx_pixel__93__Vfuncout)))) {
        vlSelfRef.test_gtext__DOT__unnamedblk12__DOT__pc 
            = ((IData)(1U) + vlSelfRef.test_gtext__DOT__unnamedblk12__DOT__pc);
    }
    if ((0U != ([&]() {
                    vlSelfRef.__Vfunc_test_gtext__DOT__gfx_pixel__93__y = 0U;
                    vlSelfRef.__Vfunc_test_gtext__DOT__gfx_pixel__93__x = 0x0000000fU;
                    vlSelf->__Vfunc_test_gtext__DOT__gfx_pixel__93__Vfuncout = 0;
                    vlSelfRef.__Vfunc_test_gtext__DOT__gfx_pixel__93__Vfuncout 
                        = ((0xf9ffU >= (0x0000ffffU 
                                        & (VL_MULS_III(32, (IData)(0x00000140U), vlSelfRef.__Vfunc_test_gtext__DOT__gfx_pixel__93__y) 
                                           + vlSelfRef.__Vfunc_test_gtext__DOT__gfx_pixel__93__x)))
                            ? vlSelfRef.test_gtext__DOT__dut__DOT__gfx_inst__DOT__gfx_mem__DOT__mem
                           [(0x0000ffffU & (VL_MULS_III(32, (IData)(0x00000140U), vlSelfRef.__Vfunc_test_gtext__DOT__gfx_pixel__93__y) 
                                            + vlSelfRef.__Vfunc_test_gtext__DOT__gfx_pixel__93__x))]
                            : 0U);
                }(), (IData)(vlSelfRef.__Vfunc_test_gtext__DOT__gfx_pixel__93__Vfuncout)))) {
        vlSelfRef.test_gtext__DOT__unnamedblk12__DOT__pc 
            = ((IData)(1U) + vlSelfRef.test_gtext__DOT__unnamedblk12__DOT__pc);
    }
    if ((0U != ([&]() {
                    vlSelfRef.__Vfunc_test_gtext__DOT__gfx_pixel__93__y = 1U;
                    vlSelfRef.__Vfunc_test_gtext__DOT__gfx_pixel__93__x = 0U;
                    vlSelf->__Vfunc_test_gtext__DOT__gfx_pixel__93__Vfuncout = 0;
                    vlSelfRef.__Vfunc_test_gtext__DOT__gfx_pixel__93__Vfuncout 
                        = ((0xf9ffU >= (0x0000ffffU 
                                        & (VL_MULS_III(32, (IData)(0x00000140U), vlSelfRef.__Vfunc_test_gtext__DOT__gfx_pixel__93__y) 
                                           + vlSelfRef.__Vfunc_test_gtext__DOT__gfx_pixel__93__x)))
                            ? vlSelfRef.test_gtext__DOT__dut__DOT__gfx_inst__DOT__gfx_mem__DOT__mem
                           [(0x0000ffffU & (VL_MULS_III(32, (IData)(0x00000140U), vlSelfRef.__Vfunc_test_gtext__DOT__gfx_pixel__93__y) 
                                            + vlSelfRef.__Vfunc_test_gtext__DOT__gfx_pixel__93__x))]
                            : 0U);
                }(), (IData)(vlSelfRef.__Vfunc_test_gtext__DOT__gfx_pixel__93__Vfuncout)))) {
        vlSelfRef.test_gtext__DOT__unnamedblk12__DOT__pc 
            = ((IData)(1U) + vlSelfRef.test_gtext__DOT__unnamedblk12__DOT__pc);
    }
    if ((0U != ([&]() {
                    vlSelfRef.__Vfunc_test_gtext__DOT__gfx_pixel__93__y = 1U;
                    vlSelfRef.__Vfunc_test_gtext__DOT__gfx_pixel__93__x = 1U;
                    vlSelf->__Vfunc_test_gtext__DOT__gfx_pixel__93__Vfuncout = 0;
                    vlSelfRef.__Vfunc_test_gtext__DOT__gfx_pixel__93__Vfuncout 
                        = ((0xf9ffU >= (0x0000ffffU 
                                        & (VL_MULS_III(32, (IData)(0x00000140U), vlSelfRef.__Vfunc_test_gtext__DOT__gfx_pixel__93__y) 
                                           + vlSelfRef.__Vfunc_test_gtext__DOT__gfx_pixel__93__x)))
                            ? vlSelfRef.test_gtext__DOT__dut__DOT__gfx_inst__DOT__gfx_mem__DOT__mem
                           [(0x0000ffffU & (VL_MULS_III(32, (IData)(0x00000140U), vlSelfRef.__Vfunc_test_gtext__DOT__gfx_pixel__93__y) 
                                            + vlSelfRef.__Vfunc_test_gtext__DOT__gfx_pixel__93__x))]
                            : 0U);
                }(), (IData)(vlSelfRef.__Vfunc_test_gtext__DOT__gfx_pixel__93__Vfuncout)))) {
        vlSelfRef.test_gtext__DOT__unnamedblk12__DOT__pc 
            = ((IData)(1U) + vlSelfRef.test_gtext__DOT__unnamedblk12__DOT__pc);
    }
    if ((0U != ([&]() {
                    vlSelfRef.__Vfunc_test_gtext__DOT__gfx_pixel__93__y = 1U;
                    vlSelfRef.__Vfunc_test_gtext__DOT__gfx_pixel__93__x = 2U;
                    vlSelf->__Vfunc_test_gtext__DOT__gfx_pixel__93__Vfuncout = 0;
                    vlSelfRef.__Vfunc_test_gtext__DOT__gfx_pixel__93__Vfuncout 
                        = ((0xf9ffU >= (0x0000ffffU 
                                        & (VL_MULS_III(32, (IData)(0x00000140U), vlSelfRef.__Vfunc_test_gtext__DOT__gfx_pixel__93__y) 
                                           + vlSelfRef.__Vfunc_test_gtext__DOT__gfx_pixel__93__x)))
                            ? vlSelfRef.test_gtext__DOT__dut__DOT__gfx_inst__DOT__gfx_mem__DOT__mem
                           [(0x0000ffffU & (VL_MULS_III(32, (IData)(0x00000140U), vlSelfRef.__Vfunc_test_gtext__DOT__gfx_pixel__93__y) 
                                            + vlSelfRef.__Vfunc_test_gtext__DOT__gfx_pixel__93__x))]
                            : 0U);
                }(), (IData)(vlSelfRef.__Vfunc_test_gtext__DOT__gfx_pixel__93__Vfuncout)))) {
        vlSelfRef.test_gtext__DOT__unnamedblk12__DOT__pc 
            = ((IData)(1U) + vlSelfRef.test_gtext__DOT__unnamedblk12__DOT__pc);
    }
    if ((0U != ([&]() {
                    vlSelfRef.__Vfunc_test_gtext__DOT__gfx_pixel__93__y = 1U;
                    vlSelfRef.__Vfunc_test_gtext__DOT__gfx_pixel__93__x = 3U;
                    vlSelf->__Vfunc_test_gtext__DOT__gfx_pixel__93__Vfuncout = 0;
                    vlSelfRef.__Vfunc_test_gtext__DOT__gfx_pixel__93__Vfuncout 
                        = ((0xf9ffU >= (0x0000ffffU 
                                        & (VL_MULS_III(32, (IData)(0x00000140U), vlSelfRef.__Vfunc_test_gtext__DOT__gfx_pixel__93__y) 
                                           + vlSelfRef.__Vfunc_test_gtext__DOT__gfx_pixel__93__x)))
                            ? vlSelfRef.test_gtext__DOT__dut__DOT__gfx_inst__DOT__gfx_mem__DOT__mem
                           [(0x0000ffffU & (VL_MULS_III(32, (IData)(0x00000140U), vlSelfRef.__Vfunc_test_gtext__DOT__gfx_pixel__93__y) 
                                            + vlSelfRef.__Vfunc_test_gtext__DOT__gfx_pixel__93__x))]
                            : 0U);
                }(), (IData)(vlSelfRef.__Vfunc_test_gtext__DOT__gfx_pixel__93__Vfuncout)))) {
        vlSelfRef.test_gtext__DOT__unnamedblk12__DOT__pc 
            = ((IData)(1U) + vlSelfRef.test_gtext__DOT__unnamedblk12__DOT__pc);
    }
    if ((0U != ([&]() {
                    vlSelfRef.__Vfunc_test_gtext__DOT__gfx_pixel__93__y = 1U;
                    vlSelfRef.__Vfunc_test_gtext__DOT__gfx_pixel__93__x = 4U;
                    vlSelf->__Vfunc_test_gtext__DOT__gfx_pixel__93__Vfuncout = 0;
                    vlSelfRef.__Vfunc_test_gtext__DOT__gfx_pixel__93__Vfuncout 
                        = ((0xf9ffU >= (0x0000ffffU 
                                        & (VL_MULS_III(32, (IData)(0x00000140U), vlSelfRef.__Vfunc_test_gtext__DOT__gfx_pixel__93__y) 
                                           + vlSelfRef.__Vfunc_test_gtext__DOT__gfx_pixel__93__x)))
                            ? vlSelfRef.test_gtext__DOT__dut__DOT__gfx_inst__DOT__gfx_mem__DOT__mem
                           [(0x0000ffffU & (VL_MULS_III(32, (IData)(0x00000140U), vlSelfRef.__Vfunc_test_gtext__DOT__gfx_pixel__93__y) 
                                            + vlSelfRef.__Vfunc_test_gtext__DOT__gfx_pixel__93__x))]
                            : 0U);
                }(), (IData)(vlSelfRef.__Vfunc_test_gtext__DOT__gfx_pixel__93__Vfuncout)))) {
        vlSelfRef.test_gtext__DOT__unnamedblk12__DOT__pc 
            = ((IData)(1U) + vlSelfRef.test_gtext__DOT__unnamedblk12__DOT__pc);
    }
    if ((0U != ([&]() {
                    vlSelfRef.__Vfunc_test_gtext__DOT__gfx_pixel__93__y = 1U;
                    vlSelfRef.__Vfunc_test_gtext__DOT__gfx_pixel__93__x = 5U;
                    vlSelf->__Vfunc_test_gtext__DOT__gfx_pixel__93__Vfuncout = 0;
                    vlSelfRef.__Vfunc_test_gtext__DOT__gfx_pixel__93__Vfuncout 
                        = ((0xf9ffU >= (0x0000ffffU 
                                        & (VL_MULS_III(32, (IData)(0x00000140U), vlSelfRef.__Vfunc_test_gtext__DOT__gfx_pixel__93__y) 
                                           + vlSelfRef.__Vfunc_test_gtext__DOT__gfx_pixel__93__x)))
                            ? vlSelfRef.test_gtext__DOT__dut__DOT__gfx_inst__DOT__gfx_mem__DOT__mem
                           [(0x0000ffffU & (VL_MULS_III(32, (IData)(0x00000140U), vlSelfRef.__Vfunc_test_gtext__DOT__gfx_pixel__93__y) 
                                            + vlSelfRef.__Vfunc_test_gtext__DOT__gfx_pixel__93__x))]
                            : 0U);
                }(), (IData)(vlSelfRef.__Vfunc_test_gtext__DOT__gfx_pixel__93__Vfuncout)))) {
        vlSelfRef.test_gtext__DOT__unnamedblk12__DOT__pc 
            = ((IData)(1U) + vlSelfRef.test_gtext__DOT__unnamedblk12__DOT__pc);
    }
    if ((0U != ([&]() {
                    vlSelfRef.__Vfunc_test_gtext__DOT__gfx_pixel__93__y = 1U;
                    vlSelfRef.__Vfunc_test_gtext__DOT__gfx_pixel__93__x = 6U;
                    vlSelf->__Vfunc_test_gtext__DOT__gfx_pixel__93__Vfuncout = 0;
                    vlSelfRef.__Vfunc_test_gtext__DOT__gfx_pixel__93__Vfuncout 
                        = ((0xf9ffU >= (0x0000ffffU 
                                        & (VL_MULS_III(32, (IData)(0x00000140U), vlSelfRef.__Vfunc_test_gtext__DOT__gfx_pixel__93__y) 
                                           + vlSelfRef.__Vfunc_test_gtext__DOT__gfx_pixel__93__x)))
                            ? vlSelfRef.test_gtext__DOT__dut__DOT__gfx_inst__DOT__gfx_mem__DOT__mem
                           [(0x0000ffffU & (VL_MULS_III(32, (IData)(0x00000140U), vlSelfRef.__Vfunc_test_gtext__DOT__gfx_pixel__93__y) 
                                            + vlSelfRef.__Vfunc_test_gtext__DOT__gfx_pixel__93__x))]
                            : 0U);
                }(), (IData)(vlSelfRef.__Vfunc_test_gtext__DOT__gfx_pixel__93__Vfuncout)))) {
        vlSelfRef.test_gtext__DOT__unnamedblk12__DOT__pc 
            = ((IData)(1U) + vlSelfRef.test_gtext__DOT__unnamedblk12__DOT__pc);
    }
    if ((0U != ([&]() {
                    vlSelfRef.__Vfunc_test_gtext__DOT__gfx_pixel__93__y = 1U;
                    vlSelfRef.__Vfunc_test_gtext__DOT__gfx_pixel__93__x = 7U;
                    vlSelf->__Vfunc_test_gtext__DOT__gfx_pixel__93__Vfuncout = 0;
                    vlSelfRef.__Vfunc_test_gtext__DOT__gfx_pixel__93__Vfuncout 
                        = ((0xf9ffU >= (0x0000ffffU 
                                        & (VL_MULS_III(32, (IData)(0x00000140U), vlSelfRef.__Vfunc_test_gtext__DOT__gfx_pixel__93__y) 
                                           + vlSelfRef.__Vfunc_test_gtext__DOT__gfx_pixel__93__x)))
                            ? vlSelfRef.test_gtext__DOT__dut__DOT__gfx_inst__DOT__gfx_mem__DOT__mem
                           [(0x0000ffffU & (VL_MULS_III(32, (IData)(0x00000140U), vlSelfRef.__Vfunc_test_gtext__DOT__gfx_pixel__93__y) 
                                            + vlSelfRef.__Vfunc_test_gtext__DOT__gfx_pixel__93__x))]
                            : 0U);
                }(), (IData)(vlSelfRef.__Vfunc_test_gtext__DOT__gfx_pixel__93__Vfuncout)))) {
        vlSelfRef.test_gtext__DOT__unnamedblk12__DOT__pc 
            = ((IData)(1U) + vlSelfRef.test_gtext__DOT__unnamedblk12__DOT__pc);
    }
    if ((0U != ([&]() {
                    vlSelfRef.__Vfunc_test_gtext__DOT__gfx_pixel__93__y = 1U;
                    vlSelfRef.__Vfunc_test_gtext__DOT__gfx_pixel__93__x = 8U;
                    vlSelf->__Vfunc_test_gtext__DOT__gfx_pixel__93__Vfuncout = 0;
                    vlSelfRef.__Vfunc_test_gtext__DOT__gfx_pixel__93__Vfuncout 
                        = ((0xf9ffU >= (0x0000ffffU 
                                        & (VL_MULS_III(32, (IData)(0x00000140U), vlSelfRef.__Vfunc_test_gtext__DOT__gfx_pixel__93__y) 
                                           + vlSelfRef.__Vfunc_test_gtext__DOT__gfx_pixel__93__x)))
                            ? vlSelfRef.test_gtext__DOT__dut__DOT__gfx_inst__DOT__gfx_mem__DOT__mem
                           [(0x0000ffffU & (VL_MULS_III(32, (IData)(0x00000140U), vlSelfRef.__Vfunc_test_gtext__DOT__gfx_pixel__93__y) 
                                            + vlSelfRef.__Vfunc_test_gtext__DOT__gfx_pixel__93__x))]
                            : 0U);
                }(), (IData)(vlSelfRef.__Vfunc_test_gtext__DOT__gfx_pixel__93__Vfuncout)))) {
        vlSelfRef.test_gtext__DOT__unnamedblk12__DOT__pc 
            = ((IData)(1U) + vlSelfRef.test_gtext__DOT__unnamedblk12__DOT__pc);
    }
    if ((0U != ([&]() {
                    vlSelfRef.__Vfunc_test_gtext__DOT__gfx_pixel__93__y = 1U;
                    vlSelfRef.__Vfunc_test_gtext__DOT__gfx_pixel__93__x = 9U;
                    vlSelf->__Vfunc_test_gtext__DOT__gfx_pixel__93__Vfuncout = 0;
                    vlSelfRef.__Vfunc_test_gtext__DOT__gfx_pixel__93__Vfuncout 
                        = ((0xf9ffU >= (0x0000ffffU 
                                        & (VL_MULS_III(32, (IData)(0x00000140U), vlSelfRef.__Vfunc_test_gtext__DOT__gfx_pixel__93__y) 
                                           + vlSelfRef.__Vfunc_test_gtext__DOT__gfx_pixel__93__x)))
                            ? vlSelfRef.test_gtext__DOT__dut__DOT__gfx_inst__DOT__gfx_mem__DOT__mem
                           [(0x0000ffffU & (VL_MULS_III(32, (IData)(0x00000140U), vlSelfRef.__Vfunc_test_gtext__DOT__gfx_pixel__93__y) 
                                            + vlSelfRef.__Vfunc_test_gtext__DOT__gfx_pixel__93__x))]
                            : 0U);
                }(), (IData)(vlSelfRef.__Vfunc_test_gtext__DOT__gfx_pixel__93__Vfuncout)))) {
        vlSelfRef.test_gtext__DOT__unnamedblk12__DOT__pc 
            = ((IData)(1U) + vlSelfRef.test_gtext__DOT__unnamedblk12__DOT__pc);
    }
    if ((0U != ([&]() {
                    vlSelfRef.__Vfunc_test_gtext__DOT__gfx_pixel__93__y = 1U;
                    vlSelfRef.__Vfunc_test_gtext__DOT__gfx_pixel__93__x = 0x0000000aU;
                    vlSelf->__Vfunc_test_gtext__DOT__gfx_pixel__93__Vfuncout = 0;
                    vlSelfRef.__Vfunc_test_gtext__DOT__gfx_pixel__93__Vfuncout 
                        = ((0xf9ffU >= (0x0000ffffU 
                                        & (VL_MULS_III(32, (IData)(0x00000140U), vlSelfRef.__Vfunc_test_gtext__DOT__gfx_pixel__93__y) 
                                           + vlSelfRef.__Vfunc_test_gtext__DOT__gfx_pixel__93__x)))
                            ? vlSelfRef.test_gtext__DOT__dut__DOT__gfx_inst__DOT__gfx_mem__DOT__mem
                           [(0x0000ffffU & (VL_MULS_III(32, (IData)(0x00000140U), vlSelfRef.__Vfunc_test_gtext__DOT__gfx_pixel__93__y) 
                                            + vlSelfRef.__Vfunc_test_gtext__DOT__gfx_pixel__93__x))]
                            : 0U);
                }(), (IData)(vlSelfRef.__Vfunc_test_gtext__DOT__gfx_pixel__93__Vfuncout)))) {
        vlSelfRef.test_gtext__DOT__unnamedblk12__DOT__pc 
            = ((IData)(1U) + vlSelfRef.test_gtext__DOT__unnamedblk12__DOT__pc);
    }
    if ((0U != ([&]() {
                    vlSelfRef.__Vfunc_test_gtext__DOT__gfx_pixel__93__y = 1U;
                    vlSelfRef.__Vfunc_test_gtext__DOT__gfx_pixel__93__x = 0x0000000bU;
                    vlSelf->__Vfunc_test_gtext__DOT__gfx_pixel__93__Vfuncout = 0;
                    vlSelfRef.__Vfunc_test_gtext__DOT__gfx_pixel__93__Vfuncout 
                        = ((0xf9ffU >= (0x0000ffffU 
                                        & (VL_MULS_III(32, (IData)(0x00000140U), vlSelfRef.__Vfunc_test_gtext__DOT__gfx_pixel__93__y) 
                                           + vlSelfRef.__Vfunc_test_gtext__DOT__gfx_pixel__93__x)))
                            ? vlSelfRef.test_gtext__DOT__dut__DOT__gfx_inst__DOT__gfx_mem__DOT__mem
                           [(0x0000ffffU & (VL_MULS_III(32, (IData)(0x00000140U), vlSelfRef.__Vfunc_test_gtext__DOT__gfx_pixel__93__y) 
                                            + vlSelfRef.__Vfunc_test_gtext__DOT__gfx_pixel__93__x))]
                            : 0U);
                }(), (IData)(vlSelfRef.__Vfunc_test_gtext__DOT__gfx_pixel__93__Vfuncout)))) {
        vlSelfRef.test_gtext__DOT__unnamedblk12__DOT__pc 
            = ((IData)(1U) + vlSelfRef.test_gtext__DOT__unnamedblk12__DOT__pc);
    }
    if ((0U != ([&]() {
                    vlSelfRef.__Vfunc_test_gtext__DOT__gfx_pixel__93__y = 1U;
                    vlSelfRef.__Vfunc_test_gtext__DOT__gfx_pixel__93__x = 0x0000000cU;
                    vlSelf->__Vfunc_test_gtext__DOT__gfx_pixel__93__Vfuncout = 0;
                    vlSelfRef.__Vfunc_test_gtext__DOT__gfx_pixel__93__Vfuncout 
                        = ((0xf9ffU >= (0x0000ffffU 
                                        & (VL_MULS_III(32, (IData)(0x00000140U), vlSelfRef.__Vfunc_test_gtext__DOT__gfx_pixel__93__y) 
                                           + vlSelfRef.__Vfunc_test_gtext__DOT__gfx_pixel__93__x)))
                            ? vlSelfRef.test_gtext__DOT__dut__DOT__gfx_inst__DOT__gfx_mem__DOT__mem
                           [(0x0000ffffU & (VL_MULS_III(32, (IData)(0x00000140U), vlSelfRef.__Vfunc_test_gtext__DOT__gfx_pixel__93__y) 
                                            + vlSelfRef.__Vfunc_test_gtext__DOT__gfx_pixel__93__x))]
                            : 0U);
                }(), (IData)(vlSelfRef.__Vfunc_test_gtext__DOT__gfx_pixel__93__Vfuncout)))) {
        vlSelfRef.test_gtext__DOT__unnamedblk12__DOT__pc 
            = ((IData)(1U) + vlSelfRef.test_gtext__DOT__unnamedblk12__DOT__pc);
    }
    if ((0U != ([&]() {
                    vlSelfRef.__Vfunc_test_gtext__DOT__gfx_pixel__93__y = 1U;
                    vlSelfRef.__Vfunc_test_gtext__DOT__gfx_pixel__93__x = 0x0000000dU;
                    vlSelf->__Vfunc_test_gtext__DOT__gfx_pixel__93__Vfuncout = 0;
                    vlSelfRef.__Vfunc_test_gtext__DOT__gfx_pixel__93__Vfuncout 
                        = ((0xf9ffU >= (0x0000ffffU 
                                        & (VL_MULS_III(32, (IData)(0x00000140U), vlSelfRef.__Vfunc_test_gtext__DOT__gfx_pixel__93__y) 
                                           + vlSelfRef.__Vfunc_test_gtext__DOT__gfx_pixel__93__x)))
                            ? vlSelfRef.test_gtext__DOT__dut__DOT__gfx_inst__DOT__gfx_mem__DOT__mem
                           [(0x0000ffffU & (VL_MULS_III(32, (IData)(0x00000140U), vlSelfRef.__Vfunc_test_gtext__DOT__gfx_pixel__93__y) 
                                            + vlSelfRef.__Vfunc_test_gtext__DOT__gfx_pixel__93__x))]
                            : 0U);
                }(), (IData)(vlSelfRef.__Vfunc_test_gtext__DOT__gfx_pixel__93__Vfuncout)))) {
        vlSelfRef.test_gtext__DOT__unnamedblk12__DOT__pc 
            = ((IData)(1U) + vlSelfRef.test_gtext__DOT__unnamedblk12__DOT__pc);
    }
    if ((0U != ([&]() {
                    vlSelfRef.__Vfunc_test_gtext__DOT__gfx_pixel__93__y = 1U;
                    vlSelfRef.__Vfunc_test_gtext__DOT__gfx_pixel__93__x = 0x0000000eU;
                    vlSelf->__Vfunc_test_gtext__DOT__gfx_pixel__93__Vfuncout = 0;
                    vlSelfRef.__Vfunc_test_gtext__DOT__gfx_pixel__93__Vfuncout 
                        = ((0xf9ffU >= (0x0000ffffU 
                                        & (VL_MULS_III(32, (IData)(0x00000140U), vlSelfRef.__Vfunc_test_gtext__DOT__gfx_pixel__93__y) 
                                           + vlSelfRef.__Vfunc_test_gtext__DOT__gfx_pixel__93__x)))
                            ? vlSelfRef.test_gtext__DOT__dut__DOT__gfx_inst__DOT__gfx_mem__DOT__mem
                           [(0x0000ffffU & (VL_MULS_III(32, (IData)(0x00000140U), vlSelfRef.__Vfunc_test_gtext__DOT__gfx_pixel__93__y) 
                                            + vlSelfRef.__Vfunc_test_gtext__DOT__gfx_pixel__93__x))]
                            : 0U);
                }(), (IData)(vlSelfRef.__Vfunc_test_gtext__DOT__gfx_pixel__93__Vfuncout)))) {
        vlSelfRef.test_gtext__DOT__unnamedblk12__DOT__pc 
            = ((IData)(1U) + vlSelfRef.test_gtext__DOT__unnamedblk12__DOT__pc);
    }
    if ((0U != ([&]() {
                    vlSelfRef.__Vfunc_test_gtext__DOT__gfx_pixel__93__y = 1U;
                    vlSelfRef.__Vfunc_test_gtext__DOT__gfx_pixel__93__x = 0x0000000fU;
                    vlSelf->__Vfunc_test_gtext__DOT__gfx_pixel__93__Vfuncout = 0;
                    vlSelfRef.__Vfunc_test_gtext__DOT__gfx_pixel__93__Vfuncout 
                        = ((0xf9ffU >= (0x0000ffffU 
                                        & (VL_MULS_III(32, (IData)(0x00000140U), vlSelfRef.__Vfunc_test_gtext__DOT__gfx_pixel__93__y) 
                                           + vlSelfRef.__Vfunc_test_gtext__DOT__gfx_pixel__93__x)))
                            ? vlSelfRef.test_gtext__DOT__dut__DOT__gfx_inst__DOT__gfx_mem__DOT__mem
                           [(0x0000ffffU & (VL_MULS_III(32, (IData)(0x00000140U), vlSelfRef.__Vfunc_test_gtext__DOT__gfx_pixel__93__y) 
                                            + vlSelfRef.__Vfunc_test_gtext__DOT__gfx_pixel__93__x))]
                            : 0U);
                }(), (IData)(vlSelfRef.__Vfunc_test_gtext__DOT__gfx_pixel__93__Vfuncout)))) {
        vlSelfRef.test_gtext__DOT__unnamedblk12__DOT__pc 
            = ((IData)(1U) + vlSelfRef.test_gtext__DOT__unnamedblk12__DOT__pc);
    }
    if ((0U != ([&]() {
                    vlSelfRef.__Vfunc_test_gtext__DOT__gfx_pixel__93__y = 2U;
                    vlSelfRef.__Vfunc_test_gtext__DOT__gfx_pixel__93__x = 0U;
                    vlSelf->__Vfunc_test_gtext__DOT__gfx_pixel__93__Vfuncout = 0;
                    vlSelfRef.__Vfunc_test_gtext__DOT__gfx_pixel__93__Vfuncout 
                        = ((0xf9ffU >= (0x0000ffffU 
                                        & (VL_MULS_III(32, (IData)(0x00000140U), vlSelfRef.__Vfunc_test_gtext__DOT__gfx_pixel__93__y) 
                                           + vlSelfRef.__Vfunc_test_gtext__DOT__gfx_pixel__93__x)))
                            ? vlSelfRef.test_gtext__DOT__dut__DOT__gfx_inst__DOT__gfx_mem__DOT__mem
                           [(0x0000ffffU & (VL_MULS_III(32, (IData)(0x00000140U), vlSelfRef.__Vfunc_test_gtext__DOT__gfx_pixel__93__y) 
                                            + vlSelfRef.__Vfunc_test_gtext__DOT__gfx_pixel__93__x))]
                            : 0U);
                }(), (IData)(vlSelfRef.__Vfunc_test_gtext__DOT__gfx_pixel__93__Vfuncout)))) {
        vlSelfRef.test_gtext__DOT__unnamedblk12__DOT__pc 
            = ((IData)(1U) + vlSelfRef.test_gtext__DOT__unnamedblk12__DOT__pc);
    }
    if ((0U != ([&]() {
                    vlSelfRef.__Vfunc_test_gtext__DOT__gfx_pixel__93__y = 2U;
                    vlSelfRef.__Vfunc_test_gtext__DOT__gfx_pixel__93__x = 1U;
                    vlSelf->__Vfunc_test_gtext__DOT__gfx_pixel__93__Vfuncout = 0;
                    vlSelfRef.__Vfunc_test_gtext__DOT__gfx_pixel__93__Vfuncout 
                        = ((0xf9ffU >= (0x0000ffffU 
                                        & (VL_MULS_III(32, (IData)(0x00000140U), vlSelfRef.__Vfunc_test_gtext__DOT__gfx_pixel__93__y) 
                                           + vlSelfRef.__Vfunc_test_gtext__DOT__gfx_pixel__93__x)))
                            ? vlSelfRef.test_gtext__DOT__dut__DOT__gfx_inst__DOT__gfx_mem__DOT__mem
                           [(0x0000ffffU & (VL_MULS_III(32, (IData)(0x00000140U), vlSelfRef.__Vfunc_test_gtext__DOT__gfx_pixel__93__y) 
                                            + vlSelfRef.__Vfunc_test_gtext__DOT__gfx_pixel__93__x))]
                            : 0U);
                }(), (IData)(vlSelfRef.__Vfunc_test_gtext__DOT__gfx_pixel__93__Vfuncout)))) {
        vlSelfRef.test_gtext__DOT__unnamedblk12__DOT__pc 
            = ((IData)(1U) + vlSelfRef.test_gtext__DOT__unnamedblk12__DOT__pc);
    }
    if ((0U != ([&]() {
                    vlSelfRef.__Vfunc_test_gtext__DOT__gfx_pixel__93__y = 2U;
                    vlSelfRef.__Vfunc_test_gtext__DOT__gfx_pixel__93__x = 2U;
                    vlSelf->__Vfunc_test_gtext__DOT__gfx_pixel__93__Vfuncout = 0;
                    vlSelfRef.__Vfunc_test_gtext__DOT__gfx_pixel__93__Vfuncout 
                        = ((0xf9ffU >= (0x0000ffffU 
                                        & (VL_MULS_III(32, (IData)(0x00000140U), vlSelfRef.__Vfunc_test_gtext__DOT__gfx_pixel__93__y) 
                                           + vlSelfRef.__Vfunc_test_gtext__DOT__gfx_pixel__93__x)))
                            ? vlSelfRef.test_gtext__DOT__dut__DOT__gfx_inst__DOT__gfx_mem__DOT__mem
                           [(0x0000ffffU & (VL_MULS_III(32, (IData)(0x00000140U), vlSelfRef.__Vfunc_test_gtext__DOT__gfx_pixel__93__y) 
                                            + vlSelfRef.__Vfunc_test_gtext__DOT__gfx_pixel__93__x))]
                            : 0U);
                }(), (IData)(vlSelfRef.__Vfunc_test_gtext__DOT__gfx_pixel__93__Vfuncout)))) {
        vlSelfRef.test_gtext__DOT__unnamedblk12__DOT__pc 
            = ((IData)(1U) + vlSelfRef.test_gtext__DOT__unnamedblk12__DOT__pc);
    }
    if ((0U != ([&]() {
                    vlSelfRef.__Vfunc_test_gtext__DOT__gfx_pixel__93__y = 2U;
                    vlSelfRef.__Vfunc_test_gtext__DOT__gfx_pixel__93__x = 3U;
                    vlSelf->__Vfunc_test_gtext__DOT__gfx_pixel__93__Vfuncout = 0;
                    vlSelfRef.__Vfunc_test_gtext__DOT__gfx_pixel__93__Vfuncout 
                        = ((0xf9ffU >= (0x0000ffffU 
                                        & (VL_MULS_III(32, (IData)(0x00000140U), vlSelfRef.__Vfunc_test_gtext__DOT__gfx_pixel__93__y) 
                                           + vlSelfRef.__Vfunc_test_gtext__DOT__gfx_pixel__93__x)))
                            ? vlSelfRef.test_gtext__DOT__dut__DOT__gfx_inst__DOT__gfx_mem__DOT__mem
                           [(0x0000ffffU & (VL_MULS_III(32, (IData)(0x00000140U), vlSelfRef.__Vfunc_test_gtext__DOT__gfx_pixel__93__y) 
                                            + vlSelfRef.__Vfunc_test_gtext__DOT__gfx_pixel__93__x))]
                            : 0U);
                }(), (IData)(vlSelfRef.__Vfunc_test_gtext__DOT__gfx_pixel__93__Vfuncout)))) {
        vlSelfRef.test_gtext__DOT__unnamedblk12__DOT__pc 
            = ((IData)(1U) + vlSelfRef.test_gtext__DOT__unnamedblk12__DOT__pc);
    }
    if ((0U != ([&]() {
                    vlSelfRef.__Vfunc_test_gtext__DOT__gfx_pixel__93__y = 2U;
                    vlSelfRef.__Vfunc_test_gtext__DOT__gfx_pixel__93__x = 4U;
                    vlSelf->__Vfunc_test_gtext__DOT__gfx_pixel__93__Vfuncout = 0;
                    vlSelfRef.__Vfunc_test_gtext__DOT__gfx_pixel__93__Vfuncout 
                        = ((0xf9ffU >= (0x0000ffffU 
                                        & (VL_MULS_III(32, (IData)(0x00000140U), vlSelfRef.__Vfunc_test_gtext__DOT__gfx_pixel__93__y) 
                                           + vlSelfRef.__Vfunc_test_gtext__DOT__gfx_pixel__93__x)))
                            ? vlSelfRef.test_gtext__DOT__dut__DOT__gfx_inst__DOT__gfx_mem__DOT__mem
                           [(0x0000ffffU & (VL_MULS_III(32, (IData)(0x00000140U), vlSelfRef.__Vfunc_test_gtext__DOT__gfx_pixel__93__y) 
                                            + vlSelfRef.__Vfunc_test_gtext__DOT__gfx_pixel__93__x))]
                            : 0U);
                }(), (IData)(vlSelfRef.__Vfunc_test_gtext__DOT__gfx_pixel__93__Vfuncout)))) {
        vlSelfRef.test_gtext__DOT__unnamedblk12__DOT__pc 
            = ((IData)(1U) + vlSelfRef.test_gtext__DOT__unnamedblk12__DOT__pc);
    }
    if ((0U != ([&]() {
                    vlSelfRef.__Vfunc_test_gtext__DOT__gfx_pixel__93__y = 2U;
                    vlSelfRef.__Vfunc_test_gtext__DOT__gfx_pixel__93__x = 5U;
                    vlSelf->__Vfunc_test_gtext__DOT__gfx_pixel__93__Vfuncout = 0;
                    vlSelfRef.__Vfunc_test_gtext__DOT__gfx_pixel__93__Vfuncout 
                        = ((0xf9ffU >= (0x0000ffffU 
                                        & (VL_MULS_III(32, (IData)(0x00000140U), vlSelfRef.__Vfunc_test_gtext__DOT__gfx_pixel__93__y) 
                                           + vlSelfRef.__Vfunc_test_gtext__DOT__gfx_pixel__93__x)))
                            ? vlSelfRef.test_gtext__DOT__dut__DOT__gfx_inst__DOT__gfx_mem__DOT__mem
                           [(0x0000ffffU & (VL_MULS_III(32, (IData)(0x00000140U), vlSelfRef.__Vfunc_test_gtext__DOT__gfx_pixel__93__y) 
                                            + vlSelfRef.__Vfunc_test_gtext__DOT__gfx_pixel__93__x))]
                            : 0U);
                }(), (IData)(vlSelfRef.__Vfunc_test_gtext__DOT__gfx_pixel__93__Vfuncout)))) {
        vlSelfRef.test_gtext__DOT__unnamedblk12__DOT__pc 
            = ((IData)(1U) + vlSelfRef.test_gtext__DOT__unnamedblk12__DOT__pc);
    }
    if ((0U != ([&]() {
                    vlSelfRef.__Vfunc_test_gtext__DOT__gfx_pixel__93__y = 2U;
                    vlSelfRef.__Vfunc_test_gtext__DOT__gfx_pixel__93__x = 6U;
                    vlSelf->__Vfunc_test_gtext__DOT__gfx_pixel__93__Vfuncout = 0;
                    vlSelfRef.__Vfunc_test_gtext__DOT__gfx_pixel__93__Vfuncout 
                        = ((0xf9ffU >= (0x0000ffffU 
                                        & (VL_MULS_III(32, (IData)(0x00000140U), vlSelfRef.__Vfunc_test_gtext__DOT__gfx_pixel__93__y) 
                                           + vlSelfRef.__Vfunc_test_gtext__DOT__gfx_pixel__93__x)))
                            ? vlSelfRef.test_gtext__DOT__dut__DOT__gfx_inst__DOT__gfx_mem__DOT__mem
                           [(0x0000ffffU & (VL_MULS_III(32, (IData)(0x00000140U), vlSelfRef.__Vfunc_test_gtext__DOT__gfx_pixel__93__y) 
                                            + vlSelfRef.__Vfunc_test_gtext__DOT__gfx_pixel__93__x))]
                            : 0U);
                }(), (IData)(vlSelfRef.__Vfunc_test_gtext__DOT__gfx_pixel__93__Vfuncout)))) {
        vlSelfRef.test_gtext__DOT__unnamedblk12__DOT__pc 
            = ((IData)(1U) + vlSelfRef.test_gtext__DOT__unnamedblk12__DOT__pc);
    }
    if ((0U != ([&]() {
                    vlSelfRef.__Vfunc_test_gtext__DOT__gfx_pixel__93__y = 2U;
                    vlSelfRef.__Vfunc_test_gtext__DOT__gfx_pixel__93__x = 7U;
                    vlSelf->__Vfunc_test_gtext__DOT__gfx_pixel__93__Vfuncout = 0;
                    vlSelfRef.__Vfunc_test_gtext__DOT__gfx_pixel__93__Vfuncout 
                        = ((0xf9ffU >= (0x0000ffffU 
                                        & (VL_MULS_III(32, (IData)(0x00000140U), vlSelfRef.__Vfunc_test_gtext__DOT__gfx_pixel__93__y) 
                                           + vlSelfRef.__Vfunc_test_gtext__DOT__gfx_pixel__93__x)))
                            ? vlSelfRef.test_gtext__DOT__dut__DOT__gfx_inst__DOT__gfx_mem__DOT__mem
                           [(0x0000ffffU & (VL_MULS_III(32, (IData)(0x00000140U), vlSelfRef.__Vfunc_test_gtext__DOT__gfx_pixel__93__y) 
                                            + vlSelfRef.__Vfunc_test_gtext__DOT__gfx_pixel__93__x))]
                            : 0U);
                }(), (IData)(vlSelfRef.__Vfunc_test_gtext__DOT__gfx_pixel__93__Vfuncout)))) {
        vlSelfRef.test_gtext__DOT__unnamedblk12__DOT__pc 
            = ((IData)(1U) + vlSelfRef.test_gtext__DOT__unnamedblk12__DOT__pc);
    }
    if ((0U != ([&]() {
                    vlSelfRef.__Vfunc_test_gtext__DOT__gfx_pixel__93__y = 2U;
                    vlSelfRef.__Vfunc_test_gtext__DOT__gfx_pixel__93__x = 8U;
                    vlSelf->__Vfunc_test_gtext__DOT__gfx_pixel__93__Vfuncout = 0;
                    vlSelfRef.__Vfunc_test_gtext__DOT__gfx_pixel__93__Vfuncout 
                        = ((0xf9ffU >= (0x0000ffffU 
                                        & (VL_MULS_III(32, (IData)(0x00000140U), vlSelfRef.__Vfunc_test_gtext__DOT__gfx_pixel__93__y) 
                                           + vlSelfRef.__Vfunc_test_gtext__DOT__gfx_pixel__93__x)))
                            ? vlSelfRef.test_gtext__DOT__dut__DOT__gfx_inst__DOT__gfx_mem__DOT__mem
                           [(0x0000ffffU & (VL_MULS_III(32, (IData)(0x00000140U), vlSelfRef.__Vfunc_test_gtext__DOT__gfx_pixel__93__y) 
                                            + vlSelfRef.__Vfunc_test_gtext__DOT__gfx_pixel__93__x))]
                            : 0U);
                }(), (IData)(vlSelfRef.__Vfunc_test_gtext__DOT__gfx_pixel__93__Vfuncout)))) {
        vlSelfRef.test_gtext__DOT__unnamedblk12__DOT__pc 
            = ((IData)(1U) + vlSelfRef.test_gtext__DOT__unnamedblk12__DOT__pc);
    }
    if ((0U != ([&]() {
                    vlSelfRef.__Vfunc_test_gtext__DOT__gfx_pixel__93__y = 2U;
                    vlSelfRef.__Vfunc_test_gtext__DOT__gfx_pixel__93__x = 9U;
                    vlSelf->__Vfunc_test_gtext__DOT__gfx_pixel__93__Vfuncout = 0;
                    vlSelfRef.__Vfunc_test_gtext__DOT__gfx_pixel__93__Vfuncout 
                        = ((0xf9ffU >= (0x0000ffffU 
                                        & (VL_MULS_III(32, (IData)(0x00000140U), vlSelfRef.__Vfunc_test_gtext__DOT__gfx_pixel__93__y) 
                                           + vlSelfRef.__Vfunc_test_gtext__DOT__gfx_pixel__93__x)))
                            ? vlSelfRef.test_gtext__DOT__dut__DOT__gfx_inst__DOT__gfx_mem__DOT__mem
                           [(0x0000ffffU & (VL_MULS_III(32, (IData)(0x00000140U), vlSelfRef.__Vfunc_test_gtext__DOT__gfx_pixel__93__y) 
                                            + vlSelfRef.__Vfunc_test_gtext__DOT__gfx_pixel__93__x))]
                            : 0U);
                }(), (IData)(vlSelfRef.__Vfunc_test_gtext__DOT__gfx_pixel__93__Vfuncout)))) {
        vlSelfRef.test_gtext__DOT__unnamedblk12__DOT__pc 
            = ((IData)(1U) + vlSelfRef.test_gtext__DOT__unnamedblk12__DOT__pc);
    }
    if ((0U != ([&]() {
                    vlSelfRef.__Vfunc_test_gtext__DOT__gfx_pixel__93__y = 2U;
                    vlSelfRef.__Vfunc_test_gtext__DOT__gfx_pixel__93__x = 0x0000000aU;
                    vlSelf->__Vfunc_test_gtext__DOT__gfx_pixel__93__Vfuncout = 0;
                    vlSelfRef.__Vfunc_test_gtext__DOT__gfx_pixel__93__Vfuncout 
                        = ((0xf9ffU >= (0x0000ffffU 
                                        & (VL_MULS_III(32, (IData)(0x00000140U), vlSelfRef.__Vfunc_test_gtext__DOT__gfx_pixel__93__y) 
                                           + vlSelfRef.__Vfunc_test_gtext__DOT__gfx_pixel__93__x)))
                            ? vlSelfRef.test_gtext__DOT__dut__DOT__gfx_inst__DOT__gfx_mem__DOT__mem
                           [(0x0000ffffU & (VL_MULS_III(32, (IData)(0x00000140U), vlSelfRef.__Vfunc_test_gtext__DOT__gfx_pixel__93__y) 
                                            + vlSelfRef.__Vfunc_test_gtext__DOT__gfx_pixel__93__x))]
                            : 0U);
                }(), (IData)(vlSelfRef.__Vfunc_test_gtext__DOT__gfx_pixel__93__Vfuncout)))) {
        vlSelfRef.test_gtext__DOT__unnamedblk12__DOT__pc 
            = ((IData)(1U) + vlSelfRef.test_gtext__DOT__unnamedblk12__DOT__pc);
    }
    if ((0U != ([&]() {
                    vlSelfRef.__Vfunc_test_gtext__DOT__gfx_pixel__93__y = 2U;
                    vlSelfRef.__Vfunc_test_gtext__DOT__gfx_pixel__93__x = 0x0000000bU;
                    vlSelf->__Vfunc_test_gtext__DOT__gfx_pixel__93__Vfuncout = 0;
                    vlSelfRef.__Vfunc_test_gtext__DOT__gfx_pixel__93__Vfuncout 
                        = ((0xf9ffU >= (0x0000ffffU 
                                        & (VL_MULS_III(32, (IData)(0x00000140U), vlSelfRef.__Vfunc_test_gtext__DOT__gfx_pixel__93__y) 
                                           + vlSelfRef.__Vfunc_test_gtext__DOT__gfx_pixel__93__x)))
                            ? vlSelfRef.test_gtext__DOT__dut__DOT__gfx_inst__DOT__gfx_mem__DOT__mem
                           [(0x0000ffffU & (VL_MULS_III(32, (IData)(0x00000140U), vlSelfRef.__Vfunc_test_gtext__DOT__gfx_pixel__93__y) 
                                            + vlSelfRef.__Vfunc_test_gtext__DOT__gfx_pixel__93__x))]
                            : 0U);
                }(), (IData)(vlSelfRef.__Vfunc_test_gtext__DOT__gfx_pixel__93__Vfuncout)))) {
        vlSelfRef.test_gtext__DOT__unnamedblk12__DOT__pc 
            = ((IData)(1U) + vlSelfRef.test_gtext__DOT__unnamedblk12__DOT__pc);
    }
    if ((0U != ([&]() {
                    vlSelfRef.__Vfunc_test_gtext__DOT__gfx_pixel__93__y = 2U;
                    vlSelfRef.__Vfunc_test_gtext__DOT__gfx_pixel__93__x = 0x0000000cU;
                    vlSelf->__Vfunc_test_gtext__DOT__gfx_pixel__93__Vfuncout = 0;
                    vlSelfRef.__Vfunc_test_gtext__DOT__gfx_pixel__93__Vfuncout 
                        = ((0xf9ffU >= (0x0000ffffU 
                                        & (VL_MULS_III(32, (IData)(0x00000140U), vlSelfRef.__Vfunc_test_gtext__DOT__gfx_pixel__93__y) 
                                           + vlSelfRef.__Vfunc_test_gtext__DOT__gfx_pixel__93__x)))
                            ? vlSelfRef.test_gtext__DOT__dut__DOT__gfx_inst__DOT__gfx_mem__DOT__mem
                           [(0x0000ffffU & (VL_MULS_III(32, (IData)(0x00000140U), vlSelfRef.__Vfunc_test_gtext__DOT__gfx_pixel__93__y) 
                                            + vlSelfRef.__Vfunc_test_gtext__DOT__gfx_pixel__93__x))]
                            : 0U);
                }(), (IData)(vlSelfRef.__Vfunc_test_gtext__DOT__gfx_pixel__93__Vfuncout)))) {
        vlSelfRef.test_gtext__DOT__unnamedblk12__DOT__pc 
            = ((IData)(1U) + vlSelfRef.test_gtext__DOT__unnamedblk12__DOT__pc);
    }
    if ((0U != ([&]() {
                    vlSelfRef.__Vfunc_test_gtext__DOT__gfx_pixel__93__y = 2U;
                    vlSelfRef.__Vfunc_test_gtext__DOT__gfx_pixel__93__x = 0x0000000dU;
                    vlSelf->__Vfunc_test_gtext__DOT__gfx_pixel__93__Vfuncout = 0;
                    vlSelfRef.__Vfunc_test_gtext__DOT__gfx_pixel__93__Vfuncout 
                        = ((0xf9ffU >= (0x0000ffffU 
                                        & (VL_MULS_III(32, (IData)(0x00000140U), vlSelfRef.__Vfunc_test_gtext__DOT__gfx_pixel__93__y) 
                                           + vlSelfRef.__Vfunc_test_gtext__DOT__gfx_pixel__93__x)))
                            ? vlSelfRef.test_gtext__DOT__dut__DOT__gfx_inst__DOT__gfx_mem__DOT__mem
                           [(0x0000ffffU & (VL_MULS_III(32, (IData)(0x00000140U), vlSelfRef.__Vfunc_test_gtext__DOT__gfx_pixel__93__y) 
                                            + vlSelfRef.__Vfunc_test_gtext__DOT__gfx_pixel__93__x))]
                            : 0U);
                }(), (IData)(vlSelfRef.__Vfunc_test_gtext__DOT__gfx_pixel__93__Vfuncout)))) {
        vlSelfRef.test_gtext__DOT__unnamedblk12__DOT__pc 
            = ((IData)(1U) + vlSelfRef.test_gtext__DOT__unnamedblk12__DOT__pc);
    }
    if ((0U != ([&]() {
                    vlSelfRef.__Vfunc_test_gtext__DOT__gfx_pixel__93__y = 2U;
                    vlSelfRef.__Vfunc_test_gtext__DOT__gfx_pixel__93__x = 0x0000000eU;
                    vlSelf->__Vfunc_test_gtext__DOT__gfx_pixel__93__Vfuncout = 0;
                    vlSelfRef.__Vfunc_test_gtext__DOT__gfx_pixel__93__Vfuncout 
                        = ((0xf9ffU >= (0x0000ffffU 
                                        & (VL_MULS_III(32, (IData)(0x00000140U), vlSelfRef.__Vfunc_test_gtext__DOT__gfx_pixel__93__y) 
                                           + vlSelfRef.__Vfunc_test_gtext__DOT__gfx_pixel__93__x)))
                            ? vlSelfRef.test_gtext__DOT__dut__DOT__gfx_inst__DOT__gfx_mem__DOT__mem
                           [(0x0000ffffU & (VL_MULS_III(32, (IData)(0x00000140U), vlSelfRef.__Vfunc_test_gtext__DOT__gfx_pixel__93__y) 
                                            + vlSelfRef.__Vfunc_test_gtext__DOT__gfx_pixel__93__x))]
                            : 0U);
                }(), (IData)(vlSelfRef.__Vfunc_test_gtext__DOT__gfx_pixel__93__Vfuncout)))) {
        vlSelfRef.test_gtext__DOT__unnamedblk12__DOT__pc 
            = ((IData)(1U) + vlSelfRef.test_gtext__DOT__unnamedblk12__DOT__pc);
    }
    if ((0U != ([&]() {
                    vlSelfRef.__Vfunc_test_gtext__DOT__gfx_pixel__93__y = 2U;
                    vlSelfRef.__Vfunc_test_gtext__DOT__gfx_pixel__93__x = 0x0000000fU;
                    vlSelf->__Vfunc_test_gtext__DOT__gfx_pixel__93__Vfuncout = 0;
                    vlSelfRef.__Vfunc_test_gtext__DOT__gfx_pixel__93__Vfuncout 
                        = ((0xf9ffU >= (0x0000ffffU 
                                        & (VL_MULS_III(32, (IData)(0x00000140U), vlSelfRef.__Vfunc_test_gtext__DOT__gfx_pixel__93__y) 
                                           + vlSelfRef.__Vfunc_test_gtext__DOT__gfx_pixel__93__x)))
                            ? vlSelfRef.test_gtext__DOT__dut__DOT__gfx_inst__DOT__gfx_mem__DOT__mem
                           [(0x0000ffffU & (VL_MULS_III(32, (IData)(0x00000140U), vlSelfRef.__Vfunc_test_gtext__DOT__gfx_pixel__93__y) 
                                            + vlSelfRef.__Vfunc_test_gtext__DOT__gfx_pixel__93__x))]
                            : 0U);
                }(), (IData)(vlSelfRef.__Vfunc_test_gtext__DOT__gfx_pixel__93__Vfuncout)))) {
        vlSelfRef.test_gtext__DOT__unnamedblk12__DOT__pc 
            = ((IData)(1U) + vlSelfRef.test_gtext__DOT__unnamedblk12__DOT__pc);
    }
    if ((0U != ([&]() {
                    vlSelfRef.__Vfunc_test_gtext__DOT__gfx_pixel__93__y = 3U;
                    vlSelfRef.__Vfunc_test_gtext__DOT__gfx_pixel__93__x = 0U;
                    vlSelf->__Vfunc_test_gtext__DOT__gfx_pixel__93__Vfuncout = 0;
                    vlSelfRef.__Vfunc_test_gtext__DOT__gfx_pixel__93__Vfuncout 
                        = ((0xf9ffU >= (0x0000ffffU 
                                        & (VL_MULS_III(32, (IData)(0x00000140U), vlSelfRef.__Vfunc_test_gtext__DOT__gfx_pixel__93__y) 
                                           + vlSelfRef.__Vfunc_test_gtext__DOT__gfx_pixel__93__x)))
                            ? vlSelfRef.test_gtext__DOT__dut__DOT__gfx_inst__DOT__gfx_mem__DOT__mem
                           [(0x0000ffffU & (VL_MULS_III(32, (IData)(0x00000140U), vlSelfRef.__Vfunc_test_gtext__DOT__gfx_pixel__93__y) 
                                            + vlSelfRef.__Vfunc_test_gtext__DOT__gfx_pixel__93__x))]
                            : 0U);
                }(), (IData)(vlSelfRef.__Vfunc_test_gtext__DOT__gfx_pixel__93__Vfuncout)))) {
        vlSelfRef.test_gtext__DOT__unnamedblk12__DOT__pc 
            = ((IData)(1U) + vlSelfRef.test_gtext__DOT__unnamedblk12__DOT__pc);
    }
    if ((0U != ([&]() {
                    vlSelfRef.__Vfunc_test_gtext__DOT__gfx_pixel__93__y = 3U;
                    vlSelfRef.__Vfunc_test_gtext__DOT__gfx_pixel__93__x = 1U;
                    vlSelf->__Vfunc_test_gtext__DOT__gfx_pixel__93__Vfuncout = 0;
                    vlSelfRef.__Vfunc_test_gtext__DOT__gfx_pixel__93__Vfuncout 
                        = ((0xf9ffU >= (0x0000ffffU 
                                        & (VL_MULS_III(32, (IData)(0x00000140U), vlSelfRef.__Vfunc_test_gtext__DOT__gfx_pixel__93__y) 
                                           + vlSelfRef.__Vfunc_test_gtext__DOT__gfx_pixel__93__x)))
                            ? vlSelfRef.test_gtext__DOT__dut__DOT__gfx_inst__DOT__gfx_mem__DOT__mem
                           [(0x0000ffffU & (VL_MULS_III(32, (IData)(0x00000140U), vlSelfRef.__Vfunc_test_gtext__DOT__gfx_pixel__93__y) 
                                            + vlSelfRef.__Vfunc_test_gtext__DOT__gfx_pixel__93__x))]
                            : 0U);
                }(), (IData)(vlSelfRef.__Vfunc_test_gtext__DOT__gfx_pixel__93__Vfuncout)))) {
        vlSelfRef.test_gtext__DOT__unnamedblk12__DOT__pc 
            = ((IData)(1U) + vlSelfRef.test_gtext__DOT__unnamedblk12__DOT__pc);
    }
    if ((0U != ([&]() {
                    vlSelfRef.__Vfunc_test_gtext__DOT__gfx_pixel__93__y = 3U;
                    vlSelfRef.__Vfunc_test_gtext__DOT__gfx_pixel__93__x = 2U;
                    vlSelf->__Vfunc_test_gtext__DOT__gfx_pixel__93__Vfuncout = 0;
                    vlSelfRef.__Vfunc_test_gtext__DOT__gfx_pixel__93__Vfuncout 
                        = ((0xf9ffU >= (0x0000ffffU 
                                        & (VL_MULS_III(32, (IData)(0x00000140U), vlSelfRef.__Vfunc_test_gtext__DOT__gfx_pixel__93__y) 
                                           + vlSelfRef.__Vfunc_test_gtext__DOT__gfx_pixel__93__x)))
                            ? vlSelfRef.test_gtext__DOT__dut__DOT__gfx_inst__DOT__gfx_mem__DOT__mem
                           [(0x0000ffffU & (VL_MULS_III(32, (IData)(0x00000140U), vlSelfRef.__Vfunc_test_gtext__DOT__gfx_pixel__93__y) 
                                            + vlSelfRef.__Vfunc_test_gtext__DOT__gfx_pixel__93__x))]
                            : 0U);
                }(), (IData)(vlSelfRef.__Vfunc_test_gtext__DOT__gfx_pixel__93__Vfuncout)))) {
        vlSelfRef.test_gtext__DOT__unnamedblk12__DOT__pc 
            = ((IData)(1U) + vlSelfRef.test_gtext__DOT__unnamedblk12__DOT__pc);
    }
    if ((0U != ([&]() {
                    vlSelfRef.__Vfunc_test_gtext__DOT__gfx_pixel__93__y = 3U;
                    vlSelfRef.__Vfunc_test_gtext__DOT__gfx_pixel__93__x = 3U;
                    vlSelf->__Vfunc_test_gtext__DOT__gfx_pixel__93__Vfuncout = 0;
                    vlSelfRef.__Vfunc_test_gtext__DOT__gfx_pixel__93__Vfuncout 
                        = ((0xf9ffU >= (0x0000ffffU 
                                        & (VL_MULS_III(32, (IData)(0x00000140U), vlSelfRef.__Vfunc_test_gtext__DOT__gfx_pixel__93__y) 
                                           + vlSelfRef.__Vfunc_test_gtext__DOT__gfx_pixel__93__x)))
                            ? vlSelfRef.test_gtext__DOT__dut__DOT__gfx_inst__DOT__gfx_mem__DOT__mem
                           [(0x0000ffffU & (VL_MULS_III(32, (IData)(0x00000140U), vlSelfRef.__Vfunc_test_gtext__DOT__gfx_pixel__93__y) 
                                            + vlSelfRef.__Vfunc_test_gtext__DOT__gfx_pixel__93__x))]
                            : 0U);
                }(), (IData)(vlSelfRef.__Vfunc_test_gtext__DOT__gfx_pixel__93__Vfuncout)))) {
        vlSelfRef.test_gtext__DOT__unnamedblk12__DOT__pc 
            = ((IData)(1U) + vlSelfRef.test_gtext__DOT__unnamedblk12__DOT__pc);
    }
    if ((0U != ([&]() {
                    vlSelfRef.__Vfunc_test_gtext__DOT__gfx_pixel__93__y = 3U;
                    vlSelfRef.__Vfunc_test_gtext__DOT__gfx_pixel__93__x = 4U;
                    vlSelf->__Vfunc_test_gtext__DOT__gfx_pixel__93__Vfuncout = 0;
                    vlSelfRef.__Vfunc_test_gtext__DOT__gfx_pixel__93__Vfuncout 
                        = ((0xf9ffU >= (0x0000ffffU 
                                        & (VL_MULS_III(32, (IData)(0x00000140U), vlSelfRef.__Vfunc_test_gtext__DOT__gfx_pixel__93__y) 
                                           + vlSelfRef.__Vfunc_test_gtext__DOT__gfx_pixel__93__x)))
                            ? vlSelfRef.test_gtext__DOT__dut__DOT__gfx_inst__DOT__gfx_mem__DOT__mem
                           [(0x0000ffffU & (VL_MULS_III(32, (IData)(0x00000140U), vlSelfRef.__Vfunc_test_gtext__DOT__gfx_pixel__93__y) 
                                            + vlSelfRef.__Vfunc_test_gtext__DOT__gfx_pixel__93__x))]
                            : 0U);
                }(), (IData)(vlSelfRef.__Vfunc_test_gtext__DOT__gfx_pixel__93__Vfuncout)))) {
        vlSelfRef.test_gtext__DOT__unnamedblk12__DOT__pc 
            = ((IData)(1U) + vlSelfRef.test_gtext__DOT__unnamedblk12__DOT__pc);
    }
    if ((0U != ([&]() {
                    vlSelfRef.__Vfunc_test_gtext__DOT__gfx_pixel__93__y = 3U;
                    vlSelfRef.__Vfunc_test_gtext__DOT__gfx_pixel__93__x = 5U;
                    vlSelf->__Vfunc_test_gtext__DOT__gfx_pixel__93__Vfuncout = 0;
                    vlSelfRef.__Vfunc_test_gtext__DOT__gfx_pixel__93__Vfuncout 
                        = ((0xf9ffU >= (0x0000ffffU 
                                        & (VL_MULS_III(32, (IData)(0x00000140U), vlSelfRef.__Vfunc_test_gtext__DOT__gfx_pixel__93__y) 
                                           + vlSelfRef.__Vfunc_test_gtext__DOT__gfx_pixel__93__x)))
                            ? vlSelfRef.test_gtext__DOT__dut__DOT__gfx_inst__DOT__gfx_mem__DOT__mem
                           [(0x0000ffffU & (VL_MULS_III(32, (IData)(0x00000140U), vlSelfRef.__Vfunc_test_gtext__DOT__gfx_pixel__93__y) 
                                            + vlSelfRef.__Vfunc_test_gtext__DOT__gfx_pixel__93__x))]
                            : 0U);
                }(), (IData)(vlSelfRef.__Vfunc_test_gtext__DOT__gfx_pixel__93__Vfuncout)))) {
        vlSelfRef.test_gtext__DOT__unnamedblk12__DOT__pc 
            = ((IData)(1U) + vlSelfRef.test_gtext__DOT__unnamedblk12__DOT__pc);
    }
    if ((0U != ([&]() {
                    vlSelfRef.__Vfunc_test_gtext__DOT__gfx_pixel__93__y = 3U;
                    vlSelfRef.__Vfunc_test_gtext__DOT__gfx_pixel__93__x = 6U;
                    vlSelf->__Vfunc_test_gtext__DOT__gfx_pixel__93__Vfuncout = 0;
                    vlSelfRef.__Vfunc_test_gtext__DOT__gfx_pixel__93__Vfuncout 
                        = ((0xf9ffU >= (0x0000ffffU 
                                        & (VL_MULS_III(32, (IData)(0x00000140U), vlSelfRef.__Vfunc_test_gtext__DOT__gfx_pixel__93__y) 
                                           + vlSelfRef.__Vfunc_test_gtext__DOT__gfx_pixel__93__x)))
                            ? vlSelfRef.test_gtext__DOT__dut__DOT__gfx_inst__DOT__gfx_mem__DOT__mem
                           [(0x0000ffffU & (VL_MULS_III(32, (IData)(0x00000140U), vlSelfRef.__Vfunc_test_gtext__DOT__gfx_pixel__93__y) 
                                            + vlSelfRef.__Vfunc_test_gtext__DOT__gfx_pixel__93__x))]
                            : 0U);
                }(), (IData)(vlSelfRef.__Vfunc_test_gtext__DOT__gfx_pixel__93__Vfuncout)))) {
        vlSelfRef.test_gtext__DOT__unnamedblk12__DOT__pc 
            = ((IData)(1U) + vlSelfRef.test_gtext__DOT__unnamedblk12__DOT__pc);
    }
    if ((0U != ([&]() {
                    vlSelfRef.__Vfunc_test_gtext__DOT__gfx_pixel__93__y = 3U;
                    vlSelfRef.__Vfunc_test_gtext__DOT__gfx_pixel__93__x = 7U;
                    vlSelf->__Vfunc_test_gtext__DOT__gfx_pixel__93__Vfuncout = 0;
                    vlSelfRef.__Vfunc_test_gtext__DOT__gfx_pixel__93__Vfuncout 
                        = ((0xf9ffU >= (0x0000ffffU 
                                        & (VL_MULS_III(32, (IData)(0x00000140U), vlSelfRef.__Vfunc_test_gtext__DOT__gfx_pixel__93__y) 
                                           + vlSelfRef.__Vfunc_test_gtext__DOT__gfx_pixel__93__x)))
                            ? vlSelfRef.test_gtext__DOT__dut__DOT__gfx_inst__DOT__gfx_mem__DOT__mem
                           [(0x0000ffffU & (VL_MULS_III(32, (IData)(0x00000140U), vlSelfRef.__Vfunc_test_gtext__DOT__gfx_pixel__93__y) 
                                            + vlSelfRef.__Vfunc_test_gtext__DOT__gfx_pixel__93__x))]
                            : 0U);
                }(), (IData)(vlSelfRef.__Vfunc_test_gtext__DOT__gfx_pixel__93__Vfuncout)))) {
        vlSelfRef.test_gtext__DOT__unnamedblk12__DOT__pc 
            = ((IData)(1U) + vlSelfRef.test_gtext__DOT__unnamedblk12__DOT__pc);
    }
    if ((0U != ([&]() {
                    vlSelfRef.__Vfunc_test_gtext__DOT__gfx_pixel__93__y = 3U;
                    vlSelfRef.__Vfunc_test_gtext__DOT__gfx_pixel__93__x = 8U;
                    vlSelf->__Vfunc_test_gtext__DOT__gfx_pixel__93__Vfuncout = 0;
                    vlSelfRef.__Vfunc_test_gtext__DOT__gfx_pixel__93__Vfuncout 
                        = ((0xf9ffU >= (0x0000ffffU 
                                        & (VL_MULS_III(32, (IData)(0x00000140U), vlSelfRef.__Vfunc_test_gtext__DOT__gfx_pixel__93__y) 
                                           + vlSelfRef.__Vfunc_test_gtext__DOT__gfx_pixel__93__x)))
                            ? vlSelfRef.test_gtext__DOT__dut__DOT__gfx_inst__DOT__gfx_mem__DOT__mem
                           [(0x0000ffffU & (VL_MULS_III(32, (IData)(0x00000140U), vlSelfRef.__Vfunc_test_gtext__DOT__gfx_pixel__93__y) 
                                            + vlSelfRef.__Vfunc_test_gtext__DOT__gfx_pixel__93__x))]
                            : 0U);
                }(), (IData)(vlSelfRef.__Vfunc_test_gtext__DOT__gfx_pixel__93__Vfuncout)))) {
        vlSelfRef.test_gtext__DOT__unnamedblk12__DOT__pc 
            = ((IData)(1U) + vlSelfRef.test_gtext__DOT__unnamedblk12__DOT__pc);
    }
    if ((0U != ([&]() {
                    vlSelfRef.__Vfunc_test_gtext__DOT__gfx_pixel__93__y = 3U;
                    vlSelfRef.__Vfunc_test_gtext__DOT__gfx_pixel__93__x = 9U;
                    vlSelf->__Vfunc_test_gtext__DOT__gfx_pixel__93__Vfuncout = 0;
                    vlSelfRef.__Vfunc_test_gtext__DOT__gfx_pixel__93__Vfuncout 
                        = ((0xf9ffU >= (0x0000ffffU 
                                        & (VL_MULS_III(32, (IData)(0x00000140U), vlSelfRef.__Vfunc_test_gtext__DOT__gfx_pixel__93__y) 
                                           + vlSelfRef.__Vfunc_test_gtext__DOT__gfx_pixel__93__x)))
                            ? vlSelfRef.test_gtext__DOT__dut__DOT__gfx_inst__DOT__gfx_mem__DOT__mem
                           [(0x0000ffffU & (VL_MULS_III(32, (IData)(0x00000140U), vlSelfRef.__Vfunc_test_gtext__DOT__gfx_pixel__93__y) 
                                            + vlSelfRef.__Vfunc_test_gtext__DOT__gfx_pixel__93__x))]
                            : 0U);
                }(), (IData)(vlSelfRef.__Vfunc_test_gtext__DOT__gfx_pixel__93__Vfuncout)))) {
        vlSelfRef.test_gtext__DOT__unnamedblk12__DOT__pc 
            = ((IData)(1U) + vlSelfRef.test_gtext__DOT__unnamedblk12__DOT__pc);
    }
    if ((0U != ([&]() {
                    vlSelfRef.__Vfunc_test_gtext__DOT__gfx_pixel__93__y = 3U;
                    vlSelfRef.__Vfunc_test_gtext__DOT__gfx_pixel__93__x = 0x0000000aU;
                    vlSelf->__Vfunc_test_gtext__DOT__gfx_pixel__93__Vfuncout = 0;
                    vlSelfRef.__Vfunc_test_gtext__DOT__gfx_pixel__93__Vfuncout 
                        = ((0xf9ffU >= (0x0000ffffU 
                                        & (VL_MULS_III(32, (IData)(0x00000140U), vlSelfRef.__Vfunc_test_gtext__DOT__gfx_pixel__93__y) 
                                           + vlSelfRef.__Vfunc_test_gtext__DOT__gfx_pixel__93__x)))
                            ? vlSelfRef.test_gtext__DOT__dut__DOT__gfx_inst__DOT__gfx_mem__DOT__mem
                           [(0x0000ffffU & (VL_MULS_III(32, (IData)(0x00000140U), vlSelfRef.__Vfunc_test_gtext__DOT__gfx_pixel__93__y) 
                                            + vlSelfRef.__Vfunc_test_gtext__DOT__gfx_pixel__93__x))]
                            : 0U);
                }(), (IData)(vlSelfRef.__Vfunc_test_gtext__DOT__gfx_pixel__93__Vfuncout)))) {
        vlSelfRef.test_gtext__DOT__unnamedblk12__DOT__pc 
            = ((IData)(1U) + vlSelfRef.test_gtext__DOT__unnamedblk12__DOT__pc);
    }
    if ((0U != ([&]() {
                    vlSelfRef.__Vfunc_test_gtext__DOT__gfx_pixel__93__y = 3U;
                    vlSelfRef.__Vfunc_test_gtext__DOT__gfx_pixel__93__x = 0x0000000bU;
                    vlSelf->__Vfunc_test_gtext__DOT__gfx_pixel__93__Vfuncout = 0;
                    vlSelfRef.__Vfunc_test_gtext__DOT__gfx_pixel__93__Vfuncout 
                        = ((0xf9ffU >= (0x0000ffffU 
                                        & (VL_MULS_III(32, (IData)(0x00000140U), vlSelfRef.__Vfunc_test_gtext__DOT__gfx_pixel__93__y) 
                                           + vlSelfRef.__Vfunc_test_gtext__DOT__gfx_pixel__93__x)))
                            ? vlSelfRef.test_gtext__DOT__dut__DOT__gfx_inst__DOT__gfx_mem__DOT__mem
                           [(0x0000ffffU & (VL_MULS_III(32, (IData)(0x00000140U), vlSelfRef.__Vfunc_test_gtext__DOT__gfx_pixel__93__y) 
                                            + vlSelfRef.__Vfunc_test_gtext__DOT__gfx_pixel__93__x))]
                            : 0U);
                }(), (IData)(vlSelfRef.__Vfunc_test_gtext__DOT__gfx_pixel__93__Vfuncout)))) {
        vlSelfRef.test_gtext__DOT__unnamedblk12__DOT__pc 
            = ((IData)(1U) + vlSelfRef.test_gtext__DOT__unnamedblk12__DOT__pc);
    }
    if ((0U != ([&]() {
                    vlSelfRef.__Vfunc_test_gtext__DOT__gfx_pixel__93__y = 3U;
                    vlSelfRef.__Vfunc_test_gtext__DOT__gfx_pixel__93__x = 0x0000000cU;
                    vlSelf->__Vfunc_test_gtext__DOT__gfx_pixel__93__Vfuncout = 0;
                    vlSelfRef.__Vfunc_test_gtext__DOT__gfx_pixel__93__Vfuncout 
                        = ((0xf9ffU >= (0x0000ffffU 
                                        & (VL_MULS_III(32, (IData)(0x00000140U), vlSelfRef.__Vfunc_test_gtext__DOT__gfx_pixel__93__y) 
                                           + vlSelfRef.__Vfunc_test_gtext__DOT__gfx_pixel__93__x)))
                            ? vlSelfRef.test_gtext__DOT__dut__DOT__gfx_inst__DOT__gfx_mem__DOT__mem
                           [(0x0000ffffU & (VL_MULS_III(32, (IData)(0x00000140U), vlSelfRef.__Vfunc_test_gtext__DOT__gfx_pixel__93__y) 
                                            + vlSelfRef.__Vfunc_test_gtext__DOT__gfx_pixel__93__x))]
                            : 0U);
                }(), (IData)(vlSelfRef.__Vfunc_test_gtext__DOT__gfx_pixel__93__Vfuncout)))) {
        vlSelfRef.test_gtext__DOT__unnamedblk12__DOT__pc 
            = ((IData)(1U) + vlSelfRef.test_gtext__DOT__unnamedblk12__DOT__pc);
    }
    if ((0U != ([&]() {
                    vlSelfRef.__Vfunc_test_gtext__DOT__gfx_pixel__93__y = 3U;
                    vlSelfRef.__Vfunc_test_gtext__DOT__gfx_pixel__93__x = 0x0000000dU;
                    vlSelf->__Vfunc_test_gtext__DOT__gfx_pixel__93__Vfuncout = 0;
                    vlSelfRef.__Vfunc_test_gtext__DOT__gfx_pixel__93__Vfuncout 
                        = ((0xf9ffU >= (0x0000ffffU 
                                        & (VL_MULS_III(32, (IData)(0x00000140U), vlSelfRef.__Vfunc_test_gtext__DOT__gfx_pixel__93__y) 
                                           + vlSelfRef.__Vfunc_test_gtext__DOT__gfx_pixel__93__x)))
                            ? vlSelfRef.test_gtext__DOT__dut__DOT__gfx_inst__DOT__gfx_mem__DOT__mem
                           [(0x0000ffffU & (VL_MULS_III(32, (IData)(0x00000140U), vlSelfRef.__Vfunc_test_gtext__DOT__gfx_pixel__93__y) 
                                            + vlSelfRef.__Vfunc_test_gtext__DOT__gfx_pixel__93__x))]
                            : 0U);
                }(), (IData)(vlSelfRef.__Vfunc_test_gtext__DOT__gfx_pixel__93__Vfuncout)))) {
        vlSelfRef.test_gtext__DOT__unnamedblk12__DOT__pc 
            = ((IData)(1U) + vlSelfRef.test_gtext__DOT__unnamedblk12__DOT__pc);
    }
    if ((0U != ([&]() {
                    vlSelfRef.__Vfunc_test_gtext__DOT__gfx_pixel__93__y = 3U;
                    vlSelfRef.__Vfunc_test_gtext__DOT__gfx_pixel__93__x = 0x0000000eU;
                    vlSelf->__Vfunc_test_gtext__DOT__gfx_pixel__93__Vfuncout = 0;
                    vlSelfRef.__Vfunc_test_gtext__DOT__gfx_pixel__93__Vfuncout 
                        = ((0xf9ffU >= (0x0000ffffU 
                                        & (VL_MULS_III(32, (IData)(0x00000140U), vlSelfRef.__Vfunc_test_gtext__DOT__gfx_pixel__93__y) 
                                           + vlSelfRef.__Vfunc_test_gtext__DOT__gfx_pixel__93__x)))
                            ? vlSelfRef.test_gtext__DOT__dut__DOT__gfx_inst__DOT__gfx_mem__DOT__mem
                           [(0x0000ffffU & (VL_MULS_III(32, (IData)(0x00000140U), vlSelfRef.__Vfunc_test_gtext__DOT__gfx_pixel__93__y) 
                                            + vlSelfRef.__Vfunc_test_gtext__DOT__gfx_pixel__93__x))]
                            : 0U);
                }(), (IData)(vlSelfRef.__Vfunc_test_gtext__DOT__gfx_pixel__93__Vfuncout)))) {
        vlSelfRef.test_gtext__DOT__unnamedblk12__DOT__pc 
            = ((IData)(1U) + vlSelfRef.test_gtext__DOT__unnamedblk12__DOT__pc);
    }
    if ((0U != ([&]() {
                    vlSelfRef.__Vfunc_test_gtext__DOT__gfx_pixel__93__y = 3U;
                    vlSelfRef.__Vfunc_test_gtext__DOT__gfx_pixel__93__x = 0x0000000fU;
                    vlSelf->__Vfunc_test_gtext__DOT__gfx_pixel__93__Vfuncout = 0;
                    vlSelfRef.__Vfunc_test_gtext__DOT__gfx_pixel__93__Vfuncout 
                        = ((0xf9ffU >= (0x0000ffffU 
                                        & (VL_MULS_III(32, (IData)(0x00000140U), vlSelfRef.__Vfunc_test_gtext__DOT__gfx_pixel__93__y) 
                                           + vlSelfRef.__Vfunc_test_gtext__DOT__gfx_pixel__93__x)))
                            ? vlSelfRef.test_gtext__DOT__dut__DOT__gfx_inst__DOT__gfx_mem__DOT__mem
                           [(0x0000ffffU & (VL_MULS_III(32, (IData)(0x00000140U), vlSelfRef.__Vfunc_test_gtext__DOT__gfx_pixel__93__y) 
                                            + vlSelfRef.__Vfunc_test_gtext__DOT__gfx_pixel__93__x))]
                            : 0U);
                }(), (IData)(vlSelfRef.__Vfunc_test_gtext__DOT__gfx_pixel__93__Vfuncout)))) {
        vlSelfRef.test_gtext__DOT__unnamedblk12__DOT__pc 
            = ((IData)(1U) + vlSelfRef.test_gtext__DOT__unnamedblk12__DOT__pc);
    }
    if ((0U != ([&]() {
                    vlSelfRef.__Vfunc_test_gtext__DOT__gfx_pixel__93__y = 4U;
                    vlSelfRef.__Vfunc_test_gtext__DOT__gfx_pixel__93__x = 0U;
                    vlSelf->__Vfunc_test_gtext__DOT__gfx_pixel__93__Vfuncout = 0;
                    vlSelfRef.__Vfunc_test_gtext__DOT__gfx_pixel__93__Vfuncout 
                        = ((0xf9ffU >= (0x0000ffffU 
                                        & (VL_MULS_III(32, (IData)(0x00000140U), vlSelfRef.__Vfunc_test_gtext__DOT__gfx_pixel__93__y) 
                                           + vlSelfRef.__Vfunc_test_gtext__DOT__gfx_pixel__93__x)))
                            ? vlSelfRef.test_gtext__DOT__dut__DOT__gfx_inst__DOT__gfx_mem__DOT__mem
                           [(0x0000ffffU & (VL_MULS_III(32, (IData)(0x00000140U), vlSelfRef.__Vfunc_test_gtext__DOT__gfx_pixel__93__y) 
                                            + vlSelfRef.__Vfunc_test_gtext__DOT__gfx_pixel__93__x))]
                            : 0U);
                }(), (IData)(vlSelfRef.__Vfunc_test_gtext__DOT__gfx_pixel__93__Vfuncout)))) {
        vlSelfRef.test_gtext__DOT__unnamedblk12__DOT__pc 
            = ((IData)(1U) + vlSelfRef.test_gtext__DOT__unnamedblk12__DOT__pc);
    }
    if ((0U != ([&]() {
                    vlSelfRef.__Vfunc_test_gtext__DOT__gfx_pixel__93__y = 4U;
                    vlSelfRef.__Vfunc_test_gtext__DOT__gfx_pixel__93__x = 1U;
                    vlSelf->__Vfunc_test_gtext__DOT__gfx_pixel__93__Vfuncout = 0;
                    vlSelfRef.__Vfunc_test_gtext__DOT__gfx_pixel__93__Vfuncout 
                        = ((0xf9ffU >= (0x0000ffffU 
                                        & (VL_MULS_III(32, (IData)(0x00000140U), vlSelfRef.__Vfunc_test_gtext__DOT__gfx_pixel__93__y) 
                                           + vlSelfRef.__Vfunc_test_gtext__DOT__gfx_pixel__93__x)))
                            ? vlSelfRef.test_gtext__DOT__dut__DOT__gfx_inst__DOT__gfx_mem__DOT__mem
                           [(0x0000ffffU & (VL_MULS_III(32, (IData)(0x00000140U), vlSelfRef.__Vfunc_test_gtext__DOT__gfx_pixel__93__y) 
                                            + vlSelfRef.__Vfunc_test_gtext__DOT__gfx_pixel__93__x))]
                            : 0U);
                }(), (IData)(vlSelfRef.__Vfunc_test_gtext__DOT__gfx_pixel__93__Vfuncout)))) {
        vlSelfRef.test_gtext__DOT__unnamedblk12__DOT__pc 
            = ((IData)(1U) + vlSelfRef.test_gtext__DOT__unnamedblk12__DOT__pc);
    }
    if ((0U != ([&]() {
                    vlSelfRef.__Vfunc_test_gtext__DOT__gfx_pixel__93__y = 4U;
                    vlSelfRef.__Vfunc_test_gtext__DOT__gfx_pixel__93__x = 2U;
                    vlSelf->__Vfunc_test_gtext__DOT__gfx_pixel__93__Vfuncout = 0;
                    vlSelfRef.__Vfunc_test_gtext__DOT__gfx_pixel__93__Vfuncout 
                        = ((0xf9ffU >= (0x0000ffffU 
                                        & (VL_MULS_III(32, (IData)(0x00000140U), vlSelfRef.__Vfunc_test_gtext__DOT__gfx_pixel__93__y) 
                                           + vlSelfRef.__Vfunc_test_gtext__DOT__gfx_pixel__93__x)))
                            ? vlSelfRef.test_gtext__DOT__dut__DOT__gfx_inst__DOT__gfx_mem__DOT__mem
                           [(0x0000ffffU & (VL_MULS_III(32, (IData)(0x00000140U), vlSelfRef.__Vfunc_test_gtext__DOT__gfx_pixel__93__y) 
                                            + vlSelfRef.__Vfunc_test_gtext__DOT__gfx_pixel__93__x))]
                            : 0U);
                }(), (IData)(vlSelfRef.__Vfunc_test_gtext__DOT__gfx_pixel__93__Vfuncout)))) {
        vlSelfRef.test_gtext__DOT__unnamedblk12__DOT__pc 
            = ((IData)(1U) + vlSelfRef.test_gtext__DOT__unnamedblk12__DOT__pc);
    }
    if ((0U != ([&]() {
                    vlSelfRef.__Vfunc_test_gtext__DOT__gfx_pixel__93__y = 4U;
                    vlSelfRef.__Vfunc_test_gtext__DOT__gfx_pixel__93__x = 3U;
                    vlSelf->__Vfunc_test_gtext__DOT__gfx_pixel__93__Vfuncout = 0;
                    vlSelfRef.__Vfunc_test_gtext__DOT__gfx_pixel__93__Vfuncout 
                        = ((0xf9ffU >= (0x0000ffffU 
                                        & (VL_MULS_III(32, (IData)(0x00000140U), vlSelfRef.__Vfunc_test_gtext__DOT__gfx_pixel__93__y) 
                                           + vlSelfRef.__Vfunc_test_gtext__DOT__gfx_pixel__93__x)))
                            ? vlSelfRef.test_gtext__DOT__dut__DOT__gfx_inst__DOT__gfx_mem__DOT__mem
                           [(0x0000ffffU & (VL_MULS_III(32, (IData)(0x00000140U), vlSelfRef.__Vfunc_test_gtext__DOT__gfx_pixel__93__y) 
                                            + vlSelfRef.__Vfunc_test_gtext__DOT__gfx_pixel__93__x))]
                            : 0U);
                }(), (IData)(vlSelfRef.__Vfunc_test_gtext__DOT__gfx_pixel__93__Vfuncout)))) {
        vlSelfRef.test_gtext__DOT__unnamedblk12__DOT__pc 
            = ((IData)(1U) + vlSelfRef.test_gtext__DOT__unnamedblk12__DOT__pc);
    }
    if ((0U != ([&]() {
                    vlSelfRef.__Vfunc_test_gtext__DOT__gfx_pixel__93__y = 4U;
                    vlSelfRef.__Vfunc_test_gtext__DOT__gfx_pixel__93__x = 4U;
                    vlSelf->__Vfunc_test_gtext__DOT__gfx_pixel__93__Vfuncout = 0;
                    vlSelfRef.__Vfunc_test_gtext__DOT__gfx_pixel__93__Vfuncout 
                        = ((0xf9ffU >= (0x0000ffffU 
                                        & (VL_MULS_III(32, (IData)(0x00000140U), vlSelfRef.__Vfunc_test_gtext__DOT__gfx_pixel__93__y) 
                                           + vlSelfRef.__Vfunc_test_gtext__DOT__gfx_pixel__93__x)))
                            ? vlSelfRef.test_gtext__DOT__dut__DOT__gfx_inst__DOT__gfx_mem__DOT__mem
                           [(0x0000ffffU & (VL_MULS_III(32, (IData)(0x00000140U), vlSelfRef.__Vfunc_test_gtext__DOT__gfx_pixel__93__y) 
                                            + vlSelfRef.__Vfunc_test_gtext__DOT__gfx_pixel__93__x))]
                            : 0U);
                }(), (IData)(vlSelfRef.__Vfunc_test_gtext__DOT__gfx_pixel__93__Vfuncout)))) {
        vlSelfRef.test_gtext__DOT__unnamedblk12__DOT__pc 
            = ((IData)(1U) + vlSelfRef.test_gtext__DOT__unnamedblk12__DOT__pc);
    }
    if ((0U != ([&]() {
                    vlSelfRef.__Vfunc_test_gtext__DOT__gfx_pixel__93__y = 4U;
                    vlSelfRef.__Vfunc_test_gtext__DOT__gfx_pixel__93__x = 5U;
                    vlSelf->__Vfunc_test_gtext__DOT__gfx_pixel__93__Vfuncout = 0;
                    vlSelfRef.__Vfunc_test_gtext__DOT__gfx_pixel__93__Vfuncout 
                        = ((0xf9ffU >= (0x0000ffffU 
                                        & (VL_MULS_III(32, (IData)(0x00000140U), vlSelfRef.__Vfunc_test_gtext__DOT__gfx_pixel__93__y) 
                                           + vlSelfRef.__Vfunc_test_gtext__DOT__gfx_pixel__93__x)))
                            ? vlSelfRef.test_gtext__DOT__dut__DOT__gfx_inst__DOT__gfx_mem__DOT__mem
                           [(0x0000ffffU & (VL_MULS_III(32, (IData)(0x00000140U), vlSelfRef.__Vfunc_test_gtext__DOT__gfx_pixel__93__y) 
                                            + vlSelfRef.__Vfunc_test_gtext__DOT__gfx_pixel__93__x))]
                            : 0U);
                }(), (IData)(vlSelfRef.__Vfunc_test_gtext__DOT__gfx_pixel__93__Vfuncout)))) {
        vlSelfRef.test_gtext__DOT__unnamedblk12__DOT__pc 
            = ((IData)(1U) + vlSelfRef.test_gtext__DOT__unnamedblk12__DOT__pc);
    }
    if ((0U != ([&]() {
                    vlSelfRef.__Vfunc_test_gtext__DOT__gfx_pixel__93__y = 4U;
                    vlSelfRef.__Vfunc_test_gtext__DOT__gfx_pixel__93__x = 6U;
                    vlSelf->__Vfunc_test_gtext__DOT__gfx_pixel__93__Vfuncout = 0;
                    vlSelfRef.__Vfunc_test_gtext__DOT__gfx_pixel__93__Vfuncout 
                        = ((0xf9ffU >= (0x0000ffffU 
                                        & (VL_MULS_III(32, (IData)(0x00000140U), vlSelfRef.__Vfunc_test_gtext__DOT__gfx_pixel__93__y) 
                                           + vlSelfRef.__Vfunc_test_gtext__DOT__gfx_pixel__93__x)))
                            ? vlSelfRef.test_gtext__DOT__dut__DOT__gfx_inst__DOT__gfx_mem__DOT__mem
                           [(0x0000ffffU & (VL_MULS_III(32, (IData)(0x00000140U), vlSelfRef.__Vfunc_test_gtext__DOT__gfx_pixel__93__y) 
                                            + vlSelfRef.__Vfunc_test_gtext__DOT__gfx_pixel__93__x))]
                            : 0U);
                }(), (IData)(vlSelfRef.__Vfunc_test_gtext__DOT__gfx_pixel__93__Vfuncout)))) {
        vlSelfRef.test_gtext__DOT__unnamedblk12__DOT__pc 
            = ((IData)(1U) + vlSelfRef.test_gtext__DOT__unnamedblk12__DOT__pc);
    }
    if ((0U != ([&]() {
                    vlSelfRef.__Vfunc_test_gtext__DOT__gfx_pixel__93__y = 4U;
                    vlSelfRef.__Vfunc_test_gtext__DOT__gfx_pixel__93__x = 7U;
                    vlSelf->__Vfunc_test_gtext__DOT__gfx_pixel__93__Vfuncout = 0;
                    vlSelfRef.__Vfunc_test_gtext__DOT__gfx_pixel__93__Vfuncout 
                        = ((0xf9ffU >= (0x0000ffffU 
                                        & (VL_MULS_III(32, (IData)(0x00000140U), vlSelfRef.__Vfunc_test_gtext__DOT__gfx_pixel__93__y) 
                                           + vlSelfRef.__Vfunc_test_gtext__DOT__gfx_pixel__93__x)))
                            ? vlSelfRef.test_gtext__DOT__dut__DOT__gfx_inst__DOT__gfx_mem__DOT__mem
                           [(0x0000ffffU & (VL_MULS_III(32, (IData)(0x00000140U), vlSelfRef.__Vfunc_test_gtext__DOT__gfx_pixel__93__y) 
                                            + vlSelfRef.__Vfunc_test_gtext__DOT__gfx_pixel__93__x))]
                            : 0U);
                }(), (IData)(vlSelfRef.__Vfunc_test_gtext__DOT__gfx_pixel__93__Vfuncout)))) {
        vlSelfRef.test_gtext__DOT__unnamedblk12__DOT__pc 
            = ((IData)(1U) + vlSelfRef.test_gtext__DOT__unnamedblk12__DOT__pc);
    }
    if ((0U != ([&]() {
                    vlSelfRef.__Vfunc_test_gtext__DOT__gfx_pixel__93__y = 4U;
                    vlSelfRef.__Vfunc_test_gtext__DOT__gfx_pixel__93__x = 8U;
                    vlSelf->__Vfunc_test_gtext__DOT__gfx_pixel__93__Vfuncout = 0;
                    vlSelfRef.__Vfunc_test_gtext__DOT__gfx_pixel__93__Vfuncout 
                        = ((0xf9ffU >= (0x0000ffffU 
                                        & (VL_MULS_III(32, (IData)(0x00000140U), vlSelfRef.__Vfunc_test_gtext__DOT__gfx_pixel__93__y) 
                                           + vlSelfRef.__Vfunc_test_gtext__DOT__gfx_pixel__93__x)))
                            ? vlSelfRef.test_gtext__DOT__dut__DOT__gfx_inst__DOT__gfx_mem__DOT__mem
                           [(0x0000ffffU & (VL_MULS_III(32, (IData)(0x00000140U), vlSelfRef.__Vfunc_test_gtext__DOT__gfx_pixel__93__y) 
                                            + vlSelfRef.__Vfunc_test_gtext__DOT__gfx_pixel__93__x))]
                            : 0U);
                }(), (IData)(vlSelfRef.__Vfunc_test_gtext__DOT__gfx_pixel__93__Vfuncout)))) {
        vlSelfRef.test_gtext__DOT__unnamedblk12__DOT__pc 
            = ((IData)(1U) + vlSelfRef.test_gtext__DOT__unnamedblk12__DOT__pc);
    }
    if ((0U != ([&]() {
                    vlSelfRef.__Vfunc_test_gtext__DOT__gfx_pixel__93__y = 4U;
                    vlSelfRef.__Vfunc_test_gtext__DOT__gfx_pixel__93__x = 9U;
                    vlSelf->__Vfunc_test_gtext__DOT__gfx_pixel__93__Vfuncout = 0;
                    vlSelfRef.__Vfunc_test_gtext__DOT__gfx_pixel__93__Vfuncout 
                        = ((0xf9ffU >= (0x0000ffffU 
                                        & (VL_MULS_III(32, (IData)(0x00000140U), vlSelfRef.__Vfunc_test_gtext__DOT__gfx_pixel__93__y) 
                                           + vlSelfRef.__Vfunc_test_gtext__DOT__gfx_pixel__93__x)))
                            ? vlSelfRef.test_gtext__DOT__dut__DOT__gfx_inst__DOT__gfx_mem__DOT__mem
                           [(0x0000ffffU & (VL_MULS_III(32, (IData)(0x00000140U), vlSelfRef.__Vfunc_test_gtext__DOT__gfx_pixel__93__y) 
                                            + vlSelfRef.__Vfunc_test_gtext__DOT__gfx_pixel__93__x))]
                            : 0U);
                }(), (IData)(vlSelfRef.__Vfunc_test_gtext__DOT__gfx_pixel__93__Vfuncout)))) {
        vlSelfRef.test_gtext__DOT__unnamedblk12__DOT__pc 
            = ((IData)(1U) + vlSelfRef.test_gtext__DOT__unnamedblk12__DOT__pc);
    }
    if ((0U != ([&]() {
                    vlSelfRef.__Vfunc_test_gtext__DOT__gfx_pixel__93__y = 4U;
                    vlSelfRef.__Vfunc_test_gtext__DOT__gfx_pixel__93__x = 0x0000000aU;
                    vlSelf->__Vfunc_test_gtext__DOT__gfx_pixel__93__Vfuncout = 0;
                    vlSelfRef.__Vfunc_test_gtext__DOT__gfx_pixel__93__Vfuncout 
                        = ((0xf9ffU >= (0x0000ffffU 
                                        & (VL_MULS_III(32, (IData)(0x00000140U), vlSelfRef.__Vfunc_test_gtext__DOT__gfx_pixel__93__y) 
                                           + vlSelfRef.__Vfunc_test_gtext__DOT__gfx_pixel__93__x)))
                            ? vlSelfRef.test_gtext__DOT__dut__DOT__gfx_inst__DOT__gfx_mem__DOT__mem
                           [(0x0000ffffU & (VL_MULS_III(32, (IData)(0x00000140U), vlSelfRef.__Vfunc_test_gtext__DOT__gfx_pixel__93__y) 
                                            + vlSelfRef.__Vfunc_test_gtext__DOT__gfx_pixel__93__x))]
                            : 0U);
                }(), (IData)(vlSelfRef.__Vfunc_test_gtext__DOT__gfx_pixel__93__Vfuncout)))) {
        vlSelfRef.test_gtext__DOT__unnamedblk12__DOT__pc 
            = ((IData)(1U) + vlSelfRef.test_gtext__DOT__unnamedblk12__DOT__pc);
    }
    if ((0U != ([&]() {
                    vlSelfRef.__Vfunc_test_gtext__DOT__gfx_pixel__93__y = 4U;
                    vlSelfRef.__Vfunc_test_gtext__DOT__gfx_pixel__93__x = 0x0000000bU;
                    vlSelf->__Vfunc_test_gtext__DOT__gfx_pixel__93__Vfuncout = 0;
                    vlSelfRef.__Vfunc_test_gtext__DOT__gfx_pixel__93__Vfuncout 
                        = ((0xf9ffU >= (0x0000ffffU 
                                        & (VL_MULS_III(32, (IData)(0x00000140U), vlSelfRef.__Vfunc_test_gtext__DOT__gfx_pixel__93__y) 
                                           + vlSelfRef.__Vfunc_test_gtext__DOT__gfx_pixel__93__x)))
                            ? vlSelfRef.test_gtext__DOT__dut__DOT__gfx_inst__DOT__gfx_mem__DOT__mem
                           [(0x0000ffffU & (VL_MULS_III(32, (IData)(0x00000140U), vlSelfRef.__Vfunc_test_gtext__DOT__gfx_pixel__93__y) 
                                            + vlSelfRef.__Vfunc_test_gtext__DOT__gfx_pixel__93__x))]
                            : 0U);
                }(), (IData)(vlSelfRef.__Vfunc_test_gtext__DOT__gfx_pixel__93__Vfuncout)))) {
        vlSelfRef.test_gtext__DOT__unnamedblk12__DOT__pc 
            = ((IData)(1U) + vlSelfRef.test_gtext__DOT__unnamedblk12__DOT__pc);
    }
    if ((0U != ([&]() {
                    vlSelfRef.__Vfunc_test_gtext__DOT__gfx_pixel__93__y = 4U;
                    vlSelfRef.__Vfunc_test_gtext__DOT__gfx_pixel__93__x = 0x0000000cU;
                    vlSelf->__Vfunc_test_gtext__DOT__gfx_pixel__93__Vfuncout = 0;
                    vlSelfRef.__Vfunc_test_gtext__DOT__gfx_pixel__93__Vfuncout 
                        = ((0xf9ffU >= (0x0000ffffU 
                                        & (VL_MULS_III(32, (IData)(0x00000140U), vlSelfRef.__Vfunc_test_gtext__DOT__gfx_pixel__93__y) 
                                           + vlSelfRef.__Vfunc_test_gtext__DOT__gfx_pixel__93__x)))
                            ? vlSelfRef.test_gtext__DOT__dut__DOT__gfx_inst__DOT__gfx_mem__DOT__mem
                           [(0x0000ffffU & (VL_MULS_III(32, (IData)(0x00000140U), vlSelfRef.__Vfunc_test_gtext__DOT__gfx_pixel__93__y) 
                                            + vlSelfRef.__Vfunc_test_gtext__DOT__gfx_pixel__93__x))]
                            : 0U);
                }(), (IData)(vlSelfRef.__Vfunc_test_gtext__DOT__gfx_pixel__93__Vfuncout)))) {
        vlSelfRef.test_gtext__DOT__unnamedblk12__DOT__pc 
            = ((IData)(1U) + vlSelfRef.test_gtext__DOT__unnamedblk12__DOT__pc);
    }
    if ((0U != ([&]() {
                    vlSelfRef.__Vfunc_test_gtext__DOT__gfx_pixel__93__y = 4U;
                    vlSelfRef.__Vfunc_test_gtext__DOT__gfx_pixel__93__x = 0x0000000dU;
                    vlSelf->__Vfunc_test_gtext__DOT__gfx_pixel__93__Vfuncout = 0;
                    vlSelfRef.__Vfunc_test_gtext__DOT__gfx_pixel__93__Vfuncout 
                        = ((0xf9ffU >= (0x0000ffffU 
                                        & (VL_MULS_III(32, (IData)(0x00000140U), vlSelfRef.__Vfunc_test_gtext__DOT__gfx_pixel__93__y) 
                                           + vlSelfRef.__Vfunc_test_gtext__DOT__gfx_pixel__93__x)))
                            ? vlSelfRef.test_gtext__DOT__dut__DOT__gfx_inst__DOT__gfx_mem__DOT__mem
                           [(0x0000ffffU & (VL_MULS_III(32, (IData)(0x00000140U), vlSelfRef.__Vfunc_test_gtext__DOT__gfx_pixel__93__y) 
                                            + vlSelfRef.__Vfunc_test_gtext__DOT__gfx_pixel__93__x))]
                            : 0U);
                }(), (IData)(vlSelfRef.__Vfunc_test_gtext__DOT__gfx_pixel__93__Vfuncout)))) {
        vlSelfRef.test_gtext__DOT__unnamedblk12__DOT__pc 
            = ((IData)(1U) + vlSelfRef.test_gtext__DOT__unnamedblk12__DOT__pc);
    }
    if ((0U != ([&]() {
                    vlSelfRef.__Vfunc_test_gtext__DOT__gfx_pixel__93__y = 4U;
                    vlSelfRef.__Vfunc_test_gtext__DOT__gfx_pixel__93__x = 0x0000000eU;
                    vlSelf->__Vfunc_test_gtext__DOT__gfx_pixel__93__Vfuncout = 0;
                    vlSelfRef.__Vfunc_test_gtext__DOT__gfx_pixel__93__Vfuncout 
                        = ((0xf9ffU >= (0x0000ffffU 
                                        & (VL_MULS_III(32, (IData)(0x00000140U), vlSelfRef.__Vfunc_test_gtext__DOT__gfx_pixel__93__y) 
                                           + vlSelfRef.__Vfunc_test_gtext__DOT__gfx_pixel__93__x)))
                            ? vlSelfRef.test_gtext__DOT__dut__DOT__gfx_inst__DOT__gfx_mem__DOT__mem
                           [(0x0000ffffU & (VL_MULS_III(32, (IData)(0x00000140U), vlSelfRef.__Vfunc_test_gtext__DOT__gfx_pixel__93__y) 
                                            + vlSelfRef.__Vfunc_test_gtext__DOT__gfx_pixel__93__x))]
                            : 0U);
                }(), (IData)(vlSelfRef.__Vfunc_test_gtext__DOT__gfx_pixel__93__Vfuncout)))) {
        vlSelfRef.test_gtext__DOT__unnamedblk12__DOT__pc 
            = ((IData)(1U) + vlSelfRef.test_gtext__DOT__unnamedblk12__DOT__pc);
    }
    if ((0U != ([&]() {
                    vlSelfRef.__Vfunc_test_gtext__DOT__gfx_pixel__93__y = 4U;
                    vlSelfRef.__Vfunc_test_gtext__DOT__gfx_pixel__93__x = 0x0000000fU;
                    vlSelf->__Vfunc_test_gtext__DOT__gfx_pixel__93__Vfuncout = 0;
                    vlSelfRef.__Vfunc_test_gtext__DOT__gfx_pixel__93__Vfuncout 
                        = ((0xf9ffU >= (0x0000ffffU 
                                        & (VL_MULS_III(32, (IData)(0x00000140U), vlSelfRef.__Vfunc_test_gtext__DOT__gfx_pixel__93__y) 
                                           + vlSelfRef.__Vfunc_test_gtext__DOT__gfx_pixel__93__x)))
                            ? vlSelfRef.test_gtext__DOT__dut__DOT__gfx_inst__DOT__gfx_mem__DOT__mem
                           [(0x0000ffffU & (VL_MULS_III(32, (IData)(0x00000140U), vlSelfRef.__Vfunc_test_gtext__DOT__gfx_pixel__93__y) 
                                            + vlSelfRef.__Vfunc_test_gtext__DOT__gfx_pixel__93__x))]
                            : 0U);
                }(), (IData)(vlSelfRef.__Vfunc_test_gtext__DOT__gfx_pixel__93__Vfuncout)))) {
        vlSelfRef.test_gtext__DOT__unnamedblk12__DOT__pc 
            = ((IData)(1U) + vlSelfRef.test_gtext__DOT__unnamedblk12__DOT__pc);
    }
    if ((0U != ([&]() {
                    vlSelfRef.__Vfunc_test_gtext__DOT__gfx_pixel__93__y = 5U;
                    vlSelfRef.__Vfunc_test_gtext__DOT__gfx_pixel__93__x = 0U;
                    vlSelf->__Vfunc_test_gtext__DOT__gfx_pixel__93__Vfuncout = 0;
                    vlSelfRef.__Vfunc_test_gtext__DOT__gfx_pixel__93__Vfuncout 
                        = ((0xf9ffU >= (0x0000ffffU 
                                        & (VL_MULS_III(32, (IData)(0x00000140U), vlSelfRef.__Vfunc_test_gtext__DOT__gfx_pixel__93__y) 
                                           + vlSelfRef.__Vfunc_test_gtext__DOT__gfx_pixel__93__x)))
                            ? vlSelfRef.test_gtext__DOT__dut__DOT__gfx_inst__DOT__gfx_mem__DOT__mem
                           [(0x0000ffffU & (VL_MULS_III(32, (IData)(0x00000140U), vlSelfRef.__Vfunc_test_gtext__DOT__gfx_pixel__93__y) 
                                            + vlSelfRef.__Vfunc_test_gtext__DOT__gfx_pixel__93__x))]
                            : 0U);
                }(), (IData)(vlSelfRef.__Vfunc_test_gtext__DOT__gfx_pixel__93__Vfuncout)))) {
        vlSelfRef.test_gtext__DOT__unnamedblk12__DOT__pc 
            = ((IData)(1U) + vlSelfRef.test_gtext__DOT__unnamedblk12__DOT__pc);
    }
    if ((0U != ([&]() {
                    vlSelfRef.__Vfunc_test_gtext__DOT__gfx_pixel__93__y = 5U;
                    vlSelfRef.__Vfunc_test_gtext__DOT__gfx_pixel__93__x = 1U;
                    vlSelf->__Vfunc_test_gtext__DOT__gfx_pixel__93__Vfuncout = 0;
                    vlSelfRef.__Vfunc_test_gtext__DOT__gfx_pixel__93__Vfuncout 
                        = ((0xf9ffU >= (0x0000ffffU 
                                        & (VL_MULS_III(32, (IData)(0x00000140U), vlSelfRef.__Vfunc_test_gtext__DOT__gfx_pixel__93__y) 
                                           + vlSelfRef.__Vfunc_test_gtext__DOT__gfx_pixel__93__x)))
                            ? vlSelfRef.test_gtext__DOT__dut__DOT__gfx_inst__DOT__gfx_mem__DOT__mem
                           [(0x0000ffffU & (VL_MULS_III(32, (IData)(0x00000140U), vlSelfRef.__Vfunc_test_gtext__DOT__gfx_pixel__93__y) 
                                            + vlSelfRef.__Vfunc_test_gtext__DOT__gfx_pixel__93__x))]
                            : 0U);
                }(), (IData)(vlSelfRef.__Vfunc_test_gtext__DOT__gfx_pixel__93__Vfuncout)))) {
        vlSelfRef.test_gtext__DOT__unnamedblk12__DOT__pc 
            = ((IData)(1U) + vlSelfRef.test_gtext__DOT__unnamedblk12__DOT__pc);
    }
    if ((0U != ([&]() {
                    vlSelfRef.__Vfunc_test_gtext__DOT__gfx_pixel__93__y = 5U;
                    vlSelfRef.__Vfunc_test_gtext__DOT__gfx_pixel__93__x = 2U;
                    vlSelf->__Vfunc_test_gtext__DOT__gfx_pixel__93__Vfuncout = 0;
                    vlSelfRef.__Vfunc_test_gtext__DOT__gfx_pixel__93__Vfuncout 
                        = ((0xf9ffU >= (0x0000ffffU 
                                        & (VL_MULS_III(32, (IData)(0x00000140U), vlSelfRef.__Vfunc_test_gtext__DOT__gfx_pixel__93__y) 
                                           + vlSelfRef.__Vfunc_test_gtext__DOT__gfx_pixel__93__x)))
                            ? vlSelfRef.test_gtext__DOT__dut__DOT__gfx_inst__DOT__gfx_mem__DOT__mem
                           [(0x0000ffffU & (VL_MULS_III(32, (IData)(0x00000140U), vlSelfRef.__Vfunc_test_gtext__DOT__gfx_pixel__93__y) 
                                            + vlSelfRef.__Vfunc_test_gtext__DOT__gfx_pixel__93__x))]
                            : 0U);
                }(), (IData)(vlSelfRef.__Vfunc_test_gtext__DOT__gfx_pixel__93__Vfuncout)))) {
        vlSelfRef.test_gtext__DOT__unnamedblk12__DOT__pc 
            = ((IData)(1U) + vlSelfRef.test_gtext__DOT__unnamedblk12__DOT__pc);
    }
    if ((0U != ([&]() {
                    vlSelfRef.__Vfunc_test_gtext__DOT__gfx_pixel__93__y = 5U;
                    vlSelfRef.__Vfunc_test_gtext__DOT__gfx_pixel__93__x = 3U;
                    vlSelf->__Vfunc_test_gtext__DOT__gfx_pixel__93__Vfuncout = 0;
                    vlSelfRef.__Vfunc_test_gtext__DOT__gfx_pixel__93__Vfuncout 
                        = ((0xf9ffU >= (0x0000ffffU 
                                        & (VL_MULS_III(32, (IData)(0x00000140U), vlSelfRef.__Vfunc_test_gtext__DOT__gfx_pixel__93__y) 
                                           + vlSelfRef.__Vfunc_test_gtext__DOT__gfx_pixel__93__x)))
                            ? vlSelfRef.test_gtext__DOT__dut__DOT__gfx_inst__DOT__gfx_mem__DOT__mem
                           [(0x0000ffffU & (VL_MULS_III(32, (IData)(0x00000140U), vlSelfRef.__Vfunc_test_gtext__DOT__gfx_pixel__93__y) 
                                            + vlSelfRef.__Vfunc_test_gtext__DOT__gfx_pixel__93__x))]
                            : 0U);
                }(), (IData)(vlSelfRef.__Vfunc_test_gtext__DOT__gfx_pixel__93__Vfuncout)))) {
        vlSelfRef.test_gtext__DOT__unnamedblk12__DOT__pc 
            = ((IData)(1U) + vlSelfRef.test_gtext__DOT__unnamedblk12__DOT__pc);
    }
    if ((0U != ([&]() {
                    vlSelfRef.__Vfunc_test_gtext__DOT__gfx_pixel__93__y = 5U;
                    vlSelfRef.__Vfunc_test_gtext__DOT__gfx_pixel__93__x = 4U;
                    vlSelf->__Vfunc_test_gtext__DOT__gfx_pixel__93__Vfuncout = 0;
                    vlSelfRef.__Vfunc_test_gtext__DOT__gfx_pixel__93__Vfuncout 
                        = ((0xf9ffU >= (0x0000ffffU 
                                        & (VL_MULS_III(32, (IData)(0x00000140U), vlSelfRef.__Vfunc_test_gtext__DOT__gfx_pixel__93__y) 
                                           + vlSelfRef.__Vfunc_test_gtext__DOT__gfx_pixel__93__x)))
                            ? vlSelfRef.test_gtext__DOT__dut__DOT__gfx_inst__DOT__gfx_mem__DOT__mem
                           [(0x0000ffffU & (VL_MULS_III(32, (IData)(0x00000140U), vlSelfRef.__Vfunc_test_gtext__DOT__gfx_pixel__93__y) 
                                            + vlSelfRef.__Vfunc_test_gtext__DOT__gfx_pixel__93__x))]
                            : 0U);
                }(), (IData)(vlSelfRef.__Vfunc_test_gtext__DOT__gfx_pixel__93__Vfuncout)))) {
        vlSelfRef.test_gtext__DOT__unnamedblk12__DOT__pc 
            = ((IData)(1U) + vlSelfRef.test_gtext__DOT__unnamedblk12__DOT__pc);
    }
    if ((0U != ([&]() {
                    vlSelfRef.__Vfunc_test_gtext__DOT__gfx_pixel__93__y = 5U;
                    vlSelfRef.__Vfunc_test_gtext__DOT__gfx_pixel__93__x = 5U;
                    vlSelf->__Vfunc_test_gtext__DOT__gfx_pixel__93__Vfuncout = 0;
                    vlSelfRef.__Vfunc_test_gtext__DOT__gfx_pixel__93__Vfuncout 
                        = ((0xf9ffU >= (0x0000ffffU 
                                        & (VL_MULS_III(32, (IData)(0x00000140U), vlSelfRef.__Vfunc_test_gtext__DOT__gfx_pixel__93__y) 
                                           + vlSelfRef.__Vfunc_test_gtext__DOT__gfx_pixel__93__x)))
                            ? vlSelfRef.test_gtext__DOT__dut__DOT__gfx_inst__DOT__gfx_mem__DOT__mem
                           [(0x0000ffffU & (VL_MULS_III(32, (IData)(0x00000140U), vlSelfRef.__Vfunc_test_gtext__DOT__gfx_pixel__93__y) 
                                            + vlSelfRef.__Vfunc_test_gtext__DOT__gfx_pixel__93__x))]
                            : 0U);
                }(), (IData)(vlSelfRef.__Vfunc_test_gtext__DOT__gfx_pixel__93__Vfuncout)))) {
        vlSelfRef.test_gtext__DOT__unnamedblk12__DOT__pc 
            = ((IData)(1U) + vlSelfRef.test_gtext__DOT__unnamedblk12__DOT__pc);
    }
    if ((0U != ([&]() {
                    vlSelfRef.__Vfunc_test_gtext__DOT__gfx_pixel__93__y = 5U;
                    vlSelfRef.__Vfunc_test_gtext__DOT__gfx_pixel__93__x = 6U;
                    vlSelf->__Vfunc_test_gtext__DOT__gfx_pixel__93__Vfuncout = 0;
                    vlSelfRef.__Vfunc_test_gtext__DOT__gfx_pixel__93__Vfuncout 
                        = ((0xf9ffU >= (0x0000ffffU 
                                        & (VL_MULS_III(32, (IData)(0x00000140U), vlSelfRef.__Vfunc_test_gtext__DOT__gfx_pixel__93__y) 
                                           + vlSelfRef.__Vfunc_test_gtext__DOT__gfx_pixel__93__x)))
                            ? vlSelfRef.test_gtext__DOT__dut__DOT__gfx_inst__DOT__gfx_mem__DOT__mem
                           [(0x0000ffffU & (VL_MULS_III(32, (IData)(0x00000140U), vlSelfRef.__Vfunc_test_gtext__DOT__gfx_pixel__93__y) 
                                            + vlSelfRef.__Vfunc_test_gtext__DOT__gfx_pixel__93__x))]
                            : 0U);
                }(), (IData)(vlSelfRef.__Vfunc_test_gtext__DOT__gfx_pixel__93__Vfuncout)))) {
        vlSelfRef.test_gtext__DOT__unnamedblk12__DOT__pc 
            = ((IData)(1U) + vlSelfRef.test_gtext__DOT__unnamedblk12__DOT__pc);
    }
    if ((0U != ([&]() {
                    vlSelfRef.__Vfunc_test_gtext__DOT__gfx_pixel__93__y = 5U;
                    vlSelfRef.__Vfunc_test_gtext__DOT__gfx_pixel__93__x = 7U;
                    vlSelf->__Vfunc_test_gtext__DOT__gfx_pixel__93__Vfuncout = 0;
                    vlSelfRef.__Vfunc_test_gtext__DOT__gfx_pixel__93__Vfuncout 
                        = ((0xf9ffU >= (0x0000ffffU 
                                        & (VL_MULS_III(32, (IData)(0x00000140U), vlSelfRef.__Vfunc_test_gtext__DOT__gfx_pixel__93__y) 
                                           + vlSelfRef.__Vfunc_test_gtext__DOT__gfx_pixel__93__x)))
                            ? vlSelfRef.test_gtext__DOT__dut__DOT__gfx_inst__DOT__gfx_mem__DOT__mem
                           [(0x0000ffffU & (VL_MULS_III(32, (IData)(0x00000140U), vlSelfRef.__Vfunc_test_gtext__DOT__gfx_pixel__93__y) 
                                            + vlSelfRef.__Vfunc_test_gtext__DOT__gfx_pixel__93__x))]
                            : 0U);
                }(), (IData)(vlSelfRef.__Vfunc_test_gtext__DOT__gfx_pixel__93__Vfuncout)))) {
        vlSelfRef.test_gtext__DOT__unnamedblk12__DOT__pc 
            = ((IData)(1U) + vlSelfRef.test_gtext__DOT__unnamedblk12__DOT__pc);
    }
    if ((0U != ([&]() {
                    vlSelfRef.__Vfunc_test_gtext__DOT__gfx_pixel__93__y = 5U;
                    vlSelfRef.__Vfunc_test_gtext__DOT__gfx_pixel__93__x = 8U;
                    vlSelf->__Vfunc_test_gtext__DOT__gfx_pixel__93__Vfuncout = 0;
                    vlSelfRef.__Vfunc_test_gtext__DOT__gfx_pixel__93__Vfuncout 
                        = ((0xf9ffU >= (0x0000ffffU 
                                        & (VL_MULS_III(32, (IData)(0x00000140U), vlSelfRef.__Vfunc_test_gtext__DOT__gfx_pixel__93__y) 
                                           + vlSelfRef.__Vfunc_test_gtext__DOT__gfx_pixel__93__x)))
                            ? vlSelfRef.test_gtext__DOT__dut__DOT__gfx_inst__DOT__gfx_mem__DOT__mem
                           [(0x0000ffffU & (VL_MULS_III(32, (IData)(0x00000140U), vlSelfRef.__Vfunc_test_gtext__DOT__gfx_pixel__93__y) 
                                            + vlSelfRef.__Vfunc_test_gtext__DOT__gfx_pixel__93__x))]
                            : 0U);
                }(), (IData)(vlSelfRef.__Vfunc_test_gtext__DOT__gfx_pixel__93__Vfuncout)))) {
        vlSelfRef.test_gtext__DOT__unnamedblk12__DOT__pc 
            = ((IData)(1U) + vlSelfRef.test_gtext__DOT__unnamedblk12__DOT__pc);
    }
    if ((0U != ([&]() {
                    vlSelfRef.__Vfunc_test_gtext__DOT__gfx_pixel__93__y = 5U;
                    vlSelfRef.__Vfunc_test_gtext__DOT__gfx_pixel__93__x = 9U;
                    vlSelf->__Vfunc_test_gtext__DOT__gfx_pixel__93__Vfuncout = 0;
                    vlSelfRef.__Vfunc_test_gtext__DOT__gfx_pixel__93__Vfuncout 
                        = ((0xf9ffU >= (0x0000ffffU 
                                        & (VL_MULS_III(32, (IData)(0x00000140U), vlSelfRef.__Vfunc_test_gtext__DOT__gfx_pixel__93__y) 
                                           + vlSelfRef.__Vfunc_test_gtext__DOT__gfx_pixel__93__x)))
                            ? vlSelfRef.test_gtext__DOT__dut__DOT__gfx_inst__DOT__gfx_mem__DOT__mem
                           [(0x0000ffffU & (VL_MULS_III(32, (IData)(0x00000140U), vlSelfRef.__Vfunc_test_gtext__DOT__gfx_pixel__93__y) 
                                            + vlSelfRef.__Vfunc_test_gtext__DOT__gfx_pixel__93__x))]
                            : 0U);
                }(), (IData)(vlSelfRef.__Vfunc_test_gtext__DOT__gfx_pixel__93__Vfuncout)))) {
        vlSelfRef.test_gtext__DOT__unnamedblk12__DOT__pc 
            = ((IData)(1U) + vlSelfRef.test_gtext__DOT__unnamedblk12__DOT__pc);
    }
    if ((0U != ([&]() {
                    vlSelfRef.__Vfunc_test_gtext__DOT__gfx_pixel__93__y = 5U;
                    vlSelfRef.__Vfunc_test_gtext__DOT__gfx_pixel__93__x = 0x0000000aU;
                    vlSelf->__Vfunc_test_gtext__DOT__gfx_pixel__93__Vfuncout = 0;
                    vlSelfRef.__Vfunc_test_gtext__DOT__gfx_pixel__93__Vfuncout 
                        = ((0xf9ffU >= (0x0000ffffU 
                                        & (VL_MULS_III(32, (IData)(0x00000140U), vlSelfRef.__Vfunc_test_gtext__DOT__gfx_pixel__93__y) 
                                           + vlSelfRef.__Vfunc_test_gtext__DOT__gfx_pixel__93__x)))
                            ? vlSelfRef.test_gtext__DOT__dut__DOT__gfx_inst__DOT__gfx_mem__DOT__mem
                           [(0x0000ffffU & (VL_MULS_III(32, (IData)(0x00000140U), vlSelfRef.__Vfunc_test_gtext__DOT__gfx_pixel__93__y) 
                                            + vlSelfRef.__Vfunc_test_gtext__DOT__gfx_pixel__93__x))]
                            : 0U);
                }(), (IData)(vlSelfRef.__Vfunc_test_gtext__DOT__gfx_pixel__93__Vfuncout)))) {
        vlSelfRef.test_gtext__DOT__unnamedblk12__DOT__pc 
            = ((IData)(1U) + vlSelfRef.test_gtext__DOT__unnamedblk12__DOT__pc);
    }
    if ((0U != ([&]() {
                    vlSelfRef.__Vfunc_test_gtext__DOT__gfx_pixel__93__y = 5U;
                    vlSelfRef.__Vfunc_test_gtext__DOT__gfx_pixel__93__x = 0x0000000bU;
                    vlSelf->__Vfunc_test_gtext__DOT__gfx_pixel__93__Vfuncout = 0;
                    vlSelfRef.__Vfunc_test_gtext__DOT__gfx_pixel__93__Vfuncout 
                        = ((0xf9ffU >= (0x0000ffffU 
                                        & (VL_MULS_III(32, (IData)(0x00000140U), vlSelfRef.__Vfunc_test_gtext__DOT__gfx_pixel__93__y) 
                                           + vlSelfRef.__Vfunc_test_gtext__DOT__gfx_pixel__93__x)))
                            ? vlSelfRef.test_gtext__DOT__dut__DOT__gfx_inst__DOT__gfx_mem__DOT__mem
                           [(0x0000ffffU & (VL_MULS_III(32, (IData)(0x00000140U), vlSelfRef.__Vfunc_test_gtext__DOT__gfx_pixel__93__y) 
                                            + vlSelfRef.__Vfunc_test_gtext__DOT__gfx_pixel__93__x))]
                            : 0U);
                }(), (IData)(vlSelfRef.__Vfunc_test_gtext__DOT__gfx_pixel__93__Vfuncout)))) {
        vlSelfRef.test_gtext__DOT__unnamedblk12__DOT__pc 
            = ((IData)(1U) + vlSelfRef.test_gtext__DOT__unnamedblk12__DOT__pc);
    }
    if ((0U != ([&]() {
                    vlSelfRef.__Vfunc_test_gtext__DOT__gfx_pixel__93__y = 5U;
                    vlSelfRef.__Vfunc_test_gtext__DOT__gfx_pixel__93__x = 0x0000000cU;
                    vlSelf->__Vfunc_test_gtext__DOT__gfx_pixel__93__Vfuncout = 0;
                    vlSelfRef.__Vfunc_test_gtext__DOT__gfx_pixel__93__Vfuncout 
                        = ((0xf9ffU >= (0x0000ffffU 
                                        & (VL_MULS_III(32, (IData)(0x00000140U), vlSelfRef.__Vfunc_test_gtext__DOT__gfx_pixel__93__y) 
                                           + vlSelfRef.__Vfunc_test_gtext__DOT__gfx_pixel__93__x)))
                            ? vlSelfRef.test_gtext__DOT__dut__DOT__gfx_inst__DOT__gfx_mem__DOT__mem
                           [(0x0000ffffU & (VL_MULS_III(32, (IData)(0x00000140U), vlSelfRef.__Vfunc_test_gtext__DOT__gfx_pixel__93__y) 
                                            + vlSelfRef.__Vfunc_test_gtext__DOT__gfx_pixel__93__x))]
                            : 0U);
                }(), (IData)(vlSelfRef.__Vfunc_test_gtext__DOT__gfx_pixel__93__Vfuncout)))) {
        vlSelfRef.test_gtext__DOT__unnamedblk12__DOT__pc 
            = ((IData)(1U) + vlSelfRef.test_gtext__DOT__unnamedblk12__DOT__pc);
    }
    if ((0U != ([&]() {
                    vlSelfRef.__Vfunc_test_gtext__DOT__gfx_pixel__93__y = 5U;
                    vlSelfRef.__Vfunc_test_gtext__DOT__gfx_pixel__93__x = 0x0000000dU;
                    vlSelf->__Vfunc_test_gtext__DOT__gfx_pixel__93__Vfuncout = 0;
                    vlSelfRef.__Vfunc_test_gtext__DOT__gfx_pixel__93__Vfuncout 
                        = ((0xf9ffU >= (0x0000ffffU 
                                        & (VL_MULS_III(32, (IData)(0x00000140U), vlSelfRef.__Vfunc_test_gtext__DOT__gfx_pixel__93__y) 
                                           + vlSelfRef.__Vfunc_test_gtext__DOT__gfx_pixel__93__x)))
                            ? vlSelfRef.test_gtext__DOT__dut__DOT__gfx_inst__DOT__gfx_mem__DOT__mem
                           [(0x0000ffffU & (VL_MULS_III(32, (IData)(0x00000140U), vlSelfRef.__Vfunc_test_gtext__DOT__gfx_pixel__93__y) 
                                            + vlSelfRef.__Vfunc_test_gtext__DOT__gfx_pixel__93__x))]
                            : 0U);
                }(), (IData)(vlSelfRef.__Vfunc_test_gtext__DOT__gfx_pixel__93__Vfuncout)))) {
        vlSelfRef.test_gtext__DOT__unnamedblk12__DOT__pc 
            = ((IData)(1U) + vlSelfRef.test_gtext__DOT__unnamedblk12__DOT__pc);
    }
    if ((0U != ([&]() {
                    vlSelfRef.__Vfunc_test_gtext__DOT__gfx_pixel__93__y = 5U;
                    vlSelfRef.__Vfunc_test_gtext__DOT__gfx_pixel__93__x = 0x0000000eU;
                    vlSelf->__Vfunc_test_gtext__DOT__gfx_pixel__93__Vfuncout = 0;
                    vlSelfRef.__Vfunc_test_gtext__DOT__gfx_pixel__93__Vfuncout 
                        = ((0xf9ffU >= (0x0000ffffU 
                                        & (VL_MULS_III(32, (IData)(0x00000140U), vlSelfRef.__Vfunc_test_gtext__DOT__gfx_pixel__93__y) 
                                           + vlSelfRef.__Vfunc_test_gtext__DOT__gfx_pixel__93__x)))
                            ? vlSelfRef.test_gtext__DOT__dut__DOT__gfx_inst__DOT__gfx_mem__DOT__mem
                           [(0x0000ffffU & (VL_MULS_III(32, (IData)(0x00000140U), vlSelfRef.__Vfunc_test_gtext__DOT__gfx_pixel__93__y) 
                                            + vlSelfRef.__Vfunc_test_gtext__DOT__gfx_pixel__93__x))]
                            : 0U);
                }(), (IData)(vlSelfRef.__Vfunc_test_gtext__DOT__gfx_pixel__93__Vfuncout)))) {
        vlSelfRef.test_gtext__DOT__unnamedblk12__DOT__pc 
            = ((IData)(1U) + vlSelfRef.test_gtext__DOT__unnamedblk12__DOT__pc);
    }
    if ((0U != ([&]() {
                    vlSelfRef.__Vfunc_test_gtext__DOT__gfx_pixel__93__y = 5U;
                    vlSelfRef.__Vfunc_test_gtext__DOT__gfx_pixel__93__x = 0x0000000fU;
                    vlSelf->__Vfunc_test_gtext__DOT__gfx_pixel__93__Vfuncout = 0;
                    vlSelfRef.__Vfunc_test_gtext__DOT__gfx_pixel__93__Vfuncout 
                        = ((0xf9ffU >= (0x0000ffffU 
                                        & (VL_MULS_III(32, (IData)(0x00000140U), vlSelfRef.__Vfunc_test_gtext__DOT__gfx_pixel__93__y) 
                                           + vlSelfRef.__Vfunc_test_gtext__DOT__gfx_pixel__93__x)))
                            ? vlSelfRef.test_gtext__DOT__dut__DOT__gfx_inst__DOT__gfx_mem__DOT__mem
                           [(0x0000ffffU & (VL_MULS_III(32, (IData)(0x00000140U), vlSelfRef.__Vfunc_test_gtext__DOT__gfx_pixel__93__y) 
                                            + vlSelfRef.__Vfunc_test_gtext__DOT__gfx_pixel__93__x))]
                            : 0U);
                }(), (IData)(vlSelfRef.__Vfunc_test_gtext__DOT__gfx_pixel__93__Vfuncout)))) {
        vlSelfRef.test_gtext__DOT__unnamedblk12__DOT__pc 
            = ((IData)(1U) + vlSelfRef.test_gtext__DOT__unnamedblk12__DOT__pc);
    }
    if ((0U != ([&]() {
                    vlSelfRef.__Vfunc_test_gtext__DOT__gfx_pixel__93__y = 6U;
                    vlSelfRef.__Vfunc_test_gtext__DOT__gfx_pixel__93__x = 0U;
                    vlSelf->__Vfunc_test_gtext__DOT__gfx_pixel__93__Vfuncout = 0;
                    vlSelfRef.__Vfunc_test_gtext__DOT__gfx_pixel__93__Vfuncout 
                        = ((0xf9ffU >= (0x0000ffffU 
                                        & (VL_MULS_III(32, (IData)(0x00000140U), vlSelfRef.__Vfunc_test_gtext__DOT__gfx_pixel__93__y) 
                                           + vlSelfRef.__Vfunc_test_gtext__DOT__gfx_pixel__93__x)))
                            ? vlSelfRef.test_gtext__DOT__dut__DOT__gfx_inst__DOT__gfx_mem__DOT__mem
                           [(0x0000ffffU & (VL_MULS_III(32, (IData)(0x00000140U), vlSelfRef.__Vfunc_test_gtext__DOT__gfx_pixel__93__y) 
                                            + vlSelfRef.__Vfunc_test_gtext__DOT__gfx_pixel__93__x))]
                            : 0U);
                }(), (IData)(vlSelfRef.__Vfunc_test_gtext__DOT__gfx_pixel__93__Vfuncout)))) {
        vlSelfRef.test_gtext__DOT__unnamedblk12__DOT__pc 
            = ((IData)(1U) + vlSelfRef.test_gtext__DOT__unnamedblk12__DOT__pc);
    }
    if ((0U != ([&]() {
                    vlSelfRef.__Vfunc_test_gtext__DOT__gfx_pixel__93__y = 6U;
                    vlSelfRef.__Vfunc_test_gtext__DOT__gfx_pixel__93__x = 1U;
                    vlSelf->__Vfunc_test_gtext__DOT__gfx_pixel__93__Vfuncout = 0;
                    vlSelfRef.__Vfunc_test_gtext__DOT__gfx_pixel__93__Vfuncout 
                        = ((0xf9ffU >= (0x0000ffffU 
                                        & (VL_MULS_III(32, (IData)(0x00000140U), vlSelfRef.__Vfunc_test_gtext__DOT__gfx_pixel__93__y) 
                                           + vlSelfRef.__Vfunc_test_gtext__DOT__gfx_pixel__93__x)))
                            ? vlSelfRef.test_gtext__DOT__dut__DOT__gfx_inst__DOT__gfx_mem__DOT__mem
                           [(0x0000ffffU & (VL_MULS_III(32, (IData)(0x00000140U), vlSelfRef.__Vfunc_test_gtext__DOT__gfx_pixel__93__y) 
                                            + vlSelfRef.__Vfunc_test_gtext__DOT__gfx_pixel__93__x))]
                            : 0U);
                }(), (IData)(vlSelfRef.__Vfunc_test_gtext__DOT__gfx_pixel__93__Vfuncout)))) {
        vlSelfRef.test_gtext__DOT__unnamedblk12__DOT__pc 
            = ((IData)(1U) + vlSelfRef.test_gtext__DOT__unnamedblk12__DOT__pc);
    }
    if ((0U != ([&]() {
                    vlSelfRef.__Vfunc_test_gtext__DOT__gfx_pixel__93__y = 6U;
                    vlSelfRef.__Vfunc_test_gtext__DOT__gfx_pixel__93__x = 2U;
                    vlSelf->__Vfunc_test_gtext__DOT__gfx_pixel__93__Vfuncout = 0;
                    vlSelfRef.__Vfunc_test_gtext__DOT__gfx_pixel__93__Vfuncout 
                        = ((0xf9ffU >= (0x0000ffffU 
                                        & (VL_MULS_III(32, (IData)(0x00000140U), vlSelfRef.__Vfunc_test_gtext__DOT__gfx_pixel__93__y) 
                                           + vlSelfRef.__Vfunc_test_gtext__DOT__gfx_pixel__93__x)))
                            ? vlSelfRef.test_gtext__DOT__dut__DOT__gfx_inst__DOT__gfx_mem__DOT__mem
                           [(0x0000ffffU & (VL_MULS_III(32, (IData)(0x00000140U), vlSelfRef.__Vfunc_test_gtext__DOT__gfx_pixel__93__y) 
                                            + vlSelfRef.__Vfunc_test_gtext__DOT__gfx_pixel__93__x))]
                            : 0U);
                }(), (IData)(vlSelfRef.__Vfunc_test_gtext__DOT__gfx_pixel__93__Vfuncout)))) {
        vlSelfRef.test_gtext__DOT__unnamedblk12__DOT__pc 
            = ((IData)(1U) + vlSelfRef.test_gtext__DOT__unnamedblk12__DOT__pc);
    }
    if ((0U != ([&]() {
                    vlSelfRef.__Vfunc_test_gtext__DOT__gfx_pixel__93__y = 6U;
                    vlSelfRef.__Vfunc_test_gtext__DOT__gfx_pixel__93__x = 3U;
                    vlSelf->__Vfunc_test_gtext__DOT__gfx_pixel__93__Vfuncout = 0;
                    vlSelfRef.__Vfunc_test_gtext__DOT__gfx_pixel__93__Vfuncout 
                        = ((0xf9ffU >= (0x0000ffffU 
                                        & (VL_MULS_III(32, (IData)(0x00000140U), vlSelfRef.__Vfunc_test_gtext__DOT__gfx_pixel__93__y) 
                                           + vlSelfRef.__Vfunc_test_gtext__DOT__gfx_pixel__93__x)))
                            ? vlSelfRef.test_gtext__DOT__dut__DOT__gfx_inst__DOT__gfx_mem__DOT__mem
                           [(0x0000ffffU & (VL_MULS_III(32, (IData)(0x00000140U), vlSelfRef.__Vfunc_test_gtext__DOT__gfx_pixel__93__y) 
                                            + vlSelfRef.__Vfunc_test_gtext__DOT__gfx_pixel__93__x))]
                            : 0U);
                }(), (IData)(vlSelfRef.__Vfunc_test_gtext__DOT__gfx_pixel__93__Vfuncout)))) {
        vlSelfRef.test_gtext__DOT__unnamedblk12__DOT__pc 
            = ((IData)(1U) + vlSelfRef.test_gtext__DOT__unnamedblk12__DOT__pc);
    }
    if ((0U != ([&]() {
                    vlSelfRef.__Vfunc_test_gtext__DOT__gfx_pixel__93__y = 6U;
                    vlSelfRef.__Vfunc_test_gtext__DOT__gfx_pixel__93__x = 4U;
                    vlSelf->__Vfunc_test_gtext__DOT__gfx_pixel__93__Vfuncout = 0;
                    vlSelfRef.__Vfunc_test_gtext__DOT__gfx_pixel__93__Vfuncout 
                        = ((0xf9ffU >= (0x0000ffffU 
                                        & (VL_MULS_III(32, (IData)(0x00000140U), vlSelfRef.__Vfunc_test_gtext__DOT__gfx_pixel__93__y) 
                                           + vlSelfRef.__Vfunc_test_gtext__DOT__gfx_pixel__93__x)))
                            ? vlSelfRef.test_gtext__DOT__dut__DOT__gfx_inst__DOT__gfx_mem__DOT__mem
                           [(0x0000ffffU & (VL_MULS_III(32, (IData)(0x00000140U), vlSelfRef.__Vfunc_test_gtext__DOT__gfx_pixel__93__y) 
                                            + vlSelfRef.__Vfunc_test_gtext__DOT__gfx_pixel__93__x))]
                            : 0U);
                }(), (IData)(vlSelfRef.__Vfunc_test_gtext__DOT__gfx_pixel__93__Vfuncout)))) {
        vlSelfRef.test_gtext__DOT__unnamedblk12__DOT__pc 
            = ((IData)(1U) + vlSelfRef.test_gtext__DOT__unnamedblk12__DOT__pc);
    }
    if ((0U != ([&]() {
                    vlSelfRef.__Vfunc_test_gtext__DOT__gfx_pixel__93__y = 6U;
                    vlSelfRef.__Vfunc_test_gtext__DOT__gfx_pixel__93__x = 5U;
                    vlSelf->__Vfunc_test_gtext__DOT__gfx_pixel__93__Vfuncout = 0;
                    vlSelfRef.__Vfunc_test_gtext__DOT__gfx_pixel__93__Vfuncout 
                        = ((0xf9ffU >= (0x0000ffffU 
                                        & (VL_MULS_III(32, (IData)(0x00000140U), vlSelfRef.__Vfunc_test_gtext__DOT__gfx_pixel__93__y) 
                                           + vlSelfRef.__Vfunc_test_gtext__DOT__gfx_pixel__93__x)))
                            ? vlSelfRef.test_gtext__DOT__dut__DOT__gfx_inst__DOT__gfx_mem__DOT__mem
                           [(0x0000ffffU & (VL_MULS_III(32, (IData)(0x00000140U), vlSelfRef.__Vfunc_test_gtext__DOT__gfx_pixel__93__y) 
                                            + vlSelfRef.__Vfunc_test_gtext__DOT__gfx_pixel__93__x))]
                            : 0U);
                }(), (IData)(vlSelfRef.__Vfunc_test_gtext__DOT__gfx_pixel__93__Vfuncout)))) {
        vlSelfRef.test_gtext__DOT__unnamedblk12__DOT__pc 
            = ((IData)(1U) + vlSelfRef.test_gtext__DOT__unnamedblk12__DOT__pc);
    }
    co_return;
}
