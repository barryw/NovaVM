// Verilated -*- C++ -*-
// DESCRIPTION: Verilator output: Design implementation internals
// See Vtest_gtext.h for the primary calling header

#include "Vtest_gtext__pch.h"

void Vtest_gtext___024root___timing_ready(Vtest_gtext___024root* vlSelf);

VL_ATTR_COLD void Vtest_gtext___024root___eval_static(Vtest_gtext___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vtest_gtext___024root___eval_static\n"); );
    Vtest_gtext__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
    // Body
    vlSelfRef.test_gtext__DOT__clk = 0U;
    vlSelfRef.test_gtext__DOT__pass_count = 0U;
    vlSelfRef.test_gtext__DOT__fail_count = 0U;
    vlSelfRef.test_gtext__DOT__test_num = 0U;
    vlSelfRef.test_gtext__DOT__unnamedblk1__DOT__pixel_count = 0U;
    vlSelfRef.test_gtext__DOT__unnamedblk4__DOT__match = 0U;
    const uint64_t __VscopeHash = VL_MURMUR64_HASH(vlSelf->vlNamep);
    vlSelf->test_gtext__DOT__unnamedblk4__DOT__unnamedblk5__DOT__unnamedblk6__DOT__fb = VL_SCOPED_RAND_RESET_I(8, __VscopeHash, 1577965973047541832ull);
    vlSelf->test_gtext__DOT__unnamedblk4__DOT__unnamedblk5__DOT__unnamedblk6__DOT__unnamedblk7__DOT__unnamedblk8__DOT__expected = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 13111292170923196376ull);
    vlSelfRef.test_gtext__DOT__unnamedblk9__DOT__pc = 0U;
    vlSelfRef.test_gtext__DOT__unnamedblk12__DOT__pc = 0U;
    vlSelf->test_gtext__DOT__unnamedblk15__DOT__fb = VL_SCOPED_RAND_RESET_I(8, __VscopeHash, 18267580500064809425ull);
    vlSelfRef.test_gtext__DOT__unnamedblk15__DOT__ok = 0U;
    vlSelfRef.test_gtext__DOT__unnamedblk17__DOT__pc_a = 0U;
    vlSelfRef.test_gtext__DOT__unnamedblk17__DOT__pc_b = 0U;
    vlSelfRef.test_gtext__DOT__unnamedblk21__DOT__found = 0U;
    vlSelfRef.__Vtrigprevexpr___TOP__test_gtext__DOT__clk__0 = 0U;
    Vtest_gtext___024root___timing_ready(vlSelf);
    do {
        vlSelfRef.__VactTriggeredAcc[vlSelfRef.__Vi] 
            = vlSelfRef.__VactTriggered[vlSelfRef.__Vi];
        vlSelfRef.__Vi = ((IData)(1U) + vlSelfRef.__Vi);
    } while ((0U >= vlSelfRef.__Vi));
}

VL_ATTR_COLD void Vtest_gtext___024root___eval_static__TOP(Vtest_gtext___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vtest_gtext___024root___eval_static__TOP\n"); );
    Vtest_gtext__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
    // Body
    vlSelfRef.test_gtext__DOT__clk = 0U;
    vlSelfRef.test_gtext__DOT__pass_count = 0U;
    vlSelfRef.test_gtext__DOT__fail_count = 0U;
    vlSelfRef.test_gtext__DOT__test_num = 0U;
    vlSelfRef.test_gtext__DOT__unnamedblk1__DOT__pixel_count = 0U;
    vlSelfRef.test_gtext__DOT__unnamedblk4__DOT__match = 0U;
    const uint64_t __VscopeHash = VL_MURMUR64_HASH(vlSelf->vlNamep);
    vlSelf->test_gtext__DOT__unnamedblk4__DOT__unnamedblk5__DOT__unnamedblk6__DOT__fb = VL_SCOPED_RAND_RESET_I(8, __VscopeHash, 1577965973047541832ull);
    vlSelf->test_gtext__DOT__unnamedblk4__DOT__unnamedblk5__DOT__unnamedblk6__DOT__unnamedblk7__DOT__unnamedblk8__DOT__expected = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 13111292170923196376ull);
    vlSelfRef.test_gtext__DOT__unnamedblk9__DOT__pc = 0U;
    vlSelfRef.test_gtext__DOT__unnamedblk12__DOT__pc = 0U;
    vlSelf->test_gtext__DOT__unnamedblk15__DOT__fb = VL_SCOPED_RAND_RESET_I(8, __VscopeHash, 18267580500064809425ull);
    vlSelfRef.test_gtext__DOT__unnamedblk15__DOT__ok = 0U;
    vlSelfRef.test_gtext__DOT__unnamedblk17__DOT__pc_a = 0U;
    vlSelfRef.test_gtext__DOT__unnamedblk17__DOT__pc_b = 0U;
    vlSelfRef.test_gtext__DOT__unnamedblk21__DOT__found = 0U;
}

VL_ATTR_COLD void Vtest_gtext___024root___eval_initial__TOP(Vtest_gtext___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vtest_gtext___024root___eval_initial__TOP\n"); );
    Vtest_gtext__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
    // Locals
    VlUnpacked<CData/*3:0*/, 16> test_gtext__DOT__dut__DOT__spr_trans;
    for (int __Vi0 = 0; __Vi0 < 16; ++__Vi0) {
        test_gtext__DOT__dut__DOT__spr_trans[__Vi0] = 0;
    }
    IData/*31:0*/ test_gtext__DOT__dut__DOT__unnamedblk6__DOT__i;
    test_gtext__DOT__dut__DOT__unnamedblk6__DOT__i = 0;
    IData/*31:0*/ test_gtext__DOT__dut__DOT__unnamedblk7__DOT__i;
    test_gtext__DOT__dut__DOT__unnamedblk7__DOT__i = 0;
    IData/*31:0*/ test_gtext__DOT__dut__DOT__text_inst__DOT__char_mem__DOT__gen_init_zero__DOT__i;
    test_gtext__DOT__dut__DOT__text_inst__DOT__char_mem__DOT__gen_init_zero__DOT__i = 0;
    IData/*31:0*/ test_gtext__DOT__dut__DOT__text_inst__DOT__color_mem__DOT__gen_init_zero__DOT__i;
    test_gtext__DOT__dut__DOT__text_inst__DOT__color_mem__DOT__gen_init_zero__DOT__i = 0;
    IData/*31:0*/ test_gtext__DOT__dut__DOT__gfx_inst__DOT__gfx_mem__DOT__gen_init_zero__DOT__i;
    test_gtext__DOT__dut__DOT__gfx_inst__DOT__gfx_mem__DOT__gen_init_zero__DOT__i = 0;
    IData/*31:0*/ test_gtext__DOT__dut__DOT__artist_inst__DOT__fill_stack_mem__DOT__gen_init_zero__DOT__i;
    test_gtext__DOT__dut__DOT__artist_inst__DOT__fill_stack_mem__DOT__gen_init_zero__DOT__i = 0;
    IData/*31:0*/ test_gtext__DOT__dut__DOT__sprite_inst__DOT__spr_mem__DOT__gen_init_zero__DOT__i;
    test_gtext__DOT__dut__DOT__sprite_inst__DOT__spr_mem__DOT__gen_init_zero__DOT__i = 0;
    IData/*31:0*/ test_gtext__DOT__dut__DOT__tile_inst__DOT__tile_data_ram__DOT__gen_init_zero__DOT__i;
    test_gtext__DOT__dut__DOT__tile_inst__DOT__tile_data_ram__DOT__gen_init_zero__DOT__i = 0;
    IData/*31:0*/ test_gtext__DOT__dut__DOT__tile_inst__DOT__nametable_ram__DOT__gen_init_zero__DOT__i;
    test_gtext__DOT__dut__DOT__tile_inst__DOT__nametable_ram__DOT__gen_init_zero__DOT__i = 0;
    IData/*31:0*/ test_gtext__DOT__dut__DOT__tile_inst__DOT__attr_table_ram__DOT__gen_init_zero__DOT__i;
    test_gtext__DOT__dut__DOT__tile_inst__DOT__attr_table_ram__DOT__gen_init_zero__DOT__i = 0;
    VlUnpacked<SData/*11:0*/, 256> test_gtext__DOT__dut__DOT__tile_inst__DOT__pal_ram_inst__DOT__mem;
    for (int __Vi0 = 0; __Vi0 < 256; ++__Vi0) {
        test_gtext__DOT__dut__DOT__tile_inst__DOT__pal_ram_inst__DOT__mem[__Vi0] = 0;
    }
    IData/*31:0*/ __Vilp1;
    // Body
    vlSelfRef.test_gtext__DOT__dut__DOT__copper_pos[0U] = 0U;
    vlSelfRef.test_gtext__DOT__dut__DOT__copper_reg[0U] = 0U;
    vlSelfRef.test_gtext__DOT__dut__DOT__copper_val[0U] = 0U;
    vlSelfRef.test_gtext__DOT__dut__DOT__copper_pos[1U] = 0U;
    vlSelfRef.test_gtext__DOT__dut__DOT__copper_reg[1U] = 0U;
    vlSelfRef.test_gtext__DOT__dut__DOT__copper_val[1U] = 0U;
    vlSelfRef.test_gtext__DOT__dut__DOT__copper_pos[2U] = 0U;
    vlSelfRef.test_gtext__DOT__dut__DOT__copper_reg[2U] = 0U;
    vlSelfRef.test_gtext__DOT__dut__DOT__copper_val[2U] = 0U;
    vlSelfRef.test_gtext__DOT__dut__DOT__copper_pos[3U] = 0U;
    vlSelfRef.test_gtext__DOT__dut__DOT__copper_reg[3U] = 0U;
    vlSelfRef.test_gtext__DOT__dut__DOT__copper_val[3U] = 0U;
    vlSelfRef.test_gtext__DOT__dut__DOT__copper_pos[4U] = 0U;
    vlSelfRef.test_gtext__DOT__dut__DOT__copper_reg[4U] = 0U;
    vlSelfRef.test_gtext__DOT__dut__DOT__copper_val[4U] = 0U;
    vlSelfRef.test_gtext__DOT__dut__DOT__copper_pos[5U] = 0U;
    vlSelfRef.test_gtext__DOT__dut__DOT__copper_reg[5U] = 0U;
    vlSelfRef.test_gtext__DOT__dut__DOT__copper_val[5U] = 0U;
    vlSelfRef.test_gtext__DOT__dut__DOT__copper_pos[6U] = 0U;
    vlSelfRef.test_gtext__DOT__dut__DOT__copper_reg[6U] = 0U;
    vlSelfRef.test_gtext__DOT__dut__DOT__copper_val[6U] = 0U;
    vlSelfRef.test_gtext__DOT__dut__DOT__copper_pos[7U] = 0U;
    vlSelfRef.test_gtext__DOT__dut__DOT__copper_reg[7U] = 0U;
    vlSelfRef.test_gtext__DOT__dut__DOT__copper_val[7U] = 0U;
    vlSelfRef.test_gtext__DOT__dut__DOT__copper_pos[8U] = 0U;
    vlSelfRef.test_gtext__DOT__dut__DOT__copper_reg[8U] = 0U;
    vlSelfRef.test_gtext__DOT__dut__DOT__copper_val[8U] = 0U;
    vlSelfRef.test_gtext__DOT__dut__DOT__copper_pos[9U] = 0U;
    vlSelfRef.test_gtext__DOT__dut__DOT__copper_reg[9U] = 0U;
    vlSelfRef.test_gtext__DOT__dut__DOT__copper_val[9U] = 0U;
    vlSelfRef.test_gtext__DOT__dut__DOT__copper_pos[10U] = 0U;
    vlSelfRef.test_gtext__DOT__dut__DOT__copper_reg[10U] = 0U;
    vlSelfRef.test_gtext__DOT__dut__DOT__copper_val[10U] = 0U;
    vlSelfRef.test_gtext__DOT__dut__DOT__copper_pos[11U] = 0U;
    vlSelfRef.test_gtext__DOT__dut__DOT__copper_reg[11U] = 0U;
    vlSelfRef.test_gtext__DOT__dut__DOT__copper_val[11U] = 0U;
    vlSelfRef.test_gtext__DOT__dut__DOT__copper_pos[12U] = 0U;
    vlSelfRef.test_gtext__DOT__dut__DOT__copper_reg[12U] = 0U;
    vlSelfRef.test_gtext__DOT__dut__DOT__copper_val[12U] = 0U;
    vlSelfRef.test_gtext__DOT__dut__DOT__copper_pos[13U] = 0U;
    vlSelfRef.test_gtext__DOT__dut__DOT__copper_reg[13U] = 0U;
    vlSelfRef.test_gtext__DOT__dut__DOT__copper_val[13U] = 0U;
    vlSelfRef.test_gtext__DOT__dut__DOT__copper_pos[14U] = 0U;
    vlSelfRef.test_gtext__DOT__dut__DOT__copper_reg[14U] = 0U;
    vlSelfRef.test_gtext__DOT__dut__DOT__copper_val[14U] = 0U;
    vlSelfRef.test_gtext__DOT__dut__DOT__copper_pos[15U] = 0U;
    vlSelfRef.test_gtext__DOT__dut__DOT__copper_reg[15U] = 0U;
    vlSelfRef.test_gtext__DOT__dut__DOT__copper_val[15U] = 0U;
    vlSelfRef.test_gtext__DOT__dut__DOT__copper_pos[16U] = 0U;
    vlSelfRef.test_gtext__DOT__dut__DOT__copper_reg[16U] = 0U;
    vlSelfRef.test_gtext__DOT__dut__DOT__copper_val[16U] = 0U;
    vlSelfRef.test_gtext__DOT__dut__DOT__copper_pos[17U] = 0U;
    vlSelfRef.test_gtext__DOT__dut__DOT__copper_reg[17U] = 0U;
    vlSelfRef.test_gtext__DOT__dut__DOT__copper_val[17U] = 0U;
    vlSelfRef.test_gtext__DOT__dut__DOT__copper_pos[18U] = 0U;
    vlSelfRef.test_gtext__DOT__dut__DOT__copper_reg[18U] = 0U;
    vlSelfRef.test_gtext__DOT__dut__DOT__copper_val[18U] = 0U;
    vlSelfRef.test_gtext__DOT__dut__DOT__copper_pos[19U] = 0U;
    vlSelfRef.test_gtext__DOT__dut__DOT__copper_reg[19U] = 0U;
    vlSelfRef.test_gtext__DOT__dut__DOT__copper_val[19U] = 0U;
    vlSelfRef.test_gtext__DOT__dut__DOT__copper_pos[20U] = 0U;
    vlSelfRef.test_gtext__DOT__dut__DOT__copper_reg[20U] = 0U;
    vlSelfRef.test_gtext__DOT__dut__DOT__copper_val[20U] = 0U;
    vlSelfRef.test_gtext__DOT__dut__DOT__copper_pos[21U] = 0U;
    vlSelfRef.test_gtext__DOT__dut__DOT__copper_reg[21U] = 0U;
    vlSelfRef.test_gtext__DOT__dut__DOT__copper_val[21U] = 0U;
    vlSelfRef.test_gtext__DOT__dut__DOT__copper_pos[22U] = 0U;
    vlSelfRef.test_gtext__DOT__dut__DOT__copper_reg[22U] = 0U;
    vlSelfRef.test_gtext__DOT__dut__DOT__copper_val[22U] = 0U;
    vlSelfRef.test_gtext__DOT__dut__DOT__copper_pos[23U] = 0U;
    vlSelfRef.test_gtext__DOT__dut__DOT__copper_reg[23U] = 0U;
    vlSelfRef.test_gtext__DOT__dut__DOT__copper_val[23U] = 0U;
    vlSelfRef.test_gtext__DOT__dut__DOT__copper_pos[24U] = 0U;
    vlSelfRef.test_gtext__DOT__dut__DOT__copper_reg[24U] = 0U;
    vlSelfRef.test_gtext__DOT__dut__DOT__copper_val[24U] = 0U;
    vlSelfRef.test_gtext__DOT__dut__DOT__copper_pos[25U] = 0U;
    vlSelfRef.test_gtext__DOT__dut__DOT__copper_reg[25U] = 0U;
    vlSelfRef.test_gtext__DOT__dut__DOT__copper_val[25U] = 0U;
    vlSelfRef.test_gtext__DOT__dut__DOT__copper_pos[26U] = 0U;
    vlSelfRef.test_gtext__DOT__dut__DOT__copper_reg[26U] = 0U;
    vlSelfRef.test_gtext__DOT__dut__DOT__copper_val[26U] = 0U;
    vlSelfRef.test_gtext__DOT__dut__DOT__copper_pos[27U] = 0U;
    vlSelfRef.test_gtext__DOT__dut__DOT__copper_reg[27U] = 0U;
    vlSelfRef.test_gtext__DOT__dut__DOT__copper_val[27U] = 0U;
    vlSelfRef.test_gtext__DOT__dut__DOT__copper_pos[28U] = 0U;
    vlSelfRef.test_gtext__DOT__dut__DOT__copper_reg[28U] = 0U;
    vlSelfRef.test_gtext__DOT__dut__DOT__copper_val[28U] = 0U;
    vlSelfRef.test_gtext__DOT__dut__DOT__copper_pos[29U] = 0U;
    vlSelfRef.test_gtext__DOT__dut__DOT__copper_reg[29U] = 0U;
    vlSelfRef.test_gtext__DOT__dut__DOT__copper_val[29U] = 0U;
    vlSelfRef.test_gtext__DOT__dut__DOT__copper_pos[30U] = 0U;
    vlSelfRef.test_gtext__DOT__dut__DOT__copper_reg[30U] = 0U;
    vlSelfRef.test_gtext__DOT__dut__DOT__copper_val[30U] = 0U;
    vlSelfRef.test_gtext__DOT__dut__DOT__copper_pos[31U] = 0U;
    vlSelfRef.test_gtext__DOT__dut__DOT__copper_reg[31U] = 0U;
    vlSelfRef.test_gtext__DOT__dut__DOT__copper_val[31U] = 0U;
    test_gtext__DOT__dut__DOT__unnamedblk6__DOT__i = 0U;
    while (VL_GTS_III(32, 0x00001000U, test_gtext__DOT__dut__DOT__unnamedblk6__DOT__i)) {
        vlSelfRef.test_gtext__DOT__dut__DOT__copper_list_pos[(0x00000fffU 
                                                              & test_gtext__DOT__dut__DOT__unnamedblk6__DOT__i)] = 0U;
        vlSelfRef.test_gtext__DOT__dut__DOT__copper_list_reg[(0x00000fffU 
                                                              & test_gtext__DOT__dut__DOT__unnamedblk6__DOT__i)] = 0U;
        vlSelfRef.test_gtext__DOT__dut__DOT__copper_list_val[(0x00000fffU 
                                                              & test_gtext__DOT__dut__DOT__unnamedblk6__DOT__i)] = 0U;
        test_gtext__DOT__dut__DOT__unnamedblk6__DOT__i 
            = ((IData)(1U) + test_gtext__DOT__dut__DOT__unnamedblk6__DOT__i);
    }
    test_gtext__DOT__dut__DOT__unnamedblk7__DOT__i = 0U;
    while (VL_GTS_III(32, 0x00000080U, test_gtext__DOT__dut__DOT__unnamedblk7__DOT__i)) {
        vlSelfRef.test_gtext__DOT__dut__DOT__copper_list_count[(0x0000007fU 
                                                                & test_gtext__DOT__dut__DOT__unnamedblk7__DOT__i)] = 0U;
        test_gtext__DOT__dut__DOT__unnamedblk7__DOT__i 
            = ((IData)(1U) + test_gtext__DOT__dut__DOT__unnamedblk7__DOT__i);
    }
    vlSelfRef.test_gtext__DOT__dut__DOT__copper_count = 0U;
    vlSelfRef.test_gtext__DOT__dut__DOT__copper_enabled = 0U;
    vlSelfRef.test_gtext__DOT__dut__DOT__copper_target_list = 0U;
    vlSelfRef.test_gtext__DOT__dut__DOT__copper_active_list = 0U;
    vlSelfRef.test_gtext__DOT__dut__DOT__copper_pending_list = 0U;
    vlSelfRef.test_gtext__DOT__dut__DOT__copper_loading = 0U;
    vlSelfRef.test_gtext__DOT__dut__DOT__copper_load_idx = 0U;
    vlSelfRef.test_gtext__DOT__dut__DOT__copper_load_src = 0U;
    __Vilp1 = 0U;
    while ((__Vilp1 <= 0x0000003fU)) {
        vlSelfRef.test_gtext__DOT__dut__DOT__fio_name[__Vilp1] = 0U;
        __Vilp1 = ((IData)(1U) + __Vilp1);
    }
    vlSelfRef.test_gtext__DOT__dut__DOT__fio_name_len = 0U;
    vlSelfRef.test_gtext__DOT__dut__DOT__regs[0U] = 0U;
    vlSelfRef.test_gtext__DOT__dut__DOT__regs[1U] = 0U;
    vlSelfRef.test_gtext__DOT__dut__DOT__regs[2U] = 0U;
    vlSelfRef.test_gtext__DOT__dut__DOT__regs[3U] = 0U;
    vlSelfRef.test_gtext__DOT__dut__DOT__regs[4U] = 0U;
    vlSelfRef.test_gtext__DOT__dut__DOT__regs[5U] = 0U;
    vlSelfRef.test_gtext__DOT__dut__DOT__regs[6U] = 0U;
    vlSelfRef.test_gtext__DOT__dut__DOT__regs[7U] = 0U;
    vlSelfRef.test_gtext__DOT__dut__DOT__regs[8U] = 0U;
    vlSelfRef.test_gtext__DOT__dut__DOT__regs[9U] = 0U;
    vlSelfRef.test_gtext__DOT__dut__DOT__regs[10U] = 0U;
    vlSelfRef.test_gtext__DOT__dut__DOT__regs[11U] = 0U;
    vlSelfRef.test_gtext__DOT__dut__DOT__regs[12U] = 0U;
    vlSelfRef.test_gtext__DOT__dut__DOT__regs[13U] = 0U;
    vlSelfRef.test_gtext__DOT__dut__DOT__regs[14U] = 0U;
    vlSelfRef.test_gtext__DOT__dut__DOT__regs[15U] = 0U;
    vlSelfRef.test_gtext__DOT__dut__DOT__regs[16U] = 0U;
    vlSelfRef.test_gtext__DOT__dut__DOT__regs[17U] = 0U;
    vlSelfRef.test_gtext__DOT__dut__DOT__regs[18U] = 0U;
    vlSelfRef.test_gtext__DOT__dut__DOT__regs[19U] = 0U;
    vlSelfRef.test_gtext__DOT__dut__DOT__regs[20U] = 0U;
    vlSelfRef.test_gtext__DOT__dut__DOT__regs[21U] = 0U;
    vlSelfRef.test_gtext__DOT__dut__DOT__regs[22U] = 0U;
    vlSelfRef.test_gtext__DOT__dut__DOT__regs[23U] = 0U;
    vlSelfRef.test_gtext__DOT__dut__DOT__regs[24U] = 0U;
    vlSelfRef.test_gtext__DOT__dut__DOT__regs[25U] = 0U;
    vlSelfRef.test_gtext__DOT__dut__DOT__regs[26U] = 0U;
    vlSelfRef.test_gtext__DOT__dut__DOT__regs[27U] = 0U;
    vlSelfRef.test_gtext__DOT__dut__DOT__regs[28U] = 0U;
    vlSelfRef.test_gtext__DOT__dut__DOT__regs[29U] = 0U;
    vlSelfRef.test_gtext__DOT__dut__DOT__regs[30U] = 0U;
    vlSelfRef.test_gtext__DOT__dut__DOT__regs[31U] = 0U;
    vlSelfRef.test_gtext__DOT__dut__DOT__cursor_x = 0U;
    vlSelfRef.test_gtext__DOT__dut__DOT__cursor_y = 0U;
    vlSelfRef.test_gtext__DOT__dut__DOT__fg_color = 1U;
    vlSelfRef.test_gtext__DOT__dut__DOT__gfx_color = 1U;
    test_gtext__DOT__dut__DOT__spr_trans[0U] = 0U;
    test_gtext__DOT__dut__DOT__spr_trans[1U] = 0U;
    test_gtext__DOT__dut__DOT__spr_trans[2U] = 0U;
    test_gtext__DOT__dut__DOT__spr_trans[3U] = 0U;
    test_gtext__DOT__dut__DOT__spr_trans[4U] = 0U;
    test_gtext__DOT__dut__DOT__spr_trans[5U] = 0U;
    test_gtext__DOT__dut__DOT__spr_trans[6U] = 0U;
    test_gtext__DOT__dut__DOT__spr_trans[7U] = 0U;
    test_gtext__DOT__dut__DOT__spr_trans[8U] = 0U;
    test_gtext__DOT__dut__DOT__spr_trans[9U] = 0U;
    test_gtext__DOT__dut__DOT__spr_trans[10U] = 0U;
    test_gtext__DOT__dut__DOT__spr_trans[11U] = 0U;
    test_gtext__DOT__dut__DOT__spr_trans[12U] = 0U;
    test_gtext__DOT__dut__DOT__spr_trans[13U] = 0U;
    test_gtext__DOT__dut__DOT__spr_trans[14U] = 0U;
    test_gtext__DOT__dut__DOT__spr_trans[15U] = 0U;
    vlSelfRef.test_gtext__DOT__dut__DOT__scroll_offset = 0U;
    vlSelfRef.test_gtext__DOT__dut__DOT__scroll_pending = 0U;
    vlSelfRef.test_gtext__DOT__dut__DOT__scroll_col = 0U;
    vlSelfRef.test_gtext__DOT__dut__DOT__cmd_busy = 0U;
    vlSelfRef.test_gtext__DOT__dut__DOT__sprrow_count = 0U;
    vlSelfRef.test_gtext__DOT__dut__DOT__sprcopy_phase = 0U;
    vlSelfRef.test_gtext__DOT__dut__DOT__memread_pending = 0U;
    vlSelfRef.test_gtext__DOT__dut__DOT__vgc_tile_addr = 0U;
    vlSelfRef.test_gtext__DOT__dut__DOT__vgc_tile_wdata = 0U;
    vlSelfRef.test_gtext__DOT__dut__DOT__vgc_tile_we = 0U;
    vlSelfRef.test_gtext__DOT__dut__DOT__vgc_tile_re = 0U;
    vlSelfRef.test_gtext__DOT__dut__DOT__artist_cmd_valid = 0U;
    test_gtext__DOT__dut__DOT__text_inst__DOT__char_mem__DOT__gen_init_zero__DOT__i = 0U;
    while (VL_GTS_III(32, 0x000007d0U, test_gtext__DOT__dut__DOT__text_inst__DOT__char_mem__DOT__gen_init_zero__DOT__i)) {
        vlSelfRef.test_gtext__DOT__dut__DOT__text_inst__DOT__char_mem__DOT____Vlvbound_h3482bd38__0 = 0U;
        if (VL_LIKELY(((0x07cfU >= (0x000007ffU & test_gtext__DOT__dut__DOT__text_inst__DOT__char_mem__DOT__gen_init_zero__DOT__i))))) {
            vlSelfRef.test_gtext__DOT__dut__DOT__text_inst__DOT__char_mem__DOT__mem[(0x000007ffU 
                                                                                & test_gtext__DOT__dut__DOT__text_inst__DOT__char_mem__DOT__gen_init_zero__DOT__i)] 
                = vlSelfRef.test_gtext__DOT__dut__DOT__text_inst__DOT__char_mem__DOT____Vlvbound_h3482bd38__0;
        }
        test_gtext__DOT__dut__DOT__text_inst__DOT__char_mem__DOT__gen_init_zero__DOT__i 
            = ((IData)(1U) + test_gtext__DOT__dut__DOT__text_inst__DOT__char_mem__DOT__gen_init_zero__DOT__i);
    }
    test_gtext__DOT__dut__DOT__text_inst__DOT__color_mem__DOT__gen_init_zero__DOT__i = 0U;
    while (VL_GTS_III(32, 0x000007d0U, test_gtext__DOT__dut__DOT__text_inst__DOT__color_mem__DOT__gen_init_zero__DOT__i)) {
        vlSelfRef.test_gtext__DOT__dut__DOT__text_inst__DOT__color_mem__DOT____Vlvbound_h3482bd38__0 = 0U;
        if (VL_LIKELY(((0x07cfU >= (0x000007ffU & test_gtext__DOT__dut__DOT__text_inst__DOT__color_mem__DOT__gen_init_zero__DOT__i))))) {
            vlSelfRef.test_gtext__DOT__dut__DOT__text_inst__DOT__color_mem__DOT__mem[(0x000007ffU 
                                                                                & test_gtext__DOT__dut__DOT__text_inst__DOT__color_mem__DOT__gen_init_zero__DOT__i)] 
                = vlSelfRef.test_gtext__DOT__dut__DOT__text_inst__DOT__color_mem__DOT____Vlvbound_h3482bd38__0;
        }
        test_gtext__DOT__dut__DOT__text_inst__DOT__color_mem__DOT__gen_init_zero__DOT__i 
            = ((IData)(1U) + test_gtext__DOT__dut__DOT__text_inst__DOT__color_mem__DOT__gen_init_zero__DOT__i);
    }
    VL_READMEM_N(true, 8, 2048, 0, "rom/cp437.hex"s
                 ,  &(vlSelfRef.test_gtext__DOT__dut__DOT__text_inst__DOT__font_mem__DOT__mem)
                 , 0, ~0ULL);
    test_gtext__DOT__dut__DOT__gfx_inst__DOT__gfx_mem__DOT__gen_init_zero__DOT__i = 0U;
    while (VL_GTS_III(32, 0x0000fa00U, test_gtext__DOT__dut__DOT__gfx_inst__DOT__gfx_mem__DOT__gen_init_zero__DOT__i)) {
        vlSelfRef.test_gtext__DOT__dut__DOT__gfx_inst__DOT__gfx_mem__DOT____Vlvbound_h75f189da__0 = 0U;
        if (VL_LIKELY(((0xf9ffU >= (0x0000ffffU & test_gtext__DOT__dut__DOT__gfx_inst__DOT__gfx_mem__DOT__gen_init_zero__DOT__i))))) {
            vlSelfRef.test_gtext__DOT__dut__DOT__gfx_inst__DOT__gfx_mem__DOT__mem[(0x0000ffffU 
                                                                                & test_gtext__DOT__dut__DOT__gfx_inst__DOT__gfx_mem__DOT__gen_init_zero__DOT__i)] 
                = vlSelfRef.test_gtext__DOT__dut__DOT__gfx_inst__DOT__gfx_mem__DOT____Vlvbound_h75f189da__0;
        }
        test_gtext__DOT__dut__DOT__gfx_inst__DOT__gfx_mem__DOT__gen_init_zero__DOT__i 
            = ((IData)(1U) + test_gtext__DOT__dut__DOT__gfx_inst__DOT__gfx_mem__DOT__gen_init_zero__DOT__i);
    }
    vlSelfRef.test_gtext__DOT__dut__DOT__artist_busy = 0U;
    vlSelfRef.test_gtext__DOT__dut__DOT__artist_inst__DOT__op = 0U;
    vlSelfRef.test_gtext__DOT__dut__DOT__artist_inst__DOT__fill_sp = 0U;
    vlSelfRef.test_gtext__DOT__dut__DOT__artist_inst__DOT__octant_count = 0U;
    vlSelfRef.test_gtext__DOT__dut__DOT__artist_inst__DOT__paint_phase = 0U;
    vlSelfRef.test_gtext__DOT__dut__DOT__artist_inst__DOT__gt_font_pending = 0U;
    vlSelfRef.test_gtext__DOT__dut__DOT__artist_inst__DOT__paint_init = 0U;
    test_gtext__DOT__dut__DOT__artist_inst__DOT__fill_stack_mem__DOT__gen_init_zero__DOT__i = 0U;
    while (VL_GTS_III(32, 0x00000200U, test_gtext__DOT__dut__DOT__artist_inst__DOT__fill_stack_mem__DOT__gen_init_zero__DOT__i)) {
        vlSelfRef.test_gtext__DOT__dut__DOT__artist_inst__DOT__fill_stack_mem__DOT__mem[(0x000001ffU 
                                                                                & test_gtext__DOT__dut__DOT__artist_inst__DOT__fill_stack_mem__DOT__gen_init_zero__DOT__i)] = 0U;
        test_gtext__DOT__dut__DOT__artist_inst__DOT__fill_stack_mem__DOT__gen_init_zero__DOT__i 
            = ((IData)(1U) + test_gtext__DOT__dut__DOT__artist_inst__DOT__fill_stack_mem__DOT__gen_init_zero__DOT__i);
    }
    test_gtext__DOT__dut__DOT__sprite_inst__DOT__spr_mem__DOT__gen_init_zero__DOT__i = 0U;
    while (VL_GTS_III(32, 0x00000800U, test_gtext__DOT__dut__DOT__sprite_inst__DOT__spr_mem__DOT__gen_init_zero__DOT__i)) {
        vlSelfRef.test_gtext__DOT__dut__DOT__sprite_inst__DOT__spr_mem__DOT__mem[(0x000007ffU 
                                                                                & test_gtext__DOT__dut__DOT__sprite_inst__DOT__spr_mem__DOT__gen_init_zero__DOT__i)] = 0U;
        test_gtext__DOT__dut__DOT__sprite_inst__DOT__spr_mem__DOT__gen_init_zero__DOT__i 
            = ((IData)(1U) + test_gtext__DOT__dut__DOT__sprite_inst__DOT__spr_mem__DOT__gen_init_zero__DOT__i);
    }
    vlSelfRef.test_gtext__DOT__dut__DOT__tile_inst__DOT__tregs[0U] = 0U;
    vlSelfRef.test_gtext__DOT__dut__DOT__tile_inst__DOT__tregs[1U] = 0U;
    vlSelfRef.test_gtext__DOT__dut__DOT__tile_inst__DOT__tregs[2U] = 0U;
    vlSelfRef.test_gtext__DOT__dut__DOT__tile_inst__DOT__tregs[3U] = 0U;
    vlSelfRef.test_gtext__DOT__dut__DOT__tile_inst__DOT__tregs[4U] = 0U;
    vlSelfRef.test_gtext__DOT__dut__DOT__tile_inst__DOT__tregs[5U] = 0U;
    vlSelfRef.test_gtext__DOT__dut__DOT__tile_inst__DOT__tregs[6U] = 0U;
    vlSelfRef.test_gtext__DOT__dut__DOT__tile_inst__DOT__tregs[7U] = 0U;
    vlSelfRef.test_gtext__DOT__dut__DOT__tile_inst__DOT__tregs[8U] = 0U;
    vlSelfRef.test_gtext__DOT__dut__DOT__tile_inst__DOT__tregs[9U] = 0U;
    vlSelfRef.test_gtext__DOT__dut__DOT__tile_inst__DOT__tregs[10U] = 0U;
    vlSelfRef.test_gtext__DOT__dut__DOT__tile_inst__DOT__tregs[11U] = 0U;
    vlSelfRef.test_gtext__DOT__dut__DOT__tile_inst__DOT__tregs[12U] = 0U;
    vlSelfRef.test_gtext__DOT__dut__DOT__tile_inst__DOT__tregs[13U] = 0U;
    vlSelfRef.test_gtext__DOT__dut__DOT__tile_inst__DOT__tregs[14U] = 0U;
    vlSelfRef.test_gtext__DOT__dut__DOT__tile_inst__DOT__tregs[15U] = 0U;
    vlSelfRef.test_gtext__DOT__dut__DOT__tile_inst__DOT__tregs[16U] = 0U;
    vlSelfRef.test_gtext__DOT__dut__DOT__tile_inst__DOT__tregs[17U] = 0U;
    vlSelfRef.test_gtext__DOT__dut__DOT__tile_inst__DOT__tregs[18U] = 0U;
    vlSelfRef.test_gtext__DOT__dut__DOT__tile_inst__DOT__tregs[19U] = 0U;
    vlSelfRef.test_gtext__DOT__dut__DOT__tile_inst__DOT__tregs[20U] = 0U;
    vlSelfRef.test_gtext__DOT__dut__DOT__tile_inst__DOT__tregs[21U] = 0U;
    vlSelfRef.test_gtext__DOT__dut__DOT__tile_inst__DOT__tregs[22U] = 0U;
    vlSelfRef.test_gtext__DOT__dut__DOT__tile_inst__DOT__tregs[23U] = 0U;
    vlSelfRef.test_gtext__DOT__dut__DOT__tile_inst__DOT__tregs[24U] = 0U;
    vlSelfRef.test_gtext__DOT__dut__DOT__tile_inst__DOT__tregs[25U] = 0U;
    vlSelfRef.test_gtext__DOT__dut__DOT__tile_inst__DOT__tregs[26U] = 0U;
    vlSelfRef.test_gtext__DOT__dut__DOT__tile_inst__DOT__tregs[27U] = 0U;
    vlSelfRef.test_gtext__DOT__dut__DOT__tile_inst__DOT__tregs[28U] = 0U;
    vlSelfRef.test_gtext__DOT__dut__DOT__tile_inst__DOT__tregs[29U] = 0U;
    vlSelfRef.test_gtext__DOT__dut__DOT__tile_inst__DOT__tregs[30U] = 0U;
    vlSelfRef.test_gtext__DOT__dut__DOT__tile_inst__DOT__tregs[31U] = 0U;
    vlSelfRef.test_gtext__DOT__dut__DOT__tile_inst__DOT__col_buffer[0U] = 0U;
    vlSelfRef.test_gtext__DOT__dut__DOT__tile_inst__DOT__col_buffer[1U] = 0U;
    vlSelfRef.test_gtext__DOT__dut__DOT__tile_inst__DOT__col_buffer[2U] = 0U;
    vlSelfRef.test_gtext__DOT__dut__DOT__tile_inst__DOT__col_buffer[3U] = 0U;
    vlSelfRef.test_gtext__DOT__dut__DOT__tile_inst__DOT__col_buffer[4U] = 0U;
    vlSelfRef.test_gtext__DOT__dut__DOT__tile_inst__DOT__col_buffer[5U] = 0U;
    vlSelfRef.test_gtext__DOT__dut__DOT__tile_inst__DOT__col_buffer[6U] = 0U;
    vlSelfRef.test_gtext__DOT__dut__DOT__tile_inst__DOT__col_buffer[7U] = 0U;
    vlSelfRef.test_gtext__DOT__dut__DOT__tile_inst__DOT__col_buffer[8U] = 0U;
    vlSelfRef.test_gtext__DOT__dut__DOT__tile_inst__DOT__col_buffer[9U] = 0U;
    vlSelfRef.test_gtext__DOT__dut__DOT__tile_inst__DOT__col_buffer[10U] = 0U;
    vlSelfRef.test_gtext__DOT__dut__DOT__tile_inst__DOT__col_buffer[11U] = 0U;
    vlSelfRef.test_gtext__DOT__dut__DOT__tile_inst__DOT__col_buffer[12U] = 0U;
    vlSelfRef.test_gtext__DOT__dut__DOT__tile_inst__DOT__col_buffer[13U] = 0U;
    vlSelfRef.test_gtext__DOT__dut__DOT__tile_inst__DOT__col_buffer[14U] = 0U;
    vlSelfRef.test_gtext__DOT__dut__DOT__tile_inst__DOT__col_buffer[15U] = 0U;
    vlSelfRef.test_gtext__DOT__dut__DOT__tile_inst__DOT__col_buffer[16U] = 0U;
    vlSelfRef.test_gtext__DOT__dut__DOT__tile_inst__DOT__col_buffer[17U] = 0U;
    vlSelfRef.test_gtext__DOT__dut__DOT__tile_inst__DOT__col_buffer[18U] = 0U;
    vlSelfRef.test_gtext__DOT__dut__DOT__tile_inst__DOT__col_buffer[19U] = 0U;
    vlSelfRef.test_gtext__DOT__dut__DOT__tile_inst__DOT__col_buffer[20U] = 0U;
    vlSelfRef.test_gtext__DOT__dut__DOT__tile_inst__DOT__col_buffer[21U] = 0U;
    vlSelfRef.test_gtext__DOT__dut__DOT__tile_inst__DOT__col_buffer[22U] = 0U;
    vlSelfRef.test_gtext__DOT__dut__DOT__tile_inst__DOT__col_buffer[23U] = 0U;
    vlSelfRef.test_gtext__DOT__dut__DOT__tile_inst__DOT__col_buffer[24U] = 0U;
    vlSelfRef.test_gtext__DOT__dut__DOT__tile_inst__DOT__tile_size16 = 0U;
    vlSelfRef.test_gtext__DOT__dut__DOT__tile_inst__DOT__dma_state = 0U;
    vlSelfRef.test_gtext__DOT__dut__DOT__tile_inst__DOT__dma_src = 0U;
    vlSelfRef.test_gtext__DOT__dut__DOT__tile_inst__DOT__dma_count = 0U;
    vlSelfRef.test_gtext__DOT__dut__DOT__tile_inst__DOT__dma_dst = 0U;
    vlSelfRef.test_gtext__DOT__dut__DOT__tile_inst__DOT__dma_nt = 0U;
    vlSelfRef.test_gtext__DOT__dut__DOT__tile_inst__DOT__dma_col_row = 0U;
    vlSelfRef.test_gtext__DOT__dut__DOT__tile_inst__DOT__dma_col_idx = 0U;
    vlSelfRef.test_gtext__DOT__dut__DOT__tile_inst__DOT__dma_data_valid = 0U;
    vlSelfRef.test_gtext__DOT__dut__DOT__tile_inst__DOT__dma_pal_phase = 0U;
    vlSelfRef.test_gtext__DOT__dut__DOT__tile_inst__DOT__dma_pal_cidx = 0U;
    vlSelfRef.test_gtext__DOT__dut__DOT__tile_inst__DOT__dma_fill_val = 0U;
    vlSelfRef.test_gtext__DOT__dut__DOT__tile_inst__DOT__dma_fill_addr = 0U;
    test_gtext__DOT__dut__DOT__tile_inst__DOT__tile_data_ram__DOT__gen_init_zero__DOT__i = 0U;
    while (VL_GTS_III(32, 0x00008000U, test_gtext__DOT__dut__DOT__tile_inst__DOT__tile_data_ram__DOT__gen_init_zero__DOT__i)) {
        vlSelfRef.test_gtext__DOT__dut__DOT__tile_inst__DOT__tile_data_ram__DOT__mem[(0x00007fffU 
                                                                                & test_gtext__DOT__dut__DOT__tile_inst__DOT__tile_data_ram__DOT__gen_init_zero__DOT__i)] = 0U;
        test_gtext__DOT__dut__DOT__tile_inst__DOT__tile_data_ram__DOT__gen_init_zero__DOT__i 
            = ((IData)(1U) + test_gtext__DOT__dut__DOT__tile_inst__DOT__tile_data_ram__DOT__gen_init_zero__DOT__i);
    }
    test_gtext__DOT__dut__DOT__tile_inst__DOT__nametable_ram__DOT__gen_init_zero__DOT__i = 0U;
    while (VL_GTS_III(32, 0x00001000U, test_gtext__DOT__dut__DOT__tile_inst__DOT__nametable_ram__DOT__gen_init_zero__DOT__i)) {
        vlSelfRef.test_gtext__DOT__dut__DOT__tile_inst__DOT__nametable_ram__DOT__mem[(0x00000fffU 
                                                                                & test_gtext__DOT__dut__DOT__tile_inst__DOT__nametable_ram__DOT__gen_init_zero__DOT__i)] = 0U;
        test_gtext__DOT__dut__DOT__tile_inst__DOT__nametable_ram__DOT__gen_init_zero__DOT__i 
            = ((IData)(1U) + test_gtext__DOT__dut__DOT__tile_inst__DOT__nametable_ram__DOT__gen_init_zero__DOT__i);
    }
    test_gtext__DOT__dut__DOT__tile_inst__DOT__attr_table_ram__DOT__gen_init_zero__DOT__i = 0U;
    while (VL_GTS_III(32, 0x00001000U, test_gtext__DOT__dut__DOT__tile_inst__DOT__attr_table_ram__DOT__gen_init_zero__DOT__i)) {
        vlSelfRef.test_gtext__DOT__dut__DOT__tile_inst__DOT__attr_table_ram__DOT__mem[(0x00000fffU 
                                                                                & test_gtext__DOT__dut__DOT__tile_inst__DOT__attr_table_ram__DOT__gen_init_zero__DOT__i)] = 0U;
        test_gtext__DOT__dut__DOT__tile_inst__DOT__attr_table_ram__DOT__gen_init_zero__DOT__i 
            = ((IData)(1U) + test_gtext__DOT__dut__DOT__tile_inst__DOT__attr_table_ram__DOT__gen_init_zero__DOT__i);
    }
    VL_READMEM_N(true, 12, 256, 0, "rom/tile_pal0.hex"s
                 ,  &(test_gtext__DOT__dut__DOT__tile_inst__DOT__pal_ram_inst__DOT__mem)
                 , 0, ~0ULL);
}

VL_ATTR_COLD void Vtest_gtext___024root___eval_final(Vtest_gtext___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vtest_gtext___024root___eval_final\n"); );
    Vtest_gtext__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
}

#ifdef VL_DEBUG
VL_ATTR_COLD void Vtest_gtext___024root___dump_triggers__stl(const VlUnpacked<QData/*63:0*/, 1> &triggers, const std::string &tag);
#endif  // VL_DEBUG
VL_ATTR_COLD bool Vtest_gtext___024root___eval_phase__stl(Vtest_gtext___024root* vlSelf);

VL_ATTR_COLD void Vtest_gtext___024root___eval_settle(Vtest_gtext___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vtest_gtext___024root___eval_settle\n"); );
    Vtest_gtext__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
    // Locals
    IData/*31:0*/ __VstlIterCount;
    // Body
    __VstlIterCount = 0U;
    vlSelfRef.__VstlFirstIteration = 1U;
    do {
        if (VL_UNLIKELY(((0x00000064U < __VstlIterCount)))) {
#ifdef VL_DEBUG
            Vtest_gtext___024root___dump_triggers__stl(vlSelfRef.__VstlTriggered, "stl"s);
#endif
            VL_FATAL_MT("test_gtext.sv", 6, "", "DIDNOTCONVERGE: Settle region did not converge after '--converge-limit' of 100 tries");
        }
        __VstlIterCount = ((IData)(1U) + __VstlIterCount);
        vlSelfRef.__VstlPhaseResult = Vtest_gtext___024root___eval_phase__stl(vlSelf);
        vlSelfRef.__VstlFirstIteration = 0U;
    } while (vlSelfRef.__VstlPhaseResult);
}

VL_ATTR_COLD void Vtest_gtext___024root___eval_triggers_vec__stl(Vtest_gtext___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vtest_gtext___024root___eval_triggers_vec__stl\n"); );
    Vtest_gtext__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
    // Body
    vlSelfRef.__VstlTriggered[0U] = ((0xfffffffffffffffeULL 
                                      & vlSelfRef.__VstlTriggered[0U]) 
                                     | (IData)((IData)(vlSelfRef.__VstlFirstIteration)));
}

VL_ATTR_COLD bool Vtest_gtext___024root___trigger_anySet__stl(const VlUnpacked<QData/*63:0*/, 1> &in);

#ifdef VL_DEBUG
VL_ATTR_COLD void Vtest_gtext___024root___dump_triggers__stl(const VlUnpacked<QData/*63:0*/, 1> &triggers, const std::string &tag) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vtest_gtext___024root___dump_triggers__stl\n"); );
    // Body
    if ((1U & (~ (IData)(Vtest_gtext___024root___trigger_anySet__stl(triggers))))) {
        VL_DBG_MSGS("         No '" + tag + "' region triggers active\n");
    }
    if ((1U & (IData)(triggers[0U]))) {
        VL_DBG_MSGS("         '" + tag + "' region trigger index 0 is active: Internal 'stl' trigger - first iteration\n");
    }
}
#endif  // VL_DEBUG

VL_ATTR_COLD bool Vtest_gtext___024root___trigger_anySet__stl(const VlUnpacked<QData/*63:0*/, 1> &in) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vtest_gtext___024root___trigger_anySet__stl\n"); );
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

VL_ATTR_COLD void Vtest_gtext___024root___stl_sequent__TOP__0(Vtest_gtext___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vtest_gtext___024root___stl_sequent__TOP__0\n"); );
    Vtest_gtext__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
    // Locals
    IData/*16:0*/ test_gtext__DOT__dut__DOT____Vlvbound_h3e9a9ec3__0;
    test_gtext__DOT__dut__DOT____Vlvbound_h3e9a9ec3__0 = 0;
    CData/*0:0*/ test_gtext__DOT__dut__DOT__visible;
    test_gtext__DOT__dut__DOT__visible = 0;
    CData/*4:0*/ test_gtext__DOT__dut__DOT__text_row;
    test_gtext__DOT__dut__DOT__text_row = 0;
    CData/*4:0*/ test_gtext__DOT__dut__DOT__real_row;
    test_gtext__DOT__dut__DOT__real_row = 0;
    SData/*9:0*/ test_gtext__DOT__dut__DOT__text_line;
    test_gtext__DOT__dut__DOT__text_line = 0;
    SData/*14:0*/ test_gtext__DOT__dut__DOT__tile_blt_addr;
    test_gtext__DOT__dut__DOT__tile_blt_addr = 0;
    CData/*7:0*/ test_gtext__DOT__dut__DOT__tile_blt_wdata;
    test_gtext__DOT__dut__DOT__tile_blt_wdata = 0;
    CData/*0:0*/ __VdfgRegularize_hebeb780c_0_0;
    __VdfgRegularize_hebeb780c_0_0 = 0;
    // Body
    vlSelfRef.test_gtext__DOT__dut__DOT__fio_name_flat[0U] 
        = ((0xff000000U & vlSelfRef.test_gtext__DOT__dut__DOT__fio_name_flat[0U]) 
           | ((vlSelfRef.test_gtext__DOT__dut__DOT__fio_name[2U] 
               << 0x00000010U) | ((vlSelfRef.test_gtext__DOT__dut__DOT__fio_name[1U] 
                                   << 8U) | vlSelfRef.test_gtext__DOT__dut__DOT__fio_name[0U])));
    vlSelfRef.test_gtext__DOT__dut__DOT__fio_name_flat[0U] 
        = ((0x00ffffffU & vlSelfRef.test_gtext__DOT__dut__DOT__fio_name_flat[0U]) 
           | (vlSelfRef.test_gtext__DOT__dut__DOT__fio_name[3U] 
              << 0x00000018U));
    vlSelfRef.test_gtext__DOT__dut__DOT__fio_name_flat[1U] 
        = ((0xffff0000U & vlSelfRef.test_gtext__DOT__dut__DOT__fio_name_flat[1U]) 
           | (0x00ffffffU & ((0x00ffff00U & (vlSelfRef.test_gtext__DOT__dut__DOT__fio_name[5U] 
                                             << 8U)) 
                             | ((0x00ffffffU & vlSelfRef.test_gtext__DOT__dut__DOT__fio_name[4U]) 
                                | (vlSelfRef.test_gtext__DOT__dut__DOT__fio_name[3U] 
                                   >> 8U)))));
    vlSelfRef.test_gtext__DOT__dut__DOT__fio_name_flat[1U] 
        = ((0x0000ffffU & vlSelfRef.test_gtext__DOT__dut__DOT__fio_name_flat[1U]) 
           | (0xffff0000U & ((vlSelfRef.test_gtext__DOT__dut__DOT__fio_name[7U] 
                              << 0x00000018U) | (vlSelfRef.test_gtext__DOT__dut__DOT__fio_name[6U] 
                                                 << 0x00000010U))));
    vlSelfRef.test_gtext__DOT__dut__DOT__fio_name_flat[2U] 
        = ((0xffffff00U & vlSelfRef.test_gtext__DOT__dut__DOT__fio_name_flat[2U]) 
           | (0x0000ffffU & ((0x0000ffffU & vlSelfRef.test_gtext__DOT__dut__DOT__fio_name[8U]) 
                             | ((0x0000ffffU & (vlSelfRef.test_gtext__DOT__dut__DOT__fio_name[7U] 
                                                >> 8U)) 
                                | (vlSelfRef.test_gtext__DOT__dut__DOT__fio_name[6U] 
                                   >> 0x00000010U)))));
    vlSelfRef.test_gtext__DOT__dut__DOT__fio_name_flat[2U] 
        = ((0x000000ffU & vlSelfRef.test_gtext__DOT__dut__DOT__fio_name_flat[2U]) 
           | (0xffffff00U & ((vlSelfRef.test_gtext__DOT__dut__DOT__fio_name[11U] 
                              << 0x00000018U) | ((vlSelfRef.test_gtext__DOT__dut__DOT__fio_name[10U] 
                                                  << 0x00000010U) 
                                                 | (vlSelfRef.test_gtext__DOT__dut__DOT__fio_name[9U] 
                                                    << 8U)))));
    vlSelfRef.test_gtext__DOT__dut__DOT__fio_name_flat[3U] 
        = ((0xff000000U & vlSelfRef.test_gtext__DOT__dut__DOT__fio_name_flat[3U]) 
           | ((vlSelfRef.test_gtext__DOT__dut__DOT__fio_name[14U] 
               << 0x00000010U) | ((vlSelfRef.test_gtext__DOT__dut__DOT__fio_name[13U] 
                                   << 8U) | vlSelfRef.test_gtext__DOT__dut__DOT__fio_name[12U])));
    vlSelfRef.test_gtext__DOT__dut__DOT__fio_name_flat[3U] 
        = ((0x00ffffffU & vlSelfRef.test_gtext__DOT__dut__DOT__fio_name_flat[3U]) 
           | (vlSelfRef.test_gtext__DOT__dut__DOT__fio_name[15U] 
              << 0x00000018U));
    vlSelfRef.test_gtext__DOT__dut__DOT__fio_name_flat[4U] 
        = ((0xffff0000U & vlSelfRef.test_gtext__DOT__dut__DOT__fio_name_flat[4U]) 
           | (0x00ffffffU & ((0x00ffff00U & (vlSelfRef.test_gtext__DOT__dut__DOT__fio_name[17U] 
                                             << 8U)) 
                             | ((0x00ffffffU & vlSelfRef.test_gtext__DOT__dut__DOT__fio_name[16U]) 
                                | (vlSelfRef.test_gtext__DOT__dut__DOT__fio_name[15U] 
                                   >> 8U)))));
    vlSelfRef.test_gtext__DOT__dut__DOT__fio_name_flat[4U] 
        = ((0x0000ffffU & vlSelfRef.test_gtext__DOT__dut__DOT__fio_name_flat[4U]) 
           | (0xffff0000U & ((vlSelfRef.test_gtext__DOT__dut__DOT__fio_name[19U] 
                              << 0x00000018U) | (vlSelfRef.test_gtext__DOT__dut__DOT__fio_name[18U] 
                                                 << 0x00000010U))));
    vlSelfRef.test_gtext__DOT__dut__DOT__fio_name_flat[5U] 
        = ((0xffffff00U & vlSelfRef.test_gtext__DOT__dut__DOT__fio_name_flat[5U]) 
           | (0x0000ffffU & ((0x0000ffffU & vlSelfRef.test_gtext__DOT__dut__DOT__fio_name[20U]) 
                             | ((0x0000ffffU & (vlSelfRef.test_gtext__DOT__dut__DOT__fio_name[19U] 
                                                >> 8U)) 
                                | (vlSelfRef.test_gtext__DOT__dut__DOT__fio_name[18U] 
                                   >> 0x00000010U)))));
    vlSelfRef.test_gtext__DOT__dut__DOT__fio_name_flat[5U] 
        = ((0x000000ffU & vlSelfRef.test_gtext__DOT__dut__DOT__fio_name_flat[5U]) 
           | (0xffffff00U & ((vlSelfRef.test_gtext__DOT__dut__DOT__fio_name[23U] 
                              << 0x00000018U) | ((vlSelfRef.test_gtext__DOT__dut__DOT__fio_name[22U] 
                                                  << 0x00000010U) 
                                                 | (vlSelfRef.test_gtext__DOT__dut__DOT__fio_name[21U] 
                                                    << 8U)))));
    vlSelfRef.test_gtext__DOT__dut__DOT__fio_name_flat[6U] 
        = ((0xff000000U & vlSelfRef.test_gtext__DOT__dut__DOT__fio_name_flat[6U]) 
           | ((vlSelfRef.test_gtext__DOT__dut__DOT__fio_name[26U] 
               << 0x00000010U) | ((vlSelfRef.test_gtext__DOT__dut__DOT__fio_name[25U] 
                                   << 8U) | vlSelfRef.test_gtext__DOT__dut__DOT__fio_name[24U])));
    vlSelfRef.test_gtext__DOT__dut__DOT__fio_name_flat[6U] 
        = ((0x00ffffffU & vlSelfRef.test_gtext__DOT__dut__DOT__fio_name_flat[6U]) 
           | (vlSelfRef.test_gtext__DOT__dut__DOT__fio_name[27U] 
              << 0x00000018U));
    vlSelfRef.test_gtext__DOT__dut__DOT__fio_name_flat[7U] 
        = ((0xffff0000U & vlSelfRef.test_gtext__DOT__dut__DOT__fio_name_flat[7U]) 
           | (0x00ffffffU & ((0x00ffff00U & (vlSelfRef.test_gtext__DOT__dut__DOT__fio_name[29U] 
                                             << 8U)) 
                             | ((0x00ffffffU & vlSelfRef.test_gtext__DOT__dut__DOT__fio_name[28U]) 
                                | (vlSelfRef.test_gtext__DOT__dut__DOT__fio_name[27U] 
                                   >> 8U)))));
    vlSelfRef.test_gtext__DOT__dut__DOT__fio_name_flat[7U] 
        = ((0x0000ffffU & vlSelfRef.test_gtext__DOT__dut__DOT__fio_name_flat[7U]) 
           | (0xffff0000U & ((vlSelfRef.test_gtext__DOT__dut__DOT__fio_name[31U] 
                              << 0x00000018U) | (vlSelfRef.test_gtext__DOT__dut__DOT__fio_name[30U] 
                                                 << 0x00000010U))));
    vlSelfRef.test_gtext__DOT__dut__DOT__fio_name_flat[8U] 
        = ((0xffffff00U & vlSelfRef.test_gtext__DOT__dut__DOT__fio_name_flat[8U]) 
           | (0x0000ffffU & ((0x0000ffffU & vlSelfRef.test_gtext__DOT__dut__DOT__fio_name[32U]) 
                             | ((0x0000ffffU & (vlSelfRef.test_gtext__DOT__dut__DOT__fio_name[31U] 
                                                >> 8U)) 
                                | (vlSelfRef.test_gtext__DOT__dut__DOT__fio_name[30U] 
                                   >> 0x00000010U)))));
    vlSelfRef.test_gtext__DOT__dut__DOT__fio_name_flat[8U] 
        = ((0x000000ffU & vlSelfRef.test_gtext__DOT__dut__DOT__fio_name_flat[8U]) 
           | (0xffffff00U & ((vlSelfRef.test_gtext__DOT__dut__DOT__fio_name[35U] 
                              << 0x00000018U) | ((vlSelfRef.test_gtext__DOT__dut__DOT__fio_name[34U] 
                                                  << 0x00000010U) 
                                                 | (vlSelfRef.test_gtext__DOT__dut__DOT__fio_name[33U] 
                                                    << 8U)))));
    vlSelfRef.test_gtext__DOT__dut__DOT__fio_name_flat[9U] 
        = ((0xff000000U & vlSelfRef.test_gtext__DOT__dut__DOT__fio_name_flat[9U]) 
           | ((vlSelfRef.test_gtext__DOT__dut__DOT__fio_name[38U] 
               << 0x00000010U) | ((vlSelfRef.test_gtext__DOT__dut__DOT__fio_name[37U] 
                                   << 8U) | vlSelfRef.test_gtext__DOT__dut__DOT__fio_name[36U])));
    vlSelfRef.test_gtext__DOT__dut__DOT__fio_name_flat[9U] 
        = ((0x00ffffffU & vlSelfRef.test_gtext__DOT__dut__DOT__fio_name_flat[9U]) 
           | (vlSelfRef.test_gtext__DOT__dut__DOT__fio_name[39U] 
              << 0x00000018U));
    vlSelfRef.test_gtext__DOT__dut__DOT__fio_name_flat[10U] 
        = ((0xffff0000U & vlSelfRef.test_gtext__DOT__dut__DOT__fio_name_flat[10U]) 
           | (0x00ffffffU & ((0x00ffff00U & (vlSelfRef.test_gtext__DOT__dut__DOT__fio_name[41U] 
                                             << 8U)) 
                             | ((0x00ffffffU & vlSelfRef.test_gtext__DOT__dut__DOT__fio_name[40U]) 
                                | (vlSelfRef.test_gtext__DOT__dut__DOT__fio_name[39U] 
                                   >> 8U)))));
    vlSelfRef.test_gtext__DOT__dut__DOT__fio_name_flat[10U] 
        = ((0x0000ffffU & vlSelfRef.test_gtext__DOT__dut__DOT__fio_name_flat[10U]) 
           | (0xffff0000U & ((vlSelfRef.test_gtext__DOT__dut__DOT__fio_name[43U] 
                              << 0x00000018U) | (vlSelfRef.test_gtext__DOT__dut__DOT__fio_name[42U] 
                                                 << 0x00000010U))));
    vlSelfRef.test_gtext__DOT__dut__DOT__fio_name_flat[11U] 
        = ((0xffffff00U & vlSelfRef.test_gtext__DOT__dut__DOT__fio_name_flat[11U]) 
           | (0x0000ffffU & ((0x0000ffffU & vlSelfRef.test_gtext__DOT__dut__DOT__fio_name[44U]) 
                             | ((0x0000ffffU & (vlSelfRef.test_gtext__DOT__dut__DOT__fio_name[43U] 
                                                >> 8U)) 
                                | (vlSelfRef.test_gtext__DOT__dut__DOT__fio_name[42U] 
                                   >> 0x00000010U)))));
    vlSelfRef.test_gtext__DOT__dut__DOT__fio_name_flat[11U] 
        = ((0x000000ffU & vlSelfRef.test_gtext__DOT__dut__DOT__fio_name_flat[11U]) 
           | (0xffffff00U & ((vlSelfRef.test_gtext__DOT__dut__DOT__fio_name[47U] 
                              << 0x00000018U) | ((vlSelfRef.test_gtext__DOT__dut__DOT__fio_name[46U] 
                                                  << 0x00000010U) 
                                                 | (vlSelfRef.test_gtext__DOT__dut__DOT__fio_name[45U] 
                                                    << 8U)))));
    vlSelfRef.test_gtext__DOT__dut__DOT__fio_name_flat[12U] 
        = ((0xff000000U & vlSelfRef.test_gtext__DOT__dut__DOT__fio_name_flat[12U]) 
           | ((vlSelfRef.test_gtext__DOT__dut__DOT__fio_name[50U] 
               << 0x00000010U) | ((vlSelfRef.test_gtext__DOT__dut__DOT__fio_name[49U] 
                                   << 8U) | vlSelfRef.test_gtext__DOT__dut__DOT__fio_name[48U])));
    vlSelfRef.test_gtext__DOT__dut__DOT__fio_name_flat[12U] 
        = ((0x00ffffffU & vlSelfRef.test_gtext__DOT__dut__DOT__fio_name_flat[12U]) 
           | (vlSelfRef.test_gtext__DOT__dut__DOT__fio_name[51U] 
              << 0x00000018U));
    vlSelfRef.test_gtext__DOT__dut__DOT__fio_name_flat[13U] 
        = ((0xffff0000U & vlSelfRef.test_gtext__DOT__dut__DOT__fio_name_flat[13U]) 
           | (0x00ffffffU & ((0x00ffff00U & (vlSelfRef.test_gtext__DOT__dut__DOT__fio_name[53U] 
                                             << 8U)) 
                             | ((0x00ffffffU & vlSelfRef.test_gtext__DOT__dut__DOT__fio_name[52U]) 
                                | (vlSelfRef.test_gtext__DOT__dut__DOT__fio_name[51U] 
                                   >> 8U)))));
    vlSelfRef.test_gtext__DOT__dut__DOT__fio_name_flat[13U] 
        = ((0x0000ffffU & vlSelfRef.test_gtext__DOT__dut__DOT__fio_name_flat[13U]) 
           | (0xffff0000U & ((vlSelfRef.test_gtext__DOT__dut__DOT__fio_name[55U] 
                              << 0x00000018U) | (vlSelfRef.test_gtext__DOT__dut__DOT__fio_name[54U] 
                                                 << 0x00000010U))));
    vlSelfRef.test_gtext__DOT__dut__DOT__fio_name_flat[14U] 
        = ((0xffffff00U & vlSelfRef.test_gtext__DOT__dut__DOT__fio_name_flat[14U]) 
           | (0x0000ffffU & ((0x0000ffffU & vlSelfRef.test_gtext__DOT__dut__DOT__fio_name[56U]) 
                             | ((0x0000ffffU & (vlSelfRef.test_gtext__DOT__dut__DOT__fio_name[55U] 
                                                >> 8U)) 
                                | (vlSelfRef.test_gtext__DOT__dut__DOT__fio_name[54U] 
                                   >> 0x00000010U)))));
    vlSelfRef.test_gtext__DOT__dut__DOT__fio_name_flat[14U] 
        = ((0x000000ffU & vlSelfRef.test_gtext__DOT__dut__DOT__fio_name_flat[14U]) 
           | (0xffffff00U & ((vlSelfRef.test_gtext__DOT__dut__DOT__fio_name[59U] 
                              << 0x00000018U) | ((vlSelfRef.test_gtext__DOT__dut__DOT__fio_name[58U] 
                                                  << 0x00000010U) 
                                                 | (vlSelfRef.test_gtext__DOT__dut__DOT__fio_name[57U] 
                                                    << 8U)))));
    vlSelfRef.test_gtext__DOT__dut__DOT__fio_name_flat[15U] 
        = ((0xff000000U & vlSelfRef.test_gtext__DOT__dut__DOT__fio_name_flat[15U]) 
           | ((vlSelfRef.test_gtext__DOT__dut__DOT__fio_name[62U] 
               << 0x00000010U) | ((vlSelfRef.test_gtext__DOT__dut__DOT__fio_name[61U] 
                                   << 8U) | vlSelfRef.test_gtext__DOT__dut__DOT__fio_name[60U])));
    vlSelfRef.test_gtext__DOT__dut__DOT__fio_name_flat[15U] 
        = ((0x00ffffffU & vlSelfRef.test_gtext__DOT__dut__DOT__fio_name_flat[15U]) 
           | (vlSelfRef.test_gtext__DOT__dut__DOT__fio_name[63U] 
              << 0x00000018U));
    vlSelfRef.test_gtext__DOT__dut__DOT__vgc_reg_sel 
        = ((0xa000U <= (IData)(vlSelfRef.test_gtext__DOT__cpu_addr)) 
           & (0xa01fU >= (IData)(vlSelfRef.test_gtext__DOT__cpu_addr)));
    vlSelfRef.test_gtext__DOT__dut__DOT__spr_reg_sel 
        = ((0xa040U <= (IData)(vlSelfRef.test_gtext__DOT__cpu_addr)) 
           & (0xa0bfU >= (IData)(vlSelfRef.test_gtext__DOT__cpu_addr)));
    vlSelfRef.test_gtext__DOT__dut__DOT__tile_reg_sel 
        = ((0xa0c0U <= (IData)(vlSelfRef.test_gtext__DOT__cpu_addr)) 
           & (0xa0dfU >= (IData)(vlSelfRef.test_gtext__DOT__cpu_addr)));
    vlSelfRef.test_gtext__DOT__dut__DOT____Vcellinp__artist_inst__cmd_x0 
        = ((0x00000100U & (vlSelfRef.test_gtext__DOT__dut__DOT__regs[18U] 
                           << 8U)) | vlSelfRef.test_gtext__DOT__dut__DOT__regs[17U]);
    vlSelfRef.test_gtext__DOT__dut__DOT____Vcellinp__artist_inst__cmd_x1 
        = ((0x00000100U & (vlSelfRef.test_gtext__DOT__dut__DOT__regs[22U] 
                           << 8U)) | vlSelfRef.test_gtext__DOT__dut__DOT__regs[21U]);
    vlSelfRef.test_gtext__DOT__dut__DOT__gfx_a_we = 0U;
    vlSelfRef.test_gtext__DOT__dut__DOT____Vcellinp__tile_inst__cpu_we 
        = ((IData)(vlSelfRef.test_gtext__DOT__cpu_ce) 
           & (IData)(vlSelfRef.test_gtext__DOT__cpu_we));
    vlSelfRef.test_gtext__DOT__dut__DOT__font_a_addr 
        = ((IData)(vlSelfRef.test_gtext__DOT__dut__DOT__artist_font_re)
            ? (IData)(vlSelfRef.test_gtext__DOT__dut__DOT__artist_font_addr)
            : 0U);
    vlSelfRef.test_gtext__DOT__dut__DOT__spr_a_addr = 0U;
    if (vlSelfRef.test_gtext__DOT__dut__DOT__cmd_spr_we) {
        vlSelfRef.test_gtext__DOT__dut__DOT__spr_a_addr 
            = vlSelfRef.test_gtext__DOT__dut__DOT__cmd_spr_addr;
    } else if (vlSelfRef.test_gtext__DOT__dut__DOT__cmd_spr_re) {
        vlSelfRef.test_gtext__DOT__dut__DOT__spr_a_addr 
            = vlSelfRef.test_gtext__DOT__dut__DOT__cmd_spr_addr;
    }
    vlSelfRef.test_gtext__DOT__dut__DOT__tile_inst__DOT__td_we_a = 0U;
    vlSelfRef.test_gtext__DOT__dut__DOT__tile_blt_rdata 
        = ((IData)(vlSelfRef.test_gtext__DOT__dut__DOT__tile_inst__DOT__blt_tile_rd_pending)
            ? (IData)(vlSelfRef.test_gtext__DOT__dut__DOT__tile_inst__DOT__td_dout_a)
            : (IData)(vlSelfRef.test_gtext__DOT__dut__DOT__tile_inst__DOT__blt_tile_rd_latch));
    vlSelfRef.test_gtext__DOT__dut__DOT__copper_reg_flat[0U] 
        = ((0xff000000U & vlSelfRef.test_gtext__DOT__dut__DOT__copper_reg_flat[0U]) 
           | ((vlSelfRef.test_gtext__DOT__dut__DOT__copper_reg[2U] 
               << 0x00000010U) | ((vlSelfRef.test_gtext__DOT__dut__DOT__copper_reg[1U] 
                                   << 8U) | vlSelfRef.test_gtext__DOT__dut__DOT__copper_reg[0U])));
    vlSelfRef.test_gtext__DOT__dut__DOT__copper_reg_flat[0U] 
        = ((0x00ffffffU & vlSelfRef.test_gtext__DOT__dut__DOT__copper_reg_flat[0U]) 
           | (vlSelfRef.test_gtext__DOT__dut__DOT__copper_reg[3U] 
              << 0x00000018U));
    vlSelfRef.test_gtext__DOT__dut__DOT__copper_reg_flat[1U] 
        = ((0xffff0000U & vlSelfRef.test_gtext__DOT__dut__DOT__copper_reg_flat[1U]) 
           | (0x00ffffffU & ((0x00ffff00U & (vlSelfRef.test_gtext__DOT__dut__DOT__copper_reg[5U] 
                                             << 8U)) 
                             | ((0x00ffffffU & vlSelfRef.test_gtext__DOT__dut__DOT__copper_reg[4U]) 
                                | (vlSelfRef.test_gtext__DOT__dut__DOT__copper_reg[3U] 
                                   >> 8U)))));
    vlSelfRef.test_gtext__DOT__dut__DOT__copper_reg_flat[1U] 
        = ((0x0000ffffU & vlSelfRef.test_gtext__DOT__dut__DOT__copper_reg_flat[1U]) 
           | (0xffff0000U & ((vlSelfRef.test_gtext__DOT__dut__DOT__copper_reg[7U] 
                              << 0x00000018U) | (vlSelfRef.test_gtext__DOT__dut__DOT__copper_reg[6U] 
                                                 << 0x00000010U))));
    vlSelfRef.test_gtext__DOT__dut__DOT__copper_reg_flat[2U] 
        = ((0xffffff00U & vlSelfRef.test_gtext__DOT__dut__DOT__copper_reg_flat[2U]) 
           | (0x0000ffffU & ((0x0000ffffU & vlSelfRef.test_gtext__DOT__dut__DOT__copper_reg[8U]) 
                             | ((0x0000ffffU & (vlSelfRef.test_gtext__DOT__dut__DOT__copper_reg[7U] 
                                                >> 8U)) 
                                | (vlSelfRef.test_gtext__DOT__dut__DOT__copper_reg[6U] 
                                   >> 0x00000010U)))));
    vlSelfRef.test_gtext__DOT__dut__DOT__copper_reg_flat[2U] 
        = ((0x000000ffU & vlSelfRef.test_gtext__DOT__dut__DOT__copper_reg_flat[2U]) 
           | (0xffffff00U & ((vlSelfRef.test_gtext__DOT__dut__DOT__copper_reg[11U] 
                              << 0x00000018U) | ((vlSelfRef.test_gtext__DOT__dut__DOT__copper_reg[10U] 
                                                  << 0x00000010U) 
                                                 | (vlSelfRef.test_gtext__DOT__dut__DOT__copper_reg[9U] 
                                                    << 8U)))));
    vlSelfRef.test_gtext__DOT__dut__DOT__copper_reg_flat[3U] 
        = ((0xff000000U & vlSelfRef.test_gtext__DOT__dut__DOT__copper_reg_flat[3U]) 
           | ((vlSelfRef.test_gtext__DOT__dut__DOT__copper_reg[14U] 
               << 0x00000010U) | ((vlSelfRef.test_gtext__DOT__dut__DOT__copper_reg[13U] 
                                   << 8U) | vlSelfRef.test_gtext__DOT__dut__DOT__copper_reg[12U])));
    vlSelfRef.test_gtext__DOT__dut__DOT__copper_reg_flat[3U] 
        = ((0x00ffffffU & vlSelfRef.test_gtext__DOT__dut__DOT__copper_reg_flat[3U]) 
           | (vlSelfRef.test_gtext__DOT__dut__DOT__copper_reg[15U] 
              << 0x00000018U));
    vlSelfRef.test_gtext__DOT__dut__DOT__copper_reg_flat[4U] 
        = ((0xffff0000U & vlSelfRef.test_gtext__DOT__dut__DOT__copper_reg_flat[4U]) 
           | (0x00ffffffU & ((0x00ffff00U & (vlSelfRef.test_gtext__DOT__dut__DOT__copper_reg[17U] 
                                             << 8U)) 
                             | ((0x00ffffffU & vlSelfRef.test_gtext__DOT__dut__DOT__copper_reg[16U]) 
                                | (vlSelfRef.test_gtext__DOT__dut__DOT__copper_reg[15U] 
                                   >> 8U)))));
    vlSelfRef.test_gtext__DOT__dut__DOT__copper_reg_flat[4U] 
        = ((0x0000ffffU & vlSelfRef.test_gtext__DOT__dut__DOT__copper_reg_flat[4U]) 
           | (0xffff0000U & ((vlSelfRef.test_gtext__DOT__dut__DOT__copper_reg[19U] 
                              << 0x00000018U) | (vlSelfRef.test_gtext__DOT__dut__DOT__copper_reg[18U] 
                                                 << 0x00000010U))));
    vlSelfRef.test_gtext__DOT__dut__DOT__copper_reg_flat[5U] 
        = ((0xffffff00U & vlSelfRef.test_gtext__DOT__dut__DOT__copper_reg_flat[5U]) 
           | (0x0000ffffU & ((0x0000ffffU & vlSelfRef.test_gtext__DOT__dut__DOT__copper_reg[20U]) 
                             | ((0x0000ffffU & (vlSelfRef.test_gtext__DOT__dut__DOT__copper_reg[19U] 
                                                >> 8U)) 
                                | (vlSelfRef.test_gtext__DOT__dut__DOT__copper_reg[18U] 
                                   >> 0x00000010U)))));
    vlSelfRef.test_gtext__DOT__dut__DOT__copper_reg_flat[5U] 
        = ((0x000000ffU & vlSelfRef.test_gtext__DOT__dut__DOT__copper_reg_flat[5U]) 
           | (0xffffff00U & ((vlSelfRef.test_gtext__DOT__dut__DOT__copper_reg[23U] 
                              << 0x00000018U) | ((vlSelfRef.test_gtext__DOT__dut__DOT__copper_reg[22U] 
                                                  << 0x00000010U) 
                                                 | (vlSelfRef.test_gtext__DOT__dut__DOT__copper_reg[21U] 
                                                    << 8U)))));
    vlSelfRef.test_gtext__DOT__dut__DOT__copper_reg_flat[6U] 
        = ((0xff000000U & vlSelfRef.test_gtext__DOT__dut__DOT__copper_reg_flat[6U]) 
           | ((vlSelfRef.test_gtext__DOT__dut__DOT__copper_reg[26U] 
               << 0x00000010U) | ((vlSelfRef.test_gtext__DOT__dut__DOT__copper_reg[25U] 
                                   << 8U) | vlSelfRef.test_gtext__DOT__dut__DOT__copper_reg[24U])));
    vlSelfRef.test_gtext__DOT__dut__DOT__copper_reg_flat[6U] 
        = ((0x00ffffffU & vlSelfRef.test_gtext__DOT__dut__DOT__copper_reg_flat[6U]) 
           | (vlSelfRef.test_gtext__DOT__dut__DOT__copper_reg[27U] 
              << 0x00000018U));
    vlSelfRef.test_gtext__DOT__dut__DOT__copper_reg_flat[7U] 
        = ((0xffff0000U & vlSelfRef.test_gtext__DOT__dut__DOT__copper_reg_flat[7U]) 
           | (0x00ffffffU & ((0x00ffff00U & (vlSelfRef.test_gtext__DOT__dut__DOT__copper_reg[29U] 
                                             << 8U)) 
                             | ((0x00ffffffU & vlSelfRef.test_gtext__DOT__dut__DOT__copper_reg[28U]) 
                                | (vlSelfRef.test_gtext__DOT__dut__DOT__copper_reg[27U] 
                                   >> 8U)))));
    vlSelfRef.test_gtext__DOT__dut__DOT__copper_reg_flat[7U] 
        = ((0x0000ffffU & vlSelfRef.test_gtext__DOT__dut__DOT__copper_reg_flat[7U]) 
           | (0xffff0000U & ((vlSelfRef.test_gtext__DOT__dut__DOT__copper_reg[31U] 
                              << 0x00000018U) | (vlSelfRef.test_gtext__DOT__dut__DOT__copper_reg[30U] 
                                                 << 0x00000010U))));
    vlSelfRef.test_gtext__DOT__dut__DOT__copper_val_flat[0U] 
        = ((0xff000000U & vlSelfRef.test_gtext__DOT__dut__DOT__copper_val_flat[0U]) 
           | ((vlSelfRef.test_gtext__DOT__dut__DOT__copper_val[2U] 
               << 0x00000010U) | ((vlSelfRef.test_gtext__DOT__dut__DOT__copper_val[1U] 
                                   << 8U) | vlSelfRef.test_gtext__DOT__dut__DOT__copper_val[0U])));
    vlSelfRef.test_gtext__DOT__dut__DOT__copper_val_flat[0U] 
        = ((0x00ffffffU & vlSelfRef.test_gtext__DOT__dut__DOT__copper_val_flat[0U]) 
           | (vlSelfRef.test_gtext__DOT__dut__DOT__copper_val[3U] 
              << 0x00000018U));
    vlSelfRef.test_gtext__DOT__dut__DOT__copper_val_flat[1U] 
        = ((0xffff0000U & vlSelfRef.test_gtext__DOT__dut__DOT__copper_val_flat[1U]) 
           | (0x00ffffffU & ((0x00ffff00U & (vlSelfRef.test_gtext__DOT__dut__DOT__copper_val[5U] 
                                             << 8U)) 
                             | ((0x00ffffffU & vlSelfRef.test_gtext__DOT__dut__DOT__copper_val[4U]) 
                                | (vlSelfRef.test_gtext__DOT__dut__DOT__copper_val[3U] 
                                   >> 8U)))));
    vlSelfRef.test_gtext__DOT__dut__DOT__copper_val_flat[1U] 
        = ((0x0000ffffU & vlSelfRef.test_gtext__DOT__dut__DOT__copper_val_flat[1U]) 
           | (0xffff0000U & ((vlSelfRef.test_gtext__DOT__dut__DOT__copper_val[7U] 
                              << 0x00000018U) | (vlSelfRef.test_gtext__DOT__dut__DOT__copper_val[6U] 
                                                 << 0x00000010U))));
    vlSelfRef.test_gtext__DOT__dut__DOT__copper_val_flat[2U] 
        = ((0xffffff00U & vlSelfRef.test_gtext__DOT__dut__DOT__copper_val_flat[2U]) 
           | (0x0000ffffU & ((0x0000ffffU & vlSelfRef.test_gtext__DOT__dut__DOT__copper_val[8U]) 
                             | ((0x0000ffffU & (vlSelfRef.test_gtext__DOT__dut__DOT__copper_val[7U] 
                                                >> 8U)) 
                                | (vlSelfRef.test_gtext__DOT__dut__DOT__copper_val[6U] 
                                   >> 0x00000010U)))));
    vlSelfRef.test_gtext__DOT__dut__DOT__copper_val_flat[2U] 
        = ((0x000000ffU & vlSelfRef.test_gtext__DOT__dut__DOT__copper_val_flat[2U]) 
           | (0xffffff00U & ((vlSelfRef.test_gtext__DOT__dut__DOT__copper_val[11U] 
                              << 0x00000018U) | ((vlSelfRef.test_gtext__DOT__dut__DOT__copper_val[10U] 
                                                  << 0x00000010U) 
                                                 | (vlSelfRef.test_gtext__DOT__dut__DOT__copper_val[9U] 
                                                    << 8U)))));
    vlSelfRef.test_gtext__DOT__dut__DOT__copper_val_flat[3U] 
        = ((0xff000000U & vlSelfRef.test_gtext__DOT__dut__DOT__copper_val_flat[3U]) 
           | ((vlSelfRef.test_gtext__DOT__dut__DOT__copper_val[14U] 
               << 0x00000010U) | ((vlSelfRef.test_gtext__DOT__dut__DOT__copper_val[13U] 
                                   << 8U) | vlSelfRef.test_gtext__DOT__dut__DOT__copper_val[12U])));
    vlSelfRef.test_gtext__DOT__dut__DOT__copper_val_flat[3U] 
        = ((0x00ffffffU & vlSelfRef.test_gtext__DOT__dut__DOT__copper_val_flat[3U]) 
           | (vlSelfRef.test_gtext__DOT__dut__DOT__copper_val[15U] 
              << 0x00000018U));
    vlSelfRef.test_gtext__DOT__dut__DOT__copper_val_flat[4U] 
        = ((0xffff0000U & vlSelfRef.test_gtext__DOT__dut__DOT__copper_val_flat[4U]) 
           | (0x00ffffffU & ((0x00ffff00U & (vlSelfRef.test_gtext__DOT__dut__DOT__copper_val[17U] 
                                             << 8U)) 
                             | ((0x00ffffffU & vlSelfRef.test_gtext__DOT__dut__DOT__copper_val[16U]) 
                                | (vlSelfRef.test_gtext__DOT__dut__DOT__copper_val[15U] 
                                   >> 8U)))));
    vlSelfRef.test_gtext__DOT__dut__DOT__copper_val_flat[4U] 
        = ((0x0000ffffU & vlSelfRef.test_gtext__DOT__dut__DOT__copper_val_flat[4U]) 
           | (0xffff0000U & ((vlSelfRef.test_gtext__DOT__dut__DOT__copper_val[19U] 
                              << 0x00000018U) | (vlSelfRef.test_gtext__DOT__dut__DOT__copper_val[18U] 
                                                 << 0x00000010U))));
    vlSelfRef.test_gtext__DOT__dut__DOT__copper_val_flat[5U] 
        = ((0xffffff00U & vlSelfRef.test_gtext__DOT__dut__DOT__copper_val_flat[5U]) 
           | (0x0000ffffU & ((0x0000ffffU & vlSelfRef.test_gtext__DOT__dut__DOT__copper_val[20U]) 
                             | ((0x0000ffffU & (vlSelfRef.test_gtext__DOT__dut__DOT__copper_val[19U] 
                                                >> 8U)) 
                                | (vlSelfRef.test_gtext__DOT__dut__DOT__copper_val[18U] 
                                   >> 0x00000010U)))));
    vlSelfRef.test_gtext__DOT__dut__DOT__copper_val_flat[5U] 
        = ((0x000000ffU & vlSelfRef.test_gtext__DOT__dut__DOT__copper_val_flat[5U]) 
           | (0xffffff00U & ((vlSelfRef.test_gtext__DOT__dut__DOT__copper_val[23U] 
                              << 0x00000018U) | ((vlSelfRef.test_gtext__DOT__dut__DOT__copper_val[22U] 
                                                  << 0x00000010U) 
                                                 | (vlSelfRef.test_gtext__DOT__dut__DOT__copper_val[21U] 
                                                    << 8U)))));
    vlSelfRef.test_gtext__DOT__dut__DOT__copper_val_flat[6U] 
        = ((0xff000000U & vlSelfRef.test_gtext__DOT__dut__DOT__copper_val_flat[6U]) 
           | ((vlSelfRef.test_gtext__DOT__dut__DOT__copper_val[26U] 
               << 0x00000010U) | ((vlSelfRef.test_gtext__DOT__dut__DOT__copper_val[25U] 
                                   << 8U) | vlSelfRef.test_gtext__DOT__dut__DOT__copper_val[24U])));
    vlSelfRef.test_gtext__DOT__dut__DOT__copper_val_flat[6U] 
        = ((0x00ffffffU & vlSelfRef.test_gtext__DOT__dut__DOT__copper_val_flat[6U]) 
           | (vlSelfRef.test_gtext__DOT__dut__DOT__copper_val[27U] 
              << 0x00000018U));
    vlSelfRef.test_gtext__DOT__dut__DOT__copper_val_flat[7U] 
        = ((0xffff0000U & vlSelfRef.test_gtext__DOT__dut__DOT__copper_val_flat[7U]) 
           | (0x00ffffffU & ((0x00ffff00U & (vlSelfRef.test_gtext__DOT__dut__DOT__copper_val[29U] 
                                             << 8U)) 
                             | ((0x00ffffffU & vlSelfRef.test_gtext__DOT__dut__DOT__copper_val[28U]) 
                                | (vlSelfRef.test_gtext__DOT__dut__DOT__copper_val[27U] 
                                   >> 8U)))));
    vlSelfRef.test_gtext__DOT__dut__DOT__copper_val_flat[7U] 
        = ((0x0000ffffU & vlSelfRef.test_gtext__DOT__dut__DOT__copper_val_flat[7U]) 
           | (0xffff0000U & ((vlSelfRef.test_gtext__DOT__dut__DOT__copper_val[31U] 
                              << 0x00000018U) | (vlSelfRef.test_gtext__DOT__dut__DOT__copper_val[30U] 
                                                 << 0x00000010U))));
    vlSelfRef.test_gtext__DOT__dut__DOT__gfx_a_din = 0U;
    vlSelfRef.test_gtext__DOT__dut__DOT__tile_inst__DOT__at_we_a = 0U;
    vlSelfRef.test_gtext__DOT__dut__DOT__tile_inst__DOT__at_din_a = 0U;
    test_gtext__DOT__dut__DOT____Vlvbound_h3e9a9ec3__0 
        = vlSelfRef.test_gtext__DOT__dut__DOT__copper_pos[0U];
    vlSelfRef.test_gtext__DOT__dut__DOT__copper_pos_flat[0U] 
        = ((0xfffe0000U & vlSelfRef.test_gtext__DOT__dut__DOT__copper_pos_flat[0U]) 
           | test_gtext__DOT__dut__DOT____Vlvbound_h3e9a9ec3__0);
    test_gtext__DOT__dut__DOT____Vlvbound_h3e9a9ec3__0 
        = vlSelfRef.test_gtext__DOT__dut__DOT__copper_pos[1U];
    vlSelfRef.test_gtext__DOT__dut__DOT__copper_pos_flat[0U] 
        = ((0x0001ffffU & vlSelfRef.test_gtext__DOT__dut__DOT__copper_pos_flat[0U]) 
           | (test_gtext__DOT__dut__DOT____Vlvbound_h3e9a9ec3__0 
              << 0x00000011U));
    vlSelfRef.test_gtext__DOT__dut__DOT__copper_pos_flat[1U] 
        = ((0xfffffffcU & vlSelfRef.test_gtext__DOT__dut__DOT__copper_pos_flat[1U]) 
           | (test_gtext__DOT__dut__DOT____Vlvbound_h3e9a9ec3__0 
              >> 0x0000000fU));
    test_gtext__DOT__dut__DOT____Vlvbound_h3e9a9ec3__0 
        = vlSelfRef.test_gtext__DOT__dut__DOT__copper_pos[2U];
    vlSelfRef.test_gtext__DOT__dut__DOT__copper_pos_flat[1U] 
        = ((0xfff80003U & vlSelfRef.test_gtext__DOT__dut__DOT__copper_pos_flat[1U]) 
           | (test_gtext__DOT__dut__DOT____Vlvbound_h3e9a9ec3__0 
              << 2U));
    test_gtext__DOT__dut__DOT____Vlvbound_h3e9a9ec3__0 
        = vlSelfRef.test_gtext__DOT__dut__DOT__copper_pos[3U];
    vlSelfRef.test_gtext__DOT__dut__DOT__copper_pos_flat[1U] 
        = ((0x0007ffffU & vlSelfRef.test_gtext__DOT__dut__DOT__copper_pos_flat[1U]) 
           | (test_gtext__DOT__dut__DOT____Vlvbound_h3e9a9ec3__0 
              << 0x00000013U));
    vlSelfRef.test_gtext__DOT__dut__DOT__copper_pos_flat[2U] 
        = ((0xfffffff0U & vlSelfRef.test_gtext__DOT__dut__DOT__copper_pos_flat[2U]) 
           | (test_gtext__DOT__dut__DOT____Vlvbound_h3e9a9ec3__0 
              >> 0x0000000dU));
    test_gtext__DOT__dut__DOT____Vlvbound_h3e9a9ec3__0 
        = vlSelfRef.test_gtext__DOT__dut__DOT__copper_pos[4U];
    vlSelfRef.test_gtext__DOT__dut__DOT__copper_pos_flat[2U] 
        = ((0xffe0000fU & vlSelfRef.test_gtext__DOT__dut__DOT__copper_pos_flat[2U]) 
           | (test_gtext__DOT__dut__DOT____Vlvbound_h3e9a9ec3__0 
              << 4U));
    test_gtext__DOT__dut__DOT____Vlvbound_h3e9a9ec3__0 
        = vlSelfRef.test_gtext__DOT__dut__DOT__copper_pos[5U];
    vlSelfRef.test_gtext__DOT__dut__DOT__copper_pos_flat[2U] 
        = ((0x001fffffU & vlSelfRef.test_gtext__DOT__dut__DOT__copper_pos_flat[2U]) 
           | (test_gtext__DOT__dut__DOT____Vlvbound_h3e9a9ec3__0 
              << 0x00000015U));
    vlSelfRef.test_gtext__DOT__dut__DOT__copper_pos_flat[3U] 
        = ((0xffffffc0U & vlSelfRef.test_gtext__DOT__dut__DOT__copper_pos_flat[3U]) 
           | (test_gtext__DOT__dut__DOT____Vlvbound_h3e9a9ec3__0 
              >> 0x0000000bU));
    test_gtext__DOT__dut__DOT____Vlvbound_h3e9a9ec3__0 
        = vlSelfRef.test_gtext__DOT__dut__DOT__copper_pos[6U];
    vlSelfRef.test_gtext__DOT__dut__DOT__copper_pos_flat[3U] 
        = ((0xff80003fU & vlSelfRef.test_gtext__DOT__dut__DOT__copper_pos_flat[3U]) 
           | (test_gtext__DOT__dut__DOT____Vlvbound_h3e9a9ec3__0 
              << 6U));
    test_gtext__DOT__dut__DOT____Vlvbound_h3e9a9ec3__0 
        = vlSelfRef.test_gtext__DOT__dut__DOT__copper_pos[7U];
    vlSelfRef.test_gtext__DOT__dut__DOT__copper_pos_flat[3U] 
        = ((0x007fffffU & vlSelfRef.test_gtext__DOT__dut__DOT__copper_pos_flat[3U]) 
           | (test_gtext__DOT__dut__DOT____Vlvbound_h3e9a9ec3__0 
              << 0x00000017U));
    vlSelfRef.test_gtext__DOT__dut__DOT__copper_pos_flat[4U] 
        = ((0xffffff00U & vlSelfRef.test_gtext__DOT__dut__DOT__copper_pos_flat[4U]) 
           | (test_gtext__DOT__dut__DOT____Vlvbound_h3e9a9ec3__0 
              >> 9U));
    test_gtext__DOT__dut__DOT____Vlvbound_h3e9a9ec3__0 
        = vlSelfRef.test_gtext__DOT__dut__DOT__copper_pos[8U];
    vlSelfRef.test_gtext__DOT__dut__DOT__copper_pos_flat[4U] 
        = ((0xfe0000ffU & vlSelfRef.test_gtext__DOT__dut__DOT__copper_pos_flat[4U]) 
           | (test_gtext__DOT__dut__DOT____Vlvbound_h3e9a9ec3__0 
              << 8U));
    test_gtext__DOT__dut__DOT____Vlvbound_h3e9a9ec3__0 
        = vlSelfRef.test_gtext__DOT__dut__DOT__copper_pos[9U];
    vlSelfRef.test_gtext__DOT__dut__DOT__copper_pos_flat[4U] 
        = ((0x01ffffffU & vlSelfRef.test_gtext__DOT__dut__DOT__copper_pos_flat[4U]) 
           | (test_gtext__DOT__dut__DOT____Vlvbound_h3e9a9ec3__0 
              << 0x00000019U));
    vlSelfRef.test_gtext__DOT__dut__DOT__copper_pos_flat[5U] 
        = ((0xfffffc00U & vlSelfRef.test_gtext__DOT__dut__DOT__copper_pos_flat[5U]) 
           | (test_gtext__DOT__dut__DOT____Vlvbound_h3e9a9ec3__0 
              >> 7U));
    test_gtext__DOT__dut__DOT____Vlvbound_h3e9a9ec3__0 
        = vlSelfRef.test_gtext__DOT__dut__DOT__copper_pos[10U];
    vlSelfRef.test_gtext__DOT__dut__DOT__copper_pos_flat[5U] 
        = ((0xf80003ffU & vlSelfRef.test_gtext__DOT__dut__DOT__copper_pos_flat[5U]) 
           | (test_gtext__DOT__dut__DOT____Vlvbound_h3e9a9ec3__0 
              << 0x0000000aU));
    test_gtext__DOT__dut__DOT____Vlvbound_h3e9a9ec3__0 
        = vlSelfRef.test_gtext__DOT__dut__DOT__copper_pos[11U];
    vlSelfRef.test_gtext__DOT__dut__DOT__copper_pos_flat[5U] 
        = ((0x07ffffffU & vlSelfRef.test_gtext__DOT__dut__DOT__copper_pos_flat[5U]) 
           | (test_gtext__DOT__dut__DOT____Vlvbound_h3e9a9ec3__0 
              << 0x0000001bU));
    vlSelfRef.test_gtext__DOT__dut__DOT__copper_pos_flat[6U] 
        = ((0xfffff000U & vlSelfRef.test_gtext__DOT__dut__DOT__copper_pos_flat[6U]) 
           | (test_gtext__DOT__dut__DOT____Vlvbound_h3e9a9ec3__0 
              >> 5U));
    test_gtext__DOT__dut__DOT____Vlvbound_h3e9a9ec3__0 
        = vlSelfRef.test_gtext__DOT__dut__DOT__copper_pos[12U];
    vlSelfRef.test_gtext__DOT__dut__DOT__copper_pos_flat[6U] 
        = ((0xe0000fffU & vlSelfRef.test_gtext__DOT__dut__DOT__copper_pos_flat[6U]) 
           | (test_gtext__DOT__dut__DOT____Vlvbound_h3e9a9ec3__0 
              << 0x0000000cU));
    test_gtext__DOT__dut__DOT____Vlvbound_h3e9a9ec3__0 
        = vlSelfRef.test_gtext__DOT__dut__DOT__copper_pos[13U];
    vlSelfRef.test_gtext__DOT__dut__DOT__copper_pos_flat[6U] 
        = ((0x1fffffffU & vlSelfRef.test_gtext__DOT__dut__DOT__copper_pos_flat[6U]) 
           | (test_gtext__DOT__dut__DOT____Vlvbound_h3e9a9ec3__0 
              << 0x0000001dU));
    vlSelfRef.test_gtext__DOT__dut__DOT__copper_pos_flat[7U] 
        = ((0xffffc000U & vlSelfRef.test_gtext__DOT__dut__DOT__copper_pos_flat[7U]) 
           | (test_gtext__DOT__dut__DOT____Vlvbound_h3e9a9ec3__0 
              >> 3U));
    test_gtext__DOT__dut__DOT____Vlvbound_h3e9a9ec3__0 
        = vlSelfRef.test_gtext__DOT__dut__DOT__copper_pos[14U];
    vlSelfRef.test_gtext__DOT__dut__DOT__copper_pos_flat[7U] 
        = ((0x80003fffU & vlSelfRef.test_gtext__DOT__dut__DOT__copper_pos_flat[7U]) 
           | (test_gtext__DOT__dut__DOT____Vlvbound_h3e9a9ec3__0 
              << 0x0000000eU));
    test_gtext__DOT__dut__DOT____Vlvbound_h3e9a9ec3__0 
        = vlSelfRef.test_gtext__DOT__dut__DOT__copper_pos[15U];
    vlSelfRef.test_gtext__DOT__dut__DOT__copper_pos_flat[7U] 
        = ((0x7fffffffU & vlSelfRef.test_gtext__DOT__dut__DOT__copper_pos_flat[7U]) 
           | (test_gtext__DOT__dut__DOT____Vlvbound_h3e9a9ec3__0 
              << 0x0000001fU));
    vlSelfRef.test_gtext__DOT__dut__DOT__copper_pos_flat[8U] 
        = ((0xffff0000U & vlSelfRef.test_gtext__DOT__dut__DOT__copper_pos_flat[8U]) 
           | (test_gtext__DOT__dut__DOT____Vlvbound_h3e9a9ec3__0 
              >> 1U));
    test_gtext__DOT__dut__DOT____Vlvbound_h3e9a9ec3__0 
        = vlSelfRef.test_gtext__DOT__dut__DOT__copper_pos[16U];
    vlSelfRef.test_gtext__DOT__dut__DOT__copper_pos_flat[8U] 
        = ((0x0000ffffU & vlSelfRef.test_gtext__DOT__dut__DOT__copper_pos_flat[8U]) 
           | (test_gtext__DOT__dut__DOT____Vlvbound_h3e9a9ec3__0 
              << 0x00000010U));
    vlSelfRef.test_gtext__DOT__dut__DOT__copper_pos_flat[9U] 
        = ((0xfffffffeU & vlSelfRef.test_gtext__DOT__dut__DOT__copper_pos_flat[9U]) 
           | (test_gtext__DOT__dut__DOT____Vlvbound_h3e9a9ec3__0 
              >> 0x00000010U));
    test_gtext__DOT__dut__DOT____Vlvbound_h3e9a9ec3__0 
        = vlSelfRef.test_gtext__DOT__dut__DOT__copper_pos[17U];
    vlSelfRef.test_gtext__DOT__dut__DOT__copper_pos_flat[9U] 
        = ((0xfffc0001U & vlSelfRef.test_gtext__DOT__dut__DOT__copper_pos_flat[9U]) 
           | (test_gtext__DOT__dut__DOT____Vlvbound_h3e9a9ec3__0 
              << 1U));
    test_gtext__DOT__dut__DOT____Vlvbound_h3e9a9ec3__0 
        = vlSelfRef.test_gtext__DOT__dut__DOT__copper_pos[18U];
    vlSelfRef.test_gtext__DOT__dut__DOT__copper_pos_flat[9U] 
        = ((0x0003ffffU & vlSelfRef.test_gtext__DOT__dut__DOT__copper_pos_flat[9U]) 
           | (test_gtext__DOT__dut__DOT____Vlvbound_h3e9a9ec3__0 
              << 0x00000012U));
    vlSelfRef.test_gtext__DOT__dut__DOT__copper_pos_flat[10U] 
        = ((0xfffffff8U & vlSelfRef.test_gtext__DOT__dut__DOT__copper_pos_flat[10U]) 
           | (test_gtext__DOT__dut__DOT____Vlvbound_h3e9a9ec3__0 
              >> 0x0000000eU));
    test_gtext__DOT__dut__DOT____Vlvbound_h3e9a9ec3__0 
        = vlSelfRef.test_gtext__DOT__dut__DOT__copper_pos[19U];
    vlSelfRef.test_gtext__DOT__dut__DOT__copper_pos_flat[10U] 
        = ((0xfff00007U & vlSelfRef.test_gtext__DOT__dut__DOT__copper_pos_flat[10U]) 
           | (test_gtext__DOT__dut__DOT____Vlvbound_h3e9a9ec3__0 
              << 3U));
    test_gtext__DOT__dut__DOT____Vlvbound_h3e9a9ec3__0 
        = vlSelfRef.test_gtext__DOT__dut__DOT__copper_pos[20U];
    vlSelfRef.test_gtext__DOT__dut__DOT__copper_pos_flat[10U] 
        = ((0x000fffffU & vlSelfRef.test_gtext__DOT__dut__DOT__copper_pos_flat[10U]) 
           | (test_gtext__DOT__dut__DOT____Vlvbound_h3e9a9ec3__0 
              << 0x00000014U));
    vlSelfRef.test_gtext__DOT__dut__DOT__copper_pos_flat[11U] 
        = ((0xffffffe0U & vlSelfRef.test_gtext__DOT__dut__DOT__copper_pos_flat[11U]) 
           | (test_gtext__DOT__dut__DOT____Vlvbound_h3e9a9ec3__0 
              >> 0x0000000cU));
    test_gtext__DOT__dut__DOT____Vlvbound_h3e9a9ec3__0 
        = vlSelfRef.test_gtext__DOT__dut__DOT__copper_pos[21U];
    vlSelfRef.test_gtext__DOT__dut__DOT__copper_pos_flat[11U] 
        = ((0xffc0001fU & vlSelfRef.test_gtext__DOT__dut__DOT__copper_pos_flat[11U]) 
           | (test_gtext__DOT__dut__DOT____Vlvbound_h3e9a9ec3__0 
              << 5U));
    test_gtext__DOT__dut__DOT____Vlvbound_h3e9a9ec3__0 
        = vlSelfRef.test_gtext__DOT__dut__DOT__copper_pos[22U];
    vlSelfRef.test_gtext__DOT__dut__DOT__copper_pos_flat[11U] 
        = ((0x003fffffU & vlSelfRef.test_gtext__DOT__dut__DOT__copper_pos_flat[11U]) 
           | (test_gtext__DOT__dut__DOT____Vlvbound_h3e9a9ec3__0 
              << 0x00000016U));
    vlSelfRef.test_gtext__DOT__dut__DOT__copper_pos_flat[12U] 
        = ((0xffffff80U & vlSelfRef.test_gtext__DOT__dut__DOT__copper_pos_flat[12U]) 
           | (test_gtext__DOT__dut__DOT____Vlvbound_h3e9a9ec3__0 
              >> 0x0000000aU));
    test_gtext__DOT__dut__DOT____Vlvbound_h3e9a9ec3__0 
        = vlSelfRef.test_gtext__DOT__dut__DOT__copper_pos[23U];
    vlSelfRef.test_gtext__DOT__dut__DOT__copper_pos_flat[12U] 
        = ((0xff00007fU & vlSelfRef.test_gtext__DOT__dut__DOT__copper_pos_flat[12U]) 
           | (test_gtext__DOT__dut__DOT____Vlvbound_h3e9a9ec3__0 
              << 7U));
    test_gtext__DOT__dut__DOT____Vlvbound_h3e9a9ec3__0 
        = vlSelfRef.test_gtext__DOT__dut__DOT__copper_pos[24U];
    vlSelfRef.test_gtext__DOT__dut__DOT__copper_pos_flat[12U] 
        = ((0x00ffffffU & vlSelfRef.test_gtext__DOT__dut__DOT__copper_pos_flat[12U]) 
           | (test_gtext__DOT__dut__DOT____Vlvbound_h3e9a9ec3__0 
              << 0x00000018U));
    vlSelfRef.test_gtext__DOT__dut__DOT__copper_pos_flat[13U] 
        = ((0xfffffe00U & vlSelfRef.test_gtext__DOT__dut__DOT__copper_pos_flat[13U]) 
           | (test_gtext__DOT__dut__DOT____Vlvbound_h3e9a9ec3__0 
              >> 8U));
    test_gtext__DOT__dut__DOT____Vlvbound_h3e9a9ec3__0 
        = vlSelfRef.test_gtext__DOT__dut__DOT__copper_pos[25U];
    vlSelfRef.test_gtext__DOT__dut__DOT__copper_pos_flat[13U] 
        = ((0xfc0001ffU & vlSelfRef.test_gtext__DOT__dut__DOT__copper_pos_flat[13U]) 
           | (test_gtext__DOT__dut__DOT____Vlvbound_h3e9a9ec3__0 
              << 9U));
    test_gtext__DOT__dut__DOT____Vlvbound_h3e9a9ec3__0 
        = vlSelfRef.test_gtext__DOT__dut__DOT__copper_pos[26U];
    vlSelfRef.test_gtext__DOT__dut__DOT__copper_pos_flat[13U] 
        = ((0x03ffffffU & vlSelfRef.test_gtext__DOT__dut__DOT__copper_pos_flat[13U]) 
           | (test_gtext__DOT__dut__DOT____Vlvbound_h3e9a9ec3__0 
              << 0x0000001aU));
    vlSelfRef.test_gtext__DOT__dut__DOT__copper_pos_flat[14U] 
        = ((0xfffff800U & vlSelfRef.test_gtext__DOT__dut__DOT__copper_pos_flat[14U]) 
           | (test_gtext__DOT__dut__DOT____Vlvbound_h3e9a9ec3__0 
              >> 6U));
    test_gtext__DOT__dut__DOT____Vlvbound_h3e9a9ec3__0 
        = vlSelfRef.test_gtext__DOT__dut__DOT__copper_pos[27U];
    vlSelfRef.test_gtext__DOT__dut__DOT__copper_pos_flat[14U] 
        = ((0xf00007ffU & vlSelfRef.test_gtext__DOT__dut__DOT__copper_pos_flat[14U]) 
           | (test_gtext__DOT__dut__DOT____Vlvbound_h3e9a9ec3__0 
              << 0x0000000bU));
    test_gtext__DOT__dut__DOT____Vlvbound_h3e9a9ec3__0 
        = vlSelfRef.test_gtext__DOT__dut__DOT__copper_pos[28U];
    vlSelfRef.test_gtext__DOT__dut__DOT__copper_pos_flat[14U] 
        = ((0x0fffffffU & vlSelfRef.test_gtext__DOT__dut__DOT__copper_pos_flat[14U]) 
           | (test_gtext__DOT__dut__DOT____Vlvbound_h3e9a9ec3__0 
              << 0x0000001cU));
    vlSelfRef.test_gtext__DOT__dut__DOT__copper_pos_flat[15U] 
        = ((0xffffe000U & vlSelfRef.test_gtext__DOT__dut__DOT__copper_pos_flat[15U]) 
           | (test_gtext__DOT__dut__DOT____Vlvbound_h3e9a9ec3__0 
              >> 4U));
    test_gtext__DOT__dut__DOT____Vlvbound_h3e9a9ec3__0 
        = vlSelfRef.test_gtext__DOT__dut__DOT__copper_pos[29U];
    vlSelfRef.test_gtext__DOT__dut__DOT__copper_pos_flat[15U] 
        = ((0xc0001fffU & vlSelfRef.test_gtext__DOT__dut__DOT__copper_pos_flat[15U]) 
           | (test_gtext__DOT__dut__DOT____Vlvbound_h3e9a9ec3__0 
              << 0x0000000dU));
    test_gtext__DOT__dut__DOT____Vlvbound_h3e9a9ec3__0 
        = vlSelfRef.test_gtext__DOT__dut__DOT__copper_pos[30U];
    vlSelfRef.test_gtext__DOT__dut__DOT__copper_pos_flat[15U] 
        = ((0x3fffffffU & vlSelfRef.test_gtext__DOT__dut__DOT__copper_pos_flat[15U]) 
           | (test_gtext__DOT__dut__DOT____Vlvbound_h3e9a9ec3__0 
              << 0x0000001eU));
    vlSelfRef.test_gtext__DOT__dut__DOT__copper_pos_flat[16U] 
        = ((0xffff8000U & vlSelfRef.test_gtext__DOT__dut__DOT__copper_pos_flat[16U]) 
           | (test_gtext__DOT__dut__DOT____Vlvbound_h3e9a9ec3__0 
              >> 2U));
    test_gtext__DOT__dut__DOT____Vlvbound_h3e9a9ec3__0 
        = vlSelfRef.test_gtext__DOT__dut__DOT__copper_pos[31U];
    vlSelfRef.test_gtext__DOT__dut__DOT__copper_pos_flat[16U] 
        = ((0x00007fffU & vlSelfRef.test_gtext__DOT__dut__DOT__copper_pos_flat[16U]) 
           | (test_gtext__DOT__dut__DOT____Vlvbound_h3e9a9ec3__0 
              << 0x0000000fU));
    vlSelfRef.test_gtext__DOT__dut__DOT__tile_inst__DOT__nt_we_a = 0U;
    test_gtext__DOT__dut__DOT__visible = ((0x0280U 
                                           > (IData)(vlSelfRef.test_gtext__DOT__dut__DOT__h_count)) 
                                          & (0x01e0U 
                                             > (IData)(vlSelfRef.test_gtext__DOT__dut__DOT__v_count)));
    vlSelfRef.test_gtext__DOT__dut__DOT__gfx_a_addr = 0U;
    if (vlSelfRef.test_gtext__DOT__dut__DOT__artist_gfx_we) {
        vlSelfRef.test_gtext__DOT__dut__DOT__gfx_a_we = 1U;
        vlSelfRef.test_gtext__DOT__dut__DOT__gfx_a_din 
            = vlSelfRef.test_gtext__DOT__dut__DOT__artist_gfx_wdata;
        vlSelfRef.test_gtext__DOT__dut__DOT__gfx_a_addr 
            = vlSelfRef.test_gtext__DOT__dut__DOT__artist_gfx_addr;
    } else if (vlSelfRef.test_gtext__DOT__dut__DOT__cmd_gfx_we) {
        vlSelfRef.test_gtext__DOT__dut__DOT__gfx_a_we = 1U;
        vlSelfRef.test_gtext__DOT__dut__DOT__gfx_a_din 
            = vlSelfRef.test_gtext__DOT__dut__DOT__cmd_gfx_din;
        vlSelfRef.test_gtext__DOT__dut__DOT__gfx_a_addr 
            = vlSelfRef.test_gtext__DOT__dut__DOT__cmd_gfx_addr;
    } else if (vlSelfRef.test_gtext__DOT__dut__DOT__artist_gfx_re) {
        vlSelfRef.test_gtext__DOT__dut__DOT__gfx_a_addr 
            = vlSelfRef.test_gtext__DOT__dut__DOT__artist_gfx_raddr;
    } else if (vlSelfRef.test_gtext__DOT__dut__DOT__cmd_gfx_re) {
        vlSelfRef.test_gtext__DOT__dut__DOT__gfx_a_addr 
            = vlSelfRef.test_gtext__DOT__dut__DOT__cmd_gfx_addr;
    }
    test_gtext__DOT__dut__DOT__text_line = (0x000003ffU 
                                            & (((IData)(vlSelfRef.test_gtext__DOT__dut__DOT__v_count) 
                                                - (IData)(0x00000028U)) 
                                               >> 1U));
    test_gtext__DOT__dut__DOT__text_row = (0x0000001fU 
                                           & ((IData)(test_gtext__DOT__dut__DOT__text_line) 
                                              >> 3U));
    test_gtext__DOT__dut__DOT__real_row = (0x0000001fU 
                                           & ((IData)(test_gtext__DOT__dut__DOT__text_row) 
                                              + (IData)(vlSelfRef.test_gtext__DOT__dut__DOT__scroll_offset)));
    if ((0x19U <= (IData)(test_gtext__DOT__dut__DOT__real_row))) {
        test_gtext__DOT__dut__DOT__real_row = (0x0000001fU 
                                               & ((IData)(test_gtext__DOT__dut__DOT__real_row) 
                                                  - (IData)(0x19U)));
    }
    vlSelfRef.test_gtext__DOT__dut__DOT__gfx_y = (0x000000ffU 
                                                  & (IData)(test_gtext__DOT__dut__DOT__text_line));
    test_gtext__DOT__dut__DOT__tile_blt_wdata = ((IData)(vlSelfRef.test_gtext__DOT__dut__DOT__vgc_tile_we)
                                                  ? (IData)(vlSelfRef.test_gtext__DOT__dut__DOT__vgc_tile_wdata)
                                                  : 0U);
    vlSelfRef.test_gtext__DOT__dut__DOT__tile_inst__DOT__nt_din_a = 0U;
    vlSelfRef.test_gtext__DOT__dut__DOT__tile_inst__DOT__at_addr_a = 0U;
    vlSelfRef.test_gtext__DOT__dut__DOT__tile_inst__DOT__nt_addr_a = 0U;
    vlSelfRef.test_gtext__DOT__dut__DOT__char_ram_sel 
        = ((0xaa00U <= (IData)(vlSelfRef.test_gtext__DOT__cpu_addr)) 
           & (0xb1cfU >= (IData)(vlSelfRef.test_gtext__DOT__cpu_addr)));
    vlSelfRef.test_gtext__DOT__dut__DOT__dbg_char_sel 
        = ((0xaa00U <= (IData)(vlSelfRef.test_gtext__DOT__dut__DOT__dbg_addr)) 
           & (0xb1cfU >= (IData)(vlSelfRef.test_gtext__DOT__dut__DOT__dbg_addr)));
    vlSelfRef.test_gtext__DOT__dut__DOT__color_ram_sel 
        = ((0xb1d0U <= (IData)(vlSelfRef.test_gtext__DOT__cpu_addr)) 
           & (0xb99fU >= (IData)(vlSelfRef.test_gtext__DOT__cpu_addr)));
    vlSelfRef.test_gtext__DOT__dut__DOT__dbg_color_sel 
        = ((0xb1d0U <= (IData)(vlSelfRef.test_gtext__DOT__dut__DOT__dbg_addr)) 
           & (0xb99fU >= (IData)(vlSelfRef.test_gtext__DOT__dut__DOT__dbg_addr)));
    test_gtext__DOT__dut__DOT__tile_blt_addr = (((IData)(vlSelfRef.test_gtext__DOT__dut__DOT__vgc_tile_re) 
                                                 | (IData)(vlSelfRef.test_gtext__DOT__dut__DOT__vgc_tile_we))
                                                 ? (IData)(vlSelfRef.test_gtext__DOT__dut__DOT__vgc_tile_addr)
                                                 : 0U);
    __VdfgRegularize_hebeb780c_0_0 = ((IData)(vlSelfRef.test_gtext__DOT__cpu_ce) 
                                      & (IData)(vlSelfRef.test_gtext__DOT__cpu_re));
    vlSelfRef.test_gtext__DOT__dut__DOT__copper_inst__DOT__copper_reg[0U] 
        = (0x000000ffU & vlSelfRef.test_gtext__DOT__dut__DOT__copper_reg_flat[0U]);
    vlSelfRef.test_gtext__DOT__dut__DOT__copper_inst__DOT__copper_reg[1U] 
        = (0x000000ffU & (vlSelfRef.test_gtext__DOT__dut__DOT__copper_reg_flat[0U] 
                          >> 8U));
    vlSelfRef.test_gtext__DOT__dut__DOT__copper_inst__DOT__copper_reg[2U] 
        = (0x000000ffU & (vlSelfRef.test_gtext__DOT__dut__DOT__copper_reg_flat[0U] 
                          >> 0x00000010U));
    vlSelfRef.test_gtext__DOT__dut__DOT__copper_inst__DOT__copper_reg[3U] 
        = (vlSelfRef.test_gtext__DOT__dut__DOT__copper_reg_flat[0U] 
           >> 0x00000018U);
    vlSelfRef.test_gtext__DOT__dut__DOT__copper_inst__DOT__copper_reg[4U] 
        = (0x000000ffU & vlSelfRef.test_gtext__DOT__dut__DOT__copper_reg_flat[1U]);
    vlSelfRef.test_gtext__DOT__dut__DOT__copper_inst__DOT__copper_reg[5U] 
        = (0x000000ffU & (vlSelfRef.test_gtext__DOT__dut__DOT__copper_reg_flat[1U] 
                          >> 8U));
    vlSelfRef.test_gtext__DOT__dut__DOT__copper_inst__DOT__copper_reg[6U] 
        = (0x000000ffU & (vlSelfRef.test_gtext__DOT__dut__DOT__copper_reg_flat[1U] 
                          >> 0x00000010U));
    vlSelfRef.test_gtext__DOT__dut__DOT__copper_inst__DOT__copper_reg[7U] 
        = (vlSelfRef.test_gtext__DOT__dut__DOT__copper_reg_flat[1U] 
           >> 0x00000018U);
    vlSelfRef.test_gtext__DOT__dut__DOT__copper_inst__DOT__copper_reg[8U] 
        = (0x000000ffU & vlSelfRef.test_gtext__DOT__dut__DOT__copper_reg_flat[2U]);
    vlSelfRef.test_gtext__DOT__dut__DOT__copper_inst__DOT__copper_reg[9U] 
        = (0x000000ffU & (vlSelfRef.test_gtext__DOT__dut__DOT__copper_reg_flat[2U] 
                          >> 8U));
    vlSelfRef.test_gtext__DOT__dut__DOT__copper_inst__DOT__copper_reg[10U] 
        = (0x000000ffU & (vlSelfRef.test_gtext__DOT__dut__DOT__copper_reg_flat[2U] 
                          >> 0x00000010U));
    vlSelfRef.test_gtext__DOT__dut__DOT__copper_inst__DOT__copper_reg[11U] 
        = (vlSelfRef.test_gtext__DOT__dut__DOT__copper_reg_flat[2U] 
           >> 0x00000018U);
    vlSelfRef.test_gtext__DOT__dut__DOT__copper_inst__DOT__copper_reg[12U] 
        = (0x000000ffU & vlSelfRef.test_gtext__DOT__dut__DOT__copper_reg_flat[3U]);
    vlSelfRef.test_gtext__DOT__dut__DOT__copper_inst__DOT__copper_reg[13U] 
        = (0x000000ffU & (vlSelfRef.test_gtext__DOT__dut__DOT__copper_reg_flat[3U] 
                          >> 8U));
    vlSelfRef.test_gtext__DOT__dut__DOT__copper_inst__DOT__copper_reg[14U] 
        = (0x000000ffU & (vlSelfRef.test_gtext__DOT__dut__DOT__copper_reg_flat[3U] 
                          >> 0x00000010U));
    vlSelfRef.test_gtext__DOT__dut__DOT__copper_inst__DOT__copper_reg[15U] 
        = (vlSelfRef.test_gtext__DOT__dut__DOT__copper_reg_flat[3U] 
           >> 0x00000018U);
    vlSelfRef.test_gtext__DOT__dut__DOT__copper_inst__DOT__copper_reg[16U] 
        = (0x000000ffU & vlSelfRef.test_gtext__DOT__dut__DOT__copper_reg_flat[4U]);
    vlSelfRef.test_gtext__DOT__dut__DOT__copper_inst__DOT__copper_reg[17U] 
        = (0x000000ffU & (vlSelfRef.test_gtext__DOT__dut__DOT__copper_reg_flat[4U] 
                          >> 8U));
    vlSelfRef.test_gtext__DOT__dut__DOT__copper_inst__DOT__copper_reg[18U] 
        = (0x000000ffU & (vlSelfRef.test_gtext__DOT__dut__DOT__copper_reg_flat[4U] 
                          >> 0x00000010U));
    vlSelfRef.test_gtext__DOT__dut__DOT__copper_inst__DOT__copper_reg[19U] 
        = (vlSelfRef.test_gtext__DOT__dut__DOT__copper_reg_flat[4U] 
           >> 0x00000018U);
    vlSelfRef.test_gtext__DOT__dut__DOT__copper_inst__DOT__copper_reg[20U] 
        = (0x000000ffU & vlSelfRef.test_gtext__DOT__dut__DOT__copper_reg_flat[5U]);
    vlSelfRef.test_gtext__DOT__dut__DOT__copper_inst__DOT__copper_reg[21U] 
        = (0x000000ffU & (vlSelfRef.test_gtext__DOT__dut__DOT__copper_reg_flat[5U] 
                          >> 8U));
    vlSelfRef.test_gtext__DOT__dut__DOT__copper_inst__DOT__copper_reg[22U] 
        = (0x000000ffU & (vlSelfRef.test_gtext__DOT__dut__DOT__copper_reg_flat[5U] 
                          >> 0x00000010U));
    vlSelfRef.test_gtext__DOT__dut__DOT__copper_inst__DOT__copper_reg[23U] 
        = (vlSelfRef.test_gtext__DOT__dut__DOT__copper_reg_flat[5U] 
           >> 0x00000018U);
    vlSelfRef.test_gtext__DOT__dut__DOT__copper_inst__DOT__copper_reg[24U] 
        = (0x000000ffU & vlSelfRef.test_gtext__DOT__dut__DOT__copper_reg_flat[6U]);
    vlSelfRef.test_gtext__DOT__dut__DOT__copper_inst__DOT__copper_reg[25U] 
        = (0x000000ffU & (vlSelfRef.test_gtext__DOT__dut__DOT__copper_reg_flat[6U] 
                          >> 8U));
    vlSelfRef.test_gtext__DOT__dut__DOT__copper_inst__DOT__copper_reg[26U] 
        = (0x000000ffU & (vlSelfRef.test_gtext__DOT__dut__DOT__copper_reg_flat[6U] 
                          >> 0x00000010U));
    vlSelfRef.test_gtext__DOT__dut__DOT__copper_inst__DOT__copper_reg[27U] 
        = (vlSelfRef.test_gtext__DOT__dut__DOT__copper_reg_flat[6U] 
           >> 0x00000018U);
    vlSelfRef.test_gtext__DOT__dut__DOT__copper_inst__DOT__copper_reg[28U] 
        = (0x000000ffU & vlSelfRef.test_gtext__DOT__dut__DOT__copper_reg_flat[7U]);
    vlSelfRef.test_gtext__DOT__dut__DOT__copper_inst__DOT__copper_reg[29U] 
        = (0x000000ffU & (vlSelfRef.test_gtext__DOT__dut__DOT__copper_reg_flat[7U] 
                          >> 8U));
    vlSelfRef.test_gtext__DOT__dut__DOT__copper_inst__DOT__copper_reg[30U] 
        = (0x000000ffU & (vlSelfRef.test_gtext__DOT__dut__DOT__copper_reg_flat[7U] 
                          >> 0x00000010U));
    vlSelfRef.test_gtext__DOT__dut__DOT__copper_inst__DOT__copper_reg[31U] 
        = (vlSelfRef.test_gtext__DOT__dut__DOT__copper_reg_flat[7U] 
           >> 0x00000018U);
    vlSelfRef.test_gtext__DOT__dut__DOT__copper_inst__DOT__copper_val[0U] 
        = (0x000000ffU & vlSelfRef.test_gtext__DOT__dut__DOT__copper_val_flat[0U]);
    vlSelfRef.test_gtext__DOT__dut__DOT__copper_inst__DOT__copper_val[1U] 
        = (0x000000ffU & (vlSelfRef.test_gtext__DOT__dut__DOT__copper_val_flat[0U] 
                          >> 8U));
    vlSelfRef.test_gtext__DOT__dut__DOT__copper_inst__DOT__copper_val[2U] 
        = (0x000000ffU & (vlSelfRef.test_gtext__DOT__dut__DOT__copper_val_flat[0U] 
                          >> 0x00000010U));
    vlSelfRef.test_gtext__DOT__dut__DOT__copper_inst__DOT__copper_val[3U] 
        = (vlSelfRef.test_gtext__DOT__dut__DOT__copper_val_flat[0U] 
           >> 0x00000018U);
    vlSelfRef.test_gtext__DOT__dut__DOT__copper_inst__DOT__copper_val[4U] 
        = (0x000000ffU & vlSelfRef.test_gtext__DOT__dut__DOT__copper_val_flat[1U]);
    vlSelfRef.test_gtext__DOT__dut__DOT__copper_inst__DOT__copper_val[5U] 
        = (0x000000ffU & (vlSelfRef.test_gtext__DOT__dut__DOT__copper_val_flat[1U] 
                          >> 8U));
    vlSelfRef.test_gtext__DOT__dut__DOT__copper_inst__DOT__copper_val[6U] 
        = (0x000000ffU & (vlSelfRef.test_gtext__DOT__dut__DOT__copper_val_flat[1U] 
                          >> 0x00000010U));
    vlSelfRef.test_gtext__DOT__dut__DOT__copper_inst__DOT__copper_val[7U] 
        = (vlSelfRef.test_gtext__DOT__dut__DOT__copper_val_flat[1U] 
           >> 0x00000018U);
    vlSelfRef.test_gtext__DOT__dut__DOT__copper_inst__DOT__copper_val[8U] 
        = (0x000000ffU & vlSelfRef.test_gtext__DOT__dut__DOT__copper_val_flat[2U]);
    vlSelfRef.test_gtext__DOT__dut__DOT__copper_inst__DOT__copper_val[9U] 
        = (0x000000ffU & (vlSelfRef.test_gtext__DOT__dut__DOT__copper_val_flat[2U] 
                          >> 8U));
    vlSelfRef.test_gtext__DOT__dut__DOT__copper_inst__DOT__copper_val[10U] 
        = (0x000000ffU & (vlSelfRef.test_gtext__DOT__dut__DOT__copper_val_flat[2U] 
                          >> 0x00000010U));
    vlSelfRef.test_gtext__DOT__dut__DOT__copper_inst__DOT__copper_val[11U] 
        = (vlSelfRef.test_gtext__DOT__dut__DOT__copper_val_flat[2U] 
           >> 0x00000018U);
    vlSelfRef.test_gtext__DOT__dut__DOT__copper_inst__DOT__copper_val[12U] 
        = (0x000000ffU & vlSelfRef.test_gtext__DOT__dut__DOT__copper_val_flat[3U]);
    vlSelfRef.test_gtext__DOT__dut__DOT__copper_inst__DOT__copper_val[13U] 
        = (0x000000ffU & (vlSelfRef.test_gtext__DOT__dut__DOT__copper_val_flat[3U] 
                          >> 8U));
    vlSelfRef.test_gtext__DOT__dut__DOT__copper_inst__DOT__copper_val[14U] 
        = (0x000000ffU & (vlSelfRef.test_gtext__DOT__dut__DOT__copper_val_flat[3U] 
                          >> 0x00000010U));
    vlSelfRef.test_gtext__DOT__dut__DOT__copper_inst__DOT__copper_val[15U] 
        = (vlSelfRef.test_gtext__DOT__dut__DOT__copper_val_flat[3U] 
           >> 0x00000018U);
    vlSelfRef.test_gtext__DOT__dut__DOT__copper_inst__DOT__copper_val[16U] 
        = (0x000000ffU & vlSelfRef.test_gtext__DOT__dut__DOT__copper_val_flat[4U]);
    vlSelfRef.test_gtext__DOT__dut__DOT__copper_inst__DOT__copper_val[17U] 
        = (0x000000ffU & (vlSelfRef.test_gtext__DOT__dut__DOT__copper_val_flat[4U] 
                          >> 8U));
    vlSelfRef.test_gtext__DOT__dut__DOT__copper_inst__DOT__copper_val[18U] 
        = (0x000000ffU & (vlSelfRef.test_gtext__DOT__dut__DOT__copper_val_flat[4U] 
                          >> 0x00000010U));
    vlSelfRef.test_gtext__DOT__dut__DOT__copper_inst__DOT__copper_val[19U] 
        = (vlSelfRef.test_gtext__DOT__dut__DOT__copper_val_flat[4U] 
           >> 0x00000018U);
    vlSelfRef.test_gtext__DOT__dut__DOT__copper_inst__DOT__copper_val[20U] 
        = (0x000000ffU & vlSelfRef.test_gtext__DOT__dut__DOT__copper_val_flat[5U]);
    vlSelfRef.test_gtext__DOT__dut__DOT__copper_inst__DOT__copper_val[21U] 
        = (0x000000ffU & (vlSelfRef.test_gtext__DOT__dut__DOT__copper_val_flat[5U] 
                          >> 8U));
    vlSelfRef.test_gtext__DOT__dut__DOT__copper_inst__DOT__copper_val[22U] 
        = (0x000000ffU & (vlSelfRef.test_gtext__DOT__dut__DOT__copper_val_flat[5U] 
                          >> 0x00000010U));
    vlSelfRef.test_gtext__DOT__dut__DOT__copper_inst__DOT__copper_val[23U] 
        = (vlSelfRef.test_gtext__DOT__dut__DOT__copper_val_flat[5U] 
           >> 0x00000018U);
    vlSelfRef.test_gtext__DOT__dut__DOT__copper_inst__DOT__copper_val[24U] 
        = (0x000000ffU & vlSelfRef.test_gtext__DOT__dut__DOT__copper_val_flat[6U]);
    vlSelfRef.test_gtext__DOT__dut__DOT__copper_inst__DOT__copper_val[25U] 
        = (0x000000ffU & (vlSelfRef.test_gtext__DOT__dut__DOT__copper_val_flat[6U] 
                          >> 8U));
    vlSelfRef.test_gtext__DOT__dut__DOT__copper_inst__DOT__copper_val[26U] 
        = (0x000000ffU & (vlSelfRef.test_gtext__DOT__dut__DOT__copper_val_flat[6U] 
                          >> 0x00000010U));
    vlSelfRef.test_gtext__DOT__dut__DOT__copper_inst__DOT__copper_val[27U] 
        = (vlSelfRef.test_gtext__DOT__dut__DOT__copper_val_flat[6U] 
           >> 0x00000018U);
    vlSelfRef.test_gtext__DOT__dut__DOT__copper_inst__DOT__copper_val[28U] 
        = (0x000000ffU & vlSelfRef.test_gtext__DOT__dut__DOT__copper_val_flat[7U]);
    vlSelfRef.test_gtext__DOT__dut__DOT__copper_inst__DOT__copper_val[29U] 
        = (0x000000ffU & (vlSelfRef.test_gtext__DOT__dut__DOT__copper_val_flat[7U] 
                          >> 8U));
    vlSelfRef.test_gtext__DOT__dut__DOT__copper_inst__DOT__copper_val[30U] 
        = (0x000000ffU & (vlSelfRef.test_gtext__DOT__dut__DOT__copper_val_flat[7U] 
                          >> 0x00000010U));
    vlSelfRef.test_gtext__DOT__dut__DOT__copper_inst__DOT__copper_val[31U] 
        = (vlSelfRef.test_gtext__DOT__dut__DOT__copper_val_flat[7U] 
           >> 0x00000018U);
    vlSelfRef.test_gtext__DOT__dut__DOT__copper_inst__DOT__copper_pos[0U] 
        = (0x0001ffffU & vlSelfRef.test_gtext__DOT__dut__DOT__copper_pos_flat[0U]);
    vlSelfRef.test_gtext__DOT__dut__DOT__copper_inst__DOT__copper_pos[1U] 
        = (0x0001ffffU & ((vlSelfRef.test_gtext__DOT__dut__DOT__copper_pos_flat[1U] 
                           << 0x0000000fU) | (vlSelfRef.test_gtext__DOT__dut__DOT__copper_pos_flat[0U] 
                                              >> 0x00000011U)));
    vlSelfRef.test_gtext__DOT__dut__DOT__copper_inst__DOT__copper_pos[2U] 
        = (0x0001ffffU & (vlSelfRef.test_gtext__DOT__dut__DOT__copper_pos_flat[1U] 
                          >> 2U));
    vlSelfRef.test_gtext__DOT__dut__DOT__copper_inst__DOT__copper_pos[3U] 
        = (0x0001ffffU & ((vlSelfRef.test_gtext__DOT__dut__DOT__copper_pos_flat[2U] 
                           << 0x0000000dU) | (vlSelfRef.test_gtext__DOT__dut__DOT__copper_pos_flat[1U] 
                                              >> 0x00000013U)));
    vlSelfRef.test_gtext__DOT__dut__DOT__copper_inst__DOT__copper_pos[4U] 
        = (0x0001ffffU & (vlSelfRef.test_gtext__DOT__dut__DOT__copper_pos_flat[2U] 
                          >> 4U));
    vlSelfRef.test_gtext__DOT__dut__DOT__copper_inst__DOT__copper_pos[5U] 
        = (0x0001ffffU & ((vlSelfRef.test_gtext__DOT__dut__DOT__copper_pos_flat[3U] 
                           << 0x0000000bU) | (vlSelfRef.test_gtext__DOT__dut__DOT__copper_pos_flat[2U] 
                                              >> 0x00000015U)));
    vlSelfRef.test_gtext__DOT__dut__DOT__copper_inst__DOT__copper_pos[6U] 
        = (0x0001ffffU & (vlSelfRef.test_gtext__DOT__dut__DOT__copper_pos_flat[3U] 
                          >> 6U));
    vlSelfRef.test_gtext__DOT__dut__DOT__copper_inst__DOT__copper_pos[7U] 
        = (0x0001ffffU & ((vlSelfRef.test_gtext__DOT__dut__DOT__copper_pos_flat[4U] 
                           << 9U) | (vlSelfRef.test_gtext__DOT__dut__DOT__copper_pos_flat[3U] 
                                     >> 0x00000017U)));
    vlSelfRef.test_gtext__DOT__dut__DOT__copper_inst__DOT__copper_pos[8U] 
        = (0x0001ffffU & (vlSelfRef.test_gtext__DOT__dut__DOT__copper_pos_flat[4U] 
                          >> 8U));
    vlSelfRef.test_gtext__DOT__dut__DOT__copper_inst__DOT__copper_pos[9U] 
        = (0x0001ffffU & ((vlSelfRef.test_gtext__DOT__dut__DOT__copper_pos_flat[5U] 
                           << 7U) | (vlSelfRef.test_gtext__DOT__dut__DOT__copper_pos_flat[4U] 
                                     >> 0x00000019U)));
    vlSelfRef.test_gtext__DOT__dut__DOT__copper_inst__DOT__copper_pos[10U] 
        = (0x0001ffffU & (vlSelfRef.test_gtext__DOT__dut__DOT__copper_pos_flat[5U] 
                          >> 0x0000000aU));
    vlSelfRef.test_gtext__DOT__dut__DOT__copper_inst__DOT__copper_pos[11U] 
        = (0x0001ffffU & ((vlSelfRef.test_gtext__DOT__dut__DOT__copper_pos_flat[6U] 
                           << 5U) | (vlSelfRef.test_gtext__DOT__dut__DOT__copper_pos_flat[5U] 
                                     >> 0x0000001bU)));
    vlSelfRef.test_gtext__DOT__dut__DOT__copper_inst__DOT__copper_pos[12U] 
        = (0x0001ffffU & (vlSelfRef.test_gtext__DOT__dut__DOT__copper_pos_flat[6U] 
                          >> 0x0000000cU));
    vlSelfRef.test_gtext__DOT__dut__DOT__copper_inst__DOT__copper_pos[13U] 
        = (0x0001ffffU & ((vlSelfRef.test_gtext__DOT__dut__DOT__copper_pos_flat[7U] 
                           << 3U) | (vlSelfRef.test_gtext__DOT__dut__DOT__copper_pos_flat[6U] 
                                     >> 0x0000001dU)));
    vlSelfRef.test_gtext__DOT__dut__DOT__copper_inst__DOT__copper_pos[14U] 
        = (0x0001ffffU & (vlSelfRef.test_gtext__DOT__dut__DOT__copper_pos_flat[7U] 
                          >> 0x0000000eU));
    vlSelfRef.test_gtext__DOT__dut__DOT__copper_inst__DOT__copper_pos[15U] 
        = (0x0001ffffU & ((vlSelfRef.test_gtext__DOT__dut__DOT__copper_pos_flat[8U] 
                           << 1U) | (vlSelfRef.test_gtext__DOT__dut__DOT__copper_pos_flat[7U] 
                                     >> 0x0000001fU)));
    vlSelfRef.test_gtext__DOT__dut__DOT__copper_inst__DOT__copper_pos[16U] 
        = (0x0001ffffU & ((vlSelfRef.test_gtext__DOT__dut__DOT__copper_pos_flat[9U] 
                           << 0x00000010U) | (vlSelfRef.test_gtext__DOT__dut__DOT__copper_pos_flat[8U] 
                                              >> 0x00000010U)));
    vlSelfRef.test_gtext__DOT__dut__DOT__copper_inst__DOT__copper_pos[17U] 
        = (0x0001ffffU & (vlSelfRef.test_gtext__DOT__dut__DOT__copper_pos_flat[9U] 
                          >> 1U));
    vlSelfRef.test_gtext__DOT__dut__DOT__copper_inst__DOT__copper_pos[18U] 
        = (0x0001ffffU & ((vlSelfRef.test_gtext__DOT__dut__DOT__copper_pos_flat[10U] 
                           << 0x0000000eU) | (vlSelfRef.test_gtext__DOT__dut__DOT__copper_pos_flat[9U] 
                                              >> 0x00000012U)));
    vlSelfRef.test_gtext__DOT__dut__DOT__copper_inst__DOT__copper_pos[19U] 
        = (0x0001ffffU & (vlSelfRef.test_gtext__DOT__dut__DOT__copper_pos_flat[10U] 
                          >> 3U));
    vlSelfRef.test_gtext__DOT__dut__DOT__copper_inst__DOT__copper_pos[20U] 
        = (0x0001ffffU & ((vlSelfRef.test_gtext__DOT__dut__DOT__copper_pos_flat[11U] 
                           << 0x0000000cU) | (vlSelfRef.test_gtext__DOT__dut__DOT__copper_pos_flat[10U] 
                                              >> 0x00000014U)));
    vlSelfRef.test_gtext__DOT__dut__DOT__copper_inst__DOT__copper_pos[21U] 
        = (0x0001ffffU & (vlSelfRef.test_gtext__DOT__dut__DOT__copper_pos_flat[11U] 
                          >> 5U));
    vlSelfRef.test_gtext__DOT__dut__DOT__copper_inst__DOT__copper_pos[22U] 
        = (0x0001ffffU & ((vlSelfRef.test_gtext__DOT__dut__DOT__copper_pos_flat[12U] 
                           << 0x0000000aU) | (vlSelfRef.test_gtext__DOT__dut__DOT__copper_pos_flat[11U] 
                                              >> 0x00000016U)));
    vlSelfRef.test_gtext__DOT__dut__DOT__copper_inst__DOT__copper_pos[23U] 
        = (0x0001ffffU & (vlSelfRef.test_gtext__DOT__dut__DOT__copper_pos_flat[12U] 
                          >> 7U));
    vlSelfRef.test_gtext__DOT__dut__DOT__copper_inst__DOT__copper_pos[24U] 
        = (0x0001ffffU & ((vlSelfRef.test_gtext__DOT__dut__DOT__copper_pos_flat[13U] 
                           << 8U) | (vlSelfRef.test_gtext__DOT__dut__DOT__copper_pos_flat[12U] 
                                     >> 0x00000018U)));
    vlSelfRef.test_gtext__DOT__dut__DOT__copper_inst__DOT__copper_pos[25U] 
        = (0x0001ffffU & (vlSelfRef.test_gtext__DOT__dut__DOT__copper_pos_flat[13U] 
                          >> 9U));
    vlSelfRef.test_gtext__DOT__dut__DOT__copper_inst__DOT__copper_pos[26U] 
        = (0x0001ffffU & ((vlSelfRef.test_gtext__DOT__dut__DOT__copper_pos_flat[14U] 
                           << 6U) | (vlSelfRef.test_gtext__DOT__dut__DOT__copper_pos_flat[13U] 
                                     >> 0x0000001aU)));
    vlSelfRef.test_gtext__DOT__dut__DOT__copper_inst__DOT__copper_pos[27U] 
        = (0x0001ffffU & (vlSelfRef.test_gtext__DOT__dut__DOT__copper_pos_flat[14U] 
                          >> 0x0000000bU));
    vlSelfRef.test_gtext__DOT__dut__DOT__copper_inst__DOT__copper_pos[28U] 
        = (0x0001ffffU & ((vlSelfRef.test_gtext__DOT__dut__DOT__copper_pos_flat[15U] 
                           << 4U) | (vlSelfRef.test_gtext__DOT__dut__DOT__copper_pos_flat[14U] 
                                     >> 0x0000001cU)));
    vlSelfRef.test_gtext__DOT__dut__DOT__copper_inst__DOT__copper_pos[29U] 
        = (0x0001ffffU & (vlSelfRef.test_gtext__DOT__dut__DOT__copper_pos_flat[15U] 
                          >> 0x0000000dU));
    vlSelfRef.test_gtext__DOT__dut__DOT__copper_inst__DOT__copper_pos[30U] 
        = (0x0001ffffU & ((vlSelfRef.test_gtext__DOT__dut__DOT__copper_pos_flat[16U] 
                           << 2U) | (vlSelfRef.test_gtext__DOT__dut__DOT__copper_pos_flat[15U] 
                                     >> 0x0000001eU)));
    vlSelfRef.test_gtext__DOT__dut__DOT__copper_inst__DOT__copper_pos[31U] 
        = (vlSelfRef.test_gtext__DOT__dut__DOT__copper_pos_flat[16U] 
           >> 0x0000000fU);
    vlSelfRef.test_gtext__DOT__dut__DOT__in_text_area 
        = ((IData)(test_gtext__DOT__dut__DOT__visible) 
           & ((0x0028U <= (IData)(vlSelfRef.test_gtext__DOT__dut__DOT__v_count)) 
              & (0x01b8U > (IData)(vlSelfRef.test_gtext__DOT__dut__DOT__v_count))));
    vlSelfRef.test_gtext__DOT__dut__DOT__tile_inst__DOT__td_din_a = 0U;
    vlSelfRef.test_gtext__DOT__dut__DOT__tile_inst__DOT__td_addr_a = 0U;
    if ((0x00000010U & (IData)(vlSelfRef.test_gtext__DOT__dut__DOT__tile_inst__DOT__dma_state))) {
        if (vlSelfRef.test_gtext__DOT__dut__DOT__vgc_tile_we) {
            vlSelfRef.test_gtext__DOT__dut__DOT__tile_inst__DOT__td_we_a = 1U;
            vlSelfRef.test_gtext__DOT__dut__DOT__tile_inst__DOT__td_din_a 
                = test_gtext__DOT__dut__DOT__tile_blt_wdata;
            vlSelfRef.test_gtext__DOT__dut__DOT__tile_inst__DOT__td_addr_a 
                = test_gtext__DOT__dut__DOT__tile_blt_addr;
        } else if (vlSelfRef.test_gtext__DOT__dut__DOT__vgc_tile_re) {
            vlSelfRef.test_gtext__DOT__dut__DOT__tile_inst__DOT__td_addr_a 
                = test_gtext__DOT__dut__DOT__tile_blt_addr;
        }
        if (vlSelfRef.test_gtext__DOT__dut__DOT__tile_inst__DOT__cmd_at_we) {
            vlSelfRef.test_gtext__DOT__dut__DOT__tile_inst__DOT__at_we_a = 1U;
            vlSelfRef.test_gtext__DOT__dut__DOT__tile_inst__DOT__at_din_a 
                = vlSelfRef.test_gtext__DOT__dut__DOT__tile_inst__DOT__cmd_at_din;
            vlSelfRef.test_gtext__DOT__dut__DOT__tile_inst__DOT__at_addr_a 
                = vlSelfRef.test_gtext__DOT__dut__DOT__tile_inst__DOT__cmd_at_addr;
        }
        if (vlSelfRef.test_gtext__DOT__dut__DOT__tile_inst__DOT__cmd_nt_we) {
            vlSelfRef.test_gtext__DOT__dut__DOT__tile_inst__DOT__nt_we_a = 1U;
            vlSelfRef.test_gtext__DOT__dut__DOT__tile_inst__DOT__nt_din_a 
                = vlSelfRef.test_gtext__DOT__dut__DOT__tile_inst__DOT__cmd_nt_din;
            vlSelfRef.test_gtext__DOT__dut__DOT__tile_inst__DOT__nt_addr_a 
                = vlSelfRef.test_gtext__DOT__dut__DOT__tile_inst__DOT__cmd_nt_addr;
        }
    } else if ((8U & (IData)(vlSelfRef.test_gtext__DOT__dut__DOT__tile_inst__DOT__dma_state))) {
        if ((4U & (IData)(vlSelfRef.test_gtext__DOT__dut__DOT__tile_inst__DOT__dma_state))) {
            if ((2U & (IData)(vlSelfRef.test_gtext__DOT__dut__DOT__tile_inst__DOT__dma_state))) {
                if ((1U & (IData)(vlSelfRef.test_gtext__DOT__dut__DOT__tile_inst__DOT__dma_state))) {
                    if (vlSelfRef.test_gtext__DOT__dut__DOT__vgc_tile_we) {
                        vlSelfRef.test_gtext__DOT__dut__DOT__tile_inst__DOT__td_we_a = 1U;
                        vlSelfRef.test_gtext__DOT__dut__DOT__tile_inst__DOT__td_din_a 
                            = test_gtext__DOT__dut__DOT__tile_blt_wdata;
                        vlSelfRef.test_gtext__DOT__dut__DOT__tile_inst__DOT__td_addr_a 
                            = test_gtext__DOT__dut__DOT__tile_blt_addr;
                    } else if (vlSelfRef.test_gtext__DOT__dut__DOT__vgc_tile_re) {
                        vlSelfRef.test_gtext__DOT__dut__DOT__tile_inst__DOT__td_addr_a 
                            = test_gtext__DOT__dut__DOT__tile_blt_addr;
                    }
                    if (vlSelfRef.test_gtext__DOT__dut__DOT__tile_inst__DOT__cmd_at_we) {
                        vlSelfRef.test_gtext__DOT__dut__DOT__tile_inst__DOT__at_we_a = 1U;
                        vlSelfRef.test_gtext__DOT__dut__DOT__tile_inst__DOT__at_din_a 
                            = vlSelfRef.test_gtext__DOT__dut__DOT__tile_inst__DOT__cmd_at_din;
                        vlSelfRef.test_gtext__DOT__dut__DOT__tile_inst__DOT__at_addr_a 
                            = vlSelfRef.test_gtext__DOT__dut__DOT__tile_inst__DOT__cmd_at_addr;
                    }
                    if (vlSelfRef.test_gtext__DOT__dut__DOT__tile_inst__DOT__cmd_nt_we) {
                        vlSelfRef.test_gtext__DOT__dut__DOT__tile_inst__DOT__nt_we_a = 1U;
                        vlSelfRef.test_gtext__DOT__dut__DOT__tile_inst__DOT__nt_din_a 
                            = vlSelfRef.test_gtext__DOT__dut__DOT__tile_inst__DOT__cmd_nt_din;
                        vlSelfRef.test_gtext__DOT__dut__DOT__tile_inst__DOT__nt_addr_a 
                            = vlSelfRef.test_gtext__DOT__dut__DOT__tile_inst__DOT__cmd_nt_addr;
                    }
                }
            } else {
                vlSelfRef.test_gtext__DOT__dut__DOT__tile_inst__DOT__at_addr_a 
                    = vlSelfRef.test_gtext__DOT__dut__DOT__tile_inst__DOT__cmd_at_addr;
                vlSelfRef.test_gtext__DOT__dut__DOT__tile_inst__DOT__nt_addr_a 
                    = vlSelfRef.test_gtext__DOT__dut__DOT__tile_inst__DOT__cmd_nt_addr;
            }
        } else {
            if ((1U & (~ ((IData)(vlSelfRef.test_gtext__DOT__dut__DOT__tile_inst__DOT__dma_state) 
                          >> 1U)))) {
                if ((1U & (IData)(vlSelfRef.test_gtext__DOT__dut__DOT__tile_inst__DOT__dma_state))) {
                    if ((0x0fa0U > (IData)(vlSelfRef.test_gtext__DOT__dut__DOT__tile_inst__DOT__dma_fill_addr))) {
                        vlSelfRef.test_gtext__DOT__dut__DOT__tile_inst__DOT__at_we_a = 1U;
                        vlSelfRef.test_gtext__DOT__dut__DOT__tile_inst__DOT__at_din_a = 0U;
                        vlSelfRef.test_gtext__DOT__dut__DOT__tile_inst__DOT__at_addr_a 
                            = vlSelfRef.test_gtext__DOT__dut__DOT__tile_inst__DOT__dma_fill_addr;
                    }
                } else if (vlSelfRef.test_gtext__DOT__dut__DOT__tile_inst__DOT__dma_data_valid) {
                    vlSelfRef.test_gtext__DOT__dut__DOT__tile_inst__DOT__at_we_a = 1U;
                    vlSelfRef.test_gtext__DOT__dut__DOT__tile_inst__DOT__at_din_a = 0U;
                    vlSelfRef.test_gtext__DOT__dut__DOT__tile_inst__DOT__at_addr_a 
                        = (0x00000fffU & ((((IData)(0x000003e8U) 
                                            * (IData)(vlSelfRef.test_gtext__DOT__dut__DOT__tile_inst__DOT__dma_nt)) 
                                           + ((IData)(0x0028U) 
                                              * (IData)(vlSelfRef.test_gtext__DOT__dut__DOT__tile_inst__DOT__dma_col_idx))) 
                                          + (IData)(vlSelfRef.test_gtext__DOT__dut__DOT__tile_inst__DOT__dma_col_row)));
                }
            }
            if ((2U & (IData)(vlSelfRef.test_gtext__DOT__dut__DOT__tile_inst__DOT__dma_state))) {
                if ((1U & (IData)(vlSelfRef.test_gtext__DOT__dut__DOT__tile_inst__DOT__dma_state))) {
                    if ((0x19U > (IData)(vlSelfRef.test_gtext__DOT__dut__DOT__tile_inst__DOT__dma_col_idx))) {
                        vlSelfRef.test_gtext__DOT__dut__DOT__tile_inst__DOT__nt_we_a = 1U;
                        vlSelfRef.test_gtext__DOT__dut__DOT__tile_inst__DOT__nt_din_a 
                            = ((0x18U >= (IData)(vlSelfRef.test_gtext__DOT__dut__DOT__tile_inst__DOT__dma_col_idx))
                                ? vlSelfRef.test_gtext__DOT__dut__DOT__tile_inst__DOT__col_buffer
                               [vlSelfRef.test_gtext__DOT__dut__DOT__tile_inst__DOT__dma_col_idx]
                                : 0U);
                        vlSelfRef.test_gtext__DOT__dut__DOT__tile_inst__DOT__nt_addr_a 
                            = (0x00000fffU & ((((IData)(0x000003e8U) 
                                                * (IData)(vlSelfRef.test_gtext__DOT__dut__DOT__tile_inst__DOT__dma_nt)) 
                                               + ((IData)(0x0028U) 
                                                  * (IData)(vlSelfRef.test_gtext__DOT__dut__DOT__tile_inst__DOT__dma_col_idx))) 
                                              + (IData)(vlSelfRef.test_gtext__DOT__dut__DOT__tile_inst__DOT__dma_col_row)));
                    }
                } else if ((0x03e8U > (IData)(vlSelfRef.test_gtext__DOT__dut__DOT__tile_inst__DOT__dma_count))) {
                    vlSelfRef.test_gtext__DOT__dut__DOT__tile_inst__DOT__nt_we_a = 1U;
                    vlSelfRef.test_gtext__DOT__dut__DOT__tile_inst__DOT__nt_din_a 
                        = vlSelfRef.test_gtext__DOT__dut__DOT__tile_inst__DOT__dma_fill_val;
                    vlSelfRef.test_gtext__DOT__dut__DOT__tile_inst__DOT__nt_addr_a 
                        = (0x00000fffU & (((IData)(0x000003e8U) 
                                           * (IData)(vlSelfRef.test_gtext__DOT__dut__DOT__tile_inst__DOT__dma_nt)) 
                                          + (IData)(vlSelfRef.test_gtext__DOT__dut__DOT__tile_inst__DOT__dma_count)));
                }
            } else if ((1U & (IData)(vlSelfRef.test_gtext__DOT__dut__DOT__tile_inst__DOT__dma_state))) {
                if ((0x0fa0U > (IData)(vlSelfRef.test_gtext__DOT__dut__DOT__tile_inst__DOT__dma_fill_addr))) {
                    vlSelfRef.test_gtext__DOT__dut__DOT__tile_inst__DOT__nt_we_a = 1U;
                    vlSelfRef.test_gtext__DOT__dut__DOT__tile_inst__DOT__nt_din_a = 0U;
                    vlSelfRef.test_gtext__DOT__dut__DOT__tile_inst__DOT__nt_addr_a 
                        = vlSelfRef.test_gtext__DOT__dut__DOT__tile_inst__DOT__dma_fill_addr;
                }
            }
        }
    } else {
        if ((1U & (~ ((IData)(vlSelfRef.test_gtext__DOT__dut__DOT__tile_inst__DOT__dma_state) 
                      >> 2U)))) {
            if ((1U & (~ ((IData)(vlSelfRef.test_gtext__DOT__dut__DOT__tile_inst__DOT__dma_state) 
                          >> 1U)))) {
                if ((1U & (IData)(vlSelfRef.test_gtext__DOT__dut__DOT__tile_inst__DOT__dma_state))) {
                    if (vlSelfRef.test_gtext__DOT__dut__DOT__tile_inst__DOT__dma_data_valid) {
                        vlSelfRef.test_gtext__DOT__dut__DOT__tile_inst__DOT__td_we_a = 1U;
                        vlSelfRef.test_gtext__DOT__dut__DOT__tile_inst__DOT__td_din_a = 0U;
                        vlSelfRef.test_gtext__DOT__dut__DOT__tile_inst__DOT__td_addr_a 
                            = (0x00007fffU & (IData)(vlSelfRef.test_gtext__DOT__dut__DOT__tile_inst__DOT__dma_dst));
                    }
                } else if (vlSelfRef.test_gtext__DOT__dut__DOT__vgc_tile_we) {
                    vlSelfRef.test_gtext__DOT__dut__DOT__tile_inst__DOT__td_we_a = 1U;
                    vlSelfRef.test_gtext__DOT__dut__DOT__tile_inst__DOT__td_din_a 
                        = test_gtext__DOT__dut__DOT__tile_blt_wdata;
                    vlSelfRef.test_gtext__DOT__dut__DOT__tile_inst__DOT__td_addr_a 
                        = test_gtext__DOT__dut__DOT__tile_blt_addr;
                } else if (vlSelfRef.test_gtext__DOT__dut__DOT__vgc_tile_re) {
                    vlSelfRef.test_gtext__DOT__dut__DOT__tile_inst__DOT__td_addr_a 
                        = test_gtext__DOT__dut__DOT__tile_blt_addr;
                }
            }
        }
        if ((4U & (IData)(vlSelfRef.test_gtext__DOT__dut__DOT__tile_inst__DOT__dma_state))) {
            if ((2U & (IData)(vlSelfRef.test_gtext__DOT__dut__DOT__tile_inst__DOT__dma_state))) {
                if ((1U & (IData)(vlSelfRef.test_gtext__DOT__dut__DOT__tile_inst__DOT__dma_state))) {
                    if (vlSelfRef.test_gtext__DOT__dut__DOT__tile_inst__DOT__dma_data_valid) {
                        vlSelfRef.test_gtext__DOT__dut__DOT__tile_inst__DOT__at_we_a = 1U;
                        vlSelfRef.test_gtext__DOT__dut__DOT__tile_inst__DOT__at_din_a = 0U;
                        vlSelfRef.test_gtext__DOT__dut__DOT__tile_inst__DOT__at_addr_a 
                            = (0x00000fffU & ((((IData)(0x000003e8U) 
                                                * (IData)(vlSelfRef.test_gtext__DOT__dut__DOT__tile_inst__DOT__dma_nt)) 
                                               + ((IData)(0x00000028U) 
                                                  * (IData)(vlSelfRef.test_gtext__DOT__dut__DOT__tile_inst__DOT__dma_col_row))) 
                                              + (0x0000003fU 
                                                 & (IData)(vlSelfRef.test_gtext__DOT__dut__DOT__tile_inst__DOT__dma_dst))));
                    }
                }
                if ((1U & (~ (IData)(vlSelfRef.test_gtext__DOT__dut__DOT__tile_inst__DOT__dma_state)))) {
                    if (vlSelfRef.test_gtext__DOT__dut__DOT__tile_inst__DOT__dma_data_valid) {
                        vlSelfRef.test_gtext__DOT__dut__DOT__tile_inst__DOT__nt_we_a = 1U;
                        vlSelfRef.test_gtext__DOT__dut__DOT__tile_inst__DOT__nt_din_a = 0U;
                        vlSelfRef.test_gtext__DOT__dut__DOT__tile_inst__DOT__nt_addr_a 
                            = (0x00000fffU & ((((IData)(0x000003e8U) 
                                                * (IData)(vlSelfRef.test_gtext__DOT__dut__DOT__tile_inst__DOT__dma_nt)) 
                                               + ((IData)(0x0028U) 
                                                  * (IData)(vlSelfRef.test_gtext__DOT__dut__DOT__tile_inst__DOT__dma_col_idx))) 
                                              + (IData)(vlSelfRef.test_gtext__DOT__dut__DOT__tile_inst__DOT__dma_col_row)));
                    }
                }
            } else if ((1U & (IData)(vlSelfRef.test_gtext__DOT__dut__DOT__tile_inst__DOT__dma_state))) {
                if (vlSelfRef.test_gtext__DOT__dut__DOT__tile_inst__DOT__dma_data_valid) {
                    vlSelfRef.test_gtext__DOT__dut__DOT__tile_inst__DOT__nt_we_a = 1U;
                    vlSelfRef.test_gtext__DOT__dut__DOT__tile_inst__DOT__nt_din_a = 0U;
                    vlSelfRef.test_gtext__DOT__dut__DOT__tile_inst__DOT__nt_addr_a 
                        = (0x00000fffU & ((((IData)(0x000003e8U) 
                                            * (IData)(vlSelfRef.test_gtext__DOT__dut__DOT__tile_inst__DOT__dma_nt)) 
                                           + ((IData)(0x00000028U) 
                                              * (IData)(vlSelfRef.test_gtext__DOT__dut__DOT__tile_inst__DOT__dma_col_row))) 
                                          + (0x0000003fU 
                                             & (IData)(vlSelfRef.test_gtext__DOT__dut__DOT__tile_inst__DOT__dma_dst))));
                }
            }
        } else if ((2U & (IData)(vlSelfRef.test_gtext__DOT__dut__DOT__tile_inst__DOT__dma_state))) {
            if ((1U & (IData)(vlSelfRef.test_gtext__DOT__dut__DOT__tile_inst__DOT__dma_state))) {
                if (vlSelfRef.test_gtext__DOT__dut__DOT__tile_inst__DOT__dma_data_valid) {
                    vlSelfRef.test_gtext__DOT__dut__DOT__tile_inst__DOT__at_we_a = 1U;
                    vlSelfRef.test_gtext__DOT__dut__DOT__tile_inst__DOT__at_din_a = 0U;
                    vlSelfRef.test_gtext__DOT__dut__DOT__tile_inst__DOT__at_addr_a 
                        = (0x00000fffU & (((IData)(0x000003e8U) 
                                           * (IData)(vlSelfRef.test_gtext__DOT__dut__DOT__tile_inst__DOT__dma_nt)) 
                                          + (0x000003ffU 
                                             & (IData)(vlSelfRef.test_gtext__DOT__dut__DOT__tile_inst__DOT__dma_dst))));
                }
            }
            if ((1U & (~ (IData)(vlSelfRef.test_gtext__DOT__dut__DOT__tile_inst__DOT__dma_state)))) {
                if (vlSelfRef.test_gtext__DOT__dut__DOT__tile_inst__DOT__dma_data_valid) {
                    vlSelfRef.test_gtext__DOT__dut__DOT__tile_inst__DOT__nt_we_a = 1U;
                    vlSelfRef.test_gtext__DOT__dut__DOT__tile_inst__DOT__nt_din_a = 0U;
                    vlSelfRef.test_gtext__DOT__dut__DOT__tile_inst__DOT__nt_addr_a 
                        = (0x00000fffU & (((IData)(0x000003e8U) 
                                           * (IData)(vlSelfRef.test_gtext__DOT__dut__DOT__tile_inst__DOT__dma_nt)) 
                                          + (0x000003ffU 
                                             & (IData)(vlSelfRef.test_gtext__DOT__dut__DOT__tile_inst__DOT__dma_dst))));
                }
            }
        } else if ((1U & (~ (IData)(vlSelfRef.test_gtext__DOT__dut__DOT__tile_inst__DOT__dma_state)))) {
            if (vlSelfRef.test_gtext__DOT__dut__DOT__tile_inst__DOT__cmd_at_we) {
                vlSelfRef.test_gtext__DOT__dut__DOT__tile_inst__DOT__at_we_a = 1U;
                vlSelfRef.test_gtext__DOT__dut__DOT__tile_inst__DOT__at_din_a 
                    = vlSelfRef.test_gtext__DOT__dut__DOT__tile_inst__DOT__cmd_at_din;
                vlSelfRef.test_gtext__DOT__dut__DOT__tile_inst__DOT__at_addr_a 
                    = vlSelfRef.test_gtext__DOT__dut__DOT__tile_inst__DOT__cmd_at_addr;
            }
            if (vlSelfRef.test_gtext__DOT__dut__DOT__tile_inst__DOT__cmd_nt_we) {
                vlSelfRef.test_gtext__DOT__dut__DOT__tile_inst__DOT__nt_we_a = 1U;
                vlSelfRef.test_gtext__DOT__dut__DOT__tile_inst__DOT__nt_din_a 
                    = vlSelfRef.test_gtext__DOT__dut__DOT__tile_inst__DOT__cmd_nt_din;
                vlSelfRef.test_gtext__DOT__dut__DOT__tile_inst__DOT__nt_addr_a 
                    = vlSelfRef.test_gtext__DOT__dut__DOT__tile_inst__DOT__cmd_nt_addr;
            }
        }
    }
    vlSelfRef.test_gtext__DOT__dut__DOT__char_a_addr 
        = (0x000007ffU & ((IData)(vlSelfRef.test_gtext__DOT__dut__DOT__cmd_char_we)
                           ? (IData)(vlSelfRef.test_gtext__DOT__dut__DOT__cmd_char_addr)
                           : (((IData)(__VdfgRegularize_hebeb780c_0_0) 
                               & (IData)(vlSelfRef.test_gtext__DOT__dut__DOT__char_ram_sel))
                               ? ((IData)(vlSelfRef.test_gtext__DOT__cpu_addr) 
                                  - (IData)(0x0200U))
                               : ((IData)(vlSelfRef.test_gtext__DOT__dut__DOT__dbg_char_sel)
                                   ? ((IData)(vlSelfRef.test_gtext__DOT__dut__DOT__dbg_addr) 
                                      - (IData)(0x0200U))
                                   : (IData)(vlSelfRef.test_gtext__DOT__dut__DOT__cmd_char_addr)))));
    vlSelfRef.test_gtext__DOT__dut__DOT__color_a_addr 
        = (0x000007ffU & ((IData)(vlSelfRef.test_gtext__DOT__dut__DOT__cmd_color_we)
                           ? (IData)(vlSelfRef.test_gtext__DOT__dut__DOT__cmd_color_addr)
                           : (((IData)(__VdfgRegularize_hebeb780c_0_0) 
                               & (IData)(vlSelfRef.test_gtext__DOT__dut__DOT__color_ram_sel))
                               ? ((IData)(vlSelfRef.test_gtext__DOT__cpu_addr) 
                                  - (IData)(0x01d0U))
                               : ((IData)(vlSelfRef.test_gtext__DOT__dut__DOT__dbg_color_sel)
                                   ? ((IData)(vlSelfRef.test_gtext__DOT__dut__DOT__dbg_addr) 
                                      - (IData)(0x01d0U))
                                   : (IData)(vlSelfRef.test_gtext__DOT__dut__DOT__cmd_color_addr)))));
}

VL_ATTR_COLD void Vtest_gtext___024root___eval_stl(Vtest_gtext___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vtest_gtext___024root___eval_stl\n"); );
    Vtest_gtext__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
    // Body
    if ((1ULL & vlSelfRef.__VstlTriggered[0U])) {
        Vtest_gtext___024root___stl_sequent__TOP__0(vlSelf);
    }
}

VL_ATTR_COLD bool Vtest_gtext___024root___eval_phase__stl(Vtest_gtext___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vtest_gtext___024root___eval_phase__stl\n"); );
    Vtest_gtext__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
    // Locals
    CData/*0:0*/ __VstlExecute;
    // Body
    Vtest_gtext___024root___eval_triggers_vec__stl(vlSelf);
#ifdef VL_DEBUG
    if (VL_UNLIKELY(vlSymsp->_vm_contextp__->debug())) {
        Vtest_gtext___024root___dump_triggers__stl(vlSelfRef.__VstlTriggered, "stl"s);
    }
#endif
    __VstlExecute = Vtest_gtext___024root___trigger_anySet__stl(vlSelfRef.__VstlTriggered);
    if (__VstlExecute) {
        Vtest_gtext___024root___eval_stl(vlSelf);
    }
    return (__VstlExecute);
}

bool Vtest_gtext___024root___trigger_anySet__act(const VlUnpacked<QData/*63:0*/, 1> &in);

#ifdef VL_DEBUG
VL_ATTR_COLD void Vtest_gtext___024root___dump_triggers__act(const VlUnpacked<QData/*63:0*/, 1> &triggers, const std::string &tag) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vtest_gtext___024root___dump_triggers__act\n"); );
    // Body
    if ((1U & (~ (IData)(Vtest_gtext___024root___trigger_anySet__act(triggers))))) {
        VL_DBG_MSGS("         No '" + tag + "' region triggers active\n");
    }
    if ((1U & (IData)(triggers[0U]))) {
        VL_DBG_MSGS("         '" + tag + "' region trigger index 0 is active: @(posedge test_gtext.clk)\n");
    }
    if ((1U & (IData)((triggers[0U] >> 1U)))) {
        VL_DBG_MSGS("         '" + tag + "' region trigger index 1 is active: @([true] __VdlySched.awaitingCurrentTime())\n");
    }
}
#endif  // VL_DEBUG

VL_ATTR_COLD void Vtest_gtext___024root___ctor_var_reset(Vtest_gtext___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vtest_gtext___024root___ctor_var_reset\n"); );
    Vtest_gtext__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
    // Body
    const uint64_t __VscopeHash = VL_MURMUR64_HASH(vlSelf->vlNamep);
    vlSelf->test_gtext__DOT__rst = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 14303400775415336536ull);
    vlSelf->test_gtext__DOT__cpu_ce = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 6493861718214198967ull);
    vlSelf->test_gtext__DOT__cpu_addr = VL_SCOPED_RAND_RESET_I(16, __VscopeHash, 13540603281645981109ull);
    vlSelf->test_gtext__DOT__cpu_wdata = VL_SCOPED_RAND_RESET_I(8, __VscopeHash, 18434036441828036121ull);
    vlSelf->test_gtext__DOT__cpu_we = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 1440029442511923587ull);
    vlSelf->test_gtext__DOT__cpu_re = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 313003488021195825ull);
    vlSelf->test_gtext__DOT__unnamedblk21__DOT__unnamedblk22__DOT__unnamedblk23__DOT__c = 0;
    vlSelf->test_gtext__DOT__dut__DOT__dbg_addr = VL_SCOPED_RAND_RESET_I(16, __VscopeHash, 16719761686325773130ull);
    vlSelf->test_gtext__DOT__dut__DOT__h_count = VL_SCOPED_RAND_RESET_I(10, __VscopeHash, 14639252724418562700ull);
    vlSelf->test_gtext__DOT__dut__DOT__v_count = VL_SCOPED_RAND_RESET_I(10, __VscopeHash, 15752458022618394008ull);
    vlSelf->test_gtext__DOT__dut__DOT__in_text_area = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 2628945918846439950ull);
    vlSelf->test_gtext__DOT__dut__DOT__gfx_y = VL_SCOPED_RAND_RESET_I(8, __VscopeHash, 3586718646104789082ull);
    vlSelf->test_gtext__DOT__dut__DOT__scroll_offset = VL_SCOPED_RAND_RESET_I(5, __VscopeHash, 16141629118237099922ull);
    vlSelf->test_gtext__DOT__dut__DOT__char_a_addr = VL_SCOPED_RAND_RESET_I(11, __VscopeHash, 14772912608721163214ull);
    vlSelf->test_gtext__DOT__dut__DOT__char_a_dout = VL_SCOPED_RAND_RESET_I(8, __VscopeHash, 11593101704059693149ull);
    vlSelf->test_gtext__DOT__dut__DOT__color_a_addr = VL_SCOPED_RAND_RESET_I(11, __VscopeHash, 141981630766303066ull);
    vlSelf->test_gtext__DOT__dut__DOT__color_a_dout = VL_SCOPED_RAND_RESET_I(8, __VscopeHash, 15293606374391935785ull);
    vlSelf->test_gtext__DOT__dut__DOT__font_a_addr = VL_SCOPED_RAND_RESET_I(11, __VscopeHash, 15673933023647244778ull);
    vlSelf->test_gtext__DOT__dut__DOT__font_a_dout = VL_SCOPED_RAND_RESET_I(8, __VscopeHash, 8337134953495980195ull);
    vlSelf->test_gtext__DOT__dut__DOT__gfx_a_addr = VL_SCOPED_RAND_RESET_I(16, __VscopeHash, 4020288927754660816ull);
    vlSelf->test_gtext__DOT__dut__DOT__gfx_a_din = VL_SCOPED_RAND_RESET_I(4, __VscopeHash, 10589303273967157602ull);
    vlSelf->test_gtext__DOT__dut__DOT__gfx_a_we = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 16836551771709668058ull);
    vlSelf->test_gtext__DOT__dut__DOT__gfx_a_dout = VL_SCOPED_RAND_RESET_I(4, __VscopeHash, 8863984893323870540ull);
    VL_SCOPED_RAND_RESET_W(512, vlSelf->test_gtext__DOT__dut__DOT__fio_name_flat, __VscopeHash, 11452118710901515500ull);
    vlSelf->test_gtext__DOT__dut__DOT____Vcellinp__artist_inst__cmd_x1 = 0;
    vlSelf->test_gtext__DOT__dut__DOT____Vcellinp__artist_inst__cmd_x0 = 0;
    vlSelf->test_gtext__DOT__dut__DOT__spr_a_addr = VL_SCOPED_RAND_RESET_I(11, __VscopeHash, 17351034789362911127ull);
    vlSelf->test_gtext__DOT__dut__DOT__spr_a_dout = VL_SCOPED_RAND_RESET_I(8, __VscopeHash, 1814918138159767262ull);
    for (int __Vi0 = 0; __Vi0 < 32; ++__Vi0) {
        vlSelf->test_gtext__DOT__dut__DOT__copper_pos[__Vi0] = VL_SCOPED_RAND_RESET_I(17, __VscopeHash, 13348176889484970079ull);
    }
    for (int __Vi0 = 0; __Vi0 < 32; ++__Vi0) {
        vlSelf->test_gtext__DOT__dut__DOT__copper_reg[__Vi0] = VL_SCOPED_RAND_RESET_I(8, __VscopeHash, 6344207626922330584ull);
    }
    for (int __Vi0 = 0; __Vi0 < 32; ++__Vi0) {
        vlSelf->test_gtext__DOT__dut__DOT__copper_val[__Vi0] = VL_SCOPED_RAND_RESET_I(8, __VscopeHash, 12954210795585683711ull);
    }
    vlSelf->test_gtext__DOT__dut__DOT__copper_count = VL_SCOPED_RAND_RESET_I(9, __VscopeHash, 2503304984579484620ull);
    vlSelf->test_gtext__DOT__dut__DOT__copper_index = VL_SCOPED_RAND_RESET_I(9, __VscopeHash, 2985676083971159835ull);
    vlSelf->test_gtext__DOT__dut__DOT__copper_enabled = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 9016215159935639775ull);
    for (int __Vi0 = 0; __Vi0 < 4096; ++__Vi0) {
        vlSelf->test_gtext__DOT__dut__DOT__copper_list_pos[__Vi0] = VL_SCOPED_RAND_RESET_I(17, __VscopeHash, 18061174532420144910ull);
    }
    for (int __Vi0 = 0; __Vi0 < 4096; ++__Vi0) {
        vlSelf->test_gtext__DOT__dut__DOT__copper_list_reg[__Vi0] = VL_SCOPED_RAND_RESET_I(8, __VscopeHash, 15278629023587605120ull);
    }
    for (int __Vi0 = 0; __Vi0 < 4096; ++__Vi0) {
        vlSelf->test_gtext__DOT__dut__DOT__copper_list_val[__Vi0] = VL_SCOPED_RAND_RESET_I(8, __VscopeHash, 13737211589415651560ull);
    }
    for (int __Vi0 = 0; __Vi0 < 128; ++__Vi0) {
        vlSelf->test_gtext__DOT__dut__DOT__copper_list_count[__Vi0] = VL_SCOPED_RAND_RESET_I(9, __VscopeHash, 18088861645044480554ull);
    }
    vlSelf->test_gtext__DOT__dut__DOT__copper_target_list = VL_SCOPED_RAND_RESET_I(7, __VscopeHash, 4135179491151537336ull);
    vlSelf->test_gtext__DOT__dut__DOT__copper_active_list = VL_SCOPED_RAND_RESET_I(7, __VscopeHash, 6397484730973174071ull);
    vlSelf->test_gtext__DOT__dut__DOT__copper_pending_list = VL_SCOPED_RAND_RESET_I(7, __VscopeHash, 3059484220026955400ull);
    vlSelf->test_gtext__DOT__dut__DOT__copper_loading = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 7860474674698436309ull);
    vlSelf->test_gtext__DOT__dut__DOT__copper_load_idx = VL_SCOPED_RAND_RESET_I(9, __VscopeHash, 16449422076845652878ull);
    vlSelf->test_gtext__DOT__dut__DOT__copper_load_src = VL_SCOPED_RAND_RESET_I(7, __VscopeHash, 8790662691994087932ull);
    vlSelf->test_gtext__DOT__dut__DOT__copper_fire = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 14904240582071306838ull);
    vlSelf->test_gtext__DOT__dut__DOT__copper_fire_reg = VL_SCOPED_RAND_RESET_I(8, __VscopeHash, 17503011659109007767ull);
    vlSelf->test_gtext__DOT__dut__DOT__copper_fire_val = VL_SCOPED_RAND_RESET_I(8, __VscopeHash, 3147001082458484190ull);
    VL_SCOPED_RAND_RESET_W(544, vlSelf->test_gtext__DOT__dut__DOT__copper_pos_flat, __VscopeHash, 9725217780183627914ull);
    VL_SCOPED_RAND_RESET_W(256, vlSelf->test_gtext__DOT__dut__DOT__copper_reg_flat, __VscopeHash, 13715810015385479705ull);
    VL_SCOPED_RAND_RESET_W(256, vlSelf->test_gtext__DOT__dut__DOT__copper_val_flat, __VscopeHash, 389942207681022397ull);
    for (int __Vi0 = 0; __Vi0 < 32; ++__Vi0) {
        vlSelf->test_gtext__DOT__dut__DOT__regs[__Vi0] = VL_SCOPED_RAND_RESET_I(8, __VscopeHash, 5717803435238711724ull);
    }
    vlSelf->test_gtext__DOT__dut__DOT__cursor_x = VL_SCOPED_RAND_RESET_I(7, __VscopeHash, 17411306239001005824ull);
    vlSelf->test_gtext__DOT__dut__DOT__cursor_y = VL_SCOPED_RAND_RESET_I(5, __VscopeHash, 16616073805274190156ull);
    vlSelf->test_gtext__DOT__dut__DOT__fg_color = VL_SCOPED_RAND_RESET_I(4, __VscopeHash, 13440960544688180852ull);
    vlSelf->test_gtext__DOT__dut__DOT__gfx_color = VL_SCOPED_RAND_RESET_I(4, __VscopeHash, 9163041710980608268ull);
    vlSelf->test_gtext__DOT__dut__DOT__scroll_pending = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 17219666960472691212ull);
    vlSelf->test_gtext__DOT__dut__DOT__scroll_col = VL_SCOPED_RAND_RESET_I(7, __VscopeHash, 17489950201487617822ull);
    for (int __Vi0 = 0; __Vi0 < 64; ++__Vi0) {
        vlSelf->test_gtext__DOT__dut__DOT__fio_name[__Vi0] = VL_SCOPED_RAND_RESET_I(8, __VscopeHash, 2969694487675594196ull);
    }
    vlSelf->test_gtext__DOT__dut__DOT__fio_name_len = VL_SCOPED_RAND_RESET_I(6, __VscopeHash, 3544210825385513275ull);
    vlSelf->test_gtext__DOT__dut__DOT__cmd_busy = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 859465715554707943ull);
    vlSelf->test_gtext__DOT__dut__DOT__cmd_op = VL_SCOPED_RAND_RESET_I(8, __VscopeHash, 13412883726153095885ull);
    vlSelf->test_gtext__DOT__dut__DOT__cmd_x = VL_SCOPED_RAND_RESET_I(10, __VscopeHash, 493775422602295883ull);
    vlSelf->test_gtext__DOT__dut__DOT__cmd_y = VL_SCOPED_RAND_RESET_I(10, __VscopeHash, 9407697303666194405ull);
    vlSelf->test_gtext__DOT__dut__DOT__cmd_cx = VL_SCOPED_RAND_RESET_I(9, __VscopeHash, 17753300258138138982ull);
    vlSelf->test_gtext__DOT__dut__DOT__cmd_cy = VL_SCOPED_RAND_RESET_I(8, __VscopeHash, 5233459421947107579ull);
    vlSelf->test_gtext__DOT__dut__DOT__sprrow_count = VL_SCOPED_RAND_RESET_I(3, __VscopeHash, 12073205762070592897ull);
    for (int __Vi0 = 0; __Vi0 < 8; ++__Vi0) {
        vlSelf->test_gtext__DOT__dut__DOT__sprrow_data[__Vi0] = VL_SCOPED_RAND_RESET_I(8, __VscopeHash, 3423454827941843128ull);
    }
    vlSelf->test_gtext__DOT__dut__DOT__sprrow_spr = VL_SCOPED_RAND_RESET_I(4, __VscopeHash, 12055337424985448248ull);
    vlSelf->test_gtext__DOT__dut__DOT__sprrow_row = VL_SCOPED_RAND_RESET_I(4, __VscopeHash, 9451281006212627942ull);
    vlSelf->test_gtext__DOT__dut__DOT__sprcopy_phase = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 3956787598884182606ull);
    vlSelf->test_gtext__DOT__dut__DOT__memread_pending = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 16668045770269896660ull);
    vlSelf->test_gtext__DOT__dut__DOT__memread_space = VL_SCOPED_RAND_RESET_I(3, __VscopeHash, 9018013328976705466ull);
    vlSelf->test_gtext__DOT__dut__DOT__cmd_char_addr = VL_SCOPED_RAND_RESET_I(11, __VscopeHash, 10124110284126256661ull);
    vlSelf->test_gtext__DOT__dut__DOT__cmd_char_din = VL_SCOPED_RAND_RESET_I(8, __VscopeHash, 5452104126224573969ull);
    vlSelf->test_gtext__DOT__dut__DOT__cmd_char_we = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 16842147220832497030ull);
    vlSelf->test_gtext__DOT__dut__DOT__cmd_color_addr = VL_SCOPED_RAND_RESET_I(11, __VscopeHash, 17083840882822814226ull);
    vlSelf->test_gtext__DOT__dut__DOT__cmd_color_din = VL_SCOPED_RAND_RESET_I(8, __VscopeHash, 13579141363974231390ull);
    vlSelf->test_gtext__DOT__dut__DOT__cmd_color_we = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 6954291729239583953ull);
    vlSelf->test_gtext__DOT__dut__DOT__cmd_gfx_addr = VL_SCOPED_RAND_RESET_I(16, __VscopeHash, 5555358918635362162ull);
    vlSelf->test_gtext__DOT__dut__DOT__cmd_gfx_din = VL_SCOPED_RAND_RESET_I(4, __VscopeHash, 4908380653489600170ull);
    vlSelf->test_gtext__DOT__dut__DOT__cmd_gfx_we = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 11993374866197327663ull);
    vlSelf->test_gtext__DOT__dut__DOT__cmd_gfx_re = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 13928135095297694221ull);
    vlSelf->test_gtext__DOT__dut__DOT__cmd_spr_addr = VL_SCOPED_RAND_RESET_I(11, __VscopeHash, 10611297741066092585ull);
    vlSelf->test_gtext__DOT__dut__DOT__cmd_spr_din = VL_SCOPED_RAND_RESET_I(8, __VscopeHash, 4722308608190363685ull);
    vlSelf->test_gtext__DOT__dut__DOT__cmd_spr_we = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 2869022028240755829ull);
    vlSelf->test_gtext__DOT__dut__DOT__cmd_spr_re = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 3688385961016970219ull);
    vlSelf->test_gtext__DOT__dut__DOT__artist_cmd_valid = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 15151789108510095632ull);
    vlSelf->test_gtext__DOT__dut__DOT__artist_cmd_code = VL_SCOPED_RAND_RESET_I(8, __VscopeHash, 12111975090368178336ull);
    vlSelf->test_gtext__DOT__dut__DOT__artist_gfx_addr = VL_SCOPED_RAND_RESET_I(16, __VscopeHash, 7003706206410659973ull);
    vlSelf->test_gtext__DOT__dut__DOT__artist_gfx_wdata = VL_SCOPED_RAND_RESET_I(4, __VscopeHash, 5542055644786734351ull);
    vlSelf->test_gtext__DOT__dut__DOT__artist_gfx_we = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 12275227436036785234ull);
    vlSelf->test_gtext__DOT__dut__DOT__artist_gfx_raddr = VL_SCOPED_RAND_RESET_I(16, __VscopeHash, 1311080676480362183ull);
    vlSelf->test_gtext__DOT__dut__DOT__artist_gfx_re = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 11091176713943986472ull);
    vlSelf->test_gtext__DOT__dut__DOT__artist_font_addr = VL_SCOPED_RAND_RESET_I(11, __VscopeHash, 7686286930674267074ull);
    vlSelf->test_gtext__DOT__dut__DOT__artist_font_re = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 13095959532060614087ull);
    vlSelf->test_gtext__DOT__dut__DOT__artist_busy = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 684803599237142013ull);
    vlSelf->test_gtext__DOT__dut__DOT__vgc_reg_sel = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 1444984040164101776ull);
    vlSelf->test_gtext__DOT__dut__DOT__spr_reg_sel = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 8006816090463396602ull);
    vlSelf->test_gtext__DOT__dut__DOT__char_ram_sel = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 15296460757522858040ull);
    vlSelf->test_gtext__DOT__dut__DOT__color_ram_sel = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 7131567158615532781ull);
    vlSelf->test_gtext__DOT__dut__DOT__tile_reg_sel = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 11217055117323895150ull);
    vlSelf->test_gtext__DOT__dut__DOT__dbg_char_sel = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 18147528781408615228ull);
    vlSelf->test_gtext__DOT__dut__DOT__dbg_color_sel = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 2237996875460021469ull);
    vlSelf->test_gtext__DOT__dut__DOT__tile_blt_rdata = VL_SCOPED_RAND_RESET_I(8, __VscopeHash, 9080324024385011692ull);
    vlSelf->test_gtext__DOT__dut__DOT__vgc_tile_addr = VL_SCOPED_RAND_RESET_I(15, __VscopeHash, 2250598560825225833ull);
    vlSelf->test_gtext__DOT__dut__DOT__vgc_tile_wdata = VL_SCOPED_RAND_RESET_I(8, __VscopeHash, 1793415118035973560ull);
    vlSelf->test_gtext__DOT__dut__DOT__vgc_tile_we = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 14201625656512424411ull);
    vlSelf->test_gtext__DOT__dut__DOT__vgc_tile_re = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 7517317466244818314ull);
    vlSelf->test_gtext__DOT__dut__DOT____Vcellinp__tile_inst__cpu_we = 0;
    vlSelf->test_gtext__DOT__dut__DOT__unnamedblk14__DOT__i = 0;
    vlSelf->test_gtext__DOT__dut__DOT__text_inst__DOT__char_mem__DOT____Vlvbound_hcad4ec09__0 = 0;
    vlSelf->test_gtext__DOT__dut__DOT__text_inst__DOT__char_mem__DOT____Vlvbound_h3482bd38__0 = 0;
    for (int __Vi0 = 0; __Vi0 < 2000; ++__Vi0) {
        vlSelf->test_gtext__DOT__dut__DOT__text_inst__DOT__char_mem__DOT__mem[__Vi0] = VL_SCOPED_RAND_RESET_I(8, __VscopeHash, 3324099194248913701ull);
    }
    vlSelf->test_gtext__DOT__dut__DOT__text_inst__DOT__color_mem__DOT____Vlvbound_hcad4ec09__0 = 0;
    vlSelf->test_gtext__DOT__dut__DOT__text_inst__DOT__color_mem__DOT____Vlvbound_h3482bd38__0 = 0;
    for (int __Vi0 = 0; __Vi0 < 2000; ++__Vi0) {
        vlSelf->test_gtext__DOT__dut__DOT__text_inst__DOT__color_mem__DOT__mem[__Vi0] = VL_SCOPED_RAND_RESET_I(8, __VscopeHash, 15130306874733961155ull);
    }
    for (int __Vi0 = 0; __Vi0 < 2048; ++__Vi0) {
        vlSelf->test_gtext__DOT__dut__DOT__text_inst__DOT__font_mem__DOT__mem[__Vi0] = VL_SCOPED_RAND_RESET_I(8, __VscopeHash, 3515235401615220121ull);
    }
    vlSelf->test_gtext__DOT__dut__DOT__gfx_inst__DOT__gfx_mem__DOT____Vlvbound_h0a4ce44c__0 = 0;
    vlSelf->test_gtext__DOT__dut__DOT__gfx_inst__DOT__gfx_mem__DOT____Vlvbound_h75f189da__0 = 0;
    for (int __Vi0 = 0; __Vi0 < 64000; ++__Vi0) {
        vlSelf->test_gtext__DOT__dut__DOT__gfx_inst__DOT__gfx_mem__DOT__mem[__Vi0] = VL_SCOPED_RAND_RESET_I(4, __VscopeHash, 2332415870273382022ull);
    }
    vlSelf->test_gtext__DOT__dut__DOT__artist_inst__DOT__op = VL_SCOPED_RAND_RESET_I(8, __VscopeHash, 2304557846128389951ull);
    vlSelf->test_gtext__DOT__dut__DOT__artist_inst__DOT__x = VL_SCOPED_RAND_RESET_I(10, __VscopeHash, 13758380268550901145ull);
    vlSelf->test_gtext__DOT__dut__DOT__artist_inst__DOT__y = VL_SCOPED_RAND_RESET_I(10, __VscopeHash, 2263400886486067676ull);
    vlSelf->test_gtext__DOT__dut__DOT__artist_inst__DOT__x2 = VL_SCOPED_RAND_RESET_I(10, __VscopeHash, 11745265969186740589ull);
    vlSelf->test_gtext__DOT__dut__DOT__artist_inst__DOT__y2 = VL_SCOPED_RAND_RESET_I(10, __VscopeHash, 11085847502312035355ull);
    vlSelf->test_gtext__DOT__dut__DOT__artist_inst__DOT__dx = VL_SCOPED_RAND_RESET_I(10, __VscopeHash, 7402457480336796089ull);
    vlSelf->test_gtext__DOT__dut__DOT__artist_inst__DOT__dy = VL_SCOPED_RAND_RESET_I(10, __VscopeHash, 17125321810191983882ull);
    vlSelf->test_gtext__DOT__dut__DOT__artist_inst__DOT__err = VL_SCOPED_RAND_RESET_I(10, __VscopeHash, 4864214268378525916ull);
    vlSelf->test_gtext__DOT__dut__DOT__artist_inst__DOT__sx = VL_SCOPED_RAND_RESET_I(10, __VscopeHash, 3199394403092458130ull);
    vlSelf->test_gtext__DOT__dut__DOT__artist_inst__DOT__sy = VL_SCOPED_RAND_RESET_I(10, __VscopeHash, 8433678066705126882ull);
    vlSelf->test_gtext__DOT__dut__DOT__artist_inst__DOT__cx = VL_SCOPED_RAND_RESET_I(9, __VscopeHash, 3332136576068379509ull);
    vlSelf->test_gtext__DOT__dut__DOT__artist_inst__DOT__cy = VL_SCOPED_RAND_RESET_I(8, __VscopeHash, 5885598183236206798ull);
    vlSelf->test_gtext__DOT__dut__DOT__artist_inst__DOT__color = VL_SCOPED_RAND_RESET_I(4, __VscopeHash, 11851917671942696493ull);
    vlSelf->test_gtext__DOT__dut__DOT__artist_inst__DOT__octant_count = VL_SCOPED_RAND_RESET_I(3, __VscopeHash, 16082446679997337894ull);
    vlSelf->test_gtext__DOT__dut__DOT__artist_inst__DOT__fs_a_addr = VL_SCOPED_RAND_RESET_I(9, __VscopeHash, 12902011812673869139ull);
    vlSelf->test_gtext__DOT__dut__DOT__artist_inst__DOT__fs_a_din = VL_SCOPED_RAND_RESET_I(17, __VscopeHash, 17282869607484572640ull);
    vlSelf->test_gtext__DOT__dut__DOT__artist_inst__DOT__fs_a_we = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 4754909356319162707ull);
    vlSelf->test_gtext__DOT__dut__DOT__artist_inst__DOT__fs_b_addr = VL_SCOPED_RAND_RESET_I(9, __VscopeHash, 8841531295678846669ull);
    vlSelf->test_gtext__DOT__dut__DOT__artist_inst__DOT__fs_b_dout = VL_SCOPED_RAND_RESET_I(17, __VscopeHash, 8504533321188606309ull);
    vlSelf->test_gtext__DOT__dut__DOT__artist_inst__DOT__fill_sp = VL_SCOPED_RAND_RESET_I(10, __VscopeHash, 4144953516483594756ull);
    vlSelf->test_gtext__DOT__dut__DOT__artist_inst__DOT__fill_target = VL_SCOPED_RAND_RESET_I(4, __VscopeHash, 12812284605656599674ull);
    vlSelf->test_gtext__DOT__dut__DOT__artist_inst__DOT__paint_phase = VL_SCOPED_RAND_RESET_I(3, __VscopeHash, 17701740741233530562ull);
    vlSelf->test_gtext__DOT__dut__DOT__artist_inst__DOT__paint_init = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 913383447358947330ull);
    vlSelf->test_gtext__DOT__dut__DOT__artist_inst__DOT__push_count = VL_SCOPED_RAND_RESET_I(2, __VscopeHash, 16614558745467228467ull);
    for (int __Vi0 = 0; __Vi0 < 4; ++__Vi0) {
        vlSelf->test_gtext__DOT__dut__DOT__artist_inst__DOT__push_neighbors[__Vi0] = VL_SCOPED_RAND_RESET_I(17, __VscopeHash, 16259677386481627837ull);
    }
    vlSelf->test_gtext__DOT__dut__DOT__artist_inst__DOT__gt_char_idx = VL_SCOPED_RAND_RESET_I(6, __VscopeHash, 9367210026442960565ull);
    vlSelf->test_gtext__DOT__dut__DOT__artist_inst__DOT__gt_len = VL_SCOPED_RAND_RESET_I(6, __VscopeHash, 8065999485589070717ull);
    vlSelf->test_gtext__DOT__dut__DOT__artist_inst__DOT__gt_font_row = VL_SCOPED_RAND_RESET_I(3, __VscopeHash, 12097528086088591613ull);
    vlSelf->test_gtext__DOT__dut__DOT__artist_inst__DOT__gt_font_col = VL_SCOPED_RAND_RESET_I(3, __VscopeHash, 15015973173207041350ull);
    vlSelf->test_gtext__DOT__dut__DOT__artist_inst__DOT__gt_scale = VL_SCOPED_RAND_RESET_I(8, __VscopeHash, 11150054078781290082ull);
    vlSelf->test_gtext__DOT__dut__DOT__artist_inst__DOT__gt_scale_x = VL_SCOPED_RAND_RESET_I(8, __VscopeHash, 6453829795991289163ull);
    vlSelf->test_gtext__DOT__dut__DOT__artist_inst__DOT__gt_scale_y = VL_SCOPED_RAND_RESET_I(8, __VscopeHash, 4651616042739750637ull);
    vlSelf->test_gtext__DOT__dut__DOT__artist_inst__DOT__gt_pen_x = VL_SCOPED_RAND_RESET_I(9, __VscopeHash, 4522036753383707425ull);
    vlSelf->test_gtext__DOT__dut__DOT__artist_inst__DOT__gt_start_y = VL_SCOPED_RAND_RESET_I(8, __VscopeHash, 8679941897598674657ull);
    vlSelf->test_gtext__DOT__dut__DOT__artist_inst__DOT__gt_cur_char = VL_SCOPED_RAND_RESET_I(8, __VscopeHash, 4044820825821060266ull);
    vlSelf->test_gtext__DOT__dut__DOT__artist_inst__DOT__gt_font_byte = VL_SCOPED_RAND_RESET_I(8, __VscopeHash, 4465618845673838209ull);
    vlSelf->test_gtext__DOT__dut__DOT__artist_inst__DOT__gt_font_pending = VL_SCOPED_RAND_RESET_I(2, __VscopeHash, 72898870975486307ull);
    for (int __Vi0 = 0; __Vi0 < 512; ++__Vi0) {
        vlSelf->test_gtext__DOT__dut__DOT__artist_inst__DOT__fill_stack_mem__DOT__mem[__Vi0] = VL_SCOPED_RAND_RESET_I(17, __VscopeHash, 13448192980250926294ull);
    }
    for (int __Vi0 = 0; __Vi0 < 2048; ++__Vi0) {
        vlSelf->test_gtext__DOT__dut__DOT__sprite_inst__DOT__spr_mem__DOT__mem[__Vi0] = VL_SCOPED_RAND_RESET_I(8, __VscopeHash, 8602155681380926289ull);
    }
    for (int __Vi0 = 0; __Vi0 < 32; ++__Vi0) {
        vlSelf->test_gtext__DOT__dut__DOT__copper_inst__DOT__copper_pos[__Vi0] = VL_SCOPED_RAND_RESET_I(17, __VscopeHash, 2475153076739313104ull);
    }
    for (int __Vi0 = 0; __Vi0 < 32; ++__Vi0) {
        vlSelf->test_gtext__DOT__dut__DOT__copper_inst__DOT__copper_reg[__Vi0] = VL_SCOPED_RAND_RESET_I(8, __VscopeHash, 13701060137518146381ull);
    }
    for (int __Vi0 = 0; __Vi0 < 32; ++__Vi0) {
        vlSelf->test_gtext__DOT__dut__DOT__copper_inst__DOT__copper_val[__Vi0] = VL_SCOPED_RAND_RESET_I(8, __VscopeHash, 17019529162278766698ull);
    }
    vlSelf->test_gtext__DOT__dut__DOT__tile_inst__DOT____Vlvbound_ha768cd16__0 = 0;
    vlSelf->test_gtext__DOT__dut__DOT__tile_inst__DOT____Vlvbound_h67f0dfac__0 = 0;
    vlSelf->test_gtext__DOT__dut__DOT__tile_inst__DOT____Vlvbound_hbbfa6b2d__0 = 0;
    for (int __Vi0 = 0; __Vi0 < 32; ++__Vi0) {
        vlSelf->test_gtext__DOT__dut__DOT__tile_inst__DOT__tregs[__Vi0] = VL_SCOPED_RAND_RESET_I(8, __VscopeHash, 18270243406106332108ull);
    }
    vlSelf->test_gtext__DOT__dut__DOT__tile_inst__DOT__tile_size16 = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 13139656258643222596ull);
    for (int __Vi0 = 0; __Vi0 < 25; ++__Vi0) {
        vlSelf->test_gtext__DOT__dut__DOT__tile_inst__DOT__col_buffer[__Vi0] = VL_SCOPED_RAND_RESET_I(8, __VscopeHash, 1795781375190662032ull);
    }
    vlSelf->test_gtext__DOT__dut__DOT__tile_inst__DOT__td_addr_a = VL_SCOPED_RAND_RESET_I(15, __VscopeHash, 8595058557668753648ull);
    vlSelf->test_gtext__DOT__dut__DOT__tile_inst__DOT__td_din_a = VL_SCOPED_RAND_RESET_I(8, __VscopeHash, 857995034330003069ull);
    vlSelf->test_gtext__DOT__dut__DOT__tile_inst__DOT__td_we_a = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 1596058680492233554ull);
    vlSelf->test_gtext__DOT__dut__DOT__tile_inst__DOT__td_dout_a = VL_SCOPED_RAND_RESET_I(8, __VscopeHash, 1114612786883218153ull);
    vlSelf->test_gtext__DOT__dut__DOT__tile_inst__DOT__nt_addr_a = VL_SCOPED_RAND_RESET_I(12, __VscopeHash, 16990967063424948314ull);
    vlSelf->test_gtext__DOT__dut__DOT__tile_inst__DOT__nt_din_a = VL_SCOPED_RAND_RESET_I(8, __VscopeHash, 11560035163761443577ull);
    vlSelf->test_gtext__DOT__dut__DOT__tile_inst__DOT__nt_we_a = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 11600057944134107231ull);
    vlSelf->test_gtext__DOT__dut__DOT__tile_inst__DOT__at_addr_a = VL_SCOPED_RAND_RESET_I(12, __VscopeHash, 17159685107014643110ull);
    vlSelf->test_gtext__DOT__dut__DOT__tile_inst__DOT__at_din_a = VL_SCOPED_RAND_RESET_I(8, __VscopeHash, 602364917741857112ull);
    vlSelf->test_gtext__DOT__dut__DOT__tile_inst__DOT__at_we_a = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 657401868738977146ull);
    vlSelf->test_gtext__DOT__dut__DOT__tile_inst__DOT__dma_state = VL_SCOPED_RAND_RESET_I(5, __VscopeHash, 4369830011025993429ull);
    vlSelf->test_gtext__DOT__dut__DOT__tile_inst__DOT__dma_src = VL_SCOPED_RAND_RESET_I(16, __VscopeHash, 4234971837780970878ull);
    vlSelf->test_gtext__DOT__dut__DOT__tile_inst__DOT__dma_count = VL_SCOPED_RAND_RESET_I(16, __VscopeHash, 13200183622572246966ull);
    vlSelf->test_gtext__DOT__dut__DOT__tile_inst__DOT__dma_dst = VL_SCOPED_RAND_RESET_I(16, __VscopeHash, 15845440449076165420ull);
    vlSelf->test_gtext__DOT__dut__DOT__tile_inst__DOT__dma_nt = VL_SCOPED_RAND_RESET_I(2, __VscopeHash, 16195395304032593976ull);
    vlSelf->test_gtext__DOT__dut__DOT__tile_inst__DOT__dma_col_row = VL_SCOPED_RAND_RESET_I(6, __VscopeHash, 4101907933597741616ull);
    vlSelf->test_gtext__DOT__dut__DOT__tile_inst__DOT__dma_col_idx = VL_SCOPED_RAND_RESET_I(5, __VscopeHash, 16870770182329836049ull);
    vlSelf->test_gtext__DOT__dut__DOT__tile_inst__DOT__dma_data_valid = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 9499442344939337129ull);
    vlSelf->test_gtext__DOT__dut__DOT__tile_inst__DOT__dma_pal_phase = VL_SCOPED_RAND_RESET_I(8, __VscopeHash, 17705008398272713815ull);
    vlSelf->test_gtext__DOT__dut__DOT__tile_inst__DOT__dma_pal_cidx = VL_SCOPED_RAND_RESET_I(4, __VscopeHash, 2936190877280518371ull);
    vlSelf->test_gtext__DOT__dut__DOT__tile_inst__DOT__dma_fill_val = VL_SCOPED_RAND_RESET_I(8, __VscopeHash, 11166417674430488722ull);
    vlSelf->test_gtext__DOT__dut__DOT__tile_inst__DOT__dma_fill_addr = VL_SCOPED_RAND_RESET_I(12, __VscopeHash, 192697004294980404ull);
    vlSelf->test_gtext__DOT__dut__DOT__tile_inst__DOT__cmd_nt_we = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 512552315511070494ull);
    vlSelf->test_gtext__DOT__dut__DOT__tile_inst__DOT__cmd_at_we = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 7227519735740186249ull);
    vlSelf->test_gtext__DOT__dut__DOT__tile_inst__DOT__cmd_nt_addr = VL_SCOPED_RAND_RESET_I(12, __VscopeHash, 16260361081401717554ull);
    vlSelf->test_gtext__DOT__dut__DOT__tile_inst__DOT__cmd_at_addr = VL_SCOPED_RAND_RESET_I(12, __VscopeHash, 15113869147604700826ull);
    vlSelf->test_gtext__DOT__dut__DOT__tile_inst__DOT__cmd_nt_din = VL_SCOPED_RAND_RESET_I(8, __VscopeHash, 3380505477093231411ull);
    vlSelf->test_gtext__DOT__dut__DOT__tile_inst__DOT__cmd_at_din = VL_SCOPED_RAND_RESET_I(8, __VscopeHash, 8032313573648633549ull);
    vlSelf->test_gtext__DOT__dut__DOT__tile_inst__DOT__blt_tile_rd_pending = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 13711510907210145398ull);
    vlSelf->test_gtext__DOT__dut__DOT__tile_inst__DOT__blt_tile_rd_latch = VL_SCOPED_RAND_RESET_I(8, __VscopeHash, 13112911728976026965ull);
    for (int __Vi0 = 0; __Vi0 < 32768; ++__Vi0) {
        vlSelf->test_gtext__DOT__dut__DOT__tile_inst__DOT__tile_data_ram__DOT__mem[__Vi0] = VL_SCOPED_RAND_RESET_I(8, __VscopeHash, 2759890791125057150ull);
    }
    for (int __Vi0 = 0; __Vi0 < 4096; ++__Vi0) {
        vlSelf->test_gtext__DOT__dut__DOT__tile_inst__DOT__nametable_ram__DOT__mem[__Vi0] = VL_SCOPED_RAND_RESET_I(8, __VscopeHash, 7973566383505751918ull);
    }
    for (int __Vi0 = 0; __Vi0 < 4096; ++__Vi0) {
        vlSelf->test_gtext__DOT__dut__DOT__tile_inst__DOT__attr_table_ram__DOT__mem[__Vi0] = VL_SCOPED_RAND_RESET_I(8, __VscopeHash, 8187312026641285533ull);
    }
    vlSelf->__Vfunc_test_gtext__DOT__gfx_pixel__93__Vfuncout = 0;
    vlSelf->__Vfunc_test_gtext__DOT__gfx_pixel__93__x = 0;
    vlSelf->__Vfunc_test_gtext__DOT__gfx_pixel__93__y = 0;
    for (int __Vi0 = 0; __Vi0 < 1; ++__Vi0) {
        vlSelf->__VstlTriggered[__Vi0] = 0;
    }
    for (int __Vi0 = 0; __Vi0 < 1; ++__Vi0) {
        vlSelf->__VactTriggered[__Vi0] = 0;
    }
    for (int __Vi0 = 0; __Vi0 < 1; ++__Vi0) {
        vlSelf->__VactTriggeredAcc[__Vi0] = 0;
    }
    vlSelf->__Vtrigprevexpr___TOP__test_gtext__DOT__clk__0 = 0;
    for (int __Vi0 = 0; __Vi0 < 1; ++__Vi0) {
        vlSelf->__VnbaTriggered[__Vi0] = 0;
    }
    vlSelf->__Vi = 0;
}
