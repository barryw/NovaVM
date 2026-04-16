// Verilated -*- C++ -*-
// DESCRIPTION: Verilator output: Design implementation internals
// See Vtest_tile_engine.h for the primary calling header

#include "Vtest_tile_engine__pch.h"

void Vtest_tile_engine___024root___timing_ready(Vtest_tile_engine___024root* vlSelf);

VL_ATTR_COLD void Vtest_tile_engine___024root___eval_static(Vtest_tile_engine___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vtest_tile_engine___024root___eval_static\n"); );
    Vtest_tile_engine__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
    // Body
    vlSelfRef.test_tile_engine__DOT__clk = 0U;
    vlSelfRef.test_tile_engine__DOT__pass_count = 0U;
    vlSelfRef.test_tile_engine__DOT__fail_count = 0U;
    vlSelfRef.test_tile_engine__DOT__test_num = 0U;
    vlSelfRef.__Vtrigprevexpr___TOP__test_tile_engine__DOT__clk__0 = 0U;
    Vtest_tile_engine___024root___timing_ready(vlSelf);
    do {
        vlSelfRef.__VactTriggeredAcc[vlSelfRef.__Vi] 
            = vlSelfRef.__VactTriggered[vlSelfRef.__Vi];
        vlSelfRef.__Vi = ((IData)(1U) + vlSelfRef.__Vi);
    } while ((0U >= vlSelfRef.__Vi));
}

VL_ATTR_COLD void Vtest_tile_engine___024root___eval_static__TOP(Vtest_tile_engine___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vtest_tile_engine___024root___eval_static__TOP\n"); );
    Vtest_tile_engine__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
    // Body
    vlSelfRef.test_tile_engine__DOT__clk = 0U;
    vlSelfRef.test_tile_engine__DOT__pass_count = 0U;
    vlSelfRef.test_tile_engine__DOT__fail_count = 0U;
    vlSelfRef.test_tile_engine__DOT__test_num = 0U;
}

VL_ATTR_COLD void Vtest_tile_engine___024root___eval_initial__TOP(Vtest_tile_engine___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vtest_tile_engine___024root___eval_initial__TOP\n"); );
    Vtest_tile_engine__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
    // Locals
    IData/*31:0*/ test_tile_engine__DOT__dut__DOT__tile_data_ram__DOT__gen_init_zero__DOT__i;
    test_tile_engine__DOT__dut__DOT__tile_data_ram__DOT__gen_init_zero__DOT__i = 0;
    IData/*31:0*/ test_tile_engine__DOT__dut__DOT__nametable_ram__DOT__gen_init_zero__DOT__i;
    test_tile_engine__DOT__dut__DOT__nametable_ram__DOT__gen_init_zero__DOT__i = 0;
    IData/*31:0*/ test_tile_engine__DOT__dut__DOT__attr_table_ram__DOT__gen_init_zero__DOT__i;
    test_tile_engine__DOT__dut__DOT__attr_table_ram__DOT__gen_init_zero__DOT__i = 0;
    // Body
    vlSelfRef.test_tile_engine__DOT__dut__DOT__tregs[0U] = 0U;
    vlSelfRef.test_tile_engine__DOT__dut__DOT__tregs[1U] = 0U;
    vlSelfRef.test_tile_engine__DOT__dut__DOT__tregs[2U] = 0U;
    vlSelfRef.test_tile_engine__DOT__dut__DOT__tregs[3U] = 0U;
    vlSelfRef.test_tile_engine__DOT__dut__DOT__tregs[4U] = 0U;
    vlSelfRef.test_tile_engine__DOT__dut__DOT__tregs[5U] = 0U;
    vlSelfRef.test_tile_engine__DOT__dut__DOT__tregs[6U] = 0U;
    vlSelfRef.test_tile_engine__DOT__dut__DOT__tregs[7U] = 0U;
    vlSelfRef.test_tile_engine__DOT__dut__DOT__tregs[8U] = 0U;
    vlSelfRef.test_tile_engine__DOT__dut__DOT__tregs[9U] = 0U;
    vlSelfRef.test_tile_engine__DOT__dut__DOT__tregs[10U] = 0U;
    vlSelfRef.test_tile_engine__DOT__dut__DOT__tregs[11U] = 0U;
    vlSelfRef.test_tile_engine__DOT__dut__DOT__tregs[12U] = 0U;
    vlSelfRef.test_tile_engine__DOT__dut__DOT__tregs[13U] = 0U;
    vlSelfRef.test_tile_engine__DOT__dut__DOT__tregs[14U] = 0U;
    vlSelfRef.test_tile_engine__DOT__dut__DOT__tregs[15U] = 0U;
    vlSelfRef.test_tile_engine__DOT__dut__DOT__tregs[16U] = 0U;
    vlSelfRef.test_tile_engine__DOT__dut__DOT__tregs[17U] = 0U;
    vlSelfRef.test_tile_engine__DOT__dut__DOT__tregs[18U] = 0U;
    vlSelfRef.test_tile_engine__DOT__dut__DOT__tregs[19U] = 0U;
    vlSelfRef.test_tile_engine__DOT__dut__DOT__tregs[20U] = 0U;
    vlSelfRef.test_tile_engine__DOT__dut__DOT__tregs[21U] = 0U;
    vlSelfRef.test_tile_engine__DOT__dut__DOT__tregs[22U] = 0U;
    vlSelfRef.test_tile_engine__DOT__dut__DOT__tregs[23U] = 0U;
    vlSelfRef.test_tile_engine__DOT__dut__DOT__tregs[24U] = 0U;
    vlSelfRef.test_tile_engine__DOT__dut__DOT__tregs[25U] = 0U;
    vlSelfRef.test_tile_engine__DOT__dut__DOT__tregs[26U] = 0U;
    vlSelfRef.test_tile_engine__DOT__dut__DOT__tregs[27U] = 0U;
    vlSelfRef.test_tile_engine__DOT__dut__DOT__tregs[28U] = 0U;
    vlSelfRef.test_tile_engine__DOT__dut__DOT__tregs[29U] = 0U;
    vlSelfRef.test_tile_engine__DOT__dut__DOT__tregs[30U] = 0U;
    vlSelfRef.test_tile_engine__DOT__dut__DOT__tregs[31U] = 0U;
    vlSelfRef.test_tile_engine__DOT__dut__DOT__col_buffer[0U] = 0U;
    vlSelfRef.test_tile_engine__DOT__dut__DOT__col_buffer[1U] = 0U;
    vlSelfRef.test_tile_engine__DOT__dut__DOT__col_buffer[2U] = 0U;
    vlSelfRef.test_tile_engine__DOT__dut__DOT__col_buffer[3U] = 0U;
    vlSelfRef.test_tile_engine__DOT__dut__DOT__col_buffer[4U] = 0U;
    vlSelfRef.test_tile_engine__DOT__dut__DOT__col_buffer[5U] = 0U;
    vlSelfRef.test_tile_engine__DOT__dut__DOT__col_buffer[6U] = 0U;
    vlSelfRef.test_tile_engine__DOT__dut__DOT__col_buffer[7U] = 0U;
    vlSelfRef.test_tile_engine__DOT__dut__DOT__col_buffer[8U] = 0U;
    vlSelfRef.test_tile_engine__DOT__dut__DOT__col_buffer[9U] = 0U;
    vlSelfRef.test_tile_engine__DOT__dut__DOT__col_buffer[10U] = 0U;
    vlSelfRef.test_tile_engine__DOT__dut__DOT__col_buffer[11U] = 0U;
    vlSelfRef.test_tile_engine__DOT__dut__DOT__col_buffer[12U] = 0U;
    vlSelfRef.test_tile_engine__DOT__dut__DOT__col_buffer[13U] = 0U;
    vlSelfRef.test_tile_engine__DOT__dut__DOT__col_buffer[14U] = 0U;
    vlSelfRef.test_tile_engine__DOT__dut__DOT__col_buffer[15U] = 0U;
    vlSelfRef.test_tile_engine__DOT__dut__DOT__col_buffer[16U] = 0U;
    vlSelfRef.test_tile_engine__DOT__dut__DOT__col_buffer[17U] = 0U;
    vlSelfRef.test_tile_engine__DOT__dut__DOT__col_buffer[18U] = 0U;
    vlSelfRef.test_tile_engine__DOT__dut__DOT__col_buffer[19U] = 0U;
    vlSelfRef.test_tile_engine__DOT__dut__DOT__col_buffer[20U] = 0U;
    vlSelfRef.test_tile_engine__DOT__dut__DOT__col_buffer[21U] = 0U;
    vlSelfRef.test_tile_engine__DOT__dut__DOT__col_buffer[22U] = 0U;
    vlSelfRef.test_tile_engine__DOT__dut__DOT__col_buffer[23U] = 0U;
    vlSelfRef.test_tile_engine__DOT__dut__DOT__col_buffer[24U] = 0U;
    vlSelfRef.test_tile_engine__DOT__dut__DOT__tile_size16 = 0U;
    vlSelfRef.test_tile_engine__DOT__dut__DOT__mirror_mode = 0U;
    vlSelfRef.test_tile_engine__DOT__dut__DOT__trans_color = 0U;
    vlSelfRef.test_tile_engine__DOT__dut__DOT__scroll_x = 0U;
    vlSelfRef.test_tile_engine__DOT__dut__DOT__scroll_y = 0U;
    vlSelfRef.test_tile_engine__DOT__dut__DOT__peek_val = 0U;
    vlSelfRef.test_tile_engine__DOT__dut__DOT__peek_attr = 0U;
    vlSelfRef.test_tile_engine__DOT__dut__DOT__dma_state = 0U;
    vlSelfRef.test_tile_engine__DOT__dut__DOT__dma_src = 0U;
    vlSelfRef.test_tile_engine__DOT__dut__DOT__dma_count = 0U;
    vlSelfRef.test_tile_engine__DOT__dut__DOT__dma_dst = 0U;
    vlSelfRef.test_tile_engine__DOT__dut__DOT__dma_nt = 0U;
    vlSelfRef.test_tile_engine__DOT__dut__DOT__dma_col_row = 0U;
    vlSelfRef.test_tile_engine__DOT__dut__DOT__dma_col_idx = 0U;
    vlSelfRef.test_tile_engine__DOT__dut__DOT__dma_data_valid = 0U;
    vlSelfRef.test_tile_engine__DOT__dut__DOT__dma_pal_phase = 0U;
    vlSelfRef.test_tile_engine__DOT__dut__DOT__dma_pal_r = 0U;
    vlSelfRef.test_tile_engine__DOT__dut__DOT__dma_pal_g = 0U;
    vlSelfRef.test_tile_engine__DOT__dut__DOT__dma_pal_sub = 0U;
    vlSelfRef.test_tile_engine__DOT__dut__DOT__dma_pal_cidx = 0U;
    vlSelfRef.test_tile_engine__DOT__dut__DOT__dma_fill_val = 0U;
    vlSelfRef.test_tile_engine__DOT__dut__DOT__dma_fill_addr = 0U;
    test_tile_engine__DOT__dut__DOT__tile_data_ram__DOT__gen_init_zero__DOT__i = 0U;
    while (VL_GTS_III(32, 0x00008000U, test_tile_engine__DOT__dut__DOT__tile_data_ram__DOT__gen_init_zero__DOT__i)) {
        vlSelfRef.test_tile_engine__DOT__dut__DOT__tile_data_ram__DOT__mem[(0x00007fffU 
                                                                            & test_tile_engine__DOT__dut__DOT__tile_data_ram__DOT__gen_init_zero__DOT__i)] = 0U;
        test_tile_engine__DOT__dut__DOT__tile_data_ram__DOT__gen_init_zero__DOT__i 
            = ((IData)(1U) + test_tile_engine__DOT__dut__DOT__tile_data_ram__DOT__gen_init_zero__DOT__i);
    }
    test_tile_engine__DOT__dut__DOT__nametable_ram__DOT__gen_init_zero__DOT__i = 0U;
    while (VL_GTS_III(32, 0x00001000U, test_tile_engine__DOT__dut__DOT__nametable_ram__DOT__gen_init_zero__DOT__i)) {
        vlSelfRef.test_tile_engine__DOT__dut__DOT__nametable_ram__DOT__mem[(0x00000fffU 
                                                                            & test_tile_engine__DOT__dut__DOT__nametable_ram__DOT__gen_init_zero__DOT__i)] = 0U;
        test_tile_engine__DOT__dut__DOT__nametable_ram__DOT__gen_init_zero__DOT__i 
            = ((IData)(1U) + test_tile_engine__DOT__dut__DOT__nametable_ram__DOT__gen_init_zero__DOT__i);
    }
    test_tile_engine__DOT__dut__DOT__attr_table_ram__DOT__gen_init_zero__DOT__i = 0U;
    while (VL_GTS_III(32, 0x00001000U, test_tile_engine__DOT__dut__DOT__attr_table_ram__DOT__gen_init_zero__DOT__i)) {
        vlSelfRef.test_tile_engine__DOT__dut__DOT__attr_table_ram__DOT__mem[(0x00000fffU 
                                                                             & test_tile_engine__DOT__dut__DOT__attr_table_ram__DOT__gen_init_zero__DOT__i)] = 0U;
        test_tile_engine__DOT__dut__DOT__attr_table_ram__DOT__gen_init_zero__DOT__i 
            = ((IData)(1U) + test_tile_engine__DOT__dut__DOT__attr_table_ram__DOT__gen_init_zero__DOT__i);
    }
    VL_READMEM_N(true, 12, 256, 0, "rom/tile_pal0.hex"s
                 ,  &(vlSelfRef.test_tile_engine__DOT__dut__DOT__pal_ram_inst__DOT__mem)
                 , 0, ~0ULL);
}

VL_ATTR_COLD void Vtest_tile_engine___024root___eval_final(Vtest_tile_engine___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vtest_tile_engine___024root___eval_final\n"); );
    Vtest_tile_engine__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
}

#ifdef VL_DEBUG
VL_ATTR_COLD void Vtest_tile_engine___024root___dump_triggers__stl(const VlUnpacked<QData/*63:0*/, 1> &triggers, const std::string &tag);
#endif  // VL_DEBUG
VL_ATTR_COLD bool Vtest_tile_engine___024root___eval_phase__stl(Vtest_tile_engine___024root* vlSelf);

VL_ATTR_COLD void Vtest_tile_engine___024root___eval_settle(Vtest_tile_engine___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vtest_tile_engine___024root___eval_settle\n"); );
    Vtest_tile_engine__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
    // Locals
    IData/*31:0*/ __VstlIterCount;
    // Body
    __VstlIterCount = 0U;
    vlSelfRef.__VstlFirstIteration = 1U;
    do {
        if (VL_UNLIKELY(((0x00000064U < __VstlIterCount)))) {
#ifdef VL_DEBUG
            Vtest_tile_engine___024root___dump_triggers__stl(vlSelfRef.__VstlTriggered, "stl"s);
#endif
            VL_FATAL_MT("test_tile_engine.sv", 6, "", "DIDNOTCONVERGE: Settle region did not converge after '--converge-limit' of 100 tries");
        }
        __VstlIterCount = ((IData)(1U) + __VstlIterCount);
        vlSelfRef.__VstlPhaseResult = Vtest_tile_engine___024root___eval_phase__stl(vlSelf);
        vlSelfRef.__VstlFirstIteration = 0U;
    } while (vlSelfRef.__VstlPhaseResult);
}

VL_ATTR_COLD void Vtest_tile_engine___024root___eval_triggers_vec__stl(Vtest_tile_engine___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vtest_tile_engine___024root___eval_triggers_vec__stl\n"); );
    Vtest_tile_engine__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
    // Body
    vlSelfRef.__VstlTriggered[0U] = ((0xfffffffffffffffeULL 
                                      & vlSelfRef.__VstlTriggered[0U]) 
                                     | (IData)((IData)(vlSelfRef.__VstlFirstIteration)));
}

VL_ATTR_COLD bool Vtest_tile_engine___024root___trigger_anySet__stl(const VlUnpacked<QData/*63:0*/, 1> &in);

#ifdef VL_DEBUG
VL_ATTR_COLD void Vtest_tile_engine___024root___dump_triggers__stl(const VlUnpacked<QData/*63:0*/, 1> &triggers, const std::string &tag) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vtest_tile_engine___024root___dump_triggers__stl\n"); );
    // Body
    if ((1U & (~ (IData)(Vtest_tile_engine___024root___trigger_anySet__stl(triggers))))) {
        VL_DBG_MSGS("         No '" + tag + "' region triggers active\n");
    }
    if ((1U & (IData)(triggers[0U]))) {
        VL_DBG_MSGS("         '" + tag + "' region trigger index 0 is active: Internal 'stl' trigger - first iteration\n");
    }
}
#endif  // VL_DEBUG

VL_ATTR_COLD bool Vtest_tile_engine___024root___trigger_anySet__stl(const VlUnpacked<QData/*63:0*/, 1> &in) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vtest_tile_engine___024root___trigger_anySet__stl\n"); );
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

VL_ATTR_COLD void Vtest_tile_engine___024root___stl_sequent__TOP__0(Vtest_tile_engine___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vtest_tile_engine___024root___stl_sequent__TOP__0\n"); );
    Vtest_tile_engine__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
    // Locals
    SData/*9:0*/ test_tile_engine__DOT__dut__DOT__world_y;
    test_tile_engine__DOT__dut__DOT__world_y = 0;
    SData/*15:0*/ test_tile_engine__DOT__dut__DOT____VdfgRegularize_hb33eefcd_0_1;
    test_tile_engine__DOT__dut__DOT____VdfgRegularize_hb33eefcd_0_1 = 0;
    CData/*1:0*/ __VdfgRegularize_hebeb780c_0_0;
    __VdfgRegularize_hebeb780c_0_0 = 0;
    // Body
    vlSelfRef.test_tile_engine__DOT__dut__DOT__tile_reg_sel 
        = ((0xa0c0U <= (IData)(vlSelfRef.test_tile_engine__DOT__cpu_addr)) 
           & (0xa0dfU >= (IData)(vlSelfRef.test_tile_engine__DOT__cpu_addr)));
    vlSelfRef.test_tile_engine__DOT__dut__DOT__s2_color_idx 
        = (0x0000000fU & ((IData)(vlSelfRef.test_tile_engine__DOT__dut__DOT__s2_sample_x_lsb)
                           ? (IData)(vlSelfRef.test_tile_engine__DOT__dut__DOT__td_dout_b)
                           : ((IData)(vlSelfRef.test_tile_engine__DOT__dut__DOT__td_dout_b) 
                              >> 4U)));
    vlSelfRef.test_tile_engine__DOT__dut__DOT__td_we_a = 0U;
    vlSelfRef.test_tile_engine__DOT__dut__DOT__s1_sample_x 
        = (0x0000000fU & ((0x00000040U & (IData)(vlSelfRef.test_tile_engine__DOT__dut__DOT__at_dout_b))
                           ? ((IData)(vlSelfRef.test_tile_engine__DOT__dut__DOT__s1_tile_w) 
                              - (IData)(vlSelfRef.test_tile_engine__DOT__dut__DOT__s1_pix_in_x))
                           : (IData)(vlSelfRef.test_tile_engine__DOT__dut__DOT__s1_pix_in_x)));
    vlSelfRef.test_tile_engine__DOT__dut__DOT__s1_sample_y 
        = (0x0000000fU & ((0x00000080U & (IData)(vlSelfRef.test_tile_engine__DOT__dut__DOT__at_dout_b))
                           ? ((IData)(vlSelfRef.test_tile_engine__DOT__dut__DOT__s1_tile_h) 
                              - (IData)(vlSelfRef.test_tile_engine__DOT__dut__DOT__s1_pix_in_y))
                           : (IData)(vlSelfRef.test_tile_engine__DOT__dut__DOT__s1_pix_in_y)));
    vlSelfRef.__VdfgRegularize_hebeb780c_0_2 = ((IData)(vlSelfRef.test_tile_engine__DOT__dut__DOT__s3_pixel_valid) 
                                                & ((IData)(vlSelfRef.test_tile_engine__DOT__dut__DOT__s3_color_idx) 
                                                   != (IData)(vlSelfRef.test_tile_engine__DOT__dut__DOT__s3_trans_color)));
    vlSelfRef.test_tile_engine__DOT__dut__DOT__at_we_a = 0U;
    vlSelfRef.test_tile_engine__DOT__dut__DOT__pr_we_a = 0U;
    vlSelfRef.test_tile_engine__DOT__dut__DOT__td_addr_a = 0U;
    vlSelfRef.test_tile_engine__DOT__dut__DOT__nt_we_a = 0U;
    vlSelfRef.test_tile_engine__DOT__dut__DOT__pr_addr_a = 0U;
    vlSelfRef.test_tile_engine__DOT__dut__DOT__at_addr_a = 0U;
    vlSelfRef.test_tile_engine__DOT__dut__DOT__nt_addr_a = 0U;
    test_tile_engine__DOT__dut__DOT__world_y = (0x000003ffU 
                                                & VL_MODDIV_III(10, 
                                                                (0x000003ffU 
                                                                 & ((IData)(vlSelfRef.test_tile_engine__DOT__pixel_y) 
                                                                    + (IData)(vlSelfRef.test_tile_engine__DOT__dut__DOT__scroll_y))), 
                                                                (((1U 
                                                                   == (IData)(vlSelfRef.test_tile_engine__DOT__dut__DOT__mirror_mode)) 
                                                                  | (3U 
                                                                     == (IData)(vlSelfRef.test_tile_engine__DOT__dut__DOT__mirror_mode)))
                                                                  ? 0x00c8U
                                                                  : 0x0190U)));
    test_tile_engine__DOT__dut__DOT____VdfgRegularize_hb33eefcd_0_1 
        = (0x0000ffffU & VL_MODDIV_III(16, (0x000007ffU 
                                            & ((IData)(vlSelfRef.test_tile_engine__DOT__pixel_x) 
                                               + (IData)(vlSelfRef.test_tile_engine__DOT__dut__DOT__scroll_x))), 
                                       (0x000007ffU 
                                        & VL_SHIFTL_III(11,11,32, 
                                                        (((0U 
                                                           == (IData)(vlSelfRef.test_tile_engine__DOT__dut__DOT__mirror_mode)) 
                                                          | (3U 
                                                             == (IData)(vlSelfRef.test_tile_engine__DOT__dut__DOT__mirror_mode)))
                                                          ? 0x0140U
                                                          : 0x0280U), 1U))));
    vlSelfRef.test_tile_engine__DOT__dma_data = vlSelfRef.test_tile_engine__DOT__sim_ram
        [vlSelfRef.test_tile_engine__DOT__dma_addr];
    vlSelfRef.test_tile_engine__DOT__dut__DOT____VdfgRegularize_hb33eefcd_0_3 
        = (0x000003ffU & ((0x00c8U <= (IData)(test_tile_engine__DOT__dut__DOT__world_y))
                           ? ((IData)(test_tile_engine__DOT__dut__DOT__world_y) 
                              - (IData)(0x00c8U)) : 
                          (0x000001ffU & (IData)(test_tile_engine__DOT__dut__DOT__world_y))));
    vlSelfRef.test_tile_engine__DOT__dut__DOT____VdfgRegularize_hb33eefcd_0_2 
        = (0x000007ffU & ((0x0140U <= (0x000007ffU 
                                       & (IData)(test_tile_engine__DOT__dut__DOT____VdfgRegularize_hb33eefcd_0_1)))
                           ? ((IData)(test_tile_engine__DOT__dut__DOT____VdfgRegularize_hb33eefcd_0_1) 
                              - (IData)(0x0140U)) : 
                          (0x000003ffU & (IData)(test_tile_engine__DOT__dut__DOT____VdfgRegularize_hb33eefcd_0_1))));
    __VdfgRegularize_hebeb780c_0_0 = (((0x00c8U <= (IData)(test_tile_engine__DOT__dut__DOT__world_y)) 
                                       << 1U) | (0x0140U 
                                                 <= 
                                                 (0x000007ffU 
                                                  & (IData)(test_tile_engine__DOT__dut__DOT____VdfgRegularize_hb33eefcd_0_1))));
    vlSelfRef.test_tile_engine__DOT__dut__DOT__td_din_a = 0U;
    vlSelfRef.test_tile_engine__DOT__dut__DOT__at_din_a = 0U;
    vlSelfRef.test_tile_engine__DOT__dut__DOT__pr_din_a = 0U;
    vlSelfRef.test_tile_engine__DOT__dut__DOT__nt_din_a = 0U;
    if ((0x00000010U & (IData)(vlSelfRef.test_tile_engine__DOT__dut__DOT__dma_state))) {
        if (vlSelfRef.test_tile_engine__DOT__blt_tile_we) {
            vlSelfRef.test_tile_engine__DOT__dut__DOT__td_we_a = 1U;
            vlSelfRef.test_tile_engine__DOT__dut__DOT__td_addr_a 
                = vlSelfRef.test_tile_engine__DOT__blt_tile_addr;
            vlSelfRef.test_tile_engine__DOT__dut__DOT__td_din_a 
                = vlSelfRef.test_tile_engine__DOT__blt_tile_wdata;
        } else if (vlSelfRef.test_tile_engine__DOT__blt_tile_re) {
            vlSelfRef.test_tile_engine__DOT__dut__DOT__td_addr_a 
                = vlSelfRef.test_tile_engine__DOT__blt_tile_addr;
        }
        if (vlSelfRef.test_tile_engine__DOT__dut__DOT__cmd_at_we) {
            vlSelfRef.test_tile_engine__DOT__dut__DOT__at_we_a = 1U;
            vlSelfRef.test_tile_engine__DOT__dut__DOT__at_addr_a 
                = vlSelfRef.test_tile_engine__DOT__dut__DOT__cmd_at_addr;
            vlSelfRef.test_tile_engine__DOT__dut__DOT__at_din_a 
                = vlSelfRef.test_tile_engine__DOT__dut__DOT__cmd_at_din;
        }
        if (vlSelfRef.test_tile_engine__DOT__dut__DOT__cmd_pal_we) {
            vlSelfRef.test_tile_engine__DOT__dut__DOT__pr_we_a = 1U;
            vlSelfRef.test_tile_engine__DOT__dut__DOT__pr_addr_a 
                = vlSelfRef.test_tile_engine__DOT__dut__DOT__cmd_pal_addr_reg;
            vlSelfRef.test_tile_engine__DOT__dut__DOT__pr_din_a 
                = vlSelfRef.test_tile_engine__DOT__dut__DOT__cmd_pal_din;
        }
        if (vlSelfRef.test_tile_engine__DOT__dut__DOT__cmd_nt_we) {
            vlSelfRef.test_tile_engine__DOT__dut__DOT__nt_we_a = 1U;
            vlSelfRef.test_tile_engine__DOT__dut__DOT__nt_addr_a 
                = vlSelfRef.test_tile_engine__DOT__dut__DOT__cmd_nt_addr;
            vlSelfRef.test_tile_engine__DOT__dut__DOT__nt_din_a 
                = vlSelfRef.test_tile_engine__DOT__dut__DOT__cmd_nt_din;
        }
    } else if ((8U & (IData)(vlSelfRef.test_tile_engine__DOT__dut__DOT__dma_state))) {
        if ((4U & (IData)(vlSelfRef.test_tile_engine__DOT__dut__DOT__dma_state))) {
            if ((2U & (IData)(vlSelfRef.test_tile_engine__DOT__dut__DOT__dma_state))) {
                if ((1U & (IData)(vlSelfRef.test_tile_engine__DOT__dut__DOT__dma_state))) {
                    if (vlSelfRef.test_tile_engine__DOT__blt_tile_we) {
                        vlSelfRef.test_tile_engine__DOT__dut__DOT__td_we_a = 1U;
                        vlSelfRef.test_tile_engine__DOT__dut__DOT__td_addr_a 
                            = vlSelfRef.test_tile_engine__DOT__blt_tile_addr;
                        vlSelfRef.test_tile_engine__DOT__dut__DOT__td_din_a 
                            = vlSelfRef.test_tile_engine__DOT__blt_tile_wdata;
                    } else if (vlSelfRef.test_tile_engine__DOT__blt_tile_re) {
                        vlSelfRef.test_tile_engine__DOT__dut__DOT__td_addr_a 
                            = vlSelfRef.test_tile_engine__DOT__blt_tile_addr;
                    }
                    if (vlSelfRef.test_tile_engine__DOT__dut__DOT__cmd_at_we) {
                        vlSelfRef.test_tile_engine__DOT__dut__DOT__at_we_a = 1U;
                        vlSelfRef.test_tile_engine__DOT__dut__DOT__at_addr_a 
                            = vlSelfRef.test_tile_engine__DOT__dut__DOT__cmd_at_addr;
                        vlSelfRef.test_tile_engine__DOT__dut__DOT__at_din_a 
                            = vlSelfRef.test_tile_engine__DOT__dut__DOT__cmd_at_din;
                    }
                    if (vlSelfRef.test_tile_engine__DOT__dut__DOT__cmd_pal_we) {
                        vlSelfRef.test_tile_engine__DOT__dut__DOT__pr_we_a = 1U;
                        vlSelfRef.test_tile_engine__DOT__dut__DOT__pr_addr_a 
                            = vlSelfRef.test_tile_engine__DOT__dut__DOT__cmd_pal_addr_reg;
                        vlSelfRef.test_tile_engine__DOT__dut__DOT__pr_din_a 
                            = vlSelfRef.test_tile_engine__DOT__dut__DOT__cmd_pal_din;
                    }
                    if (vlSelfRef.test_tile_engine__DOT__dut__DOT__cmd_nt_we) {
                        vlSelfRef.test_tile_engine__DOT__dut__DOT__nt_we_a = 1U;
                        vlSelfRef.test_tile_engine__DOT__dut__DOT__nt_addr_a 
                            = vlSelfRef.test_tile_engine__DOT__dut__DOT__cmd_nt_addr;
                        vlSelfRef.test_tile_engine__DOT__dut__DOT__nt_din_a 
                            = vlSelfRef.test_tile_engine__DOT__dut__DOT__cmd_nt_din;
                    }
                }
            } else {
                vlSelfRef.test_tile_engine__DOT__dut__DOT__at_addr_a 
                    = vlSelfRef.test_tile_engine__DOT__dut__DOT__cmd_at_addr;
                vlSelfRef.test_tile_engine__DOT__dut__DOT__nt_addr_a 
                    = vlSelfRef.test_tile_engine__DOT__dut__DOT__cmd_nt_addr;
            }
        } else {
            if ((1U & (~ ((IData)(vlSelfRef.test_tile_engine__DOT__dut__DOT__dma_state) 
                          >> 1U)))) {
                if ((1U & (IData)(vlSelfRef.test_tile_engine__DOT__dut__DOT__dma_state))) {
                    if ((0x0fa0U > (IData)(vlSelfRef.test_tile_engine__DOT__dut__DOT__dma_fill_addr))) {
                        vlSelfRef.test_tile_engine__DOT__dut__DOT__at_we_a = 1U;
                        vlSelfRef.test_tile_engine__DOT__dut__DOT__at_addr_a 
                            = vlSelfRef.test_tile_engine__DOT__dut__DOT__dma_fill_addr;
                        vlSelfRef.test_tile_engine__DOT__dut__DOT__at_din_a = 0U;
                    }
                } else if (vlSelfRef.test_tile_engine__DOT__dut__DOT__dma_data_valid) {
                    vlSelfRef.test_tile_engine__DOT__dut__DOT__at_we_a = 1U;
                    vlSelfRef.test_tile_engine__DOT__dut__DOT__at_addr_a 
                        = (0x00000fffU & ((((IData)(0x000003e8U) 
                                            * (IData)(vlSelfRef.test_tile_engine__DOT__dut__DOT__dma_nt)) 
                                           + ((IData)(0x0028U) 
                                              * (IData)(vlSelfRef.test_tile_engine__DOT__dut__DOT__dma_col_idx))) 
                                          + (IData)(vlSelfRef.test_tile_engine__DOT__dut__DOT__dma_col_row)));
                    vlSelfRef.test_tile_engine__DOT__dut__DOT__at_din_a 
                        = vlSelfRef.test_tile_engine__DOT__dma_data;
                }
            }
            if ((2U & (IData)(vlSelfRef.test_tile_engine__DOT__dut__DOT__dma_state))) {
                if ((1U & (IData)(vlSelfRef.test_tile_engine__DOT__dut__DOT__dma_state))) {
                    if ((0x19U > (IData)(vlSelfRef.test_tile_engine__DOT__dut__DOT__dma_col_idx))) {
                        vlSelfRef.test_tile_engine__DOT__dut__DOT__nt_we_a = 1U;
                        vlSelfRef.test_tile_engine__DOT__dut__DOT__nt_addr_a 
                            = (0x00000fffU & ((((IData)(0x000003e8U) 
                                                * (IData)(vlSelfRef.test_tile_engine__DOT__dut__DOT__dma_nt)) 
                                               + ((IData)(0x0028U) 
                                                  * (IData)(vlSelfRef.test_tile_engine__DOT__dut__DOT__dma_col_idx))) 
                                              + (IData)(vlSelfRef.test_tile_engine__DOT__dut__DOT__dma_col_row)));
                        vlSelfRef.test_tile_engine__DOT__dut__DOT__nt_din_a 
                            = ((0x18U >= (IData)(vlSelfRef.test_tile_engine__DOT__dut__DOT__dma_col_idx))
                                ? vlSelfRef.test_tile_engine__DOT__dut__DOT__col_buffer
                               [vlSelfRef.test_tile_engine__DOT__dut__DOT__dma_col_idx]
                                : 0U);
                    }
                } else if ((0x03e8U > (IData)(vlSelfRef.test_tile_engine__DOT__dut__DOT__dma_count))) {
                    vlSelfRef.test_tile_engine__DOT__dut__DOT__nt_we_a = 1U;
                    vlSelfRef.test_tile_engine__DOT__dut__DOT__nt_addr_a 
                        = (0x00000fffU & (((IData)(0x000003e8U) 
                                           * (IData)(vlSelfRef.test_tile_engine__DOT__dut__DOT__dma_nt)) 
                                          + (IData)(vlSelfRef.test_tile_engine__DOT__dut__DOT__dma_count)));
                    vlSelfRef.test_tile_engine__DOT__dut__DOT__nt_din_a 
                        = vlSelfRef.test_tile_engine__DOT__dut__DOT__dma_fill_val;
                }
            } else if ((1U & (IData)(vlSelfRef.test_tile_engine__DOT__dut__DOT__dma_state))) {
                if ((0x0fa0U > (IData)(vlSelfRef.test_tile_engine__DOT__dut__DOT__dma_fill_addr))) {
                    vlSelfRef.test_tile_engine__DOT__dut__DOT__nt_we_a = 1U;
                    vlSelfRef.test_tile_engine__DOT__dut__DOT__nt_addr_a 
                        = vlSelfRef.test_tile_engine__DOT__dut__DOT__dma_fill_addr;
                    vlSelfRef.test_tile_engine__DOT__dut__DOT__nt_din_a = 0U;
                }
            }
        }
    } else {
        if ((1U & (~ ((IData)(vlSelfRef.test_tile_engine__DOT__dut__DOT__dma_state) 
                      >> 2U)))) {
            if ((1U & (~ ((IData)(vlSelfRef.test_tile_engine__DOT__dut__DOT__dma_state) 
                          >> 1U)))) {
                if ((1U & (IData)(vlSelfRef.test_tile_engine__DOT__dut__DOT__dma_state))) {
                    if (vlSelfRef.test_tile_engine__DOT__dut__DOT__dma_data_valid) {
                        vlSelfRef.test_tile_engine__DOT__dut__DOT__td_we_a = 1U;
                        vlSelfRef.test_tile_engine__DOT__dut__DOT__td_addr_a 
                            = (0x00007fffU & (IData)(vlSelfRef.test_tile_engine__DOT__dut__DOT__dma_dst));
                        vlSelfRef.test_tile_engine__DOT__dut__DOT__td_din_a 
                            = vlSelfRef.test_tile_engine__DOT__dma_data;
                    }
                } else if (vlSelfRef.test_tile_engine__DOT__blt_tile_we) {
                    vlSelfRef.test_tile_engine__DOT__dut__DOT__td_we_a = 1U;
                    vlSelfRef.test_tile_engine__DOT__dut__DOT__td_addr_a 
                        = vlSelfRef.test_tile_engine__DOT__blt_tile_addr;
                    vlSelfRef.test_tile_engine__DOT__dut__DOT__td_din_a 
                        = vlSelfRef.test_tile_engine__DOT__blt_tile_wdata;
                } else if (vlSelfRef.test_tile_engine__DOT__blt_tile_re) {
                    vlSelfRef.test_tile_engine__DOT__dut__DOT__td_addr_a 
                        = vlSelfRef.test_tile_engine__DOT__blt_tile_addr;
                }
            }
        }
        if ((4U & (IData)(vlSelfRef.test_tile_engine__DOT__dut__DOT__dma_state))) {
            if ((2U & (IData)(vlSelfRef.test_tile_engine__DOT__dut__DOT__dma_state))) {
                if ((1U & (IData)(vlSelfRef.test_tile_engine__DOT__dut__DOT__dma_state))) {
                    if (vlSelfRef.test_tile_engine__DOT__dut__DOT__dma_data_valid) {
                        vlSelfRef.test_tile_engine__DOT__dut__DOT__at_we_a = 1U;
                        vlSelfRef.test_tile_engine__DOT__dut__DOT__at_addr_a 
                            = (0x00000fffU & ((((IData)(0x000003e8U) 
                                                * (IData)(vlSelfRef.test_tile_engine__DOT__dut__DOT__dma_nt)) 
                                               + ((IData)(0x00000028U) 
                                                  * (IData)(vlSelfRef.test_tile_engine__DOT__dut__DOT__dma_col_row))) 
                                              + (0x0000003fU 
                                                 & (IData)(vlSelfRef.test_tile_engine__DOT__dut__DOT__dma_dst))));
                        vlSelfRef.test_tile_engine__DOT__dut__DOT__at_din_a 
                            = vlSelfRef.test_tile_engine__DOT__dma_data;
                    }
                }
                if ((1U & (~ (IData)(vlSelfRef.test_tile_engine__DOT__dut__DOT__dma_state)))) {
                    if (vlSelfRef.test_tile_engine__DOT__dut__DOT__dma_data_valid) {
                        vlSelfRef.test_tile_engine__DOT__dut__DOT__nt_we_a = 1U;
                        vlSelfRef.test_tile_engine__DOT__dut__DOT__nt_addr_a 
                            = (0x00000fffU & ((((IData)(0x000003e8U) 
                                                * (IData)(vlSelfRef.test_tile_engine__DOT__dut__DOT__dma_nt)) 
                                               + ((IData)(0x0028U) 
                                                  * (IData)(vlSelfRef.test_tile_engine__DOT__dut__DOT__dma_col_idx))) 
                                              + (IData)(vlSelfRef.test_tile_engine__DOT__dut__DOT__dma_col_row)));
                        vlSelfRef.test_tile_engine__DOT__dut__DOT__nt_din_a 
                            = vlSelfRef.test_tile_engine__DOT__dma_data;
                    }
                }
            } else if ((1U & (IData)(vlSelfRef.test_tile_engine__DOT__dut__DOT__dma_state))) {
                if (vlSelfRef.test_tile_engine__DOT__dut__DOT__dma_data_valid) {
                    vlSelfRef.test_tile_engine__DOT__dut__DOT__nt_we_a = 1U;
                    vlSelfRef.test_tile_engine__DOT__dut__DOT__nt_addr_a 
                        = (0x00000fffU & ((((IData)(0x000003e8U) 
                                            * (IData)(vlSelfRef.test_tile_engine__DOT__dut__DOT__dma_nt)) 
                                           + ((IData)(0x00000028U) 
                                              * (IData)(vlSelfRef.test_tile_engine__DOT__dut__DOT__dma_col_row))) 
                                          + (0x0000003fU 
                                             & (IData)(vlSelfRef.test_tile_engine__DOT__dut__DOT__dma_dst))));
                    vlSelfRef.test_tile_engine__DOT__dut__DOT__nt_din_a 
                        = vlSelfRef.test_tile_engine__DOT__dma_data;
                }
            }
            if ((1U & (~ ((IData)(vlSelfRef.test_tile_engine__DOT__dut__DOT__dma_state) 
                          >> 1U)))) {
                if ((1U & (~ (IData)(vlSelfRef.test_tile_engine__DOT__dut__DOT__dma_state)))) {
                    if (((IData)(vlSelfRef.test_tile_engine__DOT__dut__DOT__dma_data_valid) 
                         & (2U == (IData)(vlSelfRef.test_tile_engine__DOT__dut__DOT__dma_pal_phase)))) {
                        vlSelfRef.test_tile_engine__DOT__dut__DOT__pr_we_a = 1U;
                        vlSelfRef.test_tile_engine__DOT__dut__DOT__pr_addr_a 
                            = (((IData)(vlSelfRef.test_tile_engine__DOT__dut__DOT__dma_pal_sub) 
                                << 4U) | (IData)(vlSelfRef.test_tile_engine__DOT__dut__DOT__dma_pal_cidx));
                        vlSelfRef.test_tile_engine__DOT__dut__DOT__pr_din_a 
                            = ((0x00000f00U & ((IData)(vlSelfRef.test_tile_engine__DOT__dut__DOT__dma_pal_r) 
                                               << 4U)) 
                               | ((0x000000f0U & (IData)(vlSelfRef.test_tile_engine__DOT__dut__DOT__dma_pal_g)) 
                                  | (0x0000000fU & 
                                     ((IData)(vlSelfRef.test_tile_engine__DOT__dma_data) 
                                      >> 4U))));
                    }
                }
            }
        } else {
            if ((2U & (IData)(vlSelfRef.test_tile_engine__DOT__dut__DOT__dma_state))) {
                if ((1U & (IData)(vlSelfRef.test_tile_engine__DOT__dut__DOT__dma_state))) {
                    if (vlSelfRef.test_tile_engine__DOT__dut__DOT__dma_data_valid) {
                        vlSelfRef.test_tile_engine__DOT__dut__DOT__at_we_a = 1U;
                        vlSelfRef.test_tile_engine__DOT__dut__DOT__at_addr_a 
                            = (0x00000fffU & (((IData)(0x000003e8U) 
                                               * (IData)(vlSelfRef.test_tile_engine__DOT__dut__DOT__dma_nt)) 
                                              + (0x000003ffU 
                                                 & (IData)(vlSelfRef.test_tile_engine__DOT__dut__DOT__dma_dst))));
                        vlSelfRef.test_tile_engine__DOT__dut__DOT__at_din_a 
                            = vlSelfRef.test_tile_engine__DOT__dma_data;
                    }
                }
                if ((1U & (~ (IData)(vlSelfRef.test_tile_engine__DOT__dut__DOT__dma_state)))) {
                    if (vlSelfRef.test_tile_engine__DOT__dut__DOT__dma_data_valid) {
                        vlSelfRef.test_tile_engine__DOT__dut__DOT__nt_we_a = 1U;
                        vlSelfRef.test_tile_engine__DOT__dut__DOT__nt_addr_a 
                            = (0x00000fffU & (((IData)(0x000003e8U) 
                                               * (IData)(vlSelfRef.test_tile_engine__DOT__dut__DOT__dma_nt)) 
                                              + (0x000003ffU 
                                                 & (IData)(vlSelfRef.test_tile_engine__DOT__dut__DOT__dma_dst))));
                        vlSelfRef.test_tile_engine__DOT__dut__DOT__nt_din_a 
                            = vlSelfRef.test_tile_engine__DOT__dma_data;
                    }
                }
            } else if ((1U & (~ (IData)(vlSelfRef.test_tile_engine__DOT__dut__DOT__dma_state)))) {
                if (vlSelfRef.test_tile_engine__DOT__dut__DOT__cmd_at_we) {
                    vlSelfRef.test_tile_engine__DOT__dut__DOT__at_we_a = 1U;
                    vlSelfRef.test_tile_engine__DOT__dut__DOT__at_addr_a 
                        = vlSelfRef.test_tile_engine__DOT__dut__DOT__cmd_at_addr;
                    vlSelfRef.test_tile_engine__DOT__dut__DOT__at_din_a 
                        = vlSelfRef.test_tile_engine__DOT__dut__DOT__cmd_at_din;
                }
                if (vlSelfRef.test_tile_engine__DOT__dut__DOT__cmd_nt_we) {
                    vlSelfRef.test_tile_engine__DOT__dut__DOT__nt_we_a = 1U;
                    vlSelfRef.test_tile_engine__DOT__dut__DOT__nt_addr_a 
                        = vlSelfRef.test_tile_engine__DOT__dut__DOT__cmd_nt_addr;
                    vlSelfRef.test_tile_engine__DOT__dut__DOT__nt_din_a 
                        = vlSelfRef.test_tile_engine__DOT__dut__DOT__cmd_nt_din;
                }
            }
            if ((1U & (~ ((IData)(vlSelfRef.test_tile_engine__DOT__dut__DOT__dma_state) 
                          >> 1U)))) {
                if ((1U & (~ (IData)(vlSelfRef.test_tile_engine__DOT__dut__DOT__dma_state)))) {
                    if (vlSelfRef.test_tile_engine__DOT__dut__DOT__cmd_pal_we) {
                        vlSelfRef.test_tile_engine__DOT__dut__DOT__pr_we_a = 1U;
                        vlSelfRef.test_tile_engine__DOT__dut__DOT__pr_addr_a 
                            = vlSelfRef.test_tile_engine__DOT__dut__DOT__cmd_pal_addr_reg;
                        vlSelfRef.test_tile_engine__DOT__dut__DOT__pr_din_a 
                            = vlSelfRef.test_tile_engine__DOT__dut__DOT__cmd_pal_din;
                    }
                }
            }
        }
    }
    vlSelfRef.test_tile_engine__DOT__dut__DOT__nt_addr_b 
        = (0x00000fffU & (((IData)(0x000003e8U) * (
                                                   (2U 
                                                    & (IData)(vlSelfRef.test_tile_engine__DOT__dut__DOT__mirror_mode))
                                                    ? 
                                                   ((1U 
                                                     & (IData)(vlSelfRef.test_tile_engine__DOT__dut__DOT__mirror_mode))
                                                     ? 0U
                                                     : (IData)(__VdfgRegularize_hebeb780c_0_0))
                                                    : 
                                                   ((1U 
                                                     & (IData)(vlSelfRef.test_tile_engine__DOT__dut__DOT__mirror_mode))
                                                     ? 
                                                    ((2U 
                                                      == (IData)(__VdfgRegularize_hebeb780c_0_0))
                                                      ? 0U
                                                      : 
                                                     ((3U 
                                                       == (IData)(__VdfgRegularize_hebeb780c_0_0))
                                                       ? 1U
                                                       : (IData)(__VdfgRegularize_hebeb780c_0_0)))
                                                     : 
                                                    ((1U 
                                                      == (IData)(__VdfgRegularize_hebeb780c_0_0))
                                                      ? 0U
                                                      : 
                                                     ((3U 
                                                       == (IData)(__VdfgRegularize_hebeb780c_0_0))
                                                       ? 2U
                                                       : (IData)(__VdfgRegularize_hebeb780c_0_0)))))) 
                          + (((IData)(0x00000028U) 
                              * (0x0000001fU & ((IData)(vlSelfRef.test_tile_engine__DOT__dut__DOT__tile_size16)
                                                 ? 
                                                (0x0000001fU 
                                                 & ((IData)(vlSelfRef.test_tile_engine__DOT__dut__DOT____VdfgRegularize_hb33eefcd_0_3) 
                                                    >> 4U))
                                                 : 
                                                ((IData)(vlSelfRef.test_tile_engine__DOT__dut__DOT____VdfgRegularize_hb33eefcd_0_3) 
                                                 >> 3U)))) 
                             + (0x0000003fU & ((IData)(vlSelfRef.test_tile_engine__DOT__dut__DOT__tile_size16)
                                                ? (0x0000003fU 
                                                   & ((IData)(vlSelfRef.test_tile_engine__DOT__dut__DOT____VdfgRegularize_hb33eefcd_0_2) 
                                                      >> 4U))
                                                : ((IData)(vlSelfRef.test_tile_engine__DOT__dut__DOT____VdfgRegularize_hb33eefcd_0_2) 
                                                   >> 3U))))));
}

VL_ATTR_COLD void Vtest_tile_engine___024root___eval_stl(Vtest_tile_engine___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vtest_tile_engine___024root___eval_stl\n"); );
    Vtest_tile_engine__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
    // Body
    if ((1ULL & vlSelfRef.__VstlTriggered[0U])) {
        Vtest_tile_engine___024root___stl_sequent__TOP__0(vlSelf);
    }
}

VL_ATTR_COLD bool Vtest_tile_engine___024root___eval_phase__stl(Vtest_tile_engine___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vtest_tile_engine___024root___eval_phase__stl\n"); );
    Vtest_tile_engine__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
    // Locals
    CData/*0:0*/ __VstlExecute;
    // Body
    Vtest_tile_engine___024root___eval_triggers_vec__stl(vlSelf);
#ifdef VL_DEBUG
    if (VL_UNLIKELY(vlSymsp->_vm_contextp__->debug())) {
        Vtest_tile_engine___024root___dump_triggers__stl(vlSelfRef.__VstlTriggered, "stl"s);
    }
#endif
    __VstlExecute = Vtest_tile_engine___024root___trigger_anySet__stl(vlSelfRef.__VstlTriggered);
    if (__VstlExecute) {
        Vtest_tile_engine___024root___eval_stl(vlSelf);
    }
    return (__VstlExecute);
}

bool Vtest_tile_engine___024root___trigger_anySet__act(const VlUnpacked<QData/*63:0*/, 1> &in);

#ifdef VL_DEBUG
VL_ATTR_COLD void Vtest_tile_engine___024root___dump_triggers__act(const VlUnpacked<QData/*63:0*/, 1> &triggers, const std::string &tag) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vtest_tile_engine___024root___dump_triggers__act\n"); );
    // Body
    if ((1U & (~ (IData)(Vtest_tile_engine___024root___trigger_anySet__act(triggers))))) {
        VL_DBG_MSGS("         No '" + tag + "' region triggers active\n");
    }
    if ((1U & (IData)(triggers[0U]))) {
        VL_DBG_MSGS("         '" + tag + "' region trigger index 0 is active: @(posedge test_tile_engine.clk)\n");
    }
    if ((1U & (IData)((triggers[0U] >> 1U)))) {
        VL_DBG_MSGS("         '" + tag + "' region trigger index 1 is active: @([true] __VdlySched.awaitingCurrentTime())\n");
    }
}
#endif  // VL_DEBUG

VL_ATTR_COLD void Vtest_tile_engine___024root___ctor_var_reset(Vtest_tile_engine___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vtest_tile_engine___024root___ctor_var_reset\n"); );
    Vtest_tile_engine__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
    // Body
    const uint64_t __VscopeHash = VL_MURMUR64_HASH(vlSelf->vlNamep);
    vlSelf->test_tile_engine__DOT__rst = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 271710713837701109ull);
    vlSelf->test_tile_engine__DOT__cpu_addr = VL_SCOPED_RAND_RESET_I(16, __VscopeHash, 803946103600872335ull);
    vlSelf->test_tile_engine__DOT__cpu_wdata = VL_SCOPED_RAND_RESET_I(8, __VscopeHash, 955538337165156160ull);
    vlSelf->test_tile_engine__DOT__cpu_we = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 9702642753232410941ull);
    vlSelf->test_tile_engine__DOT__dma_addr = VL_SCOPED_RAND_RESET_I(16, __VscopeHash, 16423132020857092397ull);
    vlSelf->test_tile_engine__DOT__dma_data = VL_SCOPED_RAND_RESET_I(8, __VscopeHash, 7616864929741500238ull);
    vlSelf->test_tile_engine__DOT__blt_tile_addr = VL_SCOPED_RAND_RESET_I(15, __VscopeHash, 4494882865674745290ull);
    vlSelf->test_tile_engine__DOT__blt_tile_wdata = VL_SCOPED_RAND_RESET_I(8, __VscopeHash, 17757965508116948869ull);
    vlSelf->test_tile_engine__DOT__blt_tile_we = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 1979204781523876407ull);
    vlSelf->test_tile_engine__DOT__blt_tile_re = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 16658413175767661424ull);
    vlSelf->test_tile_engine__DOT__pixel_x = VL_SCOPED_RAND_RESET_I(9, __VscopeHash, 16806960818367224977ull);
    vlSelf->test_tile_engine__DOT__pixel_y = VL_SCOPED_RAND_RESET_I(8, __VscopeHash, 808042982529873637ull);
    vlSelf->test_tile_engine__DOT__pixel_valid = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 10333621867875636360ull);
    for (int __Vi0 = 0; __Vi0 < 65536; ++__Vi0) {
        vlSelf->test_tile_engine__DOT__sim_ram[__Vi0] = VL_SCOPED_RAND_RESET_I(8, __VscopeHash, 17305522637293788989ull);
    }
    vlSelf->test_tile_engine__DOT__dut__DOT____Vlvbound_ha768cd16__0 = 0;
    vlSelf->test_tile_engine__DOT__dut__DOT____Vlvbound_h67f0dfac__0 = 0;
    vlSelf->test_tile_engine__DOT__dut__DOT____Vlvbound_hbbfa6b2d__0 = 0;
    for (int __Vi0 = 0; __Vi0 < 32; ++__Vi0) {
        vlSelf->test_tile_engine__DOT__dut__DOT__tregs[__Vi0] = VL_SCOPED_RAND_RESET_I(8, __VscopeHash, 1138367170071898483ull);
    }
    vlSelf->test_tile_engine__DOT__dut__DOT__tile_size16 = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 10450878103358709530ull);
    vlSelf->test_tile_engine__DOT__dut__DOT__mirror_mode = VL_SCOPED_RAND_RESET_I(2, __VscopeHash, 303774609262927154ull);
    vlSelf->test_tile_engine__DOT__dut__DOT__trans_color = VL_SCOPED_RAND_RESET_I(4, __VscopeHash, 1891349521520468563ull);
    vlSelf->test_tile_engine__DOT__dut__DOT__scroll_x = VL_SCOPED_RAND_RESET_I(16, __VscopeHash, 12780614183628355581ull);
    vlSelf->test_tile_engine__DOT__dut__DOT__scroll_y = VL_SCOPED_RAND_RESET_I(16, __VscopeHash, 2265867897021454193ull);
    vlSelf->test_tile_engine__DOT__dut__DOT__peek_val = VL_SCOPED_RAND_RESET_I(8, __VscopeHash, 9350562260170508415ull);
    vlSelf->test_tile_engine__DOT__dut__DOT__peek_attr = VL_SCOPED_RAND_RESET_I(8, __VscopeHash, 4081797378645618979ull);
    for (int __Vi0 = 0; __Vi0 < 25; ++__Vi0) {
        vlSelf->test_tile_engine__DOT__dut__DOT__col_buffer[__Vi0] = VL_SCOPED_RAND_RESET_I(8, __VscopeHash, 10774635441371973578ull);
    }
    vlSelf->test_tile_engine__DOT__dut__DOT__td_addr_a = VL_SCOPED_RAND_RESET_I(15, __VscopeHash, 7673126523101131701ull);
    vlSelf->test_tile_engine__DOT__dut__DOT__td_din_a = VL_SCOPED_RAND_RESET_I(8, __VscopeHash, 7867966092560542003ull);
    vlSelf->test_tile_engine__DOT__dut__DOT__td_we_a = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 15600914676434001948ull);
    vlSelf->test_tile_engine__DOT__dut__DOT__td_dout_a = VL_SCOPED_RAND_RESET_I(8, __VscopeHash, 7554250350765171109ull);
    vlSelf->test_tile_engine__DOT__dut__DOT__td_dout_b = VL_SCOPED_RAND_RESET_I(8, __VscopeHash, 697976405475381336ull);
    vlSelf->test_tile_engine__DOT__dut__DOT__nt_addr_a = VL_SCOPED_RAND_RESET_I(12, __VscopeHash, 16250576058284215731ull);
    vlSelf->test_tile_engine__DOT__dut__DOT__nt_din_a = VL_SCOPED_RAND_RESET_I(8, __VscopeHash, 1251791772304019810ull);
    vlSelf->test_tile_engine__DOT__dut__DOT__nt_we_a = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 9414276597944996357ull);
    vlSelf->test_tile_engine__DOT__dut__DOT__nt_dout_a = VL_SCOPED_RAND_RESET_I(8, __VscopeHash, 2508242991482677176ull);
    vlSelf->test_tile_engine__DOT__dut__DOT__nt_addr_b = VL_SCOPED_RAND_RESET_I(12, __VscopeHash, 3089268973366225051ull);
    vlSelf->test_tile_engine__DOT__dut__DOT__nt_dout_b = VL_SCOPED_RAND_RESET_I(8, __VscopeHash, 10693289076260816293ull);
    vlSelf->test_tile_engine__DOT__dut__DOT__at_addr_a = VL_SCOPED_RAND_RESET_I(12, __VscopeHash, 4055115722529780536ull);
    vlSelf->test_tile_engine__DOT__dut__DOT__at_din_a = VL_SCOPED_RAND_RESET_I(8, __VscopeHash, 8073867972512257839ull);
    vlSelf->test_tile_engine__DOT__dut__DOT__at_we_a = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 261633358851904797ull);
    vlSelf->test_tile_engine__DOT__dut__DOT__at_dout_a = VL_SCOPED_RAND_RESET_I(8, __VscopeHash, 288196517266104156ull);
    vlSelf->test_tile_engine__DOT__dut__DOT__at_dout_b = VL_SCOPED_RAND_RESET_I(8, __VscopeHash, 4552918062727436769ull);
    vlSelf->test_tile_engine__DOT__dut__DOT__pr_addr_a = VL_SCOPED_RAND_RESET_I(8, __VscopeHash, 9828144746759810509ull);
    vlSelf->test_tile_engine__DOT__dut__DOT__pr_din_a = VL_SCOPED_RAND_RESET_I(12, __VscopeHash, 5110776926990398638ull);
    vlSelf->test_tile_engine__DOT__dut__DOT__pr_we_a = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 16097572356192657967ull);
    vlSelf->test_tile_engine__DOT__dut__DOT__pr_dout_b = VL_SCOPED_RAND_RESET_I(12, __VscopeHash, 10666663962317757638ull);
    vlSelf->test_tile_engine__DOT__dut__DOT__tile_reg_sel = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 6866910759957633082ull);
    vlSelf->test_tile_engine__DOT__dut__DOT__dma_state = VL_SCOPED_RAND_RESET_I(5, __VscopeHash, 12778813815004487646ull);
    vlSelf->test_tile_engine__DOT__dut__DOT__dma_src = VL_SCOPED_RAND_RESET_I(16, __VscopeHash, 15140754559309398877ull);
    vlSelf->test_tile_engine__DOT__dut__DOT__dma_count = VL_SCOPED_RAND_RESET_I(16, __VscopeHash, 9181472726142213248ull);
    vlSelf->test_tile_engine__DOT__dut__DOT__dma_dst = VL_SCOPED_RAND_RESET_I(16, __VscopeHash, 15631435112520631851ull);
    vlSelf->test_tile_engine__DOT__dut__DOT__dma_nt = VL_SCOPED_RAND_RESET_I(2, __VscopeHash, 3780082379117485828ull);
    vlSelf->test_tile_engine__DOT__dut__DOT__dma_col_row = VL_SCOPED_RAND_RESET_I(6, __VscopeHash, 6071682187301263857ull);
    vlSelf->test_tile_engine__DOT__dut__DOT__dma_col_idx = VL_SCOPED_RAND_RESET_I(5, __VscopeHash, 16593512547379281056ull);
    vlSelf->test_tile_engine__DOT__dut__DOT__dma_data_valid = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 6857424287379225331ull);
    vlSelf->test_tile_engine__DOT__dut__DOT__dma_pal_phase = VL_SCOPED_RAND_RESET_I(8, __VscopeHash, 7075614796895809572ull);
    vlSelf->test_tile_engine__DOT__dut__DOT__dma_pal_r = VL_SCOPED_RAND_RESET_I(8, __VscopeHash, 2177135540252103409ull);
    vlSelf->test_tile_engine__DOT__dut__DOT__dma_pal_g = VL_SCOPED_RAND_RESET_I(8, __VscopeHash, 4627085913351443695ull);
    vlSelf->test_tile_engine__DOT__dut__DOT__dma_pal_sub = VL_SCOPED_RAND_RESET_I(4, __VscopeHash, 7747579906768600908ull);
    vlSelf->test_tile_engine__DOT__dut__DOT__dma_pal_cidx = VL_SCOPED_RAND_RESET_I(4, __VscopeHash, 15121643037548530845ull);
    vlSelf->test_tile_engine__DOT__dut__DOT__dma_fill_val = VL_SCOPED_RAND_RESET_I(8, __VscopeHash, 12881848119235916330ull);
    vlSelf->test_tile_engine__DOT__dut__DOT__dma_fill_addr = VL_SCOPED_RAND_RESET_I(12, __VscopeHash, 2072379948554808634ull);
    vlSelf->test_tile_engine__DOT__dut__DOT__cmd_nt_we = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 1380072387413487726ull);
    vlSelf->test_tile_engine__DOT__dut__DOT__cmd_at_we = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 16028257484088694331ull);
    vlSelf->test_tile_engine__DOT__dut__DOT__cmd_pal_we = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 957083430902433274ull);
    vlSelf->test_tile_engine__DOT__dut__DOT__cmd_nt_addr = VL_SCOPED_RAND_RESET_I(12, __VscopeHash, 14542563020617551707ull);
    vlSelf->test_tile_engine__DOT__dut__DOT__cmd_at_addr = VL_SCOPED_RAND_RESET_I(12, __VscopeHash, 16630385960695731354ull);
    vlSelf->test_tile_engine__DOT__dut__DOT__cmd_nt_din = VL_SCOPED_RAND_RESET_I(8, __VscopeHash, 4451796521230590087ull);
    vlSelf->test_tile_engine__DOT__dut__DOT__cmd_at_din = VL_SCOPED_RAND_RESET_I(8, __VscopeHash, 4977158219258379090ull);
    vlSelf->test_tile_engine__DOT__dut__DOT__cmd_pal_addr_reg = VL_SCOPED_RAND_RESET_I(8, __VscopeHash, 14321604925392028499ull);
    vlSelf->test_tile_engine__DOT__dut__DOT__cmd_pal_din = VL_SCOPED_RAND_RESET_I(12, __VscopeHash, 1667797870514356055ull);
    vlSelf->test_tile_engine__DOT__dut__DOT__blt_tile_rd_pending = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 10995128318015838021ull);
    vlSelf->test_tile_engine__DOT__dut__DOT__blt_tile_rd_latch = VL_SCOPED_RAND_RESET_I(8, __VscopeHash, 12053305421679838346ull);
    vlSelf->test_tile_engine__DOT__dut__DOT__s1_pix_in_x = VL_SCOPED_RAND_RESET_I(4, __VscopeHash, 5413982282195615289ull);
    vlSelf->test_tile_engine__DOT__dut__DOT__s1_pix_in_y = VL_SCOPED_RAND_RESET_I(4, __VscopeHash, 2507959689690036984ull);
    vlSelf->test_tile_engine__DOT__dut__DOT__s1_pixel_valid = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 11032326097856741781ull);
    vlSelf->test_tile_engine__DOT__dut__DOT__s1_tile_size16 = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 10666223805384137125ull);
    vlSelf->test_tile_engine__DOT__dut__DOT__s1_tile_w = VL_SCOPED_RAND_RESET_I(4, __VscopeHash, 11362503944803081304ull);
    vlSelf->test_tile_engine__DOT__dut__DOT__s1_tile_h = VL_SCOPED_RAND_RESET_I(4, __VscopeHash, 9131238341030348280ull);
    vlSelf->test_tile_engine__DOT__dut__DOT__s1_sample_x = VL_SCOPED_RAND_RESET_I(4, __VscopeHash, 5327443944502037160ull);
    vlSelf->test_tile_engine__DOT__dut__DOT__s1_sample_y = VL_SCOPED_RAND_RESET_I(4, __VscopeHash, 8278449735783881193ull);
    vlSelf->test_tile_engine__DOT__dut__DOT__s2_pixel_valid = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 12221424004233508438ull);
    vlSelf->test_tile_engine__DOT__dut__DOT__s2_sub_pal = VL_SCOPED_RAND_RESET_I(4, __VscopeHash, 3981829285290900610ull);
    vlSelf->test_tile_engine__DOT__dut__DOT__s2_sample_x_lsb = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 16810269598177271212ull);
    vlSelf->test_tile_engine__DOT__dut__DOT__s2_tile_pri = VL_SCOPED_RAND_RESET_I(2, __VscopeHash, 10818574066023583500ull);
    vlSelf->test_tile_engine__DOT__dut__DOT__s2_color_idx = VL_SCOPED_RAND_RESET_I(4, __VscopeHash, 13345507879516535649ull);
    vlSelf->test_tile_engine__DOT__dut__DOT__s3_pixel_valid = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 2094851807917294838ull);
    vlSelf->test_tile_engine__DOT__dut__DOT__s3_color_idx = VL_SCOPED_RAND_RESET_I(4, __VscopeHash, 4490293245266539255ull);
    vlSelf->test_tile_engine__DOT__dut__DOT__s3_tile_pri = VL_SCOPED_RAND_RESET_I(2, __VscopeHash, 13610441202979875733ull);
    vlSelf->test_tile_engine__DOT__dut__DOT__s3_trans_color = VL_SCOPED_RAND_RESET_I(4, __VscopeHash, 12342801393836128751ull);
    vlSelf->test_tile_engine__DOT__dut__DOT____VdfgRegularize_hb33eefcd_0_2 = 0;
    vlSelf->test_tile_engine__DOT__dut__DOT____VdfgRegularize_hb33eefcd_0_3 = 0;
    for (int __Vi0 = 0; __Vi0 < 32768; ++__Vi0) {
        vlSelf->test_tile_engine__DOT__dut__DOT__tile_data_ram__DOT__mem[__Vi0] = VL_SCOPED_RAND_RESET_I(8, __VscopeHash, 8544236759949089351ull);
    }
    for (int __Vi0 = 0; __Vi0 < 4096; ++__Vi0) {
        vlSelf->test_tile_engine__DOT__dut__DOT__nametable_ram__DOT__mem[__Vi0] = VL_SCOPED_RAND_RESET_I(8, __VscopeHash, 13572828102966046903ull);
    }
    for (int __Vi0 = 0; __Vi0 < 4096; ++__Vi0) {
        vlSelf->test_tile_engine__DOT__dut__DOT__attr_table_ram__DOT__mem[__Vi0] = VL_SCOPED_RAND_RESET_I(8, __VscopeHash, 689785705880798937ull);
    }
    for (int __Vi0 = 0; __Vi0 < 256; ++__Vi0) {
        vlSelf->test_tile_engine__DOT__dut__DOT__pal_ram_inst__DOT__mem[__Vi0] = VL_SCOPED_RAND_RESET_I(12, __VscopeHash, 8223028660629608285ull);
    }
    vlSelf->__VdfgRegularize_hebeb780c_0_2 = 0;
    for (int __Vi0 = 0; __Vi0 < 1; ++__Vi0) {
        vlSelf->__VstlTriggered[__Vi0] = 0;
    }
    for (int __Vi0 = 0; __Vi0 < 1; ++__Vi0) {
        vlSelf->__VactTriggered[__Vi0] = 0;
    }
    for (int __Vi0 = 0; __Vi0 < 1; ++__Vi0) {
        vlSelf->__VactTriggeredAcc[__Vi0] = 0;
    }
    vlSelf->__Vtrigprevexpr___TOP__test_tile_engine__DOT__clk__0 = 0;
    for (int __Vi0 = 0; __Vi0 < 1; ++__Vi0) {
        vlSelf->__VnbaTriggered[__Vi0] = 0;
    }
    vlSelf->__Vi = 0;
}
