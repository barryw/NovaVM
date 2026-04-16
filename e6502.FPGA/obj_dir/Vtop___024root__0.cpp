// Verilated -*- C++ -*-
// DESCRIPTION: Verilator output: Design implementation internals
// See Vtop.h for the primary calling header

#include "Vtop__pch.h"

void Vtop___024root___eval_triggers_vec__ico(Vtop___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vtop___024root___eval_triggers_vec__ico\n"); );
    Vtop__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
    // Body
    vlSelfRef.__VicoTriggered[0U] = ((0xfffffffffffffffeULL 
                                      & vlSelfRef.__VicoTriggered[0U]) 
                                     | (IData)((IData)(vlSelfRef.__VicoFirstIteration)));
}

bool Vtop___024root___trigger_anySet__ico(const VlUnpacked<QData/*63:0*/, 1> &in) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vtop___024root___trigger_anySet__ico\n"); );
    // Locals
    IData/*31:0*/ n;
    // Body
    n = 0U;
    do {
        if (in[n]) {
            return (1U);
        }
        n = ((IData)(1U) + n);
    } while ((1U > n));
    return (0U);
}

extern const VlUnpacked<CData/*0:0*/, 2048> Vtop__ConstPool__TABLE_h32401d32_0;

void Vtop___024root___ico_sequent__TOP__0(Vtop___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vtop___024root___ico_sequent__TOP__0\n"); );
    Vtop__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
    // Locals
    CData/*5:0*/ top__DOT____VdfgRegularize_hbded3b40_0_0;
    top__DOT____VdfgRegularize_hbded3b40_0_0 = 0;
    CData/*7:0*/ top__DOT__cpu_inst__DOT__BI;
    top__DOT__cpu_inst__DOT__BI = 0;
    SData/*8:0*/ top__DOT__cpu_inst__DOT__ALU__DOT__temp_logic;
    top__DOT__cpu_inst__DOT__ALU__DOT__temp_logic = 0;
    CData/*4:0*/ top__DOT__cpu_inst__DOT__ALU__DOT__temp_l;
    top__DOT__cpu_inst__DOT__ALU__DOT__temp_l = 0;
    SData/*10:0*/ __Vtableidx1;
    __Vtableidx1 = 0;
    SData/*15:0*/ __VdfgRegularize_hebeb780c_0_1;
    __VdfgRegularize_hebeb780c_0_1 = 0;
    SData/*15:0*/ __VdfgRegularize_hebeb780c_0_2;
    __VdfgRegularize_hebeb780c_0_2 = 0;
    CData/*7:0*/ __VdfgRegularize_hebeb780c_0_16;
    __VdfgRegularize_hebeb780c_0_16 = 0;
    CData/*7:0*/ __VdfgRegularize_hebeb780c_0_17;
    __VdfgRegularize_hebeb780c_0_17 = 0;
    CData/*7:0*/ __VdfgRegularize_hebeb780c_0_18;
    __VdfgRegularize_hebeb780c_0_18 = 0;
    // Body
    __Vtableidx1 = ((((IData)(vlSelfRef.top__DOT__cpu_inst__DOT__backwards) 
                      << 0x0000000aU) | (((IData)(vlSelfRef.top__DOT__cpu_inst__DOT__CO) 
                                          << 9U) | 
                                         ((IData)(vlSelfRef.top__DOT__cpu_inst__DOT__NMI_edge) 
                                          << 8U))) 
                    | ((0x00000080U & ((~ (IData)(vlSelfRef.irq_n)) 
                                       << 7U)) | (((IData)(vlSelfRef.top__DOT__cpu_inst__DOT__I) 
                                                   << 6U) 
                                                  | (IData)(vlSelfRef.top__DOT__cpu_inst__DOT__state))));
    vlSelfRef.top__DOT__cpu_inst__DOT__PC_inc = Vtop__ConstPool__TABLE_h32401d32_0
        [__Vtableidx1];
    vlSelfRef.__VdfgRegularize_hebeb780c_0_3 = (1U 
                                                & (((~ (IData)(vlSelfRef.top__DOT__cpu_inst__DOT__I)) 
                                                    & (~ (IData)(vlSelfRef.irq_n))) 
                                                   | (IData)(vlSelfRef.top__DOT__cpu_inst__DOT__NMI_edge)));
    vlSelfRef.top__DOT__vgc_inst__DOT__dbg_vgc_sel 
        = ((0xa000U <= (IData)(vlSelfRef.dbg_peek_addr)) 
           & (0xa01fU >= (IData)(vlSelfRef.dbg_peek_addr)));
    vlSelfRef.top__DOT__vgc_inst__DOT__dbg_char_sel 
        = ((0xaa00U <= (IData)(vlSelfRef.dbg_peek_addr)) 
           & (0xb1cfU >= (IData)(vlSelfRef.dbg_peek_addr)));
    vlSelfRef.top__DOT__vgc_inst__DOT__dbg_color_sel 
        = ((0xb1d0U <= (IData)(vlSelfRef.dbg_peek_addr)) 
           & (0xb99fU >= (IData)(vlSelfRef.dbg_peek_addr)));
    vlSelfRef.top__DOT____Vcellinp__cpu_inst__RDY = 
        ((~ (IData)(vlSelfRef.dbg_pause)) & ((0U == (IData)(vlSelfRef.top__DOT__blt_inst__DOT__state)) 
                                             | (6U 
                                                == (IData)(vlSelfRef.top__DOT__blt_inst__DOT__state))));
    vlSelfRef.top__DOT__vgc_inst__DOT__char_a_addr 
        = (0x000007ffU & (((IData)(vlSelfRef.top__DOT__blt_vgc_we) 
                           & (1U == (IData)(vlSelfRef.top__DOT__blt_vgc_space)))
                           ? (IData)(vlSelfRef.top__DOT__blt_vgc_addr)
                           : ((IData)(vlSelfRef.top__DOT__vgc_inst__DOT__cmd_char_we)
                               ? (IData)(vlSelfRef.top__DOT__vgc_inst__DOT__cmd_char_addr)
                               : (((IData)(vlSelfRef.top__DOT__blt_vgc_re) 
                                   & (1U == (IData)(vlSelfRef.top__DOT__blt_vgc_space)))
                                   ? (IData)(vlSelfRef.top__DOT__blt_vgc_addr)
                                   : ((IData)(vlSelfRef.top__DOT__vgc_inst__DOT__dbg_char_sel)
                                       ? ((IData)(vlSelfRef.dbg_peek_addr) 
                                          - (IData)(0x0200U))
                                       : (IData)(vlSelfRef.top__DOT__vgc_inst__DOT__cmd_char_addr))))));
    vlSelfRef.top__DOT__vgc_inst__DOT__color_a_addr 
        = (0x000007ffU & (((IData)(vlSelfRef.top__DOT__blt_vgc_we) 
                           & (2U == (IData)(vlSelfRef.top__DOT__blt_vgc_space)))
                           ? (IData)(vlSelfRef.top__DOT__blt_vgc_addr)
                           : ((IData)(vlSelfRef.top__DOT__vgc_inst__DOT__cmd_color_we)
                               ? (IData)(vlSelfRef.top__DOT__vgc_inst__DOT__cmd_color_addr)
                               : (((IData)(vlSelfRef.top__DOT__blt_vgc_re) 
                                   & (2U == (IData)(vlSelfRef.top__DOT__blt_vgc_space)))
                                   ? (IData)(vlSelfRef.top__DOT__blt_vgc_addr)
                                   : ((IData)(vlSelfRef.top__DOT__vgc_inst__DOT__dbg_color_sel)
                                       ? ((IData)(vlSelfRef.dbg_peek_addr) 
                                          - (IData)(0x01d0U))
                                       : (IData)(vlSelfRef.top__DOT__vgc_inst__DOT__cmd_color_addr))))));
    vlSelfRef.top__DOT__vgc_dbg_rdata = 0U;
    if (vlSelfRef.top__DOT__vgc_inst__DOT__dbg_char_sel) {
        vlSelfRef.top__DOT__vgc_dbg_rdata = vlSelfRef.top__DOT__vgc_inst__DOT__dbg_rd_latch;
    } else if (vlSelfRef.top__DOT__vgc_inst__DOT__dbg_color_sel) {
        vlSelfRef.top__DOT__vgc_dbg_rdata = vlSelfRef.top__DOT__vgc_inst__DOT__dbg_rd_latch;
    } else if (vlSelfRef.top__DOT__vgc_inst__DOT__dbg_vgc_sel) {
        vlSelfRef.top__DOT__vgc_dbg_rdata = ((0U == 
                                              (0x0000001fU 
                                               & (IData)(vlSelfRef.dbg_peek_addr)))
                                              ? (IData)(vlSelfRef.top__DOT__vgc_inst__DOT__mode)
                                              : ((1U 
                                                  == 
                                                  (0x0000001fU 
                                                   & (IData)(vlSelfRef.dbg_peek_addr)))
                                                  ? (IData)(vlSelfRef.top__DOT__vgc_inst__DOT__bg_color)
                                                  : 
                                                 ((2U 
                                                   == 
                                                   (0x0000001fU 
                                                    & (IData)(vlSelfRef.dbg_peek_addr)))
                                                   ? (IData)(vlSelfRef.top__DOT__vgc_inst__DOT__fg_color)
                                                   : 
                                                  ((3U 
                                                    == 
                                                    (0x0000001fU 
                                                     & (IData)(vlSelfRef.dbg_peek_addr)))
                                                    ? (IData)(vlSelfRef.top__DOT__vgc_inst__DOT__cursor_x)
                                                    : 
                                                   ((4U 
                                                     == 
                                                     (0x0000001fU 
                                                      & (IData)(vlSelfRef.dbg_peek_addr)))
                                                     ? (IData)(vlSelfRef.top__DOT__vgc_inst__DOT__cursor_y)
                                                     : 
                                                    ((7U 
                                                      == 
                                                      (0x0000001fU 
                                                       & (IData)(vlSelfRef.dbg_peek_addr)))
                                                      ? (IData)(vlSelfRef.top__DOT__vgc_inst__DOT__font_slot)
                                                      : 
                                                     ((0x0fU 
                                                       == 
                                                       (0x0000001fU 
                                                        & (IData)(vlSelfRef.dbg_peek_addr)))
                                                       ? (IData)(vlSelfRef.top__DOT__vgc_inst__DOT__char_in_reg)
                                                       : vlSelfRef.top__DOT__vgc_inst__DOT__regs
                                                      [
                                                      (0x0000001fU 
                                                       & (IData)(vlSelfRef.dbg_peek_addr))])))))));
    }
    vlSelfRef.top__DOT__cpu_inst__DOT__DIMUX = ((IData)(vlSelfRef.top__DOT____Vcellinp__cpu_inst__RDY)
                                                 ? (IData)(vlSelfRef.top__DOT__cpu_din)
                                                 : (IData)(vlSelfRef.top__DOT__cpu_inst__DOT__DIHOLD));
    vlSelfRef.top__DOT__cpu_inst__DOT__IR = ((IData)(vlSelfRef.__VdfgRegularize_hebeb780c_0_3)
                                              ? 0U : 
                                             ((IData)(vlSelfRef.top__DOT__cpu_inst__DOT__IRHOLD_valid)
                                               ? (IData)(vlSelfRef.top__DOT__cpu_inst__DOT__IRHOLD)
                                               : (IData)(vlSelfRef.top__DOT__cpu_inst__DOT__DIMUX)));
    if ((1U & (IData)(vlSelfRef.top__DOT__cpu_inst__DOT__state))) {
        __VdfgRegularize_hebeb780c_0_18 = 0U;
        __VdfgRegularize_hebeb780c_0_16 = vlSelfRef.top__DOT__cpu_inst__DOT__DIMUX;
    } else {
        __VdfgRegularize_hebeb780c_0_18 = vlSelfRef.top__DOT__cpu_inst__DOT__DIMUX;
        __VdfgRegularize_hebeb780c_0_16 = 0U;
    }
    __VdfgRegularize_hebeb780c_0_17 = ((2U & (IData)(vlSelfRef.top__DOT__cpu_inst__DOT__state))
                                        ? 0U : (IData)(vlSelfRef.top__DOT__cpu_inst__DOT__DIMUX));
    vlSelfRef.__VdfgRegularize_hebeb780c_0_0 = (((IData)(vlSelfRef.top__DOT__cpu_inst__DOT__DIMUX) 
                                                 << 8U) 
                                                | (IData)(vlSelfRef.top__DOT__cpu_inst__DOT__ADD));
    __VdfgRegularize_hebeb780c_0_1 = ((1U & (IData)(vlSelfRef.top__DOT__cpu_inst__DOT__state))
                                       ? (IData)(vlSelfRef.__VdfgRegularize_hebeb780c_0_0)
                                       : (IData)(vlSelfRef.top__DOT__cpu_inst__DOT__PC));
    __VdfgRegularize_hebeb780c_0_2 = ((2U & (IData)(vlSelfRef.top__DOT__cpu_inst__DOT__state))
                                       ? (IData)(vlSelfRef.top__DOT__cpu_inst__DOT__PC)
                                       : (IData)(__VdfgRegularize_hebeb780c_0_1));
    if ((0x00000020U & (IData)(vlSelfRef.top__DOT__cpu_inst__DOT__state))) {
        if ((0x00000010U & (IData)(vlSelfRef.top__DOT__cpu_inst__DOT__state))) {
            vlSelfRef.top__DOT__cpu_dout = vlSelfRef.top__DOT__cpu_inst__DOT__regfile;
            if ((8U & (IData)(vlSelfRef.top__DOT__cpu_inst__DOT__state))) {
                vlSelfRef.top__DOT__cpu_inst__DOT__AI = 0U;
                vlSelfRef.top__DOT__cpu_addr = vlSelfRef.top__DOT__cpu_inst__DOT__PC;
            } else if ((4U & (IData)(vlSelfRef.top__DOT__cpu_inst__DOT__state))) {
                vlSelfRef.top__DOT__cpu_inst__DOT__AI = 0U;
                vlSelfRef.top__DOT__cpu_addr = vlSelfRef.top__DOT__cpu_inst__DOT__PC;
            } else if ((2U & (IData)(vlSelfRef.top__DOT__cpu_inst__DOT__state))) {
                vlSelfRef.top__DOT__cpu_inst__DOT__AI = 0U;
                vlSelfRef.top__DOT__cpu_addr = vlSelfRef.top__DOT__cpu_inst__DOT__PC;
            } else {
                vlSelfRef.top__DOT__cpu_inst__DOT__AI 
                    = vlSelfRef.__VdfgRegularize_hebeb780c_0_12;
                vlSelfRef.top__DOT__cpu_addr = vlSelfRef.__VdfgRegularize_hebeb780c_0_6;
            }
            top__DOT__cpu_inst__DOT__BI = (0x000000ffU 
                                           & (IData)(vlSelfRef.top__DOT__cpu_inst__DOT__DIMUX));
            vlSelfRef.top__DOT__cpu_inst__DOT__PC_temp 
                = vlSelfRef.top__DOT__cpu_inst__DOT__PC;
        } else if ((8U & (IData)(vlSelfRef.top__DOT__cpu_inst__DOT__state))) {
            if ((4U & (IData)(vlSelfRef.top__DOT__cpu_inst__DOT__state))) {
                if ((2U & (IData)(vlSelfRef.top__DOT__cpu_inst__DOT__state))) {
                    if ((1U & (IData)(vlSelfRef.top__DOT__cpu_inst__DOT__state))) {
                        vlSelfRef.top__DOT__cpu_dout 
                            = vlSelfRef.top__DOT__cpu_inst__DOT__regfile;
                        vlSelfRef.top__DOT__cpu_addr 
                            = vlSelfRef.top__DOT__cpu_inst__DOT__DIMUX;
                    } else {
                        vlSelfRef.top__DOT__cpu_dout 
                            = vlSelfRef.top__DOT__cpu_inst__DOT__ADD;
                        vlSelfRef.top__DOT__cpu_addr 
                            = vlSelfRef.__VdfgRegularize_hebeb780c_0_4;
                    }
                } else {
                    vlSelfRef.top__DOT__cpu_dout = vlSelfRef.top__DOT__cpu_inst__DOT__regfile;
                    vlSelfRef.top__DOT__cpu_addr = vlSelfRef.__VdfgRegularize_hebeb780c_0_8;
                }
                vlSelfRef.top__DOT__cpu_inst__DOT__AI = 0U;
                top__DOT__cpu_inst__DOT__BI = (0x000000ffU 
                                               & (IData)(vlSelfRef.top__DOT__cpu_inst__DOT__DIMUX));
            } else {
                vlSelfRef.top__DOT__cpu_dout = vlSelfRef.top__DOT__cpu_inst__DOT__regfile;
                vlSelfRef.top__DOT__cpu_inst__DOT__AI 
                    = vlSelfRef.__VdfgRegularize_hebeb780c_0_14;
                top__DOT__cpu_inst__DOT__BI = (0x000000ffU 
                                               & (IData)(__VdfgRegularize_hebeb780c_0_17));
                vlSelfRef.top__DOT__cpu_addr = ((2U 
                                                 & (IData)(vlSelfRef.top__DOT__cpu_inst__DOT__state))
                                                 ? (IData)(vlSelfRef.__VdfgRegularize_hebeb780c_0_9)
                                                 : 
                                                ((1U 
                                                  & (IData)(vlSelfRef.top__DOT__cpu_inst__DOT__state))
                                                  ? (IData)(vlSelfRef.__VdfgRegularize_hebeb780c_0_0)
                                                  : 
                                                 (0x00000100U 
                                                  | (IData)(vlSelfRef.top__DOT__cpu_inst__DOT__ADD))));
            }
            vlSelfRef.top__DOT__cpu_inst__DOT__PC_temp 
                = __VdfgRegularize_hebeb780c_0_2;
        } else {
            if ((4U & (IData)(vlSelfRef.top__DOT__cpu_inst__DOT__state))) {
                vlSelfRef.top__DOT__cpu_dout = vlSelfRef.top__DOT__cpu_inst__DOT__regfile;
                if ((2U & (IData)(vlSelfRef.top__DOT__cpu_inst__DOT__state))) {
                    vlSelfRef.top__DOT__cpu_inst__DOT__AI 
                        = vlSelfRef.top__DOT__cpu_inst__DOT__ADD;
                    vlSelfRef.top__DOT__cpu_addr = 
                        (0x00000100U | (IData)(vlSelfRef.top__DOT__cpu_inst__DOT__ADD));
                } else {
                    vlSelfRef.top__DOT__cpu_inst__DOT__AI 
                        = vlSelfRef.top__DOT__cpu_inst__DOT__regfile;
                    vlSelfRef.top__DOT__cpu_addr = 
                        ((1U & (IData)(vlSelfRef.top__DOT__cpu_inst__DOT__state))
                          ? (0x00000100U | (IData)(vlSelfRef.top__DOT__cpu_inst__DOT__regfile))
                          : (IData)(vlSelfRef.__VdfgRegularize_hebeb780c_0_4));
                }
                top__DOT__cpu_inst__DOT__BI = (0x000000ffU 
                                               & 0U);
            } else if ((2U & (IData)(vlSelfRef.top__DOT__cpu_inst__DOT__state))) {
                if ((1U & (IData)(vlSelfRef.top__DOT__cpu_inst__DOT__state))) {
                    vlSelfRef.top__DOT__cpu_dout = vlSelfRef.top__DOT__cpu_inst__DOT__regfile;
                    vlSelfRef.top__DOT__cpu_inst__DOT__AI 
                        = vlSelfRef.top__DOT__cpu_inst__DOT__DIMUX;
                    vlSelfRef.top__DOT__cpu_addr = vlSelfRef.__VdfgRegularize_hebeb780c_0_4;
                } else {
                    vlSelfRef.top__DOT__cpu_dout = 
                        ((IData)(vlSelfRef.top__DOT__cpu_inst__DOT__php)
                          ? (IData)(vlSelfRef.top__DOT__cpu_inst__DOT__P)
                          : (IData)(vlSelfRef.top__DOT__cpu_inst__DOT__ADD));
                    vlSelfRef.top__DOT__cpu_inst__DOT__AI 
                        = vlSelfRef.top__DOT__cpu_inst__DOT__regfile;
                    vlSelfRef.top__DOT__cpu_addr = 
                        (0x00000100U | (IData)(vlSelfRef.top__DOT__cpu_inst__DOT__regfile));
                }
                top__DOT__cpu_inst__DOT__BI = (0x000000ffU 
                                               & 0U);
            } else {
                vlSelfRef.top__DOT__cpu_dout = vlSelfRef.top__DOT__cpu_inst__DOT__regfile;
                vlSelfRef.top__DOT__cpu_inst__DOT__AI 
                    = vlSelfRef.__VdfgRegularize_hebeb780c_0_15;
                top__DOT__cpu_inst__DOT__BI = (0x000000ffU 
                                               & (IData)(__VdfgRegularize_hebeb780c_0_18));
                vlSelfRef.top__DOT__cpu_addr = vlSelfRef.top__DOT__cpu_inst__DOT__PC;
            }
            vlSelfRef.top__DOT__cpu_inst__DOT__PC_temp 
                = vlSelfRef.top__DOT__cpu_inst__DOT__PC;
        }
    } else if ((0x00000010U & (IData)(vlSelfRef.top__DOT__cpu_inst__DOT__state))) {
        if ((8U & (IData)(vlSelfRef.top__DOT__cpu_inst__DOT__state))) {
            if ((4U & (IData)(vlSelfRef.top__DOT__cpu_inst__DOT__state))) {
                vlSelfRef.top__DOT__cpu_dout = vlSelfRef.top__DOT__cpu_inst__DOT__regfile;
                vlSelfRef.top__DOT__cpu_inst__DOT__AI 
                    = vlSelfRef.__VdfgRegularize_hebeb780c_0_12;
                top__DOT__cpu_inst__DOT__BI = (0x000000ffU 
                                               & (IData)(__VdfgRegularize_hebeb780c_0_16));
                vlSelfRef.top__DOT__cpu_addr = ((2U 
                                                 & (IData)(vlSelfRef.top__DOT__cpu_inst__DOT__state))
                                                 ? 
                                                ((1U 
                                                  & (IData)(vlSelfRef.top__DOT__cpu_inst__DOT__state))
                                                  ? 
                                                 (0x00000100U 
                                                  | (IData)(vlSelfRef.top__DOT__cpu_inst__DOT__ADD))
                                                  : (IData)(vlSelfRef.top__DOT__cpu_inst__DOT__PC))
                                                 : (IData)(vlSelfRef.top__DOT__cpu_inst__DOT__PC));
            } else {
                if ((2U & (IData)(vlSelfRef.top__DOT__cpu_inst__DOT__state))) {
                    vlSelfRef.top__DOT__cpu_dout = vlSelfRef.__VdfgRegularize_hebeb780c_0_11;
                    vlSelfRef.top__DOT__cpu_addr = vlSelfRef.__VdfgRegularize_hebeb780c_0_9;
                } else {
                    vlSelfRef.top__DOT__cpu_dout = vlSelfRef.top__DOT__cpu_inst__DOT__regfile;
                    vlSelfRef.top__DOT__cpu_addr = __VdfgRegularize_hebeb780c_0_1;
                }
                vlSelfRef.top__DOT__cpu_inst__DOT__AI 
                    = vlSelfRef.__VdfgRegularize_hebeb780c_0_14;
                top__DOT__cpu_inst__DOT__BI = (0x000000ffU 
                                               & (IData)(__VdfgRegularize_hebeb780c_0_17));
            }
            vlSelfRef.top__DOT__cpu_inst__DOT__PC_temp 
                = __VdfgRegularize_hebeb780c_0_2;
        } else {
            vlSelfRef.top__DOT__cpu_dout = vlSelfRef.top__DOT__cpu_inst__DOT__regfile;
            if ((4U & (IData)(vlSelfRef.top__DOT__cpu_inst__DOT__state))) {
                vlSelfRef.top__DOT__cpu_inst__DOT__AI = 0U;
                if ((2U & (IData)(vlSelfRef.top__DOT__cpu_inst__DOT__state))) {
                    vlSelfRef.top__DOT__cpu_addr = __VdfgRegularize_hebeb780c_0_1;
                    vlSelfRef.top__DOT__cpu_inst__DOT__PC_temp 
                        = __VdfgRegularize_hebeb780c_0_1;
                } else {
                    vlSelfRef.top__DOT__cpu_addr = 
                        ((1U & (IData)(vlSelfRef.top__DOT__cpu_inst__DOT__state))
                          ? (IData)(vlSelfRef.__VdfgRegularize_hebeb780c_0_10)
                          : (IData)(vlSelfRef.__VdfgRegularize_hebeb780c_0_0));
                    vlSelfRef.top__DOT__cpu_inst__DOT__PC_temp 
                        = vlSelfRef.top__DOT__cpu_inst__DOT__PC;
                }
            } else {
                if ((2U & (IData)(vlSelfRef.top__DOT__cpu_inst__DOT__state))) {
                    vlSelfRef.top__DOT__cpu_inst__DOT__AI 
                        = vlSelfRef.__VdfgRegularize_hebeb780c_0_15;
                    vlSelfRef.top__DOT__cpu_addr = 
                        ((1U & (IData)(vlSelfRef.top__DOT__cpu_inst__DOT__state))
                          ? (IData)(vlSelfRef.top__DOT__cpu_inst__DOT__ADD)
                          : (IData)(vlSelfRef.top__DOT__cpu_inst__DOT__DIMUX));
                } else {
                    vlSelfRef.top__DOT__cpu_inst__DOT__AI = 0U;
                    vlSelfRef.top__DOT__cpu_addr = 
                        ((1U & (IData)(vlSelfRef.top__DOT__cpu_inst__DOT__state))
                          ? (IData)(vlSelfRef.__VdfgRegularize_hebeb780c_0_0)
                          : (IData)(vlSelfRef.top__DOT__cpu_inst__DOT__ADD));
                }
                vlSelfRef.top__DOT__cpu_inst__DOT__PC_temp 
                    = vlSelfRef.top__DOT__cpu_inst__DOT__PC;
            }
            top__DOT__cpu_inst__DOT__BI = (0x000000ffU 
                                           & (IData)(vlSelfRef.top__DOT__cpu_inst__DOT__DIMUX));
        }
    } else if ((8U & (IData)(vlSelfRef.top__DOT__cpu_inst__DOT__state))) {
        if ((4U & (IData)(vlSelfRef.top__DOT__cpu_inst__DOT__state))) {
            vlSelfRef.top__DOT__cpu_dout = vlSelfRef.top__DOT__cpu_inst__DOT__regfile;
            if ((2U & (IData)(vlSelfRef.top__DOT__cpu_inst__DOT__state))) {
                vlSelfRef.top__DOT__cpu_inst__DOT__AI 
                    = vlSelfRef.__VdfgRegularize_hebeb780c_0_13;
                top__DOT__cpu_inst__DOT__BI = (0x000000ffU 
                                               & (IData)(__VdfgRegularize_hebeb780c_0_18));
                vlSelfRef.top__DOT__cpu_addr = vlSelfRef.__VdfgRegularize_hebeb780c_0_6;
                vlSelfRef.top__DOT__cpu_inst__DOT__PC_temp 
                    = vlSelfRef.top__DOT__cpu_inst__DOT__PC;
            } else {
                if ((1U & (IData)(vlSelfRef.top__DOT__cpu_inst__DOT__state))) {
                    vlSelfRef.top__DOT__cpu_inst__DOT__AI 
                        = ((IData)(vlSelfRef.top__DOT__cpu_inst__DOT__load_only)
                            ? 0U : (IData)(vlSelfRef.top__DOT__cpu_inst__DOT__regfile));
                    vlSelfRef.top__DOT__cpu_inst__DOT__PC_temp 
                        = vlSelfRef.top__DOT__cpu_inst__DOT__PC;
                } else {
                    vlSelfRef.top__DOT__cpu_inst__DOT__AI = 0U;
                    vlSelfRef.top__DOT__cpu_inst__DOT__PC_temp 
                        = ((IData)(vlSelfRef.__VdfgRegularize_hebeb780c_0_3)
                            ? (IData)(vlSelfRef.__VdfgRegularize_hebeb780c_0_4)
                            : (IData)(vlSelfRef.top__DOT__cpu_inst__DOT__PC));
                }
                top__DOT__cpu_inst__DOT__BI = (0x000000ffU 
                                               & (IData)(__VdfgRegularize_hebeb780c_0_16));
                vlSelfRef.top__DOT__cpu_addr = vlSelfRef.top__DOT__cpu_inst__DOT__PC;
            }
        } else if ((2U & (IData)(vlSelfRef.top__DOT__cpu_inst__DOT__state))) {
            if ((1U & (IData)(vlSelfRef.top__DOT__cpu_inst__DOT__state))) {
                vlSelfRef.top__DOT__cpu_dout = vlSelfRef.top__DOT__cpu_inst__DOT__regfile;
                vlSelfRef.top__DOT__cpu_inst__DOT__AI = 0U;
                vlSelfRef.top__DOT__cpu_inst__DOT__PC_temp 
                    = vlSelfRef.top__DOT__cpu_inst__DOT__PC;
            } else {
                vlSelfRef.top__DOT__cpu_dout = ((1U 
                                                 & ((~ (IData)(vlSelfRef.irq_n)) 
                                                    | (IData)(vlSelfRef.top__DOT__cpu_inst__DOT__NMI_edge)))
                                                 ? 
                                                (0x00000020U 
                                                 | (((IData)(vlSelfRef.top__DOT__cpu_inst__DOT__N) 
                                                     << 7U) 
                                                    | (((IData)(vlSelfRef.top__DOT__cpu_inst__DOT__V) 
                                                        << 6U) 
                                                       | (IData)(vlSelfRef.__VdfgRegularize_hebeb780c_0_19))))
                                                 : (IData)(vlSelfRef.top__DOT__cpu_inst__DOT__P));
                vlSelfRef.top__DOT__cpu_inst__DOT__AI 
                    = vlSelfRef.top__DOT__cpu_inst__DOT__ADD;
                vlSelfRef.top__DOT__cpu_inst__DOT__PC_temp 
                    = ((IData)(vlSelfRef.top__DOT__cpu_inst__DOT__res)
                        ? 0xfffcU : ((IData)(vlSelfRef.top__DOT__cpu_inst__DOT__NMI_edge)
                                      ? 0xfffaU : 0xfffeU));
            }
            top__DOT__cpu_inst__DOT__BI = (0x000000ffU 
                                           & (IData)(__VdfgRegularize_hebeb780c_0_16));
            vlSelfRef.top__DOT__cpu_addr = vlSelfRef.__VdfgRegularize_hebeb780c_0_8;
        } else {
            vlSelfRef.top__DOT__cpu_dout = vlSelfRef.__VdfgRegularize_hebeb780c_0_11;
            vlSelfRef.top__DOT__cpu_inst__DOT__AI = vlSelfRef.__VdfgRegularize_hebeb780c_0_13;
            top__DOT__cpu_inst__DOT__BI = (0x000000ffU 
                                           & 0U);
            vlSelfRef.top__DOT__cpu_addr = vlSelfRef.__VdfgRegularize_hebeb780c_0_9;
            vlSelfRef.top__DOT__cpu_inst__DOT__PC_temp 
                = vlSelfRef.top__DOT__cpu_inst__DOT__PC;
        }
    } else {
        vlSelfRef.top__DOT__cpu_dout = vlSelfRef.top__DOT__cpu_inst__DOT__regfile;
        if ((4U & (IData)(vlSelfRef.top__DOT__cpu_inst__DOT__state))) {
            if ((2U & (IData)(vlSelfRef.top__DOT__cpu_inst__DOT__state))) {
                if ((1U & (IData)(vlSelfRef.top__DOT__cpu_inst__DOT__state))) {
                    vlSelfRef.top__DOT__cpu_inst__DOT__AI = 0U;
                    vlSelfRef.top__DOT__cpu_addr = vlSelfRef.__VdfgRegularize_hebeb780c_0_10;
                    vlSelfRef.top__DOT__cpu_inst__DOT__PC_temp 
                        = (((IData)(vlSelfRef.top__DOT__cpu_inst__DOT__ADD) 
                            << 8U) | (0x000000ffU & (IData)(vlSelfRef.top__DOT__cpu_inst__DOT__PC)));
                } else {
                    vlSelfRef.top__DOT__cpu_inst__DOT__AI 
                        = vlSelfRef.top__DOT__cpu_inst__DOT__ABH;
                    vlSelfRef.top__DOT__cpu_addr = vlSelfRef.__VdfgRegularize_hebeb780c_0_5;
                    vlSelfRef.top__DOT__cpu_inst__DOT__PC_temp 
                        = vlSelfRef.__VdfgRegularize_hebeb780c_0_5;
                }
                top__DOT__cpu_inst__DOT__BI = (0x000000ffU 
                                               & (IData)(__VdfgRegularize_hebeb780c_0_16));
            } else {
                vlSelfRef.top__DOT__cpu_inst__DOT__AI 
                    = __VdfgRegularize_hebeb780c_0_16;
                if ((1U & (IData)(vlSelfRef.top__DOT__cpu_inst__DOT__state))) {
                    top__DOT__cpu_inst__DOT__BI = (0x000000ffU 
                                                   & (IData)(vlSelfRef.top__DOT__cpu_inst__DOT__PC));
                    vlSelfRef.top__DOT__cpu_addr = vlSelfRef.top__DOT__cpu_inst__DOT__PC;
                } else {
                    top__DOT__cpu_inst__DOT__BI = (0x000000ffU 
                                                   & (IData)(vlSelfRef.top__DOT__cpu_inst__DOT__DIMUX));
                    vlSelfRef.top__DOT__cpu_addr = vlSelfRef.__VdfgRegularize_hebeb780c_0_10;
                }
                vlSelfRef.top__DOT__cpu_inst__DOT__PC_temp 
                    = vlSelfRef.top__DOT__cpu_inst__DOT__PC;
            }
        } else {
            if ((2U & (IData)(vlSelfRef.top__DOT__cpu_inst__DOT__state))) {
                vlSelfRef.top__DOT__cpu_inst__DOT__AI 
                    = vlSelfRef.__VdfgRegularize_hebeb780c_0_12;
                top__DOT__cpu_inst__DOT__BI = (0x000000ffU 
                                               & (IData)(vlSelfRef.top__DOT__cpu_inst__DOT__DIMUX));
            } else {
                vlSelfRef.top__DOT__cpu_inst__DOT__AI = 0U;
                top__DOT__cpu_inst__DOT__BI = (0x000000ffU 
                                               & (IData)(__VdfgRegularize_hebeb780c_0_18));
            }
            vlSelfRef.top__DOT__cpu_addr = __VdfgRegularize_hebeb780c_0_1;
            vlSelfRef.top__DOT__cpu_inst__DOT__PC_temp 
                = vlSelfRef.top__DOT__cpu_inst__DOT__PC;
        }
    }
    top__DOT__cpu_inst__DOT__ALU__DOT__temp_logic = 
        ((IData)(vlSelfRef.top__DOT__cpu_inst__DOT__alu_shift_right)
          ? ((0x00000100U & ((IData)(vlSelfRef.top__DOT__cpu_inst__DOT__AI) 
                             << 8U)) | (((IData)(vlSelfRef.top__DOT__cpu_inst__DOT__CI) 
                                         << 7U) | (0x0000007fU 
                                                   & ((IData)(vlSelfRef.top__DOT__cpu_inst__DOT__AI) 
                                                      >> 1U))))
          : ((2U & (IData)(vlSelfRef.top__DOT__cpu_inst__DOT__alu_op))
              ? ((1U & (IData)(vlSelfRef.top__DOT__cpu_inst__DOT__alu_op))
                  ? (IData)(vlSelfRef.top__DOT__cpu_inst__DOT__AI)
                  : ((IData)(vlSelfRef.top__DOT__cpu_inst__DOT__AI) 
                     ^ (IData)(top__DOT__cpu_inst__DOT__BI)))
              : ((1U & (IData)(vlSelfRef.top__DOT__cpu_inst__DOT__alu_op))
                  ? ((IData)(vlSelfRef.top__DOT__cpu_inst__DOT__AI) 
                     & (IData)(top__DOT__cpu_inst__DOT__BI))
                  : ((IData)(vlSelfRef.top__DOT__cpu_inst__DOT__AI) 
                     | (IData)(top__DOT__cpu_inst__DOT__BI)))));
    vlSelfRef.top__DOT__cpu_inst__DOT__ALU__DOT__temp_BI 
        = (0x000000ffU & ((8U & (IData)(vlSelfRef.top__DOT__cpu_inst__DOT__alu_op))
                           ? ((4U & (IData)(vlSelfRef.top__DOT__cpu_inst__DOT__alu_op))
                               ? 0U : (IData)(top__DOT__cpu_inst__DOT__ALU__DOT__temp_logic))
                           : ((4U & (IData)(vlSelfRef.top__DOT__cpu_inst__DOT__alu_op))
                               ? (~ (IData)(top__DOT__cpu_inst__DOT__BI))
                               : (IData)(top__DOT__cpu_inst__DOT__BI))));
    vlSelfRef.top__DOT__vgc_inst__DOT__vgc_reg_sel 
        = ((0xa000U <= (IData)(vlSelfRef.top__DOT__cpu_addr)) 
           & (0xa01fU >= (IData)(vlSelfRef.top__DOT__cpu_addr)));
    vlSelfRef.top__DOT__xmc_reg_sel = ((0xba00U <= (IData)(vlSelfRef.top__DOT__cpu_addr)) 
                                       & (0xba3fU >= (IData)(vlSelfRef.top__DOT__cpu_addr)));
    vlSelfRef.top__DOT__xmc_win_sel = ((0xbc00U <= (IData)(vlSelfRef.top__DOT__cpu_addr)) 
                                       & (0xbfffU >= (IData)(vlSelfRef.top__DOT__cpu_addr)));
    vlSelfRef.top__DOT__vgc_inst__DOT__char_ram_sel 
        = ((0xaa00U <= (IData)(vlSelfRef.top__DOT__cpu_addr)) 
           & (0xb1cfU >= (IData)(vlSelfRef.top__DOT__cpu_addr)));
    vlSelfRef.top__DOT__vgc_inst__DOT__color_ram_sel 
        = ((0xb1d0U <= (IData)(vlSelfRef.top__DOT__cpu_addr)) 
           & (0xb99fU >= (IData)(vlSelfRef.top__DOT__cpu_addr)));
    vlSelfRef.top__DOT__vgc_inst__DOT__spr_reg_sel 
        = ((0xa040U <= (IData)(vlSelfRef.top__DOT__cpu_addr)) 
           & (0xa0bfU >= (IData)(vlSelfRef.top__DOT__cpu_addr)));
    vlSelfRef.top__DOT__vgc_inst__DOT__tile_reg_sel 
        = ((0xa0c0U <= (IData)(vlSelfRef.top__DOT__cpu_addr)) 
           & (0xa0dfU >= (IData)(vlSelfRef.top__DOT__cpu_addr)));
    vlSelfRef.top__DOT__xmc_win_enabled = (1U & (vlSelfRef.top__DOT__xmc_regs[22U] 
                                                 >> 
                                                 (3U 
                                                  & ((IData)(vlSelfRef.top__DOT__cpu_addr) 
                                                     >> 8U))));
    vlSelfRef.top__DOT__sid1_reg_sel = ((0xd400U <= (IData)(vlSelfRef.top__DOT__cpu_addr)) 
                                        & (0xd41fU 
                                           >= (IData)(vlSelfRef.top__DOT__cpu_addr)));
    vlSelfRef.top__DOT__sid2_reg_sel = ((0xd420U <= (IData)(vlSelfRef.top__DOT__cpu_addr)) 
                                        & (0xd43fU 
                                           >= (IData)(vlSelfRef.top__DOT__cpu_addr)));
    top__DOT____VdfgRegularize_hbded3b40_0_0 = (0x0000003fU 
                                                & ((6U 
                                                    & ((IData)(vlSelfRef.top__DOT__cpu_addr) 
                                                       >> 7U)) 
                                                   + 
                                                   (3U 
                                                    & ((IData)(vlSelfRef.top__DOT__cpu_addr) 
                                                       >> 8U))));
    vlSelfRef.top__DOT__blt_reg_sel = ((0xba83U <= (IData)(vlSelfRef.top__DOT__cpu_addr)) 
                                       & (0xba9bU >= (IData)(vlSelfRef.top__DOT__cpu_addr)));
    top__DOT__cpu_inst__DOT__ALU__DOT__temp_l = (0x0000001fU 
                                                 & ((0x0000000fU 
                                                     & (IData)(top__DOT__cpu_inst__DOT__ALU__DOT__temp_logic)) 
                                                    + 
                                                    ((0x0000000fU 
                                                      & (IData)(vlSelfRef.top__DOT__cpu_inst__DOT__ALU__DOT__temp_BI)) 
                                                     + 
                                                     ((~ 
                                                       ((IData)(vlSelfRef.top__DOT__cpu_inst__DOT__alu_shift_right) 
                                                        | (3U 
                                                           == 
                                                           (3U 
                                                            & ((IData)(vlSelfRef.top__DOT__cpu_inst__DOT__alu_op) 
                                                               >> 2U))))) 
                                                      & (IData)(vlSelfRef.top__DOT__cpu_inst__DOT__CI)))));
    vlSelfRef.top__DOT__xmc_addr = ((0x00070000U & 
                                     (vlSelfRef.top__DOT__xmc_regs
                                      [(0x0000003fU 
                                        & ((IData)(0x1aU) 
                                           + (IData)(top__DOT____VdfgRegularize_hbded3b40_0_0)))] 
                                      << 0x00000010U)) 
                                    | ((vlSelfRef.top__DOT__xmc_regs
                                        [(0x0000003fU 
                                          & ((IData)(0x19U) 
                                             + (IData)(top__DOT____VdfgRegularize_hbded3b40_0_0)))] 
                                        << 8U) | (0x000000ffU 
                                                  & (IData)(vlSelfRef.top__DOT__cpu_addr))));
    vlSelfRef.top__DOT__blt_cpu_rdata = 0U;
    if (vlSelfRef.top__DOT__blt_reg_sel) {
        vlSelfRef.top__DOT__blt_cpu_rdata = ((0x18U 
                                              >= (0x0000001fU 
                                                  & ((IData)(vlSelfRef.top__DOT__cpu_addr) 
                                                     - (IData)(3U))))
                                              ? vlSelfRef.top__DOT__blt_inst__DOT__regs
                                             [(0x0000001fU 
                                               & ((IData)(vlSelfRef.top__DOT__cpu_addr) 
                                                  - (IData)(3U)))]
                                              : 0U);
    }
    vlSelfRef.top__DOT__cpu_inst__DOT__ALU__DOT__temp_HC 
        = (1U & (((IData)(top__DOT__cpu_inst__DOT__ALU__DOT__temp_l) 
                  >> 4U) | ((IData)(vlSelfRef.top__DOT__cpu_inst__DOT____Vcellinp__ALU__BCD) 
                            & (5U <= (7U & ((IData)(top__DOT__cpu_inst__DOT__ALU__DOT__temp_l) 
                                            >> 1U))))));
    vlSelfRef.top__DOT__cpu_inst__DOT__ALU__DOT__temp_h 
        = (0x0000001fU & (((IData)(top__DOT__cpu_inst__DOT__ALU__DOT__temp_logic) 
                           >> 4U) + ((0x0000000fU & 
                                      ((IData)(vlSelfRef.top__DOT__cpu_inst__DOT__ALU__DOT__temp_BI) 
                                       >> 4U)) + (IData)(vlSelfRef.top__DOT__cpu_inst__DOT__ALU__DOT__temp_HC))));
    vlSelfRef.top__DOT__cpu_inst__DOT__ALU__DOT__temp 
        = (((IData)(vlSelfRef.top__DOT__cpu_inst__DOT__ALU__DOT__temp_h) 
            << 4U) | (0x0000000fU & (IData)(top__DOT__cpu_inst__DOT__ALU__DOT__temp_l)));
}

void Vtop___024root___ico_sequent__TOP__1(Vtop___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vtop___024root___ico_sequent__TOP__1\n"); );
    Vtop__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
    // Body
    vlSelfRef.dbg_peek_data = ((IData)(vlSelfRef.dbg_peek_en)
                                ? (((IData)(vlSelfRef.top__DOT__vgc_inst__DOT__dbg_char_sel) 
                                    | ((IData)(vlSelfRef.top__DOT__vgc_inst__DOT__dbg_color_sel) 
                                       | ((IData)(vlSelfRef.top__DOT__vgc_inst__DOT__dbg_vgc_sel) 
                                          | (((0xa0c0U 
                                               <= (IData)(vlSelfRef.dbg_peek_addr)) 
                                              & (0xa0dfU 
                                                 >= (IData)(vlSelfRef.dbg_peek_addr))) 
                                             | ((0xa040U 
                                                 <= (IData)(vlSelfRef.dbg_peek_addr)) 
                                                & (0xa0bfU 
                                                   >= (IData)(vlSelfRef.dbg_peek_addr)))))))
                                    ? (IData)(vlSelfRef.top__DOT__vgc_dbg_rdata)
                                    : (((0xd400U <= (IData)(vlSelfRef.dbg_peek_addr)) 
                                        & (0xd41fU 
                                           >= (IData)(vlSelfRef.dbg_peek_addr)))
                                        ? (IData)(vlSymsp->TOP__top__DOT__sid1_inst.dout)
                                        : (((0xd420U 
                                             <= (IData)(vlSelfRef.dbg_peek_addr)) 
                                            & (0xd43fU 
                                               >= (IData)(vlSelfRef.dbg_peek_addr)))
                                            ? (IData)(vlSymsp->TOP__top__DOT__sid2_inst.dout)
                                            : ((0xd440U 
                                                == (IData)(vlSelfRef.dbg_peek_addr))
                                                ? (IData)(vlSelfRef.top__DOT__sid_cfg_reg)
                                                : (
                                                   (0xc000U 
                                                    <= (IData)(vlSelfRef.dbg_peek_addr))
                                                    ? 
                                                   ((IData)(vlSelfRef.top__DOT__ext_rom_active)
                                                     ? vlSelfRef.top__DOT__ext_rom
                                                    [
                                                    (0x00003fffU 
                                                     & (IData)(vlSelfRef.dbg_peek_addr))]
                                                     : vlSelfRef.top__DOT__basic_rom
                                                    [
                                                    (0x00003fffU 
                                                     & (IData)(vlSelfRef.dbg_peek_addr))])
                                                    : vlSelfRef.top__DOT__ram
                                                   [vlSelfRef.dbg_peek_addr])))))
                                : 0U);
}

void Vtop_sid_chip___ico_sequent__TOP__top__DOT__sid1_inst__0(Vtop_sid_chip* vlSelf);
void Vtop_sid_chip___ico_sequent__TOP__top__DOT__sid2_inst__0(Vtop_sid_chip* vlSelf);

void Vtop___024root___eval_ico(Vtop___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vtop___024root___eval_ico\n"); );
    Vtop__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
    // Body
    if ((1ULL & vlSelfRef.__VicoTriggered[0U])) {
        Vtop___024root___ico_sequent__TOP__0(vlSelf);
        Vtop_sid_chip___ico_sequent__TOP__top__DOT__sid1_inst__0((&vlSymsp->TOP__top__DOT__sid1_inst));
        Vtop_sid_chip___ico_sequent__TOP__top__DOT__sid2_inst__0((&vlSymsp->TOP__top__DOT__sid2_inst));
        Vtop___024root___ico_sequent__TOP__1(vlSelf);
    }
}

#ifdef VL_DEBUG
VL_ATTR_COLD void Vtop___024root___dump_triggers__ico(const VlUnpacked<QData/*63:0*/, 1> &triggers, const std::string &tag);
#endif  // VL_DEBUG

bool Vtop___024root___eval_phase__ico(Vtop___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vtop___024root___eval_phase__ico\n"); );
    Vtop__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
    // Locals
    CData/*0:0*/ __VicoExecute;
    // Body
    Vtop___024root___eval_triggers_vec__ico(vlSelf);
#ifdef VL_DEBUG
    if (VL_UNLIKELY(vlSymsp->_vm_contextp__->debug())) {
        Vtop___024root___dump_triggers__ico(vlSelfRef.__VicoTriggered, "ico"s);
    }
#endif
    __VicoExecute = Vtop___024root___trigger_anySet__ico(vlSelfRef.__VicoTriggered);
    if (__VicoExecute) {
        Vtop___024root___eval_ico(vlSelf);
    }
    return (__VicoExecute);
}

void Vtop___024root___eval_triggers_vec__act(Vtop___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vtop___024root___eval_triggers_vec__act\n"); );
    Vtop__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
    // Body
    vlSelfRef.__VactTriggered[0U] = (QData)((IData)(
                                                    (((((((IData)(vlSelfRef.rst) 
                                                          & (~ (IData)(vlSelfRef.__Vtrigprevexpr___TOP__rst__0))) 
                                                         << 5U) 
                                                        | (((IData)(vlSelfRef.clk) 
                                                            & (~ (IData)(vlSelfRef.__Vtrigprevexpr___TOP__clk__0))) 
                                                           << 4U)) 
                                                       | (((vlSelfRef.__Vtrigprevexpr___TOP__top__DOT__sid2_inst____PVT__v3__DOT__envelope_dac__DOT__bitsum__1.neq(vlSymsp->TOP__top__DOT__sid2_inst.__PVT__v3__DOT__envelope_dac__DOT__bitsum) 
                                                            << 3U) 
                                                           | (vlSelfRef.__Vtrigprevexpr___TOP__top__DOT__sid2_inst____PVT__v3__DOT__waveform_dac__DOT__bitsum__1.neq(vlSymsp->TOP__top__DOT__sid2_inst.__PVT__v3__DOT__waveform_dac__DOT__bitsum) 
                                                              << 2U)) 
                                                          | ((vlSelfRef.__Vtrigprevexpr___TOP__top__DOT__sid2_inst____PVT__v2__DOT__envelope_dac__DOT__bitsum__1.neq(vlSymsp->TOP__top__DOT__sid2_inst.__PVT__v2__DOT__envelope_dac__DOT__bitsum) 
                                                              << 1U) 
                                                             | vlSelfRef.__Vtrigprevexpr___TOP__top__DOT__sid2_inst____PVT__v2__DOT__waveform_dac__DOT__bitsum__1.neq(vlSymsp->TOP__top__DOT__sid2_inst.__PVT__v2__DOT__waveform_dac__DOT__bitsum)))) 
                                                      << 8U) 
                                                     | (((((vlSelfRef.__Vtrigprevexpr___TOP__top__DOT__sid2_inst____PVT__v1__DOT__envelope_dac__DOT__bitsum__1.neq(vlSymsp->TOP__top__DOT__sid2_inst.__PVT__v1__DOT__envelope_dac__DOT__bitsum) 
                                                            << 3U) 
                                                           | (vlSelfRef.__Vtrigprevexpr___TOP__top__DOT__sid2_inst____PVT__v1__DOT__waveform_dac__DOT__bitsum__1.neq(vlSymsp->TOP__top__DOT__sid2_inst.__PVT__v1__DOT__waveform_dac__DOT__bitsum) 
                                                              << 2U)) 
                                                          | ((vlSelfRef.__Vtrigprevexpr___TOP__top__DOT__sid1_inst____PVT__v3__DOT__envelope_dac__DOT__bitsum__1.neq(vlSymsp->TOP__top__DOT__sid1_inst.__PVT__v3__DOT__envelope_dac__DOT__bitsum) 
                                                              << 1U) 
                                                             | vlSelfRef.__Vtrigprevexpr___TOP__top__DOT__sid1_inst____PVT__v3__DOT__waveform_dac__DOT__bitsum__1.neq(vlSymsp->TOP__top__DOT__sid1_inst.__PVT__v3__DOT__waveform_dac__DOT__bitsum))) 
                                                         << 4U) 
                                                        | (((vlSelfRef.__Vtrigprevexpr___TOP__top__DOT__sid1_inst____PVT__v2__DOT__envelope_dac__DOT__bitsum__1.neq(vlSymsp->TOP__top__DOT__sid1_inst.__PVT__v2__DOT__envelope_dac__DOT__bitsum) 
                                                             << 3U) 
                                                            | (vlSelfRef.__Vtrigprevexpr___TOP__top__DOT__sid1_inst____PVT__v2__DOT__waveform_dac__DOT__bitsum__1.neq(vlSymsp->TOP__top__DOT__sid1_inst.__PVT__v2__DOT__waveform_dac__DOT__bitsum) 
                                                               << 2U)) 
                                                           | ((vlSelfRef.__Vtrigprevexpr___TOP__top__DOT__sid1_inst____PVT__v1__DOT__envelope_dac__DOT__bitsum__1.neq(vlSymsp->TOP__top__DOT__sid1_inst.__PVT__v1__DOT__envelope_dac__DOT__bitsum) 
                                                               << 1U) 
                                                              | vlSelfRef.__Vtrigprevexpr___TOP__top__DOT__sid1_inst____PVT__v1__DOT__waveform_dac__DOT__bitsum__1.neq(vlSymsp->TOP__top__DOT__sid1_inst.__PVT__v1__DOT__waveform_dac__DOT__bitsum)))))));
    vlSelfRef.__Vtrigprevexpr___TOP__top__DOT__sid1_inst____PVT__v1__DOT__waveform_dac__DOT__bitsum__1.assign(vlSymsp->TOP__top__DOT__sid1_inst.__PVT__v1__DOT__waveform_dac__DOT__bitsum);
    vlSelfRef.__Vtrigprevexpr___TOP__top__DOT__sid1_inst____PVT__v1__DOT__envelope_dac__DOT__bitsum__1.assign(vlSymsp->TOP__top__DOT__sid1_inst.__PVT__v1__DOT__envelope_dac__DOT__bitsum);
    vlSelfRef.__Vtrigprevexpr___TOP__top__DOT__sid1_inst____PVT__v2__DOT__waveform_dac__DOT__bitsum__1.assign(vlSymsp->TOP__top__DOT__sid1_inst.__PVT__v2__DOT__waveform_dac__DOT__bitsum);
    vlSelfRef.__Vtrigprevexpr___TOP__top__DOT__sid1_inst____PVT__v2__DOT__envelope_dac__DOT__bitsum__1.assign(vlSymsp->TOP__top__DOT__sid1_inst.__PVT__v2__DOT__envelope_dac__DOT__bitsum);
    vlSelfRef.__Vtrigprevexpr___TOP__top__DOT__sid1_inst____PVT__v3__DOT__waveform_dac__DOT__bitsum__1.assign(vlSymsp->TOP__top__DOT__sid1_inst.__PVT__v3__DOT__waveform_dac__DOT__bitsum);
    vlSelfRef.__Vtrigprevexpr___TOP__top__DOT__sid1_inst____PVT__v3__DOT__envelope_dac__DOT__bitsum__1.assign(vlSymsp->TOP__top__DOT__sid1_inst.__PVT__v3__DOT__envelope_dac__DOT__bitsum);
    vlSelfRef.__Vtrigprevexpr___TOP__top__DOT__sid2_inst____PVT__v1__DOT__waveform_dac__DOT__bitsum__1.assign(vlSymsp->TOP__top__DOT__sid2_inst.__PVT__v1__DOT__waveform_dac__DOT__bitsum);
    vlSelfRef.__Vtrigprevexpr___TOP__top__DOT__sid2_inst____PVT__v1__DOT__envelope_dac__DOT__bitsum__1.assign(vlSymsp->TOP__top__DOT__sid2_inst.__PVT__v1__DOT__envelope_dac__DOT__bitsum);
    vlSelfRef.__Vtrigprevexpr___TOP__top__DOT__sid2_inst____PVT__v2__DOT__waveform_dac__DOT__bitsum__1.assign(vlSymsp->TOP__top__DOT__sid2_inst.__PVT__v2__DOT__waveform_dac__DOT__bitsum);
    vlSelfRef.__Vtrigprevexpr___TOP__top__DOT__sid2_inst____PVT__v2__DOT__envelope_dac__DOT__bitsum__1.assign(vlSymsp->TOP__top__DOT__sid2_inst.__PVT__v2__DOT__envelope_dac__DOT__bitsum);
    vlSelfRef.__Vtrigprevexpr___TOP__top__DOT__sid2_inst____PVT__v3__DOT__waveform_dac__DOT__bitsum__1.assign(vlSymsp->TOP__top__DOT__sid2_inst.__PVT__v3__DOT__waveform_dac__DOT__bitsum);
    vlSelfRef.__Vtrigprevexpr___TOP__top__DOT__sid2_inst____PVT__v3__DOT__envelope_dac__DOT__bitsum__1.assign(vlSymsp->TOP__top__DOT__sid2_inst.__PVT__v3__DOT__envelope_dac__DOT__bitsum);
    vlSelfRef.__Vtrigprevexpr___TOP__clk__0 = vlSelfRef.clk;
    vlSelfRef.__Vtrigprevexpr___TOP__rst__0 = vlSelfRef.rst;
    if (VL_UNLIKELY(((1U & (~ (IData)(vlSelfRef.__VactDidInit)))))) {
        vlSelfRef.__VactDidInit = 1U;
        vlSelfRef.__VactTriggered[0U] = (1ULL | vlSelfRef.__VactTriggered[0U]);
        vlSelfRef.__VactTriggered[0U] = (2ULL | vlSelfRef.__VactTriggered[0U]);
        vlSelfRef.__VactTriggered[0U] = (4ULL | vlSelfRef.__VactTriggered[0U]);
        vlSelfRef.__VactTriggered[0U] = (8ULL | vlSelfRef.__VactTriggered[0U]);
        vlSelfRef.__VactTriggered[0U] = (0x0000000000000010ULL 
                                         | vlSelfRef.__VactTriggered[0U]);
        vlSelfRef.__VactTriggered[0U] = (0x0000000000000020ULL 
                                         | vlSelfRef.__VactTriggered[0U]);
        vlSelfRef.__VactTriggered[0U] = (0x0000000000000040ULL 
                                         | vlSelfRef.__VactTriggered[0U]);
        vlSelfRef.__VactTriggered[0U] = (0x0000000000000080ULL 
                                         | vlSelfRef.__VactTriggered[0U]);
        vlSelfRef.__VactTriggered[0U] = (0x0000000000000100ULL 
                                         | vlSelfRef.__VactTriggered[0U]);
        vlSelfRef.__VactTriggered[0U] = (0x0000000000000200ULL 
                                         | vlSelfRef.__VactTriggered[0U]);
        vlSelfRef.__VactTriggered[0U] = (0x0000000000000400ULL 
                                         | vlSelfRef.__VactTriggered[0U]);
        vlSelfRef.__VactTriggered[0U] = (0x0000000000000800ULL 
                                         | vlSelfRef.__VactTriggered[0U]);
    }
}

bool Vtop___024root___trigger_anySet__act(const VlUnpacked<QData/*63:0*/, 1> &in) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vtop___024root___trigger_anySet__act\n"); );
    // Locals
    IData/*31:0*/ n;
    // Body
    n = 0U;
    do {
        if (in[n]) {
            return (1U);
        }
        n = ((IData)(1U) + n);
    } while ((1U > n));
    return (0U);
}

void Vtop_sid_chip___act_sequent__TOP__top__DOT__sid1_inst__0(Vtop_sid_chip* vlSelf);
void Vtop_sid_chip___act_sequent__TOP__top__DOT__sid1_inst__1(Vtop_sid_chip* vlSelf);
void Vtop_sid_chip___act_sequent__TOP__top__DOT__sid1_inst__2(Vtop_sid_chip* vlSelf);
void Vtop_sid_chip___act_sequent__TOP__top__DOT__sid1_inst__3(Vtop_sid_chip* vlSelf);
void Vtop_sid_chip___act_sequent__TOP__top__DOT__sid1_inst__4(Vtop_sid_chip* vlSelf);
void Vtop_sid_chip___act_sequent__TOP__top__DOT__sid1_inst__5(Vtop_sid_chip* vlSelf);

void Vtop___024root___eval_act(Vtop___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vtop___024root___eval_act\n"); );
    Vtop__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
    // Body
    if ((2ULL & vlSelfRef.__VactTriggered[0U])) {
        Vtop_sid_chip___act_sequent__TOP__top__DOT__sid1_inst__0((&vlSymsp->TOP__top__DOT__sid1_inst));
    }
    if ((8ULL & vlSelfRef.__VactTriggered[0U])) {
        Vtop_sid_chip___act_sequent__TOP__top__DOT__sid1_inst__1((&vlSymsp->TOP__top__DOT__sid1_inst));
    }
    if ((0x0000000000000020ULL & vlSelfRef.__VactTriggered[0U])) {
        Vtop_sid_chip___act_sequent__TOP__top__DOT__sid1_inst__2((&vlSymsp->TOP__top__DOT__sid1_inst));
    }
    if ((0x0000000000000080ULL & vlSelfRef.__VactTriggered[0U])) {
        Vtop_sid_chip___act_sequent__TOP__top__DOT__sid1_inst__0((&vlSymsp->TOP__top__DOT__sid2_inst));
    }
    if ((0x0000000000000200ULL & vlSelfRef.__VactTriggered[0U])) {
        Vtop_sid_chip___act_sequent__TOP__top__DOT__sid1_inst__1((&vlSymsp->TOP__top__DOT__sid2_inst));
    }
    if ((0x0000000000000800ULL & vlSelfRef.__VactTriggered[0U])) {
        Vtop_sid_chip___act_sequent__TOP__top__DOT__sid1_inst__2((&vlSymsp->TOP__top__DOT__sid2_inst));
    }
    if ((1ULL & vlSelfRef.__VactTriggered[0U])) {
        Vtop_sid_chip___act_sequent__TOP__top__DOT__sid1_inst__3((&vlSymsp->TOP__top__DOT__sid1_inst));
    }
    if ((4ULL & vlSelfRef.__VactTriggered[0U])) {
        Vtop_sid_chip___act_sequent__TOP__top__DOT__sid1_inst__4((&vlSymsp->TOP__top__DOT__sid1_inst));
    }
    if ((0x0000000000000010ULL & vlSelfRef.__VactTriggered[0U])) {
        Vtop_sid_chip___act_sequent__TOP__top__DOT__sid1_inst__5((&vlSymsp->TOP__top__DOT__sid1_inst));
    }
    if ((0x0000000000000040ULL & vlSelfRef.__VactTriggered[0U])) {
        Vtop_sid_chip___act_sequent__TOP__top__DOT__sid1_inst__3((&vlSymsp->TOP__top__DOT__sid2_inst));
    }
    if ((0x0000000000000100ULL & vlSelfRef.__VactTriggered[0U])) {
        Vtop_sid_chip___act_sequent__TOP__top__DOT__sid1_inst__4((&vlSymsp->TOP__top__DOT__sid2_inst));
    }
    if ((0x0000000000000400ULL & vlSelfRef.__VactTriggered[0U])) {
        Vtop_sid_chip___act_sequent__TOP__top__DOT__sid1_inst__5((&vlSymsp->TOP__top__DOT__sid2_inst));
    }
}

void Vtop___024root___nba_sequent__TOP__0(Vtop___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vtop___024root___nba_sequent__TOP__0\n"); );
    Vtop__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
    // Locals
    CData/*7:0*/ top__DOT__blt_vgc_wdata;
    top__DOT__blt_vgc_wdata = 0;
    IData/*23:0*/ top__DOT__blt_inst__DOT__src_addr;
    top__DOT__blt_inst__DOT__src_addr = 0;
    IData/*23:0*/ top__DOT__blt_inst__DOT__dst_addr;
    top__DOT__blt_inst__DOT__dst_addr = 0;
    IData/*16:0*/ top__DOT__vgc_inst__DOT____Vlvbound_h3e9a9ec3__0;
    top__DOT__vgc_inst__DOT____Vlvbound_h3e9a9ec3__0 = 0;
    SData/*8:0*/ top__DOT__vgc_inst__DOT____Vlvbound_h6cff2040__0;
    top__DOT__vgc_inst__DOT____Vlvbound_h6cff2040__0 = 0;
    SData/*9:0*/ top__DOT__vgc_inst__DOT__text_line;
    top__DOT__vgc_inst__DOT__text_line = 0;
    CData/*3:0*/ top__DOT__vgc_inst__DOT__spr_pixel;
    top__DOT__vgc_inst__DOT__spr_pixel = 0;
    CData/*1:0*/ top__DOT__vgc_inst__DOT__spr_pixel_pri;
    top__DOT__vgc_inst__DOT__spr_pixel_pri = 0;
    CData/*0:0*/ top__DOT__vgc_inst__DOT__spr_pixel_hit;
    top__DOT__vgc_inst__DOT__spr_pixel_hit = 0;
    SData/*14:0*/ top__DOT__vgc_inst__DOT__tile_blt_addr;
    top__DOT__vgc_inst__DOT__tile_blt_addr = 0;
    CData/*7:0*/ top__DOT__vgc_inst__DOT__tile_blt_wdata;
    top__DOT__vgc_inst__DOT__tile_blt_wdata = 0;
    CData/*3:0*/ top__DOT__vgc_inst__DOT__cur_fg_d2;
    top__DOT__vgc_inst__DOT__cur_fg_d2 = 0;
    CData/*3:0*/ top__DOT__vgc_inst__DOT__cur_gfx_d2;
    top__DOT__vgc_inst__DOT__cur_gfx_d2 = 0;
    CData/*0:0*/ top__DOT__vgc_inst__DOT__pixel_on_d2;
    top__DOT__vgc_inst__DOT__pixel_on_d2 = 0;
    SData/*11:0*/ top__DOT__vgc_inst__DOT__text_pixel_d2;
    top__DOT__vgc_inst__DOT__text_pixel_d2 = 0;
    SData/*11:0*/ top__DOT__vgc_inst__DOT__gfx_pixel_d2;
    top__DOT__vgc_inst__DOT__gfx_pixel_d2 = 0;
    CData/*0:0*/ __Vfunc_top__DOT__blt_inst__DOT__rect_fits__0__Vfuncout;
    __Vfunc_top__DOT__blt_inst__DOT__rect_fits__0__Vfuncout = 0;
    IData/*23:0*/ __Vfunc_top__DOT__blt_inst__DOT__rect_fits__0__base;
    __Vfunc_top__DOT__blt_inst__DOT__rect_fits__0__base = 0;
    SData/*15:0*/ __Vfunc_top__DOT__blt_inst__DOT__rect_fits__0__w;
    __Vfunc_top__DOT__blt_inst__DOT__rect_fits__0__w = 0;
    SData/*15:0*/ __Vfunc_top__DOT__blt_inst__DOT__rect_fits__0__h;
    __Vfunc_top__DOT__blt_inst__DOT__rect_fits__0__h = 0;
    SData/*15:0*/ __Vfunc_top__DOT__blt_inst__DOT__rect_fits__0__stride;
    __Vfunc_top__DOT__blt_inst__DOT__rect_fits__0__stride = 0;
    IData/*19:0*/ __Vfunc_top__DOT__blt_inst__DOT__rect_fits__0__sz;
    __Vfunc_top__DOT__blt_inst__DOT__rect_fits__0__sz = 0;
    IData/*31:0*/ __Vfunc_top__DOT__blt_inst__DOT__rect_fits__0__last_row_start;
    __Vfunc_top__DOT__blt_inst__DOT__rect_fits__0__last_row_start = 0;
    IData/*31:0*/ __Vfunc_top__DOT__blt_inst__DOT__rect_fits__0__last_byte;
    __Vfunc_top__DOT__blt_inst__DOT__rect_fits__0__last_byte = 0;
    IData/*19:0*/ __Vfunc_top__DOT__blt_inst__DOT__space_size__1__Vfuncout;
    __Vfunc_top__DOT__blt_inst__DOT__space_size__1__Vfuncout = 0;
    CData/*2:0*/ __Vfunc_top__DOT__blt_inst__DOT__space_size__1__sp;
    __Vfunc_top__DOT__blt_inst__DOT__space_size__1__sp = 0;
    CData/*0:0*/ __Vfunc_top__DOT__blt_inst__DOT__rect_fits__2__Vfuncout;
    __Vfunc_top__DOT__blt_inst__DOT__rect_fits__2__Vfuncout = 0;
    IData/*23:0*/ __Vfunc_top__DOT__blt_inst__DOT__rect_fits__2__base;
    __Vfunc_top__DOT__blt_inst__DOT__rect_fits__2__base = 0;
    SData/*15:0*/ __Vfunc_top__DOT__blt_inst__DOT__rect_fits__2__w;
    __Vfunc_top__DOT__blt_inst__DOT__rect_fits__2__w = 0;
    SData/*15:0*/ __Vfunc_top__DOT__blt_inst__DOT__rect_fits__2__h;
    __Vfunc_top__DOT__blt_inst__DOT__rect_fits__2__h = 0;
    SData/*15:0*/ __Vfunc_top__DOT__blt_inst__DOT__rect_fits__2__stride;
    __Vfunc_top__DOT__blt_inst__DOT__rect_fits__2__stride = 0;
    IData/*19:0*/ __Vfunc_top__DOT__blt_inst__DOT__rect_fits__2__sz;
    __Vfunc_top__DOT__blt_inst__DOT__rect_fits__2__sz = 0;
    IData/*31:0*/ __Vfunc_top__DOT__blt_inst__DOT__rect_fits__2__last_row_start;
    __Vfunc_top__DOT__blt_inst__DOT__rect_fits__2__last_row_start = 0;
    IData/*31:0*/ __Vfunc_top__DOT__blt_inst__DOT__rect_fits__2__last_byte;
    __Vfunc_top__DOT__blt_inst__DOT__rect_fits__2__last_byte = 0;
    IData/*19:0*/ __Vfunc_top__DOT__blt_inst__DOT__space_size__3__Vfuncout;
    __Vfunc_top__DOT__blt_inst__DOT__space_size__3__Vfuncout = 0;
    CData/*2:0*/ __Vfunc_top__DOT__blt_inst__DOT__space_size__3__sp;
    __Vfunc_top__DOT__blt_inst__DOT__space_size__3__sp = 0;
    CData/*0:0*/ __Vfunc_top__DOT__blt_inst__DOT__write_protect_ok__4__Vfuncout;
    __Vfunc_top__DOT__blt_inst__DOT__write_protect_ok__4__Vfuncout = 0;
    CData/*2:0*/ __Vfunc_top__DOT__blt_inst__DOT__write_protect_ok__4__space;
    __Vfunc_top__DOT__blt_inst__DOT__write_protect_ok__4__space = 0;
    IData/*23:0*/ __Vfunc_top__DOT__blt_inst__DOT__write_protect_ok__4__base;
    __Vfunc_top__DOT__blt_inst__DOT__write_protect_ok__4__base = 0;
    SData/*15:0*/ __Vfunc_top__DOT__blt_inst__DOT__write_protect_ok__4__w;
    __Vfunc_top__DOT__blt_inst__DOT__write_protect_ok__4__w = 0;
    SData/*15:0*/ __Vfunc_top__DOT__blt_inst__DOT__write_protect_ok__4__h;
    __Vfunc_top__DOT__blt_inst__DOT__write_protect_ok__4__h = 0;
    SData/*15:0*/ __Vfunc_top__DOT__blt_inst__DOT__write_protect_ok__4__stride;
    __Vfunc_top__DOT__blt_inst__DOT__write_protect_ok__4__stride = 0;
    IData/*31:0*/ __Vfunc_top__DOT__blt_inst__DOT__write_protect_ok__4__last_byte;
    __Vfunc_top__DOT__blt_inst__DOT__write_protect_ok__4__last_byte = 0;
    SData/*10:0*/ __Vfunc_top__DOT__vgc_inst__DOT__screen_addr__5__Vfuncout;
    __Vfunc_top__DOT__vgc_inst__DOT__screen_addr__5__Vfuncout = 0;
    CData/*6:0*/ __Vfunc_top__DOT__vgc_inst__DOT__screen_addr__5__col;
    __Vfunc_top__DOT__vgc_inst__DOT__screen_addr__5__col = 0;
    CData/*4:0*/ __Vfunc_top__DOT__vgc_inst__DOT__screen_addr__5__rr;
    __Vfunc_top__DOT__vgc_inst__DOT__screen_addr__5__rr = 0;
    SData/*10:0*/ __Vfunc_top__DOT__vgc_inst__DOT__screen_addr__6__Vfuncout;
    __Vfunc_top__DOT__vgc_inst__DOT__screen_addr__6__Vfuncout = 0;
    CData/*6:0*/ __Vfunc_top__DOT__vgc_inst__DOT__screen_addr__6__col;
    __Vfunc_top__DOT__vgc_inst__DOT__screen_addr__6__col = 0;
    CData/*4:0*/ __Vfunc_top__DOT__vgc_inst__DOT__screen_addr__6__rr;
    __Vfunc_top__DOT__vgc_inst__DOT__screen_addr__6__rr = 0;
    SData/*10:0*/ __Vfunc_top__DOT__vgc_inst__DOT__screen_addr__7__Vfuncout;
    __Vfunc_top__DOT__vgc_inst__DOT__screen_addr__7__Vfuncout = 0;
    CData/*6:0*/ __Vfunc_top__DOT__vgc_inst__DOT__screen_addr__7__col;
    __Vfunc_top__DOT__vgc_inst__DOT__screen_addr__7__col = 0;
    CData/*4:0*/ __Vfunc_top__DOT__vgc_inst__DOT__screen_addr__7__row;
    __Vfunc_top__DOT__vgc_inst__DOT__screen_addr__7__row = 0;
    CData/*4:0*/ __Vfunc_top__DOT__vgc_inst__DOT__screen_addr__7__rr;
    __Vfunc_top__DOT__vgc_inst__DOT__screen_addr__7__rr = 0;
    SData/*10:0*/ __Vfunc_top__DOT__vgc_inst__DOT__screen_addr__8__Vfuncout;
    __Vfunc_top__DOT__vgc_inst__DOT__screen_addr__8__Vfuncout = 0;
    CData/*6:0*/ __Vfunc_top__DOT__vgc_inst__DOT__screen_addr__8__col;
    __Vfunc_top__DOT__vgc_inst__DOT__screen_addr__8__col = 0;
    CData/*4:0*/ __Vfunc_top__DOT__vgc_inst__DOT__screen_addr__8__row;
    __Vfunc_top__DOT__vgc_inst__DOT__screen_addr__8__row = 0;
    CData/*4:0*/ __Vfunc_top__DOT__vgc_inst__DOT__screen_addr__8__rr;
    __Vfunc_top__DOT__vgc_inst__DOT__screen_addr__8__rr = 0;
    SData/*10:0*/ __Vfunc_top__DOT__vgc_inst__DOT__screen_addr__9__Vfuncout;
    __Vfunc_top__DOT__vgc_inst__DOT__screen_addr__9__Vfuncout = 0;
    CData/*6:0*/ __Vfunc_top__DOT__vgc_inst__DOT__screen_addr__9__col;
    __Vfunc_top__DOT__vgc_inst__DOT__screen_addr__9__col = 0;
    CData/*4:0*/ __Vfunc_top__DOT__vgc_inst__DOT__screen_addr__9__row;
    __Vfunc_top__DOT__vgc_inst__DOT__screen_addr__9__row = 0;
    CData/*4:0*/ __Vfunc_top__DOT__vgc_inst__DOT__screen_addr__9__rr;
    __Vfunc_top__DOT__vgc_inst__DOT__screen_addr__9__rr = 0;
    CData/*7:0*/ __Vfunc_top__DOT__vgc_inst__DOT__artist_inst__DOT__gt_char__10__Vfuncout;
    __Vfunc_top__DOT__vgc_inst__DOT__artist_inst__DOT__gt_char__10__Vfuncout = 0;
    CData/*5:0*/ __Vfunc_top__DOT__vgc_inst__DOT__artist_inst__DOT__gt_char__10__idx;
    __Vfunc_top__DOT__vgc_inst__DOT__artist_inst__DOT__gt_char__10__idx = 0;
    CData/*7:0*/ __Vfunc_top__DOT__vgc_inst__DOT__artist_inst__DOT__gt_char__11__Vfuncout;
    __Vfunc_top__DOT__vgc_inst__DOT__artist_inst__DOT__gt_char__11__Vfuncout = 0;
    CData/*5:0*/ __Vfunc_top__DOT__vgc_inst__DOT__artist_inst__DOT__gt_char__11__idx;
    __Vfunc_top__DOT__vgc_inst__DOT__artist_inst__DOT__gt_char__11__idx = 0;
    CData/*7:0*/ __Vfunc_top__DOT__vgc_inst__DOT__artist_inst__DOT__gt_char__12__Vfuncout;
    __Vfunc_top__DOT__vgc_inst__DOT__artist_inst__DOT__gt_char__12__Vfuncout = 0;
    CData/*7:0*/ __Vfunc_top__DOT__vgc_inst__DOT__artist_inst__DOT__gt_char__13__Vfuncout;
    __Vfunc_top__DOT__vgc_inst__DOT__artist_inst__DOT__gt_char__13__Vfuncout = 0;
    CData/*3:0*/ __Vdly__top__DOT__blt_inst__DOT__state;
    __Vdly__top__DOT__blt_inst__DOT__state = 0;
    CData/*0:0*/ __Vdly__top__DOT__blt_inst__DOT__read_valid;
    __Vdly__top__DOT__blt_inst__DOT__read_valid = 0;
    IData/*23:0*/ __Vdly__top__DOT__blt_inst__DOT__wrote_count;
    __Vdly__top__DOT__blt_inst__DOT__wrote_count = 0;
    SData/*15:0*/ __Vdly__top__DOT__blt_inst__DOT__col;
    __Vdly__top__DOT__blt_inst__DOT__col = 0;
    SData/*15:0*/ __Vdly__top__DOT__blt_inst__DOT__row;
    __Vdly__top__DOT__blt_inst__DOT__row = 0;
    SData/*9:0*/ __Vdly__top__DOT__blt_inst__DOT__buf_idx;
    __Vdly__top__DOT__blt_inst__DOT__buf_idx = 0;
    CData/*6:0*/ __Vdly__top__DOT__vgc_inst__DOT__cursor_x;
    __Vdly__top__DOT__vgc_inst__DOT__cursor_x = 0;
    CData/*4:0*/ __Vdly__top__DOT__vgc_inst__DOT__cursor_y;
    __Vdly__top__DOT__vgc_inst__DOT__cursor_y = 0;
    CData/*3:0*/ __Vdly__top__DOT__vgc_inst__DOT__fg_color;
    __Vdly__top__DOT__vgc_inst__DOT__fg_color = 0;
    CData/*4:0*/ __Vdly__top__DOT__vgc_inst__DOT__scroll_offset;
    __Vdly__top__DOT__vgc_inst__DOT__scroll_offset = 0;
    CData/*0:0*/ __Vdly__top__DOT__vgc_inst__DOT__scroll_pending;
    __Vdly__top__DOT__vgc_inst__DOT__scroll_pending = 0;
    CData/*6:0*/ __Vdly__top__DOT__vgc_inst__DOT__scroll_col;
    __Vdly__top__DOT__vgc_inst__DOT__scroll_col = 0;
    CData/*0:0*/ __Vdly__top__DOT__vgc_inst__DOT__cmd_busy;
    __Vdly__top__DOT__vgc_inst__DOT__cmd_busy = 0;
    CData/*0:0*/ __Vdly__top__DOT__vgc_inst__DOT__copper_enabled;
    __Vdly__top__DOT__vgc_inst__DOT__copper_enabled = 0;
    SData/*8:0*/ __Vdly__top__DOT__vgc_inst__DOT__copper_count;
    __Vdly__top__DOT__vgc_inst__DOT__copper_count = 0;
    CData/*6:0*/ __Vdly__top__DOT__vgc_inst__DOT__copper_target_list;
    __Vdly__top__DOT__vgc_inst__DOT__copper_target_list = 0;
    CData/*6:0*/ __Vdly__top__DOT__vgc_inst__DOT__copper_pending_list;
    __Vdly__top__DOT__vgc_inst__DOT__copper_pending_list = 0;
    CData/*0:0*/ __Vdly__top__DOT__vgc_inst__DOT__copper_loading;
    __Vdly__top__DOT__vgc_inst__DOT__copper_loading = 0;
    CData/*2:0*/ __Vdly__top__DOT__vgc_inst__DOT__sprrow_count;
    __Vdly__top__DOT__vgc_inst__DOT__sprrow_count = 0;
    CData/*0:0*/ __Vdly__top__DOT__vgc_inst__DOT__sprcopy_phase;
    __Vdly__top__DOT__vgc_inst__DOT__sprcopy_phase = 0;
    CData/*0:0*/ __Vdly__top__DOT__vgc_inst__DOT__memread_pending;
    __Vdly__top__DOT__vgc_inst__DOT__memread_pending = 0;
    SData/*8:0*/ __Vdly__top__DOT__vgc_inst__DOT__cmd_cx;
    __Vdly__top__DOT__vgc_inst__DOT__cmd_cx = 0;
    CData/*7:0*/ __Vdly__top__DOT__vgc_inst__DOT__cmd_cy;
    __Vdly__top__DOT__vgc_inst__DOT__cmd_cy = 0;
    CData/*6:0*/ __Vdly__top__DOT__vgc_inst__DOT__copper_active_list;
    __Vdly__top__DOT__vgc_inst__DOT__copper_active_list = 0;
    CData/*7:0*/ __Vdly__top__DOT__vgc_inst__DOT__cmd_op;
    __Vdly__top__DOT__vgc_inst__DOT__cmd_op = 0;
    CData/*0:0*/ __Vdly__top__DOT__vgc_inst__DOT__cursor_blink;
    __Vdly__top__DOT__vgc_inst__DOT__cursor_blink = 0;
    CData/*4:0*/ __Vdly__top__DOT__vgc_inst__DOT__blink_count;
    __Vdly__top__DOT__vgc_inst__DOT__blink_count = 0;
    SData/*9:0*/ __Vdly__top__DOT__vgc_inst__DOT__h_count;
    __Vdly__top__DOT__vgc_inst__DOT__h_count = 0;
    SData/*9:0*/ __Vdly__top__DOT__vgc_inst__DOT__v_count;
    __Vdly__top__DOT__vgc_inst__DOT__v_count = 0;
    CData/*0:0*/ __Vdly__top__DOT__vgc_inst__DOT__artist_busy;
    __Vdly__top__DOT__vgc_inst__DOT__artist_busy = 0;
    CData/*7:0*/ __Vdly__top__DOT__vgc_inst__DOT__artist_inst__DOT__op;
    __Vdly__top__DOT__vgc_inst__DOT__artist_inst__DOT__op = 0;
    SData/*9:0*/ __Vdly__top__DOT__vgc_inst__DOT__artist_inst__DOT__fill_sp;
    __Vdly__top__DOT__vgc_inst__DOT__artist_inst__DOT__fill_sp = 0;
    CData/*2:0*/ __Vdly__top__DOT__vgc_inst__DOT__artist_inst__DOT__octant_count;
    __Vdly__top__DOT__vgc_inst__DOT__artist_inst__DOT__octant_count = 0;
    CData/*2:0*/ __Vdly__top__DOT__vgc_inst__DOT__artist_inst__DOT__paint_phase;
    __Vdly__top__DOT__vgc_inst__DOT__artist_inst__DOT__paint_phase = 0;
    CData/*1:0*/ __Vdly__top__DOT__vgc_inst__DOT__artist_inst__DOT__gt_font_pending;
    __Vdly__top__DOT__vgc_inst__DOT__artist_inst__DOT__gt_font_pending = 0;
    CData/*0:0*/ __Vdly__top__DOT__vgc_inst__DOT__artist_inst__DOT__paint_init;
    __Vdly__top__DOT__vgc_inst__DOT__artist_inst__DOT__paint_init = 0;
    CData/*7:0*/ __Vdly__top__DOT__vgc_inst__DOT__artist_inst__DOT__gt_font_byte;
    __Vdly__top__DOT__vgc_inst__DOT__artist_inst__DOT__gt_font_byte = 0;
    SData/*9:0*/ __Vdly__top__DOT__vgc_inst__DOT__artist_inst__DOT__err;
    __Vdly__top__DOT__vgc_inst__DOT__artist_inst__DOT__err = 0;
    SData/*9:0*/ __Vdly__top__DOT__vgc_inst__DOT__artist_inst__DOT__x;
    __Vdly__top__DOT__vgc_inst__DOT__artist_inst__DOT__x = 0;
    SData/*9:0*/ __Vdly__top__DOT__vgc_inst__DOT__artist_inst__DOT__y;
    __Vdly__top__DOT__vgc_inst__DOT__artist_inst__DOT__y = 0;
    SData/*8:0*/ __Vdly__top__DOT__vgc_inst__DOT__artist_inst__DOT__cx;
    __Vdly__top__DOT__vgc_inst__DOT__artist_inst__DOT__cx = 0;
    CData/*7:0*/ __Vdly__top__DOT__vgc_inst__DOT__artist_inst__DOT__cy;
    __Vdly__top__DOT__vgc_inst__DOT__artist_inst__DOT__cy = 0;
    SData/*9:0*/ __Vdly__top__DOT__vgc_inst__DOT__artist_inst__DOT__dx;
    __Vdly__top__DOT__vgc_inst__DOT__artist_inst__DOT__dx = 0;
    SData/*9:0*/ __Vdly__top__DOT__vgc_inst__DOT__artist_inst__DOT__dy;
    __Vdly__top__DOT__vgc_inst__DOT__artist_inst__DOT__dy = 0;
    CData/*3:0*/ __Vdly__top__DOT__vgc_inst__DOT__artist_inst__DOT__fill_target;
    __Vdly__top__DOT__vgc_inst__DOT__artist_inst__DOT__fill_target = 0;
    SData/*8:0*/ __Vdly__top__DOT__vgc_inst__DOT__artist_inst__DOT__fs_b_addr;
    __Vdly__top__DOT__vgc_inst__DOT__artist_inst__DOT__fs_b_addr = 0;
    CData/*2:0*/ __Vdly__top__DOT__vgc_inst__DOT__artist_inst__DOT__gt_font_row;
    __Vdly__top__DOT__vgc_inst__DOT__artist_inst__DOT__gt_font_row = 0;
    CData/*5:0*/ __Vdly__top__DOT__vgc_inst__DOT__artist_inst__DOT__gt_char_idx;
    __Vdly__top__DOT__vgc_inst__DOT__artist_inst__DOT__gt_char_idx = 0;
    CData/*2:0*/ __Vdly__top__DOT__vgc_inst__DOT__sprite_inst__DOT__spr_eval_state;
    __Vdly__top__DOT__vgc_inst__DOT__sprite_inst__DOT__spr_eval_state = 0;
    CData/*3:0*/ __Vdly__top__DOT__vgc_inst__DOT__sprite_inst__DOT__spr_eval_idx;
    __Vdly__top__DOT__vgc_inst__DOT__sprite_inst__DOT__spr_eval_idx = 0;
    SData/*8:0*/ __Vdly__top__DOT__vgc_inst__DOT__sprite_inst__DOT__spr_clear_x;
    __Vdly__top__DOT__vgc_inst__DOT__sprite_inst__DOT__spr_clear_x = 0;
    SData/*10:0*/ __Vdly__top__DOT__vgc_inst__DOT__sprite_inst__DOT__spr_b_addr;
    __Vdly__top__DOT__vgc_inst__DOT__sprite_inst__DOT__spr_b_addr = 0;
    SData/*8:0*/ __Vdly__top__DOT__vgc_inst__DOT__copper_index;
    __Vdly__top__DOT__vgc_inst__DOT__copper_index = 0;
    CData/*4:0*/ __Vdly__top__DOT__vgc_inst__DOT__tile_inst__DOT__dma_state;
    __Vdly__top__DOT__vgc_inst__DOT__tile_inst__DOT__dma_state = 0;
    CData/*0:0*/ __Vdly__top__DOT__vgc_inst__DOT__tile_inst__DOT__dma_data_valid;
    __Vdly__top__DOT__vgc_inst__DOT__tile_inst__DOT__dma_data_valid = 0;
    CData/*4:0*/ __Vdly__top__DOT__vgc_inst__DOT__tile_inst__DOT__dma_col_idx;
    __Vdly__top__DOT__vgc_inst__DOT__tile_inst__DOT__dma_col_idx = 0;
    SData/*15:0*/ __Vdly__top__DOT__vgc_inst__DOT__tile_inst__DOT__dma_count;
    __Vdly__top__DOT__vgc_inst__DOT__tile_inst__DOT__dma_count = 0;
    SData/*15:0*/ __Vdly__top__DOT__vgc_inst__DOT__tile_inst__DOT__dma_src;
    __Vdly__top__DOT__vgc_inst__DOT__tile_inst__DOT__dma_src = 0;
    SData/*15:0*/ __Vdly__top__DOT__vgc_inst__DOT__tile_inst__DOT__dma_dst;
    __Vdly__top__DOT__vgc_inst__DOT__tile_inst__DOT__dma_dst = 0;
    CData/*7:0*/ __Vdly__top__DOT__vgc_inst__DOT__tile_inst__DOT__dma_pal_phase;
    __Vdly__top__DOT__vgc_inst__DOT__tile_inst__DOT__dma_pal_phase = 0;
    CData/*7:0*/ __VdlyVal__top__DOT__xram__v0;
    __VdlyVal__top__DOT__xram__v0 = 0;
    IData/*18:0*/ __VdlyDim0__top__DOT__xram__v0;
    __VdlyDim0__top__DOT__xram__v0 = 0;
    CData/*0:0*/ __VdlySet__top__DOT__xram__v0;
    __VdlySet__top__DOT__xram__v0 = 0;
    CData/*7:0*/ __VdlyVal__top__DOT__xram__v1;
    __VdlyVal__top__DOT__xram__v1 = 0;
    IData/*18:0*/ __VdlyDim0__top__DOT__xram__v1;
    __VdlyDim0__top__DOT__xram__v1 = 0;
    CData/*0:0*/ __VdlySet__top__DOT__xram__v1;
    __VdlySet__top__DOT__xram__v1 = 0;
    CData/*7:0*/ __VdlyVal__top__DOT__xmc_regs__v0;
    __VdlyVal__top__DOT__xmc_regs__v0 = 0;
    CData/*5:0*/ __VdlyDim0__top__DOT__xmc_regs__v0;
    __VdlyDim0__top__DOT__xmc_regs__v0 = 0;
    CData/*0:0*/ __VdlySet__top__DOT__xmc_regs__v0;
    __VdlySet__top__DOT__xmc_regs__v0 = 0;
    CData/*7:0*/ __VdlyVal__top__DOT__ram__v0;
    __VdlyVal__top__DOT__ram__v0 = 0;
    SData/*15:0*/ __VdlyDim0__top__DOT__ram__v0;
    __VdlyDim0__top__DOT__ram__v0 = 0;
    CData/*0:0*/ __VdlySet__top__DOT__ram__v0;
    __VdlySet__top__DOT__ram__v0 = 0;
    CData/*7:0*/ __VdlyVal__top__DOT__ram__v1;
    __VdlyVal__top__DOT__ram__v1 = 0;
    SData/*15:0*/ __VdlyDim0__top__DOT__ram__v1;
    __VdlyDim0__top__DOT__ram__v1 = 0;
    CData/*0:0*/ __VdlySet__top__DOT__ram__v1;
    __VdlySet__top__DOT__ram__v1 = 0;
    CData/*7:0*/ __VdlyVal__top__DOT__ram__v2;
    __VdlyVal__top__DOT__ram__v2 = 0;
    CData/*0:0*/ __VdlySet__top__DOT__ram__v2;
    __VdlySet__top__DOT__ram__v2 = 0;
    CData/*0:0*/ __VdlySet__top__DOT__ram__v3;
    __VdlySet__top__DOT__ram__v3 = 0;
    CData/*7:0*/ __VdlyVal__top__DOT__ram__v4;
    __VdlyVal__top__DOT__ram__v4 = 0;
    SData/*15:0*/ __VdlyDim0__top__DOT__ram__v4;
    __VdlyDim0__top__DOT__ram__v4 = 0;
    CData/*0:0*/ __VdlySet__top__DOT__ram__v4;
    __VdlySet__top__DOT__ram__v4 = 0;
    CData/*0:0*/ __VdlySet__top__DOT__blt_inst__DOT__regs__v0;
    __VdlySet__top__DOT__blt_inst__DOT__regs__v0 = 0;
    CData/*0:0*/ __VdlySet__top__DOT__blt_inst__DOT__regs__v1;
    __VdlySet__top__DOT__blt_inst__DOT__regs__v1 = 0;
    CData/*0:0*/ __VdlySet__top__DOT__blt_inst__DOT__regs__v2;
    __VdlySet__top__DOT__blt_inst__DOT__regs__v2 = 0;
    CData/*0:0*/ __VdlySet__top__DOT__blt_inst__DOT__regs__v3;
    __VdlySet__top__DOT__blt_inst__DOT__regs__v3 = 0;
    CData/*7:0*/ __VdlyVal__top__DOT__blt_inst__DOT__regs__v4;
    __VdlyVal__top__DOT__blt_inst__DOT__regs__v4 = 0;
    CData/*7:0*/ __VdlyVal__top__DOT__blt_inst__DOT__regs__v5;
    __VdlyVal__top__DOT__blt_inst__DOT__regs__v5 = 0;
    CData/*7:0*/ __VdlyVal__top__DOT__blt_inst__DOT__regs__v6;
    __VdlyVal__top__DOT__blt_inst__DOT__regs__v6 = 0;
    CData/*7:0*/ __VdlyVal__top__DOT__blt_inst__DOT__row_buf__v0;
    __VdlyVal__top__DOT__blt_inst__DOT__row_buf__v0 = 0;
    SData/*9:0*/ __VdlyDim0__top__DOT__blt_inst__DOT__row_buf__v0;
    __VdlyDim0__top__DOT__blt_inst__DOT__row_buf__v0 = 0;
    CData/*0:0*/ __VdlySet__top__DOT__blt_inst__DOT__row_buf__v0;
    __VdlySet__top__DOT__blt_inst__DOT__row_buf__v0 = 0;
    CData/*0:0*/ __VdlySet__top__DOT__blt_inst__DOT__regs__v7;
    __VdlySet__top__DOT__blt_inst__DOT__regs__v7 = 0;
    CData/*0:0*/ __VdlySet__top__DOT__blt_inst__DOT__regs__v8;
    __VdlySet__top__DOT__blt_inst__DOT__regs__v8 = 0;
    CData/*7:0*/ __VdlyVal__top__DOT__blt_inst__DOT__regs__v9;
    __VdlyVal__top__DOT__blt_inst__DOT__regs__v9 = 0;
    CData/*7:0*/ __VdlyVal__top__DOT__blt_inst__DOT__regs__v10;
    __VdlyVal__top__DOT__blt_inst__DOT__regs__v10 = 0;
    CData/*7:0*/ __VdlyVal__top__DOT__blt_inst__DOT__regs__v11;
    __VdlyVal__top__DOT__blt_inst__DOT__regs__v11 = 0;
    CData/*0:0*/ __VdlySet__top__DOT__blt_inst__DOT__regs__v12;
    __VdlySet__top__DOT__blt_inst__DOT__regs__v12 = 0;
    CData/*0:0*/ __VdlySet__top__DOT__blt_inst__DOT__regs__v13;
    __VdlySet__top__DOT__blt_inst__DOT__regs__v13 = 0;
    CData/*0:0*/ __VdlySet__top__DOT__blt_inst__DOT__regs__v14;
    __VdlySet__top__DOT__blt_inst__DOT__regs__v14 = 0;
    CData/*0:0*/ __VdlySet__top__DOT__blt_inst__DOT__regs__v15;
    __VdlySet__top__DOT__blt_inst__DOT__regs__v15 = 0;
    CData/*0:0*/ __VdlySet__top__DOT__blt_inst__DOT__regs__v16;
    __VdlySet__top__DOT__blt_inst__DOT__regs__v16 = 0;
    CData/*0:0*/ __VdlySet__top__DOT__blt_inst__DOT__regs__v17;
    __VdlySet__top__DOT__blt_inst__DOT__regs__v17 = 0;
    CData/*0:0*/ __VdlySet__top__DOT__blt_inst__DOT__regs__v18;
    __VdlySet__top__DOT__blt_inst__DOT__regs__v18 = 0;
    CData/*0:0*/ __VdlySet__top__DOT__blt_inst__DOT__regs__v19;
    __VdlySet__top__DOT__blt_inst__DOT__regs__v19 = 0;
    CData/*0:0*/ __VdlySet__top__DOT__blt_inst__DOT__regs__v20;
    __VdlySet__top__DOT__blt_inst__DOT__regs__v20 = 0;
    CData/*0:0*/ __VdlySet__top__DOT__blt_inst__DOT__regs__v21;
    __VdlySet__top__DOT__blt_inst__DOT__regs__v21 = 0;
    CData/*0:0*/ __VdlySet__top__DOT__blt_inst__DOT__regs__v22;
    __VdlySet__top__DOT__blt_inst__DOT__regs__v22 = 0;
    CData/*0:0*/ __VdlySet__top__DOT__blt_inst__DOT__regs__v23;
    __VdlySet__top__DOT__blt_inst__DOT__regs__v23 = 0;
    CData/*7:0*/ __VdlyVal__top__DOT__blt_inst__DOT__regs__v24;
    __VdlyVal__top__DOT__blt_inst__DOT__regs__v24 = 0;
    CData/*4:0*/ __VdlyDim0__top__DOT__blt_inst__DOT__regs__v24;
    __VdlyDim0__top__DOT__blt_inst__DOT__regs__v24 = 0;
    CData/*0:0*/ __VdlySet__top__DOT__blt_inst__DOT__regs__v24;
    __VdlySet__top__DOT__blt_inst__DOT__regs__v24 = 0;
    CData/*0:0*/ __VdlySet__top__DOT__blt_inst__DOT__regs__v25;
    __VdlySet__top__DOT__blt_inst__DOT__regs__v25 = 0;
    CData/*0:0*/ __VdlySet__top__DOT__blt_inst__DOT__regs__v30;
    __VdlySet__top__DOT__blt_inst__DOT__regs__v30 = 0;
    CData/*0:0*/ __VdlySet__top__DOT__blt_inst__DOT__regs__v32;
    __VdlySet__top__DOT__blt_inst__DOT__regs__v32 = 0;
    CData/*7:0*/ __VdlyVal__top__DOT__cpu_inst__DOT__AXYS__v0;
    __VdlyVal__top__DOT__cpu_inst__DOT__AXYS__v0 = 0;
    CData/*1:0*/ __VdlyDim0__top__DOT__cpu_inst__DOT__AXYS__v0;
    __VdlyDim0__top__DOT__cpu_inst__DOT__AXYS__v0 = 0;
    CData/*0:0*/ __VdlySet__top__DOT__cpu_inst__DOT__AXYS__v0;
    __VdlySet__top__DOT__cpu_inst__DOT__AXYS__v0 = 0;
    CData/*0:0*/ __VdlySet__top__DOT__vgc_inst__DOT__spr_shape__v0;
    __VdlySet__top__DOT__vgc_inst__DOT__spr_shape__v0 = 0;
    CData/*3:0*/ __VdlyVal__top__DOT__vgc_inst__DOT__spr_shape__v16;
    __VdlyVal__top__DOT__vgc_inst__DOT__spr_shape__v16 = 0;
    CData/*3:0*/ __VdlyDim0__top__DOT__vgc_inst__DOT__spr_shape__v16;
    __VdlyDim0__top__DOT__vgc_inst__DOT__spr_shape__v16 = 0;
    CData/*0:0*/ __VdlySet__top__DOT__vgc_inst__DOT__spr_shape__v16;
    __VdlySet__top__DOT__vgc_inst__DOT__spr_shape__v16 = 0;
    CData/*0:0*/ __VdlySet__top__DOT__vgc_inst__DOT__spr_trans__v0;
    __VdlySet__top__DOT__vgc_inst__DOT__spr_trans__v0 = 0;
    CData/*3:0*/ __VdlyVal__top__DOT__vgc_inst__DOT__spr_trans__v16;
    __VdlyVal__top__DOT__vgc_inst__DOT__spr_trans__v16 = 0;
    CData/*3:0*/ __VdlyDim0__top__DOT__vgc_inst__DOT__spr_trans__v16;
    __VdlyDim0__top__DOT__vgc_inst__DOT__spr_trans__v16 = 0;
    CData/*0:0*/ __VdlySet__top__DOT__vgc_inst__DOT__spr_trans__v16;
    __VdlySet__top__DOT__vgc_inst__DOT__spr_trans__v16 = 0;
    CData/*0:0*/ __VdlySet__top__DOT__vgc_inst__DOT__regs__v0;
    __VdlySet__top__DOT__vgc_inst__DOT__regs__v0 = 0;
    CData/*7:0*/ __VdlyVal__top__DOT__vgc_inst__DOT__regs__v32;
    __VdlyVal__top__DOT__vgc_inst__DOT__regs__v32 = 0;
    CData/*0:0*/ __VdlySet__top__DOT__vgc_inst__DOT__regs__v32;
    __VdlySet__top__DOT__vgc_inst__DOT__regs__v32 = 0;
    CData/*7:0*/ __VdlyVal__top__DOT__vgc_inst__DOT__regs__v33;
    __VdlyVal__top__DOT__vgc_inst__DOT__regs__v33 = 0;
    CData/*0:0*/ __VdlySet__top__DOT__vgc_inst__DOT__regs__v33;
    __VdlySet__top__DOT__vgc_inst__DOT__regs__v33 = 0;
    IData/*16:0*/ __VdlyVal__top__DOT__vgc_inst__DOT__copper_pos__v0;
    __VdlyVal__top__DOT__vgc_inst__DOT__copper_pos__v0 = 0;
    CData/*4:0*/ __VdlyDim0__top__DOT__vgc_inst__DOT__copper_pos__v0;
    __VdlyDim0__top__DOT__vgc_inst__DOT__copper_pos__v0 = 0;
    CData/*0:0*/ __VdlySet__top__DOT__vgc_inst__DOT__copper_pos__v0;
    __VdlySet__top__DOT__vgc_inst__DOT__copper_pos__v0 = 0;
    CData/*7:0*/ __VdlyVal__top__DOT__vgc_inst__DOT__copper_reg__v0;
    __VdlyVal__top__DOT__vgc_inst__DOT__copper_reg__v0 = 0;
    CData/*4:0*/ __VdlyDim0__top__DOT__vgc_inst__DOT__copper_reg__v0;
    __VdlyDim0__top__DOT__vgc_inst__DOT__copper_reg__v0 = 0;
    CData/*7:0*/ __VdlyVal__top__DOT__vgc_inst__DOT__copper_val__v0;
    __VdlyVal__top__DOT__vgc_inst__DOT__copper_val__v0 = 0;
    CData/*4:0*/ __VdlyDim0__top__DOT__vgc_inst__DOT__copper_val__v0;
    __VdlyDim0__top__DOT__vgc_inst__DOT__copper_val__v0 = 0;
    CData/*7:0*/ __VdlyVal__top__DOT__vgc_inst__DOT__regs__v34;
    __VdlyVal__top__DOT__vgc_inst__DOT__regs__v34 = 0;
    CData/*4:0*/ __VdlyDim0__top__DOT__vgc_inst__DOT__regs__v34;
    __VdlyDim0__top__DOT__vgc_inst__DOT__regs__v34 = 0;
    CData/*0:0*/ __VdlySet__top__DOT__vgc_inst__DOT__regs__v34;
    __VdlySet__top__DOT__vgc_inst__DOT__regs__v34 = 0;
    CData/*6:0*/ __VdlyDim0__top__DOT__vgc_inst__DOT__copper_list_count__v0;
    __VdlyDim0__top__DOT__vgc_inst__DOT__copper_list_count__v0 = 0;
    CData/*6:0*/ __VdlyDim0__top__DOT__vgc_inst__DOT__copper_list_count__v1;
    __VdlyDim0__top__DOT__vgc_inst__DOT__copper_list_count__v1 = 0;
    IData/*16:0*/ __VdlyVal__top__DOT__vgc_inst__DOT__copper_list_pos__v0;
    __VdlyVal__top__DOT__vgc_inst__DOT__copper_list_pos__v0 = 0;
    SData/*11:0*/ __VdlyDim0__top__DOT__vgc_inst__DOT__copper_list_pos__v0;
    __VdlyDim0__top__DOT__vgc_inst__DOT__copper_list_pos__v0 = 0;
    CData/*0:0*/ __VdlySet__top__DOT__vgc_inst__DOT__copper_list_pos__v0;
    __VdlySet__top__DOT__vgc_inst__DOT__copper_list_pos__v0 = 0;
    CData/*7:0*/ __VdlyVal__top__DOT__vgc_inst__DOT__copper_list_reg__v0;
    __VdlyVal__top__DOT__vgc_inst__DOT__copper_list_reg__v0 = 0;
    SData/*11:0*/ __VdlyDim0__top__DOT__vgc_inst__DOT__copper_list_reg__v0;
    __VdlyDim0__top__DOT__vgc_inst__DOT__copper_list_reg__v0 = 0;
    CData/*7:0*/ __VdlyVal__top__DOT__vgc_inst__DOT__copper_list_val__v0;
    __VdlyVal__top__DOT__vgc_inst__DOT__copper_list_val__v0 = 0;
    SData/*11:0*/ __VdlyDim0__top__DOT__vgc_inst__DOT__copper_list_val__v0;
    __VdlyDim0__top__DOT__vgc_inst__DOT__copper_list_val__v0 = 0;
    SData/*8:0*/ __VdlyVal__top__DOT__vgc_inst__DOT__copper_list_count__v2;
    __VdlyVal__top__DOT__vgc_inst__DOT__copper_list_count__v2 = 0;
    CData/*6:0*/ __VdlyDim0__top__DOT__vgc_inst__DOT__copper_list_count__v2;
    __VdlyDim0__top__DOT__vgc_inst__DOT__copper_list_count__v2 = 0;
    IData/*16:0*/ __VdlyVal__top__DOT__vgc_inst__DOT__copper_pos__v1;
    __VdlyVal__top__DOT__vgc_inst__DOT__copper_pos__v1 = 0;
    CData/*4:0*/ __VdlyDim0__top__DOT__vgc_inst__DOT__copper_pos__v1;
    __VdlyDim0__top__DOT__vgc_inst__DOT__copper_pos__v1 = 0;
    CData/*0:0*/ __VdlySet__top__DOT__vgc_inst__DOT__copper_pos__v1;
    __VdlySet__top__DOT__vgc_inst__DOT__copper_pos__v1 = 0;
    CData/*7:0*/ __VdlyVal__top__DOT__vgc_inst__DOT__copper_reg__v1;
    __VdlyVal__top__DOT__vgc_inst__DOT__copper_reg__v1 = 0;
    CData/*4:0*/ __VdlyDim0__top__DOT__vgc_inst__DOT__copper_reg__v1;
    __VdlyDim0__top__DOT__vgc_inst__DOT__copper_reg__v1 = 0;
    CData/*7:0*/ __VdlyVal__top__DOT__vgc_inst__DOT__copper_val__v1;
    __VdlyVal__top__DOT__vgc_inst__DOT__copper_val__v1 = 0;
    CData/*4:0*/ __VdlyDim0__top__DOT__vgc_inst__DOT__copper_val__v1;
    __VdlyDim0__top__DOT__vgc_inst__DOT__copper_val__v1 = 0;
    CData/*1:0*/ __VdlyVal__top__DOT__vgc_inst__DOT__spr_pri__v16;
    __VdlyVal__top__DOT__vgc_inst__DOT__spr_pri__v16 = 0;
    CData/*3:0*/ __VdlyDim0__top__DOT__vgc_inst__DOT__spr_pri__v16;
    __VdlyDim0__top__DOT__vgc_inst__DOT__spr_pri__v16 = 0;
    CData/*0:0*/ __VdlySet__top__DOT__vgc_inst__DOT__spr_pri__v16;
    __VdlySet__top__DOT__vgc_inst__DOT__spr_pri__v16 = 0;
    CData/*0:0*/ __VdlyVal__top__DOT__vgc_inst__DOT__spr_flip_h__v16;
    __VdlyVal__top__DOT__vgc_inst__DOT__spr_flip_h__v16 = 0;
    CData/*3:0*/ __VdlyDim0__top__DOT__vgc_inst__DOT__spr_flip_h__v16;
    __VdlyDim0__top__DOT__vgc_inst__DOT__spr_flip_h__v16 = 0;
    CData/*0:0*/ __VdlySet__top__DOT__vgc_inst__DOT__spr_flip_h__v16;
    __VdlySet__top__DOT__vgc_inst__DOT__spr_flip_h__v16 = 0;
    CData/*0:0*/ __VdlyVal__top__DOT__vgc_inst__DOT__spr_flip_v__v16;
    __VdlyVal__top__DOT__vgc_inst__DOT__spr_flip_v__v16 = 0;
    CData/*3:0*/ __VdlyDim0__top__DOT__vgc_inst__DOT__spr_flip_v__v16;
    __VdlyDim0__top__DOT__vgc_inst__DOT__spr_flip_v__v16 = 0;
    CData/*3:0*/ __VdlyDim0__top__DOT__vgc_inst__DOT__spr_enable__v16;
    __VdlyDim0__top__DOT__vgc_inst__DOT__spr_enable__v16 = 0;
    CData/*0:0*/ __VdlySet__top__DOT__vgc_inst__DOT__spr_enable__v16;
    __VdlySet__top__DOT__vgc_inst__DOT__spr_enable__v16 = 0;
    CData/*3:0*/ __VdlyDim0__top__DOT__vgc_inst__DOT__spr_enable__v17;
    __VdlyDim0__top__DOT__vgc_inst__DOT__spr_enable__v17 = 0;
    CData/*0:0*/ __VdlySet__top__DOT__vgc_inst__DOT__spr_enable__v17;
    __VdlySet__top__DOT__vgc_inst__DOT__spr_enable__v17 = 0;
    SData/*8:0*/ __VdlyVal__top__DOT__vgc_inst__DOT__spr_x__v16;
    __VdlyVal__top__DOT__vgc_inst__DOT__spr_x__v16 = 0;
    CData/*3:0*/ __VdlyDim0__top__DOT__vgc_inst__DOT__spr_x__v16;
    __VdlyDim0__top__DOT__vgc_inst__DOT__spr_x__v16 = 0;
    CData/*0:0*/ __VdlySet__top__DOT__vgc_inst__DOT__spr_x__v16;
    __VdlySet__top__DOT__vgc_inst__DOT__spr_x__v16 = 0;
    CData/*7:0*/ __VdlyVal__top__DOT__vgc_inst__DOT__spr_y__v16;
    __VdlyVal__top__DOT__vgc_inst__DOT__spr_y__v16 = 0;
    CData/*3:0*/ __VdlyDim0__top__DOT__vgc_inst__DOT__spr_y__v16;
    __VdlyDim0__top__DOT__vgc_inst__DOT__spr_y__v16 = 0;
    CData/*7:0*/ __VdlyVal__top__DOT__vgc_inst__DOT__sprrow_data__v0;
    __VdlyVal__top__DOT__vgc_inst__DOT__sprrow_data__v0 = 0;
    CData/*0:0*/ __VdlySet__top__DOT__vgc_inst__DOT__sprrow_data__v0;
    __VdlySet__top__DOT__vgc_inst__DOT__sprrow_data__v0 = 0;
    CData/*7:0*/ __VdlyVal__top__DOT__vgc_inst__DOT__sprrow_data__v1;
    __VdlyVal__top__DOT__vgc_inst__DOT__sprrow_data__v1 = 0;
    CData/*7:0*/ __VdlyVal__top__DOT__vgc_inst__DOT__sprrow_data__v2;
    __VdlyVal__top__DOT__vgc_inst__DOT__sprrow_data__v2 = 0;
    CData/*7:0*/ __VdlyVal__top__DOT__vgc_inst__DOT__sprrow_data__v3;
    __VdlyVal__top__DOT__vgc_inst__DOT__sprrow_data__v3 = 0;
    CData/*7:0*/ __VdlyVal__top__DOT__vgc_inst__DOT__sprrow_data__v4;
    __VdlyVal__top__DOT__vgc_inst__DOT__sprrow_data__v4 = 0;
    CData/*7:0*/ __VdlyVal__top__DOT__vgc_inst__DOT__sprrow_data__v5;
    __VdlyVal__top__DOT__vgc_inst__DOT__sprrow_data__v5 = 0;
    CData/*7:0*/ __VdlyVal__top__DOT__vgc_inst__DOT__sprrow_data__v6;
    __VdlyVal__top__DOT__vgc_inst__DOT__sprrow_data__v6 = 0;
    CData/*7:0*/ __VdlyVal__top__DOT__vgc_inst__DOT__sprrow_data__v7;
    __VdlyVal__top__DOT__vgc_inst__DOT__sprrow_data__v7 = 0;
    CData/*7:0*/ __VdlyVal__top__DOT__vgc_inst__DOT__regs__v35;
    __VdlyVal__top__DOT__vgc_inst__DOT__regs__v35 = 0;
    CData/*4:0*/ __VdlyDim0__top__DOT__vgc_inst__DOT__regs__v35;
    __VdlyDim0__top__DOT__vgc_inst__DOT__regs__v35 = 0;
    CData/*0:0*/ __VdlySet__top__DOT__vgc_inst__DOT__regs__v35;
    __VdlySet__top__DOT__vgc_inst__DOT__regs__v35 = 0;
    CData/*1:0*/ __VdlyVal__top__DOT__vgc_inst__DOT__spr_pri__v17;
    __VdlyVal__top__DOT__vgc_inst__DOT__spr_pri__v17 = 0;
    CData/*3:0*/ __VdlyDim0__top__DOT__vgc_inst__DOT__spr_pri__v17;
    __VdlyDim0__top__DOT__vgc_inst__DOT__spr_pri__v17 = 0;
    CData/*0:0*/ __VdlySet__top__DOT__vgc_inst__DOT__spr_pri__v17;
    __VdlySet__top__DOT__vgc_inst__DOT__spr_pri__v17 = 0;
    CData/*0:0*/ __VdlyVal__top__DOT__vgc_inst__DOT__spr_flip_h__v17;
    __VdlyVal__top__DOT__vgc_inst__DOT__spr_flip_h__v17 = 0;
    CData/*3:0*/ __VdlyDim0__top__DOT__vgc_inst__DOT__spr_flip_h__v17;
    __VdlyDim0__top__DOT__vgc_inst__DOT__spr_flip_h__v17 = 0;
    CData/*0:0*/ __VdlySet__top__DOT__vgc_inst__DOT__spr_flip_h__v17;
    __VdlySet__top__DOT__vgc_inst__DOT__spr_flip_h__v17 = 0;
    CData/*0:0*/ __VdlyVal__top__DOT__vgc_inst__DOT__spr_flip_v__v17;
    __VdlyVal__top__DOT__vgc_inst__DOT__spr_flip_v__v17 = 0;
    CData/*3:0*/ __VdlyDim0__top__DOT__vgc_inst__DOT__spr_flip_v__v17;
    __VdlyDim0__top__DOT__vgc_inst__DOT__spr_flip_v__v17 = 0;
    CData/*0:0*/ __VdlyVal__top__DOT__vgc_inst__DOT__spr_enable__v18;
    __VdlyVal__top__DOT__vgc_inst__DOT__spr_enable__v18 = 0;
    CData/*3:0*/ __VdlyDim0__top__DOT__vgc_inst__DOT__spr_enable__v18;
    __VdlyDim0__top__DOT__vgc_inst__DOT__spr_enable__v18 = 0;
    CData/*7:0*/ __VdlyVal__top__DOT__vgc_inst__DOT__spr_y__v17;
    __VdlyVal__top__DOT__vgc_inst__DOT__spr_y__v17 = 0;
    CData/*3:0*/ __VdlyDim0__top__DOT__vgc_inst__DOT__spr_y__v17;
    __VdlyDim0__top__DOT__vgc_inst__DOT__spr_y__v17 = 0;
    CData/*0:0*/ __VdlySet__top__DOT__vgc_inst__DOT__spr_y__v17;
    __VdlySet__top__DOT__vgc_inst__DOT__spr_y__v17 = 0;
    CData/*0:0*/ __VdlyVal__top__DOT__vgc_inst__DOT__spr_x__v17;
    __VdlyVal__top__DOT__vgc_inst__DOT__spr_x__v17 = 0;
    CData/*3:0*/ __VdlyDim0__top__DOT__vgc_inst__DOT__spr_x__v17;
    __VdlyDim0__top__DOT__vgc_inst__DOT__spr_x__v17 = 0;
    CData/*0:0*/ __VdlySet__top__DOT__vgc_inst__DOT__spr_x__v17;
    __VdlySet__top__DOT__vgc_inst__DOT__spr_x__v17 = 0;
    CData/*7:0*/ __VdlyVal__top__DOT__vgc_inst__DOT__spr_x__v18;
    __VdlyVal__top__DOT__vgc_inst__DOT__spr_x__v18 = 0;
    CData/*3:0*/ __VdlyDim0__top__DOT__vgc_inst__DOT__spr_x__v18;
    __VdlyDim0__top__DOT__vgc_inst__DOT__spr_x__v18 = 0;
    CData/*0:0*/ __VdlySet__top__DOT__vgc_inst__DOT__spr_x__v18;
    __VdlySet__top__DOT__vgc_inst__DOT__spr_x__v18 = 0;
    CData/*7:0*/ __VdlyVal__top__DOT__vgc_inst__DOT__fio_name__v0;
    __VdlyVal__top__DOT__vgc_inst__DOT__fio_name__v0 = 0;
    CData/*5:0*/ __VdlyDim0__top__DOT__vgc_inst__DOT__fio_name__v0;
    __VdlyDim0__top__DOT__vgc_inst__DOT__fio_name__v0 = 0;
    CData/*0:0*/ __VdlySet__top__DOT__vgc_inst__DOT__fio_name__v0;
    __VdlySet__top__DOT__vgc_inst__DOT__fio_name__v0 = 0;
    CData/*7:0*/ __VdlyVal__top__DOT__vgc_inst__DOT__text_inst__DOT__char_mem__DOT__mem__v0;
    __VdlyVal__top__DOT__vgc_inst__DOT__text_inst__DOT__char_mem__DOT__mem__v0 = 0;
    SData/*10:0*/ __VdlyDim0__top__DOT__vgc_inst__DOT__text_inst__DOT__char_mem__DOT__mem__v0;
    __VdlyDim0__top__DOT__vgc_inst__DOT__text_inst__DOT__char_mem__DOT__mem__v0 = 0;
    CData/*0:0*/ __VdlySet__top__DOT__vgc_inst__DOT__text_inst__DOT__char_mem__DOT__mem__v0;
    __VdlySet__top__DOT__vgc_inst__DOT__text_inst__DOT__char_mem__DOT__mem__v0 = 0;
    CData/*7:0*/ __VdlyVal__top__DOT__vgc_inst__DOT__text_inst__DOT__color_mem__DOT__mem__v0;
    __VdlyVal__top__DOT__vgc_inst__DOT__text_inst__DOT__color_mem__DOT__mem__v0 = 0;
    SData/*10:0*/ __VdlyDim0__top__DOT__vgc_inst__DOT__text_inst__DOT__color_mem__DOT__mem__v0;
    __VdlyDim0__top__DOT__vgc_inst__DOT__text_inst__DOT__color_mem__DOT__mem__v0 = 0;
    CData/*0:0*/ __VdlySet__top__DOT__vgc_inst__DOT__text_inst__DOT__color_mem__DOT__mem__v0;
    __VdlySet__top__DOT__vgc_inst__DOT__text_inst__DOT__color_mem__DOT__mem__v0 = 0;
    CData/*3:0*/ __VdlyVal__top__DOT__vgc_inst__DOT__gfx_inst__DOT__gfx_mem__DOT__mem__v0;
    __VdlyVal__top__DOT__vgc_inst__DOT__gfx_inst__DOT__gfx_mem__DOT__mem__v0 = 0;
    SData/*15:0*/ __VdlyDim0__top__DOT__vgc_inst__DOT__gfx_inst__DOT__gfx_mem__DOT__mem__v0;
    __VdlyDim0__top__DOT__vgc_inst__DOT__gfx_inst__DOT__gfx_mem__DOT__mem__v0 = 0;
    CData/*0:0*/ __VdlySet__top__DOT__vgc_inst__DOT__gfx_inst__DOT__gfx_mem__DOT__mem__v0;
    __VdlySet__top__DOT__vgc_inst__DOT__gfx_inst__DOT__gfx_mem__DOT__mem__v0 = 0;
    IData/*16:0*/ __VdlyVal__top__DOT__vgc_inst__DOT__artist_inst__DOT__push_neighbors__v0;
    __VdlyVal__top__DOT__vgc_inst__DOT__artist_inst__DOT__push_neighbors__v0 = 0;
    CData/*0:0*/ __VdlySet__top__DOT__vgc_inst__DOT__artist_inst__DOT__push_neighbors__v0;
    __VdlySet__top__DOT__vgc_inst__DOT__artist_inst__DOT__push_neighbors__v0 = 0;
    IData/*16:0*/ __VdlyVal__top__DOT__vgc_inst__DOT__artist_inst__DOT__push_neighbors__v1;
    __VdlyVal__top__DOT__vgc_inst__DOT__artist_inst__DOT__push_neighbors__v1 = 0;
    IData/*16:0*/ __VdlyVal__top__DOT__vgc_inst__DOT__artist_inst__DOT__push_neighbors__v2;
    __VdlyVal__top__DOT__vgc_inst__DOT__artist_inst__DOT__push_neighbors__v2 = 0;
    IData/*16:0*/ __VdlyVal__top__DOT__vgc_inst__DOT__artist_inst__DOT__push_neighbors__v3;
    __VdlyVal__top__DOT__vgc_inst__DOT__artist_inst__DOT__push_neighbors__v3 = 0;
    IData/*16:0*/ __VdlyVal__top__DOT__vgc_inst__DOT__artist_inst__DOT__fill_stack_mem__DOT__mem__v0;
    __VdlyVal__top__DOT__vgc_inst__DOT__artist_inst__DOT__fill_stack_mem__DOT__mem__v0 = 0;
    SData/*8:0*/ __VdlyDim0__top__DOT__vgc_inst__DOT__artist_inst__DOT__fill_stack_mem__DOT__mem__v0;
    __VdlyDim0__top__DOT__vgc_inst__DOT__artist_inst__DOT__fill_stack_mem__DOT__mem__v0 = 0;
    CData/*0:0*/ __VdlySet__top__DOT__vgc_inst__DOT__artist_inst__DOT__fill_stack_mem__DOT__mem__v0;
    __VdlySet__top__DOT__vgc_inst__DOT__artist_inst__DOT__fill_stack_mem__DOT__mem__v0 = 0;
    CData/*7:0*/ __VdlyVal__top__DOT__vgc_inst__DOT__sprite_inst__DOT__spr_row_data__v0;
    __VdlyVal__top__DOT__vgc_inst__DOT__sprite_inst__DOT__spr_row_data__v0 = 0;
    CData/*0:0*/ __VdlySet__top__DOT__vgc_inst__DOT__sprite_inst__DOT__spr_row_data__v0;
    __VdlySet__top__DOT__vgc_inst__DOT__sprite_inst__DOT__spr_row_data__v0 = 0;
    CData/*7:0*/ __VdlyVal__top__DOT__vgc_inst__DOT__sprite_inst__DOT__spr_row_data__v1;
    __VdlyVal__top__DOT__vgc_inst__DOT__sprite_inst__DOT__spr_row_data__v1 = 0;
    CData/*2:0*/ __VdlyDim0__top__DOT__vgc_inst__DOT__sprite_inst__DOT__spr_row_data__v1;
    __VdlyDim0__top__DOT__vgc_inst__DOT__sprite_inst__DOT__spr_row_data__v1 = 0;
    CData/*0:0*/ __VdlySet__top__DOT__vgc_inst__DOT__sprite_inst__DOT__spr_row_data__v1;
    __VdlySet__top__DOT__vgc_inst__DOT__sprite_inst__DOT__spr_row_data__v1 = 0;
    CData/*7:0*/ __VdlyVal__top__DOT__vgc_inst__DOT__sprite_inst__DOT__spr_mem__DOT__mem__v0;
    __VdlyVal__top__DOT__vgc_inst__DOT__sprite_inst__DOT__spr_mem__DOT__mem__v0 = 0;
    SData/*10:0*/ __VdlyDim0__top__DOT__vgc_inst__DOT__sprite_inst__DOT__spr_mem__DOT__mem__v0;
    __VdlyDim0__top__DOT__vgc_inst__DOT__sprite_inst__DOT__spr_mem__DOT__mem__v0 = 0;
    CData/*0:0*/ __VdlySet__top__DOT__vgc_inst__DOT__sprite_inst__DOT__spr_mem__DOT__mem__v0;
    __VdlySet__top__DOT__vgc_inst__DOT__sprite_inst__DOT__spr_mem__DOT__mem__v0 = 0;
    CData/*6:0*/ __VdlyVal__top__DOT__vgc_inst__DOT__sprite_inst__DOT__scanline_buf__DOT__mem__v0;
    __VdlyVal__top__DOT__vgc_inst__DOT__sprite_inst__DOT__scanline_buf__DOT__mem__v0 = 0;
    SData/*8:0*/ __VdlyDim0__top__DOT__vgc_inst__DOT__sprite_inst__DOT__scanline_buf__DOT__mem__v0;
    __VdlyDim0__top__DOT__vgc_inst__DOT__sprite_inst__DOT__scanline_buf__DOT__mem__v0 = 0;
    CData/*0:0*/ __VdlySet__top__DOT__vgc_inst__DOT__sprite_inst__DOT__scanline_buf__DOT__mem__v0;
    __VdlySet__top__DOT__vgc_inst__DOT__sprite_inst__DOT__scanline_buf__DOT__mem__v0 = 0;
    CData/*7:0*/ __VdlyVal__top__DOT__vgc_inst__DOT__tile_inst__DOT__tregs__v0;
    __VdlyVal__top__DOT__vgc_inst__DOT__tile_inst__DOT__tregs__v0 = 0;
    CData/*4:0*/ __VdlyDim0__top__DOT__vgc_inst__DOT__tile_inst__DOT__tregs__v0;
    __VdlyDim0__top__DOT__vgc_inst__DOT__tile_inst__DOT__tregs__v0 = 0;
    CData/*0:0*/ __VdlySet__top__DOT__vgc_inst__DOT__tile_inst__DOT__tregs__v0;
    __VdlySet__top__DOT__vgc_inst__DOT__tile_inst__DOT__tregs__v0 = 0;
    CData/*7:0*/ __VdlyVal__top__DOT__vgc_inst__DOT__tile_inst__DOT__col_buffer__v0;
    __VdlyVal__top__DOT__vgc_inst__DOT__tile_inst__DOT__col_buffer__v0 = 0;
    CData/*0:0*/ __VdlySet__top__DOT__vgc_inst__DOT__tile_inst__DOT__col_buffer__v0;
    __VdlySet__top__DOT__vgc_inst__DOT__tile_inst__DOT__col_buffer__v0 = 0;
    CData/*7:0*/ __VdlyVal__top__DOT__vgc_inst__DOT__tile_inst__DOT__col_buffer__v1;
    __VdlyVal__top__DOT__vgc_inst__DOT__tile_inst__DOT__col_buffer__v1 = 0;
    CData/*0:0*/ __VdlySet__top__DOT__vgc_inst__DOT__tile_inst__DOT__col_buffer__v1;
    __VdlySet__top__DOT__vgc_inst__DOT__tile_inst__DOT__col_buffer__v1 = 0;
    CData/*7:0*/ __VdlyVal__top__DOT__vgc_inst__DOT__tile_inst__DOT__col_buffer__v2;
    __VdlyVal__top__DOT__vgc_inst__DOT__tile_inst__DOT__col_buffer__v2 = 0;
    CData/*0:0*/ __VdlySet__top__DOT__vgc_inst__DOT__tile_inst__DOT__col_buffer__v2;
    __VdlySet__top__DOT__vgc_inst__DOT__tile_inst__DOT__col_buffer__v2 = 0;
    CData/*7:0*/ __VdlyVal__top__DOT__vgc_inst__DOT__tile_inst__DOT__col_buffer__v3;
    __VdlyVal__top__DOT__vgc_inst__DOT__tile_inst__DOT__col_buffer__v3 = 0;
    CData/*0:0*/ __VdlySet__top__DOT__vgc_inst__DOT__tile_inst__DOT__col_buffer__v3;
    __VdlySet__top__DOT__vgc_inst__DOT__tile_inst__DOT__col_buffer__v3 = 0;
    CData/*7:0*/ __VdlyVal__top__DOT__vgc_inst__DOT__tile_inst__DOT__col_buffer__v4;
    __VdlyVal__top__DOT__vgc_inst__DOT__tile_inst__DOT__col_buffer__v4 = 0;
    CData/*0:0*/ __VdlySet__top__DOT__vgc_inst__DOT__tile_inst__DOT__col_buffer__v4;
    __VdlySet__top__DOT__vgc_inst__DOT__tile_inst__DOT__col_buffer__v4 = 0;
    CData/*7:0*/ __VdlyVal__top__DOT__vgc_inst__DOT__tile_inst__DOT__col_buffer__v5;
    __VdlyVal__top__DOT__vgc_inst__DOT__tile_inst__DOT__col_buffer__v5 = 0;
    CData/*0:0*/ __VdlySet__top__DOT__vgc_inst__DOT__tile_inst__DOT__col_buffer__v5;
    __VdlySet__top__DOT__vgc_inst__DOT__tile_inst__DOT__col_buffer__v5 = 0;
    CData/*7:0*/ __VdlyVal__top__DOT__vgc_inst__DOT__tile_inst__DOT__col_buffer__v6;
    __VdlyVal__top__DOT__vgc_inst__DOT__tile_inst__DOT__col_buffer__v6 = 0;
    CData/*0:0*/ __VdlySet__top__DOT__vgc_inst__DOT__tile_inst__DOT__col_buffer__v6;
    __VdlySet__top__DOT__vgc_inst__DOT__tile_inst__DOT__col_buffer__v6 = 0;
    CData/*7:0*/ __VdlyVal__top__DOT__vgc_inst__DOT__tile_inst__DOT__col_buffer__v7;
    __VdlyVal__top__DOT__vgc_inst__DOT__tile_inst__DOT__col_buffer__v7 = 0;
    CData/*0:0*/ __VdlySet__top__DOT__vgc_inst__DOT__tile_inst__DOT__col_buffer__v7;
    __VdlySet__top__DOT__vgc_inst__DOT__tile_inst__DOT__col_buffer__v7 = 0;
    CData/*7:0*/ __VdlyVal__top__DOT__vgc_inst__DOT__tile_inst__DOT__col_buffer__v8;
    __VdlyVal__top__DOT__vgc_inst__DOT__tile_inst__DOT__col_buffer__v8 = 0;
    CData/*0:0*/ __VdlySet__top__DOT__vgc_inst__DOT__tile_inst__DOT__col_buffer__v8;
    __VdlySet__top__DOT__vgc_inst__DOT__tile_inst__DOT__col_buffer__v8 = 0;
    CData/*7:0*/ __VdlyVal__top__DOT__vgc_inst__DOT__tile_inst__DOT__col_buffer__v9;
    __VdlyVal__top__DOT__vgc_inst__DOT__tile_inst__DOT__col_buffer__v9 = 0;
    CData/*0:0*/ __VdlySet__top__DOT__vgc_inst__DOT__tile_inst__DOT__col_buffer__v9;
    __VdlySet__top__DOT__vgc_inst__DOT__tile_inst__DOT__col_buffer__v9 = 0;
    CData/*7:0*/ __VdlyVal__top__DOT__vgc_inst__DOT__tile_inst__DOT__col_buffer__v10;
    __VdlyVal__top__DOT__vgc_inst__DOT__tile_inst__DOT__col_buffer__v10 = 0;
    CData/*0:0*/ __VdlySet__top__DOT__vgc_inst__DOT__tile_inst__DOT__col_buffer__v10;
    __VdlySet__top__DOT__vgc_inst__DOT__tile_inst__DOT__col_buffer__v10 = 0;
    CData/*7:0*/ __VdlyVal__top__DOT__vgc_inst__DOT__tile_inst__DOT__col_buffer__v11;
    __VdlyVal__top__DOT__vgc_inst__DOT__tile_inst__DOT__col_buffer__v11 = 0;
    CData/*0:0*/ __VdlySet__top__DOT__vgc_inst__DOT__tile_inst__DOT__col_buffer__v11;
    __VdlySet__top__DOT__vgc_inst__DOT__tile_inst__DOT__col_buffer__v11 = 0;
    CData/*7:0*/ __VdlyVal__top__DOT__vgc_inst__DOT__tile_inst__DOT__col_buffer__v12;
    __VdlyVal__top__DOT__vgc_inst__DOT__tile_inst__DOT__col_buffer__v12 = 0;
    CData/*0:0*/ __VdlySet__top__DOT__vgc_inst__DOT__tile_inst__DOT__col_buffer__v12;
    __VdlySet__top__DOT__vgc_inst__DOT__tile_inst__DOT__col_buffer__v12 = 0;
    CData/*7:0*/ __VdlyVal__top__DOT__vgc_inst__DOT__tile_inst__DOT__col_buffer__v13;
    __VdlyVal__top__DOT__vgc_inst__DOT__tile_inst__DOT__col_buffer__v13 = 0;
    CData/*0:0*/ __VdlySet__top__DOT__vgc_inst__DOT__tile_inst__DOT__col_buffer__v13;
    __VdlySet__top__DOT__vgc_inst__DOT__tile_inst__DOT__col_buffer__v13 = 0;
    CData/*7:0*/ __VdlyVal__top__DOT__vgc_inst__DOT__tile_inst__DOT__col_buffer__v14;
    __VdlyVal__top__DOT__vgc_inst__DOT__tile_inst__DOT__col_buffer__v14 = 0;
    CData/*0:0*/ __VdlySet__top__DOT__vgc_inst__DOT__tile_inst__DOT__col_buffer__v14;
    __VdlySet__top__DOT__vgc_inst__DOT__tile_inst__DOT__col_buffer__v14 = 0;
    CData/*7:0*/ __VdlyVal__top__DOT__vgc_inst__DOT__tile_inst__DOT__col_buffer__v15;
    __VdlyVal__top__DOT__vgc_inst__DOT__tile_inst__DOT__col_buffer__v15 = 0;
    CData/*0:0*/ __VdlySet__top__DOT__vgc_inst__DOT__tile_inst__DOT__col_buffer__v15;
    __VdlySet__top__DOT__vgc_inst__DOT__tile_inst__DOT__col_buffer__v15 = 0;
    CData/*7:0*/ __VdlyVal__top__DOT__vgc_inst__DOT__tile_inst__DOT__col_buffer__v16;
    __VdlyVal__top__DOT__vgc_inst__DOT__tile_inst__DOT__col_buffer__v16 = 0;
    CData/*0:0*/ __VdlySet__top__DOT__vgc_inst__DOT__tile_inst__DOT__col_buffer__v16;
    __VdlySet__top__DOT__vgc_inst__DOT__tile_inst__DOT__col_buffer__v16 = 0;
    CData/*7:0*/ __VdlyVal__top__DOT__vgc_inst__DOT__tile_inst__DOT__col_buffer__v17;
    __VdlyVal__top__DOT__vgc_inst__DOT__tile_inst__DOT__col_buffer__v17 = 0;
    CData/*0:0*/ __VdlySet__top__DOT__vgc_inst__DOT__tile_inst__DOT__col_buffer__v17;
    __VdlySet__top__DOT__vgc_inst__DOT__tile_inst__DOT__col_buffer__v17 = 0;
    CData/*7:0*/ __VdlyVal__top__DOT__vgc_inst__DOT__tile_inst__DOT__col_buffer__v18;
    __VdlyVal__top__DOT__vgc_inst__DOT__tile_inst__DOT__col_buffer__v18 = 0;
    CData/*0:0*/ __VdlySet__top__DOT__vgc_inst__DOT__tile_inst__DOT__col_buffer__v18;
    __VdlySet__top__DOT__vgc_inst__DOT__tile_inst__DOT__col_buffer__v18 = 0;
    CData/*7:0*/ __VdlyVal__top__DOT__vgc_inst__DOT__tile_inst__DOT__col_buffer__v19;
    __VdlyVal__top__DOT__vgc_inst__DOT__tile_inst__DOT__col_buffer__v19 = 0;
    CData/*0:0*/ __VdlySet__top__DOT__vgc_inst__DOT__tile_inst__DOT__col_buffer__v19;
    __VdlySet__top__DOT__vgc_inst__DOT__tile_inst__DOT__col_buffer__v19 = 0;
    CData/*7:0*/ __VdlyVal__top__DOT__vgc_inst__DOT__tile_inst__DOT__col_buffer__v20;
    __VdlyVal__top__DOT__vgc_inst__DOT__tile_inst__DOT__col_buffer__v20 = 0;
    CData/*0:0*/ __VdlySet__top__DOT__vgc_inst__DOT__tile_inst__DOT__col_buffer__v20;
    __VdlySet__top__DOT__vgc_inst__DOT__tile_inst__DOT__col_buffer__v20 = 0;
    CData/*7:0*/ __VdlyVal__top__DOT__vgc_inst__DOT__tile_inst__DOT__col_buffer__v21;
    __VdlyVal__top__DOT__vgc_inst__DOT__tile_inst__DOT__col_buffer__v21 = 0;
    CData/*0:0*/ __VdlySet__top__DOT__vgc_inst__DOT__tile_inst__DOT__col_buffer__v21;
    __VdlySet__top__DOT__vgc_inst__DOT__tile_inst__DOT__col_buffer__v21 = 0;
    CData/*7:0*/ __VdlyVal__top__DOT__vgc_inst__DOT__tile_inst__DOT__col_buffer__v22;
    __VdlyVal__top__DOT__vgc_inst__DOT__tile_inst__DOT__col_buffer__v22 = 0;
    CData/*0:0*/ __VdlySet__top__DOT__vgc_inst__DOT__tile_inst__DOT__col_buffer__v22;
    __VdlySet__top__DOT__vgc_inst__DOT__tile_inst__DOT__col_buffer__v22 = 0;
    CData/*7:0*/ __VdlyVal__top__DOT__vgc_inst__DOT__tile_inst__DOT__col_buffer__v23;
    __VdlyVal__top__DOT__vgc_inst__DOT__tile_inst__DOT__col_buffer__v23 = 0;
    CData/*0:0*/ __VdlySet__top__DOT__vgc_inst__DOT__tile_inst__DOT__col_buffer__v23;
    __VdlySet__top__DOT__vgc_inst__DOT__tile_inst__DOT__col_buffer__v23 = 0;
    CData/*7:0*/ __VdlyVal__top__DOT__vgc_inst__DOT__tile_inst__DOT__col_buffer__v24;
    __VdlyVal__top__DOT__vgc_inst__DOT__tile_inst__DOT__col_buffer__v24 = 0;
    CData/*0:0*/ __VdlySet__top__DOT__vgc_inst__DOT__tile_inst__DOT__col_buffer__v24;
    __VdlySet__top__DOT__vgc_inst__DOT__tile_inst__DOT__col_buffer__v24 = 0;
    CData/*7:0*/ __VdlyVal__top__DOT__vgc_inst__DOT__tile_inst__DOT__col_buffer__v25;
    __VdlyVal__top__DOT__vgc_inst__DOT__tile_inst__DOT__col_buffer__v25 = 0;
    CData/*4:0*/ __VdlyDim0__top__DOT__vgc_inst__DOT__tile_inst__DOT__col_buffer__v25;
    __VdlyDim0__top__DOT__vgc_inst__DOT__tile_inst__DOT__col_buffer__v25 = 0;
    CData/*0:0*/ __VdlySet__top__DOT__vgc_inst__DOT__tile_inst__DOT__col_buffer__v25;
    __VdlySet__top__DOT__vgc_inst__DOT__tile_inst__DOT__col_buffer__v25 = 0;
    CData/*7:0*/ __VdlyVal__top__DOT__vgc_inst__DOT__tile_inst__DOT__col_buffer__v26;
    __VdlyVal__top__DOT__vgc_inst__DOT__tile_inst__DOT__col_buffer__v26 = 0;
    CData/*0:0*/ __VdlySet__top__DOT__vgc_inst__DOT__tile_inst__DOT__col_buffer__v26;
    __VdlySet__top__DOT__vgc_inst__DOT__tile_inst__DOT__col_buffer__v26 = 0;
    CData/*7:0*/ __VdlyVal__top__DOT__vgc_inst__DOT__tile_inst__DOT__col_buffer__v27;
    __VdlyVal__top__DOT__vgc_inst__DOT__tile_inst__DOT__col_buffer__v27 = 0;
    CData/*0:0*/ __VdlySet__top__DOT__vgc_inst__DOT__tile_inst__DOT__col_buffer__v27;
    __VdlySet__top__DOT__vgc_inst__DOT__tile_inst__DOT__col_buffer__v27 = 0;
    CData/*7:0*/ __VdlyVal__top__DOT__vgc_inst__DOT__tile_inst__DOT__col_buffer__v28;
    __VdlyVal__top__DOT__vgc_inst__DOT__tile_inst__DOT__col_buffer__v28 = 0;
    CData/*0:0*/ __VdlySet__top__DOT__vgc_inst__DOT__tile_inst__DOT__col_buffer__v28;
    __VdlySet__top__DOT__vgc_inst__DOT__tile_inst__DOT__col_buffer__v28 = 0;
    CData/*7:0*/ __VdlyVal__top__DOT__vgc_inst__DOT__tile_inst__DOT__col_buffer__v29;
    __VdlyVal__top__DOT__vgc_inst__DOT__tile_inst__DOT__col_buffer__v29 = 0;
    CData/*0:0*/ __VdlySet__top__DOT__vgc_inst__DOT__tile_inst__DOT__col_buffer__v29;
    __VdlySet__top__DOT__vgc_inst__DOT__tile_inst__DOT__col_buffer__v29 = 0;
    CData/*7:0*/ __VdlyVal__top__DOT__vgc_inst__DOT__tile_inst__DOT__col_buffer__v30;
    __VdlyVal__top__DOT__vgc_inst__DOT__tile_inst__DOT__col_buffer__v30 = 0;
    CData/*0:0*/ __VdlySet__top__DOT__vgc_inst__DOT__tile_inst__DOT__col_buffer__v30;
    __VdlySet__top__DOT__vgc_inst__DOT__tile_inst__DOT__col_buffer__v30 = 0;
    CData/*7:0*/ __VdlyVal__top__DOT__vgc_inst__DOT__tile_inst__DOT__col_buffer__v31;
    __VdlyVal__top__DOT__vgc_inst__DOT__tile_inst__DOT__col_buffer__v31 = 0;
    CData/*0:0*/ __VdlySet__top__DOT__vgc_inst__DOT__tile_inst__DOT__col_buffer__v31;
    __VdlySet__top__DOT__vgc_inst__DOT__tile_inst__DOT__col_buffer__v31 = 0;
    CData/*7:0*/ __VdlyVal__top__DOT__vgc_inst__DOT__tile_inst__DOT__col_buffer__v32;
    __VdlyVal__top__DOT__vgc_inst__DOT__tile_inst__DOT__col_buffer__v32 = 0;
    CData/*0:0*/ __VdlySet__top__DOT__vgc_inst__DOT__tile_inst__DOT__col_buffer__v32;
    __VdlySet__top__DOT__vgc_inst__DOT__tile_inst__DOT__col_buffer__v32 = 0;
    CData/*7:0*/ __VdlyVal__top__DOT__vgc_inst__DOT__tile_inst__DOT__col_buffer__v33;
    __VdlyVal__top__DOT__vgc_inst__DOT__tile_inst__DOT__col_buffer__v33 = 0;
    CData/*0:0*/ __VdlySet__top__DOT__vgc_inst__DOT__tile_inst__DOT__col_buffer__v33;
    __VdlySet__top__DOT__vgc_inst__DOT__tile_inst__DOT__col_buffer__v33 = 0;
    CData/*7:0*/ __VdlyVal__top__DOT__vgc_inst__DOT__tile_inst__DOT__col_buffer__v34;
    __VdlyVal__top__DOT__vgc_inst__DOT__tile_inst__DOT__col_buffer__v34 = 0;
    CData/*0:0*/ __VdlySet__top__DOT__vgc_inst__DOT__tile_inst__DOT__col_buffer__v34;
    __VdlySet__top__DOT__vgc_inst__DOT__tile_inst__DOT__col_buffer__v34 = 0;
    CData/*7:0*/ __VdlyVal__top__DOT__vgc_inst__DOT__tile_inst__DOT__col_buffer__v35;
    __VdlyVal__top__DOT__vgc_inst__DOT__tile_inst__DOT__col_buffer__v35 = 0;
    CData/*0:0*/ __VdlySet__top__DOT__vgc_inst__DOT__tile_inst__DOT__col_buffer__v35;
    __VdlySet__top__DOT__vgc_inst__DOT__tile_inst__DOT__col_buffer__v35 = 0;
    CData/*7:0*/ __VdlyVal__top__DOT__vgc_inst__DOT__tile_inst__DOT__col_buffer__v36;
    __VdlyVal__top__DOT__vgc_inst__DOT__tile_inst__DOT__col_buffer__v36 = 0;
    CData/*0:0*/ __VdlySet__top__DOT__vgc_inst__DOT__tile_inst__DOT__col_buffer__v36;
    __VdlySet__top__DOT__vgc_inst__DOT__tile_inst__DOT__col_buffer__v36 = 0;
    CData/*7:0*/ __VdlyVal__top__DOT__vgc_inst__DOT__tile_inst__DOT__col_buffer__v37;
    __VdlyVal__top__DOT__vgc_inst__DOT__tile_inst__DOT__col_buffer__v37 = 0;
    CData/*0:0*/ __VdlySet__top__DOT__vgc_inst__DOT__tile_inst__DOT__col_buffer__v37;
    __VdlySet__top__DOT__vgc_inst__DOT__tile_inst__DOT__col_buffer__v37 = 0;
    CData/*7:0*/ __VdlyVal__top__DOT__vgc_inst__DOT__tile_inst__DOT__col_buffer__v38;
    __VdlyVal__top__DOT__vgc_inst__DOT__tile_inst__DOT__col_buffer__v38 = 0;
    CData/*0:0*/ __VdlySet__top__DOT__vgc_inst__DOT__tile_inst__DOT__col_buffer__v38;
    __VdlySet__top__DOT__vgc_inst__DOT__tile_inst__DOT__col_buffer__v38 = 0;
    CData/*7:0*/ __VdlyVal__top__DOT__vgc_inst__DOT__tile_inst__DOT__col_buffer__v39;
    __VdlyVal__top__DOT__vgc_inst__DOT__tile_inst__DOT__col_buffer__v39 = 0;
    CData/*0:0*/ __VdlySet__top__DOT__vgc_inst__DOT__tile_inst__DOT__col_buffer__v39;
    __VdlySet__top__DOT__vgc_inst__DOT__tile_inst__DOT__col_buffer__v39 = 0;
    CData/*7:0*/ __VdlyVal__top__DOT__vgc_inst__DOT__tile_inst__DOT__col_buffer__v40;
    __VdlyVal__top__DOT__vgc_inst__DOT__tile_inst__DOT__col_buffer__v40 = 0;
    CData/*0:0*/ __VdlySet__top__DOT__vgc_inst__DOT__tile_inst__DOT__col_buffer__v40;
    __VdlySet__top__DOT__vgc_inst__DOT__tile_inst__DOT__col_buffer__v40 = 0;
    CData/*7:0*/ __VdlyVal__top__DOT__vgc_inst__DOT__tile_inst__DOT__col_buffer__v41;
    __VdlyVal__top__DOT__vgc_inst__DOT__tile_inst__DOT__col_buffer__v41 = 0;
    CData/*0:0*/ __VdlySet__top__DOT__vgc_inst__DOT__tile_inst__DOT__col_buffer__v41;
    __VdlySet__top__DOT__vgc_inst__DOT__tile_inst__DOT__col_buffer__v41 = 0;
    CData/*7:0*/ __VdlyVal__top__DOT__vgc_inst__DOT__tile_inst__DOT__col_buffer__v42;
    __VdlyVal__top__DOT__vgc_inst__DOT__tile_inst__DOT__col_buffer__v42 = 0;
    CData/*0:0*/ __VdlySet__top__DOT__vgc_inst__DOT__tile_inst__DOT__col_buffer__v42;
    __VdlySet__top__DOT__vgc_inst__DOT__tile_inst__DOT__col_buffer__v42 = 0;
    CData/*7:0*/ __VdlyVal__top__DOT__vgc_inst__DOT__tile_inst__DOT__col_buffer__v43;
    __VdlyVal__top__DOT__vgc_inst__DOT__tile_inst__DOT__col_buffer__v43 = 0;
    CData/*0:0*/ __VdlySet__top__DOT__vgc_inst__DOT__tile_inst__DOT__col_buffer__v43;
    __VdlySet__top__DOT__vgc_inst__DOT__tile_inst__DOT__col_buffer__v43 = 0;
    CData/*7:0*/ __VdlyVal__top__DOT__vgc_inst__DOT__tile_inst__DOT__col_buffer__v44;
    __VdlyVal__top__DOT__vgc_inst__DOT__tile_inst__DOT__col_buffer__v44 = 0;
    CData/*0:0*/ __VdlySet__top__DOT__vgc_inst__DOT__tile_inst__DOT__col_buffer__v44;
    __VdlySet__top__DOT__vgc_inst__DOT__tile_inst__DOT__col_buffer__v44 = 0;
    CData/*7:0*/ __VdlyVal__top__DOT__vgc_inst__DOT__tile_inst__DOT__col_buffer__v45;
    __VdlyVal__top__DOT__vgc_inst__DOT__tile_inst__DOT__col_buffer__v45 = 0;
    CData/*0:0*/ __VdlySet__top__DOT__vgc_inst__DOT__tile_inst__DOT__col_buffer__v45;
    __VdlySet__top__DOT__vgc_inst__DOT__tile_inst__DOT__col_buffer__v45 = 0;
    CData/*7:0*/ __VdlyVal__top__DOT__vgc_inst__DOT__tile_inst__DOT__col_buffer__v46;
    __VdlyVal__top__DOT__vgc_inst__DOT__tile_inst__DOT__col_buffer__v46 = 0;
    CData/*0:0*/ __VdlySet__top__DOT__vgc_inst__DOT__tile_inst__DOT__col_buffer__v46;
    __VdlySet__top__DOT__vgc_inst__DOT__tile_inst__DOT__col_buffer__v46 = 0;
    CData/*7:0*/ __VdlyVal__top__DOT__vgc_inst__DOT__tile_inst__DOT__col_buffer__v47;
    __VdlyVal__top__DOT__vgc_inst__DOT__tile_inst__DOT__col_buffer__v47 = 0;
    CData/*0:0*/ __VdlySet__top__DOT__vgc_inst__DOT__tile_inst__DOT__col_buffer__v47;
    __VdlySet__top__DOT__vgc_inst__DOT__tile_inst__DOT__col_buffer__v47 = 0;
    CData/*7:0*/ __VdlyVal__top__DOT__vgc_inst__DOT__tile_inst__DOT__col_buffer__v48;
    __VdlyVal__top__DOT__vgc_inst__DOT__tile_inst__DOT__col_buffer__v48 = 0;
    CData/*0:0*/ __VdlySet__top__DOT__vgc_inst__DOT__tile_inst__DOT__col_buffer__v48;
    __VdlySet__top__DOT__vgc_inst__DOT__tile_inst__DOT__col_buffer__v48 = 0;
    CData/*7:0*/ __VdlyVal__top__DOT__vgc_inst__DOT__tile_inst__DOT__col_buffer__v49;
    __VdlyVal__top__DOT__vgc_inst__DOT__tile_inst__DOT__col_buffer__v49 = 0;
    CData/*0:0*/ __VdlySet__top__DOT__vgc_inst__DOT__tile_inst__DOT__col_buffer__v49;
    __VdlySet__top__DOT__vgc_inst__DOT__tile_inst__DOT__col_buffer__v49 = 0;
    CData/*7:0*/ __VdlyVal__top__DOT__vgc_inst__DOT__tile_inst__DOT__col_buffer__v50;
    __VdlyVal__top__DOT__vgc_inst__DOT__tile_inst__DOT__col_buffer__v50 = 0;
    CData/*0:0*/ __VdlySet__top__DOT__vgc_inst__DOT__tile_inst__DOT__col_buffer__v50;
    __VdlySet__top__DOT__vgc_inst__DOT__tile_inst__DOT__col_buffer__v50 = 0;
    CData/*7:0*/ __VdlyVal__top__DOT__vgc_inst__DOT__tile_inst__DOT__tile_data_ram__DOT__mem__v0;
    __VdlyVal__top__DOT__vgc_inst__DOT__tile_inst__DOT__tile_data_ram__DOT__mem__v0 = 0;
    SData/*14:0*/ __VdlyDim0__top__DOT__vgc_inst__DOT__tile_inst__DOT__tile_data_ram__DOT__mem__v0;
    __VdlyDim0__top__DOT__vgc_inst__DOT__tile_inst__DOT__tile_data_ram__DOT__mem__v0 = 0;
    CData/*0:0*/ __VdlySet__top__DOT__vgc_inst__DOT__tile_inst__DOT__tile_data_ram__DOT__mem__v0;
    __VdlySet__top__DOT__vgc_inst__DOT__tile_inst__DOT__tile_data_ram__DOT__mem__v0 = 0;
    CData/*7:0*/ __VdlyVal__top__DOT__vgc_inst__DOT__tile_inst__DOT__nametable_ram__DOT__mem__v0;
    __VdlyVal__top__DOT__vgc_inst__DOT__tile_inst__DOT__nametable_ram__DOT__mem__v0 = 0;
    SData/*11:0*/ __VdlyDim0__top__DOT__vgc_inst__DOT__tile_inst__DOT__nametable_ram__DOT__mem__v0;
    __VdlyDim0__top__DOT__vgc_inst__DOT__tile_inst__DOT__nametable_ram__DOT__mem__v0 = 0;
    CData/*0:0*/ __VdlySet__top__DOT__vgc_inst__DOT__tile_inst__DOT__nametable_ram__DOT__mem__v0;
    __VdlySet__top__DOT__vgc_inst__DOT__tile_inst__DOT__nametable_ram__DOT__mem__v0 = 0;
    SData/*11:0*/ __VdlyVal__top__DOT__vgc_inst__DOT__tile_inst__DOT__pal_ram_inst__DOT__mem__v0;
    __VdlyVal__top__DOT__vgc_inst__DOT__tile_inst__DOT__pal_ram_inst__DOT__mem__v0 = 0;
    CData/*7:0*/ __VdlyDim0__top__DOT__vgc_inst__DOT__tile_inst__DOT__pal_ram_inst__DOT__mem__v0;
    __VdlyDim0__top__DOT__vgc_inst__DOT__tile_inst__DOT__pal_ram_inst__DOT__mem__v0 = 0;
    CData/*0:0*/ __VdlySet__top__DOT__vgc_inst__DOT__tile_inst__DOT__pal_ram_inst__DOT__mem__v0;
    __VdlySet__top__DOT__vgc_inst__DOT__tile_inst__DOT__pal_ram_inst__DOT__mem__v0 = 0;
    // Body
    __VdlySet__top__DOT__vgc_inst__DOT__tile_inst__DOT__tile_data_ram__DOT__mem__v0 = 0U;
    __VdlySet__top__DOT__vgc_inst__DOT__sprite_inst__DOT__spr_mem__DOT__mem__v0 = 0U;
    __VdlySet__top__DOT__vgc_inst__DOT__tile_inst__DOT__pal_ram_inst__DOT__mem__v0 = 0U;
    vlSelfRef.__VdlySet__top__DOT__vgc_inst__DOT__tile_inst__DOT__attr_table_ram__DOT__mem__v0 = 0U;
    __VdlySet__top__DOT__vgc_inst__DOT__text_inst__DOT__char_mem__DOT__mem__v0 = 0U;
    __VdlySet__top__DOT__vgc_inst__DOT__text_inst__DOT__color_mem__DOT__mem__v0 = 0U;
    __VdlySet__top__DOT__vgc_inst__DOT__gfx_inst__DOT__gfx_mem__DOT__mem__v0 = 0U;
    __VdlySet__top__DOT__vgc_inst__DOT__tile_inst__DOT__nametable_ram__DOT__mem__v0 = 0U;
    __Vdly__top__DOT__vgc_inst__DOT__cursor_blink = vlSelfRef.top__DOT__vgc_inst__DOT__cursor_blink;
    __Vdly__top__DOT__vgc_inst__DOT__blink_count = vlSelfRef.top__DOT__vgc_inst__DOT__blink_count;
    vlSelfRef.__Vdly__top__DOT__sid_clk_div = vlSelfRef.top__DOT__sid_clk_div;
    __VdlySet__top__DOT__ram__v2 = 0U;
    __VdlySet__top__DOT__ram__v4 = 0U;
    __VdlySet__top__DOT__xram__v0 = 0U;
    __VdlySet__top__DOT__xram__v1 = 0U;
    __Vdly__top__DOT__vgc_inst__DOT__copper_index = vlSelfRef.top__DOT__vgc_inst__DOT__copper_index;
    __VdlySet__top__DOT__xmc_regs__v0 = 0U;
    __Vdly__top__DOT__vgc_inst__DOT__h_count = vlSelfRef.top__DOT__vgc_inst__DOT__h_count;
    __Vdly__top__DOT__vgc_inst__DOT__v_count = vlSelfRef.top__DOT__vgc_inst__DOT__v_count;
    __VdlySet__top__DOT__ram__v3 = 0U;
    __VdlySet__top__DOT__vgc_inst__DOT__fio_name__v0 = 0U;
    __VdlySet__top__DOT__vgc_inst__DOT__tile_inst__DOT__tregs__v0 = 0U;
    __VdlySet__top__DOT__vgc_inst__DOT__spr_shape__v0 = 0U;
    __VdlySet__top__DOT__vgc_inst__DOT__spr_shape__v16 = 0U;
    __VdlySet__top__DOT__vgc_inst__DOT__spr_trans__v0 = 0U;
    __VdlySet__top__DOT__vgc_inst__DOT__spr_trans__v16 = 0U;
    __Vdly__top__DOT__vgc_inst__DOT__sprite_inst__DOT__spr_eval_state 
        = vlSelfRef.top__DOT__vgc_inst__DOT__sprite_inst__DOT__spr_eval_state;
    __Vdly__top__DOT__vgc_inst__DOT__sprite_inst__DOT__spr_eval_idx 
        = vlSelfRef.top__DOT__vgc_inst__DOT__sprite_inst__DOT__spr_eval_idx;
    __Vdly__top__DOT__vgc_inst__DOT__sprite_inst__DOT__spr_clear_x 
        = vlSelfRef.top__DOT__vgc_inst__DOT__sprite_inst__DOT__spr_clear_x;
    __Vdly__top__DOT__vgc_inst__DOT__sprite_inst__DOT__spr_b_addr 
        = vlSelfRef.top__DOT__vgc_inst__DOT__sprite_inst__DOT__spr_b_addr;
    __VdlySet__top__DOT__vgc_inst__DOT__sprite_inst__DOT__spr_row_data__v0 = 0U;
    __VdlySet__top__DOT__vgc_inst__DOT__sprite_inst__DOT__spr_row_data__v1 = 0U;
    __VdlySet__top__DOT__ram__v0 = 0U;
    __VdlySet__top__DOT__ram__v1 = 0U;
    __VdlySet__top__DOT__vgc_inst__DOT__sprite_inst__DOT__scanline_buf__DOT__mem__v0 = 0U;
    __Vdly__top__DOT__vgc_inst__DOT__scroll_pending 
        = vlSelfRef.top__DOT__vgc_inst__DOT__scroll_pending;
    __Vdly__top__DOT__vgc_inst__DOT__scroll_col = vlSelfRef.top__DOT__vgc_inst__DOT__scroll_col;
    __Vdly__top__DOT__vgc_inst__DOT__cmd_busy = vlSelfRef.top__DOT__vgc_inst__DOT__cmd_busy;
    __Vdly__top__DOT__vgc_inst__DOT__copper_enabled 
        = vlSelfRef.top__DOT__vgc_inst__DOT__copper_enabled;
    __Vdly__top__DOT__vgc_inst__DOT__copper_count = vlSelfRef.top__DOT__vgc_inst__DOT__copper_count;
    __Vdly__top__DOT__vgc_inst__DOT__copper_target_list 
        = vlSelfRef.top__DOT__vgc_inst__DOT__copper_target_list;
    __Vdly__top__DOT__vgc_inst__DOT__copper_pending_list 
        = vlSelfRef.top__DOT__vgc_inst__DOT__copper_pending_list;
    __Vdly__top__DOT__vgc_inst__DOT__copper_loading 
        = vlSelfRef.top__DOT__vgc_inst__DOT__copper_loading;
    __Vdly__top__DOT__vgc_inst__DOT__sprrow_count = vlSelfRef.top__DOT__vgc_inst__DOT__sprrow_count;
    __Vdly__top__DOT__vgc_inst__DOT__sprcopy_phase 
        = vlSelfRef.top__DOT__vgc_inst__DOT__sprcopy_phase;
    __Vdly__top__DOT__vgc_inst__DOT__memread_pending 
        = vlSelfRef.top__DOT__vgc_inst__DOT__memread_pending;
    __Vdly__top__DOT__vgc_inst__DOT__cmd_cx = vlSelfRef.top__DOT__vgc_inst__DOT__cmd_cx;
    __Vdly__top__DOT__vgc_inst__DOT__cmd_cy = vlSelfRef.top__DOT__vgc_inst__DOT__cmd_cy;
    __Vdly__top__DOT__vgc_inst__DOT__copper_active_list 
        = vlSelfRef.top__DOT__vgc_inst__DOT__copper_active_list;
    __Vdly__top__DOT__vgc_inst__DOT__cmd_op = vlSelfRef.top__DOT__vgc_inst__DOT__cmd_op;
    __VdlySet__top__DOT__vgc_inst__DOT__sprrow_data__v0 = 0U;
    __VdlySet__top__DOT__vgc_inst__DOT__spr_y__v17 = 0U;
    __VdlySet__top__DOT__vgc_inst__DOT__spr_pri__v16 = 0U;
    __VdlySet__top__DOT__vgc_inst__DOT__spr_pri__v17 = 0U;
    __VdlySet__top__DOT__vgc_inst__DOT__copper_list_pos__v0 = 0U;
    __VdlySet__top__DOT__vgc_inst__DOT__spr_enable__v16 = 0U;
    __VdlySet__top__DOT__vgc_inst__DOT__spr_enable__v17 = 0U;
    __Vdly__top__DOT__vgc_inst__DOT__scroll_offset 
        = vlSelfRef.top__DOT__vgc_inst__DOT__scroll_offset;
    __VdlySet__top__DOT__vgc_inst__DOT__spr_x__v17 = 0U;
    __VdlySet__top__DOT__vgc_inst__DOT__spr_x__v18 = 0U;
    __VdlySet__top__DOT__vgc_inst__DOT__spr_flip_h__v16 = 0U;
    __Vdly__top__DOT__vgc_inst__DOT__fg_color = vlSelfRef.top__DOT__vgc_inst__DOT__fg_color;
    __VdlySet__top__DOT__vgc_inst__DOT__spr_x__v16 = 0U;
    __Vdly__top__DOT__vgc_inst__DOT__cursor_x = vlSelfRef.top__DOT__vgc_inst__DOT__cursor_x;
    __Vdly__top__DOT__vgc_inst__DOT__cursor_y = vlSelfRef.top__DOT__vgc_inst__DOT__cursor_y;
    __VdlySet__top__DOT__vgc_inst__DOT__copper_pos__v0 = 0U;
    __VdlySet__top__DOT__vgc_inst__DOT__copper_pos__v1 = 0U;
    __VdlySet__top__DOT__vgc_inst__DOT__spr_flip_h__v17 = 0U;
    __VdlySet__top__DOT__vgc_inst__DOT__regs__v32 = 0U;
    __VdlySet__top__DOT__vgc_inst__DOT__regs__v33 = 0U;
    __VdlySet__top__DOT__vgc_inst__DOT__regs__v34 = 0U;
    __VdlySet__top__DOT__vgc_inst__DOT__regs__v35 = 0U;
    __VdlySet__top__DOT__vgc_inst__DOT__regs__v0 = 0U;
    __Vdly__top__DOT__vgc_inst__DOT__tile_inst__DOT__dma_src 
        = vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__dma_src;
    __Vdly__top__DOT__vgc_inst__DOT__tile_inst__DOT__dma_pal_phase 
        = vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__dma_pal_phase;
    __Vdly__top__DOT__vgc_inst__DOT__tile_inst__DOT__dma_dst 
        = vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__dma_dst;
    __Vdly__top__DOT__vgc_inst__DOT__tile_inst__DOT__dma_count 
        = vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__dma_count;
    __Vdly__top__DOT__vgc_inst__DOT__tile_inst__DOT__dma_col_idx 
        = vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__dma_col_idx;
    __Vdly__top__DOT__vgc_inst__DOT__tile_inst__DOT__dma_state 
        = vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__dma_state;
    __Vdly__top__DOT__vgc_inst__DOT__tile_inst__DOT__dma_data_valid 
        = vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__dma_data_valid;
    __VdlySet__top__DOT__vgc_inst__DOT__tile_inst__DOT__col_buffer__v0 = 0U;
    __VdlySet__top__DOT__vgc_inst__DOT__tile_inst__DOT__col_buffer__v1 = 0U;
    __VdlySet__top__DOT__vgc_inst__DOT__tile_inst__DOT__col_buffer__v2 = 0U;
    __VdlySet__top__DOT__vgc_inst__DOT__tile_inst__DOT__col_buffer__v3 = 0U;
    __VdlySet__top__DOT__vgc_inst__DOT__tile_inst__DOT__col_buffer__v4 = 0U;
    __VdlySet__top__DOT__vgc_inst__DOT__tile_inst__DOT__col_buffer__v5 = 0U;
    __VdlySet__top__DOT__vgc_inst__DOT__tile_inst__DOT__col_buffer__v6 = 0U;
    __VdlySet__top__DOT__vgc_inst__DOT__tile_inst__DOT__col_buffer__v7 = 0U;
    __VdlySet__top__DOT__vgc_inst__DOT__tile_inst__DOT__col_buffer__v8 = 0U;
    __VdlySet__top__DOT__vgc_inst__DOT__tile_inst__DOT__col_buffer__v9 = 0U;
    __VdlySet__top__DOT__vgc_inst__DOT__tile_inst__DOT__col_buffer__v10 = 0U;
    __VdlySet__top__DOT__vgc_inst__DOT__tile_inst__DOT__col_buffer__v11 = 0U;
    __VdlySet__top__DOT__vgc_inst__DOT__tile_inst__DOT__col_buffer__v12 = 0U;
    __VdlySet__top__DOT__vgc_inst__DOT__tile_inst__DOT__col_buffer__v13 = 0U;
    __VdlySet__top__DOT__vgc_inst__DOT__tile_inst__DOT__col_buffer__v14 = 0U;
    __VdlySet__top__DOT__vgc_inst__DOT__tile_inst__DOT__col_buffer__v15 = 0U;
    __VdlySet__top__DOT__vgc_inst__DOT__tile_inst__DOT__col_buffer__v16 = 0U;
    __VdlySet__top__DOT__vgc_inst__DOT__tile_inst__DOT__col_buffer__v17 = 0U;
    __VdlySet__top__DOT__vgc_inst__DOT__tile_inst__DOT__col_buffer__v18 = 0U;
    __VdlySet__top__DOT__vgc_inst__DOT__tile_inst__DOT__col_buffer__v19 = 0U;
    __VdlySet__top__DOT__vgc_inst__DOT__tile_inst__DOT__col_buffer__v20 = 0U;
    __VdlySet__top__DOT__vgc_inst__DOT__tile_inst__DOT__col_buffer__v21 = 0U;
    __VdlySet__top__DOT__vgc_inst__DOT__tile_inst__DOT__col_buffer__v22 = 0U;
    __VdlySet__top__DOT__vgc_inst__DOT__tile_inst__DOT__col_buffer__v23 = 0U;
    __VdlySet__top__DOT__vgc_inst__DOT__tile_inst__DOT__col_buffer__v24 = 0U;
    __VdlySet__top__DOT__vgc_inst__DOT__tile_inst__DOT__col_buffer__v25 = 0U;
    __VdlySet__top__DOT__vgc_inst__DOT__tile_inst__DOT__col_buffer__v26 = 0U;
    __VdlySet__top__DOT__vgc_inst__DOT__tile_inst__DOT__col_buffer__v27 = 0U;
    __VdlySet__top__DOT__vgc_inst__DOT__tile_inst__DOT__col_buffer__v28 = 0U;
    __VdlySet__top__DOT__vgc_inst__DOT__tile_inst__DOT__col_buffer__v29 = 0U;
    __VdlySet__top__DOT__vgc_inst__DOT__tile_inst__DOT__col_buffer__v30 = 0U;
    __VdlySet__top__DOT__vgc_inst__DOT__tile_inst__DOT__col_buffer__v31 = 0U;
    __VdlySet__top__DOT__vgc_inst__DOT__tile_inst__DOT__col_buffer__v32 = 0U;
    __VdlySet__top__DOT__vgc_inst__DOT__tile_inst__DOT__col_buffer__v33 = 0U;
    __VdlySet__top__DOT__vgc_inst__DOT__tile_inst__DOT__col_buffer__v34 = 0U;
    __VdlySet__top__DOT__vgc_inst__DOT__tile_inst__DOT__col_buffer__v35 = 0U;
    __VdlySet__top__DOT__vgc_inst__DOT__tile_inst__DOT__col_buffer__v36 = 0U;
    __VdlySet__top__DOT__vgc_inst__DOT__tile_inst__DOT__col_buffer__v37 = 0U;
    __VdlySet__top__DOT__vgc_inst__DOT__tile_inst__DOT__col_buffer__v38 = 0U;
    __VdlySet__top__DOT__vgc_inst__DOT__tile_inst__DOT__col_buffer__v39 = 0U;
    __VdlySet__top__DOT__vgc_inst__DOT__tile_inst__DOT__col_buffer__v40 = 0U;
    __VdlySet__top__DOT__vgc_inst__DOT__tile_inst__DOT__col_buffer__v41 = 0U;
    __VdlySet__top__DOT__vgc_inst__DOT__tile_inst__DOT__col_buffer__v42 = 0U;
    __VdlySet__top__DOT__vgc_inst__DOT__tile_inst__DOT__col_buffer__v43 = 0U;
    __VdlySet__top__DOT__vgc_inst__DOT__tile_inst__DOT__col_buffer__v44 = 0U;
    __VdlySet__top__DOT__vgc_inst__DOT__tile_inst__DOT__col_buffer__v45 = 0U;
    __VdlySet__top__DOT__vgc_inst__DOT__tile_inst__DOT__col_buffer__v46 = 0U;
    __VdlySet__top__DOT__vgc_inst__DOT__tile_inst__DOT__col_buffer__v47 = 0U;
    __VdlySet__top__DOT__vgc_inst__DOT__tile_inst__DOT__col_buffer__v48 = 0U;
    __VdlySet__top__DOT__vgc_inst__DOT__tile_inst__DOT__col_buffer__v49 = 0U;
    __VdlySet__top__DOT__vgc_inst__DOT__tile_inst__DOT__col_buffer__v50 = 0U;
    __Vdly__top__DOT__blt_inst__DOT__wrote_count = vlSelfRef.top__DOT__blt_inst__DOT__wrote_count;
    __VdlySet__top__DOT__blt_inst__DOT__regs__v0 = 0U;
    __VdlySet__top__DOT__blt_inst__DOT__regs__v1 = 0U;
    __VdlySet__top__DOT__blt_inst__DOT__regs__v2 = 0U;
    __VdlySet__top__DOT__blt_inst__DOT__regs__v3 = 0U;
    __VdlySet__top__DOT__blt_inst__DOT__regs__v7 = 0U;
    __VdlySet__top__DOT__blt_inst__DOT__regs__v8 = 0U;
    __VdlySet__top__DOT__blt_inst__DOT__regs__v12 = 0U;
    __VdlySet__top__DOT__blt_inst__DOT__regs__v13 = 0U;
    __VdlySet__top__DOT__blt_inst__DOT__regs__v14 = 0U;
    __VdlySet__top__DOT__blt_inst__DOT__regs__v15 = 0U;
    __VdlySet__top__DOT__blt_inst__DOT__regs__v16 = 0U;
    __VdlySet__top__DOT__blt_inst__DOT__regs__v17 = 0U;
    __VdlySet__top__DOT__blt_inst__DOT__regs__v18 = 0U;
    __VdlySet__top__DOT__blt_inst__DOT__regs__v19 = 0U;
    __VdlySet__top__DOT__blt_inst__DOT__regs__v20 = 0U;
    __VdlySet__top__DOT__blt_inst__DOT__regs__v21 = 0U;
    __VdlySet__top__DOT__blt_inst__DOT__regs__v22 = 0U;
    __VdlySet__top__DOT__blt_inst__DOT__regs__v23 = 0U;
    __VdlySet__top__DOT__blt_inst__DOT__regs__v24 = 0U;
    __VdlySet__top__DOT__blt_inst__DOT__regs__v25 = 0U;
    __VdlySet__top__DOT__blt_inst__DOT__regs__v30 = 0U;
    __VdlySet__top__DOT__blt_inst__DOT__regs__v32 = 0U;
    __Vdly__top__DOT__blt_inst__DOT__col = vlSelfRef.top__DOT__blt_inst__DOT__col;
    __Vdly__top__DOT__blt_inst__DOT__row = vlSelfRef.top__DOT__blt_inst__DOT__row;
    __Vdly__top__DOT__blt_inst__DOT__read_valid = vlSelfRef.top__DOT__blt_inst__DOT__read_valid;
    __Vdly__top__DOT__blt_inst__DOT__buf_idx = vlSelfRef.top__DOT__blt_inst__DOT__buf_idx;
    __VdlySet__top__DOT__blt_inst__DOT__row_buf__v0 = 0U;
    __Vdly__top__DOT__vgc_inst__DOT__artist_busy = vlSelfRef.top__DOT__vgc_inst__DOT__artist_busy;
    __Vdly__top__DOT__vgc_inst__DOT__artist_inst__DOT__op 
        = vlSelfRef.top__DOT__vgc_inst__DOT__artist_inst__DOT__op;
    __Vdly__top__DOT__vgc_inst__DOT__artist_inst__DOT__fill_sp 
        = vlSelfRef.top__DOT__vgc_inst__DOT__artist_inst__DOT__fill_sp;
    __Vdly__top__DOT__vgc_inst__DOT__artist_inst__DOT__octant_count 
        = vlSelfRef.top__DOT__vgc_inst__DOT__artist_inst__DOT__octant_count;
    __Vdly__top__DOT__vgc_inst__DOT__artist_inst__DOT__paint_phase 
        = vlSelfRef.top__DOT__vgc_inst__DOT__artist_inst__DOT__paint_phase;
    __Vdly__top__DOT__vgc_inst__DOT__artist_inst__DOT__gt_font_pending 
        = vlSelfRef.top__DOT__vgc_inst__DOT__artist_inst__DOT__gt_font_pending;
    __Vdly__top__DOT__vgc_inst__DOT__artist_inst__DOT__paint_init 
        = vlSelfRef.top__DOT__vgc_inst__DOT__artist_inst__DOT__paint_init;
    __Vdly__top__DOT__vgc_inst__DOT__artist_inst__DOT__gt_font_byte 
        = vlSelfRef.top__DOT__vgc_inst__DOT__artist_inst__DOT__gt_font_byte;
    __Vdly__top__DOT__vgc_inst__DOT__artist_inst__DOT__err 
        = vlSelfRef.top__DOT__vgc_inst__DOT__artist_inst__DOT__err;
    __Vdly__top__DOT__vgc_inst__DOT__artist_inst__DOT__x 
        = vlSelfRef.top__DOT__vgc_inst__DOT__artist_inst__DOT__x;
    __Vdly__top__DOT__vgc_inst__DOT__artist_inst__DOT__y 
        = vlSelfRef.top__DOT__vgc_inst__DOT__artist_inst__DOT__y;
    __Vdly__top__DOT__vgc_inst__DOT__artist_inst__DOT__cx 
        = vlSelfRef.top__DOT__vgc_inst__DOT__artist_inst__DOT__cx;
    __Vdly__top__DOT__vgc_inst__DOT__artist_inst__DOT__cy 
        = vlSelfRef.top__DOT__vgc_inst__DOT__artist_inst__DOT__cy;
    __Vdly__top__DOT__vgc_inst__DOT__artist_inst__DOT__dx 
        = vlSelfRef.top__DOT__vgc_inst__DOT__artist_inst__DOT__dx;
    __Vdly__top__DOT__vgc_inst__DOT__artist_inst__DOT__dy 
        = vlSelfRef.top__DOT__vgc_inst__DOT__artist_inst__DOT__dy;
    __Vdly__top__DOT__vgc_inst__DOT__artist_inst__DOT__fill_target 
        = vlSelfRef.top__DOT__vgc_inst__DOT__artist_inst__DOT__fill_target;
    __Vdly__top__DOT__vgc_inst__DOT__artist_inst__DOT__fs_b_addr 
        = vlSelfRef.top__DOT__vgc_inst__DOT__artist_inst__DOT__fs_b_addr;
    __Vdly__top__DOT__vgc_inst__DOT__artist_inst__DOT__gt_font_row 
        = vlSelfRef.top__DOT__vgc_inst__DOT__artist_inst__DOT__gt_font_row;
    __Vdly__top__DOT__vgc_inst__DOT__artist_inst__DOT__gt_char_idx 
        = vlSelfRef.top__DOT__vgc_inst__DOT__artist_inst__DOT__gt_char_idx;
    __VdlySet__top__DOT__vgc_inst__DOT__artist_inst__DOT__push_neighbors__v0 = 0U;
    __Vdly__top__DOT__blt_inst__DOT__state = vlSelfRef.top__DOT__blt_inst__DOT__state;
    __VdlySet__top__DOT__vgc_inst__DOT__artist_inst__DOT__fill_stack_mem__DOT__mem__v0 = 0U;
    __VdlySet__top__DOT__cpu_inst__DOT__AXYS__v0 = 0U;
    if (vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__td_we_a) {
        __VdlyVal__top__DOT__vgc_inst__DOT__tile_inst__DOT__tile_data_ram__DOT__mem__v0 
            = vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__td_din_a;
        __VdlyDim0__top__DOT__vgc_inst__DOT__tile_inst__DOT__tile_data_ram__DOT__mem__v0 
            = vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__td_addr_a;
        __VdlySet__top__DOT__vgc_inst__DOT__tile_inst__DOT__tile_data_ram__DOT__mem__v0 = 1U;
    }
    if (vlSelfRef.top__DOT__vgc_inst__DOT__spr_a_we) {
        __VdlyVal__top__DOT__vgc_inst__DOT__sprite_inst__DOT__spr_mem__DOT__mem__v0 
            = vlSelfRef.top__DOT__vgc_inst__DOT__spr_a_din;
        __VdlyDim0__top__DOT__vgc_inst__DOT__sprite_inst__DOT__spr_mem__DOT__mem__v0 
            = vlSelfRef.top__DOT__vgc_inst__DOT__spr_a_addr;
        __VdlySet__top__DOT__vgc_inst__DOT__sprite_inst__DOT__spr_mem__DOT__mem__v0 = 1U;
    }
    if (vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__pr_we_a) {
        __VdlyVal__top__DOT__vgc_inst__DOT__tile_inst__DOT__pal_ram_inst__DOT__mem__v0 
            = vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__pr_din_a;
        __VdlyDim0__top__DOT__vgc_inst__DOT__tile_inst__DOT__pal_ram_inst__DOT__mem__v0 
            = vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__pr_addr_a;
        __VdlySet__top__DOT__vgc_inst__DOT__tile_inst__DOT__pal_ram_inst__DOT__mem__v0 = 1U;
    }
    if (vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__at_we_a) {
        vlSelfRef.__VdlyVal__top__DOT__vgc_inst__DOT__tile_inst__DOT__attr_table_ram__DOT__mem__v0 
            = vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__at_din_a;
        vlSelfRef.__VdlyDim0__top__DOT__vgc_inst__DOT__tile_inst__DOT__attr_table_ram__DOT__mem__v0 
            = vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__at_addr_a;
        vlSelfRef.__VdlySet__top__DOT__vgc_inst__DOT__tile_inst__DOT__attr_table_ram__DOT__mem__v0 = 1U;
    }
    if (vlSelfRef.top__DOT__vgc_inst__DOT__char_a_we) {
        vlSelfRef.top__DOT__vgc_inst__DOT__text_inst__DOT__char_mem__DOT____Vlvbound_hcad4ec09__0 
            = vlSelfRef.top__DOT__vgc_inst__DOT__char_a_din;
        if ((0x07cfU >= (IData)(vlSelfRef.top__DOT__vgc_inst__DOT__char_a_addr))) {
            __VdlyVal__top__DOT__vgc_inst__DOT__text_inst__DOT__char_mem__DOT__mem__v0 
                = vlSelfRef.top__DOT__vgc_inst__DOT__text_inst__DOT__char_mem__DOT____Vlvbound_hcad4ec09__0;
            __VdlyDim0__top__DOT__vgc_inst__DOT__text_inst__DOT__char_mem__DOT__mem__v0 
                = vlSelfRef.top__DOT__vgc_inst__DOT__char_a_addr;
            __VdlySet__top__DOT__vgc_inst__DOT__text_inst__DOT__char_mem__DOT__mem__v0 = 1U;
        }
    }
    if (vlSelfRef.top__DOT__vgc_inst__DOT__color_a_we) {
        vlSelfRef.top__DOT__vgc_inst__DOT__text_inst__DOT__color_mem__DOT____Vlvbound_hcad4ec09__0 
            = vlSelfRef.top__DOT__vgc_inst__DOT__color_a_din;
        if ((0x07cfU >= (IData)(vlSelfRef.top__DOT__vgc_inst__DOT__color_a_addr))) {
            __VdlyVal__top__DOT__vgc_inst__DOT__text_inst__DOT__color_mem__DOT__mem__v0 
                = vlSelfRef.top__DOT__vgc_inst__DOT__text_inst__DOT__color_mem__DOT____Vlvbound_hcad4ec09__0;
            __VdlyDim0__top__DOT__vgc_inst__DOT__text_inst__DOT__color_mem__DOT__mem__v0 
                = vlSelfRef.top__DOT__vgc_inst__DOT__color_a_addr;
            __VdlySet__top__DOT__vgc_inst__DOT__text_inst__DOT__color_mem__DOT__mem__v0 = 1U;
        }
    }
    if (vlSelfRef.top__DOT__vgc_inst__DOT__gfx_a_we) {
        vlSelfRef.top__DOT__vgc_inst__DOT__gfx_inst__DOT__gfx_mem__DOT____Vlvbound_h0a4ce44c__0 
            = vlSelfRef.top__DOT__vgc_inst__DOT__gfx_a_din;
        if ((0xf9ffU >= (IData)(vlSelfRef.top__DOT__vgc_inst__DOT__gfx_a_addr))) {
            __VdlyVal__top__DOT__vgc_inst__DOT__gfx_inst__DOT__gfx_mem__DOT__mem__v0 
                = vlSelfRef.top__DOT__vgc_inst__DOT__gfx_inst__DOT__gfx_mem__DOT____Vlvbound_h0a4ce44c__0;
            __VdlyDim0__top__DOT__vgc_inst__DOT__gfx_inst__DOT__gfx_mem__DOT__mem__v0 
                = vlSelfRef.top__DOT__vgc_inst__DOT__gfx_a_addr;
            __VdlySet__top__DOT__vgc_inst__DOT__gfx_inst__DOT__gfx_mem__DOT__mem__v0 = 1U;
        }
    }
    if (vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__nt_we_a) {
        __VdlyVal__top__DOT__vgc_inst__DOT__tile_inst__DOT__nametable_ram__DOT__mem__v0 
            = vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__nt_din_a;
        __VdlyDim0__top__DOT__vgc_inst__DOT__tile_inst__DOT__nametable_ram__DOT__mem__v0 
            = vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__nt_addr_a;
        __VdlySet__top__DOT__vgc_inst__DOT__tile_inst__DOT__nametable_ram__DOT__mem__v0 = 1U;
    }
    if (((0U == (IData)(vlSelfRef.top__DOT__vgc_inst__DOT__h_count)) 
         & (0U == (IData)(vlSelfRef.top__DOT__vgc_inst__DOT__v_count)))) {
        if ((0x0eU <= (IData)(vlSelfRef.top__DOT__vgc_inst__DOT__blink_count))) {
            __Vdly__top__DOT__vgc_inst__DOT__cursor_blink 
                = (1U & (~ (IData)(vlSelfRef.top__DOT__vgc_inst__DOT__cursor_blink)));
            __Vdly__top__DOT__vgc_inst__DOT__blink_count = 0U;
        } else {
            __Vdly__top__DOT__vgc_inst__DOT__blink_count 
                = (0x0000001fU & ((IData)(1U) + (IData)(vlSelfRef.top__DOT__vgc_inst__DOT__blink_count)));
        }
    }
    if (vlSelfRef.key_valid) {
        __VdlyVal__top__DOT__ram__v2 = vlSelfRef.key_data;
        __VdlySet__top__DOT__ram__v2 = 1U;
    }
    if (((IData)(vlSelfRef.dbg_poke_en) & (0xc000U 
                                           > (IData)(vlSelfRef.dbg_poke_addr)))) {
        __VdlyVal__top__DOT__ram__v4 = vlSelfRef.dbg_poke_data;
        __VdlyDim0__top__DOT__ram__v4 = vlSelfRef.dbg_poke_addr;
        __VdlySet__top__DOT__ram__v4 = 1U;
    }
    if (vlSelfRef.top__DOT__cpu_we) {
        if (((IData)(vlSelfRef.top__DOT__xmc_win_sel) 
             & (IData)(vlSelfRef.top__DOT__xmc_win_enabled))) {
            __VdlyVal__top__DOT__xram__v0 = vlSelfRef.top__DOT__cpu_dout;
            __VdlyDim0__top__DOT__xram__v0 = vlSelfRef.top__DOT__xmc_addr;
            __VdlySet__top__DOT__xram__v0 = 1U;
        }
        if ((1U & (~ ((IData)(vlSelfRef.top__DOT__xmc_win_sel) 
                      & (IData)(vlSelfRef.top__DOT__xmc_win_enabled))))) {
            if (vlSelfRef.top__DOT__xmc_reg_sel) {
                if ((0x0dU != (0x0000003fU & (IData)(vlSelfRef.top__DOT__cpu_addr)))) {
                    __VdlyVal__top__DOT__xmc_regs__v0 
                        = vlSelfRef.top__DOT__cpu_dout;
                    __VdlyDim0__top__DOT__xmc_regs__v0 
                        = (0x0000003fU & (IData)(vlSelfRef.top__DOT__cpu_addr));
                    __VdlySet__top__DOT__xmc_regs__v0 = 1U;
                }
            }
            if ((1U & (~ (IData)(vlSelfRef.top__DOT__xmc_reg_sel)))) {
                if ((0xc000U > (IData)(vlSelfRef.top__DOT__cpu_addr))) {
                    __VdlyVal__top__DOT__ram__v0 = vlSelfRef.top__DOT__cpu_dout;
                    __VdlyDim0__top__DOT__ram__v0 = vlSelfRef.top__DOT__cpu_addr;
                    __VdlySet__top__DOT__ram__v0 = 1U;
                }
            }
        }
    }
    if (vlSelfRef.top__DOT__blt_xram_we) {
        __VdlyVal__top__DOT__xram__v1 = vlSelfRef.top__DOT__blt_xram_wdata;
        __VdlyDim0__top__DOT__xram__v1 = vlSelfRef.top__DOT__blt_xram_addr;
        __VdlySet__top__DOT__xram__v1 = 1U;
    }
    if (((IData)(vlSelfRef.top__DOT__clear_input) & 
         (~ (IData)(vlSelfRef.key_valid)))) {
        __VdlySet__top__DOT__ram__v3 = 1U;
    }
    if (((IData)(vlSelfRef.top__DOT__blt_ram_we) & 
         (0xc000U > (IData)(vlSelfRef.top__DOT__blt_ram_addr)))) {
        __VdlyVal__top__DOT__ram__v1 = vlSelfRef.top__DOT__blt_ram_wdata;
        __VdlyDim0__top__DOT__ram__v1 = vlSelfRef.top__DOT__blt_ram_addr;
        __VdlySet__top__DOT__ram__v1 = 1U;
    }
    if (vlSelfRef.top__DOT__vgc_inst__DOT__sprite_inst__DOT__slb_a_we) {
        vlSelfRef.top__DOT__vgc_inst__DOT__sprite_inst__DOT__scanline_buf__DOT____Vlvbound_h7b43b104__0 
            = vlSelfRef.top__DOT__vgc_inst__DOT__sprite_inst__DOT__slb_a_din;
        if ((0x013fU >= (IData)(vlSelfRef.top__DOT__vgc_inst__DOT__sprite_inst__DOT__slb_a_addr))) {
            __VdlyVal__top__DOT__vgc_inst__DOT__sprite_inst__DOT__scanline_buf__DOT__mem__v0 
                = vlSelfRef.top__DOT__vgc_inst__DOT__sprite_inst__DOT__scanline_buf__DOT____Vlvbound_h7b43b104__0;
            __VdlyDim0__top__DOT__vgc_inst__DOT__sprite_inst__DOT__scanline_buf__DOT__mem__v0 
                = vlSelfRef.top__DOT__vgc_inst__DOT__sprite_inst__DOT__slb_a_addr;
            __VdlySet__top__DOT__vgc_inst__DOT__sprite_inst__DOT__scanline_buf__DOT__mem__v0 = 1U;
        }
    }
    if (((IData)(vlSelfRef.top__DOT__cpu_inst__DOT__NMI_edge) 
         & (0x0bU == (IData)(vlSelfRef.top__DOT__cpu_inst__DOT__state)))) {
        vlSelfRef.top__DOT__cpu_inst__DOT__NMI_edge = 0U;
    } else if ((1U & ((~ (IData)(vlSelfRef.nmi_n)) 
                      & (~ (IData)(vlSelfRef.top__DOT__cpu_inst__DOT__NMI_1))))) {
        vlSelfRef.top__DOT__cpu_inst__DOT__NMI_edge = 1U;
    }
    if ((1U & (~ (IData)(vlSelfRef.rst)))) {
        if (vlSelfRef.top__DOT__cpu_we) {
            if (((0xb9b0U <= (IData)(vlSelfRef.top__DOT__cpu_addr)) 
                 & (0xb9efU >= (IData)(vlSelfRef.top__DOT__cpu_addr)))) {
                __VdlyVal__top__DOT__vgc_inst__DOT__fio_name__v0 
                    = vlSelfRef.top__DOT__cpu_dout;
                __VdlyDim0__top__DOT__vgc_inst__DOT__fio_name__v0 
                    = (0x0000003fU & ((IData)(vlSelfRef.top__DOT__cpu_addr) 
                                      - (IData)(0x30U)));
                __VdlySet__top__DOT__vgc_inst__DOT__fio_name__v0 = 1U;
            }
        }
        if (((IData)(vlSelfRef.top__DOT__cpu_we) & (IData)(vlSelfRef.top__DOT__vgc_inst__DOT__tile_reg_sel))) {
            __VdlyVal__top__DOT__vgc_inst__DOT__tile_inst__DOT__tregs__v0 
                = vlSelfRef.top__DOT__cpu_dout;
            __VdlyDim0__top__DOT__vgc_inst__DOT__tile_inst__DOT__tregs__v0 
                = (0x0000001fU & (IData)(vlSelfRef.top__DOT__cpu_addr));
            __VdlySet__top__DOT__vgc_inst__DOT__tile_inst__DOT__tregs__v0 = 1U;
        }
    }
    if (vlSelfRef.rst) {
        __Vdly__top__DOT__vgc_inst__DOT__h_count = 0U;
        __Vdly__top__DOT__vgc_inst__DOT__v_count = 0U;
        __VdlySet__top__DOT__vgc_inst__DOT__spr_shape__v0 = 1U;
        __VdlySet__top__DOT__vgc_inst__DOT__spr_trans__v0 = 1U;
    } else {
        if ((0x031fU == (IData)(vlSelfRef.top__DOT__vgc_inst__DOT__h_count))) {
            __Vdly__top__DOT__vgc_inst__DOT__v_count 
                = ((0x020cU == (IData)(vlSelfRef.top__DOT__vgc_inst__DOT__v_count))
                    ? 0U : (0x000003ffU & ((IData)(1U) 
                                           + (IData)(vlSelfRef.top__DOT__vgc_inst__DOT__v_count))));
            __Vdly__top__DOT__vgc_inst__DOT__h_count = 0U;
        } else {
            __Vdly__top__DOT__vgc_inst__DOT__h_count 
                = (0x000003ffU & ((IData)(1U) + (IData)(vlSelfRef.top__DOT__vgc_inst__DOT__h_count)));
        }
        if (vlSelfRef.top__DOT__cpu_we) {
            if (vlSelfRef.top__DOT__vgc_inst__DOT__spr_reg_sel) {
                if ((4U & (IData)(vlSelfRef.top__DOT__cpu_addr))) {
                    if ((1U & (~ ((IData)(vlSelfRef.top__DOT__cpu_addr) 
                                  >> 1U)))) {
                        if ((1U & (~ (IData)(vlSelfRef.top__DOT__cpu_addr)))) {
                            __VdlyVal__top__DOT__vgc_inst__DOT__spr_shape__v16 
                                = (0x0000000fU & (IData)(vlSelfRef.top__DOT__cpu_dout));
                            __VdlyDim0__top__DOT__vgc_inst__DOT__spr_shape__v16 
                                = (0x0000000fU & ((IData)(vlSelfRef.top__DOT__cpu_addr) 
                                                  >> 3U));
                            __VdlySet__top__DOT__vgc_inst__DOT__spr_shape__v16 = 1U;
                        }
                    }
                    if ((2U & (IData)(vlSelfRef.top__DOT__cpu_addr))) {
                        if ((1U & (IData)(vlSelfRef.top__DOT__cpu_addr))) {
                            __VdlyVal__top__DOT__vgc_inst__DOT__spr_trans__v16 
                                = (0x0000000fU & (IData)(vlSelfRef.top__DOT__cpu_dout));
                            __VdlyDim0__top__DOT__vgc_inst__DOT__spr_trans__v16 
                                = (0x0000000fU & ((IData)(vlSelfRef.top__DOT__cpu_addr) 
                                                  >> 3U));
                            __VdlySet__top__DOT__vgc_inst__DOT__spr_trans__v16 = 1U;
                        }
                    }
                }
            }
        }
    }
    if (vlSelfRef.rst) {
        __Vdly__top__DOT__blt_inst__DOT__state = 0U;
        __VdlySet__top__DOT__blt_inst__DOT__regs__v0 = 1U;
        __Vdly__top__DOT__blt_inst__DOT__read_valid = 0U;
        __VdlySet__top__DOT__blt_inst__DOT__regs__v1 = 1U;
    } else {
        if ((8U & (IData)(vlSelfRef.top__DOT__blt_inst__DOT__state))) {
            __Vdly__top__DOT__blt_inst__DOT__state = 0U;
        } else if ((4U & (IData)(vlSelfRef.top__DOT__blt_inst__DOT__state))) {
            if ((2U & (IData)(vlSelfRef.top__DOT__blt_inst__DOT__state))) {
                __Vdly__top__DOT__blt_inst__DOT__state = 0U;
            } else if ((1U & (IData)(vlSelfRef.top__DOT__blt_inst__DOT__state))) {
                if ((1U & (~ ((IData)(vlSelfRef.top__DOT__blt_inst__DOT__colorkey_mode) 
                              & (vlSelfRef.top__DOT__blt_inst__DOT__row_buf
                                 [vlSelfRef.top__DOT__blt_inst__DOT__buf_idx] 
                                 == (IData)(vlSelfRef.top__DOT__blt_inst__DOT__color_key)))))) {
                    __Vdly__top__DOT__blt_inst__DOT__wrote_count 
                        = (0x00ffffffU & ((IData)(1U) 
                                          + vlSelfRef.top__DOT__blt_inst__DOT__wrote_count));
                }
                if ((((IData)(1U) + (IData)(vlSelfRef.top__DOT__blt_inst__DOT__col)) 
                     >= (IData)(vlSelfRef.top__DOT__blt_inst__DOT__width))) {
                    __Vdly__top__DOT__blt_inst__DOT__col = 0U;
                    if ((((IData)(1U) + (IData)(vlSelfRef.top__DOT__blt_inst__DOT__row)) 
                         >= (IData)(vlSelfRef.top__DOT__blt_inst__DOT__height))) {
                        __VdlySet__top__DOT__blt_inst__DOT__regs__v2 = 1U;
                        __Vdly__top__DOT__blt_inst__DOT__state = 6U;
                        __VdlySet__top__DOT__blt_inst__DOT__regs__v3 = 1U;
                        __VdlyVal__top__DOT__blt_inst__DOT__regs__v4 
                            = (0x000000ffU & ((vlSelfRef.top__DOT__blt_inst__DOT__wrote_count 
                                               + (((IData)(vlSelfRef.top__DOT__blt_inst__DOT__colorkey_mode) 
                                                   & (vlSelfRef.top__DOT__blt_inst__DOT__row_buf
                                                      [vlSelfRef.top__DOT__blt_inst__DOT__buf_idx] 
                                                      == (IData)(vlSelfRef.top__DOT__blt_inst__DOT__color_key)))
                                                   ? 0U
                                                   : 1U)) 
                                              >> 0x00000010U));
                        __VdlyVal__top__DOT__blt_inst__DOT__regs__v5 
                            = (0x000000ffU & ((vlSelfRef.top__DOT__blt_inst__DOT__wrote_count 
                                               + (((IData)(vlSelfRef.top__DOT__blt_inst__DOT__colorkey_mode) 
                                                   & (vlSelfRef.top__DOT__blt_inst__DOT__row_buf
                                                      [vlSelfRef.top__DOT__blt_inst__DOT__buf_idx] 
                                                      == (IData)(vlSelfRef.top__DOT__blt_inst__DOT__color_key)))
                                                   ? 0U
                                                   : 1U)) 
                                              >> 8U));
                        __VdlyVal__top__DOT__blt_inst__DOT__regs__v6 
                            = (0x000000ffU & (vlSelfRef.top__DOT__blt_inst__DOT__wrote_count 
                                              + (((IData)(vlSelfRef.top__DOT__blt_inst__DOT__colorkey_mode) 
                                                  & (vlSelfRef.top__DOT__blt_inst__DOT__row_buf
                                                     [vlSelfRef.top__DOT__blt_inst__DOT__buf_idx] 
                                                     == (IData)(vlSelfRef.top__DOT__blt_inst__DOT__color_key)))
                                                  ? 0U
                                                  : 1U)));
                    } else {
                        __Vdly__top__DOT__blt_inst__DOT__row 
                            = (0x0000ffffU & ((IData)(1U) 
                                              + (IData)(vlSelfRef.top__DOT__blt_inst__DOT__row)));
                        __Vdly__top__DOT__blt_inst__DOT__state = 4U;
                    }
                    __Vdly__top__DOT__blt_inst__DOT__buf_idx = 0U;
                } else {
                    __Vdly__top__DOT__blt_inst__DOT__col 
                        = (0x0000ffffU & ((IData)(1U) 
                                          + (IData)(vlSelfRef.top__DOT__blt_inst__DOT__col)));
                    __Vdly__top__DOT__blt_inst__DOT__buf_idx 
                        = (0x000003ffU & ((IData)(1U) 
                                          + (IData)(vlSelfRef.top__DOT__blt_inst__DOT__buf_idx)));
                }
            } else {
                __VdlyVal__top__DOT__blt_inst__DOT__row_buf__v0 
                    = vlSelfRef.top__DOT__blt_inst__DOT__mem_read_data;
                __VdlyDim0__top__DOT__blt_inst__DOT__row_buf__v0 
                    = vlSelfRef.top__DOT__blt_inst__DOT__buf_idx;
                __VdlySet__top__DOT__blt_inst__DOT__row_buf__v0 = 1U;
                if ((((IData)(1U) + (IData)(vlSelfRef.top__DOT__blt_inst__DOT__col)) 
                     >= (IData)(vlSelfRef.top__DOT__blt_inst__DOT__width))) {
                    __Vdly__top__DOT__blt_inst__DOT__col = 0U;
                    __Vdly__top__DOT__blt_inst__DOT__buf_idx = 0U;
                    __Vdly__top__DOT__blt_inst__DOT__state = 5U;
                } else {
                    __Vdly__top__DOT__blt_inst__DOT__col 
                        = (0x0000ffffU & ((IData)(1U) 
                                          + (IData)(vlSelfRef.top__DOT__blt_inst__DOT__col)));
                    __Vdly__top__DOT__blt_inst__DOT__buf_idx 
                        = (0x000003ffU & ((IData)(1U) 
                                          + (IData)(vlSelfRef.top__DOT__blt_inst__DOT__buf_idx)));
                }
            }
        } else if ((2U & (IData)(vlSelfRef.top__DOT__blt_inst__DOT__state))) {
            if ((1U & (IData)(vlSelfRef.top__DOT__blt_inst__DOT__state))) {
                if (vlSelfRef.top__DOT__blt_inst__DOT__read_valid) {
                    if ((1U & (~ ((IData)(vlSelfRef.top__DOT__blt_inst__DOT__colorkey_mode) 
                                  & ((IData)(vlSelfRef.top__DOT__blt_inst__DOT__read_byte) 
                                     == (IData)(vlSelfRef.top__DOT__blt_inst__DOT__color_key)))))) {
                        __Vdly__top__DOT__blt_inst__DOT__wrote_count 
                            = (0x00ffffffU & ((IData)(1U) 
                                              + vlSelfRef.top__DOT__blt_inst__DOT__wrote_count));
                    }
                    if ((((IData)(1U) + (IData)(vlSelfRef.top__DOT__blt_inst__DOT__col)) 
                         >= (IData)(vlSelfRef.top__DOT__blt_inst__DOT__width))) {
                        __Vdly__top__DOT__blt_inst__DOT__col = 0U;
                        if ((((IData)(1U) + (IData)(vlSelfRef.top__DOT__blt_inst__DOT__row)) 
                             >= (IData)(vlSelfRef.top__DOT__blt_inst__DOT__height))) {
                            __VdlySet__top__DOT__blt_inst__DOT__regs__v7 = 1U;
                            __Vdly__top__DOT__blt_inst__DOT__state = 6U;
                            __VdlySet__top__DOT__blt_inst__DOT__regs__v8 = 1U;
                            __VdlyVal__top__DOT__blt_inst__DOT__regs__v9 
                                = (0x000000ffU & ((vlSelfRef.top__DOT__blt_inst__DOT__wrote_count 
                                                   + 
                                                   (((IData)(vlSelfRef.top__DOT__blt_inst__DOT__colorkey_mode) 
                                                     & ((IData)(vlSelfRef.top__DOT__blt_inst__DOT__read_byte) 
                                                        == (IData)(vlSelfRef.top__DOT__blt_inst__DOT__color_key)))
                                                     ? 0U
                                                     : 1U)) 
                                                  >> 0x00000010U));
                            __VdlyVal__top__DOT__blt_inst__DOT__regs__v10 
                                = (0x000000ffU & ((vlSelfRef.top__DOT__blt_inst__DOT__wrote_count 
                                                   + 
                                                   (((IData)(vlSelfRef.top__DOT__blt_inst__DOT__colorkey_mode) 
                                                     & ((IData)(vlSelfRef.top__DOT__blt_inst__DOT__read_byte) 
                                                        == (IData)(vlSelfRef.top__DOT__blt_inst__DOT__color_key)))
                                                     ? 0U
                                                     : 1U)) 
                                                  >> 8U));
                            __VdlyVal__top__DOT__blt_inst__DOT__regs__v11 
                                = (0x000000ffU & (vlSelfRef.top__DOT__blt_inst__DOT__wrote_count 
                                                  + 
                                                  (((IData)(vlSelfRef.top__DOT__blt_inst__DOT__colorkey_mode) 
                                                    & ((IData)(vlSelfRef.top__DOT__blt_inst__DOT__read_byte) 
                                                       == (IData)(vlSelfRef.top__DOT__blt_inst__DOT__color_key)))
                                                    ? 0U
                                                    : 1U)));
                        } else {
                            __Vdly__top__DOT__blt_inst__DOT__row 
                                = (0x0000ffffU & ((IData)(1U) 
                                                  + (IData)(vlSelfRef.top__DOT__blt_inst__DOT__row)));
                            if (vlSelfRef.top__DOT__blt_inst__DOT__fill_mode) {
                                __Vdly__top__DOT__blt_inst__DOT__state = 3U;
                                vlSelfRef.top__DOT__blt_inst__DOT__read_byte 
                                    = vlSelfRef.top__DOT__blt_inst__DOT__fill_value;
                            } else {
                                __Vdly__top__DOT__blt_inst__DOT__state = 2U;
                            }
                        }
                    } else {
                        __Vdly__top__DOT__blt_inst__DOT__col 
                            = (0x0000ffffU & ((IData)(1U) 
                                              + (IData)(vlSelfRef.top__DOT__blt_inst__DOT__col)));
                        if (vlSelfRef.top__DOT__blt_inst__DOT__fill_mode) {
                            vlSelfRef.top__DOT__blt_inst__DOT__read_byte 
                                = vlSelfRef.top__DOT__blt_inst__DOT__fill_value;
                            __Vdly__top__DOT__blt_inst__DOT__state = 3U;
                        } else {
                            __Vdly__top__DOT__blt_inst__DOT__state = 2U;
                        }
                    }
                }
            } else {
                vlSelfRef.top__DOT__blt_inst__DOT__read_byte 
                    = vlSelfRef.top__DOT__blt_inst__DOT__mem_read_data;
                __Vdly__top__DOT__blt_inst__DOT__read_valid = 1U;
                __Vdly__top__DOT__blt_inst__DOT__state = 3U;
            }
        } else if ((1U & (IData)(vlSelfRef.top__DOT__blt_inst__DOT__state))) {
            if (((0U == (IData)(vlSelfRef.top__DOT__blt_inst__DOT__width)) 
                 | (0U == (IData)(vlSelfRef.top__DOT__blt_inst__DOT__height)))) {
                __VdlySet__top__DOT__blt_inst__DOT__regs__v12 = 1U;
                __Vdly__top__DOT__blt_inst__DOT__state = 6U;
                __VdlySet__top__DOT__blt_inst__DOT__regs__v13 = 1U;
            } else if (((~ (IData)(vlSelfRef.top__DOT__blt_inst__DOT__fill_mode)) 
                        & (6U < (IData)(vlSelfRef.top__DOT__blt_inst__DOT__src_space)))) {
                __VdlySet__top__DOT__blt_inst__DOT__regs__v14 = 1U;
                __Vdly__top__DOT__blt_inst__DOT__state = 6U;
                __VdlySet__top__DOT__blt_inst__DOT__regs__v15 = 1U;
            } else if ((6U < (IData)(vlSelfRef.top__DOT__blt_inst__DOT__dst_space))) {
                __VdlySet__top__DOT__blt_inst__DOT__regs__v16 = 1U;
                __Vdly__top__DOT__blt_inst__DOT__state = 6U;
                __VdlySet__top__DOT__blt_inst__DOT__regs__v17 = 1U;
            } else if ((1U & ((~ (IData)(vlSelfRef.top__DOT__blt_inst__DOT__fill_mode)) 
                              & (~ ([&]() {
                                    __Vfunc_top__DOT__blt_inst__DOT__space_size__1__sp 
                                        = vlSelfRef.top__DOT__blt_inst__DOT__src_space;
                                    __Vfunc_top__DOT__blt_inst__DOT__space_size__1__Vfuncout 
                                        = ((4U & (IData)(__Vfunc_top__DOT__blt_inst__DOT__space_size__1__sp))
                                            ? ((2U 
                                                & (IData)(__Vfunc_top__DOT__blt_inst__DOT__space_size__1__sp))
                                                ? (
                                                   (1U 
                                                    & (IData)(__Vfunc_top__DOT__blt_inst__DOT__space_size__1__sp))
                                                    ? 0U
                                                    : 0x00008000U)
                                                : (
                                                   (1U 
                                                    & (IData)(__Vfunc_top__DOT__blt_inst__DOT__space_size__1__sp))
                                                    ? 0x00080000U
                                                    : 0x00008000U))
                                            : ((2U 
                                                & (IData)(__Vfunc_top__DOT__blt_inst__DOT__space_size__1__sp))
                                                ? (
                                                   (1U 
                                                    & (IData)(__Vfunc_top__DOT__blt_inst__DOT__space_size__1__sp))
                                                    ? 0x0000fa00U
                                                    : 0x000007d0U)
                                                : (
                                                   (1U 
                                                    & (IData)(__Vfunc_top__DOT__blt_inst__DOT__space_size__1__sp))
                                                    ? 0x000007d0U
                                                    : 0x00010000U)));
                                    __Vfunc_top__DOT__blt_inst__DOT__rect_fits__0__sz 
                                        = __Vfunc_top__DOT__blt_inst__DOT__space_size__1__Vfuncout;
                                    __Vfunc_top__DOT__blt_inst__DOT__rect_fits__0__stride 
                                        = vlSelfRef.top__DOT__blt_inst__DOT__src_stride;
                                    __Vfunc_top__DOT__blt_inst__DOT__rect_fits__0__h 
                                        = vlSelfRef.top__DOT__blt_inst__DOT__height;
                                    __Vfunc_top__DOT__blt_inst__DOT__rect_fits__0__w 
                                        = vlSelfRef.top__DOT__blt_inst__DOT__width;
                                    __Vfunc_top__DOT__blt_inst__DOT__rect_fits__0__base 
                                        = vlSelfRef.top__DOT__blt_inst__DOT__src_base;
                                    __Vfunc_top__DOT__blt_inst__DOT__rect_fits__0__last_row_start = 0;
                                    __Vfunc_top__DOT__blt_inst__DOT__rect_fits__0__last_byte = 0;
                                    if ((((0U == (IData)(__Vfunc_top__DOT__blt_inst__DOT__rect_fits__0__w)) 
                                          | (0U == (IData)(__Vfunc_top__DOT__blt_inst__DOT__rect_fits__0__h))) 
                                         | (0U == __Vfunc_top__DOT__blt_inst__DOT__rect_fits__0__sz))) {
                                        __Vfunc_top__DOT__blt_inst__DOT__rect_fits__0__Vfuncout = 0U;
                                    } else {
                                        __Vfunc_top__DOT__blt_inst__DOT__rect_fits__0__last_row_start 
                                            = (__Vfunc_top__DOT__blt_inst__DOT__rect_fits__0__base 
                                               + (((IData)(__Vfunc_top__DOT__blt_inst__DOT__rect_fits__0__h) 
                                                   - (IData)(1U)) 
                                                  * (IData)(__Vfunc_top__DOT__blt_inst__DOT__rect_fits__0__stride)));
                                        __Vfunc_top__DOT__blt_inst__DOT__rect_fits__0__last_byte 
                                            = (__Vfunc_top__DOT__blt_inst__DOT__rect_fits__0__last_row_start 
                                               + (IData)(__Vfunc_top__DOT__blt_inst__DOT__rect_fits__0__w));
                                        __Vfunc_top__DOT__blt_inst__DOT__rect_fits__0__Vfuncout 
                                            = ((__Vfunc_top__DOT__blt_inst__DOT__rect_fits__0__last_byte 
                                                <= __Vfunc_top__DOT__blt_inst__DOT__rect_fits__0__sz) 
                                               & (__Vfunc_top__DOT__blt_inst__DOT__rect_fits__0__base 
                                                  < __Vfunc_top__DOT__blt_inst__DOT__rect_fits__0__sz));
                                    }
                                }(), (IData)(__Vfunc_top__DOT__blt_inst__DOT__rect_fits__0__Vfuncout)))))) {
                __VdlySet__top__DOT__blt_inst__DOT__regs__v18 = 1U;
                __Vdly__top__DOT__blt_inst__DOT__state = 6U;
                __VdlySet__top__DOT__blt_inst__DOT__regs__v19 = 1U;
            } else if (([&]() {
                        __Vfunc_top__DOT__blt_inst__DOT__space_size__3__sp 
                            = vlSelfRef.top__DOT__blt_inst__DOT__dst_space;
                        __Vfunc_top__DOT__blt_inst__DOT__space_size__3__Vfuncout 
                            = ((4U & (IData)(__Vfunc_top__DOT__blt_inst__DOT__space_size__3__sp))
                                ? ((2U & (IData)(__Vfunc_top__DOT__blt_inst__DOT__space_size__3__sp))
                                    ? ((1U & (IData)(__Vfunc_top__DOT__blt_inst__DOT__space_size__3__sp))
                                        ? 0U : 0x00008000U)
                                    : ((1U & (IData)(__Vfunc_top__DOT__blt_inst__DOT__space_size__3__sp))
                                        ? 0x00080000U
                                        : 0x00008000U))
                                : ((2U & (IData)(__Vfunc_top__DOT__blt_inst__DOT__space_size__3__sp))
                                    ? ((1U & (IData)(__Vfunc_top__DOT__blt_inst__DOT__space_size__3__sp))
                                        ? 0x0000fa00U
                                        : 0x000007d0U)
                                    : ((1U & (IData)(__Vfunc_top__DOT__blt_inst__DOT__space_size__3__sp))
                                        ? 0x000007d0U
                                        : 0x00010000U)));
                        __Vfunc_top__DOT__blt_inst__DOT__rect_fits__2__sz 
                            = __Vfunc_top__DOT__blt_inst__DOT__space_size__3__Vfuncout;
                        __Vfunc_top__DOT__blt_inst__DOT__rect_fits__2__stride 
                            = vlSelfRef.top__DOT__blt_inst__DOT__dst_stride;
                        __Vfunc_top__DOT__blt_inst__DOT__rect_fits__2__h 
                            = vlSelfRef.top__DOT__blt_inst__DOT__height;
                        __Vfunc_top__DOT__blt_inst__DOT__rect_fits__2__w 
                            = vlSelfRef.top__DOT__blt_inst__DOT__width;
                        __Vfunc_top__DOT__blt_inst__DOT__rect_fits__2__base 
                            = vlSelfRef.top__DOT__blt_inst__DOT__dst_base;
                        __Vfunc_top__DOT__blt_inst__DOT__rect_fits__2__last_row_start = 0;
                        __Vfunc_top__DOT__blt_inst__DOT__rect_fits__2__last_byte = 0;
                        if ((((0U == (IData)(__Vfunc_top__DOT__blt_inst__DOT__rect_fits__2__w)) 
                              | (0U == (IData)(__Vfunc_top__DOT__blt_inst__DOT__rect_fits__2__h))) 
                             | (0U == __Vfunc_top__DOT__blt_inst__DOT__rect_fits__2__sz))) {
                            __Vfunc_top__DOT__blt_inst__DOT__rect_fits__2__Vfuncout = 0U;
                        } else {
                            __Vfunc_top__DOT__blt_inst__DOT__rect_fits__2__last_row_start 
                                = (__Vfunc_top__DOT__blt_inst__DOT__rect_fits__2__base 
                                   + (((IData)(__Vfunc_top__DOT__blt_inst__DOT__rect_fits__2__h) 
                                       - (IData)(1U)) 
                                      * (IData)(__Vfunc_top__DOT__blt_inst__DOT__rect_fits__2__stride)));
                            __Vfunc_top__DOT__blt_inst__DOT__rect_fits__2__last_byte 
                                = (__Vfunc_top__DOT__blt_inst__DOT__rect_fits__2__last_row_start 
                                   + (IData)(__Vfunc_top__DOT__blt_inst__DOT__rect_fits__2__w));
                            __Vfunc_top__DOT__blt_inst__DOT__rect_fits__2__Vfuncout 
                                = ((__Vfunc_top__DOT__blt_inst__DOT__rect_fits__2__last_byte 
                                    <= __Vfunc_top__DOT__blt_inst__DOT__rect_fits__2__sz) 
                                   & (__Vfunc_top__DOT__blt_inst__DOT__rect_fits__2__base 
                                      < __Vfunc_top__DOT__blt_inst__DOT__rect_fits__2__sz));
                        }
                    }(), (IData)(__Vfunc_top__DOT__blt_inst__DOT__rect_fits__2__Vfuncout))) {
                if (([&]() {
                            __Vfunc_top__DOT__blt_inst__DOT__write_protect_ok__4__stride 
                                = vlSelfRef.top__DOT__blt_inst__DOT__dst_stride;
                            __Vfunc_top__DOT__blt_inst__DOT__write_protect_ok__4__h 
                                = vlSelfRef.top__DOT__blt_inst__DOT__height;
                            __Vfunc_top__DOT__blt_inst__DOT__write_protect_ok__4__w 
                                = vlSelfRef.top__DOT__blt_inst__DOT__width;
                            __Vfunc_top__DOT__blt_inst__DOT__write_protect_ok__4__base 
                                = vlSelfRef.top__DOT__blt_inst__DOT__dst_base;
                            __Vfunc_top__DOT__blt_inst__DOT__write_protect_ok__4__space 
                                = vlSelfRef.top__DOT__blt_inst__DOT__dst_space;
                            __Vfunc_top__DOT__blt_inst__DOT__write_protect_ok__4__last_byte = 0;
                            if ((0U != (IData)(__Vfunc_top__DOT__blt_inst__DOT__write_protect_ok__4__space))) {
                                __Vfunc_top__DOT__blt_inst__DOT__write_protect_ok__4__Vfuncout = 1U;
                            } else {
                                __Vfunc_top__DOT__blt_inst__DOT__write_protect_ok__4__last_byte 
                                    = ((__Vfunc_top__DOT__blt_inst__DOT__write_protect_ok__4__base 
                                        + (((IData)(__Vfunc_top__DOT__blt_inst__DOT__write_protect_ok__4__h) 
                                            - (IData)(1U)) 
                                           * (IData)(__Vfunc_top__DOT__blt_inst__DOT__write_protect_ok__4__stride))) 
                                       + (IData)(__Vfunc_top__DOT__blt_inst__DOT__write_protect_ok__4__w));
                                __Vfunc_top__DOT__blt_inst__DOT__write_protect_ok__4__Vfuncout 
                                    = (0x0000c000U 
                                       >= __Vfunc_top__DOT__blt_inst__DOT__write_protect_ok__4__last_byte);
                            }
                        }(), (IData)(__Vfunc_top__DOT__blt_inst__DOT__write_protect_ok__4__Vfuncout))) {
                    if (vlSelfRef.top__DOT__blt_inst__DOT__fill_mode) {
                        vlSelfRef.top__DOT__blt_inst__DOT__read_byte 
                            = vlSelfRef.top__DOT__blt_inst__DOT__fill_value;
                        __Vdly__top__DOT__blt_inst__DOT__state = 3U;
                        __Vdly__top__DOT__blt_inst__DOT__read_valid = 1U;
                    } else if (vlSelfRef.top__DOT__blt_inst__DOT__use_row_buffer) {
                        __Vdly__top__DOT__blt_inst__DOT__state = 4U;
                        __Vdly__top__DOT__blt_inst__DOT__col = 0U;
                        __Vdly__top__DOT__blt_inst__DOT__buf_idx = 0U;
                    } else {
                        __Vdly__top__DOT__blt_inst__DOT__state = 2U;
                        __Vdly__top__DOT__blt_inst__DOT__read_valid = 0U;
                    }
                } else {
                    __VdlySet__top__DOT__blt_inst__DOT__regs__v20 = 1U;
                    __Vdly__top__DOT__blt_inst__DOT__state = 6U;
                    __VdlySet__top__DOT__blt_inst__DOT__regs__v21 = 1U;
                }
            } else {
                __VdlySet__top__DOT__blt_inst__DOT__regs__v22 = 1U;
                __Vdly__top__DOT__blt_inst__DOT__state = 6U;
                __VdlySet__top__DOT__blt_inst__DOT__regs__v23 = 1U;
            }
        }
        if (((IData)(vlSelfRef.top__DOT__cpu_we) & (IData)(vlSelfRef.top__DOT__blt_reg_sel))) {
            if ((((((1U != (0x0000001fU & ((IData)(vlSelfRef.top__DOT__cpu_addr) 
                                           - (IData)(3U)))) 
                    & (2U != (0x0000001fU & ((IData)(vlSelfRef.top__DOT__cpu_addr) 
                                             - (IData)(3U))))) 
                   & (0x16U != (0x0000001fU & ((IData)(vlSelfRef.top__DOT__cpu_addr) 
                                               - (IData)(3U))))) 
                  & (0x17U != (0x0000001fU & ((IData)(vlSelfRef.top__DOT__cpu_addr) 
                                              - (IData)(3U))))) 
                 & (0x18U != (0x0000001fU & ((IData)(vlSelfRef.top__DOT__cpu_addr) 
                                             - (IData)(3U)))))) {
                vlSelfRef.top__DOT__blt_inst__DOT____Vlvbound_hf31f73b8__0 
                    = vlSelfRef.top__DOT__cpu_dout;
                if (VL_LIKELY(((0x18U >= (0x0000001fU 
                                          & ((IData)(vlSelfRef.top__DOT__cpu_addr) 
                                             - (IData)(3U))))))) {
                    __VdlyVal__top__DOT__blt_inst__DOT__regs__v24 
                        = vlSelfRef.top__DOT__blt_inst__DOT____Vlvbound_hf31f73b8__0;
                    __VdlyDim0__top__DOT__blt_inst__DOT__regs__v24 
                        = (0x0000001fU & ((IData)(vlSelfRef.top__DOT__cpu_addr) 
                                          - (IData)(3U)));
                    __VdlySet__top__DOT__blt_inst__DOT__regs__v24 = 1U;
                }
            }
            if ((0U == (0x0000001fU & ((IData)(vlSelfRef.top__DOT__cpu_addr) 
                                       - (IData)(3U))))) {
                if ((1U == (IData)(vlSelfRef.top__DOT__cpu_dout))) {
                    if ((0U == (IData)(vlSelfRef.top__DOT__blt_inst__DOT__state))) {
                        __Vdly__top__DOT__blt_inst__DOT__row = 0U;
                        vlSelfRef.top__DOT__blt_inst__DOT__src_space 
                            = (7U & vlSelfRef.top__DOT__blt_inst__DOT__regs[3U]);
                        __Vdly__top__DOT__blt_inst__DOT__col = 0U;
                        __Vdly__top__DOT__blt_inst__DOT__wrote_count = 0U;
                        __Vdly__top__DOT__blt_inst__DOT__read_valid = 0U;
                        __Vdly__top__DOT__blt_inst__DOT__buf_idx = 0U;
                        __Vdly__top__DOT__blt_inst__DOT__state = 1U;
                        vlSelfRef.top__DOT__blt_inst__DOT__dst_space 
                            = (7U & vlSelfRef.top__DOT__blt_inst__DOT__regs[4U]);
                        vlSelfRef.top__DOT__blt_inst__DOT__src_base 
                            = ((vlSelfRef.top__DOT__blt_inst__DOT__regs[7U] 
                                << 0x00000010U) | (
                                                   (vlSelfRef.top__DOT__blt_inst__DOT__regs[6U] 
                                                    << 8U) 
                                                   | vlSelfRef.top__DOT__blt_inst__DOT__regs[5U]));
                        vlSelfRef.top__DOT__blt_inst__DOT__dst_base 
                            = ((vlSelfRef.top__DOT__blt_inst__DOT__regs[10U] 
                                << 0x00000010U) | (
                                                   (vlSelfRef.top__DOT__blt_inst__DOT__regs[9U] 
                                                    << 8U) 
                                                   | vlSelfRef.top__DOT__blt_inst__DOT__regs[8U]));
                        vlSelfRef.top__DOT__blt_inst__DOT__width 
                            = ((vlSelfRef.top__DOT__blt_inst__DOT__regs[12U] 
                                << 8U) | vlSelfRef.top__DOT__blt_inst__DOT__regs[11U]);
                        vlSelfRef.top__DOT__blt_inst__DOT__height 
                            = ((vlSelfRef.top__DOT__blt_inst__DOT__regs[14U] 
                                << 8U) | vlSelfRef.top__DOT__blt_inst__DOT__regs[13U]);
                        vlSelfRef.top__DOT__blt_inst__DOT__src_stride 
                            = ((vlSelfRef.top__DOT__blt_inst__DOT__regs[16U] 
                                << 8U) | vlSelfRef.top__DOT__blt_inst__DOT__regs[15U]);
                        vlSelfRef.top__DOT__blt_inst__DOT__dst_stride 
                            = ((vlSelfRef.top__DOT__blt_inst__DOT__regs[18U] 
                                << 8U) | vlSelfRef.top__DOT__blt_inst__DOT__regs[17U]);
                        vlSelfRef.top__DOT__blt_inst__DOT__fill_mode 
                            = (1U & vlSelfRef.top__DOT__blt_inst__DOT__regs[19U]);
                        vlSelfRef.top__DOT__blt_inst__DOT__colorkey_mode 
                            = (1U & (vlSelfRef.top__DOT__blt_inst__DOT__regs[19U] 
                                     >> 1U));
                        vlSelfRef.top__DOT__blt_inst__DOT__fill_value 
                            = vlSelfRef.top__DOT__blt_inst__DOT__regs[20U];
                        vlSelfRef.top__DOT__blt_inst__DOT__color_key 
                            = vlSelfRef.top__DOT__blt_inst__DOT__regs[21U];
                        vlSelfRef.top__DOT__blt_inst__DOT__use_row_buffer 
                            = ((~ vlSelfRef.top__DOT__blt_inst__DOT__regs[19U]) 
                               & ((7U & vlSelfRef.top__DOT__blt_inst__DOT__regs[3U]) 
                                  == (7U & vlSelfRef.top__DOT__blt_inst__DOT__regs[4U])));
                        __VdlySet__top__DOT__blt_inst__DOT__regs__v25 = 1U;
                    } else {
                        __VdlySet__top__DOT__blt_inst__DOT__regs__v30 = 1U;
                    }
                } else {
                    __VdlySet__top__DOT__blt_inst__DOT__regs__v32 = 1U;
                }
            }
        }
    }
    if (vlSelfRef.top__DOT__vgc_inst__DOT__artist_inst__DOT__fs_a_we) {
        __VdlyVal__top__DOT__vgc_inst__DOT__artist_inst__DOT__fill_stack_mem__DOT__mem__v0 
            = vlSelfRef.top__DOT__vgc_inst__DOT__artist_inst__DOT__fs_a_din;
        __VdlyDim0__top__DOT__vgc_inst__DOT__artist_inst__DOT__fill_stack_mem__DOT__mem__v0 
            = vlSelfRef.top__DOT__vgc_inst__DOT__artist_inst__DOT__fs_a_addr;
        __VdlySet__top__DOT__vgc_inst__DOT__artist_inst__DOT__fill_stack_mem__DOT__mem__v0 = 1U;
    }
    if (((IData)(vlSelfRef.top__DOT__cpu_inst__DOT__write_register) 
         & (IData)(vlSelfRef.top__DOT____Vcellinp__cpu_inst__RDY))) {
        __VdlyVal__top__DOT__cpu_inst__DOT__AXYS__v0 
            = ((0x1aU == (IData)(vlSelfRef.top__DOT__cpu_inst__DOT__state))
                ? (IData)(vlSelfRef.top__DOT__cpu_inst__DOT__DIMUX)
                : ((0x000000f0U & ((((IData)(vlSelfRef.top__DOT__cpu_inst__DOT__ADD) 
                                     >> 4U) + ((IData)(vlSelfRef.top__DOT__cpu_inst__DOT__adj_bcd)
                                                ? ((IData)(vlSelfRef.top__DOT__cpu_inst__DOT__adc_bcd)
                                                    ? 
                                                   ((IData)(vlSelfRef.top__DOT__cpu_inst__DOT__CO)
                                                     ? 6U
                                                     : 0U)
                                                    : 
                                                   ((IData)(vlSelfRef.top__DOT__cpu_inst__DOT__CO)
                                                     ? 0U
                                                     : 0x0aU))
                                                : 0U)) 
                                   << 4U)) | (0x0000000fU 
                                              & ((IData)(vlSelfRef.top__DOT__cpu_inst__DOT__ADD) 
                                                 + 
                                                 ((IData)(vlSelfRef.top__DOT__cpu_inst__DOT__adj_bcd)
                                                   ? 
                                                  ((IData)(vlSelfRef.top__DOT__cpu_inst__DOT__adc_bcd)
                                                    ? 
                                                   ((IData)(vlSelfRef.top__DOT__cpu_inst__DOT__HC)
                                                     ? 6U
                                                     : 0U)
                                                    : 
                                                   ((IData)(vlSelfRef.top__DOT__cpu_inst__DOT__HC)
                                                     ? 0U
                                                     : 0x0aU))
                                                   : 0U)))));
        __VdlyDim0__top__DOT__cpu_inst__DOT__AXYS__v0 
            = vlSelfRef.top__DOT__cpu_inst__DOT__regsel;
        __VdlySet__top__DOT__cpu_inst__DOT__AXYS__v0 = 1U;
    }
    vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__s2_sample_x_lsb 
        = (1U & (IData)(vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__s1_sample_x));
    vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__s3_color_idx 
        = vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__s2_color_idx;
    vlSelfRef.top__DOT__vgc_inst__DOT__text_row_d2 
        = vlSelfRef.top__DOT__vgc_inst__DOT__text_row_d1;
    vlSelfRef.top__DOT__vgc_inst__DOT__text_col_d2 
        = vlSelfRef.top__DOT__vgc_inst__DOT__text_col_d1;
    vlSelfRef.vid_hsync = (1U & (~ (IData)(vlSelfRef.top__DOT__vgc_inst__DOT__h_sync_area_d2)));
    if (((IData)(vlSelfRef.top__DOT__vgc_inst__DOT__cursor_here) 
         & (IData)(vlSelfRef.top__DOT__vgc_inst__DOT__cursor_blink))) {
        vlSelfRef.vid_r = (0x0000000fU & (vlSelfRef.top__DOT__vgc_inst__DOT__palette
                                          [vlSelfRef.top__DOT__vgc_inst__DOT__fg_color] 
                                          >> 8U));
        vlSelfRef.vid_b = (0x0000000fU & vlSelfRef.top__DOT__vgc_inst__DOT__palette
                           [vlSelfRef.top__DOT__vgc_inst__DOT__fg_color]);
        vlSelfRef.vid_g = (0x0000000fU & (vlSelfRef.top__DOT__vgc_inst__DOT__palette
                                          [vlSelfRef.top__DOT__vgc_inst__DOT__fg_color] 
                                          >> 4U));
    } else {
        vlSelfRef.vid_r = (0x0000000fU & ((IData)(vlSelfRef.top__DOT__vgc_inst__DOT__pixel_color) 
                                          >> 8U));
        vlSelfRef.vid_b = (0x0000000fU & (IData)(vlSelfRef.top__DOT__vgc_inst__DOT__pixel_color));
        vlSelfRef.vid_g = (0x0000000fU & ((IData)(vlSelfRef.top__DOT__vgc_inst__DOT__pixel_color) 
                                          >> 4U));
    }
    vlSelfRef.vid_vsync = (1U & (~ (IData)(vlSelfRef.top__DOT__vgc_inst__DOT__v_sync_area_d2)));
    vlSelfRef.top__DOT__vgc_inst__DOT__font_b_dout 
        = vlSelfRef.top__DOT__vgc_inst__DOT__text_inst__DOT__font_mem__DOT__mem
        [(((IData)(vlSelfRef.top__DOT__vgc_inst__DOT__char_b_dout) 
           << 3U) | (IData)(vlSelfRef.top__DOT__vgc_inst__DOT__font_line))];
    vlSelfRef.vid_de = vlSelfRef.top__DOT__vgc_inst__DOT__visible_d2;
    vlSelfRef.top__DOT__vgc_inst__DOT__font_pixel_d2 
        = vlSelfRef.top__DOT__vgc_inst__DOT__font_pixel_d1;
    vlSelfRef.top__DOT__vgc_inst__DOT__gfx_b_dout = 
        ((0xf9ffU >= (IData)(vlSelfRef.top__DOT__vgc_inst__DOT__gfx_inst__DOT__gfx_b_addr))
          ? vlSelfRef.top__DOT__vgc_inst__DOT__gfx_inst__DOT__gfx_mem__DOT__mem
         [vlSelfRef.top__DOT__vgc_inst__DOT__gfx_inst__DOT__gfx_b_addr]
          : 0U);
    vlSelfRef.top__DOT__vgc_inst__DOT__tile_opaque_d2 
        = vlSelfRef.top__DOT__vgc_inst__DOT__tile_opaque_d1;
    vlSelfRef.top__DOT__vgc_inst__DOT__tile_rgb_d2 
        = vlSelfRef.top__DOT__vgc_inst__DOT__tile_rgb_d1;
    vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__s3_pixel_valid 
        = vlSelfRef.top__DOT__vgc_inst__DOT__in_text_area_d2;
    vlSelfRef.top__DOT__vgc_inst__DOT__sprite_inst__DOT__slb_b_dout 
        = ((0x013fU >= (IData)(vlSelfRef.top__DOT__vgc_inst__DOT__gfx_x_d2))
            ? vlSelfRef.top__DOT__vgc_inst__DOT__sprite_inst__DOT__scanline_buf__DOT__mem
           [vlSelfRef.top__DOT__vgc_inst__DOT__gfx_x_d2]
            : 0U);
    vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__s3_trans_color 
        = vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__trans_color;
    if (vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__tile_size16) {
        vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__s1_tile_w = 0x0fU;
        vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__s1_pix_in_x 
            = (0x0000000fU & (IData)(vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT____VdfgRegularize_hb33eefcd_0_2));
        vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__s1_pix_in_y 
            = (0x0000000fU & (IData)(vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT____VdfgRegularize_hb33eefcd_0_3));
    } else {
        vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__s1_tile_w = 7U;
        vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__s1_pix_in_x 
            = (0x0000000fU & (7U & (IData)(vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT____VdfgRegularize_hb33eefcd_0_2)));
        vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__s1_pix_in_y 
            = (0x0000000fU & (7U & (IData)(vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT____VdfgRegularize_hb33eefcd_0_3)));
    }
    vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__td_dout_b 
        = vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__tile_data_ram__DOT__mem
        [((IData)(vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__s1_tile_size16)
           ? (((IData)(vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__nt_dout_b) 
               << 7U) | (((IData)(vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__s1_sample_y) 
                          << 3U) | (7U & ((IData)(vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__s1_sample_x) 
                                          >> 1U))))
           : (((IData)(vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__nt_dout_b) 
               << 5U) | ((0x0000001cU & ((IData)(vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__s1_sample_y) 
                                         << 2U)) | 
                         (3U & ((IData)(vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__s1_sample_x) 
                                >> 1U)))))];
    if (((((((0x21U != (IData)(vlSelfRef.top__DOT__cpu_inst__DOT__state)) 
             & (0x22U != (IData)(vlSelfRef.top__DOT__cpu_inst__DOT__state))) 
            & (IData)(vlSelfRef.top__DOT____Vcellinp__cpu_inst__RDY)) 
           & (0x1eU != (IData)(vlSelfRef.top__DOT__cpu_inst__DOT__state))) 
          & (0x1fU != (IData)(vlSelfRef.top__DOT__cpu_inst__DOT__state))) 
         & (0x20U != (IData)(vlSelfRef.top__DOT__cpu_inst__DOT__state)))) {
        vlSelfRef.top__DOT__cpu_inst__DOT__ABL = (0x000000ffU 
                                                  & (IData)(vlSelfRef.top__DOT__cpu_addr));
        vlSelfRef.top__DOT__cpu_inst__DOT__ABH = (0x000000ffU 
                                                  & ((IData)(vlSelfRef.top__DOT__cpu_addr) 
                                                     >> 8U));
    }
    vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__cmd_nt_we = 0U;
    vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__cmd_at_we = 0U;
    vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__cmd_pal_we = 0U;
    if ((0x0bU == (IData)(vlSelfRef.top__DOT__cpu_inst__DOT__state))) {
        vlSelfRef.top__DOT__cpu_inst__DOT__I = 1U;
    } else if ((0x27U == (IData)(vlSelfRef.top__DOT__cpu_inst__DOT__state))) {
        vlSelfRef.top__DOT__cpu_inst__DOT__I = (1U 
                                                & ((IData)(vlSelfRef.top__DOT__cpu_inst__DOT__DIMUX) 
                                                   >> 2U));
    } else if ((0x24U == (IData)(vlSelfRef.top__DOT__cpu_inst__DOT__state))) {
        if (vlSelfRef.top__DOT__cpu_inst__DOT__sei) {
            vlSelfRef.top__DOT__cpu_inst__DOT__I = 1U;
        }
        if (vlSelfRef.top__DOT__cpu_inst__DOT__cli) {
            vlSelfRef.top__DOT__cpu_inst__DOT__I = 0U;
        }
    } else if ((0x0cU == (IData)(vlSelfRef.top__DOT__cpu_inst__DOT__state))) {
        if (vlSelfRef.top__DOT__cpu_inst__DOT__plp) {
            vlSelfRef.top__DOT__cpu_inst__DOT__I = 
                (1U & ((IData)(vlSelfRef.top__DOT__cpu_inst__DOT__ADD) 
                       >> 2U));
        }
    }
    if ((0x2eU == (IData)(vlSelfRef.top__DOT__cpu_inst__DOT__state))) {
        vlSelfRef.top__DOT__cpu_inst__DOT__Z = (1U 
                                                & (~ 
                                                   (0U 
                                                    != (IData)(vlSelfRef.top__DOT__cpu_inst__DOT__ADD))));
        vlSelfRef.top__DOT__cpu_inst__DOT__N = vlSelfRef.top__DOT__cpu_inst__DOT__AN;
    } else if ((0x27U == (IData)(vlSelfRef.top__DOT__cpu_inst__DOT__state))) {
        vlSelfRef.top__DOT__cpu_inst__DOT__Z = (1U 
                                                & ((IData)(vlSelfRef.top__DOT__cpu_inst__DOT__DIMUX) 
                                                   >> 1U));
        vlSelfRef.top__DOT__cpu_inst__DOT__N = (1U 
                                                & ((IData)(vlSelfRef.top__DOT__cpu_inst__DOT__DIMUX) 
                                                   >> 7U));
    } else if ((0x0cU == (IData)(vlSelfRef.top__DOT__cpu_inst__DOT__state))) {
        if (vlSelfRef.top__DOT__cpu_inst__DOT__plp) {
            vlSelfRef.top__DOT__cpu_inst__DOT__Z = 
                (1U & ((IData)(vlSelfRef.top__DOT__cpu_inst__DOT__ADD) 
                       >> 1U));
            vlSelfRef.top__DOT__cpu_inst__DOT__N = 
                (1U & ((IData)(vlSelfRef.top__DOT__cpu_inst__DOT__ADD) 
                       >> 7U));
        } else {
            if (((((IData)(vlSelfRef.top__DOT__cpu_inst__DOT__load_reg) 
                   & (1U != (IData)(vlSelfRef.top__DOT__cpu_inst__DOT__regsel))) 
                  | (IData)(vlSelfRef.top__DOT__cpu_inst__DOT__compare)) 
                 | (IData)(vlSelfRef.top__DOT__cpu_inst__DOT__bit_ins))) {
                vlSelfRef.top__DOT__cpu_inst__DOT__Z 
                    = (1U & (~ (0U != (IData)(vlSelfRef.top__DOT__cpu_inst__DOT__ADD))));
            }
            if ((((IData)(vlSelfRef.top__DOT__cpu_inst__DOT__load_reg) 
                  & (1U != (IData)(vlSelfRef.top__DOT__cpu_inst__DOT__regsel))) 
                 | (IData)(vlSelfRef.top__DOT__cpu_inst__DOT__compare))) {
                vlSelfRef.top__DOT__cpu_inst__DOT__N 
                    = vlSelfRef.top__DOT__cpu_inst__DOT__AN;
            }
        }
    } else if (((0x0dU == (IData)(vlSelfRef.top__DOT__cpu_inst__DOT__state)) 
                & (IData)(vlSelfRef.top__DOT__cpu_inst__DOT__bit_ins))) {
        vlSelfRef.top__DOT__cpu_inst__DOT__N = (1U 
                                                & ((IData)(vlSelfRef.top__DOT__cpu_inst__DOT__DIMUX) 
                                                   >> 7U));
    }
    if (((IData)(vlSelfRef.top__DOT__cpu_inst__DOT__shift) 
         & (0x2eU == (IData)(vlSelfRef.top__DOT__cpu_inst__DOT__state)))) {
        vlSelfRef.top__DOT__cpu_inst__DOT__C = vlSelfRef.top__DOT__cpu_inst__DOT__CO;
    } else if ((0x27U == (IData)(vlSelfRef.top__DOT__cpu_inst__DOT__state))) {
        vlSelfRef.top__DOT__cpu_inst__DOT__C = (1U 
                                                & (IData)(vlSelfRef.top__DOT__cpu_inst__DOT__DIMUX));
    } else if (((~ (IData)(vlSelfRef.top__DOT__cpu_inst__DOT__write_back)) 
                & (0x0cU == (IData)(vlSelfRef.top__DOT__cpu_inst__DOT__state)))) {
        if ((((IData)(vlSelfRef.top__DOT__cpu_inst__DOT__adc_sbc) 
              | (IData)(vlSelfRef.top__DOT__cpu_inst__DOT__shift)) 
             | (IData)(vlSelfRef.top__DOT__cpu_inst__DOT__compare))) {
            vlSelfRef.top__DOT__cpu_inst__DOT__C = vlSelfRef.top__DOT__cpu_inst__DOT__CO;
        } else if (vlSelfRef.top__DOT__cpu_inst__DOT__plp) {
            vlSelfRef.top__DOT__cpu_inst__DOT__C = 
                (1U & (IData)(vlSelfRef.top__DOT__cpu_inst__DOT__ADD));
        } else {
            if (vlSelfRef.top__DOT__cpu_inst__DOT__sec) {
                vlSelfRef.top__DOT__cpu_inst__DOT__C = 1U;
            }
            if (vlSelfRef.top__DOT__cpu_inst__DOT__clc) {
                vlSelfRef.top__DOT__cpu_inst__DOT__C = 0U;
            }
        }
    }
    vlSelfRef.top__DOT__vgc_inst__DOT__blink_count 
        = __Vdly__top__DOT__vgc_inst__DOT__blink_count;
    if (__VdlySet__top__DOT__vgc_inst__DOT__fio_name__v0) {
        vlSelfRef.top__DOT__vgc_inst__DOT__fio_name[__VdlyDim0__top__DOT__vgc_inst__DOT__fio_name__v0] 
            = __VdlyVal__top__DOT__vgc_inst__DOT__fio_name__v0;
    }
    if (__VdlySet__top__DOT__vgc_inst__DOT__spr_shape__v0) {
        vlSelfRef.top__DOT__vgc_inst__DOT__spr_shape[0U] = 0U;
        vlSelfRef.top__DOT__vgc_inst__DOT__spr_shape[1U] = 0U;
        vlSelfRef.top__DOT__vgc_inst__DOT__spr_shape[2U] = 0U;
        vlSelfRef.top__DOT__vgc_inst__DOT__spr_shape[3U] = 0U;
        vlSelfRef.top__DOT__vgc_inst__DOT__spr_shape[4U] = 0U;
        vlSelfRef.top__DOT__vgc_inst__DOT__spr_shape[5U] = 0U;
        vlSelfRef.top__DOT__vgc_inst__DOT__spr_shape[6U] = 0U;
        vlSelfRef.top__DOT__vgc_inst__DOT__spr_shape[7U] = 0U;
        vlSelfRef.top__DOT__vgc_inst__DOT__spr_shape[8U] = 0U;
        vlSelfRef.top__DOT__vgc_inst__DOT__spr_shape[9U] = 0U;
        vlSelfRef.top__DOT__vgc_inst__DOT__spr_shape[10U] = 0U;
        vlSelfRef.top__DOT__vgc_inst__DOT__spr_shape[11U] = 0U;
        vlSelfRef.top__DOT__vgc_inst__DOT__spr_shape[12U] = 0U;
        vlSelfRef.top__DOT__vgc_inst__DOT__spr_shape[13U] = 0U;
        vlSelfRef.top__DOT__vgc_inst__DOT__spr_shape[14U] = 0U;
        vlSelfRef.top__DOT__vgc_inst__DOT__spr_shape[15U] = 0U;
    }
    if (__VdlySet__top__DOT__vgc_inst__DOT__spr_shape__v16) {
        vlSelfRef.top__DOT__vgc_inst__DOT__spr_shape[__VdlyDim0__top__DOT__vgc_inst__DOT__spr_shape__v16] 
            = __VdlyVal__top__DOT__vgc_inst__DOT__spr_shape__v16;
    }
    if (__VdlySet__top__DOT__vgc_inst__DOT__spr_trans__v0) {
        vlSelfRef.top__DOT__vgc_inst__DOT__spr_trans[0U] = 0U;
        vlSelfRef.top__DOT__vgc_inst__DOT__spr_trans[1U] = 0U;
        vlSelfRef.top__DOT__vgc_inst__DOT__spr_trans[2U] = 0U;
        vlSelfRef.top__DOT__vgc_inst__DOT__spr_trans[3U] = 0U;
        vlSelfRef.top__DOT__vgc_inst__DOT__spr_trans[4U] = 0U;
        vlSelfRef.top__DOT__vgc_inst__DOT__spr_trans[5U] = 0U;
        vlSelfRef.top__DOT__vgc_inst__DOT__spr_trans[6U] = 0U;
        vlSelfRef.top__DOT__vgc_inst__DOT__spr_trans[7U] = 0U;
        vlSelfRef.top__DOT__vgc_inst__DOT__spr_trans[8U] = 0U;
        vlSelfRef.top__DOT__vgc_inst__DOT__spr_trans[9U] = 0U;
        vlSelfRef.top__DOT__vgc_inst__DOT__spr_trans[10U] = 0U;
        vlSelfRef.top__DOT__vgc_inst__DOT__spr_trans[11U] = 0U;
        vlSelfRef.top__DOT__vgc_inst__DOT__spr_trans[12U] = 0U;
        vlSelfRef.top__DOT__vgc_inst__DOT__spr_trans[13U] = 0U;
        vlSelfRef.top__DOT__vgc_inst__DOT__spr_trans[14U] = 0U;
        vlSelfRef.top__DOT__vgc_inst__DOT__spr_trans[15U] = 0U;
    }
    if (__VdlySet__top__DOT__vgc_inst__DOT__spr_trans__v16) {
        vlSelfRef.top__DOT__vgc_inst__DOT__spr_trans[__VdlyDim0__top__DOT__vgc_inst__DOT__spr_trans__v16] 
            = __VdlyVal__top__DOT__vgc_inst__DOT__spr_trans__v16;
    }
    vlSelfRef.top__DOT__blt_inst__DOT__wrote_count 
        = __Vdly__top__DOT__blt_inst__DOT__wrote_count;
    if (__VdlySet__top__DOT__blt_inst__DOT__regs__v0) {
        vlSelfRef.top__DOT__blt_inst__DOT__regs[1U] = 0U;
    }
    if (__VdlySet__top__DOT__blt_inst__DOT__regs__v1) {
        vlSelfRef.top__DOT__blt_inst__DOT__regs[2U] = 0U;
    }
    if (__VdlySet__top__DOT__blt_inst__DOT__regs__v2) {
        vlSelfRef.top__DOT__blt_inst__DOT__regs[1U] = 2U;
    }
    if (__VdlySet__top__DOT__blt_inst__DOT__regs__v3) {
        vlSelfRef.top__DOT__blt_inst__DOT__regs[2U] = 0U;
        vlSelfRef.top__DOT__blt_inst__DOT__regs[24U] 
            = __VdlyVal__top__DOT__blt_inst__DOT__regs__v4;
        vlSelfRef.top__DOT__blt_inst__DOT__regs[23U] 
            = __VdlyVal__top__DOT__blt_inst__DOT__regs__v5;
        vlSelfRef.top__DOT__blt_inst__DOT__regs[22U] 
            = __VdlyVal__top__DOT__blt_inst__DOT__regs__v6;
    }
    if (__VdlySet__top__DOT__blt_inst__DOT__regs__v7) {
        vlSelfRef.top__DOT__blt_inst__DOT__regs[1U] = 2U;
    }
    if (__VdlySet__top__DOT__blt_inst__DOT__regs__v8) {
        vlSelfRef.top__DOT__blt_inst__DOT__regs[2U] = 0U;
        vlSelfRef.top__DOT__blt_inst__DOT__regs[24U] 
            = __VdlyVal__top__DOT__blt_inst__DOT__regs__v9;
        vlSelfRef.top__DOT__blt_inst__DOT__regs[23U] 
            = __VdlyVal__top__DOT__blt_inst__DOT__regs__v10;
        vlSelfRef.top__DOT__blt_inst__DOT__regs[22U] 
            = __VdlyVal__top__DOT__blt_inst__DOT__regs__v11;
    }
    if (__VdlySet__top__DOT__blt_inst__DOT__regs__v12) {
        vlSelfRef.top__DOT__blt_inst__DOT__regs[1U] = 3U;
    }
    if (__VdlySet__top__DOT__blt_inst__DOT__regs__v13) {
        vlSelfRef.top__DOT__blt_inst__DOT__regs[2U] = 4U;
    }
    if (__VdlySet__top__DOT__blt_inst__DOT__regs__v14) {
        vlSelfRef.top__DOT__blt_inst__DOT__regs[1U] = 3U;
    }
    if (__VdlySet__top__DOT__blt_inst__DOT__regs__v15) {
        vlSelfRef.top__DOT__blt_inst__DOT__regs[2U] = 2U;
    }
    if (__VdlySet__top__DOT__blt_inst__DOT__regs__v16) {
        vlSelfRef.top__DOT__blt_inst__DOT__regs[1U] = 3U;
    }
    if (__VdlySet__top__DOT__blt_inst__DOT__regs__v17) {
        vlSelfRef.top__DOT__blt_inst__DOT__regs[2U] = 2U;
    }
    if (__VdlySet__top__DOT__blt_inst__DOT__regs__v18) {
        vlSelfRef.top__DOT__blt_inst__DOT__regs[1U] = 3U;
    }
    if (__VdlySet__top__DOT__blt_inst__DOT__regs__v19) {
        vlSelfRef.top__DOT__blt_inst__DOT__regs[2U] = 3U;
    }
    if (__VdlySet__top__DOT__blt_inst__DOT__regs__v20) {
        vlSelfRef.top__DOT__blt_inst__DOT__regs[1U] = 3U;
    }
    if (__VdlySet__top__DOT__blt_inst__DOT__regs__v21) {
        vlSelfRef.top__DOT__blt_inst__DOT__regs[2U] = 5U;
    }
    if (__VdlySet__top__DOT__blt_inst__DOT__regs__v22) {
        vlSelfRef.top__DOT__blt_inst__DOT__regs[1U] = 3U;
    }
    if (__VdlySet__top__DOT__blt_inst__DOT__regs__v23) {
        vlSelfRef.top__DOT__blt_inst__DOT__regs[2U] = 3U;
    }
    if (__VdlySet__top__DOT__blt_inst__DOT__regs__v24) {
        vlSelfRef.top__DOT__blt_inst__DOT__regs[__VdlyDim0__top__DOT__blt_inst__DOT__regs__v24] 
            = __VdlyVal__top__DOT__blt_inst__DOT__regs__v24;
    }
    if (__VdlySet__top__DOT__blt_inst__DOT__regs__v25) {
        vlSelfRef.top__DOT__blt_inst__DOT__regs[1U] = 1U;
        vlSelfRef.top__DOT__blt_inst__DOT__regs[2U] = 0U;
        vlSelfRef.top__DOT__blt_inst__DOT__regs[22U] = 0U;
        vlSelfRef.top__DOT__blt_inst__DOT__regs[23U] = 0U;
        vlSelfRef.top__DOT__blt_inst__DOT__regs[24U] = 0U;
    }
    if (__VdlySet__top__DOT__blt_inst__DOT__regs__v30) {
        vlSelfRef.top__DOT__blt_inst__DOT__regs[1U] = 3U;
        vlSelfRef.top__DOT__blt_inst__DOT__regs[2U] = 1U;
    }
    if (__VdlySet__top__DOT__blt_inst__DOT__regs__v32) {
        vlSelfRef.top__DOT__blt_inst__DOT__regs[1U] = 3U;
        vlSelfRef.top__DOT__blt_inst__DOT__regs[2U] = 1U;
    }
    vlSelfRef.top__DOT__blt_inst__DOT__col = __Vdly__top__DOT__blt_inst__DOT__col;
    vlSelfRef.top__DOT__blt_inst__DOT__row = __Vdly__top__DOT__blt_inst__DOT__row;
    vlSelfRef.top__DOT__blt_inst__DOT__read_valid = __Vdly__top__DOT__blt_inst__DOT__read_valid;
    vlSelfRef.top__DOT__blt_inst__DOT__buf_idx = __Vdly__top__DOT__blt_inst__DOT__buf_idx;
    if (__VdlySet__top__DOT__blt_inst__DOT__row_buf__v0) {
        vlSelfRef.top__DOT__blt_inst__DOT__row_buf[__VdlyDim0__top__DOT__blt_inst__DOT__row_buf__v0] 
            = __VdlyVal__top__DOT__blt_inst__DOT__row_buf__v0;
    }
    vlSelfRef.top__DOT__blt_inst__DOT__state = __Vdly__top__DOT__blt_inst__DOT__state;
    if (__VdlySet__top__DOT__cpu_inst__DOT__AXYS__v0) {
        vlSelfRef.top__DOT__cpu_inst__DOT__AXYS[__VdlyDim0__top__DOT__cpu_inst__DOT__AXYS__v0] 
            = __VdlyVal__top__DOT__cpu_inst__DOT__AXYS__v0;
    }
    vlSelfRef.top__DOT__vgc_inst__DOT__cursor_blink 
        = __Vdly__top__DOT__vgc_inst__DOT__cursor_blink;
    if (__VdlySet__top__DOT__vgc_inst__DOT__sprite_inst__DOT__scanline_buf__DOT__mem__v0) {
        vlSelfRef.top__DOT__vgc_inst__DOT__sprite_inst__DOT__scanline_buf__DOT__mem[__VdlyDim0__top__DOT__vgc_inst__DOT__sprite_inst__DOT__scanline_buf__DOT__mem__v0] 
            = __VdlyVal__top__DOT__vgc_inst__DOT__sprite_inst__DOT__scanline_buf__DOT__mem__v0;
    }
    if (vlSelfRef.rst) {
        vlSelfRef.top__DOT__vgc_inst__DOT__char_in_reg = 0U;
        vlSelfRef.top__DOT__vgc_inst__DOT__font_slot = 0U;
        vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__scroll_x = 0U;
        vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__scroll_y = 0U;
        vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__mirror_mode = 0U;
        vlSelfRef.top__DOT__cpu_inst__DOT__res = 1U;
        vlSelfRef.top__DOT__cpu_inst__DOT__IRHOLD_valid = 0U;
        __Vdly__top__DOT__vgc_inst__DOT__tile_inst__DOT__dma_state = 0U;
        __Vdly__top__DOT__vgc_inst__DOT__tile_inst__DOT__dma_data_valid = 0U;
    } else {
        if (vlSelfRef.key_valid) {
            vlSelfRef.top__DOT__vgc_inst__DOT__char_in_reg 
                = vlSelfRef.key_data;
        }
        if (vlSelfRef.top__DOT__cpu_we) {
            if (vlSelfRef.top__DOT__vgc_inst__DOT__vgc_reg_sel) {
                if (((((((((0U == (0x0000001fU & (IData)(vlSelfRef.top__DOT__cpu_addr))) 
                           | (1U == (0x0000001fU & (IData)(vlSelfRef.top__DOT__cpu_addr)))) 
                          | (2U == (0x0000001fU & (IData)(vlSelfRef.top__DOT__cpu_addr)))) 
                         | (3U == (0x0000001fU & (IData)(vlSelfRef.top__DOT__cpu_addr)))) 
                        | (4U == (0x0000001fU & (IData)(vlSelfRef.top__DOT__cpu_addr)))) 
                       | (5U == (0x0000001fU & (IData)(vlSelfRef.top__DOT__cpu_addr)))) 
                      | (6U == (0x0000001fU & (IData)(vlSelfRef.top__DOT__cpu_addr)))) 
                     | (7U == (0x0000001fU & (IData)(vlSelfRef.top__DOT__cpu_addr))))) {
                    if ((0U != (0x0000001fU & (IData)(vlSelfRef.top__DOT__cpu_addr)))) {
                        if ((1U != (0x0000001fU & (IData)(vlSelfRef.top__DOT__cpu_addr)))) {
                            if ((2U != (0x0000001fU 
                                        & (IData)(vlSelfRef.top__DOT__cpu_addr)))) {
                                if ((3U != (0x0000001fU 
                                            & (IData)(vlSelfRef.top__DOT__cpu_addr)))) {
                                    if ((4U != (0x0000001fU 
                                                & (IData)(vlSelfRef.top__DOT__cpu_addr)))) {
                                        if ((5U != 
                                             (0x0000001fU 
                                              & (IData)(vlSelfRef.top__DOT__cpu_addr)))) {
                                            if ((6U 
                                                 != 
                                                 (0x0000001fU 
                                                  & (IData)(vlSelfRef.top__DOT__cpu_addr)))) {
                                                vlSelfRef.top__DOT__vgc_inst__DOT__font_slot 
                                                    = 
                                                    (7U 
                                                     & (IData)(vlSelfRef.top__DOT__cpu_dout));
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
        if ((0x0cU == (IData)(vlSelfRef.top__DOT__cpu_inst__DOT__state))) {
            vlSelfRef.top__DOT__cpu_inst__DOT__res = 0U;
        }
        if (vlSelfRef.top__DOT____Vcellinp__cpu_inst__RDY) {
            if (((0x1eU == (IData)(vlSelfRef.top__DOT__cpu_inst__DOT__state)) 
                 | (0x21U == (IData)(vlSelfRef.top__DOT__cpu_inst__DOT__state)))) {
                vlSelfRef.top__DOT__cpu_inst__DOT__IRHOLD_valid = 1U;
            } else if ((0x0cU == (IData)(vlSelfRef.top__DOT__cpu_inst__DOT__state))) {
                vlSelfRef.top__DOT__cpu_inst__DOT__IRHOLD_valid = 0U;
            }
        }
        __Vdly__top__DOT__vgc_inst__DOT__tile_inst__DOT__dma_data_valid = 0U;
        if ((0x00000010U & (IData)(vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__dma_state))) {
            __Vdly__top__DOT__vgc_inst__DOT__tile_inst__DOT__dma_state = 0U;
        } else if ((8U & (IData)(vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__dma_state))) {
            if ((4U & (IData)(vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__dma_state))) {
                __Vdly__top__DOT__vgc_inst__DOT__tile_inst__DOT__dma_state 
                    = ((2U & (IData)(vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__dma_state))
                        ? 0U : ((1U & (IData)(vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__dma_state))
                                 ? 0x0eU : 0x0dU));
            } else if ((2U & (IData)(vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__dma_state))) {
                if ((1U & (IData)(vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__dma_state))) {
                    if ((0x18U > (IData)(vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__dma_col_idx))) {
                        __Vdly__top__DOT__vgc_inst__DOT__tile_inst__DOT__dma_col_idx 
                            = (0x0000001fU & ((IData)(1U) 
                                              + (IData)(vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__dma_col_idx)));
                    } else {
                        __Vdly__top__DOT__vgc_inst__DOT__tile_inst__DOT__dma_state = 0U;
                    }
                } else if ((0x03e7U > (IData)(vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__dma_count))) {
                    __Vdly__top__DOT__vgc_inst__DOT__tile_inst__DOT__dma_count 
                        = (0x0000ffffU & ((IData)(1U) 
                                          + (IData)(vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__dma_count)));
                } else {
                    __Vdly__top__DOT__vgc_inst__DOT__tile_inst__DOT__dma_state = 0U;
                }
            } else if ((1U & (IData)(vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__dma_state))) {
                if ((0x0fa0U > (IData)(vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__dma_fill_addr))) {
                    vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__dma_fill_addr 
                        = (0x00000fffU & ((IData)(1U) 
                                          + (IData)(vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__dma_fill_addr)));
                } else {
                    __Vdly__top__DOT__vgc_inst__DOT__tile_inst__DOT__dma_state = 0U;
                }
            } else {
                if (vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__dma_data_valid) {
                    __Vdly__top__DOT__vgc_inst__DOT__tile_inst__DOT__dma_col_idx 
                        = (0x0000001fU & ((IData)(1U) 
                                          + (IData)(vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__dma_col_idx)));
                    __Vdly__top__DOT__vgc_inst__DOT__tile_inst__DOT__dma_count 
                        = (0x0000ffffU & ((IData)(vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__dma_count) 
                                          - (IData)(1U)));
                }
                if (((0U == (IData)(vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__dma_count)) 
                     & (~ (IData)(vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__dma_data_valid)))) {
                    __Vdly__top__DOT__vgc_inst__DOT__tile_inst__DOT__dma_state = 0U;
                } else if ((1U & ((~ (IData)(vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__dma_data_valid)) 
                                  | (1U < (IData)(vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__dma_count))))) {
                    vlSelfRef.top__DOT__tile_dma_addr 
                        = vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__dma_src;
                    __Vdly__top__DOT__vgc_inst__DOT__tile_inst__DOT__dma_data_valid = 1U;
                    __Vdly__top__DOT__vgc_inst__DOT__tile_inst__DOT__dma_src 
                        = (0x0000ffffU & ((IData)(1U) 
                                          + (IData)(vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__dma_src)));
                }
            }
        } else if ((4U & (IData)(vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__dma_state))) {
            if ((2U & (IData)(vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__dma_state))) {
                if ((1U & (IData)(vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__dma_state))) {
                    if (vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__dma_data_valid) {
                        __Vdly__top__DOT__vgc_inst__DOT__tile_inst__DOT__dma_count 
                            = (0x0000ffffU & ((IData)(vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__dma_count) 
                                              - (IData)(1U)));
                        __Vdly__top__DOT__vgc_inst__DOT__tile_inst__DOT__dma_dst 
                            = (0x0000ffffU & ((IData)(1U) 
                                              + (IData)(vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__dma_dst)));
                    }
                    if (((0U == (IData)(vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__dma_count)) 
                         & (~ (IData)(vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__dma_data_valid)))) {
                        __Vdly__top__DOT__vgc_inst__DOT__tile_inst__DOT__dma_state = 0U;
                    } else if ((1U & ((~ (IData)(vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__dma_data_valid)) 
                                      | (1U < (IData)(vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__dma_count))))) {
                        vlSelfRef.top__DOT__tile_dma_addr 
                            = vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__dma_src;
                        __Vdly__top__DOT__vgc_inst__DOT__tile_inst__DOT__dma_data_valid = 1U;
                        __Vdly__top__DOT__vgc_inst__DOT__tile_inst__DOT__dma_src 
                            = (0x0000ffffU & ((IData)(1U) 
                                              + (IData)(vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__dma_src)));
                    }
                } else {
                    if (vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__dma_data_valid) {
                        __Vdly__top__DOT__vgc_inst__DOT__tile_inst__DOT__dma_col_idx 
                            = (0x0000001fU & ((IData)(1U) 
                                              + (IData)(vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__dma_col_idx)));
                        __Vdly__top__DOT__vgc_inst__DOT__tile_inst__DOT__dma_count 
                            = (0x0000ffffU & ((IData)(vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__dma_count) 
                                              - (IData)(1U)));
                    }
                    if (((0U == (IData)(vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__dma_count)) 
                         & (~ (IData)(vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__dma_data_valid)))) {
                        __Vdly__top__DOT__vgc_inst__DOT__tile_inst__DOT__dma_state = 0U;
                    } else if ((1U & ((~ (IData)(vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__dma_data_valid)) 
                                      | (1U < (IData)(vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__dma_count))))) {
                        vlSelfRef.top__DOT__tile_dma_addr 
                            = vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__dma_src;
                        __Vdly__top__DOT__vgc_inst__DOT__tile_inst__DOT__dma_data_valid = 1U;
                        __Vdly__top__DOT__vgc_inst__DOT__tile_inst__DOT__dma_src 
                            = (0x0000ffffU & ((IData)(1U) 
                                              + (IData)(vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__dma_src)));
                    }
                }
            } else if ((1U & (IData)(vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__dma_state))) {
                if (vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__dma_data_valid) {
                    __Vdly__top__DOT__vgc_inst__DOT__tile_inst__DOT__dma_count 
                        = (0x0000ffffU & ((IData)(vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__dma_count) 
                                          - (IData)(1U)));
                    __Vdly__top__DOT__vgc_inst__DOT__tile_inst__DOT__dma_dst 
                        = (0x0000ffffU & ((IData)(1U) 
                                          + (IData)(vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__dma_dst)));
                }
                if (((0U == (IData)(vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__dma_count)) 
                     & (~ (IData)(vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__dma_data_valid)))) {
                    __Vdly__top__DOT__vgc_inst__DOT__tile_inst__DOT__dma_state = 0U;
                } else if ((1U & ((~ (IData)(vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__dma_data_valid)) 
                                  | (1U < (IData)(vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__dma_count))))) {
                    vlSelfRef.top__DOT__tile_dma_addr 
                        = vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__dma_src;
                    __Vdly__top__DOT__vgc_inst__DOT__tile_inst__DOT__dma_data_valid = 1U;
                    __Vdly__top__DOT__vgc_inst__DOT__tile_inst__DOT__dma_src 
                        = (0x0000ffffU & ((IData)(1U) 
                                          + (IData)(vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__dma_src)));
                }
            } else {
                if (vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__dma_data_valid) {
                    if ((0U == (IData)(vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__dma_pal_phase))) {
                        vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__dma_pal_r 
                            = vlSelfRef.top__DOT__tile_dma_data;
                        __Vdly__top__DOT__vgc_inst__DOT__tile_inst__DOT__dma_pal_phase = 1U;
                    } else if ((1U == (IData)(vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__dma_pal_phase))) {
                        vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__dma_pal_g 
                            = vlSelfRef.top__DOT__tile_dma_data;
                        __Vdly__top__DOT__vgc_inst__DOT__tile_inst__DOT__dma_pal_phase = 2U;
                    } else if ((2U == (IData)(vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__dma_pal_phase))) {
                        __Vdly__top__DOT__vgc_inst__DOT__tile_inst__DOT__dma_pal_phase = 0U;
                        if ((0x0fU == (IData)(vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__dma_pal_cidx))) {
                            __Vdly__top__DOT__vgc_inst__DOT__tile_inst__DOT__dma_count = 0U;
                        } else {
                            vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__dma_pal_cidx 
                                = (0x0000000fU & ((IData)(1U) 
                                                  + (IData)(vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__dma_pal_cidx)));
                        }
                    } else {
                        __Vdly__top__DOT__vgc_inst__DOT__tile_inst__DOT__dma_pal_phase = 0U;
                    }
                }
                if (((0U == (IData)(vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__dma_count)) 
                     & (~ (IData)(vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__dma_data_valid)))) {
                    __Vdly__top__DOT__vgc_inst__DOT__tile_inst__DOT__dma_state = 0U;
                } else if ((1U & ((~ (IData)(vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__dma_data_valid)) 
                                  | (0U < (IData)(vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__dma_count))))) {
                    vlSelfRef.top__DOT__tile_dma_addr 
                        = vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__dma_src;
                    __Vdly__top__DOT__vgc_inst__DOT__tile_inst__DOT__dma_data_valid = 1U;
                    __Vdly__top__DOT__vgc_inst__DOT__tile_inst__DOT__dma_src 
                        = (0x0000ffffU & ((IData)(1U) 
                                          + (IData)(vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__dma_src)));
                }
            }
        } else if ((2U & (IData)(vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__dma_state))) {
            if ((1U & (IData)(vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__dma_state))) {
                if (vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__dma_data_valid) {
                    __Vdly__top__DOT__vgc_inst__DOT__tile_inst__DOT__dma_count 
                        = (0x0000ffffU & ((IData)(vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__dma_count) 
                                          - (IData)(1U)));
                    __Vdly__top__DOT__vgc_inst__DOT__tile_inst__DOT__dma_dst 
                        = (0x0000ffffU & ((IData)(1U) 
                                          + (IData)(vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__dma_dst)));
                }
                if (((0U == (IData)(vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__dma_count)) 
                     & (~ (IData)(vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__dma_data_valid)))) {
                    __Vdly__top__DOT__vgc_inst__DOT__tile_inst__DOT__dma_state = 0U;
                } else if ((1U & ((~ (IData)(vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__dma_data_valid)) 
                                  | (1U < (IData)(vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__dma_count))))) {
                    vlSelfRef.top__DOT__tile_dma_addr 
                        = vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__dma_src;
                    __Vdly__top__DOT__vgc_inst__DOT__tile_inst__DOT__dma_data_valid = 1U;
                    __Vdly__top__DOT__vgc_inst__DOT__tile_inst__DOT__dma_src 
                        = (0x0000ffffU & ((IData)(1U) 
                                          + (IData)(vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__dma_src)));
                }
            } else {
                if (vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__dma_data_valid) {
                    __Vdly__top__DOT__vgc_inst__DOT__tile_inst__DOT__dma_count 
                        = (0x0000ffffU & ((IData)(vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__dma_count) 
                                          - (IData)(1U)));
                    __Vdly__top__DOT__vgc_inst__DOT__tile_inst__DOT__dma_dst 
                        = (0x0000ffffU & ((IData)(1U) 
                                          + (IData)(vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__dma_dst)));
                }
                if (((0U == (IData)(vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__dma_count)) 
                     & (~ (IData)(vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__dma_data_valid)))) {
                    __Vdly__top__DOT__vgc_inst__DOT__tile_inst__DOT__dma_state = 0U;
                } else if ((1U & ((~ (IData)(vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__dma_data_valid)) 
                                  | (1U < (IData)(vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__dma_count))))) {
                    vlSelfRef.top__DOT__tile_dma_addr 
                        = vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__dma_src;
                    __Vdly__top__DOT__vgc_inst__DOT__tile_inst__DOT__dma_data_valid = 1U;
                    __Vdly__top__DOT__vgc_inst__DOT__tile_inst__DOT__dma_src 
                        = (0x0000ffffU & ((IData)(1U) 
                                          + (IData)(vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__dma_src)));
                }
            }
        } else if ((1U & (IData)(vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__dma_state))) {
            if (vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__dma_data_valid) {
                __Vdly__top__DOT__vgc_inst__DOT__tile_inst__DOT__dma_count 
                    = (0x0000ffffU & ((IData)(vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__dma_count) 
                                      - (IData)(1U)));
                __Vdly__top__DOT__vgc_inst__DOT__tile_inst__DOT__dma_dst 
                    = (0x0000ffffU & ((IData)(1U) + (IData)(vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__dma_dst)));
            }
            if (((0U == (IData)(vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__dma_count)) 
                 & (~ (IData)(vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__dma_data_valid)))) {
                __Vdly__top__DOT__vgc_inst__DOT__tile_inst__DOT__dma_state = 0U;
            } else if ((1U & ((~ (IData)(vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__dma_data_valid)) 
                              | (1U < (IData)(vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__dma_count))))) {
                vlSelfRef.top__DOT__tile_dma_addr = vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__dma_src;
                __Vdly__top__DOT__vgc_inst__DOT__tile_inst__DOT__dma_data_valid = 1U;
                __Vdly__top__DOT__vgc_inst__DOT__tile_inst__DOT__dma_src 
                    = (0x0000ffffU & ((IData)(1U) + (IData)(vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__dma_src)));
            }
        }
        if (((IData)(vlSelfRef.top__DOT__cpu_we) & (IData)(vlSelfRef.top__DOT__vgc_inst__DOT__tile_reg_sel))) {
            if ((0U != (0x0000001fU & (IData)(vlSelfRef.top__DOT__cpu_addr)))) {
                if ((1U != (0x0000001fU & (IData)(vlSelfRef.top__DOT__cpu_addr)))) {
                    if ((2U == (0x0000001fU & (IData)(vlSelfRef.top__DOT__cpu_addr)))) {
                        vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__scroll_x 
                            = ((0xff00U & (IData)(vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__scroll_x)) 
                               | (IData)(vlSelfRef.top__DOT__cpu_dout));
                    } else if ((3U == (0x0000001fU 
                                       & (IData)(vlSelfRef.top__DOT__cpu_addr)))) {
                        vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__scroll_x 
                            = ((0x00ffU & (IData)(vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__scroll_x)) 
                               | ((IData)(vlSelfRef.top__DOT__cpu_dout) 
                                  << 8U));
                    }
                    if ((2U != (0x0000001fU & (IData)(vlSelfRef.top__DOT__cpu_addr)))) {
                        if ((3U != (0x0000001fU & (IData)(vlSelfRef.top__DOT__cpu_addr)))) {
                            if ((4U == (0x0000001fU 
                                        & (IData)(vlSelfRef.top__DOT__cpu_addr)))) {
                                vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__scroll_y 
                                    = ((0xff00U & (IData)(vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__scroll_y)) 
                                       | (IData)(vlSelfRef.top__DOT__cpu_dout));
                            } else if ((5U == (0x0000001fU 
                                               & (IData)(vlSelfRef.top__DOT__cpu_addr)))) {
                                vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__scroll_y 
                                    = ((0x00ffU & (IData)(vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__scroll_y)) 
                                       | ((IData)(vlSelfRef.top__DOT__cpu_dout) 
                                          << 8U));
                            }
                            if ((4U != (0x0000001fU 
                                        & (IData)(vlSelfRef.top__DOT__cpu_addr)))) {
                                if ((5U != (0x0000001fU 
                                            & (IData)(vlSelfRef.top__DOT__cpu_addr)))) {
                                    if ((7U == (0x0000001fU 
                                                & (IData)(vlSelfRef.top__DOT__cpu_addr)))) {
                                        if ((0U == (IData)(vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__dma_state))) {
                                            if ((1U 
                                                 & (~ 
                                                    ((IData)(vlSelfRef.top__DOT__cpu_dout) 
                                                     >> 7U)))) {
                                                if (
                                                    (1U 
                                                     & (~ 
                                                        ((IData)(vlSelfRef.top__DOT__cpu_dout) 
                                                         >> 6U)))) {
                                                    if (
                                                        (1U 
                                                         & (~ 
                                                            ((IData)(vlSelfRef.top__DOT__cpu_dout) 
                                                             >> 5U)))) {
                                                        if (
                                                            (0x00000010U 
                                                             & (IData)(vlSelfRef.top__DOT__cpu_dout))) {
                                                            if (
                                                                (1U 
                                                                 & (~ 
                                                                    ((IData)(vlSelfRef.top__DOT__cpu_dout) 
                                                                     >> 3U)))) {
                                                                if (
                                                                    (1U 
                                                                     & (~ 
                                                                        ((IData)(vlSelfRef.top__DOT__cpu_dout) 
                                                                         >> 2U)))) {
                                                                    if (
                                                                        (2U 
                                                                         & (IData)(vlSelfRef.top__DOT__cpu_dout))) {
                                                                        if (
                                                                            (1U 
                                                                             & (IData)(vlSelfRef.top__DOT__cpu_dout))) {
                                                                            if (
                                                                                ((4U 
                                                                                > vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__tregs[8U]) 
                                                                                & (0x28U 
                                                                                > vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__tregs[9U]))) {
                                                                                vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__dma_nt 
                                                                                = 
                                                                                (3U 
                                                                                & vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__tregs[8U]);
                                                                                vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__dma_col_row 
                                                                                = 
                                                                                (0x0000003fU 
                                                                                & vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__tregs[9U]);
                                                                                __Vdly__top__DOT__vgc_inst__DOT__tile_inst__DOT__dma_col_idx = 0U;
                                                                                __Vdly__top__DOT__vgc_inst__DOT__tile_inst__DOT__dma_state = 0x0bU;
                                                                            }
                                                                        } else {
                                                                            if (
                                                                                (0U 
                                                                                >= vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__tregs[8U])) {
                                                                                vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT____Vlvbound_ha768cd16__0 
                                                                                = vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__tregs[10U];
                                                                                __VdlyVal__top__DOT__vgc_inst__DOT__tile_inst__DOT__col_buffer__v0 
                                                                                = vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT____Vlvbound_ha768cd16__0;
                                                                                __VdlySet__top__DOT__vgc_inst__DOT__tile_inst__DOT__col_buffer__v0 = 1U;
                                                                            }
                                                                            if (
                                                                                ((1U 
                                                                                >= vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__tregs[8U]) 
                                                                                & (1U 
                                                                                <= vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__tregs[9U]))) {
                                                                                vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT____Vlvbound_ha768cd16__0 
                                                                                = vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__tregs[10U];
                                                                                __VdlyVal__top__DOT__vgc_inst__DOT__tile_inst__DOT__col_buffer__v1 
                                                                                = vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT____Vlvbound_ha768cd16__0;
                                                                                __VdlySet__top__DOT__vgc_inst__DOT__tile_inst__DOT__col_buffer__v1 = 1U;
                                                                            }
                                                                            if (
                                                                                ((2U 
                                                                                >= vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__tregs[8U]) 
                                                                                & (2U 
                                                                                <= vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__tregs[9U]))) {
                                                                                vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT____Vlvbound_ha768cd16__0 
                                                                                = vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__tregs[10U];
                                                                                __VdlyVal__top__DOT__vgc_inst__DOT__tile_inst__DOT__col_buffer__v2 
                                                                                = vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT____Vlvbound_ha768cd16__0;
                                                                                __VdlySet__top__DOT__vgc_inst__DOT__tile_inst__DOT__col_buffer__v2 = 1U;
                                                                            }
                                                                            if (
                                                                                ((3U 
                                                                                >= vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__tregs[8U]) 
                                                                                & (3U 
                                                                                <= vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__tregs[9U]))) {
                                                                                vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT____Vlvbound_ha768cd16__0 
                                                                                = vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__tregs[10U];
                                                                                __VdlyVal__top__DOT__vgc_inst__DOT__tile_inst__DOT__col_buffer__v3 
                                                                                = vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT____Vlvbound_ha768cd16__0;
                                                                                __VdlySet__top__DOT__vgc_inst__DOT__tile_inst__DOT__col_buffer__v3 = 1U;
                                                                            }
                                                                            if (
                                                                                ((4U 
                                                                                >= vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__tregs[8U]) 
                                                                                & (4U 
                                                                                <= vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__tregs[9U]))) {
                                                                                vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT____Vlvbound_ha768cd16__0 
                                                                                = vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__tregs[10U];
                                                                                __VdlyVal__top__DOT__vgc_inst__DOT__tile_inst__DOT__col_buffer__v4 
                                                                                = vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT____Vlvbound_ha768cd16__0;
                                                                                __VdlySet__top__DOT__vgc_inst__DOT__tile_inst__DOT__col_buffer__v4 = 1U;
                                                                            }
                                                                            if (
                                                                                ((5U 
                                                                                >= vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__tregs[8U]) 
                                                                                & (5U 
                                                                                <= vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__tregs[9U]))) {
                                                                                vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT____Vlvbound_ha768cd16__0 
                                                                                = vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__tregs[10U];
                                                                                __VdlyVal__top__DOT__vgc_inst__DOT__tile_inst__DOT__col_buffer__v5 
                                                                                = vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT____Vlvbound_ha768cd16__0;
                                                                                __VdlySet__top__DOT__vgc_inst__DOT__tile_inst__DOT__col_buffer__v5 = 1U;
                                                                            }
                                                                            if (
                                                                                ((6U 
                                                                                >= vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__tregs[8U]) 
                                                                                & (6U 
                                                                                <= vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__tregs[9U]))) {
                                                                                vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT____Vlvbound_ha768cd16__0 
                                                                                = vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__tregs[10U];
                                                                                __VdlyVal__top__DOT__vgc_inst__DOT__tile_inst__DOT__col_buffer__v6 
                                                                                = vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT____Vlvbound_ha768cd16__0;
                                                                                __VdlySet__top__DOT__vgc_inst__DOT__tile_inst__DOT__col_buffer__v6 = 1U;
                                                                            }
                                                                            if (
                                                                                ((7U 
                                                                                >= vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__tregs[8U]) 
                                                                                & (7U 
                                                                                <= vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__tregs[9U]))) {
                                                                                vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT____Vlvbound_ha768cd16__0 
                                                                                = vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__tregs[10U];
                                                                                __VdlyVal__top__DOT__vgc_inst__DOT__tile_inst__DOT__col_buffer__v7 
                                                                                = vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT____Vlvbound_ha768cd16__0;
                                                                                __VdlySet__top__DOT__vgc_inst__DOT__tile_inst__DOT__col_buffer__v7 = 1U;
                                                                            }
                                                                            if (
                                                                                ((8U 
                                                                                >= vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__tregs[8U]) 
                                                                                & (8U 
                                                                                <= vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__tregs[9U]))) {
                                                                                vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT____Vlvbound_ha768cd16__0 
                                                                                = vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__tregs[10U];
                                                                                __VdlyVal__top__DOT__vgc_inst__DOT__tile_inst__DOT__col_buffer__v8 
                                                                                = vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT____Vlvbound_ha768cd16__0;
                                                                                __VdlySet__top__DOT__vgc_inst__DOT__tile_inst__DOT__col_buffer__v8 = 1U;
                                                                            }
                                                                            if (
                                                                                ((9U 
                                                                                >= vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__tregs[8U]) 
                                                                                & (9U 
                                                                                <= vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__tregs[9U]))) {
                                                                                vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT____Vlvbound_ha768cd16__0 
                                                                                = vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__tregs[10U];
                                                                                __VdlyVal__top__DOT__vgc_inst__DOT__tile_inst__DOT__col_buffer__v9 
                                                                                = vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT____Vlvbound_ha768cd16__0;
                                                                                __VdlySet__top__DOT__vgc_inst__DOT__tile_inst__DOT__col_buffer__v9 = 1U;
                                                                            }
                                                                            if (
                                                                                ((0x0aU 
                                                                                >= vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__tregs[8U]) 
                                                                                & (0x0aU 
                                                                                <= vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__tregs[9U]))) {
                                                                                vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT____Vlvbound_ha768cd16__0 
                                                                                = vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__tregs[10U];
                                                                                __VdlyVal__top__DOT__vgc_inst__DOT__tile_inst__DOT__col_buffer__v10 
                                                                                = vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT____Vlvbound_ha768cd16__0;
                                                                                __VdlySet__top__DOT__vgc_inst__DOT__tile_inst__DOT__col_buffer__v10 = 1U;
                                                                            }
                                                                            if (
                                                                                ((0x0bU 
                                                                                >= vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__tregs[8U]) 
                                                                                & (0x0bU 
                                                                                <= vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__tregs[9U]))) {
                                                                                vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT____Vlvbound_ha768cd16__0 
                                                                                = vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__tregs[10U];
                                                                                __VdlyVal__top__DOT__vgc_inst__DOT__tile_inst__DOT__col_buffer__v11 
                                                                                = vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT____Vlvbound_ha768cd16__0;
                                                                                __VdlySet__top__DOT__vgc_inst__DOT__tile_inst__DOT__col_buffer__v11 = 1U;
                                                                            }
                                                                            if (
                                                                                ((0x0cU 
                                                                                >= vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__tregs[8U]) 
                                                                                & (0x0cU 
                                                                                <= vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__tregs[9U]))) {
                                                                                vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT____Vlvbound_ha768cd16__0 
                                                                                = vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__tregs[10U];
                                                                                __VdlyVal__top__DOT__vgc_inst__DOT__tile_inst__DOT__col_buffer__v12 
                                                                                = vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT____Vlvbound_ha768cd16__0;
                                                                                __VdlySet__top__DOT__vgc_inst__DOT__tile_inst__DOT__col_buffer__v12 = 1U;
                                                                            }
                                                                            if (
                                                                                ((0x0dU 
                                                                                >= vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__tregs[8U]) 
                                                                                & (0x0dU 
                                                                                <= vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__tregs[9U]))) {
                                                                                vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT____Vlvbound_ha768cd16__0 
                                                                                = vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__tregs[10U];
                                                                                __VdlyVal__top__DOT__vgc_inst__DOT__tile_inst__DOT__col_buffer__v13 
                                                                                = vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT____Vlvbound_ha768cd16__0;
                                                                                __VdlySet__top__DOT__vgc_inst__DOT__tile_inst__DOT__col_buffer__v13 = 1U;
                                                                            }
                                                                            if (
                                                                                ((0x0eU 
                                                                                >= vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__tregs[8U]) 
                                                                                & (0x0eU 
                                                                                <= vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__tregs[9U]))) {
                                                                                vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT____Vlvbound_ha768cd16__0 
                                                                                = vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__tregs[10U];
                                                                                __VdlyVal__top__DOT__vgc_inst__DOT__tile_inst__DOT__col_buffer__v14 
                                                                                = vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT____Vlvbound_ha768cd16__0;
                                                                                __VdlySet__top__DOT__vgc_inst__DOT__tile_inst__DOT__col_buffer__v14 = 1U;
                                                                            }
                                                                            if (
                                                                                ((0x0fU 
                                                                                >= vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__tregs[8U]) 
                                                                                & (0x0fU 
                                                                                <= vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__tregs[9U]))) {
                                                                                vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT____Vlvbound_ha768cd16__0 
                                                                                = vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__tregs[10U];
                                                                                __VdlyVal__top__DOT__vgc_inst__DOT__tile_inst__DOT__col_buffer__v15 
                                                                                = vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT____Vlvbound_ha768cd16__0;
                                                                                __VdlySet__top__DOT__vgc_inst__DOT__tile_inst__DOT__col_buffer__v15 = 1U;
                                                                            }
                                                                            if (
                                                                                ((0x10U 
                                                                                >= vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__tregs[8U]) 
                                                                                & (0x10U 
                                                                                <= vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__tregs[9U]))) {
                                                                                vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT____Vlvbound_ha768cd16__0 
                                                                                = vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__tregs[10U];
                                                                                __VdlyVal__top__DOT__vgc_inst__DOT__tile_inst__DOT__col_buffer__v16 
                                                                                = vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT____Vlvbound_ha768cd16__0;
                                                                                __VdlySet__top__DOT__vgc_inst__DOT__tile_inst__DOT__col_buffer__v16 = 1U;
                                                                            }
                                                                            if (
                                                                                ((0x11U 
                                                                                >= vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__tregs[8U]) 
                                                                                & (0x11U 
                                                                                <= vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__tregs[9U]))) {
                                                                                vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT____Vlvbound_ha768cd16__0 
                                                                                = vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__tregs[10U];
                                                                                __VdlyVal__top__DOT__vgc_inst__DOT__tile_inst__DOT__col_buffer__v17 
                                                                                = vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT____Vlvbound_ha768cd16__0;
                                                                                __VdlySet__top__DOT__vgc_inst__DOT__tile_inst__DOT__col_buffer__v17 = 1U;
                                                                            }
                                                                            if (
                                                                                ((0x12U 
                                                                                >= vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__tregs[8U]) 
                                                                                & (0x12U 
                                                                                <= vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__tregs[9U]))) {
                                                                                vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT____Vlvbound_ha768cd16__0 
                                                                                = vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__tregs[10U];
                                                                                __VdlyVal__top__DOT__vgc_inst__DOT__tile_inst__DOT__col_buffer__v18 
                                                                                = vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT____Vlvbound_ha768cd16__0;
                                                                                __VdlySet__top__DOT__vgc_inst__DOT__tile_inst__DOT__col_buffer__v18 = 1U;
                                                                            }
                                                                            if (
                                                                                ((0x13U 
                                                                                >= vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__tregs[8U]) 
                                                                                & (0x13U 
                                                                                <= vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__tregs[9U]))) {
                                                                                vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT____Vlvbound_ha768cd16__0 
                                                                                = vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__tregs[10U];
                                                                                __VdlyVal__top__DOT__vgc_inst__DOT__tile_inst__DOT__col_buffer__v19 
                                                                                = vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT____Vlvbound_ha768cd16__0;
                                                                                __VdlySet__top__DOT__vgc_inst__DOT__tile_inst__DOT__col_buffer__v19 = 1U;
                                                                            }
                                                                            if (
                                                                                ((0x14U 
                                                                                >= vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__tregs[8U]) 
                                                                                & (0x14U 
                                                                                <= vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__tregs[9U]))) {
                                                                                vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT____Vlvbound_ha768cd16__0 
                                                                                = vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__tregs[10U];
                                                                                __VdlyVal__top__DOT__vgc_inst__DOT__tile_inst__DOT__col_buffer__v20 
                                                                                = vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT____Vlvbound_ha768cd16__0;
                                                                                __VdlySet__top__DOT__vgc_inst__DOT__tile_inst__DOT__col_buffer__v20 = 1U;
                                                                            }
                                                                            if (
                                                                                ((0x15U 
                                                                                >= vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__tregs[8U]) 
                                                                                & (0x15U 
                                                                                <= vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__tregs[9U]))) {
                                                                                vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT____Vlvbound_ha768cd16__0 
                                                                                = vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__tregs[10U];
                                                                                __VdlyVal__top__DOT__vgc_inst__DOT__tile_inst__DOT__col_buffer__v21 
                                                                                = vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT____Vlvbound_ha768cd16__0;
                                                                                __VdlySet__top__DOT__vgc_inst__DOT__tile_inst__DOT__col_buffer__v21 = 1U;
                                                                            }
                                                                            if (
                                                                                ((0x16U 
                                                                                >= vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__tregs[8U]) 
                                                                                & (0x16U 
                                                                                <= vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__tregs[9U]))) {
                                                                                vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT____Vlvbound_ha768cd16__0 
                                                                                = vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__tregs[10U];
                                                                                __VdlyVal__top__DOT__vgc_inst__DOT__tile_inst__DOT__col_buffer__v22 
                                                                                = vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT____Vlvbound_ha768cd16__0;
                                                                                __VdlySet__top__DOT__vgc_inst__DOT__tile_inst__DOT__col_buffer__v22 = 1U;
                                                                            }
                                                                            if (
                                                                                ((0x17U 
                                                                                >= vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__tregs[8U]) 
                                                                                & (0x17U 
                                                                                <= vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__tregs[9U]))) {
                                                                                vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT____Vlvbound_ha768cd16__0 
                                                                                = vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__tregs[10U];
                                                                                __VdlyVal__top__DOT__vgc_inst__DOT__tile_inst__DOT__col_buffer__v23 
                                                                                = vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT____Vlvbound_ha768cd16__0;
                                                                                __VdlySet__top__DOT__vgc_inst__DOT__tile_inst__DOT__col_buffer__v23 = 1U;
                                                                            }
                                                                            if (
                                                                                ((0x18U 
                                                                                >= vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__tregs[8U]) 
                                                                                & (0x18U 
                                                                                <= vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__tregs[9U]))) {
                                                                                vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT____Vlvbound_ha768cd16__0 
                                                                                = vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__tregs[10U];
                                                                                __VdlyVal__top__DOT__vgc_inst__DOT__tile_inst__DOT__col_buffer__v24 
                                                                                = vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT____Vlvbound_ha768cd16__0;
                                                                                __VdlySet__top__DOT__vgc_inst__DOT__tile_inst__DOT__col_buffer__v24 = 1U;
                                                                            }
                                                                        }
                                                                    } else if (
                                                                               (1U 
                                                                                & (IData)(vlSelfRef.top__DOT__cpu_dout))) {
                                                                        if (
                                                                            (0x19U 
                                                                             > vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__tregs[8U])) {
                                                                            vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT____Vlvbound_h67f0dfac__0 
                                                                                = vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__tregs[9U];
                                                                            if (
                                                                                (0x18U 
                                                                                >= 
                                                                                (0x0000001fU 
                                                                                & vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__tregs[8U]))) {
                                                                                __VdlyVal__top__DOT__vgc_inst__DOT__tile_inst__DOT__col_buffer__v25 
                                                                                = vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT____Vlvbound_h67f0dfac__0;
                                                                                __VdlyDim0__top__DOT__vgc_inst__DOT__tile_inst__DOT__col_buffer__v25 
                                                                                = 
                                                                                (0x0000001fU 
                                                                                & vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__tregs[8U]);
                                                                                __VdlySet__top__DOT__vgc_inst__DOT__tile_inst__DOT__col_buffer__v25 = 1U;
                                                                            }
                                                                        }
                                                                    } else {
                                                                        vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT____Vlvbound_hbbfa6b2d__0 
                                                                            = vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__tregs[8U];
                                                                        __VdlyVal__top__DOT__vgc_inst__DOT__tile_inst__DOT__col_buffer__v26 
                                                                            = vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT____Vlvbound_hbbfa6b2d__0;
                                                                        __VdlySet__top__DOT__vgc_inst__DOT__tile_inst__DOT__col_buffer__v26 = 1U;
                                                                        vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT____Vlvbound_hbbfa6b2d__0 
                                                                            = vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__tregs[8U];
                                                                        __VdlyVal__top__DOT__vgc_inst__DOT__tile_inst__DOT__col_buffer__v27 
                                                                            = vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT____Vlvbound_hbbfa6b2d__0;
                                                                        __VdlySet__top__DOT__vgc_inst__DOT__tile_inst__DOT__col_buffer__v27 = 1U;
                                                                        vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT____Vlvbound_hbbfa6b2d__0 
                                                                            = vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__tregs[8U];
                                                                        __VdlyVal__top__DOT__vgc_inst__DOT__tile_inst__DOT__col_buffer__v28 
                                                                            = vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT____Vlvbound_hbbfa6b2d__0;
                                                                        __VdlySet__top__DOT__vgc_inst__DOT__tile_inst__DOT__col_buffer__v28 = 1U;
                                                                        vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT____Vlvbound_hbbfa6b2d__0 
                                                                            = vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__tregs[8U];
                                                                        __VdlyVal__top__DOT__vgc_inst__DOT__tile_inst__DOT__col_buffer__v29 
                                                                            = vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT____Vlvbound_hbbfa6b2d__0;
                                                                        __VdlySet__top__DOT__vgc_inst__DOT__tile_inst__DOT__col_buffer__v29 = 1U;
                                                                        vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT____Vlvbound_hbbfa6b2d__0 
                                                                            = vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__tregs[8U];
                                                                        __VdlyVal__top__DOT__vgc_inst__DOT__tile_inst__DOT__col_buffer__v30 
                                                                            = vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT____Vlvbound_hbbfa6b2d__0;
                                                                        __VdlySet__top__DOT__vgc_inst__DOT__tile_inst__DOT__col_buffer__v30 = 1U;
                                                                        vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT____Vlvbound_hbbfa6b2d__0 
                                                                            = vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__tregs[8U];
                                                                        __VdlyVal__top__DOT__vgc_inst__DOT__tile_inst__DOT__col_buffer__v31 
                                                                            = vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT____Vlvbound_hbbfa6b2d__0;
                                                                        __VdlySet__top__DOT__vgc_inst__DOT__tile_inst__DOT__col_buffer__v31 = 1U;
                                                                        vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT____Vlvbound_hbbfa6b2d__0 
                                                                            = vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__tregs[8U];
                                                                        __VdlyVal__top__DOT__vgc_inst__DOT__tile_inst__DOT__col_buffer__v32 
                                                                            = vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT____Vlvbound_hbbfa6b2d__0;
                                                                        __VdlySet__top__DOT__vgc_inst__DOT__tile_inst__DOT__col_buffer__v32 = 1U;
                                                                        vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT____Vlvbound_hbbfa6b2d__0 
                                                                            = vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__tregs[8U];
                                                                        __VdlyVal__top__DOT__vgc_inst__DOT__tile_inst__DOT__col_buffer__v33 
                                                                            = vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT____Vlvbound_hbbfa6b2d__0;
                                                                        __VdlySet__top__DOT__vgc_inst__DOT__tile_inst__DOT__col_buffer__v33 = 1U;
                                                                        vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT____Vlvbound_hbbfa6b2d__0 
                                                                            = vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__tregs[8U];
                                                                        __VdlyVal__top__DOT__vgc_inst__DOT__tile_inst__DOT__col_buffer__v34 
                                                                            = vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT____Vlvbound_hbbfa6b2d__0;
                                                                        __VdlySet__top__DOT__vgc_inst__DOT__tile_inst__DOT__col_buffer__v34 = 1U;
                                                                        vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT____Vlvbound_hbbfa6b2d__0 
                                                                            = vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__tregs[8U];
                                                                        __VdlyVal__top__DOT__vgc_inst__DOT__tile_inst__DOT__col_buffer__v35 
                                                                            = vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT____Vlvbound_hbbfa6b2d__0;
                                                                        __VdlySet__top__DOT__vgc_inst__DOT__tile_inst__DOT__col_buffer__v35 = 1U;
                                                                        vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT____Vlvbound_hbbfa6b2d__0 
                                                                            = vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__tregs[8U];
                                                                        __VdlyVal__top__DOT__vgc_inst__DOT__tile_inst__DOT__col_buffer__v36 
                                                                            = vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT____Vlvbound_hbbfa6b2d__0;
                                                                        __VdlySet__top__DOT__vgc_inst__DOT__tile_inst__DOT__col_buffer__v36 = 1U;
                                                                        vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT____Vlvbound_hbbfa6b2d__0 
                                                                            = vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__tregs[8U];
                                                                        __VdlyVal__top__DOT__vgc_inst__DOT__tile_inst__DOT__col_buffer__v37 
                                                                            = vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT____Vlvbound_hbbfa6b2d__0;
                                                                        __VdlySet__top__DOT__vgc_inst__DOT__tile_inst__DOT__col_buffer__v37 = 1U;
                                                                        vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT____Vlvbound_hbbfa6b2d__0 
                                                                            = vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__tregs[8U];
                                                                        __VdlyVal__top__DOT__vgc_inst__DOT__tile_inst__DOT__col_buffer__v38 
                                                                            = vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT____Vlvbound_hbbfa6b2d__0;
                                                                        __VdlySet__top__DOT__vgc_inst__DOT__tile_inst__DOT__col_buffer__v38 = 1U;
                                                                        vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT____Vlvbound_hbbfa6b2d__0 
                                                                            = vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__tregs[8U];
                                                                        __VdlyVal__top__DOT__vgc_inst__DOT__tile_inst__DOT__col_buffer__v39 
                                                                            = vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT____Vlvbound_hbbfa6b2d__0;
                                                                        __VdlySet__top__DOT__vgc_inst__DOT__tile_inst__DOT__col_buffer__v39 = 1U;
                                                                        vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT____Vlvbound_hbbfa6b2d__0 
                                                                            = vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__tregs[8U];
                                                                        __VdlyVal__top__DOT__vgc_inst__DOT__tile_inst__DOT__col_buffer__v40 
                                                                            = vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT____Vlvbound_hbbfa6b2d__0;
                                                                        __VdlySet__top__DOT__vgc_inst__DOT__tile_inst__DOT__col_buffer__v40 = 1U;
                                                                        vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT____Vlvbound_hbbfa6b2d__0 
                                                                            = vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__tregs[8U];
                                                                        __VdlyVal__top__DOT__vgc_inst__DOT__tile_inst__DOT__col_buffer__v41 
                                                                            = vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT____Vlvbound_hbbfa6b2d__0;
                                                                        __VdlySet__top__DOT__vgc_inst__DOT__tile_inst__DOT__col_buffer__v41 = 1U;
                                                                        vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT____Vlvbound_hbbfa6b2d__0 
                                                                            = vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__tregs[8U];
                                                                        __VdlyVal__top__DOT__vgc_inst__DOT__tile_inst__DOT__col_buffer__v42 
                                                                            = vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT____Vlvbound_hbbfa6b2d__0;
                                                                        __VdlySet__top__DOT__vgc_inst__DOT__tile_inst__DOT__col_buffer__v42 = 1U;
                                                                        vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT____Vlvbound_hbbfa6b2d__0 
                                                                            = vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__tregs[8U];
                                                                        __VdlyVal__top__DOT__vgc_inst__DOT__tile_inst__DOT__col_buffer__v43 
                                                                            = vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT____Vlvbound_hbbfa6b2d__0;
                                                                        __VdlySet__top__DOT__vgc_inst__DOT__tile_inst__DOT__col_buffer__v43 = 1U;
                                                                        vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT____Vlvbound_hbbfa6b2d__0 
                                                                            = vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__tregs[8U];
                                                                        __VdlyVal__top__DOT__vgc_inst__DOT__tile_inst__DOT__col_buffer__v44 
                                                                            = vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT____Vlvbound_hbbfa6b2d__0;
                                                                        __VdlySet__top__DOT__vgc_inst__DOT__tile_inst__DOT__col_buffer__v44 = 1U;
                                                                        vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT____Vlvbound_hbbfa6b2d__0 
                                                                            = vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__tregs[8U];
                                                                        __VdlyVal__top__DOT__vgc_inst__DOT__tile_inst__DOT__col_buffer__v45 
                                                                            = vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT____Vlvbound_hbbfa6b2d__0;
                                                                        __VdlySet__top__DOT__vgc_inst__DOT__tile_inst__DOT__col_buffer__v45 = 1U;
                                                                        vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT____Vlvbound_hbbfa6b2d__0 
                                                                            = vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__tregs[8U];
                                                                        __VdlyVal__top__DOT__vgc_inst__DOT__tile_inst__DOT__col_buffer__v46 
                                                                            = vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT____Vlvbound_hbbfa6b2d__0;
                                                                        __VdlySet__top__DOT__vgc_inst__DOT__tile_inst__DOT__col_buffer__v46 = 1U;
                                                                        vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT____Vlvbound_hbbfa6b2d__0 
                                                                            = vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__tregs[8U];
                                                                        __VdlyVal__top__DOT__vgc_inst__DOT__tile_inst__DOT__col_buffer__v47 
                                                                            = vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT____Vlvbound_hbbfa6b2d__0;
                                                                        __VdlySet__top__DOT__vgc_inst__DOT__tile_inst__DOT__col_buffer__v47 = 1U;
                                                                        vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT____Vlvbound_hbbfa6b2d__0 
                                                                            = vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__tregs[8U];
                                                                        __VdlyVal__top__DOT__vgc_inst__DOT__tile_inst__DOT__col_buffer__v48 
                                                                            = vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT____Vlvbound_hbbfa6b2d__0;
                                                                        __VdlySet__top__DOT__vgc_inst__DOT__tile_inst__DOT__col_buffer__v48 = 1U;
                                                                        vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT____Vlvbound_hbbfa6b2d__0 
                                                                            = vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__tregs[8U];
                                                                        __VdlyVal__top__DOT__vgc_inst__DOT__tile_inst__DOT__col_buffer__v49 
                                                                            = vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT____Vlvbound_hbbfa6b2d__0;
                                                                        __VdlySet__top__DOT__vgc_inst__DOT__tile_inst__DOT__col_buffer__v49 = 1U;
                                                                        vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT____Vlvbound_hbbfa6b2d__0 
                                                                            = vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__tregs[8U];
                                                                        __VdlyVal__top__DOT__vgc_inst__DOT__tile_inst__DOT__col_buffer__v50 
                                                                            = vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT____Vlvbound_hbbfa6b2d__0;
                                                                        __VdlySet__top__DOT__vgc_inst__DOT__tile_inst__DOT__col_buffer__v50 = 1U;
                                                                    }
                                                                }
                                                            }
                                                        } else if (
                                                                   (8U 
                                                                    & (IData)(vlSelfRef.top__DOT__cpu_dout))) {
                                                            if (
                                                                (4U 
                                                                 & (IData)(vlSelfRef.top__DOT__cpu_dout))) {
                                                                if (
                                                                    (2U 
                                                                     & (IData)(vlSelfRef.top__DOT__cpu_dout))) {
                                                                    if (
                                                                        (1U 
                                                                         & (IData)(vlSelfRef.top__DOT__cpu_dout))) {
                                                                        vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__dma_fill_addr = 0U;
                                                                        __Vdly__top__DOT__vgc_inst__DOT__tile_inst__DOT__dma_state = 9U;
                                                                    } else {
                                                                        __Vdly__top__DOT__vgc_inst__DOT__tile_inst__DOT__dma_src 
                                                                            = 
                                                                            ((vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__tregs[13U] 
                                                                              << 8U) 
                                                                             | vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__tregs[12U]);
                                                                        vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__dma_nt 
                                                                            = 
                                                                            (3U 
                                                                             & vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__tregs[8U]);
                                                                        vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__dma_col_row 
                                                                            = 
                                                                            (0x0000003fU 
                                                                             & vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__tregs[9U]);
                                                                        __Vdly__top__DOT__vgc_inst__DOT__tile_inst__DOT__dma_col_idx = 0U;
                                                                        __Vdly__top__DOT__vgc_inst__DOT__tile_inst__DOT__dma_count = 0x0019U;
                                                                        __Vdly__top__DOT__vgc_inst__DOT__tile_inst__DOT__dma_state = 8U;
                                                                        __Vdly__top__DOT__vgc_inst__DOT__tile_inst__DOT__dma_data_valid = 0U;
                                                                    }
                                                                } else if (
                                                                           (1U 
                                                                            & (IData)(vlSelfRef.top__DOT__cpu_dout))) {
                                                                    __Vdly__top__DOT__vgc_inst__DOT__tile_inst__DOT__dma_src 
                                                                        = 
                                                                        ((vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__tregs[13U] 
                                                                          << 8U) 
                                                                         | vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__tregs[12U]);
                                                                    __Vdly__top__DOT__vgc_inst__DOT__tile_inst__DOT__dma_dst = 0U;
                                                                    vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__dma_nt 
                                                                        = 
                                                                        (3U 
                                                                         & vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__tregs[8U]);
                                                                    vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__dma_col_row 
                                                                        = 
                                                                        (0x0000003fU 
                                                                         & vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__tregs[9U]);
                                                                    __Vdly__top__DOT__vgc_inst__DOT__tile_inst__DOT__dma_count = 0x0028U;
                                                                    __Vdly__top__DOT__vgc_inst__DOT__tile_inst__DOT__dma_state = 7U;
                                                                    __Vdly__top__DOT__vgc_inst__DOT__tile_inst__DOT__dma_data_valid = 0U;
                                                                } else if (
                                                                           (((4U 
                                                                              > vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__tregs[8U]) 
                                                                             & (0x28U 
                                                                                > vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__tregs[9U])) 
                                                                            & (0x19U 
                                                                               > vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__tregs[10U]))) {
                                                                    vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__cmd_nt_addr 
                                                                        = 
                                                                        (0x00000fffU 
                                                                         & ((((IData)(0x000003e8U) 
                                                                              * vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__tregs[8U]) 
                                                                             + 
                                                                             ((IData)(0x00000028U) 
                                                                              * vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__tregs[10U])) 
                                                                            + vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__tregs[9U]));
                                                                    vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__cmd_at_addr 
                                                                        = 
                                                                        (0x00000fffU 
                                                                         & ((((IData)(0x000003e8U) 
                                                                              * vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__tregs[8U]) 
                                                                             + 
                                                                             ((IData)(0x00000028U) 
                                                                              * vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__tregs[10U])) 
                                                                            + vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__tregs[9U]));
                                                                    __Vdly__top__DOT__vgc_inst__DOT__tile_inst__DOT__dma_state = 0x0cU;
                                                                }
                                                            } else if (
                                                                       (2U 
                                                                        & (IData)(vlSelfRef.top__DOT__cpu_dout))) {
                                                                if (
                                                                    (1U 
                                                                     & (IData)(vlSelfRef.top__DOT__cpu_dout))) {
                                                                    vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__cmd_pal_addr_reg 
                                                                        = 
                                                                        ((0x000000f0U 
                                                                          & (vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__tregs[14U] 
                                                                             << 4U)) 
                                                                         | (0x0000000fU 
                                                                            & vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__tregs[15U]));
                                                                    vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__cmd_pal_din 
                                                                        = 
                                                                        ((0x00000f00U 
                                                                          & (vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__tregs[8U] 
                                                                             << 4U)) 
                                                                         | ((0x000000f0U 
                                                                             & vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__tregs[9U]) 
                                                                            | (0x0000000fU 
                                                                               & (vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__tregs[10U] 
                                                                                >> 4U))));
                                                                    vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__cmd_pal_we = 1U;
                                                                } else {
                                                                    __Vdly__top__DOT__vgc_inst__DOT__tile_inst__DOT__dma_src 
                                                                        = 
                                                                        ((vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__tregs[13U] 
                                                                          << 8U) 
                                                                         | vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__tregs[12U]);
                                                                    vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__dma_pal_cidx = 0U;
                                                                    vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__dma_pal_sub 
                                                                        = 
                                                                        (0x0000000fU 
                                                                         & vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__tregs[14U]);
                                                                    __Vdly__top__DOT__vgc_inst__DOT__tile_inst__DOT__dma_pal_phase = 0U;
                                                                    __Vdly__top__DOT__vgc_inst__DOT__tile_inst__DOT__dma_count = 0x0030U;
                                                                    __Vdly__top__DOT__vgc_inst__DOT__tile_inst__DOT__dma_state = 4U;
                                                                    __Vdly__top__DOT__vgc_inst__DOT__tile_inst__DOT__dma_data_valid = 0U;
                                                                }
                                                            } else if (
                                                                       (1U 
                                                                        & (IData)(vlSelfRef.top__DOT__cpu_dout))) {
                                                                __Vdly__top__DOT__vgc_inst__DOT__tile_inst__DOT__dma_src 
                                                                    = 
                                                                    ((vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__tregs[13U] 
                                                                      << 8U) 
                                                                     | vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__tregs[12U]);
                                                                __Vdly__top__DOT__vgc_inst__DOT__tile_inst__DOT__dma_dst = 0U;
                                                                vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__dma_nt 
                                                                    = 
                                                                    (3U 
                                                                     & vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__tregs[8U]);
                                                                __Vdly__top__DOT__vgc_inst__DOT__tile_inst__DOT__dma_count = 0x03e8U;
                                                                __Vdly__top__DOT__vgc_inst__DOT__tile_inst__DOT__dma_state = 3U;
                                                                __Vdly__top__DOT__vgc_inst__DOT__tile_inst__DOT__dma_data_valid = 0U;
                                                            } else {
                                                                __Vdly__top__DOT__vgc_inst__DOT__tile_inst__DOT__dma_src 
                                                                    = 
                                                                    ((vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__tregs[13U] 
                                                                      << 8U) 
                                                                     | vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__tregs[12U]);
                                                                __Vdly__top__DOT__vgc_inst__DOT__tile_inst__DOT__dma_dst = 0U;
                                                                vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__dma_nt 
                                                                    = 
                                                                    (3U 
                                                                     & vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__tregs[8U]);
                                                                __Vdly__top__DOT__vgc_inst__DOT__tile_inst__DOT__dma_count = 0x03e8U;
                                                                __Vdly__top__DOT__vgc_inst__DOT__tile_inst__DOT__dma_state = 2U;
                                                                __Vdly__top__DOT__vgc_inst__DOT__tile_inst__DOT__dma_data_valid = 0U;
                                                            }
                                                        } else if (
                                                                   (4U 
                                                                    & (IData)(vlSelfRef.top__DOT__cpu_dout))) {
                                                            if (
                                                                (2U 
                                                                 & (IData)(vlSelfRef.top__DOT__cpu_dout))) {
                                                                if (
                                                                    (1U 
                                                                     & (IData)(vlSelfRef.top__DOT__cpu_dout))) {
                                                                    __Vdly__top__DOT__vgc_inst__DOT__tile_inst__DOT__dma_src 
                                                                        = 
                                                                        ((vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__tregs[13U] 
                                                                          << 8U) 
                                                                         | vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__tregs[12U]);
                                                                    vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__dma_nt 
                                                                        = 
                                                                        (3U 
                                                                         & vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__tregs[8U]);
                                                                    vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__dma_col_row 
                                                                        = 
                                                                        (0x0000003fU 
                                                                         & vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__tregs[9U]);
                                                                    __Vdly__top__DOT__vgc_inst__DOT__tile_inst__DOT__dma_col_idx = 0U;
                                                                    __Vdly__top__DOT__vgc_inst__DOT__tile_inst__DOT__dma_count = 0x0019U;
                                                                    __Vdly__top__DOT__vgc_inst__DOT__tile_inst__DOT__dma_state = 6U;
                                                                    __Vdly__top__DOT__vgc_inst__DOT__tile_inst__DOT__dma_data_valid = 0U;
                                                                } else {
                                                                    __Vdly__top__DOT__vgc_inst__DOT__tile_inst__DOT__dma_src 
                                                                        = 
                                                                        ((vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__tregs[13U] 
                                                                          << 8U) 
                                                                         | vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__tregs[12U]);
                                                                    __Vdly__top__DOT__vgc_inst__DOT__tile_inst__DOT__dma_dst = 0U;
                                                                    vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__dma_nt 
                                                                        = 
                                                                        (3U 
                                                                         & vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__tregs[8U]);
                                                                    vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__dma_col_row 
                                                                        = 
                                                                        (0x0000003fU 
                                                                         & vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__tregs[9U]);
                                                                    __Vdly__top__DOT__vgc_inst__DOT__tile_inst__DOT__dma_count = 0x0028U;
                                                                    __Vdly__top__DOT__vgc_inst__DOT__tile_inst__DOT__dma_state = 5U;
                                                                    __Vdly__top__DOT__vgc_inst__DOT__tile_inst__DOT__dma_data_valid = 0U;
                                                                }
                                                            } else if (
                                                                       (1U 
                                                                        & (IData)(vlSelfRef.top__DOT__cpu_dout))) {
                                                                if (
                                                                    (4U 
                                                                     > vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__tregs[8U])) {
                                                                    vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__dma_nt 
                                                                        = 
                                                                        (3U 
                                                                         & vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__tregs[8U]);
                                                                    vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__dma_fill_val 
                                                                        = vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__tregs[9U];
                                                                    __Vdly__top__DOT__vgc_inst__DOT__tile_inst__DOT__dma_count = 0U;
                                                                    __Vdly__top__DOT__vgc_inst__DOT__tile_inst__DOT__dma_state = 0x0aU;
                                                                }
                                                            } else if (
                                                                       (((4U 
                                                                          > vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__tregs[8U]) 
                                                                         & (0x28U 
                                                                            > vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__tregs[9U])) 
                                                                        & (0x19U 
                                                                           > vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__tregs[10U]))) {
                                                                vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__cmd_at_addr 
                                                                    = 
                                                                    (0x00000fffU 
                                                                     & ((((IData)(0x000003e8U) 
                                                                          * vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__tregs[8U]) 
                                                                         + 
                                                                         ((IData)(0x00000028U) 
                                                                          * vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__tregs[10U])) 
                                                                        + vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__tregs[9U]));
                                                                vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__cmd_at_din 
                                                                    = vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__tregs[11U];
                                                                vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__cmd_at_we = 1U;
                                                            }
                                                        } else if (
                                                                   (2U 
                                                                    & (IData)(vlSelfRef.top__DOT__cpu_dout))) {
                                                            if (
                                                                (1U 
                                                                 & (IData)(vlSelfRef.top__DOT__cpu_dout))) {
                                                                if (
                                                                    (((4U 
                                                                       > vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__tregs[8U]) 
                                                                      & (0x28U 
                                                                         > vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__tregs[9U])) 
                                                                     & (0x19U 
                                                                        > vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__tregs[10U]))) {
                                                                    vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__cmd_nt_addr 
                                                                        = 
                                                                        (0x00000fffU 
                                                                         & ((((IData)(0x000003e8U) 
                                                                              * vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__tregs[8U]) 
                                                                             + 
                                                                             ((IData)(0x00000028U) 
                                                                              * vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__tregs[10U])) 
                                                                            + vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__tregs[9U]));
                                                                    vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__cmd_nt_din 
                                                                        = vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__tregs[11U];
                                                                    vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__cmd_nt_we = 1U;
                                                                }
                                                            } else {
                                                                __Vdly__top__DOT__vgc_inst__DOT__tile_inst__DOT__dma_src 
                                                                    = 
                                                                    ((vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__tregs[13U] 
                                                                      << 8U) 
                                                                     | vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__tregs[12U]);
                                                                if (vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__tile_size16) {
                                                                    __Vdly__top__DOT__vgc_inst__DOT__tile_inst__DOT__dma_dst 
                                                                        = 
                                                                        (vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__tregs[8U] 
                                                                         << 7U);
                                                                    __Vdly__top__DOT__vgc_inst__DOT__tile_inst__DOT__dma_count 
                                                                        = 
                                                                        (0x0000ffffU 
                                                                         & VL_SHIFTL_III(32,32,32, vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__tregs[9U], 7U));
                                                                } else {
                                                                    __Vdly__top__DOT__vgc_inst__DOT__tile_inst__DOT__dma_dst 
                                                                        = 
                                                                        (vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__tregs[8U] 
                                                                         << 5U);
                                                                    __Vdly__top__DOT__vgc_inst__DOT__tile_inst__DOT__dma_count 
                                                                        = 
                                                                        (0x0000ffffU 
                                                                         & VL_SHIFTL_III(32,32,32, vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__tregs[9U], 5U));
                                                                }
                                                                __Vdly__top__DOT__vgc_inst__DOT__tile_inst__DOT__dma_state = 1U;
                                                                __Vdly__top__DOT__vgc_inst__DOT__tile_inst__DOT__dma_data_valid = 0U;
                                                            }
                                                        } else if (
                                                                   (1U 
                                                                    & (IData)(vlSelfRef.top__DOT__cpu_dout))) {
                                                            __Vdly__top__DOT__vgc_inst__DOT__tile_inst__DOT__dma_src 
                                                                = 
                                                                ((vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__tregs[13U] 
                                                                  << 8U) 
                                                                 | vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__tregs[12U]);
                                                            if (vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__tile_size16) {
                                                                __Vdly__top__DOT__vgc_inst__DOT__tile_inst__DOT__dma_dst 
                                                                    = 
                                                                    (vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__tregs[8U] 
                                                                     << 7U);
                                                                __Vdly__top__DOT__vgc_inst__DOT__tile_inst__DOT__dma_count = 0x0080U;
                                                            } else {
                                                                __Vdly__top__DOT__vgc_inst__DOT__tile_inst__DOT__dma_dst 
                                                                    = 
                                                                    (vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__tregs[8U] 
                                                                     << 5U);
                                                                __Vdly__top__DOT__vgc_inst__DOT__tile_inst__DOT__dma_count = 0x0020U;
                                                            }
                                                            __Vdly__top__DOT__vgc_inst__DOT__tile_inst__DOT__dma_state = 1U;
                                                            __Vdly__top__DOT__vgc_inst__DOT__tile_inst__DOT__dma_data_valid = 0U;
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
            if ((0U == (0x0000001fU & (IData)(vlSelfRef.top__DOT__cpu_addr)))) {
                vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__mirror_mode 
                    = (3U & ((IData)(vlSelfRef.top__DOT__cpu_dout) 
                             >> 1U));
            }
        }
    }
    vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__dma_src 
        = __Vdly__top__DOT__vgc_inst__DOT__tile_inst__DOT__dma_src;
    if (__VdlySet__top__DOT__vgc_inst__DOT__tile_inst__DOT__tregs__v0) {
        vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__tregs[__VdlyDim0__top__DOT__vgc_inst__DOT__tile_inst__DOT__tregs__v0] 
            = __VdlyVal__top__DOT__vgc_inst__DOT__tile_inst__DOT__tregs__v0;
    }
    vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__dma_pal_phase 
        = __Vdly__top__DOT__vgc_inst__DOT__tile_inst__DOT__dma_pal_phase;
    vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__dma_dst 
        = __Vdly__top__DOT__vgc_inst__DOT__tile_inst__DOT__dma_dst;
    vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__dma_count 
        = __Vdly__top__DOT__vgc_inst__DOT__tile_inst__DOT__dma_count;
    vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__dma_col_idx 
        = __Vdly__top__DOT__vgc_inst__DOT__tile_inst__DOT__dma_col_idx;
    vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__dma_data_valid 
        = __Vdly__top__DOT__vgc_inst__DOT__tile_inst__DOT__dma_data_valid;
    if (__VdlySet__top__DOT__vgc_inst__DOT__tile_inst__DOT__col_buffer__v0) {
        vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__col_buffer[0U] 
            = __VdlyVal__top__DOT__vgc_inst__DOT__tile_inst__DOT__col_buffer__v0;
    }
    if (__VdlySet__top__DOT__vgc_inst__DOT__tile_inst__DOT__col_buffer__v1) {
        vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__col_buffer[1U] 
            = __VdlyVal__top__DOT__vgc_inst__DOT__tile_inst__DOT__col_buffer__v1;
    }
    if (__VdlySet__top__DOT__vgc_inst__DOT__tile_inst__DOT__col_buffer__v2) {
        vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__col_buffer[2U] 
            = __VdlyVal__top__DOT__vgc_inst__DOT__tile_inst__DOT__col_buffer__v2;
    }
    if (__VdlySet__top__DOT__vgc_inst__DOT__tile_inst__DOT__col_buffer__v3) {
        vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__col_buffer[3U] 
            = __VdlyVal__top__DOT__vgc_inst__DOT__tile_inst__DOT__col_buffer__v3;
    }
    if (__VdlySet__top__DOT__vgc_inst__DOT__tile_inst__DOT__col_buffer__v4) {
        vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__col_buffer[4U] 
            = __VdlyVal__top__DOT__vgc_inst__DOT__tile_inst__DOT__col_buffer__v4;
    }
    if (__VdlySet__top__DOT__vgc_inst__DOT__tile_inst__DOT__col_buffer__v5) {
        vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__col_buffer[5U] 
            = __VdlyVal__top__DOT__vgc_inst__DOT__tile_inst__DOT__col_buffer__v5;
    }
    if (__VdlySet__top__DOT__vgc_inst__DOT__tile_inst__DOT__col_buffer__v6) {
        vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__col_buffer[6U] 
            = __VdlyVal__top__DOT__vgc_inst__DOT__tile_inst__DOT__col_buffer__v6;
    }
    if (__VdlySet__top__DOT__vgc_inst__DOT__tile_inst__DOT__col_buffer__v7) {
        vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__col_buffer[7U] 
            = __VdlyVal__top__DOT__vgc_inst__DOT__tile_inst__DOT__col_buffer__v7;
    }
    if (__VdlySet__top__DOT__vgc_inst__DOT__tile_inst__DOT__col_buffer__v8) {
        vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__col_buffer[8U] 
            = __VdlyVal__top__DOT__vgc_inst__DOT__tile_inst__DOT__col_buffer__v8;
    }
    if (__VdlySet__top__DOT__vgc_inst__DOT__tile_inst__DOT__col_buffer__v9) {
        vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__col_buffer[9U] 
            = __VdlyVal__top__DOT__vgc_inst__DOT__tile_inst__DOT__col_buffer__v9;
    }
    if (__VdlySet__top__DOT__vgc_inst__DOT__tile_inst__DOT__col_buffer__v10) {
        vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__col_buffer[10U] 
            = __VdlyVal__top__DOT__vgc_inst__DOT__tile_inst__DOT__col_buffer__v10;
    }
    if (__VdlySet__top__DOT__vgc_inst__DOT__tile_inst__DOT__col_buffer__v11) {
        vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__col_buffer[11U] 
            = __VdlyVal__top__DOT__vgc_inst__DOT__tile_inst__DOT__col_buffer__v11;
    }
    if (__VdlySet__top__DOT__vgc_inst__DOT__tile_inst__DOT__col_buffer__v12) {
        vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__col_buffer[12U] 
            = __VdlyVal__top__DOT__vgc_inst__DOT__tile_inst__DOT__col_buffer__v12;
    }
    if (__VdlySet__top__DOT__vgc_inst__DOT__tile_inst__DOT__col_buffer__v13) {
        vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__col_buffer[13U] 
            = __VdlyVal__top__DOT__vgc_inst__DOT__tile_inst__DOT__col_buffer__v13;
    }
    if (__VdlySet__top__DOT__vgc_inst__DOT__tile_inst__DOT__col_buffer__v14) {
        vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__col_buffer[14U] 
            = __VdlyVal__top__DOT__vgc_inst__DOT__tile_inst__DOT__col_buffer__v14;
    }
    if (__VdlySet__top__DOT__vgc_inst__DOT__tile_inst__DOT__col_buffer__v15) {
        vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__col_buffer[15U] 
            = __VdlyVal__top__DOT__vgc_inst__DOT__tile_inst__DOT__col_buffer__v15;
    }
    if (__VdlySet__top__DOT__vgc_inst__DOT__tile_inst__DOT__col_buffer__v16) {
        vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__col_buffer[16U] 
            = __VdlyVal__top__DOT__vgc_inst__DOT__tile_inst__DOT__col_buffer__v16;
    }
    if (__VdlySet__top__DOT__vgc_inst__DOT__tile_inst__DOT__col_buffer__v17) {
        vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__col_buffer[17U] 
            = __VdlyVal__top__DOT__vgc_inst__DOT__tile_inst__DOT__col_buffer__v17;
    }
    if (__VdlySet__top__DOT__vgc_inst__DOT__tile_inst__DOT__col_buffer__v18) {
        vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__col_buffer[18U] 
            = __VdlyVal__top__DOT__vgc_inst__DOT__tile_inst__DOT__col_buffer__v18;
    }
    if (__VdlySet__top__DOT__vgc_inst__DOT__tile_inst__DOT__col_buffer__v19) {
        vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__col_buffer[19U] 
            = __VdlyVal__top__DOT__vgc_inst__DOT__tile_inst__DOT__col_buffer__v19;
    }
    if (__VdlySet__top__DOT__vgc_inst__DOT__tile_inst__DOT__col_buffer__v20) {
        vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__col_buffer[20U] 
            = __VdlyVal__top__DOT__vgc_inst__DOT__tile_inst__DOT__col_buffer__v20;
    }
    if (__VdlySet__top__DOT__vgc_inst__DOT__tile_inst__DOT__col_buffer__v21) {
        vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__col_buffer[21U] 
            = __VdlyVal__top__DOT__vgc_inst__DOT__tile_inst__DOT__col_buffer__v21;
    }
    if (__VdlySet__top__DOT__vgc_inst__DOT__tile_inst__DOT__col_buffer__v22) {
        vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__col_buffer[22U] 
            = __VdlyVal__top__DOT__vgc_inst__DOT__tile_inst__DOT__col_buffer__v22;
    }
    if (__VdlySet__top__DOT__vgc_inst__DOT__tile_inst__DOT__col_buffer__v23) {
        vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__col_buffer[23U] 
            = __VdlyVal__top__DOT__vgc_inst__DOT__tile_inst__DOT__col_buffer__v23;
    }
    if (__VdlySet__top__DOT__vgc_inst__DOT__tile_inst__DOT__col_buffer__v24) {
        vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__col_buffer[24U] 
            = __VdlyVal__top__DOT__vgc_inst__DOT__tile_inst__DOT__col_buffer__v24;
    }
    if (__VdlySet__top__DOT__vgc_inst__DOT__tile_inst__DOT__col_buffer__v25) {
        vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__col_buffer[__VdlyDim0__top__DOT__vgc_inst__DOT__tile_inst__DOT__col_buffer__v25] 
            = __VdlyVal__top__DOT__vgc_inst__DOT__tile_inst__DOT__col_buffer__v25;
    }
    if (__VdlySet__top__DOT__vgc_inst__DOT__tile_inst__DOT__col_buffer__v26) {
        vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__col_buffer[0U] 
            = __VdlyVal__top__DOT__vgc_inst__DOT__tile_inst__DOT__col_buffer__v26;
    }
    if (__VdlySet__top__DOT__vgc_inst__DOT__tile_inst__DOT__col_buffer__v27) {
        vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__col_buffer[1U] 
            = __VdlyVal__top__DOT__vgc_inst__DOT__tile_inst__DOT__col_buffer__v27;
    }
    if (__VdlySet__top__DOT__vgc_inst__DOT__tile_inst__DOT__col_buffer__v28) {
        vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__col_buffer[2U] 
            = __VdlyVal__top__DOT__vgc_inst__DOT__tile_inst__DOT__col_buffer__v28;
    }
    if (__VdlySet__top__DOT__vgc_inst__DOT__tile_inst__DOT__col_buffer__v29) {
        vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__col_buffer[3U] 
            = __VdlyVal__top__DOT__vgc_inst__DOT__tile_inst__DOT__col_buffer__v29;
    }
    if (__VdlySet__top__DOT__vgc_inst__DOT__tile_inst__DOT__col_buffer__v30) {
        vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__col_buffer[4U] 
            = __VdlyVal__top__DOT__vgc_inst__DOT__tile_inst__DOT__col_buffer__v30;
    }
    if (__VdlySet__top__DOT__vgc_inst__DOT__tile_inst__DOT__col_buffer__v31) {
        vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__col_buffer[5U] 
            = __VdlyVal__top__DOT__vgc_inst__DOT__tile_inst__DOT__col_buffer__v31;
    }
    if (__VdlySet__top__DOT__vgc_inst__DOT__tile_inst__DOT__col_buffer__v32) {
        vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__col_buffer[6U] 
            = __VdlyVal__top__DOT__vgc_inst__DOT__tile_inst__DOT__col_buffer__v32;
    }
    if (__VdlySet__top__DOT__vgc_inst__DOT__tile_inst__DOT__col_buffer__v33) {
        vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__col_buffer[7U] 
            = __VdlyVal__top__DOT__vgc_inst__DOT__tile_inst__DOT__col_buffer__v33;
    }
    if (__VdlySet__top__DOT__vgc_inst__DOT__tile_inst__DOT__col_buffer__v34) {
        vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__col_buffer[8U] 
            = __VdlyVal__top__DOT__vgc_inst__DOT__tile_inst__DOT__col_buffer__v34;
    }
    if (__VdlySet__top__DOT__vgc_inst__DOT__tile_inst__DOT__col_buffer__v35) {
        vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__col_buffer[9U] 
            = __VdlyVal__top__DOT__vgc_inst__DOT__tile_inst__DOT__col_buffer__v35;
    }
    if (__VdlySet__top__DOT__vgc_inst__DOT__tile_inst__DOT__col_buffer__v36) {
        vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__col_buffer[10U] 
            = __VdlyVal__top__DOT__vgc_inst__DOT__tile_inst__DOT__col_buffer__v36;
    }
    if (__VdlySet__top__DOT__vgc_inst__DOT__tile_inst__DOT__col_buffer__v37) {
        vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__col_buffer[11U] 
            = __VdlyVal__top__DOT__vgc_inst__DOT__tile_inst__DOT__col_buffer__v37;
    }
    if (__VdlySet__top__DOT__vgc_inst__DOT__tile_inst__DOT__col_buffer__v38) {
        vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__col_buffer[12U] 
            = __VdlyVal__top__DOT__vgc_inst__DOT__tile_inst__DOT__col_buffer__v38;
    }
    if (__VdlySet__top__DOT__vgc_inst__DOT__tile_inst__DOT__col_buffer__v39) {
        vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__col_buffer[13U] 
            = __VdlyVal__top__DOT__vgc_inst__DOT__tile_inst__DOT__col_buffer__v39;
    }
    if (__VdlySet__top__DOT__vgc_inst__DOT__tile_inst__DOT__col_buffer__v40) {
        vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__col_buffer[14U] 
            = __VdlyVal__top__DOT__vgc_inst__DOT__tile_inst__DOT__col_buffer__v40;
    }
    if (__VdlySet__top__DOT__vgc_inst__DOT__tile_inst__DOT__col_buffer__v41) {
        vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__col_buffer[15U] 
            = __VdlyVal__top__DOT__vgc_inst__DOT__tile_inst__DOT__col_buffer__v41;
    }
    if (__VdlySet__top__DOT__vgc_inst__DOT__tile_inst__DOT__col_buffer__v42) {
        vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__col_buffer[16U] 
            = __VdlyVal__top__DOT__vgc_inst__DOT__tile_inst__DOT__col_buffer__v42;
    }
    if (__VdlySet__top__DOT__vgc_inst__DOT__tile_inst__DOT__col_buffer__v43) {
        vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__col_buffer[17U] 
            = __VdlyVal__top__DOT__vgc_inst__DOT__tile_inst__DOT__col_buffer__v43;
    }
    if (__VdlySet__top__DOT__vgc_inst__DOT__tile_inst__DOT__col_buffer__v44) {
        vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__col_buffer[18U] 
            = __VdlyVal__top__DOT__vgc_inst__DOT__tile_inst__DOT__col_buffer__v44;
    }
    if (__VdlySet__top__DOT__vgc_inst__DOT__tile_inst__DOT__col_buffer__v45) {
        vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__col_buffer[19U] 
            = __VdlyVal__top__DOT__vgc_inst__DOT__tile_inst__DOT__col_buffer__v45;
    }
    if (__VdlySet__top__DOT__vgc_inst__DOT__tile_inst__DOT__col_buffer__v46) {
        vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__col_buffer[20U] 
            = __VdlyVal__top__DOT__vgc_inst__DOT__tile_inst__DOT__col_buffer__v46;
    }
    if (__VdlySet__top__DOT__vgc_inst__DOT__tile_inst__DOT__col_buffer__v47) {
        vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__col_buffer[21U] 
            = __VdlyVal__top__DOT__vgc_inst__DOT__tile_inst__DOT__col_buffer__v47;
    }
    if (__VdlySet__top__DOT__vgc_inst__DOT__tile_inst__DOT__col_buffer__v48) {
        vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__col_buffer[22U] 
            = __VdlyVal__top__DOT__vgc_inst__DOT__tile_inst__DOT__col_buffer__v48;
    }
    if (__VdlySet__top__DOT__vgc_inst__DOT__tile_inst__DOT__col_buffer__v49) {
        vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__col_buffer[23U] 
            = __VdlyVal__top__DOT__vgc_inst__DOT__tile_inst__DOT__col_buffer__v49;
    }
    if (__VdlySet__top__DOT__vgc_inst__DOT__tile_inst__DOT__col_buffer__v50) {
        vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__col_buffer[24U] 
            = __VdlyVal__top__DOT__vgc_inst__DOT__tile_inst__DOT__col_buffer__v50;
    }
    vlSelfRef.top__DOT__clear_input = (((~ (IData)(vlSelfRef.top__DOT__cpu_we)) 
                                        & (0xa00fU 
                                           == (IData)(vlSelfRef.top__DOT__cpu_addr))) 
                                       & (0U != vlSelfRef.top__DOT__ram[40975U]));
    vlSelfRef.top__DOT__vgc_inst__DOT__spr_shape_flat 
        = ((0xfffffffffffff000ULL & vlSelfRef.top__DOT__vgc_inst__DOT__spr_shape_flat) 
           | (IData)((IData)(((vlSelfRef.top__DOT__vgc_inst__DOT__spr_shape[2U] 
                               << 8U) | ((vlSelfRef.top__DOT__vgc_inst__DOT__spr_shape[1U] 
                                          << 4U) | vlSelfRef.top__DOT__vgc_inst__DOT__spr_shape[0U])))));
    vlSelfRef.top__DOT__vgc_inst__DOT__spr_shape_flat 
        = ((0xffffffffff000fffULL & vlSelfRef.top__DOT__vgc_inst__DOT__spr_shape_flat) 
           | ((QData)((IData)(((vlSelfRef.top__DOT__vgc_inst__DOT__spr_shape[5U] 
                                << 8U) | ((vlSelfRef.top__DOT__vgc_inst__DOT__spr_shape[4U] 
                                           << 4U) | vlSelfRef.top__DOT__vgc_inst__DOT__spr_shape[3U])))) 
              << 0x0000000cU));
    vlSelfRef.top__DOT__vgc_inst__DOT__spr_shape_flat 
        = ((0xfffffff000ffffffULL & vlSelfRef.top__DOT__vgc_inst__DOT__spr_shape_flat) 
           | ((QData)((IData)(((vlSelfRef.top__DOT__vgc_inst__DOT__spr_shape[8U] 
                                << 8U) | ((vlSelfRef.top__DOT__vgc_inst__DOT__spr_shape[7U] 
                                           << 4U) | vlSelfRef.top__DOT__vgc_inst__DOT__spr_shape[6U])))) 
              << 0x00000018U));
    vlSelfRef.top__DOT__vgc_inst__DOT__spr_shape_flat 
        = ((0xffff000fffffffffULL & vlSelfRef.top__DOT__vgc_inst__DOT__spr_shape_flat) 
           | ((QData)((IData)(((vlSelfRef.top__DOT__vgc_inst__DOT__spr_shape[11U] 
                                << 8U) | ((vlSelfRef.top__DOT__vgc_inst__DOT__spr_shape[10U] 
                                           << 4U) | vlSelfRef.top__DOT__vgc_inst__DOT__spr_shape[9U])))) 
              << 0x00000024U));
    vlSelfRef.top__DOT__vgc_inst__DOT__spr_shape_flat 
        = ((0xf000ffffffffffffULL & vlSelfRef.top__DOT__vgc_inst__DOT__spr_shape_flat) 
           | ((QData)((IData)(((vlSelfRef.top__DOT__vgc_inst__DOT__spr_shape[14U] 
                                << 8U) | ((vlSelfRef.top__DOT__vgc_inst__DOT__spr_shape[13U] 
                                           << 4U) | vlSelfRef.top__DOT__vgc_inst__DOT__spr_shape[12U])))) 
              << 0x00000030U));
    vlSelfRef.top__DOT__vgc_inst__DOT__spr_shape_flat 
        = ((0x0fffffffffffffffULL & vlSelfRef.top__DOT__vgc_inst__DOT__spr_shape_flat) 
           | ((QData)((IData)(vlSelfRef.top__DOT__vgc_inst__DOT__spr_shape[15U])) 
              << 0x0000003cU));
    vlSelfRef.top__DOT__vgc_inst__DOT__spr_trans_flat 
        = ((0xfffffffffffff000ULL & vlSelfRef.top__DOT__vgc_inst__DOT__spr_trans_flat) 
           | (IData)((IData)(((vlSelfRef.top__DOT__vgc_inst__DOT__spr_trans[2U] 
                               << 8U) | ((vlSelfRef.top__DOT__vgc_inst__DOT__spr_trans[1U] 
                                          << 4U) | vlSelfRef.top__DOT__vgc_inst__DOT__spr_trans[0U])))));
    vlSelfRef.top__DOT__vgc_inst__DOT__spr_trans_flat 
        = ((0xffffffffff000fffULL & vlSelfRef.top__DOT__vgc_inst__DOT__spr_trans_flat) 
           | ((QData)((IData)(((vlSelfRef.top__DOT__vgc_inst__DOT__spr_trans[5U] 
                                << 8U) | ((vlSelfRef.top__DOT__vgc_inst__DOT__spr_trans[4U] 
                                           << 4U) | vlSelfRef.top__DOT__vgc_inst__DOT__spr_trans[3U])))) 
              << 0x0000000cU));
    vlSelfRef.top__DOT__vgc_inst__DOT__spr_trans_flat 
        = ((0xfffffff000ffffffULL & vlSelfRef.top__DOT__vgc_inst__DOT__spr_trans_flat) 
           | ((QData)((IData)(((vlSelfRef.top__DOT__vgc_inst__DOT__spr_trans[8U] 
                                << 8U) | ((vlSelfRef.top__DOT__vgc_inst__DOT__spr_trans[7U] 
                                           << 4U) | vlSelfRef.top__DOT__vgc_inst__DOT__spr_trans[6U])))) 
              << 0x00000018U));
    vlSelfRef.top__DOT__vgc_inst__DOT__spr_trans_flat 
        = ((0xffff000fffffffffULL & vlSelfRef.top__DOT__vgc_inst__DOT__spr_trans_flat) 
           | ((QData)((IData)(((vlSelfRef.top__DOT__vgc_inst__DOT__spr_trans[11U] 
                                << 8U) | ((vlSelfRef.top__DOT__vgc_inst__DOT__spr_trans[10U] 
                                           << 4U) | vlSelfRef.top__DOT__vgc_inst__DOT__spr_trans[9U])))) 
              << 0x00000024U));
    vlSelfRef.top__DOT__vgc_inst__DOT__spr_trans_flat 
        = ((0xf000ffffffffffffULL & vlSelfRef.top__DOT__vgc_inst__DOT__spr_trans_flat) 
           | ((QData)((IData)(((vlSelfRef.top__DOT__vgc_inst__DOT__spr_trans[14U] 
                                << 8U) | ((vlSelfRef.top__DOT__vgc_inst__DOT__spr_trans[13U] 
                                           << 4U) | vlSelfRef.top__DOT__vgc_inst__DOT__spr_trans[12U])))) 
              << 0x00000030U));
    vlSelfRef.top__DOT__vgc_inst__DOT__spr_trans_flat 
        = ((0x0fffffffffffffffULL & vlSelfRef.top__DOT__vgc_inst__DOT__spr_trans_flat) 
           | ((QData)((IData)(vlSelfRef.top__DOT__vgc_inst__DOT__spr_trans[15U])) 
              << 0x0000003cU));
    vlSelfRef.top__DOT__vgc_inst__DOT__sprite_inst__DOT__slb_a_we = 0U;
    vlSelfRef.top__DOT__cpu_inst__DOT__NMI_1 = (1U 
                                                & (~ (IData)(vlSelfRef.nmi_n)));
    top__DOT__blt_inst__DOT__src_addr = (0x00ffffffU 
                                         & (vlSelfRef.top__DOT__blt_inst__DOT__src_base 
                                            + (((IData)(vlSelfRef.top__DOT__blt_inst__DOT__row) 
                                                * (IData)(vlSelfRef.top__DOT__blt_inst__DOT__src_stride)) 
                                               + (IData)(vlSelfRef.top__DOT__blt_inst__DOT__col))));
    top__DOT__blt_inst__DOT__dst_addr = (0x00ffffffU 
                                         & (vlSelfRef.top__DOT__blt_inst__DOT__dst_base 
                                            + (((IData)(vlSelfRef.top__DOT__blt_inst__DOT__row) 
                                                * (IData)(vlSelfRef.top__DOT__blt_inst__DOT__dst_stride)) 
                                               + (IData)(vlSelfRef.top__DOT__blt_inst__DOT__col))));
    vlSelfRef.top__DOT__blt_ram_we = 0U;
    vlSelfRef.top__DOT__blt_ram_wdata = 0U;
    vlSelfRef.top__DOT__blt_xram_we = 0U;
    vlSelfRef.top__DOT__blt_xram_wdata = 0U;
    top__DOT__blt_vgc_wdata = 0U;
    vlSelfRef.top__DOT__blt_vgc_we = 0U;
    vlSelfRef.top__DOT__vgc_inst__DOT__color_b_dout 
        = ((0x07cfU >= (IData)(vlSelfRef.top__DOT__vgc_inst__DOT__text_inst__DOT__char_b_addr))
            ? vlSelfRef.top__DOT__vgc_inst__DOT__text_inst__DOT__color_mem__DOT__mem
           [vlSelfRef.top__DOT__vgc_inst__DOT__text_inst__DOT__char_b_addr]
            : 0U);
    if (((((3U == (IData)(vlSelfRef.top__DOT__blt_inst__DOT__state)) 
           & (IData)(vlSelfRef.top__DOT__blt_inst__DOT__read_valid)) 
          & (~ ((IData)(vlSelfRef.top__DOT__blt_inst__DOT__colorkey_mode) 
                & ((IData)(vlSelfRef.top__DOT__blt_inst__DOT__read_byte) 
                   == (IData)(vlSelfRef.top__DOT__blt_inst__DOT__color_key))))) 
         | ((5U == (IData)(vlSelfRef.top__DOT__blt_inst__DOT__state)) 
            & (~ ((IData)(vlSelfRef.top__DOT__blt_inst__DOT__colorkey_mode) 
                  & (vlSelfRef.top__DOT__blt_inst__DOT__row_buf
                     [vlSelfRef.top__DOT__blt_inst__DOT__buf_idx] 
                     == (IData)(vlSelfRef.top__DOT__blt_inst__DOT__color_key))))))) {
        if ((0U == (IData)(vlSelfRef.top__DOT__blt_inst__DOT__dst_space))) {
            vlSelfRef.top__DOT__blt_ram_we = 1U;
            vlSelfRef.top__DOT__blt_ram_wdata = ((5U 
                                                  == (IData)(vlSelfRef.top__DOT__blt_inst__DOT__state))
                                                  ? vlSelfRef.top__DOT__blt_inst__DOT__row_buf
                                                 [vlSelfRef.top__DOT__blt_inst__DOT__buf_idx]
                                                  : (IData)(vlSelfRef.top__DOT__blt_inst__DOT__read_byte));
        }
        if ((0U != (IData)(vlSelfRef.top__DOT__blt_inst__DOT__dst_space))) {
            if ((5U == (IData)(vlSelfRef.top__DOT__blt_inst__DOT__dst_space))) {
                vlSelfRef.top__DOT__blt_xram_we = 1U;
                vlSelfRef.top__DOT__blt_xram_wdata 
                    = ((5U == (IData)(vlSelfRef.top__DOT__blt_inst__DOT__state))
                        ? vlSelfRef.top__DOT__blt_inst__DOT__row_buf
                       [vlSelfRef.top__DOT__blt_inst__DOT__buf_idx]
                        : (IData)(vlSelfRef.top__DOT__blt_inst__DOT__read_byte));
            }
            if ((5U != (IData)(vlSelfRef.top__DOT__blt_inst__DOT__dst_space))) {
                top__DOT__blt_vgc_wdata = ((5U == (IData)(vlSelfRef.top__DOT__blt_inst__DOT__state))
                                            ? vlSelfRef.top__DOT__blt_inst__DOT__row_buf
                                           [vlSelfRef.top__DOT__blt_inst__DOT__buf_idx]
                                            : (IData)(vlSelfRef.top__DOT__blt_inst__DOT__read_byte));
                vlSelfRef.top__DOT__blt_vgc_we = 1U;
            }
        }
    }
    if ((1U & (~ (IData)(vlSelfRef.rst)))) {
        if (vlSelfRef.top__DOT__vgc_inst__DOT__dbg_rd_pending) {
            if ((1U == (IData)(vlSelfRef.top__DOT__vgc_inst__DOT__dbg_rd_source))) {
                vlSelfRef.top__DOT__vgc_inst__DOT__dbg_rd_latch 
                    = vlSelfRef.top__DOT__vgc_inst__DOT__char_a_dout;
            } else if ((2U == (IData)(vlSelfRef.top__DOT__vgc_inst__DOT__dbg_rd_source))) {
                vlSelfRef.top__DOT__vgc_inst__DOT__dbg_rd_latch 
                    = vlSelfRef.top__DOT__vgc_inst__DOT__color_a_dout;
            }
        }
        if (vlSelfRef.top__DOT__vgc_inst__DOT__blt_rd_pending) {
            if ((4U & (IData)(vlSelfRef.top__DOT__vgc_inst__DOT__blt_rd_space))) {
                if ((2U & (IData)(vlSelfRef.top__DOT__vgc_inst__DOT__blt_rd_space))) {
                    if ((1U & (~ (IData)(vlSelfRef.top__DOT__vgc_inst__DOT__blt_rd_space)))) {
                        vlSelfRef.top__DOT__vgc_inst__DOT__blt_rd_latch 
                            = vlSelfRef.top__DOT__vgc_inst__DOT__tile_blt_rdata;
                    }
                } else if ((1U & (~ (IData)(vlSelfRef.top__DOT__vgc_inst__DOT__blt_rd_space)))) {
                    vlSelfRef.top__DOT__vgc_inst__DOT__blt_rd_latch 
                        = vlSelfRef.top__DOT__vgc_inst__DOT__spr_a_dout;
                }
            } else if ((2U & (IData)(vlSelfRef.top__DOT__vgc_inst__DOT__blt_rd_space))) {
                vlSelfRef.top__DOT__vgc_inst__DOT__blt_rd_latch 
                    = ((1U & (IData)(vlSelfRef.top__DOT__vgc_inst__DOT__blt_rd_space))
                        ? (IData)(vlSelfRef.top__DOT__vgc_inst__DOT__gfx_a_dout)
                        : (IData)(vlSelfRef.top__DOT__vgc_inst__DOT__color_a_dout));
            } else if ((1U & (IData)(vlSelfRef.top__DOT__vgc_inst__DOT__blt_rd_space))) {
                vlSelfRef.top__DOT__vgc_inst__DOT__blt_rd_latch 
                    = vlSelfRef.top__DOT__vgc_inst__DOT__char_a_dout;
            }
        }
        if (vlSelfRef.top__DOT____Vcellinp__cpu_inst__RDY) {
            if (((0x1eU == (IData)(vlSelfRef.top__DOT__cpu_inst__DOT__state)) 
                 | (0x21U == (IData)(vlSelfRef.top__DOT__cpu_inst__DOT__state)))) {
                vlSelfRef.top__DOT__cpu_inst__DOT__IRHOLD 
                    = vlSelfRef.top__DOT__cpu_inst__DOT__DIMUX;
            }
        }
    }
    if ((0x27U == (IData)(vlSelfRef.top__DOT__cpu_inst__DOT__state))) {
        vlSelfRef.top__DOT__cpu_inst__DOT__V = (1U 
                                                & ((IData)(vlSelfRef.top__DOT__cpu_inst__DOT__DIMUX) 
                                                   >> 6U));
    } else if ((0x0cU == (IData)(vlSelfRef.top__DOT__cpu_inst__DOT__state))) {
        if (vlSelfRef.top__DOT__cpu_inst__DOT__adc_sbc) {
            vlSelfRef.top__DOT__cpu_inst__DOT__V = 
                ((IData)(vlSelfRef.top__DOT__cpu_inst__DOT__ALU__DOT__AI7) 
                 ^ ((IData)(vlSelfRef.top__DOT__cpu_inst__DOT__ALU__DOT__BI7) 
                    ^ ((IData)(vlSelfRef.top__DOT__cpu_inst__DOT__AN) 
                       ^ (IData)(vlSelfRef.top__DOT__cpu_inst__DOT__CO))));
        }
        if (vlSelfRef.top__DOT__cpu_inst__DOT__clv) {
            vlSelfRef.top__DOT__cpu_inst__DOT__V = 0U;
        }
        if (vlSelfRef.top__DOT__cpu_inst__DOT__plp) {
            vlSelfRef.top__DOT__cpu_inst__DOT__V = 
                (1U & ((IData)(vlSelfRef.top__DOT__cpu_inst__DOT__ADD) 
                       >> 6U));
        }
    } else if (((0x0dU == (IData)(vlSelfRef.top__DOT__cpu_inst__DOT__state)) 
                & (IData)(vlSelfRef.top__DOT__cpu_inst__DOT__bit_ins))) {
        vlSelfRef.top__DOT__cpu_inst__DOT__V = (1U 
                                                & ((IData)(vlSelfRef.top__DOT__cpu_inst__DOT__DIMUX) 
                                                   >> 6U));
    }
    if (vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__blt_tile_rd_pending) {
        vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__blt_tile_rd_latch 
            = vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__td_dout_a;
    }
    if (vlSelfRef.rst) {
        __Vdly__top__DOT__vgc_inst__DOT__sprite_inst__DOT__spr_eval_state = 0U;
        __Vdly__top__DOT__vgc_inst__DOT__sprite_inst__DOT__spr_eval_idx = 0U;
        __Vdly__top__DOT__vgc_inst__DOT__sprite_inst__DOT__spr_clear_x = 0U;
    } else if ((4U & (IData)(vlSelfRef.top__DOT__vgc_inst__DOT__sprite_inst__DOT__spr_eval_state))) {
        if ((2U & (IData)(vlSelfRef.top__DOT__vgc_inst__DOT__sprite_inst__DOT__spr_eval_state))) {
            if ((1U & (IData)(vlSelfRef.top__DOT__vgc_inst__DOT__sprite_inst__DOT__spr_eval_state))) {
                __Vdly__top__DOT__vgc_inst__DOT__sprite_inst__DOT__spr_eval_state = 0U;
            } else {
                vlSelfRef.top__DOT__vgc_inst__DOT__sprite_inst__DOT__unnamedblk3__DOT__fx 
                    = (0x0000000fU & ((IData)(vlSelfRef.top__DOT__vgc_inst__DOT__sprite_inst__DOT__spr_eval_flip_h)
                                       ? ((IData)(0x0fU) 
                                          - (IData)(vlSelfRef.top__DOT__vgc_inst__DOT__sprite_inst__DOT__spr_write_px))
                                       : (IData)(vlSelfRef.top__DOT__vgc_inst__DOT__sprite_inst__DOT__spr_write_px)));
                vlSelfRef.top__DOT__vgc_inst__DOT__sprite_inst__DOT__unnamedblk3__DOT__color 
                    = (0x0000000fU & ((1U & (IData)(vlSelfRef.top__DOT__vgc_inst__DOT__sprite_inst__DOT__unnamedblk3__DOT__fx))
                                       ? vlSelfRef.top__DOT__vgc_inst__DOT__sprite_inst__DOT__spr_row_data
                                      [(7U & ((IData)(vlSelfRef.top__DOT__vgc_inst__DOT__sprite_inst__DOT__unnamedblk3__DOT__fx) 
                                              >> 1U))]
                                       : (vlSelfRef.top__DOT__vgc_inst__DOT__sprite_inst__DOT__spr_row_data
                                          [(7U & ((IData)(vlSelfRef.top__DOT__vgc_inst__DOT__sprite_inst__DOT__unnamedblk3__DOT__fx) 
                                                  >> 1U))] 
                                          >> 4U)));
                vlSelfRef.top__DOT__vgc_inst__DOT__sprite_inst__DOT__unnamedblk3__DOT__screen_x 
                    = (0x000001ffU & ((IData)(vlSelfRef.top__DOT__vgc_inst__DOT__sprite_inst__DOT__spr_eval_x_r) 
                                      + (0x0000000fU 
                                         & (IData)(vlSelfRef.top__DOT__vgc_inst__DOT__sprite_inst__DOT__spr_write_px))));
                if (((0x0140U > (IData)(vlSelfRef.top__DOT__vgc_inst__DOT__sprite_inst__DOT__unnamedblk3__DOT__screen_x)) 
                     & ((IData)(vlSelfRef.top__DOT__vgc_inst__DOT__sprite_inst__DOT__unnamedblk3__DOT__color) 
                        != (IData)(vlSelfRef.top__DOT__vgc_inst__DOT__sprite_inst__DOT__spr_eval_trans_r)))) {
                    vlSelfRef.top__DOT__vgc_inst__DOT__sprite_inst__DOT__slb_a_addr 
                        = vlSelfRef.top__DOT__vgc_inst__DOT__sprite_inst__DOT__unnamedblk3__DOT__screen_x;
                    vlSelfRef.top__DOT__vgc_inst__DOT__sprite_inst__DOT__slb_a_din 
                        = (1U | (((IData)(vlSelfRef.top__DOT__vgc_inst__DOT__sprite_inst__DOT__spr_eval_pri_r) 
                                  << 5U) | ((IData)(vlSelfRef.top__DOT__vgc_inst__DOT__sprite_inst__DOT__unnamedblk3__DOT__color) 
                                            << 1U)));
                    vlSelfRef.top__DOT__vgc_inst__DOT__sprite_inst__DOT__slb_a_we = 1U;
                }
                if ((0x0fU == (IData)(vlSelfRef.top__DOT__vgc_inst__DOT__sprite_inst__DOT__spr_write_px))) {
                    __Vdly__top__DOT__vgc_inst__DOT__sprite_inst__DOT__spr_eval_idx 
                        = (0x0000000fU & ((IData)(1U) 
                                          + (IData)(vlSelfRef.top__DOT__vgc_inst__DOT__sprite_inst__DOT__spr_eval_idx)));
                    __Vdly__top__DOT__vgc_inst__DOT__sprite_inst__DOT__spr_eval_state = 2U;
                } else {
                    vlSelfRef.top__DOT__vgc_inst__DOT__sprite_inst__DOT__spr_write_px 
                        = (0x0000001fU & ((IData)(1U) 
                                          + (IData)(vlSelfRef.top__DOT__vgc_inst__DOT__sprite_inst__DOT__spr_write_px)));
                }
            }
        } else if ((1U & (IData)(vlSelfRef.top__DOT__vgc_inst__DOT__sprite_inst__DOT__spr_eval_state))) {
            __Vdly__top__DOT__vgc_inst__DOT__sprite_inst__DOT__spr_eval_state = 0U;
        } else {
            __VdlyVal__top__DOT__vgc_inst__DOT__sprite_inst__DOT__spr_row_data__v0 
                = vlSelfRef.top__DOT__vgc_inst__DOT__sprite_inst__DOT__spr_b_dout;
            __VdlySet__top__DOT__vgc_inst__DOT__sprite_inst__DOT__spr_row_data__v0 = 1U;
            vlSelfRef.top__DOT__vgc_inst__DOT__sprite_inst__DOT__spr_write_px = 0U;
            __Vdly__top__DOT__vgc_inst__DOT__sprite_inst__DOT__spr_eval_state = 6U;
        }
    } else if ((2U & (IData)(vlSelfRef.top__DOT__vgc_inst__DOT__sprite_inst__DOT__spr_eval_state))) {
        if ((1U & (IData)(vlSelfRef.top__DOT__vgc_inst__DOT__sprite_inst__DOT__spr_eval_state))) {
            vlSelfRef.top__DOT__vgc_inst__DOT__sprite_inst__DOT__unnamedblk2__DOT__fy 
                = (0x0000000fU & ((IData)(vlSelfRef.top__DOT__vgc_inst__DOT__sprite_inst__DOT__spr_eval_flip_v)
                                   ? ((IData)(0x0fU) 
                                      - (IData)(vlSelfRef.top__DOT__vgc_inst__DOT__sprite_inst__DOT__spr_eval_y_line))
                                   : (IData)(vlSelfRef.top__DOT__vgc_inst__DOT__sprite_inst__DOT__spr_eval_y_line)));
            __Vdly__top__DOT__vgc_inst__DOT__sprite_inst__DOT__spr_b_addr 
                = (((IData)(vlSelfRef.top__DOT__vgc_inst__DOT__sprite_inst__DOT__spr_eval_shape_r) 
                    << 7U) | (((IData)(vlSelfRef.top__DOT__vgc_inst__DOT__sprite_inst__DOT__unnamedblk2__DOT__fy) 
                               << 3U) | (IData)(vlSelfRef.top__DOT__vgc_inst__DOT__sprite_inst__DOT__spr_read_byte)));
            if ((0U < (IData)(vlSelfRef.top__DOT__vgc_inst__DOT__sprite_inst__DOT__spr_read_byte))) {
                __VdlyVal__top__DOT__vgc_inst__DOT__sprite_inst__DOT__spr_row_data__v1 
                    = vlSelfRef.top__DOT__vgc_inst__DOT__sprite_inst__DOT__spr_b_dout;
                __VdlyDim0__top__DOT__vgc_inst__DOT__sprite_inst__DOT__spr_row_data__v1 
                    = (7U & ((IData)(vlSelfRef.top__DOT__vgc_inst__DOT__sprite_inst__DOT__spr_read_byte) 
                             - (IData)(1U)));
                __VdlySet__top__DOT__vgc_inst__DOT__sprite_inst__DOT__spr_row_data__v1 = 1U;
            }
            if ((7U == (IData)(vlSelfRef.top__DOT__vgc_inst__DOT__sprite_inst__DOT__spr_read_byte))) {
                __Vdly__top__DOT__vgc_inst__DOT__sprite_inst__DOT__spr_eval_state = 4U;
            }
            vlSelfRef.top__DOT__vgc_inst__DOT__sprite_inst__DOT__spr_read_byte 
                = (7U & ((IData)(1U) + (IData)(vlSelfRef.top__DOT__vgc_inst__DOT__sprite_inst__DOT__spr_read_byte)));
        } else if (vlSelfRef.top__DOT__vgc_inst__DOT__sprite_inst__DOT__spr_enable
                   [vlSelfRef.top__DOT__vgc_inst__DOT__sprite_inst__DOT__spr_eval_idx]) {
            if ((((IData)(vlSelfRef.top__DOT__vgc_inst__DOT__sprite_inst__DOT__spr_next_scanline) 
                  >= vlSelfRef.top__DOT__vgc_inst__DOT__sprite_inst__DOT__spr_y
                  [vlSelfRef.top__DOT__vgc_inst__DOT__sprite_inst__DOT__spr_eval_idx]) 
                 & ((IData)(vlSelfRef.top__DOT__vgc_inst__DOT__sprite_inst__DOT__spr_next_scanline) 
                    < ((IData)(0x00000010U) + vlSelfRef.top__DOT__vgc_inst__DOT__sprite_inst__DOT__spr_y
                       [vlSelfRef.top__DOT__vgc_inst__DOT__sprite_inst__DOT__spr_eval_idx])))) {
                vlSelfRef.top__DOT__vgc_inst__DOT__sprite_inst__DOT__spr_eval_y_line 
                    = (0x000000ffU & ((IData)(vlSelfRef.top__DOT__vgc_inst__DOT__sprite_inst__DOT__spr_next_scanline) 
                                      - vlSelfRef.top__DOT__vgc_inst__DOT__sprite_inst__DOT__spr_y
                                      [vlSelfRef.top__DOT__vgc_inst__DOT__sprite_inst__DOT__spr_eval_idx]));
                vlSelfRef.top__DOT__vgc_inst__DOT__sprite_inst__DOT__spr_read_byte = 0U;
                __Vdly__top__DOT__vgc_inst__DOT__sprite_inst__DOT__spr_eval_state = 3U;
                vlSelfRef.top__DOT__vgc_inst__DOT__sprite_inst__DOT__spr_eval_flip_h 
                    = vlSelfRef.top__DOT__vgc_inst__DOT__sprite_inst__DOT__spr_flip_h
                    [vlSelfRef.top__DOT__vgc_inst__DOT__sprite_inst__DOT__spr_eval_idx];
                vlSelfRef.top__DOT__vgc_inst__DOT__sprite_inst__DOT__spr_eval_flip_v 
                    = vlSelfRef.top__DOT__vgc_inst__DOT__sprite_inst__DOT__spr_flip_v
                    [vlSelfRef.top__DOT__vgc_inst__DOT__sprite_inst__DOT__spr_eval_idx];
                vlSelfRef.top__DOT__vgc_inst__DOT__sprite_inst__DOT__spr_eval_pri_r 
                    = vlSelfRef.top__DOT__vgc_inst__DOT__sprite_inst__DOT__spr_pri
                    [vlSelfRef.top__DOT__vgc_inst__DOT__sprite_inst__DOT__spr_eval_idx];
                vlSelfRef.top__DOT__vgc_inst__DOT__sprite_inst__DOT__spr_eval_x_r 
                    = vlSelfRef.top__DOT__vgc_inst__DOT__sprite_inst__DOT__spr_x
                    [vlSelfRef.top__DOT__vgc_inst__DOT__sprite_inst__DOT__spr_eval_idx];
                vlSelfRef.top__DOT__vgc_inst__DOT__sprite_inst__DOT__spr_eval_shape_r 
                    = vlSelfRef.top__DOT__vgc_inst__DOT__sprite_inst__DOT__spr_shape
                    [vlSelfRef.top__DOT__vgc_inst__DOT__sprite_inst__DOT__spr_eval_idx];
                vlSelfRef.top__DOT__vgc_inst__DOT__sprite_inst__DOT__spr_eval_trans_r 
                    = vlSelfRef.top__DOT__vgc_inst__DOT__sprite_inst__DOT__spr_trans
                    [vlSelfRef.top__DOT__vgc_inst__DOT__sprite_inst__DOT__spr_eval_idx];
            } else {
                __Vdly__top__DOT__vgc_inst__DOT__sprite_inst__DOT__spr_eval_idx 
                    = (0x0000000fU & ((IData)(1U) + (IData)(vlSelfRef.top__DOT__vgc_inst__DOT__sprite_inst__DOT__spr_eval_idx)));
            }
        } else {
            __Vdly__top__DOT__vgc_inst__DOT__sprite_inst__DOT__spr_eval_idx 
                = (0x0000000fU & ((IData)(1U) + (IData)(vlSelfRef.top__DOT__vgc_inst__DOT__sprite_inst__DOT__spr_eval_idx)));
        }
    } else if ((1U & (IData)(vlSelfRef.top__DOT__vgc_inst__DOT__sprite_inst__DOT__spr_eval_state))) {
        vlSelfRef.top__DOT__vgc_inst__DOT__sprite_inst__DOT__slb_a_addr 
            = vlSelfRef.top__DOT__vgc_inst__DOT__sprite_inst__DOT__spr_clear_x;
        vlSelfRef.top__DOT__vgc_inst__DOT__sprite_inst__DOT__slb_a_din = 0U;
        vlSelfRef.top__DOT__vgc_inst__DOT__sprite_inst__DOT__slb_a_we = 1U;
        if ((0x013fU <= (IData)(vlSelfRef.top__DOT__vgc_inst__DOT__sprite_inst__DOT__spr_clear_x))) {
            __Vdly__top__DOT__vgc_inst__DOT__sprite_inst__DOT__spr_eval_state = 2U;
            __Vdly__top__DOT__vgc_inst__DOT__sprite_inst__DOT__spr_eval_idx = 0U;
        } else {
            __Vdly__top__DOT__vgc_inst__DOT__sprite_inst__DOT__spr_clear_x 
                = (0x000001ffU & ((IData)(1U) + (IData)(vlSelfRef.top__DOT__vgc_inst__DOT__sprite_inst__DOT__spr_clear_x)));
        }
    } else if ((((0x0280U == (IData)(vlSelfRef.top__DOT__vgc_inst__DOT__h_count)) 
                 & (0x0028U <= (IData)(vlSelfRef.top__DOT__vgc_inst__DOT__v_count))) 
                & (0x01b7U > (IData)(vlSelfRef.top__DOT__vgc_inst__DOT__v_count)))) {
        __Vdly__top__DOT__vgc_inst__DOT__sprite_inst__DOT__spr_eval_state = 1U;
        __Vdly__top__DOT__vgc_inst__DOT__sprite_inst__DOT__spr_clear_x = 0U;
        vlSelfRef.top__DOT__vgc_inst__DOT__sprite_inst__DOT__spr_next_scanline 
            = (0x000000ffU & (((0x000003ffU & ((IData)(1U) 
                                               + (IData)(vlSelfRef.top__DOT__vgc_inst__DOT__v_count))) 
                               - (IData)(0x00000028U)) 
                              >> 1U));
    }
    if (vlSelfRef.top__DOT____Vcellinp__cpu_inst__RDY) {
        vlSelfRef.top__DOT__cpu_inst__DOT__cond_code 
            = (7U & ((IData)(vlSelfRef.top__DOT__cpu_inst__DOT__IR) 
                     >> 5U));
        vlSelfRef.top__DOT__cpu_inst__DOT__PC = (0x0000ffffU 
                                                 & ((IData)(vlSelfRef.top__DOT__cpu_inst__DOT__PC_temp) 
                                                    + (IData)(vlSelfRef.top__DOT__cpu_inst__DOT__PC_inc)));
        vlSelfRef.top__DOT__cpu_inst__DOT__DIHOLD = vlSelfRef.top__DOT__cpu_din;
    }
    if (((0x0cU == (IData)(vlSelfRef.top__DOT__cpu_inst__DOT__state)) 
         & (IData)(vlSelfRef.top__DOT____Vcellinp__cpu_inst__RDY))) {
        vlSelfRef.top__DOT__cpu_inst__DOT__php = (8U 
                                                  == (IData)(vlSelfRef.top__DOT__cpu_inst__DOT__IR));
        vlSelfRef.top__DOT__cpu_inst__DOT__shift_right 
            = (0x42U == (0xc3U & (IData)(vlSelfRef.top__DOT__cpu_inst__DOT__IR)));
        vlSelfRef.top__DOT__cpu_inst__DOT__inc = ((0xe6U 
                                                   == 
                                                   (0xe7U 
                                                    & (IData)(vlSelfRef.top__DOT__cpu_inst__DOT__IR))) 
                                                  || (0xc8U 
                                                      == 
                                                      (0xdfU 
                                                       & (IData)(vlSelfRef.top__DOT__cpu_inst__DOT__IR))));
        vlSelfRef.top__DOT__cpu_inst__DOT__rotate = 
            ((0x2aU == (0xafU & (IData)(vlSelfRef.top__DOT__cpu_inst__DOT__IR))) 
             || (0x26U == (0xa7U & (IData)(vlSelfRef.top__DOT__cpu_inst__DOT__IR))));
        vlSelfRef.top__DOT__cpu_inst__DOT__op = ((0x00000080U 
                                                  & (IData)(vlSelfRef.top__DOT__cpu_inst__DOT__IR))
                                                  ? 
                                                 ((0x00000040U 
                                                   & (IData)(vlSelfRef.top__DOT__cpu_inst__DOT__IR))
                                                   ? 
                                                  ((0x00000020U 
                                                    & (IData)(vlSelfRef.top__DOT__cpu_inst__DOT__IR))
                                                    ? 
                                                   ((0x00000010U 
                                                     & (IData)(vlSelfRef.top__DOT__cpu_inst__DOT__IR))
                                                     ? 
                                                    ((2U 
                                                      & (IData)(vlSelfRef.top__DOT__cpu_inst__DOT__IR))
                                                      ? 3U
                                                      : 
                                                     ((1U 
                                                       & (IData)(vlSelfRef.top__DOT__cpu_inst__DOT__IR))
                                                       ? 7U
                                                       : 3U))
                                                     : 
                                                    ((8U 
                                                      & (IData)(vlSelfRef.top__DOT__cpu_inst__DOT__IR))
                                                      ? 
                                                     ((4U 
                                                       & (IData)(vlSelfRef.top__DOT__cpu_inst__DOT__IR))
                                                       ? 
                                                      ((2U 
                                                        & (IData)(vlSelfRef.top__DOT__cpu_inst__DOT__IR))
                                                        ? 3U
                                                        : 7U)
                                                       : 
                                                      ((2U 
                                                        & (IData)(vlSelfRef.top__DOT__cpu_inst__DOT__IR))
                                                        ? 3U
                                                        : 
                                                       ((1U 
                                                         & (IData)(vlSelfRef.top__DOT__cpu_inst__DOT__IR))
                                                         ? 7U
                                                         : 3U)))
                                                      : 
                                                     ((2U 
                                                       & (IData)(vlSelfRef.top__DOT__cpu_inst__DOT__IR))
                                                       ? 3U
                                                       : 7U)))
                                                    : 
                                                   ((0x00000010U 
                                                     & (IData)(vlSelfRef.top__DOT__cpu_inst__DOT__IR))
                                                     ? 
                                                    ((4U 
                                                      & (IData)(vlSelfRef.top__DOT__cpu_inst__DOT__IR))
                                                      ? 
                                                     ((2U 
                                                       & (IData)(vlSelfRef.top__DOT__cpu_inst__DOT__IR))
                                                       ? 
                                                      ((1U 
                                                        & (IData)(vlSelfRef.top__DOT__cpu_inst__DOT__IR))
                                                        ? 3U
                                                        : 7U)
                                                       : 
                                                      ((1U 
                                                        & (IData)(vlSelfRef.top__DOT__cpu_inst__DOT__IR))
                                                        ? 7U
                                                        : 3U))
                                                      : 
                                                     ((2U 
                                                       & (IData)(vlSelfRef.top__DOT__cpu_inst__DOT__IR))
                                                       ? 3U
                                                       : 
                                                      ((1U 
                                                        & (IData)(vlSelfRef.top__DOT__cpu_inst__DOT__IR))
                                                        ? 7U
                                                        : 3U)))
                                                     : 
                                                    ((8U 
                                                      & (IData)(vlSelfRef.top__DOT__cpu_inst__DOT__IR))
                                                      ? 
                                                     ((4U 
                                                       & (IData)(vlSelfRef.top__DOT__cpu_inst__DOT__IR))
                                                       ? 
                                                      ((2U 
                                                        & (IData)(vlSelfRef.top__DOT__cpu_inst__DOT__IR))
                                                        ? 
                                                       ((1U 
                                                         & (IData)(vlSelfRef.top__DOT__cpu_inst__DOT__IR))
                                                         ? 3U
                                                         : 7U)
                                                        : 7U)
                                                       : 
                                                      ((2U 
                                                        & (IData)(vlSelfRef.top__DOT__cpu_inst__DOT__IR))
                                                        ? 
                                                       ((1U 
                                                         & (IData)(vlSelfRef.top__DOT__cpu_inst__DOT__IR))
                                                         ? 3U
                                                         : 7U)
                                                        : 
                                                       ((1U 
                                                         & (IData)(vlSelfRef.top__DOT__cpu_inst__DOT__IR))
                                                         ? 7U
                                                         : 3U)))
                                                      : 
                                                     ((4U 
                                                       & (IData)(vlSelfRef.top__DOT__cpu_inst__DOT__IR))
                                                       ? 
                                                      ((2U 
                                                        & (IData)(vlSelfRef.top__DOT__cpu_inst__DOT__IR))
                                                        ? 
                                                       ((1U 
                                                         & (IData)(vlSelfRef.top__DOT__cpu_inst__DOT__IR))
                                                         ? 3U
                                                         : 7U)
                                                        : 7U)
                                                       : 
                                                      ((2U 
                                                        & (IData)(vlSelfRef.top__DOT__cpu_inst__DOT__IR))
                                                        ? 3U
                                                        : 7U)))))
                                                   : 
                                                  ((0x00000020U 
                                                    & (IData)(vlSelfRef.top__DOT__cpu_inst__DOT__IR))
                                                    ? 3U
                                                    : 
                                                   ((0x00000010U 
                                                     & (IData)(vlSelfRef.top__DOT__cpu_inst__DOT__IR))
                                                     ? 3U
                                                     : 
                                                    ((8U 
                                                      & (IData)(vlSelfRef.top__DOT__cpu_inst__DOT__IR))
                                                      ? 
                                                     ((4U 
                                                       & (IData)(vlSelfRef.top__DOT__cpu_inst__DOT__IR))
                                                       ? 3U
                                                       : 
                                                      ((2U 
                                                        & (IData)(vlSelfRef.top__DOT__cpu_inst__DOT__IR))
                                                        ? 3U
                                                        : 
                                                       ((1U 
                                                         & (IData)(vlSelfRef.top__DOT__cpu_inst__DOT__IR))
                                                         ? 3U
                                                         : 7U)))
                                                      : 3U))))
                                                  : 
                                                 ((0x00000040U 
                                                   & (IData)(vlSelfRef.top__DOT__cpu_inst__DOT__IR))
                                                   ? 
                                                  ((0x00000020U 
                                                    & (IData)(vlSelfRef.top__DOT__cpu_inst__DOT__IR))
                                                    ? 
                                                   ((2U 
                                                     & (IData)(vlSelfRef.top__DOT__cpu_inst__DOT__IR))
                                                     ? 
                                                    ((1U 
                                                      & (IData)(vlSelfRef.top__DOT__cpu_inst__DOT__IR))
                                                      ? 3U
                                                      : 0x0fU)
                                                     : 3U)
                                                    : 
                                                   ((2U 
                                                     & (IData)(vlSelfRef.top__DOT__cpu_inst__DOT__IR))
                                                     ? 
                                                    ((1U 
                                                      & (IData)(vlSelfRef.top__DOT__cpu_inst__DOT__IR))
                                                      ? 3U
                                                      : 0x0fU)
                                                     : 
                                                    ((1U 
                                                      & (IData)(vlSelfRef.top__DOT__cpu_inst__DOT__IR))
                                                      ? 
                                                     (0x0000000cU 
                                                      | (3U 
                                                         & ((IData)(vlSelfRef.top__DOT__cpu_inst__DOT__IR) 
                                                            >> 5U)))
                                                      : 3U)))
                                                   : 
                                                  ((0x00000020U 
                                                    & (IData)(vlSelfRef.top__DOT__cpu_inst__DOT__IR))
                                                    ? 
                                                   ((0x00000010U 
                                                     & (IData)(vlSelfRef.top__DOT__cpu_inst__DOT__IR))
                                                     ? 
                                                    ((2U 
                                                      & (IData)(vlSelfRef.top__DOT__cpu_inst__DOT__IR))
                                                      ? 
                                                     ((1U 
                                                       & (IData)(vlSelfRef.top__DOT__cpu_inst__DOT__IR))
                                                       ? 3U
                                                       : 0x0bU)
                                                      : 
                                                     ((1U 
                                                       & (IData)(vlSelfRef.top__DOT__cpu_inst__DOT__IR))
                                                       ? 
                                                      (0x0000000cU 
                                                       | (3U 
                                                          & ((IData)(vlSelfRef.top__DOT__cpu_inst__DOT__IR) 
                                                             >> 5U)))
                                                       : 3U))
                                                     : 
                                                    ((4U 
                                                      & (IData)(vlSelfRef.top__DOT__cpu_inst__DOT__IR))
                                                      ? 
                                                     ((2U 
                                                       & (IData)(vlSelfRef.top__DOT__cpu_inst__DOT__IR))
                                                       ? 
                                                      ((1U 
                                                        & (IData)(vlSelfRef.top__DOT__cpu_inst__DOT__IR))
                                                        ? 3U
                                                        : 0x0bU)
                                                       : 
                                                      ((1U 
                                                        & (IData)(vlSelfRef.top__DOT__cpu_inst__DOT__IR))
                                                        ? 
                                                       (0x0000000cU 
                                                        | (3U 
                                                           & ((IData)(vlSelfRef.top__DOT__cpu_inst__DOT__IR) 
                                                              >> 5U)))
                                                        : 0x0dU))
                                                      : 
                                                     ((2U 
                                                       & (IData)(vlSelfRef.top__DOT__cpu_inst__DOT__IR))
                                                       ? 
                                                      ((1U 
                                                        & (IData)(vlSelfRef.top__DOT__cpu_inst__DOT__IR))
                                                        ? 3U
                                                        : 0x0bU)
                                                       : 
                                                      ((1U 
                                                        & (IData)(vlSelfRef.top__DOT__cpu_inst__DOT__IR))
                                                        ? 
                                                       (0x0000000cU 
                                                        | (3U 
                                                           & ((IData)(vlSelfRef.top__DOT__cpu_inst__DOT__IR) 
                                                              >> 5U)))
                                                        : 3U))))
                                                    : 
                                                   ((2U 
                                                     & (IData)(vlSelfRef.top__DOT__cpu_inst__DOT__IR))
                                                     ? 
                                                    ((1U 
                                                      & (IData)(vlSelfRef.top__DOT__cpu_inst__DOT__IR))
                                                      ? 3U
                                                      : 0x0bU)
                                                     : 
                                                    ((1U 
                                                      & (IData)(vlSelfRef.top__DOT__cpu_inst__DOT__IR))
                                                      ? 
                                                     (0x0000000cU 
                                                      | (3U 
                                                         & ((IData)(vlSelfRef.top__DOT__cpu_inst__DOT__IR) 
                                                            >> 5U)))
                                                      : 3U)))));
        vlSelfRef.top__DOT__cpu_inst__DOT__load_only 
            = (0xa0U == (0xe0U & (IData)(vlSelfRef.top__DOT__cpu_inst__DOT__IR)));
        vlSelfRef.top__DOT__cpu_inst__DOT__dst_reg 
            = ((((0xe8U == (IData)(vlSelfRef.top__DOT__cpu_inst__DOT__IR)) 
                 || (0xcaU == (IData)(vlSelfRef.top__DOT__cpu_inst__DOT__IR))) 
                || (0xa2U == (0xe3U & (IData)(vlSelfRef.top__DOT__cpu_inst__DOT__IR))))
                ? 2U : (((8U == (0xbfU & (IData)(vlSelfRef.top__DOT__cpu_inst__DOT__IR))) 
                         || (0x9aU == (IData)(vlSelfRef.top__DOT__cpu_inst__DOT__IR)))
                         ? 1U : ((((0x88U == (0xbfU 
                                              & (IData)(vlSelfRef.top__DOT__cpu_inst__DOT__IR))) 
                                   || (0xa4U == (0xe7U 
                                                 & (IData)(vlSelfRef.top__DOT__cpu_inst__DOT__IR)))) 
                                  || (0xa0U == (0xf7U 
                                                & (IData)(vlSelfRef.top__DOT__cpu_inst__DOT__IR))))
                                  ? 3U : 0U)));
        vlSelfRef.top__DOT__cpu_inst__DOT__src_reg 
            = ((0xbaU == (IData)(vlSelfRef.top__DOT__cpu_inst__DOT__IR))
                ? 1U : (((((0x86U == (0xe7U & (IData)(vlSelfRef.top__DOT__cpu_inst__DOT__IR))) 
                           || (0x8aU == (0xebU & (IData)(vlSelfRef.top__DOT__cpu_inst__DOT__IR)))) 
                          || (0xe0U == (0xf3U & (IData)(vlSelfRef.top__DOT__cpu_inst__DOT__IR)))) 
                         || (0xcaU == (IData)(vlSelfRef.top__DOT__cpu_inst__DOT__IR)))
                         ? 2U : (((((0x84U == (0xe7U 
                                               & (IData)(vlSelfRef.top__DOT__cpu_inst__DOT__IR))) 
                                    || (0x98U == (IData)(vlSelfRef.top__DOT__cpu_inst__DOT__IR))) 
                                   || (0xc0U == (0xf3U 
                                                 & (IData)(vlSelfRef.top__DOT__cpu_inst__DOT__IR)))) 
                                  || (0x88U == (0xbfU 
                                                & (IData)(vlSelfRef.top__DOT__cpu_inst__DOT__IR))))
                                  ? 3U : 0U)));
        vlSelfRef.top__DOT__cpu_inst__DOT__index_y 
            = (((0x11U == (0x1fU & (IData)(vlSelfRef.top__DOT__cpu_inst__DOT__IR))) 
                || (0x96U == (0xd7U & (IData)(vlSelfRef.top__DOT__cpu_inst__DOT__IR)))) 
               || (9U == (0x0fU & (IData)(vlSelfRef.top__DOT__cpu_inst__DOT__IR))));
    }
    if (vlSelfRef.rst) {
        __Vdly__top__DOT__vgc_inst__DOT__artist_busy = 0U;
        __Vdly__top__DOT__vgc_inst__DOT__artist_inst__DOT__op = 0U;
        __Vdly__top__DOT__vgc_inst__DOT__artist_inst__DOT__fill_sp = 0U;
        __Vdly__top__DOT__vgc_inst__DOT__artist_inst__DOT__octant_count = 0U;
        __Vdly__top__DOT__vgc_inst__DOT__artist_inst__DOT__paint_phase = 0U;
        __Vdly__top__DOT__vgc_inst__DOT__artist_inst__DOT__gt_font_pending = 0U;
        __Vdly__top__DOT__vgc_inst__DOT__artist_inst__DOT__paint_init = 0U;
        vlSelfRef.top__DOT__vgc_inst__DOT__artist_gfx_we = 0U;
        vlSelfRef.top__DOT__vgc_inst__DOT__artist_gfx_re = 0U;
        vlSelfRef.top__DOT__vgc_inst__DOT__artist_font_re = 0U;
        vlSelfRef.top__DOT__vgc_inst__DOT__artist_inst__DOT__fs_a_we = 0U;
    } else {
        vlSelfRef.top__DOT__vgc_inst__DOT__artist_gfx_we = 0U;
        vlSelfRef.top__DOT__vgc_inst__DOT__artist_gfx_re = 0U;
        vlSelfRef.top__DOT__vgc_inst__DOT__artist_font_re = 0U;
        vlSelfRef.top__DOT__vgc_inst__DOT__artist_inst__DOT__fs_a_we = 0U;
        if ((1U == (IData)(vlSelfRef.top__DOT__vgc_inst__DOT__artist_inst__DOT__gt_font_pending))) {
            __Vdly__top__DOT__vgc_inst__DOT__artist_inst__DOT__gt_font_pending = 2U;
        } else if ((2U == (IData)(vlSelfRef.top__DOT__vgc_inst__DOT__artist_inst__DOT__gt_font_pending))) {
            __Vdly__top__DOT__vgc_inst__DOT__artist_inst__DOT__gt_font_byte 
                = vlSelfRef.top__DOT__vgc_inst__DOT__font_a_dout;
            __Vdly__top__DOT__vgc_inst__DOT__artist_inst__DOT__gt_font_pending = 0U;
        }
        if (vlSelfRef.top__DOT__vgc_inst__DOT__artist_busy) {
            if ((3U == (IData)(vlSelfRef.top__DOT__vgc_inst__DOT__artist_inst__DOT__op))) {
                if ((((VL_LTES_III(32, 0U, VL_EXTENDS_II(32,10, (IData)(vlSelfRef.top__DOT__vgc_inst__DOT__artist_inst__DOT__x))) 
                       & VL_GTS_III(32, 0x00000140U, 
                                    VL_EXTENDS_II(32,10, (IData)(vlSelfRef.top__DOT__vgc_inst__DOT__artist_inst__DOT__x)))) 
                      & VL_LTES_III(32, 0U, VL_EXTENDS_II(32,10, (IData)(vlSelfRef.top__DOT__vgc_inst__DOT__artist_inst__DOT__y)))) 
                     & VL_GTS_III(32, 0x000000c8U, 
                                  VL_EXTENDS_II(32,10, (IData)(vlSelfRef.top__DOT__vgc_inst__DOT__artist_inst__DOT__y))))) {
                    vlSelfRef.top__DOT__vgc_inst__DOT__artist_gfx_addr 
                        = (0x0000ffffU & (VL_MULS_III(32, (IData)(0x00000140U), 
                                                      VL_EXTENDS_II(32,10, (IData)(vlSelfRef.top__DOT__vgc_inst__DOT__artist_inst__DOT__y))) 
                                          + VL_EXTENDS_II(16,10, (IData)(vlSelfRef.top__DOT__vgc_inst__DOT__artist_inst__DOT__x))));
                    vlSelfRef.top__DOT__vgc_inst__DOT__artist_gfx_wdata 
                        = vlSelfRef.top__DOT__vgc_inst__DOT__artist_inst__DOT__color;
                    vlSelfRef.top__DOT__vgc_inst__DOT__artist_gfx_we = 1U;
                }
                if ((((((((IData)(vlSelfRef.top__DOT__vgc_inst__DOT__artist_inst__DOT__x) 
                          == (IData)(vlSelfRef.top__DOT__vgc_inst__DOT__artist_inst__DOT__x2)) 
                         & ((IData)(vlSelfRef.top__DOT__vgc_inst__DOT__artist_inst__DOT__y) 
                            == (IData)(vlSelfRef.top__DOT__vgc_inst__DOT__artist_inst__DOT__y2))) 
                        | VL_GTS_III(32, 0xfffffff0U, 
                                     VL_EXTENDS_II(32,10, (IData)(vlSelfRef.top__DOT__vgc_inst__DOT__artist_inst__DOT__x)))) 
                       | VL_LTS_III(32, 0x00000150U, 
                                    VL_EXTENDS_II(32,10, (IData)(vlSelfRef.top__DOT__vgc_inst__DOT__artist_inst__DOT__x)))) 
                      | VL_GTS_III(32, 0xfffffff0U, 
                                   VL_EXTENDS_II(32,10, (IData)(vlSelfRef.top__DOT__vgc_inst__DOT__artist_inst__DOT__y)))) 
                     | VL_LTS_III(32, 0x000000d8U, 
                                  VL_EXTENDS_II(32,10, (IData)(vlSelfRef.top__DOT__vgc_inst__DOT__artist_inst__DOT__y))))) {
                    __Vdly__top__DOT__vgc_inst__DOT__artist_busy = 0U;
                } else if ((VL_GTES_III(32, VL_MULS_III(32, (IData)(2U), 
                                                        VL_EXTENDS_II(32,10, (IData)(vlSelfRef.top__DOT__vgc_inst__DOT__artist_inst__DOT__err))), 
                                        VL_EXTENDS_II(32,10, (IData)(vlSelfRef.top__DOT__vgc_inst__DOT__artist_inst__DOT__dy))) 
                            & VL_LTES_III(32, VL_MULS_III(32, (IData)(2U), 
                                                          VL_EXTENDS_II(32,10, (IData)(vlSelfRef.top__DOT__vgc_inst__DOT__artist_inst__DOT__err))), 
                                          VL_EXTENDS_II(32,10, (IData)(vlSelfRef.top__DOT__vgc_inst__DOT__artist_inst__DOT__dx))))) {
                    __Vdly__top__DOT__vgc_inst__DOT__artist_inst__DOT__err 
                        = (0x000003ffU & (((IData)(vlSelfRef.top__DOT__vgc_inst__DOT__artist_inst__DOT__err) 
                                           + (IData)(vlSelfRef.top__DOT__vgc_inst__DOT__artist_inst__DOT__dy)) 
                                          + (IData)(vlSelfRef.top__DOT__vgc_inst__DOT__artist_inst__DOT__dx)));
                    __Vdly__top__DOT__vgc_inst__DOT__artist_inst__DOT__x 
                        = (0x000003ffU & ((IData)(vlSelfRef.top__DOT__vgc_inst__DOT__artist_inst__DOT__x) 
                                          + (IData)(vlSelfRef.top__DOT__vgc_inst__DOT__artist_inst__DOT__sx)));
                    __Vdly__top__DOT__vgc_inst__DOT__artist_inst__DOT__y 
                        = (0x000003ffU & ((IData)(vlSelfRef.top__DOT__vgc_inst__DOT__artist_inst__DOT__y) 
                                          + (IData)(vlSelfRef.top__DOT__vgc_inst__DOT__artist_inst__DOT__sy)));
                } else if (VL_GTES_III(32, VL_MULS_III(32, (IData)(2U), 
                                                       VL_EXTENDS_II(32,10, (IData)(vlSelfRef.top__DOT__vgc_inst__DOT__artist_inst__DOT__err))), 
                                       VL_EXTENDS_II(32,10, (IData)(vlSelfRef.top__DOT__vgc_inst__DOT__artist_inst__DOT__dy)))) {
                    __Vdly__top__DOT__vgc_inst__DOT__artist_inst__DOT__err 
                        = (0x000003ffU & ((IData)(vlSelfRef.top__DOT__vgc_inst__DOT__artist_inst__DOT__err) 
                                          + (IData)(vlSelfRef.top__DOT__vgc_inst__DOT__artist_inst__DOT__dy)));
                    __Vdly__top__DOT__vgc_inst__DOT__artist_inst__DOT__x 
                        = (0x000003ffU & ((IData)(vlSelfRef.top__DOT__vgc_inst__DOT__artist_inst__DOT__x) 
                                          + (IData)(vlSelfRef.top__DOT__vgc_inst__DOT__artist_inst__DOT__sx)));
                } else if (VL_LTES_III(32, VL_MULS_III(32, (IData)(2U), 
                                                       VL_EXTENDS_II(32,10, (IData)(vlSelfRef.top__DOT__vgc_inst__DOT__artist_inst__DOT__err))), 
                                       VL_EXTENDS_II(32,10, (IData)(vlSelfRef.top__DOT__vgc_inst__DOT__artist_inst__DOT__dx)))) {
                    __Vdly__top__DOT__vgc_inst__DOT__artist_inst__DOT__err 
                        = (0x000003ffU & ((IData)(vlSelfRef.top__DOT__vgc_inst__DOT__artist_inst__DOT__err) 
                                          + (IData)(vlSelfRef.top__DOT__vgc_inst__DOT__artist_inst__DOT__dx)));
                    __Vdly__top__DOT__vgc_inst__DOT__artist_inst__DOT__y 
                        = (0x000003ffU & ((IData)(vlSelfRef.top__DOT__vgc_inst__DOT__artist_inst__DOT__y) 
                                          + (IData)(vlSelfRef.top__DOT__vgc_inst__DOT__artist_inst__DOT__sy)));
                }
            } else if ((5U == (IData)(vlSelfRef.top__DOT__vgc_inst__DOT__artist_inst__DOT__op))) {
                if (((0x0140U > (IData)(vlSelfRef.top__DOT__vgc_inst__DOT__artist_inst__DOT__cx)) 
                     & (0xc8U > (IData)(vlSelfRef.top__DOT__vgc_inst__DOT__artist_inst__DOT__cy)))) {
                    if ((((((IData)(vlSelfRef.top__DOT__vgc_inst__DOT__artist_inst__DOT__cy) 
                            == (0x000000ffU & (IData)(vlSelfRef.top__DOT__vgc_inst__DOT__artist_inst__DOT__y))) 
                           | ((IData)(vlSelfRef.top__DOT__vgc_inst__DOT__artist_inst__DOT__cy) 
                              == (0x000000ffU & (IData)(vlSelfRef.top__DOT__vgc_inst__DOT__artist_inst__DOT__y2)))) 
                          | ((IData)(vlSelfRef.top__DOT__vgc_inst__DOT__artist_inst__DOT__cx) 
                             == (0x000001ffU & (IData)(vlSelfRef.top__DOT__vgc_inst__DOT__artist_inst__DOT__x)))) 
                         | ((IData)(vlSelfRef.top__DOT__vgc_inst__DOT__artist_inst__DOT__cx) 
                            == (0x000001ffU & (IData)(vlSelfRef.top__DOT__vgc_inst__DOT__artist_inst__DOT__x2))))) {
                        vlSelfRef.top__DOT__vgc_inst__DOT__artist_gfx_addr 
                            = (0x0000ffffU & (((IData)(0x00000140U) 
                                               * (IData)(vlSelfRef.top__DOT__vgc_inst__DOT__artist_inst__DOT__cy)) 
                                              + (IData)(vlSelfRef.top__DOT__vgc_inst__DOT__artist_inst__DOT__cx)));
                        vlSelfRef.top__DOT__vgc_inst__DOT__artist_gfx_wdata 
                            = vlSelfRef.top__DOT__vgc_inst__DOT__artist_inst__DOT__color;
                        vlSelfRef.top__DOT__vgc_inst__DOT__artist_gfx_we = 1U;
                    }
                }
                if (((IData)(vlSelfRef.top__DOT__vgc_inst__DOT__artist_inst__DOT__cx) 
                     >= (0x000001ffU & (IData)(vlSelfRef.top__DOT__vgc_inst__DOT__artist_inst__DOT__x2)))) {
                    __Vdly__top__DOT__vgc_inst__DOT__artist_inst__DOT__cx 
                        = (0x000001ffU & (IData)(vlSelfRef.top__DOT__vgc_inst__DOT__artist_inst__DOT__x));
                    if (((IData)(vlSelfRef.top__DOT__vgc_inst__DOT__artist_inst__DOT__cy) 
                         >= (0x000000ffU & (IData)(vlSelfRef.top__DOT__vgc_inst__DOT__artist_inst__DOT__y2)))) {
                        __Vdly__top__DOT__vgc_inst__DOT__artist_busy = 0U;
                    } else {
                        __Vdly__top__DOT__vgc_inst__DOT__artist_inst__DOT__cy 
                            = (0x000000ffU & ((IData)(1U) 
                                              + (IData)(vlSelfRef.top__DOT__vgc_inst__DOT__artist_inst__DOT__cy)));
                    }
                } else {
                    __Vdly__top__DOT__vgc_inst__DOT__artist_inst__DOT__cx 
                        = (0x000001ffU & ((IData)(1U) 
                                          + (IData)(vlSelfRef.top__DOT__vgc_inst__DOT__artist_inst__DOT__cx)));
                }
            } else if ((6U == (IData)(vlSelfRef.top__DOT__vgc_inst__DOT__artist_inst__DOT__op))) {
                if (((0x0140U > (IData)(vlSelfRef.top__DOT__vgc_inst__DOT__artist_inst__DOT__cx)) 
                     & (0xc8U > (IData)(vlSelfRef.top__DOT__vgc_inst__DOT__artist_inst__DOT__cy)))) {
                    vlSelfRef.top__DOT__vgc_inst__DOT__artist_gfx_addr 
                        = (0x0000ffffU & (((IData)(0x00000140U) 
                                           * (IData)(vlSelfRef.top__DOT__vgc_inst__DOT__artist_inst__DOT__cy)) 
                                          + (IData)(vlSelfRef.top__DOT__vgc_inst__DOT__artist_inst__DOT__cx)));
                    vlSelfRef.top__DOT__vgc_inst__DOT__artist_gfx_wdata 
                        = vlSelfRef.top__DOT__vgc_inst__DOT__artist_inst__DOT__color;
                    vlSelfRef.top__DOT__vgc_inst__DOT__artist_gfx_we = 1U;
                }
                if (((IData)(vlSelfRef.top__DOT__vgc_inst__DOT__artist_inst__DOT__cx) 
                     >= (0x000001ffU & (IData)(vlSelfRef.top__DOT__vgc_inst__DOT__artist_inst__DOT__x2)))) {
                    __Vdly__top__DOT__vgc_inst__DOT__artist_inst__DOT__cx 
                        = (0x000001ffU & (IData)(vlSelfRef.top__DOT__vgc_inst__DOT__artist_inst__DOT__x));
                    if (((IData)(vlSelfRef.top__DOT__vgc_inst__DOT__artist_inst__DOT__cy) 
                         >= (0x000000ffU & (IData)(vlSelfRef.top__DOT__vgc_inst__DOT__artist_inst__DOT__y2)))) {
                        __Vdly__top__DOT__vgc_inst__DOT__artist_busy = 0U;
                    } else {
                        __Vdly__top__DOT__vgc_inst__DOT__artist_inst__DOT__cy 
                            = (0x000000ffU & ((IData)(1U) 
                                              + (IData)(vlSelfRef.top__DOT__vgc_inst__DOT__artist_inst__DOT__cy)));
                    }
                } else {
                    __Vdly__top__DOT__vgc_inst__DOT__artist_inst__DOT__cx 
                        = (0x000001ffU & ((IData)(1U) 
                                          + (IData)(vlSelfRef.top__DOT__vgc_inst__DOT__artist_inst__DOT__cx)));
                }
            } else if ((7U == (IData)(vlSelfRef.top__DOT__vgc_inst__DOT__artist_inst__DOT__op))) {
                vlSelfRef.top__DOT__vgc_inst__DOT__artist_gfx_addr 
                    = (0x0000ffffU & (((IData)(0x00000140U) 
                                       * (IData)(vlSelfRef.top__DOT__vgc_inst__DOT__artist_inst__DOT__cy)) 
                                      + (IData)(vlSelfRef.top__DOT__vgc_inst__DOT__artist_inst__DOT__cx)));
                vlSelfRef.top__DOT__vgc_inst__DOT__artist_gfx_wdata = 0U;
                vlSelfRef.top__DOT__vgc_inst__DOT__artist_gfx_we = 1U;
                if ((0x013fU == (IData)(vlSelfRef.top__DOT__vgc_inst__DOT__artist_inst__DOT__cx))) {
                    __Vdly__top__DOT__vgc_inst__DOT__artist_inst__DOT__cx = 0U;
                    if ((0xc7U == (IData)(vlSelfRef.top__DOT__vgc_inst__DOT__artist_inst__DOT__cy))) {
                        __Vdly__top__DOT__vgc_inst__DOT__artist_busy = 0U;
                    } else {
                        __Vdly__top__DOT__vgc_inst__DOT__artist_inst__DOT__cy 
                            = (0x000000ffU & ((IData)(1U) 
                                              + (IData)(vlSelfRef.top__DOT__vgc_inst__DOT__artist_inst__DOT__cy)));
                    }
                } else {
                    __Vdly__top__DOT__vgc_inst__DOT__artist_inst__DOT__cx 
                        = (0x000001ffU & ((IData)(1U) 
                                          + (IData)(vlSelfRef.top__DOT__vgc_inst__DOT__artist_inst__DOT__cx)));
                }
            } else if ((4U == (IData)(vlSelfRef.top__DOT__vgc_inst__DOT__artist_inst__DOT__op))) {
                if (VL_LTES_III(10, (IData)(vlSelfRef.top__DOT__vgc_inst__DOT__artist_inst__DOT__dx), (IData)(vlSelfRef.top__DOT__vgc_inst__DOT__artist_inst__DOT__dy))) {
                    if ((4U & (IData)(vlSelfRef.top__DOT__vgc_inst__DOT__artist_inst__DOT__octant_count))) {
                        if ((2U & (IData)(vlSelfRef.top__DOT__vgc_inst__DOT__artist_inst__DOT__octant_count))) {
                            if ((1U & (IData)(vlSelfRef.top__DOT__vgc_inst__DOT__artist_inst__DOT__octant_count))) {
                                if ((((VL_LTES_III(32, 0U, 
                                                   (VL_EXTENDS_II(32,10, (IData)(vlSelfRef.top__DOT__vgc_inst__DOT__artist_inst__DOT__x)) 
                                                    - 
                                                    VL_EXTENDS_II(32,10, (IData)(vlSelfRef.top__DOT__vgc_inst__DOT__artist_inst__DOT__dy)))) 
                                       & VL_GTS_III(32, 0x00000140U, 
                                                    (VL_EXTENDS_II(32,10, (IData)(vlSelfRef.top__DOT__vgc_inst__DOT__artist_inst__DOT__x)) 
                                                     - 
                                                     VL_EXTENDS_II(32,10, (IData)(vlSelfRef.top__DOT__vgc_inst__DOT__artist_inst__DOT__dy))))) 
                                      & VL_LTES_III(32, 0U, 
                                                    (VL_EXTENDS_II(32,10, (IData)(vlSelfRef.top__DOT__vgc_inst__DOT__artist_inst__DOT__y)) 
                                                     - 
                                                     VL_EXTENDS_II(32,10, (IData)(vlSelfRef.top__DOT__vgc_inst__DOT__artist_inst__DOT__dx))))) 
                                     & VL_GTS_III(32, 0x000000c8U, 
                                                  (VL_EXTENDS_II(32,10, (IData)(vlSelfRef.top__DOT__vgc_inst__DOT__artist_inst__DOT__y)) 
                                                   - 
                                                   VL_EXTENDS_II(32,10, (IData)(vlSelfRef.top__DOT__vgc_inst__DOT__artist_inst__DOT__dx)))))) {
                                    vlSelfRef.top__DOT__vgc_inst__DOT__artist_gfx_addr 
                                        = (0x0000ffffU 
                                           & (VL_MULS_III(32, (IData)(0x00000140U), 
                                                          (VL_EXTENDS_II(32,10, (IData)(vlSelfRef.top__DOT__vgc_inst__DOT__artist_inst__DOT__y)) 
                                                           - 
                                                           VL_EXTENDS_II(32,10, (IData)(vlSelfRef.top__DOT__vgc_inst__DOT__artist_inst__DOT__dx)))) 
                                              + (VL_EXTENDS_II(16,10, (IData)(vlSelfRef.top__DOT__vgc_inst__DOT__artist_inst__DOT__x)) 
                                                 - 
                                                 VL_EXTENDS_II(16,10, (IData)(vlSelfRef.top__DOT__vgc_inst__DOT__artist_inst__DOT__dy)))));
                                    vlSelfRef.top__DOT__vgc_inst__DOT__artist_gfx_wdata 
                                        = vlSelfRef.top__DOT__vgc_inst__DOT__artist_inst__DOT__color;
                                    vlSelfRef.top__DOT__vgc_inst__DOT__artist_gfx_we = 1U;
                                }
                                __Vdly__top__DOT__vgc_inst__DOT__artist_inst__DOT__octant_count = 0U;
                                __Vdly__top__DOT__vgc_inst__DOT__artist_inst__DOT__dx 
                                    = (0x000003ffU 
                                       & ((IData)(1U) 
                                          + VL_EXTENDS_II(10,10, (IData)(vlSelfRef.top__DOT__vgc_inst__DOT__artist_inst__DOT__dx))));
                                if (VL_GTS_III(32, 0U, 
                                               VL_EXTENDS_II(32,10, (IData)(vlSelfRef.top__DOT__vgc_inst__DOT__artist_inst__DOT__err)))) {
                                    __Vdly__top__DOT__vgc_inst__DOT__artist_inst__DOT__err 
                                        = (0x000003ffU 
                                           & ((IData)(3U) 
                                              + (VL_EXTENDS_II(10,10, (IData)(vlSelfRef.top__DOT__vgc_inst__DOT__artist_inst__DOT__err)) 
                                                 + 
                                                 VL_MULS_III(32, (IData)(2U), 
                                                             VL_EXTENDS_II(32,10, (IData)(vlSelfRef.top__DOT__vgc_inst__DOT__artist_inst__DOT__dx))))));
                                } else {
                                    __Vdly__top__DOT__vgc_inst__DOT__artist_inst__DOT__err 
                                        = (0x000003ffU 
                                           & ((IData)(5U) 
                                              + (VL_EXTENDS_II(10,10, (IData)(vlSelfRef.top__DOT__vgc_inst__DOT__artist_inst__DOT__err)) 
                                                 + 
                                                 VL_MULS_III(32, (IData)(2U), 
                                                             (VL_EXTENDS_II(32,10, (IData)(vlSelfRef.top__DOT__vgc_inst__DOT__artist_inst__DOT__dx)) 
                                                              - 
                                                              VL_EXTENDS_II(32,10, (IData)(vlSelfRef.top__DOT__vgc_inst__DOT__artist_inst__DOT__dy)))))));
                                    __Vdly__top__DOT__vgc_inst__DOT__artist_inst__DOT__dy 
                                        = (0x000003ffU 
                                           & (VL_EXTENDS_II(10,10, (IData)(vlSelfRef.top__DOT__vgc_inst__DOT__artist_inst__DOT__dy)) 
                                              - (IData)(1U)));
                                }
                            } else {
                                if ((((VL_LTES_III(32, 0U, 
                                                   (VL_EXTENDS_II(32,10, (IData)(vlSelfRef.top__DOT__vgc_inst__DOT__artist_inst__DOT__x)) 
                                                    + 
                                                    VL_EXTENDS_II(32,10, (IData)(vlSelfRef.top__DOT__vgc_inst__DOT__artist_inst__DOT__dy)))) 
                                       & VL_GTS_III(32, 0x00000140U, 
                                                    (VL_EXTENDS_II(32,10, (IData)(vlSelfRef.top__DOT__vgc_inst__DOT__artist_inst__DOT__x)) 
                                                     + 
                                                     VL_EXTENDS_II(32,10, (IData)(vlSelfRef.top__DOT__vgc_inst__DOT__artist_inst__DOT__dy))))) 
                                      & VL_LTES_III(32, 0U, 
                                                    (VL_EXTENDS_II(32,10, (IData)(vlSelfRef.top__DOT__vgc_inst__DOT__artist_inst__DOT__y)) 
                                                     - 
                                                     VL_EXTENDS_II(32,10, (IData)(vlSelfRef.top__DOT__vgc_inst__DOT__artist_inst__DOT__dx))))) 
                                     & VL_GTS_III(32, 0x000000c8U, 
                                                  (VL_EXTENDS_II(32,10, (IData)(vlSelfRef.top__DOT__vgc_inst__DOT__artist_inst__DOT__y)) 
                                                   - 
                                                   VL_EXTENDS_II(32,10, (IData)(vlSelfRef.top__DOT__vgc_inst__DOT__artist_inst__DOT__dx)))))) {
                                    vlSelfRef.top__DOT__vgc_inst__DOT__artist_gfx_addr 
                                        = (0x0000ffffU 
                                           & (VL_MULS_III(32, (IData)(0x00000140U), 
                                                          (VL_EXTENDS_II(32,10, (IData)(vlSelfRef.top__DOT__vgc_inst__DOT__artist_inst__DOT__y)) 
                                                           - 
                                                           VL_EXTENDS_II(32,10, (IData)(vlSelfRef.top__DOT__vgc_inst__DOT__artist_inst__DOT__dx)))) 
                                              + (VL_EXTENDS_II(16,10, (IData)(vlSelfRef.top__DOT__vgc_inst__DOT__artist_inst__DOT__x)) 
                                                 + 
                                                 VL_EXTENDS_II(16,10, (IData)(vlSelfRef.top__DOT__vgc_inst__DOT__artist_inst__DOT__dy)))));
                                    vlSelfRef.top__DOT__vgc_inst__DOT__artist_gfx_wdata 
                                        = vlSelfRef.top__DOT__vgc_inst__DOT__artist_inst__DOT__color;
                                    vlSelfRef.top__DOT__vgc_inst__DOT__artist_gfx_we = 1U;
                                }
                                __Vdly__top__DOT__vgc_inst__DOT__artist_inst__DOT__octant_count = 7U;
                            }
                        } else if ((1U & (IData)(vlSelfRef.top__DOT__vgc_inst__DOT__artist_inst__DOT__octant_count))) {
                            if ((((VL_LTES_III(32, 0U, 
                                               (VL_EXTENDS_II(32,10, (IData)(vlSelfRef.top__DOT__vgc_inst__DOT__artist_inst__DOT__x)) 
                                                - VL_EXTENDS_II(32,10, (IData)(vlSelfRef.top__DOT__vgc_inst__DOT__artist_inst__DOT__dy)))) 
                                   & VL_GTS_III(32, 0x00000140U, 
                                                (VL_EXTENDS_II(32,10, (IData)(vlSelfRef.top__DOT__vgc_inst__DOT__artist_inst__DOT__x)) 
                                                 - 
                                                 VL_EXTENDS_II(32,10, (IData)(vlSelfRef.top__DOT__vgc_inst__DOT__artist_inst__DOT__dy))))) 
                                  & VL_LTES_III(32, 0U, 
                                                (VL_EXTENDS_II(32,10, (IData)(vlSelfRef.top__DOT__vgc_inst__DOT__artist_inst__DOT__y)) 
                                                 + 
                                                 VL_EXTENDS_II(32,10, (IData)(vlSelfRef.top__DOT__vgc_inst__DOT__artist_inst__DOT__dx))))) 
                                 & VL_GTS_III(32, 0x000000c8U, 
                                              (VL_EXTENDS_II(32,10, (IData)(vlSelfRef.top__DOT__vgc_inst__DOT__artist_inst__DOT__y)) 
                                               + VL_EXTENDS_II(32,10, (IData)(vlSelfRef.top__DOT__vgc_inst__DOT__artist_inst__DOT__dx)))))) {
                                vlSelfRef.top__DOT__vgc_inst__DOT__artist_gfx_addr 
                                    = (0x0000ffffU 
                                       & (VL_MULS_III(32, (IData)(0x00000140U), 
                                                      (VL_EXTENDS_II(32,10, (IData)(vlSelfRef.top__DOT__vgc_inst__DOT__artist_inst__DOT__y)) 
                                                       + 
                                                       VL_EXTENDS_II(32,10, (IData)(vlSelfRef.top__DOT__vgc_inst__DOT__artist_inst__DOT__dx)))) 
                                          + (VL_EXTENDS_II(16,10, (IData)(vlSelfRef.top__DOT__vgc_inst__DOT__artist_inst__DOT__x)) 
                                             - VL_EXTENDS_II(16,10, (IData)(vlSelfRef.top__DOT__vgc_inst__DOT__artist_inst__DOT__dy)))));
                                vlSelfRef.top__DOT__vgc_inst__DOT__artist_gfx_wdata 
                                    = vlSelfRef.top__DOT__vgc_inst__DOT__artist_inst__DOT__color;
                                vlSelfRef.top__DOT__vgc_inst__DOT__artist_gfx_we = 1U;
                            }
                            __Vdly__top__DOT__vgc_inst__DOT__artist_inst__DOT__octant_count = 6U;
                        } else {
                            if ((((VL_LTES_III(32, 0U, 
                                               (VL_EXTENDS_II(32,10, (IData)(vlSelfRef.top__DOT__vgc_inst__DOT__artist_inst__DOT__x)) 
                                                + VL_EXTENDS_II(32,10, (IData)(vlSelfRef.top__DOT__vgc_inst__DOT__artist_inst__DOT__dy)))) 
                                   & VL_GTS_III(32, 0x00000140U, 
                                                (VL_EXTENDS_II(32,10, (IData)(vlSelfRef.top__DOT__vgc_inst__DOT__artist_inst__DOT__x)) 
                                                 + 
                                                 VL_EXTENDS_II(32,10, (IData)(vlSelfRef.top__DOT__vgc_inst__DOT__artist_inst__DOT__dy))))) 
                                  & VL_LTES_III(32, 0U, 
                                                (VL_EXTENDS_II(32,10, (IData)(vlSelfRef.top__DOT__vgc_inst__DOT__artist_inst__DOT__y)) 
                                                 + 
                                                 VL_EXTENDS_II(32,10, (IData)(vlSelfRef.top__DOT__vgc_inst__DOT__artist_inst__DOT__dx))))) 
                                 & VL_GTS_III(32, 0x000000c8U, 
                                              (VL_EXTENDS_II(32,10, (IData)(vlSelfRef.top__DOT__vgc_inst__DOT__artist_inst__DOT__y)) 
                                               + VL_EXTENDS_II(32,10, (IData)(vlSelfRef.top__DOT__vgc_inst__DOT__artist_inst__DOT__dx)))))) {
                                vlSelfRef.top__DOT__vgc_inst__DOT__artist_gfx_addr 
                                    = (0x0000ffffU 
                                       & (VL_MULS_III(32, (IData)(0x00000140U), 
                                                      (VL_EXTENDS_II(32,10, (IData)(vlSelfRef.top__DOT__vgc_inst__DOT__artist_inst__DOT__y)) 
                                                       + 
                                                       VL_EXTENDS_II(32,10, (IData)(vlSelfRef.top__DOT__vgc_inst__DOT__artist_inst__DOT__dx)))) 
                                          + (VL_EXTENDS_II(16,10, (IData)(vlSelfRef.top__DOT__vgc_inst__DOT__artist_inst__DOT__x)) 
                                             + VL_EXTENDS_II(16,10, (IData)(vlSelfRef.top__DOT__vgc_inst__DOT__artist_inst__DOT__dy)))));
                                vlSelfRef.top__DOT__vgc_inst__DOT__artist_gfx_wdata 
                                    = vlSelfRef.top__DOT__vgc_inst__DOT__artist_inst__DOT__color;
                                vlSelfRef.top__DOT__vgc_inst__DOT__artist_gfx_we = 1U;
                            }
                            __Vdly__top__DOT__vgc_inst__DOT__artist_inst__DOT__octant_count = 5U;
                        }
                    } else if ((2U & (IData)(vlSelfRef.top__DOT__vgc_inst__DOT__artist_inst__DOT__octant_count))) {
                        if ((1U & (IData)(vlSelfRef.top__DOT__vgc_inst__DOT__artist_inst__DOT__octant_count))) {
                            if ((((VL_LTES_III(32, 0U, 
                                               (VL_EXTENDS_II(32,10, (IData)(vlSelfRef.top__DOT__vgc_inst__DOT__artist_inst__DOT__x)) 
                                                - VL_EXTENDS_II(32,10, (IData)(vlSelfRef.top__DOT__vgc_inst__DOT__artist_inst__DOT__dx)))) 
                                   & VL_GTS_III(32, 0x00000140U, 
                                                (VL_EXTENDS_II(32,10, (IData)(vlSelfRef.top__DOT__vgc_inst__DOT__artist_inst__DOT__x)) 
                                                 - 
                                                 VL_EXTENDS_II(32,10, (IData)(vlSelfRef.top__DOT__vgc_inst__DOT__artist_inst__DOT__dx))))) 
                                  & VL_LTES_III(32, 0U, 
                                                (VL_EXTENDS_II(32,10, (IData)(vlSelfRef.top__DOT__vgc_inst__DOT__artist_inst__DOT__y)) 
                                                 - 
                                                 VL_EXTENDS_II(32,10, (IData)(vlSelfRef.top__DOT__vgc_inst__DOT__artist_inst__DOT__dy))))) 
                                 & VL_GTS_III(32, 0x000000c8U, 
                                              (VL_EXTENDS_II(32,10, (IData)(vlSelfRef.top__DOT__vgc_inst__DOT__artist_inst__DOT__y)) 
                                               - VL_EXTENDS_II(32,10, (IData)(vlSelfRef.top__DOT__vgc_inst__DOT__artist_inst__DOT__dy)))))) {
                                vlSelfRef.top__DOT__vgc_inst__DOT__artist_gfx_addr 
                                    = (0x0000ffffU 
                                       & (VL_MULS_III(32, (IData)(0x00000140U), 
                                                      (VL_EXTENDS_II(32,10, (IData)(vlSelfRef.top__DOT__vgc_inst__DOT__artist_inst__DOT__y)) 
                                                       - 
                                                       VL_EXTENDS_II(32,10, (IData)(vlSelfRef.top__DOT__vgc_inst__DOT__artist_inst__DOT__dy)))) 
                                          + (VL_EXTENDS_II(16,10, (IData)(vlSelfRef.top__DOT__vgc_inst__DOT__artist_inst__DOT__x)) 
                                             - VL_EXTENDS_II(16,10, (IData)(vlSelfRef.top__DOT__vgc_inst__DOT__artist_inst__DOT__dx)))));
                                vlSelfRef.top__DOT__vgc_inst__DOT__artist_gfx_wdata 
                                    = vlSelfRef.top__DOT__vgc_inst__DOT__artist_inst__DOT__color;
                                vlSelfRef.top__DOT__vgc_inst__DOT__artist_gfx_we = 1U;
                            }
                            __Vdly__top__DOT__vgc_inst__DOT__artist_inst__DOT__octant_count = 4U;
                        } else {
                            if ((((VL_LTES_III(32, 0U, 
                                               (VL_EXTENDS_II(32,10, (IData)(vlSelfRef.top__DOT__vgc_inst__DOT__artist_inst__DOT__x)) 
                                                + VL_EXTENDS_II(32,10, (IData)(vlSelfRef.top__DOT__vgc_inst__DOT__artist_inst__DOT__dx)))) 
                                   & VL_GTS_III(32, 0x00000140U, 
                                                (VL_EXTENDS_II(32,10, (IData)(vlSelfRef.top__DOT__vgc_inst__DOT__artist_inst__DOT__x)) 
                                                 + 
                                                 VL_EXTENDS_II(32,10, (IData)(vlSelfRef.top__DOT__vgc_inst__DOT__artist_inst__DOT__dx))))) 
                                  & VL_LTES_III(32, 0U, 
                                                (VL_EXTENDS_II(32,10, (IData)(vlSelfRef.top__DOT__vgc_inst__DOT__artist_inst__DOT__y)) 
                                                 - 
                                                 VL_EXTENDS_II(32,10, (IData)(vlSelfRef.top__DOT__vgc_inst__DOT__artist_inst__DOT__dy))))) 
                                 & VL_GTS_III(32, 0x000000c8U, 
                                              (VL_EXTENDS_II(32,10, (IData)(vlSelfRef.top__DOT__vgc_inst__DOT__artist_inst__DOT__y)) 
                                               - VL_EXTENDS_II(32,10, (IData)(vlSelfRef.top__DOT__vgc_inst__DOT__artist_inst__DOT__dy)))))) {
                                vlSelfRef.top__DOT__vgc_inst__DOT__artist_gfx_addr 
                                    = (0x0000ffffU 
                                       & (VL_MULS_III(32, (IData)(0x00000140U), 
                                                      (VL_EXTENDS_II(32,10, (IData)(vlSelfRef.top__DOT__vgc_inst__DOT__artist_inst__DOT__y)) 
                                                       - 
                                                       VL_EXTENDS_II(32,10, (IData)(vlSelfRef.top__DOT__vgc_inst__DOT__artist_inst__DOT__dy)))) 
                                          + (VL_EXTENDS_II(16,10, (IData)(vlSelfRef.top__DOT__vgc_inst__DOT__artist_inst__DOT__x)) 
                                             + VL_EXTENDS_II(16,10, (IData)(vlSelfRef.top__DOT__vgc_inst__DOT__artist_inst__DOT__dx)))));
                                vlSelfRef.top__DOT__vgc_inst__DOT__artist_gfx_wdata 
                                    = vlSelfRef.top__DOT__vgc_inst__DOT__artist_inst__DOT__color;
                                vlSelfRef.top__DOT__vgc_inst__DOT__artist_gfx_we = 1U;
                            }
                            __Vdly__top__DOT__vgc_inst__DOT__artist_inst__DOT__octant_count = 3U;
                        }
                    } else if ((1U & (IData)(vlSelfRef.top__DOT__vgc_inst__DOT__artist_inst__DOT__octant_count))) {
                        if ((((VL_LTES_III(32, 0U, 
                                           (VL_EXTENDS_II(32,10, (IData)(vlSelfRef.top__DOT__vgc_inst__DOT__artist_inst__DOT__x)) 
                                            - VL_EXTENDS_II(32,10, (IData)(vlSelfRef.top__DOT__vgc_inst__DOT__artist_inst__DOT__dx)))) 
                               & VL_GTS_III(32, 0x00000140U, 
                                            (VL_EXTENDS_II(32,10, (IData)(vlSelfRef.top__DOT__vgc_inst__DOT__artist_inst__DOT__x)) 
                                             - VL_EXTENDS_II(32,10, (IData)(vlSelfRef.top__DOT__vgc_inst__DOT__artist_inst__DOT__dx))))) 
                              & VL_LTES_III(32, 0U, 
                                            (VL_EXTENDS_II(32,10, (IData)(vlSelfRef.top__DOT__vgc_inst__DOT__artist_inst__DOT__y)) 
                                             + VL_EXTENDS_II(32,10, (IData)(vlSelfRef.top__DOT__vgc_inst__DOT__artist_inst__DOT__dy))))) 
                             & VL_GTS_III(32, 0x000000c8U, 
                                          (VL_EXTENDS_II(32,10, (IData)(vlSelfRef.top__DOT__vgc_inst__DOT__artist_inst__DOT__y)) 
                                           + VL_EXTENDS_II(32,10, (IData)(vlSelfRef.top__DOT__vgc_inst__DOT__artist_inst__DOT__dy)))))) {
                            vlSelfRef.top__DOT__vgc_inst__DOT__artist_gfx_addr 
                                = (0x0000ffffU & (VL_MULS_III(32, (IData)(0x00000140U), 
                                                              (VL_EXTENDS_II(32,10, (IData)(vlSelfRef.top__DOT__vgc_inst__DOT__artist_inst__DOT__y)) 
                                                               + 
                                                               VL_EXTENDS_II(32,10, (IData)(vlSelfRef.top__DOT__vgc_inst__DOT__artist_inst__DOT__dy)))) 
                                                  + 
                                                  (VL_EXTENDS_II(16,10, (IData)(vlSelfRef.top__DOT__vgc_inst__DOT__artist_inst__DOT__x)) 
                                                   - 
                                                   VL_EXTENDS_II(16,10, (IData)(vlSelfRef.top__DOT__vgc_inst__DOT__artist_inst__DOT__dx)))));
                            vlSelfRef.top__DOT__vgc_inst__DOT__artist_gfx_wdata 
                                = vlSelfRef.top__DOT__vgc_inst__DOT__artist_inst__DOT__color;
                            vlSelfRef.top__DOT__vgc_inst__DOT__artist_gfx_we = 1U;
                        }
                        __Vdly__top__DOT__vgc_inst__DOT__artist_inst__DOT__octant_count = 2U;
                    } else {
                        if ((((VL_LTES_III(32, 0U, 
                                           (VL_EXTENDS_II(32,10, (IData)(vlSelfRef.top__DOT__vgc_inst__DOT__artist_inst__DOT__x)) 
                                            + VL_EXTENDS_II(32,10, (IData)(vlSelfRef.top__DOT__vgc_inst__DOT__artist_inst__DOT__dx)))) 
                               & VL_GTS_III(32, 0x00000140U, 
                                            (VL_EXTENDS_II(32,10, (IData)(vlSelfRef.top__DOT__vgc_inst__DOT__artist_inst__DOT__x)) 
                                             + VL_EXTENDS_II(32,10, (IData)(vlSelfRef.top__DOT__vgc_inst__DOT__artist_inst__DOT__dx))))) 
                              & VL_LTES_III(32, 0U, 
                                            (VL_EXTENDS_II(32,10, (IData)(vlSelfRef.top__DOT__vgc_inst__DOT__artist_inst__DOT__y)) 
                                             + VL_EXTENDS_II(32,10, (IData)(vlSelfRef.top__DOT__vgc_inst__DOT__artist_inst__DOT__dy))))) 
                             & VL_GTS_III(32, 0x000000c8U, 
                                          (VL_EXTENDS_II(32,10, (IData)(vlSelfRef.top__DOT__vgc_inst__DOT__artist_inst__DOT__y)) 
                                           + VL_EXTENDS_II(32,10, (IData)(vlSelfRef.top__DOT__vgc_inst__DOT__artist_inst__DOT__dy)))))) {
                            vlSelfRef.top__DOT__vgc_inst__DOT__artist_gfx_addr 
                                = (0x0000ffffU & (VL_MULS_III(32, (IData)(0x00000140U), 
                                                              (VL_EXTENDS_II(32,10, (IData)(vlSelfRef.top__DOT__vgc_inst__DOT__artist_inst__DOT__y)) 
                                                               + 
                                                               VL_EXTENDS_II(32,10, (IData)(vlSelfRef.top__DOT__vgc_inst__DOT__artist_inst__DOT__dy)))) 
                                                  + 
                                                  (VL_EXTENDS_II(16,10, (IData)(vlSelfRef.top__DOT__vgc_inst__DOT__artist_inst__DOT__x)) 
                                                   + 
                                                   VL_EXTENDS_II(16,10, (IData)(vlSelfRef.top__DOT__vgc_inst__DOT__artist_inst__DOT__dx)))));
                            vlSelfRef.top__DOT__vgc_inst__DOT__artist_gfx_wdata 
                                = vlSelfRef.top__DOT__vgc_inst__DOT__artist_inst__DOT__color;
                            vlSelfRef.top__DOT__vgc_inst__DOT__artist_gfx_we = 1U;
                        }
                        __Vdly__top__DOT__vgc_inst__DOT__artist_inst__DOT__octant_count = 1U;
                    }
                } else {
                    __Vdly__top__DOT__vgc_inst__DOT__artist_busy = 0U;
                }
            } else if ((9U == (IData)(vlSelfRef.top__DOT__vgc_inst__DOT__artist_inst__DOT__op))) {
                if (vlSelfRef.top__DOT__vgc_inst__DOT__artist_inst__DOT__paint_init) {
                    __Vdly__top__DOT__vgc_inst__DOT__artist_inst__DOT__fill_target 
                        = vlSelfRef.top__DOT__vgc_inst__DOT__gfx_a_dout;
                    __Vdly__top__DOT__vgc_inst__DOT__artist_inst__DOT__paint_init = 0U;
                    __Vdly__top__DOT__vgc_inst__DOT__artist_inst__DOT__paint_phase = 0U;
                } else if ((4U & (IData)(vlSelfRef.top__DOT__vgc_inst__DOT__artist_inst__DOT__paint_phase))) {
                    if ((2U & (IData)(vlSelfRef.top__DOT__vgc_inst__DOT__artist_inst__DOT__paint_phase))) {
                        __Vdly__top__DOT__vgc_inst__DOT__artist_inst__DOT__paint_phase = 0U;
                    } else if ((1U & (IData)(vlSelfRef.top__DOT__vgc_inst__DOT__artist_inst__DOT__paint_phase))) {
                        if ((0x0200U > (IData)(vlSelfRef.top__DOT__vgc_inst__DOT__artist_inst__DOT__fill_sp))) {
                            vlSelfRef.top__DOT__vgc_inst__DOT__artist_inst__DOT__fs_a_addr 
                                = (0x000001ffU & (IData)(vlSelfRef.top__DOT__vgc_inst__DOT__artist_inst__DOT__fill_sp));
                            vlSelfRef.top__DOT__vgc_inst__DOT__artist_inst__DOT__fs_a_din 
                                = vlSelfRef.top__DOT__vgc_inst__DOT__artist_inst__DOT__push_neighbors
                                [vlSelfRef.top__DOT__vgc_inst__DOT__artist_inst__DOT__push_count];
                            vlSelfRef.top__DOT__vgc_inst__DOT__artist_inst__DOT__fs_a_we = 1U;
                            __Vdly__top__DOT__vgc_inst__DOT__artist_inst__DOT__fill_sp 
                                = (0x000003ffU & ((IData)(1U) 
                                                  + (IData)(vlSelfRef.top__DOT__vgc_inst__DOT__artist_inst__DOT__fill_sp)));
                        }
                        if ((3U == (IData)(vlSelfRef.top__DOT__vgc_inst__DOT__artist_inst__DOT__push_count))) {
                            __Vdly__top__DOT__vgc_inst__DOT__artist_inst__DOT__paint_phase = 0U;
                        } else {
                            vlSelfRef.top__DOT__vgc_inst__DOT__artist_inst__DOT__push_count 
                                = (3U & ((IData)(1U) 
                                         + (IData)(vlSelfRef.top__DOT__vgc_inst__DOT__artist_inst__DOT__push_count)));
                        }
                    } else if ((((IData)(vlSelfRef.top__DOT__vgc_inst__DOT__gfx_a_dout) 
                                 == (IData)(vlSelfRef.top__DOT__vgc_inst__DOT__artist_inst__DOT__fill_target)) 
                                & ((IData)(vlSelfRef.top__DOT__vgc_inst__DOT__artist_inst__DOT__fill_target) 
                                   != (IData)(vlSelfRef.top__DOT__vgc_inst__DOT__artist_inst__DOT__color)))) {
                        vlSelfRef.top__DOT__vgc_inst__DOT__artist_gfx_addr 
                            = (0x0000ffffU & (((IData)(0x00000140U) 
                                               * (IData)(vlSelfRef.top__DOT__vgc_inst__DOT__artist_inst__DOT__cy)) 
                                              + (IData)(vlSelfRef.top__DOT__vgc_inst__DOT__artist_inst__DOT__cx)));
                        vlSelfRef.top__DOT__vgc_inst__DOT__artist_gfx_wdata 
                            = vlSelfRef.top__DOT__vgc_inst__DOT__artist_inst__DOT__color;
                        vlSelfRef.top__DOT__vgc_inst__DOT__artist_gfx_we = 1U;
                        vlSelfRef.top__DOT__vgc_inst__DOT__artist_inst__DOT__push_count = 0U;
                        __Vdly__top__DOT__vgc_inst__DOT__artist_inst__DOT__paint_phase = 5U;
                        __VdlyVal__top__DOT__vgc_inst__DOT__artist_inst__DOT__push_neighbors__v0 
                            = (((IData)(vlSelfRef.top__DOT__vgc_inst__DOT__artist_inst__DOT__cy) 
                                << 9U) | (0x000001ffU 
                                          & ((IData)(1U) 
                                             + (IData)(vlSelfRef.top__DOT__vgc_inst__DOT__artist_inst__DOT__cx))));
                        __VdlySet__top__DOT__vgc_inst__DOT__artist_inst__DOT__push_neighbors__v0 = 1U;
                        __VdlyVal__top__DOT__vgc_inst__DOT__artist_inst__DOT__push_neighbors__v1 
                            = (((IData)(vlSelfRef.top__DOT__vgc_inst__DOT__artist_inst__DOT__cy) 
                                << 9U) | (0x000001ffU 
                                          & ((IData)(vlSelfRef.top__DOT__vgc_inst__DOT__artist_inst__DOT__cx) 
                                             - (IData)(1U))));
                        __VdlyVal__top__DOT__vgc_inst__DOT__artist_inst__DOT__push_neighbors__v2 
                            = ((0x0001fe00U & (((IData)(1U) 
                                                + (IData)(vlSelfRef.top__DOT__vgc_inst__DOT__artist_inst__DOT__cy)) 
                                               << 9U)) 
                               | (IData)(vlSelfRef.top__DOT__vgc_inst__DOT__artist_inst__DOT__cx));
                        __VdlyVal__top__DOT__vgc_inst__DOT__artist_inst__DOT__push_neighbors__v3 
                            = ((0x0001fe00U & (((IData)(vlSelfRef.top__DOT__vgc_inst__DOT__artist_inst__DOT__cy) 
                                                - (IData)(1U)) 
                                               << 9U)) 
                               | (IData)(vlSelfRef.top__DOT__vgc_inst__DOT__artist_inst__DOT__cx));
                    } else {
                        __Vdly__top__DOT__vgc_inst__DOT__artist_inst__DOT__paint_phase = 0U;
                    }
                } else if ((2U & (IData)(vlSelfRef.top__DOT__vgc_inst__DOT__artist_inst__DOT__paint_phase))) {
                    if ((1U & (IData)(vlSelfRef.top__DOT__vgc_inst__DOT__artist_inst__DOT__paint_phase))) {
                        __Vdly__top__DOT__vgc_inst__DOT__artist_inst__DOT__paint_phase = 4U;
                    } else {
                        __Vdly__top__DOT__vgc_inst__DOT__artist_inst__DOT__cx 
                            = (0x000001ffU & vlSelfRef.top__DOT__vgc_inst__DOT__artist_inst__DOT__fs_b_dout);
                        __Vdly__top__DOT__vgc_inst__DOT__artist_inst__DOT__cy 
                            = (0x000000ffU & (vlSelfRef.top__DOT__vgc_inst__DOT__artist_inst__DOT__fs_b_dout 
                                              >> 9U));
                        if (((0x0140U > (0x000001ffU 
                                         & vlSelfRef.top__DOT__vgc_inst__DOT__artist_inst__DOT__fs_b_dout)) 
                             & (0xc8U > (0x000000ffU 
                                         & (vlSelfRef.top__DOT__vgc_inst__DOT__artist_inst__DOT__fs_b_dout 
                                            >> 9U))))) {
                            vlSelfRef.top__DOT__vgc_inst__DOT__artist_gfx_raddr 
                                = (0x0000ffffU & (((IData)(0x00000140U) 
                                                   * 
                                                   (0x000000ffU 
                                                    & (vlSelfRef.top__DOT__vgc_inst__DOT__artist_inst__DOT__fs_b_dout 
                                                       >> 9U))) 
                                                  + 
                                                  (0x000001ffU 
                                                   & vlSelfRef.top__DOT__vgc_inst__DOT__artist_inst__DOT__fs_b_dout)));
                            vlSelfRef.top__DOT__vgc_inst__DOT__artist_gfx_re = 1U;
                            __Vdly__top__DOT__vgc_inst__DOT__artist_inst__DOT__paint_phase = 3U;
                        } else {
                            __Vdly__top__DOT__vgc_inst__DOT__artist_inst__DOT__paint_phase = 0U;
                        }
                    }
                } else if ((1U & (IData)(vlSelfRef.top__DOT__vgc_inst__DOT__artist_inst__DOT__paint_phase))) {
                    __Vdly__top__DOT__vgc_inst__DOT__artist_inst__DOT__paint_phase = 2U;
                } else if ((0U < (IData)(vlSelfRef.top__DOT__vgc_inst__DOT__artist_inst__DOT__fill_sp))) {
                    __Vdly__top__DOT__vgc_inst__DOT__artist_inst__DOT__fs_b_addr 
                        = (0x000001ffU & ((IData)(vlSelfRef.top__DOT__vgc_inst__DOT__artist_inst__DOT__fill_sp) 
                                          - (IData)(1U)));
                    __Vdly__top__DOT__vgc_inst__DOT__artist_inst__DOT__paint_phase = 1U;
                    __Vdly__top__DOT__vgc_inst__DOT__artist_inst__DOT__fill_sp 
                        = (0x000003ffU & ((IData)(vlSelfRef.top__DOT__vgc_inst__DOT__artist_inst__DOT__fill_sp) 
                                          - (IData)(1U)));
                } else {
                    __Vdly__top__DOT__vgc_inst__DOT__artist_busy = 0U;
                }
            } else if ((0x0aU == (IData)(vlSelfRef.top__DOT__vgc_inst__DOT__artist_inst__DOT__op))) {
                if ((0U == (IData)(vlSelfRef.top__DOT__vgc_inst__DOT__artist_inst__DOT__gt_font_pending))) {
                    if (((IData)(vlSelfRef.top__DOT__vgc_inst__DOT__artist_inst__DOT__gt_char_idx) 
                         < (IData)(vlSelfRef.top__DOT__vgc_inst__DOT__artist_inst__DOT__gt_len))) {
                        if ((1U & ((IData)(vlSelfRef.top__DOT__vgc_inst__DOT__artist_inst__DOT__gt_font_byte) 
                                   >> (7U & ((IData)(7U) 
                                             - (IData)(vlSelfRef.top__DOT__vgc_inst__DOT__artist_inst__DOT__gt_font_col)))))) {
                            if (((0x00000140U > (((IData)(vlSelfRef.top__DOT__vgc_inst__DOT__artist_inst__DOT__gt_pen_x) 
                                                  + 
                                                  ((IData)(vlSelfRef.top__DOT__vgc_inst__DOT__artist_inst__DOT__gt_font_col) 
                                                   * (IData)(vlSelfRef.top__DOT__vgc_inst__DOT__artist_inst__DOT__gt_scale))) 
                                                 + (IData)(vlSelfRef.top__DOT__vgc_inst__DOT__artist_inst__DOT__gt_scale_x))) 
                                 & (0x000000c8U > (
                                                   ((IData)(vlSelfRef.top__DOT__vgc_inst__DOT__artist_inst__DOT__gt_start_y) 
                                                    + 
                                                    ((IData)(vlSelfRef.top__DOT__vgc_inst__DOT__artist_inst__DOT__gt_font_row) 
                                                     * (IData)(vlSelfRef.top__DOT__vgc_inst__DOT__artist_inst__DOT__gt_scale))) 
                                                   + (IData)(vlSelfRef.top__DOT__vgc_inst__DOT__artist_inst__DOT__gt_scale_y))))) {
                                vlSelfRef.top__DOT__vgc_inst__DOT__artist_gfx_addr 
                                    = (0x0000ffffU 
                                       & (((IData)(0x00000140U) 
                                           * (((IData)(vlSelfRef.top__DOT__vgc_inst__DOT__artist_inst__DOT__gt_start_y) 
                                               + ((IData)(vlSelfRef.top__DOT__vgc_inst__DOT__artist_inst__DOT__gt_font_row) 
                                                  * (IData)(vlSelfRef.top__DOT__vgc_inst__DOT__artist_inst__DOT__gt_scale))) 
                                              + (IData)(vlSelfRef.top__DOT__vgc_inst__DOT__artist_inst__DOT__gt_scale_y))) 
                                          + (((IData)(vlSelfRef.top__DOT__vgc_inst__DOT__artist_inst__DOT__gt_pen_x) 
                                              + ((IData)(vlSelfRef.top__DOT__vgc_inst__DOT__artist_inst__DOT__gt_font_col) 
                                                 * (IData)(vlSelfRef.top__DOT__vgc_inst__DOT__artist_inst__DOT__gt_scale))) 
                                             + (IData)(vlSelfRef.top__DOT__vgc_inst__DOT__artist_inst__DOT__gt_scale_x))));
                                vlSelfRef.top__DOT__vgc_inst__DOT__artist_gfx_wdata 
                                    = vlSelfRef.top__DOT__vgc_inst__DOT__artist_inst__DOT__color;
                                vlSelfRef.top__DOT__vgc_inst__DOT__artist_gfx_we = 1U;
                            }
                        }
                        if (((0x000000ffU & ((IData)(1U) 
                                             + (IData)(vlSelfRef.top__DOT__vgc_inst__DOT__artist_inst__DOT__gt_scale_x))) 
                             < (IData)(vlSelfRef.top__DOT__vgc_inst__DOT__artist_inst__DOT__gt_scale))) {
                            vlSelfRef.top__DOT__vgc_inst__DOT__artist_inst__DOT__gt_scale_x 
                                = (0x000000ffU & ((IData)(1U) 
                                                  + (IData)(vlSelfRef.top__DOT__vgc_inst__DOT__artist_inst__DOT__gt_scale_x)));
                        } else {
                            vlSelfRef.top__DOT__vgc_inst__DOT__artist_inst__DOT__gt_scale_x = 0U;
                            if ((7U > (IData)(vlSelfRef.top__DOT__vgc_inst__DOT__artist_inst__DOT__gt_font_col))) {
                                vlSelfRef.top__DOT__vgc_inst__DOT__artist_inst__DOT__gt_font_col 
                                    = (7U & ((IData)(1U) 
                                             + (IData)(vlSelfRef.top__DOT__vgc_inst__DOT__artist_inst__DOT__gt_font_col)));
                            } else {
                                vlSelfRef.top__DOT__vgc_inst__DOT__artist_inst__DOT__gt_font_col = 0U;
                                if (((0x000000ffU & 
                                      ((IData)(1U) 
                                       + (IData)(vlSelfRef.top__DOT__vgc_inst__DOT__artist_inst__DOT__gt_scale_y))) 
                                     < (IData)(vlSelfRef.top__DOT__vgc_inst__DOT__artist_inst__DOT__gt_scale))) {
                                    vlSelfRef.top__DOT__vgc_inst__DOT__artist_inst__DOT__gt_scale_y 
                                        = (0x000000ffU 
                                           & ((IData)(1U) 
                                              + (IData)(vlSelfRef.top__DOT__vgc_inst__DOT__artist_inst__DOT__gt_scale_y)));
                                } else {
                                    vlSelfRef.top__DOT__vgc_inst__DOT__artist_inst__DOT__gt_scale_y = 0U;
                                    if ((7U > (IData)(vlSelfRef.top__DOT__vgc_inst__DOT__artist_inst__DOT__gt_font_row))) {
                                        __Vdly__top__DOT__vgc_inst__DOT__artist_inst__DOT__gt_font_row 
                                            = (7U & 
                                               ((IData)(1U) 
                                                + (IData)(vlSelfRef.top__DOT__vgc_inst__DOT__artist_inst__DOT__gt_font_row)));
                                        vlSelfRef.top__DOT__vgc_inst__DOT__artist_font_addr 
                                            = (((IData)(vlSelfRef.top__DOT__vgc_inst__DOT__artist_inst__DOT__gt_cur_char) 
                                                << 3U) 
                                               | (7U 
                                                  & ((IData)(1U) 
                                                     + (IData)(vlSelfRef.top__DOT__vgc_inst__DOT__artist_inst__DOT__gt_font_row))));
                                        vlSelfRef.top__DOT__vgc_inst__DOT__artist_font_re = 1U;
                                        __Vdly__top__DOT__vgc_inst__DOT__artist_inst__DOT__gt_font_pending = 1U;
                                    } else {
                                        __Vdly__top__DOT__vgc_inst__DOT__artist_inst__DOT__gt_char_idx 
                                            = (0x0000003fU 
                                               & ((IData)(1U) 
                                                  + (IData)(vlSelfRef.top__DOT__vgc_inst__DOT__artist_inst__DOT__gt_char_idx)));
                                        vlSelfRef.top__DOT__vgc_inst__DOT__artist_inst__DOT__gt_pen_x 
                                            = (0x000001ffU 
                                               & ((IData)(vlSelfRef.top__DOT__vgc_inst__DOT__artist_inst__DOT__gt_pen_x) 
                                                  + 
                                                  (0x0000ffffU 
                                                   & VL_SHIFTL_III(16,16,32, (IData)(vlSelfRef.top__DOT__vgc_inst__DOT__artist_inst__DOT__gt_scale), 3U))));
                                        __Vdly__top__DOT__vgc_inst__DOT__artist_inst__DOT__gt_font_row = 0U;
                                        if ((((IData)(1U) 
                                              + (IData)(vlSelfRef.top__DOT__vgc_inst__DOT__artist_inst__DOT__gt_char_idx)) 
                                             < (IData)(vlSelfRef.top__DOT__vgc_inst__DOT__artist_inst__DOT__gt_len))) {
                                            __Vfunc_top__DOT__vgc_inst__DOT__artist_inst__DOT__gt_char__10__idx 
                                                = (0x0000003fU 
                                                   & ((IData)(1U) 
                                                      + (IData)(vlSelfRef.top__DOT__vgc_inst__DOT__artist_inst__DOT__gt_char_idx)));
                                            __Vfunc_top__DOT__vgc_inst__DOT__artist_inst__DOT__gt_char__10__Vfuncout 
                                                = (0x000000ffU 
                                                   & (((0U 
                                                        == 
                                                        (0x0000001fU 
                                                         & VL_SHIFTL_III(9,32,32, (IData)(__Vfunc_top__DOT__vgc_inst__DOT__artist_inst__DOT__gt_char__10__idx), 3U)))
                                                        ? 0U
                                                        : 
                                                       (vlSelfRef.top__DOT__vgc_inst__DOT__fio_name_flat
                                                        [
                                                        (((IData)(7U) 
                                                          + 
                                                          (0x000001ffU 
                                                           & VL_SHIFTL_III(9,32,32, (IData)(__Vfunc_top__DOT__vgc_inst__DOT__artist_inst__DOT__gt_char__10__idx), 3U))) 
                                                         >> 5U)] 
                                                        << 
                                                        ((IData)(0x00000020U) 
                                                         - 
                                                         (0x0000001fU 
                                                          & VL_SHIFTL_III(9,32,32, (IData)(__Vfunc_top__DOT__vgc_inst__DOT__artist_inst__DOT__gt_char__10__idx), 3U))))) 
                                                      | (vlSelfRef.top__DOT__vgc_inst__DOT__fio_name_flat
                                                         [
                                                         (0x0000000fU 
                                                          & (VL_SHIFTL_III(9,32,32, (IData)(__Vfunc_top__DOT__vgc_inst__DOT__artist_inst__DOT__gt_char__10__idx), 3U) 
                                                             >> 5U))] 
                                                         >> 
                                                         (0x0000001fU 
                                                          & VL_SHIFTL_III(9,32,32, (IData)(__Vfunc_top__DOT__vgc_inst__DOT__artist_inst__DOT__gt_char__10__idx), 3U)))));
                                            vlSelfRef.top__DOT__vgc_inst__DOT__artist_inst__DOT__gt_cur_char 
                                                = __Vfunc_top__DOT__vgc_inst__DOT__artist_inst__DOT__gt_char__10__Vfuncout;
                                            vlSelfRef.top__DOT__vgc_inst__DOT__artist_font_re = 1U;
                                            __Vdly__top__DOT__vgc_inst__DOT__artist_inst__DOT__gt_font_pending = 1U;
                                            vlSelfRef.top__DOT__vgc_inst__DOT__artist_font_addr 
                                                = (
                                                   ([&]() {
                                                        __Vfunc_top__DOT__vgc_inst__DOT__artist_inst__DOT__gt_char__11__idx 
                                                            = 
                                                            (0x0000003fU 
                                                             & ((IData)(1U) 
                                                                + (IData)(vlSelfRef.top__DOT__vgc_inst__DOT__artist_inst__DOT__gt_char_idx)));
                                                        __Vfunc_top__DOT__vgc_inst__DOT__artist_inst__DOT__gt_char__11__Vfuncout 
                                                            = 
                                                            (0x000000ffU 
                                                             & (((0U 
                                                                  == 
                                                                  (0x0000001fU 
                                                                   & VL_SHIFTL_III(9,32,32, (IData)(__Vfunc_top__DOT__vgc_inst__DOT__artist_inst__DOT__gt_char__11__idx), 3U)))
                                                                  ? 0U
                                                                  : 
                                                                 (vlSelfRef.top__DOT__vgc_inst__DOT__fio_name_flat
                                                                  [
                                                                  (((IData)(7U) 
                                                                    + 
                                                                    (0x000001ffU 
                                                                     & VL_SHIFTL_III(9,32,32, (IData)(__Vfunc_top__DOT__vgc_inst__DOT__artist_inst__DOT__gt_char__11__idx), 3U))) 
                                                                   >> 5U)] 
                                                                  << 
                                                                  ((IData)(0x00000020U) 
                                                                   - 
                                                                   (0x0000001fU 
                                                                    & VL_SHIFTL_III(9,32,32, (IData)(__Vfunc_top__DOT__vgc_inst__DOT__artist_inst__DOT__gt_char__11__idx), 3U))))) 
                                                                | (vlSelfRef.top__DOT__vgc_inst__DOT__fio_name_flat
                                                                   [
                                                                   (0x0000000fU 
                                                                    & (VL_SHIFTL_III(9,32,32, (IData)(__Vfunc_top__DOT__vgc_inst__DOT__artist_inst__DOT__gt_char__11__idx), 3U) 
                                                                       >> 5U))] 
                                                                   >> 
                                                                   (0x0000001fU 
                                                                    & VL_SHIFTL_III(9,32,32, (IData)(__Vfunc_top__DOT__vgc_inst__DOT__artist_inst__DOT__gt_char__11__idx), 3U)))));
                                                    }(), (IData)(__Vfunc_top__DOT__vgc_inst__DOT__artist_inst__DOT__gt_char__11__Vfuncout)) 
                                                   << 3U);
                                        }
                                    }
                                }
                            }
                        }
                    } else {
                        __Vdly__top__DOT__vgc_inst__DOT__artist_busy = 0U;
                    }
                }
            } else {
                __Vdly__top__DOT__vgc_inst__DOT__artist_busy = 0U;
            }
        }
        if (((IData)(vlSelfRef.top__DOT__vgc_inst__DOT__artist_cmd_valid) 
             & (~ (IData)(vlSelfRef.top__DOT__vgc_inst__DOT__artist_busy)))) {
            vlSelfRef.top__DOT__vgc_inst__DOT__artist_inst__DOT__color 
                = vlSelfRef.top__DOT__vgc_inst__DOT__gfx_color;
            if (((((((((1U == (IData)(vlSelfRef.top__DOT__vgc_inst__DOT__artist_cmd_code)) 
                       | (2U == (IData)(vlSelfRef.top__DOT__vgc_inst__DOT__artist_cmd_code))) 
                      | (3U == (IData)(vlSelfRef.top__DOT__vgc_inst__DOT__artist_cmd_code))) 
                     | ((5U == (IData)(vlSelfRef.top__DOT__vgc_inst__DOT__artist_cmd_code)) 
                        || (6U == (IData)(vlSelfRef.top__DOT__vgc_inst__DOT__artist_cmd_code)))) 
                    | (7U == (IData)(vlSelfRef.top__DOT__vgc_inst__DOT__artist_cmd_code))) 
                   | (4U == (IData)(vlSelfRef.top__DOT__vgc_inst__DOT__artist_cmd_code))) 
                  | (9U == (IData)(vlSelfRef.top__DOT__vgc_inst__DOT__artist_cmd_code))) 
                 | (0x0aU == (IData)(vlSelfRef.top__DOT__vgc_inst__DOT__artist_cmd_code)))) {
                if ((1U == (IData)(vlSelfRef.top__DOT__vgc_inst__DOT__artist_cmd_code))) {
                    if (((0x0140U > (IData)(vlSelfRef.top__DOT__vgc_inst__DOT____Vcellinp__artist_inst__cmd_x0)) 
                         & (0xc8U > vlSelfRef.top__DOT__vgc_inst__DOT__regs[19U]))) {
                        vlSelfRef.top__DOT__vgc_inst__DOT__artist_gfx_addr 
                            = (0x0000ffffU & (((IData)(0x00000140U) 
                                               * vlSelfRef.top__DOT__vgc_inst__DOT__regs[19U]) 
                                              + (IData)(vlSelfRef.top__DOT__vgc_inst__DOT____Vcellinp__artist_inst__cmd_x0)));
                        vlSelfRef.top__DOT__vgc_inst__DOT__artist_gfx_wdata 
                            = vlSelfRef.top__DOT__vgc_inst__DOT__gfx_color;
                        vlSelfRef.top__DOT__vgc_inst__DOT__artist_gfx_we = 1U;
                    }
                } else if ((2U == (IData)(vlSelfRef.top__DOT__vgc_inst__DOT__artist_cmd_code))) {
                    if (((0x0140U > (IData)(vlSelfRef.top__DOT__vgc_inst__DOT____Vcellinp__artist_inst__cmd_x0)) 
                         & (0xc8U > vlSelfRef.top__DOT__vgc_inst__DOT__regs[19U]))) {
                        vlSelfRef.top__DOT__vgc_inst__DOT__artist_gfx_addr 
                            = (0x0000ffffU & (((IData)(0x00000140U) 
                                               * vlSelfRef.top__DOT__vgc_inst__DOT__regs[19U]) 
                                              + (IData)(vlSelfRef.top__DOT__vgc_inst__DOT____Vcellinp__artist_inst__cmd_x0)));
                        vlSelfRef.top__DOT__vgc_inst__DOT__artist_gfx_wdata = 0U;
                        vlSelfRef.top__DOT__vgc_inst__DOT__artist_gfx_we = 1U;
                    }
                } else if ((3U == (IData)(vlSelfRef.top__DOT__vgc_inst__DOT__artist_cmd_code))) {
                    __Vdly__top__DOT__vgc_inst__DOT__artist_inst__DOT__x 
                        = vlSelfRef.top__DOT__vgc_inst__DOT____Vcellinp__artist_inst__cmd_x0;
                    __Vdly__top__DOT__vgc_inst__DOT__artist_inst__DOT__y 
                        = vlSelfRef.top__DOT__vgc_inst__DOT__regs[19U];
                    if ((vlSelfRef.top__DOT__vgc_inst__DOT__regs[23U] 
                         >= vlSelfRef.top__DOT__vgc_inst__DOT__regs[19U])) {
                        __Vdly__top__DOT__vgc_inst__DOT__artist_inst__DOT__dy 
                            = (0x000003ffU & (- (vlSelfRef.top__DOT__vgc_inst__DOT__regs[23U] 
                                                 - vlSelfRef.top__DOT__vgc_inst__DOT__regs[19U])));
                        vlSelfRef.top__DOT__vgc_inst__DOT__artist_inst__DOT__sy = 1U;
                    } else {
                        __Vdly__top__DOT__vgc_inst__DOT__artist_inst__DOT__dy 
                            = (0x000003ffU & (vlSelfRef.top__DOT__vgc_inst__DOT__regs[19U] 
                                              - vlSelfRef.top__DOT__vgc_inst__DOT__regs[23U]));
                        vlSelfRef.top__DOT__vgc_inst__DOT__artist_inst__DOT__sy = 0x03ffU;
                    }
                    __Vdly__top__DOT__vgc_inst__DOT__artist_inst__DOT__err 
                        = (0x000003ffU & ((((IData)(vlSelfRef.top__DOT__vgc_inst__DOT____Vcellinp__artist_inst__cmd_x1) 
                                            >= (IData)(vlSelfRef.top__DOT__vgc_inst__DOT____Vcellinp__artist_inst__cmd_x0))
                                            ? ((IData)(vlSelfRef.top__DOT__vgc_inst__DOT____Vcellinp__artist_inst__cmd_x1) 
                                               - (IData)(vlSelfRef.top__DOT__vgc_inst__DOT____Vcellinp__artist_inst__cmd_x0))
                                            : ((IData)(vlSelfRef.top__DOT__vgc_inst__DOT____Vcellinp__artist_inst__cmd_x0) 
                                               - (IData)(vlSelfRef.top__DOT__vgc_inst__DOT____Vcellinp__artist_inst__cmd_x1))) 
                                          + ((vlSelfRef.top__DOT__vgc_inst__DOT__regs[23U] 
                                              >= vlSelfRef.top__DOT__vgc_inst__DOT__regs[19U])
                                              ? (- 
                                                 (vlSelfRef.top__DOT__vgc_inst__DOT__regs[23U] 
                                                  - vlSelfRef.top__DOT__vgc_inst__DOT__regs[19U]))
                                              : (vlSelfRef.top__DOT__vgc_inst__DOT__regs[19U] 
                                                 - vlSelfRef.top__DOT__vgc_inst__DOT__regs[23U]))));
                    vlSelfRef.top__DOT__vgc_inst__DOT__artist_inst__DOT__x2 
                        = vlSelfRef.top__DOT__vgc_inst__DOT____Vcellinp__artist_inst__cmd_x1;
                    vlSelfRef.top__DOT__vgc_inst__DOT__artist_inst__DOT__y2 
                        = vlSelfRef.top__DOT__vgc_inst__DOT__regs[23U];
                    if (((IData)(vlSelfRef.top__DOT__vgc_inst__DOT____Vcellinp__artist_inst__cmd_x1) 
                         >= (IData)(vlSelfRef.top__DOT__vgc_inst__DOT____Vcellinp__artist_inst__cmd_x0))) {
                        __Vdly__top__DOT__vgc_inst__DOT__artist_inst__DOT__dx 
                            = (0x000003ffU & ((IData)(vlSelfRef.top__DOT__vgc_inst__DOT____Vcellinp__artist_inst__cmd_x1) 
                                              - (IData)(vlSelfRef.top__DOT__vgc_inst__DOT____Vcellinp__artist_inst__cmd_x0)));
                        vlSelfRef.top__DOT__vgc_inst__DOT__artist_inst__DOT__sx = 1U;
                    } else {
                        __Vdly__top__DOT__vgc_inst__DOT__artist_inst__DOT__dx 
                            = (0x000003ffU & ((IData)(vlSelfRef.top__DOT__vgc_inst__DOT____Vcellinp__artist_inst__cmd_x0) 
                                              - (IData)(vlSelfRef.top__DOT__vgc_inst__DOT____Vcellinp__artist_inst__cmd_x1)));
                        vlSelfRef.top__DOT__vgc_inst__DOT__artist_inst__DOT__sx = 0x03ffU;
                    }
                    __Vdly__top__DOT__vgc_inst__DOT__artist_busy = 1U;
                    __Vdly__top__DOT__vgc_inst__DOT__artist_inst__DOT__op = 3U;
                } else if (((5U == (IData)(vlSelfRef.top__DOT__vgc_inst__DOT__artist_cmd_code)) 
                            || (6U == (IData)(vlSelfRef.top__DOT__vgc_inst__DOT__artist_cmd_code)))) {
                    __Vdly__top__DOT__vgc_inst__DOT__artist_inst__DOT__x 
                        = vlSelfRef.top__DOT__vgc_inst__DOT____Vcellinp__artist_inst__cmd_x0;
                    __Vdly__top__DOT__vgc_inst__DOT__artist_inst__DOT__y 
                        = vlSelfRef.top__DOT__vgc_inst__DOT__regs[19U];
                    __Vdly__top__DOT__vgc_inst__DOT__artist_inst__DOT__cx 
                        = vlSelfRef.top__DOT__vgc_inst__DOT____Vcellinp__artist_inst__cmd_x0;
                    __Vdly__top__DOT__vgc_inst__DOT__artist_inst__DOT__cy 
                        = vlSelfRef.top__DOT__vgc_inst__DOT__regs[19U];
                    vlSelfRef.top__DOT__vgc_inst__DOT__artist_inst__DOT__x2 
                        = vlSelfRef.top__DOT__vgc_inst__DOT____Vcellinp__artist_inst__cmd_x1;
                    vlSelfRef.top__DOT__vgc_inst__DOT__artist_inst__DOT__y2 
                        = vlSelfRef.top__DOT__vgc_inst__DOT__regs[23U];
                    __Vdly__top__DOT__vgc_inst__DOT__artist_busy = 1U;
                    __Vdly__top__DOT__vgc_inst__DOT__artist_inst__DOT__op 
                        = vlSelfRef.top__DOT__vgc_inst__DOT__artist_cmd_code;
                } else if ((7U == (IData)(vlSelfRef.top__DOT__vgc_inst__DOT__artist_cmd_code))) {
                    __Vdly__top__DOT__vgc_inst__DOT__artist_inst__DOT__cx = 0U;
                    __Vdly__top__DOT__vgc_inst__DOT__artist_inst__DOT__cy = 0U;
                    __Vdly__top__DOT__vgc_inst__DOT__artist_busy = 1U;
                    __Vdly__top__DOT__vgc_inst__DOT__artist_inst__DOT__op = 7U;
                } else if ((4U == (IData)(vlSelfRef.top__DOT__vgc_inst__DOT__artist_cmd_code))) {
                    __Vdly__top__DOT__vgc_inst__DOT__artist_inst__DOT__x 
                        = vlSelfRef.top__DOT__vgc_inst__DOT____Vcellinp__artist_inst__cmd_x0;
                    __Vdly__top__DOT__vgc_inst__DOT__artist_inst__DOT__y 
                        = vlSelfRef.top__DOT__vgc_inst__DOT__regs[19U];
                    __Vdly__top__DOT__vgc_inst__DOT__artist_inst__DOT__dy 
                        = vlSelfRef.top__DOT__vgc_inst__DOT____Vcellinp__artist_inst__cmd_x1;
                    __Vdly__top__DOT__vgc_inst__DOT__artist_inst__DOT__err 
                        = (0x000003ffU & ((IData)(1U) 
                                          - (IData)(vlSelfRef.top__DOT__vgc_inst__DOT____Vcellinp__artist_inst__cmd_x1)));
                    vlSelfRef.top__DOT__vgc_inst__DOT__artist_inst__DOT__x2 
                        = vlSelfRef.top__DOT__vgc_inst__DOT____Vcellinp__artist_inst__cmd_x1;
                    __Vdly__top__DOT__vgc_inst__DOT__artist_inst__DOT__dx = 0U;
                    __Vdly__top__DOT__vgc_inst__DOT__artist_inst__DOT__octant_count = 0U;
                    __Vdly__top__DOT__vgc_inst__DOT__artist_busy = 1U;
                    __Vdly__top__DOT__vgc_inst__DOT__artist_inst__DOT__op = 4U;
                } else if ((9U == (IData)(vlSelfRef.top__DOT__vgc_inst__DOT__artist_cmd_code))) {
                    __Vdly__top__DOT__vgc_inst__DOT__artist_inst__DOT__x 
                        = vlSelfRef.top__DOT__vgc_inst__DOT____Vcellinp__artist_inst__cmd_x0;
                    __Vdly__top__DOT__vgc_inst__DOT__artist_inst__DOT__y 
                        = vlSelfRef.top__DOT__vgc_inst__DOT__regs[19U];
                    if (((0x0140U > (IData)(vlSelfRef.top__DOT__vgc_inst__DOT____Vcellinp__artist_inst__cmd_x0)) 
                         & (0xc8U > vlSelfRef.top__DOT__vgc_inst__DOT__regs[19U]))) {
                        vlSelfRef.top__DOT__vgc_inst__DOT__artist_gfx_raddr 
                            = (0x0000ffffU & (((IData)(0x00000140U) 
                                               * vlSelfRef.top__DOT__vgc_inst__DOT__regs[19U]) 
                                              + (IData)(vlSelfRef.top__DOT__vgc_inst__DOT____Vcellinp__artist_inst__cmd_x0)));
                        vlSelfRef.top__DOT__vgc_inst__DOT__artist_gfx_re = 1U;
                        vlSelfRef.top__DOT__vgc_inst__DOT__artist_inst__DOT__fs_a_addr = 0U;
                        vlSelfRef.top__DOT__vgc_inst__DOT__artist_inst__DOT__fs_a_din 
                            = ((vlSelfRef.top__DOT__vgc_inst__DOT__regs[19U] 
                                << 9U) | (IData)(vlSelfRef.top__DOT__vgc_inst__DOT____Vcellinp__artist_inst__cmd_x0));
                        vlSelfRef.top__DOT__vgc_inst__DOT__artist_inst__DOT__fs_a_we = 1U;
                        __Vdly__top__DOT__vgc_inst__DOT__artist_inst__DOT__fill_sp = 1U;
                        __Vdly__top__DOT__vgc_inst__DOT__artist_inst__DOT__paint_phase = 3U;
                        __Vdly__top__DOT__vgc_inst__DOT__artist_inst__DOT__paint_init = 1U;
                        __Vdly__top__DOT__vgc_inst__DOT__artist_busy = 1U;
                        __Vdly__top__DOT__vgc_inst__DOT__artist_inst__DOT__op = 9U;
                    }
                } else if ((0U < (IData)(vlSelfRef.top__DOT__vgc_inst__DOT__fio_name_len))) {
                    __Vfunc_top__DOT__vgc_inst__DOT__artist_inst__DOT__gt_char__12__Vfuncout 
                        = (0x000000ffU & vlSelfRef.top__DOT__vgc_inst__DOT__fio_name_flat[0U]);
                    vlSelfRef.top__DOT__vgc_inst__DOT__artist_inst__DOT__gt_pen_x 
                        = vlSelfRef.top__DOT__vgc_inst__DOT____Vcellinp__artist_inst__cmd_x0;
                    vlSelfRef.top__DOT__vgc_inst__DOT__artist_inst__DOT__gt_start_y 
                        = vlSelfRef.top__DOT__vgc_inst__DOT__regs[19U];
                    vlSelfRef.top__DOT__vgc_inst__DOT__artist_inst__DOT__gt_scale 
                        = ((1U > vlSelfRef.top__DOT__vgc_inst__DOT__regs[22U])
                            ? 1U : vlSelfRef.top__DOT__vgc_inst__DOT__regs[22U]);
                    __Vdly__top__DOT__vgc_inst__DOT__artist_inst__DOT__gt_char_idx = 0U;
                    vlSelfRef.top__DOT__vgc_inst__DOT__artist_inst__DOT__gt_len 
                        = vlSelfRef.top__DOT__vgc_inst__DOT__fio_name_len;
                    __Vdly__top__DOT__vgc_inst__DOT__artist_inst__DOT__gt_font_row = 0U;
                    vlSelfRef.top__DOT__vgc_inst__DOT__artist_inst__DOT__gt_font_col = 0U;
                    vlSelfRef.top__DOT__vgc_inst__DOT__artist_inst__DOT__gt_scale_x = 0U;
                    vlSelfRef.top__DOT__vgc_inst__DOT__artist_inst__DOT__gt_scale_y = 0U;
                    vlSelfRef.top__DOT__vgc_inst__DOT__artist_inst__DOT__gt_cur_char 
                        = __Vfunc_top__DOT__vgc_inst__DOT__artist_inst__DOT__gt_char__12__Vfuncout;
                    vlSelfRef.top__DOT__vgc_inst__DOT__artist_font_addr 
                        = (([&]() {
                                __Vfunc_top__DOT__vgc_inst__DOT__artist_inst__DOT__gt_char__13__Vfuncout 
                                    = (0x000000ffU 
                                       & vlSelfRef.top__DOT__vgc_inst__DOT__fio_name_flat[0U]);
                            }(), (IData)(__Vfunc_top__DOT__vgc_inst__DOT__artist_inst__DOT__gt_char__13__Vfuncout)) 
                           << 3U);
                    vlSelfRef.top__DOT__vgc_inst__DOT__artist_font_re = 1U;
                    __Vdly__top__DOT__vgc_inst__DOT__artist_inst__DOT__gt_font_pending = 1U;
                    __Vdly__top__DOT__vgc_inst__DOT__artist_busy = 1U;
                    __Vdly__top__DOT__vgc_inst__DOT__artist_inst__DOT__op = 0x0aU;
                }
            }
        }
    }
    if ((1U & (~ (IData)(vlSelfRef.rst)))) {
        if (vlSelfRef.top__DOT__cpu_we) {
            if ((0xb9a3U == (IData)(vlSelfRef.top__DOT__cpu_addr))) {
                vlSelfRef.top__DOT__vgc_inst__DOT__fio_name_len 
                    = (0x0000003fU & (IData)(vlSelfRef.top__DOT__cpu_dout));
            }
        }
    }
    vlSelfRef.top__DOT__vgc_inst__DOT__char_b_dout 
        = ((0x07cfU >= (IData)(vlSelfRef.top__DOT__vgc_inst__DOT__text_inst__DOT__char_b_addr))
            ? vlSelfRef.top__DOT__vgc_inst__DOT__text_inst__DOT__char_mem__DOT__mem
           [vlSelfRef.top__DOT__vgc_inst__DOT__text_inst__DOT__char_b_addr]
            : 0U);
    vlSelfRef.dbg_cpu_a = vlSelfRef.top__DOT__cpu_inst__DOT__AXYS[0U];
    vlSelfRef.dbg_cpu_x = vlSelfRef.top__DOT__cpu_inst__DOT__AXYS[1U];
    vlSelfRef.dbg_cpu_y = vlSelfRef.top__DOT__cpu_inst__DOT__AXYS[2U];
    vlSelfRef.dbg_cpu_sp = vlSelfRef.top__DOT__cpu_inst__DOT__AXYS[3U];
    vlSelfRef.top__DOT__cpu_inst__DOT__adj_bcd = ((IData)(vlSelfRef.top__DOT__cpu_inst__DOT__adc_sbc) 
                                                  & (IData)(vlSelfRef.top__DOT__cpu_inst__DOT__D));
    vlSelfRef.top__DOT__vgc_inst__DOT__text_row_d1 
        = vlSelfRef.top__DOT__vgc_inst__DOT__text_row;
    vlSelfRef.top__DOT__vgc_inst__DOT__text_col_d1 
        = (0x0000007fU & ((IData)(vlSelfRef.top__DOT__vgc_inst__DOT__h_count) 
                          >> 3U));
    vlSelfRef.top__DOT__vgc_inst__DOT__h_sync_area_d2 
        = vlSelfRef.top__DOT__vgc_inst__DOT__h_sync_area_d1;
    vlSelfRef.top__DOT__vgc_inst__DOT__v_sync_area_d2 
        = vlSelfRef.top__DOT__vgc_inst__DOT__v_sync_area_d1;
    vlSelfRef.top__DOT__vgc_inst__DOT__visible_d2 = vlSelfRef.top__DOT__vgc_inst__DOT__visible_d1;
    vlSelfRef.top__DOT__vgc_inst__DOT__font_pixel_d1 
        = (7U & (IData)(vlSelfRef.top__DOT__vgc_inst__DOT__h_count));
    vlSelfRef.top__DOT__vgc_inst__DOT__gfx_inst__DOT__gfx_b_addr 
        = (0x0000ffffU & (((IData)(0x00000140U) * (IData)(vlSelfRef.top__DOT__vgc_inst__DOT__gfx_y)) 
                          + (0x000001ffU & ((IData)(vlSelfRef.top__DOT__vgc_inst__DOT__h_count) 
                                            >> 1U))));
    if (vlSelfRef.__VdfgRegularize_hebeb780c_0_25) {
        vlSelfRef.top__DOT__vgc_inst__DOT__tile_opaque_d1 
            = vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__s3_tile_pri;
        vlSelfRef.top__DOT__vgc_inst__DOT__tile_rgb_d1 
            = vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__pr_dout_b;
    } else {
        vlSelfRef.top__DOT__vgc_inst__DOT__tile_opaque_d1 = 0U;
        vlSelfRef.top__DOT__vgc_inst__DOT__tile_rgb_d1 = 0U;
    }
    vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__td_dout_a 
        = vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__tile_data_ram__DOT__mem
        [vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__td_addr_a];
    vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__blt_tile_rd_pending 
        = (((IData)(vlSelfRef.top__DOT__vgc_inst__DOT__tile_blt_re) 
            & (~ (IData)(vlSelfRef.top__DOT__vgc_inst__DOT__tile_blt_we))) 
           & (0U == (IData)(vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__dma_state)));
    vlSelfRef.top__DOT__vgc_inst__DOT__in_text_area_d2 
        = vlSelfRef.top__DOT__vgc_inst__DOT__in_text_area_d1;
    vlSelfRef.top__DOT__vgc_inst__DOT__gfx_x_d2 = vlSelfRef.top__DOT__vgc_inst__DOT__gfx_x_d1;
    vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__nt_dout_b 
        = vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__nametable_ram__DOT__mem
        [vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__nt_addr_b];
    vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__s1_tile_size16 
        = vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__tile_size16;
    vlSelfRef.dbg_cpu_pc = vlSelfRef.top__DOT__cpu_inst__DOT__PC;
    vlSelfRef.__VdfgRegularize_hebeb780c_0_4 = (((IData)(vlSelfRef.top__DOT__cpu_inst__DOT__ABH) 
                                                 << 8U) 
                                                | (IData)(vlSelfRef.top__DOT__cpu_inst__DOT__ABL));
    vlSelfRef.top__DOT__cpu_din = (((IData)(vlSelfRef.top__DOT__xmc_win_sel) 
                                    & (IData)(vlSelfRef.top__DOT__xmc_win_enabled))
                                    ? vlSelfRef.top__DOT__xram
                                   [vlSelfRef.top__DOT__xmc_addr]
                                    : (((IData)(vlSelfRef.top__DOT__xmc_win_sel) 
                                        & (~ (IData)(vlSelfRef.top__DOT__xmc_win_enabled)))
                                        ? 0xffU : ((IData)(vlSelfRef.top__DOT__xmc_reg_sel)
                                                    ? vlSelfRef.top__DOT__xmc_regs
                                                   [
                                                   (0x0000003fU 
                                                    & (IData)(vlSelfRef.top__DOT__cpu_addr))]
                                                    : 
                                                   ((IData)(vlSelfRef.top__DOT__blt_reg_sel)
                                                     ? (IData)(vlSelfRef.top__DOT__blt_cpu_rdata)
                                                     : 
                                                    ((IData)(vlSelfRef.top__DOT__sid1_reg_sel)
                                                      ? (IData)(vlSymsp->TOP__top__DOT__sid1_inst.dout)
                                                      : 
                                                     ((IData)(vlSelfRef.top__DOT__sid2_reg_sel)
                                                       ? (IData)(vlSymsp->TOP__top__DOT__sid2_inst.dout)
                                                       : 
                                                      ((0xd440U 
                                                        == (IData)(vlSelfRef.top__DOT__cpu_addr))
                                                        ? (IData)(vlSelfRef.top__DOT__sid_cfg_reg)
                                                        : 
                                                       ((0xc000U 
                                                         <= (IData)(vlSelfRef.top__DOT__cpu_addr))
                                                         ? 
                                                        ((IData)(vlSelfRef.top__DOT__ext_rom_active)
                                                          ? vlSelfRef.top__DOT__ext_rom
                                                         [
                                                         (0x00003fffU 
                                                          & (IData)(vlSelfRef.top__DOT__cpu_addr))]
                                                          : vlSelfRef.top__DOT__basic_rom
                                                         [
                                                         (0x00003fffU 
                                                          & (IData)(vlSelfRef.top__DOT__cpu_addr))])
                                                         : vlSelfRef.top__DOT__ram
                                                        [vlSelfRef.top__DOT__cpu_addr]))))))));
    vlSelfRef.__VdfgRegularize_hebeb780c_0_3 = (1U 
                                                & (((~ (IData)(vlSelfRef.top__DOT__cpu_inst__DOT__I)) 
                                                    & (~ (IData)(vlSelfRef.irq_n))) 
                                                   | (IData)(vlSelfRef.top__DOT__cpu_inst__DOT__NMI_edge)));
    vlSelfRef.top__DOT__vgc_inst__DOT__sprite_inst__DOT__spr_shape[0U] 
        = (0x0000000fU & (IData)(vlSelfRef.top__DOT__vgc_inst__DOT__spr_shape_flat));
    vlSelfRef.top__DOT__vgc_inst__DOT__sprite_inst__DOT__spr_shape[1U] 
        = (0x0000000fU & (IData)((vlSelfRef.top__DOT__vgc_inst__DOT__spr_shape_flat 
                                  >> 4U)));
    vlSelfRef.top__DOT__vgc_inst__DOT__sprite_inst__DOT__spr_shape[2U] 
        = (0x0000000fU & (IData)((vlSelfRef.top__DOT__vgc_inst__DOT__spr_shape_flat 
                                  >> 8U)));
    vlSelfRef.top__DOT__vgc_inst__DOT__sprite_inst__DOT__spr_shape[3U] 
        = (0x0000000fU & (IData)((vlSelfRef.top__DOT__vgc_inst__DOT__spr_shape_flat 
                                  >> 0x0cU)));
    vlSelfRef.top__DOT__vgc_inst__DOT__sprite_inst__DOT__spr_shape[4U] 
        = (0x0000000fU & (IData)((vlSelfRef.top__DOT__vgc_inst__DOT__spr_shape_flat 
                                  >> 0x10U)));
    vlSelfRef.top__DOT__vgc_inst__DOT__sprite_inst__DOT__spr_shape[5U] 
        = (0x0000000fU & (IData)((vlSelfRef.top__DOT__vgc_inst__DOT__spr_shape_flat 
                                  >> 0x14U)));
    vlSelfRef.top__DOT__vgc_inst__DOT__sprite_inst__DOT__spr_shape[6U] 
        = (0x0000000fU & (IData)((vlSelfRef.top__DOT__vgc_inst__DOT__spr_shape_flat 
                                  >> 0x18U)));
    vlSelfRef.top__DOT__vgc_inst__DOT__sprite_inst__DOT__spr_shape[7U] 
        = (0x0000000fU & (IData)((vlSelfRef.top__DOT__vgc_inst__DOT__spr_shape_flat 
                                  >> 0x1cU)));
    vlSelfRef.top__DOT__vgc_inst__DOT__sprite_inst__DOT__spr_shape[8U] 
        = (0x0000000fU & (IData)((vlSelfRef.top__DOT__vgc_inst__DOT__spr_shape_flat 
                                  >> 0x20U)));
    vlSelfRef.top__DOT__vgc_inst__DOT__sprite_inst__DOT__spr_shape[9U] 
        = (0x0000000fU & (IData)((vlSelfRef.top__DOT__vgc_inst__DOT__spr_shape_flat 
                                  >> 0x24U)));
    vlSelfRef.top__DOT__vgc_inst__DOT__sprite_inst__DOT__spr_shape[10U] 
        = (0x0000000fU & (IData)((vlSelfRef.top__DOT__vgc_inst__DOT__spr_shape_flat 
                                  >> 0x28U)));
    vlSelfRef.top__DOT__vgc_inst__DOT__sprite_inst__DOT__spr_shape[11U] 
        = (0x0000000fU & (IData)((vlSelfRef.top__DOT__vgc_inst__DOT__spr_shape_flat 
                                  >> 0x2cU)));
    vlSelfRef.top__DOT__vgc_inst__DOT__sprite_inst__DOT__spr_shape[12U] 
        = (0x0000000fU & (IData)((vlSelfRef.top__DOT__vgc_inst__DOT__spr_shape_flat 
                                  >> 0x30U)));
    vlSelfRef.top__DOT__vgc_inst__DOT__sprite_inst__DOT__spr_shape[13U] 
        = (0x0000000fU & (IData)((vlSelfRef.top__DOT__vgc_inst__DOT__spr_shape_flat 
                                  >> 0x34U)));
    vlSelfRef.top__DOT__vgc_inst__DOT__sprite_inst__DOT__spr_shape[14U] 
        = (0x0000000fU & (IData)((vlSelfRef.top__DOT__vgc_inst__DOT__spr_shape_flat 
                                  >> 0x38U)));
    vlSelfRef.top__DOT__vgc_inst__DOT__sprite_inst__DOT__spr_shape[15U] 
        = (0x0000000fU & (IData)((vlSelfRef.top__DOT__vgc_inst__DOT__spr_shape_flat 
                                  >> 0x3cU)));
    vlSelfRef.top__DOT__vgc_inst__DOT__sprite_inst__DOT__spr_trans[0U] 
        = (0x0000000fU & (IData)(vlSelfRef.top__DOT__vgc_inst__DOT__spr_trans_flat));
    vlSelfRef.top__DOT__vgc_inst__DOT__sprite_inst__DOT__spr_trans[1U] 
        = (0x0000000fU & (IData)((vlSelfRef.top__DOT__vgc_inst__DOT__spr_trans_flat 
                                  >> 4U)));
    vlSelfRef.top__DOT__vgc_inst__DOT__sprite_inst__DOT__spr_trans[2U] 
        = (0x0000000fU & (IData)((vlSelfRef.top__DOT__vgc_inst__DOT__spr_trans_flat 
                                  >> 8U)));
    vlSelfRef.top__DOT__vgc_inst__DOT__sprite_inst__DOT__spr_trans[3U] 
        = (0x0000000fU & (IData)((vlSelfRef.top__DOT__vgc_inst__DOT__spr_trans_flat 
                                  >> 0x0cU)));
    vlSelfRef.top__DOT__vgc_inst__DOT__sprite_inst__DOT__spr_trans[4U] 
        = (0x0000000fU & (IData)((vlSelfRef.top__DOT__vgc_inst__DOT__spr_trans_flat 
                                  >> 0x10U)));
    vlSelfRef.top__DOT__vgc_inst__DOT__sprite_inst__DOT__spr_trans[5U] 
        = (0x0000000fU & (IData)((vlSelfRef.top__DOT__vgc_inst__DOT__spr_trans_flat 
                                  >> 0x14U)));
    vlSelfRef.top__DOT__vgc_inst__DOT__sprite_inst__DOT__spr_trans[6U] 
        = (0x0000000fU & (IData)((vlSelfRef.top__DOT__vgc_inst__DOT__spr_trans_flat 
                                  >> 0x18U)));
    vlSelfRef.top__DOT__vgc_inst__DOT__sprite_inst__DOT__spr_trans[7U] 
        = (0x0000000fU & (IData)((vlSelfRef.top__DOT__vgc_inst__DOT__spr_trans_flat 
                                  >> 0x1cU)));
    vlSelfRef.top__DOT__vgc_inst__DOT__sprite_inst__DOT__spr_trans[8U] 
        = (0x0000000fU & (IData)((vlSelfRef.top__DOT__vgc_inst__DOT__spr_trans_flat 
                                  >> 0x20U)));
    vlSelfRef.top__DOT__vgc_inst__DOT__sprite_inst__DOT__spr_trans[9U] 
        = (0x0000000fU & (IData)((vlSelfRef.top__DOT__vgc_inst__DOT__spr_trans_flat 
                                  >> 0x24U)));
    vlSelfRef.top__DOT__vgc_inst__DOT__sprite_inst__DOT__spr_trans[10U] 
        = (0x0000000fU & (IData)((vlSelfRef.top__DOT__vgc_inst__DOT__spr_trans_flat 
                                  >> 0x28U)));
    vlSelfRef.top__DOT__vgc_inst__DOT__sprite_inst__DOT__spr_trans[11U] 
        = (0x0000000fU & (IData)((vlSelfRef.top__DOT__vgc_inst__DOT__spr_trans_flat 
                                  >> 0x2cU)));
    vlSelfRef.top__DOT__vgc_inst__DOT__sprite_inst__DOT__spr_trans[12U] 
        = (0x0000000fU & (IData)((vlSelfRef.top__DOT__vgc_inst__DOT__spr_trans_flat 
                                  >> 0x30U)));
    vlSelfRef.top__DOT__vgc_inst__DOT__sprite_inst__DOT__spr_trans[13U] 
        = (0x0000000fU & (IData)((vlSelfRef.top__DOT__vgc_inst__DOT__spr_trans_flat 
                                  >> 0x34U)));
    vlSelfRef.top__DOT__vgc_inst__DOT__sprite_inst__DOT__spr_trans[14U] 
        = (0x0000000fU & (IData)((vlSelfRef.top__DOT__vgc_inst__DOT__spr_trans_flat 
                                  >> 0x38U)));
    vlSelfRef.top__DOT__vgc_inst__DOT__sprite_inst__DOT__spr_trans[15U] 
        = (0x0000000fU & (IData)((vlSelfRef.top__DOT__vgc_inst__DOT__spr_trans_flat 
                                  >> 0x3cU)));
    vlSelfRef.top__DOT__vgc_inst__DOT__sprite_inst__DOT__spr_eval_state 
        = __Vdly__top__DOT__vgc_inst__DOT__sprite_inst__DOT__spr_eval_state;
    vlSelfRef.top__DOT__vgc_inst__DOT__sprite_inst__DOT__spr_eval_idx 
        = __Vdly__top__DOT__vgc_inst__DOT__sprite_inst__DOT__spr_eval_idx;
    vlSelfRef.top__DOT__vgc_inst__DOT__sprite_inst__DOT__spr_clear_x 
        = __Vdly__top__DOT__vgc_inst__DOT__sprite_inst__DOT__spr_clear_x;
    if (__VdlySet__top__DOT__vgc_inst__DOT__sprite_inst__DOT__spr_row_data__v0) {
        vlSelfRef.top__DOT__vgc_inst__DOT__sprite_inst__DOT__spr_row_data[7U] 
            = __VdlyVal__top__DOT__vgc_inst__DOT__sprite_inst__DOT__spr_row_data__v0;
    }
    if (__VdlySet__top__DOT__vgc_inst__DOT__sprite_inst__DOT__spr_row_data__v1) {
        vlSelfRef.top__DOT__vgc_inst__DOT__sprite_inst__DOT__spr_row_data[__VdlyDim0__top__DOT__vgc_inst__DOT__sprite_inst__DOT__spr_row_data__v1] 
            = __VdlyVal__top__DOT__vgc_inst__DOT__sprite_inst__DOT__spr_row_data__v1;
    }
    vlSelfRef.top__DOT__blt_ram_addr = 0U;
    vlSelfRef.top__DOT__blt_xram_addr = 0U;
    vlSelfRef.top__DOT__blt_vgc_addr = 0U;
    if (((2U == (IData)(vlSelfRef.top__DOT__blt_inst__DOT__state)) 
         | (4U == (IData)(vlSelfRef.top__DOT__blt_inst__DOT__state)))) {
        if ((1U & (~ (IData)(vlSelfRef.top__DOT__blt_inst__DOT__fill_mode)))) {
            if ((0U == (IData)(vlSelfRef.top__DOT__blt_inst__DOT__src_space))) {
                vlSelfRef.top__DOT__blt_ram_addr = 
                    (0x0000ffffU & top__DOT__blt_inst__DOT__src_addr);
            }
            if ((0U != (IData)(vlSelfRef.top__DOT__blt_inst__DOT__src_space))) {
                if ((5U == (IData)(vlSelfRef.top__DOT__blt_inst__DOT__src_space))) {
                    vlSelfRef.top__DOT__blt_xram_addr 
                        = (0x0007ffffU & top__DOT__blt_inst__DOT__src_addr);
                }
                if ((5U != (IData)(vlSelfRef.top__DOT__blt_inst__DOT__src_space))) {
                    vlSelfRef.top__DOT__blt_vgc_addr 
                        = (0x0000ffffU & top__DOT__blt_inst__DOT__src_addr);
                }
            }
        }
    }
    vlSelfRef.top__DOT__vgc_inst__DOT__fio_name_flat[0U] 
        = ((0xff000000U & vlSelfRef.top__DOT__vgc_inst__DOT__fio_name_flat[0U]) 
           | ((vlSelfRef.top__DOT__vgc_inst__DOT__fio_name[2U] 
               << 0x00000010U) | ((vlSelfRef.top__DOT__vgc_inst__DOT__fio_name[1U] 
                                   << 8U) | vlSelfRef.top__DOT__vgc_inst__DOT__fio_name[0U])));
    vlSelfRef.top__DOT__vgc_inst__DOT__fio_name_flat[0U] 
        = ((0x00ffffffU & vlSelfRef.top__DOT__vgc_inst__DOT__fio_name_flat[0U]) 
           | (vlSelfRef.top__DOT__vgc_inst__DOT__fio_name[3U] 
              << 0x00000018U));
    vlSelfRef.top__DOT__vgc_inst__DOT__fio_name_flat[1U] 
        = ((0xffff0000U & vlSelfRef.top__DOT__vgc_inst__DOT__fio_name_flat[1U]) 
           | (0x00ffffffU & ((0x00ffff00U & (vlSelfRef.top__DOT__vgc_inst__DOT__fio_name[5U] 
                                             << 8U)) 
                             | ((0x00ffffffU & vlSelfRef.top__DOT__vgc_inst__DOT__fio_name[4U]) 
                                | (vlSelfRef.top__DOT__vgc_inst__DOT__fio_name[3U] 
                                   >> 8U)))));
    vlSelfRef.top__DOT__vgc_inst__DOT__fio_name_flat[1U] 
        = ((0x0000ffffU & vlSelfRef.top__DOT__vgc_inst__DOT__fio_name_flat[1U]) 
           | (0xffff0000U & ((vlSelfRef.top__DOT__vgc_inst__DOT__fio_name[7U] 
                              << 0x00000018U) | (vlSelfRef.top__DOT__vgc_inst__DOT__fio_name[6U] 
                                                 << 0x00000010U))));
    vlSelfRef.top__DOT__vgc_inst__DOT__fio_name_flat[2U] 
        = ((0xffffff00U & vlSelfRef.top__DOT__vgc_inst__DOT__fio_name_flat[2U]) 
           | (0x0000ffffU & ((0x0000ffffU & vlSelfRef.top__DOT__vgc_inst__DOT__fio_name[8U]) 
                             | ((0x0000ffffU & (vlSelfRef.top__DOT__vgc_inst__DOT__fio_name[7U] 
                                                >> 8U)) 
                                | (vlSelfRef.top__DOT__vgc_inst__DOT__fio_name[6U] 
                                   >> 0x00000010U)))));
    vlSelfRef.top__DOT__vgc_inst__DOT__fio_name_flat[2U] 
        = ((0x000000ffU & vlSelfRef.top__DOT__vgc_inst__DOT__fio_name_flat[2U]) 
           | (0xffffff00U & ((vlSelfRef.top__DOT__vgc_inst__DOT__fio_name[11U] 
                              << 0x00000018U) | ((vlSelfRef.top__DOT__vgc_inst__DOT__fio_name[10U] 
                                                  << 0x00000010U) 
                                                 | (vlSelfRef.top__DOT__vgc_inst__DOT__fio_name[9U] 
                                                    << 8U)))));
    vlSelfRef.top__DOT__vgc_inst__DOT__fio_name_flat[3U] 
        = ((0xff000000U & vlSelfRef.top__DOT__vgc_inst__DOT__fio_name_flat[3U]) 
           | ((vlSelfRef.top__DOT__vgc_inst__DOT__fio_name[14U] 
               << 0x00000010U) | ((vlSelfRef.top__DOT__vgc_inst__DOT__fio_name[13U] 
                                   << 8U) | vlSelfRef.top__DOT__vgc_inst__DOT__fio_name[12U])));
    vlSelfRef.top__DOT__vgc_inst__DOT__fio_name_flat[3U] 
        = ((0x00ffffffU & vlSelfRef.top__DOT__vgc_inst__DOT__fio_name_flat[3U]) 
           | (vlSelfRef.top__DOT__vgc_inst__DOT__fio_name[15U] 
              << 0x00000018U));
    vlSelfRef.top__DOT__vgc_inst__DOT__fio_name_flat[4U] 
        = ((0xffff0000U & vlSelfRef.top__DOT__vgc_inst__DOT__fio_name_flat[4U]) 
           | (0x00ffffffU & ((0x00ffff00U & (vlSelfRef.top__DOT__vgc_inst__DOT__fio_name[17U] 
                                             << 8U)) 
                             | ((0x00ffffffU & vlSelfRef.top__DOT__vgc_inst__DOT__fio_name[16U]) 
                                | (vlSelfRef.top__DOT__vgc_inst__DOT__fio_name[15U] 
                                   >> 8U)))));
    vlSelfRef.top__DOT__vgc_inst__DOT__fio_name_flat[4U] 
        = ((0x0000ffffU & vlSelfRef.top__DOT__vgc_inst__DOT__fio_name_flat[4U]) 
           | (0xffff0000U & ((vlSelfRef.top__DOT__vgc_inst__DOT__fio_name[19U] 
                              << 0x00000018U) | (vlSelfRef.top__DOT__vgc_inst__DOT__fio_name[18U] 
                                                 << 0x00000010U))));
    vlSelfRef.top__DOT__vgc_inst__DOT__fio_name_flat[5U] 
        = ((0xffffff00U & vlSelfRef.top__DOT__vgc_inst__DOT__fio_name_flat[5U]) 
           | (0x0000ffffU & ((0x0000ffffU & vlSelfRef.top__DOT__vgc_inst__DOT__fio_name[20U]) 
                             | ((0x0000ffffU & (vlSelfRef.top__DOT__vgc_inst__DOT__fio_name[19U] 
                                                >> 8U)) 
                                | (vlSelfRef.top__DOT__vgc_inst__DOT__fio_name[18U] 
                                   >> 0x00000010U)))));
    vlSelfRef.top__DOT__vgc_inst__DOT__fio_name_flat[5U] 
        = ((0x000000ffU & vlSelfRef.top__DOT__vgc_inst__DOT__fio_name_flat[5U]) 
           | (0xffffff00U & ((vlSelfRef.top__DOT__vgc_inst__DOT__fio_name[23U] 
                              << 0x00000018U) | ((vlSelfRef.top__DOT__vgc_inst__DOT__fio_name[22U] 
                                                  << 0x00000010U) 
                                                 | (vlSelfRef.top__DOT__vgc_inst__DOT__fio_name[21U] 
                                                    << 8U)))));
    vlSelfRef.top__DOT__vgc_inst__DOT__fio_name_flat[6U] 
        = ((0xff000000U & vlSelfRef.top__DOT__vgc_inst__DOT__fio_name_flat[6U]) 
           | ((vlSelfRef.top__DOT__vgc_inst__DOT__fio_name[26U] 
               << 0x00000010U) | ((vlSelfRef.top__DOT__vgc_inst__DOT__fio_name[25U] 
                                   << 8U) | vlSelfRef.top__DOT__vgc_inst__DOT__fio_name[24U])));
    vlSelfRef.top__DOT__vgc_inst__DOT__fio_name_flat[6U] 
        = ((0x00ffffffU & vlSelfRef.top__DOT__vgc_inst__DOT__fio_name_flat[6U]) 
           | (vlSelfRef.top__DOT__vgc_inst__DOT__fio_name[27U] 
              << 0x00000018U));
    vlSelfRef.top__DOT__vgc_inst__DOT__fio_name_flat[7U] 
        = ((0xffff0000U & vlSelfRef.top__DOT__vgc_inst__DOT__fio_name_flat[7U]) 
           | (0x00ffffffU & ((0x00ffff00U & (vlSelfRef.top__DOT__vgc_inst__DOT__fio_name[29U] 
                                             << 8U)) 
                             | ((0x00ffffffU & vlSelfRef.top__DOT__vgc_inst__DOT__fio_name[28U]) 
                                | (vlSelfRef.top__DOT__vgc_inst__DOT__fio_name[27U] 
                                   >> 8U)))));
    vlSelfRef.top__DOT__vgc_inst__DOT__fio_name_flat[7U] 
        = ((0x0000ffffU & vlSelfRef.top__DOT__vgc_inst__DOT__fio_name_flat[7U]) 
           | (0xffff0000U & ((vlSelfRef.top__DOT__vgc_inst__DOT__fio_name[31U] 
                              << 0x00000018U) | (vlSelfRef.top__DOT__vgc_inst__DOT__fio_name[30U] 
                                                 << 0x00000010U))));
    vlSelfRef.top__DOT__vgc_inst__DOT__fio_name_flat[8U] 
        = ((0xffffff00U & vlSelfRef.top__DOT__vgc_inst__DOT__fio_name_flat[8U]) 
           | (0x0000ffffU & ((0x0000ffffU & vlSelfRef.top__DOT__vgc_inst__DOT__fio_name[32U]) 
                             | ((0x0000ffffU & (vlSelfRef.top__DOT__vgc_inst__DOT__fio_name[31U] 
                                                >> 8U)) 
                                | (vlSelfRef.top__DOT__vgc_inst__DOT__fio_name[30U] 
                                   >> 0x00000010U)))));
    vlSelfRef.top__DOT__vgc_inst__DOT__fio_name_flat[8U] 
        = ((0x000000ffU & vlSelfRef.top__DOT__vgc_inst__DOT__fio_name_flat[8U]) 
           | (0xffffff00U & ((vlSelfRef.top__DOT__vgc_inst__DOT__fio_name[35U] 
                              << 0x00000018U) | ((vlSelfRef.top__DOT__vgc_inst__DOT__fio_name[34U] 
                                                  << 0x00000010U) 
                                                 | (vlSelfRef.top__DOT__vgc_inst__DOT__fio_name[33U] 
                                                    << 8U)))));
    vlSelfRef.top__DOT__vgc_inst__DOT__fio_name_flat[9U] 
        = ((0xff000000U & vlSelfRef.top__DOT__vgc_inst__DOT__fio_name_flat[9U]) 
           | ((vlSelfRef.top__DOT__vgc_inst__DOT__fio_name[38U] 
               << 0x00000010U) | ((vlSelfRef.top__DOT__vgc_inst__DOT__fio_name[37U] 
                                   << 8U) | vlSelfRef.top__DOT__vgc_inst__DOT__fio_name[36U])));
    vlSelfRef.top__DOT__vgc_inst__DOT__fio_name_flat[9U] 
        = ((0x00ffffffU & vlSelfRef.top__DOT__vgc_inst__DOT__fio_name_flat[9U]) 
           | (vlSelfRef.top__DOT__vgc_inst__DOT__fio_name[39U] 
              << 0x00000018U));
    vlSelfRef.top__DOT__vgc_inst__DOT__fio_name_flat[10U] 
        = ((0xffff0000U & vlSelfRef.top__DOT__vgc_inst__DOT__fio_name_flat[10U]) 
           | (0x00ffffffU & ((0x00ffff00U & (vlSelfRef.top__DOT__vgc_inst__DOT__fio_name[41U] 
                                             << 8U)) 
                             | ((0x00ffffffU & vlSelfRef.top__DOT__vgc_inst__DOT__fio_name[40U]) 
                                | (vlSelfRef.top__DOT__vgc_inst__DOT__fio_name[39U] 
                                   >> 8U)))));
    vlSelfRef.top__DOT__vgc_inst__DOT__fio_name_flat[10U] 
        = ((0x0000ffffU & vlSelfRef.top__DOT__vgc_inst__DOT__fio_name_flat[10U]) 
           | (0xffff0000U & ((vlSelfRef.top__DOT__vgc_inst__DOT__fio_name[43U] 
                              << 0x00000018U) | (vlSelfRef.top__DOT__vgc_inst__DOT__fio_name[42U] 
                                                 << 0x00000010U))));
    vlSelfRef.top__DOT__vgc_inst__DOT__fio_name_flat[11U] 
        = ((0xffffff00U & vlSelfRef.top__DOT__vgc_inst__DOT__fio_name_flat[11U]) 
           | (0x0000ffffU & ((0x0000ffffU & vlSelfRef.top__DOT__vgc_inst__DOT__fio_name[44U]) 
                             | ((0x0000ffffU & (vlSelfRef.top__DOT__vgc_inst__DOT__fio_name[43U] 
                                                >> 8U)) 
                                | (vlSelfRef.top__DOT__vgc_inst__DOT__fio_name[42U] 
                                   >> 0x00000010U)))));
    vlSelfRef.top__DOT__vgc_inst__DOT__fio_name_flat[11U] 
        = ((0x000000ffU & vlSelfRef.top__DOT__vgc_inst__DOT__fio_name_flat[11U]) 
           | (0xffffff00U & ((vlSelfRef.top__DOT__vgc_inst__DOT__fio_name[47U] 
                              << 0x00000018U) | ((vlSelfRef.top__DOT__vgc_inst__DOT__fio_name[46U] 
                                                  << 0x00000010U) 
                                                 | (vlSelfRef.top__DOT__vgc_inst__DOT__fio_name[45U] 
                                                    << 8U)))));
    vlSelfRef.top__DOT__vgc_inst__DOT__fio_name_flat[12U] 
        = ((0xff000000U & vlSelfRef.top__DOT__vgc_inst__DOT__fio_name_flat[12U]) 
           | ((vlSelfRef.top__DOT__vgc_inst__DOT__fio_name[50U] 
               << 0x00000010U) | ((vlSelfRef.top__DOT__vgc_inst__DOT__fio_name[49U] 
                                   << 8U) | vlSelfRef.top__DOT__vgc_inst__DOT__fio_name[48U])));
    vlSelfRef.top__DOT__vgc_inst__DOT__fio_name_flat[12U] 
        = ((0x00ffffffU & vlSelfRef.top__DOT__vgc_inst__DOT__fio_name_flat[12U]) 
           | (vlSelfRef.top__DOT__vgc_inst__DOT__fio_name[51U] 
              << 0x00000018U));
    vlSelfRef.top__DOT__vgc_inst__DOT__fio_name_flat[13U] 
        = ((0xffff0000U & vlSelfRef.top__DOT__vgc_inst__DOT__fio_name_flat[13U]) 
           | (0x00ffffffU & ((0x00ffff00U & (vlSelfRef.top__DOT__vgc_inst__DOT__fio_name[53U] 
                                             << 8U)) 
                             | ((0x00ffffffU & vlSelfRef.top__DOT__vgc_inst__DOT__fio_name[52U]) 
                                | (vlSelfRef.top__DOT__vgc_inst__DOT__fio_name[51U] 
                                   >> 8U)))));
    vlSelfRef.top__DOT__vgc_inst__DOT__fio_name_flat[13U] 
        = ((0x0000ffffU & vlSelfRef.top__DOT__vgc_inst__DOT__fio_name_flat[13U]) 
           | (0xffff0000U & ((vlSelfRef.top__DOT__vgc_inst__DOT__fio_name[55U] 
                              << 0x00000018U) | (vlSelfRef.top__DOT__vgc_inst__DOT__fio_name[54U] 
                                                 << 0x00000010U))));
    vlSelfRef.top__DOT__vgc_inst__DOT__fio_name_flat[14U] 
        = ((0xffffff00U & vlSelfRef.top__DOT__vgc_inst__DOT__fio_name_flat[14U]) 
           | (0x0000ffffU & ((0x0000ffffU & vlSelfRef.top__DOT__vgc_inst__DOT__fio_name[56U]) 
                             | ((0x0000ffffU & (vlSelfRef.top__DOT__vgc_inst__DOT__fio_name[55U] 
                                                >> 8U)) 
                                | (vlSelfRef.top__DOT__vgc_inst__DOT__fio_name[54U] 
                                   >> 0x00000010U)))));
    vlSelfRef.top__DOT__vgc_inst__DOT__fio_name_flat[14U] 
        = ((0x000000ffU & vlSelfRef.top__DOT__vgc_inst__DOT__fio_name_flat[14U]) 
           | (0xffffff00U & ((vlSelfRef.top__DOT__vgc_inst__DOT__fio_name[59U] 
                              << 0x00000018U) | ((vlSelfRef.top__DOT__vgc_inst__DOT__fio_name[58U] 
                                                  << 0x00000010U) 
                                                 | (vlSelfRef.top__DOT__vgc_inst__DOT__fio_name[57U] 
                                                    << 8U)))));
    vlSelfRef.top__DOT__vgc_inst__DOT__fio_name_flat[15U] 
        = ((0xff000000U & vlSelfRef.top__DOT__vgc_inst__DOT__fio_name_flat[15U]) 
           | ((vlSelfRef.top__DOT__vgc_inst__DOT__fio_name[62U] 
               << 0x00000010U) | ((vlSelfRef.top__DOT__vgc_inst__DOT__fio_name[61U] 
                                   << 8U) | vlSelfRef.top__DOT__vgc_inst__DOT__fio_name[60U])));
    vlSelfRef.top__DOT__vgc_inst__DOT__fio_name_flat[15U] 
        = ((0x00ffffffU & vlSelfRef.top__DOT__vgc_inst__DOT__fio_name_flat[15U]) 
           | (vlSelfRef.top__DOT__vgc_inst__DOT__fio_name[63U] 
              << 0x00000018U));
    vlSelfRef.top__DOT__vgc_inst__DOT__artist_inst__DOT__op 
        = __Vdly__top__DOT__vgc_inst__DOT__artist_inst__DOT__op;
    vlSelfRef.top__DOT__vgc_inst__DOT__artist_inst__DOT__fill_sp 
        = __Vdly__top__DOT__vgc_inst__DOT__artist_inst__DOT__fill_sp;
    vlSelfRef.top__DOT__vgc_inst__DOT__artist_inst__DOT__octant_count 
        = __Vdly__top__DOT__vgc_inst__DOT__artist_inst__DOT__octant_count;
    vlSelfRef.top__DOT__vgc_inst__DOT__artist_inst__DOT__paint_phase 
        = __Vdly__top__DOT__vgc_inst__DOT__artist_inst__DOT__paint_phase;
    vlSelfRef.top__DOT__vgc_inst__DOT__artist_inst__DOT__gt_font_pending 
        = __Vdly__top__DOT__vgc_inst__DOT__artist_inst__DOT__gt_font_pending;
    vlSelfRef.top__DOT__vgc_inst__DOT__artist_inst__DOT__paint_init 
        = __Vdly__top__DOT__vgc_inst__DOT__artist_inst__DOT__paint_init;
    vlSelfRef.top__DOT__vgc_inst__DOT__artist_inst__DOT__gt_font_byte 
        = __Vdly__top__DOT__vgc_inst__DOT__artist_inst__DOT__gt_font_byte;
    vlSelfRef.top__DOT__vgc_inst__DOT__artist_inst__DOT__err 
        = __Vdly__top__DOT__vgc_inst__DOT__artist_inst__DOT__err;
    vlSelfRef.top__DOT__vgc_inst__DOT__artist_inst__DOT__x 
        = __Vdly__top__DOT__vgc_inst__DOT__artist_inst__DOT__x;
    vlSelfRef.top__DOT__vgc_inst__DOT__artist_inst__DOT__y 
        = __Vdly__top__DOT__vgc_inst__DOT__artist_inst__DOT__y;
    vlSelfRef.top__DOT__vgc_inst__DOT__artist_inst__DOT__cx 
        = __Vdly__top__DOT__vgc_inst__DOT__artist_inst__DOT__cx;
    vlSelfRef.top__DOT__vgc_inst__DOT__artist_inst__DOT__cy 
        = __Vdly__top__DOT__vgc_inst__DOT__artist_inst__DOT__cy;
    vlSelfRef.top__DOT__vgc_inst__DOT__artist_inst__DOT__dx 
        = __Vdly__top__DOT__vgc_inst__DOT__artist_inst__DOT__dx;
    vlSelfRef.top__DOT__vgc_inst__DOT__artist_inst__DOT__dy 
        = __Vdly__top__DOT__vgc_inst__DOT__artist_inst__DOT__dy;
    vlSelfRef.top__DOT__vgc_inst__DOT__artist_inst__DOT__fill_target 
        = __Vdly__top__DOT__vgc_inst__DOT__artist_inst__DOT__fill_target;
    vlSelfRef.top__DOT__vgc_inst__DOT__artist_inst__DOT__gt_font_row 
        = __Vdly__top__DOT__vgc_inst__DOT__artist_inst__DOT__gt_font_row;
    vlSelfRef.top__DOT__vgc_inst__DOT__artist_inst__DOT__gt_char_idx 
        = __Vdly__top__DOT__vgc_inst__DOT__artist_inst__DOT__gt_char_idx;
    if (__VdlySet__top__DOT__vgc_inst__DOT__artist_inst__DOT__push_neighbors__v0) {
        vlSelfRef.top__DOT__vgc_inst__DOT__artist_inst__DOT__push_neighbors[0U] 
            = __VdlyVal__top__DOT__vgc_inst__DOT__artist_inst__DOT__push_neighbors__v0;
        vlSelfRef.top__DOT__vgc_inst__DOT__artist_inst__DOT__push_neighbors[1U] 
            = __VdlyVal__top__DOT__vgc_inst__DOT__artist_inst__DOT__push_neighbors__v1;
        vlSelfRef.top__DOT__vgc_inst__DOT__artist_inst__DOT__push_neighbors[2U] 
            = __VdlyVal__top__DOT__vgc_inst__DOT__artist_inst__DOT__push_neighbors__v2;
        vlSelfRef.top__DOT__vgc_inst__DOT__artist_inst__DOT__push_neighbors[3U] 
            = __VdlyVal__top__DOT__vgc_inst__DOT__artist_inst__DOT__push_neighbors__v3;
    }
    vlSelfRef.__VdfgRegularize_hebeb780c_0_25 = ((IData)(vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__s3_pixel_valid) 
                                                 & ((IData)(vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__s3_color_idx) 
                                                    != (IData)(vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__s3_trans_color)));
    if (__VdlySet__top__DOT__vgc_inst__DOT__tile_inst__DOT__tile_data_ram__DOT__mem__v0) {
        vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__tile_data_ram__DOT__mem[__VdlyDim0__top__DOT__vgc_inst__DOT__tile_inst__DOT__tile_data_ram__DOT__mem__v0] 
            = __VdlyVal__top__DOT__vgc_inst__DOT__tile_inst__DOT__tile_data_ram__DOT__mem__v0;
    }
    vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__dma_state 
        = __Vdly__top__DOT__vgc_inst__DOT__tile_inst__DOT__dma_state;
    if (__VdlySet__top__DOT__vgc_inst__DOT__tile_inst__DOT__nametable_ram__DOT__mem__v0) {
        vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__nametable_ram__DOT__mem[__VdlyDim0__top__DOT__vgc_inst__DOT__tile_inst__DOT__nametable_ram__DOT__mem__v0] 
            = __VdlyVal__top__DOT__vgc_inst__DOT__tile_inst__DOT__nametable_ram__DOT__mem__v0;
    }
    if (vlSelfRef.rst) {
        vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__trans_color = 0U;
        vlSelfRef.top__DOT__vgc_inst__DOT__dbg_rd_pending = 0U;
        vlSelfRef.top__DOT__vgc_inst__DOT__dbg_rd_source = 0U;
        vlSelfRef.top__DOT__vgc_inst__DOT__blt_rd_pending = 0U;
        vlSelfRef.top__DOT__vgc_inst__DOT__blt_rd_space = 0U;
    } else {
        if (((IData)(vlSelfRef.top__DOT__cpu_we) & (IData)(vlSelfRef.top__DOT__vgc_inst__DOT__tile_reg_sel))) {
            if ((0U != (0x0000001fU & (IData)(vlSelfRef.top__DOT__cpu_addr)))) {
                if ((1U == (0x0000001fU & (IData)(vlSelfRef.top__DOT__cpu_addr)))) {
                    vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__trans_color 
                        = (0x0000000fU & (IData)(vlSelfRef.top__DOT__cpu_dout));
                }
            }
        }
        vlSelfRef.top__DOT__vgc_inst__DOT__dbg_rd_pending = 0U;
        vlSelfRef.top__DOT__vgc_inst__DOT__dbg_rd_source = 0U;
        if (vlSelfRef.top__DOT__vgc_inst__DOT__dbg_char_sel) {
            vlSelfRef.top__DOT__vgc_inst__DOT__dbg_rd_pending = 1U;
            vlSelfRef.top__DOT__vgc_inst__DOT__dbg_rd_source = 1U;
        } else if (vlSelfRef.top__DOT__vgc_inst__DOT__dbg_color_sel) {
            vlSelfRef.top__DOT__vgc_inst__DOT__dbg_rd_pending = 1U;
            vlSelfRef.top__DOT__vgc_inst__DOT__dbg_rd_source = 2U;
        }
        vlSelfRef.top__DOT__vgc_inst__DOT__blt_rd_pending = 0U;
        if (vlSelfRef.top__DOT__blt_vgc_re) {
            vlSelfRef.top__DOT__vgc_inst__DOT__blt_rd_pending = 1U;
            vlSelfRef.top__DOT__vgc_inst__DOT__blt_rd_space 
                = vlSelfRef.top__DOT__blt_vgc_space;
        }
    }
    vlSelfRef.top__DOT__blt_vgc_re = 0U;
    vlSelfRef.top__DOT__blt_vgc_space = 0U;
    if (((2U == (IData)(vlSelfRef.top__DOT__blt_inst__DOT__state)) 
         | (4U == (IData)(vlSelfRef.top__DOT__blt_inst__DOT__state)))) {
        if ((1U & (~ (IData)(vlSelfRef.top__DOT__blt_inst__DOT__fill_mode)))) {
            if ((0U != (IData)(vlSelfRef.top__DOT__blt_inst__DOT__src_space))) {
                if ((5U != (IData)(vlSelfRef.top__DOT__blt_inst__DOT__src_space))) {
                    vlSelfRef.top__DOT__blt_vgc_re = 1U;
                    vlSelfRef.top__DOT__blt_vgc_space 
                        = vlSelfRef.top__DOT__blt_inst__DOT__src_space;
                }
            }
        }
    }
    if (((((3U == (IData)(vlSelfRef.top__DOT__blt_inst__DOT__state)) 
           & (IData)(vlSelfRef.top__DOT__blt_inst__DOT__read_valid)) 
          & (~ ((IData)(vlSelfRef.top__DOT__blt_inst__DOT__colorkey_mode) 
                & ((IData)(vlSelfRef.top__DOT__blt_inst__DOT__read_byte) 
                   == (IData)(vlSelfRef.top__DOT__blt_inst__DOT__color_key))))) 
         | ((5U == (IData)(vlSelfRef.top__DOT__blt_inst__DOT__state)) 
            & (~ ((IData)(vlSelfRef.top__DOT__blt_inst__DOT__colorkey_mode) 
                  & (vlSelfRef.top__DOT__blt_inst__DOT__row_buf
                     [vlSelfRef.top__DOT__blt_inst__DOT__buf_idx] 
                     == (IData)(vlSelfRef.top__DOT__blt_inst__DOT__color_key))))))) {
        if ((0U == (IData)(vlSelfRef.top__DOT__blt_inst__DOT__dst_space))) {
            vlSelfRef.top__DOT__blt_ram_addr = (0x0000ffffU 
                                                & top__DOT__blt_inst__DOT__dst_addr);
        }
        if ((0U != (IData)(vlSelfRef.top__DOT__blt_inst__DOT__dst_space))) {
            if ((5U == (IData)(vlSelfRef.top__DOT__blt_inst__DOT__dst_space))) {
                vlSelfRef.top__DOT__blt_xram_addr = 
                    (0x0007ffffU & top__DOT__blt_inst__DOT__dst_addr);
            }
            if ((5U != (IData)(vlSelfRef.top__DOT__blt_inst__DOT__dst_space))) {
                vlSelfRef.top__DOT__blt_vgc_addr = 
                    (0x0000ffffU & top__DOT__blt_inst__DOT__dst_addr);
                vlSelfRef.top__DOT__blt_vgc_space = vlSelfRef.top__DOT__blt_inst__DOT__dst_space;
            }
        }
    }
    if (__VdlySet__top__DOT__xmc_regs__v0) {
        vlSelfRef.top__DOT__xmc_regs[__VdlyDim0__top__DOT__xmc_regs__v0] 
            = __VdlyVal__top__DOT__xmc_regs__v0;
    }
    if (__VdlySet__top__DOT__xram__v0) {
        vlSelfRef.top__DOT__xram[__VdlyDim0__top__DOT__xram__v0] 
            = __VdlyVal__top__DOT__xram__v0;
    }
    if (__VdlySet__top__DOT__xram__v1) {
        vlSelfRef.top__DOT__xram[__VdlyDim0__top__DOT__xram__v1] 
            = __VdlyVal__top__DOT__xram__v1;
    }
    if (__VdlySet__top__DOT__ram__v0) {
        vlSelfRef.top__DOT__ram[__VdlyDim0__top__DOT__ram__v0] 
            = __VdlyVal__top__DOT__ram__v0;
    }
    if (__VdlySet__top__DOT__ram__v1) {
        vlSelfRef.top__DOT__ram[__VdlyDim0__top__DOT__ram__v1] 
            = __VdlyVal__top__DOT__ram__v1;
    }
    if (__VdlySet__top__DOT__ram__v2) {
        vlSelfRef.top__DOT__ram[40975U] = __VdlyVal__top__DOT__ram__v2;
    }
    if (__VdlySet__top__DOT__ram__v3) {
        vlSelfRef.top__DOT__ram[40975U] = 0U;
    }
    if (__VdlySet__top__DOT__ram__v4) {
        vlSelfRef.top__DOT__ram[__VdlyDim0__top__DOT__ram__v4] 
            = __VdlyVal__top__DOT__ram__v4;
    }
    vlSelfRef.top__DOT__vgc_inst__DOT__sprite_inst__DOT__spr_b_dout 
        = vlSelfRef.top__DOT__vgc_inst__DOT__sprite_inst__DOT__spr_mem__DOT__mem
        [vlSelfRef.top__DOT__vgc_inst__DOT__sprite_inst__DOT__spr_b_addr];
    vlSelfRef.top__DOT__vgc_inst__DOT__font_a_dout 
        = vlSelfRef.top__DOT__vgc_inst__DOT__text_inst__DOT__font_mem__DOT__mem
        [vlSelfRef.top__DOT__vgc_inst__DOT__font_a_addr];
    vlSelfRef.top__DOT__vgc_inst__DOT__artist_inst__DOT__fs_b_dout 
        = vlSelfRef.top__DOT__vgc_inst__DOT__artist_inst__DOT__fill_stack_mem__DOT__mem
        [vlSelfRef.top__DOT__vgc_inst__DOT__artist_inst__DOT__fs_b_addr];
    if ((((0x0cU == (IData)(vlSelfRef.top__DOT__cpu_inst__DOT__state)) 
          | (8U == (IData)(vlSelfRef.top__DOT__cpu_inst__DOT__state))) 
         & (IData)(vlSelfRef.top__DOT____Vcellinp__cpu_inst__RDY))) {
        vlSelfRef.top__DOT__cpu_inst__DOT__adc_bcd 
            = ((0x61U == (0xe3U & (IData)(vlSelfRef.top__DOT__cpu_inst__DOT__IR))) 
               && (IData)(vlSelfRef.top__DOT__cpu_inst__DOT__D));
    }
    if (vlSelfRef.top__DOT____Vcellinp__cpu_inst__RDY) {
        vlSelfRef.top__DOT__cpu_inst__DOT__HC = vlSelfRef.top__DOT__cpu_inst__DOT__ALU__DOT__temp_HC;
        vlSelfRef.top__DOT__cpu_inst__DOT__AN = (1U 
                                                 & ((IData)(vlSelfRef.top__DOT__cpu_inst__DOT__ALU__DOT__temp) 
                                                    >> 7U));
        vlSelfRef.top__DOT__cpu_inst__DOT__ALU__DOT__BI7 
            = (1U & ((IData)(vlSelfRef.top__DOT__cpu_inst__DOT__ALU__DOT__temp_BI) 
                     >> 7U));
        vlSelfRef.top__DOT__cpu_inst__DOT__ALU__DOT__AI7 
            = (1U & ((IData)(vlSelfRef.top__DOT__cpu_inst__DOT__AI) 
                     >> 7U));
    }
    if (((0x0cU == (IData)(vlSelfRef.top__DOT__cpu_inst__DOT__state)) 
         & (IData)(vlSelfRef.top__DOT____Vcellinp__cpu_inst__RDY))) {
        vlSelfRef.top__DOT__cpu_inst__DOT__sei = (0x78U 
                                                  == (IData)(vlSelfRef.top__DOT__cpu_inst__DOT__IR));
        vlSelfRef.top__DOT__cpu_inst__DOT__cli = (0x58U 
                                                  == (IData)(vlSelfRef.top__DOT__cpu_inst__DOT__IR));
        vlSelfRef.top__DOT__cpu_inst__DOT__load_reg 
            = (((((((((0x0aU == (0x9fU & (IData)(vlSelfRef.top__DOT__cpu_inst__DOT__IR))) 
                      || (1U == (0x83U & (IData)(vlSelfRef.top__DOT__cpu_inst__DOT__IR)))) 
                     || (0x88U == (0xedU & (IData)(vlSelfRef.top__DOT__cpu_inst__DOT__IR)))) 
                    || (0xa0U == (0xf1U & (IData)(vlSelfRef.top__DOT__cpu_inst__DOT__IR)))) 
                   || (0xbaU == (IData)(vlSelfRef.top__DOT__cpu_inst__DOT__IR))) 
                  || (0xb4U == (0xf5U & (IData)(vlSelfRef.top__DOT__cpu_inst__DOT__IR)))) 
                 || (0xcaU == (IData)(vlSelfRef.top__DOT__cpu_inst__DOT__IR))) 
                || (0xa1U == (0xa3U & (IData)(vlSelfRef.top__DOT__cpu_inst__DOT__IR)))) 
               || (8U == (0x1fU & (IData)(vlSelfRef.top__DOT__cpu_inst__DOT__IR))));
        vlSelfRef.top__DOT__cpu_inst__DOT__clv = (0xb8U 
                                                  == (IData)(vlSelfRef.top__DOT__cpu_inst__DOT__IR));
        vlSelfRef.top__DOT__cpu_inst__DOT__bit_ins 
            = (0x24U == (0xf7U & (IData)(vlSelfRef.top__DOT__cpu_inst__DOT__IR)));
        vlSelfRef.top__DOT__cpu_inst__DOT__sec = (0x38U 
                                                  == (IData)(vlSelfRef.top__DOT__cpu_inst__DOT__IR));
        vlSelfRef.top__DOT__cpu_inst__DOT__clc = (0x18U 
                                                  == (IData)(vlSelfRef.top__DOT__cpu_inst__DOT__IR));
        vlSelfRef.top__DOT__cpu_inst__DOT__shift = 
            ((6U == (0x87U & (IData)(vlSelfRef.top__DOT__cpu_inst__DOT__IR))) 
             || (0x0aU == (0x8fU & (IData)(vlSelfRef.top__DOT__cpu_inst__DOT__IR))));
        vlSelfRef.top__DOT__cpu_inst__DOT__compare 
            = (((0xc0U == (0xdbU & (IData)(vlSelfRef.top__DOT__cpu_inst__DOT__IR))) 
                || (0xccU == (0xdfU & (IData)(vlSelfRef.top__DOT__cpu_inst__DOT__IR)))) 
               || (0xc1U == (0xe3U & (IData)(vlSelfRef.top__DOT__cpu_inst__DOT__IR))));
    }
    if (vlSelfRef.rst) {
        __Vdly__top__DOT__vgc_inst__DOT__cursor_x = 0U;
        __Vdly__top__DOT__vgc_inst__DOT__cursor_y = 0U;
        vlSelfRef.top__DOT__vgc_inst__DOT__mode = 0U;
        __Vdly__top__DOT__vgc_inst__DOT__fg_color = 1U;
        vlSelfRef.top__DOT__vgc_inst__DOT__bg_color = 6U;
        vlSelfRef.top__DOT__vgc_inst__DOT__border_color = 6U;
        vlSelfRef.top__DOT__vgc_inst__DOT__gfx_color = 1U;
        vlSelfRef.top__DOT__vgc_inst__DOT__cursor_enable = 1U;
        __Vdly__top__DOT__vgc_inst__DOT__scroll_offset = 0U;
        __Vdly__top__DOT__vgc_inst__DOT__scroll_pending = 0U;
        __Vdly__top__DOT__vgc_inst__DOT__scroll_col = 0U;
        __Vdly__top__DOT__vgc_inst__DOT__cmd_busy = 0U;
        __Vdly__top__DOT__vgc_inst__DOT__copper_enabled = 0U;
        __Vdly__top__DOT__vgc_inst__DOT__copper_count = 0U;
        __Vdly__top__DOT__vgc_inst__DOT__copper_target_list = 0U;
        __Vdly__top__DOT__vgc_inst__DOT__copper_pending_list = 0U;
        __Vdly__top__DOT__vgc_inst__DOT__copper_loading = 0U;
        __Vdly__top__DOT__vgc_inst__DOT__sprrow_count = 0U;
        __Vdly__top__DOT__vgc_inst__DOT__sprcopy_phase = 0U;
        __Vdly__top__DOT__vgc_inst__DOT__memread_pending = 0U;
        vlSelfRef.top__DOT__vgc_inst__DOT__vgc_tile_we = 0U;
        vlSelfRef.top__DOT__vgc_inst__DOT__vgc_tile_re = 0U;
        vlSelfRef.top__DOT__vgc_inst__DOT__artist_cmd_valid = 0U;
        __VdlySet__top__DOT__vgc_inst__DOT__regs__v0 = 1U;
    } else {
        vlSelfRef.top__DOT__vgc_inst__DOT__cmd_char_we = 0U;
        vlSelfRef.top__DOT__vgc_inst__DOT__cmd_color_we = 0U;
        vlSelfRef.top__DOT__vgc_inst__DOT__cmd_gfx_we = 0U;
        vlSelfRef.top__DOT__vgc_inst__DOT__cmd_gfx_re = 0U;
        vlSelfRef.top__DOT__vgc_inst__DOT__cmd_spr_we = 0U;
        vlSelfRef.top__DOT__vgc_inst__DOT__cmd_spr_re = 0U;
        vlSelfRef.top__DOT__vgc_inst__DOT__artist_cmd_valid = 0U;
        if (vlSelfRef.top__DOT__vgc_inst__DOT__scroll_pending) {
            __Vfunc_top__DOT__vgc_inst__DOT__screen_addr__5__col 
                = vlSelfRef.top__DOT__vgc_inst__DOT__scroll_col;
            __Vfunc_top__DOT__vgc_inst__DOT__screen_addr__5__rr 
                = (0x0000001fU & ((IData)(0x18U) + (IData)(vlSelfRef.top__DOT__vgc_inst__DOT__scroll_offset)));
            if ((0x19U <= (IData)(__Vfunc_top__DOT__vgc_inst__DOT__screen_addr__5__rr))) {
                __Vfunc_top__DOT__vgc_inst__DOT__screen_addr__5__rr 
                    = (0x0000001fU & ((IData)(__Vfunc_top__DOT__vgc_inst__DOT__screen_addr__5__rr) 
                                      - (IData)(0x19U)));
            }
            vlSelfRef.top__DOT__vgc_inst__DOT__cmd_char_din = 0x20U;
            vlSelfRef.top__DOT__vgc_inst__DOT__cmd_char_we = 1U;
            vlSelfRef.top__DOT__vgc_inst__DOT__cmd_color_din 
                = vlSelfRef.top__DOT__vgc_inst__DOT__fg_color;
            vlSelfRef.top__DOT__vgc_inst__DOT__cmd_color_we = 1U;
            __Vfunc_top__DOT__vgc_inst__DOT__screen_addr__5__Vfuncout 
                = (0x000007ffU & (((IData)(0x00000050U) 
                                   * (IData)(__Vfunc_top__DOT__vgc_inst__DOT__screen_addr__5__rr)) 
                                  + (IData)(__Vfunc_top__DOT__vgc_inst__DOT__screen_addr__5__col)));
            vlSelfRef.top__DOT__vgc_inst__DOT__cmd_char_addr 
                = __Vfunc_top__DOT__vgc_inst__DOT__screen_addr__5__Vfuncout;
            __Vfunc_top__DOT__vgc_inst__DOT__screen_addr__6__col 
                = vlSelfRef.top__DOT__vgc_inst__DOT__scroll_col;
            __Vfunc_top__DOT__vgc_inst__DOT__screen_addr__6__rr 
                = (0x0000001fU & ((IData)(0x18U) + (IData)(vlSelfRef.top__DOT__vgc_inst__DOT__scroll_offset)));
            if ((0x19U <= (IData)(__Vfunc_top__DOT__vgc_inst__DOT__screen_addr__6__rr))) {
                __Vfunc_top__DOT__vgc_inst__DOT__screen_addr__6__rr 
                    = (0x0000001fU & ((IData)(__Vfunc_top__DOT__vgc_inst__DOT__screen_addr__6__rr) 
                                      - (IData)(0x19U)));
            }
            __Vfunc_top__DOT__vgc_inst__DOT__screen_addr__6__Vfuncout 
                = (0x000007ffU & (((IData)(0x00000050U) 
                                   * (IData)(__Vfunc_top__DOT__vgc_inst__DOT__screen_addr__6__rr)) 
                                  + (IData)(__Vfunc_top__DOT__vgc_inst__DOT__screen_addr__6__col)));
            vlSelfRef.top__DOT__vgc_inst__DOT__cmd_color_addr 
                = __Vfunc_top__DOT__vgc_inst__DOT__screen_addr__6__Vfuncout;
            if ((0x4fU == (IData)(vlSelfRef.top__DOT__vgc_inst__DOT__scroll_col))) {
                __Vdly__top__DOT__vgc_inst__DOT__scroll_pending = 0U;
                __Vdly__top__DOT__vgc_inst__DOT__scroll_col = 0U;
            } else {
                __Vdly__top__DOT__vgc_inst__DOT__scroll_col 
                    = (0x0000007fU & ((IData)(1U) + (IData)(vlSelfRef.top__DOT__vgc_inst__DOT__scroll_col)));
            }
        }
        vlSelfRef.top__DOT__vgc_inst__DOT__vgc_tile_we = 0U;
        vlSelfRef.top__DOT__vgc_inst__DOT__vgc_tile_re = 0U;
        if (vlSelfRef.top__DOT__vgc_inst__DOT__memread_pending) {
            if ((4U & (IData)(vlSelfRef.top__DOT__vgc_inst__DOT__memread_space))) {
                if ((1U & (~ ((IData)(vlSelfRef.top__DOT__vgc_inst__DOT__memread_space) 
                              >> 1U)))) {
                    if ((1U & (~ (IData)(vlSelfRef.top__DOT__vgc_inst__DOT__memread_space)))) {
                        __VdlyVal__top__DOT__vgc_inst__DOT__regs__v32 
                            = vlSelfRef.top__DOT__vgc_inst__DOT__tile_blt_rdata;
                        __VdlySet__top__DOT__vgc_inst__DOT__regs__v32 = 1U;
                    }
                }
            } else {
                __VdlyVal__top__DOT__vgc_inst__DOT__regs__v33 
                    = ((2U & (IData)(vlSelfRef.top__DOT__vgc_inst__DOT__memread_space))
                        ? ((1U & (IData)(vlSelfRef.top__DOT__vgc_inst__DOT__memread_space))
                            ? (IData)(vlSelfRef.top__DOT__vgc_inst__DOT__spr_a_dout)
                            : (IData)(vlSelfRef.top__DOT__vgc_inst__DOT__gfx_a_dout))
                        : ((1U & (IData)(vlSelfRef.top__DOT__vgc_inst__DOT__memread_space))
                            ? (IData)(vlSelfRef.top__DOT__vgc_inst__DOT__color_a_dout)
                            : (IData)(vlSelfRef.top__DOT__vgc_inst__DOT__char_a_dout)));
                __VdlySet__top__DOT__vgc_inst__DOT__regs__v33 = 1U;
            }
            __Vdly__top__DOT__vgc_inst__DOT__memread_pending = 0U;
        }
        if (vlSelfRef.top__DOT__vgc_inst__DOT__cmd_busy) {
            if ((0x12U == (IData)(vlSelfRef.top__DOT__vgc_inst__DOT__cmd_op))) {
                vlSelfRef.top__DOT__vgc_inst__DOT__cmd_spr_addr 
                    = ((0x00000780U & ((IData)(vlSelfRef.top__DOT__vgc_inst__DOT__cmd_x) 
                                       << 6U)) | ((0x00000078U 
                                                   & ((IData)(vlSelfRef.top__DOT__vgc_inst__DOT__cmd_cy) 
                                                      << 3U)) 
                                                  | (7U 
                                                     & (IData)(vlSelfRef.top__DOT__vgc_inst__DOT__cmd_cx))));
                vlSelfRef.top__DOT__vgc_inst__DOT__cmd_spr_din = 0U;
                vlSelfRef.top__DOT__vgc_inst__DOT__cmd_spr_we = 1U;
                if ((7U == (IData)(vlSelfRef.top__DOT__vgc_inst__DOT__cmd_cx))) {
                    __Vdly__top__DOT__vgc_inst__DOT__cmd_cx = 0U;
                    if ((0x0fU == (IData)(vlSelfRef.top__DOT__vgc_inst__DOT__cmd_cy))) {
                        __Vdly__top__DOT__vgc_inst__DOT__cmd_busy = 0U;
                    } else {
                        __Vdly__top__DOT__vgc_inst__DOT__cmd_cy 
                            = (0x000000ffU & ((IData)(1U) 
                                              + (IData)(vlSelfRef.top__DOT__vgc_inst__DOT__cmd_cy)));
                    }
                } else {
                    __Vdly__top__DOT__vgc_inst__DOT__cmd_cx 
                        = (0x000001ffU & ((IData)(1U) 
                                          + (IData)(vlSelfRef.top__DOT__vgc_inst__DOT__cmd_cx)));
                }
            } else if ((0xf0U == (IData)(vlSelfRef.top__DOT__vgc_inst__DOT__cmd_op))) {
                vlSelfRef.top__DOT__vgc_inst__DOT__cmd_char_addr 
                    = (0x000007ffU & (((IData)(0x00000050U) 
                                       * (0x0000001fU 
                                          & (IData)(vlSelfRef.top__DOT__vgc_inst__DOT__cmd_cy))) 
                                      + (0x0000007fU 
                                         & (IData)(vlSelfRef.top__DOT__vgc_inst__DOT__cmd_cx))));
                vlSelfRef.top__DOT__vgc_inst__DOT__cmd_char_din = 0x20U;
                vlSelfRef.top__DOT__vgc_inst__DOT__cmd_char_we = 1U;
                vlSelfRef.top__DOT__vgc_inst__DOT__cmd_color_din 
                    = vlSelfRef.top__DOT__vgc_inst__DOT__fg_color;
                vlSelfRef.top__DOT__vgc_inst__DOT__cmd_color_we = 1U;
                vlSelfRef.top__DOT__vgc_inst__DOT__cmd_color_addr 
                    = (0x000007ffU & (((IData)(0x00000050U) 
                                       * (0x0000001fU 
                                          & (IData)(vlSelfRef.top__DOT__vgc_inst__DOT__cmd_cy))) 
                                      + (0x0000007fU 
                                         & (IData)(vlSelfRef.top__DOT__vgc_inst__DOT__cmd_cx))));
                if ((0x004fU == (IData)(vlSelfRef.top__DOT__vgc_inst__DOT__cmd_cx))) {
                    __Vdly__top__DOT__vgc_inst__DOT__cmd_cx = 0U;
                    if ((0x18U == (IData)(vlSelfRef.top__DOT__vgc_inst__DOT__cmd_cy))) {
                        __Vdly__top__DOT__vgc_inst__DOT__cmd_busy = 0U;
                    } else {
                        __Vdly__top__DOT__vgc_inst__DOT__cmd_cy 
                            = (0x000000ffU & ((IData)(1U) 
                                              + (IData)(vlSelfRef.top__DOT__vgc_inst__DOT__cmd_cy)));
                    }
                } else {
                    __Vdly__top__DOT__vgc_inst__DOT__cmd_cx 
                        = (0x000001ffU & ((IData)(1U) 
                                          + (IData)(vlSelfRef.top__DOT__vgc_inst__DOT__cmd_cx)));
                }
            } else if ((0x13U == (IData)(vlSelfRef.top__DOT__vgc_inst__DOT__cmd_op))) {
                if (vlSelfRef.top__DOT__vgc_inst__DOT__sprcopy_phase) {
                    vlSelfRef.top__DOT__vgc_inst__DOT__cmd_spr_addr 
                        = ((0x00000780U & ((IData)(vlSelfRef.top__DOT__vgc_inst__DOT__cmd_x) 
                                           << 7U)) 
                           | ((0x00000078U & ((IData)(vlSelfRef.top__DOT__vgc_inst__DOT__cmd_cy) 
                                              << 3U)) 
                              | (7U & (IData)(vlSelfRef.top__DOT__vgc_inst__DOT__cmd_cx))));
                    vlSelfRef.top__DOT__vgc_inst__DOT__cmd_spr_din 
                        = vlSelfRef.top__DOT__vgc_inst__DOT__spr_a_dout;
                    vlSelfRef.top__DOT__vgc_inst__DOT__cmd_spr_we = 1U;
                    __Vdly__top__DOT__vgc_inst__DOT__sprcopy_phase = 0U;
                    if ((7U == (IData)(vlSelfRef.top__DOT__vgc_inst__DOT__cmd_cx))) {
                        __Vdly__top__DOT__vgc_inst__DOT__cmd_cx = 0U;
                        if ((0x0fU == (IData)(vlSelfRef.top__DOT__vgc_inst__DOT__cmd_cy))) {
                            __Vdly__top__DOT__vgc_inst__DOT__cmd_busy = 0U;
                        } else {
                            __Vdly__top__DOT__vgc_inst__DOT__cmd_cy 
                                = (0x000000ffU & ((IData)(1U) 
                                                  + (IData)(vlSelfRef.top__DOT__vgc_inst__DOT__cmd_cy)));
                        }
                    } else {
                        __Vdly__top__DOT__vgc_inst__DOT__cmd_cx 
                            = (0x000001ffU & ((IData)(1U) 
                                              + (IData)(vlSelfRef.top__DOT__vgc_inst__DOT__cmd_cx)));
                    }
                } else {
                    vlSelfRef.top__DOT__vgc_inst__DOT__cmd_spr_addr 
                        = ((0x00000780U & ((IData)(vlSelfRef.top__DOT__vgc_inst__DOT__cmd_y) 
                                           << 7U)) 
                           | ((0x00000078U & ((IData)(vlSelfRef.top__DOT__vgc_inst__DOT__cmd_cy) 
                                              << 3U)) 
                              | (7U & (IData)(vlSelfRef.top__DOT__vgc_inst__DOT__cmd_cx))));
                    vlSelfRef.top__DOT__vgc_inst__DOT__cmd_spr_re = 1U;
                    __Vdly__top__DOT__vgc_inst__DOT__sprcopy_phase = 1U;
                }
            } else if ((0x11U == (IData)(vlSelfRef.top__DOT__vgc_inst__DOT__cmd_op))) {
                vlSelfRef.top__DOT__vgc_inst__DOT__cmd_spr_addr 
                    = (((IData)(vlSelfRef.top__DOT__vgc_inst__DOT__sprrow_spr) 
                        << 7U) | (((IData)(vlSelfRef.top__DOT__vgc_inst__DOT__sprrow_row) 
                                   << 3U) | (IData)(vlSelfRef.top__DOT__vgc_inst__DOT__sprrow_count)));
                vlSelfRef.top__DOT__vgc_inst__DOT__cmd_spr_we = 1U;
                vlSelfRef.top__DOT__vgc_inst__DOT__cmd_spr_din 
                    = vlSelfRef.top__DOT__vgc_inst__DOT__sprrow_data
                    [vlSelfRef.top__DOT__vgc_inst__DOT__sprrow_count];
                if ((7U == (IData)(vlSelfRef.top__DOT__vgc_inst__DOT__sprrow_count))) {
                    __Vdly__top__DOT__vgc_inst__DOT__cmd_busy = 0U;
                } else {
                    __Vdly__top__DOT__vgc_inst__DOT__sprrow_count 
                        = (7U & ((IData)(1U) + (IData)(vlSelfRef.top__DOT__vgc_inst__DOT__sprrow_count)));
                }
            } else {
                __Vdly__top__DOT__vgc_inst__DOT__cmd_busy = 0U;
            }
        }
        if (vlSelfRef.top__DOT__vgc_inst__DOT__copper_loading) {
            if (((IData)(vlSelfRef.top__DOT__vgc_inst__DOT__copper_load_idx) 
                 < vlSelfRef.top__DOT__vgc_inst__DOT__copper_list_count
                 [vlSelfRef.top__DOT__vgc_inst__DOT__copper_load_src])) {
                __VdlyVal__top__DOT__vgc_inst__DOT__copper_pos__v0 
                    = vlSelfRef.top__DOT__vgc_inst__DOT__copper_list_pos
                    [(((IData)(vlSelfRef.top__DOT__vgc_inst__DOT__copper_load_src) 
                       << 5U) | (0x0000001fU & (IData)(vlSelfRef.top__DOT__vgc_inst__DOT__copper_load_idx)))];
                __VdlyDim0__top__DOT__vgc_inst__DOT__copper_pos__v0 
                    = (0x0000001fU & (IData)(vlSelfRef.top__DOT__vgc_inst__DOT__copper_load_idx));
                __VdlySet__top__DOT__vgc_inst__DOT__copper_pos__v0 = 1U;
                __VdlyVal__top__DOT__vgc_inst__DOT__copper_reg__v0 
                    = vlSelfRef.top__DOT__vgc_inst__DOT__copper_list_reg
                    [(((IData)(vlSelfRef.top__DOT__vgc_inst__DOT__copper_load_src) 
                       << 5U) | (0x0000001fU & (IData)(vlSelfRef.top__DOT__vgc_inst__DOT__copper_load_idx)))];
                __VdlyDim0__top__DOT__vgc_inst__DOT__copper_reg__v0 
                    = (0x0000001fU & (IData)(vlSelfRef.top__DOT__vgc_inst__DOT__copper_load_idx));
                __VdlyVal__top__DOT__vgc_inst__DOT__copper_val__v0 
                    = vlSelfRef.top__DOT__vgc_inst__DOT__copper_list_val
                    [(((IData)(vlSelfRef.top__DOT__vgc_inst__DOT__copper_load_src) 
                       << 5U) | (0x0000001fU & (IData)(vlSelfRef.top__DOT__vgc_inst__DOT__copper_load_idx)))];
                __VdlyDim0__top__DOT__vgc_inst__DOT__copper_val__v0 
                    = (0x0000001fU & (IData)(vlSelfRef.top__DOT__vgc_inst__DOT__copper_load_idx));
                vlSelfRef.top__DOT__vgc_inst__DOT__copper_load_idx 
                    = (0x000001ffU & ((IData)(1U) + (IData)(vlSelfRef.top__DOT__vgc_inst__DOT__copper_load_idx)));
            } else {
                __Vdly__top__DOT__vgc_inst__DOT__copper_count 
                    = vlSelfRef.top__DOT__vgc_inst__DOT__copper_list_count
                    [vlSelfRef.top__DOT__vgc_inst__DOT__copper_load_src];
                __Vdly__top__DOT__vgc_inst__DOT__copper_loading = 0U;
            }
        }
        if (((0U == (IData)(vlSelfRef.top__DOT__vgc_inst__DOT__h_count)) 
             & (0x01e0U == (IData)(vlSelfRef.top__DOT__vgc_inst__DOT__v_count)))) {
            if ((((IData)(vlSelfRef.top__DOT__vgc_inst__DOT__copper_pending_list) 
                  != (IData)(vlSelfRef.top__DOT__vgc_inst__DOT__copper_active_list)) 
                 & (~ (IData)(vlSelfRef.top__DOT__vgc_inst__DOT__copper_loading)))) {
                vlSelfRef.top__DOT__vgc_inst__DOT__copper_load_idx = 0U;
                __Vdly__top__DOT__vgc_inst__DOT__copper_active_list 
                    = vlSelfRef.top__DOT__vgc_inst__DOT__copper_pending_list;
                __Vdly__top__DOT__vgc_inst__DOT__copper_loading = 1U;
                vlSelfRef.top__DOT__vgc_inst__DOT__copper_load_src 
                    = vlSelfRef.top__DOT__vgc_inst__DOT__copper_pending_list;
            }
        }
        if (vlSelfRef.top__DOT__vgc_inst__DOT__copper_fire) {
            if ((1U == (IData)(vlSelfRef.top__DOT__vgc_inst__DOT__copper_fire_reg))) {
                vlSelfRef.top__DOT__vgc_inst__DOT__bg_color 
                    = (0x0000000fU & (IData)(vlSelfRef.top__DOT__vgc_inst__DOT__copper_fire_val));
            } else if ((2U == (IData)(vlSelfRef.top__DOT__vgc_inst__DOT__copper_fire_reg))) {
                __Vdly__top__DOT__vgc_inst__DOT__fg_color 
                    = (0x0000000fU & (IData)(vlSelfRef.top__DOT__vgc_inst__DOT__copper_fire_val));
            } else if ((0x0dU == (IData)(vlSelfRef.top__DOT__vgc_inst__DOT__copper_fire_reg))) {
                vlSelfRef.top__DOT__vgc_inst__DOT__border_color 
                    = (0x0000000fU & (IData)(vlSelfRef.top__DOT__vgc_inst__DOT__copper_fire_val));
            } else {
                __VdlyVal__top__DOT__vgc_inst__DOT__regs__v34 
                    = vlSelfRef.top__DOT__vgc_inst__DOT__copper_fire_val;
                __VdlyDim0__top__DOT__vgc_inst__DOT__regs__v34 
                    = (0x0000001fU & (IData)(vlSelfRef.top__DOT__vgc_inst__DOT__copper_fire_reg));
                __VdlySet__top__DOT__vgc_inst__DOT__regs__v34 = 1U;
            }
        }
        if (vlSelfRef.top__DOT__cpu_we) {
            if (vlSelfRef.top__DOT__vgc_inst__DOT__vgc_reg_sel) {
                if (((((((((0U == (0x0000001fU & (IData)(vlSelfRef.top__DOT__cpu_addr))) 
                           | (1U == (0x0000001fU & (IData)(vlSelfRef.top__DOT__cpu_addr)))) 
                          | (2U == (0x0000001fU & (IData)(vlSelfRef.top__DOT__cpu_addr)))) 
                         | (3U == (0x0000001fU & (IData)(vlSelfRef.top__DOT__cpu_addr)))) 
                        | (4U == (0x0000001fU & (IData)(vlSelfRef.top__DOT__cpu_addr)))) 
                       | (5U == (0x0000001fU & (IData)(vlSelfRef.top__DOT__cpu_addr)))) 
                      | (6U == (0x0000001fU & (IData)(vlSelfRef.top__DOT__cpu_addr)))) 
                     | (7U == (0x0000001fU & (IData)(vlSelfRef.top__DOT__cpu_addr))))) {
                    if ((0U == (0x0000001fU & (IData)(vlSelfRef.top__DOT__cpu_addr)))) {
                        vlSelfRef.top__DOT__vgc_inst__DOT__mode 
                            = (7U & (IData)(vlSelfRef.top__DOT__cpu_dout));
                    } else if ((1U == (0x0000001fU 
                                       & (IData)(vlSelfRef.top__DOT__cpu_addr)))) {
                        vlSelfRef.top__DOT__vgc_inst__DOT__bg_color 
                            = (0x0000000fU & (IData)(vlSelfRef.top__DOT__cpu_dout));
                    } else if ((2U == (0x0000001fU 
                                       & (IData)(vlSelfRef.top__DOT__cpu_addr)))) {
                        __Vdly__top__DOT__vgc_inst__DOT__fg_color 
                            = (0x0000000fU & (IData)(vlSelfRef.top__DOT__cpu_dout));
                    } else if ((3U == (0x0000001fU 
                                       & (IData)(vlSelfRef.top__DOT__cpu_addr)))) {
                        __Vdly__top__DOT__vgc_inst__DOT__cursor_x 
                            = (0x0000007fU & (IData)(vlSelfRef.top__DOT__cpu_dout));
                    } else if ((4U == (0x0000001fU 
                                       & (IData)(vlSelfRef.top__DOT__cpu_addr)))) {
                        __Vdly__top__DOT__vgc_inst__DOT__cursor_y 
                            = (0x0000001fU & (IData)(vlSelfRef.top__DOT__cpu_dout));
                    }
                } else if (((((((((8U == (0x0000001fU 
                                          & (IData)(vlSelfRef.top__DOT__cpu_addr))) 
                                  | (0x0aU == (0x0000001fU 
                                               & (IData)(vlSelfRef.top__DOT__cpu_addr)))) 
                                 | (0x0bU == (0x0000001fU 
                                              & (IData)(vlSelfRef.top__DOT__cpu_addr)))) 
                                | (0x0cU == (0x0000001fU 
                                             & (IData)(vlSelfRef.top__DOT__cpu_addr)))) 
                               | (0x0dU == (0x0000001fU 
                                            & (IData)(vlSelfRef.top__DOT__cpu_addr)))) 
                              | (0x1fU == (0x0000001fU 
                                           & (IData)(vlSelfRef.top__DOT__cpu_addr)))) 
                             | (0x0eU == (0x0000001fU 
                                          & (IData)(vlSelfRef.top__DOT__cpu_addr)))) 
                            | (0x10U == (0x0000001fU 
                                         & (IData)(vlSelfRef.top__DOT__cpu_addr))))) {
                    if ((8U == (0x0000001fU & (IData)(vlSelfRef.top__DOT__cpu_addr)))) {
                        vlSelfRef.top__DOT__vgc_inst__DOT__gfx_color 
                            = (0x0000000fU & (IData)(vlSelfRef.top__DOT__cpu_dout));
                    } else if ((0x0aU == (0x0000001fU 
                                          & (IData)(vlSelfRef.top__DOT__cpu_addr)))) {
                        vlSelfRef.top__DOT__vgc_inst__DOT__cursor_enable 
                            = (1U & (IData)(vlSelfRef.top__DOT__cpu_dout));
                    } else if ((0x0bU != (0x0000001fU 
                                          & (IData)(vlSelfRef.top__DOT__cpu_addr)))) {
                        if ((0x0cU != (0x0000001fU 
                                       & (IData)(vlSelfRef.top__DOT__cpu_addr)))) {
                            if ((0x0dU == (0x0000001fU 
                                           & (IData)(vlSelfRef.top__DOT__cpu_addr)))) {
                                vlSelfRef.top__DOT__vgc_inst__DOT__border_color 
                                    = (0x0000000fU 
                                       & (IData)(vlSelfRef.top__DOT__cpu_dout));
                            } else if ((0x1fU != (0x0000001fU 
                                                  & (IData)(vlSelfRef.top__DOT__cpu_addr)))) {
                                if ((0x0eU == (0x0000001fU 
                                               & (IData)(vlSelfRef.top__DOT__cpu_addr)))) {
                                    if ((8U == (IData)(vlSelfRef.top__DOT__cpu_dout))) {
                                        if ((0U < (IData)(vlSelfRef.top__DOT__vgc_inst__DOT__cursor_x))) {
                                            __Vdly__top__DOT__vgc_inst__DOT__cursor_x 
                                                = (0x0000007fU 
                                                   & ((IData)(vlSelfRef.top__DOT__vgc_inst__DOT__cursor_x) 
                                                      - (IData)(1U)));
                                            __Vfunc_top__DOT__vgc_inst__DOT__screen_addr__7__col 
                                                = (0x0000007fU 
                                                   & ((IData)(vlSelfRef.top__DOT__vgc_inst__DOT__cursor_x) 
                                                      - (IData)(1U)));
                                        } else {
                                            __Vfunc_top__DOT__vgc_inst__DOT__screen_addr__7__col = 0U;
                                        }
                                        vlSelfRef.top__DOT__vgc_inst__DOT__cmd_char_din = 0x20U;
                                        vlSelfRef.top__DOT__vgc_inst__DOT__cmd_char_we = 1U;
                                        __Vfunc_top__DOT__vgc_inst__DOT__screen_addr__7__row 
                                            = vlSelfRef.top__DOT__vgc_inst__DOT__cursor_y;
                                        __Vfunc_top__DOT__vgc_inst__DOT__screen_addr__7__rr 
                                            = (0x0000001fU 
                                               & ((IData)(__Vfunc_top__DOT__vgc_inst__DOT__screen_addr__7__row) 
                                                  + (IData)(vlSelfRef.top__DOT__vgc_inst__DOT__scroll_offset)));
                                        if ((0x19U 
                                             <= (IData)(__Vfunc_top__DOT__vgc_inst__DOT__screen_addr__7__rr))) {
                                            __Vfunc_top__DOT__vgc_inst__DOT__screen_addr__7__rr 
                                                = (0x0000001fU 
                                                   & ((IData)(__Vfunc_top__DOT__vgc_inst__DOT__screen_addr__7__rr) 
                                                      - (IData)(0x19U)));
                                        }
                                        __Vfunc_top__DOT__vgc_inst__DOT__screen_addr__7__Vfuncout 
                                            = (0x000007ffU 
                                               & (((IData)(0x00000050U) 
                                                   * (IData)(__Vfunc_top__DOT__vgc_inst__DOT__screen_addr__7__rr)) 
                                                  + (IData)(__Vfunc_top__DOT__vgc_inst__DOT__screen_addr__7__col)));
                                        vlSelfRef.top__DOT__vgc_inst__DOT__cmd_char_addr 
                                            = __Vfunc_top__DOT__vgc_inst__DOT__screen_addr__7__Vfuncout;
                                    } else if ((0x0aU 
                                                == (IData)(vlSelfRef.top__DOT__cpu_dout))) {
                                        if ((0x18U 
                                             <= (IData)(vlSelfRef.top__DOT__vgc_inst__DOT__cursor_y))) {
                                            __Vdly__top__DOT__vgc_inst__DOT__scroll_offset 
                                                = (
                                                   (0x18U 
                                                    <= (IData)(vlSelfRef.top__DOT__vgc_inst__DOT__scroll_offset))
                                                    ? 0U
                                                    : 
                                                   (0x0000001fU 
                                                    & ((IData)(1U) 
                                                       + (IData)(vlSelfRef.top__DOT__vgc_inst__DOT__scroll_offset))));
                                            __Vdly__top__DOT__vgc_inst__DOT__scroll_pending = 1U;
                                            __Vdly__top__DOT__vgc_inst__DOT__scroll_col = 0U;
                                        } else {
                                            __Vdly__top__DOT__vgc_inst__DOT__cursor_y 
                                                = (0x0000001fU 
                                                   & ((IData)(1U) 
                                                      + (IData)(vlSelfRef.top__DOT__vgc_inst__DOT__cursor_y)));
                                        }
                                    } else if ((0x0cU 
                                                == (IData)(vlSelfRef.top__DOT__cpu_dout))) {
                                        __Vdly__top__DOT__vgc_inst__DOT__cmd_cx = 0U;
                                        __Vdly__top__DOT__vgc_inst__DOT__cmd_cy = 0U;
                                        __Vdly__top__DOT__vgc_inst__DOT__cursor_x = 0U;
                                        __Vdly__top__DOT__vgc_inst__DOT__cursor_y = 0U;
                                        __Vdly__top__DOT__vgc_inst__DOT__scroll_offset = 0U;
                                        __Vdly__top__DOT__vgc_inst__DOT__cmd_busy = 1U;
                                        __Vdly__top__DOT__vgc_inst__DOT__cmd_op = 0xf0U;
                                    } else if ((0x0dU 
                                                == (IData)(vlSelfRef.top__DOT__cpu_dout))) {
                                        __Vdly__top__DOT__vgc_inst__DOT__cursor_x = 0U;
                                    } else if ((0x13U 
                                                == (IData)(vlSelfRef.top__DOT__cpu_dout))) {
                                        __Vdly__top__DOT__vgc_inst__DOT__cursor_x = 0U;
                                        __Vdly__top__DOT__vgc_inst__DOT__cursor_y = 0U;
                                    } else if ((0x20U 
                                                <= (IData)(vlSelfRef.top__DOT__cpu_dout))) {
                                        __Vfunc_top__DOT__vgc_inst__DOT__screen_addr__8__row 
                                            = vlSelfRef.top__DOT__vgc_inst__DOT__cursor_y;
                                        __Vfunc_top__DOT__vgc_inst__DOT__screen_addr__8__col 
                                            = vlSelfRef.top__DOT__vgc_inst__DOT__cursor_x;
                                        __Vfunc_top__DOT__vgc_inst__DOT__screen_addr__8__rr 
                                            = (0x0000001fU 
                                               & ((IData)(__Vfunc_top__DOT__vgc_inst__DOT__screen_addr__8__row) 
                                                  + (IData)(vlSelfRef.top__DOT__vgc_inst__DOT__scroll_offset)));
                                        vlSelfRef.top__DOT__vgc_inst__DOT__cmd_char_din 
                                            = vlSelfRef.top__DOT__cpu_dout;
                                        vlSelfRef.top__DOT__vgc_inst__DOT__cmd_char_we = 1U;
                                        vlSelfRef.top__DOT__vgc_inst__DOT__cmd_color_din 
                                            = vlSelfRef.top__DOT__vgc_inst__DOT__fg_color;
                                        vlSelfRef.top__DOT__vgc_inst__DOT__cmd_color_we = 1U;
                                        if ((0x19U 
                                             <= (IData)(__Vfunc_top__DOT__vgc_inst__DOT__screen_addr__8__rr))) {
                                            __Vfunc_top__DOT__vgc_inst__DOT__screen_addr__8__rr 
                                                = (0x0000001fU 
                                                   & ((IData)(__Vfunc_top__DOT__vgc_inst__DOT__screen_addr__8__rr) 
                                                      - (IData)(0x19U)));
                                        }
                                        __Vfunc_top__DOT__vgc_inst__DOT__screen_addr__8__Vfuncout 
                                            = (0x000007ffU 
                                               & (((IData)(0x00000050U) 
                                                   * (IData)(__Vfunc_top__DOT__vgc_inst__DOT__screen_addr__8__rr)) 
                                                  + (IData)(__Vfunc_top__DOT__vgc_inst__DOT__screen_addr__8__col)));
                                        vlSelfRef.top__DOT__vgc_inst__DOT__cmd_char_addr 
                                            = __Vfunc_top__DOT__vgc_inst__DOT__screen_addr__8__Vfuncout;
                                        __Vfunc_top__DOT__vgc_inst__DOT__screen_addr__9__row 
                                            = vlSelfRef.top__DOT__vgc_inst__DOT__cursor_y;
                                        __Vfunc_top__DOT__vgc_inst__DOT__screen_addr__9__col 
                                            = vlSelfRef.top__DOT__vgc_inst__DOT__cursor_x;
                                        __Vfunc_top__DOT__vgc_inst__DOT__screen_addr__9__rr 
                                            = (0x0000001fU 
                                               & ((IData)(__Vfunc_top__DOT__vgc_inst__DOT__screen_addr__9__row) 
                                                  + (IData)(vlSelfRef.top__DOT__vgc_inst__DOT__scroll_offset)));
                                        if ((0x19U 
                                             <= (IData)(__Vfunc_top__DOT__vgc_inst__DOT__screen_addr__9__rr))) {
                                            __Vfunc_top__DOT__vgc_inst__DOT__screen_addr__9__rr 
                                                = (0x0000001fU 
                                                   & ((IData)(__Vfunc_top__DOT__vgc_inst__DOT__screen_addr__9__rr) 
                                                      - (IData)(0x19U)));
                                        }
                                        __Vfunc_top__DOT__vgc_inst__DOT__screen_addr__9__Vfuncout 
                                            = (0x000007ffU 
                                               & (((IData)(0x00000050U) 
                                                   * (IData)(__Vfunc_top__DOT__vgc_inst__DOT__screen_addr__9__rr)) 
                                                  + (IData)(__Vfunc_top__DOT__vgc_inst__DOT__screen_addr__9__col)));
                                        vlSelfRef.top__DOT__vgc_inst__DOT__cmd_color_addr 
                                            = __Vfunc_top__DOT__vgc_inst__DOT__screen_addr__9__Vfuncout;
                                        if ((0x4fU 
                                             <= (IData)(vlSelfRef.top__DOT__vgc_inst__DOT__cursor_x))) {
                                            __Vdly__top__DOT__vgc_inst__DOT__cursor_x = 0U;
                                            if ((0x18U 
                                                 <= (IData)(vlSelfRef.top__DOT__vgc_inst__DOT__cursor_y))) {
                                                __Vdly__top__DOT__vgc_inst__DOT__scroll_offset 
                                                    = 
                                                    ((0x18U 
                                                      <= (IData)(vlSelfRef.top__DOT__vgc_inst__DOT__scroll_offset))
                                                      ? 0U
                                                      : 
                                                     (0x0000001fU 
                                                      & ((IData)(1U) 
                                                         + (IData)(vlSelfRef.top__DOT__vgc_inst__DOT__scroll_offset))));
                                                __Vdly__top__DOT__vgc_inst__DOT__scroll_pending = 1U;
                                                __Vdly__top__DOT__vgc_inst__DOT__scroll_col = 0U;
                                            } else {
                                                __Vdly__top__DOT__vgc_inst__DOT__cursor_y 
                                                    = 
                                                    (0x0000001fU 
                                                     & ((IData)(1U) 
                                                        + (IData)(vlSelfRef.top__DOT__vgc_inst__DOT__cursor_y)));
                                            }
                                        } else {
                                            __Vdly__top__DOT__vgc_inst__DOT__cursor_x 
                                                = (0x0000007fU 
                                                   & ((IData)(1U) 
                                                      + (IData)(vlSelfRef.top__DOT__vgc_inst__DOT__cursor_x)));
                                        }
                                    }
                                } else if ((1U & ((~ (IData)(vlSelfRef.top__DOT__vgc_inst__DOT__cmd_busy)) 
                                                  & (~ (IData)(vlSelfRef.top__DOT__vgc_inst__DOT__artist_busy))))) {
                                    vlSelfRef.top__DOT__vgc_inst__DOT__cmd_x 
                                        = ((0x00000300U 
                                            & (vlSelfRef.top__DOT__vgc_inst__DOT__regs[18U] 
                                               << 8U)) 
                                           | vlSelfRef.top__DOT__vgc_inst__DOT__regs[17U]);
                                    vlSelfRef.top__DOT__vgc_inst__DOT__cmd_y 
                                        = ((0x00000300U 
                                            & (vlSelfRef.top__DOT__vgc_inst__DOT__regs[20U] 
                                               << 8U)) 
                                           | vlSelfRef.top__DOT__vgc_inst__DOT__regs[19U]);
                                    if ((1U & (~ ((IData)(vlSelfRef.top__DOT__cpu_dout) 
                                                  >> 7U)))) {
                                        if ((1U & (~ 
                                                   ((IData)(vlSelfRef.top__DOT__cpu_dout) 
                                                    >> 6U)))) {
                                            if ((0x00000020U 
                                                 & (IData)(vlSelfRef.top__DOT__cpu_dout))) {
                                                if (
                                                    (1U 
                                                     & (~ 
                                                        ((IData)(vlSelfRef.top__DOT__cpu_dout) 
                                                         >> 4U)))) {
                                                    if (
                                                        (1U 
                                                         & (~ 
                                                            ((IData)(vlSelfRef.top__DOT__cpu_dout) 
                                                             >> 3U)))) {
                                                        if (
                                                            (1U 
                                                             & (~ 
                                                                ((IData)(vlSelfRef.top__DOT__cpu_dout) 
                                                                 >> 2U)))) {
                                                            if (
                                                                (2U 
                                                                 & (IData)(vlSelfRef.top__DOT__cpu_dout))) {
                                                                if (
                                                                    (1U 
                                                                     & (~ (IData)(vlSelfRef.top__DOT__cpu_dout)))) {
                                                                    __Vdly__top__DOT__vgc_inst__DOT__copper_target_list 
                                                                        = vlSelfRef.top__DOT__vgc_inst__DOT__copper_active_list;
                                                                }
                                                            } else if (
                                                                       (1U 
                                                                        & (IData)(vlSelfRef.top__DOT__cpu_dout))) {
                                                                __Vdly__top__DOT__vgc_inst__DOT__copper_pending_list 
                                                                    = 
                                                                    (0x0000007fU 
                                                                     & vlSelfRef.top__DOT__vgc_inst__DOT__regs[17U]);
                                                            } else {
                                                                __Vdly__top__DOT__vgc_inst__DOT__copper_target_list 
                                                                    = 
                                                                    (0x0000007fU 
                                                                     & vlSelfRef.top__DOT__vgc_inst__DOT__regs[17U]);
                                                            }
                                                        }
                                                    }
                                                }
                                            } else if (
                                                       (0x00000010U 
                                                        & (IData)(vlSelfRef.top__DOT__cpu_dout))) {
                                                if (
                                                    (8U 
                                                     & (IData)(vlSelfRef.top__DOT__cpu_dout))) {
                                                    if (
                                                        (4U 
                                                         & (IData)(vlSelfRef.top__DOT__cpu_dout))) {
                                                        if (
                                                            (2U 
                                                             & (IData)(vlSelfRef.top__DOT__cpu_dout))) {
                                                            if (
                                                                (1U 
                                                                 & (IData)(vlSelfRef.top__DOT__cpu_dout))) {
                                                                vlSelfRef.top__DOT__vgc_inst__DOT__unnamedblk14__DOT__i = 0U;
                                                                __Vdly__top__DOT__vgc_inst__DOT__cursor_x = 0U;
                                                                __Vdly__top__DOT__vgc_inst__DOT__cursor_y = 0U;
                                                                vlSelfRef.top__DOT__vgc_inst__DOT__mode = 0U;
                                                                __Vdly__top__DOT__vgc_inst__DOT__fg_color = 1U;
                                                                vlSelfRef.top__DOT__vgc_inst__DOT__bg_color = 6U;
                                                                vlSelfRef.top__DOT__vgc_inst__DOT__border_color = 6U;
                                                                vlSelfRef.top__DOT__vgc_inst__DOT__gfx_color = 1U;
                                                                __Vdly__top__DOT__vgc_inst__DOT__scroll_offset = 0U;
                                                                vlSelfRef.top__DOT__vgc_inst__DOT__cursor_enable = 1U;
                                                                __Vdly__top__DOT__vgc_inst__DOT__copper_enabled = 0U;
                                                                __Vdly__top__DOT__vgc_inst__DOT__copper_count = 0U;
                                                                __Vdly__top__DOT__vgc_inst__DOT__copper_target_list = 0U;
                                                                __Vdly__top__DOT__vgc_inst__DOT__copper_active_list = 0U;
                                                                __Vdly__top__DOT__vgc_inst__DOT__copper_pending_list = 0U;
                                                                __Vdly__top__DOT__vgc_inst__DOT__copper_loading = 0U;
                                                                while (
                                                                       VL_GTS_III(32, 0x00000080U, vlSelfRef.top__DOT__vgc_inst__DOT__unnamedblk14__DOT__i)) {
                                                                    __VdlyDim0__top__DOT__vgc_inst__DOT__copper_list_count__v0 
                                                                        = 
                                                                        (0x0000007fU 
                                                                         & vlSelfRef.top__DOT__vgc_inst__DOT__unnamedblk14__DOT__i);
                                                                    vlSelfRef.__VdlyCommitQueuetop__DOT__vgc_inst__DOT__copper_list_count.enqueue(0U, (IData)(__VdlyDim0__top__DOT__vgc_inst__DOT__copper_list_count__v0));
                                                                    vlSelfRef.top__DOT__vgc_inst__DOT__unnamedblk14__DOT__i 
                                                                        = 
                                                                        ((IData)(1U) 
                                                                         + vlSelfRef.top__DOT__vgc_inst__DOT__unnamedblk14__DOT__i);
                                                                }
                                                            } else {
                                                                __Vdly__top__DOT__vgc_inst__DOT__copper_enabled = 0U;
                                                            }
                                                        } else if (
                                                                   (1U 
                                                                    & (IData)(vlSelfRef.top__DOT__cpu_dout))) {
                                                            __Vdly__top__DOT__vgc_inst__DOT__copper_enabled = 1U;
                                                        } else {
                                                            __VdlyDim0__top__DOT__vgc_inst__DOT__copper_list_count__v1 
                                                                = vlSelfRef.top__DOT__vgc_inst__DOT__copper_target_list;
                                                            vlSelfRef.__VdlyCommitQueuetop__DOT__vgc_inst__DOT__copper_list_count.enqueue(0U, (IData)(__VdlyDim0__top__DOT__vgc_inst__DOT__copper_list_count__v1));
                                                            if (
                                                                ((IData)(vlSelfRef.top__DOT__vgc_inst__DOT__copper_target_list) 
                                                                 == (IData)(vlSelfRef.top__DOT__vgc_inst__DOT__copper_active_list))) {
                                                                __Vdly__top__DOT__vgc_inst__DOT__copper_count = 0U;
                                                            }
                                                        }
                                                    } else if (
                                                               (2U 
                                                                & (IData)(vlSelfRef.top__DOT__cpu_dout))) {
                                                        if (
                                                            (1U 
                                                             & (IData)(vlSelfRef.top__DOT__cpu_dout))) {
                                                            if (
                                                                (0x0020U 
                                                                 > vlSelfRef.top__DOT__vgc_inst__DOT__copper_list_count
                                                                 [vlSelfRef.top__DOT__vgc_inst__DOT__copper_target_list])) {
                                                                __VdlyVal__top__DOT__vgc_inst__DOT__copper_list_pos__v0 
                                                                    = 
                                                                    (0x0001ffffU 
                                                                     & (((IData)(0x00000140U) 
                                                                         * vlSelfRef.top__DOT__vgc_inst__DOT__regs[19U]) 
                                                                        + 
                                                                        ((0x00000100U 
                                                                          & (vlSelfRef.top__DOT__vgc_inst__DOT__regs[18U] 
                                                                             << 8U)) 
                                                                         | vlSelfRef.top__DOT__vgc_inst__DOT__regs[17U])));
                                                                __VdlyDim0__top__DOT__vgc_inst__DOT__copper_list_pos__v0 
                                                                    = 
                                                                    (((IData)(vlSelfRef.top__DOT__vgc_inst__DOT__copper_target_list) 
                                                                      << 5U) 
                                                                     | (0x0000001fU 
                                                                        & vlSelfRef.top__DOT__vgc_inst__DOT__copper_list_count
                                                                        [vlSelfRef.top__DOT__vgc_inst__DOT__copper_target_list]));
                                                                __VdlySet__top__DOT__vgc_inst__DOT__copper_list_pos__v0 = 1U;
                                                                __VdlyVal__top__DOT__vgc_inst__DOT__copper_list_reg__v0 
                                                                    = vlSelfRef.top__DOT__vgc_inst__DOT__regs[20U];
                                                                __VdlyDim0__top__DOT__vgc_inst__DOT__copper_list_reg__v0 
                                                                    = 
                                                                    (((IData)(vlSelfRef.top__DOT__vgc_inst__DOT__copper_target_list) 
                                                                      << 5U) 
                                                                     | (0x0000001fU 
                                                                        & vlSelfRef.top__DOT__vgc_inst__DOT__copper_list_count
                                                                        [vlSelfRef.top__DOT__vgc_inst__DOT__copper_target_list]));
                                                                __VdlyVal__top__DOT__vgc_inst__DOT__copper_list_val__v0 
                                                                    = vlSelfRef.top__DOT__vgc_inst__DOT__regs[22U];
                                                                __VdlyDim0__top__DOT__vgc_inst__DOT__copper_list_val__v0 
                                                                    = 
                                                                    (((IData)(vlSelfRef.top__DOT__vgc_inst__DOT__copper_target_list) 
                                                                      << 5U) 
                                                                     | (0x0000001fU 
                                                                        & vlSelfRef.top__DOT__vgc_inst__DOT__copper_list_count
                                                                        [vlSelfRef.top__DOT__vgc_inst__DOT__copper_target_list]));
                                                                __VdlyVal__top__DOT__vgc_inst__DOT__copper_list_count__v2 
                                                                    = 
                                                                    (0x000001ffU 
                                                                     & ((IData)(1U) 
                                                                        + vlSelfRef.top__DOT__vgc_inst__DOT__copper_list_count
                                                                        [vlSelfRef.top__DOT__vgc_inst__DOT__copper_target_list]));
                                                                __VdlyDim0__top__DOT__vgc_inst__DOT__copper_list_count__v2 
                                                                    = vlSelfRef.top__DOT__vgc_inst__DOT__copper_target_list;
                                                                vlSelfRef.__VdlyCommitQueuetop__DOT__vgc_inst__DOT__copper_list_count.enqueue(__VdlyVal__top__DOT__vgc_inst__DOT__copper_list_count__v2, (IData)(__VdlyDim0__top__DOT__vgc_inst__DOT__copper_list_count__v2));
                                                                if (
                                                                    ((IData)(vlSelfRef.top__DOT__vgc_inst__DOT__copper_target_list) 
                                                                     == (IData)(vlSelfRef.top__DOT__vgc_inst__DOT__copper_active_list))) {
                                                                    __VdlyVal__top__DOT__vgc_inst__DOT__copper_pos__v1 
                                                                        = 
                                                                        (0x0001ffffU 
                                                                         & (((IData)(0x00000140U) 
                                                                             * vlSelfRef.top__DOT__vgc_inst__DOT__regs[19U]) 
                                                                            + 
                                                                            ((0x00000100U 
                                                                              & (vlSelfRef.top__DOT__vgc_inst__DOT__regs[18U] 
                                                                                << 8U)) 
                                                                             | vlSelfRef.top__DOT__vgc_inst__DOT__regs[17U])));
                                                                    __VdlyDim0__top__DOT__vgc_inst__DOT__copper_pos__v1 
                                                                        = 
                                                                        (0x0000001fU 
                                                                         & (IData)(vlSelfRef.top__DOT__vgc_inst__DOT__copper_count));
                                                                    __VdlySet__top__DOT__vgc_inst__DOT__copper_pos__v1 = 1U;
                                                                    __VdlyVal__top__DOT__vgc_inst__DOT__copper_reg__v1 
                                                                        = vlSelfRef.top__DOT__vgc_inst__DOT__regs[20U];
                                                                    __VdlyDim0__top__DOT__vgc_inst__DOT__copper_reg__v1 
                                                                        = 
                                                                        (0x0000001fU 
                                                                         & (IData)(vlSelfRef.top__DOT__vgc_inst__DOT__copper_count));
                                                                    __VdlyVal__top__DOT__vgc_inst__DOT__copper_val__v1 
                                                                        = vlSelfRef.top__DOT__vgc_inst__DOT__regs[22U];
                                                                    __VdlyDim0__top__DOT__vgc_inst__DOT__copper_val__v1 
                                                                        = 
                                                                        (0x0000001fU 
                                                                         & (IData)(vlSelfRef.top__DOT__vgc_inst__DOT__copper_count));
                                                                    __Vdly__top__DOT__vgc_inst__DOT__copper_count 
                                                                        = 
                                                                        (0x000001ffU 
                                                                         & ((IData)(1U) 
                                                                            + (IData)(vlSelfRef.top__DOT__vgc_inst__DOT__copper_count)));
                                                                }
                                                            }
                                                        } else if (
                                                                   (0U 
                                                                    == vlSelfRef.top__DOT__vgc_inst__DOT__regs[17U])) {
                                                            vlSelfRef.top__DOT__vgc_inst__DOT__cmd_char_addr 
                                                                = 
                                                                ((0x00000700U 
                                                                  & (vlSelfRef.top__DOT__vgc_inst__DOT__regs[19U] 
                                                                     << 8U)) 
                                                                 | vlSelfRef.top__DOT__vgc_inst__DOT__regs[18U]);
                                                            vlSelfRef.top__DOT__vgc_inst__DOT__cmd_char_we = 1U;
                                                            vlSelfRef.top__DOT__vgc_inst__DOT__cmd_char_din 
                                                                = vlSelfRef.top__DOT__vgc_inst__DOT__regs[20U];
                                                        } else if (
                                                                   (1U 
                                                                    == vlSelfRef.top__DOT__vgc_inst__DOT__regs[17U])) {
                                                            vlSelfRef.top__DOT__vgc_inst__DOT__cmd_color_addr 
                                                                = 
                                                                ((0x00000700U 
                                                                  & (vlSelfRef.top__DOT__vgc_inst__DOT__regs[19U] 
                                                                     << 8U)) 
                                                                 | vlSelfRef.top__DOT__vgc_inst__DOT__regs[18U]);
                                                            vlSelfRef.top__DOT__vgc_inst__DOT__cmd_color_we = 1U;
                                                            vlSelfRef.top__DOT__vgc_inst__DOT__cmd_color_din 
                                                                = vlSelfRef.top__DOT__vgc_inst__DOT__regs[20U];
                                                        } else if (
                                                                   (2U 
                                                                    == vlSelfRef.top__DOT__vgc_inst__DOT__regs[17U])) {
                                                            vlSelfRef.top__DOT__vgc_inst__DOT__cmd_gfx_addr 
                                                                = 
                                                                ((vlSelfRef.top__DOT__vgc_inst__DOT__regs[19U] 
                                                                  << 8U) 
                                                                 | vlSelfRef.top__DOT__vgc_inst__DOT__regs[18U]);
                                                            vlSelfRef.top__DOT__vgc_inst__DOT__cmd_gfx_we = 1U;
                                                            vlSelfRef.top__DOT__vgc_inst__DOT__cmd_gfx_din 
                                                                = 
                                                                (0x0000000fU 
                                                                 & vlSelfRef.top__DOT__vgc_inst__DOT__regs[20U]);
                                                        } else if (
                                                                   (3U 
                                                                    == vlSelfRef.top__DOT__vgc_inst__DOT__regs[17U])) {
                                                            vlSelfRef.top__DOT__vgc_inst__DOT__cmd_spr_addr 
                                                                = 
                                                                ((0x00000700U 
                                                                  & (vlSelfRef.top__DOT__vgc_inst__DOT__regs[19U] 
                                                                     << 8U)) 
                                                                 | vlSelfRef.top__DOT__vgc_inst__DOT__regs[18U]);
                                                            vlSelfRef.top__DOT__vgc_inst__DOT__cmd_spr_we = 1U;
                                                            vlSelfRef.top__DOT__vgc_inst__DOT__cmd_spr_din 
                                                                = vlSelfRef.top__DOT__vgc_inst__DOT__regs[20U];
                                                        } else if (
                                                                   (4U 
                                                                    == vlSelfRef.top__DOT__vgc_inst__DOT__regs[17U])) {
                                                            vlSelfRef.top__DOT__vgc_inst__DOT__vgc_tile_addr 
                                                                = 
                                                                ((0x00007f00U 
                                                                  & (vlSelfRef.top__DOT__vgc_inst__DOT__regs[19U] 
                                                                     << 8U)) 
                                                                 | vlSelfRef.top__DOT__vgc_inst__DOT__regs[18U]);
                                                            vlSelfRef.top__DOT__vgc_inst__DOT__vgc_tile_we = 1U;
                                                            vlSelfRef.top__DOT__vgc_inst__DOT__vgc_tile_wdata 
                                                                = vlSelfRef.top__DOT__vgc_inst__DOT__regs[20U];
                                                        }
                                                    } else if (
                                                               (1U 
                                                                & (IData)(vlSelfRef.top__DOT__cpu_dout))) {
                                                        if (
                                                            (0U 
                                                             == vlSelfRef.top__DOT__vgc_inst__DOT__regs[17U])) {
                                                            vlSelfRef.top__DOT__vgc_inst__DOT__cmd_char_addr 
                                                                = 
                                                                ((0x00000700U 
                                                                  & (vlSelfRef.top__DOT__vgc_inst__DOT__regs[19U] 
                                                                     << 8U)) 
                                                                 | vlSelfRef.top__DOT__vgc_inst__DOT__regs[18U]);
                                                            __Vdly__top__DOT__vgc_inst__DOT__memread_pending = 1U;
                                                            vlSelfRef.top__DOT__vgc_inst__DOT__memread_space = 0U;
                                                        } else if (
                                                                   (1U 
                                                                    == vlSelfRef.top__DOT__vgc_inst__DOT__regs[17U])) {
                                                            vlSelfRef.top__DOT__vgc_inst__DOT__cmd_color_addr 
                                                                = 
                                                                ((0x00000700U 
                                                                  & (vlSelfRef.top__DOT__vgc_inst__DOT__regs[19U] 
                                                                     << 8U)) 
                                                                 | vlSelfRef.top__DOT__vgc_inst__DOT__regs[18U]);
                                                            __Vdly__top__DOT__vgc_inst__DOT__memread_pending = 1U;
                                                            vlSelfRef.top__DOT__vgc_inst__DOT__memread_space = 1U;
                                                        } else if (
                                                                   (2U 
                                                                    == vlSelfRef.top__DOT__vgc_inst__DOT__regs[17U])) {
                                                            vlSelfRef.top__DOT__vgc_inst__DOT__cmd_gfx_addr 
                                                                = 
                                                                ((vlSelfRef.top__DOT__vgc_inst__DOT__regs[19U] 
                                                                  << 8U) 
                                                                 | vlSelfRef.top__DOT__vgc_inst__DOT__regs[18U]);
                                                            vlSelfRef.top__DOT__vgc_inst__DOT__cmd_gfx_re = 1U;
                                                            __Vdly__top__DOT__vgc_inst__DOT__memread_pending = 1U;
                                                            vlSelfRef.top__DOT__vgc_inst__DOT__memread_space = 2U;
                                                        } else if (
                                                                   (3U 
                                                                    == vlSelfRef.top__DOT__vgc_inst__DOT__regs[17U])) {
                                                            vlSelfRef.top__DOT__vgc_inst__DOT__cmd_spr_addr 
                                                                = 
                                                                ((0x00000700U 
                                                                  & (vlSelfRef.top__DOT__vgc_inst__DOT__regs[19U] 
                                                                     << 8U)) 
                                                                 | vlSelfRef.top__DOT__vgc_inst__DOT__regs[18U]);
                                                            vlSelfRef.top__DOT__vgc_inst__DOT__cmd_spr_re = 1U;
                                                            __Vdly__top__DOT__vgc_inst__DOT__memread_pending = 1U;
                                                            vlSelfRef.top__DOT__vgc_inst__DOT__memread_space = 3U;
                                                        } else if (
                                                                   (4U 
                                                                    == vlSelfRef.top__DOT__vgc_inst__DOT__regs[17U])) {
                                                            vlSelfRef.top__DOT__vgc_inst__DOT__vgc_tile_addr 
                                                                = 
                                                                ((0x00007f00U 
                                                                  & (vlSelfRef.top__DOT__vgc_inst__DOT__regs[19U] 
                                                                     << 8U)) 
                                                                 | vlSelfRef.top__DOT__vgc_inst__DOT__regs[18U]);
                                                            vlSelfRef.top__DOT__vgc_inst__DOT__vgc_tile_re = 1U;
                                                            __Vdly__top__DOT__vgc_inst__DOT__memread_pending = 1U;
                                                            vlSelfRef.top__DOT__vgc_inst__DOT__memread_space = 4U;
                                                        }
                                                    } else if (
                                                               (0x10U 
                                                                > vlSelfRef.top__DOT__vgc_inst__DOT__regs[17U])) {
                                                        __VdlyVal__top__DOT__vgc_inst__DOT__spr_pri__v16 
                                                            = 
                                                            (3U 
                                                             & vlSelfRef.top__DOT__vgc_inst__DOT__regs[18U]);
                                                        __VdlyDim0__top__DOT__vgc_inst__DOT__spr_pri__v16 
                                                            = 
                                                            (0x0000000fU 
                                                             & vlSelfRef.top__DOT__vgc_inst__DOT__regs[17U]);
                                                        __VdlySet__top__DOT__vgc_inst__DOT__spr_pri__v16 = 1U;
                                                    }
                                                } else if (
                                                           (4U 
                                                            & (IData)(vlSelfRef.top__DOT__cpu_dout))) {
                                                    if (
                                                        (2U 
                                                         & (IData)(vlSelfRef.top__DOT__cpu_dout))) {
                                                        if (
                                                            (1U 
                                                             & (IData)(vlSelfRef.top__DOT__cpu_dout))) {
                                                            if (
                                                                (0x10U 
                                                                 > vlSelfRef.top__DOT__vgc_inst__DOT__regs[17U])) {
                                                                __VdlyVal__top__DOT__vgc_inst__DOT__spr_flip_h__v16 
                                                                    = 
                                                                    (1U 
                                                                     & vlSelfRef.top__DOT__vgc_inst__DOT__regs[18U]);
                                                                __VdlyDim0__top__DOT__vgc_inst__DOT__spr_flip_h__v16 
                                                                    = 
                                                                    (0x0000000fU 
                                                                     & vlSelfRef.top__DOT__vgc_inst__DOT__regs[17U]);
                                                                __VdlySet__top__DOT__vgc_inst__DOT__spr_flip_h__v16 = 1U;
                                                                __VdlyVal__top__DOT__vgc_inst__DOT__spr_flip_v__v16 
                                                                    = 
                                                                    (1U 
                                                                     & (vlSelfRef.top__DOT__vgc_inst__DOT__regs[18U] 
                                                                        >> 1U));
                                                                __VdlyDim0__top__DOT__vgc_inst__DOT__spr_flip_v__v16 
                                                                    = 
                                                                    (0x0000000fU 
                                                                     & vlSelfRef.top__DOT__vgc_inst__DOT__regs[17U]);
                                                            }
                                                        } else if (
                                                                   (0x10U 
                                                                    > vlSelfRef.top__DOT__vgc_inst__DOT__regs[17U])) {
                                                            __VdlyDim0__top__DOT__vgc_inst__DOT__spr_enable__v16 
                                                                = 
                                                                (0x0000000fU 
                                                                 & vlSelfRef.top__DOT__vgc_inst__DOT__regs[17U]);
                                                            __VdlySet__top__DOT__vgc_inst__DOT__spr_enable__v16 = 1U;
                                                        }
                                                    } else if (
                                                               (1U 
                                                                & (IData)(vlSelfRef.top__DOT__cpu_dout))) {
                                                        if (
                                                            (0x10U 
                                                             > vlSelfRef.top__DOT__vgc_inst__DOT__regs[17U])) {
                                                            __VdlyDim0__top__DOT__vgc_inst__DOT__spr_enable__v17 
                                                                = 
                                                                (0x0000000fU 
                                                                 & vlSelfRef.top__DOT__vgc_inst__DOT__regs[17U]);
                                                            __VdlySet__top__DOT__vgc_inst__DOT__spr_enable__v17 = 1U;
                                                        }
                                                    } else if (
                                                               (0x10U 
                                                                > vlSelfRef.top__DOT__vgc_inst__DOT__regs[17U])) {
                                                        __VdlyVal__top__DOT__vgc_inst__DOT__spr_x__v16 
                                                            = 
                                                            ((0x00000100U 
                                                              & (vlSelfRef.top__DOT__vgc_inst__DOT__regs[19U] 
                                                                 << 8U)) 
                                                             | vlSelfRef.top__DOT__vgc_inst__DOT__regs[18U]);
                                                        __VdlyDim0__top__DOT__vgc_inst__DOT__spr_x__v16 
                                                            = 
                                                            (0x0000000fU 
                                                             & vlSelfRef.top__DOT__vgc_inst__DOT__regs[17U]);
                                                        __VdlySet__top__DOT__vgc_inst__DOT__spr_x__v16 = 1U;
                                                        __VdlyVal__top__DOT__vgc_inst__DOT__spr_y__v16 
                                                            = vlSelfRef.top__DOT__vgc_inst__DOT__regs[20U];
                                                        __VdlyDim0__top__DOT__vgc_inst__DOT__spr_y__v16 
                                                            = 
                                                            (0x0000000fU 
                                                             & vlSelfRef.top__DOT__vgc_inst__DOT__regs[17U]);
                                                    }
                                                } else if (
                                                           (2U 
                                                            & (IData)(vlSelfRef.top__DOT__cpu_dout))) {
                                                    if (
                                                        (1U 
                                                         & (IData)(vlSelfRef.top__DOT__cpu_dout))) {
                                                        if (
                                                            ((0x10U 
                                                              > vlSelfRef.top__DOT__vgc_inst__DOT__regs[17U]) 
                                                             & (0x10U 
                                                                > vlSelfRef.top__DOT__vgc_inst__DOT__regs[18U]))) {
                                                            __Vdly__top__DOT__vgc_inst__DOT__cmd_cx = 0U;
                                                            __Vdly__top__DOT__vgc_inst__DOT__cmd_cy = 0U;
                                                            vlSelfRef.top__DOT__vgc_inst__DOT__cmd_x 
                                                                = 
                                                                (0x0000000fU 
                                                                 & vlSelfRef.top__DOT__vgc_inst__DOT__regs[17U]);
                                                            __Vdly__top__DOT__vgc_inst__DOT__sprcopy_phase = 0U;
                                                            __Vdly__top__DOT__vgc_inst__DOT__cmd_busy = 1U;
                                                            __Vdly__top__DOT__vgc_inst__DOT__cmd_op = 0x13U;
                                                            vlSelfRef.top__DOT__vgc_inst__DOT__cmd_y 
                                                                = 
                                                                (0x0000000fU 
                                                                 & vlSelfRef.top__DOT__vgc_inst__DOT__regs[18U]);
                                                        }
                                                    } else if (
                                                               (0x10U 
                                                                > vlSelfRef.top__DOT__vgc_inst__DOT__regs[17U])) {
                                                        __Vdly__top__DOT__vgc_inst__DOT__cmd_cx = 0U;
                                                        __Vdly__top__DOT__vgc_inst__DOT__cmd_cy = 0U;
                                                        vlSelfRef.top__DOT__vgc_inst__DOT__cmd_x 
                                                            = 
                                                            (0x0000001eU 
                                                             & (vlSelfRef.top__DOT__vgc_inst__DOT__regs[17U] 
                                                                << 1U));
                                                        __Vdly__top__DOT__vgc_inst__DOT__cmd_busy = 1U;
                                                        __Vdly__top__DOT__vgc_inst__DOT__cmd_op = 0x12U;
                                                    }
                                                } else if (
                                                           (1U 
                                                            & (IData)(vlSelfRef.top__DOT__cpu_dout))) {
                                                    if (
                                                        ((0x10U 
                                                          > vlSelfRef.top__DOT__vgc_inst__DOT__regs[17U]) 
                                                         & (0x10U 
                                                            > vlSelfRef.top__DOT__vgc_inst__DOT__regs[18U]))) {
                                                        vlSelfRef.top__DOT__vgc_inst__DOT__sprrow_spr 
                                                            = 
                                                            (0x0000000fU 
                                                             & vlSelfRef.top__DOT__vgc_inst__DOT__regs[17U]);
                                                        __Vdly__top__DOT__vgc_inst__DOT__sprrow_count = 0U;
                                                        __Vdly__top__DOT__vgc_inst__DOT__cmd_busy = 1U;
                                                        __Vdly__top__DOT__vgc_inst__DOT__cmd_op = 0x11U;
                                                        vlSelfRef.top__DOT__vgc_inst__DOT__sprrow_row 
                                                            = 
                                                            (0x0000000fU 
                                                             & vlSelfRef.top__DOT__vgc_inst__DOT__regs[18U]);
                                                        __VdlyVal__top__DOT__vgc_inst__DOT__sprrow_data__v0 
                                                            = vlSelfRef.top__DOT__vgc_inst__DOT__regs[19U];
                                                        __VdlySet__top__DOT__vgc_inst__DOT__sprrow_data__v0 = 1U;
                                                        __VdlyVal__top__DOT__vgc_inst__DOT__sprrow_data__v1 
                                                            = vlSelfRef.top__DOT__vgc_inst__DOT__regs[20U];
                                                        __VdlyVal__top__DOT__vgc_inst__DOT__sprrow_data__v2 
                                                            = vlSelfRef.top__DOT__vgc_inst__DOT__regs[21U];
                                                        __VdlyVal__top__DOT__vgc_inst__DOT__sprrow_data__v3 
                                                            = vlSelfRef.top__DOT__vgc_inst__DOT__regs[22U];
                                                        __VdlyVal__top__DOT__vgc_inst__DOT__sprrow_data__v4 
                                                            = vlSelfRef.top__DOT__vgc_inst__DOT__regs[23U];
                                                        __VdlyVal__top__DOT__vgc_inst__DOT__sprrow_data__v5 
                                                            = vlSelfRef.top__DOT__vgc_inst__DOT__regs[24U];
                                                        __VdlyVal__top__DOT__vgc_inst__DOT__sprrow_data__v6 
                                                            = vlSelfRef.top__DOT__vgc_inst__DOT__regs[25U];
                                                        __VdlyVal__top__DOT__vgc_inst__DOT__sprrow_data__v7 
                                                            = vlSelfRef.top__DOT__vgc_inst__DOT__regs[26U];
                                                    }
                                                } else if (
                                                           (((0x10U 
                                                              > vlSelfRef.top__DOT__vgc_inst__DOT__regs[17U]) 
                                                             & (0x10U 
                                                                > vlSelfRef.top__DOT__vgc_inst__DOT__regs[18U])) 
                                                            & (0x10U 
                                                               > vlSelfRef.top__DOT__vgc_inst__DOT__regs[19U]))) {
                                                    vlSelfRef.top__DOT__vgc_inst__DOT__cmd_spr_addr 
                                                        = 
                                                        ((0x00000780U 
                                                          & (vlSelfRef.top__DOT__vgc_inst__DOT__regs[17U] 
                                                             << 7U)) 
                                                         | ((0x00000078U 
                                                             & (vlSelfRef.top__DOT__vgc_inst__DOT__regs[19U] 
                                                                << 3U)) 
                                                            | (7U 
                                                               & (vlSelfRef.top__DOT__vgc_inst__DOT__regs[18U] 
                                                                  >> 1U))));
                                                    vlSelfRef.top__DOT__vgc_inst__DOT__cmd_spr_re = 1U;
                                                    __Vdly__top__DOT__vgc_inst__DOT__cmd_busy = 1U;
                                                    __Vdly__top__DOT__vgc_inst__DOT__cmd_op = 0x10U;
                                                }
                                            } else if (
                                                       (8U 
                                                        & (IData)(vlSelfRef.top__DOT__cpu_dout))) {
                                                if (
                                                    (1U 
                                                     & (~ 
                                                        ((IData)(vlSelfRef.top__DOT__cpu_dout) 
                                                         >> 2U)))) {
                                                    if (
                                                        (2U 
                                                         & (IData)(vlSelfRef.top__DOT__cpu_dout))) {
                                                        if (
                                                            (1U 
                                                             & (~ (IData)(vlSelfRef.top__DOT__cpu_dout)))) {
                                                            vlSelfRef.top__DOT__vgc_inst__DOT__artist_cmd_valid = 1U;
                                                            vlSelfRef.top__DOT__vgc_inst__DOT__artist_cmd_code 
                                                                = vlSelfRef.top__DOT__cpu_dout;
                                                        }
                                                    } else if (
                                                               (1U 
                                                                & (IData)(vlSelfRef.top__DOT__cpu_dout))) {
                                                        vlSelfRef.top__DOT__vgc_inst__DOT__artist_cmd_valid = 1U;
                                                        vlSelfRef.top__DOT__vgc_inst__DOT__artist_cmd_code 
                                                            = vlSelfRef.top__DOT__cpu_dout;
                                                    } else {
                                                        vlSelfRef.top__DOT__vgc_inst__DOT__gfx_color 
                                                            = 
                                                            (0x0000000fU 
                                                             & vlSelfRef.top__DOT__vgc_inst__DOT__regs[17U]);
                                                    }
                                                }
                                            } else if (
                                                       (4U 
                                                        & (IData)(vlSelfRef.top__DOT__cpu_dout))) {
                                                vlSelfRef.top__DOT__vgc_inst__DOT__artist_cmd_valid = 1U;
                                                vlSelfRef.top__DOT__vgc_inst__DOT__artist_cmd_code 
                                                    = vlSelfRef.top__DOT__cpu_dout;
                                            } else if (
                                                       (2U 
                                                        & (IData)(vlSelfRef.top__DOT__cpu_dout))) {
                                                vlSelfRef.top__DOT__vgc_inst__DOT__artist_cmd_valid = 1U;
                                                vlSelfRef.top__DOT__vgc_inst__DOT__artist_cmd_code 
                                                    = vlSelfRef.top__DOT__cpu_dout;
                                            } else if (
                                                       (1U 
                                                        & (IData)(vlSelfRef.top__DOT__cpu_dout))) {
                                                vlSelfRef.top__DOT__vgc_inst__DOT__artist_cmd_valid = 1U;
                                                vlSelfRef.top__DOT__vgc_inst__DOT__artist_cmd_code 
                                                    = vlSelfRef.top__DOT__cpu_dout;
                                            }
                                        }
                                    }
                                }
                            }
                        }
                    }
                } else {
                    __VdlyVal__top__DOT__vgc_inst__DOT__regs__v35 
                        = vlSelfRef.top__DOT__cpu_dout;
                    __VdlyDim0__top__DOT__vgc_inst__DOT__regs__v35 
                        = (0x0000001fU & (IData)(vlSelfRef.top__DOT__cpu_addr));
                    __VdlySet__top__DOT__vgc_inst__DOT__regs__v35 = 1U;
                }
            }
            if (vlSelfRef.top__DOT__vgc_inst__DOT__char_ram_sel) {
                vlSelfRef.top__DOT__vgc_inst__DOT__cmd_char_addr 
                    = (0x000007ffU & ((IData)(vlSelfRef.top__DOT__cpu_addr) 
                                      - (IData)(0x0200U)));
                vlSelfRef.top__DOT__vgc_inst__DOT__cmd_char_din 
                    = vlSelfRef.top__DOT__cpu_dout;
                vlSelfRef.top__DOT__vgc_inst__DOT__cmd_char_we = 1U;
            }
            if (vlSelfRef.top__DOT__vgc_inst__DOT__color_ram_sel) {
                vlSelfRef.top__DOT__vgc_inst__DOT__cmd_color_addr 
                    = (0x000007ffU & ((IData)(vlSelfRef.top__DOT__cpu_addr) 
                                      - (IData)(0x01d0U)));
                vlSelfRef.top__DOT__vgc_inst__DOT__cmd_color_din 
                    = vlSelfRef.top__DOT__cpu_dout;
                vlSelfRef.top__DOT__vgc_inst__DOT__cmd_color_we = 1U;
            }
            if (vlSelfRef.top__DOT__vgc_inst__DOT__spr_reg_sel) {
                if ((4U & (IData)(vlSelfRef.top__DOT__cpu_addr))) {
                    if ((2U & (IData)(vlSelfRef.top__DOT__cpu_addr))) {
                        if ((1U & (~ (IData)(vlSelfRef.top__DOT__cpu_addr)))) {
                            __VdlyVal__top__DOT__vgc_inst__DOT__spr_pri__v17 
                                = (3U & (IData)(vlSelfRef.top__DOT__cpu_dout));
                            __VdlyDim0__top__DOT__vgc_inst__DOT__spr_pri__v17 
                                = (0x0000000fU & ((IData)(vlSelfRef.top__DOT__cpu_addr) 
                                                  >> 3U));
                            __VdlySet__top__DOT__vgc_inst__DOT__spr_pri__v17 = 1U;
                        }
                    } else if ((1U & (IData)(vlSelfRef.top__DOT__cpu_addr))) {
                        __VdlyVal__top__DOT__vgc_inst__DOT__spr_flip_h__v17 
                            = (1U & (IData)(vlSelfRef.top__DOT__cpu_dout));
                        __VdlyDim0__top__DOT__vgc_inst__DOT__spr_flip_h__v17 
                            = (0x0000000fU & ((IData)(vlSelfRef.top__DOT__cpu_addr) 
                                              >> 3U));
                        __VdlySet__top__DOT__vgc_inst__DOT__spr_flip_h__v17 = 1U;
                        __VdlyVal__top__DOT__vgc_inst__DOT__spr_flip_v__v17 
                            = (1U & ((IData)(vlSelfRef.top__DOT__cpu_dout) 
                                     >> 1U));
                        __VdlyDim0__top__DOT__vgc_inst__DOT__spr_flip_v__v17 
                            = (0x0000000fU & ((IData)(vlSelfRef.top__DOT__cpu_addr) 
                                              >> 3U));
                        __VdlyVal__top__DOT__vgc_inst__DOT__spr_enable__v18 
                            = (1U & ((IData)(vlSelfRef.top__DOT__cpu_dout) 
                                     >> 7U));
                        __VdlyDim0__top__DOT__vgc_inst__DOT__spr_enable__v18 
                            = (0x0000000fU & ((IData)(vlSelfRef.top__DOT__cpu_addr) 
                                              >> 3U));
                    }
                } else if ((2U & (IData)(vlSelfRef.top__DOT__cpu_addr))) {
                    if ((1U & (~ (IData)(vlSelfRef.top__DOT__cpu_addr)))) {
                        __VdlyVal__top__DOT__vgc_inst__DOT__spr_y__v17 
                            = vlSelfRef.top__DOT__cpu_dout;
                        __VdlyDim0__top__DOT__vgc_inst__DOT__spr_y__v17 
                            = (0x0000000fU & ((IData)(vlSelfRef.top__DOT__cpu_addr) 
                                              >> 3U));
                        __VdlySet__top__DOT__vgc_inst__DOT__spr_y__v17 = 1U;
                    }
                } else if ((1U & (IData)(vlSelfRef.top__DOT__cpu_addr))) {
                    __VdlyVal__top__DOT__vgc_inst__DOT__spr_x__v17 
                        = (1U & (IData)(vlSelfRef.top__DOT__cpu_dout));
                    __VdlyDim0__top__DOT__vgc_inst__DOT__spr_x__v17 
                        = (0x0000000fU & ((IData)(vlSelfRef.top__DOT__cpu_addr) 
                                          >> 3U));
                    __VdlySet__top__DOT__vgc_inst__DOT__spr_x__v17 = 1U;
                } else {
                    __VdlyVal__top__DOT__vgc_inst__DOT__spr_x__v18 
                        = vlSelfRef.top__DOT__cpu_dout;
                    __VdlyDim0__top__DOT__vgc_inst__DOT__spr_x__v18 
                        = (0x0000000fU & ((IData)(vlSelfRef.top__DOT__cpu_addr) 
                                          >> 3U));
                    __VdlySet__top__DOT__vgc_inst__DOT__spr_x__v18 = 1U;
                }
            }
        }
        if ((((IData)(vlSelfRef.top__DOT__vgc_inst__DOT__cmd_busy) 
              & (0x10U == (IData)(vlSelfRef.top__DOT__vgc_inst__DOT__cmd_op))) 
             & (~ (IData)(vlSelfRef.top__DOT__vgc_inst__DOT__memread_pending)))) {
            vlSelfRef.top__DOT__vgc_inst__DOT__cmd_spr_addr 
                = ((0x00000780U & (vlSelfRef.top__DOT__vgc_inst__DOT__regs[17U] 
                                   << 7U)) | ((0x00000078U 
                                               & (vlSelfRef.top__DOT__vgc_inst__DOT__regs[19U] 
                                                  << 3U)) 
                                              | (7U 
                                                 & (vlSelfRef.top__DOT__vgc_inst__DOT__regs[18U] 
                                                    >> 1U))));
            vlSelfRef.top__DOT__vgc_inst__DOT__cmd_spr_we = 1U;
            __Vdly__top__DOT__vgc_inst__DOT__cmd_busy = 0U;
            vlSelfRef.top__DOT__vgc_inst__DOT__cmd_spr_din 
                = ((1U & vlSelfRef.top__DOT__vgc_inst__DOT__regs[18U])
                    ? ((0x000000f0U & (IData)(vlSelfRef.top__DOT__vgc_inst__DOT__spr_a_dout)) 
                       | (0x0000000fU & vlSelfRef.top__DOT__vgc_inst__DOT__regs[20U]))
                    : ((0x000000f0U & (vlSelfRef.top__DOT__vgc_inst__DOT__regs[20U] 
                                       << 4U)) | (0x0000000fU 
                                                  & (IData)(vlSelfRef.top__DOT__vgc_inst__DOT__spr_a_dout))));
        }
    }
    if ((0x27U == (IData)(vlSelfRef.top__DOT__cpu_inst__DOT__state))) {
        vlSelfRef.top__DOT__cpu_inst__DOT__D = (1U 
                                                & ((IData)(vlSelfRef.top__DOT__cpu_inst__DOT__DIMUX) 
                                                   >> 3U));
    } else if ((0x0cU == (IData)(vlSelfRef.top__DOT__cpu_inst__DOT__state))) {
        if (vlSelfRef.top__DOT__cpu_inst__DOT__sed) {
            vlSelfRef.top__DOT__cpu_inst__DOT__D = 1U;
        }
        if (vlSelfRef.top__DOT__cpu_inst__DOT__cld) {
            vlSelfRef.top__DOT__cpu_inst__DOT__D = 0U;
        }
        if (vlSelfRef.top__DOT__cpu_inst__DOT__plp) {
            vlSelfRef.top__DOT__cpu_inst__DOT__D = 
                (1U & ((IData)(vlSelfRef.top__DOT__cpu_inst__DOT__ADD) 
                       >> 3U));
        }
    }
    if ((((0x0cU == (IData)(vlSelfRef.top__DOT__cpu_inst__DOT__state)) 
          | (8U == (IData)(vlSelfRef.top__DOT__cpu_inst__DOT__state))) 
         & (IData)(vlSelfRef.top__DOT____Vcellinp__cpu_inst__RDY))) {
        vlSelfRef.top__DOT__cpu_inst__DOT__adc_sbc 
            = (0x61U == (0x63U & (IData)(vlSelfRef.top__DOT__cpu_inst__DOT__IR)));
    }
    vlSelfRef.top__DOT__vgc_inst__DOT__h_sync_area_d1 
        = ((0x0290U <= (IData)(vlSelfRef.top__DOT__vgc_inst__DOT__h_count)) 
           & (0x02f0U > (IData)(vlSelfRef.top__DOT__vgc_inst__DOT__h_count)));
    vlSelfRef.top__DOT__vgc_inst__DOT__v_sync_area_d1 
        = ((0x01eaU <= (IData)(vlSelfRef.top__DOT__vgc_inst__DOT__v_count)) 
           & (0x01ecU > (IData)(vlSelfRef.top__DOT__vgc_inst__DOT__v_count)));
    vlSelfRef.top__DOT__vgc_inst__DOT__visible_d1 = vlSelfRef.top__DOT__vgc_inst__DOT__visible;
    vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__s3_tile_pri 
        = vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__s2_tile_pri;
    vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__pr_dout_b 
        = vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__pal_ram_inst__DOT__mem
        [(((IData)(vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__s2_sub_pal) 
           << 4U) | (IData)(vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__s2_color_idx))];
    vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__at_we_a = 0U;
    vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__pr_we_a = 0U;
    vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__nt_we_a = 0U;
    vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__pr_addr_a = 0U;
    vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__at_addr_a = 0U;
    vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__nt_addr_a = 0U;
    vlSelfRef.top__DOT__vgc_inst__DOT__in_text_area_d1 
        = vlSelfRef.top__DOT__vgc_inst__DOT__in_text_area;
    vlSelfRef.top__DOT__vgc_inst__DOT__gfx_x_d1 = (0x000001ffU 
                                                   & ((IData)(vlSelfRef.top__DOT__vgc_inst__DOT__h_count) 
                                                      >> 1U));
    top__DOT__vgc_inst__DOT__spr_pixel = 0U;
    top__DOT__vgc_inst__DOT__spr_pixel_pri = 0U;
    top__DOT__vgc_inst__DOT__spr_pixel_hit = 0U;
    if (((IData)(vlSelfRef.top__DOT__vgc_inst__DOT__in_text_area_d2) 
         & (0x0140U > (IData)(vlSelfRef.top__DOT__vgc_inst__DOT__gfx_x_d2)))) {
        if ((1U & (IData)(vlSelfRef.top__DOT__vgc_inst__DOT__sprite_inst__DOT__slb_b_dout))) {
            top__DOT__vgc_inst__DOT__spr_pixel = (0x0000000fU 
                                                  & ((IData)(vlSelfRef.top__DOT__vgc_inst__DOT__sprite_inst__DOT__slb_b_dout) 
                                                     >> 1U));
            top__DOT__vgc_inst__DOT__spr_pixel_pri 
                = (3U & ((IData)(vlSelfRef.top__DOT__vgc_inst__DOT__sprite_inst__DOT__slb_b_dout) 
                         >> 5U));
            top__DOT__vgc_inst__DOT__spr_pixel_hit = 1U;
        }
    }
    vlSelfRef.top__DOT__blt_inst__DOT__mem_read_data 
        = ((0U == (IData)(vlSelfRef.top__DOT__blt_inst__DOT__src_space))
            ? vlSelfRef.top__DOT__ram[vlSelfRef.top__DOT__blt_ram_addr]
            : ((5U == (IData)(vlSelfRef.top__DOT__blt_inst__DOT__src_space))
                ? vlSelfRef.top__DOT__xram[vlSelfRef.top__DOT__blt_xram_addr]
                : (IData)(vlSelfRef.top__DOT__vgc_inst__DOT__blt_rd_latch)));
    vlSelfRef.top__DOT__tile_dma_data = vlSelfRef.top__DOT__ram
        [vlSelfRef.top__DOT__tile_dma_addr];
    if (((IData)(vlSelfRef.top__DOT__cpu_we) & (0xa03fU 
                                                == (IData)(vlSelfRef.top__DOT__cpu_addr)))) {
        if ((4U == (IData)(vlSelfRef.top__DOT__cpu_dout))) {
            vlSelfRef.top__DOT__ext_rom_active = 1U;
        } else if ((2U == (IData)(vlSelfRef.top__DOT__cpu_dout))) {
            vlSelfRef.top__DOT__ext_rom_active = 0U;
        }
    }
    vlSelfRef.top__DOT__vgc_inst__DOT__sprite_inst__DOT__spr_b_addr 
        = __Vdly__top__DOT__vgc_inst__DOT__sprite_inst__DOT__spr_b_addr;
    vlSelfRef.top__DOT__vgc_inst__DOT__font_a_addr 
        = ((IData)(vlSelfRef.top__DOT__vgc_inst__DOT__artist_font_re)
            ? (IData)(vlSelfRef.top__DOT__vgc_inst__DOT__artist_font_addr)
            : 0U);
    vlSelfRef.top__DOT__vgc_inst__DOT__artist_inst__DOT__fs_b_addr 
        = __Vdly__top__DOT__vgc_inst__DOT__artist_inst__DOT__fs_b_addr;
    if (__VdlySet__top__DOT__vgc_inst__DOT__artist_inst__DOT__fill_stack_mem__DOT__mem__v0) {
        vlSelfRef.top__DOT__vgc_inst__DOT__artist_inst__DOT__fill_stack_mem__DOT__mem[__VdlyDim0__top__DOT__vgc_inst__DOT__artist_inst__DOT__fill_stack_mem__DOT__mem__v0] 
            = __VdlyVal__top__DOT__vgc_inst__DOT__artist_inst__DOT__fill_stack_mem__DOT__mem__v0;
    }
    vlSelfRef.top__DOT__vgc_inst__DOT__scroll_pending 
        = __Vdly__top__DOT__vgc_inst__DOT__scroll_pending;
    vlSelfRef.top__DOT__vgc_inst__DOT__scroll_col = __Vdly__top__DOT__vgc_inst__DOT__scroll_col;
    vlSelfRef.top__DOT__vgc_inst__DOT__cmd_busy = __Vdly__top__DOT__vgc_inst__DOT__cmd_busy;
    vlSelfRef.top__DOT__vgc_inst__DOT__copper_target_list 
        = __Vdly__top__DOT__vgc_inst__DOT__copper_target_list;
    vlSelfRef.top__DOT__vgc_inst__DOT__copper_pending_list 
        = __Vdly__top__DOT__vgc_inst__DOT__copper_pending_list;
    vlSelfRef.top__DOT__vgc_inst__DOT__copper_loading 
        = __Vdly__top__DOT__vgc_inst__DOT__copper_loading;
    vlSelfRef.top__DOT__vgc_inst__DOT__sprrow_count 
        = __Vdly__top__DOT__vgc_inst__DOT__sprrow_count;
    vlSelfRef.top__DOT__vgc_inst__DOT__sprcopy_phase 
        = __Vdly__top__DOT__vgc_inst__DOT__sprcopy_phase;
    vlSelfRef.top__DOT__vgc_inst__DOT__memread_pending 
        = __Vdly__top__DOT__vgc_inst__DOT__memread_pending;
    vlSelfRef.top__DOT__vgc_inst__DOT__cmd_cx = __Vdly__top__DOT__vgc_inst__DOT__cmd_cx;
    vlSelfRef.top__DOT__vgc_inst__DOT__cmd_cy = __Vdly__top__DOT__vgc_inst__DOT__cmd_cy;
    vlSelfRef.top__DOT__vgc_inst__DOT__copper_active_list 
        = __Vdly__top__DOT__vgc_inst__DOT__copper_active_list;
    vlSelfRef.top__DOT__vgc_inst__DOT__cmd_op = __Vdly__top__DOT__vgc_inst__DOT__cmd_op;
    vlSelfRef.top__DOT__vgc_inst__DOT__artist_busy 
        = __Vdly__top__DOT__vgc_inst__DOT__artist_busy;
    if (vlSelfRef.rst) {
        vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__tile_size16 = 0U;
        vlSelfRef.top__DOT__ext_rom_active = 0U;
    } else if (((IData)(vlSelfRef.top__DOT__cpu_we) 
                & (IData)(vlSelfRef.top__DOT__vgc_inst__DOT__tile_reg_sel))) {
        if ((0U == (0x0000001fU & (IData)(vlSelfRef.top__DOT__cpu_addr)))) {
            vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__tile_size16 
                = (1U & (IData)(vlSelfRef.top__DOT__cpu_dout));
        }
    }
    if ((0x00000010U & (IData)(vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__dma_state))) {
        if (vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__cmd_at_we) {
            vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__at_we_a = 1U;
            vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__at_addr_a 
                = vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__cmd_at_addr;
        }
        if (vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__cmd_pal_we) {
            vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__pr_we_a = 1U;
            vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__pr_addr_a 
                = vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__cmd_pal_addr_reg;
        }
        if (vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__cmd_nt_we) {
            vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__nt_we_a = 1U;
            vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__nt_addr_a 
                = vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__cmd_nt_addr;
        }
    } else if ((8U & (IData)(vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__dma_state))) {
        if ((4U & (IData)(vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__dma_state))) {
            if ((2U & (IData)(vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__dma_state))) {
                if ((1U & (IData)(vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__dma_state))) {
                    if (vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__cmd_at_we) {
                        vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__at_we_a = 1U;
                        vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__at_addr_a 
                            = vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__cmd_at_addr;
                    }
                    if (vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__cmd_pal_we) {
                        vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__pr_we_a = 1U;
                        vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__pr_addr_a 
                            = vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__cmd_pal_addr_reg;
                    }
                    if (vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__cmd_nt_we) {
                        vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__nt_we_a = 1U;
                        vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__nt_addr_a 
                            = vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__cmd_nt_addr;
                    }
                }
            } else {
                vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__at_addr_a 
                    = vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__cmd_at_addr;
                vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__nt_addr_a 
                    = vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__cmd_nt_addr;
            }
        } else {
            if ((1U & (~ ((IData)(vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__dma_state) 
                          >> 1U)))) {
                if ((1U & (IData)(vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__dma_state))) {
                    if ((0x0fa0U > (IData)(vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__dma_fill_addr))) {
                        vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__at_we_a = 1U;
                        vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__at_addr_a 
                            = vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__dma_fill_addr;
                    }
                } else if (vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__dma_data_valid) {
                    vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__at_we_a = 1U;
                    vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__at_addr_a 
                        = (0x00000fffU & ((((IData)(0x000003e8U) 
                                            * (IData)(vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__dma_nt)) 
                                           + ((IData)(0x0028U) 
                                              * (IData)(vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__dma_col_idx))) 
                                          + (IData)(vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__dma_col_row)));
                }
            }
            if ((2U & (IData)(vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__dma_state))) {
                if ((1U & (IData)(vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__dma_state))) {
                    if ((0x19U > (IData)(vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__dma_col_idx))) {
                        vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__nt_we_a = 1U;
                        vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__nt_addr_a 
                            = (0x00000fffU & ((((IData)(0x000003e8U) 
                                                * (IData)(vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__dma_nt)) 
                                               + ((IData)(0x0028U) 
                                                  * (IData)(vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__dma_col_idx))) 
                                              + (IData)(vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__dma_col_row)));
                    }
                } else if ((0x03e8U > (IData)(vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__dma_count))) {
                    vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__nt_we_a = 1U;
                    vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__nt_addr_a 
                        = (0x00000fffU & (((IData)(0x000003e8U) 
                                           * (IData)(vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__dma_nt)) 
                                          + (IData)(vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__dma_count)));
                }
            } else if ((1U & (IData)(vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__dma_state))) {
                if ((0x0fa0U > (IData)(vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__dma_fill_addr))) {
                    vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__nt_we_a = 1U;
                    vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__nt_addr_a 
                        = vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__dma_fill_addr;
                }
            }
        }
    } else if ((4U & (IData)(vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__dma_state))) {
        if ((2U & (IData)(vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__dma_state))) {
            if ((1U & (IData)(vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__dma_state))) {
                if (vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__dma_data_valid) {
                    vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__at_we_a = 1U;
                    vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__at_addr_a 
                        = (0x00000fffU & ((((IData)(0x000003e8U) 
                                            * (IData)(vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__dma_nt)) 
                                           + ((IData)(0x00000028U) 
                                              * (IData)(vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__dma_col_row))) 
                                          + (0x0000003fU 
                                             & (IData)(vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__dma_dst))));
                }
            }
            if ((1U & (~ (IData)(vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__dma_state)))) {
                if (vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__dma_data_valid) {
                    vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__nt_we_a = 1U;
                    vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__nt_addr_a 
                        = (0x00000fffU & ((((IData)(0x000003e8U) 
                                            * (IData)(vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__dma_nt)) 
                                           + ((IData)(0x0028U) 
                                              * (IData)(vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__dma_col_idx))) 
                                          + (IData)(vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__dma_col_row)));
                }
            }
        } else if ((1U & (IData)(vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__dma_state))) {
            if (vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__dma_data_valid) {
                vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__nt_we_a = 1U;
                vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__nt_addr_a 
                    = (0x00000fffU & ((((IData)(0x000003e8U) 
                                        * (IData)(vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__dma_nt)) 
                                       + ((IData)(0x00000028U) 
                                          * (IData)(vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__dma_col_row))) 
                                      + (0x0000003fU 
                                         & (IData)(vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__dma_dst))));
            }
        }
        if ((1U & (~ ((IData)(vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__dma_state) 
                      >> 1U)))) {
            if ((1U & (~ (IData)(vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__dma_state)))) {
                if (((IData)(vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__dma_data_valid) 
                     & (2U == (IData)(vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__dma_pal_phase)))) {
                    vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__pr_we_a = 1U;
                    vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__pr_addr_a 
                        = (((IData)(vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__dma_pal_sub) 
                            << 4U) | (IData)(vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__dma_pal_cidx));
                }
            }
        }
    } else {
        if ((2U & (IData)(vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__dma_state))) {
            if ((1U & (IData)(vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__dma_state))) {
                if (vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__dma_data_valid) {
                    vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__at_we_a = 1U;
                    vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__at_addr_a 
                        = (0x00000fffU & (((IData)(0x000003e8U) 
                                           * (IData)(vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__dma_nt)) 
                                          + (0x000003ffU 
                                             & (IData)(vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__dma_dst))));
                }
            }
            if ((1U & (~ (IData)(vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__dma_state)))) {
                if (vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__dma_data_valid) {
                    vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__nt_we_a = 1U;
                    vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__nt_addr_a 
                        = (0x00000fffU & (((IData)(0x000003e8U) 
                                           * (IData)(vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__dma_nt)) 
                                          + (0x000003ffU 
                                             & (IData)(vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__dma_dst))));
                }
            }
        } else if ((1U & (~ (IData)(vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__dma_state)))) {
            if (vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__cmd_at_we) {
                vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__at_we_a = 1U;
                vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__at_addr_a 
                    = vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__cmd_at_addr;
            }
            if (vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__cmd_nt_we) {
                vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__nt_we_a = 1U;
                vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__nt_addr_a 
                    = vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__cmd_nt_addr;
            }
        }
        if ((1U & (~ ((IData)(vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__dma_state) 
                      >> 1U)))) {
            if ((1U & (~ (IData)(vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__dma_state)))) {
                if (vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__cmd_pal_we) {
                    vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__pr_we_a = 1U;
                    vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__pr_addr_a 
                        = vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__cmd_pal_addr_reg;
                }
            }
        }
    }
    vlSelfRef.__VdlyCommitQueuetop__DOT__vgc_inst__DOT__copper_list_count.commit(vlSelfRef.top__DOT__vgc_inst__DOT__copper_list_count);
    if (((0x0cU == (IData)(vlSelfRef.top__DOT__cpu_inst__DOT__state)) 
         & (IData)(vlSelfRef.top__DOT____Vcellinp__cpu_inst__RDY))) {
        vlSelfRef.top__DOT__cpu_inst__DOT__sed = (0xf8U 
                                                  == (IData)(vlSelfRef.top__DOT__cpu_inst__DOT__IR));
        vlSelfRef.top__DOT__cpu_inst__DOT__cld = (0xd8U 
                                                  == (IData)(vlSelfRef.top__DOT__cpu_inst__DOT__IR));
        vlSelfRef.top__DOT__cpu_inst__DOT__plp = (0x28U 
                                                  == (IData)(vlSelfRef.top__DOT__cpu_inst__DOT__IR));
    }
    if (vlSelfRef.top__DOT____Vcellinp__cpu_inst__RDY) {
        vlSelfRef.top__DOT__cpu_inst__DOT__ADD = (0x000000ffU 
                                                  & (IData)(vlSelfRef.top__DOT__cpu_inst__DOT__ALU__DOT__temp));
    }
    if (vlSelfRef.rst) {
        __Vdly__top__DOT__vgc_inst__DOT__copper_index = 0U;
        vlSelfRef.top__DOT__vgc_inst__DOT__copper_fire = 0U;
    } else {
        vlSelfRef.top__DOT__vgc_inst__DOT__copper_fire = 0U;
        if (((0U == (IData)(vlSelfRef.top__DOT__vgc_inst__DOT__h_count)) 
             & (0x01e0U == (IData)(vlSelfRef.top__DOT__vgc_inst__DOT__v_count)))) {
            __Vdly__top__DOT__vgc_inst__DOT__copper_index = 0U;
        }
        if (((((IData)(vlSelfRef.top__DOT__vgc_inst__DOT__copper_enabled) 
               & (IData)(vlSelfRef.top__DOT__vgc_inst__DOT__in_text_area)) 
              & ((IData)(vlSelfRef.top__DOT__vgc_inst__DOT__copper_index) 
                 < (IData)(vlSelfRef.top__DOT__vgc_inst__DOT__copper_count))) 
             & ((0x0001ffffU & (((IData)(0x00000140U) 
                                 * (IData)(vlSelfRef.top__DOT__vgc_inst__DOT__gfx_y)) 
                                + (0x000001ffU & ((IData)(vlSelfRef.top__DOT__vgc_inst__DOT__h_count) 
                                                  >> 1U)))) 
                >= vlSelfRef.top__DOT__vgc_inst__DOT__copper_inst__DOT__copper_pos
                [(0x0000001fU & (IData)(vlSelfRef.top__DOT__vgc_inst__DOT__copper_index))]))) {
            vlSelfRef.top__DOT__vgc_inst__DOT__copper_fire = 1U;
            vlSelfRef.top__DOT__vgc_inst__DOT__copper_fire_reg 
                = vlSelfRef.top__DOT__vgc_inst__DOT__copper_inst__DOT__copper_reg
                [(0x0000001fU & (IData)(vlSelfRef.top__DOT__vgc_inst__DOT__copper_index))];
            vlSelfRef.top__DOT__vgc_inst__DOT__copper_fire_val 
                = vlSelfRef.top__DOT__vgc_inst__DOT__copper_inst__DOT__copper_val
                [(0x0000001fU & (IData)(vlSelfRef.top__DOT__vgc_inst__DOT__copper_index))];
            __Vdly__top__DOT__vgc_inst__DOT__copper_index 
                = (0x000001ffU & ((IData)(1U) + (IData)(vlSelfRef.top__DOT__vgc_inst__DOT__copper_index)));
        }
    }
    vlSelfRef.top__DOT__vgc_inst__DOT__tile_blt_rdata 
        = ((IData)(vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__blt_tile_rd_pending)
            ? (IData)(vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__td_dout_a)
            : (IData)(vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__blt_tile_rd_latch));
    if (__VdlySet__top__DOT__vgc_inst__DOT__sprrow_data__v0) {
        vlSelfRef.top__DOT__vgc_inst__DOT__sprrow_data[0U] 
            = __VdlyVal__top__DOT__vgc_inst__DOT__sprrow_data__v0;
        vlSelfRef.top__DOT__vgc_inst__DOT__sprrow_data[1U] 
            = __VdlyVal__top__DOT__vgc_inst__DOT__sprrow_data__v1;
        vlSelfRef.top__DOT__vgc_inst__DOT__sprrow_data[2U] 
            = __VdlyVal__top__DOT__vgc_inst__DOT__sprrow_data__v2;
        vlSelfRef.top__DOT__vgc_inst__DOT__sprrow_data[3U] 
            = __VdlyVal__top__DOT__vgc_inst__DOT__sprrow_data__v3;
        vlSelfRef.top__DOT__vgc_inst__DOT__sprrow_data[4U] 
            = __VdlyVal__top__DOT__vgc_inst__DOT__sprrow_data__v4;
        vlSelfRef.top__DOT__vgc_inst__DOT__sprrow_data[5U] 
            = __VdlyVal__top__DOT__vgc_inst__DOT__sprrow_data__v5;
        vlSelfRef.top__DOT__vgc_inst__DOT__sprrow_data[6U] 
            = __VdlyVal__top__DOT__vgc_inst__DOT__sprrow_data__v6;
        vlSelfRef.top__DOT__vgc_inst__DOT__sprrow_data[7U] 
            = __VdlyVal__top__DOT__vgc_inst__DOT__sprrow_data__v7;
    }
    if (__VdlySet__top__DOT__vgc_inst__DOT__copper_list_pos__v0) {
        vlSelfRef.top__DOT__vgc_inst__DOT__copper_list_pos[__VdlyDim0__top__DOT__vgc_inst__DOT__copper_list_pos__v0] 
            = __VdlyVal__top__DOT__vgc_inst__DOT__copper_list_pos__v0;
        vlSelfRef.top__DOT__vgc_inst__DOT__copper_list_reg[__VdlyDim0__top__DOT__vgc_inst__DOT__copper_list_reg__v0] 
            = __VdlyVal__top__DOT__vgc_inst__DOT__copper_list_reg__v0;
        vlSelfRef.top__DOT__vgc_inst__DOT__copper_list_val[__VdlyDim0__top__DOT__vgc_inst__DOT__copper_list_val__v0] 
            = __VdlyVal__top__DOT__vgc_inst__DOT__copper_list_val__v0;
    }
    vlSelfRef.top__DOT__vgc_inst__DOT__scroll_offset 
        = __Vdly__top__DOT__vgc_inst__DOT__scroll_offset;
    vlSelfRef.top__DOT__vgc_inst__DOT__fg_color = __Vdly__top__DOT__vgc_inst__DOT__fg_color;
    vlSelfRef.top__DOT__vgc_inst__DOT__cursor_x = __Vdly__top__DOT__vgc_inst__DOT__cursor_x;
    vlSelfRef.top__DOT__vgc_inst__DOT__cursor_y = __Vdly__top__DOT__vgc_inst__DOT__cursor_y;
    if (__VdlySet__top__DOT__vgc_inst__DOT__copper_pos__v0) {
        vlSelfRef.top__DOT__vgc_inst__DOT__copper_reg[__VdlyDim0__top__DOT__vgc_inst__DOT__copper_reg__v0] 
            = __VdlyVal__top__DOT__vgc_inst__DOT__copper_reg__v0;
        vlSelfRef.top__DOT__vgc_inst__DOT__copper_val[__VdlyDim0__top__DOT__vgc_inst__DOT__copper_val__v0] 
            = __VdlyVal__top__DOT__vgc_inst__DOT__copper_val__v0;
        vlSelfRef.top__DOT__vgc_inst__DOT__copper_pos[__VdlyDim0__top__DOT__vgc_inst__DOT__copper_pos__v0] 
            = __VdlyVal__top__DOT__vgc_inst__DOT__copper_pos__v0;
    }
    if (__VdlySet__top__DOT__vgc_inst__DOT__copper_pos__v1) {
        vlSelfRef.top__DOT__vgc_inst__DOT__copper_reg[__VdlyDim0__top__DOT__vgc_inst__DOT__copper_reg__v1] 
            = __VdlyVal__top__DOT__vgc_inst__DOT__copper_reg__v1;
        vlSelfRef.top__DOT__vgc_inst__DOT__copper_val[__VdlyDim0__top__DOT__vgc_inst__DOT__copper_val__v1] 
            = __VdlyVal__top__DOT__vgc_inst__DOT__copper_val__v1;
        vlSelfRef.top__DOT__vgc_inst__DOT__copper_pos[__VdlyDim0__top__DOT__vgc_inst__DOT__copper_pos__v1] 
            = __VdlyVal__top__DOT__vgc_inst__DOT__copper_pos__v1;
    }
    if (__VdlySet__top__DOT__vgc_inst__DOT__regs__v0) {
        vlSelfRef.top__DOT__vgc_inst__DOT__spr_y[0U] = 0U;
        vlSelfRef.top__DOT__vgc_inst__DOT__spr_y[1U] = 0U;
        vlSelfRef.top__DOT__vgc_inst__DOT__spr_y[2U] = 0U;
        vlSelfRef.top__DOT__vgc_inst__DOT__spr_y[3U] = 0U;
        vlSelfRef.top__DOT__vgc_inst__DOT__spr_y[4U] = 0U;
        vlSelfRef.top__DOT__vgc_inst__DOT__spr_y[5U] = 0U;
        vlSelfRef.top__DOT__vgc_inst__DOT__spr_y[6U] = 0U;
        vlSelfRef.top__DOT__vgc_inst__DOT__spr_y[7U] = 0U;
        vlSelfRef.top__DOT__vgc_inst__DOT__spr_y[8U] = 0U;
        vlSelfRef.top__DOT__vgc_inst__DOT__spr_y[9U] = 0U;
        vlSelfRef.top__DOT__vgc_inst__DOT__spr_y[10U] = 0U;
        vlSelfRef.top__DOT__vgc_inst__DOT__spr_y[11U] = 0U;
        vlSelfRef.top__DOT__vgc_inst__DOT__spr_y[12U] = 0U;
        vlSelfRef.top__DOT__vgc_inst__DOT__spr_y[13U] = 0U;
        vlSelfRef.top__DOT__vgc_inst__DOT__spr_y[14U] = 0U;
        vlSelfRef.top__DOT__vgc_inst__DOT__spr_y[15U] = 0U;
        vlSelfRef.top__DOT__vgc_inst__DOT__spr_flip_h[0U] = 0U;
        vlSelfRef.top__DOT__vgc_inst__DOT__spr_flip_h[1U] = 0U;
        vlSelfRef.top__DOT__vgc_inst__DOT__spr_flip_h[2U] = 0U;
        vlSelfRef.top__DOT__vgc_inst__DOT__spr_flip_h[3U] = 0U;
        vlSelfRef.top__DOT__vgc_inst__DOT__spr_flip_h[4U] = 0U;
        vlSelfRef.top__DOT__vgc_inst__DOT__spr_flip_h[5U] = 0U;
        vlSelfRef.top__DOT__vgc_inst__DOT__spr_flip_h[6U] = 0U;
        vlSelfRef.top__DOT__vgc_inst__DOT__spr_flip_h[7U] = 0U;
        vlSelfRef.top__DOT__vgc_inst__DOT__spr_flip_h[8U] = 0U;
        vlSelfRef.top__DOT__vgc_inst__DOT__spr_flip_h[9U] = 0U;
        vlSelfRef.top__DOT__vgc_inst__DOT__spr_flip_h[10U] = 0U;
        vlSelfRef.top__DOT__vgc_inst__DOT__spr_flip_h[11U] = 0U;
        vlSelfRef.top__DOT__vgc_inst__DOT__spr_flip_h[12U] = 0U;
        vlSelfRef.top__DOT__vgc_inst__DOT__spr_flip_h[13U] = 0U;
        vlSelfRef.top__DOT__vgc_inst__DOT__spr_flip_h[14U] = 0U;
        vlSelfRef.top__DOT__vgc_inst__DOT__spr_flip_h[15U] = 0U;
        vlSelfRef.top__DOT__vgc_inst__DOT__spr_flip_v[0U] = 0U;
        vlSelfRef.top__DOT__vgc_inst__DOT__spr_flip_v[1U] = 0U;
        vlSelfRef.top__DOT__vgc_inst__DOT__spr_flip_v[2U] = 0U;
        vlSelfRef.top__DOT__vgc_inst__DOT__spr_flip_v[3U] = 0U;
        vlSelfRef.top__DOT__vgc_inst__DOT__spr_flip_v[4U] = 0U;
        vlSelfRef.top__DOT__vgc_inst__DOT__spr_flip_v[5U] = 0U;
        vlSelfRef.top__DOT__vgc_inst__DOT__spr_flip_v[6U] = 0U;
        vlSelfRef.top__DOT__vgc_inst__DOT__spr_flip_v[7U] = 0U;
        vlSelfRef.top__DOT__vgc_inst__DOT__spr_flip_v[8U] = 0U;
        vlSelfRef.top__DOT__vgc_inst__DOT__spr_flip_v[9U] = 0U;
        vlSelfRef.top__DOT__vgc_inst__DOT__spr_flip_v[10U] = 0U;
        vlSelfRef.top__DOT__vgc_inst__DOT__spr_flip_v[11U] = 0U;
        vlSelfRef.top__DOT__vgc_inst__DOT__spr_flip_v[12U] = 0U;
        vlSelfRef.top__DOT__vgc_inst__DOT__spr_flip_v[13U] = 0U;
        vlSelfRef.top__DOT__vgc_inst__DOT__spr_flip_v[14U] = 0U;
        vlSelfRef.top__DOT__vgc_inst__DOT__spr_flip_v[15U] = 0U;
        vlSelfRef.top__DOT__vgc_inst__DOT__spr_pri[0U] = 0U;
        vlSelfRef.top__DOT__vgc_inst__DOT__spr_pri[1U] = 0U;
        vlSelfRef.top__DOT__vgc_inst__DOT__spr_pri[2U] = 0U;
        vlSelfRef.top__DOT__vgc_inst__DOT__spr_pri[3U] = 0U;
        vlSelfRef.top__DOT__vgc_inst__DOT__spr_pri[4U] = 0U;
        vlSelfRef.top__DOT__vgc_inst__DOT__spr_pri[5U] = 0U;
        vlSelfRef.top__DOT__vgc_inst__DOT__spr_pri[6U] = 0U;
        vlSelfRef.top__DOT__vgc_inst__DOT__spr_pri[7U] = 0U;
        vlSelfRef.top__DOT__vgc_inst__DOT__spr_pri[8U] = 0U;
        vlSelfRef.top__DOT__vgc_inst__DOT__spr_pri[9U] = 0U;
        vlSelfRef.top__DOT__vgc_inst__DOT__spr_pri[10U] = 0U;
        vlSelfRef.top__DOT__vgc_inst__DOT__spr_pri[11U] = 0U;
        vlSelfRef.top__DOT__vgc_inst__DOT__spr_pri[12U] = 0U;
        vlSelfRef.top__DOT__vgc_inst__DOT__spr_pri[13U] = 0U;
        vlSelfRef.top__DOT__vgc_inst__DOT__spr_pri[14U] = 0U;
        vlSelfRef.top__DOT__vgc_inst__DOT__spr_pri[15U] = 0U;
        vlSelfRef.top__DOT__vgc_inst__DOT__spr_enable[0U] = 0U;
        vlSelfRef.top__DOT__vgc_inst__DOT__spr_enable[1U] = 0U;
        vlSelfRef.top__DOT__vgc_inst__DOT__spr_enable[2U] = 0U;
        vlSelfRef.top__DOT__vgc_inst__DOT__spr_enable[3U] = 0U;
        vlSelfRef.top__DOT__vgc_inst__DOT__spr_enable[4U] = 0U;
        vlSelfRef.top__DOT__vgc_inst__DOT__spr_enable[5U] = 0U;
        vlSelfRef.top__DOT__vgc_inst__DOT__spr_enable[6U] = 0U;
        vlSelfRef.top__DOT__vgc_inst__DOT__spr_enable[7U] = 0U;
        vlSelfRef.top__DOT__vgc_inst__DOT__spr_enable[8U] = 0U;
        vlSelfRef.top__DOT__vgc_inst__DOT__spr_enable[9U] = 0U;
        vlSelfRef.top__DOT__vgc_inst__DOT__spr_enable[10U] = 0U;
        vlSelfRef.top__DOT__vgc_inst__DOT__spr_enable[11U] = 0U;
        vlSelfRef.top__DOT__vgc_inst__DOT__spr_enable[12U] = 0U;
        vlSelfRef.top__DOT__vgc_inst__DOT__spr_enable[13U] = 0U;
        vlSelfRef.top__DOT__vgc_inst__DOT__spr_enable[14U] = 0U;
        vlSelfRef.top__DOT__vgc_inst__DOT__spr_enable[15U] = 0U;
        vlSelfRef.top__DOT__vgc_inst__DOT__spr_x[0U] = 0U;
        vlSelfRef.top__DOT__vgc_inst__DOT__spr_x[1U] = 0U;
        vlSelfRef.top__DOT__vgc_inst__DOT__spr_x[2U] = 0U;
        vlSelfRef.top__DOT__vgc_inst__DOT__spr_x[3U] = 0U;
        vlSelfRef.top__DOT__vgc_inst__DOT__spr_x[4U] = 0U;
        vlSelfRef.top__DOT__vgc_inst__DOT__spr_x[5U] = 0U;
        vlSelfRef.top__DOT__vgc_inst__DOT__spr_x[6U] = 0U;
        vlSelfRef.top__DOT__vgc_inst__DOT__spr_x[7U] = 0U;
        vlSelfRef.top__DOT__vgc_inst__DOT__spr_x[8U] = 0U;
        vlSelfRef.top__DOT__vgc_inst__DOT__spr_x[9U] = 0U;
        vlSelfRef.top__DOT__vgc_inst__DOT__spr_x[10U] = 0U;
        vlSelfRef.top__DOT__vgc_inst__DOT__spr_x[11U] = 0U;
        vlSelfRef.top__DOT__vgc_inst__DOT__spr_x[12U] = 0U;
        vlSelfRef.top__DOT__vgc_inst__DOT__spr_x[13U] = 0U;
        vlSelfRef.top__DOT__vgc_inst__DOT__spr_x[14U] = 0U;
        vlSelfRef.top__DOT__vgc_inst__DOT__spr_x[15U] = 0U;
        vlSelfRef.top__DOT__vgc_inst__DOT__regs[0U] = 0U;
        vlSelfRef.top__DOT__vgc_inst__DOT__regs[1U] = 0U;
        vlSelfRef.top__DOT__vgc_inst__DOT__regs[2U] = 0U;
        vlSelfRef.top__DOT__vgc_inst__DOT__regs[3U] = 0U;
        vlSelfRef.top__DOT__vgc_inst__DOT__regs[4U] = 0U;
        vlSelfRef.top__DOT__vgc_inst__DOT__regs[5U] = 0U;
        vlSelfRef.top__DOT__vgc_inst__DOT__regs[6U] = 0U;
        vlSelfRef.top__DOT__vgc_inst__DOT__regs[7U] = 0U;
        vlSelfRef.top__DOT__vgc_inst__DOT__regs[8U] = 0U;
        vlSelfRef.top__DOT__vgc_inst__DOT__regs[9U] = 0U;
        vlSelfRef.top__DOT__vgc_inst__DOT__regs[10U] = 0U;
        vlSelfRef.top__DOT__vgc_inst__DOT__regs[11U] = 0U;
        vlSelfRef.top__DOT__vgc_inst__DOT__regs[12U] = 0U;
        vlSelfRef.top__DOT__vgc_inst__DOT__regs[13U] = 0U;
        vlSelfRef.top__DOT__vgc_inst__DOT__regs[14U] = 0U;
        vlSelfRef.top__DOT__vgc_inst__DOT__regs[15U] = 0U;
        vlSelfRef.top__DOT__vgc_inst__DOT__regs[16U] = 0U;
        vlSelfRef.top__DOT__vgc_inst__DOT__regs[17U] = 0U;
        vlSelfRef.top__DOT__vgc_inst__DOT__regs[18U] = 0U;
        vlSelfRef.top__DOT__vgc_inst__DOT__regs[19U] = 0U;
        vlSelfRef.top__DOT__vgc_inst__DOT__regs[20U] = 0U;
        vlSelfRef.top__DOT__vgc_inst__DOT__regs[21U] = 0U;
        vlSelfRef.top__DOT__vgc_inst__DOT__regs[22U] = 0U;
        vlSelfRef.top__DOT__vgc_inst__DOT__regs[23U] = 0U;
        vlSelfRef.top__DOT__vgc_inst__DOT__regs[24U] = 0U;
        vlSelfRef.top__DOT__vgc_inst__DOT__regs[25U] = 0U;
        vlSelfRef.top__DOT__vgc_inst__DOT__regs[26U] = 0U;
        vlSelfRef.top__DOT__vgc_inst__DOT__regs[27U] = 0U;
        vlSelfRef.top__DOT__vgc_inst__DOT__regs[28U] = 0U;
        vlSelfRef.top__DOT__vgc_inst__DOT__regs[29U] = 0U;
        vlSelfRef.top__DOT__vgc_inst__DOT__regs[30U] = 0U;
        vlSelfRef.top__DOT__vgc_inst__DOT__regs[31U] = 0U;
    }
    if (__VdlySet__top__DOT__vgc_inst__DOT__spr_x__v16) {
        vlSelfRef.top__DOT__vgc_inst__DOT__spr_y[__VdlyDim0__top__DOT__vgc_inst__DOT__spr_y__v16] 
            = __VdlyVal__top__DOT__vgc_inst__DOT__spr_y__v16;
        vlSelfRef.top__DOT__vgc_inst__DOT__spr_x[__VdlyDim0__top__DOT__vgc_inst__DOT__spr_x__v16] 
            = __VdlyVal__top__DOT__vgc_inst__DOT__spr_x__v16;
    }
    if (__VdlySet__top__DOT__vgc_inst__DOT__spr_y__v17) {
        vlSelfRef.top__DOT__vgc_inst__DOT__spr_y[__VdlyDim0__top__DOT__vgc_inst__DOT__spr_y__v17] 
            = __VdlyVal__top__DOT__vgc_inst__DOT__spr_y__v17;
    }
    if (__VdlySet__top__DOT__vgc_inst__DOT__spr_flip_h__v16) {
        vlSelfRef.top__DOT__vgc_inst__DOT__spr_flip_h[__VdlyDim0__top__DOT__vgc_inst__DOT__spr_flip_h__v16] 
            = __VdlyVal__top__DOT__vgc_inst__DOT__spr_flip_h__v16;
        vlSelfRef.top__DOT__vgc_inst__DOT__spr_flip_v[__VdlyDim0__top__DOT__vgc_inst__DOT__spr_flip_v__v16] 
            = __VdlyVal__top__DOT__vgc_inst__DOT__spr_flip_v__v16;
    }
    if (__VdlySet__top__DOT__vgc_inst__DOT__spr_pri__v16) {
        vlSelfRef.top__DOT__vgc_inst__DOT__spr_pri[__VdlyDim0__top__DOT__vgc_inst__DOT__spr_pri__v16] 
            = __VdlyVal__top__DOT__vgc_inst__DOT__spr_pri__v16;
    }
    if (__VdlySet__top__DOT__vgc_inst__DOT__spr_pri__v17) {
        vlSelfRef.top__DOT__vgc_inst__DOT__spr_pri[__VdlyDim0__top__DOT__vgc_inst__DOT__spr_pri__v17] 
            = __VdlyVal__top__DOT__vgc_inst__DOT__spr_pri__v17;
    }
    if (__VdlySet__top__DOT__vgc_inst__DOT__spr_enable__v16) {
        vlSelfRef.top__DOT__vgc_inst__DOT__spr_enable[__VdlyDim0__top__DOT__vgc_inst__DOT__spr_enable__v16] = 0U;
    }
    if (__VdlySet__top__DOT__vgc_inst__DOT__spr_enable__v17) {
        vlSelfRef.top__DOT__vgc_inst__DOT__spr_enable[__VdlyDim0__top__DOT__vgc_inst__DOT__spr_enable__v17] = 1U;
    }
    if (__VdlySet__top__DOT__vgc_inst__DOT__spr_flip_h__v17) {
        vlSelfRef.top__DOT__vgc_inst__DOT__spr_flip_h[__VdlyDim0__top__DOT__vgc_inst__DOT__spr_flip_h__v17] 
            = __VdlyVal__top__DOT__vgc_inst__DOT__spr_flip_h__v17;
        vlSelfRef.top__DOT__vgc_inst__DOT__spr_flip_v[__VdlyDim0__top__DOT__vgc_inst__DOT__spr_flip_v__v17] 
            = __VdlyVal__top__DOT__vgc_inst__DOT__spr_flip_v__v17;
        vlSelfRef.top__DOT__vgc_inst__DOT__spr_enable[__VdlyDim0__top__DOT__vgc_inst__DOT__spr_enable__v18] 
            = __VdlyVal__top__DOT__vgc_inst__DOT__spr_enable__v18;
    }
    if (__VdlySet__top__DOT__vgc_inst__DOT__spr_x__v17) {
        vlSelfRef.top__DOT__vgc_inst__DOT__spr_x[__VdlyDim0__top__DOT__vgc_inst__DOT__spr_x__v17] 
            = ((0x00ffU & vlSelfRef.top__DOT__vgc_inst__DOT__spr_x
                [__VdlyDim0__top__DOT__vgc_inst__DOT__spr_x__v17]) 
               | ((IData)(__VdlyVal__top__DOT__vgc_inst__DOT__spr_x__v17) 
                  << 8U));
    }
    if (__VdlySet__top__DOT__vgc_inst__DOT__spr_x__v18) {
        vlSelfRef.top__DOT__vgc_inst__DOT__spr_x[__VdlyDim0__top__DOT__vgc_inst__DOT__spr_x__v18] 
            = ((0x0100U & vlSelfRef.top__DOT__vgc_inst__DOT__spr_x
                [__VdlyDim0__top__DOT__vgc_inst__DOT__spr_x__v18]) 
               | (IData)(__VdlyVal__top__DOT__vgc_inst__DOT__spr_x__v18));
    }
    if (__VdlySet__top__DOT__vgc_inst__DOT__regs__v32) {
        vlSelfRef.top__DOT__vgc_inst__DOT__regs[20U] 
            = __VdlyVal__top__DOT__vgc_inst__DOT__regs__v32;
    }
    if (__VdlySet__top__DOT__vgc_inst__DOT__regs__v33) {
        vlSelfRef.top__DOT__vgc_inst__DOT__regs[20U] 
            = __VdlyVal__top__DOT__vgc_inst__DOT__regs__v33;
    }
    if (__VdlySet__top__DOT__vgc_inst__DOT__regs__v34) {
        vlSelfRef.top__DOT__vgc_inst__DOT__regs[__VdlyDim0__top__DOT__vgc_inst__DOT__regs__v34] 
            = __VdlyVal__top__DOT__vgc_inst__DOT__regs__v34;
    }
    if (__VdlySet__top__DOT__vgc_inst__DOT__regs__v35) {
        vlSelfRef.top__DOT__vgc_inst__DOT__regs[__VdlyDim0__top__DOT__vgc_inst__DOT__regs__v35] 
            = __VdlyVal__top__DOT__vgc_inst__DOT__regs__v35;
    }
    if (__VdlySet__top__DOT__vgc_inst__DOT__tile_inst__DOT__pal_ram_inst__DOT__mem__v0) {
        vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__pal_ram_inst__DOT__mem[__VdlyDim0__top__DOT__vgc_inst__DOT__tile_inst__DOT__pal_ram_inst__DOT__mem__v0] 
            = __VdlyVal__top__DOT__vgc_inst__DOT__tile_inst__DOT__pal_ram_inst__DOT__mem__v0;
    }
    vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__s2_color_idx 
        = (0x0000000fU & ((IData)(vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__s2_sample_x_lsb)
                           ? (IData)(vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__td_dout_b)
                           : ((IData)(vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__td_dout_b) 
                              >> 4U)));
    vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__at_din_a = 0U;
    vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__pr_din_a = 0U;
    vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__nt_din_a = 0U;
    vlSelfRef.top__DOT__vgc_inst__DOT__spr_a_we = 0U;
    vlSelfRef.top__DOT__vgc_inst__DOT__spr_a_din = 0U;
    vlSelfRef.top__DOT__vgc_inst__DOT__char_a_we = 0U;
    vlSelfRef.top__DOT__vgc_inst__DOT__char_a_din = 0U;
    vlSelfRef.top__DOT__vgc_inst__DOT__color_a_we = 0U;
    vlSelfRef.top__DOT__vgc_inst__DOT__color_a_din = 0U;
    vlSelfRef.top__DOT__vgc_inst__DOT__tile_blt_re 
        = ((IData)(vlSelfRef.top__DOT__vgc_inst__DOT__vgc_tile_re) 
           | ((IData)(vlSelfRef.top__DOT__blt_vgc_re) 
              & (6U == (IData)(vlSelfRef.top__DOT__blt_vgc_space))));
    vlSelfRef.top__DOT__vgc_inst__DOT__gfx_a_we = 0U;
    vlSelfRef.top__DOT__vgc_inst__DOT__gfx_a_din = 0U;
    vlSelfRef.top__DOT__vgc_inst__DOT__spr_a_dout = vlSelfRef.top__DOT__vgc_inst__DOT__sprite_inst__DOT__spr_mem__DOT__mem
        [vlSelfRef.top__DOT__vgc_inst__DOT__spr_a_addr];
    vlSelfRef.top__DOT__vgc_inst__DOT__color_a_dout 
        = ((0x07cfU >= (IData)(vlSelfRef.top__DOT__vgc_inst__DOT__color_a_addr))
            ? vlSelfRef.top__DOT__vgc_inst__DOT__text_inst__DOT__color_mem__DOT__mem
           [vlSelfRef.top__DOT__vgc_inst__DOT__color_a_addr]
            : 0U);
    if (((IData)(vlSelfRef.top__DOT__blt_vgc_we) & 
         (2U == (IData)(vlSelfRef.top__DOT__blt_vgc_space)))) {
        vlSelfRef.top__DOT__vgc_inst__DOT__color_a_we = 1U;
        vlSelfRef.top__DOT__vgc_inst__DOT__color_a_din 
            = top__DOT__blt_vgc_wdata;
        vlSelfRef.top__DOT__vgc_inst__DOT__color_a_addr 
            = (0x000007ffU & (IData)(vlSelfRef.top__DOT__blt_vgc_addr));
    } else if (vlSelfRef.top__DOT__vgc_inst__DOT__cmd_color_we) {
        vlSelfRef.top__DOT__vgc_inst__DOT__color_a_we = 1U;
        vlSelfRef.top__DOT__vgc_inst__DOT__color_a_din 
            = vlSelfRef.top__DOT__vgc_inst__DOT__cmd_color_din;
        vlSelfRef.top__DOT__vgc_inst__DOT__color_a_addr 
            = (0x000007ffU & (IData)(vlSelfRef.top__DOT__vgc_inst__DOT__cmd_color_addr));
    } else {
        vlSelfRef.top__DOT__vgc_inst__DOT__color_a_addr 
            = (0x000007ffU & (((IData)(vlSelfRef.top__DOT__blt_vgc_re) 
                               & (2U == (IData)(vlSelfRef.top__DOT__blt_vgc_space)))
                               ? (IData)(vlSelfRef.top__DOT__blt_vgc_addr)
                               : ((IData)(vlSelfRef.top__DOT__vgc_inst__DOT__dbg_color_sel)
                                   ? ((IData)(vlSelfRef.dbg_peek_addr) 
                                      - (IData)(0x01d0U))
                                   : (IData)(vlSelfRef.top__DOT__vgc_inst__DOT__cmd_color_addr))));
    }
    vlSelfRef.top__DOT__vgc_inst__DOT__char_a_dout 
        = ((0x07cfU >= (IData)(vlSelfRef.top__DOT__vgc_inst__DOT__char_a_addr))
            ? vlSelfRef.top__DOT__vgc_inst__DOT__text_inst__DOT__char_mem__DOT__mem
           [vlSelfRef.top__DOT__vgc_inst__DOT__char_a_addr]
            : 0U);
    if (((IData)(vlSelfRef.top__DOT__blt_vgc_we) & 
         (1U == (IData)(vlSelfRef.top__DOT__blt_vgc_space)))) {
        vlSelfRef.top__DOT__vgc_inst__DOT__char_a_we = 1U;
        vlSelfRef.top__DOT__vgc_inst__DOT__char_a_din 
            = top__DOT__blt_vgc_wdata;
        vlSelfRef.top__DOT__vgc_inst__DOT__char_a_addr 
            = (0x000007ffU & (IData)(vlSelfRef.top__DOT__blt_vgc_addr));
    } else if (vlSelfRef.top__DOT__vgc_inst__DOT__cmd_char_we) {
        vlSelfRef.top__DOT__vgc_inst__DOT__char_a_we = 1U;
        vlSelfRef.top__DOT__vgc_inst__DOT__char_a_din 
            = vlSelfRef.top__DOT__vgc_inst__DOT__cmd_char_din;
        vlSelfRef.top__DOT__vgc_inst__DOT__char_a_addr 
            = (0x000007ffU & (IData)(vlSelfRef.top__DOT__vgc_inst__DOT__cmd_char_addr));
    } else {
        vlSelfRef.top__DOT__vgc_inst__DOT__char_a_addr 
            = (0x000007ffU & (((IData)(vlSelfRef.top__DOT__blt_vgc_re) 
                               & (1U == (IData)(vlSelfRef.top__DOT__blt_vgc_space)))
                               ? (IData)(vlSelfRef.top__DOT__blt_vgc_addr)
                               : ((IData)(vlSelfRef.top__DOT__vgc_inst__DOT__dbg_char_sel)
                                   ? ((IData)(vlSelfRef.dbg_peek_addr) 
                                      - (IData)(0x0200U))
                                   : (IData)(vlSelfRef.top__DOT__vgc_inst__DOT__cmd_char_addr))));
    }
    vlSelfRef.top__DOT__vgc_inst__DOT__gfx_a_dout = 
        ((0xf9ffU >= (IData)(vlSelfRef.top__DOT__vgc_inst__DOT__gfx_a_addr))
          ? vlSelfRef.top__DOT__vgc_inst__DOT__gfx_inst__DOT__gfx_mem__DOT__mem
         [vlSelfRef.top__DOT__vgc_inst__DOT__gfx_a_addr]
          : 0U);
    vlSelfRef.top__DOT__vgc_inst__DOT__cursor_here 
        = ((IData)(vlSelfRef.top__DOT__vgc_inst__DOT__cursor_enable) 
           & ((IData)(vlSelfRef.top__DOT__vgc_inst__DOT__in_text_area_d2) 
              & (((IData)(vlSelfRef.top__DOT__vgc_inst__DOT__cursor_x) 
                  == (IData)(vlSelfRef.top__DOT__vgc_inst__DOT__text_col_d2)) 
                 & ((IData)(vlSelfRef.top__DOT__vgc_inst__DOT__cursor_y) 
                    == (IData)(vlSelfRef.top__DOT__vgc_inst__DOT__text_row_d2)))));
    vlSelfRef.top__DOT__vgc_inst__DOT__copper_reg_flat[0U] 
        = ((0xff000000U & vlSelfRef.top__DOT__vgc_inst__DOT__copper_reg_flat[0U]) 
           | ((vlSelfRef.top__DOT__vgc_inst__DOT__copper_reg[2U] 
               << 0x00000010U) | ((vlSelfRef.top__DOT__vgc_inst__DOT__copper_reg[1U] 
                                   << 8U) | vlSelfRef.top__DOT__vgc_inst__DOT__copper_reg[0U])));
    vlSelfRef.top__DOT__vgc_inst__DOT__copper_reg_flat[0U] 
        = ((0x00ffffffU & vlSelfRef.top__DOT__vgc_inst__DOT__copper_reg_flat[0U]) 
           | (vlSelfRef.top__DOT__vgc_inst__DOT__copper_reg[3U] 
              << 0x00000018U));
    vlSelfRef.top__DOT__vgc_inst__DOT__copper_reg_flat[1U] 
        = ((0xffff0000U & vlSelfRef.top__DOT__vgc_inst__DOT__copper_reg_flat[1U]) 
           | (0x00ffffffU & ((0x00ffff00U & (vlSelfRef.top__DOT__vgc_inst__DOT__copper_reg[5U] 
                                             << 8U)) 
                             | ((0x00ffffffU & vlSelfRef.top__DOT__vgc_inst__DOT__copper_reg[4U]) 
                                | (vlSelfRef.top__DOT__vgc_inst__DOT__copper_reg[3U] 
                                   >> 8U)))));
    vlSelfRef.top__DOT__vgc_inst__DOT__copper_reg_flat[1U] 
        = ((0x0000ffffU & vlSelfRef.top__DOT__vgc_inst__DOT__copper_reg_flat[1U]) 
           | (0xffff0000U & ((vlSelfRef.top__DOT__vgc_inst__DOT__copper_reg[7U] 
                              << 0x00000018U) | (vlSelfRef.top__DOT__vgc_inst__DOT__copper_reg[6U] 
                                                 << 0x00000010U))));
    vlSelfRef.top__DOT__vgc_inst__DOT__copper_reg_flat[2U] 
        = ((0xffffff00U & vlSelfRef.top__DOT__vgc_inst__DOT__copper_reg_flat[2U]) 
           | (0x0000ffffU & ((0x0000ffffU & vlSelfRef.top__DOT__vgc_inst__DOT__copper_reg[8U]) 
                             | ((0x0000ffffU & (vlSelfRef.top__DOT__vgc_inst__DOT__copper_reg[7U] 
                                                >> 8U)) 
                                | (vlSelfRef.top__DOT__vgc_inst__DOT__copper_reg[6U] 
                                   >> 0x00000010U)))));
    vlSelfRef.top__DOT__vgc_inst__DOT__copper_reg_flat[2U] 
        = ((0x000000ffU & vlSelfRef.top__DOT__vgc_inst__DOT__copper_reg_flat[2U]) 
           | (0xffffff00U & ((vlSelfRef.top__DOT__vgc_inst__DOT__copper_reg[11U] 
                              << 0x00000018U) | ((vlSelfRef.top__DOT__vgc_inst__DOT__copper_reg[10U] 
                                                  << 0x00000010U) 
                                                 | (vlSelfRef.top__DOT__vgc_inst__DOT__copper_reg[9U] 
                                                    << 8U)))));
    vlSelfRef.top__DOT__vgc_inst__DOT__copper_reg_flat[3U] 
        = ((0xff000000U & vlSelfRef.top__DOT__vgc_inst__DOT__copper_reg_flat[3U]) 
           | ((vlSelfRef.top__DOT__vgc_inst__DOT__copper_reg[14U] 
               << 0x00000010U) | ((vlSelfRef.top__DOT__vgc_inst__DOT__copper_reg[13U] 
                                   << 8U) | vlSelfRef.top__DOT__vgc_inst__DOT__copper_reg[12U])));
    vlSelfRef.top__DOT__vgc_inst__DOT__copper_reg_flat[3U] 
        = ((0x00ffffffU & vlSelfRef.top__DOT__vgc_inst__DOT__copper_reg_flat[3U]) 
           | (vlSelfRef.top__DOT__vgc_inst__DOT__copper_reg[15U] 
              << 0x00000018U));
    vlSelfRef.top__DOT__vgc_inst__DOT__copper_reg_flat[4U] 
        = ((0xffff0000U & vlSelfRef.top__DOT__vgc_inst__DOT__copper_reg_flat[4U]) 
           | (0x00ffffffU & ((0x00ffff00U & (vlSelfRef.top__DOT__vgc_inst__DOT__copper_reg[17U] 
                                             << 8U)) 
                             | ((0x00ffffffU & vlSelfRef.top__DOT__vgc_inst__DOT__copper_reg[16U]) 
                                | (vlSelfRef.top__DOT__vgc_inst__DOT__copper_reg[15U] 
                                   >> 8U)))));
    vlSelfRef.top__DOT__vgc_inst__DOT__copper_reg_flat[4U] 
        = ((0x0000ffffU & vlSelfRef.top__DOT__vgc_inst__DOT__copper_reg_flat[4U]) 
           | (0xffff0000U & ((vlSelfRef.top__DOT__vgc_inst__DOT__copper_reg[19U] 
                              << 0x00000018U) | (vlSelfRef.top__DOT__vgc_inst__DOT__copper_reg[18U] 
                                                 << 0x00000010U))));
    vlSelfRef.top__DOT__vgc_inst__DOT__copper_reg_flat[5U] 
        = ((0xffffff00U & vlSelfRef.top__DOT__vgc_inst__DOT__copper_reg_flat[5U]) 
           | (0x0000ffffU & ((0x0000ffffU & vlSelfRef.top__DOT__vgc_inst__DOT__copper_reg[20U]) 
                             | ((0x0000ffffU & (vlSelfRef.top__DOT__vgc_inst__DOT__copper_reg[19U] 
                                                >> 8U)) 
                                | (vlSelfRef.top__DOT__vgc_inst__DOT__copper_reg[18U] 
                                   >> 0x00000010U)))));
    vlSelfRef.top__DOT__vgc_inst__DOT__copper_reg_flat[5U] 
        = ((0x000000ffU & vlSelfRef.top__DOT__vgc_inst__DOT__copper_reg_flat[5U]) 
           | (0xffffff00U & ((vlSelfRef.top__DOT__vgc_inst__DOT__copper_reg[23U] 
                              << 0x00000018U) | ((vlSelfRef.top__DOT__vgc_inst__DOT__copper_reg[22U] 
                                                  << 0x00000010U) 
                                                 | (vlSelfRef.top__DOT__vgc_inst__DOT__copper_reg[21U] 
                                                    << 8U)))));
    vlSelfRef.top__DOT__vgc_inst__DOT__copper_reg_flat[6U] 
        = ((0xff000000U & vlSelfRef.top__DOT__vgc_inst__DOT__copper_reg_flat[6U]) 
           | ((vlSelfRef.top__DOT__vgc_inst__DOT__copper_reg[26U] 
               << 0x00000010U) | ((vlSelfRef.top__DOT__vgc_inst__DOT__copper_reg[25U] 
                                   << 8U) | vlSelfRef.top__DOT__vgc_inst__DOT__copper_reg[24U])));
    vlSelfRef.top__DOT__vgc_inst__DOT__copper_reg_flat[6U] 
        = ((0x00ffffffU & vlSelfRef.top__DOT__vgc_inst__DOT__copper_reg_flat[6U]) 
           | (vlSelfRef.top__DOT__vgc_inst__DOT__copper_reg[27U] 
              << 0x00000018U));
    vlSelfRef.top__DOT__vgc_inst__DOT__copper_reg_flat[7U] 
        = ((0xffff0000U & vlSelfRef.top__DOT__vgc_inst__DOT__copper_reg_flat[7U]) 
           | (0x00ffffffU & ((0x00ffff00U & (vlSelfRef.top__DOT__vgc_inst__DOT__copper_reg[29U] 
                                             << 8U)) 
                             | ((0x00ffffffU & vlSelfRef.top__DOT__vgc_inst__DOT__copper_reg[28U]) 
                                | (vlSelfRef.top__DOT__vgc_inst__DOT__copper_reg[27U] 
                                   >> 8U)))));
    vlSelfRef.top__DOT__vgc_inst__DOT__copper_reg_flat[7U] 
        = ((0x0000ffffU & vlSelfRef.top__DOT__vgc_inst__DOT__copper_reg_flat[7U]) 
           | (0xffff0000U & ((vlSelfRef.top__DOT__vgc_inst__DOT__copper_reg[31U] 
                              << 0x00000018U) | (vlSelfRef.top__DOT__vgc_inst__DOT__copper_reg[30U] 
                                                 << 0x00000010U))));
    vlSelfRef.top__DOT__vgc_inst__DOT__copper_val_flat[0U] 
        = ((0xff000000U & vlSelfRef.top__DOT__vgc_inst__DOT__copper_val_flat[0U]) 
           | ((vlSelfRef.top__DOT__vgc_inst__DOT__copper_val[2U] 
               << 0x00000010U) | ((vlSelfRef.top__DOT__vgc_inst__DOT__copper_val[1U] 
                                   << 8U) | vlSelfRef.top__DOT__vgc_inst__DOT__copper_val[0U])));
    vlSelfRef.top__DOT__vgc_inst__DOT__copper_val_flat[0U] 
        = ((0x00ffffffU & vlSelfRef.top__DOT__vgc_inst__DOT__copper_val_flat[0U]) 
           | (vlSelfRef.top__DOT__vgc_inst__DOT__copper_val[3U] 
              << 0x00000018U));
    vlSelfRef.top__DOT__vgc_inst__DOT__copper_val_flat[1U] 
        = ((0xffff0000U & vlSelfRef.top__DOT__vgc_inst__DOT__copper_val_flat[1U]) 
           | (0x00ffffffU & ((0x00ffff00U & (vlSelfRef.top__DOT__vgc_inst__DOT__copper_val[5U] 
                                             << 8U)) 
                             | ((0x00ffffffU & vlSelfRef.top__DOT__vgc_inst__DOT__copper_val[4U]) 
                                | (vlSelfRef.top__DOT__vgc_inst__DOT__copper_val[3U] 
                                   >> 8U)))));
    vlSelfRef.top__DOT__vgc_inst__DOT__copper_val_flat[1U] 
        = ((0x0000ffffU & vlSelfRef.top__DOT__vgc_inst__DOT__copper_val_flat[1U]) 
           | (0xffff0000U & ((vlSelfRef.top__DOT__vgc_inst__DOT__copper_val[7U] 
                              << 0x00000018U) | (vlSelfRef.top__DOT__vgc_inst__DOT__copper_val[6U] 
                                                 << 0x00000010U))));
    vlSelfRef.top__DOT__vgc_inst__DOT__copper_val_flat[2U] 
        = ((0xffffff00U & vlSelfRef.top__DOT__vgc_inst__DOT__copper_val_flat[2U]) 
           | (0x0000ffffU & ((0x0000ffffU & vlSelfRef.top__DOT__vgc_inst__DOT__copper_val[8U]) 
                             | ((0x0000ffffU & (vlSelfRef.top__DOT__vgc_inst__DOT__copper_val[7U] 
                                                >> 8U)) 
                                | (vlSelfRef.top__DOT__vgc_inst__DOT__copper_val[6U] 
                                   >> 0x00000010U)))));
    vlSelfRef.top__DOT__vgc_inst__DOT__copper_val_flat[2U] 
        = ((0x000000ffU & vlSelfRef.top__DOT__vgc_inst__DOT__copper_val_flat[2U]) 
           | (0xffffff00U & ((vlSelfRef.top__DOT__vgc_inst__DOT__copper_val[11U] 
                              << 0x00000018U) | ((vlSelfRef.top__DOT__vgc_inst__DOT__copper_val[10U] 
                                                  << 0x00000010U) 
                                                 | (vlSelfRef.top__DOT__vgc_inst__DOT__copper_val[9U] 
                                                    << 8U)))));
    vlSelfRef.top__DOT__vgc_inst__DOT__copper_val_flat[3U] 
        = ((0xff000000U & vlSelfRef.top__DOT__vgc_inst__DOT__copper_val_flat[3U]) 
           | ((vlSelfRef.top__DOT__vgc_inst__DOT__copper_val[14U] 
               << 0x00000010U) | ((vlSelfRef.top__DOT__vgc_inst__DOT__copper_val[13U] 
                                   << 8U) | vlSelfRef.top__DOT__vgc_inst__DOT__copper_val[12U])));
    vlSelfRef.top__DOT__vgc_inst__DOT__copper_val_flat[3U] 
        = ((0x00ffffffU & vlSelfRef.top__DOT__vgc_inst__DOT__copper_val_flat[3U]) 
           | (vlSelfRef.top__DOT__vgc_inst__DOT__copper_val[15U] 
              << 0x00000018U));
    vlSelfRef.top__DOT__vgc_inst__DOT__copper_val_flat[4U] 
        = ((0xffff0000U & vlSelfRef.top__DOT__vgc_inst__DOT__copper_val_flat[4U]) 
           | (0x00ffffffU & ((0x00ffff00U & (vlSelfRef.top__DOT__vgc_inst__DOT__copper_val[17U] 
                                             << 8U)) 
                             | ((0x00ffffffU & vlSelfRef.top__DOT__vgc_inst__DOT__copper_val[16U]) 
                                | (vlSelfRef.top__DOT__vgc_inst__DOT__copper_val[15U] 
                                   >> 8U)))));
    vlSelfRef.top__DOT__vgc_inst__DOT__copper_val_flat[4U] 
        = ((0x0000ffffU & vlSelfRef.top__DOT__vgc_inst__DOT__copper_val_flat[4U]) 
           | (0xffff0000U & ((vlSelfRef.top__DOT__vgc_inst__DOT__copper_val[19U] 
                              << 0x00000018U) | (vlSelfRef.top__DOT__vgc_inst__DOT__copper_val[18U] 
                                                 << 0x00000010U))));
    vlSelfRef.top__DOT__vgc_inst__DOT__copper_val_flat[5U] 
        = ((0xffffff00U & vlSelfRef.top__DOT__vgc_inst__DOT__copper_val_flat[5U]) 
           | (0x0000ffffU & ((0x0000ffffU & vlSelfRef.top__DOT__vgc_inst__DOT__copper_val[20U]) 
                             | ((0x0000ffffU & (vlSelfRef.top__DOT__vgc_inst__DOT__copper_val[19U] 
                                                >> 8U)) 
                                | (vlSelfRef.top__DOT__vgc_inst__DOT__copper_val[18U] 
                                   >> 0x00000010U)))));
    vlSelfRef.top__DOT__vgc_inst__DOT__copper_val_flat[5U] 
        = ((0x000000ffU & vlSelfRef.top__DOT__vgc_inst__DOT__copper_val_flat[5U]) 
           | (0xffffff00U & ((vlSelfRef.top__DOT__vgc_inst__DOT__copper_val[23U] 
                              << 0x00000018U) | ((vlSelfRef.top__DOT__vgc_inst__DOT__copper_val[22U] 
                                                  << 0x00000010U) 
                                                 | (vlSelfRef.top__DOT__vgc_inst__DOT__copper_val[21U] 
                                                    << 8U)))));
    vlSelfRef.top__DOT__vgc_inst__DOT__copper_val_flat[6U] 
        = ((0xff000000U & vlSelfRef.top__DOT__vgc_inst__DOT__copper_val_flat[6U]) 
           | ((vlSelfRef.top__DOT__vgc_inst__DOT__copper_val[26U] 
               << 0x00000010U) | ((vlSelfRef.top__DOT__vgc_inst__DOT__copper_val[25U] 
                                   << 8U) | vlSelfRef.top__DOT__vgc_inst__DOT__copper_val[24U])));
    vlSelfRef.top__DOT__vgc_inst__DOT__copper_val_flat[6U] 
        = ((0x00ffffffU & vlSelfRef.top__DOT__vgc_inst__DOT__copper_val_flat[6U]) 
           | (vlSelfRef.top__DOT__vgc_inst__DOT__copper_val[27U] 
              << 0x00000018U));
    vlSelfRef.top__DOT__vgc_inst__DOT__copper_val_flat[7U] 
        = ((0xffff0000U & vlSelfRef.top__DOT__vgc_inst__DOT__copper_val_flat[7U]) 
           | (0x00ffffffU & ((0x00ffff00U & (vlSelfRef.top__DOT__vgc_inst__DOT__copper_val[29U] 
                                             << 8U)) 
                             | ((0x00ffffffU & vlSelfRef.top__DOT__vgc_inst__DOT__copper_val[28U]) 
                                | (vlSelfRef.top__DOT__vgc_inst__DOT__copper_val[27U] 
                                   >> 8U)))));
    vlSelfRef.top__DOT__vgc_inst__DOT__copper_val_flat[7U] 
        = ((0x0000ffffU & vlSelfRef.top__DOT__vgc_inst__DOT__copper_val_flat[7U]) 
           | (0xffff0000U & ((vlSelfRef.top__DOT__vgc_inst__DOT__copper_val[31U] 
                              << 0x00000018U) | (vlSelfRef.top__DOT__vgc_inst__DOT__copper_val[30U] 
                                                 << 0x00000010U))));
    top__DOT__vgc_inst__DOT____Vlvbound_h3e9a9ec3__0 
        = vlSelfRef.top__DOT__vgc_inst__DOT__copper_pos[0U];
    vlSelfRef.top__DOT__vgc_inst__DOT__copper_pos_flat[0U] 
        = ((0xfffe0000U & vlSelfRef.top__DOT__vgc_inst__DOT__copper_pos_flat[0U]) 
           | top__DOT__vgc_inst__DOT____Vlvbound_h3e9a9ec3__0);
    top__DOT__vgc_inst__DOT____Vlvbound_h3e9a9ec3__0 
        = vlSelfRef.top__DOT__vgc_inst__DOT__copper_pos[1U];
    vlSelfRef.top__DOT__vgc_inst__DOT__copper_pos_flat[0U] 
        = ((0x0001ffffU & vlSelfRef.top__DOT__vgc_inst__DOT__copper_pos_flat[0U]) 
           | (top__DOT__vgc_inst__DOT____Vlvbound_h3e9a9ec3__0 
              << 0x00000011U));
    vlSelfRef.top__DOT__vgc_inst__DOT__copper_pos_flat[1U] 
        = ((0xfffffffcU & vlSelfRef.top__DOT__vgc_inst__DOT__copper_pos_flat[1U]) 
           | (top__DOT__vgc_inst__DOT____Vlvbound_h3e9a9ec3__0 
              >> 0x0000000fU));
    top__DOT__vgc_inst__DOT____Vlvbound_h3e9a9ec3__0 
        = vlSelfRef.top__DOT__vgc_inst__DOT__copper_pos[2U];
    vlSelfRef.top__DOT__vgc_inst__DOT__copper_pos_flat[1U] 
        = ((0xfff80003U & vlSelfRef.top__DOT__vgc_inst__DOT__copper_pos_flat[1U]) 
           | (top__DOT__vgc_inst__DOT____Vlvbound_h3e9a9ec3__0 
              << 2U));
    top__DOT__vgc_inst__DOT____Vlvbound_h3e9a9ec3__0 
        = vlSelfRef.top__DOT__vgc_inst__DOT__copper_pos[3U];
    vlSelfRef.top__DOT__vgc_inst__DOT__copper_pos_flat[1U] 
        = ((0x0007ffffU & vlSelfRef.top__DOT__vgc_inst__DOT__copper_pos_flat[1U]) 
           | (top__DOT__vgc_inst__DOT____Vlvbound_h3e9a9ec3__0 
              << 0x00000013U));
    vlSelfRef.top__DOT__vgc_inst__DOT__copper_pos_flat[2U] 
        = ((0xfffffff0U & vlSelfRef.top__DOT__vgc_inst__DOT__copper_pos_flat[2U]) 
           | (top__DOT__vgc_inst__DOT____Vlvbound_h3e9a9ec3__0 
              >> 0x0000000dU));
    top__DOT__vgc_inst__DOT____Vlvbound_h3e9a9ec3__0 
        = vlSelfRef.top__DOT__vgc_inst__DOT__copper_pos[4U];
    vlSelfRef.top__DOT__vgc_inst__DOT__copper_pos_flat[2U] 
        = ((0xffe0000fU & vlSelfRef.top__DOT__vgc_inst__DOT__copper_pos_flat[2U]) 
           | (top__DOT__vgc_inst__DOT____Vlvbound_h3e9a9ec3__0 
              << 4U));
    top__DOT__vgc_inst__DOT____Vlvbound_h3e9a9ec3__0 
        = vlSelfRef.top__DOT__vgc_inst__DOT__copper_pos[5U];
    vlSelfRef.top__DOT__vgc_inst__DOT__copper_pos_flat[2U] 
        = ((0x001fffffU & vlSelfRef.top__DOT__vgc_inst__DOT__copper_pos_flat[2U]) 
           | (top__DOT__vgc_inst__DOT____Vlvbound_h3e9a9ec3__0 
              << 0x00000015U));
    vlSelfRef.top__DOT__vgc_inst__DOT__copper_pos_flat[3U] 
        = ((0xffffffc0U & vlSelfRef.top__DOT__vgc_inst__DOT__copper_pos_flat[3U]) 
           | (top__DOT__vgc_inst__DOT____Vlvbound_h3e9a9ec3__0 
              >> 0x0000000bU));
    top__DOT__vgc_inst__DOT____Vlvbound_h3e9a9ec3__0 
        = vlSelfRef.top__DOT__vgc_inst__DOT__copper_pos[6U];
    vlSelfRef.top__DOT__vgc_inst__DOT__copper_pos_flat[3U] 
        = ((0xff80003fU & vlSelfRef.top__DOT__vgc_inst__DOT__copper_pos_flat[3U]) 
           | (top__DOT__vgc_inst__DOT____Vlvbound_h3e9a9ec3__0 
              << 6U));
    top__DOT__vgc_inst__DOT____Vlvbound_h3e9a9ec3__0 
        = vlSelfRef.top__DOT__vgc_inst__DOT__copper_pos[7U];
    vlSelfRef.top__DOT__vgc_inst__DOT__copper_pos_flat[3U] 
        = ((0x007fffffU & vlSelfRef.top__DOT__vgc_inst__DOT__copper_pos_flat[3U]) 
           | (top__DOT__vgc_inst__DOT____Vlvbound_h3e9a9ec3__0 
              << 0x00000017U));
    vlSelfRef.top__DOT__vgc_inst__DOT__copper_pos_flat[4U] 
        = ((0xffffff00U & vlSelfRef.top__DOT__vgc_inst__DOT__copper_pos_flat[4U]) 
           | (top__DOT__vgc_inst__DOT____Vlvbound_h3e9a9ec3__0 
              >> 9U));
    top__DOT__vgc_inst__DOT____Vlvbound_h3e9a9ec3__0 
        = vlSelfRef.top__DOT__vgc_inst__DOT__copper_pos[8U];
    vlSelfRef.top__DOT__vgc_inst__DOT__copper_pos_flat[4U] 
        = ((0xfe0000ffU & vlSelfRef.top__DOT__vgc_inst__DOT__copper_pos_flat[4U]) 
           | (top__DOT__vgc_inst__DOT____Vlvbound_h3e9a9ec3__0 
              << 8U));
    top__DOT__vgc_inst__DOT____Vlvbound_h3e9a9ec3__0 
        = vlSelfRef.top__DOT__vgc_inst__DOT__copper_pos[9U];
    vlSelfRef.top__DOT__vgc_inst__DOT__copper_pos_flat[4U] 
        = ((0x01ffffffU & vlSelfRef.top__DOT__vgc_inst__DOT__copper_pos_flat[4U]) 
           | (top__DOT__vgc_inst__DOT____Vlvbound_h3e9a9ec3__0 
              << 0x00000019U));
    vlSelfRef.top__DOT__vgc_inst__DOT__copper_pos_flat[5U] 
        = ((0xfffffc00U & vlSelfRef.top__DOT__vgc_inst__DOT__copper_pos_flat[5U]) 
           | (top__DOT__vgc_inst__DOT____Vlvbound_h3e9a9ec3__0 
              >> 7U));
    top__DOT__vgc_inst__DOT____Vlvbound_h3e9a9ec3__0 
        = vlSelfRef.top__DOT__vgc_inst__DOT__copper_pos[10U];
    vlSelfRef.top__DOT__vgc_inst__DOT__copper_pos_flat[5U] 
        = ((0xf80003ffU & vlSelfRef.top__DOT__vgc_inst__DOT__copper_pos_flat[5U]) 
           | (top__DOT__vgc_inst__DOT____Vlvbound_h3e9a9ec3__0 
              << 0x0000000aU));
    top__DOT__vgc_inst__DOT____Vlvbound_h3e9a9ec3__0 
        = vlSelfRef.top__DOT__vgc_inst__DOT__copper_pos[11U];
    vlSelfRef.top__DOT__vgc_inst__DOT__copper_pos_flat[5U] 
        = ((0x07ffffffU & vlSelfRef.top__DOT__vgc_inst__DOT__copper_pos_flat[5U]) 
           | (top__DOT__vgc_inst__DOT____Vlvbound_h3e9a9ec3__0 
              << 0x0000001bU));
    vlSelfRef.top__DOT__vgc_inst__DOT__copper_pos_flat[6U] 
        = ((0xfffff000U & vlSelfRef.top__DOT__vgc_inst__DOT__copper_pos_flat[6U]) 
           | (top__DOT__vgc_inst__DOT____Vlvbound_h3e9a9ec3__0 
              >> 5U));
    top__DOT__vgc_inst__DOT____Vlvbound_h3e9a9ec3__0 
        = vlSelfRef.top__DOT__vgc_inst__DOT__copper_pos[12U];
    vlSelfRef.top__DOT__vgc_inst__DOT__copper_pos_flat[6U] 
        = ((0xe0000fffU & vlSelfRef.top__DOT__vgc_inst__DOT__copper_pos_flat[6U]) 
           | (top__DOT__vgc_inst__DOT____Vlvbound_h3e9a9ec3__0 
              << 0x0000000cU));
    top__DOT__vgc_inst__DOT____Vlvbound_h3e9a9ec3__0 
        = vlSelfRef.top__DOT__vgc_inst__DOT__copper_pos[13U];
    vlSelfRef.top__DOT__vgc_inst__DOT__copper_pos_flat[6U] 
        = ((0x1fffffffU & vlSelfRef.top__DOT__vgc_inst__DOT__copper_pos_flat[6U]) 
           | (top__DOT__vgc_inst__DOT____Vlvbound_h3e9a9ec3__0 
              << 0x0000001dU));
    vlSelfRef.top__DOT__vgc_inst__DOT__copper_pos_flat[7U] 
        = ((0xffffc000U & vlSelfRef.top__DOT__vgc_inst__DOT__copper_pos_flat[7U]) 
           | (top__DOT__vgc_inst__DOT____Vlvbound_h3e9a9ec3__0 
              >> 3U));
    top__DOT__vgc_inst__DOT____Vlvbound_h3e9a9ec3__0 
        = vlSelfRef.top__DOT__vgc_inst__DOT__copper_pos[14U];
    vlSelfRef.top__DOT__vgc_inst__DOT__copper_pos_flat[7U] 
        = ((0x80003fffU & vlSelfRef.top__DOT__vgc_inst__DOT__copper_pos_flat[7U]) 
           | (top__DOT__vgc_inst__DOT____Vlvbound_h3e9a9ec3__0 
              << 0x0000000eU));
    top__DOT__vgc_inst__DOT____Vlvbound_h3e9a9ec3__0 
        = vlSelfRef.top__DOT__vgc_inst__DOT__copper_pos[15U];
    vlSelfRef.top__DOT__vgc_inst__DOT__copper_pos_flat[7U] 
        = ((0x7fffffffU & vlSelfRef.top__DOT__vgc_inst__DOT__copper_pos_flat[7U]) 
           | (top__DOT__vgc_inst__DOT____Vlvbound_h3e9a9ec3__0 
              << 0x0000001fU));
    vlSelfRef.top__DOT__vgc_inst__DOT__copper_pos_flat[8U] 
        = ((0xffff0000U & vlSelfRef.top__DOT__vgc_inst__DOT__copper_pos_flat[8U]) 
           | (top__DOT__vgc_inst__DOT____Vlvbound_h3e9a9ec3__0 
              >> 1U));
    top__DOT__vgc_inst__DOT____Vlvbound_h3e9a9ec3__0 
        = vlSelfRef.top__DOT__vgc_inst__DOT__copper_pos[16U];
    vlSelfRef.top__DOT__vgc_inst__DOT__copper_pos_flat[8U] 
        = ((0x0000ffffU & vlSelfRef.top__DOT__vgc_inst__DOT__copper_pos_flat[8U]) 
           | (top__DOT__vgc_inst__DOT____Vlvbound_h3e9a9ec3__0 
              << 0x00000010U));
    vlSelfRef.top__DOT__vgc_inst__DOT__copper_pos_flat[9U] 
        = ((0xfffffffeU & vlSelfRef.top__DOT__vgc_inst__DOT__copper_pos_flat[9U]) 
           | (top__DOT__vgc_inst__DOT____Vlvbound_h3e9a9ec3__0 
              >> 0x00000010U));
    top__DOT__vgc_inst__DOT____Vlvbound_h3e9a9ec3__0 
        = vlSelfRef.top__DOT__vgc_inst__DOT__copper_pos[17U];
    vlSelfRef.top__DOT__vgc_inst__DOT__copper_pos_flat[9U] 
        = ((0xfffc0001U & vlSelfRef.top__DOT__vgc_inst__DOT__copper_pos_flat[9U]) 
           | (top__DOT__vgc_inst__DOT____Vlvbound_h3e9a9ec3__0 
              << 1U));
    top__DOT__vgc_inst__DOT____Vlvbound_h3e9a9ec3__0 
        = vlSelfRef.top__DOT__vgc_inst__DOT__copper_pos[18U];
    vlSelfRef.top__DOT__vgc_inst__DOT__copper_pos_flat[9U] 
        = ((0x0003ffffU & vlSelfRef.top__DOT__vgc_inst__DOT__copper_pos_flat[9U]) 
           | (top__DOT__vgc_inst__DOT____Vlvbound_h3e9a9ec3__0 
              << 0x00000012U));
    vlSelfRef.top__DOT__vgc_inst__DOT__copper_pos_flat[10U] 
        = ((0xfffffff8U & vlSelfRef.top__DOT__vgc_inst__DOT__copper_pos_flat[10U]) 
           | (top__DOT__vgc_inst__DOT____Vlvbound_h3e9a9ec3__0 
              >> 0x0000000eU));
    top__DOT__vgc_inst__DOT____Vlvbound_h3e9a9ec3__0 
        = vlSelfRef.top__DOT__vgc_inst__DOT__copper_pos[19U];
    vlSelfRef.top__DOT__vgc_inst__DOT__copper_pos_flat[10U] 
        = ((0xfff00007U & vlSelfRef.top__DOT__vgc_inst__DOT__copper_pos_flat[10U]) 
           | (top__DOT__vgc_inst__DOT____Vlvbound_h3e9a9ec3__0 
              << 3U));
    top__DOT__vgc_inst__DOT____Vlvbound_h3e9a9ec3__0 
        = vlSelfRef.top__DOT__vgc_inst__DOT__copper_pos[20U];
    vlSelfRef.top__DOT__vgc_inst__DOT__copper_pos_flat[10U] 
        = ((0x000fffffU & vlSelfRef.top__DOT__vgc_inst__DOT__copper_pos_flat[10U]) 
           | (top__DOT__vgc_inst__DOT____Vlvbound_h3e9a9ec3__0 
              << 0x00000014U));
    vlSelfRef.top__DOT__vgc_inst__DOT__copper_pos_flat[11U] 
        = ((0xffffffe0U & vlSelfRef.top__DOT__vgc_inst__DOT__copper_pos_flat[11U]) 
           | (top__DOT__vgc_inst__DOT____Vlvbound_h3e9a9ec3__0 
              >> 0x0000000cU));
    top__DOT__vgc_inst__DOT____Vlvbound_h3e9a9ec3__0 
        = vlSelfRef.top__DOT__vgc_inst__DOT__copper_pos[21U];
    vlSelfRef.top__DOT__vgc_inst__DOT__copper_pos_flat[11U] 
        = ((0xffc0001fU & vlSelfRef.top__DOT__vgc_inst__DOT__copper_pos_flat[11U]) 
           | (top__DOT__vgc_inst__DOT____Vlvbound_h3e9a9ec3__0 
              << 5U));
    top__DOT__vgc_inst__DOT____Vlvbound_h3e9a9ec3__0 
        = vlSelfRef.top__DOT__vgc_inst__DOT__copper_pos[22U];
    vlSelfRef.top__DOT__vgc_inst__DOT__copper_pos_flat[11U] 
        = ((0x003fffffU & vlSelfRef.top__DOT__vgc_inst__DOT__copper_pos_flat[11U]) 
           | (top__DOT__vgc_inst__DOT____Vlvbound_h3e9a9ec3__0 
              << 0x00000016U));
    vlSelfRef.top__DOT__vgc_inst__DOT__copper_pos_flat[12U] 
        = ((0xffffff80U & vlSelfRef.top__DOT__vgc_inst__DOT__copper_pos_flat[12U]) 
           | (top__DOT__vgc_inst__DOT____Vlvbound_h3e9a9ec3__0 
              >> 0x0000000aU));
    top__DOT__vgc_inst__DOT____Vlvbound_h3e9a9ec3__0 
        = vlSelfRef.top__DOT__vgc_inst__DOT__copper_pos[23U];
    vlSelfRef.top__DOT__vgc_inst__DOT__copper_pos_flat[12U] 
        = ((0xff00007fU & vlSelfRef.top__DOT__vgc_inst__DOT__copper_pos_flat[12U]) 
           | (top__DOT__vgc_inst__DOT____Vlvbound_h3e9a9ec3__0 
              << 7U));
    top__DOT__vgc_inst__DOT____Vlvbound_h3e9a9ec3__0 
        = vlSelfRef.top__DOT__vgc_inst__DOT__copper_pos[24U];
    vlSelfRef.top__DOT__vgc_inst__DOT__copper_pos_flat[12U] 
        = ((0x00ffffffU & vlSelfRef.top__DOT__vgc_inst__DOT__copper_pos_flat[12U]) 
           | (top__DOT__vgc_inst__DOT____Vlvbound_h3e9a9ec3__0 
              << 0x00000018U));
    vlSelfRef.top__DOT__vgc_inst__DOT__copper_pos_flat[13U] 
        = ((0xfffffe00U & vlSelfRef.top__DOT__vgc_inst__DOT__copper_pos_flat[13U]) 
           | (top__DOT__vgc_inst__DOT____Vlvbound_h3e9a9ec3__0 
              >> 8U));
    top__DOT__vgc_inst__DOT____Vlvbound_h3e9a9ec3__0 
        = vlSelfRef.top__DOT__vgc_inst__DOT__copper_pos[25U];
    vlSelfRef.top__DOT__vgc_inst__DOT__copper_pos_flat[13U] 
        = ((0xfc0001ffU & vlSelfRef.top__DOT__vgc_inst__DOT__copper_pos_flat[13U]) 
           | (top__DOT__vgc_inst__DOT____Vlvbound_h3e9a9ec3__0 
              << 9U));
    top__DOT__vgc_inst__DOT____Vlvbound_h3e9a9ec3__0 
        = vlSelfRef.top__DOT__vgc_inst__DOT__copper_pos[26U];
    vlSelfRef.top__DOT__vgc_inst__DOT__copper_pos_flat[13U] 
        = ((0x03ffffffU & vlSelfRef.top__DOT__vgc_inst__DOT__copper_pos_flat[13U]) 
           | (top__DOT__vgc_inst__DOT____Vlvbound_h3e9a9ec3__0 
              << 0x0000001aU));
    vlSelfRef.top__DOT__vgc_inst__DOT__copper_pos_flat[14U] 
        = ((0xfffff800U & vlSelfRef.top__DOT__vgc_inst__DOT__copper_pos_flat[14U]) 
           | (top__DOT__vgc_inst__DOT____Vlvbound_h3e9a9ec3__0 
              >> 6U));
    top__DOT__vgc_inst__DOT____Vlvbound_h3e9a9ec3__0 
        = vlSelfRef.top__DOT__vgc_inst__DOT__copper_pos[27U];
    vlSelfRef.top__DOT__vgc_inst__DOT__copper_pos_flat[14U] 
        = ((0xf00007ffU & vlSelfRef.top__DOT__vgc_inst__DOT__copper_pos_flat[14U]) 
           | (top__DOT__vgc_inst__DOT____Vlvbound_h3e9a9ec3__0 
              << 0x0000000bU));
    top__DOT__vgc_inst__DOT____Vlvbound_h3e9a9ec3__0 
        = vlSelfRef.top__DOT__vgc_inst__DOT__copper_pos[28U];
    vlSelfRef.top__DOT__vgc_inst__DOT__copper_pos_flat[14U] 
        = ((0x0fffffffU & vlSelfRef.top__DOT__vgc_inst__DOT__copper_pos_flat[14U]) 
           | (top__DOT__vgc_inst__DOT____Vlvbound_h3e9a9ec3__0 
              << 0x0000001cU));
    vlSelfRef.top__DOT__vgc_inst__DOT__copper_pos_flat[15U] 
        = ((0xffffe000U & vlSelfRef.top__DOT__vgc_inst__DOT__copper_pos_flat[15U]) 
           | (top__DOT__vgc_inst__DOT____Vlvbound_h3e9a9ec3__0 
              >> 4U));
    top__DOT__vgc_inst__DOT____Vlvbound_h3e9a9ec3__0 
        = vlSelfRef.top__DOT__vgc_inst__DOT__copper_pos[29U];
    vlSelfRef.top__DOT__vgc_inst__DOT__copper_pos_flat[15U] 
        = ((0xc0001fffU & vlSelfRef.top__DOT__vgc_inst__DOT__copper_pos_flat[15U]) 
           | (top__DOT__vgc_inst__DOT____Vlvbound_h3e9a9ec3__0 
              << 0x0000000dU));
    top__DOT__vgc_inst__DOT____Vlvbound_h3e9a9ec3__0 
        = vlSelfRef.top__DOT__vgc_inst__DOT__copper_pos[30U];
    vlSelfRef.top__DOT__vgc_inst__DOT__copper_pos_flat[15U] 
        = ((0x3fffffffU & vlSelfRef.top__DOT__vgc_inst__DOT__copper_pos_flat[15U]) 
           | (top__DOT__vgc_inst__DOT____Vlvbound_h3e9a9ec3__0 
              << 0x0000001eU));
    vlSelfRef.top__DOT__vgc_inst__DOT__copper_pos_flat[16U] 
        = ((0xffff8000U & vlSelfRef.top__DOT__vgc_inst__DOT__copper_pos_flat[16U]) 
           | (top__DOT__vgc_inst__DOT____Vlvbound_h3e9a9ec3__0 
              >> 2U));
    top__DOT__vgc_inst__DOT____Vlvbound_h3e9a9ec3__0 
        = vlSelfRef.top__DOT__vgc_inst__DOT__copper_pos[31U];
    vlSelfRef.top__DOT__vgc_inst__DOT__copper_pos_flat[16U] 
        = ((0x00007fffU & vlSelfRef.top__DOT__vgc_inst__DOT__copper_pos_flat[16U]) 
           | (top__DOT__vgc_inst__DOT____Vlvbound_h3e9a9ec3__0 
              << 0x0000000fU));
    top__DOT__vgc_inst__DOT__tile_blt_wdata = ((IData)(vlSelfRef.top__DOT__vgc_inst__DOT__vgc_tile_we)
                                                ? (IData)(vlSelfRef.top__DOT__vgc_inst__DOT__vgc_tile_wdata)
                                                : (IData)(top__DOT__blt_vgc_wdata));
    top__DOT__vgc_inst__DOT__tile_blt_addr = (0x00007fffU 
                                              & (((IData)(vlSelfRef.top__DOT__vgc_inst__DOT__vgc_tile_re) 
                                                  | (IData)(vlSelfRef.top__DOT__vgc_inst__DOT__vgc_tile_we))
                                                  ? (IData)(vlSelfRef.top__DOT__vgc_inst__DOT__vgc_tile_addr)
                                                  : (IData)(vlSelfRef.top__DOT__blt_vgc_addr)));
    vlSelfRef.top__DOT__vgc_inst__DOT__tile_blt_we 
        = ((IData)(vlSelfRef.top__DOT__vgc_inst__DOT__vgc_tile_we) 
           | ((IData)(vlSelfRef.top__DOT__blt_vgc_we) 
              & (6U == (IData)(vlSelfRef.top__DOT__blt_vgc_space))));
    top__DOT__vgc_inst__DOT__cur_fg_d2 = (0x0000000fU 
                                          & (IData)(vlSelfRef.top__DOT__vgc_inst__DOT__color_b_dout));
    top__DOT__vgc_inst__DOT__pixel_on_d2 = (1U & ((IData)(vlSelfRef.top__DOT__vgc_inst__DOT__font_b_dout) 
                                                  >> 
                                                  (7U 
                                                   & ((IData)(7U) 
                                                      - (IData)(vlSelfRef.top__DOT__vgc_inst__DOT__font_pixel_d2)))));
    top__DOT__vgc_inst__DOT__text_pixel_d2 = ((IData)(top__DOT__vgc_inst__DOT__pixel_on_d2)
                                               ? vlSelfRef.top__DOT__vgc_inst__DOT__palette
                                              [top__DOT__vgc_inst__DOT__cur_fg_d2]
                                               : vlSelfRef.top__DOT__vgc_inst__DOT__palette
                                              [vlSelfRef.top__DOT__vgc_inst__DOT__bg_color]);
    top__DOT__vgc_inst__DOT__cur_gfx_d2 = vlSelfRef.top__DOT__vgc_inst__DOT__gfx_b_dout;
    top__DOT__vgc_inst__DOT__gfx_pixel_d2 = vlSelfRef.top__DOT__vgc_inst__DOT__palette
        [top__DOT__vgc_inst__DOT__cur_gfx_d2];
    if (vlSelfRef.top__DOT__vgc_inst__DOT__visible_d2) {
        if (vlSelfRef.top__DOT__vgc_inst__DOT__in_text_area_d2) {
            vlSelfRef.top__DOT__vgc_inst__DOT__pixel_color 
                = ((4U & (IData)(vlSelfRef.top__DOT__vgc_inst__DOT__mode))
                    ? ((2U & (IData)(vlSelfRef.top__DOT__vgc_inst__DOT__mode))
                        ? (IData)(top__DOT__vgc_inst__DOT__text_pixel_d2)
                        : ((1U & (IData)(vlSelfRef.top__DOT__vgc_inst__DOT__mode))
                            ? (IData)(top__DOT__vgc_inst__DOT__text_pixel_d2)
                            : ((0U != (IData)(vlSelfRef.top__DOT__vgc_inst__DOT__tile_opaque_d2))
                                ? (IData)(vlSelfRef.top__DOT__vgc_inst__DOT__tile_rgb_d2)
                                : vlSelfRef.top__DOT__vgc_inst__DOT__palette
                               [vlSelfRef.top__DOT__vgc_inst__DOT__bg_color])))
                    : ((2U & (IData)(vlSelfRef.top__DOT__vgc_inst__DOT__mode))
                        ? ((1U & (IData)(vlSelfRef.top__DOT__vgc_inst__DOT__mode))
                            ? ((0U != (IData)(top__DOT__vgc_inst__DOT__cur_gfx_d2))
                                ? (IData)(top__DOT__vgc_inst__DOT__gfx_pixel_d2)
                                : vlSelfRef.top__DOT__vgc_inst__DOT__palette
                               [vlSelfRef.top__DOT__vgc_inst__DOT__bg_color])
                            : ((IData)(top__DOT__vgc_inst__DOT__pixel_on_d2)
                                ? vlSelfRef.top__DOT__vgc_inst__DOT__palette
                               [top__DOT__vgc_inst__DOT__cur_fg_d2]
                                : ((0U != (IData)(top__DOT__vgc_inst__DOT__cur_gfx_d2))
                                    ? (IData)(top__DOT__vgc_inst__DOT__gfx_pixel_d2)
                                    : vlSelfRef.top__DOT__vgc_inst__DOT__palette
                                   [vlSelfRef.top__DOT__vgc_inst__DOT__bg_color])))
                        : ((1U & (IData)(vlSelfRef.top__DOT__vgc_inst__DOT__mode))
                            ? ((0U != (IData)(top__DOT__vgc_inst__DOT__cur_gfx_d2))
                                ? (IData)(top__DOT__vgc_inst__DOT__gfx_pixel_d2)
                                : (IData)(top__DOT__vgc_inst__DOT__text_pixel_d2))
                            : (IData)(top__DOT__vgc_inst__DOT__text_pixel_d2))));
            if (top__DOT__vgc_inst__DOT__spr_pixel_hit) {
                if ((4U == (IData)(vlSelfRef.top__DOT__vgc_inst__DOT__mode))) {
                    if ((0U == (IData)(top__DOT__vgc_inst__DOT__spr_pixel_pri))) {
                        if ((0U == (IData)(vlSelfRef.top__DOT__vgc_inst__DOT__tile_opaque_d2))) {
                            vlSelfRef.top__DOT__vgc_inst__DOT__pixel_color 
                                = vlSelfRef.top__DOT__vgc_inst__DOT__palette
                                [top__DOT__vgc_inst__DOT__spr_pixel];
                        }
                    } else if ((1U == (IData)(top__DOT__vgc_inst__DOT__spr_pixel_pri))) {
                        if ((2U > (IData)(vlSelfRef.top__DOT__vgc_inst__DOT__tile_opaque_d2))) {
                            vlSelfRef.top__DOT__vgc_inst__DOT__pixel_color 
                                = vlSelfRef.top__DOT__vgc_inst__DOT__palette
                                [top__DOT__vgc_inst__DOT__spr_pixel];
                        }
                    } else {
                        vlSelfRef.top__DOT__vgc_inst__DOT__pixel_color 
                            = vlSelfRef.top__DOT__vgc_inst__DOT__palette
                            [top__DOT__vgc_inst__DOT__spr_pixel];
                    }
                } else if ((0U == (IData)(top__DOT__vgc_inst__DOT__spr_pixel_pri))) {
                    if (((~ (IData)(top__DOT__vgc_inst__DOT__pixel_on_d2)) 
                         & (0U == (IData)(top__DOT__vgc_inst__DOT__cur_gfx_d2)))) {
                        vlSelfRef.top__DOT__vgc_inst__DOT__pixel_color 
                            = vlSelfRef.top__DOT__vgc_inst__DOT__palette
                            [top__DOT__vgc_inst__DOT__spr_pixel];
                    }
                } else {
                    vlSelfRef.top__DOT__vgc_inst__DOT__pixel_color 
                        = vlSelfRef.top__DOT__vgc_inst__DOT__palette
                        [top__DOT__vgc_inst__DOT__spr_pixel];
                }
            }
        } else {
            vlSelfRef.top__DOT__vgc_inst__DOT__pixel_color 
                = vlSelfRef.top__DOT__vgc_inst__DOT__palette
                [vlSelfRef.top__DOT__vgc_inst__DOT__border_color];
        }
    } else {
        vlSelfRef.top__DOT__vgc_inst__DOT__pixel_color = 0U;
    }
    vlSelfRef.top__DOT__vgc_inst__DOT__spr_y_flat[0U] 
        = ((0xff000000U & vlSelfRef.top__DOT__vgc_inst__DOT__spr_y_flat[0U]) 
           | ((vlSelfRef.top__DOT__vgc_inst__DOT__spr_y[2U] 
               << 0x00000010U) | ((vlSelfRef.top__DOT__vgc_inst__DOT__spr_y[1U] 
                                   << 8U) | vlSelfRef.top__DOT__vgc_inst__DOT__spr_y[0U])));
    vlSelfRef.top__DOT__vgc_inst__DOT__spr_y_flat[0U] 
        = ((0x00ffffffU & vlSelfRef.top__DOT__vgc_inst__DOT__spr_y_flat[0U]) 
           | (vlSelfRef.top__DOT__vgc_inst__DOT__spr_y[3U] 
              << 0x00000018U));
    vlSelfRef.top__DOT__vgc_inst__DOT__spr_y_flat[1U] 
        = ((0xffff0000U & vlSelfRef.top__DOT__vgc_inst__DOT__spr_y_flat[1U]) 
           | (0x00ffffffU & ((0x00ffff00U & (vlSelfRef.top__DOT__vgc_inst__DOT__spr_y[5U] 
                                             << 8U)) 
                             | ((0x00ffffffU & vlSelfRef.top__DOT__vgc_inst__DOT__spr_y[4U]) 
                                | (vlSelfRef.top__DOT__vgc_inst__DOT__spr_y[3U] 
                                   >> 8U)))));
    vlSelfRef.top__DOT__vgc_inst__DOT__spr_y_flat[1U] 
        = ((0x0000ffffU & vlSelfRef.top__DOT__vgc_inst__DOT__spr_y_flat[1U]) 
           | (0xffff0000U & ((vlSelfRef.top__DOT__vgc_inst__DOT__spr_y[7U] 
                              << 0x00000018U) | (vlSelfRef.top__DOT__vgc_inst__DOT__spr_y[6U] 
                                                 << 0x00000010U))));
    vlSelfRef.top__DOT__vgc_inst__DOT__spr_y_flat[2U] 
        = ((0xffffff00U & vlSelfRef.top__DOT__vgc_inst__DOT__spr_y_flat[2U]) 
           | (0x0000ffffU & ((0x0000ffffU & vlSelfRef.top__DOT__vgc_inst__DOT__spr_y[8U]) 
                             | ((0x0000ffffU & (vlSelfRef.top__DOT__vgc_inst__DOT__spr_y[7U] 
                                                >> 8U)) 
                                | (vlSelfRef.top__DOT__vgc_inst__DOT__spr_y[6U] 
                                   >> 0x00000010U)))));
    vlSelfRef.top__DOT__vgc_inst__DOT__spr_y_flat[2U] 
        = ((0x000000ffU & vlSelfRef.top__DOT__vgc_inst__DOT__spr_y_flat[2U]) 
           | (0xffffff00U & ((vlSelfRef.top__DOT__vgc_inst__DOT__spr_y[11U] 
                              << 0x00000018U) | ((vlSelfRef.top__DOT__vgc_inst__DOT__spr_y[10U] 
                                                  << 0x00000010U) 
                                                 | (vlSelfRef.top__DOT__vgc_inst__DOT__spr_y[9U] 
                                                    << 8U)))));
    vlSelfRef.top__DOT__vgc_inst__DOT__spr_y_flat[3U] 
        = ((0xff000000U & vlSelfRef.top__DOT__vgc_inst__DOT__spr_y_flat[3U]) 
           | ((vlSelfRef.top__DOT__vgc_inst__DOT__spr_y[14U] 
               << 0x00000010U) | ((vlSelfRef.top__DOT__vgc_inst__DOT__spr_y[13U] 
                                   << 8U) | vlSelfRef.top__DOT__vgc_inst__DOT__spr_y[12U])));
    vlSelfRef.top__DOT__vgc_inst__DOT__spr_y_flat[3U] 
        = ((0x00ffffffU & vlSelfRef.top__DOT__vgc_inst__DOT__spr_y_flat[3U]) 
           | (vlSelfRef.top__DOT__vgc_inst__DOT__spr_y[15U] 
              << 0x00000018U));
    vlSelfRef.top__DOT__vgc_inst__DOT__spr_flip_h_flat 
        = ((0xfff8U & (IData)(vlSelfRef.top__DOT__vgc_inst__DOT__spr_flip_h_flat)) 
           | ((vlSelfRef.top__DOT__vgc_inst__DOT__spr_flip_h[2U] 
               << 2U) | ((vlSelfRef.top__DOT__vgc_inst__DOT__spr_flip_h[1U] 
                          << 1U) | vlSelfRef.top__DOT__vgc_inst__DOT__spr_flip_h[0U])));
    vlSelfRef.top__DOT__vgc_inst__DOT__spr_flip_h_flat 
        = ((0xffc7U & (IData)(vlSelfRef.top__DOT__vgc_inst__DOT__spr_flip_h_flat)) 
           | ((vlSelfRef.top__DOT__vgc_inst__DOT__spr_flip_h[5U] 
               << 5U) | ((vlSelfRef.top__DOT__vgc_inst__DOT__spr_flip_h[4U] 
                          << 4U) | (vlSelfRef.top__DOT__vgc_inst__DOT__spr_flip_h[3U] 
                                    << 3U))));
    vlSelfRef.top__DOT__vgc_inst__DOT__spr_flip_h_flat 
        = ((0xfe3fU & (IData)(vlSelfRef.top__DOT__vgc_inst__DOT__spr_flip_h_flat)) 
           | ((vlSelfRef.top__DOT__vgc_inst__DOT__spr_flip_h[8U] 
               << 8U) | ((vlSelfRef.top__DOT__vgc_inst__DOT__spr_flip_h[7U] 
                          << 7U) | (vlSelfRef.top__DOT__vgc_inst__DOT__spr_flip_h[6U] 
                                    << 6U))));
    vlSelfRef.top__DOT__vgc_inst__DOT__spr_flip_h_flat 
        = ((0xf1ffU & (IData)(vlSelfRef.top__DOT__vgc_inst__DOT__spr_flip_h_flat)) 
           | ((vlSelfRef.top__DOT__vgc_inst__DOT__spr_flip_h[11U] 
               << 0x0000000bU) | ((vlSelfRef.top__DOT__vgc_inst__DOT__spr_flip_h[10U] 
                                   << 0x0000000aU) 
                                  | (vlSelfRef.top__DOT__vgc_inst__DOT__spr_flip_h[9U] 
                                     << 9U))));
    vlSelfRef.top__DOT__vgc_inst__DOT__spr_flip_h_flat 
        = ((0x8fffU & (IData)(vlSelfRef.top__DOT__vgc_inst__DOT__spr_flip_h_flat)) 
           | ((vlSelfRef.top__DOT__vgc_inst__DOT__spr_flip_h[14U] 
               << 0x0000000eU) | ((vlSelfRef.top__DOT__vgc_inst__DOT__spr_flip_h[13U] 
                                   << 0x0000000dU) 
                                  | (vlSelfRef.top__DOT__vgc_inst__DOT__spr_flip_h[12U] 
                                     << 0x0000000cU))));
    vlSelfRef.top__DOT__vgc_inst__DOT__spr_flip_h_flat 
        = ((0x7fffU & (IData)(vlSelfRef.top__DOT__vgc_inst__DOT__spr_flip_h_flat)) 
           | (vlSelfRef.top__DOT__vgc_inst__DOT__spr_flip_h[15U] 
              << 0x0000000fU));
    vlSelfRef.top__DOT__vgc_inst__DOT__spr_flip_v_flat 
        = ((0xfff8U & (IData)(vlSelfRef.top__DOT__vgc_inst__DOT__spr_flip_v_flat)) 
           | ((vlSelfRef.top__DOT__vgc_inst__DOT__spr_flip_v[2U] 
               << 2U) | ((vlSelfRef.top__DOT__vgc_inst__DOT__spr_flip_v[1U] 
                          << 1U) | vlSelfRef.top__DOT__vgc_inst__DOT__spr_flip_v[0U])));
    vlSelfRef.top__DOT__vgc_inst__DOT__spr_flip_v_flat 
        = ((0xffc7U & (IData)(vlSelfRef.top__DOT__vgc_inst__DOT__spr_flip_v_flat)) 
           | ((vlSelfRef.top__DOT__vgc_inst__DOT__spr_flip_v[5U] 
               << 5U) | ((vlSelfRef.top__DOT__vgc_inst__DOT__spr_flip_v[4U] 
                          << 4U) | (vlSelfRef.top__DOT__vgc_inst__DOT__spr_flip_v[3U] 
                                    << 3U))));
    vlSelfRef.top__DOT__vgc_inst__DOT__spr_flip_v_flat 
        = ((0xfe3fU & (IData)(vlSelfRef.top__DOT__vgc_inst__DOT__spr_flip_v_flat)) 
           | ((vlSelfRef.top__DOT__vgc_inst__DOT__spr_flip_v[8U] 
               << 8U) | ((vlSelfRef.top__DOT__vgc_inst__DOT__spr_flip_v[7U] 
                          << 7U) | (vlSelfRef.top__DOT__vgc_inst__DOT__spr_flip_v[6U] 
                                    << 6U))));
    vlSelfRef.top__DOT__vgc_inst__DOT__spr_flip_v_flat 
        = ((0xf1ffU & (IData)(vlSelfRef.top__DOT__vgc_inst__DOT__spr_flip_v_flat)) 
           | ((vlSelfRef.top__DOT__vgc_inst__DOT__spr_flip_v[11U] 
               << 0x0000000bU) | ((vlSelfRef.top__DOT__vgc_inst__DOT__spr_flip_v[10U] 
                                   << 0x0000000aU) 
                                  | (vlSelfRef.top__DOT__vgc_inst__DOT__spr_flip_v[9U] 
                                     << 9U))));
    vlSelfRef.top__DOT__vgc_inst__DOT__spr_flip_v_flat 
        = ((0x8fffU & (IData)(vlSelfRef.top__DOT__vgc_inst__DOT__spr_flip_v_flat)) 
           | ((vlSelfRef.top__DOT__vgc_inst__DOT__spr_flip_v[14U] 
               << 0x0000000eU) | ((vlSelfRef.top__DOT__vgc_inst__DOT__spr_flip_v[13U] 
                                   << 0x0000000dU) 
                                  | (vlSelfRef.top__DOT__vgc_inst__DOT__spr_flip_v[12U] 
                                     << 0x0000000cU))));
    vlSelfRef.top__DOT__vgc_inst__DOT__spr_flip_v_flat 
        = ((0x7fffU & (IData)(vlSelfRef.top__DOT__vgc_inst__DOT__spr_flip_v_flat)) 
           | (vlSelfRef.top__DOT__vgc_inst__DOT__spr_flip_v[15U] 
              << 0x0000000fU));
    vlSelfRef.top__DOT__vgc_inst__DOT__spr_pri_flat 
        = ((0xffffffc0U & vlSelfRef.top__DOT__vgc_inst__DOT__spr_pri_flat) 
           | ((vlSelfRef.top__DOT__vgc_inst__DOT__spr_pri[2U] 
               << 4U) | ((vlSelfRef.top__DOT__vgc_inst__DOT__spr_pri[1U] 
                          << 2U) | vlSelfRef.top__DOT__vgc_inst__DOT__spr_pri[0U])));
    vlSelfRef.top__DOT__vgc_inst__DOT__spr_pri_flat 
        = ((0xfffff03fU & vlSelfRef.top__DOT__vgc_inst__DOT__spr_pri_flat) 
           | ((vlSelfRef.top__DOT__vgc_inst__DOT__spr_pri[5U] 
               << 0x0000000aU) | ((vlSelfRef.top__DOT__vgc_inst__DOT__spr_pri[4U] 
                                   << 8U) | (vlSelfRef.top__DOT__vgc_inst__DOT__spr_pri[3U] 
                                             << 6U))));
    vlSelfRef.top__DOT__vgc_inst__DOT__spr_pri_flat 
        = ((0xfffc0fffU & vlSelfRef.top__DOT__vgc_inst__DOT__spr_pri_flat) 
           | ((vlSelfRef.top__DOT__vgc_inst__DOT__spr_pri[8U] 
               << 0x00000010U) | ((vlSelfRef.top__DOT__vgc_inst__DOT__spr_pri[7U] 
                                   << 0x0000000eU) 
                                  | (vlSelfRef.top__DOT__vgc_inst__DOT__spr_pri[6U] 
                                     << 0x0000000cU))));
    vlSelfRef.top__DOT__vgc_inst__DOT__spr_pri_flat 
        = ((0xff03ffffU & vlSelfRef.top__DOT__vgc_inst__DOT__spr_pri_flat) 
           | ((vlSelfRef.top__DOT__vgc_inst__DOT__spr_pri[11U] 
               << 0x00000016U) | ((vlSelfRef.top__DOT__vgc_inst__DOT__spr_pri[10U] 
                                   << 0x00000014U) 
                                  | (vlSelfRef.top__DOT__vgc_inst__DOT__spr_pri[9U] 
                                     << 0x00000012U))));
    vlSelfRef.top__DOT__vgc_inst__DOT__spr_pri_flat 
        = ((0xc0ffffffU & vlSelfRef.top__DOT__vgc_inst__DOT__spr_pri_flat) 
           | ((vlSelfRef.top__DOT__vgc_inst__DOT__spr_pri[14U] 
               << 0x0000001cU) | ((vlSelfRef.top__DOT__vgc_inst__DOT__spr_pri[13U] 
                                   << 0x0000001aU) 
                                  | (vlSelfRef.top__DOT__vgc_inst__DOT__spr_pri[12U] 
                                     << 0x00000018U))));
    vlSelfRef.top__DOT__vgc_inst__DOT__spr_pri_flat 
        = ((0x3fffffffU & vlSelfRef.top__DOT__vgc_inst__DOT__spr_pri_flat) 
           | (vlSelfRef.top__DOT__vgc_inst__DOT__spr_pri[15U] 
              << 0x0000001eU));
    vlSelfRef.top__DOT__vgc_inst__DOT__spr_enable_flat 
        = ((0xfff8U & (IData)(vlSelfRef.top__DOT__vgc_inst__DOT__spr_enable_flat)) 
           | ((vlSelfRef.top__DOT__vgc_inst__DOT__spr_enable[2U] 
               << 2U) | ((vlSelfRef.top__DOT__vgc_inst__DOT__spr_enable[1U] 
                          << 1U) | vlSelfRef.top__DOT__vgc_inst__DOT__spr_enable[0U])));
    vlSelfRef.top__DOT__vgc_inst__DOT__spr_enable_flat 
        = ((0xffc7U & (IData)(vlSelfRef.top__DOT__vgc_inst__DOT__spr_enable_flat)) 
           | ((vlSelfRef.top__DOT__vgc_inst__DOT__spr_enable[5U] 
               << 5U) | ((vlSelfRef.top__DOT__vgc_inst__DOT__spr_enable[4U] 
                          << 4U) | (vlSelfRef.top__DOT__vgc_inst__DOT__spr_enable[3U] 
                                    << 3U))));
    vlSelfRef.top__DOT__vgc_inst__DOT__spr_enable_flat 
        = ((0xfe3fU & (IData)(vlSelfRef.top__DOT__vgc_inst__DOT__spr_enable_flat)) 
           | ((vlSelfRef.top__DOT__vgc_inst__DOT__spr_enable[8U] 
               << 8U) | ((vlSelfRef.top__DOT__vgc_inst__DOT__spr_enable[7U] 
                          << 7U) | (vlSelfRef.top__DOT__vgc_inst__DOT__spr_enable[6U] 
                                    << 6U))));
    vlSelfRef.top__DOT__vgc_inst__DOT__spr_enable_flat 
        = ((0xf1ffU & (IData)(vlSelfRef.top__DOT__vgc_inst__DOT__spr_enable_flat)) 
           | ((vlSelfRef.top__DOT__vgc_inst__DOT__spr_enable[11U] 
               << 0x0000000bU) | ((vlSelfRef.top__DOT__vgc_inst__DOT__spr_enable[10U] 
                                   << 0x0000000aU) 
                                  | (vlSelfRef.top__DOT__vgc_inst__DOT__spr_enable[9U] 
                                     << 9U))));
    vlSelfRef.top__DOT__vgc_inst__DOT__spr_enable_flat 
        = ((0x8fffU & (IData)(vlSelfRef.top__DOT__vgc_inst__DOT__spr_enable_flat)) 
           | ((vlSelfRef.top__DOT__vgc_inst__DOT__spr_enable[14U] 
               << 0x0000000eU) | ((vlSelfRef.top__DOT__vgc_inst__DOT__spr_enable[13U] 
                                   << 0x0000000dU) 
                                  | (vlSelfRef.top__DOT__vgc_inst__DOT__spr_enable[12U] 
                                     << 0x0000000cU))));
    vlSelfRef.top__DOT__vgc_inst__DOT__spr_enable_flat 
        = ((0x7fffU & (IData)(vlSelfRef.top__DOT__vgc_inst__DOT__spr_enable_flat)) 
           | (vlSelfRef.top__DOT__vgc_inst__DOT__spr_enable[15U] 
              << 0x0000000fU));
    top__DOT__vgc_inst__DOT____Vlvbound_h6cff2040__0 
        = vlSelfRef.top__DOT__vgc_inst__DOT__spr_x[0U];
    vlSelfRef.top__DOT__vgc_inst__DOT__spr_x_flat[0U] 
        = ((0xfffffe00U & vlSelfRef.top__DOT__vgc_inst__DOT__spr_x_flat[0U]) 
           | (IData)(top__DOT__vgc_inst__DOT____Vlvbound_h6cff2040__0));
    top__DOT__vgc_inst__DOT____Vlvbound_h6cff2040__0 
        = vlSelfRef.top__DOT__vgc_inst__DOT__spr_x[1U];
    vlSelfRef.top__DOT__vgc_inst__DOT__spr_x_flat[0U] 
        = ((0xfffc01ffU & vlSelfRef.top__DOT__vgc_inst__DOT__spr_x_flat[0U]) 
           | ((IData)(top__DOT__vgc_inst__DOT____Vlvbound_h6cff2040__0) 
              << 9U));
    top__DOT__vgc_inst__DOT____Vlvbound_h6cff2040__0 
        = vlSelfRef.top__DOT__vgc_inst__DOT__spr_x[2U];
    vlSelfRef.top__DOT__vgc_inst__DOT__spr_x_flat[0U] 
        = ((0xf803ffffU & vlSelfRef.top__DOT__vgc_inst__DOT__spr_x_flat[0U]) 
           | ((IData)(top__DOT__vgc_inst__DOT____Vlvbound_h6cff2040__0) 
              << 0x00000012U));
    top__DOT__vgc_inst__DOT____Vlvbound_h6cff2040__0 
        = vlSelfRef.top__DOT__vgc_inst__DOT__spr_x[3U];
    vlSelfRef.top__DOT__vgc_inst__DOT__spr_x_flat[0U] 
        = ((0x07ffffffU & vlSelfRef.top__DOT__vgc_inst__DOT__spr_x_flat[0U]) 
           | ((IData)(top__DOT__vgc_inst__DOT____Vlvbound_h6cff2040__0) 
              << 0x0000001bU));
    vlSelfRef.top__DOT__vgc_inst__DOT__spr_x_flat[1U] 
        = ((0xfffffff0U & vlSelfRef.top__DOT__vgc_inst__DOT__spr_x_flat[1U]) 
           | ((IData)(top__DOT__vgc_inst__DOT____Vlvbound_h6cff2040__0) 
              >> 5U));
    top__DOT__vgc_inst__DOT____Vlvbound_h6cff2040__0 
        = vlSelfRef.top__DOT__vgc_inst__DOT__spr_x[4U];
    vlSelfRef.top__DOT__vgc_inst__DOT__spr_x_flat[1U] 
        = ((0xffffe00fU & vlSelfRef.top__DOT__vgc_inst__DOT__spr_x_flat[1U]) 
           | ((IData)(top__DOT__vgc_inst__DOT____Vlvbound_h6cff2040__0) 
              << 4U));
    top__DOT__vgc_inst__DOT____Vlvbound_h6cff2040__0 
        = vlSelfRef.top__DOT__vgc_inst__DOT__spr_x[5U];
    vlSelfRef.top__DOT__vgc_inst__DOT__spr_x_flat[1U] 
        = ((0xffc01fffU & vlSelfRef.top__DOT__vgc_inst__DOT__spr_x_flat[1U]) 
           | ((IData)(top__DOT__vgc_inst__DOT____Vlvbound_h6cff2040__0) 
              << 0x0000000dU));
    top__DOT__vgc_inst__DOT____Vlvbound_h6cff2040__0 
        = vlSelfRef.top__DOT__vgc_inst__DOT__spr_x[6U];
    vlSelfRef.top__DOT__vgc_inst__DOT__spr_x_flat[1U] 
        = ((0x803fffffU & vlSelfRef.top__DOT__vgc_inst__DOT__spr_x_flat[1U]) 
           | ((IData)(top__DOT__vgc_inst__DOT____Vlvbound_h6cff2040__0) 
              << 0x00000016U));
    top__DOT__vgc_inst__DOT____Vlvbound_h6cff2040__0 
        = vlSelfRef.top__DOT__vgc_inst__DOT__spr_x[7U];
    vlSelfRef.top__DOT__vgc_inst__DOT__spr_x_flat[1U] 
        = ((0x7fffffffU & vlSelfRef.top__DOT__vgc_inst__DOT__spr_x_flat[1U]) 
           | ((IData)(top__DOT__vgc_inst__DOT____Vlvbound_h6cff2040__0) 
              << 0x0000001fU));
    vlSelfRef.top__DOT__vgc_inst__DOT__spr_x_flat[2U] 
        = ((0xffffff00U & vlSelfRef.top__DOT__vgc_inst__DOT__spr_x_flat[2U]) 
           | ((IData)(top__DOT__vgc_inst__DOT____Vlvbound_h6cff2040__0) 
              >> 1U));
    top__DOT__vgc_inst__DOT____Vlvbound_h6cff2040__0 
        = vlSelfRef.top__DOT__vgc_inst__DOT__spr_x[8U];
    vlSelfRef.top__DOT__vgc_inst__DOT__spr_x_flat[2U] 
        = ((0xfffe00ffU & vlSelfRef.top__DOT__vgc_inst__DOT__spr_x_flat[2U]) 
           | ((IData)(top__DOT__vgc_inst__DOT____Vlvbound_h6cff2040__0) 
              << 8U));
    top__DOT__vgc_inst__DOT____Vlvbound_h6cff2040__0 
        = vlSelfRef.top__DOT__vgc_inst__DOT__spr_x[9U];
    vlSelfRef.top__DOT__vgc_inst__DOT__spr_x_flat[2U] 
        = ((0xfc01ffffU & vlSelfRef.top__DOT__vgc_inst__DOT__spr_x_flat[2U]) 
           | ((IData)(top__DOT__vgc_inst__DOT____Vlvbound_h6cff2040__0) 
              << 0x00000011U));
    top__DOT__vgc_inst__DOT____Vlvbound_h6cff2040__0 
        = vlSelfRef.top__DOT__vgc_inst__DOT__spr_x[10U];
    vlSelfRef.top__DOT__vgc_inst__DOT__spr_x_flat[2U] 
        = ((0x03ffffffU & vlSelfRef.top__DOT__vgc_inst__DOT__spr_x_flat[2U]) 
           | ((IData)(top__DOT__vgc_inst__DOT____Vlvbound_h6cff2040__0) 
              << 0x0000001aU));
    vlSelfRef.top__DOT__vgc_inst__DOT__spr_x_flat[3U] 
        = ((0xfffffff8U & vlSelfRef.top__DOT__vgc_inst__DOT__spr_x_flat[3U]) 
           | ((IData)(top__DOT__vgc_inst__DOT____Vlvbound_h6cff2040__0) 
              >> 6U));
    top__DOT__vgc_inst__DOT____Vlvbound_h6cff2040__0 
        = vlSelfRef.top__DOT__vgc_inst__DOT__spr_x[11U];
    vlSelfRef.top__DOT__vgc_inst__DOT__spr_x_flat[3U] 
        = ((0xfffff007U & vlSelfRef.top__DOT__vgc_inst__DOT__spr_x_flat[3U]) 
           | ((IData)(top__DOT__vgc_inst__DOT____Vlvbound_h6cff2040__0) 
              << 3U));
    top__DOT__vgc_inst__DOT____Vlvbound_h6cff2040__0 
        = vlSelfRef.top__DOT__vgc_inst__DOT__spr_x[12U];
    vlSelfRef.top__DOT__vgc_inst__DOT__spr_x_flat[3U] 
        = ((0xffe00fffU & vlSelfRef.top__DOT__vgc_inst__DOT__spr_x_flat[3U]) 
           | ((IData)(top__DOT__vgc_inst__DOT____Vlvbound_h6cff2040__0) 
              << 0x0000000cU));
    top__DOT__vgc_inst__DOT____Vlvbound_h6cff2040__0 
        = vlSelfRef.top__DOT__vgc_inst__DOT__spr_x[13U];
    vlSelfRef.top__DOT__vgc_inst__DOT__spr_x_flat[3U] 
        = ((0xc01fffffU & vlSelfRef.top__DOT__vgc_inst__DOT__spr_x_flat[3U]) 
           | ((IData)(top__DOT__vgc_inst__DOT____Vlvbound_h6cff2040__0) 
              << 0x00000015U));
    top__DOT__vgc_inst__DOT____Vlvbound_h6cff2040__0 
        = vlSelfRef.top__DOT__vgc_inst__DOT__spr_x[14U];
    vlSelfRef.top__DOT__vgc_inst__DOT__spr_x_flat[3U] 
        = ((0x3fffffffU & vlSelfRef.top__DOT__vgc_inst__DOT__spr_x_flat[3U]) 
           | ((IData)(top__DOT__vgc_inst__DOT____Vlvbound_h6cff2040__0) 
              << 0x0000001eU));
    vlSelfRef.top__DOT__vgc_inst__DOT__spr_x_flat[4U] 
        = ((0x0000ff80U & vlSelfRef.top__DOT__vgc_inst__DOT__spr_x_flat[4U]) 
           | (0x0000ffffU & ((IData)(top__DOT__vgc_inst__DOT____Vlvbound_h6cff2040__0) 
                             >> 2U)));
    top__DOT__vgc_inst__DOT____Vlvbound_h6cff2040__0 
        = vlSelfRef.top__DOT__vgc_inst__DOT__spr_x[15U];
    vlSelfRef.top__DOT__vgc_inst__DOT__spr_x_flat[4U] 
        = ((0x0000007fU & vlSelfRef.top__DOT__vgc_inst__DOT__spr_x_flat[4U]) 
           | (0x0000ffffU & ((IData)(top__DOT__vgc_inst__DOT____Vlvbound_h6cff2040__0) 
                             << 7U)));
    vlSelfRef.top__DOT__vgc_inst__DOT____Vcellinp__artist_inst__cmd_x0 
        = ((0x00000100U & (vlSelfRef.top__DOT__vgc_inst__DOT__regs[18U] 
                           << 8U)) | vlSelfRef.top__DOT__vgc_inst__DOT__regs[17U]);
    vlSelfRef.top__DOT__vgc_inst__DOT____Vcellinp__artist_inst__cmd_x1 
        = ((0x00000100U & (vlSelfRef.top__DOT__vgc_inst__DOT__regs[22U] 
                           << 8U)) | vlSelfRef.top__DOT__vgc_inst__DOT__regs[21U]);
    vlSelfRef.top__DOT__vgc_dbg_rdata = 0U;
    if (vlSelfRef.top__DOT__vgc_inst__DOT__dbg_char_sel) {
        vlSelfRef.top__DOT__vgc_dbg_rdata = vlSelfRef.top__DOT__vgc_inst__DOT__dbg_rd_latch;
    } else if (vlSelfRef.top__DOT__vgc_inst__DOT__dbg_color_sel) {
        vlSelfRef.top__DOT__vgc_dbg_rdata = vlSelfRef.top__DOT__vgc_inst__DOT__dbg_rd_latch;
    } else if (vlSelfRef.top__DOT__vgc_inst__DOT__dbg_vgc_sel) {
        vlSelfRef.top__DOT__vgc_dbg_rdata = ((0U == 
                                              (0x0000001fU 
                                               & (IData)(vlSelfRef.dbg_peek_addr)))
                                              ? (IData)(vlSelfRef.top__DOT__vgc_inst__DOT__mode)
                                              : ((1U 
                                                  == 
                                                  (0x0000001fU 
                                                   & (IData)(vlSelfRef.dbg_peek_addr)))
                                                  ? (IData)(vlSelfRef.top__DOT__vgc_inst__DOT__bg_color)
                                                  : 
                                                 ((2U 
                                                   == 
                                                   (0x0000001fU 
                                                    & (IData)(vlSelfRef.dbg_peek_addr)))
                                                   ? (IData)(vlSelfRef.top__DOT__vgc_inst__DOT__fg_color)
                                                   : 
                                                  ((3U 
                                                    == 
                                                    (0x0000001fU 
                                                     & (IData)(vlSelfRef.dbg_peek_addr)))
                                                    ? (IData)(vlSelfRef.top__DOT__vgc_inst__DOT__cursor_x)
                                                    : 
                                                   ((4U 
                                                     == 
                                                     (0x0000001fU 
                                                      & (IData)(vlSelfRef.dbg_peek_addr)))
                                                     ? (IData)(vlSelfRef.top__DOT__vgc_inst__DOT__cursor_y)
                                                     : 
                                                    ((7U 
                                                      == 
                                                      (0x0000001fU 
                                                       & (IData)(vlSelfRef.dbg_peek_addr)))
                                                      ? (IData)(vlSelfRef.top__DOT__vgc_inst__DOT__font_slot)
                                                      : 
                                                     ((0x0fU 
                                                       == 
                                                       (0x0000001fU 
                                                        & (IData)(vlSelfRef.dbg_peek_addr)))
                                                       ? (IData)(vlSelfRef.top__DOT__vgc_inst__DOT__char_in_reg)
                                                       : vlSelfRef.top__DOT__vgc_inst__DOT__regs
                                                      [
                                                      (0x0000001fU 
                                                       & (IData)(vlSelfRef.dbg_peek_addr))])))))));
    }
    vlSelfRef.__VdfgRegularize_hebeb780c_0_19 = ((((IData)(vlSelfRef.top__DOT__cpu_inst__DOT__D) 
                                                   << 3U) 
                                                  | ((IData)(vlSelfRef.top__DOT__cpu_inst__DOT__I) 
                                                     << 2U)) 
                                                 | (((IData)(vlSelfRef.top__DOT__cpu_inst__DOT__Z) 
                                                     << 1U) 
                                                    | (IData)(vlSelfRef.top__DOT__cpu_inst__DOT__C)));
    vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__s2_tile_pri 
        = ((0x00000020U & (IData)(vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__at_dout_b))
            ? 2U : 1U);
    vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__s2_sub_pal 
        = (0x0000000fU & (IData)(vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__at_dout_b));
    if (__VdlySet__top__DOT__vgc_inst__DOT__sprite_inst__DOT__spr_mem__DOT__mem__v0) {
        vlSelfRef.top__DOT__vgc_inst__DOT__sprite_inst__DOT__spr_mem__DOT__mem[__VdlyDim0__top__DOT__vgc_inst__DOT__sprite_inst__DOT__spr_mem__DOT__mem__v0] 
            = __VdlyVal__top__DOT__vgc_inst__DOT__sprite_inst__DOT__spr_mem__DOT__mem__v0;
    }
    vlSelfRef.top__DOT__vgc_inst__DOT__spr_a_addr = 0U;
    if (((IData)(vlSelfRef.top__DOT__blt_vgc_we) & 
         (4U == (IData)(vlSelfRef.top__DOT__blt_vgc_space)))) {
        vlSelfRef.top__DOT__vgc_inst__DOT__spr_a_we = 1U;
        vlSelfRef.top__DOT__vgc_inst__DOT__spr_a_din 
            = top__DOT__blt_vgc_wdata;
        vlSelfRef.top__DOT__vgc_inst__DOT__spr_a_addr 
            = (0x000007ffU & (IData)(vlSelfRef.top__DOT__blt_vgc_addr));
    } else if (vlSelfRef.top__DOT__vgc_inst__DOT__cmd_spr_we) {
        vlSelfRef.top__DOT__vgc_inst__DOT__spr_a_we = 1U;
        vlSelfRef.top__DOT__vgc_inst__DOT__spr_a_din 
            = vlSelfRef.top__DOT__vgc_inst__DOT__cmd_spr_din;
        vlSelfRef.top__DOT__vgc_inst__DOT__spr_a_addr 
            = vlSelfRef.top__DOT__vgc_inst__DOT__cmd_spr_addr;
    } else if (((IData)(vlSelfRef.top__DOT__blt_vgc_re) 
                & (4U == (IData)(vlSelfRef.top__DOT__blt_vgc_space)))) {
        vlSelfRef.top__DOT__vgc_inst__DOT__spr_a_addr 
            = (0x000007ffU & (IData)(vlSelfRef.top__DOT__blt_vgc_addr));
    } else if (vlSelfRef.top__DOT__vgc_inst__DOT__cmd_spr_re) {
        vlSelfRef.top__DOT__vgc_inst__DOT__spr_a_addr 
            = vlSelfRef.top__DOT__vgc_inst__DOT__cmd_spr_addr;
    }
    if (__VdlySet__top__DOT__vgc_inst__DOT__text_inst__DOT__color_mem__DOT__mem__v0) {
        vlSelfRef.top__DOT__vgc_inst__DOT__text_inst__DOT__color_mem__DOT__mem[__VdlyDim0__top__DOT__vgc_inst__DOT__text_inst__DOT__color_mem__DOT__mem__v0] 
            = __VdlyVal__top__DOT__vgc_inst__DOT__text_inst__DOT__color_mem__DOT__mem__v0;
    }
    if (__VdlySet__top__DOT__vgc_inst__DOT__text_inst__DOT__char_mem__DOT__mem__v0) {
        vlSelfRef.top__DOT__vgc_inst__DOT__text_inst__DOT__char_mem__DOT__mem[__VdlyDim0__top__DOT__vgc_inst__DOT__text_inst__DOT__char_mem__DOT__mem__v0] 
            = __VdlyVal__top__DOT__vgc_inst__DOT__text_inst__DOT__char_mem__DOT__mem__v0;
    }
    if (__VdlySet__top__DOT__vgc_inst__DOT__gfx_inst__DOT__gfx_mem__DOT__mem__v0) {
        vlSelfRef.top__DOT__vgc_inst__DOT__gfx_inst__DOT__gfx_mem__DOT__mem[__VdlyDim0__top__DOT__vgc_inst__DOT__gfx_inst__DOT__gfx_mem__DOT__mem__v0] 
            = __VdlyVal__top__DOT__vgc_inst__DOT__gfx_inst__DOT__gfx_mem__DOT__mem__v0;
    }
    vlSelfRef.top__DOT__vgc_inst__DOT__gfx_a_addr = 0U;
    if (((IData)(vlSelfRef.top__DOT__blt_vgc_we) & 
         (3U == (IData)(vlSelfRef.top__DOT__blt_vgc_space)))) {
        vlSelfRef.top__DOT__vgc_inst__DOT__gfx_a_we = 1U;
        vlSelfRef.top__DOT__vgc_inst__DOT__gfx_a_din 
            = (0x0000000fU & (IData)(top__DOT__blt_vgc_wdata));
        vlSelfRef.top__DOT__vgc_inst__DOT__gfx_a_addr 
            = vlSelfRef.top__DOT__blt_vgc_addr;
    } else if (vlSelfRef.top__DOT__vgc_inst__DOT__artist_gfx_we) {
        vlSelfRef.top__DOT__vgc_inst__DOT__gfx_a_we = 1U;
        vlSelfRef.top__DOT__vgc_inst__DOT__gfx_a_din 
            = vlSelfRef.top__DOT__vgc_inst__DOT__artist_gfx_wdata;
        vlSelfRef.top__DOT__vgc_inst__DOT__gfx_a_addr 
            = vlSelfRef.top__DOT__vgc_inst__DOT__artist_gfx_addr;
    } else if (vlSelfRef.top__DOT__vgc_inst__DOT__cmd_gfx_we) {
        vlSelfRef.top__DOT__vgc_inst__DOT__gfx_a_we = 1U;
        vlSelfRef.top__DOT__vgc_inst__DOT__gfx_a_din 
            = vlSelfRef.top__DOT__vgc_inst__DOT__cmd_gfx_din;
        vlSelfRef.top__DOT__vgc_inst__DOT__gfx_a_addr 
            = vlSelfRef.top__DOT__vgc_inst__DOT__cmd_gfx_addr;
    } else if (((IData)(vlSelfRef.top__DOT__blt_vgc_re) 
                & (3U == (IData)(vlSelfRef.top__DOT__blt_vgc_space)))) {
        vlSelfRef.top__DOT__vgc_inst__DOT__gfx_a_addr 
            = vlSelfRef.top__DOT__blt_vgc_addr;
    } else if (vlSelfRef.top__DOT__vgc_inst__DOT__artist_gfx_re) {
        vlSelfRef.top__DOT__vgc_inst__DOT__gfx_a_addr 
            = vlSelfRef.top__DOT__vgc_inst__DOT__artist_gfx_raddr;
    } else if (vlSelfRef.top__DOT__vgc_inst__DOT__cmd_gfx_re) {
        vlSelfRef.top__DOT__vgc_inst__DOT__gfx_a_addr 
            = vlSelfRef.top__DOT__vgc_inst__DOT__cmd_gfx_addr;
    }
    vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__td_we_a = 0U;
    vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__td_din_a = 0U;
    vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__td_addr_a = 0U;
    if ((0x00000010U & (IData)(vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__dma_state))) {
        if (vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__cmd_at_we) {
            vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__at_din_a 
                = vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__cmd_at_din;
        }
        if (vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__cmd_pal_we) {
            vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__pr_din_a 
                = vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__cmd_pal_din;
        }
        if (vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__cmd_nt_we) {
            vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__nt_din_a 
                = vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__cmd_nt_din;
        }
        if (vlSelfRef.top__DOT__vgc_inst__DOT__tile_blt_we) {
            vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__td_we_a = 1U;
            vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__td_din_a 
                = top__DOT__vgc_inst__DOT__tile_blt_wdata;
            vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__td_addr_a 
                = top__DOT__vgc_inst__DOT__tile_blt_addr;
        } else if (vlSelfRef.top__DOT__vgc_inst__DOT__tile_blt_re) {
            vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__td_addr_a 
                = top__DOT__vgc_inst__DOT__tile_blt_addr;
        }
    } else if ((8U & (IData)(vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__dma_state))) {
        if ((4U & (IData)(vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__dma_state))) {
            if ((2U & (IData)(vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__dma_state))) {
                if ((1U & (IData)(vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__dma_state))) {
                    if (vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__cmd_at_we) {
                        vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__at_din_a 
                            = vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__cmd_at_din;
                    }
                    if (vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__cmd_pal_we) {
                        vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__pr_din_a 
                            = vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__cmd_pal_din;
                    }
                    if (vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__cmd_nt_we) {
                        vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__nt_din_a 
                            = vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__cmd_nt_din;
                    }
                    if (vlSelfRef.top__DOT__vgc_inst__DOT__tile_blt_we) {
                        vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__td_we_a = 1U;
                        vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__td_din_a 
                            = top__DOT__vgc_inst__DOT__tile_blt_wdata;
                        vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__td_addr_a 
                            = top__DOT__vgc_inst__DOT__tile_blt_addr;
                    } else if (vlSelfRef.top__DOT__vgc_inst__DOT__tile_blt_re) {
                        vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__td_addr_a 
                            = top__DOT__vgc_inst__DOT__tile_blt_addr;
                    }
                }
            }
        } else {
            if ((1U & (~ ((IData)(vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__dma_state) 
                          >> 1U)))) {
                if ((1U & (IData)(vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__dma_state))) {
                    if ((0x0fa0U > (IData)(vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__dma_fill_addr))) {
                        vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__at_din_a = 0U;
                    }
                } else if (vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__dma_data_valid) {
                    vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__at_din_a 
                        = vlSelfRef.top__DOT__tile_dma_data;
                }
            }
            if ((2U & (IData)(vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__dma_state))) {
                if ((1U & (IData)(vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__dma_state))) {
                    if ((0x19U > (IData)(vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__dma_col_idx))) {
                        vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__nt_din_a 
                            = ((0x18U >= (IData)(vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__dma_col_idx))
                                ? vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__col_buffer
                               [vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__dma_col_idx]
                                : 0U);
                    }
                } else if ((0x03e8U > (IData)(vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__dma_count))) {
                    vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__nt_din_a 
                        = vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__dma_fill_val;
                }
            } else if ((1U & (IData)(vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__dma_state))) {
                if ((0x0fa0U > (IData)(vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__dma_fill_addr))) {
                    vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__nt_din_a = 0U;
                }
            }
        }
    } else {
        if ((4U & (IData)(vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__dma_state))) {
            if ((2U & (IData)(vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__dma_state))) {
                if ((1U & (IData)(vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__dma_state))) {
                    if (vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__dma_data_valid) {
                        vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__at_din_a 
                            = vlSelfRef.top__DOT__tile_dma_data;
                    }
                }
                if ((1U & (~ (IData)(vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__dma_state)))) {
                    if (vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__dma_data_valid) {
                        vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__nt_din_a 
                            = vlSelfRef.top__DOT__tile_dma_data;
                    }
                }
            } else if ((1U & (IData)(vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__dma_state))) {
                if (vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__dma_data_valid) {
                    vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__nt_din_a 
                        = vlSelfRef.top__DOT__tile_dma_data;
                }
            }
            if ((1U & (~ ((IData)(vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__dma_state) 
                          >> 1U)))) {
                if ((1U & (~ (IData)(vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__dma_state)))) {
                    if (((IData)(vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__dma_data_valid) 
                         & (2U == (IData)(vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__dma_pal_phase)))) {
                        vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__pr_din_a 
                            = ((0x00000f00U & ((IData)(vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__dma_pal_r) 
                                               << 4U)) 
                               | ((0x000000f0U & (IData)(vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__dma_pal_g)) 
                                  | (0x0000000fU & 
                                     ((IData)(vlSelfRef.top__DOT__tile_dma_data) 
                                      >> 4U))));
                    }
                }
            }
        } else {
            if ((2U & (IData)(vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__dma_state))) {
                if ((1U & (IData)(vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__dma_state))) {
                    if (vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__dma_data_valid) {
                        vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__at_din_a 
                            = vlSelfRef.top__DOT__tile_dma_data;
                    }
                }
                if ((1U & (~ (IData)(vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__dma_state)))) {
                    if (vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__dma_data_valid) {
                        vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__nt_din_a 
                            = vlSelfRef.top__DOT__tile_dma_data;
                    }
                }
            } else if ((1U & (~ (IData)(vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__dma_state)))) {
                if (vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__cmd_at_we) {
                    vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__at_din_a 
                        = vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__cmd_at_din;
                }
                if (vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__cmd_nt_we) {
                    vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__nt_din_a 
                        = vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__cmd_nt_din;
                }
            }
            if ((1U & (~ ((IData)(vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__dma_state) 
                          >> 1U)))) {
                if ((1U & (~ (IData)(vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__dma_state)))) {
                    if (vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__cmd_pal_we) {
                        vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__pr_din_a 
                            = vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__cmd_pal_din;
                    }
                }
            }
        }
        if ((1U & (~ ((IData)(vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__dma_state) 
                      >> 2U)))) {
            if ((1U & (~ ((IData)(vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__dma_state) 
                          >> 1U)))) {
                if ((1U & (IData)(vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__dma_state))) {
                    if (vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__dma_data_valid) {
                        vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__td_we_a = 1U;
                        vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__td_din_a 
                            = vlSelfRef.top__DOT__tile_dma_data;
                        vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__td_addr_a 
                            = (0x00007fffU & (IData)(vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__dma_dst));
                    }
                } else if (vlSelfRef.top__DOT__vgc_inst__DOT__tile_blt_we) {
                    vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__td_we_a = 1U;
                    vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__td_din_a 
                        = top__DOT__vgc_inst__DOT__tile_blt_wdata;
                    vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__td_addr_a 
                        = top__DOT__vgc_inst__DOT__tile_blt_addr;
                } else if (vlSelfRef.top__DOT__vgc_inst__DOT__tile_blt_re) {
                    vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__td_addr_a 
                        = top__DOT__vgc_inst__DOT__tile_blt_addr;
                }
            }
        }
    }
    vlSelfRef.top__DOT__vgc_inst__DOT__sprite_inst__DOT__spr_y[0U] 
        = (0x000000ffU & vlSelfRef.top__DOT__vgc_inst__DOT__spr_y_flat[0U]);
    vlSelfRef.top__DOT__vgc_inst__DOT__sprite_inst__DOT__spr_y[1U] 
        = (0x000000ffU & (vlSelfRef.top__DOT__vgc_inst__DOT__spr_y_flat[0U] 
                          >> 8U));
    vlSelfRef.top__DOT__vgc_inst__DOT__sprite_inst__DOT__spr_y[2U] 
        = (0x000000ffU & (vlSelfRef.top__DOT__vgc_inst__DOT__spr_y_flat[0U] 
                          >> 0x00000010U));
    vlSelfRef.top__DOT__vgc_inst__DOT__sprite_inst__DOT__spr_y[3U] 
        = (vlSelfRef.top__DOT__vgc_inst__DOT__spr_y_flat[0U] 
           >> 0x00000018U);
    vlSelfRef.top__DOT__vgc_inst__DOT__sprite_inst__DOT__spr_y[4U] 
        = (0x000000ffU & vlSelfRef.top__DOT__vgc_inst__DOT__spr_y_flat[1U]);
    vlSelfRef.top__DOT__vgc_inst__DOT__sprite_inst__DOT__spr_y[5U] 
        = (0x000000ffU & (vlSelfRef.top__DOT__vgc_inst__DOT__spr_y_flat[1U] 
                          >> 8U));
    vlSelfRef.top__DOT__vgc_inst__DOT__sprite_inst__DOT__spr_y[6U] 
        = (0x000000ffU & (vlSelfRef.top__DOT__vgc_inst__DOT__spr_y_flat[1U] 
                          >> 0x00000010U));
    vlSelfRef.top__DOT__vgc_inst__DOT__sprite_inst__DOT__spr_y[7U] 
        = (vlSelfRef.top__DOT__vgc_inst__DOT__spr_y_flat[1U] 
           >> 0x00000018U);
    vlSelfRef.top__DOT__vgc_inst__DOT__sprite_inst__DOT__spr_y[8U] 
        = (0x000000ffU & vlSelfRef.top__DOT__vgc_inst__DOT__spr_y_flat[2U]);
    vlSelfRef.top__DOT__vgc_inst__DOT__sprite_inst__DOT__spr_y[9U] 
        = (0x000000ffU & (vlSelfRef.top__DOT__vgc_inst__DOT__spr_y_flat[2U] 
                          >> 8U));
    vlSelfRef.top__DOT__vgc_inst__DOT__sprite_inst__DOT__spr_y[10U] 
        = (0x000000ffU & (vlSelfRef.top__DOT__vgc_inst__DOT__spr_y_flat[2U] 
                          >> 0x00000010U));
    vlSelfRef.top__DOT__vgc_inst__DOT__sprite_inst__DOT__spr_y[11U] 
        = (vlSelfRef.top__DOT__vgc_inst__DOT__spr_y_flat[2U] 
           >> 0x00000018U);
    vlSelfRef.top__DOT__vgc_inst__DOT__sprite_inst__DOT__spr_y[12U] 
        = (0x000000ffU & vlSelfRef.top__DOT__vgc_inst__DOT__spr_y_flat[3U]);
    vlSelfRef.top__DOT__vgc_inst__DOT__sprite_inst__DOT__spr_y[13U] 
        = (0x000000ffU & (vlSelfRef.top__DOT__vgc_inst__DOT__spr_y_flat[3U] 
                          >> 8U));
    vlSelfRef.top__DOT__vgc_inst__DOT__sprite_inst__DOT__spr_y[14U] 
        = (0x000000ffU & (vlSelfRef.top__DOT__vgc_inst__DOT__spr_y_flat[3U] 
                          >> 0x00000010U));
    vlSelfRef.top__DOT__vgc_inst__DOT__sprite_inst__DOT__spr_y[15U] 
        = (vlSelfRef.top__DOT__vgc_inst__DOT__spr_y_flat[3U] 
           >> 0x00000018U);
    vlSelfRef.top__DOT__vgc_inst__DOT__sprite_inst__DOT__spr_flip_h[0U] 
        = (1U & (IData)(vlSelfRef.top__DOT__vgc_inst__DOT__spr_flip_h_flat));
    vlSelfRef.top__DOT__vgc_inst__DOT__sprite_inst__DOT__spr_flip_h[1U] 
        = (1U & ((IData)(vlSelfRef.top__DOT__vgc_inst__DOT__spr_flip_h_flat) 
                 >> 1U));
    vlSelfRef.top__DOT__vgc_inst__DOT__sprite_inst__DOT__spr_flip_h[2U] 
        = (1U & ((IData)(vlSelfRef.top__DOT__vgc_inst__DOT__spr_flip_h_flat) 
                 >> 2U));
    vlSelfRef.top__DOT__vgc_inst__DOT__sprite_inst__DOT__spr_flip_h[3U] 
        = (1U & ((IData)(vlSelfRef.top__DOT__vgc_inst__DOT__spr_flip_h_flat) 
                 >> 3U));
    vlSelfRef.top__DOT__vgc_inst__DOT__sprite_inst__DOT__spr_flip_h[4U] 
        = (1U & ((IData)(vlSelfRef.top__DOT__vgc_inst__DOT__spr_flip_h_flat) 
                 >> 4U));
    vlSelfRef.top__DOT__vgc_inst__DOT__sprite_inst__DOT__spr_flip_h[5U] 
        = (1U & ((IData)(vlSelfRef.top__DOT__vgc_inst__DOT__spr_flip_h_flat) 
                 >> 5U));
    vlSelfRef.top__DOT__vgc_inst__DOT__sprite_inst__DOT__spr_flip_h[6U] 
        = (1U & ((IData)(vlSelfRef.top__DOT__vgc_inst__DOT__spr_flip_h_flat) 
                 >> 6U));
    vlSelfRef.top__DOT__vgc_inst__DOT__sprite_inst__DOT__spr_flip_h[7U] 
        = (1U & ((IData)(vlSelfRef.top__DOT__vgc_inst__DOT__spr_flip_h_flat) 
                 >> 7U));
    vlSelfRef.top__DOT__vgc_inst__DOT__sprite_inst__DOT__spr_flip_h[8U] 
        = (1U & ((IData)(vlSelfRef.top__DOT__vgc_inst__DOT__spr_flip_h_flat) 
                 >> 8U));
    vlSelfRef.top__DOT__vgc_inst__DOT__sprite_inst__DOT__spr_flip_h[9U] 
        = (1U & ((IData)(vlSelfRef.top__DOT__vgc_inst__DOT__spr_flip_h_flat) 
                 >> 9U));
    vlSelfRef.top__DOT__vgc_inst__DOT__sprite_inst__DOT__spr_flip_h[10U] 
        = (1U & ((IData)(vlSelfRef.top__DOT__vgc_inst__DOT__spr_flip_h_flat) 
                 >> 0x0aU));
    vlSelfRef.top__DOT__vgc_inst__DOT__sprite_inst__DOT__spr_flip_h[11U] 
        = (1U & ((IData)(vlSelfRef.top__DOT__vgc_inst__DOT__spr_flip_h_flat) 
                 >> 0x0bU));
    vlSelfRef.top__DOT__vgc_inst__DOT__sprite_inst__DOT__spr_flip_h[12U] 
        = (1U & ((IData)(vlSelfRef.top__DOT__vgc_inst__DOT__spr_flip_h_flat) 
                 >> 0x0cU));
    vlSelfRef.top__DOT__vgc_inst__DOT__sprite_inst__DOT__spr_flip_h[13U] 
        = (1U & ((IData)(vlSelfRef.top__DOT__vgc_inst__DOT__spr_flip_h_flat) 
                 >> 0x0dU));
    vlSelfRef.top__DOT__vgc_inst__DOT__sprite_inst__DOT__spr_flip_h[14U] 
        = (1U & ((IData)(vlSelfRef.top__DOT__vgc_inst__DOT__spr_flip_h_flat) 
                 >> 0x0eU));
    vlSelfRef.top__DOT__vgc_inst__DOT__sprite_inst__DOT__spr_flip_h[15U] 
        = (1U & ((IData)(vlSelfRef.top__DOT__vgc_inst__DOT__spr_flip_h_flat) 
                 >> 0x0fU));
    vlSelfRef.top__DOT__vgc_inst__DOT__sprite_inst__DOT__spr_flip_v[0U] 
        = (1U & (IData)(vlSelfRef.top__DOT__vgc_inst__DOT__spr_flip_v_flat));
    vlSelfRef.top__DOT__vgc_inst__DOT__sprite_inst__DOT__spr_flip_v[1U] 
        = (1U & ((IData)(vlSelfRef.top__DOT__vgc_inst__DOT__spr_flip_v_flat) 
                 >> 1U));
    vlSelfRef.top__DOT__vgc_inst__DOT__sprite_inst__DOT__spr_flip_v[2U] 
        = (1U & ((IData)(vlSelfRef.top__DOT__vgc_inst__DOT__spr_flip_v_flat) 
                 >> 2U));
    vlSelfRef.top__DOT__vgc_inst__DOT__sprite_inst__DOT__spr_flip_v[3U] 
        = (1U & ((IData)(vlSelfRef.top__DOT__vgc_inst__DOT__spr_flip_v_flat) 
                 >> 3U));
    vlSelfRef.top__DOT__vgc_inst__DOT__sprite_inst__DOT__spr_flip_v[4U] 
        = (1U & ((IData)(vlSelfRef.top__DOT__vgc_inst__DOT__spr_flip_v_flat) 
                 >> 4U));
    vlSelfRef.top__DOT__vgc_inst__DOT__sprite_inst__DOT__spr_flip_v[5U] 
        = (1U & ((IData)(vlSelfRef.top__DOT__vgc_inst__DOT__spr_flip_v_flat) 
                 >> 5U));
    vlSelfRef.top__DOT__vgc_inst__DOT__sprite_inst__DOT__spr_flip_v[6U] 
        = (1U & ((IData)(vlSelfRef.top__DOT__vgc_inst__DOT__spr_flip_v_flat) 
                 >> 6U));
    vlSelfRef.top__DOT__vgc_inst__DOT__sprite_inst__DOT__spr_flip_v[7U] 
        = (1U & ((IData)(vlSelfRef.top__DOT__vgc_inst__DOT__spr_flip_v_flat) 
                 >> 7U));
    vlSelfRef.top__DOT__vgc_inst__DOT__sprite_inst__DOT__spr_flip_v[8U] 
        = (1U & ((IData)(vlSelfRef.top__DOT__vgc_inst__DOT__spr_flip_v_flat) 
                 >> 8U));
    vlSelfRef.top__DOT__vgc_inst__DOT__sprite_inst__DOT__spr_flip_v[9U] 
        = (1U & ((IData)(vlSelfRef.top__DOT__vgc_inst__DOT__spr_flip_v_flat) 
                 >> 9U));
    vlSelfRef.top__DOT__vgc_inst__DOT__sprite_inst__DOT__spr_flip_v[10U] 
        = (1U & ((IData)(vlSelfRef.top__DOT__vgc_inst__DOT__spr_flip_v_flat) 
                 >> 0x0aU));
    vlSelfRef.top__DOT__vgc_inst__DOT__sprite_inst__DOT__spr_flip_v[11U] 
        = (1U & ((IData)(vlSelfRef.top__DOT__vgc_inst__DOT__spr_flip_v_flat) 
                 >> 0x0bU));
    vlSelfRef.top__DOT__vgc_inst__DOT__sprite_inst__DOT__spr_flip_v[12U] 
        = (1U & ((IData)(vlSelfRef.top__DOT__vgc_inst__DOT__spr_flip_v_flat) 
                 >> 0x0cU));
    vlSelfRef.top__DOT__vgc_inst__DOT__sprite_inst__DOT__spr_flip_v[13U] 
        = (1U & ((IData)(vlSelfRef.top__DOT__vgc_inst__DOT__spr_flip_v_flat) 
                 >> 0x0dU));
    vlSelfRef.top__DOT__vgc_inst__DOT__sprite_inst__DOT__spr_flip_v[14U] 
        = (1U & ((IData)(vlSelfRef.top__DOT__vgc_inst__DOT__spr_flip_v_flat) 
                 >> 0x0eU));
    vlSelfRef.top__DOT__vgc_inst__DOT__sprite_inst__DOT__spr_flip_v[15U] 
        = (1U & ((IData)(vlSelfRef.top__DOT__vgc_inst__DOT__spr_flip_v_flat) 
                 >> 0x0fU));
    vlSelfRef.top__DOT__vgc_inst__DOT__sprite_inst__DOT__spr_pri[0U] 
        = (3U & vlSelfRef.top__DOT__vgc_inst__DOT__spr_pri_flat);
    vlSelfRef.top__DOT__vgc_inst__DOT__sprite_inst__DOT__spr_pri[1U] 
        = (3U & (vlSelfRef.top__DOT__vgc_inst__DOT__spr_pri_flat 
                 >> 2U));
    vlSelfRef.top__DOT__vgc_inst__DOT__sprite_inst__DOT__spr_pri[2U] 
        = (3U & (vlSelfRef.top__DOT__vgc_inst__DOT__spr_pri_flat 
                 >> 4U));
    vlSelfRef.top__DOT__vgc_inst__DOT__sprite_inst__DOT__spr_pri[3U] 
        = (3U & (vlSelfRef.top__DOT__vgc_inst__DOT__spr_pri_flat 
                 >> 6U));
    vlSelfRef.top__DOT__vgc_inst__DOT__sprite_inst__DOT__spr_pri[4U] 
        = (3U & (vlSelfRef.top__DOT__vgc_inst__DOT__spr_pri_flat 
                 >> 8U));
    vlSelfRef.top__DOT__vgc_inst__DOT__sprite_inst__DOT__spr_pri[5U] 
        = (3U & (vlSelfRef.top__DOT__vgc_inst__DOT__spr_pri_flat 
                 >> 0x0aU));
    vlSelfRef.top__DOT__vgc_inst__DOT__sprite_inst__DOT__spr_pri[6U] 
        = (3U & (vlSelfRef.top__DOT__vgc_inst__DOT__spr_pri_flat 
                 >> 0x0cU));
    vlSelfRef.top__DOT__vgc_inst__DOT__sprite_inst__DOT__spr_pri[7U] 
        = (3U & (vlSelfRef.top__DOT__vgc_inst__DOT__spr_pri_flat 
                 >> 0x0eU));
    vlSelfRef.top__DOT__vgc_inst__DOT__sprite_inst__DOT__spr_pri[8U] 
        = (3U & (vlSelfRef.top__DOT__vgc_inst__DOT__spr_pri_flat 
                 >> 0x10U));
    vlSelfRef.top__DOT__vgc_inst__DOT__sprite_inst__DOT__spr_pri[9U] 
        = (3U & (vlSelfRef.top__DOT__vgc_inst__DOT__spr_pri_flat 
                 >> 0x12U));
    vlSelfRef.top__DOT__vgc_inst__DOT__sprite_inst__DOT__spr_pri[10U] 
        = (3U & (vlSelfRef.top__DOT__vgc_inst__DOT__spr_pri_flat 
                 >> 0x14U));
    vlSelfRef.top__DOT__vgc_inst__DOT__sprite_inst__DOT__spr_pri[11U] 
        = (3U & (vlSelfRef.top__DOT__vgc_inst__DOT__spr_pri_flat 
                 >> 0x16U));
    vlSelfRef.top__DOT__vgc_inst__DOT__sprite_inst__DOT__spr_pri[12U] 
        = (3U & (vlSelfRef.top__DOT__vgc_inst__DOT__spr_pri_flat 
                 >> 0x18U));
    vlSelfRef.top__DOT__vgc_inst__DOT__sprite_inst__DOT__spr_pri[13U] 
        = (3U & (vlSelfRef.top__DOT__vgc_inst__DOT__spr_pri_flat 
                 >> 0x1aU));
    vlSelfRef.top__DOT__vgc_inst__DOT__sprite_inst__DOT__spr_pri[14U] 
        = (3U & (vlSelfRef.top__DOT__vgc_inst__DOT__spr_pri_flat 
                 >> 0x1cU));
    vlSelfRef.top__DOT__vgc_inst__DOT__sprite_inst__DOT__spr_pri[15U] 
        = (vlSelfRef.top__DOT__vgc_inst__DOT__spr_pri_flat 
           >> 0x1eU);
    vlSelfRef.top__DOT__vgc_inst__DOT__sprite_inst__DOT__spr_enable[0U] 
        = (1U & (IData)(vlSelfRef.top__DOT__vgc_inst__DOT__spr_enable_flat));
    vlSelfRef.top__DOT__vgc_inst__DOT__sprite_inst__DOT__spr_enable[1U] 
        = (1U & ((IData)(vlSelfRef.top__DOT__vgc_inst__DOT__spr_enable_flat) 
                 >> 1U));
    vlSelfRef.top__DOT__vgc_inst__DOT__sprite_inst__DOT__spr_enable[2U] 
        = (1U & ((IData)(vlSelfRef.top__DOT__vgc_inst__DOT__spr_enable_flat) 
                 >> 2U));
    vlSelfRef.top__DOT__vgc_inst__DOT__sprite_inst__DOT__spr_enable[3U] 
        = (1U & ((IData)(vlSelfRef.top__DOT__vgc_inst__DOT__spr_enable_flat) 
                 >> 3U));
    vlSelfRef.top__DOT__vgc_inst__DOT__sprite_inst__DOT__spr_enable[4U] 
        = (1U & ((IData)(vlSelfRef.top__DOT__vgc_inst__DOT__spr_enable_flat) 
                 >> 4U));
    vlSelfRef.top__DOT__vgc_inst__DOT__sprite_inst__DOT__spr_enable[5U] 
        = (1U & ((IData)(vlSelfRef.top__DOT__vgc_inst__DOT__spr_enable_flat) 
                 >> 5U));
    vlSelfRef.top__DOT__vgc_inst__DOT__sprite_inst__DOT__spr_enable[6U] 
        = (1U & ((IData)(vlSelfRef.top__DOT__vgc_inst__DOT__spr_enable_flat) 
                 >> 6U));
    vlSelfRef.top__DOT__vgc_inst__DOT__sprite_inst__DOT__spr_enable[7U] 
        = (1U & ((IData)(vlSelfRef.top__DOT__vgc_inst__DOT__spr_enable_flat) 
                 >> 7U));
    vlSelfRef.top__DOT__vgc_inst__DOT__sprite_inst__DOT__spr_enable[8U] 
        = (1U & ((IData)(vlSelfRef.top__DOT__vgc_inst__DOT__spr_enable_flat) 
                 >> 8U));
    vlSelfRef.top__DOT__vgc_inst__DOT__sprite_inst__DOT__spr_enable[9U] 
        = (1U & ((IData)(vlSelfRef.top__DOT__vgc_inst__DOT__spr_enable_flat) 
                 >> 9U));
    vlSelfRef.top__DOT__vgc_inst__DOT__sprite_inst__DOT__spr_enable[10U] 
        = (1U & ((IData)(vlSelfRef.top__DOT__vgc_inst__DOT__spr_enable_flat) 
                 >> 0x0aU));
    vlSelfRef.top__DOT__vgc_inst__DOT__sprite_inst__DOT__spr_enable[11U] 
        = (1U & ((IData)(vlSelfRef.top__DOT__vgc_inst__DOT__spr_enable_flat) 
                 >> 0x0bU));
    vlSelfRef.top__DOT__vgc_inst__DOT__sprite_inst__DOT__spr_enable[12U] 
        = (1U & ((IData)(vlSelfRef.top__DOT__vgc_inst__DOT__spr_enable_flat) 
                 >> 0x0cU));
    vlSelfRef.top__DOT__vgc_inst__DOT__sprite_inst__DOT__spr_enable[13U] 
        = (1U & ((IData)(vlSelfRef.top__DOT__vgc_inst__DOT__spr_enable_flat) 
                 >> 0x0dU));
    vlSelfRef.top__DOT__vgc_inst__DOT__sprite_inst__DOT__spr_enable[14U] 
        = (1U & ((IData)(vlSelfRef.top__DOT__vgc_inst__DOT__spr_enable_flat) 
                 >> 0x0eU));
    vlSelfRef.top__DOT__vgc_inst__DOT__sprite_inst__DOT__spr_enable[15U] 
        = (1U & ((IData)(vlSelfRef.top__DOT__vgc_inst__DOT__spr_enable_flat) 
                 >> 0x0fU));
    vlSelfRef.top__DOT__vgc_inst__DOT__sprite_inst__DOT__spr_x[0U] 
        = (0x000001ffU & vlSelfRef.top__DOT__vgc_inst__DOT__spr_x_flat[0U]);
    vlSelfRef.top__DOT__vgc_inst__DOT__sprite_inst__DOT__spr_x[1U] 
        = (0x000001ffU & (vlSelfRef.top__DOT__vgc_inst__DOT__spr_x_flat[0U] 
                          >> 9U));
    vlSelfRef.top__DOT__vgc_inst__DOT__sprite_inst__DOT__spr_x[2U] 
        = (0x000001ffU & (vlSelfRef.top__DOT__vgc_inst__DOT__spr_x_flat[0U] 
                          >> 0x00000012U));
    vlSelfRef.top__DOT__vgc_inst__DOT__sprite_inst__DOT__spr_x[3U] 
        = (0x000001ffU & ((vlSelfRef.top__DOT__vgc_inst__DOT__spr_x_flat[1U] 
                           << 5U) | (vlSelfRef.top__DOT__vgc_inst__DOT__spr_x_flat[0U] 
                                     >> 0x0000001bU)));
    vlSelfRef.top__DOT__vgc_inst__DOT__sprite_inst__DOT__spr_x[4U] 
        = (0x000001ffU & (vlSelfRef.top__DOT__vgc_inst__DOT__spr_x_flat[1U] 
                          >> 4U));
    vlSelfRef.top__DOT__vgc_inst__DOT__sprite_inst__DOT__spr_x[5U] 
        = (0x000001ffU & (vlSelfRef.top__DOT__vgc_inst__DOT__spr_x_flat[1U] 
                          >> 0x0000000dU));
    vlSelfRef.top__DOT__vgc_inst__DOT__sprite_inst__DOT__spr_x[6U] 
        = (0x000001ffU & (vlSelfRef.top__DOT__vgc_inst__DOT__spr_x_flat[1U] 
                          >> 0x00000016U));
    vlSelfRef.top__DOT__vgc_inst__DOT__sprite_inst__DOT__spr_x[7U] 
        = (0x000001ffU & ((vlSelfRef.top__DOT__vgc_inst__DOT__spr_x_flat[2U] 
                           << 1U) | (vlSelfRef.top__DOT__vgc_inst__DOT__spr_x_flat[1U] 
                                     >> 0x0000001fU)));
    vlSelfRef.top__DOT__vgc_inst__DOT__sprite_inst__DOT__spr_x[8U] 
        = (0x000001ffU & (vlSelfRef.top__DOT__vgc_inst__DOT__spr_x_flat[2U] 
                          >> 8U));
    vlSelfRef.top__DOT__vgc_inst__DOT__sprite_inst__DOT__spr_x[9U] 
        = (0x000001ffU & (vlSelfRef.top__DOT__vgc_inst__DOT__spr_x_flat[2U] 
                          >> 0x00000011U));
    vlSelfRef.top__DOT__vgc_inst__DOT__sprite_inst__DOT__spr_x[10U] 
        = (0x000001ffU & ((vlSelfRef.top__DOT__vgc_inst__DOT__spr_x_flat[3U] 
                           << 6U) | (vlSelfRef.top__DOT__vgc_inst__DOT__spr_x_flat[2U] 
                                     >> 0x0000001aU)));
    vlSelfRef.top__DOT__vgc_inst__DOT__sprite_inst__DOT__spr_x[11U] 
        = (0x000001ffU & (vlSelfRef.top__DOT__vgc_inst__DOT__spr_x_flat[3U] 
                          >> 3U));
    vlSelfRef.top__DOT__vgc_inst__DOT__sprite_inst__DOT__spr_x[12U] 
        = (0x000001ffU & (vlSelfRef.top__DOT__vgc_inst__DOT__spr_x_flat[3U] 
                          >> 0x0000000cU));
    vlSelfRef.top__DOT__vgc_inst__DOT__sprite_inst__DOT__spr_x[13U] 
        = (0x000001ffU & (vlSelfRef.top__DOT__vgc_inst__DOT__spr_x_flat[3U] 
                          >> 0x00000015U));
    vlSelfRef.top__DOT__vgc_inst__DOT__sprite_inst__DOT__spr_x[14U] 
        = (0x000001ffU & ((vlSelfRef.top__DOT__vgc_inst__DOT__spr_x_flat[4U] 
                           << 2U) | (vlSelfRef.top__DOT__vgc_inst__DOT__spr_x_flat[3U] 
                                     >> 0x0000001eU)));
    vlSelfRef.top__DOT__vgc_inst__DOT__sprite_inst__DOT__spr_x[15U] 
        = (0x000001ffU & (vlSelfRef.top__DOT__vgc_inst__DOT__spr_x_flat[4U] 
                          >> 7U));
    vlSelfRef.top__DOT__vgc_inst__DOT__copper_enabled 
        = __Vdly__top__DOT__vgc_inst__DOT__copper_enabled;
    vlSelfRef.top__DOT__vgc_inst__DOT__copper_inst__DOT__copper_pos[0U] 
        = (0x0001ffffU & vlSelfRef.top__DOT__vgc_inst__DOT__copper_pos_flat[0U]);
    vlSelfRef.top__DOT__vgc_inst__DOT__copper_inst__DOT__copper_pos[1U] 
        = (0x0001ffffU & ((vlSelfRef.top__DOT__vgc_inst__DOT__copper_pos_flat[1U] 
                           << 0x0000000fU) | (vlSelfRef.top__DOT__vgc_inst__DOT__copper_pos_flat[0U] 
                                              >> 0x00000011U)));
    vlSelfRef.top__DOT__vgc_inst__DOT__copper_inst__DOT__copper_pos[2U] 
        = (0x0001ffffU & (vlSelfRef.top__DOT__vgc_inst__DOT__copper_pos_flat[1U] 
                          >> 2U));
    vlSelfRef.top__DOT__vgc_inst__DOT__copper_inst__DOT__copper_pos[3U] 
        = (0x0001ffffU & ((vlSelfRef.top__DOT__vgc_inst__DOT__copper_pos_flat[2U] 
                           << 0x0000000dU) | (vlSelfRef.top__DOT__vgc_inst__DOT__copper_pos_flat[1U] 
                                              >> 0x00000013U)));
    vlSelfRef.top__DOT__vgc_inst__DOT__copper_inst__DOT__copper_pos[4U] 
        = (0x0001ffffU & (vlSelfRef.top__DOT__vgc_inst__DOT__copper_pos_flat[2U] 
                          >> 4U));
    vlSelfRef.top__DOT__vgc_inst__DOT__copper_inst__DOT__copper_pos[5U] 
        = (0x0001ffffU & ((vlSelfRef.top__DOT__vgc_inst__DOT__copper_pos_flat[3U] 
                           << 0x0000000bU) | (vlSelfRef.top__DOT__vgc_inst__DOT__copper_pos_flat[2U] 
                                              >> 0x00000015U)));
    vlSelfRef.top__DOT__vgc_inst__DOT__copper_inst__DOT__copper_pos[6U] 
        = (0x0001ffffU & (vlSelfRef.top__DOT__vgc_inst__DOT__copper_pos_flat[3U] 
                          >> 6U));
    vlSelfRef.top__DOT__vgc_inst__DOT__copper_inst__DOT__copper_pos[7U] 
        = (0x0001ffffU & ((vlSelfRef.top__DOT__vgc_inst__DOT__copper_pos_flat[4U] 
                           << 9U) | (vlSelfRef.top__DOT__vgc_inst__DOT__copper_pos_flat[3U] 
                                     >> 0x00000017U)));
    vlSelfRef.top__DOT__vgc_inst__DOT__copper_inst__DOT__copper_pos[8U] 
        = (0x0001ffffU & (vlSelfRef.top__DOT__vgc_inst__DOT__copper_pos_flat[4U] 
                          >> 8U));
    vlSelfRef.top__DOT__vgc_inst__DOT__copper_inst__DOT__copper_pos[9U] 
        = (0x0001ffffU & ((vlSelfRef.top__DOT__vgc_inst__DOT__copper_pos_flat[5U] 
                           << 7U) | (vlSelfRef.top__DOT__vgc_inst__DOT__copper_pos_flat[4U] 
                                     >> 0x00000019U)));
    vlSelfRef.top__DOT__vgc_inst__DOT__copper_inst__DOT__copper_pos[10U] 
        = (0x0001ffffU & (vlSelfRef.top__DOT__vgc_inst__DOT__copper_pos_flat[5U] 
                          >> 0x0000000aU));
    vlSelfRef.top__DOT__vgc_inst__DOT__copper_inst__DOT__copper_pos[11U] 
        = (0x0001ffffU & ((vlSelfRef.top__DOT__vgc_inst__DOT__copper_pos_flat[6U] 
                           << 5U) | (vlSelfRef.top__DOT__vgc_inst__DOT__copper_pos_flat[5U] 
                                     >> 0x0000001bU)));
    vlSelfRef.top__DOT__vgc_inst__DOT__copper_inst__DOT__copper_pos[12U] 
        = (0x0001ffffU & (vlSelfRef.top__DOT__vgc_inst__DOT__copper_pos_flat[6U] 
                          >> 0x0000000cU));
    vlSelfRef.top__DOT__vgc_inst__DOT__copper_inst__DOT__copper_pos[13U] 
        = (0x0001ffffU & ((vlSelfRef.top__DOT__vgc_inst__DOT__copper_pos_flat[7U] 
                           << 3U) | (vlSelfRef.top__DOT__vgc_inst__DOT__copper_pos_flat[6U] 
                                     >> 0x0000001dU)));
    vlSelfRef.top__DOT__vgc_inst__DOT__copper_inst__DOT__copper_pos[14U] 
        = (0x0001ffffU & (vlSelfRef.top__DOT__vgc_inst__DOT__copper_pos_flat[7U] 
                          >> 0x0000000eU));
    vlSelfRef.top__DOT__vgc_inst__DOT__copper_inst__DOT__copper_pos[15U] 
        = (0x0001ffffU & ((vlSelfRef.top__DOT__vgc_inst__DOT__copper_pos_flat[8U] 
                           << 1U) | (vlSelfRef.top__DOT__vgc_inst__DOT__copper_pos_flat[7U] 
                                     >> 0x0000001fU)));
    vlSelfRef.top__DOT__vgc_inst__DOT__copper_inst__DOT__copper_pos[16U] 
        = (0x0001ffffU & ((vlSelfRef.top__DOT__vgc_inst__DOT__copper_pos_flat[9U] 
                           << 0x00000010U) | (vlSelfRef.top__DOT__vgc_inst__DOT__copper_pos_flat[8U] 
                                              >> 0x00000010U)));
    vlSelfRef.top__DOT__vgc_inst__DOT__copper_inst__DOT__copper_pos[17U] 
        = (0x0001ffffU & (vlSelfRef.top__DOT__vgc_inst__DOT__copper_pos_flat[9U] 
                          >> 1U));
    vlSelfRef.top__DOT__vgc_inst__DOT__copper_inst__DOT__copper_pos[18U] 
        = (0x0001ffffU & ((vlSelfRef.top__DOT__vgc_inst__DOT__copper_pos_flat[10U] 
                           << 0x0000000eU) | (vlSelfRef.top__DOT__vgc_inst__DOT__copper_pos_flat[9U] 
                                              >> 0x00000012U)));
    vlSelfRef.top__DOT__vgc_inst__DOT__copper_inst__DOT__copper_pos[19U] 
        = (0x0001ffffU & (vlSelfRef.top__DOT__vgc_inst__DOT__copper_pos_flat[10U] 
                          >> 3U));
    vlSelfRef.top__DOT__vgc_inst__DOT__copper_inst__DOT__copper_pos[20U] 
        = (0x0001ffffU & ((vlSelfRef.top__DOT__vgc_inst__DOT__copper_pos_flat[11U] 
                           << 0x0000000cU) | (vlSelfRef.top__DOT__vgc_inst__DOT__copper_pos_flat[10U] 
                                              >> 0x00000014U)));
    vlSelfRef.top__DOT__vgc_inst__DOT__copper_inst__DOT__copper_pos[21U] 
        = (0x0001ffffU & (vlSelfRef.top__DOT__vgc_inst__DOT__copper_pos_flat[11U] 
                          >> 5U));
    vlSelfRef.top__DOT__vgc_inst__DOT__copper_inst__DOT__copper_pos[22U] 
        = (0x0001ffffU & ((vlSelfRef.top__DOT__vgc_inst__DOT__copper_pos_flat[12U] 
                           << 0x0000000aU) | (vlSelfRef.top__DOT__vgc_inst__DOT__copper_pos_flat[11U] 
                                              >> 0x00000016U)));
    vlSelfRef.top__DOT__vgc_inst__DOT__copper_inst__DOT__copper_pos[23U] 
        = (0x0001ffffU & (vlSelfRef.top__DOT__vgc_inst__DOT__copper_pos_flat[12U] 
                          >> 7U));
    vlSelfRef.top__DOT__vgc_inst__DOT__copper_inst__DOT__copper_pos[24U] 
        = (0x0001ffffU & ((vlSelfRef.top__DOT__vgc_inst__DOT__copper_pos_flat[13U] 
                           << 8U) | (vlSelfRef.top__DOT__vgc_inst__DOT__copper_pos_flat[12U] 
                                     >> 0x00000018U)));
    vlSelfRef.top__DOT__vgc_inst__DOT__copper_inst__DOT__copper_pos[25U] 
        = (0x0001ffffU & (vlSelfRef.top__DOT__vgc_inst__DOT__copper_pos_flat[13U] 
                          >> 9U));
    vlSelfRef.top__DOT__vgc_inst__DOT__copper_inst__DOT__copper_pos[26U] 
        = (0x0001ffffU & ((vlSelfRef.top__DOT__vgc_inst__DOT__copper_pos_flat[14U] 
                           << 6U) | (vlSelfRef.top__DOT__vgc_inst__DOT__copper_pos_flat[13U] 
                                     >> 0x0000001aU)));
    vlSelfRef.top__DOT__vgc_inst__DOT__copper_inst__DOT__copper_pos[27U] 
        = (0x0001ffffU & (vlSelfRef.top__DOT__vgc_inst__DOT__copper_pos_flat[14U] 
                          >> 0x0000000bU));
    vlSelfRef.top__DOT__vgc_inst__DOT__copper_inst__DOT__copper_pos[28U] 
        = (0x0001ffffU & ((vlSelfRef.top__DOT__vgc_inst__DOT__copper_pos_flat[15U] 
                           << 4U) | (vlSelfRef.top__DOT__vgc_inst__DOT__copper_pos_flat[14U] 
                                     >> 0x0000001cU)));
    vlSelfRef.top__DOT__vgc_inst__DOT__copper_inst__DOT__copper_pos[29U] 
        = (0x0001ffffU & (vlSelfRef.top__DOT__vgc_inst__DOT__copper_pos_flat[15U] 
                          >> 0x0000000dU));
    vlSelfRef.top__DOT__vgc_inst__DOT__copper_inst__DOT__copper_pos[30U] 
        = (0x0001ffffU & ((vlSelfRef.top__DOT__vgc_inst__DOT__copper_pos_flat[16U] 
                           << 2U) | (vlSelfRef.top__DOT__vgc_inst__DOT__copper_pos_flat[15U] 
                                     >> 0x0000001eU)));
    vlSelfRef.top__DOT__vgc_inst__DOT__copper_inst__DOT__copper_pos[31U] 
        = (vlSelfRef.top__DOT__vgc_inst__DOT__copper_pos_flat[16U] 
           >> 0x0000000fU);
    vlSelfRef.top__DOT__vgc_inst__DOT__copper_inst__DOT__copper_reg[0U] 
        = (0x000000ffU & vlSelfRef.top__DOT__vgc_inst__DOT__copper_reg_flat[0U]);
    vlSelfRef.top__DOT__vgc_inst__DOT__copper_inst__DOT__copper_reg[1U] 
        = (0x000000ffU & (vlSelfRef.top__DOT__vgc_inst__DOT__copper_reg_flat[0U] 
                          >> 8U));
    vlSelfRef.top__DOT__vgc_inst__DOT__copper_inst__DOT__copper_reg[2U] 
        = (0x000000ffU & (vlSelfRef.top__DOT__vgc_inst__DOT__copper_reg_flat[0U] 
                          >> 0x00000010U));
    vlSelfRef.top__DOT__vgc_inst__DOT__copper_inst__DOT__copper_reg[3U] 
        = (vlSelfRef.top__DOT__vgc_inst__DOT__copper_reg_flat[0U] 
           >> 0x00000018U);
    vlSelfRef.top__DOT__vgc_inst__DOT__copper_inst__DOT__copper_reg[4U] 
        = (0x000000ffU & vlSelfRef.top__DOT__vgc_inst__DOT__copper_reg_flat[1U]);
    vlSelfRef.top__DOT__vgc_inst__DOT__copper_inst__DOT__copper_reg[5U] 
        = (0x000000ffU & (vlSelfRef.top__DOT__vgc_inst__DOT__copper_reg_flat[1U] 
                          >> 8U));
    vlSelfRef.top__DOT__vgc_inst__DOT__copper_inst__DOT__copper_reg[6U] 
        = (0x000000ffU & (vlSelfRef.top__DOT__vgc_inst__DOT__copper_reg_flat[1U] 
                          >> 0x00000010U));
    vlSelfRef.top__DOT__vgc_inst__DOT__copper_inst__DOT__copper_reg[7U] 
        = (vlSelfRef.top__DOT__vgc_inst__DOT__copper_reg_flat[1U] 
           >> 0x00000018U);
    vlSelfRef.top__DOT__vgc_inst__DOT__copper_inst__DOT__copper_reg[8U] 
        = (0x000000ffU & vlSelfRef.top__DOT__vgc_inst__DOT__copper_reg_flat[2U]);
    vlSelfRef.top__DOT__vgc_inst__DOT__copper_inst__DOT__copper_reg[9U] 
        = (0x000000ffU & (vlSelfRef.top__DOT__vgc_inst__DOT__copper_reg_flat[2U] 
                          >> 8U));
    vlSelfRef.top__DOT__vgc_inst__DOT__copper_inst__DOT__copper_reg[10U] 
        = (0x000000ffU & (vlSelfRef.top__DOT__vgc_inst__DOT__copper_reg_flat[2U] 
                          >> 0x00000010U));
    vlSelfRef.top__DOT__vgc_inst__DOT__copper_inst__DOT__copper_reg[11U] 
        = (vlSelfRef.top__DOT__vgc_inst__DOT__copper_reg_flat[2U] 
           >> 0x00000018U);
    vlSelfRef.top__DOT__vgc_inst__DOT__copper_inst__DOT__copper_reg[12U] 
        = (0x000000ffU & vlSelfRef.top__DOT__vgc_inst__DOT__copper_reg_flat[3U]);
    vlSelfRef.top__DOT__vgc_inst__DOT__copper_inst__DOT__copper_reg[13U] 
        = (0x000000ffU & (vlSelfRef.top__DOT__vgc_inst__DOT__copper_reg_flat[3U] 
                          >> 8U));
    vlSelfRef.top__DOT__vgc_inst__DOT__copper_inst__DOT__copper_reg[14U] 
        = (0x000000ffU & (vlSelfRef.top__DOT__vgc_inst__DOT__copper_reg_flat[3U] 
                          >> 0x00000010U));
    vlSelfRef.top__DOT__vgc_inst__DOT__copper_inst__DOT__copper_reg[15U] 
        = (vlSelfRef.top__DOT__vgc_inst__DOT__copper_reg_flat[3U] 
           >> 0x00000018U);
    vlSelfRef.top__DOT__vgc_inst__DOT__copper_inst__DOT__copper_reg[16U] 
        = (0x000000ffU & vlSelfRef.top__DOT__vgc_inst__DOT__copper_reg_flat[4U]);
    vlSelfRef.top__DOT__vgc_inst__DOT__copper_inst__DOT__copper_reg[17U] 
        = (0x000000ffU & (vlSelfRef.top__DOT__vgc_inst__DOT__copper_reg_flat[4U] 
                          >> 8U));
    vlSelfRef.top__DOT__vgc_inst__DOT__copper_inst__DOT__copper_reg[18U] 
        = (0x000000ffU & (vlSelfRef.top__DOT__vgc_inst__DOT__copper_reg_flat[4U] 
                          >> 0x00000010U));
    vlSelfRef.top__DOT__vgc_inst__DOT__copper_inst__DOT__copper_reg[19U] 
        = (vlSelfRef.top__DOT__vgc_inst__DOT__copper_reg_flat[4U] 
           >> 0x00000018U);
    vlSelfRef.top__DOT__vgc_inst__DOT__copper_inst__DOT__copper_reg[20U] 
        = (0x000000ffU & vlSelfRef.top__DOT__vgc_inst__DOT__copper_reg_flat[5U]);
    vlSelfRef.top__DOT__vgc_inst__DOT__copper_inst__DOT__copper_reg[21U] 
        = (0x000000ffU & (vlSelfRef.top__DOT__vgc_inst__DOT__copper_reg_flat[5U] 
                          >> 8U));
    vlSelfRef.top__DOT__vgc_inst__DOT__copper_inst__DOT__copper_reg[22U] 
        = (0x000000ffU & (vlSelfRef.top__DOT__vgc_inst__DOT__copper_reg_flat[5U] 
                          >> 0x00000010U));
    vlSelfRef.top__DOT__vgc_inst__DOT__copper_inst__DOT__copper_reg[23U] 
        = (vlSelfRef.top__DOT__vgc_inst__DOT__copper_reg_flat[5U] 
           >> 0x00000018U);
    vlSelfRef.top__DOT__vgc_inst__DOT__copper_inst__DOT__copper_reg[24U] 
        = (0x000000ffU & vlSelfRef.top__DOT__vgc_inst__DOT__copper_reg_flat[6U]);
    vlSelfRef.top__DOT__vgc_inst__DOT__copper_inst__DOT__copper_reg[25U] 
        = (0x000000ffU & (vlSelfRef.top__DOT__vgc_inst__DOT__copper_reg_flat[6U] 
                          >> 8U));
    vlSelfRef.top__DOT__vgc_inst__DOT__copper_inst__DOT__copper_reg[26U] 
        = (0x000000ffU & (vlSelfRef.top__DOT__vgc_inst__DOT__copper_reg_flat[6U] 
                          >> 0x00000010U));
    vlSelfRef.top__DOT__vgc_inst__DOT__copper_inst__DOT__copper_reg[27U] 
        = (vlSelfRef.top__DOT__vgc_inst__DOT__copper_reg_flat[6U] 
           >> 0x00000018U);
    vlSelfRef.top__DOT__vgc_inst__DOT__copper_inst__DOT__copper_reg[28U] 
        = (0x000000ffU & vlSelfRef.top__DOT__vgc_inst__DOT__copper_reg_flat[7U]);
    vlSelfRef.top__DOT__vgc_inst__DOT__copper_inst__DOT__copper_reg[29U] 
        = (0x000000ffU & (vlSelfRef.top__DOT__vgc_inst__DOT__copper_reg_flat[7U] 
                          >> 8U));
    vlSelfRef.top__DOT__vgc_inst__DOT__copper_inst__DOT__copper_reg[30U] 
        = (0x000000ffU & (vlSelfRef.top__DOT__vgc_inst__DOT__copper_reg_flat[7U] 
                          >> 0x00000010U));
    vlSelfRef.top__DOT__vgc_inst__DOT__copper_inst__DOT__copper_reg[31U] 
        = (vlSelfRef.top__DOT__vgc_inst__DOT__copper_reg_flat[7U] 
           >> 0x00000018U);
    vlSelfRef.top__DOT__vgc_inst__DOT__copper_inst__DOT__copper_val[0U] 
        = (0x000000ffU & vlSelfRef.top__DOT__vgc_inst__DOT__copper_val_flat[0U]);
    vlSelfRef.top__DOT__vgc_inst__DOT__copper_inst__DOT__copper_val[1U] 
        = (0x000000ffU & (vlSelfRef.top__DOT__vgc_inst__DOT__copper_val_flat[0U] 
                          >> 8U));
    vlSelfRef.top__DOT__vgc_inst__DOT__copper_inst__DOT__copper_val[2U] 
        = (0x000000ffU & (vlSelfRef.top__DOT__vgc_inst__DOT__copper_val_flat[0U] 
                          >> 0x00000010U));
    vlSelfRef.top__DOT__vgc_inst__DOT__copper_inst__DOT__copper_val[3U] 
        = (vlSelfRef.top__DOT__vgc_inst__DOT__copper_val_flat[0U] 
           >> 0x00000018U);
    vlSelfRef.top__DOT__vgc_inst__DOT__copper_inst__DOT__copper_val[4U] 
        = (0x000000ffU & vlSelfRef.top__DOT__vgc_inst__DOT__copper_val_flat[1U]);
    vlSelfRef.top__DOT__vgc_inst__DOT__copper_inst__DOT__copper_val[5U] 
        = (0x000000ffU & (vlSelfRef.top__DOT__vgc_inst__DOT__copper_val_flat[1U] 
                          >> 8U));
    vlSelfRef.top__DOT__vgc_inst__DOT__copper_inst__DOT__copper_val[6U] 
        = (0x000000ffU & (vlSelfRef.top__DOT__vgc_inst__DOT__copper_val_flat[1U] 
                          >> 0x00000010U));
    vlSelfRef.top__DOT__vgc_inst__DOT__copper_inst__DOT__copper_val[7U] 
        = (vlSelfRef.top__DOT__vgc_inst__DOT__copper_val_flat[1U] 
           >> 0x00000018U);
    vlSelfRef.top__DOT__vgc_inst__DOT__copper_inst__DOT__copper_val[8U] 
        = (0x000000ffU & vlSelfRef.top__DOT__vgc_inst__DOT__copper_val_flat[2U]);
    vlSelfRef.top__DOT__vgc_inst__DOT__copper_inst__DOT__copper_val[9U] 
        = (0x000000ffU & (vlSelfRef.top__DOT__vgc_inst__DOT__copper_val_flat[2U] 
                          >> 8U));
    vlSelfRef.top__DOT__vgc_inst__DOT__copper_inst__DOT__copper_val[10U] 
        = (0x000000ffU & (vlSelfRef.top__DOT__vgc_inst__DOT__copper_val_flat[2U] 
                          >> 0x00000010U));
    vlSelfRef.top__DOT__vgc_inst__DOT__copper_inst__DOT__copper_val[11U] 
        = (vlSelfRef.top__DOT__vgc_inst__DOT__copper_val_flat[2U] 
           >> 0x00000018U);
    vlSelfRef.top__DOT__vgc_inst__DOT__copper_inst__DOT__copper_val[12U] 
        = (0x000000ffU & vlSelfRef.top__DOT__vgc_inst__DOT__copper_val_flat[3U]);
    vlSelfRef.top__DOT__vgc_inst__DOT__copper_inst__DOT__copper_val[13U] 
        = (0x000000ffU & (vlSelfRef.top__DOT__vgc_inst__DOT__copper_val_flat[3U] 
                          >> 8U));
    vlSelfRef.top__DOT__vgc_inst__DOT__copper_inst__DOT__copper_val[14U] 
        = (0x000000ffU & (vlSelfRef.top__DOT__vgc_inst__DOT__copper_val_flat[3U] 
                          >> 0x00000010U));
    vlSelfRef.top__DOT__vgc_inst__DOT__copper_inst__DOT__copper_val[15U] 
        = (vlSelfRef.top__DOT__vgc_inst__DOT__copper_val_flat[3U] 
           >> 0x00000018U);
    vlSelfRef.top__DOT__vgc_inst__DOT__copper_inst__DOT__copper_val[16U] 
        = (0x000000ffU & vlSelfRef.top__DOT__vgc_inst__DOT__copper_val_flat[4U]);
    vlSelfRef.top__DOT__vgc_inst__DOT__copper_inst__DOT__copper_val[17U] 
        = (0x000000ffU & (vlSelfRef.top__DOT__vgc_inst__DOT__copper_val_flat[4U] 
                          >> 8U));
    vlSelfRef.top__DOT__vgc_inst__DOT__copper_inst__DOT__copper_val[18U] 
        = (0x000000ffU & (vlSelfRef.top__DOT__vgc_inst__DOT__copper_val_flat[4U] 
                          >> 0x00000010U));
    vlSelfRef.top__DOT__vgc_inst__DOT__copper_inst__DOT__copper_val[19U] 
        = (vlSelfRef.top__DOT__vgc_inst__DOT__copper_val_flat[4U] 
           >> 0x00000018U);
    vlSelfRef.top__DOT__vgc_inst__DOT__copper_inst__DOT__copper_val[20U] 
        = (0x000000ffU & vlSelfRef.top__DOT__vgc_inst__DOT__copper_val_flat[5U]);
    vlSelfRef.top__DOT__vgc_inst__DOT__copper_inst__DOT__copper_val[21U] 
        = (0x000000ffU & (vlSelfRef.top__DOT__vgc_inst__DOT__copper_val_flat[5U] 
                          >> 8U));
    vlSelfRef.top__DOT__vgc_inst__DOT__copper_inst__DOT__copper_val[22U] 
        = (0x000000ffU & (vlSelfRef.top__DOT__vgc_inst__DOT__copper_val_flat[5U] 
                          >> 0x00000010U));
    vlSelfRef.top__DOT__vgc_inst__DOT__copper_inst__DOT__copper_val[23U] 
        = (vlSelfRef.top__DOT__vgc_inst__DOT__copper_val_flat[5U] 
           >> 0x00000018U);
    vlSelfRef.top__DOT__vgc_inst__DOT__copper_inst__DOT__copper_val[24U] 
        = (0x000000ffU & vlSelfRef.top__DOT__vgc_inst__DOT__copper_val_flat[6U]);
    vlSelfRef.top__DOT__vgc_inst__DOT__copper_inst__DOT__copper_val[25U] 
        = (0x000000ffU & (vlSelfRef.top__DOT__vgc_inst__DOT__copper_val_flat[6U] 
                          >> 8U));
    vlSelfRef.top__DOT__vgc_inst__DOT__copper_inst__DOT__copper_val[26U] 
        = (0x000000ffU & (vlSelfRef.top__DOT__vgc_inst__DOT__copper_val_flat[6U] 
                          >> 0x00000010U));
    vlSelfRef.top__DOT__vgc_inst__DOT__copper_inst__DOT__copper_val[27U] 
        = (vlSelfRef.top__DOT__vgc_inst__DOT__copper_val_flat[6U] 
           >> 0x00000018U);
    vlSelfRef.top__DOT__vgc_inst__DOT__copper_inst__DOT__copper_val[28U] 
        = (0x000000ffU & vlSelfRef.top__DOT__vgc_inst__DOT__copper_val_flat[7U]);
    vlSelfRef.top__DOT__vgc_inst__DOT__copper_inst__DOT__copper_val[29U] 
        = (0x000000ffU & (vlSelfRef.top__DOT__vgc_inst__DOT__copper_val_flat[7U] 
                          >> 8U));
    vlSelfRef.top__DOT__vgc_inst__DOT__copper_inst__DOT__copper_val[30U] 
        = (0x000000ffU & (vlSelfRef.top__DOT__vgc_inst__DOT__copper_val_flat[7U] 
                          >> 0x00000010U));
    vlSelfRef.top__DOT__vgc_inst__DOT__copper_inst__DOT__copper_val[31U] 
        = (vlSelfRef.top__DOT__vgc_inst__DOT__copper_val_flat[7U] 
           >> 0x00000018U);
    vlSelfRef.top__DOT__vgc_inst__DOT__copper_index 
        = __Vdly__top__DOT__vgc_inst__DOT__copper_index;
    vlSelfRef.top__DOT__vgc_inst__DOT__copper_count 
        = __Vdly__top__DOT__vgc_inst__DOT__copper_count;
    vlSelfRef.top__DOT__vgc_inst__DOT__h_count = __Vdly__top__DOT__vgc_inst__DOT__h_count;
    vlSelfRef.top__DOT__vgc_inst__DOT__v_count = __Vdly__top__DOT__vgc_inst__DOT__v_count;
    vlSelfRef.top__DOT__cpu_inst__DOT__P = (0x00000030U 
                                            | (((IData)(vlSelfRef.top__DOT__cpu_inst__DOT__N) 
                                                << 7U) 
                                               | (((IData)(vlSelfRef.top__DOT__cpu_inst__DOT__V) 
                                                   << 6U) 
                                                  | (IData)(vlSelfRef.__VdfgRegularize_hebeb780c_0_19))));
    vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT____VdfgRegularize_hb33eefcd_0_1 
        = (0x0000ffffU & VL_MODDIV_III(16, (0x000007ffU 
                                            & ((0x000001ffU 
                                                & ((IData)(vlSelfRef.top__DOT__vgc_inst__DOT__h_count) 
                                                   >> 1U)) 
                                               + (IData)(vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__scroll_x))), 
                                       (0x000007ffU 
                                        & VL_SHIFTL_III(11,11,32, 
                                                        (((0U 
                                                           == (IData)(vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__mirror_mode)) 
                                                          | (3U 
                                                             == (IData)(vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__mirror_mode)))
                                                          ? 0x0140U
                                                          : 0x0280U), 1U))));
    vlSelfRef.top__DOT__vgc_inst__DOT__visible = ((0x0280U 
                                                   > (IData)(vlSelfRef.top__DOT__vgc_inst__DOT__h_count)) 
                                                  & (0x01e0U 
                                                     > (IData)(vlSelfRef.top__DOT__vgc_inst__DOT__v_count)));
    top__DOT__vgc_inst__DOT__text_line = (0x000003ffU 
                                          & (((IData)(vlSelfRef.top__DOT__vgc_inst__DOT__v_count) 
                                              - (IData)(0x00000028U)) 
                                             >> 1U));
    vlSelfRef.top__DOT__vgc_inst__DOT__text_row = (0x0000001fU 
                                                   & ((IData)(top__DOT__vgc_inst__DOT__text_line) 
                                                      >> 3U));
    vlSelfRef.top__DOT__vgc_inst__DOT__font_line = 
        (7U & (IData)(top__DOT__vgc_inst__DOT__text_line));
    vlSelfRef.top__DOT__vgc_inst__DOT__real_row = (0x0000001fU 
                                                   & ((IData)(vlSelfRef.top__DOT__vgc_inst__DOT__text_row) 
                                                      + (IData)(vlSelfRef.top__DOT__vgc_inst__DOT__scroll_offset)));
    if ((0x19U <= (IData)(vlSelfRef.top__DOT__vgc_inst__DOT__real_row))) {
        vlSelfRef.top__DOT__vgc_inst__DOT__real_row 
            = (0x0000001fU & ((IData)(vlSelfRef.top__DOT__vgc_inst__DOT__real_row) 
                              - (IData)(0x19U)));
    }
    vlSelfRef.top__DOT__vgc_inst__DOT__gfx_y = (0x000000ffU 
                                                & (IData)(top__DOT__vgc_inst__DOT__text_line));
    vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__world_y 
        = (0x000003ffU & VL_MODDIV_III(10, (0x000003ffU 
                                            & ((0x000000ffU 
                                                & (((IData)(vlSelfRef.top__DOT__vgc_inst__DOT__v_count) 
                                                    - (IData)(0x00000028U)) 
                                                   >> 1U)) 
                                               + (IData)(vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__scroll_y))), 
                                       (((1U == (IData)(vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__mirror_mode)) 
                                         | (3U == (IData)(vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__mirror_mode)))
                                         ? 0x00c8U : 0x0190U)));
    vlSelfRef.dbg_cpu_flags = vlSelfRef.top__DOT__cpu_inst__DOT__P;
    vlSelfRef.__VdfgRegularize_hebeb780c_0_10 = (((IData)(vlSelfRef.top__DOT__cpu_inst__DOT__ADD) 
                                                  << 8U) 
                                                 | (IData)(vlSelfRef.top__DOT__cpu_inst__DOT__ABL));
}
