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
    CData/*7:0*/ top__DOT__vgc_dbg_rdata;
    top__DOT__vgc_dbg_rdata = 0;
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
    vlSelfRef.top__DOT__vgc_inst__DOT__dbg_char_sel 
        = ((0xaa00U <= (IData)(vlSelfRef.dbg_peek_addr)) 
           & (0xb1cfU >= (IData)(vlSelfRef.dbg_peek_addr)));
    vlSelfRef.top__DOT__vgc_inst__DOT__dbg_color_sel 
        = ((0xb1d0U <= (IData)(vlSelfRef.dbg_peek_addr)) 
           & (0xb99fU >= (IData)(vlSelfRef.dbg_peek_addr)));
    vlSelfRef.top__DOT__vgc_inst__DOT__dbg_vgc_sel 
        = ((0xa000U <= (IData)(vlSelfRef.dbg_peek_addr)) 
           & (0xa01fU >= (IData)(vlSelfRef.dbg_peek_addr)));
    vlSelfRef.top__DOT____Vcellinp__cpu_inst__RDY = 
        ((~ (IData)(vlSelfRef.dbg_pause)) & ((0U == (IData)(vlSelfRef.top__DOT__blt_inst__DOT__state)) 
                                             | (6U 
                                                == (IData)(vlSelfRef.top__DOT__blt_inst__DOT__state))));
    top__DOT__vgc_dbg_rdata = 0U;
    if (vlSelfRef.top__DOT__vgc_inst__DOT__dbg_char_sel) {
        top__DOT__vgc_dbg_rdata = ((0x07cfU >= (0x000007ffU 
                                                & ((IData)(vlSelfRef.dbg_peek_addr) 
                                                   - (IData)(0x0200U))))
                                    ? vlSelfRef.top__DOT__vgc_inst__DOT__char_ram
                                   [(0x000007ffU & 
                                     ((IData)(vlSelfRef.dbg_peek_addr) 
                                      - (IData)(0x0200U)))]
                                    : 0U);
    } else if (vlSelfRef.top__DOT__vgc_inst__DOT__dbg_color_sel) {
        top__DOT__vgc_dbg_rdata = ((0x07cfU >= (0x000007ffU 
                                                & ((IData)(vlSelfRef.dbg_peek_addr) 
                                                   - (IData)(0x01d0U))))
                                    ? vlSelfRef.top__DOT__vgc_inst__DOT__color_ram
                                   [(0x000007ffU & 
                                     ((IData)(vlSelfRef.dbg_peek_addr) 
                                      - (IData)(0x01d0U)))]
                                    : 0U);
    } else if (vlSelfRef.top__DOT__vgc_inst__DOT__dbg_vgc_sel) {
        top__DOT__vgc_dbg_rdata = ((0U == (0x0000001fU 
                                           & (IData)(vlSelfRef.dbg_peek_addr)))
                                    ? (IData)(vlSelfRef.top__DOT__vgc_inst__DOT__mode)
                                    : ((1U == (0x0000001fU 
                                               & (IData)(vlSelfRef.dbg_peek_addr)))
                                        ? (IData)(vlSelfRef.top__DOT__vgc_inst__DOT__bg_color)
                                        : ((2U == (0x0000001fU 
                                                   & (IData)(vlSelfRef.dbg_peek_addr)))
                                            ? (IData)(vlSelfRef.top__DOT__vgc_inst__DOT__fg_color)
                                            : ((3U 
                                                == 
                                                (0x0000001fU 
                                                 & (IData)(vlSelfRef.dbg_peek_addr)))
                                                ? (IData)(vlSelfRef.top__DOT__vgc_inst__DOT__cursor_x)
                                                : (
                                                   (4U 
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
                                    ? (IData)(top__DOT__vgc_dbg_rdata)
                                    : ((0xc000U <= (IData)(vlSelfRef.dbg_peek_addr))
                                        ? ((IData)(vlSelfRef.top__DOT__ext_rom_active)
                                            ? vlSelfRef.top__DOT__ext_rom
                                           [(0x00003fffU 
                                             & (IData)(vlSelfRef.dbg_peek_addr))]
                                            : vlSelfRef.top__DOT__basic_rom
                                           [(0x00003fffU 
                                             & (IData)(vlSelfRef.dbg_peek_addr))])
                                        : vlSelfRef.top__DOT__ram
                                       [vlSelfRef.dbg_peek_addr]))
                                : 0U);
    vlSelfRef.top__DOT__cpu_inst__DOT__IR = ((IData)(vlSelfRef.__VdfgRegularize_hebeb780c_0_3)
                                              ? 0U : 
                                             ((IData)(vlSelfRef.top__DOT__cpu_inst__DOT__IRHOLD_valid)
                                               ? (IData)(vlSelfRef.top__DOT__cpu_inst__DOT__IRHOLD)
                                               : (IData)(vlSelfRef.top__DOT__cpu_inst__DOT__DIMUX)));
    vlSelfRef.__VdfgRegularize_hebeb780c_0_0 = (((IData)(vlSelfRef.top__DOT__cpu_inst__DOT__DIMUX) 
                                                 << 8U) 
                                                | (IData)(vlSelfRef.top__DOT__cpu_inst__DOT__ADD));
    if ((1U & (IData)(vlSelfRef.top__DOT__cpu_inst__DOT__state))) {
        __VdfgRegularize_hebeb780c_0_18 = 0U;
        __VdfgRegularize_hebeb780c_0_16 = vlSelfRef.top__DOT__cpu_inst__DOT__DIMUX;
    } else {
        __VdfgRegularize_hebeb780c_0_18 = vlSelfRef.top__DOT__cpu_inst__DOT__DIMUX;
        __VdfgRegularize_hebeb780c_0_16 = 0U;
    }
    __VdfgRegularize_hebeb780c_0_17 = ((2U & (IData)(vlSelfRef.top__DOT__cpu_inst__DOT__state))
                                        ? 0U : (IData)(vlSelfRef.top__DOT__cpu_inst__DOT__DIMUX));
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
    vlSelfRef.top__DOT__vgc_inst__DOT__tile_reg_sel 
        = ((0xa0c0U <= (IData)(vlSelfRef.top__DOT__cpu_addr)) 
           & (0xa0dfU >= (IData)(vlSelfRef.top__DOT__cpu_addr)));
    vlSelfRef.top__DOT__xmc_win_enabled = (1U & (vlSelfRef.top__DOT__xmc_regs[22U] 
                                                 >> 
                                                 (3U 
                                                  & ((IData)(vlSelfRef.top__DOT__cpu_addr) 
                                                     >> 8U))));
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
    vlSelfRef.top__DOT__cpu_inst__DOT__ALU__DOT__temp_BI 
        = (0x000000ffU & ((8U & (IData)(vlSelfRef.top__DOT__cpu_inst__DOT__alu_op))
                           ? ((4U & (IData)(vlSelfRef.top__DOT__cpu_inst__DOT__alu_op))
                               ? 0U : (IData)(top__DOT__cpu_inst__DOT__ALU__DOT__temp_logic))
                           : ((4U & (IData)(vlSelfRef.top__DOT__cpu_inst__DOT__alu_op))
                               ? (~ (IData)(top__DOT__cpu_inst__DOT__BI))
                               : (IData)(top__DOT__cpu_inst__DOT__BI))));
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

void Vtop___024root___eval_ico(Vtop___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vtop___024root___eval_ico\n"); );
    Vtop__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
    // Body
    if ((1ULL & vlSelfRef.__VicoTriggered[0U])) {
        Vtop___024root___ico_sequent__TOP__0(vlSelf);
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
                                                    ((((IData)(vlSelfRef.rst) 
                                                       & (~ (IData)(vlSelfRef.__Vtrigprevexpr___TOP__rst__0))) 
                                                      << 1U) 
                                                     | ((IData)(vlSelfRef.clk) 
                                                        & (~ (IData)(vlSelfRef.__Vtrigprevexpr___TOP__clk__0))))));
    vlSelfRef.__Vtrigprevexpr___TOP__clk__0 = vlSelfRef.clk;
    vlSelfRef.__Vtrigprevexpr___TOP__rst__0 = vlSelfRef.rst;
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

void Vtop___024root___nba_sequent__TOP__0(Vtop___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vtop___024root___nba_sequent__TOP__0\n"); );
    Vtop__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
    // Locals
    CData/*0:0*/ top__DOT__blt_vgc_re;
    top__DOT__blt_vgc_re = 0;
    CData/*7:0*/ top__DOT__blt_vgc_rdata;
    top__DOT__blt_vgc_rdata = 0;
    CData/*7:0*/ top__DOT__vgc_dbg_rdata;
    top__DOT__vgc_dbg_rdata = 0;
    IData/*23:0*/ top__DOT__blt_inst__DOT__src_addr;
    top__DOT__blt_inst__DOT__src_addr = 0;
    IData/*23:0*/ top__DOT__blt_inst__DOT__dst_addr;
    top__DOT__blt_inst__DOT__dst_addr = 0;
    CData/*1:0*/ top__DOT__vgc_inst__DOT__tile_opaque;
    top__DOT__vgc_inst__DOT__tile_opaque = 0;
    CData/*4:0*/ top__DOT__vgc_inst__DOT__text_row;
    top__DOT__vgc_inst__DOT__text_row = 0;
    CData/*4:0*/ top__DOT__vgc_inst__DOT__real_row;
    top__DOT__vgc_inst__DOT__real_row = 0;
    CData/*2:0*/ top__DOT__vgc_inst__DOT__font_line;
    top__DOT__vgc_inst__DOT__font_line = 0;
    SData/*9:0*/ top__DOT__vgc_inst__DOT__text_line;
    top__DOT__vgc_inst__DOT__text_line = 0;
    CData/*7:0*/ top__DOT__vgc_inst__DOT__cur_char;
    top__DOT__vgc_inst__DOT__cur_char = 0;
    CData/*3:0*/ top__DOT__vgc_inst__DOT__cur_fg;
    top__DOT__vgc_inst__DOT__cur_fg = 0;
    CData/*3:0*/ top__DOT__vgc_inst__DOT__cur_gfx;
    top__DOT__vgc_inst__DOT__cur_gfx = 0;
    CData/*7:0*/ top__DOT__vgc_inst__DOT__font_byte;
    top__DOT__vgc_inst__DOT__font_byte = 0;
    CData/*0:0*/ top__DOT__vgc_inst__DOT__pixel_on;
    top__DOT__vgc_inst__DOT__pixel_on = 0;
    SData/*11:0*/ top__DOT__vgc_inst__DOT__text_pixel;
    top__DOT__vgc_inst__DOT__text_pixel = 0;
    SData/*11:0*/ top__DOT__vgc_inst__DOT__gfx_pixel;
    top__DOT__vgc_inst__DOT__gfx_pixel = 0;
    CData/*3:0*/ top__DOT__vgc_inst__DOT__spr_pixel;
    top__DOT__vgc_inst__DOT__spr_pixel = 0;
    CData/*1:0*/ top__DOT__vgc_inst__DOT__spr_pixel_pri;
    top__DOT__vgc_inst__DOT__spr_pixel_pri = 0;
    CData/*0:0*/ top__DOT__vgc_inst__DOT__spr_pixel_hit;
    top__DOT__vgc_inst__DOT__spr_pixel_hit = 0;
    SData/*9:0*/ top__DOT__vgc_inst__DOT__tile_inst__DOT__world_y;
    top__DOT__vgc_inst__DOT__tile_inst__DOT__world_y = 0;
    CData/*3:0*/ top__DOT__vgc_inst__DOT__tile_inst__DOT__pix_in_x;
    top__DOT__vgc_inst__DOT__tile_inst__DOT__pix_in_x = 0;
    CData/*3:0*/ top__DOT__vgc_inst__DOT__tile_inst__DOT__pix_in_y;
    top__DOT__vgc_inst__DOT__tile_inst__DOT__pix_in_y = 0;
    CData/*7:0*/ top__DOT__vgc_inst__DOT__tile_inst__DOT__cur_tile;
    top__DOT__vgc_inst__DOT__tile_inst__DOT__cur_tile = 0;
    CData/*7:0*/ top__DOT__vgc_inst__DOT__tile_inst__DOT__cur_attr;
    top__DOT__vgc_inst__DOT__tile_inst__DOT__cur_attr = 0;
    CData/*3:0*/ top__DOT__vgc_inst__DOT__tile_inst__DOT__sample_x;
    top__DOT__vgc_inst__DOT__tile_inst__DOT__sample_x = 0;
    CData/*3:0*/ top__DOT__vgc_inst__DOT__tile_inst__DOT__sample_y;
    top__DOT__vgc_inst__DOT__tile_inst__DOT__sample_y = 0;
    CData/*7:0*/ top__DOT__vgc_inst__DOT__tile_inst__DOT__packed_byte;
    top__DOT__vgc_inst__DOT__tile_inst__DOT__packed_byte = 0;
    CData/*3:0*/ top__DOT__vgc_inst__DOT__tile_inst__DOT__color_idx;
    top__DOT__vgc_inst__DOT__tile_inst__DOT__color_idx = 0;
    SData/*15:0*/ top__DOT__vgc_inst__DOT__tile_inst__DOT____VdfgRegularize_hb33eefcd_0_1;
    top__DOT__vgc_inst__DOT__tile_inst__DOT____VdfgRegularize_hb33eefcd_0_1 = 0;
    SData/*10:0*/ top__DOT__vgc_inst__DOT__tile_inst__DOT____VdfgRegularize_hb33eefcd_0_2;
    top__DOT__vgc_inst__DOT__tile_inst__DOT____VdfgRegularize_hb33eefcd_0_2 = 0;
    SData/*9:0*/ top__DOT__vgc_inst__DOT__tile_inst__DOT____VdfgRegularize_hb33eefcd_0_3;
    top__DOT__vgc_inst__DOT__tile_inst__DOT____VdfgRegularize_hb33eefcd_0_3 = 0;
    SData/*11:0*/ top__DOT__vgc_inst__DOT__tile_inst__DOT____VdfgRegularize_hb33eefcd_0_4;
    top__DOT__vgc_inst__DOT__tile_inst__DOT____VdfgRegularize_hb33eefcd_0_4 = 0;
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
    CData/*4:0*/ __Vfunc_top__DOT__vgc_inst__DOT__screen_addr__7__rr;
    __Vfunc_top__DOT__vgc_inst__DOT__screen_addr__7__rr = 0;
    SData/*10:0*/ __Vfunc_top__DOT__vgc_inst__DOT__screen_addr__8__Vfuncout;
    __Vfunc_top__DOT__vgc_inst__DOT__screen_addr__8__Vfuncout = 0;
    CData/*6:0*/ __Vfunc_top__DOT__vgc_inst__DOT__screen_addr__8__col;
    __Vfunc_top__DOT__vgc_inst__DOT__screen_addr__8__col = 0;
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
    SData/*10:0*/ __Vfunc_top__DOT__vgc_inst__DOT__screen_addr__10__Vfuncout;
    __Vfunc_top__DOT__vgc_inst__DOT__screen_addr__10__Vfuncout = 0;
    CData/*6:0*/ __Vfunc_top__DOT__vgc_inst__DOT__screen_addr__10__col;
    __Vfunc_top__DOT__vgc_inst__DOT__screen_addr__10__col = 0;
    CData/*4:0*/ __Vfunc_top__DOT__vgc_inst__DOT__screen_addr__10__row;
    __Vfunc_top__DOT__vgc_inst__DOT__screen_addr__10__row = 0;
    CData/*4:0*/ __Vfunc_top__DOT__vgc_inst__DOT__screen_addr__10__rr;
    __Vfunc_top__DOT__vgc_inst__DOT__screen_addr__10__rr = 0;
    SData/*10:0*/ __Vfunc_top__DOT__vgc_inst__DOT__screen_addr__11__Vfuncout;
    __Vfunc_top__DOT__vgc_inst__DOT__screen_addr__11__Vfuncout = 0;
    CData/*6:0*/ __Vfunc_top__DOT__vgc_inst__DOT__screen_addr__11__col;
    __Vfunc_top__DOT__vgc_inst__DOT__screen_addr__11__col = 0;
    CData/*4:0*/ __Vfunc_top__DOT__vgc_inst__DOT__screen_addr__11__row;
    __Vfunc_top__DOT__vgc_inst__DOT__screen_addr__11__row = 0;
    CData/*4:0*/ __Vfunc_top__DOT__vgc_inst__DOT__screen_addr__11__rr;
    __Vfunc_top__DOT__vgc_inst__DOT__screen_addr__11__rr = 0;
    SData/*10:0*/ __Vfunc_top__DOT__vgc_inst__DOT__screen_addr__12__Vfuncout;
    __Vfunc_top__DOT__vgc_inst__DOT__screen_addr__12__Vfuncout = 0;
    CData/*6:0*/ __Vfunc_top__DOT__vgc_inst__DOT__screen_addr__12__col;
    __Vfunc_top__DOT__vgc_inst__DOT__screen_addr__12__col = 0;
    CData/*4:0*/ __Vfunc_top__DOT__vgc_inst__DOT__screen_addr__12__row;
    __Vfunc_top__DOT__vgc_inst__DOT__screen_addr__12__row = 0;
    CData/*4:0*/ __Vfunc_top__DOT__vgc_inst__DOT__screen_addr__12__rr;
    __Vfunc_top__DOT__vgc_inst__DOT__screen_addr__12__rr = 0;
    SData/*10:0*/ __Vfunc_top__DOT__vgc_inst__DOT__screen_addr__13__Vfuncout;
    __Vfunc_top__DOT__vgc_inst__DOT__screen_addr__13__Vfuncout = 0;
    CData/*6:0*/ __Vfunc_top__DOT__vgc_inst__DOT__screen_addr__13__col;
    __Vfunc_top__DOT__vgc_inst__DOT__screen_addr__13__col = 0;
    CData/*4:0*/ __Vfunc_top__DOT__vgc_inst__DOT__screen_addr__13__row;
    __Vfunc_top__DOT__vgc_inst__DOT__screen_addr__13__row = 0;
    CData/*4:0*/ __Vfunc_top__DOT__vgc_inst__DOT__screen_addr__13__rr;
    __Vfunc_top__DOT__vgc_inst__DOT__screen_addr__13__rr = 0;
    SData/*10:0*/ __Vfunc_top__DOT__vgc_inst__DOT__screen_addr__14__Vfuncout;
    __Vfunc_top__DOT__vgc_inst__DOT__screen_addr__14__Vfuncout = 0;
    CData/*6:0*/ __Vfunc_top__DOT__vgc_inst__DOT__screen_addr__14__col;
    __Vfunc_top__DOT__vgc_inst__DOT__screen_addr__14__col = 0;
    CData/*4:0*/ __Vfunc_top__DOT__vgc_inst__DOT__screen_addr__14__row;
    __Vfunc_top__DOT__vgc_inst__DOT__screen_addr__14__row = 0;
    CData/*4:0*/ __Vfunc_top__DOT__vgc_inst__DOT__screen_addr__14__rr;
    __Vfunc_top__DOT__vgc_inst__DOT__screen_addr__14__rr = 0;
    CData/*1:0*/ __VdfgRegularize_hebeb780c_0_23;
    __VdfgRegularize_hebeb780c_0_23 = 0;
    CData/*0:0*/ __VdfgRegularize_hebeb780c_0_25;
    __VdfgRegularize_hebeb780c_0_25 = 0;
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
    SData/*9:0*/ __Vdly__top__DOT__vgc_inst__DOT__h_count;
    __Vdly__top__DOT__vgc_inst__DOT__h_count = 0;
    SData/*9:0*/ __Vdly__top__DOT__vgc_inst__DOT__v_count;
    __Vdly__top__DOT__vgc_inst__DOT__v_count = 0;
    CData/*6:0*/ __Vdly__top__DOT__vgc_inst__DOT__cursor_x;
    __Vdly__top__DOT__vgc_inst__DOT__cursor_x = 0;
    CData/*4:0*/ __Vdly__top__DOT__vgc_inst__DOT__cursor_y;
    __Vdly__top__DOT__vgc_inst__DOT__cursor_y = 0;
    CData/*3:0*/ __Vdly__top__DOT__vgc_inst__DOT__fg_color;
    __Vdly__top__DOT__vgc_inst__DOT__fg_color = 0;
    CData/*3:0*/ __Vdly__top__DOT__vgc_inst__DOT__gfx_color;
    __Vdly__top__DOT__vgc_inst__DOT__gfx_color = 0;
    CData/*4:0*/ __Vdly__top__DOT__vgc_inst__DOT__scroll_offset;
    __Vdly__top__DOT__vgc_inst__DOT__scroll_offset = 0;
    CData/*0:0*/ __Vdly__top__DOT__vgc_inst__DOT__scroll_pending;
    __Vdly__top__DOT__vgc_inst__DOT__scroll_pending = 0;
    CData/*6:0*/ __Vdly__top__DOT__vgc_inst__DOT__scroll_col;
    __Vdly__top__DOT__vgc_inst__DOT__scroll_col = 0;
    CData/*0:0*/ __Vdly__top__DOT__vgc_inst__DOT__cmd_busy;
    __Vdly__top__DOT__vgc_inst__DOT__cmd_busy = 0;
    SData/*8:0*/ __Vdly__top__DOT__vgc_inst__DOT__copper_count;
    __Vdly__top__DOT__vgc_inst__DOT__copper_count = 0;
    SData/*8:0*/ __Vdly__top__DOT__vgc_inst__DOT__copper_index;
    __Vdly__top__DOT__vgc_inst__DOT__copper_index = 0;
    CData/*6:0*/ __Vdly__top__DOT__vgc_inst__DOT__copper_target_list;
    __Vdly__top__DOT__vgc_inst__DOT__copper_target_list = 0;
    CData/*6:0*/ __Vdly__top__DOT__vgc_inst__DOT__copper_pending_list;
    __Vdly__top__DOT__vgc_inst__DOT__copper_pending_list = 0;
    CData/*0:0*/ __Vdly__top__DOT__vgc_inst__DOT__copper_loading;
    __Vdly__top__DOT__vgc_inst__DOT__copper_loading = 0;
    SData/*9:0*/ __Vdly__top__DOT__vgc_inst__DOT__fill_sp;
    __Vdly__top__DOT__vgc_inst__DOT__fill_sp = 0;
    SData/*8:0*/ __Vdly__top__DOT__vgc_inst__DOT__cmd_cx;
    __Vdly__top__DOT__vgc_inst__DOT__cmd_cx = 0;
    CData/*7:0*/ __Vdly__top__DOT__vgc_inst__DOT__cmd_cy;
    __Vdly__top__DOT__vgc_inst__DOT__cmd_cy = 0;
    CData/*2:0*/ __Vdly__top__DOT__vgc_inst__DOT__gt_font_row;
    __Vdly__top__DOT__vgc_inst__DOT__gt_font_row = 0;
    CData/*5:0*/ __Vdly__top__DOT__vgc_inst__DOT__gt_char_idx;
    __Vdly__top__DOT__vgc_inst__DOT__gt_char_idx = 0;
    SData/*9:0*/ __Vdly__top__DOT__vgc_inst__DOT__cmd_dx;
    __Vdly__top__DOT__vgc_inst__DOT__cmd_dx = 0;
    SData/*9:0*/ __Vdly__top__DOT__vgc_inst__DOT__cmd_err;
    __Vdly__top__DOT__vgc_inst__DOT__cmd_err = 0;
    SData/*9:0*/ __Vdly__top__DOT__vgc_inst__DOT__cmd_dy;
    __Vdly__top__DOT__vgc_inst__DOT__cmd_dy = 0;
    SData/*9:0*/ __Vdly__top__DOT__vgc_inst__DOT__cmd_x;
    __Vdly__top__DOT__vgc_inst__DOT__cmd_x = 0;
    SData/*9:0*/ __Vdly__top__DOT__vgc_inst__DOT__cmd_y;
    __Vdly__top__DOT__vgc_inst__DOT__cmd_y = 0;
    CData/*6:0*/ __Vdly__top__DOT__vgc_inst__DOT__copper_active_list;
    __Vdly__top__DOT__vgc_inst__DOT__copper_active_list = 0;
    CData/*0:0*/ __Vdly__top__DOT__vgc_inst__DOT__cursor_blink;
    __Vdly__top__DOT__vgc_inst__DOT__cursor_blink = 0;
    CData/*4:0*/ __Vdly__top__DOT__vgc_inst__DOT__blink_count;
    __Vdly__top__DOT__vgc_inst__DOT__blink_count = 0;
    CData/*3:0*/ __Vdly__top__DOT__vgc_inst__DOT__tile_inst__DOT__dma_state;
    __Vdly__top__DOT__vgc_inst__DOT__tile_inst__DOT__dma_state = 0;
    CData/*0:0*/ __Vdly__top__DOT__vgc_inst__DOT__tile_inst__DOT__dma_data_valid;
    __Vdly__top__DOT__vgc_inst__DOT__tile_inst__DOT__dma_data_valid = 0;
    SData/*15:0*/ __Vdly__top__DOT__vgc_inst__DOT__tile_inst__DOT__dma_count;
    __Vdly__top__DOT__vgc_inst__DOT__tile_inst__DOT__dma_count = 0;
    CData/*4:0*/ __Vdly__top__DOT__vgc_inst__DOT__tile_inst__DOT__dma_col_idx;
    __Vdly__top__DOT__vgc_inst__DOT__tile_inst__DOT__dma_col_idx = 0;
    SData/*15:0*/ __Vdly__top__DOT__vgc_inst__DOT__tile_inst__DOT__dma_src;
    __Vdly__top__DOT__vgc_inst__DOT__tile_inst__DOT__dma_src = 0;
    SData/*15:0*/ __Vdly__top__DOT__vgc_inst__DOT__tile_inst__DOT__dma_dst;
    __Vdly__top__DOT__vgc_inst__DOT__tile_inst__DOT__dma_dst = 0;
    CData/*7:0*/ __Vdly__top__DOT__vgc_inst__DOT__tile_inst__DOT__dma_pal_r;
    __Vdly__top__DOT__vgc_inst__DOT__tile_inst__DOT__dma_pal_r = 0;
    CData/*7:0*/ __Vdly__top__DOT__vgc_inst__DOT__tile_inst__DOT__dma_pal_phase;
    __Vdly__top__DOT__vgc_inst__DOT__tile_inst__DOT__dma_pal_phase = 0;
    CData/*7:0*/ __Vdly__top__DOT__vgc_inst__DOT__tile_inst__DOT__dma_pal_g;
    __Vdly__top__DOT__vgc_inst__DOT__tile_inst__DOT__dma_pal_g = 0;
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
    CData/*0:0*/ __VdlySet__top__DOT__blt_inst__DOT__regs__v20;
    __VdlySet__top__DOT__blt_inst__DOT__regs__v20 = 0;
    CData/*0:0*/ __VdlySet__top__DOT__blt_inst__DOT__regs__v22;
    __VdlySet__top__DOT__blt_inst__DOT__regs__v22 = 0;
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
    CData/*0:0*/ __VdlySet__top__DOT__vgc_inst__DOT__regs__v0;
    __VdlySet__top__DOT__vgc_inst__DOT__regs__v0 = 0;
    CData/*7:0*/ __VdlyVal__top__DOT__vgc_inst__DOT__char_ram__v0;
    __VdlyVal__top__DOT__vgc_inst__DOT__char_ram__v0 = 0;
    SData/*10:0*/ __VdlyDim0__top__DOT__vgc_inst__DOT__char_ram__v0;
    __VdlyDim0__top__DOT__vgc_inst__DOT__char_ram__v0 = 0;
    CData/*7:0*/ __VdlyVal__top__DOT__vgc_inst__DOT__color_ram__v0;
    __VdlyVal__top__DOT__vgc_inst__DOT__color_ram__v0 = 0;
    SData/*10:0*/ __VdlyDim0__top__DOT__vgc_inst__DOT__color_ram__v0;
    __VdlyDim0__top__DOT__vgc_inst__DOT__color_ram__v0 = 0;
    CData/*3:0*/ __VdlyVal__top__DOT__vgc_inst__DOT__gfx_ram__v0;
    __VdlyVal__top__DOT__vgc_inst__DOT__gfx_ram__v0 = 0;
    SData/*15:0*/ __VdlyDim0__top__DOT__vgc_inst__DOT__gfx_ram__v0;
    __VdlyDim0__top__DOT__vgc_inst__DOT__gfx_ram__v0 = 0;
    SData/*10:0*/ __VdlyDim0__top__DOT__vgc_inst__DOT__sprite_shapes__v0;
    __VdlyDim0__top__DOT__vgc_inst__DOT__sprite_shapes__v0 = 0;
    CData/*0:0*/ __VdlySet__top__DOT__vgc_inst__DOT__regs__v1;
    __VdlySet__top__DOT__vgc_inst__DOT__regs__v1 = 0;
    CData/*7:0*/ __VdlyVal__top__DOT__vgc_inst__DOT__char_ram__v1;
    __VdlyVal__top__DOT__vgc_inst__DOT__char_ram__v1 = 0;
    SData/*10:0*/ __VdlyDim0__top__DOT__vgc_inst__DOT__char_ram__v1;
    __VdlyDim0__top__DOT__vgc_inst__DOT__char_ram__v1 = 0;
    CData/*7:0*/ __VdlyVal__top__DOT__vgc_inst__DOT__color_ram__v1;
    __VdlyVal__top__DOT__vgc_inst__DOT__color_ram__v1 = 0;
    SData/*10:0*/ __VdlyDim0__top__DOT__vgc_inst__DOT__color_ram__v1;
    __VdlyDim0__top__DOT__vgc_inst__DOT__color_ram__v1 = 0;
    CData/*7:0*/ __VdlyVal__top__DOT__vgc_inst__DOT__char_ram__v2;
    __VdlyVal__top__DOT__vgc_inst__DOT__char_ram__v2 = 0;
    SData/*10:0*/ __VdlyDim0__top__DOT__vgc_inst__DOT__char_ram__v2;
    __VdlyDim0__top__DOT__vgc_inst__DOT__char_ram__v2 = 0;
    CData/*7:0*/ __VdlyVal__top__DOT__vgc_inst__DOT__color_ram__v2;
    __VdlyVal__top__DOT__vgc_inst__DOT__color_ram__v2 = 0;
    SData/*10:0*/ __VdlyDim0__top__DOT__vgc_inst__DOT__color_ram__v2;
    __VdlyDim0__top__DOT__vgc_inst__DOT__color_ram__v2 = 0;
    CData/*7:0*/ __VdlyVal__top__DOT__vgc_inst__DOT__sprite_shapes__v1;
    __VdlyVal__top__DOT__vgc_inst__DOT__sprite_shapes__v1 = 0;
    SData/*10:0*/ __VdlyDim0__top__DOT__vgc_inst__DOT__sprite_shapes__v1;
    __VdlyDim0__top__DOT__vgc_inst__DOT__sprite_shapes__v1 = 0;
    SData/*10:0*/ __VdlyDim0__top__DOT__vgc_inst__DOT__sprite_shapes__v2;
    __VdlyDim0__top__DOT__vgc_inst__DOT__sprite_shapes__v2 = 0;
    CData/*3:0*/ __VdlyVal__top__DOT__vgc_inst__DOT__gfx_ram__v1;
    __VdlyVal__top__DOT__vgc_inst__DOT__gfx_ram__v1 = 0;
    SData/*15:0*/ __VdlyDim0__top__DOT__vgc_inst__DOT__gfx_ram__v1;
    __VdlyDim0__top__DOT__vgc_inst__DOT__gfx_ram__v1 = 0;
    CData/*3:0*/ __VdlyVal__top__DOT__vgc_inst__DOT__gfx_ram__v2;
    __VdlyVal__top__DOT__vgc_inst__DOT__gfx_ram__v2 = 0;
    SData/*15:0*/ __VdlyDim0__top__DOT__vgc_inst__DOT__gfx_ram__v2;
    __VdlyDim0__top__DOT__vgc_inst__DOT__gfx_ram__v2 = 0;
    IData/*16:0*/ __VdlyVal__top__DOT__vgc_inst__DOT__fill_stack__v0;
    __VdlyVal__top__DOT__vgc_inst__DOT__fill_stack__v0 = 0;
    SData/*8:0*/ __VdlyDim0__top__DOT__vgc_inst__DOT__fill_stack__v0;
    __VdlyDim0__top__DOT__vgc_inst__DOT__fill_stack__v0 = 0;
    CData/*0:0*/ __VdlySet__top__DOT__vgc_inst__DOT__fill_stack__v0;
    __VdlySet__top__DOT__vgc_inst__DOT__fill_stack__v0 = 0;
    IData/*16:0*/ __VdlyVal__top__DOT__vgc_inst__DOT__fill_stack__v1;
    __VdlyVal__top__DOT__vgc_inst__DOT__fill_stack__v1 = 0;
    SData/*8:0*/ __VdlyDim0__top__DOT__vgc_inst__DOT__fill_stack__v1;
    __VdlyDim0__top__DOT__vgc_inst__DOT__fill_stack__v1 = 0;
    IData/*16:0*/ __VdlyVal__top__DOT__vgc_inst__DOT__fill_stack__v2;
    __VdlyVal__top__DOT__vgc_inst__DOT__fill_stack__v2 = 0;
    SData/*8:0*/ __VdlyDim0__top__DOT__vgc_inst__DOT__fill_stack__v2;
    __VdlyDim0__top__DOT__vgc_inst__DOT__fill_stack__v2 = 0;
    IData/*16:0*/ __VdlyVal__top__DOT__vgc_inst__DOT__fill_stack__v3;
    __VdlyVal__top__DOT__vgc_inst__DOT__fill_stack__v3 = 0;
    SData/*8:0*/ __VdlyDim0__top__DOT__vgc_inst__DOT__fill_stack__v3;
    __VdlyDim0__top__DOT__vgc_inst__DOT__fill_stack__v3 = 0;
    CData/*3:0*/ __VdlyVal__top__DOT__vgc_inst__DOT__gfx_ram__v3;
    __VdlyVal__top__DOT__vgc_inst__DOT__gfx_ram__v3 = 0;
    SData/*15:0*/ __VdlyDim0__top__DOT__vgc_inst__DOT__gfx_ram__v3;
    __VdlyDim0__top__DOT__vgc_inst__DOT__gfx_ram__v3 = 0;
    CData/*3:0*/ __VdlyVal__top__DOT__vgc_inst__DOT__gfx_ram__v4;
    __VdlyVal__top__DOT__vgc_inst__DOT__gfx_ram__v4 = 0;
    SData/*15:0*/ __VdlyDim0__top__DOT__vgc_inst__DOT__gfx_ram__v4;
    __VdlyDim0__top__DOT__vgc_inst__DOT__gfx_ram__v4 = 0;
    CData/*3:0*/ __VdlyVal__top__DOT__vgc_inst__DOT__gfx_ram__v5;
    __VdlyVal__top__DOT__vgc_inst__DOT__gfx_ram__v5 = 0;
    SData/*15:0*/ __VdlyDim0__top__DOT__vgc_inst__DOT__gfx_ram__v5;
    __VdlyDim0__top__DOT__vgc_inst__DOT__gfx_ram__v5 = 0;
    CData/*3:0*/ __VdlyVal__top__DOT__vgc_inst__DOT__gfx_ram__v6;
    __VdlyVal__top__DOT__vgc_inst__DOT__gfx_ram__v6 = 0;
    SData/*15:0*/ __VdlyDim0__top__DOT__vgc_inst__DOT__gfx_ram__v6;
    __VdlyDim0__top__DOT__vgc_inst__DOT__gfx_ram__v6 = 0;
    CData/*3:0*/ __VdlyVal__top__DOT__vgc_inst__DOT__gfx_ram__v7;
    __VdlyVal__top__DOT__vgc_inst__DOT__gfx_ram__v7 = 0;
    SData/*15:0*/ __VdlyDim0__top__DOT__vgc_inst__DOT__gfx_ram__v7;
    __VdlyDim0__top__DOT__vgc_inst__DOT__gfx_ram__v7 = 0;
    CData/*3:0*/ __VdlyVal__top__DOT__vgc_inst__DOT__gfx_ram__v8;
    __VdlyVal__top__DOT__vgc_inst__DOT__gfx_ram__v8 = 0;
    SData/*15:0*/ __VdlyDim0__top__DOT__vgc_inst__DOT__gfx_ram__v8;
    __VdlyDim0__top__DOT__vgc_inst__DOT__gfx_ram__v8 = 0;
    CData/*3:0*/ __VdlyVal__top__DOT__vgc_inst__DOT__gfx_ram__v9;
    __VdlyVal__top__DOT__vgc_inst__DOT__gfx_ram__v9 = 0;
    SData/*15:0*/ __VdlyDim0__top__DOT__vgc_inst__DOT__gfx_ram__v9;
    __VdlyDim0__top__DOT__vgc_inst__DOT__gfx_ram__v9 = 0;
    CData/*3:0*/ __VdlyVal__top__DOT__vgc_inst__DOT__gfx_ram__v10;
    __VdlyVal__top__DOT__vgc_inst__DOT__gfx_ram__v10 = 0;
    SData/*15:0*/ __VdlyDim0__top__DOT__vgc_inst__DOT__gfx_ram__v10;
    __VdlyDim0__top__DOT__vgc_inst__DOT__gfx_ram__v10 = 0;
    CData/*3:0*/ __VdlyVal__top__DOT__vgc_inst__DOT__gfx_ram__v11;
    __VdlyVal__top__DOT__vgc_inst__DOT__gfx_ram__v11 = 0;
    SData/*15:0*/ __VdlyDim0__top__DOT__vgc_inst__DOT__gfx_ram__v11;
    __VdlyDim0__top__DOT__vgc_inst__DOT__gfx_ram__v11 = 0;
    CData/*3:0*/ __VdlyVal__top__DOT__vgc_inst__DOT__gfx_ram__v12;
    __VdlyVal__top__DOT__vgc_inst__DOT__gfx_ram__v12 = 0;
    SData/*15:0*/ __VdlyDim0__top__DOT__vgc_inst__DOT__gfx_ram__v12;
    __VdlyDim0__top__DOT__vgc_inst__DOT__gfx_ram__v12 = 0;
    CData/*3:0*/ __VdlyVal__top__DOT__vgc_inst__DOT__gfx_ram__v13;
    __VdlyVal__top__DOT__vgc_inst__DOT__gfx_ram__v13 = 0;
    SData/*15:0*/ __VdlyDim0__top__DOT__vgc_inst__DOT__gfx_ram__v13;
    __VdlyDim0__top__DOT__vgc_inst__DOT__gfx_ram__v13 = 0;
    CData/*3:0*/ __VdlyVal__top__DOT__vgc_inst__DOT__gfx_ram__v14;
    __VdlyVal__top__DOT__vgc_inst__DOT__gfx_ram__v14 = 0;
    SData/*15:0*/ __VdlyDim0__top__DOT__vgc_inst__DOT__gfx_ram__v14;
    __VdlyDim0__top__DOT__vgc_inst__DOT__gfx_ram__v14 = 0;
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
    CData/*7:0*/ __VdlyVal__top__DOT__vgc_inst__DOT__char_ram__v3;
    __VdlyVal__top__DOT__vgc_inst__DOT__char_ram__v3 = 0;
    SData/*10:0*/ __VdlyDim0__top__DOT__vgc_inst__DOT__char_ram__v3;
    __VdlyDim0__top__DOT__vgc_inst__DOT__char_ram__v3 = 0;
    CData/*7:0*/ __VdlyVal__top__DOT__vgc_inst__DOT__char_ram__v4;
    __VdlyVal__top__DOT__vgc_inst__DOT__char_ram__v4 = 0;
    SData/*10:0*/ __VdlyDim0__top__DOT__vgc_inst__DOT__char_ram__v4;
    __VdlyDim0__top__DOT__vgc_inst__DOT__char_ram__v4 = 0;
    CData/*7:0*/ __VdlyVal__top__DOT__vgc_inst__DOT__color_ram__v3;
    __VdlyVal__top__DOT__vgc_inst__DOT__color_ram__v3 = 0;
    SData/*10:0*/ __VdlyDim0__top__DOT__vgc_inst__DOT__color_ram__v3;
    __VdlyDim0__top__DOT__vgc_inst__DOT__color_ram__v3 = 0;
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
    CData/*7:0*/ __VdlyVal__top__DOT__vgc_inst__DOT__char_ram__v5;
    __VdlyVal__top__DOT__vgc_inst__DOT__char_ram__v5 = 0;
    SData/*10:0*/ __VdlyDim0__top__DOT__vgc_inst__DOT__char_ram__v5;
    __VdlyDim0__top__DOT__vgc_inst__DOT__char_ram__v5 = 0;
    CData/*7:0*/ __VdlyVal__top__DOT__vgc_inst__DOT__color_ram__v4;
    __VdlyVal__top__DOT__vgc_inst__DOT__color_ram__v4 = 0;
    SData/*10:0*/ __VdlyDim0__top__DOT__vgc_inst__DOT__color_ram__v4;
    __VdlyDim0__top__DOT__vgc_inst__DOT__color_ram__v4 = 0;
    CData/*3:0*/ __VdlyVal__top__DOT__vgc_inst__DOT__gfx_ram__v15;
    __VdlyVal__top__DOT__vgc_inst__DOT__gfx_ram__v15 = 0;
    SData/*15:0*/ __VdlyDim0__top__DOT__vgc_inst__DOT__gfx_ram__v15;
    __VdlyDim0__top__DOT__vgc_inst__DOT__gfx_ram__v15 = 0;
    CData/*7:0*/ __VdlyVal__top__DOT__vgc_inst__DOT__sprite_shapes__v3;
    __VdlyVal__top__DOT__vgc_inst__DOT__sprite_shapes__v3 = 0;
    SData/*10:0*/ __VdlyDim0__top__DOT__vgc_inst__DOT__sprite_shapes__v3;
    __VdlyDim0__top__DOT__vgc_inst__DOT__sprite_shapes__v3 = 0;
    CData/*7:0*/ __VdlyVal__top__DOT__vgc_inst__DOT__tile_inst__DOT__tile_data__v0;
    __VdlyVal__top__DOT__vgc_inst__DOT__tile_inst__DOT__tile_data__v0 = 0;
    SData/*14:0*/ __VdlyDim0__top__DOT__vgc_inst__DOT__tile_inst__DOT__tile_data__v0;
    __VdlyDim0__top__DOT__vgc_inst__DOT__tile_inst__DOT__tile_data__v0 = 0;
    CData/*0:0*/ __VdlySet__top__DOT__vgc_inst__DOT__tile_inst__DOT__tile_data__v0;
    __VdlySet__top__DOT__vgc_inst__DOT__tile_inst__DOT__tile_data__v0 = 0;
    CData/*7:0*/ __VdlyVal__top__DOT__vgc_inst__DOT__regs__v32;
    __VdlyVal__top__DOT__vgc_inst__DOT__regs__v32 = 0;
    CData/*0:0*/ __VdlySet__top__DOT__vgc_inst__DOT__regs__v32;
    __VdlySet__top__DOT__vgc_inst__DOT__regs__v32 = 0;
    CData/*7:0*/ __VdlyVal__top__DOT__vgc_inst__DOT__regs__v33;
    __VdlyVal__top__DOT__vgc_inst__DOT__regs__v33 = 0;
    CData/*0:0*/ __VdlySet__top__DOT__vgc_inst__DOT__regs__v33;
    __VdlySet__top__DOT__vgc_inst__DOT__regs__v33 = 0;
    CData/*7:0*/ __VdlyVal__top__DOT__vgc_inst__DOT__regs__v34;
    __VdlyVal__top__DOT__vgc_inst__DOT__regs__v34 = 0;
    CData/*0:0*/ __VdlySet__top__DOT__vgc_inst__DOT__regs__v34;
    __VdlySet__top__DOT__vgc_inst__DOT__regs__v34 = 0;
    CData/*7:0*/ __VdlyVal__top__DOT__vgc_inst__DOT__regs__v35;
    __VdlyVal__top__DOT__vgc_inst__DOT__regs__v35 = 0;
    CData/*0:0*/ __VdlySet__top__DOT__vgc_inst__DOT__regs__v35;
    __VdlySet__top__DOT__vgc_inst__DOT__regs__v35 = 0;
    CData/*7:0*/ __VdlyVal__top__DOT__vgc_inst__DOT__regs__v36;
    __VdlyVal__top__DOT__vgc_inst__DOT__regs__v36 = 0;
    CData/*0:0*/ __VdlySet__top__DOT__vgc_inst__DOT__regs__v36;
    __VdlySet__top__DOT__vgc_inst__DOT__regs__v36 = 0;
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
    CData/*7:0*/ __VdlyVal__top__DOT__vgc_inst__DOT__sprite_shapes__v4;
    __VdlyVal__top__DOT__vgc_inst__DOT__sprite_shapes__v4 = 0;
    SData/*10:0*/ __VdlyDim0__top__DOT__vgc_inst__DOT__sprite_shapes__v4;
    __VdlyDim0__top__DOT__vgc_inst__DOT__sprite_shapes__v4 = 0;
    CData/*7:0*/ __VdlyVal__top__DOT__vgc_inst__DOT__sprite_shapes__v5;
    __VdlyVal__top__DOT__vgc_inst__DOT__sprite_shapes__v5 = 0;
    SData/*10:0*/ __VdlyDim0__top__DOT__vgc_inst__DOT__sprite_shapes__v5;
    __VdlyDim0__top__DOT__vgc_inst__DOT__sprite_shapes__v5 = 0;
    CData/*7:0*/ __VdlyVal__top__DOT__vgc_inst__DOT__sprite_shapes__v6;
    __VdlyVal__top__DOT__vgc_inst__DOT__sprite_shapes__v6 = 0;
    SData/*10:0*/ __VdlyDim0__top__DOT__vgc_inst__DOT__sprite_shapes__v6;
    __VdlyDim0__top__DOT__vgc_inst__DOT__sprite_shapes__v6 = 0;
    CData/*7:0*/ __VdlyVal__top__DOT__vgc_inst__DOT__sprite_shapes__v7;
    __VdlyVal__top__DOT__vgc_inst__DOT__sprite_shapes__v7 = 0;
    SData/*10:0*/ __VdlyDim0__top__DOT__vgc_inst__DOT__sprite_shapes__v7;
    __VdlyDim0__top__DOT__vgc_inst__DOT__sprite_shapes__v7 = 0;
    CData/*7:0*/ __VdlyVal__top__DOT__vgc_inst__DOT__sprite_shapes__v8;
    __VdlyVal__top__DOT__vgc_inst__DOT__sprite_shapes__v8 = 0;
    SData/*10:0*/ __VdlyDim0__top__DOT__vgc_inst__DOT__sprite_shapes__v8;
    __VdlyDim0__top__DOT__vgc_inst__DOT__sprite_shapes__v8 = 0;
    CData/*7:0*/ __VdlyVal__top__DOT__vgc_inst__DOT__sprite_shapes__v9;
    __VdlyVal__top__DOT__vgc_inst__DOT__sprite_shapes__v9 = 0;
    SData/*10:0*/ __VdlyDim0__top__DOT__vgc_inst__DOT__sprite_shapes__v9;
    __VdlyDim0__top__DOT__vgc_inst__DOT__sprite_shapes__v9 = 0;
    CData/*7:0*/ __VdlyVal__top__DOT__vgc_inst__DOT__sprite_shapes__v10;
    __VdlyVal__top__DOT__vgc_inst__DOT__sprite_shapes__v10 = 0;
    SData/*10:0*/ __VdlyDim0__top__DOT__vgc_inst__DOT__sprite_shapes__v10;
    __VdlyDim0__top__DOT__vgc_inst__DOT__sprite_shapes__v10 = 0;
    CData/*7:0*/ __VdlyVal__top__DOT__vgc_inst__DOT__sprite_shapes__v11;
    __VdlyVal__top__DOT__vgc_inst__DOT__sprite_shapes__v11 = 0;
    SData/*10:0*/ __VdlyDim0__top__DOT__vgc_inst__DOT__sprite_shapes__v11;
    __VdlyDim0__top__DOT__vgc_inst__DOT__sprite_shapes__v11 = 0;
    CData/*7:0*/ __VdlyVal__top__DOT__vgc_inst__DOT__sprite_shapes__v12;
    __VdlyVal__top__DOT__vgc_inst__DOT__sprite_shapes__v12 = 0;
    SData/*10:0*/ __VdlyDim0__top__DOT__vgc_inst__DOT__sprite_shapes__v12;
    __VdlyDim0__top__DOT__vgc_inst__DOT__sprite_shapes__v12 = 0;
    IData/*16:0*/ __VdlyVal__top__DOT__vgc_inst__DOT__fill_stack__v4;
    __VdlyVal__top__DOT__vgc_inst__DOT__fill_stack__v4 = 0;
    CData/*0:0*/ __VdlySet__top__DOT__vgc_inst__DOT__fill_stack__v4;
    __VdlySet__top__DOT__vgc_inst__DOT__fill_stack__v4 = 0;
    CData/*3:0*/ __VdlyVal__top__DOT__vgc_inst__DOT__gfx_ram__v16;
    __VdlyVal__top__DOT__vgc_inst__DOT__gfx_ram__v16 = 0;
    SData/*15:0*/ __VdlyDim0__top__DOT__vgc_inst__DOT__gfx_ram__v16;
    __VdlyDim0__top__DOT__vgc_inst__DOT__gfx_ram__v16 = 0;
    CData/*3:0*/ __VdlyVal__top__DOT__vgc_inst__DOT__gfx_ram__v17;
    __VdlyVal__top__DOT__vgc_inst__DOT__gfx_ram__v17 = 0;
    SData/*15:0*/ __VdlyDim0__top__DOT__vgc_inst__DOT__gfx_ram__v17;
    __VdlyDim0__top__DOT__vgc_inst__DOT__gfx_ram__v17 = 0;
    CData/*7:0*/ __VdlyVal__top__DOT__vgc_inst__DOT__regs__v37;
    __VdlyVal__top__DOT__vgc_inst__DOT__regs__v37 = 0;
    CData/*4:0*/ __VdlyDim0__top__DOT__vgc_inst__DOT__regs__v37;
    __VdlyDim0__top__DOT__vgc_inst__DOT__regs__v37 = 0;
    CData/*0:0*/ __VdlySet__top__DOT__vgc_inst__DOT__regs__v37;
    __VdlySet__top__DOT__vgc_inst__DOT__regs__v37 = 0;
    CData/*7:0*/ __VdlyVal__top__DOT__vgc_inst__DOT__char_ram__v6;
    __VdlyVal__top__DOT__vgc_inst__DOT__char_ram__v6 = 0;
    SData/*10:0*/ __VdlyDim0__top__DOT__vgc_inst__DOT__char_ram__v6;
    __VdlyDim0__top__DOT__vgc_inst__DOT__char_ram__v6 = 0;
    CData/*7:0*/ __VdlyVal__top__DOT__vgc_inst__DOT__fio_name__v0;
    __VdlyVal__top__DOT__vgc_inst__DOT__fio_name__v0 = 0;
    CData/*5:0*/ __VdlyDim0__top__DOT__vgc_inst__DOT__fio_name__v0;
    __VdlyDim0__top__DOT__vgc_inst__DOT__fio_name__v0 = 0;
    CData/*0:0*/ __VdlySet__top__DOT__vgc_inst__DOT__fio_name__v0;
    __VdlySet__top__DOT__vgc_inst__DOT__fio_name__v0 = 0;
    CData/*7:0*/ __VdlyVal__top__DOT__vgc_inst__DOT__color_ram__v5;
    __VdlyVal__top__DOT__vgc_inst__DOT__color_ram__v5 = 0;
    SData/*10:0*/ __VdlyDim0__top__DOT__vgc_inst__DOT__color_ram__v5;
    __VdlyDim0__top__DOT__vgc_inst__DOT__color_ram__v5 = 0;
    CData/*3:0*/ __VdlyVal__top__DOT__vgc_inst__DOT__spr_trans__v16;
    __VdlyVal__top__DOT__vgc_inst__DOT__spr_trans__v16 = 0;
    CData/*3:0*/ __VdlyDim0__top__DOT__vgc_inst__DOT__spr_trans__v16;
    __VdlyDim0__top__DOT__vgc_inst__DOT__spr_trans__v16 = 0;
    CData/*0:0*/ __VdlySet__top__DOT__vgc_inst__DOT__spr_trans__v16;
    __VdlySet__top__DOT__vgc_inst__DOT__spr_trans__v16 = 0;
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
    CData/*3:0*/ __VdlyVal__top__DOT__vgc_inst__DOT__spr_shape__v16;
    __VdlyVal__top__DOT__vgc_inst__DOT__spr_shape__v16 = 0;
    CData/*3:0*/ __VdlyDim0__top__DOT__vgc_inst__DOT__spr_shape__v16;
    __VdlyDim0__top__DOT__vgc_inst__DOT__spr_shape__v16 = 0;
    CData/*0:0*/ __VdlySet__top__DOT__vgc_inst__DOT__spr_shape__v16;
    __VdlySet__top__DOT__vgc_inst__DOT__spr_shape__v16 = 0;
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
    CData/*7:0*/ __VdlyVal__top__DOT__vgc_inst__DOT__regs__v38;
    __VdlyVal__top__DOT__vgc_inst__DOT__regs__v38 = 0;
    CData/*4:0*/ __VdlyDim0__top__DOT__vgc_inst__DOT__regs__v38;
    __VdlyDim0__top__DOT__vgc_inst__DOT__regs__v38 = 0;
    CData/*0:0*/ __VdlySet__top__DOT__vgc_inst__DOT__regs__v38;
    __VdlySet__top__DOT__vgc_inst__DOT__regs__v38 = 0;
    CData/*7:0*/ __VdlyVal__top__DOT__vgc_inst__DOT__color_ram__v6;
    __VdlyVal__top__DOT__vgc_inst__DOT__color_ram__v6 = 0;
    SData/*10:0*/ __VdlyDim0__top__DOT__vgc_inst__DOT__color_ram__v6;
    __VdlyDim0__top__DOT__vgc_inst__DOT__color_ram__v6 = 0;
    CData/*7:0*/ __VdlyVal__top__DOT__vgc_inst__DOT__tile_inst__DOT__tile_data__v1;
    __VdlyVal__top__DOT__vgc_inst__DOT__tile_inst__DOT__tile_data__v1 = 0;
    SData/*14:0*/ __VdlyDim0__top__DOT__vgc_inst__DOT__tile_inst__DOT__tile_data__v1;
    __VdlyDim0__top__DOT__vgc_inst__DOT__tile_inst__DOT__tile_data__v1 = 0;
    CData/*0:0*/ __VdlySet__top__DOT__vgc_inst__DOT__tile_inst__DOT__tile_data__v1;
    __VdlySet__top__DOT__vgc_inst__DOT__tile_inst__DOT__tile_data__v1 = 0;
    CData/*3:0*/ __VdlyVal__top__DOT__vgc_inst__DOT__gfx_ram__v18;
    __VdlyVal__top__DOT__vgc_inst__DOT__gfx_ram__v18 = 0;
    SData/*15:0*/ __VdlyDim0__top__DOT__vgc_inst__DOT__gfx_ram__v18;
    __VdlyDim0__top__DOT__vgc_inst__DOT__gfx_ram__v18 = 0;
    CData/*7:0*/ __VdlyVal__top__DOT__vgc_inst__DOT__sprite_shapes__v13;
    __VdlyVal__top__DOT__vgc_inst__DOT__sprite_shapes__v13 = 0;
    SData/*10:0*/ __VdlyDim0__top__DOT__vgc_inst__DOT__sprite_shapes__v13;
    __VdlyDim0__top__DOT__vgc_inst__DOT__sprite_shapes__v13 = 0;
    CData/*7:0*/ __VdlyVal__top__DOT__vgc_inst__DOT__char_ram__v7;
    __VdlyVal__top__DOT__vgc_inst__DOT__char_ram__v7 = 0;
    SData/*10:0*/ __VdlyDim0__top__DOT__vgc_inst__DOT__char_ram__v7;
    __VdlyDim0__top__DOT__vgc_inst__DOT__char_ram__v7 = 0;
    CData/*7:0*/ __VdlyVal__top__DOT__vgc_inst__DOT__tile_inst__DOT__tregs__v0;
    __VdlyVal__top__DOT__vgc_inst__DOT__tile_inst__DOT__tregs__v0 = 0;
    CData/*4:0*/ __VdlyDim0__top__DOT__vgc_inst__DOT__tile_inst__DOT__tregs__v0;
    __VdlyDim0__top__DOT__vgc_inst__DOT__tile_inst__DOT__tregs__v0 = 0;
    CData/*0:0*/ __VdlySet__top__DOT__vgc_inst__DOT__tile_inst__DOT__tregs__v0;
    __VdlySet__top__DOT__vgc_inst__DOT__tile_inst__DOT__tregs__v0 = 0;
    CData/*7:0*/ __VdlyVal__top__DOT__vgc_inst__DOT__tile_inst__DOT__nametable__v0;
    __VdlyVal__top__DOT__vgc_inst__DOT__tile_inst__DOT__nametable__v0 = 0;
    SData/*11:0*/ __VdlyDim0__top__DOT__vgc_inst__DOT__tile_inst__DOT__nametable__v0;
    __VdlyDim0__top__DOT__vgc_inst__DOT__tile_inst__DOT__nametable__v0 = 0;
    CData/*7:0*/ __VdlyVal__top__DOT__vgc_inst__DOT__tile_inst__DOT__attr_table__v0;
    __VdlyVal__top__DOT__vgc_inst__DOT__tile_inst__DOT__attr_table__v0 = 0;
    SData/*11:0*/ __VdlyDim0__top__DOT__vgc_inst__DOT__tile_inst__DOT__attr_table__v0;
    __VdlyDim0__top__DOT__vgc_inst__DOT__tile_inst__DOT__attr_table__v0 = 0;
    CData/*0:0*/ __VdlySet__top__DOT__vgc_inst__DOT__tile_inst__DOT__attr_table__v0;
    __VdlySet__top__DOT__vgc_inst__DOT__tile_inst__DOT__attr_table__v0 = 0;
    CData/*7:0*/ __VdlyVal__top__DOT__vgc_inst__DOT__tile_inst__DOT__attr_table__v1;
    __VdlyVal__top__DOT__vgc_inst__DOT__tile_inst__DOT__attr_table__v1 = 0;
    SData/*11:0*/ __VdlyDim0__top__DOT__vgc_inst__DOT__tile_inst__DOT__attr_table__v1;
    __VdlyDim0__top__DOT__vgc_inst__DOT__tile_inst__DOT__attr_table__v1 = 0;
    CData/*0:0*/ __VdlySet__top__DOT__vgc_inst__DOT__tile_inst__DOT__attr_table__v1;
    __VdlySet__top__DOT__vgc_inst__DOT__tile_inst__DOT__attr_table__v1 = 0;
    CData/*7:0*/ __VdlyVal__top__DOT__vgc_inst__DOT__tile_inst__DOT__attr_table__v2;
    __VdlyVal__top__DOT__vgc_inst__DOT__tile_inst__DOT__attr_table__v2 = 0;
    SData/*11:0*/ __VdlyDim0__top__DOT__vgc_inst__DOT__tile_inst__DOT__attr_table__v2;
    __VdlyDim0__top__DOT__vgc_inst__DOT__tile_inst__DOT__attr_table__v2 = 0;
    CData/*0:0*/ __VdlySet__top__DOT__vgc_inst__DOT__tile_inst__DOT__attr_table__v2;
    __VdlySet__top__DOT__vgc_inst__DOT__tile_inst__DOT__attr_table__v2 = 0;
    CData/*7:0*/ __VdlyVal__top__DOT__vgc_inst__DOT__tile_inst__DOT__nametable__v1;
    __VdlyVal__top__DOT__vgc_inst__DOT__tile_inst__DOT__nametable__v1 = 0;
    SData/*11:0*/ __VdlyDim0__top__DOT__vgc_inst__DOT__tile_inst__DOT__nametable__v1;
    __VdlyDim0__top__DOT__vgc_inst__DOT__tile_inst__DOT__nametable__v1 = 0;
    CData/*7:0*/ __VdlyVal__top__DOT__vgc_inst__DOT__tile_inst__DOT__nametable__v2;
    __VdlyVal__top__DOT__vgc_inst__DOT__tile_inst__DOT__nametable__v2 = 0;
    SData/*11:0*/ __VdlyDim0__top__DOT__vgc_inst__DOT__tile_inst__DOT__nametable__v2;
    __VdlyDim0__top__DOT__vgc_inst__DOT__tile_inst__DOT__nametable__v2 = 0;
    SData/*11:0*/ __VdlyVal__top__DOT__vgc_inst__DOT__tile_inst__DOT__pal_ram__v0;
    __VdlyVal__top__DOT__vgc_inst__DOT__tile_inst__DOT__pal_ram__v0 = 0;
    CData/*7:0*/ __VdlyDim0__top__DOT__vgc_inst__DOT__tile_inst__DOT__pal_ram__v0;
    __VdlyDim0__top__DOT__vgc_inst__DOT__tile_inst__DOT__pal_ram__v0 = 0;
    CData/*0:0*/ __VdlySet__top__DOT__vgc_inst__DOT__tile_inst__DOT__pal_ram__v0;
    __VdlySet__top__DOT__vgc_inst__DOT__tile_inst__DOT__pal_ram__v0 = 0;
    CData/*7:0*/ __VdlyVal__top__DOT__vgc_inst__DOT__tile_inst__DOT__attr_table__v3;
    __VdlyVal__top__DOT__vgc_inst__DOT__tile_inst__DOT__attr_table__v3 = 0;
    SData/*11:0*/ __VdlyDim0__top__DOT__vgc_inst__DOT__tile_inst__DOT__attr_table__v3;
    __VdlyDim0__top__DOT__vgc_inst__DOT__tile_inst__DOT__attr_table__v3 = 0;
    CData/*0:0*/ __VdlySet__top__DOT__vgc_inst__DOT__tile_inst__DOT__attr_table__v3;
    __VdlySet__top__DOT__vgc_inst__DOT__tile_inst__DOT__attr_table__v3 = 0;
    CData/*7:0*/ __VdlyVal__top__DOT__vgc_inst__DOT__tile_inst__DOT__nametable__v3;
    __VdlyVal__top__DOT__vgc_inst__DOT__tile_inst__DOT__nametable__v3 = 0;
    SData/*11:0*/ __VdlyDim0__top__DOT__vgc_inst__DOT__tile_inst__DOT__nametable__v3;
    __VdlyDim0__top__DOT__vgc_inst__DOT__tile_inst__DOT__nametable__v3 = 0;
    CData/*7:0*/ __VdlyVal__top__DOT__vgc_inst__DOT__tile_inst__DOT__tile_data__v2;
    __VdlyVal__top__DOT__vgc_inst__DOT__tile_inst__DOT__tile_data__v2 = 0;
    SData/*14:0*/ __VdlyDim0__top__DOT__vgc_inst__DOT__tile_inst__DOT__tile_data__v2;
    __VdlyDim0__top__DOT__vgc_inst__DOT__tile_inst__DOT__tile_data__v2 = 0;
    CData/*0:0*/ __VdlySet__top__DOT__vgc_inst__DOT__tile_inst__DOT__tile_data__v2;
    __VdlySet__top__DOT__vgc_inst__DOT__tile_inst__DOT__tile_data__v2 = 0;
    CData/*7:0*/ __VdlyVal__top__DOT__vgc_inst__DOT__tile_inst__DOT__nametable__v4;
    __VdlyVal__top__DOT__vgc_inst__DOT__tile_inst__DOT__nametable__v4 = 0;
    SData/*11:0*/ __VdlyDim0__top__DOT__vgc_inst__DOT__tile_inst__DOT__nametable__v4;
    __VdlyDim0__top__DOT__vgc_inst__DOT__tile_inst__DOT__nametable__v4 = 0;
    CData/*7:0*/ __VdlyVal__top__DOT__vgc_inst__DOT__tile_inst__DOT__nametable__v5;
    __VdlyVal__top__DOT__vgc_inst__DOT__tile_inst__DOT__nametable__v5 = 0;
    SData/*11:0*/ __VdlyDim0__top__DOT__vgc_inst__DOT__tile_inst__DOT__nametable__v5;
    __VdlyDim0__top__DOT__vgc_inst__DOT__tile_inst__DOT__nametable__v5 = 0;
    CData/*7:0*/ __VdlyVal__top__DOT__vgc_inst__DOT__tile_inst__DOT__nametable__v6;
    __VdlyVal__top__DOT__vgc_inst__DOT__tile_inst__DOT__nametable__v6 = 0;
    SData/*11:0*/ __VdlyDim0__top__DOT__vgc_inst__DOT__tile_inst__DOT__nametable__v6;
    __VdlyDim0__top__DOT__vgc_inst__DOT__tile_inst__DOT__nametable__v6 = 0;
    CData/*7:0*/ __VdlyVal__top__DOT__vgc_inst__DOT__tile_inst__DOT__nametable__v7;
    __VdlyVal__top__DOT__vgc_inst__DOT__tile_inst__DOT__nametable__v7 = 0;
    SData/*11:0*/ __VdlyDim0__top__DOT__vgc_inst__DOT__tile_inst__DOT__nametable__v7;
    __VdlyDim0__top__DOT__vgc_inst__DOT__tile_inst__DOT__nametable__v7 = 0;
    CData/*7:0*/ __VdlyVal__top__DOT__vgc_inst__DOT__tile_inst__DOT__nametable__v8;
    __VdlyVal__top__DOT__vgc_inst__DOT__tile_inst__DOT__nametable__v8 = 0;
    SData/*11:0*/ __VdlyDim0__top__DOT__vgc_inst__DOT__tile_inst__DOT__nametable__v8;
    __VdlyDim0__top__DOT__vgc_inst__DOT__tile_inst__DOT__nametable__v8 = 0;
    CData/*7:0*/ __VdlyVal__top__DOT__vgc_inst__DOT__tile_inst__DOT__nametable__v9;
    __VdlyVal__top__DOT__vgc_inst__DOT__tile_inst__DOT__nametable__v9 = 0;
    SData/*11:0*/ __VdlyDim0__top__DOT__vgc_inst__DOT__tile_inst__DOT__nametable__v9;
    __VdlyDim0__top__DOT__vgc_inst__DOT__tile_inst__DOT__nametable__v9 = 0;
    CData/*7:0*/ __VdlyVal__top__DOT__vgc_inst__DOT__tile_inst__DOT__nametable__v10;
    __VdlyVal__top__DOT__vgc_inst__DOT__tile_inst__DOT__nametable__v10 = 0;
    SData/*11:0*/ __VdlyDim0__top__DOT__vgc_inst__DOT__tile_inst__DOT__nametable__v10;
    __VdlyDim0__top__DOT__vgc_inst__DOT__tile_inst__DOT__nametable__v10 = 0;
    CData/*7:0*/ __VdlyVal__top__DOT__vgc_inst__DOT__tile_inst__DOT__nametable__v11;
    __VdlyVal__top__DOT__vgc_inst__DOT__tile_inst__DOT__nametable__v11 = 0;
    SData/*11:0*/ __VdlyDim0__top__DOT__vgc_inst__DOT__tile_inst__DOT__nametable__v11;
    __VdlyDim0__top__DOT__vgc_inst__DOT__tile_inst__DOT__nametable__v11 = 0;
    CData/*7:0*/ __VdlyVal__top__DOT__vgc_inst__DOT__tile_inst__DOT__nametable__v12;
    __VdlyVal__top__DOT__vgc_inst__DOT__tile_inst__DOT__nametable__v12 = 0;
    SData/*11:0*/ __VdlyDim0__top__DOT__vgc_inst__DOT__tile_inst__DOT__nametable__v12;
    __VdlyDim0__top__DOT__vgc_inst__DOT__tile_inst__DOT__nametable__v12 = 0;
    CData/*7:0*/ __VdlyVal__top__DOT__vgc_inst__DOT__tile_inst__DOT__nametable__v13;
    __VdlyVal__top__DOT__vgc_inst__DOT__tile_inst__DOT__nametable__v13 = 0;
    SData/*11:0*/ __VdlyDim0__top__DOT__vgc_inst__DOT__tile_inst__DOT__nametable__v13;
    __VdlyDim0__top__DOT__vgc_inst__DOT__tile_inst__DOT__nametable__v13 = 0;
    CData/*7:0*/ __VdlyVal__top__DOT__vgc_inst__DOT__tile_inst__DOT__nametable__v14;
    __VdlyVal__top__DOT__vgc_inst__DOT__tile_inst__DOT__nametable__v14 = 0;
    SData/*11:0*/ __VdlyDim0__top__DOT__vgc_inst__DOT__tile_inst__DOT__nametable__v14;
    __VdlyDim0__top__DOT__vgc_inst__DOT__tile_inst__DOT__nametable__v14 = 0;
    CData/*7:0*/ __VdlyVal__top__DOT__vgc_inst__DOT__tile_inst__DOT__nametable__v15;
    __VdlyVal__top__DOT__vgc_inst__DOT__tile_inst__DOT__nametable__v15 = 0;
    SData/*11:0*/ __VdlyDim0__top__DOT__vgc_inst__DOT__tile_inst__DOT__nametable__v15;
    __VdlyDim0__top__DOT__vgc_inst__DOT__tile_inst__DOT__nametable__v15 = 0;
    CData/*7:0*/ __VdlyVal__top__DOT__vgc_inst__DOT__tile_inst__DOT__nametable__v16;
    __VdlyVal__top__DOT__vgc_inst__DOT__tile_inst__DOT__nametable__v16 = 0;
    SData/*11:0*/ __VdlyDim0__top__DOT__vgc_inst__DOT__tile_inst__DOT__nametable__v16;
    __VdlyDim0__top__DOT__vgc_inst__DOT__tile_inst__DOT__nametable__v16 = 0;
    CData/*7:0*/ __VdlyVal__top__DOT__vgc_inst__DOT__tile_inst__DOT__nametable__v17;
    __VdlyVal__top__DOT__vgc_inst__DOT__tile_inst__DOT__nametable__v17 = 0;
    SData/*11:0*/ __VdlyDim0__top__DOT__vgc_inst__DOT__tile_inst__DOT__nametable__v17;
    __VdlyDim0__top__DOT__vgc_inst__DOT__tile_inst__DOT__nametable__v17 = 0;
    CData/*7:0*/ __VdlyVal__top__DOT__vgc_inst__DOT__tile_inst__DOT__nametable__v18;
    __VdlyVal__top__DOT__vgc_inst__DOT__tile_inst__DOT__nametable__v18 = 0;
    SData/*11:0*/ __VdlyDim0__top__DOT__vgc_inst__DOT__tile_inst__DOT__nametable__v18;
    __VdlyDim0__top__DOT__vgc_inst__DOT__tile_inst__DOT__nametable__v18 = 0;
    CData/*7:0*/ __VdlyVal__top__DOT__vgc_inst__DOT__tile_inst__DOT__nametable__v19;
    __VdlyVal__top__DOT__vgc_inst__DOT__tile_inst__DOT__nametable__v19 = 0;
    SData/*11:0*/ __VdlyDim0__top__DOT__vgc_inst__DOT__tile_inst__DOT__nametable__v19;
    __VdlyDim0__top__DOT__vgc_inst__DOT__tile_inst__DOT__nametable__v19 = 0;
    CData/*7:0*/ __VdlyVal__top__DOT__vgc_inst__DOT__tile_inst__DOT__nametable__v20;
    __VdlyVal__top__DOT__vgc_inst__DOT__tile_inst__DOT__nametable__v20 = 0;
    SData/*11:0*/ __VdlyDim0__top__DOT__vgc_inst__DOT__tile_inst__DOT__nametable__v20;
    __VdlyDim0__top__DOT__vgc_inst__DOT__tile_inst__DOT__nametable__v20 = 0;
    CData/*7:0*/ __VdlyVal__top__DOT__vgc_inst__DOT__tile_inst__DOT__nametable__v21;
    __VdlyVal__top__DOT__vgc_inst__DOT__tile_inst__DOT__nametable__v21 = 0;
    SData/*11:0*/ __VdlyDim0__top__DOT__vgc_inst__DOT__tile_inst__DOT__nametable__v21;
    __VdlyDim0__top__DOT__vgc_inst__DOT__tile_inst__DOT__nametable__v21 = 0;
    CData/*7:0*/ __VdlyVal__top__DOT__vgc_inst__DOT__tile_inst__DOT__nametable__v22;
    __VdlyVal__top__DOT__vgc_inst__DOT__tile_inst__DOT__nametable__v22 = 0;
    SData/*11:0*/ __VdlyDim0__top__DOT__vgc_inst__DOT__tile_inst__DOT__nametable__v22;
    __VdlyDim0__top__DOT__vgc_inst__DOT__tile_inst__DOT__nametable__v22 = 0;
    CData/*7:0*/ __VdlyVal__top__DOT__vgc_inst__DOT__tile_inst__DOT__nametable__v23;
    __VdlyVal__top__DOT__vgc_inst__DOT__tile_inst__DOT__nametable__v23 = 0;
    SData/*11:0*/ __VdlyDim0__top__DOT__vgc_inst__DOT__tile_inst__DOT__nametable__v23;
    __VdlyDim0__top__DOT__vgc_inst__DOT__tile_inst__DOT__nametable__v23 = 0;
    CData/*7:0*/ __VdlyVal__top__DOT__vgc_inst__DOT__tile_inst__DOT__nametable__v24;
    __VdlyVal__top__DOT__vgc_inst__DOT__tile_inst__DOT__nametable__v24 = 0;
    SData/*11:0*/ __VdlyDim0__top__DOT__vgc_inst__DOT__tile_inst__DOT__nametable__v24;
    __VdlyDim0__top__DOT__vgc_inst__DOT__tile_inst__DOT__nametable__v24 = 0;
    CData/*7:0*/ __VdlyVal__top__DOT__vgc_inst__DOT__tile_inst__DOT__nametable__v25;
    __VdlyVal__top__DOT__vgc_inst__DOT__tile_inst__DOT__nametable__v25 = 0;
    SData/*11:0*/ __VdlyDim0__top__DOT__vgc_inst__DOT__tile_inst__DOT__nametable__v25;
    __VdlyDim0__top__DOT__vgc_inst__DOT__tile_inst__DOT__nametable__v25 = 0;
    CData/*7:0*/ __VdlyVal__top__DOT__vgc_inst__DOT__tile_inst__DOT__nametable__v26;
    __VdlyVal__top__DOT__vgc_inst__DOT__tile_inst__DOT__nametable__v26 = 0;
    SData/*11:0*/ __VdlyDim0__top__DOT__vgc_inst__DOT__tile_inst__DOT__nametable__v26;
    __VdlyDim0__top__DOT__vgc_inst__DOT__tile_inst__DOT__nametable__v26 = 0;
    CData/*7:0*/ __VdlyVal__top__DOT__vgc_inst__DOT__tile_inst__DOT__nametable__v27;
    __VdlyVal__top__DOT__vgc_inst__DOT__tile_inst__DOT__nametable__v27 = 0;
    SData/*11:0*/ __VdlyDim0__top__DOT__vgc_inst__DOT__tile_inst__DOT__nametable__v27;
    __VdlyDim0__top__DOT__vgc_inst__DOT__tile_inst__DOT__nametable__v27 = 0;
    CData/*7:0*/ __VdlyVal__top__DOT__vgc_inst__DOT__tile_inst__DOT__nametable__v28;
    __VdlyVal__top__DOT__vgc_inst__DOT__tile_inst__DOT__nametable__v28 = 0;
    SData/*11:0*/ __VdlyDim0__top__DOT__vgc_inst__DOT__tile_inst__DOT__nametable__v28;
    __VdlyDim0__top__DOT__vgc_inst__DOT__tile_inst__DOT__nametable__v28 = 0;
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
    SData/*11:0*/ __VdlyVal__top__DOT__vgc_inst__DOT__tile_inst__DOT__pal_ram__v1;
    __VdlyVal__top__DOT__vgc_inst__DOT__tile_inst__DOT__pal_ram__v1 = 0;
    CData/*7:0*/ __VdlyDim0__top__DOT__vgc_inst__DOT__tile_inst__DOT__pal_ram__v1;
    __VdlyDim0__top__DOT__vgc_inst__DOT__tile_inst__DOT__pal_ram__v1 = 0;
    CData/*0:0*/ __VdlySet__top__DOT__vgc_inst__DOT__tile_inst__DOT__pal_ram__v1;
    __VdlySet__top__DOT__vgc_inst__DOT__tile_inst__DOT__pal_ram__v1 = 0;
    CData/*7:0*/ __VdlyVal__top__DOT__vgc_inst__DOT__tile_inst__DOT__nametable__v29;
    __VdlyVal__top__DOT__vgc_inst__DOT__tile_inst__DOT__nametable__v29 = 0;
    SData/*11:0*/ __VdlyDim0__top__DOT__vgc_inst__DOT__tile_inst__DOT__nametable__v29;
    __VdlyDim0__top__DOT__vgc_inst__DOT__tile_inst__DOT__nametable__v29 = 0;
    CData/*7:0*/ __VdlyVal__top__DOT__vgc_inst__DOT__tile_inst__DOT__attr_table__v4;
    __VdlyVal__top__DOT__vgc_inst__DOT__tile_inst__DOT__attr_table__v4 = 0;
    SData/*11:0*/ __VdlyDim0__top__DOT__vgc_inst__DOT__tile_inst__DOT__attr_table__v4;
    __VdlyDim0__top__DOT__vgc_inst__DOT__tile_inst__DOT__attr_table__v4 = 0;
    CData/*0:0*/ __VdlySet__top__DOT__vgc_inst__DOT__tile_inst__DOT__attr_table__v4;
    __VdlySet__top__DOT__vgc_inst__DOT__tile_inst__DOT__attr_table__v4 = 0;
    CData/*7:0*/ __VdlyVal__top__DOT__vgc_inst__DOT__tile_inst__DOT__nametable__v30;
    __VdlyVal__top__DOT__vgc_inst__DOT__tile_inst__DOT__nametable__v30 = 0;
    SData/*11:0*/ __VdlyDim0__top__DOT__vgc_inst__DOT__tile_inst__DOT__nametable__v30;
    __VdlyDim0__top__DOT__vgc_inst__DOT__tile_inst__DOT__nametable__v30 = 0;
    // Body
    __VdlySet__top__DOT__ram__v2 = 0U;
    __VdlySet__top__DOT__ram__v4 = 0U;
    __VdlySet__top__DOT__vgc_inst__DOT__tile_inst__DOT__tregs__v0 = 0U;
    __VdlySet__top__DOT__xmc_regs__v0 = 0U;
    __VdlySet__top__DOT__ram__v3 = 0U;
    __VdlySet__top__DOT__xram__v0 = 0U;
    __VdlySet__top__DOT__xram__v1 = 0U;
    __VdlySet__top__DOT__ram__v0 = 0U;
    __VdlySet__top__DOT__ram__v1 = 0U;
    __VdlySet__top__DOT__vgc_inst__DOT__tile_inst__DOT__tile_data__v1 = 0U;
    __Vdly__top__DOT__vgc_inst__DOT__cursor_blink = vlSelfRef.top__DOT__vgc_inst__DOT__cursor_blink;
    __Vdly__top__DOT__vgc_inst__DOT__blink_count = vlSelfRef.top__DOT__vgc_inst__DOT__blink_count;
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
    __VdlySet__top__DOT__blt_inst__DOT__regs__v20 = 0U;
    __VdlySet__top__DOT__blt_inst__DOT__regs__v22 = 0U;
    __VdlySet__top__DOT__blt_inst__DOT__regs__v24 = 0U;
    __VdlySet__top__DOT__blt_inst__DOT__regs__v25 = 0U;
    __VdlySet__top__DOT__blt_inst__DOT__regs__v30 = 0U;
    __VdlySet__top__DOT__blt_inst__DOT__regs__v32 = 0U;
    __Vdly__top__DOT__blt_inst__DOT__col = vlSelfRef.top__DOT__blt_inst__DOT__col;
    __Vdly__top__DOT__blt_inst__DOT__row = vlSelfRef.top__DOT__blt_inst__DOT__row;
    __Vdly__top__DOT__blt_inst__DOT__read_valid = vlSelfRef.top__DOT__blt_inst__DOT__read_valid;
    __Vdly__top__DOT__blt_inst__DOT__buf_idx = vlSelfRef.top__DOT__blt_inst__DOT__buf_idx;
    __VdlySet__top__DOT__blt_inst__DOT__row_buf__v0 = 0U;
    __Vdly__top__DOT__blt_inst__DOT__state = vlSelfRef.top__DOT__blt_inst__DOT__state;
    __Vdly__top__DOT__vgc_inst__DOT__h_count = vlSelfRef.top__DOT__vgc_inst__DOT__h_count;
    __Vdly__top__DOT__vgc_inst__DOT__v_count = vlSelfRef.top__DOT__vgc_inst__DOT__v_count;
    __Vdly__top__DOT__vgc_inst__DOT__gfx_color = vlSelfRef.top__DOT__vgc_inst__DOT__gfx_color;
    __Vdly__top__DOT__vgc_inst__DOT__scroll_pending 
        = vlSelfRef.top__DOT__vgc_inst__DOT__scroll_pending;
    __Vdly__top__DOT__vgc_inst__DOT__scroll_col = vlSelfRef.top__DOT__vgc_inst__DOT__scroll_col;
    __Vdly__top__DOT__vgc_inst__DOT__cmd_busy = vlSelfRef.top__DOT__vgc_inst__DOT__cmd_busy;
    __Vdly__top__DOT__vgc_inst__DOT__copper_count = vlSelfRef.top__DOT__vgc_inst__DOT__copper_count;
    __Vdly__top__DOT__vgc_inst__DOT__copper_target_list 
        = vlSelfRef.top__DOT__vgc_inst__DOT__copper_target_list;
    __Vdly__top__DOT__vgc_inst__DOT__copper_pending_list 
        = vlSelfRef.top__DOT__vgc_inst__DOT__copper_pending_list;
    __Vdly__top__DOT__vgc_inst__DOT__fill_sp = vlSelfRef.top__DOT__vgc_inst__DOT__fill_sp;
    __Vdly__top__DOT__vgc_inst__DOT__cmd_cx = vlSelfRef.top__DOT__vgc_inst__DOT__cmd_cx;
    __Vdly__top__DOT__vgc_inst__DOT__cmd_cy = vlSelfRef.top__DOT__vgc_inst__DOT__cmd_cy;
    __Vdly__top__DOT__vgc_inst__DOT__gt_font_row = vlSelfRef.top__DOT__vgc_inst__DOT__gt_font_row;
    __Vdly__top__DOT__vgc_inst__DOT__gt_char_idx = vlSelfRef.top__DOT__vgc_inst__DOT__gt_char_idx;
    __Vdly__top__DOT__vgc_inst__DOT__cmd_dx = vlSelfRef.top__DOT__vgc_inst__DOT__cmd_dx;
    __Vdly__top__DOT__vgc_inst__DOT__cmd_err = vlSelfRef.top__DOT__vgc_inst__DOT__cmd_err;
    __Vdly__top__DOT__vgc_inst__DOT__cmd_dy = vlSelfRef.top__DOT__vgc_inst__DOT__cmd_dy;
    __Vdly__top__DOT__vgc_inst__DOT__cmd_x = vlSelfRef.top__DOT__vgc_inst__DOT__cmd_x;
    __Vdly__top__DOT__vgc_inst__DOT__cmd_y = vlSelfRef.top__DOT__vgc_inst__DOT__cmd_y;
    __VdlySet__top__DOT__vgc_inst__DOT__fio_name__v0 = 0U;
    __VdlySet__top__DOT__vgc_inst__DOT__fill_stack__v0 = 0U;
    __VdlySet__top__DOT__vgc_inst__DOT__fill_stack__v4 = 0U;
    __VdlySet__top__DOT__vgc_inst__DOT__copper_list_pos__v0 = 0U;
    __VdlySet__top__DOT__vgc_inst__DOT__copper_pos__v0 = 0U;
    __VdlySet__top__DOT__vgc_inst__DOT__copper_pos__v1 = 0U;
    __Vdly__top__DOT__vgc_inst__DOT__cursor_x = vlSelfRef.top__DOT__vgc_inst__DOT__cursor_x;
    __Vdly__top__DOT__vgc_inst__DOT__cursor_y = vlSelfRef.top__DOT__vgc_inst__DOT__cursor_y;
    __VdlySet__top__DOT__vgc_inst__DOT__regs__v32 = 0U;
    __VdlySet__top__DOT__vgc_inst__DOT__regs__v33 = 0U;
    __VdlySet__top__DOT__vgc_inst__DOT__regs__v34 = 0U;
    __VdlySet__top__DOT__vgc_inst__DOT__regs__v35 = 0U;
    __VdlySet__top__DOT__vgc_inst__DOT__regs__v36 = 0U;
    __VdlySet__top__DOT__vgc_inst__DOT__regs__v37 = 0U;
    __VdlySet__top__DOT__vgc_inst__DOT__spr_shape__v16 = 0U;
    __VdlySet__top__DOT__vgc_inst__DOT__spr_trans__v16 = 0U;
    __VdlySet__top__DOT__vgc_inst__DOT__spr_y__v17 = 0U;
    __VdlySet__top__DOT__vgc_inst__DOT__spr_pri__v16 = 0U;
    __VdlySet__top__DOT__vgc_inst__DOT__spr_pri__v17 = 0U;
    __VdlySet__top__DOT__vgc_inst__DOT__spr_enable__v16 = 0U;
    __VdlySet__top__DOT__vgc_inst__DOT__spr_enable__v17 = 0U;
    __VdlySet__top__DOT__vgc_inst__DOT__spr_x__v17 = 0U;
    __VdlySet__top__DOT__vgc_inst__DOT__spr_x__v18 = 0U;
    __VdlySet__top__DOT__vgc_inst__DOT__spr_flip_h__v16 = 0U;
    __VdlySet__top__DOT__vgc_inst__DOT__spr_x__v16 = 0U;
    __Vdly__top__DOT__vgc_inst__DOT__scroll_offset 
        = vlSelfRef.top__DOT__vgc_inst__DOT__scroll_offset;
    __VdlySet__top__DOT__vgc_inst__DOT__spr_flip_h__v17 = 0U;
    __VdlySet__top__DOT__vgc_inst__DOT__tile_inst__DOT__tile_data__v0 = 0U;
    __VdlySet__top__DOT__vgc_inst__DOT__regs__v0 = 0U;
    __VdlySet__top__DOT__vgc_inst__DOT__regs__v1 = 0U;
    __Vdly__top__DOT__vgc_inst__DOT__copper_loading 
        = vlSelfRef.top__DOT__vgc_inst__DOT__copper_loading;
    __Vdly__top__DOT__vgc_inst__DOT__copper_active_list 
        = vlSelfRef.top__DOT__vgc_inst__DOT__copper_active_list;
    __VdlySet__top__DOT__vgc_inst__DOT__regs__v38 = 0U;
    __Vdly__top__DOT__vgc_inst__DOT__tile_inst__DOT__dma_state 
        = vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__dma_state;
    __Vdly__top__DOT__vgc_inst__DOT__tile_inst__DOT__dma_data_valid 
        = vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__dma_data_valid;
    __Vdly__top__DOT__vgc_inst__DOT__tile_inst__DOT__dma_count 
        = vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__dma_count;
    __Vdly__top__DOT__vgc_inst__DOT__tile_inst__DOT__dma_col_idx 
        = vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__dma_col_idx;
    __Vdly__top__DOT__vgc_inst__DOT__tile_inst__DOT__dma_src 
        = vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__dma_src;
    __Vdly__top__DOT__vgc_inst__DOT__tile_inst__DOT__dma_dst 
        = vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__dma_dst;
    __Vdly__top__DOT__vgc_inst__DOT__tile_inst__DOT__dma_pal_r 
        = vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__dma_pal_r;
    __Vdly__top__DOT__vgc_inst__DOT__tile_inst__DOT__dma_pal_phase 
        = vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__dma_pal_phase;
    __Vdly__top__DOT__vgc_inst__DOT__tile_inst__DOT__dma_pal_g 
        = vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__dma_pal_g;
    __VdlySet__top__DOT__vgc_inst__DOT__tile_inst__DOT__pal_ram__v0 = 0U;
    __VdlySet__top__DOT__vgc_inst__DOT__tile_inst__DOT__pal_ram__v1 = 0U;
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
    __VdlySet__top__DOT__vgc_inst__DOT__tile_inst__DOT__tile_data__v2 = 0U;
    __VdlySet__top__DOT__vgc_inst__DOT__tile_inst__DOT__attr_table__v0 = 0U;
    __VdlySet__top__DOT__vgc_inst__DOT__tile_inst__DOT__attr_table__v1 = 0U;
    __VdlySet__top__DOT__vgc_inst__DOT__tile_inst__DOT__attr_table__v2 = 0U;
    __VdlySet__top__DOT__vgc_inst__DOT__tile_inst__DOT__attr_table__v3 = 0U;
    __VdlySet__top__DOT__vgc_inst__DOT__tile_inst__DOT__attr_table__v4 = 0U;
    __Vdly__top__DOT__vgc_inst__DOT__copper_index = vlSelfRef.top__DOT__vgc_inst__DOT__copper_index;
    __Vdly__top__DOT__vgc_inst__DOT__fg_color = vlSelfRef.top__DOT__vgc_inst__DOT__fg_color;
    __VdlySet__top__DOT__cpu_inst__DOT__AXYS__v0 = 0U;
    if (vlSelfRef.top__DOT__blt_vgc_we) {
        if ((4U & (IData)(vlSelfRef.top__DOT__blt_vgc_space))) {
            if ((1U & (~ ((IData)(vlSelfRef.top__DOT__blt_vgc_space) 
                          >> 1U)))) {
                if ((1U & (~ (IData)(vlSelfRef.top__DOT__blt_vgc_space)))) {
                    __VdlyVal__top__DOT__vgc_inst__DOT__sprite_shapes__v13 
                        = vlSelfRef.top__DOT__blt_vgc_wdata;
                    __VdlyDim0__top__DOT__vgc_inst__DOT__sprite_shapes__v13 
                        = (0x000007ffU & (IData)(vlSelfRef.top__DOT__blt_vgc_addr));
                    vlSelfRef.__VdlyCommitQueuetop__DOT__vgc_inst__DOT__sprite_shapes.enqueue(__VdlyVal__top__DOT__vgc_inst__DOT__sprite_shapes__v13, (IData)(__VdlyDim0__top__DOT__vgc_inst__DOT__sprite_shapes__v13));
                }
            }
        }
    }
    if (vlSelfRef.top__DOT__blt_vgc_we) {
        if ((1U & (~ ((IData)(vlSelfRef.top__DOT__blt_vgc_space) 
                      >> 2U)))) {
            if ((2U & (IData)(vlSelfRef.top__DOT__blt_vgc_space))) {
                if ((1U & (IData)(vlSelfRef.top__DOT__blt_vgc_space))) {
                    vlSelfRef.top__DOT__vgc_inst__DOT____Vlvbound_h90cd3135__0 
                        = (0x0000000fU & (IData)(vlSelfRef.top__DOT__blt_vgc_wdata));
                    if ((0xf9ffU >= (IData)(vlSelfRef.top__DOT__blt_vgc_addr))) {
                        __VdlyVal__top__DOT__vgc_inst__DOT__gfx_ram__v18 
                            = vlSelfRef.top__DOT__vgc_inst__DOT____Vlvbound_h90cd3135__0;
                        __VdlyDim0__top__DOT__vgc_inst__DOT__gfx_ram__v18 
                            = vlSelfRef.top__DOT__blt_vgc_addr;
                        vlSelfRef.__VdlyCommitQueuetop__DOT__vgc_inst__DOT__gfx_ram.enqueue(__VdlyVal__top__DOT__vgc_inst__DOT__gfx_ram__v18, (IData)(__VdlyDim0__top__DOT__vgc_inst__DOT__gfx_ram__v18));
                    }
                }
            }
        }
    }
    if (vlSelfRef.top__DOT__blt_vgc_we) {
        if ((1U & (~ ((IData)(vlSelfRef.top__DOT__blt_vgc_space) 
                      >> 2U)))) {
            if ((2U & (IData)(vlSelfRef.top__DOT__blt_vgc_space))) {
                if ((1U & (~ (IData)(vlSelfRef.top__DOT__blt_vgc_space)))) {
                    vlSelfRef.top__DOT__vgc_inst__DOT____Vlvbound_h5a8b2575__0 
                        = vlSelfRef.top__DOT__blt_vgc_wdata;
                    if ((0x07cfU >= (0x000007ffU & (IData)(vlSelfRef.top__DOT__blt_vgc_addr)))) {
                        __VdlyVal__top__DOT__vgc_inst__DOT__color_ram__v6 
                            = vlSelfRef.top__DOT__vgc_inst__DOT____Vlvbound_h5a8b2575__0;
                        __VdlyDim0__top__DOT__vgc_inst__DOT__color_ram__v6 
                            = (0x000007ffU & (IData)(vlSelfRef.top__DOT__blt_vgc_addr));
                        vlSelfRef.__VdlyCommitQueuetop__DOT__vgc_inst__DOT__color_ram.enqueue(__VdlyVal__top__DOT__vgc_inst__DOT__color_ram__v6, (IData)(__VdlyDim0__top__DOT__vgc_inst__DOT__color_ram__v6));
                    }
                }
            }
        }
    }
    if (vlSelfRef.top__DOT__blt_vgc_we) {
        if ((1U & (~ ((IData)(vlSelfRef.top__DOT__blt_vgc_space) 
                      >> 2U)))) {
            if ((1U & (~ ((IData)(vlSelfRef.top__DOT__blt_vgc_space) 
                          >> 1U)))) {
                if ((1U & (IData)(vlSelfRef.top__DOT__blt_vgc_space))) {
                    vlSelfRef.top__DOT__vgc_inst__DOT____Vlvbound_h28c4f18f__0 
                        = vlSelfRef.top__DOT__blt_vgc_wdata;
                    if ((0x07cfU >= (0x000007ffU & (IData)(vlSelfRef.top__DOT__blt_vgc_addr)))) {
                        __VdlyVal__top__DOT__vgc_inst__DOT__char_ram__v7 
                            = vlSelfRef.top__DOT__vgc_inst__DOT____Vlvbound_h28c4f18f__0;
                        __VdlyDim0__top__DOT__vgc_inst__DOT__char_ram__v7 
                            = (0x000007ffU & (IData)(vlSelfRef.top__DOT__blt_vgc_addr));
                        vlSelfRef.__VdlyCommitQueuetop__DOT__vgc_inst__DOT__char_ram.enqueue(__VdlyVal__top__DOT__vgc_inst__DOT__char_ram__v7, (IData)(__VdlyDim0__top__DOT__vgc_inst__DOT__char_ram__v7));
                    }
                }
            }
        }
    }
    if (vlSelfRef.top__DOT__blt_vgc_we) {
        if ((4U & (IData)(vlSelfRef.top__DOT__blt_vgc_space))) {
            if ((2U & (IData)(vlSelfRef.top__DOT__blt_vgc_space))) {
                if ((1U & (~ (IData)(vlSelfRef.top__DOT__blt_vgc_space)))) {
                    __VdlyVal__top__DOT__vgc_inst__DOT__tile_inst__DOT__tile_data__v1 
                        = vlSelfRef.top__DOT__blt_vgc_wdata;
                    __VdlyDim0__top__DOT__vgc_inst__DOT__tile_inst__DOT__tile_data__v1 
                        = (0x00007fffU & (IData)(vlSelfRef.top__DOT__blt_vgc_addr));
                    __VdlySet__top__DOT__vgc_inst__DOT__tile_inst__DOT__tile_data__v1 = 1U;
                }
            }
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
        if (((IData)(vlSelfRef.top__DOT__xmc_win_sel) 
             & (IData)(vlSelfRef.top__DOT__xmc_win_enabled))) {
            __VdlyVal__top__DOT__xram__v0 = vlSelfRef.top__DOT__cpu_dout;
            __VdlyDim0__top__DOT__xram__v0 = vlSelfRef.top__DOT__xmc_addr;
            __VdlySet__top__DOT__xram__v0 = 1U;
        }
    }
    if (((IData)(vlSelfRef.top__DOT__clear_input) & 
         (~ (IData)(vlSelfRef.key_valid)))) {
        __VdlySet__top__DOT__ram__v3 = 1U;
    }
    if (vlSelfRef.top__DOT__blt_xram_we) {
        __VdlyVal__top__DOT__xram__v1 = vlSelfRef.top__DOT__blt_xram_wdata;
        __VdlyDim0__top__DOT__xram__v1 = vlSelfRef.top__DOT__blt_xram_addr;
        __VdlySet__top__DOT__xram__v1 = 1U;
    }
    if (((IData)(vlSelfRef.top__DOT__blt_ram_we) & 
         (0xc000U > (IData)(vlSelfRef.top__DOT__blt_ram_addr)))) {
        __VdlyVal__top__DOT__ram__v1 = vlSelfRef.top__DOT__blt_ram_wdata;
        __VdlyDim0__top__DOT__ram__v1 = vlSelfRef.top__DOT__blt_ram_addr;
        __VdlySet__top__DOT__ram__v1 = 1U;
    }
    if (((IData)(vlSelfRef.top__DOT__cpu_inst__DOT__NMI_edge) 
         & (0x0bU == (IData)(vlSelfRef.top__DOT__cpu_inst__DOT__state)))) {
        vlSelfRef.top__DOT__cpu_inst__DOT__NMI_edge = 0U;
    } else if ((1U & ((~ (IData)(vlSelfRef.nmi_n)) 
                      & (~ (IData)(vlSelfRef.top__DOT__cpu_inst__DOT__NMI_1))))) {
        vlSelfRef.top__DOT__cpu_inst__DOT__NMI_edge = 1U;
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
    if ((1U & (~ (IData)(vlSelfRef.rst)))) {
        if (((IData)(vlSelfRef.top__DOT__cpu_we) & (IData)(vlSelfRef.top__DOT__vgc_inst__DOT__tile_reg_sel))) {
            __VdlyVal__top__DOT__vgc_inst__DOT__tile_inst__DOT__tregs__v0 
                = vlSelfRef.top__DOT__cpu_dout;
            __VdlyDim0__top__DOT__vgc_inst__DOT__tile_inst__DOT__tregs__v0 
                = (0x0000001fU & (IData)(vlSelfRef.top__DOT__cpu_addr));
            __VdlySet__top__DOT__vgc_inst__DOT__tile_inst__DOT__tregs__v0 = 1U;
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
                                    {
                                        __Vfunc_top__DOT__blt_inst__DOT__rect_fits__0__Vfuncout = 0;
                                        __Vfunc_top__DOT__blt_inst__DOT__rect_fits__0__last_row_start = 0;
                                        __Vfunc_top__DOT__blt_inst__DOT__rect_fits__0__last_byte = 0;
                                        if ((((0U == (IData)(__Vfunc_top__DOT__blt_inst__DOT__rect_fits__0__w)) 
                                              | (0U 
                                                 == (IData)(__Vfunc_top__DOT__blt_inst__DOT__rect_fits__0__h))) 
                                             | (0U 
                                                == __Vfunc_top__DOT__blt_inst__DOT__rect_fits__0__sz))) {
                                            __Vfunc_top__DOT__blt_inst__DOT__rect_fits__0__Vfuncout = 0U;
                                            goto __Vlabel0;
                                        }
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
                                        __Vlabel0: ;
                                    }
                                }(), (IData)(__Vfunc_top__DOT__blt_inst__DOT__rect_fits__0__Vfuncout)))))) {
                __VdlySet__top__DOT__blt_inst__DOT__regs__v18 = 1U;
                __Vdly__top__DOT__blt_inst__DOT__state = 6U;
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
                        {
                            __Vfunc_top__DOT__blt_inst__DOT__rect_fits__2__Vfuncout = 0;
                            __Vfunc_top__DOT__blt_inst__DOT__rect_fits__2__last_row_start = 0;
                            __Vfunc_top__DOT__blt_inst__DOT__rect_fits__2__last_byte = 0;
                            if ((((0U == (IData)(__Vfunc_top__DOT__blt_inst__DOT__rect_fits__2__w)) 
                                  | (0U == (IData)(__Vfunc_top__DOT__blt_inst__DOT__rect_fits__2__h))) 
                                 | (0U == __Vfunc_top__DOT__blt_inst__DOT__rect_fits__2__sz))) {
                                __Vfunc_top__DOT__blt_inst__DOT__rect_fits__2__Vfuncout = 0U;
                                goto __Vlabel1;
                            }
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
                            __Vlabel1: ;
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
                            {
                                __Vfunc_top__DOT__blt_inst__DOT__write_protect_ok__4__Vfuncout = 0;
                                __Vfunc_top__DOT__blt_inst__DOT__write_protect_ok__4__last_byte = 0;
                                if ((0U != (IData)(__Vfunc_top__DOT__blt_inst__DOT__write_protect_ok__4__space))) {
                                    __Vfunc_top__DOT__blt_inst__DOT__write_protect_ok__4__Vfuncout = 1U;
                                    goto __Vlabel2;
                                }
                                __Vfunc_top__DOT__blt_inst__DOT__write_protect_ok__4__last_byte 
                                    = ((__Vfunc_top__DOT__blt_inst__DOT__write_protect_ok__4__base 
                                        + (((IData)(__Vfunc_top__DOT__blt_inst__DOT__write_protect_ok__4__h) 
                                            - (IData)(1U)) 
                                           * (IData)(__Vfunc_top__DOT__blt_inst__DOT__write_protect_ok__4__stride))) 
                                       + (IData)(__Vfunc_top__DOT__blt_inst__DOT__write_protect_ok__4__w));
                                __Vfunc_top__DOT__blt_inst__DOT__write_protect_ok__4__Vfuncout 
                                    = (0x0000c000U 
                                       >= __Vfunc_top__DOT__blt_inst__DOT__write_protect_ok__4__last_byte);
                                __Vlabel2: ;
                            }
                        }(), (IData)(__Vfunc_top__DOT__blt_inst__DOT__write_protect_ok__4__Vfuncout))) {
                    if (vlSelfRef.top__DOT__blt_inst__DOT__fill_mode) {
                        __Vdly__top__DOT__blt_inst__DOT__state = 3U;
                        __Vdly__top__DOT__blt_inst__DOT__read_valid = 1U;
                        vlSelfRef.top__DOT__blt_inst__DOT__read_byte 
                            = vlSelfRef.top__DOT__blt_inst__DOT__fill_value;
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
                }
            } else {
                __VdlySet__top__DOT__blt_inst__DOT__regs__v22 = 1U;
                __Vdly__top__DOT__blt_inst__DOT__state = 6U;
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
                        vlSelfRef.top__DOT__blt_inst__DOT__src_space 
                            = (7U & vlSelfRef.top__DOT__blt_inst__DOT__regs[3U]);
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
                        __Vdly__top__DOT__blt_inst__DOT__row = 0U;
                        __Vdly__top__DOT__blt_inst__DOT__col = 0U;
                        __Vdly__top__DOT__blt_inst__DOT__wrote_count = 0U;
                        __Vdly__top__DOT__blt_inst__DOT__read_valid = 0U;
                        __Vdly__top__DOT__blt_inst__DOT__buf_idx = 0U;
                        __VdlySet__top__DOT__blt_inst__DOT__regs__v25 = 1U;
                        __Vdly__top__DOT__blt_inst__DOT__state = 1U;
                    } else {
                        __VdlySet__top__DOT__blt_inst__DOT__regs__v30 = 1U;
                    }
                } else {
                    __VdlySet__top__DOT__blt_inst__DOT__regs__v32 = 1U;
                }
            }
        }
    }
    if ((1U & (~ (IData)(vlSelfRef.rst)))) {
        if (vlSelfRef.top__DOT____Vcellinp__cpu_inst__RDY) {
            if (((0x1eU == (IData)(vlSelfRef.top__DOT__cpu_inst__DOT__state)) 
                 | (0x21U == (IData)(vlSelfRef.top__DOT__cpu_inst__DOT__state)))) {
                vlSelfRef.top__DOT__cpu_inst__DOT__IRHOLD 
                    = vlSelfRef.top__DOT__cpu_inst__DOT__DIMUX;
            }
        }
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
    vlSelfRef.vid_de = vlSelfRef.top__DOT__vgc_inst__DOT__visible;
    vlSelfRef.vid_hsync = (1U & (~ ((0x0290U <= (IData)(vlSelfRef.top__DOT__vgc_inst__DOT__h_count)) 
                                    & (0x02f0U > (IData)(vlSelfRef.top__DOT__vgc_inst__DOT__h_count)))));
    vlSelfRef.vid_vsync = (1U & (~ ((0x01eaU <= (IData)(vlSelfRef.top__DOT__vgc_inst__DOT__v_count)) 
                                    & (0x01ecU > (IData)(vlSelfRef.top__DOT__vgc_inst__DOT__v_count)))));
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
    if (vlSelfRef.rst) {
        __Vdly__top__DOT__vgc_inst__DOT__h_count = 0U;
        __Vdly__top__DOT__vgc_inst__DOT__v_count = 0U;
        vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__trans_color = 0U;
        vlSelfRef.top__DOT__cpu_inst__DOT__res = 1U;
        vlSelfRef.top__DOT__cpu_inst__DOT__IRHOLD_valid = 0U;
        vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__scroll_y = 0U;
        vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__scroll_x = 0U;
        vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__mirror_mode = 0U;
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
        if (((IData)(vlSelfRef.top__DOT__cpu_we) & (IData)(vlSelfRef.top__DOT__vgc_inst__DOT__tile_reg_sel))) {
            if ((0U != (0x0000001fU & (IData)(vlSelfRef.top__DOT__cpu_addr)))) {
                if ((1U == (0x0000001fU & (IData)(vlSelfRef.top__DOT__cpu_addr)))) {
                    vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__trans_color 
                        = (0x0000000fU & (IData)(vlSelfRef.top__DOT__cpu_dout));
                }
                if ((1U != (0x0000001fU & (IData)(vlSelfRef.top__DOT__cpu_addr)))) {
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
                        }
                    }
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
                }
            }
            if ((0U == (0x0000001fU & (IData)(vlSelfRef.top__DOT__cpu_addr)))) {
                vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__mirror_mode 
                    = (3U & ((IData)(vlSelfRef.top__DOT__cpu_dout) 
                             >> 1U));
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
        __Vdly__top__DOT__vgc_inst__DOT__tile_inst__DOT__dma_state = 0U;
        __Vdly__top__DOT__vgc_inst__DOT__tile_inst__DOT__dma_data_valid = 0U;
    } else {
        __Vdly__top__DOT__vgc_inst__DOT__tile_inst__DOT__dma_data_valid = 0U;
        if ((8U & (IData)(vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__dma_state))) {
            if ((4U & (IData)(vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__dma_state))) {
                __Vdly__top__DOT__vgc_inst__DOT__tile_inst__DOT__dma_state = 0U;
            } else if ((2U & (IData)(vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__dma_state))) {
                __Vdly__top__DOT__vgc_inst__DOT__tile_inst__DOT__dma_state = 0U;
            } else if ((1U & (IData)(vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__dma_state))) {
                if ((0x0fa0U > (IData)(vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__dma_count))) {
                    vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT____Vlvbound_hf26d05a2__0 = 0U;
                    if ((0x0f9fU >= (0x00000fffU & (IData)(vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__dma_count)))) {
                        __VdlyVal__top__DOT__vgc_inst__DOT__tile_inst__DOT__nametable__v0 
                            = vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT____Vlvbound_hf26d05a2__0;
                        __VdlyDim0__top__DOT__vgc_inst__DOT__tile_inst__DOT__nametable__v0 
                            = (0x00000fffU & (IData)(vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__dma_count));
                        vlSelfRef.__VdlyCommitQueuetop__DOT__vgc_inst__DOT__tile_inst__DOT__nametable.enqueue(__VdlyVal__top__DOT__vgc_inst__DOT__tile_inst__DOT__nametable__v0, (IData)(__VdlyDim0__top__DOT__vgc_inst__DOT__tile_inst__DOT__nametable__v0));
                    }
                    vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT____Vlvbound_hf7076230__0 = 0U;
                    if ((0x0f9fU >= (0x00000fffU & (IData)(vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__dma_count)))) {
                        __VdlyVal__top__DOT__vgc_inst__DOT__tile_inst__DOT__attr_table__v0 
                            = vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT____Vlvbound_hf7076230__0;
                        __VdlyDim0__top__DOT__vgc_inst__DOT__tile_inst__DOT__attr_table__v0 
                            = (0x00000fffU & (IData)(vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__dma_count));
                        __VdlySet__top__DOT__vgc_inst__DOT__tile_inst__DOT__attr_table__v0 = 1U;
                    }
                    __Vdly__top__DOT__vgc_inst__DOT__tile_inst__DOT__dma_count 
                        = (0x0000ffffU & ((IData)(1U) 
                                          + (IData)(vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__dma_count)));
                } else {
                    __Vdly__top__DOT__vgc_inst__DOT__tile_inst__DOT__dma_state = 0U;
                }
            } else {
                if (vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__dma_data_valid) {
                    vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT____Vlvbound_h8c784627__0 
                        = vlSelfRef.top__DOT__tile_dma_data;
                    __Vdly__top__DOT__vgc_inst__DOT__tile_inst__DOT__dma_count 
                        = (0x0000ffffU & ((IData)(vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__dma_count) 
                                          - (IData)(1U)));
                    if ((0x0f9fU >= (0x00000fffU & 
                                     ((((IData)(0x000003e8U) 
                                        * (IData)(vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__dma_nt)) 
                                       + ((IData)(0x00000028U) 
                                          * (IData)(vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__dma_col_idx))) 
                                      + (IData)(vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__dma_col_row))))) {
                        __VdlyVal__top__DOT__vgc_inst__DOT__tile_inst__DOT__attr_table__v1 
                            = vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT____Vlvbound_h8c784627__0;
                        __VdlyDim0__top__DOT__vgc_inst__DOT__tile_inst__DOT__attr_table__v1 
                            = (0x00000fffU & ((((IData)(0x000003e8U) 
                                                * (IData)(vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__dma_nt)) 
                                               + ((IData)(0x00000028U) 
                                                  * (IData)(vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__dma_col_idx))) 
                                              + (IData)(vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__dma_col_row)));
                        __VdlySet__top__DOT__vgc_inst__DOT__tile_inst__DOT__attr_table__v1 = 1U;
                    }
                    __Vdly__top__DOT__vgc_inst__DOT__tile_inst__DOT__dma_col_idx 
                        = (0x0000001fU & ((IData)(1U) 
                                          + (IData)(vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__dma_col_idx)));
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
                        vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT____Vlvbound_h02aa0280__0 
                            = vlSelfRef.top__DOT__tile_dma_data;
                        __Vdly__top__DOT__vgc_inst__DOT__tile_inst__DOT__dma_count 
                            = (0x0000ffffU & ((IData)(vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__dma_count) 
                                              - (IData)(1U)));
                        if ((0x0f9fU >= (0x00000fffU 
                                         & ((((IData)(0x000003e8U) 
                                              * (IData)(vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__dma_nt)) 
                                             + ((IData)(0x00000028U) 
                                                * (IData)(vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__dma_col_row))) 
                                            + (0x0000003fU 
                                               & (IData)(vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__dma_dst)))))) {
                            __VdlyVal__top__DOT__vgc_inst__DOT__tile_inst__DOT__attr_table__v2 
                                = vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT____Vlvbound_h02aa0280__0;
                            __VdlyDim0__top__DOT__vgc_inst__DOT__tile_inst__DOT__attr_table__v2 
                                = (0x00000fffU & ((
                                                   ((IData)(0x000003e8U) 
                                                    * (IData)(vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__dma_nt)) 
                                                   + 
                                                   ((IData)(0x00000028U) 
                                                    * (IData)(vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__dma_col_row))) 
                                                  + 
                                                  (0x0000003fU 
                                                   & (IData)(vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__dma_dst))));
                            __VdlySet__top__DOT__vgc_inst__DOT__tile_inst__DOT__attr_table__v2 = 1U;
                        }
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
                        vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT____Vlvbound_h21da2949__0 
                            = vlSelfRef.top__DOT__tile_dma_data;
                        __Vdly__top__DOT__vgc_inst__DOT__tile_inst__DOT__dma_count 
                            = (0x0000ffffU & ((IData)(vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__dma_count) 
                                              - (IData)(1U)));
                        if ((0x0f9fU >= (0x00000fffU 
                                         & ((((IData)(0x000003e8U) 
                                              * (IData)(vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__dma_nt)) 
                                             + ((IData)(0x00000028U) 
                                                * (IData)(vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__dma_col_idx))) 
                                            + (IData)(vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__dma_col_row))))) {
                            __VdlyVal__top__DOT__vgc_inst__DOT__tile_inst__DOT__nametable__v1 
                                = vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT____Vlvbound_h21da2949__0;
                            __VdlyDim0__top__DOT__vgc_inst__DOT__tile_inst__DOT__nametable__v1 
                                = (0x00000fffU & ((
                                                   ((IData)(0x000003e8U) 
                                                    * (IData)(vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__dma_nt)) 
                                                   + 
                                                   ((IData)(0x00000028U) 
                                                    * (IData)(vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__dma_col_idx))) 
                                                  + (IData)(vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__dma_col_row)));
                            vlSelfRef.__VdlyCommitQueuetop__DOT__vgc_inst__DOT__tile_inst__DOT__nametable.enqueue(__VdlyVal__top__DOT__vgc_inst__DOT__tile_inst__DOT__nametable__v1, (IData)(__VdlyDim0__top__DOT__vgc_inst__DOT__tile_inst__DOT__nametable__v1));
                        }
                        __Vdly__top__DOT__vgc_inst__DOT__tile_inst__DOT__dma_col_idx 
                            = (0x0000001fU & ((IData)(1U) 
                                              + (IData)(vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__dma_col_idx)));
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
                    vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT____Vlvbound_h17e6e432__0 
                        = vlSelfRef.top__DOT__tile_dma_data;
                    __Vdly__top__DOT__vgc_inst__DOT__tile_inst__DOT__dma_count 
                        = (0x0000ffffU & ((IData)(vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__dma_count) 
                                          - (IData)(1U)));
                    if ((0x0f9fU >= (0x00000fffU & 
                                     ((((IData)(0x000003e8U) 
                                        * (IData)(vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__dma_nt)) 
                                       + ((IData)(0x00000028U) 
                                          * (IData)(vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__dma_col_row))) 
                                      + (0x0000003fU 
                                         & (IData)(vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__dma_dst)))))) {
                        __VdlyVal__top__DOT__vgc_inst__DOT__tile_inst__DOT__nametable__v2 
                            = vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT____Vlvbound_h17e6e432__0;
                        __VdlyDim0__top__DOT__vgc_inst__DOT__tile_inst__DOT__nametable__v2 
                            = (0x00000fffU & ((((IData)(0x000003e8U) 
                                                * (IData)(vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__dma_nt)) 
                                               + ((IData)(0x00000028U) 
                                                  * (IData)(vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__dma_col_row))) 
                                              + (0x0000003fU 
                                                 & (IData)(vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__dma_dst))));
                        vlSelfRef.__VdlyCommitQueuetop__DOT__vgc_inst__DOT__tile_inst__DOT__nametable.enqueue(__VdlyVal__top__DOT__vgc_inst__DOT__tile_inst__DOT__nametable__v2, (IData)(__VdlyDim0__top__DOT__vgc_inst__DOT__tile_inst__DOT__nametable__v2));
                    }
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
                        __Vdly__top__DOT__vgc_inst__DOT__tile_inst__DOT__dma_pal_r 
                            = vlSelfRef.top__DOT__tile_dma_data;
                        __Vdly__top__DOT__vgc_inst__DOT__tile_inst__DOT__dma_pal_phase = 1U;
                    } else if ((1U == (IData)(vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__dma_pal_phase))) {
                        __Vdly__top__DOT__vgc_inst__DOT__tile_inst__DOT__dma_pal_g 
                            = vlSelfRef.top__DOT__tile_dma_data;
                        __Vdly__top__DOT__vgc_inst__DOT__tile_inst__DOT__dma_pal_phase = 2U;
                    } else if ((2U == (IData)(vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__dma_pal_phase))) {
                        __VdlyVal__top__DOT__vgc_inst__DOT__tile_inst__DOT__pal_ram__v0 
                            = ((0x00000f00U & ((IData)(vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__dma_pal_r) 
                                               << 4U)) 
                               | ((0x000000f0U & (IData)(vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__dma_pal_g)) 
                                  | (0x0000000fU & 
                                     ((IData)(vlSelfRef.top__DOT__tile_dma_data) 
                                      >> 4U))));
                        __VdlyDim0__top__DOT__vgc_inst__DOT__tile_inst__DOT__pal_ram__v0 
                            = (((IData)(vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__dma_pal_sub) 
                                << 4U) | (IData)(vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__dma_pal_cidx));
                        __VdlySet__top__DOT__vgc_inst__DOT__tile_inst__DOT__pal_ram__v0 = 1U;
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
                    vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT____Vlvbound_h3335cc4a__0 
                        = vlSelfRef.top__DOT__tile_dma_data;
                    __Vdly__top__DOT__vgc_inst__DOT__tile_inst__DOT__dma_count 
                        = (0x0000ffffU & ((IData)(vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__dma_count) 
                                          - (IData)(1U)));
                    if ((0x0f9fU >= (0x00000fffU & 
                                     (((IData)(0x000003e8U) 
                                       * (IData)(vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__dma_nt)) 
                                      + (0x000003ffU 
                                         & (IData)(vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__dma_dst)))))) {
                        __VdlyVal__top__DOT__vgc_inst__DOT__tile_inst__DOT__attr_table__v3 
                            = vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT____Vlvbound_h3335cc4a__0;
                        __VdlyDim0__top__DOT__vgc_inst__DOT__tile_inst__DOT__attr_table__v3 
                            = (0x00000fffU & (((IData)(0x000003e8U) 
                                               * (IData)(vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__dma_nt)) 
                                              + (0x000003ffU 
                                                 & (IData)(vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__dma_dst))));
                        __VdlySet__top__DOT__vgc_inst__DOT__tile_inst__DOT__attr_table__v3 = 1U;
                    }
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
                    vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT____Vlvbound_ha65a9a44__0 
                        = vlSelfRef.top__DOT__tile_dma_data;
                    __Vdly__top__DOT__vgc_inst__DOT__tile_inst__DOT__dma_count 
                        = (0x0000ffffU & ((IData)(vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__dma_count) 
                                          - (IData)(1U)));
                    if ((0x0f9fU >= (0x00000fffU & 
                                     (((IData)(0x000003e8U) 
                                       * (IData)(vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__dma_nt)) 
                                      + (0x000003ffU 
                                         & (IData)(vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__dma_dst)))))) {
                        __VdlyVal__top__DOT__vgc_inst__DOT__tile_inst__DOT__nametable__v3 
                            = vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT____Vlvbound_ha65a9a44__0;
                        __VdlyDim0__top__DOT__vgc_inst__DOT__tile_inst__DOT__nametable__v3 
                            = (0x00000fffU & (((IData)(0x000003e8U) 
                                               * (IData)(vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__dma_nt)) 
                                              + (0x000003ffU 
                                                 & (IData)(vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__dma_dst))));
                        vlSelfRef.__VdlyCommitQueuetop__DOT__vgc_inst__DOT__tile_inst__DOT__nametable.enqueue(__VdlyVal__top__DOT__vgc_inst__DOT__tile_inst__DOT__nametable__v3, (IData)(__VdlyDim0__top__DOT__vgc_inst__DOT__tile_inst__DOT__nametable__v3));
                    }
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
                __VdlyVal__top__DOT__vgc_inst__DOT__tile_inst__DOT__tile_data__v2 
                    = vlSelfRef.top__DOT__tile_dma_data;
                __VdlyDim0__top__DOT__vgc_inst__DOT__tile_inst__DOT__tile_data__v2 
                    = (0x00007fffU & (IData)(vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__dma_dst));
                __VdlySet__top__DOT__vgc_inst__DOT__tile_inst__DOT__tile_data__v2 = 1U;
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
                    if ((2U != (0x0000001fU & (IData)(vlSelfRef.top__DOT__cpu_addr)))) {
                        if ((3U != (0x0000001fU & (IData)(vlSelfRef.top__DOT__cpu_addr)))) {
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
                                                                                vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT____Vlvbound_h021f5fae__0 
                                                                                = vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__col_buffer[0U];
                                                                                if (
                                                                                (0x0f9fU 
                                                                                >= 
                                                                                (0x00000fffU 
                                                                                & (((IData)(0x000003e8U) 
                                                                                * vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__tregs[8U]) 
                                                                                + vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__tregs[9U])))) {
                                                                                __VdlyVal__top__DOT__vgc_inst__DOT__tile_inst__DOT__nametable__v4 
                                                                                = vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT____Vlvbound_h021f5fae__0;
                                                                                __VdlyDim0__top__DOT__vgc_inst__DOT__tile_inst__DOT__nametable__v4 
                                                                                = 
                                                                                (0x00000fffU 
                                                                                & (((IData)(0x000003e8U) 
                                                                                * vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__tregs[8U]) 
                                                                                + vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__tregs[9U]));
                                                                                vlSelfRef.__VdlyCommitQueuetop__DOT__vgc_inst__DOT__tile_inst__DOT__nametable.enqueue(__VdlyVal__top__DOT__vgc_inst__DOT__tile_inst__DOT__nametable__v4, (IData)(__VdlyDim0__top__DOT__vgc_inst__DOT__tile_inst__DOT__nametable__v4));
                                                                                }
                                                                                vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT____Vlvbound_h021f5fae__0 
                                                                                = vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__col_buffer[1U];
                                                                                if (
                                                                                (0x0f9fU 
                                                                                >= 
                                                                                (0x00000fffU 
                                                                                & ((IData)(0x0028U) 
                                                                                + 
                                                                                (((IData)(0x000003e8U) 
                                                                                * vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__tregs[8U]) 
                                                                                + vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__tregs[9U]))))) {
                                                                                __VdlyVal__top__DOT__vgc_inst__DOT__tile_inst__DOT__nametable__v5 
                                                                                = vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT____Vlvbound_h021f5fae__0;
                                                                                __VdlyDim0__top__DOT__vgc_inst__DOT__tile_inst__DOT__nametable__v5 
                                                                                = 
                                                                                (0x00000fffU 
                                                                                & ((IData)(0x0028U) 
                                                                                + 
                                                                                (((IData)(0x000003e8U) 
                                                                                * vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__tregs[8U]) 
                                                                                + vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__tregs[9U])));
                                                                                vlSelfRef.__VdlyCommitQueuetop__DOT__vgc_inst__DOT__tile_inst__DOT__nametable.enqueue(__VdlyVal__top__DOT__vgc_inst__DOT__tile_inst__DOT__nametable__v5, (IData)(__VdlyDim0__top__DOT__vgc_inst__DOT__tile_inst__DOT__nametable__v5));
                                                                                }
                                                                                vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT____Vlvbound_h021f5fae__0 
                                                                                = vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__col_buffer[2U];
                                                                                if (
                                                                                (0x0f9fU 
                                                                                >= 
                                                                                (0x00000fffU 
                                                                                & ((IData)(0x0050U) 
                                                                                + 
                                                                                (((IData)(0x000003e8U) 
                                                                                * vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__tregs[8U]) 
                                                                                + vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__tregs[9U]))))) {
                                                                                __VdlyVal__top__DOT__vgc_inst__DOT__tile_inst__DOT__nametable__v6 
                                                                                = vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT____Vlvbound_h021f5fae__0;
                                                                                __VdlyDim0__top__DOT__vgc_inst__DOT__tile_inst__DOT__nametable__v6 
                                                                                = 
                                                                                (0x00000fffU 
                                                                                & ((IData)(0x0050U) 
                                                                                + 
                                                                                (((IData)(0x000003e8U) 
                                                                                * vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__tregs[8U]) 
                                                                                + vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__tregs[9U])));
                                                                                vlSelfRef.__VdlyCommitQueuetop__DOT__vgc_inst__DOT__tile_inst__DOT__nametable.enqueue(__VdlyVal__top__DOT__vgc_inst__DOT__tile_inst__DOT__nametable__v6, (IData)(__VdlyDim0__top__DOT__vgc_inst__DOT__tile_inst__DOT__nametable__v6));
                                                                                }
                                                                                vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT____Vlvbound_h021f5fae__0 
                                                                                = vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__col_buffer[3U];
                                                                                if (
                                                                                (0x0f9fU 
                                                                                >= 
                                                                                (0x00000fffU 
                                                                                & ((IData)(0x0078U) 
                                                                                + 
                                                                                (((IData)(0x000003e8U) 
                                                                                * vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__tregs[8U]) 
                                                                                + vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__tregs[9U]))))) {
                                                                                __VdlyVal__top__DOT__vgc_inst__DOT__tile_inst__DOT__nametable__v7 
                                                                                = vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT____Vlvbound_h021f5fae__0;
                                                                                __VdlyDim0__top__DOT__vgc_inst__DOT__tile_inst__DOT__nametable__v7 
                                                                                = 
                                                                                (0x00000fffU 
                                                                                & ((IData)(0x0078U) 
                                                                                + 
                                                                                (((IData)(0x000003e8U) 
                                                                                * vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__tregs[8U]) 
                                                                                + vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__tregs[9U])));
                                                                                vlSelfRef.__VdlyCommitQueuetop__DOT__vgc_inst__DOT__tile_inst__DOT__nametable.enqueue(__VdlyVal__top__DOT__vgc_inst__DOT__tile_inst__DOT__nametable__v7, (IData)(__VdlyDim0__top__DOT__vgc_inst__DOT__tile_inst__DOT__nametable__v7));
                                                                                }
                                                                                vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT____Vlvbound_h021f5fae__0 
                                                                                = vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__col_buffer[4U];
                                                                                if (
                                                                                (0x0f9fU 
                                                                                >= 
                                                                                (0x00000fffU 
                                                                                & ((IData)(0x00a0U) 
                                                                                + 
                                                                                (((IData)(0x000003e8U) 
                                                                                * vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__tregs[8U]) 
                                                                                + vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__tregs[9U]))))) {
                                                                                __VdlyVal__top__DOT__vgc_inst__DOT__tile_inst__DOT__nametable__v8 
                                                                                = vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT____Vlvbound_h021f5fae__0;
                                                                                __VdlyDim0__top__DOT__vgc_inst__DOT__tile_inst__DOT__nametable__v8 
                                                                                = 
                                                                                (0x00000fffU 
                                                                                & ((IData)(0x00a0U) 
                                                                                + 
                                                                                (((IData)(0x000003e8U) 
                                                                                * vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__tregs[8U]) 
                                                                                + vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__tregs[9U])));
                                                                                vlSelfRef.__VdlyCommitQueuetop__DOT__vgc_inst__DOT__tile_inst__DOT__nametable.enqueue(__VdlyVal__top__DOT__vgc_inst__DOT__tile_inst__DOT__nametable__v8, (IData)(__VdlyDim0__top__DOT__vgc_inst__DOT__tile_inst__DOT__nametable__v8));
                                                                                }
                                                                                vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT____Vlvbound_h021f5fae__0 
                                                                                = vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__col_buffer[5U];
                                                                                if (
                                                                                (0x0f9fU 
                                                                                >= 
                                                                                (0x00000fffU 
                                                                                & ((IData)(0x00c8U) 
                                                                                + 
                                                                                (((IData)(0x000003e8U) 
                                                                                * vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__tregs[8U]) 
                                                                                + vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__tregs[9U]))))) {
                                                                                __VdlyVal__top__DOT__vgc_inst__DOT__tile_inst__DOT__nametable__v9 
                                                                                = vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT____Vlvbound_h021f5fae__0;
                                                                                __VdlyDim0__top__DOT__vgc_inst__DOT__tile_inst__DOT__nametable__v9 
                                                                                = 
                                                                                (0x00000fffU 
                                                                                & ((IData)(0x00c8U) 
                                                                                + 
                                                                                (((IData)(0x000003e8U) 
                                                                                * vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__tregs[8U]) 
                                                                                + vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__tregs[9U])));
                                                                                vlSelfRef.__VdlyCommitQueuetop__DOT__vgc_inst__DOT__tile_inst__DOT__nametable.enqueue(__VdlyVal__top__DOT__vgc_inst__DOT__tile_inst__DOT__nametable__v9, (IData)(__VdlyDim0__top__DOT__vgc_inst__DOT__tile_inst__DOT__nametable__v9));
                                                                                }
                                                                                vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT____Vlvbound_h021f5fae__0 
                                                                                = vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__col_buffer[6U];
                                                                                if (
                                                                                (0x0f9fU 
                                                                                >= 
                                                                                (0x00000fffU 
                                                                                & ((IData)(0x00f0U) 
                                                                                + 
                                                                                (((IData)(0x000003e8U) 
                                                                                * vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__tregs[8U]) 
                                                                                + vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__tregs[9U]))))) {
                                                                                __VdlyVal__top__DOT__vgc_inst__DOT__tile_inst__DOT__nametable__v10 
                                                                                = vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT____Vlvbound_h021f5fae__0;
                                                                                __VdlyDim0__top__DOT__vgc_inst__DOT__tile_inst__DOT__nametable__v10 
                                                                                = 
                                                                                (0x00000fffU 
                                                                                & ((IData)(0x00f0U) 
                                                                                + 
                                                                                (((IData)(0x000003e8U) 
                                                                                * vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__tregs[8U]) 
                                                                                + vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__tregs[9U])));
                                                                                vlSelfRef.__VdlyCommitQueuetop__DOT__vgc_inst__DOT__tile_inst__DOT__nametable.enqueue(__VdlyVal__top__DOT__vgc_inst__DOT__tile_inst__DOT__nametable__v10, (IData)(__VdlyDim0__top__DOT__vgc_inst__DOT__tile_inst__DOT__nametable__v10));
                                                                                }
                                                                                vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT____Vlvbound_h021f5fae__0 
                                                                                = vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__col_buffer[7U];
                                                                                if (
                                                                                (0x0f9fU 
                                                                                >= 
                                                                                (0x00000fffU 
                                                                                & ((IData)(0x0118U) 
                                                                                + 
                                                                                (((IData)(0x000003e8U) 
                                                                                * vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__tregs[8U]) 
                                                                                + vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__tregs[9U]))))) {
                                                                                __VdlyVal__top__DOT__vgc_inst__DOT__tile_inst__DOT__nametable__v11 
                                                                                = vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT____Vlvbound_h021f5fae__0;
                                                                                __VdlyDim0__top__DOT__vgc_inst__DOT__tile_inst__DOT__nametable__v11 
                                                                                = 
                                                                                (0x00000fffU 
                                                                                & ((IData)(0x0118U) 
                                                                                + 
                                                                                (((IData)(0x000003e8U) 
                                                                                * vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__tregs[8U]) 
                                                                                + vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__tregs[9U])));
                                                                                vlSelfRef.__VdlyCommitQueuetop__DOT__vgc_inst__DOT__tile_inst__DOT__nametable.enqueue(__VdlyVal__top__DOT__vgc_inst__DOT__tile_inst__DOT__nametable__v11, (IData)(__VdlyDim0__top__DOT__vgc_inst__DOT__tile_inst__DOT__nametable__v11));
                                                                                }
                                                                                vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT____Vlvbound_h021f5fae__0 
                                                                                = vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__col_buffer[8U];
                                                                                if (
                                                                                (0x0f9fU 
                                                                                >= 
                                                                                (0x00000fffU 
                                                                                & ((IData)(0x0140U) 
                                                                                + 
                                                                                (((IData)(0x000003e8U) 
                                                                                * vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__tregs[8U]) 
                                                                                + vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__tregs[9U]))))) {
                                                                                __VdlyVal__top__DOT__vgc_inst__DOT__tile_inst__DOT__nametable__v12 
                                                                                = vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT____Vlvbound_h021f5fae__0;
                                                                                __VdlyDim0__top__DOT__vgc_inst__DOT__tile_inst__DOT__nametable__v12 
                                                                                = 
                                                                                (0x00000fffU 
                                                                                & ((IData)(0x0140U) 
                                                                                + 
                                                                                (((IData)(0x000003e8U) 
                                                                                * vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__tregs[8U]) 
                                                                                + vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__tregs[9U])));
                                                                                vlSelfRef.__VdlyCommitQueuetop__DOT__vgc_inst__DOT__tile_inst__DOT__nametable.enqueue(__VdlyVal__top__DOT__vgc_inst__DOT__tile_inst__DOT__nametable__v12, (IData)(__VdlyDim0__top__DOT__vgc_inst__DOT__tile_inst__DOT__nametable__v12));
                                                                                }
                                                                                vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT____Vlvbound_h021f5fae__0 
                                                                                = vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__col_buffer[9U];
                                                                                if (
                                                                                (0x0f9fU 
                                                                                >= 
                                                                                (0x00000fffU 
                                                                                & ((IData)(0x0168U) 
                                                                                + 
                                                                                (((IData)(0x000003e8U) 
                                                                                * vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__tregs[8U]) 
                                                                                + vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__tregs[9U]))))) {
                                                                                __VdlyVal__top__DOT__vgc_inst__DOT__tile_inst__DOT__nametable__v13 
                                                                                = vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT____Vlvbound_h021f5fae__0;
                                                                                __VdlyDim0__top__DOT__vgc_inst__DOT__tile_inst__DOT__nametable__v13 
                                                                                = 
                                                                                (0x00000fffU 
                                                                                & ((IData)(0x0168U) 
                                                                                + 
                                                                                (((IData)(0x000003e8U) 
                                                                                * vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__tregs[8U]) 
                                                                                + vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__tregs[9U])));
                                                                                vlSelfRef.__VdlyCommitQueuetop__DOT__vgc_inst__DOT__tile_inst__DOT__nametable.enqueue(__VdlyVal__top__DOT__vgc_inst__DOT__tile_inst__DOT__nametable__v13, (IData)(__VdlyDim0__top__DOT__vgc_inst__DOT__tile_inst__DOT__nametable__v13));
                                                                                }
                                                                                vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT____Vlvbound_h021f5fae__0 
                                                                                = vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__col_buffer[10U];
                                                                                if (
                                                                                (0x0f9fU 
                                                                                >= 
                                                                                (0x00000fffU 
                                                                                & ((IData)(0x0190U) 
                                                                                + 
                                                                                (((IData)(0x000003e8U) 
                                                                                * vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__tregs[8U]) 
                                                                                + vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__tregs[9U]))))) {
                                                                                __VdlyVal__top__DOT__vgc_inst__DOT__tile_inst__DOT__nametable__v14 
                                                                                = vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT____Vlvbound_h021f5fae__0;
                                                                                __VdlyDim0__top__DOT__vgc_inst__DOT__tile_inst__DOT__nametable__v14 
                                                                                = 
                                                                                (0x00000fffU 
                                                                                & ((IData)(0x0190U) 
                                                                                + 
                                                                                (((IData)(0x000003e8U) 
                                                                                * vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__tregs[8U]) 
                                                                                + vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__tregs[9U])));
                                                                                vlSelfRef.__VdlyCommitQueuetop__DOT__vgc_inst__DOT__tile_inst__DOT__nametable.enqueue(__VdlyVal__top__DOT__vgc_inst__DOT__tile_inst__DOT__nametable__v14, (IData)(__VdlyDim0__top__DOT__vgc_inst__DOT__tile_inst__DOT__nametable__v14));
                                                                                }
                                                                                vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT____Vlvbound_h021f5fae__0 
                                                                                = vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__col_buffer[11U];
                                                                                if (
                                                                                (0x0f9fU 
                                                                                >= 
                                                                                (0x00000fffU 
                                                                                & ((IData)(0x01b8U) 
                                                                                + 
                                                                                (((IData)(0x000003e8U) 
                                                                                * vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__tregs[8U]) 
                                                                                + vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__tregs[9U]))))) {
                                                                                __VdlyVal__top__DOT__vgc_inst__DOT__tile_inst__DOT__nametable__v15 
                                                                                = vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT____Vlvbound_h021f5fae__0;
                                                                                __VdlyDim0__top__DOT__vgc_inst__DOT__tile_inst__DOT__nametable__v15 
                                                                                = 
                                                                                (0x00000fffU 
                                                                                & ((IData)(0x01b8U) 
                                                                                + 
                                                                                (((IData)(0x000003e8U) 
                                                                                * vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__tregs[8U]) 
                                                                                + vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__tregs[9U])));
                                                                                vlSelfRef.__VdlyCommitQueuetop__DOT__vgc_inst__DOT__tile_inst__DOT__nametable.enqueue(__VdlyVal__top__DOT__vgc_inst__DOT__tile_inst__DOT__nametable__v15, (IData)(__VdlyDim0__top__DOT__vgc_inst__DOT__tile_inst__DOT__nametable__v15));
                                                                                }
                                                                                vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT____Vlvbound_h021f5fae__0 
                                                                                = vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__col_buffer[12U];
                                                                                if (
                                                                                (0x0f9fU 
                                                                                >= 
                                                                                (0x00000fffU 
                                                                                & ((IData)(0x01e0U) 
                                                                                + 
                                                                                (((IData)(0x000003e8U) 
                                                                                * vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__tregs[8U]) 
                                                                                + vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__tregs[9U]))))) {
                                                                                __VdlyVal__top__DOT__vgc_inst__DOT__tile_inst__DOT__nametable__v16 
                                                                                = vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT____Vlvbound_h021f5fae__0;
                                                                                __VdlyDim0__top__DOT__vgc_inst__DOT__tile_inst__DOT__nametable__v16 
                                                                                = 
                                                                                (0x00000fffU 
                                                                                & ((IData)(0x01e0U) 
                                                                                + 
                                                                                (((IData)(0x000003e8U) 
                                                                                * vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__tregs[8U]) 
                                                                                + vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__tregs[9U])));
                                                                                vlSelfRef.__VdlyCommitQueuetop__DOT__vgc_inst__DOT__tile_inst__DOT__nametable.enqueue(__VdlyVal__top__DOT__vgc_inst__DOT__tile_inst__DOT__nametable__v16, (IData)(__VdlyDim0__top__DOT__vgc_inst__DOT__tile_inst__DOT__nametable__v16));
                                                                                }
                                                                                vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT____Vlvbound_h021f5fae__0 
                                                                                = vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__col_buffer[13U];
                                                                                if (
                                                                                (0x0f9fU 
                                                                                >= 
                                                                                (0x00000fffU 
                                                                                & ((IData)(0x0208U) 
                                                                                + 
                                                                                (((IData)(0x000003e8U) 
                                                                                * vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__tregs[8U]) 
                                                                                + vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__tregs[9U]))))) {
                                                                                __VdlyVal__top__DOT__vgc_inst__DOT__tile_inst__DOT__nametable__v17 
                                                                                = vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT____Vlvbound_h021f5fae__0;
                                                                                __VdlyDim0__top__DOT__vgc_inst__DOT__tile_inst__DOT__nametable__v17 
                                                                                = 
                                                                                (0x00000fffU 
                                                                                & ((IData)(0x0208U) 
                                                                                + 
                                                                                (((IData)(0x000003e8U) 
                                                                                * vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__tregs[8U]) 
                                                                                + vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__tregs[9U])));
                                                                                vlSelfRef.__VdlyCommitQueuetop__DOT__vgc_inst__DOT__tile_inst__DOT__nametable.enqueue(__VdlyVal__top__DOT__vgc_inst__DOT__tile_inst__DOT__nametable__v17, (IData)(__VdlyDim0__top__DOT__vgc_inst__DOT__tile_inst__DOT__nametable__v17));
                                                                                }
                                                                                vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT____Vlvbound_h021f5fae__0 
                                                                                = vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__col_buffer[14U];
                                                                                if (
                                                                                (0x0f9fU 
                                                                                >= 
                                                                                (0x00000fffU 
                                                                                & ((IData)(0x0230U) 
                                                                                + 
                                                                                (((IData)(0x000003e8U) 
                                                                                * vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__tregs[8U]) 
                                                                                + vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__tregs[9U]))))) {
                                                                                __VdlyVal__top__DOT__vgc_inst__DOT__tile_inst__DOT__nametable__v18 
                                                                                = vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT____Vlvbound_h021f5fae__0;
                                                                                __VdlyDim0__top__DOT__vgc_inst__DOT__tile_inst__DOT__nametable__v18 
                                                                                = 
                                                                                (0x00000fffU 
                                                                                & ((IData)(0x0230U) 
                                                                                + 
                                                                                (((IData)(0x000003e8U) 
                                                                                * vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__tregs[8U]) 
                                                                                + vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__tregs[9U])));
                                                                                vlSelfRef.__VdlyCommitQueuetop__DOT__vgc_inst__DOT__tile_inst__DOT__nametable.enqueue(__VdlyVal__top__DOT__vgc_inst__DOT__tile_inst__DOT__nametable__v18, (IData)(__VdlyDim0__top__DOT__vgc_inst__DOT__tile_inst__DOT__nametable__v18));
                                                                                }
                                                                                vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT____Vlvbound_h021f5fae__0 
                                                                                = vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__col_buffer[15U];
                                                                                if (
                                                                                (0x0f9fU 
                                                                                >= 
                                                                                (0x00000fffU 
                                                                                & ((IData)(0x0258U) 
                                                                                + 
                                                                                (((IData)(0x000003e8U) 
                                                                                * vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__tregs[8U]) 
                                                                                + vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__tregs[9U]))))) {
                                                                                __VdlyVal__top__DOT__vgc_inst__DOT__tile_inst__DOT__nametable__v19 
                                                                                = vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT____Vlvbound_h021f5fae__0;
                                                                                __VdlyDim0__top__DOT__vgc_inst__DOT__tile_inst__DOT__nametable__v19 
                                                                                = 
                                                                                (0x00000fffU 
                                                                                & ((IData)(0x0258U) 
                                                                                + 
                                                                                (((IData)(0x000003e8U) 
                                                                                * vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__tregs[8U]) 
                                                                                + vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__tregs[9U])));
                                                                                vlSelfRef.__VdlyCommitQueuetop__DOT__vgc_inst__DOT__tile_inst__DOT__nametable.enqueue(__VdlyVal__top__DOT__vgc_inst__DOT__tile_inst__DOT__nametable__v19, (IData)(__VdlyDim0__top__DOT__vgc_inst__DOT__tile_inst__DOT__nametable__v19));
                                                                                }
                                                                                vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT____Vlvbound_h021f5fae__0 
                                                                                = vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__col_buffer[16U];
                                                                                if (
                                                                                (0x0f9fU 
                                                                                >= 
                                                                                (0x00000fffU 
                                                                                & ((IData)(0x0280U) 
                                                                                + 
                                                                                (((IData)(0x000003e8U) 
                                                                                * vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__tregs[8U]) 
                                                                                + vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__tregs[9U]))))) {
                                                                                __VdlyVal__top__DOT__vgc_inst__DOT__tile_inst__DOT__nametable__v20 
                                                                                = vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT____Vlvbound_h021f5fae__0;
                                                                                __VdlyDim0__top__DOT__vgc_inst__DOT__tile_inst__DOT__nametable__v20 
                                                                                = 
                                                                                (0x00000fffU 
                                                                                & ((IData)(0x0280U) 
                                                                                + 
                                                                                (((IData)(0x000003e8U) 
                                                                                * vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__tregs[8U]) 
                                                                                + vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__tregs[9U])));
                                                                                vlSelfRef.__VdlyCommitQueuetop__DOT__vgc_inst__DOT__tile_inst__DOT__nametable.enqueue(__VdlyVal__top__DOT__vgc_inst__DOT__tile_inst__DOT__nametable__v20, (IData)(__VdlyDim0__top__DOT__vgc_inst__DOT__tile_inst__DOT__nametable__v20));
                                                                                }
                                                                                vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT____Vlvbound_h021f5fae__0 
                                                                                = vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__col_buffer[17U];
                                                                                if (
                                                                                (0x0f9fU 
                                                                                >= 
                                                                                (0x00000fffU 
                                                                                & ((IData)(0x02a8U) 
                                                                                + 
                                                                                (((IData)(0x000003e8U) 
                                                                                * vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__tregs[8U]) 
                                                                                + vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__tregs[9U]))))) {
                                                                                __VdlyVal__top__DOT__vgc_inst__DOT__tile_inst__DOT__nametable__v21 
                                                                                = vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT____Vlvbound_h021f5fae__0;
                                                                                __VdlyDim0__top__DOT__vgc_inst__DOT__tile_inst__DOT__nametable__v21 
                                                                                = 
                                                                                (0x00000fffU 
                                                                                & ((IData)(0x02a8U) 
                                                                                + 
                                                                                (((IData)(0x000003e8U) 
                                                                                * vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__tregs[8U]) 
                                                                                + vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__tregs[9U])));
                                                                                vlSelfRef.__VdlyCommitQueuetop__DOT__vgc_inst__DOT__tile_inst__DOT__nametable.enqueue(__VdlyVal__top__DOT__vgc_inst__DOT__tile_inst__DOT__nametable__v21, (IData)(__VdlyDim0__top__DOT__vgc_inst__DOT__tile_inst__DOT__nametable__v21));
                                                                                }
                                                                                vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT____Vlvbound_h021f5fae__0 
                                                                                = vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__col_buffer[18U];
                                                                                if (
                                                                                (0x0f9fU 
                                                                                >= 
                                                                                (0x00000fffU 
                                                                                & ((IData)(0x02d0U) 
                                                                                + 
                                                                                (((IData)(0x000003e8U) 
                                                                                * vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__tregs[8U]) 
                                                                                + vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__tregs[9U]))))) {
                                                                                __VdlyVal__top__DOT__vgc_inst__DOT__tile_inst__DOT__nametable__v22 
                                                                                = vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT____Vlvbound_h021f5fae__0;
                                                                                __VdlyDim0__top__DOT__vgc_inst__DOT__tile_inst__DOT__nametable__v22 
                                                                                = 
                                                                                (0x00000fffU 
                                                                                & ((IData)(0x02d0U) 
                                                                                + 
                                                                                (((IData)(0x000003e8U) 
                                                                                * vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__tregs[8U]) 
                                                                                + vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__tregs[9U])));
                                                                                vlSelfRef.__VdlyCommitQueuetop__DOT__vgc_inst__DOT__tile_inst__DOT__nametable.enqueue(__VdlyVal__top__DOT__vgc_inst__DOT__tile_inst__DOT__nametable__v22, (IData)(__VdlyDim0__top__DOT__vgc_inst__DOT__tile_inst__DOT__nametable__v22));
                                                                                }
                                                                                vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT____Vlvbound_h021f5fae__0 
                                                                                = vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__col_buffer[19U];
                                                                                if (
                                                                                (0x0f9fU 
                                                                                >= 
                                                                                (0x00000fffU 
                                                                                & ((IData)(0x02f8U) 
                                                                                + 
                                                                                (((IData)(0x000003e8U) 
                                                                                * vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__tregs[8U]) 
                                                                                + vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__tregs[9U]))))) {
                                                                                __VdlyVal__top__DOT__vgc_inst__DOT__tile_inst__DOT__nametable__v23 
                                                                                = vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT____Vlvbound_h021f5fae__0;
                                                                                __VdlyDim0__top__DOT__vgc_inst__DOT__tile_inst__DOT__nametable__v23 
                                                                                = 
                                                                                (0x00000fffU 
                                                                                & ((IData)(0x02f8U) 
                                                                                + 
                                                                                (((IData)(0x000003e8U) 
                                                                                * vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__tregs[8U]) 
                                                                                + vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__tregs[9U])));
                                                                                vlSelfRef.__VdlyCommitQueuetop__DOT__vgc_inst__DOT__tile_inst__DOT__nametable.enqueue(__VdlyVal__top__DOT__vgc_inst__DOT__tile_inst__DOT__nametable__v23, (IData)(__VdlyDim0__top__DOT__vgc_inst__DOT__tile_inst__DOT__nametable__v23));
                                                                                }
                                                                                vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT____Vlvbound_h021f5fae__0 
                                                                                = vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__col_buffer[20U];
                                                                                if (
                                                                                (0x0f9fU 
                                                                                >= 
                                                                                (0x00000fffU 
                                                                                & ((IData)(0x0320U) 
                                                                                + 
                                                                                (((IData)(0x000003e8U) 
                                                                                * vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__tregs[8U]) 
                                                                                + vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__tregs[9U]))))) {
                                                                                __VdlyVal__top__DOT__vgc_inst__DOT__tile_inst__DOT__nametable__v24 
                                                                                = vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT____Vlvbound_h021f5fae__0;
                                                                                __VdlyDim0__top__DOT__vgc_inst__DOT__tile_inst__DOT__nametable__v24 
                                                                                = 
                                                                                (0x00000fffU 
                                                                                & ((IData)(0x0320U) 
                                                                                + 
                                                                                (((IData)(0x000003e8U) 
                                                                                * vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__tregs[8U]) 
                                                                                + vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__tregs[9U])));
                                                                                vlSelfRef.__VdlyCommitQueuetop__DOT__vgc_inst__DOT__tile_inst__DOT__nametable.enqueue(__VdlyVal__top__DOT__vgc_inst__DOT__tile_inst__DOT__nametable__v24, (IData)(__VdlyDim0__top__DOT__vgc_inst__DOT__tile_inst__DOT__nametable__v24));
                                                                                }
                                                                                vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT____Vlvbound_h021f5fae__0 
                                                                                = vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__col_buffer[21U];
                                                                                if (
                                                                                (0x0f9fU 
                                                                                >= 
                                                                                (0x00000fffU 
                                                                                & ((IData)(0x0348U) 
                                                                                + 
                                                                                (((IData)(0x000003e8U) 
                                                                                * vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__tregs[8U]) 
                                                                                + vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__tregs[9U]))))) {
                                                                                __VdlyVal__top__DOT__vgc_inst__DOT__tile_inst__DOT__nametable__v25 
                                                                                = vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT____Vlvbound_h021f5fae__0;
                                                                                __VdlyDim0__top__DOT__vgc_inst__DOT__tile_inst__DOT__nametable__v25 
                                                                                = 
                                                                                (0x00000fffU 
                                                                                & ((IData)(0x0348U) 
                                                                                + 
                                                                                (((IData)(0x000003e8U) 
                                                                                * vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__tregs[8U]) 
                                                                                + vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__tregs[9U])));
                                                                                vlSelfRef.__VdlyCommitQueuetop__DOT__vgc_inst__DOT__tile_inst__DOT__nametable.enqueue(__VdlyVal__top__DOT__vgc_inst__DOT__tile_inst__DOT__nametable__v25, (IData)(__VdlyDim0__top__DOT__vgc_inst__DOT__tile_inst__DOT__nametable__v25));
                                                                                }
                                                                                vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT____Vlvbound_h021f5fae__0 
                                                                                = vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__col_buffer[22U];
                                                                                if (
                                                                                (0x0f9fU 
                                                                                >= 
                                                                                (0x00000fffU 
                                                                                & ((IData)(0x0370U) 
                                                                                + 
                                                                                (((IData)(0x000003e8U) 
                                                                                * vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__tregs[8U]) 
                                                                                + vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__tregs[9U]))))) {
                                                                                __VdlyVal__top__DOT__vgc_inst__DOT__tile_inst__DOT__nametable__v26 
                                                                                = vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT____Vlvbound_h021f5fae__0;
                                                                                __VdlyDim0__top__DOT__vgc_inst__DOT__tile_inst__DOT__nametable__v26 
                                                                                = 
                                                                                (0x00000fffU 
                                                                                & ((IData)(0x0370U) 
                                                                                + 
                                                                                (((IData)(0x000003e8U) 
                                                                                * vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__tregs[8U]) 
                                                                                + vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__tregs[9U])));
                                                                                vlSelfRef.__VdlyCommitQueuetop__DOT__vgc_inst__DOT__tile_inst__DOT__nametable.enqueue(__VdlyVal__top__DOT__vgc_inst__DOT__tile_inst__DOT__nametable__v26, (IData)(__VdlyDim0__top__DOT__vgc_inst__DOT__tile_inst__DOT__nametable__v26));
                                                                                }
                                                                                vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT____Vlvbound_h021f5fae__0 
                                                                                = vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__col_buffer[23U];
                                                                                if (
                                                                                (0x0f9fU 
                                                                                >= 
                                                                                (0x00000fffU 
                                                                                & ((IData)(0x0398U) 
                                                                                + 
                                                                                (((IData)(0x000003e8U) 
                                                                                * vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__tregs[8U]) 
                                                                                + vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__tregs[9U]))))) {
                                                                                __VdlyVal__top__DOT__vgc_inst__DOT__tile_inst__DOT__nametable__v27 
                                                                                = vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT____Vlvbound_h021f5fae__0;
                                                                                __VdlyDim0__top__DOT__vgc_inst__DOT__tile_inst__DOT__nametable__v27 
                                                                                = 
                                                                                (0x00000fffU 
                                                                                & ((IData)(0x0398U) 
                                                                                + 
                                                                                (((IData)(0x000003e8U) 
                                                                                * vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__tregs[8U]) 
                                                                                + vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__tregs[9U])));
                                                                                vlSelfRef.__VdlyCommitQueuetop__DOT__vgc_inst__DOT__tile_inst__DOT__nametable.enqueue(__VdlyVal__top__DOT__vgc_inst__DOT__tile_inst__DOT__nametable__v27, (IData)(__VdlyDim0__top__DOT__vgc_inst__DOT__tile_inst__DOT__nametable__v27));
                                                                                }
                                                                                vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT____Vlvbound_h021f5fae__0 
                                                                                = vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__col_buffer[24U];
                                                                                if (
                                                                                (0x0f9fU 
                                                                                >= 
                                                                                (0x00000fffU 
                                                                                & ((IData)(0x03c0U) 
                                                                                + 
                                                                                (((IData)(0x000003e8U) 
                                                                                * vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__tregs[8U]) 
                                                                                + vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__tregs[9U]))))) {
                                                                                __VdlyVal__top__DOT__vgc_inst__DOT__tile_inst__DOT__nametable__v28 
                                                                                = vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT____Vlvbound_h021f5fae__0;
                                                                                __VdlyDim0__top__DOT__vgc_inst__DOT__tile_inst__DOT__nametable__v28 
                                                                                = 
                                                                                (0x00000fffU 
                                                                                & ((IData)(0x03c0U) 
                                                                                + 
                                                                                (((IData)(0x000003e8U) 
                                                                                * vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__tregs[8U]) 
                                                                                + vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__tregs[9U])));
                                                                                vlSelfRef.__VdlyCommitQueuetop__DOT__vgc_inst__DOT__tile_inst__DOT__nametable.enqueue(__VdlyVal__top__DOT__vgc_inst__DOT__tile_inst__DOT__nametable__v28, (IData)(__VdlyDim0__top__DOT__vgc_inst__DOT__tile_inst__DOT__nametable__v28));
                                                                                }
                                                                            }
                                                                        } else {
                                                                            if (
                                                                                (0U 
                                                                                >= vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__tregs[8U])) {
                                                                                vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT____Vlvbound_h02fc01d7__0 
                                                                                = vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__tregs[10U];
                                                                                __VdlyVal__top__DOT__vgc_inst__DOT__tile_inst__DOT__col_buffer__v0 
                                                                                = vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT____Vlvbound_h02fc01d7__0;
                                                                                __VdlySet__top__DOT__vgc_inst__DOT__tile_inst__DOT__col_buffer__v0 = 1U;
                                                                            }
                                                                            if (
                                                                                ((1U 
                                                                                >= vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__tregs[8U]) 
                                                                                & (1U 
                                                                                <= vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__tregs[9U]))) {
                                                                                vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT____Vlvbound_h02fc01d7__0 
                                                                                = vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__tregs[10U];
                                                                                __VdlyVal__top__DOT__vgc_inst__DOT__tile_inst__DOT__col_buffer__v1 
                                                                                = vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT____Vlvbound_h02fc01d7__0;
                                                                                __VdlySet__top__DOT__vgc_inst__DOT__tile_inst__DOT__col_buffer__v1 = 1U;
                                                                            }
                                                                            if (
                                                                                ((2U 
                                                                                >= vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__tregs[8U]) 
                                                                                & (2U 
                                                                                <= vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__tregs[9U]))) {
                                                                                vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT____Vlvbound_h02fc01d7__0 
                                                                                = vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__tregs[10U];
                                                                                __VdlyVal__top__DOT__vgc_inst__DOT__tile_inst__DOT__col_buffer__v2 
                                                                                = vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT____Vlvbound_h02fc01d7__0;
                                                                                __VdlySet__top__DOT__vgc_inst__DOT__tile_inst__DOT__col_buffer__v2 = 1U;
                                                                            }
                                                                            if (
                                                                                ((3U 
                                                                                >= vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__tregs[8U]) 
                                                                                & (3U 
                                                                                <= vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__tregs[9U]))) {
                                                                                vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT____Vlvbound_h02fc01d7__0 
                                                                                = vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__tregs[10U];
                                                                                __VdlyVal__top__DOT__vgc_inst__DOT__tile_inst__DOT__col_buffer__v3 
                                                                                = vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT____Vlvbound_h02fc01d7__0;
                                                                                __VdlySet__top__DOT__vgc_inst__DOT__tile_inst__DOT__col_buffer__v3 = 1U;
                                                                            }
                                                                            if (
                                                                                ((4U 
                                                                                >= vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__tregs[8U]) 
                                                                                & (4U 
                                                                                <= vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__tregs[9U]))) {
                                                                                vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT____Vlvbound_h02fc01d7__0 
                                                                                = vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__tregs[10U];
                                                                                __VdlyVal__top__DOT__vgc_inst__DOT__tile_inst__DOT__col_buffer__v4 
                                                                                = vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT____Vlvbound_h02fc01d7__0;
                                                                                __VdlySet__top__DOT__vgc_inst__DOT__tile_inst__DOT__col_buffer__v4 = 1U;
                                                                            }
                                                                            if (
                                                                                ((5U 
                                                                                >= vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__tregs[8U]) 
                                                                                & (5U 
                                                                                <= vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__tregs[9U]))) {
                                                                                vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT____Vlvbound_h02fc01d7__0 
                                                                                = vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__tregs[10U];
                                                                                __VdlyVal__top__DOT__vgc_inst__DOT__tile_inst__DOT__col_buffer__v5 
                                                                                = vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT____Vlvbound_h02fc01d7__0;
                                                                                __VdlySet__top__DOT__vgc_inst__DOT__tile_inst__DOT__col_buffer__v5 = 1U;
                                                                            }
                                                                            if (
                                                                                ((6U 
                                                                                >= vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__tregs[8U]) 
                                                                                & (6U 
                                                                                <= vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__tregs[9U]))) {
                                                                                vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT____Vlvbound_h02fc01d7__0 
                                                                                = vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__tregs[10U];
                                                                                __VdlyVal__top__DOT__vgc_inst__DOT__tile_inst__DOT__col_buffer__v6 
                                                                                = vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT____Vlvbound_h02fc01d7__0;
                                                                                __VdlySet__top__DOT__vgc_inst__DOT__tile_inst__DOT__col_buffer__v6 = 1U;
                                                                            }
                                                                            if (
                                                                                ((7U 
                                                                                >= vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__tregs[8U]) 
                                                                                & (7U 
                                                                                <= vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__tregs[9U]))) {
                                                                                vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT____Vlvbound_h02fc01d7__0 
                                                                                = vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__tregs[10U];
                                                                                __VdlyVal__top__DOT__vgc_inst__DOT__tile_inst__DOT__col_buffer__v7 
                                                                                = vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT____Vlvbound_h02fc01d7__0;
                                                                                __VdlySet__top__DOT__vgc_inst__DOT__tile_inst__DOT__col_buffer__v7 = 1U;
                                                                            }
                                                                            if (
                                                                                ((8U 
                                                                                >= vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__tregs[8U]) 
                                                                                & (8U 
                                                                                <= vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__tregs[9U]))) {
                                                                                vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT____Vlvbound_h02fc01d7__0 
                                                                                = vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__tregs[10U];
                                                                                __VdlyVal__top__DOT__vgc_inst__DOT__tile_inst__DOT__col_buffer__v8 
                                                                                = vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT____Vlvbound_h02fc01d7__0;
                                                                                __VdlySet__top__DOT__vgc_inst__DOT__tile_inst__DOT__col_buffer__v8 = 1U;
                                                                            }
                                                                            if (
                                                                                ((9U 
                                                                                >= vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__tregs[8U]) 
                                                                                & (9U 
                                                                                <= vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__tregs[9U]))) {
                                                                                vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT____Vlvbound_h02fc01d7__0 
                                                                                = vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__tregs[10U];
                                                                                __VdlyVal__top__DOT__vgc_inst__DOT__tile_inst__DOT__col_buffer__v9 
                                                                                = vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT____Vlvbound_h02fc01d7__0;
                                                                                __VdlySet__top__DOT__vgc_inst__DOT__tile_inst__DOT__col_buffer__v9 = 1U;
                                                                            }
                                                                            if (
                                                                                ((0x0aU 
                                                                                >= vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__tregs[8U]) 
                                                                                & (0x0aU 
                                                                                <= vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__tregs[9U]))) {
                                                                                vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT____Vlvbound_h02fc01d7__0 
                                                                                = vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__tregs[10U];
                                                                                __VdlyVal__top__DOT__vgc_inst__DOT__tile_inst__DOT__col_buffer__v10 
                                                                                = vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT____Vlvbound_h02fc01d7__0;
                                                                                __VdlySet__top__DOT__vgc_inst__DOT__tile_inst__DOT__col_buffer__v10 = 1U;
                                                                            }
                                                                            if (
                                                                                ((0x0bU 
                                                                                >= vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__tregs[8U]) 
                                                                                & (0x0bU 
                                                                                <= vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__tregs[9U]))) {
                                                                                vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT____Vlvbound_h02fc01d7__0 
                                                                                = vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__tregs[10U];
                                                                                __VdlyVal__top__DOT__vgc_inst__DOT__tile_inst__DOT__col_buffer__v11 
                                                                                = vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT____Vlvbound_h02fc01d7__0;
                                                                                __VdlySet__top__DOT__vgc_inst__DOT__tile_inst__DOT__col_buffer__v11 = 1U;
                                                                            }
                                                                            if (
                                                                                ((0x0cU 
                                                                                >= vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__tregs[8U]) 
                                                                                & (0x0cU 
                                                                                <= vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__tregs[9U]))) {
                                                                                vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT____Vlvbound_h02fc01d7__0 
                                                                                = vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__tregs[10U];
                                                                                __VdlyVal__top__DOT__vgc_inst__DOT__tile_inst__DOT__col_buffer__v12 
                                                                                = vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT____Vlvbound_h02fc01d7__0;
                                                                                __VdlySet__top__DOT__vgc_inst__DOT__tile_inst__DOT__col_buffer__v12 = 1U;
                                                                            }
                                                                            if (
                                                                                ((0x0dU 
                                                                                >= vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__tregs[8U]) 
                                                                                & (0x0dU 
                                                                                <= vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__tregs[9U]))) {
                                                                                vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT____Vlvbound_h02fc01d7__0 
                                                                                = vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__tregs[10U];
                                                                                __VdlyVal__top__DOT__vgc_inst__DOT__tile_inst__DOT__col_buffer__v13 
                                                                                = vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT____Vlvbound_h02fc01d7__0;
                                                                                __VdlySet__top__DOT__vgc_inst__DOT__tile_inst__DOT__col_buffer__v13 = 1U;
                                                                            }
                                                                            if (
                                                                                ((0x0eU 
                                                                                >= vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__tregs[8U]) 
                                                                                & (0x0eU 
                                                                                <= vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__tregs[9U]))) {
                                                                                vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT____Vlvbound_h02fc01d7__0 
                                                                                = vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__tregs[10U];
                                                                                __VdlyVal__top__DOT__vgc_inst__DOT__tile_inst__DOT__col_buffer__v14 
                                                                                = vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT____Vlvbound_h02fc01d7__0;
                                                                                __VdlySet__top__DOT__vgc_inst__DOT__tile_inst__DOT__col_buffer__v14 = 1U;
                                                                            }
                                                                            if (
                                                                                ((0x0fU 
                                                                                >= vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__tregs[8U]) 
                                                                                & (0x0fU 
                                                                                <= vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__tregs[9U]))) {
                                                                                vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT____Vlvbound_h02fc01d7__0 
                                                                                = vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__tregs[10U];
                                                                                __VdlyVal__top__DOT__vgc_inst__DOT__tile_inst__DOT__col_buffer__v15 
                                                                                = vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT____Vlvbound_h02fc01d7__0;
                                                                                __VdlySet__top__DOT__vgc_inst__DOT__tile_inst__DOT__col_buffer__v15 = 1U;
                                                                            }
                                                                            if (
                                                                                ((0x10U 
                                                                                >= vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__tregs[8U]) 
                                                                                & (0x10U 
                                                                                <= vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__tregs[9U]))) {
                                                                                vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT____Vlvbound_h02fc01d7__0 
                                                                                = vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__tregs[10U];
                                                                                __VdlyVal__top__DOT__vgc_inst__DOT__tile_inst__DOT__col_buffer__v16 
                                                                                = vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT____Vlvbound_h02fc01d7__0;
                                                                                __VdlySet__top__DOT__vgc_inst__DOT__tile_inst__DOT__col_buffer__v16 = 1U;
                                                                            }
                                                                            if (
                                                                                ((0x11U 
                                                                                >= vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__tregs[8U]) 
                                                                                & (0x11U 
                                                                                <= vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__tregs[9U]))) {
                                                                                vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT____Vlvbound_h02fc01d7__0 
                                                                                = vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__tregs[10U];
                                                                                __VdlyVal__top__DOT__vgc_inst__DOT__tile_inst__DOT__col_buffer__v17 
                                                                                = vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT____Vlvbound_h02fc01d7__0;
                                                                                __VdlySet__top__DOT__vgc_inst__DOT__tile_inst__DOT__col_buffer__v17 = 1U;
                                                                            }
                                                                            if (
                                                                                ((0x12U 
                                                                                >= vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__tregs[8U]) 
                                                                                & (0x12U 
                                                                                <= vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__tregs[9U]))) {
                                                                                vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT____Vlvbound_h02fc01d7__0 
                                                                                = vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__tregs[10U];
                                                                                __VdlyVal__top__DOT__vgc_inst__DOT__tile_inst__DOT__col_buffer__v18 
                                                                                = vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT____Vlvbound_h02fc01d7__0;
                                                                                __VdlySet__top__DOT__vgc_inst__DOT__tile_inst__DOT__col_buffer__v18 = 1U;
                                                                            }
                                                                            if (
                                                                                ((0x13U 
                                                                                >= vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__tregs[8U]) 
                                                                                & (0x13U 
                                                                                <= vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__tregs[9U]))) {
                                                                                vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT____Vlvbound_h02fc01d7__0 
                                                                                = vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__tregs[10U];
                                                                                __VdlyVal__top__DOT__vgc_inst__DOT__tile_inst__DOT__col_buffer__v19 
                                                                                = vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT____Vlvbound_h02fc01d7__0;
                                                                                __VdlySet__top__DOT__vgc_inst__DOT__tile_inst__DOT__col_buffer__v19 = 1U;
                                                                            }
                                                                            if (
                                                                                ((0x14U 
                                                                                >= vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__tregs[8U]) 
                                                                                & (0x14U 
                                                                                <= vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__tregs[9U]))) {
                                                                                vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT____Vlvbound_h02fc01d7__0 
                                                                                = vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__tregs[10U];
                                                                                __VdlyVal__top__DOT__vgc_inst__DOT__tile_inst__DOT__col_buffer__v20 
                                                                                = vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT____Vlvbound_h02fc01d7__0;
                                                                                __VdlySet__top__DOT__vgc_inst__DOT__tile_inst__DOT__col_buffer__v20 = 1U;
                                                                            }
                                                                            if (
                                                                                ((0x15U 
                                                                                >= vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__tregs[8U]) 
                                                                                & (0x15U 
                                                                                <= vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__tregs[9U]))) {
                                                                                vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT____Vlvbound_h02fc01d7__0 
                                                                                = vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__tregs[10U];
                                                                                __VdlyVal__top__DOT__vgc_inst__DOT__tile_inst__DOT__col_buffer__v21 
                                                                                = vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT____Vlvbound_h02fc01d7__0;
                                                                                __VdlySet__top__DOT__vgc_inst__DOT__tile_inst__DOT__col_buffer__v21 = 1U;
                                                                            }
                                                                            if (
                                                                                ((0x16U 
                                                                                >= vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__tregs[8U]) 
                                                                                & (0x16U 
                                                                                <= vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__tregs[9U]))) {
                                                                                vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT____Vlvbound_h02fc01d7__0 
                                                                                = vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__tregs[10U];
                                                                                __VdlyVal__top__DOT__vgc_inst__DOT__tile_inst__DOT__col_buffer__v22 
                                                                                = vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT____Vlvbound_h02fc01d7__0;
                                                                                __VdlySet__top__DOT__vgc_inst__DOT__tile_inst__DOT__col_buffer__v22 = 1U;
                                                                            }
                                                                            if (
                                                                                ((0x17U 
                                                                                >= vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__tregs[8U]) 
                                                                                & (0x17U 
                                                                                <= vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__tregs[9U]))) {
                                                                                vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT____Vlvbound_h02fc01d7__0 
                                                                                = vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__tregs[10U];
                                                                                __VdlyVal__top__DOT__vgc_inst__DOT__tile_inst__DOT__col_buffer__v23 
                                                                                = vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT____Vlvbound_h02fc01d7__0;
                                                                                __VdlySet__top__DOT__vgc_inst__DOT__tile_inst__DOT__col_buffer__v23 = 1U;
                                                                            }
                                                                            if (
                                                                                ((0x18U 
                                                                                >= vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__tregs[8U]) 
                                                                                & (0x18U 
                                                                                <= vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__tregs[9U]))) {
                                                                                vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT____Vlvbound_h02fc01d7__0 
                                                                                = vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__tregs[10U];
                                                                                __VdlyVal__top__DOT__vgc_inst__DOT__tile_inst__DOT__col_buffer__v24 
                                                                                = vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT____Vlvbound_h02fc01d7__0;
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
                                                                        vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT____Vlvbound_ha057723b__0 
                                                                            = vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__tregs[8U];
                                                                        __VdlyVal__top__DOT__vgc_inst__DOT__tile_inst__DOT__col_buffer__v26 
                                                                            = vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT____Vlvbound_ha057723b__0;
                                                                        __VdlySet__top__DOT__vgc_inst__DOT__tile_inst__DOT__col_buffer__v26 = 1U;
                                                                        vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT____Vlvbound_ha057723b__0 
                                                                            = vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__tregs[8U];
                                                                        __VdlyVal__top__DOT__vgc_inst__DOT__tile_inst__DOT__col_buffer__v27 
                                                                            = vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT____Vlvbound_ha057723b__0;
                                                                        __VdlySet__top__DOT__vgc_inst__DOT__tile_inst__DOT__col_buffer__v27 = 1U;
                                                                        vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT____Vlvbound_ha057723b__0 
                                                                            = vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__tregs[8U];
                                                                        __VdlyVal__top__DOT__vgc_inst__DOT__tile_inst__DOT__col_buffer__v28 
                                                                            = vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT____Vlvbound_ha057723b__0;
                                                                        __VdlySet__top__DOT__vgc_inst__DOT__tile_inst__DOT__col_buffer__v28 = 1U;
                                                                        vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT____Vlvbound_ha057723b__0 
                                                                            = vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__tregs[8U];
                                                                        __VdlyVal__top__DOT__vgc_inst__DOT__tile_inst__DOT__col_buffer__v29 
                                                                            = vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT____Vlvbound_ha057723b__0;
                                                                        __VdlySet__top__DOT__vgc_inst__DOT__tile_inst__DOT__col_buffer__v29 = 1U;
                                                                        vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT____Vlvbound_ha057723b__0 
                                                                            = vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__tregs[8U];
                                                                        __VdlyVal__top__DOT__vgc_inst__DOT__tile_inst__DOT__col_buffer__v30 
                                                                            = vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT____Vlvbound_ha057723b__0;
                                                                        __VdlySet__top__DOT__vgc_inst__DOT__tile_inst__DOT__col_buffer__v30 = 1U;
                                                                        vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT____Vlvbound_ha057723b__0 
                                                                            = vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__tregs[8U];
                                                                        __VdlyVal__top__DOT__vgc_inst__DOT__tile_inst__DOT__col_buffer__v31 
                                                                            = vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT____Vlvbound_ha057723b__0;
                                                                        __VdlySet__top__DOT__vgc_inst__DOT__tile_inst__DOT__col_buffer__v31 = 1U;
                                                                        vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT____Vlvbound_ha057723b__0 
                                                                            = vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__tregs[8U];
                                                                        __VdlyVal__top__DOT__vgc_inst__DOT__tile_inst__DOT__col_buffer__v32 
                                                                            = vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT____Vlvbound_ha057723b__0;
                                                                        __VdlySet__top__DOT__vgc_inst__DOT__tile_inst__DOT__col_buffer__v32 = 1U;
                                                                        vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT____Vlvbound_ha057723b__0 
                                                                            = vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__tregs[8U];
                                                                        __VdlyVal__top__DOT__vgc_inst__DOT__tile_inst__DOT__col_buffer__v33 
                                                                            = vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT____Vlvbound_ha057723b__0;
                                                                        __VdlySet__top__DOT__vgc_inst__DOT__tile_inst__DOT__col_buffer__v33 = 1U;
                                                                        vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT____Vlvbound_ha057723b__0 
                                                                            = vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__tregs[8U];
                                                                        __VdlyVal__top__DOT__vgc_inst__DOT__tile_inst__DOT__col_buffer__v34 
                                                                            = vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT____Vlvbound_ha057723b__0;
                                                                        __VdlySet__top__DOT__vgc_inst__DOT__tile_inst__DOT__col_buffer__v34 = 1U;
                                                                        vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT____Vlvbound_ha057723b__0 
                                                                            = vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__tregs[8U];
                                                                        __VdlyVal__top__DOT__vgc_inst__DOT__tile_inst__DOT__col_buffer__v35 
                                                                            = vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT____Vlvbound_ha057723b__0;
                                                                        __VdlySet__top__DOT__vgc_inst__DOT__tile_inst__DOT__col_buffer__v35 = 1U;
                                                                        vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT____Vlvbound_ha057723b__0 
                                                                            = vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__tregs[8U];
                                                                        __VdlyVal__top__DOT__vgc_inst__DOT__tile_inst__DOT__col_buffer__v36 
                                                                            = vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT____Vlvbound_ha057723b__0;
                                                                        __VdlySet__top__DOT__vgc_inst__DOT__tile_inst__DOT__col_buffer__v36 = 1U;
                                                                        vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT____Vlvbound_ha057723b__0 
                                                                            = vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__tregs[8U];
                                                                        __VdlyVal__top__DOT__vgc_inst__DOT__tile_inst__DOT__col_buffer__v37 
                                                                            = vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT____Vlvbound_ha057723b__0;
                                                                        __VdlySet__top__DOT__vgc_inst__DOT__tile_inst__DOT__col_buffer__v37 = 1U;
                                                                        vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT____Vlvbound_ha057723b__0 
                                                                            = vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__tregs[8U];
                                                                        __VdlyVal__top__DOT__vgc_inst__DOT__tile_inst__DOT__col_buffer__v38 
                                                                            = vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT____Vlvbound_ha057723b__0;
                                                                        __VdlySet__top__DOT__vgc_inst__DOT__tile_inst__DOT__col_buffer__v38 = 1U;
                                                                        vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT____Vlvbound_ha057723b__0 
                                                                            = vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__tregs[8U];
                                                                        __VdlyVal__top__DOT__vgc_inst__DOT__tile_inst__DOT__col_buffer__v39 
                                                                            = vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT____Vlvbound_ha057723b__0;
                                                                        __VdlySet__top__DOT__vgc_inst__DOT__tile_inst__DOT__col_buffer__v39 = 1U;
                                                                        vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT____Vlvbound_ha057723b__0 
                                                                            = vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__tregs[8U];
                                                                        __VdlyVal__top__DOT__vgc_inst__DOT__tile_inst__DOT__col_buffer__v40 
                                                                            = vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT____Vlvbound_ha057723b__0;
                                                                        __VdlySet__top__DOT__vgc_inst__DOT__tile_inst__DOT__col_buffer__v40 = 1U;
                                                                        vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT____Vlvbound_ha057723b__0 
                                                                            = vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__tregs[8U];
                                                                        __VdlyVal__top__DOT__vgc_inst__DOT__tile_inst__DOT__col_buffer__v41 
                                                                            = vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT____Vlvbound_ha057723b__0;
                                                                        __VdlySet__top__DOT__vgc_inst__DOT__tile_inst__DOT__col_buffer__v41 = 1U;
                                                                        vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT____Vlvbound_ha057723b__0 
                                                                            = vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__tregs[8U];
                                                                        __VdlyVal__top__DOT__vgc_inst__DOT__tile_inst__DOT__col_buffer__v42 
                                                                            = vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT____Vlvbound_ha057723b__0;
                                                                        __VdlySet__top__DOT__vgc_inst__DOT__tile_inst__DOT__col_buffer__v42 = 1U;
                                                                        vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT____Vlvbound_ha057723b__0 
                                                                            = vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__tregs[8U];
                                                                        __VdlyVal__top__DOT__vgc_inst__DOT__tile_inst__DOT__col_buffer__v43 
                                                                            = vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT____Vlvbound_ha057723b__0;
                                                                        __VdlySet__top__DOT__vgc_inst__DOT__tile_inst__DOT__col_buffer__v43 = 1U;
                                                                        vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT____Vlvbound_ha057723b__0 
                                                                            = vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__tregs[8U];
                                                                        __VdlyVal__top__DOT__vgc_inst__DOT__tile_inst__DOT__col_buffer__v44 
                                                                            = vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT____Vlvbound_ha057723b__0;
                                                                        __VdlySet__top__DOT__vgc_inst__DOT__tile_inst__DOT__col_buffer__v44 = 1U;
                                                                        vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT____Vlvbound_ha057723b__0 
                                                                            = vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__tregs[8U];
                                                                        __VdlyVal__top__DOT__vgc_inst__DOT__tile_inst__DOT__col_buffer__v45 
                                                                            = vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT____Vlvbound_ha057723b__0;
                                                                        __VdlySet__top__DOT__vgc_inst__DOT__tile_inst__DOT__col_buffer__v45 = 1U;
                                                                        vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT____Vlvbound_ha057723b__0 
                                                                            = vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__tregs[8U];
                                                                        __VdlyVal__top__DOT__vgc_inst__DOT__tile_inst__DOT__col_buffer__v46 
                                                                            = vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT____Vlvbound_ha057723b__0;
                                                                        __VdlySet__top__DOT__vgc_inst__DOT__tile_inst__DOT__col_buffer__v46 = 1U;
                                                                        vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT____Vlvbound_ha057723b__0 
                                                                            = vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__tregs[8U];
                                                                        __VdlyVal__top__DOT__vgc_inst__DOT__tile_inst__DOT__col_buffer__v47 
                                                                            = vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT____Vlvbound_ha057723b__0;
                                                                        __VdlySet__top__DOT__vgc_inst__DOT__tile_inst__DOT__col_buffer__v47 = 1U;
                                                                        vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT____Vlvbound_ha057723b__0 
                                                                            = vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__tregs[8U];
                                                                        __VdlyVal__top__DOT__vgc_inst__DOT__tile_inst__DOT__col_buffer__v48 
                                                                            = vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT____Vlvbound_ha057723b__0;
                                                                        __VdlySet__top__DOT__vgc_inst__DOT__tile_inst__DOT__col_buffer__v48 = 1U;
                                                                        vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT____Vlvbound_ha057723b__0 
                                                                            = vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__tregs[8U];
                                                                        __VdlyVal__top__DOT__vgc_inst__DOT__tile_inst__DOT__col_buffer__v49 
                                                                            = vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT____Vlvbound_ha057723b__0;
                                                                        __VdlySet__top__DOT__vgc_inst__DOT__tile_inst__DOT__col_buffer__v49 = 1U;
                                                                        vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT____Vlvbound_ha057723b__0 
                                                                            = vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__tregs[8U];
                                                                        __VdlyVal__top__DOT__vgc_inst__DOT__tile_inst__DOT__col_buffer__v50 
                                                                            = vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT____Vlvbound_ha057723b__0;
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
                                                                        __Vdly__top__DOT__vgc_inst__DOT__tile_inst__DOT__dma_count = 0U;
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
                                                                }
                                                            } else if (
                                                                       (2U 
                                                                        & (IData)(vlSelfRef.top__DOT__cpu_dout))) {
                                                                if (
                                                                    (1U 
                                                                     & (IData)(vlSelfRef.top__DOT__cpu_dout))) {
                                                                    __VdlyVal__top__DOT__vgc_inst__DOT__tile_inst__DOT__pal_ram__v1 
                                                                        = 
                                                                        ((0x00000f00U 
                                                                          & (vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__tregs[8U] 
                                                                             << 4U)) 
                                                                         | ((0x000000f0U 
                                                                             & vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__tregs[9U]) 
                                                                            | (0x0000000fU 
                                                                               & (vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__tregs[10U] 
                                                                                >> 4U))));
                                                                    __VdlyDim0__top__DOT__vgc_inst__DOT__tile_inst__DOT__pal_ram__v1 
                                                                        = 
                                                                        ((0x000000f0U 
                                                                          & (vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__tregs[14U] 
                                                                             << 4U)) 
                                                                         | (0x0000000fU 
                                                                            & vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__tregs[15U]));
                                                                    __VdlySet__top__DOT__vgc_inst__DOT__tile_inst__DOT__pal_ram__v1 = 1U;
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
                                                                    vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__unnamedblk6__DOT__i = 0U;
                                                                    while (
                                                                           VL_GTS_III(32, 0x000003e8U, vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__unnamedblk6__DOT__i)) {
                                                                        vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT____Vlvbound_h8898988b__0 
                                                                            = vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__tregs[9U];
                                                                        if (VL_LIKELY((
                                                                                (0x0f9fU 
                                                                                >= 
                                                                                (0x00000fffU 
                                                                                & (((IData)(0x000003e8U) 
                                                                                * vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__tregs[8U]) 
                                                                                + vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__unnamedblk6__DOT__i)))))) {
                                                                            __VdlyVal__top__DOT__vgc_inst__DOT__tile_inst__DOT__nametable__v29 
                                                                                = vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT____Vlvbound_h8898988b__0;
                                                                            __VdlyDim0__top__DOT__vgc_inst__DOT__tile_inst__DOT__nametable__v29 
                                                                                = 
                                                                                (0x00000fffU 
                                                                                & (((IData)(0x000003e8U) 
                                                                                * vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__tregs[8U]) 
                                                                                + vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__unnamedblk6__DOT__i));
                                                                            vlSelfRef.__VdlyCommitQueuetop__DOT__vgc_inst__DOT__tile_inst__DOT__nametable.enqueue(__VdlyVal__top__DOT__vgc_inst__DOT__tile_inst__DOT__nametable__v29, (IData)(__VdlyDim0__top__DOT__vgc_inst__DOT__tile_inst__DOT__nametable__v29));
                                                                        }
                                                                        vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__unnamedblk6__DOT__i 
                                                                            = 
                                                                            ((IData)(1U) 
                                                                             + vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__unnamedblk6__DOT__i);
                                                                    }
                                                                }
                                                            } else if (
                                                                       (((4U 
                                                                          > vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__tregs[8U]) 
                                                                         & (0x28U 
                                                                            > vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__tregs[9U])) 
                                                                        & (0x19U 
                                                                           > vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__tregs[10U]))) {
                                                                vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT____Vlvbound_ha3d87205__0 
                                                                    = vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__tregs[11U];
                                                                if (
                                                                    (0x0f9fU 
                                                                     >= 
                                                                     (0x00000fffU 
                                                                      & ((((IData)(0x000003e8U) 
                                                                           * vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__tregs[8U]) 
                                                                          + 
                                                                          ((IData)(0x00000028U) 
                                                                           * vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__tregs[10U])) 
                                                                         + vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__tregs[9U])))) {
                                                                    __VdlyVal__top__DOT__vgc_inst__DOT__tile_inst__DOT__attr_table__v4 
                                                                        = vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT____Vlvbound_ha3d87205__0;
                                                                    __VdlyDim0__top__DOT__vgc_inst__DOT__tile_inst__DOT__attr_table__v4 
                                                                        = 
                                                                        (0x00000fffU 
                                                                         & ((((IData)(0x000003e8U) 
                                                                              * vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__tregs[8U]) 
                                                                             + 
                                                                             ((IData)(0x00000028U) 
                                                                              * vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__tregs[10U])) 
                                                                            + vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__tregs[9U]));
                                                                    __VdlySet__top__DOT__vgc_inst__DOT__tile_inst__DOT__attr_table__v4 = 1U;
                                                                }
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
                                                                    vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT____Vlvbound_h353a15af__0 
                                                                        = vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__tregs[11U];
                                                                    if (
                                                                        (0x0f9fU 
                                                                         >= 
                                                                         (0x00000fffU 
                                                                          & ((((IData)(0x000003e8U) 
                                                                               * vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__tregs[8U]) 
                                                                              + 
                                                                              ((IData)(0x00000028U) 
                                                                               * vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__tregs[10U])) 
                                                                             + vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__tregs[9U])))) {
                                                                        __VdlyVal__top__DOT__vgc_inst__DOT__tile_inst__DOT__nametable__v30 
                                                                            = vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT____Vlvbound_h353a15af__0;
                                                                        __VdlyDim0__top__DOT__vgc_inst__DOT__tile_inst__DOT__nametable__v30 
                                                                            = 
                                                                            (0x00000fffU 
                                                                             & ((((IData)(0x000003e8U) 
                                                                                * vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__tregs[8U]) 
                                                                                + 
                                                                                ((IData)(0x00000028U) 
                                                                                * vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__tregs[10U])) 
                                                                                + vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__tregs[9U]));
                                                                        vlSelfRef.__VdlyCommitQueuetop__DOT__vgc_inst__DOT__tile_inst__DOT__nametable.enqueue(__VdlyVal__top__DOT__vgc_inst__DOT__tile_inst__DOT__nametable__v30, (IData)(__VdlyDim0__top__DOT__vgc_inst__DOT__tile_inst__DOT__nametable__v30));
                                                                    }
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
        }
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
        vlSelfRef.top__DOT__blt_inst__DOT__regs[2U] = 3U;
    }
    if (__VdlySet__top__DOT__blt_inst__DOT__regs__v20) {
        vlSelfRef.top__DOT__blt_inst__DOT__regs[1U] = 3U;
        vlSelfRef.top__DOT__blt_inst__DOT__regs[2U] = 5U;
    }
    if (__VdlySet__top__DOT__blt_inst__DOT__regs__v22) {
        vlSelfRef.top__DOT__blt_inst__DOT__regs[1U] = 3U;
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
    vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__dma_state 
        = __Vdly__top__DOT__vgc_inst__DOT__tile_inst__DOT__dma_state;
    vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__dma_data_valid 
        = __Vdly__top__DOT__vgc_inst__DOT__tile_inst__DOT__dma_data_valid;
    vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__dma_count 
        = __Vdly__top__DOT__vgc_inst__DOT__tile_inst__DOT__dma_count;
    vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__dma_col_idx 
        = __Vdly__top__DOT__vgc_inst__DOT__tile_inst__DOT__dma_col_idx;
    vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__dma_src 
        = __Vdly__top__DOT__vgc_inst__DOT__tile_inst__DOT__dma_src;
    vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__dma_dst 
        = __Vdly__top__DOT__vgc_inst__DOT__tile_inst__DOT__dma_dst;
    vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__dma_pal_r 
        = __Vdly__top__DOT__vgc_inst__DOT__tile_inst__DOT__dma_pal_r;
    vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__dma_pal_phase 
        = __Vdly__top__DOT__vgc_inst__DOT__tile_inst__DOT__dma_pal_phase;
    vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__dma_pal_g 
        = __Vdly__top__DOT__vgc_inst__DOT__tile_inst__DOT__dma_pal_g;
    if (__VdlySet__top__DOT__vgc_inst__DOT__tile_inst__DOT__tregs__v0) {
        vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__tregs[__VdlyDim0__top__DOT__vgc_inst__DOT__tile_inst__DOT__tregs__v0] 
            = __VdlyVal__top__DOT__vgc_inst__DOT__tile_inst__DOT__tregs__v0;
    }
    if (__VdlySet__top__DOT__vgc_inst__DOT__tile_inst__DOT__pal_ram__v0) {
        vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__pal_ram[__VdlyDim0__top__DOT__vgc_inst__DOT__tile_inst__DOT__pal_ram__v0] 
            = __VdlyVal__top__DOT__vgc_inst__DOT__tile_inst__DOT__pal_ram__v0;
    }
    if (__VdlySet__top__DOT__vgc_inst__DOT__tile_inst__DOT__pal_ram__v1) {
        vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__pal_ram[__VdlyDim0__top__DOT__vgc_inst__DOT__tile_inst__DOT__pal_ram__v1] 
            = __VdlyVal__top__DOT__vgc_inst__DOT__tile_inst__DOT__pal_ram__v1;
    }
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
    if (vlSelfRef.rst) {
        __Vdly__top__DOT__vgc_inst__DOT__copper_index = 0U;
    } else {
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
                >= vlSelfRef.top__DOT__vgc_inst__DOT__copper_pos
                [(0x0000001fU & (IData)(vlSelfRef.top__DOT__vgc_inst__DOT__copper_index))]))) {
            if ((1U == vlSelfRef.top__DOT__vgc_inst__DOT__copper_reg
                 [(0x0000001fU & (IData)(vlSelfRef.top__DOT__vgc_inst__DOT__copper_index))])) {
                vlSelfRef.top__DOT__vgc_inst__DOT__bg_color 
                    = (0x0000000fU & vlSelfRef.top__DOT__vgc_inst__DOT__copper_val
                       [(0x0000001fU & (IData)(vlSelfRef.top__DOT__vgc_inst__DOT__copper_index))]);
            } else if ((2U == vlSelfRef.top__DOT__vgc_inst__DOT__copper_reg
                        [(0x0000001fU & (IData)(vlSelfRef.top__DOT__vgc_inst__DOT__copper_index))])) {
                __Vdly__top__DOT__vgc_inst__DOT__fg_color 
                    = (0x0000000fU & vlSelfRef.top__DOT__vgc_inst__DOT__copper_val
                       [(0x0000001fU & (IData)(vlSelfRef.top__DOT__vgc_inst__DOT__copper_index))]);
            } else if ((0x0dU == vlSelfRef.top__DOT__vgc_inst__DOT__copper_reg
                        [(0x0000001fU & (IData)(vlSelfRef.top__DOT__vgc_inst__DOT__copper_index))])) {
                vlSelfRef.top__DOT__vgc_inst__DOT__border_color 
                    = (0x0000000fU & vlSelfRef.top__DOT__vgc_inst__DOT__copper_val
                       [(0x0000001fU & (IData)(vlSelfRef.top__DOT__vgc_inst__DOT__copper_index))]);
            } else {
                __VdlyVal__top__DOT__vgc_inst__DOT__regs__v38 
                    = vlSelfRef.top__DOT__vgc_inst__DOT__copper_val
                    [(0x0000001fU & (IData)(vlSelfRef.top__DOT__vgc_inst__DOT__copper_index))];
                __VdlyDim0__top__DOT__vgc_inst__DOT__regs__v38 
                    = (0x0000001fU & vlSelfRef.top__DOT__vgc_inst__DOT__copper_reg
                       [(0x0000001fU & (IData)(vlSelfRef.top__DOT__vgc_inst__DOT__copper_index))]);
                __VdlySet__top__DOT__vgc_inst__DOT__regs__v38 = 1U;
            }
            __Vdly__top__DOT__vgc_inst__DOT__copper_index 
                = (0x000001ffU & ((IData)(1U) + (IData)(vlSelfRef.top__DOT__vgc_inst__DOT__copper_index)));
        }
    }
    vlSelfRef.__VdlyCommitQueuetop__DOT__vgc_inst__DOT__tile_inst__DOT__nametable.commit(vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__nametable);
    if (__VdlySet__top__DOT__vgc_inst__DOT__tile_inst__DOT__attr_table__v0) {
        vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__attr_table[__VdlyDim0__top__DOT__vgc_inst__DOT__tile_inst__DOT__attr_table__v0] 
            = __VdlyVal__top__DOT__vgc_inst__DOT__tile_inst__DOT__attr_table__v0;
    }
    if (__VdlySet__top__DOT__vgc_inst__DOT__tile_inst__DOT__attr_table__v1) {
        vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__attr_table[__VdlyDim0__top__DOT__vgc_inst__DOT__tile_inst__DOT__attr_table__v1] 
            = __VdlyVal__top__DOT__vgc_inst__DOT__tile_inst__DOT__attr_table__v1;
    }
    if (__VdlySet__top__DOT__vgc_inst__DOT__tile_inst__DOT__attr_table__v2) {
        vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__attr_table[__VdlyDim0__top__DOT__vgc_inst__DOT__tile_inst__DOT__attr_table__v2] 
            = __VdlyVal__top__DOT__vgc_inst__DOT__tile_inst__DOT__attr_table__v2;
    }
    if (__VdlySet__top__DOT__vgc_inst__DOT__tile_inst__DOT__attr_table__v3) {
        vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__attr_table[__VdlyDim0__top__DOT__vgc_inst__DOT__tile_inst__DOT__attr_table__v3] 
            = __VdlyVal__top__DOT__vgc_inst__DOT__tile_inst__DOT__attr_table__v3;
    }
    if (__VdlySet__top__DOT__vgc_inst__DOT__tile_inst__DOT__attr_table__v4) {
        vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__attr_table[__VdlyDim0__top__DOT__vgc_inst__DOT__tile_inst__DOT__attr_table__v4] 
            = __VdlyVal__top__DOT__vgc_inst__DOT__tile_inst__DOT__attr_table__v4;
    }
    vlSelfRef.top__DOT__clear_input = (((~ (IData)(vlSelfRef.top__DOT__cpu_we)) 
                                        & (0xa00fU 
                                           == (IData)(vlSelfRef.top__DOT__cpu_addr))) 
                                       & (0U != vlSelfRef.top__DOT__ram[40975U]));
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
    vlSelfRef.top__DOT__blt_vgc_we = 0U;
    vlSelfRef.top__DOT__blt_vgc_wdata = 0U;
    top__DOT__blt_vgc_re = 0U;
    vlSelfRef.top__DOT__blt_vgc_space = 0U;
    vlSelfRef.dbg_cpu_a = vlSelfRef.top__DOT__cpu_inst__DOT__AXYS[0U];
    vlSelfRef.dbg_cpu_x = vlSelfRef.top__DOT__cpu_inst__DOT__AXYS[1U];
    vlSelfRef.dbg_cpu_y = vlSelfRef.top__DOT__cpu_inst__DOT__AXYS[2U];
    vlSelfRef.dbg_cpu_sp = vlSelfRef.top__DOT__cpu_inst__DOT__AXYS[3U];
    vlSelfRef.top__DOT__cpu_inst__DOT__adj_bcd = ((IData)(vlSelfRef.top__DOT__cpu_inst__DOT__adc_sbc) 
                                                  & (IData)(vlSelfRef.top__DOT__cpu_inst__DOT__D));
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
                                                     [vlSelfRef.top__DOT__cpu_addr])))));
    vlSelfRef.__VdfgRegularize_hebeb780c_0_3 = (1U 
                                                & (((~ (IData)(vlSelfRef.top__DOT__cpu_inst__DOT__I)) 
                                                    & (~ (IData)(vlSelfRef.irq_n))) 
                                                   | (IData)(vlSelfRef.top__DOT__cpu_inst__DOT__NMI_edge)));
    if (((2U == (IData)(vlSelfRef.top__DOT__blt_inst__DOT__state)) 
         | (4U == (IData)(vlSelfRef.top__DOT__blt_inst__DOT__state)))) {
        if ((1U & (~ (IData)(vlSelfRef.top__DOT__blt_inst__DOT__fill_mode)))) {
            if ((0U != (IData)(vlSelfRef.top__DOT__blt_inst__DOT__src_space))) {
                if ((5U != (IData)(vlSelfRef.top__DOT__blt_inst__DOT__src_space))) {
                    top__DOT__blt_vgc_re = 1U;
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
                vlSelfRef.top__DOT__blt_vgc_we = 1U;
                vlSelfRef.top__DOT__blt_vgc_wdata = 
                    ((5U == (IData)(vlSelfRef.top__DOT__blt_inst__DOT__state))
                      ? vlSelfRef.top__DOT__blt_inst__DOT__row_buf
                     [vlSelfRef.top__DOT__blt_inst__DOT__buf_idx]
                      : (IData)(vlSelfRef.top__DOT__blt_inst__DOT__read_byte));
                vlSelfRef.top__DOT__blt_vgc_space = vlSelfRef.top__DOT__blt_inst__DOT__dst_space;
            }
        }
    }
    if ((((0x0cU == (IData)(vlSelfRef.top__DOT__cpu_inst__DOT__state)) 
          | (8U == (IData)(vlSelfRef.top__DOT__cpu_inst__DOT__state))) 
         & (IData)(vlSelfRef.top__DOT____Vcellinp__cpu_inst__RDY))) {
        vlSelfRef.top__DOT__cpu_inst__DOT__adc_bcd 
            = ((0x61U == (0xe3U & (IData)(vlSelfRef.top__DOT__cpu_inst__DOT__IR))) 
               && (IData)(vlSelfRef.top__DOT__cpu_inst__DOT__D));
    }
    if (vlSelfRef.rst) {
        vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__tile_size16 = 0U;
    } else if (((IData)(vlSelfRef.top__DOT__cpu_we) 
                & (IData)(vlSelfRef.top__DOT__vgc_inst__DOT__tile_reg_sel))) {
        if ((0U == (0x0000001fU & (IData)(vlSelfRef.top__DOT__cpu_addr)))) {
            vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__tile_size16 
                = (1U & (IData)(vlSelfRef.top__DOT__cpu_dout));
        }
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
        vlSelfRef.top__DOT__vgc_inst__DOT__unnamedblk14__DOT__i = 0U;
        vlSelfRef.top__DOT__vgc_inst__DOT__unnamedblk15__DOT__i = 0U;
        vlSelfRef.top__DOT__vgc_inst__DOT__unnamedblk16__DOT__i = 0U;
        __Vdly__top__DOT__vgc_inst__DOT__cursor_x = 0U;
        __Vdly__top__DOT__vgc_inst__DOT__cursor_y = 0U;
        vlSelfRef.top__DOT__vgc_inst__DOT__mode = 0U;
        __Vdly__top__DOT__vgc_inst__DOT__fg_color = 1U;
        vlSelfRef.top__DOT__vgc_inst__DOT__bg_color = 6U;
        vlSelfRef.top__DOT__vgc_inst__DOT__border_color = 6U;
        __Vdly__top__DOT__vgc_inst__DOT__gfx_color = 1U;
        vlSelfRef.top__DOT__vgc_inst__DOT__cursor_enable = 1U;
        vlSelfRef.top__DOT__vgc_inst__DOT__font_slot = 0U;
        __Vdly__top__DOT__vgc_inst__DOT__scroll_offset = 0U;
        __Vdly__top__DOT__vgc_inst__DOT__scroll_pending = 0U;
        __Vdly__top__DOT__vgc_inst__DOT__scroll_col = 0U;
        __Vdly__top__DOT__vgc_inst__DOT__cmd_busy = 0U;
        vlSelfRef.top__DOT__vgc_inst__DOT__char_in_reg = 0U;
        vlSelfRef.top__DOT__vgc_inst__DOT__copper_enabled = 0U;
        __Vdly__top__DOT__vgc_inst__DOT__copper_count = 0U;
        __Vdly__top__DOT__vgc_inst__DOT__copper_index = 0U;
        __Vdly__top__DOT__vgc_inst__DOT__copper_target_list = 0U;
        __Vdly__top__DOT__vgc_inst__DOT__copper_pending_list = 0U;
        __Vdly__top__DOT__vgc_inst__DOT__copper_loading = 0U;
        __Vdly__top__DOT__vgc_inst__DOT__fill_sp = 0U;
        __VdlySet__top__DOT__vgc_inst__DOT__regs__v0 = 1U;
        while (VL_GTS_III(32, 0x000007d0U, vlSelfRef.top__DOT__vgc_inst__DOT__unnamedblk14__DOT__i)) {
            vlSelfRef.top__DOT__vgc_inst__DOT____Vlvbound_h9d4d7d6f__0 = 0x20U;
            if (VL_LIKELY(((0x07cfU >= (0x000007ffU 
                                        & vlSelfRef.top__DOT__vgc_inst__DOT__unnamedblk14__DOT__i))))) {
                __VdlyVal__top__DOT__vgc_inst__DOT__char_ram__v0 
                    = vlSelfRef.top__DOT__vgc_inst__DOT____Vlvbound_h9d4d7d6f__0;
                __VdlyDim0__top__DOT__vgc_inst__DOT__char_ram__v0 
                    = (0x000007ffU & vlSelfRef.top__DOT__vgc_inst__DOT__unnamedblk14__DOT__i);
                vlSelfRef.__VdlyCommitQueuetop__DOT__vgc_inst__DOT__char_ram.enqueue(__VdlyVal__top__DOT__vgc_inst__DOT__char_ram__v0, (IData)(__VdlyDim0__top__DOT__vgc_inst__DOT__char_ram__v0));
            }
            vlSelfRef.top__DOT__vgc_inst__DOT____Vlvbound_hef6c2195__0 = 1U;
            if (VL_LIKELY(((0x07cfU >= (0x000007ffU 
                                        & vlSelfRef.top__DOT__vgc_inst__DOT__unnamedblk14__DOT__i))))) {
                __VdlyVal__top__DOT__vgc_inst__DOT__color_ram__v0 
                    = vlSelfRef.top__DOT__vgc_inst__DOT____Vlvbound_hef6c2195__0;
                __VdlyDim0__top__DOT__vgc_inst__DOT__color_ram__v0 
                    = (0x000007ffU & vlSelfRef.top__DOT__vgc_inst__DOT__unnamedblk14__DOT__i);
                vlSelfRef.__VdlyCommitQueuetop__DOT__vgc_inst__DOT__color_ram.enqueue(__VdlyVal__top__DOT__vgc_inst__DOT__color_ram__v0, (IData)(__VdlyDim0__top__DOT__vgc_inst__DOT__color_ram__v0));
            }
            vlSelfRef.top__DOT__vgc_inst__DOT__unnamedblk14__DOT__i 
                = ((IData)(1U) + vlSelfRef.top__DOT__vgc_inst__DOT__unnamedblk14__DOT__i);
        }
        while (VL_GTS_III(32, 0x0000fa00U, vlSelfRef.top__DOT__vgc_inst__DOT__unnamedblk15__DOT__i)) {
            vlSelfRef.top__DOT__vgc_inst__DOT____Vlvbound_h9d27345d__0 = 0U;
            if (VL_LIKELY(((0xf9ffU >= (0x0000ffffU 
                                        & vlSelfRef.top__DOT__vgc_inst__DOT__unnamedblk15__DOT__i))))) {
                __VdlyVal__top__DOT__vgc_inst__DOT__gfx_ram__v0 
                    = vlSelfRef.top__DOT__vgc_inst__DOT____Vlvbound_h9d27345d__0;
                __VdlyDim0__top__DOT__vgc_inst__DOT__gfx_ram__v0 
                    = (0x0000ffffU & vlSelfRef.top__DOT__vgc_inst__DOT__unnamedblk15__DOT__i);
                vlSelfRef.__VdlyCommitQueuetop__DOT__vgc_inst__DOT__gfx_ram.enqueue(__VdlyVal__top__DOT__vgc_inst__DOT__gfx_ram__v0, (IData)(__VdlyDim0__top__DOT__vgc_inst__DOT__gfx_ram__v0));
            }
            vlSelfRef.top__DOT__vgc_inst__DOT__unnamedblk15__DOT__i 
                = ((IData)(1U) + vlSelfRef.top__DOT__vgc_inst__DOT__unnamedblk15__DOT__i);
        }
        while (VL_GTS_III(32, 0x00000800U, vlSelfRef.top__DOT__vgc_inst__DOT__unnamedblk16__DOT__i)) {
            __VdlyDim0__top__DOT__vgc_inst__DOT__sprite_shapes__v0 
                = (0x000007ffU & vlSelfRef.top__DOT__vgc_inst__DOT__unnamedblk16__DOT__i);
            vlSelfRef.__VdlyCommitQueuetop__DOT__vgc_inst__DOT__sprite_shapes.enqueue(0U, (IData)(__VdlyDim0__top__DOT__vgc_inst__DOT__sprite_shapes__v0));
            vlSelfRef.top__DOT__vgc_inst__DOT__unnamedblk16__DOT__i 
                = ((IData)(1U) + vlSelfRef.top__DOT__vgc_inst__DOT__unnamedblk16__DOT__i);
        }
        __VdlySet__top__DOT__vgc_inst__DOT__regs__v1 = 1U;
    } else {
        if (vlSelfRef.top__DOT__vgc_inst__DOT__scroll_pending) {
            vlSelfRef.top__DOT__vgc_inst__DOT____Vlvbound_h902d5c6a__0 = 0x20U;
            if (VL_LIKELY(((0x07cfU >= ([&]() {
                                    __Vfunc_top__DOT__vgc_inst__DOT__screen_addr__5__col 
                                        = vlSelfRef.top__DOT__vgc_inst__DOT__scroll_col;
                                    __Vfunc_top__DOT__vgc_inst__DOT__screen_addr__5__rr 
                                        = (0x0000001fU 
                                           & ((IData)(0x18U) 
                                              + (IData)(vlSelfRef.top__DOT__vgc_inst__DOT__scroll_offset)));
                                    if ((0x19U <= (IData)(__Vfunc_top__DOT__vgc_inst__DOT__screen_addr__5__rr))) {
                                        __Vfunc_top__DOT__vgc_inst__DOT__screen_addr__5__rr 
                                            = (0x0000001fU 
                                               & ((IData)(__Vfunc_top__DOT__vgc_inst__DOT__screen_addr__5__rr) 
                                                  - (IData)(0x19U)));
                                    }
                                    __Vfunc_top__DOT__vgc_inst__DOT__screen_addr__5__Vfuncout 
                                        = (0x000007ffU 
                                           & (((IData)(0x00000050U) 
                                               * (IData)(__Vfunc_top__DOT__vgc_inst__DOT__screen_addr__5__rr)) 
                                              + (IData)(__Vfunc_top__DOT__vgc_inst__DOT__screen_addr__5__col)));
                                }(), (IData)(__Vfunc_top__DOT__vgc_inst__DOT__screen_addr__5__Vfuncout)))))) {
                __Vfunc_top__DOT__vgc_inst__DOT__screen_addr__6__col 
                    = vlSelfRef.top__DOT__vgc_inst__DOT__scroll_col;
                __Vfunc_top__DOT__vgc_inst__DOT__screen_addr__6__rr 
                    = (0x0000001fU & ((IData)(0x18U) 
                                      + (IData)(vlSelfRef.top__DOT__vgc_inst__DOT__scroll_offset)));
                if ((0x19U <= (IData)(__Vfunc_top__DOT__vgc_inst__DOT__screen_addr__6__rr))) {
                    __Vfunc_top__DOT__vgc_inst__DOT__screen_addr__6__rr 
                        = (0x0000001fU & ((IData)(__Vfunc_top__DOT__vgc_inst__DOT__screen_addr__6__rr) 
                                          - (IData)(0x19U)));
                }
                __Vfunc_top__DOT__vgc_inst__DOT__screen_addr__6__Vfuncout 
                    = (0x000007ffU & (((IData)(0x00000050U) 
                                       * (IData)(__Vfunc_top__DOT__vgc_inst__DOT__screen_addr__6__rr)) 
                                      + (IData)(__Vfunc_top__DOT__vgc_inst__DOT__screen_addr__6__col)));
                __VdlyVal__top__DOT__vgc_inst__DOT__char_ram__v1 
                    = vlSelfRef.top__DOT__vgc_inst__DOT____Vlvbound_h902d5c6a__0;
                __VdlyDim0__top__DOT__vgc_inst__DOT__char_ram__v1 
                    = __Vfunc_top__DOT__vgc_inst__DOT__screen_addr__6__Vfuncout;
                vlSelfRef.__VdlyCommitQueuetop__DOT__vgc_inst__DOT__char_ram.enqueue(__VdlyVal__top__DOT__vgc_inst__DOT__char_ram__v1, (IData)(__VdlyDim0__top__DOT__vgc_inst__DOT__char_ram__v1));
            }
            vlSelfRef.top__DOT__vgc_inst__DOT____Vlvbound_hdecc4094__0 
                = vlSelfRef.top__DOT__vgc_inst__DOT__fg_color;
            if (VL_LIKELY(((0x07cfU >= ([&]() {
                                    __Vfunc_top__DOT__vgc_inst__DOT__screen_addr__7__col 
                                        = vlSelfRef.top__DOT__vgc_inst__DOT__scroll_col;
                                    __Vfunc_top__DOT__vgc_inst__DOT__screen_addr__7__rr 
                                        = (0x0000001fU 
                                           & ((IData)(0x18U) 
                                              + (IData)(vlSelfRef.top__DOT__vgc_inst__DOT__scroll_offset)));
                                    if ((0x19U <= (IData)(__Vfunc_top__DOT__vgc_inst__DOT__screen_addr__7__rr))) {
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
                                }(), (IData)(__Vfunc_top__DOT__vgc_inst__DOT__screen_addr__7__Vfuncout)))))) {
                __Vfunc_top__DOT__vgc_inst__DOT__screen_addr__8__col 
                    = vlSelfRef.top__DOT__vgc_inst__DOT__scroll_col;
                __Vfunc_top__DOT__vgc_inst__DOT__screen_addr__8__rr 
                    = (0x0000001fU & ((IData)(0x18U) 
                                      + (IData)(vlSelfRef.top__DOT__vgc_inst__DOT__scroll_offset)));
                if ((0x19U <= (IData)(__Vfunc_top__DOT__vgc_inst__DOT__screen_addr__8__rr))) {
                    __Vfunc_top__DOT__vgc_inst__DOT__screen_addr__8__rr 
                        = (0x0000001fU & ((IData)(__Vfunc_top__DOT__vgc_inst__DOT__screen_addr__8__rr) 
                                          - (IData)(0x19U)));
                }
                __Vfunc_top__DOT__vgc_inst__DOT__screen_addr__8__Vfuncout 
                    = (0x000007ffU & (((IData)(0x00000050U) 
                                       * (IData)(__Vfunc_top__DOT__vgc_inst__DOT__screen_addr__8__rr)) 
                                      + (IData)(__Vfunc_top__DOT__vgc_inst__DOT__screen_addr__8__col)));
                __VdlyVal__top__DOT__vgc_inst__DOT__color_ram__v1 
                    = vlSelfRef.top__DOT__vgc_inst__DOT____Vlvbound_hdecc4094__0;
                __VdlyDim0__top__DOT__vgc_inst__DOT__color_ram__v1 
                    = __Vfunc_top__DOT__vgc_inst__DOT__screen_addr__8__Vfuncout;
                vlSelfRef.__VdlyCommitQueuetop__DOT__vgc_inst__DOT__color_ram.enqueue(__VdlyVal__top__DOT__vgc_inst__DOT__color_ram__v1, (IData)(__VdlyDim0__top__DOT__vgc_inst__DOT__color_ram__v1));
            }
            if ((0x4fU == (IData)(vlSelfRef.top__DOT__vgc_inst__DOT__scroll_col))) {
                __Vdly__top__DOT__vgc_inst__DOT__scroll_pending = 0U;
                __Vdly__top__DOT__vgc_inst__DOT__scroll_col = 0U;
            } else {
                __Vdly__top__DOT__vgc_inst__DOT__scroll_col 
                    = (0x0000007fU & ((IData)(1U) + (IData)(vlSelfRef.top__DOT__vgc_inst__DOT__scroll_col)));
            }
        }
        if (vlSelfRef.top__DOT__vgc_inst__DOT__cmd_busy) {
            if ((0x00000080U & (IData)(vlSelfRef.top__DOT__vgc_inst__DOT__cmd_op))) {
                if ((0x00000040U & (IData)(vlSelfRef.top__DOT__vgc_inst__DOT__cmd_op))) {
                    if ((0x00000020U & (IData)(vlSelfRef.top__DOT__vgc_inst__DOT__cmd_op))) {
                        if ((0x00000010U & (IData)(vlSelfRef.top__DOT__vgc_inst__DOT__cmd_op))) {
                            if ((8U & (IData)(vlSelfRef.top__DOT__vgc_inst__DOT__cmd_op))) {
                                __Vdly__top__DOT__vgc_inst__DOT__cmd_busy = 0U;
                            } else if ((4U & (IData)(vlSelfRef.top__DOT__vgc_inst__DOT__cmd_op))) {
                                __Vdly__top__DOT__vgc_inst__DOT__cmd_busy = 0U;
                            } else if ((2U & (IData)(vlSelfRef.top__DOT__vgc_inst__DOT__cmd_op))) {
                                __Vdly__top__DOT__vgc_inst__DOT__cmd_busy = 0U;
                            } else if ((1U & (IData)(vlSelfRef.top__DOT__vgc_inst__DOT__cmd_op))) {
                                __Vdly__top__DOT__vgc_inst__DOT__cmd_busy = 0U;
                            } else {
                                vlSelfRef.top__DOT__vgc_inst__DOT____Vlvbound_h6e0ff81d__0 = 0x20U;
                                if (VL_LIKELY(((0x07cfU 
                                                >= 
                                                (0x000007ffU 
                                                 & (((IData)(0x00000050U) 
                                                     * 
                                                     (0x0000001fU 
                                                      & (IData)(vlSelfRef.top__DOT__vgc_inst__DOT__cmd_cy))) 
                                                    + 
                                                    (0x0000007fU 
                                                     & (IData)(vlSelfRef.top__DOT__vgc_inst__DOT__cmd_cx)))))))) {
                                    __VdlyVal__top__DOT__vgc_inst__DOT__char_ram__v2 
                                        = vlSelfRef.top__DOT__vgc_inst__DOT____Vlvbound_h6e0ff81d__0;
                                    __VdlyDim0__top__DOT__vgc_inst__DOT__char_ram__v2 
                                        = (0x000007ffU 
                                           & (((IData)(0x00000050U) 
                                               * (0x0000001fU 
                                                  & (IData)(vlSelfRef.top__DOT__vgc_inst__DOT__cmd_cy))) 
                                              + (0x0000007fU 
                                                 & (IData)(vlSelfRef.top__DOT__vgc_inst__DOT__cmd_cx))));
                                    vlSelfRef.__VdlyCommitQueuetop__DOT__vgc_inst__DOT__char_ram.enqueue(__VdlyVal__top__DOT__vgc_inst__DOT__char_ram__v2, (IData)(__VdlyDim0__top__DOT__vgc_inst__DOT__char_ram__v2));
                                }
                                vlSelfRef.top__DOT__vgc_inst__DOT____Vlvbound_h1cc62cc7__0 
                                    = vlSelfRef.top__DOT__vgc_inst__DOT__fg_color;
                                if (VL_LIKELY(((0x07cfU 
                                                >= 
                                                (0x000007ffU 
                                                 & (((IData)(0x00000050U) 
                                                     * 
                                                     (0x0000001fU 
                                                      & (IData)(vlSelfRef.top__DOT__vgc_inst__DOT__cmd_cy))) 
                                                    + 
                                                    (0x0000007fU 
                                                     & (IData)(vlSelfRef.top__DOT__vgc_inst__DOT__cmd_cx)))))))) {
                                    __VdlyVal__top__DOT__vgc_inst__DOT__color_ram__v2 
                                        = vlSelfRef.top__DOT__vgc_inst__DOT____Vlvbound_h1cc62cc7__0;
                                    __VdlyDim0__top__DOT__vgc_inst__DOT__color_ram__v2 
                                        = (0x000007ffU 
                                           & (((IData)(0x00000050U) 
                                               * (0x0000001fU 
                                                  & (IData)(vlSelfRef.top__DOT__vgc_inst__DOT__cmd_cy))) 
                                              + (0x0000007fU 
                                                 & (IData)(vlSelfRef.top__DOT__vgc_inst__DOT__cmd_cx))));
                                    vlSelfRef.__VdlyCommitQueuetop__DOT__vgc_inst__DOT__color_ram.enqueue(__VdlyVal__top__DOT__vgc_inst__DOT__color_ram__v2, (IData)(__VdlyDim0__top__DOT__vgc_inst__DOT__color_ram__v2));
                                }
                                if ((0x004fU == (IData)(vlSelfRef.top__DOT__vgc_inst__DOT__cmd_cx))) {
                                    __Vdly__top__DOT__vgc_inst__DOT__cmd_cx = 0U;
                                    if ((0x18U == (IData)(vlSelfRef.top__DOT__vgc_inst__DOT__cmd_cy))) {
                                        __Vdly__top__DOT__vgc_inst__DOT__cmd_busy = 0U;
                                    } else {
                                        __Vdly__top__DOT__vgc_inst__DOT__cmd_cy 
                                            = (0x000000ffU 
                                               & ((IData)(1U) 
                                                  + (IData)(vlSelfRef.top__DOT__vgc_inst__DOT__cmd_cy)));
                                    }
                                } else {
                                    __Vdly__top__DOT__vgc_inst__DOT__cmd_cx 
                                        = (0x000001ffU 
                                           & ((IData)(1U) 
                                              + (IData)(vlSelfRef.top__DOT__vgc_inst__DOT__cmd_cx)));
                                }
                            }
                        } else {
                            __Vdly__top__DOT__vgc_inst__DOT__cmd_busy = 0U;
                        }
                    } else {
                        __Vdly__top__DOT__vgc_inst__DOT__cmd_busy = 0U;
                    }
                } else {
                    __Vdly__top__DOT__vgc_inst__DOT__cmd_busy = 0U;
                }
            } else if ((0x00000040U & (IData)(vlSelfRef.top__DOT__vgc_inst__DOT__cmd_op))) {
                __Vdly__top__DOT__vgc_inst__DOT__cmd_busy = 0U;
            } else if ((0x00000020U & (IData)(vlSelfRef.top__DOT__vgc_inst__DOT__cmd_op))) {
                __Vdly__top__DOT__vgc_inst__DOT__cmd_busy = 0U;
            } else if ((0x00000010U & (IData)(vlSelfRef.top__DOT__vgc_inst__DOT__cmd_op))) {
                if ((8U & (IData)(vlSelfRef.top__DOT__vgc_inst__DOT__cmd_op))) {
                    __Vdly__top__DOT__vgc_inst__DOT__cmd_busy = 0U;
                } else if ((4U & (IData)(vlSelfRef.top__DOT__vgc_inst__DOT__cmd_op))) {
                    __Vdly__top__DOT__vgc_inst__DOT__cmd_busy = 0U;
                } else if ((2U & (IData)(vlSelfRef.top__DOT__vgc_inst__DOT__cmd_op))) {
                    if ((1U & (IData)(vlSelfRef.top__DOT__vgc_inst__DOT__cmd_op))) {
                        __VdlyVal__top__DOT__vgc_inst__DOT__sprite_shapes__v1 
                            = vlSelfRef.top__DOT__vgc_inst__DOT__sprite_shapes
                            [((0x00000780U & ((IData)(vlSelfRef.top__DOT__vgc_inst__DOT__cmd_y) 
                                              << 7U)) 
                              | ((0x00000078U & ((IData)(vlSelfRef.top__DOT__vgc_inst__DOT__cmd_cy) 
                                                 << 3U)) 
                                 | (7U & (IData)(vlSelfRef.top__DOT__vgc_inst__DOT__cmd_cx))))];
                        __VdlyDim0__top__DOT__vgc_inst__DOT__sprite_shapes__v1 
                            = ((0x00000780U & ((IData)(vlSelfRef.top__DOT__vgc_inst__DOT__cmd_x) 
                                               << 7U)) 
                               | ((0x00000078U & ((IData)(vlSelfRef.top__DOT__vgc_inst__DOT__cmd_cy) 
                                                  << 3U)) 
                                  | (7U & (IData)(vlSelfRef.top__DOT__vgc_inst__DOT__cmd_cx))));
                        vlSelfRef.__VdlyCommitQueuetop__DOT__vgc_inst__DOT__sprite_shapes.enqueue(__VdlyVal__top__DOT__vgc_inst__DOT__sprite_shapes__v1, (IData)(__VdlyDim0__top__DOT__vgc_inst__DOT__sprite_shapes__v1));
                        if ((7U == (IData)(vlSelfRef.top__DOT__vgc_inst__DOT__cmd_cx))) {
                            __Vdly__top__DOT__vgc_inst__DOT__cmd_cx = 0U;
                            if ((0x0fU == (IData)(vlSelfRef.top__DOT__vgc_inst__DOT__cmd_cy))) {
                                __Vdly__top__DOT__vgc_inst__DOT__cmd_busy = 0U;
                            } else {
                                __Vdly__top__DOT__vgc_inst__DOT__cmd_cy 
                                    = (0x000000ffU 
                                       & ((IData)(1U) 
                                          + (IData)(vlSelfRef.top__DOT__vgc_inst__DOT__cmd_cy)));
                            }
                        } else {
                            __Vdly__top__DOT__vgc_inst__DOT__cmd_cx 
                                = (0x000001ffU & ((IData)(1U) 
                                                  + (IData)(vlSelfRef.top__DOT__vgc_inst__DOT__cmd_cx)));
                        }
                    } else {
                        __VdlyDim0__top__DOT__vgc_inst__DOT__sprite_shapes__v2 
                            = ((0x00000780U & ((IData)(vlSelfRef.top__DOT__vgc_inst__DOT__cmd_x) 
                                               << 6U)) 
                               | ((0x00000078U & ((IData)(vlSelfRef.top__DOT__vgc_inst__DOT__cmd_cy) 
                                                  << 3U)) 
                                  | (7U & (IData)(vlSelfRef.top__DOT__vgc_inst__DOT__cmd_cx))));
                        vlSelfRef.__VdlyCommitQueuetop__DOT__vgc_inst__DOT__sprite_shapes.enqueue(0U, (IData)(__VdlyDim0__top__DOT__vgc_inst__DOT__sprite_shapes__v2));
                        if ((7U == (IData)(vlSelfRef.top__DOT__vgc_inst__DOT__cmd_cx))) {
                            __Vdly__top__DOT__vgc_inst__DOT__cmd_cx = 0U;
                            if ((0x0fU == (IData)(vlSelfRef.top__DOT__vgc_inst__DOT__cmd_cy))) {
                                __Vdly__top__DOT__vgc_inst__DOT__cmd_busy = 0U;
                            } else {
                                __Vdly__top__DOT__vgc_inst__DOT__cmd_cy 
                                    = (0x000000ffU 
                                       & ((IData)(1U) 
                                          + (IData)(vlSelfRef.top__DOT__vgc_inst__DOT__cmd_cy)));
                            }
                        } else {
                            __Vdly__top__DOT__vgc_inst__DOT__cmd_cx 
                                = (0x000001ffU & ((IData)(1U) 
                                                  + (IData)(vlSelfRef.top__DOT__vgc_inst__DOT__cmd_cx)));
                        }
                    }
                } else {
                    __Vdly__top__DOT__vgc_inst__DOT__cmd_busy = 0U;
                }
            } else if ((8U & (IData)(vlSelfRef.top__DOT__vgc_inst__DOT__cmd_op))) {
                if ((4U & (IData)(vlSelfRef.top__DOT__vgc_inst__DOT__cmd_op))) {
                    __Vdly__top__DOT__vgc_inst__DOT__cmd_busy = 0U;
                } else if ((2U & (IData)(vlSelfRef.top__DOT__vgc_inst__DOT__cmd_op))) {
                    if ((1U & (IData)(vlSelfRef.top__DOT__vgc_inst__DOT__cmd_op))) {
                        __Vdly__top__DOT__vgc_inst__DOT__cmd_busy = 0U;
                    } else if (((IData)(vlSelfRef.top__DOT__vgc_inst__DOT__gt_char_idx) 
                                < (IData)(vlSelfRef.top__DOT__vgc_inst__DOT__gt_char_len))) {
                        if ((1U & ((IData)(vlSelfRef.top__DOT__vgc_inst__DOT__gt_font_byte) 
                                   >> (7U & ((IData)(7U) 
                                             - (IData)(vlSelfRef.top__DOT__vgc_inst__DOT__gt_font_col)))))) {
                            if (((0x00000140U > (((IData)(vlSelfRef.top__DOT__vgc_inst__DOT__gt_pen_x) 
                                                  + 
                                                  ((IData)(vlSelfRef.top__DOT__vgc_inst__DOT__gt_font_col) 
                                                   * (IData)(vlSelfRef.top__DOT__vgc_inst__DOT__gt_scale))) 
                                                 + (IData)(vlSelfRef.top__DOT__vgc_inst__DOT__gt_scale_x))) 
                                 & (0x000000c8U > (
                                                   ((IData)(vlSelfRef.top__DOT__vgc_inst__DOT__gt_start_y) 
                                                    + 
                                                    ((IData)(vlSelfRef.top__DOT__vgc_inst__DOT__gt_font_row) 
                                                     * (IData)(vlSelfRef.top__DOT__vgc_inst__DOT__gt_scale))) 
                                                   + (IData)(vlSelfRef.top__DOT__vgc_inst__DOT__gt_scale_y))))) {
                                vlSelfRef.top__DOT__vgc_inst__DOT____Vlvbound_he1a62a70__0 
                                    = vlSelfRef.top__DOT__vgc_inst__DOT__gfx_color;
                                if (VL_LIKELY(((0xf9ffU 
                                                >= 
                                                (0x0000ffffU 
                                                 & (((IData)(0x00000140U) 
                                                     * 
                                                     (((IData)(vlSelfRef.top__DOT__vgc_inst__DOT__gt_start_y) 
                                                       + 
                                                       ((IData)(vlSelfRef.top__DOT__vgc_inst__DOT__gt_font_row) 
                                                        * (IData)(vlSelfRef.top__DOT__vgc_inst__DOT__gt_scale))) 
                                                      + (IData)(vlSelfRef.top__DOT__vgc_inst__DOT__gt_scale_y))) 
                                                    + 
                                                    (((IData)(vlSelfRef.top__DOT__vgc_inst__DOT__gt_pen_x) 
                                                      + 
                                                      ((IData)(vlSelfRef.top__DOT__vgc_inst__DOT__gt_font_col) 
                                                       * (IData)(vlSelfRef.top__DOT__vgc_inst__DOT__gt_scale))) 
                                                     + (IData)(vlSelfRef.top__DOT__vgc_inst__DOT__gt_scale_x)))))))) {
                                    __VdlyVal__top__DOT__vgc_inst__DOT__gfx_ram__v1 
                                        = vlSelfRef.top__DOT__vgc_inst__DOT____Vlvbound_he1a62a70__0;
                                    __VdlyDim0__top__DOT__vgc_inst__DOT__gfx_ram__v1 
                                        = (0x0000ffffU 
                                           & (((IData)(0x00000140U) 
                                               * (((IData)(vlSelfRef.top__DOT__vgc_inst__DOT__gt_start_y) 
                                                   + 
                                                   ((IData)(vlSelfRef.top__DOT__vgc_inst__DOT__gt_font_row) 
                                                    * (IData)(vlSelfRef.top__DOT__vgc_inst__DOT__gt_scale))) 
                                                  + (IData)(vlSelfRef.top__DOT__vgc_inst__DOT__gt_scale_y))) 
                                              + (((IData)(vlSelfRef.top__DOT__vgc_inst__DOT__gt_pen_x) 
                                                  + 
                                                  ((IData)(vlSelfRef.top__DOT__vgc_inst__DOT__gt_font_col) 
                                                   * (IData)(vlSelfRef.top__DOT__vgc_inst__DOT__gt_scale))) 
                                                 + (IData)(vlSelfRef.top__DOT__vgc_inst__DOT__gt_scale_x))));
                                    vlSelfRef.__VdlyCommitQueuetop__DOT__vgc_inst__DOT__gfx_ram.enqueue(__VdlyVal__top__DOT__vgc_inst__DOT__gfx_ram__v1, (IData)(__VdlyDim0__top__DOT__vgc_inst__DOT__gfx_ram__v1));
                                }
                            }
                        }
                        if (((0x000000ffU & ((IData)(1U) 
                                             + (IData)(vlSelfRef.top__DOT__vgc_inst__DOT__gt_scale_x))) 
                             < (IData)(vlSelfRef.top__DOT__vgc_inst__DOT__gt_scale))) {
                            vlSelfRef.top__DOT__vgc_inst__DOT__gt_scale_x 
                                = (0x000000ffU & ((IData)(1U) 
                                                  + (IData)(vlSelfRef.top__DOT__vgc_inst__DOT__gt_scale_x)));
                        } else {
                            vlSelfRef.top__DOT__vgc_inst__DOT__gt_scale_x = 0U;
                            if ((7U > (IData)(vlSelfRef.top__DOT__vgc_inst__DOT__gt_font_col))) {
                                vlSelfRef.top__DOT__vgc_inst__DOT__gt_font_col 
                                    = (7U & ((IData)(1U) 
                                             + (IData)(vlSelfRef.top__DOT__vgc_inst__DOT__gt_font_col)));
                            } else {
                                vlSelfRef.top__DOT__vgc_inst__DOT__gt_font_col = 0U;
                                if (((0x000000ffU & 
                                      ((IData)(1U) 
                                       + (IData)(vlSelfRef.top__DOT__vgc_inst__DOT__gt_scale_y))) 
                                     < (IData)(vlSelfRef.top__DOT__vgc_inst__DOT__gt_scale))) {
                                    vlSelfRef.top__DOT__vgc_inst__DOT__gt_scale_y 
                                        = (0x000000ffU 
                                           & ((IData)(1U) 
                                              + (IData)(vlSelfRef.top__DOT__vgc_inst__DOT__gt_scale_y)));
                                } else {
                                    vlSelfRef.top__DOT__vgc_inst__DOT__gt_scale_y = 0U;
                                    if ((7U > (IData)(vlSelfRef.top__DOT__vgc_inst__DOT__gt_font_row))) {
                                        __Vdly__top__DOT__vgc_inst__DOT__gt_font_row 
                                            = (7U & 
                                               ((IData)(1U) 
                                                + (IData)(vlSelfRef.top__DOT__vgc_inst__DOT__gt_font_row)));
                                        vlSelfRef.top__DOT__vgc_inst__DOT__gt_font_byte 
                                            = vlSelfRef.top__DOT__vgc_inst__DOT__font_rom
                                            [(((IData)(vlSelfRef.top__DOT__vgc_inst__DOT__gt_cur_char) 
                                               << 3U) 
                                              | (7U 
                                                 & ((IData)(1U) 
                                                    + (IData)(vlSelfRef.top__DOT__vgc_inst__DOT__gt_font_row))))];
                                    } else {
                                        __Vdly__top__DOT__vgc_inst__DOT__gt_char_idx 
                                            = (0x0000003fU 
                                               & ((IData)(1U) 
                                                  + (IData)(vlSelfRef.top__DOT__vgc_inst__DOT__gt_char_idx)));
                                        vlSelfRef.top__DOT__vgc_inst__DOT__gt_pen_x 
                                            = (0x000001ffU 
                                               & ((IData)(vlSelfRef.top__DOT__vgc_inst__DOT__gt_pen_x) 
                                                  + 
                                                  (0x0000ffffU 
                                                   & VL_SHIFTL_III(16,16,32, (IData)(vlSelfRef.top__DOT__vgc_inst__DOT__gt_scale), 3U))));
                                        __Vdly__top__DOT__vgc_inst__DOT__gt_font_row = 0U;
                                        if ((((IData)(1U) 
                                              + (IData)(vlSelfRef.top__DOT__vgc_inst__DOT__gt_char_idx)) 
                                             < (IData)(vlSelfRef.top__DOT__vgc_inst__DOT__gt_char_len))) {
                                            vlSelfRef.top__DOT__vgc_inst__DOT__gt_cur_char 
                                                = vlSelfRef.top__DOT__vgc_inst__DOT__fio_name
                                                [(0x0000003fU 
                                                  & ((IData)(1U) 
                                                     + (IData)(vlSelfRef.top__DOT__vgc_inst__DOT__gt_char_idx)))];
                                            vlSelfRef.top__DOT__vgc_inst__DOT__gt_font_byte 
                                                = vlSelfRef.top__DOT__vgc_inst__DOT__font_rom
                                                [(vlSelfRef.top__DOT__vgc_inst__DOT__fio_name
                                                  [
                                                  (0x0000003fU 
                                                   & ((IData)(1U) 
                                                      + (IData)(vlSelfRef.top__DOT__vgc_inst__DOT__gt_char_idx)))] 
                                                  << 3U)];
                                        }
                                    }
                                }
                            }
                        }
                    } else {
                        __Vdly__top__DOT__vgc_inst__DOT__cmd_busy = 0U;
                    }
                } else if ((1U & (IData)(vlSelfRef.top__DOT__vgc_inst__DOT__cmd_op))) {
                    if (((0U < (IData)(vlSelfRef.top__DOT__vgc_inst__DOT__fill_sp)) 
                         & (0x0200U > (IData)(vlSelfRef.top__DOT__vgc_inst__DOT__fill_sp)))) {
                        __Vdly__top__DOT__vgc_inst__DOT__fill_sp 
                            = (0x000003ffU & ((IData)(vlSelfRef.top__DOT__vgc_inst__DOT__fill_sp) 
                                              - (IData)(1U)));
                        __Vdly__top__DOT__vgc_inst__DOT__cmd_cx 
                            = (0x000001ffU & vlSelfRef.top__DOT__vgc_inst__DOT__fill_stack
                               [(0x000001ffU & ((IData)(vlSelfRef.top__DOT__vgc_inst__DOT__fill_sp) 
                                                - (IData)(1U)))]);
                        __Vdly__top__DOT__vgc_inst__DOT__cmd_cy 
                            = (0x000000ffU & (vlSelfRef.top__DOT__vgc_inst__DOT__fill_stack
                                              [(0x000001ffU 
                                                & ((IData)(vlSelfRef.top__DOT__vgc_inst__DOT__fill_sp) 
                                                   - (IData)(1U)))] 
                                              >> 9U));
                    } else if ((0U == (IData)(vlSelfRef.top__DOT__vgc_inst__DOT__fill_sp))) {
                        __Vdly__top__DOT__vgc_inst__DOT__cmd_busy = 0U;
                    } else if (((((0x0140U > (IData)(vlSelfRef.top__DOT__vgc_inst__DOT__cmd_cx)) 
                                  & (0xc8U > (IData)(vlSelfRef.top__DOT__vgc_inst__DOT__cmd_cy))) 
                                 & (((0xf9ffU >= (0x0000ffffU 
                                                  & (((IData)(0x00000140U) 
                                                      * (IData)(vlSelfRef.top__DOT__vgc_inst__DOT__cmd_cy)) 
                                                     + (IData)(vlSelfRef.top__DOT__vgc_inst__DOT__cmd_cx))))
                                      ? vlSelfRef.top__DOT__vgc_inst__DOT__gfx_ram
                                     [(0x0000ffffU 
                                       & (((IData)(0x00000140U) 
                                           * (IData)(vlSelfRef.top__DOT__vgc_inst__DOT__cmd_cy)) 
                                          + (IData)(vlSelfRef.top__DOT__vgc_inst__DOT__cmd_cx)))]
                                      : 0U) == (IData)(vlSelfRef.top__DOT__vgc_inst__DOT__fill_target))) 
                                & ((IData)(vlSelfRef.top__DOT__vgc_inst__DOT__fill_target) 
                                   != (IData)(vlSelfRef.top__DOT__vgc_inst__DOT__gfx_color)))) {
                        vlSelfRef.top__DOT__vgc_inst__DOT____Vlvbound_h3f27e46b__3 
                            = vlSelfRef.top__DOT__vgc_inst__DOT__gfx_color;
                        if (VL_LIKELY(((0xf9ffU >= 
                                        (0x0000ffffU 
                                         & (((IData)(0x00000140U) 
                                             * (IData)(vlSelfRef.top__DOT__vgc_inst__DOT__cmd_cy)) 
                                            + (IData)(vlSelfRef.top__DOT__vgc_inst__DOT__cmd_cx))))))) {
                            __VdlyVal__top__DOT__vgc_inst__DOT__gfx_ram__v2 
                                = vlSelfRef.top__DOT__vgc_inst__DOT____Vlvbound_h3f27e46b__3;
                            __VdlyDim0__top__DOT__vgc_inst__DOT__gfx_ram__v2 
                                = (0x0000ffffU & (((IData)(0x00000140U) 
                                                   * (IData)(vlSelfRef.top__DOT__vgc_inst__DOT__cmd_cy)) 
                                                  + (IData)(vlSelfRef.top__DOT__vgc_inst__DOT__cmd_cx)));
                            vlSelfRef.__VdlyCommitQueuetop__DOT__vgc_inst__DOT__gfx_ram.enqueue(__VdlyVal__top__DOT__vgc_inst__DOT__gfx_ram__v2, (IData)(__VdlyDim0__top__DOT__vgc_inst__DOT__gfx_ram__v2));
                        }
                        if ((0x00000200U > ((IData)(3U) 
                                            + (IData)(vlSelfRef.top__DOT__vgc_inst__DOT__fill_sp)))) {
                            __VdlyVal__top__DOT__vgc_inst__DOT__fill_stack__v0 
                                = (((IData)(vlSelfRef.top__DOT__vgc_inst__DOT__cmd_cy) 
                                    << 9U) | (0x000001ffU 
                                              & ((IData)(1U) 
                                                 + (IData)(vlSelfRef.top__DOT__vgc_inst__DOT__cmd_cx))));
                            __VdlyDim0__top__DOT__vgc_inst__DOT__fill_stack__v0 
                                = (0x000001ffU & (IData)(vlSelfRef.top__DOT__vgc_inst__DOT__fill_sp));
                            __VdlySet__top__DOT__vgc_inst__DOT__fill_stack__v0 = 1U;
                            __VdlyVal__top__DOT__vgc_inst__DOT__fill_stack__v1 
                                = (((IData)(vlSelfRef.top__DOT__vgc_inst__DOT__cmd_cy) 
                                    << 9U) | (0x000001ffU 
                                              & ((IData)(vlSelfRef.top__DOT__vgc_inst__DOT__cmd_cx) 
                                                 - (IData)(1U))));
                            __VdlyDim0__top__DOT__vgc_inst__DOT__fill_stack__v1 
                                = (0x000001ffU & ((IData)(1U) 
                                                  + (IData)(vlSelfRef.top__DOT__vgc_inst__DOT__fill_sp)));
                            __VdlyVal__top__DOT__vgc_inst__DOT__fill_stack__v2 
                                = ((0x0001fe00U & (
                                                   ((IData)(1U) 
                                                    + (IData)(vlSelfRef.top__DOT__vgc_inst__DOT__cmd_cy)) 
                                                   << 9U)) 
                                   | (IData)(vlSelfRef.top__DOT__vgc_inst__DOT__cmd_cx));
                            __VdlyDim0__top__DOT__vgc_inst__DOT__fill_stack__v2 
                                = (0x000001ffU & ((IData)(2U) 
                                                  + (IData)(vlSelfRef.top__DOT__vgc_inst__DOT__fill_sp)));
                            __VdlyVal__top__DOT__vgc_inst__DOT__fill_stack__v3 
                                = ((0x0001fe00U & (
                                                   ((IData)(vlSelfRef.top__DOT__vgc_inst__DOT__cmd_cy) 
                                                    - (IData)(1U)) 
                                                   << 9U)) 
                                   | (IData)(vlSelfRef.top__DOT__vgc_inst__DOT__cmd_cx));
                            __VdlyDim0__top__DOT__vgc_inst__DOT__fill_stack__v3 
                                = (0x000001ffU & ((IData)(3U) 
                                                  + (IData)(vlSelfRef.top__DOT__vgc_inst__DOT__fill_sp)));
                            __Vdly__top__DOT__vgc_inst__DOT__fill_sp 
                                = (0x000003ffU & ((IData)(4U) 
                                                  + (IData)(vlSelfRef.top__DOT__vgc_inst__DOT__fill_sp)));
                        }
                    }
                } else {
                    __Vdly__top__DOT__vgc_inst__DOT__cmd_busy = 0U;
                }
            } else if ((4U & (IData)(vlSelfRef.top__DOT__vgc_inst__DOT__cmd_op))) {
                if ((2U & (IData)(vlSelfRef.top__DOT__vgc_inst__DOT__cmd_op))) {
                    if ((1U & (IData)(vlSelfRef.top__DOT__vgc_inst__DOT__cmd_op))) {
                        vlSelfRef.top__DOT__vgc_inst__DOT____Vlvbound_h3f27e46b__2 = 0U;
                        if (VL_LIKELY(((0xf9ffU >= 
                                        (0x0000ffffU 
                                         & (((IData)(0x00000140U) 
                                             * (IData)(vlSelfRef.top__DOT__vgc_inst__DOT__cmd_cy)) 
                                            + (IData)(vlSelfRef.top__DOT__vgc_inst__DOT__cmd_cx))))))) {
                            __VdlyVal__top__DOT__vgc_inst__DOT__gfx_ram__v3 
                                = vlSelfRef.top__DOT__vgc_inst__DOT____Vlvbound_h3f27e46b__2;
                            __VdlyDim0__top__DOT__vgc_inst__DOT__gfx_ram__v3 
                                = (0x0000ffffU & (((IData)(0x00000140U) 
                                                   * (IData)(vlSelfRef.top__DOT__vgc_inst__DOT__cmd_cy)) 
                                                  + (IData)(vlSelfRef.top__DOT__vgc_inst__DOT__cmd_cx)));
                            vlSelfRef.__VdlyCommitQueuetop__DOT__vgc_inst__DOT__gfx_ram.enqueue(__VdlyVal__top__DOT__vgc_inst__DOT__gfx_ram__v3, (IData)(__VdlyDim0__top__DOT__vgc_inst__DOT__gfx_ram__v3));
                        }
                        if ((0x013fU == (IData)(vlSelfRef.top__DOT__vgc_inst__DOT__cmd_cx))) {
                            __Vdly__top__DOT__vgc_inst__DOT__cmd_cx = 0U;
                            if ((0xc7U == (IData)(vlSelfRef.top__DOT__vgc_inst__DOT__cmd_cy))) {
                                __Vdly__top__DOT__vgc_inst__DOT__cmd_busy = 0U;
                            } else {
                                __Vdly__top__DOT__vgc_inst__DOT__cmd_cy 
                                    = (0x000000ffU 
                                       & ((IData)(1U) 
                                          + (IData)(vlSelfRef.top__DOT__vgc_inst__DOT__cmd_cy)));
                            }
                        } else {
                            __Vdly__top__DOT__vgc_inst__DOT__cmd_cx 
                                = (0x000001ffU & ((IData)(1U) 
                                                  + (IData)(vlSelfRef.top__DOT__vgc_inst__DOT__cmd_cx)));
                        }
                    } else {
                        if (((0x0140U > (IData)(vlSelfRef.top__DOT__vgc_inst__DOT__cmd_cx)) 
                             & (0xc8U > (IData)(vlSelfRef.top__DOT__vgc_inst__DOT__cmd_cy)))) {
                            vlSelfRef.top__DOT__vgc_inst__DOT____Vlvbound_h3f27e46b__1 
                                = vlSelfRef.top__DOT__vgc_inst__DOT__gfx_color;
                            if (VL_LIKELY(((0xf9ffU 
                                            >= (0x0000ffffU 
                                                & (((IData)(0x00000140U) 
                                                    * (IData)(vlSelfRef.top__DOT__vgc_inst__DOT__cmd_cy)) 
                                                   + (IData)(vlSelfRef.top__DOT__vgc_inst__DOT__cmd_cx))))))) {
                                __VdlyVal__top__DOT__vgc_inst__DOT__gfx_ram__v4 
                                    = vlSelfRef.top__DOT__vgc_inst__DOT____Vlvbound_h3f27e46b__1;
                                __VdlyDim0__top__DOT__vgc_inst__DOT__gfx_ram__v4 
                                    = (0x0000ffffU 
                                       & (((IData)(0x00000140U) 
                                           * (IData)(vlSelfRef.top__DOT__vgc_inst__DOT__cmd_cy)) 
                                          + (IData)(vlSelfRef.top__DOT__vgc_inst__DOT__cmd_cx)));
                                vlSelfRef.__VdlyCommitQueuetop__DOT__vgc_inst__DOT__gfx_ram.enqueue(__VdlyVal__top__DOT__vgc_inst__DOT__gfx_ram__v4, (IData)(__VdlyDim0__top__DOT__vgc_inst__DOT__gfx_ram__v4));
                            }
                        }
                        if (((IData)(vlSelfRef.top__DOT__vgc_inst__DOT__cmd_cx) 
                             >= (0x000001ffU & (IData)(vlSelfRef.top__DOT__vgc_inst__DOT__cmd_x2)))) {
                            __Vdly__top__DOT__vgc_inst__DOT__cmd_cx 
                                = (0x000001ffU & (IData)(vlSelfRef.top__DOT__vgc_inst__DOT__cmd_x));
                            if (((IData)(vlSelfRef.top__DOT__vgc_inst__DOT__cmd_cy) 
                                 >= (0x000000ffU & (IData)(vlSelfRef.top__DOT__vgc_inst__DOT__cmd_y2)))) {
                                __Vdly__top__DOT__vgc_inst__DOT__cmd_busy = 0U;
                            } else {
                                __Vdly__top__DOT__vgc_inst__DOT__cmd_cy 
                                    = (0x000000ffU 
                                       & ((IData)(1U) 
                                          + (IData)(vlSelfRef.top__DOT__vgc_inst__DOT__cmd_cy)));
                            }
                        } else {
                            __Vdly__top__DOT__vgc_inst__DOT__cmd_cx 
                                = (0x000001ffU & ((IData)(1U) 
                                                  + (IData)(vlSelfRef.top__DOT__vgc_inst__DOT__cmd_cx)));
                        }
                    }
                } else if ((1U & (IData)(vlSelfRef.top__DOT__vgc_inst__DOT__cmd_op))) {
                    if (((0x0140U > (IData)(vlSelfRef.top__DOT__vgc_inst__DOT__cmd_cx)) 
                         & (0xc8U > (IData)(vlSelfRef.top__DOT__vgc_inst__DOT__cmd_cy)))) {
                        if ((((((IData)(vlSelfRef.top__DOT__vgc_inst__DOT__cmd_cy) 
                                == (0x000000ffU & (IData)(vlSelfRef.top__DOT__vgc_inst__DOT__cmd_y))) 
                               | ((IData)(vlSelfRef.top__DOT__vgc_inst__DOT__cmd_cy) 
                                  == (0x000000ffU & (IData)(vlSelfRef.top__DOT__vgc_inst__DOT__cmd_y2)))) 
                              | ((IData)(vlSelfRef.top__DOT__vgc_inst__DOT__cmd_cx) 
                                 == (0x000001ffU & (IData)(vlSelfRef.top__DOT__vgc_inst__DOT__cmd_x)))) 
                             | ((IData)(vlSelfRef.top__DOT__vgc_inst__DOT__cmd_cx) 
                                == (0x000001ffU & (IData)(vlSelfRef.top__DOT__vgc_inst__DOT__cmd_x2))))) {
                            vlSelfRef.top__DOT__vgc_inst__DOT____Vlvbound_h3f27e46b__0 
                                = vlSelfRef.top__DOT__vgc_inst__DOT__gfx_color;
                            if (VL_LIKELY(((0xf9ffU 
                                            >= (0x0000ffffU 
                                                & (((IData)(0x00000140U) 
                                                    * (IData)(vlSelfRef.top__DOT__vgc_inst__DOT__cmd_cy)) 
                                                   + (IData)(vlSelfRef.top__DOT__vgc_inst__DOT__cmd_cx))))))) {
                                __VdlyVal__top__DOT__vgc_inst__DOT__gfx_ram__v5 
                                    = vlSelfRef.top__DOT__vgc_inst__DOT____Vlvbound_h3f27e46b__0;
                                __VdlyDim0__top__DOT__vgc_inst__DOT__gfx_ram__v5 
                                    = (0x0000ffffU 
                                       & (((IData)(0x00000140U) 
                                           * (IData)(vlSelfRef.top__DOT__vgc_inst__DOT__cmd_cy)) 
                                          + (IData)(vlSelfRef.top__DOT__vgc_inst__DOT__cmd_cx)));
                                vlSelfRef.__VdlyCommitQueuetop__DOT__vgc_inst__DOT__gfx_ram.enqueue(__VdlyVal__top__DOT__vgc_inst__DOT__gfx_ram__v5, (IData)(__VdlyDim0__top__DOT__vgc_inst__DOT__gfx_ram__v5));
                            }
                        }
                    }
                    if (((IData)(vlSelfRef.top__DOT__vgc_inst__DOT__cmd_cx) 
                         >= (0x000001ffU & (IData)(vlSelfRef.top__DOT__vgc_inst__DOT__cmd_x2)))) {
                        __Vdly__top__DOT__vgc_inst__DOT__cmd_cx 
                            = (0x000001ffU & (IData)(vlSelfRef.top__DOT__vgc_inst__DOT__cmd_x));
                        if (((IData)(vlSelfRef.top__DOT__vgc_inst__DOT__cmd_cy) 
                             >= (0x000000ffU & (IData)(vlSelfRef.top__DOT__vgc_inst__DOT__cmd_y2)))) {
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
                } else if (VL_LTES_III(10, (IData)(vlSelfRef.top__DOT__vgc_inst__DOT__cmd_dx), (IData)(vlSelfRef.top__DOT__vgc_inst__DOT__cmd_dy))) {
                    if ((((VL_LTES_III(32, 0U, (VL_EXTENDS_II(32,10, (IData)(vlSelfRef.top__DOT__vgc_inst__DOT__cmd_x)) 
                                                + VL_EXTENDS_II(32,10, (IData)(vlSelfRef.top__DOT__vgc_inst__DOT__cmd_dx)))) 
                           & VL_GTS_III(32, 0x00000140U, 
                                        (VL_EXTENDS_II(32,10, (IData)(vlSelfRef.top__DOT__vgc_inst__DOT__cmd_x)) 
                                         + VL_EXTENDS_II(32,10, (IData)(vlSelfRef.top__DOT__vgc_inst__DOT__cmd_dx))))) 
                          & VL_LTES_III(32, 0U, (VL_EXTENDS_II(32,10, (IData)(vlSelfRef.top__DOT__vgc_inst__DOT__cmd_y)) 
                                                 + 
                                                 VL_EXTENDS_II(32,10, (IData)(vlSelfRef.top__DOT__vgc_inst__DOT__cmd_dy))))) 
                         & VL_GTS_III(32, 0x000000c8U, 
                                      (VL_EXTENDS_II(32,10, (IData)(vlSelfRef.top__DOT__vgc_inst__DOT__cmd_y)) 
                                       + VL_EXTENDS_II(32,10, (IData)(vlSelfRef.top__DOT__vgc_inst__DOT__cmd_dy)))))) {
                        vlSelfRef.top__DOT__vgc_inst__DOT____Vlvbound_hafb21497__0 
                            = vlSelfRef.top__DOT__vgc_inst__DOT__gfx_color;
                        if (VL_LIKELY(((0xf9ffU >= 
                                        (0x0000ffffU 
                                         & (VL_MULS_III(32, (IData)(0x00000140U), 
                                                        (VL_EXTENDS_II(32,10, (IData)(vlSelfRef.top__DOT__vgc_inst__DOT__cmd_y)) 
                                                         + 
                                                         VL_EXTENDS_II(32,10, (IData)(vlSelfRef.top__DOT__vgc_inst__DOT__cmd_dy)))) 
                                            + (VL_EXTENDS_II(16,10, (IData)(vlSelfRef.top__DOT__vgc_inst__DOT__cmd_x)) 
                                               + VL_EXTENDS_II(16,10, (IData)(vlSelfRef.top__DOT__vgc_inst__DOT__cmd_dx))))))))) {
                            __VdlyVal__top__DOT__vgc_inst__DOT__gfx_ram__v6 
                                = vlSelfRef.top__DOT__vgc_inst__DOT____Vlvbound_hafb21497__0;
                            __VdlyDim0__top__DOT__vgc_inst__DOT__gfx_ram__v6 
                                = (0x0000ffffU & (VL_MULS_III(32, (IData)(0x00000140U), 
                                                              (VL_EXTENDS_II(32,10, (IData)(vlSelfRef.top__DOT__vgc_inst__DOT__cmd_y)) 
                                                               + 
                                                               VL_EXTENDS_II(32,10, (IData)(vlSelfRef.top__DOT__vgc_inst__DOT__cmd_dy)))) 
                                                  + 
                                                  (VL_EXTENDS_II(16,10, (IData)(vlSelfRef.top__DOT__vgc_inst__DOT__cmd_x)) 
                                                   + 
                                                   VL_EXTENDS_II(16,10, (IData)(vlSelfRef.top__DOT__vgc_inst__DOT__cmd_dx)))));
                            vlSelfRef.__VdlyCommitQueuetop__DOT__vgc_inst__DOT__gfx_ram.enqueue(__VdlyVal__top__DOT__vgc_inst__DOT__gfx_ram__v6, (IData)(__VdlyDim0__top__DOT__vgc_inst__DOT__gfx_ram__v6));
                        }
                    }
                    if ((((VL_LTES_III(32, 0U, (VL_EXTENDS_II(32,10, (IData)(vlSelfRef.top__DOT__vgc_inst__DOT__cmd_x)) 
                                                - VL_EXTENDS_II(32,10, (IData)(vlSelfRef.top__DOT__vgc_inst__DOT__cmd_dx)))) 
                           & VL_GTS_III(32, 0x00000140U, 
                                        (VL_EXTENDS_II(32,10, (IData)(vlSelfRef.top__DOT__vgc_inst__DOT__cmd_x)) 
                                         - VL_EXTENDS_II(32,10, (IData)(vlSelfRef.top__DOT__vgc_inst__DOT__cmd_dx))))) 
                          & VL_LTES_III(32, 0U, (VL_EXTENDS_II(32,10, (IData)(vlSelfRef.top__DOT__vgc_inst__DOT__cmd_y)) 
                                                 + 
                                                 VL_EXTENDS_II(32,10, (IData)(vlSelfRef.top__DOT__vgc_inst__DOT__cmd_dy))))) 
                         & VL_GTS_III(32, 0x000000c8U, 
                                      (VL_EXTENDS_II(32,10, (IData)(vlSelfRef.top__DOT__vgc_inst__DOT__cmd_y)) 
                                       + VL_EXTENDS_II(32,10, (IData)(vlSelfRef.top__DOT__vgc_inst__DOT__cmd_dy)))))) {
                        vlSelfRef.top__DOT__vgc_inst__DOT____Vlvbound_haf1931bd__0 
                            = vlSelfRef.top__DOT__vgc_inst__DOT__gfx_color;
                        if (VL_LIKELY(((0xf9ffU >= 
                                        (0x0000ffffU 
                                         & (VL_MULS_III(32, (IData)(0x00000140U), 
                                                        (VL_EXTENDS_II(32,10, (IData)(vlSelfRef.top__DOT__vgc_inst__DOT__cmd_y)) 
                                                         + 
                                                         VL_EXTENDS_II(32,10, (IData)(vlSelfRef.top__DOT__vgc_inst__DOT__cmd_dy)))) 
                                            + (VL_EXTENDS_II(16,10, (IData)(vlSelfRef.top__DOT__vgc_inst__DOT__cmd_x)) 
                                               - VL_EXTENDS_II(16,10, (IData)(vlSelfRef.top__DOT__vgc_inst__DOT__cmd_dx))))))))) {
                            __VdlyVal__top__DOT__vgc_inst__DOT__gfx_ram__v7 
                                = vlSelfRef.top__DOT__vgc_inst__DOT____Vlvbound_haf1931bd__0;
                            __VdlyDim0__top__DOT__vgc_inst__DOT__gfx_ram__v7 
                                = (0x0000ffffU & (VL_MULS_III(32, (IData)(0x00000140U), 
                                                              (VL_EXTENDS_II(32,10, (IData)(vlSelfRef.top__DOT__vgc_inst__DOT__cmd_y)) 
                                                               + 
                                                               VL_EXTENDS_II(32,10, (IData)(vlSelfRef.top__DOT__vgc_inst__DOT__cmd_dy)))) 
                                                  + 
                                                  (VL_EXTENDS_II(16,10, (IData)(vlSelfRef.top__DOT__vgc_inst__DOT__cmd_x)) 
                                                   - 
                                                   VL_EXTENDS_II(16,10, (IData)(vlSelfRef.top__DOT__vgc_inst__DOT__cmd_dx)))));
                            vlSelfRef.__VdlyCommitQueuetop__DOT__vgc_inst__DOT__gfx_ram.enqueue(__VdlyVal__top__DOT__vgc_inst__DOT__gfx_ram__v7, (IData)(__VdlyDim0__top__DOT__vgc_inst__DOT__gfx_ram__v7));
                        }
                    }
                    if ((((VL_LTES_III(32, 0U, (VL_EXTENDS_II(32,10, (IData)(vlSelfRef.top__DOT__vgc_inst__DOT__cmd_x)) 
                                                + VL_EXTENDS_II(32,10, (IData)(vlSelfRef.top__DOT__vgc_inst__DOT__cmd_dx)))) 
                           & VL_GTS_III(32, 0x00000140U, 
                                        (VL_EXTENDS_II(32,10, (IData)(vlSelfRef.top__DOT__vgc_inst__DOT__cmd_x)) 
                                         + VL_EXTENDS_II(32,10, (IData)(vlSelfRef.top__DOT__vgc_inst__DOT__cmd_dx))))) 
                          & VL_LTES_III(32, 0U, (VL_EXTENDS_II(32,10, (IData)(vlSelfRef.top__DOT__vgc_inst__DOT__cmd_y)) 
                                                 - 
                                                 VL_EXTENDS_II(32,10, (IData)(vlSelfRef.top__DOT__vgc_inst__DOT__cmd_dy))))) 
                         & VL_GTS_III(32, 0x000000c8U, 
                                      (VL_EXTENDS_II(32,10, (IData)(vlSelfRef.top__DOT__vgc_inst__DOT__cmd_y)) 
                                       - VL_EXTENDS_II(32,10, (IData)(vlSelfRef.top__DOT__vgc_inst__DOT__cmd_dy)))))) {
                        vlSelfRef.top__DOT__vgc_inst__DOT____Vlvbound_hc84a7865__0 
                            = vlSelfRef.top__DOT__vgc_inst__DOT__gfx_color;
                        if (VL_LIKELY(((0xf9ffU >= 
                                        (0x0000ffffU 
                                         & (VL_MULS_III(32, (IData)(0x00000140U), 
                                                        (VL_EXTENDS_II(32,10, (IData)(vlSelfRef.top__DOT__vgc_inst__DOT__cmd_y)) 
                                                         - 
                                                         VL_EXTENDS_II(32,10, (IData)(vlSelfRef.top__DOT__vgc_inst__DOT__cmd_dy)))) 
                                            + (VL_EXTENDS_II(16,10, (IData)(vlSelfRef.top__DOT__vgc_inst__DOT__cmd_x)) 
                                               + VL_EXTENDS_II(16,10, (IData)(vlSelfRef.top__DOT__vgc_inst__DOT__cmd_dx))))))))) {
                            __VdlyVal__top__DOT__vgc_inst__DOT__gfx_ram__v8 
                                = vlSelfRef.top__DOT__vgc_inst__DOT____Vlvbound_hc84a7865__0;
                            __VdlyDim0__top__DOT__vgc_inst__DOT__gfx_ram__v8 
                                = (0x0000ffffU & (VL_MULS_III(32, (IData)(0x00000140U), 
                                                              (VL_EXTENDS_II(32,10, (IData)(vlSelfRef.top__DOT__vgc_inst__DOT__cmd_y)) 
                                                               - 
                                                               VL_EXTENDS_II(32,10, (IData)(vlSelfRef.top__DOT__vgc_inst__DOT__cmd_dy)))) 
                                                  + 
                                                  (VL_EXTENDS_II(16,10, (IData)(vlSelfRef.top__DOT__vgc_inst__DOT__cmd_x)) 
                                                   + 
                                                   VL_EXTENDS_II(16,10, (IData)(vlSelfRef.top__DOT__vgc_inst__DOT__cmd_dx)))));
                            vlSelfRef.__VdlyCommitQueuetop__DOT__vgc_inst__DOT__gfx_ram.enqueue(__VdlyVal__top__DOT__vgc_inst__DOT__gfx_ram__v8, (IData)(__VdlyDim0__top__DOT__vgc_inst__DOT__gfx_ram__v8));
                        }
                    }
                    if ((((VL_LTES_III(32, 0U, (VL_EXTENDS_II(32,10, (IData)(vlSelfRef.top__DOT__vgc_inst__DOT__cmd_x)) 
                                                - VL_EXTENDS_II(32,10, (IData)(vlSelfRef.top__DOT__vgc_inst__DOT__cmd_dx)))) 
                           & VL_GTS_III(32, 0x00000140U, 
                                        (VL_EXTENDS_II(32,10, (IData)(vlSelfRef.top__DOT__vgc_inst__DOT__cmd_x)) 
                                         - VL_EXTENDS_II(32,10, (IData)(vlSelfRef.top__DOT__vgc_inst__DOT__cmd_dx))))) 
                          & VL_LTES_III(32, 0U, (VL_EXTENDS_II(32,10, (IData)(vlSelfRef.top__DOT__vgc_inst__DOT__cmd_y)) 
                                                 - 
                                                 VL_EXTENDS_II(32,10, (IData)(vlSelfRef.top__DOT__vgc_inst__DOT__cmd_dy))))) 
                         & VL_GTS_III(32, 0x000000c8U, 
                                      (VL_EXTENDS_II(32,10, (IData)(vlSelfRef.top__DOT__vgc_inst__DOT__cmd_y)) 
                                       - VL_EXTENDS_II(32,10, (IData)(vlSelfRef.top__DOT__vgc_inst__DOT__cmd_dy)))))) {
                        vlSelfRef.top__DOT__vgc_inst__DOT____Vlvbound_hed509973__0 
                            = vlSelfRef.top__DOT__vgc_inst__DOT__gfx_color;
                        if (VL_LIKELY(((0xf9ffU >= 
                                        (0x0000ffffU 
                                         & (VL_MULS_III(32, (IData)(0x00000140U), 
                                                        (VL_EXTENDS_II(32,10, (IData)(vlSelfRef.top__DOT__vgc_inst__DOT__cmd_y)) 
                                                         - 
                                                         VL_EXTENDS_II(32,10, (IData)(vlSelfRef.top__DOT__vgc_inst__DOT__cmd_dy)))) 
                                            + (VL_EXTENDS_II(16,10, (IData)(vlSelfRef.top__DOT__vgc_inst__DOT__cmd_x)) 
                                               - VL_EXTENDS_II(16,10, (IData)(vlSelfRef.top__DOT__vgc_inst__DOT__cmd_dx))))))))) {
                            __VdlyVal__top__DOT__vgc_inst__DOT__gfx_ram__v9 
                                = vlSelfRef.top__DOT__vgc_inst__DOT____Vlvbound_hed509973__0;
                            __VdlyDim0__top__DOT__vgc_inst__DOT__gfx_ram__v9 
                                = (0x0000ffffU & (VL_MULS_III(32, (IData)(0x00000140U), 
                                                              (VL_EXTENDS_II(32,10, (IData)(vlSelfRef.top__DOT__vgc_inst__DOT__cmd_y)) 
                                                               - 
                                                               VL_EXTENDS_II(32,10, (IData)(vlSelfRef.top__DOT__vgc_inst__DOT__cmd_dy)))) 
                                                  + 
                                                  (VL_EXTENDS_II(16,10, (IData)(vlSelfRef.top__DOT__vgc_inst__DOT__cmd_x)) 
                                                   - 
                                                   VL_EXTENDS_II(16,10, (IData)(vlSelfRef.top__DOT__vgc_inst__DOT__cmd_dx)))));
                            vlSelfRef.__VdlyCommitQueuetop__DOT__vgc_inst__DOT__gfx_ram.enqueue(__VdlyVal__top__DOT__vgc_inst__DOT__gfx_ram__v9, (IData)(__VdlyDim0__top__DOT__vgc_inst__DOT__gfx_ram__v9));
                        }
                    }
                    if ((((VL_LTES_III(32, 0U, (VL_EXTENDS_II(32,10, (IData)(vlSelfRef.top__DOT__vgc_inst__DOT__cmd_x)) 
                                                + VL_EXTENDS_II(32,10, (IData)(vlSelfRef.top__DOT__vgc_inst__DOT__cmd_dy)))) 
                           & VL_GTS_III(32, 0x00000140U, 
                                        (VL_EXTENDS_II(32,10, (IData)(vlSelfRef.top__DOT__vgc_inst__DOT__cmd_x)) 
                                         + VL_EXTENDS_II(32,10, (IData)(vlSelfRef.top__DOT__vgc_inst__DOT__cmd_dy))))) 
                          & VL_LTES_III(32, 0U, (VL_EXTENDS_II(32,10, (IData)(vlSelfRef.top__DOT__vgc_inst__DOT__cmd_y)) 
                                                 + 
                                                 VL_EXTENDS_II(32,10, (IData)(vlSelfRef.top__DOT__vgc_inst__DOT__cmd_dx))))) 
                         & VL_GTS_III(32, 0x000000c8U, 
                                      (VL_EXTENDS_II(32,10, (IData)(vlSelfRef.top__DOT__vgc_inst__DOT__cmd_y)) 
                                       + VL_EXTENDS_II(32,10, (IData)(vlSelfRef.top__DOT__vgc_inst__DOT__cmd_dx)))))) {
                        vlSelfRef.top__DOT__vgc_inst__DOT____Vlvbound_hd5133de6__0 
                            = vlSelfRef.top__DOT__vgc_inst__DOT__gfx_color;
                        if (VL_LIKELY(((0xf9ffU >= 
                                        (0x0000ffffU 
                                         & (VL_MULS_III(32, (IData)(0x00000140U), 
                                                        (VL_EXTENDS_II(32,10, (IData)(vlSelfRef.top__DOT__vgc_inst__DOT__cmd_y)) 
                                                         + 
                                                         VL_EXTENDS_II(32,10, (IData)(vlSelfRef.top__DOT__vgc_inst__DOT__cmd_dx)))) 
                                            + (VL_EXTENDS_II(16,10, (IData)(vlSelfRef.top__DOT__vgc_inst__DOT__cmd_x)) 
                                               + VL_EXTENDS_II(16,10, (IData)(vlSelfRef.top__DOT__vgc_inst__DOT__cmd_dy))))))))) {
                            __VdlyVal__top__DOT__vgc_inst__DOT__gfx_ram__v10 
                                = vlSelfRef.top__DOT__vgc_inst__DOT____Vlvbound_hd5133de6__0;
                            __VdlyDim0__top__DOT__vgc_inst__DOT__gfx_ram__v10 
                                = (0x0000ffffU & (VL_MULS_III(32, (IData)(0x00000140U), 
                                                              (VL_EXTENDS_II(32,10, (IData)(vlSelfRef.top__DOT__vgc_inst__DOT__cmd_y)) 
                                                               + 
                                                               VL_EXTENDS_II(32,10, (IData)(vlSelfRef.top__DOT__vgc_inst__DOT__cmd_dx)))) 
                                                  + 
                                                  (VL_EXTENDS_II(16,10, (IData)(vlSelfRef.top__DOT__vgc_inst__DOT__cmd_x)) 
                                                   + 
                                                   VL_EXTENDS_II(16,10, (IData)(vlSelfRef.top__DOT__vgc_inst__DOT__cmd_dy)))));
                            vlSelfRef.__VdlyCommitQueuetop__DOT__vgc_inst__DOT__gfx_ram.enqueue(__VdlyVal__top__DOT__vgc_inst__DOT__gfx_ram__v10, (IData)(__VdlyDim0__top__DOT__vgc_inst__DOT__gfx_ram__v10));
                        }
                    }
                    if ((((VL_LTES_III(32, 0U, (VL_EXTENDS_II(32,10, (IData)(vlSelfRef.top__DOT__vgc_inst__DOT__cmd_x)) 
                                                - VL_EXTENDS_II(32,10, (IData)(vlSelfRef.top__DOT__vgc_inst__DOT__cmd_dy)))) 
                           & VL_GTS_III(32, 0x00000140U, 
                                        (VL_EXTENDS_II(32,10, (IData)(vlSelfRef.top__DOT__vgc_inst__DOT__cmd_x)) 
                                         - VL_EXTENDS_II(32,10, (IData)(vlSelfRef.top__DOT__vgc_inst__DOT__cmd_dy))))) 
                          & VL_LTES_III(32, 0U, (VL_EXTENDS_II(32,10, (IData)(vlSelfRef.top__DOT__vgc_inst__DOT__cmd_y)) 
                                                 + 
                                                 VL_EXTENDS_II(32,10, (IData)(vlSelfRef.top__DOT__vgc_inst__DOT__cmd_dx))))) 
                         & VL_GTS_III(32, 0x000000c8U, 
                                      (VL_EXTENDS_II(32,10, (IData)(vlSelfRef.top__DOT__vgc_inst__DOT__cmd_y)) 
                                       + VL_EXTENDS_II(32,10, (IData)(vlSelfRef.top__DOT__vgc_inst__DOT__cmd_dx)))))) {
                        vlSelfRef.top__DOT__vgc_inst__DOT____Vlvbound_hd5fa5e14__0 
                            = vlSelfRef.top__DOT__vgc_inst__DOT__gfx_color;
                        if (VL_LIKELY(((0xf9ffU >= 
                                        (0x0000ffffU 
                                         & (VL_MULS_III(32, (IData)(0x00000140U), 
                                                        (VL_EXTENDS_II(32,10, (IData)(vlSelfRef.top__DOT__vgc_inst__DOT__cmd_y)) 
                                                         + 
                                                         VL_EXTENDS_II(32,10, (IData)(vlSelfRef.top__DOT__vgc_inst__DOT__cmd_dx)))) 
                                            + (VL_EXTENDS_II(16,10, (IData)(vlSelfRef.top__DOT__vgc_inst__DOT__cmd_x)) 
                                               - VL_EXTENDS_II(16,10, (IData)(vlSelfRef.top__DOT__vgc_inst__DOT__cmd_dy))))))))) {
                            __VdlyVal__top__DOT__vgc_inst__DOT__gfx_ram__v11 
                                = vlSelfRef.top__DOT__vgc_inst__DOT____Vlvbound_hd5fa5e14__0;
                            __VdlyDim0__top__DOT__vgc_inst__DOT__gfx_ram__v11 
                                = (0x0000ffffU & (VL_MULS_III(32, (IData)(0x00000140U), 
                                                              (VL_EXTENDS_II(32,10, (IData)(vlSelfRef.top__DOT__vgc_inst__DOT__cmd_y)) 
                                                               + 
                                                               VL_EXTENDS_II(32,10, (IData)(vlSelfRef.top__DOT__vgc_inst__DOT__cmd_dx)))) 
                                                  + 
                                                  (VL_EXTENDS_II(16,10, (IData)(vlSelfRef.top__DOT__vgc_inst__DOT__cmd_x)) 
                                                   - 
                                                   VL_EXTENDS_II(16,10, (IData)(vlSelfRef.top__DOT__vgc_inst__DOT__cmd_dy)))));
                            vlSelfRef.__VdlyCommitQueuetop__DOT__vgc_inst__DOT__gfx_ram.enqueue(__VdlyVal__top__DOT__vgc_inst__DOT__gfx_ram__v11, (IData)(__VdlyDim0__top__DOT__vgc_inst__DOT__gfx_ram__v11));
                        }
                    }
                    if ((((VL_LTES_III(32, 0U, (VL_EXTENDS_II(32,10, (IData)(vlSelfRef.top__DOT__vgc_inst__DOT__cmd_x)) 
                                                + VL_EXTENDS_II(32,10, (IData)(vlSelfRef.top__DOT__vgc_inst__DOT__cmd_dy)))) 
                           & VL_GTS_III(32, 0x00000140U, 
                                        (VL_EXTENDS_II(32,10, (IData)(vlSelfRef.top__DOT__vgc_inst__DOT__cmd_x)) 
                                         + VL_EXTENDS_II(32,10, (IData)(vlSelfRef.top__DOT__vgc_inst__DOT__cmd_dy))))) 
                          & VL_LTES_III(32, 0U, (VL_EXTENDS_II(32,10, (IData)(vlSelfRef.top__DOT__vgc_inst__DOT__cmd_y)) 
                                                 - 
                                                 VL_EXTENDS_II(32,10, (IData)(vlSelfRef.top__DOT__vgc_inst__DOT__cmd_dx))))) 
                         & VL_GTS_III(32, 0x000000c8U, 
                                      (VL_EXTENDS_II(32,10, (IData)(vlSelfRef.top__DOT__vgc_inst__DOT__cmd_y)) 
                                       - VL_EXTENDS_II(32,10, (IData)(vlSelfRef.top__DOT__vgc_inst__DOT__cmd_dx)))))) {
                        vlSelfRef.top__DOT__vgc_inst__DOT____Vlvbound_haa426699__0 
                            = vlSelfRef.top__DOT__vgc_inst__DOT__gfx_color;
                        if (VL_LIKELY(((0xf9ffU >= 
                                        (0x0000ffffU 
                                         & (VL_MULS_III(32, (IData)(0x00000140U), 
                                                        (VL_EXTENDS_II(32,10, (IData)(vlSelfRef.top__DOT__vgc_inst__DOT__cmd_y)) 
                                                         - 
                                                         VL_EXTENDS_II(32,10, (IData)(vlSelfRef.top__DOT__vgc_inst__DOT__cmd_dx)))) 
                                            + (VL_EXTENDS_II(16,10, (IData)(vlSelfRef.top__DOT__vgc_inst__DOT__cmd_x)) 
                                               + VL_EXTENDS_II(16,10, (IData)(vlSelfRef.top__DOT__vgc_inst__DOT__cmd_dy))))))))) {
                            __VdlyVal__top__DOT__vgc_inst__DOT__gfx_ram__v12 
                                = vlSelfRef.top__DOT__vgc_inst__DOT____Vlvbound_haa426699__0;
                            __VdlyDim0__top__DOT__vgc_inst__DOT__gfx_ram__v12 
                                = (0x0000ffffU & (VL_MULS_III(32, (IData)(0x00000140U), 
                                                              (VL_EXTENDS_II(32,10, (IData)(vlSelfRef.top__DOT__vgc_inst__DOT__cmd_y)) 
                                                               - 
                                                               VL_EXTENDS_II(32,10, (IData)(vlSelfRef.top__DOT__vgc_inst__DOT__cmd_dx)))) 
                                                  + 
                                                  (VL_EXTENDS_II(16,10, (IData)(vlSelfRef.top__DOT__vgc_inst__DOT__cmd_x)) 
                                                   + 
                                                   VL_EXTENDS_II(16,10, (IData)(vlSelfRef.top__DOT__vgc_inst__DOT__cmd_dy)))));
                            vlSelfRef.__VdlyCommitQueuetop__DOT__vgc_inst__DOT__gfx_ram.enqueue(__VdlyVal__top__DOT__vgc_inst__DOT__gfx_ram__v12, (IData)(__VdlyDim0__top__DOT__vgc_inst__DOT__gfx_ram__v12));
                        }
                    }
                    if ((((VL_LTES_III(32, 0U, (VL_EXTENDS_II(32,10, (IData)(vlSelfRef.top__DOT__vgc_inst__DOT__cmd_x)) 
                                                - VL_EXTENDS_II(32,10, (IData)(vlSelfRef.top__DOT__vgc_inst__DOT__cmd_dy)))) 
                           & VL_GTS_III(32, 0x00000140U, 
                                        (VL_EXTENDS_II(32,10, (IData)(vlSelfRef.top__DOT__vgc_inst__DOT__cmd_x)) 
                                         - VL_EXTENDS_II(32,10, (IData)(vlSelfRef.top__DOT__vgc_inst__DOT__cmd_dy))))) 
                          & VL_LTES_III(32, 0U, (VL_EXTENDS_II(32,10, (IData)(vlSelfRef.top__DOT__vgc_inst__DOT__cmd_y)) 
                                                 - 
                                                 VL_EXTENDS_II(32,10, (IData)(vlSelfRef.top__DOT__vgc_inst__DOT__cmd_dx))))) 
                         & VL_GTS_III(32, 0x000000c8U, 
                                      (VL_EXTENDS_II(32,10, (IData)(vlSelfRef.top__DOT__vgc_inst__DOT__cmd_y)) 
                                       - VL_EXTENDS_II(32,10, (IData)(vlSelfRef.top__DOT__vgc_inst__DOT__cmd_dx)))))) {
                        vlSelfRef.top__DOT__vgc_inst__DOT____Vlvbound_h9f4406d7__0 
                            = vlSelfRef.top__DOT__vgc_inst__DOT__gfx_color;
                        if (VL_LIKELY(((0xf9ffU >= 
                                        (0x0000ffffU 
                                         & (VL_MULS_III(32, (IData)(0x00000140U), 
                                                        (VL_EXTENDS_II(32,10, (IData)(vlSelfRef.top__DOT__vgc_inst__DOT__cmd_y)) 
                                                         - 
                                                         VL_EXTENDS_II(32,10, (IData)(vlSelfRef.top__DOT__vgc_inst__DOT__cmd_dx)))) 
                                            + (VL_EXTENDS_II(16,10, (IData)(vlSelfRef.top__DOT__vgc_inst__DOT__cmd_x)) 
                                               - VL_EXTENDS_II(16,10, (IData)(vlSelfRef.top__DOT__vgc_inst__DOT__cmd_dy))))))))) {
                            __VdlyVal__top__DOT__vgc_inst__DOT__gfx_ram__v13 
                                = vlSelfRef.top__DOT__vgc_inst__DOT____Vlvbound_h9f4406d7__0;
                            __VdlyDim0__top__DOT__vgc_inst__DOT__gfx_ram__v13 
                                = (0x0000ffffU & (VL_MULS_III(32, (IData)(0x00000140U), 
                                                              (VL_EXTENDS_II(32,10, (IData)(vlSelfRef.top__DOT__vgc_inst__DOT__cmd_y)) 
                                                               - 
                                                               VL_EXTENDS_II(32,10, (IData)(vlSelfRef.top__DOT__vgc_inst__DOT__cmd_dx)))) 
                                                  + 
                                                  (VL_EXTENDS_II(16,10, (IData)(vlSelfRef.top__DOT__vgc_inst__DOT__cmd_x)) 
                                                   - 
                                                   VL_EXTENDS_II(16,10, (IData)(vlSelfRef.top__DOT__vgc_inst__DOT__cmd_dy)))));
                            vlSelfRef.__VdlyCommitQueuetop__DOT__vgc_inst__DOT__gfx_ram.enqueue(__VdlyVal__top__DOT__vgc_inst__DOT__gfx_ram__v13, (IData)(__VdlyDim0__top__DOT__vgc_inst__DOT__gfx_ram__v13));
                        }
                    }
                    __Vdly__top__DOT__vgc_inst__DOT__cmd_dx 
                        = (0x000003ffU & ((IData)(1U) 
                                          + VL_EXTENDS_II(10,10, (IData)(vlSelfRef.top__DOT__vgc_inst__DOT__cmd_dx))));
                    if (VL_GTS_III(32, 0U, VL_EXTENDS_II(32,10, (IData)(vlSelfRef.top__DOT__vgc_inst__DOT__cmd_err)))) {
                        __Vdly__top__DOT__vgc_inst__DOT__cmd_err 
                            = (0x000003ffU & ((IData)(3U) 
                                              + (VL_EXTENDS_II(10,10, (IData)(vlSelfRef.top__DOT__vgc_inst__DOT__cmd_err)) 
                                                 + 
                                                 VL_MULS_III(32, (IData)(2U), 
                                                             VL_EXTENDS_II(32,10, (IData)(vlSelfRef.top__DOT__vgc_inst__DOT__cmd_dx))))));
                    } else {
                        __Vdly__top__DOT__vgc_inst__DOT__cmd_err 
                            = (0x000003ffU & ((IData)(5U) 
                                              + (VL_EXTENDS_II(10,10, (IData)(vlSelfRef.top__DOT__vgc_inst__DOT__cmd_err)) 
                                                 + 
                                                 VL_MULS_III(32, (IData)(2U), 
                                                             (VL_EXTENDS_II(32,10, (IData)(vlSelfRef.top__DOT__vgc_inst__DOT__cmd_dx)) 
                                                              - 
                                                              VL_EXTENDS_II(32,10, (IData)(vlSelfRef.top__DOT__vgc_inst__DOT__cmd_dy)))))));
                        __Vdly__top__DOT__vgc_inst__DOT__cmd_dy 
                            = (0x000003ffU & (VL_EXTENDS_II(10,10, (IData)(vlSelfRef.top__DOT__vgc_inst__DOT__cmd_dy)) 
                                              - (IData)(1U)));
                    }
                } else {
                    __Vdly__top__DOT__vgc_inst__DOT__cmd_busy = 0U;
                }
            } else if ((2U & (IData)(vlSelfRef.top__DOT__vgc_inst__DOT__cmd_op))) {
                if ((1U & (IData)(vlSelfRef.top__DOT__vgc_inst__DOT__cmd_op))) {
                    if ((((VL_LTES_III(32, 0U, VL_EXTENDS_II(32,10, (IData)(vlSelfRef.top__DOT__vgc_inst__DOT__cmd_x))) 
                           & VL_GTS_III(32, 0x00000140U, 
                                        VL_EXTENDS_II(32,10, (IData)(vlSelfRef.top__DOT__vgc_inst__DOT__cmd_x)))) 
                          & VL_LTES_III(32, 0U, VL_EXTENDS_II(32,10, (IData)(vlSelfRef.top__DOT__vgc_inst__DOT__cmd_y)))) 
                         & VL_GTS_III(32, 0x000000c8U, 
                                      VL_EXTENDS_II(32,10, (IData)(vlSelfRef.top__DOT__vgc_inst__DOT__cmd_y))))) {
                        vlSelfRef.top__DOT__vgc_inst__DOT____Vlvbound_hc0a966b6__0 
                            = vlSelfRef.top__DOT__vgc_inst__DOT__gfx_color;
                        if (VL_LIKELY(((0xf9ffU >= 
                                        (0x0000ffffU 
                                         & (VL_MULS_III(32, (IData)(0x00000140U), 
                                                        VL_EXTENDS_II(32,10, (IData)(vlSelfRef.top__DOT__vgc_inst__DOT__cmd_y))) 
                                            + VL_EXTENDS_II(16,10, (IData)(vlSelfRef.top__DOT__vgc_inst__DOT__cmd_x)))))))) {
                            __VdlyVal__top__DOT__vgc_inst__DOT__gfx_ram__v14 
                                = vlSelfRef.top__DOT__vgc_inst__DOT____Vlvbound_hc0a966b6__0;
                            __VdlyDim0__top__DOT__vgc_inst__DOT__gfx_ram__v14 
                                = (0x0000ffffU & (VL_MULS_III(32, (IData)(0x00000140U), 
                                                              VL_EXTENDS_II(32,10, (IData)(vlSelfRef.top__DOT__vgc_inst__DOT__cmd_y))) 
                                                  + 
                                                  VL_EXTENDS_II(16,10, (IData)(vlSelfRef.top__DOT__vgc_inst__DOT__cmd_x))));
                            vlSelfRef.__VdlyCommitQueuetop__DOT__vgc_inst__DOT__gfx_ram.enqueue(__VdlyVal__top__DOT__vgc_inst__DOT__gfx_ram__v14, (IData)(__VdlyDim0__top__DOT__vgc_inst__DOT__gfx_ram__v14));
                        }
                    }
                    if ((((((((IData)(vlSelfRef.top__DOT__vgc_inst__DOT__cmd_x) 
                              == (IData)(vlSelfRef.top__DOT__vgc_inst__DOT__cmd_x2)) 
                             & ((IData)(vlSelfRef.top__DOT__vgc_inst__DOT__cmd_y) 
                                == (IData)(vlSelfRef.top__DOT__vgc_inst__DOT__cmd_y2))) 
                            | VL_GTS_III(32, 0xfffffff0U, 
                                         VL_EXTENDS_II(32,10, (IData)(vlSelfRef.top__DOT__vgc_inst__DOT__cmd_x)))) 
                           | VL_LTS_III(32, 0x00000150U, 
                                        VL_EXTENDS_II(32,10, (IData)(vlSelfRef.top__DOT__vgc_inst__DOT__cmd_x)))) 
                          | VL_GTS_III(32, 0xfffffff0U, 
                                       VL_EXTENDS_II(32,10, (IData)(vlSelfRef.top__DOT__vgc_inst__DOT__cmd_y)))) 
                         | VL_LTS_III(32, 0x000000d8U, 
                                      VL_EXTENDS_II(32,10, (IData)(vlSelfRef.top__DOT__vgc_inst__DOT__cmd_y))))) {
                        __Vdly__top__DOT__vgc_inst__DOT__cmd_busy = 0U;
                    } else if ((VL_GTES_III(32, VL_MULS_III(32, (IData)(2U), 
                                                            VL_EXTENDS_II(32,10, (IData)(vlSelfRef.top__DOT__vgc_inst__DOT__cmd_err))), 
                                            VL_EXTENDS_II(32,10, (IData)(vlSelfRef.top__DOT__vgc_inst__DOT__cmd_dy))) 
                                & VL_LTES_III(32, VL_MULS_III(32, (IData)(2U), 
                                                              VL_EXTENDS_II(32,10, (IData)(vlSelfRef.top__DOT__vgc_inst__DOT__cmd_err))), 
                                              VL_EXTENDS_II(32,10, (IData)(vlSelfRef.top__DOT__vgc_inst__DOT__cmd_dx))))) {
                        __Vdly__top__DOT__vgc_inst__DOT__cmd_err 
                            = (0x000003ffU & (((IData)(vlSelfRef.top__DOT__vgc_inst__DOT__cmd_err) 
                                               + (IData)(vlSelfRef.top__DOT__vgc_inst__DOT__cmd_dy)) 
                                              + (IData)(vlSelfRef.top__DOT__vgc_inst__DOT__cmd_dx)));
                        __Vdly__top__DOT__vgc_inst__DOT__cmd_x 
                            = (0x000003ffU & ((IData)(vlSelfRef.top__DOT__vgc_inst__DOT__cmd_x) 
                                              + (IData)(vlSelfRef.top__DOT__vgc_inst__DOT__cmd_sx)));
                        __Vdly__top__DOT__vgc_inst__DOT__cmd_y 
                            = (0x000003ffU & ((IData)(vlSelfRef.top__DOT__vgc_inst__DOT__cmd_y) 
                                              + (IData)(vlSelfRef.top__DOT__vgc_inst__DOT__cmd_sy)));
                    } else if (VL_GTES_III(32, VL_MULS_III(32, (IData)(2U), 
                                                           VL_EXTENDS_II(32,10, (IData)(vlSelfRef.top__DOT__vgc_inst__DOT__cmd_err))), 
                                           VL_EXTENDS_II(32,10, (IData)(vlSelfRef.top__DOT__vgc_inst__DOT__cmd_dy)))) {
                        __Vdly__top__DOT__vgc_inst__DOT__cmd_err 
                            = (0x000003ffU & ((IData)(vlSelfRef.top__DOT__vgc_inst__DOT__cmd_err) 
                                              + (IData)(vlSelfRef.top__DOT__vgc_inst__DOT__cmd_dy)));
                        __Vdly__top__DOT__vgc_inst__DOT__cmd_x 
                            = (0x000003ffU & ((IData)(vlSelfRef.top__DOT__vgc_inst__DOT__cmd_x) 
                                              + (IData)(vlSelfRef.top__DOT__vgc_inst__DOT__cmd_sx)));
                    } else if (VL_LTES_III(32, VL_MULS_III(32, (IData)(2U), 
                                                           VL_EXTENDS_II(32,10, (IData)(vlSelfRef.top__DOT__vgc_inst__DOT__cmd_err))), 
                                           VL_EXTENDS_II(32,10, (IData)(vlSelfRef.top__DOT__vgc_inst__DOT__cmd_dx)))) {
                        __Vdly__top__DOT__vgc_inst__DOT__cmd_err 
                            = (0x000003ffU & ((IData)(vlSelfRef.top__DOT__vgc_inst__DOT__cmd_err) 
                                              + (IData)(vlSelfRef.top__DOT__vgc_inst__DOT__cmd_dx)));
                        __Vdly__top__DOT__vgc_inst__DOT__cmd_y 
                            = (0x000003ffU & ((IData)(vlSelfRef.top__DOT__vgc_inst__DOT__cmd_y) 
                                              + (IData)(vlSelfRef.top__DOT__vgc_inst__DOT__cmd_sy)));
                    }
                } else {
                    __Vdly__top__DOT__vgc_inst__DOT__cmd_busy = 0U;
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
                    } else if ((5U != (0x0000001fU 
                                       & (IData)(vlSelfRef.top__DOT__cpu_addr)))) {
                        if ((6U != (0x0000001fU & (IData)(vlSelfRef.top__DOT__cpu_addr)))) {
                            vlSelfRef.top__DOT__vgc_inst__DOT__font_slot 
                                = (7U & (IData)(vlSelfRef.top__DOT__cpu_dout));
                        }
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
                        __Vdly__top__DOT__vgc_inst__DOT__gfx_color 
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
                                        }
                                        vlSelfRef.top__DOT__vgc_inst__DOT____Vlvbound_hb8130fd5__0 = 0x20U;
                                        if (VL_LIKELY((
                                                       (0x07cfU 
                                                        >= 
                                                        ([&]() {
                                                                __Vfunc_top__DOT__vgc_inst__DOT__screen_addr__9__row 
                                                                    = vlSelfRef.top__DOT__vgc_inst__DOT__cursor_y;
                                                                __Vfunc_top__DOT__vgc_inst__DOT__screen_addr__9__col 
                                                                    = 
                                                                    ((0U 
                                                                      < (IData)(vlSelfRef.top__DOT__vgc_inst__DOT__cursor_x))
                                                                      ? 
                                                                     (0x0000007fU 
                                                                      & ((IData)(vlSelfRef.top__DOT__vgc_inst__DOT__cursor_x) 
                                                                         - (IData)(1U)))
                                                                      : 0U);
                                                                __Vfunc_top__DOT__vgc_inst__DOT__screen_addr__9__rr 
                                                                    = 
                                                                    (0x0000001fU 
                                                                     & ((IData)(__Vfunc_top__DOT__vgc_inst__DOT__screen_addr__9__row) 
                                                                        + (IData)(vlSelfRef.top__DOT__vgc_inst__DOT__scroll_offset)));
                                                                if (
                                                                    (0x19U 
                                                                     <= (IData)(__Vfunc_top__DOT__vgc_inst__DOT__screen_addr__9__rr))) {
                                                                    __Vfunc_top__DOT__vgc_inst__DOT__screen_addr__9__rr 
                                                                        = 
                                                                        (0x0000001fU 
                                                                         & ((IData)(__Vfunc_top__DOT__vgc_inst__DOT__screen_addr__9__rr) 
                                                                            - (IData)(0x19U)));
                                                                }
                                                                __Vfunc_top__DOT__vgc_inst__DOT__screen_addr__9__Vfuncout 
                                                                    = 
                                                                    (0x000007ffU 
                                                                     & (((IData)(0x00000050U) 
                                                                         * (IData)(__Vfunc_top__DOT__vgc_inst__DOT__screen_addr__9__rr)) 
                                                                        + (IData)(__Vfunc_top__DOT__vgc_inst__DOT__screen_addr__9__col)));
                                                            }(), (IData)(__Vfunc_top__DOT__vgc_inst__DOT__screen_addr__9__Vfuncout)))))) {
                                            __Vfunc_top__DOT__vgc_inst__DOT__screen_addr__10__row 
                                                = vlSelfRef.top__DOT__vgc_inst__DOT__cursor_y;
                                            __Vfunc_top__DOT__vgc_inst__DOT__screen_addr__10__col 
                                                = (
                                                   (0U 
                                                    < (IData)(vlSelfRef.top__DOT__vgc_inst__DOT__cursor_x))
                                                    ? 
                                                   (0x0000007fU 
                                                    & ((IData)(vlSelfRef.top__DOT__vgc_inst__DOT__cursor_x) 
                                                       - (IData)(1U)))
                                                    : 0U);
                                            __Vfunc_top__DOT__vgc_inst__DOT__screen_addr__10__rr 
                                                = (0x0000001fU 
                                                   & ((IData)(__Vfunc_top__DOT__vgc_inst__DOT__screen_addr__10__row) 
                                                      + (IData)(vlSelfRef.top__DOT__vgc_inst__DOT__scroll_offset)));
                                            if ((0x19U 
                                                 <= (IData)(__Vfunc_top__DOT__vgc_inst__DOT__screen_addr__10__rr))) {
                                                __Vfunc_top__DOT__vgc_inst__DOT__screen_addr__10__rr 
                                                    = 
                                                    (0x0000001fU 
                                                     & ((IData)(__Vfunc_top__DOT__vgc_inst__DOT__screen_addr__10__rr) 
                                                        - (IData)(0x19U)));
                                            }
                                            __Vfunc_top__DOT__vgc_inst__DOT__screen_addr__10__Vfuncout 
                                                = (0x000007ffU 
                                                   & (((IData)(0x00000050U) 
                                                       * (IData)(__Vfunc_top__DOT__vgc_inst__DOT__screen_addr__10__rr)) 
                                                      + (IData)(__Vfunc_top__DOT__vgc_inst__DOT__screen_addr__10__col)));
                                            __VdlyVal__top__DOT__vgc_inst__DOT__char_ram__v3 
                                                = vlSelfRef.top__DOT__vgc_inst__DOT____Vlvbound_hb8130fd5__0;
                                            __VdlyDim0__top__DOT__vgc_inst__DOT__char_ram__v3 
                                                = __Vfunc_top__DOT__vgc_inst__DOT__screen_addr__10__Vfuncout;
                                            vlSelfRef.__VdlyCommitQueuetop__DOT__vgc_inst__DOT__char_ram.enqueue(__VdlyVal__top__DOT__vgc_inst__DOT__char_ram__v3, (IData)(__VdlyDim0__top__DOT__vgc_inst__DOT__char_ram__v3));
                                        }
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
                                        __Vdly__top__DOT__vgc_inst__DOT__cmd_cy = 0U;
                                        __Vdly__top__DOT__vgc_inst__DOT__cursor_x = 0U;
                                        __Vdly__top__DOT__vgc_inst__DOT__cursor_y = 0U;
                                        __Vdly__top__DOT__vgc_inst__DOT__scroll_offset = 0U;
                                        __Vdly__top__DOT__vgc_inst__DOT__cmd_cx = 0U;
                                        __Vdly__top__DOT__vgc_inst__DOT__cmd_busy = 1U;
                                        vlSelfRef.top__DOT__vgc_inst__DOT__cmd_op = 0xf0U;
                                    } else if ((0x0dU 
                                                == (IData)(vlSelfRef.top__DOT__cpu_dout))) {
                                        __Vdly__top__DOT__vgc_inst__DOT__cursor_x = 0U;
                                    } else if ((0x13U 
                                                == (IData)(vlSelfRef.top__DOT__cpu_dout))) {
                                        __Vdly__top__DOT__vgc_inst__DOT__cursor_x = 0U;
                                        __Vdly__top__DOT__vgc_inst__DOT__cursor_y = 0U;
                                    } else if ((0x20U 
                                                <= (IData)(vlSelfRef.top__DOT__cpu_dout))) {
                                        vlSelfRef.top__DOT__vgc_inst__DOT____Vlvbound_h2b480197__0 
                                            = vlSelfRef.top__DOT__cpu_dout;
                                        if (VL_LIKELY((
                                                       (0x07cfU 
                                                        >= 
                                                        ([&]() {
                                                                __Vfunc_top__DOT__vgc_inst__DOT__screen_addr__11__row 
                                                                    = vlSelfRef.top__DOT__vgc_inst__DOT__cursor_y;
                                                                __Vfunc_top__DOT__vgc_inst__DOT__screen_addr__11__col 
                                                                    = vlSelfRef.top__DOT__vgc_inst__DOT__cursor_x;
                                                                __Vfunc_top__DOT__vgc_inst__DOT__screen_addr__11__rr 
                                                                    = 
                                                                    (0x0000001fU 
                                                                     & ((IData)(__Vfunc_top__DOT__vgc_inst__DOT__screen_addr__11__row) 
                                                                        + (IData)(vlSelfRef.top__DOT__vgc_inst__DOT__scroll_offset)));
                                                                if (
                                                                    (0x19U 
                                                                     <= (IData)(__Vfunc_top__DOT__vgc_inst__DOT__screen_addr__11__rr))) {
                                                                    __Vfunc_top__DOT__vgc_inst__DOT__screen_addr__11__rr 
                                                                        = 
                                                                        (0x0000001fU 
                                                                         & ((IData)(__Vfunc_top__DOT__vgc_inst__DOT__screen_addr__11__rr) 
                                                                            - (IData)(0x19U)));
                                                                }
                                                                __Vfunc_top__DOT__vgc_inst__DOT__screen_addr__11__Vfuncout 
                                                                    = 
                                                                    (0x000007ffU 
                                                                     & (((IData)(0x00000050U) 
                                                                         * (IData)(__Vfunc_top__DOT__vgc_inst__DOT__screen_addr__11__rr)) 
                                                                        + (IData)(__Vfunc_top__DOT__vgc_inst__DOT__screen_addr__11__col)));
                                                            }(), (IData)(__Vfunc_top__DOT__vgc_inst__DOT__screen_addr__11__Vfuncout)))))) {
                                            __Vfunc_top__DOT__vgc_inst__DOT__screen_addr__12__row 
                                                = vlSelfRef.top__DOT__vgc_inst__DOT__cursor_y;
                                            __Vfunc_top__DOT__vgc_inst__DOT__screen_addr__12__col 
                                                = vlSelfRef.top__DOT__vgc_inst__DOT__cursor_x;
                                            __Vfunc_top__DOT__vgc_inst__DOT__screen_addr__12__rr 
                                                = (0x0000001fU 
                                                   & ((IData)(__Vfunc_top__DOT__vgc_inst__DOT__screen_addr__12__row) 
                                                      + (IData)(vlSelfRef.top__DOT__vgc_inst__DOT__scroll_offset)));
                                            if ((0x19U 
                                                 <= (IData)(__Vfunc_top__DOT__vgc_inst__DOT__screen_addr__12__rr))) {
                                                __Vfunc_top__DOT__vgc_inst__DOT__screen_addr__12__rr 
                                                    = 
                                                    (0x0000001fU 
                                                     & ((IData)(__Vfunc_top__DOT__vgc_inst__DOT__screen_addr__12__rr) 
                                                        - (IData)(0x19U)));
                                            }
                                            __Vfunc_top__DOT__vgc_inst__DOT__screen_addr__12__Vfuncout 
                                                = (0x000007ffU 
                                                   & (((IData)(0x00000050U) 
                                                       * (IData)(__Vfunc_top__DOT__vgc_inst__DOT__screen_addr__12__rr)) 
                                                      + (IData)(__Vfunc_top__DOT__vgc_inst__DOT__screen_addr__12__col)));
                                            __VdlyVal__top__DOT__vgc_inst__DOT__char_ram__v4 
                                                = vlSelfRef.top__DOT__vgc_inst__DOT____Vlvbound_h2b480197__0;
                                            __VdlyDim0__top__DOT__vgc_inst__DOT__char_ram__v4 
                                                = __Vfunc_top__DOT__vgc_inst__DOT__screen_addr__12__Vfuncout;
                                            vlSelfRef.__VdlyCommitQueuetop__DOT__vgc_inst__DOT__char_ram.enqueue(__VdlyVal__top__DOT__vgc_inst__DOT__char_ram__v4, (IData)(__VdlyDim0__top__DOT__vgc_inst__DOT__char_ram__v4));
                                        }
                                        vlSelfRef.top__DOT__vgc_inst__DOT____Vlvbound_h5d6d955d__0 
                                            = vlSelfRef.top__DOT__vgc_inst__DOT__fg_color;
                                        if (VL_LIKELY((
                                                       (0x07cfU 
                                                        >= 
                                                        ([&]() {
                                                                __Vfunc_top__DOT__vgc_inst__DOT__screen_addr__13__row 
                                                                    = vlSelfRef.top__DOT__vgc_inst__DOT__cursor_y;
                                                                __Vfunc_top__DOT__vgc_inst__DOT__screen_addr__13__col 
                                                                    = vlSelfRef.top__DOT__vgc_inst__DOT__cursor_x;
                                                                __Vfunc_top__DOT__vgc_inst__DOT__screen_addr__13__rr 
                                                                    = 
                                                                    (0x0000001fU 
                                                                     & ((IData)(__Vfunc_top__DOT__vgc_inst__DOT__screen_addr__13__row) 
                                                                        + (IData)(vlSelfRef.top__DOT__vgc_inst__DOT__scroll_offset)));
                                                                if (
                                                                    (0x19U 
                                                                     <= (IData)(__Vfunc_top__DOT__vgc_inst__DOT__screen_addr__13__rr))) {
                                                                    __Vfunc_top__DOT__vgc_inst__DOT__screen_addr__13__rr 
                                                                        = 
                                                                        (0x0000001fU 
                                                                         & ((IData)(__Vfunc_top__DOT__vgc_inst__DOT__screen_addr__13__rr) 
                                                                            - (IData)(0x19U)));
                                                                }
                                                                __Vfunc_top__DOT__vgc_inst__DOT__screen_addr__13__Vfuncout 
                                                                    = 
                                                                    (0x000007ffU 
                                                                     & (((IData)(0x00000050U) 
                                                                         * (IData)(__Vfunc_top__DOT__vgc_inst__DOT__screen_addr__13__rr)) 
                                                                        + (IData)(__Vfunc_top__DOT__vgc_inst__DOT__screen_addr__13__col)));
                                                            }(), (IData)(__Vfunc_top__DOT__vgc_inst__DOT__screen_addr__13__Vfuncout)))))) {
                                            __Vfunc_top__DOT__vgc_inst__DOT__screen_addr__14__row 
                                                = vlSelfRef.top__DOT__vgc_inst__DOT__cursor_y;
                                            __Vfunc_top__DOT__vgc_inst__DOT__screen_addr__14__col 
                                                = vlSelfRef.top__DOT__vgc_inst__DOT__cursor_x;
                                            __Vfunc_top__DOT__vgc_inst__DOT__screen_addr__14__rr 
                                                = (0x0000001fU 
                                                   & ((IData)(__Vfunc_top__DOT__vgc_inst__DOT__screen_addr__14__row) 
                                                      + (IData)(vlSelfRef.top__DOT__vgc_inst__DOT__scroll_offset)));
                                            if ((0x19U 
                                                 <= (IData)(__Vfunc_top__DOT__vgc_inst__DOT__screen_addr__14__rr))) {
                                                __Vfunc_top__DOT__vgc_inst__DOT__screen_addr__14__rr 
                                                    = 
                                                    (0x0000001fU 
                                                     & ((IData)(__Vfunc_top__DOT__vgc_inst__DOT__screen_addr__14__rr) 
                                                        - (IData)(0x19U)));
                                            }
                                            __Vfunc_top__DOT__vgc_inst__DOT__screen_addr__14__Vfuncout 
                                                = (0x000007ffU 
                                                   & (((IData)(0x00000050U) 
                                                       * (IData)(__Vfunc_top__DOT__vgc_inst__DOT__screen_addr__14__rr)) 
                                                      + (IData)(__Vfunc_top__DOT__vgc_inst__DOT__screen_addr__14__col)));
                                            __VdlyVal__top__DOT__vgc_inst__DOT__color_ram__v3 
                                                = vlSelfRef.top__DOT__vgc_inst__DOT____Vlvbound_h5d6d955d__0;
                                            __VdlyDim0__top__DOT__vgc_inst__DOT__color_ram__v3 
                                                = __Vfunc_top__DOT__vgc_inst__DOT__screen_addr__14__Vfuncout;
                                            vlSelfRef.__VdlyCommitQueuetop__DOT__vgc_inst__DOT__color_ram.enqueue(__VdlyVal__top__DOT__vgc_inst__DOT__color_ram__v3, (IData)(__VdlyDim0__top__DOT__vgc_inst__DOT__color_ram__v3));
                                        }
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
                                } else if ((1U & (~ (IData)(vlSelfRef.top__DOT__vgc_inst__DOT__cmd_busy)))) {
                                    __Vdly__top__DOT__vgc_inst__DOT__cmd_x 
                                        = ((0x00000300U 
                                            & (vlSelfRef.top__DOT__vgc_inst__DOT__regs[18U] 
                                               << 8U)) 
                                           | vlSelfRef.top__DOT__vgc_inst__DOT__regs[17U]);
                                    __Vdly__top__DOT__vgc_inst__DOT__cmd_y 
                                        = ((0x00000300U 
                                            & (vlSelfRef.top__DOT__vgc_inst__DOT__regs[20U] 
                                               << 8U)) 
                                           | vlSelfRef.top__DOT__vgc_inst__DOT__regs[19U]);
                                    vlSelfRef.top__DOT__vgc_inst__DOT__cmd_x2 
                                        = ((0x00000300U 
                                            & (vlSelfRef.top__DOT__vgc_inst__DOT__regs[22U] 
                                               << 8U)) 
                                           | vlSelfRef.top__DOT__vgc_inst__DOT__regs[21U]);
                                    vlSelfRef.top__DOT__vgc_inst__DOT__cmd_y2 
                                        = ((0x00000300U 
                                            & (vlSelfRef.top__DOT__vgc_inst__DOT__regs[24U] 
                                               << 8U)) 
                                           | vlSelfRef.top__DOT__vgc_inst__DOT__regs[23U]);
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
                                                                vlSelfRef.top__DOT__vgc_inst__DOT__unnamedblk17__DOT__i = 0U;
                                                                __Vdly__top__DOT__vgc_inst__DOT__cursor_x = 0U;
                                                                __Vdly__top__DOT__vgc_inst__DOT__cursor_y = 0U;
                                                                vlSelfRef.top__DOT__vgc_inst__DOT__mode = 0U;
                                                                __Vdly__top__DOT__vgc_inst__DOT__fg_color = 1U;
                                                                vlSelfRef.top__DOT__vgc_inst__DOT__bg_color = 6U;
                                                                vlSelfRef.top__DOT__vgc_inst__DOT__border_color = 6U;
                                                                __Vdly__top__DOT__vgc_inst__DOT__gfx_color = 1U;
                                                                __Vdly__top__DOT__vgc_inst__DOT__scroll_offset = 0U;
                                                                vlSelfRef.top__DOT__vgc_inst__DOT__cursor_enable = 1U;
                                                                vlSelfRef.top__DOT__vgc_inst__DOT__copper_enabled = 0U;
                                                                __Vdly__top__DOT__vgc_inst__DOT__copper_count = 0U;
                                                                __Vdly__top__DOT__vgc_inst__DOT__copper_index = 0U;
                                                                __Vdly__top__DOT__vgc_inst__DOT__copper_target_list = 0U;
                                                                __Vdly__top__DOT__vgc_inst__DOT__copper_active_list = 0U;
                                                                __Vdly__top__DOT__vgc_inst__DOT__copper_pending_list = 0U;
                                                                __Vdly__top__DOT__vgc_inst__DOT__copper_loading = 0U;
                                                                while (
                                                                       VL_GTS_III(32, 0x00000080U, vlSelfRef.top__DOT__vgc_inst__DOT__unnamedblk17__DOT__i)) {
                                                                    __VdlyDim0__top__DOT__vgc_inst__DOT__copper_list_count__v0 
                                                                        = 
                                                                        (0x0000007fU 
                                                                         & vlSelfRef.top__DOT__vgc_inst__DOT__unnamedblk17__DOT__i);
                                                                    vlSelfRef.__VdlyCommitQueuetop__DOT__vgc_inst__DOT__copper_list_count.enqueue(0U, (IData)(__VdlyDim0__top__DOT__vgc_inst__DOT__copper_list_count__v0));
                                                                    vlSelfRef.top__DOT__vgc_inst__DOT__unnamedblk17__DOT__i 
                                                                        = 
                                                                        ((IData)(1U) 
                                                                         + vlSelfRef.top__DOT__vgc_inst__DOT__unnamedblk17__DOT__i);
                                                                }
                                                            } else {
                                                                vlSelfRef.top__DOT__vgc_inst__DOT__copper_enabled = 0U;
                                                            }
                                                        } else if (
                                                                   (1U 
                                                                    & (IData)(vlSelfRef.top__DOT__cpu_dout))) {
                                                            vlSelfRef.top__DOT__vgc_inst__DOT__copper_enabled = 1U;
                                                        } else {
                                                            __VdlyDim0__top__DOT__vgc_inst__DOT__copper_list_count__v1 
                                                                = vlSelfRef.top__DOT__vgc_inst__DOT__copper_target_list;
                                                            vlSelfRef.__VdlyCommitQueuetop__DOT__vgc_inst__DOT__copper_list_count.enqueue(0U, (IData)(__VdlyDim0__top__DOT__vgc_inst__DOT__copper_list_count__v1));
                                                            if (
                                                                ((IData)(vlSelfRef.top__DOT__vgc_inst__DOT__copper_target_list) 
                                                                 == (IData)(vlSelfRef.top__DOT__vgc_inst__DOT__copper_active_list))) {
                                                                __Vdly__top__DOT__vgc_inst__DOT__copper_count = 0U;
                                                                __Vdly__top__DOT__vgc_inst__DOT__copper_index = 0U;
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
                                                            vlSelfRef.top__DOT__vgc_inst__DOT____Vlvbound_ha13cfb6b__0 
                                                                = vlSelfRef.top__DOT__vgc_inst__DOT__regs[20U];
                                                            if (VL_LIKELY((
                                                                           (0x07cfU 
                                                                            >= 
                                                                            ((0x00000700U 
                                                                              & (vlSelfRef.top__DOT__vgc_inst__DOT__regs[19U] 
                                                                                << 8U)) 
                                                                             | vlSelfRef.top__DOT__vgc_inst__DOT__regs[18U]))))) {
                                                                __VdlyVal__top__DOT__vgc_inst__DOT__char_ram__v5 
                                                                    = vlSelfRef.top__DOT__vgc_inst__DOT____Vlvbound_ha13cfb6b__0;
                                                                __VdlyDim0__top__DOT__vgc_inst__DOT__char_ram__v5 
                                                                    = 
                                                                    ((0x00000700U 
                                                                      & (vlSelfRef.top__DOT__vgc_inst__DOT__regs[19U] 
                                                                         << 8U)) 
                                                                     | vlSelfRef.top__DOT__vgc_inst__DOT__regs[18U]);
                                                                vlSelfRef.__VdlyCommitQueuetop__DOT__vgc_inst__DOT__char_ram.enqueue(__VdlyVal__top__DOT__vgc_inst__DOT__char_ram__v5, (IData)(__VdlyDim0__top__DOT__vgc_inst__DOT__char_ram__v5));
                                                            }
                                                        } else if (
                                                                   (1U 
                                                                    == vlSelfRef.top__DOT__vgc_inst__DOT__regs[17U])) {
                                                            vlSelfRef.top__DOT__vgc_inst__DOT____Vlvbound_hd4129f91__0 
                                                                = vlSelfRef.top__DOT__vgc_inst__DOT__regs[20U];
                                                            if (VL_LIKELY((
                                                                           (0x07cfU 
                                                                            >= 
                                                                            ((0x00000700U 
                                                                              & (vlSelfRef.top__DOT__vgc_inst__DOT__regs[19U] 
                                                                                << 8U)) 
                                                                             | vlSelfRef.top__DOT__vgc_inst__DOT__regs[18U]))))) {
                                                                __VdlyVal__top__DOT__vgc_inst__DOT__color_ram__v4 
                                                                    = vlSelfRef.top__DOT__vgc_inst__DOT____Vlvbound_hd4129f91__0;
                                                                __VdlyDim0__top__DOT__vgc_inst__DOT__color_ram__v4 
                                                                    = 
                                                                    ((0x00000700U 
                                                                      & (vlSelfRef.top__DOT__vgc_inst__DOT__regs[19U] 
                                                                         << 8U)) 
                                                                     | vlSelfRef.top__DOT__vgc_inst__DOT__regs[18U]);
                                                                vlSelfRef.__VdlyCommitQueuetop__DOT__vgc_inst__DOT__color_ram.enqueue(__VdlyVal__top__DOT__vgc_inst__DOT__color_ram__v4, (IData)(__VdlyDim0__top__DOT__vgc_inst__DOT__color_ram__v4));
                                                            }
                                                        } else if (
                                                                   (2U 
                                                                    == vlSelfRef.top__DOT__vgc_inst__DOT__regs[17U])) {
                                                            vlSelfRef.top__DOT__vgc_inst__DOT____Vlvbound_h45978041__0 
                                                                = 
                                                                (0x0000000fU 
                                                                 & vlSelfRef.top__DOT__vgc_inst__DOT__regs[20U]);
                                                            if (VL_LIKELY((
                                                                           (0xf9ffU 
                                                                            >= 
                                                                            ((vlSelfRef.top__DOT__vgc_inst__DOT__regs[19U] 
                                                                              << 8U) 
                                                                             | vlSelfRef.top__DOT__vgc_inst__DOT__regs[18U]))))) {
                                                                __VdlyVal__top__DOT__vgc_inst__DOT__gfx_ram__v15 
                                                                    = vlSelfRef.top__DOT__vgc_inst__DOT____Vlvbound_h45978041__0;
                                                                __VdlyDim0__top__DOT__vgc_inst__DOT__gfx_ram__v15 
                                                                    = 
                                                                    ((vlSelfRef.top__DOT__vgc_inst__DOT__regs[19U] 
                                                                      << 8U) 
                                                                     | vlSelfRef.top__DOT__vgc_inst__DOT__regs[18U]);
                                                                vlSelfRef.__VdlyCommitQueuetop__DOT__vgc_inst__DOT__gfx_ram.enqueue(__VdlyVal__top__DOT__vgc_inst__DOT__gfx_ram__v15, (IData)(__VdlyDim0__top__DOT__vgc_inst__DOT__gfx_ram__v15));
                                                            }
                                                        } else if (
                                                                   (3U 
                                                                    == vlSelfRef.top__DOT__vgc_inst__DOT__regs[17U])) {
                                                            __VdlyVal__top__DOT__vgc_inst__DOT__sprite_shapes__v3 
                                                                = vlSelfRef.top__DOT__vgc_inst__DOT__regs[20U];
                                                            __VdlyDim0__top__DOT__vgc_inst__DOT__sprite_shapes__v3 
                                                                = 
                                                                ((0x00000700U 
                                                                  & (vlSelfRef.top__DOT__vgc_inst__DOT__regs[19U] 
                                                                     << 8U)) 
                                                                 | vlSelfRef.top__DOT__vgc_inst__DOT__regs[18U]);
                                                            vlSelfRef.__VdlyCommitQueuetop__DOT__vgc_inst__DOT__sprite_shapes.enqueue(__VdlyVal__top__DOT__vgc_inst__DOT__sprite_shapes__v3, (IData)(__VdlyDim0__top__DOT__vgc_inst__DOT__sprite_shapes__v3));
                                                        } else if (
                                                                   (4U 
                                                                    == vlSelfRef.top__DOT__vgc_inst__DOT__regs[17U])) {
                                                            __VdlyVal__top__DOT__vgc_inst__DOT__tile_inst__DOT__tile_data__v0 
                                                                = vlSelfRef.top__DOT__vgc_inst__DOT__regs[20U];
                                                            __VdlyDim0__top__DOT__vgc_inst__DOT__tile_inst__DOT__tile_data__v0 
                                                                = 
                                                                ((0x00007f00U 
                                                                  & (vlSelfRef.top__DOT__vgc_inst__DOT__regs[19U] 
                                                                     << 8U)) 
                                                                 | vlSelfRef.top__DOT__vgc_inst__DOT__regs[18U]);
                                                            __VdlySet__top__DOT__vgc_inst__DOT__tile_inst__DOT__tile_data__v0 = 1U;
                                                        }
                                                    } else if (
                                                               (1U 
                                                                & (IData)(vlSelfRef.top__DOT__cpu_dout))) {
                                                        if (
                                                            (0U 
                                                             == vlSelfRef.top__DOT__vgc_inst__DOT__regs[17U])) {
                                                            __VdlyVal__top__DOT__vgc_inst__DOT__regs__v32 
                                                                = 
                                                                ((0x07cfU 
                                                                  >= 
                                                                  ((0x00000700U 
                                                                    & (vlSelfRef.top__DOT__vgc_inst__DOT__regs[19U] 
                                                                       << 8U)) 
                                                                   | vlSelfRef.top__DOT__vgc_inst__DOT__regs[18U]))
                                                                  ? vlSelfRef.top__DOT__vgc_inst__DOT__char_ram
                                                                 [
                                                                 ((0x00000700U 
                                                                   & (vlSelfRef.top__DOT__vgc_inst__DOT__regs[19U] 
                                                                      << 8U)) 
                                                                  | vlSelfRef.top__DOT__vgc_inst__DOT__regs[18U])]
                                                                  : 0U);
                                                            __VdlySet__top__DOT__vgc_inst__DOT__regs__v32 = 1U;
                                                        } else if (
                                                                   (1U 
                                                                    == vlSelfRef.top__DOT__vgc_inst__DOT__regs[17U])) {
                                                            __VdlyVal__top__DOT__vgc_inst__DOT__regs__v33 
                                                                = 
                                                                ((0x07cfU 
                                                                  >= 
                                                                  ((0x00000700U 
                                                                    & (vlSelfRef.top__DOT__vgc_inst__DOT__regs[19U] 
                                                                       << 8U)) 
                                                                   | vlSelfRef.top__DOT__vgc_inst__DOT__regs[18U]))
                                                                  ? vlSelfRef.top__DOT__vgc_inst__DOT__color_ram
                                                                 [
                                                                 ((0x00000700U 
                                                                   & (vlSelfRef.top__DOT__vgc_inst__DOT__regs[19U] 
                                                                      << 8U)) 
                                                                  | vlSelfRef.top__DOT__vgc_inst__DOT__regs[18U])]
                                                                  : 0U);
                                                            __VdlySet__top__DOT__vgc_inst__DOT__regs__v33 = 1U;
                                                        } else if (
                                                                   (2U 
                                                                    == vlSelfRef.top__DOT__vgc_inst__DOT__regs[17U])) {
                                                            __VdlyVal__top__DOT__vgc_inst__DOT__regs__v34 
                                                                = 
                                                                ((0xf9ffU 
                                                                  >= 
                                                                  ((vlSelfRef.top__DOT__vgc_inst__DOT__regs[19U] 
                                                                    << 8U) 
                                                                   | vlSelfRef.top__DOT__vgc_inst__DOT__regs[18U]))
                                                                  ? vlSelfRef.top__DOT__vgc_inst__DOT__gfx_ram
                                                                 [
                                                                 ((vlSelfRef.top__DOT__vgc_inst__DOT__regs[19U] 
                                                                   << 8U) 
                                                                  | vlSelfRef.top__DOT__vgc_inst__DOT__regs[18U])]
                                                                  : 0U);
                                                            __VdlySet__top__DOT__vgc_inst__DOT__regs__v34 = 1U;
                                                        } else if (
                                                                   (3U 
                                                                    == vlSelfRef.top__DOT__vgc_inst__DOT__regs[17U])) {
                                                            __VdlyVal__top__DOT__vgc_inst__DOT__regs__v35 
                                                                = vlSelfRef.top__DOT__vgc_inst__DOT__sprite_shapes
                                                                [
                                                                ((0x00000700U 
                                                                  & (vlSelfRef.top__DOT__vgc_inst__DOT__regs[19U] 
                                                                     << 8U)) 
                                                                 | vlSelfRef.top__DOT__vgc_inst__DOT__regs[18U])];
                                                            __VdlySet__top__DOT__vgc_inst__DOT__regs__v35 = 1U;
                                                        } else if (
                                                                   (4U 
                                                                    == vlSelfRef.top__DOT__vgc_inst__DOT__regs[17U])) {
                                                            __VdlyVal__top__DOT__vgc_inst__DOT__regs__v36 
                                                                = vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__tile_data
                                                                [
                                                                ((0x00007f00U 
                                                                  & (vlSelfRef.top__DOT__vgc_inst__DOT__regs[19U] 
                                                                     << 8U)) 
                                                                 | vlSelfRef.top__DOT__vgc_inst__DOT__regs[18U])];
                                                            __VdlySet__top__DOT__vgc_inst__DOT__regs__v36 = 1U;
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
                                                            __Vdly__top__DOT__vgc_inst__DOT__cmd_x 
                                                                = 
                                                                (0x0000000fU 
                                                                 & vlSelfRef.top__DOT__vgc_inst__DOT__regs[17U]);
                                                            __Vdly__top__DOT__vgc_inst__DOT__cmd_cy = 0U;
                                                            __Vdly__top__DOT__vgc_inst__DOT__cmd_y 
                                                                = 
                                                                (0x0000000fU 
                                                                 & vlSelfRef.top__DOT__vgc_inst__DOT__regs[18U]);
                                                            __Vdly__top__DOT__vgc_inst__DOT__cmd_cx = 0U;
                                                            __Vdly__top__DOT__vgc_inst__DOT__cmd_busy = 1U;
                                                            vlSelfRef.top__DOT__vgc_inst__DOT__cmd_op = 0x13U;
                                                        }
                                                    } else if (
                                                               (0x10U 
                                                                > vlSelfRef.top__DOT__vgc_inst__DOT__regs[17U])) {
                                                        __Vdly__top__DOT__vgc_inst__DOT__cmd_cy = 0U;
                                                        __Vdly__top__DOT__vgc_inst__DOT__cmd_x 
                                                            = 
                                                            (0x0000001eU 
                                                             & (vlSelfRef.top__DOT__vgc_inst__DOT__regs[17U] 
                                                                << 1U));
                                                        __Vdly__top__DOT__vgc_inst__DOT__cmd_cx = 0U;
                                                        __Vdly__top__DOT__vgc_inst__DOT__cmd_busy = 1U;
                                                        vlSelfRef.top__DOT__vgc_inst__DOT__cmd_op = 0x12U;
                                                    }
                                                } else if (
                                                           (1U 
                                                            & (IData)(vlSelfRef.top__DOT__cpu_dout))) {
                                                    if (
                                                        ((0x10U 
                                                          > vlSelfRef.top__DOT__vgc_inst__DOT__regs[17U]) 
                                                         & (0x10U 
                                                            > vlSelfRef.top__DOT__vgc_inst__DOT__regs[18U]))) {
                                                        __VdlyVal__top__DOT__vgc_inst__DOT__sprite_shapes__v4 
                                                            = vlSelfRef.top__DOT__vgc_inst__DOT__regs[19U];
                                                        __VdlyDim0__top__DOT__vgc_inst__DOT__sprite_shapes__v4 
                                                            = 
                                                            ((0x00000780U 
                                                              & (vlSelfRef.top__DOT__vgc_inst__DOT__regs[17U] 
                                                                 << 7U)) 
                                                             | (0x00000078U 
                                                                & (vlSelfRef.top__DOT__vgc_inst__DOT__regs[18U] 
                                                                   << 3U)));
                                                        vlSelfRef.__VdlyCommitQueuetop__DOT__vgc_inst__DOT__sprite_shapes.enqueue(__VdlyVal__top__DOT__vgc_inst__DOT__sprite_shapes__v4, (IData)(__VdlyDim0__top__DOT__vgc_inst__DOT__sprite_shapes__v4));
                                                        __VdlyVal__top__DOT__vgc_inst__DOT__sprite_shapes__v5 
                                                            = vlSelfRef.top__DOT__vgc_inst__DOT__regs[20U];
                                                        __VdlyDim0__top__DOT__vgc_inst__DOT__sprite_shapes__v5 
                                                            = 
                                                            (1U 
                                                             | ((0x00000780U 
                                                                 & (vlSelfRef.top__DOT__vgc_inst__DOT__regs[17U] 
                                                                    << 7U)) 
                                                                | (0x00000078U 
                                                                   & (vlSelfRef.top__DOT__vgc_inst__DOT__regs[18U] 
                                                                      << 3U))));
                                                        vlSelfRef.__VdlyCommitQueuetop__DOT__vgc_inst__DOT__sprite_shapes.enqueue(__VdlyVal__top__DOT__vgc_inst__DOT__sprite_shapes__v5, (IData)(__VdlyDim0__top__DOT__vgc_inst__DOT__sprite_shapes__v5));
                                                        __VdlyVal__top__DOT__vgc_inst__DOT__sprite_shapes__v6 
                                                            = vlSelfRef.top__DOT__vgc_inst__DOT__regs[21U];
                                                        __VdlyDim0__top__DOT__vgc_inst__DOT__sprite_shapes__v6 
                                                            = 
                                                            (2U 
                                                             | ((0x00000780U 
                                                                 & (vlSelfRef.top__DOT__vgc_inst__DOT__regs[17U] 
                                                                    << 7U)) 
                                                                | (0x00000078U 
                                                                   & (vlSelfRef.top__DOT__vgc_inst__DOT__regs[18U] 
                                                                      << 3U))));
                                                        vlSelfRef.__VdlyCommitQueuetop__DOT__vgc_inst__DOT__sprite_shapes.enqueue(__VdlyVal__top__DOT__vgc_inst__DOT__sprite_shapes__v6, (IData)(__VdlyDim0__top__DOT__vgc_inst__DOT__sprite_shapes__v6));
                                                        __VdlyVal__top__DOT__vgc_inst__DOT__sprite_shapes__v7 
                                                            = vlSelfRef.top__DOT__vgc_inst__DOT__regs[22U];
                                                        __VdlyDim0__top__DOT__vgc_inst__DOT__sprite_shapes__v7 
                                                            = 
                                                            (3U 
                                                             | ((0x00000780U 
                                                                 & (vlSelfRef.top__DOT__vgc_inst__DOT__regs[17U] 
                                                                    << 7U)) 
                                                                | (0x00000078U 
                                                                   & (vlSelfRef.top__DOT__vgc_inst__DOT__regs[18U] 
                                                                      << 3U))));
                                                        vlSelfRef.__VdlyCommitQueuetop__DOT__vgc_inst__DOT__sprite_shapes.enqueue(__VdlyVal__top__DOT__vgc_inst__DOT__sprite_shapes__v7, (IData)(__VdlyDim0__top__DOT__vgc_inst__DOT__sprite_shapes__v7));
                                                        __VdlyVal__top__DOT__vgc_inst__DOT__sprite_shapes__v8 
                                                            = vlSelfRef.top__DOT__vgc_inst__DOT__regs[23U];
                                                        __VdlyDim0__top__DOT__vgc_inst__DOT__sprite_shapes__v8 
                                                            = 
                                                            (4U 
                                                             | ((0x00000780U 
                                                                 & (vlSelfRef.top__DOT__vgc_inst__DOT__regs[17U] 
                                                                    << 7U)) 
                                                                | (0x00000078U 
                                                                   & (vlSelfRef.top__DOT__vgc_inst__DOT__regs[18U] 
                                                                      << 3U))));
                                                        vlSelfRef.__VdlyCommitQueuetop__DOT__vgc_inst__DOT__sprite_shapes.enqueue(__VdlyVal__top__DOT__vgc_inst__DOT__sprite_shapes__v8, (IData)(__VdlyDim0__top__DOT__vgc_inst__DOT__sprite_shapes__v8));
                                                        __VdlyVal__top__DOT__vgc_inst__DOT__sprite_shapes__v9 
                                                            = vlSelfRef.top__DOT__vgc_inst__DOT__regs[24U];
                                                        __VdlyDim0__top__DOT__vgc_inst__DOT__sprite_shapes__v9 
                                                            = 
                                                            (5U 
                                                             | ((0x00000780U 
                                                                 & (vlSelfRef.top__DOT__vgc_inst__DOT__regs[17U] 
                                                                    << 7U)) 
                                                                | (0x00000078U 
                                                                   & (vlSelfRef.top__DOT__vgc_inst__DOT__regs[18U] 
                                                                      << 3U))));
                                                        vlSelfRef.__VdlyCommitQueuetop__DOT__vgc_inst__DOT__sprite_shapes.enqueue(__VdlyVal__top__DOT__vgc_inst__DOT__sprite_shapes__v9, (IData)(__VdlyDim0__top__DOT__vgc_inst__DOT__sprite_shapes__v9));
                                                        __VdlyVal__top__DOT__vgc_inst__DOT__sprite_shapes__v10 
                                                            = vlSelfRef.top__DOT__vgc_inst__DOT__regs[25U];
                                                        __VdlyDim0__top__DOT__vgc_inst__DOT__sprite_shapes__v10 
                                                            = 
                                                            (6U 
                                                             | ((0x00000780U 
                                                                 & (vlSelfRef.top__DOT__vgc_inst__DOT__regs[17U] 
                                                                    << 7U)) 
                                                                | (0x00000078U 
                                                                   & (vlSelfRef.top__DOT__vgc_inst__DOT__regs[18U] 
                                                                      << 3U))));
                                                        vlSelfRef.__VdlyCommitQueuetop__DOT__vgc_inst__DOT__sprite_shapes.enqueue(__VdlyVal__top__DOT__vgc_inst__DOT__sprite_shapes__v10, (IData)(__VdlyDim0__top__DOT__vgc_inst__DOT__sprite_shapes__v10));
                                                        __VdlyVal__top__DOT__vgc_inst__DOT__sprite_shapes__v11 
                                                            = vlSelfRef.top__DOT__vgc_inst__DOT__regs[26U];
                                                        __VdlyDim0__top__DOT__vgc_inst__DOT__sprite_shapes__v11 
                                                            = 
                                                            (7U 
                                                             | ((0x00000780U 
                                                                 & (vlSelfRef.top__DOT__vgc_inst__DOT__regs[17U] 
                                                                    << 7U)) 
                                                                | (0x00000078U 
                                                                   & (vlSelfRef.top__DOT__vgc_inst__DOT__regs[18U] 
                                                                      << 3U))));
                                                        vlSelfRef.__VdlyCommitQueuetop__DOT__vgc_inst__DOT__sprite_shapes.enqueue(__VdlyVal__top__DOT__vgc_inst__DOT__sprite_shapes__v11, (IData)(__VdlyDim0__top__DOT__vgc_inst__DOT__sprite_shapes__v11));
                                                    }
                                                } else if (
                                                           (((0x10U 
                                                              > vlSelfRef.top__DOT__vgc_inst__DOT__regs[17U]) 
                                                             & (0x10U 
                                                                > vlSelfRef.top__DOT__vgc_inst__DOT__regs[18U])) 
                                                            & (0x10U 
                                                               > vlSelfRef.top__DOT__vgc_inst__DOT__regs[19U]))) {
                                                    __VdlyVal__top__DOT__vgc_inst__DOT__sprite_shapes__v12 
                                                        = 
                                                        ((1U 
                                                          & vlSelfRef.top__DOT__vgc_inst__DOT__regs[18U])
                                                          ? 
                                                         ((0x000000f0U 
                                                           & vlSelfRef.top__DOT__vgc_inst__DOT__sprite_shapes
                                                           [
                                                           ((0x00000780U 
                                                             & (vlSelfRef.top__DOT__vgc_inst__DOT__regs[17U] 
                                                                << 7U)) 
                                                            | ((0x00000078U 
                                                                & (vlSelfRef.top__DOT__vgc_inst__DOT__regs[19U] 
                                                                   << 3U)) 
                                                               | (7U 
                                                                  & (vlSelfRef.top__DOT__vgc_inst__DOT__regs[18U] 
                                                                     >> 1U))))]) 
                                                          | (0x0000000fU 
                                                             & vlSelfRef.top__DOT__vgc_inst__DOT__regs[20U]))
                                                          : 
                                                         ((0x000000f0U 
                                                           & (vlSelfRef.top__DOT__vgc_inst__DOT__regs[20U] 
                                                              << 4U)) 
                                                          | (0x0000000fU 
                                                             & vlSelfRef.top__DOT__vgc_inst__DOT__sprite_shapes
                                                             [
                                                             ((0x00000780U 
                                                               & (vlSelfRef.top__DOT__vgc_inst__DOT__regs[17U] 
                                                                  << 7U)) 
                                                              | ((0x00000078U 
                                                                  & (vlSelfRef.top__DOT__vgc_inst__DOT__regs[19U] 
                                                                     << 3U)) 
                                                                 | (7U 
                                                                    & (vlSelfRef.top__DOT__vgc_inst__DOT__regs[18U] 
                                                                       >> 1U))))])));
                                                    __VdlyDim0__top__DOT__vgc_inst__DOT__sprite_shapes__v12 
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
                                                    vlSelfRef.__VdlyCommitQueuetop__DOT__vgc_inst__DOT__sprite_shapes.enqueue(__VdlyVal__top__DOT__vgc_inst__DOT__sprite_shapes__v12, (IData)(__VdlyDim0__top__DOT__vgc_inst__DOT__sprite_shapes__v12));
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
                                                            if (
                                                                (0U 
                                                                 < (IData)(vlSelfRef.top__DOT__vgc_inst__DOT__fio_name_len))) {
                                                                vlSelfRef.top__DOT__vgc_inst__DOT__gt_pen_x 
                                                                    = 
                                                                    ((0x00000100U 
                                                                      & (vlSelfRef.top__DOT__vgc_inst__DOT__regs[18U] 
                                                                         << 8U)) 
                                                                     | vlSelfRef.top__DOT__vgc_inst__DOT__regs[17U]);
                                                                vlSelfRef.top__DOT__vgc_inst__DOT__gt_start_y 
                                                                    = vlSelfRef.top__DOT__vgc_inst__DOT__regs[19U];
                                                                __Vdly__top__DOT__vgc_inst__DOT__gt_char_idx = 0U;
                                                                vlSelfRef.top__DOT__vgc_inst__DOT__gt_char_len 
                                                                    = vlSelfRef.top__DOT__vgc_inst__DOT__fio_name_len;
                                                                __Vdly__top__DOT__vgc_inst__DOT__gt_font_row = 0U;
                                                                vlSelfRef.top__DOT__vgc_inst__DOT__gt_font_col = 0U;
                                                                vlSelfRef.top__DOT__vgc_inst__DOT__gt_scale_x = 0U;
                                                                vlSelfRef.top__DOT__vgc_inst__DOT__gt_scale_y = 0U;
                                                                vlSelfRef.top__DOT__vgc_inst__DOT__gt_cur_char 
                                                                    = vlSelfRef.top__DOT__vgc_inst__DOT__fio_name[0U];
                                                                vlSelfRef.top__DOT__vgc_inst__DOT__gt_font_byte 
                                                                    = vlSelfRef.top__DOT__vgc_inst__DOT__font_rom
                                                                    [
                                                                    (vlSelfRef.top__DOT__vgc_inst__DOT__fio_name[0U] 
                                                                     << 3U)];
                                                                __Vdly__top__DOT__vgc_inst__DOT__cmd_busy = 1U;
                                                                vlSelfRef.top__DOT__vgc_inst__DOT__cmd_op = 0x0aU;
                                                                vlSelfRef.top__DOT__vgc_inst__DOT__gt_scale 
                                                                    = 
                                                                    ((1U 
                                                                      > vlSelfRef.top__DOT__vgc_inst__DOT__regs[22U])
                                                                      ? 1U
                                                                      : vlSelfRef.top__DOT__vgc_inst__DOT__regs[22U]);
                                                            }
                                                        }
                                                    } else if (
                                                               (1U 
                                                                & (IData)(vlSelfRef.top__DOT__cpu_dout))) {
                                                        __Vdly__top__DOT__vgc_inst__DOT__cmd_x 
                                                            = 
                                                            ((0x00000300U 
                                                              & (vlSelfRef.top__DOT__vgc_inst__DOT__regs[18U] 
                                                                 << 8U)) 
                                                             | vlSelfRef.top__DOT__vgc_inst__DOT__regs[17U]);
                                                        __Vdly__top__DOT__vgc_inst__DOT__cmd_y 
                                                            = 
                                                            ((0x00000300U 
                                                              & (vlSelfRef.top__DOT__vgc_inst__DOT__regs[20U] 
                                                                 << 8U)) 
                                                             | vlSelfRef.top__DOT__vgc_inst__DOT__regs[19U]);
                                                        if (
                                                            ((0x0140U 
                                                              > 
                                                              ((0x00000100U 
                                                                & (vlSelfRef.top__DOT__vgc_inst__DOT__regs[18U] 
                                                                   << 8U)) 
                                                               | vlSelfRef.top__DOT__vgc_inst__DOT__regs[17U])) 
                                                             & (0xc8U 
                                                                > vlSelfRef.top__DOT__vgc_inst__DOT__regs[19U]))) {
                                                            vlSelfRef.top__DOT__vgc_inst__DOT__fill_target 
                                                                = 
                                                                ((0xf9ffU 
                                                                  >= 
                                                                  (0x0000ffffU 
                                                                   & (((IData)(0x00000140U) 
                                                                       * vlSelfRef.top__DOT__vgc_inst__DOT__regs[19U]) 
                                                                      + 
                                                                      ((0x00000100U 
                                                                        & (vlSelfRef.top__DOT__vgc_inst__DOT__regs[18U] 
                                                                           << 8U)) 
                                                                       | vlSelfRef.top__DOT__vgc_inst__DOT__regs[17U]))))
                                                                  ? vlSelfRef.top__DOT__vgc_inst__DOT__gfx_ram
                                                                 [
                                                                 (0x0000ffffU 
                                                                  & (((IData)(0x00000140U) 
                                                                      * vlSelfRef.top__DOT__vgc_inst__DOT__regs[19U]) 
                                                                     + 
                                                                     ((0x00000100U 
                                                                       & (vlSelfRef.top__DOT__vgc_inst__DOT__regs[18U] 
                                                                          << 8U)) 
                                                                      | vlSelfRef.top__DOT__vgc_inst__DOT__regs[17U])))]
                                                                  : 0U);
                                                            __Vdly__top__DOT__vgc_inst__DOT__fill_sp = 1U;
                                                            __Vdly__top__DOT__vgc_inst__DOT__cmd_busy = 1U;
                                                            vlSelfRef.top__DOT__vgc_inst__DOT__cmd_op = 9U;
                                                            __VdlyVal__top__DOT__vgc_inst__DOT__fill_stack__v4 
                                                                = 
                                                                ((vlSelfRef.top__DOT__vgc_inst__DOT__regs[19U] 
                                                                  << 9U) 
                                                                 | ((0x00000100U 
                                                                     & (vlSelfRef.top__DOT__vgc_inst__DOT__regs[18U] 
                                                                        << 8U)) 
                                                                    | vlSelfRef.top__DOT__vgc_inst__DOT__regs[17U]));
                                                            __VdlySet__top__DOT__vgc_inst__DOT__fill_stack__v4 = 1U;
                                                        }
                                                    } else {
                                                        __Vdly__top__DOT__vgc_inst__DOT__gfx_color 
                                                            = 
                                                            (0x0000000fU 
                                                             & vlSelfRef.top__DOT__vgc_inst__DOT__regs[17U]);
                                                    }
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
                                                        __Vdly__top__DOT__vgc_inst__DOT__cmd_cy = 0U;
                                                        __Vdly__top__DOT__vgc_inst__DOT__cmd_cx = 0U;
                                                        __Vdly__top__DOT__vgc_inst__DOT__cmd_busy = 1U;
                                                        vlSelfRef.top__DOT__vgc_inst__DOT__cmd_op = 7U;
                                                    } else {
                                                        __Vdly__top__DOT__vgc_inst__DOT__cmd_cx 
                                                            = 
                                                            ((0x00000100U 
                                                              & (vlSelfRef.top__DOT__vgc_inst__DOT__regs[18U] 
                                                                 << 8U)) 
                                                             | vlSelfRef.top__DOT__vgc_inst__DOT__regs[17U]);
                                                        __Vdly__top__DOT__vgc_inst__DOT__cmd_busy = 1U;
                                                        vlSelfRef.top__DOT__vgc_inst__DOT__cmd_op 
                                                            = vlSelfRef.top__DOT__cpu_dout;
                                                        __Vdly__top__DOT__vgc_inst__DOT__cmd_cy 
                                                            = vlSelfRef.top__DOT__vgc_inst__DOT__regs[19U];
                                                    }
                                                } else if (
                                                           (1U 
                                                            & (IData)(vlSelfRef.top__DOT__cpu_dout))) {
                                                    __Vdly__top__DOT__vgc_inst__DOT__cmd_cx 
                                                        = 
                                                        ((0x00000100U 
                                                          & (vlSelfRef.top__DOT__vgc_inst__DOT__regs[18U] 
                                                             << 8U)) 
                                                         | vlSelfRef.top__DOT__vgc_inst__DOT__regs[17U]);
                                                    __Vdly__top__DOT__vgc_inst__DOT__cmd_busy = 1U;
                                                    vlSelfRef.top__DOT__vgc_inst__DOT__cmd_op 
                                                        = vlSelfRef.top__DOT__cpu_dout;
                                                    __Vdly__top__DOT__vgc_inst__DOT__cmd_cy 
                                                        = vlSelfRef.top__DOT__vgc_inst__DOT__regs[19U];
                                                } else {
                                                    __Vdly__top__DOT__vgc_inst__DOT__cmd_x 
                                                        = 
                                                        ((0x00000300U 
                                                          & (vlSelfRef.top__DOT__vgc_inst__DOT__regs[18U] 
                                                             << 8U)) 
                                                         | vlSelfRef.top__DOT__vgc_inst__DOT__regs[17U]);
                                                    __Vdly__top__DOT__vgc_inst__DOT__cmd_y 
                                                        = 
                                                        ((0x00000300U 
                                                          & (vlSelfRef.top__DOT__vgc_inst__DOT__regs[20U] 
                                                             << 8U)) 
                                                         | vlSelfRef.top__DOT__vgc_inst__DOT__regs[19U]);
                                                    __Vdly__top__DOT__vgc_inst__DOT__cmd_dx = 0U;
                                                    __Vdly__top__DOT__vgc_inst__DOT__cmd_busy = 1U;
                                                    vlSelfRef.top__DOT__vgc_inst__DOT__cmd_op = 4U;
                                                    vlSelfRef.top__DOT__vgc_inst__DOT__cmd_x2 
                                                        = 
                                                        ((0x00000300U 
                                                          & (vlSelfRef.top__DOT__vgc_inst__DOT__regs[22U] 
                                                             << 8U)) 
                                                         | vlSelfRef.top__DOT__vgc_inst__DOT__regs[21U]);
                                                    __Vdly__top__DOT__vgc_inst__DOT__cmd_dy 
                                                        = 
                                                        ((0x00000300U 
                                                          & (vlSelfRef.top__DOT__vgc_inst__DOT__regs[22U] 
                                                             << 8U)) 
                                                         | vlSelfRef.top__DOT__vgc_inst__DOT__regs[21U]);
                                                    __Vdly__top__DOT__vgc_inst__DOT__cmd_err 
                                                        = 
                                                        (0x000003ffU 
                                                         & ((IData)(1U) 
                                                            - 
                                                            ((0x00000300U 
                                                              & (vlSelfRef.top__DOT__vgc_inst__DOT__regs[22U] 
                                                                 << 8U)) 
                                                             | vlSelfRef.top__DOT__vgc_inst__DOT__regs[21U])));
                                                }
                                            } else if (
                                                       (2U 
                                                        & (IData)(vlSelfRef.top__DOT__cpu_dout))) {
                                                if (
                                                    (1U 
                                                     & (IData)(vlSelfRef.top__DOT__cpu_dout))) {
                                                    if (
                                                        (((0x00000100U 
                                                           & (vlSelfRef.top__DOT__vgc_inst__DOT__regs[22U] 
                                                              << 8U)) 
                                                          | vlSelfRef.top__DOT__vgc_inst__DOT__regs[21U]) 
                                                         >= 
                                                         ((0x00000100U 
                                                           & (vlSelfRef.top__DOT__vgc_inst__DOT__regs[18U] 
                                                              << 8U)) 
                                                          | vlSelfRef.top__DOT__vgc_inst__DOT__regs[17U]))) {
                                                        __Vdly__top__DOT__vgc_inst__DOT__cmd_dx 
                                                            = 
                                                            (0x000003ffU 
                                                             & (((0x00000100U 
                                                                  & (vlSelfRef.top__DOT__vgc_inst__DOT__regs[22U] 
                                                                     << 8U)) 
                                                                 | vlSelfRef.top__DOT__vgc_inst__DOT__regs[21U]) 
                                                                - 
                                                                ((0x00000100U 
                                                                  & (vlSelfRef.top__DOT__vgc_inst__DOT__regs[18U] 
                                                                     << 8U)) 
                                                                 | vlSelfRef.top__DOT__vgc_inst__DOT__regs[17U])));
                                                        vlSelfRef.top__DOT__vgc_inst__DOT__cmd_sx = 1U;
                                                    } else {
                                                        __Vdly__top__DOT__vgc_inst__DOT__cmd_dx 
                                                            = 
                                                            (0x000003ffU 
                                                             & (((0x00000100U 
                                                                  & (vlSelfRef.top__DOT__vgc_inst__DOT__regs[18U] 
                                                                     << 8U)) 
                                                                 | vlSelfRef.top__DOT__vgc_inst__DOT__regs[17U]) 
                                                                - 
                                                                ((0x00000100U 
                                                                  & (vlSelfRef.top__DOT__vgc_inst__DOT__regs[22U] 
                                                                     << 8U)) 
                                                                 | vlSelfRef.top__DOT__vgc_inst__DOT__regs[21U])));
                                                        vlSelfRef.top__DOT__vgc_inst__DOT__cmd_sx = 0x03ffU;
                                                    }
                                                    __Vdly__top__DOT__vgc_inst__DOT__cmd_busy = 1U;
                                                    vlSelfRef.top__DOT__vgc_inst__DOT__cmd_op = 3U;
                                                    if (
                                                        (vlSelfRef.top__DOT__vgc_inst__DOT__regs[23U] 
                                                         >= vlSelfRef.top__DOT__vgc_inst__DOT__regs[19U])) {
                                                        __Vdly__top__DOT__vgc_inst__DOT__cmd_dy 
                                                            = 
                                                            (0x000003ffU 
                                                             & (- 
                                                                (vlSelfRef.top__DOT__vgc_inst__DOT__regs[23U] 
                                                                 - vlSelfRef.top__DOT__vgc_inst__DOT__regs[19U])));
                                                        vlSelfRef.top__DOT__vgc_inst__DOT__cmd_sy = 1U;
                                                    } else {
                                                        __Vdly__top__DOT__vgc_inst__DOT__cmd_dy 
                                                            = 
                                                            (0x000003ffU 
                                                             & (vlSelfRef.top__DOT__vgc_inst__DOT__regs[19U] 
                                                                - vlSelfRef.top__DOT__vgc_inst__DOT__regs[23U]));
                                                        vlSelfRef.top__DOT__vgc_inst__DOT__cmd_sy = 0x03ffU;
                                                    }
                                                    __Vdly__top__DOT__vgc_inst__DOT__cmd_err 
                                                        = 
                                                        (0x000003ffU 
                                                         & (((((0x00000100U 
                                                                & (vlSelfRef.top__DOT__vgc_inst__DOT__regs[22U] 
                                                                   << 8U)) 
                                                               | vlSelfRef.top__DOT__vgc_inst__DOT__regs[21U]) 
                                                              >= 
                                                              ((0x00000100U 
                                                                & (vlSelfRef.top__DOT__vgc_inst__DOT__regs[18U] 
                                                                   << 8U)) 
                                                               | vlSelfRef.top__DOT__vgc_inst__DOT__regs[17U]))
                                                              ? 
                                                             (((0x00000100U 
                                                                & (vlSelfRef.top__DOT__vgc_inst__DOT__regs[22U] 
                                                                   << 8U)) 
                                                               | vlSelfRef.top__DOT__vgc_inst__DOT__regs[21U]) 
                                                              - 
                                                              ((0x00000100U 
                                                                & (vlSelfRef.top__DOT__vgc_inst__DOT__regs[18U] 
                                                                   << 8U)) 
                                                               | vlSelfRef.top__DOT__vgc_inst__DOT__regs[17U]))
                                                              : 
                                                             (((0x00000100U 
                                                                & (vlSelfRef.top__DOT__vgc_inst__DOT__regs[18U] 
                                                                   << 8U)) 
                                                               | vlSelfRef.top__DOT__vgc_inst__DOT__regs[17U]) 
                                                              - 
                                                              ((0x00000100U 
                                                                & (vlSelfRef.top__DOT__vgc_inst__DOT__regs[22U] 
                                                                   << 8U)) 
                                                               | vlSelfRef.top__DOT__vgc_inst__DOT__regs[21U]))) 
                                                            + 
                                                            ((vlSelfRef.top__DOT__vgc_inst__DOT__regs[23U] 
                                                              >= vlSelfRef.top__DOT__vgc_inst__DOT__regs[19U])
                                                              ? 
                                                             (- 
                                                              (vlSelfRef.top__DOT__vgc_inst__DOT__regs[23U] 
                                                               - vlSelfRef.top__DOT__vgc_inst__DOT__regs[19U]))
                                                              : 
                                                             (vlSelfRef.top__DOT__vgc_inst__DOT__regs[19U] 
                                                              - vlSelfRef.top__DOT__vgc_inst__DOT__regs[23U]))));
                                                } else if (
                                                           ((0x0140U 
                                                             > 
                                                             ((0x00000100U 
                                                               & (vlSelfRef.top__DOT__vgc_inst__DOT__regs[18U] 
                                                                  << 8U)) 
                                                              | vlSelfRef.top__DOT__vgc_inst__DOT__regs[17U])) 
                                                            & (0xc8U 
                                                               > vlSelfRef.top__DOT__vgc_inst__DOT__regs[19U]))) {
                                                    vlSelfRef.top__DOT__vgc_inst__DOT____Vlvbound_hde0d773d__1 = 0U;
                                                    if (VL_LIKELY((
                                                                   (0xf9ffU 
                                                                    >= 
                                                                    (0x0000ffffU 
                                                                     & (((IData)(0x00000140U) 
                                                                         * vlSelfRef.top__DOT__vgc_inst__DOT__regs[19U]) 
                                                                        + 
                                                                        ((0x00000100U 
                                                                          & (vlSelfRef.top__DOT__vgc_inst__DOT__regs[18U] 
                                                                             << 8U)) 
                                                                         | vlSelfRef.top__DOT__vgc_inst__DOT__regs[17U]))))))) {
                                                        __VdlyVal__top__DOT__vgc_inst__DOT__gfx_ram__v16 
                                                            = vlSelfRef.top__DOT__vgc_inst__DOT____Vlvbound_hde0d773d__1;
                                                        __VdlyDim0__top__DOT__vgc_inst__DOT__gfx_ram__v16 
                                                            = 
                                                            (0x0000ffffU 
                                                             & (((IData)(0x00000140U) 
                                                                 * vlSelfRef.top__DOT__vgc_inst__DOT__regs[19U]) 
                                                                + 
                                                                ((0x00000100U 
                                                                  & (vlSelfRef.top__DOT__vgc_inst__DOT__regs[18U] 
                                                                     << 8U)) 
                                                                 | vlSelfRef.top__DOT__vgc_inst__DOT__regs[17U])));
                                                        vlSelfRef.__VdlyCommitQueuetop__DOT__vgc_inst__DOT__gfx_ram.enqueue(__VdlyVal__top__DOT__vgc_inst__DOT__gfx_ram__v16, (IData)(__VdlyDim0__top__DOT__vgc_inst__DOT__gfx_ram__v16));
                                                    }
                                                }
                                            } else if (
                                                       (1U 
                                                        & (IData)(vlSelfRef.top__DOT__cpu_dout))) {
                                                if (
                                                    ((0x0140U 
                                                      > 
                                                      ((0x00000100U 
                                                        & (vlSelfRef.top__DOT__vgc_inst__DOT__regs[18U] 
                                                           << 8U)) 
                                                       | vlSelfRef.top__DOT__vgc_inst__DOT__regs[17U])) 
                                                     & (0xc8U 
                                                        > vlSelfRef.top__DOT__vgc_inst__DOT__regs[19U]))) {
                                                    vlSelfRef.top__DOT__vgc_inst__DOT____Vlvbound_hde0d773d__0 
                                                        = vlSelfRef.top__DOT__vgc_inst__DOT__gfx_color;
                                                    if (VL_LIKELY((
                                                                   (0xf9ffU 
                                                                    >= 
                                                                    (0x0000ffffU 
                                                                     & (((IData)(0x00000140U) 
                                                                         * vlSelfRef.top__DOT__vgc_inst__DOT__regs[19U]) 
                                                                        + 
                                                                        ((0x00000100U 
                                                                          & (vlSelfRef.top__DOT__vgc_inst__DOT__regs[18U] 
                                                                             << 8U)) 
                                                                         | vlSelfRef.top__DOT__vgc_inst__DOT__regs[17U]))))))) {
                                                        __VdlyVal__top__DOT__vgc_inst__DOT__gfx_ram__v17 
                                                            = vlSelfRef.top__DOT__vgc_inst__DOT____Vlvbound_hde0d773d__0;
                                                        __VdlyDim0__top__DOT__vgc_inst__DOT__gfx_ram__v17 
                                                            = 
                                                            (0x0000ffffU 
                                                             & (((IData)(0x00000140U) 
                                                                 * vlSelfRef.top__DOT__vgc_inst__DOT__regs[19U]) 
                                                                + 
                                                                ((0x00000100U 
                                                                  & (vlSelfRef.top__DOT__vgc_inst__DOT__regs[18U] 
                                                                     << 8U)) 
                                                                 | vlSelfRef.top__DOT__vgc_inst__DOT__regs[17U])));
                                                        vlSelfRef.__VdlyCommitQueuetop__DOT__vgc_inst__DOT__gfx_ram.enqueue(__VdlyVal__top__DOT__vgc_inst__DOT__gfx_ram__v17, (IData)(__VdlyDim0__top__DOT__vgc_inst__DOT__gfx_ram__v17));
                                                    }
                                                }
                                            }
                                        }
                                    }
                                }
                            }
                        }
                    }
                } else {
                    __VdlyVal__top__DOT__vgc_inst__DOT__regs__v37 
                        = vlSelfRef.top__DOT__cpu_dout;
                    __VdlyDim0__top__DOT__vgc_inst__DOT__regs__v37 
                        = (0x0000001fU & (IData)(vlSelfRef.top__DOT__cpu_addr));
                    __VdlySet__top__DOT__vgc_inst__DOT__regs__v37 = 1U;
                }
            }
            if (vlSelfRef.top__DOT__vgc_inst__DOT__char_ram_sel) {
                vlSelfRef.top__DOT__vgc_inst__DOT____Vlvbound_h87c37a99__0 
                    = vlSelfRef.top__DOT__cpu_dout;
                if (VL_LIKELY(((0x07cfU >= (0x000007ffU 
                                            & ((IData)(vlSelfRef.top__DOT__cpu_addr) 
                                               - (IData)(0x0200U))))))) {
                    __VdlyVal__top__DOT__vgc_inst__DOT__char_ram__v6 
                        = vlSelfRef.top__DOT__vgc_inst__DOT____Vlvbound_h87c37a99__0;
                    __VdlyDim0__top__DOT__vgc_inst__DOT__char_ram__v6 
                        = (0x000007ffU & ((IData)(vlSelfRef.top__DOT__cpu_addr) 
                                          - (IData)(0x0200U)));
                    vlSelfRef.__VdlyCommitQueuetop__DOT__vgc_inst__DOT__char_ram.enqueue(__VdlyVal__top__DOT__vgc_inst__DOT__char_ram__v6, (IData)(__VdlyDim0__top__DOT__vgc_inst__DOT__char_ram__v6));
                }
            }
            if ((0xb9a3U == (IData)(vlSelfRef.top__DOT__cpu_addr))) {
                vlSelfRef.top__DOT__vgc_inst__DOT__fio_name_len 
                    = (0x0000003fU & (IData)(vlSelfRef.top__DOT__cpu_dout));
            }
            if (((0xb9b0U <= (IData)(vlSelfRef.top__DOT__cpu_addr)) 
                 & (0xb9efU >= (IData)(vlSelfRef.top__DOT__cpu_addr)))) {
                __VdlyVal__top__DOT__vgc_inst__DOT__fio_name__v0 
                    = vlSelfRef.top__DOT__cpu_dout;
                __VdlyDim0__top__DOT__vgc_inst__DOT__fio_name__v0 
                    = (0x0000003fU & ((IData)(vlSelfRef.top__DOT__cpu_addr) 
                                      - (IData)(0x30U)));
                __VdlySet__top__DOT__vgc_inst__DOT__fio_name__v0 = 1U;
            }
            if (vlSelfRef.top__DOT__vgc_inst__DOT__color_ram_sel) {
                vlSelfRef.top__DOT__vgc_inst__DOT____Vlvbound_hced7a248__0 
                    = vlSelfRef.top__DOT__cpu_dout;
                if (VL_LIKELY(((0x07cfU >= (0x000007ffU 
                                            & ((IData)(vlSelfRef.top__DOT__cpu_addr) 
                                               - (IData)(0x01d0U))))))) {
                    __VdlyVal__top__DOT__vgc_inst__DOT__color_ram__v5 
                        = vlSelfRef.top__DOT__vgc_inst__DOT____Vlvbound_hced7a248__0;
                    __VdlyDim0__top__DOT__vgc_inst__DOT__color_ram__v5 
                        = (0x000007ffU & ((IData)(vlSelfRef.top__DOT__cpu_addr) 
                                          - (IData)(0x01d0U)));
                    vlSelfRef.__VdlyCommitQueuetop__DOT__vgc_inst__DOT__color_ram.enqueue(__VdlyVal__top__DOT__vgc_inst__DOT__color_ram__v5, (IData)(__VdlyDim0__top__DOT__vgc_inst__DOT__color_ram__v5));
                }
            }
            if (((0xa040U <= (IData)(vlSelfRef.top__DOT__cpu_addr)) 
                 & (0xa0bfU >= (IData)(vlSelfRef.top__DOT__cpu_addr)))) {
                if ((4U & (IData)(vlSelfRef.top__DOT__cpu_addr))) {
                    if ((2U & (IData)(vlSelfRef.top__DOT__cpu_addr))) {
                        if ((1U & (IData)(vlSelfRef.top__DOT__cpu_addr))) {
                            __VdlyVal__top__DOT__vgc_inst__DOT__spr_trans__v16 
                                = (0x0000000fU & (IData)(vlSelfRef.top__DOT__cpu_dout));
                            __VdlyDim0__top__DOT__vgc_inst__DOT__spr_trans__v16 
                                = (0x0000000fU & ((IData)(vlSelfRef.top__DOT__cpu_addr) 
                                                  >> 3U));
                            __VdlySet__top__DOT__vgc_inst__DOT__spr_trans__v16 = 1U;
                        } else {
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
                    } else {
                        __VdlyVal__top__DOT__vgc_inst__DOT__spr_shape__v16 
                            = (0x0000000fU & (IData)(vlSelfRef.top__DOT__cpu_dout));
                        __VdlyDim0__top__DOT__vgc_inst__DOT__spr_shape__v16 
                            = (0x0000000fU & ((IData)(vlSelfRef.top__DOT__cpu_addr) 
                                              >> 3U));
                        __VdlySet__top__DOT__vgc_inst__DOT__spr_shape__v16 = 1U;
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
    }
    vlSelfRef.top__DOT__blt_ram_addr = 0U;
    vlSelfRef.top__DOT__blt_xram_addr = 0U;
    if (((2U == (IData)(vlSelfRef.top__DOT__blt_inst__DOT__state)) 
         | (4U == (IData)(vlSelfRef.top__DOT__blt_inst__DOT__state)))) {
        vlSelfRef.top__DOT__blt_vgc_addr = 0U;
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
    } else {
        vlSelfRef.top__DOT__blt_vgc_addr = 0U;
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
    vlSelfRef.top__DOT__vgc_inst__DOT__gfx_color = __Vdly__top__DOT__vgc_inst__DOT__gfx_color;
    vlSelfRef.top__DOT__vgc_inst__DOT__scroll_pending 
        = __Vdly__top__DOT__vgc_inst__DOT__scroll_pending;
    vlSelfRef.top__DOT__vgc_inst__DOT__scroll_col = __Vdly__top__DOT__vgc_inst__DOT__scroll_col;
    vlSelfRef.top__DOT__vgc_inst__DOT__cmd_busy = __Vdly__top__DOT__vgc_inst__DOT__cmd_busy;
    vlSelfRef.top__DOT__vgc_inst__DOT__copper_target_list 
        = __Vdly__top__DOT__vgc_inst__DOT__copper_target_list;
    vlSelfRef.top__DOT__vgc_inst__DOT__fill_sp = __Vdly__top__DOT__vgc_inst__DOT__fill_sp;
    vlSelfRef.top__DOT__vgc_inst__DOT__cmd_cx = __Vdly__top__DOT__vgc_inst__DOT__cmd_cx;
    vlSelfRef.top__DOT__vgc_inst__DOT__cmd_cy = __Vdly__top__DOT__vgc_inst__DOT__cmd_cy;
    vlSelfRef.top__DOT__vgc_inst__DOT__gt_font_row 
        = __Vdly__top__DOT__vgc_inst__DOT__gt_font_row;
    vlSelfRef.top__DOT__vgc_inst__DOT__gt_char_idx 
        = __Vdly__top__DOT__vgc_inst__DOT__gt_char_idx;
    vlSelfRef.top__DOT__vgc_inst__DOT__cmd_dx = __Vdly__top__DOT__vgc_inst__DOT__cmd_dx;
    vlSelfRef.top__DOT__vgc_inst__DOT__cmd_err = __Vdly__top__DOT__vgc_inst__DOT__cmd_err;
    vlSelfRef.top__DOT__vgc_inst__DOT__cmd_dy = __Vdly__top__DOT__vgc_inst__DOT__cmd_dy;
    vlSelfRef.top__DOT__vgc_inst__DOT__cmd_x = __Vdly__top__DOT__vgc_inst__DOT__cmd_x;
    vlSelfRef.top__DOT__vgc_inst__DOT__cmd_y = __Vdly__top__DOT__vgc_inst__DOT__cmd_y;
    vlSelfRef.__VdlyCommitQueuetop__DOT__vgc_inst__DOT__copper_list_count.commit(vlSelfRef.top__DOT__vgc_inst__DOT__copper_list_count);
    vlSelfRef.top__DOT__vgc_inst__DOT__copper_count 
        = __Vdly__top__DOT__vgc_inst__DOT__copper_count;
    vlSelfRef.top__DOT__vgc_inst__DOT__copper_index 
        = __Vdly__top__DOT__vgc_inst__DOT__copper_index;
    if (__VdlySet__top__DOT__vgc_inst__DOT__fio_name__v0) {
        vlSelfRef.top__DOT__vgc_inst__DOT__fio_name[__VdlyDim0__top__DOT__vgc_inst__DOT__fio_name__v0] 
            = __VdlyVal__top__DOT__vgc_inst__DOT__fio_name__v0;
    }
    if (__VdlySet__top__DOT__vgc_inst__DOT__fill_stack__v0) {
        vlSelfRef.top__DOT__vgc_inst__DOT__fill_stack[__VdlyDim0__top__DOT__vgc_inst__DOT__fill_stack__v0] 
            = __VdlyVal__top__DOT__vgc_inst__DOT__fill_stack__v0;
        vlSelfRef.top__DOT__vgc_inst__DOT__fill_stack[__VdlyDim0__top__DOT__vgc_inst__DOT__fill_stack__v1] 
            = __VdlyVal__top__DOT__vgc_inst__DOT__fill_stack__v1;
        vlSelfRef.top__DOT__vgc_inst__DOT__fill_stack[__VdlyDim0__top__DOT__vgc_inst__DOT__fill_stack__v2] 
            = __VdlyVal__top__DOT__vgc_inst__DOT__fill_stack__v2;
        vlSelfRef.top__DOT__vgc_inst__DOT__fill_stack[__VdlyDim0__top__DOT__vgc_inst__DOT__fill_stack__v3] 
            = __VdlyVal__top__DOT__vgc_inst__DOT__fill_stack__v3;
    }
    if (__VdlySet__top__DOT__vgc_inst__DOT__fill_stack__v4) {
        vlSelfRef.top__DOT__vgc_inst__DOT__fill_stack[0U] 
            = __VdlyVal__top__DOT__vgc_inst__DOT__fill_stack__v4;
    }
    if (__VdlySet__top__DOT__vgc_inst__DOT__copper_list_pos__v0) {
        vlSelfRef.top__DOT__vgc_inst__DOT__copper_list_pos[__VdlyDim0__top__DOT__vgc_inst__DOT__copper_list_pos__v0] 
            = __VdlyVal__top__DOT__vgc_inst__DOT__copper_list_pos__v0;
        vlSelfRef.top__DOT__vgc_inst__DOT__copper_list_reg[__VdlyDim0__top__DOT__vgc_inst__DOT__copper_list_reg__v0] 
            = __VdlyVal__top__DOT__vgc_inst__DOT__copper_list_reg__v0;
        vlSelfRef.top__DOT__vgc_inst__DOT__copper_list_val[__VdlyDim0__top__DOT__vgc_inst__DOT__copper_list_val__v0] 
            = __VdlyVal__top__DOT__vgc_inst__DOT__copper_list_val__v0;
    }
    if (__VdlySet__top__DOT__vgc_inst__DOT__copper_pos__v0) {
        vlSelfRef.top__DOT__vgc_inst__DOT__copper_pos[__VdlyDim0__top__DOT__vgc_inst__DOT__copper_pos__v0] 
            = __VdlyVal__top__DOT__vgc_inst__DOT__copper_pos__v0;
        vlSelfRef.top__DOT__vgc_inst__DOT__copper_reg[__VdlyDim0__top__DOT__vgc_inst__DOT__copper_reg__v0] 
            = __VdlyVal__top__DOT__vgc_inst__DOT__copper_reg__v0;
        vlSelfRef.top__DOT__vgc_inst__DOT__copper_val[__VdlyDim0__top__DOT__vgc_inst__DOT__copper_val__v0] 
            = __VdlyVal__top__DOT__vgc_inst__DOT__copper_val__v0;
    }
    if (__VdlySet__top__DOT__vgc_inst__DOT__copper_pos__v1) {
        vlSelfRef.top__DOT__vgc_inst__DOT__copper_pos[__VdlyDim0__top__DOT__vgc_inst__DOT__copper_pos__v1] 
            = __VdlyVal__top__DOT__vgc_inst__DOT__copper_pos__v1;
        vlSelfRef.top__DOT__vgc_inst__DOT__copper_reg[__VdlyDim0__top__DOT__vgc_inst__DOT__copper_reg__v1] 
            = __VdlyVal__top__DOT__vgc_inst__DOT__copper_reg__v1;
        vlSelfRef.top__DOT__vgc_inst__DOT__copper_val[__VdlyDim0__top__DOT__vgc_inst__DOT__copper_val__v1] 
            = __VdlyVal__top__DOT__vgc_inst__DOT__copper_val__v1;
    }
    vlSelfRef.top__DOT__vgc_inst__DOT__fg_color = __Vdly__top__DOT__vgc_inst__DOT__fg_color;
    vlSelfRef.top__DOT__vgc_inst__DOT__cursor_x = __Vdly__top__DOT__vgc_inst__DOT__cursor_x;
    vlSelfRef.top__DOT__vgc_inst__DOT__cursor_y = __Vdly__top__DOT__vgc_inst__DOT__cursor_y;
    vlSelfRef.__VdlyCommitQueuetop__DOT__vgc_inst__DOT__gfx_ram.commit(vlSelfRef.top__DOT__vgc_inst__DOT__gfx_ram);
    vlSelfRef.__VdlyCommitQueuetop__DOT__vgc_inst__DOT__sprite_shapes.commit(vlSelfRef.top__DOT__vgc_inst__DOT__sprite_shapes);
    vlSelfRef.__VdlyCommitQueuetop__DOT__vgc_inst__DOT__char_ram.commit(vlSelfRef.top__DOT__vgc_inst__DOT__char_ram);
    vlSelfRef.__VdlyCommitQueuetop__DOT__vgc_inst__DOT__color_ram.commit(vlSelfRef.top__DOT__vgc_inst__DOT__color_ram);
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
    if ((((0x0cU == (IData)(vlSelfRef.top__DOT__cpu_inst__DOT__state)) 
          | (8U == (IData)(vlSelfRef.top__DOT__cpu_inst__DOT__state))) 
         & (IData)(vlSelfRef.top__DOT____Vcellinp__cpu_inst__RDY))) {
        vlSelfRef.top__DOT__cpu_inst__DOT__adc_sbc 
            = (0x61U == (0x63U & (IData)(vlSelfRef.top__DOT__cpu_inst__DOT__IR)));
    }
    vlSelfRef.top__DOT__vgc_inst__DOT__scroll_offset 
        = __Vdly__top__DOT__vgc_inst__DOT__scroll_offset;
    if (__VdlySet__top__DOT__vgc_inst__DOT__tile_inst__DOT__tile_data__v0) {
        vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__tile_data[__VdlyDim0__top__DOT__vgc_inst__DOT__tile_inst__DOT__tile_data__v0] 
            = __VdlyVal__top__DOT__vgc_inst__DOT__tile_inst__DOT__tile_data__v0;
    }
    if (__VdlySet__top__DOT__vgc_inst__DOT__tile_inst__DOT__tile_data__v1) {
        vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__tile_data[__VdlyDim0__top__DOT__vgc_inst__DOT__tile_inst__DOT__tile_data__v1] 
            = __VdlyVal__top__DOT__vgc_inst__DOT__tile_inst__DOT__tile_data__v1;
    }
    if (__VdlySet__top__DOT__vgc_inst__DOT__tile_inst__DOT__tile_data__v2) {
        vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__tile_data[__VdlyDim0__top__DOT__vgc_inst__DOT__tile_inst__DOT__tile_data__v2] 
            = __VdlyVal__top__DOT__vgc_inst__DOT__tile_inst__DOT__tile_data__v2;
    }
    if (__VdlySet__top__DOT__vgc_inst__DOT__regs__v0) {
        vlSelfRef.top__DOT__vgc_inst__DOT__regs[0U] = 0U;
        vlSelfRef.top__DOT__vgc_inst__DOT__spr_shape[0U] = 0U;
        vlSelfRef.top__DOT__vgc_inst__DOT__spr_trans[0U] = 0U;
        vlSelfRef.top__DOT__vgc_inst__DOT__spr_y[0U] = 0U;
        vlSelfRef.top__DOT__vgc_inst__DOT__spr_flip_h[0U] = 0U;
        vlSelfRef.top__DOT__vgc_inst__DOT__spr_flip_v[0U] = 0U;
        vlSelfRef.top__DOT__vgc_inst__DOT__spr_pri[0U] = 0U;
        vlSelfRef.top__DOT__vgc_inst__DOT__spr_enable[0U] = 0U;
        vlSelfRef.top__DOT__vgc_inst__DOT__spr_x[0U] = 0U;
    }
    if (__VdlySet__top__DOT__vgc_inst__DOT__regs__v1) {
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
        vlSelfRef.top__DOT__vgc_inst__DOT__regs[20U] 
            = __VdlyVal__top__DOT__vgc_inst__DOT__regs__v34;
    }
    if (__VdlySet__top__DOT__vgc_inst__DOT__regs__v35) {
        vlSelfRef.top__DOT__vgc_inst__DOT__regs[20U] 
            = __VdlyVal__top__DOT__vgc_inst__DOT__regs__v35;
    }
    if (__VdlySet__top__DOT__vgc_inst__DOT__regs__v36) {
        vlSelfRef.top__DOT__vgc_inst__DOT__regs[20U] 
            = __VdlyVal__top__DOT__vgc_inst__DOT__regs__v36;
    }
    if (__VdlySet__top__DOT__vgc_inst__DOT__regs__v37) {
        vlSelfRef.top__DOT__vgc_inst__DOT__regs[__VdlyDim0__top__DOT__vgc_inst__DOT__regs__v37] 
            = __VdlyVal__top__DOT__vgc_inst__DOT__regs__v37;
    }
    if (__VdlySet__top__DOT__vgc_inst__DOT__regs__v38) {
        vlSelfRef.top__DOT__vgc_inst__DOT__regs[__VdlyDim0__top__DOT__vgc_inst__DOT__regs__v38] 
            = __VdlyVal__top__DOT__vgc_inst__DOT__regs__v38;
    }
    if (__VdlySet__top__DOT__vgc_inst__DOT__spr_shape__v16) {
        vlSelfRef.top__DOT__vgc_inst__DOT__spr_shape[__VdlyDim0__top__DOT__vgc_inst__DOT__spr_shape__v16] 
            = __VdlyVal__top__DOT__vgc_inst__DOT__spr_shape__v16;
    }
    if (__VdlySet__top__DOT__vgc_inst__DOT__spr_trans__v16) {
        vlSelfRef.top__DOT__vgc_inst__DOT__spr_trans[__VdlyDim0__top__DOT__vgc_inst__DOT__spr_trans__v16] 
            = __VdlyVal__top__DOT__vgc_inst__DOT__spr_trans__v16;
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
    if (vlSelfRef.rst) {
        vlSelfRef.top__DOT__ext_rom_active = 0U;
        __Vdly__top__DOT__vgc_inst__DOT__copper_active_list = 0U;
    } else if (((0U == (IData)(vlSelfRef.top__DOT__vgc_inst__DOT__h_count)) 
                & (0x01e0U == (IData)(vlSelfRef.top__DOT__vgc_inst__DOT__v_count)))) {
        if ((((IData)(vlSelfRef.top__DOT__vgc_inst__DOT__copper_pending_list) 
              != (IData)(vlSelfRef.top__DOT__vgc_inst__DOT__copper_active_list)) 
             & (~ (IData)(vlSelfRef.top__DOT__vgc_inst__DOT__copper_loading)))) {
            __Vdly__top__DOT__vgc_inst__DOT__copper_active_list 
                = vlSelfRef.top__DOT__vgc_inst__DOT__copper_pending_list;
            __Vdly__top__DOT__vgc_inst__DOT__copper_loading = 1U;
            vlSelfRef.top__DOT__vgc_inst__DOT__copper_load_idx = 0U;
            vlSelfRef.top__DOT__vgc_inst__DOT__copper_load_src 
                = vlSelfRef.top__DOT__vgc_inst__DOT__copper_pending_list;
        }
    }
    top__DOT__blt_vgc_rdata = 0U;
    if (top__DOT__blt_vgc_re) {
        top__DOT__blt_vgc_rdata = ((4U & (IData)(vlSelfRef.top__DOT__blt_vgc_space))
                                    ? ((2U & (IData)(vlSelfRef.top__DOT__blt_vgc_space))
                                        ? ((1U & (IData)(vlSelfRef.top__DOT__blt_vgc_space))
                                            ? 0U : vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__tile_data
                                           [(0x00007fffU 
                                             & (IData)(vlSelfRef.top__DOT__blt_vgc_addr))])
                                        : ((1U & (IData)(vlSelfRef.top__DOT__blt_vgc_space))
                                            ? 0U : vlSelfRef.top__DOT__vgc_inst__DOT__sprite_shapes
                                           [(0x000007ffU 
                                             & (IData)(vlSelfRef.top__DOT__blt_vgc_addr))]))
                                    : ((2U & (IData)(vlSelfRef.top__DOT__blt_vgc_space))
                                        ? ((1U & (IData)(vlSelfRef.top__DOT__blt_vgc_space))
                                            ? ((0xf9ffU 
                                                >= (IData)(vlSelfRef.top__DOT__blt_vgc_addr))
                                                ? vlSelfRef.top__DOT__vgc_inst__DOT__gfx_ram
                                               [vlSelfRef.top__DOT__blt_vgc_addr]
                                                : 0U)
                                            : ((0x07cfU 
                                                >= 
                                                (0x000007ffU 
                                                 & (IData)(vlSelfRef.top__DOT__blt_vgc_addr)))
                                                ? vlSelfRef.top__DOT__vgc_inst__DOT__color_ram
                                               [(0x000007ffU 
                                                 & (IData)(vlSelfRef.top__DOT__blt_vgc_addr))]
                                                : 0U))
                                        : ((1U & (IData)(vlSelfRef.top__DOT__blt_vgc_space))
                                            ? ((0x07cfU 
                                                >= 
                                                (0x000007ffU 
                                                 & (IData)(vlSelfRef.top__DOT__blt_vgc_addr)))
                                                ? vlSelfRef.top__DOT__vgc_inst__DOT__char_ram
                                               [(0x000007ffU 
                                                 & (IData)(vlSelfRef.top__DOT__blt_vgc_addr))]
                                                : 0U)
                                            : 0U)));
    }
    top__DOT__vgc_dbg_rdata = 0U;
    if (vlSelfRef.top__DOT__vgc_inst__DOT__dbg_char_sel) {
        top__DOT__vgc_dbg_rdata = ((0x07cfU >= (0x000007ffU 
                                                & ((IData)(vlSelfRef.dbg_peek_addr) 
                                                   - (IData)(0x0200U))))
                                    ? vlSelfRef.top__DOT__vgc_inst__DOT__char_ram
                                   [(0x000007ffU & 
                                     ((IData)(vlSelfRef.dbg_peek_addr) 
                                      - (IData)(0x0200U)))]
                                    : 0U);
    } else if (vlSelfRef.top__DOT__vgc_inst__DOT__dbg_color_sel) {
        top__DOT__vgc_dbg_rdata = ((0x07cfU >= (0x000007ffU 
                                                & ((IData)(vlSelfRef.dbg_peek_addr) 
                                                   - (IData)(0x01d0U))))
                                    ? vlSelfRef.top__DOT__vgc_inst__DOT__color_ram
                                   [(0x000007ffU & 
                                     ((IData)(vlSelfRef.dbg_peek_addr) 
                                      - (IData)(0x01d0U)))]
                                    : 0U);
    } else if (vlSelfRef.top__DOT__vgc_inst__DOT__dbg_vgc_sel) {
        top__DOT__vgc_dbg_rdata = ((0U == (0x0000001fU 
                                           & (IData)(vlSelfRef.dbg_peek_addr)))
                                    ? (IData)(vlSelfRef.top__DOT__vgc_inst__DOT__mode)
                                    : ((1U == (0x0000001fU 
                                               & (IData)(vlSelfRef.dbg_peek_addr)))
                                        ? (IData)(vlSelfRef.top__DOT__vgc_inst__DOT__bg_color)
                                        : ((2U == (0x0000001fU 
                                                   & (IData)(vlSelfRef.dbg_peek_addr)))
                                            ? (IData)(vlSelfRef.top__DOT__vgc_inst__DOT__fg_color)
                                            : ((3U 
                                                == 
                                                (0x0000001fU 
                                                 & (IData)(vlSelfRef.dbg_peek_addr)))
                                                ? (IData)(vlSelfRef.top__DOT__vgc_inst__DOT__cursor_x)
                                                : (
                                                   (4U 
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
    vlSelfRef.top__DOT__blt_inst__DOT__mem_read_data 
        = ((0U == (IData)(vlSelfRef.top__DOT__blt_inst__DOT__src_space))
            ? vlSelfRef.top__DOT__ram[vlSelfRef.top__DOT__blt_ram_addr]
            : ((5U == (IData)(vlSelfRef.top__DOT__blt_inst__DOT__src_space))
                ? vlSelfRef.top__DOT__xram[vlSelfRef.top__DOT__blt_xram_addr]
                : (IData)(top__DOT__blt_vgc_rdata)));
    vlSelfRef.top__DOT__vgc_inst__DOT__copper_pending_list 
        = __Vdly__top__DOT__vgc_inst__DOT__copper_pending_list;
    vlSelfRef.top__DOT__vgc_inst__DOT__copper_active_list 
        = __Vdly__top__DOT__vgc_inst__DOT__copper_active_list;
    vlSelfRef.top__DOT__vgc_inst__DOT__copper_loading 
        = __Vdly__top__DOT__vgc_inst__DOT__copper_loading;
    vlSelfRef.top__DOT__vgc_inst__DOT__h_count = __Vdly__top__DOT__vgc_inst__DOT__h_count;
    vlSelfRef.top__DOT__vgc_inst__DOT__v_count = __Vdly__top__DOT__vgc_inst__DOT__v_count;
    vlSelfRef.__VdfgRegularize_hebeb780c_0_19 = ((((IData)(vlSelfRef.top__DOT__cpu_inst__DOT__D) 
                                                   << 3U) 
                                                  | ((IData)(vlSelfRef.top__DOT__cpu_inst__DOT__I) 
                                                     << 2U)) 
                                                 | (((IData)(vlSelfRef.top__DOT__cpu_inst__DOT__Z) 
                                                     << 1U) 
                                                    | (IData)(vlSelfRef.top__DOT__cpu_inst__DOT__C)));
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
                                    ? (IData)(top__DOT__vgc_dbg_rdata)
                                    : ((0xc000U <= (IData)(vlSelfRef.dbg_peek_addr))
                                        ? ((IData)(vlSelfRef.top__DOT__ext_rom_active)
                                            ? vlSelfRef.top__DOT__ext_rom
                                           [(0x00003fffU 
                                             & (IData)(vlSelfRef.dbg_peek_addr))]
                                            : vlSelfRef.top__DOT__basic_rom
                                           [(0x00003fffU 
                                             & (IData)(vlSelfRef.dbg_peek_addr))])
                                        : vlSelfRef.top__DOT__ram
                                       [vlSelfRef.dbg_peek_addr]))
                                : 0U);
    top__DOT__vgc_inst__DOT__tile_inst__DOT____VdfgRegularize_hb33eefcd_0_1 
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
    top__DOT__vgc_inst__DOT__text_line = (0x000003ffU 
                                          & (((IData)(vlSelfRef.top__DOT__vgc_inst__DOT__v_count) 
                                              - (IData)(0x00000028U)) 
                                             >> 1U));
    top__DOT__vgc_inst__DOT__text_row = (0x0000001fU 
                                         & ((IData)(top__DOT__vgc_inst__DOT__text_line) 
                                            >> 3U));
    top__DOT__vgc_inst__DOT__font_line = (7U & (IData)(top__DOT__vgc_inst__DOT__text_line));
    top__DOT__vgc_inst__DOT__real_row = (0x0000001fU 
                                         & ((IData)(top__DOT__vgc_inst__DOT__text_row) 
                                            + (IData)(vlSelfRef.top__DOT__vgc_inst__DOT__scroll_offset)));
    if ((0x19U <= (IData)(top__DOT__vgc_inst__DOT__real_row))) {
        top__DOT__vgc_inst__DOT__real_row = (0x0000001fU 
                                             & ((IData)(top__DOT__vgc_inst__DOT__real_row) 
                                                - (IData)(0x19U)));
    }
    vlSelfRef.top__DOT__vgc_inst__DOT__gfx_y = (0x000000ffU 
                                                & (IData)(top__DOT__vgc_inst__DOT__text_line));
    vlSelfRef.top__DOT__vgc_inst__DOT__visible = ((0x0280U 
                                                   > (IData)(vlSelfRef.top__DOT__vgc_inst__DOT__h_count)) 
                                                  & (0x01e0U 
                                                     > (IData)(vlSelfRef.top__DOT__vgc_inst__DOT__v_count)));
    top__DOT__vgc_inst__DOT__tile_inst__DOT__world_y 
        = (0x000003ffU & VL_MODDIV_III(10, (0x000003ffU 
                                            & ((0x000000ffU 
                                                & (((IData)(vlSelfRef.top__DOT__vgc_inst__DOT__v_count) 
                                                    - (IData)(0x00000028U)) 
                                                   >> 1U)) 
                                               + (IData)(vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__scroll_y))), 
                                       (((1U == (IData)(vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__mirror_mode)) 
                                         | (3U == (IData)(vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__mirror_mode)))
                                         ? 0x00c8U : 0x0190U)));
    vlSelfRef.top__DOT__cpu_inst__DOT__P = (0x00000030U 
                                            | (((IData)(vlSelfRef.top__DOT__cpu_inst__DOT__N) 
                                                << 7U) 
                                               | (((IData)(vlSelfRef.top__DOT__cpu_inst__DOT__V) 
                                                   << 6U) 
                                                  | (IData)(vlSelfRef.__VdfgRegularize_hebeb780c_0_19))));
    top__DOT__vgc_inst__DOT__tile_inst__DOT____VdfgRegularize_hb33eefcd_0_2 
        = (0x000007ffU & ((0x0140U <= (0x000007ffU 
                                       & (IData)(top__DOT__vgc_inst__DOT__tile_inst__DOT____VdfgRegularize_hb33eefcd_0_1)))
                           ? ((IData)(top__DOT__vgc_inst__DOT__tile_inst__DOT____VdfgRegularize_hb33eefcd_0_1) 
                              - (IData)(0x0140U)) : 
                          (0x000003ffU & (IData)(top__DOT__vgc_inst__DOT__tile_inst__DOT____VdfgRegularize_hb33eefcd_0_1))));
    vlSelfRef.top__DOT__vgc_inst__DOT__in_text_area 
        = ((IData)(vlSelfRef.top__DOT__vgc_inst__DOT__visible) 
           & ((0x0028U <= (IData)(vlSelfRef.top__DOT__vgc_inst__DOT__v_count)) 
              & (0x01b8U > (IData)(vlSelfRef.top__DOT__vgc_inst__DOT__v_count))));
    __VdfgRegularize_hebeb780c_0_23 = (((0x00c8U <= (IData)(top__DOT__vgc_inst__DOT__tile_inst__DOT__world_y)) 
                                        << 1U) | (0x0140U 
                                                  <= 
                                                  (0x000007ffU 
                                                   & (IData)(top__DOT__vgc_inst__DOT__tile_inst__DOT____VdfgRegularize_hb33eefcd_0_1))));
    top__DOT__vgc_inst__DOT__tile_inst__DOT____VdfgRegularize_hb33eefcd_0_3 
        = (0x000003ffU & ((0x00c8U <= (IData)(top__DOT__vgc_inst__DOT__tile_inst__DOT__world_y))
                           ? ((IData)(top__DOT__vgc_inst__DOT__tile_inst__DOT__world_y) 
                              - (IData)(0x00c8U)) : 
                          (0x000001ffU & (IData)(top__DOT__vgc_inst__DOT__tile_inst__DOT__world_y))));
    vlSelfRef.dbg_cpu_flags = vlSelfRef.top__DOT__cpu_inst__DOT__P;
    vlSelfRef.__VdfgRegularize_hebeb780c_0_10 = (((IData)(vlSelfRef.top__DOT__cpu_inst__DOT__ADD) 
                                                  << 8U) 
                                                 | (IData)(vlSelfRef.top__DOT__cpu_inst__DOT__ABL));
    vlSelfRef.__VdfgRegularize_hebeb780c_0_5 = (((IData)(vlSelfRef.top__DOT__cpu_inst__DOT__ABH) 
                                                 << 8U) 
                                                | (IData)(vlSelfRef.top__DOT__cpu_inst__DOT__ADD));
    if (vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__tile_size16) {
        top__DOT__vgc_inst__DOT__tile_inst__DOT__pix_in_x 
            = (0x0000000fU & (IData)(top__DOT__vgc_inst__DOT__tile_inst__DOT____VdfgRegularize_hb33eefcd_0_2));
        top__DOT__vgc_inst__DOT__tile_inst__DOT__pix_in_y 
            = (0x0000000fU & (IData)(top__DOT__vgc_inst__DOT__tile_inst__DOT____VdfgRegularize_hb33eefcd_0_3));
    } else {
        top__DOT__vgc_inst__DOT__tile_inst__DOT__pix_in_x 
            = (0x0000000fU & (7U & (IData)(top__DOT__vgc_inst__DOT__tile_inst__DOT____VdfgRegularize_hb33eefcd_0_2)));
        top__DOT__vgc_inst__DOT__tile_inst__DOT__pix_in_y 
            = (0x0000000fU & (7U & (IData)(top__DOT__vgc_inst__DOT__tile_inst__DOT____VdfgRegularize_hb33eefcd_0_3)));
    }
    vlSelfRef.top__DOT__vgc_inst__DOT__cursor_here 
        = ((IData)(vlSelfRef.top__DOT__vgc_inst__DOT__cursor_enable) 
           & ((IData)(vlSelfRef.top__DOT__vgc_inst__DOT__in_text_area) 
              & (((IData)(vlSelfRef.top__DOT__vgc_inst__DOT__cursor_x) 
                  == (0x0000007fU & ((IData)(vlSelfRef.top__DOT__vgc_inst__DOT__h_count) 
                                     >> 3U))) & ((IData)(vlSelfRef.top__DOT__vgc_inst__DOT__cursor_y) 
                                                 == (IData)(top__DOT__vgc_inst__DOT__text_row)))));
    top__DOT__vgc_inst__DOT__tile_inst__DOT____VdfgRegularize_hb33eefcd_0_4 
        = (0x00000fffU & (((IData)(0x000003e8U) * (
                                                   (2U 
                                                    & (IData)(vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__mirror_mode))
                                                    ? 
                                                   ((1U 
                                                     & (IData)(vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__mirror_mode))
                                                     ? 0U
                                                     : (IData)(__VdfgRegularize_hebeb780c_0_23))
                                                    : 
                                                   ((1U 
                                                     & (IData)(vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__mirror_mode))
                                                     ? 
                                                    ((2U 
                                                      == (IData)(__VdfgRegularize_hebeb780c_0_23))
                                                      ? 0U
                                                      : 
                                                     ((3U 
                                                       == (IData)(__VdfgRegularize_hebeb780c_0_23))
                                                       ? 1U
                                                       : (IData)(__VdfgRegularize_hebeb780c_0_23)))
                                                     : 
                                                    ((1U 
                                                      == (IData)(__VdfgRegularize_hebeb780c_0_23))
                                                      ? 0U
                                                      : 
                                                     ((3U 
                                                       == (IData)(__VdfgRegularize_hebeb780c_0_23))
                                                       ? 2U
                                                       : (IData)(__VdfgRegularize_hebeb780c_0_23)))))) 
                          + (0x000003ffU & (((IData)(0x00000028U) 
                                             * (0x0000001fU 
                                                & ((IData)(vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__tile_size16)
                                                    ? 
                                                   (0x0000001fU 
                                                    & ((IData)(top__DOT__vgc_inst__DOT__tile_inst__DOT____VdfgRegularize_hb33eefcd_0_3) 
                                                       >> 4U))
                                                    : 
                                                   ((IData)(top__DOT__vgc_inst__DOT__tile_inst__DOT____VdfgRegularize_hb33eefcd_0_3) 
                                                    >> 3U)))) 
                                            + (0x0000003fU 
                                               & ((IData)(vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__tile_size16)
                                                   ? 
                                                  (0x0000003fU 
                                                   & ((IData)(top__DOT__vgc_inst__DOT__tile_inst__DOT____VdfgRegularize_hb33eefcd_0_2) 
                                                      >> 4U))
                                                   : 
                                                  ((IData)(top__DOT__vgc_inst__DOT__tile_inst__DOT____VdfgRegularize_hb33eefcd_0_2) 
                                                   >> 3U)))))));
    if ((0x0f9fU >= (IData)(top__DOT__vgc_inst__DOT__tile_inst__DOT____VdfgRegularize_hb33eefcd_0_4))) {
        top__DOT__vgc_inst__DOT__tile_inst__DOT__cur_tile 
            = vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__nametable
            [top__DOT__vgc_inst__DOT__tile_inst__DOT____VdfgRegularize_hb33eefcd_0_4];
        top__DOT__vgc_inst__DOT__tile_inst__DOT__cur_attr 
            = vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__attr_table
            [top__DOT__vgc_inst__DOT__tile_inst__DOT____VdfgRegularize_hb33eefcd_0_4];
    } else {
        top__DOT__vgc_inst__DOT__tile_inst__DOT__cur_tile = 0U;
        top__DOT__vgc_inst__DOT__tile_inst__DOT__cur_attr = 0U;
    }
    top__DOT__vgc_inst__DOT__tile_inst__DOT__sample_y 
        = (0x0000000fU & ((0x00000080U & (IData)(top__DOT__vgc_inst__DOT__tile_inst__DOT__cur_attr))
                           ? (((IData)(vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__tile_size16)
                                ? 0x0fU : 7U) - (IData)(top__DOT__vgc_inst__DOT__tile_inst__DOT__pix_in_y))
                           : (IData)(top__DOT__vgc_inst__DOT__tile_inst__DOT__pix_in_y)));
    top__DOT__vgc_inst__DOT__tile_inst__DOT__sample_x 
        = (0x0000000fU & ((0x00000040U & (IData)(top__DOT__vgc_inst__DOT__tile_inst__DOT__cur_attr))
                           ? (((IData)(vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__tile_size16)
                                ? 0x0fU : 7U) - (IData)(top__DOT__vgc_inst__DOT__tile_inst__DOT__pix_in_x))
                           : (IData)(top__DOT__vgc_inst__DOT__tile_inst__DOT__pix_in_x)));
    top__DOT__vgc_inst__DOT__tile_inst__DOT__packed_byte 
        = vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__tile_data
        [((IData)(vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__tile_size16)
           ? (((IData)(top__DOT__vgc_inst__DOT__tile_inst__DOT__cur_tile) 
               << 7U) | (((IData)(top__DOT__vgc_inst__DOT__tile_inst__DOT__sample_y) 
                          << 3U) | (7U & ((IData)(top__DOT__vgc_inst__DOT__tile_inst__DOT__sample_x) 
                                          >> 1U))))
           : (((IData)(top__DOT__vgc_inst__DOT__tile_inst__DOT__cur_tile) 
               << 5U) | ((0x0000001cU & ((IData)(top__DOT__vgc_inst__DOT__tile_inst__DOT__sample_y) 
                                         << 2U)) | 
                         (3U & ((IData)(top__DOT__vgc_inst__DOT__tile_inst__DOT__sample_x) 
                                >> 1U)))))];
    top__DOT__vgc_inst__DOT__tile_inst__DOT__color_idx 
        = (0x0000000fU & ((1U & (IData)(top__DOT__vgc_inst__DOT__tile_inst__DOT__sample_x))
                           ? (IData)(top__DOT__vgc_inst__DOT__tile_inst__DOT__packed_byte)
                           : ((IData)(top__DOT__vgc_inst__DOT__tile_inst__DOT__packed_byte) 
                              >> 4U)));
    __VdfgRegularize_hebeb780c_0_25 = ((IData)(vlSelfRef.top__DOT__vgc_inst__DOT__in_text_area) 
                                       & ((IData)(top__DOT__vgc_inst__DOT__tile_inst__DOT__color_idx) 
                                          != (IData)(vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__trans_color)));
    top__DOT__vgc_inst__DOT__tile_opaque = ((IData)(__VdfgRegularize_hebeb780c_0_25)
                                             ? ((0x00000020U 
                                                 & (IData)(top__DOT__vgc_inst__DOT__tile_inst__DOT__cur_attr))
                                                 ? 2U
                                                 : 1U)
                                             : 0U);
    if ((0x07cfU >= (0x000007ffU & (((IData)(0x00000050U) 
                                     * (IData)(top__DOT__vgc_inst__DOT__real_row)) 
                                    + (0x0000007fU 
                                       & ((IData)(vlSelfRef.top__DOT__vgc_inst__DOT__h_count) 
                                          >> 3U)))))) {
        top__DOT__vgc_inst__DOT__cur_char = vlSelfRef.top__DOT__vgc_inst__DOT__char_ram
            [(0x000007ffU & (((IData)(0x00000050U) 
                              * (IData)(top__DOT__vgc_inst__DOT__real_row)) 
                             + (0x0000007fU & ((IData)(vlSelfRef.top__DOT__vgc_inst__DOT__h_count) 
                                               >> 3U))))];
        top__DOT__vgc_inst__DOT__cur_fg = (0x0000000fU 
                                           & vlSelfRef.top__DOT__vgc_inst__DOT__color_ram
                                           [(0x000007ffU 
                                             & (((IData)(0x00000050U) 
                                                 * (IData)(top__DOT__vgc_inst__DOT__real_row)) 
                                                + (0x0000007fU 
                                                   & ((IData)(vlSelfRef.top__DOT__vgc_inst__DOT__h_count) 
                                                      >> 3U))))]);
    } else {
        top__DOT__vgc_inst__DOT__cur_char = 0U;
        top__DOT__vgc_inst__DOT__cur_fg = 0U;
    }
    top__DOT__vgc_inst__DOT__font_byte = vlSelfRef.top__DOT__vgc_inst__DOT__font_rom
        [(((IData)(top__DOT__vgc_inst__DOT__cur_char) 
           << 3U) | (IData)(top__DOT__vgc_inst__DOT__font_line))];
    top__DOT__vgc_inst__DOT__pixel_on = (1U & ((IData)(top__DOT__vgc_inst__DOT__font_byte) 
                                               >> (7U 
                                                   & ((IData)(7U) 
                                                      - (IData)(vlSelfRef.top__DOT__vgc_inst__DOT__h_count)))));
    top__DOT__vgc_inst__DOT__text_pixel = ((IData)(top__DOT__vgc_inst__DOT__pixel_on)
                                            ? vlSelfRef.top__DOT__vgc_inst__DOT__palette
                                           [top__DOT__vgc_inst__DOT__cur_fg]
                                            : vlSelfRef.top__DOT__vgc_inst__DOT__palette
                                           [vlSelfRef.top__DOT__vgc_inst__DOT__bg_color]);
    top__DOT__vgc_inst__DOT__cur_gfx = ((0xf9ffU >= 
                                         (0x0000ffffU 
                                          & (((IData)(0x00000140U) 
                                              * (IData)(vlSelfRef.top__DOT__vgc_inst__DOT__gfx_y)) 
                                             + (0x000001ffU 
                                                & ((IData)(vlSelfRef.top__DOT__vgc_inst__DOT__h_count) 
                                                   >> 1U)))))
                                         ? vlSelfRef.top__DOT__vgc_inst__DOT__gfx_ram
                                        [(0x0000ffffU 
                                          & (((IData)(0x00000140U) 
                                              * (IData)(vlSelfRef.top__DOT__vgc_inst__DOT__gfx_y)) 
                                             + (0x000001ffU 
                                                & ((IData)(vlSelfRef.top__DOT__vgc_inst__DOT__h_count) 
                                                   >> 1U))))]
                                         : 0U);
    top__DOT__vgc_inst__DOT__gfx_pixel = vlSelfRef.top__DOT__vgc_inst__DOT__palette
        [top__DOT__vgc_inst__DOT__cur_gfx];
    vlSelfRef.top__DOT__vgc_inst__DOT__spr_dx[0U] = 
        (0x000003ffU & ((0x000001ffU & ((IData)(vlSelfRef.top__DOT__vgc_inst__DOT__h_count) 
                                        >> 1U)) - vlSelfRef.top__DOT__vgc_inst__DOT__spr_x[0U]));
    vlSelfRef.top__DOT__vgc_inst__DOT__spr_dy[0U] = 
        (0x000003ffU & ((IData)(vlSelfRef.top__DOT__vgc_inst__DOT__gfx_y) 
                        - vlSelfRef.top__DOT__vgc_inst__DOT__spr_y[0U]));
    vlSelfRef.top__DOT__vgc_inst__DOT__spr_fx[0U] = 
        (0x0000000fU & (vlSelfRef.top__DOT__vgc_inst__DOT__spr_flip_h[0U]
                         ? ((IData)(0x0fU) - vlSelfRef.top__DOT__vgc_inst__DOT__spr_dx[0U])
                         : vlSelfRef.top__DOT__vgc_inst__DOT__spr_dx[0U]));
    vlSelfRef.top__DOT__vgc_inst__DOT__spr_fy[0U] = 
        (0x0000000fU & (vlSelfRef.top__DOT__vgc_inst__DOT__spr_flip_v[0U]
                         ? ((IData)(0x0fU) - vlSelfRef.top__DOT__vgc_inst__DOT__spr_dy[0U])
                         : vlSelfRef.top__DOT__vgc_inst__DOT__spr_dy[0U]));
    vlSelfRef.top__DOT__vgc_inst__DOT__spr_sa[0U] = 
        ((vlSelfRef.top__DOT__vgc_inst__DOT__spr_shape[0U] 
          << 7U) | ((vlSelfRef.top__DOT__vgc_inst__DOT__spr_fy[0U] 
                     << 3U) | (7U & (vlSelfRef.top__DOT__vgc_inst__DOT__spr_fx[0U] 
                                     >> 1U))));
    vlSelfRef.top__DOT__vgc_inst__DOT__spr_pix[0U] 
        = (0x0000000fU & ((1U & vlSelfRef.top__DOT__vgc_inst__DOT__spr_fx[0U])
                           ? vlSelfRef.top__DOT__vgc_inst__DOT__sprite_shapes
                          [vlSelfRef.top__DOT__vgc_inst__DOT__spr_sa[0U]]
                           : (vlSelfRef.top__DOT__vgc_inst__DOT__sprite_shapes
                              [vlSelfRef.top__DOT__vgc_inst__DOT__spr_sa[0U]] 
                              >> 4U)));
    vlSelfRef.top__DOT__vgc_inst__DOT__spr_vis[0U] 
        = ((((((vlSelfRef.top__DOT__vgc_inst__DOT__spr_enable[0U] 
                & (IData)(vlSelfRef.top__DOT__vgc_inst__DOT__in_text_area)) 
               & VL_LTES_III(32, 0U, VL_EXTENDS_II(32,10, vlSelfRef.top__DOT__vgc_inst__DOT__spr_dx[0U]))) 
              & VL_GTS_III(32, 0x00000010U, VL_EXTENDS_II(32,10, vlSelfRef.top__DOT__vgc_inst__DOT__spr_dx[0U]))) 
             & VL_LTES_III(32, 0U, VL_EXTENDS_II(32,10, vlSelfRef.top__DOT__vgc_inst__DOT__spr_dy[0U]))) 
            & VL_GTS_III(32, 0x00000010U, VL_EXTENDS_II(32,10, vlSelfRef.top__DOT__vgc_inst__DOT__spr_dy[0U]))) 
           & (vlSelfRef.top__DOT__vgc_inst__DOT__spr_pix[0U] 
              != vlSelfRef.top__DOT__vgc_inst__DOT__spr_trans[0U]));
    vlSelfRef.top__DOT__vgc_inst__DOT__spr_dx[1U] = 
        (0x000003ffU & ((0x000001ffU & ((IData)(vlSelfRef.top__DOT__vgc_inst__DOT__h_count) 
                                        >> 1U)) - vlSelfRef.top__DOT__vgc_inst__DOT__spr_x[1U]));
    vlSelfRef.top__DOT__vgc_inst__DOT__spr_dy[1U] = 
        (0x000003ffU & ((IData)(vlSelfRef.top__DOT__vgc_inst__DOT__gfx_y) 
                        - vlSelfRef.top__DOT__vgc_inst__DOT__spr_y[1U]));
    vlSelfRef.top__DOT__vgc_inst__DOT__spr_fx[1U] = 
        (0x0000000fU & (vlSelfRef.top__DOT__vgc_inst__DOT__spr_flip_h[1U]
                         ? ((IData)(0x0fU) - vlSelfRef.top__DOT__vgc_inst__DOT__spr_dx[1U])
                         : vlSelfRef.top__DOT__vgc_inst__DOT__spr_dx[1U]));
    vlSelfRef.top__DOT__vgc_inst__DOT__spr_fy[1U] = 
        (0x0000000fU & (vlSelfRef.top__DOT__vgc_inst__DOT__spr_flip_v[1U]
                         ? ((IData)(0x0fU) - vlSelfRef.top__DOT__vgc_inst__DOT__spr_dy[1U])
                         : vlSelfRef.top__DOT__vgc_inst__DOT__spr_dy[1U]));
    vlSelfRef.top__DOT__vgc_inst__DOT__spr_sa[1U] = 
        ((vlSelfRef.top__DOT__vgc_inst__DOT__spr_shape[1U] 
          << 7U) | ((vlSelfRef.top__DOT__vgc_inst__DOT__spr_fy[1U] 
                     << 3U) | (7U & (vlSelfRef.top__DOT__vgc_inst__DOT__spr_fx[1U] 
                                     >> 1U))));
    vlSelfRef.top__DOT__vgc_inst__DOT__spr_pix[1U] 
        = (0x0000000fU & ((1U & vlSelfRef.top__DOT__vgc_inst__DOT__spr_fx[1U])
                           ? vlSelfRef.top__DOT__vgc_inst__DOT__sprite_shapes
                          [vlSelfRef.top__DOT__vgc_inst__DOT__spr_sa[1U]]
                           : (vlSelfRef.top__DOT__vgc_inst__DOT__sprite_shapes
                              [vlSelfRef.top__DOT__vgc_inst__DOT__spr_sa[1U]] 
                              >> 4U)));
    vlSelfRef.top__DOT__vgc_inst__DOT__spr_vis[1U] 
        = ((((((vlSelfRef.top__DOT__vgc_inst__DOT__spr_enable[1U] 
                & (IData)(vlSelfRef.top__DOT__vgc_inst__DOT__in_text_area)) 
               & VL_LTES_III(32, 0U, VL_EXTENDS_II(32,10, vlSelfRef.top__DOT__vgc_inst__DOT__spr_dx[1U]))) 
              & VL_GTS_III(32, 0x00000010U, VL_EXTENDS_II(32,10, vlSelfRef.top__DOT__vgc_inst__DOT__spr_dx[1U]))) 
             & VL_LTES_III(32, 0U, VL_EXTENDS_II(32,10, vlSelfRef.top__DOT__vgc_inst__DOT__spr_dy[1U]))) 
            & VL_GTS_III(32, 0x00000010U, VL_EXTENDS_II(32,10, vlSelfRef.top__DOT__vgc_inst__DOT__spr_dy[1U]))) 
           & (vlSelfRef.top__DOT__vgc_inst__DOT__spr_pix[1U] 
              != vlSelfRef.top__DOT__vgc_inst__DOT__spr_trans[1U]));
    vlSelfRef.top__DOT__vgc_inst__DOT__spr_dx[2U] = 
        (0x000003ffU & ((0x000001ffU & ((IData)(vlSelfRef.top__DOT__vgc_inst__DOT__h_count) 
                                        >> 1U)) - vlSelfRef.top__DOT__vgc_inst__DOT__spr_x[2U]));
    vlSelfRef.top__DOT__vgc_inst__DOT__spr_dy[2U] = 
        (0x000003ffU & ((IData)(vlSelfRef.top__DOT__vgc_inst__DOT__gfx_y) 
                        - vlSelfRef.top__DOT__vgc_inst__DOT__spr_y[2U]));
    vlSelfRef.top__DOT__vgc_inst__DOT__spr_fx[2U] = 
        (0x0000000fU & (vlSelfRef.top__DOT__vgc_inst__DOT__spr_flip_h[2U]
                         ? ((IData)(0x0fU) - vlSelfRef.top__DOT__vgc_inst__DOT__spr_dx[2U])
                         : vlSelfRef.top__DOT__vgc_inst__DOT__spr_dx[2U]));
    vlSelfRef.top__DOT__vgc_inst__DOT__spr_fy[2U] = 
        (0x0000000fU & (vlSelfRef.top__DOT__vgc_inst__DOT__spr_flip_v[2U]
                         ? ((IData)(0x0fU) - vlSelfRef.top__DOT__vgc_inst__DOT__spr_dy[2U])
                         : vlSelfRef.top__DOT__vgc_inst__DOT__spr_dy[2U]));
    vlSelfRef.top__DOT__vgc_inst__DOT__spr_sa[2U] = 
        ((vlSelfRef.top__DOT__vgc_inst__DOT__spr_shape[2U] 
          << 7U) | ((vlSelfRef.top__DOT__vgc_inst__DOT__spr_fy[2U] 
                     << 3U) | (7U & (vlSelfRef.top__DOT__vgc_inst__DOT__spr_fx[2U] 
                                     >> 1U))));
    vlSelfRef.top__DOT__vgc_inst__DOT__spr_pix[2U] 
        = (0x0000000fU & ((1U & vlSelfRef.top__DOT__vgc_inst__DOT__spr_fx[2U])
                           ? vlSelfRef.top__DOT__vgc_inst__DOT__sprite_shapes
                          [vlSelfRef.top__DOT__vgc_inst__DOT__spr_sa[2U]]
                           : (vlSelfRef.top__DOT__vgc_inst__DOT__sprite_shapes
                              [vlSelfRef.top__DOT__vgc_inst__DOT__spr_sa[2U]] 
                              >> 4U)));
    vlSelfRef.top__DOT__vgc_inst__DOT__spr_vis[2U] 
        = ((((((vlSelfRef.top__DOT__vgc_inst__DOT__spr_enable[2U] 
                & (IData)(vlSelfRef.top__DOT__vgc_inst__DOT__in_text_area)) 
               & VL_LTES_III(32, 0U, VL_EXTENDS_II(32,10, vlSelfRef.top__DOT__vgc_inst__DOT__spr_dx[2U]))) 
              & VL_GTS_III(32, 0x00000010U, VL_EXTENDS_II(32,10, vlSelfRef.top__DOT__vgc_inst__DOT__spr_dx[2U]))) 
             & VL_LTES_III(32, 0U, VL_EXTENDS_II(32,10, vlSelfRef.top__DOT__vgc_inst__DOT__spr_dy[2U]))) 
            & VL_GTS_III(32, 0x00000010U, VL_EXTENDS_II(32,10, vlSelfRef.top__DOT__vgc_inst__DOT__spr_dy[2U]))) 
           & (vlSelfRef.top__DOT__vgc_inst__DOT__spr_pix[2U] 
              != vlSelfRef.top__DOT__vgc_inst__DOT__spr_trans[2U]));
    vlSelfRef.top__DOT__vgc_inst__DOT__spr_dx[3U] = 
        (0x000003ffU & ((0x000001ffU & ((IData)(vlSelfRef.top__DOT__vgc_inst__DOT__h_count) 
                                        >> 1U)) - vlSelfRef.top__DOT__vgc_inst__DOT__spr_x[3U]));
    vlSelfRef.top__DOT__vgc_inst__DOT__spr_dy[3U] = 
        (0x000003ffU & ((IData)(vlSelfRef.top__DOT__vgc_inst__DOT__gfx_y) 
                        - vlSelfRef.top__DOT__vgc_inst__DOT__spr_y[3U]));
    vlSelfRef.top__DOT__vgc_inst__DOT__spr_fx[3U] = 
        (0x0000000fU & (vlSelfRef.top__DOT__vgc_inst__DOT__spr_flip_h[3U]
                         ? ((IData)(0x0fU) - vlSelfRef.top__DOT__vgc_inst__DOT__spr_dx[3U])
                         : vlSelfRef.top__DOT__vgc_inst__DOT__spr_dx[3U]));
    vlSelfRef.top__DOT__vgc_inst__DOT__spr_fy[3U] = 
        (0x0000000fU & (vlSelfRef.top__DOT__vgc_inst__DOT__spr_flip_v[3U]
                         ? ((IData)(0x0fU) - vlSelfRef.top__DOT__vgc_inst__DOT__spr_dy[3U])
                         : vlSelfRef.top__DOT__vgc_inst__DOT__spr_dy[3U]));
    vlSelfRef.top__DOT__vgc_inst__DOT__spr_sa[3U] = 
        ((vlSelfRef.top__DOT__vgc_inst__DOT__spr_shape[3U] 
          << 7U) | ((vlSelfRef.top__DOT__vgc_inst__DOT__spr_fy[3U] 
                     << 3U) | (7U & (vlSelfRef.top__DOT__vgc_inst__DOT__spr_fx[3U] 
                                     >> 1U))));
    vlSelfRef.top__DOT__vgc_inst__DOT__spr_pix[3U] 
        = (0x0000000fU & ((1U & vlSelfRef.top__DOT__vgc_inst__DOT__spr_fx[3U])
                           ? vlSelfRef.top__DOT__vgc_inst__DOT__sprite_shapes
                          [vlSelfRef.top__DOT__vgc_inst__DOT__spr_sa[3U]]
                           : (vlSelfRef.top__DOT__vgc_inst__DOT__sprite_shapes
                              [vlSelfRef.top__DOT__vgc_inst__DOT__spr_sa[3U]] 
                              >> 4U)));
    vlSelfRef.top__DOT__vgc_inst__DOT__spr_vis[3U] 
        = ((((((vlSelfRef.top__DOT__vgc_inst__DOT__spr_enable[3U] 
                & (IData)(vlSelfRef.top__DOT__vgc_inst__DOT__in_text_area)) 
               & VL_LTES_III(32, 0U, VL_EXTENDS_II(32,10, vlSelfRef.top__DOT__vgc_inst__DOT__spr_dx[3U]))) 
              & VL_GTS_III(32, 0x00000010U, VL_EXTENDS_II(32,10, vlSelfRef.top__DOT__vgc_inst__DOT__spr_dx[3U]))) 
             & VL_LTES_III(32, 0U, VL_EXTENDS_II(32,10, vlSelfRef.top__DOT__vgc_inst__DOT__spr_dy[3U]))) 
            & VL_GTS_III(32, 0x00000010U, VL_EXTENDS_II(32,10, vlSelfRef.top__DOT__vgc_inst__DOT__spr_dy[3U]))) 
           & (vlSelfRef.top__DOT__vgc_inst__DOT__spr_pix[3U] 
              != vlSelfRef.top__DOT__vgc_inst__DOT__spr_trans[3U]));
    vlSelfRef.top__DOT__vgc_inst__DOT__spr_dx[4U] = 
        (0x000003ffU & ((0x000001ffU & ((IData)(vlSelfRef.top__DOT__vgc_inst__DOT__h_count) 
                                        >> 1U)) - vlSelfRef.top__DOT__vgc_inst__DOT__spr_x[4U]));
    vlSelfRef.top__DOT__vgc_inst__DOT__spr_dy[4U] = 
        (0x000003ffU & ((IData)(vlSelfRef.top__DOT__vgc_inst__DOT__gfx_y) 
                        - vlSelfRef.top__DOT__vgc_inst__DOT__spr_y[4U]));
    vlSelfRef.top__DOT__vgc_inst__DOT__spr_fx[4U] = 
        (0x0000000fU & (vlSelfRef.top__DOT__vgc_inst__DOT__spr_flip_h[4U]
                         ? ((IData)(0x0fU) - vlSelfRef.top__DOT__vgc_inst__DOT__spr_dx[4U])
                         : vlSelfRef.top__DOT__vgc_inst__DOT__spr_dx[4U]));
    vlSelfRef.top__DOT__vgc_inst__DOT__spr_fy[4U] = 
        (0x0000000fU & (vlSelfRef.top__DOT__vgc_inst__DOT__spr_flip_v[4U]
                         ? ((IData)(0x0fU) - vlSelfRef.top__DOT__vgc_inst__DOT__spr_dy[4U])
                         : vlSelfRef.top__DOT__vgc_inst__DOT__spr_dy[4U]));
    vlSelfRef.top__DOT__vgc_inst__DOT__spr_sa[4U] = 
        ((vlSelfRef.top__DOT__vgc_inst__DOT__spr_shape[4U] 
          << 7U) | ((vlSelfRef.top__DOT__vgc_inst__DOT__spr_fy[4U] 
                     << 3U) | (7U & (vlSelfRef.top__DOT__vgc_inst__DOT__spr_fx[4U] 
                                     >> 1U))));
    vlSelfRef.top__DOT__vgc_inst__DOT__spr_pix[4U] 
        = (0x0000000fU & ((1U & vlSelfRef.top__DOT__vgc_inst__DOT__spr_fx[4U])
                           ? vlSelfRef.top__DOT__vgc_inst__DOT__sprite_shapes
                          [vlSelfRef.top__DOT__vgc_inst__DOT__spr_sa[4U]]
                           : (vlSelfRef.top__DOT__vgc_inst__DOT__sprite_shapes
                              [vlSelfRef.top__DOT__vgc_inst__DOT__spr_sa[4U]] 
                              >> 4U)));
    vlSelfRef.top__DOT__vgc_inst__DOT__spr_vis[4U] 
        = ((((((vlSelfRef.top__DOT__vgc_inst__DOT__spr_enable[4U] 
                & (IData)(vlSelfRef.top__DOT__vgc_inst__DOT__in_text_area)) 
               & VL_LTES_III(32, 0U, VL_EXTENDS_II(32,10, vlSelfRef.top__DOT__vgc_inst__DOT__spr_dx[4U]))) 
              & VL_GTS_III(32, 0x00000010U, VL_EXTENDS_II(32,10, vlSelfRef.top__DOT__vgc_inst__DOT__spr_dx[4U]))) 
             & VL_LTES_III(32, 0U, VL_EXTENDS_II(32,10, vlSelfRef.top__DOT__vgc_inst__DOT__spr_dy[4U]))) 
            & VL_GTS_III(32, 0x00000010U, VL_EXTENDS_II(32,10, vlSelfRef.top__DOT__vgc_inst__DOT__spr_dy[4U]))) 
           & (vlSelfRef.top__DOT__vgc_inst__DOT__spr_pix[4U] 
              != vlSelfRef.top__DOT__vgc_inst__DOT__spr_trans[4U]));
    vlSelfRef.top__DOT__vgc_inst__DOT__spr_dx[5U] = 
        (0x000003ffU & ((0x000001ffU & ((IData)(vlSelfRef.top__DOT__vgc_inst__DOT__h_count) 
                                        >> 1U)) - vlSelfRef.top__DOT__vgc_inst__DOT__spr_x[5U]));
    vlSelfRef.top__DOT__vgc_inst__DOT__spr_dy[5U] = 
        (0x000003ffU & ((IData)(vlSelfRef.top__DOT__vgc_inst__DOT__gfx_y) 
                        - vlSelfRef.top__DOT__vgc_inst__DOT__spr_y[5U]));
    vlSelfRef.top__DOT__vgc_inst__DOT__spr_fx[5U] = 
        (0x0000000fU & (vlSelfRef.top__DOT__vgc_inst__DOT__spr_flip_h[5U]
                         ? ((IData)(0x0fU) - vlSelfRef.top__DOT__vgc_inst__DOT__spr_dx[5U])
                         : vlSelfRef.top__DOT__vgc_inst__DOT__spr_dx[5U]));
    vlSelfRef.top__DOT__vgc_inst__DOT__spr_fy[5U] = 
        (0x0000000fU & (vlSelfRef.top__DOT__vgc_inst__DOT__spr_flip_v[5U]
                         ? ((IData)(0x0fU) - vlSelfRef.top__DOT__vgc_inst__DOT__spr_dy[5U])
                         : vlSelfRef.top__DOT__vgc_inst__DOT__spr_dy[5U]));
    vlSelfRef.top__DOT__vgc_inst__DOT__spr_sa[5U] = 
        ((vlSelfRef.top__DOT__vgc_inst__DOT__spr_shape[5U] 
          << 7U) | ((vlSelfRef.top__DOT__vgc_inst__DOT__spr_fy[5U] 
                     << 3U) | (7U & (vlSelfRef.top__DOT__vgc_inst__DOT__spr_fx[5U] 
                                     >> 1U))));
    vlSelfRef.top__DOT__vgc_inst__DOT__spr_pix[5U] 
        = (0x0000000fU & ((1U & vlSelfRef.top__DOT__vgc_inst__DOT__spr_fx[5U])
                           ? vlSelfRef.top__DOT__vgc_inst__DOT__sprite_shapes
                          [vlSelfRef.top__DOT__vgc_inst__DOT__spr_sa[5U]]
                           : (vlSelfRef.top__DOT__vgc_inst__DOT__sprite_shapes
                              [vlSelfRef.top__DOT__vgc_inst__DOT__spr_sa[5U]] 
                              >> 4U)));
    vlSelfRef.top__DOT__vgc_inst__DOT__spr_vis[5U] 
        = ((((((vlSelfRef.top__DOT__vgc_inst__DOT__spr_enable[5U] 
                & (IData)(vlSelfRef.top__DOT__vgc_inst__DOT__in_text_area)) 
               & VL_LTES_III(32, 0U, VL_EXTENDS_II(32,10, vlSelfRef.top__DOT__vgc_inst__DOT__spr_dx[5U]))) 
              & VL_GTS_III(32, 0x00000010U, VL_EXTENDS_II(32,10, vlSelfRef.top__DOT__vgc_inst__DOT__spr_dx[5U]))) 
             & VL_LTES_III(32, 0U, VL_EXTENDS_II(32,10, vlSelfRef.top__DOT__vgc_inst__DOT__spr_dy[5U]))) 
            & VL_GTS_III(32, 0x00000010U, VL_EXTENDS_II(32,10, vlSelfRef.top__DOT__vgc_inst__DOT__spr_dy[5U]))) 
           & (vlSelfRef.top__DOT__vgc_inst__DOT__spr_pix[5U] 
              != vlSelfRef.top__DOT__vgc_inst__DOT__spr_trans[5U]));
    vlSelfRef.top__DOT__vgc_inst__DOT__spr_dx[6U] = 
        (0x000003ffU & ((0x000001ffU & ((IData)(vlSelfRef.top__DOT__vgc_inst__DOT__h_count) 
                                        >> 1U)) - vlSelfRef.top__DOT__vgc_inst__DOT__spr_x[6U]));
    vlSelfRef.top__DOT__vgc_inst__DOT__spr_dy[6U] = 
        (0x000003ffU & ((IData)(vlSelfRef.top__DOT__vgc_inst__DOT__gfx_y) 
                        - vlSelfRef.top__DOT__vgc_inst__DOT__spr_y[6U]));
    vlSelfRef.top__DOT__vgc_inst__DOT__spr_fx[6U] = 
        (0x0000000fU & (vlSelfRef.top__DOT__vgc_inst__DOT__spr_flip_h[6U]
                         ? ((IData)(0x0fU) - vlSelfRef.top__DOT__vgc_inst__DOT__spr_dx[6U])
                         : vlSelfRef.top__DOT__vgc_inst__DOT__spr_dx[6U]));
    vlSelfRef.top__DOT__vgc_inst__DOT__spr_fy[6U] = 
        (0x0000000fU & (vlSelfRef.top__DOT__vgc_inst__DOT__spr_flip_v[6U]
                         ? ((IData)(0x0fU) - vlSelfRef.top__DOT__vgc_inst__DOT__spr_dy[6U])
                         : vlSelfRef.top__DOT__vgc_inst__DOT__spr_dy[6U]));
    vlSelfRef.top__DOT__vgc_inst__DOT__spr_sa[6U] = 
        ((vlSelfRef.top__DOT__vgc_inst__DOT__spr_shape[6U] 
          << 7U) | ((vlSelfRef.top__DOT__vgc_inst__DOT__spr_fy[6U] 
                     << 3U) | (7U & (vlSelfRef.top__DOT__vgc_inst__DOT__spr_fx[6U] 
                                     >> 1U))));
    vlSelfRef.top__DOT__vgc_inst__DOT__spr_pix[6U] 
        = (0x0000000fU & ((1U & vlSelfRef.top__DOT__vgc_inst__DOT__spr_fx[6U])
                           ? vlSelfRef.top__DOT__vgc_inst__DOT__sprite_shapes
                          [vlSelfRef.top__DOT__vgc_inst__DOT__spr_sa[6U]]
                           : (vlSelfRef.top__DOT__vgc_inst__DOT__sprite_shapes
                              [vlSelfRef.top__DOT__vgc_inst__DOT__spr_sa[6U]] 
                              >> 4U)));
    vlSelfRef.top__DOT__vgc_inst__DOT__spr_vis[6U] 
        = ((((((vlSelfRef.top__DOT__vgc_inst__DOT__spr_enable[6U] 
                & (IData)(vlSelfRef.top__DOT__vgc_inst__DOT__in_text_area)) 
               & VL_LTES_III(32, 0U, VL_EXTENDS_II(32,10, vlSelfRef.top__DOT__vgc_inst__DOT__spr_dx[6U]))) 
              & VL_GTS_III(32, 0x00000010U, VL_EXTENDS_II(32,10, vlSelfRef.top__DOT__vgc_inst__DOT__spr_dx[6U]))) 
             & VL_LTES_III(32, 0U, VL_EXTENDS_II(32,10, vlSelfRef.top__DOT__vgc_inst__DOT__spr_dy[6U]))) 
            & VL_GTS_III(32, 0x00000010U, VL_EXTENDS_II(32,10, vlSelfRef.top__DOT__vgc_inst__DOT__spr_dy[6U]))) 
           & (vlSelfRef.top__DOT__vgc_inst__DOT__spr_pix[6U] 
              != vlSelfRef.top__DOT__vgc_inst__DOT__spr_trans[6U]));
    vlSelfRef.top__DOT__vgc_inst__DOT__spr_dx[7U] = 
        (0x000003ffU & ((0x000001ffU & ((IData)(vlSelfRef.top__DOT__vgc_inst__DOT__h_count) 
                                        >> 1U)) - vlSelfRef.top__DOT__vgc_inst__DOT__spr_x[7U]));
    vlSelfRef.top__DOT__vgc_inst__DOT__spr_dy[7U] = 
        (0x000003ffU & ((IData)(vlSelfRef.top__DOT__vgc_inst__DOT__gfx_y) 
                        - vlSelfRef.top__DOT__vgc_inst__DOT__spr_y[7U]));
    vlSelfRef.top__DOT__vgc_inst__DOT__spr_fx[7U] = 
        (0x0000000fU & (vlSelfRef.top__DOT__vgc_inst__DOT__spr_flip_h[7U]
                         ? ((IData)(0x0fU) - vlSelfRef.top__DOT__vgc_inst__DOT__spr_dx[7U])
                         : vlSelfRef.top__DOT__vgc_inst__DOT__spr_dx[7U]));
    vlSelfRef.top__DOT__vgc_inst__DOT__spr_fy[7U] = 
        (0x0000000fU & (vlSelfRef.top__DOT__vgc_inst__DOT__spr_flip_v[7U]
                         ? ((IData)(0x0fU) - vlSelfRef.top__DOT__vgc_inst__DOT__spr_dy[7U])
                         : vlSelfRef.top__DOT__vgc_inst__DOT__spr_dy[7U]));
    vlSelfRef.top__DOT__vgc_inst__DOT__spr_sa[7U] = 
        ((vlSelfRef.top__DOT__vgc_inst__DOT__spr_shape[7U] 
          << 7U) | ((vlSelfRef.top__DOT__vgc_inst__DOT__spr_fy[7U] 
                     << 3U) | (7U & (vlSelfRef.top__DOT__vgc_inst__DOT__spr_fx[7U] 
                                     >> 1U))));
    vlSelfRef.top__DOT__vgc_inst__DOT__spr_pix[7U] 
        = (0x0000000fU & ((1U & vlSelfRef.top__DOT__vgc_inst__DOT__spr_fx[7U])
                           ? vlSelfRef.top__DOT__vgc_inst__DOT__sprite_shapes
                          [vlSelfRef.top__DOT__vgc_inst__DOT__spr_sa[7U]]
                           : (vlSelfRef.top__DOT__vgc_inst__DOT__sprite_shapes
                              [vlSelfRef.top__DOT__vgc_inst__DOT__spr_sa[7U]] 
                              >> 4U)));
    vlSelfRef.top__DOT__vgc_inst__DOT__spr_vis[7U] 
        = ((((((vlSelfRef.top__DOT__vgc_inst__DOT__spr_enable[7U] 
                & (IData)(vlSelfRef.top__DOT__vgc_inst__DOT__in_text_area)) 
               & VL_LTES_III(32, 0U, VL_EXTENDS_II(32,10, vlSelfRef.top__DOT__vgc_inst__DOT__spr_dx[7U]))) 
              & VL_GTS_III(32, 0x00000010U, VL_EXTENDS_II(32,10, vlSelfRef.top__DOT__vgc_inst__DOT__spr_dx[7U]))) 
             & VL_LTES_III(32, 0U, VL_EXTENDS_II(32,10, vlSelfRef.top__DOT__vgc_inst__DOT__spr_dy[7U]))) 
            & VL_GTS_III(32, 0x00000010U, VL_EXTENDS_II(32,10, vlSelfRef.top__DOT__vgc_inst__DOT__spr_dy[7U]))) 
           & (vlSelfRef.top__DOT__vgc_inst__DOT__spr_pix[7U] 
              != vlSelfRef.top__DOT__vgc_inst__DOT__spr_trans[7U]));
    vlSelfRef.top__DOT__vgc_inst__DOT__spr_dx[8U] = 
        (0x000003ffU & ((0x000001ffU & ((IData)(vlSelfRef.top__DOT__vgc_inst__DOT__h_count) 
                                        >> 1U)) - vlSelfRef.top__DOT__vgc_inst__DOT__spr_x[8U]));
    vlSelfRef.top__DOT__vgc_inst__DOT__spr_dy[8U] = 
        (0x000003ffU & ((IData)(vlSelfRef.top__DOT__vgc_inst__DOT__gfx_y) 
                        - vlSelfRef.top__DOT__vgc_inst__DOT__spr_y[8U]));
    vlSelfRef.top__DOT__vgc_inst__DOT__spr_fx[8U] = 
        (0x0000000fU & (vlSelfRef.top__DOT__vgc_inst__DOT__spr_flip_h[8U]
                         ? ((IData)(0x0fU) - vlSelfRef.top__DOT__vgc_inst__DOT__spr_dx[8U])
                         : vlSelfRef.top__DOT__vgc_inst__DOT__spr_dx[8U]));
    vlSelfRef.top__DOT__vgc_inst__DOT__spr_fy[8U] = 
        (0x0000000fU & (vlSelfRef.top__DOT__vgc_inst__DOT__spr_flip_v[8U]
                         ? ((IData)(0x0fU) - vlSelfRef.top__DOT__vgc_inst__DOT__spr_dy[8U])
                         : vlSelfRef.top__DOT__vgc_inst__DOT__spr_dy[8U]));
    vlSelfRef.top__DOT__vgc_inst__DOT__spr_sa[8U] = 
        ((vlSelfRef.top__DOT__vgc_inst__DOT__spr_shape[8U] 
          << 7U) | ((vlSelfRef.top__DOT__vgc_inst__DOT__spr_fy[8U] 
                     << 3U) | (7U & (vlSelfRef.top__DOT__vgc_inst__DOT__spr_fx[8U] 
                                     >> 1U))));
    vlSelfRef.top__DOT__vgc_inst__DOT__spr_pix[8U] 
        = (0x0000000fU & ((1U & vlSelfRef.top__DOT__vgc_inst__DOT__spr_fx[8U])
                           ? vlSelfRef.top__DOT__vgc_inst__DOT__sprite_shapes
                          [vlSelfRef.top__DOT__vgc_inst__DOT__spr_sa[8U]]
                           : (vlSelfRef.top__DOT__vgc_inst__DOT__sprite_shapes
                              [vlSelfRef.top__DOT__vgc_inst__DOT__spr_sa[8U]] 
                              >> 4U)));
    vlSelfRef.top__DOT__vgc_inst__DOT__spr_vis[8U] 
        = ((((((vlSelfRef.top__DOT__vgc_inst__DOT__spr_enable[8U] 
                & (IData)(vlSelfRef.top__DOT__vgc_inst__DOT__in_text_area)) 
               & VL_LTES_III(32, 0U, VL_EXTENDS_II(32,10, vlSelfRef.top__DOT__vgc_inst__DOT__spr_dx[8U]))) 
              & VL_GTS_III(32, 0x00000010U, VL_EXTENDS_II(32,10, vlSelfRef.top__DOT__vgc_inst__DOT__spr_dx[8U]))) 
             & VL_LTES_III(32, 0U, VL_EXTENDS_II(32,10, vlSelfRef.top__DOT__vgc_inst__DOT__spr_dy[8U]))) 
            & VL_GTS_III(32, 0x00000010U, VL_EXTENDS_II(32,10, vlSelfRef.top__DOT__vgc_inst__DOT__spr_dy[8U]))) 
           & (vlSelfRef.top__DOT__vgc_inst__DOT__spr_pix[8U] 
              != vlSelfRef.top__DOT__vgc_inst__DOT__spr_trans[8U]));
    vlSelfRef.top__DOT__vgc_inst__DOT__spr_dx[9U] = 
        (0x000003ffU & ((0x000001ffU & ((IData)(vlSelfRef.top__DOT__vgc_inst__DOT__h_count) 
                                        >> 1U)) - vlSelfRef.top__DOT__vgc_inst__DOT__spr_x[9U]));
    vlSelfRef.top__DOT__vgc_inst__DOT__spr_dy[9U] = 
        (0x000003ffU & ((IData)(vlSelfRef.top__DOT__vgc_inst__DOT__gfx_y) 
                        - vlSelfRef.top__DOT__vgc_inst__DOT__spr_y[9U]));
    vlSelfRef.top__DOT__vgc_inst__DOT__spr_fx[9U] = 
        (0x0000000fU & (vlSelfRef.top__DOT__vgc_inst__DOT__spr_flip_h[9U]
                         ? ((IData)(0x0fU) - vlSelfRef.top__DOT__vgc_inst__DOT__spr_dx[9U])
                         : vlSelfRef.top__DOT__vgc_inst__DOT__spr_dx[9U]));
    vlSelfRef.top__DOT__vgc_inst__DOT__spr_fy[9U] = 
        (0x0000000fU & (vlSelfRef.top__DOT__vgc_inst__DOT__spr_flip_v[9U]
                         ? ((IData)(0x0fU) - vlSelfRef.top__DOT__vgc_inst__DOT__spr_dy[9U])
                         : vlSelfRef.top__DOT__vgc_inst__DOT__spr_dy[9U]));
    vlSelfRef.top__DOT__vgc_inst__DOT__spr_sa[9U] = 
        ((vlSelfRef.top__DOT__vgc_inst__DOT__spr_shape[9U] 
          << 7U) | ((vlSelfRef.top__DOT__vgc_inst__DOT__spr_fy[9U] 
                     << 3U) | (7U & (vlSelfRef.top__DOT__vgc_inst__DOT__spr_fx[9U] 
                                     >> 1U))));
    vlSelfRef.top__DOT__vgc_inst__DOT__spr_pix[9U] 
        = (0x0000000fU & ((1U & vlSelfRef.top__DOT__vgc_inst__DOT__spr_fx[9U])
                           ? vlSelfRef.top__DOT__vgc_inst__DOT__sprite_shapes
                          [vlSelfRef.top__DOT__vgc_inst__DOT__spr_sa[9U]]
                           : (vlSelfRef.top__DOT__vgc_inst__DOT__sprite_shapes
                              [vlSelfRef.top__DOT__vgc_inst__DOT__spr_sa[9U]] 
                              >> 4U)));
    vlSelfRef.top__DOT__vgc_inst__DOT__spr_vis[9U] 
        = ((((((vlSelfRef.top__DOT__vgc_inst__DOT__spr_enable[9U] 
                & (IData)(vlSelfRef.top__DOT__vgc_inst__DOT__in_text_area)) 
               & VL_LTES_III(32, 0U, VL_EXTENDS_II(32,10, vlSelfRef.top__DOT__vgc_inst__DOT__spr_dx[9U]))) 
              & VL_GTS_III(32, 0x00000010U, VL_EXTENDS_II(32,10, vlSelfRef.top__DOT__vgc_inst__DOT__spr_dx[9U]))) 
             & VL_LTES_III(32, 0U, VL_EXTENDS_II(32,10, vlSelfRef.top__DOT__vgc_inst__DOT__spr_dy[9U]))) 
            & VL_GTS_III(32, 0x00000010U, VL_EXTENDS_II(32,10, vlSelfRef.top__DOT__vgc_inst__DOT__spr_dy[9U]))) 
           & (vlSelfRef.top__DOT__vgc_inst__DOT__spr_pix[9U] 
              != vlSelfRef.top__DOT__vgc_inst__DOT__spr_trans[9U]));
    vlSelfRef.top__DOT__vgc_inst__DOT__spr_dx[10U] 
        = (0x000003ffU & ((0x000001ffU & ((IData)(vlSelfRef.top__DOT__vgc_inst__DOT__h_count) 
                                          >> 1U)) - vlSelfRef.top__DOT__vgc_inst__DOT__spr_x[10U]));
    vlSelfRef.top__DOT__vgc_inst__DOT__spr_dy[10U] 
        = (0x000003ffU & ((IData)(vlSelfRef.top__DOT__vgc_inst__DOT__gfx_y) 
                          - vlSelfRef.top__DOT__vgc_inst__DOT__spr_y[10U]));
    vlSelfRef.top__DOT__vgc_inst__DOT__spr_fx[10U] 
        = (0x0000000fU & (vlSelfRef.top__DOT__vgc_inst__DOT__spr_flip_h[10U]
                           ? ((IData)(0x0fU) - vlSelfRef.top__DOT__vgc_inst__DOT__spr_dx[10U])
                           : vlSelfRef.top__DOT__vgc_inst__DOT__spr_dx[10U]));
    vlSelfRef.top__DOT__vgc_inst__DOT__spr_fy[10U] 
        = (0x0000000fU & (vlSelfRef.top__DOT__vgc_inst__DOT__spr_flip_v[10U]
                           ? ((IData)(0x0fU) - vlSelfRef.top__DOT__vgc_inst__DOT__spr_dy[10U])
                           : vlSelfRef.top__DOT__vgc_inst__DOT__spr_dy[10U]));
    vlSelfRef.top__DOT__vgc_inst__DOT__spr_sa[10U] 
        = ((vlSelfRef.top__DOT__vgc_inst__DOT__spr_shape[10U] 
            << 7U) | ((vlSelfRef.top__DOT__vgc_inst__DOT__spr_fy[10U] 
                       << 3U) | (7U & (vlSelfRef.top__DOT__vgc_inst__DOT__spr_fx[10U] 
                                       >> 1U))));
    vlSelfRef.top__DOT__vgc_inst__DOT__spr_pix[10U] 
        = (0x0000000fU & ((1U & vlSelfRef.top__DOT__vgc_inst__DOT__spr_fx[10U])
                           ? vlSelfRef.top__DOT__vgc_inst__DOT__sprite_shapes
                          [vlSelfRef.top__DOT__vgc_inst__DOT__spr_sa[10U]]
                           : (vlSelfRef.top__DOT__vgc_inst__DOT__sprite_shapes
                              [vlSelfRef.top__DOT__vgc_inst__DOT__spr_sa[10U]] 
                              >> 4U)));
    vlSelfRef.top__DOT__vgc_inst__DOT__spr_vis[10U] 
        = ((((((vlSelfRef.top__DOT__vgc_inst__DOT__spr_enable[10U] 
                & (IData)(vlSelfRef.top__DOT__vgc_inst__DOT__in_text_area)) 
               & VL_LTES_III(32, 0U, VL_EXTENDS_II(32,10, vlSelfRef.top__DOT__vgc_inst__DOT__spr_dx[10U]))) 
              & VL_GTS_III(32, 0x00000010U, VL_EXTENDS_II(32,10, vlSelfRef.top__DOT__vgc_inst__DOT__spr_dx[10U]))) 
             & VL_LTES_III(32, 0U, VL_EXTENDS_II(32,10, vlSelfRef.top__DOT__vgc_inst__DOT__spr_dy[10U]))) 
            & VL_GTS_III(32, 0x00000010U, VL_EXTENDS_II(32,10, vlSelfRef.top__DOT__vgc_inst__DOT__spr_dy[10U]))) 
           & (vlSelfRef.top__DOT__vgc_inst__DOT__spr_pix[10U] 
              != vlSelfRef.top__DOT__vgc_inst__DOT__spr_trans[10U]));
    vlSelfRef.top__DOT__vgc_inst__DOT__spr_dx[11U] 
        = (0x000003ffU & ((0x000001ffU & ((IData)(vlSelfRef.top__DOT__vgc_inst__DOT__h_count) 
                                          >> 1U)) - vlSelfRef.top__DOT__vgc_inst__DOT__spr_x[11U]));
    vlSelfRef.top__DOT__vgc_inst__DOT__spr_dy[11U] 
        = (0x000003ffU & ((IData)(vlSelfRef.top__DOT__vgc_inst__DOT__gfx_y) 
                          - vlSelfRef.top__DOT__vgc_inst__DOT__spr_y[11U]));
    vlSelfRef.top__DOT__vgc_inst__DOT__spr_fx[11U] 
        = (0x0000000fU & (vlSelfRef.top__DOT__vgc_inst__DOT__spr_flip_h[11U]
                           ? ((IData)(0x0fU) - vlSelfRef.top__DOT__vgc_inst__DOT__spr_dx[11U])
                           : vlSelfRef.top__DOT__vgc_inst__DOT__spr_dx[11U]));
    vlSelfRef.top__DOT__vgc_inst__DOT__spr_fy[11U] 
        = (0x0000000fU & (vlSelfRef.top__DOT__vgc_inst__DOT__spr_flip_v[11U]
                           ? ((IData)(0x0fU) - vlSelfRef.top__DOT__vgc_inst__DOT__spr_dy[11U])
                           : vlSelfRef.top__DOT__vgc_inst__DOT__spr_dy[11U]));
    vlSelfRef.top__DOT__vgc_inst__DOT__spr_sa[11U] 
        = ((vlSelfRef.top__DOT__vgc_inst__DOT__spr_shape[11U] 
            << 7U) | ((vlSelfRef.top__DOT__vgc_inst__DOT__spr_fy[11U] 
                       << 3U) | (7U & (vlSelfRef.top__DOT__vgc_inst__DOT__spr_fx[11U] 
                                       >> 1U))));
    vlSelfRef.top__DOT__vgc_inst__DOT__spr_pix[11U] 
        = (0x0000000fU & ((1U & vlSelfRef.top__DOT__vgc_inst__DOT__spr_fx[11U])
                           ? vlSelfRef.top__DOT__vgc_inst__DOT__sprite_shapes
                          [vlSelfRef.top__DOT__vgc_inst__DOT__spr_sa[11U]]
                           : (vlSelfRef.top__DOT__vgc_inst__DOT__sprite_shapes
                              [vlSelfRef.top__DOT__vgc_inst__DOT__spr_sa[11U]] 
                              >> 4U)));
    vlSelfRef.top__DOT__vgc_inst__DOT__spr_vis[11U] 
        = ((((((vlSelfRef.top__DOT__vgc_inst__DOT__spr_enable[11U] 
                & (IData)(vlSelfRef.top__DOT__vgc_inst__DOT__in_text_area)) 
               & VL_LTES_III(32, 0U, VL_EXTENDS_II(32,10, vlSelfRef.top__DOT__vgc_inst__DOT__spr_dx[11U]))) 
              & VL_GTS_III(32, 0x00000010U, VL_EXTENDS_II(32,10, vlSelfRef.top__DOT__vgc_inst__DOT__spr_dx[11U]))) 
             & VL_LTES_III(32, 0U, VL_EXTENDS_II(32,10, vlSelfRef.top__DOT__vgc_inst__DOT__spr_dy[11U]))) 
            & VL_GTS_III(32, 0x00000010U, VL_EXTENDS_II(32,10, vlSelfRef.top__DOT__vgc_inst__DOT__spr_dy[11U]))) 
           & (vlSelfRef.top__DOT__vgc_inst__DOT__spr_pix[11U] 
              != vlSelfRef.top__DOT__vgc_inst__DOT__spr_trans[11U]));
    vlSelfRef.top__DOT__vgc_inst__DOT__spr_dx[12U] 
        = (0x000003ffU & ((0x000001ffU & ((IData)(vlSelfRef.top__DOT__vgc_inst__DOT__h_count) 
                                          >> 1U)) - vlSelfRef.top__DOT__vgc_inst__DOT__spr_x[12U]));
    vlSelfRef.top__DOT__vgc_inst__DOT__spr_dy[12U] 
        = (0x000003ffU & ((IData)(vlSelfRef.top__DOT__vgc_inst__DOT__gfx_y) 
                          - vlSelfRef.top__DOT__vgc_inst__DOT__spr_y[12U]));
    vlSelfRef.top__DOT__vgc_inst__DOT__spr_fx[12U] 
        = (0x0000000fU & (vlSelfRef.top__DOT__vgc_inst__DOT__spr_flip_h[12U]
                           ? ((IData)(0x0fU) - vlSelfRef.top__DOT__vgc_inst__DOT__spr_dx[12U])
                           : vlSelfRef.top__DOT__vgc_inst__DOT__spr_dx[12U]));
    vlSelfRef.top__DOT__vgc_inst__DOT__spr_fy[12U] 
        = (0x0000000fU & (vlSelfRef.top__DOT__vgc_inst__DOT__spr_flip_v[12U]
                           ? ((IData)(0x0fU) - vlSelfRef.top__DOT__vgc_inst__DOT__spr_dy[12U])
                           : vlSelfRef.top__DOT__vgc_inst__DOT__spr_dy[12U]));
    vlSelfRef.top__DOT__vgc_inst__DOT__spr_sa[12U] 
        = ((vlSelfRef.top__DOT__vgc_inst__DOT__spr_shape[12U] 
            << 7U) | ((vlSelfRef.top__DOT__vgc_inst__DOT__spr_fy[12U] 
                       << 3U) | (7U & (vlSelfRef.top__DOT__vgc_inst__DOT__spr_fx[12U] 
                                       >> 1U))));
    vlSelfRef.top__DOT__vgc_inst__DOT__spr_pix[12U] 
        = (0x0000000fU & ((1U & vlSelfRef.top__DOT__vgc_inst__DOT__spr_fx[12U])
                           ? vlSelfRef.top__DOT__vgc_inst__DOT__sprite_shapes
                          [vlSelfRef.top__DOT__vgc_inst__DOT__spr_sa[12U]]
                           : (vlSelfRef.top__DOT__vgc_inst__DOT__sprite_shapes
                              [vlSelfRef.top__DOT__vgc_inst__DOT__spr_sa[12U]] 
                              >> 4U)));
    vlSelfRef.top__DOT__vgc_inst__DOT__spr_vis[12U] 
        = ((((((vlSelfRef.top__DOT__vgc_inst__DOT__spr_enable[12U] 
                & (IData)(vlSelfRef.top__DOT__vgc_inst__DOT__in_text_area)) 
               & VL_LTES_III(32, 0U, VL_EXTENDS_II(32,10, vlSelfRef.top__DOT__vgc_inst__DOT__spr_dx[12U]))) 
              & VL_GTS_III(32, 0x00000010U, VL_EXTENDS_II(32,10, vlSelfRef.top__DOT__vgc_inst__DOT__spr_dx[12U]))) 
             & VL_LTES_III(32, 0U, VL_EXTENDS_II(32,10, vlSelfRef.top__DOT__vgc_inst__DOT__spr_dy[12U]))) 
            & VL_GTS_III(32, 0x00000010U, VL_EXTENDS_II(32,10, vlSelfRef.top__DOT__vgc_inst__DOT__spr_dy[12U]))) 
           & (vlSelfRef.top__DOT__vgc_inst__DOT__spr_pix[12U] 
              != vlSelfRef.top__DOT__vgc_inst__DOT__spr_trans[12U]));
    vlSelfRef.top__DOT__vgc_inst__DOT__spr_dx[13U] 
        = (0x000003ffU & ((0x000001ffU & ((IData)(vlSelfRef.top__DOT__vgc_inst__DOT__h_count) 
                                          >> 1U)) - vlSelfRef.top__DOT__vgc_inst__DOT__spr_x[13U]));
    vlSelfRef.top__DOT__vgc_inst__DOT__spr_dy[13U] 
        = (0x000003ffU & ((IData)(vlSelfRef.top__DOT__vgc_inst__DOT__gfx_y) 
                          - vlSelfRef.top__DOT__vgc_inst__DOT__spr_y[13U]));
    vlSelfRef.top__DOT__vgc_inst__DOT__spr_fx[13U] 
        = (0x0000000fU & (vlSelfRef.top__DOT__vgc_inst__DOT__spr_flip_h[13U]
                           ? ((IData)(0x0fU) - vlSelfRef.top__DOT__vgc_inst__DOT__spr_dx[13U])
                           : vlSelfRef.top__DOT__vgc_inst__DOT__spr_dx[13U]));
    vlSelfRef.top__DOT__vgc_inst__DOT__spr_fy[13U] 
        = (0x0000000fU & (vlSelfRef.top__DOT__vgc_inst__DOT__spr_flip_v[13U]
                           ? ((IData)(0x0fU) - vlSelfRef.top__DOT__vgc_inst__DOT__spr_dy[13U])
                           : vlSelfRef.top__DOT__vgc_inst__DOT__spr_dy[13U]));
    vlSelfRef.top__DOT__vgc_inst__DOT__spr_sa[13U] 
        = ((vlSelfRef.top__DOT__vgc_inst__DOT__spr_shape[13U] 
            << 7U) | ((vlSelfRef.top__DOT__vgc_inst__DOT__spr_fy[13U] 
                       << 3U) | (7U & (vlSelfRef.top__DOT__vgc_inst__DOT__spr_fx[13U] 
                                       >> 1U))));
    vlSelfRef.top__DOT__vgc_inst__DOT__spr_pix[13U] 
        = (0x0000000fU & ((1U & vlSelfRef.top__DOT__vgc_inst__DOT__spr_fx[13U])
                           ? vlSelfRef.top__DOT__vgc_inst__DOT__sprite_shapes
                          [vlSelfRef.top__DOT__vgc_inst__DOT__spr_sa[13U]]
                           : (vlSelfRef.top__DOT__vgc_inst__DOT__sprite_shapes
                              [vlSelfRef.top__DOT__vgc_inst__DOT__spr_sa[13U]] 
                              >> 4U)));
    vlSelfRef.top__DOT__vgc_inst__DOT__spr_vis[13U] 
        = ((((((vlSelfRef.top__DOT__vgc_inst__DOT__spr_enable[13U] 
                & (IData)(vlSelfRef.top__DOT__vgc_inst__DOT__in_text_area)) 
               & VL_LTES_III(32, 0U, VL_EXTENDS_II(32,10, vlSelfRef.top__DOT__vgc_inst__DOT__spr_dx[13U]))) 
              & VL_GTS_III(32, 0x00000010U, VL_EXTENDS_II(32,10, vlSelfRef.top__DOT__vgc_inst__DOT__spr_dx[13U]))) 
             & VL_LTES_III(32, 0U, VL_EXTENDS_II(32,10, vlSelfRef.top__DOT__vgc_inst__DOT__spr_dy[13U]))) 
            & VL_GTS_III(32, 0x00000010U, VL_EXTENDS_II(32,10, vlSelfRef.top__DOT__vgc_inst__DOT__spr_dy[13U]))) 
           & (vlSelfRef.top__DOT__vgc_inst__DOT__spr_pix[13U] 
              != vlSelfRef.top__DOT__vgc_inst__DOT__spr_trans[13U]));
    vlSelfRef.top__DOT__vgc_inst__DOT__spr_dx[14U] 
        = (0x000003ffU & ((0x000001ffU & ((IData)(vlSelfRef.top__DOT__vgc_inst__DOT__h_count) 
                                          >> 1U)) - vlSelfRef.top__DOT__vgc_inst__DOT__spr_x[14U]));
    vlSelfRef.top__DOT__vgc_inst__DOT__spr_dy[14U] 
        = (0x000003ffU & ((IData)(vlSelfRef.top__DOT__vgc_inst__DOT__gfx_y) 
                          - vlSelfRef.top__DOT__vgc_inst__DOT__spr_y[14U]));
    vlSelfRef.top__DOT__vgc_inst__DOT__spr_fx[14U] 
        = (0x0000000fU & (vlSelfRef.top__DOT__vgc_inst__DOT__spr_flip_h[14U]
                           ? ((IData)(0x0fU) - vlSelfRef.top__DOT__vgc_inst__DOT__spr_dx[14U])
                           : vlSelfRef.top__DOT__vgc_inst__DOT__spr_dx[14U]));
    vlSelfRef.top__DOT__vgc_inst__DOT__spr_fy[14U] 
        = (0x0000000fU & (vlSelfRef.top__DOT__vgc_inst__DOT__spr_flip_v[14U]
                           ? ((IData)(0x0fU) - vlSelfRef.top__DOT__vgc_inst__DOT__spr_dy[14U])
                           : vlSelfRef.top__DOT__vgc_inst__DOT__spr_dy[14U]));
    vlSelfRef.top__DOT__vgc_inst__DOT__spr_sa[14U] 
        = ((vlSelfRef.top__DOT__vgc_inst__DOT__spr_shape[14U] 
            << 7U) | ((vlSelfRef.top__DOT__vgc_inst__DOT__spr_fy[14U] 
                       << 3U) | (7U & (vlSelfRef.top__DOT__vgc_inst__DOT__spr_fx[14U] 
                                       >> 1U))));
    vlSelfRef.top__DOT__vgc_inst__DOT__spr_pix[14U] 
        = (0x0000000fU & ((1U & vlSelfRef.top__DOT__vgc_inst__DOT__spr_fx[14U])
                           ? vlSelfRef.top__DOT__vgc_inst__DOT__sprite_shapes
                          [vlSelfRef.top__DOT__vgc_inst__DOT__spr_sa[14U]]
                           : (vlSelfRef.top__DOT__vgc_inst__DOT__sprite_shapes
                              [vlSelfRef.top__DOT__vgc_inst__DOT__spr_sa[14U]] 
                              >> 4U)));
    vlSelfRef.top__DOT__vgc_inst__DOT__spr_vis[14U] 
        = ((((((vlSelfRef.top__DOT__vgc_inst__DOT__spr_enable[14U] 
                & (IData)(vlSelfRef.top__DOT__vgc_inst__DOT__in_text_area)) 
               & VL_LTES_III(32, 0U, VL_EXTENDS_II(32,10, vlSelfRef.top__DOT__vgc_inst__DOT__spr_dx[14U]))) 
              & VL_GTS_III(32, 0x00000010U, VL_EXTENDS_II(32,10, vlSelfRef.top__DOT__vgc_inst__DOT__spr_dx[14U]))) 
             & VL_LTES_III(32, 0U, VL_EXTENDS_II(32,10, vlSelfRef.top__DOT__vgc_inst__DOT__spr_dy[14U]))) 
            & VL_GTS_III(32, 0x00000010U, VL_EXTENDS_II(32,10, vlSelfRef.top__DOT__vgc_inst__DOT__spr_dy[14U]))) 
           & (vlSelfRef.top__DOT__vgc_inst__DOT__spr_pix[14U] 
              != vlSelfRef.top__DOT__vgc_inst__DOT__spr_trans[14U]));
    vlSelfRef.top__DOT__vgc_inst__DOT__spr_dx[15U] 
        = (0x000003ffU & ((0x000001ffU & ((IData)(vlSelfRef.top__DOT__vgc_inst__DOT__h_count) 
                                          >> 1U)) - vlSelfRef.top__DOT__vgc_inst__DOT__spr_x[15U]));
    vlSelfRef.top__DOT__vgc_inst__DOT__spr_dy[15U] 
        = (0x000003ffU & ((IData)(vlSelfRef.top__DOT__vgc_inst__DOT__gfx_y) 
                          - vlSelfRef.top__DOT__vgc_inst__DOT__spr_y[15U]));
    vlSelfRef.top__DOT__vgc_inst__DOT__spr_fx[15U] 
        = (0x0000000fU & (vlSelfRef.top__DOT__vgc_inst__DOT__spr_flip_h[15U]
                           ? ((IData)(0x0fU) - vlSelfRef.top__DOT__vgc_inst__DOT__spr_dx[15U])
                           : vlSelfRef.top__DOT__vgc_inst__DOT__spr_dx[15U]));
    vlSelfRef.top__DOT__vgc_inst__DOT__spr_fy[15U] 
        = (0x0000000fU & (vlSelfRef.top__DOT__vgc_inst__DOT__spr_flip_v[15U]
                           ? ((IData)(0x0fU) - vlSelfRef.top__DOT__vgc_inst__DOT__spr_dy[15U])
                           : vlSelfRef.top__DOT__vgc_inst__DOT__spr_dy[15U]));
    vlSelfRef.top__DOT__vgc_inst__DOT__spr_sa[15U] 
        = ((vlSelfRef.top__DOT__vgc_inst__DOT__spr_shape[15U] 
            << 7U) | ((vlSelfRef.top__DOT__vgc_inst__DOT__spr_fy[15U] 
                       << 3U) | (7U & (vlSelfRef.top__DOT__vgc_inst__DOT__spr_fx[15U] 
                                       >> 1U))));
    vlSelfRef.top__DOT__vgc_inst__DOT__spr_pix[15U] 
        = (0x0000000fU & ((1U & vlSelfRef.top__DOT__vgc_inst__DOT__spr_fx[15U])
                           ? vlSelfRef.top__DOT__vgc_inst__DOT__sprite_shapes
                          [vlSelfRef.top__DOT__vgc_inst__DOT__spr_sa[15U]]
                           : (vlSelfRef.top__DOT__vgc_inst__DOT__sprite_shapes
                              [vlSelfRef.top__DOT__vgc_inst__DOT__spr_sa[15U]] 
                              >> 4U)));
    vlSelfRef.top__DOT__vgc_inst__DOT__spr_vis[15U] 
        = ((((((vlSelfRef.top__DOT__vgc_inst__DOT__spr_enable[15U] 
                & (IData)(vlSelfRef.top__DOT__vgc_inst__DOT__in_text_area)) 
               & VL_LTES_III(32, 0U, VL_EXTENDS_II(32,10, vlSelfRef.top__DOT__vgc_inst__DOT__spr_dx[15U]))) 
              & VL_GTS_III(32, 0x00000010U, VL_EXTENDS_II(32,10, vlSelfRef.top__DOT__vgc_inst__DOT__spr_dx[15U]))) 
             & VL_LTES_III(32, 0U, VL_EXTENDS_II(32,10, vlSelfRef.top__DOT__vgc_inst__DOT__spr_dy[15U]))) 
            & VL_GTS_III(32, 0x00000010U, VL_EXTENDS_II(32,10, vlSelfRef.top__DOT__vgc_inst__DOT__spr_dy[15U]))) 
           & (vlSelfRef.top__DOT__vgc_inst__DOT__spr_pix[15U] 
              != vlSelfRef.top__DOT__vgc_inst__DOT__spr_trans[15U]));
    top__DOT__vgc_inst__DOT__spr_pixel = 0U;
    top__DOT__vgc_inst__DOT__spr_pixel_pri = 0U;
    top__DOT__vgc_inst__DOT__spr_pixel_hit = 0U;
    if (vlSelfRef.top__DOT__vgc_inst__DOT__spr_vis[0U]) {
        top__DOT__vgc_inst__DOT__spr_pixel = vlSelfRef.top__DOT__vgc_inst__DOT__spr_pix[0U];
        top__DOT__vgc_inst__DOT__spr_pixel_pri = vlSelfRef.top__DOT__vgc_inst__DOT__spr_pri[0U];
        top__DOT__vgc_inst__DOT__spr_pixel_hit = 1U;
    }
    if (vlSelfRef.top__DOT__vgc_inst__DOT__spr_vis[1U]) {
        top__DOT__vgc_inst__DOT__spr_pixel = vlSelfRef.top__DOT__vgc_inst__DOT__spr_pix[1U];
        top__DOT__vgc_inst__DOT__spr_pixel_pri = vlSelfRef.top__DOT__vgc_inst__DOT__spr_pri[1U];
        top__DOT__vgc_inst__DOT__spr_pixel_hit = 1U;
    }
    if (vlSelfRef.top__DOT__vgc_inst__DOT__spr_vis[2U]) {
        top__DOT__vgc_inst__DOT__spr_pixel = vlSelfRef.top__DOT__vgc_inst__DOT__spr_pix[2U];
        top__DOT__vgc_inst__DOT__spr_pixel_pri = vlSelfRef.top__DOT__vgc_inst__DOT__spr_pri[2U];
        top__DOT__vgc_inst__DOT__spr_pixel_hit = 1U;
    }
    if (vlSelfRef.top__DOT__vgc_inst__DOT__spr_vis[3U]) {
        top__DOT__vgc_inst__DOT__spr_pixel = vlSelfRef.top__DOT__vgc_inst__DOT__spr_pix[3U];
        top__DOT__vgc_inst__DOT__spr_pixel_pri = vlSelfRef.top__DOT__vgc_inst__DOT__spr_pri[3U];
        top__DOT__vgc_inst__DOT__spr_pixel_hit = 1U;
    }
    if (vlSelfRef.top__DOT__vgc_inst__DOT__spr_vis[4U]) {
        top__DOT__vgc_inst__DOT__spr_pixel = vlSelfRef.top__DOT__vgc_inst__DOT__spr_pix[4U];
        top__DOT__vgc_inst__DOT__spr_pixel_pri = vlSelfRef.top__DOT__vgc_inst__DOT__spr_pri[4U];
        top__DOT__vgc_inst__DOT__spr_pixel_hit = 1U;
    }
    if (vlSelfRef.top__DOT__vgc_inst__DOT__spr_vis[5U]) {
        top__DOT__vgc_inst__DOT__spr_pixel = vlSelfRef.top__DOT__vgc_inst__DOT__spr_pix[5U];
        top__DOT__vgc_inst__DOT__spr_pixel_pri = vlSelfRef.top__DOT__vgc_inst__DOT__spr_pri[5U];
        top__DOT__vgc_inst__DOT__spr_pixel_hit = 1U;
    }
    if (vlSelfRef.top__DOT__vgc_inst__DOT__spr_vis[6U]) {
        top__DOT__vgc_inst__DOT__spr_pixel = vlSelfRef.top__DOT__vgc_inst__DOT__spr_pix[6U];
        top__DOT__vgc_inst__DOT__spr_pixel_pri = vlSelfRef.top__DOT__vgc_inst__DOT__spr_pri[6U];
        top__DOT__vgc_inst__DOT__spr_pixel_hit = 1U;
    }
    if (vlSelfRef.top__DOT__vgc_inst__DOT__spr_vis[7U]) {
        top__DOT__vgc_inst__DOT__spr_pixel = vlSelfRef.top__DOT__vgc_inst__DOT__spr_pix[7U];
        top__DOT__vgc_inst__DOT__spr_pixel_pri = vlSelfRef.top__DOT__vgc_inst__DOT__spr_pri[7U];
        top__DOT__vgc_inst__DOT__spr_pixel_hit = 1U;
    }
    if (vlSelfRef.top__DOT__vgc_inst__DOT__spr_vis[8U]) {
        top__DOT__vgc_inst__DOT__spr_pixel = vlSelfRef.top__DOT__vgc_inst__DOT__spr_pix[8U];
        top__DOT__vgc_inst__DOT__spr_pixel_pri = vlSelfRef.top__DOT__vgc_inst__DOT__spr_pri[8U];
        top__DOT__vgc_inst__DOT__spr_pixel_hit = 1U;
    }
    if (vlSelfRef.top__DOT__vgc_inst__DOT__spr_vis[9U]) {
        top__DOT__vgc_inst__DOT__spr_pixel = vlSelfRef.top__DOT__vgc_inst__DOT__spr_pix[9U];
        top__DOT__vgc_inst__DOT__spr_pixel_pri = vlSelfRef.top__DOT__vgc_inst__DOT__spr_pri[9U];
        top__DOT__vgc_inst__DOT__spr_pixel_hit = 1U;
    }
    if (vlSelfRef.top__DOT__vgc_inst__DOT__spr_vis[10U]) {
        top__DOT__vgc_inst__DOT__spr_pixel = vlSelfRef.top__DOT__vgc_inst__DOT__spr_pix[10U];
        top__DOT__vgc_inst__DOT__spr_pixel_pri = vlSelfRef.top__DOT__vgc_inst__DOT__spr_pri[10U];
        top__DOT__vgc_inst__DOT__spr_pixel_hit = 1U;
    }
    if (vlSelfRef.top__DOT__vgc_inst__DOT__spr_vis[11U]) {
        top__DOT__vgc_inst__DOT__spr_pixel = vlSelfRef.top__DOT__vgc_inst__DOT__spr_pix[11U];
        top__DOT__vgc_inst__DOT__spr_pixel_pri = vlSelfRef.top__DOT__vgc_inst__DOT__spr_pri[11U];
        top__DOT__vgc_inst__DOT__spr_pixel_hit = 1U;
    }
    if (vlSelfRef.top__DOT__vgc_inst__DOT__spr_vis[12U]) {
        top__DOT__vgc_inst__DOT__spr_pixel = vlSelfRef.top__DOT__vgc_inst__DOT__spr_pix[12U];
        top__DOT__vgc_inst__DOT__spr_pixel_pri = vlSelfRef.top__DOT__vgc_inst__DOT__spr_pri[12U];
        top__DOT__vgc_inst__DOT__spr_pixel_hit = 1U;
    }
    if (vlSelfRef.top__DOT__vgc_inst__DOT__spr_vis[13U]) {
        top__DOT__vgc_inst__DOT__spr_pixel = vlSelfRef.top__DOT__vgc_inst__DOT__spr_pix[13U];
        top__DOT__vgc_inst__DOT__spr_pixel_pri = vlSelfRef.top__DOT__vgc_inst__DOT__spr_pri[13U];
        top__DOT__vgc_inst__DOT__spr_pixel_hit = 1U;
    }
    if (vlSelfRef.top__DOT__vgc_inst__DOT__spr_vis[14U]) {
        top__DOT__vgc_inst__DOT__spr_pixel = vlSelfRef.top__DOT__vgc_inst__DOT__spr_pix[14U];
        top__DOT__vgc_inst__DOT__spr_pixel_pri = vlSelfRef.top__DOT__vgc_inst__DOT__spr_pri[14U];
        top__DOT__vgc_inst__DOT__spr_pixel_hit = 1U;
    }
    if (vlSelfRef.top__DOT__vgc_inst__DOT__spr_vis[15U]) {
        top__DOT__vgc_inst__DOT__spr_pixel = vlSelfRef.top__DOT__vgc_inst__DOT__spr_pix[15U];
        top__DOT__vgc_inst__DOT__spr_pixel_pri = vlSelfRef.top__DOT__vgc_inst__DOT__spr_pri[15U];
        top__DOT__vgc_inst__DOT__spr_pixel_hit = 1U;
    }
    if (vlSelfRef.top__DOT__vgc_inst__DOT__visible) {
        if (vlSelfRef.top__DOT__vgc_inst__DOT__in_text_area) {
            vlSelfRef.top__DOT__vgc_inst__DOT__pixel_color 
                = ((4U & (IData)(vlSelfRef.top__DOT__vgc_inst__DOT__mode))
                    ? ((2U & (IData)(vlSelfRef.top__DOT__vgc_inst__DOT__mode))
                        ? (IData)(top__DOT__vgc_inst__DOT__text_pixel)
                        : ((1U & (IData)(vlSelfRef.top__DOT__vgc_inst__DOT__mode))
                            ? (IData)(top__DOT__vgc_inst__DOT__text_pixel)
                            : ((0U != (IData)(top__DOT__vgc_inst__DOT__tile_opaque))
                                ? ((IData)(__VdfgRegularize_hebeb780c_0_25)
                                    ? vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__pal_ram
                                   [((0x000000f0U & 
                                      ((IData)(top__DOT__vgc_inst__DOT__tile_inst__DOT__cur_attr) 
                                       << 4U)) | (IData)(top__DOT__vgc_inst__DOT__tile_inst__DOT__color_idx))]
                                    : 0U) : vlSelfRef.top__DOT__vgc_inst__DOT__palette
                               [vlSelfRef.top__DOT__vgc_inst__DOT__bg_color])))
                    : ((2U & (IData)(vlSelfRef.top__DOT__vgc_inst__DOT__mode))
                        ? ((1U & (IData)(vlSelfRef.top__DOT__vgc_inst__DOT__mode))
                            ? ((0U != (IData)(top__DOT__vgc_inst__DOT__cur_gfx))
                                ? (IData)(top__DOT__vgc_inst__DOT__gfx_pixel)
                                : vlSelfRef.top__DOT__vgc_inst__DOT__palette
                               [vlSelfRef.top__DOT__vgc_inst__DOT__bg_color])
                            : ((IData)(top__DOT__vgc_inst__DOT__pixel_on)
                                ? vlSelfRef.top__DOT__vgc_inst__DOT__palette
                               [top__DOT__vgc_inst__DOT__cur_fg]
                                : ((0U != (IData)(top__DOT__vgc_inst__DOT__cur_gfx))
                                    ? (IData)(top__DOT__vgc_inst__DOT__gfx_pixel)
                                    : vlSelfRef.top__DOT__vgc_inst__DOT__palette
                                   [vlSelfRef.top__DOT__vgc_inst__DOT__bg_color])))
                        : ((1U & (IData)(vlSelfRef.top__DOT__vgc_inst__DOT__mode))
                            ? ((0U != (IData)(top__DOT__vgc_inst__DOT__cur_gfx))
                                ? (IData)(top__DOT__vgc_inst__DOT__gfx_pixel)
                                : (IData)(top__DOT__vgc_inst__DOT__text_pixel))
                            : (IData)(top__DOT__vgc_inst__DOT__text_pixel))));
            if (top__DOT__vgc_inst__DOT__spr_pixel_hit) {
                if ((4U == (IData)(vlSelfRef.top__DOT__vgc_inst__DOT__mode))) {
                    if ((0U == (IData)(top__DOT__vgc_inst__DOT__spr_pixel_pri))) {
                        if ((0U == (IData)(top__DOT__vgc_inst__DOT__tile_opaque))) {
                            vlSelfRef.top__DOT__vgc_inst__DOT__pixel_color 
                                = vlSelfRef.top__DOT__vgc_inst__DOT__palette
                                [top__DOT__vgc_inst__DOT__spr_pixel];
                        }
                    } else if ((1U == (IData)(top__DOT__vgc_inst__DOT__spr_pixel_pri))) {
                        if ((2U > (IData)(top__DOT__vgc_inst__DOT__tile_opaque))) {
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
                    if (((~ (IData)(top__DOT__vgc_inst__DOT__pixel_on)) 
                         & (0U == (IData)(top__DOT__vgc_inst__DOT__cur_gfx)))) {
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
}
