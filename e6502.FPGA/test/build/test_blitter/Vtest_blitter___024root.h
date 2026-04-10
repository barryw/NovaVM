// Verilated -*- C++ -*-
// DESCRIPTION: Verilator output: Design internal header
// See Vtest_blitter.h for the primary calling header

#ifndef VERILATED_VTEST_BLITTER___024ROOT_H_
#define VERILATED_VTEST_BLITTER___024ROOT_H_  // guard

#include "verilated.h"
#include "verilated_timing.h"


class Vtest_blitter__Syms;

class alignas(VL_CACHE_LINE_BYTES) Vtest_blitter___024root final {
  public:

    // DESIGN SPECIFIC STATE
    // Anonymous structures to workaround compiler member-count bugs
    struct {
        CData/*3:0*/ test_blitter__DOT____Vlvbound_h321272e9__0;
        CData/*7:0*/ test_blitter__DOT____Vlvbound_h22a4cf02__0;
        CData/*7:0*/ test_blitter__DOT____Vlvbound_h7d2f9654__0;
        CData/*3:0*/ test_blitter__DOT____Vlvbound_ha699a557__0;
        CData/*7:0*/ test_blitter__DOT____Vlvbound_h530fe462__0;
        CData/*7:0*/ test_blitter__DOT____Vlvbound_h6c644df4__0;
        CData/*0:0*/ test_blitter__DOT__clk;
        CData/*0:0*/ test_blitter__DOT__rst;
        CData/*7:0*/ test_blitter__DOT__cpu_wdata;
        CData/*0:0*/ test_blitter__DOT__cpu_we;
        CData/*7:0*/ test_blitter__DOT__blt_ram_wdata;
        CData/*0:0*/ test_blitter__DOT__blt_ram_we;
        CData/*7:0*/ test_blitter__DOT__blt_xram_wdata;
        CData/*0:0*/ test_blitter__DOT__blt_xram_we;
        CData/*2:0*/ test_blitter__DOT__blt_vgc_space;
        CData/*7:0*/ test_blitter__DOT__blt_vgc_wdata;
        CData/*0:0*/ test_blitter__DOT__blt_vgc_we;
        CData/*7:0*/ test_blitter__DOT__dut__DOT____Vlvbound_hf31f73b8__0;
        CData/*3:0*/ test_blitter__DOT__dut__DOT__state;
        CData/*0:0*/ test_blitter__DOT__dut__DOT__fill_mode;
        CData/*0:0*/ test_blitter__DOT__dut__DOT__colorkey_mode;
        CData/*0:0*/ test_blitter__DOT__dut__DOT__use_row_buffer;
        CData/*2:0*/ test_blitter__DOT__dut__DOT__src_space;
        CData/*2:0*/ test_blitter__DOT__dut__DOT__dst_space;
        CData/*7:0*/ test_blitter__DOT__dut__DOT__fill_value;
        CData/*7:0*/ test_blitter__DOT__dut__DOT__color_key;
        CData/*7:0*/ test_blitter__DOT__dut__DOT__read_byte;
        CData/*0:0*/ test_blitter__DOT__dut__DOT__read_valid;
        CData/*0:0*/ test_blitter__DOT__dut__DOT__blt_sel;
        CData/*7:0*/ test_blitter__DOT__dut__DOT__mem_read_data;
        CData/*7:0*/ __Vtask_test_blitter__DOT__write_reg__661__data;
        CData/*0:0*/ __VstlFirstIteration;
        CData/*0:0*/ __VstlPhaseResult;
        CData/*0:0*/ __Vtrigprevexpr___TOP__test_blitter__DOT__clk__0;
        CData/*0:0*/ __VactPhaseResult;
        CData/*0:0*/ __VinactPhaseResult;
        CData/*0:0*/ __VnbaPhaseResult;
        SData/*15:0*/ test_blitter__DOT__cpu_addr;
        SData/*15:0*/ test_blitter__DOT__blt_ram_addr;
        SData/*15:0*/ test_blitter__DOT__blt_vgc_addr;
        SData/*15:0*/ test_blitter__DOT__dut__DOT__width;
        SData/*15:0*/ test_blitter__DOT__dut__DOT__height;
        SData/*15:0*/ test_blitter__DOT__dut__DOT__src_stride;
        SData/*15:0*/ test_blitter__DOT__dut__DOT__dst_stride;
        SData/*15:0*/ test_blitter__DOT__dut__DOT__row;
        SData/*15:0*/ test_blitter__DOT__dut__DOT__col;
        SData/*9:0*/ test_blitter__DOT__dut__DOT__buf_idx;
        IData/*18:0*/ test_blitter__DOT__blt_xram_addr;
        IData/*31:0*/ test_blitter__DOT__pass_count;
        IData/*31:0*/ test_blitter__DOT__fail_count;
        IData/*31:0*/ test_blitter__DOT__test_num;
        IData/*23:0*/ test_blitter__DOT__dut__DOT__src_base;
        IData/*23:0*/ test_blitter__DOT__dut__DOT__dst_base;
        IData/*23:0*/ test_blitter__DOT__dut__DOT__wrote_count;
        IData/*31:0*/ __VactIterCount;
        IData/*31:0*/ __VinactIterCount;
        IData/*31:0*/ __Vi;
        VlUnpacked<CData/*7:0*/, 65536> test_blitter__DOT__sim_ram;
        VlUnpacked<CData/*7:0*/, 524288> test_blitter__DOT__sim_xram;
        VlUnpacked<CData/*7:0*/, 2000> test_blitter__DOT__sim_char;
        VlUnpacked<CData/*7:0*/, 2000> test_blitter__DOT__sim_color;
        VlUnpacked<CData/*3:0*/, 64000> test_blitter__DOT__sim_gfx;
        VlUnpacked<CData/*7:0*/, 32768> test_blitter__DOT__sim_sprite;
        VlUnpacked<CData/*7:0*/, 25> test_blitter__DOT__dut__DOT__regs;
    };
    struct {
        VlUnpacked<CData/*7:0*/, 1024> test_blitter__DOT__dut__DOT__row_buf;
        VlUnpacked<QData/*63:0*/, 1> __VstlTriggered;
        VlUnpacked<QData/*63:0*/, 1> __VactTriggered;
        VlUnpacked<QData/*63:0*/, 1> __VactTriggeredAcc;
        VlUnpacked<QData/*63:0*/, 1> __VnbaTriggered;
    };
    std::string __Vtask_test_blitter__DOT__check__0__name;
    std::string __Vtask_test_blitter__DOT__check__1__name;
    std::string __Vtask_test_blitter__DOT__check__31__name;
    std::string __Vtask_test_blitter__DOT__check__32__name;
    std::string __Vtask_test_blitter__DOT__check__33__name;
    std::string __Vtask_test_blitter__DOT__check__34__name;
    std::string __Vtask_test_blitter__DOT__check__35__name;
    std::string __Vtask_test_blitter__DOT__check__36__name;
    std::string __Vtask_test_blitter__DOT__check__37__name;
    std::string __Vtask_test_blitter__DOT__check__38__name;
    std::string __Vtask_test_blitter__DOT__check__39__name;
    std::string __Vtask_test_blitter__DOT__check__40__name;
    std::string __Vtask_test_blitter__DOT__check__70__name;
    std::string __Vtask_test_blitter__DOT__check__71__name;
    std::string __Vtask_test_blitter__DOT__check__72__name;
    std::string __Vtask_test_blitter__DOT__check__73__name;
    std::string __Vtask_test_blitter__DOT__check__74__name;
    std::string __Vtask_test_blitter__DOT__check__114__name;
    std::string __Vtask_test_blitter__DOT__check__115__name;
    std::string __Vtask_test_blitter__DOT__check__116__name;
    std::string __Vtask_test_blitter__DOT__check__117__name;
    std::string __Vtask_test_blitter__DOT__check__118__name;
    std::string __Vtask_test_blitter__DOT__check__119__name;
    std::string __Vtask_test_blitter__DOT__check__159__name;
    std::string __Vtask_test_blitter__DOT__check__160__name;
    std::string __Vtask_test_blitter__DOT__check__161__name;
    std::string __Vtask_test_blitter__DOT__check__162__name;
    std::string __Vtask_test_blitter__DOT__check__206__name;
    std::string __Vtask_test_blitter__DOT__check__207__name;
    std::string __Vtask_test_blitter__DOT__check__208__name;
    std::string __Vtask_test_blitter__DOT__check__209__name;
    std::string __Vtask_test_blitter__DOT__check__210__name;
    std::string __Vtask_test_blitter__DOT__check__211__name;
    std::string __Vtask_test_blitter__DOT__check__212__name;
    std::string __Vtask_test_blitter__DOT__check__241__name;
    std::string __Vtask_test_blitter__DOT__check__243__name;
    std::string __Vtask_test_blitter__DOT__check__246__name;
    std::string __Vtask_test_blitter__DOT__check__247__name;
    std::string __Vtask_test_blitter__DOT__check__261__name;
    std::string __Vtask_test_blitter__DOT__check__262__name;
    std::string __Vtask_test_blitter__DOT__check__274__name;
    std::string __Vtask_test_blitter__DOT__check__275__name;
    std::string __Vtask_test_blitter__DOT__check__292__name;
    std::string __Vtask_test_blitter__DOT__check__293__name;
    std::string __Vtask_test_blitter__DOT__check__333__name;
    std::string __Vtask_test_blitter__DOT__check__334__name;
    std::string __Vtask_test_blitter__DOT__check__335__name;
    std::string __Vtask_test_blitter__DOT__check__336__name;
    std::string __Vtask_test_blitter__DOT__check__337__name;
    std::string __Vtask_test_blitter__DOT__check__338__name;
    std::string __Vtask_test_blitter__DOT__check__368__name;
    std::string __Vtask_test_blitter__DOT__check__369__name;
    std::string __Vtask_test_blitter__DOT__check__370__name;
    std::string __Vtask_test_blitter__DOT__check__371__name;
    std::string __Vtask_test_blitter__DOT__check__411__name;
    std::string __Vtask_test_blitter__DOT__check__412__name;
    std::string __Vtask_test_blitter__DOT__check__413__name;
    std::string __Vtask_test_blitter__DOT__check__443__name;
    std::string __Vtask_test_blitter__DOT__check__444__name;
    std::string __Vtask_test_blitter__DOT__check__445__name;
    std::string __Vtask_test_blitter__DOT__check__446__name;
    std::string __Vtask_test_blitter__DOT__check__486__name;
    std::string __Vtask_test_blitter__DOT__check__487__name;
    std::string __Vtask_test_blitter__DOT__check__488__name;
    std::string __Vtask_test_blitter__DOT__check__489__name;
    std::string __Vtask_test_blitter__DOT__check__490__name;
    std::string __Vtask_test_blitter__DOT__check__530__name;
    std::string __Vtask_test_blitter__DOT__check__531__name;
    std::string __Vtask_test_blitter__DOT__check__561__name;
    std::string __Vtask_test_blitter__DOT__check__562__name;
    std::string __Vtask_test_blitter__DOT__check__592__name;
    std::string __Vtask_test_blitter__DOT__check__593__name;
    std::string __Vtask_test_blitter__DOT__check__623__name;
    std::string __Vtask_test_blitter__DOT__check__624__name;
    std::string __Vtask_test_blitter__DOT__check__668__name;
    std::string __Vtask_test_blitter__DOT__check__669__name;
    std::string __Vtask_test_blitter__DOT__check__670__name;
    std::string __Vtask_test_blitter__DOT__check__671__name;
    std::string __Vtask_test_blitter__DOT__check__672__name;
    VlDelayScheduler __VdlySched;
    VlTriggerScheduler __VtrigSched_h603fc354__0;

    // INTERNAL VARIABLES
    Vtest_blitter__Syms* vlSymsp;
    const char* vlNamep;

    // CONSTRUCTORS
    Vtest_blitter___024root(Vtest_blitter__Syms* symsp, const char* namep);
    ~Vtest_blitter___024root();
    VL_UNCOPYABLE(Vtest_blitter___024root);

    // INTERNAL METHODS
    void __Vconfigure(bool first);
};


#endif  // guard
