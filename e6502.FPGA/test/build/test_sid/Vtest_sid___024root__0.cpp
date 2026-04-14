// Verilated -*- C++ -*-
// DESCRIPTION: Verilator output: Design implementation internals
// See Vtest_sid.h for the primary calling header

#include "Vtest_sid__pch.h"

VL_ATTR_COLD void Vtest_sid___024root___eval_initial__TOP(Vtest_sid___024root* vlSelf);
VlCoroutine Vtest_sid___024root___eval_initial__TOP__Vtiming__0(Vtest_sid___024root* vlSelf);
VlCoroutine Vtest_sid___024root___eval_initial__TOP__Vtiming__1(Vtest_sid___024root* vlSelf);

void Vtest_sid___024root___eval_initial(Vtest_sid___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vtest_sid___024root___eval_initial\n"); );
    Vtest_sid__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
    // Body
    Vtest_sid___024root___eval_initial__TOP(vlSelf);
    Vtest_sid___024root___eval_initial__TOP__Vtiming__0(vlSelf);
    Vtest_sid___024root___eval_initial__TOP__Vtiming__1(vlSelf);
}

void Vtest_sid___024root____VbeforeTrig_hb52908e9__0(Vtest_sid___024root* vlSelf, const char* __VeventDescription);

VlCoroutine Vtest_sid___024root___eval_initial__TOP__Vtiming__0(Vtest_sid___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vtest_sid___024root___eval_initial__TOP__Vtiming__0\n"); );
    Vtest_sid__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
    // Locals
    IData/*31:0*/ test_sid__DOT__unnamedblk1_1__DOT____Vrepeat0;
    test_sid__DOT__unnamedblk1_1__DOT____Vrepeat0 = 0;
    IData/*31:0*/ test_sid__DOT__unnamedblk1_2__DOT____Vrepeat1;
    test_sid__DOT__unnamedblk1_2__DOT____Vrepeat1 = 0;
    IData/*31:0*/ test_sid__DOT__unnamedblk1_3__DOT____Vrepeat2;
    test_sid__DOT__unnamedblk1_3__DOT____Vrepeat2 = 0;
    IData/*31:0*/ test_sid__DOT__unnamedblk1_4__DOT____Vrepeat3;
    test_sid__DOT__unnamedblk1_4__DOT____Vrepeat3 = 0;
    IData/*31:0*/ test_sid__DOT__unnamedblk1_5__DOT____Vrepeat4;
    test_sid__DOT__unnamedblk1_5__DOT____Vrepeat4 = 0;
    CData/*4:0*/ __Vtask_test_sid__DOT__write_reg__0__a;
    __Vtask_test_sid__DOT__write_reg__0__a = 0;
    CData/*7:0*/ __Vtask_test_sid__DOT__write_reg__0__d;
    __Vtask_test_sid__DOT__write_reg__0__d = 0;
    CData/*4:0*/ __Vtask_test_sid__DOT__write_reg__1__a;
    __Vtask_test_sid__DOT__write_reg__1__a = 0;
    CData/*7:0*/ __Vtask_test_sid__DOT__write_reg__1__d;
    __Vtask_test_sid__DOT__write_reg__1__d = 0;
    IData/*31:0*/ __Vtask_test_sid__DOT__check__2__actual;
    __Vtask_test_sid__DOT__check__2__actual = 0;
    IData/*31:0*/ __Vtask_test_sid__DOT__check__2__expected;
    __Vtask_test_sid__DOT__check__2__expected = 0;
    IData/*31:0*/ __Vtask_test_sid__DOT__check__3__actual;
    __Vtask_test_sid__DOT__check__3__actual = 0;
    IData/*31:0*/ __Vtask_test_sid__DOT__check__3__expected;
    __Vtask_test_sid__DOT__check__3__expected = 0;
    CData/*4:0*/ __Vtask_test_sid__DOT__write_reg__4__a;
    __Vtask_test_sid__DOT__write_reg__4__a = 0;
    CData/*7:0*/ __Vtask_test_sid__DOT__write_reg__4__d;
    __Vtask_test_sid__DOT__write_reg__4__d = 0;
    IData/*31:0*/ __Vtask_test_sid__DOT__check__5__actual;
    __Vtask_test_sid__DOT__check__5__actual = 0;
    IData/*31:0*/ __Vtask_test_sid__DOT__check__5__expected;
    __Vtask_test_sid__DOT__check__5__expected = 0;
    IData/*31:0*/ __Vtask_test_sid__DOT__check__6__actual;
    __Vtask_test_sid__DOT__check__6__actual = 0;
    IData/*31:0*/ __Vtask_test_sid__DOT__check__6__expected;
    __Vtask_test_sid__DOT__check__6__expected = 0;
    IData/*31:0*/ __Vtask_test_sid__DOT__check__7__actual;
    __Vtask_test_sid__DOT__check__7__actual = 0;
    IData/*31:0*/ __Vtask_test_sid__DOT__check__7__expected;
    __Vtask_test_sid__DOT__check__7__expected = 0;
    CData/*4:0*/ __Vtask_test_sid__DOT__write_reg__8__a;
    __Vtask_test_sid__DOT__write_reg__8__a = 0;
    CData/*7:0*/ __Vtask_test_sid__DOT__write_reg__8__d;
    __Vtask_test_sid__DOT__write_reg__8__d = 0;
    CData/*4:0*/ __Vtask_test_sid__DOT__write_reg__9__a;
    __Vtask_test_sid__DOT__write_reg__9__a = 0;
    CData/*7:0*/ __Vtask_test_sid__DOT__write_reg__9__d;
    __Vtask_test_sid__DOT__write_reg__9__d = 0;
    CData/*4:0*/ __Vtask_test_sid__DOT__write_reg__10__a;
    __Vtask_test_sid__DOT__write_reg__10__a = 0;
    CData/*7:0*/ __Vtask_test_sid__DOT__write_reg__10__d;
    __Vtask_test_sid__DOT__write_reg__10__d = 0;
    CData/*4:0*/ __Vtask_test_sid__DOT__write_reg__11__a;
    __Vtask_test_sid__DOT__write_reg__11__a = 0;
    CData/*7:0*/ __Vtask_test_sid__DOT__write_reg__11__d;
    __Vtask_test_sid__DOT__write_reg__11__d = 0;
    CData/*4:0*/ __Vtask_test_sid__DOT__write_reg__12__a;
    __Vtask_test_sid__DOT__write_reg__12__a = 0;
    CData/*7:0*/ __Vtask_test_sid__DOT__write_reg__12__d;
    __Vtask_test_sid__DOT__write_reg__12__d = 0;
    CData/*4:0*/ __Vtask_test_sid__DOT__write_reg__13__a;
    __Vtask_test_sid__DOT__write_reg__13__a = 0;
    CData/*7:0*/ __Vtask_test_sid__DOT__write_reg__13__d;
    __Vtask_test_sid__DOT__write_reg__13__d = 0;
    IData/*31:0*/ __Vtask_test_sid__DOT__check__14__actual;
    __Vtask_test_sid__DOT__check__14__actual = 0;
    IData/*31:0*/ __Vtask_test_sid__DOT__check__14__expected;
    __Vtask_test_sid__DOT__check__14__expected = 0;
    IData/*31:0*/ __Vtask_test_sid__DOT__check__15__actual;
    __Vtask_test_sid__DOT__check__15__actual = 0;
    IData/*31:0*/ __Vtask_test_sid__DOT__check__15__expected;
    __Vtask_test_sid__DOT__check__15__expected = 0;
    CData/*4:0*/ __Vtask_test_sid__DOT__write_reg__16__a;
    __Vtask_test_sid__DOT__write_reg__16__a = 0;
    CData/*7:0*/ __Vtask_test_sid__DOT__write_reg__16__d;
    __Vtask_test_sid__DOT__write_reg__16__d = 0;
    // Body
    vlSelfRef.test_sid__DOT__rst = 1U;
    vlSelfRef.test_sid__DOT__cs = 0U;
    vlSelfRef.test_sid__DOT__we = 0U;
    vlSelfRef.test_sid__DOT__addr = 0U;
    vlSelfRef.test_sid__DOT__din = 0U;
    vlSelfRef.test_sid__DOT__mode = 0U;
    test_sid__DOT__unnamedblk1_1__DOT____Vrepeat0 = 0x0000000aU;
    while (VL_LTS_III(32, 0U, test_sid__DOT__unnamedblk1_1__DOT____Vrepeat0)) {
        Vtest_sid___024root____VbeforeTrig_hb52908e9__0(vlSelf, 
                                                        "@(posedge test_sid.clk)");
        co_await vlSelfRef.__VtrigSched_hb52908e9__0.trigger(0U, 
                                                             nullptr, 
                                                             "@(posedge test_sid.clk)", 
                                                             "test_sid.sv", 
                                                             45);
        test_sid__DOT__unnamedblk1_1__DOT____Vrepeat0 
            = (test_sid__DOT__unnamedblk1_1__DOT____Vrepeat0 
               - (IData)(1U));
    }
    vlSelfRef.test_sid__DOT__rst = 0U;
    test_sid__DOT__unnamedblk1_2__DOT____Vrepeat1 = 5U;
    while (VL_LTS_III(32, 0U, test_sid__DOT__unnamedblk1_2__DOT____Vrepeat1)) {
        Vtest_sid___024root____VbeforeTrig_hb52908e9__0(vlSelf, 
                                                        "@(posedge test_sid.clk)");
        co_await vlSelfRef.__VtrigSched_hb52908e9__0.trigger(0U, 
                                                             nullptr, 
                                                             "@(posedge test_sid.clk)", 
                                                             "test_sid.sv", 
                                                             47);
        test_sid__DOT__unnamedblk1_2__DOT____Vrepeat1 
            = (test_sid__DOT__unnamedblk1_2__DOT____Vrepeat1 
               - (IData)(1U));
    }
    __Vtask_test_sid__DOT__write_reg__0__d = 0xabU;
    __Vtask_test_sid__DOT__write_reg__0__a = 0U;
    Vtest_sid___024root____VbeforeTrig_hb52908e9__0(vlSelf, 
                                                    "@(posedge test_sid.clk)");
    co_await vlSelfRef.__VtrigSched_hb52908e9__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge test_sid.clk)", 
                                                         "test_sid.sv", 
                                                         28);
    vlSelfRef.test_sid__DOT__cs = 1U;
    vlSelfRef.test_sid__DOT__we = 1U;
    vlSelfRef.test_sid__DOT__addr = __Vtask_test_sid__DOT__write_reg__0__a;
    vlSelfRef.test_sid__DOT__din = __Vtask_test_sid__DOT__write_reg__0__d;
    Vtest_sid___024root____VbeforeTrig_hb52908e9__0(vlSelf, 
                                                    "@(posedge test_sid.clk)");
    co_await vlSelfRef.__VtrigSched_hb52908e9__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge test_sid.clk)", 
                                                         "test_sid.sv", 
                                                         30);
    vlSelfRef.test_sid__DOT__cs = 0U;
    vlSelfRef.test_sid__DOT__we = 0U;
    __Vtask_test_sid__DOT__write_reg__1__d = 0xcdU;
    __Vtask_test_sid__DOT__write_reg__1__a = 1U;
    Vtest_sid___024root____VbeforeTrig_hb52908e9__0(vlSelf, 
                                                    "@(posedge test_sid.clk)");
    co_await vlSelfRef.__VtrigSched_hb52908e9__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge test_sid.clk)", 
                                                         "test_sid.sv", 
                                                         28);
    vlSelfRef.test_sid__DOT__cs = 1U;
    vlSelfRef.test_sid__DOT__we = 1U;
    vlSelfRef.test_sid__DOT__addr = __Vtask_test_sid__DOT__write_reg__1__a;
    vlSelfRef.test_sid__DOT__din = __Vtask_test_sid__DOT__write_reg__1__d;
    Vtest_sid___024root____VbeforeTrig_hb52908e9__0(vlSelf, 
                                                    "@(posedge test_sid.clk)");
    co_await vlSelfRef.__VtrigSched_hb52908e9__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge test_sid.clk)", 
                                                         "test_sid.sv", 
                                                         30);
    vlSelfRef.test_sid__DOT__cs = 0U;
    vlSelfRef.test_sid__DOT__we = 0U;
    vlSelfRef.test_sid__DOT__cs = 1U;
    vlSelfRef.test_sid__DOT__we = 0U;
    vlSelfRef.test_sid__DOT__addr = 1U;
    Vtest_sid___024root____VbeforeTrig_hb52908e9__0(vlSelf, 
                                                    "@(posedge test_sid.clk)");
    co_await vlSelfRef.__VtrigSched_hb52908e9__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge test_sid.clk)", 
                                                         "test_sid.sv", 
                                                         54);
    __Vtask_test_sid__DOT__check__2__expected = 0x000000cdU;
    __Vtask_test_sid__DOT__check__2__actual = vlSelfRef.test_sid__DOT__dout;
    vlSelfRef.__Vtask_test_sid__DOT__check__2__name = "Freq hi read-back"s;
    if (VL_LIKELY(((__Vtask_test_sid__DOT__check__2__actual 
                    == __Vtask_test_sid__DOT__check__2__expected)))) {
        vlSelfRef.test_sid__DOT__pass = ((IData)(1U) 
                                         + vlSelfRef.test_sid__DOT__pass);
    } else {
        VL_WRITEF_NX("FAIL: %@ \342\200\224 got %0d, expected %0d\n",0,
                     -1,&(vlSelfRef.__Vtask_test_sid__DOT__check__2__name),
                     32,__Vtask_test_sid__DOT__check__2__actual,
                     32,__Vtask_test_sid__DOT__check__2__expected);
        vlSelfRef.test_sid__DOT__fail = ((IData)(1U) 
                                         + vlSelfRef.test_sid__DOT__fail);
    }
    vlSelfRef.test_sid__DOT__cs = 0U;
    vlSelfRef.test_sid__DOT__cs = 1U;
    vlSelfRef.test_sid__DOT__we = 0U;
    vlSelfRef.test_sid__DOT__addr = 0x1dU;
    Vtest_sid___024root____VbeforeTrig_hb52908e9__0(vlSelf, 
                                                    "@(posedge test_sid.clk)");
    co_await vlSelfRef.__VtrigSched_hb52908e9__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge test_sid.clk)", 
                                                         "test_sid.sv", 
                                                         60);
    __Vtask_test_sid__DOT__check__3__expected = 0x0000000fU;
    __Vtask_test_sid__DOT__check__3__actual = vlSelfRef.test_sid__DOT__dout;
    vlSelfRef.__Vtask_test_sid__DOT__check__3__name = "Voice 1 vol default"s;
    if (VL_LIKELY(((__Vtask_test_sid__DOT__check__3__actual 
                    == __Vtask_test_sid__DOT__check__3__expected)))) {
        vlSelfRef.test_sid__DOT__pass = ((IData)(1U) 
                                         + vlSelfRef.test_sid__DOT__pass);
    } else {
        VL_WRITEF_NX("FAIL: %@ \342\200\224 got %0d, expected %0d\n",0,
                     -1,&(vlSelfRef.__Vtask_test_sid__DOT__check__3__name),
                     32,__Vtask_test_sid__DOT__check__3__actual,
                     32,__Vtask_test_sid__DOT__check__3__expected);
        vlSelfRef.test_sid__DOT__fail = ((IData)(1U) 
                                         + vlSelfRef.test_sid__DOT__fail);
    }
    vlSelfRef.test_sid__DOT__cs = 0U;
    __Vtask_test_sid__DOT__write_reg__4__d = 7U;
    __Vtask_test_sid__DOT__write_reg__4__a = 0x1dU;
    Vtest_sid___024root____VbeforeTrig_hb52908e9__0(vlSelf, 
                                                    "@(posedge test_sid.clk)");
    co_await vlSelfRef.__VtrigSched_hb52908e9__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge test_sid.clk)", 
                                                         "test_sid.sv", 
                                                         28);
    vlSelfRef.test_sid__DOT__cs = 1U;
    vlSelfRef.test_sid__DOT__we = 1U;
    vlSelfRef.test_sid__DOT__addr = __Vtask_test_sid__DOT__write_reg__4__a;
    vlSelfRef.test_sid__DOT__din = __Vtask_test_sid__DOT__write_reg__4__d;
    Vtest_sid___024root____VbeforeTrig_hb52908e9__0(vlSelf, 
                                                    "@(posedge test_sid.clk)");
    co_await vlSelfRef.__VtrigSched_hb52908e9__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge test_sid.clk)", 
                                                         "test_sid.sv", 
                                                         30);
    vlSelfRef.test_sid__DOT__cs = 0U;
    vlSelfRef.test_sid__DOT__we = 0U;
    vlSelfRef.test_sid__DOT__cs = 1U;
    vlSelfRef.test_sid__DOT__we = 0U;
    vlSelfRef.test_sid__DOT__addr = 0x1dU;
    Vtest_sid___024root____VbeforeTrig_hb52908e9__0(vlSelf, 
                                                    "@(posedge test_sid.clk)");
    co_await vlSelfRef.__VtrigSched_hb52908e9__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge test_sid.clk)", 
                                                         "test_sid.sv", 
                                                         67);
    __Vtask_test_sid__DOT__check__5__expected = 7U;
    __Vtask_test_sid__DOT__check__5__actual = vlSelfRef.test_sid__DOT__dout;
    vlSelfRef.__Vtask_test_sid__DOT__check__5__name = "Voice 1 vol written"s;
    if (VL_LIKELY(((__Vtask_test_sid__DOT__check__5__actual 
                    == __Vtask_test_sid__DOT__check__5__expected)))) {
        vlSelfRef.test_sid__DOT__pass = ((IData)(1U) 
                                         + vlSelfRef.test_sid__DOT__pass);
    } else {
        VL_WRITEF_NX("FAIL: %@ \342\200\224 got %0d, expected %0d\n",0,
                     -1,&(vlSelfRef.__Vtask_test_sid__DOT__check__5__name),
                     32,__Vtask_test_sid__DOT__check__5__actual,
                     32,__Vtask_test_sid__DOT__check__5__expected);
        vlSelfRef.test_sid__DOT__fail = ((IData)(1U) 
                                         + vlSelfRef.test_sid__DOT__fail);
    }
    vlSelfRef.test_sid__DOT__cs = 0U;
    vlSelfRef.test_sid__DOT__cs = 1U;
    vlSelfRef.test_sid__DOT__we = 0U;
    vlSelfRef.test_sid__DOT__addr = 0x1bU;
    Vtest_sid___024root____VbeforeTrig_hb52908e9__0(vlSelf, 
                                                    "@(posedge test_sid.clk)");
    co_await vlSelfRef.__VtrigSched_hb52908e9__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge test_sid.clk)", 
                                                         "test_sid.sv", 
                                                         73);
    __Vtask_test_sid__DOT__check__6__expected = 1U;
    __Vtask_test_sid__DOT__check__6__actual = 1U;
    vlSelfRef.__Vtask_test_sid__DOT__check__6__name = "OSC3 readable"s;
    if (VL_LIKELY(((__Vtask_test_sid__DOT__check__6__actual 
                    == __Vtask_test_sid__DOT__check__6__expected)))) {
        vlSelfRef.test_sid__DOT__pass = ((IData)(1U) 
                                         + vlSelfRef.test_sid__DOT__pass);
    } else {
        VL_WRITEF_NX("FAIL: %@ \342\200\224 got %0d, expected %0d\n",0,
                     -1,&(vlSelfRef.__Vtask_test_sid__DOT__check__6__name),
                     32,__Vtask_test_sid__DOT__check__6__actual,
                     32,__Vtask_test_sid__DOT__check__6__expected);
        vlSelfRef.test_sid__DOT__fail = ((IData)(1U) 
                                         + vlSelfRef.test_sid__DOT__fail);
    }
    vlSelfRef.test_sid__DOT__cs = 0U;
    vlSelfRef.test_sid__DOT__cs = 1U;
    vlSelfRef.test_sid__DOT__we = 0U;
    vlSelfRef.test_sid__DOT__addr = 0x1cU;
    Vtest_sid___024root____VbeforeTrig_hb52908e9__0(vlSelf, 
                                                    "@(posedge test_sid.clk)");
    co_await vlSelfRef.__VtrigSched_hb52908e9__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge test_sid.clk)", 
                                                         "test_sid.sv", 
                                                         79);
    __Vtask_test_sid__DOT__check__7__expected = 1U;
    __Vtask_test_sid__DOT__check__7__actual = 1U;
    vlSelfRef.__Vtask_test_sid__DOT__check__7__name = "ENV3 readable"s;
    if (VL_LIKELY(((__Vtask_test_sid__DOT__check__7__actual 
                    == __Vtask_test_sid__DOT__check__7__expected)))) {
        vlSelfRef.test_sid__DOT__pass = ((IData)(1U) 
                                         + vlSelfRef.test_sid__DOT__pass);
    } else {
        VL_WRITEF_NX("FAIL: %@ \342\200\224 got %0d, expected %0d\n",0,
                     -1,&(vlSelfRef.__Vtask_test_sid__DOT__check__7__name),
                     32,__Vtask_test_sid__DOT__check__7__actual,
                     32,__Vtask_test_sid__DOT__check__7__expected);
        vlSelfRef.test_sid__DOT__fail = ((IData)(1U) 
                                         + vlSelfRef.test_sid__DOT__fail);
    }
    vlSelfRef.test_sid__DOT__cs = 0U;
    vlSelfRef.test_sid__DOT__mode = 0U;
    __Vtask_test_sid__DOT__write_reg__8__d = 0U;
    __Vtask_test_sid__DOT__write_reg__8__a = 0U;
    Vtest_sid___024root____VbeforeTrig_hb52908e9__0(vlSelf, 
                                                    "@(posedge test_sid.clk)");
    co_await vlSelfRef.__VtrigSched_hb52908e9__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge test_sid.clk)", 
                                                         "test_sid.sv", 
                                                         28);
    vlSelfRef.test_sid__DOT__cs = 1U;
    vlSelfRef.test_sid__DOT__we = 1U;
    vlSelfRef.test_sid__DOT__addr = __Vtask_test_sid__DOT__write_reg__8__a;
    vlSelfRef.test_sid__DOT__din = __Vtask_test_sid__DOT__write_reg__8__d;
    Vtest_sid___024root____VbeforeTrig_hb52908e9__0(vlSelf, 
                                                    "@(posedge test_sid.clk)");
    co_await vlSelfRef.__VtrigSched_hb52908e9__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge test_sid.clk)", 
                                                         "test_sid.sv", 
                                                         30);
    vlSelfRef.test_sid__DOT__cs = 0U;
    vlSelfRef.test_sid__DOT__we = 0U;
    __Vtask_test_sid__DOT__write_reg__9__d = 0x10U;
    __Vtask_test_sid__DOT__write_reg__9__a = 1U;
    Vtest_sid___024root____VbeforeTrig_hb52908e9__0(vlSelf, 
                                                    "@(posedge test_sid.clk)");
    co_await vlSelfRef.__VtrigSched_hb52908e9__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge test_sid.clk)", 
                                                         "test_sid.sv", 
                                                         28);
    vlSelfRef.test_sid__DOT__cs = 1U;
    vlSelfRef.test_sid__DOT__we = 1U;
    vlSelfRef.test_sid__DOT__addr = __Vtask_test_sid__DOT__write_reg__9__a;
    vlSelfRef.test_sid__DOT__din = __Vtask_test_sid__DOT__write_reg__9__d;
    Vtest_sid___024root____VbeforeTrig_hb52908e9__0(vlSelf, 
                                                    "@(posedge test_sid.clk)");
    co_await vlSelfRef.__VtrigSched_hb52908e9__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge test_sid.clk)", 
                                                         "test_sid.sv", 
                                                         30);
    vlSelfRef.test_sid__DOT__cs = 0U;
    vlSelfRef.test_sid__DOT__we = 0U;
    __Vtask_test_sid__DOT__write_reg__10__d = 0U;
    __Vtask_test_sid__DOT__write_reg__10__a = 5U;
    Vtest_sid___024root____VbeforeTrig_hb52908e9__0(vlSelf, 
                                                    "@(posedge test_sid.clk)");
    co_await vlSelfRef.__VtrigSched_hb52908e9__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge test_sid.clk)", 
                                                         "test_sid.sv", 
                                                         28);
    vlSelfRef.test_sid__DOT__cs = 1U;
    vlSelfRef.test_sid__DOT__we = 1U;
    vlSelfRef.test_sid__DOT__addr = __Vtask_test_sid__DOT__write_reg__10__a;
    vlSelfRef.test_sid__DOT__din = __Vtask_test_sid__DOT__write_reg__10__d;
    Vtest_sid___024root____VbeforeTrig_hb52908e9__0(vlSelf, 
                                                    "@(posedge test_sid.clk)");
    co_await vlSelfRef.__VtrigSched_hb52908e9__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge test_sid.clk)", 
                                                         "test_sid.sv", 
                                                         30);
    vlSelfRef.test_sid__DOT__cs = 0U;
    vlSelfRef.test_sid__DOT__we = 0U;
    __Vtask_test_sid__DOT__write_reg__11__d = 0xf0U;
    __Vtask_test_sid__DOT__write_reg__11__a = 6U;
    Vtest_sid___024root____VbeforeTrig_hb52908e9__0(vlSelf, 
                                                    "@(posedge test_sid.clk)");
    co_await vlSelfRef.__VtrigSched_hb52908e9__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge test_sid.clk)", 
                                                         "test_sid.sv", 
                                                         28);
    vlSelfRef.test_sid__DOT__cs = 1U;
    vlSelfRef.test_sid__DOT__we = 1U;
    vlSelfRef.test_sid__DOT__addr = __Vtask_test_sid__DOT__write_reg__11__a;
    vlSelfRef.test_sid__DOT__din = __Vtask_test_sid__DOT__write_reg__11__d;
    Vtest_sid___024root____VbeforeTrig_hb52908e9__0(vlSelf, 
                                                    "@(posedge test_sid.clk)");
    co_await vlSelfRef.__VtrigSched_hb52908e9__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge test_sid.clk)", 
                                                         "test_sid.sv", 
                                                         30);
    vlSelfRef.test_sid__DOT__cs = 0U;
    vlSelfRef.test_sid__DOT__we = 0U;
    __Vtask_test_sid__DOT__write_reg__12__d = 0x0fU;
    __Vtask_test_sid__DOT__write_reg__12__a = 0x18U;
    Vtest_sid___024root____VbeforeTrig_hb52908e9__0(vlSelf, 
                                                    "@(posedge test_sid.clk)");
    co_await vlSelfRef.__VtrigSched_hb52908e9__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge test_sid.clk)", 
                                                         "test_sid.sv", 
                                                         28);
    vlSelfRef.test_sid__DOT__cs = 1U;
    vlSelfRef.test_sid__DOT__we = 1U;
    vlSelfRef.test_sid__DOT__addr = __Vtask_test_sid__DOT__write_reg__12__a;
    vlSelfRef.test_sid__DOT__din = __Vtask_test_sid__DOT__write_reg__12__d;
    Vtest_sid___024root____VbeforeTrig_hb52908e9__0(vlSelf, 
                                                    "@(posedge test_sid.clk)");
    co_await vlSelfRef.__VtrigSched_hb52908e9__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge test_sid.clk)", 
                                                         "test_sid.sv", 
                                                         30);
    vlSelfRef.test_sid__DOT__cs = 0U;
    vlSelfRef.test_sid__DOT__we = 0U;
    __Vtask_test_sid__DOT__write_reg__13__d = 0x21U;
    __Vtask_test_sid__DOT__write_reg__13__a = 4U;
    Vtest_sid___024root____VbeforeTrig_hb52908e9__0(vlSelf, 
                                                    "@(posedge test_sid.clk)");
    co_await vlSelfRef.__VtrigSched_hb52908e9__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge test_sid.clk)", 
                                                         "test_sid.sv", 
                                                         28);
    vlSelfRef.test_sid__DOT__cs = 1U;
    vlSelfRef.test_sid__DOT__we = 1U;
    vlSelfRef.test_sid__DOT__addr = __Vtask_test_sid__DOT__write_reg__13__a;
    vlSelfRef.test_sid__DOT__din = __Vtask_test_sid__DOT__write_reg__13__d;
    Vtest_sid___024root____VbeforeTrig_hb52908e9__0(vlSelf, 
                                                    "@(posedge test_sid.clk)");
    co_await vlSelfRef.__VtrigSched_hb52908e9__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge test_sid.clk)", 
                                                         "test_sid.sv", 
                                                         30);
    vlSelfRef.test_sid__DOT__cs = 0U;
    vlSelfRef.test_sid__DOT__we = 0U;
    test_sid__DOT__unnamedblk1_3__DOT____Vrepeat2 = 0x0000c350U;
    while (VL_LTS_III(32, 0U, test_sid__DOT__unnamedblk1_3__DOT____Vrepeat2)) {
        Vtest_sid___024root____VbeforeTrig_hb52908e9__0(vlSelf, 
                                                        "@(posedge test_sid.clk)");
        co_await vlSelfRef.__VtrigSched_hb52908e9__0.trigger(0U, 
                                                             nullptr, 
                                                             "@(posedge test_sid.clk)", 
                                                             "test_sid.sv", 
                                                             93);
        test_sid__DOT__unnamedblk1_3__DOT____Vrepeat2 
            = (test_sid__DOT__unnamedblk1_3__DOT____Vrepeat2 
               - (IData)(1U));
    }
    __Vtask_test_sid__DOT__check__14__expected = 1U;
    __Vtask_test_sid__DOT__check__14__actual = (0U 
                                                != 
                                                VL_EXTENDS_II(32,18, vlSelfRef.test_sid__DOT__audio_out));
    vlSelfRef.__Vtask_test_sid__DOT__check__14__name = "Audio non-zero (6581)"s;
    if (VL_LIKELY(((__Vtask_test_sid__DOT__check__14__actual 
                    == __Vtask_test_sid__DOT__check__14__expected)))) {
        vlSelfRef.test_sid__DOT__pass = ((IData)(1U) 
                                         + vlSelfRef.test_sid__DOT__pass);
    } else {
        VL_WRITEF_NX("FAIL: %@ \342\200\224 got %0d, expected %0d\n",0,
                     -1,&(vlSelfRef.__Vtask_test_sid__DOT__check__14__name),
                     32,__Vtask_test_sid__DOT__check__14__actual,
                     32,__Vtask_test_sid__DOT__check__14__expected);
        vlSelfRef.test_sid__DOT__fail = ((IData)(1U) 
                                         + vlSelfRef.test_sid__DOT__fail);
    }
    vlSelfRef.test_sid__DOT__mode = 1U;
    test_sid__DOT__unnamedblk1_4__DOT____Vrepeat3 = 0x0000c350U;
    while (VL_LTS_III(32, 0U, test_sid__DOT__unnamedblk1_4__DOT____Vrepeat3)) {
        Vtest_sid___024root____VbeforeTrig_hb52908e9__0(vlSelf, 
                                                        "@(posedge test_sid.clk)");
        co_await vlSelfRef.__VtrigSched_hb52908e9__0.trigger(0U, 
                                                             nullptr, 
                                                             "@(posedge test_sid.clk)", 
                                                             "test_sid.sv", 
                                                             100);
        test_sid__DOT__unnamedblk1_4__DOT____Vrepeat3 
            = (test_sid__DOT__unnamedblk1_4__DOT____Vrepeat3 
               - (IData)(1U));
    }
    __Vtask_test_sid__DOT__check__15__expected = 1U;
    __Vtask_test_sid__DOT__check__15__actual = (0U 
                                                != 
                                                VL_EXTENDS_II(32,18, vlSelfRef.test_sid__DOT__audio_out));
    vlSelfRef.__Vtask_test_sid__DOT__check__15__name = "Audio non-zero (8580)"s;
    if (VL_LIKELY(((__Vtask_test_sid__DOT__check__15__actual 
                    == __Vtask_test_sid__DOT__check__15__expected)))) {
        vlSelfRef.test_sid__DOT__pass = ((IData)(1U) 
                                         + vlSelfRef.test_sid__DOT__pass);
    } else {
        VL_WRITEF_NX("FAIL: %@ \342\200\224 got %0d, expected %0d\n",0,
                     -1,&(vlSelfRef.__Vtask_test_sid__DOT__check__15__name),
                     32,__Vtask_test_sid__DOT__check__15__actual,
                     32,__Vtask_test_sid__DOT__check__15__expected);
        vlSelfRef.test_sid__DOT__fail = ((IData)(1U) 
                                         + vlSelfRef.test_sid__DOT__fail);
    }
    __Vtask_test_sid__DOT__write_reg__16__d = 0x20U;
    __Vtask_test_sid__DOT__write_reg__16__a = 4U;
    Vtest_sid___024root____VbeforeTrig_hb52908e9__0(vlSelf, 
                                                    "@(posedge test_sid.clk)");
    co_await vlSelfRef.__VtrigSched_hb52908e9__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge test_sid.clk)", 
                                                         "test_sid.sv", 
                                                         28);
    vlSelfRef.test_sid__DOT__cs = 1U;
    vlSelfRef.test_sid__DOT__we = 1U;
    vlSelfRef.test_sid__DOT__addr = __Vtask_test_sid__DOT__write_reg__16__a;
    vlSelfRef.test_sid__DOT__din = __Vtask_test_sid__DOT__write_reg__16__d;
    Vtest_sid___024root____VbeforeTrig_hb52908e9__0(vlSelf, 
                                                    "@(posedge test_sid.clk)");
    co_await vlSelfRef.__VtrigSched_hb52908e9__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge test_sid.clk)", 
                                                         "test_sid.sv", 
                                                         30);
    vlSelfRef.test_sid__DOT__cs = 0U;
    vlSelfRef.test_sid__DOT__we = 0U;
    test_sid__DOT__unnamedblk1_5__DOT____Vrepeat4 = 0x00030d40U;
    while (VL_LTS_III(32, 0U, test_sid__DOT__unnamedblk1_5__DOT____Vrepeat4)) {
        Vtest_sid___024root____VbeforeTrig_hb52908e9__0(vlSelf, 
                                                        "@(posedge test_sid.clk)");
        co_await vlSelfRef.__VtrigSched_hb52908e9__0.trigger(0U, 
                                                             nullptr, 
                                                             "@(posedge test_sid.clk)", 
                                                             "test_sid.sv", 
                                                             105);
        test_sid__DOT__unnamedblk1_5__DOT____Vrepeat4 
            = (test_sid__DOT__unnamedblk1_5__DOT____Vrepeat4 
               - (IData)(1U));
    }
    VL_WRITEF_NX("\n",0);
    if ((0U == vlSelfRef.test_sid__DOT__fail)) {
        VL_WRITEF_NX("SID: All %0d tests passed\n",0,
                     32,vlSelfRef.test_sid__DOT__pass);
    } else {
        VL_WRITEF_NX("SID: %0d passed, %0d FAILED\n",0,
                     32,vlSelfRef.test_sid__DOT__pass,
                     32,vlSelfRef.test_sid__DOT__fail);
    }
    VL_FINISH_MT("test_sid.sv", 113, "");
    co_return;
}

VlCoroutine Vtest_sid___024root___eval_initial__TOP__Vtiming__1(Vtest_sid___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vtest_sid___024root___eval_initial__TOP__Vtiming__1\n"); );
    Vtest_sid__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
    // Body
    while (VL_LIKELY(!vlSymsp->_vm_contextp__->gotFinish())) {
        co_await vlSelfRef.__VdlySched.delay(0x0000000000004e20ULL, 
                                             nullptr, 
                                             "test_sid.sv", 
                                             6);
        vlSelfRef.test_sid__DOT__clk = (1U & (~ (IData)(vlSelfRef.test_sid__DOT__clk)));
    }
    co_return;
}

void Vtest_sid___024root___eval_triggers_vec__act(Vtest_sid___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vtest_sid___024root___eval_triggers_vec__act\n"); );
    Vtest_sid__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
    // Body
    vlSelfRef.__VactTriggered[0U] = (QData)((IData)(
                                                    (((((vlSelfRef.__VdlySched.awaitingCurrentTime() 
                                                         << 3U) 
                                                        | (((IData)(vlSelfRef.test_sid__DOT__clk) 
                                                            & (~ (IData)(vlSelfRef.__Vtrigprevexpr___TOP__test_sid__DOT__clk__0))) 
                                                           << 2U)) 
                                                       | ((vlSelfRef.__Vtrigprevexpr___TOP__test_sid__DOT__uut__DOT__v3__DOT__envelope_dac__DOT__bitsum__0.neq(vlSelfRef.test_sid__DOT__uut__DOT__v3__DOT__envelope_dac__DOT__bitsum) 
                                                           << 1U) 
                                                          | vlSelfRef.__Vtrigprevexpr___TOP__test_sid__DOT__uut__DOT__v3__DOT__waveform_dac__DOT__bitsum__0.neq(vlSelfRef.test_sid__DOT__uut__DOT__v3__DOT__waveform_dac__DOT__bitsum))) 
                                                      << 4U) 
                                                     | (((vlSelfRef.__Vtrigprevexpr___TOP__test_sid__DOT__uut__DOT__v2__DOT__envelope_dac__DOT__bitsum__0.neq(vlSelfRef.test_sid__DOT__uut__DOT__v2__DOT__envelope_dac__DOT__bitsum) 
                                                          << 3U) 
                                                         | (vlSelfRef.__Vtrigprevexpr___TOP__test_sid__DOT__uut__DOT__v2__DOT__waveform_dac__DOT__bitsum__0.neq(vlSelfRef.test_sid__DOT__uut__DOT__v2__DOT__waveform_dac__DOT__bitsum) 
                                                            << 2U)) 
                                                        | ((vlSelfRef.__Vtrigprevexpr___TOP__test_sid__DOT__uut__DOT__v1__DOT__envelope_dac__DOT__bitsum__0.neq(vlSelfRef.test_sid__DOT__uut__DOT__v1__DOT__envelope_dac__DOT__bitsum) 
                                                            << 1U) 
                                                           | vlSelfRef.__Vtrigprevexpr___TOP__test_sid__DOT__uut__DOT__v1__DOT__waveform_dac__DOT__bitsum__0.neq(vlSelfRef.test_sid__DOT__uut__DOT__v1__DOT__waveform_dac__DOT__bitsum))))));
    vlSelfRef.__Vtrigprevexpr___TOP__test_sid__DOT__uut__DOT__v1__DOT__waveform_dac__DOT__bitsum__0.assign(vlSelfRef.test_sid__DOT__uut__DOT__v1__DOT__waveform_dac__DOT__bitsum);
    vlSelfRef.__Vtrigprevexpr___TOP__test_sid__DOT__uut__DOT__v1__DOT__envelope_dac__DOT__bitsum__0.assign(vlSelfRef.test_sid__DOT__uut__DOT__v1__DOT__envelope_dac__DOT__bitsum);
    vlSelfRef.__Vtrigprevexpr___TOP__test_sid__DOT__uut__DOT__v2__DOT__waveform_dac__DOT__bitsum__0.assign(vlSelfRef.test_sid__DOT__uut__DOT__v2__DOT__waveform_dac__DOT__bitsum);
    vlSelfRef.__Vtrigprevexpr___TOP__test_sid__DOT__uut__DOT__v2__DOT__envelope_dac__DOT__bitsum__0.assign(vlSelfRef.test_sid__DOT__uut__DOT__v2__DOT__envelope_dac__DOT__bitsum);
    vlSelfRef.__Vtrigprevexpr___TOP__test_sid__DOT__uut__DOT__v3__DOT__waveform_dac__DOT__bitsum__0.assign(vlSelfRef.test_sid__DOT__uut__DOT__v3__DOT__waveform_dac__DOT__bitsum);
    vlSelfRef.__Vtrigprevexpr___TOP__test_sid__DOT__uut__DOT__v3__DOT__envelope_dac__DOT__bitsum__0.assign(vlSelfRef.test_sid__DOT__uut__DOT__v3__DOT__envelope_dac__DOT__bitsum);
    vlSelfRef.__Vtrigprevexpr___TOP__test_sid__DOT__clk__0 
        = vlSelfRef.test_sid__DOT__clk;
}

bool Vtest_sid___024root___trigger_anySet__act(const VlUnpacked<QData/*63:0*/, 1> &in) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vtest_sid___024root___trigger_anySet__act\n"); );
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

void Vtest_sid___024root___act_sequent__TOP__0(Vtest_sid___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vtest_sid___024root___act_sequent__TOP__0\n"); );
    Vtest_sid__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
    // Body
    vlSelfRef.test_sid__DOT__dout = ((0x00000010U & (IData)(vlSelfRef.test_sid__DOT__addr))
                                      ? ((8U & (IData)(vlSelfRef.test_sid__DOT__addr))
                                          ? ((4U & (IData)(vlSelfRef.test_sid__DOT__addr))
                                              ? ((2U 
                                                  & (IData)(vlSelfRef.test_sid__DOT__addr))
                                                  ? 
                                                 ((1U 
                                                   & (IData)(vlSelfRef.test_sid__DOT__addr))
                                                   ? vlSelfRef.test_sid__DOT__uut__DOT__voice_vol[2U]
                                                   : vlSelfRef.test_sid__DOT__uut__DOT__voice_vol[1U])
                                                  : 
                                                 ((1U 
                                                   & (IData)(vlSelfRef.test_sid__DOT__addr))
                                                   ? vlSelfRef.test_sid__DOT__uut__DOT__voice_vol[0U]
                                                   : (IData)(vlSelfRef.test_sid__DOT__uut__DOT__v3__DOT__envelope)))
                                              : ((2U 
                                                  & (IData)(vlSelfRef.test_sid__DOT__addr))
                                                  ? 
                                                 ((1U 
                                                   & (IData)(vlSelfRef.test_sid__DOT__addr))
                                                   ? (IData)(vlSelfRef.test_sid__DOT__uut__DOT__v3__DOT__wave_out)
                                                   : 0U)
                                                  : 
                                                 ((1U 
                                                   & (IData)(vlSelfRef.test_sid__DOT__addr))
                                                   ? 0U
                                                   : (IData)(vlSelfRef.test_sid__DOT__uut__DOT__last_written))))
                                          : (IData)(vlSelfRef.test_sid__DOT__uut__DOT__last_written))
                                      : (IData)(vlSelfRef.test_sid__DOT__uut__DOT__last_written));
}

void Vtest_sid___024root___act_sequent__TOP__1(Vtest_sid___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vtest_sid___024root___act_sequent__TOP__1\n"); );
    Vtest_sid__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
    // Body
    vlSelfRef.test_sid__DOT__uut__DOT__v1__DOT__envelope_dac__DOT__bitsum[1U] 
        = (0x00000fffU & (vlSelfRef.test_sid__DOT__uut__DOT__v1__DOT__envelope_dac__DOT__bitsum[0U] 
                          + ((2U & (IData)(vlSelfRef.test_sid__DOT__uut__DOT__v1__DOT__envelope))
                              ? vlSelfRef.test_sid__DOT__uut__DOT__v1__DOT__envelope_dac__DOT__bitval[1U]
                              : 0U)));
    vlSelfRef.test_sid__DOT__uut__DOT__v1__DOT__envelope_dac__DOT__bitsum[2U] 
        = (0x00000fffU & (vlSelfRef.test_sid__DOT__uut__DOT__v1__DOT__envelope_dac__DOT__bitsum[1U] 
                          + ((4U & (IData)(vlSelfRef.test_sid__DOT__uut__DOT__v1__DOT__envelope))
                              ? vlSelfRef.test_sid__DOT__uut__DOT__v1__DOT__envelope_dac__DOT__bitval[2U]
                              : 0U)));
    vlSelfRef.test_sid__DOT__uut__DOT__v1__DOT__envelope_dac__DOT__bitsum[3U] 
        = (0x00000fffU & (vlSelfRef.test_sid__DOT__uut__DOT__v1__DOT__envelope_dac__DOT__bitsum[2U] 
                          + ((8U & (IData)(vlSelfRef.test_sid__DOT__uut__DOT__v1__DOT__envelope))
                              ? vlSelfRef.test_sid__DOT__uut__DOT__v1__DOT__envelope_dac__DOT__bitval[3U]
                              : 0U)));
    vlSelfRef.test_sid__DOT__uut__DOT__v1__DOT__envelope_dac__DOT__bitsum[4U] 
        = (0x00000fffU & (vlSelfRef.test_sid__DOT__uut__DOT__v1__DOT__envelope_dac__DOT__bitsum[3U] 
                          + ((0x00000010U & (IData)(vlSelfRef.test_sid__DOT__uut__DOT__v1__DOT__envelope))
                              ? vlSelfRef.test_sid__DOT__uut__DOT__v1__DOT__envelope_dac__DOT__bitval[4U]
                              : 0U)));
    vlSelfRef.test_sid__DOT__uut__DOT__v1__DOT__envelope_dac__DOT__bitsum[5U] 
        = (0x00000fffU & (vlSelfRef.test_sid__DOT__uut__DOT__v1__DOT__envelope_dac__DOT__bitsum[4U] 
                          + ((0x00000020U & (IData)(vlSelfRef.test_sid__DOT__uut__DOT__v1__DOT__envelope))
                              ? vlSelfRef.test_sid__DOT__uut__DOT__v1__DOT__envelope_dac__DOT__bitval[5U]
                              : 0U)));
    vlSelfRef.test_sid__DOT__uut__DOT__v1__DOT__envelope_dac__DOT__bitsum[6U] 
        = (0x00000fffU & (vlSelfRef.test_sid__DOT__uut__DOT__v1__DOT__envelope_dac__DOT__bitsum[5U] 
                          + ((0x00000040U & (IData)(vlSelfRef.test_sid__DOT__uut__DOT__v1__DOT__envelope))
                              ? vlSelfRef.test_sid__DOT__uut__DOT__v1__DOT__envelope_dac__DOT__bitval[6U]
                              : 0U)));
    vlSelfRef.test_sid__DOT__uut__DOT__v1__DOT__envelope_dac__DOT__bitsum[7U] 
        = (0x00000fffU & (vlSelfRef.test_sid__DOT__uut__DOT__v1__DOT__envelope_dac__DOT__bitsum[6U] 
                          + ((0x00000080U & (IData)(vlSelfRef.test_sid__DOT__uut__DOT__v1__DOT__envelope))
                              ? vlSelfRef.test_sid__DOT__uut__DOT__v1__DOT__envelope_dac__DOT__bitval[7U]
                              : 0U)));
}

void Vtest_sid___024root___act_sequent__TOP__2(Vtest_sid___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vtest_sid___024root___act_sequent__TOP__2\n"); );
    Vtest_sid__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
    // Body
    vlSelfRef.test_sid__DOT__uut__DOT__v2__DOT__envelope_dac__DOT__bitsum[1U] 
        = (0x00000fffU & (vlSelfRef.test_sid__DOT__uut__DOT__v2__DOT__envelope_dac__DOT__bitsum[0U] 
                          + ((2U & (IData)(vlSelfRef.test_sid__DOT__uut__DOT__v2__DOT__envelope))
                              ? vlSelfRef.test_sid__DOT__uut__DOT__v2__DOT__envelope_dac__DOT__bitval[1U]
                              : 0U)));
    vlSelfRef.test_sid__DOT__uut__DOT__v2__DOT__envelope_dac__DOT__bitsum[2U] 
        = (0x00000fffU & (vlSelfRef.test_sid__DOT__uut__DOT__v2__DOT__envelope_dac__DOT__bitsum[1U] 
                          + ((4U & (IData)(vlSelfRef.test_sid__DOT__uut__DOT__v2__DOT__envelope))
                              ? vlSelfRef.test_sid__DOT__uut__DOT__v2__DOT__envelope_dac__DOT__bitval[2U]
                              : 0U)));
    vlSelfRef.test_sid__DOT__uut__DOT__v2__DOT__envelope_dac__DOT__bitsum[3U] 
        = (0x00000fffU & (vlSelfRef.test_sid__DOT__uut__DOT__v2__DOT__envelope_dac__DOT__bitsum[2U] 
                          + ((8U & (IData)(vlSelfRef.test_sid__DOT__uut__DOT__v2__DOT__envelope))
                              ? vlSelfRef.test_sid__DOT__uut__DOT__v2__DOT__envelope_dac__DOT__bitval[3U]
                              : 0U)));
    vlSelfRef.test_sid__DOT__uut__DOT__v2__DOT__envelope_dac__DOT__bitsum[4U] 
        = (0x00000fffU & (vlSelfRef.test_sid__DOT__uut__DOT__v2__DOT__envelope_dac__DOT__bitsum[3U] 
                          + ((0x00000010U & (IData)(vlSelfRef.test_sid__DOT__uut__DOT__v2__DOT__envelope))
                              ? vlSelfRef.test_sid__DOT__uut__DOT__v2__DOT__envelope_dac__DOT__bitval[4U]
                              : 0U)));
    vlSelfRef.test_sid__DOT__uut__DOT__v2__DOT__envelope_dac__DOT__bitsum[5U] 
        = (0x00000fffU & (vlSelfRef.test_sid__DOT__uut__DOT__v2__DOT__envelope_dac__DOT__bitsum[4U] 
                          + ((0x00000020U & (IData)(vlSelfRef.test_sid__DOT__uut__DOT__v2__DOT__envelope))
                              ? vlSelfRef.test_sid__DOT__uut__DOT__v2__DOT__envelope_dac__DOT__bitval[5U]
                              : 0U)));
    vlSelfRef.test_sid__DOT__uut__DOT__v2__DOT__envelope_dac__DOT__bitsum[6U] 
        = (0x00000fffU & (vlSelfRef.test_sid__DOT__uut__DOT__v2__DOT__envelope_dac__DOT__bitsum[5U] 
                          + ((0x00000040U & (IData)(vlSelfRef.test_sid__DOT__uut__DOT__v2__DOT__envelope))
                              ? vlSelfRef.test_sid__DOT__uut__DOT__v2__DOT__envelope_dac__DOT__bitval[6U]
                              : 0U)));
    vlSelfRef.test_sid__DOT__uut__DOT__v2__DOT__envelope_dac__DOT__bitsum[7U] 
        = (0x00000fffU & (vlSelfRef.test_sid__DOT__uut__DOT__v2__DOT__envelope_dac__DOT__bitsum[6U] 
                          + ((0x00000080U & (IData)(vlSelfRef.test_sid__DOT__uut__DOT__v2__DOT__envelope))
                              ? vlSelfRef.test_sid__DOT__uut__DOT__v2__DOT__envelope_dac__DOT__bitval[7U]
                              : 0U)));
}

void Vtest_sid___024root___act_sequent__TOP__3(Vtest_sid___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vtest_sid___024root___act_sequent__TOP__3\n"); );
    Vtest_sid__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
    // Body
    vlSelfRef.test_sid__DOT__uut__DOT__v3__DOT__envelope_dac__DOT__bitsum[1U] 
        = (0x00000fffU & (vlSelfRef.test_sid__DOT__uut__DOT__v3__DOT__envelope_dac__DOT__bitsum[0U] 
                          + ((2U & (IData)(vlSelfRef.test_sid__DOT__uut__DOT__v3__DOT__envelope))
                              ? vlSelfRef.test_sid__DOT__uut__DOT__v3__DOT__envelope_dac__DOT__bitval[1U]
                              : 0U)));
    vlSelfRef.test_sid__DOT__uut__DOT__v3__DOT__envelope_dac__DOT__bitsum[2U] 
        = (0x00000fffU & (vlSelfRef.test_sid__DOT__uut__DOT__v3__DOT__envelope_dac__DOT__bitsum[1U] 
                          + ((4U & (IData)(vlSelfRef.test_sid__DOT__uut__DOT__v3__DOT__envelope))
                              ? vlSelfRef.test_sid__DOT__uut__DOT__v3__DOT__envelope_dac__DOT__bitval[2U]
                              : 0U)));
    vlSelfRef.test_sid__DOT__uut__DOT__v3__DOT__envelope_dac__DOT__bitsum[3U] 
        = (0x00000fffU & (vlSelfRef.test_sid__DOT__uut__DOT__v3__DOT__envelope_dac__DOT__bitsum[2U] 
                          + ((8U & (IData)(vlSelfRef.test_sid__DOT__uut__DOT__v3__DOT__envelope))
                              ? vlSelfRef.test_sid__DOT__uut__DOT__v3__DOT__envelope_dac__DOT__bitval[3U]
                              : 0U)));
    vlSelfRef.test_sid__DOT__uut__DOT__v3__DOT__envelope_dac__DOT__bitsum[4U] 
        = (0x00000fffU & (vlSelfRef.test_sid__DOT__uut__DOT__v3__DOT__envelope_dac__DOT__bitsum[3U] 
                          + ((0x00000010U & (IData)(vlSelfRef.test_sid__DOT__uut__DOT__v3__DOT__envelope))
                              ? vlSelfRef.test_sid__DOT__uut__DOT__v3__DOT__envelope_dac__DOT__bitval[4U]
                              : 0U)));
    vlSelfRef.test_sid__DOT__uut__DOT__v3__DOT__envelope_dac__DOT__bitsum[5U] 
        = (0x00000fffU & (vlSelfRef.test_sid__DOT__uut__DOT__v3__DOT__envelope_dac__DOT__bitsum[4U] 
                          + ((0x00000020U & (IData)(vlSelfRef.test_sid__DOT__uut__DOT__v3__DOT__envelope))
                              ? vlSelfRef.test_sid__DOT__uut__DOT__v3__DOT__envelope_dac__DOT__bitval[5U]
                              : 0U)));
    vlSelfRef.test_sid__DOT__uut__DOT__v3__DOT__envelope_dac__DOT__bitsum[6U] 
        = (0x00000fffU & (vlSelfRef.test_sid__DOT__uut__DOT__v3__DOT__envelope_dac__DOT__bitsum[5U] 
                          + ((0x00000040U & (IData)(vlSelfRef.test_sid__DOT__uut__DOT__v3__DOT__envelope))
                              ? vlSelfRef.test_sid__DOT__uut__DOT__v3__DOT__envelope_dac__DOT__bitval[6U]
                              : 0U)));
    vlSelfRef.test_sid__DOT__uut__DOT__v3__DOT__envelope_dac__DOT__bitsum[7U] 
        = (0x00000fffU & (vlSelfRef.test_sid__DOT__uut__DOT__v3__DOT__envelope_dac__DOT__bitsum[6U] 
                          + ((0x00000080U & (IData)(vlSelfRef.test_sid__DOT__uut__DOT__v3__DOT__envelope))
                              ? vlSelfRef.test_sid__DOT__uut__DOT__v3__DOT__envelope_dac__DOT__bitval[7U]
                              : 0U)));
}

void Vtest_sid___024root___act_sequent__TOP__4(Vtest_sid___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vtest_sid___024root___act_sequent__TOP__4\n"); );
    Vtest_sid__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
    // Body
    vlSelfRef.test_sid__DOT__uut__DOT__v1__DOT__waveform_dac__DOT__bitsum[1U] 
        = (0x0000ffffU & (vlSelfRef.test_sid__DOT__uut__DOT__v1__DOT__waveform_dac__DOT__bitsum[0U] 
                          + ((2U & (IData)(vlSelfRef.test_sid__DOT__uut__DOT__v1__DOT__norm))
                              ? vlSelfRef.test_sid__DOT__uut__DOT__v1__DOT__waveform_dac__DOT__bitval[1U]
                              : 0U)));
    vlSelfRef.test_sid__DOT__uut__DOT__v1__DOT__waveform_dac__DOT__bitsum[2U] 
        = (0x0000ffffU & (vlSelfRef.test_sid__DOT__uut__DOT__v1__DOT__waveform_dac__DOT__bitsum[1U] 
                          + ((4U & (IData)(vlSelfRef.test_sid__DOT__uut__DOT__v1__DOT__norm))
                              ? vlSelfRef.test_sid__DOT__uut__DOT__v1__DOT__waveform_dac__DOT__bitval[2U]
                              : 0U)));
    vlSelfRef.test_sid__DOT__uut__DOT__v1__DOT__waveform_dac__DOT__bitsum[3U] 
        = (0x0000ffffU & (vlSelfRef.test_sid__DOT__uut__DOT__v1__DOT__waveform_dac__DOT__bitsum[2U] 
                          + ((8U & (IData)(vlSelfRef.test_sid__DOT__uut__DOT__v1__DOT__norm))
                              ? vlSelfRef.test_sid__DOT__uut__DOT__v1__DOT__waveform_dac__DOT__bitval[3U]
                              : 0U)));
    vlSelfRef.test_sid__DOT__uut__DOT__v1__DOT__waveform_dac__DOT__bitsum[4U] 
        = (0x0000ffffU & (vlSelfRef.test_sid__DOT__uut__DOT__v1__DOT__waveform_dac__DOT__bitsum[3U] 
                          + ((0x00000010U & (IData)(vlSelfRef.test_sid__DOT__uut__DOT__v1__DOT__norm))
                              ? vlSelfRef.test_sid__DOT__uut__DOT__v1__DOT__waveform_dac__DOT__bitval[4U]
                              : 0U)));
    vlSelfRef.test_sid__DOT__uut__DOT__v1__DOT__waveform_dac__DOT__bitsum[5U] 
        = (0x0000ffffU & (vlSelfRef.test_sid__DOT__uut__DOT__v1__DOT__waveform_dac__DOT__bitsum[4U] 
                          + ((0x00000020U & (IData)(vlSelfRef.test_sid__DOT__uut__DOT__v1__DOT__norm))
                              ? vlSelfRef.test_sid__DOT__uut__DOT__v1__DOT__waveform_dac__DOT__bitval[5U]
                              : 0U)));
    vlSelfRef.test_sid__DOT__uut__DOT__v1__DOT__waveform_dac__DOT__bitsum[6U] 
        = (0x0000ffffU & (vlSelfRef.test_sid__DOT__uut__DOT__v1__DOT__waveform_dac__DOT__bitsum[5U] 
                          + ((0x00000040U & (IData)(vlSelfRef.test_sid__DOT__uut__DOT__v1__DOT__norm))
                              ? vlSelfRef.test_sid__DOT__uut__DOT__v1__DOT__waveform_dac__DOT__bitval[6U]
                              : 0U)));
    vlSelfRef.test_sid__DOT__uut__DOT__v1__DOT__waveform_dac__DOT__bitsum[7U] 
        = (0x0000ffffU & (vlSelfRef.test_sid__DOT__uut__DOT__v1__DOT__waveform_dac__DOT__bitsum[6U] 
                          + ((0x00000080U & (IData)(vlSelfRef.test_sid__DOT__uut__DOT__v1__DOT__norm))
                              ? vlSelfRef.test_sid__DOT__uut__DOT__v1__DOT__waveform_dac__DOT__bitval[7U]
                              : 0U)));
    vlSelfRef.test_sid__DOT__uut__DOT__v1__DOT__waveform_dac__DOT__bitsum[8U] 
        = (0x0000ffffU & (vlSelfRef.test_sid__DOT__uut__DOT__v1__DOT__waveform_dac__DOT__bitsum[7U] 
                          + ((0x00000100U & (IData)(vlSelfRef.test_sid__DOT__uut__DOT__v1__DOT__norm))
                              ? vlSelfRef.test_sid__DOT__uut__DOT__v1__DOT__waveform_dac__DOT__bitval[8U]
                              : 0U)));
    vlSelfRef.test_sid__DOT__uut__DOT__v1__DOT__waveform_dac__DOT__bitsum[9U] 
        = (0x0000ffffU & (vlSelfRef.test_sid__DOT__uut__DOT__v1__DOT__waveform_dac__DOT__bitsum[8U] 
                          + ((0x00000200U & (IData)(vlSelfRef.test_sid__DOT__uut__DOT__v1__DOT__norm))
                              ? vlSelfRef.test_sid__DOT__uut__DOT__v1__DOT__waveform_dac__DOT__bitval[9U]
                              : 0U)));
    vlSelfRef.test_sid__DOT__uut__DOT__v1__DOT__waveform_dac__DOT__bitsum[10U] 
        = (0x0000ffffU & (vlSelfRef.test_sid__DOT__uut__DOT__v1__DOT__waveform_dac__DOT__bitsum[9U] 
                          + ((0x00000400U & (IData)(vlSelfRef.test_sid__DOT__uut__DOT__v1__DOT__norm))
                              ? vlSelfRef.test_sid__DOT__uut__DOT__v1__DOT__waveform_dac__DOT__bitval[10U]
                              : 0U)));
    vlSelfRef.test_sid__DOT__uut__DOT__v1__DOT__waveform_dac__DOT__bitsum[11U] 
        = (0x0000ffffU & (vlSelfRef.test_sid__DOT__uut__DOT__v1__DOT__waveform_dac__DOT__bitsum[10U] 
                          + ((0x00000800U & (IData)(vlSelfRef.test_sid__DOT__uut__DOT__v1__DOT__norm))
                              ? vlSelfRef.test_sid__DOT__uut__DOT__v1__DOT__waveform_dac__DOT__bitval[11U]
                              : 0U)));
}

void Vtest_sid___024root___act_sequent__TOP__5(Vtest_sid___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vtest_sid___024root___act_sequent__TOP__5\n"); );
    Vtest_sid__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
    // Body
    vlSelfRef.test_sid__DOT__uut__DOT__v2__DOT__waveform_dac__DOT__bitsum[1U] 
        = (0x0000ffffU & (vlSelfRef.test_sid__DOT__uut__DOT__v2__DOT__waveform_dac__DOT__bitsum[0U] 
                          + ((2U & (IData)(vlSelfRef.test_sid__DOT__uut__DOT__v2__DOT__norm))
                              ? vlSelfRef.test_sid__DOT__uut__DOT__v2__DOT__waveform_dac__DOT__bitval[1U]
                              : 0U)));
    vlSelfRef.test_sid__DOT__uut__DOT__v2__DOT__waveform_dac__DOT__bitsum[2U] 
        = (0x0000ffffU & (vlSelfRef.test_sid__DOT__uut__DOT__v2__DOT__waveform_dac__DOT__bitsum[1U] 
                          + ((4U & (IData)(vlSelfRef.test_sid__DOT__uut__DOT__v2__DOT__norm))
                              ? vlSelfRef.test_sid__DOT__uut__DOT__v2__DOT__waveform_dac__DOT__bitval[2U]
                              : 0U)));
    vlSelfRef.test_sid__DOT__uut__DOT__v2__DOT__waveform_dac__DOT__bitsum[3U] 
        = (0x0000ffffU & (vlSelfRef.test_sid__DOT__uut__DOT__v2__DOT__waveform_dac__DOT__bitsum[2U] 
                          + ((8U & (IData)(vlSelfRef.test_sid__DOT__uut__DOT__v2__DOT__norm))
                              ? vlSelfRef.test_sid__DOT__uut__DOT__v2__DOT__waveform_dac__DOT__bitval[3U]
                              : 0U)));
    vlSelfRef.test_sid__DOT__uut__DOT__v2__DOT__waveform_dac__DOT__bitsum[4U] 
        = (0x0000ffffU & (vlSelfRef.test_sid__DOT__uut__DOT__v2__DOT__waveform_dac__DOT__bitsum[3U] 
                          + ((0x00000010U & (IData)(vlSelfRef.test_sid__DOT__uut__DOT__v2__DOT__norm))
                              ? vlSelfRef.test_sid__DOT__uut__DOT__v2__DOT__waveform_dac__DOT__bitval[4U]
                              : 0U)));
    vlSelfRef.test_sid__DOT__uut__DOT__v2__DOT__waveform_dac__DOT__bitsum[5U] 
        = (0x0000ffffU & (vlSelfRef.test_sid__DOT__uut__DOT__v2__DOT__waveform_dac__DOT__bitsum[4U] 
                          + ((0x00000020U & (IData)(vlSelfRef.test_sid__DOT__uut__DOT__v2__DOT__norm))
                              ? vlSelfRef.test_sid__DOT__uut__DOT__v2__DOT__waveform_dac__DOT__bitval[5U]
                              : 0U)));
    vlSelfRef.test_sid__DOT__uut__DOT__v2__DOT__waveform_dac__DOT__bitsum[6U] 
        = (0x0000ffffU & (vlSelfRef.test_sid__DOT__uut__DOT__v2__DOT__waveform_dac__DOT__bitsum[5U] 
                          + ((0x00000040U & (IData)(vlSelfRef.test_sid__DOT__uut__DOT__v2__DOT__norm))
                              ? vlSelfRef.test_sid__DOT__uut__DOT__v2__DOT__waveform_dac__DOT__bitval[6U]
                              : 0U)));
    vlSelfRef.test_sid__DOT__uut__DOT__v2__DOT__waveform_dac__DOT__bitsum[7U] 
        = (0x0000ffffU & (vlSelfRef.test_sid__DOT__uut__DOT__v2__DOT__waveform_dac__DOT__bitsum[6U] 
                          + ((0x00000080U & (IData)(vlSelfRef.test_sid__DOT__uut__DOT__v2__DOT__norm))
                              ? vlSelfRef.test_sid__DOT__uut__DOT__v2__DOT__waveform_dac__DOT__bitval[7U]
                              : 0U)));
    vlSelfRef.test_sid__DOT__uut__DOT__v2__DOT__waveform_dac__DOT__bitsum[8U] 
        = (0x0000ffffU & (vlSelfRef.test_sid__DOT__uut__DOT__v2__DOT__waveform_dac__DOT__bitsum[7U] 
                          + ((0x00000100U & (IData)(vlSelfRef.test_sid__DOT__uut__DOT__v2__DOT__norm))
                              ? vlSelfRef.test_sid__DOT__uut__DOT__v2__DOT__waveform_dac__DOT__bitval[8U]
                              : 0U)));
    vlSelfRef.test_sid__DOT__uut__DOT__v2__DOT__waveform_dac__DOT__bitsum[9U] 
        = (0x0000ffffU & (vlSelfRef.test_sid__DOT__uut__DOT__v2__DOT__waveform_dac__DOT__bitsum[8U] 
                          + ((0x00000200U & (IData)(vlSelfRef.test_sid__DOT__uut__DOT__v2__DOT__norm))
                              ? vlSelfRef.test_sid__DOT__uut__DOT__v2__DOT__waveform_dac__DOT__bitval[9U]
                              : 0U)));
    vlSelfRef.test_sid__DOT__uut__DOT__v2__DOT__waveform_dac__DOT__bitsum[10U] 
        = (0x0000ffffU & (vlSelfRef.test_sid__DOT__uut__DOT__v2__DOT__waveform_dac__DOT__bitsum[9U] 
                          + ((0x00000400U & (IData)(vlSelfRef.test_sid__DOT__uut__DOT__v2__DOT__norm))
                              ? vlSelfRef.test_sid__DOT__uut__DOT__v2__DOT__waveform_dac__DOT__bitval[10U]
                              : 0U)));
    vlSelfRef.test_sid__DOT__uut__DOT__v2__DOT__waveform_dac__DOT__bitsum[11U] 
        = (0x0000ffffU & (vlSelfRef.test_sid__DOT__uut__DOT__v2__DOT__waveform_dac__DOT__bitsum[10U] 
                          + ((0x00000800U & (IData)(vlSelfRef.test_sid__DOT__uut__DOT__v2__DOT__norm))
                              ? vlSelfRef.test_sid__DOT__uut__DOT__v2__DOT__waveform_dac__DOT__bitval[11U]
                              : 0U)));
}

void Vtest_sid___024root___act_sequent__TOP__6(Vtest_sid___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vtest_sid___024root___act_sequent__TOP__6\n"); );
    Vtest_sid__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
    // Body
    vlSelfRef.test_sid__DOT__uut__DOT__v3__DOT__waveform_dac__DOT__bitsum[1U] 
        = (0x0000ffffU & (vlSelfRef.test_sid__DOT__uut__DOT__v3__DOT__waveform_dac__DOT__bitsum[0U] 
                          + ((2U & (IData)(vlSelfRef.test_sid__DOT__uut__DOT__v3__DOT__norm))
                              ? vlSelfRef.test_sid__DOT__uut__DOT__v3__DOT__waveform_dac__DOT__bitval[1U]
                              : 0U)));
    vlSelfRef.test_sid__DOT__uut__DOT__v3__DOT__waveform_dac__DOT__bitsum[2U] 
        = (0x0000ffffU & (vlSelfRef.test_sid__DOT__uut__DOT__v3__DOT__waveform_dac__DOT__bitsum[1U] 
                          + ((4U & (IData)(vlSelfRef.test_sid__DOT__uut__DOT__v3__DOT__norm))
                              ? vlSelfRef.test_sid__DOT__uut__DOT__v3__DOT__waveform_dac__DOT__bitval[2U]
                              : 0U)));
    vlSelfRef.test_sid__DOT__uut__DOT__v3__DOT__waveform_dac__DOT__bitsum[3U] 
        = (0x0000ffffU & (vlSelfRef.test_sid__DOT__uut__DOT__v3__DOT__waveform_dac__DOT__bitsum[2U] 
                          + ((8U & (IData)(vlSelfRef.test_sid__DOT__uut__DOT__v3__DOT__norm))
                              ? vlSelfRef.test_sid__DOT__uut__DOT__v3__DOT__waveform_dac__DOT__bitval[3U]
                              : 0U)));
    vlSelfRef.test_sid__DOT__uut__DOT__v3__DOT__waveform_dac__DOT__bitsum[4U] 
        = (0x0000ffffU & (vlSelfRef.test_sid__DOT__uut__DOT__v3__DOT__waveform_dac__DOT__bitsum[3U] 
                          + ((0x00000010U & (IData)(vlSelfRef.test_sid__DOT__uut__DOT__v3__DOT__norm))
                              ? vlSelfRef.test_sid__DOT__uut__DOT__v3__DOT__waveform_dac__DOT__bitval[4U]
                              : 0U)));
    vlSelfRef.test_sid__DOT__uut__DOT__v3__DOT__waveform_dac__DOT__bitsum[5U] 
        = (0x0000ffffU & (vlSelfRef.test_sid__DOT__uut__DOT__v3__DOT__waveform_dac__DOT__bitsum[4U] 
                          + ((0x00000020U & (IData)(vlSelfRef.test_sid__DOT__uut__DOT__v3__DOT__norm))
                              ? vlSelfRef.test_sid__DOT__uut__DOT__v3__DOT__waveform_dac__DOT__bitval[5U]
                              : 0U)));
    vlSelfRef.test_sid__DOT__uut__DOT__v3__DOT__waveform_dac__DOT__bitsum[6U] 
        = (0x0000ffffU & (vlSelfRef.test_sid__DOT__uut__DOT__v3__DOT__waveform_dac__DOT__bitsum[5U] 
                          + ((0x00000040U & (IData)(vlSelfRef.test_sid__DOT__uut__DOT__v3__DOT__norm))
                              ? vlSelfRef.test_sid__DOT__uut__DOT__v3__DOT__waveform_dac__DOT__bitval[6U]
                              : 0U)));
    vlSelfRef.test_sid__DOT__uut__DOT__v3__DOT__waveform_dac__DOT__bitsum[7U] 
        = (0x0000ffffU & (vlSelfRef.test_sid__DOT__uut__DOT__v3__DOT__waveform_dac__DOT__bitsum[6U] 
                          + ((0x00000080U & (IData)(vlSelfRef.test_sid__DOT__uut__DOT__v3__DOT__norm))
                              ? vlSelfRef.test_sid__DOT__uut__DOT__v3__DOT__waveform_dac__DOT__bitval[7U]
                              : 0U)));
    vlSelfRef.test_sid__DOT__uut__DOT__v3__DOT__waveform_dac__DOT__bitsum[8U] 
        = (0x0000ffffU & (vlSelfRef.test_sid__DOT__uut__DOT__v3__DOT__waveform_dac__DOT__bitsum[7U] 
                          + ((0x00000100U & (IData)(vlSelfRef.test_sid__DOT__uut__DOT__v3__DOT__norm))
                              ? vlSelfRef.test_sid__DOT__uut__DOT__v3__DOT__waveform_dac__DOT__bitval[8U]
                              : 0U)));
    vlSelfRef.test_sid__DOT__uut__DOT__v3__DOT__waveform_dac__DOT__bitsum[9U] 
        = (0x0000ffffU & (vlSelfRef.test_sid__DOT__uut__DOT__v3__DOT__waveform_dac__DOT__bitsum[8U] 
                          + ((0x00000200U & (IData)(vlSelfRef.test_sid__DOT__uut__DOT__v3__DOT__norm))
                              ? vlSelfRef.test_sid__DOT__uut__DOT__v3__DOT__waveform_dac__DOT__bitval[9U]
                              : 0U)));
    vlSelfRef.test_sid__DOT__uut__DOT__v3__DOT__waveform_dac__DOT__bitsum[10U] 
        = (0x0000ffffU & (vlSelfRef.test_sid__DOT__uut__DOT__v3__DOT__waveform_dac__DOT__bitsum[9U] 
                          + ((0x00000400U & (IData)(vlSelfRef.test_sid__DOT__uut__DOT__v3__DOT__norm))
                              ? vlSelfRef.test_sid__DOT__uut__DOT__v3__DOT__waveform_dac__DOT__bitval[10U]
                              : 0U)));
    vlSelfRef.test_sid__DOT__uut__DOT__v3__DOT__waveform_dac__DOT__bitsum[11U] 
        = (0x0000ffffU & (vlSelfRef.test_sid__DOT__uut__DOT__v3__DOT__waveform_dac__DOT__bitsum[10U] 
                          + ((0x00000800U & (IData)(vlSelfRef.test_sid__DOT__uut__DOT__v3__DOT__norm))
                              ? vlSelfRef.test_sid__DOT__uut__DOT__v3__DOT__waveform_dac__DOT__bitval[11U]
                              : 0U)));
}

void Vtest_sid___024root___eval_act(Vtest_sid___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vtest_sid___024root___eval_act\n"); );
    Vtest_sid__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
    // Body
    if ((0x0000000000000040ULL & vlSelfRef.__VactTriggered[0U])) {
        vlSelfRef.test_sid__DOT__dout = ((0x00000010U 
                                          & (IData)(vlSelfRef.test_sid__DOT__addr))
                                          ? ((8U & (IData)(vlSelfRef.test_sid__DOT__addr))
                                              ? ((4U 
                                                  & (IData)(vlSelfRef.test_sid__DOT__addr))
                                                  ? 
                                                 ((2U 
                                                   & (IData)(vlSelfRef.test_sid__DOT__addr))
                                                   ? 
                                                  ((1U 
                                                    & (IData)(vlSelfRef.test_sid__DOT__addr))
                                                    ? vlSelfRef.test_sid__DOT__uut__DOT__voice_vol[2U]
                                                    : vlSelfRef.test_sid__DOT__uut__DOT__voice_vol[1U])
                                                   : 
                                                  ((1U 
                                                    & (IData)(vlSelfRef.test_sid__DOT__addr))
                                                    ? vlSelfRef.test_sid__DOT__uut__DOT__voice_vol[0U]
                                                    : (IData)(vlSelfRef.test_sid__DOT__uut__DOT__v3__DOT__envelope)))
                                                  : 
                                                 ((2U 
                                                   & (IData)(vlSelfRef.test_sid__DOT__addr))
                                                   ? 
                                                  ((1U 
                                                    & (IData)(vlSelfRef.test_sid__DOT__addr))
                                                    ? (IData)(vlSelfRef.test_sid__DOT__uut__DOT__v3__DOT__wave_out)
                                                    : 0U)
                                                   : 
                                                  ((1U 
                                                    & (IData)(vlSelfRef.test_sid__DOT__addr))
                                                    ? 0U
                                                    : (IData)(vlSelfRef.test_sid__DOT__uut__DOT__last_written))))
                                              : (IData)(vlSelfRef.test_sid__DOT__uut__DOT__last_written))
                                          : (IData)(vlSelfRef.test_sid__DOT__uut__DOT__last_written));
    }
    if ((2ULL & vlSelfRef.__VactTriggered[0U])) {
        Vtest_sid___024root___act_sequent__TOP__1(vlSelf);
    }
    if ((8ULL & vlSelfRef.__VactTriggered[0U])) {
        Vtest_sid___024root___act_sequent__TOP__2(vlSelf);
    }
    if ((0x0000000000000020ULL & vlSelfRef.__VactTriggered[0U])) {
        Vtest_sid___024root___act_sequent__TOP__3(vlSelf);
    }
    if ((1ULL & vlSelfRef.__VactTriggered[0U])) {
        Vtest_sid___024root___act_sequent__TOP__4(vlSelf);
    }
    if ((4ULL & vlSelfRef.__VactTriggered[0U])) {
        Vtest_sid___024root___act_sequent__TOP__5(vlSelf);
    }
    if ((0x0000000000000010ULL & vlSelfRef.__VactTriggered[0U])) {
        Vtest_sid___024root___act_sequent__TOP__6(vlSelf);
    }
}

extern const VlUnpacked<CData/*7:0*/, 8192> Vtest_sid__ConstPool__TABLE_h5c0f3167_0;
extern const VlUnpacked<CData/*7:0*/, 8192> Vtest_sid__ConstPool__TABLE_h8a1656db_0;

void Vtest_sid___024root___nba_sequent__TOP__0(Vtest_sid___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vtest_sid___024root___nba_sequent__TOP__0\n"); );
    Vtest_sid__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
    // Locals
    IData/*31:0*/ test_sid__DOT__uut__DOT__flt__DOT__m;
    test_sid__DOT__uut__DOT__flt__DOT__m = 0;
    SData/*15:0*/ __Vfunc_test_sid__DOT__uut__DOT__flt__DOT__clamp__19__Vfuncout;
    __Vfunc_test_sid__DOT__uut__DOT__flt__DOT__clamp__19__Vfuncout = 0;
    IData/*16:0*/ __Vfunc_test_sid__DOT__uut__DOT__flt__DOT__clamp__19__x;
    __Vfunc_test_sid__DOT__uut__DOT__flt__DOT__clamp__19__x = 0;
    SData/*12:0*/ __Vtableidx1;
    __Vtableidx1 = 0;
    SData/*12:0*/ __Vtableidx2;
    __Vtableidx2 = 0;
    IData/*16:0*/ __VdfgRegularize_hebeb780c_0_4;
    __VdfgRegularize_hebeb780c_0_4 = 0;
    CData/*3:0*/ __Vdly__test_sid__DOT__uut__DOT__tbl_state;
    __Vdly__test_sid__DOT__uut__DOT__tbl_state = 0;
    SData/*11:0*/ __Vdly__test_sid__DOT__uut__DOT__f_acc_t;
    __Vdly__test_sid__DOT__uut__DOT__f_acc_t = 0;
    IData/*23:0*/ __Vdly__test_sid__DOT__uut__DOT__v1__DOT__oscillator;
    __Vdly__test_sid__DOT__uut__DOT__v1__DOT__oscillator = 0;
    IData/*22:0*/ __Vdly__test_sid__DOT__uut__DOT__v1__DOT__unnamedblk2__DOT__lfsr_noise;
    __Vdly__test_sid__DOT__uut__DOT__v1__DOT__unnamedblk2__DOT__lfsr_noise = 0;
    IData/*23:0*/ __Vdly__test_sid__DOT__uut__DOT__v1__DOT__unnamedblk2__DOT__noise_age;
    __Vdly__test_sid__DOT__uut__DOT__v1__DOT__unnamedblk2__DOT__noise_age = 0;
    IData/*23:0*/ __Vdly__test_sid__DOT__uut__DOT__v1__DOT__unnamedblk3__DOT__keep_cnt;
    __Vdly__test_sid__DOT__uut__DOT__v1__DOT__unnamedblk3__DOT__keep_cnt = 0;
    CData/*4:0*/ __Vdly__test_sid__DOT__uut__DOT__v1__DOT__adsr__DOT__unnamedblk1__DOT__exponential_counter_period;
    __Vdly__test_sid__DOT__uut__DOT__v1__DOT__adsr__DOT__unnamedblk1__DOT__exponential_counter_period = 0;
    CData/*1:0*/ __Vdly__test_sid__DOT__uut__DOT__v1__DOT__adsr__DOT__state;
    __Vdly__test_sid__DOT__uut__DOT__v1__DOT__adsr__DOT__state = 0;
    CData/*7:0*/ __Vdly__test_sid__DOT__uut__DOT__v1__DOT__envelope;
    __Vdly__test_sid__DOT__uut__DOT__v1__DOT__envelope = 0;
    CData/*0:0*/ __Vdly__test_sid__DOT__uut__DOT__v1__DOT__adsr__DOT__unnamedblk1__DOT__hold_zero;
    __Vdly__test_sid__DOT__uut__DOT__v1__DOT__adsr__DOT__unnamedblk1__DOT__hold_zero = 0;
    CData/*4:0*/ __Vdly__test_sid__DOT__uut__DOT__v1__DOT__adsr__DOT__unnamedblk1__DOT__exponential_counter;
    __Vdly__test_sid__DOT__uut__DOT__v1__DOT__adsr__DOT__unnamedblk1__DOT__exponential_counter = 0;
    SData/*14:0*/ __Vdly__test_sid__DOT__uut__DOT__v1__DOT__adsr__DOT__unnamedblk1__DOT__rate_counter;
    __Vdly__test_sid__DOT__uut__DOT__v1__DOT__adsr__DOT__unnamedblk1__DOT__rate_counter = 0;
    IData/*23:0*/ __Vdly__test_sid__DOT__uut__DOT__v2__DOT__oscillator;
    __Vdly__test_sid__DOT__uut__DOT__v2__DOT__oscillator = 0;
    IData/*22:0*/ __Vdly__test_sid__DOT__uut__DOT__v2__DOT__unnamedblk2__DOT__lfsr_noise;
    __Vdly__test_sid__DOT__uut__DOT__v2__DOT__unnamedblk2__DOT__lfsr_noise = 0;
    IData/*23:0*/ __Vdly__test_sid__DOT__uut__DOT__v2__DOT__unnamedblk2__DOT__noise_age;
    __Vdly__test_sid__DOT__uut__DOT__v2__DOT__unnamedblk2__DOT__noise_age = 0;
    IData/*23:0*/ __Vdly__test_sid__DOT__uut__DOT__v2__DOT__unnamedblk3__DOT__keep_cnt;
    __Vdly__test_sid__DOT__uut__DOT__v2__DOT__unnamedblk3__DOT__keep_cnt = 0;
    CData/*4:0*/ __Vdly__test_sid__DOT__uut__DOT__v2__DOT__adsr__DOT__unnamedblk1__DOT__exponential_counter_period;
    __Vdly__test_sid__DOT__uut__DOT__v2__DOT__adsr__DOT__unnamedblk1__DOT__exponential_counter_period = 0;
    CData/*1:0*/ __Vdly__test_sid__DOT__uut__DOT__v2__DOT__adsr__DOT__state;
    __Vdly__test_sid__DOT__uut__DOT__v2__DOT__adsr__DOT__state = 0;
    CData/*7:0*/ __Vdly__test_sid__DOT__uut__DOT__v2__DOT__envelope;
    __Vdly__test_sid__DOT__uut__DOT__v2__DOT__envelope = 0;
    CData/*0:0*/ __Vdly__test_sid__DOT__uut__DOT__v2__DOT__adsr__DOT__unnamedblk1__DOT__hold_zero;
    __Vdly__test_sid__DOT__uut__DOT__v2__DOT__adsr__DOT__unnamedblk1__DOT__hold_zero = 0;
    CData/*4:0*/ __Vdly__test_sid__DOT__uut__DOT__v2__DOT__adsr__DOT__unnamedblk1__DOT__exponential_counter;
    __Vdly__test_sid__DOT__uut__DOT__v2__DOT__adsr__DOT__unnamedblk1__DOT__exponential_counter = 0;
    SData/*14:0*/ __Vdly__test_sid__DOT__uut__DOT__v2__DOT__adsr__DOT__unnamedblk1__DOT__rate_counter;
    __Vdly__test_sid__DOT__uut__DOT__v2__DOT__adsr__DOT__unnamedblk1__DOT__rate_counter = 0;
    IData/*23:0*/ __Vdly__test_sid__DOT__uut__DOT__v3__DOT__oscillator;
    __Vdly__test_sid__DOT__uut__DOT__v3__DOT__oscillator = 0;
    IData/*22:0*/ __Vdly__test_sid__DOT__uut__DOT__v3__DOT__unnamedblk2__DOT__lfsr_noise;
    __Vdly__test_sid__DOT__uut__DOT__v3__DOT__unnamedblk2__DOT__lfsr_noise = 0;
    IData/*23:0*/ __Vdly__test_sid__DOT__uut__DOT__v3__DOT__unnamedblk2__DOT__noise_age;
    __Vdly__test_sid__DOT__uut__DOT__v3__DOT__unnamedblk2__DOT__noise_age = 0;
    IData/*23:0*/ __Vdly__test_sid__DOT__uut__DOT__v3__DOT__unnamedblk3__DOT__keep_cnt;
    __Vdly__test_sid__DOT__uut__DOT__v3__DOT__unnamedblk3__DOT__keep_cnt = 0;
    CData/*4:0*/ __Vdly__test_sid__DOT__uut__DOT__v3__DOT__adsr__DOT__unnamedblk1__DOT__exponential_counter_period;
    __Vdly__test_sid__DOT__uut__DOT__v3__DOT__adsr__DOT__unnamedblk1__DOT__exponential_counter_period = 0;
    CData/*1:0*/ __Vdly__test_sid__DOT__uut__DOT__v3__DOT__adsr__DOT__state;
    __Vdly__test_sid__DOT__uut__DOT__v3__DOT__adsr__DOT__state = 0;
    CData/*7:0*/ __Vdly__test_sid__DOT__uut__DOT__v3__DOT__envelope;
    __Vdly__test_sid__DOT__uut__DOT__v3__DOT__envelope = 0;
    CData/*0:0*/ __Vdly__test_sid__DOT__uut__DOT__v3__DOT__adsr__DOT__unnamedblk1__DOT__hold_zero;
    __Vdly__test_sid__DOT__uut__DOT__v3__DOT__adsr__DOT__unnamedblk1__DOT__hold_zero = 0;
    CData/*4:0*/ __Vdly__test_sid__DOT__uut__DOT__v3__DOT__adsr__DOT__unnamedblk1__DOT__exponential_counter;
    __Vdly__test_sid__DOT__uut__DOT__v3__DOT__adsr__DOT__unnamedblk1__DOT__exponential_counter = 0;
    SData/*14:0*/ __Vdly__test_sid__DOT__uut__DOT__v3__DOT__adsr__DOT__unnamedblk1__DOT__rate_counter;
    __Vdly__test_sid__DOT__uut__DOT__v3__DOT__adsr__DOT__unnamedblk1__DOT__rate_counter = 0;
    CData/*0:0*/ __VdlySet__test_sid__DOT__uut__DOT__voice_pw__v0;
    __VdlySet__test_sid__DOT__uut__DOT__voice_pw__v0 = 0;
    CData/*7:0*/ __VdlyVal__test_sid__DOT__uut__DOT__voice_pw__v3;
    __VdlyVal__test_sid__DOT__uut__DOT__voice_pw__v3 = 0;
    CData/*0:0*/ __VdlySet__test_sid__DOT__uut__DOT__voice_pw__v3;
    __VdlySet__test_sid__DOT__uut__DOT__voice_pw__v3 = 0;
    CData/*3:0*/ __VdlyVal__test_sid__DOT__uut__DOT__voice_pw__v4;
    __VdlyVal__test_sid__DOT__uut__DOT__voice_pw__v4 = 0;
    CData/*0:0*/ __VdlySet__test_sid__DOT__uut__DOT__voice_pw__v4;
    __VdlySet__test_sid__DOT__uut__DOT__voice_pw__v4 = 0;
    CData/*7:0*/ __VdlyVal__test_sid__DOT__uut__DOT__voice_pw__v5;
    __VdlyVal__test_sid__DOT__uut__DOT__voice_pw__v5 = 0;
    CData/*0:0*/ __VdlySet__test_sid__DOT__uut__DOT__voice_pw__v5;
    __VdlySet__test_sid__DOT__uut__DOT__voice_pw__v5 = 0;
    CData/*3:0*/ __VdlyVal__test_sid__DOT__uut__DOT__voice_pw__v6;
    __VdlyVal__test_sid__DOT__uut__DOT__voice_pw__v6 = 0;
    CData/*0:0*/ __VdlySet__test_sid__DOT__uut__DOT__voice_pw__v6;
    __VdlySet__test_sid__DOT__uut__DOT__voice_pw__v6 = 0;
    CData/*7:0*/ __VdlyVal__test_sid__DOT__uut__DOT__voice_pw__v7;
    __VdlyVal__test_sid__DOT__uut__DOT__voice_pw__v7 = 0;
    CData/*0:0*/ __VdlySet__test_sid__DOT__uut__DOT__voice_pw__v7;
    __VdlySet__test_sid__DOT__uut__DOT__voice_pw__v7 = 0;
    CData/*3:0*/ __VdlyVal__test_sid__DOT__uut__DOT__voice_pw__v8;
    __VdlyVal__test_sid__DOT__uut__DOT__voice_pw__v8 = 0;
    CData/*0:0*/ __VdlySet__test_sid__DOT__uut__DOT__voice_pw__v8;
    __VdlySet__test_sid__DOT__uut__DOT__voice_pw__v8 = 0;
    CData/*0:0*/ __VdlySet__test_sid__DOT__uut__DOT__voice_freq__v0;
    __VdlySet__test_sid__DOT__uut__DOT__voice_freq__v0 = 0;
    CData/*7:0*/ __VdlyVal__test_sid__DOT__uut__DOT__voice_freq__v3;
    __VdlyVal__test_sid__DOT__uut__DOT__voice_freq__v3 = 0;
    CData/*0:0*/ __VdlySet__test_sid__DOT__uut__DOT__voice_freq__v3;
    __VdlySet__test_sid__DOT__uut__DOT__voice_freq__v3 = 0;
    CData/*7:0*/ __VdlyVal__test_sid__DOT__uut__DOT__voice_freq__v4;
    __VdlyVal__test_sid__DOT__uut__DOT__voice_freq__v4 = 0;
    CData/*0:0*/ __VdlySet__test_sid__DOT__uut__DOT__voice_freq__v4;
    __VdlySet__test_sid__DOT__uut__DOT__voice_freq__v4 = 0;
    CData/*7:0*/ __VdlyVal__test_sid__DOT__uut__DOT__voice_freq__v5;
    __VdlyVal__test_sid__DOT__uut__DOT__voice_freq__v5 = 0;
    CData/*0:0*/ __VdlySet__test_sid__DOT__uut__DOT__voice_freq__v5;
    __VdlySet__test_sid__DOT__uut__DOT__voice_freq__v5 = 0;
    CData/*7:0*/ __VdlyVal__test_sid__DOT__uut__DOT__voice_freq__v6;
    __VdlyVal__test_sid__DOT__uut__DOT__voice_freq__v6 = 0;
    CData/*0:0*/ __VdlySet__test_sid__DOT__uut__DOT__voice_freq__v6;
    __VdlySet__test_sid__DOT__uut__DOT__voice_freq__v6 = 0;
    CData/*7:0*/ __VdlyVal__test_sid__DOT__uut__DOT__voice_freq__v7;
    __VdlyVal__test_sid__DOT__uut__DOT__voice_freq__v7 = 0;
    CData/*0:0*/ __VdlySet__test_sid__DOT__uut__DOT__voice_freq__v7;
    __VdlySet__test_sid__DOT__uut__DOT__voice_freq__v7 = 0;
    CData/*7:0*/ __VdlyVal__test_sid__DOT__uut__DOT__voice_freq__v8;
    __VdlyVal__test_sid__DOT__uut__DOT__voice_freq__v8 = 0;
    CData/*0:0*/ __VdlySet__test_sid__DOT__uut__DOT__voice_freq__v8;
    __VdlySet__test_sid__DOT__uut__DOT__voice_freq__v8 = 0;
    CData/*0:0*/ __VdlySet__test_sid__DOT__uut__DOT__voice_ctrl__v0;
    __VdlySet__test_sid__DOT__uut__DOT__voice_ctrl__v0 = 0;
    CData/*7:0*/ __VdlyVal__test_sid__DOT__uut__DOT__voice_ctrl__v3;
    __VdlyVal__test_sid__DOT__uut__DOT__voice_ctrl__v3 = 0;
    CData/*0:0*/ __VdlySet__test_sid__DOT__uut__DOT__voice_ctrl__v3;
    __VdlySet__test_sid__DOT__uut__DOT__voice_ctrl__v3 = 0;
    CData/*7:0*/ __VdlyVal__test_sid__DOT__uut__DOT__voice_ctrl__v4;
    __VdlyVal__test_sid__DOT__uut__DOT__voice_ctrl__v4 = 0;
    CData/*0:0*/ __VdlySet__test_sid__DOT__uut__DOT__voice_ctrl__v4;
    __VdlySet__test_sid__DOT__uut__DOT__voice_ctrl__v4 = 0;
    CData/*7:0*/ __VdlyVal__test_sid__DOT__uut__DOT__voice_ctrl__v5;
    __VdlyVal__test_sid__DOT__uut__DOT__voice_ctrl__v5 = 0;
    CData/*0:0*/ __VdlySet__test_sid__DOT__uut__DOT__voice_ctrl__v5;
    __VdlySet__test_sid__DOT__uut__DOT__voice_ctrl__v5 = 0;
    CData/*0:0*/ __VdlySet__test_sid__DOT__uut__DOT__voice_ad__v0;
    __VdlySet__test_sid__DOT__uut__DOT__voice_ad__v0 = 0;
    CData/*7:0*/ __VdlyVal__test_sid__DOT__uut__DOT__voice_ad__v3;
    __VdlyVal__test_sid__DOT__uut__DOT__voice_ad__v3 = 0;
    CData/*0:0*/ __VdlySet__test_sid__DOT__uut__DOT__voice_ad__v3;
    __VdlySet__test_sid__DOT__uut__DOT__voice_ad__v3 = 0;
    CData/*7:0*/ __VdlyVal__test_sid__DOT__uut__DOT__voice_ad__v4;
    __VdlyVal__test_sid__DOT__uut__DOT__voice_ad__v4 = 0;
    CData/*0:0*/ __VdlySet__test_sid__DOT__uut__DOT__voice_ad__v4;
    __VdlySet__test_sid__DOT__uut__DOT__voice_ad__v4 = 0;
    CData/*7:0*/ __VdlyVal__test_sid__DOT__uut__DOT__voice_ad__v5;
    __VdlyVal__test_sid__DOT__uut__DOT__voice_ad__v5 = 0;
    CData/*0:0*/ __VdlySet__test_sid__DOT__uut__DOT__voice_ad__v5;
    __VdlySet__test_sid__DOT__uut__DOT__voice_ad__v5 = 0;
    CData/*0:0*/ __VdlySet__test_sid__DOT__uut__DOT__voice_vol__v0;
    __VdlySet__test_sid__DOT__uut__DOT__voice_vol__v0 = 0;
    CData/*3:0*/ __VdlyVal__test_sid__DOT__uut__DOT__voice_vol__v3;
    __VdlyVal__test_sid__DOT__uut__DOT__voice_vol__v3 = 0;
    CData/*0:0*/ __VdlySet__test_sid__DOT__uut__DOT__voice_vol__v3;
    __VdlySet__test_sid__DOT__uut__DOT__voice_vol__v3 = 0;
    CData/*3:0*/ __VdlyVal__test_sid__DOT__uut__DOT__voice_vol__v4;
    __VdlyVal__test_sid__DOT__uut__DOT__voice_vol__v4 = 0;
    CData/*0:0*/ __VdlySet__test_sid__DOT__uut__DOT__voice_vol__v4;
    __VdlySet__test_sid__DOT__uut__DOT__voice_vol__v4 = 0;
    CData/*3:0*/ __VdlyVal__test_sid__DOT__uut__DOT__voice_vol__v5;
    __VdlyVal__test_sid__DOT__uut__DOT__voice_vol__v5 = 0;
    CData/*0:0*/ __VdlySet__test_sid__DOT__uut__DOT__voice_vol__v5;
    __VdlySet__test_sid__DOT__uut__DOT__voice_vol__v5 = 0;
    CData/*0:0*/ __VdlySet__test_sid__DOT__uut__DOT__voice_sr__v0;
    __VdlySet__test_sid__DOT__uut__DOT__voice_sr__v0 = 0;
    CData/*7:0*/ __VdlyVal__test_sid__DOT__uut__DOT__voice_sr__v3;
    __VdlyVal__test_sid__DOT__uut__DOT__voice_sr__v3 = 0;
    CData/*0:0*/ __VdlySet__test_sid__DOT__uut__DOT__voice_sr__v3;
    __VdlySet__test_sid__DOT__uut__DOT__voice_sr__v3 = 0;
    CData/*7:0*/ __VdlyVal__test_sid__DOT__uut__DOT__voice_sr__v4;
    __VdlyVal__test_sid__DOT__uut__DOT__voice_sr__v4 = 0;
    CData/*0:0*/ __VdlySet__test_sid__DOT__uut__DOT__voice_sr__v4;
    __VdlySet__test_sid__DOT__uut__DOT__voice_sr__v4 = 0;
    CData/*7:0*/ __VdlyVal__test_sid__DOT__uut__DOT__voice_sr__v5;
    __VdlyVal__test_sid__DOT__uut__DOT__voice_sr__v5 = 0;
    CData/*0:0*/ __VdlySet__test_sid__DOT__uut__DOT__voice_sr__v5;
    __VdlySet__test_sid__DOT__uut__DOT__voice_sr__v5 = 0;
    CData/*7:0*/ __VdlyVal__test_sid__DOT__uut__DOT___st_out__v0;
    __VdlyVal__test_sid__DOT__uut__DOT___st_out__v0 = 0;
    CData/*2:0*/ __VdlyDim0__test_sid__DOT__uut__DOT___st_out__v0;
    __VdlyDim0__test_sid__DOT__uut__DOT___st_out__v0 = 0;
    CData/*0:0*/ __VdlySet__test_sid__DOT__uut__DOT___st_out__v0;
    __VdlySet__test_sid__DOT__uut__DOT___st_out__v0 = 0;
    CData/*7:0*/ __VdlyVal__test_sid__DOT__uut__DOT__p_t_out__v0;
    __VdlyVal__test_sid__DOT__uut__DOT__p_t_out__v0 = 0;
    CData/*2:0*/ __VdlyDim0__test_sid__DOT__uut__DOT__p_t_out__v0;
    __VdlyDim0__test_sid__DOT__uut__DOT__p_t_out__v0 = 0;
    CData/*0:0*/ __VdlySet__test_sid__DOT__uut__DOT__p_t_out__v0;
    __VdlySet__test_sid__DOT__uut__DOT__p_t_out__v0 = 0;
    CData/*7:0*/ __VdlyVal__test_sid__DOT__uut__DOT__ps___05Fout__v0;
    __VdlyVal__test_sid__DOT__uut__DOT__ps___05Fout__v0 = 0;
    CData/*2:0*/ __VdlyDim0__test_sid__DOT__uut__DOT__ps___05Fout__v0;
    __VdlyDim0__test_sid__DOT__uut__DOT__ps___05Fout__v0 = 0;
    CData/*0:0*/ __VdlySet__test_sid__DOT__uut__DOT__ps___05Fout__v0;
    __VdlySet__test_sid__DOT__uut__DOT__ps___05Fout__v0 = 0;
    CData/*7:0*/ __VdlyVal__test_sid__DOT__uut__DOT__pst_out__v0;
    __VdlyVal__test_sid__DOT__uut__DOT__pst_out__v0 = 0;
    CData/*2:0*/ __VdlyDim0__test_sid__DOT__uut__DOT__pst_out__v0;
    __VdlyDim0__test_sid__DOT__uut__DOT__pst_out__v0 = 0;
    CData/*0:0*/ __VdlySet__test_sid__DOT__uut__DOT__pst_out__v0;
    __VdlySet__test_sid__DOT__uut__DOT__pst_out__v0 = 0;
    // Body
    __VdlySet__test_sid__DOT__uut__DOT__voice_ad__v0 = 0U;
    __VdlySet__test_sid__DOT__uut__DOT__voice_ad__v3 = 0U;
    __VdlySet__test_sid__DOT__uut__DOT__voice_ad__v4 = 0U;
    __VdlySet__test_sid__DOT__uut__DOT__voice_ad__v5 = 0U;
    __VdlySet__test_sid__DOT__uut__DOT__voice_sr__v0 = 0U;
    __VdlySet__test_sid__DOT__uut__DOT__voice_sr__v3 = 0U;
    __VdlySet__test_sid__DOT__uut__DOT__voice_sr__v4 = 0U;
    __VdlySet__test_sid__DOT__uut__DOT__voice_sr__v5 = 0U;
    __VdlySet__test_sid__DOT__uut__DOT__voice_vol__v0 = 0U;
    __VdlySet__test_sid__DOT__uut__DOT__voice_vol__v3 = 0U;
    __VdlySet__test_sid__DOT__uut__DOT__voice_vol__v4 = 0U;
    __VdlySet__test_sid__DOT__uut__DOT__voice_vol__v5 = 0U;
    __VdlySet__test_sid__DOT__uut__DOT__voice_pw__v0 = 0U;
    __VdlySet__test_sid__DOT__uut__DOT__voice_pw__v3 = 0U;
    __VdlySet__test_sid__DOT__uut__DOT__voice_pw__v4 = 0U;
    __VdlySet__test_sid__DOT__uut__DOT__voice_pw__v5 = 0U;
    __VdlySet__test_sid__DOT__uut__DOT__voice_pw__v6 = 0U;
    __VdlySet__test_sid__DOT__uut__DOT__voice_pw__v7 = 0U;
    __VdlySet__test_sid__DOT__uut__DOT__voice_pw__v8 = 0U;
    __VdlySet__test_sid__DOT__uut__DOT__voice_freq__v0 = 0U;
    __VdlySet__test_sid__DOT__uut__DOT__voice_freq__v3 = 0U;
    __VdlySet__test_sid__DOT__uut__DOT__voice_freq__v4 = 0U;
    __VdlySet__test_sid__DOT__uut__DOT__voice_freq__v5 = 0U;
    __VdlySet__test_sid__DOT__uut__DOT__voice_freq__v6 = 0U;
    __VdlySet__test_sid__DOT__uut__DOT__voice_freq__v7 = 0U;
    __VdlySet__test_sid__DOT__uut__DOT__voice_freq__v8 = 0U;
    __Vdly__test_sid__DOT__uut__DOT__tbl_state = vlSelfRef.test_sid__DOT__uut__DOT__tbl_state;
    __Vdly__test_sid__DOT__uut__DOT__f_acc_t = vlSelfRef.test_sid__DOT__uut__DOT__f_acc_t;
    __VdlySet__test_sid__DOT__uut__DOT___st_out__v0 = 0U;
    __VdlySet__test_sid__DOT__uut__DOT__p_t_out__v0 = 0U;
    __VdlySet__test_sid__DOT__uut__DOT__ps___05Fout__v0 = 0U;
    __VdlySet__test_sid__DOT__uut__DOT__pst_out__v0 = 0U;
    __Vdly__test_sid__DOT__uut__DOT__v1__DOT__unnamedblk3__DOT__keep_cnt 
        = vlSelfRef.test_sid__DOT__uut__DOT__v1__DOT__unnamedblk3__DOT__keep_cnt;
    __Vdly__test_sid__DOT__uut__DOT__v2__DOT__unnamedblk3__DOT__keep_cnt 
        = vlSelfRef.test_sid__DOT__uut__DOT__v2__DOT__unnamedblk3__DOT__keep_cnt;
    __Vdly__test_sid__DOT__uut__DOT__v3__DOT__unnamedblk3__DOT__keep_cnt 
        = vlSelfRef.test_sid__DOT__uut__DOT__v3__DOT__unnamedblk3__DOT__keep_cnt;
    __Vdly__test_sid__DOT__uut__DOT__v1__DOT__oscillator 
        = vlSelfRef.test_sid__DOT__uut__DOT__v1__DOT__oscillator;
    __Vdly__test_sid__DOT__uut__DOT__v2__DOT__oscillator 
        = vlSelfRef.test_sid__DOT__uut__DOT__v2__DOT__oscillator;
    __Vdly__test_sid__DOT__uut__DOT__v3__DOT__oscillator 
        = vlSelfRef.test_sid__DOT__uut__DOT__v3__DOT__oscillator;
    __Vdly__test_sid__DOT__uut__DOT__v1__DOT__adsr__DOT__unnamedblk1__DOT__exponential_counter_period 
        = vlSelfRef.test_sid__DOT__uut__DOT__v1__DOT__adsr__DOT__unnamedblk1__DOT__exponential_counter_period;
    __Vdly__test_sid__DOT__uut__DOT__v1__DOT__adsr__DOT__unnamedblk1__DOT__hold_zero 
        = vlSelfRef.test_sid__DOT__uut__DOT__v1__DOT__adsr__DOT__unnamedblk1__DOT__hold_zero;
    __Vdly__test_sid__DOT__uut__DOT__v1__DOT__adsr__DOT__unnamedblk1__DOT__exponential_counter 
        = vlSelfRef.test_sid__DOT__uut__DOT__v1__DOT__adsr__DOT__unnamedblk1__DOT__exponential_counter;
    __Vdly__test_sid__DOT__uut__DOT__v1__DOT__adsr__DOT__unnamedblk1__DOT__rate_counter 
        = vlSelfRef.test_sid__DOT__uut__DOT__v1__DOT__adsr__DOT__unnamedblk1__DOT__rate_counter;
    __Vdly__test_sid__DOT__uut__DOT__v2__DOT__adsr__DOT__unnamedblk1__DOT__exponential_counter_period 
        = vlSelfRef.test_sid__DOT__uut__DOT__v2__DOT__adsr__DOT__unnamedblk1__DOT__exponential_counter_period;
    __Vdly__test_sid__DOT__uut__DOT__v2__DOT__adsr__DOT__unnamedblk1__DOT__hold_zero 
        = vlSelfRef.test_sid__DOT__uut__DOT__v2__DOT__adsr__DOT__unnamedblk1__DOT__hold_zero;
    __Vdly__test_sid__DOT__uut__DOT__v2__DOT__adsr__DOT__unnamedblk1__DOT__exponential_counter 
        = vlSelfRef.test_sid__DOT__uut__DOT__v2__DOT__adsr__DOT__unnamedblk1__DOT__exponential_counter;
    __Vdly__test_sid__DOT__uut__DOT__v2__DOT__adsr__DOT__unnamedblk1__DOT__rate_counter 
        = vlSelfRef.test_sid__DOT__uut__DOT__v2__DOT__adsr__DOT__unnamedblk1__DOT__rate_counter;
    __Vdly__test_sid__DOT__uut__DOT__v1__DOT__adsr__DOT__state 
        = vlSelfRef.test_sid__DOT__uut__DOT__v1__DOT__adsr__DOT__state;
    __Vdly__test_sid__DOT__uut__DOT__v2__DOT__adsr__DOT__state 
        = vlSelfRef.test_sid__DOT__uut__DOT__v2__DOT__adsr__DOT__state;
    __Vdly__test_sid__DOT__uut__DOT__v3__DOT__adsr__DOT__unnamedblk1__DOT__exponential_counter_period 
        = vlSelfRef.test_sid__DOT__uut__DOT__v3__DOT__adsr__DOT__unnamedblk1__DOT__exponential_counter_period;
    __Vdly__test_sid__DOT__uut__DOT__v3__DOT__adsr__DOT__unnamedblk1__DOT__hold_zero 
        = vlSelfRef.test_sid__DOT__uut__DOT__v3__DOT__adsr__DOT__unnamedblk1__DOT__hold_zero;
    __Vdly__test_sid__DOT__uut__DOT__v3__DOT__adsr__DOT__unnamedblk1__DOT__exponential_counter 
        = vlSelfRef.test_sid__DOT__uut__DOT__v3__DOT__adsr__DOT__unnamedblk1__DOT__exponential_counter;
    __Vdly__test_sid__DOT__uut__DOT__v3__DOT__adsr__DOT__unnamedblk1__DOT__rate_counter 
        = vlSelfRef.test_sid__DOT__uut__DOT__v3__DOT__adsr__DOT__unnamedblk1__DOT__rate_counter;
    __Vdly__test_sid__DOT__uut__DOT__v3__DOT__adsr__DOT__state 
        = vlSelfRef.test_sid__DOT__uut__DOT__v3__DOT__adsr__DOT__state;
    __Vdly__test_sid__DOT__uut__DOT__v1__DOT__envelope 
        = vlSelfRef.test_sid__DOT__uut__DOT__v1__DOT__envelope;
    __Vdly__test_sid__DOT__uut__DOT__v2__DOT__envelope 
        = vlSelfRef.test_sid__DOT__uut__DOT__v2__DOT__envelope;
    __Vdly__test_sid__DOT__uut__DOT__v3__DOT__envelope 
        = vlSelfRef.test_sid__DOT__uut__DOT__v3__DOT__envelope;
    __Vdly__test_sid__DOT__uut__DOT__v1__DOT__unnamedblk2__DOT__lfsr_noise 
        = vlSelfRef.test_sid__DOT__uut__DOT__v1__DOT__unnamedblk2__DOT__lfsr_noise;
    __Vdly__test_sid__DOT__uut__DOT__v1__DOT__unnamedblk2__DOT__noise_age 
        = vlSelfRef.test_sid__DOT__uut__DOT__v1__DOT__unnamedblk2__DOT__noise_age;
    __Vdly__test_sid__DOT__uut__DOT__v2__DOT__unnamedblk2__DOT__lfsr_noise 
        = vlSelfRef.test_sid__DOT__uut__DOT__v2__DOT__unnamedblk2__DOT__lfsr_noise;
    __Vdly__test_sid__DOT__uut__DOT__v2__DOT__unnamedblk2__DOT__noise_age 
        = vlSelfRef.test_sid__DOT__uut__DOT__v2__DOT__unnamedblk2__DOT__noise_age;
    __Vdly__test_sid__DOT__uut__DOT__v3__DOT__unnamedblk2__DOT__lfsr_noise 
        = vlSelfRef.test_sid__DOT__uut__DOT__v3__DOT__unnamedblk2__DOT__lfsr_noise;
    __Vdly__test_sid__DOT__uut__DOT__v3__DOT__unnamedblk2__DOT__noise_age 
        = vlSelfRef.test_sid__DOT__uut__DOT__v3__DOT__unnamedblk2__DOT__noise_age;
    __VdlySet__test_sid__DOT__uut__DOT__voice_ctrl__v0 = 0U;
    __VdlySet__test_sid__DOT__uut__DOT__voice_ctrl__v3 = 0U;
    __VdlySet__test_sid__DOT__uut__DOT__voice_ctrl__v4 = 0U;
    __VdlySet__test_sid__DOT__uut__DOT__voice_ctrl__v5 = 0U;
    if ((1U & (~ (0x0000000fU == (IData)(vlSelfRef.test_sid__DOT__uut__DOT__tbl_state))))) {
        __Vdly__test_sid__DOT__uut__DOT__tbl_state 
            = (0x0000000fU & ((IData)(1U) + (IData)(vlSelfRef.test_sid__DOT__uut__DOT__tbl_state)));
    }
    if ((((1U == (IData)(vlSelfRef.test_sid__DOT__uut__DOT__tbl_state)) 
          || (3U == (IData)(vlSelfRef.test_sid__DOT__uut__DOT__tbl_state))) 
         || (5U == (IData)(vlSelfRef.test_sid__DOT__uut__DOT__tbl_state)))) {
        __Vdly__test_sid__DOT__uut__DOT__f_acc_t = 
            ((2U >= (3U & ((IData)(vlSelfRef.test_sid__DOT__uut__DOT__tbl_state) 
                           >> 1U))) ? vlSelfRef.test_sid__DOT__uut__DOT__acc_t
             [(3U & ((IData)(vlSelfRef.test_sid__DOT__uut__DOT__tbl_state) 
                     >> 1U))] : 0U);
    }
    if ((((3U == (IData)(vlSelfRef.test_sid__DOT__uut__DOT__tbl_state)) 
          || (5U == (IData)(vlSelfRef.test_sid__DOT__uut__DOT__tbl_state))) 
         || (7U == (IData)(vlSelfRef.test_sid__DOT__uut__DOT__tbl_state)))) {
        vlSelfRef.test_sid__DOT__uut__DOT____Vlvbound_h9cc45699__0 
            = vlSelfRef.test_sid__DOT__uut__DOT__f___05Fst_out;
        vlSelfRef.test_sid__DOT__uut__DOT____Vlvbound_h49d001ad__0 
            = vlSelfRef.test_sid__DOT__uut__DOT__f_p_t_out;
        vlSelfRef.test_sid__DOT__uut__DOT____Vlvbound_h061f25e8__0 
            = vlSelfRef.test_sid__DOT__uut__DOT__f_ps___05Fout;
        vlSelfRef.test_sid__DOT__uut__DOT____Vlvbound_h0706378b__0 
            = vlSelfRef.test_sid__DOT__uut__DOT__f_pst_out;
        if ((5U >= (7U & ((3U & ((IData)(vlSelfRef.test_sid__DOT__uut__DOT__tbl_state) 
                                 >> 1U)) - (IData)(1U))))) {
            __VdlyVal__test_sid__DOT__uut__DOT___st_out__v0 
                = vlSelfRef.test_sid__DOT__uut__DOT____Vlvbound_h9cc45699__0;
            __VdlyDim0__test_sid__DOT__uut__DOT___st_out__v0 
                = (7U & ((3U & ((IData)(vlSelfRef.test_sid__DOT__uut__DOT__tbl_state) 
                                >> 1U)) - (IData)(1U)));
            __VdlySet__test_sid__DOT__uut__DOT___st_out__v0 = 1U;
            __VdlyVal__test_sid__DOT__uut__DOT__p_t_out__v0 
                = vlSelfRef.test_sid__DOT__uut__DOT____Vlvbound_h49d001ad__0;
            __VdlyDim0__test_sid__DOT__uut__DOT__p_t_out__v0 
                = (7U & ((3U & ((IData)(vlSelfRef.test_sid__DOT__uut__DOT__tbl_state) 
                                >> 1U)) - (IData)(1U)));
            __VdlySet__test_sid__DOT__uut__DOT__p_t_out__v0 = 1U;
            __VdlyVal__test_sid__DOT__uut__DOT__ps___05Fout__v0 
                = vlSelfRef.test_sid__DOT__uut__DOT____Vlvbound_h061f25e8__0;
            __VdlyDim0__test_sid__DOT__uut__DOT__ps___05Fout__v0 
                = (7U & ((3U & ((IData)(vlSelfRef.test_sid__DOT__uut__DOT__tbl_state) 
                                >> 1U)) - (IData)(1U)));
            __VdlySet__test_sid__DOT__uut__DOT__ps___05Fout__v0 = 1U;
            __VdlyVal__test_sid__DOT__uut__DOT__pst_out__v0 
                = vlSelfRef.test_sid__DOT__uut__DOT____Vlvbound_h0706378b__0;
            __VdlyDim0__test_sid__DOT__uut__DOT__pst_out__v0 
                = (7U & ((3U & ((IData)(vlSelfRef.test_sid__DOT__uut__DOT__tbl_state) 
                                >> 1U)) - (IData)(1U)));
            __VdlySet__test_sid__DOT__uut__DOT__pst_out__v0 = 1U;
        }
    }
    if ((0xffU == (IData)(vlSelfRef.test_sid__DOT__uut__DOT__v1__DOT__envelope))) {
        __Vdly__test_sid__DOT__uut__DOT__v1__DOT__adsr__DOT__unnamedblk1__DOT__exponential_counter_period = 0U;
    } else if ((0x5dU == (IData)(vlSelfRef.test_sid__DOT__uut__DOT__v1__DOT__envelope))) {
        __Vdly__test_sid__DOT__uut__DOT__v1__DOT__adsr__DOT__unnamedblk1__DOT__exponential_counter_period = 1U;
    } else if ((0x36U == (IData)(vlSelfRef.test_sid__DOT__uut__DOT__v1__DOT__envelope))) {
        __Vdly__test_sid__DOT__uut__DOT__v1__DOT__adsr__DOT__unnamedblk1__DOT__exponential_counter_period = 3U;
    } else if ((0x1aU == (IData)(vlSelfRef.test_sid__DOT__uut__DOT__v1__DOT__envelope))) {
        __Vdly__test_sid__DOT__uut__DOT__v1__DOT__adsr__DOT__unnamedblk1__DOT__exponential_counter_period = 7U;
    } else if ((0x0eU == (IData)(vlSelfRef.test_sid__DOT__uut__DOT__v1__DOT__envelope))) {
        __Vdly__test_sid__DOT__uut__DOT__v1__DOT__adsr__DOT__unnamedblk1__DOT__exponential_counter_period = 0x0fU;
    } else if ((6U == (IData)(vlSelfRef.test_sid__DOT__uut__DOT__v1__DOT__envelope))) {
        __Vdly__test_sid__DOT__uut__DOT__v1__DOT__adsr__DOT__unnamedblk1__DOT__exponential_counter_period = 0x1dU;
    } else if ((0U == (IData)(vlSelfRef.test_sid__DOT__uut__DOT__v1__DOT__envelope))) {
        __Vdly__test_sid__DOT__uut__DOT__v1__DOT__adsr__DOT__unnamedblk1__DOT__exponential_counter_period = 0U;
    }
    if ((0xffU == (IData)(vlSelfRef.test_sid__DOT__uut__DOT__v2__DOT__envelope))) {
        __Vdly__test_sid__DOT__uut__DOT__v2__DOT__adsr__DOT__unnamedblk1__DOT__exponential_counter_period = 0U;
    } else if ((0x5dU == (IData)(vlSelfRef.test_sid__DOT__uut__DOT__v2__DOT__envelope))) {
        __Vdly__test_sid__DOT__uut__DOT__v2__DOT__adsr__DOT__unnamedblk1__DOT__exponential_counter_period = 1U;
    } else if ((0x36U == (IData)(vlSelfRef.test_sid__DOT__uut__DOT__v2__DOT__envelope))) {
        __Vdly__test_sid__DOT__uut__DOT__v2__DOT__adsr__DOT__unnamedblk1__DOT__exponential_counter_period = 3U;
    } else if ((0x1aU == (IData)(vlSelfRef.test_sid__DOT__uut__DOT__v2__DOT__envelope))) {
        __Vdly__test_sid__DOT__uut__DOT__v2__DOT__adsr__DOT__unnamedblk1__DOT__exponential_counter_period = 7U;
    } else if ((0x0eU == (IData)(vlSelfRef.test_sid__DOT__uut__DOT__v2__DOT__envelope))) {
        __Vdly__test_sid__DOT__uut__DOT__v2__DOT__adsr__DOT__unnamedblk1__DOT__exponential_counter_period = 0x0fU;
    } else if ((6U == (IData)(vlSelfRef.test_sid__DOT__uut__DOT__v2__DOT__envelope))) {
        __Vdly__test_sid__DOT__uut__DOT__v2__DOT__adsr__DOT__unnamedblk1__DOT__exponential_counter_period = 0x1dU;
    } else if ((0U == (IData)(vlSelfRef.test_sid__DOT__uut__DOT__v2__DOT__envelope))) {
        __Vdly__test_sid__DOT__uut__DOT__v2__DOT__adsr__DOT__unnamedblk1__DOT__exponential_counter_period = 0U;
    }
    if ((0xffU == (IData)(vlSelfRef.test_sid__DOT__uut__DOT__v3__DOT__envelope))) {
        __Vdly__test_sid__DOT__uut__DOT__v3__DOT__adsr__DOT__unnamedblk1__DOT__exponential_counter_period = 0U;
    } else if ((0x5dU == (IData)(vlSelfRef.test_sid__DOT__uut__DOT__v3__DOT__envelope))) {
        __Vdly__test_sid__DOT__uut__DOT__v3__DOT__adsr__DOT__unnamedblk1__DOT__exponential_counter_period = 1U;
    } else if ((0x36U == (IData)(vlSelfRef.test_sid__DOT__uut__DOT__v3__DOT__envelope))) {
        __Vdly__test_sid__DOT__uut__DOT__v3__DOT__adsr__DOT__unnamedblk1__DOT__exponential_counter_period = 3U;
    } else if ((0x1aU == (IData)(vlSelfRef.test_sid__DOT__uut__DOT__v3__DOT__envelope))) {
        __Vdly__test_sid__DOT__uut__DOT__v3__DOT__adsr__DOT__unnamedblk1__DOT__exponential_counter_period = 7U;
    } else if ((0x0eU == (IData)(vlSelfRef.test_sid__DOT__uut__DOT__v3__DOT__envelope))) {
        __Vdly__test_sid__DOT__uut__DOT__v3__DOT__adsr__DOT__unnamedblk1__DOT__exponential_counter_period = 0x0fU;
    } else if ((6U == (IData)(vlSelfRef.test_sid__DOT__uut__DOT__v3__DOT__envelope))) {
        __Vdly__test_sid__DOT__uut__DOT__v3__DOT__adsr__DOT__unnamedblk1__DOT__exponential_counter_period = 0x1dU;
    } else if ((0U == (IData)(vlSelfRef.test_sid__DOT__uut__DOT__v3__DOT__envelope))) {
        __Vdly__test_sid__DOT__uut__DOT__v3__DOT__adsr__DOT__unnamedblk1__DOT__exponential_counter_period = 0U;
    }
    if (vlSelfRef.test_sid__DOT__ce_1m) {
        __Vdly__test_sid__DOT__uut__DOT__tbl_state = 0U;
        __Vdly__test_sid__DOT__uut__DOT__v1__DOT__oscillator 
            = ((1U & ((((IData)(vlSelfRef.test_sid__DOT__rst) 
                        | (vlSelfRef.test_sid__DOT__uut__DOT__voice_ctrl[0U] 
                           >> 3U)) | (IData)(vlSelfRef.test_sid__DOT__uut__DOT__v1__DOT__unnamedblk1__DOT__test_delay)) 
                      | (((vlSelfRef.test_sid__DOT__uut__DOT__voice_ctrl[0U] 
                           >> 1U) & (~ (vlSelfRef.test_sid__DOT__uut__DOT__v3__DOT__oscillator 
                                        >> 0x00000017U))) 
                         & (IData)(vlSelfRef.test_sid__DOT__uut__DOT__v1__DOT__osc_msb_in_prv))))
                ? 0U : (0x00ffffffU & (vlSelfRef.test_sid__DOT__uut__DOT__v1__DOT__oscillator 
                                       + vlSelfRef.test_sid__DOT__uut__DOT__voice_freq[0U])));
        __Vdly__test_sid__DOT__uut__DOT__v2__DOT__oscillator 
            = ((1U & ((((IData)(vlSelfRef.test_sid__DOT__rst) 
                        | (vlSelfRef.test_sid__DOT__uut__DOT__voice_ctrl[1U] 
                           >> 3U)) | (IData)(vlSelfRef.test_sid__DOT__uut__DOT__v2__DOT__unnamedblk1__DOT__test_delay)) 
                      | (((vlSelfRef.test_sid__DOT__uut__DOT__voice_ctrl[1U] 
                           >> 1U) & (~ (vlSelfRef.test_sid__DOT__uut__DOT__v1__DOT__oscillator 
                                        >> 0x00000017U))) 
                         & (IData)(vlSelfRef.test_sid__DOT__uut__DOT__v2__DOT__osc_msb_in_prv))))
                ? 0U : (0x00ffffffU & (vlSelfRef.test_sid__DOT__uut__DOT__v2__DOT__oscillator 
                                       + vlSelfRef.test_sid__DOT__uut__DOT__voice_freq[1U])));
        __Vdly__test_sid__DOT__uut__DOT__v3__DOT__oscillator 
            = ((1U & ((((IData)(vlSelfRef.test_sid__DOT__rst) 
                        | (vlSelfRef.test_sid__DOT__uut__DOT__voice_ctrl[2U] 
                           >> 3U)) | (IData)(vlSelfRef.test_sid__DOT__uut__DOT__v3__DOT__unnamedblk1__DOT__test_delay)) 
                      | (((vlSelfRef.test_sid__DOT__uut__DOT__voice_ctrl[2U] 
                           >> 1U) & (~ (vlSelfRef.test_sid__DOT__uut__DOT__v2__DOT__oscillator 
                                        >> 0x00000017U))) 
                         & (IData)(vlSelfRef.test_sid__DOT__uut__DOT__v3__DOT__osc_msb_in_prv))))
                ? 0U : (0x00ffffffU & (vlSelfRef.test_sid__DOT__uut__DOT__v3__DOT__oscillator 
                                       + vlSelfRef.test_sid__DOT__uut__DOT__voice_freq[2U])));
        vlSelfRef.test_sid__DOT__uut__DOT__v1__DOT__dca_out 
            = (0x003fffffU & (((IData)(vlSelfRef.test_sid__DOT__mode)
                                ? 0U : 0x00033cc0U) 
                              + VL_MULS_III(22, (0x003fffffU 
                                                 & VL_EXTENDS_II(22,13, (IData)(vlSelfRef.test_sid__DOT__uut__DOT__v1__DOT__unnamedblk3__DOT__dac_out))), 
                                            (0x003fffffU 
                                             & VL_EXTENDS_II(22,9, (IData)(vlSelfRef.test_sid__DOT__uut__DOT__v1__DOT__unnamedblk3__DOT__env_dac))))));
        vlSelfRef.test_sid__DOT__uut__DOT__v2__DOT__dca_out 
            = (0x003fffffU & (((IData)(vlSelfRef.test_sid__DOT__mode)
                                ? 0U : 0x00033cc0U) 
                              + VL_MULS_III(22, (0x003fffffU 
                                                 & VL_EXTENDS_II(22,13, (IData)(vlSelfRef.test_sid__DOT__uut__DOT__v2__DOT__unnamedblk3__DOT__dac_out))), 
                                            (0x003fffffU 
                                             & VL_EXTENDS_II(22,9, (IData)(vlSelfRef.test_sid__DOT__uut__DOT__v2__DOT__unnamedblk3__DOT__env_dac))))));
        vlSelfRef.test_sid__DOT__uut__DOT__v3__DOT__dca_out 
            = (0x003fffffU & (((IData)(vlSelfRef.test_sid__DOT__mode)
                                ? 0U : 0x00033cc0U) 
                              + VL_MULS_III(22, (0x003fffffU 
                                                 & VL_EXTENDS_II(22,13, (IData)(vlSelfRef.test_sid__DOT__uut__DOT__v3__DOT__unnamedblk3__DOT__dac_out))), 
                                            (0x003fffffU 
                                             & VL_EXTENDS_II(22,9, (IData)(vlSelfRef.test_sid__DOT__uut__DOT__v3__DOT__unnamedblk3__DOT__env_dac))))));
        vlSelfRef.test_sid__DOT__uut__DOT__v1__DOT__osc_msb_in_prv 
            = (1U & (vlSelfRef.test_sid__DOT__uut__DOT__v3__DOT__oscillator 
                     >> 0x00000017U));
        vlSelfRef.test_sid__DOT__uut__DOT__v1__DOT__unnamedblk1__DOT__test_delay 
            = ((IData)(vlSelfRef.test_sid__DOT__mode) 
               & (vlSelfRef.test_sid__DOT__uut__DOT__voice_ctrl[0U] 
                  >> 3U));
        vlSelfRef.test_sid__DOT__uut__DOT__v2__DOT__osc_msb_in_prv 
            = (1U & (vlSelfRef.test_sid__DOT__uut__DOT__v1__DOT__oscillator 
                     >> 0x00000017U));
        vlSelfRef.test_sid__DOT__uut__DOT__v2__DOT__unnamedblk1__DOT__test_delay 
            = ((IData)(vlSelfRef.test_sid__DOT__mode) 
               & (vlSelfRef.test_sid__DOT__uut__DOT__voice_ctrl[1U] 
                  >> 3U));
        vlSelfRef.test_sid__DOT__uut__DOT__v3__DOT__osc_msb_in_prv 
            = (1U & (vlSelfRef.test_sid__DOT__uut__DOT__v2__DOT__oscillator 
                     >> 0x00000017U));
        if (vlSelfRef.test_sid__DOT__mode) {
            vlSelfRef.test_sid__DOT__uut__DOT__v3__DOT__unnamedblk1__DOT__test_delay 
                = (1U & (vlSelfRef.test_sid__DOT__uut__DOT__voice_ctrl[2U] 
                         >> 3U));
            vlSelfRef.test_sid__DOT__uut__DOT__v1__DOT__unnamedblk3__DOT__env_dac 
                = vlSelfRef.test_sid__DOT__uut__DOT__v1__DOT__envelope;
            vlSelfRef.test_sid__DOT__uut__DOT__v2__DOT__unnamedblk3__DOT__env_dac 
                = vlSelfRef.test_sid__DOT__uut__DOT__v2__DOT__envelope;
            vlSelfRef.test_sid__DOT__uut__DOT__v3__DOT__unnamedblk3__DOT__env_dac 
                = vlSelfRef.test_sid__DOT__uut__DOT__v3__DOT__envelope;
        } else {
            vlSelfRef.test_sid__DOT__uut__DOT__v3__DOT__unnamedblk1__DOT__test_delay = 0U;
            vlSelfRef.test_sid__DOT__uut__DOT__v1__DOT__unnamedblk3__DOT__env_dac 
                = (0x000000ffU & (vlSelfRef.test_sid__DOT__uut__DOT__v1__DOT__envelope_dac__DOT__bitsum[7U] 
                                  >> 4U));
            vlSelfRef.test_sid__DOT__uut__DOT__v2__DOT__unnamedblk3__DOT__env_dac 
                = (0x000000ffU & (vlSelfRef.test_sid__DOT__uut__DOT__v2__DOT__envelope_dac__DOT__bitsum[7U] 
                                  >> 4U));
            vlSelfRef.test_sid__DOT__uut__DOT__v3__DOT__unnamedblk3__DOT__env_dac 
                = (0x000000ffU & (vlSelfRef.test_sid__DOT__uut__DOT__v3__DOT__envelope_dac__DOT__bitsum[7U] 
                                  >> 4U));
        }
        if ((0U != (0x0000000fU & (vlSelfRef.test_sid__DOT__uut__DOT__voice_ctrl[0U] 
                                   >> 4U)))) {
            __Vdly__test_sid__DOT__uut__DOT__v1__DOT__unnamedblk3__DOT__keep_cnt 
                = ((IData)(vlSelfRef.test_sid__DOT__mode)
                    ? 0x00030d40U : 0x004c4b40U);
            vlSelfRef.test_sid__DOT__uut__DOT__v1__DOT__unnamedblk3__DOT__dac_out 
                = (0x00001fffU & (((IData)(vlSelfRef.test_sid__DOT__uut__DOT__v1__DOT__norm_dac) 
                                   | ((IData)(vlSelfRef.test_sid__DOT__uut__DOT__v1__DOT__comb) 
                                      << 4U)) - ((IData)(vlSelfRef.test_sid__DOT__mode)
                                                  ? 0x0800U
                                                  : 0x0380U)));
        } else if ((0U != vlSelfRef.test_sid__DOT__uut__DOT__v1__DOT__unnamedblk3__DOT__keep_cnt)) {
            __Vdly__test_sid__DOT__uut__DOT__v1__DOT__unnamedblk3__DOT__keep_cnt 
                = (0x00ffffffU & (vlSelfRef.test_sid__DOT__uut__DOT__v1__DOT__unnamedblk3__DOT__keep_cnt 
                                  - (IData)(1U)));
        } else {
            vlSelfRef.test_sid__DOT__uut__DOT__v1__DOT__unnamedblk3__DOT__dac_out = 0U;
        }
        if ((0U != (0x0000000fU & (vlSelfRef.test_sid__DOT__uut__DOT__voice_ctrl[1U] 
                                   >> 4U)))) {
            __Vdly__test_sid__DOT__uut__DOT__v2__DOT__unnamedblk3__DOT__keep_cnt 
                = ((IData)(vlSelfRef.test_sid__DOT__mode)
                    ? 0x00030d40U : 0x004c4b40U);
            vlSelfRef.test_sid__DOT__uut__DOT__v2__DOT__unnamedblk3__DOT__dac_out 
                = (0x00001fffU & (((IData)(vlSelfRef.test_sid__DOT__uut__DOT__v2__DOT__norm_dac) 
                                   | ((IData)(vlSelfRef.test_sid__DOT__uut__DOT__v2__DOT__comb) 
                                      << 4U)) - ((IData)(vlSelfRef.test_sid__DOT__mode)
                                                  ? 0x0800U
                                                  : 0x0380U)));
        } else if ((0U != vlSelfRef.test_sid__DOT__uut__DOT__v2__DOT__unnamedblk3__DOT__keep_cnt)) {
            __Vdly__test_sid__DOT__uut__DOT__v2__DOT__unnamedblk3__DOT__keep_cnt 
                = (0x00ffffffU & (vlSelfRef.test_sid__DOT__uut__DOT__v2__DOT__unnamedblk3__DOT__keep_cnt 
                                  - (IData)(1U)));
        } else {
            vlSelfRef.test_sid__DOT__uut__DOT__v2__DOT__unnamedblk3__DOT__dac_out = 0U;
        }
        if ((0U != (0x0000000fU & (vlSelfRef.test_sid__DOT__uut__DOT__voice_ctrl[2U] 
                                   >> 4U)))) {
            __Vdly__test_sid__DOT__uut__DOT__v3__DOT__unnamedblk3__DOT__keep_cnt 
                = ((IData)(vlSelfRef.test_sid__DOT__mode)
                    ? 0x00030d40U : 0x004c4b40U);
            vlSelfRef.test_sid__DOT__uut__DOT__v3__DOT__unnamedblk3__DOT__dac_out 
                = (0x00001fffU & (((IData)(vlSelfRef.test_sid__DOT__uut__DOT__v3__DOT__norm_dac) 
                                   | ((IData)(vlSelfRef.test_sid__DOT__uut__DOT__v3__DOT__comb) 
                                      << 4U)) - ((IData)(vlSelfRef.test_sid__DOT__mode)
                                                  ? 0x0800U
                                                  : 0x0380U)));
        } else if ((0U != vlSelfRef.test_sid__DOT__uut__DOT__v3__DOT__unnamedblk3__DOT__keep_cnt)) {
            __Vdly__test_sid__DOT__uut__DOT__v3__DOT__unnamedblk3__DOT__keep_cnt 
                = (0x00ffffffU & (vlSelfRef.test_sid__DOT__uut__DOT__v3__DOT__unnamedblk3__DOT__keep_cnt 
                                  - (IData)(1U)));
        } else {
            vlSelfRef.test_sid__DOT__uut__DOT__v3__DOT__unnamedblk3__DOT__dac_out = 0U;
        }
    }
    if (vlSelfRef.test_sid__DOT__rst) {
        __VdlySet__test_sid__DOT__uut__DOT__voice_ad__v0 = 1U;
        __VdlySet__test_sid__DOT__uut__DOT__voice_sr__v0 = 1U;
        __VdlySet__test_sid__DOT__uut__DOT__voice_vol__v0 = 1U;
        __VdlySet__test_sid__DOT__uut__DOT__voice_pw__v0 = 1U;
        __VdlySet__test_sid__DOT__uut__DOT__voice_freq__v0 = 1U;
        __Vdly__test_sid__DOT__uut__DOT__v1__DOT__adsr__DOT__state = 0U;
        __Vdly__test_sid__DOT__uut__DOT__v1__DOT__envelope = 0U;
        __Vdly__test_sid__DOT__uut__DOT__v1__DOT__adsr__DOT__unnamedblk1__DOT__hold_zero = 1U;
        __Vdly__test_sid__DOT__uut__DOT__v1__DOT__adsr__DOT__unnamedblk1__DOT__exponential_counter = 0U;
        __Vdly__test_sid__DOT__uut__DOT__v1__DOT__adsr__DOT__unnamedblk1__DOT__exponential_counter_period = 0U;
        __Vdly__test_sid__DOT__uut__DOT__v1__DOT__adsr__DOT__unnamedblk1__DOT__rate_counter = 0U;
        __Vdly__test_sid__DOT__uut__DOT__v2__DOT__adsr__DOT__state = 0U;
        __Vdly__test_sid__DOT__uut__DOT__v2__DOT__envelope = 0U;
        __Vdly__test_sid__DOT__uut__DOT__v2__DOT__adsr__DOT__unnamedblk1__DOT__hold_zero = 1U;
        __Vdly__test_sid__DOT__uut__DOT__v2__DOT__adsr__DOT__unnamedblk1__DOT__exponential_counter = 0U;
        __Vdly__test_sid__DOT__uut__DOT__v2__DOT__adsr__DOT__unnamedblk1__DOT__exponential_counter_period = 0U;
        __Vdly__test_sid__DOT__uut__DOT__v2__DOT__adsr__DOT__unnamedblk1__DOT__rate_counter = 0U;
        __Vdly__test_sid__DOT__uut__DOT__v3__DOT__adsr__DOT__state = 0U;
        __Vdly__test_sid__DOT__uut__DOT__v3__DOT__envelope = 0U;
        __Vdly__test_sid__DOT__uut__DOT__v3__DOT__adsr__DOT__unnamedblk1__DOT__hold_zero = 1U;
        __Vdly__test_sid__DOT__uut__DOT__v3__DOT__adsr__DOT__unnamedblk1__DOT__exponential_counter = 0U;
        __Vdly__test_sid__DOT__uut__DOT__v3__DOT__adsr__DOT__unnamedblk1__DOT__exponential_counter_period = 0U;
        __Vdly__test_sid__DOT__uut__DOT__v3__DOT__adsr__DOT__unnamedblk1__DOT__rate_counter = 0U;
        __VdlySet__test_sid__DOT__uut__DOT__voice_ctrl__v0 = 1U;
        vlSelfRef.test_sid__DOT__uut__DOT__last_written = 0U;
        vlSelfRef.test_sid__DOT__uut__DOT__v1__DOT__saw_tri = 0U;
        vlSelfRef.test_sid__DOT__uut__DOT__v2__DOT__saw_tri = 0U;
        vlSelfRef.test_sid__DOT__uut__DOT__v3__DOT__saw_tri = 0U;
        vlSelfRef.test_sid__DOT__uut__DOT__v1__DOT__pulse = 0U;
        vlSelfRef.test_sid__DOT__uut__DOT__v2__DOT__pulse = 0U;
        vlSelfRef.test_sid__DOT__uut__DOT__v3__DOT__pulse = 0U;
        vlSelfRef.test_sid__DOT__uut__DOT__v1__DOT__noise = 0U;
        __Vdly__test_sid__DOT__uut__DOT__v1__DOT__unnamedblk2__DOT__lfsr_noise = 0x007fffffU;
        __Vdly__test_sid__DOT__uut__DOT__v1__DOT__unnamedblk2__DOT__noise_age = 0U;
    } else {
        if (((IData)(vlSelfRef.test_sid__DOT__cs) & (IData)(vlSelfRef.test_sid__DOT__we))) {
            if (((((((((0U == (IData)(vlSelfRef.test_sid__DOT__addr)) 
                       | (1U == (IData)(vlSelfRef.test_sid__DOT__addr))) 
                      | (2U == (IData)(vlSelfRef.test_sid__DOT__addr))) 
                     | (3U == (IData)(vlSelfRef.test_sid__DOT__addr))) 
                    | (4U == (IData)(vlSelfRef.test_sid__DOT__addr))) 
                   | (5U == (IData)(vlSelfRef.test_sid__DOT__addr))) 
                  | (6U == (IData)(vlSelfRef.test_sid__DOT__addr))) 
                 | (7U == (IData)(vlSelfRef.test_sid__DOT__addr)))) {
                if ((0U != (IData)(vlSelfRef.test_sid__DOT__addr))) {
                    if ((1U != (IData)(vlSelfRef.test_sid__DOT__addr))) {
                        if ((2U != (IData)(vlSelfRef.test_sid__DOT__addr))) {
                            if ((3U != (IData)(vlSelfRef.test_sid__DOT__addr))) {
                                if ((4U != (IData)(vlSelfRef.test_sid__DOT__addr))) {
                                    if ((5U == (IData)(vlSelfRef.test_sid__DOT__addr))) {
                                        __VdlyVal__test_sid__DOT__uut__DOT__voice_ad__v3 
                                            = vlSelfRef.test_sid__DOT__din;
                                        __VdlySet__test_sid__DOT__uut__DOT__voice_ad__v3 = 1U;
                                    }
                                    if ((5U != (IData)(vlSelfRef.test_sid__DOT__addr))) {
                                        if ((6U == (IData)(vlSelfRef.test_sid__DOT__addr))) {
                                            __VdlyVal__test_sid__DOT__uut__DOT__voice_sr__v3 
                                                = vlSelfRef.test_sid__DOT__din;
                                            __VdlySet__test_sid__DOT__uut__DOT__voice_sr__v3 = 1U;
                                        }
                                    }
                                }
                                if ((4U == (IData)(vlSelfRef.test_sid__DOT__addr))) {
                                    __VdlyVal__test_sid__DOT__uut__DOT__voice_ctrl__v3 
                                        = vlSelfRef.test_sid__DOT__din;
                                    __VdlySet__test_sid__DOT__uut__DOT__voice_ctrl__v3 = 1U;
                                }
                            }
                        }
                        if ((2U == (IData)(vlSelfRef.test_sid__DOT__addr))) {
                            __VdlyVal__test_sid__DOT__uut__DOT__voice_pw__v3 
                                = vlSelfRef.test_sid__DOT__din;
                            __VdlySet__test_sid__DOT__uut__DOT__voice_pw__v3 = 1U;
                        } else if ((3U == (IData)(vlSelfRef.test_sid__DOT__addr))) {
                            __VdlyVal__test_sid__DOT__uut__DOT__voice_pw__v4 
                                = (0x0000000fU & (IData)(vlSelfRef.test_sid__DOT__din));
                            __VdlySet__test_sid__DOT__uut__DOT__voice_pw__v4 = 1U;
                        }
                    }
                }
                if ((0U == (IData)(vlSelfRef.test_sid__DOT__addr))) {
                    __VdlyVal__test_sid__DOT__uut__DOT__voice_freq__v3 
                        = vlSelfRef.test_sid__DOT__din;
                    __VdlySet__test_sid__DOT__uut__DOT__voice_freq__v3 = 1U;
                } else if ((1U == (IData)(vlSelfRef.test_sid__DOT__addr))) {
                    __VdlyVal__test_sid__DOT__uut__DOT__voice_freq__v4 
                        = vlSelfRef.test_sid__DOT__din;
                    __VdlySet__test_sid__DOT__uut__DOT__voice_freq__v4 = 1U;
                } else if ((2U != (IData)(vlSelfRef.test_sid__DOT__addr))) {
                    if ((3U != (IData)(vlSelfRef.test_sid__DOT__addr))) {
                        if ((4U != (IData)(vlSelfRef.test_sid__DOT__addr))) {
                            if ((5U != (IData)(vlSelfRef.test_sid__DOT__addr))) {
                                if ((6U != (IData)(vlSelfRef.test_sid__DOT__addr))) {
                                    __VdlyVal__test_sid__DOT__uut__DOT__voice_freq__v5 
                                        = vlSelfRef.test_sid__DOT__din;
                                    __VdlySet__test_sid__DOT__uut__DOT__voice_freq__v5 = 1U;
                                }
                            }
                        }
                    }
                }
            } else if (((((((((8U == (IData)(vlSelfRef.test_sid__DOT__addr)) 
                              | (9U == (IData)(vlSelfRef.test_sid__DOT__addr))) 
                             | (0x0aU == (IData)(vlSelfRef.test_sid__DOT__addr))) 
                            | (0x0bU == (IData)(vlSelfRef.test_sid__DOT__addr))) 
                           | (0x0cU == (IData)(vlSelfRef.test_sid__DOT__addr))) 
                          | (0x0dU == (IData)(vlSelfRef.test_sid__DOT__addr))) 
                         | (0x0eU == (IData)(vlSelfRef.test_sid__DOT__addr))) 
                        | (0x0fU == (IData)(vlSelfRef.test_sid__DOT__addr)))) {
                if ((8U != (IData)(vlSelfRef.test_sid__DOT__addr))) {
                    if ((9U != (IData)(vlSelfRef.test_sid__DOT__addr))) {
                        if ((0x0aU != (IData)(vlSelfRef.test_sid__DOT__addr))) {
                            if ((0x0bU != (IData)(vlSelfRef.test_sid__DOT__addr))) {
                                if ((0x0cU == (IData)(vlSelfRef.test_sid__DOT__addr))) {
                                    __VdlyVal__test_sid__DOT__uut__DOT__voice_ad__v4 
                                        = vlSelfRef.test_sid__DOT__din;
                                    __VdlySet__test_sid__DOT__uut__DOT__voice_ad__v4 = 1U;
                                }
                                if ((0x0cU != (IData)(vlSelfRef.test_sid__DOT__addr))) {
                                    if ((0x0dU == (IData)(vlSelfRef.test_sid__DOT__addr))) {
                                        __VdlyVal__test_sid__DOT__uut__DOT__voice_sr__v4 
                                            = vlSelfRef.test_sid__DOT__din;
                                        __VdlySet__test_sid__DOT__uut__DOT__voice_sr__v4 = 1U;
                                    }
                                }
                            }
                            if ((0x0bU == (IData)(vlSelfRef.test_sid__DOT__addr))) {
                                __VdlyVal__test_sid__DOT__uut__DOT__voice_ctrl__v4 
                                    = vlSelfRef.test_sid__DOT__din;
                                __VdlySet__test_sid__DOT__uut__DOT__voice_ctrl__v4 = 1U;
                            }
                        }
                    }
                    if ((9U == (IData)(vlSelfRef.test_sid__DOT__addr))) {
                        __VdlyVal__test_sid__DOT__uut__DOT__voice_pw__v5 
                            = vlSelfRef.test_sid__DOT__din;
                        __VdlySet__test_sid__DOT__uut__DOT__voice_pw__v5 = 1U;
                    } else if ((0x0aU == (IData)(vlSelfRef.test_sid__DOT__addr))) {
                        __VdlyVal__test_sid__DOT__uut__DOT__voice_pw__v6 
                            = (0x0000000fU & (IData)(vlSelfRef.test_sid__DOT__din));
                        __VdlySet__test_sid__DOT__uut__DOT__voice_pw__v6 = 1U;
                    }
                }
                if ((8U == (IData)(vlSelfRef.test_sid__DOT__addr))) {
                    __VdlyVal__test_sid__DOT__uut__DOT__voice_freq__v6 
                        = vlSelfRef.test_sid__DOT__din;
                    __VdlySet__test_sid__DOT__uut__DOT__voice_freq__v6 = 1U;
                } else if ((9U != (IData)(vlSelfRef.test_sid__DOT__addr))) {
                    if ((0x0aU != (IData)(vlSelfRef.test_sid__DOT__addr))) {
                        if ((0x0bU != (IData)(vlSelfRef.test_sid__DOT__addr))) {
                            if ((0x0cU != (IData)(vlSelfRef.test_sid__DOT__addr))) {
                                if ((0x0dU != (IData)(vlSelfRef.test_sid__DOT__addr))) {
                                    if ((0x0eU == (IData)(vlSelfRef.test_sid__DOT__addr))) {
                                        __VdlyVal__test_sid__DOT__uut__DOT__voice_freq__v7 
                                            = vlSelfRef.test_sid__DOT__din;
                                        __VdlySet__test_sid__DOT__uut__DOT__voice_freq__v7 = 1U;
                                    } else {
                                        __VdlyVal__test_sid__DOT__uut__DOT__voice_freq__v8 
                                            = vlSelfRef.test_sid__DOT__din;
                                        __VdlySet__test_sid__DOT__uut__DOT__voice_freq__v8 = 1U;
                                    }
                                }
                            }
                        }
                    }
                }
            } else if (((((((((0x10U == (IData)(vlSelfRef.test_sid__DOT__addr)) 
                              | (0x11U == (IData)(vlSelfRef.test_sid__DOT__addr))) 
                             | (0x12U == (IData)(vlSelfRef.test_sid__DOT__addr))) 
                            | (0x13U == (IData)(vlSelfRef.test_sid__DOT__addr))) 
                           | (0x14U == (IData)(vlSelfRef.test_sid__DOT__addr))) 
                          | (0x15U == (IData)(vlSelfRef.test_sid__DOT__addr))) 
                         | (0x16U == (IData)(vlSelfRef.test_sid__DOT__addr))) 
                        | (0x17U == (IData)(vlSelfRef.test_sid__DOT__addr)))) {
                if ((0x10U != (IData)(vlSelfRef.test_sid__DOT__addr))) {
                    if ((0x11U != (IData)(vlSelfRef.test_sid__DOT__addr))) {
                        if ((0x12U != (IData)(vlSelfRef.test_sid__DOT__addr))) {
                            if ((0x13U == (IData)(vlSelfRef.test_sid__DOT__addr))) {
                                __VdlyVal__test_sid__DOT__uut__DOT__voice_ad__v5 
                                    = vlSelfRef.test_sid__DOT__din;
                                __VdlySet__test_sid__DOT__uut__DOT__voice_ad__v5 = 1U;
                            }
                            if ((0x13U != (IData)(vlSelfRef.test_sid__DOT__addr))) {
                                if ((0x14U == (IData)(vlSelfRef.test_sid__DOT__addr))) {
                                    __VdlyVal__test_sid__DOT__uut__DOT__voice_sr__v5 
                                        = vlSelfRef.test_sid__DOT__din;
                                    __VdlySet__test_sid__DOT__uut__DOT__voice_sr__v5 = 1U;
                                }
                            }
                        }
                        if ((0x12U == (IData)(vlSelfRef.test_sid__DOT__addr))) {
                            __VdlyVal__test_sid__DOT__uut__DOT__voice_ctrl__v5 
                                = vlSelfRef.test_sid__DOT__din;
                            __VdlySet__test_sid__DOT__uut__DOT__voice_ctrl__v5 = 1U;
                        }
                    }
                }
                if ((0x10U == (IData)(vlSelfRef.test_sid__DOT__addr))) {
                    __VdlyVal__test_sid__DOT__uut__DOT__voice_pw__v7 
                        = vlSelfRef.test_sid__DOT__din;
                    __VdlySet__test_sid__DOT__uut__DOT__voice_pw__v7 = 1U;
                } else if ((0x11U == (IData)(vlSelfRef.test_sid__DOT__addr))) {
                    __VdlyVal__test_sid__DOT__uut__DOT__voice_pw__v8 
                        = (0x0000000fU & (IData)(vlSelfRef.test_sid__DOT__din));
                    __VdlySet__test_sid__DOT__uut__DOT__voice_pw__v8 = 1U;
                }
            }
            if ((1U & (~ ((((((((0U == (IData)(vlSelfRef.test_sid__DOT__addr)) 
                                | (1U == (IData)(vlSelfRef.test_sid__DOT__addr))) 
                               | (2U == (IData)(vlSelfRef.test_sid__DOT__addr))) 
                              | (3U == (IData)(vlSelfRef.test_sid__DOT__addr))) 
                             | (4U == (IData)(vlSelfRef.test_sid__DOT__addr))) 
                            | (5U == (IData)(vlSelfRef.test_sid__DOT__addr))) 
                           | (6U == (IData)(vlSelfRef.test_sid__DOT__addr))) 
                          | (7U == (IData)(vlSelfRef.test_sid__DOT__addr)))))) {
                if ((1U & (~ ((((((((8U == (IData)(vlSelfRef.test_sid__DOT__addr)) 
                                    | (9U == (IData)(vlSelfRef.test_sid__DOT__addr))) 
                                   | (0x0aU == (IData)(vlSelfRef.test_sid__DOT__addr))) 
                                  | (0x0bU == (IData)(vlSelfRef.test_sid__DOT__addr))) 
                                 | (0x0cU == (IData)(vlSelfRef.test_sid__DOT__addr))) 
                                | (0x0dU == (IData)(vlSelfRef.test_sid__DOT__addr))) 
                               | (0x0eU == (IData)(vlSelfRef.test_sid__DOT__addr))) 
                              | (0x0fU == (IData)(vlSelfRef.test_sid__DOT__addr)))))) {
                    if ((1U & (~ ((((((((0x10U == (IData)(vlSelfRef.test_sid__DOT__addr)) 
                                        | (0x11U == (IData)(vlSelfRef.test_sid__DOT__addr))) 
                                       | (0x12U == (IData)(vlSelfRef.test_sid__DOT__addr))) 
                                      | (0x13U == (IData)(vlSelfRef.test_sid__DOT__addr))) 
                                     | (0x14U == (IData)(vlSelfRef.test_sid__DOT__addr))) 
                                    | (0x15U == (IData)(vlSelfRef.test_sid__DOT__addr))) 
                                   | (0x16U == (IData)(vlSelfRef.test_sid__DOT__addr))) 
                                  | (0x17U == (IData)(vlSelfRef.test_sid__DOT__addr)))))) {
                        if ((0x18U != (IData)(vlSelfRef.test_sid__DOT__addr))) {
                            if ((0x1dU == (IData)(vlSelfRef.test_sid__DOT__addr))) {
                                __VdlyVal__test_sid__DOT__uut__DOT__voice_vol__v3 
                                    = (0x0000000fU 
                                       & (IData)(vlSelfRef.test_sid__DOT__din));
                                __VdlySet__test_sid__DOT__uut__DOT__voice_vol__v3 = 1U;
                            } else if ((0x1eU == (IData)(vlSelfRef.test_sid__DOT__addr))) {
                                __VdlyVal__test_sid__DOT__uut__DOT__voice_vol__v4 
                                    = (0x0000000fU 
                                       & (IData)(vlSelfRef.test_sid__DOT__din));
                                __VdlySet__test_sid__DOT__uut__DOT__voice_vol__v4 = 1U;
                            } else if ((0x1fU == (IData)(vlSelfRef.test_sid__DOT__addr))) {
                                __VdlyVal__test_sid__DOT__uut__DOT__voice_vol__v5 
                                    = (0x0000000fU 
                                       & (IData)(vlSelfRef.test_sid__DOT__din));
                                __VdlySet__test_sid__DOT__uut__DOT__voice_vol__v5 = 1U;
                            }
                        }
                    }
                }
            }
            vlSelfRef.test_sid__DOT__uut__DOT__last_written 
                = vlSelfRef.test_sid__DOT__din;
        }
        if (vlSelfRef.test_sid__DOT__ce_1m) {
            __Vdly__test_sid__DOT__uut__DOT__v1__DOT__adsr__DOT__unnamedblk1__DOT__rate_counter 
                = (0x00007fffU & ((IData)(1U) + (IData)(vlSelfRef.test_sid__DOT__uut__DOT__v1__DOT__adsr__DOT__unnamedblk1__DOT__rate_counter)));
            if (((IData)(vlSelfRef.test_sid__DOT__uut__DOT__v1__DOT__adsr__DOT__unnamedblk1__DOT__rate_counter) 
                 == (IData)(vlSelfRef.test_sid__DOT__uut__DOT__v1__DOT__adsr__DOT__rate_period))) {
                __Vdly__test_sid__DOT__uut__DOT__v1__DOT__adsr__DOT__unnamedblk1__DOT__rate_counter = 0U;
                __Vdly__test_sid__DOT__uut__DOT__v1__DOT__adsr__DOT__unnamedblk1__DOT__exponential_counter 
                    = (0x0000001fU & ((IData)(1U) + (IData)(vlSelfRef.test_sid__DOT__uut__DOT__v1__DOT__adsr__DOT__unnamedblk1__DOT__exponential_counter)));
                if (((1U == (IData)(vlSelfRef.test_sid__DOT__uut__DOT__v1__DOT__adsr__DOT__state)) 
                     | ((IData)(vlSelfRef.test_sid__DOT__uut__DOT__v1__DOT__adsr__DOT__unnamedblk1__DOT__exponential_counter) 
                        == (IData)(vlSelfRef.test_sid__DOT__uut__DOT__v1__DOT__adsr__DOT__unnamedblk1__DOT__exponential_counter_period)))) {
                    __Vdly__test_sid__DOT__uut__DOT__v1__DOT__adsr__DOT__unnamedblk1__DOT__exponential_counter = 0U;
                    if ((1U == (IData)(vlSelfRef.test_sid__DOT__uut__DOT__v1__DOT__adsr__DOT__state))) {
                        __Vdly__test_sid__DOT__uut__DOT__v1__DOT__envelope 
                            = (0x000000ffU & ((IData)(1U) 
                                              + (IData)(vlSelfRef.test_sid__DOT__uut__DOT__v1__DOT__envelope)));
                        if ((0xfeU == (IData)(vlSelfRef.test_sid__DOT__uut__DOT__v1__DOT__envelope))) {
                            __Vdly__test_sid__DOT__uut__DOT__v1__DOT__adsr__DOT__state = 2U;
                        }
                    } else if ((2U == (IData)(vlSelfRef.test_sid__DOT__uut__DOT__v1__DOT__adsr__DOT__state))) {
                        if ((((IData)(vlSelfRef.test_sid__DOT__uut__DOT__v1__DOT__envelope) 
                              != (0x000000ffU & ((0x000000f0U 
                                                  & vlSelfRef.test_sid__DOT__uut__DOT__voice_sr[0U]) 
                                                 | (0x0000000fU 
                                                    & (vlSelfRef.test_sid__DOT__uut__DOT__voice_sr[0U] 
                                                       >> 4U))))) 
                             & (~ (IData)(vlSelfRef.test_sid__DOT__uut__DOT__v1__DOT__adsr__DOT__unnamedblk1__DOT__hold_zero)))) {
                            __Vdly__test_sid__DOT__uut__DOT__v1__DOT__envelope 
                                = (0x000000ffU & ((IData)(vlSelfRef.test_sid__DOT__uut__DOT__v1__DOT__envelope) 
                                                  - (IData)(1U)));
                        }
                    } else if ((0U == (IData)(vlSelfRef.test_sid__DOT__uut__DOT__v1__DOT__adsr__DOT__state))) {
                        if ((1U & (~ (IData)(vlSelfRef.test_sid__DOT__uut__DOT__v1__DOT__adsr__DOT__unnamedblk1__DOT__hold_zero)))) {
                            __Vdly__test_sid__DOT__uut__DOT__v1__DOT__envelope 
                                = (0x000000ffU & ((IData)(vlSelfRef.test_sid__DOT__uut__DOT__v1__DOT__envelope) 
                                                  - (IData)(1U)));
                        }
                    }
                    if (((1U != (IData)(vlSelfRef.test_sid__DOT__uut__DOT__v1__DOT__adsr__DOT__state)) 
                         & (1U == (IData)(vlSelfRef.test_sid__DOT__uut__DOT__v1__DOT__envelope)))) {
                        __Vdly__test_sid__DOT__uut__DOT__v1__DOT__adsr__DOT__unnamedblk1__DOT__hold_zero = 1U;
                    }
                }
            }
            if ((1U & ((~ (IData)(vlSelfRef.test_sid__DOT__uut__DOT__v1__DOT__adsr__DOT__unnamedblk1__DOT__gate_edge)) 
                       & vlSelfRef.test_sid__DOT__uut__DOT__voice_ctrl[0U]))) {
                __Vdly__test_sid__DOT__uut__DOT__v1__DOT__adsr__DOT__state = 1U;
                __Vdly__test_sid__DOT__uut__DOT__v1__DOT__adsr__DOT__unnamedblk1__DOT__hold_zero = 0U;
            }
            if (((IData)(vlSelfRef.test_sid__DOT__uut__DOT__v1__DOT__adsr__DOT__unnamedblk1__DOT__gate_edge) 
                 & (~ vlSelfRef.test_sid__DOT__uut__DOT__voice_ctrl[0U]))) {
                __Vdly__test_sid__DOT__uut__DOT__v1__DOT__adsr__DOT__state = 0U;
            }
            __Vdly__test_sid__DOT__uut__DOT__v2__DOT__adsr__DOT__unnamedblk1__DOT__rate_counter 
                = (0x00007fffU & ((IData)(1U) + (IData)(vlSelfRef.test_sid__DOT__uut__DOT__v2__DOT__adsr__DOT__unnamedblk1__DOT__rate_counter)));
            if (((IData)(vlSelfRef.test_sid__DOT__uut__DOT__v2__DOT__adsr__DOT__unnamedblk1__DOT__rate_counter) 
                 == (IData)(vlSelfRef.test_sid__DOT__uut__DOT__v2__DOT__adsr__DOT__rate_period))) {
                __Vdly__test_sid__DOT__uut__DOT__v2__DOT__adsr__DOT__unnamedblk1__DOT__rate_counter = 0U;
                __Vdly__test_sid__DOT__uut__DOT__v2__DOT__adsr__DOT__unnamedblk1__DOT__exponential_counter 
                    = (0x0000001fU & ((IData)(1U) + (IData)(vlSelfRef.test_sid__DOT__uut__DOT__v2__DOT__adsr__DOT__unnamedblk1__DOT__exponential_counter)));
                if (((1U == (IData)(vlSelfRef.test_sid__DOT__uut__DOT__v2__DOT__adsr__DOT__state)) 
                     | ((IData)(vlSelfRef.test_sid__DOT__uut__DOT__v2__DOT__adsr__DOT__unnamedblk1__DOT__exponential_counter) 
                        == (IData)(vlSelfRef.test_sid__DOT__uut__DOT__v2__DOT__adsr__DOT__unnamedblk1__DOT__exponential_counter_period)))) {
                    __Vdly__test_sid__DOT__uut__DOT__v2__DOT__adsr__DOT__unnamedblk1__DOT__exponential_counter = 0U;
                    if ((1U == (IData)(vlSelfRef.test_sid__DOT__uut__DOT__v2__DOT__adsr__DOT__state))) {
                        __Vdly__test_sid__DOT__uut__DOT__v2__DOT__envelope 
                            = (0x000000ffU & ((IData)(1U) 
                                              + (IData)(vlSelfRef.test_sid__DOT__uut__DOT__v2__DOT__envelope)));
                        if ((0xfeU == (IData)(vlSelfRef.test_sid__DOT__uut__DOT__v2__DOT__envelope))) {
                            __Vdly__test_sid__DOT__uut__DOT__v2__DOT__adsr__DOT__state = 2U;
                        }
                    } else if ((2U == (IData)(vlSelfRef.test_sid__DOT__uut__DOT__v2__DOT__adsr__DOT__state))) {
                        if ((((IData)(vlSelfRef.test_sid__DOT__uut__DOT__v2__DOT__envelope) 
                              != (0x000000ffU & ((0x000000f0U 
                                                  & vlSelfRef.test_sid__DOT__uut__DOT__voice_sr[1U]) 
                                                 | (0x0000000fU 
                                                    & (vlSelfRef.test_sid__DOT__uut__DOT__voice_sr[1U] 
                                                       >> 4U))))) 
                             & (~ (IData)(vlSelfRef.test_sid__DOT__uut__DOT__v2__DOT__adsr__DOT__unnamedblk1__DOT__hold_zero)))) {
                            __Vdly__test_sid__DOT__uut__DOT__v2__DOT__envelope 
                                = (0x000000ffU & ((IData)(vlSelfRef.test_sid__DOT__uut__DOT__v2__DOT__envelope) 
                                                  - (IData)(1U)));
                        }
                    } else if ((0U == (IData)(vlSelfRef.test_sid__DOT__uut__DOT__v2__DOT__adsr__DOT__state))) {
                        if ((1U & (~ (IData)(vlSelfRef.test_sid__DOT__uut__DOT__v2__DOT__adsr__DOT__unnamedblk1__DOT__hold_zero)))) {
                            __Vdly__test_sid__DOT__uut__DOT__v2__DOT__envelope 
                                = (0x000000ffU & ((IData)(vlSelfRef.test_sid__DOT__uut__DOT__v2__DOT__envelope) 
                                                  - (IData)(1U)));
                        }
                    }
                    if (((1U != (IData)(vlSelfRef.test_sid__DOT__uut__DOT__v2__DOT__adsr__DOT__state)) 
                         & (1U == (IData)(vlSelfRef.test_sid__DOT__uut__DOT__v2__DOT__envelope)))) {
                        __Vdly__test_sid__DOT__uut__DOT__v2__DOT__adsr__DOT__unnamedblk1__DOT__hold_zero = 1U;
                    }
                }
            }
            if ((1U & ((~ (IData)(vlSelfRef.test_sid__DOT__uut__DOT__v2__DOT__adsr__DOT__unnamedblk1__DOT__gate_edge)) 
                       & vlSelfRef.test_sid__DOT__uut__DOT__voice_ctrl[1U]))) {
                __Vdly__test_sid__DOT__uut__DOT__v2__DOT__adsr__DOT__state = 1U;
                __Vdly__test_sid__DOT__uut__DOT__v2__DOT__adsr__DOT__unnamedblk1__DOT__hold_zero = 0U;
            }
            if (((IData)(vlSelfRef.test_sid__DOT__uut__DOT__v2__DOT__adsr__DOT__unnamedblk1__DOT__gate_edge) 
                 & (~ vlSelfRef.test_sid__DOT__uut__DOT__voice_ctrl[1U]))) {
                __Vdly__test_sid__DOT__uut__DOT__v2__DOT__adsr__DOT__state = 0U;
            }
            __Vdly__test_sid__DOT__uut__DOT__v3__DOT__adsr__DOT__unnamedblk1__DOT__rate_counter 
                = (0x00007fffU & ((IData)(1U) + (IData)(vlSelfRef.test_sid__DOT__uut__DOT__v3__DOT__adsr__DOT__unnamedblk1__DOT__rate_counter)));
            if (((IData)(vlSelfRef.test_sid__DOT__uut__DOT__v3__DOT__adsr__DOT__unnamedblk1__DOT__rate_counter) 
                 == (IData)(vlSelfRef.test_sid__DOT__uut__DOT__v3__DOT__adsr__DOT__rate_period))) {
                __Vdly__test_sid__DOT__uut__DOT__v3__DOT__adsr__DOT__unnamedblk1__DOT__rate_counter = 0U;
                __Vdly__test_sid__DOT__uut__DOT__v3__DOT__adsr__DOT__unnamedblk1__DOT__exponential_counter 
                    = (0x0000001fU & ((IData)(1U) + (IData)(vlSelfRef.test_sid__DOT__uut__DOT__v3__DOT__adsr__DOT__unnamedblk1__DOT__exponential_counter)));
                if (((1U == (IData)(vlSelfRef.test_sid__DOT__uut__DOT__v3__DOT__adsr__DOT__state)) 
                     | ((IData)(vlSelfRef.test_sid__DOT__uut__DOT__v3__DOT__adsr__DOT__unnamedblk1__DOT__exponential_counter) 
                        == (IData)(vlSelfRef.test_sid__DOT__uut__DOT__v3__DOT__adsr__DOT__unnamedblk1__DOT__exponential_counter_period)))) {
                    __Vdly__test_sid__DOT__uut__DOT__v3__DOT__adsr__DOT__unnamedblk1__DOT__exponential_counter = 0U;
                    if ((1U == (IData)(vlSelfRef.test_sid__DOT__uut__DOT__v3__DOT__adsr__DOT__state))) {
                        __Vdly__test_sid__DOT__uut__DOT__v3__DOT__envelope 
                            = (0x000000ffU & ((IData)(1U) 
                                              + (IData)(vlSelfRef.test_sid__DOT__uut__DOT__v3__DOT__envelope)));
                        if ((0xfeU == (IData)(vlSelfRef.test_sid__DOT__uut__DOT__v3__DOT__envelope))) {
                            __Vdly__test_sid__DOT__uut__DOT__v3__DOT__adsr__DOT__state = 2U;
                        }
                    } else if ((2U == (IData)(vlSelfRef.test_sid__DOT__uut__DOT__v3__DOT__adsr__DOT__state))) {
                        if ((((IData)(vlSelfRef.test_sid__DOT__uut__DOT__v3__DOT__envelope) 
                              != (0x000000ffU & ((0x000000f0U 
                                                  & vlSelfRef.test_sid__DOT__uut__DOT__voice_sr[2U]) 
                                                 | (0x0000000fU 
                                                    & (vlSelfRef.test_sid__DOT__uut__DOT__voice_sr[2U] 
                                                       >> 4U))))) 
                             & (~ (IData)(vlSelfRef.test_sid__DOT__uut__DOT__v3__DOT__adsr__DOT__unnamedblk1__DOT__hold_zero)))) {
                            __Vdly__test_sid__DOT__uut__DOT__v3__DOT__envelope 
                                = (0x000000ffU & ((IData)(vlSelfRef.test_sid__DOT__uut__DOT__v3__DOT__envelope) 
                                                  - (IData)(1U)));
                        }
                    } else if ((0U == (IData)(vlSelfRef.test_sid__DOT__uut__DOT__v3__DOT__adsr__DOT__state))) {
                        if ((1U & (~ (IData)(vlSelfRef.test_sid__DOT__uut__DOT__v3__DOT__adsr__DOT__unnamedblk1__DOT__hold_zero)))) {
                            __Vdly__test_sid__DOT__uut__DOT__v3__DOT__envelope 
                                = (0x000000ffU & ((IData)(vlSelfRef.test_sid__DOT__uut__DOT__v3__DOT__envelope) 
                                                  - (IData)(1U)));
                        }
                    }
                    if (((1U != (IData)(vlSelfRef.test_sid__DOT__uut__DOT__v3__DOT__adsr__DOT__state)) 
                         & (1U == (IData)(vlSelfRef.test_sid__DOT__uut__DOT__v3__DOT__envelope)))) {
                        __Vdly__test_sid__DOT__uut__DOT__v3__DOT__adsr__DOT__unnamedblk1__DOT__hold_zero = 1U;
                    }
                }
            }
            if ((1U & ((~ (IData)(vlSelfRef.test_sid__DOT__uut__DOT__v3__DOT__adsr__DOT__unnamedblk1__DOT__gate_edge)) 
                       & vlSelfRef.test_sid__DOT__uut__DOT__voice_ctrl[2U]))) {
                __Vdly__test_sid__DOT__uut__DOT__v3__DOT__adsr__DOT__state = 1U;
                __Vdly__test_sid__DOT__uut__DOT__v3__DOT__adsr__DOT__unnamedblk1__DOT__hold_zero = 0U;
            }
            if (((IData)(vlSelfRef.test_sid__DOT__uut__DOT__v3__DOT__adsr__DOT__unnamedblk1__DOT__gate_edge) 
                 & (~ vlSelfRef.test_sid__DOT__uut__DOT__voice_ctrl[2U]))) {
                __Vdly__test_sid__DOT__uut__DOT__v3__DOT__adsr__DOT__state = 0U;
            }
            vlSelfRef.test_sid__DOT__uut__DOT__v1__DOT__saw_tri 
                = vlSelfRef.test_sid__DOT__uut__DOT____Vcellout__v1__acc_t;
            vlSelfRef.test_sid__DOT__uut__DOT__v2__DOT__saw_tri 
                = vlSelfRef.test_sid__DOT__uut__DOT____Vcellout__v2__acc_t;
            vlSelfRef.test_sid__DOT__uut__DOT__v3__DOT__saw_tri 
                = vlSelfRef.test_sid__DOT__uut__DOT____Vcellout__v3__acc_t;
            vlSelfRef.test_sid__DOT__uut__DOT__v1__DOT__pulse 
                = (1U & ((vlSelfRef.test_sid__DOT__uut__DOT__voice_ctrl[0U] 
                          >> 3U) | ((0x00000fffU & 
                                     (vlSelfRef.test_sid__DOT__uut__DOT__v1__DOT__oscillator 
                                      >> 0x0cU)) >= vlSelfRef.test_sid__DOT__uut__DOT__voice_pw[0U])));
            vlSelfRef.test_sid__DOT__uut__DOT__v2__DOT__pulse 
                = (1U & ((vlSelfRef.test_sid__DOT__uut__DOT__voice_ctrl[1U] 
                          >> 3U) | ((0x00000fffU & 
                                     (vlSelfRef.test_sid__DOT__uut__DOT__v2__DOT__oscillator 
                                      >> 0x0cU)) >= vlSelfRef.test_sid__DOT__uut__DOT__voice_pw[1U])));
            vlSelfRef.test_sid__DOT__uut__DOT__v3__DOT__pulse 
                = (1U & ((vlSelfRef.test_sid__DOT__uut__DOT__voice_ctrl[2U] 
                          >> 3U) | ((0x00000fffU & 
                                     (vlSelfRef.test_sid__DOT__uut__DOT__v3__DOT__oscillator 
                                      >> 0x0cU)) >= vlSelfRef.test_sid__DOT__uut__DOT__voice_pw[2U])));
            vlSelfRef.test_sid__DOT__uut__DOT__v1__DOT__noise 
                = (((((4U & (vlSelfRef.test_sid__DOT__uut__DOT__v1__DOT__unnamedblk2__DOT__lfsr_noise 
                             >> 0x00000012U)) | ((2U 
                                                  & (vlSelfRef.test_sid__DOT__uut__DOT__v1__DOT__unnamedblk2__DOT__lfsr_noise 
                                                     >> 0x00000011U)) 
                                                 | (1U 
                                                    & (vlSelfRef.test_sid__DOT__uut__DOT__v1__DOT__unnamedblk2__DOT__lfsr_noise 
                                                       >> 0x0eU)))) 
                     << 9U) | (((4U & (vlSelfRef.test_sid__DOT__uut__DOT__v1__DOT__unnamedblk2__DOT__lfsr_noise 
                                       >> 9U)) | ((2U 
                                                   & (vlSelfRef.test_sid__DOT__uut__DOT__v1__DOT__unnamedblk2__DOT__lfsr_noise 
                                                      >> 8U)) 
                                                  | (1U 
                                                     & (vlSelfRef.test_sid__DOT__uut__DOT__v1__DOT__unnamedblk2__DOT__lfsr_noise 
                                                        >> 5U)))) 
                               << 6U)) | ((0x00000020U 
                                           & (vlSelfRef.test_sid__DOT__uut__DOT__v1__DOT__unnamedblk2__DOT__lfsr_noise 
                                              << 3U)) 
                                          | (0x00000010U 
                                             & (vlSelfRef.test_sid__DOT__uut__DOT__v1__DOT__unnamedblk2__DOT__lfsr_noise 
                                                << 4U))));
            if (vlSelfRef.test_sid__DOT__uut__DOT__v1__DOT__unnamedblk2__DOT__clk) {
                if (((IData)(vlSelfRef.test_sid__DOT__uut__DOT__v1__DOT__unnamedblk2__DOT__clk) 
                     & (~ (IData)(vlSelfRef.test_sid__DOT__uut__DOT__v1__DOT__unnamedblk2__DOT__clk_d)))) {
                    __Vdly__test_sid__DOT__uut__DOT__v1__DOT__unnamedblk2__DOT__lfsr_noise 
                        = ((0x007ffffeU & (vlSelfRef.test_sid__DOT__uut__DOT__v1__DOT__unnamedblk2__DOT__lfsr_noise 
                                           << 1U)) 
                           | (1U & ((((IData)(vlSelfRef.test_sid__DOT__rst) 
                                      | (vlSelfRef.test_sid__DOT__uut__DOT__voice_ctrl[0U] 
                                         >> 3U)) | 
                                     (vlSelfRef.test_sid__DOT__uut__DOT__v1__DOT__unnamedblk2__DOT__lfsr_noise 
                                      >> 0x16U)) ^ 
                                    (vlSelfRef.test_sid__DOT__uut__DOT__v1__DOT__unnamedblk2__DOT__lfsr_noise 
                                     >> 0x11U))));
                } else if (((vlSelfRef.test_sid__DOT__uut__DOT__voice_ctrl[0U] 
                             >> 7U) & (0U != (7U & 
                                              (vlSelfRef.test_sid__DOT__uut__DOT__voice_ctrl[0U] 
                                               >> 4U))))) {
                    __Vdly__test_sid__DOT__uut__DOT__v1__DOT__unnamedblk2__DOT__lfsr_noise 
                        = ((0x006fffffU & __Vdly__test_sid__DOT__uut__DOT__v1__DOT__unnamedblk2__DOT__lfsr_noise) 
                           | ((IData)(((vlSelfRef.test_sid__DOT__uut__DOT__v1__DOT__unnamedblk2__DOT__lfsr_noise 
                                        >> 0x00000014U) 
                                       & ((IData)(vlSelfRef.test_sid__DOT__uut__DOT__v1__DOT__wave_out) 
                                          >> 7U))) 
                              << 0x00000014U));
                    __Vdly__test_sid__DOT__uut__DOT__v1__DOT__unnamedblk2__DOT__lfsr_noise 
                        = ((0x007bffffU & __Vdly__test_sid__DOT__uut__DOT__v1__DOT__unnamedblk2__DOT__lfsr_noise) 
                           | (0x00040000U & (vlSelfRef.test_sid__DOT__uut__DOT__v1__DOT__unnamedblk2__DOT__lfsr_noise 
                                             & ((IData)(vlSelfRef.test_sid__DOT__uut__DOT__v1__DOT__wave_out) 
                                                << 0x0000000cU))));
                    __Vdly__test_sid__DOT__uut__DOT__v1__DOT__unnamedblk2__DOT__lfsr_noise 
                        = ((0x007fbfffU & __Vdly__test_sid__DOT__uut__DOT__v1__DOT__unnamedblk2__DOT__lfsr_noise) 
                           | (0x00004000U & (vlSelfRef.test_sid__DOT__uut__DOT__v1__DOT__unnamedblk2__DOT__lfsr_noise 
                                             & ((IData)(vlSelfRef.test_sid__DOT__uut__DOT__v1__DOT__wave_out) 
                                                << 9U))));
                    __Vdly__test_sid__DOT__uut__DOT__v1__DOT__unnamedblk2__DOT__lfsr_noise 
                        = ((0x007ff7ffU & __Vdly__test_sid__DOT__uut__DOT__v1__DOT__unnamedblk2__DOT__lfsr_noise) 
                           | (0x00000800U & (vlSelfRef.test_sid__DOT__uut__DOT__v1__DOT__unnamedblk2__DOT__lfsr_noise 
                                             & ((IData)(vlSelfRef.test_sid__DOT__uut__DOT__v1__DOT__wave_out) 
                                                << 7U))));
                    __Vdly__test_sid__DOT__uut__DOT__v1__DOT__unnamedblk2__DOT__lfsr_noise 
                        = ((0x007ffdffU & __Vdly__test_sid__DOT__uut__DOT__v1__DOT__unnamedblk2__DOT__lfsr_noise) 
                           | (0x00000200U & (vlSelfRef.test_sid__DOT__uut__DOT__v1__DOT__unnamedblk2__DOT__lfsr_noise 
                                             & ((IData)(vlSelfRef.test_sid__DOT__uut__DOT__v1__DOT__wave_out) 
                                                << 6U))));
                    __Vdly__test_sid__DOT__uut__DOT__v1__DOT__unnamedblk2__DOT__lfsr_noise 
                        = ((0x007fffdfU & __Vdly__test_sid__DOT__uut__DOT__v1__DOT__unnamedblk2__DOT__lfsr_noise) 
                           | (0x00000020U & (vlSelfRef.test_sid__DOT__uut__DOT__v1__DOT__unnamedblk2__DOT__lfsr_noise 
                                             & ((IData)(vlSelfRef.test_sid__DOT__uut__DOT__v1__DOT__wave_out) 
                                                << 3U))));
                    __Vdly__test_sid__DOT__uut__DOT__v1__DOT__unnamedblk2__DOT__lfsr_noise 
                        = ((0x007ffffbU & __Vdly__test_sid__DOT__uut__DOT__v1__DOT__unnamedblk2__DOT__lfsr_noise) 
                           | (4U & (vlSelfRef.test_sid__DOT__uut__DOT__v1__DOT__unnamedblk2__DOT__lfsr_noise 
                                    & ((IData)(vlSelfRef.test_sid__DOT__uut__DOT__v1__DOT__wave_out) 
                                       << 1U))));
                    __Vdly__test_sid__DOT__uut__DOT__v1__DOT__unnamedblk2__DOT__lfsr_noise 
                        = ((0x007ffffeU & __Vdly__test_sid__DOT__uut__DOT__v1__DOT__unnamedblk2__DOT__lfsr_noise) 
                           | (1U & (vlSelfRef.test_sid__DOT__uut__DOT__v1__DOT__unnamedblk2__DOT__lfsr_noise 
                                    & (IData)(vlSelfRef.test_sid__DOT__uut__DOT__v1__DOT__wave_out))));
                }
                __Vdly__test_sid__DOT__uut__DOT__v1__DOT__unnamedblk2__DOT__noise_age = 0U;
                vlSelfRef.test_sid__DOT__uut__DOT__v1__DOT__unnamedblk2__DOT__clk_d = 1U;
            } else {
                if ((vlSelfRef.test_sid__DOT__uut__DOT__v1__DOT__unnamedblk2__DOT__noise_age 
                     >= ((IData)(vlSelfRef.test_sid__DOT__mode)
                          ? 0x00950000U : 0x00008000U))) {
                    vlSelfRef.test_sid__DOT__uut__DOT__v1__DOT__noise = 0x0fffU;
                } else {
                    __Vdly__test_sid__DOT__uut__DOT__v1__DOT__unnamedblk2__DOT__noise_age 
                        = (0x00ffffffU & ((IData)(1U) 
                                          + vlSelfRef.test_sid__DOT__uut__DOT__v1__DOT__unnamedblk2__DOT__noise_age));
                }
                vlSelfRef.test_sid__DOT__uut__DOT__v1__DOT__unnamedblk2__DOT__clk_d = 0U;
            }
        }
    }
    if (vlSelfRef.test_sid__DOT__ce_1m) {
        vlSelfRef.test_sid__DOT__uut__DOT__v1__DOT__wave_out 
            = (0x000000ffU & (((IData)(vlSelfRef.test_sid__DOT__uut__DOT__v1__DOT__norm) 
                               >> 4U) | (IData)(vlSelfRef.test_sid__DOT__uut__DOT__v1__DOT__comb)));
    }
    if (vlSelfRef.test_sid__DOT__rst) {
        vlSelfRef.test_sid__DOT__uut__DOT__v2__DOT__noise = 0U;
        __Vdly__test_sid__DOT__uut__DOT__v2__DOT__unnamedblk2__DOT__lfsr_noise = 0x007fffffU;
        __Vdly__test_sid__DOT__uut__DOT__v2__DOT__unnamedblk2__DOT__noise_age = 0U;
    } else if (vlSelfRef.test_sid__DOT__ce_1m) {
        vlSelfRef.test_sid__DOT__uut__DOT__v2__DOT__noise 
            = (((((4U & (vlSelfRef.test_sid__DOT__uut__DOT__v2__DOT__unnamedblk2__DOT__lfsr_noise 
                         >> 0x00000012U)) | ((2U & 
                                              (vlSelfRef.test_sid__DOT__uut__DOT__v2__DOT__unnamedblk2__DOT__lfsr_noise 
                                               >> 0x00000011U)) 
                                             | (1U 
                                                & (vlSelfRef.test_sid__DOT__uut__DOT__v2__DOT__unnamedblk2__DOT__lfsr_noise 
                                                   >> 0x0eU)))) 
                 << 9U) | (((4U & (vlSelfRef.test_sid__DOT__uut__DOT__v2__DOT__unnamedblk2__DOT__lfsr_noise 
                                   >> 9U)) | ((2U & 
                                               (vlSelfRef.test_sid__DOT__uut__DOT__v2__DOT__unnamedblk2__DOT__lfsr_noise 
                                                >> 8U)) 
                                              | (1U 
                                                 & (vlSelfRef.test_sid__DOT__uut__DOT__v2__DOT__unnamedblk2__DOT__lfsr_noise 
                                                    >> 5U)))) 
                           << 6U)) | ((0x00000020U 
                                       & (vlSelfRef.test_sid__DOT__uut__DOT__v2__DOT__unnamedblk2__DOT__lfsr_noise 
                                          << 3U)) | 
                                      (0x00000010U 
                                       & (vlSelfRef.test_sid__DOT__uut__DOT__v2__DOT__unnamedblk2__DOT__lfsr_noise 
                                          << 4U))));
        if (vlSelfRef.test_sid__DOT__uut__DOT__v2__DOT__unnamedblk2__DOT__clk) {
            if (((IData)(vlSelfRef.test_sid__DOT__uut__DOT__v2__DOT__unnamedblk2__DOT__clk) 
                 & (~ (IData)(vlSelfRef.test_sid__DOT__uut__DOT__v2__DOT__unnamedblk2__DOT__clk_d)))) {
                __Vdly__test_sid__DOT__uut__DOT__v2__DOT__unnamedblk2__DOT__lfsr_noise 
                    = ((0x007ffffeU & (vlSelfRef.test_sid__DOT__uut__DOT__v2__DOT__unnamedblk2__DOT__lfsr_noise 
                                       << 1U)) | (1U 
                                                  & ((((IData)(vlSelfRef.test_sid__DOT__rst) 
                                                       | (vlSelfRef.test_sid__DOT__uut__DOT__voice_ctrl[1U] 
                                                          >> 3U)) 
                                                      | (vlSelfRef.test_sid__DOT__uut__DOT__v2__DOT__unnamedblk2__DOT__lfsr_noise 
                                                         >> 0x16U)) 
                                                     ^ 
                                                     (vlSelfRef.test_sid__DOT__uut__DOT__v2__DOT__unnamedblk2__DOT__lfsr_noise 
                                                      >> 0x11U))));
            } else if (((vlSelfRef.test_sid__DOT__uut__DOT__voice_ctrl[1U] 
                         >> 7U) & (0U != (7U & (vlSelfRef.test_sid__DOT__uut__DOT__voice_ctrl[1U] 
                                                >> 4U))))) {
                __Vdly__test_sid__DOT__uut__DOT__v2__DOT__unnamedblk2__DOT__lfsr_noise 
                    = ((0x006fffffU & __Vdly__test_sid__DOT__uut__DOT__v2__DOT__unnamedblk2__DOT__lfsr_noise) 
                       | ((IData)(((vlSelfRef.test_sid__DOT__uut__DOT__v2__DOT__unnamedblk2__DOT__lfsr_noise 
                                    >> 0x00000014U) 
                                   & ((IData)(vlSelfRef.test_sid__DOT__uut__DOT__v2__DOT__wave_out) 
                                      >> 7U))) << 0x00000014U));
                __Vdly__test_sid__DOT__uut__DOT__v2__DOT__unnamedblk2__DOT__lfsr_noise 
                    = ((0x007bffffU & __Vdly__test_sid__DOT__uut__DOT__v2__DOT__unnamedblk2__DOT__lfsr_noise) 
                       | (0x00040000U & (vlSelfRef.test_sid__DOT__uut__DOT__v2__DOT__unnamedblk2__DOT__lfsr_noise 
                                         & ((IData)(vlSelfRef.test_sid__DOT__uut__DOT__v2__DOT__wave_out) 
                                            << 0x0000000cU))));
                __Vdly__test_sid__DOT__uut__DOT__v2__DOT__unnamedblk2__DOT__lfsr_noise 
                    = ((0x007fbfffU & __Vdly__test_sid__DOT__uut__DOT__v2__DOT__unnamedblk2__DOT__lfsr_noise) 
                       | (0x00004000U & (vlSelfRef.test_sid__DOT__uut__DOT__v2__DOT__unnamedblk2__DOT__lfsr_noise 
                                         & ((IData)(vlSelfRef.test_sid__DOT__uut__DOT__v2__DOT__wave_out) 
                                            << 9U))));
                __Vdly__test_sid__DOT__uut__DOT__v2__DOT__unnamedblk2__DOT__lfsr_noise 
                    = ((0x007ff7ffU & __Vdly__test_sid__DOT__uut__DOT__v2__DOT__unnamedblk2__DOT__lfsr_noise) 
                       | (0x00000800U & (vlSelfRef.test_sid__DOT__uut__DOT__v2__DOT__unnamedblk2__DOT__lfsr_noise 
                                         & ((IData)(vlSelfRef.test_sid__DOT__uut__DOT__v2__DOT__wave_out) 
                                            << 7U))));
                __Vdly__test_sid__DOT__uut__DOT__v2__DOT__unnamedblk2__DOT__lfsr_noise 
                    = ((0x007ffdffU & __Vdly__test_sid__DOT__uut__DOT__v2__DOT__unnamedblk2__DOT__lfsr_noise) 
                       | (0x00000200U & (vlSelfRef.test_sid__DOT__uut__DOT__v2__DOT__unnamedblk2__DOT__lfsr_noise 
                                         & ((IData)(vlSelfRef.test_sid__DOT__uut__DOT__v2__DOT__wave_out) 
                                            << 6U))));
                __Vdly__test_sid__DOT__uut__DOT__v2__DOT__unnamedblk2__DOT__lfsr_noise 
                    = ((0x007fffdfU & __Vdly__test_sid__DOT__uut__DOT__v2__DOT__unnamedblk2__DOT__lfsr_noise) 
                       | (0x00000020U & (vlSelfRef.test_sid__DOT__uut__DOT__v2__DOT__unnamedblk2__DOT__lfsr_noise 
                                         & ((IData)(vlSelfRef.test_sid__DOT__uut__DOT__v2__DOT__wave_out) 
                                            << 3U))));
                __Vdly__test_sid__DOT__uut__DOT__v2__DOT__unnamedblk2__DOT__lfsr_noise 
                    = ((0x007ffffbU & __Vdly__test_sid__DOT__uut__DOT__v2__DOT__unnamedblk2__DOT__lfsr_noise) 
                       | (4U & (vlSelfRef.test_sid__DOT__uut__DOT__v2__DOT__unnamedblk2__DOT__lfsr_noise 
                                & ((IData)(vlSelfRef.test_sid__DOT__uut__DOT__v2__DOT__wave_out) 
                                   << 1U))));
                __Vdly__test_sid__DOT__uut__DOT__v2__DOT__unnamedblk2__DOT__lfsr_noise 
                    = ((0x007ffffeU & __Vdly__test_sid__DOT__uut__DOT__v2__DOT__unnamedblk2__DOT__lfsr_noise) 
                       | (1U & (vlSelfRef.test_sid__DOT__uut__DOT__v2__DOT__unnamedblk2__DOT__lfsr_noise 
                                & (IData)(vlSelfRef.test_sid__DOT__uut__DOT__v2__DOT__wave_out))));
            }
            __Vdly__test_sid__DOT__uut__DOT__v2__DOT__unnamedblk2__DOT__noise_age = 0U;
            vlSelfRef.test_sid__DOT__uut__DOT__v2__DOT__unnamedblk2__DOT__clk_d = 1U;
        } else {
            if ((vlSelfRef.test_sid__DOT__uut__DOT__v2__DOT__unnamedblk2__DOT__noise_age 
                 >= ((IData)(vlSelfRef.test_sid__DOT__mode)
                      ? 0x00950000U : 0x00008000U))) {
                vlSelfRef.test_sid__DOT__uut__DOT__v2__DOT__noise = 0x0fffU;
            } else {
                __Vdly__test_sid__DOT__uut__DOT__v2__DOT__unnamedblk2__DOT__noise_age 
                    = (0x00ffffffU & ((IData)(1U) + vlSelfRef.test_sid__DOT__uut__DOT__v2__DOT__unnamedblk2__DOT__noise_age));
            }
            vlSelfRef.test_sid__DOT__uut__DOT__v2__DOT__unnamedblk2__DOT__clk_d = 0U;
        }
    }
    if (vlSelfRef.test_sid__DOT__ce_1m) {
        vlSelfRef.test_sid__DOT__uut__DOT__v2__DOT__wave_out 
            = (0x000000ffU & (((IData)(vlSelfRef.test_sid__DOT__uut__DOT__v2__DOT__norm) 
                               >> 4U) | (IData)(vlSelfRef.test_sid__DOT__uut__DOT__v2__DOT__comb)));
    }
    if (vlSelfRef.test_sid__DOT__rst) {
        vlSelfRef.test_sid__DOT__uut__DOT__v3__DOT__noise = 0U;
        __Vdly__test_sid__DOT__uut__DOT__v3__DOT__unnamedblk2__DOT__lfsr_noise = 0x007fffffU;
        __Vdly__test_sid__DOT__uut__DOT__v3__DOT__unnamedblk2__DOT__noise_age = 0U;
    } else if (vlSelfRef.test_sid__DOT__ce_1m) {
        vlSelfRef.test_sid__DOT__uut__DOT__v3__DOT__noise 
            = (((((4U & (vlSelfRef.test_sid__DOT__uut__DOT__v3__DOT__unnamedblk2__DOT__lfsr_noise 
                         >> 0x00000012U)) | ((2U & 
                                              (vlSelfRef.test_sid__DOT__uut__DOT__v3__DOT__unnamedblk2__DOT__lfsr_noise 
                                               >> 0x00000011U)) 
                                             | (1U 
                                                & (vlSelfRef.test_sid__DOT__uut__DOT__v3__DOT__unnamedblk2__DOT__lfsr_noise 
                                                   >> 0x0eU)))) 
                 << 9U) | (((4U & (vlSelfRef.test_sid__DOT__uut__DOT__v3__DOT__unnamedblk2__DOT__lfsr_noise 
                                   >> 9U)) | ((2U & 
                                               (vlSelfRef.test_sid__DOT__uut__DOT__v3__DOT__unnamedblk2__DOT__lfsr_noise 
                                                >> 8U)) 
                                              | (1U 
                                                 & (vlSelfRef.test_sid__DOT__uut__DOT__v3__DOT__unnamedblk2__DOT__lfsr_noise 
                                                    >> 5U)))) 
                           << 6U)) | ((0x00000020U 
                                       & (vlSelfRef.test_sid__DOT__uut__DOT__v3__DOT__unnamedblk2__DOT__lfsr_noise 
                                          << 3U)) | 
                                      (0x00000010U 
                                       & (vlSelfRef.test_sid__DOT__uut__DOT__v3__DOT__unnamedblk2__DOT__lfsr_noise 
                                          << 4U))));
        if (vlSelfRef.test_sid__DOT__uut__DOT__v3__DOT__unnamedblk2__DOT__clk) {
            if (((IData)(vlSelfRef.test_sid__DOT__uut__DOT__v3__DOT__unnamedblk2__DOT__clk) 
                 & (~ (IData)(vlSelfRef.test_sid__DOT__uut__DOT__v3__DOT__unnamedblk2__DOT__clk_d)))) {
                __Vdly__test_sid__DOT__uut__DOT__v3__DOT__unnamedblk2__DOT__lfsr_noise 
                    = ((0x007ffffeU & (vlSelfRef.test_sid__DOT__uut__DOT__v3__DOT__unnamedblk2__DOT__lfsr_noise 
                                       << 1U)) | (1U 
                                                  & ((((IData)(vlSelfRef.test_sid__DOT__rst) 
                                                       | (vlSelfRef.test_sid__DOT__uut__DOT__voice_ctrl[2U] 
                                                          >> 3U)) 
                                                      | (vlSelfRef.test_sid__DOT__uut__DOT__v3__DOT__unnamedblk2__DOT__lfsr_noise 
                                                         >> 0x16U)) 
                                                     ^ 
                                                     (vlSelfRef.test_sid__DOT__uut__DOT__v3__DOT__unnamedblk2__DOT__lfsr_noise 
                                                      >> 0x11U))));
            } else if (((vlSelfRef.test_sid__DOT__uut__DOT__voice_ctrl[2U] 
                         >> 7U) & (0U != (7U & (vlSelfRef.test_sid__DOT__uut__DOT__voice_ctrl[2U] 
                                                >> 4U))))) {
                __Vdly__test_sid__DOT__uut__DOT__v3__DOT__unnamedblk2__DOT__lfsr_noise 
                    = ((0x006fffffU & __Vdly__test_sid__DOT__uut__DOT__v3__DOT__unnamedblk2__DOT__lfsr_noise) 
                       | ((IData)(((vlSelfRef.test_sid__DOT__uut__DOT__v3__DOT__unnamedblk2__DOT__lfsr_noise 
                                    >> 0x00000014U) 
                                   & ((IData)(vlSelfRef.test_sid__DOT__uut__DOT__v3__DOT__wave_out) 
                                      >> 7U))) << 0x00000014U));
                __Vdly__test_sid__DOT__uut__DOT__v3__DOT__unnamedblk2__DOT__lfsr_noise 
                    = ((0x007bffffU & __Vdly__test_sid__DOT__uut__DOT__v3__DOT__unnamedblk2__DOT__lfsr_noise) 
                       | (0x00040000U & (vlSelfRef.test_sid__DOT__uut__DOT__v3__DOT__unnamedblk2__DOT__lfsr_noise 
                                         & ((IData)(vlSelfRef.test_sid__DOT__uut__DOT__v3__DOT__wave_out) 
                                            << 0x0000000cU))));
                __Vdly__test_sid__DOT__uut__DOT__v3__DOT__unnamedblk2__DOT__lfsr_noise 
                    = ((0x007fbfffU & __Vdly__test_sid__DOT__uut__DOT__v3__DOT__unnamedblk2__DOT__lfsr_noise) 
                       | (0x00004000U & (vlSelfRef.test_sid__DOT__uut__DOT__v3__DOT__unnamedblk2__DOT__lfsr_noise 
                                         & ((IData)(vlSelfRef.test_sid__DOT__uut__DOT__v3__DOT__wave_out) 
                                            << 9U))));
                __Vdly__test_sid__DOT__uut__DOT__v3__DOT__unnamedblk2__DOT__lfsr_noise 
                    = ((0x007ff7ffU & __Vdly__test_sid__DOT__uut__DOT__v3__DOT__unnamedblk2__DOT__lfsr_noise) 
                       | (0x00000800U & (vlSelfRef.test_sid__DOT__uut__DOT__v3__DOT__unnamedblk2__DOT__lfsr_noise 
                                         & ((IData)(vlSelfRef.test_sid__DOT__uut__DOT__v3__DOT__wave_out) 
                                            << 7U))));
                __Vdly__test_sid__DOT__uut__DOT__v3__DOT__unnamedblk2__DOT__lfsr_noise 
                    = ((0x007ffdffU & __Vdly__test_sid__DOT__uut__DOT__v3__DOT__unnamedblk2__DOT__lfsr_noise) 
                       | (0x00000200U & (vlSelfRef.test_sid__DOT__uut__DOT__v3__DOT__unnamedblk2__DOT__lfsr_noise 
                                         & ((IData)(vlSelfRef.test_sid__DOT__uut__DOT__v3__DOT__wave_out) 
                                            << 6U))));
                __Vdly__test_sid__DOT__uut__DOT__v3__DOT__unnamedblk2__DOT__lfsr_noise 
                    = ((0x007fffdfU & __Vdly__test_sid__DOT__uut__DOT__v3__DOT__unnamedblk2__DOT__lfsr_noise) 
                       | (0x00000020U & (vlSelfRef.test_sid__DOT__uut__DOT__v3__DOT__unnamedblk2__DOT__lfsr_noise 
                                         & ((IData)(vlSelfRef.test_sid__DOT__uut__DOT__v3__DOT__wave_out) 
                                            << 3U))));
                __Vdly__test_sid__DOT__uut__DOT__v3__DOT__unnamedblk2__DOT__lfsr_noise 
                    = ((0x007ffffbU & __Vdly__test_sid__DOT__uut__DOT__v3__DOT__unnamedblk2__DOT__lfsr_noise) 
                       | (4U & (vlSelfRef.test_sid__DOT__uut__DOT__v3__DOT__unnamedblk2__DOT__lfsr_noise 
                                & ((IData)(vlSelfRef.test_sid__DOT__uut__DOT__v3__DOT__wave_out) 
                                   << 1U))));
                __Vdly__test_sid__DOT__uut__DOT__v3__DOT__unnamedblk2__DOT__lfsr_noise 
                    = ((0x007ffffeU & __Vdly__test_sid__DOT__uut__DOT__v3__DOT__unnamedblk2__DOT__lfsr_noise) 
                       | (1U & (vlSelfRef.test_sid__DOT__uut__DOT__v3__DOT__unnamedblk2__DOT__lfsr_noise 
                                & (IData)(vlSelfRef.test_sid__DOT__uut__DOT__v3__DOT__wave_out))));
            }
            __Vdly__test_sid__DOT__uut__DOT__v3__DOT__unnamedblk2__DOT__noise_age = 0U;
            vlSelfRef.test_sid__DOT__uut__DOT__v3__DOT__unnamedblk2__DOT__clk_d = 1U;
        } else {
            if ((vlSelfRef.test_sid__DOT__uut__DOT__v3__DOT__unnamedblk2__DOT__noise_age 
                 >= ((IData)(vlSelfRef.test_sid__DOT__mode)
                      ? 0x00950000U : 0x00008000U))) {
                vlSelfRef.test_sid__DOT__uut__DOT__v3__DOT__noise = 0x0fffU;
            } else {
                __Vdly__test_sid__DOT__uut__DOT__v3__DOT__unnamedblk2__DOT__noise_age 
                    = (0x00ffffffU & ((IData)(1U) + vlSelfRef.test_sid__DOT__uut__DOT__v3__DOT__unnamedblk2__DOT__noise_age));
            }
            vlSelfRef.test_sid__DOT__uut__DOT__v3__DOT__unnamedblk2__DOT__clk_d = 0U;
        }
    }
    if (vlSelfRef.test_sid__DOT__ce_1m) {
        vlSelfRef.test_sid__DOT__uut__DOT__v3__DOT__wave_out 
            = (0x000000ffU & (((IData)(vlSelfRef.test_sid__DOT__uut__DOT__v3__DOT__norm) 
                               >> 4U) | (IData)(vlSelfRef.test_sid__DOT__uut__DOT__v3__DOT__comb)));
    }
    if ((6U == (IData)(vlSelfRef.test_sid__DOT__uut__DOT__tbl_state))) {
        vlSelfRef.test_sid__DOT__audio_out = (0x0003ffffU 
                                              & (vlSelfRef.test_sid__DOT__uut__DOT__flt__DOT__o 
                                                 >> 2U));
    }
    if ((2U == (7U & (IData)(vlSelfRef.test_sid__DOT__uut__DOT__tbl_state)))) {
        vlSelfRef.test_sid__DOT__uut__DOT__flt__DOT__s = 1U;
        vlSelfRef.test_sid__DOT__uut__DOT__flt__DOT__a 
            = (0x0000ffffU & ((IData)(vlSelfRef.test_sid__DOT__mode)
                               ? (((IData)(vlSelfRef.test_sid__DOT__uut__DOT__filter_fc) 
                                   << 2U) + (IData)(vlSelfRef.test_sid__DOT__uut__DOT__filter_fc))
                               : VL_SHIFTR_III(16,16,32, (IData)(vlSelfRef.test_sid__DOT__uut__DOT__tbl__DOT__f0), 1U)));
        vlSelfRef.test_sid__DOT__uut__DOT__flt__DOT__c = 0U;
        vlSelfRef.test_sid__DOT__uut__DOT__flt__DOT__b 
            = vlSelfRef.test_sid__DOT__uut__DOT__flt__DOT__vbp;
        vlSelfRef.test_sid__DOT__uut__DOT__flt__DOT__unnamedblk1__DOT___1_Q_lsl10 
            = vlSelfRef.test_sid__DOT__uut__DOT__flt__DOT___1_Q_lsl10_tbl
            [(((IData)(vlSelfRef.test_sid__DOT__mode) 
               << 4U) | (0x0000000fU & ((IData)(vlSelfRef.test_sid__DOT__uut__DOT__filter_res_filt) 
                                        >> 4U)))];
        vlSelfRef.test_sid__DOT__uut__DOT__flt__DOT__unnamedblk1__DOT__vi 
            = (0x0000ffffU & ((((((1U & (IData)(vlSelfRef.test_sid__DOT__uut__DOT__filter_res_filt))
                                   ? VL_EXTENDS_II(24,22, vlSelfRef.test_sid__DOT__uut__DOT__v1_scaled)
                                   : 0U) + ((2U & (IData)(vlSelfRef.test_sid__DOT__uut__DOT__filter_res_filt))
                                             ? VL_EXTENDS_II(24,22, vlSelfRef.test_sid__DOT__uut__DOT__v2_scaled)
                                             : 0U)) 
                                + ((4U & (IData)(vlSelfRef.test_sid__DOT__uut__DOT__filter_res_filt))
                                    ? VL_EXTENDS_II(24,22, vlSelfRef.test_sid__DOT__uut__DOT__v3_scaled)
                                    : 0U)) + ((8U & (IData)(vlSelfRef.test_sid__DOT__uut__DOT__filter_res_filt))
                                               ? 0U
                                               : 0U)) 
                              >> 7U));
        vlSelfRef.test_sid__DOT__uut__DOT__flt__DOT__unnamedblk1__DOT__vd 
            = (0x0000ffffU & (((((((IData)(vlSelfRef.test_sid__DOT__mode)
                                    ? 0U : 0x00ff1c72U) 
                                  + ((1U & (IData)(vlSelfRef.test_sid__DOT__uut__DOT__filter_res_filt))
                                      ? 0U : VL_EXTENDS_II(24,22, vlSelfRef.test_sid__DOT__uut__DOT__v1_scaled))) 
                                 + ((2U & (IData)(vlSelfRef.test_sid__DOT__uut__DOT__filter_res_filt))
                                     ? 0U : VL_EXTENDS_II(24,22, vlSelfRef.test_sid__DOT__uut__DOT__v2_scaled))) 
                                + ((1U & (((IData)(vlSelfRef.test_sid__DOT__uut__DOT__filter_res_filt) 
                                           >> 2U) | 
                                          ((IData)(vlSelfRef.test_sid__DOT__uut__DOT__filter_mode_vol) 
                                           >> 7U)))
                                    ? 0U : VL_EXTENDS_II(24,22, vlSelfRef.test_sid__DOT__uut__DOT__v3_scaled))) 
                               + ((8U & (IData)(vlSelfRef.test_sid__DOT__uut__DOT__filter_res_filt))
                                   ? 0U : 0U)) >> 7U));
    } else {
        if ((3U == (7U & (IData)(vlSelfRef.test_sid__DOT__uut__DOT__tbl_state)))) {
            vlSelfRef.test_sid__DOT__uut__DOT__flt__DOT__s = 1U;
            vlSelfRef.test_sid__DOT__uut__DOT__flt__DOT__vlp 
                = vlSelfRef.test_sid__DOT__uut__DOT__flt__DOT__vlp2;
            vlSelfRef.test_sid__DOT__uut__DOT__flt__DOT__c = 0U;
            vlSelfRef.test_sid__DOT__uut__DOT__flt__DOT__b 
                = vlSelfRef.test_sid__DOT__uut__DOT__flt__DOT__vhp;
        } else if ((4U == (7U & (IData)(vlSelfRef.test_sid__DOT__uut__DOT__tbl_state)))) {
            vlSelfRef.test_sid__DOT__uut__DOT__flt__DOT__s = 0U;
            vlSelfRef.test_sid__DOT__uut__DOT__flt__DOT__vbp 
                = vlSelfRef.test_sid__DOT__uut__DOT__flt__DOT__vbp2;
            vlSelfRef.test_sid__DOT__uut__DOT__flt__DOT__c 
                = VL_SHIFTL_III(32,32,32, (- (VL_EXTENDS_II(32,16, (IData)(vlSelfRef.test_sid__DOT__uut__DOT__flt__DOT__vlp2)) 
                                              + VL_EXTENDS_II(32,16, (IData)(vlSelfRef.test_sid__DOT__uut__DOT__flt__DOT__unnamedblk1__DOT__vi)))), 0x0000000aU);
            vlSelfRef.test_sid__DOT__uut__DOT__flt__DOT__b 
                = vlSelfRef.test_sid__DOT__uut__DOT__flt__DOT__vbp_next;
        } else if ((5U == (7U & (IData)(vlSelfRef.test_sid__DOT__uut__DOT__tbl_state)))) {
            vlSelfRef.test_sid__DOT__uut__DOT__flt__DOT__s = 0U;
            __Vfunc_test_sid__DOT__uut__DOT__flt__DOT__clamp__19__x 
                = (0x0001ffffU & (((VL_EXTENDS_II(17,16, (IData)(vlSelfRef.test_sid__DOT__uut__DOT__flt__DOT__unnamedblk1__DOT__vd)) 
                                    + ((0x00000010U 
                                        & (IData)(vlSelfRef.test_sid__DOT__uut__DOT__filter_mode_vol))
                                        ? VL_EXTENDS_II(17,16, (IData)(vlSelfRef.test_sid__DOT__uut__DOT__flt__DOT__vlp2))
                                        : 0U)) + ((0x00000020U 
                                                   & (IData)(vlSelfRef.test_sid__DOT__uut__DOT__filter_mode_vol))
                                                   ? 
                                                  VL_EXTENDS_II(17,16, (IData)(vlSelfRef.test_sid__DOT__uut__DOT__flt__DOT__vbp2))
                                                   : 0U)) 
                                  + ((0x00000040U & (IData)(vlSelfRef.test_sid__DOT__uut__DOT__filter_mode_vol))
                                      ? VL_EXTENDS_II(17,16, (IData)(vlSelfRef.test_sid__DOT__uut__DOT__flt__DOT__vhp_next))
                                      : 0U)));
            __Vfunc_test_sid__DOT__uut__DOT__flt__DOT__clamp__19__Vfuncout 
                = (0x0000ffffU & ((1U & VL_REDXOR_32(
                                                     (3U 
                                                      & (__Vfunc_test_sid__DOT__uut__DOT__flt__DOT__clamp__19__x 
                                                         >> 0x0fU))))
                                   ? ((0x00008000U 
                                       & (__Vfunc_test_sid__DOT__uut__DOT__flt__DOT__clamp__19__x 
                                          >> 1U)) | 
                                      (0x00007fffU 
                                       & (- (IData)(
                                                    (1U 
                                                     & (__Vfunc_test_sid__DOT__uut__DOT__flt__DOT__clamp__19__x 
                                                        >> 0x0fU))))))
                                   : __Vfunc_test_sid__DOT__uut__DOT__flt__DOT__clamp__19__x));
            vlSelfRef.test_sid__DOT__uut__DOT__flt__DOT__c = 0U;
            vlSelfRef.test_sid__DOT__uut__DOT__flt__DOT__b 
                = __Vfunc_test_sid__DOT__uut__DOT__flt__DOT__clamp__19__Vfuncout;
        }
        if ((3U != (7U & (IData)(vlSelfRef.test_sid__DOT__uut__DOT__tbl_state)))) {
            if ((4U == (7U & (IData)(vlSelfRef.test_sid__DOT__uut__DOT__tbl_state)))) {
                vlSelfRef.test_sid__DOT__uut__DOT__flt__DOT__a 
                    = vlSelfRef.test_sid__DOT__uut__DOT__flt__DOT__unnamedblk1__DOT___1_Q_lsl10;
            } else if ((5U == (7U & (IData)(vlSelfRef.test_sid__DOT__uut__DOT__tbl_state)))) {
                vlSelfRef.test_sid__DOT__uut__DOT__flt__DOT__a 
                    = (0x0000000fU & (IData)(vlSelfRef.test_sid__DOT__uut__DOT__filter_mode_vol));
            }
        }
    }
    if (__VdlySet__test_sid__DOT__uut__DOT__voice_ad__v0) {
        vlSelfRef.test_sid__DOT__uut__DOT__voice_ad[0U] = 0U;
        vlSelfRef.test_sid__DOT__uut__DOT__voice_ad[1U] = 0U;
        vlSelfRef.test_sid__DOT__uut__DOT__voice_ad[2U] = 0U;
    }
    if (__VdlySet__test_sid__DOT__uut__DOT__voice_ad__v3) {
        vlSelfRef.test_sid__DOT__uut__DOT__voice_ad[0U] 
            = __VdlyVal__test_sid__DOT__uut__DOT__voice_ad__v3;
    }
    if (__VdlySet__test_sid__DOT__uut__DOT__voice_ad__v4) {
        vlSelfRef.test_sid__DOT__uut__DOT__voice_ad[1U] 
            = __VdlyVal__test_sid__DOT__uut__DOT__voice_ad__v4;
    }
    if (__VdlySet__test_sid__DOT__uut__DOT__voice_ad__v5) {
        vlSelfRef.test_sid__DOT__uut__DOT__voice_ad[2U] 
            = __VdlyVal__test_sid__DOT__uut__DOT__voice_ad__v5;
    }
    if (__VdlySet__test_sid__DOT__uut__DOT__voice_vol__v0) {
        vlSelfRef.test_sid__DOT__uut__DOT__voice_vol[0U] = 0x0fU;
        vlSelfRef.test_sid__DOT__uut__DOT__voice_vol[1U] = 0x0fU;
        vlSelfRef.test_sid__DOT__uut__DOT__voice_vol[2U] = 0x0fU;
    }
    if (__VdlySet__test_sid__DOT__uut__DOT__voice_vol__v3) {
        vlSelfRef.test_sid__DOT__uut__DOT__voice_vol[0U] 
            = __VdlyVal__test_sid__DOT__uut__DOT__voice_vol__v3;
    }
    if (__VdlySet__test_sid__DOT__uut__DOT__voice_vol__v4) {
        vlSelfRef.test_sid__DOT__uut__DOT__voice_vol[1U] 
            = __VdlyVal__test_sid__DOT__uut__DOT__voice_vol__v4;
    }
    if (__VdlySet__test_sid__DOT__uut__DOT__voice_vol__v5) {
        vlSelfRef.test_sid__DOT__uut__DOT__voice_vol[2U] 
            = __VdlyVal__test_sid__DOT__uut__DOT__voice_vol__v5;
    }
    if (__VdlySet__test_sid__DOT__uut__DOT___st_out__v0) {
        vlSelfRef.test_sid__DOT__uut__DOT___st_out[__VdlyDim0__test_sid__DOT__uut__DOT___st_out__v0] 
            = __VdlyVal__test_sid__DOT__uut__DOT___st_out__v0;
    }
    if (__VdlySet__test_sid__DOT__uut__DOT__p_t_out__v0) {
        vlSelfRef.test_sid__DOT__uut__DOT__p_t_out[__VdlyDim0__test_sid__DOT__uut__DOT__p_t_out__v0] 
            = __VdlyVal__test_sid__DOT__uut__DOT__p_t_out__v0;
    }
    if (__VdlySet__test_sid__DOT__uut__DOT__ps___05Fout__v0) {
        vlSelfRef.test_sid__DOT__uut__DOT__ps___05Fout[__VdlyDim0__test_sid__DOT__uut__DOT__ps___05Fout__v0] 
            = __VdlyVal__test_sid__DOT__uut__DOT__ps___05Fout__v0;
    }
    if (__VdlySet__test_sid__DOT__uut__DOT__pst_out__v0) {
        vlSelfRef.test_sid__DOT__uut__DOT__pst_out[__VdlyDim0__test_sid__DOT__uut__DOT__pst_out__v0] 
            = __VdlyVal__test_sid__DOT__uut__DOT__pst_out__v0;
    }
    if (__VdlySet__test_sid__DOT__uut__DOT__voice_freq__v0) {
        vlSelfRef.test_sid__DOT__uut__DOT__voice_freq[0U] = 0U;
        vlSelfRef.test_sid__DOT__uut__DOT__voice_freq[1U] = 0U;
        vlSelfRef.test_sid__DOT__uut__DOT__voice_freq[2U] = 0U;
    }
    if (__VdlySet__test_sid__DOT__uut__DOT__voice_freq__v3) {
        vlSelfRef.test_sid__DOT__uut__DOT__voice_freq[0U] 
            = ((0xff00U & vlSelfRef.test_sid__DOT__uut__DOT__voice_freq[0U]) 
               | (IData)(__VdlyVal__test_sid__DOT__uut__DOT__voice_freq__v3));
    }
    if (__VdlySet__test_sid__DOT__uut__DOT__voice_freq__v4) {
        vlSelfRef.test_sid__DOT__uut__DOT__voice_freq[0U] 
            = ((0x00ffU & vlSelfRef.test_sid__DOT__uut__DOT__voice_freq[0U]) 
               | ((IData)(__VdlyVal__test_sid__DOT__uut__DOT__voice_freq__v4) 
                  << 8U));
    }
    if (__VdlySet__test_sid__DOT__uut__DOT__voice_freq__v5) {
        vlSelfRef.test_sid__DOT__uut__DOT__voice_freq[1U] 
            = ((0xff00U & vlSelfRef.test_sid__DOT__uut__DOT__voice_freq[1U]) 
               | (IData)(__VdlyVal__test_sid__DOT__uut__DOT__voice_freq__v5));
    }
    if (__VdlySet__test_sid__DOT__uut__DOT__voice_freq__v6) {
        vlSelfRef.test_sid__DOT__uut__DOT__voice_freq[1U] 
            = ((0x00ffU & vlSelfRef.test_sid__DOT__uut__DOT__voice_freq[1U]) 
               | ((IData)(__VdlyVal__test_sid__DOT__uut__DOT__voice_freq__v6) 
                  << 8U));
    }
    if (__VdlySet__test_sid__DOT__uut__DOT__voice_freq__v7) {
        vlSelfRef.test_sid__DOT__uut__DOT__voice_freq[2U] 
            = ((0xff00U & vlSelfRef.test_sid__DOT__uut__DOT__voice_freq[2U]) 
               | (IData)(__VdlyVal__test_sid__DOT__uut__DOT__voice_freq__v7));
    }
    if (__VdlySet__test_sid__DOT__uut__DOT__voice_freq__v8) {
        vlSelfRef.test_sid__DOT__uut__DOT__voice_freq[2U] 
            = ((0x00ffU & vlSelfRef.test_sid__DOT__uut__DOT__voice_freq[2U]) 
               | ((IData)(__VdlyVal__test_sid__DOT__uut__DOT__voice_freq__v8) 
                  << 8U));
    }
    vlSelfRef.test_sid__DOT__uut__DOT__v1__DOT__adsr__DOT__unnamedblk1__DOT__exponential_counter_period 
        = __Vdly__test_sid__DOT__uut__DOT__v1__DOT__adsr__DOT__unnamedblk1__DOT__exponential_counter_period;
    vlSelfRef.test_sid__DOT__uut__DOT__v1__DOT__adsr__DOT__unnamedblk1__DOT__hold_zero 
        = __Vdly__test_sid__DOT__uut__DOT__v1__DOT__adsr__DOT__unnamedblk1__DOT__hold_zero;
    vlSelfRef.test_sid__DOT__uut__DOT__v1__DOT__adsr__DOT__unnamedblk1__DOT__exponential_counter 
        = __Vdly__test_sid__DOT__uut__DOT__v1__DOT__adsr__DOT__unnamedblk1__DOT__exponential_counter;
    vlSelfRef.test_sid__DOT__uut__DOT__v1__DOT__adsr__DOT__unnamedblk1__DOT__rate_counter 
        = __Vdly__test_sid__DOT__uut__DOT__v1__DOT__adsr__DOT__unnamedblk1__DOT__rate_counter;
    vlSelfRef.test_sid__DOT__uut__DOT__v1__DOT__adsr__DOT__state 
        = __Vdly__test_sid__DOT__uut__DOT__v1__DOT__adsr__DOT__state;
    vlSelfRef.test_sid__DOT__uut__DOT__v2__DOT__adsr__DOT__unnamedblk1__DOT__exponential_counter_period 
        = __Vdly__test_sid__DOT__uut__DOT__v2__DOT__adsr__DOT__unnamedblk1__DOT__exponential_counter_period;
    vlSelfRef.test_sid__DOT__uut__DOT__v2__DOT__adsr__DOT__unnamedblk1__DOT__hold_zero 
        = __Vdly__test_sid__DOT__uut__DOT__v2__DOT__adsr__DOT__unnamedblk1__DOT__hold_zero;
    vlSelfRef.test_sid__DOT__uut__DOT__v2__DOT__adsr__DOT__unnamedblk1__DOT__exponential_counter 
        = __Vdly__test_sid__DOT__uut__DOT__v2__DOT__adsr__DOT__unnamedblk1__DOT__exponential_counter;
    vlSelfRef.test_sid__DOT__uut__DOT__v2__DOT__adsr__DOT__unnamedblk1__DOT__rate_counter 
        = __Vdly__test_sid__DOT__uut__DOT__v2__DOT__adsr__DOT__unnamedblk1__DOT__rate_counter;
    vlSelfRef.test_sid__DOT__uut__DOT__v2__DOT__adsr__DOT__state 
        = __Vdly__test_sid__DOT__uut__DOT__v2__DOT__adsr__DOT__state;
    vlSelfRef.test_sid__DOT__uut__DOT__v3__DOT__adsr__DOT__unnamedblk1__DOT__exponential_counter_period 
        = __Vdly__test_sid__DOT__uut__DOT__v3__DOT__adsr__DOT__unnamedblk1__DOT__exponential_counter_period;
    vlSelfRef.test_sid__DOT__uut__DOT__v3__DOT__adsr__DOT__unnamedblk1__DOT__hold_zero 
        = __Vdly__test_sid__DOT__uut__DOT__v3__DOT__adsr__DOT__unnamedblk1__DOT__hold_zero;
    vlSelfRef.test_sid__DOT__uut__DOT__v3__DOT__adsr__DOT__unnamedblk1__DOT__exponential_counter 
        = __Vdly__test_sid__DOT__uut__DOT__v3__DOT__adsr__DOT__unnamedblk1__DOT__exponential_counter;
    vlSelfRef.test_sid__DOT__uut__DOT__v3__DOT__adsr__DOT__unnamedblk1__DOT__rate_counter 
        = __Vdly__test_sid__DOT__uut__DOT__v3__DOT__adsr__DOT__unnamedblk1__DOT__rate_counter;
    vlSelfRef.test_sid__DOT__uut__DOT__v3__DOT__adsr__DOT__state 
        = __Vdly__test_sid__DOT__uut__DOT__v3__DOT__adsr__DOT__state;
    if (__VdlySet__test_sid__DOT__uut__DOT__voice_sr__v0) {
        vlSelfRef.test_sid__DOT__uut__DOT__voice_sr[0U] = 0U;
        vlSelfRef.test_sid__DOT__uut__DOT__voice_sr[1U] = 0U;
        vlSelfRef.test_sid__DOT__uut__DOT__voice_sr[2U] = 0U;
    }
    if (__VdlySet__test_sid__DOT__uut__DOT__voice_sr__v3) {
        vlSelfRef.test_sid__DOT__uut__DOT__voice_sr[0U] 
            = __VdlyVal__test_sid__DOT__uut__DOT__voice_sr__v3;
    }
    if (__VdlySet__test_sid__DOT__uut__DOT__voice_sr__v4) {
        vlSelfRef.test_sid__DOT__uut__DOT__voice_sr[1U] 
            = __VdlyVal__test_sid__DOT__uut__DOT__voice_sr__v4;
    }
    if (__VdlySet__test_sid__DOT__uut__DOT__voice_sr__v5) {
        vlSelfRef.test_sid__DOT__uut__DOT__voice_sr[2U] 
            = __VdlyVal__test_sid__DOT__uut__DOT__voice_sr__v5;
    }
    if (__VdlySet__test_sid__DOT__uut__DOT__voice_pw__v0) {
        vlSelfRef.test_sid__DOT__uut__DOT__voice_pw[0U] = 0U;
        vlSelfRef.test_sid__DOT__uut__DOT__voice_pw[1U] = 0U;
        vlSelfRef.test_sid__DOT__uut__DOT__voice_pw[2U] = 0U;
    }
    if (__VdlySet__test_sid__DOT__uut__DOT__voice_pw__v3) {
        vlSelfRef.test_sid__DOT__uut__DOT__voice_pw[0U] 
            = ((0x0f00U & vlSelfRef.test_sid__DOT__uut__DOT__voice_pw[0U]) 
               | (IData)(__VdlyVal__test_sid__DOT__uut__DOT__voice_pw__v3));
    }
    if (__VdlySet__test_sid__DOT__uut__DOT__voice_pw__v4) {
        vlSelfRef.test_sid__DOT__uut__DOT__voice_pw[0U] 
            = ((0x00ffU & vlSelfRef.test_sid__DOT__uut__DOT__voice_pw[0U]) 
               | ((IData)(__VdlyVal__test_sid__DOT__uut__DOT__voice_pw__v4) 
                  << 8U));
    }
    if (__VdlySet__test_sid__DOT__uut__DOT__voice_pw__v5) {
        vlSelfRef.test_sid__DOT__uut__DOT__voice_pw[1U] 
            = ((0x0f00U & vlSelfRef.test_sid__DOT__uut__DOT__voice_pw[1U]) 
               | (IData)(__VdlyVal__test_sid__DOT__uut__DOT__voice_pw__v5));
    }
    if (__VdlySet__test_sid__DOT__uut__DOT__voice_pw__v6) {
        vlSelfRef.test_sid__DOT__uut__DOT__voice_pw[1U] 
            = ((0x00ffU & vlSelfRef.test_sid__DOT__uut__DOT__voice_pw[1U]) 
               | ((IData)(__VdlyVal__test_sid__DOT__uut__DOT__voice_pw__v6) 
                  << 8U));
    }
    if (__VdlySet__test_sid__DOT__uut__DOT__voice_pw__v7) {
        vlSelfRef.test_sid__DOT__uut__DOT__voice_pw[2U] 
            = ((0x0f00U & vlSelfRef.test_sid__DOT__uut__DOT__voice_pw[2U]) 
               | (IData)(__VdlyVal__test_sid__DOT__uut__DOT__voice_pw__v7));
    }
    if (__VdlySet__test_sid__DOT__uut__DOT__voice_pw__v8) {
        vlSelfRef.test_sid__DOT__uut__DOT__voice_pw[2U] 
            = ((0x00ffU & vlSelfRef.test_sid__DOT__uut__DOT__voice_pw[2U]) 
               | ((IData)(__VdlyVal__test_sid__DOT__uut__DOT__voice_pw__v8) 
                  << 8U));
    }
    vlSelfRef.test_sid__DOT__uut__DOT__v1__DOT__unnamedblk2__DOT__lfsr_noise 
        = __Vdly__test_sid__DOT__uut__DOT__v1__DOT__unnamedblk2__DOT__lfsr_noise;
    vlSelfRef.test_sid__DOT__uut__DOT__v1__DOT__unnamedblk2__DOT__noise_age 
        = __Vdly__test_sid__DOT__uut__DOT__v1__DOT__unnamedblk2__DOT__noise_age;
    vlSelfRef.test_sid__DOT__uut__DOT__v2__DOT__unnamedblk2__DOT__lfsr_noise 
        = __Vdly__test_sid__DOT__uut__DOT__v2__DOT__unnamedblk2__DOT__lfsr_noise;
    vlSelfRef.test_sid__DOT__uut__DOT__v2__DOT__unnamedblk2__DOT__noise_age 
        = __Vdly__test_sid__DOT__uut__DOT__v2__DOT__unnamedblk2__DOT__noise_age;
    vlSelfRef.test_sid__DOT__uut__DOT__v3__DOT__unnamedblk2__DOT__lfsr_noise 
        = __Vdly__test_sid__DOT__uut__DOT__v3__DOT__unnamedblk2__DOT__lfsr_noise;
    vlSelfRef.test_sid__DOT__uut__DOT__v3__DOT__unnamedblk2__DOT__noise_age 
        = __Vdly__test_sid__DOT__uut__DOT__v3__DOT__unnamedblk2__DOT__noise_age;
    if (vlSelfRef.test_sid__DOT__mode) {
        vlSelfRef.test_sid__DOT__uut__DOT__f_p_t_out 
            = vlSelfRef.test_sid__DOT__uut__DOT__tbl__DOT__wave8580_p_t
            [(0x000007ffU & (IData)(vlSelfRef.test_sid__DOT__uut__DOT__f_acc_t))];
        vlSelfRef.test_sid__DOT__uut__DOT__f_ps___05Fout 
            = vlSelfRef.test_sid__DOT__uut__DOT__tbl__DOT__wave8580_ps_
            [vlSelfRef.test_sid__DOT__uut__DOT__f_acc_t];
        vlSelfRef.test_sid__DOT__uut__DOT__v1__DOT__norm_dac 
            = (0x00000fffU & (IData)(vlSelfRef.test_sid__DOT__uut__DOT__v1__DOT__norm));
        vlSelfRef.test_sid__DOT__uut__DOT__v2__DOT__norm_dac 
            = (0x00000fffU & (IData)(vlSelfRef.test_sid__DOT__uut__DOT__v2__DOT__norm));
        vlSelfRef.test_sid__DOT__uut__DOT__v3__DOT__norm_dac 
            = (0x00000fffU & (IData)(vlSelfRef.test_sid__DOT__uut__DOT__v3__DOT__norm));
    } else {
        vlSelfRef.test_sid__DOT__uut__DOT__f_p_t_out 
            = vlSelfRef.test_sid__DOT__uut__DOT__tbl__DOT__wave6581_p_t
            [(0x000007ffU & (IData)(vlSelfRef.test_sid__DOT__uut__DOT__f_acc_t))];
        vlSelfRef.test_sid__DOT__uut__DOT__f_ps___05Fout 
            = vlSelfRef.test_sid__DOT__uut__DOT__tbl__DOT__wave6581_ps_
            [(0x000007ffU & (IData)(vlSelfRef.test_sid__DOT__uut__DOT__f_acc_t))];
        vlSelfRef.test_sid__DOT__uut__DOT__v1__DOT__norm_dac 
            = (0x00000fffU & (vlSelfRef.test_sid__DOT__uut__DOT__v1__DOT__waveform_dac__DOT__bitsum[11U] 
                              >> 4U));
        vlSelfRef.test_sid__DOT__uut__DOT__v2__DOT__norm_dac 
            = (0x00000fffU & (vlSelfRef.test_sid__DOT__uut__DOT__v2__DOT__waveform_dac__DOT__bitsum[11U] 
                              >> 4U));
        vlSelfRef.test_sid__DOT__uut__DOT__v3__DOT__norm_dac 
            = (0x00000fffU & (vlSelfRef.test_sid__DOT__uut__DOT__v3__DOT__waveform_dac__DOT__bitsum[11U] 
                              >> 4U));
    }
    __Vtableidx1 = (((IData)(vlSelfRef.test_sid__DOT__uut__DOT__f_acc_t) 
                     << 1U) | (IData)(vlSelfRef.test_sid__DOT__mode));
    vlSelfRef.test_sid__DOT__uut__DOT__f___05Fst_out 
        = Vtest_sid__ConstPool__TABLE_h5c0f3167_0[__Vtableidx1];
    __Vtableidx2 = (((IData)(vlSelfRef.test_sid__DOT__uut__DOT__f_acc_t) 
                     << 1U) | (IData)(vlSelfRef.test_sid__DOT__mode));
    vlSelfRef.test_sid__DOT__uut__DOT__f_pst_out = Vtest_sid__ConstPool__TABLE_h8a1656db_0
        [__Vtableidx2];
    vlSelfRef.test_sid__DOT__uut__DOT__v1__DOT__adsr__DOT__rate_period 
        = vlSelfRef.test_sid__DOT__uut__DOT__v1__DOT__adsr__DOT__rates
        [(0x0000000fU & ((1U == (IData)(vlSelfRef.test_sid__DOT__uut__DOT__v1__DOT__adsr__DOT__state))
                          ? (vlSelfRef.test_sid__DOT__uut__DOT__voice_ad[0U] 
                             >> 4U) : ((2U == (IData)(vlSelfRef.test_sid__DOT__uut__DOT__v1__DOT__adsr__DOT__state))
                                        ? vlSelfRef.test_sid__DOT__uut__DOT__voice_ad[0U]
                                        : vlSelfRef.test_sid__DOT__uut__DOT__voice_sr[0U])))];
    vlSelfRef.test_sid__DOT__uut__DOT__v2__DOT__adsr__DOT__rate_period 
        = vlSelfRef.test_sid__DOT__uut__DOT__v2__DOT__adsr__DOT__rates
        [(0x0000000fU & ((1U == (IData)(vlSelfRef.test_sid__DOT__uut__DOT__v2__DOT__adsr__DOT__state))
                          ? (vlSelfRef.test_sid__DOT__uut__DOT__voice_ad[1U] 
                             >> 4U) : ((2U == (IData)(vlSelfRef.test_sid__DOT__uut__DOT__v2__DOT__adsr__DOT__state))
                                        ? vlSelfRef.test_sid__DOT__uut__DOT__voice_ad[1U]
                                        : vlSelfRef.test_sid__DOT__uut__DOT__voice_sr[1U])))];
    vlSelfRef.test_sid__DOT__uut__DOT__v3__DOT__adsr__DOT__rate_period 
        = vlSelfRef.test_sid__DOT__uut__DOT__v3__DOT__adsr__DOT__rates
        [(0x0000000fU & ((1U == (IData)(vlSelfRef.test_sid__DOT__uut__DOT__v3__DOT__adsr__DOT__state))
                          ? (vlSelfRef.test_sid__DOT__uut__DOT__voice_ad[2U] 
                             >> 4U) : ((2U == (IData)(vlSelfRef.test_sid__DOT__uut__DOT__v3__DOT__adsr__DOT__state))
                                        ? vlSelfRef.test_sid__DOT__uut__DOT__voice_ad[2U]
                                        : vlSelfRef.test_sid__DOT__uut__DOT__voice_sr[2U])))];
    vlSelfRef.test_sid__DOT__uut__DOT__tbl__DOT__f0 
        = vlSelfRef.test_sid__DOT__uut__DOT__tbl__DOT__f6581_curve
        [(0x000003ffU & ((IData)(vlSelfRef.test_sid__DOT__uut__DOT__filter_fc) 
                         >> 1U))];
    test_sid__DOT__uut__DOT__flt__DOT__m = VL_MULS_III(32, 
                                                       VL_EXTENDS_II(32,16, (IData)(vlSelfRef.test_sid__DOT__uut__DOT__flt__DOT__a)), 
                                                       VL_EXTENDS_II(32,16, (IData)(vlSelfRef.test_sid__DOT__uut__DOT__flt__DOT__b)));
    if ((2U != (7U & (IData)(vlSelfRef.test_sid__DOT__uut__DOT__tbl_state)))) {
        if ((3U == (7U & (IData)(vlSelfRef.test_sid__DOT__uut__DOT__tbl_state)))) {
            vlSelfRef.test_sid__DOT__uut__DOT__flt__DOT__vlp2 
                = (0x0000ffffU & ((1U & VL_REDXOR_32(
                                                     (3U 
                                                      & (vlSelfRef.__VdfgRegularize_hebeb780c_0_3 
                                                         >> 0x0000000fU))))
                                   ? ((0x00008000U 
                                       & (vlSelfRef.__VdfgRegularize_hebeb780c_0_3 
                                          >> 1U)) | 
                                      (0x00007fffU 
                                       & (- (IData)(
                                                    (1U 
                                                     & (vlSelfRef.__VdfgRegularize_hebeb780c_0_3 
                                                        >> 0x0000000fU))))))
                                   : vlSelfRef.__VdfgRegularize_hebeb780c_0_3));
        }
        if ((3U != (7U & (IData)(vlSelfRef.test_sid__DOT__uut__DOT__tbl_state)))) {
            if ((4U == (7U & (IData)(vlSelfRef.test_sid__DOT__uut__DOT__tbl_state)))) {
                vlSelfRef.test_sid__DOT__uut__DOT__flt__DOT__vbp2 
                    = vlSelfRef.test_sid__DOT__uut__DOT__flt__DOT__vbp_next;
            }
            if ((4U != (7U & (IData)(vlSelfRef.test_sid__DOT__uut__DOT__tbl_state)))) {
                if ((5U == (7U & (IData)(vlSelfRef.test_sid__DOT__uut__DOT__tbl_state)))) {
                    vlSelfRef.test_sid__DOT__uut__DOT__flt__DOT__vhp 
                        = vlSelfRef.test_sid__DOT__uut__DOT__flt__DOT__vhp2;
                    vlSelfRef.test_sid__DOT__uut__DOT__flt__DOT__vhp2 
                        = vlSelfRef.test_sid__DOT__uut__DOT__flt__DOT__vhp_next;
                }
            }
        }
    }
    if ((1U & (~ (IData)(vlSelfRef.test_sid__DOT__rst)))) {
        if (vlSelfRef.test_sid__DOT__ce_1m) {
            vlSelfRef.test_sid__DOT__uut__DOT__v1__DOT__unnamedblk2__DOT__clk 
                = (1U & (~ (((IData)(vlSelfRef.test_sid__DOT__rst) 
                             | (vlSelfRef.test_sid__DOT__uut__DOT__voice_ctrl[0U] 
                                >> 3U)) | ((~ (IData)(vlSelfRef.test_sid__DOT__uut__DOT__v1__DOT__unnamedblk2__DOT__osc_edge)) 
                                           & (vlSelfRef.test_sid__DOT__uut__DOT__v1__DOT__oscillator 
                                              >> 0x13U)))));
            vlSelfRef.test_sid__DOT__uut__DOT__v2__DOT__unnamedblk2__DOT__clk 
                = (1U & (~ (((IData)(vlSelfRef.test_sid__DOT__rst) 
                             | (vlSelfRef.test_sid__DOT__uut__DOT__voice_ctrl[1U] 
                                >> 3U)) | ((~ (IData)(vlSelfRef.test_sid__DOT__uut__DOT__v2__DOT__unnamedblk2__DOT__osc_edge)) 
                                           & (vlSelfRef.test_sid__DOT__uut__DOT__v2__DOT__oscillator 
                                              >> 0x13U)))));
            vlSelfRef.test_sid__DOT__uut__DOT__v3__DOT__unnamedblk2__DOT__clk 
                = (1U & (~ (((IData)(vlSelfRef.test_sid__DOT__rst) 
                             | (vlSelfRef.test_sid__DOT__uut__DOT__voice_ctrl[2U] 
                                >> 3U)) | ((~ (IData)(vlSelfRef.test_sid__DOT__uut__DOT__v3__DOT__unnamedblk2__DOT__osc_edge)) 
                                           & (vlSelfRef.test_sid__DOT__uut__DOT__v3__DOT__oscillator 
                                              >> 0x13U)))));
        }
    }
    if (vlSelfRef.test_sid__DOT__rst) {
        vlSelfRef.test_sid__DOT__uut__DOT__v1__DOT__adsr__DOT__unnamedblk1__DOT__gate_edge 
            = (1U & vlSelfRef.test_sid__DOT__uut__DOT__voice_ctrl[0U]);
        vlSelfRef.test_sid__DOT__uut__DOT__v2__DOT__adsr__DOT__unnamedblk1__DOT__gate_edge 
            = (1U & vlSelfRef.test_sid__DOT__uut__DOT__voice_ctrl[1U]);
        vlSelfRef.test_sid__DOT__uut__DOT__v3__DOT__adsr__DOT__unnamedblk1__DOT__gate_edge 
            = (1U & vlSelfRef.test_sid__DOT__uut__DOT__voice_ctrl[2U]);
        vlSelfRef.test_sid__DOT__uut__DOT__filter_fc = 0U;
        vlSelfRef.test_sid__DOT__uut__DOT__filter_res_filt = 0U;
        vlSelfRef.test_sid__DOT__uut__DOT__filter_mode_vol = 0U;
        vlSelfRef.test_sid__DOT__uut__DOT__v1__DOT__unnamedblk2__DOT__osc_edge = 0U;
        vlSelfRef.test_sid__DOT__uut__DOT__v2__DOT__unnamedblk2__DOT__osc_edge = 0U;
        vlSelfRef.test_sid__DOT__uut__DOT__v3__DOT__unnamedblk2__DOT__osc_edge = 0U;
    } else {
        if (vlSelfRef.test_sid__DOT__ce_1m) {
            vlSelfRef.test_sid__DOT__uut__DOT__v1__DOT__adsr__DOT__unnamedblk1__DOT__gate_edge 
                = (1U & vlSelfRef.test_sid__DOT__uut__DOT__voice_ctrl[0U]);
            vlSelfRef.test_sid__DOT__uut__DOT__v2__DOT__adsr__DOT__unnamedblk1__DOT__gate_edge 
                = (1U & vlSelfRef.test_sid__DOT__uut__DOT__voice_ctrl[1U]);
            vlSelfRef.test_sid__DOT__uut__DOT__v3__DOT__adsr__DOT__unnamedblk1__DOT__gate_edge 
                = (1U & vlSelfRef.test_sid__DOT__uut__DOT__voice_ctrl[2U]);
            vlSelfRef.test_sid__DOT__uut__DOT__v1__DOT__unnamedblk2__DOT__osc_edge 
                = (1U & (vlSelfRef.test_sid__DOT__uut__DOT__v1__DOT__oscillator 
                         >> 0x13U));
            vlSelfRef.test_sid__DOT__uut__DOT__v2__DOT__unnamedblk2__DOT__osc_edge 
                = (1U & (vlSelfRef.test_sid__DOT__uut__DOT__v2__DOT__oscillator 
                         >> 0x13U));
            vlSelfRef.test_sid__DOT__uut__DOT__v3__DOT__unnamedblk2__DOT__osc_edge 
                = (1U & (vlSelfRef.test_sid__DOT__uut__DOT__v3__DOT__oscillator 
                         >> 0x13U));
        }
        if (((IData)(vlSelfRef.test_sid__DOT__cs) & (IData)(vlSelfRef.test_sid__DOT__we))) {
            if ((1U & (~ ((((((((0U == (IData)(vlSelfRef.test_sid__DOT__addr)) 
                                | (1U == (IData)(vlSelfRef.test_sid__DOT__addr))) 
                               | (2U == (IData)(vlSelfRef.test_sid__DOT__addr))) 
                              | (3U == (IData)(vlSelfRef.test_sid__DOT__addr))) 
                             | (4U == (IData)(vlSelfRef.test_sid__DOT__addr))) 
                            | (5U == (IData)(vlSelfRef.test_sid__DOT__addr))) 
                           | (6U == (IData)(vlSelfRef.test_sid__DOT__addr))) 
                          | (7U == (IData)(vlSelfRef.test_sid__DOT__addr)))))) {
                if ((1U & (~ ((((((((8U == (IData)(vlSelfRef.test_sid__DOT__addr)) 
                                    | (9U == (IData)(vlSelfRef.test_sid__DOT__addr))) 
                                   | (0x0aU == (IData)(vlSelfRef.test_sid__DOT__addr))) 
                                  | (0x0bU == (IData)(vlSelfRef.test_sid__DOT__addr))) 
                                 | (0x0cU == (IData)(vlSelfRef.test_sid__DOT__addr))) 
                                | (0x0dU == (IData)(vlSelfRef.test_sid__DOT__addr))) 
                               | (0x0eU == (IData)(vlSelfRef.test_sid__DOT__addr))) 
                              | (0x0fU == (IData)(vlSelfRef.test_sid__DOT__addr)))))) {
                    if (((((((((0x10U == (IData)(vlSelfRef.test_sid__DOT__addr)) 
                               | (0x11U == (IData)(vlSelfRef.test_sid__DOT__addr))) 
                              | (0x12U == (IData)(vlSelfRef.test_sid__DOT__addr))) 
                             | (0x13U == (IData)(vlSelfRef.test_sid__DOT__addr))) 
                            | (0x14U == (IData)(vlSelfRef.test_sid__DOT__addr))) 
                           | (0x15U == (IData)(vlSelfRef.test_sid__DOT__addr))) 
                          | (0x16U == (IData)(vlSelfRef.test_sid__DOT__addr))) 
                         | (0x17U == (IData)(vlSelfRef.test_sid__DOT__addr)))) {
                        if ((0x10U != (IData)(vlSelfRef.test_sid__DOT__addr))) {
                            if ((0x11U != (IData)(vlSelfRef.test_sid__DOT__addr))) {
                                if ((0x12U != (IData)(vlSelfRef.test_sid__DOT__addr))) {
                                    if ((0x13U != (IData)(vlSelfRef.test_sid__DOT__addr))) {
                                        if ((0x14U 
                                             != (IData)(vlSelfRef.test_sid__DOT__addr))) {
                                            if ((0x15U 
                                                 == (IData)(vlSelfRef.test_sid__DOT__addr))) {
                                                vlSelfRef.test_sid__DOT__uut__DOT__filter_fc 
                                                    = 
                                                    ((0x07f8U 
                                                      & (IData)(vlSelfRef.test_sid__DOT__uut__DOT__filter_fc)) 
                                                     | (7U 
                                                        & (IData)(vlSelfRef.test_sid__DOT__din)));
                                            } else if (
                                                       (0x16U 
                                                        == (IData)(vlSelfRef.test_sid__DOT__addr))) {
                                                vlSelfRef.test_sid__DOT__uut__DOT__filter_fc 
                                                    = 
                                                    ((7U 
                                                      & (IData)(vlSelfRef.test_sid__DOT__uut__DOT__filter_fc)) 
                                                     | ((IData)(vlSelfRef.test_sid__DOT__din) 
                                                        << 3U));
                                            }
                                            if ((0x15U 
                                                 != (IData)(vlSelfRef.test_sid__DOT__addr))) {
                                                if (
                                                    (0x16U 
                                                     != (IData)(vlSelfRef.test_sid__DOT__addr))) {
                                                    vlSelfRef.test_sid__DOT__uut__DOT__filter_res_filt 
                                                        = vlSelfRef.test_sid__DOT__din;
                                                }
                                            }
                                        }
                                    }
                                }
                            }
                        }
                    }
                    if ((1U & (~ ((((((((0x10U == (IData)(vlSelfRef.test_sid__DOT__addr)) 
                                        | (0x11U == (IData)(vlSelfRef.test_sid__DOT__addr))) 
                                       | (0x12U == (IData)(vlSelfRef.test_sid__DOT__addr))) 
                                      | (0x13U == (IData)(vlSelfRef.test_sid__DOT__addr))) 
                                     | (0x14U == (IData)(vlSelfRef.test_sid__DOT__addr))) 
                                    | (0x15U == (IData)(vlSelfRef.test_sid__DOT__addr))) 
                                   | (0x16U == (IData)(vlSelfRef.test_sid__DOT__addr))) 
                                  | (0x17U == (IData)(vlSelfRef.test_sid__DOT__addr)))))) {
                        if ((0x18U == (IData)(vlSelfRef.test_sid__DOT__addr))) {
                            vlSelfRef.test_sid__DOT__uut__DOT__filter_mode_vol 
                                = vlSelfRef.test_sid__DOT__din;
                        }
                    }
                }
            }
        }
    }
    vlSelfRef.test_sid__DOT__uut__DOT__f_acc_t = __Vdly__test_sid__DOT__uut__DOT__f_acc_t;
    vlSelfRef.test_sid__DOT__uut__DOT__flt__DOT__o 
        = ((IData)(vlSelfRef.test_sid__DOT__uut__DOT__flt__DOT__s)
            ? (vlSelfRef.test_sid__DOT__uut__DOT__flt__DOT__c 
               - test_sid__DOT__uut__DOT__flt__DOT__m)
            : (vlSelfRef.test_sid__DOT__uut__DOT__flt__DOT__c 
               + test_sid__DOT__uut__DOT__flt__DOT__m));
    vlSelfRef.test_sid__DOT__uut__DOT__v1_scaled = 
        (0x003fffffU & VL_SHIFTRS_III(22,22,32, (0x003fffffU 
                                                 & VL_MULS_III(22, vlSelfRef.test_sid__DOT__uut__DOT__v1__DOT__dca_out, 
                                                               (0x003fffffU 
                                                                & VL_EXTENDS_II(22,5, vlSelfRef.test_sid__DOT__uut__DOT__voice_vol[0U])))), 4U));
    vlSelfRef.test_sid__DOT__uut__DOT__v2_scaled = 
        (0x003fffffU & VL_SHIFTRS_III(22,22,32, (0x003fffffU 
                                                 & VL_MULS_III(22, vlSelfRef.test_sid__DOT__uut__DOT__v2__DOT__dca_out, 
                                                               (0x003fffffU 
                                                                & VL_EXTENDS_II(22,5, vlSelfRef.test_sid__DOT__uut__DOT__voice_vol[1U])))), 4U));
    vlSelfRef.test_sid__DOT__uut__DOT__v3_scaled = 
        (0x003fffffU & VL_SHIFTRS_III(22,22,32, (0x003fffffU 
                                                 & VL_MULS_III(22, vlSelfRef.test_sid__DOT__uut__DOT__v3__DOT__dca_out, 
                                                               (0x003fffffU 
                                                                & VL_EXTENDS_II(22,5, vlSelfRef.test_sid__DOT__uut__DOT__voice_vol[2U])))), 4U));
    vlSelfRef.test_sid__DOT__uut__DOT__v1__DOT__envelope 
        = __Vdly__test_sid__DOT__uut__DOT__v1__DOT__envelope;
    vlSelfRef.test_sid__DOT__uut__DOT__v1__DOT__unnamedblk3__DOT__keep_cnt 
        = __Vdly__test_sid__DOT__uut__DOT__v1__DOT__unnamedblk3__DOT__keep_cnt;
    vlSelfRef.test_sid__DOT__uut__DOT__v2__DOT__envelope 
        = __Vdly__test_sid__DOT__uut__DOT__v2__DOT__envelope;
    vlSelfRef.test_sid__DOT__uut__DOT__v2__DOT__unnamedblk3__DOT__keep_cnt 
        = __Vdly__test_sid__DOT__uut__DOT__v2__DOT__unnamedblk3__DOT__keep_cnt;
    vlSelfRef.test_sid__DOT__uut__DOT__v3__DOT__envelope 
        = __Vdly__test_sid__DOT__uut__DOT__v3__DOT__envelope;
    vlSelfRef.test_sid__DOT__uut__DOT__v3__DOT__unnamedblk3__DOT__keep_cnt 
        = __Vdly__test_sid__DOT__uut__DOT__v3__DOT__unnamedblk3__DOT__keep_cnt;
    if (__VdlySet__test_sid__DOT__uut__DOT__voice_ctrl__v0) {
        vlSelfRef.test_sid__DOT__uut__DOT__voice_ctrl[0U] = 0U;
        vlSelfRef.test_sid__DOT__uut__DOT__voice_ctrl[1U] = 0U;
        vlSelfRef.test_sid__DOT__uut__DOT__voice_ctrl[2U] = 0U;
    }
    if (__VdlySet__test_sid__DOT__uut__DOT__voice_ctrl__v3) {
        vlSelfRef.test_sid__DOT__uut__DOT__voice_ctrl[0U] 
            = __VdlyVal__test_sid__DOT__uut__DOT__voice_ctrl__v3;
    }
    if (__VdlySet__test_sid__DOT__uut__DOT__voice_ctrl__v4) {
        vlSelfRef.test_sid__DOT__uut__DOT__voice_ctrl[1U] 
            = __VdlyVal__test_sid__DOT__uut__DOT__voice_ctrl__v4;
    }
    if (__VdlySet__test_sid__DOT__uut__DOT__voice_ctrl__v5) {
        vlSelfRef.test_sid__DOT__uut__DOT__voice_ctrl[2U] 
            = __VdlyVal__test_sid__DOT__uut__DOT__voice_ctrl__v5;
    }
    vlSelfRef.__VdfgRegularize_hebeb780c_0_3 = (0x0001ffffU 
                                                & (VL_EXTENDS_II(17,16, (IData)(vlSelfRef.test_sid__DOT__uut__DOT__flt__DOT__vlp)) 
                                                   + 
                                                   VL_SHIFTRS_III(17,32,32, vlSelfRef.test_sid__DOT__uut__DOT__flt__DOT__o, 0x00000011U)));
    __VdfgRegularize_hebeb780c_0_4 = (0x0001ffffU & 
                                      (VL_EXTENDS_II(17,16, (IData)(vlSelfRef.test_sid__DOT__uut__DOT__flt__DOT__vbp)) 
                                       + VL_SHIFTRS_III(17,32,32, vlSelfRef.test_sid__DOT__uut__DOT__flt__DOT__o, 0x00000011U)));
    vlSelfRef.test_sid__DOT__uut__DOT__v1__DOT__envelope_dac__DOT__bitsum[0U] 
        = (0x00000fffU & ((IData)(8U) + ((1U & (IData)(vlSelfRef.test_sid__DOT__uut__DOT__v1__DOT__envelope))
                                          ? vlSelfRef.test_sid__DOT__uut__DOT__v1__DOT__envelope_dac__DOT__bitval[0U]
                                          : 0U)));
    vlSelfRef.test_sid__DOT__uut__DOT__v2__DOT__envelope_dac__DOT__bitsum[0U] 
        = (0x00000fffU & ((IData)(8U) + ((1U & (IData)(vlSelfRef.test_sid__DOT__uut__DOT__v2__DOT__envelope))
                                          ? vlSelfRef.test_sid__DOT__uut__DOT__v2__DOT__envelope_dac__DOT__bitval[0U]
                                          : 0U)));
    vlSelfRef.test_sid__DOT__uut__DOT__v3__DOT__envelope_dac__DOT__bitsum[0U] 
        = (0x00000fffU & ((IData)(8U) + ((1U & (IData)(vlSelfRef.test_sid__DOT__uut__DOT__v3__DOT__envelope))
                                          ? vlSelfRef.test_sid__DOT__uut__DOT__v3__DOT__envelope_dac__DOT__bitval[0U]
                                          : 0U)));
    vlSelfRef.test_sid__DOT__dout = ((0x00000010U & (IData)(vlSelfRef.test_sid__DOT__addr))
                                      ? ((8U & (IData)(vlSelfRef.test_sid__DOT__addr))
                                          ? ((4U & (IData)(vlSelfRef.test_sid__DOT__addr))
                                              ? ((2U 
                                                  & (IData)(vlSelfRef.test_sid__DOT__addr))
                                                  ? 
                                                 ((1U 
                                                   & (IData)(vlSelfRef.test_sid__DOT__addr))
                                                   ? vlSelfRef.test_sid__DOT__uut__DOT__voice_vol[2U]
                                                   : vlSelfRef.test_sid__DOT__uut__DOT__voice_vol[1U])
                                                  : 
                                                 ((1U 
                                                   & (IData)(vlSelfRef.test_sid__DOT__addr))
                                                   ? vlSelfRef.test_sid__DOT__uut__DOT__voice_vol[0U]
                                                   : (IData)(vlSelfRef.test_sid__DOT__uut__DOT__v3__DOT__envelope)))
                                              : ((2U 
                                                  & (IData)(vlSelfRef.test_sid__DOT__addr))
                                                  ? 
                                                 ((1U 
                                                   & (IData)(vlSelfRef.test_sid__DOT__addr))
                                                   ? (IData)(vlSelfRef.test_sid__DOT__uut__DOT__v3__DOT__wave_out)
                                                   : 0U)
                                                  : 
                                                 ((1U 
                                                   & (IData)(vlSelfRef.test_sid__DOT__addr))
                                                   ? 0U
                                                   : (IData)(vlSelfRef.test_sid__DOT__uut__DOT__last_written))))
                                          : (IData)(vlSelfRef.test_sid__DOT__uut__DOT__last_written))
                                      : (IData)(vlSelfRef.test_sid__DOT__uut__DOT__last_written));
    if ((0x00000080U & vlSelfRef.test_sid__DOT__uut__DOT__voice_ctrl[0U])) {
        vlSelfRef.test_sid__DOT__uut__DOT__v1__DOT__comb = 0U;
        vlSelfRef.test_sid__DOT__uut__DOT__v1__DOT__norm 
            = (0x00000fffU & ((0x00000040U & vlSelfRef.test_sid__DOT__uut__DOT__voice_ctrl[0U])
                               ? 0U : ((0x00000020U 
                                        & vlSelfRef.test_sid__DOT__uut__DOT__voice_ctrl[0U])
                                        ? 0U : ((0x00000010U 
                                                 & vlSelfRef.test_sid__DOT__uut__DOT__voice_ctrl[0U])
                                                 ? 0U
                                                 : (IData)(vlSelfRef.test_sid__DOT__uut__DOT__v1__DOT__noise)))));
    } else if ((0x00000040U & vlSelfRef.test_sid__DOT__uut__DOT__voice_ctrl[0U])) {
        if ((0x00000020U & vlSelfRef.test_sid__DOT__uut__DOT__voice_ctrl[0U])) {
            vlSelfRef.test_sid__DOT__uut__DOT__v1__DOT__comb 
                = ((0x00000010U & vlSelfRef.test_sid__DOT__uut__DOT__voice_ctrl[0U])
                    ? (vlSelfRef.test_sid__DOT__uut__DOT__pst_out[0U] 
                       & (- (IData)((IData)(vlSelfRef.test_sid__DOT__uut__DOT__v1__DOT__pulse))))
                    : (vlSelfRef.test_sid__DOT__uut__DOT__ps___05Fout[0U] 
                       & (- (IData)((IData)(vlSelfRef.test_sid__DOT__uut__DOT__v1__DOT__pulse)))));
            vlSelfRef.test_sid__DOT__uut__DOT__v1__DOT__norm 
                = (0x00000fffU & 0U);
        } else if ((0x00000010U & vlSelfRef.test_sid__DOT__uut__DOT__voice_ctrl[0U])) {
            vlSelfRef.test_sid__DOT__uut__DOT__v1__DOT__comb 
                = (vlSelfRef.test_sid__DOT__uut__DOT__p_t_out[0U] 
                   & (- (IData)((IData)(vlSelfRef.test_sid__DOT__uut__DOT__v1__DOT__pulse))));
            vlSelfRef.test_sid__DOT__uut__DOT__v1__DOT__norm 
                = (0x00000fffU & 0U);
        } else {
            vlSelfRef.test_sid__DOT__uut__DOT__v1__DOT__comb = 0U;
            vlSelfRef.test_sid__DOT__uut__DOT__v1__DOT__norm 
                = (0x00000fffU & (- (IData)((IData)(vlSelfRef.test_sid__DOT__uut__DOT__v1__DOT__pulse))));
        }
    } else if ((0x00000020U & vlSelfRef.test_sid__DOT__uut__DOT__voice_ctrl[0U])) {
        if ((0x00000010U & vlSelfRef.test_sid__DOT__uut__DOT__voice_ctrl[0U])) {
            vlSelfRef.test_sid__DOT__uut__DOT__v1__DOT__comb 
                = vlSelfRef.test_sid__DOT__uut__DOT___st_out[0U];
            vlSelfRef.test_sid__DOT__uut__DOT__v1__DOT__norm 
                = (0x00000fffU & 0U);
        } else {
            vlSelfRef.test_sid__DOT__uut__DOT__v1__DOT__comb = 0U;
            vlSelfRef.test_sid__DOT__uut__DOT__v1__DOT__norm 
                = (0x00000fffU & (IData)(vlSelfRef.test_sid__DOT__uut__DOT__v1__DOT__saw_tri));
        }
    } else {
        vlSelfRef.test_sid__DOT__uut__DOT__v1__DOT__comb = 0U;
        vlSelfRef.test_sid__DOT__uut__DOT__v1__DOT__norm 
            = (0x00000fffU & ((0x00000010U & vlSelfRef.test_sid__DOT__uut__DOT__voice_ctrl[0U])
                               ? VL_SHIFTL_III(12,12,32, (IData)(vlSelfRef.test_sid__DOT__uut__DOT__v1__DOT__saw_tri), 1U)
                               : 0U));
    }
    if ((0x00000080U & vlSelfRef.test_sid__DOT__uut__DOT__voice_ctrl[1U])) {
        vlSelfRef.test_sid__DOT__uut__DOT__v2__DOT__comb = 0U;
        vlSelfRef.test_sid__DOT__uut__DOT__v2__DOT__norm 
            = (0x00000fffU & ((0x00000040U & vlSelfRef.test_sid__DOT__uut__DOT__voice_ctrl[1U])
                               ? 0U : ((0x00000020U 
                                        & vlSelfRef.test_sid__DOT__uut__DOT__voice_ctrl[1U])
                                        ? 0U : ((0x00000010U 
                                                 & vlSelfRef.test_sid__DOT__uut__DOT__voice_ctrl[1U])
                                                 ? 0U
                                                 : (IData)(vlSelfRef.test_sid__DOT__uut__DOT__v2__DOT__noise)))));
    } else if ((0x00000040U & vlSelfRef.test_sid__DOT__uut__DOT__voice_ctrl[1U])) {
        if ((0x00000020U & vlSelfRef.test_sid__DOT__uut__DOT__voice_ctrl[1U])) {
            vlSelfRef.test_sid__DOT__uut__DOT__v2__DOT__comb 
                = ((0x00000010U & vlSelfRef.test_sid__DOT__uut__DOT__voice_ctrl[1U])
                    ? (vlSelfRef.test_sid__DOT__uut__DOT__pst_out[1U] 
                       & (- (IData)((IData)(vlSelfRef.test_sid__DOT__uut__DOT__v2__DOT__pulse))))
                    : (vlSelfRef.test_sid__DOT__uut__DOT__ps___05Fout[1U] 
                       & (- (IData)((IData)(vlSelfRef.test_sid__DOT__uut__DOT__v2__DOT__pulse)))));
            vlSelfRef.test_sid__DOT__uut__DOT__v2__DOT__norm 
                = (0x00000fffU & 0U);
        } else if ((0x00000010U & vlSelfRef.test_sid__DOT__uut__DOT__voice_ctrl[1U])) {
            vlSelfRef.test_sid__DOT__uut__DOT__v2__DOT__comb 
                = (vlSelfRef.test_sid__DOT__uut__DOT__p_t_out[1U] 
                   & (- (IData)((IData)(vlSelfRef.test_sid__DOT__uut__DOT__v2__DOT__pulse))));
            vlSelfRef.test_sid__DOT__uut__DOT__v2__DOT__norm 
                = (0x00000fffU & 0U);
        } else {
            vlSelfRef.test_sid__DOT__uut__DOT__v2__DOT__comb = 0U;
            vlSelfRef.test_sid__DOT__uut__DOT__v2__DOT__norm 
                = (0x00000fffU & (- (IData)((IData)(vlSelfRef.test_sid__DOT__uut__DOT__v2__DOT__pulse))));
        }
    } else if ((0x00000020U & vlSelfRef.test_sid__DOT__uut__DOT__voice_ctrl[1U])) {
        if ((0x00000010U & vlSelfRef.test_sid__DOT__uut__DOT__voice_ctrl[1U])) {
            vlSelfRef.test_sid__DOT__uut__DOT__v2__DOT__comb 
                = vlSelfRef.test_sid__DOT__uut__DOT___st_out[1U];
            vlSelfRef.test_sid__DOT__uut__DOT__v2__DOT__norm 
                = (0x00000fffU & 0U);
        } else {
            vlSelfRef.test_sid__DOT__uut__DOT__v2__DOT__comb = 0U;
            vlSelfRef.test_sid__DOT__uut__DOT__v2__DOT__norm 
                = (0x00000fffU & (IData)(vlSelfRef.test_sid__DOT__uut__DOT__v2__DOT__saw_tri));
        }
    } else {
        vlSelfRef.test_sid__DOT__uut__DOT__v2__DOT__comb = 0U;
        vlSelfRef.test_sid__DOT__uut__DOT__v2__DOT__norm 
            = (0x00000fffU & ((0x00000010U & vlSelfRef.test_sid__DOT__uut__DOT__voice_ctrl[1U])
                               ? VL_SHIFTL_III(12,12,32, (IData)(vlSelfRef.test_sid__DOT__uut__DOT__v2__DOT__saw_tri), 1U)
                               : 0U));
    }
    if ((0x00000080U & vlSelfRef.test_sid__DOT__uut__DOT__voice_ctrl[2U])) {
        vlSelfRef.test_sid__DOT__uut__DOT__v3__DOT__comb = 0U;
        vlSelfRef.test_sid__DOT__uut__DOT__v3__DOT__norm 
            = (0x00000fffU & ((0x00000040U & vlSelfRef.test_sid__DOT__uut__DOT__voice_ctrl[2U])
                               ? 0U : ((0x00000020U 
                                        & vlSelfRef.test_sid__DOT__uut__DOT__voice_ctrl[2U])
                                        ? 0U : ((0x00000010U 
                                                 & vlSelfRef.test_sid__DOT__uut__DOT__voice_ctrl[2U])
                                                 ? 0U
                                                 : (IData)(vlSelfRef.test_sid__DOT__uut__DOT__v3__DOT__noise)))));
    } else if ((0x00000040U & vlSelfRef.test_sid__DOT__uut__DOT__voice_ctrl[2U])) {
        if ((0x00000020U & vlSelfRef.test_sid__DOT__uut__DOT__voice_ctrl[2U])) {
            vlSelfRef.test_sid__DOT__uut__DOT__v3__DOT__comb 
                = ((0x00000010U & vlSelfRef.test_sid__DOT__uut__DOT__voice_ctrl[2U])
                    ? (vlSelfRef.test_sid__DOT__uut__DOT__pst_out[2U] 
                       & (- (IData)((IData)(vlSelfRef.test_sid__DOT__uut__DOT__v3__DOT__pulse))))
                    : (vlSelfRef.test_sid__DOT__uut__DOT__ps___05Fout[2U] 
                       & (- (IData)((IData)(vlSelfRef.test_sid__DOT__uut__DOT__v3__DOT__pulse)))));
            vlSelfRef.test_sid__DOT__uut__DOT__v3__DOT__norm 
                = (0x00000fffU & 0U);
        } else if ((0x00000010U & vlSelfRef.test_sid__DOT__uut__DOT__voice_ctrl[2U])) {
            vlSelfRef.test_sid__DOT__uut__DOT__v3__DOT__comb 
                = (vlSelfRef.test_sid__DOT__uut__DOT__p_t_out[2U] 
                   & (- (IData)((IData)(vlSelfRef.test_sid__DOT__uut__DOT__v3__DOT__pulse))));
            vlSelfRef.test_sid__DOT__uut__DOT__v3__DOT__norm 
                = (0x00000fffU & 0U);
        } else {
            vlSelfRef.test_sid__DOT__uut__DOT__v3__DOT__comb = 0U;
            vlSelfRef.test_sid__DOT__uut__DOT__v3__DOT__norm 
                = (0x00000fffU & (- (IData)((IData)(vlSelfRef.test_sid__DOT__uut__DOT__v3__DOT__pulse))));
        }
    } else if ((0x00000020U & vlSelfRef.test_sid__DOT__uut__DOT__voice_ctrl[2U])) {
        if ((0x00000010U & vlSelfRef.test_sid__DOT__uut__DOT__voice_ctrl[2U])) {
            vlSelfRef.test_sid__DOT__uut__DOT__v3__DOT__comb 
                = vlSelfRef.test_sid__DOT__uut__DOT___st_out[2U];
            vlSelfRef.test_sid__DOT__uut__DOT__v3__DOT__norm 
                = (0x00000fffU & 0U);
        } else {
            vlSelfRef.test_sid__DOT__uut__DOT__v3__DOT__comb = 0U;
            vlSelfRef.test_sid__DOT__uut__DOT__v3__DOT__norm 
                = (0x00000fffU & (IData)(vlSelfRef.test_sid__DOT__uut__DOT__v3__DOT__saw_tri));
        }
    } else {
        vlSelfRef.test_sid__DOT__uut__DOT__v3__DOT__comb = 0U;
        vlSelfRef.test_sid__DOT__uut__DOT__v3__DOT__norm 
            = (0x00000fffU & ((0x00000010U & vlSelfRef.test_sid__DOT__uut__DOT__voice_ctrl[2U])
                               ? VL_SHIFTL_III(12,12,32, (IData)(vlSelfRef.test_sid__DOT__uut__DOT__v3__DOT__saw_tri), 1U)
                               : 0U));
    }
    vlSelfRef.test_sid__DOT__uut__DOT__flt__DOT__vbp_next 
        = (0x0000ffffU & ((1U & VL_REDXOR_32((3U & 
                                              (__VdfgRegularize_hebeb780c_0_4 
                                               >> 0x0000000fU))))
                           ? ((0x00008000U & (__VdfgRegularize_hebeb780c_0_4 
                                              >> 1U)) 
                              | (0x00007fffU & (- (IData)(
                                                          (1U 
                                                           & (__VdfgRegularize_hebeb780c_0_4 
                                                              >> 0x0000000fU))))))
                           : __VdfgRegularize_hebeb780c_0_4));
    vlSelfRef.test_sid__DOT__uut__DOT__flt__DOT__vhp_next 
        = (0x0000ffffU & ((1U & VL_REDXOR_32((3U & 
                                              (vlSelfRef.test_sid__DOT__uut__DOT__flt__DOT__o 
                                               >> 0x00000019U))))
                           ? ((0x00008000U & (vlSelfRef.test_sid__DOT__uut__DOT__flt__DOT__o 
                                              >> 0x0000000bU)) 
                              | (0x00007fffU & (- (IData)(
                                                          (1U 
                                                           & (vlSelfRef.test_sid__DOT__uut__DOT__flt__DOT__o 
                                                              >> 0x00000019U))))))
                           : (vlSelfRef.test_sid__DOT__uut__DOT__flt__DOT__o 
                              >> 0x0000000aU)));
    vlSelfRef.test_sid__DOT__uut__DOT__tbl_state = __Vdly__test_sid__DOT__uut__DOT__tbl_state;
    vlSelfRef.test_sid__DOT__uut__DOT__v1__DOT__oscillator 
        = __Vdly__test_sid__DOT__uut__DOT__v1__DOT__oscillator;
    vlSelfRef.test_sid__DOT__uut__DOT__v2__DOT__oscillator 
        = __Vdly__test_sid__DOT__uut__DOT__v2__DOT__oscillator;
    vlSelfRef.test_sid__DOT__uut__DOT__v3__DOT__oscillator 
        = __Vdly__test_sid__DOT__uut__DOT__v3__DOT__oscillator;
    vlSelfRef.test_sid__DOT__uut__DOT__v1__DOT__waveform_dac__DOT__bitsum[0U] 
        = (0x0000ffffU & ((IData)(8U) + ((1U & (IData)(vlSelfRef.test_sid__DOT__uut__DOT__v1__DOT__norm))
                                          ? vlSelfRef.test_sid__DOT__uut__DOT__v1__DOT__waveform_dac__DOT__bitval[0U]
                                          : 0U)));
    vlSelfRef.test_sid__DOT__uut__DOT__v2__DOT__waveform_dac__DOT__bitsum[0U] 
        = (0x0000ffffU & ((IData)(8U) + ((1U & (IData)(vlSelfRef.test_sid__DOT__uut__DOT__v2__DOT__norm))
                                          ? vlSelfRef.test_sid__DOT__uut__DOT__v2__DOT__waveform_dac__DOT__bitval[0U]
                                          : 0U)));
    vlSelfRef.test_sid__DOT__uut__DOT__v3__DOT__waveform_dac__DOT__bitsum[0U] 
        = (0x0000ffffU & ((IData)(8U) + ((1U & (IData)(vlSelfRef.test_sid__DOT__uut__DOT__v3__DOT__norm))
                                          ? vlSelfRef.test_sid__DOT__uut__DOT__v3__DOT__waveform_dac__DOT__bitval[0U]
                                          : 0U)));
    vlSelfRef.test_sid__DOT__uut__DOT____Vcellout__v2__acc_t 
        = ((0x00000800U & (vlSelfRef.test_sid__DOT__uut__DOT__v2__DOT__oscillator 
                           >> 0x0000000cU)) | (0x000007ffU 
                                               & ((vlSelfRef.test_sid__DOT__uut__DOT__v2__DOT__oscillator 
                                                   >> 0x0000000cU) 
                                                  ^ 
                                                  (- (IData)(
                                                             (1U 
                                                              & ((~ 
                                                                  (vlSelfRef.test_sid__DOT__uut__DOT__voice_ctrl[1U] 
                                                                   >> 5U)) 
                                                                 & (((~ 
                                                                      (vlSelfRef.test_sid__DOT__uut__DOT__v1__DOT__oscillator 
                                                                       >> 0x00000017U)) 
                                                                     & (vlSelfRef.test_sid__DOT__uut__DOT__voice_ctrl[1U] 
                                                                        >> 2U)) 
                                                                    ^ 
                                                                    (vlSelfRef.test_sid__DOT__uut__DOT__v2__DOT__oscillator 
                                                                     >> 0x00000017U)))))))));
    vlSelfRef.test_sid__DOT__uut__DOT____Vcellout__v1__acc_t 
        = ((0x00000800U & (vlSelfRef.test_sid__DOT__uut__DOT__v1__DOT__oscillator 
                           >> 0x0000000cU)) | (0x000007ffU 
                                               & ((vlSelfRef.test_sid__DOT__uut__DOT__v1__DOT__oscillator 
                                                   >> 0x0000000cU) 
                                                  ^ 
                                                  (- (IData)(
                                                             (1U 
                                                              & ((~ 
                                                                  (vlSelfRef.test_sid__DOT__uut__DOT__voice_ctrl[0U] 
                                                                   >> 5U)) 
                                                                 & (((~ 
                                                                      (vlSelfRef.test_sid__DOT__uut__DOT__v3__DOT__oscillator 
                                                                       >> 0x00000017U)) 
                                                                     & (vlSelfRef.test_sid__DOT__uut__DOT__voice_ctrl[0U] 
                                                                        >> 2U)) 
                                                                    ^ 
                                                                    (vlSelfRef.test_sid__DOT__uut__DOT__v1__DOT__oscillator 
                                                                     >> 0x00000017U)))))))));
    vlSelfRef.test_sid__DOT__uut__DOT____Vcellout__v3__acc_t 
        = ((0x00000800U & (vlSelfRef.test_sid__DOT__uut__DOT__v3__DOT__oscillator 
                           >> 0x0000000cU)) | (0x000007ffU 
                                               & ((vlSelfRef.test_sid__DOT__uut__DOT__v3__DOT__oscillator 
                                                   >> 0x0000000cU) 
                                                  ^ 
                                                  (- (IData)(
                                                             (1U 
                                                              & ((~ 
                                                                  (vlSelfRef.test_sid__DOT__uut__DOT__voice_ctrl[2U] 
                                                                   >> 5U)) 
                                                                 & (((~ 
                                                                      (vlSelfRef.test_sid__DOT__uut__DOT__v2__DOT__oscillator 
                                                                       >> 0x00000017U)) 
                                                                     & (vlSelfRef.test_sid__DOT__uut__DOT__voice_ctrl[2U] 
                                                                        >> 2U)) 
                                                                    ^ 
                                                                    (vlSelfRef.test_sid__DOT__uut__DOT__v3__DOT__oscillator 
                                                                     >> 0x00000017U)))))))));
    vlSelfRef.test_sid__DOT__ce_1m = (0x00000018U == vlSelfRef.test_sid__DOT__ce_cnt);
    vlSelfRef.test_sid__DOT__ce_cnt = ((0x00000018U 
                                        == vlSelfRef.test_sid__DOT__ce_cnt)
                                        ? 0U : ((IData)(1U) 
                                                + vlSelfRef.test_sid__DOT__ce_cnt));
    vlSelfRef.test_sid__DOT__uut__DOT__acc_t[1U] = vlSelfRef.test_sid__DOT__uut__DOT____Vcellout__v2__acc_t;
    vlSelfRef.test_sid__DOT__uut__DOT__acc_t[0U] = vlSelfRef.test_sid__DOT__uut__DOT____Vcellout__v1__acc_t;
    vlSelfRef.test_sid__DOT__uut__DOT__acc_t[2U] = vlSelfRef.test_sid__DOT__uut__DOT____Vcellout__v3__acc_t;
}

void Vtest_sid___024root___eval_nba(Vtest_sid___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vtest_sid___024root___eval_nba\n"); );
    Vtest_sid__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
    // Body
    if ((0x0000000000000040ULL & vlSelfRef.__VnbaTriggered[0U])) {
        Vtest_sid___024root___nba_sequent__TOP__0(vlSelf);
    }
    if ((0x0000000000000042ULL & vlSelfRef.__VnbaTriggered[0U])) {
        Vtest_sid___024root___act_sequent__TOP__1(vlSelf);
    }
    if ((0x0000000000000048ULL & vlSelfRef.__VnbaTriggered[0U])) {
        Vtest_sid___024root___act_sequent__TOP__2(vlSelf);
    }
    if ((0x0000000000000060ULL & vlSelfRef.__VnbaTriggered[0U])) {
        Vtest_sid___024root___act_sequent__TOP__3(vlSelf);
    }
    if ((0x0000000000000041ULL & vlSelfRef.__VnbaTriggered[0U])) {
        Vtest_sid___024root___act_sequent__TOP__4(vlSelf);
    }
    if ((0x0000000000000044ULL & vlSelfRef.__VnbaTriggered[0U])) {
        Vtest_sid___024root___act_sequent__TOP__5(vlSelf);
    }
    if ((0x0000000000000050ULL & vlSelfRef.__VnbaTriggered[0U])) {
        Vtest_sid___024root___act_sequent__TOP__6(vlSelf);
    }
}

void Vtest_sid___024root___timing_ready(Vtest_sid___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vtest_sid___024root___timing_ready\n"); );
    Vtest_sid__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
    // Body
    if ((0x0000000000000040ULL & vlSelfRef.__VactTriggered[0U])) {
        vlSelfRef.__VtrigSched_hb52908e9__0.ready("@(posedge test_sid.clk)");
    }
}

void Vtest_sid___024root___timing_resume(Vtest_sid___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vtest_sid___024root___timing_resume\n"); );
    Vtest_sid__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
    // Body
    vlSelfRef.__VtrigSched_hb52908e9__0.moveToResumeQueue(
                                                          "@(posedge test_sid.clk)");
    vlSelfRef.__VtrigSched_hb52908e9__0.resume("@(posedge test_sid.clk)");
    if ((0x0000000000000080ULL & vlSelfRef.__VactTriggered[0U])) {
        vlSelfRef.__VdlySched.resume();
    }
}

void Vtest_sid___024root___trigger_orInto__act_vec_vec(VlUnpacked<QData/*63:0*/, 1> &out, const VlUnpacked<QData/*63:0*/, 1> &in) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vtest_sid___024root___trigger_orInto__act_vec_vec\n"); );
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
VL_ATTR_COLD void Vtest_sid___024root___dump_triggers__act(const VlUnpacked<QData/*63:0*/, 1> &triggers, const std::string &tag);
#endif  // VL_DEBUG

bool Vtest_sid___024root___eval_phase__act(Vtest_sid___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vtest_sid___024root___eval_phase__act\n"); );
    Vtest_sid__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
    // Locals
    CData/*0:0*/ __VactExecute;
    // Body
    Vtest_sid___024root___eval_triggers_vec__act(vlSelf);
    Vtest_sid___024root___timing_ready(vlSelf);
    Vtest_sid___024root___trigger_orInto__act_vec_vec(vlSelfRef.__VactTriggered, vlSelfRef.__VactTriggeredAcc);
#ifdef VL_DEBUG
    if (VL_UNLIKELY(vlSymsp->_vm_contextp__->debug())) {
        Vtest_sid___024root___dump_triggers__act(vlSelfRef.__VactTriggered, "act"s);
    }
#endif
    Vtest_sid___024root___trigger_orInto__act_vec_vec(vlSelfRef.__VnbaTriggered, vlSelfRef.__VactTriggered);
    __VactExecute = Vtest_sid___024root___trigger_anySet__act(vlSelfRef.__VactTriggered);
    if (__VactExecute) {
        vlSelfRef.__VactTriggeredAcc.fill(0ULL);
        Vtest_sid___024root___timing_resume(vlSelf);
        Vtest_sid___024root___eval_act(vlSelf);
    }
    return (__VactExecute);
}

bool Vtest_sid___024root___eval_phase__inact(Vtest_sid___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vtest_sid___024root___eval_phase__inact\n"); );
    Vtest_sid__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
    // Locals
    CData/*0:0*/ __VinactExecute;
    // Body
    __VinactExecute = vlSelfRef.__VdlySched.awaitingZeroDelay();
    if (__VinactExecute) {
        VL_FATAL_MT("test_sid.sv", 4, "", "ZERODLY: Design Verilated with '--no-sched-zero-delay', but #0 delay executed at runtime");
    }
    return (__VinactExecute);
}

void Vtest_sid___024root___trigger_clear__act(VlUnpacked<QData/*63:0*/, 1> &out) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vtest_sid___024root___trigger_clear__act\n"); );
    // Locals
    IData/*31:0*/ n;
    // Body
    n = 0U;
    do {
        out[n] = 0ULL;
        n = ((IData)(1U) + n);
    } while ((1U > n));
}

bool Vtest_sid___024root___eval_phase__nba(Vtest_sid___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vtest_sid___024root___eval_phase__nba\n"); );
    Vtest_sid__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
    // Locals
    CData/*0:0*/ __VnbaExecute;
    // Body
    __VnbaExecute = Vtest_sid___024root___trigger_anySet__act(vlSelfRef.__VnbaTriggered);
    if (__VnbaExecute) {
        Vtest_sid___024root___eval_nba(vlSelf);
        Vtest_sid___024root___trigger_clear__act(vlSelfRef.__VnbaTriggered);
    }
    return (__VnbaExecute);
}

void Vtest_sid___024root___eval(Vtest_sid___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vtest_sid___024root___eval\n"); );
    Vtest_sid__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
    // Locals
    IData/*31:0*/ __VnbaIterCount;
    // Body
    __VnbaIterCount = 0U;
    do {
        if (VL_UNLIKELY(((0x00000064U < __VnbaIterCount)))) {
#ifdef VL_DEBUG
            Vtest_sid___024root___dump_triggers__act(vlSelfRef.__VnbaTriggered, "nba"s);
#endif
            VL_FATAL_MT("test_sid.sv", 4, "", "DIDNOTCONVERGE: NBA region did not converge after '--converge-limit' of 100 tries");
        }
        __VnbaIterCount = ((IData)(1U) + __VnbaIterCount);
        vlSelfRef.__VinactIterCount = 0U;
        do {
            if (VL_UNLIKELY(((0x00000064U < vlSelfRef.__VinactIterCount)))) {
                VL_FATAL_MT("test_sid.sv", 4, "", "DIDNOTCONVERGE: Inactive region did not converge after '--converge-limit' of 100 tries");
            }
            vlSelfRef.__VinactIterCount = ((IData)(1U) 
                                           + vlSelfRef.__VinactIterCount);
            vlSelfRef.__VactIterCount = 0U;
            do {
                if (VL_UNLIKELY(((0x00000064U < vlSelfRef.__VactIterCount)))) {
#ifdef VL_DEBUG
                    Vtest_sid___024root___dump_triggers__act(vlSelfRef.__VactTriggered, "act"s);
#endif
                    VL_FATAL_MT("test_sid.sv", 4, "", "DIDNOTCONVERGE: Active region did not converge after '--converge-limit' of 100 tries");
                }
                vlSelfRef.__VactIterCount = ((IData)(1U) 
                                             + vlSelfRef.__VactIterCount);
                vlSelfRef.__VactPhaseResult = Vtest_sid___024root___eval_phase__act(vlSelf);
            } while (vlSelfRef.__VactPhaseResult);
            vlSelfRef.__VinactPhaseResult = Vtest_sid___024root___eval_phase__inact(vlSelf);
        } while (vlSelfRef.__VinactPhaseResult);
        vlSelfRef.__VnbaPhaseResult = Vtest_sid___024root___eval_phase__nba(vlSelf);
    } while (vlSelfRef.__VnbaPhaseResult);
}

void Vtest_sid___024root____VbeforeTrig_hb52908e9__0(Vtest_sid___024root* vlSelf, const char* __VeventDescription) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vtest_sid___024root____VbeforeTrig_hb52908e9__0\n"); );
    Vtest_sid__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
    // Locals
    VlUnpacked<QData/*63:0*/, 1> __VTmp;
    // Body
    __VTmp[0U] = (QData)((IData)((((IData)(vlSelfRef.test_sid__DOT__clk) 
                                   & (~ (IData)(vlSelfRef.__Vtrigprevexpr___TOP__test_sid__DOT__clk__0))) 
                                  << 6U)));
    vlSelfRef.__Vtrigprevexpr___TOP__test_sid__DOT__clk__0 
        = vlSelfRef.test_sid__DOT__clk;
    if ((0x0000000000000040ULL & __VTmp[0U])) {
        vlSelfRef.__VtrigSched_hb52908e9__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_hb52908e9__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_hb52908e9__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_hb52908e9__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_hb52908e9__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_hb52908e9__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_hb52908e9__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_hb52908e9__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_hb52908e9__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_hb52908e9__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_hb52908e9__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_hb52908e9__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_hb52908e9__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_hb52908e9__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_hb52908e9__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_hb52908e9__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_hb52908e9__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_hb52908e9__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_hb52908e9__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_hb52908e9__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_hb52908e9__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_hb52908e9__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_hb52908e9__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_hb52908e9__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_hb52908e9__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_hb52908e9__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_hb52908e9__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_hb52908e9__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_hb52908e9__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_hb52908e9__0.ready(__VeventDescription);
    }
    vlSelfRef.__VactTriggeredAcc[0U] = (vlSelfRef.__VactTriggeredAcc[0U] 
                                        | __VTmp[0U]);
}

#ifdef VL_DEBUG
void Vtest_sid___024root___eval_debug_assertions(Vtest_sid___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vtest_sid___024root___eval_debug_assertions\n"); );
    Vtest_sid__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
}
#endif  // VL_DEBUG
