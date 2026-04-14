// Verilated -*- C++ -*-
// DESCRIPTION: Verilator output: Design implementation internals
// See Vtop.h for the primary calling header

#include "Vtop__pch.h"

void Vtop_sid_chip___ico_sequent__TOP__top__DOT__sid1_inst__0(Vtop_sid_chip* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+        Vtop_sid_chip___ico_sequent__TOP__top__DOT__sid1_inst__0\n"); );
    Vtop__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
    // Body
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
}

void Vtop_sid_chip___act_sequent__TOP__top__DOT__sid1_inst__0(Vtop_sid_chip* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+        Vtop_sid_chip___act_sequent__TOP__top__DOT__sid1_inst__0\n"); );
    Vtop__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
    // Body
    vlSelfRef.__PVT__v1__DOT__envelope_dac__DOT__bitsum[1U] 
        = (0x00000fffU & (vlSelfRef.__PVT__v1__DOT__envelope_dac__DOT__bitsum[0U] 
                          + ((2U & (IData)(vlSelfRef.__PVT__v1__DOT__envelope))
                              ? vlSelfRef.__PVT__v1__DOT__envelope_dac__DOT__bitval[1U]
                              : 0U)));
    vlSelfRef.__PVT__v1__DOT__envelope_dac__DOT__bitsum[2U] 
        = (0x00000fffU & (vlSelfRef.__PVT__v1__DOT__envelope_dac__DOT__bitsum[1U] 
                          + ((4U & (IData)(vlSelfRef.__PVT__v1__DOT__envelope))
                              ? vlSelfRef.__PVT__v1__DOT__envelope_dac__DOT__bitval[2U]
                              : 0U)));
    vlSelfRef.__PVT__v1__DOT__envelope_dac__DOT__bitsum[3U] 
        = (0x00000fffU & (vlSelfRef.__PVT__v1__DOT__envelope_dac__DOT__bitsum[2U] 
                          + ((8U & (IData)(vlSelfRef.__PVT__v1__DOT__envelope))
                              ? vlSelfRef.__PVT__v1__DOT__envelope_dac__DOT__bitval[3U]
                              : 0U)));
    vlSelfRef.__PVT__v1__DOT__envelope_dac__DOT__bitsum[4U] 
        = (0x00000fffU & (vlSelfRef.__PVT__v1__DOT__envelope_dac__DOT__bitsum[3U] 
                          + ((0x00000010U & (IData)(vlSelfRef.__PVT__v1__DOT__envelope))
                              ? vlSelfRef.__PVT__v1__DOT__envelope_dac__DOT__bitval[4U]
                              : 0U)));
    vlSelfRef.__PVT__v1__DOT__envelope_dac__DOT__bitsum[5U] 
        = (0x00000fffU & (vlSelfRef.__PVT__v1__DOT__envelope_dac__DOT__bitsum[4U] 
                          + ((0x00000020U & (IData)(vlSelfRef.__PVT__v1__DOT__envelope))
                              ? vlSelfRef.__PVT__v1__DOT__envelope_dac__DOT__bitval[5U]
                              : 0U)));
    vlSelfRef.__PVT__v1__DOT__envelope_dac__DOT__bitsum[6U] 
        = (0x00000fffU & (vlSelfRef.__PVT__v1__DOT__envelope_dac__DOT__bitsum[5U] 
                          + ((0x00000040U & (IData)(vlSelfRef.__PVT__v1__DOT__envelope))
                              ? vlSelfRef.__PVT__v1__DOT__envelope_dac__DOT__bitval[6U]
                              : 0U)));
    vlSelfRef.__PVT__v1__DOT__envelope_dac__DOT__bitsum[7U] 
        = (0x00000fffU & (vlSelfRef.__PVT__v1__DOT__envelope_dac__DOT__bitsum[6U] 
                          + ((0x00000080U & (IData)(vlSelfRef.__PVT__v1__DOT__envelope))
                              ? vlSelfRef.__PVT__v1__DOT__envelope_dac__DOT__bitval[7U]
                              : 0U)));
}

void Vtop_sid_chip___act_sequent__TOP__top__DOT__sid1_inst__1(Vtop_sid_chip* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+        Vtop_sid_chip___act_sequent__TOP__top__DOT__sid1_inst__1\n"); );
    Vtop__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
    // Body
    vlSelfRef.__PVT__v2__DOT__envelope_dac__DOT__bitsum[1U] 
        = (0x00000fffU & (vlSelfRef.__PVT__v2__DOT__envelope_dac__DOT__bitsum[0U] 
                          + ((2U & (IData)(vlSelfRef.__PVT__v2__DOT__envelope))
                              ? vlSelfRef.__PVT__v2__DOT__envelope_dac__DOT__bitval[1U]
                              : 0U)));
    vlSelfRef.__PVT__v2__DOT__envelope_dac__DOT__bitsum[2U] 
        = (0x00000fffU & (vlSelfRef.__PVT__v2__DOT__envelope_dac__DOT__bitsum[1U] 
                          + ((4U & (IData)(vlSelfRef.__PVT__v2__DOT__envelope))
                              ? vlSelfRef.__PVT__v2__DOT__envelope_dac__DOT__bitval[2U]
                              : 0U)));
    vlSelfRef.__PVT__v2__DOT__envelope_dac__DOT__bitsum[3U] 
        = (0x00000fffU & (vlSelfRef.__PVT__v2__DOT__envelope_dac__DOT__bitsum[2U] 
                          + ((8U & (IData)(vlSelfRef.__PVT__v2__DOT__envelope))
                              ? vlSelfRef.__PVT__v2__DOT__envelope_dac__DOT__bitval[3U]
                              : 0U)));
    vlSelfRef.__PVT__v2__DOT__envelope_dac__DOT__bitsum[4U] 
        = (0x00000fffU & (vlSelfRef.__PVT__v2__DOT__envelope_dac__DOT__bitsum[3U] 
                          + ((0x00000010U & (IData)(vlSelfRef.__PVT__v2__DOT__envelope))
                              ? vlSelfRef.__PVT__v2__DOT__envelope_dac__DOT__bitval[4U]
                              : 0U)));
    vlSelfRef.__PVT__v2__DOT__envelope_dac__DOT__bitsum[5U] 
        = (0x00000fffU & (vlSelfRef.__PVT__v2__DOT__envelope_dac__DOT__bitsum[4U] 
                          + ((0x00000020U & (IData)(vlSelfRef.__PVT__v2__DOT__envelope))
                              ? vlSelfRef.__PVT__v2__DOT__envelope_dac__DOT__bitval[5U]
                              : 0U)));
    vlSelfRef.__PVT__v2__DOT__envelope_dac__DOT__bitsum[6U] 
        = (0x00000fffU & (vlSelfRef.__PVT__v2__DOT__envelope_dac__DOT__bitsum[5U] 
                          + ((0x00000040U & (IData)(vlSelfRef.__PVT__v2__DOT__envelope))
                              ? vlSelfRef.__PVT__v2__DOT__envelope_dac__DOT__bitval[6U]
                              : 0U)));
    vlSelfRef.__PVT__v2__DOT__envelope_dac__DOT__bitsum[7U] 
        = (0x00000fffU & (vlSelfRef.__PVT__v2__DOT__envelope_dac__DOT__bitsum[6U] 
                          + ((0x00000080U & (IData)(vlSelfRef.__PVT__v2__DOT__envelope))
                              ? vlSelfRef.__PVT__v2__DOT__envelope_dac__DOT__bitval[7U]
                              : 0U)));
}

void Vtop_sid_chip___act_sequent__TOP__top__DOT__sid1_inst__2(Vtop_sid_chip* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+        Vtop_sid_chip___act_sequent__TOP__top__DOT__sid1_inst__2\n"); );
    Vtop__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
    // Body
    vlSelfRef.__PVT__v3__DOT__envelope_dac__DOT__bitsum[1U] 
        = (0x00000fffU & (vlSelfRef.__PVT__v3__DOT__envelope_dac__DOT__bitsum[0U] 
                          + ((2U & (IData)(vlSelfRef.__PVT__v3__DOT__envelope))
                              ? vlSelfRef.__PVT__v3__DOT__envelope_dac__DOT__bitval[1U]
                              : 0U)));
    vlSelfRef.__PVT__v3__DOT__envelope_dac__DOT__bitsum[2U] 
        = (0x00000fffU & (vlSelfRef.__PVT__v3__DOT__envelope_dac__DOT__bitsum[1U] 
                          + ((4U & (IData)(vlSelfRef.__PVT__v3__DOT__envelope))
                              ? vlSelfRef.__PVT__v3__DOT__envelope_dac__DOT__bitval[2U]
                              : 0U)));
    vlSelfRef.__PVT__v3__DOT__envelope_dac__DOT__bitsum[3U] 
        = (0x00000fffU & (vlSelfRef.__PVT__v3__DOT__envelope_dac__DOT__bitsum[2U] 
                          + ((8U & (IData)(vlSelfRef.__PVT__v3__DOT__envelope))
                              ? vlSelfRef.__PVT__v3__DOT__envelope_dac__DOT__bitval[3U]
                              : 0U)));
    vlSelfRef.__PVT__v3__DOT__envelope_dac__DOT__bitsum[4U] 
        = (0x00000fffU & (vlSelfRef.__PVT__v3__DOT__envelope_dac__DOT__bitsum[3U] 
                          + ((0x00000010U & (IData)(vlSelfRef.__PVT__v3__DOT__envelope))
                              ? vlSelfRef.__PVT__v3__DOT__envelope_dac__DOT__bitval[4U]
                              : 0U)));
    vlSelfRef.__PVT__v3__DOT__envelope_dac__DOT__bitsum[5U] 
        = (0x00000fffU & (vlSelfRef.__PVT__v3__DOT__envelope_dac__DOT__bitsum[4U] 
                          + ((0x00000020U & (IData)(vlSelfRef.__PVT__v3__DOT__envelope))
                              ? vlSelfRef.__PVT__v3__DOT__envelope_dac__DOT__bitval[5U]
                              : 0U)));
    vlSelfRef.__PVT__v3__DOT__envelope_dac__DOT__bitsum[6U] 
        = (0x00000fffU & (vlSelfRef.__PVT__v3__DOT__envelope_dac__DOT__bitsum[5U] 
                          + ((0x00000040U & (IData)(vlSelfRef.__PVT__v3__DOT__envelope))
                              ? vlSelfRef.__PVT__v3__DOT__envelope_dac__DOT__bitval[6U]
                              : 0U)));
    vlSelfRef.__PVT__v3__DOT__envelope_dac__DOT__bitsum[7U] 
        = (0x00000fffU & (vlSelfRef.__PVT__v3__DOT__envelope_dac__DOT__bitsum[6U] 
                          + ((0x00000080U & (IData)(vlSelfRef.__PVT__v3__DOT__envelope))
                              ? vlSelfRef.__PVT__v3__DOT__envelope_dac__DOT__bitval[7U]
                              : 0U)));
}

void Vtop_sid_chip___act_sequent__TOP__top__DOT__sid1_inst__3(Vtop_sid_chip* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+        Vtop_sid_chip___act_sequent__TOP__top__DOT__sid1_inst__3\n"); );
    Vtop__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
    // Body
    vlSelfRef.__PVT__v1__DOT__waveform_dac__DOT__bitsum[1U] 
        = (0x0000ffffU & (vlSelfRef.__PVT__v1__DOT__waveform_dac__DOT__bitsum[0U] 
                          + ((2U & (IData)(vlSelfRef.__PVT__v1__DOT__norm))
                              ? vlSelfRef.__PVT__v1__DOT__waveform_dac__DOT__bitval[1U]
                              : 0U)));
    vlSelfRef.__PVT__v1__DOT__waveform_dac__DOT__bitsum[2U] 
        = (0x0000ffffU & (vlSelfRef.__PVT__v1__DOT__waveform_dac__DOT__bitsum[1U] 
                          + ((4U & (IData)(vlSelfRef.__PVT__v1__DOT__norm))
                              ? vlSelfRef.__PVT__v1__DOT__waveform_dac__DOT__bitval[2U]
                              : 0U)));
    vlSelfRef.__PVT__v1__DOT__waveform_dac__DOT__bitsum[3U] 
        = (0x0000ffffU & (vlSelfRef.__PVT__v1__DOT__waveform_dac__DOT__bitsum[2U] 
                          + ((8U & (IData)(vlSelfRef.__PVT__v1__DOT__norm))
                              ? vlSelfRef.__PVT__v1__DOT__waveform_dac__DOT__bitval[3U]
                              : 0U)));
    vlSelfRef.__PVT__v1__DOT__waveform_dac__DOT__bitsum[4U] 
        = (0x0000ffffU & (vlSelfRef.__PVT__v1__DOT__waveform_dac__DOT__bitsum[3U] 
                          + ((0x00000010U & (IData)(vlSelfRef.__PVT__v1__DOT__norm))
                              ? vlSelfRef.__PVT__v1__DOT__waveform_dac__DOT__bitval[4U]
                              : 0U)));
    vlSelfRef.__PVT__v1__DOT__waveform_dac__DOT__bitsum[5U] 
        = (0x0000ffffU & (vlSelfRef.__PVT__v1__DOT__waveform_dac__DOT__bitsum[4U] 
                          + ((0x00000020U & (IData)(vlSelfRef.__PVT__v1__DOT__norm))
                              ? vlSelfRef.__PVT__v1__DOT__waveform_dac__DOT__bitval[5U]
                              : 0U)));
    vlSelfRef.__PVT__v1__DOT__waveform_dac__DOT__bitsum[6U] 
        = (0x0000ffffU & (vlSelfRef.__PVT__v1__DOT__waveform_dac__DOT__bitsum[5U] 
                          + ((0x00000040U & (IData)(vlSelfRef.__PVT__v1__DOT__norm))
                              ? vlSelfRef.__PVT__v1__DOT__waveform_dac__DOT__bitval[6U]
                              : 0U)));
    vlSelfRef.__PVT__v1__DOT__waveform_dac__DOT__bitsum[7U] 
        = (0x0000ffffU & (vlSelfRef.__PVT__v1__DOT__waveform_dac__DOT__bitsum[6U] 
                          + ((0x00000080U & (IData)(vlSelfRef.__PVT__v1__DOT__norm))
                              ? vlSelfRef.__PVT__v1__DOT__waveform_dac__DOT__bitval[7U]
                              : 0U)));
    vlSelfRef.__PVT__v1__DOT__waveform_dac__DOT__bitsum[8U] 
        = (0x0000ffffU & (vlSelfRef.__PVT__v1__DOT__waveform_dac__DOT__bitsum[7U] 
                          + ((0x00000100U & (IData)(vlSelfRef.__PVT__v1__DOT__norm))
                              ? vlSelfRef.__PVT__v1__DOT__waveform_dac__DOT__bitval[8U]
                              : 0U)));
    vlSelfRef.__PVT__v1__DOT__waveform_dac__DOT__bitsum[9U] 
        = (0x0000ffffU & (vlSelfRef.__PVT__v1__DOT__waveform_dac__DOT__bitsum[8U] 
                          + ((0x00000200U & (IData)(vlSelfRef.__PVT__v1__DOT__norm))
                              ? vlSelfRef.__PVT__v1__DOT__waveform_dac__DOT__bitval[9U]
                              : 0U)));
    vlSelfRef.__PVT__v1__DOT__waveform_dac__DOT__bitsum[10U] 
        = (0x0000ffffU & (vlSelfRef.__PVT__v1__DOT__waveform_dac__DOT__bitsum[9U] 
                          + ((0x00000400U & (IData)(vlSelfRef.__PVT__v1__DOT__norm))
                              ? vlSelfRef.__PVT__v1__DOT__waveform_dac__DOT__bitval[10U]
                              : 0U)));
    vlSelfRef.__PVT__v1__DOT__waveform_dac__DOT__bitsum[11U] 
        = (0x0000ffffU & (vlSelfRef.__PVT__v1__DOT__waveform_dac__DOT__bitsum[10U] 
                          + ((0x00000800U & (IData)(vlSelfRef.__PVT__v1__DOT__norm))
                              ? vlSelfRef.__PVT__v1__DOT__waveform_dac__DOT__bitval[11U]
                              : 0U)));
}

void Vtop_sid_chip___act_sequent__TOP__top__DOT__sid1_inst__4(Vtop_sid_chip* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+        Vtop_sid_chip___act_sequent__TOP__top__DOT__sid1_inst__4\n"); );
    Vtop__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
    // Body
    vlSelfRef.__PVT__v2__DOT__waveform_dac__DOT__bitsum[1U] 
        = (0x0000ffffU & (vlSelfRef.__PVT__v2__DOT__waveform_dac__DOT__bitsum[0U] 
                          + ((2U & (IData)(vlSelfRef.__PVT__v2__DOT__norm))
                              ? vlSelfRef.__PVT__v2__DOT__waveform_dac__DOT__bitval[1U]
                              : 0U)));
    vlSelfRef.__PVT__v2__DOT__waveform_dac__DOT__bitsum[2U] 
        = (0x0000ffffU & (vlSelfRef.__PVT__v2__DOT__waveform_dac__DOT__bitsum[1U] 
                          + ((4U & (IData)(vlSelfRef.__PVT__v2__DOT__norm))
                              ? vlSelfRef.__PVT__v2__DOT__waveform_dac__DOT__bitval[2U]
                              : 0U)));
    vlSelfRef.__PVT__v2__DOT__waveform_dac__DOT__bitsum[3U] 
        = (0x0000ffffU & (vlSelfRef.__PVT__v2__DOT__waveform_dac__DOT__bitsum[2U] 
                          + ((8U & (IData)(vlSelfRef.__PVT__v2__DOT__norm))
                              ? vlSelfRef.__PVT__v2__DOT__waveform_dac__DOT__bitval[3U]
                              : 0U)));
    vlSelfRef.__PVT__v2__DOT__waveform_dac__DOT__bitsum[4U] 
        = (0x0000ffffU & (vlSelfRef.__PVT__v2__DOT__waveform_dac__DOT__bitsum[3U] 
                          + ((0x00000010U & (IData)(vlSelfRef.__PVT__v2__DOT__norm))
                              ? vlSelfRef.__PVT__v2__DOT__waveform_dac__DOT__bitval[4U]
                              : 0U)));
    vlSelfRef.__PVT__v2__DOT__waveform_dac__DOT__bitsum[5U] 
        = (0x0000ffffU & (vlSelfRef.__PVT__v2__DOT__waveform_dac__DOT__bitsum[4U] 
                          + ((0x00000020U & (IData)(vlSelfRef.__PVT__v2__DOT__norm))
                              ? vlSelfRef.__PVT__v2__DOT__waveform_dac__DOT__bitval[5U]
                              : 0U)));
    vlSelfRef.__PVT__v2__DOT__waveform_dac__DOT__bitsum[6U] 
        = (0x0000ffffU & (vlSelfRef.__PVT__v2__DOT__waveform_dac__DOT__bitsum[5U] 
                          + ((0x00000040U & (IData)(vlSelfRef.__PVT__v2__DOT__norm))
                              ? vlSelfRef.__PVT__v2__DOT__waveform_dac__DOT__bitval[6U]
                              : 0U)));
    vlSelfRef.__PVT__v2__DOT__waveform_dac__DOT__bitsum[7U] 
        = (0x0000ffffU & (vlSelfRef.__PVT__v2__DOT__waveform_dac__DOT__bitsum[6U] 
                          + ((0x00000080U & (IData)(vlSelfRef.__PVT__v2__DOT__norm))
                              ? vlSelfRef.__PVT__v2__DOT__waveform_dac__DOT__bitval[7U]
                              : 0U)));
    vlSelfRef.__PVT__v2__DOT__waveform_dac__DOT__bitsum[8U] 
        = (0x0000ffffU & (vlSelfRef.__PVT__v2__DOT__waveform_dac__DOT__bitsum[7U] 
                          + ((0x00000100U & (IData)(vlSelfRef.__PVT__v2__DOT__norm))
                              ? vlSelfRef.__PVT__v2__DOT__waveform_dac__DOT__bitval[8U]
                              : 0U)));
    vlSelfRef.__PVT__v2__DOT__waveform_dac__DOT__bitsum[9U] 
        = (0x0000ffffU & (vlSelfRef.__PVT__v2__DOT__waveform_dac__DOT__bitsum[8U] 
                          + ((0x00000200U & (IData)(vlSelfRef.__PVT__v2__DOT__norm))
                              ? vlSelfRef.__PVT__v2__DOT__waveform_dac__DOT__bitval[9U]
                              : 0U)));
    vlSelfRef.__PVT__v2__DOT__waveform_dac__DOT__bitsum[10U] 
        = (0x0000ffffU & (vlSelfRef.__PVT__v2__DOT__waveform_dac__DOT__bitsum[9U] 
                          + ((0x00000400U & (IData)(vlSelfRef.__PVT__v2__DOT__norm))
                              ? vlSelfRef.__PVT__v2__DOT__waveform_dac__DOT__bitval[10U]
                              : 0U)));
    vlSelfRef.__PVT__v2__DOT__waveform_dac__DOT__bitsum[11U] 
        = (0x0000ffffU & (vlSelfRef.__PVT__v2__DOT__waveform_dac__DOT__bitsum[10U] 
                          + ((0x00000800U & (IData)(vlSelfRef.__PVT__v2__DOT__norm))
                              ? vlSelfRef.__PVT__v2__DOT__waveform_dac__DOT__bitval[11U]
                              : 0U)));
}

void Vtop_sid_chip___act_sequent__TOP__top__DOT__sid1_inst__5(Vtop_sid_chip* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+        Vtop_sid_chip___act_sequent__TOP__top__DOT__sid1_inst__5\n"); );
    Vtop__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
    // Body
    vlSelfRef.__PVT__v3__DOT__waveform_dac__DOT__bitsum[1U] 
        = (0x0000ffffU & (vlSelfRef.__PVT__v3__DOT__waveform_dac__DOT__bitsum[0U] 
                          + ((2U & (IData)(vlSelfRef.__PVT__v3__DOT__norm))
                              ? vlSelfRef.__PVT__v3__DOT__waveform_dac__DOT__bitval[1U]
                              : 0U)));
    vlSelfRef.__PVT__v3__DOT__waveform_dac__DOT__bitsum[2U] 
        = (0x0000ffffU & (vlSelfRef.__PVT__v3__DOT__waveform_dac__DOT__bitsum[1U] 
                          + ((4U & (IData)(vlSelfRef.__PVT__v3__DOT__norm))
                              ? vlSelfRef.__PVT__v3__DOT__waveform_dac__DOT__bitval[2U]
                              : 0U)));
    vlSelfRef.__PVT__v3__DOT__waveform_dac__DOT__bitsum[3U] 
        = (0x0000ffffU & (vlSelfRef.__PVT__v3__DOT__waveform_dac__DOT__bitsum[2U] 
                          + ((8U & (IData)(vlSelfRef.__PVT__v3__DOT__norm))
                              ? vlSelfRef.__PVT__v3__DOT__waveform_dac__DOT__bitval[3U]
                              : 0U)));
    vlSelfRef.__PVT__v3__DOT__waveform_dac__DOT__bitsum[4U] 
        = (0x0000ffffU & (vlSelfRef.__PVT__v3__DOT__waveform_dac__DOT__bitsum[3U] 
                          + ((0x00000010U & (IData)(vlSelfRef.__PVT__v3__DOT__norm))
                              ? vlSelfRef.__PVT__v3__DOT__waveform_dac__DOT__bitval[4U]
                              : 0U)));
    vlSelfRef.__PVT__v3__DOT__waveform_dac__DOT__bitsum[5U] 
        = (0x0000ffffU & (vlSelfRef.__PVT__v3__DOT__waveform_dac__DOT__bitsum[4U] 
                          + ((0x00000020U & (IData)(vlSelfRef.__PVT__v3__DOT__norm))
                              ? vlSelfRef.__PVT__v3__DOT__waveform_dac__DOT__bitval[5U]
                              : 0U)));
    vlSelfRef.__PVT__v3__DOT__waveform_dac__DOT__bitsum[6U] 
        = (0x0000ffffU & (vlSelfRef.__PVT__v3__DOT__waveform_dac__DOT__bitsum[5U] 
                          + ((0x00000040U & (IData)(vlSelfRef.__PVT__v3__DOT__norm))
                              ? vlSelfRef.__PVT__v3__DOT__waveform_dac__DOT__bitval[6U]
                              : 0U)));
    vlSelfRef.__PVT__v3__DOT__waveform_dac__DOT__bitsum[7U] 
        = (0x0000ffffU & (vlSelfRef.__PVT__v3__DOT__waveform_dac__DOT__bitsum[6U] 
                          + ((0x00000080U & (IData)(vlSelfRef.__PVT__v3__DOT__norm))
                              ? vlSelfRef.__PVT__v3__DOT__waveform_dac__DOT__bitval[7U]
                              : 0U)));
    vlSelfRef.__PVT__v3__DOT__waveform_dac__DOT__bitsum[8U] 
        = (0x0000ffffU & (vlSelfRef.__PVT__v3__DOT__waveform_dac__DOT__bitsum[7U] 
                          + ((0x00000100U & (IData)(vlSelfRef.__PVT__v3__DOT__norm))
                              ? vlSelfRef.__PVT__v3__DOT__waveform_dac__DOT__bitval[8U]
                              : 0U)));
    vlSelfRef.__PVT__v3__DOT__waveform_dac__DOT__bitsum[9U] 
        = (0x0000ffffU & (vlSelfRef.__PVT__v3__DOT__waveform_dac__DOT__bitsum[8U] 
                          + ((0x00000200U & (IData)(vlSelfRef.__PVT__v3__DOT__norm))
                              ? vlSelfRef.__PVT__v3__DOT__waveform_dac__DOT__bitval[9U]
                              : 0U)));
    vlSelfRef.__PVT__v3__DOT__waveform_dac__DOT__bitsum[10U] 
        = (0x0000ffffU & (vlSelfRef.__PVT__v3__DOT__waveform_dac__DOT__bitsum[9U] 
                          + ((0x00000400U & (IData)(vlSelfRef.__PVT__v3__DOT__norm))
                              ? vlSelfRef.__PVT__v3__DOT__waveform_dac__DOT__bitval[10U]
                              : 0U)));
    vlSelfRef.__PVT__v3__DOT__waveform_dac__DOT__bitsum[11U] 
        = (0x0000ffffU & (vlSelfRef.__PVT__v3__DOT__waveform_dac__DOT__bitsum[10U] 
                          + ((0x00000800U & (IData)(vlSelfRef.__PVT__v3__DOT__norm))
                              ? vlSelfRef.__PVT__v3__DOT__waveform_dac__DOT__bitval[11U]
                              : 0U)));
}

extern const VlUnpacked<CData/*7:0*/, 8192> Vtop__ConstPool__TABLE_h5c0f3167_0;
extern const VlUnpacked<CData/*7:0*/, 8192> Vtop__ConstPool__TABLE_h8a1656db_0;

void Vtop_sid_chip___nba_sequent__TOP__top__DOT__sid1_inst__0(Vtop_sid_chip* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+        Vtop_sid_chip___nba_sequent__TOP__top__DOT__sid1_inst__0\n"); );
    Vtop__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
    // Locals
    IData/*31:0*/ __PVT__flt__DOT__m;
    __PVT__flt__DOT__m = 0;
    SData/*15:0*/ __Vfunc_flt__DOT__clamp__2__Vfuncout;
    __Vfunc_flt__DOT__clamp__2__Vfuncout = 0;
    IData/*16:0*/ __Vfunc_flt__DOT__clamp__2__x;
    __Vfunc_flt__DOT__clamp__2__x = 0;
    SData/*12:0*/ __Vtableidx1;
    __Vtableidx1 = 0;
    SData/*12:0*/ __Vtableidx2;
    __Vtableidx2 = 0;
    IData/*16:0*/ __VdfgRegularize_hebeb780c_0_30;
    __VdfgRegularize_hebeb780c_0_30 = 0;
    CData/*3:0*/ __Vdly__tbl_state;
    __Vdly__tbl_state = 0;
    SData/*11:0*/ __Vdly__f_acc_t;
    __Vdly__f_acc_t = 0;
    IData/*23:0*/ __Vdly__v1__DOT__oscillator;
    __Vdly__v1__DOT__oscillator = 0;
    IData/*22:0*/ __Vdly__v1__DOT__unnamedblk2__DOT__lfsr_noise;
    __Vdly__v1__DOT__unnamedblk2__DOT__lfsr_noise = 0;
    IData/*23:0*/ __Vdly__v1__DOT__unnamedblk2__DOT__noise_age;
    __Vdly__v1__DOT__unnamedblk2__DOT__noise_age = 0;
    IData/*23:0*/ __Vdly__v1__DOT__unnamedblk3__DOT__keep_cnt;
    __Vdly__v1__DOT__unnamedblk3__DOT__keep_cnt = 0;
    CData/*4:0*/ __Vdly__v1__DOT__adsr__DOT__unnamedblk1__DOT__exponential_counter_period;
    __Vdly__v1__DOT__adsr__DOT__unnamedblk1__DOT__exponential_counter_period = 0;
    CData/*1:0*/ __Vdly__v1__DOT__adsr__DOT__state;
    __Vdly__v1__DOT__adsr__DOT__state = 0;
    CData/*7:0*/ __Vdly__v1__DOT__envelope;
    __Vdly__v1__DOT__envelope = 0;
    CData/*0:0*/ __Vdly__v1__DOT__adsr__DOT__unnamedblk1__DOT__hold_zero;
    __Vdly__v1__DOT__adsr__DOT__unnamedblk1__DOT__hold_zero = 0;
    CData/*4:0*/ __Vdly__v1__DOT__adsr__DOT__unnamedblk1__DOT__exponential_counter;
    __Vdly__v1__DOT__adsr__DOT__unnamedblk1__DOT__exponential_counter = 0;
    SData/*14:0*/ __Vdly__v1__DOT__adsr__DOT__unnamedblk1__DOT__rate_counter;
    __Vdly__v1__DOT__adsr__DOT__unnamedblk1__DOT__rate_counter = 0;
    IData/*23:0*/ __Vdly__v2__DOT__oscillator;
    __Vdly__v2__DOT__oscillator = 0;
    IData/*22:0*/ __Vdly__v2__DOT__unnamedblk2__DOT__lfsr_noise;
    __Vdly__v2__DOT__unnamedblk2__DOT__lfsr_noise = 0;
    IData/*23:0*/ __Vdly__v2__DOT__unnamedblk2__DOT__noise_age;
    __Vdly__v2__DOT__unnamedblk2__DOT__noise_age = 0;
    IData/*23:0*/ __Vdly__v2__DOT__unnamedblk3__DOT__keep_cnt;
    __Vdly__v2__DOT__unnamedblk3__DOT__keep_cnt = 0;
    CData/*4:0*/ __Vdly__v2__DOT__adsr__DOT__unnamedblk1__DOT__exponential_counter_period;
    __Vdly__v2__DOT__adsr__DOT__unnamedblk1__DOT__exponential_counter_period = 0;
    CData/*1:0*/ __Vdly__v2__DOT__adsr__DOT__state;
    __Vdly__v2__DOT__adsr__DOT__state = 0;
    CData/*7:0*/ __Vdly__v2__DOT__envelope;
    __Vdly__v2__DOT__envelope = 0;
    CData/*0:0*/ __Vdly__v2__DOT__adsr__DOT__unnamedblk1__DOT__hold_zero;
    __Vdly__v2__DOT__adsr__DOT__unnamedblk1__DOT__hold_zero = 0;
    CData/*4:0*/ __Vdly__v2__DOT__adsr__DOT__unnamedblk1__DOT__exponential_counter;
    __Vdly__v2__DOT__adsr__DOT__unnamedblk1__DOT__exponential_counter = 0;
    SData/*14:0*/ __Vdly__v2__DOT__adsr__DOT__unnamedblk1__DOT__rate_counter;
    __Vdly__v2__DOT__adsr__DOT__unnamedblk1__DOT__rate_counter = 0;
    IData/*23:0*/ __Vdly__v3__DOT__oscillator;
    __Vdly__v3__DOT__oscillator = 0;
    IData/*22:0*/ __Vdly__v3__DOT__unnamedblk2__DOT__lfsr_noise;
    __Vdly__v3__DOT__unnamedblk2__DOT__lfsr_noise = 0;
    IData/*23:0*/ __Vdly__v3__DOT__unnamedblk2__DOT__noise_age;
    __Vdly__v3__DOT__unnamedblk2__DOT__noise_age = 0;
    IData/*23:0*/ __Vdly__v3__DOT__unnamedblk3__DOT__keep_cnt;
    __Vdly__v3__DOT__unnamedblk3__DOT__keep_cnt = 0;
    CData/*4:0*/ __Vdly__v3__DOT__adsr__DOT__unnamedblk1__DOT__exponential_counter_period;
    __Vdly__v3__DOT__adsr__DOT__unnamedblk1__DOT__exponential_counter_period = 0;
    CData/*1:0*/ __Vdly__v3__DOT__adsr__DOT__state;
    __Vdly__v3__DOT__adsr__DOT__state = 0;
    CData/*7:0*/ __Vdly__v3__DOT__envelope;
    __Vdly__v3__DOT__envelope = 0;
    CData/*0:0*/ __Vdly__v3__DOT__adsr__DOT__unnamedblk1__DOT__hold_zero;
    __Vdly__v3__DOT__adsr__DOT__unnamedblk1__DOT__hold_zero = 0;
    CData/*4:0*/ __Vdly__v3__DOT__adsr__DOT__unnamedblk1__DOT__exponential_counter;
    __Vdly__v3__DOT__adsr__DOT__unnamedblk1__DOT__exponential_counter = 0;
    SData/*14:0*/ __Vdly__v3__DOT__adsr__DOT__unnamedblk1__DOT__rate_counter;
    __Vdly__v3__DOT__adsr__DOT__unnamedblk1__DOT__rate_counter = 0;
    CData/*0:0*/ __VdlySet__voice_pw__v0;
    __VdlySet__voice_pw__v0 = 0;
    CData/*7:0*/ __VdlyVal__voice_pw__v3;
    __VdlyVal__voice_pw__v3 = 0;
    CData/*0:0*/ __VdlySet__voice_pw__v3;
    __VdlySet__voice_pw__v3 = 0;
    CData/*3:0*/ __VdlyVal__voice_pw__v4;
    __VdlyVal__voice_pw__v4 = 0;
    CData/*0:0*/ __VdlySet__voice_pw__v4;
    __VdlySet__voice_pw__v4 = 0;
    CData/*7:0*/ __VdlyVal__voice_pw__v5;
    __VdlyVal__voice_pw__v5 = 0;
    CData/*0:0*/ __VdlySet__voice_pw__v5;
    __VdlySet__voice_pw__v5 = 0;
    CData/*3:0*/ __VdlyVal__voice_pw__v6;
    __VdlyVal__voice_pw__v6 = 0;
    CData/*0:0*/ __VdlySet__voice_pw__v6;
    __VdlySet__voice_pw__v6 = 0;
    CData/*7:0*/ __VdlyVal__voice_pw__v7;
    __VdlyVal__voice_pw__v7 = 0;
    CData/*0:0*/ __VdlySet__voice_pw__v7;
    __VdlySet__voice_pw__v7 = 0;
    CData/*3:0*/ __VdlyVal__voice_pw__v8;
    __VdlyVal__voice_pw__v8 = 0;
    CData/*0:0*/ __VdlySet__voice_pw__v8;
    __VdlySet__voice_pw__v8 = 0;
    CData/*0:0*/ __VdlySet__voice_freq__v0;
    __VdlySet__voice_freq__v0 = 0;
    CData/*7:0*/ __VdlyVal__voice_freq__v3;
    __VdlyVal__voice_freq__v3 = 0;
    CData/*0:0*/ __VdlySet__voice_freq__v3;
    __VdlySet__voice_freq__v3 = 0;
    CData/*7:0*/ __VdlyVal__voice_freq__v4;
    __VdlyVal__voice_freq__v4 = 0;
    CData/*0:0*/ __VdlySet__voice_freq__v4;
    __VdlySet__voice_freq__v4 = 0;
    CData/*7:0*/ __VdlyVal__voice_freq__v5;
    __VdlyVal__voice_freq__v5 = 0;
    CData/*0:0*/ __VdlySet__voice_freq__v5;
    __VdlySet__voice_freq__v5 = 0;
    CData/*7:0*/ __VdlyVal__voice_freq__v6;
    __VdlyVal__voice_freq__v6 = 0;
    CData/*0:0*/ __VdlySet__voice_freq__v6;
    __VdlySet__voice_freq__v6 = 0;
    CData/*7:0*/ __VdlyVal__voice_freq__v7;
    __VdlyVal__voice_freq__v7 = 0;
    CData/*0:0*/ __VdlySet__voice_freq__v7;
    __VdlySet__voice_freq__v7 = 0;
    CData/*7:0*/ __VdlyVal__voice_freq__v8;
    __VdlyVal__voice_freq__v8 = 0;
    CData/*0:0*/ __VdlySet__voice_freq__v8;
    __VdlySet__voice_freq__v8 = 0;
    CData/*0:0*/ __VdlySet__voice_ctrl__v0;
    __VdlySet__voice_ctrl__v0 = 0;
    CData/*7:0*/ __VdlyVal__voice_ctrl__v3;
    __VdlyVal__voice_ctrl__v3 = 0;
    CData/*0:0*/ __VdlySet__voice_ctrl__v3;
    __VdlySet__voice_ctrl__v3 = 0;
    CData/*7:0*/ __VdlyVal__voice_ctrl__v4;
    __VdlyVal__voice_ctrl__v4 = 0;
    CData/*0:0*/ __VdlySet__voice_ctrl__v4;
    __VdlySet__voice_ctrl__v4 = 0;
    CData/*7:0*/ __VdlyVal__voice_ctrl__v5;
    __VdlyVal__voice_ctrl__v5 = 0;
    CData/*0:0*/ __VdlySet__voice_ctrl__v5;
    __VdlySet__voice_ctrl__v5 = 0;
    CData/*0:0*/ __VdlySet__voice_ad__v0;
    __VdlySet__voice_ad__v0 = 0;
    CData/*7:0*/ __VdlyVal__voice_ad__v3;
    __VdlyVal__voice_ad__v3 = 0;
    CData/*0:0*/ __VdlySet__voice_ad__v3;
    __VdlySet__voice_ad__v3 = 0;
    CData/*7:0*/ __VdlyVal__voice_ad__v4;
    __VdlyVal__voice_ad__v4 = 0;
    CData/*0:0*/ __VdlySet__voice_ad__v4;
    __VdlySet__voice_ad__v4 = 0;
    CData/*7:0*/ __VdlyVal__voice_ad__v5;
    __VdlyVal__voice_ad__v5 = 0;
    CData/*0:0*/ __VdlySet__voice_ad__v5;
    __VdlySet__voice_ad__v5 = 0;
    CData/*0:0*/ __VdlySet__voice_vol__v0;
    __VdlySet__voice_vol__v0 = 0;
    CData/*3:0*/ __VdlyVal__voice_vol__v3;
    __VdlyVal__voice_vol__v3 = 0;
    CData/*0:0*/ __VdlySet__voice_vol__v3;
    __VdlySet__voice_vol__v3 = 0;
    CData/*3:0*/ __VdlyVal__voice_vol__v4;
    __VdlyVal__voice_vol__v4 = 0;
    CData/*0:0*/ __VdlySet__voice_vol__v4;
    __VdlySet__voice_vol__v4 = 0;
    CData/*3:0*/ __VdlyVal__voice_vol__v5;
    __VdlyVal__voice_vol__v5 = 0;
    CData/*0:0*/ __VdlySet__voice_vol__v5;
    __VdlySet__voice_vol__v5 = 0;
    CData/*0:0*/ __VdlySet__voice_sr__v0;
    __VdlySet__voice_sr__v0 = 0;
    CData/*7:0*/ __VdlyVal__voice_sr__v3;
    __VdlyVal__voice_sr__v3 = 0;
    CData/*0:0*/ __VdlySet__voice_sr__v3;
    __VdlySet__voice_sr__v3 = 0;
    CData/*7:0*/ __VdlyVal__voice_sr__v4;
    __VdlyVal__voice_sr__v4 = 0;
    CData/*0:0*/ __VdlySet__voice_sr__v4;
    __VdlySet__voice_sr__v4 = 0;
    CData/*7:0*/ __VdlyVal__voice_sr__v5;
    __VdlyVal__voice_sr__v5 = 0;
    CData/*0:0*/ __VdlySet__voice_sr__v5;
    __VdlySet__voice_sr__v5 = 0;
    CData/*7:0*/ __VdlyVal___st_out__v0;
    __VdlyVal___st_out__v0 = 0;
    CData/*2:0*/ __VdlyDim0___st_out__v0;
    __VdlyDim0___st_out__v0 = 0;
    CData/*0:0*/ __VdlySet___st_out__v0;
    __VdlySet___st_out__v0 = 0;
    CData/*7:0*/ __VdlyVal__p_t_out__v0;
    __VdlyVal__p_t_out__v0 = 0;
    CData/*2:0*/ __VdlyDim0__p_t_out__v0;
    __VdlyDim0__p_t_out__v0 = 0;
    CData/*0:0*/ __VdlySet__p_t_out__v0;
    __VdlySet__p_t_out__v0 = 0;
    CData/*7:0*/ __VdlyVal__ps___05Fout__v0;
    __VdlyVal__ps___05Fout__v0 = 0;
    CData/*2:0*/ __VdlyDim0__ps___05Fout__v0;
    __VdlyDim0__ps___05Fout__v0 = 0;
    CData/*0:0*/ __VdlySet__ps___05Fout__v0;
    __VdlySet__ps___05Fout__v0 = 0;
    CData/*7:0*/ __VdlyVal__pst_out__v0;
    __VdlyVal__pst_out__v0 = 0;
    CData/*2:0*/ __VdlyDim0__pst_out__v0;
    __VdlyDim0__pst_out__v0 = 0;
    CData/*0:0*/ __VdlySet__pst_out__v0;
    __VdlySet__pst_out__v0 = 0;
    // Body
    __VdlySet__voice_ad__v0 = 0U;
    __VdlySet__voice_ad__v3 = 0U;
    __VdlySet__voice_ad__v4 = 0U;
    __VdlySet__voice_ad__v5 = 0U;
    __VdlySet__voice_sr__v0 = 0U;
    __VdlySet__voice_sr__v3 = 0U;
    __VdlySet__voice_sr__v4 = 0U;
    __VdlySet__voice_sr__v5 = 0U;
    __VdlySet__voice_pw__v0 = 0U;
    __VdlySet__voice_pw__v3 = 0U;
    __VdlySet__voice_pw__v4 = 0U;
    __VdlySet__voice_pw__v5 = 0U;
    __VdlySet__voice_pw__v6 = 0U;
    __VdlySet__voice_pw__v7 = 0U;
    __VdlySet__voice_pw__v8 = 0U;
    __VdlySet__voice_freq__v0 = 0U;
    __VdlySet__voice_freq__v3 = 0U;
    __VdlySet__voice_freq__v4 = 0U;
    __VdlySet__voice_freq__v5 = 0U;
    __VdlySet__voice_freq__v6 = 0U;
    __VdlySet__voice_freq__v7 = 0U;
    __VdlySet__voice_freq__v8 = 0U;
    __VdlySet__voice_vol__v0 = 0U;
    __VdlySet__voice_vol__v3 = 0U;
    __VdlySet__voice_vol__v4 = 0U;
    __VdlySet__voice_vol__v5 = 0U;
    __Vdly__v1__DOT__adsr__DOT__unnamedblk1__DOT__exponential_counter_period 
        = vlSelfRef.__PVT__v1__DOT__adsr__DOT__unnamedblk1__DOT__exponential_counter_period;
    __Vdly__v1__DOT__adsr__DOT__unnamedblk1__DOT__hold_zero 
        = vlSelfRef.__PVT__v1__DOT__adsr__DOT__unnamedblk1__DOT__hold_zero;
    __Vdly__v1__DOT__adsr__DOT__unnamedblk1__DOT__exponential_counter 
        = vlSelfRef.__PVT__v1__DOT__adsr__DOT__unnamedblk1__DOT__exponential_counter;
    __Vdly__v1__DOT__adsr__DOT__unnamedblk1__DOT__rate_counter 
        = vlSelfRef.__PVT__v1__DOT__adsr__DOT__unnamedblk1__DOT__rate_counter;
    __Vdly__v2__DOT__adsr__DOT__unnamedblk1__DOT__exponential_counter_period 
        = vlSelfRef.__PVT__v2__DOT__adsr__DOT__unnamedblk1__DOT__exponential_counter_period;
    __Vdly__v2__DOT__adsr__DOT__unnamedblk1__DOT__hold_zero 
        = vlSelfRef.__PVT__v2__DOT__adsr__DOT__unnamedblk1__DOT__hold_zero;
    __Vdly__v2__DOT__adsr__DOT__unnamedblk1__DOT__exponential_counter 
        = vlSelfRef.__PVT__v2__DOT__adsr__DOT__unnamedblk1__DOT__exponential_counter;
    __Vdly__v2__DOT__adsr__DOT__unnamedblk1__DOT__rate_counter 
        = vlSelfRef.__PVT__v2__DOT__adsr__DOT__unnamedblk1__DOT__rate_counter;
    __Vdly__v1__DOT__adsr__DOT__state = vlSelfRef.__PVT__v1__DOT__adsr__DOT__state;
    __Vdly__v2__DOT__adsr__DOT__state = vlSelfRef.__PVT__v2__DOT__adsr__DOT__state;
    __Vdly__v1__DOT__unnamedblk3__DOT__keep_cnt = vlSelfRef.__PVT__v1__DOT__unnamedblk3__DOT__keep_cnt;
    __Vdly__v2__DOT__unnamedblk3__DOT__keep_cnt = vlSelfRef.__PVT__v2__DOT__unnamedblk3__DOT__keep_cnt;
    __Vdly__v3__DOT__unnamedblk3__DOT__keep_cnt = vlSelfRef.__PVT__v3__DOT__unnamedblk3__DOT__keep_cnt;
    __Vdly__v3__DOT__adsr__DOT__unnamedblk1__DOT__exponential_counter_period 
        = vlSelfRef.__PVT__v3__DOT__adsr__DOT__unnamedblk1__DOT__exponential_counter_period;
    __Vdly__v3__DOT__adsr__DOT__unnamedblk1__DOT__hold_zero 
        = vlSelfRef.__PVT__v3__DOT__adsr__DOT__unnamedblk1__DOT__hold_zero;
    __Vdly__v3__DOT__adsr__DOT__unnamedblk1__DOT__exponential_counter 
        = vlSelfRef.__PVT__v3__DOT__adsr__DOT__unnamedblk1__DOT__exponential_counter;
    __Vdly__v3__DOT__adsr__DOT__unnamedblk1__DOT__rate_counter 
        = vlSelfRef.__PVT__v3__DOT__adsr__DOT__unnamedblk1__DOT__rate_counter;
    __Vdly__v3__DOT__adsr__DOT__state = vlSelfRef.__PVT__v3__DOT__adsr__DOT__state;
    __Vdly__v1__DOT__envelope = vlSelfRef.__PVT__v1__DOT__envelope;
    __Vdly__v2__DOT__envelope = vlSelfRef.__PVT__v2__DOT__envelope;
    __Vdly__v3__DOT__envelope = vlSelfRef.__PVT__v3__DOT__envelope;
    __Vdly__v1__DOT__oscillator = vlSelfRef.__PVT__v1__DOT__oscillator;
    __Vdly__v2__DOT__oscillator = vlSelfRef.__PVT__v2__DOT__oscillator;
    __Vdly__v3__DOT__oscillator = vlSelfRef.__PVT__v3__DOT__oscillator;
    __Vdly__tbl_state = vlSelfRef.__PVT__tbl_state;
    __Vdly__f_acc_t = vlSelfRef.__PVT__f_acc_t;
    __VdlySet___st_out__v0 = 0U;
    __VdlySet__p_t_out__v0 = 0U;
    __VdlySet__ps___05Fout__v0 = 0U;
    __VdlySet__pst_out__v0 = 0U;
    __Vdly__v1__DOT__unnamedblk2__DOT__lfsr_noise = vlSelfRef.__PVT__v1__DOT__unnamedblk2__DOT__lfsr_noise;
    __Vdly__v1__DOT__unnamedblk2__DOT__noise_age = vlSelfRef.__PVT__v1__DOT__unnamedblk2__DOT__noise_age;
    __Vdly__v2__DOT__unnamedblk2__DOT__lfsr_noise = vlSelfRef.__PVT__v2__DOT__unnamedblk2__DOT__lfsr_noise;
    __Vdly__v2__DOT__unnamedblk2__DOT__noise_age = vlSelfRef.__PVT__v2__DOT__unnamedblk2__DOT__noise_age;
    __Vdly__v3__DOT__unnamedblk2__DOT__lfsr_noise = vlSelfRef.__PVT__v3__DOT__unnamedblk2__DOT__lfsr_noise;
    __Vdly__v3__DOT__unnamedblk2__DOT__noise_age = vlSelfRef.__PVT__v3__DOT__unnamedblk2__DOT__noise_age;
    __VdlySet__voice_ctrl__v0 = 0U;
    __VdlySet__voice_ctrl__v3 = 0U;
    __VdlySet__voice_ctrl__v4 = 0U;
    __VdlySet__voice_ctrl__v5 = 0U;
    if ((0xffU == (IData)(vlSelfRef.__PVT__v1__DOT__envelope))) {
        __Vdly__v1__DOT__adsr__DOT__unnamedblk1__DOT__exponential_counter_period = 0U;
    } else if ((0x5dU == (IData)(vlSelfRef.__PVT__v1__DOT__envelope))) {
        __Vdly__v1__DOT__adsr__DOT__unnamedblk1__DOT__exponential_counter_period = 1U;
    } else if ((0x36U == (IData)(vlSelfRef.__PVT__v1__DOT__envelope))) {
        __Vdly__v1__DOT__adsr__DOT__unnamedblk1__DOT__exponential_counter_period = 3U;
    } else if ((0x1aU == (IData)(vlSelfRef.__PVT__v1__DOT__envelope))) {
        __Vdly__v1__DOT__adsr__DOT__unnamedblk1__DOT__exponential_counter_period = 7U;
    } else if ((0x0eU == (IData)(vlSelfRef.__PVT__v1__DOT__envelope))) {
        __Vdly__v1__DOT__adsr__DOT__unnamedblk1__DOT__exponential_counter_period = 0x0fU;
    } else if ((6U == (IData)(vlSelfRef.__PVT__v1__DOT__envelope))) {
        __Vdly__v1__DOT__adsr__DOT__unnamedblk1__DOT__exponential_counter_period = 0x1dU;
    } else if ((0U == (IData)(vlSelfRef.__PVT__v1__DOT__envelope))) {
        __Vdly__v1__DOT__adsr__DOT__unnamedblk1__DOT__exponential_counter_period = 0U;
    }
    if ((0xffU == (IData)(vlSelfRef.__PVT__v2__DOT__envelope))) {
        __Vdly__v2__DOT__adsr__DOT__unnamedblk1__DOT__exponential_counter_period = 0U;
    } else if ((0x5dU == (IData)(vlSelfRef.__PVT__v2__DOT__envelope))) {
        __Vdly__v2__DOT__adsr__DOT__unnamedblk1__DOT__exponential_counter_period = 1U;
    } else if ((0x36U == (IData)(vlSelfRef.__PVT__v2__DOT__envelope))) {
        __Vdly__v2__DOT__adsr__DOT__unnamedblk1__DOT__exponential_counter_period = 3U;
    } else if ((0x1aU == (IData)(vlSelfRef.__PVT__v2__DOT__envelope))) {
        __Vdly__v2__DOT__adsr__DOT__unnamedblk1__DOT__exponential_counter_period = 7U;
    } else if ((0x0eU == (IData)(vlSelfRef.__PVT__v2__DOT__envelope))) {
        __Vdly__v2__DOT__adsr__DOT__unnamedblk1__DOT__exponential_counter_period = 0x0fU;
    } else if ((6U == (IData)(vlSelfRef.__PVT__v2__DOT__envelope))) {
        __Vdly__v2__DOT__adsr__DOT__unnamedblk1__DOT__exponential_counter_period = 0x1dU;
    } else if ((0U == (IData)(vlSelfRef.__PVT__v2__DOT__envelope))) {
        __Vdly__v2__DOT__adsr__DOT__unnamedblk1__DOT__exponential_counter_period = 0U;
    }
    if ((0xffU == (IData)(vlSelfRef.__PVT__v3__DOT__envelope))) {
        __Vdly__v3__DOT__adsr__DOT__unnamedblk1__DOT__exponential_counter_period = 0U;
    } else if ((0x5dU == (IData)(vlSelfRef.__PVT__v3__DOT__envelope))) {
        __Vdly__v3__DOT__adsr__DOT__unnamedblk1__DOT__exponential_counter_period = 1U;
    } else if ((0x36U == (IData)(vlSelfRef.__PVT__v3__DOT__envelope))) {
        __Vdly__v3__DOT__adsr__DOT__unnamedblk1__DOT__exponential_counter_period = 3U;
    } else if ((0x1aU == (IData)(vlSelfRef.__PVT__v3__DOT__envelope))) {
        __Vdly__v3__DOT__adsr__DOT__unnamedblk1__DOT__exponential_counter_period = 7U;
    } else if ((0x0eU == (IData)(vlSelfRef.__PVT__v3__DOT__envelope))) {
        __Vdly__v3__DOT__adsr__DOT__unnamedblk1__DOT__exponential_counter_period = 0x0fU;
    } else if ((6U == (IData)(vlSelfRef.__PVT__v3__DOT__envelope))) {
        __Vdly__v3__DOT__adsr__DOT__unnamedblk1__DOT__exponential_counter_period = 0x1dU;
    } else if ((0U == (IData)(vlSelfRef.__PVT__v3__DOT__envelope))) {
        __Vdly__v3__DOT__adsr__DOT__unnamedblk1__DOT__exponential_counter_period = 0U;
    }
    if ((1U & (~ (0x0000000fU == (IData)(vlSelfRef.__PVT__tbl_state))))) {
        __Vdly__tbl_state = (0x0000000fU & ((IData)(1U) 
                                            + (IData)(vlSelfRef.__PVT__tbl_state)));
    }
    if (vlSymsp->TOP.top__DOT__sid_ce_1m) {
        __Vdly__v1__DOT__oscillator = ((1U & ((((IData)(vlSymsp->TOP.rst) 
                                                | (vlSelfRef.__PVT__voice_ctrl[0U] 
                                                   >> 3U)) 
                                               | (IData)(vlSelfRef.__PVT__v1__DOT__unnamedblk1__DOT__test_delay)) 
                                              | (((vlSelfRef.__PVT__voice_ctrl[0U] 
                                                   >> 1U) 
                                                  & (~ 
                                                     (vlSelfRef.__PVT__v3__DOT__oscillator 
                                                      >> 0x00000017U))) 
                                                 & (IData)(vlSelfRef.__PVT__v1__DOT__osc_msb_in_prv))))
                                        ? 0U : (0x00ffffffU 
                                                & (vlSelfRef.__PVT__v1__DOT__oscillator 
                                                   + vlSelfRef.__PVT__voice_freq[0U])));
        __Vdly__v2__DOT__oscillator = ((1U & ((((IData)(vlSymsp->TOP.rst) 
                                                | (vlSelfRef.__PVT__voice_ctrl[1U] 
                                                   >> 3U)) 
                                               | (IData)(vlSelfRef.__PVT__v2__DOT__unnamedblk1__DOT__test_delay)) 
                                              | (((vlSelfRef.__PVT__voice_ctrl[1U] 
                                                   >> 1U) 
                                                  & (~ 
                                                     (vlSelfRef.__PVT__v1__DOT__oscillator 
                                                      >> 0x00000017U))) 
                                                 & (IData)(vlSelfRef.__PVT__v2__DOT__osc_msb_in_prv))))
                                        ? 0U : (0x00ffffffU 
                                                & (vlSelfRef.__PVT__v2__DOT__oscillator 
                                                   + vlSelfRef.__PVT__voice_freq[1U])));
        __Vdly__v3__DOT__oscillator = ((1U & ((((IData)(vlSymsp->TOP.rst) 
                                                | (vlSelfRef.__PVT__voice_ctrl[2U] 
                                                   >> 3U)) 
                                               | (IData)(vlSelfRef.__PVT__v3__DOT__unnamedblk1__DOT__test_delay)) 
                                              | (((vlSelfRef.__PVT__voice_ctrl[2U] 
                                                   >> 1U) 
                                                  & (~ 
                                                     (vlSelfRef.__PVT__v2__DOT__oscillator 
                                                      >> 0x00000017U))) 
                                                 & (IData)(vlSelfRef.__PVT__v3__DOT__osc_msb_in_prv))))
                                        ? 0U : (0x00ffffffU 
                                                & (vlSelfRef.__PVT__v3__DOT__oscillator 
                                                   + vlSelfRef.__PVT__voice_freq[2U])));
        __Vdly__tbl_state = 0U;
        vlSelfRef.__PVT__v1__DOT__dca_out = (0x003fffffU 
                                             & (((1U 
                                                  & (IData)(vlSymsp->TOP.top__DOT__sid_cfg_reg))
                                                  ? 0U
                                                  : 0x00033cc0U) 
                                                + VL_MULS_III(22, 
                                                              (0x003fffffU 
                                                               & VL_EXTENDS_II(22,13, (IData)(vlSelfRef.__PVT__v1__DOT__unnamedblk3__DOT__dac_out))), 
                                                              (0x003fffffU 
                                                               & VL_EXTENDS_II(22,9, (IData)(vlSelfRef.__PVT__v1__DOT__unnamedblk3__DOT__env_dac))))));
        vlSelfRef.__PVT__v2__DOT__dca_out = (0x003fffffU 
                                             & (((1U 
                                                  & (IData)(vlSymsp->TOP.top__DOT__sid_cfg_reg))
                                                  ? 0U
                                                  : 0x00033cc0U) 
                                                + VL_MULS_III(22, 
                                                              (0x003fffffU 
                                                               & VL_EXTENDS_II(22,13, (IData)(vlSelfRef.__PVT__v2__DOT__unnamedblk3__DOT__dac_out))), 
                                                              (0x003fffffU 
                                                               & VL_EXTENDS_II(22,9, (IData)(vlSelfRef.__PVT__v2__DOT__unnamedblk3__DOT__env_dac))))));
        vlSelfRef.__PVT__v3__DOT__dca_out = (0x003fffffU 
                                             & (((1U 
                                                  & (IData)(vlSymsp->TOP.top__DOT__sid_cfg_reg))
                                                  ? 0U
                                                  : 0x00033cc0U) 
                                                + VL_MULS_III(22, 
                                                              (0x003fffffU 
                                                               & VL_EXTENDS_II(22,13, (IData)(vlSelfRef.__PVT__v3__DOT__unnamedblk3__DOT__dac_out))), 
                                                              (0x003fffffU 
                                                               & VL_EXTENDS_II(22,9, (IData)(vlSelfRef.__PVT__v3__DOT__unnamedblk3__DOT__env_dac))))));
        vlSelfRef.__PVT__v1__DOT__osc_msb_in_prv = 
            (1U & (vlSelfRef.__PVT__v3__DOT__oscillator 
                   >> 0x00000017U));
        vlSelfRef.__PVT__v1__DOT__unnamedblk1__DOT__test_delay 
            = (1U & ((IData)(vlSymsp->TOP.top__DOT__sid_cfg_reg) 
                     & (vlSelfRef.__PVT__voice_ctrl[0U] 
                        >> 3U)));
        vlSelfRef.__PVT__v2__DOT__osc_msb_in_prv = 
            (1U & (vlSelfRef.__PVT__v1__DOT__oscillator 
                   >> 0x00000017U));
        vlSelfRef.__PVT__v2__DOT__unnamedblk1__DOT__test_delay 
            = (1U & ((IData)(vlSymsp->TOP.top__DOT__sid_cfg_reg) 
                     & (vlSelfRef.__PVT__voice_ctrl[1U] 
                        >> 3U)));
        vlSelfRef.__PVT__v3__DOT__osc_msb_in_prv = 
            (1U & (vlSelfRef.__PVT__v2__DOT__oscillator 
                   >> 0x00000017U));
        vlSelfRef.__PVT__v3__DOT__unnamedblk1__DOT__test_delay 
            = (1U & ((IData)(vlSymsp->TOP.top__DOT__sid_cfg_reg) 
                     & (vlSelfRef.__PVT__voice_ctrl[2U] 
                        >> 3U)));
    }
    if (vlSymsp->TOP.rst) {
        __VdlySet__voice_ad__v0 = 1U;
        __VdlySet__voice_sr__v0 = 1U;
        __VdlySet__voice_pw__v0 = 1U;
        __VdlySet__voice_freq__v0 = 1U;
        __VdlySet__voice_vol__v0 = 1U;
        __Vdly__v1__DOT__adsr__DOT__state = 0U;
        __Vdly__v1__DOT__envelope = 0U;
        __Vdly__v1__DOT__adsr__DOT__unnamedblk1__DOT__hold_zero = 1U;
        __Vdly__v1__DOT__adsr__DOT__unnamedblk1__DOT__exponential_counter = 0U;
        __Vdly__v1__DOT__adsr__DOT__unnamedblk1__DOT__exponential_counter_period = 0U;
        __Vdly__v1__DOT__adsr__DOT__unnamedblk1__DOT__rate_counter = 0U;
        __Vdly__v2__DOT__adsr__DOT__state = 0U;
        __Vdly__v2__DOT__envelope = 0U;
        __Vdly__v2__DOT__adsr__DOT__unnamedblk1__DOT__hold_zero = 1U;
        __Vdly__v2__DOT__adsr__DOT__unnamedblk1__DOT__exponential_counter = 0U;
        __Vdly__v2__DOT__adsr__DOT__unnamedblk1__DOT__exponential_counter_period = 0U;
        __Vdly__v2__DOT__adsr__DOT__unnamedblk1__DOT__rate_counter = 0U;
        __Vdly__v3__DOT__adsr__DOT__state = 0U;
        __Vdly__v3__DOT__envelope = 0U;
        __Vdly__v3__DOT__adsr__DOT__unnamedblk1__DOT__hold_zero = 1U;
        __Vdly__v3__DOT__adsr__DOT__unnamedblk1__DOT__exponential_counter = 0U;
        __Vdly__v3__DOT__adsr__DOT__unnamedblk1__DOT__exponential_counter_period = 0U;
        __Vdly__v3__DOT__adsr__DOT__unnamedblk1__DOT__rate_counter = 0U;
        __VdlySet__voice_ctrl__v0 = 1U;
        vlSelfRef.__PVT__last_written = 0U;
        vlSelfRef.__PVT__v1__DOT__saw_tri = 0U;
        vlSelfRef.__PVT__v2__DOT__saw_tri = 0U;
        vlSelfRef.__PVT__v3__DOT__saw_tri = 0U;
        vlSelfRef.__PVT__v1__DOT__pulse = 0U;
        vlSelfRef.__PVT__v2__DOT__pulse = 0U;
        vlSelfRef.__PVT__v3__DOT__pulse = 0U;
        vlSelfRef.__PVT__v1__DOT__noise = 0U;
        __Vdly__v1__DOT__unnamedblk2__DOT__lfsr_noise = 0x007fffffU;
        __Vdly__v1__DOT__unnamedblk2__DOT__noise_age = 0U;
    } else {
        if (vlSelfRef.cs) {
            if (((((((((0U == (0x0000001fU & (IData)(vlSymsp->TOP.top__DOT__cpu_addr))) 
                       | (1U == (0x0000001fU & (IData)(vlSymsp->TOP.top__DOT__cpu_addr)))) 
                      | (2U == (0x0000001fU & (IData)(vlSymsp->TOP.top__DOT__cpu_addr)))) 
                     | (3U == (0x0000001fU & (IData)(vlSymsp->TOP.top__DOT__cpu_addr)))) 
                    | (4U == (0x0000001fU & (IData)(vlSymsp->TOP.top__DOT__cpu_addr)))) 
                   | (5U == (0x0000001fU & (IData)(vlSymsp->TOP.top__DOT__cpu_addr)))) 
                  | (6U == (0x0000001fU & (IData)(vlSymsp->TOP.top__DOT__cpu_addr)))) 
                 | (7U == (0x0000001fU & (IData)(vlSymsp->TOP.top__DOT__cpu_addr))))) {
                if ((0U != (0x0000001fU & (IData)(vlSymsp->TOP.top__DOT__cpu_addr)))) {
                    if ((1U != (0x0000001fU & (IData)(vlSymsp->TOP.top__DOT__cpu_addr)))) {
                        if ((2U != (0x0000001fU & (IData)(vlSymsp->TOP.top__DOT__cpu_addr)))) {
                            if ((3U != (0x0000001fU 
                                        & (IData)(vlSymsp->TOP.top__DOT__cpu_addr)))) {
                                if ((4U != (0x0000001fU 
                                            & (IData)(vlSymsp->TOP.top__DOT__cpu_addr)))) {
                                    if ((5U == (0x0000001fU 
                                                & (IData)(vlSymsp->TOP.top__DOT__cpu_addr)))) {
                                        __VdlyVal__voice_ad__v3 
                                            = vlSymsp->TOP.top__DOT__cpu_dout;
                                        __VdlySet__voice_ad__v3 = 1U;
                                    }
                                    if ((5U != (0x0000001fU 
                                                & (IData)(vlSymsp->TOP.top__DOT__cpu_addr)))) {
                                        if ((6U == 
                                             (0x0000001fU 
                                              & (IData)(vlSymsp->TOP.top__DOT__cpu_addr)))) {
                                            __VdlyVal__voice_sr__v3 
                                                = vlSymsp->TOP.top__DOT__cpu_dout;
                                            __VdlySet__voice_sr__v3 = 1U;
                                        }
                                    }
                                }
                                if ((4U == (0x0000001fU 
                                            & (IData)(vlSymsp->TOP.top__DOT__cpu_addr)))) {
                                    __VdlyVal__voice_ctrl__v3 
                                        = vlSymsp->TOP.top__DOT__cpu_dout;
                                    __VdlySet__voice_ctrl__v3 = 1U;
                                }
                            }
                        }
                        if ((2U == (0x0000001fU & (IData)(vlSymsp->TOP.top__DOT__cpu_addr)))) {
                            __VdlyVal__voice_pw__v3 
                                = vlSymsp->TOP.top__DOT__cpu_dout;
                            __VdlySet__voice_pw__v3 = 1U;
                        } else if ((3U == (0x0000001fU 
                                           & (IData)(vlSymsp->TOP.top__DOT__cpu_addr)))) {
                            __VdlyVal__voice_pw__v4 
                                = (0x0000000fU & (IData)(vlSymsp->TOP.top__DOT__cpu_dout));
                            __VdlySet__voice_pw__v4 = 1U;
                        }
                    }
                }
                if ((0U == (0x0000001fU & (IData)(vlSymsp->TOP.top__DOT__cpu_addr)))) {
                    __VdlyVal__voice_freq__v3 = vlSymsp->TOP.top__DOT__cpu_dout;
                    __VdlySet__voice_freq__v3 = 1U;
                } else if ((1U == (0x0000001fU & (IData)(vlSymsp->TOP.top__DOT__cpu_addr)))) {
                    __VdlyVal__voice_freq__v4 = vlSymsp->TOP.top__DOT__cpu_dout;
                    __VdlySet__voice_freq__v4 = 1U;
                } else if ((2U != (0x0000001fU & (IData)(vlSymsp->TOP.top__DOT__cpu_addr)))) {
                    if ((3U != (0x0000001fU & (IData)(vlSymsp->TOP.top__DOT__cpu_addr)))) {
                        if ((4U != (0x0000001fU & (IData)(vlSymsp->TOP.top__DOT__cpu_addr)))) {
                            if ((5U != (0x0000001fU 
                                        & (IData)(vlSymsp->TOP.top__DOT__cpu_addr)))) {
                                if ((6U != (0x0000001fU 
                                            & (IData)(vlSymsp->TOP.top__DOT__cpu_addr)))) {
                                    __VdlyVal__voice_freq__v5 
                                        = vlSymsp->TOP.top__DOT__cpu_dout;
                                    __VdlySet__voice_freq__v5 = 1U;
                                }
                            }
                        }
                    }
                }
            } else if (((((((((8U == (0x0000001fU & (IData)(vlSymsp->TOP.top__DOT__cpu_addr))) 
                              | (9U == (0x0000001fU 
                                        & (IData)(vlSymsp->TOP.top__DOT__cpu_addr)))) 
                             | (0x0aU == (0x0000001fU 
                                          & (IData)(vlSymsp->TOP.top__DOT__cpu_addr)))) 
                            | (0x0bU == (0x0000001fU 
                                         & (IData)(vlSymsp->TOP.top__DOT__cpu_addr)))) 
                           | (0x0cU == (0x0000001fU 
                                        & (IData)(vlSymsp->TOP.top__DOT__cpu_addr)))) 
                          | (0x0dU == (0x0000001fU 
                                       & (IData)(vlSymsp->TOP.top__DOT__cpu_addr)))) 
                         | (0x0eU == (0x0000001fU & (IData)(vlSymsp->TOP.top__DOT__cpu_addr)))) 
                        | (0x0fU == (0x0000001fU & (IData)(vlSymsp->TOP.top__DOT__cpu_addr))))) {
                if ((8U != (0x0000001fU & (IData)(vlSymsp->TOP.top__DOT__cpu_addr)))) {
                    if ((9U != (0x0000001fU & (IData)(vlSymsp->TOP.top__DOT__cpu_addr)))) {
                        if ((0x0aU != (0x0000001fU 
                                       & (IData)(vlSymsp->TOP.top__DOT__cpu_addr)))) {
                            if ((0x0bU != (0x0000001fU 
                                           & (IData)(vlSymsp->TOP.top__DOT__cpu_addr)))) {
                                if ((0x0cU == (0x0000001fU 
                                               & (IData)(vlSymsp->TOP.top__DOT__cpu_addr)))) {
                                    __VdlyVal__voice_ad__v4 
                                        = vlSymsp->TOP.top__DOT__cpu_dout;
                                    __VdlySet__voice_ad__v4 = 1U;
                                }
                                if ((0x0cU != (0x0000001fU 
                                               & (IData)(vlSymsp->TOP.top__DOT__cpu_addr)))) {
                                    if ((0x0dU == (0x0000001fU 
                                                   & (IData)(vlSymsp->TOP.top__DOT__cpu_addr)))) {
                                        __VdlyVal__voice_sr__v4 
                                            = vlSymsp->TOP.top__DOT__cpu_dout;
                                        __VdlySet__voice_sr__v4 = 1U;
                                    }
                                }
                            }
                            if ((0x0bU == (0x0000001fU 
                                           & (IData)(vlSymsp->TOP.top__DOT__cpu_addr)))) {
                                __VdlyVal__voice_ctrl__v4 
                                    = vlSymsp->TOP.top__DOT__cpu_dout;
                                __VdlySet__voice_ctrl__v4 = 1U;
                            }
                        }
                    }
                    if ((9U == (0x0000001fU & (IData)(vlSymsp->TOP.top__DOT__cpu_addr)))) {
                        __VdlyVal__voice_pw__v5 = vlSymsp->TOP.top__DOT__cpu_dout;
                        __VdlySet__voice_pw__v5 = 1U;
                    } else if ((0x0aU == (0x0000001fU 
                                          & (IData)(vlSymsp->TOP.top__DOT__cpu_addr)))) {
                        __VdlyVal__voice_pw__v6 = (0x0000000fU 
                                                   & (IData)(vlSymsp->TOP.top__DOT__cpu_dout));
                        __VdlySet__voice_pw__v6 = 1U;
                    }
                }
                if ((8U == (0x0000001fU & (IData)(vlSymsp->TOP.top__DOT__cpu_addr)))) {
                    __VdlyVal__voice_freq__v6 = vlSymsp->TOP.top__DOT__cpu_dout;
                    __VdlySet__voice_freq__v6 = 1U;
                } else if ((9U != (0x0000001fU & (IData)(vlSymsp->TOP.top__DOT__cpu_addr)))) {
                    if ((0x0aU != (0x0000001fU & (IData)(vlSymsp->TOP.top__DOT__cpu_addr)))) {
                        if ((0x0bU != (0x0000001fU 
                                       & (IData)(vlSymsp->TOP.top__DOT__cpu_addr)))) {
                            if ((0x0cU != (0x0000001fU 
                                           & (IData)(vlSymsp->TOP.top__DOT__cpu_addr)))) {
                                if ((0x0dU != (0x0000001fU 
                                               & (IData)(vlSymsp->TOP.top__DOT__cpu_addr)))) {
                                    if ((0x0eU == (0x0000001fU 
                                                   & (IData)(vlSymsp->TOP.top__DOT__cpu_addr)))) {
                                        __VdlyVal__voice_freq__v7 
                                            = vlSymsp->TOP.top__DOT__cpu_dout;
                                        __VdlySet__voice_freq__v7 = 1U;
                                    } else {
                                        __VdlyVal__voice_freq__v8 
                                            = vlSymsp->TOP.top__DOT__cpu_dout;
                                        __VdlySet__voice_freq__v8 = 1U;
                                    }
                                }
                            }
                        }
                    }
                }
            } else if (((((((((0x10U == (0x0000001fU 
                                         & (IData)(vlSymsp->TOP.top__DOT__cpu_addr))) 
                              | (0x11U == (0x0000001fU 
                                           & (IData)(vlSymsp->TOP.top__DOT__cpu_addr)))) 
                             | (0x12U == (0x0000001fU 
                                          & (IData)(vlSymsp->TOP.top__DOT__cpu_addr)))) 
                            | (0x13U == (0x0000001fU 
                                         & (IData)(vlSymsp->TOP.top__DOT__cpu_addr)))) 
                           | (0x14U == (0x0000001fU 
                                        & (IData)(vlSymsp->TOP.top__DOT__cpu_addr)))) 
                          | (0x15U == (0x0000001fU 
                                       & (IData)(vlSymsp->TOP.top__DOT__cpu_addr)))) 
                         | (0x16U == (0x0000001fU & (IData)(vlSymsp->TOP.top__DOT__cpu_addr)))) 
                        | (0x17U == (0x0000001fU & (IData)(vlSymsp->TOP.top__DOT__cpu_addr))))) {
                if ((0x10U != (0x0000001fU & (IData)(vlSymsp->TOP.top__DOT__cpu_addr)))) {
                    if ((0x11U != (0x0000001fU & (IData)(vlSymsp->TOP.top__DOT__cpu_addr)))) {
                        if ((0x12U != (0x0000001fU 
                                       & (IData)(vlSymsp->TOP.top__DOT__cpu_addr)))) {
                            if ((0x13U == (0x0000001fU 
                                           & (IData)(vlSymsp->TOP.top__DOT__cpu_addr)))) {
                                __VdlyVal__voice_ad__v5 
                                    = vlSymsp->TOP.top__DOT__cpu_dout;
                                __VdlySet__voice_ad__v5 = 1U;
                            }
                            if ((0x13U != (0x0000001fU 
                                           & (IData)(vlSymsp->TOP.top__DOT__cpu_addr)))) {
                                if ((0x14U == (0x0000001fU 
                                               & (IData)(vlSymsp->TOP.top__DOT__cpu_addr)))) {
                                    __VdlyVal__voice_sr__v5 
                                        = vlSymsp->TOP.top__DOT__cpu_dout;
                                    __VdlySet__voice_sr__v5 = 1U;
                                }
                            }
                        }
                        if ((0x12U == (0x0000001fU 
                                       & (IData)(vlSymsp->TOP.top__DOT__cpu_addr)))) {
                            __VdlyVal__voice_ctrl__v5 
                                = vlSymsp->TOP.top__DOT__cpu_dout;
                            __VdlySet__voice_ctrl__v5 = 1U;
                        }
                    }
                }
                if ((0x10U == (0x0000001fU & (IData)(vlSymsp->TOP.top__DOT__cpu_addr)))) {
                    __VdlyVal__voice_pw__v7 = vlSymsp->TOP.top__DOT__cpu_dout;
                    __VdlySet__voice_pw__v7 = 1U;
                } else if ((0x11U == (0x0000001fU & (IData)(vlSymsp->TOP.top__DOT__cpu_addr)))) {
                    __VdlyVal__voice_pw__v8 = (0x0000000fU 
                                               & (IData)(vlSymsp->TOP.top__DOT__cpu_dout));
                    __VdlySet__voice_pw__v8 = 1U;
                }
            }
            if ((1U & (~ ((((((((0U == (0x0000001fU 
                                        & (IData)(vlSymsp->TOP.top__DOT__cpu_addr))) 
                                | (1U == (0x0000001fU 
                                          & (IData)(vlSymsp->TOP.top__DOT__cpu_addr)))) 
                               | (2U == (0x0000001fU 
                                         & (IData)(vlSymsp->TOP.top__DOT__cpu_addr)))) 
                              | (3U == (0x0000001fU 
                                        & (IData)(vlSymsp->TOP.top__DOT__cpu_addr)))) 
                             | (4U == (0x0000001fU 
                                       & (IData)(vlSymsp->TOP.top__DOT__cpu_addr)))) 
                            | (5U == (0x0000001fU & (IData)(vlSymsp->TOP.top__DOT__cpu_addr)))) 
                           | (6U == (0x0000001fU & (IData)(vlSymsp->TOP.top__DOT__cpu_addr)))) 
                          | (7U == (0x0000001fU & (IData)(vlSymsp->TOP.top__DOT__cpu_addr))))))) {
                if ((1U & (~ ((((((((8U == (0x0000001fU 
                                            & (IData)(vlSymsp->TOP.top__DOT__cpu_addr))) 
                                    | (9U == (0x0000001fU 
                                              & (IData)(vlSymsp->TOP.top__DOT__cpu_addr)))) 
                                   | (0x0aU == (0x0000001fU 
                                                & (IData)(vlSymsp->TOP.top__DOT__cpu_addr)))) 
                                  | (0x0bU == (0x0000001fU 
                                               & (IData)(vlSymsp->TOP.top__DOT__cpu_addr)))) 
                                 | (0x0cU == (0x0000001fU 
                                              & (IData)(vlSymsp->TOP.top__DOT__cpu_addr)))) 
                                | (0x0dU == (0x0000001fU 
                                             & (IData)(vlSymsp->TOP.top__DOT__cpu_addr)))) 
                               | (0x0eU == (0x0000001fU 
                                            & (IData)(vlSymsp->TOP.top__DOT__cpu_addr)))) 
                              | (0x0fU == (0x0000001fU 
                                           & (IData)(vlSymsp->TOP.top__DOT__cpu_addr))))))) {
                    if ((1U & (~ ((((((((0x10U == (0x0000001fU 
                                                   & (IData)(vlSymsp->TOP.top__DOT__cpu_addr))) 
                                        | (0x11U == 
                                           (0x0000001fU 
                                            & (IData)(vlSymsp->TOP.top__DOT__cpu_addr)))) 
                                       | (0x12U == 
                                          (0x0000001fU 
                                           & (IData)(vlSymsp->TOP.top__DOT__cpu_addr)))) 
                                      | (0x13U == (0x0000001fU 
                                                   & (IData)(vlSymsp->TOP.top__DOT__cpu_addr)))) 
                                     | (0x14U == (0x0000001fU 
                                                  & (IData)(vlSymsp->TOP.top__DOT__cpu_addr)))) 
                                    | (0x15U == (0x0000001fU 
                                                 & (IData)(vlSymsp->TOP.top__DOT__cpu_addr)))) 
                                   | (0x16U == (0x0000001fU 
                                                & (IData)(vlSymsp->TOP.top__DOT__cpu_addr)))) 
                                  | (0x17U == (0x0000001fU 
                                               & (IData)(vlSymsp->TOP.top__DOT__cpu_addr))))))) {
                        if ((0x18U != (0x0000001fU 
                                       & (IData)(vlSymsp->TOP.top__DOT__cpu_addr)))) {
                            if ((0x1dU == (0x0000001fU 
                                           & (IData)(vlSymsp->TOP.top__DOT__cpu_addr)))) {
                                __VdlyVal__voice_vol__v3 
                                    = (0x0000000fU 
                                       & (IData)(vlSymsp->TOP.top__DOT__cpu_dout));
                                __VdlySet__voice_vol__v3 = 1U;
                            } else if ((0x1eU == (0x0000001fU 
                                                  & (IData)(vlSymsp->TOP.top__DOT__cpu_addr)))) {
                                __VdlyVal__voice_vol__v4 
                                    = (0x0000000fU 
                                       & (IData)(vlSymsp->TOP.top__DOT__cpu_dout));
                                __VdlySet__voice_vol__v4 = 1U;
                            } else if ((0x1fU == (0x0000001fU 
                                                  & (IData)(vlSymsp->TOP.top__DOT__cpu_addr)))) {
                                __VdlyVal__voice_vol__v5 
                                    = (0x0000000fU 
                                       & (IData)(vlSymsp->TOP.top__DOT__cpu_dout));
                                __VdlySet__voice_vol__v5 = 1U;
                            }
                        }
                    }
                }
            }
            vlSelfRef.__PVT__last_written = vlSymsp->TOP.top__DOT__cpu_dout;
        }
        if (vlSymsp->TOP.top__DOT__sid_ce_1m) {
            __Vdly__v1__DOT__adsr__DOT__unnamedblk1__DOT__rate_counter 
                = (0x00007fffU & ((IData)(1U) + (IData)(vlSelfRef.__PVT__v1__DOT__adsr__DOT__unnamedblk1__DOT__rate_counter)));
            if (((IData)(vlSelfRef.__PVT__v1__DOT__adsr__DOT__unnamedblk1__DOT__rate_counter) 
                 == (IData)(vlSelfRef.__PVT__v1__DOT__adsr__DOT__rate_period))) {
                __Vdly__v1__DOT__adsr__DOT__unnamedblk1__DOT__rate_counter = 0U;
                __Vdly__v1__DOT__adsr__DOT__unnamedblk1__DOT__exponential_counter 
                    = (0x0000001fU & ((IData)(1U) + (IData)(vlSelfRef.__PVT__v1__DOT__adsr__DOT__unnamedblk1__DOT__exponential_counter)));
                if (((1U == (IData)(vlSelfRef.__PVT__v1__DOT__adsr__DOT__state)) 
                     | ((IData)(vlSelfRef.__PVT__v1__DOT__adsr__DOT__unnamedblk1__DOT__exponential_counter) 
                        == (IData)(vlSelfRef.__PVT__v1__DOT__adsr__DOT__unnamedblk1__DOT__exponential_counter_period)))) {
                    __Vdly__v1__DOT__adsr__DOT__unnamedblk1__DOT__exponential_counter = 0U;
                    if ((1U == (IData)(vlSelfRef.__PVT__v1__DOT__adsr__DOT__state))) {
                        __Vdly__v1__DOT__envelope = 
                            (0x000000ffU & ((IData)(1U) 
                                            + (IData)(vlSelfRef.__PVT__v1__DOT__envelope)));
                        if ((0xfeU == (IData)(vlSelfRef.__PVT__v1__DOT__envelope))) {
                            __Vdly__v1__DOT__adsr__DOT__state = 2U;
                        }
                    } else if ((2U == (IData)(vlSelfRef.__PVT__v1__DOT__adsr__DOT__state))) {
                        if ((((IData)(vlSelfRef.__PVT__v1__DOT__envelope) 
                              != (0x000000ffU & ((0x000000f0U 
                                                  & vlSelfRef.__PVT__voice_sr[0U]) 
                                                 | (0x0000000fU 
                                                    & (vlSelfRef.__PVT__voice_sr[0U] 
                                                       >> 4U))))) 
                             & (~ (IData)(vlSelfRef.__PVT__v1__DOT__adsr__DOT__unnamedblk1__DOT__hold_zero)))) {
                            __Vdly__v1__DOT__envelope 
                                = (0x000000ffU & ((IData)(vlSelfRef.__PVT__v1__DOT__envelope) 
                                                  - (IData)(1U)));
                        }
                    } else if ((0U == (IData)(vlSelfRef.__PVT__v1__DOT__adsr__DOT__state))) {
                        if ((1U & (~ (IData)(vlSelfRef.__PVT__v1__DOT__adsr__DOT__unnamedblk1__DOT__hold_zero)))) {
                            __Vdly__v1__DOT__envelope 
                                = (0x000000ffU & ((IData)(vlSelfRef.__PVT__v1__DOT__envelope) 
                                                  - (IData)(1U)));
                        }
                    }
                    if (((1U != (IData)(vlSelfRef.__PVT__v1__DOT__adsr__DOT__state)) 
                         & (1U == (IData)(vlSelfRef.__PVT__v1__DOT__envelope)))) {
                        __Vdly__v1__DOT__adsr__DOT__unnamedblk1__DOT__hold_zero = 1U;
                    }
                }
            }
            if ((1U & ((~ (IData)(vlSelfRef.__PVT__v1__DOT__adsr__DOT__unnamedblk1__DOT__gate_edge)) 
                       & vlSelfRef.__PVT__voice_ctrl[0U]))) {
                __Vdly__v1__DOT__adsr__DOT__state = 1U;
                __Vdly__v1__DOT__adsr__DOT__unnamedblk1__DOT__hold_zero = 0U;
            }
            if (((IData)(vlSelfRef.__PVT__v1__DOT__adsr__DOT__unnamedblk1__DOT__gate_edge) 
                 & (~ vlSelfRef.__PVT__voice_ctrl[0U]))) {
                __Vdly__v1__DOT__adsr__DOT__state = 0U;
            }
            __Vdly__v2__DOT__adsr__DOT__unnamedblk1__DOT__rate_counter 
                = (0x00007fffU & ((IData)(1U) + (IData)(vlSelfRef.__PVT__v2__DOT__adsr__DOT__unnamedblk1__DOT__rate_counter)));
            if (((IData)(vlSelfRef.__PVT__v2__DOT__adsr__DOT__unnamedblk1__DOT__rate_counter) 
                 == (IData)(vlSelfRef.__PVT__v2__DOT__adsr__DOT__rate_period))) {
                __Vdly__v2__DOT__adsr__DOT__unnamedblk1__DOT__rate_counter = 0U;
                __Vdly__v2__DOT__adsr__DOT__unnamedblk1__DOT__exponential_counter 
                    = (0x0000001fU & ((IData)(1U) + (IData)(vlSelfRef.__PVT__v2__DOT__adsr__DOT__unnamedblk1__DOT__exponential_counter)));
                if (((1U == (IData)(vlSelfRef.__PVT__v2__DOT__adsr__DOT__state)) 
                     | ((IData)(vlSelfRef.__PVT__v2__DOT__adsr__DOT__unnamedblk1__DOT__exponential_counter) 
                        == (IData)(vlSelfRef.__PVT__v2__DOT__adsr__DOT__unnamedblk1__DOT__exponential_counter_period)))) {
                    __Vdly__v2__DOT__adsr__DOT__unnamedblk1__DOT__exponential_counter = 0U;
                    if ((1U == (IData)(vlSelfRef.__PVT__v2__DOT__adsr__DOT__state))) {
                        __Vdly__v2__DOT__envelope = 
                            (0x000000ffU & ((IData)(1U) 
                                            + (IData)(vlSelfRef.__PVT__v2__DOT__envelope)));
                        if ((0xfeU == (IData)(vlSelfRef.__PVT__v2__DOT__envelope))) {
                            __Vdly__v2__DOT__adsr__DOT__state = 2U;
                        }
                    } else if ((2U == (IData)(vlSelfRef.__PVT__v2__DOT__adsr__DOT__state))) {
                        if ((((IData)(vlSelfRef.__PVT__v2__DOT__envelope) 
                              != (0x000000ffU & ((0x000000f0U 
                                                  & vlSelfRef.__PVT__voice_sr[1U]) 
                                                 | (0x0000000fU 
                                                    & (vlSelfRef.__PVT__voice_sr[1U] 
                                                       >> 4U))))) 
                             & (~ (IData)(vlSelfRef.__PVT__v2__DOT__adsr__DOT__unnamedblk1__DOT__hold_zero)))) {
                            __Vdly__v2__DOT__envelope 
                                = (0x000000ffU & ((IData)(vlSelfRef.__PVT__v2__DOT__envelope) 
                                                  - (IData)(1U)));
                        }
                    } else if ((0U == (IData)(vlSelfRef.__PVT__v2__DOT__adsr__DOT__state))) {
                        if ((1U & (~ (IData)(vlSelfRef.__PVT__v2__DOT__adsr__DOT__unnamedblk1__DOT__hold_zero)))) {
                            __Vdly__v2__DOT__envelope 
                                = (0x000000ffU & ((IData)(vlSelfRef.__PVT__v2__DOT__envelope) 
                                                  - (IData)(1U)));
                        }
                    }
                    if (((1U != (IData)(vlSelfRef.__PVT__v2__DOT__adsr__DOT__state)) 
                         & (1U == (IData)(vlSelfRef.__PVT__v2__DOT__envelope)))) {
                        __Vdly__v2__DOT__adsr__DOT__unnamedblk1__DOT__hold_zero = 1U;
                    }
                }
            }
            if ((1U & ((~ (IData)(vlSelfRef.__PVT__v2__DOT__adsr__DOT__unnamedblk1__DOT__gate_edge)) 
                       & vlSelfRef.__PVT__voice_ctrl[1U]))) {
                __Vdly__v2__DOT__adsr__DOT__state = 1U;
                __Vdly__v2__DOT__adsr__DOT__unnamedblk1__DOT__hold_zero = 0U;
            }
            if (((IData)(vlSelfRef.__PVT__v2__DOT__adsr__DOT__unnamedblk1__DOT__gate_edge) 
                 & (~ vlSelfRef.__PVT__voice_ctrl[1U]))) {
                __Vdly__v2__DOT__adsr__DOT__state = 0U;
            }
            __Vdly__v3__DOT__adsr__DOT__unnamedblk1__DOT__rate_counter 
                = (0x00007fffU & ((IData)(1U) + (IData)(vlSelfRef.__PVT__v3__DOT__adsr__DOT__unnamedblk1__DOT__rate_counter)));
            if (((IData)(vlSelfRef.__PVT__v3__DOT__adsr__DOT__unnamedblk1__DOT__rate_counter) 
                 == (IData)(vlSelfRef.__PVT__v3__DOT__adsr__DOT__rate_period))) {
                __Vdly__v3__DOT__adsr__DOT__unnamedblk1__DOT__rate_counter = 0U;
                __Vdly__v3__DOT__adsr__DOT__unnamedblk1__DOT__exponential_counter 
                    = (0x0000001fU & ((IData)(1U) + (IData)(vlSelfRef.__PVT__v3__DOT__adsr__DOT__unnamedblk1__DOT__exponential_counter)));
                if (((1U == (IData)(vlSelfRef.__PVT__v3__DOT__adsr__DOT__state)) 
                     | ((IData)(vlSelfRef.__PVT__v3__DOT__adsr__DOT__unnamedblk1__DOT__exponential_counter) 
                        == (IData)(vlSelfRef.__PVT__v3__DOT__adsr__DOT__unnamedblk1__DOT__exponential_counter_period)))) {
                    __Vdly__v3__DOT__adsr__DOT__unnamedblk1__DOT__exponential_counter = 0U;
                    if ((1U == (IData)(vlSelfRef.__PVT__v3__DOT__adsr__DOT__state))) {
                        __Vdly__v3__DOT__envelope = 
                            (0x000000ffU & ((IData)(1U) 
                                            + (IData)(vlSelfRef.__PVT__v3__DOT__envelope)));
                        if ((0xfeU == (IData)(vlSelfRef.__PVT__v3__DOT__envelope))) {
                            __Vdly__v3__DOT__adsr__DOT__state = 2U;
                        }
                    } else if ((2U == (IData)(vlSelfRef.__PVT__v3__DOT__adsr__DOT__state))) {
                        if ((((IData)(vlSelfRef.__PVT__v3__DOT__envelope) 
                              != (0x000000ffU & ((0x000000f0U 
                                                  & vlSelfRef.__PVT__voice_sr[2U]) 
                                                 | (0x0000000fU 
                                                    & (vlSelfRef.__PVT__voice_sr[2U] 
                                                       >> 4U))))) 
                             & (~ (IData)(vlSelfRef.__PVT__v3__DOT__adsr__DOT__unnamedblk1__DOT__hold_zero)))) {
                            __Vdly__v3__DOT__envelope 
                                = (0x000000ffU & ((IData)(vlSelfRef.__PVT__v3__DOT__envelope) 
                                                  - (IData)(1U)));
                        }
                    } else if ((0U == (IData)(vlSelfRef.__PVT__v3__DOT__adsr__DOT__state))) {
                        if ((1U & (~ (IData)(vlSelfRef.__PVT__v3__DOT__adsr__DOT__unnamedblk1__DOT__hold_zero)))) {
                            __Vdly__v3__DOT__envelope 
                                = (0x000000ffU & ((IData)(vlSelfRef.__PVT__v3__DOT__envelope) 
                                                  - (IData)(1U)));
                        }
                    }
                    if (((1U != (IData)(vlSelfRef.__PVT__v3__DOT__adsr__DOT__state)) 
                         & (1U == (IData)(vlSelfRef.__PVT__v3__DOT__envelope)))) {
                        __Vdly__v3__DOT__adsr__DOT__unnamedblk1__DOT__hold_zero = 1U;
                    }
                }
            }
            if ((1U & ((~ (IData)(vlSelfRef.__PVT__v3__DOT__adsr__DOT__unnamedblk1__DOT__gate_edge)) 
                       & vlSelfRef.__PVT__voice_ctrl[2U]))) {
                __Vdly__v3__DOT__adsr__DOT__state = 1U;
                __Vdly__v3__DOT__adsr__DOT__unnamedblk1__DOT__hold_zero = 0U;
            }
            if (((IData)(vlSelfRef.__PVT__v3__DOT__adsr__DOT__unnamedblk1__DOT__gate_edge) 
                 & (~ vlSelfRef.__PVT__voice_ctrl[2U]))) {
                __Vdly__v3__DOT__adsr__DOT__state = 0U;
            }
            vlSelfRef.__PVT__v1__DOT__saw_tri = vlSelfRef.__Vcellout__v1__acc_t;
            vlSelfRef.__PVT__v2__DOT__saw_tri = vlSelfRef.__Vcellout__v2__acc_t;
            vlSelfRef.__PVT__v3__DOT__saw_tri = vlSelfRef.__Vcellout__v3__acc_t;
            vlSelfRef.__PVT__v1__DOT__pulse = (1U & 
                                               ((vlSelfRef.__PVT__voice_ctrl[0U] 
                                                 >> 3U) 
                                                | ((0x00000fffU 
                                                    & (vlSelfRef.__PVT__v1__DOT__oscillator 
                                                       >> 0x0cU)) 
                                                   >= vlSelfRef.__PVT__voice_pw[0U])));
            vlSelfRef.__PVT__v2__DOT__pulse = (1U & 
                                               ((vlSelfRef.__PVT__voice_ctrl[1U] 
                                                 >> 3U) 
                                                | ((0x00000fffU 
                                                    & (vlSelfRef.__PVT__v2__DOT__oscillator 
                                                       >> 0x0cU)) 
                                                   >= vlSelfRef.__PVT__voice_pw[1U])));
            vlSelfRef.__PVT__v3__DOT__pulse = (1U & 
                                               ((vlSelfRef.__PVT__voice_ctrl[2U] 
                                                 >> 3U) 
                                                | ((0x00000fffU 
                                                    & (vlSelfRef.__PVT__v3__DOT__oscillator 
                                                       >> 0x0cU)) 
                                                   >= vlSelfRef.__PVT__voice_pw[2U])));
            vlSelfRef.__PVT__v1__DOT__noise = (((((4U 
                                                   & (vlSelfRef.__PVT__v1__DOT__unnamedblk2__DOT__lfsr_noise 
                                                      >> 0x00000012U)) 
                                                  | ((2U 
                                                      & (vlSelfRef.__PVT__v1__DOT__unnamedblk2__DOT__lfsr_noise 
                                                         >> 0x00000011U)) 
                                                     | (1U 
                                                        & (vlSelfRef.__PVT__v1__DOT__unnamedblk2__DOT__lfsr_noise 
                                                           >> 0x0eU)))) 
                                                 << 9U) 
                                                | (((4U 
                                                     & (vlSelfRef.__PVT__v1__DOT__unnamedblk2__DOT__lfsr_noise 
                                                        >> 9U)) 
                                                    | ((2U 
                                                        & (vlSelfRef.__PVT__v1__DOT__unnamedblk2__DOT__lfsr_noise 
                                                           >> 8U)) 
                                                       | (1U 
                                                          & (vlSelfRef.__PVT__v1__DOT__unnamedblk2__DOT__lfsr_noise 
                                                             >> 5U)))) 
                                                   << 6U)) 
                                               | ((0x00000020U 
                                                   & (vlSelfRef.__PVT__v1__DOT__unnamedblk2__DOT__lfsr_noise 
                                                      << 3U)) 
                                                  | (0x00000010U 
                                                     & (vlSelfRef.__PVT__v1__DOT__unnamedblk2__DOT__lfsr_noise 
                                                        << 4U))));
            if (vlSelfRef.__PVT__v1__DOT__unnamedblk2__DOT__clk) {
                if (((IData)(vlSelfRef.__PVT__v1__DOT__unnamedblk2__DOT__clk) 
                     & (~ (IData)(vlSelfRef.__PVT__v1__DOT__unnamedblk2__DOT__clk_d)))) {
                    __Vdly__v1__DOT__unnamedblk2__DOT__lfsr_noise 
                        = ((0x007ffffeU & (vlSelfRef.__PVT__v1__DOT__unnamedblk2__DOT__lfsr_noise 
                                           << 1U)) 
                           | (1U & ((((IData)(vlSymsp->TOP.rst) 
                                      | (vlSelfRef.__PVT__voice_ctrl[0U] 
                                         >> 3U)) | 
                                     (vlSelfRef.__PVT__v1__DOT__unnamedblk2__DOT__lfsr_noise 
                                      >> 0x16U)) ^ 
                                    (vlSelfRef.__PVT__v1__DOT__unnamedblk2__DOT__lfsr_noise 
                                     >> 0x11U))));
                } else if (((vlSelfRef.__PVT__voice_ctrl[0U] 
                             >> 7U) & (0U != (7U & 
                                              (vlSelfRef.__PVT__voice_ctrl[0U] 
                                               >> 4U))))) {
                    __Vdly__v1__DOT__unnamedblk2__DOT__lfsr_noise 
                        = ((0x006fffffU & __Vdly__v1__DOT__unnamedblk2__DOT__lfsr_noise) 
                           | ((IData)(((vlSelfRef.__PVT__v1__DOT__unnamedblk2__DOT__lfsr_noise 
                                        >> 0x00000014U) 
                                       & ((IData)(vlSelfRef.__PVT__v1__DOT__wave_out) 
                                          >> 7U))) 
                              << 0x00000014U));
                    __Vdly__v1__DOT__unnamedblk2__DOT__lfsr_noise 
                        = ((0x007bffffU & __Vdly__v1__DOT__unnamedblk2__DOT__lfsr_noise) 
                           | (0x00040000U & (vlSelfRef.__PVT__v1__DOT__unnamedblk2__DOT__lfsr_noise 
                                             & ((IData)(vlSelfRef.__PVT__v1__DOT__wave_out) 
                                                << 0x0000000cU))));
                    __Vdly__v1__DOT__unnamedblk2__DOT__lfsr_noise 
                        = ((0x007fbfffU & __Vdly__v1__DOT__unnamedblk2__DOT__lfsr_noise) 
                           | (0x00004000U & (vlSelfRef.__PVT__v1__DOT__unnamedblk2__DOT__lfsr_noise 
                                             & ((IData)(vlSelfRef.__PVT__v1__DOT__wave_out) 
                                                << 9U))));
                    __Vdly__v1__DOT__unnamedblk2__DOT__lfsr_noise 
                        = ((0x007ff7ffU & __Vdly__v1__DOT__unnamedblk2__DOT__lfsr_noise) 
                           | (0x00000800U & (vlSelfRef.__PVT__v1__DOT__unnamedblk2__DOT__lfsr_noise 
                                             & ((IData)(vlSelfRef.__PVT__v1__DOT__wave_out) 
                                                << 7U))));
                    __Vdly__v1__DOT__unnamedblk2__DOT__lfsr_noise 
                        = ((0x007ffdffU & __Vdly__v1__DOT__unnamedblk2__DOT__lfsr_noise) 
                           | (0x00000200U & (vlSelfRef.__PVT__v1__DOT__unnamedblk2__DOT__lfsr_noise 
                                             & ((IData)(vlSelfRef.__PVT__v1__DOT__wave_out) 
                                                << 6U))));
                    __Vdly__v1__DOT__unnamedblk2__DOT__lfsr_noise 
                        = ((0x007fffdfU & __Vdly__v1__DOT__unnamedblk2__DOT__lfsr_noise) 
                           | (0x00000020U & (vlSelfRef.__PVT__v1__DOT__unnamedblk2__DOT__lfsr_noise 
                                             & ((IData)(vlSelfRef.__PVT__v1__DOT__wave_out) 
                                                << 3U))));
                    __Vdly__v1__DOT__unnamedblk2__DOT__lfsr_noise 
                        = ((0x007ffffbU & __Vdly__v1__DOT__unnamedblk2__DOT__lfsr_noise) 
                           | (4U & (vlSelfRef.__PVT__v1__DOT__unnamedblk2__DOT__lfsr_noise 
                                    & ((IData)(vlSelfRef.__PVT__v1__DOT__wave_out) 
                                       << 1U))));
                    __Vdly__v1__DOT__unnamedblk2__DOT__lfsr_noise 
                        = ((0x007ffffeU & __Vdly__v1__DOT__unnamedblk2__DOT__lfsr_noise) 
                           | (1U & (vlSelfRef.__PVT__v1__DOT__unnamedblk2__DOT__lfsr_noise 
                                    & (IData)(vlSelfRef.__PVT__v1__DOT__wave_out))));
                }
                __Vdly__v1__DOT__unnamedblk2__DOT__noise_age = 0U;
                vlSelfRef.__PVT__v1__DOT__unnamedblk2__DOT__clk_d = 1U;
            } else {
                if ((vlSelfRef.__PVT__v1__DOT__unnamedblk2__DOT__noise_age 
                     >= ((1U & (IData)(vlSymsp->TOP.top__DOT__sid_cfg_reg))
                          ? 0x00950000U : 0x00008000U))) {
                    vlSelfRef.__PVT__v1__DOT__noise = 0x0fffU;
                } else {
                    __Vdly__v1__DOT__unnamedblk2__DOT__noise_age 
                        = (0x00ffffffU & ((IData)(1U) 
                                          + vlSelfRef.__PVT__v1__DOT__unnamedblk2__DOT__noise_age));
                }
                vlSelfRef.__PVT__v1__DOT__unnamedblk2__DOT__clk_d = 0U;
            }
        }
    }
    if (vlSymsp->TOP.top__DOT__sid_ce_1m) {
        vlSelfRef.__PVT__v1__DOT__wave_out = (0x000000ffU 
                                              & (((IData)(vlSelfRef.__PVT__v1__DOT__norm) 
                                                  >> 4U) 
                                                 | (IData)(vlSelfRef.__PVT__v1__DOT__comb)));
    }
    if (vlSymsp->TOP.rst) {
        vlSelfRef.__PVT__v2__DOT__noise = 0U;
        __Vdly__v2__DOT__unnamedblk2__DOT__lfsr_noise = 0x007fffffU;
        __Vdly__v2__DOT__unnamedblk2__DOT__noise_age = 0U;
    } else if (vlSymsp->TOP.top__DOT__sid_ce_1m) {
        vlSelfRef.__PVT__v2__DOT__noise = (((((4U & 
                                               (vlSelfRef.__PVT__v2__DOT__unnamedblk2__DOT__lfsr_noise 
                                                >> 0x00000012U)) 
                                              | ((2U 
                                                  & (vlSelfRef.__PVT__v2__DOT__unnamedblk2__DOT__lfsr_noise 
                                                     >> 0x00000011U)) 
                                                 | (1U 
                                                    & (vlSelfRef.__PVT__v2__DOT__unnamedblk2__DOT__lfsr_noise 
                                                       >> 0x0eU)))) 
                                             << 9U) 
                                            | (((4U 
                                                 & (vlSelfRef.__PVT__v2__DOT__unnamedblk2__DOT__lfsr_noise 
                                                    >> 9U)) 
                                                | ((2U 
                                                    & (vlSelfRef.__PVT__v2__DOT__unnamedblk2__DOT__lfsr_noise 
                                                       >> 8U)) 
                                                   | (1U 
                                                      & (vlSelfRef.__PVT__v2__DOT__unnamedblk2__DOT__lfsr_noise 
                                                         >> 5U)))) 
                                               << 6U)) 
                                           | ((0x00000020U 
                                               & (vlSelfRef.__PVT__v2__DOT__unnamedblk2__DOT__lfsr_noise 
                                                  << 3U)) 
                                              | (0x00000010U 
                                                 & (vlSelfRef.__PVT__v2__DOT__unnamedblk2__DOT__lfsr_noise 
                                                    << 4U))));
        if (vlSelfRef.__PVT__v2__DOT__unnamedblk2__DOT__clk) {
            if (((IData)(vlSelfRef.__PVT__v2__DOT__unnamedblk2__DOT__clk) 
                 & (~ (IData)(vlSelfRef.__PVT__v2__DOT__unnamedblk2__DOT__clk_d)))) {
                __Vdly__v2__DOT__unnamedblk2__DOT__lfsr_noise 
                    = ((0x007ffffeU & (vlSelfRef.__PVT__v2__DOT__unnamedblk2__DOT__lfsr_noise 
                                       << 1U)) | (1U 
                                                  & ((((IData)(vlSymsp->TOP.rst) 
                                                       | (vlSelfRef.__PVT__voice_ctrl[1U] 
                                                          >> 3U)) 
                                                      | (vlSelfRef.__PVT__v2__DOT__unnamedblk2__DOT__lfsr_noise 
                                                         >> 0x16U)) 
                                                     ^ 
                                                     (vlSelfRef.__PVT__v2__DOT__unnamedblk2__DOT__lfsr_noise 
                                                      >> 0x11U))));
            } else if (((vlSelfRef.__PVT__voice_ctrl[1U] 
                         >> 7U) & (0U != (7U & (vlSelfRef.__PVT__voice_ctrl[1U] 
                                                >> 4U))))) {
                __Vdly__v2__DOT__unnamedblk2__DOT__lfsr_noise 
                    = ((0x006fffffU & __Vdly__v2__DOT__unnamedblk2__DOT__lfsr_noise) 
                       | ((IData)(((vlSelfRef.__PVT__v2__DOT__unnamedblk2__DOT__lfsr_noise 
                                    >> 0x00000014U) 
                                   & ((IData)(vlSelfRef.__PVT__v2__DOT__wave_out) 
                                      >> 7U))) << 0x00000014U));
                __Vdly__v2__DOT__unnamedblk2__DOT__lfsr_noise 
                    = ((0x007bffffU & __Vdly__v2__DOT__unnamedblk2__DOT__lfsr_noise) 
                       | (0x00040000U & (vlSelfRef.__PVT__v2__DOT__unnamedblk2__DOT__lfsr_noise 
                                         & ((IData)(vlSelfRef.__PVT__v2__DOT__wave_out) 
                                            << 0x0000000cU))));
                __Vdly__v2__DOT__unnamedblk2__DOT__lfsr_noise 
                    = ((0x007fbfffU & __Vdly__v2__DOT__unnamedblk2__DOT__lfsr_noise) 
                       | (0x00004000U & (vlSelfRef.__PVT__v2__DOT__unnamedblk2__DOT__lfsr_noise 
                                         & ((IData)(vlSelfRef.__PVT__v2__DOT__wave_out) 
                                            << 9U))));
                __Vdly__v2__DOT__unnamedblk2__DOT__lfsr_noise 
                    = ((0x007ff7ffU & __Vdly__v2__DOT__unnamedblk2__DOT__lfsr_noise) 
                       | (0x00000800U & (vlSelfRef.__PVT__v2__DOT__unnamedblk2__DOT__lfsr_noise 
                                         & ((IData)(vlSelfRef.__PVT__v2__DOT__wave_out) 
                                            << 7U))));
                __Vdly__v2__DOT__unnamedblk2__DOT__lfsr_noise 
                    = ((0x007ffdffU & __Vdly__v2__DOT__unnamedblk2__DOT__lfsr_noise) 
                       | (0x00000200U & (vlSelfRef.__PVT__v2__DOT__unnamedblk2__DOT__lfsr_noise 
                                         & ((IData)(vlSelfRef.__PVT__v2__DOT__wave_out) 
                                            << 6U))));
                __Vdly__v2__DOT__unnamedblk2__DOT__lfsr_noise 
                    = ((0x007fffdfU & __Vdly__v2__DOT__unnamedblk2__DOT__lfsr_noise) 
                       | (0x00000020U & (vlSelfRef.__PVT__v2__DOT__unnamedblk2__DOT__lfsr_noise 
                                         & ((IData)(vlSelfRef.__PVT__v2__DOT__wave_out) 
                                            << 3U))));
                __Vdly__v2__DOT__unnamedblk2__DOT__lfsr_noise 
                    = ((0x007ffffbU & __Vdly__v2__DOT__unnamedblk2__DOT__lfsr_noise) 
                       | (4U & (vlSelfRef.__PVT__v2__DOT__unnamedblk2__DOT__lfsr_noise 
                                & ((IData)(vlSelfRef.__PVT__v2__DOT__wave_out) 
                                   << 1U))));
                __Vdly__v2__DOT__unnamedblk2__DOT__lfsr_noise 
                    = ((0x007ffffeU & __Vdly__v2__DOT__unnamedblk2__DOT__lfsr_noise) 
                       | (1U & (vlSelfRef.__PVT__v2__DOT__unnamedblk2__DOT__lfsr_noise 
                                & (IData)(vlSelfRef.__PVT__v2__DOT__wave_out))));
            }
            __Vdly__v2__DOT__unnamedblk2__DOT__noise_age = 0U;
            vlSelfRef.__PVT__v2__DOT__unnamedblk2__DOT__clk_d = 1U;
        } else {
            if ((vlSelfRef.__PVT__v2__DOT__unnamedblk2__DOT__noise_age 
                 >= ((1U & (IData)(vlSymsp->TOP.top__DOT__sid_cfg_reg))
                      ? 0x00950000U : 0x00008000U))) {
                vlSelfRef.__PVT__v2__DOT__noise = 0x0fffU;
            } else {
                __Vdly__v2__DOT__unnamedblk2__DOT__noise_age 
                    = (0x00ffffffU & ((IData)(1U) + vlSelfRef.__PVT__v2__DOT__unnamedblk2__DOT__noise_age));
            }
            vlSelfRef.__PVT__v2__DOT__unnamedblk2__DOT__clk_d = 0U;
        }
    }
    if (vlSymsp->TOP.top__DOT__sid_ce_1m) {
        vlSelfRef.__PVT__v2__DOT__wave_out = (0x000000ffU 
                                              & (((IData)(vlSelfRef.__PVT__v2__DOT__norm) 
                                                  >> 4U) 
                                                 | (IData)(vlSelfRef.__PVT__v2__DOT__comb)));
    }
    if (vlSymsp->TOP.rst) {
        vlSelfRef.__PVT__v3__DOT__noise = 0U;
        __Vdly__v3__DOT__unnamedblk2__DOT__lfsr_noise = 0x007fffffU;
        __Vdly__v3__DOT__unnamedblk2__DOT__noise_age = 0U;
    } else if (vlSymsp->TOP.top__DOT__sid_ce_1m) {
        vlSelfRef.__PVT__v3__DOT__noise = (((((4U & 
                                               (vlSelfRef.__PVT__v3__DOT__unnamedblk2__DOT__lfsr_noise 
                                                >> 0x00000012U)) 
                                              | ((2U 
                                                  & (vlSelfRef.__PVT__v3__DOT__unnamedblk2__DOT__lfsr_noise 
                                                     >> 0x00000011U)) 
                                                 | (1U 
                                                    & (vlSelfRef.__PVT__v3__DOT__unnamedblk2__DOT__lfsr_noise 
                                                       >> 0x0eU)))) 
                                             << 9U) 
                                            | (((4U 
                                                 & (vlSelfRef.__PVT__v3__DOT__unnamedblk2__DOT__lfsr_noise 
                                                    >> 9U)) 
                                                | ((2U 
                                                    & (vlSelfRef.__PVT__v3__DOT__unnamedblk2__DOT__lfsr_noise 
                                                       >> 8U)) 
                                                   | (1U 
                                                      & (vlSelfRef.__PVT__v3__DOT__unnamedblk2__DOT__lfsr_noise 
                                                         >> 5U)))) 
                                               << 6U)) 
                                           | ((0x00000020U 
                                               & (vlSelfRef.__PVT__v3__DOT__unnamedblk2__DOT__lfsr_noise 
                                                  << 3U)) 
                                              | (0x00000010U 
                                                 & (vlSelfRef.__PVT__v3__DOT__unnamedblk2__DOT__lfsr_noise 
                                                    << 4U))));
        if (vlSelfRef.__PVT__v3__DOT__unnamedblk2__DOT__clk) {
            if (((IData)(vlSelfRef.__PVT__v3__DOT__unnamedblk2__DOT__clk) 
                 & (~ (IData)(vlSelfRef.__PVT__v3__DOT__unnamedblk2__DOT__clk_d)))) {
                __Vdly__v3__DOT__unnamedblk2__DOT__lfsr_noise 
                    = ((0x007ffffeU & (vlSelfRef.__PVT__v3__DOT__unnamedblk2__DOT__lfsr_noise 
                                       << 1U)) | (1U 
                                                  & ((((IData)(vlSymsp->TOP.rst) 
                                                       | (vlSelfRef.__PVT__voice_ctrl[2U] 
                                                          >> 3U)) 
                                                      | (vlSelfRef.__PVT__v3__DOT__unnamedblk2__DOT__lfsr_noise 
                                                         >> 0x16U)) 
                                                     ^ 
                                                     (vlSelfRef.__PVT__v3__DOT__unnamedblk2__DOT__lfsr_noise 
                                                      >> 0x11U))));
            } else if (((vlSelfRef.__PVT__voice_ctrl[2U] 
                         >> 7U) & (0U != (7U & (vlSelfRef.__PVT__voice_ctrl[2U] 
                                                >> 4U))))) {
                __Vdly__v3__DOT__unnamedblk2__DOT__lfsr_noise 
                    = ((0x006fffffU & __Vdly__v3__DOT__unnamedblk2__DOT__lfsr_noise) 
                       | ((IData)(((vlSelfRef.__PVT__v3__DOT__unnamedblk2__DOT__lfsr_noise 
                                    >> 0x00000014U) 
                                   & ((IData)(vlSelfRef.__PVT__v3__DOT__wave_out) 
                                      >> 7U))) << 0x00000014U));
                __Vdly__v3__DOT__unnamedblk2__DOT__lfsr_noise 
                    = ((0x007bffffU & __Vdly__v3__DOT__unnamedblk2__DOT__lfsr_noise) 
                       | (0x00040000U & (vlSelfRef.__PVT__v3__DOT__unnamedblk2__DOT__lfsr_noise 
                                         & ((IData)(vlSelfRef.__PVT__v3__DOT__wave_out) 
                                            << 0x0000000cU))));
                __Vdly__v3__DOT__unnamedblk2__DOT__lfsr_noise 
                    = ((0x007fbfffU & __Vdly__v3__DOT__unnamedblk2__DOT__lfsr_noise) 
                       | (0x00004000U & (vlSelfRef.__PVT__v3__DOT__unnamedblk2__DOT__lfsr_noise 
                                         & ((IData)(vlSelfRef.__PVT__v3__DOT__wave_out) 
                                            << 9U))));
                __Vdly__v3__DOT__unnamedblk2__DOT__lfsr_noise 
                    = ((0x007ff7ffU & __Vdly__v3__DOT__unnamedblk2__DOT__lfsr_noise) 
                       | (0x00000800U & (vlSelfRef.__PVT__v3__DOT__unnamedblk2__DOT__lfsr_noise 
                                         & ((IData)(vlSelfRef.__PVT__v3__DOT__wave_out) 
                                            << 7U))));
                __Vdly__v3__DOT__unnamedblk2__DOT__lfsr_noise 
                    = ((0x007ffdffU & __Vdly__v3__DOT__unnamedblk2__DOT__lfsr_noise) 
                       | (0x00000200U & (vlSelfRef.__PVT__v3__DOT__unnamedblk2__DOT__lfsr_noise 
                                         & ((IData)(vlSelfRef.__PVT__v3__DOT__wave_out) 
                                            << 6U))));
                __Vdly__v3__DOT__unnamedblk2__DOT__lfsr_noise 
                    = ((0x007fffdfU & __Vdly__v3__DOT__unnamedblk2__DOT__lfsr_noise) 
                       | (0x00000020U & (vlSelfRef.__PVT__v3__DOT__unnamedblk2__DOT__lfsr_noise 
                                         & ((IData)(vlSelfRef.__PVT__v3__DOT__wave_out) 
                                            << 3U))));
                __Vdly__v3__DOT__unnamedblk2__DOT__lfsr_noise 
                    = ((0x007ffffbU & __Vdly__v3__DOT__unnamedblk2__DOT__lfsr_noise) 
                       | (4U & (vlSelfRef.__PVT__v3__DOT__unnamedblk2__DOT__lfsr_noise 
                                & ((IData)(vlSelfRef.__PVT__v3__DOT__wave_out) 
                                   << 1U))));
                __Vdly__v3__DOT__unnamedblk2__DOT__lfsr_noise 
                    = ((0x007ffffeU & __Vdly__v3__DOT__unnamedblk2__DOT__lfsr_noise) 
                       | (1U & (vlSelfRef.__PVT__v3__DOT__unnamedblk2__DOT__lfsr_noise 
                                & (IData)(vlSelfRef.__PVT__v3__DOT__wave_out))));
            }
            __Vdly__v3__DOT__unnamedblk2__DOT__noise_age = 0U;
            vlSelfRef.__PVT__v3__DOT__unnamedblk2__DOT__clk_d = 1U;
        } else {
            if ((vlSelfRef.__PVT__v3__DOT__unnamedblk2__DOT__noise_age 
                 >= ((1U & (IData)(vlSymsp->TOP.top__DOT__sid_cfg_reg))
                      ? 0x00950000U : 0x00008000U))) {
                vlSelfRef.__PVT__v3__DOT__noise = 0x0fffU;
            } else {
                __Vdly__v3__DOT__unnamedblk2__DOT__noise_age 
                    = (0x00ffffffU & ((IData)(1U) + vlSelfRef.__PVT__v3__DOT__unnamedblk2__DOT__noise_age));
            }
            vlSelfRef.__PVT__v3__DOT__unnamedblk2__DOT__clk_d = 0U;
        }
    }
    if (vlSymsp->TOP.top__DOT__sid_ce_1m) {
        vlSelfRef.__PVT__v3__DOT__wave_out = (0x000000ffU 
                                              & (((IData)(vlSelfRef.__PVT__v3__DOT__norm) 
                                                  >> 4U) 
                                                 | (IData)(vlSelfRef.__PVT__v3__DOT__comb)));
        if ((1U & (IData)(vlSymsp->TOP.top__DOT__sid_cfg_reg))) {
            vlSelfRef.__PVT__v1__DOT__unnamedblk3__DOT__env_dac 
                = vlSelfRef.__PVT__v1__DOT__envelope;
            vlSelfRef.__PVT__v2__DOT__unnamedblk3__DOT__env_dac 
                = vlSelfRef.__PVT__v2__DOT__envelope;
            vlSelfRef.__PVT__v3__DOT__unnamedblk3__DOT__env_dac 
                = vlSelfRef.__PVT__v3__DOT__envelope;
        } else {
            vlSelfRef.__PVT__v1__DOT__unnamedblk3__DOT__env_dac 
                = (0x000000ffU & (vlSelfRef.__PVT__v1__DOT__envelope_dac__DOT__bitsum[7U] 
                                  >> 4U));
            vlSelfRef.__PVT__v2__DOT__unnamedblk3__DOT__env_dac 
                = (0x000000ffU & (vlSelfRef.__PVT__v2__DOT__envelope_dac__DOT__bitsum[7U] 
                                  >> 4U));
            vlSelfRef.__PVT__v3__DOT__unnamedblk3__DOT__env_dac 
                = (0x000000ffU & (vlSelfRef.__PVT__v3__DOT__envelope_dac__DOT__bitsum[7U] 
                                  >> 4U));
        }
        if ((0U != (0x0000000fU & (vlSelfRef.__PVT__voice_ctrl[0U] 
                                   >> 4U)))) {
            __Vdly__v1__DOT__unnamedblk3__DOT__keep_cnt 
                = ((1U & (IData)(vlSymsp->TOP.top__DOT__sid_cfg_reg))
                    ? 0x00030d40U : 0x004c4b40U);
            vlSelfRef.__PVT__v1__DOT__unnamedblk3__DOT__dac_out 
                = (0x00001fffU & (((IData)(vlSelfRef.__PVT__v1__DOT__norm_dac) 
                                   | ((IData)(vlSelfRef.__PVT__v1__DOT__comb) 
                                      << 4U)) - ((1U 
                                                  & (IData)(vlSymsp->TOP.top__DOT__sid_cfg_reg))
                                                  ? 0x0800U
                                                  : 0x0380U)));
        } else if ((0U != vlSelfRef.__PVT__v1__DOT__unnamedblk3__DOT__keep_cnt)) {
            __Vdly__v1__DOT__unnamedblk3__DOT__keep_cnt 
                = (0x00ffffffU & (vlSelfRef.__PVT__v1__DOT__unnamedblk3__DOT__keep_cnt 
                                  - (IData)(1U)));
        } else {
            vlSelfRef.__PVT__v1__DOT__unnamedblk3__DOT__dac_out = 0U;
        }
        if ((0U != (0x0000000fU & (vlSelfRef.__PVT__voice_ctrl[1U] 
                                   >> 4U)))) {
            __Vdly__v2__DOT__unnamedblk3__DOT__keep_cnt 
                = ((1U & (IData)(vlSymsp->TOP.top__DOT__sid_cfg_reg))
                    ? 0x00030d40U : 0x004c4b40U);
            vlSelfRef.__PVT__v2__DOT__unnamedblk3__DOT__dac_out 
                = (0x00001fffU & (((IData)(vlSelfRef.__PVT__v2__DOT__norm_dac) 
                                   | ((IData)(vlSelfRef.__PVT__v2__DOT__comb) 
                                      << 4U)) - ((1U 
                                                  & (IData)(vlSymsp->TOP.top__DOT__sid_cfg_reg))
                                                  ? 0x0800U
                                                  : 0x0380U)));
        } else if ((0U != vlSelfRef.__PVT__v2__DOT__unnamedblk3__DOT__keep_cnt)) {
            __Vdly__v2__DOT__unnamedblk3__DOT__keep_cnt 
                = (0x00ffffffU & (vlSelfRef.__PVT__v2__DOT__unnamedblk3__DOT__keep_cnt 
                                  - (IData)(1U)));
        } else {
            vlSelfRef.__PVT__v2__DOT__unnamedblk3__DOT__dac_out = 0U;
        }
        if ((0U != (0x0000000fU & (vlSelfRef.__PVT__voice_ctrl[2U] 
                                   >> 4U)))) {
            __Vdly__v3__DOT__unnamedblk3__DOT__keep_cnt 
                = ((1U & (IData)(vlSymsp->TOP.top__DOT__sid_cfg_reg))
                    ? 0x00030d40U : 0x004c4b40U);
            vlSelfRef.__PVT__v3__DOT__unnamedblk3__DOT__dac_out 
                = (0x00001fffU & (((IData)(vlSelfRef.__PVT__v3__DOT__norm_dac) 
                                   | ((IData)(vlSelfRef.__PVT__v3__DOT__comb) 
                                      << 4U)) - ((1U 
                                                  & (IData)(vlSymsp->TOP.top__DOT__sid_cfg_reg))
                                                  ? 0x0800U
                                                  : 0x0380U)));
        } else if ((0U != vlSelfRef.__PVT__v3__DOT__unnamedblk3__DOT__keep_cnt)) {
            __Vdly__v3__DOT__unnamedblk3__DOT__keep_cnt 
                = (0x00ffffffU & (vlSelfRef.__PVT__v3__DOT__unnamedblk3__DOT__keep_cnt 
                                  - (IData)(1U)));
        } else {
            vlSelfRef.__PVT__v3__DOT__unnamedblk3__DOT__dac_out = 0U;
        }
    }
    if ((((1U == (IData)(vlSelfRef.__PVT__tbl_state)) 
          || (3U == (IData)(vlSelfRef.__PVT__tbl_state))) 
         || (5U == (IData)(vlSelfRef.__PVT__tbl_state)))) {
        __Vdly__f_acc_t = ((2U >= (3U & ((IData)(vlSelfRef.__PVT__tbl_state) 
                                         >> 1U))) ? vlSelfRef.__PVT__acc_t
                           [(3U & ((IData)(vlSelfRef.__PVT__tbl_state) 
                                   >> 1U))] : 0U);
    }
    if ((((3U == (IData)(vlSelfRef.__PVT__tbl_state)) 
          || (5U == (IData)(vlSelfRef.__PVT__tbl_state))) 
         || (7U == (IData)(vlSelfRef.__PVT__tbl_state)))) {
        vlSelfRef.__Vlvbound_h9cc45699__0 = vlSelfRef.__PVT__f___05Fst_out;
        vlSelfRef.__Vlvbound_h49d001ad__0 = vlSelfRef.__PVT__f_p_t_out;
        vlSelfRef.__Vlvbound_h061f25e8__0 = vlSelfRef.__PVT__f_ps___05Fout;
        vlSelfRef.__Vlvbound_h0706378b__0 = vlSelfRef.__PVT__f_pst_out;
        if ((5U >= (7U & ((3U & ((IData)(vlSelfRef.__PVT__tbl_state) 
                                 >> 1U)) - (IData)(1U))))) {
            __VdlyVal___st_out__v0 = vlSelfRef.__Vlvbound_h9cc45699__0;
            __VdlyDim0___st_out__v0 = (7U & ((3U & 
                                              ((IData)(vlSelfRef.__PVT__tbl_state) 
                                               >> 1U)) 
                                             - (IData)(1U)));
            __VdlySet___st_out__v0 = 1U;
            __VdlyVal__p_t_out__v0 = vlSelfRef.__Vlvbound_h49d001ad__0;
            __VdlyDim0__p_t_out__v0 = (7U & ((3U & 
                                              ((IData)(vlSelfRef.__PVT__tbl_state) 
                                               >> 1U)) 
                                             - (IData)(1U)));
            __VdlySet__p_t_out__v0 = 1U;
            __VdlyVal__ps___05Fout__v0 = vlSelfRef.__Vlvbound_h061f25e8__0;
            __VdlyDim0__ps___05Fout__v0 = (7U & ((3U 
                                                  & ((IData)(vlSelfRef.__PVT__tbl_state) 
                                                     >> 1U)) 
                                                 - (IData)(1U)));
            __VdlySet__ps___05Fout__v0 = 1U;
            __VdlyVal__pst_out__v0 = vlSelfRef.__Vlvbound_h0706378b__0;
            __VdlyDim0__pst_out__v0 = (7U & ((3U & 
                                              ((IData)(vlSelfRef.__PVT__tbl_state) 
                                               >> 1U)) 
                                             - (IData)(1U)));
            __VdlySet__pst_out__v0 = 1U;
        }
    }
    if ((6U == (IData)(vlSelfRef.__PVT__tbl_state))) {
        vlSelfRef.audio_out = (0x0003ffffU & (vlSelfRef.__PVT__flt__DOT__o 
                                              >> 2U));
    }
    if ((2U == (7U & (IData)(vlSelfRef.__PVT__tbl_state)))) {
        vlSelfRef.__PVT__flt__DOT__s = 1U;
        vlSelfRef.__PVT__flt__DOT__c = 0U;
        vlSelfRef.__PVT__flt__DOT__b = vlSelfRef.__PVT__flt__DOT__vbp;
        vlSelfRef.__PVT__flt__DOT__a = (0x0000ffffU 
                                        & ((1U & (IData)(vlSymsp->TOP.top__DOT__sid_cfg_reg))
                                            ? (((IData)(vlSelfRef.__PVT__filter_fc) 
                                                << 2U) 
                                               + (IData)(vlSelfRef.__PVT__filter_fc))
                                            : VL_SHIFTR_III(16,16,32, (IData)(vlSelfRef.__PVT__tbl__DOT__f0), 1U)));
        vlSelfRef.__PVT__flt__DOT__unnamedblk1__DOT__vi 
            = (0x0000ffffU & ((((((1U & (IData)(vlSelfRef.__PVT__filter_res_filt))
                                   ? VL_EXTENDS_II(24,22, vlSelfRef.__PVT__v1_scaled)
                                   : 0U) + ((2U & (IData)(vlSelfRef.__PVT__filter_res_filt))
                                             ? VL_EXTENDS_II(24,22, vlSelfRef.__PVT__v2_scaled)
                                             : 0U)) 
                                + ((4U & (IData)(vlSelfRef.__PVT__filter_res_filt))
                                    ? VL_EXTENDS_II(24,22, vlSelfRef.__PVT__v3_scaled)
                                    : 0U)) + ((8U & (IData)(vlSelfRef.__PVT__filter_res_filt))
                                               ? 0U
                                               : 0U)) 
                              >> 7U));
        vlSelfRef.__PVT__flt__DOT__unnamedblk1__DOT__vd 
            = (0x0000ffffU & (((((((1U & (IData)(vlSymsp->TOP.top__DOT__sid_cfg_reg))
                                    ? 0U : 0x00ff1c72U) 
                                  + ((1U & (IData)(vlSelfRef.__PVT__filter_res_filt))
                                      ? 0U : VL_EXTENDS_II(24,22, vlSelfRef.__PVT__v1_scaled))) 
                                 + ((2U & (IData)(vlSelfRef.__PVT__filter_res_filt))
                                     ? 0U : VL_EXTENDS_II(24,22, vlSelfRef.__PVT__v2_scaled))) 
                                + ((1U & (((IData)(vlSelfRef.__PVT__filter_res_filt) 
                                           >> 2U) | 
                                          ((IData)(vlSelfRef.__PVT__filter_mode_vol) 
                                           >> 7U)))
                                    ? 0U : VL_EXTENDS_II(24,22, vlSelfRef.__PVT__v3_scaled))) 
                               + ((8U & (IData)(vlSelfRef.__PVT__filter_res_filt))
                                   ? 0U : 0U)) >> 7U));
        vlSelfRef.__PVT__flt__DOT__unnamedblk1__DOT___1_Q_lsl10 
            = vlSelfRef.__PVT__flt__DOT___1_Q_lsl10_tbl
            [((0x00000010U & ((IData)(vlSymsp->TOP.top__DOT__sid_cfg_reg) 
                              << 4U)) | (0x0000000fU 
                                         & ((IData)(vlSelfRef.__PVT__filter_res_filt) 
                                            >> 4U)))];
    } else {
        if ((3U == (7U & (IData)(vlSelfRef.__PVT__tbl_state)))) {
            vlSelfRef.__PVT__flt__DOT__s = 1U;
            vlSelfRef.__PVT__flt__DOT__vlp = vlSelfRef.__PVT__flt__DOT__vlp2;
            vlSelfRef.__PVT__flt__DOT__c = 0U;
            vlSelfRef.__PVT__flt__DOT__b = vlSelfRef.__PVT__flt__DOT__vhp;
        } else if ((4U == (7U & (IData)(vlSelfRef.__PVT__tbl_state)))) {
            vlSelfRef.__PVT__flt__DOT__s = 0U;
            vlSelfRef.__PVT__flt__DOT__vbp = vlSelfRef.__PVT__flt__DOT__vbp2;
            vlSelfRef.__PVT__flt__DOT__c = VL_SHIFTL_III(32,32,32, 
                                                         (- 
                                                          (VL_EXTENDS_II(32,16, (IData)(vlSelfRef.__PVT__flt__DOT__vlp2)) 
                                                           + 
                                                           VL_EXTENDS_II(32,16, (IData)(vlSelfRef.__PVT__flt__DOT__unnamedblk1__DOT__vi)))), 0x0000000aU);
            vlSelfRef.__PVT__flt__DOT__b = vlSelfRef.__PVT__flt__DOT__vbp_next;
        } else if ((5U == (7U & (IData)(vlSelfRef.__PVT__tbl_state)))) {
            vlSelfRef.__PVT__flt__DOT__s = 0U;
            __Vfunc_flt__DOT__clamp__2__x = (0x0001ffffU 
                                             & (((VL_EXTENDS_II(17,16, (IData)(vlSelfRef.__PVT__flt__DOT__unnamedblk1__DOT__vd)) 
                                                  + 
                                                  ((0x00000010U 
                                                    & (IData)(vlSelfRef.__PVT__filter_mode_vol))
                                                    ? 
                                                   VL_EXTENDS_II(17,16, (IData)(vlSelfRef.__PVT__flt__DOT__vlp2))
                                                    : 0U)) 
                                                 + 
                                                 ((0x00000020U 
                                                   & (IData)(vlSelfRef.__PVT__filter_mode_vol))
                                                   ? 
                                                  VL_EXTENDS_II(17,16, (IData)(vlSelfRef.__PVT__flt__DOT__vbp2))
                                                   : 0U)) 
                                                + (
                                                   (0x00000040U 
                                                    & (IData)(vlSelfRef.__PVT__filter_mode_vol))
                                                    ? 
                                                   VL_EXTENDS_II(17,16, (IData)(vlSelfRef.__PVT__flt__DOT__vhp_next))
                                                    : 0U)));
            __Vfunc_flt__DOT__clamp__2__Vfuncout = 
                (0x0000ffffU & ((1U & VL_REDXOR_32(
                                                   (3U 
                                                    & (__Vfunc_flt__DOT__clamp__2__x 
                                                       >> 0x0fU))))
                                 ? ((0x00008000U & 
                                     (__Vfunc_flt__DOT__clamp__2__x 
                                      >> 1U)) | (0x00007fffU 
                                                 & (- (IData)(
                                                              (1U 
                                                               & (__Vfunc_flt__DOT__clamp__2__x 
                                                                  >> 0x0fU))))))
                                 : __Vfunc_flt__DOT__clamp__2__x));
            vlSelfRef.__PVT__flt__DOT__c = 0U;
            vlSelfRef.__PVT__flt__DOT__b = __Vfunc_flt__DOT__clamp__2__Vfuncout;
        }
        if ((3U != (7U & (IData)(vlSelfRef.__PVT__tbl_state)))) {
            if ((4U == (7U & (IData)(vlSelfRef.__PVT__tbl_state)))) {
                vlSelfRef.__PVT__flt__DOT__a = vlSelfRef.__PVT__flt__DOT__unnamedblk1__DOT___1_Q_lsl10;
            } else if ((5U == (7U & (IData)(vlSelfRef.__PVT__tbl_state)))) {
                vlSelfRef.__PVT__flt__DOT__a = (0x0000000fU 
                                                & (IData)(vlSelfRef.__PVT__filter_mode_vol));
            }
        }
    }
    if (__VdlySet__voice_ad__v0) {
        vlSelfRef.__PVT__voice_ad[0U] = 0U;
        vlSelfRef.__PVT__voice_ad[1U] = 0U;
        vlSelfRef.__PVT__voice_ad[2U] = 0U;
    }
    if (__VdlySet__voice_ad__v3) {
        vlSelfRef.__PVT__voice_ad[0U] = __VdlyVal__voice_ad__v3;
    }
    if (__VdlySet__voice_ad__v4) {
        vlSelfRef.__PVT__voice_ad[1U] = __VdlyVal__voice_ad__v4;
    }
    if (__VdlySet__voice_ad__v5) {
        vlSelfRef.__PVT__voice_ad[2U] = __VdlyVal__voice_ad__v5;
    }
    if (__VdlySet__voice_vol__v0) {
        vlSelfRef.__PVT__voice_vol[0U] = 0x0fU;
        vlSelfRef.__PVT__voice_vol[1U] = 0x0fU;
        vlSelfRef.__PVT__voice_vol[2U] = 0x0fU;
    }
    if (__VdlySet__voice_vol__v3) {
        vlSelfRef.__PVT__voice_vol[0U] = __VdlyVal__voice_vol__v3;
    }
    if (__VdlySet__voice_vol__v4) {
        vlSelfRef.__PVT__voice_vol[1U] = __VdlyVal__voice_vol__v4;
    }
    if (__VdlySet__voice_vol__v5) {
        vlSelfRef.__PVT__voice_vol[2U] = __VdlyVal__voice_vol__v5;
    }
    vlSelfRef.__PVT__v1__DOT__adsr__DOT__unnamedblk1__DOT__exponential_counter_period 
        = __Vdly__v1__DOT__adsr__DOT__unnamedblk1__DOT__exponential_counter_period;
    vlSelfRef.__PVT__v1__DOT__adsr__DOT__unnamedblk1__DOT__hold_zero 
        = __Vdly__v1__DOT__adsr__DOT__unnamedblk1__DOT__hold_zero;
    vlSelfRef.__PVT__v1__DOT__adsr__DOT__unnamedblk1__DOT__exponential_counter 
        = __Vdly__v1__DOT__adsr__DOT__unnamedblk1__DOT__exponential_counter;
    vlSelfRef.__PVT__v1__DOT__adsr__DOT__unnamedblk1__DOT__rate_counter 
        = __Vdly__v1__DOT__adsr__DOT__unnamedblk1__DOT__rate_counter;
    vlSelfRef.__PVT__v1__DOT__adsr__DOT__state = __Vdly__v1__DOT__adsr__DOT__state;
    vlSelfRef.__PVT__v2__DOT__adsr__DOT__unnamedblk1__DOT__exponential_counter_period 
        = __Vdly__v2__DOT__adsr__DOT__unnamedblk1__DOT__exponential_counter_period;
    vlSelfRef.__PVT__v2__DOT__adsr__DOT__unnamedblk1__DOT__hold_zero 
        = __Vdly__v2__DOT__adsr__DOT__unnamedblk1__DOT__hold_zero;
    vlSelfRef.__PVT__v2__DOT__adsr__DOT__unnamedblk1__DOT__exponential_counter 
        = __Vdly__v2__DOT__adsr__DOT__unnamedblk1__DOT__exponential_counter;
    vlSelfRef.__PVT__v2__DOT__adsr__DOT__unnamedblk1__DOT__rate_counter 
        = __Vdly__v2__DOT__adsr__DOT__unnamedblk1__DOT__rate_counter;
    vlSelfRef.__PVT__v2__DOT__adsr__DOT__state = __Vdly__v2__DOT__adsr__DOT__state;
    vlSelfRef.__PVT__v3__DOT__adsr__DOT__unnamedblk1__DOT__exponential_counter_period 
        = __Vdly__v3__DOT__adsr__DOT__unnamedblk1__DOT__exponential_counter_period;
    vlSelfRef.__PVT__v3__DOT__adsr__DOT__unnamedblk1__DOT__hold_zero 
        = __Vdly__v3__DOT__adsr__DOT__unnamedblk1__DOT__hold_zero;
    vlSelfRef.__PVT__v3__DOT__adsr__DOT__unnamedblk1__DOT__exponential_counter 
        = __Vdly__v3__DOT__adsr__DOT__unnamedblk1__DOT__exponential_counter;
    vlSelfRef.__PVT__v3__DOT__adsr__DOT__unnamedblk1__DOT__rate_counter 
        = __Vdly__v3__DOT__adsr__DOT__unnamedblk1__DOT__rate_counter;
    vlSelfRef.__PVT__v3__DOT__adsr__DOT__state = __Vdly__v3__DOT__adsr__DOT__state;
    if (__VdlySet__voice_sr__v0) {
        vlSelfRef.__PVT__voice_sr[0U] = 0U;
        vlSelfRef.__PVT__voice_sr[1U] = 0U;
        vlSelfRef.__PVT__voice_sr[2U] = 0U;
    }
    if (__VdlySet__voice_sr__v3) {
        vlSelfRef.__PVT__voice_sr[0U] = __VdlyVal__voice_sr__v3;
    }
    if (__VdlySet__voice_sr__v4) {
        vlSelfRef.__PVT__voice_sr[1U] = __VdlyVal__voice_sr__v4;
    }
    if (__VdlySet__voice_sr__v5) {
        vlSelfRef.__PVT__voice_sr[2U] = __VdlyVal__voice_sr__v5;
    }
    if (__VdlySet__voice_freq__v0) {
        vlSelfRef.__PVT__voice_freq[0U] = 0U;
        vlSelfRef.__PVT__voice_freq[1U] = 0U;
        vlSelfRef.__PVT__voice_freq[2U] = 0U;
    }
    if (__VdlySet__voice_freq__v3) {
        vlSelfRef.__PVT__voice_freq[0U] = ((0xff00U 
                                            & vlSelfRef.__PVT__voice_freq[0U]) 
                                           | (IData)(__VdlyVal__voice_freq__v3));
    }
    if (__VdlySet__voice_freq__v4) {
        vlSelfRef.__PVT__voice_freq[0U] = ((0x00ffU 
                                            & vlSelfRef.__PVT__voice_freq[0U]) 
                                           | ((IData)(__VdlyVal__voice_freq__v4) 
                                              << 8U));
    }
    if (__VdlySet__voice_freq__v5) {
        vlSelfRef.__PVT__voice_freq[1U] = ((0xff00U 
                                            & vlSelfRef.__PVT__voice_freq[1U]) 
                                           | (IData)(__VdlyVal__voice_freq__v5));
    }
    if (__VdlySet__voice_freq__v6) {
        vlSelfRef.__PVT__voice_freq[1U] = ((0x00ffU 
                                            & vlSelfRef.__PVT__voice_freq[1U]) 
                                           | ((IData)(__VdlyVal__voice_freq__v6) 
                                              << 8U));
    }
    if (__VdlySet__voice_freq__v7) {
        vlSelfRef.__PVT__voice_freq[2U] = ((0xff00U 
                                            & vlSelfRef.__PVT__voice_freq[2U]) 
                                           | (IData)(__VdlyVal__voice_freq__v7));
    }
    if (__VdlySet__voice_freq__v8) {
        vlSelfRef.__PVT__voice_freq[2U] = ((0x00ffU 
                                            & vlSelfRef.__PVT__voice_freq[2U]) 
                                           | ((IData)(__VdlyVal__voice_freq__v8) 
                                              << 8U));
    }
    if (__VdlySet___st_out__v0) {
        vlSelfRef.__PVT___st_out[__VdlyDim0___st_out__v0] 
            = __VdlyVal___st_out__v0;
    }
    if (__VdlySet__p_t_out__v0) {
        vlSelfRef.__PVT__p_t_out[__VdlyDim0__p_t_out__v0] 
            = __VdlyVal__p_t_out__v0;
    }
    if (__VdlySet__ps___05Fout__v0) {
        vlSelfRef.__PVT__ps___05Fout[__VdlyDim0__ps___05Fout__v0] 
            = __VdlyVal__ps___05Fout__v0;
    }
    if (__VdlySet__pst_out__v0) {
        vlSelfRef.__PVT__pst_out[__VdlyDim0__pst_out__v0] 
            = __VdlyVal__pst_out__v0;
    }
    if (__VdlySet__voice_pw__v0) {
        vlSelfRef.__PVT__voice_pw[0U] = 0U;
        vlSelfRef.__PVT__voice_pw[1U] = 0U;
        vlSelfRef.__PVT__voice_pw[2U] = 0U;
    }
    if (__VdlySet__voice_pw__v3) {
        vlSelfRef.__PVT__voice_pw[0U] = ((0x0f00U & vlSelfRef.__PVT__voice_pw[0U]) 
                                         | (IData)(__VdlyVal__voice_pw__v3));
    }
    if (__VdlySet__voice_pw__v4) {
        vlSelfRef.__PVT__voice_pw[0U] = ((0x00ffU & vlSelfRef.__PVT__voice_pw[0U]) 
                                         | ((IData)(__VdlyVal__voice_pw__v4) 
                                            << 8U));
    }
    if (__VdlySet__voice_pw__v5) {
        vlSelfRef.__PVT__voice_pw[1U] = ((0x0f00U & vlSelfRef.__PVT__voice_pw[1U]) 
                                         | (IData)(__VdlyVal__voice_pw__v5));
    }
    if (__VdlySet__voice_pw__v6) {
        vlSelfRef.__PVT__voice_pw[1U] = ((0x00ffU & vlSelfRef.__PVT__voice_pw[1U]) 
                                         | ((IData)(__VdlyVal__voice_pw__v6) 
                                            << 8U));
    }
    if (__VdlySet__voice_pw__v7) {
        vlSelfRef.__PVT__voice_pw[2U] = ((0x0f00U & vlSelfRef.__PVT__voice_pw[2U]) 
                                         | (IData)(__VdlyVal__voice_pw__v7));
    }
    if (__VdlySet__voice_pw__v8) {
        vlSelfRef.__PVT__voice_pw[2U] = ((0x00ffU & vlSelfRef.__PVT__voice_pw[2U]) 
                                         | ((IData)(__VdlyVal__voice_pw__v8) 
                                            << 8U));
    }
    vlSelfRef.__PVT__v1__DOT__unnamedblk2__DOT__lfsr_noise 
        = __Vdly__v1__DOT__unnamedblk2__DOT__lfsr_noise;
    vlSelfRef.__PVT__v1__DOT__unnamedblk2__DOT__noise_age 
        = __Vdly__v1__DOT__unnamedblk2__DOT__noise_age;
    vlSelfRef.__PVT__v2__DOT__unnamedblk2__DOT__lfsr_noise 
        = __Vdly__v2__DOT__unnamedblk2__DOT__lfsr_noise;
    vlSelfRef.__PVT__v2__DOT__unnamedblk2__DOT__noise_age 
        = __Vdly__v2__DOT__unnamedblk2__DOT__noise_age;
    vlSelfRef.__PVT__v3__DOT__unnamedblk2__DOT__lfsr_noise 
        = __Vdly__v3__DOT__unnamedblk2__DOT__lfsr_noise;
    vlSelfRef.__PVT__v3__DOT__unnamedblk2__DOT__noise_age 
        = __Vdly__v3__DOT__unnamedblk2__DOT__noise_age;
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
    if ((1U & (IData)(vlSymsp->TOP.top__DOT__sid_cfg_reg))) {
        vlSelfRef.__PVT__f_p_t_out = vlSelfRef.__PVT__tbl__DOT__wave8580_p_t
            [(0x000007ffU & (IData)(vlSelfRef.__PVT__f_acc_t))];
        vlSelfRef.__PVT__f_ps___05Fout = vlSelfRef.__PVT__tbl__DOT__wave8580_ps_
            [vlSelfRef.__PVT__f_acc_t];
        vlSelfRef.__PVT__v1__DOT__norm_dac = (0x00000fffU 
                                              & (IData)(vlSelfRef.__PVT__v1__DOT__norm));
        vlSelfRef.__PVT__v2__DOT__norm_dac = (0x00000fffU 
                                              & (IData)(vlSelfRef.__PVT__v2__DOT__norm));
        vlSelfRef.__PVT__v3__DOT__norm_dac = (0x00000fffU 
                                              & (IData)(vlSelfRef.__PVT__v3__DOT__norm));
    } else {
        vlSelfRef.__PVT__f_p_t_out = vlSelfRef.__PVT__tbl__DOT__wave6581_p_t
            [(0x000007ffU & (IData)(vlSelfRef.__PVT__f_acc_t))];
        vlSelfRef.__PVT__f_ps___05Fout = vlSelfRef.__PVT__tbl__DOT__wave6581_ps_
            [(0x000007ffU & (IData)(vlSelfRef.__PVT__f_acc_t))];
        vlSelfRef.__PVT__v1__DOT__norm_dac = (0x00000fffU 
                                              & (vlSelfRef.__PVT__v1__DOT__waveform_dac__DOT__bitsum[11U] 
                                                 >> 4U));
        vlSelfRef.__PVT__v2__DOT__norm_dac = (0x00000fffU 
                                              & (vlSelfRef.__PVT__v2__DOT__waveform_dac__DOT__bitsum[11U] 
                                                 >> 4U));
        vlSelfRef.__PVT__v3__DOT__norm_dac = (0x00000fffU 
                                              & (vlSelfRef.__PVT__v3__DOT__waveform_dac__DOT__bitsum[11U] 
                                                 >> 4U));
    }
    __Vtableidx1 = (((IData)(vlSelfRef.__PVT__f_acc_t) 
                     << 1U) | (1U & (IData)(vlSymsp->TOP.top__DOT__sid_cfg_reg)));
    vlSelfRef.__PVT__f___05Fst_out = Vtop__ConstPool__TABLE_h5c0f3167_0
        [__Vtableidx1];
    __Vtableidx2 = (((IData)(vlSelfRef.__PVT__f_acc_t) 
                     << 1U) | (1U & (IData)(vlSymsp->TOP.top__DOT__sid_cfg_reg)));
    vlSelfRef.__PVT__f_pst_out = Vtop__ConstPool__TABLE_h8a1656db_0
        [__Vtableidx2];
    if ((2U != (7U & (IData)(vlSelfRef.__PVT__tbl_state)))) {
        if ((3U == (7U & (IData)(vlSelfRef.__PVT__tbl_state)))) {
            vlSelfRef.__PVT__flt__DOT__vlp2 = (0x0000ffffU 
                                               & ((1U 
                                                   & VL_REDXOR_32(
                                                                  (3U 
                                                                   & (vlSelfRef.__VdfgRegularize_hebeb780c_0_29 
                                                                      >> 0x0000000fU))))
                                                   ? 
                                                  ((0x00008000U 
                                                    & (vlSelfRef.__VdfgRegularize_hebeb780c_0_29 
                                                       >> 1U)) 
                                                   | (0x00007fffU 
                                                      & (- (IData)(
                                                                   (1U 
                                                                    & (vlSelfRef.__VdfgRegularize_hebeb780c_0_29 
                                                                       >> 0x0000000fU))))))
                                                   : vlSelfRef.__VdfgRegularize_hebeb780c_0_29));
        }
        if ((3U != (7U & (IData)(vlSelfRef.__PVT__tbl_state)))) {
            if ((4U == (7U & (IData)(vlSelfRef.__PVT__tbl_state)))) {
                vlSelfRef.__PVT__flt__DOT__vbp2 = vlSelfRef.__PVT__flt__DOT__vbp_next;
            }
            if ((4U != (7U & (IData)(vlSelfRef.__PVT__tbl_state)))) {
                if ((5U == (7U & (IData)(vlSelfRef.__PVT__tbl_state)))) {
                    vlSelfRef.__PVT__flt__DOT__vhp 
                        = vlSelfRef.__PVT__flt__DOT__vhp2;
                    vlSelfRef.__PVT__flt__DOT__vhp2 
                        = vlSelfRef.__PVT__flt__DOT__vhp_next;
                }
            }
        }
    }
    __PVT__flt__DOT__m = VL_MULS_III(32, VL_EXTENDS_II(32,16, (IData)(vlSelfRef.__PVT__flt__DOT__a)), 
                                     VL_EXTENDS_II(32,16, (IData)(vlSelfRef.__PVT__flt__DOT__b)));
    vlSelfRef.__PVT__tbl__DOT__f0 = vlSelfRef.__PVT__tbl__DOT__f6581_curve
        [(0x000003ffU & ((IData)(vlSelfRef.__PVT__filter_fc) 
                         >> 1U))];
    if ((1U & (~ (IData)(vlSymsp->TOP.rst)))) {
        if (vlSymsp->TOP.top__DOT__sid_ce_1m) {
            vlSelfRef.__PVT__v1__DOT__unnamedblk2__DOT__clk 
                = (1U & (~ (((IData)(vlSymsp->TOP.rst) 
                             | (vlSelfRef.__PVT__voice_ctrl[0U] 
                                >> 3U)) | ((~ (IData)(vlSelfRef.__PVT__v1__DOT__unnamedblk2__DOT__osc_edge)) 
                                           & (vlSelfRef.__PVT__v1__DOT__oscillator 
                                              >> 0x13U)))));
            vlSelfRef.__PVT__v2__DOT__unnamedblk2__DOT__clk 
                = (1U & (~ (((IData)(vlSymsp->TOP.rst) 
                             | (vlSelfRef.__PVT__voice_ctrl[1U] 
                                >> 3U)) | ((~ (IData)(vlSelfRef.__PVT__v2__DOT__unnamedblk2__DOT__osc_edge)) 
                                           & (vlSelfRef.__PVT__v2__DOT__oscillator 
                                              >> 0x13U)))));
            vlSelfRef.__PVT__v3__DOT__unnamedblk2__DOT__clk 
                = (1U & (~ (((IData)(vlSymsp->TOP.rst) 
                             | (vlSelfRef.__PVT__voice_ctrl[2U] 
                                >> 3U)) | ((~ (IData)(vlSelfRef.__PVT__v3__DOT__unnamedblk2__DOT__osc_edge)) 
                                           & (vlSelfRef.__PVT__v3__DOT__oscillator 
                                              >> 0x13U)))));
        }
    }
    if (vlSymsp->TOP.rst) {
        vlSelfRef.__PVT__v1__DOT__adsr__DOT__unnamedblk1__DOT__gate_edge 
            = (1U & vlSelfRef.__PVT__voice_ctrl[0U]);
        vlSelfRef.__PVT__v2__DOT__adsr__DOT__unnamedblk1__DOT__gate_edge 
            = (1U & vlSelfRef.__PVT__voice_ctrl[1U]);
        vlSelfRef.__PVT__v3__DOT__adsr__DOT__unnamedblk1__DOT__gate_edge 
            = (1U & vlSelfRef.__PVT__voice_ctrl[2U]);
        vlSelfRef.__PVT__filter_mode_vol = 0U;
        vlSelfRef.__PVT__filter_fc = 0U;
        vlSelfRef.__PVT__filter_res_filt = 0U;
        vlSelfRef.__PVT__v1__DOT__unnamedblk2__DOT__osc_edge = 0U;
        vlSelfRef.__PVT__v2__DOT__unnamedblk2__DOT__osc_edge = 0U;
        vlSelfRef.__PVT__v3__DOT__unnamedblk2__DOT__osc_edge = 0U;
    } else {
        if (vlSymsp->TOP.top__DOT__sid_ce_1m) {
            vlSelfRef.__PVT__v1__DOT__adsr__DOT__unnamedblk1__DOT__gate_edge 
                = (1U & vlSelfRef.__PVT__voice_ctrl[0U]);
            vlSelfRef.__PVT__v2__DOT__adsr__DOT__unnamedblk1__DOT__gate_edge 
                = (1U & vlSelfRef.__PVT__voice_ctrl[1U]);
            vlSelfRef.__PVT__v3__DOT__adsr__DOT__unnamedblk1__DOT__gate_edge 
                = (1U & vlSelfRef.__PVT__voice_ctrl[2U]);
            vlSelfRef.__PVT__v1__DOT__unnamedblk2__DOT__osc_edge 
                = (1U & (vlSelfRef.__PVT__v1__DOT__oscillator 
                         >> 0x13U));
            vlSelfRef.__PVT__v2__DOT__unnamedblk2__DOT__osc_edge 
                = (1U & (vlSelfRef.__PVT__v2__DOT__oscillator 
                         >> 0x13U));
            vlSelfRef.__PVT__v3__DOT__unnamedblk2__DOT__osc_edge 
                = (1U & (vlSelfRef.__PVT__v3__DOT__oscillator 
                         >> 0x13U));
        }
        if (vlSelfRef.cs) {
            if ((1U & (~ ((((((((0U == (0x0000001fU 
                                        & (IData)(vlSymsp->TOP.top__DOT__cpu_addr))) 
                                | (1U == (0x0000001fU 
                                          & (IData)(vlSymsp->TOP.top__DOT__cpu_addr)))) 
                               | (2U == (0x0000001fU 
                                         & (IData)(vlSymsp->TOP.top__DOT__cpu_addr)))) 
                              | (3U == (0x0000001fU 
                                        & (IData)(vlSymsp->TOP.top__DOT__cpu_addr)))) 
                             | (4U == (0x0000001fU 
                                       & (IData)(vlSymsp->TOP.top__DOT__cpu_addr)))) 
                            | (5U == (0x0000001fU & (IData)(vlSymsp->TOP.top__DOT__cpu_addr)))) 
                           | (6U == (0x0000001fU & (IData)(vlSymsp->TOP.top__DOT__cpu_addr)))) 
                          | (7U == (0x0000001fU & (IData)(vlSymsp->TOP.top__DOT__cpu_addr))))))) {
                if ((1U & (~ ((((((((8U == (0x0000001fU 
                                            & (IData)(vlSymsp->TOP.top__DOT__cpu_addr))) 
                                    | (9U == (0x0000001fU 
                                              & (IData)(vlSymsp->TOP.top__DOT__cpu_addr)))) 
                                   | (0x0aU == (0x0000001fU 
                                                & (IData)(vlSymsp->TOP.top__DOT__cpu_addr)))) 
                                  | (0x0bU == (0x0000001fU 
                                               & (IData)(vlSymsp->TOP.top__DOT__cpu_addr)))) 
                                 | (0x0cU == (0x0000001fU 
                                              & (IData)(vlSymsp->TOP.top__DOT__cpu_addr)))) 
                                | (0x0dU == (0x0000001fU 
                                             & (IData)(vlSymsp->TOP.top__DOT__cpu_addr)))) 
                               | (0x0eU == (0x0000001fU 
                                            & (IData)(vlSymsp->TOP.top__DOT__cpu_addr)))) 
                              | (0x0fU == (0x0000001fU 
                                           & (IData)(vlSymsp->TOP.top__DOT__cpu_addr))))))) {
                    if ((1U & (~ ((((((((0x10U == (0x0000001fU 
                                                   & (IData)(vlSymsp->TOP.top__DOT__cpu_addr))) 
                                        | (0x11U == 
                                           (0x0000001fU 
                                            & (IData)(vlSymsp->TOP.top__DOT__cpu_addr)))) 
                                       | (0x12U == 
                                          (0x0000001fU 
                                           & (IData)(vlSymsp->TOP.top__DOT__cpu_addr)))) 
                                      | (0x13U == (0x0000001fU 
                                                   & (IData)(vlSymsp->TOP.top__DOT__cpu_addr)))) 
                                     | (0x14U == (0x0000001fU 
                                                  & (IData)(vlSymsp->TOP.top__DOT__cpu_addr)))) 
                                    | (0x15U == (0x0000001fU 
                                                 & (IData)(vlSymsp->TOP.top__DOT__cpu_addr)))) 
                                   | (0x16U == (0x0000001fU 
                                                & (IData)(vlSymsp->TOP.top__DOT__cpu_addr)))) 
                                  | (0x17U == (0x0000001fU 
                                               & (IData)(vlSymsp->TOP.top__DOT__cpu_addr))))))) {
                        if ((0x18U == (0x0000001fU 
                                       & (IData)(vlSymsp->TOP.top__DOT__cpu_addr)))) {
                            vlSelfRef.__PVT__filter_mode_vol 
                                = vlSymsp->TOP.top__DOT__cpu_dout;
                        }
                    }
                    if (((((((((0x10U == (0x0000001fU 
                                          & (IData)(vlSymsp->TOP.top__DOT__cpu_addr))) 
                               | (0x11U == (0x0000001fU 
                                            & (IData)(vlSymsp->TOP.top__DOT__cpu_addr)))) 
                              | (0x12U == (0x0000001fU 
                                           & (IData)(vlSymsp->TOP.top__DOT__cpu_addr)))) 
                             | (0x13U == (0x0000001fU 
                                          & (IData)(vlSymsp->TOP.top__DOT__cpu_addr)))) 
                            | (0x14U == (0x0000001fU 
                                         & (IData)(vlSymsp->TOP.top__DOT__cpu_addr)))) 
                           | (0x15U == (0x0000001fU 
                                        & (IData)(vlSymsp->TOP.top__DOT__cpu_addr)))) 
                          | (0x16U == (0x0000001fU 
                                       & (IData)(vlSymsp->TOP.top__DOT__cpu_addr)))) 
                         | (0x17U == (0x0000001fU & (IData)(vlSymsp->TOP.top__DOT__cpu_addr))))) {
                        if ((0x10U != (0x0000001fU 
                                       & (IData)(vlSymsp->TOP.top__DOT__cpu_addr)))) {
                            if ((0x11U != (0x0000001fU 
                                           & (IData)(vlSymsp->TOP.top__DOT__cpu_addr)))) {
                                if ((0x12U != (0x0000001fU 
                                               & (IData)(vlSymsp->TOP.top__DOT__cpu_addr)))) {
                                    if ((0x13U != (0x0000001fU 
                                                   & (IData)(vlSymsp->TOP.top__DOT__cpu_addr)))) {
                                        if ((0x14U 
                                             != (0x0000001fU 
                                                 & (IData)(vlSymsp->TOP.top__DOT__cpu_addr)))) {
                                            if ((0x15U 
                                                 == 
                                                 (0x0000001fU 
                                                  & (IData)(vlSymsp->TOP.top__DOT__cpu_addr)))) {
                                                vlSelfRef.__PVT__filter_fc 
                                                    = 
                                                    ((0x07f8U 
                                                      & (IData)(vlSelfRef.__PVT__filter_fc)) 
                                                     | (7U 
                                                        & (IData)(vlSymsp->TOP.top__DOT__cpu_dout)));
                                            } else if (
                                                       (0x16U 
                                                        == 
                                                        (0x0000001fU 
                                                         & (IData)(vlSymsp->TOP.top__DOT__cpu_addr)))) {
                                                vlSelfRef.__PVT__filter_fc 
                                                    = 
                                                    ((7U 
                                                      & (IData)(vlSelfRef.__PVT__filter_fc)) 
                                                     | ((IData)(vlSymsp->TOP.top__DOT__cpu_dout) 
                                                        << 3U));
                                            }
                                            if ((0x15U 
                                                 != 
                                                 (0x0000001fU 
                                                  & (IData)(vlSymsp->TOP.top__DOT__cpu_addr)))) {
                                                if (
                                                    (0x16U 
                                                     != 
                                                     (0x0000001fU 
                                                      & (IData)(vlSymsp->TOP.top__DOT__cpu_addr)))) {
                                                    vlSelfRef.__PVT__filter_res_filt 
                                                        = vlSymsp->TOP.top__DOT__cpu_dout;
                                                }
                                            }
                                        }
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }
    }
    vlSelfRef.__PVT__f_acc_t = __Vdly__f_acc_t;
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
    vlSelfRef.__PVT__flt__DOT__o = ((IData)(vlSelfRef.__PVT__flt__DOT__s)
                                     ? (vlSelfRef.__PVT__flt__DOT__c 
                                        - __PVT__flt__DOT__m)
                                     : (vlSelfRef.__PVT__flt__DOT__c 
                                        + __PVT__flt__DOT__m));
    vlSelfRef.__PVT__v1__DOT__envelope = __Vdly__v1__DOT__envelope;
    vlSelfRef.__PVT__v1__DOT__unnamedblk3__DOT__keep_cnt 
        = __Vdly__v1__DOT__unnamedblk3__DOT__keep_cnt;
    vlSelfRef.__PVT__v2__DOT__envelope = __Vdly__v2__DOT__envelope;
    vlSelfRef.__PVT__v2__DOT__unnamedblk3__DOT__keep_cnt 
        = __Vdly__v2__DOT__unnamedblk3__DOT__keep_cnt;
    vlSelfRef.__PVT__v3__DOT__envelope = __Vdly__v3__DOT__envelope;
    vlSelfRef.__PVT__v3__DOT__unnamedblk3__DOT__keep_cnt 
        = __Vdly__v3__DOT__unnamedblk3__DOT__keep_cnt;
    if (__VdlySet__voice_ctrl__v0) {
        vlSelfRef.__PVT__voice_ctrl[0U] = 0U;
        vlSelfRef.__PVT__voice_ctrl[1U] = 0U;
        vlSelfRef.__PVT__voice_ctrl[2U] = 0U;
    }
    if (__VdlySet__voice_ctrl__v3) {
        vlSelfRef.__PVT__voice_ctrl[0U] = __VdlyVal__voice_ctrl__v3;
    }
    if (__VdlySet__voice_ctrl__v4) {
        vlSelfRef.__PVT__voice_ctrl[1U] = __VdlyVal__voice_ctrl__v4;
    }
    if (__VdlySet__voice_ctrl__v5) {
        vlSelfRef.__PVT__voice_ctrl[2U] = __VdlyVal__voice_ctrl__v5;
    }
    vlSelfRef.__VdfgRegularize_hebeb780c_0_29 = (0x0001ffffU 
                                                 & (VL_EXTENDS_II(17,16, (IData)(vlSelfRef.__PVT__flt__DOT__vlp)) 
                                                    + 
                                                    VL_SHIFTRS_III(17,32,32, vlSelfRef.__PVT__flt__DOT__o, 0x00000011U)));
    __VdfgRegularize_hebeb780c_0_30 = (0x0001ffffU 
                                       & (VL_EXTENDS_II(17,16, (IData)(vlSelfRef.__PVT__flt__DOT__vbp)) 
                                          + VL_SHIFTRS_III(17,32,32, vlSelfRef.__PVT__flt__DOT__o, 0x00000011U)));
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
    vlSelfRef.__PVT__tbl_state = __Vdly__tbl_state;
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
    vlSelfRef.__PVT__v1__DOT__oscillator = __Vdly__v1__DOT__oscillator;
    vlSelfRef.__PVT__v2__DOT__oscillator = __Vdly__v2__DOT__oscillator;
    vlSelfRef.__PVT__v3__DOT__oscillator = __Vdly__v3__DOT__oscillator;
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
    vlSelfRef.__PVT__acc_t[1U] = vlSelfRef.__Vcellout__v2__acc_t;
    vlSelfRef.__PVT__acc_t[0U] = vlSelfRef.__Vcellout__v1__acc_t;
    vlSelfRef.__PVT__acc_t[2U] = vlSelfRef.__Vcellout__v3__acc_t;
}

void Vtop_sid_chip___ico_sequent__TOP__top__DOT__sid2_inst__0(Vtop_sid_chip* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+        Vtop_sid_chip___ico_sequent__TOP__top__DOT__sid2_inst__0\n"); );
    Vtop__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
    // Body
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
}

void Vtop_sid_chip___nba_sequent__TOP__top__DOT__sid2_inst__0(Vtop_sid_chip* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+        Vtop_sid_chip___nba_sequent__TOP__top__DOT__sid2_inst__0\n"); );
    Vtop__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
    // Locals
    IData/*31:0*/ __PVT__flt__DOT__m;
    __PVT__flt__DOT__m = 0;
    SData/*15:0*/ __Vfunc_flt__DOT__clamp__6__Vfuncout;
    __Vfunc_flt__DOT__clamp__6__Vfuncout = 0;
    IData/*16:0*/ __Vfunc_flt__DOT__clamp__6__x;
    __Vfunc_flt__DOT__clamp__6__x = 0;
    SData/*12:0*/ __Vtableidx3;
    __Vtableidx3 = 0;
    SData/*12:0*/ __Vtableidx4;
    __Vtableidx4 = 0;
    IData/*16:0*/ __VdfgRegularize_hebeb780c_0_35;
    __VdfgRegularize_hebeb780c_0_35 = 0;
    CData/*3:0*/ __Vdly__tbl_state;
    __Vdly__tbl_state = 0;
    SData/*11:0*/ __Vdly__f_acc_t;
    __Vdly__f_acc_t = 0;
    IData/*23:0*/ __Vdly__v1__DOT__oscillator;
    __Vdly__v1__DOT__oscillator = 0;
    IData/*22:0*/ __Vdly__v1__DOT__unnamedblk2__DOT__lfsr_noise;
    __Vdly__v1__DOT__unnamedblk2__DOT__lfsr_noise = 0;
    IData/*23:0*/ __Vdly__v1__DOT__unnamedblk2__DOT__noise_age;
    __Vdly__v1__DOT__unnamedblk2__DOT__noise_age = 0;
    IData/*23:0*/ __Vdly__v1__DOT__unnamedblk3__DOT__keep_cnt;
    __Vdly__v1__DOT__unnamedblk3__DOT__keep_cnt = 0;
    CData/*4:0*/ __Vdly__v1__DOT__adsr__DOT__unnamedblk1__DOT__exponential_counter_period;
    __Vdly__v1__DOT__adsr__DOT__unnamedblk1__DOT__exponential_counter_period = 0;
    CData/*1:0*/ __Vdly__v1__DOT__adsr__DOT__state;
    __Vdly__v1__DOT__adsr__DOT__state = 0;
    CData/*7:0*/ __Vdly__v1__DOT__envelope;
    __Vdly__v1__DOT__envelope = 0;
    CData/*0:0*/ __Vdly__v1__DOT__adsr__DOT__unnamedblk1__DOT__hold_zero;
    __Vdly__v1__DOT__adsr__DOT__unnamedblk1__DOT__hold_zero = 0;
    CData/*4:0*/ __Vdly__v1__DOT__adsr__DOT__unnamedblk1__DOT__exponential_counter;
    __Vdly__v1__DOT__adsr__DOT__unnamedblk1__DOT__exponential_counter = 0;
    SData/*14:0*/ __Vdly__v1__DOT__adsr__DOT__unnamedblk1__DOT__rate_counter;
    __Vdly__v1__DOT__adsr__DOT__unnamedblk1__DOT__rate_counter = 0;
    IData/*23:0*/ __Vdly__v2__DOT__oscillator;
    __Vdly__v2__DOT__oscillator = 0;
    IData/*22:0*/ __Vdly__v2__DOT__unnamedblk2__DOT__lfsr_noise;
    __Vdly__v2__DOT__unnamedblk2__DOT__lfsr_noise = 0;
    IData/*23:0*/ __Vdly__v2__DOT__unnamedblk2__DOT__noise_age;
    __Vdly__v2__DOT__unnamedblk2__DOT__noise_age = 0;
    IData/*23:0*/ __Vdly__v2__DOT__unnamedblk3__DOT__keep_cnt;
    __Vdly__v2__DOT__unnamedblk3__DOT__keep_cnt = 0;
    CData/*4:0*/ __Vdly__v2__DOT__adsr__DOT__unnamedblk1__DOT__exponential_counter_period;
    __Vdly__v2__DOT__adsr__DOT__unnamedblk1__DOT__exponential_counter_period = 0;
    CData/*1:0*/ __Vdly__v2__DOT__adsr__DOT__state;
    __Vdly__v2__DOT__adsr__DOT__state = 0;
    CData/*7:0*/ __Vdly__v2__DOT__envelope;
    __Vdly__v2__DOT__envelope = 0;
    CData/*0:0*/ __Vdly__v2__DOT__adsr__DOT__unnamedblk1__DOT__hold_zero;
    __Vdly__v2__DOT__adsr__DOT__unnamedblk1__DOT__hold_zero = 0;
    CData/*4:0*/ __Vdly__v2__DOT__adsr__DOT__unnamedblk1__DOT__exponential_counter;
    __Vdly__v2__DOT__adsr__DOT__unnamedblk1__DOT__exponential_counter = 0;
    SData/*14:0*/ __Vdly__v2__DOT__adsr__DOT__unnamedblk1__DOT__rate_counter;
    __Vdly__v2__DOT__adsr__DOT__unnamedblk1__DOT__rate_counter = 0;
    IData/*23:0*/ __Vdly__v3__DOT__oscillator;
    __Vdly__v3__DOT__oscillator = 0;
    IData/*22:0*/ __Vdly__v3__DOT__unnamedblk2__DOT__lfsr_noise;
    __Vdly__v3__DOT__unnamedblk2__DOT__lfsr_noise = 0;
    IData/*23:0*/ __Vdly__v3__DOT__unnamedblk2__DOT__noise_age;
    __Vdly__v3__DOT__unnamedblk2__DOT__noise_age = 0;
    IData/*23:0*/ __Vdly__v3__DOT__unnamedblk3__DOT__keep_cnt;
    __Vdly__v3__DOT__unnamedblk3__DOT__keep_cnt = 0;
    CData/*4:0*/ __Vdly__v3__DOT__adsr__DOT__unnamedblk1__DOT__exponential_counter_period;
    __Vdly__v3__DOT__adsr__DOT__unnamedblk1__DOT__exponential_counter_period = 0;
    CData/*1:0*/ __Vdly__v3__DOT__adsr__DOT__state;
    __Vdly__v3__DOT__adsr__DOT__state = 0;
    CData/*7:0*/ __Vdly__v3__DOT__envelope;
    __Vdly__v3__DOT__envelope = 0;
    CData/*0:0*/ __Vdly__v3__DOT__adsr__DOT__unnamedblk1__DOT__hold_zero;
    __Vdly__v3__DOT__adsr__DOT__unnamedblk1__DOT__hold_zero = 0;
    CData/*4:0*/ __Vdly__v3__DOT__adsr__DOT__unnamedblk1__DOT__exponential_counter;
    __Vdly__v3__DOT__adsr__DOT__unnamedblk1__DOT__exponential_counter = 0;
    SData/*14:0*/ __Vdly__v3__DOT__adsr__DOT__unnamedblk1__DOT__rate_counter;
    __Vdly__v3__DOT__adsr__DOT__unnamedblk1__DOT__rate_counter = 0;
    CData/*0:0*/ __VdlySet__voice_pw__v0;
    __VdlySet__voice_pw__v0 = 0;
    CData/*7:0*/ __VdlyVal__voice_pw__v3;
    __VdlyVal__voice_pw__v3 = 0;
    CData/*0:0*/ __VdlySet__voice_pw__v3;
    __VdlySet__voice_pw__v3 = 0;
    CData/*3:0*/ __VdlyVal__voice_pw__v4;
    __VdlyVal__voice_pw__v4 = 0;
    CData/*0:0*/ __VdlySet__voice_pw__v4;
    __VdlySet__voice_pw__v4 = 0;
    CData/*7:0*/ __VdlyVal__voice_pw__v5;
    __VdlyVal__voice_pw__v5 = 0;
    CData/*0:0*/ __VdlySet__voice_pw__v5;
    __VdlySet__voice_pw__v5 = 0;
    CData/*3:0*/ __VdlyVal__voice_pw__v6;
    __VdlyVal__voice_pw__v6 = 0;
    CData/*0:0*/ __VdlySet__voice_pw__v6;
    __VdlySet__voice_pw__v6 = 0;
    CData/*7:0*/ __VdlyVal__voice_pw__v7;
    __VdlyVal__voice_pw__v7 = 0;
    CData/*0:0*/ __VdlySet__voice_pw__v7;
    __VdlySet__voice_pw__v7 = 0;
    CData/*3:0*/ __VdlyVal__voice_pw__v8;
    __VdlyVal__voice_pw__v8 = 0;
    CData/*0:0*/ __VdlySet__voice_pw__v8;
    __VdlySet__voice_pw__v8 = 0;
    CData/*0:0*/ __VdlySet__voice_freq__v0;
    __VdlySet__voice_freq__v0 = 0;
    CData/*7:0*/ __VdlyVal__voice_freq__v3;
    __VdlyVal__voice_freq__v3 = 0;
    CData/*0:0*/ __VdlySet__voice_freq__v3;
    __VdlySet__voice_freq__v3 = 0;
    CData/*7:0*/ __VdlyVal__voice_freq__v4;
    __VdlyVal__voice_freq__v4 = 0;
    CData/*0:0*/ __VdlySet__voice_freq__v4;
    __VdlySet__voice_freq__v4 = 0;
    CData/*7:0*/ __VdlyVal__voice_freq__v5;
    __VdlyVal__voice_freq__v5 = 0;
    CData/*0:0*/ __VdlySet__voice_freq__v5;
    __VdlySet__voice_freq__v5 = 0;
    CData/*7:0*/ __VdlyVal__voice_freq__v6;
    __VdlyVal__voice_freq__v6 = 0;
    CData/*0:0*/ __VdlySet__voice_freq__v6;
    __VdlySet__voice_freq__v6 = 0;
    CData/*7:0*/ __VdlyVal__voice_freq__v7;
    __VdlyVal__voice_freq__v7 = 0;
    CData/*0:0*/ __VdlySet__voice_freq__v7;
    __VdlySet__voice_freq__v7 = 0;
    CData/*7:0*/ __VdlyVal__voice_freq__v8;
    __VdlyVal__voice_freq__v8 = 0;
    CData/*0:0*/ __VdlySet__voice_freq__v8;
    __VdlySet__voice_freq__v8 = 0;
    CData/*0:0*/ __VdlySet__voice_ctrl__v0;
    __VdlySet__voice_ctrl__v0 = 0;
    CData/*7:0*/ __VdlyVal__voice_ctrl__v3;
    __VdlyVal__voice_ctrl__v3 = 0;
    CData/*0:0*/ __VdlySet__voice_ctrl__v3;
    __VdlySet__voice_ctrl__v3 = 0;
    CData/*7:0*/ __VdlyVal__voice_ctrl__v4;
    __VdlyVal__voice_ctrl__v4 = 0;
    CData/*0:0*/ __VdlySet__voice_ctrl__v4;
    __VdlySet__voice_ctrl__v4 = 0;
    CData/*7:0*/ __VdlyVal__voice_ctrl__v5;
    __VdlyVal__voice_ctrl__v5 = 0;
    CData/*0:0*/ __VdlySet__voice_ctrl__v5;
    __VdlySet__voice_ctrl__v5 = 0;
    CData/*0:0*/ __VdlySet__voice_ad__v0;
    __VdlySet__voice_ad__v0 = 0;
    CData/*7:0*/ __VdlyVal__voice_ad__v3;
    __VdlyVal__voice_ad__v3 = 0;
    CData/*0:0*/ __VdlySet__voice_ad__v3;
    __VdlySet__voice_ad__v3 = 0;
    CData/*7:0*/ __VdlyVal__voice_ad__v4;
    __VdlyVal__voice_ad__v4 = 0;
    CData/*0:0*/ __VdlySet__voice_ad__v4;
    __VdlySet__voice_ad__v4 = 0;
    CData/*7:0*/ __VdlyVal__voice_ad__v5;
    __VdlyVal__voice_ad__v5 = 0;
    CData/*0:0*/ __VdlySet__voice_ad__v5;
    __VdlySet__voice_ad__v5 = 0;
    CData/*0:0*/ __VdlySet__voice_vol__v0;
    __VdlySet__voice_vol__v0 = 0;
    CData/*3:0*/ __VdlyVal__voice_vol__v3;
    __VdlyVal__voice_vol__v3 = 0;
    CData/*0:0*/ __VdlySet__voice_vol__v3;
    __VdlySet__voice_vol__v3 = 0;
    CData/*3:0*/ __VdlyVal__voice_vol__v4;
    __VdlyVal__voice_vol__v4 = 0;
    CData/*0:0*/ __VdlySet__voice_vol__v4;
    __VdlySet__voice_vol__v4 = 0;
    CData/*3:0*/ __VdlyVal__voice_vol__v5;
    __VdlyVal__voice_vol__v5 = 0;
    CData/*0:0*/ __VdlySet__voice_vol__v5;
    __VdlySet__voice_vol__v5 = 0;
    CData/*0:0*/ __VdlySet__voice_sr__v0;
    __VdlySet__voice_sr__v0 = 0;
    CData/*7:0*/ __VdlyVal__voice_sr__v3;
    __VdlyVal__voice_sr__v3 = 0;
    CData/*0:0*/ __VdlySet__voice_sr__v3;
    __VdlySet__voice_sr__v3 = 0;
    CData/*7:0*/ __VdlyVal__voice_sr__v4;
    __VdlyVal__voice_sr__v4 = 0;
    CData/*0:0*/ __VdlySet__voice_sr__v4;
    __VdlySet__voice_sr__v4 = 0;
    CData/*7:0*/ __VdlyVal__voice_sr__v5;
    __VdlyVal__voice_sr__v5 = 0;
    CData/*0:0*/ __VdlySet__voice_sr__v5;
    __VdlySet__voice_sr__v5 = 0;
    CData/*7:0*/ __VdlyVal___st_out__v0;
    __VdlyVal___st_out__v0 = 0;
    CData/*2:0*/ __VdlyDim0___st_out__v0;
    __VdlyDim0___st_out__v0 = 0;
    CData/*0:0*/ __VdlySet___st_out__v0;
    __VdlySet___st_out__v0 = 0;
    CData/*7:0*/ __VdlyVal__p_t_out__v0;
    __VdlyVal__p_t_out__v0 = 0;
    CData/*2:0*/ __VdlyDim0__p_t_out__v0;
    __VdlyDim0__p_t_out__v0 = 0;
    CData/*0:0*/ __VdlySet__p_t_out__v0;
    __VdlySet__p_t_out__v0 = 0;
    CData/*7:0*/ __VdlyVal__ps___05Fout__v0;
    __VdlyVal__ps___05Fout__v0 = 0;
    CData/*2:0*/ __VdlyDim0__ps___05Fout__v0;
    __VdlyDim0__ps___05Fout__v0 = 0;
    CData/*0:0*/ __VdlySet__ps___05Fout__v0;
    __VdlySet__ps___05Fout__v0 = 0;
    CData/*7:0*/ __VdlyVal__pst_out__v0;
    __VdlyVal__pst_out__v0 = 0;
    CData/*2:0*/ __VdlyDim0__pst_out__v0;
    __VdlyDim0__pst_out__v0 = 0;
    CData/*0:0*/ __VdlySet__pst_out__v0;
    __VdlySet__pst_out__v0 = 0;
    // Body
    __VdlySet__voice_ad__v0 = 0U;
    __VdlySet__voice_ad__v3 = 0U;
    __VdlySet__voice_ad__v4 = 0U;
    __VdlySet__voice_ad__v5 = 0U;
    __VdlySet__voice_sr__v0 = 0U;
    __VdlySet__voice_sr__v3 = 0U;
    __VdlySet__voice_sr__v4 = 0U;
    __VdlySet__voice_sr__v5 = 0U;
    __VdlySet__voice_pw__v0 = 0U;
    __VdlySet__voice_pw__v3 = 0U;
    __VdlySet__voice_pw__v4 = 0U;
    __VdlySet__voice_pw__v5 = 0U;
    __VdlySet__voice_pw__v6 = 0U;
    __VdlySet__voice_pw__v7 = 0U;
    __VdlySet__voice_pw__v8 = 0U;
    __VdlySet__voice_freq__v0 = 0U;
    __VdlySet__voice_freq__v3 = 0U;
    __VdlySet__voice_freq__v4 = 0U;
    __VdlySet__voice_freq__v5 = 0U;
    __VdlySet__voice_freq__v6 = 0U;
    __VdlySet__voice_freq__v7 = 0U;
    __VdlySet__voice_freq__v8 = 0U;
    __VdlySet__voice_vol__v0 = 0U;
    __VdlySet__voice_vol__v3 = 0U;
    __VdlySet__voice_vol__v4 = 0U;
    __VdlySet__voice_vol__v5 = 0U;
    __Vdly__v1__DOT__adsr__DOT__unnamedblk1__DOT__exponential_counter_period 
        = vlSelfRef.__PVT__v1__DOT__adsr__DOT__unnamedblk1__DOT__exponential_counter_period;
    __Vdly__v1__DOT__adsr__DOT__unnamedblk1__DOT__hold_zero 
        = vlSelfRef.__PVT__v1__DOT__adsr__DOT__unnamedblk1__DOT__hold_zero;
    __Vdly__v1__DOT__adsr__DOT__unnamedblk1__DOT__exponential_counter 
        = vlSelfRef.__PVT__v1__DOT__adsr__DOT__unnamedblk1__DOT__exponential_counter;
    __Vdly__v1__DOT__adsr__DOT__unnamedblk1__DOT__rate_counter 
        = vlSelfRef.__PVT__v1__DOT__adsr__DOT__unnamedblk1__DOT__rate_counter;
    __Vdly__v2__DOT__adsr__DOT__unnamedblk1__DOT__exponential_counter_period 
        = vlSelfRef.__PVT__v2__DOT__adsr__DOT__unnamedblk1__DOT__exponential_counter_period;
    __Vdly__v2__DOT__adsr__DOT__unnamedblk1__DOT__hold_zero 
        = vlSelfRef.__PVT__v2__DOT__adsr__DOT__unnamedblk1__DOT__hold_zero;
    __Vdly__v2__DOT__adsr__DOT__unnamedblk1__DOT__exponential_counter 
        = vlSelfRef.__PVT__v2__DOT__adsr__DOT__unnamedblk1__DOT__exponential_counter;
    __Vdly__v2__DOT__adsr__DOT__unnamedblk1__DOT__rate_counter 
        = vlSelfRef.__PVT__v2__DOT__adsr__DOT__unnamedblk1__DOT__rate_counter;
    __Vdly__v1__DOT__adsr__DOT__state = vlSelfRef.__PVT__v1__DOT__adsr__DOT__state;
    __Vdly__v2__DOT__adsr__DOT__state = vlSelfRef.__PVT__v2__DOT__adsr__DOT__state;
    __Vdly__v1__DOT__unnamedblk3__DOT__keep_cnt = vlSelfRef.__PVT__v1__DOT__unnamedblk3__DOT__keep_cnt;
    __Vdly__v2__DOT__unnamedblk3__DOT__keep_cnt = vlSelfRef.__PVT__v2__DOT__unnamedblk3__DOT__keep_cnt;
    __Vdly__v3__DOT__unnamedblk3__DOT__keep_cnt = vlSelfRef.__PVT__v3__DOT__unnamedblk3__DOT__keep_cnt;
    __Vdly__v3__DOT__adsr__DOT__unnamedblk1__DOT__exponential_counter_period 
        = vlSelfRef.__PVT__v3__DOT__adsr__DOT__unnamedblk1__DOT__exponential_counter_period;
    __Vdly__v3__DOT__adsr__DOT__unnamedblk1__DOT__hold_zero 
        = vlSelfRef.__PVT__v3__DOT__adsr__DOT__unnamedblk1__DOT__hold_zero;
    __Vdly__v3__DOT__adsr__DOT__unnamedblk1__DOT__exponential_counter 
        = vlSelfRef.__PVT__v3__DOT__adsr__DOT__unnamedblk1__DOT__exponential_counter;
    __Vdly__v3__DOT__adsr__DOT__unnamedblk1__DOT__rate_counter 
        = vlSelfRef.__PVT__v3__DOT__adsr__DOT__unnamedblk1__DOT__rate_counter;
    __Vdly__v3__DOT__adsr__DOT__state = vlSelfRef.__PVT__v3__DOT__adsr__DOT__state;
    __Vdly__v1__DOT__envelope = vlSelfRef.__PVT__v1__DOT__envelope;
    __Vdly__v2__DOT__envelope = vlSelfRef.__PVT__v2__DOT__envelope;
    __Vdly__v3__DOT__envelope = vlSelfRef.__PVT__v3__DOT__envelope;
    __Vdly__v1__DOT__oscillator = vlSelfRef.__PVT__v1__DOT__oscillator;
    __Vdly__v2__DOT__oscillator = vlSelfRef.__PVT__v2__DOT__oscillator;
    __Vdly__v3__DOT__oscillator = vlSelfRef.__PVT__v3__DOT__oscillator;
    __Vdly__tbl_state = vlSelfRef.__PVT__tbl_state;
    __Vdly__f_acc_t = vlSelfRef.__PVT__f_acc_t;
    __VdlySet___st_out__v0 = 0U;
    __VdlySet__p_t_out__v0 = 0U;
    __VdlySet__ps___05Fout__v0 = 0U;
    __VdlySet__pst_out__v0 = 0U;
    __Vdly__v1__DOT__unnamedblk2__DOT__lfsr_noise = vlSelfRef.__PVT__v1__DOT__unnamedblk2__DOT__lfsr_noise;
    __Vdly__v1__DOT__unnamedblk2__DOT__noise_age = vlSelfRef.__PVT__v1__DOT__unnamedblk2__DOT__noise_age;
    __Vdly__v2__DOT__unnamedblk2__DOT__lfsr_noise = vlSelfRef.__PVT__v2__DOT__unnamedblk2__DOT__lfsr_noise;
    __Vdly__v2__DOT__unnamedblk2__DOT__noise_age = vlSelfRef.__PVT__v2__DOT__unnamedblk2__DOT__noise_age;
    __Vdly__v3__DOT__unnamedblk2__DOT__lfsr_noise = vlSelfRef.__PVT__v3__DOT__unnamedblk2__DOT__lfsr_noise;
    __Vdly__v3__DOT__unnamedblk2__DOT__noise_age = vlSelfRef.__PVT__v3__DOT__unnamedblk2__DOT__noise_age;
    __VdlySet__voice_ctrl__v0 = 0U;
    __VdlySet__voice_ctrl__v3 = 0U;
    __VdlySet__voice_ctrl__v4 = 0U;
    __VdlySet__voice_ctrl__v5 = 0U;
    if ((0xffU == (IData)(vlSelfRef.__PVT__v1__DOT__envelope))) {
        __Vdly__v1__DOT__adsr__DOT__unnamedblk1__DOT__exponential_counter_period = 0U;
    } else if ((0x5dU == (IData)(vlSelfRef.__PVT__v1__DOT__envelope))) {
        __Vdly__v1__DOT__adsr__DOT__unnamedblk1__DOT__exponential_counter_period = 1U;
    } else if ((0x36U == (IData)(vlSelfRef.__PVT__v1__DOT__envelope))) {
        __Vdly__v1__DOT__adsr__DOT__unnamedblk1__DOT__exponential_counter_period = 3U;
    } else if ((0x1aU == (IData)(vlSelfRef.__PVT__v1__DOT__envelope))) {
        __Vdly__v1__DOT__adsr__DOT__unnamedblk1__DOT__exponential_counter_period = 7U;
    } else if ((0x0eU == (IData)(vlSelfRef.__PVT__v1__DOT__envelope))) {
        __Vdly__v1__DOT__adsr__DOT__unnamedblk1__DOT__exponential_counter_period = 0x0fU;
    } else if ((6U == (IData)(vlSelfRef.__PVT__v1__DOT__envelope))) {
        __Vdly__v1__DOT__adsr__DOT__unnamedblk1__DOT__exponential_counter_period = 0x1dU;
    } else if ((0U == (IData)(vlSelfRef.__PVT__v1__DOT__envelope))) {
        __Vdly__v1__DOT__adsr__DOT__unnamedblk1__DOT__exponential_counter_period = 0U;
    }
    if ((0xffU == (IData)(vlSelfRef.__PVT__v2__DOT__envelope))) {
        __Vdly__v2__DOT__adsr__DOT__unnamedblk1__DOT__exponential_counter_period = 0U;
    } else if ((0x5dU == (IData)(vlSelfRef.__PVT__v2__DOT__envelope))) {
        __Vdly__v2__DOT__adsr__DOT__unnamedblk1__DOT__exponential_counter_period = 1U;
    } else if ((0x36U == (IData)(vlSelfRef.__PVT__v2__DOT__envelope))) {
        __Vdly__v2__DOT__adsr__DOT__unnamedblk1__DOT__exponential_counter_period = 3U;
    } else if ((0x1aU == (IData)(vlSelfRef.__PVT__v2__DOT__envelope))) {
        __Vdly__v2__DOT__adsr__DOT__unnamedblk1__DOT__exponential_counter_period = 7U;
    } else if ((0x0eU == (IData)(vlSelfRef.__PVT__v2__DOT__envelope))) {
        __Vdly__v2__DOT__adsr__DOT__unnamedblk1__DOT__exponential_counter_period = 0x0fU;
    } else if ((6U == (IData)(vlSelfRef.__PVT__v2__DOT__envelope))) {
        __Vdly__v2__DOT__adsr__DOT__unnamedblk1__DOT__exponential_counter_period = 0x1dU;
    } else if ((0U == (IData)(vlSelfRef.__PVT__v2__DOT__envelope))) {
        __Vdly__v2__DOT__adsr__DOT__unnamedblk1__DOT__exponential_counter_period = 0U;
    }
    if ((0xffU == (IData)(vlSelfRef.__PVT__v3__DOT__envelope))) {
        __Vdly__v3__DOT__adsr__DOT__unnamedblk1__DOT__exponential_counter_period = 0U;
    } else if ((0x5dU == (IData)(vlSelfRef.__PVT__v3__DOT__envelope))) {
        __Vdly__v3__DOT__adsr__DOT__unnamedblk1__DOT__exponential_counter_period = 1U;
    } else if ((0x36U == (IData)(vlSelfRef.__PVT__v3__DOT__envelope))) {
        __Vdly__v3__DOT__adsr__DOT__unnamedblk1__DOT__exponential_counter_period = 3U;
    } else if ((0x1aU == (IData)(vlSelfRef.__PVT__v3__DOT__envelope))) {
        __Vdly__v3__DOT__adsr__DOT__unnamedblk1__DOT__exponential_counter_period = 7U;
    } else if ((0x0eU == (IData)(vlSelfRef.__PVT__v3__DOT__envelope))) {
        __Vdly__v3__DOT__adsr__DOT__unnamedblk1__DOT__exponential_counter_period = 0x0fU;
    } else if ((6U == (IData)(vlSelfRef.__PVT__v3__DOT__envelope))) {
        __Vdly__v3__DOT__adsr__DOT__unnamedblk1__DOT__exponential_counter_period = 0x1dU;
    } else if ((0U == (IData)(vlSelfRef.__PVT__v3__DOT__envelope))) {
        __Vdly__v3__DOT__adsr__DOT__unnamedblk1__DOT__exponential_counter_period = 0U;
    }
    if ((1U & (~ (0x0000000fU == (IData)(vlSelfRef.__PVT__tbl_state))))) {
        __Vdly__tbl_state = (0x0000000fU & ((IData)(1U) 
                                            + (IData)(vlSelfRef.__PVT__tbl_state)));
    }
    if (vlSymsp->TOP.top__DOT__sid_ce_1m) {
        __Vdly__v1__DOT__oscillator = ((1U & ((((IData)(vlSymsp->TOP.rst) 
                                                | (vlSelfRef.__PVT__voice_ctrl[0U] 
                                                   >> 3U)) 
                                               | (IData)(vlSelfRef.__PVT__v1__DOT__unnamedblk1__DOT__test_delay)) 
                                              | (((vlSelfRef.__PVT__voice_ctrl[0U] 
                                                   >> 1U) 
                                                  & (~ 
                                                     (vlSelfRef.__PVT__v3__DOT__oscillator 
                                                      >> 0x00000017U))) 
                                                 & (IData)(vlSelfRef.__PVT__v1__DOT__osc_msb_in_prv))))
                                        ? 0U : (0x00ffffffU 
                                                & (vlSelfRef.__PVT__v1__DOT__oscillator 
                                                   + vlSelfRef.__PVT__voice_freq[0U])));
        __Vdly__v2__DOT__oscillator = ((1U & ((((IData)(vlSymsp->TOP.rst) 
                                                | (vlSelfRef.__PVT__voice_ctrl[1U] 
                                                   >> 3U)) 
                                               | (IData)(vlSelfRef.__PVT__v2__DOT__unnamedblk1__DOT__test_delay)) 
                                              | (((vlSelfRef.__PVT__voice_ctrl[1U] 
                                                   >> 1U) 
                                                  & (~ 
                                                     (vlSelfRef.__PVT__v1__DOT__oscillator 
                                                      >> 0x00000017U))) 
                                                 & (IData)(vlSelfRef.__PVT__v2__DOT__osc_msb_in_prv))))
                                        ? 0U : (0x00ffffffU 
                                                & (vlSelfRef.__PVT__v2__DOT__oscillator 
                                                   + vlSelfRef.__PVT__voice_freq[1U])));
        __Vdly__v3__DOT__oscillator = ((1U & ((((IData)(vlSymsp->TOP.rst) 
                                                | (vlSelfRef.__PVT__voice_ctrl[2U] 
                                                   >> 3U)) 
                                               | (IData)(vlSelfRef.__PVT__v3__DOT__unnamedblk1__DOT__test_delay)) 
                                              | (((vlSelfRef.__PVT__voice_ctrl[2U] 
                                                   >> 1U) 
                                                  & (~ 
                                                     (vlSelfRef.__PVT__v2__DOT__oscillator 
                                                      >> 0x00000017U))) 
                                                 & (IData)(vlSelfRef.__PVT__v3__DOT__osc_msb_in_prv))))
                                        ? 0U : (0x00ffffffU 
                                                & (vlSelfRef.__PVT__v3__DOT__oscillator 
                                                   + vlSelfRef.__PVT__voice_freq[2U])));
        __Vdly__tbl_state = 0U;
        vlSelfRef.__PVT__v1__DOT__dca_out = (0x003fffffU 
                                             & (((2U 
                                                  & (IData)(vlSymsp->TOP.top__DOT__sid_cfg_reg))
                                                  ? 0U
                                                  : 0x00033cc0U) 
                                                + VL_MULS_III(22, 
                                                              (0x003fffffU 
                                                               & VL_EXTENDS_II(22,13, (IData)(vlSelfRef.__PVT__v1__DOT__unnamedblk3__DOT__dac_out))), 
                                                              (0x003fffffU 
                                                               & VL_EXTENDS_II(22,9, (IData)(vlSelfRef.__PVT__v1__DOT__unnamedblk3__DOT__env_dac))))));
        vlSelfRef.__PVT__v2__DOT__dca_out = (0x003fffffU 
                                             & (((2U 
                                                  & (IData)(vlSymsp->TOP.top__DOT__sid_cfg_reg))
                                                  ? 0U
                                                  : 0x00033cc0U) 
                                                + VL_MULS_III(22, 
                                                              (0x003fffffU 
                                                               & VL_EXTENDS_II(22,13, (IData)(vlSelfRef.__PVT__v2__DOT__unnamedblk3__DOT__dac_out))), 
                                                              (0x003fffffU 
                                                               & VL_EXTENDS_II(22,9, (IData)(vlSelfRef.__PVT__v2__DOT__unnamedblk3__DOT__env_dac))))));
        vlSelfRef.__PVT__v3__DOT__dca_out = (0x003fffffU 
                                             & (((2U 
                                                  & (IData)(vlSymsp->TOP.top__DOT__sid_cfg_reg))
                                                  ? 0U
                                                  : 0x00033cc0U) 
                                                + VL_MULS_III(22, 
                                                              (0x003fffffU 
                                                               & VL_EXTENDS_II(22,13, (IData)(vlSelfRef.__PVT__v3__DOT__unnamedblk3__DOT__dac_out))), 
                                                              (0x003fffffU 
                                                               & VL_EXTENDS_II(22,9, (IData)(vlSelfRef.__PVT__v3__DOT__unnamedblk3__DOT__env_dac))))));
        vlSelfRef.__PVT__v1__DOT__osc_msb_in_prv = 
            (1U & (vlSelfRef.__PVT__v3__DOT__oscillator 
                   >> 0x00000017U));
        vlSelfRef.__PVT__v1__DOT__unnamedblk1__DOT__test_delay 
            = (1U & (((IData)(vlSymsp->TOP.top__DOT__sid_cfg_reg) 
                      >> 1U) & (vlSelfRef.__PVT__voice_ctrl[0U] 
                                >> 3U)));
        vlSelfRef.__PVT__v2__DOT__osc_msb_in_prv = 
            (1U & (vlSelfRef.__PVT__v1__DOT__oscillator 
                   >> 0x00000017U));
        vlSelfRef.__PVT__v2__DOT__unnamedblk1__DOT__test_delay 
            = (1U & (((IData)(vlSymsp->TOP.top__DOT__sid_cfg_reg) 
                      >> 1U) & (vlSelfRef.__PVT__voice_ctrl[1U] 
                                >> 3U)));
        vlSelfRef.__PVT__v3__DOT__osc_msb_in_prv = 
            (1U & (vlSelfRef.__PVT__v2__DOT__oscillator 
                   >> 0x00000017U));
        vlSelfRef.__PVT__v3__DOT__unnamedblk1__DOT__test_delay 
            = (1U & (((IData)(vlSymsp->TOP.top__DOT__sid_cfg_reg) 
                      >> 1U) & (vlSelfRef.__PVT__voice_ctrl[2U] 
                                >> 3U)));
    }
    if (vlSymsp->TOP.rst) {
        __VdlySet__voice_ad__v0 = 1U;
        __VdlySet__voice_sr__v0 = 1U;
        __VdlySet__voice_pw__v0 = 1U;
        __VdlySet__voice_freq__v0 = 1U;
        __VdlySet__voice_vol__v0 = 1U;
        __Vdly__v1__DOT__adsr__DOT__state = 0U;
        __Vdly__v1__DOT__envelope = 0U;
        __Vdly__v1__DOT__adsr__DOT__unnamedblk1__DOT__hold_zero = 1U;
        __Vdly__v1__DOT__adsr__DOT__unnamedblk1__DOT__exponential_counter = 0U;
        __Vdly__v1__DOT__adsr__DOT__unnamedblk1__DOT__exponential_counter_period = 0U;
        __Vdly__v1__DOT__adsr__DOT__unnamedblk1__DOT__rate_counter = 0U;
        __Vdly__v2__DOT__adsr__DOT__state = 0U;
        __Vdly__v2__DOT__envelope = 0U;
        __Vdly__v2__DOT__adsr__DOT__unnamedblk1__DOT__hold_zero = 1U;
        __Vdly__v2__DOT__adsr__DOT__unnamedblk1__DOT__exponential_counter = 0U;
        __Vdly__v2__DOT__adsr__DOT__unnamedblk1__DOT__exponential_counter_period = 0U;
        __Vdly__v2__DOT__adsr__DOT__unnamedblk1__DOT__rate_counter = 0U;
        __Vdly__v3__DOT__adsr__DOT__state = 0U;
        __Vdly__v3__DOT__envelope = 0U;
        __Vdly__v3__DOT__adsr__DOT__unnamedblk1__DOT__hold_zero = 1U;
        __Vdly__v3__DOT__adsr__DOT__unnamedblk1__DOT__exponential_counter = 0U;
        __Vdly__v3__DOT__adsr__DOT__unnamedblk1__DOT__exponential_counter_period = 0U;
        __Vdly__v3__DOT__adsr__DOT__unnamedblk1__DOT__rate_counter = 0U;
        __VdlySet__voice_ctrl__v0 = 1U;
        vlSelfRef.__PVT__last_written = 0U;
        vlSelfRef.__PVT__v1__DOT__saw_tri = 0U;
        vlSelfRef.__PVT__v2__DOT__saw_tri = 0U;
        vlSelfRef.__PVT__v3__DOT__saw_tri = 0U;
        vlSelfRef.__PVT__v1__DOT__pulse = 0U;
        vlSelfRef.__PVT__v2__DOT__pulse = 0U;
        vlSelfRef.__PVT__v3__DOT__pulse = 0U;
        vlSelfRef.__PVT__v1__DOT__noise = 0U;
        __Vdly__v1__DOT__unnamedblk2__DOT__lfsr_noise = 0x007fffffU;
        __Vdly__v1__DOT__unnamedblk2__DOT__noise_age = 0U;
    } else {
        if (vlSelfRef.cs) {
            if (((((((((0U == (0x0000001fU & (IData)(vlSymsp->TOP.top__DOT__cpu_addr))) 
                       | (1U == (0x0000001fU & (IData)(vlSymsp->TOP.top__DOT__cpu_addr)))) 
                      | (2U == (0x0000001fU & (IData)(vlSymsp->TOP.top__DOT__cpu_addr)))) 
                     | (3U == (0x0000001fU & (IData)(vlSymsp->TOP.top__DOT__cpu_addr)))) 
                    | (4U == (0x0000001fU & (IData)(vlSymsp->TOP.top__DOT__cpu_addr)))) 
                   | (5U == (0x0000001fU & (IData)(vlSymsp->TOP.top__DOT__cpu_addr)))) 
                  | (6U == (0x0000001fU & (IData)(vlSymsp->TOP.top__DOT__cpu_addr)))) 
                 | (7U == (0x0000001fU & (IData)(vlSymsp->TOP.top__DOT__cpu_addr))))) {
                if ((0U != (0x0000001fU & (IData)(vlSymsp->TOP.top__DOT__cpu_addr)))) {
                    if ((1U != (0x0000001fU & (IData)(vlSymsp->TOP.top__DOT__cpu_addr)))) {
                        if ((2U != (0x0000001fU & (IData)(vlSymsp->TOP.top__DOT__cpu_addr)))) {
                            if ((3U != (0x0000001fU 
                                        & (IData)(vlSymsp->TOP.top__DOT__cpu_addr)))) {
                                if ((4U != (0x0000001fU 
                                            & (IData)(vlSymsp->TOP.top__DOT__cpu_addr)))) {
                                    if ((5U == (0x0000001fU 
                                                & (IData)(vlSymsp->TOP.top__DOT__cpu_addr)))) {
                                        __VdlyVal__voice_ad__v3 
                                            = vlSymsp->TOP.top__DOT__cpu_dout;
                                        __VdlySet__voice_ad__v3 = 1U;
                                    }
                                    if ((5U != (0x0000001fU 
                                                & (IData)(vlSymsp->TOP.top__DOT__cpu_addr)))) {
                                        if ((6U == 
                                             (0x0000001fU 
                                              & (IData)(vlSymsp->TOP.top__DOT__cpu_addr)))) {
                                            __VdlyVal__voice_sr__v3 
                                                = vlSymsp->TOP.top__DOT__cpu_dout;
                                            __VdlySet__voice_sr__v3 = 1U;
                                        }
                                    }
                                }
                                if ((4U == (0x0000001fU 
                                            & (IData)(vlSymsp->TOP.top__DOT__cpu_addr)))) {
                                    __VdlyVal__voice_ctrl__v3 
                                        = vlSymsp->TOP.top__DOT__cpu_dout;
                                    __VdlySet__voice_ctrl__v3 = 1U;
                                }
                            }
                        }
                        if ((2U == (0x0000001fU & (IData)(vlSymsp->TOP.top__DOT__cpu_addr)))) {
                            __VdlyVal__voice_pw__v3 
                                = vlSymsp->TOP.top__DOT__cpu_dout;
                            __VdlySet__voice_pw__v3 = 1U;
                        } else if ((3U == (0x0000001fU 
                                           & (IData)(vlSymsp->TOP.top__DOT__cpu_addr)))) {
                            __VdlyVal__voice_pw__v4 
                                = (0x0000000fU & (IData)(vlSymsp->TOP.top__DOT__cpu_dout));
                            __VdlySet__voice_pw__v4 = 1U;
                        }
                    }
                }
                if ((0U == (0x0000001fU & (IData)(vlSymsp->TOP.top__DOT__cpu_addr)))) {
                    __VdlyVal__voice_freq__v3 = vlSymsp->TOP.top__DOT__cpu_dout;
                    __VdlySet__voice_freq__v3 = 1U;
                } else if ((1U == (0x0000001fU & (IData)(vlSymsp->TOP.top__DOT__cpu_addr)))) {
                    __VdlyVal__voice_freq__v4 = vlSymsp->TOP.top__DOT__cpu_dout;
                    __VdlySet__voice_freq__v4 = 1U;
                } else if ((2U != (0x0000001fU & (IData)(vlSymsp->TOP.top__DOT__cpu_addr)))) {
                    if ((3U != (0x0000001fU & (IData)(vlSymsp->TOP.top__DOT__cpu_addr)))) {
                        if ((4U != (0x0000001fU & (IData)(vlSymsp->TOP.top__DOT__cpu_addr)))) {
                            if ((5U != (0x0000001fU 
                                        & (IData)(vlSymsp->TOP.top__DOT__cpu_addr)))) {
                                if ((6U != (0x0000001fU 
                                            & (IData)(vlSymsp->TOP.top__DOT__cpu_addr)))) {
                                    __VdlyVal__voice_freq__v5 
                                        = vlSymsp->TOP.top__DOT__cpu_dout;
                                    __VdlySet__voice_freq__v5 = 1U;
                                }
                            }
                        }
                    }
                }
            } else if (((((((((8U == (0x0000001fU & (IData)(vlSymsp->TOP.top__DOT__cpu_addr))) 
                              | (9U == (0x0000001fU 
                                        & (IData)(vlSymsp->TOP.top__DOT__cpu_addr)))) 
                             | (0x0aU == (0x0000001fU 
                                          & (IData)(vlSymsp->TOP.top__DOT__cpu_addr)))) 
                            | (0x0bU == (0x0000001fU 
                                         & (IData)(vlSymsp->TOP.top__DOT__cpu_addr)))) 
                           | (0x0cU == (0x0000001fU 
                                        & (IData)(vlSymsp->TOP.top__DOT__cpu_addr)))) 
                          | (0x0dU == (0x0000001fU 
                                       & (IData)(vlSymsp->TOP.top__DOT__cpu_addr)))) 
                         | (0x0eU == (0x0000001fU & (IData)(vlSymsp->TOP.top__DOT__cpu_addr)))) 
                        | (0x0fU == (0x0000001fU & (IData)(vlSymsp->TOP.top__DOT__cpu_addr))))) {
                if ((8U != (0x0000001fU & (IData)(vlSymsp->TOP.top__DOT__cpu_addr)))) {
                    if ((9U != (0x0000001fU & (IData)(vlSymsp->TOP.top__DOT__cpu_addr)))) {
                        if ((0x0aU != (0x0000001fU 
                                       & (IData)(vlSymsp->TOP.top__DOT__cpu_addr)))) {
                            if ((0x0bU != (0x0000001fU 
                                           & (IData)(vlSymsp->TOP.top__DOT__cpu_addr)))) {
                                if ((0x0cU == (0x0000001fU 
                                               & (IData)(vlSymsp->TOP.top__DOT__cpu_addr)))) {
                                    __VdlyVal__voice_ad__v4 
                                        = vlSymsp->TOP.top__DOT__cpu_dout;
                                    __VdlySet__voice_ad__v4 = 1U;
                                }
                                if ((0x0cU != (0x0000001fU 
                                               & (IData)(vlSymsp->TOP.top__DOT__cpu_addr)))) {
                                    if ((0x0dU == (0x0000001fU 
                                                   & (IData)(vlSymsp->TOP.top__DOT__cpu_addr)))) {
                                        __VdlyVal__voice_sr__v4 
                                            = vlSymsp->TOP.top__DOT__cpu_dout;
                                        __VdlySet__voice_sr__v4 = 1U;
                                    }
                                }
                            }
                            if ((0x0bU == (0x0000001fU 
                                           & (IData)(vlSymsp->TOP.top__DOT__cpu_addr)))) {
                                __VdlyVal__voice_ctrl__v4 
                                    = vlSymsp->TOP.top__DOT__cpu_dout;
                                __VdlySet__voice_ctrl__v4 = 1U;
                            }
                        }
                    }
                    if ((9U == (0x0000001fU & (IData)(vlSymsp->TOP.top__DOT__cpu_addr)))) {
                        __VdlyVal__voice_pw__v5 = vlSymsp->TOP.top__DOT__cpu_dout;
                        __VdlySet__voice_pw__v5 = 1U;
                    } else if ((0x0aU == (0x0000001fU 
                                          & (IData)(vlSymsp->TOP.top__DOT__cpu_addr)))) {
                        __VdlyVal__voice_pw__v6 = (0x0000000fU 
                                                   & (IData)(vlSymsp->TOP.top__DOT__cpu_dout));
                        __VdlySet__voice_pw__v6 = 1U;
                    }
                }
                if ((8U == (0x0000001fU & (IData)(vlSymsp->TOP.top__DOT__cpu_addr)))) {
                    __VdlyVal__voice_freq__v6 = vlSymsp->TOP.top__DOT__cpu_dout;
                    __VdlySet__voice_freq__v6 = 1U;
                } else if ((9U != (0x0000001fU & (IData)(vlSymsp->TOP.top__DOT__cpu_addr)))) {
                    if ((0x0aU != (0x0000001fU & (IData)(vlSymsp->TOP.top__DOT__cpu_addr)))) {
                        if ((0x0bU != (0x0000001fU 
                                       & (IData)(vlSymsp->TOP.top__DOT__cpu_addr)))) {
                            if ((0x0cU != (0x0000001fU 
                                           & (IData)(vlSymsp->TOP.top__DOT__cpu_addr)))) {
                                if ((0x0dU != (0x0000001fU 
                                               & (IData)(vlSymsp->TOP.top__DOT__cpu_addr)))) {
                                    if ((0x0eU == (0x0000001fU 
                                                   & (IData)(vlSymsp->TOP.top__DOT__cpu_addr)))) {
                                        __VdlyVal__voice_freq__v7 
                                            = vlSymsp->TOP.top__DOT__cpu_dout;
                                        __VdlySet__voice_freq__v7 = 1U;
                                    } else {
                                        __VdlyVal__voice_freq__v8 
                                            = vlSymsp->TOP.top__DOT__cpu_dout;
                                        __VdlySet__voice_freq__v8 = 1U;
                                    }
                                }
                            }
                        }
                    }
                }
            } else if (((((((((0x10U == (0x0000001fU 
                                         & (IData)(vlSymsp->TOP.top__DOT__cpu_addr))) 
                              | (0x11U == (0x0000001fU 
                                           & (IData)(vlSymsp->TOP.top__DOT__cpu_addr)))) 
                             | (0x12U == (0x0000001fU 
                                          & (IData)(vlSymsp->TOP.top__DOT__cpu_addr)))) 
                            | (0x13U == (0x0000001fU 
                                         & (IData)(vlSymsp->TOP.top__DOT__cpu_addr)))) 
                           | (0x14U == (0x0000001fU 
                                        & (IData)(vlSymsp->TOP.top__DOT__cpu_addr)))) 
                          | (0x15U == (0x0000001fU 
                                       & (IData)(vlSymsp->TOP.top__DOT__cpu_addr)))) 
                         | (0x16U == (0x0000001fU & (IData)(vlSymsp->TOP.top__DOT__cpu_addr)))) 
                        | (0x17U == (0x0000001fU & (IData)(vlSymsp->TOP.top__DOT__cpu_addr))))) {
                if ((0x10U != (0x0000001fU & (IData)(vlSymsp->TOP.top__DOT__cpu_addr)))) {
                    if ((0x11U != (0x0000001fU & (IData)(vlSymsp->TOP.top__DOT__cpu_addr)))) {
                        if ((0x12U != (0x0000001fU 
                                       & (IData)(vlSymsp->TOP.top__DOT__cpu_addr)))) {
                            if ((0x13U == (0x0000001fU 
                                           & (IData)(vlSymsp->TOP.top__DOT__cpu_addr)))) {
                                __VdlyVal__voice_ad__v5 
                                    = vlSymsp->TOP.top__DOT__cpu_dout;
                                __VdlySet__voice_ad__v5 = 1U;
                            }
                            if ((0x13U != (0x0000001fU 
                                           & (IData)(vlSymsp->TOP.top__DOT__cpu_addr)))) {
                                if ((0x14U == (0x0000001fU 
                                               & (IData)(vlSymsp->TOP.top__DOT__cpu_addr)))) {
                                    __VdlyVal__voice_sr__v5 
                                        = vlSymsp->TOP.top__DOT__cpu_dout;
                                    __VdlySet__voice_sr__v5 = 1U;
                                }
                            }
                        }
                        if ((0x12U == (0x0000001fU 
                                       & (IData)(vlSymsp->TOP.top__DOT__cpu_addr)))) {
                            __VdlyVal__voice_ctrl__v5 
                                = vlSymsp->TOP.top__DOT__cpu_dout;
                            __VdlySet__voice_ctrl__v5 = 1U;
                        }
                    }
                }
                if ((0x10U == (0x0000001fU & (IData)(vlSymsp->TOP.top__DOT__cpu_addr)))) {
                    __VdlyVal__voice_pw__v7 = vlSymsp->TOP.top__DOT__cpu_dout;
                    __VdlySet__voice_pw__v7 = 1U;
                } else if ((0x11U == (0x0000001fU & (IData)(vlSymsp->TOP.top__DOT__cpu_addr)))) {
                    __VdlyVal__voice_pw__v8 = (0x0000000fU 
                                               & (IData)(vlSymsp->TOP.top__DOT__cpu_dout));
                    __VdlySet__voice_pw__v8 = 1U;
                }
            }
            if ((1U & (~ ((((((((0U == (0x0000001fU 
                                        & (IData)(vlSymsp->TOP.top__DOT__cpu_addr))) 
                                | (1U == (0x0000001fU 
                                          & (IData)(vlSymsp->TOP.top__DOT__cpu_addr)))) 
                               | (2U == (0x0000001fU 
                                         & (IData)(vlSymsp->TOP.top__DOT__cpu_addr)))) 
                              | (3U == (0x0000001fU 
                                        & (IData)(vlSymsp->TOP.top__DOT__cpu_addr)))) 
                             | (4U == (0x0000001fU 
                                       & (IData)(vlSymsp->TOP.top__DOT__cpu_addr)))) 
                            | (5U == (0x0000001fU & (IData)(vlSymsp->TOP.top__DOT__cpu_addr)))) 
                           | (6U == (0x0000001fU & (IData)(vlSymsp->TOP.top__DOT__cpu_addr)))) 
                          | (7U == (0x0000001fU & (IData)(vlSymsp->TOP.top__DOT__cpu_addr))))))) {
                if ((1U & (~ ((((((((8U == (0x0000001fU 
                                            & (IData)(vlSymsp->TOP.top__DOT__cpu_addr))) 
                                    | (9U == (0x0000001fU 
                                              & (IData)(vlSymsp->TOP.top__DOT__cpu_addr)))) 
                                   | (0x0aU == (0x0000001fU 
                                                & (IData)(vlSymsp->TOP.top__DOT__cpu_addr)))) 
                                  | (0x0bU == (0x0000001fU 
                                               & (IData)(vlSymsp->TOP.top__DOT__cpu_addr)))) 
                                 | (0x0cU == (0x0000001fU 
                                              & (IData)(vlSymsp->TOP.top__DOT__cpu_addr)))) 
                                | (0x0dU == (0x0000001fU 
                                             & (IData)(vlSymsp->TOP.top__DOT__cpu_addr)))) 
                               | (0x0eU == (0x0000001fU 
                                            & (IData)(vlSymsp->TOP.top__DOT__cpu_addr)))) 
                              | (0x0fU == (0x0000001fU 
                                           & (IData)(vlSymsp->TOP.top__DOT__cpu_addr))))))) {
                    if ((1U & (~ ((((((((0x10U == (0x0000001fU 
                                                   & (IData)(vlSymsp->TOP.top__DOT__cpu_addr))) 
                                        | (0x11U == 
                                           (0x0000001fU 
                                            & (IData)(vlSymsp->TOP.top__DOT__cpu_addr)))) 
                                       | (0x12U == 
                                          (0x0000001fU 
                                           & (IData)(vlSymsp->TOP.top__DOT__cpu_addr)))) 
                                      | (0x13U == (0x0000001fU 
                                                   & (IData)(vlSymsp->TOP.top__DOT__cpu_addr)))) 
                                     | (0x14U == (0x0000001fU 
                                                  & (IData)(vlSymsp->TOP.top__DOT__cpu_addr)))) 
                                    | (0x15U == (0x0000001fU 
                                                 & (IData)(vlSymsp->TOP.top__DOT__cpu_addr)))) 
                                   | (0x16U == (0x0000001fU 
                                                & (IData)(vlSymsp->TOP.top__DOT__cpu_addr)))) 
                                  | (0x17U == (0x0000001fU 
                                               & (IData)(vlSymsp->TOP.top__DOT__cpu_addr))))))) {
                        if ((0x18U != (0x0000001fU 
                                       & (IData)(vlSymsp->TOP.top__DOT__cpu_addr)))) {
                            if ((0x1dU == (0x0000001fU 
                                           & (IData)(vlSymsp->TOP.top__DOT__cpu_addr)))) {
                                __VdlyVal__voice_vol__v3 
                                    = (0x0000000fU 
                                       & (IData)(vlSymsp->TOP.top__DOT__cpu_dout));
                                __VdlySet__voice_vol__v3 = 1U;
                            } else if ((0x1eU == (0x0000001fU 
                                                  & (IData)(vlSymsp->TOP.top__DOT__cpu_addr)))) {
                                __VdlyVal__voice_vol__v4 
                                    = (0x0000000fU 
                                       & (IData)(vlSymsp->TOP.top__DOT__cpu_dout));
                                __VdlySet__voice_vol__v4 = 1U;
                            } else if ((0x1fU == (0x0000001fU 
                                                  & (IData)(vlSymsp->TOP.top__DOT__cpu_addr)))) {
                                __VdlyVal__voice_vol__v5 
                                    = (0x0000000fU 
                                       & (IData)(vlSymsp->TOP.top__DOT__cpu_dout));
                                __VdlySet__voice_vol__v5 = 1U;
                            }
                        }
                    }
                }
            }
            vlSelfRef.__PVT__last_written = vlSymsp->TOP.top__DOT__cpu_dout;
        }
        if (vlSymsp->TOP.top__DOT__sid_ce_1m) {
            __Vdly__v1__DOT__adsr__DOT__unnamedblk1__DOT__rate_counter 
                = (0x00007fffU & ((IData)(1U) + (IData)(vlSelfRef.__PVT__v1__DOT__adsr__DOT__unnamedblk1__DOT__rate_counter)));
            if (((IData)(vlSelfRef.__PVT__v1__DOT__adsr__DOT__unnamedblk1__DOT__rate_counter) 
                 == (IData)(vlSelfRef.__PVT__v1__DOT__adsr__DOT__rate_period))) {
                __Vdly__v1__DOT__adsr__DOT__unnamedblk1__DOT__rate_counter = 0U;
                __Vdly__v1__DOT__adsr__DOT__unnamedblk1__DOT__exponential_counter 
                    = (0x0000001fU & ((IData)(1U) + (IData)(vlSelfRef.__PVT__v1__DOT__adsr__DOT__unnamedblk1__DOT__exponential_counter)));
                if (((1U == (IData)(vlSelfRef.__PVT__v1__DOT__adsr__DOT__state)) 
                     | ((IData)(vlSelfRef.__PVT__v1__DOT__adsr__DOT__unnamedblk1__DOT__exponential_counter) 
                        == (IData)(vlSelfRef.__PVT__v1__DOT__adsr__DOT__unnamedblk1__DOT__exponential_counter_period)))) {
                    __Vdly__v1__DOT__adsr__DOT__unnamedblk1__DOT__exponential_counter = 0U;
                    if ((1U == (IData)(vlSelfRef.__PVT__v1__DOT__adsr__DOT__state))) {
                        __Vdly__v1__DOT__envelope = 
                            (0x000000ffU & ((IData)(1U) 
                                            + (IData)(vlSelfRef.__PVT__v1__DOT__envelope)));
                        if ((0xfeU == (IData)(vlSelfRef.__PVT__v1__DOT__envelope))) {
                            __Vdly__v1__DOT__adsr__DOT__state = 2U;
                        }
                    } else if ((2U == (IData)(vlSelfRef.__PVT__v1__DOT__adsr__DOT__state))) {
                        if ((((IData)(vlSelfRef.__PVT__v1__DOT__envelope) 
                              != (0x000000ffU & ((0x000000f0U 
                                                  & vlSelfRef.__PVT__voice_sr[0U]) 
                                                 | (0x0000000fU 
                                                    & (vlSelfRef.__PVT__voice_sr[0U] 
                                                       >> 4U))))) 
                             & (~ (IData)(vlSelfRef.__PVT__v1__DOT__adsr__DOT__unnamedblk1__DOT__hold_zero)))) {
                            __Vdly__v1__DOT__envelope 
                                = (0x000000ffU & ((IData)(vlSelfRef.__PVT__v1__DOT__envelope) 
                                                  - (IData)(1U)));
                        }
                    } else if ((0U == (IData)(vlSelfRef.__PVT__v1__DOT__adsr__DOT__state))) {
                        if ((1U & (~ (IData)(vlSelfRef.__PVT__v1__DOT__adsr__DOT__unnamedblk1__DOT__hold_zero)))) {
                            __Vdly__v1__DOT__envelope 
                                = (0x000000ffU & ((IData)(vlSelfRef.__PVT__v1__DOT__envelope) 
                                                  - (IData)(1U)));
                        }
                    }
                    if (((1U != (IData)(vlSelfRef.__PVT__v1__DOT__adsr__DOT__state)) 
                         & (1U == (IData)(vlSelfRef.__PVT__v1__DOT__envelope)))) {
                        __Vdly__v1__DOT__adsr__DOT__unnamedblk1__DOT__hold_zero = 1U;
                    }
                }
            }
            if ((1U & ((~ (IData)(vlSelfRef.__PVT__v1__DOT__adsr__DOT__unnamedblk1__DOT__gate_edge)) 
                       & vlSelfRef.__PVT__voice_ctrl[0U]))) {
                __Vdly__v1__DOT__adsr__DOT__state = 1U;
                __Vdly__v1__DOT__adsr__DOT__unnamedblk1__DOT__hold_zero = 0U;
            }
            if (((IData)(vlSelfRef.__PVT__v1__DOT__adsr__DOT__unnamedblk1__DOT__gate_edge) 
                 & (~ vlSelfRef.__PVT__voice_ctrl[0U]))) {
                __Vdly__v1__DOT__adsr__DOT__state = 0U;
            }
            __Vdly__v2__DOT__adsr__DOT__unnamedblk1__DOT__rate_counter 
                = (0x00007fffU & ((IData)(1U) + (IData)(vlSelfRef.__PVT__v2__DOT__adsr__DOT__unnamedblk1__DOT__rate_counter)));
            if (((IData)(vlSelfRef.__PVT__v2__DOT__adsr__DOT__unnamedblk1__DOT__rate_counter) 
                 == (IData)(vlSelfRef.__PVT__v2__DOT__adsr__DOT__rate_period))) {
                __Vdly__v2__DOT__adsr__DOT__unnamedblk1__DOT__rate_counter = 0U;
                __Vdly__v2__DOT__adsr__DOT__unnamedblk1__DOT__exponential_counter 
                    = (0x0000001fU & ((IData)(1U) + (IData)(vlSelfRef.__PVT__v2__DOT__adsr__DOT__unnamedblk1__DOT__exponential_counter)));
                if (((1U == (IData)(vlSelfRef.__PVT__v2__DOT__adsr__DOT__state)) 
                     | ((IData)(vlSelfRef.__PVT__v2__DOT__adsr__DOT__unnamedblk1__DOT__exponential_counter) 
                        == (IData)(vlSelfRef.__PVT__v2__DOT__adsr__DOT__unnamedblk1__DOT__exponential_counter_period)))) {
                    __Vdly__v2__DOT__adsr__DOT__unnamedblk1__DOT__exponential_counter = 0U;
                    if ((1U == (IData)(vlSelfRef.__PVT__v2__DOT__adsr__DOT__state))) {
                        __Vdly__v2__DOT__envelope = 
                            (0x000000ffU & ((IData)(1U) 
                                            + (IData)(vlSelfRef.__PVT__v2__DOT__envelope)));
                        if ((0xfeU == (IData)(vlSelfRef.__PVT__v2__DOT__envelope))) {
                            __Vdly__v2__DOT__adsr__DOT__state = 2U;
                        }
                    } else if ((2U == (IData)(vlSelfRef.__PVT__v2__DOT__adsr__DOT__state))) {
                        if ((((IData)(vlSelfRef.__PVT__v2__DOT__envelope) 
                              != (0x000000ffU & ((0x000000f0U 
                                                  & vlSelfRef.__PVT__voice_sr[1U]) 
                                                 | (0x0000000fU 
                                                    & (vlSelfRef.__PVT__voice_sr[1U] 
                                                       >> 4U))))) 
                             & (~ (IData)(vlSelfRef.__PVT__v2__DOT__adsr__DOT__unnamedblk1__DOT__hold_zero)))) {
                            __Vdly__v2__DOT__envelope 
                                = (0x000000ffU & ((IData)(vlSelfRef.__PVT__v2__DOT__envelope) 
                                                  - (IData)(1U)));
                        }
                    } else if ((0U == (IData)(vlSelfRef.__PVT__v2__DOT__adsr__DOT__state))) {
                        if ((1U & (~ (IData)(vlSelfRef.__PVT__v2__DOT__adsr__DOT__unnamedblk1__DOT__hold_zero)))) {
                            __Vdly__v2__DOT__envelope 
                                = (0x000000ffU & ((IData)(vlSelfRef.__PVT__v2__DOT__envelope) 
                                                  - (IData)(1U)));
                        }
                    }
                    if (((1U != (IData)(vlSelfRef.__PVT__v2__DOT__adsr__DOT__state)) 
                         & (1U == (IData)(vlSelfRef.__PVT__v2__DOT__envelope)))) {
                        __Vdly__v2__DOT__adsr__DOT__unnamedblk1__DOT__hold_zero = 1U;
                    }
                }
            }
            if ((1U & ((~ (IData)(vlSelfRef.__PVT__v2__DOT__adsr__DOT__unnamedblk1__DOT__gate_edge)) 
                       & vlSelfRef.__PVT__voice_ctrl[1U]))) {
                __Vdly__v2__DOT__adsr__DOT__state = 1U;
                __Vdly__v2__DOT__adsr__DOT__unnamedblk1__DOT__hold_zero = 0U;
            }
            if (((IData)(vlSelfRef.__PVT__v2__DOT__adsr__DOT__unnamedblk1__DOT__gate_edge) 
                 & (~ vlSelfRef.__PVT__voice_ctrl[1U]))) {
                __Vdly__v2__DOT__adsr__DOT__state = 0U;
            }
            __Vdly__v3__DOT__adsr__DOT__unnamedblk1__DOT__rate_counter 
                = (0x00007fffU & ((IData)(1U) + (IData)(vlSelfRef.__PVT__v3__DOT__adsr__DOT__unnamedblk1__DOT__rate_counter)));
            if (((IData)(vlSelfRef.__PVT__v3__DOT__adsr__DOT__unnamedblk1__DOT__rate_counter) 
                 == (IData)(vlSelfRef.__PVT__v3__DOT__adsr__DOT__rate_period))) {
                __Vdly__v3__DOT__adsr__DOT__unnamedblk1__DOT__rate_counter = 0U;
                __Vdly__v3__DOT__adsr__DOT__unnamedblk1__DOT__exponential_counter 
                    = (0x0000001fU & ((IData)(1U) + (IData)(vlSelfRef.__PVT__v3__DOT__adsr__DOT__unnamedblk1__DOT__exponential_counter)));
                if (((1U == (IData)(vlSelfRef.__PVT__v3__DOT__adsr__DOT__state)) 
                     | ((IData)(vlSelfRef.__PVT__v3__DOT__adsr__DOT__unnamedblk1__DOT__exponential_counter) 
                        == (IData)(vlSelfRef.__PVT__v3__DOT__adsr__DOT__unnamedblk1__DOT__exponential_counter_period)))) {
                    __Vdly__v3__DOT__adsr__DOT__unnamedblk1__DOT__exponential_counter = 0U;
                    if ((1U == (IData)(vlSelfRef.__PVT__v3__DOT__adsr__DOT__state))) {
                        __Vdly__v3__DOT__envelope = 
                            (0x000000ffU & ((IData)(1U) 
                                            + (IData)(vlSelfRef.__PVT__v3__DOT__envelope)));
                        if ((0xfeU == (IData)(vlSelfRef.__PVT__v3__DOT__envelope))) {
                            __Vdly__v3__DOT__adsr__DOT__state = 2U;
                        }
                    } else if ((2U == (IData)(vlSelfRef.__PVT__v3__DOT__adsr__DOT__state))) {
                        if ((((IData)(vlSelfRef.__PVT__v3__DOT__envelope) 
                              != (0x000000ffU & ((0x000000f0U 
                                                  & vlSelfRef.__PVT__voice_sr[2U]) 
                                                 | (0x0000000fU 
                                                    & (vlSelfRef.__PVT__voice_sr[2U] 
                                                       >> 4U))))) 
                             & (~ (IData)(vlSelfRef.__PVT__v3__DOT__adsr__DOT__unnamedblk1__DOT__hold_zero)))) {
                            __Vdly__v3__DOT__envelope 
                                = (0x000000ffU & ((IData)(vlSelfRef.__PVT__v3__DOT__envelope) 
                                                  - (IData)(1U)));
                        }
                    } else if ((0U == (IData)(vlSelfRef.__PVT__v3__DOT__adsr__DOT__state))) {
                        if ((1U & (~ (IData)(vlSelfRef.__PVT__v3__DOT__adsr__DOT__unnamedblk1__DOT__hold_zero)))) {
                            __Vdly__v3__DOT__envelope 
                                = (0x000000ffU & ((IData)(vlSelfRef.__PVT__v3__DOT__envelope) 
                                                  - (IData)(1U)));
                        }
                    }
                    if (((1U != (IData)(vlSelfRef.__PVT__v3__DOT__adsr__DOT__state)) 
                         & (1U == (IData)(vlSelfRef.__PVT__v3__DOT__envelope)))) {
                        __Vdly__v3__DOT__adsr__DOT__unnamedblk1__DOT__hold_zero = 1U;
                    }
                }
            }
            if ((1U & ((~ (IData)(vlSelfRef.__PVT__v3__DOT__adsr__DOT__unnamedblk1__DOT__gate_edge)) 
                       & vlSelfRef.__PVT__voice_ctrl[2U]))) {
                __Vdly__v3__DOT__adsr__DOT__state = 1U;
                __Vdly__v3__DOT__adsr__DOT__unnamedblk1__DOT__hold_zero = 0U;
            }
            if (((IData)(vlSelfRef.__PVT__v3__DOT__adsr__DOT__unnamedblk1__DOT__gate_edge) 
                 & (~ vlSelfRef.__PVT__voice_ctrl[2U]))) {
                __Vdly__v3__DOT__adsr__DOT__state = 0U;
            }
            vlSelfRef.__PVT__v1__DOT__saw_tri = vlSelfRef.__Vcellout__v1__acc_t;
            vlSelfRef.__PVT__v2__DOT__saw_tri = vlSelfRef.__Vcellout__v2__acc_t;
            vlSelfRef.__PVT__v3__DOT__saw_tri = vlSelfRef.__Vcellout__v3__acc_t;
            vlSelfRef.__PVT__v1__DOT__pulse = (1U & 
                                               ((vlSelfRef.__PVT__voice_ctrl[0U] 
                                                 >> 3U) 
                                                | ((0x00000fffU 
                                                    & (vlSelfRef.__PVT__v1__DOT__oscillator 
                                                       >> 0x0cU)) 
                                                   >= vlSelfRef.__PVT__voice_pw[0U])));
            vlSelfRef.__PVT__v2__DOT__pulse = (1U & 
                                               ((vlSelfRef.__PVT__voice_ctrl[1U] 
                                                 >> 3U) 
                                                | ((0x00000fffU 
                                                    & (vlSelfRef.__PVT__v2__DOT__oscillator 
                                                       >> 0x0cU)) 
                                                   >= vlSelfRef.__PVT__voice_pw[1U])));
            vlSelfRef.__PVT__v3__DOT__pulse = (1U & 
                                               ((vlSelfRef.__PVT__voice_ctrl[2U] 
                                                 >> 3U) 
                                                | ((0x00000fffU 
                                                    & (vlSelfRef.__PVT__v3__DOT__oscillator 
                                                       >> 0x0cU)) 
                                                   >= vlSelfRef.__PVT__voice_pw[2U])));
            vlSelfRef.__PVT__v1__DOT__noise = (((((4U 
                                                   & (vlSelfRef.__PVT__v1__DOT__unnamedblk2__DOT__lfsr_noise 
                                                      >> 0x00000012U)) 
                                                  | ((2U 
                                                      & (vlSelfRef.__PVT__v1__DOT__unnamedblk2__DOT__lfsr_noise 
                                                         >> 0x00000011U)) 
                                                     | (1U 
                                                        & (vlSelfRef.__PVT__v1__DOT__unnamedblk2__DOT__lfsr_noise 
                                                           >> 0x0eU)))) 
                                                 << 9U) 
                                                | (((4U 
                                                     & (vlSelfRef.__PVT__v1__DOT__unnamedblk2__DOT__lfsr_noise 
                                                        >> 9U)) 
                                                    | ((2U 
                                                        & (vlSelfRef.__PVT__v1__DOT__unnamedblk2__DOT__lfsr_noise 
                                                           >> 8U)) 
                                                       | (1U 
                                                          & (vlSelfRef.__PVT__v1__DOT__unnamedblk2__DOT__lfsr_noise 
                                                             >> 5U)))) 
                                                   << 6U)) 
                                               | ((0x00000020U 
                                                   & (vlSelfRef.__PVT__v1__DOT__unnamedblk2__DOT__lfsr_noise 
                                                      << 3U)) 
                                                  | (0x00000010U 
                                                     & (vlSelfRef.__PVT__v1__DOT__unnamedblk2__DOT__lfsr_noise 
                                                        << 4U))));
            if (vlSelfRef.__PVT__v1__DOT__unnamedblk2__DOT__clk) {
                if (((IData)(vlSelfRef.__PVT__v1__DOT__unnamedblk2__DOT__clk) 
                     & (~ (IData)(vlSelfRef.__PVT__v1__DOT__unnamedblk2__DOT__clk_d)))) {
                    __Vdly__v1__DOT__unnamedblk2__DOT__lfsr_noise 
                        = ((0x007ffffeU & (vlSelfRef.__PVT__v1__DOT__unnamedblk2__DOT__lfsr_noise 
                                           << 1U)) 
                           | (1U & ((((IData)(vlSymsp->TOP.rst) 
                                      | (vlSelfRef.__PVT__voice_ctrl[0U] 
                                         >> 3U)) | 
                                     (vlSelfRef.__PVT__v1__DOT__unnamedblk2__DOT__lfsr_noise 
                                      >> 0x16U)) ^ 
                                    (vlSelfRef.__PVT__v1__DOT__unnamedblk2__DOT__lfsr_noise 
                                     >> 0x11U))));
                } else if (((vlSelfRef.__PVT__voice_ctrl[0U] 
                             >> 7U) & (0U != (7U & 
                                              (vlSelfRef.__PVT__voice_ctrl[0U] 
                                               >> 4U))))) {
                    __Vdly__v1__DOT__unnamedblk2__DOT__lfsr_noise 
                        = ((0x006fffffU & __Vdly__v1__DOT__unnamedblk2__DOT__lfsr_noise) 
                           | ((IData)(((vlSelfRef.__PVT__v1__DOT__unnamedblk2__DOT__lfsr_noise 
                                        >> 0x00000014U) 
                                       & ((IData)(vlSelfRef.__PVT__v1__DOT__wave_out) 
                                          >> 7U))) 
                              << 0x00000014U));
                    __Vdly__v1__DOT__unnamedblk2__DOT__lfsr_noise 
                        = ((0x007bffffU & __Vdly__v1__DOT__unnamedblk2__DOT__lfsr_noise) 
                           | (0x00040000U & (vlSelfRef.__PVT__v1__DOT__unnamedblk2__DOT__lfsr_noise 
                                             & ((IData)(vlSelfRef.__PVT__v1__DOT__wave_out) 
                                                << 0x0000000cU))));
                    __Vdly__v1__DOT__unnamedblk2__DOT__lfsr_noise 
                        = ((0x007fbfffU & __Vdly__v1__DOT__unnamedblk2__DOT__lfsr_noise) 
                           | (0x00004000U & (vlSelfRef.__PVT__v1__DOT__unnamedblk2__DOT__lfsr_noise 
                                             & ((IData)(vlSelfRef.__PVT__v1__DOT__wave_out) 
                                                << 9U))));
                    __Vdly__v1__DOT__unnamedblk2__DOT__lfsr_noise 
                        = ((0x007ff7ffU & __Vdly__v1__DOT__unnamedblk2__DOT__lfsr_noise) 
                           | (0x00000800U & (vlSelfRef.__PVT__v1__DOT__unnamedblk2__DOT__lfsr_noise 
                                             & ((IData)(vlSelfRef.__PVT__v1__DOT__wave_out) 
                                                << 7U))));
                    __Vdly__v1__DOT__unnamedblk2__DOT__lfsr_noise 
                        = ((0x007ffdffU & __Vdly__v1__DOT__unnamedblk2__DOT__lfsr_noise) 
                           | (0x00000200U & (vlSelfRef.__PVT__v1__DOT__unnamedblk2__DOT__lfsr_noise 
                                             & ((IData)(vlSelfRef.__PVT__v1__DOT__wave_out) 
                                                << 6U))));
                    __Vdly__v1__DOT__unnamedblk2__DOT__lfsr_noise 
                        = ((0x007fffdfU & __Vdly__v1__DOT__unnamedblk2__DOT__lfsr_noise) 
                           | (0x00000020U & (vlSelfRef.__PVT__v1__DOT__unnamedblk2__DOT__lfsr_noise 
                                             & ((IData)(vlSelfRef.__PVT__v1__DOT__wave_out) 
                                                << 3U))));
                    __Vdly__v1__DOT__unnamedblk2__DOT__lfsr_noise 
                        = ((0x007ffffbU & __Vdly__v1__DOT__unnamedblk2__DOT__lfsr_noise) 
                           | (4U & (vlSelfRef.__PVT__v1__DOT__unnamedblk2__DOT__lfsr_noise 
                                    & ((IData)(vlSelfRef.__PVT__v1__DOT__wave_out) 
                                       << 1U))));
                    __Vdly__v1__DOT__unnamedblk2__DOT__lfsr_noise 
                        = ((0x007ffffeU & __Vdly__v1__DOT__unnamedblk2__DOT__lfsr_noise) 
                           | (1U & (vlSelfRef.__PVT__v1__DOT__unnamedblk2__DOT__lfsr_noise 
                                    & (IData)(vlSelfRef.__PVT__v1__DOT__wave_out))));
                }
                __Vdly__v1__DOT__unnamedblk2__DOT__noise_age = 0U;
                vlSelfRef.__PVT__v1__DOT__unnamedblk2__DOT__clk_d = 1U;
            } else {
                if ((vlSelfRef.__PVT__v1__DOT__unnamedblk2__DOT__noise_age 
                     >= ((2U & (IData)(vlSymsp->TOP.top__DOT__sid_cfg_reg))
                          ? 0x00950000U : 0x00008000U))) {
                    vlSelfRef.__PVT__v1__DOT__noise = 0x0fffU;
                } else {
                    __Vdly__v1__DOT__unnamedblk2__DOT__noise_age 
                        = (0x00ffffffU & ((IData)(1U) 
                                          + vlSelfRef.__PVT__v1__DOT__unnamedblk2__DOT__noise_age));
                }
                vlSelfRef.__PVT__v1__DOT__unnamedblk2__DOT__clk_d = 0U;
            }
        }
    }
    if (vlSymsp->TOP.top__DOT__sid_ce_1m) {
        vlSelfRef.__PVT__v1__DOT__wave_out = (0x000000ffU 
                                              & (((IData)(vlSelfRef.__PVT__v1__DOT__norm) 
                                                  >> 4U) 
                                                 | (IData)(vlSelfRef.__PVT__v1__DOT__comb)));
    }
    if (vlSymsp->TOP.rst) {
        vlSelfRef.__PVT__v2__DOT__noise = 0U;
        __Vdly__v2__DOT__unnamedblk2__DOT__lfsr_noise = 0x007fffffU;
        __Vdly__v2__DOT__unnamedblk2__DOT__noise_age = 0U;
    } else if (vlSymsp->TOP.top__DOT__sid_ce_1m) {
        vlSelfRef.__PVT__v2__DOT__noise = (((((4U & 
                                               (vlSelfRef.__PVT__v2__DOT__unnamedblk2__DOT__lfsr_noise 
                                                >> 0x00000012U)) 
                                              | ((2U 
                                                  & (vlSelfRef.__PVT__v2__DOT__unnamedblk2__DOT__lfsr_noise 
                                                     >> 0x00000011U)) 
                                                 | (1U 
                                                    & (vlSelfRef.__PVT__v2__DOT__unnamedblk2__DOT__lfsr_noise 
                                                       >> 0x0eU)))) 
                                             << 9U) 
                                            | (((4U 
                                                 & (vlSelfRef.__PVT__v2__DOT__unnamedblk2__DOT__lfsr_noise 
                                                    >> 9U)) 
                                                | ((2U 
                                                    & (vlSelfRef.__PVT__v2__DOT__unnamedblk2__DOT__lfsr_noise 
                                                       >> 8U)) 
                                                   | (1U 
                                                      & (vlSelfRef.__PVT__v2__DOT__unnamedblk2__DOT__lfsr_noise 
                                                         >> 5U)))) 
                                               << 6U)) 
                                           | ((0x00000020U 
                                               & (vlSelfRef.__PVT__v2__DOT__unnamedblk2__DOT__lfsr_noise 
                                                  << 3U)) 
                                              | (0x00000010U 
                                                 & (vlSelfRef.__PVT__v2__DOT__unnamedblk2__DOT__lfsr_noise 
                                                    << 4U))));
        if (vlSelfRef.__PVT__v2__DOT__unnamedblk2__DOT__clk) {
            if (((IData)(vlSelfRef.__PVT__v2__DOT__unnamedblk2__DOT__clk) 
                 & (~ (IData)(vlSelfRef.__PVT__v2__DOT__unnamedblk2__DOT__clk_d)))) {
                __Vdly__v2__DOT__unnamedblk2__DOT__lfsr_noise 
                    = ((0x007ffffeU & (vlSelfRef.__PVT__v2__DOT__unnamedblk2__DOT__lfsr_noise 
                                       << 1U)) | (1U 
                                                  & ((((IData)(vlSymsp->TOP.rst) 
                                                       | (vlSelfRef.__PVT__voice_ctrl[1U] 
                                                          >> 3U)) 
                                                      | (vlSelfRef.__PVT__v2__DOT__unnamedblk2__DOT__lfsr_noise 
                                                         >> 0x16U)) 
                                                     ^ 
                                                     (vlSelfRef.__PVT__v2__DOT__unnamedblk2__DOT__lfsr_noise 
                                                      >> 0x11U))));
            } else if (((vlSelfRef.__PVT__voice_ctrl[1U] 
                         >> 7U) & (0U != (7U & (vlSelfRef.__PVT__voice_ctrl[1U] 
                                                >> 4U))))) {
                __Vdly__v2__DOT__unnamedblk2__DOT__lfsr_noise 
                    = ((0x006fffffU & __Vdly__v2__DOT__unnamedblk2__DOT__lfsr_noise) 
                       | ((IData)(((vlSelfRef.__PVT__v2__DOT__unnamedblk2__DOT__lfsr_noise 
                                    >> 0x00000014U) 
                                   & ((IData)(vlSelfRef.__PVT__v2__DOT__wave_out) 
                                      >> 7U))) << 0x00000014U));
                __Vdly__v2__DOT__unnamedblk2__DOT__lfsr_noise 
                    = ((0x007bffffU & __Vdly__v2__DOT__unnamedblk2__DOT__lfsr_noise) 
                       | (0x00040000U & (vlSelfRef.__PVT__v2__DOT__unnamedblk2__DOT__lfsr_noise 
                                         & ((IData)(vlSelfRef.__PVT__v2__DOT__wave_out) 
                                            << 0x0000000cU))));
                __Vdly__v2__DOT__unnamedblk2__DOT__lfsr_noise 
                    = ((0x007fbfffU & __Vdly__v2__DOT__unnamedblk2__DOT__lfsr_noise) 
                       | (0x00004000U & (vlSelfRef.__PVT__v2__DOT__unnamedblk2__DOT__lfsr_noise 
                                         & ((IData)(vlSelfRef.__PVT__v2__DOT__wave_out) 
                                            << 9U))));
                __Vdly__v2__DOT__unnamedblk2__DOT__lfsr_noise 
                    = ((0x007ff7ffU & __Vdly__v2__DOT__unnamedblk2__DOT__lfsr_noise) 
                       | (0x00000800U & (vlSelfRef.__PVT__v2__DOT__unnamedblk2__DOT__lfsr_noise 
                                         & ((IData)(vlSelfRef.__PVT__v2__DOT__wave_out) 
                                            << 7U))));
                __Vdly__v2__DOT__unnamedblk2__DOT__lfsr_noise 
                    = ((0x007ffdffU & __Vdly__v2__DOT__unnamedblk2__DOT__lfsr_noise) 
                       | (0x00000200U & (vlSelfRef.__PVT__v2__DOT__unnamedblk2__DOT__lfsr_noise 
                                         & ((IData)(vlSelfRef.__PVT__v2__DOT__wave_out) 
                                            << 6U))));
                __Vdly__v2__DOT__unnamedblk2__DOT__lfsr_noise 
                    = ((0x007fffdfU & __Vdly__v2__DOT__unnamedblk2__DOT__lfsr_noise) 
                       | (0x00000020U & (vlSelfRef.__PVT__v2__DOT__unnamedblk2__DOT__lfsr_noise 
                                         & ((IData)(vlSelfRef.__PVT__v2__DOT__wave_out) 
                                            << 3U))));
                __Vdly__v2__DOT__unnamedblk2__DOT__lfsr_noise 
                    = ((0x007ffffbU & __Vdly__v2__DOT__unnamedblk2__DOT__lfsr_noise) 
                       | (4U & (vlSelfRef.__PVT__v2__DOT__unnamedblk2__DOT__lfsr_noise 
                                & ((IData)(vlSelfRef.__PVT__v2__DOT__wave_out) 
                                   << 1U))));
                __Vdly__v2__DOT__unnamedblk2__DOT__lfsr_noise 
                    = ((0x007ffffeU & __Vdly__v2__DOT__unnamedblk2__DOT__lfsr_noise) 
                       | (1U & (vlSelfRef.__PVT__v2__DOT__unnamedblk2__DOT__lfsr_noise 
                                & (IData)(vlSelfRef.__PVT__v2__DOT__wave_out))));
            }
            __Vdly__v2__DOT__unnamedblk2__DOT__noise_age = 0U;
            vlSelfRef.__PVT__v2__DOT__unnamedblk2__DOT__clk_d = 1U;
        } else {
            if ((vlSelfRef.__PVT__v2__DOT__unnamedblk2__DOT__noise_age 
                 >= ((2U & (IData)(vlSymsp->TOP.top__DOT__sid_cfg_reg))
                      ? 0x00950000U : 0x00008000U))) {
                vlSelfRef.__PVT__v2__DOT__noise = 0x0fffU;
            } else {
                __Vdly__v2__DOT__unnamedblk2__DOT__noise_age 
                    = (0x00ffffffU & ((IData)(1U) + vlSelfRef.__PVT__v2__DOT__unnamedblk2__DOT__noise_age));
            }
            vlSelfRef.__PVT__v2__DOT__unnamedblk2__DOT__clk_d = 0U;
        }
    }
    if (vlSymsp->TOP.top__DOT__sid_ce_1m) {
        vlSelfRef.__PVT__v2__DOT__wave_out = (0x000000ffU 
                                              & (((IData)(vlSelfRef.__PVT__v2__DOT__norm) 
                                                  >> 4U) 
                                                 | (IData)(vlSelfRef.__PVT__v2__DOT__comb)));
    }
    if (vlSymsp->TOP.rst) {
        vlSelfRef.__PVT__v3__DOT__noise = 0U;
        __Vdly__v3__DOT__unnamedblk2__DOT__lfsr_noise = 0x007fffffU;
        __Vdly__v3__DOT__unnamedblk2__DOT__noise_age = 0U;
    } else if (vlSymsp->TOP.top__DOT__sid_ce_1m) {
        vlSelfRef.__PVT__v3__DOT__noise = (((((4U & 
                                               (vlSelfRef.__PVT__v3__DOT__unnamedblk2__DOT__lfsr_noise 
                                                >> 0x00000012U)) 
                                              | ((2U 
                                                  & (vlSelfRef.__PVT__v3__DOT__unnamedblk2__DOT__lfsr_noise 
                                                     >> 0x00000011U)) 
                                                 | (1U 
                                                    & (vlSelfRef.__PVT__v3__DOT__unnamedblk2__DOT__lfsr_noise 
                                                       >> 0x0eU)))) 
                                             << 9U) 
                                            | (((4U 
                                                 & (vlSelfRef.__PVT__v3__DOT__unnamedblk2__DOT__lfsr_noise 
                                                    >> 9U)) 
                                                | ((2U 
                                                    & (vlSelfRef.__PVT__v3__DOT__unnamedblk2__DOT__lfsr_noise 
                                                       >> 8U)) 
                                                   | (1U 
                                                      & (vlSelfRef.__PVT__v3__DOT__unnamedblk2__DOT__lfsr_noise 
                                                         >> 5U)))) 
                                               << 6U)) 
                                           | ((0x00000020U 
                                               & (vlSelfRef.__PVT__v3__DOT__unnamedblk2__DOT__lfsr_noise 
                                                  << 3U)) 
                                              | (0x00000010U 
                                                 & (vlSelfRef.__PVT__v3__DOT__unnamedblk2__DOT__lfsr_noise 
                                                    << 4U))));
        if (vlSelfRef.__PVT__v3__DOT__unnamedblk2__DOT__clk) {
            if (((IData)(vlSelfRef.__PVT__v3__DOT__unnamedblk2__DOT__clk) 
                 & (~ (IData)(vlSelfRef.__PVT__v3__DOT__unnamedblk2__DOT__clk_d)))) {
                __Vdly__v3__DOT__unnamedblk2__DOT__lfsr_noise 
                    = ((0x007ffffeU & (vlSelfRef.__PVT__v3__DOT__unnamedblk2__DOT__lfsr_noise 
                                       << 1U)) | (1U 
                                                  & ((((IData)(vlSymsp->TOP.rst) 
                                                       | (vlSelfRef.__PVT__voice_ctrl[2U] 
                                                          >> 3U)) 
                                                      | (vlSelfRef.__PVT__v3__DOT__unnamedblk2__DOT__lfsr_noise 
                                                         >> 0x16U)) 
                                                     ^ 
                                                     (vlSelfRef.__PVT__v3__DOT__unnamedblk2__DOT__lfsr_noise 
                                                      >> 0x11U))));
            } else if (((vlSelfRef.__PVT__voice_ctrl[2U] 
                         >> 7U) & (0U != (7U & (vlSelfRef.__PVT__voice_ctrl[2U] 
                                                >> 4U))))) {
                __Vdly__v3__DOT__unnamedblk2__DOT__lfsr_noise 
                    = ((0x006fffffU & __Vdly__v3__DOT__unnamedblk2__DOT__lfsr_noise) 
                       | ((IData)(((vlSelfRef.__PVT__v3__DOT__unnamedblk2__DOT__lfsr_noise 
                                    >> 0x00000014U) 
                                   & ((IData)(vlSelfRef.__PVT__v3__DOT__wave_out) 
                                      >> 7U))) << 0x00000014U));
                __Vdly__v3__DOT__unnamedblk2__DOT__lfsr_noise 
                    = ((0x007bffffU & __Vdly__v3__DOT__unnamedblk2__DOT__lfsr_noise) 
                       | (0x00040000U & (vlSelfRef.__PVT__v3__DOT__unnamedblk2__DOT__lfsr_noise 
                                         & ((IData)(vlSelfRef.__PVT__v3__DOT__wave_out) 
                                            << 0x0000000cU))));
                __Vdly__v3__DOT__unnamedblk2__DOT__lfsr_noise 
                    = ((0x007fbfffU & __Vdly__v3__DOT__unnamedblk2__DOT__lfsr_noise) 
                       | (0x00004000U & (vlSelfRef.__PVT__v3__DOT__unnamedblk2__DOT__lfsr_noise 
                                         & ((IData)(vlSelfRef.__PVT__v3__DOT__wave_out) 
                                            << 9U))));
                __Vdly__v3__DOT__unnamedblk2__DOT__lfsr_noise 
                    = ((0x007ff7ffU & __Vdly__v3__DOT__unnamedblk2__DOT__lfsr_noise) 
                       | (0x00000800U & (vlSelfRef.__PVT__v3__DOT__unnamedblk2__DOT__lfsr_noise 
                                         & ((IData)(vlSelfRef.__PVT__v3__DOT__wave_out) 
                                            << 7U))));
                __Vdly__v3__DOT__unnamedblk2__DOT__lfsr_noise 
                    = ((0x007ffdffU & __Vdly__v3__DOT__unnamedblk2__DOT__lfsr_noise) 
                       | (0x00000200U & (vlSelfRef.__PVT__v3__DOT__unnamedblk2__DOT__lfsr_noise 
                                         & ((IData)(vlSelfRef.__PVT__v3__DOT__wave_out) 
                                            << 6U))));
                __Vdly__v3__DOT__unnamedblk2__DOT__lfsr_noise 
                    = ((0x007fffdfU & __Vdly__v3__DOT__unnamedblk2__DOT__lfsr_noise) 
                       | (0x00000020U & (vlSelfRef.__PVT__v3__DOT__unnamedblk2__DOT__lfsr_noise 
                                         & ((IData)(vlSelfRef.__PVT__v3__DOT__wave_out) 
                                            << 3U))));
                __Vdly__v3__DOT__unnamedblk2__DOT__lfsr_noise 
                    = ((0x007ffffbU & __Vdly__v3__DOT__unnamedblk2__DOT__lfsr_noise) 
                       | (4U & (vlSelfRef.__PVT__v3__DOT__unnamedblk2__DOT__lfsr_noise 
                                & ((IData)(vlSelfRef.__PVT__v3__DOT__wave_out) 
                                   << 1U))));
                __Vdly__v3__DOT__unnamedblk2__DOT__lfsr_noise 
                    = ((0x007ffffeU & __Vdly__v3__DOT__unnamedblk2__DOT__lfsr_noise) 
                       | (1U & (vlSelfRef.__PVT__v3__DOT__unnamedblk2__DOT__lfsr_noise 
                                & (IData)(vlSelfRef.__PVT__v3__DOT__wave_out))));
            }
            __Vdly__v3__DOT__unnamedblk2__DOT__noise_age = 0U;
            vlSelfRef.__PVT__v3__DOT__unnamedblk2__DOT__clk_d = 1U;
        } else {
            if ((vlSelfRef.__PVT__v3__DOT__unnamedblk2__DOT__noise_age 
                 >= ((2U & (IData)(vlSymsp->TOP.top__DOT__sid_cfg_reg))
                      ? 0x00950000U : 0x00008000U))) {
                vlSelfRef.__PVT__v3__DOT__noise = 0x0fffU;
            } else {
                __Vdly__v3__DOT__unnamedblk2__DOT__noise_age 
                    = (0x00ffffffU & ((IData)(1U) + vlSelfRef.__PVT__v3__DOT__unnamedblk2__DOT__noise_age));
            }
            vlSelfRef.__PVT__v3__DOT__unnamedblk2__DOT__clk_d = 0U;
        }
    }
    if (vlSymsp->TOP.top__DOT__sid_ce_1m) {
        vlSelfRef.__PVT__v3__DOT__wave_out = (0x000000ffU 
                                              & (((IData)(vlSelfRef.__PVT__v3__DOT__norm) 
                                                  >> 4U) 
                                                 | (IData)(vlSelfRef.__PVT__v3__DOT__comb)));
        if ((2U & (IData)(vlSymsp->TOP.top__DOT__sid_cfg_reg))) {
            vlSelfRef.__PVT__v1__DOT__unnamedblk3__DOT__env_dac 
                = vlSelfRef.__PVT__v1__DOT__envelope;
            vlSelfRef.__PVT__v2__DOT__unnamedblk3__DOT__env_dac 
                = vlSelfRef.__PVT__v2__DOT__envelope;
            vlSelfRef.__PVT__v3__DOT__unnamedblk3__DOT__env_dac 
                = vlSelfRef.__PVT__v3__DOT__envelope;
        } else {
            vlSelfRef.__PVT__v1__DOT__unnamedblk3__DOT__env_dac 
                = (0x000000ffU & (vlSelfRef.__PVT__v1__DOT__envelope_dac__DOT__bitsum[7U] 
                                  >> 4U));
            vlSelfRef.__PVT__v2__DOT__unnamedblk3__DOT__env_dac 
                = (0x000000ffU & (vlSelfRef.__PVT__v2__DOT__envelope_dac__DOT__bitsum[7U] 
                                  >> 4U));
            vlSelfRef.__PVT__v3__DOT__unnamedblk3__DOT__env_dac 
                = (0x000000ffU & (vlSelfRef.__PVT__v3__DOT__envelope_dac__DOT__bitsum[7U] 
                                  >> 4U));
        }
        if ((0U != (0x0000000fU & (vlSelfRef.__PVT__voice_ctrl[0U] 
                                   >> 4U)))) {
            __Vdly__v1__DOT__unnamedblk3__DOT__keep_cnt 
                = ((2U & (IData)(vlSymsp->TOP.top__DOT__sid_cfg_reg))
                    ? 0x00030d40U : 0x004c4b40U);
            vlSelfRef.__PVT__v1__DOT__unnamedblk3__DOT__dac_out 
                = (0x00001fffU & (((IData)(vlSelfRef.__PVT__v1__DOT__norm_dac) 
                                   | ((IData)(vlSelfRef.__PVT__v1__DOT__comb) 
                                      << 4U)) - ((2U 
                                                  & (IData)(vlSymsp->TOP.top__DOT__sid_cfg_reg))
                                                  ? 0x0800U
                                                  : 0x0380U)));
        } else if ((0U != vlSelfRef.__PVT__v1__DOT__unnamedblk3__DOT__keep_cnt)) {
            __Vdly__v1__DOT__unnamedblk3__DOT__keep_cnt 
                = (0x00ffffffU & (vlSelfRef.__PVT__v1__DOT__unnamedblk3__DOT__keep_cnt 
                                  - (IData)(1U)));
        } else {
            vlSelfRef.__PVT__v1__DOT__unnamedblk3__DOT__dac_out = 0U;
        }
        if ((0U != (0x0000000fU & (vlSelfRef.__PVT__voice_ctrl[1U] 
                                   >> 4U)))) {
            __Vdly__v2__DOT__unnamedblk3__DOT__keep_cnt 
                = ((2U & (IData)(vlSymsp->TOP.top__DOT__sid_cfg_reg))
                    ? 0x00030d40U : 0x004c4b40U);
            vlSelfRef.__PVT__v2__DOT__unnamedblk3__DOT__dac_out 
                = (0x00001fffU & (((IData)(vlSelfRef.__PVT__v2__DOT__norm_dac) 
                                   | ((IData)(vlSelfRef.__PVT__v2__DOT__comb) 
                                      << 4U)) - ((2U 
                                                  & (IData)(vlSymsp->TOP.top__DOT__sid_cfg_reg))
                                                  ? 0x0800U
                                                  : 0x0380U)));
        } else if ((0U != vlSelfRef.__PVT__v2__DOT__unnamedblk3__DOT__keep_cnt)) {
            __Vdly__v2__DOT__unnamedblk3__DOT__keep_cnt 
                = (0x00ffffffU & (vlSelfRef.__PVT__v2__DOT__unnamedblk3__DOT__keep_cnt 
                                  - (IData)(1U)));
        } else {
            vlSelfRef.__PVT__v2__DOT__unnamedblk3__DOT__dac_out = 0U;
        }
        if ((0U != (0x0000000fU & (vlSelfRef.__PVT__voice_ctrl[2U] 
                                   >> 4U)))) {
            __Vdly__v3__DOT__unnamedblk3__DOT__keep_cnt 
                = ((2U & (IData)(vlSymsp->TOP.top__DOT__sid_cfg_reg))
                    ? 0x00030d40U : 0x004c4b40U);
            vlSelfRef.__PVT__v3__DOT__unnamedblk3__DOT__dac_out 
                = (0x00001fffU & (((IData)(vlSelfRef.__PVT__v3__DOT__norm_dac) 
                                   | ((IData)(vlSelfRef.__PVT__v3__DOT__comb) 
                                      << 4U)) - ((2U 
                                                  & (IData)(vlSymsp->TOP.top__DOT__sid_cfg_reg))
                                                  ? 0x0800U
                                                  : 0x0380U)));
        } else if ((0U != vlSelfRef.__PVT__v3__DOT__unnamedblk3__DOT__keep_cnt)) {
            __Vdly__v3__DOT__unnamedblk3__DOT__keep_cnt 
                = (0x00ffffffU & (vlSelfRef.__PVT__v3__DOT__unnamedblk3__DOT__keep_cnt 
                                  - (IData)(1U)));
        } else {
            vlSelfRef.__PVT__v3__DOT__unnamedblk3__DOT__dac_out = 0U;
        }
    }
    if ((((1U == (IData)(vlSelfRef.__PVT__tbl_state)) 
          || (3U == (IData)(vlSelfRef.__PVT__tbl_state))) 
         || (5U == (IData)(vlSelfRef.__PVT__tbl_state)))) {
        __Vdly__f_acc_t = ((2U >= (3U & ((IData)(vlSelfRef.__PVT__tbl_state) 
                                         >> 1U))) ? vlSelfRef.__PVT__acc_t
                           [(3U & ((IData)(vlSelfRef.__PVT__tbl_state) 
                                   >> 1U))] : 0U);
    }
    if ((((3U == (IData)(vlSelfRef.__PVT__tbl_state)) 
          || (5U == (IData)(vlSelfRef.__PVT__tbl_state))) 
         || (7U == (IData)(vlSelfRef.__PVT__tbl_state)))) {
        vlSelfRef.__Vlvbound_h9cc45699__0 = vlSelfRef.__PVT__f___05Fst_out;
        vlSelfRef.__Vlvbound_h49d001ad__0 = vlSelfRef.__PVT__f_p_t_out;
        vlSelfRef.__Vlvbound_h061f25e8__0 = vlSelfRef.__PVT__f_ps___05Fout;
        vlSelfRef.__Vlvbound_h0706378b__0 = vlSelfRef.__PVT__f_pst_out;
        if ((5U >= (7U & ((3U & ((IData)(vlSelfRef.__PVT__tbl_state) 
                                 >> 1U)) - (IData)(1U))))) {
            __VdlyVal___st_out__v0 = vlSelfRef.__Vlvbound_h9cc45699__0;
            __VdlyDim0___st_out__v0 = (7U & ((3U & 
                                              ((IData)(vlSelfRef.__PVT__tbl_state) 
                                               >> 1U)) 
                                             - (IData)(1U)));
            __VdlySet___st_out__v0 = 1U;
            __VdlyVal__p_t_out__v0 = vlSelfRef.__Vlvbound_h49d001ad__0;
            __VdlyDim0__p_t_out__v0 = (7U & ((3U & 
                                              ((IData)(vlSelfRef.__PVT__tbl_state) 
                                               >> 1U)) 
                                             - (IData)(1U)));
            __VdlySet__p_t_out__v0 = 1U;
            __VdlyVal__ps___05Fout__v0 = vlSelfRef.__Vlvbound_h061f25e8__0;
            __VdlyDim0__ps___05Fout__v0 = (7U & ((3U 
                                                  & ((IData)(vlSelfRef.__PVT__tbl_state) 
                                                     >> 1U)) 
                                                 - (IData)(1U)));
            __VdlySet__ps___05Fout__v0 = 1U;
            __VdlyVal__pst_out__v0 = vlSelfRef.__Vlvbound_h0706378b__0;
            __VdlyDim0__pst_out__v0 = (7U & ((3U & 
                                              ((IData)(vlSelfRef.__PVT__tbl_state) 
                                               >> 1U)) 
                                             - (IData)(1U)));
            __VdlySet__pst_out__v0 = 1U;
        }
    }
    if ((6U == (IData)(vlSelfRef.__PVT__tbl_state))) {
        vlSelfRef.audio_out = (0x0003ffffU & (vlSelfRef.__PVT__flt__DOT__o 
                                              >> 2U));
    }
    if ((2U == (7U & (IData)(vlSelfRef.__PVT__tbl_state)))) {
        vlSelfRef.__PVT__flt__DOT__s = 1U;
        vlSelfRef.__PVT__flt__DOT__c = 0U;
        vlSelfRef.__PVT__flt__DOT__b = vlSelfRef.__PVT__flt__DOT__vbp;
        vlSelfRef.__PVT__flt__DOT__a = (0x0000ffffU 
                                        & ((2U & (IData)(vlSymsp->TOP.top__DOT__sid_cfg_reg))
                                            ? (((IData)(vlSelfRef.__PVT__filter_fc) 
                                                << 2U) 
                                               + (IData)(vlSelfRef.__PVT__filter_fc))
                                            : VL_SHIFTR_III(16,16,32, (IData)(vlSelfRef.__PVT__tbl__DOT__f0), 1U)));
        vlSelfRef.__PVT__flt__DOT__unnamedblk1__DOT__vi 
            = (0x0000ffffU & ((((((1U & (IData)(vlSelfRef.__PVT__filter_res_filt))
                                   ? VL_EXTENDS_II(24,22, vlSelfRef.__PVT__v1_scaled)
                                   : 0U) + ((2U & (IData)(vlSelfRef.__PVT__filter_res_filt))
                                             ? VL_EXTENDS_II(24,22, vlSelfRef.__PVT__v2_scaled)
                                             : 0U)) 
                                + ((4U & (IData)(vlSelfRef.__PVT__filter_res_filt))
                                    ? VL_EXTENDS_II(24,22, vlSelfRef.__PVT__v3_scaled)
                                    : 0U)) + ((8U & (IData)(vlSelfRef.__PVT__filter_res_filt))
                                               ? 0U
                                               : 0U)) 
                              >> 7U));
        vlSelfRef.__PVT__flt__DOT__unnamedblk1__DOT__vd 
            = (0x0000ffffU & (((((((2U & (IData)(vlSymsp->TOP.top__DOT__sid_cfg_reg))
                                    ? 0U : 0x00ff1c72U) 
                                  + ((1U & (IData)(vlSelfRef.__PVT__filter_res_filt))
                                      ? 0U : VL_EXTENDS_II(24,22, vlSelfRef.__PVT__v1_scaled))) 
                                 + ((2U & (IData)(vlSelfRef.__PVT__filter_res_filt))
                                     ? 0U : VL_EXTENDS_II(24,22, vlSelfRef.__PVT__v2_scaled))) 
                                + ((1U & (((IData)(vlSelfRef.__PVT__filter_res_filt) 
                                           >> 2U) | 
                                          ((IData)(vlSelfRef.__PVT__filter_mode_vol) 
                                           >> 7U)))
                                    ? 0U : VL_EXTENDS_II(24,22, vlSelfRef.__PVT__v3_scaled))) 
                               + ((8U & (IData)(vlSelfRef.__PVT__filter_res_filt))
                                   ? 0U : 0U)) >> 7U));
        vlSelfRef.__PVT__flt__DOT__unnamedblk1__DOT___1_Q_lsl10 
            = vlSelfRef.__PVT__flt__DOT___1_Q_lsl10_tbl
            [((0x00000010U & ((IData)(vlSymsp->TOP.top__DOT__sid_cfg_reg) 
                              << 3U)) | (0x0000000fU 
                                         & ((IData)(vlSelfRef.__PVT__filter_res_filt) 
                                            >> 4U)))];
    } else {
        if ((3U == (7U & (IData)(vlSelfRef.__PVT__tbl_state)))) {
            vlSelfRef.__PVT__flt__DOT__s = 1U;
            vlSelfRef.__PVT__flt__DOT__vlp = vlSelfRef.__PVT__flt__DOT__vlp2;
            vlSelfRef.__PVT__flt__DOT__c = 0U;
            vlSelfRef.__PVT__flt__DOT__b = vlSelfRef.__PVT__flt__DOT__vhp;
        } else if ((4U == (7U & (IData)(vlSelfRef.__PVT__tbl_state)))) {
            vlSelfRef.__PVT__flt__DOT__s = 0U;
            vlSelfRef.__PVT__flt__DOT__vbp = vlSelfRef.__PVT__flt__DOT__vbp2;
            vlSelfRef.__PVT__flt__DOT__c = VL_SHIFTL_III(32,32,32, 
                                                         (- 
                                                          (VL_EXTENDS_II(32,16, (IData)(vlSelfRef.__PVT__flt__DOT__vlp2)) 
                                                           + 
                                                           VL_EXTENDS_II(32,16, (IData)(vlSelfRef.__PVT__flt__DOT__unnamedblk1__DOT__vi)))), 0x0000000aU);
            vlSelfRef.__PVT__flt__DOT__b = vlSelfRef.__PVT__flt__DOT__vbp_next;
        } else if ((5U == (7U & (IData)(vlSelfRef.__PVT__tbl_state)))) {
            vlSelfRef.__PVT__flt__DOT__s = 0U;
            __Vfunc_flt__DOT__clamp__6__x = (0x0001ffffU 
                                             & (((VL_EXTENDS_II(17,16, (IData)(vlSelfRef.__PVT__flt__DOT__unnamedblk1__DOT__vd)) 
                                                  + 
                                                  ((0x00000010U 
                                                    & (IData)(vlSelfRef.__PVT__filter_mode_vol))
                                                    ? 
                                                   VL_EXTENDS_II(17,16, (IData)(vlSelfRef.__PVT__flt__DOT__vlp2))
                                                    : 0U)) 
                                                 + 
                                                 ((0x00000020U 
                                                   & (IData)(vlSelfRef.__PVT__filter_mode_vol))
                                                   ? 
                                                  VL_EXTENDS_II(17,16, (IData)(vlSelfRef.__PVT__flt__DOT__vbp2))
                                                   : 0U)) 
                                                + (
                                                   (0x00000040U 
                                                    & (IData)(vlSelfRef.__PVT__filter_mode_vol))
                                                    ? 
                                                   VL_EXTENDS_II(17,16, (IData)(vlSelfRef.__PVT__flt__DOT__vhp_next))
                                                    : 0U)));
            __Vfunc_flt__DOT__clamp__6__Vfuncout = 
                (0x0000ffffU & ((1U & VL_REDXOR_32(
                                                   (3U 
                                                    & (__Vfunc_flt__DOT__clamp__6__x 
                                                       >> 0x0fU))))
                                 ? ((0x00008000U & 
                                     (__Vfunc_flt__DOT__clamp__6__x 
                                      >> 1U)) | (0x00007fffU 
                                                 & (- (IData)(
                                                              (1U 
                                                               & (__Vfunc_flt__DOT__clamp__6__x 
                                                                  >> 0x0fU))))))
                                 : __Vfunc_flt__DOT__clamp__6__x));
            vlSelfRef.__PVT__flt__DOT__c = 0U;
            vlSelfRef.__PVT__flt__DOT__b = __Vfunc_flt__DOT__clamp__6__Vfuncout;
        }
        if ((3U != (7U & (IData)(vlSelfRef.__PVT__tbl_state)))) {
            if ((4U == (7U & (IData)(vlSelfRef.__PVT__tbl_state)))) {
                vlSelfRef.__PVT__flt__DOT__a = vlSelfRef.__PVT__flt__DOT__unnamedblk1__DOT___1_Q_lsl10;
            } else if ((5U == (7U & (IData)(vlSelfRef.__PVT__tbl_state)))) {
                vlSelfRef.__PVT__flt__DOT__a = (0x0000000fU 
                                                & (IData)(vlSelfRef.__PVT__filter_mode_vol));
            }
        }
    }
    if (__VdlySet__voice_ad__v0) {
        vlSelfRef.__PVT__voice_ad[0U] = 0U;
        vlSelfRef.__PVT__voice_ad[1U] = 0U;
        vlSelfRef.__PVT__voice_ad[2U] = 0U;
    }
    if (__VdlySet__voice_ad__v3) {
        vlSelfRef.__PVT__voice_ad[0U] = __VdlyVal__voice_ad__v3;
    }
    if (__VdlySet__voice_ad__v4) {
        vlSelfRef.__PVT__voice_ad[1U] = __VdlyVal__voice_ad__v4;
    }
    if (__VdlySet__voice_ad__v5) {
        vlSelfRef.__PVT__voice_ad[2U] = __VdlyVal__voice_ad__v5;
    }
    if (__VdlySet__voice_vol__v0) {
        vlSelfRef.__PVT__voice_vol[0U] = 0x0fU;
        vlSelfRef.__PVT__voice_vol[1U] = 0x0fU;
        vlSelfRef.__PVT__voice_vol[2U] = 0x0fU;
    }
    if (__VdlySet__voice_vol__v3) {
        vlSelfRef.__PVT__voice_vol[0U] = __VdlyVal__voice_vol__v3;
    }
    if (__VdlySet__voice_vol__v4) {
        vlSelfRef.__PVT__voice_vol[1U] = __VdlyVal__voice_vol__v4;
    }
    if (__VdlySet__voice_vol__v5) {
        vlSelfRef.__PVT__voice_vol[2U] = __VdlyVal__voice_vol__v5;
    }
    vlSelfRef.__PVT__v1__DOT__adsr__DOT__unnamedblk1__DOT__exponential_counter_period 
        = __Vdly__v1__DOT__adsr__DOT__unnamedblk1__DOT__exponential_counter_period;
    vlSelfRef.__PVT__v1__DOT__adsr__DOT__unnamedblk1__DOT__hold_zero 
        = __Vdly__v1__DOT__adsr__DOT__unnamedblk1__DOT__hold_zero;
    vlSelfRef.__PVT__v1__DOT__adsr__DOT__unnamedblk1__DOT__exponential_counter 
        = __Vdly__v1__DOT__adsr__DOT__unnamedblk1__DOT__exponential_counter;
    vlSelfRef.__PVT__v1__DOT__adsr__DOT__unnamedblk1__DOT__rate_counter 
        = __Vdly__v1__DOT__adsr__DOT__unnamedblk1__DOT__rate_counter;
    vlSelfRef.__PVT__v1__DOT__adsr__DOT__state = __Vdly__v1__DOT__adsr__DOT__state;
    vlSelfRef.__PVT__v2__DOT__adsr__DOT__unnamedblk1__DOT__exponential_counter_period 
        = __Vdly__v2__DOT__adsr__DOT__unnamedblk1__DOT__exponential_counter_period;
    vlSelfRef.__PVT__v2__DOT__adsr__DOT__unnamedblk1__DOT__hold_zero 
        = __Vdly__v2__DOT__adsr__DOT__unnamedblk1__DOT__hold_zero;
    vlSelfRef.__PVT__v2__DOT__adsr__DOT__unnamedblk1__DOT__exponential_counter 
        = __Vdly__v2__DOT__adsr__DOT__unnamedblk1__DOT__exponential_counter;
    vlSelfRef.__PVT__v2__DOT__adsr__DOT__unnamedblk1__DOT__rate_counter 
        = __Vdly__v2__DOT__adsr__DOT__unnamedblk1__DOT__rate_counter;
    vlSelfRef.__PVT__v2__DOT__adsr__DOT__state = __Vdly__v2__DOT__adsr__DOT__state;
    vlSelfRef.__PVT__v3__DOT__adsr__DOT__unnamedblk1__DOT__exponential_counter_period 
        = __Vdly__v3__DOT__adsr__DOT__unnamedblk1__DOT__exponential_counter_period;
    vlSelfRef.__PVT__v3__DOT__adsr__DOT__unnamedblk1__DOT__hold_zero 
        = __Vdly__v3__DOT__adsr__DOT__unnamedblk1__DOT__hold_zero;
    vlSelfRef.__PVT__v3__DOT__adsr__DOT__unnamedblk1__DOT__exponential_counter 
        = __Vdly__v3__DOT__adsr__DOT__unnamedblk1__DOT__exponential_counter;
    vlSelfRef.__PVT__v3__DOT__adsr__DOT__unnamedblk1__DOT__rate_counter 
        = __Vdly__v3__DOT__adsr__DOT__unnamedblk1__DOT__rate_counter;
    vlSelfRef.__PVT__v3__DOT__adsr__DOT__state = __Vdly__v3__DOT__adsr__DOT__state;
    if (__VdlySet__voice_sr__v0) {
        vlSelfRef.__PVT__voice_sr[0U] = 0U;
        vlSelfRef.__PVT__voice_sr[1U] = 0U;
        vlSelfRef.__PVT__voice_sr[2U] = 0U;
    }
    if (__VdlySet__voice_sr__v3) {
        vlSelfRef.__PVT__voice_sr[0U] = __VdlyVal__voice_sr__v3;
    }
    if (__VdlySet__voice_sr__v4) {
        vlSelfRef.__PVT__voice_sr[1U] = __VdlyVal__voice_sr__v4;
    }
    if (__VdlySet__voice_sr__v5) {
        vlSelfRef.__PVT__voice_sr[2U] = __VdlyVal__voice_sr__v5;
    }
    if (__VdlySet__voice_freq__v0) {
        vlSelfRef.__PVT__voice_freq[0U] = 0U;
        vlSelfRef.__PVT__voice_freq[1U] = 0U;
        vlSelfRef.__PVT__voice_freq[2U] = 0U;
    }
    if (__VdlySet__voice_freq__v3) {
        vlSelfRef.__PVT__voice_freq[0U] = ((0xff00U 
                                            & vlSelfRef.__PVT__voice_freq[0U]) 
                                           | (IData)(__VdlyVal__voice_freq__v3));
    }
    if (__VdlySet__voice_freq__v4) {
        vlSelfRef.__PVT__voice_freq[0U] = ((0x00ffU 
                                            & vlSelfRef.__PVT__voice_freq[0U]) 
                                           | ((IData)(__VdlyVal__voice_freq__v4) 
                                              << 8U));
    }
    if (__VdlySet__voice_freq__v5) {
        vlSelfRef.__PVT__voice_freq[1U] = ((0xff00U 
                                            & vlSelfRef.__PVT__voice_freq[1U]) 
                                           | (IData)(__VdlyVal__voice_freq__v5));
    }
    if (__VdlySet__voice_freq__v6) {
        vlSelfRef.__PVT__voice_freq[1U] = ((0x00ffU 
                                            & vlSelfRef.__PVT__voice_freq[1U]) 
                                           | ((IData)(__VdlyVal__voice_freq__v6) 
                                              << 8U));
    }
    if (__VdlySet__voice_freq__v7) {
        vlSelfRef.__PVT__voice_freq[2U] = ((0xff00U 
                                            & vlSelfRef.__PVT__voice_freq[2U]) 
                                           | (IData)(__VdlyVal__voice_freq__v7));
    }
    if (__VdlySet__voice_freq__v8) {
        vlSelfRef.__PVT__voice_freq[2U] = ((0x00ffU 
                                            & vlSelfRef.__PVT__voice_freq[2U]) 
                                           | ((IData)(__VdlyVal__voice_freq__v8) 
                                              << 8U));
    }
    if (__VdlySet___st_out__v0) {
        vlSelfRef.__PVT___st_out[__VdlyDim0___st_out__v0] 
            = __VdlyVal___st_out__v0;
    }
    if (__VdlySet__p_t_out__v0) {
        vlSelfRef.__PVT__p_t_out[__VdlyDim0__p_t_out__v0] 
            = __VdlyVal__p_t_out__v0;
    }
    if (__VdlySet__ps___05Fout__v0) {
        vlSelfRef.__PVT__ps___05Fout[__VdlyDim0__ps___05Fout__v0] 
            = __VdlyVal__ps___05Fout__v0;
    }
    if (__VdlySet__pst_out__v0) {
        vlSelfRef.__PVT__pst_out[__VdlyDim0__pst_out__v0] 
            = __VdlyVal__pst_out__v0;
    }
    if (__VdlySet__voice_pw__v0) {
        vlSelfRef.__PVT__voice_pw[0U] = 0U;
        vlSelfRef.__PVT__voice_pw[1U] = 0U;
        vlSelfRef.__PVT__voice_pw[2U] = 0U;
    }
    if (__VdlySet__voice_pw__v3) {
        vlSelfRef.__PVT__voice_pw[0U] = ((0x0f00U & vlSelfRef.__PVT__voice_pw[0U]) 
                                         | (IData)(__VdlyVal__voice_pw__v3));
    }
    if (__VdlySet__voice_pw__v4) {
        vlSelfRef.__PVT__voice_pw[0U] = ((0x00ffU & vlSelfRef.__PVT__voice_pw[0U]) 
                                         | ((IData)(__VdlyVal__voice_pw__v4) 
                                            << 8U));
    }
    if (__VdlySet__voice_pw__v5) {
        vlSelfRef.__PVT__voice_pw[1U] = ((0x0f00U & vlSelfRef.__PVT__voice_pw[1U]) 
                                         | (IData)(__VdlyVal__voice_pw__v5));
    }
    if (__VdlySet__voice_pw__v6) {
        vlSelfRef.__PVT__voice_pw[1U] = ((0x00ffU & vlSelfRef.__PVT__voice_pw[1U]) 
                                         | ((IData)(__VdlyVal__voice_pw__v6) 
                                            << 8U));
    }
    if (__VdlySet__voice_pw__v7) {
        vlSelfRef.__PVT__voice_pw[2U] = ((0x0f00U & vlSelfRef.__PVT__voice_pw[2U]) 
                                         | (IData)(__VdlyVal__voice_pw__v7));
    }
    if (__VdlySet__voice_pw__v8) {
        vlSelfRef.__PVT__voice_pw[2U] = ((0x00ffU & vlSelfRef.__PVT__voice_pw[2U]) 
                                         | ((IData)(__VdlyVal__voice_pw__v8) 
                                            << 8U));
    }
    vlSelfRef.__PVT__v1__DOT__unnamedblk2__DOT__lfsr_noise 
        = __Vdly__v1__DOT__unnamedblk2__DOT__lfsr_noise;
    vlSelfRef.__PVT__v1__DOT__unnamedblk2__DOT__noise_age 
        = __Vdly__v1__DOT__unnamedblk2__DOT__noise_age;
    vlSelfRef.__PVT__v2__DOT__unnamedblk2__DOT__lfsr_noise 
        = __Vdly__v2__DOT__unnamedblk2__DOT__lfsr_noise;
    vlSelfRef.__PVT__v2__DOT__unnamedblk2__DOT__noise_age 
        = __Vdly__v2__DOT__unnamedblk2__DOT__noise_age;
    vlSelfRef.__PVT__v3__DOT__unnamedblk2__DOT__lfsr_noise 
        = __Vdly__v3__DOT__unnamedblk2__DOT__lfsr_noise;
    vlSelfRef.__PVT__v3__DOT__unnamedblk2__DOT__noise_age 
        = __Vdly__v3__DOT__unnamedblk2__DOT__noise_age;
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
    if ((2U & (IData)(vlSymsp->TOP.top__DOT__sid_cfg_reg))) {
        vlSelfRef.__PVT__f_p_t_out = vlSelfRef.__PVT__tbl__DOT__wave8580_p_t
            [(0x000007ffU & (IData)(vlSelfRef.__PVT__f_acc_t))];
        vlSelfRef.__PVT__f_ps___05Fout = vlSelfRef.__PVT__tbl__DOT__wave8580_ps_
            [vlSelfRef.__PVT__f_acc_t];
        vlSelfRef.__PVT__v1__DOT__norm_dac = (0x00000fffU 
                                              & (IData)(vlSelfRef.__PVT__v1__DOT__norm));
        vlSelfRef.__PVT__v2__DOT__norm_dac = (0x00000fffU 
                                              & (IData)(vlSelfRef.__PVT__v2__DOT__norm));
        vlSelfRef.__PVT__v3__DOT__norm_dac = (0x00000fffU 
                                              & (IData)(vlSelfRef.__PVT__v3__DOT__norm));
    } else {
        vlSelfRef.__PVT__f_p_t_out = vlSelfRef.__PVT__tbl__DOT__wave6581_p_t
            [(0x000007ffU & (IData)(vlSelfRef.__PVT__f_acc_t))];
        vlSelfRef.__PVT__f_ps___05Fout = vlSelfRef.__PVT__tbl__DOT__wave6581_ps_
            [(0x000007ffU & (IData)(vlSelfRef.__PVT__f_acc_t))];
        vlSelfRef.__PVT__v1__DOT__norm_dac = (0x00000fffU 
                                              & (vlSelfRef.__PVT__v1__DOT__waveform_dac__DOT__bitsum[11U] 
                                                 >> 4U));
        vlSelfRef.__PVT__v2__DOT__norm_dac = (0x00000fffU 
                                              & (vlSelfRef.__PVT__v2__DOT__waveform_dac__DOT__bitsum[11U] 
                                                 >> 4U));
        vlSelfRef.__PVT__v3__DOT__norm_dac = (0x00000fffU 
                                              & (vlSelfRef.__PVT__v3__DOT__waveform_dac__DOT__bitsum[11U] 
                                                 >> 4U));
    }
    __Vtableidx3 = (((IData)(vlSelfRef.__PVT__f_acc_t) 
                     << 1U) | (1U & ((IData)(vlSymsp->TOP.top__DOT__sid_cfg_reg) 
                                     >> 1U)));
    vlSelfRef.__PVT__f___05Fst_out = Vtop__ConstPool__TABLE_h5c0f3167_0
        [__Vtableidx3];
    __Vtableidx4 = (((IData)(vlSelfRef.__PVT__f_acc_t) 
                     << 1U) | (1U & ((IData)(vlSymsp->TOP.top__DOT__sid_cfg_reg) 
                                     >> 1U)));
    vlSelfRef.__PVT__f_pst_out = Vtop__ConstPool__TABLE_h8a1656db_0
        [__Vtableidx4];
    if ((2U != (7U & (IData)(vlSelfRef.__PVT__tbl_state)))) {
        if ((3U == (7U & (IData)(vlSelfRef.__PVT__tbl_state)))) {
            vlSelfRef.__PVT__flt__DOT__vlp2 = (0x0000ffffU 
                                               & ((1U 
                                                   & VL_REDXOR_32(
                                                                  (3U 
                                                                   & (vlSelfRef.__VdfgRegularize_hebeb780c_0_34 
                                                                      >> 0x0000000fU))))
                                                   ? 
                                                  ((0x00008000U 
                                                    & (vlSelfRef.__VdfgRegularize_hebeb780c_0_34 
                                                       >> 1U)) 
                                                   | (0x00007fffU 
                                                      & (- (IData)(
                                                                   (1U 
                                                                    & (vlSelfRef.__VdfgRegularize_hebeb780c_0_34 
                                                                       >> 0x0000000fU))))))
                                                   : vlSelfRef.__VdfgRegularize_hebeb780c_0_34));
        }
        if ((3U != (7U & (IData)(vlSelfRef.__PVT__tbl_state)))) {
            if ((4U == (7U & (IData)(vlSelfRef.__PVT__tbl_state)))) {
                vlSelfRef.__PVT__flt__DOT__vbp2 = vlSelfRef.__PVT__flt__DOT__vbp_next;
            }
            if ((4U != (7U & (IData)(vlSelfRef.__PVT__tbl_state)))) {
                if ((5U == (7U & (IData)(vlSelfRef.__PVT__tbl_state)))) {
                    vlSelfRef.__PVT__flt__DOT__vhp 
                        = vlSelfRef.__PVT__flt__DOT__vhp2;
                    vlSelfRef.__PVT__flt__DOT__vhp2 
                        = vlSelfRef.__PVT__flt__DOT__vhp_next;
                }
            }
        }
    }
    __PVT__flt__DOT__m = VL_MULS_III(32, VL_EXTENDS_II(32,16, (IData)(vlSelfRef.__PVT__flt__DOT__a)), 
                                     VL_EXTENDS_II(32,16, (IData)(vlSelfRef.__PVT__flt__DOT__b)));
    vlSelfRef.__PVT__tbl__DOT__f0 = vlSelfRef.__PVT__tbl__DOT__f6581_curve
        [(0x000003ffU & ((IData)(vlSelfRef.__PVT__filter_fc) 
                         >> 1U))];
    if ((1U & (~ (IData)(vlSymsp->TOP.rst)))) {
        if (vlSymsp->TOP.top__DOT__sid_ce_1m) {
            vlSelfRef.__PVT__v1__DOT__unnamedblk2__DOT__clk 
                = (1U & (~ (((IData)(vlSymsp->TOP.rst) 
                             | (vlSelfRef.__PVT__voice_ctrl[0U] 
                                >> 3U)) | ((~ (IData)(vlSelfRef.__PVT__v1__DOT__unnamedblk2__DOT__osc_edge)) 
                                           & (vlSelfRef.__PVT__v1__DOT__oscillator 
                                              >> 0x13U)))));
            vlSelfRef.__PVT__v2__DOT__unnamedblk2__DOT__clk 
                = (1U & (~ (((IData)(vlSymsp->TOP.rst) 
                             | (vlSelfRef.__PVT__voice_ctrl[1U] 
                                >> 3U)) | ((~ (IData)(vlSelfRef.__PVT__v2__DOT__unnamedblk2__DOT__osc_edge)) 
                                           & (vlSelfRef.__PVT__v2__DOT__oscillator 
                                              >> 0x13U)))));
            vlSelfRef.__PVT__v3__DOT__unnamedblk2__DOT__clk 
                = (1U & (~ (((IData)(vlSymsp->TOP.rst) 
                             | (vlSelfRef.__PVT__voice_ctrl[2U] 
                                >> 3U)) | ((~ (IData)(vlSelfRef.__PVT__v3__DOT__unnamedblk2__DOT__osc_edge)) 
                                           & (vlSelfRef.__PVT__v3__DOT__oscillator 
                                              >> 0x13U)))));
        }
    }
    if (vlSymsp->TOP.rst) {
        vlSelfRef.__PVT__v1__DOT__adsr__DOT__unnamedblk1__DOT__gate_edge 
            = (1U & vlSelfRef.__PVT__voice_ctrl[0U]);
        vlSelfRef.__PVT__v2__DOT__adsr__DOT__unnamedblk1__DOT__gate_edge 
            = (1U & vlSelfRef.__PVT__voice_ctrl[1U]);
        vlSelfRef.__PVT__v3__DOT__adsr__DOT__unnamedblk1__DOT__gate_edge 
            = (1U & vlSelfRef.__PVT__voice_ctrl[2U]);
        vlSelfRef.__PVT__filter_mode_vol = 0U;
        vlSelfRef.__PVT__filter_fc = 0U;
        vlSelfRef.__PVT__filter_res_filt = 0U;
        vlSelfRef.__PVT__v1__DOT__unnamedblk2__DOT__osc_edge = 0U;
        vlSelfRef.__PVT__v2__DOT__unnamedblk2__DOT__osc_edge = 0U;
        vlSelfRef.__PVT__v3__DOT__unnamedblk2__DOT__osc_edge = 0U;
    } else {
        if (vlSymsp->TOP.top__DOT__sid_ce_1m) {
            vlSelfRef.__PVT__v1__DOT__adsr__DOT__unnamedblk1__DOT__gate_edge 
                = (1U & vlSelfRef.__PVT__voice_ctrl[0U]);
            vlSelfRef.__PVT__v2__DOT__adsr__DOT__unnamedblk1__DOT__gate_edge 
                = (1U & vlSelfRef.__PVT__voice_ctrl[1U]);
            vlSelfRef.__PVT__v3__DOT__adsr__DOT__unnamedblk1__DOT__gate_edge 
                = (1U & vlSelfRef.__PVT__voice_ctrl[2U]);
            vlSelfRef.__PVT__v1__DOT__unnamedblk2__DOT__osc_edge 
                = (1U & (vlSelfRef.__PVT__v1__DOT__oscillator 
                         >> 0x13U));
            vlSelfRef.__PVT__v2__DOT__unnamedblk2__DOT__osc_edge 
                = (1U & (vlSelfRef.__PVT__v2__DOT__oscillator 
                         >> 0x13U));
            vlSelfRef.__PVT__v3__DOT__unnamedblk2__DOT__osc_edge 
                = (1U & (vlSelfRef.__PVT__v3__DOT__oscillator 
                         >> 0x13U));
        }
        if (vlSelfRef.cs) {
            if ((1U & (~ ((((((((0U == (0x0000001fU 
                                        & (IData)(vlSymsp->TOP.top__DOT__cpu_addr))) 
                                | (1U == (0x0000001fU 
                                          & (IData)(vlSymsp->TOP.top__DOT__cpu_addr)))) 
                               | (2U == (0x0000001fU 
                                         & (IData)(vlSymsp->TOP.top__DOT__cpu_addr)))) 
                              | (3U == (0x0000001fU 
                                        & (IData)(vlSymsp->TOP.top__DOT__cpu_addr)))) 
                             | (4U == (0x0000001fU 
                                       & (IData)(vlSymsp->TOP.top__DOT__cpu_addr)))) 
                            | (5U == (0x0000001fU & (IData)(vlSymsp->TOP.top__DOT__cpu_addr)))) 
                           | (6U == (0x0000001fU & (IData)(vlSymsp->TOP.top__DOT__cpu_addr)))) 
                          | (7U == (0x0000001fU & (IData)(vlSymsp->TOP.top__DOT__cpu_addr))))))) {
                if ((1U & (~ ((((((((8U == (0x0000001fU 
                                            & (IData)(vlSymsp->TOP.top__DOT__cpu_addr))) 
                                    | (9U == (0x0000001fU 
                                              & (IData)(vlSymsp->TOP.top__DOT__cpu_addr)))) 
                                   | (0x0aU == (0x0000001fU 
                                                & (IData)(vlSymsp->TOP.top__DOT__cpu_addr)))) 
                                  | (0x0bU == (0x0000001fU 
                                               & (IData)(vlSymsp->TOP.top__DOT__cpu_addr)))) 
                                 | (0x0cU == (0x0000001fU 
                                              & (IData)(vlSymsp->TOP.top__DOT__cpu_addr)))) 
                                | (0x0dU == (0x0000001fU 
                                             & (IData)(vlSymsp->TOP.top__DOT__cpu_addr)))) 
                               | (0x0eU == (0x0000001fU 
                                            & (IData)(vlSymsp->TOP.top__DOT__cpu_addr)))) 
                              | (0x0fU == (0x0000001fU 
                                           & (IData)(vlSymsp->TOP.top__DOT__cpu_addr))))))) {
                    if ((1U & (~ ((((((((0x10U == (0x0000001fU 
                                                   & (IData)(vlSymsp->TOP.top__DOT__cpu_addr))) 
                                        | (0x11U == 
                                           (0x0000001fU 
                                            & (IData)(vlSymsp->TOP.top__DOT__cpu_addr)))) 
                                       | (0x12U == 
                                          (0x0000001fU 
                                           & (IData)(vlSymsp->TOP.top__DOT__cpu_addr)))) 
                                      | (0x13U == (0x0000001fU 
                                                   & (IData)(vlSymsp->TOP.top__DOT__cpu_addr)))) 
                                     | (0x14U == (0x0000001fU 
                                                  & (IData)(vlSymsp->TOP.top__DOT__cpu_addr)))) 
                                    | (0x15U == (0x0000001fU 
                                                 & (IData)(vlSymsp->TOP.top__DOT__cpu_addr)))) 
                                   | (0x16U == (0x0000001fU 
                                                & (IData)(vlSymsp->TOP.top__DOT__cpu_addr)))) 
                                  | (0x17U == (0x0000001fU 
                                               & (IData)(vlSymsp->TOP.top__DOT__cpu_addr))))))) {
                        if ((0x18U == (0x0000001fU 
                                       & (IData)(vlSymsp->TOP.top__DOT__cpu_addr)))) {
                            vlSelfRef.__PVT__filter_mode_vol 
                                = vlSymsp->TOP.top__DOT__cpu_dout;
                        }
                    }
                    if (((((((((0x10U == (0x0000001fU 
                                          & (IData)(vlSymsp->TOP.top__DOT__cpu_addr))) 
                               | (0x11U == (0x0000001fU 
                                            & (IData)(vlSymsp->TOP.top__DOT__cpu_addr)))) 
                              | (0x12U == (0x0000001fU 
                                           & (IData)(vlSymsp->TOP.top__DOT__cpu_addr)))) 
                             | (0x13U == (0x0000001fU 
                                          & (IData)(vlSymsp->TOP.top__DOT__cpu_addr)))) 
                            | (0x14U == (0x0000001fU 
                                         & (IData)(vlSymsp->TOP.top__DOT__cpu_addr)))) 
                           | (0x15U == (0x0000001fU 
                                        & (IData)(vlSymsp->TOP.top__DOT__cpu_addr)))) 
                          | (0x16U == (0x0000001fU 
                                       & (IData)(vlSymsp->TOP.top__DOT__cpu_addr)))) 
                         | (0x17U == (0x0000001fU & (IData)(vlSymsp->TOP.top__DOT__cpu_addr))))) {
                        if ((0x10U != (0x0000001fU 
                                       & (IData)(vlSymsp->TOP.top__DOT__cpu_addr)))) {
                            if ((0x11U != (0x0000001fU 
                                           & (IData)(vlSymsp->TOP.top__DOT__cpu_addr)))) {
                                if ((0x12U != (0x0000001fU 
                                               & (IData)(vlSymsp->TOP.top__DOT__cpu_addr)))) {
                                    if ((0x13U != (0x0000001fU 
                                                   & (IData)(vlSymsp->TOP.top__DOT__cpu_addr)))) {
                                        if ((0x14U 
                                             != (0x0000001fU 
                                                 & (IData)(vlSymsp->TOP.top__DOT__cpu_addr)))) {
                                            if ((0x15U 
                                                 == 
                                                 (0x0000001fU 
                                                  & (IData)(vlSymsp->TOP.top__DOT__cpu_addr)))) {
                                                vlSelfRef.__PVT__filter_fc 
                                                    = 
                                                    ((0x07f8U 
                                                      & (IData)(vlSelfRef.__PVT__filter_fc)) 
                                                     | (7U 
                                                        & (IData)(vlSymsp->TOP.top__DOT__cpu_dout)));
                                            } else if (
                                                       (0x16U 
                                                        == 
                                                        (0x0000001fU 
                                                         & (IData)(vlSymsp->TOP.top__DOT__cpu_addr)))) {
                                                vlSelfRef.__PVT__filter_fc 
                                                    = 
                                                    ((7U 
                                                      & (IData)(vlSelfRef.__PVT__filter_fc)) 
                                                     | ((IData)(vlSymsp->TOP.top__DOT__cpu_dout) 
                                                        << 3U));
                                            }
                                            if ((0x15U 
                                                 != 
                                                 (0x0000001fU 
                                                  & (IData)(vlSymsp->TOP.top__DOT__cpu_addr)))) {
                                                if (
                                                    (0x16U 
                                                     != 
                                                     (0x0000001fU 
                                                      & (IData)(vlSymsp->TOP.top__DOT__cpu_addr)))) {
                                                    vlSelfRef.__PVT__filter_res_filt 
                                                        = vlSymsp->TOP.top__DOT__cpu_dout;
                                                }
                                            }
                                        }
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }
    }
    vlSelfRef.__PVT__f_acc_t = __Vdly__f_acc_t;
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
    vlSelfRef.__PVT__flt__DOT__o = ((IData)(vlSelfRef.__PVT__flt__DOT__s)
                                     ? (vlSelfRef.__PVT__flt__DOT__c 
                                        - __PVT__flt__DOT__m)
                                     : (vlSelfRef.__PVT__flt__DOT__c 
                                        + __PVT__flt__DOT__m));
    vlSelfRef.__PVT__v1__DOT__envelope = __Vdly__v1__DOT__envelope;
    vlSelfRef.__PVT__v1__DOT__unnamedblk3__DOT__keep_cnt 
        = __Vdly__v1__DOT__unnamedblk3__DOT__keep_cnt;
    vlSelfRef.__PVT__v2__DOT__envelope = __Vdly__v2__DOT__envelope;
    vlSelfRef.__PVT__v2__DOT__unnamedblk3__DOT__keep_cnt 
        = __Vdly__v2__DOT__unnamedblk3__DOT__keep_cnt;
    vlSelfRef.__PVT__v3__DOT__envelope = __Vdly__v3__DOT__envelope;
    vlSelfRef.__PVT__v3__DOT__unnamedblk3__DOT__keep_cnt 
        = __Vdly__v3__DOT__unnamedblk3__DOT__keep_cnt;
    if (__VdlySet__voice_ctrl__v0) {
        vlSelfRef.__PVT__voice_ctrl[0U] = 0U;
        vlSelfRef.__PVT__voice_ctrl[1U] = 0U;
        vlSelfRef.__PVT__voice_ctrl[2U] = 0U;
    }
    if (__VdlySet__voice_ctrl__v3) {
        vlSelfRef.__PVT__voice_ctrl[0U] = __VdlyVal__voice_ctrl__v3;
    }
    if (__VdlySet__voice_ctrl__v4) {
        vlSelfRef.__PVT__voice_ctrl[1U] = __VdlyVal__voice_ctrl__v4;
    }
    if (__VdlySet__voice_ctrl__v5) {
        vlSelfRef.__PVT__voice_ctrl[2U] = __VdlyVal__voice_ctrl__v5;
    }
    vlSelfRef.__VdfgRegularize_hebeb780c_0_34 = (0x0001ffffU 
                                                 & (VL_EXTENDS_II(17,16, (IData)(vlSelfRef.__PVT__flt__DOT__vlp)) 
                                                    + 
                                                    VL_SHIFTRS_III(17,32,32, vlSelfRef.__PVT__flt__DOT__o, 0x00000011U)));
    __VdfgRegularize_hebeb780c_0_35 = (0x0001ffffU 
                                       & (VL_EXTENDS_II(17,16, (IData)(vlSelfRef.__PVT__flt__DOT__vbp)) 
                                          + VL_SHIFTRS_III(17,32,32, vlSelfRef.__PVT__flt__DOT__o, 0x00000011U)));
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
    vlSelfRef.__PVT__tbl_state = __Vdly__tbl_state;
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
    vlSelfRef.__PVT__v1__DOT__oscillator = __Vdly__v1__DOT__oscillator;
    vlSelfRef.__PVT__v2__DOT__oscillator = __Vdly__v2__DOT__oscillator;
    vlSelfRef.__PVT__v3__DOT__oscillator = __Vdly__v3__DOT__oscillator;
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
    vlSelfRef.__PVT__acc_t[1U] = vlSelfRef.__Vcellout__v2__acc_t;
    vlSelfRef.__PVT__acc_t[0U] = vlSelfRef.__Vcellout__v1__acc_t;
    vlSelfRef.__PVT__acc_t[2U] = vlSelfRef.__Vcellout__v3__acc_t;
}
