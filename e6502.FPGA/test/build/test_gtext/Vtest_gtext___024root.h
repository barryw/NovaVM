// Verilated -*- C++ -*-
// DESCRIPTION: Verilator output: Design internal header
// See Vtest_gtext.h for the primary calling header

#ifndef VERILATED_VTEST_GTEXT___024ROOT_H_
#define VERILATED_VTEST_GTEXT___024ROOT_H_  // guard

#include "verilated.h"
#include "verilated_timing.h"


class Vtest_gtext__Syms;

class alignas(VL_CACHE_LINE_BYTES) Vtest_gtext___024root final {
  public:

    // DESIGN SPECIFIC STATE
    // Anonymous structures to workaround compiler member-count bugs
    struct {
        CData/*0:0*/ test_gtext__DOT__clk;
        CData/*0:0*/ test_gtext__DOT__rst;
        CData/*0:0*/ test_gtext__DOT__cpu_ce;
        CData/*7:0*/ test_gtext__DOT__cpu_wdata;
        CData/*0:0*/ test_gtext__DOT__cpu_we;
        CData/*0:0*/ test_gtext__DOT__cpu_re;
        CData/*7:0*/ test_gtext__DOT__unnamedblk4__DOT__unnamedblk5__DOT__unnamedblk6__DOT__fb;
        CData/*0:0*/ test_gtext__DOT__unnamedblk4__DOT__unnamedblk5__DOT__unnamedblk6__DOT__unnamedblk7__DOT__unnamedblk8__DOT__expected;
        CData/*7:0*/ test_gtext__DOT__unnamedblk15__DOT__fb;
        CData/*0:0*/ test_gtext__DOT__dut__DOT__in_text_area;
        CData/*7:0*/ test_gtext__DOT__dut__DOT__gfx_y;
        CData/*4:0*/ test_gtext__DOT__dut__DOT__scroll_offset;
        CData/*7:0*/ test_gtext__DOT__dut__DOT__char_a_dout;
        CData/*7:0*/ test_gtext__DOT__dut__DOT__color_a_dout;
        CData/*7:0*/ test_gtext__DOT__dut__DOT__font_a_dout;
        CData/*3:0*/ test_gtext__DOT__dut__DOT__gfx_a_din;
        CData/*0:0*/ test_gtext__DOT__dut__DOT__gfx_a_we;
        CData/*3:0*/ test_gtext__DOT__dut__DOT__gfx_a_dout;
        CData/*7:0*/ test_gtext__DOT__dut__DOT__spr_a_dout;
        CData/*0:0*/ test_gtext__DOT__dut__DOT__copper_enabled;
        CData/*6:0*/ test_gtext__DOT__dut__DOT__copper_target_list;
        CData/*6:0*/ test_gtext__DOT__dut__DOT__copper_active_list;
        CData/*6:0*/ test_gtext__DOT__dut__DOT__copper_pending_list;
        CData/*0:0*/ test_gtext__DOT__dut__DOT__copper_loading;
        CData/*6:0*/ test_gtext__DOT__dut__DOT__copper_load_src;
        CData/*0:0*/ test_gtext__DOT__dut__DOT__copper_fire;
        CData/*7:0*/ test_gtext__DOT__dut__DOT__copper_fire_reg;
        CData/*7:0*/ test_gtext__DOT__dut__DOT__copper_fire_val;
        CData/*6:0*/ test_gtext__DOT__dut__DOT__cursor_x;
        CData/*4:0*/ test_gtext__DOT__dut__DOT__cursor_y;
        CData/*3:0*/ test_gtext__DOT__dut__DOT__fg_color;
        CData/*3:0*/ test_gtext__DOT__dut__DOT__gfx_color;
        CData/*0:0*/ test_gtext__DOT__dut__DOT__scroll_pending;
        CData/*6:0*/ test_gtext__DOT__dut__DOT__scroll_col;
        CData/*5:0*/ test_gtext__DOT__dut__DOT__fio_name_len;
        CData/*0:0*/ test_gtext__DOT__dut__DOT__cmd_busy;
        CData/*7:0*/ test_gtext__DOT__dut__DOT__cmd_op;
        CData/*7:0*/ test_gtext__DOT__dut__DOT__cmd_cy;
        CData/*2:0*/ test_gtext__DOT__dut__DOT__sprrow_count;
        CData/*3:0*/ test_gtext__DOT__dut__DOT__sprrow_spr;
        CData/*3:0*/ test_gtext__DOT__dut__DOT__sprrow_row;
        CData/*0:0*/ test_gtext__DOT__dut__DOT__sprcopy_phase;
        CData/*0:0*/ test_gtext__DOT__dut__DOT__memread_pending;
        CData/*2:0*/ test_gtext__DOT__dut__DOT__memread_space;
        CData/*7:0*/ test_gtext__DOT__dut__DOT__cmd_char_din;
        CData/*0:0*/ test_gtext__DOT__dut__DOT__cmd_char_we;
        CData/*7:0*/ test_gtext__DOT__dut__DOT__cmd_color_din;
        CData/*0:0*/ test_gtext__DOT__dut__DOT__cmd_color_we;
        CData/*3:0*/ test_gtext__DOT__dut__DOT__cmd_gfx_din;
        CData/*0:0*/ test_gtext__DOT__dut__DOT__cmd_gfx_we;
        CData/*0:0*/ test_gtext__DOT__dut__DOT__cmd_gfx_re;
        CData/*7:0*/ test_gtext__DOT__dut__DOT__cmd_spr_din;
        CData/*0:0*/ test_gtext__DOT__dut__DOT__cmd_spr_we;
        CData/*0:0*/ test_gtext__DOT__dut__DOT__cmd_spr_re;
        CData/*0:0*/ test_gtext__DOT__dut__DOT__artist_cmd_valid;
        CData/*7:0*/ test_gtext__DOT__dut__DOT__artist_cmd_code;
        CData/*3:0*/ test_gtext__DOT__dut__DOT__artist_gfx_wdata;
        CData/*0:0*/ test_gtext__DOT__dut__DOT__artist_gfx_we;
        CData/*0:0*/ test_gtext__DOT__dut__DOT__artist_gfx_re;
        CData/*0:0*/ test_gtext__DOT__dut__DOT__artist_font_re;
        CData/*0:0*/ test_gtext__DOT__dut__DOT__artist_busy;
        CData/*0:0*/ test_gtext__DOT__dut__DOT__vgc_reg_sel;
        CData/*0:0*/ test_gtext__DOT__dut__DOT__spr_reg_sel;
        CData/*0:0*/ test_gtext__DOT__dut__DOT__char_ram_sel;
    };
    struct {
        CData/*0:0*/ test_gtext__DOT__dut__DOT__color_ram_sel;
        CData/*0:0*/ test_gtext__DOT__dut__DOT__tile_reg_sel;
        CData/*0:0*/ test_gtext__DOT__dut__DOT__dbg_char_sel;
        CData/*0:0*/ test_gtext__DOT__dut__DOT__dbg_color_sel;
        CData/*7:0*/ test_gtext__DOT__dut__DOT__tile_blt_rdata;
        CData/*7:0*/ test_gtext__DOT__dut__DOT__vgc_tile_wdata;
        CData/*0:0*/ test_gtext__DOT__dut__DOT__vgc_tile_we;
        CData/*0:0*/ test_gtext__DOT__dut__DOT__vgc_tile_re;
        CData/*0:0*/ test_gtext__DOT__dut__DOT____Vcellinp__tile_inst__cpu_we;
        CData/*7:0*/ test_gtext__DOT__dut__DOT__text_inst__DOT__char_mem__DOT____Vlvbound_hcad4ec09__0;
        CData/*7:0*/ test_gtext__DOT__dut__DOT__text_inst__DOT__char_mem__DOT____Vlvbound_h3482bd38__0;
        CData/*7:0*/ test_gtext__DOT__dut__DOT__text_inst__DOT__color_mem__DOT____Vlvbound_hcad4ec09__0;
        CData/*7:0*/ test_gtext__DOT__dut__DOT__text_inst__DOT__color_mem__DOT____Vlvbound_h3482bd38__0;
        CData/*3:0*/ test_gtext__DOT__dut__DOT__gfx_inst__DOT__gfx_mem__DOT____Vlvbound_h0a4ce44c__0;
        CData/*3:0*/ test_gtext__DOT__dut__DOT__gfx_inst__DOT__gfx_mem__DOT____Vlvbound_h75f189da__0;
        CData/*7:0*/ test_gtext__DOT__dut__DOT__artist_inst__DOT__op;
        CData/*7:0*/ test_gtext__DOT__dut__DOT__artist_inst__DOT__cy;
        CData/*3:0*/ test_gtext__DOT__dut__DOT__artist_inst__DOT__color;
        CData/*2:0*/ test_gtext__DOT__dut__DOT__artist_inst__DOT__octant_count;
        CData/*0:0*/ test_gtext__DOT__dut__DOT__artist_inst__DOT__fs_a_we;
        CData/*3:0*/ test_gtext__DOT__dut__DOT__artist_inst__DOT__fill_target;
        CData/*2:0*/ test_gtext__DOT__dut__DOT__artist_inst__DOT__paint_phase;
        CData/*0:0*/ test_gtext__DOT__dut__DOT__artist_inst__DOT__paint_init;
        CData/*1:0*/ test_gtext__DOT__dut__DOT__artist_inst__DOT__push_count;
        CData/*5:0*/ test_gtext__DOT__dut__DOT__artist_inst__DOT__gt_char_idx;
        CData/*5:0*/ test_gtext__DOT__dut__DOT__artist_inst__DOT__gt_len;
        CData/*2:0*/ test_gtext__DOT__dut__DOT__artist_inst__DOT__gt_font_row;
        CData/*2:0*/ test_gtext__DOT__dut__DOT__artist_inst__DOT__gt_font_col;
        CData/*7:0*/ test_gtext__DOT__dut__DOT__artist_inst__DOT__gt_scale;
        CData/*7:0*/ test_gtext__DOT__dut__DOT__artist_inst__DOT__gt_scale_x;
        CData/*7:0*/ test_gtext__DOT__dut__DOT__artist_inst__DOT__gt_scale_y;
        CData/*7:0*/ test_gtext__DOT__dut__DOT__artist_inst__DOT__gt_start_y;
        CData/*7:0*/ test_gtext__DOT__dut__DOT__artist_inst__DOT__gt_cur_char;
        CData/*7:0*/ test_gtext__DOT__dut__DOT__artist_inst__DOT__gt_font_byte;
        CData/*1:0*/ test_gtext__DOT__dut__DOT__artist_inst__DOT__gt_font_pending;
        CData/*7:0*/ test_gtext__DOT__dut__DOT__tile_inst__DOT____Vlvbound_ha768cd16__0;
        CData/*7:0*/ test_gtext__DOT__dut__DOT__tile_inst__DOT____Vlvbound_h67f0dfac__0;
        CData/*7:0*/ test_gtext__DOT__dut__DOT__tile_inst__DOT____Vlvbound_hbbfa6b2d__0;
        CData/*0:0*/ test_gtext__DOT__dut__DOT__tile_inst__DOT__tile_size16;
        CData/*7:0*/ test_gtext__DOT__dut__DOT__tile_inst__DOT__td_din_a;
        CData/*0:0*/ test_gtext__DOT__dut__DOT__tile_inst__DOT__td_we_a;
        CData/*7:0*/ test_gtext__DOT__dut__DOT__tile_inst__DOT__td_dout_a;
        CData/*7:0*/ test_gtext__DOT__dut__DOT__tile_inst__DOT__nt_din_a;
        CData/*0:0*/ test_gtext__DOT__dut__DOT__tile_inst__DOT__nt_we_a;
        CData/*7:0*/ test_gtext__DOT__dut__DOT__tile_inst__DOT__at_din_a;
        CData/*0:0*/ test_gtext__DOT__dut__DOT__tile_inst__DOT__at_we_a;
        CData/*4:0*/ test_gtext__DOT__dut__DOT__tile_inst__DOT__dma_state;
        CData/*1:0*/ test_gtext__DOT__dut__DOT__tile_inst__DOT__dma_nt;
        CData/*5:0*/ test_gtext__DOT__dut__DOT__tile_inst__DOT__dma_col_row;
        CData/*4:0*/ test_gtext__DOT__dut__DOT__tile_inst__DOT__dma_col_idx;
        CData/*0:0*/ test_gtext__DOT__dut__DOT__tile_inst__DOT__dma_data_valid;
        CData/*7:0*/ test_gtext__DOT__dut__DOT__tile_inst__DOT__dma_pal_phase;
        CData/*3:0*/ test_gtext__DOT__dut__DOT__tile_inst__DOT__dma_pal_cidx;
        CData/*7:0*/ test_gtext__DOT__dut__DOT__tile_inst__DOT__dma_fill_val;
        CData/*0:0*/ test_gtext__DOT__dut__DOT__tile_inst__DOT__cmd_nt_we;
        CData/*0:0*/ test_gtext__DOT__dut__DOT__tile_inst__DOT__cmd_at_we;
        CData/*7:0*/ test_gtext__DOT__dut__DOT__tile_inst__DOT__cmd_nt_din;
        CData/*7:0*/ test_gtext__DOT__dut__DOT__tile_inst__DOT__cmd_at_din;
        CData/*0:0*/ test_gtext__DOT__dut__DOT__tile_inst__DOT__blt_tile_rd_pending;
        CData/*7:0*/ test_gtext__DOT__dut__DOT__tile_inst__DOT__blt_tile_rd_latch;
        CData/*3:0*/ __Vfunc_test_gtext__DOT__gfx_pixel__93__Vfuncout;
        CData/*0:0*/ __VstlFirstIteration;
        CData/*0:0*/ __VstlPhaseResult;
        CData/*0:0*/ __Vtrigprevexpr___TOP__test_gtext__DOT__clk__0;
    };
    struct {
        CData/*0:0*/ __VactPhaseResult;
        CData/*0:0*/ __VinactPhaseResult;
        CData/*0:0*/ __VnbaPhaseResult;
        SData/*15:0*/ test_gtext__DOT__cpu_addr;
        SData/*15:0*/ test_gtext__DOT__dut__DOT__dbg_addr;
        SData/*9:0*/ test_gtext__DOT__dut__DOT__h_count;
        SData/*9:0*/ test_gtext__DOT__dut__DOT__v_count;
        SData/*10:0*/ test_gtext__DOT__dut__DOT__char_a_addr;
        SData/*10:0*/ test_gtext__DOT__dut__DOT__color_a_addr;
        SData/*10:0*/ test_gtext__DOT__dut__DOT__font_a_addr;
        SData/*15:0*/ test_gtext__DOT__dut__DOT__gfx_a_addr;
        SData/*8:0*/ test_gtext__DOT__dut__DOT____Vcellinp__artist_inst__cmd_x1;
        SData/*8:0*/ test_gtext__DOT__dut__DOT____Vcellinp__artist_inst__cmd_x0;
        SData/*10:0*/ test_gtext__DOT__dut__DOT__spr_a_addr;
        SData/*8:0*/ test_gtext__DOT__dut__DOT__copper_count;
        SData/*8:0*/ test_gtext__DOT__dut__DOT__copper_index;
        SData/*8:0*/ test_gtext__DOT__dut__DOT__copper_load_idx;
        SData/*9:0*/ test_gtext__DOT__dut__DOT__cmd_x;
        SData/*9:0*/ test_gtext__DOT__dut__DOT__cmd_y;
        SData/*8:0*/ test_gtext__DOT__dut__DOT__cmd_cx;
        SData/*10:0*/ test_gtext__DOT__dut__DOT__cmd_char_addr;
        SData/*10:0*/ test_gtext__DOT__dut__DOT__cmd_color_addr;
        SData/*15:0*/ test_gtext__DOT__dut__DOT__cmd_gfx_addr;
        SData/*10:0*/ test_gtext__DOT__dut__DOT__cmd_spr_addr;
        SData/*15:0*/ test_gtext__DOT__dut__DOT__artist_gfx_addr;
        SData/*15:0*/ test_gtext__DOT__dut__DOT__artist_gfx_raddr;
        SData/*10:0*/ test_gtext__DOT__dut__DOT__artist_font_addr;
        SData/*14:0*/ test_gtext__DOT__dut__DOT__vgc_tile_addr;
        SData/*9:0*/ test_gtext__DOT__dut__DOT__artist_inst__DOT__x;
        SData/*9:0*/ test_gtext__DOT__dut__DOT__artist_inst__DOT__y;
        SData/*9:0*/ test_gtext__DOT__dut__DOT__artist_inst__DOT__x2;
        SData/*9:0*/ test_gtext__DOT__dut__DOT__artist_inst__DOT__y2;
        SData/*9:0*/ test_gtext__DOT__dut__DOT__artist_inst__DOT__dx;
        SData/*9:0*/ test_gtext__DOT__dut__DOT__artist_inst__DOT__dy;
        SData/*9:0*/ test_gtext__DOT__dut__DOT__artist_inst__DOT__err;
        SData/*9:0*/ test_gtext__DOT__dut__DOT__artist_inst__DOT__sx;
        SData/*9:0*/ test_gtext__DOT__dut__DOT__artist_inst__DOT__sy;
        SData/*8:0*/ test_gtext__DOT__dut__DOT__artist_inst__DOT__cx;
        SData/*8:0*/ test_gtext__DOT__dut__DOT__artist_inst__DOT__fs_a_addr;
        SData/*8:0*/ test_gtext__DOT__dut__DOT__artist_inst__DOT__fs_b_addr;
        SData/*9:0*/ test_gtext__DOT__dut__DOT__artist_inst__DOT__fill_sp;
        SData/*8:0*/ test_gtext__DOT__dut__DOT__artist_inst__DOT__gt_pen_x;
        SData/*14:0*/ test_gtext__DOT__dut__DOT__tile_inst__DOT__td_addr_a;
        SData/*11:0*/ test_gtext__DOT__dut__DOT__tile_inst__DOT__nt_addr_a;
        SData/*11:0*/ test_gtext__DOT__dut__DOT__tile_inst__DOT__at_addr_a;
        SData/*15:0*/ test_gtext__DOT__dut__DOT__tile_inst__DOT__dma_src;
        SData/*15:0*/ test_gtext__DOT__dut__DOT__tile_inst__DOT__dma_count;
        SData/*15:0*/ test_gtext__DOT__dut__DOT__tile_inst__DOT__dma_dst;
        SData/*11:0*/ test_gtext__DOT__dut__DOT__tile_inst__DOT__dma_fill_addr;
        SData/*11:0*/ test_gtext__DOT__dut__DOT__tile_inst__DOT__cmd_nt_addr;
        SData/*11:0*/ test_gtext__DOT__dut__DOT__tile_inst__DOT__cmd_at_addr;
        IData/*31:0*/ test_gtext__DOT__pass_count;
        IData/*31:0*/ test_gtext__DOT__fail_count;
        IData/*31:0*/ test_gtext__DOT__test_num;
        IData/*31:0*/ test_gtext__DOT__unnamedblk1__DOT__pixel_count;
        IData/*31:0*/ test_gtext__DOT__unnamedblk4__DOT__match;
        IData/*31:0*/ test_gtext__DOT__unnamedblk9__DOT__pc;
        IData/*31:0*/ test_gtext__DOT__unnamedblk12__DOT__pc;
        IData/*31:0*/ test_gtext__DOT__unnamedblk15__DOT__ok;
        IData/*31:0*/ test_gtext__DOT__unnamedblk17__DOT__pc_a;
        IData/*31:0*/ test_gtext__DOT__unnamedblk17__DOT__pc_b;
        IData/*31:0*/ test_gtext__DOT__unnamedblk21__DOT__found;
        IData/*31:0*/ test_gtext__DOT__unnamedblk21__DOT__unnamedblk22__DOT__unnamedblk23__DOT__c;
        VlWide<16>/*511:0*/ test_gtext__DOT__dut__DOT__fio_name_flat;
    };
    struct {
        VlWide<17>/*543:0*/ test_gtext__DOT__dut__DOT__copper_pos_flat;
        VlWide<8>/*255:0*/ test_gtext__DOT__dut__DOT__copper_reg_flat;
        VlWide<8>/*255:0*/ test_gtext__DOT__dut__DOT__copper_val_flat;
        IData/*31:0*/ test_gtext__DOT__dut__DOT__unnamedblk14__DOT__i;
        IData/*16:0*/ test_gtext__DOT__dut__DOT__artist_inst__DOT__fs_a_din;
        IData/*16:0*/ test_gtext__DOT__dut__DOT__artist_inst__DOT__fs_b_dout;
        IData/*31:0*/ __Vfunc_test_gtext__DOT__gfx_pixel__93__x;
        IData/*31:0*/ __Vfunc_test_gtext__DOT__gfx_pixel__93__y;
        IData/*31:0*/ __VactIterCount;
        IData/*31:0*/ __VinactIterCount;
        IData/*31:0*/ __Vi;
        VlUnpacked<IData/*16:0*/, 32> test_gtext__DOT__dut__DOT__copper_pos;
        VlUnpacked<CData/*7:0*/, 32> test_gtext__DOT__dut__DOT__copper_reg;
        VlUnpacked<CData/*7:0*/, 32> test_gtext__DOT__dut__DOT__copper_val;
        VlUnpacked<IData/*16:0*/, 4096> test_gtext__DOT__dut__DOT__copper_list_pos;
        VlUnpacked<CData/*7:0*/, 4096> test_gtext__DOT__dut__DOT__copper_list_reg;
        VlUnpacked<CData/*7:0*/, 4096> test_gtext__DOT__dut__DOT__copper_list_val;
        VlUnpacked<SData/*8:0*/, 128> test_gtext__DOT__dut__DOT__copper_list_count;
        VlUnpacked<CData/*7:0*/, 32> test_gtext__DOT__dut__DOT__regs;
        VlUnpacked<CData/*7:0*/, 64> test_gtext__DOT__dut__DOT__fio_name;
        VlUnpacked<CData/*7:0*/, 8> test_gtext__DOT__dut__DOT__sprrow_data;
        VlUnpacked<CData/*7:0*/, 2000> test_gtext__DOT__dut__DOT__text_inst__DOT__char_mem__DOT__mem;
        VlUnpacked<CData/*7:0*/, 2000> test_gtext__DOT__dut__DOT__text_inst__DOT__color_mem__DOT__mem;
        VlUnpacked<CData/*7:0*/, 2048> test_gtext__DOT__dut__DOT__text_inst__DOT__font_mem__DOT__mem;
        VlUnpacked<CData/*3:0*/, 64000> test_gtext__DOT__dut__DOT__gfx_inst__DOT__gfx_mem__DOT__mem;
        VlUnpacked<IData/*16:0*/, 4> test_gtext__DOT__dut__DOT__artist_inst__DOT__push_neighbors;
        VlUnpacked<IData/*16:0*/, 512> test_gtext__DOT__dut__DOT__artist_inst__DOT__fill_stack_mem__DOT__mem;
        VlUnpacked<CData/*7:0*/, 2048> test_gtext__DOT__dut__DOT__sprite_inst__DOT__spr_mem__DOT__mem;
        VlUnpacked<IData/*16:0*/, 32> test_gtext__DOT__dut__DOT__copper_inst__DOT__copper_pos;
        VlUnpacked<CData/*7:0*/, 32> test_gtext__DOT__dut__DOT__copper_inst__DOT__copper_reg;
        VlUnpacked<CData/*7:0*/, 32> test_gtext__DOT__dut__DOT__copper_inst__DOT__copper_val;
        VlUnpacked<CData/*7:0*/, 32> test_gtext__DOT__dut__DOT__tile_inst__DOT__tregs;
        VlUnpacked<CData/*7:0*/, 25> test_gtext__DOT__dut__DOT__tile_inst__DOT__col_buffer;
        VlUnpacked<CData/*7:0*/, 32768> test_gtext__DOT__dut__DOT__tile_inst__DOT__tile_data_ram__DOT__mem;
        VlUnpacked<CData/*7:0*/, 4096> test_gtext__DOT__dut__DOT__tile_inst__DOT__nametable_ram__DOT__mem;
        VlUnpacked<CData/*7:0*/, 4096> test_gtext__DOT__dut__DOT__tile_inst__DOT__attr_table_ram__DOT__mem;
        VlUnpacked<QData/*63:0*/, 1> __VstlTriggered;
        VlUnpacked<QData/*63:0*/, 1> __VactTriggered;
        VlUnpacked<QData/*63:0*/, 1> __VactTriggeredAcc;
        VlUnpacked<QData/*63:0*/, 1> __VnbaTriggered;
    };
    VlNBACommitQueue<VlUnpacked<SData/*8:0*/, 128>, false, SData/*8:0*/, 1> __VdlyCommitQueuetest_gtext__DOT__dut__DOT__copper_list_count;
    std::string __Vtask_test_gtext__DOT__check__13__name;
    std::string __Vtask_test_gtext__DOT__set_fio_string__21__s;
    std::string __Vtask_test_gtext__DOT__check__40__name;
    std::string __Vtask_test_gtext__DOT__check__41__name;
    std::string __Vtask_test_gtext__DOT__check__44__name;
    std::string __Vtask_test_gtext__DOT__set_fio_string__48__s;
    std::string __Vtask_test_gtext__DOT__check__66__name;
    std::string __Vtask_test_gtext__DOT__check__68__name;
    std::string __Vtask_test_gtext__DOT__check__71__name;
    std::string __Vtask_test_gtext__DOT__set_fio_string__75__s;
    std::string __Vtask_test_gtext__DOT__check__94__name;
    std::string __Vtask_test_gtext__DOT__check__99__name;
    std::string __Vtask_test_gtext__DOT__set_fio_string__103__s;
    std::string __Vtask_test_gtext__DOT__check__123__name;
    std::string __Vtask_test_gtext__DOT__check__124__name;
    std::string __Vtask_test_gtext__DOT__set_fio_string__128__s;
    std::string __Vtask_test_gtext__DOT__check__146__name;
    std::string __Vtask_test_gtext__DOT__set_fio_string__154__s;
    std::string __Vtask_test_gtext__DOT__check__173__name;
    std::string __Vtask_test_gtext__DOT__check__175__name;
    VlDelayScheduler __VdlySched;
    VlTriggerScheduler __VtrigSched_h8248a36b__0;

    // INTERNAL VARIABLES
    Vtest_gtext__Syms* vlSymsp;
    const char* vlNamep;

    // CONSTRUCTORS
    Vtest_gtext___024root(Vtest_gtext__Syms* symsp, const char* namep);
    ~Vtest_gtext___024root();
    VL_UNCOPYABLE(Vtest_gtext___024root);

    // INTERNAL METHODS
    void __Vconfigure(bool first);
};


#endif  // guard
