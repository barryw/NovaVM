// Verilated -*- C++ -*-
// DESCRIPTION: Verilator output: Design internal header
// See Vtest_tile_engine.h for the primary calling header

#ifndef VERILATED_VTEST_TILE_ENGINE___024ROOT_H_
#define VERILATED_VTEST_TILE_ENGINE___024ROOT_H_  // guard

#include "verilated.h"
#include "verilated_timing.h"


class Vtest_tile_engine__Syms;

class alignas(VL_CACHE_LINE_BYTES) Vtest_tile_engine___024root final {
  public:

    // DESIGN SPECIFIC STATE
    // Anonymous structures to workaround compiler member-count bugs
    struct {
        CData/*0:0*/ test_tile_engine__DOT__clk;
        CData/*0:0*/ test_tile_engine__DOT__rst;
        CData/*7:0*/ test_tile_engine__DOT__cpu_wdata;
        CData/*0:0*/ test_tile_engine__DOT__cpu_we;
        CData/*7:0*/ test_tile_engine__DOT__dma_data;
        CData/*7:0*/ test_tile_engine__DOT__blt_tile_wdata;
        CData/*0:0*/ test_tile_engine__DOT__blt_tile_we;
        CData/*0:0*/ test_tile_engine__DOT__blt_tile_re;
        CData/*7:0*/ test_tile_engine__DOT__pixel_y;
        CData/*0:0*/ test_tile_engine__DOT__pixel_valid;
        CData/*7:0*/ test_tile_engine__DOT__dut__DOT____Vlvbound_ha768cd16__0;
        CData/*7:0*/ test_tile_engine__DOT__dut__DOT____Vlvbound_h67f0dfac__0;
        CData/*7:0*/ test_tile_engine__DOT__dut__DOT____Vlvbound_hbbfa6b2d__0;
        CData/*0:0*/ test_tile_engine__DOT__dut__DOT__tile_size16;
        CData/*1:0*/ test_tile_engine__DOT__dut__DOT__mirror_mode;
        CData/*3:0*/ test_tile_engine__DOT__dut__DOT__trans_color;
        CData/*7:0*/ test_tile_engine__DOT__dut__DOT__peek_val;
        CData/*7:0*/ test_tile_engine__DOT__dut__DOT__peek_attr;
        CData/*7:0*/ test_tile_engine__DOT__dut__DOT__td_din_a;
        CData/*0:0*/ test_tile_engine__DOT__dut__DOT__td_we_a;
        CData/*7:0*/ test_tile_engine__DOT__dut__DOT__td_dout_a;
        CData/*7:0*/ test_tile_engine__DOT__dut__DOT__td_dout_b;
        CData/*7:0*/ test_tile_engine__DOT__dut__DOT__nt_din_a;
        CData/*0:0*/ test_tile_engine__DOT__dut__DOT__nt_we_a;
        CData/*7:0*/ test_tile_engine__DOT__dut__DOT__nt_dout_a;
        CData/*7:0*/ test_tile_engine__DOT__dut__DOT__nt_dout_b;
        CData/*7:0*/ test_tile_engine__DOT__dut__DOT__at_din_a;
        CData/*0:0*/ test_tile_engine__DOT__dut__DOT__at_we_a;
        CData/*7:0*/ test_tile_engine__DOT__dut__DOT__at_dout_a;
        CData/*7:0*/ test_tile_engine__DOT__dut__DOT__at_dout_b;
        CData/*7:0*/ test_tile_engine__DOT__dut__DOT__pr_addr_a;
        CData/*0:0*/ test_tile_engine__DOT__dut__DOT__pr_we_a;
        CData/*0:0*/ test_tile_engine__DOT__dut__DOT__tile_reg_sel;
        CData/*4:0*/ test_tile_engine__DOT__dut__DOT__dma_state;
        CData/*1:0*/ test_tile_engine__DOT__dut__DOT__dma_nt;
        CData/*5:0*/ test_tile_engine__DOT__dut__DOT__dma_col_row;
        CData/*4:0*/ test_tile_engine__DOT__dut__DOT__dma_col_idx;
        CData/*0:0*/ test_tile_engine__DOT__dut__DOT__dma_data_valid;
        CData/*7:0*/ test_tile_engine__DOT__dut__DOT__dma_pal_phase;
        CData/*7:0*/ test_tile_engine__DOT__dut__DOT__dma_pal_r;
        CData/*7:0*/ test_tile_engine__DOT__dut__DOT__dma_pal_g;
        CData/*3:0*/ test_tile_engine__DOT__dut__DOT__dma_pal_sub;
        CData/*3:0*/ test_tile_engine__DOT__dut__DOT__dma_pal_cidx;
        CData/*7:0*/ test_tile_engine__DOT__dut__DOT__dma_fill_val;
        CData/*0:0*/ test_tile_engine__DOT__dut__DOT__cmd_nt_we;
        CData/*0:0*/ test_tile_engine__DOT__dut__DOT__cmd_at_we;
        CData/*0:0*/ test_tile_engine__DOT__dut__DOT__cmd_pal_we;
        CData/*7:0*/ test_tile_engine__DOT__dut__DOT__cmd_nt_din;
        CData/*7:0*/ test_tile_engine__DOT__dut__DOT__cmd_at_din;
        CData/*7:0*/ test_tile_engine__DOT__dut__DOT__cmd_pal_addr_reg;
        CData/*0:0*/ test_tile_engine__DOT__dut__DOT__blt_tile_rd_pending;
        CData/*7:0*/ test_tile_engine__DOT__dut__DOT__blt_tile_rd_latch;
        CData/*3:0*/ test_tile_engine__DOT__dut__DOT__s1_pix_in_x;
        CData/*3:0*/ test_tile_engine__DOT__dut__DOT__s1_pix_in_y;
        CData/*0:0*/ test_tile_engine__DOT__dut__DOT__s1_pixel_valid;
        CData/*0:0*/ test_tile_engine__DOT__dut__DOT__s1_tile_size16;
        CData/*3:0*/ test_tile_engine__DOT__dut__DOT__s1_tile_w;
        CData/*3:0*/ test_tile_engine__DOT__dut__DOT__s1_tile_h;
        CData/*3:0*/ test_tile_engine__DOT__dut__DOT__s1_sample_x;
        CData/*3:0*/ test_tile_engine__DOT__dut__DOT__s1_sample_y;
        CData/*0:0*/ test_tile_engine__DOT__dut__DOT__s2_pixel_valid;
        CData/*3:0*/ test_tile_engine__DOT__dut__DOT__s2_sub_pal;
        CData/*0:0*/ test_tile_engine__DOT__dut__DOT__s2_sample_x_lsb;
        CData/*1:0*/ test_tile_engine__DOT__dut__DOT__s2_tile_pri;
    };
    struct {
        CData/*3:0*/ test_tile_engine__DOT__dut__DOT__s2_color_idx;
        CData/*0:0*/ test_tile_engine__DOT__dut__DOT__s3_pixel_valid;
        CData/*3:0*/ test_tile_engine__DOT__dut__DOT__s3_color_idx;
        CData/*1:0*/ test_tile_engine__DOT__dut__DOT__s3_tile_pri;
        CData/*3:0*/ test_tile_engine__DOT__dut__DOT__s3_trans_color;
        CData/*0:0*/ __VdfgRegularize_hebeb780c_0_2;
        CData/*0:0*/ __VstlFirstIteration;
        CData/*0:0*/ __VstlPhaseResult;
        CData/*0:0*/ __Vtrigprevexpr___TOP__test_tile_engine__DOT__clk__0;
        CData/*0:0*/ __VactPhaseResult;
        CData/*0:0*/ __VinactPhaseResult;
        CData/*0:0*/ __VnbaPhaseResult;
        SData/*15:0*/ test_tile_engine__DOT__cpu_addr;
        SData/*15:0*/ test_tile_engine__DOT__dma_addr;
        SData/*14:0*/ test_tile_engine__DOT__blt_tile_addr;
        SData/*8:0*/ test_tile_engine__DOT__pixel_x;
        SData/*15:0*/ test_tile_engine__DOT__dut__DOT__scroll_x;
        SData/*15:0*/ test_tile_engine__DOT__dut__DOT__scroll_y;
        SData/*14:0*/ test_tile_engine__DOT__dut__DOT__td_addr_a;
        SData/*11:0*/ test_tile_engine__DOT__dut__DOT__nt_addr_a;
        SData/*11:0*/ test_tile_engine__DOT__dut__DOT__nt_addr_b;
        SData/*11:0*/ test_tile_engine__DOT__dut__DOT__at_addr_a;
        SData/*11:0*/ test_tile_engine__DOT__dut__DOT__pr_din_a;
        SData/*11:0*/ test_tile_engine__DOT__dut__DOT__pr_dout_b;
        SData/*15:0*/ test_tile_engine__DOT__dut__DOT__dma_src;
        SData/*15:0*/ test_tile_engine__DOT__dut__DOT__dma_count;
        SData/*15:0*/ test_tile_engine__DOT__dut__DOT__dma_dst;
        SData/*11:0*/ test_tile_engine__DOT__dut__DOT__dma_fill_addr;
        SData/*11:0*/ test_tile_engine__DOT__dut__DOT__cmd_nt_addr;
        SData/*11:0*/ test_tile_engine__DOT__dut__DOT__cmd_at_addr;
        SData/*11:0*/ test_tile_engine__DOT__dut__DOT__cmd_pal_din;
        SData/*10:0*/ test_tile_engine__DOT__dut__DOT____VdfgRegularize_hb33eefcd_0_2;
        SData/*9:0*/ test_tile_engine__DOT__dut__DOT____VdfgRegularize_hb33eefcd_0_3;
        IData/*31:0*/ test_tile_engine__DOT__pass_count;
        IData/*31:0*/ test_tile_engine__DOT__fail_count;
        IData/*31:0*/ test_tile_engine__DOT__test_num;
        IData/*31:0*/ __VactIterCount;
        IData/*31:0*/ __VinactIterCount;
        IData/*31:0*/ __Vi;
        VlUnpacked<CData/*7:0*/, 65536> test_tile_engine__DOT__sim_ram;
        VlUnpacked<CData/*7:0*/, 32> test_tile_engine__DOT__dut__DOT__tregs;
        VlUnpacked<CData/*7:0*/, 25> test_tile_engine__DOT__dut__DOT__col_buffer;
        VlUnpacked<CData/*7:0*/, 32768> test_tile_engine__DOT__dut__DOT__tile_data_ram__DOT__mem;
        VlUnpacked<CData/*7:0*/, 4096> test_tile_engine__DOT__dut__DOT__nametable_ram__DOT__mem;
        VlUnpacked<CData/*7:0*/, 4096> test_tile_engine__DOT__dut__DOT__attr_table_ram__DOT__mem;
        VlUnpacked<SData/*11:0*/, 256> test_tile_engine__DOT__dut__DOT__pal_ram_inst__DOT__mem;
        VlUnpacked<QData/*63:0*/, 1> __VstlTriggered;
        VlUnpacked<QData/*63:0*/, 1> __VactTriggered;
        VlUnpacked<QData/*63:0*/, 1> __VactTriggeredAcc;
        VlUnpacked<QData/*63:0*/, 1> __VnbaTriggered;
    };
    std::string __Vtask_test_tile_engine__DOT__check__0__name;
    std::string __Vtask_test_tile_engine__DOT__check__1__name;
    std::string __Vtask_test_tile_engine__DOT__check__2__name;
    std::string __Vtask_test_tile_engine__DOT__check__3__name;
    std::string __Vtask_test_tile_engine__DOT__check__4__name;
    std::string __Vtask_test_tile_engine__DOT__check__5__name;
    std::string __Vtask_test_tile_engine__DOT__check__8__name;
    std::string __Vtask_test_tile_engine__DOT__check__9__name;
    std::string __Vtask_test_tile_engine__DOT__check__22__name;
    std::string __Vtask_test_tile_engine__DOT__check__33__name;
    std::string __Vtask_test_tile_engine__DOT__check__41__name;
    std::string __Vtask_test_tile_engine__DOT__check__42__name;
    std::string __Vtask_test_tile_engine__DOT__check__43__name;
    std::string __Vtask_test_tile_engine__DOT__check__53__name;
    std::string __Vtask_test_tile_engine__DOT__check__54__name;
    std::string __Vtask_test_tile_engine__DOT__check__55__name;
    std::string __Vtask_test_tile_engine__DOT__check__65__name;
    std::string __Vtask_test_tile_engine__DOT__check__66__name;
    std::string __Vtask_test_tile_engine__DOT__check__79__name;
    std::string __Vtask_test_tile_engine__DOT__check__88__name;
    std::string __Vtask_test_tile_engine__DOT__check__89__name;
    std::string __Vtask_test_tile_engine__DOT__check__120__name;
    std::string __Vtask_test_tile_engine__DOT__check__121__name;
    std::string __Vtask_test_tile_engine__DOT__check__123__name;
    std::string __Vtask_test_tile_engine__DOT__check__125__name;
    std::string __Vtask_test_tile_engine__DOT__check__129__name;
    std::string __Vtask_test_tile_engine__DOT__check__130__name;
    std::string __Vtask_test_tile_engine__DOT__check__131__name;
    std::string __Vtask_test_tile_engine__DOT__check__136__name;
    std::string __Vtask_test_tile_engine__DOT__check__137__name;
    std::string __Vtask_test_tile_engine__DOT__check__144__name;
    std::string __Vtask_test_tile_engine__DOT__check__145__name;
    std::string __Vtask_test_tile_engine__DOT__check__154__name;
    std::string __Vtask_test_tile_engine__DOT__check__155__name;
    std::string __Vtask_test_tile_engine__DOT__check__156__name;
    std::string __Vtask_test_tile_engine__DOT__check__164__name;
    std::string __Vtask_test_tile_engine__DOT__check__165__name;
    std::string __Vtask_test_tile_engine__DOT__check__166__name;
    std::string __Vtask_test_tile_engine__DOT__check__176__name;
    std::string __Vtask_test_tile_engine__DOT__check__177__name;
    std::string __Vtask_test_tile_engine__DOT__check__178__name;
    std::string __Vtask_test_tile_engine__DOT__check__190__name;
    std::string __Vtask_test_tile_engine__DOT__check__191__name;
    std::string __Vtask_test_tile_engine__DOT__check__192__name;
    std::string __Vtask_test_tile_engine__DOT__check__194__name;
    std::string __Vtask_test_tile_engine__DOT__check__196__name;
    std::string __Vtask_test_tile_engine__DOT__check__198__name;
    std::string __Vtask_test_tile_engine__DOT__check__202__name;
    std::string __Vtask_test_tile_engine__DOT__check__204__name;
    std::string __Vtask_test_tile_engine__DOT__check__206__name;
    std::string __Vtask_test_tile_engine__DOT__check__208__name;
    std::string __Vtask_test_tile_engine__DOT__check__212__name;
    std::string __Vtask_test_tile_engine__DOT__check__214__name;
    std::string __Vtask_test_tile_engine__DOT__check__229__name;
    std::string __Vtask_test_tile_engine__DOT__check__230__name;
    std::string __Vtask_test_tile_engine__DOT__check__233__name;
    std::string __Vtask_test_tile_engine__DOT__check__234__name;
    std::string __Vtask_test_tile_engine__DOT__check__235__name;
    VlDelayScheduler __VdlySched;
    VlTriggerScheduler __VtrigSched_h160f8687__0;

    // INTERNAL VARIABLES
    Vtest_tile_engine__Syms* vlSymsp;
    const char* vlNamep;

    // CONSTRUCTORS
    Vtest_tile_engine___024root(Vtest_tile_engine__Syms* symsp, const char* namep);
    ~Vtest_tile_engine___024root();
    VL_UNCOPYABLE(Vtest_tile_engine___024root);

    // INTERNAL METHODS
    void __Vconfigure(bool first);
};


#endif  // guard
