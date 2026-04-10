// Verilated -*- C++ -*-
// DESCRIPTION: Verilator output: Design implementation internals
// See Vvgc.h for the primary calling header

#include "Vvgc__pch.h"

VL_ATTR_COLD void Vvgc___024root___eval_static(Vvgc___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vvgc___024root___eval_static\n"); );
    Vvgc__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
    // Body
    vlSelfRef.__Vtrigprevexpr___TOP__vgc__DOT__clk__0 
        = vlSelfRef.vgc__DOT__clk;
}

VL_ATTR_COLD void Vvgc___024root___eval_initial__TOP(Vvgc___024root* vlSelf);

VL_ATTR_COLD void Vvgc___024root___eval_initial(Vvgc___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vvgc___024root___eval_initial\n"); );
    Vvgc__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
    // Body
    Vvgc___024root___eval_initial__TOP(vlSelf);
}

VL_ATTR_COLD void Vvgc___024root___eval_initial__TOP(Vvgc___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vvgc___024root___eval_initial__TOP\n"); );
    Vvgc__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
    // Body
    vlSelfRef.vgc__DOT__palette[0U] = 0U;
    vlSelfRef.vgc__DOT__palette[1U] = 0x0fffU;
    vlSelfRef.vgc__DOT__palette[2U] = 0x0800U;
    vlSelfRef.vgc__DOT__palette[3U] = 0x0afeU;
    vlSelfRef.vgc__DOT__palette[4U] = 0x0c4cU;
    vlSelfRef.vgc__DOT__palette[5U] = 0x00c5U;
    vlSelfRef.vgc__DOT__palette[6U] = 0x000aU;
    vlSelfRef.vgc__DOT__palette[7U] = 0x0ee7U;
    vlSelfRef.vgc__DOT__palette[8U] = 0x0d85U;
    vlSelfRef.vgc__DOT__palette[9U] = 0x0640U;
    vlSelfRef.vgc__DOT__palette[10U] = 0x0f77U;
    vlSelfRef.vgc__DOT__palette[11U] = 0x0333U;
    vlSelfRef.vgc__DOT__palette[12U] = 0x0777U;
    vlSelfRef.vgc__DOT__palette[13U] = 0x08f6U;
    vlSelfRef.vgc__DOT__palette[14U] = 0x008fU;
    vlSelfRef.vgc__DOT__palette[15U] = 0x0bbbU;
    vlSelfRef.vgc__DOT__unnamedblk1__DOT__i = 0U;
    while (VL_GTS_III(32, 0x000007d0U, vlSelfRef.vgc__DOT__unnamedblk1__DOT__i)) {
        vlSelfRef.vgc__DOT____Vlvbound_h3318332a__0 = 0x20U;
        vlSelfRef.vgc__DOT____Vlvbound_h45ddebd4__0 = 1U;
        if (VL_LIKELY(((0x07cfU >= (0x000007ffU & vlSelfRef.vgc__DOT__unnamedblk1__DOT__i))))) {
            vlSelfRef.vgc__DOT__char_ram[(0x000007ffU 
                                          & vlSelfRef.vgc__DOT__unnamedblk1__DOT__i)] 
                = vlSelfRef.vgc__DOT____Vlvbound_h3318332a__0;
            vlSelfRef.vgc__DOT__color_ram[(0x000007ffU 
                                           & vlSelfRef.vgc__DOT__unnamedblk1__DOT__i)] 
                = vlSelfRef.vgc__DOT____Vlvbound_h45ddebd4__0;
        }
        vlSelfRef.vgc__DOT__unnamedblk1__DOT__i = ((IData)(1U) 
                                                   + vlSelfRef.vgc__DOT__unnamedblk1__DOT__i);
    }
    vlSelfRef.vgc__DOT__unnamedblk2__DOT__i = 0U;
    while (VL_GTS_III(32, 0x0000fa00U, vlSelfRef.vgc__DOT__unnamedblk2__DOT__i)) {
        vlSelfRef.vgc__DOT____Vlvbound_h652b6de2__0 = 0U;
        if (VL_LIKELY(((0xf9ffU >= (0x0000ffffU & vlSelfRef.vgc__DOT__unnamedblk2__DOT__i))))) {
            vlSelfRef.vgc__DOT__gfx_ram[(0x0000ffffU 
                                         & vlSelfRef.vgc__DOT__unnamedblk2__DOT__i)] 
                = vlSelfRef.vgc__DOT____Vlvbound_h652b6de2__0;
        }
        vlSelfRef.vgc__DOT__unnamedblk2__DOT__i = ((IData)(1U) 
                                                   + vlSelfRef.vgc__DOT__unnamedblk2__DOT__i);
    }
    vlSelfRef.vgc__DOT__unnamedblk3__DOT__i = 0U;
    while (VL_GTS_III(32, 0x00000800U, vlSelfRef.vgc__DOT__unnamedblk3__DOT__i)) {
        vlSelfRef.vgc__DOT__sprite_shapes[(0x000007ffU 
                                           & vlSelfRef.vgc__DOT__unnamedblk3__DOT__i)] = 0U;
        vlSelfRef.vgc__DOT__unnamedblk3__DOT__i = ((IData)(1U) 
                                                   + vlSelfRef.vgc__DOT__unnamedblk3__DOT__i);
    }
    vlSelfRef.vgc__DOT__unnamedblk4__DOT__i = 0U;
    while (VL_GTS_III(32, 0x00000010U, vlSelfRef.vgc__DOT__unnamedblk4__DOT__i)) {
        vlSelfRef.vgc__DOT__spr_x[(0x0000000fU & vlSelfRef.vgc__DOT__unnamedblk4__DOT__i)] = 0U;
        vlSelfRef.vgc__DOT__spr_y[(0x0000000fU & vlSelfRef.vgc__DOT__unnamedblk4__DOT__i)] = 0U;
        vlSelfRef.vgc__DOT__spr_enable[(0x0000000fU 
                                        & vlSelfRef.vgc__DOT__unnamedblk4__DOT__i)] = 0U;
        vlSelfRef.vgc__DOT__spr_flip_h[(0x0000000fU 
                                        & vlSelfRef.vgc__DOT__unnamedblk4__DOT__i)] = 0U;
        vlSelfRef.vgc__DOT__spr_flip_v[(0x0000000fU 
                                        & vlSelfRef.vgc__DOT__unnamedblk4__DOT__i)] = 0U;
        vlSelfRef.vgc__DOT__spr_pri[(0x0000000fU & vlSelfRef.vgc__DOT__unnamedblk4__DOT__i)] = 1U;
        vlSelfRef.vgc__DOT__spr_shape[(0x0000000fU 
                                       & vlSelfRef.vgc__DOT__unnamedblk4__DOT__i)] 
            = (0x0000000fU & vlSelfRef.vgc__DOT__unnamedblk4__DOT__i);
        vlSelfRef.vgc__DOT__unnamedblk4__DOT__i = ((IData)(1U) 
                                                   + vlSelfRef.vgc__DOT__unnamedblk4__DOT__i);
    }
    VL_READMEM_N(true, 8, 2048, 0, "rom/cp437.hex"s
                 ,  &(vlSelfRef.vgc__DOT__font_rom)
                 , 0, ~0ULL);
    vlSelfRef.vgc__DOT__unnamedblk5__DOT__i = 0U;
    while (VL_GTS_III(32, 0x00000100U, vlSelfRef.vgc__DOT__unnamedblk5__DOT__i)) {
        vlSelfRef.vgc__DOT__copper_pos[(0x000000ffU 
                                        & vlSelfRef.vgc__DOT__unnamedblk5__DOT__i)] = 0U;
        vlSelfRef.vgc__DOT__copper_reg[(0x000000ffU 
                                        & vlSelfRef.vgc__DOT__unnamedblk5__DOT__i)] = 0U;
        vlSelfRef.vgc__DOT__copper_val[(0x000000ffU 
                                        & vlSelfRef.vgc__DOT__unnamedblk5__DOT__i)] = 0U;
        vlSelfRef.vgc__DOT__unnamedblk5__DOT__i = ((IData)(1U) 
                                                   + vlSelfRef.vgc__DOT__unnamedblk5__DOT__i);
    }
    vlSelfRef.vgc__DOT__copper_count = 0U;
    vlSelfRef.vgc__DOT__copper_index = 0U;
    vlSelfRef.vgc__DOT__copper_enabled = 0U;
    vlSelfRef.vgc__DOT__unnamedblk6__DOT__i = 0U;
    while (VL_GTS_III(32, 0x00000020U, vlSelfRef.vgc__DOT__unnamedblk6__DOT__i)) {
        vlSelfRef.vgc__DOT__regs[(0x0000001fU & vlSelfRef.vgc__DOT__unnamedblk6__DOT__i)] = 0U;
        vlSelfRef.vgc__DOT__unnamedblk6__DOT__i = ((IData)(1U) 
                                                   + vlSelfRef.vgc__DOT__unnamedblk6__DOT__i);
    }
    vlSelfRef.vgc__DOT__cursor_x = 0U;
    vlSelfRef.vgc__DOT__cursor_y = 0U;
    vlSelfRef.vgc__DOT__fg_color = 1U;
    vlSelfRef.vgc__DOT__bg_color = 6U;
    vlSelfRef.vgc__DOT__border_color = 6U;
    vlSelfRef.vgc__DOT__gfx_color = 1U;
    vlSelfRef.vgc__DOT__mode = 0U;
    vlSelfRef.vgc__DOT__scroll_offset = 0U;
    vlSelfRef.vgc__DOT__scroll_pending = 0U;
    vlSelfRef.vgc__DOT__scroll_col = 0U;
    vlSelfRef.vgc__DOT__char_in_reg = 0U;
    vlSelfRef.vgc__DOT__cmd_busy = 0U;
}

VL_ATTR_COLD void Vvgc___024root___eval_final(Vvgc___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vvgc___024root___eval_final\n"); );
    Vvgc__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
}

#ifdef VL_DEBUG
VL_ATTR_COLD void Vvgc___024root___dump_triggers__stl(const VlUnpacked<QData/*63:0*/, 1> &triggers, const std::string &tag);
#endif  // VL_DEBUG
VL_ATTR_COLD bool Vvgc___024root___eval_phase__stl(Vvgc___024root* vlSelf);

VL_ATTR_COLD void Vvgc___024root___eval_settle(Vvgc___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vvgc___024root___eval_settle\n"); );
    Vvgc__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
    // Locals
    IData/*31:0*/ __VstlIterCount;
    // Body
    __VstlIterCount = 0U;
    vlSelfRef.__VstlFirstIteration = 1U;
    do {
        if (VL_UNLIKELY(((0x00000064U < __VstlIterCount)))) {
#ifdef VL_DEBUG
            Vvgc___024root___dump_triggers__stl(vlSelfRef.__VstlTriggered, "stl"s);
#endif
            VL_FATAL_MT("rtl/vgc.sv", 8, "", "DIDNOTCONVERGE: Settle region did not converge after '--converge-limit' of 100 tries");
        }
        __VstlIterCount = ((IData)(1U) + __VstlIterCount);
        vlSelfRef.__VstlPhaseResult = Vvgc___024root___eval_phase__stl(vlSelf);
        vlSelfRef.__VstlFirstIteration = 0U;
    } while (vlSelfRef.__VstlPhaseResult);
}

VL_ATTR_COLD void Vvgc___024root___eval_triggers_vec__stl(Vvgc___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vvgc___024root___eval_triggers_vec__stl\n"); );
    Vvgc__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
    // Body
    vlSelfRef.__VstlTriggered[0U] = ((0xfffffffffffffffeULL 
                                      & vlSelfRef.__VstlTriggered[0U]) 
                                     | (IData)((IData)(vlSelfRef.__VstlFirstIteration)));
}

VL_ATTR_COLD bool Vvgc___024root___trigger_anySet__stl(const VlUnpacked<QData/*63:0*/, 1> &in);

#ifdef VL_DEBUG
VL_ATTR_COLD void Vvgc___024root___dump_triggers__stl(const VlUnpacked<QData/*63:0*/, 1> &triggers, const std::string &tag) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vvgc___024root___dump_triggers__stl\n"); );
    // Body
    if ((1U & (~ (IData)(Vvgc___024root___trigger_anySet__stl(triggers))))) {
        VL_DBG_MSGS("         No '" + tag + "' region triggers active\n");
    }
    if ((1U & (IData)(triggers[0U]))) {
        VL_DBG_MSGS("         '" + tag + "' region trigger index 0 is active: Internal 'stl' trigger - first iteration\n");
    }
}
#endif  // VL_DEBUG

VL_ATTR_COLD bool Vvgc___024root___trigger_anySet__stl(const VlUnpacked<QData/*63:0*/, 1> &in) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vvgc___024root___trigger_anySet__stl\n"); );
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

VL_ATTR_COLD void Vvgc___024root___stl_sequent__TOP__0(Vvgc___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vvgc___024root___stl_sequent__TOP__0\n"); );
    Vvgc__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
    // Body
    vlSelfRef.vgc__DOT__h_sync_area = ((0x0290U <= (IData)(vlSelfRef.vgc__DOT__h_count)) 
                                       & (0x02f0U > (IData)(vlSelfRef.vgc__DOT__h_count)));
    vlSelfRef.vgc__DOT__v_sync_area = ((0x01eaU <= (IData)(vlSelfRef.vgc__DOT__v_count)) 
                                       & (0x01ecU > (IData)(vlSelfRef.vgc__DOT__v_count)));
    vlSelfRef.vgc__DOT__clk = vlSelfRef.clk;
    vlSelfRef.vgc__DOT__rst = vlSelfRef.rst;
    vlSelfRef.vgc__DOT__cpu_ce = vlSelfRef.cpu_ce;
    vlSelfRef.vgc__DOT__cpu_wdata = vlSelfRef.cpu_wdata;
    vlSelfRef.vgc__DOT__cpu_we = vlSelfRef.cpu_we;
    vlSelfRef.vgc__DOT__cpu_re = vlSelfRef.cpu_re;
    vlSelfRef.vgc__DOT__key_valid = vlSelfRef.key_valid;
    vlSelfRef.vgc__DOT__key_data = vlSelfRef.key_data;
    vlSelfRef.vid_r = vlSelfRef.vgc__DOT__vid_r;
    vlSelfRef.vid_g = vlSelfRef.vgc__DOT__vid_g;
    vlSelfRef.vid_b = vlSelfRef.vgc__DOT__vid_b;
    vlSelfRef.vid_hsync = vlSelfRef.vgc__DOT__vid_hsync;
    vlSelfRef.vid_vsync = vlSelfRef.vgc__DOT__vid_vsync;
    vlSelfRef.vid_de = vlSelfRef.vgc__DOT__vid_de;
    vlSelfRef.vgc__DOT__font_pixel = (7U & (IData)(vlSelfRef.vgc__DOT__h_count));
    vlSelfRef.vgc__DOT__gfx_x = (0x000001ffU & ((IData)(vlSelfRef.vgc__DOT__h_count) 
                                                >> 1U));
    vlSelfRef.vgc__DOT__text_col = (0x0000007fU & ((IData)(vlSelfRef.vgc__DOT__h_count) 
                                                   >> 3U));
    vlSelfRef.vgc__DOT__h_visible = (0x0280U > (IData)(vlSelfRef.vgc__DOT__h_count));
    vlSelfRef.vgc__DOT__v_visible = (0x01e0U > (IData)(vlSelfRef.vgc__DOT__v_count));
    vlSelfRef.vgc__DOT__cpu_addr = vlSelfRef.cpu_addr;
    vlSelfRef.vgc__DOT__text_line = (0x000003ffU & 
                                     (((IData)(vlSelfRef.vgc__DOT__v_count) 
                                       - (IData)(0x00000028U)) 
                                      >> 1U));
    vlSelfRef.vgc__DOT__text_row = (0x0000001fU & ((IData)(vlSelfRef.vgc__DOT__text_line) 
                                                   >> 3U));
    vlSelfRef.vgc__DOT__font_line = (7U & (IData)(vlSelfRef.vgc__DOT__text_line));
    vlSelfRef.vgc__DOT__real_row = (0x0000001fU & ((IData)(vlSelfRef.vgc__DOT__text_row) 
                                                   + (IData)(vlSelfRef.vgc__DOT__scroll_offset)));
    if ((0x19U <= (IData)(vlSelfRef.vgc__DOT__real_row))) {
        vlSelfRef.vgc__DOT__real_row = (0x0000001fU 
                                        & ((IData)(vlSelfRef.vgc__DOT__real_row) 
                                           - (IData)(0x19U)));
    }
    vlSelfRef.vgc__DOT__gfx_y = (0x000000ffU & (IData)(vlSelfRef.vgc__DOT__text_line));
    vlSelfRef.vgc__DOT__visible = ((IData)(vlSelfRef.vgc__DOT__h_visible) 
                                   & (IData)(vlSelfRef.vgc__DOT__v_visible));
    vlSelfRef.vgc__DOT__spr_reg_sel = ((0xa040U <= (IData)(vlSelfRef.vgc__DOT__cpu_addr)) 
                                       & (0xa0bfU >= (IData)(vlSelfRef.vgc__DOT__cpu_addr)));
    vlSelfRef.vgc__DOT__spr_index = (0x0000000fU & 
                                     ((IData)(vlSelfRef.vgc__DOT__cpu_addr) 
                                      >> 3U));
    vlSelfRef.vgc__DOT__spr_field = (7U & (IData)(vlSelfRef.vgc__DOT__cpu_addr));
    vlSelfRef.vgc__DOT__vgc_reg_sel = ((0xa000U <= (IData)(vlSelfRef.vgc__DOT__cpu_addr)) 
                                       & (0xa01fU >= (IData)(vlSelfRef.vgc__DOT__cpu_addr)));
    vlSelfRef.vgc__DOT__char_ram_sel = ((0xaa00U <= (IData)(vlSelfRef.vgc__DOT__cpu_addr)) 
                                        & (0xb1cfU 
                                           >= (IData)(vlSelfRef.vgc__DOT__cpu_addr)));
    vlSelfRef.vgc__DOT__color_ram_sel = ((0xb1d0U <= (IData)(vlSelfRef.vgc__DOT__cpu_addr)) 
                                         & (0xb99fU 
                                            >= (IData)(vlSelfRef.vgc__DOT__cpu_addr)));
    vlSelfRef.vgc__DOT__reg_offset = (0x0000001fU & (IData)(vlSelfRef.vgc__DOT__cpu_addr));
    vlSelfRef.vgc__DOT__char_offset = (0x000007ffU 
                                       & ((IData)(vlSelfRef.vgc__DOT__cpu_addr) 
                                          - (IData)(0x0200U)));
    vlSelfRef.vgc__DOT__color_offset = (0x000007ffU 
                                        & ((IData)(vlSelfRef.vgc__DOT__cpu_addr) 
                                           - (IData)(0x01d0U)));
    vlSelfRef.vgc__DOT__beam_pos = (0x0001ffffU & (
                                                   ((IData)(0x00000140U) 
                                                    * (IData)(vlSelfRef.vgc__DOT__gfx_y)) 
                                                   + (IData)(vlSelfRef.vgc__DOT__gfx_x)));
    vlSelfRef.vgc__DOT__in_text_area = ((IData)(vlSelfRef.vgc__DOT__visible) 
                                        & ((0x0028U 
                                            <= (IData)(vlSelfRef.vgc__DOT__v_count)) 
                                           & (0x01b8U 
                                              > (IData)(vlSelfRef.vgc__DOT__v_count))));
    vlSelfRef.vgc__DOT__cpu_rdata = 0U;
    if (vlSelfRef.vgc__DOT__vgc_reg_sel) {
        vlSelfRef.vgc__DOT__cpu_rdata = (((((((((0U 
                                                 == (IData)(vlSelfRef.vgc__DOT__reg_offset)) 
                                                | (1U 
                                                   == (IData)(vlSelfRef.vgc__DOT__reg_offset))) 
                                               | (2U 
                                                  == (IData)(vlSelfRef.vgc__DOT__reg_offset))) 
                                              | (3U 
                                                 == (IData)(vlSelfRef.vgc__DOT__reg_offset))) 
                                             | (4U 
                                                == (IData)(vlSelfRef.vgc__DOT__reg_offset))) 
                                            | (8U == (IData)(vlSelfRef.vgc__DOT__reg_offset))) 
                                           | (0x0dU 
                                              == (IData)(vlSelfRef.vgc__DOT__reg_offset))) 
                                          | (0x0fU 
                                             == (IData)(vlSelfRef.vgc__DOT__reg_offset)))
                                          ? ((0U == (IData)(vlSelfRef.vgc__DOT__reg_offset))
                                              ? (IData)(vlSelfRef.vgc__DOT__mode)
                                              : ((1U 
                                                  == (IData)(vlSelfRef.vgc__DOT__reg_offset))
                                                  ? (IData)(vlSelfRef.vgc__DOT__bg_color)
                                                  : 
                                                 ((2U 
                                                   == (IData)(vlSelfRef.vgc__DOT__reg_offset))
                                                   ? (IData)(vlSelfRef.vgc__DOT__fg_color)
                                                   : 
                                                  ((3U 
                                                    == (IData)(vlSelfRef.vgc__DOT__reg_offset))
                                                    ? (IData)(vlSelfRef.vgc__DOT__cursor_x)
                                                    : 
                                                   ((4U 
                                                     == (IData)(vlSelfRef.vgc__DOT__reg_offset))
                                                     ? (IData)(vlSelfRef.vgc__DOT__cursor_y)
                                                     : 
                                                    ((8U 
                                                      == (IData)(vlSelfRef.vgc__DOT__reg_offset))
                                                      ? (IData)(vlSelfRef.vgc__DOT__gfx_color)
                                                      : 
                                                     ((0x0dU 
                                                       == (IData)(vlSelfRef.vgc__DOT__reg_offset))
                                                       ? (IData)(vlSelfRef.vgc__DOT__border_color)
                                                       : (IData)(vlSelfRef.vgc__DOT__char_in_reg))))))))
                                          : ((0x10U 
                                              == (IData)(vlSelfRef.vgc__DOT__reg_offset))
                                              ? (IData)(vlSelfRef.vgc__DOT__cmd_busy)
                                              : vlSelfRef.vgc__DOT__regs
                                             [vlSelfRef.vgc__DOT__reg_offset]));
    } else if (vlSelfRef.vgc__DOT__char_ram_sel) {
        vlSelfRef.vgc__DOT__cpu_rdata = ((0x07cfU >= (IData)(vlSelfRef.vgc__DOT__char_offset))
                                          ? vlSelfRef.vgc__DOT__char_ram
                                         [vlSelfRef.vgc__DOT__char_offset]
                                          : 0U);
    } else if (vlSelfRef.vgc__DOT__color_ram_sel) {
        vlSelfRef.vgc__DOT__cpu_rdata = ((0x07cfU >= (IData)(vlSelfRef.vgc__DOT__color_offset))
                                          ? vlSelfRef.vgc__DOT__color_ram
                                         [vlSelfRef.vgc__DOT__color_offset]
                                          : 0U);
    }
    vlSelfRef.vgc__DOT__cursor_here = ((IData)(vlSelfRef.vgc__DOT__in_text_area) 
                                       & (((IData)(vlSelfRef.vgc__DOT__cursor_x) 
                                           == (IData)(vlSelfRef.vgc__DOT__text_col)) 
                                          & ((IData)(vlSelfRef.vgc__DOT__cursor_y) 
                                             == (IData)(vlSelfRef.vgc__DOT__text_row))));
    if ((0x07cfU >= (0x000007ffU & (((IData)(0x00000050U) 
                                     * (IData)(vlSelfRef.vgc__DOT__real_row)) 
                                    + (IData)(vlSelfRef.vgc__DOT__text_col))))) {
        vlSelfRef.vgc__DOT__cur_char = vlSelfRef.vgc__DOT__char_ram
            [(0x000007ffU & (((IData)(0x00000050U) 
                              * (IData)(vlSelfRef.vgc__DOT__real_row)) 
                             + (IData)(vlSelfRef.vgc__DOT__text_col)))];
        vlSelfRef.vgc__DOT__cur_fg = (0x0000000fU & vlSelfRef.vgc__DOT__color_ram
                                      [(0x000007ffU 
                                        & (((IData)(0x00000050U) 
                                            * (IData)(vlSelfRef.vgc__DOT__real_row)) 
                                           + (IData)(vlSelfRef.vgc__DOT__text_col)))]);
    } else {
        vlSelfRef.vgc__DOT__cur_char = 0U;
        vlSelfRef.vgc__DOT__cur_fg = 0U;
    }
    vlSelfRef.vgc__DOT__font_byte = vlSelfRef.vgc__DOT__font_rom
        [(((IData)(vlSelfRef.vgc__DOT__cur_char) << 3U) 
          | (IData)(vlSelfRef.vgc__DOT__font_line))];
    vlSelfRef.vgc__DOT__pixel_on = (1U & ((IData)(vlSelfRef.vgc__DOT__font_byte) 
                                          >> (7U & 
                                              ((IData)(7U) 
                                               - (IData)(vlSelfRef.vgc__DOT__font_pixel)))));
    vlSelfRef.vgc__DOT__text_pixel = ((IData)(vlSelfRef.vgc__DOT__pixel_on)
                                       ? vlSelfRef.vgc__DOT__palette
                                      [vlSelfRef.vgc__DOT__cur_fg]
                                       : vlSelfRef.vgc__DOT__palette
                                      [vlSelfRef.vgc__DOT__bg_color]);
    vlSelfRef.vgc__DOT__cur_gfx = ((0xf9ffU >= (0x0000ffffU 
                                                & (((IData)(0x00000140U) 
                                                    * (IData)(vlSelfRef.vgc__DOT__gfx_y)) 
                                                   + (IData)(vlSelfRef.vgc__DOT__gfx_x))))
                                    ? vlSelfRef.vgc__DOT__gfx_ram
                                   [(0x0000ffffU & 
                                     (((IData)(0x00000140U) 
                                       * (IData)(vlSelfRef.vgc__DOT__gfx_y)) 
                                      + (IData)(vlSelfRef.vgc__DOT__gfx_x)))]
                                    : 0U);
    vlSelfRef.vgc__DOT__gfx_pixel = vlSelfRef.vgc__DOT__palette
        [vlSelfRef.vgc__DOT__cur_gfx];
    vlSelfRef.vgc__DOT__unnamedblk7__DOT__s = 0U;
    while (VL_GTS_III(32, 0x00000010U, vlSelfRef.vgc__DOT__unnamedblk7__DOT__s)) {
        vlSelfRef.vgc__DOT__spr_dx[(0x0000000fU & vlSelfRef.vgc__DOT__unnamedblk7__DOT__s)] 
            = (0x000003ffU & ((IData)(vlSelfRef.vgc__DOT__gfx_x) 
                              - vlSelfRef.vgc__DOT__spr_x
                              [(0x0000000fU & vlSelfRef.vgc__DOT__unnamedblk7__DOT__s)]));
        vlSelfRef.vgc__DOT__spr_dy[(0x0000000fU & vlSelfRef.vgc__DOT__unnamedblk7__DOT__s)] 
            = (0x000003ffU & ((IData)(vlSelfRef.vgc__DOT__gfx_y) 
                              - vlSelfRef.vgc__DOT__spr_y
                              [(0x0000000fU & vlSelfRef.vgc__DOT__unnamedblk7__DOT__s)]));
        vlSelfRef.vgc__DOT__spr_fx[(0x0000000fU & vlSelfRef.vgc__DOT__unnamedblk7__DOT__s)] 
            = (0x0000000fU & (vlSelfRef.vgc__DOT__spr_flip_h
                              [(0x0000000fU & vlSelfRef.vgc__DOT__unnamedblk7__DOT__s)]
                               ? ((IData)(0x0fU) - vlSelfRef.vgc__DOT__spr_dx
                                  [(0x0000000fU & vlSelfRef.vgc__DOT__unnamedblk7__DOT__s)])
                               : vlSelfRef.vgc__DOT__spr_dx
                              [(0x0000000fU & vlSelfRef.vgc__DOT__unnamedblk7__DOT__s)]));
        vlSelfRef.vgc__DOT__spr_fy[(0x0000000fU & vlSelfRef.vgc__DOT__unnamedblk7__DOT__s)] 
            = (0x0000000fU & (vlSelfRef.vgc__DOT__spr_flip_v
                              [(0x0000000fU & vlSelfRef.vgc__DOT__unnamedblk7__DOT__s)]
                               ? ((IData)(0x0fU) - vlSelfRef.vgc__DOT__spr_dy
                                  [(0x0000000fU & vlSelfRef.vgc__DOT__unnamedblk7__DOT__s)])
                               : vlSelfRef.vgc__DOT__spr_dy
                              [(0x0000000fU & vlSelfRef.vgc__DOT__unnamedblk7__DOT__s)]));
        vlSelfRef.vgc__DOT__spr_sa[(0x0000000fU & vlSelfRef.vgc__DOT__unnamedblk7__DOT__s)] 
            = ((vlSelfRef.vgc__DOT__spr_shape[(0x0000000fU 
                                               & vlSelfRef.vgc__DOT__unnamedblk7__DOT__s)] 
                << 7U) | ((vlSelfRef.vgc__DOT__spr_fy
                           [(0x0000000fU & vlSelfRef.vgc__DOT__unnamedblk7__DOT__s)] 
                           << 3U) | (7U & (vlSelfRef.vgc__DOT__spr_fx
                                           [(0x0000000fU 
                                             & vlSelfRef.vgc__DOT__unnamedblk7__DOT__s)] 
                                           >> 1U))));
        vlSelfRef.vgc__DOT__spr_pix[(0x0000000fU & vlSelfRef.vgc__DOT__unnamedblk7__DOT__s)] 
            = (0x0000000fU & ((1U & vlSelfRef.vgc__DOT__spr_fx
                               [(0x0000000fU & vlSelfRef.vgc__DOT__unnamedblk7__DOT__s)])
                               ? vlSelfRef.vgc__DOT__sprite_shapes
                              [vlSelfRef.vgc__DOT__spr_sa
                              [(0x0000000fU & vlSelfRef.vgc__DOT__unnamedblk7__DOT__s)]]
                               : (vlSelfRef.vgc__DOT__sprite_shapes
                                  [vlSelfRef.vgc__DOT__spr_sa
                                  [(0x0000000fU & vlSelfRef.vgc__DOT__unnamedblk7__DOT__s)]] 
                                  >> 4U)));
        vlSelfRef.vgc__DOT__spr_vis[(0x0000000fU & vlSelfRef.vgc__DOT__unnamedblk7__DOT__s)] 
            = ((((((vlSelfRef.vgc__DOT__spr_enable[
                    (0x0000000fU & vlSelfRef.vgc__DOT__unnamedblk7__DOT__s)] 
                    & (IData)(vlSelfRef.vgc__DOT__in_text_area)) 
                   & VL_LTES_III(32, 0U, VL_EXTENDS_II(32,10, vlSelfRef.vgc__DOT__spr_dx
                                                       [
                                                       (0x0000000fU 
                                                        & vlSelfRef.vgc__DOT__unnamedblk7__DOT__s)]))) 
                  & VL_GTS_III(32, 0x00000010U, VL_EXTENDS_II(32,10, vlSelfRef.vgc__DOT__spr_dx
                                                              [
                                                              (0x0000000fU 
                                                               & vlSelfRef.vgc__DOT__unnamedblk7__DOT__s)]))) 
                 & VL_LTES_III(32, 0U, VL_EXTENDS_II(32,10, vlSelfRef.vgc__DOT__spr_dy
                                                     [
                                                     (0x0000000fU 
                                                      & vlSelfRef.vgc__DOT__unnamedblk7__DOT__s)]))) 
                & VL_GTS_III(32, 0x00000010U, VL_EXTENDS_II(32,10, vlSelfRef.vgc__DOT__spr_dy
                                                            [
                                                            (0x0000000fU 
                                                             & vlSelfRef.vgc__DOT__unnamedblk7__DOT__s)]))) 
               & (0U != vlSelfRef.vgc__DOT__spr_pix
                  [(0x0000000fU & vlSelfRef.vgc__DOT__unnamedblk7__DOT__s)]));
        vlSelfRef.vgc__DOT__unnamedblk7__DOT__s = ((IData)(1U) 
                                                   + vlSelfRef.vgc__DOT__unnamedblk7__DOT__s);
    }
    vlSelfRef.vgc__DOT__spr_pixel = 0U;
    vlSelfRef.vgc__DOT__spr_pixel_pri = 0U;
    vlSelfRef.vgc__DOT__spr_pixel_hit = 0U;
    vlSelfRef.vgc__DOT__unnamedblk8__DOT__s = 0U;
    while (VL_GTS_III(32, 0x00000010U, vlSelfRef.vgc__DOT__unnamedblk8__DOT__s)) {
        if (vlSelfRef.vgc__DOT__spr_vis[(0x0000000fU 
                                         & vlSelfRef.vgc__DOT__unnamedblk8__DOT__s)]) {
            vlSelfRef.vgc__DOT__spr_pixel = vlSelfRef.vgc__DOT__spr_pix
                [(0x0000000fU & vlSelfRef.vgc__DOT__unnamedblk8__DOT__s)];
            vlSelfRef.vgc__DOT__spr_pixel_hit = 1U;
            vlSelfRef.vgc__DOT__spr_pixel_pri = vlSelfRef.vgc__DOT__spr_pri
                [(0x0000000fU & vlSelfRef.vgc__DOT__unnamedblk8__DOT__s)];
        }
        vlSelfRef.vgc__DOT__unnamedblk8__DOT__s = ((IData)(1U) 
                                                   + vlSelfRef.vgc__DOT__unnamedblk8__DOT__s);
    }
    if (vlSelfRef.vgc__DOT__visible) {
        if (vlSelfRef.vgc__DOT__in_text_area) {
            vlSelfRef.vgc__DOT__pixel_color = ((4U 
                                                & (IData)(vlSelfRef.vgc__DOT__mode))
                                                ? (IData)(vlSelfRef.vgc__DOT__text_pixel)
                                                : (
                                                   (2U 
                                                    & (IData)(vlSelfRef.vgc__DOT__mode))
                                                    ? 
                                                   ((1U 
                                                     & (IData)(vlSelfRef.vgc__DOT__mode))
                                                     ? 
                                                    ((0U 
                                                      != (IData)(vlSelfRef.vgc__DOT__cur_gfx))
                                                      ? (IData)(vlSelfRef.vgc__DOT__gfx_pixel)
                                                      : vlSelfRef.vgc__DOT__palette
                                                     [vlSelfRef.vgc__DOT__bg_color])
                                                     : 
                                                    ((IData)(vlSelfRef.vgc__DOT__pixel_on)
                                                      ? vlSelfRef.vgc__DOT__palette
                                                     [vlSelfRef.vgc__DOT__cur_fg]
                                                      : 
                                                     ((0U 
                                                       != (IData)(vlSelfRef.vgc__DOT__cur_gfx))
                                                       ? (IData)(vlSelfRef.vgc__DOT__gfx_pixel)
                                                       : vlSelfRef.vgc__DOT__palette
                                                      [vlSelfRef.vgc__DOT__bg_color])))
                                                    : 
                                                   ((1U 
                                                     & (IData)(vlSelfRef.vgc__DOT__mode))
                                                     ? 
                                                    ((0U 
                                                      != (IData)(vlSelfRef.vgc__DOT__cur_gfx))
                                                      ? (IData)(vlSelfRef.vgc__DOT__gfx_pixel)
                                                      : (IData)(vlSelfRef.vgc__DOT__text_pixel))
                                                     : (IData)(vlSelfRef.vgc__DOT__text_pixel))));
            if (vlSelfRef.vgc__DOT__spr_pixel_hit) {
                if ((0U == (IData)(vlSelfRef.vgc__DOT__spr_pixel_pri))) {
                    if (((~ (IData)(vlSelfRef.vgc__DOT__pixel_on)) 
                         & (0U == (IData)(vlSelfRef.vgc__DOT__cur_gfx)))) {
                        vlSelfRef.vgc__DOT__pixel_color 
                            = vlSelfRef.vgc__DOT__palette
                            [vlSelfRef.vgc__DOT__spr_pixel];
                    }
                } else {
                    vlSelfRef.vgc__DOT__pixel_color 
                        = vlSelfRef.vgc__DOT__palette
                        [vlSelfRef.vgc__DOT__spr_pixel];
                }
            }
        } else {
            vlSelfRef.vgc__DOT__pixel_color = vlSelfRef.vgc__DOT__palette
                [vlSelfRef.vgc__DOT__border_color];
        }
    } else {
        vlSelfRef.vgc__DOT__pixel_color = 0U;
    }
    vlSelfRef.cpu_rdata = vlSelfRef.vgc__DOT__cpu_rdata;
}

VL_ATTR_COLD void Vvgc___024root___eval_stl(Vvgc___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vvgc___024root___eval_stl\n"); );
    Vvgc__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
    // Body
    if ((1ULL & vlSelfRef.__VstlTriggered[0U])) {
        Vvgc___024root___stl_sequent__TOP__0(vlSelf);
    }
}

VL_ATTR_COLD bool Vvgc___024root___eval_phase__stl(Vvgc___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vvgc___024root___eval_phase__stl\n"); );
    Vvgc__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
    // Locals
    CData/*0:0*/ __VstlExecute;
    // Body
    Vvgc___024root___eval_triggers_vec__stl(vlSelf);
#ifdef VL_DEBUG
    if (VL_UNLIKELY(vlSymsp->_vm_contextp__->debug())) {
        Vvgc___024root___dump_triggers__stl(vlSelfRef.__VstlTriggered, "stl"s);
    }
#endif
    __VstlExecute = Vvgc___024root___trigger_anySet__stl(vlSelfRef.__VstlTriggered);
    if (__VstlExecute) {
        Vvgc___024root___eval_stl(vlSelf);
    }
    return (__VstlExecute);
}

bool Vvgc___024root___trigger_anySet__ico(const VlUnpacked<QData/*63:0*/, 1> &in);

#ifdef VL_DEBUG
VL_ATTR_COLD void Vvgc___024root___dump_triggers__ico(const VlUnpacked<QData/*63:0*/, 1> &triggers, const std::string &tag) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vvgc___024root___dump_triggers__ico\n"); );
    // Body
    if ((1U & (~ (IData)(Vvgc___024root___trigger_anySet__ico(triggers))))) {
        VL_DBG_MSGS("         No '" + tag + "' region triggers active\n");
    }
    if ((1U & (IData)(triggers[0U]))) {
        VL_DBG_MSGS("         '" + tag + "' region trigger index 0 is active: Internal 'ico' trigger - first iteration\n");
    }
}
#endif  // VL_DEBUG

bool Vvgc___024root___trigger_anySet__act(const VlUnpacked<QData/*63:0*/, 1> &in);

#ifdef VL_DEBUG
VL_ATTR_COLD void Vvgc___024root___dump_triggers__act(const VlUnpacked<QData/*63:0*/, 1> &triggers, const std::string &tag) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vvgc___024root___dump_triggers__act\n"); );
    // Body
    if ((1U & (~ (IData)(Vvgc___024root___trigger_anySet__act(triggers))))) {
        VL_DBG_MSGS("         No '" + tag + "' region triggers active\n");
    }
    if ((1U & (IData)(triggers[0U]))) {
        VL_DBG_MSGS("         '" + tag + "' region trigger index 0 is active: @(posedge vgc.clk)\n");
    }
}
#endif  // VL_DEBUG

VL_ATTR_COLD void Vvgc___024root___ctor_var_reset(Vvgc___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vvgc___024root___ctor_var_reset\n"); );
    Vvgc__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
    // Body
    const uint64_t __VscopeHash = VL_MURMUR64_HASH(vlSelf->vlNamep);
    vlSelf->clk = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 16707436170211756652ull);
    vlSelf->rst = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 18209466448985614591ull);
    vlSelf->cpu_ce = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 12668535522681808478ull);
    vlSelf->cpu_addr = VL_SCOPED_RAND_RESET_I(16, __VscopeHash, 9730387426955054438ull);
    vlSelf->cpu_wdata = VL_SCOPED_RAND_RESET_I(8, __VscopeHash, 16764354507008586316ull);
    vlSelf->cpu_rdata = VL_SCOPED_RAND_RESET_I(8, __VscopeHash, 2874117005747311920ull);
    vlSelf->cpu_we = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 2926808991039888122ull);
    vlSelf->cpu_re = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 12477858866832293430ull);
    vlSelf->key_valid = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 11641391107834527882ull);
    vlSelf->key_data = VL_SCOPED_RAND_RESET_I(8, __VscopeHash, 16169336856313411461ull);
    vlSelf->vid_r = VL_SCOPED_RAND_RESET_I(4, __VscopeHash, 4640268748951543978ull);
    vlSelf->vid_g = VL_SCOPED_RAND_RESET_I(4, __VscopeHash, 16072396866014922634ull);
    vlSelf->vid_b = VL_SCOPED_RAND_RESET_I(4, __VscopeHash, 14561862645762128483ull);
    vlSelf->vid_hsync = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 9848722673327594505ull);
    vlSelf->vid_vsync = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 5714281414838969621ull);
    vlSelf->vid_de = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 8410564023838534647ull);
    vlSelf->vgc__DOT____Vlvbound_hced7a248__0 = 0;
    vlSelf->vgc__DOT____Vlvbound_h87c37a99__0 = 0;
    vlSelf->vgc__DOT____Vlvbound_hde0d773d__1 = 0;
    vlSelf->vgc__DOT____Vlvbound_hde0d773d__0 = 0;
    vlSelf->vgc__DOT____Vlvbound_h5d6d955d__0 = 0;
    vlSelf->vgc__DOT____Vlvbound_h2b480197__0 = 0;
    vlSelf->vgc__DOT____Vlvbound_hb8130fd5__0 = 0;
    vlSelf->vgc__DOT____Vlvbound_h3f27e46b__2 = 0;
    vlSelf->vgc__DOT____Vlvbound_h3f27e46b__1 = 0;
    vlSelf->vgc__DOT____Vlvbound_h3f27e46b__0 = 0;
    vlSelf->vgc__DOT____Vlvbound_hc0a966b6__0 = 0;
    vlSelf->vgc__DOT____Vlvbound_hdecc4094__0 = 0;
    vlSelf->vgc__DOT____Vlvbound_h902d5c6a__0 = 0;
    vlSelf->vgc__DOT____Vlvbound_h652b6de2__0 = 0;
    vlSelf->vgc__DOT____Vlvbound_h45ddebd4__0 = 0;
    vlSelf->vgc__DOT____Vlvbound_h3318332a__0 = 0;
    vlSelf->vgc__DOT__clk = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 10090518602818742210ull);
    vlSelf->vgc__DOT__rst = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 10211599278985267433ull);
    vlSelf->vgc__DOT__cpu_ce = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 14817060224908635890ull);
    vlSelf->vgc__DOT__cpu_addr = VL_SCOPED_RAND_RESET_I(16, __VscopeHash, 17101992251710240108ull);
    vlSelf->vgc__DOT__cpu_wdata = VL_SCOPED_RAND_RESET_I(8, __VscopeHash, 18140316030620658770ull);
    vlSelf->vgc__DOT__cpu_rdata = VL_SCOPED_RAND_RESET_I(8, __VscopeHash, 8330420421657900410ull);
    vlSelf->vgc__DOT__cpu_we = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 4129368799617200409ull);
    vlSelf->vgc__DOT__cpu_re = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 12053829281909807083ull);
    vlSelf->vgc__DOT__key_valid = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 12251334128857872953ull);
    vlSelf->vgc__DOT__key_data = VL_SCOPED_RAND_RESET_I(8, __VscopeHash, 17176833305784203581ull);
    vlSelf->vgc__DOT__vid_r = VL_SCOPED_RAND_RESET_I(4, __VscopeHash, 16484987495764889874ull);
    vlSelf->vgc__DOT__vid_g = VL_SCOPED_RAND_RESET_I(4, __VscopeHash, 13118867259501354830ull);
    vlSelf->vgc__DOT__vid_b = VL_SCOPED_RAND_RESET_I(4, __VscopeHash, 16285177215922228126ull);
    vlSelf->vgc__DOT__vid_hsync = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 14502834937264359324ull);
    vlSelf->vgc__DOT__vid_vsync = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 15864683223933504160ull);
    vlSelf->vgc__DOT__vid_de = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 3153374541572603372ull);
    vlSelf->vgc__DOT__h_count = VL_SCOPED_RAND_RESET_I(10, __VscopeHash, 2867683474465299920ull);
    vlSelf->vgc__DOT__v_count = VL_SCOPED_RAND_RESET_I(10, __VscopeHash, 10060028233125698049ull);
    vlSelf->vgc__DOT__h_sync_area = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 8117025023718730651ull);
    vlSelf->vgc__DOT__v_sync_area = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 8394469434248815292ull);
    vlSelf->vgc__DOT__h_visible = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 3118072649405160392ull);
    vlSelf->vgc__DOT__v_visible = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 6360485261817708481ull);
    vlSelf->vgc__DOT__visible = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 4057408906588900179ull);
    vlSelf->vgc__DOT__in_text_area = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 14242559828576331749ull);
    for (int __Vi0 = 0; __Vi0 < 16; ++__Vi0) {
        vlSelf->vgc__DOT__palette[__Vi0] = VL_SCOPED_RAND_RESET_I(12, __VscopeHash, 2777874229078052061ull);
    }
    for (int __Vi0 = 0; __Vi0 < 2000; ++__Vi0) {
        vlSelf->vgc__DOT__char_ram[__Vi0] = VL_SCOPED_RAND_RESET_I(8, __VscopeHash, 2694106723341813762ull);
    }
    for (int __Vi0 = 0; __Vi0 < 2000; ++__Vi0) {
        vlSelf->vgc__DOT__color_ram[__Vi0] = VL_SCOPED_RAND_RESET_I(8, __VscopeHash, 5867381623769530277ull);
    }
    for (int __Vi0 = 0; __Vi0 < 2048; ++__Vi0) {
        vlSelf->vgc__DOT__font_rom[__Vi0] = VL_SCOPED_RAND_RESET_I(8, __VscopeHash, 506621594893427862ull);
    }
    for (int __Vi0 = 0; __Vi0 < 64000; ++__Vi0) {
        vlSelf->vgc__DOT__gfx_ram[__Vi0] = VL_SCOPED_RAND_RESET_I(4, __VscopeHash, 12922770959478074667ull);
    }
    for (int __Vi0 = 0; __Vi0 < 2048; ++__Vi0) {
        vlSelf->vgc__DOT__sprite_shapes[__Vi0] = VL_SCOPED_RAND_RESET_I(8, __VscopeHash, 472852298865506632ull);
    }
    for (int __Vi0 = 0; __Vi0 < 16; ++__Vi0) {
        vlSelf->vgc__DOT__spr_x[__Vi0] = VL_SCOPED_RAND_RESET_I(9, __VscopeHash, 9616544967727383974ull);
    }
    for (int __Vi0 = 0; __Vi0 < 16; ++__Vi0) {
        vlSelf->vgc__DOT__spr_y[__Vi0] = VL_SCOPED_RAND_RESET_I(8, __VscopeHash, 16872385944747861881ull);
    }
    for (int __Vi0 = 0; __Vi0 < 16; ++__Vi0) {
        vlSelf->vgc__DOT__spr_enable[__Vi0] = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 2602094500950388763ull);
    }
    for (int __Vi0 = 0; __Vi0 < 16; ++__Vi0) {
        vlSelf->vgc__DOT__spr_flip_h[__Vi0] = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 7064936882944806097ull);
    }
    for (int __Vi0 = 0; __Vi0 < 16; ++__Vi0) {
        vlSelf->vgc__DOT__spr_flip_v[__Vi0] = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 5853627348694989748ull);
    }
    for (int __Vi0 = 0; __Vi0 < 16; ++__Vi0) {
        vlSelf->vgc__DOT__spr_pri[__Vi0] = VL_SCOPED_RAND_RESET_I(2, __VscopeHash, 13881101146147602378ull);
    }
    for (int __Vi0 = 0; __Vi0 < 16; ++__Vi0) {
        vlSelf->vgc__DOT__spr_shape[__Vi0] = VL_SCOPED_RAND_RESET_I(4, __VscopeHash, 5105126160797594514ull);
    }
    for (int __Vi0 = 0; __Vi0 < 32; ++__Vi0) {
        vlSelf->vgc__DOT__regs[__Vi0] = VL_SCOPED_RAND_RESET_I(8, __VscopeHash, 801998965288416300ull);
    }
    vlSelf->vgc__DOT__cursor_x = VL_SCOPED_RAND_RESET_I(7, __VscopeHash, 17839827787087428148ull);
    vlSelf->vgc__DOT__cursor_y = VL_SCOPED_RAND_RESET_I(5, __VscopeHash, 10125365203016716658ull);
    vlSelf->vgc__DOT__fg_color = VL_SCOPED_RAND_RESET_I(4, __VscopeHash, 1520157552339846969ull);
    vlSelf->vgc__DOT__bg_color = VL_SCOPED_RAND_RESET_I(4, __VscopeHash, 10004188932690309209ull);
    vlSelf->vgc__DOT__border_color = VL_SCOPED_RAND_RESET_I(4, __VscopeHash, 12407745325368003372ull);
    vlSelf->vgc__DOT__gfx_color = VL_SCOPED_RAND_RESET_I(4, __VscopeHash, 7048859210729033512ull);
    vlSelf->vgc__DOT__mode = VL_SCOPED_RAND_RESET_I(3, __VscopeHash, 8820458676242493964ull);
    vlSelf->vgc__DOT__char_in_reg = VL_SCOPED_RAND_RESET_I(8, __VscopeHash, 9497262952214620041ull);
    vlSelf->vgc__DOT__scroll_offset = VL_SCOPED_RAND_RESET_I(5, __VscopeHash, 12886233069044022628ull);
    vlSelf->vgc__DOT__scroll_pending = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 18391647169579164800ull);
    vlSelf->vgc__DOT__scroll_col = VL_SCOPED_RAND_RESET_I(7, __VscopeHash, 7277222803238926945ull);
    for (int __Vi0 = 0; __Vi0 < 256; ++__Vi0) {
        vlSelf->vgc__DOT__copper_pos[__Vi0] = VL_SCOPED_RAND_RESET_I(17, __VscopeHash, 2844854802714649858ull);
    }
    for (int __Vi0 = 0; __Vi0 < 256; ++__Vi0) {
        vlSelf->vgc__DOT__copper_reg[__Vi0] = VL_SCOPED_RAND_RESET_I(8, __VscopeHash, 11048117336516147887ull);
    }
    for (int __Vi0 = 0; __Vi0 < 256; ++__Vi0) {
        vlSelf->vgc__DOT__copper_val[__Vi0] = VL_SCOPED_RAND_RESET_I(8, __VscopeHash, 16910723685652932498ull);
    }
    vlSelf->vgc__DOT__copper_count = VL_SCOPED_RAND_RESET_I(9, __VscopeHash, 5702792575624153666ull);
    vlSelf->vgc__DOT__copper_index = VL_SCOPED_RAND_RESET_I(9, __VscopeHash, 7675000386937587047ull);
    vlSelf->vgc__DOT__copper_enabled = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 7798445408846670706ull);
    vlSelf->vgc__DOT__cmd_busy = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 243709688142814051ull);
    vlSelf->vgc__DOT__cmd_op = VL_SCOPED_RAND_RESET_I(8, __VscopeHash, 14503037207871652954ull);
    vlSelf->vgc__DOT__cmd_x = VL_SCOPED_RAND_RESET_I(10, __VscopeHash, 14405601901774574780ull);
    vlSelf->vgc__DOT__cmd_y = VL_SCOPED_RAND_RESET_I(10, __VscopeHash, 1501402728746891915ull);
    vlSelf->vgc__DOT__cmd_x2 = VL_SCOPED_RAND_RESET_I(10, __VscopeHash, 9042541721796312031ull);
    vlSelf->vgc__DOT__cmd_y2 = VL_SCOPED_RAND_RESET_I(10, __VscopeHash, 11182423648631335800ull);
    vlSelf->vgc__DOT__cmd_dx = VL_SCOPED_RAND_RESET_I(10, __VscopeHash, 17826145257909788251ull);
    vlSelf->vgc__DOT__cmd_dy = VL_SCOPED_RAND_RESET_I(10, __VscopeHash, 16406425318196617555ull);
    vlSelf->vgc__DOT__cmd_err = VL_SCOPED_RAND_RESET_I(10, __VscopeHash, 4243405597438594111ull);
    vlSelf->vgc__DOT__cmd_sx = VL_SCOPED_RAND_RESET_I(10, __VscopeHash, 2419890123709831733ull);
    vlSelf->vgc__DOT__cmd_sy = VL_SCOPED_RAND_RESET_I(10, __VscopeHash, 8658566108603244214ull);
    vlSelf->vgc__DOT__cmd_cx = VL_SCOPED_RAND_RESET_I(9, __VscopeHash, 2060627708393865929ull);
    vlSelf->vgc__DOT__cmd_cy = VL_SCOPED_RAND_RESET_I(8, __VscopeHash, 11722291267377280326ull);
    vlSelf->vgc__DOT__vgc_reg_sel = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 14831905401474699452ull);
    vlSelf->vgc__DOT__spr_reg_sel = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 17607227511188103426ull);
    vlSelf->vgc__DOT__char_ram_sel = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 13816585220643412286ull);
    vlSelf->vgc__DOT__color_ram_sel = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 15585288410817138825ull);
    vlSelf->vgc__DOT__reg_offset = VL_SCOPED_RAND_RESET_I(5, __VscopeHash, 9302232735573001014ull);
    vlSelf->vgc__DOT__spr_index = VL_SCOPED_RAND_RESET_I(4, __VscopeHash, 12943044321775120711ull);
    vlSelf->vgc__DOT__spr_field = VL_SCOPED_RAND_RESET_I(3, __VscopeHash, 11838168754585291016ull);
    vlSelf->vgc__DOT__char_offset = VL_SCOPED_RAND_RESET_I(11, __VscopeHash, 2615861214801984542ull);
    vlSelf->vgc__DOT__color_offset = VL_SCOPED_RAND_RESET_I(11, __VscopeHash, 10255301230982794818ull);
    vlSelf->vgc__DOT__text_col = VL_SCOPED_RAND_RESET_I(7, __VscopeHash, 11361460477586232537ull);
    vlSelf->vgc__DOT__text_row = VL_SCOPED_RAND_RESET_I(5, __VscopeHash, 6880267987234798501ull);
    vlSelf->vgc__DOT__real_row = VL_SCOPED_RAND_RESET_I(5, __VscopeHash, 18371904909305458693ull);
    vlSelf->vgc__DOT__font_line = VL_SCOPED_RAND_RESET_I(3, __VscopeHash, 17650988148072738218ull);
    vlSelf->vgc__DOT__font_pixel = VL_SCOPED_RAND_RESET_I(3, __VscopeHash, 16793209386038522174ull);
    vlSelf->vgc__DOT__text_line = VL_SCOPED_RAND_RESET_I(10, __VscopeHash, 1140578208556538280ull);
    vlSelf->vgc__DOT__gfx_x = VL_SCOPED_RAND_RESET_I(9, __VscopeHash, 1904414083473300387ull);
    vlSelf->vgc__DOT__gfx_y = VL_SCOPED_RAND_RESET_I(8, __VscopeHash, 8721660532356672767ull);
    vlSelf->vgc__DOT__cur_char = VL_SCOPED_RAND_RESET_I(8, __VscopeHash, 6196015948235624338ull);
    vlSelf->vgc__DOT__cur_fg = VL_SCOPED_RAND_RESET_I(4, __VscopeHash, 13800816550750900536ull);
    vlSelf->vgc__DOT__cur_gfx = VL_SCOPED_RAND_RESET_I(4, __VscopeHash, 13587654112543802216ull);
    vlSelf->vgc__DOT__font_byte = VL_SCOPED_RAND_RESET_I(8, __VscopeHash, 2054569410514670163ull);
    vlSelf->vgc__DOT__pixel_on = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 3487685202292797247ull);
    vlSelf->vgc__DOT__text_pixel = VL_SCOPED_RAND_RESET_I(12, __VscopeHash, 17715980410008106742ull);
    vlSelf->vgc__DOT__gfx_pixel = VL_SCOPED_RAND_RESET_I(12, __VscopeHash, 7371276794565542472ull);
    vlSelf->vgc__DOT__pixel_color = VL_SCOPED_RAND_RESET_I(12, __VscopeHash, 6889750886543501043ull);
    for (int __Vi0 = 0; __Vi0 < 16; ++__Vi0) {
        vlSelf->vgc__DOT__spr_dx[__Vi0] = VL_SCOPED_RAND_RESET_I(10, __VscopeHash, 2630595430977024382ull);
    }
    for (int __Vi0 = 0; __Vi0 < 16; ++__Vi0) {
        vlSelf->vgc__DOT__spr_dy[__Vi0] = VL_SCOPED_RAND_RESET_I(10, __VscopeHash, 450029139959140235ull);
    }
    for (int __Vi0 = 0; __Vi0 < 16; ++__Vi0) {
        vlSelf->vgc__DOT__spr_fx[__Vi0] = VL_SCOPED_RAND_RESET_I(4, __VscopeHash, 12019705424879728877ull);
    }
    for (int __Vi0 = 0; __Vi0 < 16; ++__Vi0) {
        vlSelf->vgc__DOT__spr_fy[__Vi0] = VL_SCOPED_RAND_RESET_I(4, __VscopeHash, 14967188851684136441ull);
    }
    for (int __Vi0 = 0; __Vi0 < 16; ++__Vi0) {
        vlSelf->vgc__DOT__spr_sa[__Vi0] = VL_SCOPED_RAND_RESET_I(11, __VscopeHash, 13328815345514886673ull);
    }
    for (int __Vi0 = 0; __Vi0 < 16; ++__Vi0) {
        vlSelf->vgc__DOT__spr_pix[__Vi0] = VL_SCOPED_RAND_RESET_I(4, __VscopeHash, 12605717045727913747ull);
    }
    for (int __Vi0 = 0; __Vi0 < 16; ++__Vi0) {
        vlSelf->vgc__DOT__spr_vis[__Vi0] = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 1760560372853308881ull);
    }
    vlSelf->vgc__DOT__spr_pixel = VL_SCOPED_RAND_RESET_I(4, __VscopeHash, 12956011104171493158ull);
    vlSelf->vgc__DOT__spr_pixel_pri = VL_SCOPED_RAND_RESET_I(2, __VscopeHash, 1274504476821008435ull);
    vlSelf->vgc__DOT__spr_pixel_hit = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 17389215234764933755ull);
    vlSelf->vgc__DOT__blink_count = VL_SCOPED_RAND_RESET_I(5, __VscopeHash, 12703097751573998099ull);
    vlSelf->vgc__DOT__cursor_blink = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 11212455903827637808ull);
    vlSelf->vgc__DOT__cursor_here = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 186906717401947128ull);
    vlSelf->vgc__DOT__beam_pos = VL_SCOPED_RAND_RESET_I(17, __VscopeHash, 8000466905727951727ull);
    vlSelf->vgc__DOT__unnamedblk1__DOT__i = 0;
    vlSelf->vgc__DOT__unnamedblk2__DOT__i = 0;
    vlSelf->vgc__DOT__unnamedblk3__DOT__i = 0;
    vlSelf->vgc__DOT__unnamedblk4__DOT__i = 0;
    vlSelf->vgc__DOT__unnamedblk5__DOT__i = 0;
    vlSelf->vgc__DOT__unnamedblk6__DOT__i = 0;
    vlSelf->vgc__DOT__unnamedblk7__DOT__s = 0;
    vlSelf->vgc__DOT__unnamedblk8__DOT__s = 0;
    for (int __Vi0 = 0; __Vi0 < 1; ++__Vi0) {
        vlSelf->__VstlTriggered[__Vi0] = 0;
    }
    for (int __Vi0 = 0; __Vi0 < 1; ++__Vi0) {
        vlSelf->__VicoTriggered[__Vi0] = 0;
    }
    for (int __Vi0 = 0; __Vi0 < 1; ++__Vi0) {
        vlSelf->__VactTriggered[__Vi0] = 0;
    }
    vlSelf->__Vtrigprevexpr___TOP__vgc__DOT__clk__0 = 0;
    for (int __Vi0 = 0; __Vi0 < 1; ++__Vi0) {
        vlSelf->__VnbaTriggered[__Vi0] = 0;
    }
}
