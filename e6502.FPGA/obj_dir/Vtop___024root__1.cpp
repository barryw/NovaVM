// Verilated -*- C++ -*-
// DESCRIPTION: Verilator output: Design implementation internals
// See Vtop.h for the primary calling header

#include "Vtop__pch.h"

void Vtop___024root___nba_sequent__TOP__1(Vtop___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vtop___024root___nba_sequent__TOP__1\n"); );
    Vtop__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
    // Body
    vlSelfRef.__Vdly__top__DOT__cpu_inst__DOT__state 
        = vlSelfRef.top__DOT__cpu_inst__DOT__state;
    if (vlSelfRef.rst) {
        vlSelfRef.__Vdly__top__DOT__cpu_inst__DOT__state = 8U;
    } else if (vlSelfRef.top__DOT____Vcellinp__cpu_inst__RDY) {
        if (((((((((0x0cU == (IData)(vlSelfRef.top__DOT__cpu_inst__DOT__state)) 
                   | (0x2fU == (IData)(vlSelfRef.top__DOT__cpu_inst__DOT__state))) 
                  | (0x30U == (IData)(vlSelfRef.top__DOT__cpu_inst__DOT__state))) 
                 | (0x31U == (IData)(vlSelfRef.top__DOT__cpu_inst__DOT__state))) 
                | (0U == (IData)(vlSelfRef.top__DOT__cpu_inst__DOT__state))) 
               | (1U == (IData)(vlSelfRef.top__DOT__cpu_inst__DOT__state))) 
              | (2U == (IData)(vlSelfRef.top__DOT__cpu_inst__DOT__state))) 
             | (3U == (IData)(vlSelfRef.top__DOT__cpu_inst__DOT__state)))) {
            if ((0x0cU == (IData)(vlSelfRef.top__DOT__cpu_inst__DOT__state))) {
                if (((((((((0U == (IData)(vlSelfRef.top__DOT__cpu_inst__DOT__IR)) 
                           | (0x20U == (IData)(vlSelfRef.top__DOT__cpu_inst__DOT__IR))) 
                          | (0x2cU == (IData)(vlSelfRef.top__DOT__cpu_inst__DOT__IR))) 
                         | (0x40U == (IData)(vlSelfRef.top__DOT__cpu_inst__DOT__IR))) 
                        | (0x4cU == (IData)(vlSelfRef.top__DOT__cpu_inst__DOT__IR))) 
                       | (0x60U == (IData)(vlSelfRef.top__DOT__cpu_inst__DOT__IR))) 
                      | (0x6cU == (IData)(vlSelfRef.top__DOT__cpu_inst__DOT__IR))) 
                     | (8U == (0xbfU & (IData)(vlSelfRef.top__DOT__cpu_inst__DOT__IR))))) {
                    vlSelfRef.__Vdly__top__DOT__cpu_inst__DOT__state 
                        = ((0U == (IData)(vlSelfRef.top__DOT__cpu_inst__DOT__IR))
                            ? 8U : ((0x20U == (IData)(vlSelfRef.top__DOT__cpu_inst__DOT__IR))
                                     ? 0x1aU : ((0x2cU 
                                                 == (IData)(vlSelfRef.top__DOT__cpu_inst__DOT__IR))
                                                 ? 0U
                                                 : 
                                                ((0x40U 
                                                  == (IData)(vlSelfRef.top__DOT__cpu_inst__DOT__IR))
                                                  ? 0x25U
                                                  : 
                                                 ((0x4cU 
                                                   == (IData)(vlSelfRef.top__DOT__cpu_inst__DOT__IR))
                                                   ? 0x16U
                                                   : 
                                                  ((0x60U 
                                                    == (IData)(vlSelfRef.top__DOT__cpu_inst__DOT__IR))
                                                    ? 0x2aU
                                                    : 
                                                   ((0x6cU 
                                                     == (IData)(vlSelfRef.top__DOT__cpu_inst__DOT__IR))
                                                     ? 0x18U
                                                     : 0x21U)))))));
                } else if (((((((((0x28U == (0xbfU 
                                             & (IData)(vlSelfRef.top__DOT__cpu_inst__DOT__IR))) 
                                  | (0x18U == (0x9fU 
                                               & (IData)(vlSelfRef.top__DOT__cpu_inst__DOT__IR)))) 
                                 | (0x80U == (0x9dU 
                                              & (IData)(vlSelfRef.top__DOT__cpu_inst__DOT__IR)))) 
                                | (0x8cU == (0x9fU 
                                             & (IData)(vlSelfRef.top__DOT__cpu_inst__DOT__IR)))) 
                               | (0x88U == (0x8fU & (IData)(vlSelfRef.top__DOT__cpu_inst__DOT__IR)))) 
                              | (1U == (0x1fU & (IData)(vlSelfRef.top__DOT__cpu_inst__DOT__IR)))) 
                             | (4U == (0x1cU & (IData)(vlSelfRef.top__DOT__cpu_inst__DOT__IR)))) 
                            | (9U == (0x1fU & (IData)(vlSelfRef.top__DOT__cpu_inst__DOT__IR))))) {
                    vlSelfRef.__Vdly__top__DOT__cpu_inst__DOT__state 
                        = ((0x28U == (0xbfU & (IData)(vlSelfRef.top__DOT__cpu_inst__DOT__IR)))
                            ? 0x1eU : ((0x18U == (0x9fU 
                                                  & (IData)(vlSelfRef.top__DOT__cpu_inst__DOT__IR)))
                                        ? 0x24U : (
                                                   (0x80U 
                                                    == 
                                                    (0x9dU 
                                                     & (IData)(vlSelfRef.top__DOT__cpu_inst__DOT__IR)))
                                                    ? 0x0dU
                                                    : 
                                                   ((0x8cU 
                                                     == 
                                                     (0x9fU 
                                                      & (IData)(vlSelfRef.top__DOT__cpu_inst__DOT__IR)))
                                                     ? 0U
                                                     : 
                                                    ((0x88U 
                                                      == 
                                                      (0x8fU 
                                                       & (IData)(vlSelfRef.top__DOT__cpu_inst__DOT__IR)))
                                                      ? 0x24U
                                                      : 
                                                     ((1U 
                                                       == 
                                                       (0x1fU 
                                                        & (IData)(vlSelfRef.top__DOT__cpu_inst__DOT__IR)))
                                                       ? 0x0eU
                                                       : 
                                                      ((4U 
                                                        == 
                                                        (0x1cU 
                                                         & (IData)(vlSelfRef.top__DOT__cpu_inst__DOT__IR)))
                                                        ? 0x2fU
                                                        : 0x0dU)))))));
                } else if (((((((((0x0dU == (0x1fU 
                                             & (IData)(vlSelfRef.top__DOT__cpu_inst__DOT__IR))) 
                                  | (0x0eU == (0x1fU 
                                               & (IData)(vlSelfRef.top__DOT__cpu_inst__DOT__IR)))) 
                                 | (0x10U == (0x1fU 
                                              & (IData)(vlSelfRef.top__DOT__cpu_inst__DOT__IR)))) 
                                | (0x11U == (0x1fU 
                                             & (IData)(vlSelfRef.top__DOT__cpu_inst__DOT__IR)))) 
                               | (0x14U == (0x1cU & (IData)(vlSelfRef.top__DOT__cpu_inst__DOT__IR)))) 
                              | (0x19U == (0x1fU & (IData)(vlSelfRef.top__DOT__cpu_inst__DOT__IR)))) 
                             | (0x1cU == (0x1cU & (IData)(vlSelfRef.top__DOT__cpu_inst__DOT__IR)))) 
                            | (0x0aU == (0x0fU & (IData)(vlSelfRef.top__DOT__cpu_inst__DOT__IR))))) {
                    vlSelfRef.__Vdly__top__DOT__cpu_inst__DOT__state 
                        = ((0x0dU == (0x1fU & (IData)(vlSelfRef.top__DOT__cpu_inst__DOT__IR)))
                            ? 0U : ((0x0eU == (0x1fU 
                                               & (IData)(vlSelfRef.top__DOT__cpu_inst__DOT__IR)))
                                     ? 0U : ((0x10U 
                                              == (0x1fU 
                                                  & (IData)(vlSelfRef.top__DOT__cpu_inst__DOT__IR)))
                                              ? 5U : 
                                             ((0x11U 
                                               == (0x1fU 
                                                   & (IData)(vlSelfRef.top__DOT__cpu_inst__DOT__IR)))
                                               ? 0x12U
                                               : ((0x14U 
                                                   == 
                                                   (0x1cU 
                                                    & (IData)(vlSelfRef.top__DOT__cpu_inst__DOT__IR)))
                                                   ? 0x30U
                                                   : 
                                                  ((0x19U 
                                                    == 
                                                    (0x1fU 
                                                     & (IData)(vlSelfRef.top__DOT__cpu_inst__DOT__IR)))
                                                    ? 2U
                                                    : 
                                                   ((0x1cU 
                                                     == 
                                                     (0x1cU 
                                                      & (IData)(vlSelfRef.top__DOT__cpu_inst__DOT__IR)))
                                                     ? 2U
                                                     : 0x24U)))))));
                }
            } else {
                vlSelfRef.__Vdly__top__DOT__cpu_inst__DOT__state 
                    = ((0x2fU == (IData)(vlSelfRef.top__DOT__cpu_inst__DOT__state))
                        ? ((IData)(vlSelfRef.top__DOT__cpu_inst__DOT__write_back)
                            ? 0x23U : 0x0dU) : ((0x30U 
                                                 == (IData)(vlSelfRef.top__DOT__cpu_inst__DOT__state))
                                                 ? 0x31U
                                                 : 
                                                ((0x31U 
                                                  == (IData)(vlSelfRef.top__DOT__cpu_inst__DOT__state))
                                                  ? 
                                                 ((IData)(vlSelfRef.top__DOT__cpu_inst__DOT__write_back)
                                                   ? 0x23U
                                                   : 0x0dU)
                                                  : 
                                                 ((0U 
                                                   == (IData)(vlSelfRef.top__DOT__cpu_inst__DOT__state))
                                                   ? 1U
                                                   : 
                                                  ((1U 
                                                    == (IData)(vlSelfRef.top__DOT__cpu_inst__DOT__state))
                                                    ? 
                                                   ((IData)(vlSelfRef.top__DOT__cpu_inst__DOT__write_back)
                                                     ? 0x23U
                                                     : 0x0dU)
                                                    : 
                                                   ((2U 
                                                     == (IData)(vlSelfRef.top__DOT__cpu_inst__DOT__state))
                                                     ? 3U
                                                     : 
                                                    ((((IData)(vlSelfRef.top__DOT__cpu_inst__DOT__CO) 
                                                       | (IData)(vlSelfRef.top__DOT__cpu_inst__DOT__store)) 
                                                      | (IData)(vlSelfRef.top__DOT__cpu_inst__DOT__write_back))
                                                      ? 4U
                                                      : 0x0dU)))))));
            }
        } else if (((((((((4U == (IData)(vlSelfRef.top__DOT__cpu_inst__DOT__state)) 
                          | (0x0eU == (IData)(vlSelfRef.top__DOT__cpu_inst__DOT__state))) 
                         | (0x0fU == (IData)(vlSelfRef.top__DOT__cpu_inst__DOT__state))) 
                        | (0x10U == (IData)(vlSelfRef.top__DOT__cpu_inst__DOT__state))) 
                       | (0x11U == (IData)(vlSelfRef.top__DOT__cpu_inst__DOT__state))) 
                      | (0x12U == (IData)(vlSelfRef.top__DOT__cpu_inst__DOT__state))) 
                     | (0x13U == (IData)(vlSelfRef.top__DOT__cpu_inst__DOT__state))) 
                    | (0x14U == (IData)(vlSelfRef.top__DOT__cpu_inst__DOT__state)))) {
            vlSelfRef.__Vdly__top__DOT__cpu_inst__DOT__state 
                = ((4U == (IData)(vlSelfRef.top__DOT__cpu_inst__DOT__state))
                    ? ((IData)(vlSelfRef.top__DOT__cpu_inst__DOT__write_back)
                        ? 0x23U : 0x0dU) : ((0x0eU 
                                             == (IData)(vlSelfRef.top__DOT__cpu_inst__DOT__state))
                                             ? 0x0fU
                                             : ((0x0fU 
                                                 == (IData)(vlSelfRef.top__DOT__cpu_inst__DOT__state))
                                                 ? 0x10U
                                                 : 
                                                ((0x10U 
                                                  == (IData)(vlSelfRef.top__DOT__cpu_inst__DOT__state))
                                                  ? 0x11U
                                                  : 
                                                 ((0x11U 
                                                   == (IData)(vlSelfRef.top__DOT__cpu_inst__DOT__state))
                                                   ? 0x0dU
                                                   : 
                                                  ((0x12U 
                                                    == (IData)(vlSelfRef.top__DOT__cpu_inst__DOT__state))
                                                    ? 0x13U
                                                    : 
                                                   ((0x13U 
                                                     == (IData)(vlSelfRef.top__DOT__cpu_inst__DOT__state))
                                                     ? 0x14U
                                                     : 
                                                    (((IData)(vlSelfRef.top__DOT__cpu_inst__DOT__CO) 
                                                      | (IData)(vlSelfRef.top__DOT__cpu_inst__DOT__store))
                                                      ? 0x15U
                                                      : 0x0dU))))))));
        } else if (((((((((0x15U == (IData)(vlSelfRef.top__DOT__cpu_inst__DOT__state)) 
                          | (0x23U == (IData)(vlSelfRef.top__DOT__cpu_inst__DOT__state))) 
                         | (0x2eU == (IData)(vlSelfRef.top__DOT__cpu_inst__DOT__state))) 
                        | (0x0dU == (IData)(vlSelfRef.top__DOT__cpu_inst__DOT__state))) 
                       | (0x24U == (IData)(vlSelfRef.top__DOT__cpu_inst__DOT__state))) 
                      | (0x21U == (IData)(vlSelfRef.top__DOT__cpu_inst__DOT__state))) 
                     | (0x22U == (IData)(vlSelfRef.top__DOT__cpu_inst__DOT__state))) 
                    | (0x1eU == (IData)(vlSelfRef.top__DOT__cpu_inst__DOT__state)))) {
            vlSelfRef.__Vdly__top__DOT__cpu_inst__DOT__state 
                = ((0x15U == (IData)(vlSelfRef.top__DOT__cpu_inst__DOT__state))
                    ? 0x0dU : ((0x23U == (IData)(vlSelfRef.top__DOT__cpu_inst__DOT__state))
                                ? 0x2eU : ((0x2eU == (IData)(vlSelfRef.top__DOT__cpu_inst__DOT__state))
                                            ? 0x0dU
                                            : ((0x0dU 
                                                == (IData)(vlSelfRef.top__DOT__cpu_inst__DOT__state))
                                                ? 0x0cU
                                                : (
                                                   (0x24U 
                                                    == (IData)(vlSelfRef.top__DOT__cpu_inst__DOT__state))
                                                    ? 0x0cU
                                                    : 
                                                   ((0x21U 
                                                     == (IData)(vlSelfRef.top__DOT__cpu_inst__DOT__state))
                                                     ? 0x22U
                                                     : 
                                                    ((0x22U 
                                                      == (IData)(vlSelfRef.top__DOT__cpu_inst__DOT__state))
                                                      ? 0x0cU
                                                      : 0x1fU)))))));
        } else if (((((((((0x1fU == (IData)(vlSelfRef.top__DOT__cpu_inst__DOT__state)) 
                          | (0x20U == (IData)(vlSelfRef.top__DOT__cpu_inst__DOT__state))) 
                         | (0x1aU == (IData)(vlSelfRef.top__DOT__cpu_inst__DOT__state))) 
                        | (0x1bU == (IData)(vlSelfRef.top__DOT__cpu_inst__DOT__state))) 
                       | (0x1cU == (IData)(vlSelfRef.top__DOT__cpu_inst__DOT__state))) 
                      | (0x1dU == (IData)(vlSelfRef.top__DOT__cpu_inst__DOT__state))) 
                     | (0x25U == (IData)(vlSelfRef.top__DOT__cpu_inst__DOT__state))) 
                    | (0x26U == (IData)(vlSelfRef.top__DOT__cpu_inst__DOT__state)))) {
            vlSelfRef.__Vdly__top__DOT__cpu_inst__DOT__state 
                = ((0x1fU == (IData)(vlSelfRef.top__DOT__cpu_inst__DOT__state))
                    ? 0x20U : ((0x20U == (IData)(vlSelfRef.top__DOT__cpu_inst__DOT__state))
                                ? 0x0cU : ((0x1aU == (IData)(vlSelfRef.top__DOT__cpu_inst__DOT__state))
                                            ? 0x1bU
                                            : ((0x1bU 
                                                == (IData)(vlSelfRef.top__DOT__cpu_inst__DOT__state))
                                                ? 0x1cU
                                                : (
                                                   (0x1cU 
                                                    == (IData)(vlSelfRef.top__DOT__cpu_inst__DOT__state))
                                                    ? 0x1dU
                                                    : 
                                                   ((0x1dU 
                                                     == (IData)(vlSelfRef.top__DOT__cpu_inst__DOT__state))
                                                     ? 0x0dU
                                                     : 
                                                    ((0x25U 
                                                      == (IData)(vlSelfRef.top__DOT__cpu_inst__DOT__state))
                                                      ? 0x26U
                                                      : 0x27U)))))));
        } else if (((((((((0x27U == (IData)(vlSelfRef.top__DOT__cpu_inst__DOT__state)) 
                          | (0x28U == (IData)(vlSelfRef.top__DOT__cpu_inst__DOT__state))) 
                         | (0x29U == (IData)(vlSelfRef.top__DOT__cpu_inst__DOT__state))) 
                        | (0x2aU == (IData)(vlSelfRef.top__DOT__cpu_inst__DOT__state))) 
                       | (0x2bU == (IData)(vlSelfRef.top__DOT__cpu_inst__DOT__state))) 
                      | (0x2cU == (IData)(vlSelfRef.top__DOT__cpu_inst__DOT__state))) 
                     | (0x2dU == (IData)(vlSelfRef.top__DOT__cpu_inst__DOT__state))) 
                    | (5U == (IData)(vlSelfRef.top__DOT__cpu_inst__DOT__state)))) {
            vlSelfRef.__Vdly__top__DOT__cpu_inst__DOT__state 
                = ((0x27U == (IData)(vlSelfRef.top__DOT__cpu_inst__DOT__state))
                    ? 0x28U : ((0x28U == (IData)(vlSelfRef.top__DOT__cpu_inst__DOT__state))
                                ? 0x29U : ((0x29U == (IData)(vlSelfRef.top__DOT__cpu_inst__DOT__state))
                                            ? 0x0cU
                                            : ((0x2aU 
                                                == (IData)(vlSelfRef.top__DOT__cpu_inst__DOT__state))
                                                ? 0x2bU
                                                : (
                                                   (0x2bU 
                                                    == (IData)(vlSelfRef.top__DOT__cpu_inst__DOT__state))
                                                    ? 0x2cU
                                                    : 
                                                   ((0x2cU 
                                                     == (IData)(vlSelfRef.top__DOT__cpu_inst__DOT__state))
                                                     ? 0x2dU
                                                     : 
                                                    ((0x2dU 
                                                      == (IData)(vlSelfRef.top__DOT__cpu_inst__DOT__state))
                                                      ? 0x0dU
                                                      : 
                                                     ((IData)(vlSelfRef.top__DOT__cpu_inst__DOT__cond_true)
                                                       ? 6U
                                                       : 0x0cU))))))));
        } else if (((((((((6U == (IData)(vlSelfRef.top__DOT__cpu_inst__DOT__state)) 
                          | (7U == (IData)(vlSelfRef.top__DOT__cpu_inst__DOT__state))) 
                         | (0x16U == (IData)(vlSelfRef.top__DOT__cpu_inst__DOT__state))) 
                        | (0x17U == (IData)(vlSelfRef.top__DOT__cpu_inst__DOT__state))) 
                       | (0x18U == (IData)(vlSelfRef.top__DOT__cpu_inst__DOT__state))) 
                      | (0x19U == (IData)(vlSelfRef.top__DOT__cpu_inst__DOT__state))) 
                     | (8U == (IData)(vlSelfRef.top__DOT__cpu_inst__DOT__state))) 
                    | (9U == (IData)(vlSelfRef.top__DOT__cpu_inst__DOT__state)))) {
            vlSelfRef.__Vdly__top__DOT__cpu_inst__DOT__state 
                = ((6U == (IData)(vlSelfRef.top__DOT__cpu_inst__DOT__state))
                    ? (((IData)(vlSelfRef.top__DOT__cpu_inst__DOT__CO) 
                        ^ (IData)(vlSelfRef.top__DOT__cpu_inst__DOT__backwards))
                        ? 7U : 0x0cU) : ((7U == (IData)(vlSelfRef.top__DOT__cpu_inst__DOT__state))
                                          ? 0x0cU : 
                                         ((0x16U == (IData)(vlSelfRef.top__DOT__cpu_inst__DOT__state))
                                           ? 0x17U : 
                                          ((0x17U == (IData)(vlSelfRef.top__DOT__cpu_inst__DOT__state))
                                            ? 0x0cU
                                            : ((0x18U 
                                                == (IData)(vlSelfRef.top__DOT__cpu_inst__DOT__state))
                                                ? 0x19U
                                                : (
                                                   (0x19U 
                                                    == (IData)(vlSelfRef.top__DOT__cpu_inst__DOT__state))
                                                    ? 0x16U
                                                    : 
                                                   ((8U 
                                                     == (IData)(vlSelfRef.top__DOT__cpu_inst__DOT__state))
                                                     ? 9U
                                                     : 0x0aU)))))));
        } else if ((0x0aU == (IData)(vlSelfRef.top__DOT__cpu_inst__DOT__state))) {
            vlSelfRef.__Vdly__top__DOT__cpu_inst__DOT__state = 0x0bU;
        } else if ((0x0bU == (IData)(vlSelfRef.top__DOT__cpu_inst__DOT__state))) {
            vlSelfRef.__Vdly__top__DOT__cpu_inst__DOT__state = 0x16U;
        }
    }
}

extern const VlUnpacked<CData/*0:0*/, 128> Vtop__ConstPool__TABLE_hdc38b0ff_0;

void Vtop___024root___nba_sequent__TOP__2(Vtop___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vtop___024root___nba_sequent__TOP__2\n"); );
    Vtop__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
    // Locals
    CData/*6:0*/ __Vtableidx7;
    __Vtableidx7 = 0;
    // Body
    __Vtableidx7 = ((((IData)(vlSelfRef.top__DOT__cpu_inst__DOT__N) 
                      << 6U) | ((IData)(vlSelfRef.top__DOT__cpu_inst__DOT__V) 
                                << 5U)) | (((IData)(vlSelfRef.top__DOT__cpu_inst__DOT__C) 
                                            << 4U) 
                                           | (((IData)(vlSelfRef.top__DOT__cpu_inst__DOT__Z) 
                                               << 3U) 
                                              | (IData)(vlSelfRef.top__DOT__cpu_inst__DOT__cond_code))));
    vlSelfRef.top__DOT__cpu_inst__DOT__cond_true = Vtop__ConstPool__TABLE_hdc38b0ff_0
        [__Vtableidx7];
    if (((0x0cU == (IData)(vlSelfRef.top__DOT__cpu_inst__DOT__state)) 
         & (IData)(vlSelfRef.top__DOT____Vcellinp__cpu_inst__RDY))) {
        vlSelfRef.top__DOT__cpu_inst__DOT__store = 
            ((0x84U == (0xe5U & (IData)(vlSelfRef.top__DOT__cpu_inst__DOT__IR))) 
             || (0x81U == (0xe3U & (IData)(vlSelfRef.top__DOT__cpu_inst__DOT__IR))));
        vlSelfRef.top__DOT__cpu_inst__DOT__write_back 
            = ((6U == (0x87U & (IData)(vlSelfRef.top__DOT__cpu_inst__DOT__IR))) 
               || (0xc6U == (0xc7U & (IData)(vlSelfRef.top__DOT__cpu_inst__DOT__IR))));
    }
    if (vlSelfRef.top__DOT____Vcellinp__cpu_inst__RDY) {
        vlSelfRef.top__DOT__cpu_inst__DOT__backwards 
            = (1U & ((IData)(vlSelfRef.top__DOT__cpu_inst__DOT__DIMUX) 
                     >> 7U));
        vlSelfRef.top__DOT__cpu_inst__DOT__CO = (1U 
                                                 & (((IData)(vlSelfRef.top__DOT__cpu_inst__DOT__ALU__DOT__temp) 
                                                     >> 8U) 
                                                    | ((IData)(vlSelfRef.top__DOT__cpu_inst__DOT____Vcellinp__ALU__BCD) 
                                                       & (5U 
                                                          <= 
                                                          (7U 
                                                           & ((IData)(vlSelfRef.top__DOT__cpu_inst__DOT__ALU__DOT__temp_h) 
                                                              >> 1U))))));
    }
    vlSelfRef.top__DOT____Vcellinp__cpu_inst__RDY = 
        ((~ (IData)(vlSelfRef.dbg_pause)) & ((0U == (IData)(vlSelfRef.top__DOT__blt_inst__DOT__state)) 
                                             | (6U 
                                                == (IData)(vlSelfRef.top__DOT__blt_inst__DOT__state))));
    vlSelfRef.top__DOT__cpu_inst__DOT__DIMUX = ((IData)(vlSelfRef.top__DOT____Vcellinp__cpu_inst__RDY)
                                                 ? (IData)(vlSelfRef.top__DOT__cpu_din)
                                                 : (IData)(vlSelfRef.top__DOT__cpu_inst__DOT__DIHOLD));
    vlSelfRef.top__DOT__cpu_inst__DOT__IR = ((IData)(vlSelfRef.__VdfgRegularize_hebeb780c_0_3)
                                              ? 0U : 
                                             ((IData)(vlSelfRef.top__DOT__cpu_inst__DOT__IRHOLD_valid)
                                               ? (IData)(vlSelfRef.top__DOT__cpu_inst__DOT__IRHOLD)
                                               : (IData)(vlSelfRef.top__DOT__cpu_inst__DOT__DIMUX)));
    vlSelfRef.__VdfgRegularize_hebeb780c_0_0 = (((IData)(vlSelfRef.top__DOT__cpu_inst__DOT__DIMUX) 
                                                 << 8U) 
                                                | (IData)(vlSelfRef.top__DOT__cpu_inst__DOT__ADD));
}

void Vtop___024root___nba_sequent__TOP__3(Vtop___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vtop___024root___nba_sequent__TOP__3\n"); );
    Vtop__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
    // Body
    vlSelfRef.top__DOT__cpu_inst__DOT__state = vlSelfRef.__Vdly__top__DOT__cpu_inst__DOT__state;
}

extern const VlUnpacked<CData/*0:0*/, 128> Vtop__ConstPool__TABLE_h30aa0085_0;
extern const VlUnpacked<CData/*0:0*/, 256> Vtop__ConstPool__TABLE_h67de06b7_0;
extern const VlUnpacked<CData/*0:0*/, 2048> Vtop__ConstPool__TABLE_h32401d32_0;
extern const VlUnpacked<CData/*0:0*/, 8192> Vtop__ConstPool__TABLE_hc7a084b0_0;
extern const VlUnpacked<CData/*3:0*/, 2048> Vtop__ConstPool__TABLE_hf67012e5_0;
extern const VlUnpacked<CData/*1:0*/, 2048> Vtop__ConstPool__TABLE_hd7a914d5_0;

void Vtop___024root___nba_comb__TOP__0(Vtop___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vtop___024root___nba_comb__TOP__0\n"); );
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
    CData/*6:0*/ __Vtableidx2;
    __Vtableidx2 = 0;
    CData/*7:0*/ __Vtableidx3;
    __Vtableidx3 = 0;
    SData/*10:0*/ __Vtableidx4;
    __Vtableidx4 = 0;
    SData/*10:0*/ __Vtableidx5;
    __Vtableidx5 = 0;
    SData/*12:0*/ __Vtableidx6;
    __Vtableidx6 = 0;
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
    __Vtableidx2 = (((IData)(vlSelfRef.top__DOT__cpu_inst__DOT__store) 
                     << 6U) | (IData)(vlSelfRef.top__DOT__cpu_inst__DOT__state));
    vlSelfRef.top__DOT__cpu_we = Vtop__ConstPool__TABLE_h30aa0085_0
        [__Vtableidx2];
    __Vtableidx3 = (((IData)(vlSelfRef.top__DOT__cpu_inst__DOT__plp) 
                     << 7U) | (((IData)(vlSelfRef.top__DOT__cpu_inst__DOT__load_reg) 
                                << 6U) | (IData)(vlSelfRef.top__DOT__cpu_inst__DOT__state)));
    vlSelfRef.top__DOT__cpu_inst__DOT__write_register 
        = Vtop__ConstPool__TABLE_h67de06b7_0[__Vtableidx3];
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
    vlSelfRef.top__DOT__cpu_inst__DOT____Vcellinp__ALU__BCD 
        = ((IData)(vlSelfRef.top__DOT__cpu_inst__DOT__adc_bcd) 
           & (0x0dU == (IData)(vlSelfRef.top__DOT__cpu_inst__DOT__state)));
    vlSelfRef.top__DOT__cpu_inst__DOT__alu_shift_right 
        = (((0x0dU == (IData)(vlSelfRef.top__DOT__cpu_inst__DOT__state)) 
            | ((0x24U == (IData)(vlSelfRef.top__DOT__cpu_inst__DOT__state)) 
               | (0x23U == (IData)(vlSelfRef.top__DOT__cpu_inst__DOT__state)))) 
           & (IData)(vlSelfRef.top__DOT__cpu_inst__DOT__shift_right));
    __Vtableidx6 = ((((((IData)(vlSelfRef.top__DOT__cpu_inst__DOT__load_only) 
                        << 3U) | ((IData)(vlSelfRef.top__DOT__cpu_inst__DOT__compare) 
                                  << 2U)) | (((IData)(vlSelfRef.top__DOT__cpu_inst__DOT__CO) 
                                              << 1U) 
                                             | (IData)(vlSelfRef.top__DOT__cpu_inst__DOT__inc))) 
                     << 9U) | (((IData)(vlSelfRef.top__DOT__cpu_inst__DOT__shift) 
                                << 8U) | (((IData)(vlSelfRef.top__DOT__cpu_inst__DOT__C) 
                                           << 7U) | 
                                          (((IData)(vlSelfRef.top__DOT__cpu_inst__DOT__rotate) 
                                            << 6U) 
                                           | (IData)(vlSelfRef.top__DOT__cpu_inst__DOT__state)))));
    vlSelfRef.top__DOT__cpu_inst__DOT__CI = Vtop__ConstPool__TABLE_hc7a084b0_0
        [__Vtableidx6];
    __Vtableidx5 = (((IData)(vlSelfRef.top__DOT__cpu_inst__DOT__backwards) 
                     << 0x0000000aU) | (((IData)(vlSelfRef.top__DOT__cpu_inst__DOT__op) 
                                         << 6U) | (IData)(vlSelfRef.top__DOT__cpu_inst__DOT__state)));
    vlSelfRef.top__DOT__cpu_inst__DOT__alu_op = Vtop__ConstPool__TABLE_hf67012e5_0
        [__Vtableidx5];
    __Vtableidx4 = (((IData)(vlSelfRef.top__DOT__cpu_inst__DOT__dst_reg) 
                     << 9U) | (((IData)(vlSelfRef.top__DOT__cpu_inst__DOT__index_y) 
                                << 8U) | (((IData)(vlSelfRef.top__DOT__cpu_inst__DOT__src_reg) 
                                           << 6U) | (IData)(vlSelfRef.top__DOT__cpu_inst__DOT__state))));
    vlSelfRef.top__DOT__cpu_inst__DOT__regsel = Vtop__ConstPool__TABLE_hd7a914d5_0
        [__Vtableidx4];
    if ((1U & (IData)(vlSelfRef.top__DOT__cpu_inst__DOT__state))) {
        vlSelfRef.__VdfgRegularize_hebeb780c_0_11 = 
            (0x000000ffU & (IData)(vlSelfRef.top__DOT__cpu_inst__DOT__PC));
        vlSelfRef.__VdfgRegularize_hebeb780c_0_6 = vlSelfRef.top__DOT__cpu_inst__DOT__ADD;
        vlSelfRef.__VdfgRegularize_hebeb780c_0_8 = vlSelfRef.top__DOT__cpu_inst__DOT__PC;
        __VdfgRegularize_hebeb780c_0_18 = 0U;
        __VdfgRegularize_hebeb780c_0_16 = vlSelfRef.top__DOT__cpu_inst__DOT__DIMUX;
        __VdfgRegularize_hebeb780c_0_1 = vlSelfRef.__VdfgRegularize_hebeb780c_0_0;
    } else {
        vlSelfRef.__VdfgRegularize_hebeb780c_0_11 = 
            (0x000000ffU & ((IData)(vlSelfRef.top__DOT__cpu_inst__DOT__PC) 
                            >> 8U));
        vlSelfRef.__VdfgRegularize_hebeb780c_0_6 = vlSelfRef.top__DOT__cpu_inst__DOT__PC;
        vlSelfRef.__VdfgRegularize_hebeb780c_0_8 = 
            (0x00000100U | (IData)(vlSelfRef.top__DOT__cpu_inst__DOT__ADD));
        __VdfgRegularize_hebeb780c_0_18 = vlSelfRef.top__DOT__cpu_inst__DOT__DIMUX;
        __VdfgRegularize_hebeb780c_0_16 = 0U;
        __VdfgRegularize_hebeb780c_0_1 = vlSelfRef.top__DOT__cpu_inst__DOT__PC;
    }
    vlSelfRef.top__DOT__cpu_inst__DOT__regfile = vlSelfRef.top__DOT__cpu_inst__DOT__AXYS
        [vlSelfRef.top__DOT__cpu_inst__DOT__regsel];
    if ((1U & (IData)(vlSelfRef.top__DOT__cpu_inst__DOT__state))) {
        vlSelfRef.__VdfgRegularize_hebeb780c_0_9 = 
            (0x00000100U | (IData)(vlSelfRef.top__DOT__cpu_inst__DOT__ADD));
        vlSelfRef.__VdfgRegularize_hebeb780c_0_12 = 0U;
        vlSelfRef.__VdfgRegularize_hebeb780c_0_15 = vlSelfRef.top__DOT__cpu_inst__DOT__regfile;
        vlSelfRef.__VdfgRegularize_hebeb780c_0_13 = vlSelfRef.top__DOT__cpu_inst__DOT__ADD;
    } else {
        vlSelfRef.__VdfgRegularize_hebeb780c_0_9 = 
            (0x00000100U | (IData)(vlSelfRef.top__DOT__cpu_inst__DOT__regfile));
        vlSelfRef.__VdfgRegularize_hebeb780c_0_12 = vlSelfRef.top__DOT__cpu_inst__DOT__regfile;
        vlSelfRef.__VdfgRegularize_hebeb780c_0_15 = 0U;
        vlSelfRef.__VdfgRegularize_hebeb780c_0_13 = vlSelfRef.top__DOT__cpu_inst__DOT__regfile;
    }
    if ((2U & (IData)(vlSelfRef.top__DOT__cpu_inst__DOT__state))) {
        __VdfgRegularize_hebeb780c_0_17 = 0U;
        __VdfgRegularize_hebeb780c_0_2 = vlSelfRef.top__DOT__cpu_inst__DOT__PC;
        vlSelfRef.__VdfgRegularize_hebeb780c_0_14 = vlSelfRef.__VdfgRegularize_hebeb780c_0_13;
    } else {
        __VdfgRegularize_hebeb780c_0_17 = vlSelfRef.top__DOT__cpu_inst__DOT__DIMUX;
        __VdfgRegularize_hebeb780c_0_2 = __VdfgRegularize_hebeb780c_0_1;
        vlSelfRef.__VdfgRegularize_hebeb780c_0_14 = 0U;
    }
    if ((0x00000020U & (IData)(vlSelfRef.top__DOT__cpu_inst__DOT__state))) {
        if ((0x00000010U & (IData)(vlSelfRef.top__DOT__cpu_inst__DOT__state))) {
            top__DOT__cpu_inst__DOT__BI = (0x000000ffU 
                                           & (IData)(vlSelfRef.top__DOT__cpu_inst__DOT__DIMUX));
            vlSelfRef.top__DOT__cpu_dout = vlSelfRef.top__DOT__cpu_inst__DOT__regfile;
            vlSelfRef.top__DOT__cpu_inst__DOT__PC_temp 
                = vlSelfRef.top__DOT__cpu_inst__DOT__PC;
            if ((8U & (IData)(vlSelfRef.top__DOT__cpu_inst__DOT__state))) {
                vlSelfRef.top__DOT__cpu_addr = vlSelfRef.top__DOT__cpu_inst__DOT__PC;
                vlSelfRef.top__DOT__cpu_inst__DOT__AI = 0U;
            } else if ((4U & (IData)(vlSelfRef.top__DOT__cpu_inst__DOT__state))) {
                vlSelfRef.top__DOT__cpu_addr = vlSelfRef.top__DOT__cpu_inst__DOT__PC;
                vlSelfRef.top__DOT__cpu_inst__DOT__AI = 0U;
            } else if ((2U & (IData)(vlSelfRef.top__DOT__cpu_inst__DOT__state))) {
                vlSelfRef.top__DOT__cpu_addr = vlSelfRef.top__DOT__cpu_inst__DOT__PC;
                vlSelfRef.top__DOT__cpu_inst__DOT__AI = 0U;
            } else {
                vlSelfRef.top__DOT__cpu_addr = vlSelfRef.__VdfgRegularize_hebeb780c_0_6;
                vlSelfRef.top__DOT__cpu_inst__DOT__AI 
                    = vlSelfRef.__VdfgRegularize_hebeb780c_0_12;
            }
        } else if ((8U & (IData)(vlSelfRef.top__DOT__cpu_inst__DOT__state))) {
            if ((4U & (IData)(vlSelfRef.top__DOT__cpu_inst__DOT__state))) {
                top__DOT__cpu_inst__DOT__BI = (0x000000ffU 
                                               & (IData)(vlSelfRef.top__DOT__cpu_inst__DOT__DIMUX));
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
            } else {
                top__DOT__cpu_inst__DOT__BI = (0x000000ffU 
                                               & (IData)(__VdfgRegularize_hebeb780c_0_17));
                vlSelfRef.top__DOT__cpu_dout = vlSelfRef.top__DOT__cpu_inst__DOT__regfile;
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
                vlSelfRef.top__DOT__cpu_inst__DOT__AI 
                    = vlSelfRef.__VdfgRegularize_hebeb780c_0_14;
            }
            vlSelfRef.top__DOT__cpu_inst__DOT__PC_temp 
                = __VdfgRegularize_hebeb780c_0_2;
        } else {
            if ((4U & (IData)(vlSelfRef.top__DOT__cpu_inst__DOT__state))) {
                top__DOT__cpu_inst__DOT__BI = (0x000000ffU 
                                               & 0U);
                vlSelfRef.top__DOT__cpu_dout = vlSelfRef.top__DOT__cpu_inst__DOT__regfile;
                if ((2U & (IData)(vlSelfRef.top__DOT__cpu_inst__DOT__state))) {
                    vlSelfRef.top__DOT__cpu_addr = 
                        (0x00000100U | (IData)(vlSelfRef.top__DOT__cpu_inst__DOT__ADD));
                    vlSelfRef.top__DOT__cpu_inst__DOT__AI 
                        = vlSelfRef.top__DOT__cpu_inst__DOT__ADD;
                } else {
                    vlSelfRef.top__DOT__cpu_addr = 
                        ((1U & (IData)(vlSelfRef.top__DOT__cpu_inst__DOT__state))
                          ? (0x00000100U | (IData)(vlSelfRef.top__DOT__cpu_inst__DOT__regfile))
                          : (IData)(vlSelfRef.__VdfgRegularize_hebeb780c_0_4));
                    vlSelfRef.top__DOT__cpu_inst__DOT__AI 
                        = vlSelfRef.top__DOT__cpu_inst__DOT__regfile;
                }
            } else if ((2U & (IData)(vlSelfRef.top__DOT__cpu_inst__DOT__state))) {
                top__DOT__cpu_inst__DOT__BI = (0x000000ffU 
                                               & 0U);
                if ((1U & (IData)(vlSelfRef.top__DOT__cpu_inst__DOT__state))) {
                    vlSelfRef.top__DOT__cpu_dout = vlSelfRef.top__DOT__cpu_inst__DOT__regfile;
                    vlSelfRef.top__DOT__cpu_addr = vlSelfRef.__VdfgRegularize_hebeb780c_0_4;
                    vlSelfRef.top__DOT__cpu_inst__DOT__AI 
                        = vlSelfRef.top__DOT__cpu_inst__DOT__DIMUX;
                } else {
                    vlSelfRef.top__DOT__cpu_dout = 
                        ((IData)(vlSelfRef.top__DOT__cpu_inst__DOT__php)
                          ? (IData)(vlSelfRef.top__DOT__cpu_inst__DOT__P)
                          : (IData)(vlSelfRef.top__DOT__cpu_inst__DOT__ADD));
                    vlSelfRef.top__DOT__cpu_addr = 
                        (0x00000100U | (IData)(vlSelfRef.top__DOT__cpu_inst__DOT__regfile));
                    vlSelfRef.top__DOT__cpu_inst__DOT__AI 
                        = vlSelfRef.top__DOT__cpu_inst__DOT__regfile;
                }
            } else {
                top__DOT__cpu_inst__DOT__BI = (0x000000ffU 
                                               & (IData)(__VdfgRegularize_hebeb780c_0_18));
                vlSelfRef.top__DOT__cpu_dout = vlSelfRef.top__DOT__cpu_inst__DOT__regfile;
                vlSelfRef.top__DOT__cpu_addr = vlSelfRef.top__DOT__cpu_inst__DOT__PC;
                vlSelfRef.top__DOT__cpu_inst__DOT__AI 
                    = vlSelfRef.__VdfgRegularize_hebeb780c_0_15;
            }
            vlSelfRef.top__DOT__cpu_inst__DOT__PC_temp 
                = vlSelfRef.top__DOT__cpu_inst__DOT__PC;
        }
    } else if ((0x00000010U & (IData)(vlSelfRef.top__DOT__cpu_inst__DOT__state))) {
        if ((8U & (IData)(vlSelfRef.top__DOT__cpu_inst__DOT__state))) {
            if ((4U & (IData)(vlSelfRef.top__DOT__cpu_inst__DOT__state))) {
                top__DOT__cpu_inst__DOT__BI = (0x000000ffU 
                                               & (IData)(__VdfgRegularize_hebeb780c_0_16));
                vlSelfRef.top__DOT__cpu_dout = vlSelfRef.top__DOT__cpu_inst__DOT__regfile;
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
                vlSelfRef.top__DOT__cpu_inst__DOT__AI 
                    = vlSelfRef.__VdfgRegularize_hebeb780c_0_12;
            } else {
                top__DOT__cpu_inst__DOT__BI = (0x000000ffU 
                                               & (IData)(__VdfgRegularize_hebeb780c_0_17));
                if ((2U & (IData)(vlSelfRef.top__DOT__cpu_inst__DOT__state))) {
                    vlSelfRef.top__DOT__cpu_dout = vlSelfRef.__VdfgRegularize_hebeb780c_0_11;
                    vlSelfRef.top__DOT__cpu_addr = vlSelfRef.__VdfgRegularize_hebeb780c_0_9;
                } else {
                    vlSelfRef.top__DOT__cpu_dout = vlSelfRef.top__DOT__cpu_inst__DOT__regfile;
                    vlSelfRef.top__DOT__cpu_addr = __VdfgRegularize_hebeb780c_0_1;
                }
                vlSelfRef.top__DOT__cpu_inst__DOT__AI 
                    = vlSelfRef.__VdfgRegularize_hebeb780c_0_14;
            }
            vlSelfRef.top__DOT__cpu_inst__DOT__PC_temp 
                = __VdfgRegularize_hebeb780c_0_2;
        } else {
            top__DOT__cpu_inst__DOT__BI = (0x000000ffU 
                                           & (IData)(vlSelfRef.top__DOT__cpu_inst__DOT__DIMUX));
            vlSelfRef.top__DOT__cpu_dout = vlSelfRef.top__DOT__cpu_inst__DOT__regfile;
            if ((4U & (IData)(vlSelfRef.top__DOT__cpu_inst__DOT__state))) {
                if ((2U & (IData)(vlSelfRef.top__DOT__cpu_inst__DOT__state))) {
                    vlSelfRef.top__DOT__cpu_inst__DOT__PC_temp 
                        = __VdfgRegularize_hebeb780c_0_1;
                    vlSelfRef.top__DOT__cpu_addr = __VdfgRegularize_hebeb780c_0_1;
                } else {
                    vlSelfRef.top__DOT__cpu_inst__DOT__PC_temp 
                        = vlSelfRef.top__DOT__cpu_inst__DOT__PC;
                    vlSelfRef.top__DOT__cpu_addr = 
                        ((1U & (IData)(vlSelfRef.top__DOT__cpu_inst__DOT__state))
                          ? (IData)(vlSelfRef.__VdfgRegularize_hebeb780c_0_10)
                          : (IData)(vlSelfRef.__VdfgRegularize_hebeb780c_0_0));
                }
                vlSelfRef.top__DOT__cpu_inst__DOT__AI = 0U;
            } else {
                vlSelfRef.top__DOT__cpu_inst__DOT__PC_temp 
                    = vlSelfRef.top__DOT__cpu_inst__DOT__PC;
                if ((2U & (IData)(vlSelfRef.top__DOT__cpu_inst__DOT__state))) {
                    vlSelfRef.top__DOT__cpu_addr = 
                        ((1U & (IData)(vlSelfRef.top__DOT__cpu_inst__DOT__state))
                          ? (IData)(vlSelfRef.top__DOT__cpu_inst__DOT__ADD)
                          : (IData)(vlSelfRef.top__DOT__cpu_inst__DOT__DIMUX));
                    vlSelfRef.top__DOT__cpu_inst__DOT__AI 
                        = vlSelfRef.__VdfgRegularize_hebeb780c_0_15;
                } else {
                    vlSelfRef.top__DOT__cpu_addr = 
                        ((1U & (IData)(vlSelfRef.top__DOT__cpu_inst__DOT__state))
                          ? (IData)(vlSelfRef.__VdfgRegularize_hebeb780c_0_0)
                          : (IData)(vlSelfRef.top__DOT__cpu_inst__DOT__ADD));
                    vlSelfRef.top__DOT__cpu_inst__DOT__AI = 0U;
                }
            }
        }
    } else if ((8U & (IData)(vlSelfRef.top__DOT__cpu_inst__DOT__state))) {
        if ((4U & (IData)(vlSelfRef.top__DOT__cpu_inst__DOT__state))) {
            if ((2U & (IData)(vlSelfRef.top__DOT__cpu_inst__DOT__state))) {
                top__DOT__cpu_inst__DOT__BI = (0x000000ffU 
                                               & (IData)(__VdfgRegularize_hebeb780c_0_18));
                vlSelfRef.top__DOT__cpu_inst__DOT__PC_temp 
                    = vlSelfRef.top__DOT__cpu_inst__DOT__PC;
                vlSelfRef.top__DOT__cpu_addr = vlSelfRef.__VdfgRegularize_hebeb780c_0_6;
                vlSelfRef.top__DOT__cpu_inst__DOT__AI 
                    = vlSelfRef.__VdfgRegularize_hebeb780c_0_13;
            } else {
                top__DOT__cpu_inst__DOT__BI = (0x000000ffU 
                                               & (IData)(__VdfgRegularize_hebeb780c_0_16));
                if ((1U & (IData)(vlSelfRef.top__DOT__cpu_inst__DOT__state))) {
                    vlSelfRef.top__DOT__cpu_inst__DOT__PC_temp 
                        = vlSelfRef.top__DOT__cpu_inst__DOT__PC;
                    vlSelfRef.top__DOT__cpu_inst__DOT__AI 
                        = ((IData)(vlSelfRef.top__DOT__cpu_inst__DOT__load_only)
                            ? 0U : (IData)(vlSelfRef.top__DOT__cpu_inst__DOT__regfile));
                } else {
                    vlSelfRef.top__DOT__cpu_inst__DOT__PC_temp 
                        = ((IData)(vlSelfRef.__VdfgRegularize_hebeb780c_0_3)
                            ? (IData)(vlSelfRef.__VdfgRegularize_hebeb780c_0_4)
                            : (IData)(vlSelfRef.top__DOT__cpu_inst__DOT__PC));
                    vlSelfRef.top__DOT__cpu_inst__DOT__AI = 0U;
                }
                vlSelfRef.top__DOT__cpu_addr = vlSelfRef.top__DOT__cpu_inst__DOT__PC;
            }
            vlSelfRef.top__DOT__cpu_dout = vlSelfRef.top__DOT__cpu_inst__DOT__regfile;
        } else if ((2U & (IData)(vlSelfRef.top__DOT__cpu_inst__DOT__state))) {
            top__DOT__cpu_inst__DOT__BI = (0x000000ffU 
                                           & (IData)(__VdfgRegularize_hebeb780c_0_16));
            if ((1U & (IData)(vlSelfRef.top__DOT__cpu_inst__DOT__state))) {
                vlSelfRef.top__DOT__cpu_dout = vlSelfRef.top__DOT__cpu_inst__DOT__regfile;
                vlSelfRef.top__DOT__cpu_inst__DOT__PC_temp 
                    = vlSelfRef.top__DOT__cpu_inst__DOT__PC;
                vlSelfRef.top__DOT__cpu_inst__DOT__AI = 0U;
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
                vlSelfRef.top__DOT__cpu_inst__DOT__PC_temp 
                    = ((IData)(vlSelfRef.top__DOT__cpu_inst__DOT__res)
                        ? 0xfffcU : ((IData)(vlSelfRef.top__DOT__cpu_inst__DOT__NMI_edge)
                                      ? 0xfffaU : 0xfffeU));
                vlSelfRef.top__DOT__cpu_inst__DOT__AI 
                    = vlSelfRef.top__DOT__cpu_inst__DOT__ADD;
            }
            vlSelfRef.top__DOT__cpu_addr = vlSelfRef.__VdfgRegularize_hebeb780c_0_8;
        } else {
            top__DOT__cpu_inst__DOT__BI = (0x000000ffU 
                                           & 0U);
            vlSelfRef.top__DOT__cpu_dout = vlSelfRef.__VdfgRegularize_hebeb780c_0_11;
            vlSelfRef.top__DOT__cpu_inst__DOT__PC_temp 
                = vlSelfRef.top__DOT__cpu_inst__DOT__PC;
            vlSelfRef.top__DOT__cpu_addr = vlSelfRef.__VdfgRegularize_hebeb780c_0_9;
            vlSelfRef.top__DOT__cpu_inst__DOT__AI = vlSelfRef.__VdfgRegularize_hebeb780c_0_13;
        }
    } else {
        if ((4U & (IData)(vlSelfRef.top__DOT__cpu_inst__DOT__state))) {
            if ((2U & (IData)(vlSelfRef.top__DOT__cpu_inst__DOT__state))) {
                top__DOT__cpu_inst__DOT__BI = (0x000000ffU 
                                               & (IData)(__VdfgRegularize_hebeb780c_0_16));
                if ((1U & (IData)(vlSelfRef.top__DOT__cpu_inst__DOT__state))) {
                    vlSelfRef.top__DOT__cpu_inst__DOT__PC_temp 
                        = (((IData)(vlSelfRef.top__DOT__cpu_inst__DOT__ADD) 
                            << 8U) | (0x000000ffU & (IData)(vlSelfRef.top__DOT__cpu_inst__DOT__PC)));
                    vlSelfRef.top__DOT__cpu_addr = vlSelfRef.__VdfgRegularize_hebeb780c_0_10;
                    vlSelfRef.top__DOT__cpu_inst__DOT__AI = 0U;
                } else {
                    vlSelfRef.top__DOT__cpu_inst__DOT__PC_temp 
                        = vlSelfRef.__VdfgRegularize_hebeb780c_0_5;
                    vlSelfRef.top__DOT__cpu_addr = vlSelfRef.__VdfgRegularize_hebeb780c_0_5;
                    vlSelfRef.top__DOT__cpu_inst__DOT__AI 
                        = vlSelfRef.top__DOT__cpu_inst__DOT__ABH;
                }
            } else {
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
                vlSelfRef.top__DOT__cpu_inst__DOT__AI 
                    = __VdfgRegularize_hebeb780c_0_16;
            }
        } else {
            if ((2U & (IData)(vlSelfRef.top__DOT__cpu_inst__DOT__state))) {
                top__DOT__cpu_inst__DOT__BI = (0x000000ffU 
                                               & (IData)(vlSelfRef.top__DOT__cpu_inst__DOT__DIMUX));
                vlSelfRef.top__DOT__cpu_inst__DOT__AI 
                    = vlSelfRef.__VdfgRegularize_hebeb780c_0_12;
            } else {
                top__DOT__cpu_inst__DOT__BI = (0x000000ffU 
                                               & (IData)(__VdfgRegularize_hebeb780c_0_18));
                vlSelfRef.top__DOT__cpu_inst__DOT__AI = 0U;
            }
            vlSelfRef.top__DOT__cpu_inst__DOT__PC_temp 
                = vlSelfRef.top__DOT__cpu_inst__DOT__PC;
            vlSelfRef.top__DOT__cpu_addr = __VdfgRegularize_hebeb780c_0_1;
        }
        vlSelfRef.top__DOT__cpu_dout = vlSelfRef.top__DOT__cpu_inst__DOT__regfile;
    }
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

void Vtop___024root___nba_sequent__TOP__0(Vtop___024root* vlSelf);

void Vtop___024root___eval_nba(Vtop___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vtop___024root___eval_nba\n"); );
    Vtop__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
    // Locals
    CData/*6:0*/ __Vinline__nba_sequent__TOP__2___Vtableidx7;
    __Vinline__nba_sequent__TOP__2___Vtableidx7 = 0;
    // Body
    if ((1ULL & vlSelfRef.__VnbaTriggered[0U])) {
        Vtop___024root___nba_sequent__TOP__0(vlSelf);
    }
    if ((3ULL & vlSelfRef.__VnbaTriggered[0U])) {
        Vtop___024root___nba_sequent__TOP__1(vlSelf);
    }
    if ((1ULL & vlSelfRef.__VnbaTriggered[0U])) {
        __Vinline__nba_sequent__TOP__2___Vtableidx7 
            = ((((IData)(vlSelfRef.top__DOT__cpu_inst__DOT__N) 
                 << 6U) | ((IData)(vlSelfRef.top__DOT__cpu_inst__DOT__V) 
                           << 5U)) | (((IData)(vlSelfRef.top__DOT__cpu_inst__DOT__C) 
                                       << 4U) | (((IData)(vlSelfRef.top__DOT__cpu_inst__DOT__Z) 
                                                  << 3U) 
                                                 | (IData)(vlSelfRef.top__DOT__cpu_inst__DOT__cond_code))));
        vlSelfRef.top__DOT__cpu_inst__DOT__cond_true 
            = Vtop__ConstPool__TABLE_hdc38b0ff_0[__Vinline__nba_sequent__TOP__2___Vtableidx7];
        if (((0x0cU == (IData)(vlSelfRef.top__DOT__cpu_inst__DOT__state)) 
             & (IData)(vlSelfRef.top__DOT____Vcellinp__cpu_inst__RDY))) {
            vlSelfRef.top__DOT__cpu_inst__DOT__store 
                = ((0x84U == (0xe5U & (IData)(vlSelfRef.top__DOT__cpu_inst__DOT__IR))) 
                   || (0x81U == (0xe3U & (IData)(vlSelfRef.top__DOT__cpu_inst__DOT__IR))));
            vlSelfRef.top__DOT__cpu_inst__DOT__write_back 
                = ((6U == (0x87U & (IData)(vlSelfRef.top__DOT__cpu_inst__DOT__IR))) 
                   || (0xc6U == (0xc7U & (IData)(vlSelfRef.top__DOT__cpu_inst__DOT__IR))));
        }
        if (vlSelfRef.top__DOT____Vcellinp__cpu_inst__RDY) {
            vlSelfRef.top__DOT__cpu_inst__DOT__backwards 
                = (1U & ((IData)(vlSelfRef.top__DOT__cpu_inst__DOT__DIMUX) 
                         >> 7U));
            vlSelfRef.top__DOT__cpu_inst__DOT__CO = 
                (1U & (((IData)(vlSelfRef.top__DOT__cpu_inst__DOT__ALU__DOT__temp) 
                        >> 8U) | ((IData)(vlSelfRef.top__DOT__cpu_inst__DOT____Vcellinp__ALU__BCD) 
                                  & (5U <= (7U & ((IData)(vlSelfRef.top__DOT__cpu_inst__DOT__ALU__DOT__temp_h) 
                                                  >> 1U))))));
        }
        vlSelfRef.top__DOT____Vcellinp__cpu_inst__RDY 
            = ((~ (IData)(vlSelfRef.dbg_pause)) & (
                                                   (0U 
                                                    == (IData)(vlSelfRef.top__DOT__blt_inst__DOT__state)) 
                                                   | (6U 
                                                      == (IData)(vlSelfRef.top__DOT__blt_inst__DOT__state))));
        vlSelfRef.top__DOT__cpu_inst__DOT__DIMUX = 
            ((IData)(vlSelfRef.top__DOT____Vcellinp__cpu_inst__RDY)
              ? (IData)(vlSelfRef.top__DOT__cpu_din)
              : (IData)(vlSelfRef.top__DOT__cpu_inst__DOT__DIHOLD));
        vlSelfRef.top__DOT__cpu_inst__DOT__IR = ((IData)(vlSelfRef.__VdfgRegularize_hebeb780c_0_3)
                                                  ? 0U
                                                  : 
                                                 ((IData)(vlSelfRef.top__DOT__cpu_inst__DOT__IRHOLD_valid)
                                                   ? (IData)(vlSelfRef.top__DOT__cpu_inst__DOT__IRHOLD)
                                                   : (IData)(vlSelfRef.top__DOT__cpu_inst__DOT__DIMUX)));
        vlSelfRef.__VdfgRegularize_hebeb780c_0_0 = 
            (((IData)(vlSelfRef.top__DOT__cpu_inst__DOT__DIMUX) 
              << 8U) | (IData)(vlSelfRef.top__DOT__cpu_inst__DOT__ADD));
    }
    if ((3ULL & vlSelfRef.__VnbaTriggered[0U])) {
        vlSelfRef.top__DOT__cpu_inst__DOT__state = vlSelfRef.__Vdly__top__DOT__cpu_inst__DOT__state;
    }
    if ((3ULL & vlSelfRef.__VnbaTriggered[0U])) {
        Vtop___024root___nba_comb__TOP__0(vlSelf);
    }
}

void Vtop___024root___trigger_orInto__act_vec_vec(VlUnpacked<QData/*63:0*/, 1> &out, const VlUnpacked<QData/*63:0*/, 1> &in) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vtop___024root___trigger_orInto__act_vec_vec\n"); );
    // Locals
    IData/*31:0*/ n;
    // Body
    n = 0U;
    do {
        out[n] = (out[n] | in[n]);
        n = ((IData)(1U) + n);
    } while ((0U >= n));
}

void Vtop___024root___eval_triggers_vec__act(Vtop___024root* vlSelf);
#ifdef VL_DEBUG
VL_ATTR_COLD void Vtop___024root___dump_triggers__act(const VlUnpacked<QData/*63:0*/, 1> &triggers, const std::string &tag);
#endif  // VL_DEBUG

bool Vtop___024root___eval_phase__act(Vtop___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vtop___024root___eval_phase__act\n"); );
    Vtop__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
    // Body
    Vtop___024root___eval_triggers_vec__act(vlSelf);
#ifdef VL_DEBUG
    if (VL_UNLIKELY(vlSymsp->_vm_contextp__->debug())) {
        Vtop___024root___dump_triggers__act(vlSelfRef.__VactTriggered, "act"s);
    }
#endif
    Vtop___024root___trigger_orInto__act_vec_vec(vlSelfRef.__VnbaTriggered, vlSelfRef.__VactTriggered);
    return (0U);
}

void Vtop___024root___trigger_clear__act(VlUnpacked<QData/*63:0*/, 1> &out) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vtop___024root___trigger_clear__act\n"); );
    // Locals
    IData/*31:0*/ n;
    // Body
    n = 0U;
    do {
        out[n] = 0ULL;
        n = ((IData)(1U) + n);
    } while ((1U > n));
}

bool Vtop___024root___trigger_anySet__act(const VlUnpacked<QData/*63:0*/, 1> &in);

bool Vtop___024root___eval_phase__nba(Vtop___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vtop___024root___eval_phase__nba\n"); );
    Vtop__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
    // Locals
    CData/*0:0*/ __VnbaExecute;
    // Body
    __VnbaExecute = Vtop___024root___trigger_anySet__act(vlSelfRef.__VnbaTriggered);
    if (__VnbaExecute) {
        Vtop___024root___eval_nba(vlSelf);
        Vtop___024root___trigger_clear__act(vlSelfRef.__VnbaTriggered);
    }
    return (__VnbaExecute);
}

#ifdef VL_DEBUG
VL_ATTR_COLD void Vtop___024root___dump_triggers__ico(const VlUnpacked<QData/*63:0*/, 1> &triggers, const std::string &tag);
#endif  // VL_DEBUG
bool Vtop___024root___eval_phase__ico(Vtop___024root* vlSelf);

void Vtop___024root___eval(Vtop___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vtop___024root___eval\n"); );
    Vtop__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
    // Locals
    IData/*31:0*/ __VicoIterCount;
    IData/*31:0*/ __VnbaIterCount;
    // Body
    __VicoIterCount = 0U;
    vlSelfRef.__VicoFirstIteration = 1U;
    do {
        if (VL_UNLIKELY(((0x00000064U < __VicoIterCount)))) {
#ifdef VL_DEBUG
            Vtop___024root___dump_triggers__ico(vlSelfRef.__VicoTriggered, "ico"s);
#endif
            VL_FATAL_MT("rtl/top.sv", 6, "", "DIDNOTCONVERGE: Input combinational region did not converge after '--converge-limit' of 100 tries");
        }
        __VicoIterCount = ((IData)(1U) + __VicoIterCount);
        vlSelfRef.__VicoPhaseResult = Vtop___024root___eval_phase__ico(vlSelf);
        vlSelfRef.__VicoFirstIteration = 0U;
    } while (vlSelfRef.__VicoPhaseResult);
    __VnbaIterCount = 0U;
    do {
        if (VL_UNLIKELY(((0x00000064U < __VnbaIterCount)))) {
#ifdef VL_DEBUG
            Vtop___024root___dump_triggers__act(vlSelfRef.__VnbaTriggered, "nba"s);
#endif
            VL_FATAL_MT("rtl/top.sv", 6, "", "DIDNOTCONVERGE: NBA region did not converge after '--converge-limit' of 100 tries");
        }
        __VnbaIterCount = ((IData)(1U) + __VnbaIterCount);
        vlSelfRef.__VactIterCount = 0U;
        do {
            if (VL_UNLIKELY(((0x00000064U < vlSelfRef.__VactIterCount)))) {
#ifdef VL_DEBUG
                Vtop___024root___dump_triggers__act(vlSelfRef.__VactTriggered, "act"s);
#endif
                VL_FATAL_MT("rtl/top.sv", 6, "", "DIDNOTCONVERGE: Active region did not converge after '--converge-limit' of 100 tries");
            }
            vlSelfRef.__VactIterCount = ((IData)(1U) 
                                         + vlSelfRef.__VactIterCount);
            vlSelfRef.__VactPhaseResult = Vtop___024root___eval_phase__act(vlSelf);
        } while (vlSelfRef.__VactPhaseResult);
        vlSelfRef.__VnbaPhaseResult = Vtop___024root___eval_phase__nba(vlSelf);
    } while (vlSelfRef.__VnbaPhaseResult);
}

#ifdef VL_DEBUG
void Vtop___024root___eval_debug_assertions(Vtop___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vtop___024root___eval_debug_assertions\n"); );
    Vtop__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
    // Body
    if (VL_UNLIKELY(((vlSelfRef.clk & 0xfeU)))) {
        Verilated::overWidthError("clk");
    }
    if (VL_UNLIKELY(((vlSelfRef.rst & 0xfeU)))) {
        Verilated::overWidthError("rst");
    }
    if (VL_UNLIKELY(((vlSelfRef.key_valid & 0xfeU)))) {
        Verilated::overWidthError("key_valid");
    }
    if (VL_UNLIKELY(((vlSelfRef.irq_n & 0xfeU)))) {
        Verilated::overWidthError("irq_n");
    }
    if (VL_UNLIKELY(((vlSelfRef.nmi_n & 0xfeU)))) {
        Verilated::overWidthError("nmi_n");
    }
    if (VL_UNLIKELY(((vlSelfRef.dbg_peek_en & 0xfeU)))) {
        Verilated::overWidthError("dbg_peek_en");
    }
    if (VL_UNLIKELY(((vlSelfRef.dbg_poke_en & 0xfeU)))) {
        Verilated::overWidthError("dbg_poke_en");
    }
    if (VL_UNLIKELY(((vlSelfRef.dbg_pause & 0xfeU)))) {
        Verilated::overWidthError("dbg_pause");
    }
}
#endif  // VL_DEBUG
