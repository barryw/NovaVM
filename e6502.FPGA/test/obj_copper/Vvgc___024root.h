// Verilated -*- C++ -*-
// DESCRIPTION: Verilator output: Design internal header
// See Vvgc.h for the primary calling header

#ifndef VERILATED_VVGC___024ROOT_H_
#define VERILATED_VVGC___024ROOT_H_  // guard

#include "verilated.h"


class Vvgc__Syms;

class alignas(VL_CACHE_LINE_BYTES) Vvgc___024root final {
  public:

    // DESIGN SPECIFIC STATE
    // Anonymous structures to workaround compiler member-count bugs
    struct {
        VL_IN8(clk,0,0);
        VL_IN8(rst,0,0);
        VL_IN8(cpu_ce,0,0);
        VL_IN8(cpu_wdata,7,0);
        VL_OUT8(cpu_rdata,7,0);
        VL_IN8(cpu_we,0,0);
        VL_IN8(cpu_re,0,0);
        VL_IN8(key_valid,0,0);
        VL_IN8(key_data,7,0);
        VL_OUT8(vid_r,3,0);
        VL_OUT8(vid_g,3,0);
        VL_OUT8(vid_b,3,0);
        VL_OUT8(vid_hsync,0,0);
        VL_OUT8(vid_vsync,0,0);
        VL_OUT8(vid_de,0,0);
        CData/*7:0*/ vgc__DOT____Vlvbound_hced7a248__0;
        CData/*7:0*/ vgc__DOT____Vlvbound_h87c37a99__0;
        CData/*3:0*/ vgc__DOT____Vlvbound_hde0d773d__1;
        CData/*3:0*/ vgc__DOT____Vlvbound_hde0d773d__0;
        CData/*7:0*/ vgc__DOT____Vlvbound_h5d6d955d__0;
        CData/*7:0*/ vgc__DOT____Vlvbound_h2b480197__0;
        CData/*7:0*/ vgc__DOT____Vlvbound_hb8130fd5__0;
        CData/*3:0*/ vgc__DOT____Vlvbound_h3f27e46b__2;
        CData/*3:0*/ vgc__DOT____Vlvbound_h3f27e46b__1;
        CData/*3:0*/ vgc__DOT____Vlvbound_h3f27e46b__0;
        CData/*3:0*/ vgc__DOT____Vlvbound_hc0a966b6__0;
        CData/*7:0*/ vgc__DOT____Vlvbound_hdecc4094__0;
        CData/*7:0*/ vgc__DOT____Vlvbound_h902d5c6a__0;
        CData/*3:0*/ vgc__DOT____Vlvbound_h652b6de2__0;
        CData/*7:0*/ vgc__DOT____Vlvbound_h45ddebd4__0;
        CData/*7:0*/ vgc__DOT____Vlvbound_h3318332a__0;
        CData/*0:0*/ vgc__DOT__clk;
        CData/*0:0*/ vgc__DOT__rst;
        CData/*0:0*/ vgc__DOT__cpu_ce;
        CData/*7:0*/ vgc__DOT__cpu_wdata;
        CData/*7:0*/ vgc__DOT__cpu_rdata;
        CData/*0:0*/ vgc__DOT__cpu_we;
        CData/*0:0*/ vgc__DOT__cpu_re;
        CData/*0:0*/ vgc__DOT__key_valid;
        CData/*7:0*/ vgc__DOT__key_data;
        CData/*3:0*/ vgc__DOT__vid_r;
        CData/*3:0*/ vgc__DOT__vid_g;
        CData/*3:0*/ vgc__DOT__vid_b;
        CData/*0:0*/ vgc__DOT__vid_hsync;
        CData/*0:0*/ vgc__DOT__vid_vsync;
        CData/*0:0*/ vgc__DOT__vid_de;
        CData/*0:0*/ vgc__DOT__h_sync_area;
        CData/*0:0*/ vgc__DOT__v_sync_area;
        CData/*0:0*/ vgc__DOT__h_visible;
        CData/*0:0*/ vgc__DOT__v_visible;
        CData/*0:0*/ vgc__DOT__visible;
        CData/*0:0*/ vgc__DOT__in_text_area;
        CData/*6:0*/ vgc__DOT__cursor_x;
        CData/*4:0*/ vgc__DOT__cursor_y;
        CData/*3:0*/ vgc__DOT__fg_color;
        CData/*3:0*/ vgc__DOT__bg_color;
        CData/*3:0*/ vgc__DOT__border_color;
        CData/*3:0*/ vgc__DOT__gfx_color;
        CData/*2:0*/ vgc__DOT__mode;
        CData/*7:0*/ vgc__DOT__char_in_reg;
        CData/*4:0*/ vgc__DOT__scroll_offset;
        CData/*0:0*/ vgc__DOT__scroll_pending;
        CData/*6:0*/ vgc__DOT__scroll_col;
        CData/*0:0*/ vgc__DOT__copper_enabled;
    };
    struct {
        CData/*0:0*/ vgc__DOT__cmd_busy;
        CData/*7:0*/ vgc__DOT__cmd_op;
        CData/*7:0*/ vgc__DOT__cmd_cy;
        CData/*0:0*/ vgc__DOT__vgc_reg_sel;
        CData/*0:0*/ vgc__DOT__spr_reg_sel;
        CData/*0:0*/ vgc__DOT__char_ram_sel;
        CData/*0:0*/ vgc__DOT__color_ram_sel;
        CData/*4:0*/ vgc__DOT__reg_offset;
        CData/*3:0*/ vgc__DOT__spr_index;
        CData/*2:0*/ vgc__DOT__spr_field;
        CData/*6:0*/ vgc__DOT__text_col;
        CData/*4:0*/ vgc__DOT__text_row;
        CData/*4:0*/ vgc__DOT__real_row;
        CData/*2:0*/ vgc__DOT__font_line;
        CData/*2:0*/ vgc__DOT__font_pixel;
        CData/*7:0*/ vgc__DOT__gfx_y;
        CData/*7:0*/ vgc__DOT__cur_char;
        CData/*3:0*/ vgc__DOT__cur_fg;
        CData/*3:0*/ vgc__DOT__cur_gfx;
        CData/*7:0*/ vgc__DOT__font_byte;
        CData/*0:0*/ vgc__DOT__pixel_on;
        CData/*3:0*/ vgc__DOT__spr_pixel;
        CData/*1:0*/ vgc__DOT__spr_pixel_pri;
        CData/*0:0*/ vgc__DOT__spr_pixel_hit;
        CData/*4:0*/ vgc__DOT__blink_count;
        CData/*0:0*/ vgc__DOT__cursor_blink;
        CData/*0:0*/ vgc__DOT__cursor_here;
        CData/*0:0*/ __VstlFirstIteration;
        CData/*0:0*/ __VstlPhaseResult;
        CData/*0:0*/ __VicoFirstIteration;
        CData/*0:0*/ __VicoPhaseResult;
        CData/*0:0*/ __Vtrigprevexpr___TOP__vgc__DOT__clk__0;
        CData/*0:0*/ __VactPhaseResult;
        CData/*0:0*/ __VnbaPhaseResult;
        VL_IN16(cpu_addr,15,0);
        SData/*15:0*/ vgc__DOT__cpu_addr;
        SData/*9:0*/ vgc__DOT__h_count;
        SData/*9:0*/ vgc__DOT__v_count;
        SData/*8:0*/ vgc__DOT__copper_count;
        SData/*8:0*/ vgc__DOT__copper_index;
        SData/*9:0*/ vgc__DOT__cmd_x;
        SData/*9:0*/ vgc__DOT__cmd_y;
        SData/*9:0*/ vgc__DOT__cmd_x2;
        SData/*9:0*/ vgc__DOT__cmd_y2;
        SData/*9:0*/ vgc__DOT__cmd_dx;
        SData/*9:0*/ vgc__DOT__cmd_dy;
        SData/*9:0*/ vgc__DOT__cmd_err;
        SData/*9:0*/ vgc__DOT__cmd_sx;
        SData/*9:0*/ vgc__DOT__cmd_sy;
        SData/*8:0*/ vgc__DOT__cmd_cx;
        SData/*10:0*/ vgc__DOT__char_offset;
        SData/*10:0*/ vgc__DOT__color_offset;
        SData/*9:0*/ vgc__DOT__text_line;
        SData/*8:0*/ vgc__DOT__gfx_x;
        SData/*11:0*/ vgc__DOT__text_pixel;
        SData/*11:0*/ vgc__DOT__gfx_pixel;
        SData/*11:0*/ vgc__DOT__pixel_color;
        IData/*16:0*/ vgc__DOT__beam_pos;
        IData/*31:0*/ vgc__DOT__unnamedblk1__DOT__i;
        IData/*31:0*/ vgc__DOT__unnamedblk2__DOT__i;
        IData/*31:0*/ vgc__DOT__unnamedblk3__DOT__i;
        IData/*31:0*/ vgc__DOT__unnamedblk4__DOT__i;
        IData/*31:0*/ vgc__DOT__unnamedblk5__DOT__i;
        IData/*31:0*/ vgc__DOT__unnamedblk6__DOT__i;
    };
    struct {
        IData/*31:0*/ vgc__DOT__unnamedblk7__DOT__s;
        IData/*31:0*/ vgc__DOT__unnamedblk8__DOT__s;
        IData/*31:0*/ __VactIterCount;
        VlUnpacked<SData/*11:0*/, 16> vgc__DOT__palette;
        VlUnpacked<CData/*7:0*/, 2000> vgc__DOT__char_ram;
        VlUnpacked<CData/*7:0*/, 2000> vgc__DOT__color_ram;
        VlUnpacked<CData/*7:0*/, 2048> vgc__DOT__font_rom;
        VlUnpacked<CData/*3:0*/, 64000> vgc__DOT__gfx_ram;
        VlUnpacked<CData/*7:0*/, 2048> vgc__DOT__sprite_shapes;
        VlUnpacked<SData/*8:0*/, 16> vgc__DOT__spr_x;
        VlUnpacked<CData/*7:0*/, 16> vgc__DOT__spr_y;
        VlUnpacked<CData/*0:0*/, 16> vgc__DOT__spr_enable;
        VlUnpacked<CData/*0:0*/, 16> vgc__DOT__spr_flip_h;
        VlUnpacked<CData/*0:0*/, 16> vgc__DOT__spr_flip_v;
        VlUnpacked<CData/*1:0*/, 16> vgc__DOT__spr_pri;
        VlUnpacked<CData/*3:0*/, 16> vgc__DOT__spr_shape;
        VlUnpacked<CData/*7:0*/, 32> vgc__DOT__regs;
        VlUnpacked<IData/*16:0*/, 256> vgc__DOT__copper_pos;
        VlUnpacked<CData/*7:0*/, 256> vgc__DOT__copper_reg;
        VlUnpacked<CData/*7:0*/, 256> vgc__DOT__copper_val;
        VlUnpacked<SData/*9:0*/, 16> vgc__DOT__spr_dx;
        VlUnpacked<SData/*9:0*/, 16> vgc__DOT__spr_dy;
        VlUnpacked<CData/*3:0*/, 16> vgc__DOT__spr_fx;
        VlUnpacked<CData/*3:0*/, 16> vgc__DOT__spr_fy;
        VlUnpacked<SData/*10:0*/, 16> vgc__DOT__spr_sa;
        VlUnpacked<CData/*3:0*/, 16> vgc__DOT__spr_pix;
        VlUnpacked<CData/*0:0*/, 16> vgc__DOT__spr_vis;
        VlUnpacked<QData/*63:0*/, 1> __VstlTriggered;
        VlUnpacked<QData/*63:0*/, 1> __VicoTriggered;
        VlUnpacked<QData/*63:0*/, 1> __VactTriggered;
        VlUnpacked<QData/*63:0*/, 1> __VnbaTriggered;
    };

    // INTERNAL VARIABLES
    Vvgc__Syms* vlSymsp;
    const char* vlNamep;

    // PARAMETERS
    static constexpr CData/*7:0*/ vgc__DOT__CMD_PLOT = 1U;
    static constexpr CData/*7:0*/ vgc__DOT__CMD_UNPLOT = 2U;
    static constexpr CData/*7:0*/ vgc__DOT__CMD_LINE = 3U;
    static constexpr CData/*7:0*/ vgc__DOT__CMD_CIRCLE = 4U;
    static constexpr CData/*7:0*/ vgc__DOT__CMD_RECT = 5U;
    static constexpr CData/*7:0*/ vgc__DOT__CMD_FILL = 6U;
    static constexpr CData/*7:0*/ vgc__DOT__CMD_GCLS = 7U;
    static constexpr CData/*7:0*/ vgc__DOT__CMD_GCOLOR = 8U;
    static constexpr CData/*7:0*/ vgc__DOT__CMD_PAINT = 9U;
    static constexpr CData/*7:0*/ vgc__DOT__CMD_SPRDEF = 0x10U;
    static constexpr CData/*7:0*/ vgc__DOT__CMD_SPRROW = 0x11U;
    static constexpr CData/*7:0*/ vgc__DOT__CMD_SPRCLR = 0x12U;
    static constexpr CData/*7:0*/ vgc__DOT__CMD_SPRCOPY = 0x13U;
    static constexpr CData/*7:0*/ vgc__DOT__CMD_SPRPOS = 0x14U;
    static constexpr CData/*7:0*/ vgc__DOT__CMD_SPRENA = 0x15U;
    static constexpr CData/*7:0*/ vgc__DOT__CMD_SPRDIS = 0x16U;
    static constexpr CData/*7:0*/ vgc__DOT__CMD_SPRFLIP = 0x17U;
    static constexpr CData/*7:0*/ vgc__DOT__CMD_SPRPRI = 0x18U;
    static constexpr SData/*15:0*/ vgc__DOT__VGC_BASE = 0xa000U;
    static constexpr SData/*15:0*/ vgc__DOT__VGC_REGS_END = 0xa01fU;
    static constexpr SData/*15:0*/ vgc__DOT__SPR_REG_BASE = 0xa040U;
    static constexpr SData/*15:0*/ vgc__DOT__SPR_REG_END = 0xa0bfU;
    static constexpr SData/*15:0*/ vgc__DOT__CHAR_RAM_BASE = 0xaa00U;
    static constexpr SData/*15:0*/ vgc__DOT__CHAR_RAM_END = 0xb1cfU;
    static constexpr SData/*15:0*/ vgc__DOT__COLOR_RAM_BASE = 0xb1d0U;
    static constexpr SData/*15:0*/ vgc__DOT__COLOR_RAM_END = 0xb99fU;
    static constexpr IData/*31:0*/ vgc__DOT__H_ACTIVE = 0x00000280U;
    static constexpr IData/*31:0*/ vgc__DOT__H_FRONT = 0x00000010U;
    static constexpr IData/*31:0*/ vgc__DOT__H_SYNC = 0x00000060U;
    static constexpr IData/*31:0*/ vgc__DOT__H_BACK = 0x00000030U;
    static constexpr IData/*31:0*/ vgc__DOT__H_TOTAL = 0x00000320U;
    static constexpr IData/*31:0*/ vgc__DOT__V_ACTIVE = 0x000001e0U;
    static constexpr IData/*31:0*/ vgc__DOT__V_FRONT = 0x0000000aU;
    static constexpr IData/*31:0*/ vgc__DOT__V_SYNC = 2U;
    static constexpr IData/*31:0*/ vgc__DOT__V_BACK = 0x00000021U;
    static constexpr IData/*31:0*/ vgc__DOT__V_TOTAL = 0x0000020dU;
    static constexpr IData/*31:0*/ vgc__DOT__COLS = 0x00000050U;
    static constexpr IData/*31:0*/ vgc__DOT__ROWS = 0x00000019U;
    static constexpr IData/*31:0*/ vgc__DOT__CHAR_W = 8U;
    static constexpr IData/*31:0*/ vgc__DOT__CHAR_H = 8U;
    static constexpr IData/*31:0*/ vgc__DOT__TEXT_H = 0x00000190U;
    static constexpr IData/*31:0*/ vgc__DOT__V_BORDER = 0x00000028U;
    static constexpr IData/*31:0*/ vgc__DOT__GFX_W = 0x00000140U;
    static constexpr IData/*31:0*/ vgc__DOT__GFX_H = 0x000000c8U;
    static constexpr IData/*31:0*/ vgc__DOT__GFX_SIZE = 0x0000fa00U;
    static constexpr IData/*31:0*/ vgc__DOT__REG_MODE = 0U;
    static constexpr IData/*31:0*/ vgc__DOT__REG_BGCOL = 1U;
    static constexpr IData/*31:0*/ vgc__DOT__REG_FGCOL = 2U;
    static constexpr IData/*31:0*/ vgc__DOT__REG_CURSORX = 3U;
    static constexpr IData/*31:0*/ vgc__DOT__REG_CURSORY = 4U;
    static constexpr IData/*31:0*/ vgc__DOT__REG_BORDER = 0x0000000dU;
    static constexpr IData/*31:0*/ vgc__DOT__REG_CHAROUT = 0x0000000eU;
    static constexpr IData/*31:0*/ vgc__DOT__REG_CHARIN = 0x0000000fU;
    static constexpr IData/*31:0*/ vgc__DOT__REG_CMD = 0x00000010U;
    static constexpr IData/*31:0*/ vgc__DOT__NUM_SPRITES = 0x00000010U;
    static constexpr IData/*31:0*/ vgc__DOT__SPR_W = 0x00000010U;
    static constexpr IData/*31:0*/ vgc__DOT__SPR_H = 0x00000010U;
    static constexpr IData/*31:0*/ vgc__DOT__COPPER_MAX = 0x00000100U;

    // CONSTRUCTORS
    Vvgc___024root(Vvgc__Syms* symsp, const char* namep);
    ~Vvgc___024root();
    VL_UNCOPYABLE(Vvgc___024root);

    // INTERNAL METHODS
    void __Vconfigure(bool first);
};


#endif  // guard
