// Verilated -*- C++ -*-
// DESCRIPTION: Verilator output: Design implementation internals
// See Vtest_blitter.h for the primary calling header

#include "Vtest_blitter__pch.h"

void Vtest_blitter___024root____VbeforeTrig_h603fc354__0(Vtest_blitter___024root* vlSelf, const char* __VeventDescription);

VlCoroutine Vtest_blitter___024root___eval_initial__TOP__Vtiming__0__1(Vtest_blitter___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vtest_blitter___024root___eval_initial__TOP__Vtiming__0__1\n"); );
    Vtest_blitter__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
    // Locals
    IData/*31:0*/ __Vtask_test_blitter__DOT__blt_reg__662__off;
    __Vtask_test_blitter__DOT__blt_reg__662__off = 0;
    CData/*7:0*/ __Vtask_test_blitter__DOT__blt_reg__662__data;
    __Vtask_test_blitter__DOT__blt_reg__662__data = 0;
    SData/*15:0*/ __Vtask_test_blitter__DOT__write_reg__663__addr;
    __Vtask_test_blitter__DOT__write_reg__663__addr = 0;
    CData/*7:0*/ __Vtask_test_blitter__DOT__write_reg__663__data;
    __Vtask_test_blitter__DOT__write_reg__663__data = 0;
    IData/*31:0*/ __Vtask_test_blitter__DOT__blt_reg__665__off;
    __Vtask_test_blitter__DOT__blt_reg__665__off = 0;
    CData/*7:0*/ __Vtask_test_blitter__DOT__blt_reg__665__data;
    __Vtask_test_blitter__DOT__blt_reg__665__data = 0;
    SData/*15:0*/ __Vtask_test_blitter__DOT__write_reg__666__addr;
    __Vtask_test_blitter__DOT__write_reg__666__addr = 0;
    CData/*7:0*/ __Vtask_test_blitter__DOT__write_reg__666__data;
    __Vtask_test_blitter__DOT__write_reg__666__data = 0;
    IData/*31:0*/ __Vtask_test_blitter__DOT__wait_blt_done__667__test_blitter__DOT__unnamedblk1_1__DOT____Vrepeat0;
    __Vtask_test_blitter__DOT__wait_blt_done__667__test_blitter__DOT__unnamedblk1_1__DOT____Vrepeat0 = 0;
    IData/*31:0*/ __Vtask_test_blitter__DOT__wait_blt_done__667__test_blitter__DOT__unnamedblk1_2__DOT____Vrepeat1;
    __Vtask_test_blitter__DOT__wait_blt_done__667__test_blitter__DOT__unnamedblk1_2__DOT____Vrepeat1 = 0;
    CData/*0:0*/ __Vtask_test_blitter__DOT__check__668__condition;
    __Vtask_test_blitter__DOT__check__668__condition = 0;
    CData/*0:0*/ __Vtask_test_blitter__DOT__check__669__condition;
    __Vtask_test_blitter__DOT__check__669__condition = 0;
    CData/*0:0*/ __Vtask_test_blitter__DOT__check__670__condition;
    __Vtask_test_blitter__DOT__check__670__condition = 0;
    CData/*0:0*/ __Vtask_test_blitter__DOT__check__671__condition;
    __Vtask_test_blitter__DOT__check__671__condition = 0;
    CData/*0:0*/ __Vtask_test_blitter__DOT__check__672__condition;
    __Vtask_test_blitter__DOT__check__672__condition = 0;
    // Body
    vlSelfRef.test_blitter__DOT__cpu_wdata = vlSelfRef.__Vtask_test_blitter__DOT__write_reg__661__data;
    vlSelfRef.test_blitter__DOT__cpu_we = 1U;
    Vtest_blitter___024root____VbeforeTrig_h603fc354__0(vlSelf, 
                                                        "@(posedge test_blitter.clk)");
    co_await vlSelfRef.__VtrigSched_h603fc354__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge test_blitter.clk)", 
                                                         "test_blitter.sv", 
                                                         115);
    vlSelfRef.test_blitter__DOT__cpu_we = 0U;
    __Vtask_test_blitter__DOT__blt_reg__662__data = 0U;
    __Vtask_test_blitter__DOT__blt_reg__662__off = 0x00000015U;
    __Vtask_test_blitter__DOT__write_reg__663__data 
        = __Vtask_test_blitter__DOT__blt_reg__662__data;
    __Vtask_test_blitter__DOT__write_reg__663__addr 
        = (0x0000ffffU & ((IData)(0xba83U) + __Vtask_test_blitter__DOT__blt_reg__662__off));
    Vtest_blitter___024root____VbeforeTrig_h603fc354__0(vlSelf, 
                                                        "@(posedge test_blitter.clk)");
    co_await vlSelfRef.__VtrigSched_h603fc354__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge test_blitter.clk)", 
                                                         "test_blitter.sv", 
                                                         113);
    vlSelfRef.test_blitter__DOT__cpu_addr = __Vtask_test_blitter__DOT__write_reg__663__addr;
    vlSelfRef.test_blitter__DOT__cpu_wdata = __Vtask_test_blitter__DOT__write_reg__663__data;
    vlSelfRef.test_blitter__DOT__cpu_we = 1U;
    Vtest_blitter___024root____VbeforeTrig_h603fc354__0(vlSelf, 
                                                        "@(posedge test_blitter.clk)");
    co_await vlSelfRef.__VtrigSched_h603fc354__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge test_blitter.clk)", 
                                                         "test_blitter.sv", 
                                                         115);
    vlSelfRef.test_blitter__DOT__cpu_we = 0U;
    __Vtask_test_blitter__DOT__blt_reg__665__data = 1U;
    __Vtask_test_blitter__DOT__blt_reg__665__off = 0U;
    __Vtask_test_blitter__DOT__write_reg__666__data 
        = __Vtask_test_blitter__DOT__blt_reg__665__data;
    __Vtask_test_blitter__DOT__write_reg__666__addr 
        = (0x0000ffffU & ((IData)(0xba83U) + __Vtask_test_blitter__DOT__blt_reg__665__off));
    Vtest_blitter___024root____VbeforeTrig_h603fc354__0(vlSelf, 
                                                        "@(posedge test_blitter.clk)");
    co_await vlSelfRef.__VtrigSched_h603fc354__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge test_blitter.clk)", 
                                                         "test_blitter.sv", 
                                                         113);
    vlSelfRef.test_blitter__DOT__cpu_addr = __Vtask_test_blitter__DOT__write_reg__666__addr;
    vlSelfRef.test_blitter__DOT__cpu_wdata = __Vtask_test_blitter__DOT__write_reg__666__data;
    vlSelfRef.test_blitter__DOT__cpu_we = 1U;
    Vtest_blitter___024root____VbeforeTrig_h603fc354__0(vlSelf, 
                                                        "@(posedge test_blitter.clk)");
    co_await vlSelfRef.__VtrigSched_h603fc354__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge test_blitter.clk)", 
                                                         "test_blitter.sv", 
                                                         115);
    vlSelfRef.test_blitter__DOT__cpu_we = 0U;
    __Vtask_test_blitter__DOT__wait_blt_done__667__test_blitter__DOT__unnamedblk1_1__DOT____Vrepeat0 = 0;
    __Vtask_test_blitter__DOT__wait_blt_done__667__test_blitter__DOT__unnamedblk1_2__DOT____Vrepeat1 = 0;
    __Vtask_test_blitter__DOT__wait_blt_done__667__test_blitter__DOT__unnamedblk1_1__DOT____Vrepeat0 = 2U;
    while (VL_LTS_III(32, 0U, __Vtask_test_blitter__DOT__wait_blt_done__667__test_blitter__DOT__unnamedblk1_1__DOT____Vrepeat0)) {
        Vtest_blitter___024root____VbeforeTrig_h603fc354__0(vlSelf, 
                                                            "@(posedge test_blitter.clk)");
        co_await vlSelfRef.__VtrigSched_h603fc354__0.trigger(0U, 
                                                             nullptr, 
                                                             "@(posedge test_blitter.clk)", 
                                                             "test_blitter.sv", 
                                                             128);
        __Vtask_test_blitter__DOT__wait_blt_done__667__test_blitter__DOT__unnamedblk1_1__DOT____Vrepeat0 
            = (__Vtask_test_blitter__DOT__wait_blt_done__667__test_blitter__DOT__unnamedblk1_1__DOT____Vrepeat0 
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
    __Vtask_test_blitter__DOT__wait_blt_done__667__test_blitter__DOT__unnamedblk1_2__DOT____Vrepeat1 = 2U;
    while (VL_LTS_III(32, 0U, __Vtask_test_blitter__DOT__wait_blt_done__667__test_blitter__DOT__unnamedblk1_2__DOT____Vrepeat1)) {
        Vtest_blitter___024root____VbeforeTrig_h603fc354__0(vlSelf, 
                                                            "@(posedge test_blitter.clk)");
        co_await vlSelfRef.__VtrigSched_h603fc354__0.trigger(0U, 
                                                             nullptr, 
                                                             "@(posedge test_blitter.clk)", 
                                                             "test_blitter.sv", 
                                                             131);
        __Vtask_test_blitter__DOT__wait_blt_done__667__test_blitter__DOT__unnamedblk1_2__DOT____Vrepeat1 
            = (__Vtask_test_blitter__DOT__wait_blt_done__667__test_blitter__DOT__unnamedblk1_2__DOT____Vrepeat1 
               - (IData)(1U));
    }
    __Vtask_test_blitter__DOT__check__668__condition 
        = (2U == vlSelfRef.test_blitter__DOT__dut__DOT__regs[1U]);
    vlSelfRef.__Vtask_test_blitter__DOT__check__668__name = "rowbuf+ck: status ok"s;
    vlSelfRef.test_blitter__DOT__test_num = ((IData)(1U) 
                                             + vlSelfRef.test_blitter__DOT__test_num);
    if (__Vtask_test_blitter__DOT__check__668__condition) {
        VL_WRITEF_NX("  PASS [%0d] %@\n",0,32,vlSelfRef.test_blitter__DOT__test_num,
                     -1,&(vlSelfRef.__Vtask_test_blitter__DOT__check__668__name));
        vlSelfRef.test_blitter__DOT__pass_count = ((IData)(1U) 
                                                   + vlSelfRef.test_blitter__DOT__pass_count);
    } else {
        VL_WRITEF_NX("  FAIL [%0d] %@\n",0,32,vlSelfRef.test_blitter__DOT__test_num,
                     -1,&(vlSelfRef.__Vtask_test_blitter__DOT__check__668__name));
        vlSelfRef.test_blitter__DOT__fail_count = ((IData)(1U) 
                                                   + vlSelfRef.test_blitter__DOT__fail_count);
    }
    __Vtask_test_blitter__DOT__check__669__condition 
        = (1U == vlSelfRef.test_blitter__DOT__sim_ram[41216U]);
    vlSelfRef.__Vtask_test_blitter__DOT__check__669__name = "rowbuf+ck: dst[0] = 01"s;
    vlSelfRef.test_blitter__DOT__test_num = ((IData)(1U) 
                                             + vlSelfRef.test_blitter__DOT__test_num);
    if (__Vtask_test_blitter__DOT__check__669__condition) {
        VL_WRITEF_NX("  PASS [%0d] %@\n",0,32,vlSelfRef.test_blitter__DOT__test_num,
                     -1,&(vlSelfRef.__Vtask_test_blitter__DOT__check__669__name));
        vlSelfRef.test_blitter__DOT__pass_count = ((IData)(1U) 
                                                   + vlSelfRef.test_blitter__DOT__pass_count);
    } else {
        VL_WRITEF_NX("  FAIL [%0d] %@\n",0,32,vlSelfRef.test_blitter__DOT__test_num,
                     -1,&(vlSelfRef.__Vtask_test_blitter__DOT__check__669__name));
        vlSelfRef.test_blitter__DOT__fail_count = ((IData)(1U) 
                                                   + vlSelfRef.test_blitter__DOT__fail_count);
    }
    __Vtask_test_blitter__DOT__check__670__condition 
        = (0xffU == vlSelfRef.test_blitter__DOT__sim_ram[41217U]);
    vlSelfRef.__Vtask_test_blitter__DOT__check__670__name = "rowbuf+ck: dst[1] = FF (skipped)"s;
    vlSelfRef.test_blitter__DOT__test_num = ((IData)(1U) 
                                             + vlSelfRef.test_blitter__DOT__test_num);
    if (__Vtask_test_blitter__DOT__check__670__condition) {
        VL_WRITEF_NX("  PASS [%0d] %@\n",0,32,vlSelfRef.test_blitter__DOT__test_num,
                     -1,&(vlSelfRef.__Vtask_test_blitter__DOT__check__670__name));
        vlSelfRef.test_blitter__DOT__pass_count = ((IData)(1U) 
                                                   + vlSelfRef.test_blitter__DOT__pass_count);
    } else {
        VL_WRITEF_NX("  FAIL [%0d] %@\n",0,32,vlSelfRef.test_blitter__DOT__test_num,
                     -1,&(vlSelfRef.__Vtask_test_blitter__DOT__check__670__name));
        vlSelfRef.test_blitter__DOT__fail_count = ((IData)(1U) 
                                                   + vlSelfRef.test_blitter__DOT__fail_count);
    }
    __Vtask_test_blitter__DOT__check__671__condition 
        = (2U == vlSelfRef.test_blitter__DOT__sim_ram[41218U]);
    vlSelfRef.__Vtask_test_blitter__DOT__check__671__name = "rowbuf+ck: dst[2] = 02"s;
    vlSelfRef.test_blitter__DOT__test_num = ((IData)(1U) 
                                             + vlSelfRef.test_blitter__DOT__test_num);
    if (__Vtask_test_blitter__DOT__check__671__condition) {
        VL_WRITEF_NX("  PASS [%0d] %@\n",0,32,vlSelfRef.test_blitter__DOT__test_num,
                     -1,&(vlSelfRef.__Vtask_test_blitter__DOT__check__671__name));
        vlSelfRef.test_blitter__DOT__pass_count = ((IData)(1U) 
                                                   + vlSelfRef.test_blitter__DOT__pass_count);
    } else {
        VL_WRITEF_NX("  FAIL [%0d] %@\n",0,32,vlSelfRef.test_blitter__DOT__test_num,
                     -1,&(vlSelfRef.__Vtask_test_blitter__DOT__check__671__name));
        vlSelfRef.test_blitter__DOT__fail_count = ((IData)(1U) 
                                                   + vlSelfRef.test_blitter__DOT__fail_count);
    }
    __Vtask_test_blitter__DOT__check__672__condition 
        = (0xffU == vlSelfRef.test_blitter__DOT__sim_ram[41219U]);
    vlSelfRef.__Vtask_test_blitter__DOT__check__672__name = "rowbuf+ck: dst[3] = FF (skipped)"s;
    vlSelfRef.test_blitter__DOT__test_num = ((IData)(1U) 
                                             + vlSelfRef.test_blitter__DOT__test_num);
    if (__Vtask_test_blitter__DOT__check__672__condition) {
        VL_WRITEF_NX("  PASS [%0d] %@\n",0,32,vlSelfRef.test_blitter__DOT__test_num,
                     -1,&(vlSelfRef.__Vtask_test_blitter__DOT__check__672__name));
        vlSelfRef.test_blitter__DOT__pass_count = ((IData)(1U) 
                                                   + vlSelfRef.test_blitter__DOT__pass_count);
    } else {
        VL_WRITEF_NX("  FAIL [%0d] %@\n",0,32,vlSelfRef.test_blitter__DOT__test_num,
                     -1,&(vlSelfRef.__Vtask_test_blitter__DOT__check__672__name));
        vlSelfRef.test_blitter__DOT__fail_count = ((IData)(1U) 
                                                   + vlSelfRef.test_blitter__DOT__fail_count);
    }
    VL_WRITEF_NX("\n=== Results: %0d passed, %0d failed ===\n",0,
                 32,vlSelfRef.test_blitter__DOT__pass_count,
                 32,vlSelfRef.test_blitter__DOT__fail_count);
    if ((0U == vlSelfRef.test_blitter__DOT__fail_count)) {
        VL_WRITEF_NX("ALL TESTS PASSED\n",0);
    } else {
        VL_WRITEF_NX("SOME TESTS FAILED\n",0);
    }
    VL_FINISH_MT("test_blitter.sv", 484, "");
    co_return;
}

VlCoroutine Vtest_blitter___024root___eval_initial__TOP__Vtiming__1(Vtest_blitter___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vtest_blitter___024root___eval_initial__TOP__Vtiming__1\n"); );
    Vtest_blitter__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
    // Body
    while (VL_LIKELY(!vlSymsp->_vm_contextp__->gotFinish())) {
        co_await vlSelfRef.__VdlySched.delay(0x0000000000004e20ULL, 
                                             nullptr, 
                                             "test_blitter.sv", 
                                             9);
        vlSelfRef.test_blitter__DOT__clk = (1U & (~ (IData)(vlSelfRef.test_blitter__DOT__clk)));
    }
    co_return;
}

void Vtest_blitter___024root___eval_triggers_vec__act(Vtest_blitter___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vtest_blitter___024root___eval_triggers_vec__act\n"); );
    Vtest_blitter__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
    // Body
    vlSelfRef.__VactTriggered[0U] = (QData)((IData)(
                                                    ((vlSelfRef.__VdlySched.awaitingCurrentTime() 
                                                      << 1U) 
                                                     | ((IData)(vlSelfRef.test_blitter__DOT__clk) 
                                                        & (~ (IData)(vlSelfRef.__Vtrigprevexpr___TOP__test_blitter__DOT__clk__0))))));
    vlSelfRef.__Vtrigprevexpr___TOP__test_blitter__DOT__clk__0 
        = vlSelfRef.test_blitter__DOT__clk;
}

bool Vtest_blitter___024root___trigger_anySet__act(const VlUnpacked<QData/*63:0*/, 1> &in) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vtest_blitter___024root___trigger_anySet__act\n"); );
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

void Vtest_blitter___024root___act_sequent__TOP__0(Vtest_blitter___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vtest_blitter___024root___act_sequent__TOP__0\n"); );
    Vtest_blitter__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
    // Body
    vlSelfRef.test_blitter__DOT__dut__DOT__blt_sel 
        = ((0xba83U <= (IData)(vlSelfRef.test_blitter__DOT__cpu_addr)) 
           & (0xba9bU >= (IData)(vlSelfRef.test_blitter__DOT__cpu_addr)));
    vlSelfRef.test_blitter__DOT__dut__DOT__mem_read_data 
        = ((0U == (IData)(vlSelfRef.test_blitter__DOT__dut__DOT__src_space))
            ? vlSelfRef.test_blitter__DOT__sim_ram[vlSelfRef.test_blitter__DOT__blt_ram_addr]
            : ((5U == (IData)(vlSelfRef.test_blitter__DOT__dut__DOT__src_space))
                ? vlSelfRef.test_blitter__DOT__sim_xram
               [vlSelfRef.test_blitter__DOT__blt_xram_addr]
                : ((4U & (IData)(vlSelfRef.test_blitter__DOT__blt_vgc_space))
                    ? ((2U & (IData)(vlSelfRef.test_blitter__DOT__blt_vgc_space))
                        ? 0U : ((1U & (IData)(vlSelfRef.test_blitter__DOT__blt_vgc_space))
                                 ? 0U : vlSelfRef.test_blitter__DOT__sim_sprite
                                [(0x00007fffU & (IData)(vlSelfRef.test_blitter__DOT__blt_vgc_addr))]))
                    : ((2U & (IData)(vlSelfRef.test_blitter__DOT__blt_vgc_space))
                        ? ((1U & (IData)(vlSelfRef.test_blitter__DOT__blt_vgc_space))
                            ? ((0xf9ffU >= (IData)(vlSelfRef.test_blitter__DOT__blt_vgc_addr))
                                ? vlSelfRef.test_blitter__DOT__sim_gfx
                               [vlSelfRef.test_blitter__DOT__blt_vgc_addr]
                                : 0U) : ((0x07cfU >= 
                                          (0x000007ffU 
                                           & (IData)(vlSelfRef.test_blitter__DOT__blt_vgc_addr)))
                                          ? vlSelfRef.test_blitter__DOT__sim_color
                                         [(0x000007ffU 
                                           & (IData)(vlSelfRef.test_blitter__DOT__blt_vgc_addr))]
                                          : 0U)) : 
                       ((1U & (IData)(vlSelfRef.test_blitter__DOT__blt_vgc_space))
                         ? ((0x07cfU >= (0x000007ffU 
                                         & (IData)(vlSelfRef.test_blitter__DOT__blt_vgc_addr)))
                             ? vlSelfRef.test_blitter__DOT__sim_char
                            [(0x000007ffU & (IData)(vlSelfRef.test_blitter__DOT__blt_vgc_addr))]
                             : 0U) : 0U)))));
}

void Vtest_blitter___024root___eval_act(Vtest_blitter___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vtest_blitter___024root___eval_act\n"); );
    Vtest_blitter__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
    // Body
    if ((1ULL & vlSelfRef.__VactTriggered[0U])) {
        vlSelfRef.test_blitter__DOT__dut__DOT__blt_sel 
            = ((0xba83U <= (IData)(vlSelfRef.test_blitter__DOT__cpu_addr)) 
               & (0xba9bU >= (IData)(vlSelfRef.test_blitter__DOT__cpu_addr)));
        vlSelfRef.test_blitter__DOT__dut__DOT__mem_read_data 
            = ((0U == (IData)(vlSelfRef.test_blitter__DOT__dut__DOT__src_space))
                ? vlSelfRef.test_blitter__DOT__sim_ram
               [vlSelfRef.test_blitter__DOT__blt_ram_addr]
                : ((5U == (IData)(vlSelfRef.test_blitter__DOT__dut__DOT__src_space))
                    ? vlSelfRef.test_blitter__DOT__sim_xram
                   [vlSelfRef.test_blitter__DOT__blt_xram_addr]
                    : ((4U & (IData)(vlSelfRef.test_blitter__DOT__blt_vgc_space))
                        ? ((2U & (IData)(vlSelfRef.test_blitter__DOT__blt_vgc_space))
                            ? 0U : ((1U & (IData)(vlSelfRef.test_blitter__DOT__blt_vgc_space))
                                     ? 0U : vlSelfRef.test_blitter__DOT__sim_sprite
                                    [(0x00007fffU & (IData)(vlSelfRef.test_blitter__DOT__blt_vgc_addr))]))
                        : ((2U & (IData)(vlSelfRef.test_blitter__DOT__blt_vgc_space))
                            ? ((1U & (IData)(vlSelfRef.test_blitter__DOT__blt_vgc_space))
                                ? ((0xf9ffU >= (IData)(vlSelfRef.test_blitter__DOT__blt_vgc_addr))
                                    ? vlSelfRef.test_blitter__DOT__sim_gfx
                                   [vlSelfRef.test_blitter__DOT__blt_vgc_addr]
                                    : 0U) : ((0x07cfU 
                                              >= (0x000007ffU 
                                                  & (IData)(vlSelfRef.test_blitter__DOT__blt_vgc_addr)))
                                              ? vlSelfRef.test_blitter__DOT__sim_color
                                             [(0x000007ffU 
                                               & (IData)(vlSelfRef.test_blitter__DOT__blt_vgc_addr))]
                                              : 0U))
                            : ((1U & (IData)(vlSelfRef.test_blitter__DOT__blt_vgc_space))
                                ? ((0x07cfU >= (0x000007ffU 
                                                & (IData)(vlSelfRef.test_blitter__DOT__blt_vgc_addr)))
                                    ? vlSelfRef.test_blitter__DOT__sim_char
                                   [(0x000007ffU & (IData)(vlSelfRef.test_blitter__DOT__blt_vgc_addr))]
                                    : 0U) : 0U)))));
    }
}

void Vtest_blitter___024root___nba_sequent__TOP__0(Vtest_blitter___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vtest_blitter___024root___nba_sequent__TOP__0\n"); );
    Vtest_blitter__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
    // Locals
    IData/*23:0*/ test_blitter__DOT__dut__DOT__src_addr;
    test_blitter__DOT__dut__DOT__src_addr = 0;
    IData/*23:0*/ test_blitter__DOT__dut__DOT__dst_addr;
    test_blitter__DOT__dut__DOT__dst_addr = 0;
    CData/*0:0*/ __Vfunc_test_blitter__DOT__dut__DOT__rect_fits__673__Vfuncout;
    __Vfunc_test_blitter__DOT__dut__DOT__rect_fits__673__Vfuncout = 0;
    IData/*23:0*/ __Vfunc_test_blitter__DOT__dut__DOT__rect_fits__673__base;
    __Vfunc_test_blitter__DOT__dut__DOT__rect_fits__673__base = 0;
    SData/*15:0*/ __Vfunc_test_blitter__DOT__dut__DOT__rect_fits__673__w;
    __Vfunc_test_blitter__DOT__dut__DOT__rect_fits__673__w = 0;
    SData/*15:0*/ __Vfunc_test_blitter__DOT__dut__DOT__rect_fits__673__h;
    __Vfunc_test_blitter__DOT__dut__DOT__rect_fits__673__h = 0;
    SData/*15:0*/ __Vfunc_test_blitter__DOT__dut__DOT__rect_fits__673__stride;
    __Vfunc_test_blitter__DOT__dut__DOT__rect_fits__673__stride = 0;
    IData/*19:0*/ __Vfunc_test_blitter__DOT__dut__DOT__rect_fits__673__sz;
    __Vfunc_test_blitter__DOT__dut__DOT__rect_fits__673__sz = 0;
    IData/*31:0*/ __Vfunc_test_blitter__DOT__dut__DOT__rect_fits__673__last_row_start;
    __Vfunc_test_blitter__DOT__dut__DOT__rect_fits__673__last_row_start = 0;
    IData/*31:0*/ __Vfunc_test_blitter__DOT__dut__DOT__rect_fits__673__last_byte;
    __Vfunc_test_blitter__DOT__dut__DOT__rect_fits__673__last_byte = 0;
    IData/*19:0*/ __Vfunc_test_blitter__DOT__dut__DOT__space_size__674__Vfuncout;
    __Vfunc_test_blitter__DOT__dut__DOT__space_size__674__Vfuncout = 0;
    CData/*2:0*/ __Vfunc_test_blitter__DOT__dut__DOT__space_size__674__sp;
    __Vfunc_test_blitter__DOT__dut__DOT__space_size__674__sp = 0;
    CData/*0:0*/ __Vfunc_test_blitter__DOT__dut__DOT__rect_fits__675__Vfuncout;
    __Vfunc_test_blitter__DOT__dut__DOT__rect_fits__675__Vfuncout = 0;
    IData/*23:0*/ __Vfunc_test_blitter__DOT__dut__DOT__rect_fits__675__base;
    __Vfunc_test_blitter__DOT__dut__DOT__rect_fits__675__base = 0;
    SData/*15:0*/ __Vfunc_test_blitter__DOT__dut__DOT__rect_fits__675__w;
    __Vfunc_test_blitter__DOT__dut__DOT__rect_fits__675__w = 0;
    SData/*15:0*/ __Vfunc_test_blitter__DOT__dut__DOT__rect_fits__675__h;
    __Vfunc_test_blitter__DOT__dut__DOT__rect_fits__675__h = 0;
    SData/*15:0*/ __Vfunc_test_blitter__DOT__dut__DOT__rect_fits__675__stride;
    __Vfunc_test_blitter__DOT__dut__DOT__rect_fits__675__stride = 0;
    IData/*19:0*/ __Vfunc_test_blitter__DOT__dut__DOT__rect_fits__675__sz;
    __Vfunc_test_blitter__DOT__dut__DOT__rect_fits__675__sz = 0;
    IData/*31:0*/ __Vfunc_test_blitter__DOT__dut__DOT__rect_fits__675__last_row_start;
    __Vfunc_test_blitter__DOT__dut__DOT__rect_fits__675__last_row_start = 0;
    IData/*31:0*/ __Vfunc_test_blitter__DOT__dut__DOT__rect_fits__675__last_byte;
    __Vfunc_test_blitter__DOT__dut__DOT__rect_fits__675__last_byte = 0;
    IData/*19:0*/ __Vfunc_test_blitter__DOT__dut__DOT__space_size__676__Vfuncout;
    __Vfunc_test_blitter__DOT__dut__DOT__space_size__676__Vfuncout = 0;
    CData/*2:0*/ __Vfunc_test_blitter__DOT__dut__DOT__space_size__676__sp;
    __Vfunc_test_blitter__DOT__dut__DOT__space_size__676__sp = 0;
    CData/*0:0*/ __Vfunc_test_blitter__DOT__dut__DOT__write_protect_ok__677__Vfuncout;
    __Vfunc_test_blitter__DOT__dut__DOT__write_protect_ok__677__Vfuncout = 0;
    CData/*2:0*/ __Vfunc_test_blitter__DOT__dut__DOT__write_protect_ok__677__space;
    __Vfunc_test_blitter__DOT__dut__DOT__write_protect_ok__677__space = 0;
    IData/*23:0*/ __Vfunc_test_blitter__DOT__dut__DOT__write_protect_ok__677__base;
    __Vfunc_test_blitter__DOT__dut__DOT__write_protect_ok__677__base = 0;
    SData/*15:0*/ __Vfunc_test_blitter__DOT__dut__DOT__write_protect_ok__677__w;
    __Vfunc_test_blitter__DOT__dut__DOT__write_protect_ok__677__w = 0;
    SData/*15:0*/ __Vfunc_test_blitter__DOT__dut__DOT__write_protect_ok__677__h;
    __Vfunc_test_blitter__DOT__dut__DOT__write_protect_ok__677__h = 0;
    SData/*15:0*/ __Vfunc_test_blitter__DOT__dut__DOT__write_protect_ok__677__stride;
    __Vfunc_test_blitter__DOT__dut__DOT__write_protect_ok__677__stride = 0;
    IData/*31:0*/ __Vfunc_test_blitter__DOT__dut__DOT__write_protect_ok__677__last_byte;
    __Vfunc_test_blitter__DOT__dut__DOT__write_protect_ok__677__last_byte = 0;
    CData/*3:0*/ __Vdly__test_blitter__DOT__dut__DOT__state;
    __Vdly__test_blitter__DOT__dut__DOT__state = 0;
    CData/*0:0*/ __Vdly__test_blitter__DOT__dut__DOT__read_valid;
    __Vdly__test_blitter__DOT__dut__DOT__read_valid = 0;
    IData/*23:0*/ __Vdly__test_blitter__DOT__dut__DOT__wrote_count;
    __Vdly__test_blitter__DOT__dut__DOT__wrote_count = 0;
    SData/*15:0*/ __Vdly__test_blitter__DOT__dut__DOT__col;
    __Vdly__test_blitter__DOT__dut__DOT__col = 0;
    SData/*15:0*/ __Vdly__test_blitter__DOT__dut__DOT__row;
    __Vdly__test_blitter__DOT__dut__DOT__row = 0;
    SData/*9:0*/ __Vdly__test_blitter__DOT__dut__DOT__buf_idx;
    __Vdly__test_blitter__DOT__dut__DOT__buf_idx = 0;
    CData/*3:0*/ __VdlyVal__test_blitter__DOT__sim_gfx__v0;
    __VdlyVal__test_blitter__DOT__sim_gfx__v0 = 0;
    SData/*15:0*/ __VdlyDim0__test_blitter__DOT__sim_gfx__v0;
    __VdlyDim0__test_blitter__DOT__sim_gfx__v0 = 0;
    CData/*0:0*/ __VdlySet__test_blitter__DOT__sim_gfx__v0;
    __VdlySet__test_blitter__DOT__sim_gfx__v0 = 0;
    CData/*7:0*/ __VdlyVal__test_blitter__DOT__sim_sprite__v0;
    __VdlyVal__test_blitter__DOT__sim_sprite__v0 = 0;
    SData/*14:0*/ __VdlyDim0__test_blitter__DOT__sim_sprite__v0;
    __VdlyDim0__test_blitter__DOT__sim_sprite__v0 = 0;
    CData/*0:0*/ __VdlySet__test_blitter__DOT__sim_sprite__v0;
    __VdlySet__test_blitter__DOT__sim_sprite__v0 = 0;
    CData/*7:0*/ __VdlyVal__test_blitter__DOT__sim_color__v0;
    __VdlyVal__test_blitter__DOT__sim_color__v0 = 0;
    SData/*10:0*/ __VdlyDim0__test_blitter__DOT__sim_color__v0;
    __VdlyDim0__test_blitter__DOT__sim_color__v0 = 0;
    CData/*0:0*/ __VdlySet__test_blitter__DOT__sim_color__v0;
    __VdlySet__test_blitter__DOT__sim_color__v0 = 0;
    CData/*7:0*/ __VdlyVal__test_blitter__DOT__sim_char__v0;
    __VdlyVal__test_blitter__DOT__sim_char__v0 = 0;
    SData/*10:0*/ __VdlyDim0__test_blitter__DOT__sim_char__v0;
    __VdlyDim0__test_blitter__DOT__sim_char__v0 = 0;
    CData/*0:0*/ __VdlySet__test_blitter__DOT__sim_char__v0;
    __VdlySet__test_blitter__DOT__sim_char__v0 = 0;
    CData/*7:0*/ __VdlyVal__test_blitter__DOT__sim_ram__v0;
    __VdlyVal__test_blitter__DOT__sim_ram__v0 = 0;
    SData/*15:0*/ __VdlyDim0__test_blitter__DOT__sim_ram__v0;
    __VdlyDim0__test_blitter__DOT__sim_ram__v0 = 0;
    CData/*0:0*/ __VdlySet__test_blitter__DOT__sim_ram__v0;
    __VdlySet__test_blitter__DOT__sim_ram__v0 = 0;
    CData/*7:0*/ __VdlyVal__test_blitter__DOT__sim_xram__v0;
    __VdlyVal__test_blitter__DOT__sim_xram__v0 = 0;
    IData/*18:0*/ __VdlyDim0__test_blitter__DOT__sim_xram__v0;
    __VdlyDim0__test_blitter__DOT__sim_xram__v0 = 0;
    CData/*0:0*/ __VdlySet__test_blitter__DOT__sim_xram__v0;
    __VdlySet__test_blitter__DOT__sim_xram__v0 = 0;
    CData/*0:0*/ __VdlySet__test_blitter__DOT__dut__DOT__regs__v0;
    __VdlySet__test_blitter__DOT__dut__DOT__regs__v0 = 0;
    CData/*0:0*/ __VdlySet__test_blitter__DOT__dut__DOT__regs__v1;
    __VdlySet__test_blitter__DOT__dut__DOT__regs__v1 = 0;
    CData/*0:0*/ __VdlySet__test_blitter__DOT__dut__DOT__regs__v2;
    __VdlySet__test_blitter__DOT__dut__DOT__regs__v2 = 0;
    CData/*0:0*/ __VdlySet__test_blitter__DOT__dut__DOT__regs__v3;
    __VdlySet__test_blitter__DOT__dut__DOT__regs__v3 = 0;
    CData/*7:0*/ __VdlyVal__test_blitter__DOT__dut__DOT__regs__v4;
    __VdlyVal__test_blitter__DOT__dut__DOT__regs__v4 = 0;
    CData/*7:0*/ __VdlyVal__test_blitter__DOT__dut__DOT__regs__v5;
    __VdlyVal__test_blitter__DOT__dut__DOT__regs__v5 = 0;
    CData/*7:0*/ __VdlyVal__test_blitter__DOT__dut__DOT__regs__v6;
    __VdlyVal__test_blitter__DOT__dut__DOT__regs__v6 = 0;
    CData/*7:0*/ __VdlyVal__test_blitter__DOT__dut__DOT__row_buf__v0;
    __VdlyVal__test_blitter__DOT__dut__DOT__row_buf__v0 = 0;
    SData/*9:0*/ __VdlyDim0__test_blitter__DOT__dut__DOT__row_buf__v0;
    __VdlyDim0__test_blitter__DOT__dut__DOT__row_buf__v0 = 0;
    CData/*0:0*/ __VdlySet__test_blitter__DOT__dut__DOT__row_buf__v0;
    __VdlySet__test_blitter__DOT__dut__DOT__row_buf__v0 = 0;
    CData/*0:0*/ __VdlySet__test_blitter__DOT__dut__DOT__regs__v7;
    __VdlySet__test_blitter__DOT__dut__DOT__regs__v7 = 0;
    CData/*0:0*/ __VdlySet__test_blitter__DOT__dut__DOT__regs__v8;
    __VdlySet__test_blitter__DOT__dut__DOT__regs__v8 = 0;
    CData/*7:0*/ __VdlyVal__test_blitter__DOT__dut__DOT__regs__v9;
    __VdlyVal__test_blitter__DOT__dut__DOT__regs__v9 = 0;
    CData/*7:0*/ __VdlyVal__test_blitter__DOT__dut__DOT__regs__v10;
    __VdlyVal__test_blitter__DOT__dut__DOT__regs__v10 = 0;
    CData/*7:0*/ __VdlyVal__test_blitter__DOT__dut__DOT__regs__v11;
    __VdlyVal__test_blitter__DOT__dut__DOT__regs__v11 = 0;
    CData/*0:0*/ __VdlySet__test_blitter__DOT__dut__DOT__regs__v12;
    __VdlySet__test_blitter__DOT__dut__DOT__regs__v12 = 0;
    CData/*0:0*/ __VdlySet__test_blitter__DOT__dut__DOT__regs__v13;
    __VdlySet__test_blitter__DOT__dut__DOT__regs__v13 = 0;
    CData/*0:0*/ __VdlySet__test_blitter__DOT__dut__DOT__regs__v14;
    __VdlySet__test_blitter__DOT__dut__DOT__regs__v14 = 0;
    CData/*0:0*/ __VdlySet__test_blitter__DOT__dut__DOT__regs__v15;
    __VdlySet__test_blitter__DOT__dut__DOT__regs__v15 = 0;
    CData/*0:0*/ __VdlySet__test_blitter__DOT__dut__DOT__regs__v16;
    __VdlySet__test_blitter__DOT__dut__DOT__regs__v16 = 0;
    CData/*0:0*/ __VdlySet__test_blitter__DOT__dut__DOT__regs__v17;
    __VdlySet__test_blitter__DOT__dut__DOT__regs__v17 = 0;
    CData/*0:0*/ __VdlySet__test_blitter__DOT__dut__DOT__regs__v18;
    __VdlySet__test_blitter__DOT__dut__DOT__regs__v18 = 0;
    CData/*0:0*/ __VdlySet__test_blitter__DOT__dut__DOT__regs__v20;
    __VdlySet__test_blitter__DOT__dut__DOT__regs__v20 = 0;
    CData/*0:0*/ __VdlySet__test_blitter__DOT__dut__DOT__regs__v22;
    __VdlySet__test_blitter__DOT__dut__DOT__regs__v22 = 0;
    CData/*7:0*/ __VdlyVal__test_blitter__DOT__dut__DOT__regs__v24;
    __VdlyVal__test_blitter__DOT__dut__DOT__regs__v24 = 0;
    CData/*4:0*/ __VdlyDim0__test_blitter__DOT__dut__DOT__regs__v24;
    __VdlyDim0__test_blitter__DOT__dut__DOT__regs__v24 = 0;
    CData/*0:0*/ __VdlySet__test_blitter__DOT__dut__DOT__regs__v24;
    __VdlySet__test_blitter__DOT__dut__DOT__regs__v24 = 0;
    CData/*0:0*/ __VdlySet__test_blitter__DOT__dut__DOT__regs__v25;
    __VdlySet__test_blitter__DOT__dut__DOT__regs__v25 = 0;
    CData/*0:0*/ __VdlySet__test_blitter__DOT__dut__DOT__regs__v30;
    __VdlySet__test_blitter__DOT__dut__DOT__regs__v30 = 0;
    CData/*0:0*/ __VdlySet__test_blitter__DOT__dut__DOT__regs__v32;
    __VdlySet__test_blitter__DOT__dut__DOT__regs__v32 = 0;
    // Body
    __VdlySet__test_blitter__DOT__sim_ram__v0 = 0U;
    __VdlySet__test_blitter__DOT__sim_xram__v0 = 0U;
    __VdlySet__test_blitter__DOT__sim_sprite__v0 = 0U;
    __VdlySet__test_blitter__DOT__sim_gfx__v0 = 0U;
    __VdlySet__test_blitter__DOT__sim_color__v0 = 0U;
    __VdlySet__test_blitter__DOT__sim_char__v0 = 0U;
    __Vdly__test_blitter__DOT__dut__DOT__wrote_count 
        = vlSelfRef.test_blitter__DOT__dut__DOT__wrote_count;
    __VdlySet__test_blitter__DOT__dut__DOT__regs__v0 = 0U;
    __VdlySet__test_blitter__DOT__dut__DOT__regs__v1 = 0U;
    __VdlySet__test_blitter__DOT__dut__DOT__regs__v2 = 0U;
    __VdlySet__test_blitter__DOT__dut__DOT__regs__v3 = 0U;
    __VdlySet__test_blitter__DOT__dut__DOT__regs__v7 = 0U;
    __VdlySet__test_blitter__DOT__dut__DOT__regs__v8 = 0U;
    __VdlySet__test_blitter__DOT__dut__DOT__regs__v12 = 0U;
    __VdlySet__test_blitter__DOT__dut__DOT__regs__v13 = 0U;
    __VdlySet__test_blitter__DOT__dut__DOT__regs__v14 = 0U;
    __VdlySet__test_blitter__DOT__dut__DOT__regs__v15 = 0U;
    __VdlySet__test_blitter__DOT__dut__DOT__regs__v16 = 0U;
    __VdlySet__test_blitter__DOT__dut__DOT__regs__v17 = 0U;
    __VdlySet__test_blitter__DOT__dut__DOT__regs__v18 = 0U;
    __VdlySet__test_blitter__DOT__dut__DOT__regs__v20 = 0U;
    __VdlySet__test_blitter__DOT__dut__DOT__regs__v22 = 0U;
    __VdlySet__test_blitter__DOT__dut__DOT__regs__v24 = 0U;
    __VdlySet__test_blitter__DOT__dut__DOT__regs__v25 = 0U;
    __VdlySet__test_blitter__DOT__dut__DOT__regs__v30 = 0U;
    __VdlySet__test_blitter__DOT__dut__DOT__regs__v32 = 0U;
    __Vdly__test_blitter__DOT__dut__DOT__state = vlSelfRef.test_blitter__DOT__dut__DOT__state;
    __Vdly__test_blitter__DOT__dut__DOT__read_valid 
        = vlSelfRef.test_blitter__DOT__dut__DOT__read_valid;
    __Vdly__test_blitter__DOT__dut__DOT__buf_idx = vlSelfRef.test_blitter__DOT__dut__DOT__buf_idx;
    __VdlySet__test_blitter__DOT__dut__DOT__row_buf__v0 = 0U;
    __Vdly__test_blitter__DOT__dut__DOT__col = vlSelfRef.test_blitter__DOT__dut__DOT__col;
    __Vdly__test_blitter__DOT__dut__DOT__row = vlSelfRef.test_blitter__DOT__dut__DOT__row;
    if (vlSelfRef.test_blitter__DOT__blt_ram_we) {
        __VdlyVal__test_blitter__DOT__sim_ram__v0 = vlSelfRef.test_blitter__DOT__blt_ram_wdata;
        __VdlyDim0__test_blitter__DOT__sim_ram__v0 
            = vlSelfRef.test_blitter__DOT__blt_ram_addr;
        __VdlySet__test_blitter__DOT__sim_ram__v0 = 1U;
    }
    if (vlSelfRef.test_blitter__DOT__blt_xram_we) {
        __VdlyVal__test_blitter__DOT__sim_xram__v0 
            = vlSelfRef.test_blitter__DOT__blt_xram_wdata;
        __VdlyDim0__test_blitter__DOT__sim_xram__v0 
            = vlSelfRef.test_blitter__DOT__blt_xram_addr;
        __VdlySet__test_blitter__DOT__sim_xram__v0 = 1U;
    }
    if (vlSelfRef.test_blitter__DOT__blt_vgc_we) {
        if ((4U & (IData)(vlSelfRef.test_blitter__DOT__blt_vgc_space))) {
            if ((1U & (~ ((IData)(vlSelfRef.test_blitter__DOT__blt_vgc_space) 
                          >> 1U)))) {
                if ((1U & (~ (IData)(vlSelfRef.test_blitter__DOT__blt_vgc_space)))) {
                    __VdlyVal__test_blitter__DOT__sim_sprite__v0 
                        = vlSelfRef.test_blitter__DOT__blt_vgc_wdata;
                    __VdlyDim0__test_blitter__DOT__sim_sprite__v0 
                        = (0x00007fffU & (IData)(vlSelfRef.test_blitter__DOT__blt_vgc_addr));
                    __VdlySet__test_blitter__DOT__sim_sprite__v0 = 1U;
                }
            }
        }
        if ((1U & (~ ((IData)(vlSelfRef.test_blitter__DOT__blt_vgc_space) 
                      >> 2U)))) {
            if ((2U & (IData)(vlSelfRef.test_blitter__DOT__blt_vgc_space))) {
                if ((1U & (IData)(vlSelfRef.test_blitter__DOT__blt_vgc_space))) {
                    vlSelfRef.test_blitter__DOT____Vlvbound_ha699a557__0 
                        = (0x0000000fU & (IData)(vlSelfRef.test_blitter__DOT__blt_vgc_wdata));
                    if ((0xf9ffU >= (IData)(vlSelfRef.test_blitter__DOT__blt_vgc_addr))) {
                        __VdlyVal__test_blitter__DOT__sim_gfx__v0 
                            = vlSelfRef.test_blitter__DOT____Vlvbound_ha699a557__0;
                        __VdlyDim0__test_blitter__DOT__sim_gfx__v0 
                            = vlSelfRef.test_blitter__DOT__blt_vgc_addr;
                        __VdlySet__test_blitter__DOT__sim_gfx__v0 = 1U;
                    }
                }
                if ((1U & (~ (IData)(vlSelfRef.test_blitter__DOT__blt_vgc_space)))) {
                    vlSelfRef.test_blitter__DOT____Vlvbound_h530fe462__0 
                        = vlSelfRef.test_blitter__DOT__blt_vgc_wdata;
                    if ((0x07cfU >= (0x000007ffU & (IData)(vlSelfRef.test_blitter__DOT__blt_vgc_addr)))) {
                        __VdlyVal__test_blitter__DOT__sim_color__v0 
                            = vlSelfRef.test_blitter__DOT____Vlvbound_h530fe462__0;
                        __VdlyDim0__test_blitter__DOT__sim_color__v0 
                            = (0x000007ffU & (IData)(vlSelfRef.test_blitter__DOT__blt_vgc_addr));
                        __VdlySet__test_blitter__DOT__sim_color__v0 = 1U;
                    }
                }
            }
            if ((1U & (~ ((IData)(vlSelfRef.test_blitter__DOT__blt_vgc_space) 
                          >> 1U)))) {
                if ((1U & (IData)(vlSelfRef.test_blitter__DOT__blt_vgc_space))) {
                    vlSelfRef.test_blitter__DOT____Vlvbound_h6c644df4__0 
                        = vlSelfRef.test_blitter__DOT__blt_vgc_wdata;
                    if ((0x07cfU >= (0x000007ffU & (IData)(vlSelfRef.test_blitter__DOT__blt_vgc_addr)))) {
                        __VdlyVal__test_blitter__DOT__sim_char__v0 
                            = vlSelfRef.test_blitter__DOT____Vlvbound_h6c644df4__0;
                        __VdlyDim0__test_blitter__DOT__sim_char__v0 
                            = (0x000007ffU & (IData)(vlSelfRef.test_blitter__DOT__blt_vgc_addr));
                        __VdlySet__test_blitter__DOT__sim_char__v0 = 1U;
                    }
                }
            }
        }
    }
    if (vlSelfRef.test_blitter__DOT__rst) {
        __Vdly__test_blitter__DOT__dut__DOT__state = 0U;
        __VdlySet__test_blitter__DOT__dut__DOT__regs__v0 = 1U;
        __Vdly__test_blitter__DOT__dut__DOT__read_valid = 0U;
        __VdlySet__test_blitter__DOT__dut__DOT__regs__v1 = 1U;
    } else {
        if ((8U & (IData)(vlSelfRef.test_blitter__DOT__dut__DOT__state))) {
            __Vdly__test_blitter__DOT__dut__DOT__state = 0U;
        } else if ((4U & (IData)(vlSelfRef.test_blitter__DOT__dut__DOT__state))) {
            if ((2U & (IData)(vlSelfRef.test_blitter__DOT__dut__DOT__state))) {
                __Vdly__test_blitter__DOT__dut__DOT__state = 0U;
            } else if ((1U & (IData)(vlSelfRef.test_blitter__DOT__dut__DOT__state))) {
                if ((1U & (~ ((IData)(vlSelfRef.test_blitter__DOT__dut__DOT__colorkey_mode) 
                              & (vlSelfRef.test_blitter__DOT__dut__DOT__row_buf
                                 [vlSelfRef.test_blitter__DOT__dut__DOT__buf_idx] 
                                 == (IData)(vlSelfRef.test_blitter__DOT__dut__DOT__color_key)))))) {
                    __Vdly__test_blitter__DOT__dut__DOT__wrote_count 
                        = (0x00ffffffU & ((IData)(1U) 
                                          + vlSelfRef.test_blitter__DOT__dut__DOT__wrote_count));
                }
                if ((((IData)(1U) + (IData)(vlSelfRef.test_blitter__DOT__dut__DOT__col)) 
                     >= (IData)(vlSelfRef.test_blitter__DOT__dut__DOT__width))) {
                    __Vdly__test_blitter__DOT__dut__DOT__col = 0U;
                    if ((((IData)(1U) + (IData)(vlSelfRef.test_blitter__DOT__dut__DOT__row)) 
                         >= (IData)(vlSelfRef.test_blitter__DOT__dut__DOT__height))) {
                        __VdlySet__test_blitter__DOT__dut__DOT__regs__v2 = 1U;
                        __Vdly__test_blitter__DOT__dut__DOT__state = 6U;
                        __VdlySet__test_blitter__DOT__dut__DOT__regs__v3 = 1U;
                        __VdlyVal__test_blitter__DOT__dut__DOT__regs__v4 
                            = (0x000000ffU & ((vlSelfRef.test_blitter__DOT__dut__DOT__wrote_count 
                                               + (((IData)(vlSelfRef.test_blitter__DOT__dut__DOT__colorkey_mode) 
                                                   & (vlSelfRef.test_blitter__DOT__dut__DOT__row_buf
                                                      [vlSelfRef.test_blitter__DOT__dut__DOT__buf_idx] 
                                                      == (IData)(vlSelfRef.test_blitter__DOT__dut__DOT__color_key)))
                                                   ? 0U
                                                   : 1U)) 
                                              >> 0x00000010U));
                        __VdlyVal__test_blitter__DOT__dut__DOT__regs__v5 
                            = (0x000000ffU & ((vlSelfRef.test_blitter__DOT__dut__DOT__wrote_count 
                                               + (((IData)(vlSelfRef.test_blitter__DOT__dut__DOT__colorkey_mode) 
                                                   & (vlSelfRef.test_blitter__DOT__dut__DOT__row_buf
                                                      [vlSelfRef.test_blitter__DOT__dut__DOT__buf_idx] 
                                                      == (IData)(vlSelfRef.test_blitter__DOT__dut__DOT__color_key)))
                                                   ? 0U
                                                   : 1U)) 
                                              >> 8U));
                        __VdlyVal__test_blitter__DOT__dut__DOT__regs__v6 
                            = (0x000000ffU & (vlSelfRef.test_blitter__DOT__dut__DOT__wrote_count 
                                              + (((IData)(vlSelfRef.test_blitter__DOT__dut__DOT__colorkey_mode) 
                                                  & (vlSelfRef.test_blitter__DOT__dut__DOT__row_buf
                                                     [vlSelfRef.test_blitter__DOT__dut__DOT__buf_idx] 
                                                     == (IData)(vlSelfRef.test_blitter__DOT__dut__DOT__color_key)))
                                                  ? 0U
                                                  : 1U)));
                    } else {
                        __Vdly__test_blitter__DOT__dut__DOT__row 
                            = (0x0000ffffU & ((IData)(1U) 
                                              + (IData)(vlSelfRef.test_blitter__DOT__dut__DOT__row)));
                        __Vdly__test_blitter__DOT__dut__DOT__state = 4U;
                    }
                    __Vdly__test_blitter__DOT__dut__DOT__buf_idx = 0U;
                } else {
                    __Vdly__test_blitter__DOT__dut__DOT__col 
                        = (0x0000ffffU & ((IData)(1U) 
                                          + (IData)(vlSelfRef.test_blitter__DOT__dut__DOT__col)));
                    __Vdly__test_blitter__DOT__dut__DOT__buf_idx 
                        = (0x000003ffU & ((IData)(1U) 
                                          + (IData)(vlSelfRef.test_blitter__DOT__dut__DOT__buf_idx)));
                }
            } else {
                __VdlyVal__test_blitter__DOT__dut__DOT__row_buf__v0 
                    = vlSelfRef.test_blitter__DOT__dut__DOT__mem_read_data;
                __VdlyDim0__test_blitter__DOT__dut__DOT__row_buf__v0 
                    = vlSelfRef.test_blitter__DOT__dut__DOT__buf_idx;
                __VdlySet__test_blitter__DOT__dut__DOT__row_buf__v0 = 1U;
                if ((((IData)(1U) + (IData)(vlSelfRef.test_blitter__DOT__dut__DOT__col)) 
                     >= (IData)(vlSelfRef.test_blitter__DOT__dut__DOT__width))) {
                    __Vdly__test_blitter__DOT__dut__DOT__col = 0U;
                    __Vdly__test_blitter__DOT__dut__DOT__buf_idx = 0U;
                    __Vdly__test_blitter__DOT__dut__DOT__state = 5U;
                } else {
                    __Vdly__test_blitter__DOT__dut__DOT__col 
                        = (0x0000ffffU & ((IData)(1U) 
                                          + (IData)(vlSelfRef.test_blitter__DOT__dut__DOT__col)));
                    __Vdly__test_blitter__DOT__dut__DOT__buf_idx 
                        = (0x000003ffU & ((IData)(1U) 
                                          + (IData)(vlSelfRef.test_blitter__DOT__dut__DOT__buf_idx)));
                }
            }
        } else if ((2U & (IData)(vlSelfRef.test_blitter__DOT__dut__DOT__state))) {
            if ((1U & (IData)(vlSelfRef.test_blitter__DOT__dut__DOT__state))) {
                if (vlSelfRef.test_blitter__DOT__dut__DOT__read_valid) {
                    if ((1U & (~ ((IData)(vlSelfRef.test_blitter__DOT__dut__DOT__colorkey_mode) 
                                  & ((IData)(vlSelfRef.test_blitter__DOT__dut__DOT__read_byte) 
                                     == (IData)(vlSelfRef.test_blitter__DOT__dut__DOT__color_key)))))) {
                        __Vdly__test_blitter__DOT__dut__DOT__wrote_count 
                            = (0x00ffffffU & ((IData)(1U) 
                                              + vlSelfRef.test_blitter__DOT__dut__DOT__wrote_count));
                    }
                    if ((((IData)(1U) + (IData)(vlSelfRef.test_blitter__DOT__dut__DOT__col)) 
                         >= (IData)(vlSelfRef.test_blitter__DOT__dut__DOT__width))) {
                        __Vdly__test_blitter__DOT__dut__DOT__col = 0U;
                        if ((((IData)(1U) + (IData)(vlSelfRef.test_blitter__DOT__dut__DOT__row)) 
                             >= (IData)(vlSelfRef.test_blitter__DOT__dut__DOT__height))) {
                            __VdlySet__test_blitter__DOT__dut__DOT__regs__v7 = 1U;
                            __Vdly__test_blitter__DOT__dut__DOT__state = 6U;
                            __VdlySet__test_blitter__DOT__dut__DOT__regs__v8 = 1U;
                            __VdlyVal__test_blitter__DOT__dut__DOT__regs__v9 
                                = (0x000000ffU & ((vlSelfRef.test_blitter__DOT__dut__DOT__wrote_count 
                                                   + 
                                                   (((IData)(vlSelfRef.test_blitter__DOT__dut__DOT__colorkey_mode) 
                                                     & ((IData)(vlSelfRef.test_blitter__DOT__dut__DOT__read_byte) 
                                                        == (IData)(vlSelfRef.test_blitter__DOT__dut__DOT__color_key)))
                                                     ? 0U
                                                     : 1U)) 
                                                  >> 0x00000010U));
                            __VdlyVal__test_blitter__DOT__dut__DOT__regs__v10 
                                = (0x000000ffU & ((vlSelfRef.test_blitter__DOT__dut__DOT__wrote_count 
                                                   + 
                                                   (((IData)(vlSelfRef.test_blitter__DOT__dut__DOT__colorkey_mode) 
                                                     & ((IData)(vlSelfRef.test_blitter__DOT__dut__DOT__read_byte) 
                                                        == (IData)(vlSelfRef.test_blitter__DOT__dut__DOT__color_key)))
                                                     ? 0U
                                                     : 1U)) 
                                                  >> 8U));
                            __VdlyVal__test_blitter__DOT__dut__DOT__regs__v11 
                                = (0x000000ffU & (vlSelfRef.test_blitter__DOT__dut__DOT__wrote_count 
                                                  + 
                                                  (((IData)(vlSelfRef.test_blitter__DOT__dut__DOT__colorkey_mode) 
                                                    & ((IData)(vlSelfRef.test_blitter__DOT__dut__DOT__read_byte) 
                                                       == (IData)(vlSelfRef.test_blitter__DOT__dut__DOT__color_key)))
                                                    ? 0U
                                                    : 1U)));
                        } else {
                            __Vdly__test_blitter__DOT__dut__DOT__row 
                                = (0x0000ffffU & ((IData)(1U) 
                                                  + (IData)(vlSelfRef.test_blitter__DOT__dut__DOT__row)));
                            if (vlSelfRef.test_blitter__DOT__dut__DOT__fill_mode) {
                                __Vdly__test_blitter__DOT__dut__DOT__state = 3U;
                                vlSelfRef.test_blitter__DOT__dut__DOT__read_byte 
                                    = vlSelfRef.test_blitter__DOT__dut__DOT__fill_value;
                            } else {
                                __Vdly__test_blitter__DOT__dut__DOT__state = 2U;
                            }
                        }
                    } else {
                        __Vdly__test_blitter__DOT__dut__DOT__col 
                            = (0x0000ffffU & ((IData)(1U) 
                                              + (IData)(vlSelfRef.test_blitter__DOT__dut__DOT__col)));
                        if (vlSelfRef.test_blitter__DOT__dut__DOT__fill_mode) {
                            vlSelfRef.test_blitter__DOT__dut__DOT__read_byte 
                                = vlSelfRef.test_blitter__DOT__dut__DOT__fill_value;
                            __Vdly__test_blitter__DOT__dut__DOT__state = 3U;
                        } else {
                            __Vdly__test_blitter__DOT__dut__DOT__state = 2U;
                        }
                    }
                }
            } else {
                vlSelfRef.test_blitter__DOT__dut__DOT__read_byte 
                    = vlSelfRef.test_blitter__DOT__dut__DOT__mem_read_data;
                __Vdly__test_blitter__DOT__dut__DOT__read_valid = 1U;
                __Vdly__test_blitter__DOT__dut__DOT__state = 3U;
            }
        } else if ((1U & (IData)(vlSelfRef.test_blitter__DOT__dut__DOT__state))) {
            if (((0U == (IData)(vlSelfRef.test_blitter__DOT__dut__DOT__width)) 
                 | (0U == (IData)(vlSelfRef.test_blitter__DOT__dut__DOT__height)))) {
                __VdlySet__test_blitter__DOT__dut__DOT__regs__v12 = 1U;
                __Vdly__test_blitter__DOT__dut__DOT__state = 6U;
                __VdlySet__test_blitter__DOT__dut__DOT__regs__v13 = 1U;
            } else if (((~ (IData)(vlSelfRef.test_blitter__DOT__dut__DOT__fill_mode)) 
                        & (6U < (IData)(vlSelfRef.test_blitter__DOT__dut__DOT__src_space)))) {
                __VdlySet__test_blitter__DOT__dut__DOT__regs__v14 = 1U;
                __Vdly__test_blitter__DOT__dut__DOT__state = 6U;
                __VdlySet__test_blitter__DOT__dut__DOT__regs__v15 = 1U;
            } else if ((6U < (IData)(vlSelfRef.test_blitter__DOT__dut__DOT__dst_space))) {
                __VdlySet__test_blitter__DOT__dut__DOT__regs__v16 = 1U;
                __Vdly__test_blitter__DOT__dut__DOT__state = 6U;
                __VdlySet__test_blitter__DOT__dut__DOT__regs__v17 = 1U;
            } else if ((1U & ((~ (IData)(vlSelfRef.test_blitter__DOT__dut__DOT__fill_mode)) 
                              & (~ ([&]() {
                                    __Vfunc_test_blitter__DOT__dut__DOT__space_size__674__sp 
                                        = vlSelfRef.test_blitter__DOT__dut__DOT__src_space;
                                    __Vfunc_test_blitter__DOT__dut__DOT__space_size__674__Vfuncout 
                                        = ((4U & (IData)(__Vfunc_test_blitter__DOT__dut__DOT__space_size__674__sp))
                                            ? ((2U 
                                                & (IData)(__Vfunc_test_blitter__DOT__dut__DOT__space_size__674__sp))
                                                ? (
                                                   (1U 
                                                    & (IData)(__Vfunc_test_blitter__DOT__dut__DOT__space_size__674__sp))
                                                    ? 0U
                                                    : 0x00008000U)
                                                : (
                                                   (1U 
                                                    & (IData)(__Vfunc_test_blitter__DOT__dut__DOT__space_size__674__sp))
                                                    ? 0x00080000U
                                                    : 0x00008000U))
                                            : ((2U 
                                                & (IData)(__Vfunc_test_blitter__DOT__dut__DOT__space_size__674__sp))
                                                ? (
                                                   (1U 
                                                    & (IData)(__Vfunc_test_blitter__DOT__dut__DOT__space_size__674__sp))
                                                    ? 0x0000fa00U
                                                    : 0x000007d0U)
                                                : (
                                                   (1U 
                                                    & (IData)(__Vfunc_test_blitter__DOT__dut__DOT__space_size__674__sp))
                                                    ? 0x000007d0U
                                                    : 0x00010000U)));
                                    __Vfunc_test_blitter__DOT__dut__DOT__rect_fits__673__sz 
                                        = __Vfunc_test_blitter__DOT__dut__DOT__space_size__674__Vfuncout;
                                    __Vfunc_test_blitter__DOT__dut__DOT__rect_fits__673__stride 
                                        = vlSelfRef.test_blitter__DOT__dut__DOT__src_stride;
                                    __Vfunc_test_blitter__DOT__dut__DOT__rect_fits__673__h 
                                        = vlSelfRef.test_blitter__DOT__dut__DOT__height;
                                    __Vfunc_test_blitter__DOT__dut__DOT__rect_fits__673__w 
                                        = vlSelfRef.test_blitter__DOT__dut__DOT__width;
                                    __Vfunc_test_blitter__DOT__dut__DOT__rect_fits__673__base 
                                        = vlSelfRef.test_blitter__DOT__dut__DOT__src_base;
                                    {
                                        __Vfunc_test_blitter__DOT__dut__DOT__rect_fits__673__Vfuncout = 0;
                                        __Vfunc_test_blitter__DOT__dut__DOT__rect_fits__673__last_row_start = 0;
                                        __Vfunc_test_blitter__DOT__dut__DOT__rect_fits__673__last_byte = 0;
                                        if ((((0U == (IData)(__Vfunc_test_blitter__DOT__dut__DOT__rect_fits__673__w)) 
                                              | (0U 
                                                 == (IData)(__Vfunc_test_blitter__DOT__dut__DOT__rect_fits__673__h))) 
                                             | (0U 
                                                == __Vfunc_test_blitter__DOT__dut__DOT__rect_fits__673__sz))) {
                                            __Vfunc_test_blitter__DOT__dut__DOT__rect_fits__673__Vfuncout = 0U;
                                            goto __Vlabel0;
                                        }
                                        __Vfunc_test_blitter__DOT__dut__DOT__rect_fits__673__last_row_start 
                                            = (__Vfunc_test_blitter__DOT__dut__DOT__rect_fits__673__base 
                                               + (((IData)(__Vfunc_test_blitter__DOT__dut__DOT__rect_fits__673__h) 
                                                   - (IData)(1U)) 
                                                  * (IData)(__Vfunc_test_blitter__DOT__dut__DOT__rect_fits__673__stride)));
                                        __Vfunc_test_blitter__DOT__dut__DOT__rect_fits__673__last_byte 
                                            = (__Vfunc_test_blitter__DOT__dut__DOT__rect_fits__673__last_row_start 
                                               + (IData)(__Vfunc_test_blitter__DOT__dut__DOT__rect_fits__673__w));
                                        __Vfunc_test_blitter__DOT__dut__DOT__rect_fits__673__Vfuncout 
                                            = ((__Vfunc_test_blitter__DOT__dut__DOT__rect_fits__673__last_byte 
                                                <= __Vfunc_test_blitter__DOT__dut__DOT__rect_fits__673__sz) 
                                               & (__Vfunc_test_blitter__DOT__dut__DOT__rect_fits__673__base 
                                                  < __Vfunc_test_blitter__DOT__dut__DOT__rect_fits__673__sz));
                                        __Vlabel0: ;
                                    }
                                }(), (IData)(__Vfunc_test_blitter__DOT__dut__DOT__rect_fits__673__Vfuncout)))))) {
                __VdlySet__test_blitter__DOT__dut__DOT__regs__v18 = 1U;
                __Vdly__test_blitter__DOT__dut__DOT__state = 6U;
            } else if (([&]() {
                        __Vfunc_test_blitter__DOT__dut__DOT__space_size__676__sp 
                            = vlSelfRef.test_blitter__DOT__dut__DOT__dst_space;
                        __Vfunc_test_blitter__DOT__dut__DOT__space_size__676__Vfuncout 
                            = ((4U & (IData)(__Vfunc_test_blitter__DOT__dut__DOT__space_size__676__sp))
                                ? ((2U & (IData)(__Vfunc_test_blitter__DOT__dut__DOT__space_size__676__sp))
                                    ? ((1U & (IData)(__Vfunc_test_blitter__DOT__dut__DOT__space_size__676__sp))
                                        ? 0U : 0x00008000U)
                                    : ((1U & (IData)(__Vfunc_test_blitter__DOT__dut__DOT__space_size__676__sp))
                                        ? 0x00080000U
                                        : 0x00008000U))
                                : ((2U & (IData)(__Vfunc_test_blitter__DOT__dut__DOT__space_size__676__sp))
                                    ? ((1U & (IData)(__Vfunc_test_blitter__DOT__dut__DOT__space_size__676__sp))
                                        ? 0x0000fa00U
                                        : 0x000007d0U)
                                    : ((1U & (IData)(__Vfunc_test_blitter__DOT__dut__DOT__space_size__676__sp))
                                        ? 0x000007d0U
                                        : 0x00010000U)));
                        __Vfunc_test_blitter__DOT__dut__DOT__rect_fits__675__sz 
                            = __Vfunc_test_blitter__DOT__dut__DOT__space_size__676__Vfuncout;
                        __Vfunc_test_blitter__DOT__dut__DOT__rect_fits__675__stride 
                            = vlSelfRef.test_blitter__DOT__dut__DOT__dst_stride;
                        __Vfunc_test_blitter__DOT__dut__DOT__rect_fits__675__h 
                            = vlSelfRef.test_blitter__DOT__dut__DOT__height;
                        __Vfunc_test_blitter__DOT__dut__DOT__rect_fits__675__w 
                            = vlSelfRef.test_blitter__DOT__dut__DOT__width;
                        __Vfunc_test_blitter__DOT__dut__DOT__rect_fits__675__base 
                            = vlSelfRef.test_blitter__DOT__dut__DOT__dst_base;
                        {
                            __Vfunc_test_blitter__DOT__dut__DOT__rect_fits__675__Vfuncout = 0;
                            __Vfunc_test_blitter__DOT__dut__DOT__rect_fits__675__last_row_start = 0;
                            __Vfunc_test_blitter__DOT__dut__DOT__rect_fits__675__last_byte = 0;
                            if ((((0U == (IData)(__Vfunc_test_blitter__DOT__dut__DOT__rect_fits__675__w)) 
                                  | (0U == (IData)(__Vfunc_test_blitter__DOT__dut__DOT__rect_fits__675__h))) 
                                 | (0U == __Vfunc_test_blitter__DOT__dut__DOT__rect_fits__675__sz))) {
                                __Vfunc_test_blitter__DOT__dut__DOT__rect_fits__675__Vfuncout = 0U;
                                goto __Vlabel1;
                            }
                            __Vfunc_test_blitter__DOT__dut__DOT__rect_fits__675__last_row_start 
                                = (__Vfunc_test_blitter__DOT__dut__DOT__rect_fits__675__base 
                                   + (((IData)(__Vfunc_test_blitter__DOT__dut__DOT__rect_fits__675__h) 
                                       - (IData)(1U)) 
                                      * (IData)(__Vfunc_test_blitter__DOT__dut__DOT__rect_fits__675__stride)));
                            __Vfunc_test_blitter__DOT__dut__DOT__rect_fits__675__last_byte 
                                = (__Vfunc_test_blitter__DOT__dut__DOT__rect_fits__675__last_row_start 
                                   + (IData)(__Vfunc_test_blitter__DOT__dut__DOT__rect_fits__675__w));
                            __Vfunc_test_blitter__DOT__dut__DOT__rect_fits__675__Vfuncout 
                                = ((__Vfunc_test_blitter__DOT__dut__DOT__rect_fits__675__last_byte 
                                    <= __Vfunc_test_blitter__DOT__dut__DOT__rect_fits__675__sz) 
                                   & (__Vfunc_test_blitter__DOT__dut__DOT__rect_fits__675__base 
                                      < __Vfunc_test_blitter__DOT__dut__DOT__rect_fits__675__sz));
                            __Vlabel1: ;
                        }
                    }(), (IData)(__Vfunc_test_blitter__DOT__dut__DOT__rect_fits__675__Vfuncout))) {
                if (([&]() {
                            __Vfunc_test_blitter__DOT__dut__DOT__write_protect_ok__677__stride 
                                = vlSelfRef.test_blitter__DOT__dut__DOT__dst_stride;
                            __Vfunc_test_blitter__DOT__dut__DOT__write_protect_ok__677__h 
                                = vlSelfRef.test_blitter__DOT__dut__DOT__height;
                            __Vfunc_test_blitter__DOT__dut__DOT__write_protect_ok__677__w 
                                = vlSelfRef.test_blitter__DOT__dut__DOT__width;
                            __Vfunc_test_blitter__DOT__dut__DOT__write_protect_ok__677__base 
                                = vlSelfRef.test_blitter__DOT__dut__DOT__dst_base;
                            __Vfunc_test_blitter__DOT__dut__DOT__write_protect_ok__677__space 
                                = vlSelfRef.test_blitter__DOT__dut__DOT__dst_space;
                            {
                                __Vfunc_test_blitter__DOT__dut__DOT__write_protect_ok__677__Vfuncout = 0;
                                __Vfunc_test_blitter__DOT__dut__DOT__write_protect_ok__677__last_byte = 0;
                                if ((0U != (IData)(__Vfunc_test_blitter__DOT__dut__DOT__write_protect_ok__677__space))) {
                                    __Vfunc_test_blitter__DOT__dut__DOT__write_protect_ok__677__Vfuncout = 1U;
                                    goto __Vlabel2;
                                }
                                __Vfunc_test_blitter__DOT__dut__DOT__write_protect_ok__677__last_byte 
                                    = ((__Vfunc_test_blitter__DOT__dut__DOT__write_protect_ok__677__base 
                                        + (((IData)(__Vfunc_test_blitter__DOT__dut__DOT__write_protect_ok__677__h) 
                                            - (IData)(1U)) 
                                           * (IData)(__Vfunc_test_blitter__DOT__dut__DOT__write_protect_ok__677__stride))) 
                                       + (IData)(__Vfunc_test_blitter__DOT__dut__DOT__write_protect_ok__677__w));
                                __Vfunc_test_blitter__DOT__dut__DOT__write_protect_ok__677__Vfuncout 
                                    = (0x0000c000U 
                                       >= __Vfunc_test_blitter__DOT__dut__DOT__write_protect_ok__677__last_byte);
                                __Vlabel2: ;
                            }
                        }(), (IData)(__Vfunc_test_blitter__DOT__dut__DOT__write_protect_ok__677__Vfuncout))) {
                    if (vlSelfRef.test_blitter__DOT__dut__DOT__fill_mode) {
                        __Vdly__test_blitter__DOT__dut__DOT__state = 3U;
                        __Vdly__test_blitter__DOT__dut__DOT__read_valid = 1U;
                        vlSelfRef.test_blitter__DOT__dut__DOT__read_byte 
                            = vlSelfRef.test_blitter__DOT__dut__DOT__fill_value;
                    } else if (vlSelfRef.test_blitter__DOT__dut__DOT__use_row_buffer) {
                        __Vdly__test_blitter__DOT__dut__DOT__state = 4U;
                        __Vdly__test_blitter__DOT__dut__DOT__col = 0U;
                        __Vdly__test_blitter__DOT__dut__DOT__buf_idx = 0U;
                    } else {
                        __Vdly__test_blitter__DOT__dut__DOT__state = 2U;
                        __Vdly__test_blitter__DOT__dut__DOT__read_valid = 0U;
                    }
                } else {
                    __VdlySet__test_blitter__DOT__dut__DOT__regs__v20 = 1U;
                    __Vdly__test_blitter__DOT__dut__DOT__state = 6U;
                }
            } else {
                __VdlySet__test_blitter__DOT__dut__DOT__regs__v22 = 1U;
                __Vdly__test_blitter__DOT__dut__DOT__state = 6U;
            }
        }
        if (((IData)(vlSelfRef.test_blitter__DOT__cpu_we) 
             & (IData)(vlSelfRef.test_blitter__DOT__dut__DOT__blt_sel))) {
            if ((((((1U != (0x0000001fU & ((IData)(vlSelfRef.test_blitter__DOT__cpu_addr) 
                                           - (IData)(3U)))) 
                    & (2U != (0x0000001fU & ((IData)(vlSelfRef.test_blitter__DOT__cpu_addr) 
                                             - (IData)(3U))))) 
                   & (0x16U != (0x0000001fU & ((IData)(vlSelfRef.test_blitter__DOT__cpu_addr) 
                                               - (IData)(3U))))) 
                  & (0x17U != (0x0000001fU & ((IData)(vlSelfRef.test_blitter__DOT__cpu_addr) 
                                              - (IData)(3U))))) 
                 & (0x18U != (0x0000001fU & ((IData)(vlSelfRef.test_blitter__DOT__cpu_addr) 
                                             - (IData)(3U)))))) {
                vlSelfRef.test_blitter__DOT__dut__DOT____Vlvbound_hf31f73b8__0 
                    = vlSelfRef.test_blitter__DOT__cpu_wdata;
                if (VL_LIKELY(((0x18U >= (0x0000001fU 
                                          & ((IData)(vlSelfRef.test_blitter__DOT__cpu_addr) 
                                             - (IData)(3U))))))) {
                    __VdlyVal__test_blitter__DOT__dut__DOT__regs__v24 
                        = vlSelfRef.test_blitter__DOT__dut__DOT____Vlvbound_hf31f73b8__0;
                    __VdlyDim0__test_blitter__DOT__dut__DOT__regs__v24 
                        = (0x0000001fU & ((IData)(vlSelfRef.test_blitter__DOT__cpu_addr) 
                                          - (IData)(3U)));
                    __VdlySet__test_blitter__DOT__dut__DOT__regs__v24 = 1U;
                }
            }
            if ((0U == (0x0000001fU & ((IData)(vlSelfRef.test_blitter__DOT__cpu_addr) 
                                       - (IData)(3U))))) {
                if ((1U == (IData)(vlSelfRef.test_blitter__DOT__cpu_wdata))) {
                    if ((0U == (IData)(vlSelfRef.test_blitter__DOT__dut__DOT__state))) {
                        vlSelfRef.test_blitter__DOT__dut__DOT__src_space 
                            = (7U & vlSelfRef.test_blitter__DOT__dut__DOT__regs[3U]);
                        vlSelfRef.test_blitter__DOT__dut__DOT__dst_space 
                            = (7U & vlSelfRef.test_blitter__DOT__dut__DOT__regs[4U]);
                        vlSelfRef.test_blitter__DOT__dut__DOT__src_base 
                            = ((vlSelfRef.test_blitter__DOT__dut__DOT__regs[7U] 
                                << 0x00000010U) | (
                                                   (vlSelfRef.test_blitter__DOT__dut__DOT__regs[6U] 
                                                    << 8U) 
                                                   | vlSelfRef.test_blitter__DOT__dut__DOT__regs[5U]));
                        vlSelfRef.test_blitter__DOT__dut__DOT__dst_base 
                            = ((vlSelfRef.test_blitter__DOT__dut__DOT__regs[10U] 
                                << 0x00000010U) | (
                                                   (vlSelfRef.test_blitter__DOT__dut__DOT__regs[9U] 
                                                    << 8U) 
                                                   | vlSelfRef.test_blitter__DOT__dut__DOT__regs[8U]));
                        vlSelfRef.test_blitter__DOT__dut__DOT__width 
                            = ((vlSelfRef.test_blitter__DOT__dut__DOT__regs[12U] 
                                << 8U) | vlSelfRef.test_blitter__DOT__dut__DOT__regs[11U]);
                        vlSelfRef.test_blitter__DOT__dut__DOT__height 
                            = ((vlSelfRef.test_blitter__DOT__dut__DOT__regs[14U] 
                                << 8U) | vlSelfRef.test_blitter__DOT__dut__DOT__regs[13U]);
                        vlSelfRef.test_blitter__DOT__dut__DOT__src_stride 
                            = ((vlSelfRef.test_blitter__DOT__dut__DOT__regs[16U] 
                                << 8U) | vlSelfRef.test_blitter__DOT__dut__DOT__regs[15U]);
                        vlSelfRef.test_blitter__DOT__dut__DOT__dst_stride 
                            = ((vlSelfRef.test_blitter__DOT__dut__DOT__regs[18U] 
                                << 8U) | vlSelfRef.test_blitter__DOT__dut__DOT__regs[17U]);
                        vlSelfRef.test_blitter__DOT__dut__DOT__fill_mode 
                            = (1U & vlSelfRef.test_blitter__DOT__dut__DOT__regs[19U]);
                        vlSelfRef.test_blitter__DOT__dut__DOT__colorkey_mode 
                            = (1U & (vlSelfRef.test_blitter__DOT__dut__DOT__regs[19U] 
                                     >> 1U));
                        vlSelfRef.test_blitter__DOT__dut__DOT__fill_value 
                            = vlSelfRef.test_blitter__DOT__dut__DOT__regs[20U];
                        vlSelfRef.test_blitter__DOT__dut__DOT__color_key 
                            = vlSelfRef.test_blitter__DOT__dut__DOT__regs[21U];
                        vlSelfRef.test_blitter__DOT__dut__DOT__use_row_buffer 
                            = ((~ vlSelfRef.test_blitter__DOT__dut__DOT__regs[19U]) 
                               & ((7U & vlSelfRef.test_blitter__DOT__dut__DOT__regs[3U]) 
                                  == (7U & vlSelfRef.test_blitter__DOT__dut__DOT__regs[4U])));
                        __Vdly__test_blitter__DOT__dut__DOT__row = 0U;
                        __Vdly__test_blitter__DOT__dut__DOT__col = 0U;
                        __Vdly__test_blitter__DOT__dut__DOT__wrote_count = 0U;
                        __Vdly__test_blitter__DOT__dut__DOT__read_valid = 0U;
                        __Vdly__test_blitter__DOT__dut__DOT__buf_idx = 0U;
                        __VdlySet__test_blitter__DOT__dut__DOT__regs__v25 = 1U;
                        __Vdly__test_blitter__DOT__dut__DOT__state = 1U;
                    } else {
                        __VdlySet__test_blitter__DOT__dut__DOT__regs__v30 = 1U;
                    }
                } else {
                    __VdlySet__test_blitter__DOT__dut__DOT__regs__v32 = 1U;
                }
            }
        }
    }
    if (__VdlySet__test_blitter__DOT__sim_ram__v0) {
        vlSelfRef.test_blitter__DOT__sim_ram[__VdlyDim0__test_blitter__DOT__sim_ram__v0] 
            = __VdlyVal__test_blitter__DOT__sim_ram__v0;
    }
    if (__VdlySet__test_blitter__DOT__sim_xram__v0) {
        vlSelfRef.test_blitter__DOT__sim_xram[__VdlyDim0__test_blitter__DOT__sim_xram__v0] 
            = __VdlyVal__test_blitter__DOT__sim_xram__v0;
    }
    if (__VdlySet__test_blitter__DOT__sim_sprite__v0) {
        vlSelfRef.test_blitter__DOT__sim_sprite[__VdlyDim0__test_blitter__DOT__sim_sprite__v0] 
            = __VdlyVal__test_blitter__DOT__sim_sprite__v0;
    }
    if (__VdlySet__test_blitter__DOT__sim_gfx__v0) {
        vlSelfRef.test_blitter__DOT__sim_gfx[__VdlyDim0__test_blitter__DOT__sim_gfx__v0] 
            = __VdlyVal__test_blitter__DOT__sim_gfx__v0;
    }
    if (__VdlySet__test_blitter__DOT__sim_color__v0) {
        vlSelfRef.test_blitter__DOT__sim_color[__VdlyDim0__test_blitter__DOT__sim_color__v0] 
            = __VdlyVal__test_blitter__DOT__sim_color__v0;
    }
    if (__VdlySet__test_blitter__DOT__sim_char__v0) {
        vlSelfRef.test_blitter__DOT__sim_char[__VdlyDim0__test_blitter__DOT__sim_char__v0] 
            = __VdlyVal__test_blitter__DOT__sim_char__v0;
    }
    vlSelfRef.test_blitter__DOT__dut__DOT__blt_sel 
        = ((0xba83U <= (IData)(vlSelfRef.test_blitter__DOT__cpu_addr)) 
           & (0xba9bU >= (IData)(vlSelfRef.test_blitter__DOT__cpu_addr)));
    vlSelfRef.test_blitter__DOT__dut__DOT__wrote_count 
        = __Vdly__test_blitter__DOT__dut__DOT__wrote_count;
    if (__VdlySet__test_blitter__DOT__dut__DOT__regs__v0) {
        vlSelfRef.test_blitter__DOT__dut__DOT__regs[1U] = 0U;
    }
    if (__VdlySet__test_blitter__DOT__dut__DOT__regs__v1) {
        vlSelfRef.test_blitter__DOT__dut__DOT__regs[2U] = 0U;
    }
    if (__VdlySet__test_blitter__DOT__dut__DOT__regs__v2) {
        vlSelfRef.test_blitter__DOT__dut__DOT__regs[1U] = 2U;
    }
    if (__VdlySet__test_blitter__DOT__dut__DOT__regs__v3) {
        vlSelfRef.test_blitter__DOT__dut__DOT__regs[2U] = 0U;
        vlSelfRef.test_blitter__DOT__dut__DOT__regs[24U] 
            = __VdlyVal__test_blitter__DOT__dut__DOT__regs__v4;
        vlSelfRef.test_blitter__DOT__dut__DOT__regs[23U] 
            = __VdlyVal__test_blitter__DOT__dut__DOT__regs__v5;
        vlSelfRef.test_blitter__DOT__dut__DOT__regs[22U] 
            = __VdlyVal__test_blitter__DOT__dut__DOT__regs__v6;
    }
    if (__VdlySet__test_blitter__DOT__dut__DOT__regs__v7) {
        vlSelfRef.test_blitter__DOT__dut__DOT__regs[1U] = 2U;
    }
    if (__VdlySet__test_blitter__DOT__dut__DOT__regs__v8) {
        vlSelfRef.test_blitter__DOT__dut__DOT__regs[2U] = 0U;
        vlSelfRef.test_blitter__DOT__dut__DOT__regs[24U] 
            = __VdlyVal__test_blitter__DOT__dut__DOT__regs__v9;
        vlSelfRef.test_blitter__DOT__dut__DOT__regs[23U] 
            = __VdlyVal__test_blitter__DOT__dut__DOT__regs__v10;
        vlSelfRef.test_blitter__DOT__dut__DOT__regs[22U] 
            = __VdlyVal__test_blitter__DOT__dut__DOT__regs__v11;
    }
    if (__VdlySet__test_blitter__DOT__dut__DOT__regs__v12) {
        vlSelfRef.test_blitter__DOT__dut__DOT__regs[1U] = 3U;
    }
    if (__VdlySet__test_blitter__DOT__dut__DOT__regs__v13) {
        vlSelfRef.test_blitter__DOT__dut__DOT__regs[2U] = 4U;
    }
    if (__VdlySet__test_blitter__DOT__dut__DOT__regs__v14) {
        vlSelfRef.test_blitter__DOT__dut__DOT__regs[1U] = 3U;
    }
    if (__VdlySet__test_blitter__DOT__dut__DOT__regs__v15) {
        vlSelfRef.test_blitter__DOT__dut__DOT__regs[2U] = 2U;
    }
    if (__VdlySet__test_blitter__DOT__dut__DOT__regs__v16) {
        vlSelfRef.test_blitter__DOT__dut__DOT__regs[1U] = 3U;
    }
    if (__VdlySet__test_blitter__DOT__dut__DOT__regs__v17) {
        vlSelfRef.test_blitter__DOT__dut__DOT__regs[2U] = 2U;
    }
    if (__VdlySet__test_blitter__DOT__dut__DOT__regs__v18) {
        vlSelfRef.test_blitter__DOT__dut__DOT__regs[1U] = 3U;
        vlSelfRef.test_blitter__DOT__dut__DOT__regs[2U] = 3U;
    }
    if (__VdlySet__test_blitter__DOT__dut__DOT__regs__v20) {
        vlSelfRef.test_blitter__DOT__dut__DOT__regs[1U] = 3U;
        vlSelfRef.test_blitter__DOT__dut__DOT__regs[2U] = 5U;
    }
    if (__VdlySet__test_blitter__DOT__dut__DOT__regs__v22) {
        vlSelfRef.test_blitter__DOT__dut__DOT__regs[1U] = 3U;
        vlSelfRef.test_blitter__DOT__dut__DOT__regs[2U] = 3U;
    }
    if (__VdlySet__test_blitter__DOT__dut__DOT__regs__v24) {
        vlSelfRef.test_blitter__DOT__dut__DOT__regs[__VdlyDim0__test_blitter__DOT__dut__DOT__regs__v24] 
            = __VdlyVal__test_blitter__DOT__dut__DOT__regs__v24;
    }
    if (__VdlySet__test_blitter__DOT__dut__DOT__regs__v25) {
        vlSelfRef.test_blitter__DOT__dut__DOT__regs[1U] = 1U;
        vlSelfRef.test_blitter__DOT__dut__DOT__regs[2U] = 0U;
        vlSelfRef.test_blitter__DOT__dut__DOT__regs[22U] = 0U;
        vlSelfRef.test_blitter__DOT__dut__DOT__regs[23U] = 0U;
        vlSelfRef.test_blitter__DOT__dut__DOT__regs[24U] = 0U;
    }
    if (__VdlySet__test_blitter__DOT__dut__DOT__regs__v30) {
        vlSelfRef.test_blitter__DOT__dut__DOT__regs[1U] = 3U;
        vlSelfRef.test_blitter__DOT__dut__DOT__regs[2U] = 1U;
    }
    if (__VdlySet__test_blitter__DOT__dut__DOT__regs__v32) {
        vlSelfRef.test_blitter__DOT__dut__DOT__regs[1U] = 3U;
        vlSelfRef.test_blitter__DOT__dut__DOT__regs[2U] = 1U;
    }
    vlSelfRef.test_blitter__DOT__dut__DOT__state = __Vdly__test_blitter__DOT__dut__DOT__state;
    vlSelfRef.test_blitter__DOT__dut__DOT__read_valid 
        = __Vdly__test_blitter__DOT__dut__DOT__read_valid;
    vlSelfRef.test_blitter__DOT__dut__DOT__buf_idx 
        = __Vdly__test_blitter__DOT__dut__DOT__buf_idx;
    if (__VdlySet__test_blitter__DOT__dut__DOT__row_buf__v0) {
        vlSelfRef.test_blitter__DOT__dut__DOT__row_buf[__VdlyDim0__test_blitter__DOT__dut__DOT__row_buf__v0] 
            = __VdlyVal__test_blitter__DOT__dut__DOT__row_buf__v0;
    }
    vlSelfRef.test_blitter__DOT__dut__DOT__col = __Vdly__test_blitter__DOT__dut__DOT__col;
    vlSelfRef.test_blitter__DOT__dut__DOT__row = __Vdly__test_blitter__DOT__dut__DOT__row;
    vlSelfRef.test_blitter__DOT__blt_ram_we = 0U;
    vlSelfRef.test_blitter__DOT__blt_ram_wdata = 0U;
    vlSelfRef.test_blitter__DOT__blt_xram_we = 0U;
    vlSelfRef.test_blitter__DOT__blt_xram_wdata = 0U;
    vlSelfRef.test_blitter__DOT__blt_vgc_we = 0U;
    vlSelfRef.test_blitter__DOT__blt_vgc_wdata = 0U;
    vlSelfRef.test_blitter__DOT__blt_vgc_space = 0U;
    test_blitter__DOT__dut__DOT__src_addr = (0x00ffffffU 
                                             & (vlSelfRef.test_blitter__DOT__dut__DOT__src_base 
                                                + (
                                                   ((IData)(vlSelfRef.test_blitter__DOT__dut__DOT__row) 
                                                    * (IData)(vlSelfRef.test_blitter__DOT__dut__DOT__src_stride)) 
                                                   + (IData)(vlSelfRef.test_blitter__DOT__dut__DOT__col))));
    test_blitter__DOT__dut__DOT__dst_addr = (0x00ffffffU 
                                             & (vlSelfRef.test_blitter__DOT__dut__DOT__dst_base 
                                                + (
                                                   ((IData)(vlSelfRef.test_blitter__DOT__dut__DOT__row) 
                                                    * (IData)(vlSelfRef.test_blitter__DOT__dut__DOT__dst_stride)) 
                                                   + (IData)(vlSelfRef.test_blitter__DOT__dut__DOT__col))));
    vlSelfRef.test_blitter__DOT__blt_ram_addr = 0U;
    vlSelfRef.test_blitter__DOT__blt_xram_addr = 0U;
    if (((2U == (IData)(vlSelfRef.test_blitter__DOT__dut__DOT__state)) 
         | (4U == (IData)(vlSelfRef.test_blitter__DOT__dut__DOT__state)))) {
        vlSelfRef.test_blitter__DOT__blt_vgc_addr = 0U;
        if ((1U & (~ (IData)(vlSelfRef.test_blitter__DOT__dut__DOT__fill_mode)))) {
            if ((0U != (IData)(vlSelfRef.test_blitter__DOT__dut__DOT__src_space))) {
                if ((5U != (IData)(vlSelfRef.test_blitter__DOT__dut__DOT__src_space))) {
                    vlSelfRef.test_blitter__DOT__blt_vgc_space 
                        = vlSelfRef.test_blitter__DOT__dut__DOT__src_space;
                    vlSelfRef.test_blitter__DOT__blt_vgc_addr 
                        = (0x0000ffffU & test_blitter__DOT__dut__DOT__src_addr);
                }
                if ((5U == (IData)(vlSelfRef.test_blitter__DOT__dut__DOT__src_space))) {
                    vlSelfRef.test_blitter__DOT__blt_xram_addr 
                        = (0x0007ffffU & test_blitter__DOT__dut__DOT__src_addr);
                }
            }
            if ((0U == (IData)(vlSelfRef.test_blitter__DOT__dut__DOT__src_space))) {
                vlSelfRef.test_blitter__DOT__blt_ram_addr 
                    = (0x0000ffffU & test_blitter__DOT__dut__DOT__src_addr);
            }
        }
    } else {
        vlSelfRef.test_blitter__DOT__blt_vgc_addr = 0U;
    }
    if (((((3U == (IData)(vlSelfRef.test_blitter__DOT__dut__DOT__state)) 
           & (IData)(vlSelfRef.test_blitter__DOT__dut__DOT__read_valid)) 
          & (~ ((IData)(vlSelfRef.test_blitter__DOT__dut__DOT__colorkey_mode) 
                & ((IData)(vlSelfRef.test_blitter__DOT__dut__DOT__read_byte) 
                   == (IData)(vlSelfRef.test_blitter__DOT__dut__DOT__color_key))))) 
         | ((5U == (IData)(vlSelfRef.test_blitter__DOT__dut__DOT__state)) 
            & (~ ((IData)(vlSelfRef.test_blitter__DOT__dut__DOT__colorkey_mode) 
                  & (vlSelfRef.test_blitter__DOT__dut__DOT__row_buf
                     [vlSelfRef.test_blitter__DOT__dut__DOT__buf_idx] 
                     == (IData)(vlSelfRef.test_blitter__DOT__dut__DOT__color_key))))))) {
        if ((0U == (IData)(vlSelfRef.test_blitter__DOT__dut__DOT__dst_space))) {
            vlSelfRef.test_blitter__DOT__blt_ram_we = 1U;
            vlSelfRef.test_blitter__DOT__blt_ram_wdata 
                = ((5U == (IData)(vlSelfRef.test_blitter__DOT__dut__DOT__state))
                    ? vlSelfRef.test_blitter__DOT__dut__DOT__row_buf
                   [vlSelfRef.test_blitter__DOT__dut__DOT__buf_idx]
                    : (IData)(vlSelfRef.test_blitter__DOT__dut__DOT__read_byte));
            vlSelfRef.test_blitter__DOT__blt_ram_addr 
                = (0x0000ffffU & test_blitter__DOT__dut__DOT__dst_addr);
        }
        if ((0U != (IData)(vlSelfRef.test_blitter__DOT__dut__DOT__dst_space))) {
            if ((5U == (IData)(vlSelfRef.test_blitter__DOT__dut__DOT__dst_space))) {
                vlSelfRef.test_blitter__DOT__blt_xram_we = 1U;
                vlSelfRef.test_blitter__DOT__blt_xram_wdata 
                    = ((5U == (IData)(vlSelfRef.test_blitter__DOT__dut__DOT__state))
                        ? vlSelfRef.test_blitter__DOT__dut__DOT__row_buf
                       [vlSelfRef.test_blitter__DOT__dut__DOT__buf_idx]
                        : (IData)(vlSelfRef.test_blitter__DOT__dut__DOT__read_byte));
                vlSelfRef.test_blitter__DOT__blt_xram_addr 
                    = (0x0007ffffU & test_blitter__DOT__dut__DOT__dst_addr);
            }
            if ((5U != (IData)(vlSelfRef.test_blitter__DOT__dut__DOT__dst_space))) {
                vlSelfRef.test_blitter__DOT__blt_vgc_we = 1U;
                vlSelfRef.test_blitter__DOT__blt_vgc_wdata 
                    = ((5U == (IData)(vlSelfRef.test_blitter__DOT__dut__DOT__state))
                        ? vlSelfRef.test_blitter__DOT__dut__DOT__row_buf
                       [vlSelfRef.test_blitter__DOT__dut__DOT__buf_idx]
                        : (IData)(vlSelfRef.test_blitter__DOT__dut__DOT__read_byte));
                vlSelfRef.test_blitter__DOT__blt_vgc_space 
                    = vlSelfRef.test_blitter__DOT__dut__DOT__dst_space;
                vlSelfRef.test_blitter__DOT__blt_vgc_addr 
                    = (0x0000ffffU & test_blitter__DOT__dut__DOT__dst_addr);
            }
        }
    }
    vlSelfRef.test_blitter__DOT__dut__DOT__mem_read_data 
        = ((0U == (IData)(vlSelfRef.test_blitter__DOT__dut__DOT__src_space))
            ? vlSelfRef.test_blitter__DOT__sim_ram[vlSelfRef.test_blitter__DOT__blt_ram_addr]
            : ((5U == (IData)(vlSelfRef.test_blitter__DOT__dut__DOT__src_space))
                ? vlSelfRef.test_blitter__DOT__sim_xram
               [vlSelfRef.test_blitter__DOT__blt_xram_addr]
                : ((4U & (IData)(vlSelfRef.test_blitter__DOT__blt_vgc_space))
                    ? ((2U & (IData)(vlSelfRef.test_blitter__DOT__blt_vgc_space))
                        ? 0U : ((1U & (IData)(vlSelfRef.test_blitter__DOT__blt_vgc_space))
                                 ? 0U : vlSelfRef.test_blitter__DOT__sim_sprite
                                [(0x00007fffU & (IData)(vlSelfRef.test_blitter__DOT__blt_vgc_addr))]))
                    : ((2U & (IData)(vlSelfRef.test_blitter__DOT__blt_vgc_space))
                        ? ((1U & (IData)(vlSelfRef.test_blitter__DOT__blt_vgc_space))
                            ? ((0xf9ffU >= (IData)(vlSelfRef.test_blitter__DOT__blt_vgc_addr))
                                ? vlSelfRef.test_blitter__DOT__sim_gfx
                               [vlSelfRef.test_blitter__DOT__blt_vgc_addr]
                                : 0U) : ((0x07cfU >= 
                                          (0x000007ffU 
                                           & (IData)(vlSelfRef.test_blitter__DOT__blt_vgc_addr)))
                                          ? vlSelfRef.test_blitter__DOT__sim_color
                                         [(0x000007ffU 
                                           & (IData)(vlSelfRef.test_blitter__DOT__blt_vgc_addr))]
                                          : 0U)) : 
                       ((1U & (IData)(vlSelfRef.test_blitter__DOT__blt_vgc_space))
                         ? ((0x07cfU >= (0x000007ffU 
                                         & (IData)(vlSelfRef.test_blitter__DOT__blt_vgc_addr)))
                             ? vlSelfRef.test_blitter__DOT__sim_char
                            [(0x000007ffU & (IData)(vlSelfRef.test_blitter__DOT__blt_vgc_addr))]
                             : 0U) : 0U)))));
}

void Vtest_blitter___024root___eval_nba(Vtest_blitter___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vtest_blitter___024root___eval_nba\n"); );
    Vtest_blitter__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
    // Body
    if ((1ULL & vlSelfRef.__VnbaTriggered[0U])) {
        Vtest_blitter___024root___nba_sequent__TOP__0(vlSelf);
    }
}

void Vtest_blitter___024root___timing_ready(Vtest_blitter___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vtest_blitter___024root___timing_ready\n"); );
    Vtest_blitter__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
    // Body
    if ((1ULL & vlSelfRef.__VactTriggered[0U])) {
        vlSelfRef.__VtrigSched_h603fc354__0.ready("@(posedge test_blitter.clk)");
    }
}

void Vtest_blitter___024root___timing_resume(Vtest_blitter___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vtest_blitter___024root___timing_resume\n"); );
    Vtest_blitter__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
    // Body
    vlSelfRef.__VtrigSched_h603fc354__0.moveToResumeQueue(
                                                          "@(posedge test_blitter.clk)");
    vlSelfRef.__VtrigSched_h603fc354__0.resume("@(posedge test_blitter.clk)");
    if ((2ULL & vlSelfRef.__VactTriggered[0U])) {
        vlSelfRef.__VdlySched.resume();
    }
}

void Vtest_blitter___024root___trigger_orInto__act_vec_vec(VlUnpacked<QData/*63:0*/, 1> &out, const VlUnpacked<QData/*63:0*/, 1> &in) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vtest_blitter___024root___trigger_orInto__act_vec_vec\n"); );
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
VL_ATTR_COLD void Vtest_blitter___024root___dump_triggers__act(const VlUnpacked<QData/*63:0*/, 1> &triggers, const std::string &tag);
#endif  // VL_DEBUG

bool Vtest_blitter___024root___eval_phase__act(Vtest_blitter___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vtest_blitter___024root___eval_phase__act\n"); );
    Vtest_blitter__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
    // Locals
    CData/*0:0*/ __VactExecute;
    // Body
    Vtest_blitter___024root___eval_triggers_vec__act(vlSelf);
    Vtest_blitter___024root___timing_ready(vlSelf);
    Vtest_blitter___024root___trigger_orInto__act_vec_vec(vlSelfRef.__VactTriggered, vlSelfRef.__VactTriggeredAcc);
#ifdef VL_DEBUG
    if (VL_UNLIKELY(vlSymsp->_vm_contextp__->debug())) {
        Vtest_blitter___024root___dump_triggers__act(vlSelfRef.__VactTriggered, "act"s);
    }
#endif
    Vtest_blitter___024root___trigger_orInto__act_vec_vec(vlSelfRef.__VnbaTriggered, vlSelfRef.__VactTriggered);
    __VactExecute = Vtest_blitter___024root___trigger_anySet__act(vlSelfRef.__VactTriggered);
    if (__VactExecute) {
        vlSelfRef.__VactTriggeredAcc.fill(0ULL);
        Vtest_blitter___024root___timing_resume(vlSelf);
        Vtest_blitter___024root___eval_act(vlSelf);
    }
    return (__VactExecute);
}

bool Vtest_blitter___024root___eval_phase__inact(Vtest_blitter___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vtest_blitter___024root___eval_phase__inact\n"); );
    Vtest_blitter__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
    // Locals
    CData/*0:0*/ __VinactExecute;
    // Body
    __VinactExecute = vlSelfRef.__VdlySched.awaitingZeroDelay();
    if (__VinactExecute) {
        VL_FATAL_MT("test_blitter.sv", 6, "", "ZERODLY: Design Verilated with '--no-sched-zero-delay', but #0 delay executed at runtime");
    }
    return (__VinactExecute);
}

void Vtest_blitter___024root___trigger_clear__act(VlUnpacked<QData/*63:0*/, 1> &out) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vtest_blitter___024root___trigger_clear__act\n"); );
    // Locals
    IData/*31:0*/ n;
    // Body
    n = 0U;
    do {
        out[n] = 0ULL;
        n = ((IData)(1U) + n);
    } while ((1U > n));
}

bool Vtest_blitter___024root___eval_phase__nba(Vtest_blitter___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vtest_blitter___024root___eval_phase__nba\n"); );
    Vtest_blitter__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
    // Locals
    CData/*0:0*/ __VnbaExecute;
    // Body
    __VnbaExecute = Vtest_blitter___024root___trigger_anySet__act(vlSelfRef.__VnbaTriggered);
    if (__VnbaExecute) {
        Vtest_blitter___024root___eval_nba(vlSelf);
        Vtest_blitter___024root___trigger_clear__act(vlSelfRef.__VnbaTriggered);
    }
    return (__VnbaExecute);
}

void Vtest_blitter___024root___eval(Vtest_blitter___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vtest_blitter___024root___eval\n"); );
    Vtest_blitter__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
    // Locals
    IData/*31:0*/ __VnbaIterCount;
    // Body
    __VnbaIterCount = 0U;
    do {
        if (VL_UNLIKELY(((0x00000064U < __VnbaIterCount)))) {
#ifdef VL_DEBUG
            Vtest_blitter___024root___dump_triggers__act(vlSelfRef.__VnbaTriggered, "nba"s);
#endif
            VL_FATAL_MT("test_blitter.sv", 6, "", "DIDNOTCONVERGE: NBA region did not converge after '--converge-limit' of 100 tries");
        }
        __VnbaIterCount = ((IData)(1U) + __VnbaIterCount);
        vlSelfRef.__VinactIterCount = 0U;
        do {
            if (VL_UNLIKELY(((0x00000064U < vlSelfRef.__VinactIterCount)))) {
                VL_FATAL_MT("test_blitter.sv", 6, "", "DIDNOTCONVERGE: Inactive region did not converge after '--converge-limit' of 100 tries");
            }
            vlSelfRef.__VinactIterCount = ((IData)(1U) 
                                           + vlSelfRef.__VinactIterCount);
            vlSelfRef.__VactIterCount = 0U;
            do {
                if (VL_UNLIKELY(((0x00000064U < vlSelfRef.__VactIterCount)))) {
#ifdef VL_DEBUG
                    Vtest_blitter___024root___dump_triggers__act(vlSelfRef.__VactTriggered, "act"s);
#endif
                    VL_FATAL_MT("test_blitter.sv", 6, "", "DIDNOTCONVERGE: Active region did not converge after '--converge-limit' of 100 tries");
                }
                vlSelfRef.__VactIterCount = ((IData)(1U) 
                                             + vlSelfRef.__VactIterCount);
                vlSelfRef.__VactPhaseResult = Vtest_blitter___024root___eval_phase__act(vlSelf);
            } while (vlSelfRef.__VactPhaseResult);
            vlSelfRef.__VinactPhaseResult = Vtest_blitter___024root___eval_phase__inact(vlSelf);
        } while (vlSelfRef.__VinactPhaseResult);
        vlSelfRef.__VnbaPhaseResult = Vtest_blitter___024root___eval_phase__nba(vlSelf);
    } while (vlSelfRef.__VnbaPhaseResult);
}

void Vtest_blitter___024root____VbeforeTrig_h603fc354__0(Vtest_blitter___024root* vlSelf, const char* __VeventDescription) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vtest_blitter___024root____VbeforeTrig_h603fc354__0\n"); );
    Vtest_blitter__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
    // Locals
    VlUnpacked<QData/*63:0*/, 1> __VTmp;
    // Body
    __VTmp[0U] = (QData)((IData)(((IData)(vlSelfRef.test_blitter__DOT__clk) 
                                  & (~ (IData)(vlSelfRef.__Vtrigprevexpr___TOP__test_blitter__DOT__clk__0)))));
    vlSelfRef.__Vtrigprevexpr___TOP__test_blitter__DOT__clk__0 
        = vlSelfRef.test_blitter__DOT__clk;
    if ((1ULL & __VTmp[0U])) {
        vlSelfRef.__VtrigSched_h603fc354__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h603fc354__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h603fc354__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h603fc354__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h603fc354__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h603fc354__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h603fc354__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h603fc354__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h603fc354__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h603fc354__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h603fc354__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h603fc354__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h603fc354__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h603fc354__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h603fc354__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h603fc354__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h603fc354__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h603fc354__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h603fc354__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h603fc354__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h603fc354__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h603fc354__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h603fc354__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h603fc354__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h603fc354__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h603fc354__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h603fc354__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h603fc354__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h603fc354__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h603fc354__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h603fc354__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h603fc354__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h603fc354__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h603fc354__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h603fc354__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h603fc354__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h603fc354__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h603fc354__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h603fc354__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h603fc354__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h603fc354__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h603fc354__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h603fc354__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h603fc354__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h603fc354__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h603fc354__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h603fc354__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h603fc354__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h603fc354__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h603fc354__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h603fc354__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h603fc354__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h603fc354__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h603fc354__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h603fc354__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h603fc354__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h603fc354__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h603fc354__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h603fc354__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h603fc354__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h603fc354__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h603fc354__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h603fc354__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h603fc354__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h603fc354__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h603fc354__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h603fc354__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h603fc354__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h603fc354__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h603fc354__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h603fc354__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h603fc354__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h603fc354__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h603fc354__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h603fc354__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h603fc354__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h603fc354__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h603fc354__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h603fc354__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h603fc354__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h603fc354__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h603fc354__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h603fc354__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h603fc354__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h603fc354__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h603fc354__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h603fc354__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h603fc354__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h603fc354__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h603fc354__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h603fc354__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h603fc354__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h603fc354__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h603fc354__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h603fc354__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h603fc354__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h603fc354__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h603fc354__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h603fc354__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h603fc354__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h603fc354__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h603fc354__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h603fc354__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h603fc354__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h603fc354__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h603fc354__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h603fc354__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h603fc354__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h603fc354__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h603fc354__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h603fc354__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h603fc354__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h603fc354__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h603fc354__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h603fc354__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h603fc354__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h603fc354__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h603fc354__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h603fc354__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h603fc354__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h603fc354__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h603fc354__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h603fc354__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h603fc354__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h603fc354__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h603fc354__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h603fc354__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h603fc354__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h603fc354__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h603fc354__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h603fc354__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h603fc354__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h603fc354__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h603fc354__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h603fc354__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h603fc354__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h603fc354__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h603fc354__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h603fc354__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h603fc354__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h603fc354__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h603fc354__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h603fc354__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h603fc354__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h603fc354__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h603fc354__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h603fc354__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h603fc354__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h603fc354__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h603fc354__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h603fc354__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h603fc354__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h603fc354__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h603fc354__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h603fc354__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h603fc354__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h603fc354__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h603fc354__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h603fc354__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h603fc354__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h603fc354__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h603fc354__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h603fc354__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h603fc354__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h603fc354__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h603fc354__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h603fc354__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h603fc354__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h603fc354__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h603fc354__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h603fc354__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h603fc354__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h603fc354__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h603fc354__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h603fc354__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h603fc354__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h603fc354__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h603fc354__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h603fc354__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h603fc354__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h603fc354__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h603fc354__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h603fc354__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h603fc354__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h603fc354__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h603fc354__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h603fc354__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h603fc354__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h603fc354__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h603fc354__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h603fc354__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h603fc354__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h603fc354__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h603fc354__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h603fc354__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h603fc354__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h603fc354__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h603fc354__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h603fc354__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h603fc354__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h603fc354__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h603fc354__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h603fc354__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h603fc354__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h603fc354__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h603fc354__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h603fc354__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h603fc354__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h603fc354__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h603fc354__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h603fc354__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h603fc354__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h603fc354__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h603fc354__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h603fc354__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h603fc354__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h603fc354__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h603fc354__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h603fc354__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h603fc354__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h603fc354__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h603fc354__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h603fc354__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h603fc354__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h603fc354__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h603fc354__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h603fc354__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h603fc354__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h603fc354__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h603fc354__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h603fc354__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h603fc354__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h603fc354__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h603fc354__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h603fc354__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h603fc354__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h603fc354__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h603fc354__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h603fc354__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h603fc354__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h603fc354__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h603fc354__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h603fc354__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h603fc354__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h603fc354__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h603fc354__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h603fc354__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h603fc354__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h603fc354__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h603fc354__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h603fc354__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h603fc354__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h603fc354__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h603fc354__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h603fc354__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h603fc354__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h603fc354__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h603fc354__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h603fc354__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h603fc354__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h603fc354__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h603fc354__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h603fc354__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h603fc354__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h603fc354__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h603fc354__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h603fc354__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h603fc354__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h603fc354__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h603fc354__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h603fc354__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h603fc354__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h603fc354__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h603fc354__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h603fc354__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h603fc354__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h603fc354__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h603fc354__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h603fc354__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h603fc354__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h603fc354__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h603fc354__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h603fc354__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h603fc354__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h603fc354__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h603fc354__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h603fc354__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h603fc354__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h603fc354__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h603fc354__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h603fc354__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h603fc354__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h603fc354__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h603fc354__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h603fc354__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h603fc354__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h603fc354__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h603fc354__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h603fc354__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h603fc354__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h603fc354__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h603fc354__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h603fc354__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h603fc354__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h603fc354__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h603fc354__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h603fc354__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h603fc354__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h603fc354__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h603fc354__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h603fc354__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h603fc354__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h603fc354__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h603fc354__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h603fc354__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h603fc354__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h603fc354__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h603fc354__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h603fc354__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h603fc354__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h603fc354__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h603fc354__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h603fc354__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h603fc354__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h603fc354__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h603fc354__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h603fc354__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h603fc354__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h603fc354__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h603fc354__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h603fc354__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h603fc354__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h603fc354__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h603fc354__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h603fc354__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h603fc354__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h603fc354__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h603fc354__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h603fc354__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h603fc354__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h603fc354__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h603fc354__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h603fc354__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h603fc354__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h603fc354__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h603fc354__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h603fc354__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h603fc354__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h603fc354__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h603fc354__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h603fc354__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h603fc354__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h603fc354__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h603fc354__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h603fc354__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h603fc354__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h603fc354__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h603fc354__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h603fc354__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h603fc354__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h603fc354__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h603fc354__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h603fc354__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h603fc354__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h603fc354__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h603fc354__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h603fc354__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h603fc354__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h603fc354__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h603fc354__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h603fc354__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h603fc354__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h603fc354__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h603fc354__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h603fc354__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h603fc354__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h603fc354__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h603fc354__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h603fc354__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h603fc354__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h603fc354__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h603fc354__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h603fc354__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h603fc354__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h603fc354__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h603fc354__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h603fc354__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h603fc354__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h603fc354__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h603fc354__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h603fc354__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h603fc354__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h603fc354__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h603fc354__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h603fc354__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h603fc354__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h603fc354__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h603fc354__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h603fc354__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h603fc354__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h603fc354__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h603fc354__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h603fc354__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h603fc354__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h603fc354__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h603fc354__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h603fc354__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h603fc354__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h603fc354__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h603fc354__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h603fc354__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h603fc354__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h603fc354__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h603fc354__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h603fc354__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h603fc354__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h603fc354__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h603fc354__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h603fc354__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h603fc354__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h603fc354__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h603fc354__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h603fc354__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h603fc354__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h603fc354__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h603fc354__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h603fc354__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h603fc354__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h603fc354__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h603fc354__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h603fc354__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h603fc354__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h603fc354__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h603fc354__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h603fc354__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h603fc354__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h603fc354__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h603fc354__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h603fc354__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h603fc354__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h603fc354__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h603fc354__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h603fc354__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h603fc354__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h603fc354__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h603fc354__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h603fc354__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h603fc354__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h603fc354__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h603fc354__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h603fc354__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h603fc354__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h603fc354__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h603fc354__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h603fc354__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h603fc354__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h603fc354__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h603fc354__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h603fc354__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h603fc354__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h603fc354__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h603fc354__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h603fc354__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h603fc354__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h603fc354__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h603fc354__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h603fc354__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h603fc354__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h603fc354__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h603fc354__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h603fc354__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h603fc354__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h603fc354__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h603fc354__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h603fc354__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h603fc354__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h603fc354__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h603fc354__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h603fc354__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h603fc354__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h603fc354__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h603fc354__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h603fc354__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h603fc354__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h603fc354__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h603fc354__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h603fc354__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h603fc354__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h603fc354__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h603fc354__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h603fc354__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h603fc354__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h603fc354__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h603fc354__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h603fc354__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h603fc354__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h603fc354__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h603fc354__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h603fc354__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h603fc354__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h603fc354__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h603fc354__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h603fc354__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h603fc354__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h603fc354__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h603fc354__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h603fc354__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h603fc354__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h603fc354__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h603fc354__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h603fc354__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h603fc354__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h603fc354__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h603fc354__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h603fc354__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h603fc354__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h603fc354__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h603fc354__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h603fc354__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h603fc354__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h603fc354__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h603fc354__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h603fc354__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h603fc354__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h603fc354__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h603fc354__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h603fc354__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h603fc354__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h603fc354__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h603fc354__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h603fc354__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h603fc354__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h603fc354__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h603fc354__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h603fc354__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h603fc354__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h603fc354__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h603fc354__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h603fc354__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h603fc354__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h603fc354__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h603fc354__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h603fc354__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h603fc354__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h603fc354__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h603fc354__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h603fc354__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h603fc354__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h603fc354__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h603fc354__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h603fc354__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h603fc354__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h603fc354__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h603fc354__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h603fc354__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h603fc354__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h603fc354__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h603fc354__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h603fc354__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h603fc354__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h603fc354__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h603fc354__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h603fc354__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h603fc354__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h603fc354__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h603fc354__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h603fc354__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h603fc354__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h603fc354__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h603fc354__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h603fc354__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h603fc354__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h603fc354__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h603fc354__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h603fc354__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h603fc354__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h603fc354__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h603fc354__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h603fc354__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h603fc354__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h603fc354__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h603fc354__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h603fc354__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h603fc354__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h603fc354__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h603fc354__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h603fc354__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h603fc354__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h603fc354__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h603fc354__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h603fc354__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h603fc354__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h603fc354__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h603fc354__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h603fc354__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h603fc354__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h603fc354__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h603fc354__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h603fc354__0.ready(__VeventDescription);
    }
    vlSelfRef.__VactTriggeredAcc[0U] = (vlSelfRef.__VactTriggeredAcc[0U] 
                                        | __VTmp[0U]);
}

#ifdef VL_DEBUG
void Vtest_blitter___024root___eval_debug_assertions(Vtest_blitter___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vtest_blitter___024root___eval_debug_assertions\n"); );
    Vtest_blitter__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
}
#endif  // VL_DEBUG
