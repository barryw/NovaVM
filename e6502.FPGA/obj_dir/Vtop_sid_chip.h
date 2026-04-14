// Verilated -*- C++ -*-
// DESCRIPTION: Verilator output: Design internal header
// See Vtop.h for the primary calling header

#ifndef VERILATED_VTOP_SID_CHIP_H_
#define VERILATED_VTOP_SID_CHIP_H_  // guard

#include "verilated.h"


class Vtop__Syms;

class alignas(VL_CACHE_LINE_BYTES) Vtop_sid_chip final {
  public:

    // DESIGN SPECIFIC STATE
    // Anonymous structures to workaround compiler member-count bugs
    struct {
        CData/*7:0*/ __Vlvbound_h0706378b__0;
        CData/*7:0*/ __Vlvbound_h061f25e8__0;
        CData/*7:0*/ __Vlvbound_h49d001ad__0;
        CData/*7:0*/ __Vlvbound_h9cc45699__0;
        CData/*0:0*/ clk;
        CData/*0:0*/ rst;
        CData/*0:0*/ ce_1m;
        CData/*0:0*/ mode;
        CData/*0:0*/ cs;
        CData/*0:0*/ we;
        CData/*4:0*/ addr;
        CData/*7:0*/ din;
        CData/*7:0*/ dout;
        CData/*7:0*/ __PVT__filter_res_filt;
        CData/*7:0*/ __PVT__filter_mode_vol;
        CData/*7:0*/ __PVT__last_written;
        CData/*3:0*/ __PVT__tbl_state;
        CData/*7:0*/ __PVT__f___05Fst_out;
        CData/*7:0*/ __PVT__f_p_t_out;
        CData/*7:0*/ __PVT__f_ps___05Fout;
        CData/*7:0*/ __PVT__f_pst_out;
        CData/*7:0*/ __PVT__v1__DOT__envelope;
        CData/*0:0*/ __PVT__v1__DOT__osc_msb_in_prv;
        CData/*0:0*/ __PVT__v1__DOT__pulse;
        CData/*7:0*/ __PVT__v1__DOT__comb;
        CData/*7:0*/ __PVT__v1__DOT__wave_out;
        CData/*0:0*/ __PVT__v1__DOT__unnamedblk1__DOT__test_delay;
        CData/*0:0*/ __PVT__v1__DOT__unnamedblk2__DOT__clk;
        CData/*0:0*/ __PVT__v1__DOT__unnamedblk2__DOT__clk_d;
        CData/*0:0*/ __PVT__v1__DOT__unnamedblk2__DOT__osc_edge;
        CData/*1:0*/ __PVT__v1__DOT__adsr__DOT__state;
        CData/*0:0*/ __PVT__v1__DOT__adsr__DOT__unnamedblk1__DOT__hold_zero;
        CData/*4:0*/ __PVT__v1__DOT__adsr__DOT__unnamedblk1__DOT__exponential_counter_period;
        CData/*0:0*/ __PVT__v1__DOT__adsr__DOT__unnamedblk1__DOT__gate_edge;
        CData/*4:0*/ __PVT__v1__DOT__adsr__DOT__unnamedblk1__DOT__exponential_counter;
        CData/*7:0*/ __PVT__v2__DOT__envelope;
        CData/*0:0*/ __PVT__v2__DOT__osc_msb_in_prv;
        CData/*0:0*/ __PVT__v2__DOT__pulse;
        CData/*7:0*/ __PVT__v2__DOT__comb;
        CData/*7:0*/ __PVT__v2__DOT__wave_out;
        CData/*0:0*/ __PVT__v2__DOT__unnamedblk1__DOT__test_delay;
        CData/*0:0*/ __PVT__v2__DOT__unnamedblk2__DOT__clk;
        CData/*0:0*/ __PVT__v2__DOT__unnamedblk2__DOT__clk_d;
        CData/*0:0*/ __PVT__v2__DOT__unnamedblk2__DOT__osc_edge;
        CData/*1:0*/ __PVT__v2__DOT__adsr__DOT__state;
        CData/*0:0*/ __PVT__v2__DOT__adsr__DOT__unnamedblk1__DOT__hold_zero;
        CData/*4:0*/ __PVT__v2__DOT__adsr__DOT__unnamedblk1__DOT__exponential_counter_period;
        CData/*0:0*/ __PVT__v2__DOT__adsr__DOT__unnamedblk1__DOT__gate_edge;
        CData/*4:0*/ __PVT__v2__DOT__adsr__DOT__unnamedblk1__DOT__exponential_counter;
        CData/*7:0*/ __PVT__v3__DOT__envelope;
        CData/*0:0*/ __PVT__v3__DOT__osc_msb_in_prv;
        CData/*0:0*/ __PVT__v3__DOT__pulse;
        CData/*7:0*/ __PVT__v3__DOT__comb;
        CData/*7:0*/ __PVT__v3__DOT__wave_out;
        CData/*0:0*/ __PVT__v3__DOT__unnamedblk1__DOT__test_delay;
        CData/*0:0*/ __PVT__v3__DOT__unnamedblk2__DOT__clk;
        CData/*0:0*/ __PVT__v3__DOT__unnamedblk2__DOT__clk_d;
        CData/*0:0*/ __PVT__v3__DOT__unnamedblk2__DOT__osc_edge;
        CData/*1:0*/ __PVT__v3__DOT__adsr__DOT__state;
        CData/*0:0*/ __PVT__v3__DOT__adsr__DOT__unnamedblk1__DOT__hold_zero;
        CData/*4:0*/ __PVT__v3__DOT__adsr__DOT__unnamedblk1__DOT__exponential_counter_period;
        CData/*0:0*/ __PVT__v3__DOT__adsr__DOT__unnamedblk1__DOT__gate_edge;
        CData/*4:0*/ __PVT__v3__DOT__adsr__DOT__unnamedblk1__DOT__exponential_counter;
        CData/*0:0*/ __PVT__flt__DOT__s;
    };
    struct {
        SData/*10:0*/ __PVT__filter_fc;
        SData/*11:0*/ __Vcellout__v1__acc_t;
        SData/*11:0*/ __Vcellout__v2__acc_t;
        SData/*11:0*/ __Vcellout__v3__acc_t;
        SData/*11:0*/ __PVT__f_acc_t;
        SData/*11:0*/ __PVT__v1__DOT__noise;
        SData/*11:0*/ __PVT__v1__DOT__saw_tri;
        SData/*11:0*/ __PVT__v1__DOT__norm;
        SData/*11:0*/ __PVT__v1__DOT__norm_dac;
        SData/*8:0*/ __PVT__v1__DOT__unnamedblk3__DOT__env_dac;
        SData/*12:0*/ __PVT__v1__DOT__unnamedblk3__DOT__dac_out;
        SData/*14:0*/ __PVT__v1__DOT__adsr__DOT__rate_period;
        SData/*14:0*/ __PVT__v1__DOT__adsr__DOT__unnamedblk1__DOT__rate_counter;
        SData/*11:0*/ __PVT__v2__DOT__noise;
        SData/*11:0*/ __PVT__v2__DOT__saw_tri;
        SData/*11:0*/ __PVT__v2__DOT__norm;
        SData/*11:0*/ __PVT__v2__DOT__norm_dac;
        SData/*8:0*/ __PVT__v2__DOT__unnamedblk3__DOT__env_dac;
        SData/*12:0*/ __PVT__v2__DOT__unnamedblk3__DOT__dac_out;
        SData/*14:0*/ __PVT__v2__DOT__adsr__DOT__rate_period;
        SData/*14:0*/ __PVT__v2__DOT__adsr__DOT__unnamedblk1__DOT__rate_counter;
        SData/*11:0*/ __PVT__v3__DOT__noise;
        SData/*11:0*/ __PVT__v3__DOT__saw_tri;
        SData/*11:0*/ __PVT__v3__DOT__norm;
        SData/*11:0*/ __PVT__v3__DOT__norm_dac;
        SData/*8:0*/ __PVT__v3__DOT__unnamedblk3__DOT__env_dac;
        SData/*12:0*/ __PVT__v3__DOT__unnamedblk3__DOT__dac_out;
        SData/*14:0*/ __PVT__v3__DOT__adsr__DOT__rate_period;
        SData/*14:0*/ __PVT__v3__DOT__adsr__DOT__unnamedblk1__DOT__rate_counter;
        SData/*15:0*/ __PVT__tbl__DOT__f0;
        SData/*15:0*/ __PVT__flt__DOT__a;
        SData/*15:0*/ __PVT__flt__DOT__b;
        SData/*15:0*/ __PVT__flt__DOT__vlp;
        SData/*15:0*/ __PVT__flt__DOT__vlp2;
        SData/*15:0*/ __PVT__flt__DOT__vbp;
        SData/*15:0*/ __PVT__flt__DOT__vbp2;
        SData/*15:0*/ __PVT__flt__DOT__vbp_next;
        SData/*15:0*/ __PVT__flt__DOT__vhp;
        SData/*15:0*/ __PVT__flt__DOT__vhp2;
        SData/*15:0*/ __PVT__flt__DOT__vhp_next;
        SData/*10:0*/ __PVT__flt__DOT__unnamedblk1__DOT___1_Q_lsl10;
        SData/*15:0*/ __PVT__flt__DOT__unnamedblk1__DOT__vi;
        SData/*15:0*/ __PVT__flt__DOT__unnamedblk1__DOT__vd;
        IData/*17:0*/ audio_out;
        IData/*21:0*/ __PVT__v1_scaled;
        IData/*21:0*/ __PVT__v2_scaled;
        IData/*21:0*/ __PVT__v3_scaled;
        IData/*23:0*/ __PVT__v1__DOT__oscillator;
        IData/*21:0*/ __PVT__v1__DOT__dca_out;
        IData/*23:0*/ __PVT__v1__DOT__unnamedblk2__DOT__noise_age;
        IData/*22:0*/ __PVT__v1__DOT__unnamedblk2__DOT__lfsr_noise;
        IData/*23:0*/ __PVT__v1__DOT__unnamedblk3__DOT__keep_cnt;
        IData/*23:0*/ __PVT__v2__DOT__oscillator;
        IData/*21:0*/ __PVT__v2__DOT__dca_out;
        IData/*23:0*/ __PVT__v2__DOT__unnamedblk2__DOT__noise_age;
        IData/*22:0*/ __PVT__v2__DOT__unnamedblk2__DOT__lfsr_noise;
        IData/*23:0*/ __PVT__v2__DOT__unnamedblk3__DOT__keep_cnt;
        IData/*23:0*/ __PVT__v3__DOT__oscillator;
        IData/*21:0*/ __PVT__v3__DOT__dca_out;
        IData/*23:0*/ __PVT__v3__DOT__unnamedblk2__DOT__noise_age;
        IData/*22:0*/ __PVT__v3__DOT__unnamedblk2__DOT__lfsr_noise;
        IData/*23:0*/ __PVT__v3__DOT__unnamedblk3__DOT__keep_cnt;
        IData/*31:0*/ __PVT__flt__DOT__c;
        IData/*31:0*/ __PVT__flt__DOT__o;
    };
    struct {
        IData/*16:0*/ __VdfgRegularize_hebeb780c_0_29;
        IData/*16:0*/ __VdfgRegularize_hebeb780c_0_34;
        VlUnpacked<SData/*15:0*/, 3> __PVT__voice_freq;
        VlUnpacked<SData/*11:0*/, 3> __PVT__voice_pw;
        VlUnpacked<CData/*7:0*/, 3> __PVT__voice_ctrl;
        VlUnpacked<CData/*7:0*/, 3> __PVT__voice_ad;
        VlUnpacked<CData/*7:0*/, 3> __PVT__voice_sr;
        VlUnpacked<CData/*3:0*/, 3> __PVT__voice_vol;
        VlUnpacked<CData/*7:0*/, 6> __PVT___st_out;
        VlUnpacked<CData/*7:0*/, 6> __PVT__p_t_out;
        VlUnpacked<CData/*7:0*/, 6> __PVT__ps___05Fout;
        VlUnpacked<CData/*7:0*/, 6> __PVT__pst_out;
        VlUnpacked<SData/*11:0*/, 3> __PVT__acc_t;
        VlUnpacked<SData/*14:0*/, 16> __PVT__v1__DOT__adsr__DOT__rates;
        VlUnpacked<SData/*15:0*/, 12> __PVT__v1__DOT__waveform_dac__DOT__bitval;
        VlUnpacked<SData/*15:0*/, 12> __PVT__v1__DOT__waveform_dac__DOT__bitsum;
        VlUnpacked<SData/*11:0*/, 8> __PVT__v1__DOT__envelope_dac__DOT__bitval;
        VlUnpacked<SData/*11:0*/, 8> __PVT__v1__DOT__envelope_dac__DOT__bitsum;
        VlUnpacked<SData/*14:0*/, 16> __PVT__v2__DOT__adsr__DOT__rates;
        VlUnpacked<SData/*15:0*/, 12> __PVT__v2__DOT__waveform_dac__DOT__bitval;
        VlUnpacked<SData/*15:0*/, 12> __PVT__v2__DOT__waveform_dac__DOT__bitsum;
        VlUnpacked<SData/*11:0*/, 8> __PVT__v2__DOT__envelope_dac__DOT__bitval;
        VlUnpacked<SData/*11:0*/, 8> __PVT__v2__DOT__envelope_dac__DOT__bitsum;
        VlUnpacked<SData/*14:0*/, 16> __PVT__v3__DOT__adsr__DOT__rates;
        VlUnpacked<SData/*15:0*/, 12> __PVT__v3__DOT__waveform_dac__DOT__bitval;
        VlUnpacked<SData/*15:0*/, 12> __PVT__v3__DOT__waveform_dac__DOT__bitsum;
        VlUnpacked<SData/*11:0*/, 8> __PVT__v3__DOT__envelope_dac__DOT__bitval;
        VlUnpacked<SData/*11:0*/, 8> __PVT__v3__DOT__envelope_dac__DOT__bitsum;
        VlUnpacked<CData/*7:0*/, 2048> __PVT__tbl__DOT__wave6581_p_t;
        VlUnpacked<CData/*7:0*/, 2048> __PVT__tbl__DOT__wave6581_ps_;
        VlUnpacked<CData/*7:0*/, 2048> __PVT__tbl__DOT__wave8580_p_t;
        VlUnpacked<CData/*7:0*/, 4096> __PVT__tbl__DOT__wave8580_ps_;
        VlUnpacked<SData/*15:0*/, 4096> __PVT__tbl__DOT__f6581_curve;
        VlUnpacked<SData/*10:0*/, 32> __PVT__flt__DOT___1_Q_lsl10_tbl;
    };

    // INTERNAL VARIABLES
    Vtop__Syms* vlSymsp;
    const char* vlNamep;

    // CONSTRUCTORS
    Vtop_sid_chip();
    ~Vtop_sid_chip();
    void ctor(Vtop__Syms* symsp, const char* namep);
    void dtor();
    VL_UNCOPYABLE(Vtop_sid_chip);

    // INTERNAL METHODS
    void __Vconfigure(bool first);
};


#endif  // guard
