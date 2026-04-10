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
    IData/*31:0*/ test_tile_engine__DOT__dut__DOT__unnamedblk2__DOT__i;
    test_tile_engine__DOT__dut__DOT__unnamedblk2__DOT__i = 0;
    IData/*31:0*/ test_tile_engine__DOT__dut__DOT__unnamedblk3__DOT__i;
    test_tile_engine__DOT__dut__DOT__unnamedblk3__DOT__i = 0;
    IData/*31:0*/ test_tile_engine__DOT__dut__DOT__unnamedblk4__DOT__i;
    test_tile_engine__DOT__dut__DOT__unnamedblk4__DOT__i = 0;
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
    test_tile_engine__DOT__dut__DOT__unnamedblk2__DOT__i = 0U;
    while (VL_GTS_III(32, 0x00008000U, test_tile_engine__DOT__dut__DOT__unnamedblk2__DOT__i)) {
        vlSelfRef.test_tile_engine__DOT__dut__DOT__tile_data[(0x00007fffU 
                                                              & test_tile_engine__DOT__dut__DOT__unnamedblk2__DOT__i)] = 0U;
        test_tile_engine__DOT__dut__DOT__unnamedblk2__DOT__i 
            = ((IData)(1U) + test_tile_engine__DOT__dut__DOT__unnamedblk2__DOT__i);
    }
    test_tile_engine__DOT__dut__DOT__unnamedblk3__DOT__i = 0U;
    while (VL_GTS_III(32, 0x00000fa0U, test_tile_engine__DOT__dut__DOT__unnamedblk3__DOT__i)) {
        vlSelfRef.test_tile_engine__DOT__dut__DOT____Vlvbound_h9afe376d__0 = 0U;
        vlSelfRef.test_tile_engine__DOT__dut__DOT____Vlvbound_h7ed4573b__0 = 0U;
        if (VL_LIKELY(((0x0f9fU >= (0x00000fffU & test_tile_engine__DOT__dut__DOT__unnamedblk3__DOT__i))))) {
            vlSelfRef.test_tile_engine__DOT__dut__DOT__nametable[(0x00000fffU 
                                                                  & test_tile_engine__DOT__dut__DOT__unnamedblk3__DOT__i)] 
                = vlSelfRef.test_tile_engine__DOT__dut__DOT____Vlvbound_h9afe376d__0;
            vlSelfRef.test_tile_engine__DOT__dut__DOT__attr_table[(0x00000fffU 
                                                                   & test_tile_engine__DOT__dut__DOT__unnamedblk3__DOT__i)] 
                = vlSelfRef.test_tile_engine__DOT__dut__DOT____Vlvbound_h7ed4573b__0;
        }
        test_tile_engine__DOT__dut__DOT__unnamedblk3__DOT__i 
            = ((IData)(1U) + test_tile_engine__DOT__dut__DOT__unnamedblk3__DOT__i);
    }
    vlSelfRef.test_tile_engine__DOT__dut__DOT__pal_ram[0U] = 0U;
    vlSelfRef.test_tile_engine__DOT__dut__DOT__pal_ram[1U] = 0x0fffU;
    vlSelfRef.test_tile_engine__DOT__dut__DOT__pal_ram[2U] = 0x0800U;
    vlSelfRef.test_tile_engine__DOT__dut__DOT__pal_ram[3U] = 0x0afeU;
    vlSelfRef.test_tile_engine__DOT__dut__DOT__pal_ram[4U] = 0x0c4cU;
    vlSelfRef.test_tile_engine__DOT__dut__DOT__pal_ram[5U] = 0x00c5U;
    vlSelfRef.test_tile_engine__DOT__dut__DOT__pal_ram[6U] = 0x000aU;
    vlSelfRef.test_tile_engine__DOT__dut__DOT__pal_ram[7U] = 0x0ee7U;
    vlSelfRef.test_tile_engine__DOT__dut__DOT__pal_ram[8U] = 0x0d85U;
    vlSelfRef.test_tile_engine__DOT__dut__DOT__pal_ram[9U] = 0x0640U;
    vlSelfRef.test_tile_engine__DOT__dut__DOT__pal_ram[10U] = 0x0f77U;
    vlSelfRef.test_tile_engine__DOT__dut__DOT__pal_ram[11U] = 0x0333U;
    vlSelfRef.test_tile_engine__DOT__dut__DOT__pal_ram[12U] = 0x0777U;
    vlSelfRef.test_tile_engine__DOT__dut__DOT__pal_ram[13U] = 0x08f6U;
    vlSelfRef.test_tile_engine__DOT__dut__DOT__pal_ram[14U] = 0x008fU;
    vlSelfRef.test_tile_engine__DOT__dut__DOT__pal_ram[15U] = 0x0bbbU;
    test_tile_engine__DOT__dut__DOT__unnamedblk4__DOT__i = 0x00000010U;
    while (VL_GTS_III(32, 0x00000100U, test_tile_engine__DOT__dut__DOT__unnamedblk4__DOT__i)) {
        vlSelfRef.test_tile_engine__DOT__dut__DOT__pal_ram[(0x000000ffU 
                                                            & test_tile_engine__DOT__dut__DOT__unnamedblk4__DOT__i)] = 0U;
        test_tile_engine__DOT__dut__DOT__unnamedblk4__DOT__i 
            = ((IData)(1U) + test_tile_engine__DOT__dut__DOT__unnamedblk4__DOT__i);
    }
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

void Vtest_tile_engine___024root___act_sequent__TOP__0(Vtest_tile_engine___024root* vlSelf);

VL_ATTR_COLD void Vtest_tile_engine___024root___eval_stl(Vtest_tile_engine___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vtest_tile_engine___024root___eval_stl\n"); );
    Vtest_tile_engine__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
    // Body
    if ((1ULL & vlSelfRef.__VstlTriggered[0U])) {
        Vtest_tile_engine___024root___act_sequent__TOP__0(vlSelf);
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
    vlSelf->test_tile_engine__DOT__pixel_x = VL_SCOPED_RAND_RESET_I(9, __VscopeHash, 16806960818367224977ull);
    vlSelf->test_tile_engine__DOT__pixel_y = VL_SCOPED_RAND_RESET_I(8, __VscopeHash, 808042982529873637ull);
    vlSelf->test_tile_engine__DOT__pixel_valid = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 10333621867875636360ull);
    for (int __Vi0 = 0; __Vi0 < 65536; ++__Vi0) {
        vlSelf->test_tile_engine__DOT__sim_ram[__Vi0] = VL_SCOPED_RAND_RESET_I(8, __VscopeHash, 17305522637293788989ull);
    }
    vlSelf->test_tile_engine__DOT__dut__DOT____Vlvbound_h021f5fae__0 = 0;
    vlSelf->test_tile_engine__DOT__dut__DOT____Vlvbound_h02fc01d7__0 = 0;
    vlSelf->test_tile_engine__DOT__dut__DOT____Vlvbound_h67f0dfac__0 = 0;
    vlSelf->test_tile_engine__DOT__dut__DOT____Vlvbound_ha057723b__0 = 0;
    vlSelf->test_tile_engine__DOT__dut__DOT____Vlvbound_h8898988b__0 = 0;
    vlSelf->test_tile_engine__DOT__dut__DOT____Vlvbound_ha3d87205__0 = 0;
    vlSelf->test_tile_engine__DOT__dut__DOT____Vlvbound_h353a15af__0 = 0;
    vlSelf->test_tile_engine__DOT__dut__DOT____Vlvbound_hf7076230__0 = 0;
    vlSelf->test_tile_engine__DOT__dut__DOT____Vlvbound_hf26d05a2__0 = 0;
    vlSelf->test_tile_engine__DOT__dut__DOT____Vlvbound_h8c784627__0 = 0;
    vlSelf->test_tile_engine__DOT__dut__DOT____Vlvbound_h02aa0280__0 = 0;
    vlSelf->test_tile_engine__DOT__dut__DOT____Vlvbound_h21da2949__0 = 0;
    vlSelf->test_tile_engine__DOT__dut__DOT____Vlvbound_h17e6e432__0 = 0;
    vlSelf->test_tile_engine__DOT__dut__DOT____Vlvbound_h3335cc4a__0 = 0;
    vlSelf->test_tile_engine__DOT__dut__DOT____Vlvbound_ha65a9a44__0 = 0;
    vlSelf->test_tile_engine__DOT__dut__DOT____Vlvbound_h7ed4573b__0 = 0;
    vlSelf->test_tile_engine__DOT__dut__DOT____Vlvbound_h9afe376d__0 = 0;
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
    for (int __Vi0 = 0; __Vi0 < 32768; ++__Vi0) {
        vlSelf->test_tile_engine__DOT__dut__DOT__tile_data[__Vi0] = VL_SCOPED_RAND_RESET_I(8, __VscopeHash, 10674721916555069836ull);
    }
    for (int __Vi0 = 0; __Vi0 < 4000; ++__Vi0) {
        vlSelf->test_tile_engine__DOT__dut__DOT__nametable[__Vi0] = VL_SCOPED_RAND_RESET_I(8, __VscopeHash, 18139926217463334632ull);
    }
    for (int __Vi0 = 0; __Vi0 < 4000; ++__Vi0) {
        vlSelf->test_tile_engine__DOT__dut__DOT__attr_table[__Vi0] = VL_SCOPED_RAND_RESET_I(8, __VscopeHash, 12896422421892742157ull);
    }
    for (int __Vi0 = 0; __Vi0 < 256; ++__Vi0) {
        vlSelf->test_tile_engine__DOT__dut__DOT__pal_ram[__Vi0] = VL_SCOPED_RAND_RESET_I(12, __VscopeHash, 9033458642578165358ull);
    }
    vlSelf->test_tile_engine__DOT__dut__DOT__tile_reg_sel = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 6866910759957633082ull);
    vlSelf->test_tile_engine__DOT__dut__DOT__dma_state = VL_SCOPED_RAND_RESET_I(4, __VscopeHash, 12778813815004487646ull);
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
    vlSelf->test_tile_engine__DOT__dut__DOT__cur_attr = VL_SCOPED_RAND_RESET_I(8, __VscopeHash, 6731814168252793486ull);
    vlSelf->test_tile_engine__DOT__dut__DOT__color_idx = VL_SCOPED_RAND_RESET_I(4, __VscopeHash, 934625216854238881ull);
    vlSelf->test_tile_engine__DOT__dut__DOT__unnamedblk6__DOT__i = 0;
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
