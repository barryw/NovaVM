// Verilated -*- C++ -*-
// DESCRIPTION: Verilator output: Design implementation internals
// See Vtest_blitter.h for the primary calling header

#include "Vtest_blitter__pch.h"

void Vtest_blitter___024root___timing_ready(Vtest_blitter___024root* vlSelf);

VL_ATTR_COLD void Vtest_blitter___024root___eval_static(Vtest_blitter___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vtest_blitter___024root___eval_static\n"); );
    Vtest_blitter__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
    // Body
    vlSelfRef.test_blitter__DOT__clk = 0U;
    vlSelfRef.test_blitter__DOT__pass_count = 0U;
    vlSelfRef.test_blitter__DOT__fail_count = 0U;
    vlSelfRef.test_blitter__DOT__test_num = 0U;
    vlSelfRef.__Vtrigprevexpr___TOP__test_blitter__DOT__clk__0 = 0U;
    Vtest_blitter___024root___timing_ready(vlSelf);
    do {
        vlSelfRef.__VactTriggeredAcc[vlSelfRef.__Vi] 
            = vlSelfRef.__VactTriggered[vlSelfRef.__Vi];
        vlSelfRef.__Vi = ((IData)(1U) + vlSelfRef.__Vi);
    } while ((0U >= vlSelfRef.__Vi));
}

VL_ATTR_COLD void Vtest_blitter___024root___eval_static__TOP(Vtest_blitter___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vtest_blitter___024root___eval_static__TOP\n"); );
    Vtest_blitter__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
    // Body
    vlSelfRef.test_blitter__DOT__clk = 0U;
    vlSelfRef.test_blitter__DOT__pass_count = 0U;
    vlSelfRef.test_blitter__DOT__fail_count = 0U;
    vlSelfRef.test_blitter__DOT__test_num = 0U;
}

VL_ATTR_COLD void Vtest_blitter___024root___eval_initial__TOP(Vtest_blitter___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vtest_blitter___024root___eval_initial__TOP\n"); );
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
}

VL_ATTR_COLD void Vtest_blitter___024root___eval_final(Vtest_blitter___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vtest_blitter___024root___eval_final\n"); );
    Vtest_blitter__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
}

#ifdef VL_DEBUG
VL_ATTR_COLD void Vtest_blitter___024root___dump_triggers__stl(const VlUnpacked<QData/*63:0*/, 1> &triggers, const std::string &tag);
#endif  // VL_DEBUG
VL_ATTR_COLD bool Vtest_blitter___024root___eval_phase__stl(Vtest_blitter___024root* vlSelf);

VL_ATTR_COLD void Vtest_blitter___024root___eval_settle(Vtest_blitter___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vtest_blitter___024root___eval_settle\n"); );
    Vtest_blitter__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
    // Locals
    IData/*31:0*/ __VstlIterCount;
    // Body
    __VstlIterCount = 0U;
    vlSelfRef.__VstlFirstIteration = 1U;
    do {
        if (VL_UNLIKELY(((0x00000064U < __VstlIterCount)))) {
#ifdef VL_DEBUG
            Vtest_blitter___024root___dump_triggers__stl(vlSelfRef.__VstlTriggered, "stl"s);
#endif
            VL_FATAL_MT("test_blitter.sv", 6, "", "DIDNOTCONVERGE: Settle region did not converge after '--converge-limit' of 100 tries");
        }
        __VstlIterCount = ((IData)(1U) + __VstlIterCount);
        vlSelfRef.__VstlPhaseResult = Vtest_blitter___024root___eval_phase__stl(vlSelf);
        vlSelfRef.__VstlFirstIteration = 0U;
    } while (vlSelfRef.__VstlPhaseResult);
}

VL_ATTR_COLD void Vtest_blitter___024root___eval_triggers_vec__stl(Vtest_blitter___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vtest_blitter___024root___eval_triggers_vec__stl\n"); );
    Vtest_blitter__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
    // Body
    vlSelfRef.__VstlTriggered[0U] = ((0xfffffffffffffffeULL 
                                      & vlSelfRef.__VstlTriggered[0U]) 
                                     | (IData)((IData)(vlSelfRef.__VstlFirstIteration)));
}

VL_ATTR_COLD bool Vtest_blitter___024root___trigger_anySet__stl(const VlUnpacked<QData/*63:0*/, 1> &in);

#ifdef VL_DEBUG
VL_ATTR_COLD void Vtest_blitter___024root___dump_triggers__stl(const VlUnpacked<QData/*63:0*/, 1> &triggers, const std::string &tag) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vtest_blitter___024root___dump_triggers__stl\n"); );
    // Body
    if ((1U & (~ (IData)(Vtest_blitter___024root___trigger_anySet__stl(triggers))))) {
        VL_DBG_MSGS("         No '" + tag + "' region triggers active\n");
    }
    if ((1U & (IData)(triggers[0U]))) {
        VL_DBG_MSGS("         '" + tag + "' region trigger index 0 is active: Internal 'stl' trigger - first iteration\n");
    }
}
#endif  // VL_DEBUG

VL_ATTR_COLD bool Vtest_blitter___024root___trigger_anySet__stl(const VlUnpacked<QData/*63:0*/, 1> &in) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vtest_blitter___024root___trigger_anySet__stl\n"); );
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

VL_ATTR_COLD void Vtest_blitter___024root___stl_sequent__TOP__0(Vtest_blitter___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vtest_blitter___024root___stl_sequent__TOP__0\n"); );
    Vtest_blitter__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
    // Locals
    IData/*23:0*/ test_blitter__DOT__dut__DOT__src_addr;
    test_blitter__DOT__dut__DOT__src_addr = 0;
    IData/*23:0*/ test_blitter__DOT__dut__DOT__dst_addr;
    test_blitter__DOT__dut__DOT__dst_addr = 0;
    // Body
    vlSelfRef.test_blitter__DOT__dut__DOT__blt_sel 
        = ((0xba83U <= (IData)(vlSelfRef.test_blitter__DOT__cpu_addr)) 
           & (0xba9bU >= (IData)(vlSelfRef.test_blitter__DOT__cpu_addr)));
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

VL_ATTR_COLD void Vtest_blitter___024root___eval_stl(Vtest_blitter___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vtest_blitter___024root___eval_stl\n"); );
    Vtest_blitter__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
    // Body
    if ((1ULL & vlSelfRef.__VstlTriggered[0U])) {
        Vtest_blitter___024root___stl_sequent__TOP__0(vlSelf);
    }
}

VL_ATTR_COLD bool Vtest_blitter___024root___eval_phase__stl(Vtest_blitter___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vtest_blitter___024root___eval_phase__stl\n"); );
    Vtest_blitter__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
    // Locals
    CData/*0:0*/ __VstlExecute;
    // Body
    Vtest_blitter___024root___eval_triggers_vec__stl(vlSelf);
#ifdef VL_DEBUG
    if (VL_UNLIKELY(vlSymsp->_vm_contextp__->debug())) {
        Vtest_blitter___024root___dump_triggers__stl(vlSelfRef.__VstlTriggered, "stl"s);
    }
#endif
    __VstlExecute = Vtest_blitter___024root___trigger_anySet__stl(vlSelfRef.__VstlTriggered);
    if (__VstlExecute) {
        Vtest_blitter___024root___eval_stl(vlSelf);
    }
    return (__VstlExecute);
}

bool Vtest_blitter___024root___trigger_anySet__act(const VlUnpacked<QData/*63:0*/, 1> &in);

#ifdef VL_DEBUG
VL_ATTR_COLD void Vtest_blitter___024root___dump_triggers__act(const VlUnpacked<QData/*63:0*/, 1> &triggers, const std::string &tag) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vtest_blitter___024root___dump_triggers__act\n"); );
    // Body
    if ((1U & (~ (IData)(Vtest_blitter___024root___trigger_anySet__act(triggers))))) {
        VL_DBG_MSGS("         No '" + tag + "' region triggers active\n");
    }
    if ((1U & (IData)(triggers[0U]))) {
        VL_DBG_MSGS("         '" + tag + "' region trigger index 0 is active: @(posedge test_blitter.clk)\n");
    }
    if ((1U & (IData)((triggers[0U] >> 1U)))) {
        VL_DBG_MSGS("         '" + tag + "' region trigger index 1 is active: @([true] __VdlySched.awaitingCurrentTime())\n");
    }
}
#endif  // VL_DEBUG

VL_ATTR_COLD void Vtest_blitter___024root___ctor_var_reset(Vtest_blitter___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vtest_blitter___024root___ctor_var_reset\n"); );
    Vtest_blitter__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
    // Body
    vlSelf->test_blitter__DOT____Vlvbound_h321272e9__0 = 0;
    vlSelf->test_blitter__DOT____Vlvbound_h22a4cf02__0 = 0;
    vlSelf->test_blitter__DOT____Vlvbound_h7d2f9654__0 = 0;
    vlSelf->test_blitter__DOT____Vlvbound_ha699a557__0 = 0;
    vlSelf->test_blitter__DOT____Vlvbound_h530fe462__0 = 0;
    vlSelf->test_blitter__DOT____Vlvbound_h6c644df4__0 = 0;
    const uint64_t __VscopeHash = VL_MURMUR64_HASH(vlSelf->vlNamep);
    vlSelf->test_blitter__DOT__rst = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 13102127282857907571ull);
    vlSelf->test_blitter__DOT__cpu_addr = VL_SCOPED_RAND_RESET_I(16, __VscopeHash, 13398723170856885241ull);
    vlSelf->test_blitter__DOT__cpu_wdata = VL_SCOPED_RAND_RESET_I(8, __VscopeHash, 13072457679268700451ull);
    vlSelf->test_blitter__DOT__cpu_we = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 10386328090574741384ull);
    vlSelf->test_blitter__DOT__blt_ram_addr = VL_SCOPED_RAND_RESET_I(16, __VscopeHash, 16006327654527272056ull);
    vlSelf->test_blitter__DOT__blt_ram_wdata = VL_SCOPED_RAND_RESET_I(8, __VscopeHash, 5171577316021078558ull);
    vlSelf->test_blitter__DOT__blt_ram_we = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 3620263454707716719ull);
    vlSelf->test_blitter__DOT__blt_xram_addr = VL_SCOPED_RAND_RESET_I(19, __VscopeHash, 16775371463360001444ull);
    vlSelf->test_blitter__DOT__blt_xram_wdata = VL_SCOPED_RAND_RESET_I(8, __VscopeHash, 4396367026175944480ull);
    vlSelf->test_blitter__DOT__blt_xram_we = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 15790602762628117766ull);
    vlSelf->test_blitter__DOT__blt_vgc_space = VL_SCOPED_RAND_RESET_I(3, __VscopeHash, 12079312253823336794ull);
    vlSelf->test_blitter__DOT__blt_vgc_addr = VL_SCOPED_RAND_RESET_I(16, __VscopeHash, 8724349500964647810ull);
    vlSelf->test_blitter__DOT__blt_vgc_wdata = VL_SCOPED_RAND_RESET_I(8, __VscopeHash, 18268901709601293052ull);
    vlSelf->test_blitter__DOT__blt_vgc_we = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 567511337920493641ull);
    for (int __Vi0 = 0; __Vi0 < 65536; ++__Vi0) {
        vlSelf->test_blitter__DOT__sim_ram[__Vi0] = VL_SCOPED_RAND_RESET_I(8, __VscopeHash, 4189634969461330161ull);
    }
    for (int __Vi0 = 0; __Vi0 < 524288; ++__Vi0) {
        vlSelf->test_blitter__DOT__sim_xram[__Vi0] = VL_SCOPED_RAND_RESET_I(8, __VscopeHash, 14043922791405614104ull);
    }
    for (int __Vi0 = 0; __Vi0 < 2000; ++__Vi0) {
        vlSelf->test_blitter__DOT__sim_char[__Vi0] = VL_SCOPED_RAND_RESET_I(8, __VscopeHash, 5135205836247557936ull);
    }
    for (int __Vi0 = 0; __Vi0 < 2000; ++__Vi0) {
        vlSelf->test_blitter__DOT__sim_color[__Vi0] = VL_SCOPED_RAND_RESET_I(8, __VscopeHash, 15731402088596588180ull);
    }
    for (int __Vi0 = 0; __Vi0 < 64000; ++__Vi0) {
        vlSelf->test_blitter__DOT__sim_gfx[__Vi0] = VL_SCOPED_RAND_RESET_I(4, __VscopeHash, 12424554438560545510ull);
    }
    for (int __Vi0 = 0; __Vi0 < 32768; ++__Vi0) {
        vlSelf->test_blitter__DOT__sim_sprite[__Vi0] = VL_SCOPED_RAND_RESET_I(8, __VscopeHash, 13788502702903057061ull);
    }
    vlSelf->test_blitter__DOT__dut__DOT____Vlvbound_hf31f73b8__0 = 0;
    for (int __Vi0 = 0; __Vi0 < 25; ++__Vi0) {
        vlSelf->test_blitter__DOT__dut__DOT__regs[__Vi0] = VL_SCOPED_RAND_RESET_I(8, __VscopeHash, 7241494770887870380ull);
    }
    vlSelf->test_blitter__DOT__dut__DOT__state = VL_SCOPED_RAND_RESET_I(4, __VscopeHash, 1217953817807473203ull);
    vlSelf->test_blitter__DOT__dut__DOT__fill_mode = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 13678727835839880550ull);
    vlSelf->test_blitter__DOT__dut__DOT__colorkey_mode = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 12639176561173122714ull);
    vlSelf->test_blitter__DOT__dut__DOT__use_row_buffer = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 1067011790798993905ull);
    vlSelf->test_blitter__DOT__dut__DOT__src_space = VL_SCOPED_RAND_RESET_I(3, __VscopeHash, 1559048981210338275ull);
    vlSelf->test_blitter__DOT__dut__DOT__dst_space = VL_SCOPED_RAND_RESET_I(3, __VscopeHash, 176460653153472080ull);
    vlSelf->test_blitter__DOT__dut__DOT__src_base = VL_SCOPED_RAND_RESET_I(24, __VscopeHash, 15190195500338983060ull);
    vlSelf->test_blitter__DOT__dut__DOT__dst_base = VL_SCOPED_RAND_RESET_I(24, __VscopeHash, 10801616813530765565ull);
    vlSelf->test_blitter__DOT__dut__DOT__width = VL_SCOPED_RAND_RESET_I(16, __VscopeHash, 15122252347165950220ull);
    vlSelf->test_blitter__DOT__dut__DOT__height = VL_SCOPED_RAND_RESET_I(16, __VscopeHash, 16996325577819280147ull);
    vlSelf->test_blitter__DOT__dut__DOT__src_stride = VL_SCOPED_RAND_RESET_I(16, __VscopeHash, 5338133683549195610ull);
    vlSelf->test_blitter__DOT__dut__DOT__dst_stride = VL_SCOPED_RAND_RESET_I(16, __VscopeHash, 10588061757026311644ull);
    vlSelf->test_blitter__DOT__dut__DOT__fill_value = VL_SCOPED_RAND_RESET_I(8, __VscopeHash, 823067432164739411ull);
    vlSelf->test_blitter__DOT__dut__DOT__color_key = VL_SCOPED_RAND_RESET_I(8, __VscopeHash, 8796042580351210832ull);
    vlSelf->test_blitter__DOT__dut__DOT__row = VL_SCOPED_RAND_RESET_I(16, __VscopeHash, 7215023620044785552ull);
    vlSelf->test_blitter__DOT__dut__DOT__col = VL_SCOPED_RAND_RESET_I(16, __VscopeHash, 10647590197375020595ull);
    vlSelf->test_blitter__DOT__dut__DOT__wrote_count = VL_SCOPED_RAND_RESET_I(24, __VscopeHash, 9601661118147430461ull);
    vlSelf->test_blitter__DOT__dut__DOT__read_byte = VL_SCOPED_RAND_RESET_I(8, __VscopeHash, 18111659607525560882ull);
    vlSelf->test_blitter__DOT__dut__DOT__read_valid = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 5524564942600177811ull);
    for (int __Vi0 = 0; __Vi0 < 1024; ++__Vi0) {
        vlSelf->test_blitter__DOT__dut__DOT__row_buf[__Vi0] = VL_SCOPED_RAND_RESET_I(8, __VscopeHash, 16717538731729331922ull);
    }
    vlSelf->test_blitter__DOT__dut__DOT__buf_idx = VL_SCOPED_RAND_RESET_I(10, __VscopeHash, 13110899488168742332ull);
    vlSelf->test_blitter__DOT__dut__DOT__blt_sel = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 8070199571880686144ull);
    vlSelf->test_blitter__DOT__dut__DOT__mem_read_data = VL_SCOPED_RAND_RESET_I(8, __VscopeHash, 11268844307605653295ull);
    vlSelf->__Vtask_test_blitter__DOT__write_reg__661__data = 0;
    for (int __Vi0 = 0; __Vi0 < 1; ++__Vi0) {
        vlSelf->__VstlTriggered[__Vi0] = 0;
    }
    for (int __Vi0 = 0; __Vi0 < 1; ++__Vi0) {
        vlSelf->__VactTriggered[__Vi0] = 0;
    }
    for (int __Vi0 = 0; __Vi0 < 1; ++__Vi0) {
        vlSelf->__VactTriggeredAcc[__Vi0] = 0;
    }
    vlSelf->__Vtrigprevexpr___TOP__test_blitter__DOT__clk__0 = 0;
    for (int __Vi0 = 0; __Vi0 < 1; ++__Vi0) {
        vlSelf->__VnbaTriggered[__Vi0] = 0;
    }
    vlSelf->__Vi = 0;
}
