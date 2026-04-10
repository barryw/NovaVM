// Verilated -*- C++ -*-
// DESCRIPTION: Verilator output: Design implementation internals
// See Vtest_copper_lists.h for the primary calling header

#include "Vtest_copper_lists__pch.h"

void Vtest_copper_lists___024root___timing_ready(Vtest_copper_lists___024root* vlSelf);

VL_ATTR_COLD void Vtest_copper_lists___024root___eval_static(Vtest_copper_lists___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vtest_copper_lists___024root___eval_static\n"); );
    Vtest_copper_lists__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
    // Body
    vlSelfRef.test_copper_lists__DOT__clk = 0U;
    vlSelfRef.test_copper_lists__DOT__pass_count = 0U;
    vlSelfRef.test_copper_lists__DOT__fail_count = 0U;
    vlSelfRef.test_copper_lists__DOT__test_num = 0U;
    vlSelfRef.__Vtrigprevexpr___TOP__test_copper_lists__DOT__clk__0 = 0U;
    Vtest_copper_lists___024root___timing_ready(vlSelf);
    do {
        vlSelfRef.__VactTriggeredAcc[vlSelfRef.__Vi] 
            = vlSelfRef.__VactTriggered[vlSelfRef.__Vi];
        vlSelfRef.__Vi = ((IData)(1U) + vlSelfRef.__Vi);
    } while ((0U >= vlSelfRef.__Vi));
}

VL_ATTR_COLD void Vtest_copper_lists___024root___eval_static__TOP(Vtest_copper_lists___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vtest_copper_lists___024root___eval_static__TOP\n"); );
    Vtest_copper_lists__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
    // Body
    vlSelfRef.test_copper_lists__DOT__clk = 0U;
    vlSelfRef.test_copper_lists__DOT__pass_count = 0U;
    vlSelfRef.test_copper_lists__DOT__fail_count = 0U;
    vlSelfRef.test_copper_lists__DOT__test_num = 0U;
}

VL_ATTR_COLD void Vtest_copper_lists___024root___eval_initial__TOP(Vtest_copper_lists___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vtest_copper_lists___024root___eval_initial__TOP\n"); );
    Vtest_copper_lists__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
    // Locals
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
    VlUnpacked<CData/*3:0*/, 16> test_copper_lists__DOT__dut__DOT__spr_shape;
    for (int __Vi0 = 0; __Vi0 < 16; ++__Vi0) {
        test_copper_lists__DOT__dut__DOT__spr_shape[__Vi0] = 0;
    }
    VlUnpacked<CData/*3:0*/, 16> test_copper_lists__DOT__dut__DOT__spr_trans;
    for (int __Vi0 = 0; __Vi0 < 16; ++__Vi0) {
        test_copper_lists__DOT__dut__DOT__spr_trans[__Vi0] = 0;
    }
    IData/*31:0*/ test_copper_lists__DOT__dut__DOT__unnamedblk1__DOT__i;
    test_copper_lists__DOT__dut__DOT__unnamedblk1__DOT__i = 0;
    IData/*31:0*/ test_copper_lists__DOT__dut__DOT__unnamedblk2__DOT__i;
    test_copper_lists__DOT__dut__DOT__unnamedblk2__DOT__i = 0;
    IData/*31:0*/ test_copper_lists__DOT__dut__DOT__unnamedblk3__DOT__i;
    test_copper_lists__DOT__dut__DOT__unnamedblk3__DOT__i = 0;
    IData/*31:0*/ test_copper_lists__DOT__dut__DOT__unnamedblk6__DOT__i;
    test_copper_lists__DOT__dut__DOT__unnamedblk6__DOT__i = 0;
    IData/*31:0*/ test_copper_lists__DOT__dut__DOT__unnamedblk7__DOT__i;
    test_copper_lists__DOT__dut__DOT__unnamedblk7__DOT__i = 0;
    VlUnpacked<SData/*11:0*/, 256> test_copper_lists__DOT__dut__DOT__tile_inst__DOT__pal_ram;
    for (int __Vi0 = 0; __Vi0 < 256; ++__Vi0) {
        test_copper_lists__DOT__dut__DOT__tile_inst__DOT__pal_ram[__Vi0] = 0;
    }
    IData/*31:0*/ test_copper_lists__DOT__dut__DOT__tile_inst__DOT__unnamedblk2__DOT__i;
    test_copper_lists__DOT__dut__DOT__tile_inst__DOT__unnamedblk2__DOT__i = 0;
    IData/*31:0*/ test_copper_lists__DOT__dut__DOT__tile_inst__DOT__unnamedblk3__DOT__i;
    test_copper_lists__DOT__dut__DOT__tile_inst__DOT__unnamedblk3__DOT__i = 0;
    IData/*31:0*/ test_copper_lists__DOT__dut__DOT__tile_inst__DOT__unnamedblk4__DOT__i;
    test_copper_lists__DOT__dut__DOT__tile_inst__DOT__unnamedblk4__DOT__i = 0;
    IData/*31:0*/ __Vilp1;
    // Body
    test_copper_lists__DOT__dut__DOT__unnamedblk1__DOT__i = 0U;
    while (VL_GTS_III(32, 0x000007d0U, test_copper_lists__DOT__dut__DOT__unnamedblk1__DOT__i)) {
        vlSelfRef.test_copper_lists__DOT__dut__DOT____Vlvbound_h3318332a__0 = 0x20U;
        vlSelfRef.test_copper_lists__DOT__dut__DOT____Vlvbound_h45ddebd4__0 = 1U;
        if (VL_LIKELY(((0x07cfU >= (0x000007ffU & test_copper_lists__DOT__dut__DOT__unnamedblk1__DOT__i))))) {
            vlSelfRef.test_copper_lists__DOT__dut__DOT__char_ram[(0x000007ffU 
                                                                  & test_copper_lists__DOT__dut__DOT__unnamedblk1__DOT__i)] 
                = vlSelfRef.test_copper_lists__DOT__dut__DOT____Vlvbound_h3318332a__0;
            vlSelfRef.test_copper_lists__DOT__dut__DOT__color_ram[(0x000007ffU 
                                                                   & test_copper_lists__DOT__dut__DOT__unnamedblk1__DOT__i)] 
                = vlSelfRef.test_copper_lists__DOT__dut__DOT____Vlvbound_h45ddebd4__0;
        }
        test_copper_lists__DOT__dut__DOT__unnamedblk1__DOT__i 
            = ((IData)(1U) + test_copper_lists__DOT__dut__DOT__unnamedblk1__DOT__i);
    }
    test_copper_lists__DOT__dut__DOT__unnamedblk2__DOT__i = 0U;
    while (VL_GTS_III(32, 0x0000fa00U, test_copper_lists__DOT__dut__DOT__unnamedblk2__DOT__i)) {
        vlSelfRef.test_copper_lists__DOT__dut__DOT____Vlvbound_h652b6de2__0 = 0U;
        if (VL_LIKELY(((0xf9ffU >= (0x0000ffffU & test_copper_lists__DOT__dut__DOT__unnamedblk2__DOT__i))))) {
            vlSelfRef.test_copper_lists__DOT__dut__DOT__gfx_ram[(0x0000ffffU 
                                                                 & test_copper_lists__DOT__dut__DOT__unnamedblk2__DOT__i)] 
                = vlSelfRef.test_copper_lists__DOT__dut__DOT____Vlvbound_h652b6de2__0;
        }
        test_copper_lists__DOT__dut__DOT__unnamedblk2__DOT__i 
            = ((IData)(1U) + test_copper_lists__DOT__dut__DOT__unnamedblk2__DOT__i);
    }
    test_copper_lists__DOT__dut__DOT__unnamedblk3__DOT__i = 0U;
    while (VL_GTS_III(32, 0x00000800U, test_copper_lists__DOT__dut__DOT__unnamedblk3__DOT__i)) {
        vlSelfRef.test_copper_lists__DOT__dut__DOT__sprite_shapes[(0x000007ffU 
                                                                   & test_copper_lists__DOT__dut__DOT__unnamedblk3__DOT__i)] = 0U;
        test_copper_lists__DOT__dut__DOT__unnamedblk3__DOT__i 
            = ((IData)(1U) + test_copper_lists__DOT__dut__DOT__unnamedblk3__DOT__i);
    }
    test_copper_lists__DOT__dut__DOT__spr_x[0U] = 0U;
    test_copper_lists__DOT__dut__DOT__spr_y[0U] = 0U;
    test_copper_lists__DOT__dut__DOT__spr_enable[0U] = 0U;
    test_copper_lists__DOT__dut__DOT__spr_flip_h[0U] = 0U;
    test_copper_lists__DOT__dut__DOT__spr_flip_v[0U] = 0U;
    test_copper_lists__DOT__dut__DOT__spr_pri[0U] = 1U;
    test_copper_lists__DOT__dut__DOT__spr_shape[0U] = 0U;
    test_copper_lists__DOT__dut__DOT__spr_x[1U] = 0U;
    test_copper_lists__DOT__dut__DOT__spr_y[1U] = 0U;
    test_copper_lists__DOT__dut__DOT__spr_enable[1U] = 0U;
    test_copper_lists__DOT__dut__DOT__spr_flip_h[1U] = 0U;
    test_copper_lists__DOT__dut__DOT__spr_flip_v[1U] = 0U;
    test_copper_lists__DOT__dut__DOT__spr_pri[1U] = 1U;
    test_copper_lists__DOT__dut__DOT__spr_shape[1U] = 1U;
    test_copper_lists__DOT__dut__DOT__spr_x[2U] = 0U;
    test_copper_lists__DOT__dut__DOT__spr_y[2U] = 0U;
    test_copper_lists__DOT__dut__DOT__spr_enable[2U] = 0U;
    test_copper_lists__DOT__dut__DOT__spr_flip_h[2U] = 0U;
    test_copper_lists__DOT__dut__DOT__spr_flip_v[2U] = 0U;
    test_copper_lists__DOT__dut__DOT__spr_pri[2U] = 1U;
    test_copper_lists__DOT__dut__DOT__spr_shape[2U] = 2U;
    test_copper_lists__DOT__dut__DOT__spr_x[3U] = 0U;
    test_copper_lists__DOT__dut__DOT__spr_y[3U] = 0U;
    test_copper_lists__DOT__dut__DOT__spr_enable[3U] = 0U;
    test_copper_lists__DOT__dut__DOT__spr_flip_h[3U] = 0U;
    test_copper_lists__DOT__dut__DOT__spr_flip_v[3U] = 0U;
    test_copper_lists__DOT__dut__DOT__spr_pri[3U] = 1U;
    test_copper_lists__DOT__dut__DOT__spr_shape[3U] = 3U;
    test_copper_lists__DOT__dut__DOT__spr_x[4U] = 0U;
    test_copper_lists__DOT__dut__DOT__spr_y[4U] = 0U;
    test_copper_lists__DOT__dut__DOT__spr_enable[4U] = 0U;
    test_copper_lists__DOT__dut__DOT__spr_flip_h[4U] = 0U;
    test_copper_lists__DOT__dut__DOT__spr_flip_v[4U] = 0U;
    test_copper_lists__DOT__dut__DOT__spr_pri[4U] = 1U;
    test_copper_lists__DOT__dut__DOT__spr_shape[4U] = 4U;
    test_copper_lists__DOT__dut__DOT__spr_x[5U] = 0U;
    test_copper_lists__DOT__dut__DOT__spr_y[5U] = 0U;
    test_copper_lists__DOT__dut__DOT__spr_enable[5U] = 0U;
    test_copper_lists__DOT__dut__DOT__spr_flip_h[5U] = 0U;
    test_copper_lists__DOT__dut__DOT__spr_flip_v[5U] = 0U;
    test_copper_lists__DOT__dut__DOT__spr_pri[5U] = 1U;
    test_copper_lists__DOT__dut__DOT__spr_shape[5U] = 5U;
    test_copper_lists__DOT__dut__DOT__spr_x[6U] = 0U;
    test_copper_lists__DOT__dut__DOT__spr_y[6U] = 0U;
    test_copper_lists__DOT__dut__DOT__spr_enable[6U] = 0U;
    test_copper_lists__DOT__dut__DOT__spr_flip_h[6U] = 0U;
    test_copper_lists__DOT__dut__DOT__spr_flip_v[6U] = 0U;
    test_copper_lists__DOT__dut__DOT__spr_pri[6U] = 1U;
    test_copper_lists__DOT__dut__DOT__spr_shape[6U] = 6U;
    test_copper_lists__DOT__dut__DOT__spr_x[7U] = 0U;
    test_copper_lists__DOT__dut__DOT__spr_y[7U] = 0U;
    test_copper_lists__DOT__dut__DOT__spr_enable[7U] = 0U;
    test_copper_lists__DOT__dut__DOT__spr_flip_h[7U] = 0U;
    test_copper_lists__DOT__dut__DOT__spr_flip_v[7U] = 0U;
    test_copper_lists__DOT__dut__DOT__spr_pri[7U] = 1U;
    test_copper_lists__DOT__dut__DOT__spr_shape[7U] = 7U;
    test_copper_lists__DOT__dut__DOT__spr_x[8U] = 0U;
    test_copper_lists__DOT__dut__DOT__spr_y[8U] = 0U;
    test_copper_lists__DOT__dut__DOT__spr_enable[8U] = 0U;
    test_copper_lists__DOT__dut__DOT__spr_flip_h[8U] = 0U;
    test_copper_lists__DOT__dut__DOT__spr_flip_v[8U] = 0U;
    test_copper_lists__DOT__dut__DOT__spr_pri[8U] = 1U;
    test_copper_lists__DOT__dut__DOT__spr_shape[8U] = 8U;
    test_copper_lists__DOT__dut__DOT__spr_x[9U] = 0U;
    test_copper_lists__DOT__dut__DOT__spr_y[9U] = 0U;
    test_copper_lists__DOT__dut__DOT__spr_enable[9U] = 0U;
    test_copper_lists__DOT__dut__DOT__spr_flip_h[9U] = 0U;
    test_copper_lists__DOT__dut__DOT__spr_flip_v[9U] = 0U;
    test_copper_lists__DOT__dut__DOT__spr_pri[9U] = 1U;
    test_copper_lists__DOT__dut__DOT__spr_shape[9U] = 9U;
    test_copper_lists__DOT__dut__DOT__spr_x[10U] = 0U;
    test_copper_lists__DOT__dut__DOT__spr_y[10U] = 0U;
    test_copper_lists__DOT__dut__DOT__spr_enable[10U] = 0U;
    test_copper_lists__DOT__dut__DOT__spr_flip_h[10U] = 0U;
    test_copper_lists__DOT__dut__DOT__spr_flip_v[10U] = 0U;
    test_copper_lists__DOT__dut__DOT__spr_pri[10U] = 1U;
    test_copper_lists__DOT__dut__DOT__spr_shape[10U] = 0x0aU;
    test_copper_lists__DOT__dut__DOT__spr_x[11U] = 0U;
    test_copper_lists__DOT__dut__DOT__spr_y[11U] = 0U;
    test_copper_lists__DOT__dut__DOT__spr_enable[11U] = 0U;
    test_copper_lists__DOT__dut__DOT__spr_flip_h[11U] = 0U;
    test_copper_lists__DOT__dut__DOT__spr_flip_v[11U] = 0U;
    test_copper_lists__DOT__dut__DOT__spr_pri[11U] = 1U;
    test_copper_lists__DOT__dut__DOT__spr_shape[11U] = 0x0bU;
    test_copper_lists__DOT__dut__DOT__spr_x[12U] = 0U;
    test_copper_lists__DOT__dut__DOT__spr_y[12U] = 0U;
    test_copper_lists__DOT__dut__DOT__spr_enable[12U] = 0U;
    test_copper_lists__DOT__dut__DOT__spr_flip_h[12U] = 0U;
    test_copper_lists__DOT__dut__DOT__spr_flip_v[12U] = 0U;
    test_copper_lists__DOT__dut__DOT__spr_pri[12U] = 1U;
    test_copper_lists__DOT__dut__DOT__spr_shape[12U] = 0x0cU;
    test_copper_lists__DOT__dut__DOT__spr_x[13U] = 0U;
    test_copper_lists__DOT__dut__DOT__spr_y[13U] = 0U;
    test_copper_lists__DOT__dut__DOT__spr_enable[13U] = 0U;
    test_copper_lists__DOT__dut__DOT__spr_flip_h[13U] = 0U;
    test_copper_lists__DOT__dut__DOT__spr_flip_v[13U] = 0U;
    test_copper_lists__DOT__dut__DOT__spr_pri[13U] = 1U;
    test_copper_lists__DOT__dut__DOT__spr_shape[13U] = 0x0dU;
    test_copper_lists__DOT__dut__DOT__spr_x[14U] = 0U;
    test_copper_lists__DOT__dut__DOT__spr_y[14U] = 0U;
    test_copper_lists__DOT__dut__DOT__spr_enable[14U] = 0U;
    test_copper_lists__DOT__dut__DOT__spr_flip_h[14U] = 0U;
    test_copper_lists__DOT__dut__DOT__spr_flip_v[14U] = 0U;
    test_copper_lists__DOT__dut__DOT__spr_pri[14U] = 1U;
    test_copper_lists__DOT__dut__DOT__spr_shape[14U] = 0x0eU;
    test_copper_lists__DOT__dut__DOT__spr_x[15U] = 0U;
    test_copper_lists__DOT__dut__DOT__spr_y[15U] = 0U;
    test_copper_lists__DOT__dut__DOT__spr_enable[15U] = 0U;
    test_copper_lists__DOT__dut__DOT__spr_flip_h[15U] = 0U;
    test_copper_lists__DOT__dut__DOT__spr_flip_v[15U] = 0U;
    test_copper_lists__DOT__dut__DOT__spr_pri[15U] = 1U;
    test_copper_lists__DOT__dut__DOT__spr_shape[15U] = 0x0fU;
    VL_READMEM_N(true, 8, 2048, 0, "rom/cp437.hex"s
                 ,  &(vlSelfRef.test_copper_lists__DOT__dut__DOT__font_rom)
                 , 0, ~0ULL);
    vlSelfRef.test_copper_lists__DOT__dut__DOT__copper_pos[0U] = 0U;
    vlSelfRef.test_copper_lists__DOT__dut__DOT__copper_reg[0U] = 0U;
    vlSelfRef.test_copper_lists__DOT__dut__DOT__copper_val[0U] = 0U;
    vlSelfRef.test_copper_lists__DOT__dut__DOT__copper_pos[1U] = 0U;
    vlSelfRef.test_copper_lists__DOT__dut__DOT__copper_reg[1U] = 0U;
    vlSelfRef.test_copper_lists__DOT__dut__DOT__copper_val[1U] = 0U;
    vlSelfRef.test_copper_lists__DOT__dut__DOT__copper_pos[2U] = 0U;
    vlSelfRef.test_copper_lists__DOT__dut__DOT__copper_reg[2U] = 0U;
    vlSelfRef.test_copper_lists__DOT__dut__DOT__copper_val[2U] = 0U;
    vlSelfRef.test_copper_lists__DOT__dut__DOT__copper_pos[3U] = 0U;
    vlSelfRef.test_copper_lists__DOT__dut__DOT__copper_reg[3U] = 0U;
    vlSelfRef.test_copper_lists__DOT__dut__DOT__copper_val[3U] = 0U;
    vlSelfRef.test_copper_lists__DOT__dut__DOT__copper_pos[4U] = 0U;
    vlSelfRef.test_copper_lists__DOT__dut__DOT__copper_reg[4U] = 0U;
    vlSelfRef.test_copper_lists__DOT__dut__DOT__copper_val[4U] = 0U;
    vlSelfRef.test_copper_lists__DOT__dut__DOT__copper_pos[5U] = 0U;
    vlSelfRef.test_copper_lists__DOT__dut__DOT__copper_reg[5U] = 0U;
    vlSelfRef.test_copper_lists__DOT__dut__DOT__copper_val[5U] = 0U;
    vlSelfRef.test_copper_lists__DOT__dut__DOT__copper_pos[6U] = 0U;
    vlSelfRef.test_copper_lists__DOT__dut__DOT__copper_reg[6U] = 0U;
    vlSelfRef.test_copper_lists__DOT__dut__DOT__copper_val[6U] = 0U;
    vlSelfRef.test_copper_lists__DOT__dut__DOT__copper_pos[7U] = 0U;
    vlSelfRef.test_copper_lists__DOT__dut__DOT__copper_reg[7U] = 0U;
    vlSelfRef.test_copper_lists__DOT__dut__DOT__copper_val[7U] = 0U;
    vlSelfRef.test_copper_lists__DOT__dut__DOT__copper_pos[8U] = 0U;
    vlSelfRef.test_copper_lists__DOT__dut__DOT__copper_reg[8U] = 0U;
    vlSelfRef.test_copper_lists__DOT__dut__DOT__copper_val[8U] = 0U;
    vlSelfRef.test_copper_lists__DOT__dut__DOT__copper_pos[9U] = 0U;
    vlSelfRef.test_copper_lists__DOT__dut__DOT__copper_reg[9U] = 0U;
    vlSelfRef.test_copper_lists__DOT__dut__DOT__copper_val[9U] = 0U;
    vlSelfRef.test_copper_lists__DOT__dut__DOT__copper_pos[10U] = 0U;
    vlSelfRef.test_copper_lists__DOT__dut__DOT__copper_reg[10U] = 0U;
    vlSelfRef.test_copper_lists__DOT__dut__DOT__copper_val[10U] = 0U;
    vlSelfRef.test_copper_lists__DOT__dut__DOT__copper_pos[11U] = 0U;
    vlSelfRef.test_copper_lists__DOT__dut__DOT__copper_reg[11U] = 0U;
    vlSelfRef.test_copper_lists__DOT__dut__DOT__copper_val[11U] = 0U;
    vlSelfRef.test_copper_lists__DOT__dut__DOT__copper_pos[12U] = 0U;
    vlSelfRef.test_copper_lists__DOT__dut__DOT__copper_reg[12U] = 0U;
    vlSelfRef.test_copper_lists__DOT__dut__DOT__copper_val[12U] = 0U;
    vlSelfRef.test_copper_lists__DOT__dut__DOT__copper_pos[13U] = 0U;
    vlSelfRef.test_copper_lists__DOT__dut__DOT__copper_reg[13U] = 0U;
    vlSelfRef.test_copper_lists__DOT__dut__DOT__copper_val[13U] = 0U;
    vlSelfRef.test_copper_lists__DOT__dut__DOT__copper_pos[14U] = 0U;
    vlSelfRef.test_copper_lists__DOT__dut__DOT__copper_reg[14U] = 0U;
    vlSelfRef.test_copper_lists__DOT__dut__DOT__copper_val[14U] = 0U;
    vlSelfRef.test_copper_lists__DOT__dut__DOT__copper_pos[15U] = 0U;
    vlSelfRef.test_copper_lists__DOT__dut__DOT__copper_reg[15U] = 0U;
    vlSelfRef.test_copper_lists__DOT__dut__DOT__copper_val[15U] = 0U;
    vlSelfRef.test_copper_lists__DOT__dut__DOT__copper_pos[16U] = 0U;
    vlSelfRef.test_copper_lists__DOT__dut__DOT__copper_reg[16U] = 0U;
    vlSelfRef.test_copper_lists__DOT__dut__DOT__copper_val[16U] = 0U;
    vlSelfRef.test_copper_lists__DOT__dut__DOT__copper_pos[17U] = 0U;
    vlSelfRef.test_copper_lists__DOT__dut__DOT__copper_reg[17U] = 0U;
    vlSelfRef.test_copper_lists__DOT__dut__DOT__copper_val[17U] = 0U;
    vlSelfRef.test_copper_lists__DOT__dut__DOT__copper_pos[18U] = 0U;
    vlSelfRef.test_copper_lists__DOT__dut__DOT__copper_reg[18U] = 0U;
    vlSelfRef.test_copper_lists__DOT__dut__DOT__copper_val[18U] = 0U;
    vlSelfRef.test_copper_lists__DOT__dut__DOT__copper_pos[19U] = 0U;
    vlSelfRef.test_copper_lists__DOT__dut__DOT__copper_reg[19U] = 0U;
    vlSelfRef.test_copper_lists__DOT__dut__DOT__copper_val[19U] = 0U;
    vlSelfRef.test_copper_lists__DOT__dut__DOT__copper_pos[20U] = 0U;
    vlSelfRef.test_copper_lists__DOT__dut__DOT__copper_reg[20U] = 0U;
    vlSelfRef.test_copper_lists__DOT__dut__DOT__copper_val[20U] = 0U;
    vlSelfRef.test_copper_lists__DOT__dut__DOT__copper_pos[21U] = 0U;
    vlSelfRef.test_copper_lists__DOT__dut__DOT__copper_reg[21U] = 0U;
    vlSelfRef.test_copper_lists__DOT__dut__DOT__copper_val[21U] = 0U;
    vlSelfRef.test_copper_lists__DOT__dut__DOT__copper_pos[22U] = 0U;
    vlSelfRef.test_copper_lists__DOT__dut__DOT__copper_reg[22U] = 0U;
    vlSelfRef.test_copper_lists__DOT__dut__DOT__copper_val[22U] = 0U;
    vlSelfRef.test_copper_lists__DOT__dut__DOT__copper_pos[23U] = 0U;
    vlSelfRef.test_copper_lists__DOT__dut__DOT__copper_reg[23U] = 0U;
    vlSelfRef.test_copper_lists__DOT__dut__DOT__copper_val[23U] = 0U;
    vlSelfRef.test_copper_lists__DOT__dut__DOT__copper_pos[24U] = 0U;
    vlSelfRef.test_copper_lists__DOT__dut__DOT__copper_reg[24U] = 0U;
    vlSelfRef.test_copper_lists__DOT__dut__DOT__copper_val[24U] = 0U;
    vlSelfRef.test_copper_lists__DOT__dut__DOT__copper_pos[25U] = 0U;
    vlSelfRef.test_copper_lists__DOT__dut__DOT__copper_reg[25U] = 0U;
    vlSelfRef.test_copper_lists__DOT__dut__DOT__copper_val[25U] = 0U;
    vlSelfRef.test_copper_lists__DOT__dut__DOT__copper_pos[26U] = 0U;
    vlSelfRef.test_copper_lists__DOT__dut__DOT__copper_reg[26U] = 0U;
    vlSelfRef.test_copper_lists__DOT__dut__DOT__copper_val[26U] = 0U;
    vlSelfRef.test_copper_lists__DOT__dut__DOT__copper_pos[27U] = 0U;
    vlSelfRef.test_copper_lists__DOT__dut__DOT__copper_reg[27U] = 0U;
    vlSelfRef.test_copper_lists__DOT__dut__DOT__copper_val[27U] = 0U;
    vlSelfRef.test_copper_lists__DOT__dut__DOT__copper_pos[28U] = 0U;
    vlSelfRef.test_copper_lists__DOT__dut__DOT__copper_reg[28U] = 0U;
    vlSelfRef.test_copper_lists__DOT__dut__DOT__copper_val[28U] = 0U;
    vlSelfRef.test_copper_lists__DOT__dut__DOT__copper_pos[29U] = 0U;
    vlSelfRef.test_copper_lists__DOT__dut__DOT__copper_reg[29U] = 0U;
    vlSelfRef.test_copper_lists__DOT__dut__DOT__copper_val[29U] = 0U;
    vlSelfRef.test_copper_lists__DOT__dut__DOT__copper_pos[30U] = 0U;
    vlSelfRef.test_copper_lists__DOT__dut__DOT__copper_reg[30U] = 0U;
    vlSelfRef.test_copper_lists__DOT__dut__DOT__copper_val[30U] = 0U;
    vlSelfRef.test_copper_lists__DOT__dut__DOT__copper_pos[31U] = 0U;
    vlSelfRef.test_copper_lists__DOT__dut__DOT__copper_reg[31U] = 0U;
    vlSelfRef.test_copper_lists__DOT__dut__DOT__copper_val[31U] = 0U;
    test_copper_lists__DOT__dut__DOT__unnamedblk6__DOT__i = 0U;
    while (VL_GTS_III(32, 0x00001000U, test_copper_lists__DOT__dut__DOT__unnamedblk6__DOT__i)) {
        vlSelfRef.test_copper_lists__DOT__dut__DOT__copper_list_pos[(0x00000fffU 
                                                                     & test_copper_lists__DOT__dut__DOT__unnamedblk6__DOT__i)] = 0U;
        vlSelfRef.test_copper_lists__DOT__dut__DOT__copper_list_reg[(0x00000fffU 
                                                                     & test_copper_lists__DOT__dut__DOT__unnamedblk6__DOT__i)] = 0U;
        vlSelfRef.test_copper_lists__DOT__dut__DOT__copper_list_val[(0x00000fffU 
                                                                     & test_copper_lists__DOT__dut__DOT__unnamedblk6__DOT__i)] = 0U;
        test_copper_lists__DOT__dut__DOT__unnamedblk6__DOT__i 
            = ((IData)(1U) + test_copper_lists__DOT__dut__DOT__unnamedblk6__DOT__i);
    }
    test_copper_lists__DOT__dut__DOT__unnamedblk7__DOT__i = 0U;
    while (VL_GTS_III(32, 0x00000080U, test_copper_lists__DOT__dut__DOT__unnamedblk7__DOT__i)) {
        vlSelfRef.test_copper_lists__DOT__dut__DOT__copper_list_count[(0x0000007fU 
                                                                       & test_copper_lists__DOT__dut__DOT__unnamedblk7__DOT__i)] = 0U;
        test_copper_lists__DOT__dut__DOT__unnamedblk7__DOT__i 
            = ((IData)(1U) + test_copper_lists__DOT__dut__DOT__unnamedblk7__DOT__i);
    }
    vlSelfRef.test_copper_lists__DOT__dut__DOT__copper_count = 0U;
    vlSelfRef.test_copper_lists__DOT__dut__DOT__copper_index = 0U;
    vlSelfRef.test_copper_lists__DOT__dut__DOT__copper_enabled = 0U;
    vlSelfRef.test_copper_lists__DOT__dut__DOT__copper_target_list = 0U;
    vlSelfRef.test_copper_lists__DOT__dut__DOT__copper_active_list = 0U;
    vlSelfRef.test_copper_lists__DOT__dut__DOT__copper_pending_list = 0U;
    vlSelfRef.test_copper_lists__DOT__dut__DOT__copper_loading = 0U;
    vlSelfRef.test_copper_lists__DOT__dut__DOT__copper_load_idx = 0U;
    vlSelfRef.test_copper_lists__DOT__dut__DOT__copper_load_src = 0U;
    __Vilp1 = 0U;
    while ((__Vilp1 <= 0x0000003fU)) {
        vlSelfRef.test_copper_lists__DOT__dut__DOT__fio_name[__Vilp1] = 0U;
        __Vilp1 = ((IData)(1U) + __Vilp1);
    }
    vlSelfRef.test_copper_lists__DOT__dut__DOT__fio_name_len = 0U;
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
    vlSelfRef.test_copper_lists__DOT__dut__DOT__cursor_x = 0U;
    vlSelfRef.test_copper_lists__DOT__dut__DOT__cursor_y = 0U;
    vlSelfRef.test_copper_lists__DOT__dut__DOT__fg_color = 1U;
    vlSelfRef.test_copper_lists__DOT__dut__DOT__bg_color = 6U;
    vlSelfRef.test_copper_lists__DOT__dut__DOT__gfx_color = 1U;
    test_copper_lists__DOT__dut__DOT__spr_trans[0U] = 0U;
    test_copper_lists__DOT__dut__DOT__spr_trans[1U] = 0U;
    test_copper_lists__DOT__dut__DOT__spr_trans[2U] = 0U;
    test_copper_lists__DOT__dut__DOT__spr_trans[3U] = 0U;
    test_copper_lists__DOT__dut__DOT__spr_trans[4U] = 0U;
    test_copper_lists__DOT__dut__DOT__spr_trans[5U] = 0U;
    test_copper_lists__DOT__dut__DOT__spr_trans[6U] = 0U;
    test_copper_lists__DOT__dut__DOT__spr_trans[7U] = 0U;
    test_copper_lists__DOT__dut__DOT__spr_trans[8U] = 0U;
    test_copper_lists__DOT__dut__DOT__spr_trans[9U] = 0U;
    test_copper_lists__DOT__dut__DOT__spr_trans[10U] = 0U;
    test_copper_lists__DOT__dut__DOT__spr_trans[11U] = 0U;
    test_copper_lists__DOT__dut__DOT__spr_trans[12U] = 0U;
    test_copper_lists__DOT__dut__DOT__spr_trans[13U] = 0U;
    test_copper_lists__DOT__dut__DOT__spr_trans[14U] = 0U;
    test_copper_lists__DOT__dut__DOT__spr_trans[15U] = 0U;
    vlSelfRef.test_copper_lists__DOT__dut__DOT__scroll_offset = 0U;
    vlSelfRef.test_copper_lists__DOT__dut__DOT__scroll_pending = 0U;
    vlSelfRef.test_copper_lists__DOT__dut__DOT__scroll_col = 0U;
    vlSelfRef.test_copper_lists__DOT__dut__DOT__cmd_busy = 0U;
    vlSelfRef.test_copper_lists__DOT__dut__DOT__fill_sp = 0U;
    vlSelfRef.test_copper_lists__DOT__dut__DOT__tile_inst__DOT__tregs[0U] = 0U;
    vlSelfRef.test_copper_lists__DOT__dut__DOT__tile_inst__DOT__tregs[1U] = 0U;
    vlSelfRef.test_copper_lists__DOT__dut__DOT__tile_inst__DOT__tregs[2U] = 0U;
    vlSelfRef.test_copper_lists__DOT__dut__DOT__tile_inst__DOT__tregs[3U] = 0U;
    vlSelfRef.test_copper_lists__DOT__dut__DOT__tile_inst__DOT__tregs[4U] = 0U;
    vlSelfRef.test_copper_lists__DOT__dut__DOT__tile_inst__DOT__tregs[5U] = 0U;
    vlSelfRef.test_copper_lists__DOT__dut__DOT__tile_inst__DOT__tregs[6U] = 0U;
    vlSelfRef.test_copper_lists__DOT__dut__DOT__tile_inst__DOT__tregs[7U] = 0U;
    vlSelfRef.test_copper_lists__DOT__dut__DOT__tile_inst__DOT__tregs[8U] = 0U;
    vlSelfRef.test_copper_lists__DOT__dut__DOT__tile_inst__DOT__tregs[9U] = 0U;
    vlSelfRef.test_copper_lists__DOT__dut__DOT__tile_inst__DOT__tregs[10U] = 0U;
    vlSelfRef.test_copper_lists__DOT__dut__DOT__tile_inst__DOT__tregs[11U] = 0U;
    vlSelfRef.test_copper_lists__DOT__dut__DOT__tile_inst__DOT__tregs[12U] = 0U;
    vlSelfRef.test_copper_lists__DOT__dut__DOT__tile_inst__DOT__tregs[13U] = 0U;
    vlSelfRef.test_copper_lists__DOT__dut__DOT__tile_inst__DOT__tregs[14U] = 0U;
    vlSelfRef.test_copper_lists__DOT__dut__DOT__tile_inst__DOT__tregs[15U] = 0U;
    vlSelfRef.test_copper_lists__DOT__dut__DOT__tile_inst__DOT__tregs[16U] = 0U;
    vlSelfRef.test_copper_lists__DOT__dut__DOT__tile_inst__DOT__tregs[17U] = 0U;
    vlSelfRef.test_copper_lists__DOT__dut__DOT__tile_inst__DOT__tregs[18U] = 0U;
    vlSelfRef.test_copper_lists__DOT__dut__DOT__tile_inst__DOT__tregs[19U] = 0U;
    vlSelfRef.test_copper_lists__DOT__dut__DOT__tile_inst__DOT__tregs[20U] = 0U;
    vlSelfRef.test_copper_lists__DOT__dut__DOT__tile_inst__DOT__tregs[21U] = 0U;
    vlSelfRef.test_copper_lists__DOT__dut__DOT__tile_inst__DOT__tregs[22U] = 0U;
    vlSelfRef.test_copper_lists__DOT__dut__DOT__tile_inst__DOT__tregs[23U] = 0U;
    vlSelfRef.test_copper_lists__DOT__dut__DOT__tile_inst__DOT__tregs[24U] = 0U;
    vlSelfRef.test_copper_lists__DOT__dut__DOT__tile_inst__DOT__tregs[25U] = 0U;
    vlSelfRef.test_copper_lists__DOT__dut__DOT__tile_inst__DOT__tregs[26U] = 0U;
    vlSelfRef.test_copper_lists__DOT__dut__DOT__tile_inst__DOT__tregs[27U] = 0U;
    vlSelfRef.test_copper_lists__DOT__dut__DOT__tile_inst__DOT__tregs[28U] = 0U;
    vlSelfRef.test_copper_lists__DOT__dut__DOT__tile_inst__DOT__tregs[29U] = 0U;
    vlSelfRef.test_copper_lists__DOT__dut__DOT__tile_inst__DOT__tregs[30U] = 0U;
    vlSelfRef.test_copper_lists__DOT__dut__DOT__tile_inst__DOT__tregs[31U] = 0U;
    test_copper_lists__DOT__dut__DOT__tile_inst__DOT__unnamedblk2__DOT__i = 0U;
    while (VL_GTS_III(32, 0x00008000U, test_copper_lists__DOT__dut__DOT__tile_inst__DOT__unnamedblk2__DOT__i)) {
        vlSelfRef.test_copper_lists__DOT__dut__DOT__tile_inst__DOT__tile_data[(0x00007fffU 
                                                                               & test_copper_lists__DOT__dut__DOT__tile_inst__DOT__unnamedblk2__DOT__i)] = 0U;
        test_copper_lists__DOT__dut__DOT__tile_inst__DOT__unnamedblk2__DOT__i 
            = ((IData)(1U) + test_copper_lists__DOT__dut__DOT__tile_inst__DOT__unnamedblk2__DOT__i);
    }
    test_copper_lists__DOT__dut__DOT__tile_inst__DOT__unnamedblk3__DOT__i = 0U;
    while (VL_GTS_III(32, 0x00000fa0U, test_copper_lists__DOT__dut__DOT__tile_inst__DOT__unnamedblk3__DOT__i)) {
        vlSelfRef.test_copper_lists__DOT__dut__DOT__tile_inst__DOT____Vlvbound_h9afe376d__0 = 0U;
        vlSelfRef.test_copper_lists__DOT__dut__DOT__tile_inst__DOT____Vlvbound_h7ed4573b__0 = 0U;
        if (VL_LIKELY(((0x0f9fU >= (0x00000fffU & test_copper_lists__DOT__dut__DOT__tile_inst__DOT__unnamedblk3__DOT__i))))) {
            vlSelfRef.test_copper_lists__DOT__dut__DOT__tile_inst__DOT__nametable[(0x00000fffU 
                                                                                & test_copper_lists__DOT__dut__DOT__tile_inst__DOT__unnamedblk3__DOT__i)] 
                = vlSelfRef.test_copper_lists__DOT__dut__DOT__tile_inst__DOT____Vlvbound_h9afe376d__0;
            vlSelfRef.test_copper_lists__DOT__dut__DOT__tile_inst__DOT__attr_table[(0x00000fffU 
                                                                                & test_copper_lists__DOT__dut__DOT__tile_inst__DOT__unnamedblk3__DOT__i)] 
                = vlSelfRef.test_copper_lists__DOT__dut__DOT__tile_inst__DOT____Vlvbound_h7ed4573b__0;
        }
        test_copper_lists__DOT__dut__DOT__tile_inst__DOT__unnamedblk3__DOT__i 
            = ((IData)(1U) + test_copper_lists__DOT__dut__DOT__tile_inst__DOT__unnamedblk3__DOT__i);
    }
    test_copper_lists__DOT__dut__DOT__tile_inst__DOT__pal_ram[0U] = 0U;
    test_copper_lists__DOT__dut__DOT__tile_inst__DOT__pal_ram[1U] = 0x0fffU;
    test_copper_lists__DOT__dut__DOT__tile_inst__DOT__pal_ram[2U] = 0x0800U;
    test_copper_lists__DOT__dut__DOT__tile_inst__DOT__pal_ram[3U] = 0x0afeU;
    test_copper_lists__DOT__dut__DOT__tile_inst__DOT__pal_ram[4U] = 0x0c4cU;
    test_copper_lists__DOT__dut__DOT__tile_inst__DOT__pal_ram[5U] = 0x00c5U;
    test_copper_lists__DOT__dut__DOT__tile_inst__DOT__pal_ram[6U] = 0x000aU;
    test_copper_lists__DOT__dut__DOT__tile_inst__DOT__pal_ram[7U] = 0x0ee7U;
    test_copper_lists__DOT__dut__DOT__tile_inst__DOT__pal_ram[8U] = 0x0d85U;
    test_copper_lists__DOT__dut__DOT__tile_inst__DOT__pal_ram[9U] = 0x0640U;
    test_copper_lists__DOT__dut__DOT__tile_inst__DOT__pal_ram[10U] = 0x0f77U;
    test_copper_lists__DOT__dut__DOT__tile_inst__DOT__pal_ram[11U] = 0x0333U;
    test_copper_lists__DOT__dut__DOT__tile_inst__DOT__pal_ram[12U] = 0x0777U;
    test_copper_lists__DOT__dut__DOT__tile_inst__DOT__pal_ram[13U] = 0x08f6U;
    test_copper_lists__DOT__dut__DOT__tile_inst__DOT__pal_ram[14U] = 0x008fU;
    test_copper_lists__DOT__dut__DOT__tile_inst__DOT__pal_ram[15U] = 0x0bbbU;
    test_copper_lists__DOT__dut__DOT__tile_inst__DOT__unnamedblk4__DOT__i = 0x00000010U;
    while (VL_GTS_III(32, 0x00000100U, test_copper_lists__DOT__dut__DOT__tile_inst__DOT__unnamedblk4__DOT__i)) {
        test_copper_lists__DOT__dut__DOT__tile_inst__DOT__pal_ram[(0x000000ffU 
                                                                   & test_copper_lists__DOT__dut__DOT__tile_inst__DOT__unnamedblk4__DOT__i)] = 0U;
        test_copper_lists__DOT__dut__DOT__tile_inst__DOT__unnamedblk4__DOT__i 
            = ((IData)(1U) + test_copper_lists__DOT__dut__DOT__tile_inst__DOT__unnamedblk4__DOT__i);
    }
    vlSelfRef.test_copper_lists__DOT__dut__DOT__tile_inst__DOT__col_buffer[0U] = 0U;
    vlSelfRef.test_copper_lists__DOT__dut__DOT__tile_inst__DOT__col_buffer[1U] = 0U;
    vlSelfRef.test_copper_lists__DOT__dut__DOT__tile_inst__DOT__col_buffer[2U] = 0U;
    vlSelfRef.test_copper_lists__DOT__dut__DOT__tile_inst__DOT__col_buffer[3U] = 0U;
    vlSelfRef.test_copper_lists__DOT__dut__DOT__tile_inst__DOT__col_buffer[4U] = 0U;
    vlSelfRef.test_copper_lists__DOT__dut__DOT__tile_inst__DOT__col_buffer[5U] = 0U;
    vlSelfRef.test_copper_lists__DOT__dut__DOT__tile_inst__DOT__col_buffer[6U] = 0U;
    vlSelfRef.test_copper_lists__DOT__dut__DOT__tile_inst__DOT__col_buffer[7U] = 0U;
    vlSelfRef.test_copper_lists__DOT__dut__DOT__tile_inst__DOT__col_buffer[8U] = 0U;
    vlSelfRef.test_copper_lists__DOT__dut__DOT__tile_inst__DOT__col_buffer[9U] = 0U;
    vlSelfRef.test_copper_lists__DOT__dut__DOT__tile_inst__DOT__col_buffer[10U] = 0U;
    vlSelfRef.test_copper_lists__DOT__dut__DOT__tile_inst__DOT__col_buffer[11U] = 0U;
    vlSelfRef.test_copper_lists__DOT__dut__DOT__tile_inst__DOT__col_buffer[12U] = 0U;
    vlSelfRef.test_copper_lists__DOT__dut__DOT__tile_inst__DOT__col_buffer[13U] = 0U;
    vlSelfRef.test_copper_lists__DOT__dut__DOT__tile_inst__DOT__col_buffer[14U] = 0U;
    vlSelfRef.test_copper_lists__DOT__dut__DOT__tile_inst__DOT__col_buffer[15U] = 0U;
    vlSelfRef.test_copper_lists__DOT__dut__DOT__tile_inst__DOT__col_buffer[16U] = 0U;
    vlSelfRef.test_copper_lists__DOT__dut__DOT__tile_inst__DOT__col_buffer[17U] = 0U;
    vlSelfRef.test_copper_lists__DOT__dut__DOT__tile_inst__DOT__col_buffer[18U] = 0U;
    vlSelfRef.test_copper_lists__DOT__dut__DOT__tile_inst__DOT__col_buffer[19U] = 0U;
    vlSelfRef.test_copper_lists__DOT__dut__DOT__tile_inst__DOT__col_buffer[20U] = 0U;
    vlSelfRef.test_copper_lists__DOT__dut__DOT__tile_inst__DOT__col_buffer[21U] = 0U;
    vlSelfRef.test_copper_lists__DOT__dut__DOT__tile_inst__DOT__col_buffer[22U] = 0U;
    vlSelfRef.test_copper_lists__DOT__dut__DOT__tile_inst__DOT__col_buffer[23U] = 0U;
    vlSelfRef.test_copper_lists__DOT__dut__DOT__tile_inst__DOT__col_buffer[24U] = 0U;
    vlSelfRef.test_copper_lists__DOT__dut__DOT__tile_inst__DOT__tile_size16 = 0U;
    vlSelfRef.test_copper_lists__DOT__dut__DOT__tile_inst__DOT__dma_state = 0U;
    vlSelfRef.test_copper_lists__DOT__dut__DOT__tile_inst__DOT__dma_src = 0U;
    vlSelfRef.test_copper_lists__DOT__dut__DOT__tile_inst__DOT__dma_count = 0U;
    vlSelfRef.test_copper_lists__DOT__dut__DOT__tile_inst__DOT__dma_dst = 0U;
    vlSelfRef.test_copper_lists__DOT__dut__DOT__tile_inst__DOT__dma_nt = 0U;
    vlSelfRef.test_copper_lists__DOT__dut__DOT__tile_inst__DOT__dma_col_row = 0U;
    vlSelfRef.test_copper_lists__DOT__dut__DOT__tile_inst__DOT__dma_col_idx = 0U;
    vlSelfRef.test_copper_lists__DOT__dut__DOT__tile_inst__DOT__dma_data_valid = 0U;
    vlSelfRef.test_copper_lists__DOT__dut__DOT__tile_inst__DOT__dma_pal_phase = 0U;
    vlSelfRef.test_copper_lists__DOT__dut__DOT__tile_inst__DOT__dma_pal_r = 0U;
    vlSelfRef.test_copper_lists__DOT__dut__DOT__tile_inst__DOT__dma_pal_g = 0U;
    vlSelfRef.test_copper_lists__DOT__dut__DOT__tile_inst__DOT__dma_pal_sub = 0U;
    vlSelfRef.test_copper_lists__DOT__dut__DOT__tile_inst__DOT__dma_pal_cidx = 0U;
}

VL_ATTR_COLD void Vtest_copper_lists___024root___eval_final(Vtest_copper_lists___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vtest_copper_lists___024root___eval_final\n"); );
    Vtest_copper_lists__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
}

#ifdef VL_DEBUG
VL_ATTR_COLD void Vtest_copper_lists___024root___dump_triggers__stl(const VlUnpacked<QData/*63:0*/, 1> &triggers, const std::string &tag);
#endif  // VL_DEBUG
VL_ATTR_COLD bool Vtest_copper_lists___024root___eval_phase__stl(Vtest_copper_lists___024root* vlSelf);

VL_ATTR_COLD void Vtest_copper_lists___024root___eval_settle(Vtest_copper_lists___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vtest_copper_lists___024root___eval_settle\n"); );
    Vtest_copper_lists__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
    // Locals
    IData/*31:0*/ __VstlIterCount;
    // Body
    __VstlIterCount = 0U;
    vlSelfRef.__VstlFirstIteration = 1U;
    do {
        if (VL_UNLIKELY(((0x00000064U < __VstlIterCount)))) {
#ifdef VL_DEBUG
            Vtest_copper_lists___024root___dump_triggers__stl(vlSelfRef.__VstlTriggered, "stl"s);
#endif
            VL_FATAL_MT("test_copper_lists.sv", 6, "", "DIDNOTCONVERGE: Settle region did not converge after '--converge-limit' of 100 tries");
        }
        __VstlIterCount = ((IData)(1U) + __VstlIterCount);
        vlSelfRef.__VstlPhaseResult = Vtest_copper_lists___024root___eval_phase__stl(vlSelf);
        vlSelfRef.__VstlFirstIteration = 0U;
    } while (vlSelfRef.__VstlPhaseResult);
}

VL_ATTR_COLD void Vtest_copper_lists___024root___eval_triggers_vec__stl(Vtest_copper_lists___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vtest_copper_lists___024root___eval_triggers_vec__stl\n"); );
    Vtest_copper_lists__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
    // Body
    vlSelfRef.__VstlTriggered[0U] = ((0xfffffffffffffffeULL 
                                      & vlSelfRef.__VstlTriggered[0U]) 
                                     | (IData)((IData)(vlSelfRef.__VstlFirstIteration)));
}

VL_ATTR_COLD bool Vtest_copper_lists___024root___trigger_anySet__stl(const VlUnpacked<QData/*63:0*/, 1> &in);

#ifdef VL_DEBUG
VL_ATTR_COLD void Vtest_copper_lists___024root___dump_triggers__stl(const VlUnpacked<QData/*63:0*/, 1> &triggers, const std::string &tag) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vtest_copper_lists___024root___dump_triggers__stl\n"); );
    // Body
    if ((1U & (~ (IData)(Vtest_copper_lists___024root___trigger_anySet__stl(triggers))))) {
        VL_DBG_MSGS("         No '" + tag + "' region triggers active\n");
    }
    if ((1U & (IData)(triggers[0U]))) {
        VL_DBG_MSGS("         '" + tag + "' region trigger index 0 is active: Internal 'stl' trigger - first iteration\n");
    }
}
#endif  // VL_DEBUG

VL_ATTR_COLD bool Vtest_copper_lists___024root___trigger_anySet__stl(const VlUnpacked<QData/*63:0*/, 1> &in) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vtest_copper_lists___024root___trigger_anySet__stl\n"); );
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

VL_ATTR_COLD void Vtest_copper_lists___024root___stl_sequent__TOP__0(Vtest_copper_lists___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vtest_copper_lists___024root___stl_sequent__TOP__0\n"); );
    Vtest_copper_lists__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
    // Locals
    CData/*0:0*/ test_copper_lists__DOT__dut__DOT__visible;
    test_copper_lists__DOT__dut__DOT__visible = 0;
    CData/*4:0*/ test_copper_lists__DOT__dut__DOT__text_row;
    test_copper_lists__DOT__dut__DOT__text_row = 0;
    CData/*4:0*/ test_copper_lists__DOT__dut__DOT__real_row;
    test_copper_lists__DOT__dut__DOT__real_row = 0;
    SData/*9:0*/ test_copper_lists__DOT__dut__DOT__text_line;
    test_copper_lists__DOT__dut__DOT__text_line = 0;
    // Body
    vlSelfRef.test_copper_lists__DOT__dut__DOT__vgc_reg_sel 
        = ((0xa000U <= (IData)(vlSelfRef.test_copper_lists__DOT__cpu_addr)) 
           & (0xa01fU >= (IData)(vlSelfRef.test_copper_lists__DOT__cpu_addr)));
    vlSelfRef.test_copper_lists__DOT__dut__DOT__char_ram_sel 
        = ((0xaa00U <= (IData)(vlSelfRef.test_copper_lists__DOT__cpu_addr)) 
           & (0xb1cfU >= (IData)(vlSelfRef.test_copper_lists__DOT__cpu_addr)));
    vlSelfRef.test_copper_lists__DOT__dut__DOT__color_ram_sel 
        = ((0xb1d0U <= (IData)(vlSelfRef.test_copper_lists__DOT__cpu_addr)) 
           & (0xb99fU >= (IData)(vlSelfRef.test_copper_lists__DOT__cpu_addr)));
    vlSelfRef.test_copper_lists__DOT__dut__DOT__tile_reg_sel 
        = ((0xa0c0U <= (IData)(vlSelfRef.test_copper_lists__DOT__cpu_addr)) 
           & (0xa0dfU >= (IData)(vlSelfRef.test_copper_lists__DOT__cpu_addr)));
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

VL_ATTR_COLD void Vtest_copper_lists___024root___eval_stl(Vtest_copper_lists___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vtest_copper_lists___024root___eval_stl\n"); );
    Vtest_copper_lists__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
    // Locals
    CData/*0:0*/ __Vinline__stl_sequent__TOP__0_test_copper_lists__DOT__dut__DOT__visible;
    __Vinline__stl_sequent__TOP__0_test_copper_lists__DOT__dut__DOT__visible = 0;
    CData/*4:0*/ __Vinline__stl_sequent__TOP__0_test_copper_lists__DOT__dut__DOT__text_row;
    __Vinline__stl_sequent__TOP__0_test_copper_lists__DOT__dut__DOT__text_row = 0;
    CData/*4:0*/ __Vinline__stl_sequent__TOP__0_test_copper_lists__DOT__dut__DOT__real_row;
    __Vinline__stl_sequent__TOP__0_test_copper_lists__DOT__dut__DOT__real_row = 0;
    SData/*9:0*/ __Vinline__stl_sequent__TOP__0_test_copper_lists__DOT__dut__DOT__text_line;
    __Vinline__stl_sequent__TOP__0_test_copper_lists__DOT__dut__DOT__text_line = 0;
    // Body
    if ((1ULL & vlSelfRef.__VstlTriggered[0U])) {
        vlSelfRef.test_copper_lists__DOT__dut__DOT__vgc_reg_sel 
            = ((0xa000U <= (IData)(vlSelfRef.test_copper_lists__DOT__cpu_addr)) 
               & (0xa01fU >= (IData)(vlSelfRef.test_copper_lists__DOT__cpu_addr)));
        vlSelfRef.test_copper_lists__DOT__dut__DOT__char_ram_sel 
            = ((0xaa00U <= (IData)(vlSelfRef.test_copper_lists__DOT__cpu_addr)) 
               & (0xb1cfU >= (IData)(vlSelfRef.test_copper_lists__DOT__cpu_addr)));
        vlSelfRef.test_copper_lists__DOT__dut__DOT__color_ram_sel 
            = ((0xb1d0U <= (IData)(vlSelfRef.test_copper_lists__DOT__cpu_addr)) 
               & (0xb99fU >= (IData)(vlSelfRef.test_copper_lists__DOT__cpu_addr)));
        vlSelfRef.test_copper_lists__DOT__dut__DOT__tile_reg_sel 
            = ((0xa0c0U <= (IData)(vlSelfRef.test_copper_lists__DOT__cpu_addr)) 
               & (0xa0dfU >= (IData)(vlSelfRef.test_copper_lists__DOT__cpu_addr)));
        __Vinline__stl_sequent__TOP__0_test_copper_lists__DOT__dut__DOT__visible 
            = ((0x0280U > (IData)(vlSelfRef.test_copper_lists__DOT__dut__DOT__h_count)) 
               & (0x01e0U > (IData)(vlSelfRef.test_copper_lists__DOT__dut__DOT__v_count)));
        __Vinline__stl_sequent__TOP__0_test_copper_lists__DOT__dut__DOT__text_line 
            = (0x000003ffU & (((IData)(vlSelfRef.test_copper_lists__DOT__dut__DOT__v_count) 
                               - (IData)(0x00000028U)) 
                              >> 1U));
        __Vinline__stl_sequent__TOP__0_test_copper_lists__DOT__dut__DOT__text_row 
            = (0x0000001fU & (__Vinline__stl_sequent__TOP__0_test_copper_lists__DOT__dut__DOT__text_line 
                              >> 3U));
        __Vinline__stl_sequent__TOP__0_test_copper_lists__DOT__dut__DOT__real_row 
            = (0x0000001fU & ((IData)(__Vinline__stl_sequent__TOP__0_test_copper_lists__DOT__dut__DOT__text_row) 
                              + (IData)(vlSelfRef.test_copper_lists__DOT__dut__DOT__scroll_offset)));
        if ((0x19U <= __Vinline__stl_sequent__TOP__0_test_copper_lists__DOT__dut__DOT__real_row)) {
            __Vinline__stl_sequent__TOP__0_test_copper_lists__DOT__dut__DOT__real_row 
                = (0x0000001fU & ((IData)(__Vinline__stl_sequent__TOP__0_test_copper_lists__DOT__dut__DOT__real_row) 
                                  - (IData)(0x19U)));
        }
        vlSelfRef.test_copper_lists__DOT__dut__DOT__gfx_y 
            = (0x000000ffU & __Vinline__stl_sequent__TOP__0_test_copper_lists__DOT__dut__DOT__text_line);
        vlSelfRef.test_copper_lists__DOT__dut__DOT__in_text_area 
            = (__Vinline__stl_sequent__TOP__0_test_copper_lists__DOT__dut__DOT__visible 
               & ((0x0028U <= (IData)(vlSelfRef.test_copper_lists__DOT__dut__DOT__v_count)) 
                  & (0x01b8U > (IData)(vlSelfRef.test_copper_lists__DOT__dut__DOT__v_count))));
    }
}

VL_ATTR_COLD bool Vtest_copper_lists___024root___eval_phase__stl(Vtest_copper_lists___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vtest_copper_lists___024root___eval_phase__stl\n"); );
    Vtest_copper_lists__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
    // Locals
    CData/*0:0*/ __VstlExecute;
    // Body
    Vtest_copper_lists___024root___eval_triggers_vec__stl(vlSelf);
#ifdef VL_DEBUG
    if (VL_UNLIKELY(vlSymsp->_vm_contextp__->debug())) {
        Vtest_copper_lists___024root___dump_triggers__stl(vlSelfRef.__VstlTriggered, "stl"s);
    }
#endif
    __VstlExecute = Vtest_copper_lists___024root___trigger_anySet__stl(vlSelfRef.__VstlTriggered);
    if (__VstlExecute) {
        Vtest_copper_lists___024root___eval_stl(vlSelf);
    }
    return (__VstlExecute);
}

bool Vtest_copper_lists___024root___trigger_anySet__act(const VlUnpacked<QData/*63:0*/, 1> &in);

#ifdef VL_DEBUG
VL_ATTR_COLD void Vtest_copper_lists___024root___dump_triggers__act(const VlUnpacked<QData/*63:0*/, 1> &triggers, const std::string &tag) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vtest_copper_lists___024root___dump_triggers__act\n"); );
    // Body
    if ((1U & (~ (IData)(Vtest_copper_lists___024root___trigger_anySet__act(triggers))))) {
        VL_DBG_MSGS("         No '" + tag + "' region triggers active\n");
    }
    if ((1U & (IData)(triggers[0U]))) {
        VL_DBG_MSGS("         '" + tag + "' region trigger index 0 is active: @(posedge test_copper_lists.clk)\n");
    }
    if ((1U & (IData)((triggers[0U] >> 1U)))) {
        VL_DBG_MSGS("         '" + tag + "' region trigger index 1 is active: @([true] __VdlySched.awaitingCurrentTime())\n");
    }
}
#endif  // VL_DEBUG

VL_ATTR_COLD void Vtest_copper_lists___024root___ctor_var_reset(Vtest_copper_lists___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vtest_copper_lists___024root___ctor_var_reset\n"); );
    Vtest_copper_lists__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
    // Body
    const uint64_t __VscopeHash = VL_MURMUR64_HASH(vlSelf->vlNamep);
    vlSelf->test_copper_lists__DOT__rst = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 9021352433391461025ull);
    vlSelf->test_copper_lists__DOT__cpu_addr = VL_SCOPED_RAND_RESET_I(16, __VscopeHash, 10027213373743130266ull);
    vlSelf->test_copper_lists__DOT__cpu_wdata = VL_SCOPED_RAND_RESET_I(8, __VscopeHash, 9470903857220663107ull);
    vlSelf->test_copper_lists__DOT__cpu_we = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 7002752627732824299ull);
    vlSelf->test_copper_lists__DOT__dut__DOT____Vlvbound_hced7a248__0 = 0;
    vlSelf->test_copper_lists__DOT__dut__DOT____Vlvbound_h87c37a99__0 = 0;
    vlSelf->test_copper_lists__DOT__dut__DOT____Vlvbound_h45978041__0 = 0;
    vlSelf->test_copper_lists__DOT__dut__DOT____Vlvbound_hd4129f91__0 = 0;
    vlSelf->test_copper_lists__DOT__dut__DOT____Vlvbound_ha13cfb6b__0 = 0;
    vlSelf->test_copper_lists__DOT__dut__DOT____Vlvbound_hde0d773d__1 = 0;
    vlSelf->test_copper_lists__DOT__dut__DOT____Vlvbound_hde0d773d__0 = 0;
    vlSelf->test_copper_lists__DOT__dut__DOT____Vlvbound_h5d6d955d__0 = 0;
    vlSelf->test_copper_lists__DOT__dut__DOT____Vlvbound_h2b480197__0 = 0;
    vlSelf->test_copper_lists__DOT__dut__DOT____Vlvbound_hb8130fd5__0 = 0;
    vlSelf->test_copper_lists__DOT__dut__DOT____Vlvbound_he1a62a70__0 = 0;
    vlSelf->test_copper_lists__DOT__dut__DOT____Vlvbound_h3f27e46b__3 = 0;
    vlSelf->test_copper_lists__DOT__dut__DOT____Vlvbound_h9f4406d7__0 = 0;
    vlSelf->test_copper_lists__DOT__dut__DOT____Vlvbound_haa426699__0 = 0;
    vlSelf->test_copper_lists__DOT__dut__DOT____Vlvbound_hd5fa5e14__0 = 0;
    vlSelf->test_copper_lists__DOT__dut__DOT____Vlvbound_hd5133de6__0 = 0;
    vlSelf->test_copper_lists__DOT__dut__DOT____Vlvbound_hed509973__0 = 0;
    vlSelf->test_copper_lists__DOT__dut__DOT____Vlvbound_hc84a7865__0 = 0;
    vlSelf->test_copper_lists__DOT__dut__DOT____Vlvbound_haf1931bd__0 = 0;
    vlSelf->test_copper_lists__DOT__dut__DOT____Vlvbound_hafb21497__0 = 0;
    vlSelf->test_copper_lists__DOT__dut__DOT____Vlvbound_h1cc62cc7__0 = 0;
    vlSelf->test_copper_lists__DOT__dut__DOT____Vlvbound_h6e0ff81d__0 = 0;
    vlSelf->test_copper_lists__DOT__dut__DOT____Vlvbound_h3f27e46b__2 = 0;
    vlSelf->test_copper_lists__DOT__dut__DOT____Vlvbound_h3f27e46b__1 = 0;
    vlSelf->test_copper_lists__DOT__dut__DOT____Vlvbound_h3f27e46b__0 = 0;
    vlSelf->test_copper_lists__DOT__dut__DOT____Vlvbound_hc0a966b6__0 = 0;
    vlSelf->test_copper_lists__DOT__dut__DOT____Vlvbound_hdecc4094__0 = 0;
    vlSelf->test_copper_lists__DOT__dut__DOT____Vlvbound_h902d5c6a__0 = 0;
    vlSelf->test_copper_lists__DOT__dut__DOT____Vlvbound_h9d27345d__0 = 0;
    vlSelf->test_copper_lists__DOT__dut__DOT____Vlvbound_hef6c2195__0 = 0;
    vlSelf->test_copper_lists__DOT__dut__DOT____Vlvbound_h9d4d7d6f__0 = 0;
    vlSelf->test_copper_lists__DOT__dut__DOT____Vlvbound_h652b6de2__0 = 0;
    vlSelf->test_copper_lists__DOT__dut__DOT____Vlvbound_h45ddebd4__0 = 0;
    vlSelf->test_copper_lists__DOT__dut__DOT____Vlvbound_h3318332a__0 = 0;
    vlSelf->test_copper_lists__DOT__dut__DOT__h_count = VL_SCOPED_RAND_RESET_I(10, __VscopeHash, 17613107803278668413ull);
    vlSelf->test_copper_lists__DOT__dut__DOT__v_count = VL_SCOPED_RAND_RESET_I(10, __VscopeHash, 6649110053538660772ull);
    vlSelf->test_copper_lists__DOT__dut__DOT__in_text_area = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 15456866328963796598ull);
    for (int __Vi0 = 0; __Vi0 < 2000; ++__Vi0) {
        vlSelf->test_copper_lists__DOT__dut__DOT__char_ram[__Vi0] = VL_SCOPED_RAND_RESET_I(8, __VscopeHash, 16146832825279898458ull);
    }
    for (int __Vi0 = 0; __Vi0 < 2000; ++__Vi0) {
        vlSelf->test_copper_lists__DOT__dut__DOT__color_ram[__Vi0] = VL_SCOPED_RAND_RESET_I(8, __VscopeHash, 17045187869033303885ull);
    }
    for (int __Vi0 = 0; __Vi0 < 2048; ++__Vi0) {
        vlSelf->test_copper_lists__DOT__dut__DOT__font_rom[__Vi0] = VL_SCOPED_RAND_RESET_I(8, __VscopeHash, 10392146267940736728ull);
    }
    for (int __Vi0 = 0; __Vi0 < 64000; ++__Vi0) {
        vlSelf->test_copper_lists__DOT__dut__DOT__gfx_ram[__Vi0] = VL_SCOPED_RAND_RESET_I(4, __VscopeHash, 11712156560327216462ull);
    }
    for (int __Vi0 = 0; __Vi0 < 2048; ++__Vi0) {
        vlSelf->test_copper_lists__DOT__dut__DOT__sprite_shapes[__Vi0] = VL_SCOPED_RAND_RESET_I(8, __VscopeHash, 8544527717359247989ull);
    }
    for (int __Vi0 = 0; __Vi0 < 32; ++__Vi0) {
        vlSelf->test_copper_lists__DOT__dut__DOT__regs[__Vi0] = VL_SCOPED_RAND_RESET_I(8, __VscopeHash, 18200896866823385287ull);
    }
    vlSelf->test_copper_lists__DOT__dut__DOT__cursor_x = VL_SCOPED_RAND_RESET_I(7, __VscopeHash, 14556122521213604212ull);
    vlSelf->test_copper_lists__DOT__dut__DOT__cursor_y = VL_SCOPED_RAND_RESET_I(5, __VscopeHash, 3418037607646263190ull);
    vlSelf->test_copper_lists__DOT__dut__DOT__fg_color = VL_SCOPED_RAND_RESET_I(4, __VscopeHash, 7571378953889139690ull);
    vlSelf->test_copper_lists__DOT__dut__DOT__bg_color = VL_SCOPED_RAND_RESET_I(4, __VscopeHash, 9320540907480492999ull);
    vlSelf->test_copper_lists__DOT__dut__DOT__gfx_color = VL_SCOPED_RAND_RESET_I(4, __VscopeHash, 4725143287403566314ull);
    vlSelf->test_copper_lists__DOT__dut__DOT__scroll_offset = VL_SCOPED_RAND_RESET_I(5, __VscopeHash, 319957956004929864ull);
    vlSelf->test_copper_lists__DOT__dut__DOT__scroll_pending = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 18313411300519946010ull);
    vlSelf->test_copper_lists__DOT__dut__DOT__scroll_col = VL_SCOPED_RAND_RESET_I(7, __VscopeHash, 14806178709776156793ull);
    for (int __Vi0 = 0; __Vi0 < 512; ++__Vi0) {
        vlSelf->test_copper_lists__DOT__dut__DOT__fill_stack[__Vi0] = VL_SCOPED_RAND_RESET_I(17, __VscopeHash, 9410899147011578174ull);
    }
    vlSelf->test_copper_lists__DOT__dut__DOT__fill_sp = VL_SCOPED_RAND_RESET_I(10, __VscopeHash, 9272913715735991020ull);
    vlSelf->test_copper_lists__DOT__dut__DOT__fill_target = VL_SCOPED_RAND_RESET_I(4, __VscopeHash, 2363136211538443472ull);
    vlSelf->test_copper_lists__DOT__dut__DOT__gt_char_idx = VL_SCOPED_RAND_RESET_I(6, __VscopeHash, 12090694851822188406ull);
    vlSelf->test_copper_lists__DOT__dut__DOT__gt_char_len = VL_SCOPED_RAND_RESET_I(6, __VscopeHash, 7794065596433639255ull);
    vlSelf->test_copper_lists__DOT__dut__DOT__gt_font_row = VL_SCOPED_RAND_RESET_I(3, __VscopeHash, 16082464777804238625ull);
    vlSelf->test_copper_lists__DOT__dut__DOT__gt_font_col = VL_SCOPED_RAND_RESET_I(3, __VscopeHash, 8609064785857683254ull);
    vlSelf->test_copper_lists__DOT__dut__DOT__gt_scale = VL_SCOPED_RAND_RESET_I(8, __VscopeHash, 10825233182961425461ull);
    vlSelf->test_copper_lists__DOT__dut__DOT__gt_scale_x = VL_SCOPED_RAND_RESET_I(8, __VscopeHash, 15051305327170924508ull);
    vlSelf->test_copper_lists__DOT__dut__DOT__gt_scale_y = VL_SCOPED_RAND_RESET_I(8, __VscopeHash, 12255925405941883883ull);
    vlSelf->test_copper_lists__DOT__dut__DOT__gt_pen_x = VL_SCOPED_RAND_RESET_I(9, __VscopeHash, 15188531162522865882ull);
    vlSelf->test_copper_lists__DOT__dut__DOT__gt_start_y = VL_SCOPED_RAND_RESET_I(8, __VscopeHash, 10458371990378136664ull);
    vlSelf->test_copper_lists__DOT__dut__DOT__gt_cur_char = VL_SCOPED_RAND_RESET_I(8, __VscopeHash, 1658530050231504053ull);
    vlSelf->test_copper_lists__DOT__dut__DOT__gt_font_byte = VL_SCOPED_RAND_RESET_I(8, __VscopeHash, 3509959472411232195ull);
    for (int __Vi0 = 0; __Vi0 < 32; ++__Vi0) {
        vlSelf->test_copper_lists__DOT__dut__DOT__copper_pos[__Vi0] = VL_SCOPED_RAND_RESET_I(17, __VscopeHash, 8447665381558246879ull);
    }
    for (int __Vi0 = 0; __Vi0 < 32; ++__Vi0) {
        vlSelf->test_copper_lists__DOT__dut__DOT__copper_reg[__Vi0] = VL_SCOPED_RAND_RESET_I(8, __VscopeHash, 3232239719693365949ull);
    }
    for (int __Vi0 = 0; __Vi0 < 32; ++__Vi0) {
        vlSelf->test_copper_lists__DOT__dut__DOT__copper_val[__Vi0] = VL_SCOPED_RAND_RESET_I(8, __VscopeHash, 17791323733776174260ull);
    }
    vlSelf->test_copper_lists__DOT__dut__DOT__copper_count = VL_SCOPED_RAND_RESET_I(9, __VscopeHash, 11496853900511138475ull);
    vlSelf->test_copper_lists__DOT__dut__DOT__copper_index = VL_SCOPED_RAND_RESET_I(9, __VscopeHash, 1549896697450403174ull);
    vlSelf->test_copper_lists__DOT__dut__DOT__copper_enabled = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 13316886222222447685ull);
    for (int __Vi0 = 0; __Vi0 < 4096; ++__Vi0) {
        vlSelf->test_copper_lists__DOT__dut__DOT__copper_list_pos[__Vi0] = VL_SCOPED_RAND_RESET_I(17, __VscopeHash, 10907831711492584996ull);
    }
    for (int __Vi0 = 0; __Vi0 < 4096; ++__Vi0) {
        vlSelf->test_copper_lists__DOT__dut__DOT__copper_list_reg[__Vi0] = VL_SCOPED_RAND_RESET_I(8, __VscopeHash, 1980941272445755466ull);
    }
    for (int __Vi0 = 0; __Vi0 < 4096; ++__Vi0) {
        vlSelf->test_copper_lists__DOT__dut__DOT__copper_list_val[__Vi0] = VL_SCOPED_RAND_RESET_I(8, __VscopeHash, 4558622867022286296ull);
    }
    for (int __Vi0 = 0; __Vi0 < 128; ++__Vi0) {
        vlSelf->test_copper_lists__DOT__dut__DOT__copper_list_count[__Vi0] = VL_SCOPED_RAND_RESET_I(9, __VscopeHash, 10606612294279866580ull);
    }
    vlSelf->test_copper_lists__DOT__dut__DOT__copper_target_list = VL_SCOPED_RAND_RESET_I(7, __VscopeHash, 2920208246454717410ull);
    vlSelf->test_copper_lists__DOT__dut__DOT__copper_active_list = VL_SCOPED_RAND_RESET_I(7, __VscopeHash, 6203660329722501088ull);
    vlSelf->test_copper_lists__DOT__dut__DOT__copper_pending_list = VL_SCOPED_RAND_RESET_I(7, __VscopeHash, 11829989950817393779ull);
    vlSelf->test_copper_lists__DOT__dut__DOT__copper_loading = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 4182929690320744417ull);
    vlSelf->test_copper_lists__DOT__dut__DOT__copper_load_idx = VL_SCOPED_RAND_RESET_I(9, __VscopeHash, 9177913618211916114ull);
    vlSelf->test_copper_lists__DOT__dut__DOT__copper_load_src = VL_SCOPED_RAND_RESET_I(7, __VscopeHash, 13613892313018056566ull);
    for (int __Vi0 = 0; __Vi0 < 64; ++__Vi0) {
        vlSelf->test_copper_lists__DOT__dut__DOT__fio_name[__Vi0] = VL_SCOPED_RAND_RESET_I(8, __VscopeHash, 7067220094671352585ull);
    }
    vlSelf->test_copper_lists__DOT__dut__DOT__fio_name_len = VL_SCOPED_RAND_RESET_I(6, __VscopeHash, 7173342617836038955ull);
    vlSelf->test_copper_lists__DOT__dut__DOT__cmd_busy = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 7234077467589206372ull);
    vlSelf->test_copper_lists__DOT__dut__DOT__cmd_op = VL_SCOPED_RAND_RESET_I(8, __VscopeHash, 9858108179392024091ull);
    vlSelf->test_copper_lists__DOT__dut__DOT__cmd_x = VL_SCOPED_RAND_RESET_I(10, __VscopeHash, 12373570214509780644ull);
    vlSelf->test_copper_lists__DOT__dut__DOT__cmd_y = VL_SCOPED_RAND_RESET_I(10, __VscopeHash, 7925111538164473711ull);
    vlSelf->test_copper_lists__DOT__dut__DOT__cmd_x2 = VL_SCOPED_RAND_RESET_I(10, __VscopeHash, 9650966264537114236ull);
    vlSelf->test_copper_lists__DOT__dut__DOT__cmd_y2 = VL_SCOPED_RAND_RESET_I(10, __VscopeHash, 16895701268054661868ull);
    vlSelf->test_copper_lists__DOT__dut__DOT__cmd_dx = VL_SCOPED_RAND_RESET_I(10, __VscopeHash, 3852794344330343604ull);
    vlSelf->test_copper_lists__DOT__dut__DOT__cmd_dy = VL_SCOPED_RAND_RESET_I(10, __VscopeHash, 4233701190349757729ull);
    vlSelf->test_copper_lists__DOT__dut__DOT__cmd_err = VL_SCOPED_RAND_RESET_I(10, __VscopeHash, 18254560993503825029ull);
    vlSelf->test_copper_lists__DOT__dut__DOT__cmd_sx = VL_SCOPED_RAND_RESET_I(10, __VscopeHash, 10855568688582183745ull);
    vlSelf->test_copper_lists__DOT__dut__DOT__cmd_sy = VL_SCOPED_RAND_RESET_I(10, __VscopeHash, 7758499419742852099ull);
    vlSelf->test_copper_lists__DOT__dut__DOT__cmd_cx = VL_SCOPED_RAND_RESET_I(9, __VscopeHash, 17888126759520988771ull);
    vlSelf->test_copper_lists__DOT__dut__DOT__cmd_cy = VL_SCOPED_RAND_RESET_I(8, __VscopeHash, 15903793663680331963ull);
    vlSelf->test_copper_lists__DOT__dut__DOT__vgc_reg_sel = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 16739107894295000746ull);
    vlSelf->test_copper_lists__DOT__dut__DOT__char_ram_sel = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 17189257726011810506ull);
    vlSelf->test_copper_lists__DOT__dut__DOT__color_ram_sel = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 16504740169810324010ull);
    vlSelf->test_copper_lists__DOT__dut__DOT__tile_reg_sel = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 5678485101329006104ull);
    vlSelf->test_copper_lists__DOT__dut__DOT__gfx_y = VL_SCOPED_RAND_RESET_I(8, __VscopeHash, 8895164588583884876ull);
    vlSelf->test_copper_lists__DOT__dut__DOT__unnamedblk14__DOT__i = 0;
    vlSelf->test_copper_lists__DOT__dut__DOT__unnamedblk15__DOT__i = 0;
    vlSelf->test_copper_lists__DOT__dut__DOT__unnamedblk16__DOT__i = 0;
    vlSelf->test_copper_lists__DOT__dut__DOT__unnamedblk17__DOT__i = 0;
    vlSelf->test_copper_lists__DOT__dut__DOT__tile_inst__DOT____Vlvbound_h021f5fae__0 = 0;
    vlSelf->test_copper_lists__DOT__dut__DOT__tile_inst__DOT____Vlvbound_h02fc01d7__0 = 0;
    vlSelf->test_copper_lists__DOT__dut__DOT__tile_inst__DOT____Vlvbound_h67f0dfac__0 = 0;
    vlSelf->test_copper_lists__DOT__dut__DOT__tile_inst__DOT____Vlvbound_ha057723b__0 = 0;
    vlSelf->test_copper_lists__DOT__dut__DOT__tile_inst__DOT____Vlvbound_h8898988b__0 = 0;
    vlSelf->test_copper_lists__DOT__dut__DOT__tile_inst__DOT____Vlvbound_ha3d87205__0 = 0;
    vlSelf->test_copper_lists__DOT__dut__DOT__tile_inst__DOT____Vlvbound_h353a15af__0 = 0;
    vlSelf->test_copper_lists__DOT__dut__DOT__tile_inst__DOT____Vlvbound_hf7076230__0 = 0;
    vlSelf->test_copper_lists__DOT__dut__DOT__tile_inst__DOT____Vlvbound_hf26d05a2__0 = 0;
    vlSelf->test_copper_lists__DOT__dut__DOT__tile_inst__DOT____Vlvbound_h8c784627__0 = 0;
    vlSelf->test_copper_lists__DOT__dut__DOT__tile_inst__DOT____Vlvbound_h02aa0280__0 = 0;
    vlSelf->test_copper_lists__DOT__dut__DOT__tile_inst__DOT____Vlvbound_h21da2949__0 = 0;
    vlSelf->test_copper_lists__DOT__dut__DOT__tile_inst__DOT____Vlvbound_h17e6e432__0 = 0;
    vlSelf->test_copper_lists__DOT__dut__DOT__tile_inst__DOT____Vlvbound_h3335cc4a__0 = 0;
    vlSelf->test_copper_lists__DOT__dut__DOT__tile_inst__DOT____Vlvbound_ha65a9a44__0 = 0;
    vlSelf->test_copper_lists__DOT__dut__DOT__tile_inst__DOT____Vlvbound_h7ed4573b__0 = 0;
    vlSelf->test_copper_lists__DOT__dut__DOT__tile_inst__DOT____Vlvbound_h9afe376d__0 = 0;
    for (int __Vi0 = 0; __Vi0 < 32; ++__Vi0) {
        vlSelf->test_copper_lists__DOT__dut__DOT__tile_inst__DOT__tregs[__Vi0] = VL_SCOPED_RAND_RESET_I(8, __VscopeHash, 17137453179991666932ull);
    }
    vlSelf->test_copper_lists__DOT__dut__DOT__tile_inst__DOT__tile_size16 = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 9068087022397134966ull);
    for (int __Vi0 = 0; __Vi0 < 25; ++__Vi0) {
        vlSelf->test_copper_lists__DOT__dut__DOT__tile_inst__DOT__col_buffer[__Vi0] = VL_SCOPED_RAND_RESET_I(8, __VscopeHash, 1583346270289936095ull);
    }
    for (int __Vi0 = 0; __Vi0 < 32768; ++__Vi0) {
        vlSelf->test_copper_lists__DOT__dut__DOT__tile_inst__DOT__tile_data[__Vi0] = VL_SCOPED_RAND_RESET_I(8, __VscopeHash, 1262874228688810997ull);
    }
    for (int __Vi0 = 0; __Vi0 < 4000; ++__Vi0) {
        vlSelf->test_copper_lists__DOT__dut__DOT__tile_inst__DOT__nametable[__Vi0] = VL_SCOPED_RAND_RESET_I(8, __VscopeHash, 904636554148674702ull);
    }
    for (int __Vi0 = 0; __Vi0 < 4000; ++__Vi0) {
        vlSelf->test_copper_lists__DOT__dut__DOT__tile_inst__DOT__attr_table[__Vi0] = VL_SCOPED_RAND_RESET_I(8, __VscopeHash, 9395886469033223434ull);
    }
    vlSelf->test_copper_lists__DOT__dut__DOT__tile_inst__DOT__dma_state = VL_SCOPED_RAND_RESET_I(4, __VscopeHash, 2850667437914060950ull);
    vlSelf->test_copper_lists__DOT__dut__DOT__tile_inst__DOT__dma_src = VL_SCOPED_RAND_RESET_I(16, __VscopeHash, 14799872691607276217ull);
    vlSelf->test_copper_lists__DOT__dut__DOT__tile_inst__DOT__dma_count = VL_SCOPED_RAND_RESET_I(16, __VscopeHash, 7272193765743926054ull);
    vlSelf->test_copper_lists__DOT__dut__DOT__tile_inst__DOT__dma_dst = VL_SCOPED_RAND_RESET_I(16, __VscopeHash, 13147220882605749595ull);
    vlSelf->test_copper_lists__DOT__dut__DOT__tile_inst__DOT__dma_nt = VL_SCOPED_RAND_RESET_I(2, __VscopeHash, 4677253422201756438ull);
    vlSelf->test_copper_lists__DOT__dut__DOT__tile_inst__DOT__dma_col_row = VL_SCOPED_RAND_RESET_I(6, __VscopeHash, 4106657411450220711ull);
    vlSelf->test_copper_lists__DOT__dut__DOT__tile_inst__DOT__dma_col_idx = VL_SCOPED_RAND_RESET_I(5, __VscopeHash, 17699941224593543784ull);
    vlSelf->test_copper_lists__DOT__dut__DOT__tile_inst__DOT__dma_data_valid = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 1369455490628165153ull);
    vlSelf->test_copper_lists__DOT__dut__DOT__tile_inst__DOT__dma_pal_phase = VL_SCOPED_RAND_RESET_I(8, __VscopeHash, 16498228444806237452ull);
    vlSelf->test_copper_lists__DOT__dut__DOT__tile_inst__DOT__dma_pal_r = VL_SCOPED_RAND_RESET_I(8, __VscopeHash, 5574756288776557395ull);
    vlSelf->test_copper_lists__DOT__dut__DOT__tile_inst__DOT__dma_pal_g = VL_SCOPED_RAND_RESET_I(8, __VscopeHash, 8923170040265673368ull);
    vlSelf->test_copper_lists__DOT__dut__DOT__tile_inst__DOT__dma_pal_sub = VL_SCOPED_RAND_RESET_I(4, __VscopeHash, 9185784672205948317ull);
    vlSelf->test_copper_lists__DOT__dut__DOT__tile_inst__DOT__dma_pal_cidx = VL_SCOPED_RAND_RESET_I(4, __VscopeHash, 13269720404138754487ull);
    vlSelf->test_copper_lists__DOT__dut__DOT__tile_inst__DOT__unnamedblk6__DOT__i = 0;
    for (int __Vi0 = 0; __Vi0 < 1; ++__Vi0) {
        vlSelf->__VstlTriggered[__Vi0] = 0;
    }
    for (int __Vi0 = 0; __Vi0 < 1; ++__Vi0) {
        vlSelf->__VactTriggered[__Vi0] = 0;
    }
    for (int __Vi0 = 0; __Vi0 < 1; ++__Vi0) {
        vlSelf->__VactTriggeredAcc[__Vi0] = 0;
    }
    vlSelf->__Vtrigprevexpr___TOP__test_copper_lists__DOT__clk__0 = 0;
    for (int __Vi0 = 0; __Vi0 < 1; ++__Vi0) {
        vlSelf->__VnbaTriggered[__Vi0] = 0;
    }
    vlSelf->__Vi = 0;
}
