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
        CData/*7:0*/ test_tile_engine__DOT__pixel_y;
        CData/*0:0*/ test_tile_engine__DOT__pixel_valid;
        CData/*7:0*/ test_tile_engine__DOT__dut__DOT____Vlvbound_h021f5fae__0;
        CData/*7:0*/ test_tile_engine__DOT__dut__DOT____Vlvbound_h02fc01d7__0;
        CData/*7:0*/ test_tile_engine__DOT__dut__DOT____Vlvbound_h67f0dfac__0;
        CData/*7:0*/ test_tile_engine__DOT__dut__DOT____Vlvbound_ha057723b__0;
        CData/*7:0*/ test_tile_engine__DOT__dut__DOT____Vlvbound_h8898988b__0;
        CData/*7:0*/ test_tile_engine__DOT__dut__DOT____Vlvbound_ha3d87205__0;
        CData/*7:0*/ test_tile_engine__DOT__dut__DOT____Vlvbound_h353a15af__0;
        CData/*7:0*/ test_tile_engine__DOT__dut__DOT____Vlvbound_hf7076230__0;
        CData/*7:0*/ test_tile_engine__DOT__dut__DOT____Vlvbound_hf26d05a2__0;
        CData/*7:0*/ test_tile_engine__DOT__dut__DOT____Vlvbound_h8c784627__0;
        CData/*7:0*/ test_tile_engine__DOT__dut__DOT____Vlvbound_h02aa0280__0;
        CData/*7:0*/ test_tile_engine__DOT__dut__DOT____Vlvbound_h21da2949__0;
        CData/*7:0*/ test_tile_engine__DOT__dut__DOT____Vlvbound_h17e6e432__0;
        CData/*7:0*/ test_tile_engine__DOT__dut__DOT____Vlvbound_h3335cc4a__0;
        CData/*7:0*/ test_tile_engine__DOT__dut__DOT____Vlvbound_ha65a9a44__0;
        CData/*7:0*/ test_tile_engine__DOT__dut__DOT____Vlvbound_h7ed4573b__0;
        CData/*7:0*/ test_tile_engine__DOT__dut__DOT____Vlvbound_h9afe376d__0;
        CData/*0:0*/ test_tile_engine__DOT__dut__DOT__tile_size16;
        CData/*1:0*/ test_tile_engine__DOT__dut__DOT__mirror_mode;
        CData/*3:0*/ test_tile_engine__DOT__dut__DOT__trans_color;
        CData/*7:0*/ test_tile_engine__DOT__dut__DOT__peek_val;
        CData/*7:0*/ test_tile_engine__DOT__dut__DOT__peek_attr;
        CData/*0:0*/ test_tile_engine__DOT__dut__DOT__tile_reg_sel;
        CData/*3:0*/ test_tile_engine__DOT__dut__DOT__dma_state;
        CData/*1:0*/ test_tile_engine__DOT__dut__DOT__dma_nt;
        CData/*5:0*/ test_tile_engine__DOT__dut__DOT__dma_col_row;
        CData/*4:0*/ test_tile_engine__DOT__dut__DOT__dma_col_idx;
        CData/*0:0*/ test_tile_engine__DOT__dut__DOT__dma_data_valid;
        CData/*7:0*/ test_tile_engine__DOT__dut__DOT__dma_pal_phase;
        CData/*7:0*/ test_tile_engine__DOT__dut__DOT__dma_pal_r;
        CData/*7:0*/ test_tile_engine__DOT__dut__DOT__dma_pal_g;
        CData/*3:0*/ test_tile_engine__DOT__dut__DOT__dma_pal_sub;
        CData/*3:0*/ test_tile_engine__DOT__dut__DOT__dma_pal_cidx;
        CData/*7:0*/ test_tile_engine__DOT__dut__DOT__cur_attr;
        CData/*3:0*/ test_tile_engine__DOT__dut__DOT__color_idx;
        CData/*0:0*/ __VdfgRegularize_hebeb780c_0_2;
        CData/*0:0*/ __VstlFirstIteration;
        CData/*0:0*/ __VstlPhaseResult;
        CData/*0:0*/ __Vtrigprevexpr___TOP__test_tile_engine__DOT__clk__0;
        CData/*0:0*/ __VactPhaseResult;
        CData/*0:0*/ __VinactPhaseResult;
        CData/*0:0*/ __VnbaPhaseResult;
        SData/*15:0*/ test_tile_engine__DOT__cpu_addr;
        SData/*15:0*/ test_tile_engine__DOT__dma_addr;
        SData/*8:0*/ test_tile_engine__DOT__pixel_x;
        SData/*15:0*/ test_tile_engine__DOT__dut__DOT__scroll_x;
        SData/*15:0*/ test_tile_engine__DOT__dut__DOT__scroll_y;
        SData/*15:0*/ test_tile_engine__DOT__dut__DOT__dma_src;
        SData/*15:0*/ test_tile_engine__DOT__dut__DOT__dma_count;
        SData/*15:0*/ test_tile_engine__DOT__dut__DOT__dma_dst;
        IData/*31:0*/ test_tile_engine__DOT__pass_count;
        IData/*31:0*/ test_tile_engine__DOT__fail_count;
        IData/*31:0*/ test_tile_engine__DOT__test_num;
        IData/*31:0*/ test_tile_engine__DOT__dut__DOT__unnamedblk6__DOT__i;
        IData/*31:0*/ __VactIterCount;
        IData/*31:0*/ __VinactIterCount;
        IData/*31:0*/ __Vi;
    };
    struct {
        VlUnpacked<CData/*7:0*/, 65536> test_tile_engine__DOT__sim_ram;
        VlUnpacked<CData/*7:0*/, 32> test_tile_engine__DOT__dut__DOT__tregs;
        VlUnpacked<CData/*7:0*/, 25> test_tile_engine__DOT__dut__DOT__col_buffer;
        VlUnpacked<CData/*7:0*/, 32768> test_tile_engine__DOT__dut__DOT__tile_data;
        VlUnpacked<CData/*7:0*/, 4000> test_tile_engine__DOT__dut__DOT__nametable;
        VlUnpacked<CData/*7:0*/, 4000> test_tile_engine__DOT__dut__DOT__attr_table;
        VlUnpacked<SData/*11:0*/, 256> test_tile_engine__DOT__dut__DOT__pal_ram;
        VlUnpacked<QData/*63:0*/, 1> __VstlTriggered;
        VlUnpacked<QData/*63:0*/, 1> __VactTriggered;
        VlUnpacked<QData/*63:0*/, 1> __VactTriggeredAcc;
        VlUnpacked<QData/*63:0*/, 1> __VnbaTriggered;
    };
    VlNBACommitQueue<VlUnpacked<CData/*7:0*/, 4000>, false, CData/*7:0*/, 1> __VdlyCommitQueuetest_tile_engine__DOT__dut__DOT__nametable;
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
    std::string __Vtask_test_tile_engine__DOT__check__40__name;
    std::string __Vtask_test_tile_engine__DOT__check__41__name;
    std::string __Vtask_test_tile_engine__DOT__check__42__name;
    std::string __Vtask_test_tile_engine__DOT__check__52__name;
    std::string __Vtask_test_tile_engine__DOT__check__53__name;
    std::string __Vtask_test_tile_engine__DOT__check__54__name;
    std::string __Vtask_test_tile_engine__DOT__check__63__name;
    std::string __Vtask_test_tile_engine__DOT__check__64__name;
    std::string __Vtask_test_tile_engine__DOT__check__77__name;
    std::string __Vtask_test_tile_engine__DOT__check__86__name;
    std::string __Vtask_test_tile_engine__DOT__check__87__name;
    std::string __Vtask_test_tile_engine__DOT__check__118__name;
    std::string __Vtask_test_tile_engine__DOT__check__119__name;
    std::string __Vtask_test_tile_engine__DOT__check__121__name;
    std::string __Vtask_test_tile_engine__DOT__check__123__name;
    std::string __Vtask_test_tile_engine__DOT__check__127__name;
    std::string __Vtask_test_tile_engine__DOT__check__128__name;
    std::string __Vtask_test_tile_engine__DOT__check__129__name;
    std::string __Vtask_test_tile_engine__DOT__check__134__name;
    std::string __Vtask_test_tile_engine__DOT__check__135__name;
    std::string __Vtask_test_tile_engine__DOT__check__142__name;
    std::string __Vtask_test_tile_engine__DOT__check__143__name;
    std::string __Vtask_test_tile_engine__DOT__check__152__name;
    std::string __Vtask_test_tile_engine__DOT__check__153__name;
    std::string __Vtask_test_tile_engine__DOT__check__154__name;
    std::string __Vtask_test_tile_engine__DOT__check__161__name;
    std::string __Vtask_test_tile_engine__DOT__check__162__name;
    std::string __Vtask_test_tile_engine__DOT__check__163__name;
    std::string __Vtask_test_tile_engine__DOT__check__173__name;
    std::string __Vtask_test_tile_engine__DOT__check__174__name;
    std::string __Vtask_test_tile_engine__DOT__check__175__name;
    std::string __Vtask_test_tile_engine__DOT__check__187__name;
    std::string __Vtask_test_tile_engine__DOT__check__188__name;
    std::string __Vtask_test_tile_engine__DOT__check__189__name;
    std::string __Vtask_test_tile_engine__DOT__check__191__name;
    std::string __Vtask_test_tile_engine__DOT__check__193__name;
    std::string __Vtask_test_tile_engine__DOT__check__195__name;
    std::string __Vtask_test_tile_engine__DOT__check__199__name;
    std::string __Vtask_test_tile_engine__DOT__check__201__name;
    std::string __Vtask_test_tile_engine__DOT__check__203__name;
    std::string __Vtask_test_tile_engine__DOT__check__205__name;
    std::string __Vtask_test_tile_engine__DOT__check__209__name;
    std::string __Vtask_test_tile_engine__DOT__check__211__name;
    std::string __Vtask_test_tile_engine__DOT__check__226__name;
    std::string __Vtask_test_tile_engine__DOT__check__227__name;
    std::string __Vtask_test_tile_engine__DOT__check__230__name;
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
