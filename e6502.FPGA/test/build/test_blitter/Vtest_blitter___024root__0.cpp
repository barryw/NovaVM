// Verilated -*- C++ -*-
// DESCRIPTION: Verilator output: Design implementation internals
// See Vtest_blitter.h for the primary calling header

#include "Vtest_blitter__pch.h"

VlCoroutine Vtest_blitter___024root___eval_initial__TOP__Vtiming__0(Vtest_blitter___024root* vlSelf);
VlCoroutine Vtest_blitter___024root___eval_initial__TOP__Vtiming__1(Vtest_blitter___024root* vlSelf);

void Vtest_blitter___024root___eval_initial(Vtest_blitter___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vtest_blitter___024root___eval_initial\n"); );
    Vtest_blitter__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
    // Body
    vlSelfRef.test_blitter__DOT__dut__DOT__regs[0U] = 0U;
    vlSelfRef.test_blitter__DOT__dut__DOT__regs[1U] = 0U;
    vlSelfRef.test_blitter__DOT__dut__DOT__regs[2U] = 0U;
    vlSelfRef.test_blitter__DOT__dut__DOT__regs[3U] = 0U;
    vlSelfRef.test_blitter__DOT__dut__DOT__regs[4U] = 0U;
    vlSelfRef.test_blitter__DOT__dut__DOT__regs[5U] = 0U;
    vlSelfRef.test_blitter__DOT__dut__DOT__regs[6U] = 0U;
    vlSelfRef.test_blitter__DOT__dut__DOT__regs[7U] = 0U;
    vlSelfRef.test_blitter__DOT__dut__DOT__regs[8U] = 0U;
    vlSelfRef.test_blitter__DOT__dut__DOT__regs[9U] = 0U;
    vlSelfRef.test_blitter__DOT__dut__DOT__regs[10U] = 0U;
    vlSelfRef.test_blitter__DOT__dut__DOT__regs[11U] = 0U;
    vlSelfRef.test_blitter__DOT__dut__DOT__regs[12U] = 0U;
    vlSelfRef.test_blitter__DOT__dut__DOT__regs[13U] = 0U;
    vlSelfRef.test_blitter__DOT__dut__DOT__regs[14U] = 0U;
    vlSelfRef.test_blitter__DOT__dut__DOT__regs[15U] = 0U;
    vlSelfRef.test_blitter__DOT__dut__DOT__regs[16U] = 0U;
    vlSelfRef.test_blitter__DOT__dut__DOT__regs[17U] = 0U;
    vlSelfRef.test_blitter__DOT__dut__DOT__regs[18U] = 0U;
    vlSelfRef.test_blitter__DOT__dut__DOT__regs[19U] = 0U;
    vlSelfRef.test_blitter__DOT__dut__DOT__regs[20U] = 0U;
    vlSelfRef.test_blitter__DOT__dut__DOT__regs[21U] = 0U;
    vlSelfRef.test_blitter__DOT__dut__DOT__regs[22U] = 0U;
    vlSelfRef.test_blitter__DOT__dut__DOT__regs[23U] = 0U;
    vlSelfRef.test_blitter__DOT__dut__DOT__regs[24U] = 0U;
    vlSelfRef.test_blitter__DOT__dut__DOT__regs[1U] = 0U;
    vlSelfRef.test_blitter__DOT__dut__DOT__state = 0U;
    vlSelfRef.test_blitter__DOT__dut__DOT__fill_mode = 0U;
    vlSelfRef.test_blitter__DOT__dut__DOT__colorkey_mode = 0U;
    vlSelfRef.test_blitter__DOT__dut__DOT__use_row_buffer = 0U;
    vlSelfRef.test_blitter__DOT__dut__DOT__src_space = 0U;
    vlSelfRef.test_blitter__DOT__dut__DOT__dst_space = 0U;
    vlSelfRef.test_blitter__DOT__dut__DOT__src_base = 0U;
    vlSelfRef.test_blitter__DOT__dut__DOT__dst_base = 0U;
    vlSelfRef.test_blitter__DOT__dut__DOT__width = 0U;
    vlSelfRef.test_blitter__DOT__dut__DOT__height = 0U;
    vlSelfRef.test_blitter__DOT__dut__DOT__src_stride = 0U;
    vlSelfRef.test_blitter__DOT__dut__DOT__dst_stride = 0U;
    vlSelfRef.test_blitter__DOT__dut__DOT__fill_value = 0U;
    vlSelfRef.test_blitter__DOT__dut__DOT__color_key = 0U;
    vlSelfRef.test_blitter__DOT__dut__DOT__row = 0U;
    vlSelfRef.test_blitter__DOT__dut__DOT__col = 0U;
    vlSelfRef.test_blitter__DOT__dut__DOT__wrote_count = 0U;
    vlSelfRef.test_blitter__DOT__dut__DOT__read_byte = 0U;
    vlSelfRef.test_blitter__DOT__dut__DOT__read_valid = 0U;
    vlSelfRef.test_blitter__DOT__dut__DOT__buf_idx = 0U;
    Vtest_blitter___024root___eval_initial__TOP__Vtiming__0(vlSelf);
    Vtest_blitter___024root___eval_initial__TOP__Vtiming__1(vlSelf);
}

VlCoroutine Vtest_blitter___024root___eval_initial__TOP__Vtiming__0__0(Vtest_blitter___024root* vlSelf);
VlCoroutine Vtest_blitter___024root___eval_initial__TOP__Vtiming__0__1(Vtest_blitter___024root* vlSelf);

VlCoroutine Vtest_blitter___024root___eval_initial__TOP__Vtiming__0(Vtest_blitter___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vtest_blitter___024root___eval_initial__TOP__Vtiming__0\n"); );
    Vtest_blitter__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
    // Body
    co_await Vtest_blitter___024root___eval_initial__TOP__Vtiming__0__0(vlSelf);
    co_await Vtest_blitter___024root___eval_initial__TOP__Vtiming__0__1(vlSelf);
    co_return;
}

void Vtest_blitter___024root____VbeforeTrig_h603fc354__0(Vtest_blitter___024root* vlSelf, const char* __VeventDescription);

VlCoroutine Vtest_blitter___024root___eval_initial__TOP__Vtiming__0__0(Vtest_blitter___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vtest_blitter___024root___eval_initial__TOP__Vtiming__0__0\n"); );
    Vtest_blitter__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
    // Locals
    IData/*31:0*/ test_blitter__DOT__unnamedblk1__DOT__i;
    test_blitter__DOT__unnamedblk1__DOT__i = 0;
    IData/*31:0*/ test_blitter__DOT__unnamedblk2__DOT__i;
    test_blitter__DOT__unnamedblk2__DOT__i = 0;
    IData/*31:0*/ test_blitter__DOT__unnamedblk3__DOT__i;
    test_blitter__DOT__unnamedblk3__DOT__i = 0;
    IData/*31:0*/ test_blitter__DOT__unnamedblk4__DOT__i;
    test_blitter__DOT__unnamedblk4__DOT__i = 0;
    IData/*31:0*/ test_blitter__DOT__unnamedblk1_3__DOT____Vrepeat2;
    test_blitter__DOT__unnamedblk1_3__DOT____Vrepeat2 = 0;
    IData/*31:0*/ test_blitter__DOT__unnamedblk1_4__DOT____Vrepeat3;
    test_blitter__DOT__unnamedblk1_4__DOT____Vrepeat3 = 0;
    IData/*31:0*/ test_blitter__DOT__unnamedblk1_5__DOT____Vrepeat4;
    test_blitter__DOT__unnamedblk1_5__DOT____Vrepeat4 = 0;
    IData/*31:0*/ test_blitter__DOT__unnamedblk1_6__DOT____Vrepeat5;
    test_blitter__DOT__unnamedblk1_6__DOT____Vrepeat5 = 0;
    IData/*31:0*/ test_blitter__DOT__unnamedblk1_7__DOT____Vrepeat6;
    test_blitter__DOT__unnamedblk1_7__DOT____Vrepeat6 = 0;
    IData/*31:0*/ test_blitter__DOT__unnamedblk1_8__DOT____Vrepeat7;
    test_blitter__DOT__unnamedblk1_8__DOT____Vrepeat7 = 0;
    CData/*0:0*/ __Vtask_test_blitter__DOT__check__0__condition;
    __Vtask_test_blitter__DOT__check__0__condition = 0;
    CData/*0:0*/ __Vtask_test_blitter__DOT__check__1__condition;
    __Vtask_test_blitter__DOT__check__1__condition = 0;
    IData/*31:0*/ __Vtask_test_blitter__DOT__setup_fill__2__dst_space;
    __Vtask_test_blitter__DOT__setup_fill__2__dst_space = 0;
    IData/*31:0*/ __Vtask_test_blitter__DOT__setup_fill__2__dst_addr;
    __Vtask_test_blitter__DOT__setup_fill__2__dst_addr = 0;
    IData/*31:0*/ __Vtask_test_blitter__DOT__setup_fill__2__dst_stride;
    __Vtask_test_blitter__DOT__setup_fill__2__dst_stride = 0;
    IData/*31:0*/ __Vtask_test_blitter__DOT__setup_fill__2__width;
    __Vtask_test_blitter__DOT__setup_fill__2__width = 0;
    IData/*31:0*/ __Vtask_test_blitter__DOT__setup_fill__2__height;
    __Vtask_test_blitter__DOT__setup_fill__2__height = 0;
    IData/*31:0*/ __Vtask_test_blitter__DOT__setup_fill__2__fill_val;
    __Vtask_test_blitter__DOT__setup_fill__2__fill_val = 0;
    IData/*31:0*/ __Vtask_test_blitter__DOT__blt_reg__3__off;
    __Vtask_test_blitter__DOT__blt_reg__3__off = 0;
    CData/*7:0*/ __Vtask_test_blitter__DOT__blt_reg__3__data;
    __Vtask_test_blitter__DOT__blt_reg__3__data = 0;
    SData/*15:0*/ __Vtask_test_blitter__DOT__write_reg__4__addr;
    __Vtask_test_blitter__DOT__write_reg__4__addr = 0;
    CData/*7:0*/ __Vtask_test_blitter__DOT__write_reg__4__data;
    __Vtask_test_blitter__DOT__write_reg__4__data = 0;
    IData/*31:0*/ __Vtask_test_blitter__DOT__blt_reg__5__off;
    __Vtask_test_blitter__DOT__blt_reg__5__off = 0;
    CData/*7:0*/ __Vtask_test_blitter__DOT__blt_reg__5__data;
    __Vtask_test_blitter__DOT__blt_reg__5__data = 0;
    SData/*15:0*/ __Vtask_test_blitter__DOT__write_reg__6__addr;
    __Vtask_test_blitter__DOT__write_reg__6__addr = 0;
    CData/*7:0*/ __Vtask_test_blitter__DOT__write_reg__6__data;
    __Vtask_test_blitter__DOT__write_reg__6__data = 0;
    IData/*31:0*/ __Vtask_test_blitter__DOT__blt_reg__7__off;
    __Vtask_test_blitter__DOT__blt_reg__7__off = 0;
    CData/*7:0*/ __Vtask_test_blitter__DOT__blt_reg__7__data;
    __Vtask_test_blitter__DOT__blt_reg__7__data = 0;
    SData/*15:0*/ __Vtask_test_blitter__DOT__write_reg__8__addr;
    __Vtask_test_blitter__DOT__write_reg__8__addr = 0;
    CData/*7:0*/ __Vtask_test_blitter__DOT__write_reg__8__data;
    __Vtask_test_blitter__DOT__write_reg__8__data = 0;
    IData/*31:0*/ __Vtask_test_blitter__DOT__blt_reg__9__off;
    __Vtask_test_blitter__DOT__blt_reg__9__off = 0;
    CData/*7:0*/ __Vtask_test_blitter__DOT__blt_reg__9__data;
    __Vtask_test_blitter__DOT__blt_reg__9__data = 0;
    SData/*15:0*/ __Vtask_test_blitter__DOT__write_reg__10__addr;
    __Vtask_test_blitter__DOT__write_reg__10__addr = 0;
    CData/*7:0*/ __Vtask_test_blitter__DOT__write_reg__10__data;
    __Vtask_test_blitter__DOT__write_reg__10__data = 0;
    IData/*31:0*/ __Vtask_test_blitter__DOT__blt_reg__11__off;
    __Vtask_test_blitter__DOT__blt_reg__11__off = 0;
    CData/*7:0*/ __Vtask_test_blitter__DOT__blt_reg__11__data;
    __Vtask_test_blitter__DOT__blt_reg__11__data = 0;
    SData/*15:0*/ __Vtask_test_blitter__DOT__write_reg__12__addr;
    __Vtask_test_blitter__DOT__write_reg__12__addr = 0;
    CData/*7:0*/ __Vtask_test_blitter__DOT__write_reg__12__data;
    __Vtask_test_blitter__DOT__write_reg__12__data = 0;
    IData/*31:0*/ __Vtask_test_blitter__DOT__blt_reg__13__off;
    __Vtask_test_blitter__DOT__blt_reg__13__off = 0;
    CData/*7:0*/ __Vtask_test_blitter__DOT__blt_reg__13__data;
    __Vtask_test_blitter__DOT__blt_reg__13__data = 0;
    SData/*15:0*/ __Vtask_test_blitter__DOT__write_reg__14__addr;
    __Vtask_test_blitter__DOT__write_reg__14__addr = 0;
    CData/*7:0*/ __Vtask_test_blitter__DOT__write_reg__14__data;
    __Vtask_test_blitter__DOT__write_reg__14__data = 0;
    IData/*31:0*/ __Vtask_test_blitter__DOT__blt_reg__15__off;
    __Vtask_test_blitter__DOT__blt_reg__15__off = 0;
    CData/*7:0*/ __Vtask_test_blitter__DOT__blt_reg__15__data;
    __Vtask_test_blitter__DOT__blt_reg__15__data = 0;
    SData/*15:0*/ __Vtask_test_blitter__DOT__write_reg__16__addr;
    __Vtask_test_blitter__DOT__write_reg__16__addr = 0;
    CData/*7:0*/ __Vtask_test_blitter__DOT__write_reg__16__data;
    __Vtask_test_blitter__DOT__write_reg__16__data = 0;
    IData/*31:0*/ __Vtask_test_blitter__DOT__blt_reg__17__off;
    __Vtask_test_blitter__DOT__blt_reg__17__off = 0;
    CData/*7:0*/ __Vtask_test_blitter__DOT__blt_reg__17__data;
    __Vtask_test_blitter__DOT__blt_reg__17__data = 0;
    SData/*15:0*/ __Vtask_test_blitter__DOT__write_reg__18__addr;
    __Vtask_test_blitter__DOT__write_reg__18__addr = 0;
    CData/*7:0*/ __Vtask_test_blitter__DOT__write_reg__18__data;
    __Vtask_test_blitter__DOT__write_reg__18__data = 0;
    IData/*31:0*/ __Vtask_test_blitter__DOT__blt_reg__19__off;
    __Vtask_test_blitter__DOT__blt_reg__19__off = 0;
    CData/*7:0*/ __Vtask_test_blitter__DOT__blt_reg__19__data;
    __Vtask_test_blitter__DOT__blt_reg__19__data = 0;
    SData/*15:0*/ __Vtask_test_blitter__DOT__write_reg__20__addr;
    __Vtask_test_blitter__DOT__write_reg__20__addr = 0;
    CData/*7:0*/ __Vtask_test_blitter__DOT__write_reg__20__data;
    __Vtask_test_blitter__DOT__write_reg__20__data = 0;
    IData/*31:0*/ __Vtask_test_blitter__DOT__blt_reg__21__off;
    __Vtask_test_blitter__DOT__blt_reg__21__off = 0;
    CData/*7:0*/ __Vtask_test_blitter__DOT__blt_reg__21__data;
    __Vtask_test_blitter__DOT__blt_reg__21__data = 0;
    SData/*15:0*/ __Vtask_test_blitter__DOT__write_reg__22__addr;
    __Vtask_test_blitter__DOT__write_reg__22__addr = 0;
    CData/*7:0*/ __Vtask_test_blitter__DOT__write_reg__22__data;
    __Vtask_test_blitter__DOT__write_reg__22__data = 0;
    IData/*31:0*/ __Vtask_test_blitter__DOT__blt_reg__23__off;
    __Vtask_test_blitter__DOT__blt_reg__23__off = 0;
    CData/*7:0*/ __Vtask_test_blitter__DOT__blt_reg__23__data;
    __Vtask_test_blitter__DOT__blt_reg__23__data = 0;
    SData/*15:0*/ __Vtask_test_blitter__DOT__write_reg__24__addr;
    __Vtask_test_blitter__DOT__write_reg__24__addr = 0;
    CData/*7:0*/ __Vtask_test_blitter__DOT__write_reg__24__data;
    __Vtask_test_blitter__DOT__write_reg__24__data = 0;
    IData/*31:0*/ __Vtask_test_blitter__DOT__blt_reg__25__off;
    __Vtask_test_blitter__DOT__blt_reg__25__off = 0;
    CData/*7:0*/ __Vtask_test_blitter__DOT__blt_reg__25__data;
    __Vtask_test_blitter__DOT__blt_reg__25__data = 0;
    SData/*15:0*/ __Vtask_test_blitter__DOT__write_reg__26__addr;
    __Vtask_test_blitter__DOT__write_reg__26__addr = 0;
    CData/*7:0*/ __Vtask_test_blitter__DOT__write_reg__26__data;
    __Vtask_test_blitter__DOT__write_reg__26__data = 0;
    IData/*31:0*/ __Vtask_test_blitter__DOT__blt_reg__28__off;
    __Vtask_test_blitter__DOT__blt_reg__28__off = 0;
    CData/*7:0*/ __Vtask_test_blitter__DOT__blt_reg__28__data;
    __Vtask_test_blitter__DOT__blt_reg__28__data = 0;
    SData/*15:0*/ __Vtask_test_blitter__DOT__write_reg__29__addr;
    __Vtask_test_blitter__DOT__write_reg__29__addr = 0;
    CData/*7:0*/ __Vtask_test_blitter__DOT__write_reg__29__data;
    __Vtask_test_blitter__DOT__write_reg__29__data = 0;
    IData/*31:0*/ __Vtask_test_blitter__DOT__wait_blt_done__30__test_blitter__DOT__unnamedblk1_1__DOT____Vrepeat0;
    __Vtask_test_blitter__DOT__wait_blt_done__30__test_blitter__DOT__unnamedblk1_1__DOT____Vrepeat0 = 0;
    IData/*31:0*/ __Vtask_test_blitter__DOT__wait_blt_done__30__test_blitter__DOT__unnamedblk1_2__DOT____Vrepeat1;
    __Vtask_test_blitter__DOT__wait_blt_done__30__test_blitter__DOT__unnamedblk1_2__DOT____Vrepeat1 = 0;
    CData/*0:0*/ __Vtask_test_blitter__DOT__check__31__condition;
    __Vtask_test_blitter__DOT__check__31__condition = 0;
    CData/*0:0*/ __Vtask_test_blitter__DOT__check__32__condition;
    __Vtask_test_blitter__DOT__check__32__condition = 0;
    CData/*0:0*/ __Vtask_test_blitter__DOT__check__33__condition;
    __Vtask_test_blitter__DOT__check__33__condition = 0;
    CData/*0:0*/ __Vtask_test_blitter__DOT__check__34__condition;
    __Vtask_test_blitter__DOT__check__34__condition = 0;
    CData/*0:0*/ __Vtask_test_blitter__DOT__check__35__condition;
    __Vtask_test_blitter__DOT__check__35__condition = 0;
    CData/*0:0*/ __Vtask_test_blitter__DOT__check__36__condition;
    __Vtask_test_blitter__DOT__check__36__condition = 0;
    CData/*0:0*/ __Vtask_test_blitter__DOT__check__37__condition;
    __Vtask_test_blitter__DOT__check__37__condition = 0;
    CData/*0:0*/ __Vtask_test_blitter__DOT__check__38__condition;
    __Vtask_test_blitter__DOT__check__38__condition = 0;
    CData/*0:0*/ __Vtask_test_blitter__DOT__check__39__condition;
    __Vtask_test_blitter__DOT__check__39__condition = 0;
    CData/*0:0*/ __Vtask_test_blitter__DOT__check__40__condition;
    __Vtask_test_blitter__DOT__check__40__condition = 0;
    IData/*31:0*/ __Vtask_test_blitter__DOT__setup_fill__41__dst_space;
    __Vtask_test_blitter__DOT__setup_fill__41__dst_space = 0;
    IData/*31:0*/ __Vtask_test_blitter__DOT__setup_fill__41__dst_addr;
    __Vtask_test_blitter__DOT__setup_fill__41__dst_addr = 0;
    IData/*31:0*/ __Vtask_test_blitter__DOT__setup_fill__41__dst_stride;
    __Vtask_test_blitter__DOT__setup_fill__41__dst_stride = 0;
    IData/*31:0*/ __Vtask_test_blitter__DOT__setup_fill__41__width;
    __Vtask_test_blitter__DOT__setup_fill__41__width = 0;
    IData/*31:0*/ __Vtask_test_blitter__DOT__setup_fill__41__height;
    __Vtask_test_blitter__DOT__setup_fill__41__height = 0;
    IData/*31:0*/ __Vtask_test_blitter__DOT__setup_fill__41__fill_val;
    __Vtask_test_blitter__DOT__setup_fill__41__fill_val = 0;
    IData/*31:0*/ __Vtask_test_blitter__DOT__blt_reg__42__off;
    __Vtask_test_blitter__DOT__blt_reg__42__off = 0;
    CData/*7:0*/ __Vtask_test_blitter__DOT__blt_reg__42__data;
    __Vtask_test_blitter__DOT__blt_reg__42__data = 0;
    SData/*15:0*/ __Vtask_test_blitter__DOT__write_reg__43__addr;
    __Vtask_test_blitter__DOT__write_reg__43__addr = 0;
    CData/*7:0*/ __Vtask_test_blitter__DOT__write_reg__43__data;
    __Vtask_test_blitter__DOT__write_reg__43__data = 0;
    IData/*31:0*/ __Vtask_test_blitter__DOT__blt_reg__44__off;
    __Vtask_test_blitter__DOT__blt_reg__44__off = 0;
    CData/*7:0*/ __Vtask_test_blitter__DOT__blt_reg__44__data;
    __Vtask_test_blitter__DOT__blt_reg__44__data = 0;
    SData/*15:0*/ __Vtask_test_blitter__DOT__write_reg__45__addr;
    __Vtask_test_blitter__DOT__write_reg__45__addr = 0;
    CData/*7:0*/ __Vtask_test_blitter__DOT__write_reg__45__data;
    __Vtask_test_blitter__DOT__write_reg__45__data = 0;
    IData/*31:0*/ __Vtask_test_blitter__DOT__blt_reg__46__off;
    __Vtask_test_blitter__DOT__blt_reg__46__off = 0;
    CData/*7:0*/ __Vtask_test_blitter__DOT__blt_reg__46__data;
    __Vtask_test_blitter__DOT__blt_reg__46__data = 0;
    SData/*15:0*/ __Vtask_test_blitter__DOT__write_reg__47__addr;
    __Vtask_test_blitter__DOT__write_reg__47__addr = 0;
    CData/*7:0*/ __Vtask_test_blitter__DOT__write_reg__47__data;
    __Vtask_test_blitter__DOT__write_reg__47__data = 0;
    IData/*31:0*/ __Vtask_test_blitter__DOT__blt_reg__48__off;
    __Vtask_test_blitter__DOT__blt_reg__48__off = 0;
    CData/*7:0*/ __Vtask_test_blitter__DOT__blt_reg__48__data;
    __Vtask_test_blitter__DOT__blt_reg__48__data = 0;
    SData/*15:0*/ __Vtask_test_blitter__DOT__write_reg__49__addr;
    __Vtask_test_blitter__DOT__write_reg__49__addr = 0;
    CData/*7:0*/ __Vtask_test_blitter__DOT__write_reg__49__data;
    __Vtask_test_blitter__DOT__write_reg__49__data = 0;
    IData/*31:0*/ __Vtask_test_blitter__DOT__blt_reg__50__off;
    __Vtask_test_blitter__DOT__blt_reg__50__off = 0;
    CData/*7:0*/ __Vtask_test_blitter__DOT__blt_reg__50__data;
    __Vtask_test_blitter__DOT__blt_reg__50__data = 0;
    SData/*15:0*/ __Vtask_test_blitter__DOT__write_reg__51__addr;
    __Vtask_test_blitter__DOT__write_reg__51__addr = 0;
    CData/*7:0*/ __Vtask_test_blitter__DOT__write_reg__51__data;
    __Vtask_test_blitter__DOT__write_reg__51__data = 0;
    IData/*31:0*/ __Vtask_test_blitter__DOT__blt_reg__52__off;
    __Vtask_test_blitter__DOT__blt_reg__52__off = 0;
    CData/*7:0*/ __Vtask_test_blitter__DOT__blt_reg__52__data;
    __Vtask_test_blitter__DOT__blt_reg__52__data = 0;
    SData/*15:0*/ __Vtask_test_blitter__DOT__write_reg__53__addr;
    __Vtask_test_blitter__DOT__write_reg__53__addr = 0;
    CData/*7:0*/ __Vtask_test_blitter__DOT__write_reg__53__data;
    __Vtask_test_blitter__DOT__write_reg__53__data = 0;
    IData/*31:0*/ __Vtask_test_blitter__DOT__blt_reg__54__off;
    __Vtask_test_blitter__DOT__blt_reg__54__off = 0;
    CData/*7:0*/ __Vtask_test_blitter__DOT__blt_reg__54__data;
    __Vtask_test_blitter__DOT__blt_reg__54__data = 0;
    SData/*15:0*/ __Vtask_test_blitter__DOT__write_reg__55__addr;
    __Vtask_test_blitter__DOT__write_reg__55__addr = 0;
    CData/*7:0*/ __Vtask_test_blitter__DOT__write_reg__55__data;
    __Vtask_test_blitter__DOT__write_reg__55__data = 0;
    IData/*31:0*/ __Vtask_test_blitter__DOT__blt_reg__56__off;
    __Vtask_test_blitter__DOT__blt_reg__56__off = 0;
    CData/*7:0*/ __Vtask_test_blitter__DOT__blt_reg__56__data;
    __Vtask_test_blitter__DOT__blt_reg__56__data = 0;
    SData/*15:0*/ __Vtask_test_blitter__DOT__write_reg__57__addr;
    __Vtask_test_blitter__DOT__write_reg__57__addr = 0;
    CData/*7:0*/ __Vtask_test_blitter__DOT__write_reg__57__data;
    __Vtask_test_blitter__DOT__write_reg__57__data = 0;
    IData/*31:0*/ __Vtask_test_blitter__DOT__blt_reg__58__off;
    __Vtask_test_blitter__DOT__blt_reg__58__off = 0;
    CData/*7:0*/ __Vtask_test_blitter__DOT__blt_reg__58__data;
    __Vtask_test_blitter__DOT__blt_reg__58__data = 0;
    SData/*15:0*/ __Vtask_test_blitter__DOT__write_reg__59__addr;
    __Vtask_test_blitter__DOT__write_reg__59__addr = 0;
    CData/*7:0*/ __Vtask_test_blitter__DOT__write_reg__59__data;
    __Vtask_test_blitter__DOT__write_reg__59__data = 0;
    IData/*31:0*/ __Vtask_test_blitter__DOT__blt_reg__60__off;
    __Vtask_test_blitter__DOT__blt_reg__60__off = 0;
    CData/*7:0*/ __Vtask_test_blitter__DOT__blt_reg__60__data;
    __Vtask_test_blitter__DOT__blt_reg__60__data = 0;
    SData/*15:0*/ __Vtask_test_blitter__DOT__write_reg__61__addr;
    __Vtask_test_blitter__DOT__write_reg__61__addr = 0;
    CData/*7:0*/ __Vtask_test_blitter__DOT__write_reg__61__data;
    __Vtask_test_blitter__DOT__write_reg__61__data = 0;
    IData/*31:0*/ __Vtask_test_blitter__DOT__blt_reg__62__off;
    __Vtask_test_blitter__DOT__blt_reg__62__off = 0;
    CData/*7:0*/ __Vtask_test_blitter__DOT__blt_reg__62__data;
    __Vtask_test_blitter__DOT__blt_reg__62__data = 0;
    SData/*15:0*/ __Vtask_test_blitter__DOT__write_reg__63__addr;
    __Vtask_test_blitter__DOT__write_reg__63__addr = 0;
    CData/*7:0*/ __Vtask_test_blitter__DOT__write_reg__63__data;
    __Vtask_test_blitter__DOT__write_reg__63__data = 0;
    IData/*31:0*/ __Vtask_test_blitter__DOT__blt_reg__64__off;
    __Vtask_test_blitter__DOT__blt_reg__64__off = 0;
    CData/*7:0*/ __Vtask_test_blitter__DOT__blt_reg__64__data;
    __Vtask_test_blitter__DOT__blt_reg__64__data = 0;
    SData/*15:0*/ __Vtask_test_blitter__DOT__write_reg__65__addr;
    __Vtask_test_blitter__DOT__write_reg__65__addr = 0;
    CData/*7:0*/ __Vtask_test_blitter__DOT__write_reg__65__data;
    __Vtask_test_blitter__DOT__write_reg__65__data = 0;
    IData/*31:0*/ __Vtask_test_blitter__DOT__blt_reg__67__off;
    __Vtask_test_blitter__DOT__blt_reg__67__off = 0;
    CData/*7:0*/ __Vtask_test_blitter__DOT__blt_reg__67__data;
    __Vtask_test_blitter__DOT__blt_reg__67__data = 0;
    SData/*15:0*/ __Vtask_test_blitter__DOT__write_reg__68__addr;
    __Vtask_test_blitter__DOT__write_reg__68__addr = 0;
    CData/*7:0*/ __Vtask_test_blitter__DOT__write_reg__68__data;
    __Vtask_test_blitter__DOT__write_reg__68__data = 0;
    IData/*31:0*/ __Vtask_test_blitter__DOT__wait_blt_done__69__test_blitter__DOT__unnamedblk1_1__DOT____Vrepeat0;
    __Vtask_test_blitter__DOT__wait_blt_done__69__test_blitter__DOT__unnamedblk1_1__DOT____Vrepeat0 = 0;
    IData/*31:0*/ __Vtask_test_blitter__DOT__wait_blt_done__69__test_blitter__DOT__unnamedblk1_2__DOT____Vrepeat1;
    __Vtask_test_blitter__DOT__wait_blt_done__69__test_blitter__DOT__unnamedblk1_2__DOT____Vrepeat1 = 0;
    CData/*0:0*/ __Vtask_test_blitter__DOT__check__70__condition;
    __Vtask_test_blitter__DOT__check__70__condition = 0;
    CData/*0:0*/ __Vtask_test_blitter__DOT__check__71__condition;
    __Vtask_test_blitter__DOT__check__71__condition = 0;
    CData/*0:0*/ __Vtask_test_blitter__DOT__check__72__condition;
    __Vtask_test_blitter__DOT__check__72__condition = 0;
    CData/*0:0*/ __Vtask_test_blitter__DOT__check__73__condition;
    __Vtask_test_blitter__DOT__check__73__condition = 0;
    CData/*0:0*/ __Vtask_test_blitter__DOT__check__74__condition;
    __Vtask_test_blitter__DOT__check__74__condition = 0;
    IData/*31:0*/ __Vtask_test_blitter__DOT__setup_copy__75__src_space;
    __Vtask_test_blitter__DOT__setup_copy__75__src_space = 0;
    IData/*31:0*/ __Vtask_test_blitter__DOT__setup_copy__75__src_addr;
    __Vtask_test_blitter__DOT__setup_copy__75__src_addr = 0;
    IData/*31:0*/ __Vtask_test_blitter__DOT__setup_copy__75__src_stride;
    __Vtask_test_blitter__DOT__setup_copy__75__src_stride = 0;
    IData/*31:0*/ __Vtask_test_blitter__DOT__setup_copy__75__dst_space;
    __Vtask_test_blitter__DOT__setup_copy__75__dst_space = 0;
    IData/*31:0*/ __Vtask_test_blitter__DOT__setup_copy__75__dst_addr;
    __Vtask_test_blitter__DOT__setup_copy__75__dst_addr = 0;
    IData/*31:0*/ __Vtask_test_blitter__DOT__setup_copy__75__dst_stride;
    __Vtask_test_blitter__DOT__setup_copy__75__dst_stride = 0;
    IData/*31:0*/ __Vtask_test_blitter__DOT__setup_copy__75__width;
    __Vtask_test_blitter__DOT__setup_copy__75__width = 0;
    IData/*31:0*/ __Vtask_test_blitter__DOT__setup_copy__75__height;
    __Vtask_test_blitter__DOT__setup_copy__75__height = 0;
    IData/*31:0*/ __Vtask_test_blitter__DOT__blt_reg__76__off;
    __Vtask_test_blitter__DOT__blt_reg__76__off = 0;
    CData/*7:0*/ __Vtask_test_blitter__DOT__blt_reg__76__data;
    __Vtask_test_blitter__DOT__blt_reg__76__data = 0;
    SData/*15:0*/ __Vtask_test_blitter__DOT__write_reg__77__addr;
    __Vtask_test_blitter__DOT__write_reg__77__addr = 0;
    CData/*7:0*/ __Vtask_test_blitter__DOT__write_reg__77__data;
    __Vtask_test_blitter__DOT__write_reg__77__data = 0;
    IData/*31:0*/ __Vtask_test_blitter__DOT__blt_reg__78__off;
    __Vtask_test_blitter__DOT__blt_reg__78__off = 0;
    CData/*7:0*/ __Vtask_test_blitter__DOT__blt_reg__78__data;
    __Vtask_test_blitter__DOT__blt_reg__78__data = 0;
    SData/*15:0*/ __Vtask_test_blitter__DOT__write_reg__79__addr;
    __Vtask_test_blitter__DOT__write_reg__79__addr = 0;
    CData/*7:0*/ __Vtask_test_blitter__DOT__write_reg__79__data;
    __Vtask_test_blitter__DOT__write_reg__79__data = 0;
    IData/*31:0*/ __Vtask_test_blitter__DOT__blt_reg__80__off;
    __Vtask_test_blitter__DOT__blt_reg__80__off = 0;
    CData/*7:0*/ __Vtask_test_blitter__DOT__blt_reg__80__data;
    __Vtask_test_blitter__DOT__blt_reg__80__data = 0;
    SData/*15:0*/ __Vtask_test_blitter__DOT__write_reg__81__addr;
    __Vtask_test_blitter__DOT__write_reg__81__addr = 0;
    CData/*7:0*/ __Vtask_test_blitter__DOT__write_reg__81__data;
    __Vtask_test_blitter__DOT__write_reg__81__data = 0;
    IData/*31:0*/ __Vtask_test_blitter__DOT__blt_reg__82__off;
    __Vtask_test_blitter__DOT__blt_reg__82__off = 0;
    CData/*7:0*/ __Vtask_test_blitter__DOT__blt_reg__82__data;
    __Vtask_test_blitter__DOT__blt_reg__82__data = 0;
    SData/*15:0*/ __Vtask_test_blitter__DOT__write_reg__83__addr;
    __Vtask_test_blitter__DOT__write_reg__83__addr = 0;
    CData/*7:0*/ __Vtask_test_blitter__DOT__write_reg__83__data;
    __Vtask_test_blitter__DOT__write_reg__83__data = 0;
    IData/*31:0*/ __Vtask_test_blitter__DOT__blt_reg__84__off;
    __Vtask_test_blitter__DOT__blt_reg__84__off = 0;
    CData/*7:0*/ __Vtask_test_blitter__DOT__blt_reg__84__data;
    __Vtask_test_blitter__DOT__blt_reg__84__data = 0;
    SData/*15:0*/ __Vtask_test_blitter__DOT__write_reg__85__addr;
    __Vtask_test_blitter__DOT__write_reg__85__addr = 0;
    CData/*7:0*/ __Vtask_test_blitter__DOT__write_reg__85__data;
    __Vtask_test_blitter__DOT__write_reg__85__data = 0;
    IData/*31:0*/ __Vtask_test_blitter__DOT__blt_reg__86__off;
    __Vtask_test_blitter__DOT__blt_reg__86__off = 0;
    CData/*7:0*/ __Vtask_test_blitter__DOT__blt_reg__86__data;
    __Vtask_test_blitter__DOT__blt_reg__86__data = 0;
    SData/*15:0*/ __Vtask_test_blitter__DOT__write_reg__87__addr;
    __Vtask_test_blitter__DOT__write_reg__87__addr = 0;
    CData/*7:0*/ __Vtask_test_blitter__DOT__write_reg__87__data;
    __Vtask_test_blitter__DOT__write_reg__87__data = 0;
    IData/*31:0*/ __Vtask_test_blitter__DOT__blt_reg__88__off;
    __Vtask_test_blitter__DOT__blt_reg__88__off = 0;
    CData/*7:0*/ __Vtask_test_blitter__DOT__blt_reg__88__data;
    __Vtask_test_blitter__DOT__blt_reg__88__data = 0;
    SData/*15:0*/ __Vtask_test_blitter__DOT__write_reg__89__addr;
    __Vtask_test_blitter__DOT__write_reg__89__addr = 0;
    CData/*7:0*/ __Vtask_test_blitter__DOT__write_reg__89__data;
    __Vtask_test_blitter__DOT__write_reg__89__data = 0;
    IData/*31:0*/ __Vtask_test_blitter__DOT__blt_reg__90__off;
    __Vtask_test_blitter__DOT__blt_reg__90__off = 0;
    CData/*7:0*/ __Vtask_test_blitter__DOT__blt_reg__90__data;
    __Vtask_test_blitter__DOT__blt_reg__90__data = 0;
    SData/*15:0*/ __Vtask_test_blitter__DOT__write_reg__91__addr;
    __Vtask_test_blitter__DOT__write_reg__91__addr = 0;
    CData/*7:0*/ __Vtask_test_blitter__DOT__write_reg__91__data;
    __Vtask_test_blitter__DOT__write_reg__91__data = 0;
    IData/*31:0*/ __Vtask_test_blitter__DOT__blt_reg__92__off;
    __Vtask_test_blitter__DOT__blt_reg__92__off = 0;
    CData/*7:0*/ __Vtask_test_blitter__DOT__blt_reg__92__data;
    __Vtask_test_blitter__DOT__blt_reg__92__data = 0;
    SData/*15:0*/ __Vtask_test_blitter__DOT__write_reg__93__addr;
    __Vtask_test_blitter__DOT__write_reg__93__addr = 0;
    CData/*7:0*/ __Vtask_test_blitter__DOT__write_reg__93__data;
    __Vtask_test_blitter__DOT__write_reg__93__data = 0;
    IData/*31:0*/ __Vtask_test_blitter__DOT__blt_reg__94__off;
    __Vtask_test_blitter__DOT__blt_reg__94__off = 0;
    CData/*7:0*/ __Vtask_test_blitter__DOT__blt_reg__94__data;
    __Vtask_test_blitter__DOT__blt_reg__94__data = 0;
    SData/*15:0*/ __Vtask_test_blitter__DOT__write_reg__95__addr;
    __Vtask_test_blitter__DOT__write_reg__95__addr = 0;
    CData/*7:0*/ __Vtask_test_blitter__DOT__write_reg__95__data;
    __Vtask_test_blitter__DOT__write_reg__95__data = 0;
    IData/*31:0*/ __Vtask_test_blitter__DOT__blt_reg__96__off;
    __Vtask_test_blitter__DOT__blt_reg__96__off = 0;
    CData/*7:0*/ __Vtask_test_blitter__DOT__blt_reg__96__data;
    __Vtask_test_blitter__DOT__blt_reg__96__data = 0;
    SData/*15:0*/ __Vtask_test_blitter__DOT__write_reg__97__addr;
    __Vtask_test_blitter__DOT__write_reg__97__addr = 0;
    CData/*7:0*/ __Vtask_test_blitter__DOT__write_reg__97__data;
    __Vtask_test_blitter__DOT__write_reg__97__data = 0;
    IData/*31:0*/ __Vtask_test_blitter__DOT__blt_reg__98__off;
    __Vtask_test_blitter__DOT__blt_reg__98__off = 0;
    CData/*7:0*/ __Vtask_test_blitter__DOT__blt_reg__98__data;
    __Vtask_test_blitter__DOT__blt_reg__98__data = 0;
    SData/*15:0*/ __Vtask_test_blitter__DOT__write_reg__99__addr;
    __Vtask_test_blitter__DOT__write_reg__99__addr = 0;
    CData/*7:0*/ __Vtask_test_blitter__DOT__write_reg__99__data;
    __Vtask_test_blitter__DOT__write_reg__99__data = 0;
    IData/*31:0*/ __Vtask_test_blitter__DOT__blt_reg__100__off;
    __Vtask_test_blitter__DOT__blt_reg__100__off = 0;
    CData/*7:0*/ __Vtask_test_blitter__DOT__blt_reg__100__data;
    __Vtask_test_blitter__DOT__blt_reg__100__data = 0;
    SData/*15:0*/ __Vtask_test_blitter__DOT__write_reg__101__addr;
    __Vtask_test_blitter__DOT__write_reg__101__addr = 0;
    CData/*7:0*/ __Vtask_test_blitter__DOT__write_reg__101__data;
    __Vtask_test_blitter__DOT__write_reg__101__data = 0;
    IData/*31:0*/ __Vtask_test_blitter__DOT__blt_reg__102__off;
    __Vtask_test_blitter__DOT__blt_reg__102__off = 0;
    CData/*7:0*/ __Vtask_test_blitter__DOT__blt_reg__102__data;
    __Vtask_test_blitter__DOT__blt_reg__102__data = 0;
    SData/*15:0*/ __Vtask_test_blitter__DOT__write_reg__103__addr;
    __Vtask_test_blitter__DOT__write_reg__103__addr = 0;
    CData/*7:0*/ __Vtask_test_blitter__DOT__write_reg__103__data;
    __Vtask_test_blitter__DOT__write_reg__103__data = 0;
    IData/*31:0*/ __Vtask_test_blitter__DOT__blt_reg__104__off;
    __Vtask_test_blitter__DOT__blt_reg__104__off = 0;
    CData/*7:0*/ __Vtask_test_blitter__DOT__blt_reg__104__data;
    __Vtask_test_blitter__DOT__blt_reg__104__data = 0;
    SData/*15:0*/ __Vtask_test_blitter__DOT__write_reg__105__addr;
    __Vtask_test_blitter__DOT__write_reg__105__addr = 0;
    CData/*7:0*/ __Vtask_test_blitter__DOT__write_reg__105__data;
    __Vtask_test_blitter__DOT__write_reg__105__data = 0;
    IData/*31:0*/ __Vtask_test_blitter__DOT__blt_reg__106__off;
    __Vtask_test_blitter__DOT__blt_reg__106__off = 0;
    CData/*7:0*/ __Vtask_test_blitter__DOT__blt_reg__106__data;
    __Vtask_test_blitter__DOT__blt_reg__106__data = 0;
    SData/*15:0*/ __Vtask_test_blitter__DOT__write_reg__107__addr;
    __Vtask_test_blitter__DOT__write_reg__107__addr = 0;
    CData/*7:0*/ __Vtask_test_blitter__DOT__write_reg__107__data;
    __Vtask_test_blitter__DOT__write_reg__107__data = 0;
    IData/*31:0*/ __Vtask_test_blitter__DOT__blt_reg__108__off;
    __Vtask_test_blitter__DOT__blt_reg__108__off = 0;
    CData/*7:0*/ __Vtask_test_blitter__DOT__blt_reg__108__data;
    __Vtask_test_blitter__DOT__blt_reg__108__data = 0;
    SData/*15:0*/ __Vtask_test_blitter__DOT__write_reg__109__addr;
    __Vtask_test_blitter__DOT__write_reg__109__addr = 0;
    CData/*7:0*/ __Vtask_test_blitter__DOT__write_reg__109__data;
    __Vtask_test_blitter__DOT__write_reg__109__data = 0;
    IData/*31:0*/ __Vtask_test_blitter__DOT__blt_reg__111__off;
    __Vtask_test_blitter__DOT__blt_reg__111__off = 0;
    CData/*7:0*/ __Vtask_test_blitter__DOT__blt_reg__111__data;
    __Vtask_test_blitter__DOT__blt_reg__111__data = 0;
    SData/*15:0*/ __Vtask_test_blitter__DOT__write_reg__112__addr;
    __Vtask_test_blitter__DOT__write_reg__112__addr = 0;
    CData/*7:0*/ __Vtask_test_blitter__DOT__write_reg__112__data;
    __Vtask_test_blitter__DOT__write_reg__112__data = 0;
    IData/*31:0*/ __Vtask_test_blitter__DOT__wait_blt_done__113__test_blitter__DOT__unnamedblk1_1__DOT____Vrepeat0;
    __Vtask_test_blitter__DOT__wait_blt_done__113__test_blitter__DOT__unnamedblk1_1__DOT____Vrepeat0 = 0;
    IData/*31:0*/ __Vtask_test_blitter__DOT__wait_blt_done__113__test_blitter__DOT__unnamedblk1_2__DOT____Vrepeat1;
    __Vtask_test_blitter__DOT__wait_blt_done__113__test_blitter__DOT__unnamedblk1_2__DOT____Vrepeat1 = 0;
    CData/*0:0*/ __Vtask_test_blitter__DOT__check__114__condition;
    __Vtask_test_blitter__DOT__check__114__condition = 0;
    CData/*0:0*/ __Vtask_test_blitter__DOT__check__115__condition;
    __Vtask_test_blitter__DOT__check__115__condition = 0;
    CData/*0:0*/ __Vtask_test_blitter__DOT__check__116__condition;
    __Vtask_test_blitter__DOT__check__116__condition = 0;
    CData/*0:0*/ __Vtask_test_blitter__DOT__check__117__condition;
    __Vtask_test_blitter__DOT__check__117__condition = 0;
    CData/*0:0*/ __Vtask_test_blitter__DOT__check__118__condition;
    __Vtask_test_blitter__DOT__check__118__condition = 0;
    CData/*0:0*/ __Vtask_test_blitter__DOT__check__119__condition;
    __Vtask_test_blitter__DOT__check__119__condition = 0;
    IData/*31:0*/ __Vtask_test_blitter__DOT__setup_copy__120__src_space;
    __Vtask_test_blitter__DOT__setup_copy__120__src_space = 0;
    IData/*31:0*/ __Vtask_test_blitter__DOT__setup_copy__120__src_addr;
    __Vtask_test_blitter__DOT__setup_copy__120__src_addr = 0;
    IData/*31:0*/ __Vtask_test_blitter__DOT__setup_copy__120__src_stride;
    __Vtask_test_blitter__DOT__setup_copy__120__src_stride = 0;
    IData/*31:0*/ __Vtask_test_blitter__DOT__setup_copy__120__dst_space;
    __Vtask_test_blitter__DOT__setup_copy__120__dst_space = 0;
    IData/*31:0*/ __Vtask_test_blitter__DOT__setup_copy__120__dst_addr;
    __Vtask_test_blitter__DOT__setup_copy__120__dst_addr = 0;
    IData/*31:0*/ __Vtask_test_blitter__DOT__setup_copy__120__dst_stride;
    __Vtask_test_blitter__DOT__setup_copy__120__dst_stride = 0;
    IData/*31:0*/ __Vtask_test_blitter__DOT__setup_copy__120__width;
    __Vtask_test_blitter__DOT__setup_copy__120__width = 0;
    IData/*31:0*/ __Vtask_test_blitter__DOT__setup_copy__120__height;
    __Vtask_test_blitter__DOT__setup_copy__120__height = 0;
    IData/*31:0*/ __Vtask_test_blitter__DOT__blt_reg__121__off;
    __Vtask_test_blitter__DOT__blt_reg__121__off = 0;
    CData/*7:0*/ __Vtask_test_blitter__DOT__blt_reg__121__data;
    __Vtask_test_blitter__DOT__blt_reg__121__data = 0;
    SData/*15:0*/ __Vtask_test_blitter__DOT__write_reg__122__addr;
    __Vtask_test_blitter__DOT__write_reg__122__addr = 0;
    CData/*7:0*/ __Vtask_test_blitter__DOT__write_reg__122__data;
    __Vtask_test_blitter__DOT__write_reg__122__data = 0;
    IData/*31:0*/ __Vtask_test_blitter__DOT__blt_reg__123__off;
    __Vtask_test_blitter__DOT__blt_reg__123__off = 0;
    CData/*7:0*/ __Vtask_test_blitter__DOT__blt_reg__123__data;
    __Vtask_test_blitter__DOT__blt_reg__123__data = 0;
    SData/*15:0*/ __Vtask_test_blitter__DOT__write_reg__124__addr;
    __Vtask_test_blitter__DOT__write_reg__124__addr = 0;
    CData/*7:0*/ __Vtask_test_blitter__DOT__write_reg__124__data;
    __Vtask_test_blitter__DOT__write_reg__124__data = 0;
    IData/*31:0*/ __Vtask_test_blitter__DOT__blt_reg__125__off;
    __Vtask_test_blitter__DOT__blt_reg__125__off = 0;
    CData/*7:0*/ __Vtask_test_blitter__DOT__blt_reg__125__data;
    __Vtask_test_blitter__DOT__blt_reg__125__data = 0;
    SData/*15:0*/ __Vtask_test_blitter__DOT__write_reg__126__addr;
    __Vtask_test_blitter__DOT__write_reg__126__addr = 0;
    CData/*7:0*/ __Vtask_test_blitter__DOT__write_reg__126__data;
    __Vtask_test_blitter__DOT__write_reg__126__data = 0;
    IData/*31:0*/ __Vtask_test_blitter__DOT__blt_reg__127__off;
    __Vtask_test_blitter__DOT__blt_reg__127__off = 0;
    CData/*7:0*/ __Vtask_test_blitter__DOT__blt_reg__127__data;
    __Vtask_test_blitter__DOT__blt_reg__127__data = 0;
    SData/*15:0*/ __Vtask_test_blitter__DOT__write_reg__128__addr;
    __Vtask_test_blitter__DOT__write_reg__128__addr = 0;
    CData/*7:0*/ __Vtask_test_blitter__DOT__write_reg__128__data;
    __Vtask_test_blitter__DOT__write_reg__128__data = 0;
    IData/*31:0*/ __Vtask_test_blitter__DOT__blt_reg__129__off;
    __Vtask_test_blitter__DOT__blt_reg__129__off = 0;
    CData/*7:0*/ __Vtask_test_blitter__DOT__blt_reg__129__data;
    __Vtask_test_blitter__DOT__blt_reg__129__data = 0;
    SData/*15:0*/ __Vtask_test_blitter__DOT__write_reg__130__addr;
    __Vtask_test_blitter__DOT__write_reg__130__addr = 0;
    CData/*7:0*/ __Vtask_test_blitter__DOT__write_reg__130__data;
    __Vtask_test_blitter__DOT__write_reg__130__data = 0;
    IData/*31:0*/ __Vtask_test_blitter__DOT__blt_reg__131__off;
    __Vtask_test_blitter__DOT__blt_reg__131__off = 0;
    CData/*7:0*/ __Vtask_test_blitter__DOT__blt_reg__131__data;
    __Vtask_test_blitter__DOT__blt_reg__131__data = 0;
    SData/*15:0*/ __Vtask_test_blitter__DOT__write_reg__132__addr;
    __Vtask_test_blitter__DOT__write_reg__132__addr = 0;
    CData/*7:0*/ __Vtask_test_blitter__DOT__write_reg__132__data;
    __Vtask_test_blitter__DOT__write_reg__132__data = 0;
    IData/*31:0*/ __Vtask_test_blitter__DOT__blt_reg__133__off;
    __Vtask_test_blitter__DOT__blt_reg__133__off = 0;
    CData/*7:0*/ __Vtask_test_blitter__DOT__blt_reg__133__data;
    __Vtask_test_blitter__DOT__blt_reg__133__data = 0;
    SData/*15:0*/ __Vtask_test_blitter__DOT__write_reg__134__addr;
    __Vtask_test_blitter__DOT__write_reg__134__addr = 0;
    CData/*7:0*/ __Vtask_test_blitter__DOT__write_reg__134__data;
    __Vtask_test_blitter__DOT__write_reg__134__data = 0;
    IData/*31:0*/ __Vtask_test_blitter__DOT__blt_reg__135__off;
    __Vtask_test_blitter__DOT__blt_reg__135__off = 0;
    CData/*7:0*/ __Vtask_test_blitter__DOT__blt_reg__135__data;
    __Vtask_test_blitter__DOT__blt_reg__135__data = 0;
    SData/*15:0*/ __Vtask_test_blitter__DOT__write_reg__136__addr;
    __Vtask_test_blitter__DOT__write_reg__136__addr = 0;
    CData/*7:0*/ __Vtask_test_blitter__DOT__write_reg__136__data;
    __Vtask_test_blitter__DOT__write_reg__136__data = 0;
    IData/*31:0*/ __Vtask_test_blitter__DOT__blt_reg__137__off;
    __Vtask_test_blitter__DOT__blt_reg__137__off = 0;
    CData/*7:0*/ __Vtask_test_blitter__DOT__blt_reg__137__data;
    __Vtask_test_blitter__DOT__blt_reg__137__data = 0;
    SData/*15:0*/ __Vtask_test_blitter__DOT__write_reg__138__addr;
    __Vtask_test_blitter__DOT__write_reg__138__addr = 0;
    CData/*7:0*/ __Vtask_test_blitter__DOT__write_reg__138__data;
    __Vtask_test_blitter__DOT__write_reg__138__data = 0;
    IData/*31:0*/ __Vtask_test_blitter__DOT__blt_reg__139__off;
    __Vtask_test_blitter__DOT__blt_reg__139__off = 0;
    CData/*7:0*/ __Vtask_test_blitter__DOT__blt_reg__139__data;
    __Vtask_test_blitter__DOT__blt_reg__139__data = 0;
    SData/*15:0*/ __Vtask_test_blitter__DOT__write_reg__140__addr;
    __Vtask_test_blitter__DOT__write_reg__140__addr = 0;
    CData/*7:0*/ __Vtask_test_blitter__DOT__write_reg__140__data;
    __Vtask_test_blitter__DOT__write_reg__140__data = 0;
    IData/*31:0*/ __Vtask_test_blitter__DOT__blt_reg__141__off;
    __Vtask_test_blitter__DOT__blt_reg__141__off = 0;
    CData/*7:0*/ __Vtask_test_blitter__DOT__blt_reg__141__data;
    __Vtask_test_blitter__DOT__blt_reg__141__data = 0;
    SData/*15:0*/ __Vtask_test_blitter__DOT__write_reg__142__addr;
    __Vtask_test_blitter__DOT__write_reg__142__addr = 0;
    CData/*7:0*/ __Vtask_test_blitter__DOT__write_reg__142__data;
    __Vtask_test_blitter__DOT__write_reg__142__data = 0;
    IData/*31:0*/ __Vtask_test_blitter__DOT__blt_reg__143__off;
    __Vtask_test_blitter__DOT__blt_reg__143__off = 0;
    CData/*7:0*/ __Vtask_test_blitter__DOT__blt_reg__143__data;
    __Vtask_test_blitter__DOT__blt_reg__143__data = 0;
    SData/*15:0*/ __Vtask_test_blitter__DOT__write_reg__144__addr;
    __Vtask_test_blitter__DOT__write_reg__144__addr = 0;
    CData/*7:0*/ __Vtask_test_blitter__DOT__write_reg__144__data;
    __Vtask_test_blitter__DOT__write_reg__144__data = 0;
    IData/*31:0*/ __Vtask_test_blitter__DOT__blt_reg__145__off;
    __Vtask_test_blitter__DOT__blt_reg__145__off = 0;
    CData/*7:0*/ __Vtask_test_blitter__DOT__blt_reg__145__data;
    __Vtask_test_blitter__DOT__blt_reg__145__data = 0;
    SData/*15:0*/ __Vtask_test_blitter__DOT__write_reg__146__addr;
    __Vtask_test_blitter__DOT__write_reg__146__addr = 0;
    CData/*7:0*/ __Vtask_test_blitter__DOT__write_reg__146__data;
    __Vtask_test_blitter__DOT__write_reg__146__data = 0;
    IData/*31:0*/ __Vtask_test_blitter__DOT__blt_reg__147__off;
    __Vtask_test_blitter__DOT__blt_reg__147__off = 0;
    CData/*7:0*/ __Vtask_test_blitter__DOT__blt_reg__147__data;
    __Vtask_test_blitter__DOT__blt_reg__147__data = 0;
    SData/*15:0*/ __Vtask_test_blitter__DOT__write_reg__148__addr;
    __Vtask_test_blitter__DOT__write_reg__148__addr = 0;
    CData/*7:0*/ __Vtask_test_blitter__DOT__write_reg__148__data;
    __Vtask_test_blitter__DOT__write_reg__148__data = 0;
    IData/*31:0*/ __Vtask_test_blitter__DOT__blt_reg__149__off;
    __Vtask_test_blitter__DOT__blt_reg__149__off = 0;
    CData/*7:0*/ __Vtask_test_blitter__DOT__blt_reg__149__data;
    __Vtask_test_blitter__DOT__blt_reg__149__data = 0;
    SData/*15:0*/ __Vtask_test_blitter__DOT__write_reg__150__addr;
    __Vtask_test_blitter__DOT__write_reg__150__addr = 0;
    CData/*7:0*/ __Vtask_test_blitter__DOT__write_reg__150__data;
    __Vtask_test_blitter__DOT__write_reg__150__data = 0;
    IData/*31:0*/ __Vtask_test_blitter__DOT__blt_reg__151__off;
    __Vtask_test_blitter__DOT__blt_reg__151__off = 0;
    CData/*7:0*/ __Vtask_test_blitter__DOT__blt_reg__151__data;
    __Vtask_test_blitter__DOT__blt_reg__151__data = 0;
    SData/*15:0*/ __Vtask_test_blitter__DOT__write_reg__152__addr;
    __Vtask_test_blitter__DOT__write_reg__152__addr = 0;
    CData/*7:0*/ __Vtask_test_blitter__DOT__write_reg__152__data;
    __Vtask_test_blitter__DOT__write_reg__152__data = 0;
    IData/*31:0*/ __Vtask_test_blitter__DOT__blt_reg__153__off;
    __Vtask_test_blitter__DOT__blt_reg__153__off = 0;
    CData/*7:0*/ __Vtask_test_blitter__DOT__blt_reg__153__data;
    __Vtask_test_blitter__DOT__blt_reg__153__data = 0;
    SData/*15:0*/ __Vtask_test_blitter__DOT__write_reg__154__addr;
    __Vtask_test_blitter__DOT__write_reg__154__addr = 0;
    CData/*7:0*/ __Vtask_test_blitter__DOT__write_reg__154__data;
    __Vtask_test_blitter__DOT__write_reg__154__data = 0;
    IData/*31:0*/ __Vtask_test_blitter__DOT__blt_reg__156__off;
    __Vtask_test_blitter__DOT__blt_reg__156__off = 0;
    CData/*7:0*/ __Vtask_test_blitter__DOT__blt_reg__156__data;
    __Vtask_test_blitter__DOT__blt_reg__156__data = 0;
    SData/*15:0*/ __Vtask_test_blitter__DOT__write_reg__157__addr;
    __Vtask_test_blitter__DOT__write_reg__157__addr = 0;
    CData/*7:0*/ __Vtask_test_blitter__DOT__write_reg__157__data;
    __Vtask_test_blitter__DOT__write_reg__157__data = 0;
    IData/*31:0*/ __Vtask_test_blitter__DOT__wait_blt_done__158__test_blitter__DOT__unnamedblk1_1__DOT____Vrepeat0;
    __Vtask_test_blitter__DOT__wait_blt_done__158__test_blitter__DOT__unnamedblk1_1__DOT____Vrepeat0 = 0;
    IData/*31:0*/ __Vtask_test_blitter__DOT__wait_blt_done__158__test_blitter__DOT__unnamedblk1_2__DOT____Vrepeat1;
    __Vtask_test_blitter__DOT__wait_blt_done__158__test_blitter__DOT__unnamedblk1_2__DOT____Vrepeat1 = 0;
    CData/*0:0*/ __Vtask_test_blitter__DOT__check__159__condition;
    __Vtask_test_blitter__DOT__check__159__condition = 0;
    CData/*0:0*/ __Vtask_test_blitter__DOT__check__160__condition;
    __Vtask_test_blitter__DOT__check__160__condition = 0;
    CData/*0:0*/ __Vtask_test_blitter__DOT__check__161__condition;
    __Vtask_test_blitter__DOT__check__161__condition = 0;
    CData/*0:0*/ __Vtask_test_blitter__DOT__check__162__condition;
    __Vtask_test_blitter__DOT__check__162__condition = 0;
    IData/*31:0*/ __Vtask_test_blitter__DOT__setup_copy__163__src_space;
    __Vtask_test_blitter__DOT__setup_copy__163__src_space = 0;
    IData/*31:0*/ __Vtask_test_blitter__DOT__setup_copy__163__src_addr;
    __Vtask_test_blitter__DOT__setup_copy__163__src_addr = 0;
    IData/*31:0*/ __Vtask_test_blitter__DOT__setup_copy__163__src_stride;
    __Vtask_test_blitter__DOT__setup_copy__163__src_stride = 0;
    IData/*31:0*/ __Vtask_test_blitter__DOT__setup_copy__163__dst_space;
    __Vtask_test_blitter__DOT__setup_copy__163__dst_space = 0;
    IData/*31:0*/ __Vtask_test_blitter__DOT__setup_copy__163__dst_addr;
    __Vtask_test_blitter__DOT__setup_copy__163__dst_addr = 0;
    IData/*31:0*/ __Vtask_test_blitter__DOT__setup_copy__163__dst_stride;
    __Vtask_test_blitter__DOT__setup_copy__163__dst_stride = 0;
    IData/*31:0*/ __Vtask_test_blitter__DOT__setup_copy__163__width;
    __Vtask_test_blitter__DOT__setup_copy__163__width = 0;
    IData/*31:0*/ __Vtask_test_blitter__DOT__setup_copy__163__height;
    __Vtask_test_blitter__DOT__setup_copy__163__height = 0;
    IData/*31:0*/ __Vtask_test_blitter__DOT__blt_reg__164__off;
    __Vtask_test_blitter__DOT__blt_reg__164__off = 0;
    CData/*7:0*/ __Vtask_test_blitter__DOT__blt_reg__164__data;
    __Vtask_test_blitter__DOT__blt_reg__164__data = 0;
    SData/*15:0*/ __Vtask_test_blitter__DOT__write_reg__165__addr;
    __Vtask_test_blitter__DOT__write_reg__165__addr = 0;
    CData/*7:0*/ __Vtask_test_blitter__DOT__write_reg__165__data;
    __Vtask_test_blitter__DOT__write_reg__165__data = 0;
    IData/*31:0*/ __Vtask_test_blitter__DOT__blt_reg__166__off;
    __Vtask_test_blitter__DOT__blt_reg__166__off = 0;
    CData/*7:0*/ __Vtask_test_blitter__DOT__blt_reg__166__data;
    __Vtask_test_blitter__DOT__blt_reg__166__data = 0;
    SData/*15:0*/ __Vtask_test_blitter__DOT__write_reg__167__addr;
    __Vtask_test_blitter__DOT__write_reg__167__addr = 0;
    CData/*7:0*/ __Vtask_test_blitter__DOT__write_reg__167__data;
    __Vtask_test_blitter__DOT__write_reg__167__data = 0;
    IData/*31:0*/ __Vtask_test_blitter__DOT__blt_reg__168__off;
    __Vtask_test_blitter__DOT__blt_reg__168__off = 0;
    CData/*7:0*/ __Vtask_test_blitter__DOT__blt_reg__168__data;
    __Vtask_test_blitter__DOT__blt_reg__168__data = 0;
    SData/*15:0*/ __Vtask_test_blitter__DOT__write_reg__169__addr;
    __Vtask_test_blitter__DOT__write_reg__169__addr = 0;
    CData/*7:0*/ __Vtask_test_blitter__DOT__write_reg__169__data;
    __Vtask_test_blitter__DOT__write_reg__169__data = 0;
    IData/*31:0*/ __Vtask_test_blitter__DOT__blt_reg__170__off;
    __Vtask_test_blitter__DOT__blt_reg__170__off = 0;
    CData/*7:0*/ __Vtask_test_blitter__DOT__blt_reg__170__data;
    __Vtask_test_blitter__DOT__blt_reg__170__data = 0;
    SData/*15:0*/ __Vtask_test_blitter__DOT__write_reg__171__addr;
    __Vtask_test_blitter__DOT__write_reg__171__addr = 0;
    CData/*7:0*/ __Vtask_test_blitter__DOT__write_reg__171__data;
    __Vtask_test_blitter__DOT__write_reg__171__data = 0;
    IData/*31:0*/ __Vtask_test_blitter__DOT__blt_reg__172__off;
    __Vtask_test_blitter__DOT__blt_reg__172__off = 0;
    CData/*7:0*/ __Vtask_test_blitter__DOT__blt_reg__172__data;
    __Vtask_test_blitter__DOT__blt_reg__172__data = 0;
    SData/*15:0*/ __Vtask_test_blitter__DOT__write_reg__173__addr;
    __Vtask_test_blitter__DOT__write_reg__173__addr = 0;
    CData/*7:0*/ __Vtask_test_blitter__DOT__write_reg__173__data;
    __Vtask_test_blitter__DOT__write_reg__173__data = 0;
    IData/*31:0*/ __Vtask_test_blitter__DOT__blt_reg__174__off;
    __Vtask_test_blitter__DOT__blt_reg__174__off = 0;
    CData/*7:0*/ __Vtask_test_blitter__DOT__blt_reg__174__data;
    __Vtask_test_blitter__DOT__blt_reg__174__data = 0;
    SData/*15:0*/ __Vtask_test_blitter__DOT__write_reg__175__addr;
    __Vtask_test_blitter__DOT__write_reg__175__addr = 0;
    CData/*7:0*/ __Vtask_test_blitter__DOT__write_reg__175__data;
    __Vtask_test_blitter__DOT__write_reg__175__data = 0;
    IData/*31:0*/ __Vtask_test_blitter__DOT__blt_reg__176__off;
    __Vtask_test_blitter__DOT__blt_reg__176__off = 0;
    CData/*7:0*/ __Vtask_test_blitter__DOT__blt_reg__176__data;
    __Vtask_test_blitter__DOT__blt_reg__176__data = 0;
    SData/*15:0*/ __Vtask_test_blitter__DOT__write_reg__177__addr;
    __Vtask_test_blitter__DOT__write_reg__177__addr = 0;
    CData/*7:0*/ __Vtask_test_blitter__DOT__write_reg__177__data;
    __Vtask_test_blitter__DOT__write_reg__177__data = 0;
    IData/*31:0*/ __Vtask_test_blitter__DOT__blt_reg__178__off;
    __Vtask_test_blitter__DOT__blt_reg__178__off = 0;
    CData/*7:0*/ __Vtask_test_blitter__DOT__blt_reg__178__data;
    __Vtask_test_blitter__DOT__blt_reg__178__data = 0;
    SData/*15:0*/ __Vtask_test_blitter__DOT__write_reg__179__addr;
    __Vtask_test_blitter__DOT__write_reg__179__addr = 0;
    CData/*7:0*/ __Vtask_test_blitter__DOT__write_reg__179__data;
    __Vtask_test_blitter__DOT__write_reg__179__data = 0;
    IData/*31:0*/ __Vtask_test_blitter__DOT__blt_reg__180__off;
    __Vtask_test_blitter__DOT__blt_reg__180__off = 0;
    CData/*7:0*/ __Vtask_test_blitter__DOT__blt_reg__180__data;
    __Vtask_test_blitter__DOT__blt_reg__180__data = 0;
    SData/*15:0*/ __Vtask_test_blitter__DOT__write_reg__181__addr;
    __Vtask_test_blitter__DOT__write_reg__181__addr = 0;
    CData/*7:0*/ __Vtask_test_blitter__DOT__write_reg__181__data;
    __Vtask_test_blitter__DOT__write_reg__181__data = 0;
    IData/*31:0*/ __Vtask_test_blitter__DOT__blt_reg__182__off;
    __Vtask_test_blitter__DOT__blt_reg__182__off = 0;
    CData/*7:0*/ __Vtask_test_blitter__DOT__blt_reg__182__data;
    __Vtask_test_blitter__DOT__blt_reg__182__data = 0;
    SData/*15:0*/ __Vtask_test_blitter__DOT__write_reg__183__addr;
    __Vtask_test_blitter__DOT__write_reg__183__addr = 0;
    CData/*7:0*/ __Vtask_test_blitter__DOT__write_reg__183__data;
    __Vtask_test_blitter__DOT__write_reg__183__data = 0;
    IData/*31:0*/ __Vtask_test_blitter__DOT__blt_reg__184__off;
    __Vtask_test_blitter__DOT__blt_reg__184__off = 0;
    CData/*7:0*/ __Vtask_test_blitter__DOT__blt_reg__184__data;
    __Vtask_test_blitter__DOT__blt_reg__184__data = 0;
    SData/*15:0*/ __Vtask_test_blitter__DOT__write_reg__185__addr;
    __Vtask_test_blitter__DOT__write_reg__185__addr = 0;
    CData/*7:0*/ __Vtask_test_blitter__DOT__write_reg__185__data;
    __Vtask_test_blitter__DOT__write_reg__185__data = 0;
    IData/*31:0*/ __Vtask_test_blitter__DOT__blt_reg__186__off;
    __Vtask_test_blitter__DOT__blt_reg__186__off = 0;
    CData/*7:0*/ __Vtask_test_blitter__DOT__blt_reg__186__data;
    __Vtask_test_blitter__DOT__blt_reg__186__data = 0;
    SData/*15:0*/ __Vtask_test_blitter__DOT__write_reg__187__addr;
    __Vtask_test_blitter__DOT__write_reg__187__addr = 0;
    CData/*7:0*/ __Vtask_test_blitter__DOT__write_reg__187__data;
    __Vtask_test_blitter__DOT__write_reg__187__data = 0;
    IData/*31:0*/ __Vtask_test_blitter__DOT__blt_reg__188__off;
    __Vtask_test_blitter__DOT__blt_reg__188__off = 0;
    CData/*7:0*/ __Vtask_test_blitter__DOT__blt_reg__188__data;
    __Vtask_test_blitter__DOT__blt_reg__188__data = 0;
    SData/*15:0*/ __Vtask_test_blitter__DOT__write_reg__189__addr;
    __Vtask_test_blitter__DOT__write_reg__189__addr = 0;
    CData/*7:0*/ __Vtask_test_blitter__DOT__write_reg__189__data;
    __Vtask_test_blitter__DOT__write_reg__189__data = 0;
    IData/*31:0*/ __Vtask_test_blitter__DOT__blt_reg__190__off;
    __Vtask_test_blitter__DOT__blt_reg__190__off = 0;
    CData/*7:0*/ __Vtask_test_blitter__DOT__blt_reg__190__data;
    __Vtask_test_blitter__DOT__blt_reg__190__data = 0;
    SData/*15:0*/ __Vtask_test_blitter__DOT__write_reg__191__addr;
    __Vtask_test_blitter__DOT__write_reg__191__addr = 0;
    CData/*7:0*/ __Vtask_test_blitter__DOT__write_reg__191__data;
    __Vtask_test_blitter__DOT__write_reg__191__data = 0;
    IData/*31:0*/ __Vtask_test_blitter__DOT__blt_reg__192__off;
    __Vtask_test_blitter__DOT__blt_reg__192__off = 0;
    CData/*7:0*/ __Vtask_test_blitter__DOT__blt_reg__192__data;
    __Vtask_test_blitter__DOT__blt_reg__192__data = 0;
    SData/*15:0*/ __Vtask_test_blitter__DOT__write_reg__193__addr;
    __Vtask_test_blitter__DOT__write_reg__193__addr = 0;
    CData/*7:0*/ __Vtask_test_blitter__DOT__write_reg__193__data;
    __Vtask_test_blitter__DOT__write_reg__193__data = 0;
    IData/*31:0*/ __Vtask_test_blitter__DOT__blt_reg__194__off;
    __Vtask_test_blitter__DOT__blt_reg__194__off = 0;
    CData/*7:0*/ __Vtask_test_blitter__DOT__blt_reg__194__data;
    __Vtask_test_blitter__DOT__blt_reg__194__data = 0;
    SData/*15:0*/ __Vtask_test_blitter__DOT__write_reg__195__addr;
    __Vtask_test_blitter__DOT__write_reg__195__addr = 0;
    CData/*7:0*/ __Vtask_test_blitter__DOT__write_reg__195__data;
    __Vtask_test_blitter__DOT__write_reg__195__data = 0;
    IData/*31:0*/ __Vtask_test_blitter__DOT__blt_reg__196__off;
    __Vtask_test_blitter__DOT__blt_reg__196__off = 0;
    CData/*7:0*/ __Vtask_test_blitter__DOT__blt_reg__196__data;
    __Vtask_test_blitter__DOT__blt_reg__196__data = 0;
    SData/*15:0*/ __Vtask_test_blitter__DOT__write_reg__197__addr;
    __Vtask_test_blitter__DOT__write_reg__197__addr = 0;
    CData/*7:0*/ __Vtask_test_blitter__DOT__write_reg__197__data;
    __Vtask_test_blitter__DOT__write_reg__197__data = 0;
    IData/*31:0*/ __Vtask_test_blitter__DOT__blt_reg__198__off;
    __Vtask_test_blitter__DOT__blt_reg__198__off = 0;
    CData/*7:0*/ __Vtask_test_blitter__DOT__blt_reg__198__data;
    __Vtask_test_blitter__DOT__blt_reg__198__data = 0;
    SData/*15:0*/ __Vtask_test_blitter__DOT__write_reg__199__addr;
    __Vtask_test_blitter__DOT__write_reg__199__addr = 0;
    CData/*7:0*/ __Vtask_test_blitter__DOT__write_reg__199__data;
    __Vtask_test_blitter__DOT__write_reg__199__data = 0;
    IData/*31:0*/ __Vtask_test_blitter__DOT__blt_reg__200__off;
    __Vtask_test_blitter__DOT__blt_reg__200__off = 0;
    CData/*7:0*/ __Vtask_test_blitter__DOT__blt_reg__200__data;
    __Vtask_test_blitter__DOT__blt_reg__200__data = 0;
    SData/*15:0*/ __Vtask_test_blitter__DOT__write_reg__201__addr;
    __Vtask_test_blitter__DOT__write_reg__201__addr = 0;
    CData/*7:0*/ __Vtask_test_blitter__DOT__write_reg__201__data;
    __Vtask_test_blitter__DOT__write_reg__201__data = 0;
    IData/*31:0*/ __Vtask_test_blitter__DOT__blt_reg__203__off;
    __Vtask_test_blitter__DOT__blt_reg__203__off = 0;
    CData/*7:0*/ __Vtask_test_blitter__DOT__blt_reg__203__data;
    __Vtask_test_blitter__DOT__blt_reg__203__data = 0;
    SData/*15:0*/ __Vtask_test_blitter__DOT__write_reg__204__addr;
    __Vtask_test_blitter__DOT__write_reg__204__addr = 0;
    CData/*7:0*/ __Vtask_test_blitter__DOT__write_reg__204__data;
    __Vtask_test_blitter__DOT__write_reg__204__data = 0;
    IData/*31:0*/ __Vtask_test_blitter__DOT__wait_blt_done__205__test_blitter__DOT__unnamedblk1_1__DOT____Vrepeat0;
    __Vtask_test_blitter__DOT__wait_blt_done__205__test_blitter__DOT__unnamedblk1_1__DOT____Vrepeat0 = 0;
    IData/*31:0*/ __Vtask_test_blitter__DOT__wait_blt_done__205__test_blitter__DOT__unnamedblk1_2__DOT____Vrepeat1;
    __Vtask_test_blitter__DOT__wait_blt_done__205__test_blitter__DOT__unnamedblk1_2__DOT____Vrepeat1 = 0;
    CData/*0:0*/ __Vtask_test_blitter__DOT__check__206__condition;
    __Vtask_test_blitter__DOT__check__206__condition = 0;
    CData/*0:0*/ __Vtask_test_blitter__DOT__check__207__condition;
    __Vtask_test_blitter__DOT__check__207__condition = 0;
    CData/*0:0*/ __Vtask_test_blitter__DOT__check__208__condition;
    __Vtask_test_blitter__DOT__check__208__condition = 0;
    CData/*0:0*/ __Vtask_test_blitter__DOT__check__209__condition;
    __Vtask_test_blitter__DOT__check__209__condition = 0;
    CData/*0:0*/ __Vtask_test_blitter__DOT__check__210__condition;
    __Vtask_test_blitter__DOT__check__210__condition = 0;
    CData/*0:0*/ __Vtask_test_blitter__DOT__check__211__condition;
    __Vtask_test_blitter__DOT__check__211__condition = 0;
    CData/*0:0*/ __Vtask_test_blitter__DOT__check__212__condition;
    __Vtask_test_blitter__DOT__check__212__condition = 0;
    IData/*31:0*/ __Vtask_test_blitter__DOT__setup_fill__213__dst_space;
    __Vtask_test_blitter__DOT__setup_fill__213__dst_space = 0;
    IData/*31:0*/ __Vtask_test_blitter__DOT__setup_fill__213__dst_addr;
    __Vtask_test_blitter__DOT__setup_fill__213__dst_addr = 0;
    IData/*31:0*/ __Vtask_test_blitter__DOT__setup_fill__213__dst_stride;
    __Vtask_test_blitter__DOT__setup_fill__213__dst_stride = 0;
    IData/*31:0*/ __Vtask_test_blitter__DOT__setup_fill__213__width;
    __Vtask_test_blitter__DOT__setup_fill__213__width = 0;
    IData/*31:0*/ __Vtask_test_blitter__DOT__setup_fill__213__height;
    __Vtask_test_blitter__DOT__setup_fill__213__height = 0;
    IData/*31:0*/ __Vtask_test_blitter__DOT__setup_fill__213__fill_val;
    __Vtask_test_blitter__DOT__setup_fill__213__fill_val = 0;
    IData/*31:0*/ __Vtask_test_blitter__DOT__blt_reg__214__off;
    __Vtask_test_blitter__DOT__blt_reg__214__off = 0;
    CData/*7:0*/ __Vtask_test_blitter__DOT__blt_reg__214__data;
    __Vtask_test_blitter__DOT__blt_reg__214__data = 0;
    SData/*15:0*/ __Vtask_test_blitter__DOT__write_reg__215__addr;
    __Vtask_test_blitter__DOT__write_reg__215__addr = 0;
    CData/*7:0*/ __Vtask_test_blitter__DOT__write_reg__215__data;
    __Vtask_test_blitter__DOT__write_reg__215__data = 0;
    IData/*31:0*/ __Vtask_test_blitter__DOT__blt_reg__216__off;
    __Vtask_test_blitter__DOT__blt_reg__216__off = 0;
    CData/*7:0*/ __Vtask_test_blitter__DOT__blt_reg__216__data;
    __Vtask_test_blitter__DOT__blt_reg__216__data = 0;
    SData/*15:0*/ __Vtask_test_blitter__DOT__write_reg__217__addr;
    __Vtask_test_blitter__DOT__write_reg__217__addr = 0;
    CData/*7:0*/ __Vtask_test_blitter__DOT__write_reg__217__data;
    __Vtask_test_blitter__DOT__write_reg__217__data = 0;
    IData/*31:0*/ __Vtask_test_blitter__DOT__blt_reg__218__off;
    __Vtask_test_blitter__DOT__blt_reg__218__off = 0;
    CData/*7:0*/ __Vtask_test_blitter__DOT__blt_reg__218__data;
    __Vtask_test_blitter__DOT__blt_reg__218__data = 0;
    SData/*15:0*/ __Vtask_test_blitter__DOT__write_reg__219__addr;
    __Vtask_test_blitter__DOT__write_reg__219__addr = 0;
    CData/*7:0*/ __Vtask_test_blitter__DOT__write_reg__219__data;
    __Vtask_test_blitter__DOT__write_reg__219__data = 0;
    IData/*31:0*/ __Vtask_test_blitter__DOT__blt_reg__220__off;
    __Vtask_test_blitter__DOT__blt_reg__220__off = 0;
    CData/*7:0*/ __Vtask_test_blitter__DOT__blt_reg__220__data;
    __Vtask_test_blitter__DOT__blt_reg__220__data = 0;
    SData/*15:0*/ __Vtask_test_blitter__DOT__write_reg__221__addr;
    __Vtask_test_blitter__DOT__write_reg__221__addr = 0;
    CData/*7:0*/ __Vtask_test_blitter__DOT__write_reg__221__data;
    __Vtask_test_blitter__DOT__write_reg__221__data = 0;
    IData/*31:0*/ __Vtask_test_blitter__DOT__blt_reg__222__off;
    __Vtask_test_blitter__DOT__blt_reg__222__off = 0;
    CData/*7:0*/ __Vtask_test_blitter__DOT__blt_reg__222__data;
    __Vtask_test_blitter__DOT__blt_reg__222__data = 0;
    SData/*15:0*/ __Vtask_test_blitter__DOT__write_reg__223__addr;
    __Vtask_test_blitter__DOT__write_reg__223__addr = 0;
    CData/*7:0*/ __Vtask_test_blitter__DOT__write_reg__223__data;
    __Vtask_test_blitter__DOT__write_reg__223__data = 0;
    IData/*31:0*/ __Vtask_test_blitter__DOT__blt_reg__224__off;
    __Vtask_test_blitter__DOT__blt_reg__224__off = 0;
    CData/*7:0*/ __Vtask_test_blitter__DOT__blt_reg__224__data;
    __Vtask_test_blitter__DOT__blt_reg__224__data = 0;
    SData/*15:0*/ __Vtask_test_blitter__DOT__write_reg__225__addr;
    __Vtask_test_blitter__DOT__write_reg__225__addr = 0;
    CData/*7:0*/ __Vtask_test_blitter__DOT__write_reg__225__data;
    __Vtask_test_blitter__DOT__write_reg__225__data = 0;
    IData/*31:0*/ __Vtask_test_blitter__DOT__blt_reg__226__off;
    __Vtask_test_blitter__DOT__blt_reg__226__off = 0;
    CData/*7:0*/ __Vtask_test_blitter__DOT__blt_reg__226__data;
    __Vtask_test_blitter__DOT__blt_reg__226__data = 0;
    SData/*15:0*/ __Vtask_test_blitter__DOT__write_reg__227__addr;
    __Vtask_test_blitter__DOT__write_reg__227__addr = 0;
    CData/*7:0*/ __Vtask_test_blitter__DOT__write_reg__227__data;
    __Vtask_test_blitter__DOT__write_reg__227__data = 0;
    IData/*31:0*/ __Vtask_test_blitter__DOT__blt_reg__228__off;
    __Vtask_test_blitter__DOT__blt_reg__228__off = 0;
    CData/*7:0*/ __Vtask_test_blitter__DOT__blt_reg__228__data;
    __Vtask_test_blitter__DOT__blt_reg__228__data = 0;
    SData/*15:0*/ __Vtask_test_blitter__DOT__write_reg__229__addr;
    __Vtask_test_blitter__DOT__write_reg__229__addr = 0;
    CData/*7:0*/ __Vtask_test_blitter__DOT__write_reg__229__data;
    __Vtask_test_blitter__DOT__write_reg__229__data = 0;
    IData/*31:0*/ __Vtask_test_blitter__DOT__blt_reg__230__off;
    __Vtask_test_blitter__DOT__blt_reg__230__off = 0;
    CData/*7:0*/ __Vtask_test_blitter__DOT__blt_reg__230__data;
    __Vtask_test_blitter__DOT__blt_reg__230__data = 0;
    SData/*15:0*/ __Vtask_test_blitter__DOT__write_reg__231__addr;
    __Vtask_test_blitter__DOT__write_reg__231__addr = 0;
    CData/*7:0*/ __Vtask_test_blitter__DOT__write_reg__231__data;
    __Vtask_test_blitter__DOT__write_reg__231__data = 0;
    IData/*31:0*/ __Vtask_test_blitter__DOT__blt_reg__232__off;
    __Vtask_test_blitter__DOT__blt_reg__232__off = 0;
    CData/*7:0*/ __Vtask_test_blitter__DOT__blt_reg__232__data;
    __Vtask_test_blitter__DOT__blt_reg__232__data = 0;
    SData/*15:0*/ __Vtask_test_blitter__DOT__write_reg__233__addr;
    __Vtask_test_blitter__DOT__write_reg__233__addr = 0;
    CData/*7:0*/ __Vtask_test_blitter__DOT__write_reg__233__data;
    __Vtask_test_blitter__DOT__write_reg__233__data = 0;
    IData/*31:0*/ __Vtask_test_blitter__DOT__blt_reg__234__off;
    __Vtask_test_blitter__DOT__blt_reg__234__off = 0;
    CData/*7:0*/ __Vtask_test_blitter__DOT__blt_reg__234__data;
    __Vtask_test_blitter__DOT__blt_reg__234__data = 0;
    SData/*15:0*/ __Vtask_test_blitter__DOT__write_reg__235__addr;
    __Vtask_test_blitter__DOT__write_reg__235__addr = 0;
    CData/*7:0*/ __Vtask_test_blitter__DOT__write_reg__235__data;
    __Vtask_test_blitter__DOT__write_reg__235__data = 0;
    IData/*31:0*/ __Vtask_test_blitter__DOT__blt_reg__236__off;
    __Vtask_test_blitter__DOT__blt_reg__236__off = 0;
    CData/*7:0*/ __Vtask_test_blitter__DOT__blt_reg__236__data;
    __Vtask_test_blitter__DOT__blt_reg__236__data = 0;
    SData/*15:0*/ __Vtask_test_blitter__DOT__write_reg__237__addr;
    __Vtask_test_blitter__DOT__write_reg__237__addr = 0;
    CData/*7:0*/ __Vtask_test_blitter__DOT__write_reg__237__data;
    __Vtask_test_blitter__DOT__write_reg__237__data = 0;
    IData/*31:0*/ __Vtask_test_blitter__DOT__blt_reg__239__off;
    __Vtask_test_blitter__DOT__blt_reg__239__off = 0;
    CData/*7:0*/ __Vtask_test_blitter__DOT__blt_reg__239__data;
    __Vtask_test_blitter__DOT__blt_reg__239__data = 0;
    SData/*15:0*/ __Vtask_test_blitter__DOT__write_reg__240__addr;
    __Vtask_test_blitter__DOT__write_reg__240__addr = 0;
    CData/*7:0*/ __Vtask_test_blitter__DOT__write_reg__240__data;
    __Vtask_test_blitter__DOT__write_reg__240__data = 0;
    CData/*0:0*/ __Vtask_test_blitter__DOT__check__241__condition;
    __Vtask_test_blitter__DOT__check__241__condition = 0;
    IData/*31:0*/ __Vtask_test_blitter__DOT__wait_blt_done__242__test_blitter__DOT__unnamedblk1_1__DOT____Vrepeat0;
    __Vtask_test_blitter__DOT__wait_blt_done__242__test_blitter__DOT__unnamedblk1_1__DOT____Vrepeat0 = 0;
    IData/*31:0*/ __Vtask_test_blitter__DOT__wait_blt_done__242__test_blitter__DOT__unnamedblk1_2__DOT____Vrepeat1;
    __Vtask_test_blitter__DOT__wait_blt_done__242__test_blitter__DOT__unnamedblk1_2__DOT____Vrepeat1 = 0;
    CData/*0:0*/ __Vtask_test_blitter__DOT__check__243__condition;
    __Vtask_test_blitter__DOT__check__243__condition = 0;
    IData/*31:0*/ __Vtask_test_blitter__DOT__blt_reg__244__off;
    __Vtask_test_blitter__DOT__blt_reg__244__off = 0;
    CData/*7:0*/ __Vtask_test_blitter__DOT__blt_reg__244__data;
    __Vtask_test_blitter__DOT__blt_reg__244__data = 0;
    SData/*15:0*/ __Vtask_test_blitter__DOT__write_reg__245__addr;
    __Vtask_test_blitter__DOT__write_reg__245__addr = 0;
    CData/*7:0*/ __Vtask_test_blitter__DOT__write_reg__245__data;
    __Vtask_test_blitter__DOT__write_reg__245__data = 0;
    CData/*0:0*/ __Vtask_test_blitter__DOT__check__246__condition;
    __Vtask_test_blitter__DOT__check__246__condition = 0;
    CData/*0:0*/ __Vtask_test_blitter__DOT__check__247__condition;
    __Vtask_test_blitter__DOT__check__247__condition = 0;
    IData/*31:0*/ __Vtask_test_blitter__DOT__blt_reg__248__off;
    __Vtask_test_blitter__DOT__blt_reg__248__off = 0;
    CData/*7:0*/ __Vtask_test_blitter__DOT__blt_reg__248__data;
    __Vtask_test_blitter__DOT__blt_reg__248__data = 0;
    SData/*15:0*/ __Vtask_test_blitter__DOT__write_reg__249__addr;
    __Vtask_test_blitter__DOT__write_reg__249__addr = 0;
    CData/*7:0*/ __Vtask_test_blitter__DOT__write_reg__249__data;
    __Vtask_test_blitter__DOT__write_reg__249__data = 0;
    IData/*31:0*/ __Vtask_test_blitter__DOT__blt_reg__250__off;
    __Vtask_test_blitter__DOT__blt_reg__250__off = 0;
    CData/*7:0*/ __Vtask_test_blitter__DOT__blt_reg__250__data;
    __Vtask_test_blitter__DOT__blt_reg__250__data = 0;
    SData/*15:0*/ __Vtask_test_blitter__DOT__write_reg__251__addr;
    __Vtask_test_blitter__DOT__write_reg__251__addr = 0;
    CData/*7:0*/ __Vtask_test_blitter__DOT__write_reg__251__data;
    __Vtask_test_blitter__DOT__write_reg__251__data = 0;
    IData/*31:0*/ __Vtask_test_blitter__DOT__blt_reg__252__off;
    __Vtask_test_blitter__DOT__blt_reg__252__off = 0;
    CData/*7:0*/ __Vtask_test_blitter__DOT__blt_reg__252__data;
    __Vtask_test_blitter__DOT__blt_reg__252__data = 0;
    SData/*15:0*/ __Vtask_test_blitter__DOT__write_reg__253__addr;
    __Vtask_test_blitter__DOT__write_reg__253__addr = 0;
    CData/*7:0*/ __Vtask_test_blitter__DOT__write_reg__253__data;
    __Vtask_test_blitter__DOT__write_reg__253__data = 0;
    IData/*31:0*/ __Vtask_test_blitter__DOT__blt_reg__254__off;
    __Vtask_test_blitter__DOT__blt_reg__254__off = 0;
    CData/*7:0*/ __Vtask_test_blitter__DOT__blt_reg__254__data;
    __Vtask_test_blitter__DOT__blt_reg__254__data = 0;
    SData/*15:0*/ __Vtask_test_blitter__DOT__write_reg__255__addr;
    __Vtask_test_blitter__DOT__write_reg__255__addr = 0;
    CData/*7:0*/ __Vtask_test_blitter__DOT__write_reg__255__data;
    __Vtask_test_blitter__DOT__write_reg__255__data = 0;
    IData/*31:0*/ __Vtask_test_blitter__DOT__blt_reg__256__off;
    __Vtask_test_blitter__DOT__blt_reg__256__off = 0;
    CData/*7:0*/ __Vtask_test_blitter__DOT__blt_reg__256__data;
    __Vtask_test_blitter__DOT__blt_reg__256__data = 0;
    SData/*15:0*/ __Vtask_test_blitter__DOT__write_reg__257__addr;
    __Vtask_test_blitter__DOT__write_reg__257__addr = 0;
    CData/*7:0*/ __Vtask_test_blitter__DOT__write_reg__257__data;
    __Vtask_test_blitter__DOT__write_reg__257__data = 0;
    IData/*31:0*/ __Vtask_test_blitter__DOT__blt_reg__259__off;
    __Vtask_test_blitter__DOT__blt_reg__259__off = 0;
    CData/*7:0*/ __Vtask_test_blitter__DOT__blt_reg__259__data;
    __Vtask_test_blitter__DOT__blt_reg__259__data = 0;
    SData/*15:0*/ __Vtask_test_blitter__DOT__write_reg__260__addr;
    __Vtask_test_blitter__DOT__write_reg__260__addr = 0;
    CData/*7:0*/ __Vtask_test_blitter__DOT__write_reg__260__data;
    __Vtask_test_blitter__DOT__write_reg__260__data = 0;
    CData/*0:0*/ __Vtask_test_blitter__DOT__check__261__condition;
    __Vtask_test_blitter__DOT__check__261__condition = 0;
    CData/*0:0*/ __Vtask_test_blitter__DOT__check__262__condition;
    __Vtask_test_blitter__DOT__check__262__condition = 0;
    IData/*31:0*/ __Vtask_test_blitter__DOT__blt_reg__263__off;
    __Vtask_test_blitter__DOT__blt_reg__263__off = 0;
    CData/*7:0*/ __Vtask_test_blitter__DOT__blt_reg__263__data;
    __Vtask_test_blitter__DOT__blt_reg__263__data = 0;
    SData/*15:0*/ __Vtask_test_blitter__DOT__write_reg__264__addr;
    __Vtask_test_blitter__DOT__write_reg__264__addr = 0;
    CData/*7:0*/ __Vtask_test_blitter__DOT__write_reg__264__data;
    __Vtask_test_blitter__DOT__write_reg__264__data = 0;
    IData/*31:0*/ __Vtask_test_blitter__DOT__blt_reg__265__off;
    __Vtask_test_blitter__DOT__blt_reg__265__off = 0;
    CData/*7:0*/ __Vtask_test_blitter__DOT__blt_reg__265__data;
    __Vtask_test_blitter__DOT__blt_reg__265__data = 0;
    SData/*15:0*/ __Vtask_test_blitter__DOT__write_reg__266__addr;
    __Vtask_test_blitter__DOT__write_reg__266__addr = 0;
    CData/*7:0*/ __Vtask_test_blitter__DOT__write_reg__266__data;
    __Vtask_test_blitter__DOT__write_reg__266__data = 0;
    IData/*31:0*/ __Vtask_test_blitter__DOT__blt_reg__267__off;
    __Vtask_test_blitter__DOT__blt_reg__267__off = 0;
    CData/*7:0*/ __Vtask_test_blitter__DOT__blt_reg__267__data;
    __Vtask_test_blitter__DOT__blt_reg__267__data = 0;
    SData/*15:0*/ __Vtask_test_blitter__DOT__write_reg__268__addr;
    __Vtask_test_blitter__DOT__write_reg__268__addr = 0;
    CData/*7:0*/ __Vtask_test_blitter__DOT__write_reg__268__data;
    __Vtask_test_blitter__DOT__write_reg__268__data = 0;
    IData/*31:0*/ __Vtask_test_blitter__DOT__blt_reg__269__off;
    __Vtask_test_blitter__DOT__blt_reg__269__off = 0;
    CData/*7:0*/ __Vtask_test_blitter__DOT__blt_reg__269__data;
    __Vtask_test_blitter__DOT__blt_reg__269__data = 0;
    SData/*15:0*/ __Vtask_test_blitter__DOT__write_reg__270__addr;
    __Vtask_test_blitter__DOT__write_reg__270__addr = 0;
    CData/*7:0*/ __Vtask_test_blitter__DOT__write_reg__270__data;
    __Vtask_test_blitter__DOT__write_reg__270__data = 0;
    IData/*31:0*/ __Vtask_test_blitter__DOT__blt_reg__272__off;
    __Vtask_test_blitter__DOT__blt_reg__272__off = 0;
    CData/*7:0*/ __Vtask_test_blitter__DOT__blt_reg__272__data;
    __Vtask_test_blitter__DOT__blt_reg__272__data = 0;
    SData/*15:0*/ __Vtask_test_blitter__DOT__write_reg__273__addr;
    __Vtask_test_blitter__DOT__write_reg__273__addr = 0;
    CData/*7:0*/ __Vtask_test_blitter__DOT__write_reg__273__data;
    __Vtask_test_blitter__DOT__write_reg__273__data = 0;
    CData/*0:0*/ __Vtask_test_blitter__DOT__check__274__condition;
    __Vtask_test_blitter__DOT__check__274__condition = 0;
    CData/*0:0*/ __Vtask_test_blitter__DOT__check__275__condition;
    __Vtask_test_blitter__DOT__check__275__condition = 0;
    IData/*31:0*/ __Vtask_test_blitter__DOT__blt_reg__276__off;
    __Vtask_test_blitter__DOT__blt_reg__276__off = 0;
    CData/*7:0*/ __Vtask_test_blitter__DOT__blt_reg__276__data;
    __Vtask_test_blitter__DOT__blt_reg__276__data = 0;
    SData/*15:0*/ __Vtask_test_blitter__DOT__write_reg__277__addr;
    __Vtask_test_blitter__DOT__write_reg__277__addr = 0;
    CData/*7:0*/ __Vtask_test_blitter__DOT__write_reg__277__data;
    __Vtask_test_blitter__DOT__write_reg__277__data = 0;
    IData/*31:0*/ __Vtask_test_blitter__DOT__blt_reg__278__off;
    __Vtask_test_blitter__DOT__blt_reg__278__off = 0;
    CData/*7:0*/ __Vtask_test_blitter__DOT__blt_reg__278__data;
    __Vtask_test_blitter__DOT__blt_reg__278__data = 0;
    SData/*15:0*/ __Vtask_test_blitter__DOT__write_reg__279__addr;
    __Vtask_test_blitter__DOT__write_reg__279__addr = 0;
    CData/*7:0*/ __Vtask_test_blitter__DOT__write_reg__279__data;
    __Vtask_test_blitter__DOT__write_reg__279__data = 0;
    IData/*31:0*/ __Vtask_test_blitter__DOT__blt_reg__280__off;
    __Vtask_test_blitter__DOT__blt_reg__280__off = 0;
    CData/*7:0*/ __Vtask_test_blitter__DOT__blt_reg__280__data;
    __Vtask_test_blitter__DOT__blt_reg__280__data = 0;
    SData/*15:0*/ __Vtask_test_blitter__DOT__write_reg__281__addr;
    __Vtask_test_blitter__DOT__write_reg__281__addr = 0;
    CData/*7:0*/ __Vtask_test_blitter__DOT__write_reg__281__data;
    __Vtask_test_blitter__DOT__write_reg__281__data = 0;
    IData/*31:0*/ __Vtask_test_blitter__DOT__blt_reg__282__off;
    __Vtask_test_blitter__DOT__blt_reg__282__off = 0;
    CData/*7:0*/ __Vtask_test_blitter__DOT__blt_reg__282__data;
    __Vtask_test_blitter__DOT__blt_reg__282__data = 0;
    SData/*15:0*/ __Vtask_test_blitter__DOT__write_reg__283__addr;
    __Vtask_test_blitter__DOT__write_reg__283__addr = 0;
    CData/*7:0*/ __Vtask_test_blitter__DOT__write_reg__283__data;
    __Vtask_test_blitter__DOT__write_reg__283__data = 0;
    IData/*31:0*/ __Vtask_test_blitter__DOT__blt_reg__284__off;
    __Vtask_test_blitter__DOT__blt_reg__284__off = 0;
    CData/*7:0*/ __Vtask_test_blitter__DOT__blt_reg__284__data;
    __Vtask_test_blitter__DOT__blt_reg__284__data = 0;
    SData/*15:0*/ __Vtask_test_blitter__DOT__write_reg__285__addr;
    __Vtask_test_blitter__DOT__write_reg__285__addr = 0;
    CData/*7:0*/ __Vtask_test_blitter__DOT__write_reg__285__data;
    __Vtask_test_blitter__DOT__write_reg__285__data = 0;
    IData/*31:0*/ __Vtask_test_blitter__DOT__blt_reg__286__off;
    __Vtask_test_blitter__DOT__blt_reg__286__off = 0;
    CData/*7:0*/ __Vtask_test_blitter__DOT__blt_reg__286__data;
    __Vtask_test_blitter__DOT__blt_reg__286__data = 0;
    SData/*15:0*/ __Vtask_test_blitter__DOT__write_reg__287__addr;
    __Vtask_test_blitter__DOT__write_reg__287__addr = 0;
    CData/*7:0*/ __Vtask_test_blitter__DOT__write_reg__287__data;
    __Vtask_test_blitter__DOT__write_reg__287__data = 0;
    IData/*31:0*/ __Vtask_test_blitter__DOT__blt_reg__289__off;
    __Vtask_test_blitter__DOT__blt_reg__289__off = 0;
    CData/*7:0*/ __Vtask_test_blitter__DOT__blt_reg__289__data;
    __Vtask_test_blitter__DOT__blt_reg__289__data = 0;
    SData/*15:0*/ __Vtask_test_blitter__DOT__write_reg__290__addr;
    __Vtask_test_blitter__DOT__write_reg__290__addr = 0;
    CData/*7:0*/ __Vtask_test_blitter__DOT__write_reg__290__data;
    __Vtask_test_blitter__DOT__write_reg__290__data = 0;
    IData/*31:0*/ __Vtask_test_blitter__DOT__wait_blt_done__291__test_blitter__DOT__unnamedblk1_1__DOT____Vrepeat0;
    __Vtask_test_blitter__DOT__wait_blt_done__291__test_blitter__DOT__unnamedblk1_1__DOT____Vrepeat0 = 0;
    IData/*31:0*/ __Vtask_test_blitter__DOT__wait_blt_done__291__test_blitter__DOT__unnamedblk1_2__DOT____Vrepeat1;
    __Vtask_test_blitter__DOT__wait_blt_done__291__test_blitter__DOT__unnamedblk1_2__DOT____Vrepeat1 = 0;
    CData/*0:0*/ __Vtask_test_blitter__DOT__check__292__condition;
    __Vtask_test_blitter__DOT__check__292__condition = 0;
    CData/*0:0*/ __Vtask_test_blitter__DOT__check__293__condition;
    __Vtask_test_blitter__DOT__check__293__condition = 0;
    IData/*31:0*/ __Vtask_test_blitter__DOT__setup_copy__294__src_space;
    __Vtask_test_blitter__DOT__setup_copy__294__src_space = 0;
    IData/*31:0*/ __Vtask_test_blitter__DOT__setup_copy__294__src_addr;
    __Vtask_test_blitter__DOT__setup_copy__294__src_addr = 0;
    IData/*31:0*/ __Vtask_test_blitter__DOT__setup_copy__294__src_stride;
    __Vtask_test_blitter__DOT__setup_copy__294__src_stride = 0;
    IData/*31:0*/ __Vtask_test_blitter__DOT__setup_copy__294__dst_space;
    __Vtask_test_blitter__DOT__setup_copy__294__dst_space = 0;
    IData/*31:0*/ __Vtask_test_blitter__DOT__setup_copy__294__dst_addr;
    __Vtask_test_blitter__DOT__setup_copy__294__dst_addr = 0;
    IData/*31:0*/ __Vtask_test_blitter__DOT__setup_copy__294__dst_stride;
    __Vtask_test_blitter__DOT__setup_copy__294__dst_stride = 0;
    IData/*31:0*/ __Vtask_test_blitter__DOT__setup_copy__294__width;
    __Vtask_test_blitter__DOT__setup_copy__294__width = 0;
    IData/*31:0*/ __Vtask_test_blitter__DOT__setup_copy__294__height;
    __Vtask_test_blitter__DOT__setup_copy__294__height = 0;
    IData/*31:0*/ __Vtask_test_blitter__DOT__blt_reg__295__off;
    __Vtask_test_blitter__DOT__blt_reg__295__off = 0;
    CData/*7:0*/ __Vtask_test_blitter__DOT__blt_reg__295__data;
    __Vtask_test_blitter__DOT__blt_reg__295__data = 0;
    SData/*15:0*/ __Vtask_test_blitter__DOT__write_reg__296__addr;
    __Vtask_test_blitter__DOT__write_reg__296__addr = 0;
    CData/*7:0*/ __Vtask_test_blitter__DOT__write_reg__296__data;
    __Vtask_test_blitter__DOT__write_reg__296__data = 0;
    IData/*31:0*/ __Vtask_test_blitter__DOT__blt_reg__297__off;
    __Vtask_test_blitter__DOT__blt_reg__297__off = 0;
    CData/*7:0*/ __Vtask_test_blitter__DOT__blt_reg__297__data;
    __Vtask_test_blitter__DOT__blt_reg__297__data = 0;
    SData/*15:0*/ __Vtask_test_blitter__DOT__write_reg__298__addr;
    __Vtask_test_blitter__DOT__write_reg__298__addr = 0;
    CData/*7:0*/ __Vtask_test_blitter__DOT__write_reg__298__data;
    __Vtask_test_blitter__DOT__write_reg__298__data = 0;
    IData/*31:0*/ __Vtask_test_blitter__DOT__blt_reg__299__off;
    __Vtask_test_blitter__DOT__blt_reg__299__off = 0;
    CData/*7:0*/ __Vtask_test_blitter__DOT__blt_reg__299__data;
    __Vtask_test_blitter__DOT__blt_reg__299__data = 0;
    SData/*15:0*/ __Vtask_test_blitter__DOT__write_reg__300__addr;
    __Vtask_test_blitter__DOT__write_reg__300__addr = 0;
    CData/*7:0*/ __Vtask_test_blitter__DOT__write_reg__300__data;
    __Vtask_test_blitter__DOT__write_reg__300__data = 0;
    IData/*31:0*/ __Vtask_test_blitter__DOT__blt_reg__301__off;
    __Vtask_test_blitter__DOT__blt_reg__301__off = 0;
    CData/*7:0*/ __Vtask_test_blitter__DOT__blt_reg__301__data;
    __Vtask_test_blitter__DOT__blt_reg__301__data = 0;
    SData/*15:0*/ __Vtask_test_blitter__DOT__write_reg__302__addr;
    __Vtask_test_blitter__DOT__write_reg__302__addr = 0;
    CData/*7:0*/ __Vtask_test_blitter__DOT__write_reg__302__data;
    __Vtask_test_blitter__DOT__write_reg__302__data = 0;
    IData/*31:0*/ __Vtask_test_blitter__DOT__blt_reg__303__off;
    __Vtask_test_blitter__DOT__blt_reg__303__off = 0;
    CData/*7:0*/ __Vtask_test_blitter__DOT__blt_reg__303__data;
    __Vtask_test_blitter__DOT__blt_reg__303__data = 0;
    SData/*15:0*/ __Vtask_test_blitter__DOT__write_reg__304__addr;
    __Vtask_test_blitter__DOT__write_reg__304__addr = 0;
    CData/*7:0*/ __Vtask_test_blitter__DOT__write_reg__304__data;
    __Vtask_test_blitter__DOT__write_reg__304__data = 0;
    IData/*31:0*/ __Vtask_test_blitter__DOT__blt_reg__305__off;
    __Vtask_test_blitter__DOT__blt_reg__305__off = 0;
    CData/*7:0*/ __Vtask_test_blitter__DOT__blt_reg__305__data;
    __Vtask_test_blitter__DOT__blt_reg__305__data = 0;
    SData/*15:0*/ __Vtask_test_blitter__DOT__write_reg__306__addr;
    __Vtask_test_blitter__DOT__write_reg__306__addr = 0;
    CData/*7:0*/ __Vtask_test_blitter__DOT__write_reg__306__data;
    __Vtask_test_blitter__DOT__write_reg__306__data = 0;
    IData/*31:0*/ __Vtask_test_blitter__DOT__blt_reg__307__off;
    __Vtask_test_blitter__DOT__blt_reg__307__off = 0;
    CData/*7:0*/ __Vtask_test_blitter__DOT__blt_reg__307__data;
    __Vtask_test_blitter__DOT__blt_reg__307__data = 0;
    SData/*15:0*/ __Vtask_test_blitter__DOT__write_reg__308__addr;
    __Vtask_test_blitter__DOT__write_reg__308__addr = 0;
    CData/*7:0*/ __Vtask_test_blitter__DOT__write_reg__308__data;
    __Vtask_test_blitter__DOT__write_reg__308__data = 0;
    IData/*31:0*/ __Vtask_test_blitter__DOT__blt_reg__309__off;
    __Vtask_test_blitter__DOT__blt_reg__309__off = 0;
    CData/*7:0*/ __Vtask_test_blitter__DOT__blt_reg__309__data;
    __Vtask_test_blitter__DOT__blt_reg__309__data = 0;
    SData/*15:0*/ __Vtask_test_blitter__DOT__write_reg__310__addr;
    __Vtask_test_blitter__DOT__write_reg__310__addr = 0;
    CData/*7:0*/ __Vtask_test_blitter__DOT__write_reg__310__data;
    __Vtask_test_blitter__DOT__write_reg__310__data = 0;
    IData/*31:0*/ __Vtask_test_blitter__DOT__blt_reg__311__off;
    __Vtask_test_blitter__DOT__blt_reg__311__off = 0;
    CData/*7:0*/ __Vtask_test_blitter__DOT__blt_reg__311__data;
    __Vtask_test_blitter__DOT__blt_reg__311__data = 0;
    SData/*15:0*/ __Vtask_test_blitter__DOT__write_reg__312__addr;
    __Vtask_test_blitter__DOT__write_reg__312__addr = 0;
    CData/*7:0*/ __Vtask_test_blitter__DOT__write_reg__312__data;
    __Vtask_test_blitter__DOT__write_reg__312__data = 0;
    IData/*31:0*/ __Vtask_test_blitter__DOT__blt_reg__313__off;
    __Vtask_test_blitter__DOT__blt_reg__313__off = 0;
    CData/*7:0*/ __Vtask_test_blitter__DOT__blt_reg__313__data;
    __Vtask_test_blitter__DOT__blt_reg__313__data = 0;
    SData/*15:0*/ __Vtask_test_blitter__DOT__write_reg__314__addr;
    __Vtask_test_blitter__DOT__write_reg__314__addr = 0;
    CData/*7:0*/ __Vtask_test_blitter__DOT__write_reg__314__data;
    __Vtask_test_blitter__DOT__write_reg__314__data = 0;
    IData/*31:0*/ __Vtask_test_blitter__DOT__blt_reg__315__off;
    __Vtask_test_blitter__DOT__blt_reg__315__off = 0;
    CData/*7:0*/ __Vtask_test_blitter__DOT__blt_reg__315__data;
    __Vtask_test_blitter__DOT__blt_reg__315__data = 0;
    SData/*15:0*/ __Vtask_test_blitter__DOT__write_reg__316__addr;
    __Vtask_test_blitter__DOT__write_reg__316__addr = 0;
    CData/*7:0*/ __Vtask_test_blitter__DOT__write_reg__316__data;
    __Vtask_test_blitter__DOT__write_reg__316__data = 0;
    IData/*31:0*/ __Vtask_test_blitter__DOT__blt_reg__317__off;
    __Vtask_test_blitter__DOT__blt_reg__317__off = 0;
    CData/*7:0*/ __Vtask_test_blitter__DOT__blt_reg__317__data;
    __Vtask_test_blitter__DOT__blt_reg__317__data = 0;
    SData/*15:0*/ __Vtask_test_blitter__DOT__write_reg__318__addr;
    __Vtask_test_blitter__DOT__write_reg__318__addr = 0;
    CData/*7:0*/ __Vtask_test_blitter__DOT__write_reg__318__data;
    __Vtask_test_blitter__DOT__write_reg__318__data = 0;
    IData/*31:0*/ __Vtask_test_blitter__DOT__blt_reg__319__off;
    __Vtask_test_blitter__DOT__blt_reg__319__off = 0;
    CData/*7:0*/ __Vtask_test_blitter__DOT__blt_reg__319__data;
    __Vtask_test_blitter__DOT__blt_reg__319__data = 0;
    SData/*15:0*/ __Vtask_test_blitter__DOT__write_reg__320__addr;
    __Vtask_test_blitter__DOT__write_reg__320__addr = 0;
    CData/*7:0*/ __Vtask_test_blitter__DOT__write_reg__320__data;
    __Vtask_test_blitter__DOT__write_reg__320__data = 0;
    IData/*31:0*/ __Vtask_test_blitter__DOT__blt_reg__321__off;
    __Vtask_test_blitter__DOT__blt_reg__321__off = 0;
    CData/*7:0*/ __Vtask_test_blitter__DOT__blt_reg__321__data;
    __Vtask_test_blitter__DOT__blt_reg__321__data = 0;
    SData/*15:0*/ __Vtask_test_blitter__DOT__write_reg__322__addr;
    __Vtask_test_blitter__DOT__write_reg__322__addr = 0;
    CData/*7:0*/ __Vtask_test_blitter__DOT__write_reg__322__data;
    __Vtask_test_blitter__DOT__write_reg__322__data = 0;
    IData/*31:0*/ __Vtask_test_blitter__DOT__blt_reg__323__off;
    __Vtask_test_blitter__DOT__blt_reg__323__off = 0;
    CData/*7:0*/ __Vtask_test_blitter__DOT__blt_reg__323__data;
    __Vtask_test_blitter__DOT__blt_reg__323__data = 0;
    SData/*15:0*/ __Vtask_test_blitter__DOT__write_reg__324__addr;
    __Vtask_test_blitter__DOT__write_reg__324__addr = 0;
    CData/*7:0*/ __Vtask_test_blitter__DOT__write_reg__324__data;
    __Vtask_test_blitter__DOT__write_reg__324__data = 0;
    IData/*31:0*/ __Vtask_test_blitter__DOT__blt_reg__325__off;
    __Vtask_test_blitter__DOT__blt_reg__325__off = 0;
    CData/*7:0*/ __Vtask_test_blitter__DOT__blt_reg__325__data;
    __Vtask_test_blitter__DOT__blt_reg__325__data = 0;
    SData/*15:0*/ __Vtask_test_blitter__DOT__write_reg__326__addr;
    __Vtask_test_blitter__DOT__write_reg__326__addr = 0;
    CData/*7:0*/ __Vtask_test_blitter__DOT__write_reg__326__data;
    __Vtask_test_blitter__DOT__write_reg__326__data = 0;
    IData/*31:0*/ __Vtask_test_blitter__DOT__blt_reg__327__off;
    __Vtask_test_blitter__DOT__blt_reg__327__off = 0;
    CData/*7:0*/ __Vtask_test_blitter__DOT__blt_reg__327__data;
    __Vtask_test_blitter__DOT__blt_reg__327__data = 0;
    SData/*15:0*/ __Vtask_test_blitter__DOT__write_reg__328__addr;
    __Vtask_test_blitter__DOT__write_reg__328__addr = 0;
    CData/*7:0*/ __Vtask_test_blitter__DOT__write_reg__328__data;
    __Vtask_test_blitter__DOT__write_reg__328__data = 0;
    IData/*31:0*/ __Vtask_test_blitter__DOT__blt_reg__330__off;
    __Vtask_test_blitter__DOT__blt_reg__330__off = 0;
    CData/*7:0*/ __Vtask_test_blitter__DOT__blt_reg__330__data;
    __Vtask_test_blitter__DOT__blt_reg__330__data = 0;
    SData/*15:0*/ __Vtask_test_blitter__DOT__write_reg__331__addr;
    __Vtask_test_blitter__DOT__write_reg__331__addr = 0;
    CData/*7:0*/ __Vtask_test_blitter__DOT__write_reg__331__data;
    __Vtask_test_blitter__DOT__write_reg__331__data = 0;
    IData/*31:0*/ __Vtask_test_blitter__DOT__wait_blt_done__332__test_blitter__DOT__unnamedblk1_1__DOT____Vrepeat0;
    __Vtask_test_blitter__DOT__wait_blt_done__332__test_blitter__DOT__unnamedblk1_1__DOT____Vrepeat0 = 0;
    IData/*31:0*/ __Vtask_test_blitter__DOT__wait_blt_done__332__test_blitter__DOT__unnamedblk1_2__DOT____Vrepeat1;
    __Vtask_test_blitter__DOT__wait_blt_done__332__test_blitter__DOT__unnamedblk1_2__DOT____Vrepeat1 = 0;
    CData/*0:0*/ __Vtask_test_blitter__DOT__check__333__condition;
    __Vtask_test_blitter__DOT__check__333__condition = 0;
    CData/*0:0*/ __Vtask_test_blitter__DOT__check__334__condition;
    __Vtask_test_blitter__DOT__check__334__condition = 0;
    CData/*0:0*/ __Vtask_test_blitter__DOT__check__335__condition;
    __Vtask_test_blitter__DOT__check__335__condition = 0;
    CData/*0:0*/ __Vtask_test_blitter__DOT__check__336__condition;
    __Vtask_test_blitter__DOT__check__336__condition = 0;
    CData/*0:0*/ __Vtask_test_blitter__DOT__check__337__condition;
    __Vtask_test_blitter__DOT__check__337__condition = 0;
    CData/*0:0*/ __Vtask_test_blitter__DOT__check__338__condition;
    __Vtask_test_blitter__DOT__check__338__condition = 0;
    IData/*31:0*/ __Vtask_test_blitter__DOT__setup_fill__339__dst_space;
    __Vtask_test_blitter__DOT__setup_fill__339__dst_space = 0;
    IData/*31:0*/ __Vtask_test_blitter__DOT__setup_fill__339__dst_addr;
    __Vtask_test_blitter__DOT__setup_fill__339__dst_addr = 0;
    IData/*31:0*/ __Vtask_test_blitter__DOT__setup_fill__339__dst_stride;
    __Vtask_test_blitter__DOT__setup_fill__339__dst_stride = 0;
    IData/*31:0*/ __Vtask_test_blitter__DOT__setup_fill__339__width;
    __Vtask_test_blitter__DOT__setup_fill__339__width = 0;
    IData/*31:0*/ __Vtask_test_blitter__DOT__setup_fill__339__height;
    __Vtask_test_blitter__DOT__setup_fill__339__height = 0;
    IData/*31:0*/ __Vtask_test_blitter__DOT__setup_fill__339__fill_val;
    __Vtask_test_blitter__DOT__setup_fill__339__fill_val = 0;
    IData/*31:0*/ __Vtask_test_blitter__DOT__blt_reg__340__off;
    __Vtask_test_blitter__DOT__blt_reg__340__off = 0;
    CData/*7:0*/ __Vtask_test_blitter__DOT__blt_reg__340__data;
    __Vtask_test_blitter__DOT__blt_reg__340__data = 0;
    SData/*15:0*/ __Vtask_test_blitter__DOT__write_reg__341__addr;
    __Vtask_test_blitter__DOT__write_reg__341__addr = 0;
    CData/*7:0*/ __Vtask_test_blitter__DOT__write_reg__341__data;
    __Vtask_test_blitter__DOT__write_reg__341__data = 0;
    IData/*31:0*/ __Vtask_test_blitter__DOT__blt_reg__342__off;
    __Vtask_test_blitter__DOT__blt_reg__342__off = 0;
    CData/*7:0*/ __Vtask_test_blitter__DOT__blt_reg__342__data;
    __Vtask_test_blitter__DOT__blt_reg__342__data = 0;
    SData/*15:0*/ __Vtask_test_blitter__DOT__write_reg__343__addr;
    __Vtask_test_blitter__DOT__write_reg__343__addr = 0;
    CData/*7:0*/ __Vtask_test_blitter__DOT__write_reg__343__data;
    __Vtask_test_blitter__DOT__write_reg__343__data = 0;
    IData/*31:0*/ __Vtask_test_blitter__DOT__blt_reg__344__off;
    __Vtask_test_blitter__DOT__blt_reg__344__off = 0;
    CData/*7:0*/ __Vtask_test_blitter__DOT__blt_reg__344__data;
    __Vtask_test_blitter__DOT__blt_reg__344__data = 0;
    SData/*15:0*/ __Vtask_test_blitter__DOT__write_reg__345__addr;
    __Vtask_test_blitter__DOT__write_reg__345__addr = 0;
    CData/*7:0*/ __Vtask_test_blitter__DOT__write_reg__345__data;
    __Vtask_test_blitter__DOT__write_reg__345__data = 0;
    IData/*31:0*/ __Vtask_test_blitter__DOT__blt_reg__346__off;
    __Vtask_test_blitter__DOT__blt_reg__346__off = 0;
    CData/*7:0*/ __Vtask_test_blitter__DOT__blt_reg__346__data;
    __Vtask_test_blitter__DOT__blt_reg__346__data = 0;
    SData/*15:0*/ __Vtask_test_blitter__DOT__write_reg__347__addr;
    __Vtask_test_blitter__DOT__write_reg__347__addr = 0;
    CData/*7:0*/ __Vtask_test_blitter__DOT__write_reg__347__data;
    __Vtask_test_blitter__DOT__write_reg__347__data = 0;
    IData/*31:0*/ __Vtask_test_blitter__DOT__blt_reg__348__off;
    __Vtask_test_blitter__DOT__blt_reg__348__off = 0;
    CData/*7:0*/ __Vtask_test_blitter__DOT__blt_reg__348__data;
    __Vtask_test_blitter__DOT__blt_reg__348__data = 0;
    SData/*15:0*/ __Vtask_test_blitter__DOT__write_reg__349__addr;
    __Vtask_test_blitter__DOT__write_reg__349__addr = 0;
    CData/*7:0*/ __Vtask_test_blitter__DOT__write_reg__349__data;
    __Vtask_test_blitter__DOT__write_reg__349__data = 0;
    IData/*31:0*/ __Vtask_test_blitter__DOT__blt_reg__350__off;
    __Vtask_test_blitter__DOT__blt_reg__350__off = 0;
    CData/*7:0*/ __Vtask_test_blitter__DOT__blt_reg__350__data;
    __Vtask_test_blitter__DOT__blt_reg__350__data = 0;
    SData/*15:0*/ __Vtask_test_blitter__DOT__write_reg__351__addr;
    __Vtask_test_blitter__DOT__write_reg__351__addr = 0;
    CData/*7:0*/ __Vtask_test_blitter__DOT__write_reg__351__data;
    __Vtask_test_blitter__DOT__write_reg__351__data = 0;
    IData/*31:0*/ __Vtask_test_blitter__DOT__blt_reg__352__off;
    __Vtask_test_blitter__DOT__blt_reg__352__off = 0;
    CData/*7:0*/ __Vtask_test_blitter__DOT__blt_reg__352__data;
    __Vtask_test_blitter__DOT__blt_reg__352__data = 0;
    SData/*15:0*/ __Vtask_test_blitter__DOT__write_reg__353__addr;
    __Vtask_test_blitter__DOT__write_reg__353__addr = 0;
    CData/*7:0*/ __Vtask_test_blitter__DOT__write_reg__353__data;
    __Vtask_test_blitter__DOT__write_reg__353__data = 0;
    IData/*31:0*/ __Vtask_test_blitter__DOT__blt_reg__354__off;
    __Vtask_test_blitter__DOT__blt_reg__354__off = 0;
    CData/*7:0*/ __Vtask_test_blitter__DOT__blt_reg__354__data;
    __Vtask_test_blitter__DOT__blt_reg__354__data = 0;
    SData/*15:0*/ __Vtask_test_blitter__DOT__write_reg__355__addr;
    __Vtask_test_blitter__DOT__write_reg__355__addr = 0;
    CData/*7:0*/ __Vtask_test_blitter__DOT__write_reg__355__data;
    __Vtask_test_blitter__DOT__write_reg__355__data = 0;
    IData/*31:0*/ __Vtask_test_blitter__DOT__blt_reg__356__off;
    __Vtask_test_blitter__DOT__blt_reg__356__off = 0;
    CData/*7:0*/ __Vtask_test_blitter__DOT__blt_reg__356__data;
    __Vtask_test_blitter__DOT__blt_reg__356__data = 0;
    SData/*15:0*/ __Vtask_test_blitter__DOT__write_reg__357__addr;
    __Vtask_test_blitter__DOT__write_reg__357__addr = 0;
    CData/*7:0*/ __Vtask_test_blitter__DOT__write_reg__357__data;
    __Vtask_test_blitter__DOT__write_reg__357__data = 0;
    IData/*31:0*/ __Vtask_test_blitter__DOT__blt_reg__358__off;
    __Vtask_test_blitter__DOT__blt_reg__358__off = 0;
    CData/*7:0*/ __Vtask_test_blitter__DOT__blt_reg__358__data;
    __Vtask_test_blitter__DOT__blt_reg__358__data = 0;
    SData/*15:0*/ __Vtask_test_blitter__DOT__write_reg__359__addr;
    __Vtask_test_blitter__DOT__write_reg__359__addr = 0;
    CData/*7:0*/ __Vtask_test_blitter__DOT__write_reg__359__data;
    __Vtask_test_blitter__DOT__write_reg__359__data = 0;
    IData/*31:0*/ __Vtask_test_blitter__DOT__blt_reg__360__off;
    __Vtask_test_blitter__DOT__blt_reg__360__off = 0;
    CData/*7:0*/ __Vtask_test_blitter__DOT__blt_reg__360__data;
    __Vtask_test_blitter__DOT__blt_reg__360__data = 0;
    SData/*15:0*/ __Vtask_test_blitter__DOT__write_reg__361__addr;
    __Vtask_test_blitter__DOT__write_reg__361__addr = 0;
    CData/*7:0*/ __Vtask_test_blitter__DOT__write_reg__361__data;
    __Vtask_test_blitter__DOT__write_reg__361__data = 0;
    IData/*31:0*/ __Vtask_test_blitter__DOT__blt_reg__362__off;
    __Vtask_test_blitter__DOT__blt_reg__362__off = 0;
    CData/*7:0*/ __Vtask_test_blitter__DOT__blt_reg__362__data;
    __Vtask_test_blitter__DOT__blt_reg__362__data = 0;
    SData/*15:0*/ __Vtask_test_blitter__DOT__write_reg__363__addr;
    __Vtask_test_blitter__DOT__write_reg__363__addr = 0;
    CData/*7:0*/ __Vtask_test_blitter__DOT__write_reg__363__data;
    __Vtask_test_blitter__DOT__write_reg__363__data = 0;
    IData/*31:0*/ __Vtask_test_blitter__DOT__blt_reg__365__off;
    __Vtask_test_blitter__DOT__blt_reg__365__off = 0;
    CData/*7:0*/ __Vtask_test_blitter__DOT__blt_reg__365__data;
    __Vtask_test_blitter__DOT__blt_reg__365__data = 0;
    SData/*15:0*/ __Vtask_test_blitter__DOT__write_reg__366__addr;
    __Vtask_test_blitter__DOT__write_reg__366__addr = 0;
    CData/*7:0*/ __Vtask_test_blitter__DOT__write_reg__366__data;
    __Vtask_test_blitter__DOT__write_reg__366__data = 0;
    IData/*31:0*/ __Vtask_test_blitter__DOT__wait_blt_done__367__test_blitter__DOT__unnamedblk1_1__DOT____Vrepeat0;
    __Vtask_test_blitter__DOT__wait_blt_done__367__test_blitter__DOT__unnamedblk1_1__DOT____Vrepeat0 = 0;
    IData/*31:0*/ __Vtask_test_blitter__DOT__wait_blt_done__367__test_blitter__DOT__unnamedblk1_2__DOT____Vrepeat1;
    __Vtask_test_blitter__DOT__wait_blt_done__367__test_blitter__DOT__unnamedblk1_2__DOT____Vrepeat1 = 0;
    CData/*0:0*/ __Vtask_test_blitter__DOT__check__368__condition;
    __Vtask_test_blitter__DOT__check__368__condition = 0;
    CData/*0:0*/ __Vtask_test_blitter__DOT__check__369__condition;
    __Vtask_test_blitter__DOT__check__369__condition = 0;
    CData/*0:0*/ __Vtask_test_blitter__DOT__check__370__condition;
    __Vtask_test_blitter__DOT__check__370__condition = 0;
    CData/*0:0*/ __Vtask_test_blitter__DOT__check__371__condition;
    __Vtask_test_blitter__DOT__check__371__condition = 0;
    IData/*31:0*/ __Vtask_test_blitter__DOT__setup_copy__372__src_space;
    __Vtask_test_blitter__DOT__setup_copy__372__src_space = 0;
    IData/*31:0*/ __Vtask_test_blitter__DOT__setup_copy__372__src_addr;
    __Vtask_test_blitter__DOT__setup_copy__372__src_addr = 0;
    IData/*31:0*/ __Vtask_test_blitter__DOT__setup_copy__372__src_stride;
    __Vtask_test_blitter__DOT__setup_copy__372__src_stride = 0;
    IData/*31:0*/ __Vtask_test_blitter__DOT__setup_copy__372__dst_space;
    __Vtask_test_blitter__DOT__setup_copy__372__dst_space = 0;
    IData/*31:0*/ __Vtask_test_blitter__DOT__setup_copy__372__dst_addr;
    __Vtask_test_blitter__DOT__setup_copy__372__dst_addr = 0;
    IData/*31:0*/ __Vtask_test_blitter__DOT__setup_copy__372__dst_stride;
    __Vtask_test_blitter__DOT__setup_copy__372__dst_stride = 0;
    IData/*31:0*/ __Vtask_test_blitter__DOT__setup_copy__372__width;
    __Vtask_test_blitter__DOT__setup_copy__372__width = 0;
    IData/*31:0*/ __Vtask_test_blitter__DOT__setup_copy__372__height;
    __Vtask_test_blitter__DOT__setup_copy__372__height = 0;
    IData/*31:0*/ __Vtask_test_blitter__DOT__blt_reg__373__off;
    __Vtask_test_blitter__DOT__blt_reg__373__off = 0;
    CData/*7:0*/ __Vtask_test_blitter__DOT__blt_reg__373__data;
    __Vtask_test_blitter__DOT__blt_reg__373__data = 0;
    SData/*15:0*/ __Vtask_test_blitter__DOT__write_reg__374__addr;
    __Vtask_test_blitter__DOT__write_reg__374__addr = 0;
    CData/*7:0*/ __Vtask_test_blitter__DOT__write_reg__374__data;
    __Vtask_test_blitter__DOT__write_reg__374__data = 0;
    IData/*31:0*/ __Vtask_test_blitter__DOT__blt_reg__375__off;
    __Vtask_test_blitter__DOT__blt_reg__375__off = 0;
    CData/*7:0*/ __Vtask_test_blitter__DOT__blt_reg__375__data;
    __Vtask_test_blitter__DOT__blt_reg__375__data = 0;
    SData/*15:0*/ __Vtask_test_blitter__DOT__write_reg__376__addr;
    __Vtask_test_blitter__DOT__write_reg__376__addr = 0;
    CData/*7:0*/ __Vtask_test_blitter__DOT__write_reg__376__data;
    __Vtask_test_blitter__DOT__write_reg__376__data = 0;
    IData/*31:0*/ __Vtask_test_blitter__DOT__blt_reg__377__off;
    __Vtask_test_blitter__DOT__blt_reg__377__off = 0;
    CData/*7:0*/ __Vtask_test_blitter__DOT__blt_reg__377__data;
    __Vtask_test_blitter__DOT__blt_reg__377__data = 0;
    SData/*15:0*/ __Vtask_test_blitter__DOT__write_reg__378__addr;
    __Vtask_test_blitter__DOT__write_reg__378__addr = 0;
    CData/*7:0*/ __Vtask_test_blitter__DOT__write_reg__378__data;
    __Vtask_test_blitter__DOT__write_reg__378__data = 0;
    IData/*31:0*/ __Vtask_test_blitter__DOT__blt_reg__379__off;
    __Vtask_test_blitter__DOT__blt_reg__379__off = 0;
    CData/*7:0*/ __Vtask_test_blitter__DOT__blt_reg__379__data;
    __Vtask_test_blitter__DOT__blt_reg__379__data = 0;
    SData/*15:0*/ __Vtask_test_blitter__DOT__write_reg__380__addr;
    __Vtask_test_blitter__DOT__write_reg__380__addr = 0;
    CData/*7:0*/ __Vtask_test_blitter__DOT__write_reg__380__data;
    __Vtask_test_blitter__DOT__write_reg__380__data = 0;
    IData/*31:0*/ __Vtask_test_blitter__DOT__blt_reg__381__off;
    __Vtask_test_blitter__DOT__blt_reg__381__off = 0;
    CData/*7:0*/ __Vtask_test_blitter__DOT__blt_reg__381__data;
    __Vtask_test_blitter__DOT__blt_reg__381__data = 0;
    SData/*15:0*/ __Vtask_test_blitter__DOT__write_reg__382__addr;
    __Vtask_test_blitter__DOT__write_reg__382__addr = 0;
    CData/*7:0*/ __Vtask_test_blitter__DOT__write_reg__382__data;
    __Vtask_test_blitter__DOT__write_reg__382__data = 0;
    IData/*31:0*/ __Vtask_test_blitter__DOT__blt_reg__383__off;
    __Vtask_test_blitter__DOT__blt_reg__383__off = 0;
    CData/*7:0*/ __Vtask_test_blitter__DOT__blt_reg__383__data;
    __Vtask_test_blitter__DOT__blt_reg__383__data = 0;
    SData/*15:0*/ __Vtask_test_blitter__DOT__write_reg__384__addr;
    __Vtask_test_blitter__DOT__write_reg__384__addr = 0;
    CData/*7:0*/ __Vtask_test_blitter__DOT__write_reg__384__data;
    __Vtask_test_blitter__DOT__write_reg__384__data = 0;
    IData/*31:0*/ __Vtask_test_blitter__DOT__blt_reg__385__off;
    __Vtask_test_blitter__DOT__blt_reg__385__off = 0;
    CData/*7:0*/ __Vtask_test_blitter__DOT__blt_reg__385__data;
    __Vtask_test_blitter__DOT__blt_reg__385__data = 0;
    SData/*15:0*/ __Vtask_test_blitter__DOT__write_reg__386__addr;
    __Vtask_test_blitter__DOT__write_reg__386__addr = 0;
    CData/*7:0*/ __Vtask_test_blitter__DOT__write_reg__386__data;
    __Vtask_test_blitter__DOT__write_reg__386__data = 0;
    IData/*31:0*/ __Vtask_test_blitter__DOT__blt_reg__387__off;
    __Vtask_test_blitter__DOT__blt_reg__387__off = 0;
    CData/*7:0*/ __Vtask_test_blitter__DOT__blt_reg__387__data;
    __Vtask_test_blitter__DOT__blt_reg__387__data = 0;
    SData/*15:0*/ __Vtask_test_blitter__DOT__write_reg__388__addr;
    __Vtask_test_blitter__DOT__write_reg__388__addr = 0;
    CData/*7:0*/ __Vtask_test_blitter__DOT__write_reg__388__data;
    __Vtask_test_blitter__DOT__write_reg__388__data = 0;
    IData/*31:0*/ __Vtask_test_blitter__DOT__blt_reg__389__off;
    __Vtask_test_blitter__DOT__blt_reg__389__off = 0;
    CData/*7:0*/ __Vtask_test_blitter__DOT__blt_reg__389__data;
    __Vtask_test_blitter__DOT__blt_reg__389__data = 0;
    SData/*15:0*/ __Vtask_test_blitter__DOT__write_reg__390__addr;
    __Vtask_test_blitter__DOT__write_reg__390__addr = 0;
    CData/*7:0*/ __Vtask_test_blitter__DOT__write_reg__390__data;
    __Vtask_test_blitter__DOT__write_reg__390__data = 0;
    IData/*31:0*/ __Vtask_test_blitter__DOT__blt_reg__391__off;
    __Vtask_test_blitter__DOT__blt_reg__391__off = 0;
    CData/*7:0*/ __Vtask_test_blitter__DOT__blt_reg__391__data;
    __Vtask_test_blitter__DOT__blt_reg__391__data = 0;
    SData/*15:0*/ __Vtask_test_blitter__DOT__write_reg__392__addr;
    __Vtask_test_blitter__DOT__write_reg__392__addr = 0;
    CData/*7:0*/ __Vtask_test_blitter__DOT__write_reg__392__data;
    __Vtask_test_blitter__DOT__write_reg__392__data = 0;
    IData/*31:0*/ __Vtask_test_blitter__DOT__blt_reg__393__off;
    __Vtask_test_blitter__DOT__blt_reg__393__off = 0;
    CData/*7:0*/ __Vtask_test_blitter__DOT__blt_reg__393__data;
    __Vtask_test_blitter__DOT__blt_reg__393__data = 0;
    SData/*15:0*/ __Vtask_test_blitter__DOT__write_reg__394__addr;
    __Vtask_test_blitter__DOT__write_reg__394__addr = 0;
    CData/*7:0*/ __Vtask_test_blitter__DOT__write_reg__394__data;
    __Vtask_test_blitter__DOT__write_reg__394__data = 0;
    IData/*31:0*/ __Vtask_test_blitter__DOT__blt_reg__395__off;
    __Vtask_test_blitter__DOT__blt_reg__395__off = 0;
    CData/*7:0*/ __Vtask_test_blitter__DOT__blt_reg__395__data;
    __Vtask_test_blitter__DOT__blt_reg__395__data = 0;
    SData/*15:0*/ __Vtask_test_blitter__DOT__write_reg__396__addr;
    __Vtask_test_blitter__DOT__write_reg__396__addr = 0;
    CData/*7:0*/ __Vtask_test_blitter__DOT__write_reg__396__data;
    __Vtask_test_blitter__DOT__write_reg__396__data = 0;
    IData/*31:0*/ __Vtask_test_blitter__DOT__blt_reg__397__off;
    __Vtask_test_blitter__DOT__blt_reg__397__off = 0;
    CData/*7:0*/ __Vtask_test_blitter__DOT__blt_reg__397__data;
    __Vtask_test_blitter__DOT__blt_reg__397__data = 0;
    SData/*15:0*/ __Vtask_test_blitter__DOT__write_reg__398__addr;
    __Vtask_test_blitter__DOT__write_reg__398__addr = 0;
    CData/*7:0*/ __Vtask_test_blitter__DOT__write_reg__398__data;
    __Vtask_test_blitter__DOT__write_reg__398__data = 0;
    IData/*31:0*/ __Vtask_test_blitter__DOT__blt_reg__399__off;
    __Vtask_test_blitter__DOT__blt_reg__399__off = 0;
    CData/*7:0*/ __Vtask_test_blitter__DOT__blt_reg__399__data;
    __Vtask_test_blitter__DOT__blt_reg__399__data = 0;
    SData/*15:0*/ __Vtask_test_blitter__DOT__write_reg__400__addr;
    __Vtask_test_blitter__DOT__write_reg__400__addr = 0;
    CData/*7:0*/ __Vtask_test_blitter__DOT__write_reg__400__data;
    __Vtask_test_blitter__DOT__write_reg__400__data = 0;
    IData/*31:0*/ __Vtask_test_blitter__DOT__blt_reg__401__off;
    __Vtask_test_blitter__DOT__blt_reg__401__off = 0;
    CData/*7:0*/ __Vtask_test_blitter__DOT__blt_reg__401__data;
    __Vtask_test_blitter__DOT__blt_reg__401__data = 0;
    SData/*15:0*/ __Vtask_test_blitter__DOT__write_reg__402__addr;
    __Vtask_test_blitter__DOT__write_reg__402__addr = 0;
    CData/*7:0*/ __Vtask_test_blitter__DOT__write_reg__402__data;
    __Vtask_test_blitter__DOT__write_reg__402__data = 0;
    IData/*31:0*/ __Vtask_test_blitter__DOT__blt_reg__403__off;
    __Vtask_test_blitter__DOT__blt_reg__403__off = 0;
    CData/*7:0*/ __Vtask_test_blitter__DOT__blt_reg__403__data;
    __Vtask_test_blitter__DOT__blt_reg__403__data = 0;
    SData/*15:0*/ __Vtask_test_blitter__DOT__write_reg__404__addr;
    __Vtask_test_blitter__DOT__write_reg__404__addr = 0;
    CData/*7:0*/ __Vtask_test_blitter__DOT__write_reg__404__data;
    __Vtask_test_blitter__DOT__write_reg__404__data = 0;
    IData/*31:0*/ __Vtask_test_blitter__DOT__blt_reg__405__off;
    __Vtask_test_blitter__DOT__blt_reg__405__off = 0;
    CData/*7:0*/ __Vtask_test_blitter__DOT__blt_reg__405__data;
    __Vtask_test_blitter__DOT__blt_reg__405__data = 0;
    SData/*15:0*/ __Vtask_test_blitter__DOT__write_reg__406__addr;
    __Vtask_test_blitter__DOT__write_reg__406__addr = 0;
    CData/*7:0*/ __Vtask_test_blitter__DOT__write_reg__406__data;
    __Vtask_test_blitter__DOT__write_reg__406__data = 0;
    IData/*31:0*/ __Vtask_test_blitter__DOT__blt_reg__408__off;
    __Vtask_test_blitter__DOT__blt_reg__408__off = 0;
    CData/*7:0*/ __Vtask_test_blitter__DOT__blt_reg__408__data;
    __Vtask_test_blitter__DOT__blt_reg__408__data = 0;
    SData/*15:0*/ __Vtask_test_blitter__DOT__write_reg__409__addr;
    __Vtask_test_blitter__DOT__write_reg__409__addr = 0;
    CData/*7:0*/ __Vtask_test_blitter__DOT__write_reg__409__data;
    __Vtask_test_blitter__DOT__write_reg__409__data = 0;
    IData/*31:0*/ __Vtask_test_blitter__DOT__wait_blt_done__410__test_blitter__DOT__unnamedblk1_1__DOT____Vrepeat0;
    __Vtask_test_blitter__DOT__wait_blt_done__410__test_blitter__DOT__unnamedblk1_1__DOT____Vrepeat0 = 0;
    IData/*31:0*/ __Vtask_test_blitter__DOT__wait_blt_done__410__test_blitter__DOT__unnamedblk1_2__DOT____Vrepeat1;
    __Vtask_test_blitter__DOT__wait_blt_done__410__test_blitter__DOT__unnamedblk1_2__DOT____Vrepeat1 = 0;
    CData/*0:0*/ __Vtask_test_blitter__DOT__check__411__condition;
    __Vtask_test_blitter__DOT__check__411__condition = 0;
    CData/*0:0*/ __Vtask_test_blitter__DOT__check__412__condition;
    __Vtask_test_blitter__DOT__check__412__condition = 0;
    CData/*0:0*/ __Vtask_test_blitter__DOT__check__413__condition;
    __Vtask_test_blitter__DOT__check__413__condition = 0;
    IData/*31:0*/ __Vtask_test_blitter__DOT__setup_fill__414__dst_space;
    __Vtask_test_blitter__DOT__setup_fill__414__dst_space = 0;
    IData/*31:0*/ __Vtask_test_blitter__DOT__setup_fill__414__dst_addr;
    __Vtask_test_blitter__DOT__setup_fill__414__dst_addr = 0;
    IData/*31:0*/ __Vtask_test_blitter__DOT__setup_fill__414__dst_stride;
    __Vtask_test_blitter__DOT__setup_fill__414__dst_stride = 0;
    IData/*31:0*/ __Vtask_test_blitter__DOT__setup_fill__414__width;
    __Vtask_test_blitter__DOT__setup_fill__414__width = 0;
    IData/*31:0*/ __Vtask_test_blitter__DOT__setup_fill__414__height;
    __Vtask_test_blitter__DOT__setup_fill__414__height = 0;
    IData/*31:0*/ __Vtask_test_blitter__DOT__setup_fill__414__fill_val;
    __Vtask_test_blitter__DOT__setup_fill__414__fill_val = 0;
    IData/*31:0*/ __Vtask_test_blitter__DOT__blt_reg__415__off;
    __Vtask_test_blitter__DOT__blt_reg__415__off = 0;
    CData/*7:0*/ __Vtask_test_blitter__DOT__blt_reg__415__data;
    __Vtask_test_blitter__DOT__blt_reg__415__data = 0;
    SData/*15:0*/ __Vtask_test_blitter__DOT__write_reg__416__addr;
    __Vtask_test_blitter__DOT__write_reg__416__addr = 0;
    CData/*7:0*/ __Vtask_test_blitter__DOT__write_reg__416__data;
    __Vtask_test_blitter__DOT__write_reg__416__data = 0;
    IData/*31:0*/ __Vtask_test_blitter__DOT__blt_reg__417__off;
    __Vtask_test_blitter__DOT__blt_reg__417__off = 0;
    CData/*7:0*/ __Vtask_test_blitter__DOT__blt_reg__417__data;
    __Vtask_test_blitter__DOT__blt_reg__417__data = 0;
    SData/*15:0*/ __Vtask_test_blitter__DOT__write_reg__418__addr;
    __Vtask_test_blitter__DOT__write_reg__418__addr = 0;
    CData/*7:0*/ __Vtask_test_blitter__DOT__write_reg__418__data;
    __Vtask_test_blitter__DOT__write_reg__418__data = 0;
    IData/*31:0*/ __Vtask_test_blitter__DOT__blt_reg__419__off;
    __Vtask_test_blitter__DOT__blt_reg__419__off = 0;
    CData/*7:0*/ __Vtask_test_blitter__DOT__blt_reg__419__data;
    __Vtask_test_blitter__DOT__blt_reg__419__data = 0;
    SData/*15:0*/ __Vtask_test_blitter__DOT__write_reg__420__addr;
    __Vtask_test_blitter__DOT__write_reg__420__addr = 0;
    CData/*7:0*/ __Vtask_test_blitter__DOT__write_reg__420__data;
    __Vtask_test_blitter__DOT__write_reg__420__data = 0;
    IData/*31:0*/ __Vtask_test_blitter__DOT__blt_reg__421__off;
    __Vtask_test_blitter__DOT__blt_reg__421__off = 0;
    CData/*7:0*/ __Vtask_test_blitter__DOT__blt_reg__421__data;
    __Vtask_test_blitter__DOT__blt_reg__421__data = 0;
    SData/*15:0*/ __Vtask_test_blitter__DOT__write_reg__422__addr;
    __Vtask_test_blitter__DOT__write_reg__422__addr = 0;
    CData/*7:0*/ __Vtask_test_blitter__DOT__write_reg__422__data;
    __Vtask_test_blitter__DOT__write_reg__422__data = 0;
    IData/*31:0*/ __Vtask_test_blitter__DOT__blt_reg__423__off;
    __Vtask_test_blitter__DOT__blt_reg__423__off = 0;
    CData/*7:0*/ __Vtask_test_blitter__DOT__blt_reg__423__data;
    __Vtask_test_blitter__DOT__blt_reg__423__data = 0;
    SData/*15:0*/ __Vtask_test_blitter__DOT__write_reg__424__addr;
    __Vtask_test_blitter__DOT__write_reg__424__addr = 0;
    CData/*7:0*/ __Vtask_test_blitter__DOT__write_reg__424__data;
    __Vtask_test_blitter__DOT__write_reg__424__data = 0;
    IData/*31:0*/ __Vtask_test_blitter__DOT__blt_reg__425__off;
    __Vtask_test_blitter__DOT__blt_reg__425__off = 0;
    CData/*7:0*/ __Vtask_test_blitter__DOT__blt_reg__425__data;
    __Vtask_test_blitter__DOT__blt_reg__425__data = 0;
    SData/*15:0*/ __Vtask_test_blitter__DOT__write_reg__426__addr;
    __Vtask_test_blitter__DOT__write_reg__426__addr = 0;
    CData/*7:0*/ __Vtask_test_blitter__DOT__write_reg__426__data;
    __Vtask_test_blitter__DOT__write_reg__426__data = 0;
    IData/*31:0*/ __Vtask_test_blitter__DOT__blt_reg__427__off;
    __Vtask_test_blitter__DOT__blt_reg__427__off = 0;
    CData/*7:0*/ __Vtask_test_blitter__DOT__blt_reg__427__data;
    __Vtask_test_blitter__DOT__blt_reg__427__data = 0;
    SData/*15:0*/ __Vtask_test_blitter__DOT__write_reg__428__addr;
    __Vtask_test_blitter__DOT__write_reg__428__addr = 0;
    CData/*7:0*/ __Vtask_test_blitter__DOT__write_reg__428__data;
    __Vtask_test_blitter__DOT__write_reg__428__data = 0;
    IData/*31:0*/ __Vtask_test_blitter__DOT__blt_reg__429__off;
    __Vtask_test_blitter__DOT__blt_reg__429__off = 0;
    CData/*7:0*/ __Vtask_test_blitter__DOT__blt_reg__429__data;
    __Vtask_test_blitter__DOT__blt_reg__429__data = 0;
    SData/*15:0*/ __Vtask_test_blitter__DOT__write_reg__430__addr;
    __Vtask_test_blitter__DOT__write_reg__430__addr = 0;
    CData/*7:0*/ __Vtask_test_blitter__DOT__write_reg__430__data;
    __Vtask_test_blitter__DOT__write_reg__430__data = 0;
    IData/*31:0*/ __Vtask_test_blitter__DOT__blt_reg__431__off;
    __Vtask_test_blitter__DOT__blt_reg__431__off = 0;
    CData/*7:0*/ __Vtask_test_blitter__DOT__blt_reg__431__data;
    __Vtask_test_blitter__DOT__blt_reg__431__data = 0;
    SData/*15:0*/ __Vtask_test_blitter__DOT__write_reg__432__addr;
    __Vtask_test_blitter__DOT__write_reg__432__addr = 0;
    CData/*7:0*/ __Vtask_test_blitter__DOT__write_reg__432__data;
    __Vtask_test_blitter__DOT__write_reg__432__data = 0;
    IData/*31:0*/ __Vtask_test_blitter__DOT__blt_reg__433__off;
    __Vtask_test_blitter__DOT__blt_reg__433__off = 0;
    CData/*7:0*/ __Vtask_test_blitter__DOT__blt_reg__433__data;
    __Vtask_test_blitter__DOT__blt_reg__433__data = 0;
    SData/*15:0*/ __Vtask_test_blitter__DOT__write_reg__434__addr;
    __Vtask_test_blitter__DOT__write_reg__434__addr = 0;
    CData/*7:0*/ __Vtask_test_blitter__DOT__write_reg__434__data;
    __Vtask_test_blitter__DOT__write_reg__434__data = 0;
    IData/*31:0*/ __Vtask_test_blitter__DOT__blt_reg__435__off;
    __Vtask_test_blitter__DOT__blt_reg__435__off = 0;
    CData/*7:0*/ __Vtask_test_blitter__DOT__blt_reg__435__data;
    __Vtask_test_blitter__DOT__blt_reg__435__data = 0;
    SData/*15:0*/ __Vtask_test_blitter__DOT__write_reg__436__addr;
    __Vtask_test_blitter__DOT__write_reg__436__addr = 0;
    CData/*7:0*/ __Vtask_test_blitter__DOT__write_reg__436__data;
    __Vtask_test_blitter__DOT__write_reg__436__data = 0;
    IData/*31:0*/ __Vtask_test_blitter__DOT__blt_reg__437__off;
    __Vtask_test_blitter__DOT__blt_reg__437__off = 0;
    CData/*7:0*/ __Vtask_test_blitter__DOT__blt_reg__437__data;
    __Vtask_test_blitter__DOT__blt_reg__437__data = 0;
    SData/*15:0*/ __Vtask_test_blitter__DOT__write_reg__438__addr;
    __Vtask_test_blitter__DOT__write_reg__438__addr = 0;
    CData/*7:0*/ __Vtask_test_blitter__DOT__write_reg__438__data;
    __Vtask_test_blitter__DOT__write_reg__438__data = 0;
    IData/*31:0*/ __Vtask_test_blitter__DOT__blt_reg__440__off;
    __Vtask_test_blitter__DOT__blt_reg__440__off = 0;
    CData/*7:0*/ __Vtask_test_blitter__DOT__blt_reg__440__data;
    __Vtask_test_blitter__DOT__blt_reg__440__data = 0;
    SData/*15:0*/ __Vtask_test_blitter__DOT__write_reg__441__addr;
    __Vtask_test_blitter__DOT__write_reg__441__addr = 0;
    CData/*7:0*/ __Vtask_test_blitter__DOT__write_reg__441__data;
    __Vtask_test_blitter__DOT__write_reg__441__data = 0;
    IData/*31:0*/ __Vtask_test_blitter__DOT__wait_blt_done__442__test_blitter__DOT__unnamedblk1_1__DOT____Vrepeat0;
    __Vtask_test_blitter__DOT__wait_blt_done__442__test_blitter__DOT__unnamedblk1_1__DOT____Vrepeat0 = 0;
    IData/*31:0*/ __Vtask_test_blitter__DOT__wait_blt_done__442__test_blitter__DOT__unnamedblk1_2__DOT____Vrepeat1;
    __Vtask_test_blitter__DOT__wait_blt_done__442__test_blitter__DOT__unnamedblk1_2__DOT____Vrepeat1 = 0;
    CData/*0:0*/ __Vtask_test_blitter__DOT__check__443__condition;
    __Vtask_test_blitter__DOT__check__443__condition = 0;
    CData/*0:0*/ __Vtask_test_blitter__DOT__check__444__condition;
    __Vtask_test_blitter__DOT__check__444__condition = 0;
    CData/*0:0*/ __Vtask_test_blitter__DOT__check__445__condition;
    __Vtask_test_blitter__DOT__check__445__condition = 0;
    CData/*0:0*/ __Vtask_test_blitter__DOT__check__446__condition;
    __Vtask_test_blitter__DOT__check__446__condition = 0;
    IData/*31:0*/ __Vtask_test_blitter__DOT__setup_copy__447__src_space;
    __Vtask_test_blitter__DOT__setup_copy__447__src_space = 0;
    IData/*31:0*/ __Vtask_test_blitter__DOT__setup_copy__447__src_addr;
    __Vtask_test_blitter__DOT__setup_copy__447__src_addr = 0;
    IData/*31:0*/ __Vtask_test_blitter__DOT__setup_copy__447__src_stride;
    __Vtask_test_blitter__DOT__setup_copy__447__src_stride = 0;
    IData/*31:0*/ __Vtask_test_blitter__DOT__setup_copy__447__dst_space;
    __Vtask_test_blitter__DOT__setup_copy__447__dst_space = 0;
    IData/*31:0*/ __Vtask_test_blitter__DOT__setup_copy__447__dst_addr;
    __Vtask_test_blitter__DOT__setup_copy__447__dst_addr = 0;
    IData/*31:0*/ __Vtask_test_blitter__DOT__setup_copy__447__dst_stride;
    __Vtask_test_blitter__DOT__setup_copy__447__dst_stride = 0;
    IData/*31:0*/ __Vtask_test_blitter__DOT__setup_copy__447__width;
    __Vtask_test_blitter__DOT__setup_copy__447__width = 0;
    IData/*31:0*/ __Vtask_test_blitter__DOT__setup_copy__447__height;
    __Vtask_test_blitter__DOT__setup_copy__447__height = 0;
    IData/*31:0*/ __Vtask_test_blitter__DOT__blt_reg__448__off;
    __Vtask_test_blitter__DOT__blt_reg__448__off = 0;
    CData/*7:0*/ __Vtask_test_blitter__DOT__blt_reg__448__data;
    __Vtask_test_blitter__DOT__blt_reg__448__data = 0;
    SData/*15:0*/ __Vtask_test_blitter__DOT__write_reg__449__addr;
    __Vtask_test_blitter__DOT__write_reg__449__addr = 0;
    CData/*7:0*/ __Vtask_test_blitter__DOT__write_reg__449__data;
    __Vtask_test_blitter__DOT__write_reg__449__data = 0;
    IData/*31:0*/ __Vtask_test_blitter__DOT__blt_reg__450__off;
    __Vtask_test_blitter__DOT__blt_reg__450__off = 0;
    CData/*7:0*/ __Vtask_test_blitter__DOT__blt_reg__450__data;
    __Vtask_test_blitter__DOT__blt_reg__450__data = 0;
    SData/*15:0*/ __Vtask_test_blitter__DOT__write_reg__451__addr;
    __Vtask_test_blitter__DOT__write_reg__451__addr = 0;
    CData/*7:0*/ __Vtask_test_blitter__DOT__write_reg__451__data;
    __Vtask_test_blitter__DOT__write_reg__451__data = 0;
    IData/*31:0*/ __Vtask_test_blitter__DOT__blt_reg__452__off;
    __Vtask_test_blitter__DOT__blt_reg__452__off = 0;
    CData/*7:0*/ __Vtask_test_blitter__DOT__blt_reg__452__data;
    __Vtask_test_blitter__DOT__blt_reg__452__data = 0;
    SData/*15:0*/ __Vtask_test_blitter__DOT__write_reg__453__addr;
    __Vtask_test_blitter__DOT__write_reg__453__addr = 0;
    CData/*7:0*/ __Vtask_test_blitter__DOT__write_reg__453__data;
    __Vtask_test_blitter__DOT__write_reg__453__data = 0;
    IData/*31:0*/ __Vtask_test_blitter__DOT__blt_reg__454__off;
    __Vtask_test_blitter__DOT__blt_reg__454__off = 0;
    CData/*7:0*/ __Vtask_test_blitter__DOT__blt_reg__454__data;
    __Vtask_test_blitter__DOT__blt_reg__454__data = 0;
    SData/*15:0*/ __Vtask_test_blitter__DOT__write_reg__455__addr;
    __Vtask_test_blitter__DOT__write_reg__455__addr = 0;
    CData/*7:0*/ __Vtask_test_blitter__DOT__write_reg__455__data;
    __Vtask_test_blitter__DOT__write_reg__455__data = 0;
    IData/*31:0*/ __Vtask_test_blitter__DOT__blt_reg__456__off;
    __Vtask_test_blitter__DOT__blt_reg__456__off = 0;
    CData/*7:0*/ __Vtask_test_blitter__DOT__blt_reg__456__data;
    __Vtask_test_blitter__DOT__blt_reg__456__data = 0;
    SData/*15:0*/ __Vtask_test_blitter__DOT__write_reg__457__addr;
    __Vtask_test_blitter__DOT__write_reg__457__addr = 0;
    CData/*7:0*/ __Vtask_test_blitter__DOT__write_reg__457__data;
    __Vtask_test_blitter__DOT__write_reg__457__data = 0;
    IData/*31:0*/ __Vtask_test_blitter__DOT__blt_reg__458__off;
    __Vtask_test_blitter__DOT__blt_reg__458__off = 0;
    CData/*7:0*/ __Vtask_test_blitter__DOT__blt_reg__458__data;
    __Vtask_test_blitter__DOT__blt_reg__458__data = 0;
    SData/*15:0*/ __Vtask_test_blitter__DOT__write_reg__459__addr;
    __Vtask_test_blitter__DOT__write_reg__459__addr = 0;
    CData/*7:0*/ __Vtask_test_blitter__DOT__write_reg__459__data;
    __Vtask_test_blitter__DOT__write_reg__459__data = 0;
    IData/*31:0*/ __Vtask_test_blitter__DOT__blt_reg__460__off;
    __Vtask_test_blitter__DOT__blt_reg__460__off = 0;
    CData/*7:0*/ __Vtask_test_blitter__DOT__blt_reg__460__data;
    __Vtask_test_blitter__DOT__blt_reg__460__data = 0;
    SData/*15:0*/ __Vtask_test_blitter__DOT__write_reg__461__addr;
    __Vtask_test_blitter__DOT__write_reg__461__addr = 0;
    CData/*7:0*/ __Vtask_test_blitter__DOT__write_reg__461__data;
    __Vtask_test_blitter__DOT__write_reg__461__data = 0;
    IData/*31:0*/ __Vtask_test_blitter__DOT__blt_reg__462__off;
    __Vtask_test_blitter__DOT__blt_reg__462__off = 0;
    CData/*7:0*/ __Vtask_test_blitter__DOT__blt_reg__462__data;
    __Vtask_test_blitter__DOT__blt_reg__462__data = 0;
    SData/*15:0*/ __Vtask_test_blitter__DOT__write_reg__463__addr;
    __Vtask_test_blitter__DOT__write_reg__463__addr = 0;
    CData/*7:0*/ __Vtask_test_blitter__DOT__write_reg__463__data;
    __Vtask_test_blitter__DOT__write_reg__463__data = 0;
    IData/*31:0*/ __Vtask_test_blitter__DOT__blt_reg__464__off;
    __Vtask_test_blitter__DOT__blt_reg__464__off = 0;
    CData/*7:0*/ __Vtask_test_blitter__DOT__blt_reg__464__data;
    __Vtask_test_blitter__DOT__blt_reg__464__data = 0;
    SData/*15:0*/ __Vtask_test_blitter__DOT__write_reg__465__addr;
    __Vtask_test_blitter__DOT__write_reg__465__addr = 0;
    CData/*7:0*/ __Vtask_test_blitter__DOT__write_reg__465__data;
    __Vtask_test_blitter__DOT__write_reg__465__data = 0;
    IData/*31:0*/ __Vtask_test_blitter__DOT__blt_reg__466__off;
    __Vtask_test_blitter__DOT__blt_reg__466__off = 0;
    CData/*7:0*/ __Vtask_test_blitter__DOT__blt_reg__466__data;
    __Vtask_test_blitter__DOT__blt_reg__466__data = 0;
    SData/*15:0*/ __Vtask_test_blitter__DOT__write_reg__467__addr;
    __Vtask_test_blitter__DOT__write_reg__467__addr = 0;
    CData/*7:0*/ __Vtask_test_blitter__DOT__write_reg__467__data;
    __Vtask_test_blitter__DOT__write_reg__467__data = 0;
    IData/*31:0*/ __Vtask_test_blitter__DOT__blt_reg__468__off;
    __Vtask_test_blitter__DOT__blt_reg__468__off = 0;
    CData/*7:0*/ __Vtask_test_blitter__DOT__blt_reg__468__data;
    __Vtask_test_blitter__DOT__blt_reg__468__data = 0;
    SData/*15:0*/ __Vtask_test_blitter__DOT__write_reg__469__addr;
    __Vtask_test_blitter__DOT__write_reg__469__addr = 0;
    CData/*7:0*/ __Vtask_test_blitter__DOT__write_reg__469__data;
    __Vtask_test_blitter__DOT__write_reg__469__data = 0;
    IData/*31:0*/ __Vtask_test_blitter__DOT__blt_reg__470__off;
    __Vtask_test_blitter__DOT__blt_reg__470__off = 0;
    CData/*7:0*/ __Vtask_test_blitter__DOT__blt_reg__470__data;
    __Vtask_test_blitter__DOT__blt_reg__470__data = 0;
    SData/*15:0*/ __Vtask_test_blitter__DOT__write_reg__471__addr;
    __Vtask_test_blitter__DOT__write_reg__471__addr = 0;
    CData/*7:0*/ __Vtask_test_blitter__DOT__write_reg__471__data;
    __Vtask_test_blitter__DOT__write_reg__471__data = 0;
    IData/*31:0*/ __Vtask_test_blitter__DOT__blt_reg__472__off;
    __Vtask_test_blitter__DOT__blt_reg__472__off = 0;
    CData/*7:0*/ __Vtask_test_blitter__DOT__blt_reg__472__data;
    __Vtask_test_blitter__DOT__blt_reg__472__data = 0;
    SData/*15:0*/ __Vtask_test_blitter__DOT__write_reg__473__addr;
    __Vtask_test_blitter__DOT__write_reg__473__addr = 0;
    CData/*7:0*/ __Vtask_test_blitter__DOT__write_reg__473__data;
    __Vtask_test_blitter__DOT__write_reg__473__data = 0;
    IData/*31:0*/ __Vtask_test_blitter__DOT__blt_reg__474__off;
    __Vtask_test_blitter__DOT__blt_reg__474__off = 0;
    CData/*7:0*/ __Vtask_test_blitter__DOT__blt_reg__474__data;
    __Vtask_test_blitter__DOT__blt_reg__474__data = 0;
    SData/*15:0*/ __Vtask_test_blitter__DOT__write_reg__475__addr;
    __Vtask_test_blitter__DOT__write_reg__475__addr = 0;
    CData/*7:0*/ __Vtask_test_blitter__DOT__write_reg__475__data;
    __Vtask_test_blitter__DOT__write_reg__475__data = 0;
    IData/*31:0*/ __Vtask_test_blitter__DOT__blt_reg__476__off;
    __Vtask_test_blitter__DOT__blt_reg__476__off = 0;
    CData/*7:0*/ __Vtask_test_blitter__DOT__blt_reg__476__data;
    __Vtask_test_blitter__DOT__blt_reg__476__data = 0;
    SData/*15:0*/ __Vtask_test_blitter__DOT__write_reg__477__addr;
    __Vtask_test_blitter__DOT__write_reg__477__addr = 0;
    CData/*7:0*/ __Vtask_test_blitter__DOT__write_reg__477__data;
    __Vtask_test_blitter__DOT__write_reg__477__data = 0;
    IData/*31:0*/ __Vtask_test_blitter__DOT__blt_reg__478__off;
    __Vtask_test_blitter__DOT__blt_reg__478__off = 0;
    CData/*7:0*/ __Vtask_test_blitter__DOT__blt_reg__478__data;
    __Vtask_test_blitter__DOT__blt_reg__478__data = 0;
    SData/*15:0*/ __Vtask_test_blitter__DOT__write_reg__479__addr;
    __Vtask_test_blitter__DOT__write_reg__479__addr = 0;
    CData/*7:0*/ __Vtask_test_blitter__DOT__write_reg__479__data;
    __Vtask_test_blitter__DOT__write_reg__479__data = 0;
    IData/*31:0*/ __Vtask_test_blitter__DOT__blt_reg__480__off;
    __Vtask_test_blitter__DOT__blt_reg__480__off = 0;
    CData/*7:0*/ __Vtask_test_blitter__DOT__blt_reg__480__data;
    __Vtask_test_blitter__DOT__blt_reg__480__data = 0;
    SData/*15:0*/ __Vtask_test_blitter__DOT__write_reg__481__addr;
    __Vtask_test_blitter__DOT__write_reg__481__addr = 0;
    CData/*7:0*/ __Vtask_test_blitter__DOT__write_reg__481__data;
    __Vtask_test_blitter__DOT__write_reg__481__data = 0;
    IData/*31:0*/ __Vtask_test_blitter__DOT__blt_reg__483__off;
    __Vtask_test_blitter__DOT__blt_reg__483__off = 0;
    CData/*7:0*/ __Vtask_test_blitter__DOT__blt_reg__483__data;
    __Vtask_test_blitter__DOT__blt_reg__483__data = 0;
    SData/*15:0*/ __Vtask_test_blitter__DOT__write_reg__484__addr;
    __Vtask_test_blitter__DOT__write_reg__484__addr = 0;
    CData/*7:0*/ __Vtask_test_blitter__DOT__write_reg__484__data;
    __Vtask_test_blitter__DOT__write_reg__484__data = 0;
    IData/*31:0*/ __Vtask_test_blitter__DOT__wait_blt_done__485__test_blitter__DOT__unnamedblk1_1__DOT____Vrepeat0;
    __Vtask_test_blitter__DOT__wait_blt_done__485__test_blitter__DOT__unnamedblk1_1__DOT____Vrepeat0 = 0;
    IData/*31:0*/ __Vtask_test_blitter__DOT__wait_blt_done__485__test_blitter__DOT__unnamedblk1_2__DOT____Vrepeat1;
    __Vtask_test_blitter__DOT__wait_blt_done__485__test_blitter__DOT__unnamedblk1_2__DOT____Vrepeat1 = 0;
    CData/*0:0*/ __Vtask_test_blitter__DOT__check__486__condition;
    __Vtask_test_blitter__DOT__check__486__condition = 0;
    CData/*0:0*/ __Vtask_test_blitter__DOT__check__487__condition;
    __Vtask_test_blitter__DOT__check__487__condition = 0;
    CData/*0:0*/ __Vtask_test_blitter__DOT__check__488__condition;
    __Vtask_test_blitter__DOT__check__488__condition = 0;
    CData/*0:0*/ __Vtask_test_blitter__DOT__check__489__condition;
    __Vtask_test_blitter__DOT__check__489__condition = 0;
    CData/*0:0*/ __Vtask_test_blitter__DOT__check__490__condition;
    __Vtask_test_blitter__DOT__check__490__condition = 0;
    IData/*31:0*/ __Vtask_test_blitter__DOT__setup_copy__491__src_space;
    __Vtask_test_blitter__DOT__setup_copy__491__src_space = 0;
    IData/*31:0*/ __Vtask_test_blitter__DOT__setup_copy__491__src_addr;
    __Vtask_test_blitter__DOT__setup_copy__491__src_addr = 0;
    IData/*31:0*/ __Vtask_test_blitter__DOT__setup_copy__491__src_stride;
    __Vtask_test_blitter__DOT__setup_copy__491__src_stride = 0;
    IData/*31:0*/ __Vtask_test_blitter__DOT__setup_copy__491__dst_space;
    __Vtask_test_blitter__DOT__setup_copy__491__dst_space = 0;
    IData/*31:0*/ __Vtask_test_blitter__DOT__setup_copy__491__dst_addr;
    __Vtask_test_blitter__DOT__setup_copy__491__dst_addr = 0;
    IData/*31:0*/ __Vtask_test_blitter__DOT__setup_copy__491__dst_stride;
    __Vtask_test_blitter__DOT__setup_copy__491__dst_stride = 0;
    IData/*31:0*/ __Vtask_test_blitter__DOT__setup_copy__491__width;
    __Vtask_test_blitter__DOT__setup_copy__491__width = 0;
    IData/*31:0*/ __Vtask_test_blitter__DOT__setup_copy__491__height;
    __Vtask_test_blitter__DOT__setup_copy__491__height = 0;
    IData/*31:0*/ __Vtask_test_blitter__DOT__blt_reg__492__off;
    __Vtask_test_blitter__DOT__blt_reg__492__off = 0;
    CData/*7:0*/ __Vtask_test_blitter__DOT__blt_reg__492__data;
    __Vtask_test_blitter__DOT__blt_reg__492__data = 0;
    SData/*15:0*/ __Vtask_test_blitter__DOT__write_reg__493__addr;
    __Vtask_test_blitter__DOT__write_reg__493__addr = 0;
    CData/*7:0*/ __Vtask_test_blitter__DOT__write_reg__493__data;
    __Vtask_test_blitter__DOT__write_reg__493__data = 0;
    IData/*31:0*/ __Vtask_test_blitter__DOT__blt_reg__494__off;
    __Vtask_test_blitter__DOT__blt_reg__494__off = 0;
    CData/*7:0*/ __Vtask_test_blitter__DOT__blt_reg__494__data;
    __Vtask_test_blitter__DOT__blt_reg__494__data = 0;
    SData/*15:0*/ __Vtask_test_blitter__DOT__write_reg__495__addr;
    __Vtask_test_blitter__DOT__write_reg__495__addr = 0;
    CData/*7:0*/ __Vtask_test_blitter__DOT__write_reg__495__data;
    __Vtask_test_blitter__DOT__write_reg__495__data = 0;
    IData/*31:0*/ __Vtask_test_blitter__DOT__blt_reg__496__off;
    __Vtask_test_blitter__DOT__blt_reg__496__off = 0;
    CData/*7:0*/ __Vtask_test_blitter__DOT__blt_reg__496__data;
    __Vtask_test_blitter__DOT__blt_reg__496__data = 0;
    SData/*15:0*/ __Vtask_test_blitter__DOT__write_reg__497__addr;
    __Vtask_test_blitter__DOT__write_reg__497__addr = 0;
    CData/*7:0*/ __Vtask_test_blitter__DOT__write_reg__497__data;
    __Vtask_test_blitter__DOT__write_reg__497__data = 0;
    IData/*31:0*/ __Vtask_test_blitter__DOT__blt_reg__498__off;
    __Vtask_test_blitter__DOT__blt_reg__498__off = 0;
    CData/*7:0*/ __Vtask_test_blitter__DOT__blt_reg__498__data;
    __Vtask_test_blitter__DOT__blt_reg__498__data = 0;
    SData/*15:0*/ __Vtask_test_blitter__DOT__write_reg__499__addr;
    __Vtask_test_blitter__DOT__write_reg__499__addr = 0;
    CData/*7:0*/ __Vtask_test_blitter__DOT__write_reg__499__data;
    __Vtask_test_blitter__DOT__write_reg__499__data = 0;
    IData/*31:0*/ __Vtask_test_blitter__DOT__blt_reg__500__off;
    __Vtask_test_blitter__DOT__blt_reg__500__off = 0;
    CData/*7:0*/ __Vtask_test_blitter__DOT__blt_reg__500__data;
    __Vtask_test_blitter__DOT__blt_reg__500__data = 0;
    SData/*15:0*/ __Vtask_test_blitter__DOT__write_reg__501__addr;
    __Vtask_test_blitter__DOT__write_reg__501__addr = 0;
    CData/*7:0*/ __Vtask_test_blitter__DOT__write_reg__501__data;
    __Vtask_test_blitter__DOT__write_reg__501__data = 0;
    IData/*31:0*/ __Vtask_test_blitter__DOT__blt_reg__502__off;
    __Vtask_test_blitter__DOT__blt_reg__502__off = 0;
    CData/*7:0*/ __Vtask_test_blitter__DOT__blt_reg__502__data;
    __Vtask_test_blitter__DOT__blt_reg__502__data = 0;
    SData/*15:0*/ __Vtask_test_blitter__DOT__write_reg__503__addr;
    __Vtask_test_blitter__DOT__write_reg__503__addr = 0;
    CData/*7:0*/ __Vtask_test_blitter__DOT__write_reg__503__data;
    __Vtask_test_blitter__DOT__write_reg__503__data = 0;
    IData/*31:0*/ __Vtask_test_blitter__DOT__blt_reg__504__off;
    __Vtask_test_blitter__DOT__blt_reg__504__off = 0;
    CData/*7:0*/ __Vtask_test_blitter__DOT__blt_reg__504__data;
    __Vtask_test_blitter__DOT__blt_reg__504__data = 0;
    SData/*15:0*/ __Vtask_test_blitter__DOT__write_reg__505__addr;
    __Vtask_test_blitter__DOT__write_reg__505__addr = 0;
    CData/*7:0*/ __Vtask_test_blitter__DOT__write_reg__505__data;
    __Vtask_test_blitter__DOT__write_reg__505__data = 0;
    IData/*31:0*/ __Vtask_test_blitter__DOT__blt_reg__506__off;
    __Vtask_test_blitter__DOT__blt_reg__506__off = 0;
    CData/*7:0*/ __Vtask_test_blitter__DOT__blt_reg__506__data;
    __Vtask_test_blitter__DOT__blt_reg__506__data = 0;
    SData/*15:0*/ __Vtask_test_blitter__DOT__write_reg__507__addr;
    __Vtask_test_blitter__DOT__write_reg__507__addr = 0;
    CData/*7:0*/ __Vtask_test_blitter__DOT__write_reg__507__data;
    __Vtask_test_blitter__DOT__write_reg__507__data = 0;
    IData/*31:0*/ __Vtask_test_blitter__DOT__blt_reg__508__off;
    __Vtask_test_blitter__DOT__blt_reg__508__off = 0;
    CData/*7:0*/ __Vtask_test_blitter__DOT__blt_reg__508__data;
    __Vtask_test_blitter__DOT__blt_reg__508__data = 0;
    SData/*15:0*/ __Vtask_test_blitter__DOT__write_reg__509__addr;
    __Vtask_test_blitter__DOT__write_reg__509__addr = 0;
    CData/*7:0*/ __Vtask_test_blitter__DOT__write_reg__509__data;
    __Vtask_test_blitter__DOT__write_reg__509__data = 0;
    IData/*31:0*/ __Vtask_test_blitter__DOT__blt_reg__510__off;
    __Vtask_test_blitter__DOT__blt_reg__510__off = 0;
    CData/*7:0*/ __Vtask_test_blitter__DOT__blt_reg__510__data;
    __Vtask_test_blitter__DOT__blt_reg__510__data = 0;
    SData/*15:0*/ __Vtask_test_blitter__DOT__write_reg__511__addr;
    __Vtask_test_blitter__DOT__write_reg__511__addr = 0;
    CData/*7:0*/ __Vtask_test_blitter__DOT__write_reg__511__data;
    __Vtask_test_blitter__DOT__write_reg__511__data = 0;
    IData/*31:0*/ __Vtask_test_blitter__DOT__blt_reg__512__off;
    __Vtask_test_blitter__DOT__blt_reg__512__off = 0;
    CData/*7:0*/ __Vtask_test_blitter__DOT__blt_reg__512__data;
    __Vtask_test_blitter__DOT__blt_reg__512__data = 0;
    SData/*15:0*/ __Vtask_test_blitter__DOT__write_reg__513__addr;
    __Vtask_test_blitter__DOT__write_reg__513__addr = 0;
    CData/*7:0*/ __Vtask_test_blitter__DOT__write_reg__513__data;
    __Vtask_test_blitter__DOT__write_reg__513__data = 0;
    IData/*31:0*/ __Vtask_test_blitter__DOT__blt_reg__514__off;
    __Vtask_test_blitter__DOT__blt_reg__514__off = 0;
    CData/*7:0*/ __Vtask_test_blitter__DOT__blt_reg__514__data;
    __Vtask_test_blitter__DOT__blt_reg__514__data = 0;
    SData/*15:0*/ __Vtask_test_blitter__DOT__write_reg__515__addr;
    __Vtask_test_blitter__DOT__write_reg__515__addr = 0;
    CData/*7:0*/ __Vtask_test_blitter__DOT__write_reg__515__data;
    __Vtask_test_blitter__DOT__write_reg__515__data = 0;
    IData/*31:0*/ __Vtask_test_blitter__DOT__blt_reg__516__off;
    __Vtask_test_blitter__DOT__blt_reg__516__off = 0;
    CData/*7:0*/ __Vtask_test_blitter__DOT__blt_reg__516__data;
    __Vtask_test_blitter__DOT__blt_reg__516__data = 0;
    SData/*15:0*/ __Vtask_test_blitter__DOT__write_reg__517__addr;
    __Vtask_test_blitter__DOT__write_reg__517__addr = 0;
    CData/*7:0*/ __Vtask_test_blitter__DOT__write_reg__517__data;
    __Vtask_test_blitter__DOT__write_reg__517__data = 0;
    IData/*31:0*/ __Vtask_test_blitter__DOT__blt_reg__518__off;
    __Vtask_test_blitter__DOT__blt_reg__518__off = 0;
    CData/*7:0*/ __Vtask_test_blitter__DOT__blt_reg__518__data;
    __Vtask_test_blitter__DOT__blt_reg__518__data = 0;
    SData/*15:0*/ __Vtask_test_blitter__DOT__write_reg__519__addr;
    __Vtask_test_blitter__DOT__write_reg__519__addr = 0;
    CData/*7:0*/ __Vtask_test_blitter__DOT__write_reg__519__data;
    __Vtask_test_blitter__DOT__write_reg__519__data = 0;
    IData/*31:0*/ __Vtask_test_blitter__DOT__blt_reg__520__off;
    __Vtask_test_blitter__DOT__blt_reg__520__off = 0;
    CData/*7:0*/ __Vtask_test_blitter__DOT__blt_reg__520__data;
    __Vtask_test_blitter__DOT__blt_reg__520__data = 0;
    SData/*15:0*/ __Vtask_test_blitter__DOT__write_reg__521__addr;
    __Vtask_test_blitter__DOT__write_reg__521__addr = 0;
    CData/*7:0*/ __Vtask_test_blitter__DOT__write_reg__521__data;
    __Vtask_test_blitter__DOT__write_reg__521__data = 0;
    IData/*31:0*/ __Vtask_test_blitter__DOT__blt_reg__522__off;
    __Vtask_test_blitter__DOT__blt_reg__522__off = 0;
    CData/*7:0*/ __Vtask_test_blitter__DOT__blt_reg__522__data;
    __Vtask_test_blitter__DOT__blt_reg__522__data = 0;
    SData/*15:0*/ __Vtask_test_blitter__DOT__write_reg__523__addr;
    __Vtask_test_blitter__DOT__write_reg__523__addr = 0;
    CData/*7:0*/ __Vtask_test_blitter__DOT__write_reg__523__data;
    __Vtask_test_blitter__DOT__write_reg__523__data = 0;
    IData/*31:0*/ __Vtask_test_blitter__DOT__blt_reg__524__off;
    __Vtask_test_blitter__DOT__blt_reg__524__off = 0;
    CData/*7:0*/ __Vtask_test_blitter__DOT__blt_reg__524__data;
    __Vtask_test_blitter__DOT__blt_reg__524__data = 0;
    SData/*15:0*/ __Vtask_test_blitter__DOT__write_reg__525__addr;
    __Vtask_test_blitter__DOT__write_reg__525__addr = 0;
    CData/*7:0*/ __Vtask_test_blitter__DOT__write_reg__525__data;
    __Vtask_test_blitter__DOT__write_reg__525__data = 0;
    IData/*31:0*/ __Vtask_test_blitter__DOT__blt_reg__527__off;
    __Vtask_test_blitter__DOT__blt_reg__527__off = 0;
    CData/*7:0*/ __Vtask_test_blitter__DOT__blt_reg__527__data;
    __Vtask_test_blitter__DOT__blt_reg__527__data = 0;
    SData/*15:0*/ __Vtask_test_blitter__DOT__write_reg__528__addr;
    __Vtask_test_blitter__DOT__write_reg__528__addr = 0;
    CData/*7:0*/ __Vtask_test_blitter__DOT__write_reg__528__data;
    __Vtask_test_blitter__DOT__write_reg__528__data = 0;
    IData/*31:0*/ __Vtask_test_blitter__DOT__wait_blt_done__529__test_blitter__DOT__unnamedblk1_1__DOT____Vrepeat0;
    __Vtask_test_blitter__DOT__wait_blt_done__529__test_blitter__DOT__unnamedblk1_1__DOT____Vrepeat0 = 0;
    IData/*31:0*/ __Vtask_test_blitter__DOT__wait_blt_done__529__test_blitter__DOT__unnamedblk1_2__DOT____Vrepeat1;
    __Vtask_test_blitter__DOT__wait_blt_done__529__test_blitter__DOT__unnamedblk1_2__DOT____Vrepeat1 = 0;
    CData/*0:0*/ __Vtask_test_blitter__DOT__check__530__condition;
    __Vtask_test_blitter__DOT__check__530__condition = 0;
    CData/*0:0*/ __Vtask_test_blitter__DOT__check__531__condition;
    __Vtask_test_blitter__DOT__check__531__condition = 0;
    IData/*31:0*/ __Vtask_test_blitter__DOT__setup_fill__532__dst_space;
    __Vtask_test_blitter__DOT__setup_fill__532__dst_space = 0;
    IData/*31:0*/ __Vtask_test_blitter__DOT__setup_fill__532__dst_addr;
    __Vtask_test_blitter__DOT__setup_fill__532__dst_addr = 0;
    IData/*31:0*/ __Vtask_test_blitter__DOT__setup_fill__532__dst_stride;
    __Vtask_test_blitter__DOT__setup_fill__532__dst_stride = 0;
    IData/*31:0*/ __Vtask_test_blitter__DOT__setup_fill__532__width;
    __Vtask_test_blitter__DOT__setup_fill__532__width = 0;
    IData/*31:0*/ __Vtask_test_blitter__DOT__setup_fill__532__height;
    __Vtask_test_blitter__DOT__setup_fill__532__height = 0;
    IData/*31:0*/ __Vtask_test_blitter__DOT__setup_fill__532__fill_val;
    __Vtask_test_blitter__DOT__setup_fill__532__fill_val = 0;
    IData/*31:0*/ __Vtask_test_blitter__DOT__blt_reg__533__off;
    __Vtask_test_blitter__DOT__blt_reg__533__off = 0;
    CData/*7:0*/ __Vtask_test_blitter__DOT__blt_reg__533__data;
    __Vtask_test_blitter__DOT__blt_reg__533__data = 0;
    SData/*15:0*/ __Vtask_test_blitter__DOT__write_reg__534__addr;
    __Vtask_test_blitter__DOT__write_reg__534__addr = 0;
    CData/*7:0*/ __Vtask_test_blitter__DOT__write_reg__534__data;
    __Vtask_test_blitter__DOT__write_reg__534__data = 0;
    IData/*31:0*/ __Vtask_test_blitter__DOT__blt_reg__535__off;
    __Vtask_test_blitter__DOT__blt_reg__535__off = 0;
    CData/*7:0*/ __Vtask_test_blitter__DOT__blt_reg__535__data;
    __Vtask_test_blitter__DOT__blt_reg__535__data = 0;
    SData/*15:0*/ __Vtask_test_blitter__DOT__write_reg__536__addr;
    __Vtask_test_blitter__DOT__write_reg__536__addr = 0;
    CData/*7:0*/ __Vtask_test_blitter__DOT__write_reg__536__data;
    __Vtask_test_blitter__DOT__write_reg__536__data = 0;
    IData/*31:0*/ __Vtask_test_blitter__DOT__blt_reg__537__off;
    __Vtask_test_blitter__DOT__blt_reg__537__off = 0;
    CData/*7:0*/ __Vtask_test_blitter__DOT__blt_reg__537__data;
    __Vtask_test_blitter__DOT__blt_reg__537__data = 0;
    SData/*15:0*/ __Vtask_test_blitter__DOT__write_reg__538__addr;
    __Vtask_test_blitter__DOT__write_reg__538__addr = 0;
    CData/*7:0*/ __Vtask_test_blitter__DOT__write_reg__538__data;
    __Vtask_test_blitter__DOT__write_reg__538__data = 0;
    IData/*31:0*/ __Vtask_test_blitter__DOT__blt_reg__539__off;
    __Vtask_test_blitter__DOT__blt_reg__539__off = 0;
    CData/*7:0*/ __Vtask_test_blitter__DOT__blt_reg__539__data;
    __Vtask_test_blitter__DOT__blt_reg__539__data = 0;
    SData/*15:0*/ __Vtask_test_blitter__DOT__write_reg__540__addr;
    __Vtask_test_blitter__DOT__write_reg__540__addr = 0;
    CData/*7:0*/ __Vtask_test_blitter__DOT__write_reg__540__data;
    __Vtask_test_blitter__DOT__write_reg__540__data = 0;
    IData/*31:0*/ __Vtask_test_blitter__DOT__blt_reg__541__off;
    __Vtask_test_blitter__DOT__blt_reg__541__off = 0;
    CData/*7:0*/ __Vtask_test_blitter__DOT__blt_reg__541__data;
    __Vtask_test_blitter__DOT__blt_reg__541__data = 0;
    SData/*15:0*/ __Vtask_test_blitter__DOT__write_reg__542__addr;
    __Vtask_test_blitter__DOT__write_reg__542__addr = 0;
    CData/*7:0*/ __Vtask_test_blitter__DOT__write_reg__542__data;
    __Vtask_test_blitter__DOT__write_reg__542__data = 0;
    IData/*31:0*/ __Vtask_test_blitter__DOT__blt_reg__543__off;
    __Vtask_test_blitter__DOT__blt_reg__543__off = 0;
    CData/*7:0*/ __Vtask_test_blitter__DOT__blt_reg__543__data;
    __Vtask_test_blitter__DOT__blt_reg__543__data = 0;
    SData/*15:0*/ __Vtask_test_blitter__DOT__write_reg__544__addr;
    __Vtask_test_blitter__DOT__write_reg__544__addr = 0;
    CData/*7:0*/ __Vtask_test_blitter__DOT__write_reg__544__data;
    __Vtask_test_blitter__DOT__write_reg__544__data = 0;
    IData/*31:0*/ __Vtask_test_blitter__DOT__blt_reg__545__off;
    __Vtask_test_blitter__DOT__blt_reg__545__off = 0;
    CData/*7:0*/ __Vtask_test_blitter__DOT__blt_reg__545__data;
    __Vtask_test_blitter__DOT__blt_reg__545__data = 0;
    SData/*15:0*/ __Vtask_test_blitter__DOT__write_reg__546__addr;
    __Vtask_test_blitter__DOT__write_reg__546__addr = 0;
    CData/*7:0*/ __Vtask_test_blitter__DOT__write_reg__546__data;
    __Vtask_test_blitter__DOT__write_reg__546__data = 0;
    IData/*31:0*/ __Vtask_test_blitter__DOT__blt_reg__547__off;
    __Vtask_test_blitter__DOT__blt_reg__547__off = 0;
    CData/*7:0*/ __Vtask_test_blitter__DOT__blt_reg__547__data;
    __Vtask_test_blitter__DOT__blt_reg__547__data = 0;
    SData/*15:0*/ __Vtask_test_blitter__DOT__write_reg__548__addr;
    __Vtask_test_blitter__DOT__write_reg__548__addr = 0;
    CData/*7:0*/ __Vtask_test_blitter__DOT__write_reg__548__data;
    __Vtask_test_blitter__DOT__write_reg__548__data = 0;
    IData/*31:0*/ __Vtask_test_blitter__DOT__blt_reg__549__off;
    __Vtask_test_blitter__DOT__blt_reg__549__off = 0;
    CData/*7:0*/ __Vtask_test_blitter__DOT__blt_reg__549__data;
    __Vtask_test_blitter__DOT__blt_reg__549__data = 0;
    SData/*15:0*/ __Vtask_test_blitter__DOT__write_reg__550__addr;
    __Vtask_test_blitter__DOT__write_reg__550__addr = 0;
    CData/*7:0*/ __Vtask_test_blitter__DOT__write_reg__550__data;
    __Vtask_test_blitter__DOT__write_reg__550__data = 0;
    IData/*31:0*/ __Vtask_test_blitter__DOT__blt_reg__551__off;
    __Vtask_test_blitter__DOT__blt_reg__551__off = 0;
    CData/*7:0*/ __Vtask_test_blitter__DOT__blt_reg__551__data;
    __Vtask_test_blitter__DOT__blt_reg__551__data = 0;
    SData/*15:0*/ __Vtask_test_blitter__DOT__write_reg__552__addr;
    __Vtask_test_blitter__DOT__write_reg__552__addr = 0;
    CData/*7:0*/ __Vtask_test_blitter__DOT__write_reg__552__data;
    __Vtask_test_blitter__DOT__write_reg__552__data = 0;
    IData/*31:0*/ __Vtask_test_blitter__DOT__blt_reg__553__off;
    __Vtask_test_blitter__DOT__blt_reg__553__off = 0;
    CData/*7:0*/ __Vtask_test_blitter__DOT__blt_reg__553__data;
    __Vtask_test_blitter__DOT__blt_reg__553__data = 0;
    SData/*15:0*/ __Vtask_test_blitter__DOT__write_reg__554__addr;
    __Vtask_test_blitter__DOT__write_reg__554__addr = 0;
    CData/*7:0*/ __Vtask_test_blitter__DOT__write_reg__554__data;
    __Vtask_test_blitter__DOT__write_reg__554__data = 0;
    IData/*31:0*/ __Vtask_test_blitter__DOT__blt_reg__555__off;
    __Vtask_test_blitter__DOT__blt_reg__555__off = 0;
    CData/*7:0*/ __Vtask_test_blitter__DOT__blt_reg__555__data;
    __Vtask_test_blitter__DOT__blt_reg__555__data = 0;
    SData/*15:0*/ __Vtask_test_blitter__DOT__write_reg__556__addr;
    __Vtask_test_blitter__DOT__write_reg__556__addr = 0;
    CData/*7:0*/ __Vtask_test_blitter__DOT__write_reg__556__data;
    __Vtask_test_blitter__DOT__write_reg__556__data = 0;
    IData/*31:0*/ __Vtask_test_blitter__DOT__blt_reg__558__off;
    __Vtask_test_blitter__DOT__blt_reg__558__off = 0;
    CData/*7:0*/ __Vtask_test_blitter__DOT__blt_reg__558__data;
    __Vtask_test_blitter__DOT__blt_reg__558__data = 0;
    SData/*15:0*/ __Vtask_test_blitter__DOT__write_reg__559__addr;
    __Vtask_test_blitter__DOT__write_reg__559__addr = 0;
    CData/*7:0*/ __Vtask_test_blitter__DOT__write_reg__559__data;
    __Vtask_test_blitter__DOT__write_reg__559__data = 0;
    IData/*31:0*/ __Vtask_test_blitter__DOT__wait_blt_done__560__test_blitter__DOT__unnamedblk1_1__DOT____Vrepeat0;
    __Vtask_test_blitter__DOT__wait_blt_done__560__test_blitter__DOT__unnamedblk1_1__DOT____Vrepeat0 = 0;
    IData/*31:0*/ __Vtask_test_blitter__DOT__wait_blt_done__560__test_blitter__DOT__unnamedblk1_2__DOT____Vrepeat1;
    __Vtask_test_blitter__DOT__wait_blt_done__560__test_blitter__DOT__unnamedblk1_2__DOT____Vrepeat1 = 0;
    CData/*0:0*/ __Vtask_test_blitter__DOT__check__561__condition;
    __Vtask_test_blitter__DOT__check__561__condition = 0;
    CData/*0:0*/ __Vtask_test_blitter__DOT__check__562__condition;
    __Vtask_test_blitter__DOT__check__562__condition = 0;
    IData/*31:0*/ __Vtask_test_blitter__DOT__setup_fill__563__dst_space;
    __Vtask_test_blitter__DOT__setup_fill__563__dst_space = 0;
    IData/*31:0*/ __Vtask_test_blitter__DOT__setup_fill__563__dst_addr;
    __Vtask_test_blitter__DOT__setup_fill__563__dst_addr = 0;
    IData/*31:0*/ __Vtask_test_blitter__DOT__setup_fill__563__dst_stride;
    __Vtask_test_blitter__DOT__setup_fill__563__dst_stride = 0;
    IData/*31:0*/ __Vtask_test_blitter__DOT__setup_fill__563__width;
    __Vtask_test_blitter__DOT__setup_fill__563__width = 0;
    IData/*31:0*/ __Vtask_test_blitter__DOT__setup_fill__563__height;
    __Vtask_test_blitter__DOT__setup_fill__563__height = 0;
    IData/*31:0*/ __Vtask_test_blitter__DOT__setup_fill__563__fill_val;
    __Vtask_test_blitter__DOT__setup_fill__563__fill_val = 0;
    IData/*31:0*/ __Vtask_test_blitter__DOT__blt_reg__564__off;
    __Vtask_test_blitter__DOT__blt_reg__564__off = 0;
    CData/*7:0*/ __Vtask_test_blitter__DOT__blt_reg__564__data;
    __Vtask_test_blitter__DOT__blt_reg__564__data = 0;
    SData/*15:0*/ __Vtask_test_blitter__DOT__write_reg__565__addr;
    __Vtask_test_blitter__DOT__write_reg__565__addr = 0;
    CData/*7:0*/ __Vtask_test_blitter__DOT__write_reg__565__data;
    __Vtask_test_blitter__DOT__write_reg__565__data = 0;
    IData/*31:0*/ __Vtask_test_blitter__DOT__blt_reg__566__off;
    __Vtask_test_blitter__DOT__blt_reg__566__off = 0;
    CData/*7:0*/ __Vtask_test_blitter__DOT__blt_reg__566__data;
    __Vtask_test_blitter__DOT__blt_reg__566__data = 0;
    SData/*15:0*/ __Vtask_test_blitter__DOT__write_reg__567__addr;
    __Vtask_test_blitter__DOT__write_reg__567__addr = 0;
    CData/*7:0*/ __Vtask_test_blitter__DOT__write_reg__567__data;
    __Vtask_test_blitter__DOT__write_reg__567__data = 0;
    IData/*31:0*/ __Vtask_test_blitter__DOT__blt_reg__568__off;
    __Vtask_test_blitter__DOT__blt_reg__568__off = 0;
    CData/*7:0*/ __Vtask_test_blitter__DOT__blt_reg__568__data;
    __Vtask_test_blitter__DOT__blt_reg__568__data = 0;
    SData/*15:0*/ __Vtask_test_blitter__DOT__write_reg__569__addr;
    __Vtask_test_blitter__DOT__write_reg__569__addr = 0;
    CData/*7:0*/ __Vtask_test_blitter__DOT__write_reg__569__data;
    __Vtask_test_blitter__DOT__write_reg__569__data = 0;
    IData/*31:0*/ __Vtask_test_blitter__DOT__blt_reg__570__off;
    __Vtask_test_blitter__DOT__blt_reg__570__off = 0;
    CData/*7:0*/ __Vtask_test_blitter__DOT__blt_reg__570__data;
    __Vtask_test_blitter__DOT__blt_reg__570__data = 0;
    SData/*15:0*/ __Vtask_test_blitter__DOT__write_reg__571__addr;
    __Vtask_test_blitter__DOT__write_reg__571__addr = 0;
    CData/*7:0*/ __Vtask_test_blitter__DOT__write_reg__571__data;
    __Vtask_test_blitter__DOT__write_reg__571__data = 0;
    IData/*31:0*/ __Vtask_test_blitter__DOT__blt_reg__572__off;
    __Vtask_test_blitter__DOT__blt_reg__572__off = 0;
    CData/*7:0*/ __Vtask_test_blitter__DOT__blt_reg__572__data;
    __Vtask_test_blitter__DOT__blt_reg__572__data = 0;
    SData/*15:0*/ __Vtask_test_blitter__DOT__write_reg__573__addr;
    __Vtask_test_blitter__DOT__write_reg__573__addr = 0;
    CData/*7:0*/ __Vtask_test_blitter__DOT__write_reg__573__data;
    __Vtask_test_blitter__DOT__write_reg__573__data = 0;
    IData/*31:0*/ __Vtask_test_blitter__DOT__blt_reg__574__off;
    __Vtask_test_blitter__DOT__blt_reg__574__off = 0;
    CData/*7:0*/ __Vtask_test_blitter__DOT__blt_reg__574__data;
    __Vtask_test_blitter__DOT__blt_reg__574__data = 0;
    SData/*15:0*/ __Vtask_test_blitter__DOT__write_reg__575__addr;
    __Vtask_test_blitter__DOT__write_reg__575__addr = 0;
    CData/*7:0*/ __Vtask_test_blitter__DOT__write_reg__575__data;
    __Vtask_test_blitter__DOT__write_reg__575__data = 0;
    IData/*31:0*/ __Vtask_test_blitter__DOT__blt_reg__576__off;
    __Vtask_test_blitter__DOT__blt_reg__576__off = 0;
    CData/*7:0*/ __Vtask_test_blitter__DOT__blt_reg__576__data;
    __Vtask_test_blitter__DOT__blt_reg__576__data = 0;
    SData/*15:0*/ __Vtask_test_blitter__DOT__write_reg__577__addr;
    __Vtask_test_blitter__DOT__write_reg__577__addr = 0;
    CData/*7:0*/ __Vtask_test_blitter__DOT__write_reg__577__data;
    __Vtask_test_blitter__DOT__write_reg__577__data = 0;
    IData/*31:0*/ __Vtask_test_blitter__DOT__blt_reg__578__off;
    __Vtask_test_blitter__DOT__blt_reg__578__off = 0;
    CData/*7:0*/ __Vtask_test_blitter__DOT__blt_reg__578__data;
    __Vtask_test_blitter__DOT__blt_reg__578__data = 0;
    SData/*15:0*/ __Vtask_test_blitter__DOT__write_reg__579__addr;
    __Vtask_test_blitter__DOT__write_reg__579__addr = 0;
    CData/*7:0*/ __Vtask_test_blitter__DOT__write_reg__579__data;
    __Vtask_test_blitter__DOT__write_reg__579__data = 0;
    IData/*31:0*/ __Vtask_test_blitter__DOT__blt_reg__580__off;
    __Vtask_test_blitter__DOT__blt_reg__580__off = 0;
    CData/*7:0*/ __Vtask_test_blitter__DOT__blt_reg__580__data;
    __Vtask_test_blitter__DOT__blt_reg__580__data = 0;
    SData/*15:0*/ __Vtask_test_blitter__DOT__write_reg__581__addr;
    __Vtask_test_blitter__DOT__write_reg__581__addr = 0;
    CData/*7:0*/ __Vtask_test_blitter__DOT__write_reg__581__data;
    __Vtask_test_blitter__DOT__write_reg__581__data = 0;
    IData/*31:0*/ __Vtask_test_blitter__DOT__blt_reg__582__off;
    __Vtask_test_blitter__DOT__blt_reg__582__off = 0;
    CData/*7:0*/ __Vtask_test_blitter__DOT__blt_reg__582__data;
    __Vtask_test_blitter__DOT__blt_reg__582__data = 0;
    SData/*15:0*/ __Vtask_test_blitter__DOT__write_reg__583__addr;
    __Vtask_test_blitter__DOT__write_reg__583__addr = 0;
    CData/*7:0*/ __Vtask_test_blitter__DOT__write_reg__583__data;
    __Vtask_test_blitter__DOT__write_reg__583__data = 0;
    IData/*31:0*/ __Vtask_test_blitter__DOT__blt_reg__584__off;
    __Vtask_test_blitter__DOT__blt_reg__584__off = 0;
    CData/*7:0*/ __Vtask_test_blitter__DOT__blt_reg__584__data;
    __Vtask_test_blitter__DOT__blt_reg__584__data = 0;
    SData/*15:0*/ __Vtask_test_blitter__DOT__write_reg__585__addr;
    __Vtask_test_blitter__DOT__write_reg__585__addr = 0;
    CData/*7:0*/ __Vtask_test_blitter__DOT__write_reg__585__data;
    __Vtask_test_blitter__DOT__write_reg__585__data = 0;
    IData/*31:0*/ __Vtask_test_blitter__DOT__blt_reg__586__off;
    __Vtask_test_blitter__DOT__blt_reg__586__off = 0;
    CData/*7:0*/ __Vtask_test_blitter__DOT__blt_reg__586__data;
    __Vtask_test_blitter__DOT__blt_reg__586__data = 0;
    SData/*15:0*/ __Vtask_test_blitter__DOT__write_reg__587__addr;
    __Vtask_test_blitter__DOT__write_reg__587__addr = 0;
    CData/*7:0*/ __Vtask_test_blitter__DOT__write_reg__587__data;
    __Vtask_test_blitter__DOT__write_reg__587__data = 0;
    IData/*31:0*/ __Vtask_test_blitter__DOT__blt_reg__589__off;
    __Vtask_test_blitter__DOT__blt_reg__589__off = 0;
    CData/*7:0*/ __Vtask_test_blitter__DOT__blt_reg__589__data;
    __Vtask_test_blitter__DOT__blt_reg__589__data = 0;
    SData/*15:0*/ __Vtask_test_blitter__DOT__write_reg__590__addr;
    __Vtask_test_blitter__DOT__write_reg__590__addr = 0;
    CData/*7:0*/ __Vtask_test_blitter__DOT__write_reg__590__data;
    __Vtask_test_blitter__DOT__write_reg__590__data = 0;
    IData/*31:0*/ __Vtask_test_blitter__DOT__wait_blt_done__591__test_blitter__DOT__unnamedblk1_1__DOT____Vrepeat0;
    __Vtask_test_blitter__DOT__wait_blt_done__591__test_blitter__DOT__unnamedblk1_1__DOT____Vrepeat0 = 0;
    IData/*31:0*/ __Vtask_test_blitter__DOT__wait_blt_done__591__test_blitter__DOT__unnamedblk1_2__DOT____Vrepeat1;
    __Vtask_test_blitter__DOT__wait_blt_done__591__test_blitter__DOT__unnamedblk1_2__DOT____Vrepeat1 = 0;
    CData/*0:0*/ __Vtask_test_blitter__DOT__check__592__condition;
    __Vtask_test_blitter__DOT__check__592__condition = 0;
    CData/*0:0*/ __Vtask_test_blitter__DOT__check__593__condition;
    __Vtask_test_blitter__DOT__check__593__condition = 0;
    IData/*31:0*/ __Vtask_test_blitter__DOT__setup_fill__594__dst_space;
    __Vtask_test_blitter__DOT__setup_fill__594__dst_space = 0;
    IData/*31:0*/ __Vtask_test_blitter__DOT__setup_fill__594__dst_addr;
    __Vtask_test_blitter__DOT__setup_fill__594__dst_addr = 0;
    IData/*31:0*/ __Vtask_test_blitter__DOT__setup_fill__594__dst_stride;
    __Vtask_test_blitter__DOT__setup_fill__594__dst_stride = 0;
    IData/*31:0*/ __Vtask_test_blitter__DOT__setup_fill__594__width;
    __Vtask_test_blitter__DOT__setup_fill__594__width = 0;
    IData/*31:0*/ __Vtask_test_blitter__DOT__setup_fill__594__height;
    __Vtask_test_blitter__DOT__setup_fill__594__height = 0;
    IData/*31:0*/ __Vtask_test_blitter__DOT__setup_fill__594__fill_val;
    __Vtask_test_blitter__DOT__setup_fill__594__fill_val = 0;
    IData/*31:0*/ __Vtask_test_blitter__DOT__blt_reg__595__off;
    __Vtask_test_blitter__DOT__blt_reg__595__off = 0;
    CData/*7:0*/ __Vtask_test_blitter__DOT__blt_reg__595__data;
    __Vtask_test_blitter__DOT__blt_reg__595__data = 0;
    SData/*15:0*/ __Vtask_test_blitter__DOT__write_reg__596__addr;
    __Vtask_test_blitter__DOT__write_reg__596__addr = 0;
    CData/*7:0*/ __Vtask_test_blitter__DOT__write_reg__596__data;
    __Vtask_test_blitter__DOT__write_reg__596__data = 0;
    IData/*31:0*/ __Vtask_test_blitter__DOT__blt_reg__597__off;
    __Vtask_test_blitter__DOT__blt_reg__597__off = 0;
    CData/*7:0*/ __Vtask_test_blitter__DOT__blt_reg__597__data;
    __Vtask_test_blitter__DOT__blt_reg__597__data = 0;
    SData/*15:0*/ __Vtask_test_blitter__DOT__write_reg__598__addr;
    __Vtask_test_blitter__DOT__write_reg__598__addr = 0;
    CData/*7:0*/ __Vtask_test_blitter__DOT__write_reg__598__data;
    __Vtask_test_blitter__DOT__write_reg__598__data = 0;
    IData/*31:0*/ __Vtask_test_blitter__DOT__blt_reg__599__off;
    __Vtask_test_blitter__DOT__blt_reg__599__off = 0;
    CData/*7:0*/ __Vtask_test_blitter__DOT__blt_reg__599__data;
    __Vtask_test_blitter__DOT__blt_reg__599__data = 0;
    SData/*15:0*/ __Vtask_test_blitter__DOT__write_reg__600__addr;
    __Vtask_test_blitter__DOT__write_reg__600__addr = 0;
    CData/*7:0*/ __Vtask_test_blitter__DOT__write_reg__600__data;
    __Vtask_test_blitter__DOT__write_reg__600__data = 0;
    IData/*31:0*/ __Vtask_test_blitter__DOT__blt_reg__601__off;
    __Vtask_test_blitter__DOT__blt_reg__601__off = 0;
    CData/*7:0*/ __Vtask_test_blitter__DOT__blt_reg__601__data;
    __Vtask_test_blitter__DOT__blt_reg__601__data = 0;
    SData/*15:0*/ __Vtask_test_blitter__DOT__write_reg__602__addr;
    __Vtask_test_blitter__DOT__write_reg__602__addr = 0;
    CData/*7:0*/ __Vtask_test_blitter__DOT__write_reg__602__data;
    __Vtask_test_blitter__DOT__write_reg__602__data = 0;
    IData/*31:0*/ __Vtask_test_blitter__DOT__blt_reg__603__off;
    __Vtask_test_blitter__DOT__blt_reg__603__off = 0;
    CData/*7:0*/ __Vtask_test_blitter__DOT__blt_reg__603__data;
    __Vtask_test_blitter__DOT__blt_reg__603__data = 0;
    SData/*15:0*/ __Vtask_test_blitter__DOT__write_reg__604__addr;
    __Vtask_test_blitter__DOT__write_reg__604__addr = 0;
    CData/*7:0*/ __Vtask_test_blitter__DOT__write_reg__604__data;
    __Vtask_test_blitter__DOT__write_reg__604__data = 0;
    IData/*31:0*/ __Vtask_test_blitter__DOT__blt_reg__605__off;
    __Vtask_test_blitter__DOT__blt_reg__605__off = 0;
    CData/*7:0*/ __Vtask_test_blitter__DOT__blt_reg__605__data;
    __Vtask_test_blitter__DOT__blt_reg__605__data = 0;
    SData/*15:0*/ __Vtask_test_blitter__DOT__write_reg__606__addr;
    __Vtask_test_blitter__DOT__write_reg__606__addr = 0;
    CData/*7:0*/ __Vtask_test_blitter__DOT__write_reg__606__data;
    __Vtask_test_blitter__DOT__write_reg__606__data = 0;
    IData/*31:0*/ __Vtask_test_blitter__DOT__blt_reg__607__off;
    __Vtask_test_blitter__DOT__blt_reg__607__off = 0;
    CData/*7:0*/ __Vtask_test_blitter__DOT__blt_reg__607__data;
    __Vtask_test_blitter__DOT__blt_reg__607__data = 0;
    SData/*15:0*/ __Vtask_test_blitter__DOT__write_reg__608__addr;
    __Vtask_test_blitter__DOT__write_reg__608__addr = 0;
    CData/*7:0*/ __Vtask_test_blitter__DOT__write_reg__608__data;
    __Vtask_test_blitter__DOT__write_reg__608__data = 0;
    IData/*31:0*/ __Vtask_test_blitter__DOT__blt_reg__609__off;
    __Vtask_test_blitter__DOT__blt_reg__609__off = 0;
    CData/*7:0*/ __Vtask_test_blitter__DOT__blt_reg__609__data;
    __Vtask_test_blitter__DOT__blt_reg__609__data = 0;
    SData/*15:0*/ __Vtask_test_blitter__DOT__write_reg__610__addr;
    __Vtask_test_blitter__DOT__write_reg__610__addr = 0;
    CData/*7:0*/ __Vtask_test_blitter__DOT__write_reg__610__data;
    __Vtask_test_blitter__DOT__write_reg__610__data = 0;
    IData/*31:0*/ __Vtask_test_blitter__DOT__blt_reg__611__off;
    __Vtask_test_blitter__DOT__blt_reg__611__off = 0;
    CData/*7:0*/ __Vtask_test_blitter__DOT__blt_reg__611__data;
    __Vtask_test_blitter__DOT__blt_reg__611__data = 0;
    SData/*15:0*/ __Vtask_test_blitter__DOT__write_reg__612__addr;
    __Vtask_test_blitter__DOT__write_reg__612__addr = 0;
    CData/*7:0*/ __Vtask_test_blitter__DOT__write_reg__612__data;
    __Vtask_test_blitter__DOT__write_reg__612__data = 0;
    IData/*31:0*/ __Vtask_test_blitter__DOT__blt_reg__613__off;
    __Vtask_test_blitter__DOT__blt_reg__613__off = 0;
    CData/*7:0*/ __Vtask_test_blitter__DOT__blt_reg__613__data;
    __Vtask_test_blitter__DOT__blt_reg__613__data = 0;
    SData/*15:0*/ __Vtask_test_blitter__DOT__write_reg__614__addr;
    __Vtask_test_blitter__DOT__write_reg__614__addr = 0;
    CData/*7:0*/ __Vtask_test_blitter__DOT__write_reg__614__data;
    __Vtask_test_blitter__DOT__write_reg__614__data = 0;
    IData/*31:0*/ __Vtask_test_blitter__DOT__blt_reg__615__off;
    __Vtask_test_blitter__DOT__blt_reg__615__off = 0;
    CData/*7:0*/ __Vtask_test_blitter__DOT__blt_reg__615__data;
    __Vtask_test_blitter__DOT__blt_reg__615__data = 0;
    SData/*15:0*/ __Vtask_test_blitter__DOT__write_reg__616__addr;
    __Vtask_test_blitter__DOT__write_reg__616__addr = 0;
    CData/*7:0*/ __Vtask_test_blitter__DOT__write_reg__616__data;
    __Vtask_test_blitter__DOT__write_reg__616__data = 0;
    IData/*31:0*/ __Vtask_test_blitter__DOT__blt_reg__617__off;
    __Vtask_test_blitter__DOT__blt_reg__617__off = 0;
    CData/*7:0*/ __Vtask_test_blitter__DOT__blt_reg__617__data;
    __Vtask_test_blitter__DOT__blt_reg__617__data = 0;
    SData/*15:0*/ __Vtask_test_blitter__DOT__write_reg__618__addr;
    __Vtask_test_blitter__DOT__write_reg__618__addr = 0;
    CData/*7:0*/ __Vtask_test_blitter__DOT__write_reg__618__data;
    __Vtask_test_blitter__DOT__write_reg__618__data = 0;
    IData/*31:0*/ __Vtask_test_blitter__DOT__blt_reg__620__off;
    __Vtask_test_blitter__DOT__blt_reg__620__off = 0;
    CData/*7:0*/ __Vtask_test_blitter__DOT__blt_reg__620__data;
    __Vtask_test_blitter__DOT__blt_reg__620__data = 0;
    SData/*15:0*/ __Vtask_test_blitter__DOT__write_reg__621__addr;
    __Vtask_test_blitter__DOT__write_reg__621__addr = 0;
    CData/*7:0*/ __Vtask_test_blitter__DOT__write_reg__621__data;
    __Vtask_test_blitter__DOT__write_reg__621__data = 0;
    IData/*31:0*/ __Vtask_test_blitter__DOT__wait_blt_done__622__test_blitter__DOT__unnamedblk1_1__DOT____Vrepeat0;
    __Vtask_test_blitter__DOT__wait_blt_done__622__test_blitter__DOT__unnamedblk1_1__DOT____Vrepeat0 = 0;
    IData/*31:0*/ __Vtask_test_blitter__DOT__wait_blt_done__622__test_blitter__DOT__unnamedblk1_2__DOT____Vrepeat1;
    __Vtask_test_blitter__DOT__wait_blt_done__622__test_blitter__DOT__unnamedblk1_2__DOT____Vrepeat1 = 0;
    CData/*0:0*/ __Vtask_test_blitter__DOT__check__623__condition;
    __Vtask_test_blitter__DOT__check__623__condition = 0;
    CData/*0:0*/ __Vtask_test_blitter__DOT__check__624__condition;
    __Vtask_test_blitter__DOT__check__624__condition = 0;
    IData/*31:0*/ __Vtask_test_blitter__DOT__setup_copy__625__src_space;
    __Vtask_test_blitter__DOT__setup_copy__625__src_space = 0;
    IData/*31:0*/ __Vtask_test_blitter__DOT__setup_copy__625__src_addr;
    __Vtask_test_blitter__DOT__setup_copy__625__src_addr = 0;
    IData/*31:0*/ __Vtask_test_blitter__DOT__setup_copy__625__src_stride;
    __Vtask_test_blitter__DOT__setup_copy__625__src_stride = 0;
    IData/*31:0*/ __Vtask_test_blitter__DOT__setup_copy__625__dst_space;
    __Vtask_test_blitter__DOT__setup_copy__625__dst_space = 0;
    IData/*31:0*/ __Vtask_test_blitter__DOT__setup_copy__625__dst_addr;
    __Vtask_test_blitter__DOT__setup_copy__625__dst_addr = 0;
    IData/*31:0*/ __Vtask_test_blitter__DOT__setup_copy__625__dst_stride;
    __Vtask_test_blitter__DOT__setup_copy__625__dst_stride = 0;
    IData/*31:0*/ __Vtask_test_blitter__DOT__setup_copy__625__width;
    __Vtask_test_blitter__DOT__setup_copy__625__width = 0;
    IData/*31:0*/ __Vtask_test_blitter__DOT__setup_copy__625__height;
    __Vtask_test_blitter__DOT__setup_copy__625__height = 0;
    IData/*31:0*/ __Vtask_test_blitter__DOT__blt_reg__626__off;
    __Vtask_test_blitter__DOT__blt_reg__626__off = 0;
    CData/*7:0*/ __Vtask_test_blitter__DOT__blt_reg__626__data;
    __Vtask_test_blitter__DOT__blt_reg__626__data = 0;
    SData/*15:0*/ __Vtask_test_blitter__DOT__write_reg__627__addr;
    __Vtask_test_blitter__DOT__write_reg__627__addr = 0;
    CData/*7:0*/ __Vtask_test_blitter__DOT__write_reg__627__data;
    __Vtask_test_blitter__DOT__write_reg__627__data = 0;
    IData/*31:0*/ __Vtask_test_blitter__DOT__blt_reg__628__off;
    __Vtask_test_blitter__DOT__blt_reg__628__off = 0;
    CData/*7:0*/ __Vtask_test_blitter__DOT__blt_reg__628__data;
    __Vtask_test_blitter__DOT__blt_reg__628__data = 0;
    SData/*15:0*/ __Vtask_test_blitter__DOT__write_reg__629__addr;
    __Vtask_test_blitter__DOT__write_reg__629__addr = 0;
    CData/*7:0*/ __Vtask_test_blitter__DOT__write_reg__629__data;
    __Vtask_test_blitter__DOT__write_reg__629__data = 0;
    IData/*31:0*/ __Vtask_test_blitter__DOT__blt_reg__630__off;
    __Vtask_test_blitter__DOT__blt_reg__630__off = 0;
    CData/*7:0*/ __Vtask_test_blitter__DOT__blt_reg__630__data;
    __Vtask_test_blitter__DOT__blt_reg__630__data = 0;
    SData/*15:0*/ __Vtask_test_blitter__DOT__write_reg__631__addr;
    __Vtask_test_blitter__DOT__write_reg__631__addr = 0;
    CData/*7:0*/ __Vtask_test_blitter__DOT__write_reg__631__data;
    __Vtask_test_blitter__DOT__write_reg__631__data = 0;
    IData/*31:0*/ __Vtask_test_blitter__DOT__blt_reg__632__off;
    __Vtask_test_blitter__DOT__blt_reg__632__off = 0;
    CData/*7:0*/ __Vtask_test_blitter__DOT__blt_reg__632__data;
    __Vtask_test_blitter__DOT__blt_reg__632__data = 0;
    SData/*15:0*/ __Vtask_test_blitter__DOT__write_reg__633__addr;
    __Vtask_test_blitter__DOT__write_reg__633__addr = 0;
    CData/*7:0*/ __Vtask_test_blitter__DOT__write_reg__633__data;
    __Vtask_test_blitter__DOT__write_reg__633__data = 0;
    IData/*31:0*/ __Vtask_test_blitter__DOT__blt_reg__634__off;
    __Vtask_test_blitter__DOT__blt_reg__634__off = 0;
    CData/*7:0*/ __Vtask_test_blitter__DOT__blt_reg__634__data;
    __Vtask_test_blitter__DOT__blt_reg__634__data = 0;
    SData/*15:0*/ __Vtask_test_blitter__DOT__write_reg__635__addr;
    __Vtask_test_blitter__DOT__write_reg__635__addr = 0;
    CData/*7:0*/ __Vtask_test_blitter__DOT__write_reg__635__data;
    __Vtask_test_blitter__DOT__write_reg__635__data = 0;
    IData/*31:0*/ __Vtask_test_blitter__DOT__blt_reg__636__off;
    __Vtask_test_blitter__DOT__blt_reg__636__off = 0;
    CData/*7:0*/ __Vtask_test_blitter__DOT__blt_reg__636__data;
    __Vtask_test_blitter__DOT__blt_reg__636__data = 0;
    SData/*15:0*/ __Vtask_test_blitter__DOT__write_reg__637__addr;
    __Vtask_test_blitter__DOT__write_reg__637__addr = 0;
    CData/*7:0*/ __Vtask_test_blitter__DOT__write_reg__637__data;
    __Vtask_test_blitter__DOT__write_reg__637__data = 0;
    IData/*31:0*/ __Vtask_test_blitter__DOT__blt_reg__638__off;
    __Vtask_test_blitter__DOT__blt_reg__638__off = 0;
    CData/*7:0*/ __Vtask_test_blitter__DOT__blt_reg__638__data;
    __Vtask_test_blitter__DOT__blt_reg__638__data = 0;
    SData/*15:0*/ __Vtask_test_blitter__DOT__write_reg__639__addr;
    __Vtask_test_blitter__DOT__write_reg__639__addr = 0;
    CData/*7:0*/ __Vtask_test_blitter__DOT__write_reg__639__data;
    __Vtask_test_blitter__DOT__write_reg__639__data = 0;
    IData/*31:0*/ __Vtask_test_blitter__DOT__blt_reg__640__off;
    __Vtask_test_blitter__DOT__blt_reg__640__off = 0;
    CData/*7:0*/ __Vtask_test_blitter__DOT__blt_reg__640__data;
    __Vtask_test_blitter__DOT__blt_reg__640__data = 0;
    SData/*15:0*/ __Vtask_test_blitter__DOT__write_reg__641__addr;
    __Vtask_test_blitter__DOT__write_reg__641__addr = 0;
    CData/*7:0*/ __Vtask_test_blitter__DOT__write_reg__641__data;
    __Vtask_test_blitter__DOT__write_reg__641__data = 0;
    IData/*31:0*/ __Vtask_test_blitter__DOT__blt_reg__642__off;
    __Vtask_test_blitter__DOT__blt_reg__642__off = 0;
    CData/*7:0*/ __Vtask_test_blitter__DOT__blt_reg__642__data;
    __Vtask_test_blitter__DOT__blt_reg__642__data = 0;
    SData/*15:0*/ __Vtask_test_blitter__DOT__write_reg__643__addr;
    __Vtask_test_blitter__DOT__write_reg__643__addr = 0;
    CData/*7:0*/ __Vtask_test_blitter__DOT__write_reg__643__data;
    __Vtask_test_blitter__DOT__write_reg__643__data = 0;
    IData/*31:0*/ __Vtask_test_blitter__DOT__blt_reg__644__off;
    __Vtask_test_blitter__DOT__blt_reg__644__off = 0;
    CData/*7:0*/ __Vtask_test_blitter__DOT__blt_reg__644__data;
    __Vtask_test_blitter__DOT__blt_reg__644__data = 0;
    SData/*15:0*/ __Vtask_test_blitter__DOT__write_reg__645__addr;
    __Vtask_test_blitter__DOT__write_reg__645__addr = 0;
    CData/*7:0*/ __Vtask_test_blitter__DOT__write_reg__645__data;
    __Vtask_test_blitter__DOT__write_reg__645__data = 0;
    IData/*31:0*/ __Vtask_test_blitter__DOT__blt_reg__646__off;
    __Vtask_test_blitter__DOT__blt_reg__646__off = 0;
    CData/*7:0*/ __Vtask_test_blitter__DOT__blt_reg__646__data;
    __Vtask_test_blitter__DOT__blt_reg__646__data = 0;
    SData/*15:0*/ __Vtask_test_blitter__DOT__write_reg__647__addr;
    __Vtask_test_blitter__DOT__write_reg__647__addr = 0;
    CData/*7:0*/ __Vtask_test_blitter__DOT__write_reg__647__data;
    __Vtask_test_blitter__DOT__write_reg__647__data = 0;
    IData/*31:0*/ __Vtask_test_blitter__DOT__blt_reg__648__off;
    __Vtask_test_blitter__DOT__blt_reg__648__off = 0;
    CData/*7:0*/ __Vtask_test_blitter__DOT__blt_reg__648__data;
    __Vtask_test_blitter__DOT__blt_reg__648__data = 0;
    SData/*15:0*/ __Vtask_test_blitter__DOT__write_reg__649__addr;
    __Vtask_test_blitter__DOT__write_reg__649__addr = 0;
    CData/*7:0*/ __Vtask_test_blitter__DOT__write_reg__649__data;
    __Vtask_test_blitter__DOT__write_reg__649__data = 0;
    IData/*31:0*/ __Vtask_test_blitter__DOT__blt_reg__650__off;
    __Vtask_test_blitter__DOT__blt_reg__650__off = 0;
    CData/*7:0*/ __Vtask_test_blitter__DOT__blt_reg__650__data;
    __Vtask_test_blitter__DOT__blt_reg__650__data = 0;
    SData/*15:0*/ __Vtask_test_blitter__DOT__write_reg__651__addr;
    __Vtask_test_blitter__DOT__write_reg__651__addr = 0;
    CData/*7:0*/ __Vtask_test_blitter__DOT__write_reg__651__data;
    __Vtask_test_blitter__DOT__write_reg__651__data = 0;
    IData/*31:0*/ __Vtask_test_blitter__DOT__blt_reg__652__off;
    __Vtask_test_blitter__DOT__blt_reg__652__off = 0;
    CData/*7:0*/ __Vtask_test_blitter__DOT__blt_reg__652__data;
    __Vtask_test_blitter__DOT__blt_reg__652__data = 0;
    SData/*15:0*/ __Vtask_test_blitter__DOT__write_reg__653__addr;
    __Vtask_test_blitter__DOT__write_reg__653__addr = 0;
    CData/*7:0*/ __Vtask_test_blitter__DOT__write_reg__653__data;
    __Vtask_test_blitter__DOT__write_reg__653__data = 0;
    IData/*31:0*/ __Vtask_test_blitter__DOT__blt_reg__654__off;
    __Vtask_test_blitter__DOT__blt_reg__654__off = 0;
    CData/*7:0*/ __Vtask_test_blitter__DOT__blt_reg__654__data;
    __Vtask_test_blitter__DOT__blt_reg__654__data = 0;
    SData/*15:0*/ __Vtask_test_blitter__DOT__write_reg__655__addr;
    __Vtask_test_blitter__DOT__write_reg__655__addr = 0;
    CData/*7:0*/ __Vtask_test_blitter__DOT__write_reg__655__data;
    __Vtask_test_blitter__DOT__write_reg__655__data = 0;
    IData/*31:0*/ __Vtask_test_blitter__DOT__blt_reg__656__off;
    __Vtask_test_blitter__DOT__blt_reg__656__off = 0;
    CData/*7:0*/ __Vtask_test_blitter__DOT__blt_reg__656__data;
    __Vtask_test_blitter__DOT__blt_reg__656__data = 0;
    SData/*15:0*/ __Vtask_test_blitter__DOT__write_reg__657__addr;
    __Vtask_test_blitter__DOT__write_reg__657__addr = 0;
    CData/*7:0*/ __Vtask_test_blitter__DOT__write_reg__657__data;
    __Vtask_test_blitter__DOT__write_reg__657__data = 0;
    IData/*31:0*/ __Vtask_test_blitter__DOT__blt_reg__658__off;
    __Vtask_test_blitter__DOT__blt_reg__658__off = 0;
    CData/*7:0*/ __Vtask_test_blitter__DOT__blt_reg__658__data;
    __Vtask_test_blitter__DOT__blt_reg__658__data = 0;
    SData/*15:0*/ __Vtask_test_blitter__DOT__write_reg__659__addr;
    __Vtask_test_blitter__DOT__write_reg__659__addr = 0;
    CData/*7:0*/ __Vtask_test_blitter__DOT__write_reg__659__data;
    __Vtask_test_blitter__DOT__write_reg__659__data = 0;
    IData/*31:0*/ __Vtask_test_blitter__DOT__blt_reg__660__off;
    __Vtask_test_blitter__DOT__blt_reg__660__off = 0;
    CData/*7:0*/ __Vtask_test_blitter__DOT__blt_reg__660__data;
    __Vtask_test_blitter__DOT__blt_reg__660__data = 0;
    SData/*15:0*/ __Vtask_test_blitter__DOT__write_reg__661__addr;
    __Vtask_test_blitter__DOT__write_reg__661__addr = 0;
    // Body
    VL_WRITEF_NX("=== Blitter Unit Tests ===\n\n",0);
    vlSelfRef.test_blitter__DOT__rst = 1U;
    vlSelfRef.test_blitter__DOT__cpu_we = 0U;
    vlSelfRef.test_blitter__DOT__cpu_addr = 0U;
    vlSelfRef.test_blitter__DOT__cpu_wdata = 0U;
    test_blitter__DOT__unnamedblk1__DOT__i = 0U;
    while (VL_GTS_III(32, 0x00010000U, test_blitter__DOT__unnamedblk1__DOT__i)) {
        vlSelfRef.test_blitter__DOT__sim_ram[(0x0000ffffU 
                                              & test_blitter__DOT__unnamedblk1__DOT__i)] = 0U;
        test_blitter__DOT__unnamedblk1__DOT__i = ((IData)(1U) 
                                                  + test_blitter__DOT__unnamedblk1__DOT__i);
    }
    test_blitter__DOT__unnamedblk2__DOT__i = 0U;
    while (VL_GTS_III(32, 0x000007d0U, test_blitter__DOT__unnamedblk2__DOT__i)) {
        vlSelfRef.test_blitter__DOT____Vlvbound_h7d2f9654__0 = 0U;
        vlSelfRef.test_blitter__DOT____Vlvbound_h22a4cf02__0 = 0U;
        if (VL_LIKELY(((0x07cfU >= (0x000007ffU & test_blitter__DOT__unnamedblk2__DOT__i))))) {
            vlSelfRef.test_blitter__DOT__sim_char[(0x000007ffU 
                                                   & test_blitter__DOT__unnamedblk2__DOT__i)] 
                = vlSelfRef.test_blitter__DOT____Vlvbound_h7d2f9654__0;
            vlSelfRef.test_blitter__DOT__sim_color[(0x000007ffU 
                                                    & test_blitter__DOT__unnamedblk2__DOT__i)] 
                = vlSelfRef.test_blitter__DOT____Vlvbound_h22a4cf02__0;
        }
        test_blitter__DOT__unnamedblk2__DOT__i = ((IData)(1U) 
                                                  + test_blitter__DOT__unnamedblk2__DOT__i);
    }
    test_blitter__DOT__unnamedblk3__DOT__i = 0U;
    while (VL_GTS_III(32, 0x0000fa00U, test_blitter__DOT__unnamedblk3__DOT__i)) {
        vlSelfRef.test_blitter__DOT____Vlvbound_h321272e9__0 = 0U;
        if (VL_LIKELY(((0xf9ffU >= (0x0000ffffU & test_blitter__DOT__unnamedblk3__DOT__i))))) {
            vlSelfRef.test_blitter__DOT__sim_gfx[(0x0000ffffU 
                                                  & test_blitter__DOT__unnamedblk3__DOT__i)] 
                = vlSelfRef.test_blitter__DOT____Vlvbound_h321272e9__0;
        }
        test_blitter__DOT__unnamedblk3__DOT__i = ((IData)(1U) 
                                                  + test_blitter__DOT__unnamedblk3__DOT__i);
    }
    test_blitter__DOT__unnamedblk4__DOT__i = 0U;
    while (VL_GTS_III(32, 0x00008000U, test_blitter__DOT__unnamedblk4__DOT__i)) {
        vlSelfRef.test_blitter__DOT__sim_sprite[(0x00007fffU 
                                                 & test_blitter__DOT__unnamedblk4__DOT__i)] = 0U;
        test_blitter__DOT__unnamedblk4__DOT__i = ((IData)(1U) 
                                                  + test_blitter__DOT__unnamedblk4__DOT__i);
    }
    test_blitter__DOT__unnamedblk1_3__DOT____Vrepeat2 = 0x00000032U;
    while (VL_LTS_III(32, 0U, test_blitter__DOT__unnamedblk1_3__DOT____Vrepeat2)) {
        Vtest_blitter___024root____VbeforeTrig_h603fc354__0(vlSelf, 
                                                            "@(posedge test_blitter.clk)");
        co_await vlSelfRef.__VtrigSched_h603fc354__0.trigger(0U, 
                                                             nullptr, 
                                                             "@(posedge test_blitter.clk)", 
                                                             "test_blitter.sv", 
                                                             204);
        test_blitter__DOT__unnamedblk1_3__DOT____Vrepeat2 
            = (test_blitter__DOT__unnamedblk1_3__DOT____Vrepeat2 
               - (IData)(1U));
    }
    vlSelfRef.test_blitter__DOT__rst = 0U;
    test_blitter__DOT__unnamedblk1_4__DOT____Vrepeat3 = 0x0000000aU;
    while (VL_LTS_III(32, 0U, test_blitter__DOT__unnamedblk1_4__DOT____Vrepeat3)) {
        Vtest_blitter___024root____VbeforeTrig_h603fc354__0(vlSelf, 
                                                            "@(posedge test_blitter.clk)");
        co_await vlSelfRef.__VtrigSched_h603fc354__0.trigger(0U, 
                                                             nullptr, 
                                                             "@(posedge test_blitter.clk)", 
                                                             "test_blitter.sv", 
                                                             206);
        test_blitter__DOT__unnamedblk1_4__DOT____Vrepeat3 
            = (test_blitter__DOT__unnamedblk1_4__DOT____Vrepeat3 
               - (IData)(1U));
    }
    VL_WRITEF_NX("Test: Initial state\n",0);
    __Vtask_test_blitter__DOT__check__0__condition 
        = (0U == vlSelfRef.test_blitter__DOT__dut__DOT__regs[1U]);
    vlSelfRef.__Vtask_test_blitter__DOT__check__0__name = "status is idle (0)"s;
    vlSelfRef.test_blitter__DOT__test_num = ((IData)(1U) 
                                             + vlSelfRef.test_blitter__DOT__test_num);
    if (__Vtask_test_blitter__DOT__check__0__condition) {
        VL_WRITEF_NX("  PASS [%0d] %@\n",0,32,vlSelfRef.test_blitter__DOT__test_num,
                     -1,&(vlSelfRef.__Vtask_test_blitter__DOT__check__0__name));
        vlSelfRef.test_blitter__DOT__pass_count = ((IData)(1U) 
                                                   + vlSelfRef.test_blitter__DOT__pass_count);
    } else {
        VL_WRITEF_NX("  FAIL [%0d] %@\n",0,32,vlSelfRef.test_blitter__DOT__test_num,
                     -1,&(vlSelfRef.__Vtask_test_blitter__DOT__check__0__name));
        vlSelfRef.test_blitter__DOT__fail_count = ((IData)(1U) 
                                                   + vlSelfRef.test_blitter__DOT__fail_count);
    }
    __Vtask_test_blitter__DOT__check__1__condition 
        = ((0U == (IData)(vlSelfRef.test_blitter__DOT__dut__DOT__state)) 
           | (6U == (IData)(vlSelfRef.test_blitter__DOT__dut__DOT__state)));
    vlSelfRef.__Vtask_test_blitter__DOT__check__1__name = "RDY is high"s;
    vlSelfRef.test_blitter__DOT__test_num = ((IData)(1U) 
                                             + vlSelfRef.test_blitter__DOT__test_num);
    if (__Vtask_test_blitter__DOT__check__1__condition) {
        VL_WRITEF_NX("  PASS [%0d] %@\n",0,32,vlSelfRef.test_blitter__DOT__test_num,
                     -1,&(vlSelfRef.__Vtask_test_blitter__DOT__check__1__name));
        vlSelfRef.test_blitter__DOT__pass_count = ((IData)(1U) 
                                                   + vlSelfRef.test_blitter__DOT__pass_count);
    } else {
        VL_WRITEF_NX("  FAIL [%0d] %@\n",0,32,vlSelfRef.test_blitter__DOT__test_num,
                     -1,&(vlSelfRef.__Vtask_test_blitter__DOT__check__1__name));
        vlSelfRef.test_blitter__DOT__fail_count = ((IData)(1U) 
                                                   + vlSelfRef.test_blitter__DOT__fail_count);
    }
    VL_WRITEF_NX("Test: Fill RAM\n",0);
    __Vtask_test_blitter__DOT__setup_fill__2__fill_val = 0x000000aaU;
    __Vtask_test_blitter__DOT__setup_fill__2__height = 3U;
    __Vtask_test_blitter__DOT__setup_fill__2__width = 5U;
    __Vtask_test_blitter__DOT__setup_fill__2__dst_stride = 0x0000000aU;
    __Vtask_test_blitter__DOT__setup_fill__2__dst_addr = 0x00001000U;
    __Vtask_test_blitter__DOT__setup_fill__2__dst_space = 0U;
    __Vtask_test_blitter__DOT__blt_reg__3__data = (0x000000ffU 
                                                   & __Vtask_test_blitter__DOT__setup_fill__2__dst_space);
    __Vtask_test_blitter__DOT__blt_reg__3__off = 4U;
    __Vtask_test_blitter__DOT__write_reg__4__data = __Vtask_test_blitter__DOT__blt_reg__3__data;
    __Vtask_test_blitter__DOT__write_reg__4__addr = 
        (0x0000ffffU & ((IData)(0xba83U) + __Vtask_test_blitter__DOT__blt_reg__3__off));
    Vtest_blitter___024root____VbeforeTrig_h603fc354__0(vlSelf, 
                                                        "@(posedge test_blitter.clk)");
    co_await vlSelfRef.__VtrigSched_h603fc354__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge test_blitter.clk)", 
                                                         "test_blitter.sv", 
                                                         113);
    vlSelfRef.test_blitter__DOT__cpu_addr = __Vtask_test_blitter__DOT__write_reg__4__addr;
    vlSelfRef.test_blitter__DOT__cpu_wdata = __Vtask_test_blitter__DOT__write_reg__4__data;
    vlSelfRef.test_blitter__DOT__cpu_we = 1U;
    Vtest_blitter___024root____VbeforeTrig_h603fc354__0(vlSelf, 
                                                        "@(posedge test_blitter.clk)");
    co_await vlSelfRef.__VtrigSched_h603fc354__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge test_blitter.clk)", 
                                                         "test_blitter.sv", 
                                                         115);
    vlSelfRef.test_blitter__DOT__cpu_we = 0U;
    __Vtask_test_blitter__DOT__blt_reg__5__data = (0x000000ffU 
                                                   & __Vtask_test_blitter__DOT__setup_fill__2__dst_addr);
    __Vtask_test_blitter__DOT__blt_reg__5__off = 8U;
    __Vtask_test_blitter__DOT__write_reg__6__data = __Vtask_test_blitter__DOT__blt_reg__5__data;
    __Vtask_test_blitter__DOT__write_reg__6__addr = 
        (0x0000ffffU & ((IData)(0xba83U) + __Vtask_test_blitter__DOT__blt_reg__5__off));
    Vtest_blitter___024root____VbeforeTrig_h603fc354__0(vlSelf, 
                                                        "@(posedge test_blitter.clk)");
    co_await vlSelfRef.__VtrigSched_h603fc354__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge test_blitter.clk)", 
                                                         "test_blitter.sv", 
                                                         113);
    vlSelfRef.test_blitter__DOT__cpu_addr = __Vtask_test_blitter__DOT__write_reg__6__addr;
    vlSelfRef.test_blitter__DOT__cpu_wdata = __Vtask_test_blitter__DOT__write_reg__6__data;
    vlSelfRef.test_blitter__DOT__cpu_we = 1U;
    Vtest_blitter___024root____VbeforeTrig_h603fc354__0(vlSelf, 
                                                        "@(posedge test_blitter.clk)");
    co_await vlSelfRef.__VtrigSched_h603fc354__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge test_blitter.clk)", 
                                                         "test_blitter.sv", 
                                                         115);
    vlSelfRef.test_blitter__DOT__cpu_we = 0U;
    __Vtask_test_blitter__DOT__blt_reg__7__data = (0x000000ffU 
                                                   & (__Vtask_test_blitter__DOT__setup_fill__2__dst_addr 
                                                      >> 8U));
    __Vtask_test_blitter__DOT__blt_reg__7__off = 9U;
    __Vtask_test_blitter__DOT__write_reg__8__data = __Vtask_test_blitter__DOT__blt_reg__7__data;
    __Vtask_test_blitter__DOT__write_reg__8__addr = 
        (0x0000ffffU & ((IData)(0xba83U) + __Vtask_test_blitter__DOT__blt_reg__7__off));
    Vtest_blitter___024root____VbeforeTrig_h603fc354__0(vlSelf, 
                                                        "@(posedge test_blitter.clk)");
    co_await vlSelfRef.__VtrigSched_h603fc354__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge test_blitter.clk)", 
                                                         "test_blitter.sv", 
                                                         113);
    vlSelfRef.test_blitter__DOT__cpu_addr = __Vtask_test_blitter__DOT__write_reg__8__addr;
    vlSelfRef.test_blitter__DOT__cpu_wdata = __Vtask_test_blitter__DOT__write_reg__8__data;
    vlSelfRef.test_blitter__DOT__cpu_we = 1U;
    Vtest_blitter___024root____VbeforeTrig_h603fc354__0(vlSelf, 
                                                        "@(posedge test_blitter.clk)");
    co_await vlSelfRef.__VtrigSched_h603fc354__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge test_blitter.clk)", 
                                                         "test_blitter.sv", 
                                                         115);
    vlSelfRef.test_blitter__DOT__cpu_we = 0U;
    __Vtask_test_blitter__DOT__blt_reg__9__data = (0x000000ffU 
                                                   & (__Vtask_test_blitter__DOT__setup_fill__2__dst_addr 
                                                      >> 0x10U));
    __Vtask_test_blitter__DOT__blt_reg__9__off = 0x0000000aU;
    __Vtask_test_blitter__DOT__write_reg__10__data 
        = __Vtask_test_blitter__DOT__blt_reg__9__data;
    __Vtask_test_blitter__DOT__write_reg__10__addr 
        = (0x0000ffffU & ((IData)(0xba83U) + __Vtask_test_blitter__DOT__blt_reg__9__off));
    Vtest_blitter___024root____VbeforeTrig_h603fc354__0(vlSelf, 
                                                        "@(posedge test_blitter.clk)");
    co_await vlSelfRef.__VtrigSched_h603fc354__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge test_blitter.clk)", 
                                                         "test_blitter.sv", 
                                                         113);
    vlSelfRef.test_blitter__DOT__cpu_addr = __Vtask_test_blitter__DOT__write_reg__10__addr;
    vlSelfRef.test_blitter__DOT__cpu_wdata = __Vtask_test_blitter__DOT__write_reg__10__data;
    vlSelfRef.test_blitter__DOT__cpu_we = 1U;
    Vtest_blitter___024root____VbeforeTrig_h603fc354__0(vlSelf, 
                                                        "@(posedge test_blitter.clk)");
    co_await vlSelfRef.__VtrigSched_h603fc354__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge test_blitter.clk)", 
                                                         "test_blitter.sv", 
                                                         115);
    vlSelfRef.test_blitter__DOT__cpu_we = 0U;
    __Vtask_test_blitter__DOT__blt_reg__11__data = 
        (0x000000ffU & __Vtask_test_blitter__DOT__setup_fill__2__width);
    __Vtask_test_blitter__DOT__blt_reg__11__off = 0x0000000bU;
    __Vtask_test_blitter__DOT__write_reg__12__data 
        = __Vtask_test_blitter__DOT__blt_reg__11__data;
    __Vtask_test_blitter__DOT__write_reg__12__addr 
        = (0x0000ffffU & ((IData)(0xba83U) + __Vtask_test_blitter__DOT__blt_reg__11__off));
    Vtest_blitter___024root____VbeforeTrig_h603fc354__0(vlSelf, 
                                                        "@(posedge test_blitter.clk)");
    co_await vlSelfRef.__VtrigSched_h603fc354__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge test_blitter.clk)", 
                                                         "test_blitter.sv", 
                                                         113);
    vlSelfRef.test_blitter__DOT__cpu_addr = __Vtask_test_blitter__DOT__write_reg__12__addr;
    vlSelfRef.test_blitter__DOT__cpu_wdata = __Vtask_test_blitter__DOT__write_reg__12__data;
    vlSelfRef.test_blitter__DOT__cpu_we = 1U;
    Vtest_blitter___024root____VbeforeTrig_h603fc354__0(vlSelf, 
                                                        "@(posedge test_blitter.clk)");
    co_await vlSelfRef.__VtrigSched_h603fc354__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge test_blitter.clk)", 
                                                         "test_blitter.sv", 
                                                         115);
    vlSelfRef.test_blitter__DOT__cpu_we = 0U;
    __Vtask_test_blitter__DOT__blt_reg__13__data = 
        (0x000000ffU & (__Vtask_test_blitter__DOT__setup_fill__2__width 
                        >> 8U));
    __Vtask_test_blitter__DOT__blt_reg__13__off = 0x0000000cU;
    __Vtask_test_blitter__DOT__write_reg__14__data 
        = __Vtask_test_blitter__DOT__blt_reg__13__data;
    __Vtask_test_blitter__DOT__write_reg__14__addr 
        = (0x0000ffffU & ((IData)(0xba83U) + __Vtask_test_blitter__DOT__blt_reg__13__off));
    Vtest_blitter___024root____VbeforeTrig_h603fc354__0(vlSelf, 
                                                        "@(posedge test_blitter.clk)");
    co_await vlSelfRef.__VtrigSched_h603fc354__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge test_blitter.clk)", 
                                                         "test_blitter.sv", 
                                                         113);
    vlSelfRef.test_blitter__DOT__cpu_addr = __Vtask_test_blitter__DOT__write_reg__14__addr;
    vlSelfRef.test_blitter__DOT__cpu_wdata = __Vtask_test_blitter__DOT__write_reg__14__data;
    vlSelfRef.test_blitter__DOT__cpu_we = 1U;
    Vtest_blitter___024root____VbeforeTrig_h603fc354__0(vlSelf, 
                                                        "@(posedge test_blitter.clk)");
    co_await vlSelfRef.__VtrigSched_h603fc354__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge test_blitter.clk)", 
                                                         "test_blitter.sv", 
                                                         115);
    vlSelfRef.test_blitter__DOT__cpu_we = 0U;
    __Vtask_test_blitter__DOT__blt_reg__15__data = 
        (0x000000ffU & __Vtask_test_blitter__DOT__setup_fill__2__height);
    __Vtask_test_blitter__DOT__blt_reg__15__off = 0x0000000dU;
    __Vtask_test_blitter__DOT__write_reg__16__data 
        = __Vtask_test_blitter__DOT__blt_reg__15__data;
    __Vtask_test_blitter__DOT__write_reg__16__addr 
        = (0x0000ffffU & ((IData)(0xba83U) + __Vtask_test_blitter__DOT__blt_reg__15__off));
    Vtest_blitter___024root____VbeforeTrig_h603fc354__0(vlSelf, 
                                                        "@(posedge test_blitter.clk)");
    co_await vlSelfRef.__VtrigSched_h603fc354__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge test_blitter.clk)", 
                                                         "test_blitter.sv", 
                                                         113);
    vlSelfRef.test_blitter__DOT__cpu_addr = __Vtask_test_blitter__DOT__write_reg__16__addr;
    vlSelfRef.test_blitter__DOT__cpu_wdata = __Vtask_test_blitter__DOT__write_reg__16__data;
    vlSelfRef.test_blitter__DOT__cpu_we = 1U;
    Vtest_blitter___024root____VbeforeTrig_h603fc354__0(vlSelf, 
                                                        "@(posedge test_blitter.clk)");
    co_await vlSelfRef.__VtrigSched_h603fc354__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge test_blitter.clk)", 
                                                         "test_blitter.sv", 
                                                         115);
    vlSelfRef.test_blitter__DOT__cpu_we = 0U;
    __Vtask_test_blitter__DOT__blt_reg__17__data = 
        (0x000000ffU & (__Vtask_test_blitter__DOT__setup_fill__2__height 
                        >> 8U));
    __Vtask_test_blitter__DOT__blt_reg__17__off = 0x0000000eU;
    __Vtask_test_blitter__DOT__write_reg__18__data 
        = __Vtask_test_blitter__DOT__blt_reg__17__data;
    __Vtask_test_blitter__DOT__write_reg__18__addr 
        = (0x0000ffffU & ((IData)(0xba83U) + __Vtask_test_blitter__DOT__blt_reg__17__off));
    Vtest_blitter___024root____VbeforeTrig_h603fc354__0(vlSelf, 
                                                        "@(posedge test_blitter.clk)");
    co_await vlSelfRef.__VtrigSched_h603fc354__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge test_blitter.clk)", 
                                                         "test_blitter.sv", 
                                                         113);
    vlSelfRef.test_blitter__DOT__cpu_addr = __Vtask_test_blitter__DOT__write_reg__18__addr;
    vlSelfRef.test_blitter__DOT__cpu_wdata = __Vtask_test_blitter__DOT__write_reg__18__data;
    vlSelfRef.test_blitter__DOT__cpu_we = 1U;
    Vtest_blitter___024root____VbeforeTrig_h603fc354__0(vlSelf, 
                                                        "@(posedge test_blitter.clk)");
    co_await vlSelfRef.__VtrigSched_h603fc354__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge test_blitter.clk)", 
                                                         "test_blitter.sv", 
                                                         115);
    vlSelfRef.test_blitter__DOT__cpu_we = 0U;
    __Vtask_test_blitter__DOT__blt_reg__19__data = 
        (0x000000ffU & __Vtask_test_blitter__DOT__setup_fill__2__dst_stride);
    __Vtask_test_blitter__DOT__blt_reg__19__off = 0x00000011U;
    __Vtask_test_blitter__DOT__write_reg__20__data 
        = __Vtask_test_blitter__DOT__blt_reg__19__data;
    __Vtask_test_blitter__DOT__write_reg__20__addr 
        = (0x0000ffffU & ((IData)(0xba83U) + __Vtask_test_blitter__DOT__blt_reg__19__off));
    Vtest_blitter___024root____VbeforeTrig_h603fc354__0(vlSelf, 
                                                        "@(posedge test_blitter.clk)");
    co_await vlSelfRef.__VtrigSched_h603fc354__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge test_blitter.clk)", 
                                                         "test_blitter.sv", 
                                                         113);
    vlSelfRef.test_blitter__DOT__cpu_addr = __Vtask_test_blitter__DOT__write_reg__20__addr;
    vlSelfRef.test_blitter__DOT__cpu_wdata = __Vtask_test_blitter__DOT__write_reg__20__data;
    vlSelfRef.test_blitter__DOT__cpu_we = 1U;
    Vtest_blitter___024root____VbeforeTrig_h603fc354__0(vlSelf, 
                                                        "@(posedge test_blitter.clk)");
    co_await vlSelfRef.__VtrigSched_h603fc354__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge test_blitter.clk)", 
                                                         "test_blitter.sv", 
                                                         115);
    vlSelfRef.test_blitter__DOT__cpu_we = 0U;
    __Vtask_test_blitter__DOT__blt_reg__21__data = 
        (0x000000ffU & (__Vtask_test_blitter__DOT__setup_fill__2__dst_stride 
                        >> 8U));
    __Vtask_test_blitter__DOT__blt_reg__21__off = 0x00000012U;
    __Vtask_test_blitter__DOT__write_reg__22__data 
        = __Vtask_test_blitter__DOT__blt_reg__21__data;
    __Vtask_test_blitter__DOT__write_reg__22__addr 
        = (0x0000ffffU & ((IData)(0xba83U) + __Vtask_test_blitter__DOT__blt_reg__21__off));
    Vtest_blitter___024root____VbeforeTrig_h603fc354__0(vlSelf, 
                                                        "@(posedge test_blitter.clk)");
    co_await vlSelfRef.__VtrigSched_h603fc354__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge test_blitter.clk)", 
                                                         "test_blitter.sv", 
                                                         113);
    vlSelfRef.test_blitter__DOT__cpu_addr = __Vtask_test_blitter__DOT__write_reg__22__addr;
    vlSelfRef.test_blitter__DOT__cpu_wdata = __Vtask_test_blitter__DOT__write_reg__22__data;
    vlSelfRef.test_blitter__DOT__cpu_we = 1U;
    Vtest_blitter___024root____VbeforeTrig_h603fc354__0(vlSelf, 
                                                        "@(posedge test_blitter.clk)");
    co_await vlSelfRef.__VtrigSched_h603fc354__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge test_blitter.clk)", 
                                                         "test_blitter.sv", 
                                                         115);
    vlSelfRef.test_blitter__DOT__cpu_we = 0U;
    __Vtask_test_blitter__DOT__blt_reg__23__data = 1U;
    __Vtask_test_blitter__DOT__blt_reg__23__off = 0x00000013U;
    __Vtask_test_blitter__DOT__write_reg__24__data 
        = __Vtask_test_blitter__DOT__blt_reg__23__data;
    __Vtask_test_blitter__DOT__write_reg__24__addr 
        = (0x0000ffffU & ((IData)(0xba83U) + __Vtask_test_blitter__DOT__blt_reg__23__off));
    Vtest_blitter___024root____VbeforeTrig_h603fc354__0(vlSelf, 
                                                        "@(posedge test_blitter.clk)");
    co_await vlSelfRef.__VtrigSched_h603fc354__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge test_blitter.clk)", 
                                                         "test_blitter.sv", 
                                                         113);
    vlSelfRef.test_blitter__DOT__cpu_addr = __Vtask_test_blitter__DOT__write_reg__24__addr;
    vlSelfRef.test_blitter__DOT__cpu_wdata = __Vtask_test_blitter__DOT__write_reg__24__data;
    vlSelfRef.test_blitter__DOT__cpu_we = 1U;
    Vtest_blitter___024root____VbeforeTrig_h603fc354__0(vlSelf, 
                                                        "@(posedge test_blitter.clk)");
    co_await vlSelfRef.__VtrigSched_h603fc354__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge test_blitter.clk)", 
                                                         "test_blitter.sv", 
                                                         115);
    vlSelfRef.test_blitter__DOT__cpu_we = 0U;
    __Vtask_test_blitter__DOT__blt_reg__25__data = 
        (0x000000ffU & __Vtask_test_blitter__DOT__setup_fill__2__fill_val);
    __Vtask_test_blitter__DOT__blt_reg__25__off = 0x00000014U;
    __Vtask_test_blitter__DOT__write_reg__26__data 
        = __Vtask_test_blitter__DOT__blt_reg__25__data;
    __Vtask_test_blitter__DOT__write_reg__26__addr 
        = (0x0000ffffU & ((IData)(0xba83U) + __Vtask_test_blitter__DOT__blt_reg__25__off));
    Vtest_blitter___024root____VbeforeTrig_h603fc354__0(vlSelf, 
                                                        "@(posedge test_blitter.clk)");
    co_await vlSelfRef.__VtrigSched_h603fc354__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge test_blitter.clk)", 
                                                         "test_blitter.sv", 
                                                         113);
    vlSelfRef.test_blitter__DOT__cpu_addr = __Vtask_test_blitter__DOT__write_reg__26__addr;
    vlSelfRef.test_blitter__DOT__cpu_wdata = __Vtask_test_blitter__DOT__write_reg__26__data;
    vlSelfRef.test_blitter__DOT__cpu_we = 1U;
    Vtest_blitter___024root____VbeforeTrig_h603fc354__0(vlSelf, 
                                                        "@(posedge test_blitter.clk)");
    co_await vlSelfRef.__VtrigSched_h603fc354__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge test_blitter.clk)", 
                                                         "test_blitter.sv", 
                                                         115);
    vlSelfRef.test_blitter__DOT__cpu_we = 0U;
    __Vtask_test_blitter__DOT__blt_reg__28__data = 1U;
    __Vtask_test_blitter__DOT__blt_reg__28__off = 0U;
    __Vtask_test_blitter__DOT__write_reg__29__data 
        = __Vtask_test_blitter__DOT__blt_reg__28__data;
    __Vtask_test_blitter__DOT__write_reg__29__addr 
        = (0x0000ffffU & ((IData)(0xba83U) + __Vtask_test_blitter__DOT__blt_reg__28__off));
    Vtest_blitter___024root____VbeforeTrig_h603fc354__0(vlSelf, 
                                                        "@(posedge test_blitter.clk)");
    co_await vlSelfRef.__VtrigSched_h603fc354__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge test_blitter.clk)", 
                                                         "test_blitter.sv", 
                                                         113);
    vlSelfRef.test_blitter__DOT__cpu_addr = __Vtask_test_blitter__DOT__write_reg__29__addr;
    vlSelfRef.test_blitter__DOT__cpu_wdata = __Vtask_test_blitter__DOT__write_reg__29__data;
    vlSelfRef.test_blitter__DOT__cpu_we = 1U;
    Vtest_blitter___024root____VbeforeTrig_h603fc354__0(vlSelf, 
                                                        "@(posedge test_blitter.clk)");
    co_await vlSelfRef.__VtrigSched_h603fc354__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge test_blitter.clk)", 
                                                         "test_blitter.sv", 
                                                         115);
    vlSelfRef.test_blitter__DOT__cpu_we = 0U;
    __Vtask_test_blitter__DOT__wait_blt_done__30__test_blitter__DOT__unnamedblk1_1__DOT____Vrepeat0 = 0;
    __Vtask_test_blitter__DOT__wait_blt_done__30__test_blitter__DOT__unnamedblk1_2__DOT____Vrepeat1 = 0;
    __Vtask_test_blitter__DOT__wait_blt_done__30__test_blitter__DOT__unnamedblk1_1__DOT____Vrepeat0 = 2U;
    while (VL_LTS_III(32, 0U, __Vtask_test_blitter__DOT__wait_blt_done__30__test_blitter__DOT__unnamedblk1_1__DOT____Vrepeat0)) {
        Vtest_blitter___024root____VbeforeTrig_h603fc354__0(vlSelf, 
                                                            "@(posedge test_blitter.clk)");
        co_await vlSelfRef.__VtrigSched_h603fc354__0.trigger(0U, 
                                                             nullptr, 
                                                             "@(posedge test_blitter.clk)", 
                                                             "test_blitter.sv", 
                                                             128);
        __Vtask_test_blitter__DOT__wait_blt_done__30__test_blitter__DOT__unnamedblk1_1__DOT____Vrepeat0 
            = (__Vtask_test_blitter__DOT__wait_blt_done__30__test_blitter__DOT__unnamedblk1_1__DOT____Vrepeat0 
               - (IData)(1U));
    }
    while (((0U != (IData)(vlSelfRef.test_blitter__DOT__dut__DOT__state)) 
            & (6U != (IData)(vlSelfRef.test_blitter__DOT__dut__DOT__state)))) {
        Vtest_blitter___024root____VbeforeTrig_h603fc354__0(vlSelf, 
                                                            "@(posedge test_blitter.clk)");
        co_await vlSelfRef.__VtrigSched_h603fc354__0.trigger(0U, 
                                                             nullptr, 
                                                             "@(posedge test_blitter.clk)", 
                                                             "test_blitter.sv", 
                                                             130);
    }
    __Vtask_test_blitter__DOT__wait_blt_done__30__test_blitter__DOT__unnamedblk1_2__DOT____Vrepeat1 = 2U;
    while (VL_LTS_III(32, 0U, __Vtask_test_blitter__DOT__wait_blt_done__30__test_blitter__DOT__unnamedblk1_2__DOT____Vrepeat1)) {
        Vtest_blitter___024root____VbeforeTrig_h603fc354__0(vlSelf, 
                                                            "@(posedge test_blitter.clk)");
        co_await vlSelfRef.__VtrigSched_h603fc354__0.trigger(0U, 
                                                             nullptr, 
                                                             "@(posedge test_blitter.clk)", 
                                                             "test_blitter.sv", 
                                                             131);
        __Vtask_test_blitter__DOT__wait_blt_done__30__test_blitter__DOT__unnamedblk1_2__DOT____Vrepeat1 
            = (__Vtask_test_blitter__DOT__wait_blt_done__30__test_blitter__DOT__unnamedblk1_2__DOT____Vrepeat1 
               - (IData)(1U));
    }
    __Vtask_test_blitter__DOT__check__31__condition 
        = (2U == vlSelfRef.test_blitter__DOT__dut__DOT__regs[1U]);
    vlSelfRef.__Vtask_test_blitter__DOT__check__31__name = "status ok"s;
    vlSelfRef.test_blitter__DOT__test_num = ((IData)(1U) 
                                             + vlSelfRef.test_blitter__DOT__test_num);
    if (__Vtask_test_blitter__DOT__check__31__condition) {
        VL_WRITEF_NX("  PASS [%0d] %@\n",0,32,vlSelfRef.test_blitter__DOT__test_num,
                     -1,&(vlSelfRef.__Vtask_test_blitter__DOT__check__31__name));
        vlSelfRef.test_blitter__DOT__pass_count = ((IData)(1U) 
                                                   + vlSelfRef.test_blitter__DOT__pass_count);
    } else {
        VL_WRITEF_NX("  FAIL [%0d] %@\n",0,32,vlSelfRef.test_blitter__DOT__test_num,
                     -1,&(vlSelfRef.__Vtask_test_blitter__DOT__check__31__name));
        vlSelfRef.test_blitter__DOT__fail_count = ((IData)(1U) 
                                                   + vlSelfRef.test_blitter__DOT__fail_count);
    }
    __Vtask_test_blitter__DOT__check__32__condition 
        = (0U == vlSelfRef.test_blitter__DOT__dut__DOT__regs[2U]);
    vlSelfRef.__Vtask_test_blitter__DOT__check__32__name = "no error"s;
    vlSelfRef.test_blitter__DOT__test_num = ((IData)(1U) 
                                             + vlSelfRef.test_blitter__DOT__test_num);
    if (__Vtask_test_blitter__DOT__check__32__condition) {
        VL_WRITEF_NX("  PASS [%0d] %@\n",0,32,vlSelfRef.test_blitter__DOT__test_num,
                     -1,&(vlSelfRef.__Vtask_test_blitter__DOT__check__32__name));
        vlSelfRef.test_blitter__DOT__pass_count = ((IData)(1U) 
                                                   + vlSelfRef.test_blitter__DOT__pass_count);
    } else {
        VL_WRITEF_NX("  FAIL [%0d] %@\n",0,32,vlSelfRef.test_blitter__DOT__test_num,
                     -1,&(vlSelfRef.__Vtask_test_blitter__DOT__check__32__name));
        vlSelfRef.test_blitter__DOT__fail_count = ((IData)(1U) 
                                                   + vlSelfRef.test_blitter__DOT__fail_count);
    }
    __Vtask_test_blitter__DOT__check__33__condition 
        = (0x0000000fU == ((vlSelfRef.test_blitter__DOT__dut__DOT__regs[24U] 
                            << 0x00000010U) | ((vlSelfRef.test_blitter__DOT__dut__DOT__regs[23U] 
                                                << 8U) 
                                               | vlSelfRef.test_blitter__DOT__dut__DOT__regs[22U])));
    vlSelfRef.__Vtask_test_blitter__DOT__check__33__name = "count = 15"s;
    vlSelfRef.test_blitter__DOT__test_num = ((IData)(1U) 
                                             + vlSelfRef.test_blitter__DOT__test_num);
    if (__Vtask_test_blitter__DOT__check__33__condition) {
        VL_WRITEF_NX("  PASS [%0d] %@\n",0,32,vlSelfRef.test_blitter__DOT__test_num,
                     -1,&(vlSelfRef.__Vtask_test_blitter__DOT__check__33__name));
        vlSelfRef.test_blitter__DOT__pass_count = ((IData)(1U) 
                                                   + vlSelfRef.test_blitter__DOT__pass_count);
    } else {
        VL_WRITEF_NX("  FAIL [%0d] %@\n",0,32,vlSelfRef.test_blitter__DOT__test_num,
                     -1,&(vlSelfRef.__Vtask_test_blitter__DOT__check__33__name));
        vlSelfRef.test_blitter__DOT__fail_count = ((IData)(1U) 
                                                   + vlSelfRef.test_blitter__DOT__fail_count);
    }
    __Vtask_test_blitter__DOT__check__34__condition 
        = ((0U == (IData)(vlSelfRef.test_blitter__DOT__dut__DOT__state)) 
           | (6U == (IData)(vlSelfRef.test_blitter__DOT__dut__DOT__state)));
    vlSelfRef.__Vtask_test_blitter__DOT__check__34__name = "RDY restored"s;
    vlSelfRef.test_blitter__DOT__test_num = ((IData)(1U) 
                                             + vlSelfRef.test_blitter__DOT__test_num);
    if (__Vtask_test_blitter__DOT__check__34__condition) {
        VL_WRITEF_NX("  PASS [%0d] %@\n",0,32,vlSelfRef.test_blitter__DOT__test_num,
                     -1,&(vlSelfRef.__Vtask_test_blitter__DOT__check__34__name));
        vlSelfRef.test_blitter__DOT__pass_count = ((IData)(1U) 
                                                   + vlSelfRef.test_blitter__DOT__pass_count);
    } else {
        VL_WRITEF_NX("  FAIL [%0d] %@\n",0,32,vlSelfRef.test_blitter__DOT__test_num,
                     -1,&(vlSelfRef.__Vtask_test_blitter__DOT__check__34__name));
        vlSelfRef.test_blitter__DOT__fail_count = ((IData)(1U) 
                                                   + vlSelfRef.test_blitter__DOT__fail_count);
    }
    __Vtask_test_blitter__DOT__check__35__condition 
        = (0xaaU == vlSelfRef.test_blitter__DOT__sim_ram[4096U]);
    vlSelfRef.__Vtask_test_blitter__DOT__check__35__name = "ram[$1000] = AA"s;
    vlSelfRef.test_blitter__DOT__test_num = ((IData)(1U) 
                                             + vlSelfRef.test_blitter__DOT__test_num);
    if (__Vtask_test_blitter__DOT__check__35__condition) {
        VL_WRITEF_NX("  PASS [%0d] %@\n",0,32,vlSelfRef.test_blitter__DOT__test_num,
                     -1,&(vlSelfRef.__Vtask_test_blitter__DOT__check__35__name));
        vlSelfRef.test_blitter__DOT__pass_count = ((IData)(1U) 
                                                   + vlSelfRef.test_blitter__DOT__pass_count);
    } else {
        VL_WRITEF_NX("  FAIL [%0d] %@\n",0,32,vlSelfRef.test_blitter__DOT__test_num,
                     -1,&(vlSelfRef.__Vtask_test_blitter__DOT__check__35__name));
        vlSelfRef.test_blitter__DOT__fail_count = ((IData)(1U) 
                                                   + vlSelfRef.test_blitter__DOT__fail_count);
    }
    __Vtask_test_blitter__DOT__check__36__condition 
        = (0xaaU == vlSelfRef.test_blitter__DOT__sim_ram[4100U]);
    vlSelfRef.__Vtask_test_blitter__DOT__check__36__name = "ram[$1004] = AA"s;
    vlSelfRef.test_blitter__DOT__test_num = ((IData)(1U) 
                                             + vlSelfRef.test_blitter__DOT__test_num);
    if (__Vtask_test_blitter__DOT__check__36__condition) {
        VL_WRITEF_NX("  PASS [%0d] %@\n",0,32,vlSelfRef.test_blitter__DOT__test_num,
                     -1,&(vlSelfRef.__Vtask_test_blitter__DOT__check__36__name));
        vlSelfRef.test_blitter__DOT__pass_count = ((IData)(1U) 
                                                   + vlSelfRef.test_blitter__DOT__pass_count);
    } else {
        VL_WRITEF_NX("  FAIL [%0d] %@\n",0,32,vlSelfRef.test_blitter__DOT__test_num,
                     -1,&(vlSelfRef.__Vtask_test_blitter__DOT__check__36__name));
        vlSelfRef.test_blitter__DOT__fail_count = ((IData)(1U) 
                                                   + vlSelfRef.test_blitter__DOT__fail_count);
    }
    __Vtask_test_blitter__DOT__check__37__condition 
        = (0U == vlSelfRef.test_blitter__DOT__sim_ram[4101U]);
    vlSelfRef.__Vtask_test_blitter__DOT__check__37__name = "ram[$1005] = 00 (beyond width)"s;
    vlSelfRef.test_blitter__DOT__test_num = ((IData)(1U) 
                                             + vlSelfRef.test_blitter__DOT__test_num);
    if (__Vtask_test_blitter__DOT__check__37__condition) {
        VL_WRITEF_NX("  PASS [%0d] %@\n",0,32,vlSelfRef.test_blitter__DOT__test_num,
                     -1,&(vlSelfRef.__Vtask_test_blitter__DOT__check__37__name));
        vlSelfRef.test_blitter__DOT__pass_count = ((IData)(1U) 
                                                   + vlSelfRef.test_blitter__DOT__pass_count);
    } else {
        VL_WRITEF_NX("  FAIL [%0d] %@\n",0,32,vlSelfRef.test_blitter__DOT__test_num,
                     -1,&(vlSelfRef.__Vtask_test_blitter__DOT__check__37__name));
        vlSelfRef.test_blitter__DOT__fail_count = ((IData)(1U) 
                                                   + vlSelfRef.test_blitter__DOT__fail_count);
    }
    __Vtask_test_blitter__DOT__check__38__condition 
        = (0xaaU == vlSelfRef.test_blitter__DOT__sim_ram[4106U]);
    vlSelfRef.__Vtask_test_blitter__DOT__check__38__name = "ram[$100A] = AA (row 1)"s;
    vlSelfRef.test_blitter__DOT__test_num = ((IData)(1U) 
                                             + vlSelfRef.test_blitter__DOT__test_num);
    if (__Vtask_test_blitter__DOT__check__38__condition) {
        VL_WRITEF_NX("  PASS [%0d] %@\n",0,32,vlSelfRef.test_blitter__DOT__test_num,
                     -1,&(vlSelfRef.__Vtask_test_blitter__DOT__check__38__name));
        vlSelfRef.test_blitter__DOT__pass_count = ((IData)(1U) 
                                                   + vlSelfRef.test_blitter__DOT__pass_count);
    } else {
        VL_WRITEF_NX("  FAIL [%0d] %@\n",0,32,vlSelfRef.test_blitter__DOT__test_num,
                     -1,&(vlSelfRef.__Vtask_test_blitter__DOT__check__38__name));
        vlSelfRef.test_blitter__DOT__fail_count = ((IData)(1U) 
                                                   + vlSelfRef.test_blitter__DOT__fail_count);
    }
    __Vtask_test_blitter__DOT__check__39__condition 
        = (0xaaU == vlSelfRef.test_blitter__DOT__sim_ram[4110U]);
    vlSelfRef.__Vtask_test_blitter__DOT__check__39__name = "ram[$100E] = AA (row 1 end)"s;
    vlSelfRef.test_blitter__DOT__test_num = ((IData)(1U) 
                                             + vlSelfRef.test_blitter__DOT__test_num);
    if (__Vtask_test_blitter__DOT__check__39__condition) {
        VL_WRITEF_NX("  PASS [%0d] %@\n",0,32,vlSelfRef.test_blitter__DOT__test_num,
                     -1,&(vlSelfRef.__Vtask_test_blitter__DOT__check__39__name));
        vlSelfRef.test_blitter__DOT__pass_count = ((IData)(1U) 
                                                   + vlSelfRef.test_blitter__DOT__pass_count);
    } else {
        VL_WRITEF_NX("  FAIL [%0d] %@\n",0,32,vlSelfRef.test_blitter__DOT__test_num,
                     -1,&(vlSelfRef.__Vtask_test_blitter__DOT__check__39__name));
        vlSelfRef.test_blitter__DOT__fail_count = ((IData)(1U) 
                                                   + vlSelfRef.test_blitter__DOT__fail_count);
    }
    __Vtask_test_blitter__DOT__check__40__condition 
        = (0xaaU == vlSelfRef.test_blitter__DOT__sim_ram[4116U]);
    vlSelfRef.__Vtask_test_blitter__DOT__check__40__name = "ram[$1014] = AA (row 2)"s;
    vlSelfRef.test_blitter__DOT__test_num = ((IData)(1U) 
                                             + vlSelfRef.test_blitter__DOT__test_num);
    if (__Vtask_test_blitter__DOT__check__40__condition) {
        VL_WRITEF_NX("  PASS [%0d] %@\n",0,32,vlSelfRef.test_blitter__DOT__test_num,
                     -1,&(vlSelfRef.__Vtask_test_blitter__DOT__check__40__name));
        vlSelfRef.test_blitter__DOT__pass_count = ((IData)(1U) 
                                                   + vlSelfRef.test_blitter__DOT__pass_count);
    } else {
        VL_WRITEF_NX("  FAIL [%0d] %@\n",0,32,vlSelfRef.test_blitter__DOT__test_num,
                     -1,&(vlSelfRef.__Vtask_test_blitter__DOT__check__40__name));
        vlSelfRef.test_blitter__DOT__fail_count = ((IData)(1U) 
                                                   + vlSelfRef.test_blitter__DOT__fail_count);
    }
    VL_WRITEF_NX("Test: Fill VGC color RAM\n",0);
    __Vtask_test_blitter__DOT__setup_fill__41__fill_val = 0x0000000eU;
    __Vtask_test_blitter__DOT__setup_fill__41__height = 2U;
    __Vtask_test_blitter__DOT__setup_fill__41__width = 0x0000000aU;
    __Vtask_test_blitter__DOT__setup_fill__41__dst_stride = 0x00000050U;
    __Vtask_test_blitter__DOT__setup_fill__41__dst_addr = 0x00000050U;
    __Vtask_test_blitter__DOT__setup_fill__41__dst_space = 2U;
    __Vtask_test_blitter__DOT__blt_reg__42__data = 
        (0x000000ffU & __Vtask_test_blitter__DOT__setup_fill__41__dst_space);
    __Vtask_test_blitter__DOT__blt_reg__42__off = 4U;
    __Vtask_test_blitter__DOT__write_reg__43__data 
        = __Vtask_test_blitter__DOT__blt_reg__42__data;
    __Vtask_test_blitter__DOT__write_reg__43__addr 
        = (0x0000ffffU & ((IData)(0xba83U) + __Vtask_test_blitter__DOT__blt_reg__42__off));
    Vtest_blitter___024root____VbeforeTrig_h603fc354__0(vlSelf, 
                                                        "@(posedge test_blitter.clk)");
    co_await vlSelfRef.__VtrigSched_h603fc354__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge test_blitter.clk)", 
                                                         "test_blitter.sv", 
                                                         113);
    vlSelfRef.test_blitter__DOT__cpu_addr = __Vtask_test_blitter__DOT__write_reg__43__addr;
    vlSelfRef.test_blitter__DOT__cpu_wdata = __Vtask_test_blitter__DOT__write_reg__43__data;
    vlSelfRef.test_blitter__DOT__cpu_we = 1U;
    Vtest_blitter___024root____VbeforeTrig_h603fc354__0(vlSelf, 
                                                        "@(posedge test_blitter.clk)");
    co_await vlSelfRef.__VtrigSched_h603fc354__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge test_blitter.clk)", 
                                                         "test_blitter.sv", 
                                                         115);
    vlSelfRef.test_blitter__DOT__cpu_we = 0U;
    __Vtask_test_blitter__DOT__blt_reg__44__data = 
        (0x000000ffU & __Vtask_test_blitter__DOT__setup_fill__41__dst_addr);
    __Vtask_test_blitter__DOT__blt_reg__44__off = 8U;
    __Vtask_test_blitter__DOT__write_reg__45__data 
        = __Vtask_test_blitter__DOT__blt_reg__44__data;
    __Vtask_test_blitter__DOT__write_reg__45__addr 
        = (0x0000ffffU & ((IData)(0xba83U) + __Vtask_test_blitter__DOT__blt_reg__44__off));
    Vtest_blitter___024root____VbeforeTrig_h603fc354__0(vlSelf, 
                                                        "@(posedge test_blitter.clk)");
    co_await vlSelfRef.__VtrigSched_h603fc354__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge test_blitter.clk)", 
                                                         "test_blitter.sv", 
                                                         113);
    vlSelfRef.test_blitter__DOT__cpu_addr = __Vtask_test_blitter__DOT__write_reg__45__addr;
    vlSelfRef.test_blitter__DOT__cpu_wdata = __Vtask_test_blitter__DOT__write_reg__45__data;
    vlSelfRef.test_blitter__DOT__cpu_we = 1U;
    Vtest_blitter___024root____VbeforeTrig_h603fc354__0(vlSelf, 
                                                        "@(posedge test_blitter.clk)");
    co_await vlSelfRef.__VtrigSched_h603fc354__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge test_blitter.clk)", 
                                                         "test_blitter.sv", 
                                                         115);
    vlSelfRef.test_blitter__DOT__cpu_we = 0U;
    __Vtask_test_blitter__DOT__blt_reg__46__data = 
        (0x000000ffU & (__Vtask_test_blitter__DOT__setup_fill__41__dst_addr 
                        >> 8U));
    __Vtask_test_blitter__DOT__blt_reg__46__off = 9U;
    __Vtask_test_blitter__DOT__write_reg__47__data 
        = __Vtask_test_blitter__DOT__blt_reg__46__data;
    __Vtask_test_blitter__DOT__write_reg__47__addr 
        = (0x0000ffffU & ((IData)(0xba83U) + __Vtask_test_blitter__DOT__blt_reg__46__off));
    Vtest_blitter___024root____VbeforeTrig_h603fc354__0(vlSelf, 
                                                        "@(posedge test_blitter.clk)");
    co_await vlSelfRef.__VtrigSched_h603fc354__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge test_blitter.clk)", 
                                                         "test_blitter.sv", 
                                                         113);
    vlSelfRef.test_blitter__DOT__cpu_addr = __Vtask_test_blitter__DOT__write_reg__47__addr;
    vlSelfRef.test_blitter__DOT__cpu_wdata = __Vtask_test_blitter__DOT__write_reg__47__data;
    vlSelfRef.test_blitter__DOT__cpu_we = 1U;
    Vtest_blitter___024root____VbeforeTrig_h603fc354__0(vlSelf, 
                                                        "@(posedge test_blitter.clk)");
    co_await vlSelfRef.__VtrigSched_h603fc354__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge test_blitter.clk)", 
                                                         "test_blitter.sv", 
                                                         115);
    vlSelfRef.test_blitter__DOT__cpu_we = 0U;
    __Vtask_test_blitter__DOT__blt_reg__48__data = 
        (0x000000ffU & (__Vtask_test_blitter__DOT__setup_fill__41__dst_addr 
                        >> 0x10U));
    __Vtask_test_blitter__DOT__blt_reg__48__off = 0x0000000aU;
    __Vtask_test_blitter__DOT__write_reg__49__data 
        = __Vtask_test_blitter__DOT__blt_reg__48__data;
    __Vtask_test_blitter__DOT__write_reg__49__addr 
        = (0x0000ffffU & ((IData)(0xba83U) + __Vtask_test_blitter__DOT__blt_reg__48__off));
    Vtest_blitter___024root____VbeforeTrig_h603fc354__0(vlSelf, 
                                                        "@(posedge test_blitter.clk)");
    co_await vlSelfRef.__VtrigSched_h603fc354__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge test_blitter.clk)", 
                                                         "test_blitter.sv", 
                                                         113);
    vlSelfRef.test_blitter__DOT__cpu_addr = __Vtask_test_blitter__DOT__write_reg__49__addr;
    vlSelfRef.test_blitter__DOT__cpu_wdata = __Vtask_test_blitter__DOT__write_reg__49__data;
    vlSelfRef.test_blitter__DOT__cpu_we = 1U;
    Vtest_blitter___024root____VbeforeTrig_h603fc354__0(vlSelf, 
                                                        "@(posedge test_blitter.clk)");
    co_await vlSelfRef.__VtrigSched_h603fc354__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge test_blitter.clk)", 
                                                         "test_blitter.sv", 
                                                         115);
    vlSelfRef.test_blitter__DOT__cpu_we = 0U;
    __Vtask_test_blitter__DOT__blt_reg__50__data = 
        (0x000000ffU & __Vtask_test_blitter__DOT__setup_fill__41__width);
    __Vtask_test_blitter__DOT__blt_reg__50__off = 0x0000000bU;
    __Vtask_test_blitter__DOT__write_reg__51__data 
        = __Vtask_test_blitter__DOT__blt_reg__50__data;
    __Vtask_test_blitter__DOT__write_reg__51__addr 
        = (0x0000ffffU & ((IData)(0xba83U) + __Vtask_test_blitter__DOT__blt_reg__50__off));
    Vtest_blitter___024root____VbeforeTrig_h603fc354__0(vlSelf, 
                                                        "@(posedge test_blitter.clk)");
    co_await vlSelfRef.__VtrigSched_h603fc354__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge test_blitter.clk)", 
                                                         "test_blitter.sv", 
                                                         113);
    vlSelfRef.test_blitter__DOT__cpu_addr = __Vtask_test_blitter__DOT__write_reg__51__addr;
    vlSelfRef.test_blitter__DOT__cpu_wdata = __Vtask_test_blitter__DOT__write_reg__51__data;
    vlSelfRef.test_blitter__DOT__cpu_we = 1U;
    Vtest_blitter___024root____VbeforeTrig_h603fc354__0(vlSelf, 
                                                        "@(posedge test_blitter.clk)");
    co_await vlSelfRef.__VtrigSched_h603fc354__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge test_blitter.clk)", 
                                                         "test_blitter.sv", 
                                                         115);
    vlSelfRef.test_blitter__DOT__cpu_we = 0U;
    __Vtask_test_blitter__DOT__blt_reg__52__data = 
        (0x000000ffU & (__Vtask_test_blitter__DOT__setup_fill__41__width 
                        >> 8U));
    __Vtask_test_blitter__DOT__blt_reg__52__off = 0x0000000cU;
    __Vtask_test_blitter__DOT__write_reg__53__data 
        = __Vtask_test_blitter__DOT__blt_reg__52__data;
    __Vtask_test_blitter__DOT__write_reg__53__addr 
        = (0x0000ffffU & ((IData)(0xba83U) + __Vtask_test_blitter__DOT__blt_reg__52__off));
    Vtest_blitter___024root____VbeforeTrig_h603fc354__0(vlSelf, 
                                                        "@(posedge test_blitter.clk)");
    co_await vlSelfRef.__VtrigSched_h603fc354__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge test_blitter.clk)", 
                                                         "test_blitter.sv", 
                                                         113);
    vlSelfRef.test_blitter__DOT__cpu_addr = __Vtask_test_blitter__DOT__write_reg__53__addr;
    vlSelfRef.test_blitter__DOT__cpu_wdata = __Vtask_test_blitter__DOT__write_reg__53__data;
    vlSelfRef.test_blitter__DOT__cpu_we = 1U;
    Vtest_blitter___024root____VbeforeTrig_h603fc354__0(vlSelf, 
                                                        "@(posedge test_blitter.clk)");
    co_await vlSelfRef.__VtrigSched_h603fc354__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge test_blitter.clk)", 
                                                         "test_blitter.sv", 
                                                         115);
    vlSelfRef.test_blitter__DOT__cpu_we = 0U;
    __Vtask_test_blitter__DOT__blt_reg__54__data = 
        (0x000000ffU & __Vtask_test_blitter__DOT__setup_fill__41__height);
    __Vtask_test_blitter__DOT__blt_reg__54__off = 0x0000000dU;
    __Vtask_test_blitter__DOT__write_reg__55__data 
        = __Vtask_test_blitter__DOT__blt_reg__54__data;
    __Vtask_test_blitter__DOT__write_reg__55__addr 
        = (0x0000ffffU & ((IData)(0xba83U) + __Vtask_test_blitter__DOT__blt_reg__54__off));
    Vtest_blitter___024root____VbeforeTrig_h603fc354__0(vlSelf, 
                                                        "@(posedge test_blitter.clk)");
    co_await vlSelfRef.__VtrigSched_h603fc354__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge test_blitter.clk)", 
                                                         "test_blitter.sv", 
                                                         113);
    vlSelfRef.test_blitter__DOT__cpu_addr = __Vtask_test_blitter__DOT__write_reg__55__addr;
    vlSelfRef.test_blitter__DOT__cpu_wdata = __Vtask_test_blitter__DOT__write_reg__55__data;
    vlSelfRef.test_blitter__DOT__cpu_we = 1U;
    Vtest_blitter___024root____VbeforeTrig_h603fc354__0(vlSelf, 
                                                        "@(posedge test_blitter.clk)");
    co_await vlSelfRef.__VtrigSched_h603fc354__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge test_blitter.clk)", 
                                                         "test_blitter.sv", 
                                                         115);
    vlSelfRef.test_blitter__DOT__cpu_we = 0U;
    __Vtask_test_blitter__DOT__blt_reg__56__data = 
        (0x000000ffU & (__Vtask_test_blitter__DOT__setup_fill__41__height 
                        >> 8U));
    __Vtask_test_blitter__DOT__blt_reg__56__off = 0x0000000eU;
    __Vtask_test_blitter__DOT__write_reg__57__data 
        = __Vtask_test_blitter__DOT__blt_reg__56__data;
    __Vtask_test_blitter__DOT__write_reg__57__addr 
        = (0x0000ffffU & ((IData)(0xba83U) + __Vtask_test_blitter__DOT__blt_reg__56__off));
    Vtest_blitter___024root____VbeforeTrig_h603fc354__0(vlSelf, 
                                                        "@(posedge test_blitter.clk)");
    co_await vlSelfRef.__VtrigSched_h603fc354__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge test_blitter.clk)", 
                                                         "test_blitter.sv", 
                                                         113);
    vlSelfRef.test_blitter__DOT__cpu_addr = __Vtask_test_blitter__DOT__write_reg__57__addr;
    vlSelfRef.test_blitter__DOT__cpu_wdata = __Vtask_test_blitter__DOT__write_reg__57__data;
    vlSelfRef.test_blitter__DOT__cpu_we = 1U;
    Vtest_blitter___024root____VbeforeTrig_h603fc354__0(vlSelf, 
                                                        "@(posedge test_blitter.clk)");
    co_await vlSelfRef.__VtrigSched_h603fc354__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge test_blitter.clk)", 
                                                         "test_blitter.sv", 
                                                         115);
    vlSelfRef.test_blitter__DOT__cpu_we = 0U;
    __Vtask_test_blitter__DOT__blt_reg__58__data = 
        (0x000000ffU & __Vtask_test_blitter__DOT__setup_fill__41__dst_stride);
    __Vtask_test_blitter__DOT__blt_reg__58__off = 0x00000011U;
    __Vtask_test_blitter__DOT__write_reg__59__data 
        = __Vtask_test_blitter__DOT__blt_reg__58__data;
    __Vtask_test_blitter__DOT__write_reg__59__addr 
        = (0x0000ffffU & ((IData)(0xba83U) + __Vtask_test_blitter__DOT__blt_reg__58__off));
    Vtest_blitter___024root____VbeforeTrig_h603fc354__0(vlSelf, 
                                                        "@(posedge test_blitter.clk)");
    co_await vlSelfRef.__VtrigSched_h603fc354__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge test_blitter.clk)", 
                                                         "test_blitter.sv", 
                                                         113);
    vlSelfRef.test_blitter__DOT__cpu_addr = __Vtask_test_blitter__DOT__write_reg__59__addr;
    vlSelfRef.test_blitter__DOT__cpu_wdata = __Vtask_test_blitter__DOT__write_reg__59__data;
    vlSelfRef.test_blitter__DOT__cpu_we = 1U;
    Vtest_blitter___024root____VbeforeTrig_h603fc354__0(vlSelf, 
                                                        "@(posedge test_blitter.clk)");
    co_await vlSelfRef.__VtrigSched_h603fc354__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge test_blitter.clk)", 
                                                         "test_blitter.sv", 
                                                         115);
    vlSelfRef.test_blitter__DOT__cpu_we = 0U;
    __Vtask_test_blitter__DOT__blt_reg__60__data = 
        (0x000000ffU & (__Vtask_test_blitter__DOT__setup_fill__41__dst_stride 
                        >> 8U));
    __Vtask_test_blitter__DOT__blt_reg__60__off = 0x00000012U;
    __Vtask_test_blitter__DOT__write_reg__61__data 
        = __Vtask_test_blitter__DOT__blt_reg__60__data;
    __Vtask_test_blitter__DOT__write_reg__61__addr 
        = (0x0000ffffU & ((IData)(0xba83U) + __Vtask_test_blitter__DOT__blt_reg__60__off));
    Vtest_blitter___024root____VbeforeTrig_h603fc354__0(vlSelf, 
                                                        "@(posedge test_blitter.clk)");
    co_await vlSelfRef.__VtrigSched_h603fc354__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge test_blitter.clk)", 
                                                         "test_blitter.sv", 
                                                         113);
    vlSelfRef.test_blitter__DOT__cpu_addr = __Vtask_test_blitter__DOT__write_reg__61__addr;
    vlSelfRef.test_blitter__DOT__cpu_wdata = __Vtask_test_blitter__DOT__write_reg__61__data;
    vlSelfRef.test_blitter__DOT__cpu_we = 1U;
    Vtest_blitter___024root____VbeforeTrig_h603fc354__0(vlSelf, 
                                                        "@(posedge test_blitter.clk)");
    co_await vlSelfRef.__VtrigSched_h603fc354__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge test_blitter.clk)", 
                                                         "test_blitter.sv", 
                                                         115);
    vlSelfRef.test_blitter__DOT__cpu_we = 0U;
    __Vtask_test_blitter__DOT__blt_reg__62__data = 1U;
    __Vtask_test_blitter__DOT__blt_reg__62__off = 0x00000013U;
    __Vtask_test_blitter__DOT__write_reg__63__data 
        = __Vtask_test_blitter__DOT__blt_reg__62__data;
    __Vtask_test_blitter__DOT__write_reg__63__addr 
        = (0x0000ffffU & ((IData)(0xba83U) + __Vtask_test_blitter__DOT__blt_reg__62__off));
    Vtest_blitter___024root____VbeforeTrig_h603fc354__0(vlSelf, 
                                                        "@(posedge test_blitter.clk)");
    co_await vlSelfRef.__VtrigSched_h603fc354__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge test_blitter.clk)", 
                                                         "test_blitter.sv", 
                                                         113);
    vlSelfRef.test_blitter__DOT__cpu_addr = __Vtask_test_blitter__DOT__write_reg__63__addr;
    vlSelfRef.test_blitter__DOT__cpu_wdata = __Vtask_test_blitter__DOT__write_reg__63__data;
    vlSelfRef.test_blitter__DOT__cpu_we = 1U;
    Vtest_blitter___024root____VbeforeTrig_h603fc354__0(vlSelf, 
                                                        "@(posedge test_blitter.clk)");
    co_await vlSelfRef.__VtrigSched_h603fc354__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge test_blitter.clk)", 
                                                         "test_blitter.sv", 
                                                         115);
    vlSelfRef.test_blitter__DOT__cpu_we = 0U;
    __Vtask_test_blitter__DOT__blt_reg__64__data = 
        (0x000000ffU & __Vtask_test_blitter__DOT__setup_fill__41__fill_val);
    __Vtask_test_blitter__DOT__blt_reg__64__off = 0x00000014U;
    __Vtask_test_blitter__DOT__write_reg__65__data 
        = __Vtask_test_blitter__DOT__blt_reg__64__data;
    __Vtask_test_blitter__DOT__write_reg__65__addr 
        = (0x0000ffffU & ((IData)(0xba83U) + __Vtask_test_blitter__DOT__blt_reg__64__off));
    Vtest_blitter___024root____VbeforeTrig_h603fc354__0(vlSelf, 
                                                        "@(posedge test_blitter.clk)");
    co_await vlSelfRef.__VtrigSched_h603fc354__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge test_blitter.clk)", 
                                                         "test_blitter.sv", 
                                                         113);
    vlSelfRef.test_blitter__DOT__cpu_addr = __Vtask_test_blitter__DOT__write_reg__65__addr;
    vlSelfRef.test_blitter__DOT__cpu_wdata = __Vtask_test_blitter__DOT__write_reg__65__data;
    vlSelfRef.test_blitter__DOT__cpu_we = 1U;
    Vtest_blitter___024root____VbeforeTrig_h603fc354__0(vlSelf, 
                                                        "@(posedge test_blitter.clk)");
    co_await vlSelfRef.__VtrigSched_h603fc354__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge test_blitter.clk)", 
                                                         "test_blitter.sv", 
                                                         115);
    vlSelfRef.test_blitter__DOT__cpu_we = 0U;
    __Vtask_test_blitter__DOT__blt_reg__67__data = 1U;
    __Vtask_test_blitter__DOT__blt_reg__67__off = 0U;
    __Vtask_test_blitter__DOT__write_reg__68__data 
        = __Vtask_test_blitter__DOT__blt_reg__67__data;
    __Vtask_test_blitter__DOT__write_reg__68__addr 
        = (0x0000ffffU & ((IData)(0xba83U) + __Vtask_test_blitter__DOT__blt_reg__67__off));
    Vtest_blitter___024root____VbeforeTrig_h603fc354__0(vlSelf, 
                                                        "@(posedge test_blitter.clk)");
    co_await vlSelfRef.__VtrigSched_h603fc354__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge test_blitter.clk)", 
                                                         "test_blitter.sv", 
                                                         113);
    vlSelfRef.test_blitter__DOT__cpu_addr = __Vtask_test_blitter__DOT__write_reg__68__addr;
    vlSelfRef.test_blitter__DOT__cpu_wdata = __Vtask_test_blitter__DOT__write_reg__68__data;
    vlSelfRef.test_blitter__DOT__cpu_we = 1U;
    Vtest_blitter___024root____VbeforeTrig_h603fc354__0(vlSelf, 
                                                        "@(posedge test_blitter.clk)");
    co_await vlSelfRef.__VtrigSched_h603fc354__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge test_blitter.clk)", 
                                                         "test_blitter.sv", 
                                                         115);
    vlSelfRef.test_blitter__DOT__cpu_we = 0U;
    __Vtask_test_blitter__DOT__wait_blt_done__69__test_blitter__DOT__unnamedblk1_1__DOT____Vrepeat0 = 0;
    __Vtask_test_blitter__DOT__wait_blt_done__69__test_blitter__DOT__unnamedblk1_2__DOT____Vrepeat1 = 0;
    __Vtask_test_blitter__DOT__wait_blt_done__69__test_blitter__DOT__unnamedblk1_1__DOT____Vrepeat0 = 2U;
    while (VL_LTS_III(32, 0U, __Vtask_test_blitter__DOT__wait_blt_done__69__test_blitter__DOT__unnamedblk1_1__DOT____Vrepeat0)) {
        Vtest_blitter___024root____VbeforeTrig_h603fc354__0(vlSelf, 
                                                            "@(posedge test_blitter.clk)");
        co_await vlSelfRef.__VtrigSched_h603fc354__0.trigger(0U, 
                                                             nullptr, 
                                                             "@(posedge test_blitter.clk)", 
                                                             "test_blitter.sv", 
                                                             128);
        __Vtask_test_blitter__DOT__wait_blt_done__69__test_blitter__DOT__unnamedblk1_1__DOT____Vrepeat0 
            = (__Vtask_test_blitter__DOT__wait_blt_done__69__test_blitter__DOT__unnamedblk1_1__DOT____Vrepeat0 
               - (IData)(1U));
    }
    while (((0U != (IData)(vlSelfRef.test_blitter__DOT__dut__DOT__state)) 
            & (6U != (IData)(vlSelfRef.test_blitter__DOT__dut__DOT__state)))) {
        Vtest_blitter___024root____VbeforeTrig_h603fc354__0(vlSelf, 
                                                            "@(posedge test_blitter.clk)");
        co_await vlSelfRef.__VtrigSched_h603fc354__0.trigger(0U, 
                                                             nullptr, 
                                                             "@(posedge test_blitter.clk)", 
                                                             "test_blitter.sv", 
                                                             130);
    }
    __Vtask_test_blitter__DOT__wait_blt_done__69__test_blitter__DOT__unnamedblk1_2__DOT____Vrepeat1 = 2U;
    while (VL_LTS_III(32, 0U, __Vtask_test_blitter__DOT__wait_blt_done__69__test_blitter__DOT__unnamedblk1_2__DOT____Vrepeat1)) {
        Vtest_blitter___024root____VbeforeTrig_h603fc354__0(vlSelf, 
                                                            "@(posedge test_blitter.clk)");
        co_await vlSelfRef.__VtrigSched_h603fc354__0.trigger(0U, 
                                                             nullptr, 
                                                             "@(posedge test_blitter.clk)", 
                                                             "test_blitter.sv", 
                                                             131);
        __Vtask_test_blitter__DOT__wait_blt_done__69__test_blitter__DOT__unnamedblk1_2__DOT____Vrepeat1 
            = (__Vtask_test_blitter__DOT__wait_blt_done__69__test_blitter__DOT__unnamedblk1_2__DOT____Vrepeat1 
               - (IData)(1U));
    }
    __Vtask_test_blitter__DOT__check__70__condition 
        = (2U == vlSelfRef.test_blitter__DOT__dut__DOT__regs[1U]);
    vlSelfRef.__Vtask_test_blitter__DOT__check__70__name = "color status ok"s;
    vlSelfRef.test_blitter__DOT__test_num = ((IData)(1U) 
                                             + vlSelfRef.test_blitter__DOT__test_num);
    if (__Vtask_test_blitter__DOT__check__70__condition) {
        VL_WRITEF_NX("  PASS [%0d] %@\n",0,32,vlSelfRef.test_blitter__DOT__test_num,
                     -1,&(vlSelfRef.__Vtask_test_blitter__DOT__check__70__name));
        vlSelfRef.test_blitter__DOT__pass_count = ((IData)(1U) 
                                                   + vlSelfRef.test_blitter__DOT__pass_count);
    } else {
        VL_WRITEF_NX("  FAIL [%0d] %@\n",0,32,vlSelfRef.test_blitter__DOT__test_num,
                     -1,&(vlSelfRef.__Vtask_test_blitter__DOT__check__70__name));
        vlSelfRef.test_blitter__DOT__fail_count = ((IData)(1U) 
                                                   + vlSelfRef.test_blitter__DOT__fail_count);
    }
    __Vtask_test_blitter__DOT__check__71__condition 
        = (0x0eU == vlSelfRef.test_blitter__DOT__sim_color[80U]);
    vlSelfRef.__Vtask_test_blitter__DOT__check__71__name = "color[80] = 0E"s;
    vlSelfRef.test_blitter__DOT__test_num = ((IData)(1U) 
                                             + vlSelfRef.test_blitter__DOT__test_num);
    if (__Vtask_test_blitter__DOT__check__71__condition) {
        VL_WRITEF_NX("  PASS [%0d] %@\n",0,32,vlSelfRef.test_blitter__DOT__test_num,
                     -1,&(vlSelfRef.__Vtask_test_blitter__DOT__check__71__name));
        vlSelfRef.test_blitter__DOT__pass_count = ((IData)(1U) 
                                                   + vlSelfRef.test_blitter__DOT__pass_count);
    } else {
        VL_WRITEF_NX("  FAIL [%0d] %@\n",0,32,vlSelfRef.test_blitter__DOT__test_num,
                     -1,&(vlSelfRef.__Vtask_test_blitter__DOT__check__71__name));
        vlSelfRef.test_blitter__DOT__fail_count = ((IData)(1U) 
                                                   + vlSelfRef.test_blitter__DOT__fail_count);
    }
    __Vtask_test_blitter__DOT__check__72__condition 
        = (0x0eU == vlSelfRef.test_blitter__DOT__sim_color[89U]);
    vlSelfRef.__Vtask_test_blitter__DOT__check__72__name = "color[89] = 0E"s;
    vlSelfRef.test_blitter__DOT__test_num = ((IData)(1U) 
                                             + vlSelfRef.test_blitter__DOT__test_num);
    if (__Vtask_test_blitter__DOT__check__72__condition) {
        VL_WRITEF_NX("  PASS [%0d] %@\n",0,32,vlSelfRef.test_blitter__DOT__test_num,
                     -1,&(vlSelfRef.__Vtask_test_blitter__DOT__check__72__name));
        vlSelfRef.test_blitter__DOT__pass_count = ((IData)(1U) 
                                                   + vlSelfRef.test_blitter__DOT__pass_count);
    } else {
        VL_WRITEF_NX("  FAIL [%0d] %@\n",0,32,vlSelfRef.test_blitter__DOT__test_num,
                     -1,&(vlSelfRef.__Vtask_test_blitter__DOT__check__72__name));
        vlSelfRef.test_blitter__DOT__fail_count = ((IData)(1U) 
                                                   + vlSelfRef.test_blitter__DOT__fail_count);
    }
    __Vtask_test_blitter__DOT__check__73__condition 
        = (0x0eU == vlSelfRef.test_blitter__DOT__sim_color[160U]);
    vlSelfRef.__Vtask_test_blitter__DOT__check__73__name = "color[160] = 0E (row 1)"s;
    vlSelfRef.test_blitter__DOT__test_num = ((IData)(1U) 
                                             + vlSelfRef.test_blitter__DOT__test_num);
    if (__Vtask_test_blitter__DOT__check__73__condition) {
        VL_WRITEF_NX("  PASS [%0d] %@\n",0,32,vlSelfRef.test_blitter__DOT__test_num,
                     -1,&(vlSelfRef.__Vtask_test_blitter__DOT__check__73__name));
        vlSelfRef.test_blitter__DOT__pass_count = ((IData)(1U) 
                                                   + vlSelfRef.test_blitter__DOT__pass_count);
    } else {
        VL_WRITEF_NX("  FAIL [%0d] %@\n",0,32,vlSelfRef.test_blitter__DOT__test_num,
                     -1,&(vlSelfRef.__Vtask_test_blitter__DOT__check__73__name));
        vlSelfRef.test_blitter__DOT__fail_count = ((IData)(1U) 
                                                   + vlSelfRef.test_blitter__DOT__fail_count);
    }
    __Vtask_test_blitter__DOT__check__74__condition 
        = (0U == vlSelfRef.test_blitter__DOT__sim_color[79U]);
    vlSelfRef.__Vtask_test_blitter__DOT__check__74__name = "color[79] untouched"s;
    vlSelfRef.test_blitter__DOT__test_num = ((IData)(1U) 
                                             + vlSelfRef.test_blitter__DOT__test_num);
    if (__Vtask_test_blitter__DOT__check__74__condition) {
        VL_WRITEF_NX("  PASS [%0d] %@\n",0,32,vlSelfRef.test_blitter__DOT__test_num,
                     -1,&(vlSelfRef.__Vtask_test_blitter__DOT__check__74__name));
        vlSelfRef.test_blitter__DOT__pass_count = ((IData)(1U) 
                                                   + vlSelfRef.test_blitter__DOT__pass_count);
    } else {
        VL_WRITEF_NX("  FAIL [%0d] %@\n",0,32,vlSelfRef.test_blitter__DOT__test_num,
                     -1,&(vlSelfRef.__Vtask_test_blitter__DOT__check__74__name));
        vlSelfRef.test_blitter__DOT__fail_count = ((IData)(1U) 
                                                   + vlSelfRef.test_blitter__DOT__fail_count);
    }
    VL_WRITEF_NX("Test: Copy RAM to RAM\n",0);
    vlSelfRef.test_blitter__DOT__sim_ram[8192U] = 1U;
    vlSelfRef.test_blitter__DOT__sim_ram[8193U] = 2U;
    vlSelfRef.test_blitter__DOT__sim_ram[8194U] = 3U;
    vlSelfRef.test_blitter__DOT__sim_ram[8195U] = 4U;
    vlSelfRef.test_blitter__DOT__sim_ram[8200U] = 0x0bU;
    vlSelfRef.test_blitter__DOT__sim_ram[8201U] = 0x0cU;
    vlSelfRef.test_blitter__DOT__sim_ram[8202U] = 0x0dU;
    vlSelfRef.test_blitter__DOT__sim_ram[8203U] = 0x0eU;
    vlSelfRef.test_blitter__DOT__sim_ram[8208U] = 0x15U;
    vlSelfRef.test_blitter__DOT__sim_ram[8209U] = 0x16U;
    vlSelfRef.test_blitter__DOT__sim_ram[8210U] = 0x17U;
    vlSelfRef.test_blitter__DOT__sim_ram[8211U] = 0x18U;
    __Vtask_test_blitter__DOT__setup_copy__75__height = 3U;
    __Vtask_test_blitter__DOT__setup_copy__75__width = 4U;
    __Vtask_test_blitter__DOT__setup_copy__75__dst_stride = 6U;
    __Vtask_test_blitter__DOT__setup_copy__75__dst_addr = 0x00003000U;
    __Vtask_test_blitter__DOT__setup_copy__75__dst_space = 0U;
    __Vtask_test_blitter__DOT__setup_copy__75__src_stride = 8U;
    __Vtask_test_blitter__DOT__setup_copy__75__src_addr = 0x00002000U;
    __Vtask_test_blitter__DOT__setup_copy__75__src_space = 0U;
    __Vtask_test_blitter__DOT__blt_reg__76__data = 
        (0x000000ffU & __Vtask_test_blitter__DOT__setup_copy__75__src_space);
    __Vtask_test_blitter__DOT__blt_reg__76__off = 3U;
    __Vtask_test_blitter__DOT__write_reg__77__data 
        = __Vtask_test_blitter__DOT__blt_reg__76__data;
    __Vtask_test_blitter__DOT__write_reg__77__addr 
        = (0x0000ffffU & ((IData)(0xba83U) + __Vtask_test_blitter__DOT__blt_reg__76__off));
    Vtest_blitter___024root____VbeforeTrig_h603fc354__0(vlSelf, 
                                                        "@(posedge test_blitter.clk)");
    co_await vlSelfRef.__VtrigSched_h603fc354__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge test_blitter.clk)", 
                                                         "test_blitter.sv", 
                                                         113);
    vlSelfRef.test_blitter__DOT__cpu_addr = __Vtask_test_blitter__DOT__write_reg__77__addr;
    vlSelfRef.test_blitter__DOT__cpu_wdata = __Vtask_test_blitter__DOT__write_reg__77__data;
    vlSelfRef.test_blitter__DOT__cpu_we = 1U;
    Vtest_blitter___024root____VbeforeTrig_h603fc354__0(vlSelf, 
                                                        "@(posedge test_blitter.clk)");
    co_await vlSelfRef.__VtrigSched_h603fc354__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge test_blitter.clk)", 
                                                         "test_blitter.sv", 
                                                         115);
    vlSelfRef.test_blitter__DOT__cpu_we = 0U;
    __Vtask_test_blitter__DOT__blt_reg__78__data = 
        (0x000000ffU & __Vtask_test_blitter__DOT__setup_copy__75__dst_space);
    __Vtask_test_blitter__DOT__blt_reg__78__off = 4U;
    __Vtask_test_blitter__DOT__write_reg__79__data 
        = __Vtask_test_blitter__DOT__blt_reg__78__data;
    __Vtask_test_blitter__DOT__write_reg__79__addr 
        = (0x0000ffffU & ((IData)(0xba83U) + __Vtask_test_blitter__DOT__blt_reg__78__off));
    Vtest_blitter___024root____VbeforeTrig_h603fc354__0(vlSelf, 
                                                        "@(posedge test_blitter.clk)");
    co_await vlSelfRef.__VtrigSched_h603fc354__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge test_blitter.clk)", 
                                                         "test_blitter.sv", 
                                                         113);
    vlSelfRef.test_blitter__DOT__cpu_addr = __Vtask_test_blitter__DOT__write_reg__79__addr;
    vlSelfRef.test_blitter__DOT__cpu_wdata = __Vtask_test_blitter__DOT__write_reg__79__data;
    vlSelfRef.test_blitter__DOT__cpu_we = 1U;
    Vtest_blitter___024root____VbeforeTrig_h603fc354__0(vlSelf, 
                                                        "@(posedge test_blitter.clk)");
    co_await vlSelfRef.__VtrigSched_h603fc354__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge test_blitter.clk)", 
                                                         "test_blitter.sv", 
                                                         115);
    vlSelfRef.test_blitter__DOT__cpu_we = 0U;
    __Vtask_test_blitter__DOT__blt_reg__80__data = 
        (0x000000ffU & __Vtask_test_blitter__DOT__setup_copy__75__src_addr);
    __Vtask_test_blitter__DOT__blt_reg__80__off = 5U;
    __Vtask_test_blitter__DOT__write_reg__81__data 
        = __Vtask_test_blitter__DOT__blt_reg__80__data;
    __Vtask_test_blitter__DOT__write_reg__81__addr 
        = (0x0000ffffU & ((IData)(0xba83U) + __Vtask_test_blitter__DOT__blt_reg__80__off));
    Vtest_blitter___024root____VbeforeTrig_h603fc354__0(vlSelf, 
                                                        "@(posedge test_blitter.clk)");
    co_await vlSelfRef.__VtrigSched_h603fc354__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge test_blitter.clk)", 
                                                         "test_blitter.sv", 
                                                         113);
    vlSelfRef.test_blitter__DOT__cpu_addr = __Vtask_test_blitter__DOT__write_reg__81__addr;
    vlSelfRef.test_blitter__DOT__cpu_wdata = __Vtask_test_blitter__DOT__write_reg__81__data;
    vlSelfRef.test_blitter__DOT__cpu_we = 1U;
    Vtest_blitter___024root____VbeforeTrig_h603fc354__0(vlSelf, 
                                                        "@(posedge test_blitter.clk)");
    co_await vlSelfRef.__VtrigSched_h603fc354__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge test_blitter.clk)", 
                                                         "test_blitter.sv", 
                                                         115);
    vlSelfRef.test_blitter__DOT__cpu_we = 0U;
    __Vtask_test_blitter__DOT__blt_reg__82__data = 
        (0x000000ffU & (__Vtask_test_blitter__DOT__setup_copy__75__src_addr 
                        >> 8U));
    __Vtask_test_blitter__DOT__blt_reg__82__off = 6U;
    __Vtask_test_blitter__DOT__write_reg__83__data 
        = __Vtask_test_blitter__DOT__blt_reg__82__data;
    __Vtask_test_blitter__DOT__write_reg__83__addr 
        = (0x0000ffffU & ((IData)(0xba83U) + __Vtask_test_blitter__DOT__blt_reg__82__off));
    Vtest_blitter___024root____VbeforeTrig_h603fc354__0(vlSelf, 
                                                        "@(posedge test_blitter.clk)");
    co_await vlSelfRef.__VtrigSched_h603fc354__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge test_blitter.clk)", 
                                                         "test_blitter.sv", 
                                                         113);
    vlSelfRef.test_blitter__DOT__cpu_addr = __Vtask_test_blitter__DOT__write_reg__83__addr;
    vlSelfRef.test_blitter__DOT__cpu_wdata = __Vtask_test_blitter__DOT__write_reg__83__data;
    vlSelfRef.test_blitter__DOT__cpu_we = 1U;
    Vtest_blitter___024root____VbeforeTrig_h603fc354__0(vlSelf, 
                                                        "@(posedge test_blitter.clk)");
    co_await vlSelfRef.__VtrigSched_h603fc354__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge test_blitter.clk)", 
                                                         "test_blitter.sv", 
                                                         115);
    vlSelfRef.test_blitter__DOT__cpu_we = 0U;
    __Vtask_test_blitter__DOT__blt_reg__84__data = 
        (0x000000ffU & (__Vtask_test_blitter__DOT__setup_copy__75__src_addr 
                        >> 0x10U));
    __Vtask_test_blitter__DOT__blt_reg__84__off = 7U;
    __Vtask_test_blitter__DOT__write_reg__85__data 
        = __Vtask_test_blitter__DOT__blt_reg__84__data;
    __Vtask_test_blitter__DOT__write_reg__85__addr 
        = (0x0000ffffU & ((IData)(0xba83U) + __Vtask_test_blitter__DOT__blt_reg__84__off));
    Vtest_blitter___024root____VbeforeTrig_h603fc354__0(vlSelf, 
                                                        "@(posedge test_blitter.clk)");
    co_await vlSelfRef.__VtrigSched_h603fc354__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge test_blitter.clk)", 
                                                         "test_blitter.sv", 
                                                         113);
    vlSelfRef.test_blitter__DOT__cpu_addr = __Vtask_test_blitter__DOT__write_reg__85__addr;
    vlSelfRef.test_blitter__DOT__cpu_wdata = __Vtask_test_blitter__DOT__write_reg__85__data;
    vlSelfRef.test_blitter__DOT__cpu_we = 1U;
    Vtest_blitter___024root____VbeforeTrig_h603fc354__0(vlSelf, 
                                                        "@(posedge test_blitter.clk)");
    co_await vlSelfRef.__VtrigSched_h603fc354__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge test_blitter.clk)", 
                                                         "test_blitter.sv", 
                                                         115);
    vlSelfRef.test_blitter__DOT__cpu_we = 0U;
    __Vtask_test_blitter__DOT__blt_reg__86__data = 
        (0x000000ffU & __Vtask_test_blitter__DOT__setup_copy__75__dst_addr);
    __Vtask_test_blitter__DOT__blt_reg__86__off = 8U;
    __Vtask_test_blitter__DOT__write_reg__87__data 
        = __Vtask_test_blitter__DOT__blt_reg__86__data;
    __Vtask_test_blitter__DOT__write_reg__87__addr 
        = (0x0000ffffU & ((IData)(0xba83U) + __Vtask_test_blitter__DOT__blt_reg__86__off));
    Vtest_blitter___024root____VbeforeTrig_h603fc354__0(vlSelf, 
                                                        "@(posedge test_blitter.clk)");
    co_await vlSelfRef.__VtrigSched_h603fc354__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge test_blitter.clk)", 
                                                         "test_blitter.sv", 
                                                         113);
    vlSelfRef.test_blitter__DOT__cpu_addr = __Vtask_test_blitter__DOT__write_reg__87__addr;
    vlSelfRef.test_blitter__DOT__cpu_wdata = __Vtask_test_blitter__DOT__write_reg__87__data;
    vlSelfRef.test_blitter__DOT__cpu_we = 1U;
    Vtest_blitter___024root____VbeforeTrig_h603fc354__0(vlSelf, 
                                                        "@(posedge test_blitter.clk)");
    co_await vlSelfRef.__VtrigSched_h603fc354__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge test_blitter.clk)", 
                                                         "test_blitter.sv", 
                                                         115);
    vlSelfRef.test_blitter__DOT__cpu_we = 0U;
    __Vtask_test_blitter__DOT__blt_reg__88__data = 
        (0x000000ffU & (__Vtask_test_blitter__DOT__setup_copy__75__dst_addr 
                        >> 8U));
    __Vtask_test_blitter__DOT__blt_reg__88__off = 9U;
    __Vtask_test_blitter__DOT__write_reg__89__data 
        = __Vtask_test_blitter__DOT__blt_reg__88__data;
    __Vtask_test_blitter__DOT__write_reg__89__addr 
        = (0x0000ffffU & ((IData)(0xba83U) + __Vtask_test_blitter__DOT__blt_reg__88__off));
    Vtest_blitter___024root____VbeforeTrig_h603fc354__0(vlSelf, 
                                                        "@(posedge test_blitter.clk)");
    co_await vlSelfRef.__VtrigSched_h603fc354__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge test_blitter.clk)", 
                                                         "test_blitter.sv", 
                                                         113);
    vlSelfRef.test_blitter__DOT__cpu_addr = __Vtask_test_blitter__DOT__write_reg__89__addr;
    vlSelfRef.test_blitter__DOT__cpu_wdata = __Vtask_test_blitter__DOT__write_reg__89__data;
    vlSelfRef.test_blitter__DOT__cpu_we = 1U;
    Vtest_blitter___024root____VbeforeTrig_h603fc354__0(vlSelf, 
                                                        "@(posedge test_blitter.clk)");
    co_await vlSelfRef.__VtrigSched_h603fc354__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge test_blitter.clk)", 
                                                         "test_blitter.sv", 
                                                         115);
    vlSelfRef.test_blitter__DOT__cpu_we = 0U;
    __Vtask_test_blitter__DOT__blt_reg__90__data = 
        (0x000000ffU & (__Vtask_test_blitter__DOT__setup_copy__75__dst_addr 
                        >> 0x10U));
    __Vtask_test_blitter__DOT__blt_reg__90__off = 0x0000000aU;
    __Vtask_test_blitter__DOT__write_reg__91__data 
        = __Vtask_test_blitter__DOT__blt_reg__90__data;
    __Vtask_test_blitter__DOT__write_reg__91__addr 
        = (0x0000ffffU & ((IData)(0xba83U) + __Vtask_test_blitter__DOT__blt_reg__90__off));
    Vtest_blitter___024root____VbeforeTrig_h603fc354__0(vlSelf, 
                                                        "@(posedge test_blitter.clk)");
    co_await vlSelfRef.__VtrigSched_h603fc354__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge test_blitter.clk)", 
                                                         "test_blitter.sv", 
                                                         113);
    vlSelfRef.test_blitter__DOT__cpu_addr = __Vtask_test_blitter__DOT__write_reg__91__addr;
    vlSelfRef.test_blitter__DOT__cpu_wdata = __Vtask_test_blitter__DOT__write_reg__91__data;
    vlSelfRef.test_blitter__DOT__cpu_we = 1U;
    Vtest_blitter___024root____VbeforeTrig_h603fc354__0(vlSelf, 
                                                        "@(posedge test_blitter.clk)");
    co_await vlSelfRef.__VtrigSched_h603fc354__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge test_blitter.clk)", 
                                                         "test_blitter.sv", 
                                                         115);
    vlSelfRef.test_blitter__DOT__cpu_we = 0U;
    __Vtask_test_blitter__DOT__blt_reg__92__data = 
        (0x000000ffU & __Vtask_test_blitter__DOT__setup_copy__75__width);
    __Vtask_test_blitter__DOT__blt_reg__92__off = 0x0000000bU;
    __Vtask_test_blitter__DOT__write_reg__93__data 
        = __Vtask_test_blitter__DOT__blt_reg__92__data;
    __Vtask_test_blitter__DOT__write_reg__93__addr 
        = (0x0000ffffU & ((IData)(0xba83U) + __Vtask_test_blitter__DOT__blt_reg__92__off));
    Vtest_blitter___024root____VbeforeTrig_h603fc354__0(vlSelf, 
                                                        "@(posedge test_blitter.clk)");
    co_await vlSelfRef.__VtrigSched_h603fc354__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge test_blitter.clk)", 
                                                         "test_blitter.sv", 
                                                         113);
    vlSelfRef.test_blitter__DOT__cpu_addr = __Vtask_test_blitter__DOT__write_reg__93__addr;
    vlSelfRef.test_blitter__DOT__cpu_wdata = __Vtask_test_blitter__DOT__write_reg__93__data;
    vlSelfRef.test_blitter__DOT__cpu_we = 1U;
    Vtest_blitter___024root____VbeforeTrig_h603fc354__0(vlSelf, 
                                                        "@(posedge test_blitter.clk)");
    co_await vlSelfRef.__VtrigSched_h603fc354__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge test_blitter.clk)", 
                                                         "test_blitter.sv", 
                                                         115);
    vlSelfRef.test_blitter__DOT__cpu_we = 0U;
    __Vtask_test_blitter__DOT__blt_reg__94__data = 
        (0x000000ffU & (__Vtask_test_blitter__DOT__setup_copy__75__width 
                        >> 8U));
    __Vtask_test_blitter__DOT__blt_reg__94__off = 0x0000000cU;
    __Vtask_test_blitter__DOT__write_reg__95__data 
        = __Vtask_test_blitter__DOT__blt_reg__94__data;
    __Vtask_test_blitter__DOT__write_reg__95__addr 
        = (0x0000ffffU & ((IData)(0xba83U) + __Vtask_test_blitter__DOT__blt_reg__94__off));
    Vtest_blitter___024root____VbeforeTrig_h603fc354__0(vlSelf, 
                                                        "@(posedge test_blitter.clk)");
    co_await vlSelfRef.__VtrigSched_h603fc354__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge test_blitter.clk)", 
                                                         "test_blitter.sv", 
                                                         113);
    vlSelfRef.test_blitter__DOT__cpu_addr = __Vtask_test_blitter__DOT__write_reg__95__addr;
    vlSelfRef.test_blitter__DOT__cpu_wdata = __Vtask_test_blitter__DOT__write_reg__95__data;
    vlSelfRef.test_blitter__DOT__cpu_we = 1U;
    Vtest_blitter___024root____VbeforeTrig_h603fc354__0(vlSelf, 
                                                        "@(posedge test_blitter.clk)");
    co_await vlSelfRef.__VtrigSched_h603fc354__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge test_blitter.clk)", 
                                                         "test_blitter.sv", 
                                                         115);
    vlSelfRef.test_blitter__DOT__cpu_we = 0U;
    __Vtask_test_blitter__DOT__blt_reg__96__data = 
        (0x000000ffU & __Vtask_test_blitter__DOT__setup_copy__75__height);
    __Vtask_test_blitter__DOT__blt_reg__96__off = 0x0000000dU;
    __Vtask_test_blitter__DOT__write_reg__97__data 
        = __Vtask_test_blitter__DOT__blt_reg__96__data;
    __Vtask_test_blitter__DOT__write_reg__97__addr 
        = (0x0000ffffU & ((IData)(0xba83U) + __Vtask_test_blitter__DOT__blt_reg__96__off));
    Vtest_blitter___024root____VbeforeTrig_h603fc354__0(vlSelf, 
                                                        "@(posedge test_blitter.clk)");
    co_await vlSelfRef.__VtrigSched_h603fc354__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge test_blitter.clk)", 
                                                         "test_blitter.sv", 
                                                         113);
    vlSelfRef.test_blitter__DOT__cpu_addr = __Vtask_test_blitter__DOT__write_reg__97__addr;
    vlSelfRef.test_blitter__DOT__cpu_wdata = __Vtask_test_blitter__DOT__write_reg__97__data;
    vlSelfRef.test_blitter__DOT__cpu_we = 1U;
    Vtest_blitter___024root____VbeforeTrig_h603fc354__0(vlSelf, 
                                                        "@(posedge test_blitter.clk)");
    co_await vlSelfRef.__VtrigSched_h603fc354__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge test_blitter.clk)", 
                                                         "test_blitter.sv", 
                                                         115);
    vlSelfRef.test_blitter__DOT__cpu_we = 0U;
    __Vtask_test_blitter__DOT__blt_reg__98__data = 
        (0x000000ffU & (__Vtask_test_blitter__DOT__setup_copy__75__height 
                        >> 8U));
    __Vtask_test_blitter__DOT__blt_reg__98__off = 0x0000000eU;
    __Vtask_test_blitter__DOT__write_reg__99__data 
        = __Vtask_test_blitter__DOT__blt_reg__98__data;
    __Vtask_test_blitter__DOT__write_reg__99__addr 
        = (0x0000ffffU & ((IData)(0xba83U) + __Vtask_test_blitter__DOT__blt_reg__98__off));
    Vtest_blitter___024root____VbeforeTrig_h603fc354__0(vlSelf, 
                                                        "@(posedge test_blitter.clk)");
    co_await vlSelfRef.__VtrigSched_h603fc354__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge test_blitter.clk)", 
                                                         "test_blitter.sv", 
                                                         113);
    vlSelfRef.test_blitter__DOT__cpu_addr = __Vtask_test_blitter__DOT__write_reg__99__addr;
    vlSelfRef.test_blitter__DOT__cpu_wdata = __Vtask_test_blitter__DOT__write_reg__99__data;
    vlSelfRef.test_blitter__DOT__cpu_we = 1U;
    Vtest_blitter___024root____VbeforeTrig_h603fc354__0(vlSelf, 
                                                        "@(posedge test_blitter.clk)");
    co_await vlSelfRef.__VtrigSched_h603fc354__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge test_blitter.clk)", 
                                                         "test_blitter.sv", 
                                                         115);
    vlSelfRef.test_blitter__DOT__cpu_we = 0U;
    __Vtask_test_blitter__DOT__blt_reg__100__data = 
        (0x000000ffU & __Vtask_test_blitter__DOT__setup_copy__75__src_stride);
    __Vtask_test_blitter__DOT__blt_reg__100__off = 0x0000000fU;
    __Vtask_test_blitter__DOT__write_reg__101__data 
        = __Vtask_test_blitter__DOT__blt_reg__100__data;
    __Vtask_test_blitter__DOT__write_reg__101__addr 
        = (0x0000ffffU & ((IData)(0xba83U) + __Vtask_test_blitter__DOT__blt_reg__100__off));
    Vtest_blitter___024root____VbeforeTrig_h603fc354__0(vlSelf, 
                                                        "@(posedge test_blitter.clk)");
    co_await vlSelfRef.__VtrigSched_h603fc354__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge test_blitter.clk)", 
                                                         "test_blitter.sv", 
                                                         113);
    vlSelfRef.test_blitter__DOT__cpu_addr = __Vtask_test_blitter__DOT__write_reg__101__addr;
    vlSelfRef.test_blitter__DOT__cpu_wdata = __Vtask_test_blitter__DOT__write_reg__101__data;
    vlSelfRef.test_blitter__DOT__cpu_we = 1U;
    Vtest_blitter___024root____VbeforeTrig_h603fc354__0(vlSelf, 
                                                        "@(posedge test_blitter.clk)");
    co_await vlSelfRef.__VtrigSched_h603fc354__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge test_blitter.clk)", 
                                                         "test_blitter.sv", 
                                                         115);
    vlSelfRef.test_blitter__DOT__cpu_we = 0U;
    __Vtask_test_blitter__DOT__blt_reg__102__data = 
        (0x000000ffU & (__Vtask_test_blitter__DOT__setup_copy__75__src_stride 
                        >> 8U));
    __Vtask_test_blitter__DOT__blt_reg__102__off = 0x00000010U;
    __Vtask_test_blitter__DOT__write_reg__103__data 
        = __Vtask_test_blitter__DOT__blt_reg__102__data;
    __Vtask_test_blitter__DOT__write_reg__103__addr 
        = (0x0000ffffU & ((IData)(0xba83U) + __Vtask_test_blitter__DOT__blt_reg__102__off));
    Vtest_blitter___024root____VbeforeTrig_h603fc354__0(vlSelf, 
                                                        "@(posedge test_blitter.clk)");
    co_await vlSelfRef.__VtrigSched_h603fc354__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge test_blitter.clk)", 
                                                         "test_blitter.sv", 
                                                         113);
    vlSelfRef.test_blitter__DOT__cpu_addr = __Vtask_test_blitter__DOT__write_reg__103__addr;
    vlSelfRef.test_blitter__DOT__cpu_wdata = __Vtask_test_blitter__DOT__write_reg__103__data;
    vlSelfRef.test_blitter__DOT__cpu_we = 1U;
    Vtest_blitter___024root____VbeforeTrig_h603fc354__0(vlSelf, 
                                                        "@(posedge test_blitter.clk)");
    co_await vlSelfRef.__VtrigSched_h603fc354__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge test_blitter.clk)", 
                                                         "test_blitter.sv", 
                                                         115);
    vlSelfRef.test_blitter__DOT__cpu_we = 0U;
    __Vtask_test_blitter__DOT__blt_reg__104__data = 
        (0x000000ffU & __Vtask_test_blitter__DOT__setup_copy__75__dst_stride);
    __Vtask_test_blitter__DOT__blt_reg__104__off = 0x00000011U;
    __Vtask_test_blitter__DOT__write_reg__105__data 
        = __Vtask_test_blitter__DOT__blt_reg__104__data;
    __Vtask_test_blitter__DOT__write_reg__105__addr 
        = (0x0000ffffU & ((IData)(0xba83U) + __Vtask_test_blitter__DOT__blt_reg__104__off));
    Vtest_blitter___024root____VbeforeTrig_h603fc354__0(vlSelf, 
                                                        "@(posedge test_blitter.clk)");
    co_await vlSelfRef.__VtrigSched_h603fc354__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge test_blitter.clk)", 
                                                         "test_blitter.sv", 
                                                         113);
    vlSelfRef.test_blitter__DOT__cpu_addr = __Vtask_test_blitter__DOT__write_reg__105__addr;
    vlSelfRef.test_blitter__DOT__cpu_wdata = __Vtask_test_blitter__DOT__write_reg__105__data;
    vlSelfRef.test_blitter__DOT__cpu_we = 1U;
    Vtest_blitter___024root____VbeforeTrig_h603fc354__0(vlSelf, 
                                                        "@(posedge test_blitter.clk)");
    co_await vlSelfRef.__VtrigSched_h603fc354__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge test_blitter.clk)", 
                                                         "test_blitter.sv", 
                                                         115);
    vlSelfRef.test_blitter__DOT__cpu_we = 0U;
    __Vtask_test_blitter__DOT__blt_reg__106__data = 
        (0x000000ffU & (__Vtask_test_blitter__DOT__setup_copy__75__dst_stride 
                        >> 8U));
    __Vtask_test_blitter__DOT__blt_reg__106__off = 0x00000012U;
    __Vtask_test_blitter__DOT__write_reg__107__data 
        = __Vtask_test_blitter__DOT__blt_reg__106__data;
    __Vtask_test_blitter__DOT__write_reg__107__addr 
        = (0x0000ffffU & ((IData)(0xba83U) + __Vtask_test_blitter__DOT__blt_reg__106__off));
    Vtest_blitter___024root____VbeforeTrig_h603fc354__0(vlSelf, 
                                                        "@(posedge test_blitter.clk)");
    co_await vlSelfRef.__VtrigSched_h603fc354__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge test_blitter.clk)", 
                                                         "test_blitter.sv", 
                                                         113);
    vlSelfRef.test_blitter__DOT__cpu_addr = __Vtask_test_blitter__DOT__write_reg__107__addr;
    vlSelfRef.test_blitter__DOT__cpu_wdata = __Vtask_test_blitter__DOT__write_reg__107__data;
    vlSelfRef.test_blitter__DOT__cpu_we = 1U;
    Vtest_blitter___024root____VbeforeTrig_h603fc354__0(vlSelf, 
                                                        "@(posedge test_blitter.clk)");
    co_await vlSelfRef.__VtrigSched_h603fc354__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge test_blitter.clk)", 
                                                         "test_blitter.sv", 
                                                         115);
    vlSelfRef.test_blitter__DOT__cpu_we = 0U;
    __Vtask_test_blitter__DOT__blt_reg__108__data = 0U;
    __Vtask_test_blitter__DOT__blt_reg__108__off = 0x00000013U;
    __Vtask_test_blitter__DOT__write_reg__109__data 
        = __Vtask_test_blitter__DOT__blt_reg__108__data;
    __Vtask_test_blitter__DOT__write_reg__109__addr 
        = (0x0000ffffU & ((IData)(0xba83U) + __Vtask_test_blitter__DOT__blt_reg__108__off));
    Vtest_blitter___024root____VbeforeTrig_h603fc354__0(vlSelf, 
                                                        "@(posedge test_blitter.clk)");
    co_await vlSelfRef.__VtrigSched_h603fc354__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge test_blitter.clk)", 
                                                         "test_blitter.sv", 
                                                         113);
    vlSelfRef.test_blitter__DOT__cpu_addr = __Vtask_test_blitter__DOT__write_reg__109__addr;
    vlSelfRef.test_blitter__DOT__cpu_wdata = __Vtask_test_blitter__DOT__write_reg__109__data;
    vlSelfRef.test_blitter__DOT__cpu_we = 1U;
    Vtest_blitter___024root____VbeforeTrig_h603fc354__0(vlSelf, 
                                                        "@(posedge test_blitter.clk)");
    co_await vlSelfRef.__VtrigSched_h603fc354__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge test_blitter.clk)", 
                                                         "test_blitter.sv", 
                                                         115);
    vlSelfRef.test_blitter__DOT__cpu_we = 0U;
    __Vtask_test_blitter__DOT__blt_reg__111__data = 1U;
    __Vtask_test_blitter__DOT__blt_reg__111__off = 0U;
    __Vtask_test_blitter__DOT__write_reg__112__data 
        = __Vtask_test_blitter__DOT__blt_reg__111__data;
    __Vtask_test_blitter__DOT__write_reg__112__addr 
        = (0x0000ffffU & ((IData)(0xba83U) + __Vtask_test_blitter__DOT__blt_reg__111__off));
    Vtest_blitter___024root____VbeforeTrig_h603fc354__0(vlSelf, 
                                                        "@(posedge test_blitter.clk)");
    co_await vlSelfRef.__VtrigSched_h603fc354__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge test_blitter.clk)", 
                                                         "test_blitter.sv", 
                                                         113);
    vlSelfRef.test_blitter__DOT__cpu_addr = __Vtask_test_blitter__DOT__write_reg__112__addr;
    vlSelfRef.test_blitter__DOT__cpu_wdata = __Vtask_test_blitter__DOT__write_reg__112__data;
    vlSelfRef.test_blitter__DOT__cpu_we = 1U;
    Vtest_blitter___024root____VbeforeTrig_h603fc354__0(vlSelf, 
                                                        "@(posedge test_blitter.clk)");
    co_await vlSelfRef.__VtrigSched_h603fc354__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge test_blitter.clk)", 
                                                         "test_blitter.sv", 
                                                         115);
    vlSelfRef.test_blitter__DOT__cpu_we = 0U;
    __Vtask_test_blitter__DOT__wait_blt_done__113__test_blitter__DOT__unnamedblk1_1__DOT____Vrepeat0 = 0;
    __Vtask_test_blitter__DOT__wait_blt_done__113__test_blitter__DOT__unnamedblk1_2__DOT____Vrepeat1 = 0;
    __Vtask_test_blitter__DOT__wait_blt_done__113__test_blitter__DOT__unnamedblk1_1__DOT____Vrepeat0 = 2U;
    while (VL_LTS_III(32, 0U, __Vtask_test_blitter__DOT__wait_blt_done__113__test_blitter__DOT__unnamedblk1_1__DOT____Vrepeat0)) {
        Vtest_blitter___024root____VbeforeTrig_h603fc354__0(vlSelf, 
                                                            "@(posedge test_blitter.clk)");
        co_await vlSelfRef.__VtrigSched_h603fc354__0.trigger(0U, 
                                                             nullptr, 
                                                             "@(posedge test_blitter.clk)", 
                                                             "test_blitter.sv", 
                                                             128);
        __Vtask_test_blitter__DOT__wait_blt_done__113__test_blitter__DOT__unnamedblk1_1__DOT____Vrepeat0 
            = (__Vtask_test_blitter__DOT__wait_blt_done__113__test_blitter__DOT__unnamedblk1_1__DOT____Vrepeat0 
               - (IData)(1U));
    }
    while (((0U != (IData)(vlSelfRef.test_blitter__DOT__dut__DOT__state)) 
            & (6U != (IData)(vlSelfRef.test_blitter__DOT__dut__DOT__state)))) {
        Vtest_blitter___024root____VbeforeTrig_h603fc354__0(vlSelf, 
                                                            "@(posedge test_blitter.clk)");
        co_await vlSelfRef.__VtrigSched_h603fc354__0.trigger(0U, 
                                                             nullptr, 
                                                             "@(posedge test_blitter.clk)", 
                                                             "test_blitter.sv", 
                                                             130);
    }
    __Vtask_test_blitter__DOT__wait_blt_done__113__test_blitter__DOT__unnamedblk1_2__DOT____Vrepeat1 = 2U;
    while (VL_LTS_III(32, 0U, __Vtask_test_blitter__DOT__wait_blt_done__113__test_blitter__DOT__unnamedblk1_2__DOT____Vrepeat1)) {
        Vtest_blitter___024root____VbeforeTrig_h603fc354__0(vlSelf, 
                                                            "@(posedge test_blitter.clk)");
        co_await vlSelfRef.__VtrigSched_h603fc354__0.trigger(0U, 
                                                             nullptr, 
                                                             "@(posedge test_blitter.clk)", 
                                                             "test_blitter.sv", 
                                                             131);
        __Vtask_test_blitter__DOT__wait_blt_done__113__test_blitter__DOT__unnamedblk1_2__DOT____Vrepeat1 
            = (__Vtask_test_blitter__DOT__wait_blt_done__113__test_blitter__DOT__unnamedblk1_2__DOT____Vrepeat1 
               - (IData)(1U));
    }
    __Vtask_test_blitter__DOT__check__114__condition 
        = (2U == vlSelfRef.test_blitter__DOT__dut__DOT__regs[1U]);
    vlSelfRef.__Vtask_test_blitter__DOT__check__114__name = "copy status ok"s;
    vlSelfRef.test_blitter__DOT__test_num = ((IData)(1U) 
                                             + vlSelfRef.test_blitter__DOT__test_num);
    if (__Vtask_test_blitter__DOT__check__114__condition) {
        VL_WRITEF_NX("  PASS [%0d] %@\n",0,32,vlSelfRef.test_blitter__DOT__test_num,
                     -1,&(vlSelfRef.__Vtask_test_blitter__DOT__check__114__name));
        vlSelfRef.test_blitter__DOT__pass_count = ((IData)(1U) 
                                                   + vlSelfRef.test_blitter__DOT__pass_count);
    } else {
        VL_WRITEF_NX("  FAIL [%0d] %@\n",0,32,vlSelfRef.test_blitter__DOT__test_num,
                     -1,&(vlSelfRef.__Vtask_test_blitter__DOT__check__114__name));
        vlSelfRef.test_blitter__DOT__fail_count = ((IData)(1U) 
                                                   + vlSelfRef.test_blitter__DOT__fail_count);
    }
    __Vtask_test_blitter__DOT__check__115__condition 
        = (0x0000000cU == ((vlSelfRef.test_blitter__DOT__dut__DOT__regs[24U] 
                            << 0x00000010U) | ((vlSelfRef.test_blitter__DOT__dut__DOT__regs[23U] 
                                                << 8U) 
                                               | vlSelfRef.test_blitter__DOT__dut__DOT__regs[22U])));
    vlSelfRef.__Vtask_test_blitter__DOT__check__115__name = "count = 12"s;
    vlSelfRef.test_blitter__DOT__test_num = ((IData)(1U) 
                                             + vlSelfRef.test_blitter__DOT__test_num);
    if (__Vtask_test_blitter__DOT__check__115__condition) {
        VL_WRITEF_NX("  PASS [%0d] %@\n",0,32,vlSelfRef.test_blitter__DOT__test_num,
                     -1,&(vlSelfRef.__Vtask_test_blitter__DOT__check__115__name));
        vlSelfRef.test_blitter__DOT__pass_count = ((IData)(1U) 
                                                   + vlSelfRef.test_blitter__DOT__pass_count);
    } else {
        VL_WRITEF_NX("  FAIL [%0d] %@\n",0,32,vlSelfRef.test_blitter__DOT__test_num,
                     -1,&(vlSelfRef.__Vtask_test_blitter__DOT__check__115__name));
        vlSelfRef.test_blitter__DOT__fail_count = ((IData)(1U) 
                                                   + vlSelfRef.test_blitter__DOT__fail_count);
    }
    __Vtask_test_blitter__DOT__check__116__condition 
        = (1U == vlSelfRef.test_blitter__DOT__sim_ram[12288U]);
    vlSelfRef.__Vtask_test_blitter__DOT__check__116__name = "dst[0] = src row0 col0"s;
    vlSelfRef.test_blitter__DOT__test_num = ((IData)(1U) 
                                             + vlSelfRef.test_blitter__DOT__test_num);
    if (__Vtask_test_blitter__DOT__check__116__condition) {
        VL_WRITEF_NX("  PASS [%0d] %@\n",0,32,vlSelfRef.test_blitter__DOT__test_num,
                     -1,&(vlSelfRef.__Vtask_test_blitter__DOT__check__116__name));
        vlSelfRef.test_blitter__DOT__pass_count = ((IData)(1U) 
                                                   + vlSelfRef.test_blitter__DOT__pass_count);
    } else {
        VL_WRITEF_NX("  FAIL [%0d] %@\n",0,32,vlSelfRef.test_blitter__DOT__test_num,
                     -1,&(vlSelfRef.__Vtask_test_blitter__DOT__check__116__name));
        vlSelfRef.test_blitter__DOT__fail_count = ((IData)(1U) 
                                                   + vlSelfRef.test_blitter__DOT__fail_count);
    }
    __Vtask_test_blitter__DOT__check__117__condition 
        = (4U == vlSelfRef.test_blitter__DOT__sim_ram[12291U]);
    vlSelfRef.__Vtask_test_blitter__DOT__check__117__name = "dst[3] = src row0 col3"s;
    vlSelfRef.test_blitter__DOT__test_num = ((IData)(1U) 
                                             + vlSelfRef.test_blitter__DOT__test_num);
    if (__Vtask_test_blitter__DOT__check__117__condition) {
        VL_WRITEF_NX("  PASS [%0d] %@\n",0,32,vlSelfRef.test_blitter__DOT__test_num,
                     -1,&(vlSelfRef.__Vtask_test_blitter__DOT__check__117__name));
        vlSelfRef.test_blitter__DOT__pass_count = ((IData)(1U) 
                                                   + vlSelfRef.test_blitter__DOT__pass_count);
    } else {
        VL_WRITEF_NX("  FAIL [%0d] %@\n",0,32,vlSelfRef.test_blitter__DOT__test_num,
                     -1,&(vlSelfRef.__Vtask_test_blitter__DOT__check__117__name));
        vlSelfRef.test_blitter__DOT__fail_count = ((IData)(1U) 
                                                   + vlSelfRef.test_blitter__DOT__fail_count);
    }
    __Vtask_test_blitter__DOT__check__118__condition 
        = (0x0bU == vlSelfRef.test_blitter__DOT__sim_ram[12294U]);
    vlSelfRef.__Vtask_test_blitter__DOT__check__118__name = "dst[6] = src row1 col0"s;
    vlSelfRef.test_blitter__DOT__test_num = ((IData)(1U) 
                                             + vlSelfRef.test_blitter__DOT__test_num);
    if (__Vtask_test_blitter__DOT__check__118__condition) {
        VL_WRITEF_NX("  PASS [%0d] %@\n",0,32,vlSelfRef.test_blitter__DOT__test_num,
                     -1,&(vlSelfRef.__Vtask_test_blitter__DOT__check__118__name));
        vlSelfRef.test_blitter__DOT__pass_count = ((IData)(1U) 
                                                   + vlSelfRef.test_blitter__DOT__pass_count);
    } else {
        VL_WRITEF_NX("  FAIL [%0d] %@\n",0,32,vlSelfRef.test_blitter__DOT__test_num,
                     -1,&(vlSelfRef.__Vtask_test_blitter__DOT__check__118__name));
        vlSelfRef.test_blitter__DOT__fail_count = ((IData)(1U) 
                                                   + vlSelfRef.test_blitter__DOT__fail_count);
    }
    __Vtask_test_blitter__DOT__check__119__condition 
        = (0x15U == vlSelfRef.test_blitter__DOT__sim_ram[12300U]);
    vlSelfRef.__Vtask_test_blitter__DOT__check__119__name = "dst[12] = src row2 col0"s;
    vlSelfRef.test_blitter__DOT__test_num = ((IData)(1U) 
                                             + vlSelfRef.test_blitter__DOT__test_num);
    if (__Vtask_test_blitter__DOT__check__119__condition) {
        VL_WRITEF_NX("  PASS [%0d] %@\n",0,32,vlSelfRef.test_blitter__DOT__test_num,
                     -1,&(vlSelfRef.__Vtask_test_blitter__DOT__check__119__name));
        vlSelfRef.test_blitter__DOT__pass_count = ((IData)(1U) 
                                                   + vlSelfRef.test_blitter__DOT__pass_count);
    } else {
        VL_WRITEF_NX("  FAIL [%0d] %@\n",0,32,vlSelfRef.test_blitter__DOT__test_num,
                     -1,&(vlSelfRef.__Vtask_test_blitter__DOT__check__119__name));
        vlSelfRef.test_blitter__DOT__fail_count = ((IData)(1U) 
                                                   + vlSelfRef.test_blitter__DOT__fail_count);
    }
    VL_WRITEF_NX("Test: Copy RAM to VGC gfx\n",0);
    vlSelfRef.test_blitter__DOT__sim_ram[16384U] = 1U;
    vlSelfRef.test_blitter__DOT__sim_ram[16385U] = 2U;
    vlSelfRef.test_blitter__DOT__sim_ram[16386U] = 3U;
    vlSelfRef.test_blitter__DOT__sim_ram[16387U] = 4U;
    vlSelfRef.test_blitter__DOT__sim_ram[16388U] = 5U;
    vlSelfRef.test_blitter__DOT__sim_ram[16389U] = 6U;
    vlSelfRef.test_blitter__DOT__sim_ram[16390U] = 7U;
    vlSelfRef.test_blitter__DOT__sim_ram[16391U] = 8U;
    vlSelfRef.test_blitter__DOT__sim_ram[16392U] = 9U;
    vlSelfRef.test_blitter__DOT__sim_ram[16393U] = 0x0aU;
    __Vtask_test_blitter__DOT__setup_copy__120__height = 1U;
    __Vtask_test_blitter__DOT__setup_copy__120__width = 0x0000000aU;
    __Vtask_test_blitter__DOT__setup_copy__120__dst_stride = 0x00000140U;
    __Vtask_test_blitter__DOT__setup_copy__120__dst_addr = 0x00000200U;
    __Vtask_test_blitter__DOT__setup_copy__120__dst_space = 3U;
    __Vtask_test_blitter__DOT__setup_copy__120__src_stride = 0x0000000aU;
    __Vtask_test_blitter__DOT__setup_copy__120__src_addr = 0x00004000U;
    __Vtask_test_blitter__DOT__setup_copy__120__src_space = 0U;
    __Vtask_test_blitter__DOT__blt_reg__121__data = 
        (0x000000ffU & __Vtask_test_blitter__DOT__setup_copy__120__src_space);
    __Vtask_test_blitter__DOT__blt_reg__121__off = 3U;
    __Vtask_test_blitter__DOT__write_reg__122__data 
        = __Vtask_test_blitter__DOT__blt_reg__121__data;
    __Vtask_test_blitter__DOT__write_reg__122__addr 
        = (0x0000ffffU & ((IData)(0xba83U) + __Vtask_test_blitter__DOT__blt_reg__121__off));
    Vtest_blitter___024root____VbeforeTrig_h603fc354__0(vlSelf, 
                                                        "@(posedge test_blitter.clk)");
    co_await vlSelfRef.__VtrigSched_h603fc354__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge test_blitter.clk)", 
                                                         "test_blitter.sv", 
                                                         113);
    vlSelfRef.test_blitter__DOT__cpu_addr = __Vtask_test_blitter__DOT__write_reg__122__addr;
    vlSelfRef.test_blitter__DOT__cpu_wdata = __Vtask_test_blitter__DOT__write_reg__122__data;
    vlSelfRef.test_blitter__DOT__cpu_we = 1U;
    Vtest_blitter___024root____VbeforeTrig_h603fc354__0(vlSelf, 
                                                        "@(posedge test_blitter.clk)");
    co_await vlSelfRef.__VtrigSched_h603fc354__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge test_blitter.clk)", 
                                                         "test_blitter.sv", 
                                                         115);
    vlSelfRef.test_blitter__DOT__cpu_we = 0U;
    __Vtask_test_blitter__DOT__blt_reg__123__data = 
        (0x000000ffU & __Vtask_test_blitter__DOT__setup_copy__120__dst_space);
    __Vtask_test_blitter__DOT__blt_reg__123__off = 4U;
    __Vtask_test_blitter__DOT__write_reg__124__data 
        = __Vtask_test_blitter__DOT__blt_reg__123__data;
    __Vtask_test_blitter__DOT__write_reg__124__addr 
        = (0x0000ffffU & ((IData)(0xba83U) + __Vtask_test_blitter__DOT__blt_reg__123__off));
    Vtest_blitter___024root____VbeforeTrig_h603fc354__0(vlSelf, 
                                                        "@(posedge test_blitter.clk)");
    co_await vlSelfRef.__VtrigSched_h603fc354__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge test_blitter.clk)", 
                                                         "test_blitter.sv", 
                                                         113);
    vlSelfRef.test_blitter__DOT__cpu_addr = __Vtask_test_blitter__DOT__write_reg__124__addr;
    vlSelfRef.test_blitter__DOT__cpu_wdata = __Vtask_test_blitter__DOT__write_reg__124__data;
    vlSelfRef.test_blitter__DOT__cpu_we = 1U;
    Vtest_blitter___024root____VbeforeTrig_h603fc354__0(vlSelf, 
                                                        "@(posedge test_blitter.clk)");
    co_await vlSelfRef.__VtrigSched_h603fc354__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge test_blitter.clk)", 
                                                         "test_blitter.sv", 
                                                         115);
    vlSelfRef.test_blitter__DOT__cpu_we = 0U;
    __Vtask_test_blitter__DOT__blt_reg__125__data = 
        (0x000000ffU & __Vtask_test_blitter__DOT__setup_copy__120__src_addr);
    __Vtask_test_blitter__DOT__blt_reg__125__off = 5U;
    __Vtask_test_blitter__DOT__write_reg__126__data 
        = __Vtask_test_blitter__DOT__blt_reg__125__data;
    __Vtask_test_blitter__DOT__write_reg__126__addr 
        = (0x0000ffffU & ((IData)(0xba83U) + __Vtask_test_blitter__DOT__blt_reg__125__off));
    Vtest_blitter___024root____VbeforeTrig_h603fc354__0(vlSelf, 
                                                        "@(posedge test_blitter.clk)");
    co_await vlSelfRef.__VtrigSched_h603fc354__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge test_blitter.clk)", 
                                                         "test_blitter.sv", 
                                                         113);
    vlSelfRef.test_blitter__DOT__cpu_addr = __Vtask_test_blitter__DOT__write_reg__126__addr;
    vlSelfRef.test_blitter__DOT__cpu_wdata = __Vtask_test_blitter__DOT__write_reg__126__data;
    vlSelfRef.test_blitter__DOT__cpu_we = 1U;
    Vtest_blitter___024root____VbeforeTrig_h603fc354__0(vlSelf, 
                                                        "@(posedge test_blitter.clk)");
    co_await vlSelfRef.__VtrigSched_h603fc354__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge test_blitter.clk)", 
                                                         "test_blitter.sv", 
                                                         115);
    vlSelfRef.test_blitter__DOT__cpu_we = 0U;
    __Vtask_test_blitter__DOT__blt_reg__127__data = 
        (0x000000ffU & (__Vtask_test_blitter__DOT__setup_copy__120__src_addr 
                        >> 8U));
    __Vtask_test_blitter__DOT__blt_reg__127__off = 6U;
    __Vtask_test_blitter__DOT__write_reg__128__data 
        = __Vtask_test_blitter__DOT__blt_reg__127__data;
    __Vtask_test_blitter__DOT__write_reg__128__addr 
        = (0x0000ffffU & ((IData)(0xba83U) + __Vtask_test_blitter__DOT__blt_reg__127__off));
    Vtest_blitter___024root____VbeforeTrig_h603fc354__0(vlSelf, 
                                                        "@(posedge test_blitter.clk)");
    co_await vlSelfRef.__VtrigSched_h603fc354__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge test_blitter.clk)", 
                                                         "test_blitter.sv", 
                                                         113);
    vlSelfRef.test_blitter__DOT__cpu_addr = __Vtask_test_blitter__DOT__write_reg__128__addr;
    vlSelfRef.test_blitter__DOT__cpu_wdata = __Vtask_test_blitter__DOT__write_reg__128__data;
    vlSelfRef.test_blitter__DOT__cpu_we = 1U;
    Vtest_blitter___024root____VbeforeTrig_h603fc354__0(vlSelf, 
                                                        "@(posedge test_blitter.clk)");
    co_await vlSelfRef.__VtrigSched_h603fc354__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge test_blitter.clk)", 
                                                         "test_blitter.sv", 
                                                         115);
    vlSelfRef.test_blitter__DOT__cpu_we = 0U;
    __Vtask_test_blitter__DOT__blt_reg__129__data = 
        (0x000000ffU & (__Vtask_test_blitter__DOT__setup_copy__120__src_addr 
                        >> 0x10U));
    __Vtask_test_blitter__DOT__blt_reg__129__off = 7U;
    __Vtask_test_blitter__DOT__write_reg__130__data 
        = __Vtask_test_blitter__DOT__blt_reg__129__data;
    __Vtask_test_blitter__DOT__write_reg__130__addr 
        = (0x0000ffffU & ((IData)(0xba83U) + __Vtask_test_blitter__DOT__blt_reg__129__off));
    Vtest_blitter___024root____VbeforeTrig_h603fc354__0(vlSelf, 
                                                        "@(posedge test_blitter.clk)");
    co_await vlSelfRef.__VtrigSched_h603fc354__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge test_blitter.clk)", 
                                                         "test_blitter.sv", 
                                                         113);
    vlSelfRef.test_blitter__DOT__cpu_addr = __Vtask_test_blitter__DOT__write_reg__130__addr;
    vlSelfRef.test_blitter__DOT__cpu_wdata = __Vtask_test_blitter__DOT__write_reg__130__data;
    vlSelfRef.test_blitter__DOT__cpu_we = 1U;
    Vtest_blitter___024root____VbeforeTrig_h603fc354__0(vlSelf, 
                                                        "@(posedge test_blitter.clk)");
    co_await vlSelfRef.__VtrigSched_h603fc354__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge test_blitter.clk)", 
                                                         "test_blitter.sv", 
                                                         115);
    vlSelfRef.test_blitter__DOT__cpu_we = 0U;
    __Vtask_test_blitter__DOT__blt_reg__131__data = 
        (0x000000ffU & __Vtask_test_blitter__DOT__setup_copy__120__dst_addr);
    __Vtask_test_blitter__DOT__blt_reg__131__off = 8U;
    __Vtask_test_blitter__DOT__write_reg__132__data 
        = __Vtask_test_blitter__DOT__blt_reg__131__data;
    __Vtask_test_blitter__DOT__write_reg__132__addr 
        = (0x0000ffffU & ((IData)(0xba83U) + __Vtask_test_blitter__DOT__blt_reg__131__off));
    Vtest_blitter___024root____VbeforeTrig_h603fc354__0(vlSelf, 
                                                        "@(posedge test_blitter.clk)");
    co_await vlSelfRef.__VtrigSched_h603fc354__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge test_blitter.clk)", 
                                                         "test_blitter.sv", 
                                                         113);
    vlSelfRef.test_blitter__DOT__cpu_addr = __Vtask_test_blitter__DOT__write_reg__132__addr;
    vlSelfRef.test_blitter__DOT__cpu_wdata = __Vtask_test_blitter__DOT__write_reg__132__data;
    vlSelfRef.test_blitter__DOT__cpu_we = 1U;
    Vtest_blitter___024root____VbeforeTrig_h603fc354__0(vlSelf, 
                                                        "@(posedge test_blitter.clk)");
    co_await vlSelfRef.__VtrigSched_h603fc354__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge test_blitter.clk)", 
                                                         "test_blitter.sv", 
                                                         115);
    vlSelfRef.test_blitter__DOT__cpu_we = 0U;
    __Vtask_test_blitter__DOT__blt_reg__133__data = 
        (0x000000ffU & (__Vtask_test_blitter__DOT__setup_copy__120__dst_addr 
                        >> 8U));
    __Vtask_test_blitter__DOT__blt_reg__133__off = 9U;
    __Vtask_test_blitter__DOT__write_reg__134__data 
        = __Vtask_test_blitter__DOT__blt_reg__133__data;
    __Vtask_test_blitter__DOT__write_reg__134__addr 
        = (0x0000ffffU & ((IData)(0xba83U) + __Vtask_test_blitter__DOT__blt_reg__133__off));
    Vtest_blitter___024root____VbeforeTrig_h603fc354__0(vlSelf, 
                                                        "@(posedge test_blitter.clk)");
    co_await vlSelfRef.__VtrigSched_h603fc354__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge test_blitter.clk)", 
                                                         "test_blitter.sv", 
                                                         113);
    vlSelfRef.test_blitter__DOT__cpu_addr = __Vtask_test_blitter__DOT__write_reg__134__addr;
    vlSelfRef.test_blitter__DOT__cpu_wdata = __Vtask_test_blitter__DOT__write_reg__134__data;
    vlSelfRef.test_blitter__DOT__cpu_we = 1U;
    Vtest_blitter___024root____VbeforeTrig_h603fc354__0(vlSelf, 
                                                        "@(posedge test_blitter.clk)");
    co_await vlSelfRef.__VtrigSched_h603fc354__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge test_blitter.clk)", 
                                                         "test_blitter.sv", 
                                                         115);
    vlSelfRef.test_blitter__DOT__cpu_we = 0U;
    __Vtask_test_blitter__DOT__blt_reg__135__data = 
        (0x000000ffU & (__Vtask_test_blitter__DOT__setup_copy__120__dst_addr 
                        >> 0x10U));
    __Vtask_test_blitter__DOT__blt_reg__135__off = 0x0000000aU;
    __Vtask_test_blitter__DOT__write_reg__136__data 
        = __Vtask_test_blitter__DOT__blt_reg__135__data;
    __Vtask_test_blitter__DOT__write_reg__136__addr 
        = (0x0000ffffU & ((IData)(0xba83U) + __Vtask_test_blitter__DOT__blt_reg__135__off));
    Vtest_blitter___024root____VbeforeTrig_h603fc354__0(vlSelf, 
                                                        "@(posedge test_blitter.clk)");
    co_await vlSelfRef.__VtrigSched_h603fc354__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge test_blitter.clk)", 
                                                         "test_blitter.sv", 
                                                         113);
    vlSelfRef.test_blitter__DOT__cpu_addr = __Vtask_test_blitter__DOT__write_reg__136__addr;
    vlSelfRef.test_blitter__DOT__cpu_wdata = __Vtask_test_blitter__DOT__write_reg__136__data;
    vlSelfRef.test_blitter__DOT__cpu_we = 1U;
    Vtest_blitter___024root____VbeforeTrig_h603fc354__0(vlSelf, 
                                                        "@(posedge test_blitter.clk)");
    co_await vlSelfRef.__VtrigSched_h603fc354__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge test_blitter.clk)", 
                                                         "test_blitter.sv", 
                                                         115);
    vlSelfRef.test_blitter__DOT__cpu_we = 0U;
    __Vtask_test_blitter__DOT__blt_reg__137__data = 
        (0x000000ffU & __Vtask_test_blitter__DOT__setup_copy__120__width);
    __Vtask_test_blitter__DOT__blt_reg__137__off = 0x0000000bU;
    __Vtask_test_blitter__DOT__write_reg__138__data 
        = __Vtask_test_blitter__DOT__blt_reg__137__data;
    __Vtask_test_blitter__DOT__write_reg__138__addr 
        = (0x0000ffffU & ((IData)(0xba83U) + __Vtask_test_blitter__DOT__blt_reg__137__off));
    Vtest_blitter___024root____VbeforeTrig_h603fc354__0(vlSelf, 
                                                        "@(posedge test_blitter.clk)");
    co_await vlSelfRef.__VtrigSched_h603fc354__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge test_blitter.clk)", 
                                                         "test_blitter.sv", 
                                                         113);
    vlSelfRef.test_blitter__DOT__cpu_addr = __Vtask_test_blitter__DOT__write_reg__138__addr;
    vlSelfRef.test_blitter__DOT__cpu_wdata = __Vtask_test_blitter__DOT__write_reg__138__data;
    vlSelfRef.test_blitter__DOT__cpu_we = 1U;
    Vtest_blitter___024root____VbeforeTrig_h603fc354__0(vlSelf, 
                                                        "@(posedge test_blitter.clk)");
    co_await vlSelfRef.__VtrigSched_h603fc354__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge test_blitter.clk)", 
                                                         "test_blitter.sv", 
                                                         115);
    vlSelfRef.test_blitter__DOT__cpu_we = 0U;
    __Vtask_test_blitter__DOT__blt_reg__139__data = 
        (0x000000ffU & (__Vtask_test_blitter__DOT__setup_copy__120__width 
                        >> 8U));
    __Vtask_test_blitter__DOT__blt_reg__139__off = 0x0000000cU;
    __Vtask_test_blitter__DOT__write_reg__140__data 
        = __Vtask_test_blitter__DOT__blt_reg__139__data;
    __Vtask_test_blitter__DOT__write_reg__140__addr 
        = (0x0000ffffU & ((IData)(0xba83U) + __Vtask_test_blitter__DOT__blt_reg__139__off));
    Vtest_blitter___024root____VbeforeTrig_h603fc354__0(vlSelf, 
                                                        "@(posedge test_blitter.clk)");
    co_await vlSelfRef.__VtrigSched_h603fc354__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge test_blitter.clk)", 
                                                         "test_blitter.sv", 
                                                         113);
    vlSelfRef.test_blitter__DOT__cpu_addr = __Vtask_test_blitter__DOT__write_reg__140__addr;
    vlSelfRef.test_blitter__DOT__cpu_wdata = __Vtask_test_blitter__DOT__write_reg__140__data;
    vlSelfRef.test_blitter__DOT__cpu_we = 1U;
    Vtest_blitter___024root____VbeforeTrig_h603fc354__0(vlSelf, 
                                                        "@(posedge test_blitter.clk)");
    co_await vlSelfRef.__VtrigSched_h603fc354__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge test_blitter.clk)", 
                                                         "test_blitter.sv", 
                                                         115);
    vlSelfRef.test_blitter__DOT__cpu_we = 0U;
    __Vtask_test_blitter__DOT__blt_reg__141__data = 
        (0x000000ffU & __Vtask_test_blitter__DOT__setup_copy__120__height);
    __Vtask_test_blitter__DOT__blt_reg__141__off = 0x0000000dU;
    __Vtask_test_blitter__DOT__write_reg__142__data 
        = __Vtask_test_blitter__DOT__blt_reg__141__data;
    __Vtask_test_blitter__DOT__write_reg__142__addr 
        = (0x0000ffffU & ((IData)(0xba83U) + __Vtask_test_blitter__DOT__blt_reg__141__off));
    Vtest_blitter___024root____VbeforeTrig_h603fc354__0(vlSelf, 
                                                        "@(posedge test_blitter.clk)");
    co_await vlSelfRef.__VtrigSched_h603fc354__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge test_blitter.clk)", 
                                                         "test_blitter.sv", 
                                                         113);
    vlSelfRef.test_blitter__DOT__cpu_addr = __Vtask_test_blitter__DOT__write_reg__142__addr;
    vlSelfRef.test_blitter__DOT__cpu_wdata = __Vtask_test_blitter__DOT__write_reg__142__data;
    vlSelfRef.test_blitter__DOT__cpu_we = 1U;
    Vtest_blitter___024root____VbeforeTrig_h603fc354__0(vlSelf, 
                                                        "@(posedge test_blitter.clk)");
    co_await vlSelfRef.__VtrigSched_h603fc354__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge test_blitter.clk)", 
                                                         "test_blitter.sv", 
                                                         115);
    vlSelfRef.test_blitter__DOT__cpu_we = 0U;
    __Vtask_test_blitter__DOT__blt_reg__143__data = 
        (0x000000ffU & (__Vtask_test_blitter__DOT__setup_copy__120__height 
                        >> 8U));
    __Vtask_test_blitter__DOT__blt_reg__143__off = 0x0000000eU;
    __Vtask_test_blitter__DOT__write_reg__144__data 
        = __Vtask_test_blitter__DOT__blt_reg__143__data;
    __Vtask_test_blitter__DOT__write_reg__144__addr 
        = (0x0000ffffU & ((IData)(0xba83U) + __Vtask_test_blitter__DOT__blt_reg__143__off));
    Vtest_blitter___024root____VbeforeTrig_h603fc354__0(vlSelf, 
                                                        "@(posedge test_blitter.clk)");
    co_await vlSelfRef.__VtrigSched_h603fc354__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge test_blitter.clk)", 
                                                         "test_blitter.sv", 
                                                         113);
    vlSelfRef.test_blitter__DOT__cpu_addr = __Vtask_test_blitter__DOT__write_reg__144__addr;
    vlSelfRef.test_blitter__DOT__cpu_wdata = __Vtask_test_blitter__DOT__write_reg__144__data;
    vlSelfRef.test_blitter__DOT__cpu_we = 1U;
    Vtest_blitter___024root____VbeforeTrig_h603fc354__0(vlSelf, 
                                                        "@(posedge test_blitter.clk)");
    co_await vlSelfRef.__VtrigSched_h603fc354__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge test_blitter.clk)", 
                                                         "test_blitter.sv", 
                                                         115);
    vlSelfRef.test_blitter__DOT__cpu_we = 0U;
    __Vtask_test_blitter__DOT__blt_reg__145__data = 
        (0x000000ffU & __Vtask_test_blitter__DOT__setup_copy__120__src_stride);
    __Vtask_test_blitter__DOT__blt_reg__145__off = 0x0000000fU;
    __Vtask_test_blitter__DOT__write_reg__146__data 
        = __Vtask_test_blitter__DOT__blt_reg__145__data;
    __Vtask_test_blitter__DOT__write_reg__146__addr 
        = (0x0000ffffU & ((IData)(0xba83U) + __Vtask_test_blitter__DOT__blt_reg__145__off));
    Vtest_blitter___024root____VbeforeTrig_h603fc354__0(vlSelf, 
                                                        "@(posedge test_blitter.clk)");
    co_await vlSelfRef.__VtrigSched_h603fc354__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge test_blitter.clk)", 
                                                         "test_blitter.sv", 
                                                         113);
    vlSelfRef.test_blitter__DOT__cpu_addr = __Vtask_test_blitter__DOT__write_reg__146__addr;
    vlSelfRef.test_blitter__DOT__cpu_wdata = __Vtask_test_blitter__DOT__write_reg__146__data;
    vlSelfRef.test_blitter__DOT__cpu_we = 1U;
    Vtest_blitter___024root____VbeforeTrig_h603fc354__0(vlSelf, 
                                                        "@(posedge test_blitter.clk)");
    co_await vlSelfRef.__VtrigSched_h603fc354__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge test_blitter.clk)", 
                                                         "test_blitter.sv", 
                                                         115);
    vlSelfRef.test_blitter__DOT__cpu_we = 0U;
    __Vtask_test_blitter__DOT__blt_reg__147__data = 
        (0x000000ffU & (__Vtask_test_blitter__DOT__setup_copy__120__src_stride 
                        >> 8U));
    __Vtask_test_blitter__DOT__blt_reg__147__off = 0x00000010U;
    __Vtask_test_blitter__DOT__write_reg__148__data 
        = __Vtask_test_blitter__DOT__blt_reg__147__data;
    __Vtask_test_blitter__DOT__write_reg__148__addr 
        = (0x0000ffffU & ((IData)(0xba83U) + __Vtask_test_blitter__DOT__blt_reg__147__off));
    Vtest_blitter___024root____VbeforeTrig_h603fc354__0(vlSelf, 
                                                        "@(posedge test_blitter.clk)");
    co_await vlSelfRef.__VtrigSched_h603fc354__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge test_blitter.clk)", 
                                                         "test_blitter.sv", 
                                                         113);
    vlSelfRef.test_blitter__DOT__cpu_addr = __Vtask_test_blitter__DOT__write_reg__148__addr;
    vlSelfRef.test_blitter__DOT__cpu_wdata = __Vtask_test_blitter__DOT__write_reg__148__data;
    vlSelfRef.test_blitter__DOT__cpu_we = 1U;
    Vtest_blitter___024root____VbeforeTrig_h603fc354__0(vlSelf, 
                                                        "@(posedge test_blitter.clk)");
    co_await vlSelfRef.__VtrigSched_h603fc354__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge test_blitter.clk)", 
                                                         "test_blitter.sv", 
                                                         115);
    vlSelfRef.test_blitter__DOT__cpu_we = 0U;
    __Vtask_test_blitter__DOT__blt_reg__149__data = 
        (0x000000ffU & __Vtask_test_blitter__DOT__setup_copy__120__dst_stride);
    __Vtask_test_blitter__DOT__blt_reg__149__off = 0x00000011U;
    __Vtask_test_blitter__DOT__write_reg__150__data 
        = __Vtask_test_blitter__DOT__blt_reg__149__data;
    __Vtask_test_blitter__DOT__write_reg__150__addr 
        = (0x0000ffffU & ((IData)(0xba83U) + __Vtask_test_blitter__DOT__blt_reg__149__off));
    Vtest_blitter___024root____VbeforeTrig_h603fc354__0(vlSelf, 
                                                        "@(posedge test_blitter.clk)");
    co_await vlSelfRef.__VtrigSched_h603fc354__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge test_blitter.clk)", 
                                                         "test_blitter.sv", 
                                                         113);
    vlSelfRef.test_blitter__DOT__cpu_addr = __Vtask_test_blitter__DOT__write_reg__150__addr;
    vlSelfRef.test_blitter__DOT__cpu_wdata = __Vtask_test_blitter__DOT__write_reg__150__data;
    vlSelfRef.test_blitter__DOT__cpu_we = 1U;
    Vtest_blitter___024root____VbeforeTrig_h603fc354__0(vlSelf, 
                                                        "@(posedge test_blitter.clk)");
    co_await vlSelfRef.__VtrigSched_h603fc354__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge test_blitter.clk)", 
                                                         "test_blitter.sv", 
                                                         115);
    vlSelfRef.test_blitter__DOT__cpu_we = 0U;
    __Vtask_test_blitter__DOT__blt_reg__151__data = 
        (0x000000ffU & (__Vtask_test_blitter__DOT__setup_copy__120__dst_stride 
                        >> 8U));
    __Vtask_test_blitter__DOT__blt_reg__151__off = 0x00000012U;
    __Vtask_test_blitter__DOT__write_reg__152__data 
        = __Vtask_test_blitter__DOT__blt_reg__151__data;
    __Vtask_test_blitter__DOT__write_reg__152__addr 
        = (0x0000ffffU & ((IData)(0xba83U) + __Vtask_test_blitter__DOT__blt_reg__151__off));
    Vtest_blitter___024root____VbeforeTrig_h603fc354__0(vlSelf, 
                                                        "@(posedge test_blitter.clk)");
    co_await vlSelfRef.__VtrigSched_h603fc354__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge test_blitter.clk)", 
                                                         "test_blitter.sv", 
                                                         113);
    vlSelfRef.test_blitter__DOT__cpu_addr = __Vtask_test_blitter__DOT__write_reg__152__addr;
    vlSelfRef.test_blitter__DOT__cpu_wdata = __Vtask_test_blitter__DOT__write_reg__152__data;
    vlSelfRef.test_blitter__DOT__cpu_we = 1U;
    Vtest_blitter___024root____VbeforeTrig_h603fc354__0(vlSelf, 
                                                        "@(posedge test_blitter.clk)");
    co_await vlSelfRef.__VtrigSched_h603fc354__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge test_blitter.clk)", 
                                                         "test_blitter.sv", 
                                                         115);
    vlSelfRef.test_blitter__DOT__cpu_we = 0U;
    __Vtask_test_blitter__DOT__blt_reg__153__data = 0U;
    __Vtask_test_blitter__DOT__blt_reg__153__off = 0x00000013U;
    __Vtask_test_blitter__DOT__write_reg__154__data 
        = __Vtask_test_blitter__DOT__blt_reg__153__data;
    __Vtask_test_blitter__DOT__write_reg__154__addr 
        = (0x0000ffffU & ((IData)(0xba83U) + __Vtask_test_blitter__DOT__blt_reg__153__off));
    Vtest_blitter___024root____VbeforeTrig_h603fc354__0(vlSelf, 
                                                        "@(posedge test_blitter.clk)");
    co_await vlSelfRef.__VtrigSched_h603fc354__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge test_blitter.clk)", 
                                                         "test_blitter.sv", 
                                                         113);
    vlSelfRef.test_blitter__DOT__cpu_addr = __Vtask_test_blitter__DOT__write_reg__154__addr;
    vlSelfRef.test_blitter__DOT__cpu_wdata = __Vtask_test_blitter__DOT__write_reg__154__data;
    vlSelfRef.test_blitter__DOT__cpu_we = 1U;
    Vtest_blitter___024root____VbeforeTrig_h603fc354__0(vlSelf, 
                                                        "@(posedge test_blitter.clk)");
    co_await vlSelfRef.__VtrigSched_h603fc354__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge test_blitter.clk)", 
                                                         "test_blitter.sv", 
                                                         115);
    vlSelfRef.test_blitter__DOT__cpu_we = 0U;
    __Vtask_test_blitter__DOT__blt_reg__156__data = 1U;
    __Vtask_test_blitter__DOT__blt_reg__156__off = 0U;
    __Vtask_test_blitter__DOT__write_reg__157__data 
        = __Vtask_test_blitter__DOT__blt_reg__156__data;
    __Vtask_test_blitter__DOT__write_reg__157__addr 
        = (0x0000ffffU & ((IData)(0xba83U) + __Vtask_test_blitter__DOT__blt_reg__156__off));
    Vtest_blitter___024root____VbeforeTrig_h603fc354__0(vlSelf, 
                                                        "@(posedge test_blitter.clk)");
    co_await vlSelfRef.__VtrigSched_h603fc354__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge test_blitter.clk)", 
                                                         "test_blitter.sv", 
                                                         113);
    vlSelfRef.test_blitter__DOT__cpu_addr = __Vtask_test_blitter__DOT__write_reg__157__addr;
    vlSelfRef.test_blitter__DOT__cpu_wdata = __Vtask_test_blitter__DOT__write_reg__157__data;
    vlSelfRef.test_blitter__DOT__cpu_we = 1U;
    Vtest_blitter___024root____VbeforeTrig_h603fc354__0(vlSelf, 
                                                        "@(posedge test_blitter.clk)");
    co_await vlSelfRef.__VtrigSched_h603fc354__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge test_blitter.clk)", 
                                                         "test_blitter.sv", 
                                                         115);
    vlSelfRef.test_blitter__DOT__cpu_we = 0U;
    __Vtask_test_blitter__DOT__wait_blt_done__158__test_blitter__DOT__unnamedblk1_1__DOT____Vrepeat0 = 0;
    __Vtask_test_blitter__DOT__wait_blt_done__158__test_blitter__DOT__unnamedblk1_2__DOT____Vrepeat1 = 0;
    __Vtask_test_blitter__DOT__wait_blt_done__158__test_blitter__DOT__unnamedblk1_1__DOT____Vrepeat0 = 2U;
    while (VL_LTS_III(32, 0U, __Vtask_test_blitter__DOT__wait_blt_done__158__test_blitter__DOT__unnamedblk1_1__DOT____Vrepeat0)) {
        Vtest_blitter___024root____VbeforeTrig_h603fc354__0(vlSelf, 
                                                            "@(posedge test_blitter.clk)");
        co_await vlSelfRef.__VtrigSched_h603fc354__0.trigger(0U, 
                                                             nullptr, 
                                                             "@(posedge test_blitter.clk)", 
                                                             "test_blitter.sv", 
                                                             128);
        __Vtask_test_blitter__DOT__wait_blt_done__158__test_blitter__DOT__unnamedblk1_1__DOT____Vrepeat0 
            = (__Vtask_test_blitter__DOT__wait_blt_done__158__test_blitter__DOT__unnamedblk1_1__DOT____Vrepeat0 
               - (IData)(1U));
    }
    while (((0U != (IData)(vlSelfRef.test_blitter__DOT__dut__DOT__state)) 
            & (6U != (IData)(vlSelfRef.test_blitter__DOT__dut__DOT__state)))) {
        Vtest_blitter___024root____VbeforeTrig_h603fc354__0(vlSelf, 
                                                            "@(posedge test_blitter.clk)");
        co_await vlSelfRef.__VtrigSched_h603fc354__0.trigger(0U, 
                                                             nullptr, 
                                                             "@(posedge test_blitter.clk)", 
                                                             "test_blitter.sv", 
                                                             130);
    }
    __Vtask_test_blitter__DOT__wait_blt_done__158__test_blitter__DOT__unnamedblk1_2__DOT____Vrepeat1 = 2U;
    while (VL_LTS_III(32, 0U, __Vtask_test_blitter__DOT__wait_blt_done__158__test_blitter__DOT__unnamedblk1_2__DOT____Vrepeat1)) {
        Vtest_blitter___024root____VbeforeTrig_h603fc354__0(vlSelf, 
                                                            "@(posedge test_blitter.clk)");
        co_await vlSelfRef.__VtrigSched_h603fc354__0.trigger(0U, 
                                                             nullptr, 
                                                             "@(posedge test_blitter.clk)", 
                                                             "test_blitter.sv", 
                                                             131);
        __Vtask_test_blitter__DOT__wait_blt_done__158__test_blitter__DOT__unnamedblk1_2__DOT____Vrepeat1 
            = (__Vtask_test_blitter__DOT__wait_blt_done__158__test_blitter__DOT__unnamedblk1_2__DOT____Vrepeat1 
               - (IData)(1U));
    }
    __Vtask_test_blitter__DOT__check__159__condition 
        = (2U == vlSelfRef.test_blitter__DOT__dut__DOT__regs[1U]);
    vlSelfRef.__Vtask_test_blitter__DOT__check__159__name = "gfx copy ok"s;
    vlSelfRef.test_blitter__DOT__test_num = ((IData)(1U) 
                                             + vlSelfRef.test_blitter__DOT__test_num);
    if (__Vtask_test_blitter__DOT__check__159__condition) {
        VL_WRITEF_NX("  PASS [%0d] %@\n",0,32,vlSelfRef.test_blitter__DOT__test_num,
                     -1,&(vlSelfRef.__Vtask_test_blitter__DOT__check__159__name));
        vlSelfRef.test_blitter__DOT__pass_count = ((IData)(1U) 
                                                   + vlSelfRef.test_blitter__DOT__pass_count);
    } else {
        VL_WRITEF_NX("  FAIL [%0d] %@\n",0,32,vlSelfRef.test_blitter__DOT__test_num,
                     -1,&(vlSelfRef.__Vtask_test_blitter__DOT__check__159__name));
        vlSelfRef.test_blitter__DOT__fail_count = ((IData)(1U) 
                                                   + vlSelfRef.test_blitter__DOT__fail_count);
    }
    __Vtask_test_blitter__DOT__check__160__condition 
        = (1U == vlSelfRef.test_blitter__DOT__sim_gfx[512U]);
    vlSelfRef.__Vtask_test_blitter__DOT__check__160__name = "gfx[512] = 1"s;
    vlSelfRef.test_blitter__DOT__test_num = ((IData)(1U) 
                                             + vlSelfRef.test_blitter__DOT__test_num);
    if (__Vtask_test_blitter__DOT__check__160__condition) {
        VL_WRITEF_NX("  PASS [%0d] %@\n",0,32,vlSelfRef.test_blitter__DOT__test_num,
                     -1,&(vlSelfRef.__Vtask_test_blitter__DOT__check__160__name));
        vlSelfRef.test_blitter__DOT__pass_count = ((IData)(1U) 
                                                   + vlSelfRef.test_blitter__DOT__pass_count);
    } else {
        VL_WRITEF_NX("  FAIL [%0d] %@\n",0,32,vlSelfRef.test_blitter__DOT__test_num,
                     -1,&(vlSelfRef.__Vtask_test_blitter__DOT__check__160__name));
        vlSelfRef.test_blitter__DOT__fail_count = ((IData)(1U) 
                                                   + vlSelfRef.test_blitter__DOT__fail_count);
    }
    __Vtask_test_blitter__DOT__check__161__condition 
        = (2U == vlSelfRef.test_blitter__DOT__sim_gfx[513U]);
    vlSelfRef.__Vtask_test_blitter__DOT__check__161__name = "gfx[513] = 2"s;
    vlSelfRef.test_blitter__DOT__test_num = ((IData)(1U) 
                                             + vlSelfRef.test_blitter__DOT__test_num);
    if (__Vtask_test_blitter__DOT__check__161__condition) {
        VL_WRITEF_NX("  PASS [%0d] %@\n",0,32,vlSelfRef.test_blitter__DOT__test_num,
                     -1,&(vlSelfRef.__Vtask_test_blitter__DOT__check__161__name));
        vlSelfRef.test_blitter__DOT__pass_count = ((IData)(1U) 
                                                   + vlSelfRef.test_blitter__DOT__pass_count);
    } else {
        VL_WRITEF_NX("  FAIL [%0d] %@\n",0,32,vlSelfRef.test_blitter__DOT__test_num,
                     -1,&(vlSelfRef.__Vtask_test_blitter__DOT__check__161__name));
        vlSelfRef.test_blitter__DOT__fail_count = ((IData)(1U) 
                                                   + vlSelfRef.test_blitter__DOT__fail_count);
    }
    __Vtask_test_blitter__DOT__check__162__condition 
        = (0x0aU == vlSelfRef.test_blitter__DOT__sim_gfx[521U]);
    vlSelfRef.__Vtask_test_blitter__DOT__check__162__name = "gfx[521] = 10 (0xA)"s;
    vlSelfRef.test_blitter__DOT__test_num = ((IData)(1U) 
                                             + vlSelfRef.test_blitter__DOT__test_num);
    if (__Vtask_test_blitter__DOT__check__162__condition) {
        VL_WRITEF_NX("  PASS [%0d] %@\n",0,32,vlSelfRef.test_blitter__DOT__test_num,
                     -1,&(vlSelfRef.__Vtask_test_blitter__DOT__check__162__name));
        vlSelfRef.test_blitter__DOT__pass_count = ((IData)(1U) 
                                                   + vlSelfRef.test_blitter__DOT__pass_count);
    } else {
        VL_WRITEF_NX("  FAIL [%0d] %@\n",0,32,vlSelfRef.test_blitter__DOT__test_num,
                     -1,&(vlSelfRef.__Vtask_test_blitter__DOT__check__162__name));
        vlSelfRef.test_blitter__DOT__fail_count = ((IData)(1U) 
                                                   + vlSelfRef.test_blitter__DOT__fail_count);
    }
    VL_WRITEF_NX("Test: Color-key copy\n",0);
    vlSelfRef.test_blitter__DOT__sim_ram[20480U] = 1U;
    vlSelfRef.test_blitter__DOT__sim_ram[20481U] = 0U;
    vlSelfRef.test_blitter__DOT__sim_ram[20482U] = 2U;
    vlSelfRef.test_blitter__DOT__sim_ram[20483U] = 0U;
    vlSelfRef.test_blitter__DOT__sim_ram[20484U] = 3U;
    vlSelfRef.test_blitter__DOT__sim_ram[20736U] = 9U;
    vlSelfRef.test_blitter__DOT__sim_ram[20737U] = 9U;
    vlSelfRef.test_blitter__DOT__sim_ram[20738U] = 9U;
    vlSelfRef.test_blitter__DOT__sim_ram[20739U] = 9U;
    vlSelfRef.test_blitter__DOT__sim_ram[20740U] = 9U;
    __Vtask_test_blitter__DOT__setup_copy__163__height = 1U;
    __Vtask_test_blitter__DOT__setup_copy__163__width = 5U;
    __Vtask_test_blitter__DOT__setup_copy__163__dst_stride = 5U;
    __Vtask_test_blitter__DOT__setup_copy__163__dst_addr = 0x00005100U;
    __Vtask_test_blitter__DOT__setup_copy__163__dst_space = 0U;
    __Vtask_test_blitter__DOT__setup_copy__163__src_stride = 5U;
    __Vtask_test_blitter__DOT__setup_copy__163__src_addr = 0x00005000U;
    __Vtask_test_blitter__DOT__setup_copy__163__src_space = 0U;
    __Vtask_test_blitter__DOT__blt_reg__164__data = 
        (0x000000ffU & __Vtask_test_blitter__DOT__setup_copy__163__src_space);
    __Vtask_test_blitter__DOT__blt_reg__164__off = 3U;
    __Vtask_test_blitter__DOT__write_reg__165__data 
        = __Vtask_test_blitter__DOT__blt_reg__164__data;
    __Vtask_test_blitter__DOT__write_reg__165__addr 
        = (0x0000ffffU & ((IData)(0xba83U) + __Vtask_test_blitter__DOT__blt_reg__164__off));
    Vtest_blitter___024root____VbeforeTrig_h603fc354__0(vlSelf, 
                                                        "@(posedge test_blitter.clk)");
    co_await vlSelfRef.__VtrigSched_h603fc354__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge test_blitter.clk)", 
                                                         "test_blitter.sv", 
                                                         113);
    vlSelfRef.test_blitter__DOT__cpu_addr = __Vtask_test_blitter__DOT__write_reg__165__addr;
    vlSelfRef.test_blitter__DOT__cpu_wdata = __Vtask_test_blitter__DOT__write_reg__165__data;
    vlSelfRef.test_blitter__DOT__cpu_we = 1U;
    Vtest_blitter___024root____VbeforeTrig_h603fc354__0(vlSelf, 
                                                        "@(posedge test_blitter.clk)");
    co_await vlSelfRef.__VtrigSched_h603fc354__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge test_blitter.clk)", 
                                                         "test_blitter.sv", 
                                                         115);
    vlSelfRef.test_blitter__DOT__cpu_we = 0U;
    __Vtask_test_blitter__DOT__blt_reg__166__data = 
        (0x000000ffU & __Vtask_test_blitter__DOT__setup_copy__163__dst_space);
    __Vtask_test_blitter__DOT__blt_reg__166__off = 4U;
    __Vtask_test_blitter__DOT__write_reg__167__data 
        = __Vtask_test_blitter__DOT__blt_reg__166__data;
    __Vtask_test_blitter__DOT__write_reg__167__addr 
        = (0x0000ffffU & ((IData)(0xba83U) + __Vtask_test_blitter__DOT__blt_reg__166__off));
    Vtest_blitter___024root____VbeforeTrig_h603fc354__0(vlSelf, 
                                                        "@(posedge test_blitter.clk)");
    co_await vlSelfRef.__VtrigSched_h603fc354__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge test_blitter.clk)", 
                                                         "test_blitter.sv", 
                                                         113);
    vlSelfRef.test_blitter__DOT__cpu_addr = __Vtask_test_blitter__DOT__write_reg__167__addr;
    vlSelfRef.test_blitter__DOT__cpu_wdata = __Vtask_test_blitter__DOT__write_reg__167__data;
    vlSelfRef.test_blitter__DOT__cpu_we = 1U;
    Vtest_blitter___024root____VbeforeTrig_h603fc354__0(vlSelf, 
                                                        "@(posedge test_blitter.clk)");
    co_await vlSelfRef.__VtrigSched_h603fc354__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge test_blitter.clk)", 
                                                         "test_blitter.sv", 
                                                         115);
    vlSelfRef.test_blitter__DOT__cpu_we = 0U;
    __Vtask_test_blitter__DOT__blt_reg__168__data = 
        (0x000000ffU & __Vtask_test_blitter__DOT__setup_copy__163__src_addr);
    __Vtask_test_blitter__DOT__blt_reg__168__off = 5U;
    __Vtask_test_blitter__DOT__write_reg__169__data 
        = __Vtask_test_blitter__DOT__blt_reg__168__data;
    __Vtask_test_blitter__DOT__write_reg__169__addr 
        = (0x0000ffffU & ((IData)(0xba83U) + __Vtask_test_blitter__DOT__blt_reg__168__off));
    Vtest_blitter___024root____VbeforeTrig_h603fc354__0(vlSelf, 
                                                        "@(posedge test_blitter.clk)");
    co_await vlSelfRef.__VtrigSched_h603fc354__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge test_blitter.clk)", 
                                                         "test_blitter.sv", 
                                                         113);
    vlSelfRef.test_blitter__DOT__cpu_addr = __Vtask_test_blitter__DOT__write_reg__169__addr;
    vlSelfRef.test_blitter__DOT__cpu_wdata = __Vtask_test_blitter__DOT__write_reg__169__data;
    vlSelfRef.test_blitter__DOT__cpu_we = 1U;
    Vtest_blitter___024root____VbeforeTrig_h603fc354__0(vlSelf, 
                                                        "@(posedge test_blitter.clk)");
    co_await vlSelfRef.__VtrigSched_h603fc354__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge test_blitter.clk)", 
                                                         "test_blitter.sv", 
                                                         115);
    vlSelfRef.test_blitter__DOT__cpu_we = 0U;
    __Vtask_test_blitter__DOT__blt_reg__170__data = 
        (0x000000ffU & (__Vtask_test_blitter__DOT__setup_copy__163__src_addr 
                        >> 8U));
    __Vtask_test_blitter__DOT__blt_reg__170__off = 6U;
    __Vtask_test_blitter__DOT__write_reg__171__data 
        = __Vtask_test_blitter__DOT__blt_reg__170__data;
    __Vtask_test_blitter__DOT__write_reg__171__addr 
        = (0x0000ffffU & ((IData)(0xba83U) + __Vtask_test_blitter__DOT__blt_reg__170__off));
    Vtest_blitter___024root____VbeforeTrig_h603fc354__0(vlSelf, 
                                                        "@(posedge test_blitter.clk)");
    co_await vlSelfRef.__VtrigSched_h603fc354__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge test_blitter.clk)", 
                                                         "test_blitter.sv", 
                                                         113);
    vlSelfRef.test_blitter__DOT__cpu_addr = __Vtask_test_blitter__DOT__write_reg__171__addr;
    vlSelfRef.test_blitter__DOT__cpu_wdata = __Vtask_test_blitter__DOT__write_reg__171__data;
    vlSelfRef.test_blitter__DOT__cpu_we = 1U;
    Vtest_blitter___024root____VbeforeTrig_h603fc354__0(vlSelf, 
                                                        "@(posedge test_blitter.clk)");
    co_await vlSelfRef.__VtrigSched_h603fc354__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge test_blitter.clk)", 
                                                         "test_blitter.sv", 
                                                         115);
    vlSelfRef.test_blitter__DOT__cpu_we = 0U;
    __Vtask_test_blitter__DOT__blt_reg__172__data = 
        (0x000000ffU & (__Vtask_test_blitter__DOT__setup_copy__163__src_addr 
                        >> 0x10U));
    __Vtask_test_blitter__DOT__blt_reg__172__off = 7U;
    __Vtask_test_blitter__DOT__write_reg__173__data 
        = __Vtask_test_blitter__DOT__blt_reg__172__data;
    __Vtask_test_blitter__DOT__write_reg__173__addr 
        = (0x0000ffffU & ((IData)(0xba83U) + __Vtask_test_blitter__DOT__blt_reg__172__off));
    Vtest_blitter___024root____VbeforeTrig_h603fc354__0(vlSelf, 
                                                        "@(posedge test_blitter.clk)");
    co_await vlSelfRef.__VtrigSched_h603fc354__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge test_blitter.clk)", 
                                                         "test_blitter.sv", 
                                                         113);
    vlSelfRef.test_blitter__DOT__cpu_addr = __Vtask_test_blitter__DOT__write_reg__173__addr;
    vlSelfRef.test_blitter__DOT__cpu_wdata = __Vtask_test_blitter__DOT__write_reg__173__data;
    vlSelfRef.test_blitter__DOT__cpu_we = 1U;
    Vtest_blitter___024root____VbeforeTrig_h603fc354__0(vlSelf, 
                                                        "@(posedge test_blitter.clk)");
    co_await vlSelfRef.__VtrigSched_h603fc354__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge test_blitter.clk)", 
                                                         "test_blitter.sv", 
                                                         115);
    vlSelfRef.test_blitter__DOT__cpu_we = 0U;
    __Vtask_test_blitter__DOT__blt_reg__174__data = 
        (0x000000ffU & __Vtask_test_blitter__DOT__setup_copy__163__dst_addr);
    __Vtask_test_blitter__DOT__blt_reg__174__off = 8U;
    __Vtask_test_blitter__DOT__write_reg__175__data 
        = __Vtask_test_blitter__DOT__blt_reg__174__data;
    __Vtask_test_blitter__DOT__write_reg__175__addr 
        = (0x0000ffffU & ((IData)(0xba83U) + __Vtask_test_blitter__DOT__blt_reg__174__off));
    Vtest_blitter___024root____VbeforeTrig_h603fc354__0(vlSelf, 
                                                        "@(posedge test_blitter.clk)");
    co_await vlSelfRef.__VtrigSched_h603fc354__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge test_blitter.clk)", 
                                                         "test_blitter.sv", 
                                                         113);
    vlSelfRef.test_blitter__DOT__cpu_addr = __Vtask_test_blitter__DOT__write_reg__175__addr;
    vlSelfRef.test_blitter__DOT__cpu_wdata = __Vtask_test_blitter__DOT__write_reg__175__data;
    vlSelfRef.test_blitter__DOT__cpu_we = 1U;
    Vtest_blitter___024root____VbeforeTrig_h603fc354__0(vlSelf, 
                                                        "@(posedge test_blitter.clk)");
    co_await vlSelfRef.__VtrigSched_h603fc354__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge test_blitter.clk)", 
                                                         "test_blitter.sv", 
                                                         115);
    vlSelfRef.test_blitter__DOT__cpu_we = 0U;
    __Vtask_test_blitter__DOT__blt_reg__176__data = 
        (0x000000ffU & (__Vtask_test_blitter__DOT__setup_copy__163__dst_addr 
                        >> 8U));
    __Vtask_test_blitter__DOT__blt_reg__176__off = 9U;
    __Vtask_test_blitter__DOT__write_reg__177__data 
        = __Vtask_test_blitter__DOT__blt_reg__176__data;
    __Vtask_test_blitter__DOT__write_reg__177__addr 
        = (0x0000ffffU & ((IData)(0xba83U) + __Vtask_test_blitter__DOT__blt_reg__176__off));
    Vtest_blitter___024root____VbeforeTrig_h603fc354__0(vlSelf, 
                                                        "@(posedge test_blitter.clk)");
    co_await vlSelfRef.__VtrigSched_h603fc354__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge test_blitter.clk)", 
                                                         "test_blitter.sv", 
                                                         113);
    vlSelfRef.test_blitter__DOT__cpu_addr = __Vtask_test_blitter__DOT__write_reg__177__addr;
    vlSelfRef.test_blitter__DOT__cpu_wdata = __Vtask_test_blitter__DOT__write_reg__177__data;
    vlSelfRef.test_blitter__DOT__cpu_we = 1U;
    Vtest_blitter___024root____VbeforeTrig_h603fc354__0(vlSelf, 
                                                        "@(posedge test_blitter.clk)");
    co_await vlSelfRef.__VtrigSched_h603fc354__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge test_blitter.clk)", 
                                                         "test_blitter.sv", 
                                                         115);
    vlSelfRef.test_blitter__DOT__cpu_we = 0U;
    __Vtask_test_blitter__DOT__blt_reg__178__data = 
        (0x000000ffU & (__Vtask_test_blitter__DOT__setup_copy__163__dst_addr 
                        >> 0x10U));
    __Vtask_test_blitter__DOT__blt_reg__178__off = 0x0000000aU;
    __Vtask_test_blitter__DOT__write_reg__179__data 
        = __Vtask_test_blitter__DOT__blt_reg__178__data;
    __Vtask_test_blitter__DOT__write_reg__179__addr 
        = (0x0000ffffU & ((IData)(0xba83U) + __Vtask_test_blitter__DOT__blt_reg__178__off));
    Vtest_blitter___024root____VbeforeTrig_h603fc354__0(vlSelf, 
                                                        "@(posedge test_blitter.clk)");
    co_await vlSelfRef.__VtrigSched_h603fc354__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge test_blitter.clk)", 
                                                         "test_blitter.sv", 
                                                         113);
    vlSelfRef.test_blitter__DOT__cpu_addr = __Vtask_test_blitter__DOT__write_reg__179__addr;
    vlSelfRef.test_blitter__DOT__cpu_wdata = __Vtask_test_blitter__DOT__write_reg__179__data;
    vlSelfRef.test_blitter__DOT__cpu_we = 1U;
    Vtest_blitter___024root____VbeforeTrig_h603fc354__0(vlSelf, 
                                                        "@(posedge test_blitter.clk)");
    co_await vlSelfRef.__VtrigSched_h603fc354__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge test_blitter.clk)", 
                                                         "test_blitter.sv", 
                                                         115);
    vlSelfRef.test_blitter__DOT__cpu_we = 0U;
    __Vtask_test_blitter__DOT__blt_reg__180__data = 
        (0x000000ffU & __Vtask_test_blitter__DOT__setup_copy__163__width);
    __Vtask_test_blitter__DOT__blt_reg__180__off = 0x0000000bU;
    __Vtask_test_blitter__DOT__write_reg__181__data 
        = __Vtask_test_blitter__DOT__blt_reg__180__data;
    __Vtask_test_blitter__DOT__write_reg__181__addr 
        = (0x0000ffffU & ((IData)(0xba83U) + __Vtask_test_blitter__DOT__blt_reg__180__off));
    Vtest_blitter___024root____VbeforeTrig_h603fc354__0(vlSelf, 
                                                        "@(posedge test_blitter.clk)");
    co_await vlSelfRef.__VtrigSched_h603fc354__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge test_blitter.clk)", 
                                                         "test_blitter.sv", 
                                                         113);
    vlSelfRef.test_blitter__DOT__cpu_addr = __Vtask_test_blitter__DOT__write_reg__181__addr;
    vlSelfRef.test_blitter__DOT__cpu_wdata = __Vtask_test_blitter__DOT__write_reg__181__data;
    vlSelfRef.test_blitter__DOT__cpu_we = 1U;
    Vtest_blitter___024root____VbeforeTrig_h603fc354__0(vlSelf, 
                                                        "@(posedge test_blitter.clk)");
    co_await vlSelfRef.__VtrigSched_h603fc354__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge test_blitter.clk)", 
                                                         "test_blitter.sv", 
                                                         115);
    vlSelfRef.test_blitter__DOT__cpu_we = 0U;
    __Vtask_test_blitter__DOT__blt_reg__182__data = 
        (0x000000ffU & (__Vtask_test_blitter__DOT__setup_copy__163__width 
                        >> 8U));
    __Vtask_test_blitter__DOT__blt_reg__182__off = 0x0000000cU;
    __Vtask_test_blitter__DOT__write_reg__183__data 
        = __Vtask_test_blitter__DOT__blt_reg__182__data;
    __Vtask_test_blitter__DOT__write_reg__183__addr 
        = (0x0000ffffU & ((IData)(0xba83U) + __Vtask_test_blitter__DOT__blt_reg__182__off));
    Vtest_blitter___024root____VbeforeTrig_h603fc354__0(vlSelf, 
                                                        "@(posedge test_blitter.clk)");
    co_await vlSelfRef.__VtrigSched_h603fc354__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge test_blitter.clk)", 
                                                         "test_blitter.sv", 
                                                         113);
    vlSelfRef.test_blitter__DOT__cpu_addr = __Vtask_test_blitter__DOT__write_reg__183__addr;
    vlSelfRef.test_blitter__DOT__cpu_wdata = __Vtask_test_blitter__DOT__write_reg__183__data;
    vlSelfRef.test_blitter__DOT__cpu_we = 1U;
    Vtest_blitter___024root____VbeforeTrig_h603fc354__0(vlSelf, 
                                                        "@(posedge test_blitter.clk)");
    co_await vlSelfRef.__VtrigSched_h603fc354__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge test_blitter.clk)", 
                                                         "test_blitter.sv", 
                                                         115);
    vlSelfRef.test_blitter__DOT__cpu_we = 0U;
    __Vtask_test_blitter__DOT__blt_reg__184__data = 
        (0x000000ffU & __Vtask_test_blitter__DOT__setup_copy__163__height);
    __Vtask_test_blitter__DOT__blt_reg__184__off = 0x0000000dU;
    __Vtask_test_blitter__DOT__write_reg__185__data 
        = __Vtask_test_blitter__DOT__blt_reg__184__data;
    __Vtask_test_blitter__DOT__write_reg__185__addr 
        = (0x0000ffffU & ((IData)(0xba83U) + __Vtask_test_blitter__DOT__blt_reg__184__off));
    Vtest_blitter___024root____VbeforeTrig_h603fc354__0(vlSelf, 
                                                        "@(posedge test_blitter.clk)");
    co_await vlSelfRef.__VtrigSched_h603fc354__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge test_blitter.clk)", 
                                                         "test_blitter.sv", 
                                                         113);
    vlSelfRef.test_blitter__DOT__cpu_addr = __Vtask_test_blitter__DOT__write_reg__185__addr;
    vlSelfRef.test_blitter__DOT__cpu_wdata = __Vtask_test_blitter__DOT__write_reg__185__data;
    vlSelfRef.test_blitter__DOT__cpu_we = 1U;
    Vtest_blitter___024root____VbeforeTrig_h603fc354__0(vlSelf, 
                                                        "@(posedge test_blitter.clk)");
    co_await vlSelfRef.__VtrigSched_h603fc354__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge test_blitter.clk)", 
                                                         "test_blitter.sv", 
                                                         115);
    vlSelfRef.test_blitter__DOT__cpu_we = 0U;
    __Vtask_test_blitter__DOT__blt_reg__186__data = 
        (0x000000ffU & (__Vtask_test_blitter__DOT__setup_copy__163__height 
                        >> 8U));
    __Vtask_test_blitter__DOT__blt_reg__186__off = 0x0000000eU;
    __Vtask_test_blitter__DOT__write_reg__187__data 
        = __Vtask_test_blitter__DOT__blt_reg__186__data;
    __Vtask_test_blitter__DOT__write_reg__187__addr 
        = (0x0000ffffU & ((IData)(0xba83U) + __Vtask_test_blitter__DOT__blt_reg__186__off));
    Vtest_blitter___024root____VbeforeTrig_h603fc354__0(vlSelf, 
                                                        "@(posedge test_blitter.clk)");
    co_await vlSelfRef.__VtrigSched_h603fc354__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge test_blitter.clk)", 
                                                         "test_blitter.sv", 
                                                         113);
    vlSelfRef.test_blitter__DOT__cpu_addr = __Vtask_test_blitter__DOT__write_reg__187__addr;
    vlSelfRef.test_blitter__DOT__cpu_wdata = __Vtask_test_blitter__DOT__write_reg__187__data;
    vlSelfRef.test_blitter__DOT__cpu_we = 1U;
    Vtest_blitter___024root____VbeforeTrig_h603fc354__0(vlSelf, 
                                                        "@(posedge test_blitter.clk)");
    co_await vlSelfRef.__VtrigSched_h603fc354__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge test_blitter.clk)", 
                                                         "test_blitter.sv", 
                                                         115);
    vlSelfRef.test_blitter__DOT__cpu_we = 0U;
    __Vtask_test_blitter__DOT__blt_reg__188__data = 
        (0x000000ffU & __Vtask_test_blitter__DOT__setup_copy__163__src_stride);
    __Vtask_test_blitter__DOT__blt_reg__188__off = 0x0000000fU;
    __Vtask_test_blitter__DOT__write_reg__189__data 
        = __Vtask_test_blitter__DOT__blt_reg__188__data;
    __Vtask_test_blitter__DOT__write_reg__189__addr 
        = (0x0000ffffU & ((IData)(0xba83U) + __Vtask_test_blitter__DOT__blt_reg__188__off));
    Vtest_blitter___024root____VbeforeTrig_h603fc354__0(vlSelf, 
                                                        "@(posedge test_blitter.clk)");
    co_await vlSelfRef.__VtrigSched_h603fc354__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge test_blitter.clk)", 
                                                         "test_blitter.sv", 
                                                         113);
    vlSelfRef.test_blitter__DOT__cpu_addr = __Vtask_test_blitter__DOT__write_reg__189__addr;
    vlSelfRef.test_blitter__DOT__cpu_wdata = __Vtask_test_blitter__DOT__write_reg__189__data;
    vlSelfRef.test_blitter__DOT__cpu_we = 1U;
    Vtest_blitter___024root____VbeforeTrig_h603fc354__0(vlSelf, 
                                                        "@(posedge test_blitter.clk)");
    co_await vlSelfRef.__VtrigSched_h603fc354__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge test_blitter.clk)", 
                                                         "test_blitter.sv", 
                                                         115);
    vlSelfRef.test_blitter__DOT__cpu_we = 0U;
    __Vtask_test_blitter__DOT__blt_reg__190__data = 
        (0x000000ffU & (__Vtask_test_blitter__DOT__setup_copy__163__src_stride 
                        >> 8U));
    __Vtask_test_blitter__DOT__blt_reg__190__off = 0x00000010U;
    __Vtask_test_blitter__DOT__write_reg__191__data 
        = __Vtask_test_blitter__DOT__blt_reg__190__data;
    __Vtask_test_blitter__DOT__write_reg__191__addr 
        = (0x0000ffffU & ((IData)(0xba83U) + __Vtask_test_blitter__DOT__blt_reg__190__off));
    Vtest_blitter___024root____VbeforeTrig_h603fc354__0(vlSelf, 
                                                        "@(posedge test_blitter.clk)");
    co_await vlSelfRef.__VtrigSched_h603fc354__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge test_blitter.clk)", 
                                                         "test_blitter.sv", 
                                                         113);
    vlSelfRef.test_blitter__DOT__cpu_addr = __Vtask_test_blitter__DOT__write_reg__191__addr;
    vlSelfRef.test_blitter__DOT__cpu_wdata = __Vtask_test_blitter__DOT__write_reg__191__data;
    vlSelfRef.test_blitter__DOT__cpu_we = 1U;
    Vtest_blitter___024root____VbeforeTrig_h603fc354__0(vlSelf, 
                                                        "@(posedge test_blitter.clk)");
    co_await vlSelfRef.__VtrigSched_h603fc354__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge test_blitter.clk)", 
                                                         "test_blitter.sv", 
                                                         115);
    vlSelfRef.test_blitter__DOT__cpu_we = 0U;
    __Vtask_test_blitter__DOT__blt_reg__192__data = 
        (0x000000ffU & __Vtask_test_blitter__DOT__setup_copy__163__dst_stride);
    __Vtask_test_blitter__DOT__blt_reg__192__off = 0x00000011U;
    __Vtask_test_blitter__DOT__write_reg__193__data 
        = __Vtask_test_blitter__DOT__blt_reg__192__data;
    __Vtask_test_blitter__DOT__write_reg__193__addr 
        = (0x0000ffffU & ((IData)(0xba83U) + __Vtask_test_blitter__DOT__blt_reg__192__off));
    Vtest_blitter___024root____VbeforeTrig_h603fc354__0(vlSelf, 
                                                        "@(posedge test_blitter.clk)");
    co_await vlSelfRef.__VtrigSched_h603fc354__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge test_blitter.clk)", 
                                                         "test_blitter.sv", 
                                                         113);
    vlSelfRef.test_blitter__DOT__cpu_addr = __Vtask_test_blitter__DOT__write_reg__193__addr;
    vlSelfRef.test_blitter__DOT__cpu_wdata = __Vtask_test_blitter__DOT__write_reg__193__data;
    vlSelfRef.test_blitter__DOT__cpu_we = 1U;
    Vtest_blitter___024root____VbeforeTrig_h603fc354__0(vlSelf, 
                                                        "@(posedge test_blitter.clk)");
    co_await vlSelfRef.__VtrigSched_h603fc354__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge test_blitter.clk)", 
                                                         "test_blitter.sv", 
                                                         115);
    vlSelfRef.test_blitter__DOT__cpu_we = 0U;
    __Vtask_test_blitter__DOT__blt_reg__194__data = 
        (0x000000ffU & (__Vtask_test_blitter__DOT__setup_copy__163__dst_stride 
                        >> 8U));
    __Vtask_test_blitter__DOT__blt_reg__194__off = 0x00000012U;
    __Vtask_test_blitter__DOT__write_reg__195__data 
        = __Vtask_test_blitter__DOT__blt_reg__194__data;
    __Vtask_test_blitter__DOT__write_reg__195__addr 
        = (0x0000ffffU & ((IData)(0xba83U) + __Vtask_test_blitter__DOT__blt_reg__194__off));
    Vtest_blitter___024root____VbeforeTrig_h603fc354__0(vlSelf, 
                                                        "@(posedge test_blitter.clk)");
    co_await vlSelfRef.__VtrigSched_h603fc354__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge test_blitter.clk)", 
                                                         "test_blitter.sv", 
                                                         113);
    vlSelfRef.test_blitter__DOT__cpu_addr = __Vtask_test_blitter__DOT__write_reg__195__addr;
    vlSelfRef.test_blitter__DOT__cpu_wdata = __Vtask_test_blitter__DOT__write_reg__195__data;
    vlSelfRef.test_blitter__DOT__cpu_we = 1U;
    Vtest_blitter___024root____VbeforeTrig_h603fc354__0(vlSelf, 
                                                        "@(posedge test_blitter.clk)");
    co_await vlSelfRef.__VtrigSched_h603fc354__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge test_blitter.clk)", 
                                                         "test_blitter.sv", 
                                                         115);
    vlSelfRef.test_blitter__DOT__cpu_we = 0U;
    __Vtask_test_blitter__DOT__blt_reg__196__data = 0U;
    __Vtask_test_blitter__DOT__blt_reg__196__off = 0x00000013U;
    __Vtask_test_blitter__DOT__write_reg__197__data 
        = __Vtask_test_blitter__DOT__blt_reg__196__data;
    __Vtask_test_blitter__DOT__write_reg__197__addr 
        = (0x0000ffffU & ((IData)(0xba83U) + __Vtask_test_blitter__DOT__blt_reg__196__off));
    Vtest_blitter___024root____VbeforeTrig_h603fc354__0(vlSelf, 
                                                        "@(posedge test_blitter.clk)");
    co_await vlSelfRef.__VtrigSched_h603fc354__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge test_blitter.clk)", 
                                                         "test_blitter.sv", 
                                                         113);
    vlSelfRef.test_blitter__DOT__cpu_addr = __Vtask_test_blitter__DOT__write_reg__197__addr;
    vlSelfRef.test_blitter__DOT__cpu_wdata = __Vtask_test_blitter__DOT__write_reg__197__data;
    vlSelfRef.test_blitter__DOT__cpu_we = 1U;
    Vtest_blitter___024root____VbeforeTrig_h603fc354__0(vlSelf, 
                                                        "@(posedge test_blitter.clk)");
    co_await vlSelfRef.__VtrigSched_h603fc354__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge test_blitter.clk)", 
                                                         "test_blitter.sv", 
                                                         115);
    vlSelfRef.test_blitter__DOT__cpu_we = 0U;
    __Vtask_test_blitter__DOT__blt_reg__198__data = 2U;
    __Vtask_test_blitter__DOT__blt_reg__198__off = 0x00000013U;
    __Vtask_test_blitter__DOT__write_reg__199__data 
        = __Vtask_test_blitter__DOT__blt_reg__198__data;
    __Vtask_test_blitter__DOT__write_reg__199__addr 
        = (0x0000ffffU & ((IData)(0xba83U) + __Vtask_test_blitter__DOT__blt_reg__198__off));
    Vtest_blitter___024root____VbeforeTrig_h603fc354__0(vlSelf, 
                                                        "@(posedge test_blitter.clk)");
    co_await vlSelfRef.__VtrigSched_h603fc354__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge test_blitter.clk)", 
                                                         "test_blitter.sv", 
                                                         113);
    vlSelfRef.test_blitter__DOT__cpu_addr = __Vtask_test_blitter__DOT__write_reg__199__addr;
    vlSelfRef.test_blitter__DOT__cpu_wdata = __Vtask_test_blitter__DOT__write_reg__199__data;
    vlSelfRef.test_blitter__DOT__cpu_we = 1U;
    Vtest_blitter___024root____VbeforeTrig_h603fc354__0(vlSelf, 
                                                        "@(posedge test_blitter.clk)");
    co_await vlSelfRef.__VtrigSched_h603fc354__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge test_blitter.clk)", 
                                                         "test_blitter.sv", 
                                                         115);
    vlSelfRef.test_blitter__DOT__cpu_we = 0U;
    __Vtask_test_blitter__DOT__blt_reg__200__data = 0U;
    __Vtask_test_blitter__DOT__blt_reg__200__off = 0x00000015U;
    __Vtask_test_blitter__DOT__write_reg__201__data 
        = __Vtask_test_blitter__DOT__blt_reg__200__data;
    __Vtask_test_blitter__DOT__write_reg__201__addr 
        = (0x0000ffffU & ((IData)(0xba83U) + __Vtask_test_blitter__DOT__blt_reg__200__off));
    Vtest_blitter___024root____VbeforeTrig_h603fc354__0(vlSelf, 
                                                        "@(posedge test_blitter.clk)");
    co_await vlSelfRef.__VtrigSched_h603fc354__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge test_blitter.clk)", 
                                                         "test_blitter.sv", 
                                                         113);
    vlSelfRef.test_blitter__DOT__cpu_addr = __Vtask_test_blitter__DOT__write_reg__201__addr;
    vlSelfRef.test_blitter__DOT__cpu_wdata = __Vtask_test_blitter__DOT__write_reg__201__data;
    vlSelfRef.test_blitter__DOT__cpu_we = 1U;
    Vtest_blitter___024root____VbeforeTrig_h603fc354__0(vlSelf, 
                                                        "@(posedge test_blitter.clk)");
    co_await vlSelfRef.__VtrigSched_h603fc354__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge test_blitter.clk)", 
                                                         "test_blitter.sv", 
                                                         115);
    vlSelfRef.test_blitter__DOT__cpu_we = 0U;
    __Vtask_test_blitter__DOT__blt_reg__203__data = 1U;
    __Vtask_test_blitter__DOT__blt_reg__203__off = 0U;
    __Vtask_test_blitter__DOT__write_reg__204__data 
        = __Vtask_test_blitter__DOT__blt_reg__203__data;
    __Vtask_test_blitter__DOT__write_reg__204__addr 
        = (0x0000ffffU & ((IData)(0xba83U) + __Vtask_test_blitter__DOT__blt_reg__203__off));
    Vtest_blitter___024root____VbeforeTrig_h603fc354__0(vlSelf, 
                                                        "@(posedge test_blitter.clk)");
    co_await vlSelfRef.__VtrigSched_h603fc354__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge test_blitter.clk)", 
                                                         "test_blitter.sv", 
                                                         113);
    vlSelfRef.test_blitter__DOT__cpu_addr = __Vtask_test_blitter__DOT__write_reg__204__addr;
    vlSelfRef.test_blitter__DOT__cpu_wdata = __Vtask_test_blitter__DOT__write_reg__204__data;
    vlSelfRef.test_blitter__DOT__cpu_we = 1U;
    Vtest_blitter___024root____VbeforeTrig_h603fc354__0(vlSelf, 
                                                        "@(posedge test_blitter.clk)");
    co_await vlSelfRef.__VtrigSched_h603fc354__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge test_blitter.clk)", 
                                                         "test_blitter.sv", 
                                                         115);
    vlSelfRef.test_blitter__DOT__cpu_we = 0U;
    __Vtask_test_blitter__DOT__wait_blt_done__205__test_blitter__DOT__unnamedblk1_1__DOT____Vrepeat0 = 0;
    __Vtask_test_blitter__DOT__wait_blt_done__205__test_blitter__DOT__unnamedblk1_2__DOT____Vrepeat1 = 0;
    __Vtask_test_blitter__DOT__wait_blt_done__205__test_blitter__DOT__unnamedblk1_1__DOT____Vrepeat0 = 2U;
    while (VL_LTS_III(32, 0U, __Vtask_test_blitter__DOT__wait_blt_done__205__test_blitter__DOT__unnamedblk1_1__DOT____Vrepeat0)) {
        Vtest_blitter___024root____VbeforeTrig_h603fc354__0(vlSelf, 
                                                            "@(posedge test_blitter.clk)");
        co_await vlSelfRef.__VtrigSched_h603fc354__0.trigger(0U, 
                                                             nullptr, 
                                                             "@(posedge test_blitter.clk)", 
                                                             "test_blitter.sv", 
                                                             128);
        __Vtask_test_blitter__DOT__wait_blt_done__205__test_blitter__DOT__unnamedblk1_1__DOT____Vrepeat0 
            = (__Vtask_test_blitter__DOT__wait_blt_done__205__test_blitter__DOT__unnamedblk1_1__DOT____Vrepeat0 
               - (IData)(1U));
    }
    while (((0U != (IData)(vlSelfRef.test_blitter__DOT__dut__DOT__state)) 
            & (6U != (IData)(vlSelfRef.test_blitter__DOT__dut__DOT__state)))) {
        Vtest_blitter___024root____VbeforeTrig_h603fc354__0(vlSelf, 
                                                            "@(posedge test_blitter.clk)");
        co_await vlSelfRef.__VtrigSched_h603fc354__0.trigger(0U, 
                                                             nullptr, 
                                                             "@(posedge test_blitter.clk)", 
                                                             "test_blitter.sv", 
                                                             130);
    }
    __Vtask_test_blitter__DOT__wait_blt_done__205__test_blitter__DOT__unnamedblk1_2__DOT____Vrepeat1 = 2U;
    while (VL_LTS_III(32, 0U, __Vtask_test_blitter__DOT__wait_blt_done__205__test_blitter__DOT__unnamedblk1_2__DOT____Vrepeat1)) {
        Vtest_blitter___024root____VbeforeTrig_h603fc354__0(vlSelf, 
                                                            "@(posedge test_blitter.clk)");
        co_await vlSelfRef.__VtrigSched_h603fc354__0.trigger(0U, 
                                                             nullptr, 
                                                             "@(posedge test_blitter.clk)", 
                                                             "test_blitter.sv", 
                                                             131);
        __Vtask_test_blitter__DOT__wait_blt_done__205__test_blitter__DOT__unnamedblk1_2__DOT____Vrepeat1 
            = (__Vtask_test_blitter__DOT__wait_blt_done__205__test_blitter__DOT__unnamedblk1_2__DOT____Vrepeat1 
               - (IData)(1U));
    }
    __Vtask_test_blitter__DOT__check__206__condition 
        = (2U == vlSelfRef.test_blitter__DOT__dut__DOT__regs[1U]);
    vlSelfRef.__Vtask_test_blitter__DOT__check__206__name = "colorkey status ok"s;
    vlSelfRef.test_blitter__DOT__test_num = ((IData)(1U) 
                                             + vlSelfRef.test_blitter__DOT__test_num);
    if (__Vtask_test_blitter__DOT__check__206__condition) {
        VL_WRITEF_NX("  PASS [%0d] %@\n",0,32,vlSelfRef.test_blitter__DOT__test_num,
                     -1,&(vlSelfRef.__Vtask_test_blitter__DOT__check__206__name));
        vlSelfRef.test_blitter__DOT__pass_count = ((IData)(1U) 
                                                   + vlSelfRef.test_blitter__DOT__pass_count);
    } else {
        VL_WRITEF_NX("  FAIL [%0d] %@\n",0,32,vlSelfRef.test_blitter__DOT__test_num,
                     -1,&(vlSelfRef.__Vtask_test_blitter__DOT__check__206__name));
        vlSelfRef.test_blitter__DOT__fail_count = ((IData)(1U) 
                                                   + vlSelfRef.test_blitter__DOT__fail_count);
    }
    __Vtask_test_blitter__DOT__check__207__condition 
        = (3U == ((vlSelfRef.test_blitter__DOT__dut__DOT__regs[24U] 
                   << 0x00000010U) | ((vlSelfRef.test_blitter__DOT__dut__DOT__regs[23U] 
                                       << 8U) | vlSelfRef.test_blitter__DOT__dut__DOT__regs[22U])));
    vlSelfRef.__Vtask_test_blitter__DOT__check__207__name = "count = 3 (skipped 2 zeros)"s;
    vlSelfRef.test_blitter__DOT__test_num = ((IData)(1U) 
                                             + vlSelfRef.test_blitter__DOT__test_num);
    if (__Vtask_test_blitter__DOT__check__207__condition) {
        VL_WRITEF_NX("  PASS [%0d] %@\n",0,32,vlSelfRef.test_blitter__DOT__test_num,
                     -1,&(vlSelfRef.__Vtask_test_blitter__DOT__check__207__name));
        vlSelfRef.test_blitter__DOT__pass_count = ((IData)(1U) 
                                                   + vlSelfRef.test_blitter__DOT__pass_count);
    } else {
        VL_WRITEF_NX("  FAIL [%0d] %@\n",0,32,vlSelfRef.test_blitter__DOT__test_num,
                     -1,&(vlSelfRef.__Vtask_test_blitter__DOT__check__207__name));
        vlSelfRef.test_blitter__DOT__fail_count = ((IData)(1U) 
                                                   + vlSelfRef.test_blitter__DOT__fail_count);
    }
    __Vtask_test_blitter__DOT__check__208__condition 
        = (1U == vlSelfRef.test_blitter__DOT__sim_ram[20736U]);
    vlSelfRef.__Vtask_test_blitter__DOT__check__208__name = "dst[0] = 1 (copied)"s;
    vlSelfRef.test_blitter__DOT__test_num = ((IData)(1U) 
                                             + vlSelfRef.test_blitter__DOT__test_num);
    if (__Vtask_test_blitter__DOT__check__208__condition) {
        VL_WRITEF_NX("  PASS [%0d] %@\n",0,32,vlSelfRef.test_blitter__DOT__test_num,
                     -1,&(vlSelfRef.__Vtask_test_blitter__DOT__check__208__name));
        vlSelfRef.test_blitter__DOT__pass_count = ((IData)(1U) 
                                                   + vlSelfRef.test_blitter__DOT__pass_count);
    } else {
        VL_WRITEF_NX("  FAIL [%0d] %@\n",0,32,vlSelfRef.test_blitter__DOT__test_num,
                     -1,&(vlSelfRef.__Vtask_test_blitter__DOT__check__208__name));
        vlSelfRef.test_blitter__DOT__fail_count = ((IData)(1U) 
                                                   + vlSelfRef.test_blitter__DOT__fail_count);
    }
    __Vtask_test_blitter__DOT__check__209__condition 
        = (9U == vlSelfRef.test_blitter__DOT__sim_ram[20737U]);
    vlSelfRef.__Vtask_test_blitter__DOT__check__209__name = "dst[1] = 9 (skipped)"s;
    vlSelfRef.test_blitter__DOT__test_num = ((IData)(1U) 
                                             + vlSelfRef.test_blitter__DOT__test_num);
    if (__Vtask_test_blitter__DOT__check__209__condition) {
        VL_WRITEF_NX("  PASS [%0d] %@\n",0,32,vlSelfRef.test_blitter__DOT__test_num,
                     -1,&(vlSelfRef.__Vtask_test_blitter__DOT__check__209__name));
        vlSelfRef.test_blitter__DOT__pass_count = ((IData)(1U) 
                                                   + vlSelfRef.test_blitter__DOT__pass_count);
    } else {
        VL_WRITEF_NX("  FAIL [%0d] %@\n",0,32,vlSelfRef.test_blitter__DOT__test_num,
                     -1,&(vlSelfRef.__Vtask_test_blitter__DOT__check__209__name));
        vlSelfRef.test_blitter__DOT__fail_count = ((IData)(1U) 
                                                   + vlSelfRef.test_blitter__DOT__fail_count);
    }
    __Vtask_test_blitter__DOT__check__210__condition 
        = (2U == vlSelfRef.test_blitter__DOT__sim_ram[20738U]);
    vlSelfRef.__Vtask_test_blitter__DOT__check__210__name = "dst[2] = 2 (copied)"s;
    vlSelfRef.test_blitter__DOT__test_num = ((IData)(1U) 
                                             + vlSelfRef.test_blitter__DOT__test_num);
    if (__Vtask_test_blitter__DOT__check__210__condition) {
        VL_WRITEF_NX("  PASS [%0d] %@\n",0,32,vlSelfRef.test_blitter__DOT__test_num,
                     -1,&(vlSelfRef.__Vtask_test_blitter__DOT__check__210__name));
        vlSelfRef.test_blitter__DOT__pass_count = ((IData)(1U) 
                                                   + vlSelfRef.test_blitter__DOT__pass_count);
    } else {
        VL_WRITEF_NX("  FAIL [%0d] %@\n",0,32,vlSelfRef.test_blitter__DOT__test_num,
                     -1,&(vlSelfRef.__Vtask_test_blitter__DOT__check__210__name));
        vlSelfRef.test_blitter__DOT__fail_count = ((IData)(1U) 
                                                   + vlSelfRef.test_blitter__DOT__fail_count);
    }
    __Vtask_test_blitter__DOT__check__211__condition 
        = (9U == vlSelfRef.test_blitter__DOT__sim_ram[20739U]);
    vlSelfRef.__Vtask_test_blitter__DOT__check__211__name = "dst[3] = 9 (skipped)"s;
    vlSelfRef.test_blitter__DOT__test_num = ((IData)(1U) 
                                             + vlSelfRef.test_blitter__DOT__test_num);
    if (__Vtask_test_blitter__DOT__check__211__condition) {
        VL_WRITEF_NX("  PASS [%0d] %@\n",0,32,vlSelfRef.test_blitter__DOT__test_num,
                     -1,&(vlSelfRef.__Vtask_test_blitter__DOT__check__211__name));
        vlSelfRef.test_blitter__DOT__pass_count = ((IData)(1U) 
                                                   + vlSelfRef.test_blitter__DOT__pass_count);
    } else {
        VL_WRITEF_NX("  FAIL [%0d] %@\n",0,32,vlSelfRef.test_blitter__DOT__test_num,
                     -1,&(vlSelfRef.__Vtask_test_blitter__DOT__check__211__name));
        vlSelfRef.test_blitter__DOT__fail_count = ((IData)(1U) 
                                                   + vlSelfRef.test_blitter__DOT__fail_count);
    }
    __Vtask_test_blitter__DOT__check__212__condition 
        = (3U == vlSelfRef.test_blitter__DOT__sim_ram[20740U]);
    vlSelfRef.__Vtask_test_blitter__DOT__check__212__name = "dst[4] = 3 (copied)"s;
    vlSelfRef.test_blitter__DOT__test_num = ((IData)(1U) 
                                             + vlSelfRef.test_blitter__DOT__test_num);
    if (__Vtask_test_blitter__DOT__check__212__condition) {
        VL_WRITEF_NX("  PASS [%0d] %@\n",0,32,vlSelfRef.test_blitter__DOT__test_num,
                     -1,&(vlSelfRef.__Vtask_test_blitter__DOT__check__212__name));
        vlSelfRef.test_blitter__DOT__pass_count = ((IData)(1U) 
                                                   + vlSelfRef.test_blitter__DOT__pass_count);
    } else {
        VL_WRITEF_NX("  FAIL [%0d] %@\n",0,32,vlSelfRef.test_blitter__DOT__test_num,
                     -1,&(vlSelfRef.__Vtask_test_blitter__DOT__check__212__name));
        vlSelfRef.test_blitter__DOT__fail_count = ((IData)(1U) 
                                                   + vlSelfRef.test_blitter__DOT__fail_count);
    }
    VL_WRITEF_NX("Test: RDY stall\n",0);
    __Vtask_test_blitter__DOT__setup_fill__213__fill_val = 0x00000055U;
    __Vtask_test_blitter__DOT__setup_fill__213__height = 1U;
    __Vtask_test_blitter__DOT__setup_fill__213__width = 0x00000064U;
    __Vtask_test_blitter__DOT__setup_fill__213__dst_stride = 0x00000064U;
    __Vtask_test_blitter__DOT__setup_fill__213__dst_addr = 0x00006000U;
    __Vtask_test_blitter__DOT__setup_fill__213__dst_space = 0U;
    __Vtask_test_blitter__DOT__blt_reg__214__data = 
        (0x000000ffU & __Vtask_test_blitter__DOT__setup_fill__213__dst_space);
    __Vtask_test_blitter__DOT__blt_reg__214__off = 4U;
    __Vtask_test_blitter__DOT__write_reg__215__data 
        = __Vtask_test_blitter__DOT__blt_reg__214__data;
    __Vtask_test_blitter__DOT__write_reg__215__addr 
        = (0x0000ffffU & ((IData)(0xba83U) + __Vtask_test_blitter__DOT__blt_reg__214__off));
    Vtest_blitter___024root____VbeforeTrig_h603fc354__0(vlSelf, 
                                                        "@(posedge test_blitter.clk)");
    co_await vlSelfRef.__VtrigSched_h603fc354__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge test_blitter.clk)", 
                                                         "test_blitter.sv", 
                                                         113);
    vlSelfRef.test_blitter__DOT__cpu_addr = __Vtask_test_blitter__DOT__write_reg__215__addr;
    vlSelfRef.test_blitter__DOT__cpu_wdata = __Vtask_test_blitter__DOT__write_reg__215__data;
    vlSelfRef.test_blitter__DOT__cpu_we = 1U;
    Vtest_blitter___024root____VbeforeTrig_h603fc354__0(vlSelf, 
                                                        "@(posedge test_blitter.clk)");
    co_await vlSelfRef.__VtrigSched_h603fc354__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge test_blitter.clk)", 
                                                         "test_blitter.sv", 
                                                         115);
    vlSelfRef.test_blitter__DOT__cpu_we = 0U;
    __Vtask_test_blitter__DOT__blt_reg__216__data = 
        (0x000000ffU & __Vtask_test_blitter__DOT__setup_fill__213__dst_addr);
    __Vtask_test_blitter__DOT__blt_reg__216__off = 8U;
    __Vtask_test_blitter__DOT__write_reg__217__data 
        = __Vtask_test_blitter__DOT__blt_reg__216__data;
    __Vtask_test_blitter__DOT__write_reg__217__addr 
        = (0x0000ffffU & ((IData)(0xba83U) + __Vtask_test_blitter__DOT__blt_reg__216__off));
    Vtest_blitter___024root____VbeforeTrig_h603fc354__0(vlSelf, 
                                                        "@(posedge test_blitter.clk)");
    co_await vlSelfRef.__VtrigSched_h603fc354__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge test_blitter.clk)", 
                                                         "test_blitter.sv", 
                                                         113);
    vlSelfRef.test_blitter__DOT__cpu_addr = __Vtask_test_blitter__DOT__write_reg__217__addr;
    vlSelfRef.test_blitter__DOT__cpu_wdata = __Vtask_test_blitter__DOT__write_reg__217__data;
    vlSelfRef.test_blitter__DOT__cpu_we = 1U;
    Vtest_blitter___024root____VbeforeTrig_h603fc354__0(vlSelf, 
                                                        "@(posedge test_blitter.clk)");
    co_await vlSelfRef.__VtrigSched_h603fc354__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge test_blitter.clk)", 
                                                         "test_blitter.sv", 
                                                         115);
    vlSelfRef.test_blitter__DOT__cpu_we = 0U;
    __Vtask_test_blitter__DOT__blt_reg__218__data = 
        (0x000000ffU & (__Vtask_test_blitter__DOT__setup_fill__213__dst_addr 
                        >> 8U));
    __Vtask_test_blitter__DOT__blt_reg__218__off = 9U;
    __Vtask_test_blitter__DOT__write_reg__219__data 
        = __Vtask_test_blitter__DOT__blt_reg__218__data;
    __Vtask_test_blitter__DOT__write_reg__219__addr 
        = (0x0000ffffU & ((IData)(0xba83U) + __Vtask_test_blitter__DOT__blt_reg__218__off));
    Vtest_blitter___024root____VbeforeTrig_h603fc354__0(vlSelf, 
                                                        "@(posedge test_blitter.clk)");
    co_await vlSelfRef.__VtrigSched_h603fc354__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge test_blitter.clk)", 
                                                         "test_blitter.sv", 
                                                         113);
    vlSelfRef.test_blitter__DOT__cpu_addr = __Vtask_test_blitter__DOT__write_reg__219__addr;
    vlSelfRef.test_blitter__DOT__cpu_wdata = __Vtask_test_blitter__DOT__write_reg__219__data;
    vlSelfRef.test_blitter__DOT__cpu_we = 1U;
    Vtest_blitter___024root____VbeforeTrig_h603fc354__0(vlSelf, 
                                                        "@(posedge test_blitter.clk)");
    co_await vlSelfRef.__VtrigSched_h603fc354__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge test_blitter.clk)", 
                                                         "test_blitter.sv", 
                                                         115);
    vlSelfRef.test_blitter__DOT__cpu_we = 0U;
    __Vtask_test_blitter__DOT__blt_reg__220__data = 
        (0x000000ffU & (__Vtask_test_blitter__DOT__setup_fill__213__dst_addr 
                        >> 0x10U));
    __Vtask_test_blitter__DOT__blt_reg__220__off = 0x0000000aU;
    __Vtask_test_blitter__DOT__write_reg__221__data 
        = __Vtask_test_blitter__DOT__blt_reg__220__data;
    __Vtask_test_blitter__DOT__write_reg__221__addr 
        = (0x0000ffffU & ((IData)(0xba83U) + __Vtask_test_blitter__DOT__blt_reg__220__off));
    Vtest_blitter___024root____VbeforeTrig_h603fc354__0(vlSelf, 
                                                        "@(posedge test_blitter.clk)");
    co_await vlSelfRef.__VtrigSched_h603fc354__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge test_blitter.clk)", 
                                                         "test_blitter.sv", 
                                                         113);
    vlSelfRef.test_blitter__DOT__cpu_addr = __Vtask_test_blitter__DOT__write_reg__221__addr;
    vlSelfRef.test_blitter__DOT__cpu_wdata = __Vtask_test_blitter__DOT__write_reg__221__data;
    vlSelfRef.test_blitter__DOT__cpu_we = 1U;
    Vtest_blitter___024root____VbeforeTrig_h603fc354__0(vlSelf, 
                                                        "@(posedge test_blitter.clk)");
    co_await vlSelfRef.__VtrigSched_h603fc354__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge test_blitter.clk)", 
                                                         "test_blitter.sv", 
                                                         115);
    vlSelfRef.test_blitter__DOT__cpu_we = 0U;
    __Vtask_test_blitter__DOT__blt_reg__222__data = 
        (0x000000ffU & __Vtask_test_blitter__DOT__setup_fill__213__width);
    __Vtask_test_blitter__DOT__blt_reg__222__off = 0x0000000bU;
    __Vtask_test_blitter__DOT__write_reg__223__data 
        = __Vtask_test_blitter__DOT__blt_reg__222__data;
    __Vtask_test_blitter__DOT__write_reg__223__addr 
        = (0x0000ffffU & ((IData)(0xba83U) + __Vtask_test_blitter__DOT__blt_reg__222__off));
    Vtest_blitter___024root____VbeforeTrig_h603fc354__0(vlSelf, 
                                                        "@(posedge test_blitter.clk)");
    co_await vlSelfRef.__VtrigSched_h603fc354__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge test_blitter.clk)", 
                                                         "test_blitter.sv", 
                                                         113);
    vlSelfRef.test_blitter__DOT__cpu_addr = __Vtask_test_blitter__DOT__write_reg__223__addr;
    vlSelfRef.test_blitter__DOT__cpu_wdata = __Vtask_test_blitter__DOT__write_reg__223__data;
    vlSelfRef.test_blitter__DOT__cpu_we = 1U;
    Vtest_blitter___024root____VbeforeTrig_h603fc354__0(vlSelf, 
                                                        "@(posedge test_blitter.clk)");
    co_await vlSelfRef.__VtrigSched_h603fc354__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge test_blitter.clk)", 
                                                         "test_blitter.sv", 
                                                         115);
    vlSelfRef.test_blitter__DOT__cpu_we = 0U;
    __Vtask_test_blitter__DOT__blt_reg__224__data = 
        (0x000000ffU & (__Vtask_test_blitter__DOT__setup_fill__213__width 
                        >> 8U));
    __Vtask_test_blitter__DOT__blt_reg__224__off = 0x0000000cU;
    __Vtask_test_blitter__DOT__write_reg__225__data 
        = __Vtask_test_blitter__DOT__blt_reg__224__data;
    __Vtask_test_blitter__DOT__write_reg__225__addr 
        = (0x0000ffffU & ((IData)(0xba83U) + __Vtask_test_blitter__DOT__blt_reg__224__off));
    Vtest_blitter___024root____VbeforeTrig_h603fc354__0(vlSelf, 
                                                        "@(posedge test_blitter.clk)");
    co_await vlSelfRef.__VtrigSched_h603fc354__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge test_blitter.clk)", 
                                                         "test_blitter.sv", 
                                                         113);
    vlSelfRef.test_blitter__DOT__cpu_addr = __Vtask_test_blitter__DOT__write_reg__225__addr;
    vlSelfRef.test_blitter__DOT__cpu_wdata = __Vtask_test_blitter__DOT__write_reg__225__data;
    vlSelfRef.test_blitter__DOT__cpu_we = 1U;
    Vtest_blitter___024root____VbeforeTrig_h603fc354__0(vlSelf, 
                                                        "@(posedge test_blitter.clk)");
    co_await vlSelfRef.__VtrigSched_h603fc354__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge test_blitter.clk)", 
                                                         "test_blitter.sv", 
                                                         115);
    vlSelfRef.test_blitter__DOT__cpu_we = 0U;
    __Vtask_test_blitter__DOT__blt_reg__226__data = 
        (0x000000ffU & __Vtask_test_blitter__DOT__setup_fill__213__height);
    __Vtask_test_blitter__DOT__blt_reg__226__off = 0x0000000dU;
    __Vtask_test_blitter__DOT__write_reg__227__data 
        = __Vtask_test_blitter__DOT__blt_reg__226__data;
    __Vtask_test_blitter__DOT__write_reg__227__addr 
        = (0x0000ffffU & ((IData)(0xba83U) + __Vtask_test_blitter__DOT__blt_reg__226__off));
    Vtest_blitter___024root____VbeforeTrig_h603fc354__0(vlSelf, 
                                                        "@(posedge test_blitter.clk)");
    co_await vlSelfRef.__VtrigSched_h603fc354__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge test_blitter.clk)", 
                                                         "test_blitter.sv", 
                                                         113);
    vlSelfRef.test_blitter__DOT__cpu_addr = __Vtask_test_blitter__DOT__write_reg__227__addr;
    vlSelfRef.test_blitter__DOT__cpu_wdata = __Vtask_test_blitter__DOT__write_reg__227__data;
    vlSelfRef.test_blitter__DOT__cpu_we = 1U;
    Vtest_blitter___024root____VbeforeTrig_h603fc354__0(vlSelf, 
                                                        "@(posedge test_blitter.clk)");
    co_await vlSelfRef.__VtrigSched_h603fc354__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge test_blitter.clk)", 
                                                         "test_blitter.sv", 
                                                         115);
    vlSelfRef.test_blitter__DOT__cpu_we = 0U;
    __Vtask_test_blitter__DOT__blt_reg__228__data = 
        (0x000000ffU & (__Vtask_test_blitter__DOT__setup_fill__213__height 
                        >> 8U));
    __Vtask_test_blitter__DOT__blt_reg__228__off = 0x0000000eU;
    __Vtask_test_blitter__DOT__write_reg__229__data 
        = __Vtask_test_blitter__DOT__blt_reg__228__data;
    __Vtask_test_blitter__DOT__write_reg__229__addr 
        = (0x0000ffffU & ((IData)(0xba83U) + __Vtask_test_blitter__DOT__blt_reg__228__off));
    Vtest_blitter___024root____VbeforeTrig_h603fc354__0(vlSelf, 
                                                        "@(posedge test_blitter.clk)");
    co_await vlSelfRef.__VtrigSched_h603fc354__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge test_blitter.clk)", 
                                                         "test_blitter.sv", 
                                                         113);
    vlSelfRef.test_blitter__DOT__cpu_addr = __Vtask_test_blitter__DOT__write_reg__229__addr;
    vlSelfRef.test_blitter__DOT__cpu_wdata = __Vtask_test_blitter__DOT__write_reg__229__data;
    vlSelfRef.test_blitter__DOT__cpu_we = 1U;
    Vtest_blitter___024root____VbeforeTrig_h603fc354__0(vlSelf, 
                                                        "@(posedge test_blitter.clk)");
    co_await vlSelfRef.__VtrigSched_h603fc354__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge test_blitter.clk)", 
                                                         "test_blitter.sv", 
                                                         115);
    vlSelfRef.test_blitter__DOT__cpu_we = 0U;
    __Vtask_test_blitter__DOT__blt_reg__230__data = 
        (0x000000ffU & __Vtask_test_blitter__DOT__setup_fill__213__dst_stride);
    __Vtask_test_blitter__DOT__blt_reg__230__off = 0x00000011U;
    __Vtask_test_blitter__DOT__write_reg__231__data 
        = __Vtask_test_blitter__DOT__blt_reg__230__data;
    __Vtask_test_blitter__DOT__write_reg__231__addr 
        = (0x0000ffffU & ((IData)(0xba83U) + __Vtask_test_blitter__DOT__blt_reg__230__off));
    Vtest_blitter___024root____VbeforeTrig_h603fc354__0(vlSelf, 
                                                        "@(posedge test_blitter.clk)");
    co_await vlSelfRef.__VtrigSched_h603fc354__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge test_blitter.clk)", 
                                                         "test_blitter.sv", 
                                                         113);
    vlSelfRef.test_blitter__DOT__cpu_addr = __Vtask_test_blitter__DOT__write_reg__231__addr;
    vlSelfRef.test_blitter__DOT__cpu_wdata = __Vtask_test_blitter__DOT__write_reg__231__data;
    vlSelfRef.test_blitter__DOT__cpu_we = 1U;
    Vtest_blitter___024root____VbeforeTrig_h603fc354__0(vlSelf, 
                                                        "@(posedge test_blitter.clk)");
    co_await vlSelfRef.__VtrigSched_h603fc354__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge test_blitter.clk)", 
                                                         "test_blitter.sv", 
                                                         115);
    vlSelfRef.test_blitter__DOT__cpu_we = 0U;
    __Vtask_test_blitter__DOT__blt_reg__232__data = 
        (0x000000ffU & (__Vtask_test_blitter__DOT__setup_fill__213__dst_stride 
                        >> 8U));
    __Vtask_test_blitter__DOT__blt_reg__232__off = 0x00000012U;
    __Vtask_test_blitter__DOT__write_reg__233__data 
        = __Vtask_test_blitter__DOT__blt_reg__232__data;
    __Vtask_test_blitter__DOT__write_reg__233__addr 
        = (0x0000ffffU & ((IData)(0xba83U) + __Vtask_test_blitter__DOT__blt_reg__232__off));
    Vtest_blitter___024root____VbeforeTrig_h603fc354__0(vlSelf, 
                                                        "@(posedge test_blitter.clk)");
    co_await vlSelfRef.__VtrigSched_h603fc354__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge test_blitter.clk)", 
                                                         "test_blitter.sv", 
                                                         113);
    vlSelfRef.test_blitter__DOT__cpu_addr = __Vtask_test_blitter__DOT__write_reg__233__addr;
    vlSelfRef.test_blitter__DOT__cpu_wdata = __Vtask_test_blitter__DOT__write_reg__233__data;
    vlSelfRef.test_blitter__DOT__cpu_we = 1U;
    Vtest_blitter___024root____VbeforeTrig_h603fc354__0(vlSelf, 
                                                        "@(posedge test_blitter.clk)");
    co_await vlSelfRef.__VtrigSched_h603fc354__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge test_blitter.clk)", 
                                                         "test_blitter.sv", 
                                                         115);
    vlSelfRef.test_blitter__DOT__cpu_we = 0U;
    __Vtask_test_blitter__DOT__blt_reg__234__data = 1U;
    __Vtask_test_blitter__DOT__blt_reg__234__off = 0x00000013U;
    __Vtask_test_blitter__DOT__write_reg__235__data 
        = __Vtask_test_blitter__DOT__blt_reg__234__data;
    __Vtask_test_blitter__DOT__write_reg__235__addr 
        = (0x0000ffffU & ((IData)(0xba83U) + __Vtask_test_blitter__DOT__blt_reg__234__off));
    Vtest_blitter___024root____VbeforeTrig_h603fc354__0(vlSelf, 
                                                        "@(posedge test_blitter.clk)");
    co_await vlSelfRef.__VtrigSched_h603fc354__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge test_blitter.clk)", 
                                                         "test_blitter.sv", 
                                                         113);
    vlSelfRef.test_blitter__DOT__cpu_addr = __Vtask_test_blitter__DOT__write_reg__235__addr;
    vlSelfRef.test_blitter__DOT__cpu_wdata = __Vtask_test_blitter__DOT__write_reg__235__data;
    vlSelfRef.test_blitter__DOT__cpu_we = 1U;
    Vtest_blitter___024root____VbeforeTrig_h603fc354__0(vlSelf, 
                                                        "@(posedge test_blitter.clk)");
    co_await vlSelfRef.__VtrigSched_h603fc354__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge test_blitter.clk)", 
                                                         "test_blitter.sv", 
                                                         115);
    vlSelfRef.test_blitter__DOT__cpu_we = 0U;
    __Vtask_test_blitter__DOT__blt_reg__236__data = 
        (0x000000ffU & __Vtask_test_blitter__DOT__setup_fill__213__fill_val);
    __Vtask_test_blitter__DOT__blt_reg__236__off = 0x00000014U;
    __Vtask_test_blitter__DOT__write_reg__237__data 
        = __Vtask_test_blitter__DOT__blt_reg__236__data;
    __Vtask_test_blitter__DOT__write_reg__237__addr 
        = (0x0000ffffU & ((IData)(0xba83U) + __Vtask_test_blitter__DOT__blt_reg__236__off));
    Vtest_blitter___024root____VbeforeTrig_h603fc354__0(vlSelf, 
                                                        "@(posedge test_blitter.clk)");
    co_await vlSelfRef.__VtrigSched_h603fc354__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge test_blitter.clk)", 
                                                         "test_blitter.sv", 
                                                         113);
    vlSelfRef.test_blitter__DOT__cpu_addr = __Vtask_test_blitter__DOT__write_reg__237__addr;
    vlSelfRef.test_blitter__DOT__cpu_wdata = __Vtask_test_blitter__DOT__write_reg__237__data;
    vlSelfRef.test_blitter__DOT__cpu_we = 1U;
    Vtest_blitter___024root____VbeforeTrig_h603fc354__0(vlSelf, 
                                                        "@(posedge test_blitter.clk)");
    co_await vlSelfRef.__VtrigSched_h603fc354__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge test_blitter.clk)", 
                                                         "test_blitter.sv", 
                                                         115);
    vlSelfRef.test_blitter__DOT__cpu_we = 0U;
    __Vtask_test_blitter__DOT__blt_reg__239__data = 1U;
    __Vtask_test_blitter__DOT__blt_reg__239__off = 0U;
    __Vtask_test_blitter__DOT__write_reg__240__data 
        = __Vtask_test_blitter__DOT__blt_reg__239__data;
    __Vtask_test_blitter__DOT__write_reg__240__addr 
        = (0x0000ffffU & ((IData)(0xba83U) + __Vtask_test_blitter__DOT__blt_reg__239__off));
    Vtest_blitter___024root____VbeforeTrig_h603fc354__0(vlSelf, 
                                                        "@(posedge test_blitter.clk)");
    co_await vlSelfRef.__VtrigSched_h603fc354__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge test_blitter.clk)", 
                                                         "test_blitter.sv", 
                                                         113);
    vlSelfRef.test_blitter__DOT__cpu_addr = __Vtask_test_blitter__DOT__write_reg__240__addr;
    vlSelfRef.test_blitter__DOT__cpu_wdata = __Vtask_test_blitter__DOT__write_reg__240__data;
    vlSelfRef.test_blitter__DOT__cpu_we = 1U;
    Vtest_blitter___024root____VbeforeTrig_h603fc354__0(vlSelf, 
                                                        "@(posedge test_blitter.clk)");
    co_await vlSelfRef.__VtrigSched_h603fc354__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge test_blitter.clk)", 
                                                         "test_blitter.sv", 
                                                         115);
    vlSelfRef.test_blitter__DOT__cpu_we = 0U;
    test_blitter__DOT__unnamedblk1_5__DOT____Vrepeat4 = 2U;
    while (VL_LTS_III(32, 0U, test_blitter__DOT__unnamedblk1_5__DOT____Vrepeat4)) {
        Vtest_blitter___024root____VbeforeTrig_h603fc354__0(vlSelf, 
                                                            "@(posedge test_blitter.clk)");
        co_await vlSelfRef.__VtrigSched_h603fc354__0.trigger(0U, 
                                                             nullptr, 
                                                             "@(posedge test_blitter.clk)", 
                                                             "test_blitter.sv", 
                                                             306);
        test_blitter__DOT__unnamedblk1_5__DOT____Vrepeat4 
            = (test_blitter__DOT__unnamedblk1_5__DOT____Vrepeat4 
               - (IData)(1U));
    }
    __Vtask_test_blitter__DOT__check__241__condition 
        = (1U & (~ ((0U == (IData)(vlSelfRef.test_blitter__DOT__dut__DOT__state)) 
                    | (6U == (IData)(vlSelfRef.test_blitter__DOT__dut__DOT__state)))));
    vlSelfRef.__Vtask_test_blitter__DOT__check__241__name = "RDY low during blit"s;
    vlSelfRef.test_blitter__DOT__test_num = ((IData)(1U) 
                                             + vlSelfRef.test_blitter__DOT__test_num);
    if (__Vtask_test_blitter__DOT__check__241__condition) {
        VL_WRITEF_NX("  PASS [%0d] %@\n",0,32,vlSelfRef.test_blitter__DOT__test_num,
                     -1,&(vlSelfRef.__Vtask_test_blitter__DOT__check__241__name));
        vlSelfRef.test_blitter__DOT__pass_count = ((IData)(1U) 
                                                   + vlSelfRef.test_blitter__DOT__pass_count);
    } else {
        VL_WRITEF_NX("  FAIL [%0d] %@\n",0,32,vlSelfRef.test_blitter__DOT__test_num,
                     -1,&(vlSelfRef.__Vtask_test_blitter__DOT__check__241__name));
        vlSelfRef.test_blitter__DOT__fail_count = ((IData)(1U) 
                                                   + vlSelfRef.test_blitter__DOT__fail_count);
    }
    __Vtask_test_blitter__DOT__wait_blt_done__242__test_blitter__DOT__unnamedblk1_1__DOT____Vrepeat0 = 0;
    __Vtask_test_blitter__DOT__wait_blt_done__242__test_blitter__DOT__unnamedblk1_2__DOT____Vrepeat1 = 0;
    __Vtask_test_blitter__DOT__wait_blt_done__242__test_blitter__DOT__unnamedblk1_1__DOT____Vrepeat0 = 2U;
    while (VL_LTS_III(32, 0U, __Vtask_test_blitter__DOT__wait_blt_done__242__test_blitter__DOT__unnamedblk1_1__DOT____Vrepeat0)) {
        Vtest_blitter___024root____VbeforeTrig_h603fc354__0(vlSelf, 
                                                            "@(posedge test_blitter.clk)");
        co_await vlSelfRef.__VtrigSched_h603fc354__0.trigger(0U, 
                                                             nullptr, 
                                                             "@(posedge test_blitter.clk)", 
                                                             "test_blitter.sv", 
                                                             128);
        __Vtask_test_blitter__DOT__wait_blt_done__242__test_blitter__DOT__unnamedblk1_1__DOT____Vrepeat0 
            = (__Vtask_test_blitter__DOT__wait_blt_done__242__test_blitter__DOT__unnamedblk1_1__DOT____Vrepeat0 
               - (IData)(1U));
    }
    while (((0U != (IData)(vlSelfRef.test_blitter__DOT__dut__DOT__state)) 
            & (6U != (IData)(vlSelfRef.test_blitter__DOT__dut__DOT__state)))) {
        Vtest_blitter___024root____VbeforeTrig_h603fc354__0(vlSelf, 
                                                            "@(posedge test_blitter.clk)");
        co_await vlSelfRef.__VtrigSched_h603fc354__0.trigger(0U, 
                                                             nullptr, 
                                                             "@(posedge test_blitter.clk)", 
                                                             "test_blitter.sv", 
                                                             130);
    }
    __Vtask_test_blitter__DOT__wait_blt_done__242__test_blitter__DOT__unnamedblk1_2__DOT____Vrepeat1 = 2U;
    while (VL_LTS_III(32, 0U, __Vtask_test_blitter__DOT__wait_blt_done__242__test_blitter__DOT__unnamedblk1_2__DOT____Vrepeat1)) {
        Vtest_blitter___024root____VbeforeTrig_h603fc354__0(vlSelf, 
                                                            "@(posedge test_blitter.clk)");
        co_await vlSelfRef.__VtrigSched_h603fc354__0.trigger(0U, 
                                                             nullptr, 
                                                             "@(posedge test_blitter.clk)", 
                                                             "test_blitter.sv", 
                                                             131);
        __Vtask_test_blitter__DOT__wait_blt_done__242__test_blitter__DOT__unnamedblk1_2__DOT____Vrepeat1 
            = (__Vtask_test_blitter__DOT__wait_blt_done__242__test_blitter__DOT__unnamedblk1_2__DOT____Vrepeat1 
               - (IData)(1U));
    }
    __Vtask_test_blitter__DOT__check__243__condition 
        = ((0U == (IData)(vlSelfRef.test_blitter__DOT__dut__DOT__state)) 
           | (6U == (IData)(vlSelfRef.test_blitter__DOT__dut__DOT__state)));
    vlSelfRef.__Vtask_test_blitter__DOT__check__243__name = "RDY restored after blit"s;
    vlSelfRef.test_blitter__DOT__test_num = ((IData)(1U) 
                                             + vlSelfRef.test_blitter__DOT__test_num);
    if (__Vtask_test_blitter__DOT__check__243__condition) {
        VL_WRITEF_NX("  PASS [%0d] %@\n",0,32,vlSelfRef.test_blitter__DOT__test_num,
                     -1,&(vlSelfRef.__Vtask_test_blitter__DOT__check__243__name));
        vlSelfRef.test_blitter__DOT__pass_count = ((IData)(1U) 
                                                   + vlSelfRef.test_blitter__DOT__pass_count);
    } else {
        VL_WRITEF_NX("  FAIL [%0d] %@\n",0,32,vlSelfRef.test_blitter__DOT__test_num,
                     -1,&(vlSelfRef.__Vtask_test_blitter__DOT__check__243__name));
        vlSelfRef.test_blitter__DOT__fail_count = ((IData)(1U) 
                                                   + vlSelfRef.test_blitter__DOT__fail_count);
    }
    VL_WRITEF_NX("Test: Error bad command\n",0);
    __Vtask_test_blitter__DOT__blt_reg__244__data = 0xffU;
    __Vtask_test_blitter__DOT__blt_reg__244__off = 0U;
    __Vtask_test_blitter__DOT__write_reg__245__data 
        = __Vtask_test_blitter__DOT__blt_reg__244__data;
    __Vtask_test_blitter__DOT__write_reg__245__addr 
        = (0x0000ffffU & ((IData)(0xba83U) + __Vtask_test_blitter__DOT__blt_reg__244__off));
    Vtest_blitter___024root____VbeforeTrig_h603fc354__0(vlSelf, 
                                                        "@(posedge test_blitter.clk)");
    co_await vlSelfRef.__VtrigSched_h603fc354__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge test_blitter.clk)", 
                                                         "test_blitter.sv", 
                                                         113);
    vlSelfRef.test_blitter__DOT__cpu_addr = __Vtask_test_blitter__DOT__write_reg__245__addr;
    vlSelfRef.test_blitter__DOT__cpu_wdata = __Vtask_test_blitter__DOT__write_reg__245__data;
    vlSelfRef.test_blitter__DOT__cpu_we = 1U;
    Vtest_blitter___024root____VbeforeTrig_h603fc354__0(vlSelf, 
                                                        "@(posedge test_blitter.clk)");
    co_await vlSelfRef.__VtrigSched_h603fc354__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge test_blitter.clk)", 
                                                         "test_blitter.sv", 
                                                         115);
    vlSelfRef.test_blitter__DOT__cpu_we = 0U;
    test_blitter__DOT__unnamedblk1_6__DOT____Vrepeat5 = 5U;
    while (VL_LTS_III(32, 0U, test_blitter__DOT__unnamedblk1_6__DOT____Vrepeat5)) {
        Vtest_blitter___024root____VbeforeTrig_h603fc354__0(vlSelf, 
                                                            "@(posedge test_blitter.clk)");
        co_await vlSelfRef.__VtrigSched_h603fc354__0.trigger(0U, 
                                                             nullptr, 
                                                             "@(posedge test_blitter.clk)", 
                                                             "test_blitter.sv", 
                                                             314);
        test_blitter__DOT__unnamedblk1_6__DOT____Vrepeat5 
            = (test_blitter__DOT__unnamedblk1_6__DOT____Vrepeat5 
               - (IData)(1U));
    }
    __Vtask_test_blitter__DOT__check__246__condition 
        = (3U == vlSelfRef.test_blitter__DOT__dut__DOT__regs[1U]);
    vlSelfRef.__Vtask_test_blitter__DOT__check__246__name = "status = error"s;
    vlSelfRef.test_blitter__DOT__test_num = ((IData)(1U) 
                                             + vlSelfRef.test_blitter__DOT__test_num);
    if (__Vtask_test_blitter__DOT__check__246__condition) {
        VL_WRITEF_NX("  PASS [%0d] %@\n",0,32,vlSelfRef.test_blitter__DOT__test_num,
                     -1,&(vlSelfRef.__Vtask_test_blitter__DOT__check__246__name));
        vlSelfRef.test_blitter__DOT__pass_count = ((IData)(1U) 
                                                   + vlSelfRef.test_blitter__DOT__pass_count);
    } else {
        VL_WRITEF_NX("  FAIL [%0d] %@\n",0,32,vlSelfRef.test_blitter__DOT__test_num,
                     -1,&(vlSelfRef.__Vtask_test_blitter__DOT__check__246__name));
        vlSelfRef.test_blitter__DOT__fail_count = ((IData)(1U) 
                                                   + vlSelfRef.test_blitter__DOT__fail_count);
    }
    __Vtask_test_blitter__DOT__check__247__condition 
        = (1U == vlSelfRef.test_blitter__DOT__dut__DOT__regs[2U]);
    vlSelfRef.__Vtask_test_blitter__DOT__check__247__name = "errcode = badcmd"s;
    vlSelfRef.test_blitter__DOT__test_num = ((IData)(1U) 
                                             + vlSelfRef.test_blitter__DOT__test_num);
    if (__Vtask_test_blitter__DOT__check__247__condition) {
        VL_WRITEF_NX("  PASS [%0d] %@\n",0,32,vlSelfRef.test_blitter__DOT__test_num,
                     -1,&(vlSelfRef.__Vtask_test_blitter__DOT__check__247__name));
        vlSelfRef.test_blitter__DOT__pass_count = ((IData)(1U) 
                                                   + vlSelfRef.test_blitter__DOT__pass_count);
    } else {
        VL_WRITEF_NX("  FAIL [%0d] %@\n",0,32,vlSelfRef.test_blitter__DOT__test_num,
                     -1,&(vlSelfRef.__Vtask_test_blitter__DOT__check__247__name));
        vlSelfRef.test_blitter__DOT__fail_count = ((IData)(1U) 
                                                   + vlSelfRef.test_blitter__DOT__fail_count);
    }
    VL_WRITEF_NX("Test: Error zero width\n",0);
    __Vtask_test_blitter__DOT__blt_reg__248__data = 0U;
    __Vtask_test_blitter__DOT__blt_reg__248__off = 0x0000000bU;
    __Vtask_test_blitter__DOT__write_reg__249__data 
        = __Vtask_test_blitter__DOT__blt_reg__248__data;
    __Vtask_test_blitter__DOT__write_reg__249__addr 
        = (0x0000ffffU & ((IData)(0xba83U) + __Vtask_test_blitter__DOT__blt_reg__248__off));
    Vtest_blitter___024root____VbeforeTrig_h603fc354__0(vlSelf, 
                                                        "@(posedge test_blitter.clk)");
    co_await vlSelfRef.__VtrigSched_h603fc354__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge test_blitter.clk)", 
                                                         "test_blitter.sv", 
                                                         113);
    vlSelfRef.test_blitter__DOT__cpu_addr = __Vtask_test_blitter__DOT__write_reg__249__addr;
    vlSelfRef.test_blitter__DOT__cpu_wdata = __Vtask_test_blitter__DOT__write_reg__249__data;
    vlSelfRef.test_blitter__DOT__cpu_we = 1U;
    Vtest_blitter___024root____VbeforeTrig_h603fc354__0(vlSelf, 
                                                        "@(posedge test_blitter.clk)");
    co_await vlSelfRef.__VtrigSched_h603fc354__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge test_blitter.clk)", 
                                                         "test_blitter.sv", 
                                                         115);
    vlSelfRef.test_blitter__DOT__cpu_we = 0U;
    __Vtask_test_blitter__DOT__blt_reg__250__data = 0U;
    __Vtask_test_blitter__DOT__blt_reg__250__off = 0x0000000cU;
    __Vtask_test_blitter__DOT__write_reg__251__data 
        = __Vtask_test_blitter__DOT__blt_reg__250__data;
    __Vtask_test_blitter__DOT__write_reg__251__addr 
        = (0x0000ffffU & ((IData)(0xba83U) + __Vtask_test_blitter__DOT__blt_reg__250__off));
    Vtest_blitter___024root____VbeforeTrig_h603fc354__0(vlSelf, 
                                                        "@(posedge test_blitter.clk)");
    co_await vlSelfRef.__VtrigSched_h603fc354__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge test_blitter.clk)", 
                                                         "test_blitter.sv", 
                                                         113);
    vlSelfRef.test_blitter__DOT__cpu_addr = __Vtask_test_blitter__DOT__write_reg__251__addr;
    vlSelfRef.test_blitter__DOT__cpu_wdata = __Vtask_test_blitter__DOT__write_reg__251__data;
    vlSelfRef.test_blitter__DOT__cpu_we = 1U;
    Vtest_blitter___024root____VbeforeTrig_h603fc354__0(vlSelf, 
                                                        "@(posedge test_blitter.clk)");
    co_await vlSelfRef.__VtrigSched_h603fc354__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge test_blitter.clk)", 
                                                         "test_blitter.sv", 
                                                         115);
    vlSelfRef.test_blitter__DOT__cpu_we = 0U;
    __Vtask_test_blitter__DOT__blt_reg__252__data = 1U;
    __Vtask_test_blitter__DOT__blt_reg__252__off = 0x0000000dU;
    __Vtask_test_blitter__DOT__write_reg__253__data 
        = __Vtask_test_blitter__DOT__blt_reg__252__data;
    __Vtask_test_blitter__DOT__write_reg__253__addr 
        = (0x0000ffffU & ((IData)(0xba83U) + __Vtask_test_blitter__DOT__blt_reg__252__off));
    Vtest_blitter___024root____VbeforeTrig_h603fc354__0(vlSelf, 
                                                        "@(posedge test_blitter.clk)");
    co_await vlSelfRef.__VtrigSched_h603fc354__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge test_blitter.clk)", 
                                                         "test_blitter.sv", 
                                                         113);
    vlSelfRef.test_blitter__DOT__cpu_addr = __Vtask_test_blitter__DOT__write_reg__253__addr;
    vlSelfRef.test_blitter__DOT__cpu_wdata = __Vtask_test_blitter__DOT__write_reg__253__data;
    vlSelfRef.test_blitter__DOT__cpu_we = 1U;
    Vtest_blitter___024root____VbeforeTrig_h603fc354__0(vlSelf, 
                                                        "@(posedge test_blitter.clk)");
    co_await vlSelfRef.__VtrigSched_h603fc354__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge test_blitter.clk)", 
                                                         "test_blitter.sv", 
                                                         115);
    vlSelfRef.test_blitter__DOT__cpu_we = 0U;
    __Vtask_test_blitter__DOT__blt_reg__254__data = 0U;
    __Vtask_test_blitter__DOT__blt_reg__254__off = 0x0000000eU;
    __Vtask_test_blitter__DOT__write_reg__255__data 
        = __Vtask_test_blitter__DOT__blt_reg__254__data;
    __Vtask_test_blitter__DOT__write_reg__255__addr 
        = (0x0000ffffU & ((IData)(0xba83U) + __Vtask_test_blitter__DOT__blt_reg__254__off));
    Vtest_blitter___024root____VbeforeTrig_h603fc354__0(vlSelf, 
                                                        "@(posedge test_blitter.clk)");
    co_await vlSelfRef.__VtrigSched_h603fc354__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge test_blitter.clk)", 
                                                         "test_blitter.sv", 
                                                         113);
    vlSelfRef.test_blitter__DOT__cpu_addr = __Vtask_test_blitter__DOT__write_reg__255__addr;
    vlSelfRef.test_blitter__DOT__cpu_wdata = __Vtask_test_blitter__DOT__write_reg__255__data;
    vlSelfRef.test_blitter__DOT__cpu_we = 1U;
    Vtest_blitter___024root____VbeforeTrig_h603fc354__0(vlSelf, 
                                                        "@(posedge test_blitter.clk)");
    co_await vlSelfRef.__VtrigSched_h603fc354__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge test_blitter.clk)", 
                                                         "test_blitter.sv", 
                                                         115);
    vlSelfRef.test_blitter__DOT__cpu_we = 0U;
    __Vtask_test_blitter__DOT__blt_reg__256__data = 1U;
    __Vtask_test_blitter__DOT__blt_reg__256__off = 0x00000013U;
    __Vtask_test_blitter__DOT__write_reg__257__data 
        = __Vtask_test_blitter__DOT__blt_reg__256__data;
    __Vtask_test_blitter__DOT__write_reg__257__addr 
        = (0x0000ffffU & ((IData)(0xba83U) + __Vtask_test_blitter__DOT__blt_reg__256__off));
    Vtest_blitter___024root____VbeforeTrig_h603fc354__0(vlSelf, 
                                                        "@(posedge test_blitter.clk)");
    co_await vlSelfRef.__VtrigSched_h603fc354__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge test_blitter.clk)", 
                                                         "test_blitter.sv", 
                                                         113);
    vlSelfRef.test_blitter__DOT__cpu_addr = __Vtask_test_blitter__DOT__write_reg__257__addr;
    vlSelfRef.test_blitter__DOT__cpu_wdata = __Vtask_test_blitter__DOT__write_reg__257__data;
    vlSelfRef.test_blitter__DOT__cpu_we = 1U;
    Vtest_blitter___024root____VbeforeTrig_h603fc354__0(vlSelf, 
                                                        "@(posedge test_blitter.clk)");
    co_await vlSelfRef.__VtrigSched_h603fc354__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge test_blitter.clk)", 
                                                         "test_blitter.sv", 
                                                         115);
    vlSelfRef.test_blitter__DOT__cpu_we = 0U;
    __Vtask_test_blitter__DOT__blt_reg__259__data = 1U;
    __Vtask_test_blitter__DOT__blt_reg__259__off = 0U;
    __Vtask_test_blitter__DOT__write_reg__260__data 
        = __Vtask_test_blitter__DOT__blt_reg__259__data;
    __Vtask_test_blitter__DOT__write_reg__260__addr 
        = (0x0000ffffU & ((IData)(0xba83U) + __Vtask_test_blitter__DOT__blt_reg__259__off));
    Vtest_blitter___024root____VbeforeTrig_h603fc354__0(vlSelf, 
                                                        "@(posedge test_blitter.clk)");
    co_await vlSelfRef.__VtrigSched_h603fc354__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge test_blitter.clk)", 
                                                         "test_blitter.sv", 
                                                         113);
    vlSelfRef.test_blitter__DOT__cpu_addr = __Vtask_test_blitter__DOT__write_reg__260__addr;
    vlSelfRef.test_blitter__DOT__cpu_wdata = __Vtask_test_blitter__DOT__write_reg__260__data;
    vlSelfRef.test_blitter__DOT__cpu_we = 1U;
    Vtest_blitter___024root____VbeforeTrig_h603fc354__0(vlSelf, 
                                                        "@(posedge test_blitter.clk)");
    co_await vlSelfRef.__VtrigSched_h603fc354__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge test_blitter.clk)", 
                                                         "test_blitter.sv", 
                                                         115);
    vlSelfRef.test_blitter__DOT__cpu_we = 0U;
    test_blitter__DOT__unnamedblk1_7__DOT____Vrepeat6 = 5U;
    while (VL_LTS_III(32, 0U, test_blitter__DOT__unnamedblk1_7__DOT____Vrepeat6)) {
        Vtest_blitter___024root____VbeforeTrig_h603fc354__0(vlSelf, 
                                                            "@(posedge test_blitter.clk)");
        co_await vlSelfRef.__VtrigSched_h603fc354__0.trigger(0U, 
                                                             nullptr, 
                                                             "@(posedge test_blitter.clk)", 
                                                             "test_blitter.sv", 
                                                             324);
        test_blitter__DOT__unnamedblk1_7__DOT____Vrepeat6 
            = (test_blitter__DOT__unnamedblk1_7__DOT____Vrepeat6 
               - (IData)(1U));
    }
    __Vtask_test_blitter__DOT__check__261__condition 
        = (3U == vlSelfRef.test_blitter__DOT__dut__DOT__regs[1U]);
    vlSelfRef.__Vtask_test_blitter__DOT__check__261__name = "zero width: status error"s;
    vlSelfRef.test_blitter__DOT__test_num = ((IData)(1U) 
                                             + vlSelfRef.test_blitter__DOT__test_num);
    if (__Vtask_test_blitter__DOT__check__261__condition) {
        VL_WRITEF_NX("  PASS [%0d] %@\n",0,32,vlSelfRef.test_blitter__DOT__test_num,
                     -1,&(vlSelfRef.__Vtask_test_blitter__DOT__check__261__name));
        vlSelfRef.test_blitter__DOT__pass_count = ((IData)(1U) 
                                                   + vlSelfRef.test_blitter__DOT__pass_count);
    } else {
        VL_WRITEF_NX("  FAIL [%0d] %@\n",0,32,vlSelfRef.test_blitter__DOT__test_num,
                     -1,&(vlSelfRef.__Vtask_test_blitter__DOT__check__261__name));
        vlSelfRef.test_blitter__DOT__fail_count = ((IData)(1U) 
                                                   + vlSelfRef.test_blitter__DOT__fail_count);
    }
    __Vtask_test_blitter__DOT__check__262__condition 
        = (4U == vlSelfRef.test_blitter__DOT__dut__DOT__regs[2U]);
    vlSelfRef.__Vtask_test_blitter__DOT__check__262__name = "zero width: errcode badargs"s;
    vlSelfRef.test_blitter__DOT__test_num = ((IData)(1U) 
                                             + vlSelfRef.test_blitter__DOT__test_num);
    if (__Vtask_test_blitter__DOT__check__262__condition) {
        VL_WRITEF_NX("  PASS [%0d] %@\n",0,32,vlSelfRef.test_blitter__DOT__test_num,
                     -1,&(vlSelfRef.__Vtask_test_blitter__DOT__check__262__name));
        vlSelfRef.test_blitter__DOT__pass_count = ((IData)(1U) 
                                                   + vlSelfRef.test_blitter__DOT__pass_count);
    } else {
        VL_WRITEF_NX("  FAIL [%0d] %@\n",0,32,vlSelfRef.test_blitter__DOT__test_num,
                     -1,&(vlSelfRef.__Vtask_test_blitter__DOT__check__262__name));
        vlSelfRef.test_blitter__DOT__fail_count = ((IData)(1U) 
                                                   + vlSelfRef.test_blitter__DOT__fail_count);
    }
    VL_WRITEF_NX("Test: Error zero height\n",0);
    __Vtask_test_blitter__DOT__blt_reg__263__data = 1U;
    __Vtask_test_blitter__DOT__blt_reg__263__off = 0x0000000bU;
    __Vtask_test_blitter__DOT__write_reg__264__data 
        = __Vtask_test_blitter__DOT__blt_reg__263__data;
    __Vtask_test_blitter__DOT__write_reg__264__addr 
        = (0x0000ffffU & ((IData)(0xba83U) + __Vtask_test_blitter__DOT__blt_reg__263__off));
    Vtest_blitter___024root____VbeforeTrig_h603fc354__0(vlSelf, 
                                                        "@(posedge test_blitter.clk)");
    co_await vlSelfRef.__VtrigSched_h603fc354__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge test_blitter.clk)", 
                                                         "test_blitter.sv", 
                                                         113);
    vlSelfRef.test_blitter__DOT__cpu_addr = __Vtask_test_blitter__DOT__write_reg__264__addr;
    vlSelfRef.test_blitter__DOT__cpu_wdata = __Vtask_test_blitter__DOT__write_reg__264__data;
    vlSelfRef.test_blitter__DOT__cpu_we = 1U;
    Vtest_blitter___024root____VbeforeTrig_h603fc354__0(vlSelf, 
                                                        "@(posedge test_blitter.clk)");
    co_await vlSelfRef.__VtrigSched_h603fc354__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge test_blitter.clk)", 
                                                         "test_blitter.sv", 
                                                         115);
    vlSelfRef.test_blitter__DOT__cpu_we = 0U;
    __Vtask_test_blitter__DOT__blt_reg__265__data = 0U;
    __Vtask_test_blitter__DOT__blt_reg__265__off = 0x0000000cU;
    __Vtask_test_blitter__DOT__write_reg__266__data 
        = __Vtask_test_blitter__DOT__blt_reg__265__data;
    __Vtask_test_blitter__DOT__write_reg__266__addr 
        = (0x0000ffffU & ((IData)(0xba83U) + __Vtask_test_blitter__DOT__blt_reg__265__off));
    Vtest_blitter___024root____VbeforeTrig_h603fc354__0(vlSelf, 
                                                        "@(posedge test_blitter.clk)");
    co_await vlSelfRef.__VtrigSched_h603fc354__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge test_blitter.clk)", 
                                                         "test_blitter.sv", 
                                                         113);
    vlSelfRef.test_blitter__DOT__cpu_addr = __Vtask_test_blitter__DOT__write_reg__266__addr;
    vlSelfRef.test_blitter__DOT__cpu_wdata = __Vtask_test_blitter__DOT__write_reg__266__data;
    vlSelfRef.test_blitter__DOT__cpu_we = 1U;
    Vtest_blitter___024root____VbeforeTrig_h603fc354__0(vlSelf, 
                                                        "@(posedge test_blitter.clk)");
    co_await vlSelfRef.__VtrigSched_h603fc354__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge test_blitter.clk)", 
                                                         "test_blitter.sv", 
                                                         115);
    vlSelfRef.test_blitter__DOT__cpu_we = 0U;
    __Vtask_test_blitter__DOT__blt_reg__267__data = 0U;
    __Vtask_test_blitter__DOT__blt_reg__267__off = 0x0000000dU;
    __Vtask_test_blitter__DOT__write_reg__268__data 
        = __Vtask_test_blitter__DOT__blt_reg__267__data;
    __Vtask_test_blitter__DOT__write_reg__268__addr 
        = (0x0000ffffU & ((IData)(0xba83U) + __Vtask_test_blitter__DOT__blt_reg__267__off));
    Vtest_blitter___024root____VbeforeTrig_h603fc354__0(vlSelf, 
                                                        "@(posedge test_blitter.clk)");
    co_await vlSelfRef.__VtrigSched_h603fc354__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge test_blitter.clk)", 
                                                         "test_blitter.sv", 
                                                         113);
    vlSelfRef.test_blitter__DOT__cpu_addr = __Vtask_test_blitter__DOT__write_reg__268__addr;
    vlSelfRef.test_blitter__DOT__cpu_wdata = __Vtask_test_blitter__DOT__write_reg__268__data;
    vlSelfRef.test_blitter__DOT__cpu_we = 1U;
    Vtest_blitter___024root____VbeforeTrig_h603fc354__0(vlSelf, 
                                                        "@(posedge test_blitter.clk)");
    co_await vlSelfRef.__VtrigSched_h603fc354__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge test_blitter.clk)", 
                                                         "test_blitter.sv", 
                                                         115);
    vlSelfRef.test_blitter__DOT__cpu_we = 0U;
    __Vtask_test_blitter__DOT__blt_reg__269__data = 0U;
    __Vtask_test_blitter__DOT__blt_reg__269__off = 0x0000000eU;
    __Vtask_test_blitter__DOT__write_reg__270__data 
        = __Vtask_test_blitter__DOT__blt_reg__269__data;
    __Vtask_test_blitter__DOT__write_reg__270__addr 
        = (0x0000ffffU & ((IData)(0xba83U) + __Vtask_test_blitter__DOT__blt_reg__269__off));
    Vtest_blitter___024root____VbeforeTrig_h603fc354__0(vlSelf, 
                                                        "@(posedge test_blitter.clk)");
    co_await vlSelfRef.__VtrigSched_h603fc354__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge test_blitter.clk)", 
                                                         "test_blitter.sv", 
                                                         113);
    vlSelfRef.test_blitter__DOT__cpu_addr = __Vtask_test_blitter__DOT__write_reg__270__addr;
    vlSelfRef.test_blitter__DOT__cpu_wdata = __Vtask_test_blitter__DOT__write_reg__270__data;
    vlSelfRef.test_blitter__DOT__cpu_we = 1U;
    Vtest_blitter___024root____VbeforeTrig_h603fc354__0(vlSelf, 
                                                        "@(posedge test_blitter.clk)");
    co_await vlSelfRef.__VtrigSched_h603fc354__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge test_blitter.clk)", 
                                                         "test_blitter.sv", 
                                                         115);
    vlSelfRef.test_blitter__DOT__cpu_we = 0U;
    __Vtask_test_blitter__DOT__blt_reg__272__data = 1U;
    __Vtask_test_blitter__DOT__blt_reg__272__off = 0U;
    __Vtask_test_blitter__DOT__write_reg__273__data 
        = __Vtask_test_blitter__DOT__blt_reg__272__data;
    __Vtask_test_blitter__DOT__write_reg__273__addr 
        = (0x0000ffffU & ((IData)(0xba83U) + __Vtask_test_blitter__DOT__blt_reg__272__off));
    Vtest_blitter___024root____VbeforeTrig_h603fc354__0(vlSelf, 
                                                        "@(posedge test_blitter.clk)");
    co_await vlSelfRef.__VtrigSched_h603fc354__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge test_blitter.clk)", 
                                                         "test_blitter.sv", 
                                                         113);
    vlSelfRef.test_blitter__DOT__cpu_addr = __Vtask_test_blitter__DOT__write_reg__273__addr;
    vlSelfRef.test_blitter__DOT__cpu_wdata = __Vtask_test_blitter__DOT__write_reg__273__data;
    vlSelfRef.test_blitter__DOT__cpu_we = 1U;
    Vtest_blitter___024root____VbeforeTrig_h603fc354__0(vlSelf, 
                                                        "@(posedge test_blitter.clk)");
    co_await vlSelfRef.__VtrigSched_h603fc354__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge test_blitter.clk)", 
                                                         "test_blitter.sv", 
                                                         115);
    vlSelfRef.test_blitter__DOT__cpu_we = 0U;
    test_blitter__DOT__unnamedblk1_8__DOT____Vrepeat7 = 5U;
    while (VL_LTS_III(32, 0U, test_blitter__DOT__unnamedblk1_8__DOT____Vrepeat7)) {
        Vtest_blitter___024root____VbeforeTrig_h603fc354__0(vlSelf, 
                                                            "@(posedge test_blitter.clk)");
        co_await vlSelfRef.__VtrigSched_h603fc354__0.trigger(0U, 
                                                             nullptr, 
                                                             "@(posedge test_blitter.clk)", 
                                                             "test_blitter.sv", 
                                                             333);
        test_blitter__DOT__unnamedblk1_8__DOT____Vrepeat7 
            = (test_blitter__DOT__unnamedblk1_8__DOT____Vrepeat7 
               - (IData)(1U));
    }
    __Vtask_test_blitter__DOT__check__274__condition 
        = (3U == vlSelfRef.test_blitter__DOT__dut__DOT__regs[1U]);
    vlSelfRef.__Vtask_test_blitter__DOT__check__274__name = "zero height: status error"s;
    vlSelfRef.test_blitter__DOT__test_num = ((IData)(1U) 
                                             + vlSelfRef.test_blitter__DOT__test_num);
    if (__Vtask_test_blitter__DOT__check__274__condition) {
        VL_WRITEF_NX("  PASS [%0d] %@\n",0,32,vlSelfRef.test_blitter__DOT__test_num,
                     -1,&(vlSelfRef.__Vtask_test_blitter__DOT__check__274__name));
        vlSelfRef.test_blitter__DOT__pass_count = ((IData)(1U) 
                                                   + vlSelfRef.test_blitter__DOT__pass_count);
    } else {
        VL_WRITEF_NX("  FAIL [%0d] %@\n",0,32,vlSelfRef.test_blitter__DOT__test_num,
                     -1,&(vlSelfRef.__Vtask_test_blitter__DOT__check__274__name));
        vlSelfRef.test_blitter__DOT__fail_count = ((IData)(1U) 
                                                   + vlSelfRef.test_blitter__DOT__fail_count);
    }
    __Vtask_test_blitter__DOT__check__275__condition 
        = (4U == vlSelfRef.test_blitter__DOT__dut__DOT__regs[2U]);
    vlSelfRef.__Vtask_test_blitter__DOT__check__275__name = "zero height: errcode badargs"s;
    vlSelfRef.test_blitter__DOT__test_num = ((IData)(1U) 
                                             + vlSelfRef.test_blitter__DOT__test_num);
    if (__Vtask_test_blitter__DOT__check__275__condition) {
        VL_WRITEF_NX("  PASS [%0d] %@\n",0,32,vlSelfRef.test_blitter__DOT__test_num,
                     -1,&(vlSelfRef.__Vtask_test_blitter__DOT__check__275__name));
        vlSelfRef.test_blitter__DOT__pass_count = ((IData)(1U) 
                                                   + vlSelfRef.test_blitter__DOT__pass_count);
    } else {
        VL_WRITEF_NX("  FAIL [%0d] %@\n",0,32,vlSelfRef.test_blitter__DOT__test_num,
                     -1,&(vlSelfRef.__Vtask_test_blitter__DOT__check__275__name));
        vlSelfRef.test_blitter__DOT__fail_count = ((IData)(1U) 
                                                   + vlSelfRef.test_blitter__DOT__fail_count);
    }
    VL_WRITEF_NX("Test: Error bad space\n",0);
    __Vtask_test_blitter__DOT__blt_reg__276__data = 7U;
    __Vtask_test_blitter__DOT__blt_reg__276__off = 4U;
    __Vtask_test_blitter__DOT__write_reg__277__data 
        = __Vtask_test_blitter__DOT__blt_reg__276__data;
    __Vtask_test_blitter__DOT__write_reg__277__addr 
        = (0x0000ffffU & ((IData)(0xba83U) + __Vtask_test_blitter__DOT__blt_reg__276__off));
    Vtest_blitter___024root____VbeforeTrig_h603fc354__0(vlSelf, 
                                                        "@(posedge test_blitter.clk)");
    co_await vlSelfRef.__VtrigSched_h603fc354__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge test_blitter.clk)", 
                                                         "test_blitter.sv", 
                                                         113);
    vlSelfRef.test_blitter__DOT__cpu_addr = __Vtask_test_blitter__DOT__write_reg__277__addr;
    vlSelfRef.test_blitter__DOT__cpu_wdata = __Vtask_test_blitter__DOT__write_reg__277__data;
    vlSelfRef.test_blitter__DOT__cpu_we = 1U;
    Vtest_blitter___024root____VbeforeTrig_h603fc354__0(vlSelf, 
                                                        "@(posedge test_blitter.clk)");
    co_await vlSelfRef.__VtrigSched_h603fc354__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge test_blitter.clk)", 
                                                         "test_blitter.sv", 
                                                         115);
    vlSelfRef.test_blitter__DOT__cpu_we = 0U;
    __Vtask_test_blitter__DOT__blt_reg__278__data = 1U;
    __Vtask_test_blitter__DOT__blt_reg__278__off = 0x0000000bU;
    __Vtask_test_blitter__DOT__write_reg__279__data 
        = __Vtask_test_blitter__DOT__blt_reg__278__data;
    __Vtask_test_blitter__DOT__write_reg__279__addr 
        = (0x0000ffffU & ((IData)(0xba83U) + __Vtask_test_blitter__DOT__blt_reg__278__off));
    Vtest_blitter___024root____VbeforeTrig_h603fc354__0(vlSelf, 
                                                        "@(posedge test_blitter.clk)");
    co_await vlSelfRef.__VtrigSched_h603fc354__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge test_blitter.clk)", 
                                                         "test_blitter.sv", 
                                                         113);
    vlSelfRef.test_blitter__DOT__cpu_addr = __Vtask_test_blitter__DOT__write_reg__279__addr;
    vlSelfRef.test_blitter__DOT__cpu_wdata = __Vtask_test_blitter__DOT__write_reg__279__data;
    vlSelfRef.test_blitter__DOT__cpu_we = 1U;
    Vtest_blitter___024root____VbeforeTrig_h603fc354__0(vlSelf, 
                                                        "@(posedge test_blitter.clk)");
    co_await vlSelfRef.__VtrigSched_h603fc354__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge test_blitter.clk)", 
                                                         "test_blitter.sv", 
                                                         115);
    vlSelfRef.test_blitter__DOT__cpu_we = 0U;
    __Vtask_test_blitter__DOT__blt_reg__280__data = 0U;
    __Vtask_test_blitter__DOT__blt_reg__280__off = 0x0000000cU;
    __Vtask_test_blitter__DOT__write_reg__281__data 
        = __Vtask_test_blitter__DOT__blt_reg__280__data;
    __Vtask_test_blitter__DOT__write_reg__281__addr 
        = (0x0000ffffU & ((IData)(0xba83U) + __Vtask_test_blitter__DOT__blt_reg__280__off));
    Vtest_blitter___024root____VbeforeTrig_h603fc354__0(vlSelf, 
                                                        "@(posedge test_blitter.clk)");
    co_await vlSelfRef.__VtrigSched_h603fc354__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge test_blitter.clk)", 
                                                         "test_blitter.sv", 
                                                         113);
    vlSelfRef.test_blitter__DOT__cpu_addr = __Vtask_test_blitter__DOT__write_reg__281__addr;
    vlSelfRef.test_blitter__DOT__cpu_wdata = __Vtask_test_blitter__DOT__write_reg__281__data;
    vlSelfRef.test_blitter__DOT__cpu_we = 1U;
    Vtest_blitter___024root____VbeforeTrig_h603fc354__0(vlSelf, 
                                                        "@(posedge test_blitter.clk)");
    co_await vlSelfRef.__VtrigSched_h603fc354__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge test_blitter.clk)", 
                                                         "test_blitter.sv", 
                                                         115);
    vlSelfRef.test_blitter__DOT__cpu_we = 0U;
    __Vtask_test_blitter__DOT__blt_reg__282__data = 1U;
    __Vtask_test_blitter__DOT__blt_reg__282__off = 0x0000000dU;
    __Vtask_test_blitter__DOT__write_reg__283__data 
        = __Vtask_test_blitter__DOT__blt_reg__282__data;
    __Vtask_test_blitter__DOT__write_reg__283__addr 
        = (0x0000ffffU & ((IData)(0xba83U) + __Vtask_test_blitter__DOT__blt_reg__282__off));
    Vtest_blitter___024root____VbeforeTrig_h603fc354__0(vlSelf, 
                                                        "@(posedge test_blitter.clk)");
    co_await vlSelfRef.__VtrigSched_h603fc354__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge test_blitter.clk)", 
                                                         "test_blitter.sv", 
                                                         113);
    vlSelfRef.test_blitter__DOT__cpu_addr = __Vtask_test_blitter__DOT__write_reg__283__addr;
    vlSelfRef.test_blitter__DOT__cpu_wdata = __Vtask_test_blitter__DOT__write_reg__283__data;
    vlSelfRef.test_blitter__DOT__cpu_we = 1U;
    Vtest_blitter___024root____VbeforeTrig_h603fc354__0(vlSelf, 
                                                        "@(posedge test_blitter.clk)");
    co_await vlSelfRef.__VtrigSched_h603fc354__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge test_blitter.clk)", 
                                                         "test_blitter.sv", 
                                                         115);
    vlSelfRef.test_blitter__DOT__cpu_we = 0U;
    __Vtask_test_blitter__DOT__blt_reg__284__data = 0U;
    __Vtask_test_blitter__DOT__blt_reg__284__off = 0x0000000eU;
    __Vtask_test_blitter__DOT__write_reg__285__data 
        = __Vtask_test_blitter__DOT__blt_reg__284__data;
    __Vtask_test_blitter__DOT__write_reg__285__addr 
        = (0x0000ffffU & ((IData)(0xba83U) + __Vtask_test_blitter__DOT__blt_reg__284__off));
    Vtest_blitter___024root____VbeforeTrig_h603fc354__0(vlSelf, 
                                                        "@(posedge test_blitter.clk)");
    co_await vlSelfRef.__VtrigSched_h603fc354__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge test_blitter.clk)", 
                                                         "test_blitter.sv", 
                                                         113);
    vlSelfRef.test_blitter__DOT__cpu_addr = __Vtask_test_blitter__DOT__write_reg__285__addr;
    vlSelfRef.test_blitter__DOT__cpu_wdata = __Vtask_test_blitter__DOT__write_reg__285__data;
    vlSelfRef.test_blitter__DOT__cpu_we = 1U;
    Vtest_blitter___024root____VbeforeTrig_h603fc354__0(vlSelf, 
                                                        "@(posedge test_blitter.clk)");
    co_await vlSelfRef.__VtrigSched_h603fc354__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge test_blitter.clk)", 
                                                         "test_blitter.sv", 
                                                         115);
    vlSelfRef.test_blitter__DOT__cpu_we = 0U;
    __Vtask_test_blitter__DOT__blt_reg__286__data = 1U;
    __Vtask_test_blitter__DOT__blt_reg__286__off = 0x00000013U;
    __Vtask_test_blitter__DOT__write_reg__287__data 
        = __Vtask_test_blitter__DOT__blt_reg__286__data;
    __Vtask_test_blitter__DOT__write_reg__287__addr 
        = (0x0000ffffU & ((IData)(0xba83U) + __Vtask_test_blitter__DOT__blt_reg__286__off));
    Vtest_blitter___024root____VbeforeTrig_h603fc354__0(vlSelf, 
                                                        "@(posedge test_blitter.clk)");
    co_await vlSelfRef.__VtrigSched_h603fc354__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge test_blitter.clk)", 
                                                         "test_blitter.sv", 
                                                         113);
    vlSelfRef.test_blitter__DOT__cpu_addr = __Vtask_test_blitter__DOT__write_reg__287__addr;
    vlSelfRef.test_blitter__DOT__cpu_wdata = __Vtask_test_blitter__DOT__write_reg__287__data;
    vlSelfRef.test_blitter__DOT__cpu_we = 1U;
    Vtest_blitter___024root____VbeforeTrig_h603fc354__0(vlSelf, 
                                                        "@(posedge test_blitter.clk)");
    co_await vlSelfRef.__VtrigSched_h603fc354__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge test_blitter.clk)", 
                                                         "test_blitter.sv", 
                                                         115);
    vlSelfRef.test_blitter__DOT__cpu_we = 0U;
    __Vtask_test_blitter__DOT__blt_reg__289__data = 1U;
    __Vtask_test_blitter__DOT__blt_reg__289__off = 0U;
    __Vtask_test_blitter__DOT__write_reg__290__data 
        = __Vtask_test_blitter__DOT__blt_reg__289__data;
    __Vtask_test_blitter__DOT__write_reg__290__addr 
        = (0x0000ffffU & ((IData)(0xba83U) + __Vtask_test_blitter__DOT__blt_reg__289__off));
    Vtest_blitter___024root____VbeforeTrig_h603fc354__0(vlSelf, 
                                                        "@(posedge test_blitter.clk)");
    co_await vlSelfRef.__VtrigSched_h603fc354__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge test_blitter.clk)", 
                                                         "test_blitter.sv", 
                                                         113);
    vlSelfRef.test_blitter__DOT__cpu_addr = __Vtask_test_blitter__DOT__write_reg__290__addr;
    vlSelfRef.test_blitter__DOT__cpu_wdata = __Vtask_test_blitter__DOT__write_reg__290__data;
    vlSelfRef.test_blitter__DOT__cpu_we = 1U;
    Vtest_blitter___024root____VbeforeTrig_h603fc354__0(vlSelf, 
                                                        "@(posedge test_blitter.clk)");
    co_await vlSelfRef.__VtrigSched_h603fc354__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge test_blitter.clk)", 
                                                         "test_blitter.sv", 
                                                         115);
    vlSelfRef.test_blitter__DOT__cpu_we = 0U;
    __Vtask_test_blitter__DOT__wait_blt_done__291__test_blitter__DOT__unnamedblk1_1__DOT____Vrepeat0 = 0;
    __Vtask_test_blitter__DOT__wait_blt_done__291__test_blitter__DOT__unnamedblk1_2__DOT____Vrepeat1 = 0;
    __Vtask_test_blitter__DOT__wait_blt_done__291__test_blitter__DOT__unnamedblk1_1__DOT____Vrepeat0 = 2U;
    while (VL_LTS_III(32, 0U, __Vtask_test_blitter__DOT__wait_blt_done__291__test_blitter__DOT__unnamedblk1_1__DOT____Vrepeat0)) {
        Vtest_blitter___024root____VbeforeTrig_h603fc354__0(vlSelf, 
                                                            "@(posedge test_blitter.clk)");
        co_await vlSelfRef.__VtrigSched_h603fc354__0.trigger(0U, 
                                                             nullptr, 
                                                             "@(posedge test_blitter.clk)", 
                                                             "test_blitter.sv", 
                                                             128);
        __Vtask_test_blitter__DOT__wait_blt_done__291__test_blitter__DOT__unnamedblk1_1__DOT____Vrepeat0 
            = (__Vtask_test_blitter__DOT__wait_blt_done__291__test_blitter__DOT__unnamedblk1_1__DOT____Vrepeat0 
               - (IData)(1U));
    }
    while (((0U != (IData)(vlSelfRef.test_blitter__DOT__dut__DOT__state)) 
            & (6U != (IData)(vlSelfRef.test_blitter__DOT__dut__DOT__state)))) {
        Vtest_blitter___024root____VbeforeTrig_h603fc354__0(vlSelf, 
                                                            "@(posedge test_blitter.clk)");
        co_await vlSelfRef.__VtrigSched_h603fc354__0.trigger(0U, 
                                                             nullptr, 
                                                             "@(posedge test_blitter.clk)", 
                                                             "test_blitter.sv", 
                                                             130);
    }
    __Vtask_test_blitter__DOT__wait_blt_done__291__test_blitter__DOT__unnamedblk1_2__DOT____Vrepeat1 = 2U;
    while (VL_LTS_III(32, 0U, __Vtask_test_blitter__DOT__wait_blt_done__291__test_blitter__DOT__unnamedblk1_2__DOT____Vrepeat1)) {
        Vtest_blitter___024root____VbeforeTrig_h603fc354__0(vlSelf, 
                                                            "@(posedge test_blitter.clk)");
        co_await vlSelfRef.__VtrigSched_h603fc354__0.trigger(0U, 
                                                             nullptr, 
                                                             "@(posedge test_blitter.clk)", 
                                                             "test_blitter.sv", 
                                                             131);
        __Vtask_test_blitter__DOT__wait_blt_done__291__test_blitter__DOT__unnamedblk1_2__DOT____Vrepeat1 
            = (__Vtask_test_blitter__DOT__wait_blt_done__291__test_blitter__DOT__unnamedblk1_2__DOT____Vrepeat1 
               - (IData)(1U));
    }
    __Vtask_test_blitter__DOT__check__292__condition 
        = (3U == vlSelfRef.test_blitter__DOT__dut__DOT__regs[1U]);
    vlSelfRef.__Vtask_test_blitter__DOT__check__292__name = "bad space: status error"s;
    vlSelfRef.test_blitter__DOT__test_num = ((IData)(1U) 
                                             + vlSelfRef.test_blitter__DOT__test_num);
    if (__Vtask_test_blitter__DOT__check__292__condition) {
        VL_WRITEF_NX("  PASS [%0d] %@\n",0,32,vlSelfRef.test_blitter__DOT__test_num,
                     -1,&(vlSelfRef.__Vtask_test_blitter__DOT__check__292__name));
        vlSelfRef.test_blitter__DOT__pass_count = ((IData)(1U) 
                                                   + vlSelfRef.test_blitter__DOT__pass_count);
    } else {
        VL_WRITEF_NX("  FAIL [%0d] %@\n",0,32,vlSelfRef.test_blitter__DOT__test_num,
                     -1,&(vlSelfRef.__Vtask_test_blitter__DOT__check__292__name));
        vlSelfRef.test_blitter__DOT__fail_count = ((IData)(1U) 
                                                   + vlSelfRef.test_blitter__DOT__fail_count);
    }
    __Vtask_test_blitter__DOT__check__293__condition 
        = (2U == vlSelfRef.test_blitter__DOT__dut__DOT__regs[2U]);
    vlSelfRef.__Vtask_test_blitter__DOT__check__293__name = "bad space: errcode"s;
    vlSelfRef.test_blitter__DOT__test_num = ((IData)(1U) 
                                             + vlSelfRef.test_blitter__DOT__test_num);
    if (__Vtask_test_blitter__DOT__check__293__condition) {
        VL_WRITEF_NX("  PASS [%0d] %@\n",0,32,vlSelfRef.test_blitter__DOT__test_num,
                     -1,&(vlSelfRef.__Vtask_test_blitter__DOT__check__293__name));
        vlSelfRef.test_blitter__DOT__pass_count = ((IData)(1U) 
                                                   + vlSelfRef.test_blitter__DOT__pass_count);
    } else {
        VL_WRITEF_NX("  FAIL [%0d] %@\n",0,32,vlSelfRef.test_blitter__DOT__test_num,
                     -1,&(vlSelfRef.__Vtask_test_blitter__DOT__check__293__name));
        vlSelfRef.test_blitter__DOT__fail_count = ((IData)(1U) 
                                                   + vlSelfRef.test_blitter__DOT__fail_count);
    }
    VL_WRITEF_NX("Test: 2D stride copy\n",0);
    vlSelfRef.test_blitter__DOT__sim_ram[28672U] = 0U;
    vlSelfRef.test_blitter__DOT__sim_ram[28673U] = 0U;
    vlSelfRef.test_blitter__DOT__sim_ram[28674U] = 0U;
    vlSelfRef.test_blitter__DOT__sim_ram[28675U] = 0U;
    vlSelfRef.test_blitter__DOT__sim_ram[28676U] = 0U;
    vlSelfRef.test_blitter__DOT__sim_ram[28677U] = 0U;
    vlSelfRef.test_blitter__DOT__sim_ram[28678U] = 0U;
    vlSelfRef.test_blitter__DOT__sim_ram[28679U] = 0U;
    vlSelfRef.test_blitter__DOT__sim_ram[28680U] = 0U;
    vlSelfRef.test_blitter__DOT__sim_ram[28681U] = 0U;
    vlSelfRef.test_blitter__DOT__sim_ram[28682U] = 0U;
    vlSelfRef.test_blitter__DOT__sim_ram[28683U] = 0U;
    vlSelfRef.test_blitter__DOT__sim_ram[28684U] = 0U;
    vlSelfRef.test_blitter__DOT__sim_ram[28685U] = 0U;
    vlSelfRef.test_blitter__DOT__sim_ram[28686U] = 0U;
    vlSelfRef.test_blitter__DOT__sim_ram[28687U] = 0U;
    vlSelfRef.test_blitter__DOT__sim_ram[28688U] = 0U;
    vlSelfRef.test_blitter__DOT__sim_ram[28689U] = 0U;
    vlSelfRef.test_blitter__DOT__sim_ram[28690U] = 0U;
    vlSelfRef.test_blitter__DOT__sim_ram[28691U] = 0U;
    vlSelfRef.test_blitter__DOT__sim_ram[28672U] = 0xa1U;
    vlSelfRef.test_blitter__DOT__sim_ram[28673U] = 0xa2U;
    vlSelfRef.test_blitter__DOT__sim_ram[28674U] = 0xa3U;
    vlSelfRef.test_blitter__DOT__sim_ram[28682U] = 0xb1U;
    vlSelfRef.test_blitter__DOT__sim_ram[28683U] = 0xb2U;
    vlSelfRef.test_blitter__DOT__sim_ram[28684U] = 0xb3U;
    vlSelfRef.test_blitter__DOT__sim_ram[28928U] = 0U;
    vlSelfRef.test_blitter__DOT__sim_ram[28929U] = 0U;
    vlSelfRef.test_blitter__DOT__sim_ram[28930U] = 0U;
    vlSelfRef.test_blitter__DOT__sim_ram[28931U] = 0U;
    vlSelfRef.test_blitter__DOT__sim_ram[28932U] = 0U;
    vlSelfRef.test_blitter__DOT__sim_ram[28933U] = 0U;
    __Vtask_test_blitter__DOT__setup_copy__294__height = 2U;
    __Vtask_test_blitter__DOT__setup_copy__294__width = 3U;
    __Vtask_test_blitter__DOT__setup_copy__294__dst_stride = 3U;
    __Vtask_test_blitter__DOT__setup_copy__294__dst_addr = 0x00007100U;
    __Vtask_test_blitter__DOT__setup_copy__294__dst_space = 0U;
    __Vtask_test_blitter__DOT__setup_copy__294__src_stride = 0x0000000aU;
    __Vtask_test_blitter__DOT__setup_copy__294__src_addr = 0x00007000U;
    __Vtask_test_blitter__DOT__setup_copy__294__src_space = 0U;
    __Vtask_test_blitter__DOT__blt_reg__295__data = 
        (0x000000ffU & __Vtask_test_blitter__DOT__setup_copy__294__src_space);
    __Vtask_test_blitter__DOT__blt_reg__295__off = 3U;
    __Vtask_test_blitter__DOT__write_reg__296__data 
        = __Vtask_test_blitter__DOT__blt_reg__295__data;
    __Vtask_test_blitter__DOT__write_reg__296__addr 
        = (0x0000ffffU & ((IData)(0xba83U) + __Vtask_test_blitter__DOT__blt_reg__295__off));
    Vtest_blitter___024root____VbeforeTrig_h603fc354__0(vlSelf, 
                                                        "@(posedge test_blitter.clk)");
    co_await vlSelfRef.__VtrigSched_h603fc354__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge test_blitter.clk)", 
                                                         "test_blitter.sv", 
                                                         113);
    vlSelfRef.test_blitter__DOT__cpu_addr = __Vtask_test_blitter__DOT__write_reg__296__addr;
    vlSelfRef.test_blitter__DOT__cpu_wdata = __Vtask_test_blitter__DOT__write_reg__296__data;
    vlSelfRef.test_blitter__DOT__cpu_we = 1U;
    Vtest_blitter___024root____VbeforeTrig_h603fc354__0(vlSelf, 
                                                        "@(posedge test_blitter.clk)");
    co_await vlSelfRef.__VtrigSched_h603fc354__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge test_blitter.clk)", 
                                                         "test_blitter.sv", 
                                                         115);
    vlSelfRef.test_blitter__DOT__cpu_we = 0U;
    __Vtask_test_blitter__DOT__blt_reg__297__data = 
        (0x000000ffU & __Vtask_test_blitter__DOT__setup_copy__294__dst_space);
    __Vtask_test_blitter__DOT__blt_reg__297__off = 4U;
    __Vtask_test_blitter__DOT__write_reg__298__data 
        = __Vtask_test_blitter__DOT__blt_reg__297__data;
    __Vtask_test_blitter__DOT__write_reg__298__addr 
        = (0x0000ffffU & ((IData)(0xba83U) + __Vtask_test_blitter__DOT__blt_reg__297__off));
    Vtest_blitter___024root____VbeforeTrig_h603fc354__0(vlSelf, 
                                                        "@(posedge test_blitter.clk)");
    co_await vlSelfRef.__VtrigSched_h603fc354__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge test_blitter.clk)", 
                                                         "test_blitter.sv", 
                                                         113);
    vlSelfRef.test_blitter__DOT__cpu_addr = __Vtask_test_blitter__DOT__write_reg__298__addr;
    vlSelfRef.test_blitter__DOT__cpu_wdata = __Vtask_test_blitter__DOT__write_reg__298__data;
    vlSelfRef.test_blitter__DOT__cpu_we = 1U;
    Vtest_blitter___024root____VbeforeTrig_h603fc354__0(vlSelf, 
                                                        "@(posedge test_blitter.clk)");
    co_await vlSelfRef.__VtrigSched_h603fc354__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge test_blitter.clk)", 
                                                         "test_blitter.sv", 
                                                         115);
    vlSelfRef.test_blitter__DOT__cpu_we = 0U;
    __Vtask_test_blitter__DOT__blt_reg__299__data = 
        (0x000000ffU & __Vtask_test_blitter__DOT__setup_copy__294__src_addr);
    __Vtask_test_blitter__DOT__blt_reg__299__off = 5U;
    __Vtask_test_blitter__DOT__write_reg__300__data 
        = __Vtask_test_blitter__DOT__blt_reg__299__data;
    __Vtask_test_blitter__DOT__write_reg__300__addr 
        = (0x0000ffffU & ((IData)(0xba83U) + __Vtask_test_blitter__DOT__blt_reg__299__off));
    Vtest_blitter___024root____VbeforeTrig_h603fc354__0(vlSelf, 
                                                        "@(posedge test_blitter.clk)");
    co_await vlSelfRef.__VtrigSched_h603fc354__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge test_blitter.clk)", 
                                                         "test_blitter.sv", 
                                                         113);
    vlSelfRef.test_blitter__DOT__cpu_addr = __Vtask_test_blitter__DOT__write_reg__300__addr;
    vlSelfRef.test_blitter__DOT__cpu_wdata = __Vtask_test_blitter__DOT__write_reg__300__data;
    vlSelfRef.test_blitter__DOT__cpu_we = 1U;
    Vtest_blitter___024root____VbeforeTrig_h603fc354__0(vlSelf, 
                                                        "@(posedge test_blitter.clk)");
    co_await vlSelfRef.__VtrigSched_h603fc354__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge test_blitter.clk)", 
                                                         "test_blitter.sv", 
                                                         115);
    vlSelfRef.test_blitter__DOT__cpu_we = 0U;
    __Vtask_test_blitter__DOT__blt_reg__301__data = 
        (0x000000ffU & (__Vtask_test_blitter__DOT__setup_copy__294__src_addr 
                        >> 8U));
    __Vtask_test_blitter__DOT__blt_reg__301__off = 6U;
    __Vtask_test_blitter__DOT__write_reg__302__data 
        = __Vtask_test_blitter__DOT__blt_reg__301__data;
    __Vtask_test_blitter__DOT__write_reg__302__addr 
        = (0x0000ffffU & ((IData)(0xba83U) + __Vtask_test_blitter__DOT__blt_reg__301__off));
    Vtest_blitter___024root____VbeforeTrig_h603fc354__0(vlSelf, 
                                                        "@(posedge test_blitter.clk)");
    co_await vlSelfRef.__VtrigSched_h603fc354__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge test_blitter.clk)", 
                                                         "test_blitter.sv", 
                                                         113);
    vlSelfRef.test_blitter__DOT__cpu_addr = __Vtask_test_blitter__DOT__write_reg__302__addr;
    vlSelfRef.test_blitter__DOT__cpu_wdata = __Vtask_test_blitter__DOT__write_reg__302__data;
    vlSelfRef.test_blitter__DOT__cpu_we = 1U;
    Vtest_blitter___024root____VbeforeTrig_h603fc354__0(vlSelf, 
                                                        "@(posedge test_blitter.clk)");
    co_await vlSelfRef.__VtrigSched_h603fc354__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge test_blitter.clk)", 
                                                         "test_blitter.sv", 
                                                         115);
    vlSelfRef.test_blitter__DOT__cpu_we = 0U;
    __Vtask_test_blitter__DOT__blt_reg__303__data = 
        (0x000000ffU & (__Vtask_test_blitter__DOT__setup_copy__294__src_addr 
                        >> 0x10U));
    __Vtask_test_blitter__DOT__blt_reg__303__off = 7U;
    __Vtask_test_blitter__DOT__write_reg__304__data 
        = __Vtask_test_blitter__DOT__blt_reg__303__data;
    __Vtask_test_blitter__DOT__write_reg__304__addr 
        = (0x0000ffffU & ((IData)(0xba83U) + __Vtask_test_blitter__DOT__blt_reg__303__off));
    Vtest_blitter___024root____VbeforeTrig_h603fc354__0(vlSelf, 
                                                        "@(posedge test_blitter.clk)");
    co_await vlSelfRef.__VtrigSched_h603fc354__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge test_blitter.clk)", 
                                                         "test_blitter.sv", 
                                                         113);
    vlSelfRef.test_blitter__DOT__cpu_addr = __Vtask_test_blitter__DOT__write_reg__304__addr;
    vlSelfRef.test_blitter__DOT__cpu_wdata = __Vtask_test_blitter__DOT__write_reg__304__data;
    vlSelfRef.test_blitter__DOT__cpu_we = 1U;
    Vtest_blitter___024root____VbeforeTrig_h603fc354__0(vlSelf, 
                                                        "@(posedge test_blitter.clk)");
    co_await vlSelfRef.__VtrigSched_h603fc354__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge test_blitter.clk)", 
                                                         "test_blitter.sv", 
                                                         115);
    vlSelfRef.test_blitter__DOT__cpu_we = 0U;
    __Vtask_test_blitter__DOT__blt_reg__305__data = 
        (0x000000ffU & __Vtask_test_blitter__DOT__setup_copy__294__dst_addr);
    __Vtask_test_blitter__DOT__blt_reg__305__off = 8U;
    __Vtask_test_blitter__DOT__write_reg__306__data 
        = __Vtask_test_blitter__DOT__blt_reg__305__data;
    __Vtask_test_blitter__DOT__write_reg__306__addr 
        = (0x0000ffffU & ((IData)(0xba83U) + __Vtask_test_blitter__DOT__blt_reg__305__off));
    Vtest_blitter___024root____VbeforeTrig_h603fc354__0(vlSelf, 
                                                        "@(posedge test_blitter.clk)");
    co_await vlSelfRef.__VtrigSched_h603fc354__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge test_blitter.clk)", 
                                                         "test_blitter.sv", 
                                                         113);
    vlSelfRef.test_blitter__DOT__cpu_addr = __Vtask_test_blitter__DOT__write_reg__306__addr;
    vlSelfRef.test_blitter__DOT__cpu_wdata = __Vtask_test_blitter__DOT__write_reg__306__data;
    vlSelfRef.test_blitter__DOT__cpu_we = 1U;
    Vtest_blitter___024root____VbeforeTrig_h603fc354__0(vlSelf, 
                                                        "@(posedge test_blitter.clk)");
    co_await vlSelfRef.__VtrigSched_h603fc354__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge test_blitter.clk)", 
                                                         "test_blitter.sv", 
                                                         115);
    vlSelfRef.test_blitter__DOT__cpu_we = 0U;
    __Vtask_test_blitter__DOT__blt_reg__307__data = 
        (0x000000ffU & (__Vtask_test_blitter__DOT__setup_copy__294__dst_addr 
                        >> 8U));
    __Vtask_test_blitter__DOT__blt_reg__307__off = 9U;
    __Vtask_test_blitter__DOT__write_reg__308__data 
        = __Vtask_test_blitter__DOT__blt_reg__307__data;
    __Vtask_test_blitter__DOT__write_reg__308__addr 
        = (0x0000ffffU & ((IData)(0xba83U) + __Vtask_test_blitter__DOT__blt_reg__307__off));
    Vtest_blitter___024root____VbeforeTrig_h603fc354__0(vlSelf, 
                                                        "@(posedge test_blitter.clk)");
    co_await vlSelfRef.__VtrigSched_h603fc354__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge test_blitter.clk)", 
                                                         "test_blitter.sv", 
                                                         113);
    vlSelfRef.test_blitter__DOT__cpu_addr = __Vtask_test_blitter__DOT__write_reg__308__addr;
    vlSelfRef.test_blitter__DOT__cpu_wdata = __Vtask_test_blitter__DOT__write_reg__308__data;
    vlSelfRef.test_blitter__DOT__cpu_we = 1U;
    Vtest_blitter___024root____VbeforeTrig_h603fc354__0(vlSelf, 
                                                        "@(posedge test_blitter.clk)");
    co_await vlSelfRef.__VtrigSched_h603fc354__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge test_blitter.clk)", 
                                                         "test_blitter.sv", 
                                                         115);
    vlSelfRef.test_blitter__DOT__cpu_we = 0U;
    __Vtask_test_blitter__DOT__blt_reg__309__data = 
        (0x000000ffU & (__Vtask_test_blitter__DOT__setup_copy__294__dst_addr 
                        >> 0x10U));
    __Vtask_test_blitter__DOT__blt_reg__309__off = 0x0000000aU;
    __Vtask_test_blitter__DOT__write_reg__310__data 
        = __Vtask_test_blitter__DOT__blt_reg__309__data;
    __Vtask_test_blitter__DOT__write_reg__310__addr 
        = (0x0000ffffU & ((IData)(0xba83U) + __Vtask_test_blitter__DOT__blt_reg__309__off));
    Vtest_blitter___024root____VbeforeTrig_h603fc354__0(vlSelf, 
                                                        "@(posedge test_blitter.clk)");
    co_await vlSelfRef.__VtrigSched_h603fc354__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge test_blitter.clk)", 
                                                         "test_blitter.sv", 
                                                         113);
    vlSelfRef.test_blitter__DOT__cpu_addr = __Vtask_test_blitter__DOT__write_reg__310__addr;
    vlSelfRef.test_blitter__DOT__cpu_wdata = __Vtask_test_blitter__DOT__write_reg__310__data;
    vlSelfRef.test_blitter__DOT__cpu_we = 1U;
    Vtest_blitter___024root____VbeforeTrig_h603fc354__0(vlSelf, 
                                                        "@(posedge test_blitter.clk)");
    co_await vlSelfRef.__VtrigSched_h603fc354__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge test_blitter.clk)", 
                                                         "test_blitter.sv", 
                                                         115);
    vlSelfRef.test_blitter__DOT__cpu_we = 0U;
    __Vtask_test_blitter__DOT__blt_reg__311__data = 
        (0x000000ffU & __Vtask_test_blitter__DOT__setup_copy__294__width);
    __Vtask_test_blitter__DOT__blt_reg__311__off = 0x0000000bU;
    __Vtask_test_blitter__DOT__write_reg__312__data 
        = __Vtask_test_blitter__DOT__blt_reg__311__data;
    __Vtask_test_blitter__DOT__write_reg__312__addr 
        = (0x0000ffffU & ((IData)(0xba83U) + __Vtask_test_blitter__DOT__blt_reg__311__off));
    Vtest_blitter___024root____VbeforeTrig_h603fc354__0(vlSelf, 
                                                        "@(posedge test_blitter.clk)");
    co_await vlSelfRef.__VtrigSched_h603fc354__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge test_blitter.clk)", 
                                                         "test_blitter.sv", 
                                                         113);
    vlSelfRef.test_blitter__DOT__cpu_addr = __Vtask_test_blitter__DOT__write_reg__312__addr;
    vlSelfRef.test_blitter__DOT__cpu_wdata = __Vtask_test_blitter__DOT__write_reg__312__data;
    vlSelfRef.test_blitter__DOT__cpu_we = 1U;
    Vtest_blitter___024root____VbeforeTrig_h603fc354__0(vlSelf, 
                                                        "@(posedge test_blitter.clk)");
    co_await vlSelfRef.__VtrigSched_h603fc354__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge test_blitter.clk)", 
                                                         "test_blitter.sv", 
                                                         115);
    vlSelfRef.test_blitter__DOT__cpu_we = 0U;
    __Vtask_test_blitter__DOT__blt_reg__313__data = 
        (0x000000ffU & (__Vtask_test_blitter__DOT__setup_copy__294__width 
                        >> 8U));
    __Vtask_test_blitter__DOT__blt_reg__313__off = 0x0000000cU;
    __Vtask_test_blitter__DOT__write_reg__314__data 
        = __Vtask_test_blitter__DOT__blt_reg__313__data;
    __Vtask_test_blitter__DOT__write_reg__314__addr 
        = (0x0000ffffU & ((IData)(0xba83U) + __Vtask_test_blitter__DOT__blt_reg__313__off));
    Vtest_blitter___024root____VbeforeTrig_h603fc354__0(vlSelf, 
                                                        "@(posedge test_blitter.clk)");
    co_await vlSelfRef.__VtrigSched_h603fc354__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge test_blitter.clk)", 
                                                         "test_blitter.sv", 
                                                         113);
    vlSelfRef.test_blitter__DOT__cpu_addr = __Vtask_test_blitter__DOT__write_reg__314__addr;
    vlSelfRef.test_blitter__DOT__cpu_wdata = __Vtask_test_blitter__DOT__write_reg__314__data;
    vlSelfRef.test_blitter__DOT__cpu_we = 1U;
    Vtest_blitter___024root____VbeforeTrig_h603fc354__0(vlSelf, 
                                                        "@(posedge test_blitter.clk)");
    co_await vlSelfRef.__VtrigSched_h603fc354__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge test_blitter.clk)", 
                                                         "test_blitter.sv", 
                                                         115);
    vlSelfRef.test_blitter__DOT__cpu_we = 0U;
    __Vtask_test_blitter__DOT__blt_reg__315__data = 
        (0x000000ffU & __Vtask_test_blitter__DOT__setup_copy__294__height);
    __Vtask_test_blitter__DOT__blt_reg__315__off = 0x0000000dU;
    __Vtask_test_blitter__DOT__write_reg__316__data 
        = __Vtask_test_blitter__DOT__blt_reg__315__data;
    __Vtask_test_blitter__DOT__write_reg__316__addr 
        = (0x0000ffffU & ((IData)(0xba83U) + __Vtask_test_blitter__DOT__blt_reg__315__off));
    Vtest_blitter___024root____VbeforeTrig_h603fc354__0(vlSelf, 
                                                        "@(posedge test_blitter.clk)");
    co_await vlSelfRef.__VtrigSched_h603fc354__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge test_blitter.clk)", 
                                                         "test_blitter.sv", 
                                                         113);
    vlSelfRef.test_blitter__DOT__cpu_addr = __Vtask_test_blitter__DOT__write_reg__316__addr;
    vlSelfRef.test_blitter__DOT__cpu_wdata = __Vtask_test_blitter__DOT__write_reg__316__data;
    vlSelfRef.test_blitter__DOT__cpu_we = 1U;
    Vtest_blitter___024root____VbeforeTrig_h603fc354__0(vlSelf, 
                                                        "@(posedge test_blitter.clk)");
    co_await vlSelfRef.__VtrigSched_h603fc354__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge test_blitter.clk)", 
                                                         "test_blitter.sv", 
                                                         115);
    vlSelfRef.test_blitter__DOT__cpu_we = 0U;
    __Vtask_test_blitter__DOT__blt_reg__317__data = 
        (0x000000ffU & (__Vtask_test_blitter__DOT__setup_copy__294__height 
                        >> 8U));
    __Vtask_test_blitter__DOT__blt_reg__317__off = 0x0000000eU;
    __Vtask_test_blitter__DOT__write_reg__318__data 
        = __Vtask_test_blitter__DOT__blt_reg__317__data;
    __Vtask_test_blitter__DOT__write_reg__318__addr 
        = (0x0000ffffU & ((IData)(0xba83U) + __Vtask_test_blitter__DOT__blt_reg__317__off));
    Vtest_blitter___024root____VbeforeTrig_h603fc354__0(vlSelf, 
                                                        "@(posedge test_blitter.clk)");
    co_await vlSelfRef.__VtrigSched_h603fc354__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge test_blitter.clk)", 
                                                         "test_blitter.sv", 
                                                         113);
    vlSelfRef.test_blitter__DOT__cpu_addr = __Vtask_test_blitter__DOT__write_reg__318__addr;
    vlSelfRef.test_blitter__DOT__cpu_wdata = __Vtask_test_blitter__DOT__write_reg__318__data;
    vlSelfRef.test_blitter__DOT__cpu_we = 1U;
    Vtest_blitter___024root____VbeforeTrig_h603fc354__0(vlSelf, 
                                                        "@(posedge test_blitter.clk)");
    co_await vlSelfRef.__VtrigSched_h603fc354__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge test_blitter.clk)", 
                                                         "test_blitter.sv", 
                                                         115);
    vlSelfRef.test_blitter__DOT__cpu_we = 0U;
    __Vtask_test_blitter__DOT__blt_reg__319__data = 
        (0x000000ffU & __Vtask_test_blitter__DOT__setup_copy__294__src_stride);
    __Vtask_test_blitter__DOT__blt_reg__319__off = 0x0000000fU;
    __Vtask_test_blitter__DOT__write_reg__320__data 
        = __Vtask_test_blitter__DOT__blt_reg__319__data;
    __Vtask_test_blitter__DOT__write_reg__320__addr 
        = (0x0000ffffU & ((IData)(0xba83U) + __Vtask_test_blitter__DOT__blt_reg__319__off));
    Vtest_blitter___024root____VbeforeTrig_h603fc354__0(vlSelf, 
                                                        "@(posedge test_blitter.clk)");
    co_await vlSelfRef.__VtrigSched_h603fc354__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge test_blitter.clk)", 
                                                         "test_blitter.sv", 
                                                         113);
    vlSelfRef.test_blitter__DOT__cpu_addr = __Vtask_test_blitter__DOT__write_reg__320__addr;
    vlSelfRef.test_blitter__DOT__cpu_wdata = __Vtask_test_blitter__DOT__write_reg__320__data;
    vlSelfRef.test_blitter__DOT__cpu_we = 1U;
    Vtest_blitter___024root____VbeforeTrig_h603fc354__0(vlSelf, 
                                                        "@(posedge test_blitter.clk)");
    co_await vlSelfRef.__VtrigSched_h603fc354__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge test_blitter.clk)", 
                                                         "test_blitter.sv", 
                                                         115);
    vlSelfRef.test_blitter__DOT__cpu_we = 0U;
    __Vtask_test_blitter__DOT__blt_reg__321__data = 
        (0x000000ffU & (__Vtask_test_blitter__DOT__setup_copy__294__src_stride 
                        >> 8U));
    __Vtask_test_blitter__DOT__blt_reg__321__off = 0x00000010U;
    __Vtask_test_blitter__DOT__write_reg__322__data 
        = __Vtask_test_blitter__DOT__blt_reg__321__data;
    __Vtask_test_blitter__DOT__write_reg__322__addr 
        = (0x0000ffffU & ((IData)(0xba83U) + __Vtask_test_blitter__DOT__blt_reg__321__off));
    Vtest_blitter___024root____VbeforeTrig_h603fc354__0(vlSelf, 
                                                        "@(posedge test_blitter.clk)");
    co_await vlSelfRef.__VtrigSched_h603fc354__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge test_blitter.clk)", 
                                                         "test_blitter.sv", 
                                                         113);
    vlSelfRef.test_blitter__DOT__cpu_addr = __Vtask_test_blitter__DOT__write_reg__322__addr;
    vlSelfRef.test_blitter__DOT__cpu_wdata = __Vtask_test_blitter__DOT__write_reg__322__data;
    vlSelfRef.test_blitter__DOT__cpu_we = 1U;
    Vtest_blitter___024root____VbeforeTrig_h603fc354__0(vlSelf, 
                                                        "@(posedge test_blitter.clk)");
    co_await vlSelfRef.__VtrigSched_h603fc354__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge test_blitter.clk)", 
                                                         "test_blitter.sv", 
                                                         115);
    vlSelfRef.test_blitter__DOT__cpu_we = 0U;
    __Vtask_test_blitter__DOT__blt_reg__323__data = 
        (0x000000ffU & __Vtask_test_blitter__DOT__setup_copy__294__dst_stride);
    __Vtask_test_blitter__DOT__blt_reg__323__off = 0x00000011U;
    __Vtask_test_blitter__DOT__write_reg__324__data 
        = __Vtask_test_blitter__DOT__blt_reg__323__data;
    __Vtask_test_blitter__DOT__write_reg__324__addr 
        = (0x0000ffffU & ((IData)(0xba83U) + __Vtask_test_blitter__DOT__blt_reg__323__off));
    Vtest_blitter___024root____VbeforeTrig_h603fc354__0(vlSelf, 
                                                        "@(posedge test_blitter.clk)");
    co_await vlSelfRef.__VtrigSched_h603fc354__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge test_blitter.clk)", 
                                                         "test_blitter.sv", 
                                                         113);
    vlSelfRef.test_blitter__DOT__cpu_addr = __Vtask_test_blitter__DOT__write_reg__324__addr;
    vlSelfRef.test_blitter__DOT__cpu_wdata = __Vtask_test_blitter__DOT__write_reg__324__data;
    vlSelfRef.test_blitter__DOT__cpu_we = 1U;
    Vtest_blitter___024root____VbeforeTrig_h603fc354__0(vlSelf, 
                                                        "@(posedge test_blitter.clk)");
    co_await vlSelfRef.__VtrigSched_h603fc354__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge test_blitter.clk)", 
                                                         "test_blitter.sv", 
                                                         115);
    vlSelfRef.test_blitter__DOT__cpu_we = 0U;
    __Vtask_test_blitter__DOT__blt_reg__325__data = 
        (0x000000ffU & (__Vtask_test_blitter__DOT__setup_copy__294__dst_stride 
                        >> 8U));
    __Vtask_test_blitter__DOT__blt_reg__325__off = 0x00000012U;
    __Vtask_test_blitter__DOT__write_reg__326__data 
        = __Vtask_test_blitter__DOT__blt_reg__325__data;
    __Vtask_test_blitter__DOT__write_reg__326__addr 
        = (0x0000ffffU & ((IData)(0xba83U) + __Vtask_test_blitter__DOT__blt_reg__325__off));
    Vtest_blitter___024root____VbeforeTrig_h603fc354__0(vlSelf, 
                                                        "@(posedge test_blitter.clk)");
    co_await vlSelfRef.__VtrigSched_h603fc354__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge test_blitter.clk)", 
                                                         "test_blitter.sv", 
                                                         113);
    vlSelfRef.test_blitter__DOT__cpu_addr = __Vtask_test_blitter__DOT__write_reg__326__addr;
    vlSelfRef.test_blitter__DOT__cpu_wdata = __Vtask_test_blitter__DOT__write_reg__326__data;
    vlSelfRef.test_blitter__DOT__cpu_we = 1U;
    Vtest_blitter___024root____VbeforeTrig_h603fc354__0(vlSelf, 
                                                        "@(posedge test_blitter.clk)");
    co_await vlSelfRef.__VtrigSched_h603fc354__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge test_blitter.clk)", 
                                                         "test_blitter.sv", 
                                                         115);
    vlSelfRef.test_blitter__DOT__cpu_we = 0U;
    __Vtask_test_blitter__DOT__blt_reg__327__data = 0U;
    __Vtask_test_blitter__DOT__blt_reg__327__off = 0x00000013U;
    __Vtask_test_blitter__DOT__write_reg__328__data 
        = __Vtask_test_blitter__DOT__blt_reg__327__data;
    __Vtask_test_blitter__DOT__write_reg__328__addr 
        = (0x0000ffffU & ((IData)(0xba83U) + __Vtask_test_blitter__DOT__blt_reg__327__off));
    Vtest_blitter___024root____VbeforeTrig_h603fc354__0(vlSelf, 
                                                        "@(posedge test_blitter.clk)");
    co_await vlSelfRef.__VtrigSched_h603fc354__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge test_blitter.clk)", 
                                                         "test_blitter.sv", 
                                                         113);
    vlSelfRef.test_blitter__DOT__cpu_addr = __Vtask_test_blitter__DOT__write_reg__328__addr;
    vlSelfRef.test_blitter__DOT__cpu_wdata = __Vtask_test_blitter__DOT__write_reg__328__data;
    vlSelfRef.test_blitter__DOT__cpu_we = 1U;
    Vtest_blitter___024root____VbeforeTrig_h603fc354__0(vlSelf, 
                                                        "@(posedge test_blitter.clk)");
    co_await vlSelfRef.__VtrigSched_h603fc354__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge test_blitter.clk)", 
                                                         "test_blitter.sv", 
                                                         115);
    vlSelfRef.test_blitter__DOT__cpu_we = 0U;
    __Vtask_test_blitter__DOT__blt_reg__330__data = 1U;
    __Vtask_test_blitter__DOT__blt_reg__330__off = 0U;
    __Vtask_test_blitter__DOT__write_reg__331__data 
        = __Vtask_test_blitter__DOT__blt_reg__330__data;
    __Vtask_test_blitter__DOT__write_reg__331__addr 
        = (0x0000ffffU & ((IData)(0xba83U) + __Vtask_test_blitter__DOT__blt_reg__330__off));
    Vtest_blitter___024root____VbeforeTrig_h603fc354__0(vlSelf, 
                                                        "@(posedge test_blitter.clk)");
    co_await vlSelfRef.__VtrigSched_h603fc354__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge test_blitter.clk)", 
                                                         "test_blitter.sv", 
                                                         113);
    vlSelfRef.test_blitter__DOT__cpu_addr = __Vtask_test_blitter__DOT__write_reg__331__addr;
    vlSelfRef.test_blitter__DOT__cpu_wdata = __Vtask_test_blitter__DOT__write_reg__331__data;
    vlSelfRef.test_blitter__DOT__cpu_we = 1U;
    Vtest_blitter___024root____VbeforeTrig_h603fc354__0(vlSelf, 
                                                        "@(posedge test_blitter.clk)");
    co_await vlSelfRef.__VtrigSched_h603fc354__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge test_blitter.clk)", 
                                                         "test_blitter.sv", 
                                                         115);
    vlSelfRef.test_blitter__DOT__cpu_we = 0U;
    __Vtask_test_blitter__DOT__wait_blt_done__332__test_blitter__DOT__unnamedblk1_1__DOT____Vrepeat0 = 0;
    __Vtask_test_blitter__DOT__wait_blt_done__332__test_blitter__DOT__unnamedblk1_2__DOT____Vrepeat1 = 0;
    __Vtask_test_blitter__DOT__wait_blt_done__332__test_blitter__DOT__unnamedblk1_1__DOT____Vrepeat0 = 2U;
    while (VL_LTS_III(32, 0U, __Vtask_test_blitter__DOT__wait_blt_done__332__test_blitter__DOT__unnamedblk1_1__DOT____Vrepeat0)) {
        Vtest_blitter___024root____VbeforeTrig_h603fc354__0(vlSelf, 
                                                            "@(posedge test_blitter.clk)");
        co_await vlSelfRef.__VtrigSched_h603fc354__0.trigger(0U, 
                                                             nullptr, 
                                                             "@(posedge test_blitter.clk)", 
                                                             "test_blitter.sv", 
                                                             128);
        __Vtask_test_blitter__DOT__wait_blt_done__332__test_blitter__DOT__unnamedblk1_1__DOT____Vrepeat0 
            = (__Vtask_test_blitter__DOT__wait_blt_done__332__test_blitter__DOT__unnamedblk1_1__DOT____Vrepeat0 
               - (IData)(1U));
    }
    while (((0U != (IData)(vlSelfRef.test_blitter__DOT__dut__DOT__state)) 
            & (6U != (IData)(vlSelfRef.test_blitter__DOT__dut__DOT__state)))) {
        Vtest_blitter___024root____VbeforeTrig_h603fc354__0(vlSelf, 
                                                            "@(posedge test_blitter.clk)");
        co_await vlSelfRef.__VtrigSched_h603fc354__0.trigger(0U, 
                                                             nullptr, 
                                                             "@(posedge test_blitter.clk)", 
                                                             "test_blitter.sv", 
                                                             130);
    }
    __Vtask_test_blitter__DOT__wait_blt_done__332__test_blitter__DOT__unnamedblk1_2__DOT____Vrepeat1 = 2U;
    while (VL_LTS_III(32, 0U, __Vtask_test_blitter__DOT__wait_blt_done__332__test_blitter__DOT__unnamedblk1_2__DOT____Vrepeat1)) {
        Vtest_blitter___024root____VbeforeTrig_h603fc354__0(vlSelf, 
                                                            "@(posedge test_blitter.clk)");
        co_await vlSelfRef.__VtrigSched_h603fc354__0.trigger(0U, 
                                                             nullptr, 
                                                             "@(posedge test_blitter.clk)", 
                                                             "test_blitter.sv", 
                                                             131);
        __Vtask_test_blitter__DOT__wait_blt_done__332__test_blitter__DOT__unnamedblk1_2__DOT____Vrepeat1 
            = (__Vtask_test_blitter__DOT__wait_blt_done__332__test_blitter__DOT__unnamedblk1_2__DOT____Vrepeat1 
               - (IData)(1U));
    }
    __Vtask_test_blitter__DOT__check__333__condition 
        = (0xa1U == vlSelfRef.test_blitter__DOT__sim_ram[28928U]);
    vlSelfRef.__Vtask_test_blitter__DOT__check__333__name = "stride dst[0] = A1"s;
    vlSelfRef.test_blitter__DOT__test_num = ((IData)(1U) 
                                             + vlSelfRef.test_blitter__DOT__test_num);
    if (__Vtask_test_blitter__DOT__check__333__condition) {
        VL_WRITEF_NX("  PASS [%0d] %@\n",0,32,vlSelfRef.test_blitter__DOT__test_num,
                     -1,&(vlSelfRef.__Vtask_test_blitter__DOT__check__333__name));
        vlSelfRef.test_blitter__DOT__pass_count = ((IData)(1U) 
                                                   + vlSelfRef.test_blitter__DOT__pass_count);
    } else {
        VL_WRITEF_NX("  FAIL [%0d] %@\n",0,32,vlSelfRef.test_blitter__DOT__test_num,
                     -1,&(vlSelfRef.__Vtask_test_blitter__DOT__check__333__name));
        vlSelfRef.test_blitter__DOT__fail_count = ((IData)(1U) 
                                                   + vlSelfRef.test_blitter__DOT__fail_count);
    }
    __Vtask_test_blitter__DOT__check__334__condition 
        = (0xa2U == vlSelfRef.test_blitter__DOT__sim_ram[28929U]);
    vlSelfRef.__Vtask_test_blitter__DOT__check__334__name = "stride dst[1] = A2"s;
    vlSelfRef.test_blitter__DOT__test_num = ((IData)(1U) 
                                             + vlSelfRef.test_blitter__DOT__test_num);
    if (__Vtask_test_blitter__DOT__check__334__condition) {
        VL_WRITEF_NX("  PASS [%0d] %@\n",0,32,vlSelfRef.test_blitter__DOT__test_num,
                     -1,&(vlSelfRef.__Vtask_test_blitter__DOT__check__334__name));
        vlSelfRef.test_blitter__DOT__pass_count = ((IData)(1U) 
                                                   + vlSelfRef.test_blitter__DOT__pass_count);
    } else {
        VL_WRITEF_NX("  FAIL [%0d] %@\n",0,32,vlSelfRef.test_blitter__DOT__test_num,
                     -1,&(vlSelfRef.__Vtask_test_blitter__DOT__check__334__name));
        vlSelfRef.test_blitter__DOT__fail_count = ((IData)(1U) 
                                                   + vlSelfRef.test_blitter__DOT__fail_count);
    }
    __Vtask_test_blitter__DOT__check__335__condition 
        = (0xa3U == vlSelfRef.test_blitter__DOT__sim_ram[28930U]);
    vlSelfRef.__Vtask_test_blitter__DOT__check__335__name = "stride dst[2] = A3"s;
    vlSelfRef.test_blitter__DOT__test_num = ((IData)(1U) 
                                             + vlSelfRef.test_blitter__DOT__test_num);
    if (__Vtask_test_blitter__DOT__check__335__condition) {
        VL_WRITEF_NX("  PASS [%0d] %@\n",0,32,vlSelfRef.test_blitter__DOT__test_num,
                     -1,&(vlSelfRef.__Vtask_test_blitter__DOT__check__335__name));
        vlSelfRef.test_blitter__DOT__pass_count = ((IData)(1U) 
                                                   + vlSelfRef.test_blitter__DOT__pass_count);
    } else {
        VL_WRITEF_NX("  FAIL [%0d] %@\n",0,32,vlSelfRef.test_blitter__DOT__test_num,
                     -1,&(vlSelfRef.__Vtask_test_blitter__DOT__check__335__name));
        vlSelfRef.test_blitter__DOT__fail_count = ((IData)(1U) 
                                                   + vlSelfRef.test_blitter__DOT__fail_count);
    }
    __Vtask_test_blitter__DOT__check__336__condition 
        = (0xb1U == vlSelfRef.test_blitter__DOT__sim_ram[28931U]);
    vlSelfRef.__Vtask_test_blitter__DOT__check__336__name = "stride dst[3] = B1"s;
    vlSelfRef.test_blitter__DOT__test_num = ((IData)(1U) 
                                             + vlSelfRef.test_blitter__DOT__test_num);
    if (__Vtask_test_blitter__DOT__check__336__condition) {
        VL_WRITEF_NX("  PASS [%0d] %@\n",0,32,vlSelfRef.test_blitter__DOT__test_num,
                     -1,&(vlSelfRef.__Vtask_test_blitter__DOT__check__336__name));
        vlSelfRef.test_blitter__DOT__pass_count = ((IData)(1U) 
                                                   + vlSelfRef.test_blitter__DOT__pass_count);
    } else {
        VL_WRITEF_NX("  FAIL [%0d] %@\n",0,32,vlSelfRef.test_blitter__DOT__test_num,
                     -1,&(vlSelfRef.__Vtask_test_blitter__DOT__check__336__name));
        vlSelfRef.test_blitter__DOT__fail_count = ((IData)(1U) 
                                                   + vlSelfRef.test_blitter__DOT__fail_count);
    }
    __Vtask_test_blitter__DOT__check__337__condition 
        = (0xb2U == vlSelfRef.test_blitter__DOT__sim_ram[28932U]);
    vlSelfRef.__Vtask_test_blitter__DOT__check__337__name = "stride dst[4] = B2"s;
    vlSelfRef.test_blitter__DOT__test_num = ((IData)(1U) 
                                             + vlSelfRef.test_blitter__DOT__test_num);
    if (__Vtask_test_blitter__DOT__check__337__condition) {
        VL_WRITEF_NX("  PASS [%0d] %@\n",0,32,vlSelfRef.test_blitter__DOT__test_num,
                     -1,&(vlSelfRef.__Vtask_test_blitter__DOT__check__337__name));
        vlSelfRef.test_blitter__DOT__pass_count = ((IData)(1U) 
                                                   + vlSelfRef.test_blitter__DOT__pass_count);
    } else {
        VL_WRITEF_NX("  FAIL [%0d] %@\n",0,32,vlSelfRef.test_blitter__DOT__test_num,
                     -1,&(vlSelfRef.__Vtask_test_blitter__DOT__check__337__name));
        vlSelfRef.test_blitter__DOT__fail_count = ((IData)(1U) 
                                                   + vlSelfRef.test_blitter__DOT__fail_count);
    }
    __Vtask_test_blitter__DOT__check__338__condition 
        = (0xb3U == vlSelfRef.test_blitter__DOT__sim_ram[28933U]);
    vlSelfRef.__Vtask_test_blitter__DOT__check__338__name = "stride dst[5] = B3"s;
    vlSelfRef.test_blitter__DOT__test_num = ((IData)(1U) 
                                             + vlSelfRef.test_blitter__DOT__test_num);
    if (__Vtask_test_blitter__DOT__check__338__condition) {
        VL_WRITEF_NX("  PASS [%0d] %@\n",0,32,vlSelfRef.test_blitter__DOT__test_num,
                     -1,&(vlSelfRef.__Vtask_test_blitter__DOT__check__338__name));
        vlSelfRef.test_blitter__DOT__pass_count = ((IData)(1U) 
                                                   + vlSelfRef.test_blitter__DOT__pass_count);
    } else {
        VL_WRITEF_NX("  FAIL [%0d] %@\n",0,32,vlSelfRef.test_blitter__DOT__test_num,
                     -1,&(vlSelfRef.__Vtask_test_blitter__DOT__check__338__name));
        vlSelfRef.test_blitter__DOT__fail_count = ((IData)(1U) 
                                                   + vlSelfRef.test_blitter__DOT__fail_count);
    }
    VL_WRITEF_NX("Test: Fill VGC gfx\n",0);
    __Vtask_test_blitter__DOT__setup_fill__339__fill_val = 5U;
    __Vtask_test_blitter__DOT__setup_fill__339__height = 1U;
    __Vtask_test_blitter__DOT__setup_fill__339__width = 0x00000140U;
    __Vtask_test_blitter__DOT__setup_fill__339__dst_stride = 0x00000140U;
    __Vtask_test_blitter__DOT__setup_fill__339__dst_addr = 0U;
    __Vtask_test_blitter__DOT__setup_fill__339__dst_space = 3U;
    __Vtask_test_blitter__DOT__blt_reg__340__data = 
        (0x000000ffU & __Vtask_test_blitter__DOT__setup_fill__339__dst_space);
    __Vtask_test_blitter__DOT__blt_reg__340__off = 4U;
    __Vtask_test_blitter__DOT__write_reg__341__data 
        = __Vtask_test_blitter__DOT__blt_reg__340__data;
    __Vtask_test_blitter__DOT__write_reg__341__addr 
        = (0x0000ffffU & ((IData)(0xba83U) + __Vtask_test_blitter__DOT__blt_reg__340__off));
    Vtest_blitter___024root____VbeforeTrig_h603fc354__0(vlSelf, 
                                                        "@(posedge test_blitter.clk)");
    co_await vlSelfRef.__VtrigSched_h603fc354__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge test_blitter.clk)", 
                                                         "test_blitter.sv", 
                                                         113);
    vlSelfRef.test_blitter__DOT__cpu_addr = __Vtask_test_blitter__DOT__write_reg__341__addr;
    vlSelfRef.test_blitter__DOT__cpu_wdata = __Vtask_test_blitter__DOT__write_reg__341__data;
    vlSelfRef.test_blitter__DOT__cpu_we = 1U;
    Vtest_blitter___024root____VbeforeTrig_h603fc354__0(vlSelf, 
                                                        "@(posedge test_blitter.clk)");
    co_await vlSelfRef.__VtrigSched_h603fc354__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge test_blitter.clk)", 
                                                         "test_blitter.sv", 
                                                         115);
    vlSelfRef.test_blitter__DOT__cpu_we = 0U;
    __Vtask_test_blitter__DOT__blt_reg__342__data = 
        (0x000000ffU & __Vtask_test_blitter__DOT__setup_fill__339__dst_addr);
    __Vtask_test_blitter__DOT__blt_reg__342__off = 8U;
    __Vtask_test_blitter__DOT__write_reg__343__data 
        = __Vtask_test_blitter__DOT__blt_reg__342__data;
    __Vtask_test_blitter__DOT__write_reg__343__addr 
        = (0x0000ffffU & ((IData)(0xba83U) + __Vtask_test_blitter__DOT__blt_reg__342__off));
    Vtest_blitter___024root____VbeforeTrig_h603fc354__0(vlSelf, 
                                                        "@(posedge test_blitter.clk)");
    co_await vlSelfRef.__VtrigSched_h603fc354__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge test_blitter.clk)", 
                                                         "test_blitter.sv", 
                                                         113);
    vlSelfRef.test_blitter__DOT__cpu_addr = __Vtask_test_blitter__DOT__write_reg__343__addr;
    vlSelfRef.test_blitter__DOT__cpu_wdata = __Vtask_test_blitter__DOT__write_reg__343__data;
    vlSelfRef.test_blitter__DOT__cpu_we = 1U;
    Vtest_blitter___024root____VbeforeTrig_h603fc354__0(vlSelf, 
                                                        "@(posedge test_blitter.clk)");
    co_await vlSelfRef.__VtrigSched_h603fc354__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge test_blitter.clk)", 
                                                         "test_blitter.sv", 
                                                         115);
    vlSelfRef.test_blitter__DOT__cpu_we = 0U;
    __Vtask_test_blitter__DOT__blt_reg__344__data = 
        (0x000000ffU & (__Vtask_test_blitter__DOT__setup_fill__339__dst_addr 
                        >> 8U));
    __Vtask_test_blitter__DOT__blt_reg__344__off = 9U;
    __Vtask_test_blitter__DOT__write_reg__345__data 
        = __Vtask_test_blitter__DOT__blt_reg__344__data;
    __Vtask_test_blitter__DOT__write_reg__345__addr 
        = (0x0000ffffU & ((IData)(0xba83U) + __Vtask_test_blitter__DOT__blt_reg__344__off));
    Vtest_blitter___024root____VbeforeTrig_h603fc354__0(vlSelf, 
                                                        "@(posedge test_blitter.clk)");
    co_await vlSelfRef.__VtrigSched_h603fc354__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge test_blitter.clk)", 
                                                         "test_blitter.sv", 
                                                         113);
    vlSelfRef.test_blitter__DOT__cpu_addr = __Vtask_test_blitter__DOT__write_reg__345__addr;
    vlSelfRef.test_blitter__DOT__cpu_wdata = __Vtask_test_blitter__DOT__write_reg__345__data;
    vlSelfRef.test_blitter__DOT__cpu_we = 1U;
    Vtest_blitter___024root____VbeforeTrig_h603fc354__0(vlSelf, 
                                                        "@(posedge test_blitter.clk)");
    co_await vlSelfRef.__VtrigSched_h603fc354__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge test_blitter.clk)", 
                                                         "test_blitter.sv", 
                                                         115);
    vlSelfRef.test_blitter__DOT__cpu_we = 0U;
    __Vtask_test_blitter__DOT__blt_reg__346__data = 
        (0x000000ffU & (__Vtask_test_blitter__DOT__setup_fill__339__dst_addr 
                        >> 0x10U));
    __Vtask_test_blitter__DOT__blt_reg__346__off = 0x0000000aU;
    __Vtask_test_blitter__DOT__write_reg__347__data 
        = __Vtask_test_blitter__DOT__blt_reg__346__data;
    __Vtask_test_blitter__DOT__write_reg__347__addr 
        = (0x0000ffffU & ((IData)(0xba83U) + __Vtask_test_blitter__DOT__blt_reg__346__off));
    Vtest_blitter___024root____VbeforeTrig_h603fc354__0(vlSelf, 
                                                        "@(posedge test_blitter.clk)");
    co_await vlSelfRef.__VtrigSched_h603fc354__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge test_blitter.clk)", 
                                                         "test_blitter.sv", 
                                                         113);
    vlSelfRef.test_blitter__DOT__cpu_addr = __Vtask_test_blitter__DOT__write_reg__347__addr;
    vlSelfRef.test_blitter__DOT__cpu_wdata = __Vtask_test_blitter__DOT__write_reg__347__data;
    vlSelfRef.test_blitter__DOT__cpu_we = 1U;
    Vtest_blitter___024root____VbeforeTrig_h603fc354__0(vlSelf, 
                                                        "@(posedge test_blitter.clk)");
    co_await vlSelfRef.__VtrigSched_h603fc354__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge test_blitter.clk)", 
                                                         "test_blitter.sv", 
                                                         115);
    vlSelfRef.test_blitter__DOT__cpu_we = 0U;
    __Vtask_test_blitter__DOT__blt_reg__348__data = 
        (0x000000ffU & __Vtask_test_blitter__DOT__setup_fill__339__width);
    __Vtask_test_blitter__DOT__blt_reg__348__off = 0x0000000bU;
    __Vtask_test_blitter__DOT__write_reg__349__data 
        = __Vtask_test_blitter__DOT__blt_reg__348__data;
    __Vtask_test_blitter__DOT__write_reg__349__addr 
        = (0x0000ffffU & ((IData)(0xba83U) + __Vtask_test_blitter__DOT__blt_reg__348__off));
    Vtest_blitter___024root____VbeforeTrig_h603fc354__0(vlSelf, 
                                                        "@(posedge test_blitter.clk)");
    co_await vlSelfRef.__VtrigSched_h603fc354__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge test_blitter.clk)", 
                                                         "test_blitter.sv", 
                                                         113);
    vlSelfRef.test_blitter__DOT__cpu_addr = __Vtask_test_blitter__DOT__write_reg__349__addr;
    vlSelfRef.test_blitter__DOT__cpu_wdata = __Vtask_test_blitter__DOT__write_reg__349__data;
    vlSelfRef.test_blitter__DOT__cpu_we = 1U;
    Vtest_blitter___024root____VbeforeTrig_h603fc354__0(vlSelf, 
                                                        "@(posedge test_blitter.clk)");
    co_await vlSelfRef.__VtrigSched_h603fc354__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge test_blitter.clk)", 
                                                         "test_blitter.sv", 
                                                         115);
    vlSelfRef.test_blitter__DOT__cpu_we = 0U;
    __Vtask_test_blitter__DOT__blt_reg__350__data = 
        (0x000000ffU & (__Vtask_test_blitter__DOT__setup_fill__339__width 
                        >> 8U));
    __Vtask_test_blitter__DOT__blt_reg__350__off = 0x0000000cU;
    __Vtask_test_blitter__DOT__write_reg__351__data 
        = __Vtask_test_blitter__DOT__blt_reg__350__data;
    __Vtask_test_blitter__DOT__write_reg__351__addr 
        = (0x0000ffffU & ((IData)(0xba83U) + __Vtask_test_blitter__DOT__blt_reg__350__off));
    Vtest_blitter___024root____VbeforeTrig_h603fc354__0(vlSelf, 
                                                        "@(posedge test_blitter.clk)");
    co_await vlSelfRef.__VtrigSched_h603fc354__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge test_blitter.clk)", 
                                                         "test_blitter.sv", 
                                                         113);
    vlSelfRef.test_blitter__DOT__cpu_addr = __Vtask_test_blitter__DOT__write_reg__351__addr;
    vlSelfRef.test_blitter__DOT__cpu_wdata = __Vtask_test_blitter__DOT__write_reg__351__data;
    vlSelfRef.test_blitter__DOT__cpu_we = 1U;
    Vtest_blitter___024root____VbeforeTrig_h603fc354__0(vlSelf, 
                                                        "@(posedge test_blitter.clk)");
    co_await vlSelfRef.__VtrigSched_h603fc354__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge test_blitter.clk)", 
                                                         "test_blitter.sv", 
                                                         115);
    vlSelfRef.test_blitter__DOT__cpu_we = 0U;
    __Vtask_test_blitter__DOT__blt_reg__352__data = 
        (0x000000ffU & __Vtask_test_blitter__DOT__setup_fill__339__height);
    __Vtask_test_blitter__DOT__blt_reg__352__off = 0x0000000dU;
    __Vtask_test_blitter__DOT__write_reg__353__data 
        = __Vtask_test_blitter__DOT__blt_reg__352__data;
    __Vtask_test_blitter__DOT__write_reg__353__addr 
        = (0x0000ffffU & ((IData)(0xba83U) + __Vtask_test_blitter__DOT__blt_reg__352__off));
    Vtest_blitter___024root____VbeforeTrig_h603fc354__0(vlSelf, 
                                                        "@(posedge test_blitter.clk)");
    co_await vlSelfRef.__VtrigSched_h603fc354__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge test_blitter.clk)", 
                                                         "test_blitter.sv", 
                                                         113);
    vlSelfRef.test_blitter__DOT__cpu_addr = __Vtask_test_blitter__DOT__write_reg__353__addr;
    vlSelfRef.test_blitter__DOT__cpu_wdata = __Vtask_test_blitter__DOT__write_reg__353__data;
    vlSelfRef.test_blitter__DOT__cpu_we = 1U;
    Vtest_blitter___024root____VbeforeTrig_h603fc354__0(vlSelf, 
                                                        "@(posedge test_blitter.clk)");
    co_await vlSelfRef.__VtrigSched_h603fc354__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge test_blitter.clk)", 
                                                         "test_blitter.sv", 
                                                         115);
    vlSelfRef.test_blitter__DOT__cpu_we = 0U;
    __Vtask_test_blitter__DOT__blt_reg__354__data = 
        (0x000000ffU & (__Vtask_test_blitter__DOT__setup_fill__339__height 
                        >> 8U));
    __Vtask_test_blitter__DOT__blt_reg__354__off = 0x0000000eU;
    __Vtask_test_blitter__DOT__write_reg__355__data 
        = __Vtask_test_blitter__DOT__blt_reg__354__data;
    __Vtask_test_blitter__DOT__write_reg__355__addr 
        = (0x0000ffffU & ((IData)(0xba83U) + __Vtask_test_blitter__DOT__blt_reg__354__off));
    Vtest_blitter___024root____VbeforeTrig_h603fc354__0(vlSelf, 
                                                        "@(posedge test_blitter.clk)");
    co_await vlSelfRef.__VtrigSched_h603fc354__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge test_blitter.clk)", 
                                                         "test_blitter.sv", 
                                                         113);
    vlSelfRef.test_blitter__DOT__cpu_addr = __Vtask_test_blitter__DOT__write_reg__355__addr;
    vlSelfRef.test_blitter__DOT__cpu_wdata = __Vtask_test_blitter__DOT__write_reg__355__data;
    vlSelfRef.test_blitter__DOT__cpu_we = 1U;
    Vtest_blitter___024root____VbeforeTrig_h603fc354__0(vlSelf, 
                                                        "@(posedge test_blitter.clk)");
    co_await vlSelfRef.__VtrigSched_h603fc354__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge test_blitter.clk)", 
                                                         "test_blitter.sv", 
                                                         115);
    vlSelfRef.test_blitter__DOT__cpu_we = 0U;
    __Vtask_test_blitter__DOT__blt_reg__356__data = 
        (0x000000ffU & __Vtask_test_blitter__DOT__setup_fill__339__dst_stride);
    __Vtask_test_blitter__DOT__blt_reg__356__off = 0x00000011U;
    __Vtask_test_blitter__DOT__write_reg__357__data 
        = __Vtask_test_blitter__DOT__blt_reg__356__data;
    __Vtask_test_blitter__DOT__write_reg__357__addr 
        = (0x0000ffffU & ((IData)(0xba83U) + __Vtask_test_blitter__DOT__blt_reg__356__off));
    Vtest_blitter___024root____VbeforeTrig_h603fc354__0(vlSelf, 
                                                        "@(posedge test_blitter.clk)");
    co_await vlSelfRef.__VtrigSched_h603fc354__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge test_blitter.clk)", 
                                                         "test_blitter.sv", 
                                                         113);
    vlSelfRef.test_blitter__DOT__cpu_addr = __Vtask_test_blitter__DOT__write_reg__357__addr;
    vlSelfRef.test_blitter__DOT__cpu_wdata = __Vtask_test_blitter__DOT__write_reg__357__data;
    vlSelfRef.test_blitter__DOT__cpu_we = 1U;
    Vtest_blitter___024root____VbeforeTrig_h603fc354__0(vlSelf, 
                                                        "@(posedge test_blitter.clk)");
    co_await vlSelfRef.__VtrigSched_h603fc354__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge test_blitter.clk)", 
                                                         "test_blitter.sv", 
                                                         115);
    vlSelfRef.test_blitter__DOT__cpu_we = 0U;
    __Vtask_test_blitter__DOT__blt_reg__358__data = 
        (0x000000ffU & (__Vtask_test_blitter__DOT__setup_fill__339__dst_stride 
                        >> 8U));
    __Vtask_test_blitter__DOT__blt_reg__358__off = 0x00000012U;
    __Vtask_test_blitter__DOT__write_reg__359__data 
        = __Vtask_test_blitter__DOT__blt_reg__358__data;
    __Vtask_test_blitter__DOT__write_reg__359__addr 
        = (0x0000ffffU & ((IData)(0xba83U) + __Vtask_test_blitter__DOT__blt_reg__358__off));
    Vtest_blitter___024root____VbeforeTrig_h603fc354__0(vlSelf, 
                                                        "@(posedge test_blitter.clk)");
    co_await vlSelfRef.__VtrigSched_h603fc354__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge test_blitter.clk)", 
                                                         "test_blitter.sv", 
                                                         113);
    vlSelfRef.test_blitter__DOT__cpu_addr = __Vtask_test_blitter__DOT__write_reg__359__addr;
    vlSelfRef.test_blitter__DOT__cpu_wdata = __Vtask_test_blitter__DOT__write_reg__359__data;
    vlSelfRef.test_blitter__DOT__cpu_we = 1U;
    Vtest_blitter___024root____VbeforeTrig_h603fc354__0(vlSelf, 
                                                        "@(posedge test_blitter.clk)");
    co_await vlSelfRef.__VtrigSched_h603fc354__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge test_blitter.clk)", 
                                                         "test_blitter.sv", 
                                                         115);
    vlSelfRef.test_blitter__DOT__cpu_we = 0U;
    __Vtask_test_blitter__DOT__blt_reg__360__data = 1U;
    __Vtask_test_blitter__DOT__blt_reg__360__off = 0x00000013U;
    __Vtask_test_blitter__DOT__write_reg__361__data 
        = __Vtask_test_blitter__DOT__blt_reg__360__data;
    __Vtask_test_blitter__DOT__write_reg__361__addr 
        = (0x0000ffffU & ((IData)(0xba83U) + __Vtask_test_blitter__DOT__blt_reg__360__off));
    Vtest_blitter___024root____VbeforeTrig_h603fc354__0(vlSelf, 
                                                        "@(posedge test_blitter.clk)");
    co_await vlSelfRef.__VtrigSched_h603fc354__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge test_blitter.clk)", 
                                                         "test_blitter.sv", 
                                                         113);
    vlSelfRef.test_blitter__DOT__cpu_addr = __Vtask_test_blitter__DOT__write_reg__361__addr;
    vlSelfRef.test_blitter__DOT__cpu_wdata = __Vtask_test_blitter__DOT__write_reg__361__data;
    vlSelfRef.test_blitter__DOT__cpu_we = 1U;
    Vtest_blitter___024root____VbeforeTrig_h603fc354__0(vlSelf, 
                                                        "@(posedge test_blitter.clk)");
    co_await vlSelfRef.__VtrigSched_h603fc354__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge test_blitter.clk)", 
                                                         "test_blitter.sv", 
                                                         115);
    vlSelfRef.test_blitter__DOT__cpu_we = 0U;
    __Vtask_test_blitter__DOT__blt_reg__362__data = 
        (0x000000ffU & __Vtask_test_blitter__DOT__setup_fill__339__fill_val);
    __Vtask_test_blitter__DOT__blt_reg__362__off = 0x00000014U;
    __Vtask_test_blitter__DOT__write_reg__363__data 
        = __Vtask_test_blitter__DOT__blt_reg__362__data;
    __Vtask_test_blitter__DOT__write_reg__363__addr 
        = (0x0000ffffU & ((IData)(0xba83U) + __Vtask_test_blitter__DOT__blt_reg__362__off));
    Vtest_blitter___024root____VbeforeTrig_h603fc354__0(vlSelf, 
                                                        "@(posedge test_blitter.clk)");
    co_await vlSelfRef.__VtrigSched_h603fc354__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge test_blitter.clk)", 
                                                         "test_blitter.sv", 
                                                         113);
    vlSelfRef.test_blitter__DOT__cpu_addr = __Vtask_test_blitter__DOT__write_reg__363__addr;
    vlSelfRef.test_blitter__DOT__cpu_wdata = __Vtask_test_blitter__DOT__write_reg__363__data;
    vlSelfRef.test_blitter__DOT__cpu_we = 1U;
    Vtest_blitter___024root____VbeforeTrig_h603fc354__0(vlSelf, 
                                                        "@(posedge test_blitter.clk)");
    co_await vlSelfRef.__VtrigSched_h603fc354__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge test_blitter.clk)", 
                                                         "test_blitter.sv", 
                                                         115);
    vlSelfRef.test_blitter__DOT__cpu_we = 0U;
    __Vtask_test_blitter__DOT__blt_reg__365__data = 1U;
    __Vtask_test_blitter__DOT__blt_reg__365__off = 0U;
    __Vtask_test_blitter__DOT__write_reg__366__data 
        = __Vtask_test_blitter__DOT__blt_reg__365__data;
    __Vtask_test_blitter__DOT__write_reg__366__addr 
        = (0x0000ffffU & ((IData)(0xba83U) + __Vtask_test_blitter__DOT__blt_reg__365__off));
    Vtest_blitter___024root____VbeforeTrig_h603fc354__0(vlSelf, 
                                                        "@(posedge test_blitter.clk)");
    co_await vlSelfRef.__VtrigSched_h603fc354__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge test_blitter.clk)", 
                                                         "test_blitter.sv", 
                                                         113);
    vlSelfRef.test_blitter__DOT__cpu_addr = __Vtask_test_blitter__DOT__write_reg__366__addr;
    vlSelfRef.test_blitter__DOT__cpu_wdata = __Vtask_test_blitter__DOT__write_reg__366__data;
    vlSelfRef.test_blitter__DOT__cpu_we = 1U;
    Vtest_blitter___024root____VbeforeTrig_h603fc354__0(vlSelf, 
                                                        "@(posedge test_blitter.clk)");
    co_await vlSelfRef.__VtrigSched_h603fc354__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge test_blitter.clk)", 
                                                         "test_blitter.sv", 
                                                         115);
    vlSelfRef.test_blitter__DOT__cpu_we = 0U;
    __Vtask_test_blitter__DOT__wait_blt_done__367__test_blitter__DOT__unnamedblk1_1__DOT____Vrepeat0 = 0;
    __Vtask_test_blitter__DOT__wait_blt_done__367__test_blitter__DOT__unnamedblk1_2__DOT____Vrepeat1 = 0;
    __Vtask_test_blitter__DOT__wait_blt_done__367__test_blitter__DOT__unnamedblk1_1__DOT____Vrepeat0 = 2U;
    while (VL_LTS_III(32, 0U, __Vtask_test_blitter__DOT__wait_blt_done__367__test_blitter__DOT__unnamedblk1_1__DOT____Vrepeat0)) {
        Vtest_blitter___024root____VbeforeTrig_h603fc354__0(vlSelf, 
                                                            "@(posedge test_blitter.clk)");
        co_await vlSelfRef.__VtrigSched_h603fc354__0.trigger(0U, 
                                                             nullptr, 
                                                             "@(posedge test_blitter.clk)", 
                                                             "test_blitter.sv", 
                                                             128);
        __Vtask_test_blitter__DOT__wait_blt_done__367__test_blitter__DOT__unnamedblk1_1__DOT____Vrepeat0 
            = (__Vtask_test_blitter__DOT__wait_blt_done__367__test_blitter__DOT__unnamedblk1_1__DOT____Vrepeat0 
               - (IData)(1U));
    }
    while (((0U != (IData)(vlSelfRef.test_blitter__DOT__dut__DOT__state)) 
            & (6U != (IData)(vlSelfRef.test_blitter__DOT__dut__DOT__state)))) {
        Vtest_blitter___024root____VbeforeTrig_h603fc354__0(vlSelf, 
                                                            "@(posedge test_blitter.clk)");
        co_await vlSelfRef.__VtrigSched_h603fc354__0.trigger(0U, 
                                                             nullptr, 
                                                             "@(posedge test_blitter.clk)", 
                                                             "test_blitter.sv", 
                                                             130);
    }
    __Vtask_test_blitter__DOT__wait_blt_done__367__test_blitter__DOT__unnamedblk1_2__DOT____Vrepeat1 = 2U;
    while (VL_LTS_III(32, 0U, __Vtask_test_blitter__DOT__wait_blt_done__367__test_blitter__DOT__unnamedblk1_2__DOT____Vrepeat1)) {
        Vtest_blitter___024root____VbeforeTrig_h603fc354__0(vlSelf, 
                                                            "@(posedge test_blitter.clk)");
        co_await vlSelfRef.__VtrigSched_h603fc354__0.trigger(0U, 
                                                             nullptr, 
                                                             "@(posedge test_blitter.clk)", 
                                                             "test_blitter.sv", 
                                                             131);
        __Vtask_test_blitter__DOT__wait_blt_done__367__test_blitter__DOT__unnamedblk1_2__DOT____Vrepeat1 
            = (__Vtask_test_blitter__DOT__wait_blt_done__367__test_blitter__DOT__unnamedblk1_2__DOT____Vrepeat1 
               - (IData)(1U));
    }
    __Vtask_test_blitter__DOT__check__368__condition 
        = (2U == vlSelfRef.test_blitter__DOT__dut__DOT__regs[1U]);
    vlSelfRef.__Vtask_test_blitter__DOT__check__368__name = "gfx fill ok"s;
    vlSelfRef.test_blitter__DOT__test_num = ((IData)(1U) 
                                             + vlSelfRef.test_blitter__DOT__test_num);
    if (__Vtask_test_blitter__DOT__check__368__condition) {
        VL_WRITEF_NX("  PASS [%0d] %@\n",0,32,vlSelfRef.test_blitter__DOT__test_num,
                     -1,&(vlSelfRef.__Vtask_test_blitter__DOT__check__368__name));
        vlSelfRef.test_blitter__DOT__pass_count = ((IData)(1U) 
                                                   + vlSelfRef.test_blitter__DOT__pass_count);
    } else {
        VL_WRITEF_NX("  FAIL [%0d] %@\n",0,32,vlSelfRef.test_blitter__DOT__test_num,
                     -1,&(vlSelfRef.__Vtask_test_blitter__DOT__check__368__name));
        vlSelfRef.test_blitter__DOT__fail_count = ((IData)(1U) 
                                                   + vlSelfRef.test_blitter__DOT__fail_count);
    }
    __Vtask_test_blitter__DOT__check__369__condition 
        = (5U == vlSelfRef.test_blitter__DOT__sim_gfx[0U]);
    vlSelfRef.__Vtask_test_blitter__DOT__check__369__name = "gfx[0] = 5"s;
    vlSelfRef.test_blitter__DOT__test_num = ((IData)(1U) 
                                             + vlSelfRef.test_blitter__DOT__test_num);
    if (__Vtask_test_blitter__DOT__check__369__condition) {
        VL_WRITEF_NX("  PASS [%0d] %@\n",0,32,vlSelfRef.test_blitter__DOT__test_num,
                     -1,&(vlSelfRef.__Vtask_test_blitter__DOT__check__369__name));
        vlSelfRef.test_blitter__DOT__pass_count = ((IData)(1U) 
                                                   + vlSelfRef.test_blitter__DOT__pass_count);
    } else {
        VL_WRITEF_NX("  FAIL [%0d] %@\n",0,32,vlSelfRef.test_blitter__DOT__test_num,
                     -1,&(vlSelfRef.__Vtask_test_blitter__DOT__check__369__name));
        vlSelfRef.test_blitter__DOT__fail_count = ((IData)(1U) 
                                                   + vlSelfRef.test_blitter__DOT__fail_count);
    }
    __Vtask_test_blitter__DOT__check__370__condition 
        = (5U == vlSelfRef.test_blitter__DOT__sim_gfx[319U]);
    vlSelfRef.__Vtask_test_blitter__DOT__check__370__name = "gfx[319] = 5"s;
    vlSelfRef.test_blitter__DOT__test_num = ((IData)(1U) 
                                             + vlSelfRef.test_blitter__DOT__test_num);
    if (__Vtask_test_blitter__DOT__check__370__condition) {
        VL_WRITEF_NX("  PASS [%0d] %@\n",0,32,vlSelfRef.test_blitter__DOT__test_num,
                     -1,&(vlSelfRef.__Vtask_test_blitter__DOT__check__370__name));
        vlSelfRef.test_blitter__DOT__pass_count = ((IData)(1U) 
                                                   + vlSelfRef.test_blitter__DOT__pass_count);
    } else {
        VL_WRITEF_NX("  FAIL [%0d] %@\n",0,32,vlSelfRef.test_blitter__DOT__test_num,
                     -1,&(vlSelfRef.__Vtask_test_blitter__DOT__check__370__name));
        vlSelfRef.test_blitter__DOT__fail_count = ((IData)(1U) 
                                                   + vlSelfRef.test_blitter__DOT__fail_count);
    }
    __Vtask_test_blitter__DOT__check__371__condition 
        = (0U == vlSelfRef.test_blitter__DOT__sim_gfx[320U]);
    vlSelfRef.__Vtask_test_blitter__DOT__check__371__name = "gfx[320] = 0 (not filled)"s;
    vlSelfRef.test_blitter__DOT__test_num = ((IData)(1U) 
                                             + vlSelfRef.test_blitter__DOT__test_num);
    if (__Vtask_test_blitter__DOT__check__371__condition) {
        VL_WRITEF_NX("  PASS [%0d] %@\n",0,32,vlSelfRef.test_blitter__DOT__test_num,
                     -1,&(vlSelfRef.__Vtask_test_blitter__DOT__check__371__name));
        vlSelfRef.test_blitter__DOT__pass_count = ((IData)(1U) 
                                                   + vlSelfRef.test_blitter__DOT__pass_count);
    } else {
        VL_WRITEF_NX("  FAIL [%0d] %@\n",0,32,vlSelfRef.test_blitter__DOT__test_num,
                     -1,&(vlSelfRef.__Vtask_test_blitter__DOT__check__371__name));
        vlSelfRef.test_blitter__DOT__fail_count = ((IData)(1U) 
                                                   + vlSelfRef.test_blitter__DOT__fail_count);
    }
    VL_WRITEF_NX("Test: Copy to XRAM\n",0);
    vlSelfRef.test_blitter__DOT__sim_ram[32768U] = 0xc0U;
    vlSelfRef.test_blitter__DOT__sim_ram[32769U] = 0xc1U;
    vlSelfRef.test_blitter__DOT__sim_ram[32770U] = 0xc2U;
    vlSelfRef.test_blitter__DOT__sim_ram[32771U] = 0xc3U;
    vlSelfRef.test_blitter__DOT__sim_ram[32772U] = 0xc4U;
    vlSelfRef.test_blitter__DOT__sim_ram[32773U] = 0xc5U;
    vlSelfRef.test_blitter__DOT__sim_ram[32774U] = 0xc6U;
    vlSelfRef.test_blitter__DOT__sim_ram[32775U] = 0xc7U;
    __Vtask_test_blitter__DOT__setup_copy__372__height = 1U;
    __Vtask_test_blitter__DOT__setup_copy__372__width = 8U;
    __Vtask_test_blitter__DOT__setup_copy__372__dst_stride = 8U;
    __Vtask_test_blitter__DOT__setup_copy__372__dst_addr = 0x00010000U;
    __Vtask_test_blitter__DOT__setup_copy__372__dst_space = 5U;
    __Vtask_test_blitter__DOT__setup_copy__372__src_stride = 8U;
    __Vtask_test_blitter__DOT__setup_copy__372__src_addr = 0x00008000U;
    __Vtask_test_blitter__DOT__setup_copy__372__src_space = 0U;
    __Vtask_test_blitter__DOT__blt_reg__373__data = 
        (0x000000ffU & __Vtask_test_blitter__DOT__setup_copy__372__src_space);
    __Vtask_test_blitter__DOT__blt_reg__373__off = 3U;
    __Vtask_test_blitter__DOT__write_reg__374__data 
        = __Vtask_test_blitter__DOT__blt_reg__373__data;
    __Vtask_test_blitter__DOT__write_reg__374__addr 
        = (0x0000ffffU & ((IData)(0xba83U) + __Vtask_test_blitter__DOT__blt_reg__373__off));
    Vtest_blitter___024root____VbeforeTrig_h603fc354__0(vlSelf, 
                                                        "@(posedge test_blitter.clk)");
    co_await vlSelfRef.__VtrigSched_h603fc354__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge test_blitter.clk)", 
                                                         "test_blitter.sv", 
                                                         113);
    vlSelfRef.test_blitter__DOT__cpu_addr = __Vtask_test_blitter__DOT__write_reg__374__addr;
    vlSelfRef.test_blitter__DOT__cpu_wdata = __Vtask_test_blitter__DOT__write_reg__374__data;
    vlSelfRef.test_blitter__DOT__cpu_we = 1U;
    Vtest_blitter___024root____VbeforeTrig_h603fc354__0(vlSelf, 
                                                        "@(posedge test_blitter.clk)");
    co_await vlSelfRef.__VtrigSched_h603fc354__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge test_blitter.clk)", 
                                                         "test_blitter.sv", 
                                                         115);
    vlSelfRef.test_blitter__DOT__cpu_we = 0U;
    __Vtask_test_blitter__DOT__blt_reg__375__data = 
        (0x000000ffU & __Vtask_test_blitter__DOT__setup_copy__372__dst_space);
    __Vtask_test_blitter__DOT__blt_reg__375__off = 4U;
    __Vtask_test_blitter__DOT__write_reg__376__data 
        = __Vtask_test_blitter__DOT__blt_reg__375__data;
    __Vtask_test_blitter__DOT__write_reg__376__addr 
        = (0x0000ffffU & ((IData)(0xba83U) + __Vtask_test_blitter__DOT__blt_reg__375__off));
    Vtest_blitter___024root____VbeforeTrig_h603fc354__0(vlSelf, 
                                                        "@(posedge test_blitter.clk)");
    co_await vlSelfRef.__VtrigSched_h603fc354__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge test_blitter.clk)", 
                                                         "test_blitter.sv", 
                                                         113);
    vlSelfRef.test_blitter__DOT__cpu_addr = __Vtask_test_blitter__DOT__write_reg__376__addr;
    vlSelfRef.test_blitter__DOT__cpu_wdata = __Vtask_test_blitter__DOT__write_reg__376__data;
    vlSelfRef.test_blitter__DOT__cpu_we = 1U;
    Vtest_blitter___024root____VbeforeTrig_h603fc354__0(vlSelf, 
                                                        "@(posedge test_blitter.clk)");
    co_await vlSelfRef.__VtrigSched_h603fc354__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge test_blitter.clk)", 
                                                         "test_blitter.sv", 
                                                         115);
    vlSelfRef.test_blitter__DOT__cpu_we = 0U;
    __Vtask_test_blitter__DOT__blt_reg__377__data = 
        (0x000000ffU & __Vtask_test_blitter__DOT__setup_copy__372__src_addr);
    __Vtask_test_blitter__DOT__blt_reg__377__off = 5U;
    __Vtask_test_blitter__DOT__write_reg__378__data 
        = __Vtask_test_blitter__DOT__blt_reg__377__data;
    __Vtask_test_blitter__DOT__write_reg__378__addr 
        = (0x0000ffffU & ((IData)(0xba83U) + __Vtask_test_blitter__DOT__blt_reg__377__off));
    Vtest_blitter___024root____VbeforeTrig_h603fc354__0(vlSelf, 
                                                        "@(posedge test_blitter.clk)");
    co_await vlSelfRef.__VtrigSched_h603fc354__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge test_blitter.clk)", 
                                                         "test_blitter.sv", 
                                                         113);
    vlSelfRef.test_blitter__DOT__cpu_addr = __Vtask_test_blitter__DOT__write_reg__378__addr;
    vlSelfRef.test_blitter__DOT__cpu_wdata = __Vtask_test_blitter__DOT__write_reg__378__data;
    vlSelfRef.test_blitter__DOT__cpu_we = 1U;
    Vtest_blitter___024root____VbeforeTrig_h603fc354__0(vlSelf, 
                                                        "@(posedge test_blitter.clk)");
    co_await vlSelfRef.__VtrigSched_h603fc354__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge test_blitter.clk)", 
                                                         "test_blitter.sv", 
                                                         115);
    vlSelfRef.test_blitter__DOT__cpu_we = 0U;
    __Vtask_test_blitter__DOT__blt_reg__379__data = 
        (0x000000ffU & (__Vtask_test_blitter__DOT__setup_copy__372__src_addr 
                        >> 8U));
    __Vtask_test_blitter__DOT__blt_reg__379__off = 6U;
    __Vtask_test_blitter__DOT__write_reg__380__data 
        = __Vtask_test_blitter__DOT__blt_reg__379__data;
    __Vtask_test_blitter__DOT__write_reg__380__addr 
        = (0x0000ffffU & ((IData)(0xba83U) + __Vtask_test_blitter__DOT__blt_reg__379__off));
    Vtest_blitter___024root____VbeforeTrig_h603fc354__0(vlSelf, 
                                                        "@(posedge test_blitter.clk)");
    co_await vlSelfRef.__VtrigSched_h603fc354__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge test_blitter.clk)", 
                                                         "test_blitter.sv", 
                                                         113);
    vlSelfRef.test_blitter__DOT__cpu_addr = __Vtask_test_blitter__DOT__write_reg__380__addr;
    vlSelfRef.test_blitter__DOT__cpu_wdata = __Vtask_test_blitter__DOT__write_reg__380__data;
    vlSelfRef.test_blitter__DOT__cpu_we = 1U;
    Vtest_blitter___024root____VbeforeTrig_h603fc354__0(vlSelf, 
                                                        "@(posedge test_blitter.clk)");
    co_await vlSelfRef.__VtrigSched_h603fc354__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge test_blitter.clk)", 
                                                         "test_blitter.sv", 
                                                         115);
    vlSelfRef.test_blitter__DOT__cpu_we = 0U;
    __Vtask_test_blitter__DOT__blt_reg__381__data = 
        (0x000000ffU & (__Vtask_test_blitter__DOT__setup_copy__372__src_addr 
                        >> 0x10U));
    __Vtask_test_blitter__DOT__blt_reg__381__off = 7U;
    __Vtask_test_blitter__DOT__write_reg__382__data 
        = __Vtask_test_blitter__DOT__blt_reg__381__data;
    __Vtask_test_blitter__DOT__write_reg__382__addr 
        = (0x0000ffffU & ((IData)(0xba83U) + __Vtask_test_blitter__DOT__blt_reg__381__off));
    Vtest_blitter___024root____VbeforeTrig_h603fc354__0(vlSelf, 
                                                        "@(posedge test_blitter.clk)");
    co_await vlSelfRef.__VtrigSched_h603fc354__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge test_blitter.clk)", 
                                                         "test_blitter.sv", 
                                                         113);
    vlSelfRef.test_blitter__DOT__cpu_addr = __Vtask_test_blitter__DOT__write_reg__382__addr;
    vlSelfRef.test_blitter__DOT__cpu_wdata = __Vtask_test_blitter__DOT__write_reg__382__data;
    vlSelfRef.test_blitter__DOT__cpu_we = 1U;
    Vtest_blitter___024root____VbeforeTrig_h603fc354__0(vlSelf, 
                                                        "@(posedge test_blitter.clk)");
    co_await vlSelfRef.__VtrigSched_h603fc354__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge test_blitter.clk)", 
                                                         "test_blitter.sv", 
                                                         115);
    vlSelfRef.test_blitter__DOT__cpu_we = 0U;
    __Vtask_test_blitter__DOT__blt_reg__383__data = 
        (0x000000ffU & __Vtask_test_blitter__DOT__setup_copy__372__dst_addr);
    __Vtask_test_blitter__DOT__blt_reg__383__off = 8U;
    __Vtask_test_blitter__DOT__write_reg__384__data 
        = __Vtask_test_blitter__DOT__blt_reg__383__data;
    __Vtask_test_blitter__DOT__write_reg__384__addr 
        = (0x0000ffffU & ((IData)(0xba83U) + __Vtask_test_blitter__DOT__blt_reg__383__off));
    Vtest_blitter___024root____VbeforeTrig_h603fc354__0(vlSelf, 
                                                        "@(posedge test_blitter.clk)");
    co_await vlSelfRef.__VtrigSched_h603fc354__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge test_blitter.clk)", 
                                                         "test_blitter.sv", 
                                                         113);
    vlSelfRef.test_blitter__DOT__cpu_addr = __Vtask_test_blitter__DOT__write_reg__384__addr;
    vlSelfRef.test_blitter__DOT__cpu_wdata = __Vtask_test_blitter__DOT__write_reg__384__data;
    vlSelfRef.test_blitter__DOT__cpu_we = 1U;
    Vtest_blitter___024root____VbeforeTrig_h603fc354__0(vlSelf, 
                                                        "@(posedge test_blitter.clk)");
    co_await vlSelfRef.__VtrigSched_h603fc354__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge test_blitter.clk)", 
                                                         "test_blitter.sv", 
                                                         115);
    vlSelfRef.test_blitter__DOT__cpu_we = 0U;
    __Vtask_test_blitter__DOT__blt_reg__385__data = 
        (0x000000ffU & (__Vtask_test_blitter__DOT__setup_copy__372__dst_addr 
                        >> 8U));
    __Vtask_test_blitter__DOT__blt_reg__385__off = 9U;
    __Vtask_test_blitter__DOT__write_reg__386__data 
        = __Vtask_test_blitter__DOT__blt_reg__385__data;
    __Vtask_test_blitter__DOT__write_reg__386__addr 
        = (0x0000ffffU & ((IData)(0xba83U) + __Vtask_test_blitter__DOT__blt_reg__385__off));
    Vtest_blitter___024root____VbeforeTrig_h603fc354__0(vlSelf, 
                                                        "@(posedge test_blitter.clk)");
    co_await vlSelfRef.__VtrigSched_h603fc354__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge test_blitter.clk)", 
                                                         "test_blitter.sv", 
                                                         113);
    vlSelfRef.test_blitter__DOT__cpu_addr = __Vtask_test_blitter__DOT__write_reg__386__addr;
    vlSelfRef.test_blitter__DOT__cpu_wdata = __Vtask_test_blitter__DOT__write_reg__386__data;
    vlSelfRef.test_blitter__DOT__cpu_we = 1U;
    Vtest_blitter___024root____VbeforeTrig_h603fc354__0(vlSelf, 
                                                        "@(posedge test_blitter.clk)");
    co_await vlSelfRef.__VtrigSched_h603fc354__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge test_blitter.clk)", 
                                                         "test_blitter.sv", 
                                                         115);
    vlSelfRef.test_blitter__DOT__cpu_we = 0U;
    __Vtask_test_blitter__DOT__blt_reg__387__data = 
        (0x000000ffU & (__Vtask_test_blitter__DOT__setup_copy__372__dst_addr 
                        >> 0x10U));
    __Vtask_test_blitter__DOT__blt_reg__387__off = 0x0000000aU;
    __Vtask_test_blitter__DOT__write_reg__388__data 
        = __Vtask_test_blitter__DOT__blt_reg__387__data;
    __Vtask_test_blitter__DOT__write_reg__388__addr 
        = (0x0000ffffU & ((IData)(0xba83U) + __Vtask_test_blitter__DOT__blt_reg__387__off));
    Vtest_blitter___024root____VbeforeTrig_h603fc354__0(vlSelf, 
                                                        "@(posedge test_blitter.clk)");
    co_await vlSelfRef.__VtrigSched_h603fc354__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge test_blitter.clk)", 
                                                         "test_blitter.sv", 
                                                         113);
    vlSelfRef.test_blitter__DOT__cpu_addr = __Vtask_test_blitter__DOT__write_reg__388__addr;
    vlSelfRef.test_blitter__DOT__cpu_wdata = __Vtask_test_blitter__DOT__write_reg__388__data;
    vlSelfRef.test_blitter__DOT__cpu_we = 1U;
    Vtest_blitter___024root____VbeforeTrig_h603fc354__0(vlSelf, 
                                                        "@(posedge test_blitter.clk)");
    co_await vlSelfRef.__VtrigSched_h603fc354__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge test_blitter.clk)", 
                                                         "test_blitter.sv", 
                                                         115);
    vlSelfRef.test_blitter__DOT__cpu_we = 0U;
    __Vtask_test_blitter__DOT__blt_reg__389__data = 
        (0x000000ffU & __Vtask_test_blitter__DOT__setup_copy__372__width);
    __Vtask_test_blitter__DOT__blt_reg__389__off = 0x0000000bU;
    __Vtask_test_blitter__DOT__write_reg__390__data 
        = __Vtask_test_blitter__DOT__blt_reg__389__data;
    __Vtask_test_blitter__DOT__write_reg__390__addr 
        = (0x0000ffffU & ((IData)(0xba83U) + __Vtask_test_blitter__DOT__blt_reg__389__off));
    Vtest_blitter___024root____VbeforeTrig_h603fc354__0(vlSelf, 
                                                        "@(posedge test_blitter.clk)");
    co_await vlSelfRef.__VtrigSched_h603fc354__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge test_blitter.clk)", 
                                                         "test_blitter.sv", 
                                                         113);
    vlSelfRef.test_blitter__DOT__cpu_addr = __Vtask_test_blitter__DOT__write_reg__390__addr;
    vlSelfRef.test_blitter__DOT__cpu_wdata = __Vtask_test_blitter__DOT__write_reg__390__data;
    vlSelfRef.test_blitter__DOT__cpu_we = 1U;
    Vtest_blitter___024root____VbeforeTrig_h603fc354__0(vlSelf, 
                                                        "@(posedge test_blitter.clk)");
    co_await vlSelfRef.__VtrigSched_h603fc354__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge test_blitter.clk)", 
                                                         "test_blitter.sv", 
                                                         115);
    vlSelfRef.test_blitter__DOT__cpu_we = 0U;
    __Vtask_test_blitter__DOT__blt_reg__391__data = 
        (0x000000ffU & (__Vtask_test_blitter__DOT__setup_copy__372__width 
                        >> 8U));
    __Vtask_test_blitter__DOT__blt_reg__391__off = 0x0000000cU;
    __Vtask_test_blitter__DOT__write_reg__392__data 
        = __Vtask_test_blitter__DOT__blt_reg__391__data;
    __Vtask_test_blitter__DOT__write_reg__392__addr 
        = (0x0000ffffU & ((IData)(0xba83U) + __Vtask_test_blitter__DOT__blt_reg__391__off));
    Vtest_blitter___024root____VbeforeTrig_h603fc354__0(vlSelf, 
                                                        "@(posedge test_blitter.clk)");
    co_await vlSelfRef.__VtrigSched_h603fc354__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge test_blitter.clk)", 
                                                         "test_blitter.sv", 
                                                         113);
    vlSelfRef.test_blitter__DOT__cpu_addr = __Vtask_test_blitter__DOT__write_reg__392__addr;
    vlSelfRef.test_blitter__DOT__cpu_wdata = __Vtask_test_blitter__DOT__write_reg__392__data;
    vlSelfRef.test_blitter__DOT__cpu_we = 1U;
    Vtest_blitter___024root____VbeforeTrig_h603fc354__0(vlSelf, 
                                                        "@(posedge test_blitter.clk)");
    co_await vlSelfRef.__VtrigSched_h603fc354__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge test_blitter.clk)", 
                                                         "test_blitter.sv", 
                                                         115);
    vlSelfRef.test_blitter__DOT__cpu_we = 0U;
    __Vtask_test_blitter__DOT__blt_reg__393__data = 
        (0x000000ffU & __Vtask_test_blitter__DOT__setup_copy__372__height);
    __Vtask_test_blitter__DOT__blt_reg__393__off = 0x0000000dU;
    __Vtask_test_blitter__DOT__write_reg__394__data 
        = __Vtask_test_blitter__DOT__blt_reg__393__data;
    __Vtask_test_blitter__DOT__write_reg__394__addr 
        = (0x0000ffffU & ((IData)(0xba83U) + __Vtask_test_blitter__DOT__blt_reg__393__off));
    Vtest_blitter___024root____VbeforeTrig_h603fc354__0(vlSelf, 
                                                        "@(posedge test_blitter.clk)");
    co_await vlSelfRef.__VtrigSched_h603fc354__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge test_blitter.clk)", 
                                                         "test_blitter.sv", 
                                                         113);
    vlSelfRef.test_blitter__DOT__cpu_addr = __Vtask_test_blitter__DOT__write_reg__394__addr;
    vlSelfRef.test_blitter__DOT__cpu_wdata = __Vtask_test_blitter__DOT__write_reg__394__data;
    vlSelfRef.test_blitter__DOT__cpu_we = 1U;
    Vtest_blitter___024root____VbeforeTrig_h603fc354__0(vlSelf, 
                                                        "@(posedge test_blitter.clk)");
    co_await vlSelfRef.__VtrigSched_h603fc354__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge test_blitter.clk)", 
                                                         "test_blitter.sv", 
                                                         115);
    vlSelfRef.test_blitter__DOT__cpu_we = 0U;
    __Vtask_test_blitter__DOT__blt_reg__395__data = 
        (0x000000ffU & (__Vtask_test_blitter__DOT__setup_copy__372__height 
                        >> 8U));
    __Vtask_test_blitter__DOT__blt_reg__395__off = 0x0000000eU;
    __Vtask_test_blitter__DOT__write_reg__396__data 
        = __Vtask_test_blitter__DOT__blt_reg__395__data;
    __Vtask_test_blitter__DOT__write_reg__396__addr 
        = (0x0000ffffU & ((IData)(0xba83U) + __Vtask_test_blitter__DOT__blt_reg__395__off));
    Vtest_blitter___024root____VbeforeTrig_h603fc354__0(vlSelf, 
                                                        "@(posedge test_blitter.clk)");
    co_await vlSelfRef.__VtrigSched_h603fc354__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge test_blitter.clk)", 
                                                         "test_blitter.sv", 
                                                         113);
    vlSelfRef.test_blitter__DOT__cpu_addr = __Vtask_test_blitter__DOT__write_reg__396__addr;
    vlSelfRef.test_blitter__DOT__cpu_wdata = __Vtask_test_blitter__DOT__write_reg__396__data;
    vlSelfRef.test_blitter__DOT__cpu_we = 1U;
    Vtest_blitter___024root____VbeforeTrig_h603fc354__0(vlSelf, 
                                                        "@(posedge test_blitter.clk)");
    co_await vlSelfRef.__VtrigSched_h603fc354__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge test_blitter.clk)", 
                                                         "test_blitter.sv", 
                                                         115);
    vlSelfRef.test_blitter__DOT__cpu_we = 0U;
    __Vtask_test_blitter__DOT__blt_reg__397__data = 
        (0x000000ffU & __Vtask_test_blitter__DOT__setup_copy__372__src_stride);
    __Vtask_test_blitter__DOT__blt_reg__397__off = 0x0000000fU;
    __Vtask_test_blitter__DOT__write_reg__398__data 
        = __Vtask_test_blitter__DOT__blt_reg__397__data;
    __Vtask_test_blitter__DOT__write_reg__398__addr 
        = (0x0000ffffU & ((IData)(0xba83U) + __Vtask_test_blitter__DOT__blt_reg__397__off));
    Vtest_blitter___024root____VbeforeTrig_h603fc354__0(vlSelf, 
                                                        "@(posedge test_blitter.clk)");
    co_await vlSelfRef.__VtrigSched_h603fc354__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge test_blitter.clk)", 
                                                         "test_blitter.sv", 
                                                         113);
    vlSelfRef.test_blitter__DOT__cpu_addr = __Vtask_test_blitter__DOT__write_reg__398__addr;
    vlSelfRef.test_blitter__DOT__cpu_wdata = __Vtask_test_blitter__DOT__write_reg__398__data;
    vlSelfRef.test_blitter__DOT__cpu_we = 1U;
    Vtest_blitter___024root____VbeforeTrig_h603fc354__0(vlSelf, 
                                                        "@(posedge test_blitter.clk)");
    co_await vlSelfRef.__VtrigSched_h603fc354__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge test_blitter.clk)", 
                                                         "test_blitter.sv", 
                                                         115);
    vlSelfRef.test_blitter__DOT__cpu_we = 0U;
    __Vtask_test_blitter__DOT__blt_reg__399__data = 
        (0x000000ffU & (__Vtask_test_blitter__DOT__setup_copy__372__src_stride 
                        >> 8U));
    __Vtask_test_blitter__DOT__blt_reg__399__off = 0x00000010U;
    __Vtask_test_blitter__DOT__write_reg__400__data 
        = __Vtask_test_blitter__DOT__blt_reg__399__data;
    __Vtask_test_blitter__DOT__write_reg__400__addr 
        = (0x0000ffffU & ((IData)(0xba83U) + __Vtask_test_blitter__DOT__blt_reg__399__off));
    Vtest_blitter___024root____VbeforeTrig_h603fc354__0(vlSelf, 
                                                        "@(posedge test_blitter.clk)");
    co_await vlSelfRef.__VtrigSched_h603fc354__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge test_blitter.clk)", 
                                                         "test_blitter.sv", 
                                                         113);
    vlSelfRef.test_blitter__DOT__cpu_addr = __Vtask_test_blitter__DOT__write_reg__400__addr;
    vlSelfRef.test_blitter__DOT__cpu_wdata = __Vtask_test_blitter__DOT__write_reg__400__data;
    vlSelfRef.test_blitter__DOT__cpu_we = 1U;
    Vtest_blitter___024root____VbeforeTrig_h603fc354__0(vlSelf, 
                                                        "@(posedge test_blitter.clk)");
    co_await vlSelfRef.__VtrigSched_h603fc354__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge test_blitter.clk)", 
                                                         "test_blitter.sv", 
                                                         115);
    vlSelfRef.test_blitter__DOT__cpu_we = 0U;
    __Vtask_test_blitter__DOT__blt_reg__401__data = 
        (0x000000ffU & __Vtask_test_blitter__DOT__setup_copy__372__dst_stride);
    __Vtask_test_blitter__DOT__blt_reg__401__off = 0x00000011U;
    __Vtask_test_blitter__DOT__write_reg__402__data 
        = __Vtask_test_blitter__DOT__blt_reg__401__data;
    __Vtask_test_blitter__DOT__write_reg__402__addr 
        = (0x0000ffffU & ((IData)(0xba83U) + __Vtask_test_blitter__DOT__blt_reg__401__off));
    Vtest_blitter___024root____VbeforeTrig_h603fc354__0(vlSelf, 
                                                        "@(posedge test_blitter.clk)");
    co_await vlSelfRef.__VtrigSched_h603fc354__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge test_blitter.clk)", 
                                                         "test_blitter.sv", 
                                                         113);
    vlSelfRef.test_blitter__DOT__cpu_addr = __Vtask_test_blitter__DOT__write_reg__402__addr;
    vlSelfRef.test_blitter__DOT__cpu_wdata = __Vtask_test_blitter__DOT__write_reg__402__data;
    vlSelfRef.test_blitter__DOT__cpu_we = 1U;
    Vtest_blitter___024root____VbeforeTrig_h603fc354__0(vlSelf, 
                                                        "@(posedge test_blitter.clk)");
    co_await vlSelfRef.__VtrigSched_h603fc354__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge test_blitter.clk)", 
                                                         "test_blitter.sv", 
                                                         115);
    vlSelfRef.test_blitter__DOT__cpu_we = 0U;
    __Vtask_test_blitter__DOT__blt_reg__403__data = 
        (0x000000ffU & (__Vtask_test_blitter__DOT__setup_copy__372__dst_stride 
                        >> 8U));
    __Vtask_test_blitter__DOT__blt_reg__403__off = 0x00000012U;
    __Vtask_test_blitter__DOT__write_reg__404__data 
        = __Vtask_test_blitter__DOT__blt_reg__403__data;
    __Vtask_test_blitter__DOT__write_reg__404__addr 
        = (0x0000ffffU & ((IData)(0xba83U) + __Vtask_test_blitter__DOT__blt_reg__403__off));
    Vtest_blitter___024root____VbeforeTrig_h603fc354__0(vlSelf, 
                                                        "@(posedge test_blitter.clk)");
    co_await vlSelfRef.__VtrigSched_h603fc354__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge test_blitter.clk)", 
                                                         "test_blitter.sv", 
                                                         113);
    vlSelfRef.test_blitter__DOT__cpu_addr = __Vtask_test_blitter__DOT__write_reg__404__addr;
    vlSelfRef.test_blitter__DOT__cpu_wdata = __Vtask_test_blitter__DOT__write_reg__404__data;
    vlSelfRef.test_blitter__DOT__cpu_we = 1U;
    Vtest_blitter___024root____VbeforeTrig_h603fc354__0(vlSelf, 
                                                        "@(posedge test_blitter.clk)");
    co_await vlSelfRef.__VtrigSched_h603fc354__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge test_blitter.clk)", 
                                                         "test_blitter.sv", 
                                                         115);
    vlSelfRef.test_blitter__DOT__cpu_we = 0U;
    __Vtask_test_blitter__DOT__blt_reg__405__data = 0U;
    __Vtask_test_blitter__DOT__blt_reg__405__off = 0x00000013U;
    __Vtask_test_blitter__DOT__write_reg__406__data 
        = __Vtask_test_blitter__DOT__blt_reg__405__data;
    __Vtask_test_blitter__DOT__write_reg__406__addr 
        = (0x0000ffffU & ((IData)(0xba83U) + __Vtask_test_blitter__DOT__blt_reg__405__off));
    Vtest_blitter___024root____VbeforeTrig_h603fc354__0(vlSelf, 
                                                        "@(posedge test_blitter.clk)");
    co_await vlSelfRef.__VtrigSched_h603fc354__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge test_blitter.clk)", 
                                                         "test_blitter.sv", 
                                                         113);
    vlSelfRef.test_blitter__DOT__cpu_addr = __Vtask_test_blitter__DOT__write_reg__406__addr;
    vlSelfRef.test_blitter__DOT__cpu_wdata = __Vtask_test_blitter__DOT__write_reg__406__data;
    vlSelfRef.test_blitter__DOT__cpu_we = 1U;
    Vtest_blitter___024root____VbeforeTrig_h603fc354__0(vlSelf, 
                                                        "@(posedge test_blitter.clk)");
    co_await vlSelfRef.__VtrigSched_h603fc354__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge test_blitter.clk)", 
                                                         "test_blitter.sv", 
                                                         115);
    vlSelfRef.test_blitter__DOT__cpu_we = 0U;
    __Vtask_test_blitter__DOT__blt_reg__408__data = 1U;
    __Vtask_test_blitter__DOT__blt_reg__408__off = 0U;
    __Vtask_test_blitter__DOT__write_reg__409__data 
        = __Vtask_test_blitter__DOT__blt_reg__408__data;
    __Vtask_test_blitter__DOT__write_reg__409__addr 
        = (0x0000ffffU & ((IData)(0xba83U) + __Vtask_test_blitter__DOT__blt_reg__408__off));
    Vtest_blitter___024root____VbeforeTrig_h603fc354__0(vlSelf, 
                                                        "@(posedge test_blitter.clk)");
    co_await vlSelfRef.__VtrigSched_h603fc354__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge test_blitter.clk)", 
                                                         "test_blitter.sv", 
                                                         113);
    vlSelfRef.test_blitter__DOT__cpu_addr = __Vtask_test_blitter__DOT__write_reg__409__addr;
    vlSelfRef.test_blitter__DOT__cpu_wdata = __Vtask_test_blitter__DOT__write_reg__409__data;
    vlSelfRef.test_blitter__DOT__cpu_we = 1U;
    Vtest_blitter___024root____VbeforeTrig_h603fc354__0(vlSelf, 
                                                        "@(posedge test_blitter.clk)");
    co_await vlSelfRef.__VtrigSched_h603fc354__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge test_blitter.clk)", 
                                                         "test_blitter.sv", 
                                                         115);
    vlSelfRef.test_blitter__DOT__cpu_we = 0U;
    __Vtask_test_blitter__DOT__wait_blt_done__410__test_blitter__DOT__unnamedblk1_1__DOT____Vrepeat0 = 0;
    __Vtask_test_blitter__DOT__wait_blt_done__410__test_blitter__DOT__unnamedblk1_2__DOT____Vrepeat1 = 0;
    __Vtask_test_blitter__DOT__wait_blt_done__410__test_blitter__DOT__unnamedblk1_1__DOT____Vrepeat0 = 2U;
    while (VL_LTS_III(32, 0U, __Vtask_test_blitter__DOT__wait_blt_done__410__test_blitter__DOT__unnamedblk1_1__DOT____Vrepeat0)) {
        Vtest_blitter___024root____VbeforeTrig_h603fc354__0(vlSelf, 
                                                            "@(posedge test_blitter.clk)");
        co_await vlSelfRef.__VtrigSched_h603fc354__0.trigger(0U, 
                                                             nullptr, 
                                                             "@(posedge test_blitter.clk)", 
                                                             "test_blitter.sv", 
                                                             128);
        __Vtask_test_blitter__DOT__wait_blt_done__410__test_blitter__DOT__unnamedblk1_1__DOT____Vrepeat0 
            = (__Vtask_test_blitter__DOT__wait_blt_done__410__test_blitter__DOT__unnamedblk1_1__DOT____Vrepeat0 
               - (IData)(1U));
    }
    while (((0U != (IData)(vlSelfRef.test_blitter__DOT__dut__DOT__state)) 
            & (6U != (IData)(vlSelfRef.test_blitter__DOT__dut__DOT__state)))) {
        Vtest_blitter___024root____VbeforeTrig_h603fc354__0(vlSelf, 
                                                            "@(posedge test_blitter.clk)");
        co_await vlSelfRef.__VtrigSched_h603fc354__0.trigger(0U, 
                                                             nullptr, 
                                                             "@(posedge test_blitter.clk)", 
                                                             "test_blitter.sv", 
                                                             130);
    }
    __Vtask_test_blitter__DOT__wait_blt_done__410__test_blitter__DOT__unnamedblk1_2__DOT____Vrepeat1 = 2U;
    while (VL_LTS_III(32, 0U, __Vtask_test_blitter__DOT__wait_blt_done__410__test_blitter__DOT__unnamedblk1_2__DOT____Vrepeat1)) {
        Vtest_blitter___024root____VbeforeTrig_h603fc354__0(vlSelf, 
                                                            "@(posedge test_blitter.clk)");
        co_await vlSelfRef.__VtrigSched_h603fc354__0.trigger(0U, 
                                                             nullptr, 
                                                             "@(posedge test_blitter.clk)", 
                                                             "test_blitter.sv", 
                                                             131);
        __Vtask_test_blitter__DOT__wait_blt_done__410__test_blitter__DOT__unnamedblk1_2__DOT____Vrepeat1 
            = (__Vtask_test_blitter__DOT__wait_blt_done__410__test_blitter__DOT__unnamedblk1_2__DOT____Vrepeat1 
               - (IData)(1U));
    }
    __Vtask_test_blitter__DOT__check__411__condition 
        = (2U == vlSelfRef.test_blitter__DOT__dut__DOT__regs[1U]);
    vlSelfRef.__Vtask_test_blitter__DOT__check__411__name = "xram copy ok"s;
    vlSelfRef.test_blitter__DOT__test_num = ((IData)(1U) 
                                             + vlSelfRef.test_blitter__DOT__test_num);
    if (__Vtask_test_blitter__DOT__check__411__condition) {
        VL_WRITEF_NX("  PASS [%0d] %@\n",0,32,vlSelfRef.test_blitter__DOT__test_num,
                     -1,&(vlSelfRef.__Vtask_test_blitter__DOT__check__411__name));
        vlSelfRef.test_blitter__DOT__pass_count = ((IData)(1U) 
                                                   + vlSelfRef.test_blitter__DOT__pass_count);
    } else {
        VL_WRITEF_NX("  FAIL [%0d] %@\n",0,32,vlSelfRef.test_blitter__DOT__test_num,
                     -1,&(vlSelfRef.__Vtask_test_blitter__DOT__check__411__name));
        vlSelfRef.test_blitter__DOT__fail_count = ((IData)(1U) 
                                                   + vlSelfRef.test_blitter__DOT__fail_count);
    }
    __Vtask_test_blitter__DOT__check__412__condition 
        = (0xc0U == vlSelfRef.test_blitter__DOT__sim_xram[65536U]);
    vlSelfRef.__Vtask_test_blitter__DOT__check__412__name = "xram[$10000] = C0"s;
    vlSelfRef.test_blitter__DOT__test_num = ((IData)(1U) 
                                             + vlSelfRef.test_blitter__DOT__test_num);
    if (__Vtask_test_blitter__DOT__check__412__condition) {
        VL_WRITEF_NX("  PASS [%0d] %@\n",0,32,vlSelfRef.test_blitter__DOT__test_num,
                     -1,&(vlSelfRef.__Vtask_test_blitter__DOT__check__412__name));
        vlSelfRef.test_blitter__DOT__pass_count = ((IData)(1U) 
                                                   + vlSelfRef.test_blitter__DOT__pass_count);
    } else {
        VL_WRITEF_NX("  FAIL [%0d] %@\n",0,32,vlSelfRef.test_blitter__DOT__test_num,
                     -1,&(vlSelfRef.__Vtask_test_blitter__DOT__check__412__name));
        vlSelfRef.test_blitter__DOT__fail_count = ((IData)(1U) 
                                                   + vlSelfRef.test_blitter__DOT__fail_count);
    }
    __Vtask_test_blitter__DOT__check__413__condition 
        = (0xc7U == vlSelfRef.test_blitter__DOT__sim_xram[65543U]);
    vlSelfRef.__Vtask_test_blitter__DOT__check__413__name = "xram[$10007] = C7"s;
    vlSelfRef.test_blitter__DOT__test_num = ((IData)(1U) 
                                             + vlSelfRef.test_blitter__DOT__test_num);
    if (__Vtask_test_blitter__DOT__check__413__condition) {
        VL_WRITEF_NX("  PASS [%0d] %@\n",0,32,vlSelfRef.test_blitter__DOT__test_num,
                     -1,&(vlSelfRef.__Vtask_test_blitter__DOT__check__413__name));
        vlSelfRef.test_blitter__DOT__pass_count = ((IData)(1U) 
                                                   + vlSelfRef.test_blitter__DOT__pass_count);
    } else {
        VL_WRITEF_NX("  FAIL [%0d] %@\n",0,32,vlSelfRef.test_blitter__DOT__test_num,
                     -1,&(vlSelfRef.__Vtask_test_blitter__DOT__check__413__name));
        vlSelfRef.test_blitter__DOT__fail_count = ((IData)(1U) 
                                                   + vlSelfRef.test_blitter__DOT__fail_count);
    }
    VL_WRITEF_NX("Test: Fill sprite shapes\n",0);
    __Vtask_test_blitter__DOT__setup_fill__414__fill_val = 0x000000ffU;
    __Vtask_test_blitter__DOT__setup_fill__414__height = 1U;
    __Vtask_test_blitter__DOT__setup_fill__414__width = 0x00000080U;
    __Vtask_test_blitter__DOT__setup_fill__414__dst_stride = 0x00000080U;
    __Vtask_test_blitter__DOT__setup_fill__414__dst_addr = 0x00000100U;
    __Vtask_test_blitter__DOT__setup_fill__414__dst_space = 4U;
    __Vtask_test_blitter__DOT__blt_reg__415__data = 
        (0x000000ffU & __Vtask_test_blitter__DOT__setup_fill__414__dst_space);
    __Vtask_test_blitter__DOT__blt_reg__415__off = 4U;
    __Vtask_test_blitter__DOT__write_reg__416__data 
        = __Vtask_test_blitter__DOT__blt_reg__415__data;
    __Vtask_test_blitter__DOT__write_reg__416__addr 
        = (0x0000ffffU & ((IData)(0xba83U) + __Vtask_test_blitter__DOT__blt_reg__415__off));
    Vtest_blitter___024root____VbeforeTrig_h603fc354__0(vlSelf, 
                                                        "@(posedge test_blitter.clk)");
    co_await vlSelfRef.__VtrigSched_h603fc354__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge test_blitter.clk)", 
                                                         "test_blitter.sv", 
                                                         113);
    vlSelfRef.test_blitter__DOT__cpu_addr = __Vtask_test_blitter__DOT__write_reg__416__addr;
    vlSelfRef.test_blitter__DOT__cpu_wdata = __Vtask_test_blitter__DOT__write_reg__416__data;
    vlSelfRef.test_blitter__DOT__cpu_we = 1U;
    Vtest_blitter___024root____VbeforeTrig_h603fc354__0(vlSelf, 
                                                        "@(posedge test_blitter.clk)");
    co_await vlSelfRef.__VtrigSched_h603fc354__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge test_blitter.clk)", 
                                                         "test_blitter.sv", 
                                                         115);
    vlSelfRef.test_blitter__DOT__cpu_we = 0U;
    __Vtask_test_blitter__DOT__blt_reg__417__data = 
        (0x000000ffU & __Vtask_test_blitter__DOT__setup_fill__414__dst_addr);
    __Vtask_test_blitter__DOT__blt_reg__417__off = 8U;
    __Vtask_test_blitter__DOT__write_reg__418__data 
        = __Vtask_test_blitter__DOT__blt_reg__417__data;
    __Vtask_test_blitter__DOT__write_reg__418__addr 
        = (0x0000ffffU & ((IData)(0xba83U) + __Vtask_test_blitter__DOT__blt_reg__417__off));
    Vtest_blitter___024root____VbeforeTrig_h603fc354__0(vlSelf, 
                                                        "@(posedge test_blitter.clk)");
    co_await vlSelfRef.__VtrigSched_h603fc354__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge test_blitter.clk)", 
                                                         "test_blitter.sv", 
                                                         113);
    vlSelfRef.test_blitter__DOT__cpu_addr = __Vtask_test_blitter__DOT__write_reg__418__addr;
    vlSelfRef.test_blitter__DOT__cpu_wdata = __Vtask_test_blitter__DOT__write_reg__418__data;
    vlSelfRef.test_blitter__DOT__cpu_we = 1U;
    Vtest_blitter___024root____VbeforeTrig_h603fc354__0(vlSelf, 
                                                        "@(posedge test_blitter.clk)");
    co_await vlSelfRef.__VtrigSched_h603fc354__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge test_blitter.clk)", 
                                                         "test_blitter.sv", 
                                                         115);
    vlSelfRef.test_blitter__DOT__cpu_we = 0U;
    __Vtask_test_blitter__DOT__blt_reg__419__data = 
        (0x000000ffU & (__Vtask_test_blitter__DOT__setup_fill__414__dst_addr 
                        >> 8U));
    __Vtask_test_blitter__DOT__blt_reg__419__off = 9U;
    __Vtask_test_blitter__DOT__write_reg__420__data 
        = __Vtask_test_blitter__DOT__blt_reg__419__data;
    __Vtask_test_blitter__DOT__write_reg__420__addr 
        = (0x0000ffffU & ((IData)(0xba83U) + __Vtask_test_blitter__DOT__blt_reg__419__off));
    Vtest_blitter___024root____VbeforeTrig_h603fc354__0(vlSelf, 
                                                        "@(posedge test_blitter.clk)");
    co_await vlSelfRef.__VtrigSched_h603fc354__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge test_blitter.clk)", 
                                                         "test_blitter.sv", 
                                                         113);
    vlSelfRef.test_blitter__DOT__cpu_addr = __Vtask_test_blitter__DOT__write_reg__420__addr;
    vlSelfRef.test_blitter__DOT__cpu_wdata = __Vtask_test_blitter__DOT__write_reg__420__data;
    vlSelfRef.test_blitter__DOT__cpu_we = 1U;
    Vtest_blitter___024root____VbeforeTrig_h603fc354__0(vlSelf, 
                                                        "@(posedge test_blitter.clk)");
    co_await vlSelfRef.__VtrigSched_h603fc354__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge test_blitter.clk)", 
                                                         "test_blitter.sv", 
                                                         115);
    vlSelfRef.test_blitter__DOT__cpu_we = 0U;
    __Vtask_test_blitter__DOT__blt_reg__421__data = 
        (0x000000ffU & (__Vtask_test_blitter__DOT__setup_fill__414__dst_addr 
                        >> 0x10U));
    __Vtask_test_blitter__DOT__blt_reg__421__off = 0x0000000aU;
    __Vtask_test_blitter__DOT__write_reg__422__data 
        = __Vtask_test_blitter__DOT__blt_reg__421__data;
    __Vtask_test_blitter__DOT__write_reg__422__addr 
        = (0x0000ffffU & ((IData)(0xba83U) + __Vtask_test_blitter__DOT__blt_reg__421__off));
    Vtest_blitter___024root____VbeforeTrig_h603fc354__0(vlSelf, 
                                                        "@(posedge test_blitter.clk)");
    co_await vlSelfRef.__VtrigSched_h603fc354__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge test_blitter.clk)", 
                                                         "test_blitter.sv", 
                                                         113);
    vlSelfRef.test_blitter__DOT__cpu_addr = __Vtask_test_blitter__DOT__write_reg__422__addr;
    vlSelfRef.test_blitter__DOT__cpu_wdata = __Vtask_test_blitter__DOT__write_reg__422__data;
    vlSelfRef.test_blitter__DOT__cpu_we = 1U;
    Vtest_blitter___024root____VbeforeTrig_h603fc354__0(vlSelf, 
                                                        "@(posedge test_blitter.clk)");
    co_await vlSelfRef.__VtrigSched_h603fc354__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge test_blitter.clk)", 
                                                         "test_blitter.sv", 
                                                         115);
    vlSelfRef.test_blitter__DOT__cpu_we = 0U;
    __Vtask_test_blitter__DOT__blt_reg__423__data = 
        (0x000000ffU & __Vtask_test_blitter__DOT__setup_fill__414__width);
    __Vtask_test_blitter__DOT__blt_reg__423__off = 0x0000000bU;
    __Vtask_test_blitter__DOT__write_reg__424__data 
        = __Vtask_test_blitter__DOT__blt_reg__423__data;
    __Vtask_test_blitter__DOT__write_reg__424__addr 
        = (0x0000ffffU & ((IData)(0xba83U) + __Vtask_test_blitter__DOT__blt_reg__423__off));
    Vtest_blitter___024root____VbeforeTrig_h603fc354__0(vlSelf, 
                                                        "@(posedge test_blitter.clk)");
    co_await vlSelfRef.__VtrigSched_h603fc354__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge test_blitter.clk)", 
                                                         "test_blitter.sv", 
                                                         113);
    vlSelfRef.test_blitter__DOT__cpu_addr = __Vtask_test_blitter__DOT__write_reg__424__addr;
    vlSelfRef.test_blitter__DOT__cpu_wdata = __Vtask_test_blitter__DOT__write_reg__424__data;
    vlSelfRef.test_blitter__DOT__cpu_we = 1U;
    Vtest_blitter___024root____VbeforeTrig_h603fc354__0(vlSelf, 
                                                        "@(posedge test_blitter.clk)");
    co_await vlSelfRef.__VtrigSched_h603fc354__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge test_blitter.clk)", 
                                                         "test_blitter.sv", 
                                                         115);
    vlSelfRef.test_blitter__DOT__cpu_we = 0U;
    __Vtask_test_blitter__DOT__blt_reg__425__data = 
        (0x000000ffU & (__Vtask_test_blitter__DOT__setup_fill__414__width 
                        >> 8U));
    __Vtask_test_blitter__DOT__blt_reg__425__off = 0x0000000cU;
    __Vtask_test_blitter__DOT__write_reg__426__data 
        = __Vtask_test_blitter__DOT__blt_reg__425__data;
    __Vtask_test_blitter__DOT__write_reg__426__addr 
        = (0x0000ffffU & ((IData)(0xba83U) + __Vtask_test_blitter__DOT__blt_reg__425__off));
    Vtest_blitter___024root____VbeforeTrig_h603fc354__0(vlSelf, 
                                                        "@(posedge test_blitter.clk)");
    co_await vlSelfRef.__VtrigSched_h603fc354__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge test_blitter.clk)", 
                                                         "test_blitter.sv", 
                                                         113);
    vlSelfRef.test_blitter__DOT__cpu_addr = __Vtask_test_blitter__DOT__write_reg__426__addr;
    vlSelfRef.test_blitter__DOT__cpu_wdata = __Vtask_test_blitter__DOT__write_reg__426__data;
    vlSelfRef.test_blitter__DOT__cpu_we = 1U;
    Vtest_blitter___024root____VbeforeTrig_h603fc354__0(vlSelf, 
                                                        "@(posedge test_blitter.clk)");
    co_await vlSelfRef.__VtrigSched_h603fc354__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge test_blitter.clk)", 
                                                         "test_blitter.sv", 
                                                         115);
    vlSelfRef.test_blitter__DOT__cpu_we = 0U;
    __Vtask_test_blitter__DOT__blt_reg__427__data = 
        (0x000000ffU & __Vtask_test_blitter__DOT__setup_fill__414__height);
    __Vtask_test_blitter__DOT__blt_reg__427__off = 0x0000000dU;
    __Vtask_test_blitter__DOT__write_reg__428__data 
        = __Vtask_test_blitter__DOT__blt_reg__427__data;
    __Vtask_test_blitter__DOT__write_reg__428__addr 
        = (0x0000ffffU & ((IData)(0xba83U) + __Vtask_test_blitter__DOT__blt_reg__427__off));
    Vtest_blitter___024root____VbeforeTrig_h603fc354__0(vlSelf, 
                                                        "@(posedge test_blitter.clk)");
    co_await vlSelfRef.__VtrigSched_h603fc354__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge test_blitter.clk)", 
                                                         "test_blitter.sv", 
                                                         113);
    vlSelfRef.test_blitter__DOT__cpu_addr = __Vtask_test_blitter__DOT__write_reg__428__addr;
    vlSelfRef.test_blitter__DOT__cpu_wdata = __Vtask_test_blitter__DOT__write_reg__428__data;
    vlSelfRef.test_blitter__DOT__cpu_we = 1U;
    Vtest_blitter___024root____VbeforeTrig_h603fc354__0(vlSelf, 
                                                        "@(posedge test_blitter.clk)");
    co_await vlSelfRef.__VtrigSched_h603fc354__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge test_blitter.clk)", 
                                                         "test_blitter.sv", 
                                                         115);
    vlSelfRef.test_blitter__DOT__cpu_we = 0U;
    __Vtask_test_blitter__DOT__blt_reg__429__data = 
        (0x000000ffU & (__Vtask_test_blitter__DOT__setup_fill__414__height 
                        >> 8U));
    __Vtask_test_blitter__DOT__blt_reg__429__off = 0x0000000eU;
    __Vtask_test_blitter__DOT__write_reg__430__data 
        = __Vtask_test_blitter__DOT__blt_reg__429__data;
    __Vtask_test_blitter__DOT__write_reg__430__addr 
        = (0x0000ffffU & ((IData)(0xba83U) + __Vtask_test_blitter__DOT__blt_reg__429__off));
    Vtest_blitter___024root____VbeforeTrig_h603fc354__0(vlSelf, 
                                                        "@(posedge test_blitter.clk)");
    co_await vlSelfRef.__VtrigSched_h603fc354__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge test_blitter.clk)", 
                                                         "test_blitter.sv", 
                                                         113);
    vlSelfRef.test_blitter__DOT__cpu_addr = __Vtask_test_blitter__DOT__write_reg__430__addr;
    vlSelfRef.test_blitter__DOT__cpu_wdata = __Vtask_test_blitter__DOT__write_reg__430__data;
    vlSelfRef.test_blitter__DOT__cpu_we = 1U;
    Vtest_blitter___024root____VbeforeTrig_h603fc354__0(vlSelf, 
                                                        "@(posedge test_blitter.clk)");
    co_await vlSelfRef.__VtrigSched_h603fc354__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge test_blitter.clk)", 
                                                         "test_blitter.sv", 
                                                         115);
    vlSelfRef.test_blitter__DOT__cpu_we = 0U;
    __Vtask_test_blitter__DOT__blt_reg__431__data = 
        (0x000000ffU & __Vtask_test_blitter__DOT__setup_fill__414__dst_stride);
    __Vtask_test_blitter__DOT__blt_reg__431__off = 0x00000011U;
    __Vtask_test_blitter__DOT__write_reg__432__data 
        = __Vtask_test_blitter__DOT__blt_reg__431__data;
    __Vtask_test_blitter__DOT__write_reg__432__addr 
        = (0x0000ffffU & ((IData)(0xba83U) + __Vtask_test_blitter__DOT__blt_reg__431__off));
    Vtest_blitter___024root____VbeforeTrig_h603fc354__0(vlSelf, 
                                                        "@(posedge test_blitter.clk)");
    co_await vlSelfRef.__VtrigSched_h603fc354__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge test_blitter.clk)", 
                                                         "test_blitter.sv", 
                                                         113);
    vlSelfRef.test_blitter__DOT__cpu_addr = __Vtask_test_blitter__DOT__write_reg__432__addr;
    vlSelfRef.test_blitter__DOT__cpu_wdata = __Vtask_test_blitter__DOT__write_reg__432__data;
    vlSelfRef.test_blitter__DOT__cpu_we = 1U;
    Vtest_blitter___024root____VbeforeTrig_h603fc354__0(vlSelf, 
                                                        "@(posedge test_blitter.clk)");
    co_await vlSelfRef.__VtrigSched_h603fc354__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge test_blitter.clk)", 
                                                         "test_blitter.sv", 
                                                         115);
    vlSelfRef.test_blitter__DOT__cpu_we = 0U;
    __Vtask_test_blitter__DOT__blt_reg__433__data = 
        (0x000000ffU & (__Vtask_test_blitter__DOT__setup_fill__414__dst_stride 
                        >> 8U));
    __Vtask_test_blitter__DOT__blt_reg__433__off = 0x00000012U;
    __Vtask_test_blitter__DOT__write_reg__434__data 
        = __Vtask_test_blitter__DOT__blt_reg__433__data;
    __Vtask_test_blitter__DOT__write_reg__434__addr 
        = (0x0000ffffU & ((IData)(0xba83U) + __Vtask_test_blitter__DOT__blt_reg__433__off));
    Vtest_blitter___024root____VbeforeTrig_h603fc354__0(vlSelf, 
                                                        "@(posedge test_blitter.clk)");
    co_await vlSelfRef.__VtrigSched_h603fc354__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge test_blitter.clk)", 
                                                         "test_blitter.sv", 
                                                         113);
    vlSelfRef.test_blitter__DOT__cpu_addr = __Vtask_test_blitter__DOT__write_reg__434__addr;
    vlSelfRef.test_blitter__DOT__cpu_wdata = __Vtask_test_blitter__DOT__write_reg__434__data;
    vlSelfRef.test_blitter__DOT__cpu_we = 1U;
    Vtest_blitter___024root____VbeforeTrig_h603fc354__0(vlSelf, 
                                                        "@(posedge test_blitter.clk)");
    co_await vlSelfRef.__VtrigSched_h603fc354__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge test_blitter.clk)", 
                                                         "test_blitter.sv", 
                                                         115);
    vlSelfRef.test_blitter__DOT__cpu_we = 0U;
    __Vtask_test_blitter__DOT__blt_reg__435__data = 1U;
    __Vtask_test_blitter__DOT__blt_reg__435__off = 0x00000013U;
    __Vtask_test_blitter__DOT__write_reg__436__data 
        = __Vtask_test_blitter__DOT__blt_reg__435__data;
    __Vtask_test_blitter__DOT__write_reg__436__addr 
        = (0x0000ffffU & ((IData)(0xba83U) + __Vtask_test_blitter__DOT__blt_reg__435__off));
    Vtest_blitter___024root____VbeforeTrig_h603fc354__0(vlSelf, 
                                                        "@(posedge test_blitter.clk)");
    co_await vlSelfRef.__VtrigSched_h603fc354__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge test_blitter.clk)", 
                                                         "test_blitter.sv", 
                                                         113);
    vlSelfRef.test_blitter__DOT__cpu_addr = __Vtask_test_blitter__DOT__write_reg__436__addr;
    vlSelfRef.test_blitter__DOT__cpu_wdata = __Vtask_test_blitter__DOT__write_reg__436__data;
    vlSelfRef.test_blitter__DOT__cpu_we = 1U;
    Vtest_blitter___024root____VbeforeTrig_h603fc354__0(vlSelf, 
                                                        "@(posedge test_blitter.clk)");
    co_await vlSelfRef.__VtrigSched_h603fc354__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge test_blitter.clk)", 
                                                         "test_blitter.sv", 
                                                         115);
    vlSelfRef.test_blitter__DOT__cpu_we = 0U;
    __Vtask_test_blitter__DOT__blt_reg__437__data = 
        (0x000000ffU & __Vtask_test_blitter__DOT__setup_fill__414__fill_val);
    __Vtask_test_blitter__DOT__blt_reg__437__off = 0x00000014U;
    __Vtask_test_blitter__DOT__write_reg__438__data 
        = __Vtask_test_blitter__DOT__blt_reg__437__data;
    __Vtask_test_blitter__DOT__write_reg__438__addr 
        = (0x0000ffffU & ((IData)(0xba83U) + __Vtask_test_blitter__DOT__blt_reg__437__off));
    Vtest_blitter___024root____VbeforeTrig_h603fc354__0(vlSelf, 
                                                        "@(posedge test_blitter.clk)");
    co_await vlSelfRef.__VtrigSched_h603fc354__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge test_blitter.clk)", 
                                                         "test_blitter.sv", 
                                                         113);
    vlSelfRef.test_blitter__DOT__cpu_addr = __Vtask_test_blitter__DOT__write_reg__438__addr;
    vlSelfRef.test_blitter__DOT__cpu_wdata = __Vtask_test_blitter__DOT__write_reg__438__data;
    vlSelfRef.test_blitter__DOT__cpu_we = 1U;
    Vtest_blitter___024root____VbeforeTrig_h603fc354__0(vlSelf, 
                                                        "@(posedge test_blitter.clk)");
    co_await vlSelfRef.__VtrigSched_h603fc354__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge test_blitter.clk)", 
                                                         "test_blitter.sv", 
                                                         115);
    vlSelfRef.test_blitter__DOT__cpu_we = 0U;
    __Vtask_test_blitter__DOT__blt_reg__440__data = 1U;
    __Vtask_test_blitter__DOT__blt_reg__440__off = 0U;
    __Vtask_test_blitter__DOT__write_reg__441__data 
        = __Vtask_test_blitter__DOT__blt_reg__440__data;
    __Vtask_test_blitter__DOT__write_reg__441__addr 
        = (0x0000ffffU & ((IData)(0xba83U) + __Vtask_test_blitter__DOT__blt_reg__440__off));
    Vtest_blitter___024root____VbeforeTrig_h603fc354__0(vlSelf, 
                                                        "@(posedge test_blitter.clk)");
    co_await vlSelfRef.__VtrigSched_h603fc354__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge test_blitter.clk)", 
                                                         "test_blitter.sv", 
                                                         113);
    vlSelfRef.test_blitter__DOT__cpu_addr = __Vtask_test_blitter__DOT__write_reg__441__addr;
    vlSelfRef.test_blitter__DOT__cpu_wdata = __Vtask_test_blitter__DOT__write_reg__441__data;
    vlSelfRef.test_blitter__DOT__cpu_we = 1U;
    Vtest_blitter___024root____VbeforeTrig_h603fc354__0(vlSelf, 
                                                        "@(posedge test_blitter.clk)");
    co_await vlSelfRef.__VtrigSched_h603fc354__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge test_blitter.clk)", 
                                                         "test_blitter.sv", 
                                                         115);
    vlSelfRef.test_blitter__DOT__cpu_we = 0U;
    __Vtask_test_blitter__DOT__wait_blt_done__442__test_blitter__DOT__unnamedblk1_1__DOT____Vrepeat0 = 0;
    __Vtask_test_blitter__DOT__wait_blt_done__442__test_blitter__DOT__unnamedblk1_2__DOT____Vrepeat1 = 0;
    __Vtask_test_blitter__DOT__wait_blt_done__442__test_blitter__DOT__unnamedblk1_1__DOT____Vrepeat0 = 2U;
    while (VL_LTS_III(32, 0U, __Vtask_test_blitter__DOT__wait_blt_done__442__test_blitter__DOT__unnamedblk1_1__DOT____Vrepeat0)) {
        Vtest_blitter___024root____VbeforeTrig_h603fc354__0(vlSelf, 
                                                            "@(posedge test_blitter.clk)");
        co_await vlSelfRef.__VtrigSched_h603fc354__0.trigger(0U, 
                                                             nullptr, 
                                                             "@(posedge test_blitter.clk)", 
                                                             "test_blitter.sv", 
                                                             128);
        __Vtask_test_blitter__DOT__wait_blt_done__442__test_blitter__DOT__unnamedblk1_1__DOT____Vrepeat0 
            = (__Vtask_test_blitter__DOT__wait_blt_done__442__test_blitter__DOT__unnamedblk1_1__DOT____Vrepeat0 
               - (IData)(1U));
    }
    while (((0U != (IData)(vlSelfRef.test_blitter__DOT__dut__DOT__state)) 
            & (6U != (IData)(vlSelfRef.test_blitter__DOT__dut__DOT__state)))) {
        Vtest_blitter___024root____VbeforeTrig_h603fc354__0(vlSelf, 
                                                            "@(posedge test_blitter.clk)");
        co_await vlSelfRef.__VtrigSched_h603fc354__0.trigger(0U, 
                                                             nullptr, 
                                                             "@(posedge test_blitter.clk)", 
                                                             "test_blitter.sv", 
                                                             130);
    }
    __Vtask_test_blitter__DOT__wait_blt_done__442__test_blitter__DOT__unnamedblk1_2__DOT____Vrepeat1 = 2U;
    while (VL_LTS_III(32, 0U, __Vtask_test_blitter__DOT__wait_blt_done__442__test_blitter__DOT__unnamedblk1_2__DOT____Vrepeat1)) {
        Vtest_blitter___024root____VbeforeTrig_h603fc354__0(vlSelf, 
                                                            "@(posedge test_blitter.clk)");
        co_await vlSelfRef.__VtrigSched_h603fc354__0.trigger(0U, 
                                                             nullptr, 
                                                             "@(posedge test_blitter.clk)", 
                                                             "test_blitter.sv", 
                                                             131);
        __Vtask_test_blitter__DOT__wait_blt_done__442__test_blitter__DOT__unnamedblk1_2__DOT____Vrepeat1 
            = (__Vtask_test_blitter__DOT__wait_blt_done__442__test_blitter__DOT__unnamedblk1_2__DOT____Vrepeat1 
               - (IData)(1U));
    }
    __Vtask_test_blitter__DOT__check__443__condition 
        = (2U == vlSelfRef.test_blitter__DOT__dut__DOT__regs[1U]);
    vlSelfRef.__Vtask_test_blitter__DOT__check__443__name = "sprite fill ok"s;
    vlSelfRef.test_blitter__DOT__test_num = ((IData)(1U) 
                                             + vlSelfRef.test_blitter__DOT__test_num);
    if (__Vtask_test_blitter__DOT__check__443__condition) {
        VL_WRITEF_NX("  PASS [%0d] %@\n",0,32,vlSelfRef.test_blitter__DOT__test_num,
                     -1,&(vlSelfRef.__Vtask_test_blitter__DOT__check__443__name));
        vlSelfRef.test_blitter__DOT__pass_count = ((IData)(1U) 
                                                   + vlSelfRef.test_blitter__DOT__pass_count);
    } else {
        VL_WRITEF_NX("  FAIL [%0d] %@\n",0,32,vlSelfRef.test_blitter__DOT__test_num,
                     -1,&(vlSelfRef.__Vtask_test_blitter__DOT__check__443__name));
        vlSelfRef.test_blitter__DOT__fail_count = ((IData)(1U) 
                                                   + vlSelfRef.test_blitter__DOT__fail_count);
    }
    __Vtask_test_blitter__DOT__check__444__condition 
        = (0xffU == vlSelfRef.test_blitter__DOT__sim_sprite[256U]);
    vlSelfRef.__Vtask_test_blitter__DOT__check__444__name = "sprite[256] = FF"s;
    vlSelfRef.test_blitter__DOT__test_num = ((IData)(1U) 
                                             + vlSelfRef.test_blitter__DOT__test_num);
    if (__Vtask_test_blitter__DOT__check__444__condition) {
        VL_WRITEF_NX("  PASS [%0d] %@\n",0,32,vlSelfRef.test_blitter__DOT__test_num,
                     -1,&(vlSelfRef.__Vtask_test_blitter__DOT__check__444__name));
        vlSelfRef.test_blitter__DOT__pass_count = ((IData)(1U) 
                                                   + vlSelfRef.test_blitter__DOT__pass_count);
    } else {
        VL_WRITEF_NX("  FAIL [%0d] %@\n",0,32,vlSelfRef.test_blitter__DOT__test_num,
                     -1,&(vlSelfRef.__Vtask_test_blitter__DOT__check__444__name));
        vlSelfRef.test_blitter__DOT__fail_count = ((IData)(1U) 
                                                   + vlSelfRef.test_blitter__DOT__fail_count);
    }
    __Vtask_test_blitter__DOT__check__445__condition 
        = (0xffU == vlSelfRef.test_blitter__DOT__sim_sprite[383U]);
    vlSelfRef.__Vtask_test_blitter__DOT__check__445__name = "sprite[383] = FF"s;
    vlSelfRef.test_blitter__DOT__test_num = ((IData)(1U) 
                                             + vlSelfRef.test_blitter__DOT__test_num);
    if (__Vtask_test_blitter__DOT__check__445__condition) {
        VL_WRITEF_NX("  PASS [%0d] %@\n",0,32,vlSelfRef.test_blitter__DOT__test_num,
                     -1,&(vlSelfRef.__Vtask_test_blitter__DOT__check__445__name));
        vlSelfRef.test_blitter__DOT__pass_count = ((IData)(1U) 
                                                   + vlSelfRef.test_blitter__DOT__pass_count);
    } else {
        VL_WRITEF_NX("  FAIL [%0d] %@\n",0,32,vlSelfRef.test_blitter__DOT__test_num,
                     -1,&(vlSelfRef.__Vtask_test_blitter__DOT__check__445__name));
        vlSelfRef.test_blitter__DOT__fail_count = ((IData)(1U) 
                                                   + vlSelfRef.test_blitter__DOT__fail_count);
    }
    __Vtask_test_blitter__DOT__check__446__condition 
        = (0U == vlSelfRef.test_blitter__DOT__sim_sprite[255U]);
    vlSelfRef.__Vtask_test_blitter__DOT__check__446__name = "sprite[255] untouched"s;
    vlSelfRef.test_blitter__DOT__test_num = ((IData)(1U) 
                                             + vlSelfRef.test_blitter__DOT__test_num);
    if (__Vtask_test_blitter__DOT__check__446__condition) {
        VL_WRITEF_NX("  PASS [%0d] %@\n",0,32,vlSelfRef.test_blitter__DOT__test_num,
                     -1,&(vlSelfRef.__Vtask_test_blitter__DOT__check__446__name));
        vlSelfRef.test_blitter__DOT__pass_count = ((IData)(1U) 
                                                   + vlSelfRef.test_blitter__DOT__pass_count);
    } else {
        VL_WRITEF_NX("  FAIL [%0d] %@\n",0,32,vlSelfRef.test_blitter__DOT__test_num,
                     -1,&(vlSelfRef.__Vtask_test_blitter__DOT__check__446__name));
        vlSelfRef.test_blitter__DOT__fail_count = ((IData)(1U) 
                                                   + vlSelfRef.test_blitter__DOT__fail_count);
    }
    VL_WRITEF_NX("Test: Row buffer overlapping copy\n",0);
    vlSelfRef.test_blitter__DOT__sim_ram[36864U] = 0xaaU;
    vlSelfRef.test_blitter__DOT__sim_ram[36865U] = 0xbbU;
    vlSelfRef.test_blitter__DOT__sim_ram[36866U] = 0xccU;
    vlSelfRef.test_blitter__DOT__sim_ram[36867U] = 0xddU;
    vlSelfRef.test_blitter__DOT__sim_ram[36868U] = 0xeeU;
    __Vtask_test_blitter__DOT__setup_copy__447__height = 1U;
    __Vtask_test_blitter__DOT__setup_copy__447__width = 4U;
    __Vtask_test_blitter__DOT__setup_copy__447__dst_stride = 5U;
    __Vtask_test_blitter__DOT__setup_copy__447__dst_addr = 0x00009001U;
    __Vtask_test_blitter__DOT__setup_copy__447__dst_space = 0U;
    __Vtask_test_blitter__DOT__setup_copy__447__src_stride = 5U;
    __Vtask_test_blitter__DOT__setup_copy__447__src_addr = 0x00009000U;
    __Vtask_test_blitter__DOT__setup_copy__447__src_space = 0U;
    __Vtask_test_blitter__DOT__blt_reg__448__data = 
        (0x000000ffU & __Vtask_test_blitter__DOT__setup_copy__447__src_space);
    __Vtask_test_blitter__DOT__blt_reg__448__off = 3U;
    __Vtask_test_blitter__DOT__write_reg__449__data 
        = __Vtask_test_blitter__DOT__blt_reg__448__data;
    __Vtask_test_blitter__DOT__write_reg__449__addr 
        = (0x0000ffffU & ((IData)(0xba83U) + __Vtask_test_blitter__DOT__blt_reg__448__off));
    Vtest_blitter___024root____VbeforeTrig_h603fc354__0(vlSelf, 
                                                        "@(posedge test_blitter.clk)");
    co_await vlSelfRef.__VtrigSched_h603fc354__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge test_blitter.clk)", 
                                                         "test_blitter.sv", 
                                                         113);
    vlSelfRef.test_blitter__DOT__cpu_addr = __Vtask_test_blitter__DOT__write_reg__449__addr;
    vlSelfRef.test_blitter__DOT__cpu_wdata = __Vtask_test_blitter__DOT__write_reg__449__data;
    vlSelfRef.test_blitter__DOT__cpu_we = 1U;
    Vtest_blitter___024root____VbeforeTrig_h603fc354__0(vlSelf, 
                                                        "@(posedge test_blitter.clk)");
    co_await vlSelfRef.__VtrigSched_h603fc354__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge test_blitter.clk)", 
                                                         "test_blitter.sv", 
                                                         115);
    vlSelfRef.test_blitter__DOT__cpu_we = 0U;
    __Vtask_test_blitter__DOT__blt_reg__450__data = 
        (0x000000ffU & __Vtask_test_blitter__DOT__setup_copy__447__dst_space);
    __Vtask_test_blitter__DOT__blt_reg__450__off = 4U;
    __Vtask_test_blitter__DOT__write_reg__451__data 
        = __Vtask_test_blitter__DOT__blt_reg__450__data;
    __Vtask_test_blitter__DOT__write_reg__451__addr 
        = (0x0000ffffU & ((IData)(0xba83U) + __Vtask_test_blitter__DOT__blt_reg__450__off));
    Vtest_blitter___024root____VbeforeTrig_h603fc354__0(vlSelf, 
                                                        "@(posedge test_blitter.clk)");
    co_await vlSelfRef.__VtrigSched_h603fc354__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge test_blitter.clk)", 
                                                         "test_blitter.sv", 
                                                         113);
    vlSelfRef.test_blitter__DOT__cpu_addr = __Vtask_test_blitter__DOT__write_reg__451__addr;
    vlSelfRef.test_blitter__DOT__cpu_wdata = __Vtask_test_blitter__DOT__write_reg__451__data;
    vlSelfRef.test_blitter__DOT__cpu_we = 1U;
    Vtest_blitter___024root____VbeforeTrig_h603fc354__0(vlSelf, 
                                                        "@(posedge test_blitter.clk)");
    co_await vlSelfRef.__VtrigSched_h603fc354__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge test_blitter.clk)", 
                                                         "test_blitter.sv", 
                                                         115);
    vlSelfRef.test_blitter__DOT__cpu_we = 0U;
    __Vtask_test_blitter__DOT__blt_reg__452__data = 
        (0x000000ffU & __Vtask_test_blitter__DOT__setup_copy__447__src_addr);
    __Vtask_test_blitter__DOT__blt_reg__452__off = 5U;
    __Vtask_test_blitter__DOT__write_reg__453__data 
        = __Vtask_test_blitter__DOT__blt_reg__452__data;
    __Vtask_test_blitter__DOT__write_reg__453__addr 
        = (0x0000ffffU & ((IData)(0xba83U) + __Vtask_test_blitter__DOT__blt_reg__452__off));
    Vtest_blitter___024root____VbeforeTrig_h603fc354__0(vlSelf, 
                                                        "@(posedge test_blitter.clk)");
    co_await vlSelfRef.__VtrigSched_h603fc354__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge test_blitter.clk)", 
                                                         "test_blitter.sv", 
                                                         113);
    vlSelfRef.test_blitter__DOT__cpu_addr = __Vtask_test_blitter__DOT__write_reg__453__addr;
    vlSelfRef.test_blitter__DOT__cpu_wdata = __Vtask_test_blitter__DOT__write_reg__453__data;
    vlSelfRef.test_blitter__DOT__cpu_we = 1U;
    Vtest_blitter___024root____VbeforeTrig_h603fc354__0(vlSelf, 
                                                        "@(posedge test_blitter.clk)");
    co_await vlSelfRef.__VtrigSched_h603fc354__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge test_blitter.clk)", 
                                                         "test_blitter.sv", 
                                                         115);
    vlSelfRef.test_blitter__DOT__cpu_we = 0U;
    __Vtask_test_blitter__DOT__blt_reg__454__data = 
        (0x000000ffU & (__Vtask_test_blitter__DOT__setup_copy__447__src_addr 
                        >> 8U));
    __Vtask_test_blitter__DOT__blt_reg__454__off = 6U;
    __Vtask_test_blitter__DOT__write_reg__455__data 
        = __Vtask_test_blitter__DOT__blt_reg__454__data;
    __Vtask_test_blitter__DOT__write_reg__455__addr 
        = (0x0000ffffU & ((IData)(0xba83U) + __Vtask_test_blitter__DOT__blt_reg__454__off));
    Vtest_blitter___024root____VbeforeTrig_h603fc354__0(vlSelf, 
                                                        "@(posedge test_blitter.clk)");
    co_await vlSelfRef.__VtrigSched_h603fc354__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge test_blitter.clk)", 
                                                         "test_blitter.sv", 
                                                         113);
    vlSelfRef.test_blitter__DOT__cpu_addr = __Vtask_test_blitter__DOT__write_reg__455__addr;
    vlSelfRef.test_blitter__DOT__cpu_wdata = __Vtask_test_blitter__DOT__write_reg__455__data;
    vlSelfRef.test_blitter__DOT__cpu_we = 1U;
    Vtest_blitter___024root____VbeforeTrig_h603fc354__0(vlSelf, 
                                                        "@(posedge test_blitter.clk)");
    co_await vlSelfRef.__VtrigSched_h603fc354__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge test_blitter.clk)", 
                                                         "test_blitter.sv", 
                                                         115);
    vlSelfRef.test_blitter__DOT__cpu_we = 0U;
    __Vtask_test_blitter__DOT__blt_reg__456__data = 
        (0x000000ffU & (__Vtask_test_blitter__DOT__setup_copy__447__src_addr 
                        >> 0x10U));
    __Vtask_test_blitter__DOT__blt_reg__456__off = 7U;
    __Vtask_test_blitter__DOT__write_reg__457__data 
        = __Vtask_test_blitter__DOT__blt_reg__456__data;
    __Vtask_test_blitter__DOT__write_reg__457__addr 
        = (0x0000ffffU & ((IData)(0xba83U) + __Vtask_test_blitter__DOT__blt_reg__456__off));
    Vtest_blitter___024root____VbeforeTrig_h603fc354__0(vlSelf, 
                                                        "@(posedge test_blitter.clk)");
    co_await vlSelfRef.__VtrigSched_h603fc354__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge test_blitter.clk)", 
                                                         "test_blitter.sv", 
                                                         113);
    vlSelfRef.test_blitter__DOT__cpu_addr = __Vtask_test_blitter__DOT__write_reg__457__addr;
    vlSelfRef.test_blitter__DOT__cpu_wdata = __Vtask_test_blitter__DOT__write_reg__457__data;
    vlSelfRef.test_blitter__DOT__cpu_we = 1U;
    Vtest_blitter___024root____VbeforeTrig_h603fc354__0(vlSelf, 
                                                        "@(posedge test_blitter.clk)");
    co_await vlSelfRef.__VtrigSched_h603fc354__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge test_blitter.clk)", 
                                                         "test_blitter.sv", 
                                                         115);
    vlSelfRef.test_blitter__DOT__cpu_we = 0U;
    __Vtask_test_blitter__DOT__blt_reg__458__data = 
        (0x000000ffU & __Vtask_test_blitter__DOT__setup_copy__447__dst_addr);
    __Vtask_test_blitter__DOT__blt_reg__458__off = 8U;
    __Vtask_test_blitter__DOT__write_reg__459__data 
        = __Vtask_test_blitter__DOT__blt_reg__458__data;
    __Vtask_test_blitter__DOT__write_reg__459__addr 
        = (0x0000ffffU & ((IData)(0xba83U) + __Vtask_test_blitter__DOT__blt_reg__458__off));
    Vtest_blitter___024root____VbeforeTrig_h603fc354__0(vlSelf, 
                                                        "@(posedge test_blitter.clk)");
    co_await vlSelfRef.__VtrigSched_h603fc354__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge test_blitter.clk)", 
                                                         "test_blitter.sv", 
                                                         113);
    vlSelfRef.test_blitter__DOT__cpu_addr = __Vtask_test_blitter__DOT__write_reg__459__addr;
    vlSelfRef.test_blitter__DOT__cpu_wdata = __Vtask_test_blitter__DOT__write_reg__459__data;
    vlSelfRef.test_blitter__DOT__cpu_we = 1U;
    Vtest_blitter___024root____VbeforeTrig_h603fc354__0(vlSelf, 
                                                        "@(posedge test_blitter.clk)");
    co_await vlSelfRef.__VtrigSched_h603fc354__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge test_blitter.clk)", 
                                                         "test_blitter.sv", 
                                                         115);
    vlSelfRef.test_blitter__DOT__cpu_we = 0U;
    __Vtask_test_blitter__DOT__blt_reg__460__data = 
        (0x000000ffU & (__Vtask_test_blitter__DOT__setup_copy__447__dst_addr 
                        >> 8U));
    __Vtask_test_blitter__DOT__blt_reg__460__off = 9U;
    __Vtask_test_blitter__DOT__write_reg__461__data 
        = __Vtask_test_blitter__DOT__blt_reg__460__data;
    __Vtask_test_blitter__DOT__write_reg__461__addr 
        = (0x0000ffffU & ((IData)(0xba83U) + __Vtask_test_blitter__DOT__blt_reg__460__off));
    Vtest_blitter___024root____VbeforeTrig_h603fc354__0(vlSelf, 
                                                        "@(posedge test_blitter.clk)");
    co_await vlSelfRef.__VtrigSched_h603fc354__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge test_blitter.clk)", 
                                                         "test_blitter.sv", 
                                                         113);
    vlSelfRef.test_blitter__DOT__cpu_addr = __Vtask_test_blitter__DOT__write_reg__461__addr;
    vlSelfRef.test_blitter__DOT__cpu_wdata = __Vtask_test_blitter__DOT__write_reg__461__data;
    vlSelfRef.test_blitter__DOT__cpu_we = 1U;
    Vtest_blitter___024root____VbeforeTrig_h603fc354__0(vlSelf, 
                                                        "@(posedge test_blitter.clk)");
    co_await vlSelfRef.__VtrigSched_h603fc354__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge test_blitter.clk)", 
                                                         "test_blitter.sv", 
                                                         115);
    vlSelfRef.test_blitter__DOT__cpu_we = 0U;
    __Vtask_test_blitter__DOT__blt_reg__462__data = 
        (0x000000ffU & (__Vtask_test_blitter__DOT__setup_copy__447__dst_addr 
                        >> 0x10U));
    __Vtask_test_blitter__DOT__blt_reg__462__off = 0x0000000aU;
    __Vtask_test_blitter__DOT__write_reg__463__data 
        = __Vtask_test_blitter__DOT__blt_reg__462__data;
    __Vtask_test_blitter__DOT__write_reg__463__addr 
        = (0x0000ffffU & ((IData)(0xba83U) + __Vtask_test_blitter__DOT__blt_reg__462__off));
    Vtest_blitter___024root____VbeforeTrig_h603fc354__0(vlSelf, 
                                                        "@(posedge test_blitter.clk)");
    co_await vlSelfRef.__VtrigSched_h603fc354__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge test_blitter.clk)", 
                                                         "test_blitter.sv", 
                                                         113);
    vlSelfRef.test_blitter__DOT__cpu_addr = __Vtask_test_blitter__DOT__write_reg__463__addr;
    vlSelfRef.test_blitter__DOT__cpu_wdata = __Vtask_test_blitter__DOT__write_reg__463__data;
    vlSelfRef.test_blitter__DOT__cpu_we = 1U;
    Vtest_blitter___024root____VbeforeTrig_h603fc354__0(vlSelf, 
                                                        "@(posedge test_blitter.clk)");
    co_await vlSelfRef.__VtrigSched_h603fc354__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge test_blitter.clk)", 
                                                         "test_blitter.sv", 
                                                         115);
    vlSelfRef.test_blitter__DOT__cpu_we = 0U;
    __Vtask_test_blitter__DOT__blt_reg__464__data = 
        (0x000000ffU & __Vtask_test_blitter__DOT__setup_copy__447__width);
    __Vtask_test_blitter__DOT__blt_reg__464__off = 0x0000000bU;
    __Vtask_test_blitter__DOT__write_reg__465__data 
        = __Vtask_test_blitter__DOT__blt_reg__464__data;
    __Vtask_test_blitter__DOT__write_reg__465__addr 
        = (0x0000ffffU & ((IData)(0xba83U) + __Vtask_test_blitter__DOT__blt_reg__464__off));
    Vtest_blitter___024root____VbeforeTrig_h603fc354__0(vlSelf, 
                                                        "@(posedge test_blitter.clk)");
    co_await vlSelfRef.__VtrigSched_h603fc354__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge test_blitter.clk)", 
                                                         "test_blitter.sv", 
                                                         113);
    vlSelfRef.test_blitter__DOT__cpu_addr = __Vtask_test_blitter__DOT__write_reg__465__addr;
    vlSelfRef.test_blitter__DOT__cpu_wdata = __Vtask_test_blitter__DOT__write_reg__465__data;
    vlSelfRef.test_blitter__DOT__cpu_we = 1U;
    Vtest_blitter___024root____VbeforeTrig_h603fc354__0(vlSelf, 
                                                        "@(posedge test_blitter.clk)");
    co_await vlSelfRef.__VtrigSched_h603fc354__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge test_blitter.clk)", 
                                                         "test_blitter.sv", 
                                                         115);
    vlSelfRef.test_blitter__DOT__cpu_we = 0U;
    __Vtask_test_blitter__DOT__blt_reg__466__data = 
        (0x000000ffU & (__Vtask_test_blitter__DOT__setup_copy__447__width 
                        >> 8U));
    __Vtask_test_blitter__DOT__blt_reg__466__off = 0x0000000cU;
    __Vtask_test_blitter__DOT__write_reg__467__data 
        = __Vtask_test_blitter__DOT__blt_reg__466__data;
    __Vtask_test_blitter__DOT__write_reg__467__addr 
        = (0x0000ffffU & ((IData)(0xba83U) + __Vtask_test_blitter__DOT__blt_reg__466__off));
    Vtest_blitter___024root____VbeforeTrig_h603fc354__0(vlSelf, 
                                                        "@(posedge test_blitter.clk)");
    co_await vlSelfRef.__VtrigSched_h603fc354__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge test_blitter.clk)", 
                                                         "test_blitter.sv", 
                                                         113);
    vlSelfRef.test_blitter__DOT__cpu_addr = __Vtask_test_blitter__DOT__write_reg__467__addr;
    vlSelfRef.test_blitter__DOT__cpu_wdata = __Vtask_test_blitter__DOT__write_reg__467__data;
    vlSelfRef.test_blitter__DOT__cpu_we = 1U;
    Vtest_blitter___024root____VbeforeTrig_h603fc354__0(vlSelf, 
                                                        "@(posedge test_blitter.clk)");
    co_await vlSelfRef.__VtrigSched_h603fc354__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge test_blitter.clk)", 
                                                         "test_blitter.sv", 
                                                         115);
    vlSelfRef.test_blitter__DOT__cpu_we = 0U;
    __Vtask_test_blitter__DOT__blt_reg__468__data = 
        (0x000000ffU & __Vtask_test_blitter__DOT__setup_copy__447__height);
    __Vtask_test_blitter__DOT__blt_reg__468__off = 0x0000000dU;
    __Vtask_test_blitter__DOT__write_reg__469__data 
        = __Vtask_test_blitter__DOT__blt_reg__468__data;
    __Vtask_test_blitter__DOT__write_reg__469__addr 
        = (0x0000ffffU & ((IData)(0xba83U) + __Vtask_test_blitter__DOT__blt_reg__468__off));
    Vtest_blitter___024root____VbeforeTrig_h603fc354__0(vlSelf, 
                                                        "@(posedge test_blitter.clk)");
    co_await vlSelfRef.__VtrigSched_h603fc354__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge test_blitter.clk)", 
                                                         "test_blitter.sv", 
                                                         113);
    vlSelfRef.test_blitter__DOT__cpu_addr = __Vtask_test_blitter__DOT__write_reg__469__addr;
    vlSelfRef.test_blitter__DOT__cpu_wdata = __Vtask_test_blitter__DOT__write_reg__469__data;
    vlSelfRef.test_blitter__DOT__cpu_we = 1U;
    Vtest_blitter___024root____VbeforeTrig_h603fc354__0(vlSelf, 
                                                        "@(posedge test_blitter.clk)");
    co_await vlSelfRef.__VtrigSched_h603fc354__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge test_blitter.clk)", 
                                                         "test_blitter.sv", 
                                                         115);
    vlSelfRef.test_blitter__DOT__cpu_we = 0U;
    __Vtask_test_blitter__DOT__blt_reg__470__data = 
        (0x000000ffU & (__Vtask_test_blitter__DOT__setup_copy__447__height 
                        >> 8U));
    __Vtask_test_blitter__DOT__blt_reg__470__off = 0x0000000eU;
    __Vtask_test_blitter__DOT__write_reg__471__data 
        = __Vtask_test_blitter__DOT__blt_reg__470__data;
    __Vtask_test_blitter__DOT__write_reg__471__addr 
        = (0x0000ffffU & ((IData)(0xba83U) + __Vtask_test_blitter__DOT__blt_reg__470__off));
    Vtest_blitter___024root____VbeforeTrig_h603fc354__0(vlSelf, 
                                                        "@(posedge test_blitter.clk)");
    co_await vlSelfRef.__VtrigSched_h603fc354__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge test_blitter.clk)", 
                                                         "test_blitter.sv", 
                                                         113);
    vlSelfRef.test_blitter__DOT__cpu_addr = __Vtask_test_blitter__DOT__write_reg__471__addr;
    vlSelfRef.test_blitter__DOT__cpu_wdata = __Vtask_test_blitter__DOT__write_reg__471__data;
    vlSelfRef.test_blitter__DOT__cpu_we = 1U;
    Vtest_blitter___024root____VbeforeTrig_h603fc354__0(vlSelf, 
                                                        "@(posedge test_blitter.clk)");
    co_await vlSelfRef.__VtrigSched_h603fc354__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge test_blitter.clk)", 
                                                         "test_blitter.sv", 
                                                         115);
    vlSelfRef.test_blitter__DOT__cpu_we = 0U;
    __Vtask_test_blitter__DOT__blt_reg__472__data = 
        (0x000000ffU & __Vtask_test_blitter__DOT__setup_copy__447__src_stride);
    __Vtask_test_blitter__DOT__blt_reg__472__off = 0x0000000fU;
    __Vtask_test_blitter__DOT__write_reg__473__data 
        = __Vtask_test_blitter__DOT__blt_reg__472__data;
    __Vtask_test_blitter__DOT__write_reg__473__addr 
        = (0x0000ffffU & ((IData)(0xba83U) + __Vtask_test_blitter__DOT__blt_reg__472__off));
    Vtest_blitter___024root____VbeforeTrig_h603fc354__0(vlSelf, 
                                                        "@(posedge test_blitter.clk)");
    co_await vlSelfRef.__VtrigSched_h603fc354__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge test_blitter.clk)", 
                                                         "test_blitter.sv", 
                                                         113);
    vlSelfRef.test_blitter__DOT__cpu_addr = __Vtask_test_blitter__DOT__write_reg__473__addr;
    vlSelfRef.test_blitter__DOT__cpu_wdata = __Vtask_test_blitter__DOT__write_reg__473__data;
    vlSelfRef.test_blitter__DOT__cpu_we = 1U;
    Vtest_blitter___024root____VbeforeTrig_h603fc354__0(vlSelf, 
                                                        "@(posedge test_blitter.clk)");
    co_await vlSelfRef.__VtrigSched_h603fc354__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge test_blitter.clk)", 
                                                         "test_blitter.sv", 
                                                         115);
    vlSelfRef.test_blitter__DOT__cpu_we = 0U;
    __Vtask_test_blitter__DOT__blt_reg__474__data = 
        (0x000000ffU & (__Vtask_test_blitter__DOT__setup_copy__447__src_stride 
                        >> 8U));
    __Vtask_test_blitter__DOT__blt_reg__474__off = 0x00000010U;
    __Vtask_test_blitter__DOT__write_reg__475__data 
        = __Vtask_test_blitter__DOT__blt_reg__474__data;
    __Vtask_test_blitter__DOT__write_reg__475__addr 
        = (0x0000ffffU & ((IData)(0xba83U) + __Vtask_test_blitter__DOT__blt_reg__474__off));
    Vtest_blitter___024root____VbeforeTrig_h603fc354__0(vlSelf, 
                                                        "@(posedge test_blitter.clk)");
    co_await vlSelfRef.__VtrigSched_h603fc354__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge test_blitter.clk)", 
                                                         "test_blitter.sv", 
                                                         113);
    vlSelfRef.test_blitter__DOT__cpu_addr = __Vtask_test_blitter__DOT__write_reg__475__addr;
    vlSelfRef.test_blitter__DOT__cpu_wdata = __Vtask_test_blitter__DOT__write_reg__475__data;
    vlSelfRef.test_blitter__DOT__cpu_we = 1U;
    Vtest_blitter___024root____VbeforeTrig_h603fc354__0(vlSelf, 
                                                        "@(posedge test_blitter.clk)");
    co_await vlSelfRef.__VtrigSched_h603fc354__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge test_blitter.clk)", 
                                                         "test_blitter.sv", 
                                                         115);
    vlSelfRef.test_blitter__DOT__cpu_we = 0U;
    __Vtask_test_blitter__DOT__blt_reg__476__data = 
        (0x000000ffU & __Vtask_test_blitter__DOT__setup_copy__447__dst_stride);
    __Vtask_test_blitter__DOT__blt_reg__476__off = 0x00000011U;
    __Vtask_test_blitter__DOT__write_reg__477__data 
        = __Vtask_test_blitter__DOT__blt_reg__476__data;
    __Vtask_test_blitter__DOT__write_reg__477__addr 
        = (0x0000ffffU & ((IData)(0xba83U) + __Vtask_test_blitter__DOT__blt_reg__476__off));
    Vtest_blitter___024root____VbeforeTrig_h603fc354__0(vlSelf, 
                                                        "@(posedge test_blitter.clk)");
    co_await vlSelfRef.__VtrigSched_h603fc354__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge test_blitter.clk)", 
                                                         "test_blitter.sv", 
                                                         113);
    vlSelfRef.test_blitter__DOT__cpu_addr = __Vtask_test_blitter__DOT__write_reg__477__addr;
    vlSelfRef.test_blitter__DOT__cpu_wdata = __Vtask_test_blitter__DOT__write_reg__477__data;
    vlSelfRef.test_blitter__DOT__cpu_we = 1U;
    Vtest_blitter___024root____VbeforeTrig_h603fc354__0(vlSelf, 
                                                        "@(posedge test_blitter.clk)");
    co_await vlSelfRef.__VtrigSched_h603fc354__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge test_blitter.clk)", 
                                                         "test_blitter.sv", 
                                                         115);
    vlSelfRef.test_blitter__DOT__cpu_we = 0U;
    __Vtask_test_blitter__DOT__blt_reg__478__data = 
        (0x000000ffU & (__Vtask_test_blitter__DOT__setup_copy__447__dst_stride 
                        >> 8U));
    __Vtask_test_blitter__DOT__blt_reg__478__off = 0x00000012U;
    __Vtask_test_blitter__DOT__write_reg__479__data 
        = __Vtask_test_blitter__DOT__blt_reg__478__data;
    __Vtask_test_blitter__DOT__write_reg__479__addr 
        = (0x0000ffffU & ((IData)(0xba83U) + __Vtask_test_blitter__DOT__blt_reg__478__off));
    Vtest_blitter___024root____VbeforeTrig_h603fc354__0(vlSelf, 
                                                        "@(posedge test_blitter.clk)");
    co_await vlSelfRef.__VtrigSched_h603fc354__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge test_blitter.clk)", 
                                                         "test_blitter.sv", 
                                                         113);
    vlSelfRef.test_blitter__DOT__cpu_addr = __Vtask_test_blitter__DOT__write_reg__479__addr;
    vlSelfRef.test_blitter__DOT__cpu_wdata = __Vtask_test_blitter__DOT__write_reg__479__data;
    vlSelfRef.test_blitter__DOT__cpu_we = 1U;
    Vtest_blitter___024root____VbeforeTrig_h603fc354__0(vlSelf, 
                                                        "@(posedge test_blitter.clk)");
    co_await vlSelfRef.__VtrigSched_h603fc354__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge test_blitter.clk)", 
                                                         "test_blitter.sv", 
                                                         115);
    vlSelfRef.test_blitter__DOT__cpu_we = 0U;
    __Vtask_test_blitter__DOT__blt_reg__480__data = 0U;
    __Vtask_test_blitter__DOT__blt_reg__480__off = 0x00000013U;
    __Vtask_test_blitter__DOT__write_reg__481__data 
        = __Vtask_test_blitter__DOT__blt_reg__480__data;
    __Vtask_test_blitter__DOT__write_reg__481__addr 
        = (0x0000ffffU & ((IData)(0xba83U) + __Vtask_test_blitter__DOT__blt_reg__480__off));
    Vtest_blitter___024root____VbeforeTrig_h603fc354__0(vlSelf, 
                                                        "@(posedge test_blitter.clk)");
    co_await vlSelfRef.__VtrigSched_h603fc354__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge test_blitter.clk)", 
                                                         "test_blitter.sv", 
                                                         113);
    vlSelfRef.test_blitter__DOT__cpu_addr = __Vtask_test_blitter__DOT__write_reg__481__addr;
    vlSelfRef.test_blitter__DOT__cpu_wdata = __Vtask_test_blitter__DOT__write_reg__481__data;
    vlSelfRef.test_blitter__DOT__cpu_we = 1U;
    Vtest_blitter___024root____VbeforeTrig_h603fc354__0(vlSelf, 
                                                        "@(posedge test_blitter.clk)");
    co_await vlSelfRef.__VtrigSched_h603fc354__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge test_blitter.clk)", 
                                                         "test_blitter.sv", 
                                                         115);
    vlSelfRef.test_blitter__DOT__cpu_we = 0U;
    __Vtask_test_blitter__DOT__blt_reg__483__data = 1U;
    __Vtask_test_blitter__DOT__blt_reg__483__off = 0U;
    __Vtask_test_blitter__DOT__write_reg__484__data 
        = __Vtask_test_blitter__DOT__blt_reg__483__data;
    __Vtask_test_blitter__DOT__write_reg__484__addr 
        = (0x0000ffffU & ((IData)(0xba83U) + __Vtask_test_blitter__DOT__blt_reg__483__off));
    Vtest_blitter___024root____VbeforeTrig_h603fc354__0(vlSelf, 
                                                        "@(posedge test_blitter.clk)");
    co_await vlSelfRef.__VtrigSched_h603fc354__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge test_blitter.clk)", 
                                                         "test_blitter.sv", 
                                                         113);
    vlSelfRef.test_blitter__DOT__cpu_addr = __Vtask_test_blitter__DOT__write_reg__484__addr;
    vlSelfRef.test_blitter__DOT__cpu_wdata = __Vtask_test_blitter__DOT__write_reg__484__data;
    vlSelfRef.test_blitter__DOT__cpu_we = 1U;
    Vtest_blitter___024root____VbeforeTrig_h603fc354__0(vlSelf, 
                                                        "@(posedge test_blitter.clk)");
    co_await vlSelfRef.__VtrigSched_h603fc354__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge test_blitter.clk)", 
                                                         "test_blitter.sv", 
                                                         115);
    vlSelfRef.test_blitter__DOT__cpu_we = 0U;
    __Vtask_test_blitter__DOT__wait_blt_done__485__test_blitter__DOT__unnamedblk1_1__DOT____Vrepeat0 = 0;
    __Vtask_test_blitter__DOT__wait_blt_done__485__test_blitter__DOT__unnamedblk1_2__DOT____Vrepeat1 = 0;
    __Vtask_test_blitter__DOT__wait_blt_done__485__test_blitter__DOT__unnamedblk1_1__DOT____Vrepeat0 = 2U;
    while (VL_LTS_III(32, 0U, __Vtask_test_blitter__DOT__wait_blt_done__485__test_blitter__DOT__unnamedblk1_1__DOT____Vrepeat0)) {
        Vtest_blitter___024root____VbeforeTrig_h603fc354__0(vlSelf, 
                                                            "@(posedge test_blitter.clk)");
        co_await vlSelfRef.__VtrigSched_h603fc354__0.trigger(0U, 
                                                             nullptr, 
                                                             "@(posedge test_blitter.clk)", 
                                                             "test_blitter.sv", 
                                                             128);
        __Vtask_test_blitter__DOT__wait_blt_done__485__test_blitter__DOT__unnamedblk1_1__DOT____Vrepeat0 
            = (__Vtask_test_blitter__DOT__wait_blt_done__485__test_blitter__DOT__unnamedblk1_1__DOT____Vrepeat0 
               - (IData)(1U));
    }
    while (((0U != (IData)(vlSelfRef.test_blitter__DOT__dut__DOT__state)) 
            & (6U != (IData)(vlSelfRef.test_blitter__DOT__dut__DOT__state)))) {
        Vtest_blitter___024root____VbeforeTrig_h603fc354__0(vlSelf, 
                                                            "@(posedge test_blitter.clk)");
        co_await vlSelfRef.__VtrigSched_h603fc354__0.trigger(0U, 
                                                             nullptr, 
                                                             "@(posedge test_blitter.clk)", 
                                                             "test_blitter.sv", 
                                                             130);
    }
    __Vtask_test_blitter__DOT__wait_blt_done__485__test_blitter__DOT__unnamedblk1_2__DOT____Vrepeat1 = 2U;
    while (VL_LTS_III(32, 0U, __Vtask_test_blitter__DOT__wait_blt_done__485__test_blitter__DOT__unnamedblk1_2__DOT____Vrepeat1)) {
        Vtest_blitter___024root____VbeforeTrig_h603fc354__0(vlSelf, 
                                                            "@(posedge test_blitter.clk)");
        co_await vlSelfRef.__VtrigSched_h603fc354__0.trigger(0U, 
                                                             nullptr, 
                                                             "@(posedge test_blitter.clk)", 
                                                             "test_blitter.sv", 
                                                             131);
        __Vtask_test_blitter__DOT__wait_blt_done__485__test_blitter__DOT__unnamedblk1_2__DOT____Vrepeat1 
            = (__Vtask_test_blitter__DOT__wait_blt_done__485__test_blitter__DOT__unnamedblk1_2__DOT____Vrepeat1 
               - (IData)(1U));
    }
    __Vtask_test_blitter__DOT__check__486__condition 
        = (2U == vlSelfRef.test_blitter__DOT__dut__DOT__regs[1U]);
    vlSelfRef.__Vtask_test_blitter__DOT__check__486__name = "overlap: status ok"s;
    vlSelfRef.test_blitter__DOT__test_num = ((IData)(1U) 
                                             + vlSelfRef.test_blitter__DOT__test_num);
    if (__Vtask_test_blitter__DOT__check__486__condition) {
        VL_WRITEF_NX("  PASS [%0d] %@\n",0,32,vlSelfRef.test_blitter__DOT__test_num,
                     -1,&(vlSelfRef.__Vtask_test_blitter__DOT__check__486__name));
        vlSelfRef.test_blitter__DOT__pass_count = ((IData)(1U) 
                                                   + vlSelfRef.test_blitter__DOT__pass_count);
    } else {
        VL_WRITEF_NX("  FAIL [%0d] %@\n",0,32,vlSelfRef.test_blitter__DOT__test_num,
                     -1,&(vlSelfRef.__Vtask_test_blitter__DOT__check__486__name));
        vlSelfRef.test_blitter__DOT__fail_count = ((IData)(1U) 
                                                   + vlSelfRef.test_blitter__DOT__fail_count);
    }
    __Vtask_test_blitter__DOT__check__487__condition 
        = (0xaaU == vlSelfRef.test_blitter__DOT__sim_ram[36865U]);
    vlSelfRef.__Vtask_test_blitter__DOT__check__487__name = "overlap: dst[0] = AA"s;
    vlSelfRef.test_blitter__DOT__test_num = ((IData)(1U) 
                                             + vlSelfRef.test_blitter__DOT__test_num);
    if (__Vtask_test_blitter__DOT__check__487__condition) {
        VL_WRITEF_NX("  PASS [%0d] %@\n",0,32,vlSelfRef.test_blitter__DOT__test_num,
                     -1,&(vlSelfRef.__Vtask_test_blitter__DOT__check__487__name));
        vlSelfRef.test_blitter__DOT__pass_count = ((IData)(1U) 
                                                   + vlSelfRef.test_blitter__DOT__pass_count);
    } else {
        VL_WRITEF_NX("  FAIL [%0d] %@\n",0,32,vlSelfRef.test_blitter__DOT__test_num,
                     -1,&(vlSelfRef.__Vtask_test_blitter__DOT__check__487__name));
        vlSelfRef.test_blitter__DOT__fail_count = ((IData)(1U) 
                                                   + vlSelfRef.test_blitter__DOT__fail_count);
    }
    __Vtask_test_blitter__DOT__check__488__condition 
        = (0xbbU == vlSelfRef.test_blitter__DOT__sim_ram[36866U]);
    vlSelfRef.__Vtask_test_blitter__DOT__check__488__name = "overlap: dst[1] = BB"s;
    vlSelfRef.test_blitter__DOT__test_num = ((IData)(1U) 
                                             + vlSelfRef.test_blitter__DOT__test_num);
    if (__Vtask_test_blitter__DOT__check__488__condition) {
        VL_WRITEF_NX("  PASS [%0d] %@\n",0,32,vlSelfRef.test_blitter__DOT__test_num,
                     -1,&(vlSelfRef.__Vtask_test_blitter__DOT__check__488__name));
        vlSelfRef.test_blitter__DOT__pass_count = ((IData)(1U) 
                                                   + vlSelfRef.test_blitter__DOT__pass_count);
    } else {
        VL_WRITEF_NX("  FAIL [%0d] %@\n",0,32,vlSelfRef.test_blitter__DOT__test_num,
                     -1,&(vlSelfRef.__Vtask_test_blitter__DOT__check__488__name));
        vlSelfRef.test_blitter__DOT__fail_count = ((IData)(1U) 
                                                   + vlSelfRef.test_blitter__DOT__fail_count);
    }
    __Vtask_test_blitter__DOT__check__489__condition 
        = (0xccU == vlSelfRef.test_blitter__DOT__sim_ram[36867U]);
    vlSelfRef.__Vtask_test_blitter__DOT__check__489__name = "overlap: dst[2] = CC"s;
    vlSelfRef.test_blitter__DOT__test_num = ((IData)(1U) 
                                             + vlSelfRef.test_blitter__DOT__test_num);
    if (__Vtask_test_blitter__DOT__check__489__condition) {
        VL_WRITEF_NX("  PASS [%0d] %@\n",0,32,vlSelfRef.test_blitter__DOT__test_num,
                     -1,&(vlSelfRef.__Vtask_test_blitter__DOT__check__489__name));
        vlSelfRef.test_blitter__DOT__pass_count = ((IData)(1U) 
                                                   + vlSelfRef.test_blitter__DOT__pass_count);
    } else {
        VL_WRITEF_NX("  FAIL [%0d] %@\n",0,32,vlSelfRef.test_blitter__DOT__test_num,
                     -1,&(vlSelfRef.__Vtask_test_blitter__DOT__check__489__name));
        vlSelfRef.test_blitter__DOT__fail_count = ((IData)(1U) 
                                                   + vlSelfRef.test_blitter__DOT__fail_count);
    }
    __Vtask_test_blitter__DOT__check__490__condition 
        = (0xddU == vlSelfRef.test_blitter__DOT__sim_ram[36868U]);
    vlSelfRef.__Vtask_test_blitter__DOT__check__490__name = "overlap: dst[3] = DD"s;
    vlSelfRef.test_blitter__DOT__test_num = ((IData)(1U) 
                                             + vlSelfRef.test_blitter__DOT__test_num);
    if (__Vtask_test_blitter__DOT__check__490__condition) {
        VL_WRITEF_NX("  PASS [%0d] %@\n",0,32,vlSelfRef.test_blitter__DOT__test_num,
                     -1,&(vlSelfRef.__Vtask_test_blitter__DOT__check__490__name));
        vlSelfRef.test_blitter__DOT__pass_count = ((IData)(1U) 
                                                   + vlSelfRef.test_blitter__DOT__pass_count);
    } else {
        VL_WRITEF_NX("  FAIL [%0d] %@\n",0,32,vlSelfRef.test_blitter__DOT__test_num,
                     -1,&(vlSelfRef.__Vtask_test_blitter__DOT__check__490__name));
        vlSelfRef.test_blitter__DOT__fail_count = ((IData)(1U) 
                                                   + vlSelfRef.test_blitter__DOT__fail_count);
    }
    VL_WRITEF_NX("Test: Range validation\n",0);
    __Vtask_test_blitter__DOT__setup_copy__491__height = 1U;
    __Vtask_test_blitter__DOT__setup_copy__491__width = 0x00000014U;
    __Vtask_test_blitter__DOT__setup_copy__491__dst_stride = 0x00000014U;
    __Vtask_test_blitter__DOT__setup_copy__491__dst_addr = 0x0000a000U;
    __Vtask_test_blitter__DOT__setup_copy__491__dst_space = 0U;
    __Vtask_test_blitter__DOT__setup_copy__491__src_stride = 0x000007d0U;
    __Vtask_test_blitter__DOT__setup_copy__491__src_addr = 0x000007c6U;
    __Vtask_test_blitter__DOT__setup_copy__491__src_space = 1U;
    __Vtask_test_blitter__DOT__blt_reg__492__data = 
        (0x000000ffU & __Vtask_test_blitter__DOT__setup_copy__491__src_space);
    __Vtask_test_blitter__DOT__blt_reg__492__off = 3U;
    __Vtask_test_blitter__DOT__write_reg__493__data 
        = __Vtask_test_blitter__DOT__blt_reg__492__data;
    __Vtask_test_blitter__DOT__write_reg__493__addr 
        = (0x0000ffffU & ((IData)(0xba83U) + __Vtask_test_blitter__DOT__blt_reg__492__off));
    Vtest_blitter___024root____VbeforeTrig_h603fc354__0(vlSelf, 
                                                        "@(posedge test_blitter.clk)");
    co_await vlSelfRef.__VtrigSched_h603fc354__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge test_blitter.clk)", 
                                                         "test_blitter.sv", 
                                                         113);
    vlSelfRef.test_blitter__DOT__cpu_addr = __Vtask_test_blitter__DOT__write_reg__493__addr;
    vlSelfRef.test_blitter__DOT__cpu_wdata = __Vtask_test_blitter__DOT__write_reg__493__data;
    vlSelfRef.test_blitter__DOT__cpu_we = 1U;
    Vtest_blitter___024root____VbeforeTrig_h603fc354__0(vlSelf, 
                                                        "@(posedge test_blitter.clk)");
    co_await vlSelfRef.__VtrigSched_h603fc354__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge test_blitter.clk)", 
                                                         "test_blitter.sv", 
                                                         115);
    vlSelfRef.test_blitter__DOT__cpu_we = 0U;
    __Vtask_test_blitter__DOT__blt_reg__494__data = 
        (0x000000ffU & __Vtask_test_blitter__DOT__setup_copy__491__dst_space);
    __Vtask_test_blitter__DOT__blt_reg__494__off = 4U;
    __Vtask_test_blitter__DOT__write_reg__495__data 
        = __Vtask_test_blitter__DOT__blt_reg__494__data;
    __Vtask_test_blitter__DOT__write_reg__495__addr 
        = (0x0000ffffU & ((IData)(0xba83U) + __Vtask_test_blitter__DOT__blt_reg__494__off));
    Vtest_blitter___024root____VbeforeTrig_h603fc354__0(vlSelf, 
                                                        "@(posedge test_blitter.clk)");
    co_await vlSelfRef.__VtrigSched_h603fc354__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge test_blitter.clk)", 
                                                         "test_blitter.sv", 
                                                         113);
    vlSelfRef.test_blitter__DOT__cpu_addr = __Vtask_test_blitter__DOT__write_reg__495__addr;
    vlSelfRef.test_blitter__DOT__cpu_wdata = __Vtask_test_blitter__DOT__write_reg__495__data;
    vlSelfRef.test_blitter__DOT__cpu_we = 1U;
    Vtest_blitter___024root____VbeforeTrig_h603fc354__0(vlSelf, 
                                                        "@(posedge test_blitter.clk)");
    co_await vlSelfRef.__VtrigSched_h603fc354__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge test_blitter.clk)", 
                                                         "test_blitter.sv", 
                                                         115);
    vlSelfRef.test_blitter__DOT__cpu_we = 0U;
    __Vtask_test_blitter__DOT__blt_reg__496__data = 
        (0x000000ffU & __Vtask_test_blitter__DOT__setup_copy__491__src_addr);
    __Vtask_test_blitter__DOT__blt_reg__496__off = 5U;
    __Vtask_test_blitter__DOT__write_reg__497__data 
        = __Vtask_test_blitter__DOT__blt_reg__496__data;
    __Vtask_test_blitter__DOT__write_reg__497__addr 
        = (0x0000ffffU & ((IData)(0xba83U) + __Vtask_test_blitter__DOT__blt_reg__496__off));
    Vtest_blitter___024root____VbeforeTrig_h603fc354__0(vlSelf, 
                                                        "@(posedge test_blitter.clk)");
    co_await vlSelfRef.__VtrigSched_h603fc354__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge test_blitter.clk)", 
                                                         "test_blitter.sv", 
                                                         113);
    vlSelfRef.test_blitter__DOT__cpu_addr = __Vtask_test_blitter__DOT__write_reg__497__addr;
    vlSelfRef.test_blitter__DOT__cpu_wdata = __Vtask_test_blitter__DOT__write_reg__497__data;
    vlSelfRef.test_blitter__DOT__cpu_we = 1U;
    Vtest_blitter___024root____VbeforeTrig_h603fc354__0(vlSelf, 
                                                        "@(posedge test_blitter.clk)");
    co_await vlSelfRef.__VtrigSched_h603fc354__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge test_blitter.clk)", 
                                                         "test_blitter.sv", 
                                                         115);
    vlSelfRef.test_blitter__DOT__cpu_we = 0U;
    __Vtask_test_blitter__DOT__blt_reg__498__data = 
        (0x000000ffU & (__Vtask_test_blitter__DOT__setup_copy__491__src_addr 
                        >> 8U));
    __Vtask_test_blitter__DOT__blt_reg__498__off = 6U;
    __Vtask_test_blitter__DOT__write_reg__499__data 
        = __Vtask_test_blitter__DOT__blt_reg__498__data;
    __Vtask_test_blitter__DOT__write_reg__499__addr 
        = (0x0000ffffU & ((IData)(0xba83U) + __Vtask_test_blitter__DOT__blt_reg__498__off));
    Vtest_blitter___024root____VbeforeTrig_h603fc354__0(vlSelf, 
                                                        "@(posedge test_blitter.clk)");
    co_await vlSelfRef.__VtrigSched_h603fc354__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge test_blitter.clk)", 
                                                         "test_blitter.sv", 
                                                         113);
    vlSelfRef.test_blitter__DOT__cpu_addr = __Vtask_test_blitter__DOT__write_reg__499__addr;
    vlSelfRef.test_blitter__DOT__cpu_wdata = __Vtask_test_blitter__DOT__write_reg__499__data;
    vlSelfRef.test_blitter__DOT__cpu_we = 1U;
    Vtest_blitter___024root____VbeforeTrig_h603fc354__0(vlSelf, 
                                                        "@(posedge test_blitter.clk)");
    co_await vlSelfRef.__VtrigSched_h603fc354__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge test_blitter.clk)", 
                                                         "test_blitter.sv", 
                                                         115);
    vlSelfRef.test_blitter__DOT__cpu_we = 0U;
    __Vtask_test_blitter__DOT__blt_reg__500__data = 
        (0x000000ffU & (__Vtask_test_blitter__DOT__setup_copy__491__src_addr 
                        >> 0x10U));
    __Vtask_test_blitter__DOT__blt_reg__500__off = 7U;
    __Vtask_test_blitter__DOT__write_reg__501__data 
        = __Vtask_test_blitter__DOT__blt_reg__500__data;
    __Vtask_test_blitter__DOT__write_reg__501__addr 
        = (0x0000ffffU & ((IData)(0xba83U) + __Vtask_test_blitter__DOT__blt_reg__500__off));
    Vtest_blitter___024root____VbeforeTrig_h603fc354__0(vlSelf, 
                                                        "@(posedge test_blitter.clk)");
    co_await vlSelfRef.__VtrigSched_h603fc354__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge test_blitter.clk)", 
                                                         "test_blitter.sv", 
                                                         113);
    vlSelfRef.test_blitter__DOT__cpu_addr = __Vtask_test_blitter__DOT__write_reg__501__addr;
    vlSelfRef.test_blitter__DOT__cpu_wdata = __Vtask_test_blitter__DOT__write_reg__501__data;
    vlSelfRef.test_blitter__DOT__cpu_we = 1U;
    Vtest_blitter___024root____VbeforeTrig_h603fc354__0(vlSelf, 
                                                        "@(posedge test_blitter.clk)");
    co_await vlSelfRef.__VtrigSched_h603fc354__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge test_blitter.clk)", 
                                                         "test_blitter.sv", 
                                                         115);
    vlSelfRef.test_blitter__DOT__cpu_we = 0U;
    __Vtask_test_blitter__DOT__blt_reg__502__data = 
        (0x000000ffU & __Vtask_test_blitter__DOT__setup_copy__491__dst_addr);
    __Vtask_test_blitter__DOT__blt_reg__502__off = 8U;
    __Vtask_test_blitter__DOT__write_reg__503__data 
        = __Vtask_test_blitter__DOT__blt_reg__502__data;
    __Vtask_test_blitter__DOT__write_reg__503__addr 
        = (0x0000ffffU & ((IData)(0xba83U) + __Vtask_test_blitter__DOT__blt_reg__502__off));
    Vtest_blitter___024root____VbeforeTrig_h603fc354__0(vlSelf, 
                                                        "@(posedge test_blitter.clk)");
    co_await vlSelfRef.__VtrigSched_h603fc354__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge test_blitter.clk)", 
                                                         "test_blitter.sv", 
                                                         113);
    vlSelfRef.test_blitter__DOT__cpu_addr = __Vtask_test_blitter__DOT__write_reg__503__addr;
    vlSelfRef.test_blitter__DOT__cpu_wdata = __Vtask_test_blitter__DOT__write_reg__503__data;
    vlSelfRef.test_blitter__DOT__cpu_we = 1U;
    Vtest_blitter___024root____VbeforeTrig_h603fc354__0(vlSelf, 
                                                        "@(posedge test_blitter.clk)");
    co_await vlSelfRef.__VtrigSched_h603fc354__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge test_blitter.clk)", 
                                                         "test_blitter.sv", 
                                                         115);
    vlSelfRef.test_blitter__DOT__cpu_we = 0U;
    __Vtask_test_blitter__DOT__blt_reg__504__data = 
        (0x000000ffU & (__Vtask_test_blitter__DOT__setup_copy__491__dst_addr 
                        >> 8U));
    __Vtask_test_blitter__DOT__blt_reg__504__off = 9U;
    __Vtask_test_blitter__DOT__write_reg__505__data 
        = __Vtask_test_blitter__DOT__blt_reg__504__data;
    __Vtask_test_blitter__DOT__write_reg__505__addr 
        = (0x0000ffffU & ((IData)(0xba83U) + __Vtask_test_blitter__DOT__blt_reg__504__off));
    Vtest_blitter___024root____VbeforeTrig_h603fc354__0(vlSelf, 
                                                        "@(posedge test_blitter.clk)");
    co_await vlSelfRef.__VtrigSched_h603fc354__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge test_blitter.clk)", 
                                                         "test_blitter.sv", 
                                                         113);
    vlSelfRef.test_blitter__DOT__cpu_addr = __Vtask_test_blitter__DOT__write_reg__505__addr;
    vlSelfRef.test_blitter__DOT__cpu_wdata = __Vtask_test_blitter__DOT__write_reg__505__data;
    vlSelfRef.test_blitter__DOT__cpu_we = 1U;
    Vtest_blitter___024root____VbeforeTrig_h603fc354__0(vlSelf, 
                                                        "@(posedge test_blitter.clk)");
    co_await vlSelfRef.__VtrigSched_h603fc354__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge test_blitter.clk)", 
                                                         "test_blitter.sv", 
                                                         115);
    vlSelfRef.test_blitter__DOT__cpu_we = 0U;
    __Vtask_test_blitter__DOT__blt_reg__506__data = 
        (0x000000ffU & (__Vtask_test_blitter__DOT__setup_copy__491__dst_addr 
                        >> 0x10U));
    __Vtask_test_blitter__DOT__blt_reg__506__off = 0x0000000aU;
    __Vtask_test_blitter__DOT__write_reg__507__data 
        = __Vtask_test_blitter__DOT__blt_reg__506__data;
    __Vtask_test_blitter__DOT__write_reg__507__addr 
        = (0x0000ffffU & ((IData)(0xba83U) + __Vtask_test_blitter__DOT__blt_reg__506__off));
    Vtest_blitter___024root____VbeforeTrig_h603fc354__0(vlSelf, 
                                                        "@(posedge test_blitter.clk)");
    co_await vlSelfRef.__VtrigSched_h603fc354__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge test_blitter.clk)", 
                                                         "test_blitter.sv", 
                                                         113);
    vlSelfRef.test_blitter__DOT__cpu_addr = __Vtask_test_blitter__DOT__write_reg__507__addr;
    vlSelfRef.test_blitter__DOT__cpu_wdata = __Vtask_test_blitter__DOT__write_reg__507__data;
    vlSelfRef.test_blitter__DOT__cpu_we = 1U;
    Vtest_blitter___024root____VbeforeTrig_h603fc354__0(vlSelf, 
                                                        "@(posedge test_blitter.clk)");
    co_await vlSelfRef.__VtrigSched_h603fc354__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge test_blitter.clk)", 
                                                         "test_blitter.sv", 
                                                         115);
    vlSelfRef.test_blitter__DOT__cpu_we = 0U;
    __Vtask_test_blitter__DOT__blt_reg__508__data = 
        (0x000000ffU & __Vtask_test_blitter__DOT__setup_copy__491__width);
    __Vtask_test_blitter__DOT__blt_reg__508__off = 0x0000000bU;
    __Vtask_test_blitter__DOT__write_reg__509__data 
        = __Vtask_test_blitter__DOT__blt_reg__508__data;
    __Vtask_test_blitter__DOT__write_reg__509__addr 
        = (0x0000ffffU & ((IData)(0xba83U) + __Vtask_test_blitter__DOT__blt_reg__508__off));
    Vtest_blitter___024root____VbeforeTrig_h603fc354__0(vlSelf, 
                                                        "@(posedge test_blitter.clk)");
    co_await vlSelfRef.__VtrigSched_h603fc354__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge test_blitter.clk)", 
                                                         "test_blitter.sv", 
                                                         113);
    vlSelfRef.test_blitter__DOT__cpu_addr = __Vtask_test_blitter__DOT__write_reg__509__addr;
    vlSelfRef.test_blitter__DOT__cpu_wdata = __Vtask_test_blitter__DOT__write_reg__509__data;
    vlSelfRef.test_blitter__DOT__cpu_we = 1U;
    Vtest_blitter___024root____VbeforeTrig_h603fc354__0(vlSelf, 
                                                        "@(posedge test_blitter.clk)");
    co_await vlSelfRef.__VtrigSched_h603fc354__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge test_blitter.clk)", 
                                                         "test_blitter.sv", 
                                                         115);
    vlSelfRef.test_blitter__DOT__cpu_we = 0U;
    __Vtask_test_blitter__DOT__blt_reg__510__data = 
        (0x000000ffU & (__Vtask_test_blitter__DOT__setup_copy__491__width 
                        >> 8U));
    __Vtask_test_blitter__DOT__blt_reg__510__off = 0x0000000cU;
    __Vtask_test_blitter__DOT__write_reg__511__data 
        = __Vtask_test_blitter__DOT__blt_reg__510__data;
    __Vtask_test_blitter__DOT__write_reg__511__addr 
        = (0x0000ffffU & ((IData)(0xba83U) + __Vtask_test_blitter__DOT__blt_reg__510__off));
    Vtest_blitter___024root____VbeforeTrig_h603fc354__0(vlSelf, 
                                                        "@(posedge test_blitter.clk)");
    co_await vlSelfRef.__VtrigSched_h603fc354__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge test_blitter.clk)", 
                                                         "test_blitter.sv", 
                                                         113);
    vlSelfRef.test_blitter__DOT__cpu_addr = __Vtask_test_blitter__DOT__write_reg__511__addr;
    vlSelfRef.test_blitter__DOT__cpu_wdata = __Vtask_test_blitter__DOT__write_reg__511__data;
    vlSelfRef.test_blitter__DOT__cpu_we = 1U;
    Vtest_blitter___024root____VbeforeTrig_h603fc354__0(vlSelf, 
                                                        "@(posedge test_blitter.clk)");
    co_await vlSelfRef.__VtrigSched_h603fc354__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge test_blitter.clk)", 
                                                         "test_blitter.sv", 
                                                         115);
    vlSelfRef.test_blitter__DOT__cpu_we = 0U;
    __Vtask_test_blitter__DOT__blt_reg__512__data = 
        (0x000000ffU & __Vtask_test_blitter__DOT__setup_copy__491__height);
    __Vtask_test_blitter__DOT__blt_reg__512__off = 0x0000000dU;
    __Vtask_test_blitter__DOT__write_reg__513__data 
        = __Vtask_test_blitter__DOT__blt_reg__512__data;
    __Vtask_test_blitter__DOT__write_reg__513__addr 
        = (0x0000ffffU & ((IData)(0xba83U) + __Vtask_test_blitter__DOT__blt_reg__512__off));
    Vtest_blitter___024root____VbeforeTrig_h603fc354__0(vlSelf, 
                                                        "@(posedge test_blitter.clk)");
    co_await vlSelfRef.__VtrigSched_h603fc354__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge test_blitter.clk)", 
                                                         "test_blitter.sv", 
                                                         113);
    vlSelfRef.test_blitter__DOT__cpu_addr = __Vtask_test_blitter__DOT__write_reg__513__addr;
    vlSelfRef.test_blitter__DOT__cpu_wdata = __Vtask_test_blitter__DOT__write_reg__513__data;
    vlSelfRef.test_blitter__DOT__cpu_we = 1U;
    Vtest_blitter___024root____VbeforeTrig_h603fc354__0(vlSelf, 
                                                        "@(posedge test_blitter.clk)");
    co_await vlSelfRef.__VtrigSched_h603fc354__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge test_blitter.clk)", 
                                                         "test_blitter.sv", 
                                                         115);
    vlSelfRef.test_blitter__DOT__cpu_we = 0U;
    __Vtask_test_blitter__DOT__blt_reg__514__data = 
        (0x000000ffU & (__Vtask_test_blitter__DOT__setup_copy__491__height 
                        >> 8U));
    __Vtask_test_blitter__DOT__blt_reg__514__off = 0x0000000eU;
    __Vtask_test_blitter__DOT__write_reg__515__data 
        = __Vtask_test_blitter__DOT__blt_reg__514__data;
    __Vtask_test_blitter__DOT__write_reg__515__addr 
        = (0x0000ffffU & ((IData)(0xba83U) + __Vtask_test_blitter__DOT__blt_reg__514__off));
    Vtest_blitter___024root____VbeforeTrig_h603fc354__0(vlSelf, 
                                                        "@(posedge test_blitter.clk)");
    co_await vlSelfRef.__VtrigSched_h603fc354__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge test_blitter.clk)", 
                                                         "test_blitter.sv", 
                                                         113);
    vlSelfRef.test_blitter__DOT__cpu_addr = __Vtask_test_blitter__DOT__write_reg__515__addr;
    vlSelfRef.test_blitter__DOT__cpu_wdata = __Vtask_test_blitter__DOT__write_reg__515__data;
    vlSelfRef.test_blitter__DOT__cpu_we = 1U;
    Vtest_blitter___024root____VbeforeTrig_h603fc354__0(vlSelf, 
                                                        "@(posedge test_blitter.clk)");
    co_await vlSelfRef.__VtrigSched_h603fc354__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge test_blitter.clk)", 
                                                         "test_blitter.sv", 
                                                         115);
    vlSelfRef.test_blitter__DOT__cpu_we = 0U;
    __Vtask_test_blitter__DOT__blt_reg__516__data = 
        (0x000000ffU & __Vtask_test_blitter__DOT__setup_copy__491__src_stride);
    __Vtask_test_blitter__DOT__blt_reg__516__off = 0x0000000fU;
    __Vtask_test_blitter__DOT__write_reg__517__data 
        = __Vtask_test_blitter__DOT__blt_reg__516__data;
    __Vtask_test_blitter__DOT__write_reg__517__addr 
        = (0x0000ffffU & ((IData)(0xba83U) + __Vtask_test_blitter__DOT__blt_reg__516__off));
    Vtest_blitter___024root____VbeforeTrig_h603fc354__0(vlSelf, 
                                                        "@(posedge test_blitter.clk)");
    co_await vlSelfRef.__VtrigSched_h603fc354__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge test_blitter.clk)", 
                                                         "test_blitter.sv", 
                                                         113);
    vlSelfRef.test_blitter__DOT__cpu_addr = __Vtask_test_blitter__DOT__write_reg__517__addr;
    vlSelfRef.test_blitter__DOT__cpu_wdata = __Vtask_test_blitter__DOT__write_reg__517__data;
    vlSelfRef.test_blitter__DOT__cpu_we = 1U;
    Vtest_blitter___024root____VbeforeTrig_h603fc354__0(vlSelf, 
                                                        "@(posedge test_blitter.clk)");
    co_await vlSelfRef.__VtrigSched_h603fc354__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge test_blitter.clk)", 
                                                         "test_blitter.sv", 
                                                         115);
    vlSelfRef.test_blitter__DOT__cpu_we = 0U;
    __Vtask_test_blitter__DOT__blt_reg__518__data = 
        (0x000000ffU & (__Vtask_test_blitter__DOT__setup_copy__491__src_stride 
                        >> 8U));
    __Vtask_test_blitter__DOT__blt_reg__518__off = 0x00000010U;
    __Vtask_test_blitter__DOT__write_reg__519__data 
        = __Vtask_test_blitter__DOT__blt_reg__518__data;
    __Vtask_test_blitter__DOT__write_reg__519__addr 
        = (0x0000ffffU & ((IData)(0xba83U) + __Vtask_test_blitter__DOT__blt_reg__518__off));
    Vtest_blitter___024root____VbeforeTrig_h603fc354__0(vlSelf, 
                                                        "@(posedge test_blitter.clk)");
    co_await vlSelfRef.__VtrigSched_h603fc354__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge test_blitter.clk)", 
                                                         "test_blitter.sv", 
                                                         113);
    vlSelfRef.test_blitter__DOT__cpu_addr = __Vtask_test_blitter__DOT__write_reg__519__addr;
    vlSelfRef.test_blitter__DOT__cpu_wdata = __Vtask_test_blitter__DOT__write_reg__519__data;
    vlSelfRef.test_blitter__DOT__cpu_we = 1U;
    Vtest_blitter___024root____VbeforeTrig_h603fc354__0(vlSelf, 
                                                        "@(posedge test_blitter.clk)");
    co_await vlSelfRef.__VtrigSched_h603fc354__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge test_blitter.clk)", 
                                                         "test_blitter.sv", 
                                                         115);
    vlSelfRef.test_blitter__DOT__cpu_we = 0U;
    __Vtask_test_blitter__DOT__blt_reg__520__data = 
        (0x000000ffU & __Vtask_test_blitter__DOT__setup_copy__491__dst_stride);
    __Vtask_test_blitter__DOT__blt_reg__520__off = 0x00000011U;
    __Vtask_test_blitter__DOT__write_reg__521__data 
        = __Vtask_test_blitter__DOT__blt_reg__520__data;
    __Vtask_test_blitter__DOT__write_reg__521__addr 
        = (0x0000ffffU & ((IData)(0xba83U) + __Vtask_test_blitter__DOT__blt_reg__520__off));
    Vtest_blitter___024root____VbeforeTrig_h603fc354__0(vlSelf, 
                                                        "@(posedge test_blitter.clk)");
    co_await vlSelfRef.__VtrigSched_h603fc354__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge test_blitter.clk)", 
                                                         "test_blitter.sv", 
                                                         113);
    vlSelfRef.test_blitter__DOT__cpu_addr = __Vtask_test_blitter__DOT__write_reg__521__addr;
    vlSelfRef.test_blitter__DOT__cpu_wdata = __Vtask_test_blitter__DOT__write_reg__521__data;
    vlSelfRef.test_blitter__DOT__cpu_we = 1U;
    Vtest_blitter___024root____VbeforeTrig_h603fc354__0(vlSelf, 
                                                        "@(posedge test_blitter.clk)");
    co_await vlSelfRef.__VtrigSched_h603fc354__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge test_blitter.clk)", 
                                                         "test_blitter.sv", 
                                                         115);
    vlSelfRef.test_blitter__DOT__cpu_we = 0U;
    __Vtask_test_blitter__DOT__blt_reg__522__data = 
        (0x000000ffU & (__Vtask_test_blitter__DOT__setup_copy__491__dst_stride 
                        >> 8U));
    __Vtask_test_blitter__DOT__blt_reg__522__off = 0x00000012U;
    __Vtask_test_blitter__DOT__write_reg__523__data 
        = __Vtask_test_blitter__DOT__blt_reg__522__data;
    __Vtask_test_blitter__DOT__write_reg__523__addr 
        = (0x0000ffffU & ((IData)(0xba83U) + __Vtask_test_blitter__DOT__blt_reg__522__off));
    Vtest_blitter___024root____VbeforeTrig_h603fc354__0(vlSelf, 
                                                        "@(posedge test_blitter.clk)");
    co_await vlSelfRef.__VtrigSched_h603fc354__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge test_blitter.clk)", 
                                                         "test_blitter.sv", 
                                                         113);
    vlSelfRef.test_blitter__DOT__cpu_addr = __Vtask_test_blitter__DOT__write_reg__523__addr;
    vlSelfRef.test_blitter__DOT__cpu_wdata = __Vtask_test_blitter__DOT__write_reg__523__data;
    vlSelfRef.test_blitter__DOT__cpu_we = 1U;
    Vtest_blitter___024root____VbeforeTrig_h603fc354__0(vlSelf, 
                                                        "@(posedge test_blitter.clk)");
    co_await vlSelfRef.__VtrigSched_h603fc354__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge test_blitter.clk)", 
                                                         "test_blitter.sv", 
                                                         115);
    vlSelfRef.test_blitter__DOT__cpu_we = 0U;
    __Vtask_test_blitter__DOT__blt_reg__524__data = 0U;
    __Vtask_test_blitter__DOT__blt_reg__524__off = 0x00000013U;
    __Vtask_test_blitter__DOT__write_reg__525__data 
        = __Vtask_test_blitter__DOT__blt_reg__524__data;
    __Vtask_test_blitter__DOT__write_reg__525__addr 
        = (0x0000ffffU & ((IData)(0xba83U) + __Vtask_test_blitter__DOT__blt_reg__524__off));
    Vtest_blitter___024root____VbeforeTrig_h603fc354__0(vlSelf, 
                                                        "@(posedge test_blitter.clk)");
    co_await vlSelfRef.__VtrigSched_h603fc354__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge test_blitter.clk)", 
                                                         "test_blitter.sv", 
                                                         113);
    vlSelfRef.test_blitter__DOT__cpu_addr = __Vtask_test_blitter__DOT__write_reg__525__addr;
    vlSelfRef.test_blitter__DOT__cpu_wdata = __Vtask_test_blitter__DOT__write_reg__525__data;
    vlSelfRef.test_blitter__DOT__cpu_we = 1U;
    Vtest_blitter___024root____VbeforeTrig_h603fc354__0(vlSelf, 
                                                        "@(posedge test_blitter.clk)");
    co_await vlSelfRef.__VtrigSched_h603fc354__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge test_blitter.clk)", 
                                                         "test_blitter.sv", 
                                                         115);
    vlSelfRef.test_blitter__DOT__cpu_we = 0U;
    __Vtask_test_blitter__DOT__blt_reg__527__data = 1U;
    __Vtask_test_blitter__DOT__blt_reg__527__off = 0U;
    __Vtask_test_blitter__DOT__write_reg__528__data 
        = __Vtask_test_blitter__DOT__blt_reg__527__data;
    __Vtask_test_blitter__DOT__write_reg__528__addr 
        = (0x0000ffffU & ((IData)(0xba83U) + __Vtask_test_blitter__DOT__blt_reg__527__off));
    Vtest_blitter___024root____VbeforeTrig_h603fc354__0(vlSelf, 
                                                        "@(posedge test_blitter.clk)");
    co_await vlSelfRef.__VtrigSched_h603fc354__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge test_blitter.clk)", 
                                                         "test_blitter.sv", 
                                                         113);
    vlSelfRef.test_blitter__DOT__cpu_addr = __Vtask_test_blitter__DOT__write_reg__528__addr;
    vlSelfRef.test_blitter__DOT__cpu_wdata = __Vtask_test_blitter__DOT__write_reg__528__data;
    vlSelfRef.test_blitter__DOT__cpu_we = 1U;
    Vtest_blitter___024root____VbeforeTrig_h603fc354__0(vlSelf, 
                                                        "@(posedge test_blitter.clk)");
    co_await vlSelfRef.__VtrigSched_h603fc354__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge test_blitter.clk)", 
                                                         "test_blitter.sv", 
                                                         115);
    vlSelfRef.test_blitter__DOT__cpu_we = 0U;
    __Vtask_test_blitter__DOT__wait_blt_done__529__test_blitter__DOT__unnamedblk1_1__DOT____Vrepeat0 = 0;
    __Vtask_test_blitter__DOT__wait_blt_done__529__test_blitter__DOT__unnamedblk1_2__DOT____Vrepeat1 = 0;
    __Vtask_test_blitter__DOT__wait_blt_done__529__test_blitter__DOT__unnamedblk1_1__DOT____Vrepeat0 = 2U;
    while (VL_LTS_III(32, 0U, __Vtask_test_blitter__DOT__wait_blt_done__529__test_blitter__DOT__unnamedblk1_1__DOT____Vrepeat0)) {
        Vtest_blitter___024root____VbeforeTrig_h603fc354__0(vlSelf, 
                                                            "@(posedge test_blitter.clk)");
        co_await vlSelfRef.__VtrigSched_h603fc354__0.trigger(0U, 
                                                             nullptr, 
                                                             "@(posedge test_blitter.clk)", 
                                                             "test_blitter.sv", 
                                                             128);
        __Vtask_test_blitter__DOT__wait_blt_done__529__test_blitter__DOT__unnamedblk1_1__DOT____Vrepeat0 
            = (__Vtask_test_blitter__DOT__wait_blt_done__529__test_blitter__DOT__unnamedblk1_1__DOT____Vrepeat0 
               - (IData)(1U));
    }
    while (((0U != (IData)(vlSelfRef.test_blitter__DOT__dut__DOT__state)) 
            & (6U != (IData)(vlSelfRef.test_blitter__DOT__dut__DOT__state)))) {
        Vtest_blitter___024root____VbeforeTrig_h603fc354__0(vlSelf, 
                                                            "@(posedge test_blitter.clk)");
        co_await vlSelfRef.__VtrigSched_h603fc354__0.trigger(0U, 
                                                             nullptr, 
                                                             "@(posedge test_blitter.clk)", 
                                                             "test_blitter.sv", 
                                                             130);
    }
    __Vtask_test_blitter__DOT__wait_blt_done__529__test_blitter__DOT__unnamedblk1_2__DOT____Vrepeat1 = 2U;
    while (VL_LTS_III(32, 0U, __Vtask_test_blitter__DOT__wait_blt_done__529__test_blitter__DOT__unnamedblk1_2__DOT____Vrepeat1)) {
        Vtest_blitter___024root____VbeforeTrig_h603fc354__0(vlSelf, 
                                                            "@(posedge test_blitter.clk)");
        co_await vlSelfRef.__VtrigSched_h603fc354__0.trigger(0U, 
                                                             nullptr, 
                                                             "@(posedge test_blitter.clk)", 
                                                             "test_blitter.sv", 
                                                             131);
        __Vtask_test_blitter__DOT__wait_blt_done__529__test_blitter__DOT__unnamedblk1_2__DOT____Vrepeat1 
            = (__Vtask_test_blitter__DOT__wait_blt_done__529__test_blitter__DOT__unnamedblk1_2__DOT____Vrepeat1 
               - (IData)(1U));
    }
    __Vtask_test_blitter__DOT__check__530__condition 
        = (3U == vlSelfRef.test_blitter__DOT__dut__DOT__regs[1U]);
    vlSelfRef.__Vtask_test_blitter__DOT__check__530__name = "src range: status error"s;
    vlSelfRef.test_blitter__DOT__test_num = ((IData)(1U) 
                                             + vlSelfRef.test_blitter__DOT__test_num);
    if (__Vtask_test_blitter__DOT__check__530__condition) {
        VL_WRITEF_NX("  PASS [%0d] %@\n",0,32,vlSelfRef.test_blitter__DOT__test_num,
                     -1,&(vlSelfRef.__Vtask_test_blitter__DOT__check__530__name));
        vlSelfRef.test_blitter__DOT__pass_count = ((IData)(1U) 
                                                   + vlSelfRef.test_blitter__DOT__pass_count);
    } else {
        VL_WRITEF_NX("  FAIL [%0d] %@\n",0,32,vlSelfRef.test_blitter__DOT__test_num,
                     -1,&(vlSelfRef.__Vtask_test_blitter__DOT__check__530__name));
        vlSelfRef.test_blitter__DOT__fail_count = ((IData)(1U) 
                                                   + vlSelfRef.test_blitter__DOT__fail_count);
    }
    __Vtask_test_blitter__DOT__check__531__condition 
        = (3U == vlSelfRef.test_blitter__DOT__dut__DOT__regs[2U]);
    vlSelfRef.__Vtask_test_blitter__DOT__check__531__name = "src range: errcode = range"s;
    vlSelfRef.test_blitter__DOT__test_num = ((IData)(1U) 
                                             + vlSelfRef.test_blitter__DOT__test_num);
    if (__Vtask_test_blitter__DOT__check__531__condition) {
        VL_WRITEF_NX("  PASS [%0d] %@\n",0,32,vlSelfRef.test_blitter__DOT__test_num,
                     -1,&(vlSelfRef.__Vtask_test_blitter__DOT__check__531__name));
        vlSelfRef.test_blitter__DOT__pass_count = ((IData)(1U) 
                                                   + vlSelfRef.test_blitter__DOT__pass_count);
    } else {
        VL_WRITEF_NX("  FAIL [%0d] %@\n",0,32,vlSelfRef.test_blitter__DOT__test_num,
                     -1,&(vlSelfRef.__Vtask_test_blitter__DOT__check__531__name));
        vlSelfRef.test_blitter__DOT__fail_count = ((IData)(1U) 
                                                   + vlSelfRef.test_blitter__DOT__fail_count);
    }
    VL_WRITEF_NX("Test: Dst range validation\n",0);
    __Vtask_test_blitter__DOT__setup_fill__532__fill_val = 1U;
    __Vtask_test_blitter__DOT__setup_fill__532__height = 1U;
    __Vtask_test_blitter__DOT__setup_fill__532__width = 0x00000014U;
    __Vtask_test_blitter__DOT__setup_fill__532__dst_stride = 0x000007d0U;
    __Vtask_test_blitter__DOT__setup_fill__532__dst_addr = 0x000007c6U;
    __Vtask_test_blitter__DOT__setup_fill__532__dst_space = 2U;
    __Vtask_test_blitter__DOT__blt_reg__533__data = 
        (0x000000ffU & __Vtask_test_blitter__DOT__setup_fill__532__dst_space);
    __Vtask_test_blitter__DOT__blt_reg__533__off = 4U;
    __Vtask_test_blitter__DOT__write_reg__534__data 
        = __Vtask_test_blitter__DOT__blt_reg__533__data;
    __Vtask_test_blitter__DOT__write_reg__534__addr 
        = (0x0000ffffU & ((IData)(0xba83U) + __Vtask_test_blitter__DOT__blt_reg__533__off));
    Vtest_blitter___024root____VbeforeTrig_h603fc354__0(vlSelf, 
                                                        "@(posedge test_blitter.clk)");
    co_await vlSelfRef.__VtrigSched_h603fc354__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge test_blitter.clk)", 
                                                         "test_blitter.sv", 
                                                         113);
    vlSelfRef.test_blitter__DOT__cpu_addr = __Vtask_test_blitter__DOT__write_reg__534__addr;
    vlSelfRef.test_blitter__DOT__cpu_wdata = __Vtask_test_blitter__DOT__write_reg__534__data;
    vlSelfRef.test_blitter__DOT__cpu_we = 1U;
    Vtest_blitter___024root____VbeforeTrig_h603fc354__0(vlSelf, 
                                                        "@(posedge test_blitter.clk)");
    co_await vlSelfRef.__VtrigSched_h603fc354__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge test_blitter.clk)", 
                                                         "test_blitter.sv", 
                                                         115);
    vlSelfRef.test_blitter__DOT__cpu_we = 0U;
    __Vtask_test_blitter__DOT__blt_reg__535__data = 
        (0x000000ffU & __Vtask_test_blitter__DOT__setup_fill__532__dst_addr);
    __Vtask_test_blitter__DOT__blt_reg__535__off = 8U;
    __Vtask_test_blitter__DOT__write_reg__536__data 
        = __Vtask_test_blitter__DOT__blt_reg__535__data;
    __Vtask_test_blitter__DOT__write_reg__536__addr 
        = (0x0000ffffU & ((IData)(0xba83U) + __Vtask_test_blitter__DOT__blt_reg__535__off));
    Vtest_blitter___024root____VbeforeTrig_h603fc354__0(vlSelf, 
                                                        "@(posedge test_blitter.clk)");
    co_await vlSelfRef.__VtrigSched_h603fc354__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge test_blitter.clk)", 
                                                         "test_blitter.sv", 
                                                         113);
    vlSelfRef.test_blitter__DOT__cpu_addr = __Vtask_test_blitter__DOT__write_reg__536__addr;
    vlSelfRef.test_blitter__DOT__cpu_wdata = __Vtask_test_blitter__DOT__write_reg__536__data;
    vlSelfRef.test_blitter__DOT__cpu_we = 1U;
    Vtest_blitter___024root____VbeforeTrig_h603fc354__0(vlSelf, 
                                                        "@(posedge test_blitter.clk)");
    co_await vlSelfRef.__VtrigSched_h603fc354__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge test_blitter.clk)", 
                                                         "test_blitter.sv", 
                                                         115);
    vlSelfRef.test_blitter__DOT__cpu_we = 0U;
    __Vtask_test_blitter__DOT__blt_reg__537__data = 
        (0x000000ffU & (__Vtask_test_blitter__DOT__setup_fill__532__dst_addr 
                        >> 8U));
    __Vtask_test_blitter__DOT__blt_reg__537__off = 9U;
    __Vtask_test_blitter__DOT__write_reg__538__data 
        = __Vtask_test_blitter__DOT__blt_reg__537__data;
    __Vtask_test_blitter__DOT__write_reg__538__addr 
        = (0x0000ffffU & ((IData)(0xba83U) + __Vtask_test_blitter__DOT__blt_reg__537__off));
    Vtest_blitter___024root____VbeforeTrig_h603fc354__0(vlSelf, 
                                                        "@(posedge test_blitter.clk)");
    co_await vlSelfRef.__VtrigSched_h603fc354__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge test_blitter.clk)", 
                                                         "test_blitter.sv", 
                                                         113);
    vlSelfRef.test_blitter__DOT__cpu_addr = __Vtask_test_blitter__DOT__write_reg__538__addr;
    vlSelfRef.test_blitter__DOT__cpu_wdata = __Vtask_test_blitter__DOT__write_reg__538__data;
    vlSelfRef.test_blitter__DOT__cpu_we = 1U;
    Vtest_blitter___024root____VbeforeTrig_h603fc354__0(vlSelf, 
                                                        "@(posedge test_blitter.clk)");
    co_await vlSelfRef.__VtrigSched_h603fc354__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge test_blitter.clk)", 
                                                         "test_blitter.sv", 
                                                         115);
    vlSelfRef.test_blitter__DOT__cpu_we = 0U;
    __Vtask_test_blitter__DOT__blt_reg__539__data = 
        (0x000000ffU & (__Vtask_test_blitter__DOT__setup_fill__532__dst_addr 
                        >> 0x10U));
    __Vtask_test_blitter__DOT__blt_reg__539__off = 0x0000000aU;
    __Vtask_test_blitter__DOT__write_reg__540__data 
        = __Vtask_test_blitter__DOT__blt_reg__539__data;
    __Vtask_test_blitter__DOT__write_reg__540__addr 
        = (0x0000ffffU & ((IData)(0xba83U) + __Vtask_test_blitter__DOT__blt_reg__539__off));
    Vtest_blitter___024root____VbeforeTrig_h603fc354__0(vlSelf, 
                                                        "@(posedge test_blitter.clk)");
    co_await vlSelfRef.__VtrigSched_h603fc354__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge test_blitter.clk)", 
                                                         "test_blitter.sv", 
                                                         113);
    vlSelfRef.test_blitter__DOT__cpu_addr = __Vtask_test_blitter__DOT__write_reg__540__addr;
    vlSelfRef.test_blitter__DOT__cpu_wdata = __Vtask_test_blitter__DOT__write_reg__540__data;
    vlSelfRef.test_blitter__DOT__cpu_we = 1U;
    Vtest_blitter___024root____VbeforeTrig_h603fc354__0(vlSelf, 
                                                        "@(posedge test_blitter.clk)");
    co_await vlSelfRef.__VtrigSched_h603fc354__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge test_blitter.clk)", 
                                                         "test_blitter.sv", 
                                                         115);
    vlSelfRef.test_blitter__DOT__cpu_we = 0U;
    __Vtask_test_blitter__DOT__blt_reg__541__data = 
        (0x000000ffU & __Vtask_test_blitter__DOT__setup_fill__532__width);
    __Vtask_test_blitter__DOT__blt_reg__541__off = 0x0000000bU;
    __Vtask_test_blitter__DOT__write_reg__542__data 
        = __Vtask_test_blitter__DOT__blt_reg__541__data;
    __Vtask_test_blitter__DOT__write_reg__542__addr 
        = (0x0000ffffU & ((IData)(0xba83U) + __Vtask_test_blitter__DOT__blt_reg__541__off));
    Vtest_blitter___024root____VbeforeTrig_h603fc354__0(vlSelf, 
                                                        "@(posedge test_blitter.clk)");
    co_await vlSelfRef.__VtrigSched_h603fc354__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge test_blitter.clk)", 
                                                         "test_blitter.sv", 
                                                         113);
    vlSelfRef.test_blitter__DOT__cpu_addr = __Vtask_test_blitter__DOT__write_reg__542__addr;
    vlSelfRef.test_blitter__DOT__cpu_wdata = __Vtask_test_blitter__DOT__write_reg__542__data;
    vlSelfRef.test_blitter__DOT__cpu_we = 1U;
    Vtest_blitter___024root____VbeforeTrig_h603fc354__0(vlSelf, 
                                                        "@(posedge test_blitter.clk)");
    co_await vlSelfRef.__VtrigSched_h603fc354__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge test_blitter.clk)", 
                                                         "test_blitter.sv", 
                                                         115);
    vlSelfRef.test_blitter__DOT__cpu_we = 0U;
    __Vtask_test_blitter__DOT__blt_reg__543__data = 
        (0x000000ffU & (__Vtask_test_blitter__DOT__setup_fill__532__width 
                        >> 8U));
    __Vtask_test_blitter__DOT__blt_reg__543__off = 0x0000000cU;
    __Vtask_test_blitter__DOT__write_reg__544__data 
        = __Vtask_test_blitter__DOT__blt_reg__543__data;
    __Vtask_test_blitter__DOT__write_reg__544__addr 
        = (0x0000ffffU & ((IData)(0xba83U) + __Vtask_test_blitter__DOT__blt_reg__543__off));
    Vtest_blitter___024root____VbeforeTrig_h603fc354__0(vlSelf, 
                                                        "@(posedge test_blitter.clk)");
    co_await vlSelfRef.__VtrigSched_h603fc354__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge test_blitter.clk)", 
                                                         "test_blitter.sv", 
                                                         113);
    vlSelfRef.test_blitter__DOT__cpu_addr = __Vtask_test_blitter__DOT__write_reg__544__addr;
    vlSelfRef.test_blitter__DOT__cpu_wdata = __Vtask_test_blitter__DOT__write_reg__544__data;
    vlSelfRef.test_blitter__DOT__cpu_we = 1U;
    Vtest_blitter___024root____VbeforeTrig_h603fc354__0(vlSelf, 
                                                        "@(posedge test_blitter.clk)");
    co_await vlSelfRef.__VtrigSched_h603fc354__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge test_blitter.clk)", 
                                                         "test_blitter.sv", 
                                                         115);
    vlSelfRef.test_blitter__DOT__cpu_we = 0U;
    __Vtask_test_blitter__DOT__blt_reg__545__data = 
        (0x000000ffU & __Vtask_test_blitter__DOT__setup_fill__532__height);
    __Vtask_test_blitter__DOT__blt_reg__545__off = 0x0000000dU;
    __Vtask_test_blitter__DOT__write_reg__546__data 
        = __Vtask_test_blitter__DOT__blt_reg__545__data;
    __Vtask_test_blitter__DOT__write_reg__546__addr 
        = (0x0000ffffU & ((IData)(0xba83U) + __Vtask_test_blitter__DOT__blt_reg__545__off));
    Vtest_blitter___024root____VbeforeTrig_h603fc354__0(vlSelf, 
                                                        "@(posedge test_blitter.clk)");
    co_await vlSelfRef.__VtrigSched_h603fc354__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge test_blitter.clk)", 
                                                         "test_blitter.sv", 
                                                         113);
    vlSelfRef.test_blitter__DOT__cpu_addr = __Vtask_test_blitter__DOT__write_reg__546__addr;
    vlSelfRef.test_blitter__DOT__cpu_wdata = __Vtask_test_blitter__DOT__write_reg__546__data;
    vlSelfRef.test_blitter__DOT__cpu_we = 1U;
    Vtest_blitter___024root____VbeforeTrig_h603fc354__0(vlSelf, 
                                                        "@(posedge test_blitter.clk)");
    co_await vlSelfRef.__VtrigSched_h603fc354__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge test_blitter.clk)", 
                                                         "test_blitter.sv", 
                                                         115);
    vlSelfRef.test_blitter__DOT__cpu_we = 0U;
    __Vtask_test_blitter__DOT__blt_reg__547__data = 
        (0x000000ffU & (__Vtask_test_blitter__DOT__setup_fill__532__height 
                        >> 8U));
    __Vtask_test_blitter__DOT__blt_reg__547__off = 0x0000000eU;
    __Vtask_test_blitter__DOT__write_reg__548__data 
        = __Vtask_test_blitter__DOT__blt_reg__547__data;
    __Vtask_test_blitter__DOT__write_reg__548__addr 
        = (0x0000ffffU & ((IData)(0xba83U) + __Vtask_test_blitter__DOT__blt_reg__547__off));
    Vtest_blitter___024root____VbeforeTrig_h603fc354__0(vlSelf, 
                                                        "@(posedge test_blitter.clk)");
    co_await vlSelfRef.__VtrigSched_h603fc354__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge test_blitter.clk)", 
                                                         "test_blitter.sv", 
                                                         113);
    vlSelfRef.test_blitter__DOT__cpu_addr = __Vtask_test_blitter__DOT__write_reg__548__addr;
    vlSelfRef.test_blitter__DOT__cpu_wdata = __Vtask_test_blitter__DOT__write_reg__548__data;
    vlSelfRef.test_blitter__DOT__cpu_we = 1U;
    Vtest_blitter___024root____VbeforeTrig_h603fc354__0(vlSelf, 
                                                        "@(posedge test_blitter.clk)");
    co_await vlSelfRef.__VtrigSched_h603fc354__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge test_blitter.clk)", 
                                                         "test_blitter.sv", 
                                                         115);
    vlSelfRef.test_blitter__DOT__cpu_we = 0U;
    __Vtask_test_blitter__DOT__blt_reg__549__data = 
        (0x000000ffU & __Vtask_test_blitter__DOT__setup_fill__532__dst_stride);
    __Vtask_test_blitter__DOT__blt_reg__549__off = 0x00000011U;
    __Vtask_test_blitter__DOT__write_reg__550__data 
        = __Vtask_test_blitter__DOT__blt_reg__549__data;
    __Vtask_test_blitter__DOT__write_reg__550__addr 
        = (0x0000ffffU & ((IData)(0xba83U) + __Vtask_test_blitter__DOT__blt_reg__549__off));
    Vtest_blitter___024root____VbeforeTrig_h603fc354__0(vlSelf, 
                                                        "@(posedge test_blitter.clk)");
    co_await vlSelfRef.__VtrigSched_h603fc354__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge test_blitter.clk)", 
                                                         "test_blitter.sv", 
                                                         113);
    vlSelfRef.test_blitter__DOT__cpu_addr = __Vtask_test_blitter__DOT__write_reg__550__addr;
    vlSelfRef.test_blitter__DOT__cpu_wdata = __Vtask_test_blitter__DOT__write_reg__550__data;
    vlSelfRef.test_blitter__DOT__cpu_we = 1U;
    Vtest_blitter___024root____VbeforeTrig_h603fc354__0(vlSelf, 
                                                        "@(posedge test_blitter.clk)");
    co_await vlSelfRef.__VtrigSched_h603fc354__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge test_blitter.clk)", 
                                                         "test_blitter.sv", 
                                                         115);
    vlSelfRef.test_blitter__DOT__cpu_we = 0U;
    __Vtask_test_blitter__DOT__blt_reg__551__data = 
        (0x000000ffU & (__Vtask_test_blitter__DOT__setup_fill__532__dst_stride 
                        >> 8U));
    __Vtask_test_blitter__DOT__blt_reg__551__off = 0x00000012U;
    __Vtask_test_blitter__DOT__write_reg__552__data 
        = __Vtask_test_blitter__DOT__blt_reg__551__data;
    __Vtask_test_blitter__DOT__write_reg__552__addr 
        = (0x0000ffffU & ((IData)(0xba83U) + __Vtask_test_blitter__DOT__blt_reg__551__off));
    Vtest_blitter___024root____VbeforeTrig_h603fc354__0(vlSelf, 
                                                        "@(posedge test_blitter.clk)");
    co_await vlSelfRef.__VtrigSched_h603fc354__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge test_blitter.clk)", 
                                                         "test_blitter.sv", 
                                                         113);
    vlSelfRef.test_blitter__DOT__cpu_addr = __Vtask_test_blitter__DOT__write_reg__552__addr;
    vlSelfRef.test_blitter__DOT__cpu_wdata = __Vtask_test_blitter__DOT__write_reg__552__data;
    vlSelfRef.test_blitter__DOT__cpu_we = 1U;
    Vtest_blitter___024root____VbeforeTrig_h603fc354__0(vlSelf, 
                                                        "@(posedge test_blitter.clk)");
    co_await vlSelfRef.__VtrigSched_h603fc354__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge test_blitter.clk)", 
                                                         "test_blitter.sv", 
                                                         115);
    vlSelfRef.test_blitter__DOT__cpu_we = 0U;
    __Vtask_test_blitter__DOT__blt_reg__553__data = 1U;
    __Vtask_test_blitter__DOT__blt_reg__553__off = 0x00000013U;
    __Vtask_test_blitter__DOT__write_reg__554__data 
        = __Vtask_test_blitter__DOT__blt_reg__553__data;
    __Vtask_test_blitter__DOT__write_reg__554__addr 
        = (0x0000ffffU & ((IData)(0xba83U) + __Vtask_test_blitter__DOT__blt_reg__553__off));
    Vtest_blitter___024root____VbeforeTrig_h603fc354__0(vlSelf, 
                                                        "@(posedge test_blitter.clk)");
    co_await vlSelfRef.__VtrigSched_h603fc354__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge test_blitter.clk)", 
                                                         "test_blitter.sv", 
                                                         113);
    vlSelfRef.test_blitter__DOT__cpu_addr = __Vtask_test_blitter__DOT__write_reg__554__addr;
    vlSelfRef.test_blitter__DOT__cpu_wdata = __Vtask_test_blitter__DOT__write_reg__554__data;
    vlSelfRef.test_blitter__DOT__cpu_we = 1U;
    Vtest_blitter___024root____VbeforeTrig_h603fc354__0(vlSelf, 
                                                        "@(posedge test_blitter.clk)");
    co_await vlSelfRef.__VtrigSched_h603fc354__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge test_blitter.clk)", 
                                                         "test_blitter.sv", 
                                                         115);
    vlSelfRef.test_blitter__DOT__cpu_we = 0U;
    __Vtask_test_blitter__DOT__blt_reg__555__data = 
        (0x000000ffU & __Vtask_test_blitter__DOT__setup_fill__532__fill_val);
    __Vtask_test_blitter__DOT__blt_reg__555__off = 0x00000014U;
    __Vtask_test_blitter__DOT__write_reg__556__data 
        = __Vtask_test_blitter__DOT__blt_reg__555__data;
    __Vtask_test_blitter__DOT__write_reg__556__addr 
        = (0x0000ffffU & ((IData)(0xba83U) + __Vtask_test_blitter__DOT__blt_reg__555__off));
    Vtest_blitter___024root____VbeforeTrig_h603fc354__0(vlSelf, 
                                                        "@(posedge test_blitter.clk)");
    co_await vlSelfRef.__VtrigSched_h603fc354__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge test_blitter.clk)", 
                                                         "test_blitter.sv", 
                                                         113);
    vlSelfRef.test_blitter__DOT__cpu_addr = __Vtask_test_blitter__DOT__write_reg__556__addr;
    vlSelfRef.test_blitter__DOT__cpu_wdata = __Vtask_test_blitter__DOT__write_reg__556__data;
    vlSelfRef.test_blitter__DOT__cpu_we = 1U;
    Vtest_blitter___024root____VbeforeTrig_h603fc354__0(vlSelf, 
                                                        "@(posedge test_blitter.clk)");
    co_await vlSelfRef.__VtrigSched_h603fc354__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge test_blitter.clk)", 
                                                         "test_blitter.sv", 
                                                         115);
    vlSelfRef.test_blitter__DOT__cpu_we = 0U;
    __Vtask_test_blitter__DOT__blt_reg__558__data = 1U;
    __Vtask_test_blitter__DOT__blt_reg__558__off = 0U;
    __Vtask_test_blitter__DOT__write_reg__559__data 
        = __Vtask_test_blitter__DOT__blt_reg__558__data;
    __Vtask_test_blitter__DOT__write_reg__559__addr 
        = (0x0000ffffU & ((IData)(0xba83U) + __Vtask_test_blitter__DOT__blt_reg__558__off));
    Vtest_blitter___024root____VbeforeTrig_h603fc354__0(vlSelf, 
                                                        "@(posedge test_blitter.clk)");
    co_await vlSelfRef.__VtrigSched_h603fc354__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge test_blitter.clk)", 
                                                         "test_blitter.sv", 
                                                         113);
    vlSelfRef.test_blitter__DOT__cpu_addr = __Vtask_test_blitter__DOT__write_reg__559__addr;
    vlSelfRef.test_blitter__DOT__cpu_wdata = __Vtask_test_blitter__DOT__write_reg__559__data;
    vlSelfRef.test_blitter__DOT__cpu_we = 1U;
    Vtest_blitter___024root____VbeforeTrig_h603fc354__0(vlSelf, 
                                                        "@(posedge test_blitter.clk)");
    co_await vlSelfRef.__VtrigSched_h603fc354__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge test_blitter.clk)", 
                                                         "test_blitter.sv", 
                                                         115);
    vlSelfRef.test_blitter__DOT__cpu_we = 0U;
    __Vtask_test_blitter__DOT__wait_blt_done__560__test_blitter__DOT__unnamedblk1_1__DOT____Vrepeat0 = 0;
    __Vtask_test_blitter__DOT__wait_blt_done__560__test_blitter__DOT__unnamedblk1_2__DOT____Vrepeat1 = 0;
    __Vtask_test_blitter__DOT__wait_blt_done__560__test_blitter__DOT__unnamedblk1_1__DOT____Vrepeat0 = 2U;
    while (VL_LTS_III(32, 0U, __Vtask_test_blitter__DOT__wait_blt_done__560__test_blitter__DOT__unnamedblk1_1__DOT____Vrepeat0)) {
        Vtest_blitter___024root____VbeforeTrig_h603fc354__0(vlSelf, 
                                                            "@(posedge test_blitter.clk)");
        co_await vlSelfRef.__VtrigSched_h603fc354__0.trigger(0U, 
                                                             nullptr, 
                                                             "@(posedge test_blitter.clk)", 
                                                             "test_blitter.sv", 
                                                             128);
        __Vtask_test_blitter__DOT__wait_blt_done__560__test_blitter__DOT__unnamedblk1_1__DOT____Vrepeat0 
            = (__Vtask_test_blitter__DOT__wait_blt_done__560__test_blitter__DOT__unnamedblk1_1__DOT____Vrepeat0 
               - (IData)(1U));
    }
    while (((0U != (IData)(vlSelfRef.test_blitter__DOT__dut__DOT__state)) 
            & (6U != (IData)(vlSelfRef.test_blitter__DOT__dut__DOT__state)))) {
        Vtest_blitter___024root____VbeforeTrig_h603fc354__0(vlSelf, 
                                                            "@(posedge test_blitter.clk)");
        co_await vlSelfRef.__VtrigSched_h603fc354__0.trigger(0U, 
                                                             nullptr, 
                                                             "@(posedge test_blitter.clk)", 
                                                             "test_blitter.sv", 
                                                             130);
    }
    __Vtask_test_blitter__DOT__wait_blt_done__560__test_blitter__DOT__unnamedblk1_2__DOT____Vrepeat1 = 2U;
    while (VL_LTS_III(32, 0U, __Vtask_test_blitter__DOT__wait_blt_done__560__test_blitter__DOT__unnamedblk1_2__DOT____Vrepeat1)) {
        Vtest_blitter___024root____VbeforeTrig_h603fc354__0(vlSelf, 
                                                            "@(posedge test_blitter.clk)");
        co_await vlSelfRef.__VtrigSched_h603fc354__0.trigger(0U, 
                                                             nullptr, 
                                                             "@(posedge test_blitter.clk)", 
                                                             "test_blitter.sv", 
                                                             131);
        __Vtask_test_blitter__DOT__wait_blt_done__560__test_blitter__DOT__unnamedblk1_2__DOT____Vrepeat1 
            = (__Vtask_test_blitter__DOT__wait_blt_done__560__test_blitter__DOT__unnamedblk1_2__DOT____Vrepeat1 
               - (IData)(1U));
    }
    __Vtask_test_blitter__DOT__check__561__condition 
        = (3U == vlSelfRef.test_blitter__DOT__dut__DOT__regs[1U]);
    vlSelfRef.__Vtask_test_blitter__DOT__check__561__name = "dst range: status error"s;
    vlSelfRef.test_blitter__DOT__test_num = ((IData)(1U) 
                                             + vlSelfRef.test_blitter__DOT__test_num);
    if (__Vtask_test_blitter__DOT__check__561__condition) {
        VL_WRITEF_NX("  PASS [%0d] %@\n",0,32,vlSelfRef.test_blitter__DOT__test_num,
                     -1,&(vlSelfRef.__Vtask_test_blitter__DOT__check__561__name));
        vlSelfRef.test_blitter__DOT__pass_count = ((IData)(1U) 
                                                   + vlSelfRef.test_blitter__DOT__pass_count);
    } else {
        VL_WRITEF_NX("  FAIL [%0d] %@\n",0,32,vlSelfRef.test_blitter__DOT__test_num,
                     -1,&(vlSelfRef.__Vtask_test_blitter__DOT__check__561__name));
        vlSelfRef.test_blitter__DOT__fail_count = ((IData)(1U) 
                                                   + vlSelfRef.test_blitter__DOT__fail_count);
    }
    __Vtask_test_blitter__DOT__check__562__condition 
        = (3U == vlSelfRef.test_blitter__DOT__dut__DOT__regs[2U]);
    vlSelfRef.__Vtask_test_blitter__DOT__check__562__name = "dst range: errcode = range"s;
    vlSelfRef.test_blitter__DOT__test_num = ((IData)(1U) 
                                             + vlSelfRef.test_blitter__DOT__test_num);
    if (__Vtask_test_blitter__DOT__check__562__condition) {
        VL_WRITEF_NX("  PASS [%0d] %@\n",0,32,vlSelfRef.test_blitter__DOT__test_num,
                     -1,&(vlSelfRef.__Vtask_test_blitter__DOT__check__562__name));
        vlSelfRef.test_blitter__DOT__pass_count = ((IData)(1U) 
                                                   + vlSelfRef.test_blitter__DOT__pass_count);
    } else {
        VL_WRITEF_NX("  FAIL [%0d] %@\n",0,32,vlSelfRef.test_blitter__DOT__test_num,
                     -1,&(vlSelfRef.__Vtask_test_blitter__DOT__check__562__name));
        vlSelfRef.test_blitter__DOT__fail_count = ((IData)(1U) 
                                                   + vlSelfRef.test_blitter__DOT__fail_count);
    }
    VL_WRITEF_NX("Test: Write protection\n",0);
    __Vtask_test_blitter__DOT__setup_fill__563__fill_val = 0x000000ffU;
    __Vtask_test_blitter__DOT__setup_fill__563__height = 1U;
    __Vtask_test_blitter__DOT__setup_fill__563__width = 0x0000000aU;
    __Vtask_test_blitter__DOT__setup_fill__563__dst_stride = 0x00000100U;
    __Vtask_test_blitter__DOT__setup_fill__563__dst_addr = 0x0000c000U;
    __Vtask_test_blitter__DOT__setup_fill__563__dst_space = 0U;
    __Vtask_test_blitter__DOT__blt_reg__564__data = 
        (0x000000ffU & __Vtask_test_blitter__DOT__setup_fill__563__dst_space);
    __Vtask_test_blitter__DOT__blt_reg__564__off = 4U;
    __Vtask_test_blitter__DOT__write_reg__565__data 
        = __Vtask_test_blitter__DOT__blt_reg__564__data;
    __Vtask_test_blitter__DOT__write_reg__565__addr 
        = (0x0000ffffU & ((IData)(0xba83U) + __Vtask_test_blitter__DOT__blt_reg__564__off));
    Vtest_blitter___024root____VbeforeTrig_h603fc354__0(vlSelf, 
                                                        "@(posedge test_blitter.clk)");
    co_await vlSelfRef.__VtrigSched_h603fc354__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge test_blitter.clk)", 
                                                         "test_blitter.sv", 
                                                         113);
    vlSelfRef.test_blitter__DOT__cpu_addr = __Vtask_test_blitter__DOT__write_reg__565__addr;
    vlSelfRef.test_blitter__DOT__cpu_wdata = __Vtask_test_blitter__DOT__write_reg__565__data;
    vlSelfRef.test_blitter__DOT__cpu_we = 1U;
    Vtest_blitter___024root____VbeforeTrig_h603fc354__0(vlSelf, 
                                                        "@(posedge test_blitter.clk)");
    co_await vlSelfRef.__VtrigSched_h603fc354__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge test_blitter.clk)", 
                                                         "test_blitter.sv", 
                                                         115);
    vlSelfRef.test_blitter__DOT__cpu_we = 0U;
    __Vtask_test_blitter__DOT__blt_reg__566__data = 
        (0x000000ffU & __Vtask_test_blitter__DOT__setup_fill__563__dst_addr);
    __Vtask_test_blitter__DOT__blt_reg__566__off = 8U;
    __Vtask_test_blitter__DOT__write_reg__567__data 
        = __Vtask_test_blitter__DOT__blt_reg__566__data;
    __Vtask_test_blitter__DOT__write_reg__567__addr 
        = (0x0000ffffU & ((IData)(0xba83U) + __Vtask_test_blitter__DOT__blt_reg__566__off));
    Vtest_blitter___024root____VbeforeTrig_h603fc354__0(vlSelf, 
                                                        "@(posedge test_blitter.clk)");
    co_await vlSelfRef.__VtrigSched_h603fc354__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge test_blitter.clk)", 
                                                         "test_blitter.sv", 
                                                         113);
    vlSelfRef.test_blitter__DOT__cpu_addr = __Vtask_test_blitter__DOT__write_reg__567__addr;
    vlSelfRef.test_blitter__DOT__cpu_wdata = __Vtask_test_blitter__DOT__write_reg__567__data;
    vlSelfRef.test_blitter__DOT__cpu_we = 1U;
    Vtest_blitter___024root____VbeforeTrig_h603fc354__0(vlSelf, 
                                                        "@(posedge test_blitter.clk)");
    co_await vlSelfRef.__VtrigSched_h603fc354__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge test_blitter.clk)", 
                                                         "test_blitter.sv", 
                                                         115);
    vlSelfRef.test_blitter__DOT__cpu_we = 0U;
    __Vtask_test_blitter__DOT__blt_reg__568__data = 
        (0x000000ffU & (__Vtask_test_blitter__DOT__setup_fill__563__dst_addr 
                        >> 8U));
    __Vtask_test_blitter__DOT__blt_reg__568__off = 9U;
    __Vtask_test_blitter__DOT__write_reg__569__data 
        = __Vtask_test_blitter__DOT__blt_reg__568__data;
    __Vtask_test_blitter__DOT__write_reg__569__addr 
        = (0x0000ffffU & ((IData)(0xba83U) + __Vtask_test_blitter__DOT__blt_reg__568__off));
    Vtest_blitter___024root____VbeforeTrig_h603fc354__0(vlSelf, 
                                                        "@(posedge test_blitter.clk)");
    co_await vlSelfRef.__VtrigSched_h603fc354__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge test_blitter.clk)", 
                                                         "test_blitter.sv", 
                                                         113);
    vlSelfRef.test_blitter__DOT__cpu_addr = __Vtask_test_blitter__DOT__write_reg__569__addr;
    vlSelfRef.test_blitter__DOT__cpu_wdata = __Vtask_test_blitter__DOT__write_reg__569__data;
    vlSelfRef.test_blitter__DOT__cpu_we = 1U;
    Vtest_blitter___024root____VbeforeTrig_h603fc354__0(vlSelf, 
                                                        "@(posedge test_blitter.clk)");
    co_await vlSelfRef.__VtrigSched_h603fc354__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge test_blitter.clk)", 
                                                         "test_blitter.sv", 
                                                         115);
    vlSelfRef.test_blitter__DOT__cpu_we = 0U;
    __Vtask_test_blitter__DOT__blt_reg__570__data = 
        (0x000000ffU & (__Vtask_test_blitter__DOT__setup_fill__563__dst_addr 
                        >> 0x10U));
    __Vtask_test_blitter__DOT__blt_reg__570__off = 0x0000000aU;
    __Vtask_test_blitter__DOT__write_reg__571__data 
        = __Vtask_test_blitter__DOT__blt_reg__570__data;
    __Vtask_test_blitter__DOT__write_reg__571__addr 
        = (0x0000ffffU & ((IData)(0xba83U) + __Vtask_test_blitter__DOT__blt_reg__570__off));
    Vtest_blitter___024root____VbeforeTrig_h603fc354__0(vlSelf, 
                                                        "@(posedge test_blitter.clk)");
    co_await vlSelfRef.__VtrigSched_h603fc354__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge test_blitter.clk)", 
                                                         "test_blitter.sv", 
                                                         113);
    vlSelfRef.test_blitter__DOT__cpu_addr = __Vtask_test_blitter__DOT__write_reg__571__addr;
    vlSelfRef.test_blitter__DOT__cpu_wdata = __Vtask_test_blitter__DOT__write_reg__571__data;
    vlSelfRef.test_blitter__DOT__cpu_we = 1U;
    Vtest_blitter___024root____VbeforeTrig_h603fc354__0(vlSelf, 
                                                        "@(posedge test_blitter.clk)");
    co_await vlSelfRef.__VtrigSched_h603fc354__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge test_blitter.clk)", 
                                                         "test_blitter.sv", 
                                                         115);
    vlSelfRef.test_blitter__DOT__cpu_we = 0U;
    __Vtask_test_blitter__DOT__blt_reg__572__data = 
        (0x000000ffU & __Vtask_test_blitter__DOT__setup_fill__563__width);
    __Vtask_test_blitter__DOT__blt_reg__572__off = 0x0000000bU;
    __Vtask_test_blitter__DOT__write_reg__573__data 
        = __Vtask_test_blitter__DOT__blt_reg__572__data;
    __Vtask_test_blitter__DOT__write_reg__573__addr 
        = (0x0000ffffU & ((IData)(0xba83U) + __Vtask_test_blitter__DOT__blt_reg__572__off));
    Vtest_blitter___024root____VbeforeTrig_h603fc354__0(vlSelf, 
                                                        "@(posedge test_blitter.clk)");
    co_await vlSelfRef.__VtrigSched_h603fc354__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge test_blitter.clk)", 
                                                         "test_blitter.sv", 
                                                         113);
    vlSelfRef.test_blitter__DOT__cpu_addr = __Vtask_test_blitter__DOT__write_reg__573__addr;
    vlSelfRef.test_blitter__DOT__cpu_wdata = __Vtask_test_blitter__DOT__write_reg__573__data;
    vlSelfRef.test_blitter__DOT__cpu_we = 1U;
    Vtest_blitter___024root____VbeforeTrig_h603fc354__0(vlSelf, 
                                                        "@(posedge test_blitter.clk)");
    co_await vlSelfRef.__VtrigSched_h603fc354__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge test_blitter.clk)", 
                                                         "test_blitter.sv", 
                                                         115);
    vlSelfRef.test_blitter__DOT__cpu_we = 0U;
    __Vtask_test_blitter__DOT__blt_reg__574__data = 
        (0x000000ffU & (__Vtask_test_blitter__DOT__setup_fill__563__width 
                        >> 8U));
    __Vtask_test_blitter__DOT__blt_reg__574__off = 0x0000000cU;
    __Vtask_test_blitter__DOT__write_reg__575__data 
        = __Vtask_test_blitter__DOT__blt_reg__574__data;
    __Vtask_test_blitter__DOT__write_reg__575__addr 
        = (0x0000ffffU & ((IData)(0xba83U) + __Vtask_test_blitter__DOT__blt_reg__574__off));
    Vtest_blitter___024root____VbeforeTrig_h603fc354__0(vlSelf, 
                                                        "@(posedge test_blitter.clk)");
    co_await vlSelfRef.__VtrigSched_h603fc354__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge test_blitter.clk)", 
                                                         "test_blitter.sv", 
                                                         113);
    vlSelfRef.test_blitter__DOT__cpu_addr = __Vtask_test_blitter__DOT__write_reg__575__addr;
    vlSelfRef.test_blitter__DOT__cpu_wdata = __Vtask_test_blitter__DOT__write_reg__575__data;
    vlSelfRef.test_blitter__DOT__cpu_we = 1U;
    Vtest_blitter___024root____VbeforeTrig_h603fc354__0(vlSelf, 
                                                        "@(posedge test_blitter.clk)");
    co_await vlSelfRef.__VtrigSched_h603fc354__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge test_blitter.clk)", 
                                                         "test_blitter.sv", 
                                                         115);
    vlSelfRef.test_blitter__DOT__cpu_we = 0U;
    __Vtask_test_blitter__DOT__blt_reg__576__data = 
        (0x000000ffU & __Vtask_test_blitter__DOT__setup_fill__563__height);
    __Vtask_test_blitter__DOT__blt_reg__576__off = 0x0000000dU;
    __Vtask_test_blitter__DOT__write_reg__577__data 
        = __Vtask_test_blitter__DOT__blt_reg__576__data;
    __Vtask_test_blitter__DOT__write_reg__577__addr 
        = (0x0000ffffU & ((IData)(0xba83U) + __Vtask_test_blitter__DOT__blt_reg__576__off));
    Vtest_blitter___024root____VbeforeTrig_h603fc354__0(vlSelf, 
                                                        "@(posedge test_blitter.clk)");
    co_await vlSelfRef.__VtrigSched_h603fc354__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge test_blitter.clk)", 
                                                         "test_blitter.sv", 
                                                         113);
    vlSelfRef.test_blitter__DOT__cpu_addr = __Vtask_test_blitter__DOT__write_reg__577__addr;
    vlSelfRef.test_blitter__DOT__cpu_wdata = __Vtask_test_blitter__DOT__write_reg__577__data;
    vlSelfRef.test_blitter__DOT__cpu_we = 1U;
    Vtest_blitter___024root____VbeforeTrig_h603fc354__0(vlSelf, 
                                                        "@(posedge test_blitter.clk)");
    co_await vlSelfRef.__VtrigSched_h603fc354__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge test_blitter.clk)", 
                                                         "test_blitter.sv", 
                                                         115);
    vlSelfRef.test_blitter__DOT__cpu_we = 0U;
    __Vtask_test_blitter__DOT__blt_reg__578__data = 
        (0x000000ffU & (__Vtask_test_blitter__DOT__setup_fill__563__height 
                        >> 8U));
    __Vtask_test_blitter__DOT__blt_reg__578__off = 0x0000000eU;
    __Vtask_test_blitter__DOT__write_reg__579__data 
        = __Vtask_test_blitter__DOT__blt_reg__578__data;
    __Vtask_test_blitter__DOT__write_reg__579__addr 
        = (0x0000ffffU & ((IData)(0xba83U) + __Vtask_test_blitter__DOT__blt_reg__578__off));
    Vtest_blitter___024root____VbeforeTrig_h603fc354__0(vlSelf, 
                                                        "@(posedge test_blitter.clk)");
    co_await vlSelfRef.__VtrigSched_h603fc354__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge test_blitter.clk)", 
                                                         "test_blitter.sv", 
                                                         113);
    vlSelfRef.test_blitter__DOT__cpu_addr = __Vtask_test_blitter__DOT__write_reg__579__addr;
    vlSelfRef.test_blitter__DOT__cpu_wdata = __Vtask_test_blitter__DOT__write_reg__579__data;
    vlSelfRef.test_blitter__DOT__cpu_we = 1U;
    Vtest_blitter___024root____VbeforeTrig_h603fc354__0(vlSelf, 
                                                        "@(posedge test_blitter.clk)");
    co_await vlSelfRef.__VtrigSched_h603fc354__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge test_blitter.clk)", 
                                                         "test_blitter.sv", 
                                                         115);
    vlSelfRef.test_blitter__DOT__cpu_we = 0U;
    __Vtask_test_blitter__DOT__blt_reg__580__data = 
        (0x000000ffU & __Vtask_test_blitter__DOT__setup_fill__563__dst_stride);
    __Vtask_test_blitter__DOT__blt_reg__580__off = 0x00000011U;
    __Vtask_test_blitter__DOT__write_reg__581__data 
        = __Vtask_test_blitter__DOT__blt_reg__580__data;
    __Vtask_test_blitter__DOT__write_reg__581__addr 
        = (0x0000ffffU & ((IData)(0xba83U) + __Vtask_test_blitter__DOT__blt_reg__580__off));
    Vtest_blitter___024root____VbeforeTrig_h603fc354__0(vlSelf, 
                                                        "@(posedge test_blitter.clk)");
    co_await vlSelfRef.__VtrigSched_h603fc354__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge test_blitter.clk)", 
                                                         "test_blitter.sv", 
                                                         113);
    vlSelfRef.test_blitter__DOT__cpu_addr = __Vtask_test_blitter__DOT__write_reg__581__addr;
    vlSelfRef.test_blitter__DOT__cpu_wdata = __Vtask_test_blitter__DOT__write_reg__581__data;
    vlSelfRef.test_blitter__DOT__cpu_we = 1U;
    Vtest_blitter___024root____VbeforeTrig_h603fc354__0(vlSelf, 
                                                        "@(posedge test_blitter.clk)");
    co_await vlSelfRef.__VtrigSched_h603fc354__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge test_blitter.clk)", 
                                                         "test_blitter.sv", 
                                                         115);
    vlSelfRef.test_blitter__DOT__cpu_we = 0U;
    __Vtask_test_blitter__DOT__blt_reg__582__data = 
        (0x000000ffU & (__Vtask_test_blitter__DOT__setup_fill__563__dst_stride 
                        >> 8U));
    __Vtask_test_blitter__DOT__blt_reg__582__off = 0x00000012U;
    __Vtask_test_blitter__DOT__write_reg__583__data 
        = __Vtask_test_blitter__DOT__blt_reg__582__data;
    __Vtask_test_blitter__DOT__write_reg__583__addr 
        = (0x0000ffffU & ((IData)(0xba83U) + __Vtask_test_blitter__DOT__blt_reg__582__off));
    Vtest_blitter___024root____VbeforeTrig_h603fc354__0(vlSelf, 
                                                        "@(posedge test_blitter.clk)");
    co_await vlSelfRef.__VtrigSched_h603fc354__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge test_blitter.clk)", 
                                                         "test_blitter.sv", 
                                                         113);
    vlSelfRef.test_blitter__DOT__cpu_addr = __Vtask_test_blitter__DOT__write_reg__583__addr;
    vlSelfRef.test_blitter__DOT__cpu_wdata = __Vtask_test_blitter__DOT__write_reg__583__data;
    vlSelfRef.test_blitter__DOT__cpu_we = 1U;
    Vtest_blitter___024root____VbeforeTrig_h603fc354__0(vlSelf, 
                                                        "@(posedge test_blitter.clk)");
    co_await vlSelfRef.__VtrigSched_h603fc354__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge test_blitter.clk)", 
                                                         "test_blitter.sv", 
                                                         115);
    vlSelfRef.test_blitter__DOT__cpu_we = 0U;
    __Vtask_test_blitter__DOT__blt_reg__584__data = 1U;
    __Vtask_test_blitter__DOT__blt_reg__584__off = 0x00000013U;
    __Vtask_test_blitter__DOT__write_reg__585__data 
        = __Vtask_test_blitter__DOT__blt_reg__584__data;
    __Vtask_test_blitter__DOT__write_reg__585__addr 
        = (0x0000ffffU & ((IData)(0xba83U) + __Vtask_test_blitter__DOT__blt_reg__584__off));
    Vtest_blitter___024root____VbeforeTrig_h603fc354__0(vlSelf, 
                                                        "@(posedge test_blitter.clk)");
    co_await vlSelfRef.__VtrigSched_h603fc354__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge test_blitter.clk)", 
                                                         "test_blitter.sv", 
                                                         113);
    vlSelfRef.test_blitter__DOT__cpu_addr = __Vtask_test_blitter__DOT__write_reg__585__addr;
    vlSelfRef.test_blitter__DOT__cpu_wdata = __Vtask_test_blitter__DOT__write_reg__585__data;
    vlSelfRef.test_blitter__DOT__cpu_we = 1U;
    Vtest_blitter___024root____VbeforeTrig_h603fc354__0(vlSelf, 
                                                        "@(posedge test_blitter.clk)");
    co_await vlSelfRef.__VtrigSched_h603fc354__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge test_blitter.clk)", 
                                                         "test_blitter.sv", 
                                                         115);
    vlSelfRef.test_blitter__DOT__cpu_we = 0U;
    __Vtask_test_blitter__DOT__blt_reg__586__data = 
        (0x000000ffU & __Vtask_test_blitter__DOT__setup_fill__563__fill_val);
    __Vtask_test_blitter__DOT__blt_reg__586__off = 0x00000014U;
    __Vtask_test_blitter__DOT__write_reg__587__data 
        = __Vtask_test_blitter__DOT__blt_reg__586__data;
    __Vtask_test_blitter__DOT__write_reg__587__addr 
        = (0x0000ffffU & ((IData)(0xba83U) + __Vtask_test_blitter__DOT__blt_reg__586__off));
    Vtest_blitter___024root____VbeforeTrig_h603fc354__0(vlSelf, 
                                                        "@(posedge test_blitter.clk)");
    co_await vlSelfRef.__VtrigSched_h603fc354__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge test_blitter.clk)", 
                                                         "test_blitter.sv", 
                                                         113);
    vlSelfRef.test_blitter__DOT__cpu_addr = __Vtask_test_blitter__DOT__write_reg__587__addr;
    vlSelfRef.test_blitter__DOT__cpu_wdata = __Vtask_test_blitter__DOT__write_reg__587__data;
    vlSelfRef.test_blitter__DOT__cpu_we = 1U;
    Vtest_blitter___024root____VbeforeTrig_h603fc354__0(vlSelf, 
                                                        "@(posedge test_blitter.clk)");
    co_await vlSelfRef.__VtrigSched_h603fc354__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge test_blitter.clk)", 
                                                         "test_blitter.sv", 
                                                         115);
    vlSelfRef.test_blitter__DOT__cpu_we = 0U;
    __Vtask_test_blitter__DOT__blt_reg__589__data = 1U;
    __Vtask_test_blitter__DOT__blt_reg__589__off = 0U;
    __Vtask_test_blitter__DOT__write_reg__590__data 
        = __Vtask_test_blitter__DOT__blt_reg__589__data;
    __Vtask_test_blitter__DOT__write_reg__590__addr 
        = (0x0000ffffU & ((IData)(0xba83U) + __Vtask_test_blitter__DOT__blt_reg__589__off));
    Vtest_blitter___024root____VbeforeTrig_h603fc354__0(vlSelf, 
                                                        "@(posedge test_blitter.clk)");
    co_await vlSelfRef.__VtrigSched_h603fc354__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge test_blitter.clk)", 
                                                         "test_blitter.sv", 
                                                         113);
    vlSelfRef.test_blitter__DOT__cpu_addr = __Vtask_test_blitter__DOT__write_reg__590__addr;
    vlSelfRef.test_blitter__DOT__cpu_wdata = __Vtask_test_blitter__DOT__write_reg__590__data;
    vlSelfRef.test_blitter__DOT__cpu_we = 1U;
    Vtest_blitter___024root____VbeforeTrig_h603fc354__0(vlSelf, 
                                                        "@(posedge test_blitter.clk)");
    co_await vlSelfRef.__VtrigSched_h603fc354__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge test_blitter.clk)", 
                                                         "test_blitter.sv", 
                                                         115);
    vlSelfRef.test_blitter__DOT__cpu_we = 0U;
    __Vtask_test_blitter__DOT__wait_blt_done__591__test_blitter__DOT__unnamedblk1_1__DOT____Vrepeat0 = 0;
    __Vtask_test_blitter__DOT__wait_blt_done__591__test_blitter__DOT__unnamedblk1_2__DOT____Vrepeat1 = 0;
    __Vtask_test_blitter__DOT__wait_blt_done__591__test_blitter__DOT__unnamedblk1_1__DOT____Vrepeat0 = 2U;
    while (VL_LTS_III(32, 0U, __Vtask_test_blitter__DOT__wait_blt_done__591__test_blitter__DOT__unnamedblk1_1__DOT____Vrepeat0)) {
        Vtest_blitter___024root____VbeforeTrig_h603fc354__0(vlSelf, 
                                                            "@(posedge test_blitter.clk)");
        co_await vlSelfRef.__VtrigSched_h603fc354__0.trigger(0U, 
                                                             nullptr, 
                                                             "@(posedge test_blitter.clk)", 
                                                             "test_blitter.sv", 
                                                             128);
        __Vtask_test_blitter__DOT__wait_blt_done__591__test_blitter__DOT__unnamedblk1_1__DOT____Vrepeat0 
            = (__Vtask_test_blitter__DOT__wait_blt_done__591__test_blitter__DOT__unnamedblk1_1__DOT____Vrepeat0 
               - (IData)(1U));
    }
    while (((0U != (IData)(vlSelfRef.test_blitter__DOT__dut__DOT__state)) 
            & (6U != (IData)(vlSelfRef.test_blitter__DOT__dut__DOT__state)))) {
        Vtest_blitter___024root____VbeforeTrig_h603fc354__0(vlSelf, 
                                                            "@(posedge test_blitter.clk)");
        co_await vlSelfRef.__VtrigSched_h603fc354__0.trigger(0U, 
                                                             nullptr, 
                                                             "@(posedge test_blitter.clk)", 
                                                             "test_blitter.sv", 
                                                             130);
    }
    __Vtask_test_blitter__DOT__wait_blt_done__591__test_blitter__DOT__unnamedblk1_2__DOT____Vrepeat1 = 2U;
    while (VL_LTS_III(32, 0U, __Vtask_test_blitter__DOT__wait_blt_done__591__test_blitter__DOT__unnamedblk1_2__DOT____Vrepeat1)) {
        Vtest_blitter___024root____VbeforeTrig_h603fc354__0(vlSelf, 
                                                            "@(posedge test_blitter.clk)");
        co_await vlSelfRef.__VtrigSched_h603fc354__0.trigger(0U, 
                                                             nullptr, 
                                                             "@(posedge test_blitter.clk)", 
                                                             "test_blitter.sv", 
                                                             131);
        __Vtask_test_blitter__DOT__wait_blt_done__591__test_blitter__DOT__unnamedblk1_2__DOT____Vrepeat1 
            = (__Vtask_test_blitter__DOT__wait_blt_done__591__test_blitter__DOT__unnamedblk1_2__DOT____Vrepeat1 
               - (IData)(1U));
    }
    __Vtask_test_blitter__DOT__check__592__condition 
        = (3U == vlSelfRef.test_blitter__DOT__dut__DOT__regs[1U]);
    vlSelfRef.__Vtask_test_blitter__DOT__check__592__name = "rom write: status error"s;
    vlSelfRef.test_blitter__DOT__test_num = ((IData)(1U) 
                                             + vlSelfRef.test_blitter__DOT__test_num);
    if (__Vtask_test_blitter__DOT__check__592__condition) {
        VL_WRITEF_NX("  PASS [%0d] %@\n",0,32,vlSelfRef.test_blitter__DOT__test_num,
                     -1,&(vlSelfRef.__Vtask_test_blitter__DOT__check__592__name));
        vlSelfRef.test_blitter__DOT__pass_count = ((IData)(1U) 
                                                   + vlSelfRef.test_blitter__DOT__pass_count);
    } else {
        VL_WRITEF_NX("  FAIL [%0d] %@\n",0,32,vlSelfRef.test_blitter__DOT__test_num,
                     -1,&(vlSelfRef.__Vtask_test_blitter__DOT__check__592__name));
        vlSelfRef.test_blitter__DOT__fail_count = ((IData)(1U) 
                                                   + vlSelfRef.test_blitter__DOT__fail_count);
    }
    __Vtask_test_blitter__DOT__check__593__condition 
        = (5U == vlSelfRef.test_blitter__DOT__dut__DOT__regs[2U]);
    vlSelfRef.__Vtask_test_blitter__DOT__check__593__name = "rom write: errcode = writeprot"s;
    vlSelfRef.test_blitter__DOT__test_num = ((IData)(1U) 
                                             + vlSelfRef.test_blitter__DOT__test_num);
    if (__Vtask_test_blitter__DOT__check__593__condition) {
        VL_WRITEF_NX("  PASS [%0d] %@\n",0,32,vlSelfRef.test_blitter__DOT__test_num,
                     -1,&(vlSelfRef.__Vtask_test_blitter__DOT__check__593__name));
        vlSelfRef.test_blitter__DOT__pass_count = ((IData)(1U) 
                                                   + vlSelfRef.test_blitter__DOT__pass_count);
    } else {
        VL_WRITEF_NX("  FAIL [%0d] %@\n",0,32,vlSelfRef.test_blitter__DOT__test_num,
                     -1,&(vlSelfRef.__Vtask_test_blitter__DOT__check__593__name));
        vlSelfRef.test_blitter__DOT__fail_count = ((IData)(1U) 
                                                   + vlSelfRef.test_blitter__DOT__fail_count);
    }
    __Vtask_test_blitter__DOT__setup_fill__594__fill_val = 0x00000042U;
    __Vtask_test_blitter__DOT__setup_fill__594__height = 1U;
    __Vtask_test_blitter__DOT__setup_fill__594__width = 0x00000010U;
    __Vtask_test_blitter__DOT__setup_fill__594__dst_stride = 0x00000010U;
    __Vtask_test_blitter__DOT__setup_fill__594__dst_addr = 0x0000bff0U;
    __Vtask_test_blitter__DOT__setup_fill__594__dst_space = 0U;
    __Vtask_test_blitter__DOT__blt_reg__595__data = 
        (0x000000ffU & __Vtask_test_blitter__DOT__setup_fill__594__dst_space);
    __Vtask_test_blitter__DOT__blt_reg__595__off = 4U;
    __Vtask_test_blitter__DOT__write_reg__596__data 
        = __Vtask_test_blitter__DOT__blt_reg__595__data;
    __Vtask_test_blitter__DOT__write_reg__596__addr 
        = (0x0000ffffU & ((IData)(0xba83U) + __Vtask_test_blitter__DOT__blt_reg__595__off));
    Vtest_blitter___024root____VbeforeTrig_h603fc354__0(vlSelf, 
                                                        "@(posedge test_blitter.clk)");
    co_await vlSelfRef.__VtrigSched_h603fc354__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge test_blitter.clk)", 
                                                         "test_blitter.sv", 
                                                         113);
    vlSelfRef.test_blitter__DOT__cpu_addr = __Vtask_test_blitter__DOT__write_reg__596__addr;
    vlSelfRef.test_blitter__DOT__cpu_wdata = __Vtask_test_blitter__DOT__write_reg__596__data;
    vlSelfRef.test_blitter__DOT__cpu_we = 1U;
    Vtest_blitter___024root____VbeforeTrig_h603fc354__0(vlSelf, 
                                                        "@(posedge test_blitter.clk)");
    co_await vlSelfRef.__VtrigSched_h603fc354__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge test_blitter.clk)", 
                                                         "test_blitter.sv", 
                                                         115);
    vlSelfRef.test_blitter__DOT__cpu_we = 0U;
    __Vtask_test_blitter__DOT__blt_reg__597__data = 
        (0x000000ffU & __Vtask_test_blitter__DOT__setup_fill__594__dst_addr);
    __Vtask_test_blitter__DOT__blt_reg__597__off = 8U;
    __Vtask_test_blitter__DOT__write_reg__598__data 
        = __Vtask_test_blitter__DOT__blt_reg__597__data;
    __Vtask_test_blitter__DOT__write_reg__598__addr 
        = (0x0000ffffU & ((IData)(0xba83U) + __Vtask_test_blitter__DOT__blt_reg__597__off));
    Vtest_blitter___024root____VbeforeTrig_h603fc354__0(vlSelf, 
                                                        "@(posedge test_blitter.clk)");
    co_await vlSelfRef.__VtrigSched_h603fc354__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge test_blitter.clk)", 
                                                         "test_blitter.sv", 
                                                         113);
    vlSelfRef.test_blitter__DOT__cpu_addr = __Vtask_test_blitter__DOT__write_reg__598__addr;
    vlSelfRef.test_blitter__DOT__cpu_wdata = __Vtask_test_blitter__DOT__write_reg__598__data;
    vlSelfRef.test_blitter__DOT__cpu_we = 1U;
    Vtest_blitter___024root____VbeforeTrig_h603fc354__0(vlSelf, 
                                                        "@(posedge test_blitter.clk)");
    co_await vlSelfRef.__VtrigSched_h603fc354__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge test_blitter.clk)", 
                                                         "test_blitter.sv", 
                                                         115);
    vlSelfRef.test_blitter__DOT__cpu_we = 0U;
    __Vtask_test_blitter__DOT__blt_reg__599__data = 
        (0x000000ffU & (__Vtask_test_blitter__DOT__setup_fill__594__dst_addr 
                        >> 8U));
    __Vtask_test_blitter__DOT__blt_reg__599__off = 9U;
    __Vtask_test_blitter__DOT__write_reg__600__data 
        = __Vtask_test_blitter__DOT__blt_reg__599__data;
    __Vtask_test_blitter__DOT__write_reg__600__addr 
        = (0x0000ffffU & ((IData)(0xba83U) + __Vtask_test_blitter__DOT__blt_reg__599__off));
    Vtest_blitter___024root____VbeforeTrig_h603fc354__0(vlSelf, 
                                                        "@(posedge test_blitter.clk)");
    co_await vlSelfRef.__VtrigSched_h603fc354__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge test_blitter.clk)", 
                                                         "test_blitter.sv", 
                                                         113);
    vlSelfRef.test_blitter__DOT__cpu_addr = __Vtask_test_blitter__DOT__write_reg__600__addr;
    vlSelfRef.test_blitter__DOT__cpu_wdata = __Vtask_test_blitter__DOT__write_reg__600__data;
    vlSelfRef.test_blitter__DOT__cpu_we = 1U;
    Vtest_blitter___024root____VbeforeTrig_h603fc354__0(vlSelf, 
                                                        "@(posedge test_blitter.clk)");
    co_await vlSelfRef.__VtrigSched_h603fc354__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge test_blitter.clk)", 
                                                         "test_blitter.sv", 
                                                         115);
    vlSelfRef.test_blitter__DOT__cpu_we = 0U;
    __Vtask_test_blitter__DOT__blt_reg__601__data = 
        (0x000000ffU & (__Vtask_test_blitter__DOT__setup_fill__594__dst_addr 
                        >> 0x10U));
    __Vtask_test_blitter__DOT__blt_reg__601__off = 0x0000000aU;
    __Vtask_test_blitter__DOT__write_reg__602__data 
        = __Vtask_test_blitter__DOT__blt_reg__601__data;
    __Vtask_test_blitter__DOT__write_reg__602__addr 
        = (0x0000ffffU & ((IData)(0xba83U) + __Vtask_test_blitter__DOT__blt_reg__601__off));
    Vtest_blitter___024root____VbeforeTrig_h603fc354__0(vlSelf, 
                                                        "@(posedge test_blitter.clk)");
    co_await vlSelfRef.__VtrigSched_h603fc354__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge test_blitter.clk)", 
                                                         "test_blitter.sv", 
                                                         113);
    vlSelfRef.test_blitter__DOT__cpu_addr = __Vtask_test_blitter__DOT__write_reg__602__addr;
    vlSelfRef.test_blitter__DOT__cpu_wdata = __Vtask_test_blitter__DOT__write_reg__602__data;
    vlSelfRef.test_blitter__DOT__cpu_we = 1U;
    Vtest_blitter___024root____VbeforeTrig_h603fc354__0(vlSelf, 
                                                        "@(posedge test_blitter.clk)");
    co_await vlSelfRef.__VtrigSched_h603fc354__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge test_blitter.clk)", 
                                                         "test_blitter.sv", 
                                                         115);
    vlSelfRef.test_blitter__DOT__cpu_we = 0U;
    __Vtask_test_blitter__DOT__blt_reg__603__data = 
        (0x000000ffU & __Vtask_test_blitter__DOT__setup_fill__594__width);
    __Vtask_test_blitter__DOT__blt_reg__603__off = 0x0000000bU;
    __Vtask_test_blitter__DOT__write_reg__604__data 
        = __Vtask_test_blitter__DOT__blt_reg__603__data;
    __Vtask_test_blitter__DOT__write_reg__604__addr 
        = (0x0000ffffU & ((IData)(0xba83U) + __Vtask_test_blitter__DOT__blt_reg__603__off));
    Vtest_blitter___024root____VbeforeTrig_h603fc354__0(vlSelf, 
                                                        "@(posedge test_blitter.clk)");
    co_await vlSelfRef.__VtrigSched_h603fc354__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge test_blitter.clk)", 
                                                         "test_blitter.sv", 
                                                         113);
    vlSelfRef.test_blitter__DOT__cpu_addr = __Vtask_test_blitter__DOT__write_reg__604__addr;
    vlSelfRef.test_blitter__DOT__cpu_wdata = __Vtask_test_blitter__DOT__write_reg__604__data;
    vlSelfRef.test_blitter__DOT__cpu_we = 1U;
    Vtest_blitter___024root____VbeforeTrig_h603fc354__0(vlSelf, 
                                                        "@(posedge test_blitter.clk)");
    co_await vlSelfRef.__VtrigSched_h603fc354__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge test_blitter.clk)", 
                                                         "test_blitter.sv", 
                                                         115);
    vlSelfRef.test_blitter__DOT__cpu_we = 0U;
    __Vtask_test_blitter__DOT__blt_reg__605__data = 
        (0x000000ffU & (__Vtask_test_blitter__DOT__setup_fill__594__width 
                        >> 8U));
    __Vtask_test_blitter__DOT__blt_reg__605__off = 0x0000000cU;
    __Vtask_test_blitter__DOT__write_reg__606__data 
        = __Vtask_test_blitter__DOT__blt_reg__605__data;
    __Vtask_test_blitter__DOT__write_reg__606__addr 
        = (0x0000ffffU & ((IData)(0xba83U) + __Vtask_test_blitter__DOT__blt_reg__605__off));
    Vtest_blitter___024root____VbeforeTrig_h603fc354__0(vlSelf, 
                                                        "@(posedge test_blitter.clk)");
    co_await vlSelfRef.__VtrigSched_h603fc354__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge test_blitter.clk)", 
                                                         "test_blitter.sv", 
                                                         113);
    vlSelfRef.test_blitter__DOT__cpu_addr = __Vtask_test_blitter__DOT__write_reg__606__addr;
    vlSelfRef.test_blitter__DOT__cpu_wdata = __Vtask_test_blitter__DOT__write_reg__606__data;
    vlSelfRef.test_blitter__DOT__cpu_we = 1U;
    Vtest_blitter___024root____VbeforeTrig_h603fc354__0(vlSelf, 
                                                        "@(posedge test_blitter.clk)");
    co_await vlSelfRef.__VtrigSched_h603fc354__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge test_blitter.clk)", 
                                                         "test_blitter.sv", 
                                                         115);
    vlSelfRef.test_blitter__DOT__cpu_we = 0U;
    __Vtask_test_blitter__DOT__blt_reg__607__data = 
        (0x000000ffU & __Vtask_test_blitter__DOT__setup_fill__594__height);
    __Vtask_test_blitter__DOT__blt_reg__607__off = 0x0000000dU;
    __Vtask_test_blitter__DOT__write_reg__608__data 
        = __Vtask_test_blitter__DOT__blt_reg__607__data;
    __Vtask_test_blitter__DOT__write_reg__608__addr 
        = (0x0000ffffU & ((IData)(0xba83U) + __Vtask_test_blitter__DOT__blt_reg__607__off));
    Vtest_blitter___024root____VbeforeTrig_h603fc354__0(vlSelf, 
                                                        "@(posedge test_blitter.clk)");
    co_await vlSelfRef.__VtrigSched_h603fc354__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge test_blitter.clk)", 
                                                         "test_blitter.sv", 
                                                         113);
    vlSelfRef.test_blitter__DOT__cpu_addr = __Vtask_test_blitter__DOT__write_reg__608__addr;
    vlSelfRef.test_blitter__DOT__cpu_wdata = __Vtask_test_blitter__DOT__write_reg__608__data;
    vlSelfRef.test_blitter__DOT__cpu_we = 1U;
    Vtest_blitter___024root____VbeforeTrig_h603fc354__0(vlSelf, 
                                                        "@(posedge test_blitter.clk)");
    co_await vlSelfRef.__VtrigSched_h603fc354__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge test_blitter.clk)", 
                                                         "test_blitter.sv", 
                                                         115);
    vlSelfRef.test_blitter__DOT__cpu_we = 0U;
    __Vtask_test_blitter__DOT__blt_reg__609__data = 
        (0x000000ffU & (__Vtask_test_blitter__DOT__setup_fill__594__height 
                        >> 8U));
    __Vtask_test_blitter__DOT__blt_reg__609__off = 0x0000000eU;
    __Vtask_test_blitter__DOT__write_reg__610__data 
        = __Vtask_test_blitter__DOT__blt_reg__609__data;
    __Vtask_test_blitter__DOT__write_reg__610__addr 
        = (0x0000ffffU & ((IData)(0xba83U) + __Vtask_test_blitter__DOT__blt_reg__609__off));
    Vtest_blitter___024root____VbeforeTrig_h603fc354__0(vlSelf, 
                                                        "@(posedge test_blitter.clk)");
    co_await vlSelfRef.__VtrigSched_h603fc354__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge test_blitter.clk)", 
                                                         "test_blitter.sv", 
                                                         113);
    vlSelfRef.test_blitter__DOT__cpu_addr = __Vtask_test_blitter__DOT__write_reg__610__addr;
    vlSelfRef.test_blitter__DOT__cpu_wdata = __Vtask_test_blitter__DOT__write_reg__610__data;
    vlSelfRef.test_blitter__DOT__cpu_we = 1U;
    Vtest_blitter___024root____VbeforeTrig_h603fc354__0(vlSelf, 
                                                        "@(posedge test_blitter.clk)");
    co_await vlSelfRef.__VtrigSched_h603fc354__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge test_blitter.clk)", 
                                                         "test_blitter.sv", 
                                                         115);
    vlSelfRef.test_blitter__DOT__cpu_we = 0U;
    __Vtask_test_blitter__DOT__blt_reg__611__data = 
        (0x000000ffU & __Vtask_test_blitter__DOT__setup_fill__594__dst_stride);
    __Vtask_test_blitter__DOT__blt_reg__611__off = 0x00000011U;
    __Vtask_test_blitter__DOT__write_reg__612__data 
        = __Vtask_test_blitter__DOT__blt_reg__611__data;
    __Vtask_test_blitter__DOT__write_reg__612__addr 
        = (0x0000ffffU & ((IData)(0xba83U) + __Vtask_test_blitter__DOT__blt_reg__611__off));
    Vtest_blitter___024root____VbeforeTrig_h603fc354__0(vlSelf, 
                                                        "@(posedge test_blitter.clk)");
    co_await vlSelfRef.__VtrigSched_h603fc354__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge test_blitter.clk)", 
                                                         "test_blitter.sv", 
                                                         113);
    vlSelfRef.test_blitter__DOT__cpu_addr = __Vtask_test_blitter__DOT__write_reg__612__addr;
    vlSelfRef.test_blitter__DOT__cpu_wdata = __Vtask_test_blitter__DOT__write_reg__612__data;
    vlSelfRef.test_blitter__DOT__cpu_we = 1U;
    Vtest_blitter___024root____VbeforeTrig_h603fc354__0(vlSelf, 
                                                        "@(posedge test_blitter.clk)");
    co_await vlSelfRef.__VtrigSched_h603fc354__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge test_blitter.clk)", 
                                                         "test_blitter.sv", 
                                                         115);
    vlSelfRef.test_blitter__DOT__cpu_we = 0U;
    __Vtask_test_blitter__DOT__blt_reg__613__data = 
        (0x000000ffU & (__Vtask_test_blitter__DOT__setup_fill__594__dst_stride 
                        >> 8U));
    __Vtask_test_blitter__DOT__blt_reg__613__off = 0x00000012U;
    __Vtask_test_blitter__DOT__write_reg__614__data 
        = __Vtask_test_blitter__DOT__blt_reg__613__data;
    __Vtask_test_blitter__DOT__write_reg__614__addr 
        = (0x0000ffffU & ((IData)(0xba83U) + __Vtask_test_blitter__DOT__blt_reg__613__off));
    Vtest_blitter___024root____VbeforeTrig_h603fc354__0(vlSelf, 
                                                        "@(posedge test_blitter.clk)");
    co_await vlSelfRef.__VtrigSched_h603fc354__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge test_blitter.clk)", 
                                                         "test_blitter.sv", 
                                                         113);
    vlSelfRef.test_blitter__DOT__cpu_addr = __Vtask_test_blitter__DOT__write_reg__614__addr;
    vlSelfRef.test_blitter__DOT__cpu_wdata = __Vtask_test_blitter__DOT__write_reg__614__data;
    vlSelfRef.test_blitter__DOT__cpu_we = 1U;
    Vtest_blitter___024root____VbeforeTrig_h603fc354__0(vlSelf, 
                                                        "@(posedge test_blitter.clk)");
    co_await vlSelfRef.__VtrigSched_h603fc354__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge test_blitter.clk)", 
                                                         "test_blitter.sv", 
                                                         115);
    vlSelfRef.test_blitter__DOT__cpu_we = 0U;
    __Vtask_test_blitter__DOT__blt_reg__615__data = 1U;
    __Vtask_test_blitter__DOT__blt_reg__615__off = 0x00000013U;
    __Vtask_test_blitter__DOT__write_reg__616__data 
        = __Vtask_test_blitter__DOT__blt_reg__615__data;
    __Vtask_test_blitter__DOT__write_reg__616__addr 
        = (0x0000ffffU & ((IData)(0xba83U) + __Vtask_test_blitter__DOT__blt_reg__615__off));
    Vtest_blitter___024root____VbeforeTrig_h603fc354__0(vlSelf, 
                                                        "@(posedge test_blitter.clk)");
    co_await vlSelfRef.__VtrigSched_h603fc354__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge test_blitter.clk)", 
                                                         "test_blitter.sv", 
                                                         113);
    vlSelfRef.test_blitter__DOT__cpu_addr = __Vtask_test_blitter__DOT__write_reg__616__addr;
    vlSelfRef.test_blitter__DOT__cpu_wdata = __Vtask_test_blitter__DOT__write_reg__616__data;
    vlSelfRef.test_blitter__DOT__cpu_we = 1U;
    Vtest_blitter___024root____VbeforeTrig_h603fc354__0(vlSelf, 
                                                        "@(posedge test_blitter.clk)");
    co_await vlSelfRef.__VtrigSched_h603fc354__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge test_blitter.clk)", 
                                                         "test_blitter.sv", 
                                                         115);
    vlSelfRef.test_blitter__DOT__cpu_we = 0U;
    __Vtask_test_blitter__DOT__blt_reg__617__data = 
        (0x000000ffU & __Vtask_test_blitter__DOT__setup_fill__594__fill_val);
    __Vtask_test_blitter__DOT__blt_reg__617__off = 0x00000014U;
    __Vtask_test_blitter__DOT__write_reg__618__data 
        = __Vtask_test_blitter__DOT__blt_reg__617__data;
    __Vtask_test_blitter__DOT__write_reg__618__addr 
        = (0x0000ffffU & ((IData)(0xba83U) + __Vtask_test_blitter__DOT__blt_reg__617__off));
    Vtest_blitter___024root____VbeforeTrig_h603fc354__0(vlSelf, 
                                                        "@(posedge test_blitter.clk)");
    co_await vlSelfRef.__VtrigSched_h603fc354__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge test_blitter.clk)", 
                                                         "test_blitter.sv", 
                                                         113);
    vlSelfRef.test_blitter__DOT__cpu_addr = __Vtask_test_blitter__DOT__write_reg__618__addr;
    vlSelfRef.test_blitter__DOT__cpu_wdata = __Vtask_test_blitter__DOT__write_reg__618__data;
    vlSelfRef.test_blitter__DOT__cpu_we = 1U;
    Vtest_blitter___024root____VbeforeTrig_h603fc354__0(vlSelf, 
                                                        "@(posedge test_blitter.clk)");
    co_await vlSelfRef.__VtrigSched_h603fc354__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge test_blitter.clk)", 
                                                         "test_blitter.sv", 
                                                         115);
    vlSelfRef.test_blitter__DOT__cpu_we = 0U;
    __Vtask_test_blitter__DOT__blt_reg__620__data = 1U;
    __Vtask_test_blitter__DOT__blt_reg__620__off = 0U;
    __Vtask_test_blitter__DOT__write_reg__621__data 
        = __Vtask_test_blitter__DOT__blt_reg__620__data;
    __Vtask_test_blitter__DOT__write_reg__621__addr 
        = (0x0000ffffU & ((IData)(0xba83U) + __Vtask_test_blitter__DOT__blt_reg__620__off));
    Vtest_blitter___024root____VbeforeTrig_h603fc354__0(vlSelf, 
                                                        "@(posedge test_blitter.clk)");
    co_await vlSelfRef.__VtrigSched_h603fc354__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge test_blitter.clk)", 
                                                         "test_blitter.sv", 
                                                         113);
    vlSelfRef.test_blitter__DOT__cpu_addr = __Vtask_test_blitter__DOT__write_reg__621__addr;
    vlSelfRef.test_blitter__DOT__cpu_wdata = __Vtask_test_blitter__DOT__write_reg__621__data;
    vlSelfRef.test_blitter__DOT__cpu_we = 1U;
    Vtest_blitter___024root____VbeforeTrig_h603fc354__0(vlSelf, 
                                                        "@(posedge test_blitter.clk)");
    co_await vlSelfRef.__VtrigSched_h603fc354__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge test_blitter.clk)", 
                                                         "test_blitter.sv", 
                                                         115);
    vlSelfRef.test_blitter__DOT__cpu_we = 0U;
    __Vtask_test_blitter__DOT__wait_blt_done__622__test_blitter__DOT__unnamedblk1_1__DOT____Vrepeat0 = 0;
    __Vtask_test_blitter__DOT__wait_blt_done__622__test_blitter__DOT__unnamedblk1_2__DOT____Vrepeat1 = 0;
    __Vtask_test_blitter__DOT__wait_blt_done__622__test_blitter__DOT__unnamedblk1_1__DOT____Vrepeat0 = 2U;
    while (VL_LTS_III(32, 0U, __Vtask_test_blitter__DOT__wait_blt_done__622__test_blitter__DOT__unnamedblk1_1__DOT____Vrepeat0)) {
        Vtest_blitter___024root____VbeforeTrig_h603fc354__0(vlSelf, 
                                                            "@(posedge test_blitter.clk)");
        co_await vlSelfRef.__VtrigSched_h603fc354__0.trigger(0U, 
                                                             nullptr, 
                                                             "@(posedge test_blitter.clk)", 
                                                             "test_blitter.sv", 
                                                             128);
        __Vtask_test_blitter__DOT__wait_blt_done__622__test_blitter__DOT__unnamedblk1_1__DOT____Vrepeat0 
            = (__Vtask_test_blitter__DOT__wait_blt_done__622__test_blitter__DOT__unnamedblk1_1__DOT____Vrepeat0 
               - (IData)(1U));
    }
    while (((0U != (IData)(vlSelfRef.test_blitter__DOT__dut__DOT__state)) 
            & (6U != (IData)(vlSelfRef.test_blitter__DOT__dut__DOT__state)))) {
        Vtest_blitter___024root____VbeforeTrig_h603fc354__0(vlSelf, 
                                                            "@(posedge test_blitter.clk)");
        co_await vlSelfRef.__VtrigSched_h603fc354__0.trigger(0U, 
                                                             nullptr, 
                                                             "@(posedge test_blitter.clk)", 
                                                             "test_blitter.sv", 
                                                             130);
    }
    __Vtask_test_blitter__DOT__wait_blt_done__622__test_blitter__DOT__unnamedblk1_2__DOT____Vrepeat1 = 2U;
    while (VL_LTS_III(32, 0U, __Vtask_test_blitter__DOT__wait_blt_done__622__test_blitter__DOT__unnamedblk1_2__DOT____Vrepeat1)) {
        Vtest_blitter___024root____VbeforeTrig_h603fc354__0(vlSelf, 
                                                            "@(posedge test_blitter.clk)");
        co_await vlSelfRef.__VtrigSched_h603fc354__0.trigger(0U, 
                                                             nullptr, 
                                                             "@(posedge test_blitter.clk)", 
                                                             "test_blitter.sv", 
                                                             131);
        __Vtask_test_blitter__DOT__wait_blt_done__622__test_blitter__DOT__unnamedblk1_2__DOT____Vrepeat1 
            = (__Vtask_test_blitter__DOT__wait_blt_done__622__test_blitter__DOT__unnamedblk1_2__DOT____Vrepeat1 
               - (IData)(1U));
    }
    __Vtask_test_blitter__DOT__check__623__condition 
        = (2U == vlSelfRef.test_blitter__DOT__dut__DOT__regs[1U]);
    vlSelfRef.__Vtask_test_blitter__DOT__check__623__name = "below rom: status ok"s;
    vlSelfRef.test_blitter__DOT__test_num = ((IData)(1U) 
                                             + vlSelfRef.test_blitter__DOT__test_num);
    if (__Vtask_test_blitter__DOT__check__623__condition) {
        VL_WRITEF_NX("  PASS [%0d] %@\n",0,32,vlSelfRef.test_blitter__DOT__test_num,
                     -1,&(vlSelfRef.__Vtask_test_blitter__DOT__check__623__name));
        vlSelfRef.test_blitter__DOT__pass_count = ((IData)(1U) 
                                                   + vlSelfRef.test_blitter__DOT__pass_count);
    } else {
        VL_WRITEF_NX("  FAIL [%0d] %@\n",0,32,vlSelfRef.test_blitter__DOT__test_num,
                     -1,&(vlSelfRef.__Vtask_test_blitter__DOT__check__623__name));
        vlSelfRef.test_blitter__DOT__fail_count = ((IData)(1U) 
                                                   + vlSelfRef.test_blitter__DOT__fail_count);
    }
    __Vtask_test_blitter__DOT__check__624__condition 
        = (0x42U == vlSelfRef.test_blitter__DOT__sim_ram[49136U]);
    vlSelfRef.__Vtask_test_blitter__DOT__check__624__name = "below rom: ram[$BFF0] = 42"s;
    vlSelfRef.test_blitter__DOT__test_num = ((IData)(1U) 
                                             + vlSelfRef.test_blitter__DOT__test_num);
    if (__Vtask_test_blitter__DOT__check__624__condition) {
        VL_WRITEF_NX("  PASS [%0d] %@\n",0,32,vlSelfRef.test_blitter__DOT__test_num,
                     -1,&(vlSelfRef.__Vtask_test_blitter__DOT__check__624__name));
        vlSelfRef.test_blitter__DOT__pass_count = ((IData)(1U) 
                                                   + vlSelfRef.test_blitter__DOT__pass_count);
    } else {
        VL_WRITEF_NX("  FAIL [%0d] %@\n",0,32,vlSelfRef.test_blitter__DOT__test_num,
                     -1,&(vlSelfRef.__Vtask_test_blitter__DOT__check__624__name));
        vlSelfRef.test_blitter__DOT__fail_count = ((IData)(1U) 
                                                   + vlSelfRef.test_blitter__DOT__fail_count);
    }
    VL_WRITEF_NX("Test: Row buffer + color-key\n",0);
    vlSelfRef.test_blitter__DOT__sim_ram[40960U] = 1U;
    vlSelfRef.test_blitter__DOT__sim_ram[40961U] = 0U;
    vlSelfRef.test_blitter__DOT__sim_ram[40962U] = 2U;
    vlSelfRef.test_blitter__DOT__sim_ram[40963U] = 0U;
    vlSelfRef.test_blitter__DOT__sim_ram[41216U] = 0xffU;
    vlSelfRef.test_blitter__DOT__sim_ram[41217U] = 0xffU;
    vlSelfRef.test_blitter__DOT__sim_ram[41218U] = 0xffU;
    vlSelfRef.test_blitter__DOT__sim_ram[41219U] = 0xffU;
    __Vtask_test_blitter__DOT__setup_copy__625__height = 1U;
    __Vtask_test_blitter__DOT__setup_copy__625__width = 4U;
    __Vtask_test_blitter__DOT__setup_copy__625__dst_stride = 4U;
    __Vtask_test_blitter__DOT__setup_copy__625__dst_addr = 0x0000a100U;
    __Vtask_test_blitter__DOT__setup_copy__625__dst_space = 0U;
    __Vtask_test_blitter__DOT__setup_copy__625__src_stride = 4U;
    __Vtask_test_blitter__DOT__setup_copy__625__src_addr = 0x0000a000U;
    __Vtask_test_blitter__DOT__setup_copy__625__src_space = 0U;
    __Vtask_test_blitter__DOT__blt_reg__626__data = 
        (0x000000ffU & __Vtask_test_blitter__DOT__setup_copy__625__src_space);
    __Vtask_test_blitter__DOT__blt_reg__626__off = 3U;
    __Vtask_test_blitter__DOT__write_reg__627__data 
        = __Vtask_test_blitter__DOT__blt_reg__626__data;
    __Vtask_test_blitter__DOT__write_reg__627__addr 
        = (0x0000ffffU & ((IData)(0xba83U) + __Vtask_test_blitter__DOT__blt_reg__626__off));
    Vtest_blitter___024root____VbeforeTrig_h603fc354__0(vlSelf, 
                                                        "@(posedge test_blitter.clk)");
    co_await vlSelfRef.__VtrigSched_h603fc354__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge test_blitter.clk)", 
                                                         "test_blitter.sv", 
                                                         113);
    vlSelfRef.test_blitter__DOT__cpu_addr = __Vtask_test_blitter__DOT__write_reg__627__addr;
    vlSelfRef.test_blitter__DOT__cpu_wdata = __Vtask_test_blitter__DOT__write_reg__627__data;
    vlSelfRef.test_blitter__DOT__cpu_we = 1U;
    Vtest_blitter___024root____VbeforeTrig_h603fc354__0(vlSelf, 
                                                        "@(posedge test_blitter.clk)");
    co_await vlSelfRef.__VtrigSched_h603fc354__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge test_blitter.clk)", 
                                                         "test_blitter.sv", 
                                                         115);
    vlSelfRef.test_blitter__DOT__cpu_we = 0U;
    __Vtask_test_blitter__DOT__blt_reg__628__data = 
        (0x000000ffU & __Vtask_test_blitter__DOT__setup_copy__625__dst_space);
    __Vtask_test_blitter__DOT__blt_reg__628__off = 4U;
    __Vtask_test_blitter__DOT__write_reg__629__data 
        = __Vtask_test_blitter__DOT__blt_reg__628__data;
    __Vtask_test_blitter__DOT__write_reg__629__addr 
        = (0x0000ffffU & ((IData)(0xba83U) + __Vtask_test_blitter__DOT__blt_reg__628__off));
    Vtest_blitter___024root____VbeforeTrig_h603fc354__0(vlSelf, 
                                                        "@(posedge test_blitter.clk)");
    co_await vlSelfRef.__VtrigSched_h603fc354__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge test_blitter.clk)", 
                                                         "test_blitter.sv", 
                                                         113);
    vlSelfRef.test_blitter__DOT__cpu_addr = __Vtask_test_blitter__DOT__write_reg__629__addr;
    vlSelfRef.test_blitter__DOT__cpu_wdata = __Vtask_test_blitter__DOT__write_reg__629__data;
    vlSelfRef.test_blitter__DOT__cpu_we = 1U;
    Vtest_blitter___024root____VbeforeTrig_h603fc354__0(vlSelf, 
                                                        "@(posedge test_blitter.clk)");
    co_await vlSelfRef.__VtrigSched_h603fc354__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge test_blitter.clk)", 
                                                         "test_blitter.sv", 
                                                         115);
    vlSelfRef.test_blitter__DOT__cpu_we = 0U;
    __Vtask_test_blitter__DOT__blt_reg__630__data = 
        (0x000000ffU & __Vtask_test_blitter__DOT__setup_copy__625__src_addr);
    __Vtask_test_blitter__DOT__blt_reg__630__off = 5U;
    __Vtask_test_blitter__DOT__write_reg__631__data 
        = __Vtask_test_blitter__DOT__blt_reg__630__data;
    __Vtask_test_blitter__DOT__write_reg__631__addr 
        = (0x0000ffffU & ((IData)(0xba83U) + __Vtask_test_blitter__DOT__blt_reg__630__off));
    Vtest_blitter___024root____VbeforeTrig_h603fc354__0(vlSelf, 
                                                        "@(posedge test_blitter.clk)");
    co_await vlSelfRef.__VtrigSched_h603fc354__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge test_blitter.clk)", 
                                                         "test_blitter.sv", 
                                                         113);
    vlSelfRef.test_blitter__DOT__cpu_addr = __Vtask_test_blitter__DOT__write_reg__631__addr;
    vlSelfRef.test_blitter__DOT__cpu_wdata = __Vtask_test_blitter__DOT__write_reg__631__data;
    vlSelfRef.test_blitter__DOT__cpu_we = 1U;
    Vtest_blitter___024root____VbeforeTrig_h603fc354__0(vlSelf, 
                                                        "@(posedge test_blitter.clk)");
    co_await vlSelfRef.__VtrigSched_h603fc354__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge test_blitter.clk)", 
                                                         "test_blitter.sv", 
                                                         115);
    vlSelfRef.test_blitter__DOT__cpu_we = 0U;
    __Vtask_test_blitter__DOT__blt_reg__632__data = 
        (0x000000ffU & (__Vtask_test_blitter__DOT__setup_copy__625__src_addr 
                        >> 8U));
    __Vtask_test_blitter__DOT__blt_reg__632__off = 6U;
    __Vtask_test_blitter__DOT__write_reg__633__data 
        = __Vtask_test_blitter__DOT__blt_reg__632__data;
    __Vtask_test_blitter__DOT__write_reg__633__addr 
        = (0x0000ffffU & ((IData)(0xba83U) + __Vtask_test_blitter__DOT__blt_reg__632__off));
    Vtest_blitter___024root____VbeforeTrig_h603fc354__0(vlSelf, 
                                                        "@(posedge test_blitter.clk)");
    co_await vlSelfRef.__VtrigSched_h603fc354__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge test_blitter.clk)", 
                                                         "test_blitter.sv", 
                                                         113);
    vlSelfRef.test_blitter__DOT__cpu_addr = __Vtask_test_blitter__DOT__write_reg__633__addr;
    vlSelfRef.test_blitter__DOT__cpu_wdata = __Vtask_test_blitter__DOT__write_reg__633__data;
    vlSelfRef.test_blitter__DOT__cpu_we = 1U;
    Vtest_blitter___024root____VbeforeTrig_h603fc354__0(vlSelf, 
                                                        "@(posedge test_blitter.clk)");
    co_await vlSelfRef.__VtrigSched_h603fc354__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge test_blitter.clk)", 
                                                         "test_blitter.sv", 
                                                         115);
    vlSelfRef.test_blitter__DOT__cpu_we = 0U;
    __Vtask_test_blitter__DOT__blt_reg__634__data = 
        (0x000000ffU & (__Vtask_test_blitter__DOT__setup_copy__625__src_addr 
                        >> 0x10U));
    __Vtask_test_blitter__DOT__blt_reg__634__off = 7U;
    __Vtask_test_blitter__DOT__write_reg__635__data 
        = __Vtask_test_blitter__DOT__blt_reg__634__data;
    __Vtask_test_blitter__DOT__write_reg__635__addr 
        = (0x0000ffffU & ((IData)(0xba83U) + __Vtask_test_blitter__DOT__blt_reg__634__off));
    Vtest_blitter___024root____VbeforeTrig_h603fc354__0(vlSelf, 
                                                        "@(posedge test_blitter.clk)");
    co_await vlSelfRef.__VtrigSched_h603fc354__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge test_blitter.clk)", 
                                                         "test_blitter.sv", 
                                                         113);
    vlSelfRef.test_blitter__DOT__cpu_addr = __Vtask_test_blitter__DOT__write_reg__635__addr;
    vlSelfRef.test_blitter__DOT__cpu_wdata = __Vtask_test_blitter__DOT__write_reg__635__data;
    vlSelfRef.test_blitter__DOT__cpu_we = 1U;
    Vtest_blitter___024root____VbeforeTrig_h603fc354__0(vlSelf, 
                                                        "@(posedge test_blitter.clk)");
    co_await vlSelfRef.__VtrigSched_h603fc354__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge test_blitter.clk)", 
                                                         "test_blitter.sv", 
                                                         115);
    vlSelfRef.test_blitter__DOT__cpu_we = 0U;
    __Vtask_test_blitter__DOT__blt_reg__636__data = 
        (0x000000ffU & __Vtask_test_blitter__DOT__setup_copy__625__dst_addr);
    __Vtask_test_blitter__DOT__blt_reg__636__off = 8U;
    __Vtask_test_blitter__DOT__write_reg__637__data 
        = __Vtask_test_blitter__DOT__blt_reg__636__data;
    __Vtask_test_blitter__DOT__write_reg__637__addr 
        = (0x0000ffffU & ((IData)(0xba83U) + __Vtask_test_blitter__DOT__blt_reg__636__off));
    Vtest_blitter___024root____VbeforeTrig_h603fc354__0(vlSelf, 
                                                        "@(posedge test_blitter.clk)");
    co_await vlSelfRef.__VtrigSched_h603fc354__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge test_blitter.clk)", 
                                                         "test_blitter.sv", 
                                                         113);
    vlSelfRef.test_blitter__DOT__cpu_addr = __Vtask_test_blitter__DOT__write_reg__637__addr;
    vlSelfRef.test_blitter__DOT__cpu_wdata = __Vtask_test_blitter__DOT__write_reg__637__data;
    vlSelfRef.test_blitter__DOT__cpu_we = 1U;
    Vtest_blitter___024root____VbeforeTrig_h603fc354__0(vlSelf, 
                                                        "@(posedge test_blitter.clk)");
    co_await vlSelfRef.__VtrigSched_h603fc354__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge test_blitter.clk)", 
                                                         "test_blitter.sv", 
                                                         115);
    vlSelfRef.test_blitter__DOT__cpu_we = 0U;
    __Vtask_test_blitter__DOT__blt_reg__638__data = 
        (0x000000ffU & (__Vtask_test_blitter__DOT__setup_copy__625__dst_addr 
                        >> 8U));
    __Vtask_test_blitter__DOT__blt_reg__638__off = 9U;
    __Vtask_test_blitter__DOT__write_reg__639__data 
        = __Vtask_test_blitter__DOT__blt_reg__638__data;
    __Vtask_test_blitter__DOT__write_reg__639__addr 
        = (0x0000ffffU & ((IData)(0xba83U) + __Vtask_test_blitter__DOT__blt_reg__638__off));
    Vtest_blitter___024root____VbeforeTrig_h603fc354__0(vlSelf, 
                                                        "@(posedge test_blitter.clk)");
    co_await vlSelfRef.__VtrigSched_h603fc354__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge test_blitter.clk)", 
                                                         "test_blitter.sv", 
                                                         113);
    vlSelfRef.test_blitter__DOT__cpu_addr = __Vtask_test_blitter__DOT__write_reg__639__addr;
    vlSelfRef.test_blitter__DOT__cpu_wdata = __Vtask_test_blitter__DOT__write_reg__639__data;
    vlSelfRef.test_blitter__DOT__cpu_we = 1U;
    Vtest_blitter___024root____VbeforeTrig_h603fc354__0(vlSelf, 
                                                        "@(posedge test_blitter.clk)");
    co_await vlSelfRef.__VtrigSched_h603fc354__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge test_blitter.clk)", 
                                                         "test_blitter.sv", 
                                                         115);
    vlSelfRef.test_blitter__DOT__cpu_we = 0U;
    __Vtask_test_blitter__DOT__blt_reg__640__data = 
        (0x000000ffU & (__Vtask_test_blitter__DOT__setup_copy__625__dst_addr 
                        >> 0x10U));
    __Vtask_test_blitter__DOT__blt_reg__640__off = 0x0000000aU;
    __Vtask_test_blitter__DOT__write_reg__641__data 
        = __Vtask_test_blitter__DOT__blt_reg__640__data;
    __Vtask_test_blitter__DOT__write_reg__641__addr 
        = (0x0000ffffU & ((IData)(0xba83U) + __Vtask_test_blitter__DOT__blt_reg__640__off));
    Vtest_blitter___024root____VbeforeTrig_h603fc354__0(vlSelf, 
                                                        "@(posedge test_blitter.clk)");
    co_await vlSelfRef.__VtrigSched_h603fc354__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge test_blitter.clk)", 
                                                         "test_blitter.sv", 
                                                         113);
    vlSelfRef.test_blitter__DOT__cpu_addr = __Vtask_test_blitter__DOT__write_reg__641__addr;
    vlSelfRef.test_blitter__DOT__cpu_wdata = __Vtask_test_blitter__DOT__write_reg__641__data;
    vlSelfRef.test_blitter__DOT__cpu_we = 1U;
    Vtest_blitter___024root____VbeforeTrig_h603fc354__0(vlSelf, 
                                                        "@(posedge test_blitter.clk)");
    co_await vlSelfRef.__VtrigSched_h603fc354__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge test_blitter.clk)", 
                                                         "test_blitter.sv", 
                                                         115);
    vlSelfRef.test_blitter__DOT__cpu_we = 0U;
    __Vtask_test_blitter__DOT__blt_reg__642__data = 
        (0x000000ffU & __Vtask_test_blitter__DOT__setup_copy__625__width);
    __Vtask_test_blitter__DOT__blt_reg__642__off = 0x0000000bU;
    __Vtask_test_blitter__DOT__write_reg__643__data 
        = __Vtask_test_blitter__DOT__blt_reg__642__data;
    __Vtask_test_blitter__DOT__write_reg__643__addr 
        = (0x0000ffffU & ((IData)(0xba83U) + __Vtask_test_blitter__DOT__blt_reg__642__off));
    Vtest_blitter___024root____VbeforeTrig_h603fc354__0(vlSelf, 
                                                        "@(posedge test_blitter.clk)");
    co_await vlSelfRef.__VtrigSched_h603fc354__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge test_blitter.clk)", 
                                                         "test_blitter.sv", 
                                                         113);
    vlSelfRef.test_blitter__DOT__cpu_addr = __Vtask_test_blitter__DOT__write_reg__643__addr;
    vlSelfRef.test_blitter__DOT__cpu_wdata = __Vtask_test_blitter__DOT__write_reg__643__data;
    vlSelfRef.test_blitter__DOT__cpu_we = 1U;
    Vtest_blitter___024root____VbeforeTrig_h603fc354__0(vlSelf, 
                                                        "@(posedge test_blitter.clk)");
    co_await vlSelfRef.__VtrigSched_h603fc354__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge test_blitter.clk)", 
                                                         "test_blitter.sv", 
                                                         115);
    vlSelfRef.test_blitter__DOT__cpu_we = 0U;
    __Vtask_test_blitter__DOT__blt_reg__644__data = 
        (0x000000ffU & (__Vtask_test_blitter__DOT__setup_copy__625__width 
                        >> 8U));
    __Vtask_test_blitter__DOT__blt_reg__644__off = 0x0000000cU;
    __Vtask_test_blitter__DOT__write_reg__645__data 
        = __Vtask_test_blitter__DOT__blt_reg__644__data;
    __Vtask_test_blitter__DOT__write_reg__645__addr 
        = (0x0000ffffU & ((IData)(0xba83U) + __Vtask_test_blitter__DOT__blt_reg__644__off));
    Vtest_blitter___024root____VbeforeTrig_h603fc354__0(vlSelf, 
                                                        "@(posedge test_blitter.clk)");
    co_await vlSelfRef.__VtrigSched_h603fc354__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge test_blitter.clk)", 
                                                         "test_blitter.sv", 
                                                         113);
    vlSelfRef.test_blitter__DOT__cpu_addr = __Vtask_test_blitter__DOT__write_reg__645__addr;
    vlSelfRef.test_blitter__DOT__cpu_wdata = __Vtask_test_blitter__DOT__write_reg__645__data;
    vlSelfRef.test_blitter__DOT__cpu_we = 1U;
    Vtest_blitter___024root____VbeforeTrig_h603fc354__0(vlSelf, 
                                                        "@(posedge test_blitter.clk)");
    co_await vlSelfRef.__VtrigSched_h603fc354__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge test_blitter.clk)", 
                                                         "test_blitter.sv", 
                                                         115);
    vlSelfRef.test_blitter__DOT__cpu_we = 0U;
    __Vtask_test_blitter__DOT__blt_reg__646__data = 
        (0x000000ffU & __Vtask_test_blitter__DOT__setup_copy__625__height);
    __Vtask_test_blitter__DOT__blt_reg__646__off = 0x0000000dU;
    __Vtask_test_blitter__DOT__write_reg__647__data 
        = __Vtask_test_blitter__DOT__blt_reg__646__data;
    __Vtask_test_blitter__DOT__write_reg__647__addr 
        = (0x0000ffffU & ((IData)(0xba83U) + __Vtask_test_blitter__DOT__blt_reg__646__off));
    Vtest_blitter___024root____VbeforeTrig_h603fc354__0(vlSelf, 
                                                        "@(posedge test_blitter.clk)");
    co_await vlSelfRef.__VtrigSched_h603fc354__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge test_blitter.clk)", 
                                                         "test_blitter.sv", 
                                                         113);
    vlSelfRef.test_blitter__DOT__cpu_addr = __Vtask_test_blitter__DOT__write_reg__647__addr;
    vlSelfRef.test_blitter__DOT__cpu_wdata = __Vtask_test_blitter__DOT__write_reg__647__data;
    vlSelfRef.test_blitter__DOT__cpu_we = 1U;
    Vtest_blitter___024root____VbeforeTrig_h603fc354__0(vlSelf, 
                                                        "@(posedge test_blitter.clk)");
    co_await vlSelfRef.__VtrigSched_h603fc354__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge test_blitter.clk)", 
                                                         "test_blitter.sv", 
                                                         115);
    vlSelfRef.test_blitter__DOT__cpu_we = 0U;
    __Vtask_test_blitter__DOT__blt_reg__648__data = 
        (0x000000ffU & (__Vtask_test_blitter__DOT__setup_copy__625__height 
                        >> 8U));
    __Vtask_test_blitter__DOT__blt_reg__648__off = 0x0000000eU;
    __Vtask_test_blitter__DOT__write_reg__649__data 
        = __Vtask_test_blitter__DOT__blt_reg__648__data;
    __Vtask_test_blitter__DOT__write_reg__649__addr 
        = (0x0000ffffU & ((IData)(0xba83U) + __Vtask_test_blitter__DOT__blt_reg__648__off));
    Vtest_blitter___024root____VbeforeTrig_h603fc354__0(vlSelf, 
                                                        "@(posedge test_blitter.clk)");
    co_await vlSelfRef.__VtrigSched_h603fc354__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge test_blitter.clk)", 
                                                         "test_blitter.sv", 
                                                         113);
    vlSelfRef.test_blitter__DOT__cpu_addr = __Vtask_test_blitter__DOT__write_reg__649__addr;
    vlSelfRef.test_blitter__DOT__cpu_wdata = __Vtask_test_blitter__DOT__write_reg__649__data;
    vlSelfRef.test_blitter__DOT__cpu_we = 1U;
    Vtest_blitter___024root____VbeforeTrig_h603fc354__0(vlSelf, 
                                                        "@(posedge test_blitter.clk)");
    co_await vlSelfRef.__VtrigSched_h603fc354__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge test_blitter.clk)", 
                                                         "test_blitter.sv", 
                                                         115);
    vlSelfRef.test_blitter__DOT__cpu_we = 0U;
    __Vtask_test_blitter__DOT__blt_reg__650__data = 
        (0x000000ffU & __Vtask_test_blitter__DOT__setup_copy__625__src_stride);
    __Vtask_test_blitter__DOT__blt_reg__650__off = 0x0000000fU;
    __Vtask_test_blitter__DOT__write_reg__651__data 
        = __Vtask_test_blitter__DOT__blt_reg__650__data;
    __Vtask_test_blitter__DOT__write_reg__651__addr 
        = (0x0000ffffU & ((IData)(0xba83U) + __Vtask_test_blitter__DOT__blt_reg__650__off));
    Vtest_blitter___024root____VbeforeTrig_h603fc354__0(vlSelf, 
                                                        "@(posedge test_blitter.clk)");
    co_await vlSelfRef.__VtrigSched_h603fc354__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge test_blitter.clk)", 
                                                         "test_blitter.sv", 
                                                         113);
    vlSelfRef.test_blitter__DOT__cpu_addr = __Vtask_test_blitter__DOT__write_reg__651__addr;
    vlSelfRef.test_blitter__DOT__cpu_wdata = __Vtask_test_blitter__DOT__write_reg__651__data;
    vlSelfRef.test_blitter__DOT__cpu_we = 1U;
    Vtest_blitter___024root____VbeforeTrig_h603fc354__0(vlSelf, 
                                                        "@(posedge test_blitter.clk)");
    co_await vlSelfRef.__VtrigSched_h603fc354__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge test_blitter.clk)", 
                                                         "test_blitter.sv", 
                                                         115);
    vlSelfRef.test_blitter__DOT__cpu_we = 0U;
    __Vtask_test_blitter__DOT__blt_reg__652__data = 
        (0x000000ffU & (__Vtask_test_blitter__DOT__setup_copy__625__src_stride 
                        >> 8U));
    __Vtask_test_blitter__DOT__blt_reg__652__off = 0x00000010U;
    __Vtask_test_blitter__DOT__write_reg__653__data 
        = __Vtask_test_blitter__DOT__blt_reg__652__data;
    __Vtask_test_blitter__DOT__write_reg__653__addr 
        = (0x0000ffffU & ((IData)(0xba83U) + __Vtask_test_blitter__DOT__blt_reg__652__off));
    Vtest_blitter___024root____VbeforeTrig_h603fc354__0(vlSelf, 
                                                        "@(posedge test_blitter.clk)");
    co_await vlSelfRef.__VtrigSched_h603fc354__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge test_blitter.clk)", 
                                                         "test_blitter.sv", 
                                                         113);
    vlSelfRef.test_blitter__DOT__cpu_addr = __Vtask_test_blitter__DOT__write_reg__653__addr;
    vlSelfRef.test_blitter__DOT__cpu_wdata = __Vtask_test_blitter__DOT__write_reg__653__data;
    vlSelfRef.test_blitter__DOT__cpu_we = 1U;
    Vtest_blitter___024root____VbeforeTrig_h603fc354__0(vlSelf, 
                                                        "@(posedge test_blitter.clk)");
    co_await vlSelfRef.__VtrigSched_h603fc354__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge test_blitter.clk)", 
                                                         "test_blitter.sv", 
                                                         115);
    vlSelfRef.test_blitter__DOT__cpu_we = 0U;
    __Vtask_test_blitter__DOT__blt_reg__654__data = 
        (0x000000ffU & __Vtask_test_blitter__DOT__setup_copy__625__dst_stride);
    __Vtask_test_blitter__DOT__blt_reg__654__off = 0x00000011U;
    __Vtask_test_blitter__DOT__write_reg__655__data 
        = __Vtask_test_blitter__DOT__blt_reg__654__data;
    __Vtask_test_blitter__DOT__write_reg__655__addr 
        = (0x0000ffffU & ((IData)(0xba83U) + __Vtask_test_blitter__DOT__blt_reg__654__off));
    Vtest_blitter___024root____VbeforeTrig_h603fc354__0(vlSelf, 
                                                        "@(posedge test_blitter.clk)");
    co_await vlSelfRef.__VtrigSched_h603fc354__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge test_blitter.clk)", 
                                                         "test_blitter.sv", 
                                                         113);
    vlSelfRef.test_blitter__DOT__cpu_addr = __Vtask_test_blitter__DOT__write_reg__655__addr;
    vlSelfRef.test_blitter__DOT__cpu_wdata = __Vtask_test_blitter__DOT__write_reg__655__data;
    vlSelfRef.test_blitter__DOT__cpu_we = 1U;
    Vtest_blitter___024root____VbeforeTrig_h603fc354__0(vlSelf, 
                                                        "@(posedge test_blitter.clk)");
    co_await vlSelfRef.__VtrigSched_h603fc354__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge test_blitter.clk)", 
                                                         "test_blitter.sv", 
                                                         115);
    vlSelfRef.test_blitter__DOT__cpu_we = 0U;
    __Vtask_test_blitter__DOT__blt_reg__656__data = 
        (0x000000ffU & (__Vtask_test_blitter__DOT__setup_copy__625__dst_stride 
                        >> 8U));
    __Vtask_test_blitter__DOT__blt_reg__656__off = 0x00000012U;
    __Vtask_test_blitter__DOT__write_reg__657__data 
        = __Vtask_test_blitter__DOT__blt_reg__656__data;
    __Vtask_test_blitter__DOT__write_reg__657__addr 
        = (0x0000ffffU & ((IData)(0xba83U) + __Vtask_test_blitter__DOT__blt_reg__656__off));
    Vtest_blitter___024root____VbeforeTrig_h603fc354__0(vlSelf, 
                                                        "@(posedge test_blitter.clk)");
    co_await vlSelfRef.__VtrigSched_h603fc354__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge test_blitter.clk)", 
                                                         "test_blitter.sv", 
                                                         113);
    vlSelfRef.test_blitter__DOT__cpu_addr = __Vtask_test_blitter__DOT__write_reg__657__addr;
    vlSelfRef.test_blitter__DOT__cpu_wdata = __Vtask_test_blitter__DOT__write_reg__657__data;
    vlSelfRef.test_blitter__DOT__cpu_we = 1U;
    Vtest_blitter___024root____VbeforeTrig_h603fc354__0(vlSelf, 
                                                        "@(posedge test_blitter.clk)");
    co_await vlSelfRef.__VtrigSched_h603fc354__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge test_blitter.clk)", 
                                                         "test_blitter.sv", 
                                                         115);
    vlSelfRef.test_blitter__DOT__cpu_we = 0U;
    __Vtask_test_blitter__DOT__blt_reg__658__data = 0U;
    __Vtask_test_blitter__DOT__blt_reg__658__off = 0x00000013U;
    __Vtask_test_blitter__DOT__write_reg__659__data 
        = __Vtask_test_blitter__DOT__blt_reg__658__data;
    __Vtask_test_blitter__DOT__write_reg__659__addr 
        = (0x0000ffffU & ((IData)(0xba83U) + __Vtask_test_blitter__DOT__blt_reg__658__off));
    Vtest_blitter___024root____VbeforeTrig_h603fc354__0(vlSelf, 
                                                        "@(posedge test_blitter.clk)");
    co_await vlSelfRef.__VtrigSched_h603fc354__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge test_blitter.clk)", 
                                                         "test_blitter.sv", 
                                                         113);
    vlSelfRef.test_blitter__DOT__cpu_addr = __Vtask_test_blitter__DOT__write_reg__659__addr;
    vlSelfRef.test_blitter__DOT__cpu_wdata = __Vtask_test_blitter__DOT__write_reg__659__data;
    vlSelfRef.test_blitter__DOT__cpu_we = 1U;
    Vtest_blitter___024root____VbeforeTrig_h603fc354__0(vlSelf, 
                                                        "@(posedge test_blitter.clk)");
    co_await vlSelfRef.__VtrigSched_h603fc354__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge test_blitter.clk)", 
                                                         "test_blitter.sv", 
                                                         115);
    vlSelfRef.test_blitter__DOT__cpu_we = 0U;
    __Vtask_test_blitter__DOT__blt_reg__660__data = 2U;
    __Vtask_test_blitter__DOT__blt_reg__660__off = 0x00000013U;
    vlSelfRef.__Vtask_test_blitter__DOT__write_reg__661__data 
        = __Vtask_test_blitter__DOT__blt_reg__660__data;
    __Vtask_test_blitter__DOT__write_reg__661__addr 
        = (0x0000ffffU & ((IData)(0xba83U) + __Vtask_test_blitter__DOT__blt_reg__660__off));
    Vtest_blitter___024root____VbeforeTrig_h603fc354__0(vlSelf, 
                                                        "@(posedge test_blitter.clk)");
    co_await vlSelfRef.__VtrigSched_h603fc354__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge test_blitter.clk)", 
                                                         "test_blitter.sv", 
                                                         113);
    vlSelfRef.test_blitter__DOT__cpu_addr = __Vtask_test_blitter__DOT__write_reg__661__addr;
    co_return;
}
