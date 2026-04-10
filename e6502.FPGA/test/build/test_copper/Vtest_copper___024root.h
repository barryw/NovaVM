// Verilated -*- C++ -*-
// DESCRIPTION: Verilator output: Design internal header
// See Vtest_copper.h for the primary calling header

#ifndef VERILATED_VTEST_COPPER___024ROOT_H_
#define VERILATED_VTEST_COPPER___024ROOT_H_  // guard

#include "verilated.h"
#include "verilated_timing.h"


class Vtest_copper__Syms;

class alignas(VL_CACHE_LINE_BYTES) Vtest_copper___024root final {
  public:

    // DESIGN SPECIFIC STATE
    // Anonymous structures to workaround compiler member-count bugs
    struct {
        CData/*0:0*/ test_copper__DOT__clk;
        CData/*0:0*/ test_copper__DOT__rst;
        CData/*7:0*/ test_copper__DOT__cpu_wdata;
        CData/*0:0*/ test_copper__DOT__cpu_we;
        CData/*7:0*/ test_copper__DOT__dut__DOT____Vlvbound_hced7a248__0;
        CData/*7:0*/ test_copper__DOT__dut__DOT____Vlvbound_h87c37a99__0;
        CData/*3:0*/ test_copper__DOT__dut__DOT____Vlvbound_h45978041__0;
        CData/*7:0*/ test_copper__DOT__dut__DOT____Vlvbound_hd4129f91__0;
        CData/*7:0*/ test_copper__DOT__dut__DOT____Vlvbound_ha13cfb6b__0;
        CData/*3:0*/ test_copper__DOT__dut__DOT____Vlvbound_hde0d773d__1;
        CData/*3:0*/ test_copper__DOT__dut__DOT____Vlvbound_hde0d773d__0;
        CData/*7:0*/ test_copper__DOT__dut__DOT____Vlvbound_h5d6d955d__0;
        CData/*7:0*/ test_copper__DOT__dut__DOT____Vlvbound_h2b480197__0;
        CData/*7:0*/ test_copper__DOT__dut__DOT____Vlvbound_hb8130fd5__0;
        CData/*3:0*/ test_copper__DOT__dut__DOT____Vlvbound_he1a62a70__0;
        CData/*3:0*/ test_copper__DOT__dut__DOT____Vlvbound_h3f27e46b__3;
        CData/*3:0*/ test_copper__DOT__dut__DOT____Vlvbound_h9f4406d7__0;
        CData/*3:0*/ test_copper__DOT__dut__DOT____Vlvbound_haa426699__0;
        CData/*3:0*/ test_copper__DOT__dut__DOT____Vlvbound_hd5fa5e14__0;
        CData/*3:0*/ test_copper__DOT__dut__DOT____Vlvbound_hd5133de6__0;
        CData/*3:0*/ test_copper__DOT__dut__DOT____Vlvbound_hed509973__0;
        CData/*3:0*/ test_copper__DOT__dut__DOT____Vlvbound_hc84a7865__0;
        CData/*3:0*/ test_copper__DOT__dut__DOT____Vlvbound_haf1931bd__0;
        CData/*3:0*/ test_copper__DOT__dut__DOT____Vlvbound_hafb21497__0;
        CData/*7:0*/ test_copper__DOT__dut__DOT____Vlvbound_h1cc62cc7__0;
        CData/*7:0*/ test_copper__DOT__dut__DOT____Vlvbound_h6e0ff81d__0;
        CData/*3:0*/ test_copper__DOT__dut__DOT____Vlvbound_h3f27e46b__2;
        CData/*3:0*/ test_copper__DOT__dut__DOT____Vlvbound_h3f27e46b__1;
        CData/*3:0*/ test_copper__DOT__dut__DOT____Vlvbound_h3f27e46b__0;
        CData/*3:0*/ test_copper__DOT__dut__DOT____Vlvbound_hc0a966b6__0;
        CData/*7:0*/ test_copper__DOT__dut__DOT____Vlvbound_hdecc4094__0;
        CData/*7:0*/ test_copper__DOT__dut__DOT____Vlvbound_h902d5c6a__0;
        CData/*3:0*/ test_copper__DOT__dut__DOT____Vlvbound_h9d27345d__0;
        CData/*7:0*/ test_copper__DOT__dut__DOT____Vlvbound_hef6c2195__0;
        CData/*7:0*/ test_copper__DOT__dut__DOT____Vlvbound_h9d4d7d6f__0;
        CData/*3:0*/ test_copper__DOT__dut__DOT____Vlvbound_h652b6de2__0;
        CData/*7:0*/ test_copper__DOT__dut__DOT____Vlvbound_h45ddebd4__0;
        CData/*7:0*/ test_copper__DOT__dut__DOT____Vlvbound_h3318332a__0;
        CData/*0:0*/ test_copper__DOT__dut__DOT__in_text_area;
        CData/*6:0*/ test_copper__DOT__dut__DOT__cursor_x;
        CData/*4:0*/ test_copper__DOT__dut__DOT__cursor_y;
        CData/*3:0*/ test_copper__DOT__dut__DOT__fg_color;
        CData/*3:0*/ test_copper__DOT__dut__DOT__bg_color;
        CData/*3:0*/ test_copper__DOT__dut__DOT__gfx_color;
        CData/*4:0*/ test_copper__DOT__dut__DOT__scroll_offset;
        CData/*0:0*/ test_copper__DOT__dut__DOT__scroll_pending;
        CData/*6:0*/ test_copper__DOT__dut__DOT__scroll_col;
        CData/*3:0*/ test_copper__DOT__dut__DOT__fill_target;
        CData/*5:0*/ test_copper__DOT__dut__DOT__gt_char_idx;
        CData/*5:0*/ test_copper__DOT__dut__DOT__gt_char_len;
        CData/*2:0*/ test_copper__DOT__dut__DOT__gt_font_row;
        CData/*2:0*/ test_copper__DOT__dut__DOT__gt_font_col;
        CData/*7:0*/ test_copper__DOT__dut__DOT__gt_scale;
        CData/*7:0*/ test_copper__DOT__dut__DOT__gt_scale_x;
        CData/*7:0*/ test_copper__DOT__dut__DOT__gt_scale_y;
        CData/*7:0*/ test_copper__DOT__dut__DOT__gt_start_y;
        CData/*7:0*/ test_copper__DOT__dut__DOT__gt_cur_char;
        CData/*7:0*/ test_copper__DOT__dut__DOT__gt_font_byte;
        CData/*0:0*/ test_copper__DOT__dut__DOT__copper_enabled;
        CData/*6:0*/ test_copper__DOT__dut__DOT__copper_target_list;
        CData/*6:0*/ test_copper__DOT__dut__DOT__copper_active_list;
        CData/*6:0*/ test_copper__DOT__dut__DOT__copper_pending_list;
        CData/*0:0*/ test_copper__DOT__dut__DOT__copper_loading;
        CData/*6:0*/ test_copper__DOT__dut__DOT__copper_load_src;
    };
    struct {
        CData/*5:0*/ test_copper__DOT__dut__DOT__fio_name_len;
        CData/*0:0*/ test_copper__DOT__dut__DOT__cmd_busy;
        CData/*7:0*/ test_copper__DOT__dut__DOT__cmd_op;
        CData/*7:0*/ test_copper__DOT__dut__DOT__cmd_cy;
        CData/*0:0*/ test_copper__DOT__dut__DOT__vgc_reg_sel;
        CData/*0:0*/ test_copper__DOT__dut__DOT__char_ram_sel;
        CData/*0:0*/ test_copper__DOT__dut__DOT__color_ram_sel;
        CData/*0:0*/ test_copper__DOT__dut__DOT__tile_reg_sel;
        CData/*7:0*/ test_copper__DOT__dut__DOT__gfx_y;
        CData/*7:0*/ test_copper__DOT__dut__DOT__tile_inst__DOT____Vlvbound_h021f5fae__0;
        CData/*7:0*/ test_copper__DOT__dut__DOT__tile_inst__DOT____Vlvbound_h02fc01d7__0;
        CData/*7:0*/ test_copper__DOT__dut__DOT__tile_inst__DOT____Vlvbound_h67f0dfac__0;
        CData/*7:0*/ test_copper__DOT__dut__DOT__tile_inst__DOT____Vlvbound_ha057723b__0;
        CData/*7:0*/ test_copper__DOT__dut__DOT__tile_inst__DOT____Vlvbound_h8898988b__0;
        CData/*7:0*/ test_copper__DOT__dut__DOT__tile_inst__DOT____Vlvbound_ha3d87205__0;
        CData/*7:0*/ test_copper__DOT__dut__DOT__tile_inst__DOT____Vlvbound_h353a15af__0;
        CData/*7:0*/ test_copper__DOT__dut__DOT__tile_inst__DOT____Vlvbound_hf7076230__0;
        CData/*7:0*/ test_copper__DOT__dut__DOT__tile_inst__DOT____Vlvbound_hf26d05a2__0;
        CData/*7:0*/ test_copper__DOT__dut__DOT__tile_inst__DOT____Vlvbound_h8c784627__0;
        CData/*7:0*/ test_copper__DOT__dut__DOT__tile_inst__DOT____Vlvbound_h02aa0280__0;
        CData/*7:0*/ test_copper__DOT__dut__DOT__tile_inst__DOT____Vlvbound_h21da2949__0;
        CData/*7:0*/ test_copper__DOT__dut__DOT__tile_inst__DOT____Vlvbound_h17e6e432__0;
        CData/*7:0*/ test_copper__DOT__dut__DOT__tile_inst__DOT____Vlvbound_h3335cc4a__0;
        CData/*7:0*/ test_copper__DOT__dut__DOT__tile_inst__DOT____Vlvbound_ha65a9a44__0;
        CData/*7:0*/ test_copper__DOT__dut__DOT__tile_inst__DOT____Vlvbound_h7ed4573b__0;
        CData/*7:0*/ test_copper__DOT__dut__DOT__tile_inst__DOT____Vlvbound_h9afe376d__0;
        CData/*0:0*/ test_copper__DOT__dut__DOT__tile_inst__DOT__tile_size16;
        CData/*3:0*/ test_copper__DOT__dut__DOT__tile_inst__DOT__dma_state;
        CData/*1:0*/ test_copper__DOT__dut__DOT__tile_inst__DOT__dma_nt;
        CData/*5:0*/ test_copper__DOT__dut__DOT__tile_inst__DOT__dma_col_row;
        CData/*4:0*/ test_copper__DOT__dut__DOT__tile_inst__DOT__dma_col_idx;
        CData/*0:0*/ test_copper__DOT__dut__DOT__tile_inst__DOT__dma_data_valid;
        CData/*7:0*/ test_copper__DOT__dut__DOT__tile_inst__DOT__dma_pal_phase;
        CData/*7:0*/ test_copper__DOT__dut__DOT__tile_inst__DOT__dma_pal_r;
        CData/*7:0*/ test_copper__DOT__dut__DOT__tile_inst__DOT__dma_pal_g;
        CData/*3:0*/ test_copper__DOT__dut__DOT__tile_inst__DOT__dma_pal_sub;
        CData/*3:0*/ test_copper__DOT__dut__DOT__tile_inst__DOT__dma_pal_cidx;
        CData/*0:0*/ __VstlFirstIteration;
        CData/*0:0*/ __VstlPhaseResult;
        CData/*0:0*/ __Vtrigprevexpr___TOP__test_copper__DOT__clk__0;
        CData/*0:0*/ __VactPhaseResult;
        CData/*0:0*/ __VinactPhaseResult;
        CData/*0:0*/ __VnbaPhaseResult;
        SData/*15:0*/ test_copper__DOT__cpu_addr;
        SData/*9:0*/ test_copper__DOT__dut__DOT__h_count;
        SData/*9:0*/ test_copper__DOT__dut__DOT__v_count;
        SData/*9:0*/ test_copper__DOT__dut__DOT__fill_sp;
        SData/*8:0*/ test_copper__DOT__dut__DOT__gt_pen_x;
        SData/*8:0*/ test_copper__DOT__dut__DOT__copper_count;
        SData/*8:0*/ test_copper__DOT__dut__DOT__copper_index;
        SData/*8:0*/ test_copper__DOT__dut__DOT__copper_load_idx;
        SData/*9:0*/ test_copper__DOT__dut__DOT__cmd_x;
        SData/*9:0*/ test_copper__DOT__dut__DOT__cmd_y;
        SData/*9:0*/ test_copper__DOT__dut__DOT__cmd_x2;
        SData/*9:0*/ test_copper__DOT__dut__DOT__cmd_y2;
        SData/*9:0*/ test_copper__DOT__dut__DOT__cmd_dx;
        SData/*9:0*/ test_copper__DOT__dut__DOT__cmd_dy;
        SData/*9:0*/ test_copper__DOT__dut__DOT__cmd_err;
        SData/*9:0*/ test_copper__DOT__dut__DOT__cmd_sx;
        SData/*9:0*/ test_copper__DOT__dut__DOT__cmd_sy;
        SData/*8:0*/ test_copper__DOT__dut__DOT__cmd_cx;
        SData/*15:0*/ test_copper__DOT__dut__DOT__tile_inst__DOT__dma_src;
        SData/*15:0*/ test_copper__DOT__dut__DOT__tile_inst__DOT__dma_count;
        SData/*15:0*/ test_copper__DOT__dut__DOT__tile_inst__DOT__dma_dst;
    };
    struct {
        IData/*31:0*/ test_copper__DOT__pass_count;
        IData/*31:0*/ test_copper__DOT__fail_count;
        IData/*31:0*/ test_copper__DOT__test_num;
        IData/*31:0*/ test_copper__DOT__dut__DOT__unnamedblk14__DOT__i;
        IData/*31:0*/ test_copper__DOT__dut__DOT__unnamedblk15__DOT__i;
        IData/*31:0*/ test_copper__DOT__dut__DOT__unnamedblk16__DOT__i;
        IData/*31:0*/ test_copper__DOT__dut__DOT__unnamedblk17__DOT__i;
        IData/*31:0*/ test_copper__DOT__dut__DOT__tile_inst__DOT__unnamedblk6__DOT__i;
        IData/*31:0*/ __VactIterCount;
        IData/*31:0*/ __VinactIterCount;
        IData/*31:0*/ __Vi;
        VlUnpacked<CData/*7:0*/, 2000> test_copper__DOT__dut__DOT__char_ram;
        VlUnpacked<CData/*7:0*/, 2000> test_copper__DOT__dut__DOT__color_ram;
        VlUnpacked<CData/*7:0*/, 2048> test_copper__DOT__dut__DOT__font_rom;
        VlUnpacked<CData/*3:0*/, 64000> test_copper__DOT__dut__DOT__gfx_ram;
        VlUnpacked<CData/*7:0*/, 2048> test_copper__DOT__dut__DOT__sprite_shapes;
        VlUnpacked<CData/*7:0*/, 32> test_copper__DOT__dut__DOT__regs;
        VlUnpacked<IData/*16:0*/, 512> test_copper__DOT__dut__DOT__fill_stack;
        VlUnpacked<IData/*16:0*/, 32> test_copper__DOT__dut__DOT__copper_pos;
        VlUnpacked<CData/*7:0*/, 32> test_copper__DOT__dut__DOT__copper_reg;
        VlUnpacked<CData/*7:0*/, 32> test_copper__DOT__dut__DOT__copper_val;
        VlUnpacked<IData/*16:0*/, 4096> test_copper__DOT__dut__DOT__copper_list_pos;
        VlUnpacked<CData/*7:0*/, 4096> test_copper__DOT__dut__DOT__copper_list_reg;
        VlUnpacked<CData/*7:0*/, 4096> test_copper__DOT__dut__DOT__copper_list_val;
        VlUnpacked<SData/*8:0*/, 128> test_copper__DOT__dut__DOT__copper_list_count;
        VlUnpacked<CData/*7:0*/, 64> test_copper__DOT__dut__DOT__fio_name;
        VlUnpacked<CData/*7:0*/, 32> test_copper__DOT__dut__DOT__tile_inst__DOT__tregs;
        VlUnpacked<CData/*7:0*/, 25> test_copper__DOT__dut__DOT__tile_inst__DOT__col_buffer;
        VlUnpacked<CData/*7:0*/, 32768> test_copper__DOT__dut__DOT__tile_inst__DOT__tile_data;
        VlUnpacked<CData/*7:0*/, 4000> test_copper__DOT__dut__DOT__tile_inst__DOT__nametable;
        VlUnpacked<CData/*7:0*/, 4000> test_copper__DOT__dut__DOT__tile_inst__DOT__attr_table;
        VlUnpacked<QData/*63:0*/, 1> __VstlTriggered;
        VlUnpacked<QData/*63:0*/, 1> __VactTriggered;
        VlUnpacked<QData/*63:0*/, 1> __VactTriggeredAcc;
        VlUnpacked<QData/*63:0*/, 1> __VnbaTriggered;
    };
    VlNBACommitQueue<VlUnpacked<CData/*7:0*/, 2000>, false, CData/*7:0*/, 1> __VdlyCommitQueuetest_copper__DOT__dut__DOT__char_ram;
    VlNBACommitQueue<VlUnpacked<CData/*7:0*/, 2000>, false, CData/*7:0*/, 1> __VdlyCommitQueuetest_copper__DOT__dut__DOT__color_ram;
    VlNBACommitQueue<VlUnpacked<CData/*3:0*/, 64000>, false, CData/*3:0*/, 1> __VdlyCommitQueuetest_copper__DOT__dut__DOT__gfx_ram;
    VlNBACommitQueue<VlUnpacked<CData/*7:0*/, 2048>, false, CData/*7:0*/, 1> __VdlyCommitQueuetest_copper__DOT__dut__DOT__sprite_shapes;
    VlNBACommitQueue<VlUnpacked<SData/*8:0*/, 128>, false, SData/*8:0*/, 1> __VdlyCommitQueuetest_copper__DOT__dut__DOT__copper_list_count;
    VlNBACommitQueue<VlUnpacked<CData/*7:0*/, 4000>, false, CData/*7:0*/, 1> __VdlyCommitQueuetest_copper__DOT__dut__DOT__tile_inst__DOT__nametable;
    std::string __Vtask_test_copper__DOT__check__2__name;
    std::string __Vtask_test_copper__DOT__check__3__name;
    std::string __Vtask_test_copper__DOT__check__23__name;
    std::string __Vtask_test_copper__DOT__check__29__name;
    std::string __Vtask_test_copper__DOT__check__31__name;
    std::string __Vtask_test_copper__DOT__check__34__name;
    std::string __Vtask_test_copper__DOT__check__42__name;
    std::string __Vtask_test_copper__DOT__check__54__name;
    std::string __Vtask_test_copper__DOT__check__105__name;
    std::string __Vtask_test_copper__DOT__check__107__name;
    std::string __Vtask_test_copper__DOT__check__109__name;
    VlDelayScheduler __VdlySched;
    VlTriggerScheduler __VtrigSched_hbd4d80cf__0;

    // INTERNAL VARIABLES
    Vtest_copper__Syms* vlSymsp;
    const char* vlNamep;

    // CONSTRUCTORS
    Vtest_copper___024root(Vtest_copper__Syms* symsp, const char* namep);
    ~Vtest_copper___024root();
    VL_UNCOPYABLE(Vtest_copper___024root);

    // INTERNAL METHODS
    void __Vconfigure(bool first);
};


#endif  // guard
