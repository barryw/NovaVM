// Verilated -*- C++ -*-
// DESCRIPTION: Verilator output: Design implementation internals
// See Vtest_copper.h for the primary calling header

#include "Vtest_copper__pch.h"

void Vtest_copper___024root___timing_ready(Vtest_copper___024root* vlSelf);

VL_ATTR_COLD void Vtest_copper___024root___eval_static(Vtest_copper___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vtest_copper___024root___eval_static\n"); );
    Vtest_copper__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
    // Body
    vlSelfRef.test_copper__DOT__clk = 0U;
    vlSelfRef.test_copper__DOT__pass_count = 0U;
    vlSelfRef.test_copper__DOT__fail_count = 0U;
    vlSelfRef.test_copper__DOT__test_num = 0U;
    vlSelfRef.__Vtrigprevexpr___TOP__test_copper__DOT__clk__0 = 0U;
    Vtest_copper___024root___timing_ready(vlSelf);
    do {
        vlSelfRef.__VactTriggeredAcc[vlSelfRef.__Vi] 
            = vlSelfRef.__VactTriggered[vlSelfRef.__Vi];
        vlSelfRef.__Vi = ((IData)(1U) + vlSelfRef.__Vi);
    } while ((0U >= vlSelfRef.__Vi));
}

VL_ATTR_COLD void Vtest_copper___024root___eval_static__TOP(Vtest_copper___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vtest_copper___024root___eval_static__TOP\n"); );
    Vtest_copper__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
    // Body
    vlSelfRef.test_copper__DOT__clk = 0U;
    vlSelfRef.test_copper__DOT__pass_count = 0U;
    vlSelfRef.test_copper__DOT__fail_count = 0U;
    vlSelfRef.test_copper__DOT__test_num = 0U;
}

VL_ATTR_COLD void Vtest_copper___024root___eval_initial__TOP(Vtest_copper___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vtest_copper___024root___eval_initial__TOP\n"); );
    Vtest_copper__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
    // Locals
    VlUnpacked<CData/*3:0*/, 16> test_copper__DOT__dut__DOT__spr_trans;
    for (int __Vi0 = 0; __Vi0 < 16; ++__Vi0) {
        test_copper__DOT__dut__DOT__spr_trans[__Vi0] = 0;
    }
    IData/*31:0*/ test_copper__DOT__dut__DOT__unnamedblk6__DOT__i;
    test_copper__DOT__dut__DOT__unnamedblk6__DOT__i = 0;
    IData/*31:0*/ test_copper__DOT__dut__DOT__unnamedblk7__DOT__i;
    test_copper__DOT__dut__DOT__unnamedblk7__DOT__i = 0;
    IData/*31:0*/ test_copper__DOT__dut__DOT__text_inst__DOT__char_mem__DOT__gen_init_zero__DOT__i;
    test_copper__DOT__dut__DOT__text_inst__DOT__char_mem__DOT__gen_init_zero__DOT__i = 0;
    IData/*31:0*/ test_copper__DOT__dut__DOT__text_inst__DOT__color_mem__DOT__gen_init_zero__DOT__i;
    test_copper__DOT__dut__DOT__text_inst__DOT__color_mem__DOT__gen_init_zero__DOT__i = 0;
    IData/*31:0*/ test_copper__DOT__dut__DOT__gfx_inst__DOT__gfx_mem__DOT__gen_init_zero__DOT__i;
    test_copper__DOT__dut__DOT__gfx_inst__DOT__gfx_mem__DOT__gen_init_zero__DOT__i = 0;
    IData/*31:0*/ test_copper__DOT__dut__DOT__artist_inst__DOT__fill_stack_mem__DOT__gen_init_zero__DOT__i;
    test_copper__DOT__dut__DOT__artist_inst__DOT__fill_stack_mem__DOT__gen_init_zero__DOT__i = 0;
    IData/*31:0*/ test_copper__DOT__dut__DOT__sprite_inst__DOT__spr_mem__DOT__gen_init_zero__DOT__i;
    test_copper__DOT__dut__DOT__sprite_inst__DOT__spr_mem__DOT__gen_init_zero__DOT__i = 0;
    IData/*31:0*/ test_copper__DOT__dut__DOT__tile_inst__DOT__tile_data_ram__DOT__gen_init_zero__DOT__i;
    test_copper__DOT__dut__DOT__tile_inst__DOT__tile_data_ram__DOT__gen_init_zero__DOT__i = 0;
    IData/*31:0*/ test_copper__DOT__dut__DOT__tile_inst__DOT__nametable_ram__DOT__gen_init_zero__DOT__i;
    test_copper__DOT__dut__DOT__tile_inst__DOT__nametable_ram__DOT__gen_init_zero__DOT__i = 0;
    IData/*31:0*/ test_copper__DOT__dut__DOT__tile_inst__DOT__attr_table_ram__DOT__gen_init_zero__DOT__i;
    test_copper__DOT__dut__DOT__tile_inst__DOT__attr_table_ram__DOT__gen_init_zero__DOT__i = 0;
    VlUnpacked<SData/*11:0*/, 256> test_copper__DOT__dut__DOT__tile_inst__DOT__pal_ram_inst__DOT__mem;
    for (int __Vi0 = 0; __Vi0 < 256; ++__Vi0) {
        test_copper__DOT__dut__DOT__tile_inst__DOT__pal_ram_inst__DOT__mem[__Vi0] = 0;
    }
    IData/*31:0*/ __Vilp1;
    // Body
    vlSelfRef.test_copper__DOT__dut__DOT__copper_pos[0U] = 0U;
    vlSelfRef.test_copper__DOT__dut__DOT__copper_reg[0U] = 0U;
    vlSelfRef.test_copper__DOT__dut__DOT__copper_val[0U] = 0U;
    vlSelfRef.test_copper__DOT__dut__DOT__copper_pos[1U] = 0U;
    vlSelfRef.test_copper__DOT__dut__DOT__copper_reg[1U] = 0U;
    vlSelfRef.test_copper__DOT__dut__DOT__copper_val[1U] = 0U;
    vlSelfRef.test_copper__DOT__dut__DOT__copper_pos[2U] = 0U;
    vlSelfRef.test_copper__DOT__dut__DOT__copper_reg[2U] = 0U;
    vlSelfRef.test_copper__DOT__dut__DOT__copper_val[2U] = 0U;
    vlSelfRef.test_copper__DOT__dut__DOT__copper_pos[3U] = 0U;
    vlSelfRef.test_copper__DOT__dut__DOT__copper_reg[3U] = 0U;
    vlSelfRef.test_copper__DOT__dut__DOT__copper_val[3U] = 0U;
    vlSelfRef.test_copper__DOT__dut__DOT__copper_pos[4U] = 0U;
    vlSelfRef.test_copper__DOT__dut__DOT__copper_reg[4U] = 0U;
    vlSelfRef.test_copper__DOT__dut__DOT__copper_val[4U] = 0U;
    vlSelfRef.test_copper__DOT__dut__DOT__copper_pos[5U] = 0U;
    vlSelfRef.test_copper__DOT__dut__DOT__copper_reg[5U] = 0U;
    vlSelfRef.test_copper__DOT__dut__DOT__copper_val[5U] = 0U;
    vlSelfRef.test_copper__DOT__dut__DOT__copper_pos[6U] = 0U;
    vlSelfRef.test_copper__DOT__dut__DOT__copper_reg[6U] = 0U;
    vlSelfRef.test_copper__DOT__dut__DOT__copper_val[6U] = 0U;
    vlSelfRef.test_copper__DOT__dut__DOT__copper_pos[7U] = 0U;
    vlSelfRef.test_copper__DOT__dut__DOT__copper_reg[7U] = 0U;
    vlSelfRef.test_copper__DOT__dut__DOT__copper_val[7U] = 0U;
    vlSelfRef.test_copper__DOT__dut__DOT__copper_pos[8U] = 0U;
    vlSelfRef.test_copper__DOT__dut__DOT__copper_reg[8U] = 0U;
    vlSelfRef.test_copper__DOT__dut__DOT__copper_val[8U] = 0U;
    vlSelfRef.test_copper__DOT__dut__DOT__copper_pos[9U] = 0U;
    vlSelfRef.test_copper__DOT__dut__DOT__copper_reg[9U] = 0U;
    vlSelfRef.test_copper__DOT__dut__DOT__copper_val[9U] = 0U;
    vlSelfRef.test_copper__DOT__dut__DOT__copper_pos[10U] = 0U;
    vlSelfRef.test_copper__DOT__dut__DOT__copper_reg[10U] = 0U;
    vlSelfRef.test_copper__DOT__dut__DOT__copper_val[10U] = 0U;
    vlSelfRef.test_copper__DOT__dut__DOT__copper_pos[11U] = 0U;
    vlSelfRef.test_copper__DOT__dut__DOT__copper_reg[11U] = 0U;
    vlSelfRef.test_copper__DOT__dut__DOT__copper_val[11U] = 0U;
    vlSelfRef.test_copper__DOT__dut__DOT__copper_pos[12U] = 0U;
    vlSelfRef.test_copper__DOT__dut__DOT__copper_reg[12U] = 0U;
    vlSelfRef.test_copper__DOT__dut__DOT__copper_val[12U] = 0U;
    vlSelfRef.test_copper__DOT__dut__DOT__copper_pos[13U] = 0U;
    vlSelfRef.test_copper__DOT__dut__DOT__copper_reg[13U] = 0U;
    vlSelfRef.test_copper__DOT__dut__DOT__copper_val[13U] = 0U;
    vlSelfRef.test_copper__DOT__dut__DOT__copper_pos[14U] = 0U;
    vlSelfRef.test_copper__DOT__dut__DOT__copper_reg[14U] = 0U;
    vlSelfRef.test_copper__DOT__dut__DOT__copper_val[14U] = 0U;
    vlSelfRef.test_copper__DOT__dut__DOT__copper_pos[15U] = 0U;
    vlSelfRef.test_copper__DOT__dut__DOT__copper_reg[15U] = 0U;
    vlSelfRef.test_copper__DOT__dut__DOT__copper_val[15U] = 0U;
    vlSelfRef.test_copper__DOT__dut__DOT__copper_pos[16U] = 0U;
    vlSelfRef.test_copper__DOT__dut__DOT__copper_reg[16U] = 0U;
    vlSelfRef.test_copper__DOT__dut__DOT__copper_val[16U] = 0U;
    vlSelfRef.test_copper__DOT__dut__DOT__copper_pos[17U] = 0U;
    vlSelfRef.test_copper__DOT__dut__DOT__copper_reg[17U] = 0U;
    vlSelfRef.test_copper__DOT__dut__DOT__copper_val[17U] = 0U;
    vlSelfRef.test_copper__DOT__dut__DOT__copper_pos[18U] = 0U;
    vlSelfRef.test_copper__DOT__dut__DOT__copper_reg[18U] = 0U;
    vlSelfRef.test_copper__DOT__dut__DOT__copper_val[18U] = 0U;
    vlSelfRef.test_copper__DOT__dut__DOT__copper_pos[19U] = 0U;
    vlSelfRef.test_copper__DOT__dut__DOT__copper_reg[19U] = 0U;
    vlSelfRef.test_copper__DOT__dut__DOT__copper_val[19U] = 0U;
    vlSelfRef.test_copper__DOT__dut__DOT__copper_pos[20U] = 0U;
    vlSelfRef.test_copper__DOT__dut__DOT__copper_reg[20U] = 0U;
    vlSelfRef.test_copper__DOT__dut__DOT__copper_val[20U] = 0U;
    vlSelfRef.test_copper__DOT__dut__DOT__copper_pos[21U] = 0U;
    vlSelfRef.test_copper__DOT__dut__DOT__copper_reg[21U] = 0U;
    vlSelfRef.test_copper__DOT__dut__DOT__copper_val[21U] = 0U;
    vlSelfRef.test_copper__DOT__dut__DOT__copper_pos[22U] = 0U;
    vlSelfRef.test_copper__DOT__dut__DOT__copper_reg[22U] = 0U;
    vlSelfRef.test_copper__DOT__dut__DOT__copper_val[22U] = 0U;
    vlSelfRef.test_copper__DOT__dut__DOT__copper_pos[23U] = 0U;
    vlSelfRef.test_copper__DOT__dut__DOT__copper_reg[23U] = 0U;
    vlSelfRef.test_copper__DOT__dut__DOT__copper_val[23U] = 0U;
    vlSelfRef.test_copper__DOT__dut__DOT__copper_pos[24U] = 0U;
    vlSelfRef.test_copper__DOT__dut__DOT__copper_reg[24U] = 0U;
    vlSelfRef.test_copper__DOT__dut__DOT__copper_val[24U] = 0U;
    vlSelfRef.test_copper__DOT__dut__DOT__copper_pos[25U] = 0U;
    vlSelfRef.test_copper__DOT__dut__DOT__copper_reg[25U] = 0U;
    vlSelfRef.test_copper__DOT__dut__DOT__copper_val[25U] = 0U;
    vlSelfRef.test_copper__DOT__dut__DOT__copper_pos[26U] = 0U;
    vlSelfRef.test_copper__DOT__dut__DOT__copper_reg[26U] = 0U;
    vlSelfRef.test_copper__DOT__dut__DOT__copper_val[26U] = 0U;
    vlSelfRef.test_copper__DOT__dut__DOT__copper_pos[27U] = 0U;
    vlSelfRef.test_copper__DOT__dut__DOT__copper_reg[27U] = 0U;
    vlSelfRef.test_copper__DOT__dut__DOT__copper_val[27U] = 0U;
    vlSelfRef.test_copper__DOT__dut__DOT__copper_pos[28U] = 0U;
    vlSelfRef.test_copper__DOT__dut__DOT__copper_reg[28U] = 0U;
    vlSelfRef.test_copper__DOT__dut__DOT__copper_val[28U] = 0U;
    vlSelfRef.test_copper__DOT__dut__DOT__copper_pos[29U] = 0U;
    vlSelfRef.test_copper__DOT__dut__DOT__copper_reg[29U] = 0U;
    vlSelfRef.test_copper__DOT__dut__DOT__copper_val[29U] = 0U;
    vlSelfRef.test_copper__DOT__dut__DOT__copper_pos[30U] = 0U;
    vlSelfRef.test_copper__DOT__dut__DOT__copper_reg[30U] = 0U;
    vlSelfRef.test_copper__DOT__dut__DOT__copper_val[30U] = 0U;
    vlSelfRef.test_copper__DOT__dut__DOT__copper_pos[31U] = 0U;
    vlSelfRef.test_copper__DOT__dut__DOT__copper_reg[31U] = 0U;
    vlSelfRef.test_copper__DOT__dut__DOT__copper_val[31U] = 0U;
    test_copper__DOT__dut__DOT__unnamedblk6__DOT__i = 0U;
    while (VL_GTS_III(32, 0x00001000U, test_copper__DOT__dut__DOT__unnamedblk6__DOT__i)) {
        vlSelfRef.test_copper__DOT__dut__DOT__copper_list_pos[(0x00000fffU 
                                                               & test_copper__DOT__dut__DOT__unnamedblk6__DOT__i)] = 0U;
        vlSelfRef.test_copper__DOT__dut__DOT__copper_list_reg[(0x00000fffU 
                                                               & test_copper__DOT__dut__DOT__unnamedblk6__DOT__i)] = 0U;
        vlSelfRef.test_copper__DOT__dut__DOT__copper_list_val[(0x00000fffU 
                                                               & test_copper__DOT__dut__DOT__unnamedblk6__DOT__i)] = 0U;
        test_copper__DOT__dut__DOT__unnamedblk6__DOT__i 
            = ((IData)(1U) + test_copper__DOT__dut__DOT__unnamedblk6__DOT__i);
    }
    test_copper__DOT__dut__DOT__unnamedblk7__DOT__i = 0U;
    while (VL_GTS_III(32, 0x00000080U, test_copper__DOT__dut__DOT__unnamedblk7__DOT__i)) {
        vlSelfRef.test_copper__DOT__dut__DOT__copper_list_count[(0x0000007fU 
                                                                 & test_copper__DOT__dut__DOT__unnamedblk7__DOT__i)] = 0U;
        test_copper__DOT__dut__DOT__unnamedblk7__DOT__i 
            = ((IData)(1U) + test_copper__DOT__dut__DOT__unnamedblk7__DOT__i);
    }
    vlSelfRef.test_copper__DOT__dut__DOT__copper_count = 0U;
    vlSelfRef.test_copper__DOT__dut__DOT__copper_enabled = 0U;
    vlSelfRef.test_copper__DOT__dut__DOT__copper_target_list = 0U;
    vlSelfRef.test_copper__DOT__dut__DOT__copper_active_list = 0U;
    vlSelfRef.test_copper__DOT__dut__DOT__copper_pending_list = 0U;
    vlSelfRef.test_copper__DOT__dut__DOT__copper_loading = 0U;
    vlSelfRef.test_copper__DOT__dut__DOT__copper_load_idx = 0U;
    vlSelfRef.test_copper__DOT__dut__DOT__copper_load_src = 0U;
    __Vilp1 = 0U;
    while ((__Vilp1 <= 0x0000003fU)) {
        vlSelfRef.test_copper__DOT__dut__DOT__fio_name[__Vilp1] = 0U;
        __Vilp1 = ((IData)(1U) + __Vilp1);
    }
    vlSelfRef.test_copper__DOT__dut__DOT__fio_name_len = 0U;
    vlSelfRef.test_copper__DOT__dut__DOT__regs[0U] = 0U;
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
    vlSelfRef.test_copper__DOT__dut__DOT__cursor_x = 0U;
    vlSelfRef.test_copper__DOT__dut__DOT__cursor_y = 0U;
    vlSelfRef.test_copper__DOT__dut__DOT__fg_color = 1U;
    vlSelfRef.test_copper__DOT__dut__DOT__bg_color = 6U;
    vlSelfRef.test_copper__DOT__dut__DOT__gfx_color = 1U;
    test_copper__DOT__dut__DOT__spr_trans[0U] = 0U;
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
    vlSelfRef.test_copper__DOT__dut__DOT__scroll_offset = 0U;
    vlSelfRef.test_copper__DOT__dut__DOT__scroll_pending = 0U;
    vlSelfRef.test_copper__DOT__dut__DOT__scroll_col = 0U;
    vlSelfRef.test_copper__DOT__dut__DOT__cmd_busy = 0U;
    vlSelfRef.test_copper__DOT__dut__DOT__sprrow_count = 0U;
    vlSelfRef.test_copper__DOT__dut__DOT__sprcopy_phase = 0U;
    vlSelfRef.test_copper__DOT__dut__DOT__memread_pending = 0U;
    vlSelfRef.test_copper__DOT__dut__DOT__vgc_tile_addr = 0U;
    vlSelfRef.test_copper__DOT__dut__DOT__vgc_tile_wdata = 0U;
    vlSelfRef.test_copper__DOT__dut__DOT__vgc_tile_we = 0U;
    vlSelfRef.test_copper__DOT__dut__DOT__vgc_tile_re = 0U;
    vlSelfRef.test_copper__DOT__dut__DOT__artist_cmd_valid = 0U;
    test_copper__DOT__dut__DOT__text_inst__DOT__char_mem__DOT__gen_init_zero__DOT__i = 0U;
    while (VL_GTS_III(32, 0x000007d0U, test_copper__DOT__dut__DOT__text_inst__DOT__char_mem__DOT__gen_init_zero__DOT__i)) {
        vlSelfRef.test_copper__DOT__dut__DOT__text_inst__DOT__char_mem__DOT____Vlvbound_h3482bd38__0 = 0U;
        if (VL_LIKELY(((0x07cfU >= (0x000007ffU & test_copper__DOT__dut__DOT__text_inst__DOT__char_mem__DOT__gen_init_zero__DOT__i))))) {
            vlSelfRef.test_copper__DOT__dut__DOT__text_inst__DOT__char_mem__DOT__mem[(0x000007ffU 
                                                                                & test_copper__DOT__dut__DOT__text_inst__DOT__char_mem__DOT__gen_init_zero__DOT__i)] 
                = vlSelfRef.test_copper__DOT__dut__DOT__text_inst__DOT__char_mem__DOT____Vlvbound_h3482bd38__0;
        }
        test_copper__DOT__dut__DOT__text_inst__DOT__char_mem__DOT__gen_init_zero__DOT__i 
            = ((IData)(1U) + test_copper__DOT__dut__DOT__text_inst__DOT__char_mem__DOT__gen_init_zero__DOT__i);
    }
    test_copper__DOT__dut__DOT__text_inst__DOT__color_mem__DOT__gen_init_zero__DOT__i = 0U;
    while (VL_GTS_III(32, 0x000007d0U, test_copper__DOT__dut__DOT__text_inst__DOT__color_mem__DOT__gen_init_zero__DOT__i)) {
        vlSelfRef.test_copper__DOT__dut__DOT__text_inst__DOT__color_mem__DOT____Vlvbound_h3482bd38__0 = 0U;
        if (VL_LIKELY(((0x07cfU >= (0x000007ffU & test_copper__DOT__dut__DOT__text_inst__DOT__color_mem__DOT__gen_init_zero__DOT__i))))) {
            vlSelfRef.test_copper__DOT__dut__DOT__text_inst__DOT__color_mem__DOT__mem[(0x000007ffU 
                                                                                & test_copper__DOT__dut__DOT__text_inst__DOT__color_mem__DOT__gen_init_zero__DOT__i)] 
                = vlSelfRef.test_copper__DOT__dut__DOT__text_inst__DOT__color_mem__DOT____Vlvbound_h3482bd38__0;
        }
        test_copper__DOT__dut__DOT__text_inst__DOT__color_mem__DOT__gen_init_zero__DOT__i 
            = ((IData)(1U) + test_copper__DOT__dut__DOT__text_inst__DOT__color_mem__DOT__gen_init_zero__DOT__i);
    }
    VL_READMEM_N(true, 8, 2048, 0, "rom/cp437.hex"s
                 ,  &(vlSelfRef.test_copper__DOT__dut__DOT__text_inst__DOT__font_mem__DOT__mem)
                 , 0, ~0ULL);
    test_copper__DOT__dut__DOT__gfx_inst__DOT__gfx_mem__DOT__gen_init_zero__DOT__i = 0U;
    while (VL_GTS_III(32, 0x0000fa00U, test_copper__DOT__dut__DOT__gfx_inst__DOT__gfx_mem__DOT__gen_init_zero__DOT__i)) {
        vlSelfRef.test_copper__DOT__dut__DOT__gfx_inst__DOT__gfx_mem__DOT____Vlvbound_h75f189da__0 = 0U;
        if (VL_LIKELY(((0xf9ffU >= (0x0000ffffU & test_copper__DOT__dut__DOT__gfx_inst__DOT__gfx_mem__DOT__gen_init_zero__DOT__i))))) {
            vlSelfRef.test_copper__DOT__dut__DOT__gfx_inst__DOT__gfx_mem__DOT__mem[(0x0000ffffU 
                                                                                & test_copper__DOT__dut__DOT__gfx_inst__DOT__gfx_mem__DOT__gen_init_zero__DOT__i)] 
                = vlSelfRef.test_copper__DOT__dut__DOT__gfx_inst__DOT__gfx_mem__DOT____Vlvbound_h75f189da__0;
        }
        test_copper__DOT__dut__DOT__gfx_inst__DOT__gfx_mem__DOT__gen_init_zero__DOT__i 
            = ((IData)(1U) + test_copper__DOT__dut__DOT__gfx_inst__DOT__gfx_mem__DOT__gen_init_zero__DOT__i);
    }
    vlSelfRef.test_copper__DOT__dut__DOT__artist_busy = 0U;
    vlSelfRef.test_copper__DOT__dut__DOT__artist_inst__DOT__op = 0U;
    vlSelfRef.test_copper__DOT__dut__DOT__artist_inst__DOT__fill_sp = 0U;
    vlSelfRef.test_copper__DOT__dut__DOT__artist_inst__DOT__octant_count = 0U;
    vlSelfRef.test_copper__DOT__dut__DOT__artist_inst__DOT__paint_phase = 0U;
    vlSelfRef.test_copper__DOT__dut__DOT__artist_inst__DOT__gt_font_pending = 0U;
    vlSelfRef.test_copper__DOT__dut__DOT__artist_inst__DOT__paint_init = 0U;
    test_copper__DOT__dut__DOT__artist_inst__DOT__fill_stack_mem__DOT__gen_init_zero__DOT__i = 0U;
    while (VL_GTS_III(32, 0x00000200U, test_copper__DOT__dut__DOT__artist_inst__DOT__fill_stack_mem__DOT__gen_init_zero__DOT__i)) {
        vlSelfRef.test_copper__DOT__dut__DOT__artist_inst__DOT__fill_stack_mem__DOT__mem[(0x000001ffU 
                                                                                & test_copper__DOT__dut__DOT__artist_inst__DOT__fill_stack_mem__DOT__gen_init_zero__DOT__i)] = 0U;
        test_copper__DOT__dut__DOT__artist_inst__DOT__fill_stack_mem__DOT__gen_init_zero__DOT__i 
            = ((IData)(1U) + test_copper__DOT__dut__DOT__artist_inst__DOT__fill_stack_mem__DOT__gen_init_zero__DOT__i);
    }
    test_copper__DOT__dut__DOT__sprite_inst__DOT__spr_mem__DOT__gen_init_zero__DOT__i = 0U;
    while (VL_GTS_III(32, 0x00000800U, test_copper__DOT__dut__DOT__sprite_inst__DOT__spr_mem__DOT__gen_init_zero__DOT__i)) {
        vlSelfRef.test_copper__DOT__dut__DOT__sprite_inst__DOT__spr_mem__DOT__mem[(0x000007ffU 
                                                                                & test_copper__DOT__dut__DOT__sprite_inst__DOT__spr_mem__DOT__gen_init_zero__DOT__i)] = 0U;
        test_copper__DOT__dut__DOT__sprite_inst__DOT__spr_mem__DOT__gen_init_zero__DOT__i 
            = ((IData)(1U) + test_copper__DOT__dut__DOT__sprite_inst__DOT__spr_mem__DOT__gen_init_zero__DOT__i);
    }
    vlSelfRef.test_copper__DOT__dut__DOT__tile_inst__DOT__tregs[0U] = 0U;
    vlSelfRef.test_copper__DOT__dut__DOT__tile_inst__DOT__tregs[1U] = 0U;
    vlSelfRef.test_copper__DOT__dut__DOT__tile_inst__DOT__tregs[2U] = 0U;
    vlSelfRef.test_copper__DOT__dut__DOT__tile_inst__DOT__tregs[3U] = 0U;
    vlSelfRef.test_copper__DOT__dut__DOT__tile_inst__DOT__tregs[4U] = 0U;
    vlSelfRef.test_copper__DOT__dut__DOT__tile_inst__DOT__tregs[5U] = 0U;
    vlSelfRef.test_copper__DOT__dut__DOT__tile_inst__DOT__tregs[6U] = 0U;
    vlSelfRef.test_copper__DOT__dut__DOT__tile_inst__DOT__tregs[7U] = 0U;
    vlSelfRef.test_copper__DOT__dut__DOT__tile_inst__DOT__tregs[8U] = 0U;
    vlSelfRef.test_copper__DOT__dut__DOT__tile_inst__DOT__tregs[9U] = 0U;
    vlSelfRef.test_copper__DOT__dut__DOT__tile_inst__DOT__tregs[10U] = 0U;
    vlSelfRef.test_copper__DOT__dut__DOT__tile_inst__DOT__tregs[11U] = 0U;
    vlSelfRef.test_copper__DOT__dut__DOT__tile_inst__DOT__tregs[12U] = 0U;
    vlSelfRef.test_copper__DOT__dut__DOT__tile_inst__DOT__tregs[13U] = 0U;
    vlSelfRef.test_copper__DOT__dut__DOT__tile_inst__DOT__tregs[14U] = 0U;
    vlSelfRef.test_copper__DOT__dut__DOT__tile_inst__DOT__tregs[15U] = 0U;
    vlSelfRef.test_copper__DOT__dut__DOT__tile_inst__DOT__tregs[16U] = 0U;
    vlSelfRef.test_copper__DOT__dut__DOT__tile_inst__DOT__tregs[17U] = 0U;
    vlSelfRef.test_copper__DOT__dut__DOT__tile_inst__DOT__tregs[18U] = 0U;
    vlSelfRef.test_copper__DOT__dut__DOT__tile_inst__DOT__tregs[19U] = 0U;
    vlSelfRef.test_copper__DOT__dut__DOT__tile_inst__DOT__tregs[20U] = 0U;
    vlSelfRef.test_copper__DOT__dut__DOT__tile_inst__DOT__tregs[21U] = 0U;
    vlSelfRef.test_copper__DOT__dut__DOT__tile_inst__DOT__tregs[22U] = 0U;
    vlSelfRef.test_copper__DOT__dut__DOT__tile_inst__DOT__tregs[23U] = 0U;
    vlSelfRef.test_copper__DOT__dut__DOT__tile_inst__DOT__tregs[24U] = 0U;
    vlSelfRef.test_copper__DOT__dut__DOT__tile_inst__DOT__tregs[25U] = 0U;
    vlSelfRef.test_copper__DOT__dut__DOT__tile_inst__DOT__tregs[26U] = 0U;
    vlSelfRef.test_copper__DOT__dut__DOT__tile_inst__DOT__tregs[27U] = 0U;
    vlSelfRef.test_copper__DOT__dut__DOT__tile_inst__DOT__tregs[28U] = 0U;
    vlSelfRef.test_copper__DOT__dut__DOT__tile_inst__DOT__tregs[29U] = 0U;
    vlSelfRef.test_copper__DOT__dut__DOT__tile_inst__DOT__tregs[30U] = 0U;
    vlSelfRef.test_copper__DOT__dut__DOT__tile_inst__DOT__tregs[31U] = 0U;
    vlSelfRef.test_copper__DOT__dut__DOT__tile_inst__DOT__col_buffer[0U] = 0U;
    vlSelfRef.test_copper__DOT__dut__DOT__tile_inst__DOT__col_buffer[1U] = 0U;
    vlSelfRef.test_copper__DOT__dut__DOT__tile_inst__DOT__col_buffer[2U] = 0U;
    vlSelfRef.test_copper__DOT__dut__DOT__tile_inst__DOT__col_buffer[3U] = 0U;
    vlSelfRef.test_copper__DOT__dut__DOT__tile_inst__DOT__col_buffer[4U] = 0U;
    vlSelfRef.test_copper__DOT__dut__DOT__tile_inst__DOT__col_buffer[5U] = 0U;
    vlSelfRef.test_copper__DOT__dut__DOT__tile_inst__DOT__col_buffer[6U] = 0U;
    vlSelfRef.test_copper__DOT__dut__DOT__tile_inst__DOT__col_buffer[7U] = 0U;
    vlSelfRef.test_copper__DOT__dut__DOT__tile_inst__DOT__col_buffer[8U] = 0U;
    vlSelfRef.test_copper__DOT__dut__DOT__tile_inst__DOT__col_buffer[9U] = 0U;
    vlSelfRef.test_copper__DOT__dut__DOT__tile_inst__DOT__col_buffer[10U] = 0U;
    vlSelfRef.test_copper__DOT__dut__DOT__tile_inst__DOT__col_buffer[11U] = 0U;
    vlSelfRef.test_copper__DOT__dut__DOT__tile_inst__DOT__col_buffer[12U] = 0U;
    vlSelfRef.test_copper__DOT__dut__DOT__tile_inst__DOT__col_buffer[13U] = 0U;
    vlSelfRef.test_copper__DOT__dut__DOT__tile_inst__DOT__col_buffer[14U] = 0U;
    vlSelfRef.test_copper__DOT__dut__DOT__tile_inst__DOT__col_buffer[15U] = 0U;
    vlSelfRef.test_copper__DOT__dut__DOT__tile_inst__DOT__col_buffer[16U] = 0U;
    vlSelfRef.test_copper__DOT__dut__DOT__tile_inst__DOT__col_buffer[17U] = 0U;
    vlSelfRef.test_copper__DOT__dut__DOT__tile_inst__DOT__col_buffer[18U] = 0U;
    vlSelfRef.test_copper__DOT__dut__DOT__tile_inst__DOT__col_buffer[19U] = 0U;
    vlSelfRef.test_copper__DOT__dut__DOT__tile_inst__DOT__col_buffer[20U] = 0U;
    vlSelfRef.test_copper__DOT__dut__DOT__tile_inst__DOT__col_buffer[21U] = 0U;
    vlSelfRef.test_copper__DOT__dut__DOT__tile_inst__DOT__col_buffer[22U] = 0U;
    vlSelfRef.test_copper__DOT__dut__DOT__tile_inst__DOT__col_buffer[23U] = 0U;
    vlSelfRef.test_copper__DOT__dut__DOT__tile_inst__DOT__col_buffer[24U] = 0U;
    vlSelfRef.test_copper__DOT__dut__DOT__tile_inst__DOT__tile_size16 = 0U;
    vlSelfRef.test_copper__DOT__dut__DOT__tile_inst__DOT__dma_state = 0U;
    vlSelfRef.test_copper__DOT__dut__DOT__tile_inst__DOT__dma_src = 0U;
    vlSelfRef.test_copper__DOT__dut__DOT__tile_inst__DOT__dma_count = 0U;
    vlSelfRef.test_copper__DOT__dut__DOT__tile_inst__DOT__dma_dst = 0U;
    vlSelfRef.test_copper__DOT__dut__DOT__tile_inst__DOT__dma_nt = 0U;
    vlSelfRef.test_copper__DOT__dut__DOT__tile_inst__DOT__dma_col_row = 0U;
    vlSelfRef.test_copper__DOT__dut__DOT__tile_inst__DOT__dma_col_idx = 0U;
    vlSelfRef.test_copper__DOT__dut__DOT__tile_inst__DOT__dma_data_valid = 0U;
    vlSelfRef.test_copper__DOT__dut__DOT__tile_inst__DOT__dma_pal_phase = 0U;
    vlSelfRef.test_copper__DOT__dut__DOT__tile_inst__DOT__dma_pal_cidx = 0U;
    vlSelfRef.test_copper__DOT__dut__DOT__tile_inst__DOT__dma_fill_val = 0U;
    vlSelfRef.test_copper__DOT__dut__DOT__tile_inst__DOT__dma_fill_addr = 0U;
    test_copper__DOT__dut__DOT__tile_inst__DOT__tile_data_ram__DOT__gen_init_zero__DOT__i = 0U;
    while (VL_GTS_III(32, 0x00008000U, test_copper__DOT__dut__DOT__tile_inst__DOT__tile_data_ram__DOT__gen_init_zero__DOT__i)) {
        vlSelfRef.test_copper__DOT__dut__DOT__tile_inst__DOT__tile_data_ram__DOT__mem[(0x00007fffU 
                                                                                & test_copper__DOT__dut__DOT__tile_inst__DOT__tile_data_ram__DOT__gen_init_zero__DOT__i)] = 0U;
        test_copper__DOT__dut__DOT__tile_inst__DOT__tile_data_ram__DOT__gen_init_zero__DOT__i 
            = ((IData)(1U) + test_copper__DOT__dut__DOT__tile_inst__DOT__tile_data_ram__DOT__gen_init_zero__DOT__i);
    }
    test_copper__DOT__dut__DOT__tile_inst__DOT__nametable_ram__DOT__gen_init_zero__DOT__i = 0U;
    while (VL_GTS_III(32, 0x00001000U, test_copper__DOT__dut__DOT__tile_inst__DOT__nametable_ram__DOT__gen_init_zero__DOT__i)) {
        vlSelfRef.test_copper__DOT__dut__DOT__tile_inst__DOT__nametable_ram__DOT__mem[(0x00000fffU 
                                                                                & test_copper__DOT__dut__DOT__tile_inst__DOT__nametable_ram__DOT__gen_init_zero__DOT__i)] = 0U;
        test_copper__DOT__dut__DOT__tile_inst__DOT__nametable_ram__DOT__gen_init_zero__DOT__i 
            = ((IData)(1U) + test_copper__DOT__dut__DOT__tile_inst__DOT__nametable_ram__DOT__gen_init_zero__DOT__i);
    }
    test_copper__DOT__dut__DOT__tile_inst__DOT__attr_table_ram__DOT__gen_init_zero__DOT__i = 0U;
    while (VL_GTS_III(32, 0x00001000U, test_copper__DOT__dut__DOT__tile_inst__DOT__attr_table_ram__DOT__gen_init_zero__DOT__i)) {
        vlSelfRef.test_copper__DOT__dut__DOT__tile_inst__DOT__attr_table_ram__DOT__mem[(0x00000fffU 
                                                                                & test_copper__DOT__dut__DOT__tile_inst__DOT__attr_table_ram__DOT__gen_init_zero__DOT__i)] = 0U;
        test_copper__DOT__dut__DOT__tile_inst__DOT__attr_table_ram__DOT__gen_init_zero__DOT__i 
            = ((IData)(1U) + test_copper__DOT__dut__DOT__tile_inst__DOT__attr_table_ram__DOT__gen_init_zero__DOT__i);
    }
    VL_READMEM_N(true, 12, 256, 0, "rom/tile_pal0.hex"s
                 ,  &(test_copper__DOT__dut__DOT__tile_inst__DOT__pal_ram_inst__DOT__mem)
                 , 0, ~0ULL);
}

VL_ATTR_COLD void Vtest_copper___024root___eval_final(Vtest_copper___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vtest_copper___024root___eval_final\n"); );
    Vtest_copper__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
}

#ifdef VL_DEBUG
VL_ATTR_COLD void Vtest_copper___024root___dump_triggers__stl(const VlUnpacked<QData/*63:0*/, 1> &triggers, const std::string &tag);
#endif  // VL_DEBUG
VL_ATTR_COLD bool Vtest_copper___024root___eval_phase__stl(Vtest_copper___024root* vlSelf);

VL_ATTR_COLD void Vtest_copper___024root___eval_settle(Vtest_copper___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vtest_copper___024root___eval_settle\n"); );
    Vtest_copper__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
    // Locals
    IData/*31:0*/ __VstlIterCount;
    // Body
    __VstlIterCount = 0U;
    vlSelfRef.__VstlFirstIteration = 1U;
    do {
        if (VL_UNLIKELY(((0x00000064U < __VstlIterCount)))) {
#ifdef VL_DEBUG
            Vtest_copper___024root___dump_triggers__stl(vlSelfRef.__VstlTriggered, "stl"s);
#endif
            VL_FATAL_MT("test_copper.sv", 6, "", "DIDNOTCONVERGE: Settle region did not converge after '--converge-limit' of 100 tries");
        }
        __VstlIterCount = ((IData)(1U) + __VstlIterCount);
        vlSelfRef.__VstlPhaseResult = Vtest_copper___024root___eval_phase__stl(vlSelf);
        vlSelfRef.__VstlFirstIteration = 0U;
    } while (vlSelfRef.__VstlPhaseResult);
}

VL_ATTR_COLD void Vtest_copper___024root___eval_triggers_vec__stl(Vtest_copper___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vtest_copper___024root___eval_triggers_vec__stl\n"); );
    Vtest_copper__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
    // Body
    vlSelfRef.__VstlTriggered[0U] = ((0xfffffffffffffffeULL 
                                      & vlSelfRef.__VstlTriggered[0U]) 
                                     | (IData)((IData)(vlSelfRef.__VstlFirstIteration)));
}

VL_ATTR_COLD bool Vtest_copper___024root___trigger_anySet__stl(const VlUnpacked<QData/*63:0*/, 1> &in);

#ifdef VL_DEBUG
VL_ATTR_COLD void Vtest_copper___024root___dump_triggers__stl(const VlUnpacked<QData/*63:0*/, 1> &triggers, const std::string &tag) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vtest_copper___024root___dump_triggers__stl\n"); );
    // Body
    if ((1U & (~ (IData)(Vtest_copper___024root___trigger_anySet__stl(triggers))))) {
        VL_DBG_MSGS("         No '" + tag + "' region triggers active\n");
    }
    if ((1U & (IData)(triggers[0U]))) {
        VL_DBG_MSGS("         '" + tag + "' region trigger index 0 is active: Internal 'stl' trigger - first iteration\n");
    }
}
#endif  // VL_DEBUG

VL_ATTR_COLD bool Vtest_copper___024root___trigger_anySet__stl(const VlUnpacked<QData/*63:0*/, 1> &in) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vtest_copper___024root___trigger_anySet__stl\n"); );
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

VL_ATTR_COLD void Vtest_copper___024root___stl_sequent__TOP__0(Vtest_copper___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vtest_copper___024root___stl_sequent__TOP__0\n"); );
    Vtest_copper__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
    // Locals
    IData/*16:0*/ test_copper__DOT__dut__DOT____Vlvbound_h3e9a9ec3__0;
    test_copper__DOT__dut__DOT____Vlvbound_h3e9a9ec3__0 = 0;
    CData/*0:0*/ test_copper__DOT__dut__DOT__visible;
    test_copper__DOT__dut__DOT__visible = 0;
    CData/*4:0*/ test_copper__DOT__dut__DOT__text_row;
    test_copper__DOT__dut__DOT__text_row = 0;
    CData/*4:0*/ test_copper__DOT__dut__DOT__real_row;
    test_copper__DOT__dut__DOT__real_row = 0;
    SData/*9:0*/ test_copper__DOT__dut__DOT__text_line;
    test_copper__DOT__dut__DOT__text_line = 0;
    SData/*14:0*/ test_copper__DOT__dut__DOT__tile_blt_addr;
    test_copper__DOT__dut__DOT__tile_blt_addr = 0;
    CData/*7:0*/ test_copper__DOT__dut__DOT__tile_blt_wdata;
    test_copper__DOT__dut__DOT__tile_blt_wdata = 0;
    CData/*0:0*/ __VdfgRegularize_hebeb780c_0_0;
    __VdfgRegularize_hebeb780c_0_0 = 0;
    // Body
    vlSelfRef.test_copper__DOT__dut__DOT__fio_name_flat[0U] 
        = ((0xff000000U & vlSelfRef.test_copper__DOT__dut__DOT__fio_name_flat[0U]) 
           | ((vlSelfRef.test_copper__DOT__dut__DOT__fio_name[2U] 
               << 0x00000010U) | ((vlSelfRef.test_copper__DOT__dut__DOT__fio_name[1U] 
                                   << 8U) | vlSelfRef.test_copper__DOT__dut__DOT__fio_name[0U])));
    vlSelfRef.test_copper__DOT__dut__DOT__fio_name_flat[0U] 
        = ((0x00ffffffU & vlSelfRef.test_copper__DOT__dut__DOT__fio_name_flat[0U]) 
           | (vlSelfRef.test_copper__DOT__dut__DOT__fio_name[3U] 
              << 0x00000018U));
    vlSelfRef.test_copper__DOT__dut__DOT__fio_name_flat[1U] 
        = ((0xffff0000U & vlSelfRef.test_copper__DOT__dut__DOT__fio_name_flat[1U]) 
           | (0x00ffffffU & ((0x00ffff00U & (vlSelfRef.test_copper__DOT__dut__DOT__fio_name[5U] 
                                             << 8U)) 
                             | ((0x00ffffffU & vlSelfRef.test_copper__DOT__dut__DOT__fio_name[4U]) 
                                | (vlSelfRef.test_copper__DOT__dut__DOT__fio_name[3U] 
                                   >> 8U)))));
    vlSelfRef.test_copper__DOT__dut__DOT__fio_name_flat[1U] 
        = ((0x0000ffffU & vlSelfRef.test_copper__DOT__dut__DOT__fio_name_flat[1U]) 
           | (0xffff0000U & ((vlSelfRef.test_copper__DOT__dut__DOT__fio_name[7U] 
                              << 0x00000018U) | (vlSelfRef.test_copper__DOT__dut__DOT__fio_name[6U] 
                                                 << 0x00000010U))));
    vlSelfRef.test_copper__DOT__dut__DOT__fio_name_flat[2U] 
        = ((0xffffff00U & vlSelfRef.test_copper__DOT__dut__DOT__fio_name_flat[2U]) 
           | (0x0000ffffU & ((0x0000ffffU & vlSelfRef.test_copper__DOT__dut__DOT__fio_name[8U]) 
                             | ((0x0000ffffU & (vlSelfRef.test_copper__DOT__dut__DOT__fio_name[7U] 
                                                >> 8U)) 
                                | (vlSelfRef.test_copper__DOT__dut__DOT__fio_name[6U] 
                                   >> 0x00000010U)))));
    vlSelfRef.test_copper__DOT__dut__DOT__fio_name_flat[2U] 
        = ((0x000000ffU & vlSelfRef.test_copper__DOT__dut__DOT__fio_name_flat[2U]) 
           | (0xffffff00U & ((vlSelfRef.test_copper__DOT__dut__DOT__fio_name[11U] 
                              << 0x00000018U) | ((vlSelfRef.test_copper__DOT__dut__DOT__fio_name[10U] 
                                                  << 0x00000010U) 
                                                 | (vlSelfRef.test_copper__DOT__dut__DOT__fio_name[9U] 
                                                    << 8U)))));
    vlSelfRef.test_copper__DOT__dut__DOT__fio_name_flat[3U] 
        = ((0xff000000U & vlSelfRef.test_copper__DOT__dut__DOT__fio_name_flat[3U]) 
           | ((vlSelfRef.test_copper__DOT__dut__DOT__fio_name[14U] 
               << 0x00000010U) | ((vlSelfRef.test_copper__DOT__dut__DOT__fio_name[13U] 
                                   << 8U) | vlSelfRef.test_copper__DOT__dut__DOT__fio_name[12U])));
    vlSelfRef.test_copper__DOT__dut__DOT__fio_name_flat[3U] 
        = ((0x00ffffffU & vlSelfRef.test_copper__DOT__dut__DOT__fio_name_flat[3U]) 
           | (vlSelfRef.test_copper__DOT__dut__DOT__fio_name[15U] 
              << 0x00000018U));
    vlSelfRef.test_copper__DOT__dut__DOT__fio_name_flat[4U] 
        = ((0xffff0000U & vlSelfRef.test_copper__DOT__dut__DOT__fio_name_flat[4U]) 
           | (0x00ffffffU & ((0x00ffff00U & (vlSelfRef.test_copper__DOT__dut__DOT__fio_name[17U] 
                                             << 8U)) 
                             | ((0x00ffffffU & vlSelfRef.test_copper__DOT__dut__DOT__fio_name[16U]) 
                                | (vlSelfRef.test_copper__DOT__dut__DOT__fio_name[15U] 
                                   >> 8U)))));
    vlSelfRef.test_copper__DOT__dut__DOT__fio_name_flat[4U] 
        = ((0x0000ffffU & vlSelfRef.test_copper__DOT__dut__DOT__fio_name_flat[4U]) 
           | (0xffff0000U & ((vlSelfRef.test_copper__DOT__dut__DOT__fio_name[19U] 
                              << 0x00000018U) | (vlSelfRef.test_copper__DOT__dut__DOT__fio_name[18U] 
                                                 << 0x00000010U))));
    vlSelfRef.test_copper__DOT__dut__DOT__fio_name_flat[5U] 
        = ((0xffffff00U & vlSelfRef.test_copper__DOT__dut__DOT__fio_name_flat[5U]) 
           | (0x0000ffffU & ((0x0000ffffU & vlSelfRef.test_copper__DOT__dut__DOT__fio_name[20U]) 
                             | ((0x0000ffffU & (vlSelfRef.test_copper__DOT__dut__DOT__fio_name[19U] 
                                                >> 8U)) 
                                | (vlSelfRef.test_copper__DOT__dut__DOT__fio_name[18U] 
                                   >> 0x00000010U)))));
    vlSelfRef.test_copper__DOT__dut__DOT__fio_name_flat[5U] 
        = ((0x000000ffU & vlSelfRef.test_copper__DOT__dut__DOT__fio_name_flat[5U]) 
           | (0xffffff00U & ((vlSelfRef.test_copper__DOT__dut__DOT__fio_name[23U] 
                              << 0x00000018U) | ((vlSelfRef.test_copper__DOT__dut__DOT__fio_name[22U] 
                                                  << 0x00000010U) 
                                                 | (vlSelfRef.test_copper__DOT__dut__DOT__fio_name[21U] 
                                                    << 8U)))));
    vlSelfRef.test_copper__DOT__dut__DOT__fio_name_flat[6U] 
        = ((0xff000000U & vlSelfRef.test_copper__DOT__dut__DOT__fio_name_flat[6U]) 
           | ((vlSelfRef.test_copper__DOT__dut__DOT__fio_name[26U] 
               << 0x00000010U) | ((vlSelfRef.test_copper__DOT__dut__DOT__fio_name[25U] 
                                   << 8U) | vlSelfRef.test_copper__DOT__dut__DOT__fio_name[24U])));
    vlSelfRef.test_copper__DOT__dut__DOT__fio_name_flat[6U] 
        = ((0x00ffffffU & vlSelfRef.test_copper__DOT__dut__DOT__fio_name_flat[6U]) 
           | (vlSelfRef.test_copper__DOT__dut__DOT__fio_name[27U] 
              << 0x00000018U));
    vlSelfRef.test_copper__DOT__dut__DOT__fio_name_flat[7U] 
        = ((0xffff0000U & vlSelfRef.test_copper__DOT__dut__DOT__fio_name_flat[7U]) 
           | (0x00ffffffU & ((0x00ffff00U & (vlSelfRef.test_copper__DOT__dut__DOT__fio_name[29U] 
                                             << 8U)) 
                             | ((0x00ffffffU & vlSelfRef.test_copper__DOT__dut__DOT__fio_name[28U]) 
                                | (vlSelfRef.test_copper__DOT__dut__DOT__fio_name[27U] 
                                   >> 8U)))));
    vlSelfRef.test_copper__DOT__dut__DOT__fio_name_flat[7U] 
        = ((0x0000ffffU & vlSelfRef.test_copper__DOT__dut__DOT__fio_name_flat[7U]) 
           | (0xffff0000U & ((vlSelfRef.test_copper__DOT__dut__DOT__fio_name[31U] 
                              << 0x00000018U) | (vlSelfRef.test_copper__DOT__dut__DOT__fio_name[30U] 
                                                 << 0x00000010U))));
    vlSelfRef.test_copper__DOT__dut__DOT__fio_name_flat[8U] 
        = ((0xffffff00U & vlSelfRef.test_copper__DOT__dut__DOT__fio_name_flat[8U]) 
           | (0x0000ffffU & ((0x0000ffffU & vlSelfRef.test_copper__DOT__dut__DOT__fio_name[32U]) 
                             | ((0x0000ffffU & (vlSelfRef.test_copper__DOT__dut__DOT__fio_name[31U] 
                                                >> 8U)) 
                                | (vlSelfRef.test_copper__DOT__dut__DOT__fio_name[30U] 
                                   >> 0x00000010U)))));
    vlSelfRef.test_copper__DOT__dut__DOT__fio_name_flat[8U] 
        = ((0x000000ffU & vlSelfRef.test_copper__DOT__dut__DOT__fio_name_flat[8U]) 
           | (0xffffff00U & ((vlSelfRef.test_copper__DOT__dut__DOT__fio_name[35U] 
                              << 0x00000018U) | ((vlSelfRef.test_copper__DOT__dut__DOT__fio_name[34U] 
                                                  << 0x00000010U) 
                                                 | (vlSelfRef.test_copper__DOT__dut__DOT__fio_name[33U] 
                                                    << 8U)))));
    vlSelfRef.test_copper__DOT__dut__DOT__fio_name_flat[9U] 
        = ((0xff000000U & vlSelfRef.test_copper__DOT__dut__DOT__fio_name_flat[9U]) 
           | ((vlSelfRef.test_copper__DOT__dut__DOT__fio_name[38U] 
               << 0x00000010U) | ((vlSelfRef.test_copper__DOT__dut__DOT__fio_name[37U] 
                                   << 8U) | vlSelfRef.test_copper__DOT__dut__DOT__fio_name[36U])));
    vlSelfRef.test_copper__DOT__dut__DOT__fio_name_flat[9U] 
        = ((0x00ffffffU & vlSelfRef.test_copper__DOT__dut__DOT__fio_name_flat[9U]) 
           | (vlSelfRef.test_copper__DOT__dut__DOT__fio_name[39U] 
              << 0x00000018U));
    vlSelfRef.test_copper__DOT__dut__DOT__fio_name_flat[10U] 
        = ((0xffff0000U & vlSelfRef.test_copper__DOT__dut__DOT__fio_name_flat[10U]) 
           | (0x00ffffffU & ((0x00ffff00U & (vlSelfRef.test_copper__DOT__dut__DOT__fio_name[41U] 
                                             << 8U)) 
                             | ((0x00ffffffU & vlSelfRef.test_copper__DOT__dut__DOT__fio_name[40U]) 
                                | (vlSelfRef.test_copper__DOT__dut__DOT__fio_name[39U] 
                                   >> 8U)))));
    vlSelfRef.test_copper__DOT__dut__DOT__fio_name_flat[10U] 
        = ((0x0000ffffU & vlSelfRef.test_copper__DOT__dut__DOT__fio_name_flat[10U]) 
           | (0xffff0000U & ((vlSelfRef.test_copper__DOT__dut__DOT__fio_name[43U] 
                              << 0x00000018U) | (vlSelfRef.test_copper__DOT__dut__DOT__fio_name[42U] 
                                                 << 0x00000010U))));
    vlSelfRef.test_copper__DOT__dut__DOT__fio_name_flat[11U] 
        = ((0xffffff00U & vlSelfRef.test_copper__DOT__dut__DOT__fio_name_flat[11U]) 
           | (0x0000ffffU & ((0x0000ffffU & vlSelfRef.test_copper__DOT__dut__DOT__fio_name[44U]) 
                             | ((0x0000ffffU & (vlSelfRef.test_copper__DOT__dut__DOT__fio_name[43U] 
                                                >> 8U)) 
                                | (vlSelfRef.test_copper__DOT__dut__DOT__fio_name[42U] 
                                   >> 0x00000010U)))));
    vlSelfRef.test_copper__DOT__dut__DOT__fio_name_flat[11U] 
        = ((0x000000ffU & vlSelfRef.test_copper__DOT__dut__DOT__fio_name_flat[11U]) 
           | (0xffffff00U & ((vlSelfRef.test_copper__DOT__dut__DOT__fio_name[47U] 
                              << 0x00000018U) | ((vlSelfRef.test_copper__DOT__dut__DOT__fio_name[46U] 
                                                  << 0x00000010U) 
                                                 | (vlSelfRef.test_copper__DOT__dut__DOT__fio_name[45U] 
                                                    << 8U)))));
    vlSelfRef.test_copper__DOT__dut__DOT__fio_name_flat[12U] 
        = ((0xff000000U & vlSelfRef.test_copper__DOT__dut__DOT__fio_name_flat[12U]) 
           | ((vlSelfRef.test_copper__DOT__dut__DOT__fio_name[50U] 
               << 0x00000010U) | ((vlSelfRef.test_copper__DOT__dut__DOT__fio_name[49U] 
                                   << 8U) | vlSelfRef.test_copper__DOT__dut__DOT__fio_name[48U])));
    vlSelfRef.test_copper__DOT__dut__DOT__fio_name_flat[12U] 
        = ((0x00ffffffU & vlSelfRef.test_copper__DOT__dut__DOT__fio_name_flat[12U]) 
           | (vlSelfRef.test_copper__DOT__dut__DOT__fio_name[51U] 
              << 0x00000018U));
    vlSelfRef.test_copper__DOT__dut__DOT__fio_name_flat[13U] 
        = ((0xffff0000U & vlSelfRef.test_copper__DOT__dut__DOT__fio_name_flat[13U]) 
           | (0x00ffffffU & ((0x00ffff00U & (vlSelfRef.test_copper__DOT__dut__DOT__fio_name[53U] 
                                             << 8U)) 
                             | ((0x00ffffffU & vlSelfRef.test_copper__DOT__dut__DOT__fio_name[52U]) 
                                | (vlSelfRef.test_copper__DOT__dut__DOT__fio_name[51U] 
                                   >> 8U)))));
    vlSelfRef.test_copper__DOT__dut__DOT__fio_name_flat[13U] 
        = ((0x0000ffffU & vlSelfRef.test_copper__DOT__dut__DOT__fio_name_flat[13U]) 
           | (0xffff0000U & ((vlSelfRef.test_copper__DOT__dut__DOT__fio_name[55U] 
                              << 0x00000018U) | (vlSelfRef.test_copper__DOT__dut__DOT__fio_name[54U] 
                                                 << 0x00000010U))));
    vlSelfRef.test_copper__DOT__dut__DOT__fio_name_flat[14U] 
        = ((0xffffff00U & vlSelfRef.test_copper__DOT__dut__DOT__fio_name_flat[14U]) 
           | (0x0000ffffU & ((0x0000ffffU & vlSelfRef.test_copper__DOT__dut__DOT__fio_name[56U]) 
                             | ((0x0000ffffU & (vlSelfRef.test_copper__DOT__dut__DOT__fio_name[55U] 
                                                >> 8U)) 
                                | (vlSelfRef.test_copper__DOT__dut__DOT__fio_name[54U] 
                                   >> 0x00000010U)))));
    vlSelfRef.test_copper__DOT__dut__DOT__fio_name_flat[14U] 
        = ((0x000000ffU & vlSelfRef.test_copper__DOT__dut__DOT__fio_name_flat[14U]) 
           | (0xffffff00U & ((vlSelfRef.test_copper__DOT__dut__DOT__fio_name[59U] 
                              << 0x00000018U) | ((vlSelfRef.test_copper__DOT__dut__DOT__fio_name[58U] 
                                                  << 0x00000010U) 
                                                 | (vlSelfRef.test_copper__DOT__dut__DOT__fio_name[57U] 
                                                    << 8U)))));
    vlSelfRef.test_copper__DOT__dut__DOT__fio_name_flat[15U] 
        = ((0xff000000U & vlSelfRef.test_copper__DOT__dut__DOT__fio_name_flat[15U]) 
           | ((vlSelfRef.test_copper__DOT__dut__DOT__fio_name[62U] 
               << 0x00000010U) | ((vlSelfRef.test_copper__DOT__dut__DOT__fio_name[61U] 
                                   << 8U) | vlSelfRef.test_copper__DOT__dut__DOT__fio_name[60U])));
    vlSelfRef.test_copper__DOT__dut__DOT__fio_name_flat[15U] 
        = ((0x00ffffffU & vlSelfRef.test_copper__DOT__dut__DOT__fio_name_flat[15U]) 
           | (vlSelfRef.test_copper__DOT__dut__DOT__fio_name[63U] 
              << 0x00000018U));
    vlSelfRef.test_copper__DOT__dut__DOT__vgc_reg_sel 
        = ((0xa000U <= (IData)(vlSelfRef.test_copper__DOT__cpu_addr)) 
           & (0xa01fU >= (IData)(vlSelfRef.test_copper__DOT__cpu_addr)));
    vlSelfRef.test_copper__DOT__dut__DOT__spr_reg_sel 
        = ((0xa040U <= (IData)(vlSelfRef.test_copper__DOT__cpu_addr)) 
           & (0xa0bfU >= (IData)(vlSelfRef.test_copper__DOT__cpu_addr)));
    vlSelfRef.test_copper__DOT__dut__DOT__tile_reg_sel 
        = ((0xa0c0U <= (IData)(vlSelfRef.test_copper__DOT__cpu_addr)) 
           & (0xa0dfU >= (IData)(vlSelfRef.test_copper__DOT__cpu_addr)));
    vlSelfRef.test_copper__DOT__dut__DOT____Vcellinp__artist_inst__cmd_x0 
        = ((0x00000100U & (vlSelfRef.test_copper__DOT__dut__DOT__regs[18U] 
                           << 8U)) | vlSelfRef.test_copper__DOT__dut__DOT__regs[17U]);
    vlSelfRef.test_copper__DOT__dut__DOT____Vcellinp__artist_inst__cmd_x1 
        = ((0x00000100U & (vlSelfRef.test_copper__DOT__dut__DOT__regs[22U] 
                           << 8U)) | vlSelfRef.test_copper__DOT__dut__DOT__regs[21U]);
    vlSelfRef.test_copper__DOT__dut__DOT__gfx_a_we = 0U;
    vlSelfRef.test_copper__DOT__dut__DOT____Vcellinp__tile_inst__cpu_we 
        = ((IData)(vlSelfRef.test_copper__DOT__cpu_ce) 
           & (IData)(vlSelfRef.test_copper__DOT__cpu_we));
    vlSelfRef.test_copper__DOT__dut__DOT__font_a_addr 
        = ((IData)(vlSelfRef.test_copper__DOT__dut__DOT__artist_font_re)
            ? (IData)(vlSelfRef.test_copper__DOT__dut__DOT__artist_font_addr)
            : 0U);
    vlSelfRef.test_copper__DOT__dut__DOT__spr_a_addr = 0U;
    if (vlSelfRef.test_copper__DOT__dut__DOT__cmd_spr_we) {
        vlSelfRef.test_copper__DOT__dut__DOT__spr_a_addr 
            = vlSelfRef.test_copper__DOT__dut__DOT__cmd_spr_addr;
    } else if (vlSelfRef.test_copper__DOT__dut__DOT__cmd_spr_re) {
        vlSelfRef.test_copper__DOT__dut__DOT__spr_a_addr 
            = vlSelfRef.test_copper__DOT__dut__DOT__cmd_spr_addr;
    }
    vlSelfRef.test_copper__DOT__dut__DOT__tile_inst__DOT__td_we_a = 0U;
    vlSelfRef.test_copper__DOT__dut__DOT__tile_blt_rdata 
        = ((IData)(vlSelfRef.test_copper__DOT__dut__DOT__tile_inst__DOT__blt_tile_rd_pending)
            ? (IData)(vlSelfRef.test_copper__DOT__dut__DOT__tile_inst__DOT__td_dout_a)
            : (IData)(vlSelfRef.test_copper__DOT__dut__DOT__tile_inst__DOT__blt_tile_rd_latch));
    vlSelfRef.test_copper__DOT__dut__DOT__copper_reg_flat[0U] 
        = ((0xff000000U & vlSelfRef.test_copper__DOT__dut__DOT__copper_reg_flat[0U]) 
           | ((vlSelfRef.test_copper__DOT__dut__DOT__copper_reg[2U] 
               << 0x00000010U) | ((vlSelfRef.test_copper__DOT__dut__DOT__copper_reg[1U] 
                                   << 8U) | vlSelfRef.test_copper__DOT__dut__DOT__copper_reg[0U])));
    vlSelfRef.test_copper__DOT__dut__DOT__copper_reg_flat[0U] 
        = ((0x00ffffffU & vlSelfRef.test_copper__DOT__dut__DOT__copper_reg_flat[0U]) 
           | (vlSelfRef.test_copper__DOT__dut__DOT__copper_reg[3U] 
              << 0x00000018U));
    vlSelfRef.test_copper__DOT__dut__DOT__copper_reg_flat[1U] 
        = ((0xffff0000U & vlSelfRef.test_copper__DOT__dut__DOT__copper_reg_flat[1U]) 
           | (0x00ffffffU & ((0x00ffff00U & (vlSelfRef.test_copper__DOT__dut__DOT__copper_reg[5U] 
                                             << 8U)) 
                             | ((0x00ffffffU & vlSelfRef.test_copper__DOT__dut__DOT__copper_reg[4U]) 
                                | (vlSelfRef.test_copper__DOT__dut__DOT__copper_reg[3U] 
                                   >> 8U)))));
    vlSelfRef.test_copper__DOT__dut__DOT__copper_reg_flat[1U] 
        = ((0x0000ffffU & vlSelfRef.test_copper__DOT__dut__DOT__copper_reg_flat[1U]) 
           | (0xffff0000U & ((vlSelfRef.test_copper__DOT__dut__DOT__copper_reg[7U] 
                              << 0x00000018U) | (vlSelfRef.test_copper__DOT__dut__DOT__copper_reg[6U] 
                                                 << 0x00000010U))));
    vlSelfRef.test_copper__DOT__dut__DOT__copper_reg_flat[2U] 
        = ((0xffffff00U & vlSelfRef.test_copper__DOT__dut__DOT__copper_reg_flat[2U]) 
           | (0x0000ffffU & ((0x0000ffffU & vlSelfRef.test_copper__DOT__dut__DOT__copper_reg[8U]) 
                             | ((0x0000ffffU & (vlSelfRef.test_copper__DOT__dut__DOT__copper_reg[7U] 
                                                >> 8U)) 
                                | (vlSelfRef.test_copper__DOT__dut__DOT__copper_reg[6U] 
                                   >> 0x00000010U)))));
    vlSelfRef.test_copper__DOT__dut__DOT__copper_reg_flat[2U] 
        = ((0x000000ffU & vlSelfRef.test_copper__DOT__dut__DOT__copper_reg_flat[2U]) 
           | (0xffffff00U & ((vlSelfRef.test_copper__DOT__dut__DOT__copper_reg[11U] 
                              << 0x00000018U) | ((vlSelfRef.test_copper__DOT__dut__DOT__copper_reg[10U] 
                                                  << 0x00000010U) 
                                                 | (vlSelfRef.test_copper__DOT__dut__DOT__copper_reg[9U] 
                                                    << 8U)))));
    vlSelfRef.test_copper__DOT__dut__DOT__copper_reg_flat[3U] 
        = ((0xff000000U & vlSelfRef.test_copper__DOT__dut__DOT__copper_reg_flat[3U]) 
           | ((vlSelfRef.test_copper__DOT__dut__DOT__copper_reg[14U] 
               << 0x00000010U) | ((vlSelfRef.test_copper__DOT__dut__DOT__copper_reg[13U] 
                                   << 8U) | vlSelfRef.test_copper__DOT__dut__DOT__copper_reg[12U])));
    vlSelfRef.test_copper__DOT__dut__DOT__copper_reg_flat[3U] 
        = ((0x00ffffffU & vlSelfRef.test_copper__DOT__dut__DOT__copper_reg_flat[3U]) 
           | (vlSelfRef.test_copper__DOT__dut__DOT__copper_reg[15U] 
              << 0x00000018U));
    vlSelfRef.test_copper__DOT__dut__DOT__copper_reg_flat[4U] 
        = ((0xffff0000U & vlSelfRef.test_copper__DOT__dut__DOT__copper_reg_flat[4U]) 
           | (0x00ffffffU & ((0x00ffff00U & (vlSelfRef.test_copper__DOT__dut__DOT__copper_reg[17U] 
                                             << 8U)) 
                             | ((0x00ffffffU & vlSelfRef.test_copper__DOT__dut__DOT__copper_reg[16U]) 
                                | (vlSelfRef.test_copper__DOT__dut__DOT__copper_reg[15U] 
                                   >> 8U)))));
    vlSelfRef.test_copper__DOT__dut__DOT__copper_reg_flat[4U] 
        = ((0x0000ffffU & vlSelfRef.test_copper__DOT__dut__DOT__copper_reg_flat[4U]) 
           | (0xffff0000U & ((vlSelfRef.test_copper__DOT__dut__DOT__copper_reg[19U] 
                              << 0x00000018U) | (vlSelfRef.test_copper__DOT__dut__DOT__copper_reg[18U] 
                                                 << 0x00000010U))));
    vlSelfRef.test_copper__DOT__dut__DOT__copper_reg_flat[5U] 
        = ((0xffffff00U & vlSelfRef.test_copper__DOT__dut__DOT__copper_reg_flat[5U]) 
           | (0x0000ffffU & ((0x0000ffffU & vlSelfRef.test_copper__DOT__dut__DOT__copper_reg[20U]) 
                             | ((0x0000ffffU & (vlSelfRef.test_copper__DOT__dut__DOT__copper_reg[19U] 
                                                >> 8U)) 
                                | (vlSelfRef.test_copper__DOT__dut__DOT__copper_reg[18U] 
                                   >> 0x00000010U)))));
    vlSelfRef.test_copper__DOT__dut__DOT__copper_reg_flat[5U] 
        = ((0x000000ffU & vlSelfRef.test_copper__DOT__dut__DOT__copper_reg_flat[5U]) 
           | (0xffffff00U & ((vlSelfRef.test_copper__DOT__dut__DOT__copper_reg[23U] 
                              << 0x00000018U) | ((vlSelfRef.test_copper__DOT__dut__DOT__copper_reg[22U] 
                                                  << 0x00000010U) 
                                                 | (vlSelfRef.test_copper__DOT__dut__DOT__copper_reg[21U] 
                                                    << 8U)))));
    vlSelfRef.test_copper__DOT__dut__DOT__copper_reg_flat[6U] 
        = ((0xff000000U & vlSelfRef.test_copper__DOT__dut__DOT__copper_reg_flat[6U]) 
           | ((vlSelfRef.test_copper__DOT__dut__DOT__copper_reg[26U] 
               << 0x00000010U) | ((vlSelfRef.test_copper__DOT__dut__DOT__copper_reg[25U] 
                                   << 8U) | vlSelfRef.test_copper__DOT__dut__DOT__copper_reg[24U])));
    vlSelfRef.test_copper__DOT__dut__DOT__copper_reg_flat[6U] 
        = ((0x00ffffffU & vlSelfRef.test_copper__DOT__dut__DOT__copper_reg_flat[6U]) 
           | (vlSelfRef.test_copper__DOT__dut__DOT__copper_reg[27U] 
              << 0x00000018U));
    vlSelfRef.test_copper__DOT__dut__DOT__copper_reg_flat[7U] 
        = ((0xffff0000U & vlSelfRef.test_copper__DOT__dut__DOT__copper_reg_flat[7U]) 
           | (0x00ffffffU & ((0x00ffff00U & (vlSelfRef.test_copper__DOT__dut__DOT__copper_reg[29U] 
                                             << 8U)) 
                             | ((0x00ffffffU & vlSelfRef.test_copper__DOT__dut__DOT__copper_reg[28U]) 
                                | (vlSelfRef.test_copper__DOT__dut__DOT__copper_reg[27U] 
                                   >> 8U)))));
    vlSelfRef.test_copper__DOT__dut__DOT__copper_reg_flat[7U] 
        = ((0x0000ffffU & vlSelfRef.test_copper__DOT__dut__DOT__copper_reg_flat[7U]) 
           | (0xffff0000U & ((vlSelfRef.test_copper__DOT__dut__DOT__copper_reg[31U] 
                              << 0x00000018U) | (vlSelfRef.test_copper__DOT__dut__DOT__copper_reg[30U] 
                                                 << 0x00000010U))));
    vlSelfRef.test_copper__DOT__dut__DOT__copper_val_flat[0U] 
        = ((0xff000000U & vlSelfRef.test_copper__DOT__dut__DOT__copper_val_flat[0U]) 
           | ((vlSelfRef.test_copper__DOT__dut__DOT__copper_val[2U] 
               << 0x00000010U) | ((vlSelfRef.test_copper__DOT__dut__DOT__copper_val[1U] 
                                   << 8U) | vlSelfRef.test_copper__DOT__dut__DOT__copper_val[0U])));
    vlSelfRef.test_copper__DOT__dut__DOT__copper_val_flat[0U] 
        = ((0x00ffffffU & vlSelfRef.test_copper__DOT__dut__DOT__copper_val_flat[0U]) 
           | (vlSelfRef.test_copper__DOT__dut__DOT__copper_val[3U] 
              << 0x00000018U));
    vlSelfRef.test_copper__DOT__dut__DOT__copper_val_flat[1U] 
        = ((0xffff0000U & vlSelfRef.test_copper__DOT__dut__DOT__copper_val_flat[1U]) 
           | (0x00ffffffU & ((0x00ffff00U & (vlSelfRef.test_copper__DOT__dut__DOT__copper_val[5U] 
                                             << 8U)) 
                             | ((0x00ffffffU & vlSelfRef.test_copper__DOT__dut__DOT__copper_val[4U]) 
                                | (vlSelfRef.test_copper__DOT__dut__DOT__copper_val[3U] 
                                   >> 8U)))));
    vlSelfRef.test_copper__DOT__dut__DOT__copper_val_flat[1U] 
        = ((0x0000ffffU & vlSelfRef.test_copper__DOT__dut__DOT__copper_val_flat[1U]) 
           | (0xffff0000U & ((vlSelfRef.test_copper__DOT__dut__DOT__copper_val[7U] 
                              << 0x00000018U) | (vlSelfRef.test_copper__DOT__dut__DOT__copper_val[6U] 
                                                 << 0x00000010U))));
    vlSelfRef.test_copper__DOT__dut__DOT__copper_val_flat[2U] 
        = ((0xffffff00U & vlSelfRef.test_copper__DOT__dut__DOT__copper_val_flat[2U]) 
           | (0x0000ffffU & ((0x0000ffffU & vlSelfRef.test_copper__DOT__dut__DOT__copper_val[8U]) 
                             | ((0x0000ffffU & (vlSelfRef.test_copper__DOT__dut__DOT__copper_val[7U] 
                                                >> 8U)) 
                                | (vlSelfRef.test_copper__DOT__dut__DOT__copper_val[6U] 
                                   >> 0x00000010U)))));
    vlSelfRef.test_copper__DOT__dut__DOT__copper_val_flat[2U] 
        = ((0x000000ffU & vlSelfRef.test_copper__DOT__dut__DOT__copper_val_flat[2U]) 
           | (0xffffff00U & ((vlSelfRef.test_copper__DOT__dut__DOT__copper_val[11U] 
                              << 0x00000018U) | ((vlSelfRef.test_copper__DOT__dut__DOT__copper_val[10U] 
                                                  << 0x00000010U) 
                                                 | (vlSelfRef.test_copper__DOT__dut__DOT__copper_val[9U] 
                                                    << 8U)))));
    vlSelfRef.test_copper__DOT__dut__DOT__copper_val_flat[3U] 
        = ((0xff000000U & vlSelfRef.test_copper__DOT__dut__DOT__copper_val_flat[3U]) 
           | ((vlSelfRef.test_copper__DOT__dut__DOT__copper_val[14U] 
               << 0x00000010U) | ((vlSelfRef.test_copper__DOT__dut__DOT__copper_val[13U] 
                                   << 8U) | vlSelfRef.test_copper__DOT__dut__DOT__copper_val[12U])));
    vlSelfRef.test_copper__DOT__dut__DOT__copper_val_flat[3U] 
        = ((0x00ffffffU & vlSelfRef.test_copper__DOT__dut__DOT__copper_val_flat[3U]) 
           | (vlSelfRef.test_copper__DOT__dut__DOT__copper_val[15U] 
              << 0x00000018U));
    vlSelfRef.test_copper__DOT__dut__DOT__copper_val_flat[4U] 
        = ((0xffff0000U & vlSelfRef.test_copper__DOT__dut__DOT__copper_val_flat[4U]) 
           | (0x00ffffffU & ((0x00ffff00U & (vlSelfRef.test_copper__DOT__dut__DOT__copper_val[17U] 
                                             << 8U)) 
                             | ((0x00ffffffU & vlSelfRef.test_copper__DOT__dut__DOT__copper_val[16U]) 
                                | (vlSelfRef.test_copper__DOT__dut__DOT__copper_val[15U] 
                                   >> 8U)))));
    vlSelfRef.test_copper__DOT__dut__DOT__copper_val_flat[4U] 
        = ((0x0000ffffU & vlSelfRef.test_copper__DOT__dut__DOT__copper_val_flat[4U]) 
           | (0xffff0000U & ((vlSelfRef.test_copper__DOT__dut__DOT__copper_val[19U] 
                              << 0x00000018U) | (vlSelfRef.test_copper__DOT__dut__DOT__copper_val[18U] 
                                                 << 0x00000010U))));
    vlSelfRef.test_copper__DOT__dut__DOT__copper_val_flat[5U] 
        = ((0xffffff00U & vlSelfRef.test_copper__DOT__dut__DOT__copper_val_flat[5U]) 
           | (0x0000ffffU & ((0x0000ffffU & vlSelfRef.test_copper__DOT__dut__DOT__copper_val[20U]) 
                             | ((0x0000ffffU & (vlSelfRef.test_copper__DOT__dut__DOT__copper_val[19U] 
                                                >> 8U)) 
                                | (vlSelfRef.test_copper__DOT__dut__DOT__copper_val[18U] 
                                   >> 0x00000010U)))));
    vlSelfRef.test_copper__DOT__dut__DOT__copper_val_flat[5U] 
        = ((0x000000ffU & vlSelfRef.test_copper__DOT__dut__DOT__copper_val_flat[5U]) 
           | (0xffffff00U & ((vlSelfRef.test_copper__DOT__dut__DOT__copper_val[23U] 
                              << 0x00000018U) | ((vlSelfRef.test_copper__DOT__dut__DOT__copper_val[22U] 
                                                  << 0x00000010U) 
                                                 | (vlSelfRef.test_copper__DOT__dut__DOT__copper_val[21U] 
                                                    << 8U)))));
    vlSelfRef.test_copper__DOT__dut__DOT__copper_val_flat[6U] 
        = ((0xff000000U & vlSelfRef.test_copper__DOT__dut__DOT__copper_val_flat[6U]) 
           | ((vlSelfRef.test_copper__DOT__dut__DOT__copper_val[26U] 
               << 0x00000010U) | ((vlSelfRef.test_copper__DOT__dut__DOT__copper_val[25U] 
                                   << 8U) | vlSelfRef.test_copper__DOT__dut__DOT__copper_val[24U])));
    vlSelfRef.test_copper__DOT__dut__DOT__copper_val_flat[6U] 
        = ((0x00ffffffU & vlSelfRef.test_copper__DOT__dut__DOT__copper_val_flat[6U]) 
           | (vlSelfRef.test_copper__DOT__dut__DOT__copper_val[27U] 
              << 0x00000018U));
    vlSelfRef.test_copper__DOT__dut__DOT__copper_val_flat[7U] 
        = ((0xffff0000U & vlSelfRef.test_copper__DOT__dut__DOT__copper_val_flat[7U]) 
           | (0x00ffffffU & ((0x00ffff00U & (vlSelfRef.test_copper__DOT__dut__DOT__copper_val[29U] 
                                             << 8U)) 
                             | ((0x00ffffffU & vlSelfRef.test_copper__DOT__dut__DOT__copper_val[28U]) 
                                | (vlSelfRef.test_copper__DOT__dut__DOT__copper_val[27U] 
                                   >> 8U)))));
    vlSelfRef.test_copper__DOT__dut__DOT__copper_val_flat[7U] 
        = ((0x0000ffffU & vlSelfRef.test_copper__DOT__dut__DOT__copper_val_flat[7U]) 
           | (0xffff0000U & ((vlSelfRef.test_copper__DOT__dut__DOT__copper_val[31U] 
                              << 0x00000018U) | (vlSelfRef.test_copper__DOT__dut__DOT__copper_val[30U] 
                                                 << 0x00000010U))));
    vlSelfRef.test_copper__DOT__dut__DOT__gfx_a_din = 0U;
    vlSelfRef.test_copper__DOT__dut__DOT__tile_inst__DOT__at_we_a = 0U;
    vlSelfRef.test_copper__DOT__dut__DOT__tile_inst__DOT__at_din_a = 0U;
    test_copper__DOT__dut__DOT____Vlvbound_h3e9a9ec3__0 
        = vlSelfRef.test_copper__DOT__dut__DOT__copper_pos[0U];
    vlSelfRef.test_copper__DOT__dut__DOT__copper_pos_flat[0U] 
        = ((0xfffe0000U & vlSelfRef.test_copper__DOT__dut__DOT__copper_pos_flat[0U]) 
           | test_copper__DOT__dut__DOT____Vlvbound_h3e9a9ec3__0);
    test_copper__DOT__dut__DOT____Vlvbound_h3e9a9ec3__0 
        = vlSelfRef.test_copper__DOT__dut__DOT__copper_pos[1U];
    vlSelfRef.test_copper__DOT__dut__DOT__copper_pos_flat[0U] 
        = ((0x0001ffffU & vlSelfRef.test_copper__DOT__dut__DOT__copper_pos_flat[0U]) 
           | (test_copper__DOT__dut__DOT____Vlvbound_h3e9a9ec3__0 
              << 0x00000011U));
    vlSelfRef.test_copper__DOT__dut__DOT__copper_pos_flat[1U] 
        = ((0xfffffffcU & vlSelfRef.test_copper__DOT__dut__DOT__copper_pos_flat[1U]) 
           | (test_copper__DOT__dut__DOT____Vlvbound_h3e9a9ec3__0 
              >> 0x0000000fU));
    test_copper__DOT__dut__DOT____Vlvbound_h3e9a9ec3__0 
        = vlSelfRef.test_copper__DOT__dut__DOT__copper_pos[2U];
    vlSelfRef.test_copper__DOT__dut__DOT__copper_pos_flat[1U] 
        = ((0xfff80003U & vlSelfRef.test_copper__DOT__dut__DOT__copper_pos_flat[1U]) 
           | (test_copper__DOT__dut__DOT____Vlvbound_h3e9a9ec3__0 
              << 2U));
    test_copper__DOT__dut__DOT____Vlvbound_h3e9a9ec3__0 
        = vlSelfRef.test_copper__DOT__dut__DOT__copper_pos[3U];
    vlSelfRef.test_copper__DOT__dut__DOT__copper_pos_flat[1U] 
        = ((0x0007ffffU & vlSelfRef.test_copper__DOT__dut__DOT__copper_pos_flat[1U]) 
           | (test_copper__DOT__dut__DOT____Vlvbound_h3e9a9ec3__0 
              << 0x00000013U));
    vlSelfRef.test_copper__DOT__dut__DOT__copper_pos_flat[2U] 
        = ((0xfffffff0U & vlSelfRef.test_copper__DOT__dut__DOT__copper_pos_flat[2U]) 
           | (test_copper__DOT__dut__DOT____Vlvbound_h3e9a9ec3__0 
              >> 0x0000000dU));
    test_copper__DOT__dut__DOT____Vlvbound_h3e9a9ec3__0 
        = vlSelfRef.test_copper__DOT__dut__DOT__copper_pos[4U];
    vlSelfRef.test_copper__DOT__dut__DOT__copper_pos_flat[2U] 
        = ((0xffe0000fU & vlSelfRef.test_copper__DOT__dut__DOT__copper_pos_flat[2U]) 
           | (test_copper__DOT__dut__DOT____Vlvbound_h3e9a9ec3__0 
              << 4U));
    test_copper__DOT__dut__DOT____Vlvbound_h3e9a9ec3__0 
        = vlSelfRef.test_copper__DOT__dut__DOT__copper_pos[5U];
    vlSelfRef.test_copper__DOT__dut__DOT__copper_pos_flat[2U] 
        = ((0x001fffffU & vlSelfRef.test_copper__DOT__dut__DOT__copper_pos_flat[2U]) 
           | (test_copper__DOT__dut__DOT____Vlvbound_h3e9a9ec3__0 
              << 0x00000015U));
    vlSelfRef.test_copper__DOT__dut__DOT__copper_pos_flat[3U] 
        = ((0xffffffc0U & vlSelfRef.test_copper__DOT__dut__DOT__copper_pos_flat[3U]) 
           | (test_copper__DOT__dut__DOT____Vlvbound_h3e9a9ec3__0 
              >> 0x0000000bU));
    test_copper__DOT__dut__DOT____Vlvbound_h3e9a9ec3__0 
        = vlSelfRef.test_copper__DOT__dut__DOT__copper_pos[6U];
    vlSelfRef.test_copper__DOT__dut__DOT__copper_pos_flat[3U] 
        = ((0xff80003fU & vlSelfRef.test_copper__DOT__dut__DOT__copper_pos_flat[3U]) 
           | (test_copper__DOT__dut__DOT____Vlvbound_h3e9a9ec3__0 
              << 6U));
    test_copper__DOT__dut__DOT____Vlvbound_h3e9a9ec3__0 
        = vlSelfRef.test_copper__DOT__dut__DOT__copper_pos[7U];
    vlSelfRef.test_copper__DOT__dut__DOT__copper_pos_flat[3U] 
        = ((0x007fffffU & vlSelfRef.test_copper__DOT__dut__DOT__copper_pos_flat[3U]) 
           | (test_copper__DOT__dut__DOT____Vlvbound_h3e9a9ec3__0 
              << 0x00000017U));
    vlSelfRef.test_copper__DOT__dut__DOT__copper_pos_flat[4U] 
        = ((0xffffff00U & vlSelfRef.test_copper__DOT__dut__DOT__copper_pos_flat[4U]) 
           | (test_copper__DOT__dut__DOT____Vlvbound_h3e9a9ec3__0 
              >> 9U));
    test_copper__DOT__dut__DOT____Vlvbound_h3e9a9ec3__0 
        = vlSelfRef.test_copper__DOT__dut__DOT__copper_pos[8U];
    vlSelfRef.test_copper__DOT__dut__DOT__copper_pos_flat[4U] 
        = ((0xfe0000ffU & vlSelfRef.test_copper__DOT__dut__DOT__copper_pos_flat[4U]) 
           | (test_copper__DOT__dut__DOT____Vlvbound_h3e9a9ec3__0 
              << 8U));
    test_copper__DOT__dut__DOT____Vlvbound_h3e9a9ec3__0 
        = vlSelfRef.test_copper__DOT__dut__DOT__copper_pos[9U];
    vlSelfRef.test_copper__DOT__dut__DOT__copper_pos_flat[4U] 
        = ((0x01ffffffU & vlSelfRef.test_copper__DOT__dut__DOT__copper_pos_flat[4U]) 
           | (test_copper__DOT__dut__DOT____Vlvbound_h3e9a9ec3__0 
              << 0x00000019U));
    vlSelfRef.test_copper__DOT__dut__DOT__copper_pos_flat[5U] 
        = ((0xfffffc00U & vlSelfRef.test_copper__DOT__dut__DOT__copper_pos_flat[5U]) 
           | (test_copper__DOT__dut__DOT____Vlvbound_h3e9a9ec3__0 
              >> 7U));
    test_copper__DOT__dut__DOT____Vlvbound_h3e9a9ec3__0 
        = vlSelfRef.test_copper__DOT__dut__DOT__copper_pos[10U];
    vlSelfRef.test_copper__DOT__dut__DOT__copper_pos_flat[5U] 
        = ((0xf80003ffU & vlSelfRef.test_copper__DOT__dut__DOT__copper_pos_flat[5U]) 
           | (test_copper__DOT__dut__DOT____Vlvbound_h3e9a9ec3__0 
              << 0x0000000aU));
    test_copper__DOT__dut__DOT____Vlvbound_h3e9a9ec3__0 
        = vlSelfRef.test_copper__DOT__dut__DOT__copper_pos[11U];
    vlSelfRef.test_copper__DOT__dut__DOT__copper_pos_flat[5U] 
        = ((0x07ffffffU & vlSelfRef.test_copper__DOT__dut__DOT__copper_pos_flat[5U]) 
           | (test_copper__DOT__dut__DOT____Vlvbound_h3e9a9ec3__0 
              << 0x0000001bU));
    vlSelfRef.test_copper__DOT__dut__DOT__copper_pos_flat[6U] 
        = ((0xfffff000U & vlSelfRef.test_copper__DOT__dut__DOT__copper_pos_flat[6U]) 
           | (test_copper__DOT__dut__DOT____Vlvbound_h3e9a9ec3__0 
              >> 5U));
    test_copper__DOT__dut__DOT____Vlvbound_h3e9a9ec3__0 
        = vlSelfRef.test_copper__DOT__dut__DOT__copper_pos[12U];
    vlSelfRef.test_copper__DOT__dut__DOT__copper_pos_flat[6U] 
        = ((0xe0000fffU & vlSelfRef.test_copper__DOT__dut__DOT__copper_pos_flat[6U]) 
           | (test_copper__DOT__dut__DOT____Vlvbound_h3e9a9ec3__0 
              << 0x0000000cU));
    test_copper__DOT__dut__DOT____Vlvbound_h3e9a9ec3__0 
        = vlSelfRef.test_copper__DOT__dut__DOT__copper_pos[13U];
    vlSelfRef.test_copper__DOT__dut__DOT__copper_pos_flat[6U] 
        = ((0x1fffffffU & vlSelfRef.test_copper__DOT__dut__DOT__copper_pos_flat[6U]) 
           | (test_copper__DOT__dut__DOT____Vlvbound_h3e9a9ec3__0 
              << 0x0000001dU));
    vlSelfRef.test_copper__DOT__dut__DOT__copper_pos_flat[7U] 
        = ((0xffffc000U & vlSelfRef.test_copper__DOT__dut__DOT__copper_pos_flat[7U]) 
           | (test_copper__DOT__dut__DOT____Vlvbound_h3e9a9ec3__0 
              >> 3U));
    test_copper__DOT__dut__DOT____Vlvbound_h3e9a9ec3__0 
        = vlSelfRef.test_copper__DOT__dut__DOT__copper_pos[14U];
    vlSelfRef.test_copper__DOT__dut__DOT__copper_pos_flat[7U] 
        = ((0x80003fffU & vlSelfRef.test_copper__DOT__dut__DOT__copper_pos_flat[7U]) 
           | (test_copper__DOT__dut__DOT____Vlvbound_h3e9a9ec3__0 
              << 0x0000000eU));
    test_copper__DOT__dut__DOT____Vlvbound_h3e9a9ec3__0 
        = vlSelfRef.test_copper__DOT__dut__DOT__copper_pos[15U];
    vlSelfRef.test_copper__DOT__dut__DOT__copper_pos_flat[7U] 
        = ((0x7fffffffU & vlSelfRef.test_copper__DOT__dut__DOT__copper_pos_flat[7U]) 
           | (test_copper__DOT__dut__DOT____Vlvbound_h3e9a9ec3__0 
              << 0x0000001fU));
    vlSelfRef.test_copper__DOT__dut__DOT__copper_pos_flat[8U] 
        = ((0xffff0000U & vlSelfRef.test_copper__DOT__dut__DOT__copper_pos_flat[8U]) 
           | (test_copper__DOT__dut__DOT____Vlvbound_h3e9a9ec3__0 
              >> 1U));
    test_copper__DOT__dut__DOT____Vlvbound_h3e9a9ec3__0 
        = vlSelfRef.test_copper__DOT__dut__DOT__copper_pos[16U];
    vlSelfRef.test_copper__DOT__dut__DOT__copper_pos_flat[8U] 
        = ((0x0000ffffU & vlSelfRef.test_copper__DOT__dut__DOT__copper_pos_flat[8U]) 
           | (test_copper__DOT__dut__DOT____Vlvbound_h3e9a9ec3__0 
              << 0x00000010U));
    vlSelfRef.test_copper__DOT__dut__DOT__copper_pos_flat[9U] 
        = ((0xfffffffeU & vlSelfRef.test_copper__DOT__dut__DOT__copper_pos_flat[9U]) 
           | (test_copper__DOT__dut__DOT____Vlvbound_h3e9a9ec3__0 
              >> 0x00000010U));
    test_copper__DOT__dut__DOT____Vlvbound_h3e9a9ec3__0 
        = vlSelfRef.test_copper__DOT__dut__DOT__copper_pos[17U];
    vlSelfRef.test_copper__DOT__dut__DOT__copper_pos_flat[9U] 
        = ((0xfffc0001U & vlSelfRef.test_copper__DOT__dut__DOT__copper_pos_flat[9U]) 
           | (test_copper__DOT__dut__DOT____Vlvbound_h3e9a9ec3__0 
              << 1U));
    test_copper__DOT__dut__DOT____Vlvbound_h3e9a9ec3__0 
        = vlSelfRef.test_copper__DOT__dut__DOT__copper_pos[18U];
    vlSelfRef.test_copper__DOT__dut__DOT__copper_pos_flat[9U] 
        = ((0x0003ffffU & vlSelfRef.test_copper__DOT__dut__DOT__copper_pos_flat[9U]) 
           | (test_copper__DOT__dut__DOT____Vlvbound_h3e9a9ec3__0 
              << 0x00000012U));
    vlSelfRef.test_copper__DOT__dut__DOT__copper_pos_flat[10U] 
        = ((0xfffffff8U & vlSelfRef.test_copper__DOT__dut__DOT__copper_pos_flat[10U]) 
           | (test_copper__DOT__dut__DOT____Vlvbound_h3e9a9ec3__0 
              >> 0x0000000eU));
    test_copper__DOT__dut__DOT____Vlvbound_h3e9a9ec3__0 
        = vlSelfRef.test_copper__DOT__dut__DOT__copper_pos[19U];
    vlSelfRef.test_copper__DOT__dut__DOT__copper_pos_flat[10U] 
        = ((0xfff00007U & vlSelfRef.test_copper__DOT__dut__DOT__copper_pos_flat[10U]) 
           | (test_copper__DOT__dut__DOT____Vlvbound_h3e9a9ec3__0 
              << 3U));
    test_copper__DOT__dut__DOT____Vlvbound_h3e9a9ec3__0 
        = vlSelfRef.test_copper__DOT__dut__DOT__copper_pos[20U];
    vlSelfRef.test_copper__DOT__dut__DOT__copper_pos_flat[10U] 
        = ((0x000fffffU & vlSelfRef.test_copper__DOT__dut__DOT__copper_pos_flat[10U]) 
           | (test_copper__DOT__dut__DOT____Vlvbound_h3e9a9ec3__0 
              << 0x00000014U));
    vlSelfRef.test_copper__DOT__dut__DOT__copper_pos_flat[11U] 
        = ((0xffffffe0U & vlSelfRef.test_copper__DOT__dut__DOT__copper_pos_flat[11U]) 
           | (test_copper__DOT__dut__DOT____Vlvbound_h3e9a9ec3__0 
              >> 0x0000000cU));
    test_copper__DOT__dut__DOT____Vlvbound_h3e9a9ec3__0 
        = vlSelfRef.test_copper__DOT__dut__DOT__copper_pos[21U];
    vlSelfRef.test_copper__DOT__dut__DOT__copper_pos_flat[11U] 
        = ((0xffc0001fU & vlSelfRef.test_copper__DOT__dut__DOT__copper_pos_flat[11U]) 
           | (test_copper__DOT__dut__DOT____Vlvbound_h3e9a9ec3__0 
              << 5U));
    test_copper__DOT__dut__DOT____Vlvbound_h3e9a9ec3__0 
        = vlSelfRef.test_copper__DOT__dut__DOT__copper_pos[22U];
    vlSelfRef.test_copper__DOT__dut__DOT__copper_pos_flat[11U] 
        = ((0x003fffffU & vlSelfRef.test_copper__DOT__dut__DOT__copper_pos_flat[11U]) 
           | (test_copper__DOT__dut__DOT____Vlvbound_h3e9a9ec3__0 
              << 0x00000016U));
    vlSelfRef.test_copper__DOT__dut__DOT__copper_pos_flat[12U] 
        = ((0xffffff80U & vlSelfRef.test_copper__DOT__dut__DOT__copper_pos_flat[12U]) 
           | (test_copper__DOT__dut__DOT____Vlvbound_h3e9a9ec3__0 
              >> 0x0000000aU));
    test_copper__DOT__dut__DOT____Vlvbound_h3e9a9ec3__0 
        = vlSelfRef.test_copper__DOT__dut__DOT__copper_pos[23U];
    vlSelfRef.test_copper__DOT__dut__DOT__copper_pos_flat[12U] 
        = ((0xff00007fU & vlSelfRef.test_copper__DOT__dut__DOT__copper_pos_flat[12U]) 
           | (test_copper__DOT__dut__DOT____Vlvbound_h3e9a9ec3__0 
              << 7U));
    test_copper__DOT__dut__DOT____Vlvbound_h3e9a9ec3__0 
        = vlSelfRef.test_copper__DOT__dut__DOT__copper_pos[24U];
    vlSelfRef.test_copper__DOT__dut__DOT__copper_pos_flat[12U] 
        = ((0x00ffffffU & vlSelfRef.test_copper__DOT__dut__DOT__copper_pos_flat[12U]) 
           | (test_copper__DOT__dut__DOT____Vlvbound_h3e9a9ec3__0 
              << 0x00000018U));
    vlSelfRef.test_copper__DOT__dut__DOT__copper_pos_flat[13U] 
        = ((0xfffffe00U & vlSelfRef.test_copper__DOT__dut__DOT__copper_pos_flat[13U]) 
           | (test_copper__DOT__dut__DOT____Vlvbound_h3e9a9ec3__0 
              >> 8U));
    test_copper__DOT__dut__DOT____Vlvbound_h3e9a9ec3__0 
        = vlSelfRef.test_copper__DOT__dut__DOT__copper_pos[25U];
    vlSelfRef.test_copper__DOT__dut__DOT__copper_pos_flat[13U] 
        = ((0xfc0001ffU & vlSelfRef.test_copper__DOT__dut__DOT__copper_pos_flat[13U]) 
           | (test_copper__DOT__dut__DOT____Vlvbound_h3e9a9ec3__0 
              << 9U));
    test_copper__DOT__dut__DOT____Vlvbound_h3e9a9ec3__0 
        = vlSelfRef.test_copper__DOT__dut__DOT__copper_pos[26U];
    vlSelfRef.test_copper__DOT__dut__DOT__copper_pos_flat[13U] 
        = ((0x03ffffffU & vlSelfRef.test_copper__DOT__dut__DOT__copper_pos_flat[13U]) 
           | (test_copper__DOT__dut__DOT____Vlvbound_h3e9a9ec3__0 
              << 0x0000001aU));
    vlSelfRef.test_copper__DOT__dut__DOT__copper_pos_flat[14U] 
        = ((0xfffff800U & vlSelfRef.test_copper__DOT__dut__DOT__copper_pos_flat[14U]) 
           | (test_copper__DOT__dut__DOT____Vlvbound_h3e9a9ec3__0 
              >> 6U));
    test_copper__DOT__dut__DOT____Vlvbound_h3e9a9ec3__0 
        = vlSelfRef.test_copper__DOT__dut__DOT__copper_pos[27U];
    vlSelfRef.test_copper__DOT__dut__DOT__copper_pos_flat[14U] 
        = ((0xf00007ffU & vlSelfRef.test_copper__DOT__dut__DOT__copper_pos_flat[14U]) 
           | (test_copper__DOT__dut__DOT____Vlvbound_h3e9a9ec3__0 
              << 0x0000000bU));
    test_copper__DOT__dut__DOT____Vlvbound_h3e9a9ec3__0 
        = vlSelfRef.test_copper__DOT__dut__DOT__copper_pos[28U];
    vlSelfRef.test_copper__DOT__dut__DOT__copper_pos_flat[14U] 
        = ((0x0fffffffU & vlSelfRef.test_copper__DOT__dut__DOT__copper_pos_flat[14U]) 
           | (test_copper__DOT__dut__DOT____Vlvbound_h3e9a9ec3__0 
              << 0x0000001cU));
    vlSelfRef.test_copper__DOT__dut__DOT__copper_pos_flat[15U] 
        = ((0xffffe000U & vlSelfRef.test_copper__DOT__dut__DOT__copper_pos_flat[15U]) 
           | (test_copper__DOT__dut__DOT____Vlvbound_h3e9a9ec3__0 
              >> 4U));
    test_copper__DOT__dut__DOT____Vlvbound_h3e9a9ec3__0 
        = vlSelfRef.test_copper__DOT__dut__DOT__copper_pos[29U];
    vlSelfRef.test_copper__DOT__dut__DOT__copper_pos_flat[15U] 
        = ((0xc0001fffU & vlSelfRef.test_copper__DOT__dut__DOT__copper_pos_flat[15U]) 
           | (test_copper__DOT__dut__DOT____Vlvbound_h3e9a9ec3__0 
              << 0x0000000dU));
    test_copper__DOT__dut__DOT____Vlvbound_h3e9a9ec3__0 
        = vlSelfRef.test_copper__DOT__dut__DOT__copper_pos[30U];
    vlSelfRef.test_copper__DOT__dut__DOT__copper_pos_flat[15U] 
        = ((0x3fffffffU & vlSelfRef.test_copper__DOT__dut__DOT__copper_pos_flat[15U]) 
           | (test_copper__DOT__dut__DOT____Vlvbound_h3e9a9ec3__0 
              << 0x0000001eU));
    vlSelfRef.test_copper__DOT__dut__DOT__copper_pos_flat[16U] 
        = ((0xffff8000U & vlSelfRef.test_copper__DOT__dut__DOT__copper_pos_flat[16U]) 
           | (test_copper__DOT__dut__DOT____Vlvbound_h3e9a9ec3__0 
              >> 2U));
    test_copper__DOT__dut__DOT____Vlvbound_h3e9a9ec3__0 
        = vlSelfRef.test_copper__DOT__dut__DOT__copper_pos[31U];
    vlSelfRef.test_copper__DOT__dut__DOT__copper_pos_flat[16U] 
        = ((0x00007fffU & vlSelfRef.test_copper__DOT__dut__DOT__copper_pos_flat[16U]) 
           | (test_copper__DOT__dut__DOT____Vlvbound_h3e9a9ec3__0 
              << 0x0000000fU));
    vlSelfRef.test_copper__DOT__dut__DOT__tile_inst__DOT__nt_we_a = 0U;
    test_copper__DOT__dut__DOT__visible = ((0x0280U 
                                            > (IData)(vlSelfRef.test_copper__DOT__dut__DOT__h_count)) 
                                           & (0x01e0U 
                                              > (IData)(vlSelfRef.test_copper__DOT__dut__DOT__v_count)));
    vlSelfRef.test_copper__DOT__dut__DOT__gfx_a_addr = 0U;
    if (vlSelfRef.test_copper__DOT__dut__DOT__artist_gfx_we) {
        vlSelfRef.test_copper__DOT__dut__DOT__gfx_a_we = 1U;
        vlSelfRef.test_copper__DOT__dut__DOT__gfx_a_din 
            = vlSelfRef.test_copper__DOT__dut__DOT__artist_gfx_wdata;
        vlSelfRef.test_copper__DOT__dut__DOT__gfx_a_addr 
            = vlSelfRef.test_copper__DOT__dut__DOT__artist_gfx_addr;
    } else if (vlSelfRef.test_copper__DOT__dut__DOT__cmd_gfx_we) {
        vlSelfRef.test_copper__DOT__dut__DOT__gfx_a_we = 1U;
        vlSelfRef.test_copper__DOT__dut__DOT__gfx_a_din 
            = vlSelfRef.test_copper__DOT__dut__DOT__cmd_gfx_din;
        vlSelfRef.test_copper__DOT__dut__DOT__gfx_a_addr 
            = vlSelfRef.test_copper__DOT__dut__DOT__cmd_gfx_addr;
    } else if (vlSelfRef.test_copper__DOT__dut__DOT__artist_gfx_re) {
        vlSelfRef.test_copper__DOT__dut__DOT__gfx_a_addr 
            = vlSelfRef.test_copper__DOT__dut__DOT__artist_gfx_raddr;
    } else if (vlSelfRef.test_copper__DOT__dut__DOT__cmd_gfx_re) {
        vlSelfRef.test_copper__DOT__dut__DOT__gfx_a_addr 
            = vlSelfRef.test_copper__DOT__dut__DOT__cmd_gfx_addr;
    }
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
    test_copper__DOT__dut__DOT__tile_blt_wdata = ((IData)(vlSelfRef.test_copper__DOT__dut__DOT__vgc_tile_we)
                                                   ? (IData)(vlSelfRef.test_copper__DOT__dut__DOT__vgc_tile_wdata)
                                                   : 0U);
    vlSelfRef.test_copper__DOT__dut__DOT__tile_inst__DOT__nt_din_a = 0U;
    vlSelfRef.test_copper__DOT__dut__DOT__tile_inst__DOT__at_addr_a = 0U;
    vlSelfRef.test_copper__DOT__dut__DOT__tile_inst__DOT__nt_addr_a = 0U;
    vlSelfRef.test_copper__DOT__dut__DOT__char_ram_sel 
        = ((0xaa00U <= (IData)(vlSelfRef.test_copper__DOT__cpu_addr)) 
           & (0xb1cfU >= (IData)(vlSelfRef.test_copper__DOT__cpu_addr)));
    vlSelfRef.test_copper__DOT__dut__DOT__dbg_char_sel 
        = ((0xaa00U <= (IData)(vlSelfRef.test_copper__DOT__dut__DOT__dbg_addr)) 
           & (0xb1cfU >= (IData)(vlSelfRef.test_copper__DOT__dut__DOT__dbg_addr)));
    vlSelfRef.test_copper__DOT__dut__DOT__color_ram_sel 
        = ((0xb1d0U <= (IData)(vlSelfRef.test_copper__DOT__cpu_addr)) 
           & (0xb99fU >= (IData)(vlSelfRef.test_copper__DOT__cpu_addr)));
    vlSelfRef.test_copper__DOT__dut__DOT__dbg_color_sel 
        = ((0xb1d0U <= (IData)(vlSelfRef.test_copper__DOT__dut__DOT__dbg_addr)) 
           & (0xb99fU >= (IData)(vlSelfRef.test_copper__DOT__dut__DOT__dbg_addr)));
    test_copper__DOT__dut__DOT__tile_blt_addr = (((IData)(vlSelfRef.test_copper__DOT__dut__DOT__vgc_tile_re) 
                                                  | (IData)(vlSelfRef.test_copper__DOT__dut__DOT__vgc_tile_we))
                                                  ? (IData)(vlSelfRef.test_copper__DOT__dut__DOT__vgc_tile_addr)
                                                  : 0U);
    __VdfgRegularize_hebeb780c_0_0 = ((IData)(vlSelfRef.test_copper__DOT__cpu_ce) 
                                      & (IData)(vlSelfRef.test_copper__DOT__cpu_re));
    vlSelfRef.test_copper__DOT__dut__DOT__copper_inst__DOT__copper_reg[0U] 
        = (0x000000ffU & vlSelfRef.test_copper__DOT__dut__DOT__copper_reg_flat[0U]);
    vlSelfRef.test_copper__DOT__dut__DOT__copper_inst__DOT__copper_reg[1U] 
        = (0x000000ffU & (vlSelfRef.test_copper__DOT__dut__DOT__copper_reg_flat[0U] 
                          >> 8U));
    vlSelfRef.test_copper__DOT__dut__DOT__copper_inst__DOT__copper_reg[2U] 
        = (0x000000ffU & (vlSelfRef.test_copper__DOT__dut__DOT__copper_reg_flat[0U] 
                          >> 0x00000010U));
    vlSelfRef.test_copper__DOT__dut__DOT__copper_inst__DOT__copper_reg[3U] 
        = (vlSelfRef.test_copper__DOT__dut__DOT__copper_reg_flat[0U] 
           >> 0x00000018U);
    vlSelfRef.test_copper__DOT__dut__DOT__copper_inst__DOT__copper_reg[4U] 
        = (0x000000ffU & vlSelfRef.test_copper__DOT__dut__DOT__copper_reg_flat[1U]);
    vlSelfRef.test_copper__DOT__dut__DOT__copper_inst__DOT__copper_reg[5U] 
        = (0x000000ffU & (vlSelfRef.test_copper__DOT__dut__DOT__copper_reg_flat[1U] 
                          >> 8U));
    vlSelfRef.test_copper__DOT__dut__DOT__copper_inst__DOT__copper_reg[6U] 
        = (0x000000ffU & (vlSelfRef.test_copper__DOT__dut__DOT__copper_reg_flat[1U] 
                          >> 0x00000010U));
    vlSelfRef.test_copper__DOT__dut__DOT__copper_inst__DOT__copper_reg[7U] 
        = (vlSelfRef.test_copper__DOT__dut__DOT__copper_reg_flat[1U] 
           >> 0x00000018U);
    vlSelfRef.test_copper__DOT__dut__DOT__copper_inst__DOT__copper_reg[8U] 
        = (0x000000ffU & vlSelfRef.test_copper__DOT__dut__DOT__copper_reg_flat[2U]);
    vlSelfRef.test_copper__DOT__dut__DOT__copper_inst__DOT__copper_reg[9U] 
        = (0x000000ffU & (vlSelfRef.test_copper__DOT__dut__DOT__copper_reg_flat[2U] 
                          >> 8U));
    vlSelfRef.test_copper__DOT__dut__DOT__copper_inst__DOT__copper_reg[10U] 
        = (0x000000ffU & (vlSelfRef.test_copper__DOT__dut__DOT__copper_reg_flat[2U] 
                          >> 0x00000010U));
    vlSelfRef.test_copper__DOT__dut__DOT__copper_inst__DOT__copper_reg[11U] 
        = (vlSelfRef.test_copper__DOT__dut__DOT__copper_reg_flat[2U] 
           >> 0x00000018U);
    vlSelfRef.test_copper__DOT__dut__DOT__copper_inst__DOT__copper_reg[12U] 
        = (0x000000ffU & vlSelfRef.test_copper__DOT__dut__DOT__copper_reg_flat[3U]);
    vlSelfRef.test_copper__DOT__dut__DOT__copper_inst__DOT__copper_reg[13U] 
        = (0x000000ffU & (vlSelfRef.test_copper__DOT__dut__DOT__copper_reg_flat[3U] 
                          >> 8U));
    vlSelfRef.test_copper__DOT__dut__DOT__copper_inst__DOT__copper_reg[14U] 
        = (0x000000ffU & (vlSelfRef.test_copper__DOT__dut__DOT__copper_reg_flat[3U] 
                          >> 0x00000010U));
    vlSelfRef.test_copper__DOT__dut__DOT__copper_inst__DOT__copper_reg[15U] 
        = (vlSelfRef.test_copper__DOT__dut__DOT__copper_reg_flat[3U] 
           >> 0x00000018U);
    vlSelfRef.test_copper__DOT__dut__DOT__copper_inst__DOT__copper_reg[16U] 
        = (0x000000ffU & vlSelfRef.test_copper__DOT__dut__DOT__copper_reg_flat[4U]);
    vlSelfRef.test_copper__DOT__dut__DOT__copper_inst__DOT__copper_reg[17U] 
        = (0x000000ffU & (vlSelfRef.test_copper__DOT__dut__DOT__copper_reg_flat[4U] 
                          >> 8U));
    vlSelfRef.test_copper__DOT__dut__DOT__copper_inst__DOT__copper_reg[18U] 
        = (0x000000ffU & (vlSelfRef.test_copper__DOT__dut__DOT__copper_reg_flat[4U] 
                          >> 0x00000010U));
    vlSelfRef.test_copper__DOT__dut__DOT__copper_inst__DOT__copper_reg[19U] 
        = (vlSelfRef.test_copper__DOT__dut__DOT__copper_reg_flat[4U] 
           >> 0x00000018U);
    vlSelfRef.test_copper__DOT__dut__DOT__copper_inst__DOT__copper_reg[20U] 
        = (0x000000ffU & vlSelfRef.test_copper__DOT__dut__DOT__copper_reg_flat[5U]);
    vlSelfRef.test_copper__DOT__dut__DOT__copper_inst__DOT__copper_reg[21U] 
        = (0x000000ffU & (vlSelfRef.test_copper__DOT__dut__DOT__copper_reg_flat[5U] 
                          >> 8U));
    vlSelfRef.test_copper__DOT__dut__DOT__copper_inst__DOT__copper_reg[22U] 
        = (0x000000ffU & (vlSelfRef.test_copper__DOT__dut__DOT__copper_reg_flat[5U] 
                          >> 0x00000010U));
    vlSelfRef.test_copper__DOT__dut__DOT__copper_inst__DOT__copper_reg[23U] 
        = (vlSelfRef.test_copper__DOT__dut__DOT__copper_reg_flat[5U] 
           >> 0x00000018U);
    vlSelfRef.test_copper__DOT__dut__DOT__copper_inst__DOT__copper_reg[24U] 
        = (0x000000ffU & vlSelfRef.test_copper__DOT__dut__DOT__copper_reg_flat[6U]);
    vlSelfRef.test_copper__DOT__dut__DOT__copper_inst__DOT__copper_reg[25U] 
        = (0x000000ffU & (vlSelfRef.test_copper__DOT__dut__DOT__copper_reg_flat[6U] 
                          >> 8U));
    vlSelfRef.test_copper__DOT__dut__DOT__copper_inst__DOT__copper_reg[26U] 
        = (0x000000ffU & (vlSelfRef.test_copper__DOT__dut__DOT__copper_reg_flat[6U] 
                          >> 0x00000010U));
    vlSelfRef.test_copper__DOT__dut__DOT__copper_inst__DOT__copper_reg[27U] 
        = (vlSelfRef.test_copper__DOT__dut__DOT__copper_reg_flat[6U] 
           >> 0x00000018U);
    vlSelfRef.test_copper__DOT__dut__DOT__copper_inst__DOT__copper_reg[28U] 
        = (0x000000ffU & vlSelfRef.test_copper__DOT__dut__DOT__copper_reg_flat[7U]);
    vlSelfRef.test_copper__DOT__dut__DOT__copper_inst__DOT__copper_reg[29U] 
        = (0x000000ffU & (vlSelfRef.test_copper__DOT__dut__DOT__copper_reg_flat[7U] 
                          >> 8U));
    vlSelfRef.test_copper__DOT__dut__DOT__copper_inst__DOT__copper_reg[30U] 
        = (0x000000ffU & (vlSelfRef.test_copper__DOT__dut__DOT__copper_reg_flat[7U] 
                          >> 0x00000010U));
    vlSelfRef.test_copper__DOT__dut__DOT__copper_inst__DOT__copper_reg[31U] 
        = (vlSelfRef.test_copper__DOT__dut__DOT__copper_reg_flat[7U] 
           >> 0x00000018U);
    vlSelfRef.test_copper__DOT__dut__DOT__copper_inst__DOT__copper_val[0U] 
        = (0x000000ffU & vlSelfRef.test_copper__DOT__dut__DOT__copper_val_flat[0U]);
    vlSelfRef.test_copper__DOT__dut__DOT__copper_inst__DOT__copper_val[1U] 
        = (0x000000ffU & (vlSelfRef.test_copper__DOT__dut__DOT__copper_val_flat[0U] 
                          >> 8U));
    vlSelfRef.test_copper__DOT__dut__DOT__copper_inst__DOT__copper_val[2U] 
        = (0x000000ffU & (vlSelfRef.test_copper__DOT__dut__DOT__copper_val_flat[0U] 
                          >> 0x00000010U));
    vlSelfRef.test_copper__DOT__dut__DOT__copper_inst__DOT__copper_val[3U] 
        = (vlSelfRef.test_copper__DOT__dut__DOT__copper_val_flat[0U] 
           >> 0x00000018U);
    vlSelfRef.test_copper__DOT__dut__DOT__copper_inst__DOT__copper_val[4U] 
        = (0x000000ffU & vlSelfRef.test_copper__DOT__dut__DOT__copper_val_flat[1U]);
    vlSelfRef.test_copper__DOT__dut__DOT__copper_inst__DOT__copper_val[5U] 
        = (0x000000ffU & (vlSelfRef.test_copper__DOT__dut__DOT__copper_val_flat[1U] 
                          >> 8U));
    vlSelfRef.test_copper__DOT__dut__DOT__copper_inst__DOT__copper_val[6U] 
        = (0x000000ffU & (vlSelfRef.test_copper__DOT__dut__DOT__copper_val_flat[1U] 
                          >> 0x00000010U));
    vlSelfRef.test_copper__DOT__dut__DOT__copper_inst__DOT__copper_val[7U] 
        = (vlSelfRef.test_copper__DOT__dut__DOT__copper_val_flat[1U] 
           >> 0x00000018U);
    vlSelfRef.test_copper__DOT__dut__DOT__copper_inst__DOT__copper_val[8U] 
        = (0x000000ffU & vlSelfRef.test_copper__DOT__dut__DOT__copper_val_flat[2U]);
    vlSelfRef.test_copper__DOT__dut__DOT__copper_inst__DOT__copper_val[9U] 
        = (0x000000ffU & (vlSelfRef.test_copper__DOT__dut__DOT__copper_val_flat[2U] 
                          >> 8U));
    vlSelfRef.test_copper__DOT__dut__DOT__copper_inst__DOT__copper_val[10U] 
        = (0x000000ffU & (vlSelfRef.test_copper__DOT__dut__DOT__copper_val_flat[2U] 
                          >> 0x00000010U));
    vlSelfRef.test_copper__DOT__dut__DOT__copper_inst__DOT__copper_val[11U] 
        = (vlSelfRef.test_copper__DOT__dut__DOT__copper_val_flat[2U] 
           >> 0x00000018U);
    vlSelfRef.test_copper__DOT__dut__DOT__copper_inst__DOT__copper_val[12U] 
        = (0x000000ffU & vlSelfRef.test_copper__DOT__dut__DOT__copper_val_flat[3U]);
    vlSelfRef.test_copper__DOT__dut__DOT__copper_inst__DOT__copper_val[13U] 
        = (0x000000ffU & (vlSelfRef.test_copper__DOT__dut__DOT__copper_val_flat[3U] 
                          >> 8U));
    vlSelfRef.test_copper__DOT__dut__DOT__copper_inst__DOT__copper_val[14U] 
        = (0x000000ffU & (vlSelfRef.test_copper__DOT__dut__DOT__copper_val_flat[3U] 
                          >> 0x00000010U));
    vlSelfRef.test_copper__DOT__dut__DOT__copper_inst__DOT__copper_val[15U] 
        = (vlSelfRef.test_copper__DOT__dut__DOT__copper_val_flat[3U] 
           >> 0x00000018U);
    vlSelfRef.test_copper__DOT__dut__DOT__copper_inst__DOT__copper_val[16U] 
        = (0x000000ffU & vlSelfRef.test_copper__DOT__dut__DOT__copper_val_flat[4U]);
    vlSelfRef.test_copper__DOT__dut__DOT__copper_inst__DOT__copper_val[17U] 
        = (0x000000ffU & (vlSelfRef.test_copper__DOT__dut__DOT__copper_val_flat[4U] 
                          >> 8U));
    vlSelfRef.test_copper__DOT__dut__DOT__copper_inst__DOT__copper_val[18U] 
        = (0x000000ffU & (vlSelfRef.test_copper__DOT__dut__DOT__copper_val_flat[4U] 
                          >> 0x00000010U));
    vlSelfRef.test_copper__DOT__dut__DOT__copper_inst__DOT__copper_val[19U] 
        = (vlSelfRef.test_copper__DOT__dut__DOT__copper_val_flat[4U] 
           >> 0x00000018U);
    vlSelfRef.test_copper__DOT__dut__DOT__copper_inst__DOT__copper_val[20U] 
        = (0x000000ffU & vlSelfRef.test_copper__DOT__dut__DOT__copper_val_flat[5U]);
    vlSelfRef.test_copper__DOT__dut__DOT__copper_inst__DOT__copper_val[21U] 
        = (0x000000ffU & (vlSelfRef.test_copper__DOT__dut__DOT__copper_val_flat[5U] 
                          >> 8U));
    vlSelfRef.test_copper__DOT__dut__DOT__copper_inst__DOT__copper_val[22U] 
        = (0x000000ffU & (vlSelfRef.test_copper__DOT__dut__DOT__copper_val_flat[5U] 
                          >> 0x00000010U));
    vlSelfRef.test_copper__DOT__dut__DOT__copper_inst__DOT__copper_val[23U] 
        = (vlSelfRef.test_copper__DOT__dut__DOT__copper_val_flat[5U] 
           >> 0x00000018U);
    vlSelfRef.test_copper__DOT__dut__DOT__copper_inst__DOT__copper_val[24U] 
        = (0x000000ffU & vlSelfRef.test_copper__DOT__dut__DOT__copper_val_flat[6U]);
    vlSelfRef.test_copper__DOT__dut__DOT__copper_inst__DOT__copper_val[25U] 
        = (0x000000ffU & (vlSelfRef.test_copper__DOT__dut__DOT__copper_val_flat[6U] 
                          >> 8U));
    vlSelfRef.test_copper__DOT__dut__DOT__copper_inst__DOT__copper_val[26U] 
        = (0x000000ffU & (vlSelfRef.test_copper__DOT__dut__DOT__copper_val_flat[6U] 
                          >> 0x00000010U));
    vlSelfRef.test_copper__DOT__dut__DOT__copper_inst__DOT__copper_val[27U] 
        = (vlSelfRef.test_copper__DOT__dut__DOT__copper_val_flat[6U] 
           >> 0x00000018U);
    vlSelfRef.test_copper__DOT__dut__DOT__copper_inst__DOT__copper_val[28U] 
        = (0x000000ffU & vlSelfRef.test_copper__DOT__dut__DOT__copper_val_flat[7U]);
    vlSelfRef.test_copper__DOT__dut__DOT__copper_inst__DOT__copper_val[29U] 
        = (0x000000ffU & (vlSelfRef.test_copper__DOT__dut__DOT__copper_val_flat[7U] 
                          >> 8U));
    vlSelfRef.test_copper__DOT__dut__DOT__copper_inst__DOT__copper_val[30U] 
        = (0x000000ffU & (vlSelfRef.test_copper__DOT__dut__DOT__copper_val_flat[7U] 
                          >> 0x00000010U));
    vlSelfRef.test_copper__DOT__dut__DOT__copper_inst__DOT__copper_val[31U] 
        = (vlSelfRef.test_copper__DOT__dut__DOT__copper_val_flat[7U] 
           >> 0x00000018U);
    vlSelfRef.test_copper__DOT__dut__DOT__copper_inst__DOT__copper_pos[0U] 
        = (0x0001ffffU & vlSelfRef.test_copper__DOT__dut__DOT__copper_pos_flat[0U]);
    vlSelfRef.test_copper__DOT__dut__DOT__copper_inst__DOT__copper_pos[1U] 
        = (0x0001ffffU & ((vlSelfRef.test_copper__DOT__dut__DOT__copper_pos_flat[1U] 
                           << 0x0000000fU) | (vlSelfRef.test_copper__DOT__dut__DOT__copper_pos_flat[0U] 
                                              >> 0x00000011U)));
    vlSelfRef.test_copper__DOT__dut__DOT__copper_inst__DOT__copper_pos[2U] 
        = (0x0001ffffU & (vlSelfRef.test_copper__DOT__dut__DOT__copper_pos_flat[1U] 
                          >> 2U));
    vlSelfRef.test_copper__DOT__dut__DOT__copper_inst__DOT__copper_pos[3U] 
        = (0x0001ffffU & ((vlSelfRef.test_copper__DOT__dut__DOT__copper_pos_flat[2U] 
                           << 0x0000000dU) | (vlSelfRef.test_copper__DOT__dut__DOT__copper_pos_flat[1U] 
                                              >> 0x00000013U)));
    vlSelfRef.test_copper__DOT__dut__DOT__copper_inst__DOT__copper_pos[4U] 
        = (0x0001ffffU & (vlSelfRef.test_copper__DOT__dut__DOT__copper_pos_flat[2U] 
                          >> 4U));
    vlSelfRef.test_copper__DOT__dut__DOT__copper_inst__DOT__copper_pos[5U] 
        = (0x0001ffffU & ((vlSelfRef.test_copper__DOT__dut__DOT__copper_pos_flat[3U] 
                           << 0x0000000bU) | (vlSelfRef.test_copper__DOT__dut__DOT__copper_pos_flat[2U] 
                                              >> 0x00000015U)));
    vlSelfRef.test_copper__DOT__dut__DOT__copper_inst__DOT__copper_pos[6U] 
        = (0x0001ffffU & (vlSelfRef.test_copper__DOT__dut__DOT__copper_pos_flat[3U] 
                          >> 6U));
    vlSelfRef.test_copper__DOT__dut__DOT__copper_inst__DOT__copper_pos[7U] 
        = (0x0001ffffU & ((vlSelfRef.test_copper__DOT__dut__DOT__copper_pos_flat[4U] 
                           << 9U) | (vlSelfRef.test_copper__DOT__dut__DOT__copper_pos_flat[3U] 
                                     >> 0x00000017U)));
    vlSelfRef.test_copper__DOT__dut__DOT__copper_inst__DOT__copper_pos[8U] 
        = (0x0001ffffU & (vlSelfRef.test_copper__DOT__dut__DOT__copper_pos_flat[4U] 
                          >> 8U));
    vlSelfRef.test_copper__DOT__dut__DOT__copper_inst__DOT__copper_pos[9U] 
        = (0x0001ffffU & ((vlSelfRef.test_copper__DOT__dut__DOT__copper_pos_flat[5U] 
                           << 7U) | (vlSelfRef.test_copper__DOT__dut__DOT__copper_pos_flat[4U] 
                                     >> 0x00000019U)));
    vlSelfRef.test_copper__DOT__dut__DOT__copper_inst__DOT__copper_pos[10U] 
        = (0x0001ffffU & (vlSelfRef.test_copper__DOT__dut__DOT__copper_pos_flat[5U] 
                          >> 0x0000000aU));
    vlSelfRef.test_copper__DOT__dut__DOT__copper_inst__DOT__copper_pos[11U] 
        = (0x0001ffffU & ((vlSelfRef.test_copper__DOT__dut__DOT__copper_pos_flat[6U] 
                           << 5U) | (vlSelfRef.test_copper__DOT__dut__DOT__copper_pos_flat[5U] 
                                     >> 0x0000001bU)));
    vlSelfRef.test_copper__DOT__dut__DOT__copper_inst__DOT__copper_pos[12U] 
        = (0x0001ffffU & (vlSelfRef.test_copper__DOT__dut__DOT__copper_pos_flat[6U] 
                          >> 0x0000000cU));
    vlSelfRef.test_copper__DOT__dut__DOT__copper_inst__DOT__copper_pos[13U] 
        = (0x0001ffffU & ((vlSelfRef.test_copper__DOT__dut__DOT__copper_pos_flat[7U] 
                           << 3U) | (vlSelfRef.test_copper__DOT__dut__DOT__copper_pos_flat[6U] 
                                     >> 0x0000001dU)));
    vlSelfRef.test_copper__DOT__dut__DOT__copper_inst__DOT__copper_pos[14U] 
        = (0x0001ffffU & (vlSelfRef.test_copper__DOT__dut__DOT__copper_pos_flat[7U] 
                          >> 0x0000000eU));
    vlSelfRef.test_copper__DOT__dut__DOT__copper_inst__DOT__copper_pos[15U] 
        = (0x0001ffffU & ((vlSelfRef.test_copper__DOT__dut__DOT__copper_pos_flat[8U] 
                           << 1U) | (vlSelfRef.test_copper__DOT__dut__DOT__copper_pos_flat[7U] 
                                     >> 0x0000001fU)));
    vlSelfRef.test_copper__DOT__dut__DOT__copper_inst__DOT__copper_pos[16U] 
        = (0x0001ffffU & ((vlSelfRef.test_copper__DOT__dut__DOT__copper_pos_flat[9U] 
                           << 0x00000010U) | (vlSelfRef.test_copper__DOT__dut__DOT__copper_pos_flat[8U] 
                                              >> 0x00000010U)));
    vlSelfRef.test_copper__DOT__dut__DOT__copper_inst__DOT__copper_pos[17U] 
        = (0x0001ffffU & (vlSelfRef.test_copper__DOT__dut__DOT__copper_pos_flat[9U] 
                          >> 1U));
    vlSelfRef.test_copper__DOT__dut__DOT__copper_inst__DOT__copper_pos[18U] 
        = (0x0001ffffU & ((vlSelfRef.test_copper__DOT__dut__DOT__copper_pos_flat[10U] 
                           << 0x0000000eU) | (vlSelfRef.test_copper__DOT__dut__DOT__copper_pos_flat[9U] 
                                              >> 0x00000012U)));
    vlSelfRef.test_copper__DOT__dut__DOT__copper_inst__DOT__copper_pos[19U] 
        = (0x0001ffffU & (vlSelfRef.test_copper__DOT__dut__DOT__copper_pos_flat[10U] 
                          >> 3U));
    vlSelfRef.test_copper__DOT__dut__DOT__copper_inst__DOT__copper_pos[20U] 
        = (0x0001ffffU & ((vlSelfRef.test_copper__DOT__dut__DOT__copper_pos_flat[11U] 
                           << 0x0000000cU) | (vlSelfRef.test_copper__DOT__dut__DOT__copper_pos_flat[10U] 
                                              >> 0x00000014U)));
    vlSelfRef.test_copper__DOT__dut__DOT__copper_inst__DOT__copper_pos[21U] 
        = (0x0001ffffU & (vlSelfRef.test_copper__DOT__dut__DOT__copper_pos_flat[11U] 
                          >> 5U));
    vlSelfRef.test_copper__DOT__dut__DOT__copper_inst__DOT__copper_pos[22U] 
        = (0x0001ffffU & ((vlSelfRef.test_copper__DOT__dut__DOT__copper_pos_flat[12U] 
                           << 0x0000000aU) | (vlSelfRef.test_copper__DOT__dut__DOT__copper_pos_flat[11U] 
                                              >> 0x00000016U)));
    vlSelfRef.test_copper__DOT__dut__DOT__copper_inst__DOT__copper_pos[23U] 
        = (0x0001ffffU & (vlSelfRef.test_copper__DOT__dut__DOT__copper_pos_flat[12U] 
                          >> 7U));
    vlSelfRef.test_copper__DOT__dut__DOT__copper_inst__DOT__copper_pos[24U] 
        = (0x0001ffffU & ((vlSelfRef.test_copper__DOT__dut__DOT__copper_pos_flat[13U] 
                           << 8U) | (vlSelfRef.test_copper__DOT__dut__DOT__copper_pos_flat[12U] 
                                     >> 0x00000018U)));
    vlSelfRef.test_copper__DOT__dut__DOT__copper_inst__DOT__copper_pos[25U] 
        = (0x0001ffffU & (vlSelfRef.test_copper__DOT__dut__DOT__copper_pos_flat[13U] 
                          >> 9U));
    vlSelfRef.test_copper__DOT__dut__DOT__copper_inst__DOT__copper_pos[26U] 
        = (0x0001ffffU & ((vlSelfRef.test_copper__DOT__dut__DOT__copper_pos_flat[14U] 
                           << 6U) | (vlSelfRef.test_copper__DOT__dut__DOT__copper_pos_flat[13U] 
                                     >> 0x0000001aU)));
    vlSelfRef.test_copper__DOT__dut__DOT__copper_inst__DOT__copper_pos[27U] 
        = (0x0001ffffU & (vlSelfRef.test_copper__DOT__dut__DOT__copper_pos_flat[14U] 
                          >> 0x0000000bU));
    vlSelfRef.test_copper__DOT__dut__DOT__copper_inst__DOT__copper_pos[28U] 
        = (0x0001ffffU & ((vlSelfRef.test_copper__DOT__dut__DOT__copper_pos_flat[15U] 
                           << 4U) | (vlSelfRef.test_copper__DOT__dut__DOT__copper_pos_flat[14U] 
                                     >> 0x0000001cU)));
    vlSelfRef.test_copper__DOT__dut__DOT__copper_inst__DOT__copper_pos[29U] 
        = (0x0001ffffU & (vlSelfRef.test_copper__DOT__dut__DOT__copper_pos_flat[15U] 
                          >> 0x0000000dU));
    vlSelfRef.test_copper__DOT__dut__DOT__copper_inst__DOT__copper_pos[30U] 
        = (0x0001ffffU & ((vlSelfRef.test_copper__DOT__dut__DOT__copper_pos_flat[16U] 
                           << 2U) | (vlSelfRef.test_copper__DOT__dut__DOT__copper_pos_flat[15U] 
                                     >> 0x0000001eU)));
    vlSelfRef.test_copper__DOT__dut__DOT__copper_inst__DOT__copper_pos[31U] 
        = (vlSelfRef.test_copper__DOT__dut__DOT__copper_pos_flat[16U] 
           >> 0x0000000fU);
    vlSelfRef.test_copper__DOT__dut__DOT__in_text_area 
        = ((IData)(test_copper__DOT__dut__DOT__visible) 
           & ((0x0028U <= (IData)(vlSelfRef.test_copper__DOT__dut__DOT__v_count)) 
              & (0x01b8U > (IData)(vlSelfRef.test_copper__DOT__dut__DOT__v_count))));
    vlSelfRef.test_copper__DOT__dut__DOT__tile_inst__DOT__td_din_a = 0U;
    vlSelfRef.test_copper__DOT__dut__DOT__tile_inst__DOT__td_addr_a = 0U;
    if ((0x00000010U & (IData)(vlSelfRef.test_copper__DOT__dut__DOT__tile_inst__DOT__dma_state))) {
        if (vlSelfRef.test_copper__DOT__dut__DOT__vgc_tile_we) {
            vlSelfRef.test_copper__DOT__dut__DOT__tile_inst__DOT__td_we_a = 1U;
            vlSelfRef.test_copper__DOT__dut__DOT__tile_inst__DOT__td_din_a 
                = test_copper__DOT__dut__DOT__tile_blt_wdata;
            vlSelfRef.test_copper__DOT__dut__DOT__tile_inst__DOT__td_addr_a 
                = test_copper__DOT__dut__DOT__tile_blt_addr;
        } else if (vlSelfRef.test_copper__DOT__dut__DOT__vgc_tile_re) {
            vlSelfRef.test_copper__DOT__dut__DOT__tile_inst__DOT__td_addr_a 
                = test_copper__DOT__dut__DOT__tile_blt_addr;
        }
        if (vlSelfRef.test_copper__DOT__dut__DOT__tile_inst__DOT__cmd_at_we) {
            vlSelfRef.test_copper__DOT__dut__DOT__tile_inst__DOT__at_we_a = 1U;
            vlSelfRef.test_copper__DOT__dut__DOT__tile_inst__DOT__at_din_a 
                = vlSelfRef.test_copper__DOT__dut__DOT__tile_inst__DOT__cmd_at_din;
            vlSelfRef.test_copper__DOT__dut__DOT__tile_inst__DOT__at_addr_a 
                = vlSelfRef.test_copper__DOT__dut__DOT__tile_inst__DOT__cmd_at_addr;
        }
        if (vlSelfRef.test_copper__DOT__dut__DOT__tile_inst__DOT__cmd_nt_we) {
            vlSelfRef.test_copper__DOT__dut__DOT__tile_inst__DOT__nt_we_a = 1U;
            vlSelfRef.test_copper__DOT__dut__DOT__tile_inst__DOT__nt_din_a 
                = vlSelfRef.test_copper__DOT__dut__DOT__tile_inst__DOT__cmd_nt_din;
            vlSelfRef.test_copper__DOT__dut__DOT__tile_inst__DOT__nt_addr_a 
                = vlSelfRef.test_copper__DOT__dut__DOT__tile_inst__DOT__cmd_nt_addr;
        }
    } else if ((8U & (IData)(vlSelfRef.test_copper__DOT__dut__DOT__tile_inst__DOT__dma_state))) {
        if ((4U & (IData)(vlSelfRef.test_copper__DOT__dut__DOT__tile_inst__DOT__dma_state))) {
            if ((2U & (IData)(vlSelfRef.test_copper__DOT__dut__DOT__tile_inst__DOT__dma_state))) {
                if ((1U & (IData)(vlSelfRef.test_copper__DOT__dut__DOT__tile_inst__DOT__dma_state))) {
                    if (vlSelfRef.test_copper__DOT__dut__DOT__vgc_tile_we) {
                        vlSelfRef.test_copper__DOT__dut__DOT__tile_inst__DOT__td_we_a = 1U;
                        vlSelfRef.test_copper__DOT__dut__DOT__tile_inst__DOT__td_din_a 
                            = test_copper__DOT__dut__DOT__tile_blt_wdata;
                        vlSelfRef.test_copper__DOT__dut__DOT__tile_inst__DOT__td_addr_a 
                            = test_copper__DOT__dut__DOT__tile_blt_addr;
                    } else if (vlSelfRef.test_copper__DOT__dut__DOT__vgc_tile_re) {
                        vlSelfRef.test_copper__DOT__dut__DOT__tile_inst__DOT__td_addr_a 
                            = test_copper__DOT__dut__DOT__tile_blt_addr;
                    }
                    if (vlSelfRef.test_copper__DOT__dut__DOT__tile_inst__DOT__cmd_at_we) {
                        vlSelfRef.test_copper__DOT__dut__DOT__tile_inst__DOT__at_we_a = 1U;
                        vlSelfRef.test_copper__DOT__dut__DOT__tile_inst__DOT__at_din_a 
                            = vlSelfRef.test_copper__DOT__dut__DOT__tile_inst__DOT__cmd_at_din;
                        vlSelfRef.test_copper__DOT__dut__DOT__tile_inst__DOT__at_addr_a 
                            = vlSelfRef.test_copper__DOT__dut__DOT__tile_inst__DOT__cmd_at_addr;
                    }
                    if (vlSelfRef.test_copper__DOT__dut__DOT__tile_inst__DOT__cmd_nt_we) {
                        vlSelfRef.test_copper__DOT__dut__DOT__tile_inst__DOT__nt_we_a = 1U;
                        vlSelfRef.test_copper__DOT__dut__DOT__tile_inst__DOT__nt_din_a 
                            = vlSelfRef.test_copper__DOT__dut__DOT__tile_inst__DOT__cmd_nt_din;
                        vlSelfRef.test_copper__DOT__dut__DOT__tile_inst__DOT__nt_addr_a 
                            = vlSelfRef.test_copper__DOT__dut__DOT__tile_inst__DOT__cmd_nt_addr;
                    }
                }
            } else {
                vlSelfRef.test_copper__DOT__dut__DOT__tile_inst__DOT__at_addr_a 
                    = vlSelfRef.test_copper__DOT__dut__DOT__tile_inst__DOT__cmd_at_addr;
                vlSelfRef.test_copper__DOT__dut__DOT__tile_inst__DOT__nt_addr_a 
                    = vlSelfRef.test_copper__DOT__dut__DOT__tile_inst__DOT__cmd_nt_addr;
            }
        } else {
            if ((1U & (~ ((IData)(vlSelfRef.test_copper__DOT__dut__DOT__tile_inst__DOT__dma_state) 
                          >> 1U)))) {
                if ((1U & (IData)(vlSelfRef.test_copper__DOT__dut__DOT__tile_inst__DOT__dma_state))) {
                    if ((0x0fa0U > (IData)(vlSelfRef.test_copper__DOT__dut__DOT__tile_inst__DOT__dma_fill_addr))) {
                        vlSelfRef.test_copper__DOT__dut__DOT__tile_inst__DOT__at_we_a = 1U;
                        vlSelfRef.test_copper__DOT__dut__DOT__tile_inst__DOT__at_din_a = 0U;
                        vlSelfRef.test_copper__DOT__dut__DOT__tile_inst__DOT__at_addr_a 
                            = vlSelfRef.test_copper__DOT__dut__DOT__tile_inst__DOT__dma_fill_addr;
                    }
                } else if (vlSelfRef.test_copper__DOT__dut__DOT__tile_inst__DOT__dma_data_valid) {
                    vlSelfRef.test_copper__DOT__dut__DOT__tile_inst__DOT__at_we_a = 1U;
                    vlSelfRef.test_copper__DOT__dut__DOT__tile_inst__DOT__at_din_a = 0U;
                    vlSelfRef.test_copper__DOT__dut__DOT__tile_inst__DOT__at_addr_a 
                        = (0x00000fffU & ((((IData)(0x000003e8U) 
                                            * (IData)(vlSelfRef.test_copper__DOT__dut__DOT__tile_inst__DOT__dma_nt)) 
                                           + ((IData)(0x0028U) 
                                              * (IData)(vlSelfRef.test_copper__DOT__dut__DOT__tile_inst__DOT__dma_col_idx))) 
                                          + (IData)(vlSelfRef.test_copper__DOT__dut__DOT__tile_inst__DOT__dma_col_row)));
                }
            }
            if ((2U & (IData)(vlSelfRef.test_copper__DOT__dut__DOT__tile_inst__DOT__dma_state))) {
                if ((1U & (IData)(vlSelfRef.test_copper__DOT__dut__DOT__tile_inst__DOT__dma_state))) {
                    if ((0x19U > (IData)(vlSelfRef.test_copper__DOT__dut__DOT__tile_inst__DOT__dma_col_idx))) {
                        vlSelfRef.test_copper__DOT__dut__DOT__tile_inst__DOT__nt_we_a = 1U;
                        vlSelfRef.test_copper__DOT__dut__DOT__tile_inst__DOT__nt_din_a 
                            = ((0x18U >= (IData)(vlSelfRef.test_copper__DOT__dut__DOT__tile_inst__DOT__dma_col_idx))
                                ? vlSelfRef.test_copper__DOT__dut__DOT__tile_inst__DOT__col_buffer
                               [vlSelfRef.test_copper__DOT__dut__DOT__tile_inst__DOT__dma_col_idx]
                                : 0U);
                        vlSelfRef.test_copper__DOT__dut__DOT__tile_inst__DOT__nt_addr_a 
                            = (0x00000fffU & ((((IData)(0x000003e8U) 
                                                * (IData)(vlSelfRef.test_copper__DOT__dut__DOT__tile_inst__DOT__dma_nt)) 
                                               + ((IData)(0x0028U) 
                                                  * (IData)(vlSelfRef.test_copper__DOT__dut__DOT__tile_inst__DOT__dma_col_idx))) 
                                              + (IData)(vlSelfRef.test_copper__DOT__dut__DOT__tile_inst__DOT__dma_col_row)));
                    }
                } else if ((0x03e8U > (IData)(vlSelfRef.test_copper__DOT__dut__DOT__tile_inst__DOT__dma_count))) {
                    vlSelfRef.test_copper__DOT__dut__DOT__tile_inst__DOT__nt_we_a = 1U;
                    vlSelfRef.test_copper__DOT__dut__DOT__tile_inst__DOT__nt_din_a 
                        = vlSelfRef.test_copper__DOT__dut__DOT__tile_inst__DOT__dma_fill_val;
                    vlSelfRef.test_copper__DOT__dut__DOT__tile_inst__DOT__nt_addr_a 
                        = (0x00000fffU & (((IData)(0x000003e8U) 
                                           * (IData)(vlSelfRef.test_copper__DOT__dut__DOT__tile_inst__DOT__dma_nt)) 
                                          + (IData)(vlSelfRef.test_copper__DOT__dut__DOT__tile_inst__DOT__dma_count)));
                }
            } else if ((1U & (IData)(vlSelfRef.test_copper__DOT__dut__DOT__tile_inst__DOT__dma_state))) {
                if ((0x0fa0U > (IData)(vlSelfRef.test_copper__DOT__dut__DOT__tile_inst__DOT__dma_fill_addr))) {
                    vlSelfRef.test_copper__DOT__dut__DOT__tile_inst__DOT__nt_we_a = 1U;
                    vlSelfRef.test_copper__DOT__dut__DOT__tile_inst__DOT__nt_din_a = 0U;
                    vlSelfRef.test_copper__DOT__dut__DOT__tile_inst__DOT__nt_addr_a 
                        = vlSelfRef.test_copper__DOT__dut__DOT__tile_inst__DOT__dma_fill_addr;
                }
            }
        }
    } else {
        if ((1U & (~ ((IData)(vlSelfRef.test_copper__DOT__dut__DOT__tile_inst__DOT__dma_state) 
                      >> 2U)))) {
            if ((1U & (~ ((IData)(vlSelfRef.test_copper__DOT__dut__DOT__tile_inst__DOT__dma_state) 
                          >> 1U)))) {
                if ((1U & (IData)(vlSelfRef.test_copper__DOT__dut__DOT__tile_inst__DOT__dma_state))) {
                    if (vlSelfRef.test_copper__DOT__dut__DOT__tile_inst__DOT__dma_data_valid) {
                        vlSelfRef.test_copper__DOT__dut__DOT__tile_inst__DOT__td_we_a = 1U;
                        vlSelfRef.test_copper__DOT__dut__DOT__tile_inst__DOT__td_din_a = 0U;
                        vlSelfRef.test_copper__DOT__dut__DOT__tile_inst__DOT__td_addr_a 
                            = (0x00007fffU & (IData)(vlSelfRef.test_copper__DOT__dut__DOT__tile_inst__DOT__dma_dst));
                    }
                } else if (vlSelfRef.test_copper__DOT__dut__DOT__vgc_tile_we) {
                    vlSelfRef.test_copper__DOT__dut__DOT__tile_inst__DOT__td_we_a = 1U;
                    vlSelfRef.test_copper__DOT__dut__DOT__tile_inst__DOT__td_din_a 
                        = test_copper__DOT__dut__DOT__tile_blt_wdata;
                    vlSelfRef.test_copper__DOT__dut__DOT__tile_inst__DOT__td_addr_a 
                        = test_copper__DOT__dut__DOT__tile_blt_addr;
                } else if (vlSelfRef.test_copper__DOT__dut__DOT__vgc_tile_re) {
                    vlSelfRef.test_copper__DOT__dut__DOT__tile_inst__DOT__td_addr_a 
                        = test_copper__DOT__dut__DOT__tile_blt_addr;
                }
            }
        }
        if ((4U & (IData)(vlSelfRef.test_copper__DOT__dut__DOT__tile_inst__DOT__dma_state))) {
            if ((2U & (IData)(vlSelfRef.test_copper__DOT__dut__DOT__tile_inst__DOT__dma_state))) {
                if ((1U & (IData)(vlSelfRef.test_copper__DOT__dut__DOT__tile_inst__DOT__dma_state))) {
                    if (vlSelfRef.test_copper__DOT__dut__DOT__tile_inst__DOT__dma_data_valid) {
                        vlSelfRef.test_copper__DOT__dut__DOT__tile_inst__DOT__at_we_a = 1U;
                        vlSelfRef.test_copper__DOT__dut__DOT__tile_inst__DOT__at_din_a = 0U;
                        vlSelfRef.test_copper__DOT__dut__DOT__tile_inst__DOT__at_addr_a 
                            = (0x00000fffU & ((((IData)(0x000003e8U) 
                                                * (IData)(vlSelfRef.test_copper__DOT__dut__DOT__tile_inst__DOT__dma_nt)) 
                                               + ((IData)(0x00000028U) 
                                                  * (IData)(vlSelfRef.test_copper__DOT__dut__DOT__tile_inst__DOT__dma_col_row))) 
                                              + (0x0000003fU 
                                                 & (IData)(vlSelfRef.test_copper__DOT__dut__DOT__tile_inst__DOT__dma_dst))));
                    }
                }
                if ((1U & (~ (IData)(vlSelfRef.test_copper__DOT__dut__DOT__tile_inst__DOT__dma_state)))) {
                    if (vlSelfRef.test_copper__DOT__dut__DOT__tile_inst__DOT__dma_data_valid) {
                        vlSelfRef.test_copper__DOT__dut__DOT__tile_inst__DOT__nt_we_a = 1U;
                        vlSelfRef.test_copper__DOT__dut__DOT__tile_inst__DOT__nt_din_a = 0U;
                        vlSelfRef.test_copper__DOT__dut__DOT__tile_inst__DOT__nt_addr_a 
                            = (0x00000fffU & ((((IData)(0x000003e8U) 
                                                * (IData)(vlSelfRef.test_copper__DOT__dut__DOT__tile_inst__DOT__dma_nt)) 
                                               + ((IData)(0x0028U) 
                                                  * (IData)(vlSelfRef.test_copper__DOT__dut__DOT__tile_inst__DOT__dma_col_idx))) 
                                              + (IData)(vlSelfRef.test_copper__DOT__dut__DOT__tile_inst__DOT__dma_col_row)));
                    }
                }
            } else if ((1U & (IData)(vlSelfRef.test_copper__DOT__dut__DOT__tile_inst__DOT__dma_state))) {
                if (vlSelfRef.test_copper__DOT__dut__DOT__tile_inst__DOT__dma_data_valid) {
                    vlSelfRef.test_copper__DOT__dut__DOT__tile_inst__DOT__nt_we_a = 1U;
                    vlSelfRef.test_copper__DOT__dut__DOT__tile_inst__DOT__nt_din_a = 0U;
                    vlSelfRef.test_copper__DOT__dut__DOT__tile_inst__DOT__nt_addr_a 
                        = (0x00000fffU & ((((IData)(0x000003e8U) 
                                            * (IData)(vlSelfRef.test_copper__DOT__dut__DOT__tile_inst__DOT__dma_nt)) 
                                           + ((IData)(0x00000028U) 
                                              * (IData)(vlSelfRef.test_copper__DOT__dut__DOT__tile_inst__DOT__dma_col_row))) 
                                          + (0x0000003fU 
                                             & (IData)(vlSelfRef.test_copper__DOT__dut__DOT__tile_inst__DOT__dma_dst))));
                }
            }
        } else if ((2U & (IData)(vlSelfRef.test_copper__DOT__dut__DOT__tile_inst__DOT__dma_state))) {
            if ((1U & (IData)(vlSelfRef.test_copper__DOT__dut__DOT__tile_inst__DOT__dma_state))) {
                if (vlSelfRef.test_copper__DOT__dut__DOT__tile_inst__DOT__dma_data_valid) {
                    vlSelfRef.test_copper__DOT__dut__DOT__tile_inst__DOT__at_we_a = 1U;
                    vlSelfRef.test_copper__DOT__dut__DOT__tile_inst__DOT__at_din_a = 0U;
                    vlSelfRef.test_copper__DOT__dut__DOT__tile_inst__DOT__at_addr_a 
                        = (0x00000fffU & (((IData)(0x000003e8U) 
                                           * (IData)(vlSelfRef.test_copper__DOT__dut__DOT__tile_inst__DOT__dma_nt)) 
                                          + (0x000003ffU 
                                             & (IData)(vlSelfRef.test_copper__DOT__dut__DOT__tile_inst__DOT__dma_dst))));
                }
            }
            if ((1U & (~ (IData)(vlSelfRef.test_copper__DOT__dut__DOT__tile_inst__DOT__dma_state)))) {
                if (vlSelfRef.test_copper__DOT__dut__DOT__tile_inst__DOT__dma_data_valid) {
                    vlSelfRef.test_copper__DOT__dut__DOT__tile_inst__DOT__nt_we_a = 1U;
                    vlSelfRef.test_copper__DOT__dut__DOT__tile_inst__DOT__nt_din_a = 0U;
                    vlSelfRef.test_copper__DOT__dut__DOT__tile_inst__DOT__nt_addr_a 
                        = (0x00000fffU & (((IData)(0x000003e8U) 
                                           * (IData)(vlSelfRef.test_copper__DOT__dut__DOT__tile_inst__DOT__dma_nt)) 
                                          + (0x000003ffU 
                                             & (IData)(vlSelfRef.test_copper__DOT__dut__DOT__tile_inst__DOT__dma_dst))));
                }
            }
        } else if ((1U & (~ (IData)(vlSelfRef.test_copper__DOT__dut__DOT__tile_inst__DOT__dma_state)))) {
            if (vlSelfRef.test_copper__DOT__dut__DOT__tile_inst__DOT__cmd_at_we) {
                vlSelfRef.test_copper__DOT__dut__DOT__tile_inst__DOT__at_we_a = 1U;
                vlSelfRef.test_copper__DOT__dut__DOT__tile_inst__DOT__at_din_a 
                    = vlSelfRef.test_copper__DOT__dut__DOT__tile_inst__DOT__cmd_at_din;
                vlSelfRef.test_copper__DOT__dut__DOT__tile_inst__DOT__at_addr_a 
                    = vlSelfRef.test_copper__DOT__dut__DOT__tile_inst__DOT__cmd_at_addr;
            }
            if (vlSelfRef.test_copper__DOT__dut__DOT__tile_inst__DOT__cmd_nt_we) {
                vlSelfRef.test_copper__DOT__dut__DOT__tile_inst__DOT__nt_we_a = 1U;
                vlSelfRef.test_copper__DOT__dut__DOT__tile_inst__DOT__nt_din_a 
                    = vlSelfRef.test_copper__DOT__dut__DOT__tile_inst__DOT__cmd_nt_din;
                vlSelfRef.test_copper__DOT__dut__DOT__tile_inst__DOT__nt_addr_a 
                    = vlSelfRef.test_copper__DOT__dut__DOT__tile_inst__DOT__cmd_nt_addr;
            }
        }
    }
    vlSelfRef.test_copper__DOT__dut__DOT__char_a_addr 
        = (0x000007ffU & ((IData)(vlSelfRef.test_copper__DOT__dut__DOT__cmd_char_we)
                           ? (IData)(vlSelfRef.test_copper__DOT__dut__DOT__cmd_char_addr)
                           : (((IData)(__VdfgRegularize_hebeb780c_0_0) 
                               & (IData)(vlSelfRef.test_copper__DOT__dut__DOT__char_ram_sel))
                               ? ((IData)(vlSelfRef.test_copper__DOT__cpu_addr) 
                                  - (IData)(0x0200U))
                               : ((IData)(vlSelfRef.test_copper__DOT__dut__DOT__dbg_char_sel)
                                   ? ((IData)(vlSelfRef.test_copper__DOT__dut__DOT__dbg_addr) 
                                      - (IData)(0x0200U))
                                   : (IData)(vlSelfRef.test_copper__DOT__dut__DOT__cmd_char_addr)))));
    vlSelfRef.test_copper__DOT__dut__DOT__color_a_addr 
        = (0x000007ffU & ((IData)(vlSelfRef.test_copper__DOT__dut__DOT__cmd_color_we)
                           ? (IData)(vlSelfRef.test_copper__DOT__dut__DOT__cmd_color_addr)
                           : (((IData)(__VdfgRegularize_hebeb780c_0_0) 
                               & (IData)(vlSelfRef.test_copper__DOT__dut__DOT__color_ram_sel))
                               ? ((IData)(vlSelfRef.test_copper__DOT__cpu_addr) 
                                  - (IData)(0x01d0U))
                               : ((IData)(vlSelfRef.test_copper__DOT__dut__DOT__dbg_color_sel)
                                   ? ((IData)(vlSelfRef.test_copper__DOT__dut__DOT__dbg_addr) 
                                      - (IData)(0x01d0U))
                                   : (IData)(vlSelfRef.test_copper__DOT__dut__DOT__cmd_color_addr)))));
}

VL_ATTR_COLD void Vtest_copper___024root___eval_stl(Vtest_copper___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vtest_copper___024root___eval_stl\n"); );
    Vtest_copper__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
    // Body
    if ((1ULL & vlSelfRef.__VstlTriggered[0U])) {
        Vtest_copper___024root___stl_sequent__TOP__0(vlSelf);
    }
}

VL_ATTR_COLD bool Vtest_copper___024root___eval_phase__stl(Vtest_copper___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vtest_copper___024root___eval_phase__stl\n"); );
    Vtest_copper__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
    // Locals
    CData/*0:0*/ __VstlExecute;
    // Body
    Vtest_copper___024root___eval_triggers_vec__stl(vlSelf);
#ifdef VL_DEBUG
    if (VL_UNLIKELY(vlSymsp->_vm_contextp__->debug())) {
        Vtest_copper___024root___dump_triggers__stl(vlSelfRef.__VstlTriggered, "stl"s);
    }
#endif
    __VstlExecute = Vtest_copper___024root___trigger_anySet__stl(vlSelfRef.__VstlTriggered);
    if (__VstlExecute) {
        Vtest_copper___024root___eval_stl(vlSelf);
    }
    return (__VstlExecute);
}

bool Vtest_copper___024root___trigger_anySet__act(const VlUnpacked<QData/*63:0*/, 1> &in);

#ifdef VL_DEBUG
VL_ATTR_COLD void Vtest_copper___024root___dump_triggers__act(const VlUnpacked<QData/*63:0*/, 1> &triggers, const std::string &tag) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vtest_copper___024root___dump_triggers__act\n"); );
    // Body
    if ((1U & (~ (IData)(Vtest_copper___024root___trigger_anySet__act(triggers))))) {
        VL_DBG_MSGS("         No '" + tag + "' region triggers active\n");
    }
    if ((1U & (IData)(triggers[0U]))) {
        VL_DBG_MSGS("         '" + tag + "' region trigger index 0 is active: @(posedge test_copper.clk)\n");
    }
    if ((1U & (IData)((triggers[0U] >> 1U)))) {
        VL_DBG_MSGS("         '" + tag + "' region trigger index 1 is active: @([true] __VdlySched.awaitingCurrentTime())\n");
    }
}
#endif  // VL_DEBUG

VL_ATTR_COLD void Vtest_copper___024root___ctor_var_reset(Vtest_copper___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vtest_copper___024root___ctor_var_reset\n"); );
    Vtest_copper__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
    // Body
    const uint64_t __VscopeHash = VL_MURMUR64_HASH(vlSelf->vlNamep);
    vlSelf->test_copper__DOT__rst = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 3220328174944393653ull);
    vlSelf->test_copper__DOT__cpu_ce = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 10030456197907170482ull);
    vlSelf->test_copper__DOT__cpu_addr = VL_SCOPED_RAND_RESET_I(16, __VscopeHash, 9316936856670518654ull);
    vlSelf->test_copper__DOT__cpu_wdata = VL_SCOPED_RAND_RESET_I(8, __VscopeHash, 589285384273775011ull);
    vlSelf->test_copper__DOT__cpu_we = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 5784950544297097834ull);
    vlSelf->test_copper__DOT__cpu_re = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 18082961623937405796ull);
    vlSelf->test_copper__DOT__dut__DOT__dbg_addr = VL_SCOPED_RAND_RESET_I(16, __VscopeHash, 2021941465679801241ull);
    vlSelf->test_copper__DOT__dut__DOT__h_count = VL_SCOPED_RAND_RESET_I(10, __VscopeHash, 7216441447748022369ull);
    vlSelf->test_copper__DOT__dut__DOT__v_count = VL_SCOPED_RAND_RESET_I(10, __VscopeHash, 4696373810696311854ull);
    vlSelf->test_copper__DOT__dut__DOT__in_text_area = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 5744053716666641270ull);
    vlSelf->test_copper__DOT__dut__DOT__gfx_y = VL_SCOPED_RAND_RESET_I(8, __VscopeHash, 3885433891873081022ull);
    vlSelf->test_copper__DOT__dut__DOT__scroll_offset = VL_SCOPED_RAND_RESET_I(5, __VscopeHash, 10635943187958388319ull);
    vlSelf->test_copper__DOT__dut__DOT__char_a_addr = VL_SCOPED_RAND_RESET_I(11, __VscopeHash, 6100542443737260477ull);
    vlSelf->test_copper__DOT__dut__DOT__char_a_dout = VL_SCOPED_RAND_RESET_I(8, __VscopeHash, 9675902932182383183ull);
    vlSelf->test_copper__DOT__dut__DOT__color_a_addr = VL_SCOPED_RAND_RESET_I(11, __VscopeHash, 11065401614544549569ull);
    vlSelf->test_copper__DOT__dut__DOT__color_a_dout = VL_SCOPED_RAND_RESET_I(8, __VscopeHash, 3164256256628391841ull);
    vlSelf->test_copper__DOT__dut__DOT__font_a_addr = VL_SCOPED_RAND_RESET_I(11, __VscopeHash, 752917738331099669ull);
    vlSelf->test_copper__DOT__dut__DOT__font_a_dout = VL_SCOPED_RAND_RESET_I(8, __VscopeHash, 10978274164303910073ull);
    vlSelf->test_copper__DOT__dut__DOT__gfx_a_addr = VL_SCOPED_RAND_RESET_I(16, __VscopeHash, 3194563897641805524ull);
    vlSelf->test_copper__DOT__dut__DOT__gfx_a_din = VL_SCOPED_RAND_RESET_I(4, __VscopeHash, 10603868624901363934ull);
    vlSelf->test_copper__DOT__dut__DOT__gfx_a_we = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 4352615397164598020ull);
    vlSelf->test_copper__DOT__dut__DOT__gfx_a_dout = VL_SCOPED_RAND_RESET_I(4, __VscopeHash, 15135909423863981800ull);
    VL_SCOPED_RAND_RESET_W(512, vlSelf->test_copper__DOT__dut__DOT__fio_name_flat, __VscopeHash, 6380473588278998555ull);
    vlSelf->test_copper__DOT__dut__DOT____Vcellinp__artist_inst__cmd_x1 = 0;
    vlSelf->test_copper__DOT__dut__DOT____Vcellinp__artist_inst__cmd_x0 = 0;
    vlSelf->test_copper__DOT__dut__DOT__spr_a_addr = VL_SCOPED_RAND_RESET_I(11, __VscopeHash, 5368051192433099774ull);
    vlSelf->test_copper__DOT__dut__DOT__spr_a_dout = VL_SCOPED_RAND_RESET_I(8, __VscopeHash, 15658985396578882694ull);
    for (int __Vi0 = 0; __Vi0 < 32; ++__Vi0) {
        vlSelf->test_copper__DOT__dut__DOT__copper_pos[__Vi0] = VL_SCOPED_RAND_RESET_I(17, __VscopeHash, 2014232440108342971ull);
    }
    for (int __Vi0 = 0; __Vi0 < 32; ++__Vi0) {
        vlSelf->test_copper__DOT__dut__DOT__copper_reg[__Vi0] = VL_SCOPED_RAND_RESET_I(8, __VscopeHash, 17810384347845286217ull);
    }
    for (int __Vi0 = 0; __Vi0 < 32; ++__Vi0) {
        vlSelf->test_copper__DOT__dut__DOT__copper_val[__Vi0] = VL_SCOPED_RAND_RESET_I(8, __VscopeHash, 10582212714271370082ull);
    }
    vlSelf->test_copper__DOT__dut__DOT__copper_count = VL_SCOPED_RAND_RESET_I(9, __VscopeHash, 7628715402836409123ull);
    vlSelf->test_copper__DOT__dut__DOT__copper_index = VL_SCOPED_RAND_RESET_I(9, __VscopeHash, 14734484031848620392ull);
    vlSelf->test_copper__DOT__dut__DOT__copper_enabled = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 17096949017282852251ull);
    for (int __Vi0 = 0; __Vi0 < 4096; ++__Vi0) {
        vlSelf->test_copper__DOT__dut__DOT__copper_list_pos[__Vi0] = VL_SCOPED_RAND_RESET_I(17, __VscopeHash, 7856130077325767245ull);
    }
    for (int __Vi0 = 0; __Vi0 < 4096; ++__Vi0) {
        vlSelf->test_copper__DOT__dut__DOT__copper_list_reg[__Vi0] = VL_SCOPED_RAND_RESET_I(8, __VscopeHash, 7620634224743330394ull);
    }
    for (int __Vi0 = 0; __Vi0 < 4096; ++__Vi0) {
        vlSelf->test_copper__DOT__dut__DOT__copper_list_val[__Vi0] = VL_SCOPED_RAND_RESET_I(8, __VscopeHash, 5556129366427879444ull);
    }
    for (int __Vi0 = 0; __Vi0 < 128; ++__Vi0) {
        vlSelf->test_copper__DOT__dut__DOT__copper_list_count[__Vi0] = VL_SCOPED_RAND_RESET_I(9, __VscopeHash, 3525229023518149782ull);
    }
    vlSelf->test_copper__DOT__dut__DOT__copper_target_list = VL_SCOPED_RAND_RESET_I(7, __VscopeHash, 13997574982133342964ull);
    vlSelf->test_copper__DOT__dut__DOT__copper_active_list = VL_SCOPED_RAND_RESET_I(7, __VscopeHash, 9819336285435178159ull);
    vlSelf->test_copper__DOT__dut__DOT__copper_pending_list = VL_SCOPED_RAND_RESET_I(7, __VscopeHash, 5926752959387877067ull);
    vlSelf->test_copper__DOT__dut__DOT__copper_loading = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 14935788763711282523ull);
    vlSelf->test_copper__DOT__dut__DOT__copper_load_idx = VL_SCOPED_RAND_RESET_I(9, __VscopeHash, 14638787596141190283ull);
    vlSelf->test_copper__DOT__dut__DOT__copper_load_src = VL_SCOPED_RAND_RESET_I(7, __VscopeHash, 9912331714364435789ull);
    vlSelf->test_copper__DOT__dut__DOT__copper_fire = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 12347944471920492251ull);
    vlSelf->test_copper__DOT__dut__DOT__copper_fire_reg = VL_SCOPED_RAND_RESET_I(8, __VscopeHash, 2182295594564956421ull);
    vlSelf->test_copper__DOT__dut__DOT__copper_fire_val = VL_SCOPED_RAND_RESET_I(8, __VscopeHash, 5959876188877262138ull);
    VL_SCOPED_RAND_RESET_W(544, vlSelf->test_copper__DOT__dut__DOT__copper_pos_flat, __VscopeHash, 17939264818418873789ull);
    VL_SCOPED_RAND_RESET_W(256, vlSelf->test_copper__DOT__dut__DOT__copper_reg_flat, __VscopeHash, 5139655927387516259ull);
    VL_SCOPED_RAND_RESET_W(256, vlSelf->test_copper__DOT__dut__DOT__copper_val_flat, __VscopeHash, 12042428803055294119ull);
    for (int __Vi0 = 0; __Vi0 < 32; ++__Vi0) {
        vlSelf->test_copper__DOT__dut__DOT__regs[__Vi0] = VL_SCOPED_RAND_RESET_I(8, __VscopeHash, 17690388050209455243ull);
    }
    vlSelf->test_copper__DOT__dut__DOT__cursor_x = VL_SCOPED_RAND_RESET_I(7, __VscopeHash, 2519170469809435915ull);
    vlSelf->test_copper__DOT__dut__DOT__cursor_y = VL_SCOPED_RAND_RESET_I(5, __VscopeHash, 9047941663757385254ull);
    vlSelf->test_copper__DOT__dut__DOT__fg_color = VL_SCOPED_RAND_RESET_I(4, __VscopeHash, 2825136248217220091ull);
    vlSelf->test_copper__DOT__dut__DOT__bg_color = VL_SCOPED_RAND_RESET_I(4, __VscopeHash, 12445762138183102203ull);
    vlSelf->test_copper__DOT__dut__DOT__gfx_color = VL_SCOPED_RAND_RESET_I(4, __VscopeHash, 12323294831705162979ull);
    vlSelf->test_copper__DOT__dut__DOT__scroll_pending = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 9545033235304614756ull);
    vlSelf->test_copper__DOT__dut__DOT__scroll_col = VL_SCOPED_RAND_RESET_I(7, __VscopeHash, 7704344420055967555ull);
    for (int __Vi0 = 0; __Vi0 < 64; ++__Vi0) {
        vlSelf->test_copper__DOT__dut__DOT__fio_name[__Vi0] = VL_SCOPED_RAND_RESET_I(8, __VscopeHash, 11357702227275443006ull);
    }
    vlSelf->test_copper__DOT__dut__DOT__fio_name_len = VL_SCOPED_RAND_RESET_I(6, __VscopeHash, 13675588151750666429ull);
    vlSelf->test_copper__DOT__dut__DOT__cmd_busy = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 4899988012400879998ull);
    vlSelf->test_copper__DOT__dut__DOT__cmd_op = VL_SCOPED_RAND_RESET_I(8, __VscopeHash, 9326636906848971133ull);
    vlSelf->test_copper__DOT__dut__DOT__cmd_x = VL_SCOPED_RAND_RESET_I(10, __VscopeHash, 606352996796768631ull);
    vlSelf->test_copper__DOT__dut__DOT__cmd_y = VL_SCOPED_RAND_RESET_I(10, __VscopeHash, 10331684634454540012ull);
    vlSelf->test_copper__DOT__dut__DOT__cmd_cx = VL_SCOPED_RAND_RESET_I(9, __VscopeHash, 3490403196008152344ull);
    vlSelf->test_copper__DOT__dut__DOT__cmd_cy = VL_SCOPED_RAND_RESET_I(8, __VscopeHash, 12634668548361215657ull);
    vlSelf->test_copper__DOT__dut__DOT__sprrow_count = VL_SCOPED_RAND_RESET_I(3, __VscopeHash, 3086638371539313446ull);
    for (int __Vi0 = 0; __Vi0 < 8; ++__Vi0) {
        vlSelf->test_copper__DOT__dut__DOT__sprrow_data[__Vi0] = VL_SCOPED_RAND_RESET_I(8, __VscopeHash, 7239174997806987873ull);
    }
    vlSelf->test_copper__DOT__dut__DOT__sprrow_spr = VL_SCOPED_RAND_RESET_I(4, __VscopeHash, 13740376409268560441ull);
    vlSelf->test_copper__DOT__dut__DOT__sprrow_row = VL_SCOPED_RAND_RESET_I(4, __VscopeHash, 16533815022002725089ull);
    vlSelf->test_copper__DOT__dut__DOT__sprcopy_phase = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 13179785587302472935ull);
    vlSelf->test_copper__DOT__dut__DOT__memread_pending = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 4768720239996053954ull);
    vlSelf->test_copper__DOT__dut__DOT__memread_space = VL_SCOPED_RAND_RESET_I(3, __VscopeHash, 7530792377868656679ull);
    vlSelf->test_copper__DOT__dut__DOT__cmd_char_addr = VL_SCOPED_RAND_RESET_I(11, __VscopeHash, 8513720849465103843ull);
    vlSelf->test_copper__DOT__dut__DOT__cmd_char_din = VL_SCOPED_RAND_RESET_I(8, __VscopeHash, 16332685835425431595ull);
    vlSelf->test_copper__DOT__dut__DOT__cmd_char_we = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 2433197663529300273ull);
    vlSelf->test_copper__DOT__dut__DOT__cmd_color_addr = VL_SCOPED_RAND_RESET_I(11, __VscopeHash, 11364395475520138265ull);
    vlSelf->test_copper__DOT__dut__DOT__cmd_color_din = VL_SCOPED_RAND_RESET_I(8, __VscopeHash, 2634826498305352784ull);
    vlSelf->test_copper__DOT__dut__DOT__cmd_color_we = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 5765714095091056255ull);
    vlSelf->test_copper__DOT__dut__DOT__cmd_gfx_addr = VL_SCOPED_RAND_RESET_I(16, __VscopeHash, 17107102411494033559ull);
    vlSelf->test_copper__DOT__dut__DOT__cmd_gfx_din = VL_SCOPED_RAND_RESET_I(4, __VscopeHash, 2286859205368163956ull);
    vlSelf->test_copper__DOT__dut__DOT__cmd_gfx_we = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 57841613094452915ull);
    vlSelf->test_copper__DOT__dut__DOT__cmd_gfx_re = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 16101857187128758602ull);
    vlSelf->test_copper__DOT__dut__DOT__cmd_spr_addr = VL_SCOPED_RAND_RESET_I(11, __VscopeHash, 12057813969125199072ull);
    vlSelf->test_copper__DOT__dut__DOT__cmd_spr_din = VL_SCOPED_RAND_RESET_I(8, __VscopeHash, 17818681510249820738ull);
    vlSelf->test_copper__DOT__dut__DOT__cmd_spr_we = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 12835661880898617266ull);
    vlSelf->test_copper__DOT__dut__DOT__cmd_spr_re = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 15291753003816204887ull);
    vlSelf->test_copper__DOT__dut__DOT__artist_cmd_valid = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 3914694911356015616ull);
    vlSelf->test_copper__DOT__dut__DOT__artist_cmd_code = VL_SCOPED_RAND_RESET_I(8, __VscopeHash, 11371276323544280904ull);
    vlSelf->test_copper__DOT__dut__DOT__artist_gfx_addr = VL_SCOPED_RAND_RESET_I(16, __VscopeHash, 15681944047634453331ull);
    vlSelf->test_copper__DOT__dut__DOT__artist_gfx_wdata = VL_SCOPED_RAND_RESET_I(4, __VscopeHash, 8826649266435326551ull);
    vlSelf->test_copper__DOT__dut__DOT__artist_gfx_we = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 12302424610306194762ull);
    vlSelf->test_copper__DOT__dut__DOT__artist_gfx_raddr = VL_SCOPED_RAND_RESET_I(16, __VscopeHash, 8972455260642083944ull);
    vlSelf->test_copper__DOT__dut__DOT__artist_gfx_re = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 17059425491611469964ull);
    vlSelf->test_copper__DOT__dut__DOT__artist_font_addr = VL_SCOPED_RAND_RESET_I(11, __VscopeHash, 16095162311870181138ull);
    vlSelf->test_copper__DOT__dut__DOT__artist_font_re = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 7350973143948394262ull);
    vlSelf->test_copper__DOT__dut__DOT__artist_busy = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 4611173624332253144ull);
    vlSelf->test_copper__DOT__dut__DOT__vgc_reg_sel = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 14302511389162822434ull);
    vlSelf->test_copper__DOT__dut__DOT__spr_reg_sel = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 15413643276467860329ull);
    vlSelf->test_copper__DOT__dut__DOT__char_ram_sel = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 12113781511749272563ull);
    vlSelf->test_copper__DOT__dut__DOT__color_ram_sel = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 5459598589104692060ull);
    vlSelf->test_copper__DOT__dut__DOT__tile_reg_sel = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 4081917905918527860ull);
    vlSelf->test_copper__DOT__dut__DOT__dbg_char_sel = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 10600472236660282111ull);
    vlSelf->test_copper__DOT__dut__DOT__dbg_color_sel = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 2422581347626739966ull);
    vlSelf->test_copper__DOT__dut__DOT__tile_blt_rdata = VL_SCOPED_RAND_RESET_I(8, __VscopeHash, 6061754416887648274ull);
    vlSelf->test_copper__DOT__dut__DOT__vgc_tile_addr = VL_SCOPED_RAND_RESET_I(15, __VscopeHash, 9293776801307453816ull);
    vlSelf->test_copper__DOT__dut__DOT__vgc_tile_wdata = VL_SCOPED_RAND_RESET_I(8, __VscopeHash, 10973792879876321455ull);
    vlSelf->test_copper__DOT__dut__DOT__vgc_tile_we = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 1266753086169141246ull);
    vlSelf->test_copper__DOT__dut__DOT__vgc_tile_re = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 308383320076005255ull);
    vlSelf->test_copper__DOT__dut__DOT____Vcellinp__tile_inst__cpu_we = 0;
    vlSelf->test_copper__DOT__dut__DOT__unnamedblk14__DOT__i = 0;
    vlSelf->test_copper__DOT__dut__DOT__text_inst__DOT__char_mem__DOT____Vlvbound_hcad4ec09__0 = 0;
    vlSelf->test_copper__DOT__dut__DOT__text_inst__DOT__char_mem__DOT____Vlvbound_h3482bd38__0 = 0;
    for (int __Vi0 = 0; __Vi0 < 2000; ++__Vi0) {
        vlSelf->test_copper__DOT__dut__DOT__text_inst__DOT__char_mem__DOT__mem[__Vi0] = VL_SCOPED_RAND_RESET_I(8, __VscopeHash, 9140945607508937467ull);
    }
    vlSelf->test_copper__DOT__dut__DOT__text_inst__DOT__color_mem__DOT____Vlvbound_hcad4ec09__0 = 0;
    vlSelf->test_copper__DOT__dut__DOT__text_inst__DOT__color_mem__DOT____Vlvbound_h3482bd38__0 = 0;
    for (int __Vi0 = 0; __Vi0 < 2000; ++__Vi0) {
        vlSelf->test_copper__DOT__dut__DOT__text_inst__DOT__color_mem__DOT__mem[__Vi0] = VL_SCOPED_RAND_RESET_I(8, __VscopeHash, 17466513903581335510ull);
    }
    for (int __Vi0 = 0; __Vi0 < 2048; ++__Vi0) {
        vlSelf->test_copper__DOT__dut__DOT__text_inst__DOT__font_mem__DOT__mem[__Vi0] = VL_SCOPED_RAND_RESET_I(8, __VscopeHash, 6554430585446622694ull);
    }
    vlSelf->test_copper__DOT__dut__DOT__gfx_inst__DOT__gfx_mem__DOT____Vlvbound_h0a4ce44c__0 = 0;
    vlSelf->test_copper__DOT__dut__DOT__gfx_inst__DOT__gfx_mem__DOT____Vlvbound_h75f189da__0 = 0;
    for (int __Vi0 = 0; __Vi0 < 64000; ++__Vi0) {
        vlSelf->test_copper__DOT__dut__DOT__gfx_inst__DOT__gfx_mem__DOT__mem[__Vi0] = VL_SCOPED_RAND_RESET_I(4, __VscopeHash, 12372911112575763166ull);
    }
    vlSelf->test_copper__DOT__dut__DOT__artist_inst__DOT__op = VL_SCOPED_RAND_RESET_I(8, __VscopeHash, 2979434108427713910ull);
    vlSelf->test_copper__DOT__dut__DOT__artist_inst__DOT__x = VL_SCOPED_RAND_RESET_I(10, __VscopeHash, 17205327370512379030ull);
    vlSelf->test_copper__DOT__dut__DOT__artist_inst__DOT__y = VL_SCOPED_RAND_RESET_I(10, __VscopeHash, 5004454681682890831ull);
    vlSelf->test_copper__DOT__dut__DOT__artist_inst__DOT__x2 = VL_SCOPED_RAND_RESET_I(10, __VscopeHash, 12448468389140939514ull);
    vlSelf->test_copper__DOT__dut__DOT__artist_inst__DOT__y2 = VL_SCOPED_RAND_RESET_I(10, __VscopeHash, 4835098327159463094ull);
    vlSelf->test_copper__DOT__dut__DOT__artist_inst__DOT__dx = VL_SCOPED_RAND_RESET_I(10, __VscopeHash, 17852312566587122801ull);
    vlSelf->test_copper__DOT__dut__DOT__artist_inst__DOT__dy = VL_SCOPED_RAND_RESET_I(10, __VscopeHash, 10641934509529023303ull);
    vlSelf->test_copper__DOT__dut__DOT__artist_inst__DOT__err = VL_SCOPED_RAND_RESET_I(10, __VscopeHash, 17601989122406018537ull);
    vlSelf->test_copper__DOT__dut__DOT__artist_inst__DOT__sx = VL_SCOPED_RAND_RESET_I(10, __VscopeHash, 3749575555240488545ull);
    vlSelf->test_copper__DOT__dut__DOT__artist_inst__DOT__sy = VL_SCOPED_RAND_RESET_I(10, __VscopeHash, 4253182537157757078ull);
    vlSelf->test_copper__DOT__dut__DOT__artist_inst__DOT__cx = VL_SCOPED_RAND_RESET_I(9, __VscopeHash, 12163832289283150551ull);
    vlSelf->test_copper__DOT__dut__DOT__artist_inst__DOT__cy = VL_SCOPED_RAND_RESET_I(8, __VscopeHash, 17263487537134832377ull);
    vlSelf->test_copper__DOT__dut__DOT__artist_inst__DOT__color = VL_SCOPED_RAND_RESET_I(4, __VscopeHash, 10822314882160832718ull);
    vlSelf->test_copper__DOT__dut__DOT__artist_inst__DOT__octant_count = VL_SCOPED_RAND_RESET_I(3, __VscopeHash, 12851648582776266398ull);
    vlSelf->test_copper__DOT__dut__DOT__artist_inst__DOT__fs_a_addr = VL_SCOPED_RAND_RESET_I(9, __VscopeHash, 7190896121896737257ull);
    vlSelf->test_copper__DOT__dut__DOT__artist_inst__DOT__fs_a_din = VL_SCOPED_RAND_RESET_I(17, __VscopeHash, 15238677622231269669ull);
    vlSelf->test_copper__DOT__dut__DOT__artist_inst__DOT__fs_a_we = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 14964958684729808767ull);
    vlSelf->test_copper__DOT__dut__DOT__artist_inst__DOT__fs_b_addr = VL_SCOPED_RAND_RESET_I(9, __VscopeHash, 14669001694212297416ull);
    vlSelf->test_copper__DOT__dut__DOT__artist_inst__DOT__fs_b_dout = VL_SCOPED_RAND_RESET_I(17, __VscopeHash, 11903750321083761933ull);
    vlSelf->test_copper__DOT__dut__DOT__artist_inst__DOT__fill_sp = VL_SCOPED_RAND_RESET_I(10, __VscopeHash, 7618329134244301295ull);
    vlSelf->test_copper__DOT__dut__DOT__artist_inst__DOT__fill_target = VL_SCOPED_RAND_RESET_I(4, __VscopeHash, 6655640320263912846ull);
    vlSelf->test_copper__DOT__dut__DOT__artist_inst__DOT__paint_phase = VL_SCOPED_RAND_RESET_I(3, __VscopeHash, 5946656241416295255ull);
    vlSelf->test_copper__DOT__dut__DOT__artist_inst__DOT__paint_init = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 16491265321618416010ull);
    vlSelf->test_copper__DOT__dut__DOT__artist_inst__DOT__push_count = VL_SCOPED_RAND_RESET_I(2, __VscopeHash, 16809595861806198273ull);
    for (int __Vi0 = 0; __Vi0 < 4; ++__Vi0) {
        vlSelf->test_copper__DOT__dut__DOT__artist_inst__DOT__push_neighbors[__Vi0] = VL_SCOPED_RAND_RESET_I(17, __VscopeHash, 6040681011899840221ull);
    }
    vlSelf->test_copper__DOT__dut__DOT__artist_inst__DOT__gt_char_idx = VL_SCOPED_RAND_RESET_I(6, __VscopeHash, 16992771402891534093ull);
    vlSelf->test_copper__DOT__dut__DOT__artist_inst__DOT__gt_len = VL_SCOPED_RAND_RESET_I(6, __VscopeHash, 13343496562213350576ull);
    vlSelf->test_copper__DOT__dut__DOT__artist_inst__DOT__gt_font_row = VL_SCOPED_RAND_RESET_I(3, __VscopeHash, 10191829535931254011ull);
    vlSelf->test_copper__DOT__dut__DOT__artist_inst__DOT__gt_font_col = VL_SCOPED_RAND_RESET_I(3, __VscopeHash, 4568671208948546975ull);
    vlSelf->test_copper__DOT__dut__DOT__artist_inst__DOT__gt_scale = VL_SCOPED_RAND_RESET_I(8, __VscopeHash, 15181742536395289816ull);
    vlSelf->test_copper__DOT__dut__DOT__artist_inst__DOT__gt_scale_x = VL_SCOPED_RAND_RESET_I(8, __VscopeHash, 4190119269769016166ull);
    vlSelf->test_copper__DOT__dut__DOT__artist_inst__DOT__gt_scale_y = VL_SCOPED_RAND_RESET_I(8, __VscopeHash, 2164051508304846259ull);
    vlSelf->test_copper__DOT__dut__DOT__artist_inst__DOT__gt_pen_x = VL_SCOPED_RAND_RESET_I(9, __VscopeHash, 6237581228890904949ull);
    vlSelf->test_copper__DOT__dut__DOT__artist_inst__DOT__gt_start_y = VL_SCOPED_RAND_RESET_I(8, __VscopeHash, 8530887362869501805ull);
    vlSelf->test_copper__DOT__dut__DOT__artist_inst__DOT__gt_cur_char = VL_SCOPED_RAND_RESET_I(8, __VscopeHash, 168818030939312244ull);
    vlSelf->test_copper__DOT__dut__DOT__artist_inst__DOT__gt_font_byte = VL_SCOPED_RAND_RESET_I(8, __VscopeHash, 13759658573355152973ull);
    vlSelf->test_copper__DOT__dut__DOT__artist_inst__DOT__gt_font_pending = VL_SCOPED_RAND_RESET_I(2, __VscopeHash, 5601322833229706774ull);
    for (int __Vi0 = 0; __Vi0 < 512; ++__Vi0) {
        vlSelf->test_copper__DOT__dut__DOT__artist_inst__DOT__fill_stack_mem__DOT__mem[__Vi0] = VL_SCOPED_RAND_RESET_I(17, __VscopeHash, 15727253565108869847ull);
    }
    for (int __Vi0 = 0; __Vi0 < 2048; ++__Vi0) {
        vlSelf->test_copper__DOT__dut__DOT__sprite_inst__DOT__spr_mem__DOT__mem[__Vi0] = VL_SCOPED_RAND_RESET_I(8, __VscopeHash, 16648293086874338898ull);
    }
    for (int __Vi0 = 0; __Vi0 < 32; ++__Vi0) {
        vlSelf->test_copper__DOT__dut__DOT__copper_inst__DOT__copper_pos[__Vi0] = VL_SCOPED_RAND_RESET_I(17, __VscopeHash, 12469280861674226909ull);
    }
    for (int __Vi0 = 0; __Vi0 < 32; ++__Vi0) {
        vlSelf->test_copper__DOT__dut__DOT__copper_inst__DOT__copper_reg[__Vi0] = VL_SCOPED_RAND_RESET_I(8, __VscopeHash, 6706706454849015853ull);
    }
    for (int __Vi0 = 0; __Vi0 < 32; ++__Vi0) {
        vlSelf->test_copper__DOT__dut__DOT__copper_inst__DOT__copper_val[__Vi0] = VL_SCOPED_RAND_RESET_I(8, __VscopeHash, 9315645929101882767ull);
    }
    vlSelf->test_copper__DOT__dut__DOT__tile_inst__DOT____Vlvbound_ha768cd16__0 = 0;
    vlSelf->test_copper__DOT__dut__DOT__tile_inst__DOT____Vlvbound_h67f0dfac__0 = 0;
    vlSelf->test_copper__DOT__dut__DOT__tile_inst__DOT____Vlvbound_hbbfa6b2d__0 = 0;
    for (int __Vi0 = 0; __Vi0 < 32; ++__Vi0) {
        vlSelf->test_copper__DOT__dut__DOT__tile_inst__DOT__tregs[__Vi0] = VL_SCOPED_RAND_RESET_I(8, __VscopeHash, 3647702653555509322ull);
    }
    vlSelf->test_copper__DOT__dut__DOT__tile_inst__DOT__tile_size16 = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 1705594749345561836ull);
    for (int __Vi0 = 0; __Vi0 < 25; ++__Vi0) {
        vlSelf->test_copper__DOT__dut__DOT__tile_inst__DOT__col_buffer[__Vi0] = VL_SCOPED_RAND_RESET_I(8, __VscopeHash, 2380392674988079436ull);
    }
    vlSelf->test_copper__DOT__dut__DOT__tile_inst__DOT__td_addr_a = VL_SCOPED_RAND_RESET_I(15, __VscopeHash, 13668715029998272372ull);
    vlSelf->test_copper__DOT__dut__DOT__tile_inst__DOT__td_din_a = VL_SCOPED_RAND_RESET_I(8, __VscopeHash, 7097268287909032405ull);
    vlSelf->test_copper__DOT__dut__DOT__tile_inst__DOT__td_we_a = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 8670353993528993052ull);
    vlSelf->test_copper__DOT__dut__DOT__tile_inst__DOT__td_dout_a = VL_SCOPED_RAND_RESET_I(8, __VscopeHash, 5652242663139064736ull);
    vlSelf->test_copper__DOT__dut__DOT__tile_inst__DOT__nt_addr_a = VL_SCOPED_RAND_RESET_I(12, __VscopeHash, 1047203636710862087ull);
    vlSelf->test_copper__DOT__dut__DOT__tile_inst__DOT__nt_din_a = VL_SCOPED_RAND_RESET_I(8, __VscopeHash, 13842942285800252088ull);
    vlSelf->test_copper__DOT__dut__DOT__tile_inst__DOT__nt_we_a = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 7283435074510346566ull);
    vlSelf->test_copper__DOT__dut__DOT__tile_inst__DOT__at_addr_a = VL_SCOPED_RAND_RESET_I(12, __VscopeHash, 7393049280559904413ull);
    vlSelf->test_copper__DOT__dut__DOT__tile_inst__DOT__at_din_a = VL_SCOPED_RAND_RESET_I(8, __VscopeHash, 6626851986246848021ull);
    vlSelf->test_copper__DOT__dut__DOT__tile_inst__DOT__at_we_a = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 16561944542430292197ull);
    vlSelf->test_copper__DOT__dut__DOT__tile_inst__DOT__dma_state = VL_SCOPED_RAND_RESET_I(5, __VscopeHash, 11344455572685184174ull);
    vlSelf->test_copper__DOT__dut__DOT__tile_inst__DOT__dma_src = VL_SCOPED_RAND_RESET_I(16, __VscopeHash, 5220648913408096971ull);
    vlSelf->test_copper__DOT__dut__DOT__tile_inst__DOT__dma_count = VL_SCOPED_RAND_RESET_I(16, __VscopeHash, 1259947313826609595ull);
    vlSelf->test_copper__DOT__dut__DOT__tile_inst__DOT__dma_dst = VL_SCOPED_RAND_RESET_I(16, __VscopeHash, 3727201715153230000ull);
    vlSelf->test_copper__DOT__dut__DOT__tile_inst__DOT__dma_nt = VL_SCOPED_RAND_RESET_I(2, __VscopeHash, 6413359774174080420ull);
    vlSelf->test_copper__DOT__dut__DOT__tile_inst__DOT__dma_col_row = VL_SCOPED_RAND_RESET_I(6, __VscopeHash, 12752130903127016330ull);
    vlSelf->test_copper__DOT__dut__DOT__tile_inst__DOT__dma_col_idx = VL_SCOPED_RAND_RESET_I(5, __VscopeHash, 4457409059877486562ull);
    vlSelf->test_copper__DOT__dut__DOT__tile_inst__DOT__dma_data_valid = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 6076078393005171996ull);
    vlSelf->test_copper__DOT__dut__DOT__tile_inst__DOT__dma_pal_phase = VL_SCOPED_RAND_RESET_I(8, __VscopeHash, 2887876880597983613ull);
    vlSelf->test_copper__DOT__dut__DOT__tile_inst__DOT__dma_pal_cidx = VL_SCOPED_RAND_RESET_I(4, __VscopeHash, 17726651879959944164ull);
    vlSelf->test_copper__DOT__dut__DOT__tile_inst__DOT__dma_fill_val = VL_SCOPED_RAND_RESET_I(8, __VscopeHash, 3370523269964379739ull);
    vlSelf->test_copper__DOT__dut__DOT__tile_inst__DOT__dma_fill_addr = VL_SCOPED_RAND_RESET_I(12, __VscopeHash, 14133611550169494375ull);
    vlSelf->test_copper__DOT__dut__DOT__tile_inst__DOT__cmd_nt_we = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 4902376860221499380ull);
    vlSelf->test_copper__DOT__dut__DOT__tile_inst__DOT__cmd_at_we = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 13958863230320562083ull);
    vlSelf->test_copper__DOT__dut__DOT__tile_inst__DOT__cmd_nt_addr = VL_SCOPED_RAND_RESET_I(12, __VscopeHash, 17135844639477524434ull);
    vlSelf->test_copper__DOT__dut__DOT__tile_inst__DOT__cmd_at_addr = VL_SCOPED_RAND_RESET_I(12, __VscopeHash, 11564282048446707286ull);
    vlSelf->test_copper__DOT__dut__DOT__tile_inst__DOT__cmd_nt_din = VL_SCOPED_RAND_RESET_I(8, __VscopeHash, 14962872772821473413ull);
    vlSelf->test_copper__DOT__dut__DOT__tile_inst__DOT__cmd_at_din = VL_SCOPED_RAND_RESET_I(8, __VscopeHash, 618454890831757513ull);
    vlSelf->test_copper__DOT__dut__DOT__tile_inst__DOT__blt_tile_rd_pending = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 9615561089885928945ull);
    vlSelf->test_copper__DOT__dut__DOT__tile_inst__DOT__blt_tile_rd_latch = VL_SCOPED_RAND_RESET_I(8, __VscopeHash, 7023523667530263475ull);
    for (int __Vi0 = 0; __Vi0 < 32768; ++__Vi0) {
        vlSelf->test_copper__DOT__dut__DOT__tile_inst__DOT__tile_data_ram__DOT__mem[__Vi0] = VL_SCOPED_RAND_RESET_I(8, __VscopeHash, 10430083825532614310ull);
    }
    for (int __Vi0 = 0; __Vi0 < 4096; ++__Vi0) {
        vlSelf->test_copper__DOT__dut__DOT__tile_inst__DOT__nametable_ram__DOT__mem[__Vi0] = VL_SCOPED_RAND_RESET_I(8, __VscopeHash, 11948609816386118284ull);
    }
    for (int __Vi0 = 0; __Vi0 < 4096; ++__Vi0) {
        vlSelf->test_copper__DOT__dut__DOT__tile_inst__DOT__attr_table_ram__DOT__mem[__Vi0] = VL_SCOPED_RAND_RESET_I(8, __VscopeHash, 11358663122607948659ull);
    }
    for (int __Vi0 = 0; __Vi0 < 1; ++__Vi0) {
        vlSelf->__VstlTriggered[__Vi0] = 0;
    }
    for (int __Vi0 = 0; __Vi0 < 1; ++__Vi0) {
        vlSelf->__VactTriggered[__Vi0] = 0;
    }
    for (int __Vi0 = 0; __Vi0 < 1; ++__Vi0) {
        vlSelf->__VactTriggeredAcc[__Vi0] = 0;
    }
    vlSelf->__Vtrigprevexpr___TOP__test_copper__DOT__clk__0 = 0;
    for (int __Vi0 = 0; __Vi0 < 1; ++__Vi0) {
        vlSelf->__VnbaTriggered[__Vi0] = 0;
    }
    vlSelf->__Vi = 0;
}
