// Verilated -*- C++ -*-
// DESCRIPTION: Verilator output: Design implementation internals
// See Vtop.h for the primary calling header

#include "Vtop__pch.h"

VL_ATTR_COLD void Vtop_sid_chip___eval_static__TOP__top__DOT__sid1_inst(Vtop_sid_chip* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+        Vtop_sid_chip___eval_static__TOP__top__DOT__sid1_inst\n"); );
    Vtop__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
    // Body
    const uint64_t __VscopeHash = VL_MURMUR64_HASH(vlSelf->vlNamep);
    vlSelf->__PVT__v1__DOT__unnamedblk1__DOT__test_delay = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 17675525903532941391ull);
    vlSelf->__PVT__v1__DOT__unnamedblk2__DOT__clk = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 7628317674956743110ull);
    vlSelf->__PVT__v1__DOT__unnamedblk2__DOT__clk_d = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 17895623569225443792ull);
    vlSelf->__PVT__v1__DOT__unnamedblk2__DOT__osc_edge = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 3596731012297611790ull);
    vlSelf->__PVT__v1__DOT__unnamedblk2__DOT__noise_age = VL_SCOPED_RAND_RESET_I(24, __VscopeHash, 190508095177976654ull);
    vlSelf->__PVT__v1__DOT__unnamedblk2__DOT__lfsr_noise = VL_SCOPED_RAND_RESET_I(23, __VscopeHash, 14759698356553119455ull);
    vlSelf->__PVT__v1__DOT__unnamedblk3__DOT__keep_cnt = VL_SCOPED_RAND_RESET_I(24, __VscopeHash, 6560436285365029967ull);
    vlSelf->__PVT__v1__DOT__unnamedblk3__DOT__env_dac = VL_SCOPED_RAND_RESET_I(9, __VscopeHash, 7607314705844518853ull);
    vlSelf->__PVT__v1__DOT__unnamedblk3__DOT__dac_out = VL_SCOPED_RAND_RESET_I(13, __VscopeHash, 10785435958798372889ull);
    vlSelf->__PVT__v1__DOT__adsr__DOT__unnamedblk1__DOT__hold_zero = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 10701218408128078879ull);
    vlSelf->__PVT__v1__DOT__adsr__DOT__unnamedblk1__DOT__exponential_counter_period = VL_SCOPED_RAND_RESET_I(5, __VscopeHash, 6765351731096681466ull);
    vlSelf->__PVT__v1__DOT__adsr__DOT__unnamedblk1__DOT__gate_edge = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 13172174857532755414ull);
    vlSelf->__PVT__v1__DOT__adsr__DOT__unnamedblk1__DOT__rate_counter = VL_SCOPED_RAND_RESET_I(15, __VscopeHash, 14872542086862103454ull);
    vlSelf->__PVT__v1__DOT__adsr__DOT__unnamedblk1__DOT__exponential_counter = VL_SCOPED_RAND_RESET_I(5, __VscopeHash, 6470191367660544301ull);
    vlSelf->__PVT__v2__DOT__unnamedblk1__DOT__test_delay = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 10401351556583579829ull);
    vlSelf->__PVT__v2__DOT__unnamedblk2__DOT__clk = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 10474218186493517067ull);
    vlSelf->__PVT__v2__DOT__unnamedblk2__DOT__clk_d = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 7588895595080491715ull);
    vlSelf->__PVT__v2__DOT__unnamedblk2__DOT__osc_edge = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 9819599145126512681ull);
    vlSelf->__PVT__v2__DOT__unnamedblk2__DOT__noise_age = VL_SCOPED_RAND_RESET_I(24, __VscopeHash, 2728683163300080555ull);
    vlSelf->__PVT__v2__DOT__unnamedblk2__DOT__lfsr_noise = VL_SCOPED_RAND_RESET_I(23, __VscopeHash, 18438855861650110489ull);
    vlSelf->__PVT__v2__DOT__unnamedblk3__DOT__keep_cnt = VL_SCOPED_RAND_RESET_I(24, __VscopeHash, 14947482669929398552ull);
    vlSelf->__PVT__v2__DOT__unnamedblk3__DOT__env_dac = VL_SCOPED_RAND_RESET_I(9, __VscopeHash, 7042978457235203456ull);
    vlSelf->__PVT__v2__DOT__unnamedblk3__DOT__dac_out = VL_SCOPED_RAND_RESET_I(13, __VscopeHash, 13474346048400885171ull);
    vlSelf->__PVT__v2__DOT__adsr__DOT__unnamedblk1__DOT__hold_zero = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 6696034689289544418ull);
    vlSelf->__PVT__v2__DOT__adsr__DOT__unnamedblk1__DOT__exponential_counter_period = VL_SCOPED_RAND_RESET_I(5, __VscopeHash, 1540078271577464219ull);
    vlSelf->__PVT__v2__DOT__adsr__DOT__unnamedblk1__DOT__gate_edge = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 13612872782261332253ull);
    vlSelf->__PVT__v2__DOT__adsr__DOT__unnamedblk1__DOT__rate_counter = VL_SCOPED_RAND_RESET_I(15, __VscopeHash, 9242819862328421722ull);
    vlSelf->__PVT__v2__DOT__adsr__DOT__unnamedblk1__DOT__exponential_counter = VL_SCOPED_RAND_RESET_I(5, __VscopeHash, 17109448992975078454ull);
    vlSelf->__PVT__v3__DOT__unnamedblk1__DOT__test_delay = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 13805126485684274437ull);
    vlSelf->__PVT__v3__DOT__unnamedblk2__DOT__clk = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 12197473931239662281ull);
    vlSelf->__PVT__v3__DOT__unnamedblk2__DOT__clk_d = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 5653152247199994257ull);
    vlSelf->__PVT__v3__DOT__unnamedblk2__DOT__osc_edge = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 3965952016474932944ull);
    vlSelf->__PVT__v3__DOT__unnamedblk2__DOT__noise_age = VL_SCOPED_RAND_RESET_I(24, __VscopeHash, 17334110419989618579ull);
    vlSelf->__PVT__v3__DOT__unnamedblk2__DOT__lfsr_noise = VL_SCOPED_RAND_RESET_I(23, __VscopeHash, 9765231236476583256ull);
    vlSelf->__PVT__v3__DOT__unnamedblk3__DOT__keep_cnt = VL_SCOPED_RAND_RESET_I(24, __VscopeHash, 6830496636320036291ull);
    vlSelf->__PVT__v3__DOT__unnamedblk3__DOT__env_dac = VL_SCOPED_RAND_RESET_I(9, __VscopeHash, 12604191844008217769ull);
    vlSelf->__PVT__v3__DOT__unnamedblk3__DOT__dac_out = VL_SCOPED_RAND_RESET_I(13, __VscopeHash, 13590978805243912419ull);
    vlSelf->__PVT__v3__DOT__adsr__DOT__unnamedblk1__DOT__hold_zero = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 9095672095480666967ull);
    vlSelf->__PVT__v3__DOT__adsr__DOT__unnamedblk1__DOT__exponential_counter_period = VL_SCOPED_RAND_RESET_I(5, __VscopeHash, 15805600923609863825ull);
    vlSelf->__PVT__v3__DOT__adsr__DOT__unnamedblk1__DOT__gate_edge = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 5185650700378575558ull);
    vlSelf->__PVT__v3__DOT__adsr__DOT__unnamedblk1__DOT__rate_counter = VL_SCOPED_RAND_RESET_I(15, __VscopeHash, 13479700002018873225ull);
    vlSelf->__PVT__v3__DOT__adsr__DOT__unnamedblk1__DOT__exponential_counter = VL_SCOPED_RAND_RESET_I(5, __VscopeHash, 3609477087306560038ull);
    vlSelf->__PVT__flt__DOT__unnamedblk1__DOT___1_Q_lsl10 = VL_SCOPED_RAND_RESET_I(11, __VscopeHash, 5449120119781615539ull);
    vlSelf->__PVT__flt__DOT__unnamedblk1__DOT__vi = VL_SCOPED_RAND_RESET_I(16, __VscopeHash, 6238884773632187629ull);
    vlSelf->__PVT__flt__DOT__unnamedblk1__DOT__vd = VL_SCOPED_RAND_RESET_I(16, __VscopeHash, 15767743066292336226ull);
}

VL_ATTR_COLD void Vtop_sid_chip___eval_initial__TOP__top__DOT__sid1_inst(Vtop_sid_chip* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+        Vtop_sid_chip___eval_initial__TOP__top__DOT__sid1_inst\n"); );
    Vtop__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
    // Body
    vlSelfRef.__PVT__v1__DOT__adsr__DOT__rates[0U] = 8U;
    vlSelfRef.__PVT__v1__DOT__adsr__DOT__rates[1U] = 0x001fU;
    vlSelfRef.__PVT__v1__DOT__adsr__DOT__rates[2U] = 0x003eU;
    vlSelfRef.__PVT__v1__DOT__adsr__DOT__rates[3U] = 0x005eU;
    vlSelfRef.__PVT__v1__DOT__adsr__DOT__rates[4U] = 0x0094U;
    vlSelfRef.__PVT__v1__DOT__adsr__DOT__rates[5U] = 0x00dbU;
    vlSelfRef.__PVT__v1__DOT__adsr__DOT__rates[6U] = 0x010aU;
    vlSelfRef.__PVT__v1__DOT__adsr__DOT__rates[7U] = 0x0138U;
    vlSelfRef.__PVT__v1__DOT__adsr__DOT__rates[8U] = 0x0187U;
    vlSelfRef.__PVT__v1__DOT__adsr__DOT__rates[9U] = 0x03d0U;
    vlSelfRef.__PVT__v1__DOT__adsr__DOT__rates[10U] = 0x07a1U;
    vlSelfRef.__PVT__v1__DOT__adsr__DOT__rates[11U] = 0x0c35U;
    vlSelfRef.__PVT__v1__DOT__adsr__DOT__rates[12U] = 0x0f42U;
    vlSelfRef.__PVT__v1__DOT__adsr__DOT__rates[13U] = 0x2dc7U;
    vlSelfRef.__PVT__v1__DOT__adsr__DOT__rates[14U] = 0x4c4bU;
    vlSelfRef.__PVT__v1__DOT__adsr__DOT__rates[15U] = 0x7a12U;
    vlSelfRef.__PVT__v1__DOT__waveform_dac__DOT__bitval[0U] = 0x0021U;
    vlSelfRef.__PVT__v1__DOT__waveform_dac__DOT__bitval[1U] = 0x0030U;
    vlSelfRef.__PVT__v1__DOT__waveform_dac__DOT__bitval[2U] = 0x0055U;
    vlSelfRef.__PVT__v1__DOT__waveform_dac__DOT__bitval[3U] = 0x00a0U;
    vlSelfRef.__PVT__v1__DOT__waveform_dac__DOT__bitval[4U] = 0x0135U;
    vlSelfRef.__PVT__v1__DOT__waveform_dac__DOT__bitval[5U] = 0x0256U;
    vlSelfRef.__PVT__v1__DOT__waveform_dac__DOT__bitval[6U] = 0x0486U;
    vlSelfRef.__PVT__v1__DOT__waveform_dac__DOT__bitval[7U] = 0x08c6U;
    vlSelfRef.__PVT__v1__DOT__waveform_dac__DOT__bitval[8U] = 0x1102U;
    vlSelfRef.__PVT__v1__DOT__waveform_dac__DOT__bitval[9U] = 0x20f8U;
    vlSelfRef.__PVT__v1__DOT__waveform_dac__DOT__bitval[10U] = 0x3fecU;
    vlSelfRef.__PVT__v1__DOT__waveform_dac__DOT__bitval[11U] = 0x7bedU;
    vlSelfRef.__PVT__v1__DOT__envelope_dac__DOT__bitval[0U] = 0x001dU;
    vlSelfRef.__PVT__v1__DOT__envelope_dac__DOT__bitval[1U] = 0x002aU;
    vlSelfRef.__PVT__v1__DOT__envelope_dac__DOT__bitval[2U] = 0x004bU;
    vlSelfRef.__PVT__v1__DOT__envelope_dac__DOT__bitval[3U] = 0x008dU;
    vlSelfRef.__PVT__v1__DOT__envelope_dac__DOT__bitval[4U] = 0x0110U;
    vlSelfRef.__PVT__v1__DOT__envelope_dac__DOT__bitval[5U] = 0x020eU;
    vlSelfRef.__PVT__v1__DOT__envelope_dac__DOT__bitval[6U] = 0x03fbU;
    vlSelfRef.__PVT__v1__DOT__envelope_dac__DOT__bitval[7U] = 0x07b8U;
    vlSelfRef.__PVT__v2__DOT__adsr__DOT__rates[0U] = 8U;
    vlSelfRef.__PVT__v2__DOT__adsr__DOT__rates[1U] = 0x001fU;
    vlSelfRef.__PVT__v2__DOT__adsr__DOT__rates[2U] = 0x003eU;
    vlSelfRef.__PVT__v2__DOT__adsr__DOT__rates[3U] = 0x005eU;
    vlSelfRef.__PVT__v2__DOT__adsr__DOT__rates[4U] = 0x0094U;
    vlSelfRef.__PVT__v2__DOT__adsr__DOT__rates[5U] = 0x00dbU;
    vlSelfRef.__PVT__v2__DOT__adsr__DOT__rates[6U] = 0x010aU;
    vlSelfRef.__PVT__v2__DOT__adsr__DOT__rates[7U] = 0x0138U;
    vlSelfRef.__PVT__v2__DOT__adsr__DOT__rates[8U] = 0x0187U;
    vlSelfRef.__PVT__v2__DOT__adsr__DOT__rates[9U] = 0x03d0U;
    vlSelfRef.__PVT__v2__DOT__adsr__DOT__rates[10U] = 0x07a1U;
    vlSelfRef.__PVT__v2__DOT__adsr__DOT__rates[11U] = 0x0c35U;
    vlSelfRef.__PVT__v2__DOT__adsr__DOT__rates[12U] = 0x0f42U;
    vlSelfRef.__PVT__v2__DOT__adsr__DOT__rates[13U] = 0x2dc7U;
    vlSelfRef.__PVT__v2__DOT__adsr__DOT__rates[14U] = 0x4c4bU;
    vlSelfRef.__PVT__v2__DOT__adsr__DOT__rates[15U] = 0x7a12U;
    vlSelfRef.__PVT__v2__DOT__waveform_dac__DOT__bitval[0U] = 0x0021U;
    vlSelfRef.__PVT__v2__DOT__waveform_dac__DOT__bitval[1U] = 0x0030U;
    vlSelfRef.__PVT__v2__DOT__waveform_dac__DOT__bitval[2U] = 0x0055U;
    vlSelfRef.__PVT__v2__DOT__waveform_dac__DOT__bitval[3U] = 0x00a0U;
    vlSelfRef.__PVT__v2__DOT__waveform_dac__DOT__bitval[4U] = 0x0135U;
    vlSelfRef.__PVT__v2__DOT__waveform_dac__DOT__bitval[5U] = 0x0256U;
    vlSelfRef.__PVT__v2__DOT__waveform_dac__DOT__bitval[6U] = 0x0486U;
    vlSelfRef.__PVT__v2__DOT__waveform_dac__DOT__bitval[7U] = 0x08c6U;
    vlSelfRef.__PVT__v2__DOT__waveform_dac__DOT__bitval[8U] = 0x1102U;
    vlSelfRef.__PVT__v2__DOT__waveform_dac__DOT__bitval[9U] = 0x20f8U;
    vlSelfRef.__PVT__v2__DOT__waveform_dac__DOT__bitval[10U] = 0x3fecU;
    vlSelfRef.__PVT__v2__DOT__waveform_dac__DOT__bitval[11U] = 0x7bedU;
    vlSelfRef.__PVT__v2__DOT__envelope_dac__DOT__bitval[0U] = 0x001dU;
    vlSelfRef.__PVT__v2__DOT__envelope_dac__DOT__bitval[1U] = 0x002aU;
    vlSelfRef.__PVT__v2__DOT__envelope_dac__DOT__bitval[2U] = 0x004bU;
    vlSelfRef.__PVT__v2__DOT__envelope_dac__DOT__bitval[3U] = 0x008dU;
    vlSelfRef.__PVT__v2__DOT__envelope_dac__DOT__bitval[4U] = 0x0110U;
    vlSelfRef.__PVT__v2__DOT__envelope_dac__DOT__bitval[5U] = 0x020eU;
    vlSelfRef.__PVT__v2__DOT__envelope_dac__DOT__bitval[6U] = 0x03fbU;
    vlSelfRef.__PVT__v2__DOT__envelope_dac__DOT__bitval[7U] = 0x07b8U;
    vlSelfRef.__PVT__v3__DOT__adsr__DOT__rates[0U] = 8U;
    vlSelfRef.__PVT__v3__DOT__adsr__DOT__rates[1U] = 0x001fU;
    vlSelfRef.__PVT__v3__DOT__adsr__DOT__rates[2U] = 0x003eU;
    vlSelfRef.__PVT__v3__DOT__adsr__DOT__rates[3U] = 0x005eU;
    vlSelfRef.__PVT__v3__DOT__adsr__DOT__rates[4U] = 0x0094U;
    vlSelfRef.__PVT__v3__DOT__adsr__DOT__rates[5U] = 0x00dbU;
    vlSelfRef.__PVT__v3__DOT__adsr__DOT__rates[6U] = 0x010aU;
    vlSelfRef.__PVT__v3__DOT__adsr__DOT__rates[7U] = 0x0138U;
    vlSelfRef.__PVT__v3__DOT__adsr__DOT__rates[8U] = 0x0187U;
    vlSelfRef.__PVT__v3__DOT__adsr__DOT__rates[9U] = 0x03d0U;
    vlSelfRef.__PVT__v3__DOT__adsr__DOT__rates[10U] = 0x07a1U;
    vlSelfRef.__PVT__v3__DOT__adsr__DOT__rates[11U] = 0x0c35U;
    vlSelfRef.__PVT__v3__DOT__adsr__DOT__rates[12U] = 0x0f42U;
    vlSelfRef.__PVT__v3__DOT__adsr__DOT__rates[13U] = 0x2dc7U;
    vlSelfRef.__PVT__v3__DOT__adsr__DOT__rates[14U] = 0x4c4bU;
    vlSelfRef.__PVT__v3__DOT__adsr__DOT__rates[15U] = 0x7a12U;
    vlSelfRef.__PVT__v3__DOT__waveform_dac__DOT__bitval[0U] = 0x0021U;
    vlSelfRef.__PVT__v3__DOT__waveform_dac__DOT__bitval[1U] = 0x0030U;
    vlSelfRef.__PVT__v3__DOT__waveform_dac__DOT__bitval[2U] = 0x0055U;
    vlSelfRef.__PVT__v3__DOT__waveform_dac__DOT__bitval[3U] = 0x00a0U;
    vlSelfRef.__PVT__v3__DOT__waveform_dac__DOT__bitval[4U] = 0x0135U;
    vlSelfRef.__PVT__v3__DOT__waveform_dac__DOT__bitval[5U] = 0x0256U;
    vlSelfRef.__PVT__v3__DOT__waveform_dac__DOT__bitval[6U] = 0x0486U;
    vlSelfRef.__PVT__v3__DOT__waveform_dac__DOT__bitval[7U] = 0x08c6U;
    vlSelfRef.__PVT__v3__DOT__waveform_dac__DOT__bitval[8U] = 0x1102U;
    vlSelfRef.__PVT__v3__DOT__waveform_dac__DOT__bitval[9U] = 0x20f8U;
    vlSelfRef.__PVT__v3__DOT__waveform_dac__DOT__bitval[10U] = 0x3fecU;
    vlSelfRef.__PVT__v3__DOT__waveform_dac__DOT__bitval[11U] = 0x7bedU;
    vlSelfRef.__PVT__v3__DOT__envelope_dac__DOT__bitval[0U] = 0x001dU;
    vlSelfRef.__PVT__v3__DOT__envelope_dac__DOT__bitval[1U] = 0x002aU;
    vlSelfRef.__PVT__v3__DOT__envelope_dac__DOT__bitval[2U] = 0x004bU;
    vlSelfRef.__PVT__v3__DOT__envelope_dac__DOT__bitval[3U] = 0x008dU;
    vlSelfRef.__PVT__v3__DOT__envelope_dac__DOT__bitval[4U] = 0x0110U;
    vlSelfRef.__PVT__v3__DOT__envelope_dac__DOT__bitval[5U] = 0x020eU;
    vlSelfRef.__PVT__v3__DOT__envelope_dac__DOT__bitval[6U] = 0x03fbU;
    vlSelfRef.__PVT__v3__DOT__envelope_dac__DOT__bitval[7U] = 0x07b8U;
    VL_READMEM_N(true, 8, 2048, 0, "rom/wave6581_p_t.hex"s
                 ,  &(vlSelfRef.__PVT__tbl__DOT__wave6581_p_t)
                 , 0, ~0ULL);
    VL_READMEM_N(true, 8, 2048, 0, "rom/wave6581_ps_.hex"s
                 ,  &(vlSelfRef.__PVT__tbl__DOT__wave6581_ps_)
                 , 0, ~0ULL);
    VL_READMEM_N(true, 8, 2048, 0, "rom/wave8580_p_t.hex"s
                 ,  &(vlSelfRef.__PVT__tbl__DOT__wave8580_p_t)
                 , 0, ~0ULL);
    VL_READMEM_N(true, 8, 4096, 0, "rom/wave8580_ps_.hex"s
                 ,  &(vlSelfRef.__PVT__tbl__DOT__wave8580_ps_)
                 , 0, ~0ULL);
    VL_READMEM_N(true, 16, 4096, 0, "rom/f6581_curve.hex"s
                 ,  &(vlSelfRef.__PVT__tbl__DOT__f6581_curve)
                 , 0, ~0ULL);
    VL_READMEM_N(true, 15, 1024, 0, "rom/_unused_f6581_adj.hex"s
                 ,  &(vlSelfRef.__PVT__tbl__DOT___unused_f6581_adj)
                 , 0, ~0ULL);
    vlSelfRef.__PVT__flt__DOT___1_Q_lsl10_tbl[0U] = 0x05a8U;
    vlSelfRef.__PVT__flt__DOT___1_Q_lsl10_tbl[1U] = 0x052cU;
    vlSelfRef.__PVT__flt__DOT___1_Q_lsl10_tbl[2U] = 0x04c3U;
    vlSelfRef.__PVT__flt__DOT___1_Q_lsl10_tbl[3U] = 0x0469U;
    vlSelfRef.__PVT__flt__DOT___1_Q_lsl10_tbl[4U] = 0x041cU;
    vlSelfRef.__PVT__flt__DOT___1_Q_lsl10_tbl[5U] = 0x03d8U;
    vlSelfRef.__PVT__flt__DOT___1_Q_lsl10_tbl[6U] = 0x039dU;
    vlSelfRef.__PVT__flt__DOT___1_Q_lsl10_tbl[7U] = 0x0368U;
    vlSelfRef.__PVT__flt__DOT___1_Q_lsl10_tbl[8U] = 0x033aU;
    vlSelfRef.__PVT__flt__DOT___1_Q_lsl10_tbl[9U] = 0x030fU;
    vlSelfRef.__PVT__flt__DOT___1_Q_lsl10_tbl[10U] = 0x02e9U;
    vlSelfRef.__PVT__flt__DOT___1_Q_lsl10_tbl[11U] = 0x02c7U;
    vlSelfRef.__PVT__flt__DOT___1_Q_lsl10_tbl[12U] = 0x02a7U;
    vlSelfRef.__PVT__flt__DOT___1_Q_lsl10_tbl[13U] = 0x028bU;
    vlSelfRef.__PVT__flt__DOT___1_Q_lsl10_tbl[14U] = 0x0270U;
    vlSelfRef.__PVT__flt__DOT___1_Q_lsl10_tbl[15U] = 0x0258U;
    vlSelfRef.__PVT__flt__DOT___1_Q_lsl10_tbl[16U] = 0x05a8U;
    vlSelfRef.__PVT__flt__DOT___1_Q_lsl10_tbl[17U] = 0x0530U;
    vlSelfRef.__PVT__flt__DOT___1_Q_lsl10_tbl[18U] = 0x04c2U;
    vlSelfRef.__PVT__flt__DOT___1_Q_lsl10_tbl[19U] = 0x045dU;
    vlSelfRef.__PVT__flt__DOT___1_Q_lsl10_tbl[20U] = 0x0400U;
    vlSelfRef.__PVT__flt__DOT___1_Q_lsl10_tbl[21U] = 0x03abU;
    vlSelfRef.__PVT__flt__DOT___1_Q_lsl10_tbl[22U] = 0x035dU;
    vlSelfRef.__PVT__flt__DOT___1_Q_lsl10_tbl[23U] = 0x0316U;
    vlSelfRef.__PVT__flt__DOT___1_Q_lsl10_tbl[24U] = 0x02d4U;
    vlSelfRef.__PVT__flt__DOT___1_Q_lsl10_tbl[25U] = 0x0298U;
    vlSelfRef.__PVT__flt__DOT___1_Q_lsl10_tbl[26U] = 0x0261U;
    vlSelfRef.__PVT__flt__DOT___1_Q_lsl10_tbl[27U] = 0x022eU;
    vlSelfRef.__PVT__flt__DOT___1_Q_lsl10_tbl[28U] = 0x0200U;
    vlSelfRef.__PVT__flt__DOT___1_Q_lsl10_tbl[29U] = 0x01d6U;
    vlSelfRef.__PVT__flt__DOT___1_Q_lsl10_tbl[30U] = 0x01afU;
    vlSelfRef.__PVT__flt__DOT___1_Q_lsl10_tbl[31U] = 0x018bU;
}

VL_ATTR_COLD void Vtop_sid_chip___stl_sequent__TOP__top__DOT__sid1_inst__0(Vtop_sid_chip* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+        Vtop_sid_chip___stl_sequent__TOP__top__DOT__sid1_inst__0\n"); );
    Vtop__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
    // Locals
    IData/*31:0*/ __PVT__flt__DOT__m;
    __PVT__flt__DOT__m = 0;
    IData/*16:0*/ __VdfgRegularize_hebeb780c_0_30;
    __VdfgRegularize_hebeb780c_0_30 = 0;
    // Body
    vlSelfRef.__PVT__v1_scaled = (0x003fffffU & VL_SHIFTRS_III(22,22,32, 
                                                               (0x003fffffU 
                                                                & VL_MULS_III(22, vlSelfRef.__PVT__v1__DOT__dca_out, 
                                                                              (0x003fffffU 
                                                                               & VL_EXTENDS_II(22,5, vlSelfRef.__PVT__voice_vol[0U])))), 4U));
    vlSelfRef.__PVT__v2_scaled = (0x003fffffU & VL_SHIFTRS_III(22,22,32, 
                                                               (0x003fffffU 
                                                                & VL_MULS_III(22, vlSelfRef.__PVT__v2__DOT__dca_out, 
                                                                              (0x003fffffU 
                                                                               & VL_EXTENDS_II(22,5, vlSelfRef.__PVT__voice_vol[1U])))), 4U));
    vlSelfRef.__PVT__v3_scaled = (0x003fffffU & VL_SHIFTRS_III(22,22,32, 
                                                               (0x003fffffU 
                                                                & VL_MULS_III(22, vlSelfRef.__PVT__v3__DOT__dca_out, 
                                                                              (0x003fffffU 
                                                                               & VL_EXTENDS_II(22,5, vlSelfRef.__PVT__voice_vol[2U])))), 4U));
    vlSelfRef.__PVT__v1__DOT__adsr__DOT__rate_period 
        = vlSelfRef.__PVT__v1__DOT__adsr__DOT__rates
        [(0x0000000fU & ((1U == (IData)(vlSelfRef.__PVT__v1__DOT__adsr__DOT__state))
                          ? (vlSelfRef.__PVT__voice_ad[0U] 
                             >> 4U) : ((2U == (IData)(vlSelfRef.__PVT__v1__DOT__adsr__DOT__state))
                                        ? vlSelfRef.__PVT__voice_ad[0U]
                                        : vlSelfRef.__PVT__voice_sr[0U])))];
    vlSelfRef.__PVT__v2__DOT__adsr__DOT__rate_period 
        = vlSelfRef.__PVT__v2__DOT__adsr__DOT__rates
        [(0x0000000fU & ((1U == (IData)(vlSelfRef.__PVT__v2__DOT__adsr__DOT__state))
                          ? (vlSelfRef.__PVT__voice_ad[1U] 
                             >> 4U) : ((2U == (IData)(vlSelfRef.__PVT__v2__DOT__adsr__DOT__state))
                                        ? vlSelfRef.__PVT__voice_ad[1U]
                                        : vlSelfRef.__PVT__voice_sr[1U])))];
    vlSelfRef.__PVT__v3__DOT__adsr__DOT__rate_period 
        = vlSelfRef.__PVT__v3__DOT__adsr__DOT__rates
        [(0x0000000fU & ((1U == (IData)(vlSelfRef.__PVT__v3__DOT__adsr__DOT__state))
                          ? (vlSelfRef.__PVT__voice_ad[2U] 
                             >> 4U) : ((2U == (IData)(vlSelfRef.__PVT__v3__DOT__adsr__DOT__state))
                                        ? vlSelfRef.__PVT__voice_ad[2U]
                                        : vlSelfRef.__PVT__voice_sr[2U])))];
    if ((0x00000080U & vlSelfRef.__PVT__voice_ctrl[0U])) {
        vlSelfRef.__PVT__v1__DOT__comb = 0U;
        vlSelfRef.__PVT__v1__DOT__norm = (0x00000fffU 
                                          & ((0x00000040U 
                                              & vlSelfRef.__PVT__voice_ctrl[0U])
                                              ? 0U : 
                                             ((0x00000020U 
                                               & vlSelfRef.__PVT__voice_ctrl[0U])
                                               ? 0U
                                               : ((0x00000010U 
                                                   & vlSelfRef.__PVT__voice_ctrl[0U])
                                                   ? 0U
                                                   : (IData)(vlSelfRef.__PVT__v1__DOT__noise)))));
    } else if ((0x00000040U & vlSelfRef.__PVT__voice_ctrl[0U])) {
        if ((0x00000020U & vlSelfRef.__PVT__voice_ctrl[0U])) {
            vlSelfRef.__PVT__v1__DOT__comb = ((0x00000010U 
                                               & vlSelfRef.__PVT__voice_ctrl[0U])
                                               ? (vlSelfRef.__PVT__pst_out[0U] 
                                                  & (- (IData)((IData)(vlSelfRef.__PVT__v1__DOT__pulse))))
                                               : (vlSelfRef.__PVT__ps___05Fout[0U] 
                                                  & (- (IData)((IData)(vlSelfRef.__PVT__v1__DOT__pulse)))));
            vlSelfRef.__PVT__v1__DOT__norm = (0x00000fffU 
                                              & 0U);
        } else if ((0x00000010U & vlSelfRef.__PVT__voice_ctrl[0U])) {
            vlSelfRef.__PVT__v1__DOT__comb = (vlSelfRef.__PVT__p_t_out[0U] 
                                              & (- (IData)((IData)(vlSelfRef.__PVT__v1__DOT__pulse))));
            vlSelfRef.__PVT__v1__DOT__norm = (0x00000fffU 
                                              & 0U);
        } else {
            vlSelfRef.__PVT__v1__DOT__comb = 0U;
            vlSelfRef.__PVT__v1__DOT__norm = (0x00000fffU 
                                              & (- (IData)((IData)(vlSelfRef.__PVT__v1__DOT__pulse))));
        }
    } else if ((0x00000020U & vlSelfRef.__PVT__voice_ctrl[0U])) {
        if ((0x00000010U & vlSelfRef.__PVT__voice_ctrl[0U])) {
            vlSelfRef.__PVT__v1__DOT__comb = vlSelfRef.__PVT___st_out[0U];
            vlSelfRef.__PVT__v1__DOT__norm = (0x00000fffU 
                                              & 0U);
        } else {
            vlSelfRef.__PVT__v1__DOT__comb = 0U;
            vlSelfRef.__PVT__v1__DOT__norm = (0x00000fffU 
                                              & (IData)(vlSelfRef.__PVT__v1__DOT__saw_tri));
        }
    } else {
        vlSelfRef.__PVT__v1__DOT__comb = 0U;
        vlSelfRef.__PVT__v1__DOT__norm = (0x00000fffU 
                                          & ((0x00000010U 
                                              & vlSelfRef.__PVT__voice_ctrl[0U])
                                              ? VL_SHIFTL_III(12,12,32, (IData)(vlSelfRef.__PVT__v1__DOT__saw_tri), 1U)
                                              : 0U));
    }
    if ((0x00000080U & vlSelfRef.__PVT__voice_ctrl[1U])) {
        vlSelfRef.__PVT__v2__DOT__comb = 0U;
        vlSelfRef.__PVT__v2__DOT__norm = (0x00000fffU 
                                          & ((0x00000040U 
                                              & vlSelfRef.__PVT__voice_ctrl[1U])
                                              ? 0U : 
                                             ((0x00000020U 
                                               & vlSelfRef.__PVT__voice_ctrl[1U])
                                               ? 0U
                                               : ((0x00000010U 
                                                   & vlSelfRef.__PVT__voice_ctrl[1U])
                                                   ? 0U
                                                   : (IData)(vlSelfRef.__PVT__v2__DOT__noise)))));
    } else if ((0x00000040U & vlSelfRef.__PVT__voice_ctrl[1U])) {
        if ((0x00000020U & vlSelfRef.__PVT__voice_ctrl[1U])) {
            vlSelfRef.__PVT__v2__DOT__comb = ((0x00000010U 
                                               & vlSelfRef.__PVT__voice_ctrl[1U])
                                               ? (vlSelfRef.__PVT__pst_out[1U] 
                                                  & (- (IData)((IData)(vlSelfRef.__PVT__v2__DOT__pulse))))
                                               : (vlSelfRef.__PVT__ps___05Fout[1U] 
                                                  & (- (IData)((IData)(vlSelfRef.__PVT__v2__DOT__pulse)))));
            vlSelfRef.__PVT__v2__DOT__norm = (0x00000fffU 
                                              & 0U);
        } else if ((0x00000010U & vlSelfRef.__PVT__voice_ctrl[1U])) {
            vlSelfRef.__PVT__v2__DOT__comb = (vlSelfRef.__PVT__p_t_out[1U] 
                                              & (- (IData)((IData)(vlSelfRef.__PVT__v2__DOT__pulse))));
            vlSelfRef.__PVT__v2__DOT__norm = (0x00000fffU 
                                              & 0U);
        } else {
            vlSelfRef.__PVT__v2__DOT__comb = 0U;
            vlSelfRef.__PVT__v2__DOT__norm = (0x00000fffU 
                                              & (- (IData)((IData)(vlSelfRef.__PVT__v2__DOT__pulse))));
        }
    } else if ((0x00000020U & vlSelfRef.__PVT__voice_ctrl[1U])) {
        if ((0x00000010U & vlSelfRef.__PVT__voice_ctrl[1U])) {
            vlSelfRef.__PVT__v2__DOT__comb = vlSelfRef.__PVT___st_out[1U];
            vlSelfRef.__PVT__v2__DOT__norm = (0x00000fffU 
                                              & 0U);
        } else {
            vlSelfRef.__PVT__v2__DOT__comb = 0U;
            vlSelfRef.__PVT__v2__DOT__norm = (0x00000fffU 
                                              & (IData)(vlSelfRef.__PVT__v2__DOT__saw_tri));
        }
    } else {
        vlSelfRef.__PVT__v2__DOT__comb = 0U;
        vlSelfRef.__PVT__v2__DOT__norm = (0x00000fffU 
                                          & ((0x00000010U 
                                              & vlSelfRef.__PVT__voice_ctrl[1U])
                                              ? VL_SHIFTL_III(12,12,32, (IData)(vlSelfRef.__PVT__v2__DOT__saw_tri), 1U)
                                              : 0U));
    }
    if ((0x00000080U & vlSelfRef.__PVT__voice_ctrl[2U])) {
        vlSelfRef.__PVT__v3__DOT__comb = 0U;
        vlSelfRef.__PVT__v3__DOT__norm = (0x00000fffU 
                                          & ((0x00000040U 
                                              & vlSelfRef.__PVT__voice_ctrl[2U])
                                              ? 0U : 
                                             ((0x00000020U 
                                               & vlSelfRef.__PVT__voice_ctrl[2U])
                                               ? 0U
                                               : ((0x00000010U 
                                                   & vlSelfRef.__PVT__voice_ctrl[2U])
                                                   ? 0U
                                                   : (IData)(vlSelfRef.__PVT__v3__DOT__noise)))));
    } else if ((0x00000040U & vlSelfRef.__PVT__voice_ctrl[2U])) {
        if ((0x00000020U & vlSelfRef.__PVT__voice_ctrl[2U])) {
            vlSelfRef.__PVT__v3__DOT__comb = ((0x00000010U 
                                               & vlSelfRef.__PVT__voice_ctrl[2U])
                                               ? (vlSelfRef.__PVT__pst_out[2U] 
                                                  & (- (IData)((IData)(vlSelfRef.__PVT__v3__DOT__pulse))))
                                               : (vlSelfRef.__PVT__ps___05Fout[2U] 
                                                  & (- (IData)((IData)(vlSelfRef.__PVT__v3__DOT__pulse)))));
            vlSelfRef.__PVT__v3__DOT__norm = (0x00000fffU 
                                              & 0U);
        } else if ((0x00000010U & vlSelfRef.__PVT__voice_ctrl[2U])) {
            vlSelfRef.__PVT__v3__DOT__comb = (vlSelfRef.__PVT__p_t_out[2U] 
                                              & (- (IData)((IData)(vlSelfRef.__PVT__v3__DOT__pulse))));
            vlSelfRef.__PVT__v3__DOT__norm = (0x00000fffU 
                                              & 0U);
        } else {
            vlSelfRef.__PVT__v3__DOT__comb = 0U;
            vlSelfRef.__PVT__v3__DOT__norm = (0x00000fffU 
                                              & (- (IData)((IData)(vlSelfRef.__PVT__v3__DOT__pulse))));
        }
    } else if ((0x00000020U & vlSelfRef.__PVT__voice_ctrl[2U])) {
        if ((0x00000010U & vlSelfRef.__PVT__voice_ctrl[2U])) {
            vlSelfRef.__PVT__v3__DOT__comb = vlSelfRef.__PVT___st_out[2U];
            vlSelfRef.__PVT__v3__DOT__norm = (0x00000fffU 
                                              & 0U);
        } else {
            vlSelfRef.__PVT__v3__DOT__comb = 0U;
            vlSelfRef.__PVT__v3__DOT__norm = (0x00000fffU 
                                              & (IData)(vlSelfRef.__PVT__v3__DOT__saw_tri));
        }
    } else {
        vlSelfRef.__PVT__v3__DOT__comb = 0U;
        vlSelfRef.__PVT__v3__DOT__norm = (0x00000fffU 
                                          & ((0x00000010U 
                                              & vlSelfRef.__PVT__voice_ctrl[2U])
                                              ? VL_SHIFTL_III(12,12,32, (IData)(vlSelfRef.__PVT__v3__DOT__saw_tri), 1U)
                                              : 0U));
    }
    vlSelfRef.__Vcellout__v1__acc_t = ((0x00000800U 
                                        & (vlSelfRef.__PVT__v1__DOT__oscillator 
                                           >> 0x0000000cU)) 
                                       | (0x000007ffU 
                                          & ((vlSelfRef.__PVT__v1__DOT__oscillator 
                                              >> 0x0000000cU) 
                                             ^ (- (IData)(
                                                          (1U 
                                                           & ((~ 
                                                               (vlSelfRef.__PVT__voice_ctrl[0U] 
                                                                >> 5U)) 
                                                              & (((~ 
                                                                   (vlSelfRef.__PVT__v3__DOT__oscillator 
                                                                    >> 0x00000017U)) 
                                                                  & (vlSelfRef.__PVT__voice_ctrl[0U] 
                                                                     >> 2U)) 
                                                                 ^ 
                                                                 (vlSelfRef.__PVT__v1__DOT__oscillator 
                                                                  >> 0x00000017U)))))))));
    vlSelfRef.__Vcellout__v2__acc_t = ((0x00000800U 
                                        & (vlSelfRef.__PVT__v2__DOT__oscillator 
                                           >> 0x0000000cU)) 
                                       | (0x000007ffU 
                                          & ((vlSelfRef.__PVT__v2__DOT__oscillator 
                                              >> 0x0000000cU) 
                                             ^ (- (IData)(
                                                          (1U 
                                                           & ((~ 
                                                               (vlSelfRef.__PVT__voice_ctrl[1U] 
                                                                >> 5U)) 
                                                              & (((~ 
                                                                   (vlSelfRef.__PVT__v1__DOT__oscillator 
                                                                    >> 0x00000017U)) 
                                                                  & (vlSelfRef.__PVT__voice_ctrl[1U] 
                                                                     >> 2U)) 
                                                                 ^ 
                                                                 (vlSelfRef.__PVT__v2__DOT__oscillator 
                                                                  >> 0x00000017U)))))))));
    vlSelfRef.__Vcellout__v3__acc_t = ((0x00000800U 
                                        & (vlSelfRef.__PVT__v3__DOT__oscillator 
                                           >> 0x0000000cU)) 
                                       | (0x000007ffU 
                                          & ((vlSelfRef.__PVT__v3__DOT__oscillator 
                                              >> 0x0000000cU) 
                                             ^ (- (IData)(
                                                          (1U 
                                                           & ((~ 
                                                               (vlSelfRef.__PVT__voice_ctrl[2U] 
                                                                >> 5U)) 
                                                              & (((~ 
                                                                   (vlSelfRef.__PVT__v2__DOT__oscillator 
                                                                    >> 0x00000017U)) 
                                                                  & (vlSelfRef.__PVT__voice_ctrl[2U] 
                                                                     >> 2U)) 
                                                                 ^ 
                                                                 (vlSelfRef.__PVT__v3__DOT__oscillator 
                                                                  >> 0x00000017U)))))))));
    vlSelfRef.__PVT__v1__DOT__envelope_dac__DOT__bitsum[0U] 
        = (0x00000fffU & ((IData)(8U) + ((1U & (IData)(vlSelfRef.__PVT__v1__DOT__envelope))
                                          ? vlSelfRef.__PVT__v1__DOT__envelope_dac__DOT__bitval[0U]
                                          : 0U)));
    vlSelfRef.__PVT__v2__DOT__envelope_dac__DOT__bitsum[0U] 
        = (0x00000fffU & ((IData)(8U) + ((1U & (IData)(vlSelfRef.__PVT__v2__DOT__envelope))
                                          ? vlSelfRef.__PVT__v2__DOT__envelope_dac__DOT__bitval[0U]
                                          : 0U)));
    vlSelfRef.__PVT__v3__DOT__envelope_dac__DOT__bitsum[0U] 
        = (0x00000fffU & ((IData)(8U) + ((1U & (IData)(vlSelfRef.__PVT__v3__DOT__envelope))
                                          ? vlSelfRef.__PVT__v3__DOT__envelope_dac__DOT__bitval[0U]
                                          : 0U)));
    __PVT__flt__DOT__m = VL_MULS_III(32, VL_EXTENDS_II(32,16, (IData)(vlSelfRef.__PVT__flt__DOT__a)), 
                                     VL_EXTENDS_II(32,16, (IData)(vlSelfRef.__PVT__flt__DOT__b)));
    vlSelfRef.dout = ((0x00000010U & (IData)(vlSymsp->TOP.top__DOT__cpu_addr))
                       ? ((8U & (IData)(vlSymsp->TOP.top__DOT__cpu_addr))
                           ? ((4U & (IData)(vlSymsp->TOP.top__DOT__cpu_addr))
                               ? ((2U & (IData)(vlSymsp->TOP.top__DOT__cpu_addr))
                                   ? ((1U & (IData)(vlSymsp->TOP.top__DOT__cpu_addr))
                                       ? vlSelfRef.__PVT__voice_vol[2U]
                                       : vlSelfRef.__PVT__voice_vol[1U])
                                   : ((1U & (IData)(vlSymsp->TOP.top__DOT__cpu_addr))
                                       ? vlSelfRef.__PVT__voice_vol[0U]
                                       : (IData)(vlSelfRef.__PVT__v3__DOT__envelope)))
                               : ((2U & (IData)(vlSymsp->TOP.top__DOT__cpu_addr))
                                   ? ((1U & (IData)(vlSymsp->TOP.top__DOT__cpu_addr))
                                       ? (IData)(vlSelfRef.__PVT__v3__DOT__wave_out)
                                       : 0U) : ((1U 
                                                 & (IData)(vlSymsp->TOP.top__DOT__cpu_addr))
                                                 ? 0U
                                                 : (IData)(vlSelfRef.__PVT__last_written))))
                           : (IData)(vlSelfRef.__PVT__last_written))
                       : (IData)(vlSelfRef.__PVT__last_written));
    vlSelfRef.cs = ((IData)(vlSymsp->TOP.top__DOT__cpu_we) 
                    & (IData)(vlSymsp->TOP.top__DOT__sid1_reg_sel));
    vlSelfRef.__PVT__acc_t[0U] = vlSelfRef.__Vcellout__v1__acc_t;
    vlSelfRef.__PVT__acc_t[1U] = vlSelfRef.__Vcellout__v2__acc_t;
    vlSelfRef.__PVT__acc_t[2U] = vlSelfRef.__Vcellout__v3__acc_t;
    vlSelfRef.__PVT__flt__DOT__o = ((IData)(vlSelfRef.__PVT__flt__DOT__s)
                                     ? (vlSelfRef.__PVT__flt__DOT__c 
                                        - __PVT__flt__DOT__m)
                                     : (vlSelfRef.__PVT__flt__DOT__c 
                                        + __PVT__flt__DOT__m));
    vlSelfRef.__PVT__v1__DOT__waveform_dac__DOT__bitsum[0U] 
        = (0x0000ffffU & ((IData)(8U) + ((1U & (IData)(vlSelfRef.__PVT__v1__DOT__norm))
                                          ? vlSelfRef.__PVT__v1__DOT__waveform_dac__DOT__bitval[0U]
                                          : 0U)));
    vlSelfRef.__PVT__v2__DOT__waveform_dac__DOT__bitsum[0U] 
        = (0x0000ffffU & ((IData)(8U) + ((1U & (IData)(vlSelfRef.__PVT__v2__DOT__norm))
                                          ? vlSelfRef.__PVT__v2__DOT__waveform_dac__DOT__bitval[0U]
                                          : 0U)));
    vlSelfRef.__PVT__v3__DOT__waveform_dac__DOT__bitsum[0U] 
        = (0x0000ffffU & ((IData)(8U) + ((1U & (IData)(vlSelfRef.__PVT__v3__DOT__norm))
                                          ? vlSelfRef.__PVT__v3__DOT__waveform_dac__DOT__bitval[0U]
                                          : 0U)));
    vlSelfRef.__PVT__flt__DOT__vhp_next = (0x0000ffffU 
                                           & ((1U & 
                                               VL_REDXOR_32(
                                                            (3U 
                                                             & (vlSelfRef.__PVT__flt__DOT__o 
                                                                >> 0x00000019U))))
                                               ? ((0x00008000U 
                                                   & (vlSelfRef.__PVT__flt__DOT__o 
                                                      >> 0x0000000bU)) 
                                                  | (0x00007fffU 
                                                     & (- (IData)(
                                                                  (1U 
                                                                   & (vlSelfRef.__PVT__flt__DOT__o 
                                                                      >> 0x00000019U))))))
                                               : (vlSelfRef.__PVT__flt__DOT__o 
                                                  >> 0x0000000aU)));
    vlSelfRef.__VdfgRegularize_hebeb780c_0_29 = (0x0001ffffU 
                                                 & (VL_EXTENDS_II(17,16, (IData)(vlSelfRef.__PVT__flt__DOT__vlp)) 
                                                    + 
                                                    VL_SHIFTRS_III(17,32,32, vlSelfRef.__PVT__flt__DOT__o, 0x00000011U)));
    __VdfgRegularize_hebeb780c_0_30 = (0x0001ffffU 
                                       & (VL_EXTENDS_II(17,16, (IData)(vlSelfRef.__PVT__flt__DOT__vbp)) 
                                          + VL_SHIFTRS_III(17,32,32, vlSelfRef.__PVT__flt__DOT__o, 0x00000011U)));
    vlSelfRef.__PVT__flt__DOT__vbp_next = (0x0000ffffU 
                                           & ((1U & 
                                               VL_REDXOR_32(
                                                            (3U 
                                                             & (__VdfgRegularize_hebeb780c_0_30 
                                                                >> 0x0000000fU))))
                                               ? ((0x00008000U 
                                                   & (__VdfgRegularize_hebeb780c_0_30 
                                                      >> 1U)) 
                                                  | (0x00007fffU 
                                                     & (- (IData)(
                                                                  (1U 
                                                                   & (__VdfgRegularize_hebeb780c_0_30 
                                                                      >> 0x0000000fU))))))
                                               : __VdfgRegularize_hebeb780c_0_30));
}

VL_ATTR_COLD void Vtop_sid_chip___stl_sequent__TOP__top__DOT__sid2_inst__0(Vtop_sid_chip* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+        Vtop_sid_chip___stl_sequent__TOP__top__DOT__sid2_inst__0\n"); );
    Vtop__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
    // Locals
    IData/*31:0*/ __PVT__flt__DOT__m;
    __PVT__flt__DOT__m = 0;
    IData/*16:0*/ __VdfgRegularize_hebeb780c_0_35;
    __VdfgRegularize_hebeb780c_0_35 = 0;
    // Body
    vlSelfRef.__PVT__v1_scaled = (0x003fffffU & VL_SHIFTRS_III(22,22,32, 
                                                               (0x003fffffU 
                                                                & VL_MULS_III(22, vlSelfRef.__PVT__v1__DOT__dca_out, 
                                                                              (0x003fffffU 
                                                                               & VL_EXTENDS_II(22,5, vlSelfRef.__PVT__voice_vol[0U])))), 4U));
    vlSelfRef.__PVT__v2_scaled = (0x003fffffU & VL_SHIFTRS_III(22,22,32, 
                                                               (0x003fffffU 
                                                                & VL_MULS_III(22, vlSelfRef.__PVT__v2__DOT__dca_out, 
                                                                              (0x003fffffU 
                                                                               & VL_EXTENDS_II(22,5, vlSelfRef.__PVT__voice_vol[1U])))), 4U));
    vlSelfRef.__PVT__v3_scaled = (0x003fffffU & VL_SHIFTRS_III(22,22,32, 
                                                               (0x003fffffU 
                                                                & VL_MULS_III(22, vlSelfRef.__PVT__v3__DOT__dca_out, 
                                                                              (0x003fffffU 
                                                                               & VL_EXTENDS_II(22,5, vlSelfRef.__PVT__voice_vol[2U])))), 4U));
    vlSelfRef.__PVT__v1__DOT__adsr__DOT__rate_period 
        = vlSelfRef.__PVT__v1__DOT__adsr__DOT__rates
        [(0x0000000fU & ((1U == (IData)(vlSelfRef.__PVT__v1__DOT__adsr__DOT__state))
                          ? (vlSelfRef.__PVT__voice_ad[0U] 
                             >> 4U) : ((2U == (IData)(vlSelfRef.__PVT__v1__DOT__adsr__DOT__state))
                                        ? vlSelfRef.__PVT__voice_ad[0U]
                                        : vlSelfRef.__PVT__voice_sr[0U])))];
    vlSelfRef.__PVT__v2__DOT__adsr__DOT__rate_period 
        = vlSelfRef.__PVT__v2__DOT__adsr__DOT__rates
        [(0x0000000fU & ((1U == (IData)(vlSelfRef.__PVT__v2__DOT__adsr__DOT__state))
                          ? (vlSelfRef.__PVT__voice_ad[1U] 
                             >> 4U) : ((2U == (IData)(vlSelfRef.__PVT__v2__DOT__adsr__DOT__state))
                                        ? vlSelfRef.__PVT__voice_ad[1U]
                                        : vlSelfRef.__PVT__voice_sr[1U])))];
    vlSelfRef.__PVT__v3__DOT__adsr__DOT__rate_period 
        = vlSelfRef.__PVT__v3__DOT__adsr__DOT__rates
        [(0x0000000fU & ((1U == (IData)(vlSelfRef.__PVT__v3__DOT__adsr__DOT__state))
                          ? (vlSelfRef.__PVT__voice_ad[2U] 
                             >> 4U) : ((2U == (IData)(vlSelfRef.__PVT__v3__DOT__adsr__DOT__state))
                                        ? vlSelfRef.__PVT__voice_ad[2U]
                                        : vlSelfRef.__PVT__voice_sr[2U])))];
    if ((0x00000080U & vlSelfRef.__PVT__voice_ctrl[0U])) {
        vlSelfRef.__PVT__v1__DOT__comb = 0U;
        vlSelfRef.__PVT__v1__DOT__norm = (0x00000fffU 
                                          & ((0x00000040U 
                                              & vlSelfRef.__PVT__voice_ctrl[0U])
                                              ? 0U : 
                                             ((0x00000020U 
                                               & vlSelfRef.__PVT__voice_ctrl[0U])
                                               ? 0U
                                               : ((0x00000010U 
                                                   & vlSelfRef.__PVT__voice_ctrl[0U])
                                                   ? 0U
                                                   : (IData)(vlSelfRef.__PVT__v1__DOT__noise)))));
    } else if ((0x00000040U & vlSelfRef.__PVT__voice_ctrl[0U])) {
        if ((0x00000020U & vlSelfRef.__PVT__voice_ctrl[0U])) {
            vlSelfRef.__PVT__v1__DOT__comb = ((0x00000010U 
                                               & vlSelfRef.__PVT__voice_ctrl[0U])
                                               ? (vlSelfRef.__PVT__pst_out[0U] 
                                                  & (- (IData)((IData)(vlSelfRef.__PVT__v1__DOT__pulse))))
                                               : (vlSelfRef.__PVT__ps___05Fout[0U] 
                                                  & (- (IData)((IData)(vlSelfRef.__PVT__v1__DOT__pulse)))));
            vlSelfRef.__PVT__v1__DOT__norm = (0x00000fffU 
                                              & 0U);
        } else if ((0x00000010U & vlSelfRef.__PVT__voice_ctrl[0U])) {
            vlSelfRef.__PVT__v1__DOT__comb = (vlSelfRef.__PVT__p_t_out[0U] 
                                              & (- (IData)((IData)(vlSelfRef.__PVT__v1__DOT__pulse))));
            vlSelfRef.__PVT__v1__DOT__norm = (0x00000fffU 
                                              & 0U);
        } else {
            vlSelfRef.__PVT__v1__DOT__comb = 0U;
            vlSelfRef.__PVT__v1__DOT__norm = (0x00000fffU 
                                              & (- (IData)((IData)(vlSelfRef.__PVT__v1__DOT__pulse))));
        }
    } else if ((0x00000020U & vlSelfRef.__PVT__voice_ctrl[0U])) {
        if ((0x00000010U & vlSelfRef.__PVT__voice_ctrl[0U])) {
            vlSelfRef.__PVT__v1__DOT__comb = vlSelfRef.__PVT___st_out[0U];
            vlSelfRef.__PVT__v1__DOT__norm = (0x00000fffU 
                                              & 0U);
        } else {
            vlSelfRef.__PVT__v1__DOT__comb = 0U;
            vlSelfRef.__PVT__v1__DOT__norm = (0x00000fffU 
                                              & (IData)(vlSelfRef.__PVT__v1__DOT__saw_tri));
        }
    } else {
        vlSelfRef.__PVT__v1__DOT__comb = 0U;
        vlSelfRef.__PVT__v1__DOT__norm = (0x00000fffU 
                                          & ((0x00000010U 
                                              & vlSelfRef.__PVT__voice_ctrl[0U])
                                              ? VL_SHIFTL_III(12,12,32, (IData)(vlSelfRef.__PVT__v1__DOT__saw_tri), 1U)
                                              : 0U));
    }
    if ((0x00000080U & vlSelfRef.__PVT__voice_ctrl[1U])) {
        vlSelfRef.__PVT__v2__DOT__comb = 0U;
        vlSelfRef.__PVT__v2__DOT__norm = (0x00000fffU 
                                          & ((0x00000040U 
                                              & vlSelfRef.__PVT__voice_ctrl[1U])
                                              ? 0U : 
                                             ((0x00000020U 
                                               & vlSelfRef.__PVT__voice_ctrl[1U])
                                               ? 0U
                                               : ((0x00000010U 
                                                   & vlSelfRef.__PVT__voice_ctrl[1U])
                                                   ? 0U
                                                   : (IData)(vlSelfRef.__PVT__v2__DOT__noise)))));
    } else if ((0x00000040U & vlSelfRef.__PVT__voice_ctrl[1U])) {
        if ((0x00000020U & vlSelfRef.__PVT__voice_ctrl[1U])) {
            vlSelfRef.__PVT__v2__DOT__comb = ((0x00000010U 
                                               & vlSelfRef.__PVT__voice_ctrl[1U])
                                               ? (vlSelfRef.__PVT__pst_out[1U] 
                                                  & (- (IData)((IData)(vlSelfRef.__PVT__v2__DOT__pulse))))
                                               : (vlSelfRef.__PVT__ps___05Fout[1U] 
                                                  & (- (IData)((IData)(vlSelfRef.__PVT__v2__DOT__pulse)))));
            vlSelfRef.__PVT__v2__DOT__norm = (0x00000fffU 
                                              & 0U);
        } else if ((0x00000010U & vlSelfRef.__PVT__voice_ctrl[1U])) {
            vlSelfRef.__PVT__v2__DOT__comb = (vlSelfRef.__PVT__p_t_out[1U] 
                                              & (- (IData)((IData)(vlSelfRef.__PVT__v2__DOT__pulse))));
            vlSelfRef.__PVT__v2__DOT__norm = (0x00000fffU 
                                              & 0U);
        } else {
            vlSelfRef.__PVT__v2__DOT__comb = 0U;
            vlSelfRef.__PVT__v2__DOT__norm = (0x00000fffU 
                                              & (- (IData)((IData)(vlSelfRef.__PVT__v2__DOT__pulse))));
        }
    } else if ((0x00000020U & vlSelfRef.__PVT__voice_ctrl[1U])) {
        if ((0x00000010U & vlSelfRef.__PVT__voice_ctrl[1U])) {
            vlSelfRef.__PVT__v2__DOT__comb = vlSelfRef.__PVT___st_out[1U];
            vlSelfRef.__PVT__v2__DOT__norm = (0x00000fffU 
                                              & 0U);
        } else {
            vlSelfRef.__PVT__v2__DOT__comb = 0U;
            vlSelfRef.__PVT__v2__DOT__norm = (0x00000fffU 
                                              & (IData)(vlSelfRef.__PVT__v2__DOT__saw_tri));
        }
    } else {
        vlSelfRef.__PVT__v2__DOT__comb = 0U;
        vlSelfRef.__PVT__v2__DOT__norm = (0x00000fffU 
                                          & ((0x00000010U 
                                              & vlSelfRef.__PVT__voice_ctrl[1U])
                                              ? VL_SHIFTL_III(12,12,32, (IData)(vlSelfRef.__PVT__v2__DOT__saw_tri), 1U)
                                              : 0U));
    }
    if ((0x00000080U & vlSelfRef.__PVT__voice_ctrl[2U])) {
        vlSelfRef.__PVT__v3__DOT__comb = 0U;
        vlSelfRef.__PVT__v3__DOT__norm = (0x00000fffU 
                                          & ((0x00000040U 
                                              & vlSelfRef.__PVT__voice_ctrl[2U])
                                              ? 0U : 
                                             ((0x00000020U 
                                               & vlSelfRef.__PVT__voice_ctrl[2U])
                                               ? 0U
                                               : ((0x00000010U 
                                                   & vlSelfRef.__PVT__voice_ctrl[2U])
                                                   ? 0U
                                                   : (IData)(vlSelfRef.__PVT__v3__DOT__noise)))));
    } else if ((0x00000040U & vlSelfRef.__PVT__voice_ctrl[2U])) {
        if ((0x00000020U & vlSelfRef.__PVT__voice_ctrl[2U])) {
            vlSelfRef.__PVT__v3__DOT__comb = ((0x00000010U 
                                               & vlSelfRef.__PVT__voice_ctrl[2U])
                                               ? (vlSelfRef.__PVT__pst_out[2U] 
                                                  & (- (IData)((IData)(vlSelfRef.__PVT__v3__DOT__pulse))))
                                               : (vlSelfRef.__PVT__ps___05Fout[2U] 
                                                  & (- (IData)((IData)(vlSelfRef.__PVT__v3__DOT__pulse)))));
            vlSelfRef.__PVT__v3__DOT__norm = (0x00000fffU 
                                              & 0U);
        } else if ((0x00000010U & vlSelfRef.__PVT__voice_ctrl[2U])) {
            vlSelfRef.__PVT__v3__DOT__comb = (vlSelfRef.__PVT__p_t_out[2U] 
                                              & (- (IData)((IData)(vlSelfRef.__PVT__v3__DOT__pulse))));
            vlSelfRef.__PVT__v3__DOT__norm = (0x00000fffU 
                                              & 0U);
        } else {
            vlSelfRef.__PVT__v3__DOT__comb = 0U;
            vlSelfRef.__PVT__v3__DOT__norm = (0x00000fffU 
                                              & (- (IData)((IData)(vlSelfRef.__PVT__v3__DOT__pulse))));
        }
    } else if ((0x00000020U & vlSelfRef.__PVT__voice_ctrl[2U])) {
        if ((0x00000010U & vlSelfRef.__PVT__voice_ctrl[2U])) {
            vlSelfRef.__PVT__v3__DOT__comb = vlSelfRef.__PVT___st_out[2U];
            vlSelfRef.__PVT__v3__DOT__norm = (0x00000fffU 
                                              & 0U);
        } else {
            vlSelfRef.__PVT__v3__DOT__comb = 0U;
            vlSelfRef.__PVT__v3__DOT__norm = (0x00000fffU 
                                              & (IData)(vlSelfRef.__PVT__v3__DOT__saw_tri));
        }
    } else {
        vlSelfRef.__PVT__v3__DOT__comb = 0U;
        vlSelfRef.__PVT__v3__DOT__norm = (0x00000fffU 
                                          & ((0x00000010U 
                                              & vlSelfRef.__PVT__voice_ctrl[2U])
                                              ? VL_SHIFTL_III(12,12,32, (IData)(vlSelfRef.__PVT__v3__DOT__saw_tri), 1U)
                                              : 0U));
    }
    vlSelfRef.__Vcellout__v1__acc_t = ((0x00000800U 
                                        & (vlSelfRef.__PVT__v1__DOT__oscillator 
                                           >> 0x0000000cU)) 
                                       | (0x000007ffU 
                                          & ((vlSelfRef.__PVT__v1__DOT__oscillator 
                                              >> 0x0000000cU) 
                                             ^ (- (IData)(
                                                          (1U 
                                                           & ((~ 
                                                               (vlSelfRef.__PVT__voice_ctrl[0U] 
                                                                >> 5U)) 
                                                              & (((~ 
                                                                   (vlSelfRef.__PVT__v3__DOT__oscillator 
                                                                    >> 0x00000017U)) 
                                                                  & (vlSelfRef.__PVT__voice_ctrl[0U] 
                                                                     >> 2U)) 
                                                                 ^ 
                                                                 (vlSelfRef.__PVT__v1__DOT__oscillator 
                                                                  >> 0x00000017U)))))))));
    vlSelfRef.__Vcellout__v2__acc_t = ((0x00000800U 
                                        & (vlSelfRef.__PVT__v2__DOT__oscillator 
                                           >> 0x0000000cU)) 
                                       | (0x000007ffU 
                                          & ((vlSelfRef.__PVT__v2__DOT__oscillator 
                                              >> 0x0000000cU) 
                                             ^ (- (IData)(
                                                          (1U 
                                                           & ((~ 
                                                               (vlSelfRef.__PVT__voice_ctrl[1U] 
                                                                >> 5U)) 
                                                              & (((~ 
                                                                   (vlSelfRef.__PVT__v1__DOT__oscillator 
                                                                    >> 0x00000017U)) 
                                                                  & (vlSelfRef.__PVT__voice_ctrl[1U] 
                                                                     >> 2U)) 
                                                                 ^ 
                                                                 (vlSelfRef.__PVT__v2__DOT__oscillator 
                                                                  >> 0x00000017U)))))))));
    vlSelfRef.__Vcellout__v3__acc_t = ((0x00000800U 
                                        & (vlSelfRef.__PVT__v3__DOT__oscillator 
                                           >> 0x0000000cU)) 
                                       | (0x000007ffU 
                                          & ((vlSelfRef.__PVT__v3__DOT__oscillator 
                                              >> 0x0000000cU) 
                                             ^ (- (IData)(
                                                          (1U 
                                                           & ((~ 
                                                               (vlSelfRef.__PVT__voice_ctrl[2U] 
                                                                >> 5U)) 
                                                              & (((~ 
                                                                   (vlSelfRef.__PVT__v2__DOT__oscillator 
                                                                    >> 0x00000017U)) 
                                                                  & (vlSelfRef.__PVT__voice_ctrl[2U] 
                                                                     >> 2U)) 
                                                                 ^ 
                                                                 (vlSelfRef.__PVT__v3__DOT__oscillator 
                                                                  >> 0x00000017U)))))))));
    vlSelfRef.__PVT__v1__DOT__envelope_dac__DOT__bitsum[0U] 
        = (0x00000fffU & ((IData)(8U) + ((1U & (IData)(vlSelfRef.__PVT__v1__DOT__envelope))
                                          ? vlSelfRef.__PVT__v1__DOT__envelope_dac__DOT__bitval[0U]
                                          : 0U)));
    vlSelfRef.__PVT__v2__DOT__envelope_dac__DOT__bitsum[0U] 
        = (0x00000fffU & ((IData)(8U) + ((1U & (IData)(vlSelfRef.__PVT__v2__DOT__envelope))
                                          ? vlSelfRef.__PVT__v2__DOT__envelope_dac__DOT__bitval[0U]
                                          : 0U)));
    vlSelfRef.__PVT__v3__DOT__envelope_dac__DOT__bitsum[0U] 
        = (0x00000fffU & ((IData)(8U) + ((1U & (IData)(vlSelfRef.__PVT__v3__DOT__envelope))
                                          ? vlSelfRef.__PVT__v3__DOT__envelope_dac__DOT__bitval[0U]
                                          : 0U)));
    __PVT__flt__DOT__m = VL_MULS_III(32, VL_EXTENDS_II(32,16, (IData)(vlSelfRef.__PVT__flt__DOT__a)), 
                                     VL_EXTENDS_II(32,16, (IData)(vlSelfRef.__PVT__flt__DOT__b)));
    vlSelfRef.dout = ((0x00000010U & (IData)(vlSymsp->TOP.top__DOT__cpu_addr))
                       ? ((8U & (IData)(vlSymsp->TOP.top__DOT__cpu_addr))
                           ? ((4U & (IData)(vlSymsp->TOP.top__DOT__cpu_addr))
                               ? ((2U & (IData)(vlSymsp->TOP.top__DOT__cpu_addr))
                                   ? ((1U & (IData)(vlSymsp->TOP.top__DOT__cpu_addr))
                                       ? vlSelfRef.__PVT__voice_vol[2U]
                                       : vlSelfRef.__PVT__voice_vol[1U])
                                   : ((1U & (IData)(vlSymsp->TOP.top__DOT__cpu_addr))
                                       ? vlSelfRef.__PVT__voice_vol[0U]
                                       : (IData)(vlSelfRef.__PVT__v3__DOT__envelope)))
                               : ((2U & (IData)(vlSymsp->TOP.top__DOT__cpu_addr))
                                   ? ((1U & (IData)(vlSymsp->TOP.top__DOT__cpu_addr))
                                       ? (IData)(vlSelfRef.__PVT__v3__DOT__wave_out)
                                       : 0U) : ((1U 
                                                 & (IData)(vlSymsp->TOP.top__DOT__cpu_addr))
                                                 ? 0U
                                                 : (IData)(vlSelfRef.__PVT__last_written))))
                           : (IData)(vlSelfRef.__PVT__last_written))
                       : (IData)(vlSelfRef.__PVT__last_written));
    vlSelfRef.cs = ((IData)(vlSymsp->TOP.top__DOT__cpu_we) 
                    & (IData)(vlSymsp->TOP.top__DOT__sid2_reg_sel));
    vlSelfRef.__PVT__acc_t[0U] = vlSelfRef.__Vcellout__v1__acc_t;
    vlSelfRef.__PVT__acc_t[1U] = vlSelfRef.__Vcellout__v2__acc_t;
    vlSelfRef.__PVT__acc_t[2U] = vlSelfRef.__Vcellout__v3__acc_t;
    vlSelfRef.__PVT__flt__DOT__o = ((IData)(vlSelfRef.__PVT__flt__DOT__s)
                                     ? (vlSelfRef.__PVT__flt__DOT__c 
                                        - __PVT__flt__DOT__m)
                                     : (vlSelfRef.__PVT__flt__DOT__c 
                                        + __PVT__flt__DOT__m));
    vlSelfRef.__PVT__v1__DOT__waveform_dac__DOT__bitsum[0U] 
        = (0x0000ffffU & ((IData)(8U) + ((1U & (IData)(vlSelfRef.__PVT__v1__DOT__norm))
                                          ? vlSelfRef.__PVT__v1__DOT__waveform_dac__DOT__bitval[0U]
                                          : 0U)));
    vlSelfRef.__PVT__v2__DOT__waveform_dac__DOT__bitsum[0U] 
        = (0x0000ffffU & ((IData)(8U) + ((1U & (IData)(vlSelfRef.__PVT__v2__DOT__norm))
                                          ? vlSelfRef.__PVT__v2__DOT__waveform_dac__DOT__bitval[0U]
                                          : 0U)));
    vlSelfRef.__PVT__v3__DOT__waveform_dac__DOT__bitsum[0U] 
        = (0x0000ffffU & ((IData)(8U) + ((1U & (IData)(vlSelfRef.__PVT__v3__DOT__norm))
                                          ? vlSelfRef.__PVT__v3__DOT__waveform_dac__DOT__bitval[0U]
                                          : 0U)));
    vlSelfRef.__PVT__flt__DOT__vhp_next = (0x0000ffffU 
                                           & ((1U & 
                                               VL_REDXOR_32(
                                                            (3U 
                                                             & (vlSelfRef.__PVT__flt__DOT__o 
                                                                >> 0x00000019U))))
                                               ? ((0x00008000U 
                                                   & (vlSelfRef.__PVT__flt__DOT__o 
                                                      >> 0x0000000bU)) 
                                                  | (0x00007fffU 
                                                     & (- (IData)(
                                                                  (1U 
                                                                   & (vlSelfRef.__PVT__flt__DOT__o 
                                                                      >> 0x00000019U))))))
                                               : (vlSelfRef.__PVT__flt__DOT__o 
                                                  >> 0x0000000aU)));
    vlSelfRef.__VdfgRegularize_hebeb780c_0_34 = (0x0001ffffU 
                                                 & (VL_EXTENDS_II(17,16, (IData)(vlSelfRef.__PVT__flt__DOT__vlp)) 
                                                    + 
                                                    VL_SHIFTRS_III(17,32,32, vlSelfRef.__PVT__flt__DOT__o, 0x00000011U)));
    __VdfgRegularize_hebeb780c_0_35 = (0x0001ffffU 
                                       & (VL_EXTENDS_II(17,16, (IData)(vlSelfRef.__PVT__flt__DOT__vbp)) 
                                          + VL_SHIFTRS_III(17,32,32, vlSelfRef.__PVT__flt__DOT__o, 0x00000011U)));
    vlSelfRef.__PVT__flt__DOT__vbp_next = (0x0000ffffU 
                                           & ((1U & 
                                               VL_REDXOR_32(
                                                            (3U 
                                                             & (__VdfgRegularize_hebeb780c_0_35 
                                                                >> 0x0000000fU))))
                                               ? ((0x00008000U 
                                                   & (__VdfgRegularize_hebeb780c_0_35 
                                                      >> 1U)) 
                                                  | (0x00007fffU 
                                                     & (- (IData)(
                                                                  (1U 
                                                                   & (__VdfgRegularize_hebeb780c_0_35 
                                                                      >> 0x0000000fU))))))
                                               : __VdfgRegularize_hebeb780c_0_35));
}

VL_ATTR_COLD void Vtop_sid_chip___ctor_var_reset(Vtop_sid_chip* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+        Vtop_sid_chip___ctor_var_reset\n"); );
    Vtop__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
    // Body
    vlSelf->__Vlvbound_h0706378b__0 = 0;
    vlSelf->__Vlvbound_h061f25e8__0 = 0;
    vlSelf->__Vlvbound_h49d001ad__0 = 0;
    vlSelf->__Vlvbound_h9cc45699__0 = 0;
    const uint64_t __VscopeHash = VL_MURMUR64_HASH(vlSelf->vlNamep);
    vlSelf->clk = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 16707436170211756652ull);
    vlSelf->rst = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 18209466448985614591ull);
    vlSelf->ce_1m = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 8323039086517112625ull);
    vlSelf->mode = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 2288075164703132177ull);
    vlSelf->cs = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 14708086533280315920ull);
    vlSelf->we = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 10105644630884274164ull);
    vlSelf->addr = VL_SCOPED_RAND_RESET_I(5, __VscopeHash, 14934084843038794831ull);
    vlSelf->din = VL_SCOPED_RAND_RESET_I(8, __VscopeHash, 15192908731043726583ull);
    vlSelf->dout = VL_SCOPED_RAND_RESET_I(8, __VscopeHash, 11474705599699299244ull);
    vlSelf->audio_out = VL_SCOPED_RAND_RESET_I(18, __VscopeHash, 16887251779955964156ull);
    for (int __Vi0 = 0; __Vi0 < 3; ++__Vi0) {
        vlSelf->__PVT__voice_freq[__Vi0] = VL_SCOPED_RAND_RESET_I(16, __VscopeHash, 5674158667248430817ull);
    }
    for (int __Vi0 = 0; __Vi0 < 3; ++__Vi0) {
        vlSelf->__PVT__voice_pw[__Vi0] = VL_SCOPED_RAND_RESET_I(12, __VscopeHash, 13912891044270967782ull);
    }
    for (int __Vi0 = 0; __Vi0 < 3; ++__Vi0) {
        vlSelf->__PVT__voice_ctrl[__Vi0] = VL_SCOPED_RAND_RESET_I(8, __VscopeHash, 3927763226681510405ull);
    }
    for (int __Vi0 = 0; __Vi0 < 3; ++__Vi0) {
        vlSelf->__PVT__voice_ad[__Vi0] = VL_SCOPED_RAND_RESET_I(8, __VscopeHash, 10014838026615654054ull);
    }
    for (int __Vi0 = 0; __Vi0 < 3; ++__Vi0) {
        vlSelf->__PVT__voice_sr[__Vi0] = VL_SCOPED_RAND_RESET_I(8, __VscopeHash, 179894831434688378ull);
    }
    for (int __Vi0 = 0; __Vi0 < 3; ++__Vi0) {
        vlSelf->__PVT__voice_vol[__Vi0] = VL_SCOPED_RAND_RESET_I(4, __VscopeHash, 16670892421006096536ull);
    }
    vlSelf->__PVT__filter_fc = VL_SCOPED_RAND_RESET_I(11, __VscopeHash, 12262944355852939179ull);
    vlSelf->__PVT__filter_res_filt = VL_SCOPED_RAND_RESET_I(8, __VscopeHash, 6087046644373591126ull);
    vlSelf->__PVT__filter_mode_vol = VL_SCOPED_RAND_RESET_I(8, __VscopeHash, 11235163987292173295ull);
    vlSelf->__PVT__last_written = VL_SCOPED_RAND_RESET_I(8, __VscopeHash, 10001302048419511631ull);
    for (int __Vi0 = 0; __Vi0 < 6; ++__Vi0) {
        vlSelf->__PVT___st_out[__Vi0] = VL_SCOPED_RAND_RESET_I(8, __VscopeHash, 1539652712272720324ull);
    }
    for (int __Vi0 = 0; __Vi0 < 6; ++__Vi0) {
        vlSelf->__PVT__p_t_out[__Vi0] = VL_SCOPED_RAND_RESET_I(8, __VscopeHash, 12145869476987885677ull);
    }
    for (int __Vi0 = 0; __Vi0 < 6; ++__Vi0) {
        vlSelf->__PVT__ps___05Fout[__Vi0] = VL_SCOPED_RAND_RESET_I(8, __VscopeHash, 6572896360595613829ull);
    }
    for (int __Vi0 = 0; __Vi0 < 6; ++__Vi0) {
        vlSelf->__PVT__pst_out[__Vi0] = VL_SCOPED_RAND_RESET_I(8, __VscopeHash, 9447882782313380585ull);
    }
    for (int __Vi0 = 0; __Vi0 < 3; ++__Vi0) {
        vlSelf->__PVT__acc_t[__Vi0] = VL_SCOPED_RAND_RESET_I(12, __VscopeHash, 3028496917395193712ull);
    }
    vlSelf->__Vcellout__v1__acc_t = 0;
    vlSelf->__Vcellout__v2__acc_t = 0;
    vlSelf->__Vcellout__v3__acc_t = 0;
    vlSelf->__PVT__tbl_state = VL_SCOPED_RAND_RESET_I(4, __VscopeHash, 157994189516097160ull);
    vlSelf->__PVT__f___05Fst_out = VL_SCOPED_RAND_RESET_I(8, __VscopeHash, 4988811502038437411ull);
    vlSelf->__PVT__f_p_t_out = VL_SCOPED_RAND_RESET_I(8, __VscopeHash, 17047654495806003721ull);
    vlSelf->__PVT__f_ps___05Fout = VL_SCOPED_RAND_RESET_I(8, __VscopeHash, 295800434588401342ull);
    vlSelf->__PVT__f_pst_out = VL_SCOPED_RAND_RESET_I(8, __VscopeHash, 1897430251457553514ull);
    vlSelf->__PVT__f_acc_t = VL_SCOPED_RAND_RESET_I(12, __VscopeHash, 16337869985979126554ull);
    vlSelf->__PVT__v1_scaled = VL_SCOPED_RAND_RESET_I(22, __VscopeHash, 5804646615560262280ull);
    vlSelf->__PVT__v2_scaled = VL_SCOPED_RAND_RESET_I(22, __VscopeHash, 11677907157221132994ull);
    vlSelf->__PVT__v3_scaled = VL_SCOPED_RAND_RESET_I(22, __VscopeHash, 13962495431650791040ull);
    vlSelf->__PVT__v1__DOT__envelope = VL_SCOPED_RAND_RESET_I(8, __VscopeHash, 14863032983519674334ull);
    vlSelf->__PVT__v1__DOT__oscillator = VL_SCOPED_RAND_RESET_I(24, __VscopeHash, 15831603950675963240ull);
    vlSelf->__PVT__v1__DOT__osc_msb_in_prv = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 8736726312026101679ull);
    vlSelf->__PVT__v1__DOT__noise = VL_SCOPED_RAND_RESET_I(12, __VscopeHash, 7297070716366444181ull);
    vlSelf->__PVT__v1__DOT__saw_tri = VL_SCOPED_RAND_RESET_I(12, __VscopeHash, 14393513291466191672ull);
    vlSelf->__PVT__v1__DOT__pulse = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 12681961198262333520ull);
    vlSelf->__PVT__v1__DOT__norm = VL_SCOPED_RAND_RESET_I(12, __VscopeHash, 16701631510926411190ull);
    vlSelf->__PVT__v1__DOT__comb = VL_SCOPED_RAND_RESET_I(8, __VscopeHash, 14759526537263794834ull);
    vlSelf->__PVT__v1__DOT__norm_dac = VL_SCOPED_RAND_RESET_I(12, __VscopeHash, 2101419631692771548ull);
    vlSelf->__PVT__v1__DOT__wave_out = VL_SCOPED_RAND_RESET_I(8, __VscopeHash, 2502808260800324006ull);
    vlSelf->__PVT__v1__DOT__dca_out = VL_SCOPED_RAND_RESET_I(22, __VscopeHash, 15529383856821071085ull);
    vlSelf->__PVT__v1__DOT__adsr__DOT__state = VL_SCOPED_RAND_RESET_I(2, __VscopeHash, 14678764399922993614ull);
    for (int __Vi0 = 0; __Vi0 < 16; ++__Vi0) {
        vlSelf->__PVT__v1__DOT__adsr__DOT__rates[__Vi0] = VL_SCOPED_RAND_RESET_I(15, __VscopeHash, 15126657100688574818ull);
    }
    vlSelf->__PVT__v1__DOT__adsr__DOT__rate_period = VL_SCOPED_RAND_RESET_I(15, __VscopeHash, 6170039404440574161ull);
    for (int __Vi0 = 0; __Vi0 < 12; ++__Vi0) {
        vlSelf->__PVT__v1__DOT__waveform_dac__DOT__bitval[__Vi0] = VL_SCOPED_RAND_RESET_I(16, __VscopeHash, 13618739468602463833ull);
    }
    for (int __Vi0 = 0; __Vi0 < 12; ++__Vi0) {
        vlSelf->__PVT__v1__DOT__waveform_dac__DOT__bitsum[__Vi0] = VL_SCOPED_RAND_RESET_I(16, __VscopeHash, 789739246571562724ull);
    }
    for (int __Vi0 = 0; __Vi0 < 8; ++__Vi0) {
        vlSelf->__PVT__v1__DOT__envelope_dac__DOT__bitval[__Vi0] = VL_SCOPED_RAND_RESET_I(12, __VscopeHash, 6305801754303078541ull);
    }
    for (int __Vi0 = 0; __Vi0 < 8; ++__Vi0) {
        vlSelf->__PVT__v1__DOT__envelope_dac__DOT__bitsum[__Vi0] = VL_SCOPED_RAND_RESET_I(12, __VscopeHash, 15324351679845396054ull);
    }
    vlSelf->__PVT__v2__DOT__envelope = VL_SCOPED_RAND_RESET_I(8, __VscopeHash, 11376670908633974850ull);
    vlSelf->__PVT__v2__DOT__oscillator = VL_SCOPED_RAND_RESET_I(24, __VscopeHash, 13431001094720431433ull);
    vlSelf->__PVT__v2__DOT__osc_msb_in_prv = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 17535860822795819409ull);
    vlSelf->__PVT__v2__DOT__noise = VL_SCOPED_RAND_RESET_I(12, __VscopeHash, 2196632442276742970ull);
    vlSelf->__PVT__v2__DOT__saw_tri = VL_SCOPED_RAND_RESET_I(12, __VscopeHash, 11930049898112134956ull);
    vlSelf->__PVT__v2__DOT__pulse = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 2380474957946725164ull);
    vlSelf->__PVT__v2__DOT__norm = VL_SCOPED_RAND_RESET_I(12, __VscopeHash, 11114001736061547644ull);
    vlSelf->__PVT__v2__DOT__comb = VL_SCOPED_RAND_RESET_I(8, __VscopeHash, 11402033623069919121ull);
    vlSelf->__PVT__v2__DOT__norm_dac = VL_SCOPED_RAND_RESET_I(12, __VscopeHash, 13796110689076201010ull);
    vlSelf->__PVT__v2__DOT__wave_out = VL_SCOPED_RAND_RESET_I(8, __VscopeHash, 6342718653285469217ull);
    vlSelf->__PVT__v2__DOT__dca_out = VL_SCOPED_RAND_RESET_I(22, __VscopeHash, 10460621023059820718ull);
    vlSelf->__PVT__v2__DOT__adsr__DOT__state = VL_SCOPED_RAND_RESET_I(2, __VscopeHash, 11239655918917443999ull);
    for (int __Vi0 = 0; __Vi0 < 16; ++__Vi0) {
        vlSelf->__PVT__v2__DOT__adsr__DOT__rates[__Vi0] = VL_SCOPED_RAND_RESET_I(15, __VscopeHash, 15075565070275549835ull);
    }
    vlSelf->__PVT__v2__DOT__adsr__DOT__rate_period = VL_SCOPED_RAND_RESET_I(15, __VscopeHash, 15692213528348682960ull);
    for (int __Vi0 = 0; __Vi0 < 12; ++__Vi0) {
        vlSelf->__PVT__v2__DOT__waveform_dac__DOT__bitval[__Vi0] = VL_SCOPED_RAND_RESET_I(16, __VscopeHash, 7916625707208717376ull);
    }
    for (int __Vi0 = 0; __Vi0 < 12; ++__Vi0) {
        vlSelf->__PVT__v2__DOT__waveform_dac__DOT__bitsum[__Vi0] = VL_SCOPED_RAND_RESET_I(16, __VscopeHash, 18041854887223527169ull);
    }
    for (int __Vi0 = 0; __Vi0 < 8; ++__Vi0) {
        vlSelf->__PVT__v2__DOT__envelope_dac__DOT__bitval[__Vi0] = VL_SCOPED_RAND_RESET_I(12, __VscopeHash, 14850278860728503871ull);
    }
    for (int __Vi0 = 0; __Vi0 < 8; ++__Vi0) {
        vlSelf->__PVT__v2__DOT__envelope_dac__DOT__bitsum[__Vi0] = VL_SCOPED_RAND_RESET_I(12, __VscopeHash, 5149436845375682293ull);
    }
    vlSelf->__PVT__v3__DOT__envelope = VL_SCOPED_RAND_RESET_I(8, __VscopeHash, 10454259945875737078ull);
    vlSelf->__PVT__v3__DOT__oscillator = VL_SCOPED_RAND_RESET_I(24, __VscopeHash, 13932692669636441967ull);
    vlSelf->__PVT__v3__DOT__osc_msb_in_prv = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 8296423022298682355ull);
    vlSelf->__PVT__v3__DOT__noise = VL_SCOPED_RAND_RESET_I(12, __VscopeHash, 6702753520158052586ull);
    vlSelf->__PVT__v3__DOT__saw_tri = VL_SCOPED_RAND_RESET_I(12, __VscopeHash, 6164557834295209099ull);
    vlSelf->__PVT__v3__DOT__pulse = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 16120048275772805253ull);
    vlSelf->__PVT__v3__DOT__norm = VL_SCOPED_RAND_RESET_I(12, __VscopeHash, 641670438116587439ull);
    vlSelf->__PVT__v3__DOT__comb = VL_SCOPED_RAND_RESET_I(8, __VscopeHash, 11557220251293551618ull);
    vlSelf->__PVT__v3__DOT__norm_dac = VL_SCOPED_RAND_RESET_I(12, __VscopeHash, 17546496432258008711ull);
    vlSelf->__PVT__v3__DOT__wave_out = VL_SCOPED_RAND_RESET_I(8, __VscopeHash, 1577867965722497748ull);
    vlSelf->__PVT__v3__DOT__dca_out = VL_SCOPED_RAND_RESET_I(22, __VscopeHash, 2627477744486606263ull);
    vlSelf->__PVT__v3__DOT__adsr__DOT__state = VL_SCOPED_RAND_RESET_I(2, __VscopeHash, 13781508607937429484ull);
    for (int __Vi0 = 0; __Vi0 < 16; ++__Vi0) {
        vlSelf->__PVT__v3__DOT__adsr__DOT__rates[__Vi0] = VL_SCOPED_RAND_RESET_I(15, __VscopeHash, 11251804435663701958ull);
    }
    vlSelf->__PVT__v3__DOT__adsr__DOT__rate_period = VL_SCOPED_RAND_RESET_I(15, __VscopeHash, 9628181160218779739ull);
    for (int __Vi0 = 0; __Vi0 < 12; ++__Vi0) {
        vlSelf->__PVT__v3__DOT__waveform_dac__DOT__bitval[__Vi0] = VL_SCOPED_RAND_RESET_I(16, __VscopeHash, 14594127354971030471ull);
    }
    for (int __Vi0 = 0; __Vi0 < 12; ++__Vi0) {
        vlSelf->__PVT__v3__DOT__waveform_dac__DOT__bitsum[__Vi0] = VL_SCOPED_RAND_RESET_I(16, __VscopeHash, 13317939829574073676ull);
    }
    for (int __Vi0 = 0; __Vi0 < 8; ++__Vi0) {
        vlSelf->__PVT__v3__DOT__envelope_dac__DOT__bitval[__Vi0] = VL_SCOPED_RAND_RESET_I(12, __VscopeHash, 4771473357176359046ull);
    }
    for (int __Vi0 = 0; __Vi0 < 8; ++__Vi0) {
        vlSelf->__PVT__v3__DOT__envelope_dac__DOT__bitsum[__Vi0] = VL_SCOPED_RAND_RESET_I(12, __VscopeHash, 8429653570487432096ull);
    }
    for (int __Vi0 = 0; __Vi0 < 2048; ++__Vi0) {
        vlSelf->__PVT__tbl__DOT__wave6581_p_t[__Vi0] = VL_SCOPED_RAND_RESET_I(8, __VscopeHash, 1285189660289268640ull);
    }
    for (int __Vi0 = 0; __Vi0 < 2048; ++__Vi0) {
        vlSelf->__PVT__tbl__DOT__wave6581_ps_[__Vi0] = VL_SCOPED_RAND_RESET_I(8, __VscopeHash, 9070850448612987635ull);
    }
    for (int __Vi0 = 0; __Vi0 < 2048; ++__Vi0) {
        vlSelf->__PVT__tbl__DOT__wave8580_p_t[__Vi0] = VL_SCOPED_RAND_RESET_I(8, __VscopeHash, 1738649317733760692ull);
    }
    for (int __Vi0 = 0; __Vi0 < 4096; ++__Vi0) {
        vlSelf->__PVT__tbl__DOT__wave8580_ps_[__Vi0] = VL_SCOPED_RAND_RESET_I(8, __VscopeHash, 13330402949258984642ull);
    }
    vlSelf->__PVT__tbl__DOT__f0 = VL_SCOPED_RAND_RESET_I(16, __VscopeHash, 5343629585586610358ull);
    for (int __Vi0 = 0; __Vi0 < 4096; ++__Vi0) {
        vlSelf->__PVT__tbl__DOT__f6581_curve[__Vi0] = VL_SCOPED_RAND_RESET_I(16, __VscopeHash, 2857523334697179119ull);
    }
    for (int __Vi0 = 0; __Vi0 < 1024; ++__Vi0) {
        vlSelf->__PVT__tbl__DOT___unused_f6581_adj[__Vi0] = VL_SCOPED_RAND_RESET_I(15, __VscopeHash, 16311080468947043780ull);
    }
    for (int __Vi0 = 0; __Vi0 < 32; ++__Vi0) {
        vlSelf->__PVT__flt__DOT___1_Q_lsl10_tbl[__Vi0] = VL_SCOPED_RAND_RESET_I(11, __VscopeHash, 1047818712287250415ull);
    }
    vlSelf->__PVT__flt__DOT__c = VL_SCOPED_RAND_RESET_I(32, __VscopeHash, 1155235706057421034ull);
    vlSelf->__PVT__flt__DOT__s = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 10035088678202705702ull);
    vlSelf->__PVT__flt__DOT__a = VL_SCOPED_RAND_RESET_I(16, __VscopeHash, 2550257369303776005ull);
    vlSelf->__PVT__flt__DOT__b = VL_SCOPED_RAND_RESET_I(16, __VscopeHash, 6037354203535418503ull);
    vlSelf->__PVT__flt__DOT__o = VL_SCOPED_RAND_RESET_I(32, __VscopeHash, 13889240958928896348ull);
    vlSelf->__PVT__flt__DOT__vlp = VL_SCOPED_RAND_RESET_I(16, __VscopeHash, 7021170713199785837ull);
    vlSelf->__PVT__flt__DOT__vlp2 = VL_SCOPED_RAND_RESET_I(16, __VscopeHash, 5964676816587864095ull);
    vlSelf->__PVT__flt__DOT__vbp = VL_SCOPED_RAND_RESET_I(16, __VscopeHash, 14618780866352364095ull);
    vlSelf->__PVT__flt__DOT__vbp2 = VL_SCOPED_RAND_RESET_I(16, __VscopeHash, 3487125247627173507ull);
    vlSelf->__PVT__flt__DOT__vbp_next = VL_SCOPED_RAND_RESET_I(16, __VscopeHash, 8213166201014812424ull);
    vlSelf->__PVT__flt__DOT__vhp = VL_SCOPED_RAND_RESET_I(16, __VscopeHash, 10429001399977854098ull);
    vlSelf->__PVT__flt__DOT__vhp2 = VL_SCOPED_RAND_RESET_I(16, __VscopeHash, 15766789446829765252ull);
    vlSelf->__PVT__flt__DOT__vhp_next = VL_SCOPED_RAND_RESET_I(16, __VscopeHash, 436287333358097377ull);
    vlSelf->__VdfgRegularize_hebeb780c_0_29 = 0;
    vlSelf->__VdfgRegularize_hebeb780c_0_34 = 0;
}
