// Verilated -*- C++ -*-
// DESCRIPTION: Verilator output: Design implementation internals
// See Vvgc.h for the primary calling header

#include "Vvgc__pch.h"

void Vvgc___024root___eval_triggers_vec__ico(Vvgc___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vvgc___024root___eval_triggers_vec__ico\n"); );
    Vvgc__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
    // Body
    vlSelfRef.__VicoTriggered[0U] = ((0xfffffffffffffffeULL 
                                      & vlSelfRef.__VicoTriggered[0U]) 
                                     | (IData)((IData)(vlSelfRef.__VicoFirstIteration)));
}

bool Vvgc___024root___trigger_anySet__ico(const VlUnpacked<QData/*63:0*/, 1> &in) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vvgc___024root___trigger_anySet__ico\n"); );
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

void Vvgc___024root___ico_sequent__TOP__0(Vvgc___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vvgc___024root___ico_sequent__TOP__0\n"); );
    Vvgc__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
    // Body
    vlSelfRef.vgc__DOT__clk = vlSelfRef.clk;
    vlSelfRef.vgc__DOT__h_sync_area = ((0x0290U <= (IData)(vlSelfRef.vgc__DOT__h_count)) 
                                       & (0x02f0U > (IData)(vlSelfRef.vgc__DOT__h_count)));
    vlSelfRef.vgc__DOT__v_sync_area = ((0x01eaU <= (IData)(vlSelfRef.vgc__DOT__v_count)) 
                                       & (0x01ecU > (IData)(vlSelfRef.vgc__DOT__v_count)));
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

void Vvgc___024root___eval_ico(Vvgc___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vvgc___024root___eval_ico\n"); );
    Vvgc__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
    // Body
    if ((1ULL & vlSelfRef.__VicoTriggered[0U])) {
        Vvgc___024root___ico_sequent__TOP__0(vlSelf);
    }
}

#ifdef VL_DEBUG
VL_ATTR_COLD void Vvgc___024root___dump_triggers__ico(const VlUnpacked<QData/*63:0*/, 1> &triggers, const std::string &tag);
#endif  // VL_DEBUG

bool Vvgc___024root___eval_phase__ico(Vvgc___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vvgc___024root___eval_phase__ico\n"); );
    Vvgc__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
    // Locals
    CData/*0:0*/ __VicoExecute;
    // Body
    Vvgc___024root___eval_triggers_vec__ico(vlSelf);
#ifdef VL_DEBUG
    if (VL_UNLIKELY(vlSymsp->_vm_contextp__->debug())) {
        Vvgc___024root___dump_triggers__ico(vlSelfRef.__VicoTriggered, "ico"s);
    }
#endif
    __VicoExecute = Vvgc___024root___trigger_anySet__ico(vlSelfRef.__VicoTriggered);
    if (__VicoExecute) {
        Vvgc___024root___eval_ico(vlSelf);
    }
    return (__VicoExecute);
}

void Vvgc___024root___eval_triggers_vec__act(Vvgc___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vvgc___024root___eval_triggers_vec__act\n"); );
    Vvgc__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
    // Body
    vlSelfRef.__VactTriggered[0U] = (QData)((IData)(
                                                    ((IData)(vlSelfRef.vgc__DOT__clk) 
                                                     & (~ (IData)(vlSelfRef.__Vtrigprevexpr___TOP__vgc__DOT__clk__0)))));
    vlSelfRef.__Vtrigprevexpr___TOP__vgc__DOT__clk__0 
        = vlSelfRef.vgc__DOT__clk;
}

bool Vvgc___024root___trigger_anySet__act(const VlUnpacked<QData/*63:0*/, 1> &in) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vvgc___024root___trigger_anySet__act\n"); );
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

void Vvgc___024root___nba_sequent__TOP__0(Vvgc___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vvgc___024root___nba_sequent__TOP__0\n"); );
    Vvgc__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
    // Locals
    SData/*10:0*/ __Vfunc_vgc__DOT__screen_addr__0__Vfuncout;
    __Vfunc_vgc__DOT__screen_addr__0__Vfuncout = 0;
    CData/*6:0*/ __Vfunc_vgc__DOT__screen_addr__0__col;
    __Vfunc_vgc__DOT__screen_addr__0__col = 0;
    CData/*4:0*/ __Vfunc_vgc__DOT__screen_addr__0__rr;
    __Vfunc_vgc__DOT__screen_addr__0__rr = 0;
    SData/*10:0*/ __Vfunc_vgc__DOT__screen_addr__1__Vfuncout;
    __Vfunc_vgc__DOT__screen_addr__1__Vfuncout = 0;
    CData/*6:0*/ __Vfunc_vgc__DOT__screen_addr__1__col;
    __Vfunc_vgc__DOT__screen_addr__1__col = 0;
    CData/*4:0*/ __Vfunc_vgc__DOT__screen_addr__1__rr;
    __Vfunc_vgc__DOT__screen_addr__1__rr = 0;
    SData/*10:0*/ __Vfunc_vgc__DOT__screen_addr__2__Vfuncout;
    __Vfunc_vgc__DOT__screen_addr__2__Vfuncout = 0;
    CData/*6:0*/ __Vfunc_vgc__DOT__screen_addr__2__col;
    __Vfunc_vgc__DOT__screen_addr__2__col = 0;
    CData/*4:0*/ __Vfunc_vgc__DOT__screen_addr__2__rr;
    __Vfunc_vgc__DOT__screen_addr__2__rr = 0;
    SData/*10:0*/ __Vfunc_vgc__DOT__screen_addr__3__Vfuncout;
    __Vfunc_vgc__DOT__screen_addr__3__Vfuncout = 0;
    CData/*6:0*/ __Vfunc_vgc__DOT__screen_addr__3__col;
    __Vfunc_vgc__DOT__screen_addr__3__col = 0;
    CData/*4:0*/ __Vfunc_vgc__DOT__screen_addr__3__rr;
    __Vfunc_vgc__DOT__screen_addr__3__rr = 0;
    SData/*10:0*/ __Vfunc_vgc__DOT__screen_addr__4__Vfuncout;
    __Vfunc_vgc__DOT__screen_addr__4__Vfuncout = 0;
    CData/*6:0*/ __Vfunc_vgc__DOT__screen_addr__4__col;
    __Vfunc_vgc__DOT__screen_addr__4__col = 0;
    CData/*4:0*/ __Vfunc_vgc__DOT__screen_addr__4__row;
    __Vfunc_vgc__DOT__screen_addr__4__row = 0;
    CData/*4:0*/ __Vfunc_vgc__DOT__screen_addr__4__rr;
    __Vfunc_vgc__DOT__screen_addr__4__rr = 0;
    SData/*10:0*/ __Vfunc_vgc__DOT__screen_addr__5__Vfuncout;
    __Vfunc_vgc__DOT__screen_addr__5__Vfuncout = 0;
    CData/*6:0*/ __Vfunc_vgc__DOT__screen_addr__5__col;
    __Vfunc_vgc__DOT__screen_addr__5__col = 0;
    CData/*4:0*/ __Vfunc_vgc__DOT__screen_addr__5__row;
    __Vfunc_vgc__DOT__screen_addr__5__row = 0;
    CData/*4:0*/ __Vfunc_vgc__DOT__screen_addr__5__rr;
    __Vfunc_vgc__DOT__screen_addr__5__rr = 0;
    SData/*10:0*/ __Vfunc_vgc__DOT__screen_addr__6__Vfuncout;
    __Vfunc_vgc__DOT__screen_addr__6__Vfuncout = 0;
    CData/*6:0*/ __Vfunc_vgc__DOT__screen_addr__6__col;
    __Vfunc_vgc__DOT__screen_addr__6__col = 0;
    CData/*4:0*/ __Vfunc_vgc__DOT__screen_addr__6__row;
    __Vfunc_vgc__DOT__screen_addr__6__row = 0;
    CData/*4:0*/ __Vfunc_vgc__DOT__screen_addr__6__rr;
    __Vfunc_vgc__DOT__screen_addr__6__rr = 0;
    SData/*10:0*/ __Vfunc_vgc__DOT__screen_addr__7__Vfuncout;
    __Vfunc_vgc__DOT__screen_addr__7__Vfuncout = 0;
    CData/*6:0*/ __Vfunc_vgc__DOT__screen_addr__7__col;
    __Vfunc_vgc__DOT__screen_addr__7__col = 0;
    CData/*4:0*/ __Vfunc_vgc__DOT__screen_addr__7__row;
    __Vfunc_vgc__DOT__screen_addr__7__row = 0;
    CData/*4:0*/ __Vfunc_vgc__DOT__screen_addr__7__rr;
    __Vfunc_vgc__DOT__screen_addr__7__rr = 0;
    SData/*10:0*/ __Vfunc_vgc__DOT__screen_addr__8__Vfuncout;
    __Vfunc_vgc__DOT__screen_addr__8__Vfuncout = 0;
    CData/*6:0*/ __Vfunc_vgc__DOT__screen_addr__8__col;
    __Vfunc_vgc__DOT__screen_addr__8__col = 0;
    CData/*4:0*/ __Vfunc_vgc__DOT__screen_addr__8__row;
    __Vfunc_vgc__DOT__screen_addr__8__row = 0;
    CData/*4:0*/ __Vfunc_vgc__DOT__screen_addr__8__rr;
    __Vfunc_vgc__DOT__screen_addr__8__rr = 0;
    SData/*10:0*/ __Vfunc_vgc__DOT__screen_addr__9__Vfuncout;
    __Vfunc_vgc__DOT__screen_addr__9__Vfuncout = 0;
    CData/*6:0*/ __Vfunc_vgc__DOT__screen_addr__9__col;
    __Vfunc_vgc__DOT__screen_addr__9__col = 0;
    CData/*4:0*/ __Vfunc_vgc__DOT__screen_addr__9__row;
    __Vfunc_vgc__DOT__screen_addr__9__row = 0;
    CData/*4:0*/ __Vfunc_vgc__DOT__screen_addr__9__rr;
    __Vfunc_vgc__DOT__screen_addr__9__rr = 0;
    SData/*9:0*/ __Vdly__vgc__DOT__h_count;
    __Vdly__vgc__DOT__h_count = 0;
    SData/*9:0*/ __Vdly__vgc__DOT__v_count;
    __Vdly__vgc__DOT__v_count = 0;
    CData/*7:0*/ __Vdly__vgc__DOT__char_in_reg;
    __Vdly__vgc__DOT__char_in_reg = 0;
    CData/*6:0*/ __Vdly__vgc__DOT__cursor_x;
    __Vdly__vgc__DOT__cursor_x = 0;
    CData/*4:0*/ __Vdly__vgc__DOT__cursor_y;
    __Vdly__vgc__DOT__cursor_y = 0;
    CData/*3:0*/ __Vdly__vgc__DOT__fg_color;
    __Vdly__vgc__DOT__fg_color = 0;
    CData/*3:0*/ __Vdly__vgc__DOT__gfx_color;
    __Vdly__vgc__DOT__gfx_color = 0;
    CData/*4:0*/ __Vdly__vgc__DOT__scroll_offset;
    __Vdly__vgc__DOT__scroll_offset = 0;
    CData/*0:0*/ __Vdly__vgc__DOT__scroll_pending;
    __Vdly__vgc__DOT__scroll_pending = 0;
    CData/*6:0*/ __Vdly__vgc__DOT__scroll_col;
    __Vdly__vgc__DOT__scroll_col = 0;
    CData/*0:0*/ __Vdly__vgc__DOT__cmd_busy;
    __Vdly__vgc__DOT__cmd_busy = 0;
    SData/*9:0*/ __Vdly__vgc__DOT__cmd_err;
    __Vdly__vgc__DOT__cmd_err = 0;
    SData/*9:0*/ __Vdly__vgc__DOT__cmd_x;
    __Vdly__vgc__DOT__cmd_x = 0;
    SData/*9:0*/ __Vdly__vgc__DOT__cmd_y;
    __Vdly__vgc__DOT__cmd_y = 0;
    SData/*8:0*/ __Vdly__vgc__DOT__cmd_cx;
    __Vdly__vgc__DOT__cmd_cx = 0;
    CData/*7:0*/ __Vdly__vgc__DOT__cmd_cy;
    __Vdly__vgc__DOT__cmd_cy = 0;
    SData/*8:0*/ __Vdly__vgc__DOT__copper_count;
    __Vdly__vgc__DOT__copper_count = 0;
    SData/*8:0*/ __Vdly__vgc__DOT__copper_index;
    __Vdly__vgc__DOT__copper_index = 0;
    CData/*0:0*/ __Vdly__vgc__DOT__cursor_blink;
    __Vdly__vgc__DOT__cursor_blink = 0;
    CData/*4:0*/ __Vdly__vgc__DOT__blink_count;
    __Vdly__vgc__DOT__blink_count = 0;
    CData/*7:0*/ __VdlyVal__vgc__DOT__char_ram__v0;
    __VdlyVal__vgc__DOT__char_ram__v0 = 0;
    SData/*10:0*/ __VdlyDim0__vgc__DOT__char_ram__v0;
    __VdlyDim0__vgc__DOT__char_ram__v0 = 0;
    CData/*0:0*/ __VdlySet__vgc__DOT__char_ram__v0;
    __VdlySet__vgc__DOT__char_ram__v0 = 0;
    CData/*7:0*/ __VdlyVal__vgc__DOT__color_ram__v0;
    __VdlyVal__vgc__DOT__color_ram__v0 = 0;
    SData/*10:0*/ __VdlyDim0__vgc__DOT__color_ram__v0;
    __VdlyDim0__vgc__DOT__color_ram__v0 = 0;
    CData/*0:0*/ __VdlySet__vgc__DOT__color_ram__v0;
    __VdlySet__vgc__DOT__color_ram__v0 = 0;
    CData/*3:0*/ __VdlyVal__vgc__DOT__gfx_ram__v0;
    __VdlyVal__vgc__DOT__gfx_ram__v0 = 0;
    SData/*15:0*/ __VdlyDim0__vgc__DOT__gfx_ram__v0;
    __VdlyDim0__vgc__DOT__gfx_ram__v0 = 0;
    CData/*0:0*/ __VdlySet__vgc__DOT__gfx_ram__v0;
    __VdlySet__vgc__DOT__gfx_ram__v0 = 0;
    CData/*3:0*/ __VdlyVal__vgc__DOT__gfx_ram__v1;
    __VdlyVal__vgc__DOT__gfx_ram__v1 = 0;
    SData/*15:0*/ __VdlyDim0__vgc__DOT__gfx_ram__v1;
    __VdlyDim0__vgc__DOT__gfx_ram__v1 = 0;
    CData/*0:0*/ __VdlySet__vgc__DOT__gfx_ram__v1;
    __VdlySet__vgc__DOT__gfx_ram__v1 = 0;
    CData/*3:0*/ __VdlyVal__vgc__DOT__gfx_ram__v2;
    __VdlyVal__vgc__DOT__gfx_ram__v2 = 0;
    SData/*15:0*/ __VdlyDim0__vgc__DOT__gfx_ram__v2;
    __VdlyDim0__vgc__DOT__gfx_ram__v2 = 0;
    CData/*0:0*/ __VdlySet__vgc__DOT__gfx_ram__v2;
    __VdlySet__vgc__DOT__gfx_ram__v2 = 0;
    CData/*3:0*/ __VdlyVal__vgc__DOT__gfx_ram__v3;
    __VdlyVal__vgc__DOT__gfx_ram__v3 = 0;
    SData/*15:0*/ __VdlyDim0__vgc__DOT__gfx_ram__v3;
    __VdlyDim0__vgc__DOT__gfx_ram__v3 = 0;
    CData/*0:0*/ __VdlySet__vgc__DOT__gfx_ram__v3;
    __VdlySet__vgc__DOT__gfx_ram__v3 = 0;
    SData/*10:0*/ __VdlyDim0__vgc__DOT__sprite_shapes__v0;
    __VdlyDim0__vgc__DOT__sprite_shapes__v0 = 0;
    CData/*0:0*/ __VdlySet__vgc__DOT__sprite_shapes__v0;
    __VdlySet__vgc__DOT__sprite_shapes__v0 = 0;
    CData/*7:0*/ __VdlyVal__vgc__DOT__char_ram__v1;
    __VdlyVal__vgc__DOT__char_ram__v1 = 0;
    SData/*10:0*/ __VdlyDim0__vgc__DOT__char_ram__v1;
    __VdlyDim0__vgc__DOT__char_ram__v1 = 0;
    CData/*0:0*/ __VdlySet__vgc__DOT__char_ram__v1;
    __VdlySet__vgc__DOT__char_ram__v1 = 0;
    CData/*7:0*/ __VdlyVal__vgc__DOT__char_ram__v2;
    __VdlyVal__vgc__DOT__char_ram__v2 = 0;
    SData/*10:0*/ __VdlyDim0__vgc__DOT__char_ram__v2;
    __VdlyDim0__vgc__DOT__char_ram__v2 = 0;
    CData/*0:0*/ __VdlySet__vgc__DOT__char_ram__v2;
    __VdlySet__vgc__DOT__char_ram__v2 = 0;
    CData/*7:0*/ __VdlyVal__vgc__DOT__color_ram__v1;
    __VdlyVal__vgc__DOT__color_ram__v1 = 0;
    SData/*10:0*/ __VdlyDim0__vgc__DOT__color_ram__v1;
    __VdlyDim0__vgc__DOT__color_ram__v1 = 0;
    CData/*0:0*/ __VdlySet__vgc__DOT__color_ram__v1;
    __VdlySet__vgc__DOT__color_ram__v1 = 0;
    IData/*16:0*/ __VdlyVal__vgc__DOT__copper_pos__v0;
    __VdlyVal__vgc__DOT__copper_pos__v0 = 0;
    CData/*7:0*/ __VdlyDim0__vgc__DOT__copper_pos__v0;
    __VdlyDim0__vgc__DOT__copper_pos__v0 = 0;
    CData/*0:0*/ __VdlySet__vgc__DOT__copper_pos__v0;
    __VdlySet__vgc__DOT__copper_pos__v0 = 0;
    CData/*7:0*/ __VdlyVal__vgc__DOT__copper_reg__v0;
    __VdlyVal__vgc__DOT__copper_reg__v0 = 0;
    CData/*7:0*/ __VdlyDim0__vgc__DOT__copper_reg__v0;
    __VdlyDim0__vgc__DOT__copper_reg__v0 = 0;
    CData/*7:0*/ __VdlyVal__vgc__DOT__copper_val__v0;
    __VdlyVal__vgc__DOT__copper_val__v0 = 0;
    CData/*7:0*/ __VdlyDim0__vgc__DOT__copper_val__v0;
    __VdlyDim0__vgc__DOT__copper_val__v0 = 0;
    CData/*1:0*/ __VdlyVal__vgc__DOT__spr_pri__v0;
    __VdlyVal__vgc__DOT__spr_pri__v0 = 0;
    CData/*3:0*/ __VdlyDim0__vgc__DOT__spr_pri__v0;
    __VdlyDim0__vgc__DOT__spr_pri__v0 = 0;
    CData/*0:0*/ __VdlySet__vgc__DOT__spr_pri__v0;
    __VdlySet__vgc__DOT__spr_pri__v0 = 0;
    CData/*0:0*/ __VdlyVal__vgc__DOT__spr_flip_h__v0;
    __VdlyVal__vgc__DOT__spr_flip_h__v0 = 0;
    CData/*3:0*/ __VdlyDim0__vgc__DOT__spr_flip_h__v0;
    __VdlyDim0__vgc__DOT__spr_flip_h__v0 = 0;
    CData/*0:0*/ __VdlySet__vgc__DOT__spr_flip_h__v0;
    __VdlySet__vgc__DOT__spr_flip_h__v0 = 0;
    CData/*0:0*/ __VdlyVal__vgc__DOT__spr_flip_v__v0;
    __VdlyVal__vgc__DOT__spr_flip_v__v0 = 0;
    CData/*3:0*/ __VdlyDim0__vgc__DOT__spr_flip_v__v0;
    __VdlyDim0__vgc__DOT__spr_flip_v__v0 = 0;
    CData/*3:0*/ __VdlyDim0__vgc__DOT__spr_enable__v0;
    __VdlyDim0__vgc__DOT__spr_enable__v0 = 0;
    CData/*0:0*/ __VdlySet__vgc__DOT__spr_enable__v0;
    __VdlySet__vgc__DOT__spr_enable__v0 = 0;
    CData/*3:0*/ __VdlyDim0__vgc__DOT__spr_enable__v1;
    __VdlyDim0__vgc__DOT__spr_enable__v1 = 0;
    CData/*0:0*/ __VdlySet__vgc__DOT__spr_enable__v1;
    __VdlySet__vgc__DOT__spr_enable__v1 = 0;
    SData/*8:0*/ __VdlyVal__vgc__DOT__spr_x__v0;
    __VdlyVal__vgc__DOT__spr_x__v0 = 0;
    CData/*3:0*/ __VdlyDim0__vgc__DOT__spr_x__v0;
    __VdlyDim0__vgc__DOT__spr_x__v0 = 0;
    CData/*0:0*/ __VdlySet__vgc__DOT__spr_x__v0;
    __VdlySet__vgc__DOT__spr_x__v0 = 0;
    CData/*7:0*/ __VdlyVal__vgc__DOT__spr_y__v0;
    __VdlyVal__vgc__DOT__spr_y__v0 = 0;
    CData/*3:0*/ __VdlyDim0__vgc__DOT__spr_y__v0;
    __VdlyDim0__vgc__DOT__spr_y__v0 = 0;
    CData/*7:0*/ __VdlyVal__vgc__DOT__sprite_shapes__v1;
    __VdlyVal__vgc__DOT__sprite_shapes__v1 = 0;
    SData/*10:0*/ __VdlyDim0__vgc__DOT__sprite_shapes__v1;
    __VdlyDim0__vgc__DOT__sprite_shapes__v1 = 0;
    CData/*0:0*/ __VdlySet__vgc__DOT__sprite_shapes__v1;
    __VdlySet__vgc__DOT__sprite_shapes__v1 = 0;
    CData/*7:0*/ __VdlyVal__vgc__DOT__sprite_shapes__v2;
    __VdlyVal__vgc__DOT__sprite_shapes__v2 = 0;
    SData/*10:0*/ __VdlyDim0__vgc__DOT__sprite_shapes__v2;
    __VdlyDim0__vgc__DOT__sprite_shapes__v2 = 0;
    CData/*7:0*/ __VdlyVal__vgc__DOT__sprite_shapes__v3;
    __VdlyVal__vgc__DOT__sprite_shapes__v3 = 0;
    SData/*10:0*/ __VdlyDim0__vgc__DOT__sprite_shapes__v3;
    __VdlyDim0__vgc__DOT__sprite_shapes__v3 = 0;
    CData/*7:0*/ __VdlyVal__vgc__DOT__sprite_shapes__v4;
    __VdlyVal__vgc__DOT__sprite_shapes__v4 = 0;
    SData/*10:0*/ __VdlyDim0__vgc__DOT__sprite_shapes__v4;
    __VdlyDim0__vgc__DOT__sprite_shapes__v4 = 0;
    CData/*7:0*/ __VdlyVal__vgc__DOT__sprite_shapes__v5;
    __VdlyVal__vgc__DOT__sprite_shapes__v5 = 0;
    SData/*10:0*/ __VdlyDim0__vgc__DOT__sprite_shapes__v5;
    __VdlyDim0__vgc__DOT__sprite_shapes__v5 = 0;
    CData/*7:0*/ __VdlyVal__vgc__DOT__sprite_shapes__v6;
    __VdlyVal__vgc__DOT__sprite_shapes__v6 = 0;
    SData/*10:0*/ __VdlyDim0__vgc__DOT__sprite_shapes__v6;
    __VdlyDim0__vgc__DOT__sprite_shapes__v6 = 0;
    CData/*7:0*/ __VdlyVal__vgc__DOT__sprite_shapes__v7;
    __VdlyVal__vgc__DOT__sprite_shapes__v7 = 0;
    SData/*10:0*/ __VdlyDim0__vgc__DOT__sprite_shapes__v7;
    __VdlyDim0__vgc__DOT__sprite_shapes__v7 = 0;
    CData/*7:0*/ __VdlyVal__vgc__DOT__sprite_shapes__v8;
    __VdlyVal__vgc__DOT__sprite_shapes__v8 = 0;
    SData/*10:0*/ __VdlyDim0__vgc__DOT__sprite_shapes__v8;
    __VdlyDim0__vgc__DOT__sprite_shapes__v8 = 0;
    CData/*3:0*/ __VdlyVal__vgc__DOT__gfx_ram__v4;
    __VdlyVal__vgc__DOT__gfx_ram__v4 = 0;
    SData/*15:0*/ __VdlyDim0__vgc__DOT__gfx_ram__v4;
    __VdlyDim0__vgc__DOT__gfx_ram__v4 = 0;
    CData/*0:0*/ __VdlySet__vgc__DOT__gfx_ram__v4;
    __VdlySet__vgc__DOT__gfx_ram__v4 = 0;
    CData/*3:0*/ __VdlyVal__vgc__DOT__gfx_ram__v5;
    __VdlyVal__vgc__DOT__gfx_ram__v5 = 0;
    SData/*15:0*/ __VdlyDim0__vgc__DOT__gfx_ram__v5;
    __VdlyDim0__vgc__DOT__gfx_ram__v5 = 0;
    CData/*0:0*/ __VdlySet__vgc__DOT__gfx_ram__v5;
    __VdlySet__vgc__DOT__gfx_ram__v5 = 0;
    CData/*7:0*/ __VdlyVal__vgc__DOT__regs__v0;
    __VdlyVal__vgc__DOT__regs__v0 = 0;
    CData/*4:0*/ __VdlyDim0__vgc__DOT__regs__v0;
    __VdlyDim0__vgc__DOT__regs__v0 = 0;
    CData/*0:0*/ __VdlySet__vgc__DOT__regs__v0;
    __VdlySet__vgc__DOT__regs__v0 = 0;
    CData/*7:0*/ __VdlyVal__vgc__DOT__char_ram__v3;
    __VdlyVal__vgc__DOT__char_ram__v3 = 0;
    SData/*10:0*/ __VdlyDim0__vgc__DOT__char_ram__v3;
    __VdlyDim0__vgc__DOT__char_ram__v3 = 0;
    CData/*0:0*/ __VdlySet__vgc__DOT__char_ram__v3;
    __VdlySet__vgc__DOT__char_ram__v3 = 0;
    CData/*7:0*/ __VdlyVal__vgc__DOT__color_ram__v2;
    __VdlyVal__vgc__DOT__color_ram__v2 = 0;
    SData/*10:0*/ __VdlyDim0__vgc__DOT__color_ram__v2;
    __VdlyDim0__vgc__DOT__color_ram__v2 = 0;
    CData/*0:0*/ __VdlySet__vgc__DOT__color_ram__v2;
    __VdlySet__vgc__DOT__color_ram__v2 = 0;
    CData/*1:0*/ __VdlyVal__vgc__DOT__spr_pri__v1;
    __VdlyVal__vgc__DOT__spr_pri__v1 = 0;
    CData/*3:0*/ __VdlyDim0__vgc__DOT__spr_pri__v1;
    __VdlyDim0__vgc__DOT__spr_pri__v1 = 0;
    CData/*0:0*/ __VdlySet__vgc__DOT__spr_pri__v1;
    __VdlySet__vgc__DOT__spr_pri__v1 = 0;
    CData/*0:0*/ __VdlyVal__vgc__DOT__spr_flip_h__v1;
    __VdlyVal__vgc__DOT__spr_flip_h__v1 = 0;
    CData/*3:0*/ __VdlyDim0__vgc__DOT__spr_flip_h__v1;
    __VdlyDim0__vgc__DOT__spr_flip_h__v1 = 0;
    CData/*0:0*/ __VdlySet__vgc__DOT__spr_flip_h__v1;
    __VdlySet__vgc__DOT__spr_flip_h__v1 = 0;
    CData/*0:0*/ __VdlyVal__vgc__DOT__spr_flip_v__v1;
    __VdlyVal__vgc__DOT__spr_flip_v__v1 = 0;
    CData/*3:0*/ __VdlyDim0__vgc__DOT__spr_flip_v__v1;
    __VdlyDim0__vgc__DOT__spr_flip_v__v1 = 0;
    CData/*0:0*/ __VdlyVal__vgc__DOT__spr_enable__v2;
    __VdlyVal__vgc__DOT__spr_enable__v2 = 0;
    CData/*3:0*/ __VdlyDim0__vgc__DOT__spr_enable__v2;
    __VdlyDim0__vgc__DOT__spr_enable__v2 = 0;
    CData/*3:0*/ __VdlyVal__vgc__DOT__spr_shape__v0;
    __VdlyVal__vgc__DOT__spr_shape__v0 = 0;
    CData/*3:0*/ __VdlyDim0__vgc__DOT__spr_shape__v0;
    __VdlyDim0__vgc__DOT__spr_shape__v0 = 0;
    CData/*0:0*/ __VdlySet__vgc__DOT__spr_shape__v0;
    __VdlySet__vgc__DOT__spr_shape__v0 = 0;
    CData/*7:0*/ __VdlyVal__vgc__DOT__spr_y__v1;
    __VdlyVal__vgc__DOT__spr_y__v1 = 0;
    CData/*3:0*/ __VdlyDim0__vgc__DOT__spr_y__v1;
    __VdlyDim0__vgc__DOT__spr_y__v1 = 0;
    CData/*0:0*/ __VdlySet__vgc__DOT__spr_y__v1;
    __VdlySet__vgc__DOT__spr_y__v1 = 0;
    CData/*0:0*/ __VdlyVal__vgc__DOT__spr_x__v1;
    __VdlyVal__vgc__DOT__spr_x__v1 = 0;
    CData/*3:0*/ __VdlyDim0__vgc__DOT__spr_x__v1;
    __VdlyDim0__vgc__DOT__spr_x__v1 = 0;
    CData/*0:0*/ __VdlySet__vgc__DOT__spr_x__v1;
    __VdlySet__vgc__DOT__spr_x__v1 = 0;
    CData/*7:0*/ __VdlyVal__vgc__DOT__spr_x__v2;
    __VdlyVal__vgc__DOT__spr_x__v2 = 0;
    CData/*3:0*/ __VdlyDim0__vgc__DOT__spr_x__v2;
    __VdlyDim0__vgc__DOT__spr_x__v2 = 0;
    CData/*0:0*/ __VdlySet__vgc__DOT__spr_x__v2;
    __VdlySet__vgc__DOT__spr_x__v2 = 0;
    CData/*7:0*/ __VdlyVal__vgc__DOT__regs__v1;
    __VdlyVal__vgc__DOT__regs__v1 = 0;
    CData/*4:0*/ __VdlyDim0__vgc__DOT__regs__v1;
    __VdlyDim0__vgc__DOT__regs__v1 = 0;
    CData/*0:0*/ __VdlySet__vgc__DOT__regs__v1;
    __VdlySet__vgc__DOT__regs__v1 = 0;
    // Body
    __Vdly__vgc__DOT__char_in_reg = vlSelfRef.vgc__DOT__char_in_reg;
    __Vdly__vgc__DOT__cursor_blink = vlSelfRef.vgc__DOT__cursor_blink;
    __Vdly__vgc__DOT__blink_count = vlSelfRef.vgc__DOT__blink_count;
    __Vdly__vgc__DOT__h_count = vlSelfRef.vgc__DOT__h_count;
    __Vdly__vgc__DOT__v_count = vlSelfRef.vgc__DOT__v_count;
    __Vdly__vgc__DOT__scroll_pending = vlSelfRef.vgc__DOT__scroll_pending;
    __Vdly__vgc__DOT__scroll_col = vlSelfRef.vgc__DOT__scroll_col;
    __Vdly__vgc__DOT__cmd_err = vlSelfRef.vgc__DOT__cmd_err;
    __Vdly__vgc__DOT__cmd_x = vlSelfRef.vgc__DOT__cmd_x;
    __Vdly__vgc__DOT__cmd_y = vlSelfRef.vgc__DOT__cmd_y;
    __Vdly__vgc__DOT__cmd_cx = vlSelfRef.vgc__DOT__cmd_cx;
    __Vdly__vgc__DOT__cmd_cy = vlSelfRef.vgc__DOT__cmd_cy;
    __Vdly__vgc__DOT__copper_count = vlSelfRef.vgc__DOT__copper_count;
    __VdlySet__vgc__DOT__copper_pos__v0 = 0U;
    __Vdly__vgc__DOT__gfx_color = vlSelfRef.vgc__DOT__gfx_color;
    __Vdly__vgc__DOT__cmd_busy = vlSelfRef.vgc__DOT__cmd_busy;
    __VdlySet__vgc__DOT__regs__v0 = 0U;
    __Vdly__vgc__DOT__cursor_x = vlSelfRef.vgc__DOT__cursor_x;
    __Vdly__vgc__DOT__cursor_y = vlSelfRef.vgc__DOT__cursor_y;
    __VdlySet__vgc__DOT__spr_shape__v0 = 0U;
    __VdlySet__vgc__DOT__spr_pri__v0 = 0U;
    __VdlySet__vgc__DOT__spr_pri__v1 = 0U;
    __VdlySet__vgc__DOT__spr_y__v1 = 0U;
    __VdlySet__vgc__DOT__spr_enable__v0 = 0U;
    __VdlySet__vgc__DOT__spr_enable__v1 = 0U;
    __VdlySet__vgc__DOT__spr_x__v1 = 0U;
    __VdlySet__vgc__DOT__spr_x__v2 = 0U;
    __VdlySet__vgc__DOT__gfx_ram__v0 = 0U;
    __VdlySet__vgc__DOT__gfx_ram__v1 = 0U;
    __VdlySet__vgc__DOT__gfx_ram__v2 = 0U;
    __VdlySet__vgc__DOT__gfx_ram__v3 = 0U;
    __VdlySet__vgc__DOT__gfx_ram__v4 = 0U;
    __VdlySet__vgc__DOT__gfx_ram__v5 = 0U;
    __VdlySet__vgc__DOT__sprite_shapes__v0 = 0U;
    __VdlySet__vgc__DOT__sprite_shapes__v1 = 0U;
    __VdlySet__vgc__DOT__color_ram__v0 = 0U;
    __VdlySet__vgc__DOT__color_ram__v1 = 0U;
    __VdlySet__vgc__DOT__color_ram__v2 = 0U;
    __VdlySet__vgc__DOT__char_ram__v0 = 0U;
    __VdlySet__vgc__DOT__char_ram__v1 = 0U;
    __VdlySet__vgc__DOT__char_ram__v2 = 0U;
    __VdlySet__vgc__DOT__char_ram__v3 = 0U;
    __VdlySet__vgc__DOT__spr_flip_h__v0 = 0U;
    __VdlySet__vgc__DOT__spr_x__v0 = 0U;
    __VdlySet__vgc__DOT__spr_flip_h__v1 = 0U;
    __Vdly__vgc__DOT__scroll_offset = vlSelfRef.vgc__DOT__scroll_offset;
    __VdlySet__vgc__DOT__regs__v1 = 0U;
    __Vdly__vgc__DOT__copper_index = vlSelfRef.vgc__DOT__copper_index;
    __Vdly__vgc__DOT__fg_color = vlSelfRef.vgc__DOT__fg_color;
    if (vlSelfRef.vgc__DOT__key_valid) {
        __Vdly__vgc__DOT__char_in_reg = vlSelfRef.vgc__DOT__key_data;
    } else if (((((IData)(vlSelfRef.vgc__DOT__cpu_re) 
                  & (IData)(vlSelfRef.vgc__DOT__cpu_ce)) 
                 & (0xa00fU == (IData)(vlSelfRef.vgc__DOT__cpu_addr))) 
                & (0U != (IData)(vlSelfRef.vgc__DOT__char_in_reg)))) {
        __Vdly__vgc__DOT__char_in_reg = 0U;
    }
    if (((0U == (IData)(vlSelfRef.vgc__DOT__h_count)) 
         & (0U == (IData)(vlSelfRef.vgc__DOT__v_count)))) {
        if ((0x0eU <= (IData)(vlSelfRef.vgc__DOT__blink_count))) {
            __Vdly__vgc__DOT__cursor_blink = (1U & 
                                              (~ (IData)(vlSelfRef.vgc__DOT__cursor_blink)));
            __Vdly__vgc__DOT__blink_count = 0U;
        } else {
            __Vdly__vgc__DOT__blink_count = (0x0000001fU 
                                             & ((IData)(1U) 
                                                + (IData)(vlSelfRef.vgc__DOT__blink_count)));
        }
    }
    vlSelfRef.vgc__DOT__vid_hsync = (1U & (~ (IData)(vlSelfRef.vgc__DOT__h_sync_area)));
    vlSelfRef.vgc__DOT__vid_vsync = (1U & (~ (IData)(vlSelfRef.vgc__DOT__v_sync_area)));
    if (((IData)(vlSelfRef.vgc__DOT__cursor_here) & (IData)(vlSelfRef.vgc__DOT__cursor_blink))) {
        vlSelfRef.vgc__DOT__vid_r = (0x0000000fU & 
                                     (vlSelfRef.vgc__DOT__palette
                                      [vlSelfRef.vgc__DOT__fg_color] 
                                      >> 8U));
        vlSelfRef.vgc__DOT__vid_b = (0x0000000fU & vlSelfRef.vgc__DOT__palette
                                     [vlSelfRef.vgc__DOT__fg_color]);
        vlSelfRef.vgc__DOT__vid_g = (0x0000000fU & 
                                     (vlSelfRef.vgc__DOT__palette
                                      [vlSelfRef.vgc__DOT__fg_color] 
                                      >> 4U));
    } else {
        vlSelfRef.vgc__DOT__vid_r = (0x0000000fU & 
                                     ((IData)(vlSelfRef.vgc__DOT__pixel_color) 
                                      >> 8U));
        vlSelfRef.vgc__DOT__vid_b = (0x0000000fU & (IData)(vlSelfRef.vgc__DOT__pixel_color));
        vlSelfRef.vgc__DOT__vid_g = (0x0000000fU & 
                                     ((IData)(vlSelfRef.vgc__DOT__pixel_color) 
                                      >> 4U));
    }
    vlSelfRef.vgc__DOT__vid_de = vlSelfRef.vgc__DOT__visible;
    vlSelfRef.vgc__DOT__char_in_reg = __Vdly__vgc__DOT__char_in_reg;
    vlSelfRef.vgc__DOT__blink_count = __Vdly__vgc__DOT__blink_count;
    vlSelfRef.vgc__DOT__cursor_blink = __Vdly__vgc__DOT__cursor_blink;
    if (vlSelfRef.vgc__DOT__rst) {
        __Vdly__vgc__DOT__h_count = 0U;
        __Vdly__vgc__DOT__v_count = 0U;
        __Vdly__vgc__DOT__copper_index = 0U;
    } else {
        if ((0x031fU == (IData)(vlSelfRef.vgc__DOT__h_count))) {
            __Vdly__vgc__DOT__v_count = ((0x020cU == (IData)(vlSelfRef.vgc__DOT__v_count))
                                          ? 0U : (0x000003ffU 
                                                  & ((IData)(1U) 
                                                     + (IData)(vlSelfRef.vgc__DOT__v_count))));
            __Vdly__vgc__DOT__h_count = 0U;
        } else {
            __Vdly__vgc__DOT__h_count = (0x000003ffU 
                                         & ((IData)(1U) 
                                            + (IData)(vlSelfRef.vgc__DOT__h_count)));
        }
        if (((0U == (IData)(vlSelfRef.vgc__DOT__h_count)) 
             & (0x01e0U == (IData)(vlSelfRef.vgc__DOT__v_count)))) {
            __Vdly__vgc__DOT__copper_index = 0U;
        }
        if (((((IData)(vlSelfRef.vgc__DOT__copper_enabled) 
               & (IData)(vlSelfRef.vgc__DOT__in_text_area)) 
              & ((IData)(vlSelfRef.vgc__DOT__copper_index) 
                 < (IData)(vlSelfRef.vgc__DOT__copper_count))) 
             & (vlSelfRef.vgc__DOT__beam_pos >= vlSelfRef.vgc__DOT__copper_pos
                [(0x000000ffU & (IData)(vlSelfRef.vgc__DOT__copper_index))]))) {
            if ((1U == vlSelfRef.vgc__DOT__copper_reg
                 [(0x000000ffU & (IData)(vlSelfRef.vgc__DOT__copper_index))])) {
                vlSelfRef.vgc__DOT__bg_color = (0x0000000fU 
                                                & vlSelfRef.vgc__DOT__copper_val
                                                [(0x000000ffU 
                                                  & (IData)(vlSelfRef.vgc__DOT__copper_index))]);
            } else if ((2U == vlSelfRef.vgc__DOT__copper_reg
                        [(0x000000ffU & (IData)(vlSelfRef.vgc__DOT__copper_index))])) {
                __Vdly__vgc__DOT__fg_color = (0x0000000fU 
                                              & vlSelfRef.vgc__DOT__copper_val
                                              [(0x000000ffU 
                                                & (IData)(vlSelfRef.vgc__DOT__copper_index))]);
            } else if ((0x0dU == vlSelfRef.vgc__DOT__copper_reg
                        [(0x000000ffU & (IData)(vlSelfRef.vgc__DOT__copper_index))])) {
                vlSelfRef.vgc__DOT__border_color = 
                    (0x0000000fU & vlSelfRef.vgc__DOT__copper_val
                     [(0x000000ffU & (IData)(vlSelfRef.vgc__DOT__copper_index))]);
            } else {
                __VdlyVal__vgc__DOT__regs__v1 = vlSelfRef.vgc__DOT__copper_val
                    [(0x000000ffU & (IData)(vlSelfRef.vgc__DOT__copper_index))];
                __VdlyDim0__vgc__DOT__regs__v1 = (0x0000001fU 
                                                  & vlSelfRef.vgc__DOT__copper_reg
                                                  [
                                                  (0x000000ffU 
                                                   & (IData)(vlSelfRef.vgc__DOT__copper_index))]);
                __VdlySet__vgc__DOT__regs__v1 = 1U;
            }
            __Vdly__vgc__DOT__copper_index = (0x000001ffU 
                                              & ((IData)(1U) 
                                                 + (IData)(vlSelfRef.vgc__DOT__copper_index)));
        }
    }
    vlSelfRef.vgc__DOT__h_count = __Vdly__vgc__DOT__h_count;
    vlSelfRef.vgc__DOT__v_count = __Vdly__vgc__DOT__v_count;
    vlSelfRef.vid_hsync = vlSelfRef.vgc__DOT__vid_hsync;
    vlSelfRef.vid_vsync = vlSelfRef.vgc__DOT__vid_vsync;
    vlSelfRef.vid_r = vlSelfRef.vgc__DOT__vid_r;
    vlSelfRef.vid_b = vlSelfRef.vgc__DOT__vid_b;
    vlSelfRef.vid_g = vlSelfRef.vgc__DOT__vid_g;
    vlSelfRef.vid_de = vlSelfRef.vgc__DOT__vid_de;
    vlSelfRef.vgc__DOT__h_sync_area = ((0x0290U <= (IData)(vlSelfRef.vgc__DOT__h_count)) 
                                       & (0x02f0U > (IData)(vlSelfRef.vgc__DOT__h_count)));
    vlSelfRef.vgc__DOT__font_pixel = (7U & (IData)(vlSelfRef.vgc__DOT__h_count));
    vlSelfRef.vgc__DOT__gfx_x = (0x000001ffU & ((IData)(vlSelfRef.vgc__DOT__h_count) 
                                                >> 1U));
    vlSelfRef.vgc__DOT__text_col = (0x0000007fU & ((IData)(vlSelfRef.vgc__DOT__h_count) 
                                                   >> 3U));
    vlSelfRef.vgc__DOT__h_visible = (0x0280U > (IData)(vlSelfRef.vgc__DOT__h_count));
    vlSelfRef.vgc__DOT__v_sync_area = ((0x01eaU <= (IData)(vlSelfRef.vgc__DOT__v_count)) 
                                       & (0x01ecU > (IData)(vlSelfRef.vgc__DOT__v_count)));
    vlSelfRef.vgc__DOT__v_visible = (0x01e0U > (IData)(vlSelfRef.vgc__DOT__v_count));
    if (vlSelfRef.vgc__DOT__rst) {
        __Vdly__vgc__DOT__cursor_x = 0U;
        __Vdly__vgc__DOT__cursor_y = 0U;
        vlSelfRef.vgc__DOT__mode = 0U;
        __Vdly__vgc__DOT__fg_color = 1U;
        vlSelfRef.vgc__DOT__bg_color = 6U;
        vlSelfRef.vgc__DOT__border_color = 6U;
        __Vdly__vgc__DOT__gfx_color = 1U;
        __Vdly__vgc__DOT__scroll_offset = 0U;
        __Vdly__vgc__DOT__scroll_pending = 0U;
        __Vdly__vgc__DOT__scroll_col = 0U;
        __Vdly__vgc__DOT__cmd_busy = 0U;
    } else {
        if (vlSelfRef.vgc__DOT__scroll_pending) {
            vlSelfRef.vgc__DOT____Vlvbound_h902d5c6a__0 = 0x20U;
            if (VL_LIKELY(((0x07cfU >= ([&]() {
                                    __Vfunc_vgc__DOT__screen_addr__0__col 
                                        = vlSelfRef.vgc__DOT__scroll_col;
                                    __Vfunc_vgc__DOT__screen_addr__0__rr 
                                        = (0x0000001fU 
                                           & ((IData)(0x18U) 
                                              + (IData)(vlSelfRef.vgc__DOT__scroll_offset)));
                                    if ((0x19U <= (IData)(__Vfunc_vgc__DOT__screen_addr__0__rr))) {
                                        __Vfunc_vgc__DOT__screen_addr__0__rr 
                                            = (0x0000001fU 
                                               & ((IData)(__Vfunc_vgc__DOT__screen_addr__0__rr) 
                                                  - (IData)(0x19U)));
                                    }
                                    __Vfunc_vgc__DOT__screen_addr__0__Vfuncout 
                                        = (0x000007ffU 
                                           & (((IData)(0x00000050U) 
                                               * (IData)(__Vfunc_vgc__DOT__screen_addr__0__rr)) 
                                              + (IData)(__Vfunc_vgc__DOT__screen_addr__0__col)));
                                }(), (IData)(__Vfunc_vgc__DOT__screen_addr__0__Vfuncout)))))) {
                __Vfunc_vgc__DOT__screen_addr__1__col 
                    = vlSelfRef.vgc__DOT__scroll_col;
                __Vfunc_vgc__DOT__screen_addr__1__rr 
                    = (0x0000001fU & ((IData)(0x18U) 
                                      + (IData)(vlSelfRef.vgc__DOT__scroll_offset)));
                if ((0x19U <= (IData)(__Vfunc_vgc__DOT__screen_addr__1__rr))) {
                    __Vfunc_vgc__DOT__screen_addr__1__rr 
                        = (0x0000001fU & ((IData)(__Vfunc_vgc__DOT__screen_addr__1__rr) 
                                          - (IData)(0x19U)));
                }
                __Vfunc_vgc__DOT__screen_addr__1__Vfuncout 
                    = (0x000007ffU & (((IData)(0x00000050U) 
                                       * (IData)(__Vfunc_vgc__DOT__screen_addr__1__rr)) 
                                      + (IData)(__Vfunc_vgc__DOT__screen_addr__1__col)));
                __VdlyVal__vgc__DOT__char_ram__v0 = vlSelfRef.vgc__DOT____Vlvbound_h902d5c6a__0;
                __VdlyDim0__vgc__DOT__char_ram__v0 
                    = __Vfunc_vgc__DOT__screen_addr__1__Vfuncout;
                __VdlySet__vgc__DOT__char_ram__v0 = 1U;
            }
            vlSelfRef.vgc__DOT____Vlvbound_hdecc4094__0 
                = vlSelfRef.vgc__DOT__fg_color;
            if (VL_LIKELY(((0x07cfU >= ([&]() {
                                    __Vfunc_vgc__DOT__screen_addr__2__col 
                                        = vlSelfRef.vgc__DOT__scroll_col;
                                    __Vfunc_vgc__DOT__screen_addr__2__rr 
                                        = (0x0000001fU 
                                           & ((IData)(0x18U) 
                                              + (IData)(vlSelfRef.vgc__DOT__scroll_offset)));
                                    if ((0x19U <= (IData)(__Vfunc_vgc__DOT__screen_addr__2__rr))) {
                                        __Vfunc_vgc__DOT__screen_addr__2__rr 
                                            = (0x0000001fU 
                                               & ((IData)(__Vfunc_vgc__DOT__screen_addr__2__rr) 
                                                  - (IData)(0x19U)));
                                    }
                                    __Vfunc_vgc__DOT__screen_addr__2__Vfuncout 
                                        = (0x000007ffU 
                                           & (((IData)(0x00000050U) 
                                               * (IData)(__Vfunc_vgc__DOT__screen_addr__2__rr)) 
                                              + (IData)(__Vfunc_vgc__DOT__screen_addr__2__col)));
                                }(), (IData)(__Vfunc_vgc__DOT__screen_addr__2__Vfuncout)))))) {
                __Vfunc_vgc__DOT__screen_addr__3__col 
                    = vlSelfRef.vgc__DOT__scroll_col;
                __Vfunc_vgc__DOT__screen_addr__3__rr 
                    = (0x0000001fU & ((IData)(0x18U) 
                                      + (IData)(vlSelfRef.vgc__DOT__scroll_offset)));
                if ((0x19U <= (IData)(__Vfunc_vgc__DOT__screen_addr__3__rr))) {
                    __Vfunc_vgc__DOT__screen_addr__3__rr 
                        = (0x0000001fU & ((IData)(__Vfunc_vgc__DOT__screen_addr__3__rr) 
                                          - (IData)(0x19U)));
                }
                __Vfunc_vgc__DOT__screen_addr__3__Vfuncout 
                    = (0x000007ffU & (((IData)(0x00000050U) 
                                       * (IData)(__Vfunc_vgc__DOT__screen_addr__3__rr)) 
                                      + (IData)(__Vfunc_vgc__DOT__screen_addr__3__col)));
                __VdlyVal__vgc__DOT__color_ram__v0 
                    = vlSelfRef.vgc__DOT____Vlvbound_hdecc4094__0;
                __VdlyDim0__vgc__DOT__color_ram__v0 
                    = __Vfunc_vgc__DOT__screen_addr__3__Vfuncout;
                __VdlySet__vgc__DOT__color_ram__v0 = 1U;
            }
            if ((0x4fU == (IData)(vlSelfRef.vgc__DOT__scroll_col))) {
                __Vdly__vgc__DOT__scroll_pending = 0U;
                __Vdly__vgc__DOT__scroll_col = 0U;
            } else {
                __Vdly__vgc__DOT__scroll_col = (0x0000007fU 
                                                & ((IData)(1U) 
                                                   + (IData)(vlSelfRef.vgc__DOT__scroll_col)));
            }
        }
        if (vlSelfRef.vgc__DOT__cmd_busy) {
            if ((3U == (IData)(vlSelfRef.vgc__DOT__cmd_op))) {
                if ((((VL_LTES_III(32, 0U, VL_EXTENDS_II(32,10, (IData)(vlSelfRef.vgc__DOT__cmd_x))) 
                       & VL_GTS_III(32, 0x00000140U, 
                                    VL_EXTENDS_II(32,10, (IData)(vlSelfRef.vgc__DOT__cmd_x)))) 
                      & VL_LTES_III(32, 0U, VL_EXTENDS_II(32,10, (IData)(vlSelfRef.vgc__DOT__cmd_y)))) 
                     & VL_GTS_III(32, 0x000000c8U, 
                                  VL_EXTENDS_II(32,10, (IData)(vlSelfRef.vgc__DOT__cmd_y))))) {
                    vlSelfRef.vgc__DOT____Vlvbound_hc0a966b6__0 
                        = vlSelfRef.vgc__DOT__gfx_color;
                    if (VL_LIKELY(((0xf9ffU >= (0x0000ffffU 
                                                & (VL_MULS_III(32, (IData)(0x00000140U), 
                                                               VL_EXTENDS_II(32,10, (IData)(vlSelfRef.vgc__DOT__cmd_y))) 
                                                   + 
                                                   VL_EXTENDS_II(16,10, (IData)(vlSelfRef.vgc__DOT__cmd_x)))))))) {
                        __VdlyVal__vgc__DOT__gfx_ram__v0 
                            = vlSelfRef.vgc__DOT____Vlvbound_hc0a966b6__0;
                        __VdlyDim0__vgc__DOT__gfx_ram__v0 
                            = (0x0000ffffU & (VL_MULS_III(32, (IData)(0x00000140U), 
                                                          VL_EXTENDS_II(32,10, (IData)(vlSelfRef.vgc__DOT__cmd_y))) 
                                              + VL_EXTENDS_II(16,10, (IData)(vlSelfRef.vgc__DOT__cmd_x))));
                        __VdlySet__vgc__DOT__gfx_ram__v0 = 1U;
                    }
                }
                if ((((IData)(vlSelfRef.vgc__DOT__cmd_x) 
                      == (IData)(vlSelfRef.vgc__DOT__cmd_x2)) 
                     & ((IData)(vlSelfRef.vgc__DOT__cmd_y) 
                        == (IData)(vlSelfRef.vgc__DOT__cmd_y2)))) {
                    __Vdly__vgc__DOT__cmd_busy = 0U;
                } else if ((VL_GTES_III(32, VL_MULS_III(32, (IData)(2U), 
                                                        VL_EXTENDS_II(32,10, (IData)(vlSelfRef.vgc__DOT__cmd_err))), 
                                        VL_EXTENDS_II(32,10, (IData)(vlSelfRef.vgc__DOT__cmd_dy))) 
                            & VL_LTES_III(32, VL_MULS_III(32, (IData)(2U), 
                                                          VL_EXTENDS_II(32,10, (IData)(vlSelfRef.vgc__DOT__cmd_err))), 
                                          VL_EXTENDS_II(32,10, (IData)(vlSelfRef.vgc__DOT__cmd_dx))))) {
                    __Vdly__vgc__DOT__cmd_err = (0x000003ffU 
                                                 & (((IData)(vlSelfRef.vgc__DOT__cmd_err) 
                                                     + (IData)(vlSelfRef.vgc__DOT__cmd_dy)) 
                                                    + (IData)(vlSelfRef.vgc__DOT__cmd_dx)));
                    __Vdly__vgc__DOT__cmd_x = (0x000003ffU 
                                               & ((IData)(vlSelfRef.vgc__DOT__cmd_x) 
                                                  + (IData)(vlSelfRef.vgc__DOT__cmd_sx)));
                    __Vdly__vgc__DOT__cmd_y = (0x000003ffU 
                                               & ((IData)(vlSelfRef.vgc__DOT__cmd_y) 
                                                  + (IData)(vlSelfRef.vgc__DOT__cmd_sy)));
                } else if (VL_GTES_III(32, VL_MULS_III(32, (IData)(2U), 
                                                       VL_EXTENDS_II(32,10, (IData)(vlSelfRef.vgc__DOT__cmd_err))), 
                                       VL_EXTENDS_II(32,10, (IData)(vlSelfRef.vgc__DOT__cmd_dy)))) {
                    __Vdly__vgc__DOT__cmd_err = (0x000003ffU 
                                                 & ((IData)(vlSelfRef.vgc__DOT__cmd_err) 
                                                    + (IData)(vlSelfRef.vgc__DOT__cmd_dy)));
                    __Vdly__vgc__DOT__cmd_x = (0x000003ffU 
                                               & ((IData)(vlSelfRef.vgc__DOT__cmd_x) 
                                                  + (IData)(vlSelfRef.vgc__DOT__cmd_sx)));
                } else if (VL_LTES_III(32, VL_MULS_III(32, (IData)(2U), 
                                                       VL_EXTENDS_II(32,10, (IData)(vlSelfRef.vgc__DOT__cmd_err))), 
                                       VL_EXTENDS_II(32,10, (IData)(vlSelfRef.vgc__DOT__cmd_dx)))) {
                    __Vdly__vgc__DOT__cmd_err = (0x000003ffU 
                                                 & ((IData)(vlSelfRef.vgc__DOT__cmd_err) 
                                                    + (IData)(vlSelfRef.vgc__DOT__cmd_dx)));
                    __Vdly__vgc__DOT__cmd_y = (0x000003ffU 
                                               & ((IData)(vlSelfRef.vgc__DOT__cmd_y) 
                                                  + (IData)(vlSelfRef.vgc__DOT__cmd_sy)));
                }
            } else if ((5U == (IData)(vlSelfRef.vgc__DOT__cmd_op))) {
                if (((0x0140U > (IData)(vlSelfRef.vgc__DOT__cmd_cx)) 
                     & (0xc8U > (IData)(vlSelfRef.vgc__DOT__cmd_cy)))) {
                    if ((((((IData)(vlSelfRef.vgc__DOT__cmd_cy) 
                            == (0x000000ffU & (IData)(vlSelfRef.vgc__DOT__cmd_y))) 
                           | ((IData)(vlSelfRef.vgc__DOT__cmd_cy) 
                              == (0x000000ffU & (IData)(vlSelfRef.vgc__DOT__cmd_y2)))) 
                          | ((IData)(vlSelfRef.vgc__DOT__cmd_cx) 
                             == (0x000001ffU & (IData)(vlSelfRef.vgc__DOT__cmd_x)))) 
                         | ((IData)(vlSelfRef.vgc__DOT__cmd_cx) 
                            == (0x000001ffU & (IData)(vlSelfRef.vgc__DOT__cmd_x2))))) {
                        vlSelfRef.vgc__DOT____Vlvbound_h3f27e46b__0 
                            = vlSelfRef.vgc__DOT__gfx_color;
                        if (VL_LIKELY(((0xf9ffU >= 
                                        (0x0000ffffU 
                                         & (((IData)(0x00000140U) 
                                             * (IData)(vlSelfRef.vgc__DOT__cmd_cy)) 
                                            + (IData)(vlSelfRef.vgc__DOT__cmd_cx))))))) {
                            __VdlyVal__vgc__DOT__gfx_ram__v1 
                                = vlSelfRef.vgc__DOT____Vlvbound_h3f27e46b__0;
                            __VdlyDim0__vgc__DOT__gfx_ram__v1 
                                = (0x0000ffffU & (((IData)(0x00000140U) 
                                                   * (IData)(vlSelfRef.vgc__DOT__cmd_cy)) 
                                                  + (IData)(vlSelfRef.vgc__DOT__cmd_cx)));
                            __VdlySet__vgc__DOT__gfx_ram__v1 = 1U;
                        }
                    }
                }
                if (((IData)(vlSelfRef.vgc__DOT__cmd_cx) 
                     >= (0x000001ffU & (IData)(vlSelfRef.vgc__DOT__cmd_x2)))) {
                    __Vdly__vgc__DOT__cmd_cx = (0x000001ffU 
                                                & (IData)(vlSelfRef.vgc__DOT__cmd_x));
                    if (((IData)(vlSelfRef.vgc__DOT__cmd_cy) 
                         >= (0x000000ffU & (IData)(vlSelfRef.vgc__DOT__cmd_y2)))) {
                        __Vdly__vgc__DOT__cmd_busy = 0U;
                    } else {
                        __Vdly__vgc__DOT__cmd_cy = 
                            (0x000000ffU & ((IData)(1U) 
                                            + (IData)(vlSelfRef.vgc__DOT__cmd_cy)));
                    }
                } else {
                    __Vdly__vgc__DOT__cmd_cx = (0x000001ffU 
                                                & ((IData)(1U) 
                                                   + (IData)(vlSelfRef.vgc__DOT__cmd_cx)));
                }
            } else if ((6U == (IData)(vlSelfRef.vgc__DOT__cmd_op))) {
                if (((0x0140U > (IData)(vlSelfRef.vgc__DOT__cmd_cx)) 
                     & (0xc8U > (IData)(vlSelfRef.vgc__DOT__cmd_cy)))) {
                    vlSelfRef.vgc__DOT____Vlvbound_h3f27e46b__1 
                        = vlSelfRef.vgc__DOT__gfx_color;
                    if (VL_LIKELY(((0xf9ffU >= (0x0000ffffU 
                                                & (((IData)(0x00000140U) 
                                                    * (IData)(vlSelfRef.vgc__DOT__cmd_cy)) 
                                                   + (IData)(vlSelfRef.vgc__DOT__cmd_cx))))))) {
                        __VdlyVal__vgc__DOT__gfx_ram__v2 
                            = vlSelfRef.vgc__DOT____Vlvbound_h3f27e46b__1;
                        __VdlyDim0__vgc__DOT__gfx_ram__v2 
                            = (0x0000ffffU & (((IData)(0x00000140U) 
                                               * (IData)(vlSelfRef.vgc__DOT__cmd_cy)) 
                                              + (IData)(vlSelfRef.vgc__DOT__cmd_cx)));
                        __VdlySet__vgc__DOT__gfx_ram__v2 = 1U;
                    }
                }
                if (((IData)(vlSelfRef.vgc__DOT__cmd_cx) 
                     >= (0x000001ffU & (IData)(vlSelfRef.vgc__DOT__cmd_x2)))) {
                    __Vdly__vgc__DOT__cmd_cx = (0x000001ffU 
                                                & (IData)(vlSelfRef.vgc__DOT__cmd_x));
                    if (((IData)(vlSelfRef.vgc__DOT__cmd_cy) 
                         >= (0x000000ffU & (IData)(vlSelfRef.vgc__DOT__cmd_y2)))) {
                        __Vdly__vgc__DOT__cmd_busy = 0U;
                    } else {
                        __Vdly__vgc__DOT__cmd_cy = 
                            (0x000000ffU & ((IData)(1U) 
                                            + (IData)(vlSelfRef.vgc__DOT__cmd_cy)));
                    }
                } else {
                    __Vdly__vgc__DOT__cmd_cx = (0x000001ffU 
                                                & ((IData)(1U) 
                                                   + (IData)(vlSelfRef.vgc__DOT__cmd_cx)));
                }
            } else if ((7U == (IData)(vlSelfRef.vgc__DOT__cmd_op))) {
                vlSelfRef.vgc__DOT____Vlvbound_h3f27e46b__2 = 0U;
                if (VL_LIKELY(((0xf9ffU >= (0x0000ffffU 
                                            & (((IData)(0x00000140U) 
                                                * (IData)(vlSelfRef.vgc__DOT__cmd_cy)) 
                                               + (IData)(vlSelfRef.vgc__DOT__cmd_cx))))))) {
                    __VdlyVal__vgc__DOT__gfx_ram__v3 
                        = vlSelfRef.vgc__DOT____Vlvbound_h3f27e46b__2;
                    __VdlyDim0__vgc__DOT__gfx_ram__v3 
                        = (0x0000ffffU & (((IData)(0x00000140U) 
                                           * (IData)(vlSelfRef.vgc__DOT__cmd_cy)) 
                                          + (IData)(vlSelfRef.vgc__DOT__cmd_cx)));
                    __VdlySet__vgc__DOT__gfx_ram__v3 = 1U;
                }
                if ((0x013fU == (IData)(vlSelfRef.vgc__DOT__cmd_cx))) {
                    __Vdly__vgc__DOT__cmd_cx = 0U;
                    if ((0xc7U == (IData)(vlSelfRef.vgc__DOT__cmd_cy))) {
                        __Vdly__vgc__DOT__cmd_busy = 0U;
                    } else {
                        __Vdly__vgc__DOT__cmd_cy = 
                            (0x000000ffU & ((IData)(1U) 
                                            + (IData)(vlSelfRef.vgc__DOT__cmd_cy)));
                    }
                } else {
                    __Vdly__vgc__DOT__cmd_cx = (0x000001ffU 
                                                & ((IData)(1U) 
                                                   + (IData)(vlSelfRef.vgc__DOT__cmd_cx)));
                }
            } else if ((0x12U == (IData)(vlSelfRef.vgc__DOT__cmd_op))) {
                __VdlyDim0__vgc__DOT__sprite_shapes__v0 
                    = ((0x00000780U & ((IData)(vlSelfRef.vgc__DOT__cmd_x) 
                                       << 6U)) | ((0x00000078U 
                                                   & ((IData)(vlSelfRef.vgc__DOT__cmd_cy) 
                                                      << 3U)) 
                                                  | (7U 
                                                     & (IData)(vlSelfRef.vgc__DOT__cmd_cx))));
                __VdlySet__vgc__DOT__sprite_shapes__v0 = 1U;
                if ((7U == (IData)(vlSelfRef.vgc__DOT__cmd_cx))) {
                    __Vdly__vgc__DOT__cmd_cx = 0U;
                    if ((0x0fU == (IData)(vlSelfRef.vgc__DOT__cmd_cy))) {
                        __Vdly__vgc__DOT__cmd_busy = 0U;
                    } else {
                        __Vdly__vgc__DOT__cmd_cy = 
                            (0x000000ffU & ((IData)(1U) 
                                            + (IData)(vlSelfRef.vgc__DOT__cmd_cy)));
                    }
                } else {
                    __Vdly__vgc__DOT__cmd_cx = (0x000001ffU 
                                                & ((IData)(1U) 
                                                   + (IData)(vlSelfRef.vgc__DOT__cmd_cx)));
                }
            } else {
                __Vdly__vgc__DOT__cmd_busy = 0U;
            }
        }
        if (vlSelfRef.vgc__DOT__cpu_we) {
            if (vlSelfRef.vgc__DOT__vgc_reg_sel) {
                if (((((((((0U == (IData)(vlSelfRef.vgc__DOT__reg_offset)) 
                           | (1U == (IData)(vlSelfRef.vgc__DOT__reg_offset))) 
                          | (2U == (IData)(vlSelfRef.vgc__DOT__reg_offset))) 
                         | (3U == (IData)(vlSelfRef.vgc__DOT__reg_offset))) 
                        | (4U == (IData)(vlSelfRef.vgc__DOT__reg_offset))) 
                       | (8U == (IData)(vlSelfRef.vgc__DOT__reg_offset))) 
                      | (0x0dU == (IData)(vlSelfRef.vgc__DOT__reg_offset))) 
                     | (0x0eU == (IData)(vlSelfRef.vgc__DOT__reg_offset)))) {
                    if ((0U == (IData)(vlSelfRef.vgc__DOT__reg_offset))) {
                        vlSelfRef.vgc__DOT__mode = 
                            (7U & (IData)(vlSelfRef.vgc__DOT__cpu_wdata));
                    } else if ((1U == (IData)(vlSelfRef.vgc__DOT__reg_offset))) {
                        vlSelfRef.vgc__DOT__bg_color 
                            = (0x0000000fU & (IData)(vlSelfRef.vgc__DOT__cpu_wdata));
                    } else if ((2U == (IData)(vlSelfRef.vgc__DOT__reg_offset))) {
                        __Vdly__vgc__DOT__fg_color 
                            = (0x0000000fU & (IData)(vlSelfRef.vgc__DOT__cpu_wdata));
                    } else if ((3U == (IData)(vlSelfRef.vgc__DOT__reg_offset))) {
                        __Vdly__vgc__DOT__cursor_x 
                            = (0x0000007fU & (IData)(vlSelfRef.vgc__DOT__cpu_wdata));
                    } else if ((4U == (IData)(vlSelfRef.vgc__DOT__reg_offset))) {
                        __Vdly__vgc__DOT__cursor_y 
                            = (0x0000001fU & (IData)(vlSelfRef.vgc__DOT__cpu_wdata));
                    } else if ((8U == (IData)(vlSelfRef.vgc__DOT__reg_offset))) {
                        __Vdly__vgc__DOT__gfx_color 
                            = (0x0000000fU & (IData)(vlSelfRef.vgc__DOT__cpu_wdata));
                    } else if ((0x0dU == (IData)(vlSelfRef.vgc__DOT__reg_offset))) {
                        vlSelfRef.vgc__DOT__border_color 
                            = (0x0000000fU & (IData)(vlSelfRef.vgc__DOT__cpu_wdata));
                    } else if ((8U == (IData)(vlSelfRef.vgc__DOT__cpu_wdata))) {
                        if ((0U < (IData)(vlSelfRef.vgc__DOT__cursor_x))) {
                            __Vdly__vgc__DOT__cursor_x 
                                = (0x0000007fU & ((IData)(vlSelfRef.vgc__DOT__cursor_x) 
                                                  - (IData)(1U)));
                        }
                        vlSelfRef.vgc__DOT____Vlvbound_hb8130fd5__0 = 0x20U;
                        if (VL_LIKELY(((0x07cfU >= 
                                        ([&]() {
                                                __Vfunc_vgc__DOT__screen_addr__4__row 
                                                    = vlSelfRef.vgc__DOT__cursor_y;
                                                __Vfunc_vgc__DOT__screen_addr__4__col 
                                                    = 
                                                    ((0U 
                                                      < (IData)(vlSelfRef.vgc__DOT__cursor_x))
                                                      ? 
                                                     (0x0000007fU 
                                                      & ((IData)(vlSelfRef.vgc__DOT__cursor_x) 
                                                         - (IData)(1U)))
                                                      : 0U);
                                                __Vfunc_vgc__DOT__screen_addr__4__rr 
                                                    = 
                                                    (0x0000001fU 
                                                     & ((IData)(__Vfunc_vgc__DOT__screen_addr__4__row) 
                                                        + (IData)(vlSelfRef.vgc__DOT__scroll_offset)));
                                                if (
                                                    (0x19U 
                                                     <= (IData)(__Vfunc_vgc__DOT__screen_addr__4__rr))) {
                                                    __Vfunc_vgc__DOT__screen_addr__4__rr 
                                                        = 
                                                        (0x0000001fU 
                                                         & ((IData)(__Vfunc_vgc__DOT__screen_addr__4__rr) 
                                                            - (IData)(0x19U)));
                                                }
                                                __Vfunc_vgc__DOT__screen_addr__4__Vfuncout 
                                                    = 
                                                    (0x000007ffU 
                                                     & (((IData)(0x00000050U) 
                                                         * (IData)(__Vfunc_vgc__DOT__screen_addr__4__rr)) 
                                                        + (IData)(__Vfunc_vgc__DOT__screen_addr__4__col)));
                                            }(), (IData)(__Vfunc_vgc__DOT__screen_addr__4__Vfuncout)))))) {
                            __Vfunc_vgc__DOT__screen_addr__5__row 
                                = vlSelfRef.vgc__DOT__cursor_y;
                            __Vfunc_vgc__DOT__screen_addr__5__col 
                                = ((0U < (IData)(vlSelfRef.vgc__DOT__cursor_x))
                                    ? (0x0000007fU 
                                       & ((IData)(vlSelfRef.vgc__DOT__cursor_x) 
                                          - (IData)(1U)))
                                    : 0U);
                            __Vfunc_vgc__DOT__screen_addr__5__rr 
                                = (0x0000001fU & ((IData)(__Vfunc_vgc__DOT__screen_addr__5__row) 
                                                  + (IData)(vlSelfRef.vgc__DOT__scroll_offset)));
                            if ((0x19U <= (IData)(__Vfunc_vgc__DOT__screen_addr__5__rr))) {
                                __Vfunc_vgc__DOT__screen_addr__5__rr 
                                    = (0x0000001fU 
                                       & ((IData)(__Vfunc_vgc__DOT__screen_addr__5__rr) 
                                          - (IData)(0x19U)));
                            }
                            __Vfunc_vgc__DOT__screen_addr__5__Vfuncout 
                                = (0x000007ffU & (((IData)(0x00000050U) 
                                                   * (IData)(__Vfunc_vgc__DOT__screen_addr__5__rr)) 
                                                  + (IData)(__Vfunc_vgc__DOT__screen_addr__5__col)));
                            __VdlyVal__vgc__DOT__char_ram__v1 
                                = vlSelfRef.vgc__DOT____Vlvbound_hb8130fd5__0;
                            __VdlyDim0__vgc__DOT__char_ram__v1 
                                = __Vfunc_vgc__DOT__screen_addr__5__Vfuncout;
                            __VdlySet__vgc__DOT__char_ram__v1 = 1U;
                        }
                    } else if ((0x0aU == (IData)(vlSelfRef.vgc__DOT__cpu_wdata))) {
                        if ((0x18U <= (IData)(vlSelfRef.vgc__DOT__cursor_y))) {
                            __Vdly__vgc__DOT__scroll_offset 
                                = ((0x18U <= (IData)(vlSelfRef.vgc__DOT__scroll_offset))
                                    ? 0U : (0x0000001fU 
                                            & ((IData)(1U) 
                                               + (IData)(vlSelfRef.vgc__DOT__scroll_offset))));
                            __Vdly__vgc__DOT__scroll_pending = 1U;
                            __Vdly__vgc__DOT__scroll_col = 0U;
                        } else {
                            __Vdly__vgc__DOT__cursor_y 
                                = (0x0000001fU & ((IData)(1U) 
                                                  + (IData)(vlSelfRef.vgc__DOT__cursor_y)));
                        }
                    } else if ((0x0dU == (IData)(vlSelfRef.vgc__DOT__cpu_wdata))) {
                        __Vdly__vgc__DOT__cursor_x = 0U;
                    } else if ((0x20U <= (IData)(vlSelfRef.vgc__DOT__cpu_wdata))) {
                        vlSelfRef.vgc__DOT____Vlvbound_h2b480197__0 
                            = vlSelfRef.vgc__DOT__cpu_wdata;
                        if (VL_LIKELY(((0x07cfU >= 
                                        ([&]() {
                                                __Vfunc_vgc__DOT__screen_addr__6__row 
                                                    = vlSelfRef.vgc__DOT__cursor_y;
                                                __Vfunc_vgc__DOT__screen_addr__6__col 
                                                    = vlSelfRef.vgc__DOT__cursor_x;
                                                __Vfunc_vgc__DOT__screen_addr__6__rr 
                                                    = 
                                                    (0x0000001fU 
                                                     & ((IData)(__Vfunc_vgc__DOT__screen_addr__6__row) 
                                                        + (IData)(vlSelfRef.vgc__DOT__scroll_offset)));
                                                if (
                                                    (0x19U 
                                                     <= (IData)(__Vfunc_vgc__DOT__screen_addr__6__rr))) {
                                                    __Vfunc_vgc__DOT__screen_addr__6__rr 
                                                        = 
                                                        (0x0000001fU 
                                                         & ((IData)(__Vfunc_vgc__DOT__screen_addr__6__rr) 
                                                            - (IData)(0x19U)));
                                                }
                                                __Vfunc_vgc__DOT__screen_addr__6__Vfuncout 
                                                    = 
                                                    (0x000007ffU 
                                                     & (((IData)(0x00000050U) 
                                                         * (IData)(__Vfunc_vgc__DOT__screen_addr__6__rr)) 
                                                        + (IData)(__Vfunc_vgc__DOT__screen_addr__6__col)));
                                            }(), (IData)(__Vfunc_vgc__DOT__screen_addr__6__Vfuncout)))))) {
                            __Vfunc_vgc__DOT__screen_addr__7__row 
                                = vlSelfRef.vgc__DOT__cursor_y;
                            __Vfunc_vgc__DOT__screen_addr__7__col 
                                = vlSelfRef.vgc__DOT__cursor_x;
                            __Vfunc_vgc__DOT__screen_addr__7__rr 
                                = (0x0000001fU & ((IData)(__Vfunc_vgc__DOT__screen_addr__7__row) 
                                                  + (IData)(vlSelfRef.vgc__DOT__scroll_offset)));
                            if ((0x19U <= (IData)(__Vfunc_vgc__DOT__screen_addr__7__rr))) {
                                __Vfunc_vgc__DOT__screen_addr__7__rr 
                                    = (0x0000001fU 
                                       & ((IData)(__Vfunc_vgc__DOT__screen_addr__7__rr) 
                                          - (IData)(0x19U)));
                            }
                            __Vfunc_vgc__DOT__screen_addr__7__Vfuncout 
                                = (0x000007ffU & (((IData)(0x00000050U) 
                                                   * (IData)(__Vfunc_vgc__DOT__screen_addr__7__rr)) 
                                                  + (IData)(__Vfunc_vgc__DOT__screen_addr__7__col)));
                            __VdlyVal__vgc__DOT__char_ram__v2 
                                = vlSelfRef.vgc__DOT____Vlvbound_h2b480197__0;
                            __VdlyDim0__vgc__DOT__char_ram__v2 
                                = __Vfunc_vgc__DOT__screen_addr__7__Vfuncout;
                            __VdlySet__vgc__DOT__char_ram__v2 = 1U;
                        }
                        vlSelfRef.vgc__DOT____Vlvbound_h5d6d955d__0 
                            = vlSelfRef.vgc__DOT__fg_color;
                        if (VL_LIKELY(((0x07cfU >= 
                                        ([&]() {
                                                __Vfunc_vgc__DOT__screen_addr__8__row 
                                                    = vlSelfRef.vgc__DOT__cursor_y;
                                                __Vfunc_vgc__DOT__screen_addr__8__col 
                                                    = vlSelfRef.vgc__DOT__cursor_x;
                                                __Vfunc_vgc__DOT__screen_addr__8__rr 
                                                    = 
                                                    (0x0000001fU 
                                                     & ((IData)(__Vfunc_vgc__DOT__screen_addr__8__row) 
                                                        + (IData)(vlSelfRef.vgc__DOT__scroll_offset)));
                                                if (
                                                    (0x19U 
                                                     <= (IData)(__Vfunc_vgc__DOT__screen_addr__8__rr))) {
                                                    __Vfunc_vgc__DOT__screen_addr__8__rr 
                                                        = 
                                                        (0x0000001fU 
                                                         & ((IData)(__Vfunc_vgc__DOT__screen_addr__8__rr) 
                                                            - (IData)(0x19U)));
                                                }
                                                __Vfunc_vgc__DOT__screen_addr__8__Vfuncout 
                                                    = 
                                                    (0x000007ffU 
                                                     & (((IData)(0x00000050U) 
                                                         * (IData)(__Vfunc_vgc__DOT__screen_addr__8__rr)) 
                                                        + (IData)(__Vfunc_vgc__DOT__screen_addr__8__col)));
                                            }(), (IData)(__Vfunc_vgc__DOT__screen_addr__8__Vfuncout)))))) {
                            __Vfunc_vgc__DOT__screen_addr__9__row 
                                = vlSelfRef.vgc__DOT__cursor_y;
                            __Vfunc_vgc__DOT__screen_addr__9__col 
                                = vlSelfRef.vgc__DOT__cursor_x;
                            __Vfunc_vgc__DOT__screen_addr__9__rr 
                                = (0x0000001fU & ((IData)(__Vfunc_vgc__DOT__screen_addr__9__row) 
                                                  + (IData)(vlSelfRef.vgc__DOT__scroll_offset)));
                            if ((0x19U <= (IData)(__Vfunc_vgc__DOT__screen_addr__9__rr))) {
                                __Vfunc_vgc__DOT__screen_addr__9__rr 
                                    = (0x0000001fU 
                                       & ((IData)(__Vfunc_vgc__DOT__screen_addr__9__rr) 
                                          - (IData)(0x19U)));
                            }
                            __Vfunc_vgc__DOT__screen_addr__9__Vfuncout 
                                = (0x000007ffU & (((IData)(0x00000050U) 
                                                   * (IData)(__Vfunc_vgc__DOT__screen_addr__9__rr)) 
                                                  + (IData)(__Vfunc_vgc__DOT__screen_addr__9__col)));
                            __VdlyVal__vgc__DOT__color_ram__v1 
                                = vlSelfRef.vgc__DOT____Vlvbound_h5d6d955d__0;
                            __VdlyDim0__vgc__DOT__color_ram__v1 
                                = __Vfunc_vgc__DOT__screen_addr__9__Vfuncout;
                            __VdlySet__vgc__DOT__color_ram__v1 = 1U;
                        }
                        if ((0x4fU <= (IData)(vlSelfRef.vgc__DOT__cursor_x))) {
                            __Vdly__vgc__DOT__cursor_x = 0U;
                            if ((0x18U <= (IData)(vlSelfRef.vgc__DOT__cursor_y))) {
                                __Vdly__vgc__DOT__scroll_offset 
                                    = ((0x18U <= (IData)(vlSelfRef.vgc__DOT__scroll_offset))
                                        ? 0U : (0x0000001fU 
                                                & ((IData)(1U) 
                                                   + (IData)(vlSelfRef.vgc__DOT__scroll_offset))));
                                __Vdly__vgc__DOT__scroll_pending = 1U;
                                __Vdly__vgc__DOT__scroll_col = 0U;
                            } else {
                                __Vdly__vgc__DOT__cursor_y 
                                    = (0x0000001fU 
                                       & ((IData)(1U) 
                                          + (IData)(vlSelfRef.vgc__DOT__cursor_y)));
                            }
                        } else {
                            __Vdly__vgc__DOT__cursor_x 
                                = (0x0000007fU & ((IData)(1U) 
                                                  + (IData)(vlSelfRef.vgc__DOT__cursor_x)));
                        }
                    }
                } else if ((0x10U == (IData)(vlSelfRef.vgc__DOT__reg_offset))) {
                    if ((1U & (~ (IData)(vlSelfRef.vgc__DOT__cmd_busy)))) {
                        __Vdly__vgc__DOT__cmd_x = (
                                                   (0x00000300U 
                                                    & (vlSelfRef.vgc__DOT__regs[18U] 
                                                       << 8U)) 
                                                   | vlSelfRef.vgc__DOT__regs[17U]);
                        __Vdly__vgc__DOT__cmd_y = (
                                                   (0x00000300U 
                                                    & (vlSelfRef.vgc__DOT__regs[20U] 
                                                       << 8U)) 
                                                   | vlSelfRef.vgc__DOT__regs[19U]);
                        vlSelfRef.vgc__DOT__cmd_x2 
                            = ((0x00000300U & (vlSelfRef.vgc__DOT__regs[22U] 
                                               << 8U)) 
                               | vlSelfRef.vgc__DOT__regs[21U]);
                        vlSelfRef.vgc__DOT__cmd_y2 
                            = ((0x00000300U & (vlSelfRef.vgc__DOT__regs[24U] 
                                               << 8U)) 
                               | vlSelfRef.vgc__DOT__regs[23U]);
                        if ((1U & (~ ((IData)(vlSelfRef.vgc__DOT__cpu_wdata) 
                                      >> 7U)))) {
                            if ((1U & (~ ((IData)(vlSelfRef.vgc__DOT__cpu_wdata) 
                                          >> 6U)))) {
                                if ((1U & (~ ((IData)(vlSelfRef.vgc__DOT__cpu_wdata) 
                                              >> 5U)))) {
                                    if ((0x00000010U 
                                         & (IData)(vlSelfRef.vgc__DOT__cpu_wdata))) {
                                        if ((8U & (IData)(vlSelfRef.vgc__DOT__cpu_wdata))) {
                                            if ((4U 
                                                 & (IData)(vlSelfRef.vgc__DOT__cpu_wdata))) {
                                                if (
                                                    (2U 
                                                     & (IData)(vlSelfRef.vgc__DOT__cpu_wdata))) {
                                                    if (
                                                        (1U 
                                                         & (~ (IData)(vlSelfRef.vgc__DOT__cpu_wdata)))) {
                                                        vlSelfRef.vgc__DOT__copper_enabled = 0U;
                                                    }
                                                } else if (
                                                           (1U 
                                                            & (IData)(vlSelfRef.vgc__DOT__cpu_wdata))) {
                                                    vlSelfRef.vgc__DOT__copper_enabled = 1U;
                                                } else {
                                                    __Vdly__vgc__DOT__copper_count = 0U;
                                                    __Vdly__vgc__DOT__copper_index = 0U;
                                                }
                                            } else if (
                                                       (2U 
                                                        & (IData)(vlSelfRef.vgc__DOT__cpu_wdata))) {
                                                if (
                                                    (1U 
                                                     & (IData)(vlSelfRef.vgc__DOT__cpu_wdata))) {
                                                    if (
                                                        (0x0100U 
                                                         > (IData)(vlSelfRef.vgc__DOT__copper_count))) {
                                                        __VdlyVal__vgc__DOT__copper_pos__v0 
                                                            = 
                                                            (0x0001ffffU 
                                                             & (((IData)(0x00000140U) 
                                                                 * vlSelfRef.vgc__DOT__regs[19U]) 
                                                                + 
                                                                ((0x00000100U 
                                                                  & (vlSelfRef.vgc__DOT__regs[18U] 
                                                                     << 8U)) 
                                                                 | vlSelfRef.vgc__DOT__regs[17U])));
                                                        __VdlyDim0__vgc__DOT__copper_pos__v0 
                                                            = 
                                                            (0x000000ffU 
                                                             & (IData)(vlSelfRef.vgc__DOT__copper_count));
                                                        __VdlySet__vgc__DOT__copper_pos__v0 = 1U;
                                                        __VdlyVal__vgc__DOT__copper_reg__v0 
                                                            = vlSelfRef.vgc__DOT__regs[20U];
                                                        __VdlyDim0__vgc__DOT__copper_reg__v0 
                                                            = 
                                                            (0x000000ffU 
                                                             & (IData)(vlSelfRef.vgc__DOT__copper_count));
                                                        __VdlyVal__vgc__DOT__copper_val__v0 
                                                            = vlSelfRef.vgc__DOT__regs[22U];
                                                        __VdlyDim0__vgc__DOT__copper_val__v0 
                                                            = 
                                                            (0x000000ffU 
                                                             & (IData)(vlSelfRef.vgc__DOT__copper_count));
                                                        __Vdly__vgc__DOT__copper_count 
                                                            = 
                                                            (0x000001ffU 
                                                             & ((IData)(1U) 
                                                                + (IData)(vlSelfRef.vgc__DOT__copper_count)));
                                                    }
                                                }
                                            } else if (
                                                       (1U 
                                                        & (~ (IData)(vlSelfRef.vgc__DOT__cpu_wdata)))) {
                                                if (
                                                    (0x10U 
                                                     > vlSelfRef.vgc__DOT__regs[17U])) {
                                                    __VdlyVal__vgc__DOT__spr_pri__v0 
                                                        = 
                                                        (3U 
                                                         & vlSelfRef.vgc__DOT__regs[18U]);
                                                    __VdlyDim0__vgc__DOT__spr_pri__v0 
                                                        = 
                                                        (0x0000000fU 
                                                         & vlSelfRef.vgc__DOT__regs[17U]);
                                                    __VdlySet__vgc__DOT__spr_pri__v0 = 1U;
                                                }
                                            }
                                        } else if (
                                                   (4U 
                                                    & (IData)(vlSelfRef.vgc__DOT__cpu_wdata))) {
                                            if ((2U 
                                                 & (IData)(vlSelfRef.vgc__DOT__cpu_wdata))) {
                                                if (
                                                    (1U 
                                                     & (IData)(vlSelfRef.vgc__DOT__cpu_wdata))) {
                                                    if (
                                                        (0x10U 
                                                         > vlSelfRef.vgc__DOT__regs[17U])) {
                                                        __VdlyVal__vgc__DOT__spr_flip_h__v0 
                                                            = 
                                                            (1U 
                                                             & vlSelfRef.vgc__DOT__regs[18U]);
                                                        __VdlyDim0__vgc__DOT__spr_flip_h__v0 
                                                            = 
                                                            (0x0000000fU 
                                                             & vlSelfRef.vgc__DOT__regs[17U]);
                                                        __VdlySet__vgc__DOT__spr_flip_h__v0 = 1U;
                                                        __VdlyVal__vgc__DOT__spr_flip_v__v0 
                                                            = 
                                                            (1U 
                                                             & (vlSelfRef.vgc__DOT__regs[18U] 
                                                                >> 1U));
                                                        __VdlyDim0__vgc__DOT__spr_flip_v__v0 
                                                            = 
                                                            (0x0000000fU 
                                                             & vlSelfRef.vgc__DOT__regs[17U]);
                                                    }
                                                } else if (
                                                           (0x10U 
                                                            > vlSelfRef.vgc__DOT__regs[17U])) {
                                                    __VdlyDim0__vgc__DOT__spr_enable__v0 
                                                        = 
                                                        (0x0000000fU 
                                                         & vlSelfRef.vgc__DOT__regs[17U]);
                                                    __VdlySet__vgc__DOT__spr_enable__v0 = 1U;
                                                }
                                            } else if (
                                                       (1U 
                                                        & (IData)(vlSelfRef.vgc__DOT__cpu_wdata))) {
                                                if (
                                                    (0x10U 
                                                     > vlSelfRef.vgc__DOT__regs[17U])) {
                                                    __VdlyDim0__vgc__DOT__spr_enable__v1 
                                                        = 
                                                        (0x0000000fU 
                                                         & vlSelfRef.vgc__DOT__regs[17U]);
                                                    __VdlySet__vgc__DOT__spr_enable__v1 = 1U;
                                                }
                                            } else if (
                                                       (0x10U 
                                                        > vlSelfRef.vgc__DOT__regs[17U])) {
                                                __VdlyVal__vgc__DOT__spr_x__v0 
                                                    = 
                                                    ((0x00000100U 
                                                      & (vlSelfRef.vgc__DOT__regs[19U] 
                                                         << 8U)) 
                                                     | vlSelfRef.vgc__DOT__regs[18U]);
                                                __VdlyDim0__vgc__DOT__spr_x__v0 
                                                    = 
                                                    (0x0000000fU 
                                                     & vlSelfRef.vgc__DOT__regs[17U]);
                                                __VdlySet__vgc__DOT__spr_x__v0 = 1U;
                                                __VdlyVal__vgc__DOT__spr_y__v0 
                                                    = vlSelfRef.vgc__DOT__regs[20U];
                                                __VdlyDim0__vgc__DOT__spr_y__v0 
                                                    = 
                                                    (0x0000000fU 
                                                     & vlSelfRef.vgc__DOT__regs[17U]);
                                            }
                                        } else if (
                                                   (2U 
                                                    & (IData)(vlSelfRef.vgc__DOT__cpu_wdata))) {
                                            if ((1U 
                                                 & (~ (IData)(vlSelfRef.vgc__DOT__cpu_wdata)))) {
                                                if (
                                                    (0x10U 
                                                     > vlSelfRef.vgc__DOT__regs[17U])) {
                                                    __Vdly__vgc__DOT__cmd_cy = 0U;
                                                    __Vdly__vgc__DOT__cmd_x 
                                                        = 
                                                        (0x0000001eU 
                                                         & (vlSelfRef.vgc__DOT__regs[17U] 
                                                            << 1U));
                                                    __Vdly__vgc__DOT__cmd_cx = 0U;
                                                    __Vdly__vgc__DOT__cmd_busy = 1U;
                                                    vlSelfRef.vgc__DOT__cmd_op = 0x12U;
                                                }
                                            }
                                        } else if (
                                                   (1U 
                                                    & (IData)(vlSelfRef.vgc__DOT__cpu_wdata))) {
                                            if (((0x10U 
                                                  > vlSelfRef.vgc__DOT__regs[17U]) 
                                                 & (0x10U 
                                                    > vlSelfRef.vgc__DOT__regs[18U]))) {
                                                __VdlyVal__vgc__DOT__sprite_shapes__v1 
                                                    = vlSelfRef.vgc__DOT__regs[19U];
                                                __VdlyDim0__vgc__DOT__sprite_shapes__v1 
                                                    = 
                                                    ((0x00000780U 
                                                      & (vlSelfRef.vgc__DOT__regs[17U] 
                                                         << 7U)) 
                                                     | (0x00000078U 
                                                        & (vlSelfRef.vgc__DOT__regs[18U] 
                                                           << 3U)));
                                                __VdlySet__vgc__DOT__sprite_shapes__v1 = 1U;
                                                __VdlyVal__vgc__DOT__sprite_shapes__v2 
                                                    = vlSelfRef.vgc__DOT__regs[20U];
                                                __VdlyDim0__vgc__DOT__sprite_shapes__v2 
                                                    = 
                                                    (1U 
                                                     | ((0x00000780U 
                                                         & (vlSelfRef.vgc__DOT__regs[17U] 
                                                            << 7U)) 
                                                        | (0x00000078U 
                                                           & (vlSelfRef.vgc__DOT__regs[18U] 
                                                              << 3U))));
                                                __VdlyVal__vgc__DOT__sprite_shapes__v3 
                                                    = vlSelfRef.vgc__DOT__regs[21U];
                                                __VdlyDim0__vgc__DOT__sprite_shapes__v3 
                                                    = 
                                                    (2U 
                                                     | ((0x00000780U 
                                                         & (vlSelfRef.vgc__DOT__regs[17U] 
                                                            << 7U)) 
                                                        | (0x00000078U 
                                                           & (vlSelfRef.vgc__DOT__regs[18U] 
                                                              << 3U))));
                                                __VdlyVal__vgc__DOT__sprite_shapes__v4 
                                                    = vlSelfRef.vgc__DOT__regs[22U];
                                                __VdlyDim0__vgc__DOT__sprite_shapes__v4 
                                                    = 
                                                    (3U 
                                                     | ((0x00000780U 
                                                         & (vlSelfRef.vgc__DOT__regs[17U] 
                                                            << 7U)) 
                                                        | (0x00000078U 
                                                           & (vlSelfRef.vgc__DOT__regs[18U] 
                                                              << 3U))));
                                                __VdlyVal__vgc__DOT__sprite_shapes__v5 
                                                    = vlSelfRef.vgc__DOT__regs[23U];
                                                __VdlyDim0__vgc__DOT__sprite_shapes__v5 
                                                    = 
                                                    (4U 
                                                     | ((0x00000780U 
                                                         & (vlSelfRef.vgc__DOT__regs[17U] 
                                                            << 7U)) 
                                                        | (0x00000078U 
                                                           & (vlSelfRef.vgc__DOT__regs[18U] 
                                                              << 3U))));
                                                __VdlyVal__vgc__DOT__sprite_shapes__v6 
                                                    = vlSelfRef.vgc__DOT__regs[24U];
                                                __VdlyDim0__vgc__DOT__sprite_shapes__v6 
                                                    = 
                                                    (5U 
                                                     | ((0x00000780U 
                                                         & (vlSelfRef.vgc__DOT__regs[17U] 
                                                            << 7U)) 
                                                        | (0x00000078U 
                                                           & (vlSelfRef.vgc__DOT__regs[18U] 
                                                              << 3U))));
                                                __VdlyVal__vgc__DOT__sprite_shapes__v7 
                                                    = vlSelfRef.vgc__DOT__regs[25U];
                                                __VdlyDim0__vgc__DOT__sprite_shapes__v7 
                                                    = 
                                                    (6U 
                                                     | ((0x00000780U 
                                                         & (vlSelfRef.vgc__DOT__regs[17U] 
                                                            << 7U)) 
                                                        | (0x00000078U 
                                                           & (vlSelfRef.vgc__DOT__regs[18U] 
                                                              << 3U))));
                                                __VdlyVal__vgc__DOT__sprite_shapes__v8 
                                                    = vlSelfRef.vgc__DOT__regs[26U];
                                                __VdlyDim0__vgc__DOT__sprite_shapes__v8 
                                                    = 
                                                    (7U 
                                                     | ((0x00000780U 
                                                         & (vlSelfRef.vgc__DOT__regs[17U] 
                                                            << 7U)) 
                                                        | (0x00000078U 
                                                           & (vlSelfRef.vgc__DOT__regs[18U] 
                                                              << 3U))));
                                            }
                                        }
                                    } else if ((8U 
                                                & (IData)(vlSelfRef.vgc__DOT__cpu_wdata))) {
                                        if ((1U & (~ 
                                                   ((IData)(vlSelfRef.vgc__DOT__cpu_wdata) 
                                                    >> 2U)))) {
                                            if ((1U 
                                                 & (~ 
                                                    ((IData)(vlSelfRef.vgc__DOT__cpu_wdata) 
                                                     >> 1U)))) {
                                                if (
                                                    (1U 
                                                     & (~ (IData)(vlSelfRef.vgc__DOT__cpu_wdata)))) {
                                                    __Vdly__vgc__DOT__gfx_color 
                                                        = 
                                                        (0x0000000fU 
                                                         & vlSelfRef.vgc__DOT__regs[17U]);
                                                }
                                            }
                                        }
                                    } else if ((4U 
                                                & (IData)(vlSelfRef.vgc__DOT__cpu_wdata))) {
                                        if ((2U & (IData)(vlSelfRef.vgc__DOT__cpu_wdata))) {
                                            if ((1U 
                                                 & (IData)(vlSelfRef.vgc__DOT__cpu_wdata))) {
                                                __Vdly__vgc__DOT__cmd_cy = 0U;
                                                __Vdly__vgc__DOT__cmd_cx = 0U;
                                                __Vdly__vgc__DOT__cmd_busy = 1U;
                                                vlSelfRef.vgc__DOT__cmd_op = 7U;
                                            } else {
                                                __Vdly__vgc__DOT__cmd_cy 
                                                    = vlSelfRef.vgc__DOT__regs[19U];
                                                __Vdly__vgc__DOT__cmd_cx 
                                                    = 
                                                    ((0x00000100U 
                                                      & (vlSelfRef.vgc__DOT__regs[18U] 
                                                         << 8U)) 
                                                     | vlSelfRef.vgc__DOT__regs[17U]);
                                                __Vdly__vgc__DOT__cmd_busy = 1U;
                                                vlSelfRef.vgc__DOT__cmd_op 
                                                    = vlSelfRef.vgc__DOT__cpu_wdata;
                                            }
                                        } else if (
                                                   (1U 
                                                    & (IData)(vlSelfRef.vgc__DOT__cpu_wdata))) {
                                            __Vdly__vgc__DOT__cmd_cy 
                                                = vlSelfRef.vgc__DOT__regs[19U];
                                            __Vdly__vgc__DOT__cmd_cx 
                                                = (
                                                   (0x00000100U 
                                                    & (vlSelfRef.vgc__DOT__regs[18U] 
                                                       << 8U)) 
                                                   | vlSelfRef.vgc__DOT__regs[17U]);
                                            __Vdly__vgc__DOT__cmd_busy = 1U;
                                            vlSelfRef.vgc__DOT__cmd_op 
                                                = vlSelfRef.vgc__DOT__cpu_wdata;
                                        }
                                    } else if ((2U 
                                                & (IData)(vlSelfRef.vgc__DOT__cpu_wdata))) {
                                        if ((1U & (IData)(vlSelfRef.vgc__DOT__cpu_wdata))) {
                                            __Vdly__vgc__DOT__cmd_err 
                                                = (0x000003ffU 
                                                   & (((((0x00000100U 
                                                          & (vlSelfRef.vgc__DOT__regs[22U] 
                                                             << 8U)) 
                                                         | vlSelfRef.vgc__DOT__regs[21U]) 
                                                        >= 
                                                        ((0x00000100U 
                                                          & (vlSelfRef.vgc__DOT__regs[18U] 
                                                             << 8U)) 
                                                         | vlSelfRef.vgc__DOT__regs[17U]))
                                                        ? 
                                                       (((0x00000100U 
                                                          & (vlSelfRef.vgc__DOT__regs[22U] 
                                                             << 8U)) 
                                                         | vlSelfRef.vgc__DOT__regs[21U]) 
                                                        - 
                                                        ((0x00000100U 
                                                          & (vlSelfRef.vgc__DOT__regs[18U] 
                                                             << 8U)) 
                                                         | vlSelfRef.vgc__DOT__regs[17U]))
                                                        : 
                                                       (((0x00000100U 
                                                          & (vlSelfRef.vgc__DOT__regs[18U] 
                                                             << 8U)) 
                                                         | vlSelfRef.vgc__DOT__regs[17U]) 
                                                        - 
                                                        ((0x00000100U 
                                                          & (vlSelfRef.vgc__DOT__regs[22U] 
                                                             << 8U)) 
                                                         | vlSelfRef.vgc__DOT__regs[21U]))) 
                                                      + 
                                                      ((vlSelfRef.vgc__DOT__regs[23U] 
                                                        >= vlSelfRef.vgc__DOT__regs[19U])
                                                        ? 
                                                       (- 
                                                        (vlSelfRef.vgc__DOT__regs[23U] 
                                                         - vlSelfRef.vgc__DOT__regs[19U]))
                                                        : 
                                                       (vlSelfRef.vgc__DOT__regs[19U] 
                                                        - vlSelfRef.vgc__DOT__regs[23U]))));
                                            if ((((0x00000100U 
                                                   & (vlSelfRef.vgc__DOT__regs[22U] 
                                                      << 8U)) 
                                                  | vlSelfRef.vgc__DOT__regs[21U]) 
                                                 >= 
                                                 ((0x00000100U 
                                                   & (vlSelfRef.vgc__DOT__regs[18U] 
                                                      << 8U)) 
                                                  | vlSelfRef.vgc__DOT__regs[17U]))) {
                                                vlSelfRef.vgc__DOT__cmd_dx 
                                                    = 
                                                    (0x000003ffU 
                                                     & (((0x00000100U 
                                                          & (vlSelfRef.vgc__DOT__regs[22U] 
                                                             << 8U)) 
                                                         | vlSelfRef.vgc__DOT__regs[21U]) 
                                                        - 
                                                        ((0x00000100U 
                                                          & (vlSelfRef.vgc__DOT__regs[18U] 
                                                             << 8U)) 
                                                         | vlSelfRef.vgc__DOT__regs[17U])));
                                                vlSelfRef.vgc__DOT__cmd_sx = 1U;
                                            } else {
                                                vlSelfRef.vgc__DOT__cmd_dx 
                                                    = 
                                                    (0x000003ffU 
                                                     & (((0x00000100U 
                                                          & (vlSelfRef.vgc__DOT__regs[18U] 
                                                             << 8U)) 
                                                         | vlSelfRef.vgc__DOT__regs[17U]) 
                                                        - 
                                                        ((0x00000100U 
                                                          & (vlSelfRef.vgc__DOT__regs[22U] 
                                                             << 8U)) 
                                                         | vlSelfRef.vgc__DOT__regs[21U])));
                                                vlSelfRef.vgc__DOT__cmd_sx = 0x03ffU;
                                            }
                                            if ((vlSelfRef.vgc__DOT__regs[23U] 
                                                 >= vlSelfRef.vgc__DOT__regs[19U])) {
                                                vlSelfRef.vgc__DOT__cmd_dy 
                                                    = 
                                                    (0x000003ffU 
                                                     & (- 
                                                        (vlSelfRef.vgc__DOT__regs[23U] 
                                                         - vlSelfRef.vgc__DOT__regs[19U])));
                                                vlSelfRef.vgc__DOT__cmd_sy = 1U;
                                            } else {
                                                vlSelfRef.vgc__DOT__cmd_dy 
                                                    = 
                                                    (0x000003ffU 
                                                     & (vlSelfRef.vgc__DOT__regs[19U] 
                                                        - vlSelfRef.vgc__DOT__regs[23U]));
                                                vlSelfRef.vgc__DOT__cmd_sy = 0x03ffU;
                                            }
                                            __Vdly__vgc__DOT__cmd_busy = 1U;
                                            vlSelfRef.vgc__DOT__cmd_op = 3U;
                                        } else if (
                                                   ((0x0140U 
                                                     > 
                                                     ((0x00000100U 
                                                       & (vlSelfRef.vgc__DOT__regs[18U] 
                                                          << 8U)) 
                                                      | vlSelfRef.vgc__DOT__regs[17U])) 
                                                    & (0xc8U 
                                                       > vlSelfRef.vgc__DOT__regs[19U]))) {
                                            vlSelfRef.vgc__DOT____Vlvbound_hde0d773d__1 = 0U;
                                            if (VL_LIKELY((
                                                           (0xf9ffU 
                                                            >= 
                                                            (0x0000ffffU 
                                                             & (((IData)(0x00000140U) 
                                                                 * vlSelfRef.vgc__DOT__regs[19U]) 
                                                                + 
                                                                ((0x00000100U 
                                                                  & (vlSelfRef.vgc__DOT__regs[18U] 
                                                                     << 8U)) 
                                                                 | vlSelfRef.vgc__DOT__regs[17U]))))))) {
                                                __VdlyVal__vgc__DOT__gfx_ram__v4 
                                                    = vlSelfRef.vgc__DOT____Vlvbound_hde0d773d__1;
                                                __VdlyDim0__vgc__DOT__gfx_ram__v4 
                                                    = 
                                                    (0x0000ffffU 
                                                     & (((IData)(0x00000140U) 
                                                         * vlSelfRef.vgc__DOT__regs[19U]) 
                                                        + 
                                                        ((0x00000100U 
                                                          & (vlSelfRef.vgc__DOT__regs[18U] 
                                                             << 8U)) 
                                                         | vlSelfRef.vgc__DOT__regs[17U])));
                                                __VdlySet__vgc__DOT__gfx_ram__v4 = 1U;
                                            }
                                        }
                                    } else if ((1U 
                                                & (IData)(vlSelfRef.vgc__DOT__cpu_wdata))) {
                                        if (((0x0140U 
                                              > ((0x00000100U 
                                                  & (vlSelfRef.vgc__DOT__regs[18U] 
                                                     << 8U)) 
                                                 | vlSelfRef.vgc__DOT__regs[17U])) 
                                             & (0xc8U 
                                                > vlSelfRef.vgc__DOT__regs[19U]))) {
                                            vlSelfRef.vgc__DOT____Vlvbound_hde0d773d__0 
                                                = vlSelfRef.vgc__DOT__gfx_color;
                                            if (VL_LIKELY((
                                                           (0xf9ffU 
                                                            >= 
                                                            (0x0000ffffU 
                                                             & (((IData)(0x00000140U) 
                                                                 * vlSelfRef.vgc__DOT__regs[19U]) 
                                                                + 
                                                                ((0x00000100U 
                                                                  & (vlSelfRef.vgc__DOT__regs[18U] 
                                                                     << 8U)) 
                                                                 | vlSelfRef.vgc__DOT__regs[17U]))))))) {
                                                __VdlyVal__vgc__DOT__gfx_ram__v5 
                                                    = vlSelfRef.vgc__DOT____Vlvbound_hde0d773d__0;
                                                __VdlyDim0__vgc__DOT__gfx_ram__v5 
                                                    = 
                                                    (0x0000ffffU 
                                                     & (((IData)(0x00000140U) 
                                                         * vlSelfRef.vgc__DOT__regs[19U]) 
                                                        + 
                                                        ((0x00000100U 
                                                          & (vlSelfRef.vgc__DOT__regs[18U] 
                                                             << 8U)) 
                                                         | vlSelfRef.vgc__DOT__regs[17U])));
                                                __VdlySet__vgc__DOT__gfx_ram__v5 = 1U;
                                            }
                                        }
                                    }
                                }
                            }
                        }
                    }
                } else {
                    __VdlyVal__vgc__DOT__regs__v0 = vlSelfRef.vgc__DOT__cpu_wdata;
                    __VdlyDim0__vgc__DOT__regs__v0 
                        = vlSelfRef.vgc__DOT__reg_offset;
                    __VdlySet__vgc__DOT__regs__v0 = 1U;
                }
            }
            if (vlSelfRef.vgc__DOT__char_ram_sel) {
                vlSelfRef.vgc__DOT____Vlvbound_h87c37a99__0 
                    = vlSelfRef.vgc__DOT__cpu_wdata;
                if (VL_LIKELY(((0x07cfU >= (IData)(vlSelfRef.vgc__DOT__char_offset))))) {
                    __VdlyVal__vgc__DOT__char_ram__v3 
                        = vlSelfRef.vgc__DOT____Vlvbound_h87c37a99__0;
                    __VdlyDim0__vgc__DOT__char_ram__v3 
                        = vlSelfRef.vgc__DOT__char_offset;
                    __VdlySet__vgc__DOT__char_ram__v3 = 1U;
                }
            }
            if (vlSelfRef.vgc__DOT__color_ram_sel) {
                vlSelfRef.vgc__DOT____Vlvbound_hced7a248__0 
                    = vlSelfRef.vgc__DOT__cpu_wdata;
                if (VL_LIKELY(((0x07cfU >= (IData)(vlSelfRef.vgc__DOT__color_offset))))) {
                    __VdlyVal__vgc__DOT__color_ram__v2 
                        = vlSelfRef.vgc__DOT____Vlvbound_hced7a248__0;
                    __VdlyDim0__vgc__DOT__color_ram__v2 
                        = vlSelfRef.vgc__DOT__color_offset;
                    __VdlySet__vgc__DOT__color_ram__v2 = 1U;
                }
            }
            if (vlSelfRef.vgc__DOT__spr_reg_sel) {
                if ((4U & (IData)(vlSelfRef.vgc__DOT__spr_field))) {
                    if ((2U & (IData)(vlSelfRef.vgc__DOT__spr_field))) {
                        if ((1U & (~ (IData)(vlSelfRef.vgc__DOT__spr_field)))) {
                            __VdlyVal__vgc__DOT__spr_pri__v1 
                                = (3U & (IData)(vlSelfRef.vgc__DOT__cpu_wdata));
                            __VdlyDim0__vgc__DOT__spr_pri__v1 
                                = vlSelfRef.vgc__DOT__spr_index;
                            __VdlySet__vgc__DOT__spr_pri__v1 = 1U;
                        }
                    } else if ((1U & (IData)(vlSelfRef.vgc__DOT__spr_field))) {
                        __VdlyVal__vgc__DOT__spr_flip_h__v1 
                            = (1U & (IData)(vlSelfRef.vgc__DOT__cpu_wdata));
                        __VdlyDim0__vgc__DOT__spr_flip_h__v1 
                            = vlSelfRef.vgc__DOT__spr_index;
                        __VdlySet__vgc__DOT__spr_flip_h__v1 = 1U;
                        __VdlyVal__vgc__DOT__spr_flip_v__v1 
                            = (1U & ((IData)(vlSelfRef.vgc__DOT__cpu_wdata) 
                                     >> 1U));
                        __VdlyDim0__vgc__DOT__spr_flip_v__v1 
                            = vlSelfRef.vgc__DOT__spr_index;
                        __VdlyVal__vgc__DOT__spr_enable__v2 
                            = (1U & ((IData)(vlSelfRef.vgc__DOT__cpu_wdata) 
                                     >> 7U));
                        __VdlyDim0__vgc__DOT__spr_enable__v2 
                            = vlSelfRef.vgc__DOT__spr_index;
                    } else {
                        __VdlyVal__vgc__DOT__spr_shape__v0 
                            = (0x0000000fU & (IData)(vlSelfRef.vgc__DOT__cpu_wdata));
                        __VdlyDim0__vgc__DOT__spr_shape__v0 
                            = vlSelfRef.vgc__DOT__spr_index;
                        __VdlySet__vgc__DOT__spr_shape__v0 = 1U;
                    }
                } else if ((2U & (IData)(vlSelfRef.vgc__DOT__spr_field))) {
                    if ((1U & (~ (IData)(vlSelfRef.vgc__DOT__spr_field)))) {
                        __VdlyVal__vgc__DOT__spr_y__v1 
                            = vlSelfRef.vgc__DOT__cpu_wdata;
                        __VdlyDim0__vgc__DOT__spr_y__v1 
                            = vlSelfRef.vgc__DOT__spr_index;
                        __VdlySet__vgc__DOT__spr_y__v1 = 1U;
                    }
                } else if ((1U & (IData)(vlSelfRef.vgc__DOT__spr_field))) {
                    __VdlyVal__vgc__DOT__spr_x__v1 
                        = (1U & (IData)(vlSelfRef.vgc__DOT__cpu_wdata));
                    __VdlyDim0__vgc__DOT__spr_x__v1 
                        = vlSelfRef.vgc__DOT__spr_index;
                    __VdlySet__vgc__DOT__spr_x__v1 = 1U;
                } else {
                    __VdlyVal__vgc__DOT__spr_x__v2 
                        = vlSelfRef.vgc__DOT__cpu_wdata;
                    __VdlyDim0__vgc__DOT__spr_x__v2 
                        = vlSelfRef.vgc__DOT__spr_index;
                    __VdlySet__vgc__DOT__spr_x__v2 = 1U;
                }
            }
        }
    }
    vlSelfRef.vgc__DOT__visible = ((IData)(vlSelfRef.vgc__DOT__h_visible) 
                                   & (IData)(vlSelfRef.vgc__DOT__v_visible));
    vlSelfRef.vgc__DOT__scroll_pending = __Vdly__vgc__DOT__scroll_pending;
    vlSelfRef.vgc__DOT__scroll_col = __Vdly__vgc__DOT__scroll_col;
    vlSelfRef.vgc__DOT__cmd_err = __Vdly__vgc__DOT__cmd_err;
    vlSelfRef.vgc__DOT__cmd_x = __Vdly__vgc__DOT__cmd_x;
    vlSelfRef.vgc__DOT__cmd_y = __Vdly__vgc__DOT__cmd_y;
    vlSelfRef.vgc__DOT__cmd_cx = __Vdly__vgc__DOT__cmd_cx;
    vlSelfRef.vgc__DOT__cmd_cy = __Vdly__vgc__DOT__cmd_cy;
    vlSelfRef.vgc__DOT__copper_count = __Vdly__vgc__DOT__copper_count;
    vlSelfRef.vgc__DOT__copper_index = __Vdly__vgc__DOT__copper_index;
    if (__VdlySet__vgc__DOT__copper_pos__v0) {
        vlSelfRef.vgc__DOT__copper_pos[__VdlyDim0__vgc__DOT__copper_pos__v0] 
            = __VdlyVal__vgc__DOT__copper_pos__v0;
        vlSelfRef.vgc__DOT__copper_reg[__VdlyDim0__vgc__DOT__copper_reg__v0] 
            = __VdlyVal__vgc__DOT__copper_reg__v0;
        vlSelfRef.vgc__DOT__copper_val[__VdlyDim0__vgc__DOT__copper_val__v0] 
            = __VdlyVal__vgc__DOT__copper_val__v0;
    }
    vlSelfRef.vgc__DOT__gfx_color = __Vdly__vgc__DOT__gfx_color;
    vlSelfRef.vgc__DOT__cmd_busy = __Vdly__vgc__DOT__cmd_busy;
    vlSelfRef.vgc__DOT__fg_color = __Vdly__vgc__DOT__fg_color;
    if (__VdlySet__vgc__DOT__regs__v0) {
        vlSelfRef.vgc__DOT__regs[__VdlyDim0__vgc__DOT__regs__v0] 
            = __VdlyVal__vgc__DOT__regs__v0;
    }
    if (__VdlySet__vgc__DOT__regs__v1) {
        vlSelfRef.vgc__DOT__regs[__VdlyDim0__vgc__DOT__regs__v1] 
            = __VdlyVal__vgc__DOT__regs__v1;
    }
    vlSelfRef.vgc__DOT__cursor_x = __Vdly__vgc__DOT__cursor_x;
    vlSelfRef.vgc__DOT__cursor_y = __Vdly__vgc__DOT__cursor_y;
    if (__VdlySet__vgc__DOT__spr_shape__v0) {
        vlSelfRef.vgc__DOT__spr_shape[__VdlyDim0__vgc__DOT__spr_shape__v0] 
            = __VdlyVal__vgc__DOT__spr_shape__v0;
    }
    if (__VdlySet__vgc__DOT__spr_pri__v0) {
        vlSelfRef.vgc__DOT__spr_pri[__VdlyDim0__vgc__DOT__spr_pri__v0] 
            = __VdlyVal__vgc__DOT__spr_pri__v0;
    }
    if (__VdlySet__vgc__DOT__spr_pri__v1) {
        vlSelfRef.vgc__DOT__spr_pri[__VdlyDim0__vgc__DOT__spr_pri__v1] 
            = __VdlyVal__vgc__DOT__spr_pri__v1;
    }
    if (__VdlySet__vgc__DOT__gfx_ram__v0) {
        vlSelfRef.vgc__DOT__gfx_ram[__VdlyDim0__vgc__DOT__gfx_ram__v0] 
            = __VdlyVal__vgc__DOT__gfx_ram__v0;
    }
    if (__VdlySet__vgc__DOT__gfx_ram__v1) {
        vlSelfRef.vgc__DOT__gfx_ram[__VdlyDim0__vgc__DOT__gfx_ram__v1] 
            = __VdlyVal__vgc__DOT__gfx_ram__v1;
    }
    if (__VdlySet__vgc__DOT__gfx_ram__v2) {
        vlSelfRef.vgc__DOT__gfx_ram[__VdlyDim0__vgc__DOT__gfx_ram__v2] 
            = __VdlyVal__vgc__DOT__gfx_ram__v2;
    }
    if (__VdlySet__vgc__DOT__gfx_ram__v3) {
        vlSelfRef.vgc__DOT__gfx_ram[__VdlyDim0__vgc__DOT__gfx_ram__v3] 
            = __VdlyVal__vgc__DOT__gfx_ram__v3;
    }
    if (__VdlySet__vgc__DOT__gfx_ram__v4) {
        vlSelfRef.vgc__DOT__gfx_ram[__VdlyDim0__vgc__DOT__gfx_ram__v4] 
            = __VdlyVal__vgc__DOT__gfx_ram__v4;
    }
    if (__VdlySet__vgc__DOT__gfx_ram__v5) {
        vlSelfRef.vgc__DOT__gfx_ram[__VdlyDim0__vgc__DOT__gfx_ram__v5] 
            = __VdlyVal__vgc__DOT__gfx_ram__v5;
    }
    if (__VdlySet__vgc__DOT__sprite_shapes__v0) {
        vlSelfRef.vgc__DOT__sprite_shapes[__VdlyDim0__vgc__DOT__sprite_shapes__v0] = 0U;
    }
    if (__VdlySet__vgc__DOT__sprite_shapes__v1) {
        vlSelfRef.vgc__DOT__sprite_shapes[__VdlyDim0__vgc__DOT__sprite_shapes__v1] 
            = __VdlyVal__vgc__DOT__sprite_shapes__v1;
        vlSelfRef.vgc__DOT__sprite_shapes[__VdlyDim0__vgc__DOT__sprite_shapes__v2] 
            = __VdlyVal__vgc__DOT__sprite_shapes__v2;
        vlSelfRef.vgc__DOT__sprite_shapes[__VdlyDim0__vgc__DOT__sprite_shapes__v3] 
            = __VdlyVal__vgc__DOT__sprite_shapes__v3;
        vlSelfRef.vgc__DOT__sprite_shapes[__VdlyDim0__vgc__DOT__sprite_shapes__v4] 
            = __VdlyVal__vgc__DOT__sprite_shapes__v4;
        vlSelfRef.vgc__DOT__sprite_shapes[__VdlyDim0__vgc__DOT__sprite_shapes__v5] 
            = __VdlyVal__vgc__DOT__sprite_shapes__v5;
        vlSelfRef.vgc__DOT__sprite_shapes[__VdlyDim0__vgc__DOT__sprite_shapes__v6] 
            = __VdlyVal__vgc__DOT__sprite_shapes__v6;
        vlSelfRef.vgc__DOT__sprite_shapes[__VdlyDim0__vgc__DOT__sprite_shapes__v7] 
            = __VdlyVal__vgc__DOT__sprite_shapes__v7;
        vlSelfRef.vgc__DOT__sprite_shapes[__VdlyDim0__vgc__DOT__sprite_shapes__v8] 
            = __VdlyVal__vgc__DOT__sprite_shapes__v8;
    }
    if (__VdlySet__vgc__DOT__color_ram__v0) {
        vlSelfRef.vgc__DOT__color_ram[__VdlyDim0__vgc__DOT__color_ram__v0] 
            = __VdlyVal__vgc__DOT__color_ram__v0;
    }
    if (__VdlySet__vgc__DOT__color_ram__v1) {
        vlSelfRef.vgc__DOT__color_ram[__VdlyDim0__vgc__DOT__color_ram__v1] 
            = __VdlyVal__vgc__DOT__color_ram__v1;
    }
    if (__VdlySet__vgc__DOT__color_ram__v2) {
        vlSelfRef.vgc__DOT__color_ram[__VdlyDim0__vgc__DOT__color_ram__v2] 
            = __VdlyVal__vgc__DOT__color_ram__v2;
    }
    if (__VdlySet__vgc__DOT__char_ram__v0) {
        vlSelfRef.vgc__DOT__char_ram[__VdlyDim0__vgc__DOT__char_ram__v0] 
            = __VdlyVal__vgc__DOT__char_ram__v0;
    }
    if (__VdlySet__vgc__DOT__char_ram__v1) {
        vlSelfRef.vgc__DOT__char_ram[__VdlyDim0__vgc__DOT__char_ram__v1] 
            = __VdlyVal__vgc__DOT__char_ram__v1;
    }
    if (__VdlySet__vgc__DOT__char_ram__v2) {
        vlSelfRef.vgc__DOT__char_ram[__VdlyDim0__vgc__DOT__char_ram__v2] 
            = __VdlyVal__vgc__DOT__char_ram__v2;
    }
    if (__VdlySet__vgc__DOT__char_ram__v3) {
        vlSelfRef.vgc__DOT__char_ram[__VdlyDim0__vgc__DOT__char_ram__v3] 
            = __VdlyVal__vgc__DOT__char_ram__v3;
    }
    if (__VdlySet__vgc__DOT__spr_x__v0) {
        vlSelfRef.vgc__DOT__spr_y[__VdlyDim0__vgc__DOT__spr_y__v0] 
            = __VdlyVal__vgc__DOT__spr_y__v0;
        vlSelfRef.vgc__DOT__spr_x[__VdlyDim0__vgc__DOT__spr_x__v0] 
            = __VdlyVal__vgc__DOT__spr_x__v0;
    }
    if (__VdlySet__vgc__DOT__spr_y__v1) {
        vlSelfRef.vgc__DOT__spr_y[__VdlyDim0__vgc__DOT__spr_y__v1] 
            = __VdlyVal__vgc__DOT__spr_y__v1;
    }
    if (__VdlySet__vgc__DOT__spr_x__v1) {
        vlSelfRef.vgc__DOT__spr_x[__VdlyDim0__vgc__DOT__spr_x__v1] 
            = ((0x00ffU & vlSelfRef.vgc__DOT__spr_x
                [__VdlyDim0__vgc__DOT__spr_x__v1]) 
               | ((IData)(__VdlyVal__vgc__DOT__spr_x__v1) 
                  << 8U));
    }
    if (__VdlySet__vgc__DOT__spr_x__v2) {
        vlSelfRef.vgc__DOT__spr_x[__VdlyDim0__vgc__DOT__spr_x__v2] 
            = ((0x0100U & vlSelfRef.vgc__DOT__spr_x
                [__VdlyDim0__vgc__DOT__spr_x__v2]) 
               | (IData)(__VdlyVal__vgc__DOT__spr_x__v2));
    }
    if (__VdlySet__vgc__DOT__spr_flip_h__v0) {
        vlSelfRef.vgc__DOT__spr_flip_h[__VdlyDim0__vgc__DOT__spr_flip_h__v0] 
            = __VdlyVal__vgc__DOT__spr_flip_h__v0;
        vlSelfRef.vgc__DOT__spr_flip_v[__VdlyDim0__vgc__DOT__spr_flip_v__v0] 
            = __VdlyVal__vgc__DOT__spr_flip_v__v0;
    }
    if (__VdlySet__vgc__DOT__spr_enable__v0) {
        vlSelfRef.vgc__DOT__spr_enable[__VdlyDim0__vgc__DOT__spr_enable__v0] = 0U;
    }
    if (__VdlySet__vgc__DOT__spr_enable__v1) {
        vlSelfRef.vgc__DOT__spr_enable[__VdlyDim0__vgc__DOT__spr_enable__v1] = 1U;
    }
    if (__VdlySet__vgc__DOT__spr_flip_h__v1) {
        vlSelfRef.vgc__DOT__spr_flip_h[__VdlyDim0__vgc__DOT__spr_flip_h__v1] 
            = __VdlyVal__vgc__DOT__spr_flip_h__v1;
        vlSelfRef.vgc__DOT__spr_flip_v[__VdlyDim0__vgc__DOT__spr_flip_v__v1] 
            = __VdlyVal__vgc__DOT__spr_flip_v__v1;
        vlSelfRef.vgc__DOT__spr_enable[__VdlyDim0__vgc__DOT__spr_enable__v2] 
            = __VdlyVal__vgc__DOT__spr_enable__v2;
    }
    vlSelfRef.vgc__DOT__scroll_offset = __Vdly__vgc__DOT__scroll_offset;
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
    vlSelfRef.cpu_rdata = vlSelfRef.vgc__DOT__cpu_rdata;
    vlSelfRef.vgc__DOT__cursor_here = ((IData)(vlSelfRef.vgc__DOT__in_text_area) 
                                       & (((IData)(vlSelfRef.vgc__DOT__cursor_x) 
                                           == (IData)(vlSelfRef.vgc__DOT__text_col)) 
                                          & ((IData)(vlSelfRef.vgc__DOT__cursor_y) 
                                             == (IData)(vlSelfRef.vgc__DOT__text_row))));
    vlSelfRef.vgc__DOT__beam_pos = (0x0001ffffU & (
                                                   ((IData)(0x00000140U) 
                                                    * (IData)(vlSelfRef.vgc__DOT__gfx_y)) 
                                                   + (IData)(vlSelfRef.vgc__DOT__gfx_x)));
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
}

void Vvgc___024root___eval_nba(Vvgc___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vvgc___024root___eval_nba\n"); );
    Vvgc__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
    // Body
    if ((1ULL & vlSelfRef.__VnbaTriggered[0U])) {
        Vvgc___024root___nba_sequent__TOP__0(vlSelf);
    }
}

void Vvgc___024root___trigger_orInto__act_vec_vec(VlUnpacked<QData/*63:0*/, 1> &out, const VlUnpacked<QData/*63:0*/, 1> &in) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vvgc___024root___trigger_orInto__act_vec_vec\n"); );
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
VL_ATTR_COLD void Vvgc___024root___dump_triggers__act(const VlUnpacked<QData/*63:0*/, 1> &triggers, const std::string &tag);
#endif  // VL_DEBUG

bool Vvgc___024root___eval_phase__act(Vvgc___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vvgc___024root___eval_phase__act\n"); );
    Vvgc__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
    // Body
    Vvgc___024root___eval_triggers_vec__act(vlSelf);
#ifdef VL_DEBUG
    if (VL_UNLIKELY(vlSymsp->_vm_contextp__->debug())) {
        Vvgc___024root___dump_triggers__act(vlSelfRef.__VactTriggered, "act"s);
    }
#endif
    Vvgc___024root___trigger_orInto__act_vec_vec(vlSelfRef.__VnbaTriggered, vlSelfRef.__VactTriggered);
    return (0U);
}

void Vvgc___024root___trigger_clear__act(VlUnpacked<QData/*63:0*/, 1> &out) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vvgc___024root___trigger_clear__act\n"); );
    // Locals
    IData/*31:0*/ n;
    // Body
    n = 0U;
    do {
        out[n] = 0ULL;
        n = ((IData)(1U) + n);
    } while ((1U > n));
}

bool Vvgc___024root___eval_phase__nba(Vvgc___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vvgc___024root___eval_phase__nba\n"); );
    Vvgc__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
    // Locals
    CData/*0:0*/ __VnbaExecute;
    // Body
    __VnbaExecute = Vvgc___024root___trigger_anySet__act(vlSelfRef.__VnbaTriggered);
    if (__VnbaExecute) {
        Vvgc___024root___eval_nba(vlSelf);
        Vvgc___024root___trigger_clear__act(vlSelfRef.__VnbaTriggered);
    }
    return (__VnbaExecute);
}

void Vvgc___024root___eval(Vvgc___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vvgc___024root___eval\n"); );
    Vvgc__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
    // Locals
    IData/*31:0*/ __VicoIterCount;
    IData/*31:0*/ __VnbaIterCount;
    // Body
    __VicoIterCount = 0U;
    vlSelfRef.__VicoFirstIteration = 1U;
    do {
        if (VL_UNLIKELY(((0x00000064U < __VicoIterCount)))) {
#ifdef VL_DEBUG
            Vvgc___024root___dump_triggers__ico(vlSelfRef.__VicoTriggered, "ico"s);
#endif
            VL_FATAL_MT("rtl/vgc.sv", 8, "", "DIDNOTCONVERGE: Input combinational region did not converge after '--converge-limit' of 100 tries");
        }
        __VicoIterCount = ((IData)(1U) + __VicoIterCount);
        vlSelfRef.__VicoPhaseResult = Vvgc___024root___eval_phase__ico(vlSelf);
        vlSelfRef.__VicoFirstIteration = 0U;
    } while (vlSelfRef.__VicoPhaseResult);
    __VnbaIterCount = 0U;
    do {
        if (VL_UNLIKELY(((0x00000064U < __VnbaIterCount)))) {
#ifdef VL_DEBUG
            Vvgc___024root___dump_triggers__act(vlSelfRef.__VnbaTriggered, "nba"s);
#endif
            VL_FATAL_MT("rtl/vgc.sv", 8, "", "DIDNOTCONVERGE: NBA region did not converge after '--converge-limit' of 100 tries");
        }
        __VnbaIterCount = ((IData)(1U) + __VnbaIterCount);
        vlSelfRef.__VactIterCount = 0U;
        do {
            if (VL_UNLIKELY(((0x00000064U < vlSelfRef.__VactIterCount)))) {
#ifdef VL_DEBUG
                Vvgc___024root___dump_triggers__act(vlSelfRef.__VactTriggered, "act"s);
#endif
                VL_FATAL_MT("rtl/vgc.sv", 8, "", "DIDNOTCONVERGE: Active region did not converge after '--converge-limit' of 100 tries");
            }
            vlSelfRef.__VactIterCount = ((IData)(1U) 
                                         + vlSelfRef.__VactIterCount);
            vlSelfRef.__VactPhaseResult = Vvgc___024root___eval_phase__act(vlSelf);
        } while (vlSelfRef.__VactPhaseResult);
        vlSelfRef.__VnbaPhaseResult = Vvgc___024root___eval_phase__nba(vlSelf);
    } while (vlSelfRef.__VnbaPhaseResult);
}

#ifdef VL_DEBUG
void Vvgc___024root___eval_debug_assertions(Vvgc___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vvgc___024root___eval_debug_assertions\n"); );
    Vvgc__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
    // Body
    if (VL_UNLIKELY(((vlSelfRef.clk & 0xfeU)))) {
        Verilated::overWidthError("clk");
    }
    if (VL_UNLIKELY(((vlSelfRef.rst & 0xfeU)))) {
        Verilated::overWidthError("rst");
    }
    if (VL_UNLIKELY(((vlSelfRef.cpu_ce & 0xfeU)))) {
        Verilated::overWidthError("cpu_ce");
    }
    if (VL_UNLIKELY(((vlSelfRef.cpu_we & 0xfeU)))) {
        Verilated::overWidthError("cpu_we");
    }
    if (VL_UNLIKELY(((vlSelfRef.cpu_re & 0xfeU)))) {
        Verilated::overWidthError("cpu_re");
    }
    if (VL_UNLIKELY(((vlSelfRef.key_valid & 0xfeU)))) {
        Verilated::overWidthError("key_valid");
    }
}
#endif  // VL_DEBUG
