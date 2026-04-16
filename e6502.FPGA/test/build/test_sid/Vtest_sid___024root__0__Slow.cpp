// Verilated -*- C++ -*-
// DESCRIPTION: Verilator output: Design implementation internals
// See Vtest_sid.h for the primary calling header

#include "Vtest_sid__pch.h"

void Vtest_sid___024root___timing_ready(Vtest_sid___024root* vlSelf);

VL_ATTR_COLD void Vtest_sid___024root___eval_static(Vtest_sid___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vtest_sid___024root___eval_static\n"); );
    Vtest_sid__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
    // Body
    vlSelfRef.test_sid__DOT__clk = 0U;
    vlSelfRef.test_sid__DOT__ce_cnt = 0U;
    vlSelfRef.test_sid__DOT__pass = 0U;
    vlSelfRef.test_sid__DOT__fail = 0U;
    const uint64_t __VscopeHash = VL_MURMUR64_HASH(vlSelf->vlNamep);
    vlSelf->test_sid__DOT__uut__DOT__v1__DOT__unnamedblk1__DOT__test_delay = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 16117600420166689154ull);
    vlSelf->test_sid__DOT__uut__DOT__v1__DOT__unnamedblk2__DOT__clk = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 13166873756405073570ull);
    vlSelf->test_sid__DOT__uut__DOT__v1__DOT__unnamedblk2__DOT__clk_d = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 5138903571730307441ull);
    vlSelf->test_sid__DOT__uut__DOT__v1__DOT__unnamedblk2__DOT__osc_edge = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 12723812506248377113ull);
    vlSelf->test_sid__DOT__uut__DOT__v1__DOT__unnamedblk2__DOT__noise_age = VL_SCOPED_RAND_RESET_I(24, __VscopeHash, 1265364590852242079ull);
    vlSelf->test_sid__DOT__uut__DOT__v1__DOT__unnamedblk2__DOT__lfsr_noise = VL_SCOPED_RAND_RESET_I(23, __VscopeHash, 9290676650827812045ull);
    vlSelf->test_sid__DOT__uut__DOT__v1__DOT__unnamedblk3__DOT__keep_cnt = VL_SCOPED_RAND_RESET_I(24, __VscopeHash, 12933861964248559640ull);
    vlSelf->test_sid__DOT__uut__DOT__v1__DOT__unnamedblk3__DOT__env_dac = VL_SCOPED_RAND_RESET_I(9, __VscopeHash, 1549336321884260572ull);
    vlSelf->test_sid__DOT__uut__DOT__v1__DOT__unnamedblk3__DOT__dac_out = VL_SCOPED_RAND_RESET_I(13, __VscopeHash, 8651860190660829892ull);
    vlSelf->test_sid__DOT__uut__DOT__v1__DOT__adsr__DOT__unnamedblk1__DOT__hold_zero = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 14753951681453024454ull);
    vlSelf->test_sid__DOT__uut__DOT__v1__DOT__adsr__DOT__unnamedblk1__DOT__exponential_counter_period = VL_SCOPED_RAND_RESET_I(5, __VscopeHash, 2754922191108436581ull);
    vlSelf->test_sid__DOT__uut__DOT__v1__DOT__adsr__DOT__unnamedblk1__DOT__gate_edge = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 12960663616485046449ull);
    vlSelf->test_sid__DOT__uut__DOT__v1__DOT__adsr__DOT__unnamedblk1__DOT__rate_counter = VL_SCOPED_RAND_RESET_I(15, __VscopeHash, 16556169920062009735ull);
    vlSelf->test_sid__DOT__uut__DOT__v1__DOT__adsr__DOT__unnamedblk1__DOT__exponential_counter = VL_SCOPED_RAND_RESET_I(5, __VscopeHash, 13323514125748582015ull);
    vlSelf->test_sid__DOT__uut__DOT__v2__DOT__unnamedblk1__DOT__test_delay = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 342076380883924365ull);
    vlSelf->test_sid__DOT__uut__DOT__v2__DOT__unnamedblk2__DOT__clk = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 3007580286433042991ull);
    vlSelf->test_sid__DOT__uut__DOT__v2__DOT__unnamedblk2__DOT__clk_d = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 1195933550901467205ull);
    vlSelf->test_sid__DOT__uut__DOT__v2__DOT__unnamedblk2__DOT__osc_edge = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 15643625234246484511ull);
    vlSelf->test_sid__DOT__uut__DOT__v2__DOT__unnamedblk2__DOT__noise_age = VL_SCOPED_RAND_RESET_I(24, __VscopeHash, 8144319909544451355ull);
    vlSelf->test_sid__DOT__uut__DOT__v2__DOT__unnamedblk2__DOT__lfsr_noise = VL_SCOPED_RAND_RESET_I(23, __VscopeHash, 2934641815267320105ull);
    vlSelf->test_sid__DOT__uut__DOT__v2__DOT__unnamedblk3__DOT__keep_cnt = VL_SCOPED_RAND_RESET_I(24, __VscopeHash, 7844733113371254550ull);
    vlSelf->test_sid__DOT__uut__DOT__v2__DOT__unnamedblk3__DOT__env_dac = VL_SCOPED_RAND_RESET_I(9, __VscopeHash, 8217007032942622364ull);
    vlSelf->test_sid__DOT__uut__DOT__v2__DOT__unnamedblk3__DOT__dac_out = VL_SCOPED_RAND_RESET_I(13, __VscopeHash, 16250628826718571804ull);
    vlSelf->test_sid__DOT__uut__DOT__v2__DOT__adsr__DOT__unnamedblk1__DOT__hold_zero = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 13385107372630140814ull);
    vlSelf->test_sid__DOT__uut__DOT__v2__DOT__adsr__DOT__unnamedblk1__DOT__exponential_counter_period = VL_SCOPED_RAND_RESET_I(5, __VscopeHash, 2067895201168155261ull);
    vlSelf->test_sid__DOT__uut__DOT__v2__DOT__adsr__DOT__unnamedblk1__DOT__gate_edge = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 9552229706695250148ull);
    vlSelf->test_sid__DOT__uut__DOT__v2__DOT__adsr__DOT__unnamedblk1__DOT__rate_counter = VL_SCOPED_RAND_RESET_I(15, __VscopeHash, 18156299963180741811ull);
    vlSelf->test_sid__DOT__uut__DOT__v2__DOT__adsr__DOT__unnamedblk1__DOT__exponential_counter = VL_SCOPED_RAND_RESET_I(5, __VscopeHash, 4873397524983108179ull);
    vlSelf->test_sid__DOT__uut__DOT__v3__DOT__unnamedblk1__DOT__test_delay = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 3606018276883058016ull);
    vlSelf->test_sid__DOT__uut__DOT__v3__DOT__unnamedblk2__DOT__clk = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 9439729028922731140ull);
    vlSelf->test_sid__DOT__uut__DOT__v3__DOT__unnamedblk2__DOT__clk_d = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 4702107345351801794ull);
    vlSelf->test_sid__DOT__uut__DOT__v3__DOT__unnamedblk2__DOT__osc_edge = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 17814592473547669558ull);
    vlSelf->test_sid__DOT__uut__DOT__v3__DOT__unnamedblk2__DOT__noise_age = VL_SCOPED_RAND_RESET_I(24, __VscopeHash, 14637992521881467732ull);
    vlSelf->test_sid__DOT__uut__DOT__v3__DOT__unnamedblk2__DOT__lfsr_noise = VL_SCOPED_RAND_RESET_I(23, __VscopeHash, 4097345513921216724ull);
    vlSelf->test_sid__DOT__uut__DOT__v3__DOT__unnamedblk3__DOT__keep_cnt = VL_SCOPED_RAND_RESET_I(24, __VscopeHash, 15385812142202535506ull);
    vlSelf->test_sid__DOT__uut__DOT__v3__DOT__unnamedblk3__DOT__env_dac = VL_SCOPED_RAND_RESET_I(9, __VscopeHash, 13635747435679609949ull);
    vlSelf->test_sid__DOT__uut__DOT__v3__DOT__unnamedblk3__DOT__dac_out = VL_SCOPED_RAND_RESET_I(13, __VscopeHash, 6061924111414143180ull);
    vlSelf->test_sid__DOT__uut__DOT__v3__DOT__adsr__DOT__unnamedblk1__DOT__hold_zero = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 2444244446512479400ull);
    vlSelf->test_sid__DOT__uut__DOT__v3__DOT__adsr__DOT__unnamedblk1__DOT__exponential_counter_period = VL_SCOPED_RAND_RESET_I(5, __VscopeHash, 16108997160192108016ull);
    vlSelf->test_sid__DOT__uut__DOT__v3__DOT__adsr__DOT__unnamedblk1__DOT__gate_edge = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 2593410852826702471ull);
    vlSelf->test_sid__DOT__uut__DOT__v3__DOT__adsr__DOT__unnamedblk1__DOT__rate_counter = VL_SCOPED_RAND_RESET_I(15, __VscopeHash, 1366459893650576837ull);
    vlSelf->test_sid__DOT__uut__DOT__v3__DOT__adsr__DOT__unnamedblk1__DOT__exponential_counter = VL_SCOPED_RAND_RESET_I(5, __VscopeHash, 9238505313244835002ull);
    vlSelf->test_sid__DOT__uut__DOT__flt__DOT__unnamedblk1__DOT___1_Q_lsl10 = VL_SCOPED_RAND_RESET_I(11, __VscopeHash, 1647819321993846030ull);
    vlSelf->test_sid__DOT__uut__DOT__flt__DOT__unnamedblk1__DOT__vi = VL_SCOPED_RAND_RESET_I(16, __VscopeHash, 13356356539196934307ull);
    vlSelf->test_sid__DOT__uut__DOT__flt__DOT__unnamedblk1__DOT__vd = VL_SCOPED_RAND_RESET_I(16, __VscopeHash, 2572394268730198598ull);
    vlSelfRef.__Vtrigprevexpr___TOP__test_sid__DOT__uut__DOT__v1__DOT__waveform_dac__DOT__bitsum__0 
        = vlSelfRef.test_sid__DOT__uut__DOT__v1__DOT__waveform_dac__DOT__bitsum;
    vlSelfRef.__Vtrigprevexpr___TOP__test_sid__DOT__uut__DOT__v1__DOT__envelope_dac__DOT__bitsum__0 
        = vlSelfRef.test_sid__DOT__uut__DOT__v1__DOT__envelope_dac__DOT__bitsum;
    vlSelfRef.__Vtrigprevexpr___TOP__test_sid__DOT__uut__DOT__v2__DOT__waveform_dac__DOT__bitsum__0 
        = vlSelfRef.test_sid__DOT__uut__DOT__v2__DOT__waveform_dac__DOT__bitsum;
    vlSelfRef.__Vtrigprevexpr___TOP__test_sid__DOT__uut__DOT__v2__DOT__envelope_dac__DOT__bitsum__0 
        = vlSelfRef.test_sid__DOT__uut__DOT__v2__DOT__envelope_dac__DOT__bitsum;
    vlSelfRef.__Vtrigprevexpr___TOP__test_sid__DOT__uut__DOT__v3__DOT__waveform_dac__DOT__bitsum__0 
        = vlSelfRef.test_sid__DOT__uut__DOT__v3__DOT__waveform_dac__DOT__bitsum;
    vlSelfRef.__Vtrigprevexpr___TOP__test_sid__DOT__uut__DOT__v3__DOT__envelope_dac__DOT__bitsum__0 
        = vlSelfRef.test_sid__DOT__uut__DOT__v3__DOT__envelope_dac__DOT__bitsum;
    vlSelfRef.__VactTriggered[0U] = (1ULL | vlSelfRef.__VactTriggered[0U]);
    vlSelfRef.__VactTriggered[0U] = (2ULL | vlSelfRef.__VactTriggered[0U]);
    vlSelfRef.__VactTriggered[0U] = (4ULL | vlSelfRef.__VactTriggered[0U]);
    vlSelfRef.__VactTriggered[0U] = (8ULL | vlSelfRef.__VactTriggered[0U]);
    vlSelfRef.__VactTriggered[0U] = (0x0000000000000010ULL 
                                     | vlSelfRef.__VactTriggered[0U]);
    vlSelfRef.__VactTriggered[0U] = (0x0000000000000020ULL 
                                     | vlSelfRef.__VactTriggered[0U]);
    vlSelfRef.__Vtrigprevexpr___TOP__test_sid__DOT__clk__0 = 0U;
    Vtest_sid___024root___timing_ready(vlSelf);
    do {
        vlSelfRef.__VactTriggeredAcc[vlSelfRef.__Vi] 
            = vlSelfRef.__VactTriggered[vlSelfRef.__Vi];
        vlSelfRef.__Vi = ((IData)(1U) + vlSelfRef.__Vi);
    } while ((0U >= vlSelfRef.__Vi));
}

VL_ATTR_COLD void Vtest_sid___024root___eval_static__TOP(Vtest_sid___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vtest_sid___024root___eval_static__TOP\n"); );
    Vtest_sid__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
    // Body
    vlSelfRef.test_sid__DOT__clk = 0U;
    vlSelfRef.test_sid__DOT__ce_cnt = 0U;
    vlSelfRef.test_sid__DOT__pass = 0U;
    vlSelfRef.test_sid__DOT__fail = 0U;
    const uint64_t __VscopeHash = VL_MURMUR64_HASH(vlSelf->vlNamep);
    vlSelf->test_sid__DOT__uut__DOT__v1__DOT__unnamedblk1__DOT__test_delay = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 16117600420166689154ull);
    vlSelf->test_sid__DOT__uut__DOT__v1__DOT__unnamedblk2__DOT__clk = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 13166873756405073570ull);
    vlSelf->test_sid__DOT__uut__DOT__v1__DOT__unnamedblk2__DOT__clk_d = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 5138903571730307441ull);
    vlSelf->test_sid__DOT__uut__DOT__v1__DOT__unnamedblk2__DOT__osc_edge = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 12723812506248377113ull);
    vlSelf->test_sid__DOT__uut__DOT__v1__DOT__unnamedblk2__DOT__noise_age = VL_SCOPED_RAND_RESET_I(24, __VscopeHash, 1265364590852242079ull);
    vlSelf->test_sid__DOT__uut__DOT__v1__DOT__unnamedblk2__DOT__lfsr_noise = VL_SCOPED_RAND_RESET_I(23, __VscopeHash, 9290676650827812045ull);
    vlSelf->test_sid__DOT__uut__DOT__v1__DOT__unnamedblk3__DOT__keep_cnt = VL_SCOPED_RAND_RESET_I(24, __VscopeHash, 12933861964248559640ull);
    vlSelf->test_sid__DOT__uut__DOT__v1__DOT__unnamedblk3__DOT__env_dac = VL_SCOPED_RAND_RESET_I(9, __VscopeHash, 1549336321884260572ull);
    vlSelf->test_sid__DOT__uut__DOT__v1__DOT__unnamedblk3__DOT__dac_out = VL_SCOPED_RAND_RESET_I(13, __VscopeHash, 8651860190660829892ull);
    vlSelf->test_sid__DOT__uut__DOT__v1__DOT__adsr__DOT__unnamedblk1__DOT__hold_zero = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 14753951681453024454ull);
    vlSelf->test_sid__DOT__uut__DOT__v1__DOT__adsr__DOT__unnamedblk1__DOT__exponential_counter_period = VL_SCOPED_RAND_RESET_I(5, __VscopeHash, 2754922191108436581ull);
    vlSelf->test_sid__DOT__uut__DOT__v1__DOT__adsr__DOT__unnamedblk1__DOT__gate_edge = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 12960663616485046449ull);
    vlSelf->test_sid__DOT__uut__DOT__v1__DOT__adsr__DOT__unnamedblk1__DOT__rate_counter = VL_SCOPED_RAND_RESET_I(15, __VscopeHash, 16556169920062009735ull);
    vlSelf->test_sid__DOT__uut__DOT__v1__DOT__adsr__DOT__unnamedblk1__DOT__exponential_counter = VL_SCOPED_RAND_RESET_I(5, __VscopeHash, 13323514125748582015ull);
    vlSelf->test_sid__DOT__uut__DOT__v2__DOT__unnamedblk1__DOT__test_delay = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 342076380883924365ull);
    vlSelf->test_sid__DOT__uut__DOT__v2__DOT__unnamedblk2__DOT__clk = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 3007580286433042991ull);
    vlSelf->test_sid__DOT__uut__DOT__v2__DOT__unnamedblk2__DOT__clk_d = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 1195933550901467205ull);
    vlSelf->test_sid__DOT__uut__DOT__v2__DOT__unnamedblk2__DOT__osc_edge = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 15643625234246484511ull);
    vlSelf->test_sid__DOT__uut__DOT__v2__DOT__unnamedblk2__DOT__noise_age = VL_SCOPED_RAND_RESET_I(24, __VscopeHash, 8144319909544451355ull);
    vlSelf->test_sid__DOT__uut__DOT__v2__DOT__unnamedblk2__DOT__lfsr_noise = VL_SCOPED_RAND_RESET_I(23, __VscopeHash, 2934641815267320105ull);
    vlSelf->test_sid__DOT__uut__DOT__v2__DOT__unnamedblk3__DOT__keep_cnt = VL_SCOPED_RAND_RESET_I(24, __VscopeHash, 7844733113371254550ull);
    vlSelf->test_sid__DOT__uut__DOT__v2__DOT__unnamedblk3__DOT__env_dac = VL_SCOPED_RAND_RESET_I(9, __VscopeHash, 8217007032942622364ull);
    vlSelf->test_sid__DOT__uut__DOT__v2__DOT__unnamedblk3__DOT__dac_out = VL_SCOPED_RAND_RESET_I(13, __VscopeHash, 16250628826718571804ull);
    vlSelf->test_sid__DOT__uut__DOT__v2__DOT__adsr__DOT__unnamedblk1__DOT__hold_zero = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 13385107372630140814ull);
    vlSelf->test_sid__DOT__uut__DOT__v2__DOT__adsr__DOT__unnamedblk1__DOT__exponential_counter_period = VL_SCOPED_RAND_RESET_I(5, __VscopeHash, 2067895201168155261ull);
    vlSelf->test_sid__DOT__uut__DOT__v2__DOT__adsr__DOT__unnamedblk1__DOT__gate_edge = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 9552229706695250148ull);
    vlSelf->test_sid__DOT__uut__DOT__v2__DOT__adsr__DOT__unnamedblk1__DOT__rate_counter = VL_SCOPED_RAND_RESET_I(15, __VscopeHash, 18156299963180741811ull);
    vlSelf->test_sid__DOT__uut__DOT__v2__DOT__adsr__DOT__unnamedblk1__DOT__exponential_counter = VL_SCOPED_RAND_RESET_I(5, __VscopeHash, 4873397524983108179ull);
    vlSelf->test_sid__DOT__uut__DOT__v3__DOT__unnamedblk1__DOT__test_delay = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 3606018276883058016ull);
    vlSelf->test_sid__DOT__uut__DOT__v3__DOT__unnamedblk2__DOT__clk = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 9439729028922731140ull);
    vlSelf->test_sid__DOT__uut__DOT__v3__DOT__unnamedblk2__DOT__clk_d = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 4702107345351801794ull);
    vlSelf->test_sid__DOT__uut__DOT__v3__DOT__unnamedblk2__DOT__osc_edge = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 17814592473547669558ull);
    vlSelf->test_sid__DOT__uut__DOT__v3__DOT__unnamedblk2__DOT__noise_age = VL_SCOPED_RAND_RESET_I(24, __VscopeHash, 14637992521881467732ull);
    vlSelf->test_sid__DOT__uut__DOT__v3__DOT__unnamedblk2__DOT__lfsr_noise = VL_SCOPED_RAND_RESET_I(23, __VscopeHash, 4097345513921216724ull);
    vlSelf->test_sid__DOT__uut__DOT__v3__DOT__unnamedblk3__DOT__keep_cnt = VL_SCOPED_RAND_RESET_I(24, __VscopeHash, 15385812142202535506ull);
    vlSelf->test_sid__DOT__uut__DOT__v3__DOT__unnamedblk3__DOT__env_dac = VL_SCOPED_RAND_RESET_I(9, __VscopeHash, 13635747435679609949ull);
    vlSelf->test_sid__DOT__uut__DOT__v3__DOT__unnamedblk3__DOT__dac_out = VL_SCOPED_RAND_RESET_I(13, __VscopeHash, 6061924111414143180ull);
    vlSelf->test_sid__DOT__uut__DOT__v3__DOT__adsr__DOT__unnamedblk1__DOT__hold_zero = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 2444244446512479400ull);
    vlSelf->test_sid__DOT__uut__DOT__v3__DOT__adsr__DOT__unnamedblk1__DOT__exponential_counter_period = VL_SCOPED_RAND_RESET_I(5, __VscopeHash, 16108997160192108016ull);
    vlSelf->test_sid__DOT__uut__DOT__v3__DOT__adsr__DOT__unnamedblk1__DOT__gate_edge = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 2593410852826702471ull);
    vlSelf->test_sid__DOT__uut__DOT__v3__DOT__adsr__DOT__unnamedblk1__DOT__rate_counter = VL_SCOPED_RAND_RESET_I(15, __VscopeHash, 1366459893650576837ull);
    vlSelf->test_sid__DOT__uut__DOT__v3__DOT__adsr__DOT__unnamedblk1__DOT__exponential_counter = VL_SCOPED_RAND_RESET_I(5, __VscopeHash, 9238505313244835002ull);
    vlSelf->test_sid__DOT__uut__DOT__flt__DOT__unnamedblk1__DOT___1_Q_lsl10 = VL_SCOPED_RAND_RESET_I(11, __VscopeHash, 1647819321993846030ull);
    vlSelf->test_sid__DOT__uut__DOT__flt__DOT__unnamedblk1__DOT__vi = VL_SCOPED_RAND_RESET_I(16, __VscopeHash, 13356356539196934307ull);
    vlSelf->test_sid__DOT__uut__DOT__flt__DOT__unnamedblk1__DOT__vd = VL_SCOPED_RAND_RESET_I(16, __VscopeHash, 2572394268730198598ull);
}

VL_ATTR_COLD void Vtest_sid___024root___eval_initial__TOP(Vtest_sid___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vtest_sid___024root___eval_initial__TOP\n"); );
    Vtest_sid__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
    // Body
    vlSelfRef.test_sid__DOT__uut__DOT__v1__DOT__adsr__DOT__rates[0U] = 8U;
    vlSelfRef.test_sid__DOT__uut__DOT__v1__DOT__adsr__DOT__rates[1U] = 0x001fU;
    vlSelfRef.test_sid__DOT__uut__DOT__v1__DOT__adsr__DOT__rates[2U] = 0x003eU;
    vlSelfRef.test_sid__DOT__uut__DOT__v1__DOT__adsr__DOT__rates[3U] = 0x005eU;
    vlSelfRef.test_sid__DOT__uut__DOT__v1__DOT__adsr__DOT__rates[4U] = 0x0094U;
    vlSelfRef.test_sid__DOT__uut__DOT__v1__DOT__adsr__DOT__rates[5U] = 0x00dbU;
    vlSelfRef.test_sid__DOT__uut__DOT__v1__DOT__adsr__DOT__rates[6U] = 0x010aU;
    vlSelfRef.test_sid__DOT__uut__DOT__v1__DOT__adsr__DOT__rates[7U] = 0x0138U;
    vlSelfRef.test_sid__DOT__uut__DOT__v1__DOT__adsr__DOT__rates[8U] = 0x0187U;
    vlSelfRef.test_sid__DOT__uut__DOT__v1__DOT__adsr__DOT__rates[9U] = 0x03d0U;
    vlSelfRef.test_sid__DOT__uut__DOT__v1__DOT__adsr__DOT__rates[10U] = 0x07a1U;
    vlSelfRef.test_sid__DOT__uut__DOT__v1__DOT__adsr__DOT__rates[11U] = 0x0c35U;
    vlSelfRef.test_sid__DOT__uut__DOT__v1__DOT__adsr__DOT__rates[12U] = 0x0f42U;
    vlSelfRef.test_sid__DOT__uut__DOT__v1__DOT__adsr__DOT__rates[13U] = 0x2dc7U;
    vlSelfRef.test_sid__DOT__uut__DOT__v1__DOT__adsr__DOT__rates[14U] = 0x4c4bU;
    vlSelfRef.test_sid__DOT__uut__DOT__v1__DOT__adsr__DOT__rates[15U] = 0x7a12U;
    vlSelfRef.test_sid__DOT__uut__DOT__v1__DOT__waveform_dac__DOT__bitval[0U] = 0x0021U;
    vlSelfRef.test_sid__DOT__uut__DOT__v1__DOT__waveform_dac__DOT__bitval[1U] = 0x0030U;
    vlSelfRef.test_sid__DOT__uut__DOT__v1__DOT__waveform_dac__DOT__bitval[2U] = 0x0055U;
    vlSelfRef.test_sid__DOT__uut__DOT__v1__DOT__waveform_dac__DOT__bitval[3U] = 0x00a0U;
    vlSelfRef.test_sid__DOT__uut__DOT__v1__DOT__waveform_dac__DOT__bitval[4U] = 0x0135U;
    vlSelfRef.test_sid__DOT__uut__DOT__v1__DOT__waveform_dac__DOT__bitval[5U] = 0x0256U;
    vlSelfRef.test_sid__DOT__uut__DOT__v1__DOT__waveform_dac__DOT__bitval[6U] = 0x0486U;
    vlSelfRef.test_sid__DOT__uut__DOT__v1__DOT__waveform_dac__DOT__bitval[7U] = 0x08c6U;
    vlSelfRef.test_sid__DOT__uut__DOT__v1__DOT__waveform_dac__DOT__bitval[8U] = 0x1102U;
    vlSelfRef.test_sid__DOT__uut__DOT__v1__DOT__waveform_dac__DOT__bitval[9U] = 0x20f8U;
    vlSelfRef.test_sid__DOT__uut__DOT__v1__DOT__waveform_dac__DOT__bitval[10U] = 0x3fecU;
    vlSelfRef.test_sid__DOT__uut__DOT__v1__DOT__waveform_dac__DOT__bitval[11U] = 0x7bedU;
    vlSelfRef.test_sid__DOT__uut__DOT__v1__DOT__envelope_dac__DOT__bitval[0U] = 0x001dU;
    vlSelfRef.test_sid__DOT__uut__DOT__v1__DOT__envelope_dac__DOT__bitval[1U] = 0x002aU;
    vlSelfRef.test_sid__DOT__uut__DOT__v1__DOT__envelope_dac__DOT__bitval[2U] = 0x004bU;
    vlSelfRef.test_sid__DOT__uut__DOT__v1__DOT__envelope_dac__DOT__bitval[3U] = 0x008dU;
    vlSelfRef.test_sid__DOT__uut__DOT__v1__DOT__envelope_dac__DOT__bitval[4U] = 0x0110U;
    vlSelfRef.test_sid__DOT__uut__DOT__v1__DOT__envelope_dac__DOT__bitval[5U] = 0x020eU;
    vlSelfRef.test_sid__DOT__uut__DOT__v1__DOT__envelope_dac__DOT__bitval[6U] = 0x03fbU;
    vlSelfRef.test_sid__DOT__uut__DOT__v1__DOT__envelope_dac__DOT__bitval[7U] = 0x07b8U;
    vlSelfRef.test_sid__DOT__uut__DOT__v2__DOT__adsr__DOT__rates[0U] = 8U;
    vlSelfRef.test_sid__DOT__uut__DOT__v2__DOT__adsr__DOT__rates[1U] = 0x001fU;
    vlSelfRef.test_sid__DOT__uut__DOT__v2__DOT__adsr__DOT__rates[2U] = 0x003eU;
    vlSelfRef.test_sid__DOT__uut__DOT__v2__DOT__adsr__DOT__rates[3U] = 0x005eU;
    vlSelfRef.test_sid__DOT__uut__DOT__v2__DOT__adsr__DOT__rates[4U] = 0x0094U;
    vlSelfRef.test_sid__DOT__uut__DOT__v2__DOT__adsr__DOT__rates[5U] = 0x00dbU;
    vlSelfRef.test_sid__DOT__uut__DOT__v2__DOT__adsr__DOT__rates[6U] = 0x010aU;
    vlSelfRef.test_sid__DOT__uut__DOT__v2__DOT__adsr__DOT__rates[7U] = 0x0138U;
    vlSelfRef.test_sid__DOT__uut__DOT__v2__DOT__adsr__DOT__rates[8U] = 0x0187U;
    vlSelfRef.test_sid__DOT__uut__DOT__v2__DOT__adsr__DOT__rates[9U] = 0x03d0U;
    vlSelfRef.test_sid__DOT__uut__DOT__v2__DOT__adsr__DOT__rates[10U] = 0x07a1U;
    vlSelfRef.test_sid__DOT__uut__DOT__v2__DOT__adsr__DOT__rates[11U] = 0x0c35U;
    vlSelfRef.test_sid__DOT__uut__DOT__v2__DOT__adsr__DOT__rates[12U] = 0x0f42U;
    vlSelfRef.test_sid__DOT__uut__DOT__v2__DOT__adsr__DOT__rates[13U] = 0x2dc7U;
    vlSelfRef.test_sid__DOT__uut__DOT__v2__DOT__adsr__DOT__rates[14U] = 0x4c4bU;
    vlSelfRef.test_sid__DOT__uut__DOT__v2__DOT__adsr__DOT__rates[15U] = 0x7a12U;
    vlSelfRef.test_sid__DOT__uut__DOT__v2__DOT__waveform_dac__DOT__bitval[0U] = 0x0021U;
    vlSelfRef.test_sid__DOT__uut__DOT__v2__DOT__waveform_dac__DOT__bitval[1U] = 0x0030U;
    vlSelfRef.test_sid__DOT__uut__DOT__v2__DOT__waveform_dac__DOT__bitval[2U] = 0x0055U;
    vlSelfRef.test_sid__DOT__uut__DOT__v2__DOT__waveform_dac__DOT__bitval[3U] = 0x00a0U;
    vlSelfRef.test_sid__DOT__uut__DOT__v2__DOT__waveform_dac__DOT__bitval[4U] = 0x0135U;
    vlSelfRef.test_sid__DOT__uut__DOT__v2__DOT__waveform_dac__DOT__bitval[5U] = 0x0256U;
    vlSelfRef.test_sid__DOT__uut__DOT__v2__DOT__waveform_dac__DOT__bitval[6U] = 0x0486U;
    vlSelfRef.test_sid__DOT__uut__DOT__v2__DOT__waveform_dac__DOT__bitval[7U] = 0x08c6U;
    vlSelfRef.test_sid__DOT__uut__DOT__v2__DOT__waveform_dac__DOT__bitval[8U] = 0x1102U;
    vlSelfRef.test_sid__DOT__uut__DOT__v2__DOT__waveform_dac__DOT__bitval[9U] = 0x20f8U;
    vlSelfRef.test_sid__DOT__uut__DOT__v2__DOT__waveform_dac__DOT__bitval[10U] = 0x3fecU;
    vlSelfRef.test_sid__DOT__uut__DOT__v2__DOT__waveform_dac__DOT__bitval[11U] = 0x7bedU;
    vlSelfRef.test_sid__DOT__uut__DOT__v2__DOT__envelope_dac__DOT__bitval[0U] = 0x001dU;
    vlSelfRef.test_sid__DOT__uut__DOT__v2__DOT__envelope_dac__DOT__bitval[1U] = 0x002aU;
    vlSelfRef.test_sid__DOT__uut__DOT__v2__DOT__envelope_dac__DOT__bitval[2U] = 0x004bU;
    vlSelfRef.test_sid__DOT__uut__DOT__v2__DOT__envelope_dac__DOT__bitval[3U] = 0x008dU;
    vlSelfRef.test_sid__DOT__uut__DOT__v2__DOT__envelope_dac__DOT__bitval[4U] = 0x0110U;
    vlSelfRef.test_sid__DOT__uut__DOT__v2__DOT__envelope_dac__DOT__bitval[5U] = 0x020eU;
    vlSelfRef.test_sid__DOT__uut__DOT__v2__DOT__envelope_dac__DOT__bitval[6U] = 0x03fbU;
    vlSelfRef.test_sid__DOT__uut__DOT__v2__DOT__envelope_dac__DOT__bitval[7U] = 0x07b8U;
    vlSelfRef.test_sid__DOT__uut__DOT__v3__DOT__adsr__DOT__rates[0U] = 8U;
    vlSelfRef.test_sid__DOT__uut__DOT__v3__DOT__adsr__DOT__rates[1U] = 0x001fU;
    vlSelfRef.test_sid__DOT__uut__DOT__v3__DOT__adsr__DOT__rates[2U] = 0x003eU;
    vlSelfRef.test_sid__DOT__uut__DOT__v3__DOT__adsr__DOT__rates[3U] = 0x005eU;
    vlSelfRef.test_sid__DOT__uut__DOT__v3__DOT__adsr__DOT__rates[4U] = 0x0094U;
    vlSelfRef.test_sid__DOT__uut__DOT__v3__DOT__adsr__DOT__rates[5U] = 0x00dbU;
    vlSelfRef.test_sid__DOT__uut__DOT__v3__DOT__adsr__DOT__rates[6U] = 0x010aU;
    vlSelfRef.test_sid__DOT__uut__DOT__v3__DOT__adsr__DOT__rates[7U] = 0x0138U;
    vlSelfRef.test_sid__DOT__uut__DOT__v3__DOT__adsr__DOT__rates[8U] = 0x0187U;
    vlSelfRef.test_sid__DOT__uut__DOT__v3__DOT__adsr__DOT__rates[9U] = 0x03d0U;
    vlSelfRef.test_sid__DOT__uut__DOT__v3__DOT__adsr__DOT__rates[10U] = 0x07a1U;
    vlSelfRef.test_sid__DOT__uut__DOT__v3__DOT__adsr__DOT__rates[11U] = 0x0c35U;
    vlSelfRef.test_sid__DOT__uut__DOT__v3__DOT__adsr__DOT__rates[12U] = 0x0f42U;
    vlSelfRef.test_sid__DOT__uut__DOT__v3__DOT__adsr__DOT__rates[13U] = 0x2dc7U;
    vlSelfRef.test_sid__DOT__uut__DOT__v3__DOT__adsr__DOT__rates[14U] = 0x4c4bU;
    vlSelfRef.test_sid__DOT__uut__DOT__v3__DOT__adsr__DOT__rates[15U] = 0x7a12U;
    vlSelfRef.test_sid__DOT__uut__DOT__v3__DOT__waveform_dac__DOT__bitval[0U] = 0x0021U;
    vlSelfRef.test_sid__DOT__uut__DOT__v3__DOT__waveform_dac__DOT__bitval[1U] = 0x0030U;
    vlSelfRef.test_sid__DOT__uut__DOT__v3__DOT__waveform_dac__DOT__bitval[2U] = 0x0055U;
    vlSelfRef.test_sid__DOT__uut__DOT__v3__DOT__waveform_dac__DOT__bitval[3U] = 0x00a0U;
    vlSelfRef.test_sid__DOT__uut__DOT__v3__DOT__waveform_dac__DOT__bitval[4U] = 0x0135U;
    vlSelfRef.test_sid__DOT__uut__DOT__v3__DOT__waveform_dac__DOT__bitval[5U] = 0x0256U;
    vlSelfRef.test_sid__DOT__uut__DOT__v3__DOT__waveform_dac__DOT__bitval[6U] = 0x0486U;
    vlSelfRef.test_sid__DOT__uut__DOT__v3__DOT__waveform_dac__DOT__bitval[7U] = 0x08c6U;
    vlSelfRef.test_sid__DOT__uut__DOT__v3__DOT__waveform_dac__DOT__bitval[8U] = 0x1102U;
    vlSelfRef.test_sid__DOT__uut__DOT__v3__DOT__waveform_dac__DOT__bitval[9U] = 0x20f8U;
    vlSelfRef.test_sid__DOT__uut__DOT__v3__DOT__waveform_dac__DOT__bitval[10U] = 0x3fecU;
    vlSelfRef.test_sid__DOT__uut__DOT__v3__DOT__waveform_dac__DOT__bitval[11U] = 0x7bedU;
    vlSelfRef.test_sid__DOT__uut__DOT__v3__DOT__envelope_dac__DOT__bitval[0U] = 0x001dU;
    vlSelfRef.test_sid__DOT__uut__DOT__v3__DOT__envelope_dac__DOT__bitval[1U] = 0x002aU;
    vlSelfRef.test_sid__DOT__uut__DOT__v3__DOT__envelope_dac__DOT__bitval[2U] = 0x004bU;
    vlSelfRef.test_sid__DOT__uut__DOT__v3__DOT__envelope_dac__DOT__bitval[3U] = 0x008dU;
    vlSelfRef.test_sid__DOT__uut__DOT__v3__DOT__envelope_dac__DOT__bitval[4U] = 0x0110U;
    vlSelfRef.test_sid__DOT__uut__DOT__v3__DOT__envelope_dac__DOT__bitval[5U] = 0x020eU;
    vlSelfRef.test_sid__DOT__uut__DOT__v3__DOT__envelope_dac__DOT__bitval[6U] = 0x03fbU;
    vlSelfRef.test_sid__DOT__uut__DOT__v3__DOT__envelope_dac__DOT__bitval[7U] = 0x07b8U;
    VL_READMEM_N(true, 8, 2048, 0, "rom/wave6581_p_t.hex"s
                 ,  &(vlSelfRef.test_sid__DOT__uut__DOT__tbl__DOT__wave6581_p_t)
                 , 0, ~0ULL);
    VL_READMEM_N(true, 8, 2048, 0, "rom/wave6581_ps_.hex"s
                 ,  &(vlSelfRef.test_sid__DOT__uut__DOT__tbl__DOT__wave6581_ps_)
                 , 0, ~0ULL);
    VL_READMEM_N(true, 8, 2048, 0, "rom/wave8580_p_t.hex"s
                 ,  &(vlSelfRef.test_sid__DOT__uut__DOT__tbl__DOT__wave8580_p_t)
                 , 0, ~0ULL);
    VL_READMEM_N(true, 8, 4096, 0, "rom/wave8580_ps_.hex"s
                 ,  &(vlSelfRef.test_sid__DOT__uut__DOT__tbl__DOT__wave8580_ps_)
                 , 0, ~0ULL);
    VL_READMEM_N(true, 16, 4096, 0, "rom/f6581_curve.hex"s
                 ,  &(vlSelfRef.test_sid__DOT__uut__DOT__tbl__DOT__f6581_curve)
                 , 0, ~0ULL);
    VL_READMEM_N(true, 15, 1024, 0, "rom/_unused_f6581_adj.hex"s
                 ,  &(vlSelfRef.test_sid__DOT__uut__DOT__tbl__DOT___unused_f6581_adj)
                 , 0, ~0ULL);
    vlSelfRef.test_sid__DOT__uut__DOT__flt__DOT___1_Q_lsl10_tbl[0U] = 0x05a8U;
    vlSelfRef.test_sid__DOT__uut__DOT__flt__DOT___1_Q_lsl10_tbl[1U] = 0x052cU;
    vlSelfRef.test_sid__DOT__uut__DOT__flt__DOT___1_Q_lsl10_tbl[2U] = 0x04c3U;
    vlSelfRef.test_sid__DOT__uut__DOT__flt__DOT___1_Q_lsl10_tbl[3U] = 0x0469U;
    vlSelfRef.test_sid__DOT__uut__DOT__flt__DOT___1_Q_lsl10_tbl[4U] = 0x041cU;
    vlSelfRef.test_sid__DOT__uut__DOT__flt__DOT___1_Q_lsl10_tbl[5U] = 0x03d8U;
    vlSelfRef.test_sid__DOT__uut__DOT__flt__DOT___1_Q_lsl10_tbl[6U] = 0x039dU;
    vlSelfRef.test_sid__DOT__uut__DOT__flt__DOT___1_Q_lsl10_tbl[7U] = 0x0368U;
    vlSelfRef.test_sid__DOT__uut__DOT__flt__DOT___1_Q_lsl10_tbl[8U] = 0x033aU;
    vlSelfRef.test_sid__DOT__uut__DOT__flt__DOT___1_Q_lsl10_tbl[9U] = 0x030fU;
    vlSelfRef.test_sid__DOT__uut__DOT__flt__DOT___1_Q_lsl10_tbl[10U] = 0x02e9U;
    vlSelfRef.test_sid__DOT__uut__DOT__flt__DOT___1_Q_lsl10_tbl[11U] = 0x02c7U;
    vlSelfRef.test_sid__DOT__uut__DOT__flt__DOT___1_Q_lsl10_tbl[12U] = 0x02a7U;
    vlSelfRef.test_sid__DOT__uut__DOT__flt__DOT___1_Q_lsl10_tbl[13U] = 0x028bU;
    vlSelfRef.test_sid__DOT__uut__DOT__flt__DOT___1_Q_lsl10_tbl[14U] = 0x0270U;
    vlSelfRef.test_sid__DOT__uut__DOT__flt__DOT___1_Q_lsl10_tbl[15U] = 0x0258U;
    vlSelfRef.test_sid__DOT__uut__DOT__flt__DOT___1_Q_lsl10_tbl[16U] = 0x05a8U;
    vlSelfRef.test_sid__DOT__uut__DOT__flt__DOT___1_Q_lsl10_tbl[17U] = 0x0530U;
    vlSelfRef.test_sid__DOT__uut__DOT__flt__DOT___1_Q_lsl10_tbl[18U] = 0x04c2U;
    vlSelfRef.test_sid__DOT__uut__DOT__flt__DOT___1_Q_lsl10_tbl[19U] = 0x045dU;
    vlSelfRef.test_sid__DOT__uut__DOT__flt__DOT___1_Q_lsl10_tbl[20U] = 0x0400U;
    vlSelfRef.test_sid__DOT__uut__DOT__flt__DOT___1_Q_lsl10_tbl[21U] = 0x03abU;
    vlSelfRef.test_sid__DOT__uut__DOT__flt__DOT___1_Q_lsl10_tbl[22U] = 0x035dU;
    vlSelfRef.test_sid__DOT__uut__DOT__flt__DOT___1_Q_lsl10_tbl[23U] = 0x0316U;
    vlSelfRef.test_sid__DOT__uut__DOT__flt__DOT___1_Q_lsl10_tbl[24U] = 0x02d4U;
    vlSelfRef.test_sid__DOT__uut__DOT__flt__DOT___1_Q_lsl10_tbl[25U] = 0x0298U;
    vlSelfRef.test_sid__DOT__uut__DOT__flt__DOT___1_Q_lsl10_tbl[26U] = 0x0261U;
    vlSelfRef.test_sid__DOT__uut__DOT__flt__DOT___1_Q_lsl10_tbl[27U] = 0x022eU;
    vlSelfRef.test_sid__DOT__uut__DOT__flt__DOT___1_Q_lsl10_tbl[28U] = 0x0200U;
    vlSelfRef.test_sid__DOT__uut__DOT__flt__DOT___1_Q_lsl10_tbl[29U] = 0x01d6U;
    vlSelfRef.test_sid__DOT__uut__DOT__flt__DOT___1_Q_lsl10_tbl[30U] = 0x01afU;
    vlSelfRef.test_sid__DOT__uut__DOT__flt__DOT___1_Q_lsl10_tbl[31U] = 0x018bU;
}

VL_ATTR_COLD void Vtest_sid___024root___eval_final(Vtest_sid___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vtest_sid___024root___eval_final\n"); );
    Vtest_sid__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
}

#ifdef VL_DEBUG
VL_ATTR_COLD void Vtest_sid___024root___dump_triggers__stl(const VlUnpacked<QData/*63:0*/, 2> &triggers, const std::string &tag);
#endif  // VL_DEBUG
VL_ATTR_COLD bool Vtest_sid___024root___eval_phase__stl(Vtest_sid___024root* vlSelf);

VL_ATTR_COLD void Vtest_sid___024root___eval_settle(Vtest_sid___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vtest_sid___024root___eval_settle\n"); );
    Vtest_sid__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
    // Locals
    IData/*31:0*/ __VstlIterCount;
    // Body
    __VstlIterCount = 0U;
    vlSelfRef.__VstlFirstIteration = 1U;
    do {
        if (VL_UNLIKELY(((0x00000064U < __VstlIterCount)))) {
#ifdef VL_DEBUG
            Vtest_sid___024root___dump_triggers__stl(vlSelfRef.__VstlTriggered, "stl"s);
#endif
            VL_FATAL_MT("test_sid.sv", 4, "", "DIDNOTCONVERGE: Settle region did not converge after '--converge-limit' of 100 tries");
        }
        __VstlIterCount = ((IData)(1U) + __VstlIterCount);
        vlSelfRef.__VstlPhaseResult = Vtest_sid___024root___eval_phase__stl(vlSelf);
        vlSelfRef.__VstlFirstIteration = 0U;
    } while (vlSelfRef.__VstlPhaseResult);
}

VL_ATTR_COLD void Vtest_sid___024root___eval_triggers_vec__stl(Vtest_sid___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vtest_sid___024root___eval_triggers_vec__stl\n"); );
    Vtest_sid__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
    // Body
    vlSelfRef.__VstlTriggered[1U] = ((0xfffffffffffffffeULL 
                                      & vlSelfRef.__VstlTriggered[1U]) 
                                     | (IData)((IData)(vlSelfRef.__VstlFirstIteration)));
    vlSelfRef.__VstlTriggered[0U] = (QData)((IData)(
                                                    (((vlSelfRef.__Vtrigprevexpr___TOP__test_sid__DOT__uut__DOT__v3__DOT__envelope_dac__DOT__bitsum__0.neq(vlSelfRef.test_sid__DOT__uut__DOT__v3__DOT__envelope_dac__DOT__bitsum) 
                                                       << 5U) 
                                                      | (vlSelfRef.__Vtrigprevexpr___TOP__test_sid__DOT__uut__DOT__v3__DOT__waveform_dac__DOT__bitsum__0.neq(vlSelfRef.test_sid__DOT__uut__DOT__v3__DOT__waveform_dac__DOT__bitsum) 
                                                         << 4U)) 
                                                     | (((vlSelfRef.__Vtrigprevexpr___TOP__test_sid__DOT__uut__DOT__v2__DOT__envelope_dac__DOT__bitsum__0.neq(vlSelfRef.test_sid__DOT__uut__DOT__v2__DOT__envelope_dac__DOT__bitsum) 
                                                          << 3U) 
                                                         | (vlSelfRef.__Vtrigprevexpr___TOP__test_sid__DOT__uut__DOT__v2__DOT__waveform_dac__DOT__bitsum__0.neq(vlSelfRef.test_sid__DOT__uut__DOT__v2__DOT__waveform_dac__DOT__bitsum) 
                                                            << 2U)) 
                                                        | ((vlSelfRef.__Vtrigprevexpr___TOP__test_sid__DOT__uut__DOT__v1__DOT__envelope_dac__DOT__bitsum__0.neq(vlSelfRef.test_sid__DOT__uut__DOT__v1__DOT__envelope_dac__DOT__bitsum) 
                                                            << 1U) 
                                                           | vlSelfRef.__Vtrigprevexpr___TOP__test_sid__DOT__uut__DOT__v1__DOT__waveform_dac__DOT__bitsum__0.neq(vlSelfRef.test_sid__DOT__uut__DOT__v1__DOT__waveform_dac__DOT__bitsum))))));
    vlSelfRef.__Vtrigprevexpr___TOP__test_sid__DOT__uut__DOT__v1__DOT__waveform_dac__DOT__bitsum__0.assign(vlSelfRef.test_sid__DOT__uut__DOT__v1__DOT__waveform_dac__DOT__bitsum);
    vlSelfRef.__Vtrigprevexpr___TOP__test_sid__DOT__uut__DOT__v1__DOT__envelope_dac__DOT__bitsum__0.assign(vlSelfRef.test_sid__DOT__uut__DOT__v1__DOT__envelope_dac__DOT__bitsum);
    vlSelfRef.__Vtrigprevexpr___TOP__test_sid__DOT__uut__DOT__v2__DOT__waveform_dac__DOT__bitsum__0.assign(vlSelfRef.test_sid__DOT__uut__DOT__v2__DOT__waveform_dac__DOT__bitsum);
    vlSelfRef.__Vtrigprevexpr___TOP__test_sid__DOT__uut__DOT__v2__DOT__envelope_dac__DOT__bitsum__0.assign(vlSelfRef.test_sid__DOT__uut__DOT__v2__DOT__envelope_dac__DOT__bitsum);
    vlSelfRef.__Vtrigprevexpr___TOP__test_sid__DOT__uut__DOT__v3__DOT__waveform_dac__DOT__bitsum__0.assign(vlSelfRef.test_sid__DOT__uut__DOT__v3__DOT__waveform_dac__DOT__bitsum);
    vlSelfRef.__Vtrigprevexpr___TOP__test_sid__DOT__uut__DOT__v3__DOT__envelope_dac__DOT__bitsum__0.assign(vlSelfRef.test_sid__DOT__uut__DOT__v3__DOT__envelope_dac__DOT__bitsum);
    if (VL_UNLIKELY(((1U & (~ (IData)(vlSelfRef.__VstlDidInit)))))) {
        vlSelfRef.__VstlDidInit = 1U;
        vlSelfRef.__VstlTriggered[0U] = (1ULL | vlSelfRef.__VstlTriggered[0U]);
        vlSelfRef.__VstlTriggered[0U] = (2ULL | vlSelfRef.__VstlTriggered[0U]);
        vlSelfRef.__VstlTriggered[0U] = (4ULL | vlSelfRef.__VstlTriggered[0U]);
        vlSelfRef.__VstlTriggered[0U] = (8ULL | vlSelfRef.__VstlTriggered[0U]);
        vlSelfRef.__VstlTriggered[0U] = (0x0000000000000010ULL 
                                         | vlSelfRef.__VstlTriggered[0U]);
        vlSelfRef.__VstlTriggered[0U] = (0x0000000000000020ULL 
                                         | vlSelfRef.__VstlTriggered[0U]);
    }
}

VL_ATTR_COLD bool Vtest_sid___024root___trigger_anySet__stl(const VlUnpacked<QData/*63:0*/, 2> &in);

#ifdef VL_DEBUG
VL_ATTR_COLD void Vtest_sid___024root___dump_triggers__stl(const VlUnpacked<QData/*63:0*/, 2> &triggers, const std::string &tag) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vtest_sid___024root___dump_triggers__stl\n"); );
    // Body
    if ((1U & (~ (IData)(Vtest_sid___024root___trigger_anySet__stl(triggers))))) {
        VL_DBG_MSGS("         No '" + tag + "' region triggers active\n");
    }
    if ((1U & (IData)(triggers[0U]))) {
        VL_DBG_MSGS("         '" + tag + "' region trigger index 0 is active: @([hybrid] test_sid.uut.v1.waveform_dac.bitsum)\n");
    }
    if ((1U & (IData)((triggers[0U] >> 1U)))) {
        VL_DBG_MSGS("         '" + tag + "' region trigger index 1 is active: @([hybrid] test_sid.uut.v1.envelope_dac.bitsum)\n");
    }
    if ((1U & (IData)((triggers[0U] >> 2U)))) {
        VL_DBG_MSGS("         '" + tag + "' region trigger index 2 is active: @([hybrid] test_sid.uut.v2.waveform_dac.bitsum)\n");
    }
    if ((1U & (IData)((triggers[0U] >> 3U)))) {
        VL_DBG_MSGS("         '" + tag + "' region trigger index 3 is active: @([hybrid] test_sid.uut.v2.envelope_dac.bitsum)\n");
    }
    if ((1U & (IData)((triggers[0U] >> 4U)))) {
        VL_DBG_MSGS("         '" + tag + "' region trigger index 4 is active: @([hybrid] test_sid.uut.v3.waveform_dac.bitsum)\n");
    }
    if ((1U & (IData)((triggers[0U] >> 5U)))) {
        VL_DBG_MSGS("         '" + tag + "' region trigger index 5 is active: @([hybrid] test_sid.uut.v3.envelope_dac.bitsum)\n");
    }
    if ((1U & (IData)(triggers[1U]))) {
        VL_DBG_MSGS("         '" + tag + "' region trigger index 64 is active: Internal 'stl' trigger - first iteration\n");
    }
}
#endif  // VL_DEBUG

VL_ATTR_COLD bool Vtest_sid___024root___trigger_anySet__stl(const VlUnpacked<QData/*63:0*/, 2> &in) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vtest_sid___024root___trigger_anySet__stl\n"); );
    // Locals
    IData/*31:0*/ n;
    // Body
    n = 0U;
    do {
        if (in[n]) {
            return (1U);
        }
        n = ((IData)(1U) + n);
    } while ((2U > n));
    return (0U);
}

VL_ATTR_COLD void Vtest_sid___024root___stl_sequent__TOP__0(Vtest_sid___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vtest_sid___024root___stl_sequent__TOP__0\n"); );
    Vtest_sid__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
    // Locals
    IData/*31:0*/ test_sid__DOT__uut__DOT__flt__DOT__m;
    test_sid__DOT__uut__DOT__flt__DOT__m = 0;
    IData/*16:0*/ __VdfgRegularize_hebeb780c_0_4;
    __VdfgRegularize_hebeb780c_0_4 = 0;
    // Body
    vlSelfRef.test_sid__DOT__uut__DOT__v1_scaled = 
        (0x003fffffU & VL_SHIFTRS_III(22,22,32, (0x003fffffU 
                                                 & VL_MULS_III(22, vlSelfRef.test_sid__DOT__uut__DOT__v1__DOT__dca_out, 
                                                               (0x003fffffU 
                                                                & VL_EXTENDS_II(22,5, vlSelfRef.test_sid__DOT__uut__DOT__voice_vol[0U])))), 4U));
    vlSelfRef.test_sid__DOT__uut__DOT__v2_scaled = 
        (0x003fffffU & VL_SHIFTRS_III(22,22,32, (0x003fffffU 
                                                 & VL_MULS_III(22, vlSelfRef.test_sid__DOT__uut__DOT__v2__DOT__dca_out, 
                                                               (0x003fffffU 
                                                                & VL_EXTENDS_II(22,5, vlSelfRef.test_sid__DOT__uut__DOT__voice_vol[1U])))), 4U));
    vlSelfRef.test_sid__DOT__uut__DOT__v3_scaled = 
        (0x003fffffU & VL_SHIFTRS_III(22,22,32, (0x003fffffU 
                                                 & VL_MULS_III(22, vlSelfRef.test_sid__DOT__uut__DOT__v3__DOT__dca_out, 
                                                               (0x003fffffU 
                                                                & VL_EXTENDS_II(22,5, vlSelfRef.test_sid__DOT__uut__DOT__voice_vol[2U])))), 4U));
    vlSelfRef.test_sid__DOT__uut__DOT__v1__DOT__adsr__DOT__rate_period 
        = vlSelfRef.test_sid__DOT__uut__DOT__v1__DOT__adsr__DOT__rates
        [(0x0000000fU & ((1U == (IData)(vlSelfRef.test_sid__DOT__uut__DOT__v1__DOT__adsr__DOT__state))
                          ? (vlSelfRef.test_sid__DOT__uut__DOT__voice_ad[0U] 
                             >> 4U) : ((2U == (IData)(vlSelfRef.test_sid__DOT__uut__DOT__v1__DOT__adsr__DOT__state))
                                        ? vlSelfRef.test_sid__DOT__uut__DOT__voice_ad[0U]
                                        : vlSelfRef.test_sid__DOT__uut__DOT__voice_sr[0U])))];
    vlSelfRef.test_sid__DOT__uut__DOT__v2__DOT__adsr__DOT__rate_period 
        = vlSelfRef.test_sid__DOT__uut__DOT__v2__DOT__adsr__DOT__rates
        [(0x0000000fU & ((1U == (IData)(vlSelfRef.test_sid__DOT__uut__DOT__v2__DOT__adsr__DOT__state))
                          ? (vlSelfRef.test_sid__DOT__uut__DOT__voice_ad[1U] 
                             >> 4U) : ((2U == (IData)(vlSelfRef.test_sid__DOT__uut__DOT__v2__DOT__adsr__DOT__state))
                                        ? vlSelfRef.test_sid__DOT__uut__DOT__voice_ad[1U]
                                        : vlSelfRef.test_sid__DOT__uut__DOT__voice_sr[1U])))];
    vlSelfRef.test_sid__DOT__uut__DOT__v3__DOT__adsr__DOT__rate_period 
        = vlSelfRef.test_sid__DOT__uut__DOT__v3__DOT__adsr__DOT__rates
        [(0x0000000fU & ((1U == (IData)(vlSelfRef.test_sid__DOT__uut__DOT__v3__DOT__adsr__DOT__state))
                          ? (vlSelfRef.test_sid__DOT__uut__DOT__voice_ad[2U] 
                             >> 4U) : ((2U == (IData)(vlSelfRef.test_sid__DOT__uut__DOT__v3__DOT__adsr__DOT__state))
                                        ? vlSelfRef.test_sid__DOT__uut__DOT__voice_ad[2U]
                                        : vlSelfRef.test_sid__DOT__uut__DOT__voice_sr[2U])))];
    vlSelfRef.test_sid__DOT__dout = ((0x00000010U & (IData)(vlSelfRef.test_sid__DOT__addr))
                                      ? ((8U & (IData)(vlSelfRef.test_sid__DOT__addr))
                                          ? ((4U & (IData)(vlSelfRef.test_sid__DOT__addr))
                                              ? ((2U 
                                                  & (IData)(vlSelfRef.test_sid__DOT__addr))
                                                  ? 
                                                 ((1U 
                                                   & (IData)(vlSelfRef.test_sid__DOT__addr))
                                                   ? vlSelfRef.test_sid__DOT__uut__DOT__voice_vol[2U]
                                                   : vlSelfRef.test_sid__DOT__uut__DOT__voice_vol[1U])
                                                  : 
                                                 ((1U 
                                                   & (IData)(vlSelfRef.test_sid__DOT__addr))
                                                   ? vlSelfRef.test_sid__DOT__uut__DOT__voice_vol[0U]
                                                   : (IData)(vlSelfRef.test_sid__DOT__uut__DOT__v3__DOT__envelope)))
                                              : ((2U 
                                                  & (IData)(vlSelfRef.test_sid__DOT__addr))
                                                  ? 
                                                 ((1U 
                                                   & (IData)(vlSelfRef.test_sid__DOT__addr))
                                                   ? (IData)(vlSelfRef.test_sid__DOT__uut__DOT__v3__DOT__wave_out)
                                                   : 0U)
                                                  : 
                                                 ((1U 
                                                   & (IData)(vlSelfRef.test_sid__DOT__addr))
                                                   ? 0U
                                                   : (IData)(vlSelfRef.test_sid__DOT__uut__DOT__last_written))))
                                          : (IData)(vlSelfRef.test_sid__DOT__uut__DOT__last_written))
                                      : (IData)(vlSelfRef.test_sid__DOT__uut__DOT__last_written));
    if ((0x00000080U & vlSelfRef.test_sid__DOT__uut__DOT__voice_ctrl[0U])) {
        vlSelfRef.test_sid__DOT__uut__DOT__v1__DOT__comb = 0U;
        vlSelfRef.test_sid__DOT__uut__DOT__v1__DOT__norm 
            = (0x00000fffU & ((0x00000040U & vlSelfRef.test_sid__DOT__uut__DOT__voice_ctrl[0U])
                               ? 0U : ((0x00000020U 
                                        & vlSelfRef.test_sid__DOT__uut__DOT__voice_ctrl[0U])
                                        ? 0U : ((0x00000010U 
                                                 & vlSelfRef.test_sid__DOT__uut__DOT__voice_ctrl[0U])
                                                 ? 0U
                                                 : (IData)(vlSelfRef.test_sid__DOT__uut__DOT__v1__DOT__noise)))));
    } else if ((0x00000040U & vlSelfRef.test_sid__DOT__uut__DOT__voice_ctrl[0U])) {
        if ((0x00000020U & vlSelfRef.test_sid__DOT__uut__DOT__voice_ctrl[0U])) {
            vlSelfRef.test_sid__DOT__uut__DOT__v1__DOT__comb 
                = ((0x00000010U & vlSelfRef.test_sid__DOT__uut__DOT__voice_ctrl[0U])
                    ? (vlSelfRef.test_sid__DOT__uut__DOT__pst_out[0U] 
                       & (- (IData)((IData)(vlSelfRef.test_sid__DOT__uut__DOT__v1__DOT__pulse))))
                    : (vlSelfRef.test_sid__DOT__uut__DOT__ps___05Fout[0U] 
                       & (- (IData)((IData)(vlSelfRef.test_sid__DOT__uut__DOT__v1__DOT__pulse)))));
            vlSelfRef.test_sid__DOT__uut__DOT__v1__DOT__norm 
                = (0x00000fffU & 0U);
        } else if ((0x00000010U & vlSelfRef.test_sid__DOT__uut__DOT__voice_ctrl[0U])) {
            vlSelfRef.test_sid__DOT__uut__DOT__v1__DOT__comb 
                = (vlSelfRef.test_sid__DOT__uut__DOT__p_t_out[0U] 
                   & (- (IData)((IData)(vlSelfRef.test_sid__DOT__uut__DOT__v1__DOT__pulse))));
            vlSelfRef.test_sid__DOT__uut__DOT__v1__DOT__norm 
                = (0x00000fffU & 0U);
        } else {
            vlSelfRef.test_sid__DOT__uut__DOT__v1__DOT__comb = 0U;
            vlSelfRef.test_sid__DOT__uut__DOT__v1__DOT__norm 
                = (0x00000fffU & (- (IData)((IData)(vlSelfRef.test_sid__DOT__uut__DOT__v1__DOT__pulse))));
        }
    } else if ((0x00000020U & vlSelfRef.test_sid__DOT__uut__DOT__voice_ctrl[0U])) {
        if ((0x00000010U & vlSelfRef.test_sid__DOT__uut__DOT__voice_ctrl[0U])) {
            vlSelfRef.test_sid__DOT__uut__DOT__v1__DOT__comb 
                = vlSelfRef.test_sid__DOT__uut__DOT___st_out[0U];
            vlSelfRef.test_sid__DOT__uut__DOT__v1__DOT__norm 
                = (0x00000fffU & 0U);
        } else {
            vlSelfRef.test_sid__DOT__uut__DOT__v1__DOT__comb = 0U;
            vlSelfRef.test_sid__DOT__uut__DOT__v1__DOT__norm 
                = (0x00000fffU & (IData)(vlSelfRef.test_sid__DOT__uut__DOT__v1__DOT__saw_tri));
        }
    } else {
        vlSelfRef.test_sid__DOT__uut__DOT__v1__DOT__comb = 0U;
        vlSelfRef.test_sid__DOT__uut__DOT__v1__DOT__norm 
            = (0x00000fffU & ((0x00000010U & vlSelfRef.test_sid__DOT__uut__DOT__voice_ctrl[0U])
                               ? VL_SHIFTL_III(12,12,32, (IData)(vlSelfRef.test_sid__DOT__uut__DOT__v1__DOT__saw_tri), 1U)
                               : 0U));
    }
    if ((0x00000080U & vlSelfRef.test_sid__DOT__uut__DOT__voice_ctrl[1U])) {
        vlSelfRef.test_sid__DOT__uut__DOT__v2__DOT__comb = 0U;
        vlSelfRef.test_sid__DOT__uut__DOT__v2__DOT__norm 
            = (0x00000fffU & ((0x00000040U & vlSelfRef.test_sid__DOT__uut__DOT__voice_ctrl[1U])
                               ? 0U : ((0x00000020U 
                                        & vlSelfRef.test_sid__DOT__uut__DOT__voice_ctrl[1U])
                                        ? 0U : ((0x00000010U 
                                                 & vlSelfRef.test_sid__DOT__uut__DOT__voice_ctrl[1U])
                                                 ? 0U
                                                 : (IData)(vlSelfRef.test_sid__DOT__uut__DOT__v2__DOT__noise)))));
    } else if ((0x00000040U & vlSelfRef.test_sid__DOT__uut__DOT__voice_ctrl[1U])) {
        if ((0x00000020U & vlSelfRef.test_sid__DOT__uut__DOT__voice_ctrl[1U])) {
            vlSelfRef.test_sid__DOT__uut__DOT__v2__DOT__comb 
                = ((0x00000010U & vlSelfRef.test_sid__DOT__uut__DOT__voice_ctrl[1U])
                    ? (vlSelfRef.test_sid__DOT__uut__DOT__pst_out[1U] 
                       & (- (IData)((IData)(vlSelfRef.test_sid__DOT__uut__DOT__v2__DOT__pulse))))
                    : (vlSelfRef.test_sid__DOT__uut__DOT__ps___05Fout[1U] 
                       & (- (IData)((IData)(vlSelfRef.test_sid__DOT__uut__DOT__v2__DOT__pulse)))));
            vlSelfRef.test_sid__DOT__uut__DOT__v2__DOT__norm 
                = (0x00000fffU & 0U);
        } else if ((0x00000010U & vlSelfRef.test_sid__DOT__uut__DOT__voice_ctrl[1U])) {
            vlSelfRef.test_sid__DOT__uut__DOT__v2__DOT__comb 
                = (vlSelfRef.test_sid__DOT__uut__DOT__p_t_out[1U] 
                   & (- (IData)((IData)(vlSelfRef.test_sid__DOT__uut__DOT__v2__DOT__pulse))));
            vlSelfRef.test_sid__DOT__uut__DOT__v2__DOT__norm 
                = (0x00000fffU & 0U);
        } else {
            vlSelfRef.test_sid__DOT__uut__DOT__v2__DOT__comb = 0U;
            vlSelfRef.test_sid__DOT__uut__DOT__v2__DOT__norm 
                = (0x00000fffU & (- (IData)((IData)(vlSelfRef.test_sid__DOT__uut__DOT__v2__DOT__pulse))));
        }
    } else if ((0x00000020U & vlSelfRef.test_sid__DOT__uut__DOT__voice_ctrl[1U])) {
        if ((0x00000010U & vlSelfRef.test_sid__DOT__uut__DOT__voice_ctrl[1U])) {
            vlSelfRef.test_sid__DOT__uut__DOT__v2__DOT__comb 
                = vlSelfRef.test_sid__DOT__uut__DOT___st_out[1U];
            vlSelfRef.test_sid__DOT__uut__DOT__v2__DOT__norm 
                = (0x00000fffU & 0U);
        } else {
            vlSelfRef.test_sid__DOT__uut__DOT__v2__DOT__comb = 0U;
            vlSelfRef.test_sid__DOT__uut__DOT__v2__DOT__norm 
                = (0x00000fffU & (IData)(vlSelfRef.test_sid__DOT__uut__DOT__v2__DOT__saw_tri));
        }
    } else {
        vlSelfRef.test_sid__DOT__uut__DOT__v2__DOT__comb = 0U;
        vlSelfRef.test_sid__DOT__uut__DOT__v2__DOT__norm 
            = (0x00000fffU & ((0x00000010U & vlSelfRef.test_sid__DOT__uut__DOT__voice_ctrl[1U])
                               ? VL_SHIFTL_III(12,12,32, (IData)(vlSelfRef.test_sid__DOT__uut__DOT__v2__DOT__saw_tri), 1U)
                               : 0U));
    }
    if ((0x00000080U & vlSelfRef.test_sid__DOT__uut__DOT__voice_ctrl[2U])) {
        vlSelfRef.test_sid__DOT__uut__DOT__v3__DOT__comb = 0U;
        vlSelfRef.test_sid__DOT__uut__DOT__v3__DOT__norm 
            = (0x00000fffU & ((0x00000040U & vlSelfRef.test_sid__DOT__uut__DOT__voice_ctrl[2U])
                               ? 0U : ((0x00000020U 
                                        & vlSelfRef.test_sid__DOT__uut__DOT__voice_ctrl[2U])
                                        ? 0U : ((0x00000010U 
                                                 & vlSelfRef.test_sid__DOT__uut__DOT__voice_ctrl[2U])
                                                 ? 0U
                                                 : (IData)(vlSelfRef.test_sid__DOT__uut__DOT__v3__DOT__noise)))));
    } else if ((0x00000040U & vlSelfRef.test_sid__DOT__uut__DOT__voice_ctrl[2U])) {
        if ((0x00000020U & vlSelfRef.test_sid__DOT__uut__DOT__voice_ctrl[2U])) {
            vlSelfRef.test_sid__DOT__uut__DOT__v3__DOT__comb 
                = ((0x00000010U & vlSelfRef.test_sid__DOT__uut__DOT__voice_ctrl[2U])
                    ? (vlSelfRef.test_sid__DOT__uut__DOT__pst_out[2U] 
                       & (- (IData)((IData)(vlSelfRef.test_sid__DOT__uut__DOT__v3__DOT__pulse))))
                    : (vlSelfRef.test_sid__DOT__uut__DOT__ps___05Fout[2U] 
                       & (- (IData)((IData)(vlSelfRef.test_sid__DOT__uut__DOT__v3__DOT__pulse)))));
            vlSelfRef.test_sid__DOT__uut__DOT__v3__DOT__norm 
                = (0x00000fffU & 0U);
        } else if ((0x00000010U & vlSelfRef.test_sid__DOT__uut__DOT__voice_ctrl[2U])) {
            vlSelfRef.test_sid__DOT__uut__DOT__v3__DOT__comb 
                = (vlSelfRef.test_sid__DOT__uut__DOT__p_t_out[2U] 
                   & (- (IData)((IData)(vlSelfRef.test_sid__DOT__uut__DOT__v3__DOT__pulse))));
            vlSelfRef.test_sid__DOT__uut__DOT__v3__DOT__norm 
                = (0x00000fffU & 0U);
        } else {
            vlSelfRef.test_sid__DOT__uut__DOT__v3__DOT__comb = 0U;
            vlSelfRef.test_sid__DOT__uut__DOT__v3__DOT__norm 
                = (0x00000fffU & (- (IData)((IData)(vlSelfRef.test_sid__DOT__uut__DOT__v3__DOT__pulse))));
        }
    } else if ((0x00000020U & vlSelfRef.test_sid__DOT__uut__DOT__voice_ctrl[2U])) {
        if ((0x00000010U & vlSelfRef.test_sid__DOT__uut__DOT__voice_ctrl[2U])) {
            vlSelfRef.test_sid__DOT__uut__DOT__v3__DOT__comb 
                = vlSelfRef.test_sid__DOT__uut__DOT___st_out[2U];
            vlSelfRef.test_sid__DOT__uut__DOT__v3__DOT__norm 
                = (0x00000fffU & 0U);
        } else {
            vlSelfRef.test_sid__DOT__uut__DOT__v3__DOT__comb = 0U;
            vlSelfRef.test_sid__DOT__uut__DOT__v3__DOT__norm 
                = (0x00000fffU & (IData)(vlSelfRef.test_sid__DOT__uut__DOT__v3__DOT__saw_tri));
        }
    } else {
        vlSelfRef.test_sid__DOT__uut__DOT__v3__DOT__comb = 0U;
        vlSelfRef.test_sid__DOT__uut__DOT__v3__DOT__norm 
            = (0x00000fffU & ((0x00000010U & vlSelfRef.test_sid__DOT__uut__DOT__voice_ctrl[2U])
                               ? VL_SHIFTL_III(12,12,32, (IData)(vlSelfRef.test_sid__DOT__uut__DOT__v3__DOT__saw_tri), 1U)
                               : 0U));
    }
    vlSelfRef.test_sid__DOT__uut__DOT____Vcellout__v1__acc_t 
        = ((0x00000800U & (vlSelfRef.test_sid__DOT__uut__DOT__v1__DOT__oscillator 
                           >> 0x0000000cU)) | (0x000007ffU 
                                               & ((vlSelfRef.test_sid__DOT__uut__DOT__v1__DOT__oscillator 
                                                   >> 0x0000000cU) 
                                                  ^ 
                                                  (- (IData)(
                                                             (1U 
                                                              & ((~ 
                                                                  (vlSelfRef.test_sid__DOT__uut__DOT__voice_ctrl[0U] 
                                                                   >> 5U)) 
                                                                 & (((~ 
                                                                      (vlSelfRef.test_sid__DOT__uut__DOT__v3__DOT__oscillator 
                                                                       >> 0x00000017U)) 
                                                                     & (vlSelfRef.test_sid__DOT__uut__DOT__voice_ctrl[0U] 
                                                                        >> 2U)) 
                                                                    ^ 
                                                                    (vlSelfRef.test_sid__DOT__uut__DOT__v1__DOT__oscillator 
                                                                     >> 0x00000017U)))))))));
    vlSelfRef.test_sid__DOT__uut__DOT____Vcellout__v2__acc_t 
        = ((0x00000800U & (vlSelfRef.test_sid__DOT__uut__DOT__v2__DOT__oscillator 
                           >> 0x0000000cU)) | (0x000007ffU 
                                               & ((vlSelfRef.test_sid__DOT__uut__DOT__v2__DOT__oscillator 
                                                   >> 0x0000000cU) 
                                                  ^ 
                                                  (- (IData)(
                                                             (1U 
                                                              & ((~ 
                                                                  (vlSelfRef.test_sid__DOT__uut__DOT__voice_ctrl[1U] 
                                                                   >> 5U)) 
                                                                 & (((~ 
                                                                      (vlSelfRef.test_sid__DOT__uut__DOT__v1__DOT__oscillator 
                                                                       >> 0x00000017U)) 
                                                                     & (vlSelfRef.test_sid__DOT__uut__DOT__voice_ctrl[1U] 
                                                                        >> 2U)) 
                                                                    ^ 
                                                                    (vlSelfRef.test_sid__DOT__uut__DOT__v2__DOT__oscillator 
                                                                     >> 0x00000017U)))))))));
    vlSelfRef.test_sid__DOT__uut__DOT____Vcellout__v3__acc_t 
        = ((0x00000800U & (vlSelfRef.test_sid__DOT__uut__DOT__v3__DOT__oscillator 
                           >> 0x0000000cU)) | (0x000007ffU 
                                               & ((vlSelfRef.test_sid__DOT__uut__DOT__v3__DOT__oscillator 
                                                   >> 0x0000000cU) 
                                                  ^ 
                                                  (- (IData)(
                                                             (1U 
                                                              & ((~ 
                                                                  (vlSelfRef.test_sid__DOT__uut__DOT__voice_ctrl[2U] 
                                                                   >> 5U)) 
                                                                 & (((~ 
                                                                      (vlSelfRef.test_sid__DOT__uut__DOT__v2__DOT__oscillator 
                                                                       >> 0x00000017U)) 
                                                                     & (vlSelfRef.test_sid__DOT__uut__DOT__voice_ctrl[2U] 
                                                                        >> 2U)) 
                                                                    ^ 
                                                                    (vlSelfRef.test_sid__DOT__uut__DOT__v3__DOT__oscillator 
                                                                     >> 0x00000017U)))))))));
    vlSelfRef.test_sid__DOT__uut__DOT__v1__DOT__envelope_dac__DOT__bitsum[0U] 
        = (0x00000fffU & ((IData)(8U) + ((1U & (IData)(vlSelfRef.test_sid__DOT__uut__DOT__v1__DOT__envelope))
                                          ? vlSelfRef.test_sid__DOT__uut__DOT__v1__DOT__envelope_dac__DOT__bitval[0U]
                                          : 0U)));
    vlSelfRef.test_sid__DOT__uut__DOT__v2__DOT__envelope_dac__DOT__bitsum[0U] 
        = (0x00000fffU & ((IData)(8U) + ((1U & (IData)(vlSelfRef.test_sid__DOT__uut__DOT__v2__DOT__envelope))
                                          ? vlSelfRef.test_sid__DOT__uut__DOT__v2__DOT__envelope_dac__DOT__bitval[0U]
                                          : 0U)));
    vlSelfRef.test_sid__DOT__uut__DOT__v3__DOT__envelope_dac__DOT__bitsum[0U] 
        = (0x00000fffU & ((IData)(8U) + ((1U & (IData)(vlSelfRef.test_sid__DOT__uut__DOT__v3__DOT__envelope))
                                          ? vlSelfRef.test_sid__DOT__uut__DOT__v3__DOT__envelope_dac__DOT__bitval[0U]
                                          : 0U)));
    test_sid__DOT__uut__DOT__flt__DOT__m = VL_MULS_III(32, 
                                                       VL_EXTENDS_II(32,16, (IData)(vlSelfRef.test_sid__DOT__uut__DOT__flt__DOT__a)), 
                                                       VL_EXTENDS_II(32,16, (IData)(vlSelfRef.test_sid__DOT__uut__DOT__flt__DOT__b)));
    vlSelfRef.test_sid__DOT__uut__DOT__acc_t[0U] = vlSelfRef.test_sid__DOT__uut__DOT____Vcellout__v1__acc_t;
    vlSelfRef.test_sid__DOT__uut__DOT__acc_t[1U] = vlSelfRef.test_sid__DOT__uut__DOT____Vcellout__v2__acc_t;
    vlSelfRef.test_sid__DOT__uut__DOT__acc_t[2U] = vlSelfRef.test_sid__DOT__uut__DOT____Vcellout__v3__acc_t;
    vlSelfRef.test_sid__DOT__uut__DOT__flt__DOT__o 
        = ((IData)(vlSelfRef.test_sid__DOT__uut__DOT__flt__DOT__s)
            ? (vlSelfRef.test_sid__DOT__uut__DOT__flt__DOT__c 
               - test_sid__DOT__uut__DOT__flt__DOT__m)
            : (vlSelfRef.test_sid__DOT__uut__DOT__flt__DOT__c 
               + test_sid__DOT__uut__DOT__flt__DOT__m));
    vlSelfRef.test_sid__DOT__uut__DOT__v1__DOT__waveform_dac__DOT__bitsum[0U] 
        = (0x0000ffffU & ((IData)(8U) + ((1U & (IData)(vlSelfRef.test_sid__DOT__uut__DOT__v1__DOT__norm))
                                          ? vlSelfRef.test_sid__DOT__uut__DOT__v1__DOT__waveform_dac__DOT__bitval[0U]
                                          : 0U)));
    vlSelfRef.test_sid__DOT__uut__DOT__v2__DOT__waveform_dac__DOT__bitsum[0U] 
        = (0x0000ffffU & ((IData)(8U) + ((1U & (IData)(vlSelfRef.test_sid__DOT__uut__DOT__v2__DOT__norm))
                                          ? vlSelfRef.test_sid__DOT__uut__DOT__v2__DOT__waveform_dac__DOT__bitval[0U]
                                          : 0U)));
    vlSelfRef.test_sid__DOT__uut__DOT__v3__DOT__waveform_dac__DOT__bitsum[0U] 
        = (0x0000ffffU & ((IData)(8U) + ((1U & (IData)(vlSelfRef.test_sid__DOT__uut__DOT__v3__DOT__norm))
                                          ? vlSelfRef.test_sid__DOT__uut__DOT__v3__DOT__waveform_dac__DOT__bitval[0U]
                                          : 0U)));
    vlSelfRef.test_sid__DOT__uut__DOT__flt__DOT__vhp_next 
        = (0x0000ffffU & ((1U & VL_REDXOR_32((3U & 
                                              (vlSelfRef.test_sid__DOT__uut__DOT__flt__DOT__o 
                                               >> 0x00000019U))))
                           ? ((0x00008000U & (vlSelfRef.test_sid__DOT__uut__DOT__flt__DOT__o 
                                              >> 0x0000000bU)) 
                              | (0x00007fffU & (- (IData)(
                                                          (1U 
                                                           & (vlSelfRef.test_sid__DOT__uut__DOT__flt__DOT__o 
                                                              >> 0x00000019U))))))
                           : (vlSelfRef.test_sid__DOT__uut__DOT__flt__DOT__o 
                              >> 0x0000000aU)));
    vlSelfRef.__VdfgRegularize_hebeb780c_0_3 = (0x0001ffffU 
                                                & (VL_EXTENDS_II(17,16, (IData)(vlSelfRef.test_sid__DOT__uut__DOT__flt__DOT__vlp)) 
                                                   + 
                                                   VL_SHIFTRS_III(17,32,32, vlSelfRef.test_sid__DOT__uut__DOT__flt__DOT__o, 0x00000011U)));
    __VdfgRegularize_hebeb780c_0_4 = (0x0001ffffU & 
                                      (VL_EXTENDS_II(17,16, (IData)(vlSelfRef.test_sid__DOT__uut__DOT__flt__DOT__vbp)) 
                                       + VL_SHIFTRS_III(17,32,32, vlSelfRef.test_sid__DOT__uut__DOT__flt__DOT__o, 0x00000011U)));
    vlSelfRef.test_sid__DOT__uut__DOT__flt__DOT__vbp_next 
        = (0x0000ffffU & ((1U & VL_REDXOR_32((3U & 
                                              (__VdfgRegularize_hebeb780c_0_4 
                                               >> 0x0000000fU))))
                           ? ((0x00008000U & (__VdfgRegularize_hebeb780c_0_4 
                                              >> 1U)) 
                              | (0x00007fffU & (- (IData)(
                                                          (1U 
                                                           & (__VdfgRegularize_hebeb780c_0_4 
                                                              >> 0x0000000fU))))))
                           : __VdfgRegularize_hebeb780c_0_4));
}

void Vtest_sid___024root___act_sequent__TOP__1(Vtest_sid___024root* vlSelf);
void Vtest_sid___024root___act_sequent__TOP__2(Vtest_sid___024root* vlSelf);
void Vtest_sid___024root___act_sequent__TOP__3(Vtest_sid___024root* vlSelf);
void Vtest_sid___024root___act_sequent__TOP__4(Vtest_sid___024root* vlSelf);
void Vtest_sid___024root___act_sequent__TOP__5(Vtest_sid___024root* vlSelf);
void Vtest_sid___024root___act_sequent__TOP__6(Vtest_sid___024root* vlSelf);

VL_ATTR_COLD void Vtest_sid___024root___eval_stl(Vtest_sid___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vtest_sid___024root___eval_stl\n"); );
    Vtest_sid__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
    // Body
    if ((1ULL & vlSelfRef.__VstlTriggered[1U])) {
        Vtest_sid___024root___stl_sequent__TOP__0(vlSelf);
    }
    if (((1ULL & vlSelfRef.__VstlTriggered[1U]) | (2ULL 
                                                   & vlSelfRef.__VstlTriggered[0U]))) {
        Vtest_sid___024root___act_sequent__TOP__1(vlSelf);
    }
    if (((1ULL & vlSelfRef.__VstlTriggered[1U]) | (8ULL 
                                                   & vlSelfRef.__VstlTriggered[0U]))) {
        Vtest_sid___024root___act_sequent__TOP__2(vlSelf);
    }
    if (((1ULL & vlSelfRef.__VstlTriggered[1U]) | (0x0000000000000020ULL 
                                                   & vlSelfRef.__VstlTriggered[0U]))) {
        Vtest_sid___024root___act_sequent__TOP__3(vlSelf);
    }
    if ((1ULL & (vlSelfRef.__VstlTriggered[1U] | vlSelfRef.__VstlTriggered[0U]))) {
        Vtest_sid___024root___act_sequent__TOP__4(vlSelf);
    }
    if (((1ULL & vlSelfRef.__VstlTriggered[1U]) | (4ULL 
                                                   & vlSelfRef.__VstlTriggered[0U]))) {
        Vtest_sid___024root___act_sequent__TOP__5(vlSelf);
    }
    if (((1ULL & vlSelfRef.__VstlTriggered[1U]) | (0x0000000000000010ULL 
                                                   & vlSelfRef.__VstlTriggered[0U]))) {
        Vtest_sid___024root___act_sequent__TOP__6(vlSelf);
    }
}

VL_ATTR_COLD bool Vtest_sid___024root___eval_phase__stl(Vtest_sid___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vtest_sid___024root___eval_phase__stl\n"); );
    Vtest_sid__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
    // Locals
    CData/*0:0*/ __VstlExecute;
    // Body
    Vtest_sid___024root___eval_triggers_vec__stl(vlSelf);
#ifdef VL_DEBUG
    if (VL_UNLIKELY(vlSymsp->_vm_contextp__->debug())) {
        Vtest_sid___024root___dump_triggers__stl(vlSelfRef.__VstlTriggered, "stl"s);
    }
#endif
    __VstlExecute = Vtest_sid___024root___trigger_anySet__stl(vlSelfRef.__VstlTriggered);
    if (__VstlExecute) {
        Vtest_sid___024root___eval_stl(vlSelf);
    }
    return (__VstlExecute);
}

bool Vtest_sid___024root___trigger_anySet__act(const VlUnpacked<QData/*63:0*/, 1> &in);

#ifdef VL_DEBUG
VL_ATTR_COLD void Vtest_sid___024root___dump_triggers__act(const VlUnpacked<QData/*63:0*/, 1> &triggers, const std::string &tag) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vtest_sid___024root___dump_triggers__act\n"); );
    // Body
    if ((1U & (~ (IData)(Vtest_sid___024root___trigger_anySet__act(triggers))))) {
        VL_DBG_MSGS("         No '" + tag + "' region triggers active\n");
    }
    if ((1U & (IData)(triggers[0U]))) {
        VL_DBG_MSGS("         '" + tag + "' region trigger index 0 is active: @([hybrid] test_sid.uut.v1.waveform_dac.bitsum)\n");
    }
    if ((1U & (IData)((triggers[0U] >> 1U)))) {
        VL_DBG_MSGS("         '" + tag + "' region trigger index 1 is active: @([hybrid] test_sid.uut.v1.envelope_dac.bitsum)\n");
    }
    if ((1U & (IData)((triggers[0U] >> 2U)))) {
        VL_DBG_MSGS("         '" + tag + "' region trigger index 2 is active: @([hybrid] test_sid.uut.v2.waveform_dac.bitsum)\n");
    }
    if ((1U & (IData)((triggers[0U] >> 3U)))) {
        VL_DBG_MSGS("         '" + tag + "' region trigger index 3 is active: @([hybrid] test_sid.uut.v2.envelope_dac.bitsum)\n");
    }
    if ((1U & (IData)((triggers[0U] >> 4U)))) {
        VL_DBG_MSGS("         '" + tag + "' region trigger index 4 is active: @([hybrid] test_sid.uut.v3.waveform_dac.bitsum)\n");
    }
    if ((1U & (IData)((triggers[0U] >> 5U)))) {
        VL_DBG_MSGS("         '" + tag + "' region trigger index 5 is active: @([hybrid] test_sid.uut.v3.envelope_dac.bitsum)\n");
    }
    if ((1U & (IData)((triggers[0U] >> 6U)))) {
        VL_DBG_MSGS("         '" + tag + "' region trigger index 6 is active: @(posedge test_sid.clk)\n");
    }
    if ((1U & (IData)((triggers[0U] >> 7U)))) {
        VL_DBG_MSGS("         '" + tag + "' region trigger index 7 is active: @([true] __VdlySched.awaitingCurrentTime())\n");
    }
}
#endif  // VL_DEBUG

VL_ATTR_COLD void Vtest_sid___024root___ctor_var_reset(Vtest_sid___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vtest_sid___024root___ctor_var_reset\n"); );
    Vtest_sid__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
    // Body
    const uint64_t __VscopeHash = VL_MURMUR64_HASH(vlSelf->vlNamep);
    vlSelf->test_sid__DOT__rst = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 1103810786431208343ull);
    vlSelf->test_sid__DOT__ce_1m = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 17710718159130250291ull);
    vlSelf->test_sid__DOT__mode = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 12566933562644065950ull);
    vlSelf->test_sid__DOT__cs = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 12580480530138051659ull);
    vlSelf->test_sid__DOT__we = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 2569743808549579901ull);
    vlSelf->test_sid__DOT__addr = VL_SCOPED_RAND_RESET_I(5, __VscopeHash, 745407602197338609ull);
    vlSelf->test_sid__DOT__din = VL_SCOPED_RAND_RESET_I(8, __VscopeHash, 5394316578225610792ull);
    vlSelf->test_sid__DOT__dout = VL_SCOPED_RAND_RESET_I(8, __VscopeHash, 3127027076219006780ull);
    vlSelf->test_sid__DOT__audio_out = VL_SCOPED_RAND_RESET_I(18, __VscopeHash, 16733943171273205459ull);
    vlSelf->test_sid__DOT__uut__DOT____Vlvbound_h0706378b__0 = 0;
    vlSelf->test_sid__DOT__uut__DOT____Vlvbound_h061f25e8__0 = 0;
    vlSelf->test_sid__DOT__uut__DOT____Vlvbound_h49d001ad__0 = 0;
    vlSelf->test_sid__DOT__uut__DOT____Vlvbound_h9cc45699__0 = 0;
    for (int __Vi0 = 0; __Vi0 < 3; ++__Vi0) {
        vlSelf->test_sid__DOT__uut__DOT__voice_freq[__Vi0] = VL_SCOPED_RAND_RESET_I(16, __VscopeHash, 9960805087068973466ull);
    }
    for (int __Vi0 = 0; __Vi0 < 3; ++__Vi0) {
        vlSelf->test_sid__DOT__uut__DOT__voice_pw[__Vi0] = VL_SCOPED_RAND_RESET_I(12, __VscopeHash, 6358663410936306471ull);
    }
    for (int __Vi0 = 0; __Vi0 < 3; ++__Vi0) {
        vlSelf->test_sid__DOT__uut__DOT__voice_ctrl[__Vi0] = VL_SCOPED_RAND_RESET_I(8, __VscopeHash, 2189814824848641ull);
    }
    for (int __Vi0 = 0; __Vi0 < 3; ++__Vi0) {
        vlSelf->test_sid__DOT__uut__DOT__voice_ad[__Vi0] = VL_SCOPED_RAND_RESET_I(8, __VscopeHash, 3093611097283334000ull);
    }
    for (int __Vi0 = 0; __Vi0 < 3; ++__Vi0) {
        vlSelf->test_sid__DOT__uut__DOT__voice_sr[__Vi0] = VL_SCOPED_RAND_RESET_I(8, __VscopeHash, 382849858621823ull);
    }
    for (int __Vi0 = 0; __Vi0 < 3; ++__Vi0) {
        vlSelf->test_sid__DOT__uut__DOT__voice_vol[__Vi0] = VL_SCOPED_RAND_RESET_I(4, __VscopeHash, 17941212507984946988ull);
    }
    vlSelf->test_sid__DOT__uut__DOT__filter_fc = VL_SCOPED_RAND_RESET_I(11, __VscopeHash, 17524163796150581160ull);
    vlSelf->test_sid__DOT__uut__DOT__filter_res_filt = VL_SCOPED_RAND_RESET_I(8, __VscopeHash, 10897913090734585994ull);
    vlSelf->test_sid__DOT__uut__DOT__filter_mode_vol = VL_SCOPED_RAND_RESET_I(8, __VscopeHash, 14272914912532775905ull);
    vlSelf->test_sid__DOT__uut__DOT__last_written = VL_SCOPED_RAND_RESET_I(8, __VscopeHash, 9166117259346428342ull);
    for (int __Vi0 = 0; __Vi0 < 6; ++__Vi0) {
        vlSelf->test_sid__DOT__uut__DOT___st_out[__Vi0] = VL_SCOPED_RAND_RESET_I(8, __VscopeHash, 5036566745413827816ull);
    }
    for (int __Vi0 = 0; __Vi0 < 6; ++__Vi0) {
        vlSelf->test_sid__DOT__uut__DOT__p_t_out[__Vi0] = VL_SCOPED_RAND_RESET_I(8, __VscopeHash, 9392628927856056605ull);
    }
    for (int __Vi0 = 0; __Vi0 < 6; ++__Vi0) {
        vlSelf->test_sid__DOT__uut__DOT__ps___05Fout[__Vi0] = VL_SCOPED_RAND_RESET_I(8, __VscopeHash, 10066770439655156428ull);
    }
    for (int __Vi0 = 0; __Vi0 < 6; ++__Vi0) {
        vlSelf->test_sid__DOT__uut__DOT__pst_out[__Vi0] = VL_SCOPED_RAND_RESET_I(8, __VscopeHash, 3910692397855142595ull);
    }
    for (int __Vi0 = 0; __Vi0 < 3; ++__Vi0) {
        vlSelf->test_sid__DOT__uut__DOT__acc_t[__Vi0] = VL_SCOPED_RAND_RESET_I(12, __VscopeHash, 6524169061885767068ull);
    }
    vlSelf->test_sid__DOT__uut__DOT____Vcellout__v1__acc_t = 0;
    vlSelf->test_sid__DOT__uut__DOT____Vcellout__v2__acc_t = 0;
    vlSelf->test_sid__DOT__uut__DOT____Vcellout__v3__acc_t = 0;
    vlSelf->test_sid__DOT__uut__DOT__tbl_state = VL_SCOPED_RAND_RESET_I(4, __VscopeHash, 15908104279349347620ull);
    vlSelf->test_sid__DOT__uut__DOT__f___05Fst_out = VL_SCOPED_RAND_RESET_I(8, __VscopeHash, 1392576086571874455ull);
    vlSelf->test_sid__DOT__uut__DOT__f_p_t_out = VL_SCOPED_RAND_RESET_I(8, __VscopeHash, 6979462163942965628ull);
    vlSelf->test_sid__DOT__uut__DOT__f_ps___05Fout = VL_SCOPED_RAND_RESET_I(8, __VscopeHash, 5457355537294882924ull);
    vlSelf->test_sid__DOT__uut__DOT__f_pst_out = VL_SCOPED_RAND_RESET_I(8, __VscopeHash, 6955032979739294560ull);
    vlSelf->test_sid__DOT__uut__DOT__f_acc_t = VL_SCOPED_RAND_RESET_I(12, __VscopeHash, 1773299918421648981ull);
    vlSelf->test_sid__DOT__uut__DOT__v1_scaled = VL_SCOPED_RAND_RESET_I(22, __VscopeHash, 15828340528074238232ull);
    vlSelf->test_sid__DOT__uut__DOT__v2_scaled = VL_SCOPED_RAND_RESET_I(22, __VscopeHash, 5026988934840978103ull);
    vlSelf->test_sid__DOT__uut__DOT__v3_scaled = VL_SCOPED_RAND_RESET_I(22, __VscopeHash, 1242323784573289597ull);
    vlSelf->test_sid__DOT__uut__DOT__v1__DOT__envelope = VL_SCOPED_RAND_RESET_I(8, __VscopeHash, 10041030987051812468ull);
    vlSelf->test_sid__DOT__uut__DOT__v1__DOT__oscillator = VL_SCOPED_RAND_RESET_I(24, __VscopeHash, 7501212498388204556ull);
    vlSelf->test_sid__DOT__uut__DOT__v1__DOT__osc_msb_in_prv = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 382543270218852610ull);
    vlSelf->test_sid__DOT__uut__DOT__v1__DOT__noise = VL_SCOPED_RAND_RESET_I(12, __VscopeHash, 6224221420381444894ull);
    vlSelf->test_sid__DOT__uut__DOT__v1__DOT__saw_tri = VL_SCOPED_RAND_RESET_I(12, __VscopeHash, 14357391814147826934ull);
    vlSelf->test_sid__DOT__uut__DOT__v1__DOT__pulse = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 9682990936470326582ull);
    vlSelf->test_sid__DOT__uut__DOT__v1__DOT__norm = VL_SCOPED_RAND_RESET_I(12, __VscopeHash, 7846804646891455568ull);
    vlSelf->test_sid__DOT__uut__DOT__v1__DOT__comb = VL_SCOPED_RAND_RESET_I(8, __VscopeHash, 14719468414940684650ull);
    vlSelf->test_sid__DOT__uut__DOT__v1__DOT__norm_dac = VL_SCOPED_RAND_RESET_I(12, __VscopeHash, 3162033103018232514ull);
    vlSelf->test_sid__DOT__uut__DOT__v1__DOT__wave_out = VL_SCOPED_RAND_RESET_I(8, __VscopeHash, 13506833849591312411ull);
    vlSelf->test_sid__DOT__uut__DOT__v1__DOT__dca_out = VL_SCOPED_RAND_RESET_I(22, __VscopeHash, 18139055380562770491ull);
    vlSelf->test_sid__DOT__uut__DOT__v1__DOT__adsr__DOT__state = VL_SCOPED_RAND_RESET_I(2, __VscopeHash, 17725991625650034773ull);
    for (int __Vi0 = 0; __Vi0 < 16; ++__Vi0) {
        vlSelf->test_sid__DOT__uut__DOT__v1__DOT__adsr__DOT__rates[__Vi0] = VL_SCOPED_RAND_RESET_I(15, __VscopeHash, 10597032459855881768ull);
    }
    vlSelf->test_sid__DOT__uut__DOT__v1__DOT__adsr__DOT__rate_period = VL_SCOPED_RAND_RESET_I(15, __VscopeHash, 10672203780759575170ull);
    for (int __Vi0 = 0; __Vi0 < 12; ++__Vi0) {
        vlSelf->test_sid__DOT__uut__DOT__v1__DOT__waveform_dac__DOT__bitval[__Vi0] = VL_SCOPED_RAND_RESET_I(16, __VscopeHash, 1059965033351886997ull);
    }
    for (int __Vi0 = 0; __Vi0 < 12; ++__Vi0) {
        vlSelf->test_sid__DOT__uut__DOT__v1__DOT__waveform_dac__DOT__bitsum[__Vi0] = VL_SCOPED_RAND_RESET_I(16, __VscopeHash, 7071713531182929897ull);
    }
    for (int __Vi0 = 0; __Vi0 < 8; ++__Vi0) {
        vlSelf->test_sid__DOT__uut__DOT__v1__DOT__envelope_dac__DOT__bitval[__Vi0] = VL_SCOPED_RAND_RESET_I(12, __VscopeHash, 10545859356608288040ull);
    }
    for (int __Vi0 = 0; __Vi0 < 8; ++__Vi0) {
        vlSelf->test_sid__DOT__uut__DOT__v1__DOT__envelope_dac__DOT__bitsum[__Vi0] = VL_SCOPED_RAND_RESET_I(12, __VscopeHash, 14458497913012221669ull);
    }
    vlSelf->test_sid__DOT__uut__DOT__v2__DOT__envelope = VL_SCOPED_RAND_RESET_I(8, __VscopeHash, 15649761197250443348ull);
    vlSelf->test_sid__DOT__uut__DOT__v2__DOT__oscillator = VL_SCOPED_RAND_RESET_I(24, __VscopeHash, 2783203285995549392ull);
    vlSelf->test_sid__DOT__uut__DOT__v2__DOT__osc_msb_in_prv = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 8150626605497624331ull);
    vlSelf->test_sid__DOT__uut__DOT__v2__DOT__noise = VL_SCOPED_RAND_RESET_I(12, __VscopeHash, 8779436519774908271ull);
    vlSelf->test_sid__DOT__uut__DOT__v2__DOT__saw_tri = VL_SCOPED_RAND_RESET_I(12, __VscopeHash, 13403348530127517271ull);
    vlSelf->test_sid__DOT__uut__DOT__v2__DOT__pulse = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 8548873614285800840ull);
    vlSelf->test_sid__DOT__uut__DOT__v2__DOT__norm = VL_SCOPED_RAND_RESET_I(12, __VscopeHash, 479928407085015050ull);
    vlSelf->test_sid__DOT__uut__DOT__v2__DOT__comb = VL_SCOPED_RAND_RESET_I(8, __VscopeHash, 18431611397344703917ull);
    vlSelf->test_sid__DOT__uut__DOT__v2__DOT__norm_dac = VL_SCOPED_RAND_RESET_I(12, __VscopeHash, 3528491694394141620ull);
    vlSelf->test_sid__DOT__uut__DOT__v2__DOT__wave_out = VL_SCOPED_RAND_RESET_I(8, __VscopeHash, 7235430753860389024ull);
    vlSelf->test_sid__DOT__uut__DOT__v2__DOT__dca_out = VL_SCOPED_RAND_RESET_I(22, __VscopeHash, 1990246579407353997ull);
    vlSelf->test_sid__DOT__uut__DOT__v2__DOT__adsr__DOT__state = VL_SCOPED_RAND_RESET_I(2, __VscopeHash, 2325594959617082392ull);
    for (int __Vi0 = 0; __Vi0 < 16; ++__Vi0) {
        vlSelf->test_sid__DOT__uut__DOT__v2__DOT__adsr__DOT__rates[__Vi0] = VL_SCOPED_RAND_RESET_I(15, __VscopeHash, 16625954351240023302ull);
    }
    vlSelf->test_sid__DOT__uut__DOT__v2__DOT__adsr__DOT__rate_period = VL_SCOPED_RAND_RESET_I(15, __VscopeHash, 4350837927871921764ull);
    for (int __Vi0 = 0; __Vi0 < 12; ++__Vi0) {
        vlSelf->test_sid__DOT__uut__DOT__v2__DOT__waveform_dac__DOT__bitval[__Vi0] = VL_SCOPED_RAND_RESET_I(16, __VscopeHash, 13556100556556432745ull);
    }
    for (int __Vi0 = 0; __Vi0 < 12; ++__Vi0) {
        vlSelf->test_sid__DOT__uut__DOT__v2__DOT__waveform_dac__DOT__bitsum[__Vi0] = VL_SCOPED_RAND_RESET_I(16, __VscopeHash, 14037402572637232578ull);
    }
    for (int __Vi0 = 0; __Vi0 < 8; ++__Vi0) {
        vlSelf->test_sid__DOT__uut__DOT__v2__DOT__envelope_dac__DOT__bitval[__Vi0] = VL_SCOPED_RAND_RESET_I(12, __VscopeHash, 3114667601025443562ull);
    }
    for (int __Vi0 = 0; __Vi0 < 8; ++__Vi0) {
        vlSelf->test_sid__DOT__uut__DOT__v2__DOT__envelope_dac__DOT__bitsum[__Vi0] = VL_SCOPED_RAND_RESET_I(12, __VscopeHash, 3291563414118512821ull);
    }
    vlSelf->test_sid__DOT__uut__DOT__v3__DOT__envelope = VL_SCOPED_RAND_RESET_I(8, __VscopeHash, 14958788529972323522ull);
    vlSelf->test_sid__DOT__uut__DOT__v3__DOT__oscillator = VL_SCOPED_RAND_RESET_I(24, __VscopeHash, 8965621349243603514ull);
    vlSelf->test_sid__DOT__uut__DOT__v3__DOT__osc_msb_in_prv = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 9781155744700362633ull);
    vlSelf->test_sid__DOT__uut__DOT__v3__DOT__noise = VL_SCOPED_RAND_RESET_I(12, __VscopeHash, 3937161705211252026ull);
    vlSelf->test_sid__DOT__uut__DOT__v3__DOT__saw_tri = VL_SCOPED_RAND_RESET_I(12, __VscopeHash, 9170250222220748471ull);
    vlSelf->test_sid__DOT__uut__DOT__v3__DOT__pulse = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 9651680863188611619ull);
    vlSelf->test_sid__DOT__uut__DOT__v3__DOT__norm = VL_SCOPED_RAND_RESET_I(12, __VscopeHash, 17405092016669904205ull);
    vlSelf->test_sid__DOT__uut__DOT__v3__DOT__comb = VL_SCOPED_RAND_RESET_I(8, __VscopeHash, 181917890317423729ull);
    vlSelf->test_sid__DOT__uut__DOT__v3__DOT__norm_dac = VL_SCOPED_RAND_RESET_I(12, __VscopeHash, 8170451647435458113ull);
    vlSelf->test_sid__DOT__uut__DOT__v3__DOT__wave_out = VL_SCOPED_RAND_RESET_I(8, __VscopeHash, 1350997259572536006ull);
    vlSelf->test_sid__DOT__uut__DOT__v3__DOT__dca_out = VL_SCOPED_RAND_RESET_I(22, __VscopeHash, 18164610961744440582ull);
    vlSelf->test_sid__DOT__uut__DOT__v3__DOT__adsr__DOT__state = VL_SCOPED_RAND_RESET_I(2, __VscopeHash, 10015232279717536475ull);
    for (int __Vi0 = 0; __Vi0 < 16; ++__Vi0) {
        vlSelf->test_sid__DOT__uut__DOT__v3__DOT__adsr__DOT__rates[__Vi0] = VL_SCOPED_RAND_RESET_I(15, __VscopeHash, 2991916959062338496ull);
    }
    vlSelf->test_sid__DOT__uut__DOT__v3__DOT__adsr__DOT__rate_period = VL_SCOPED_RAND_RESET_I(15, __VscopeHash, 13499119865784421476ull);
    for (int __Vi0 = 0; __Vi0 < 12; ++__Vi0) {
        vlSelf->test_sid__DOT__uut__DOT__v3__DOT__waveform_dac__DOT__bitval[__Vi0] = VL_SCOPED_RAND_RESET_I(16, __VscopeHash, 7974154974251728590ull);
    }
    for (int __Vi0 = 0; __Vi0 < 12; ++__Vi0) {
        vlSelf->test_sid__DOT__uut__DOT__v3__DOT__waveform_dac__DOT__bitsum[__Vi0] = VL_SCOPED_RAND_RESET_I(16, __VscopeHash, 10871049412980917717ull);
    }
    for (int __Vi0 = 0; __Vi0 < 8; ++__Vi0) {
        vlSelf->test_sid__DOT__uut__DOT__v3__DOT__envelope_dac__DOT__bitval[__Vi0] = VL_SCOPED_RAND_RESET_I(12, __VscopeHash, 7258265205818700884ull);
    }
    for (int __Vi0 = 0; __Vi0 < 8; ++__Vi0) {
        vlSelf->test_sid__DOT__uut__DOT__v3__DOT__envelope_dac__DOT__bitsum[__Vi0] = VL_SCOPED_RAND_RESET_I(12, __VscopeHash, 2523242113898261721ull);
    }
    for (int __Vi0 = 0; __Vi0 < 2048; ++__Vi0) {
        vlSelf->test_sid__DOT__uut__DOT__tbl__DOT__wave6581_p_t[__Vi0] = VL_SCOPED_RAND_RESET_I(8, __VscopeHash, 708563379520765169ull);
    }
    for (int __Vi0 = 0; __Vi0 < 2048; ++__Vi0) {
        vlSelf->test_sid__DOT__uut__DOT__tbl__DOT__wave6581_ps_[__Vi0] = VL_SCOPED_RAND_RESET_I(8, __VscopeHash, 15053421091683592215ull);
    }
    for (int __Vi0 = 0; __Vi0 < 2048; ++__Vi0) {
        vlSelf->test_sid__DOT__uut__DOT__tbl__DOT__wave8580_p_t[__Vi0] = VL_SCOPED_RAND_RESET_I(8, __VscopeHash, 6481641606128110571ull);
    }
    for (int __Vi0 = 0; __Vi0 < 4096; ++__Vi0) {
        vlSelf->test_sid__DOT__uut__DOT__tbl__DOT__wave8580_ps_[__Vi0] = VL_SCOPED_RAND_RESET_I(8, __VscopeHash, 12137755648535747656ull);
    }
    vlSelf->test_sid__DOT__uut__DOT__tbl__DOT__f0 = VL_SCOPED_RAND_RESET_I(16, __VscopeHash, 10020064281699458832ull);
    for (int __Vi0 = 0; __Vi0 < 4096; ++__Vi0) {
        vlSelf->test_sid__DOT__uut__DOT__tbl__DOT__f6581_curve[__Vi0] = VL_SCOPED_RAND_RESET_I(16, __VscopeHash, 10498715359399179934ull);
    }
    for (int __Vi0 = 0; __Vi0 < 1024; ++__Vi0) {
        vlSelf->test_sid__DOT__uut__DOT__tbl__DOT___unused_f6581_adj[__Vi0] = VL_SCOPED_RAND_RESET_I(15, __VscopeHash, 17458515437229748590ull);
    }
    for (int __Vi0 = 0; __Vi0 < 32; ++__Vi0) {
        vlSelf->test_sid__DOT__uut__DOT__flt__DOT___1_Q_lsl10_tbl[__Vi0] = VL_SCOPED_RAND_RESET_I(11, __VscopeHash, 14548029033354070413ull);
    }
    vlSelf->test_sid__DOT__uut__DOT__flt__DOT__c = VL_SCOPED_RAND_RESET_I(32, __VscopeHash, 16143637249195858207ull);
    vlSelf->test_sid__DOT__uut__DOT__flt__DOT__s = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 1292661644597354450ull);
    vlSelf->test_sid__DOT__uut__DOT__flt__DOT__a = VL_SCOPED_RAND_RESET_I(16, __VscopeHash, 17326472810195732197ull);
    vlSelf->test_sid__DOT__uut__DOT__flt__DOT__b = VL_SCOPED_RAND_RESET_I(16, __VscopeHash, 5916915797773641149ull);
    vlSelf->test_sid__DOT__uut__DOT__flt__DOT__o = VL_SCOPED_RAND_RESET_I(32, __VscopeHash, 16118956954794135838ull);
    vlSelf->test_sid__DOT__uut__DOT__flt__DOT__vlp = VL_SCOPED_RAND_RESET_I(16, __VscopeHash, 10262949885612750123ull);
    vlSelf->test_sid__DOT__uut__DOT__flt__DOT__vlp2 = VL_SCOPED_RAND_RESET_I(16, __VscopeHash, 715046400236925627ull);
    vlSelf->test_sid__DOT__uut__DOT__flt__DOT__vbp = VL_SCOPED_RAND_RESET_I(16, __VscopeHash, 3271680436890295477ull);
    vlSelf->test_sid__DOT__uut__DOT__flt__DOT__vbp2 = VL_SCOPED_RAND_RESET_I(16, __VscopeHash, 7035968774285568804ull);
    vlSelf->test_sid__DOT__uut__DOT__flt__DOT__vbp_next = VL_SCOPED_RAND_RESET_I(16, __VscopeHash, 16223926668428752052ull);
    vlSelf->test_sid__DOT__uut__DOT__flt__DOT__vhp = VL_SCOPED_RAND_RESET_I(16, __VscopeHash, 4833182571787118949ull);
    vlSelf->test_sid__DOT__uut__DOT__flt__DOT__vhp2 = VL_SCOPED_RAND_RESET_I(16, __VscopeHash, 11132941561622341548ull);
    vlSelf->test_sid__DOT__uut__DOT__flt__DOT__vhp_next = VL_SCOPED_RAND_RESET_I(16, __VscopeHash, 10776308810529317247ull);
    vlSelf->__VdfgRegularize_hebeb780c_0_3 = 0;
    for (int __Vi0 = 0; __Vi0 < 2; ++__Vi0) {
        vlSelf->__VstlTriggered[__Vi0] = 0;
    }
    for (int __Vi0 = 0; __Vi0 < 12; ++__Vi0) {
        vlSelf->__Vtrigprevexpr___TOP__test_sid__DOT__uut__DOT__v1__DOT__waveform_dac__DOT__bitsum__0[__Vi0] = 0;
    }
    for (int __Vi0 = 0; __Vi0 < 8; ++__Vi0) {
        vlSelf->__Vtrigprevexpr___TOP__test_sid__DOT__uut__DOT__v1__DOT__envelope_dac__DOT__bitsum__0[__Vi0] = 0;
    }
    for (int __Vi0 = 0; __Vi0 < 12; ++__Vi0) {
        vlSelf->__Vtrigprevexpr___TOP__test_sid__DOT__uut__DOT__v2__DOT__waveform_dac__DOT__bitsum__0[__Vi0] = 0;
    }
    for (int __Vi0 = 0; __Vi0 < 8; ++__Vi0) {
        vlSelf->__Vtrigprevexpr___TOP__test_sid__DOT__uut__DOT__v2__DOT__envelope_dac__DOT__bitsum__0[__Vi0] = 0;
    }
    for (int __Vi0 = 0; __Vi0 < 12; ++__Vi0) {
        vlSelf->__Vtrigprevexpr___TOP__test_sid__DOT__uut__DOT__v3__DOT__waveform_dac__DOT__bitsum__0[__Vi0] = 0;
    }
    for (int __Vi0 = 0; __Vi0 < 8; ++__Vi0) {
        vlSelf->__Vtrigprevexpr___TOP__test_sid__DOT__uut__DOT__v3__DOT__envelope_dac__DOT__bitsum__0[__Vi0] = 0;
    }
    vlSelf->__VstlDidInit = 0;
    for (int __Vi0 = 0; __Vi0 < 1; ++__Vi0) {
        vlSelf->__VactTriggered[__Vi0] = 0;
    }
    for (int __Vi0 = 0; __Vi0 < 1; ++__Vi0) {
        vlSelf->__VactTriggeredAcc[__Vi0] = 0;
    }
    vlSelf->__Vtrigprevexpr___TOP__test_sid__DOT__clk__0 = 0;
    for (int __Vi0 = 0; __Vi0 < 1; ++__Vi0) {
        vlSelf->__VnbaTriggered[__Vi0] = 0;
    }
    vlSelf->__Vi = 0;
}
