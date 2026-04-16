// Verilated -*- C++ -*-
// DESCRIPTION: Verilator output: Design implementation internals
// See Vtop.h for the primary calling header

#include "Vtop__pch.h"

VL_ATTR_COLD void Vtop_sid_chip___eval_static__TOP__top__DOT__sid1_inst(Vtop_sid_chip* vlSelf);

VL_ATTR_COLD void Vtop___024root___eval_static(Vtop___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vtop___024root___eval_static\n"); );
    Vtop__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
    // Body
    vlSelfRef.top__DOT__cpu_inst__DOT__C = 0U;
    vlSelfRef.top__DOT__cpu_inst__DOT__Z = 0U;
    vlSelfRef.top__DOT__cpu_inst__DOT__I = 0U;
    vlSelfRef.top__DOT__cpu_inst__DOT__D = 0U;
    vlSelfRef.top__DOT__cpu_inst__DOT__V = 0U;
    vlSelfRef.top__DOT__cpu_inst__DOT__N = 0U;
    vlSelfRef.top__DOT__cpu_inst__DOT__NMI_edge = 0U;
    vlSelfRef.top__DOT__cpu_inst__DOT__NMI_1 = 0U;
    const uint64_t __VscopeHash = VL_MURMUR64_HASH(vlSelf->vlNamep);
    vlSelf->top__DOT__vgc_inst__DOT__sprite_inst__DOT__unnamedblk2__DOT__fy = VL_SCOPED_RAND_RESET_I(4, __VscopeHash, 13807607874777985512ull);
    vlSelf->top__DOT__vgc_inst__DOT__sprite_inst__DOT__unnamedblk3__DOT__fx = VL_SCOPED_RAND_RESET_I(4, __VscopeHash, 8371614299050595205ull);
    vlSelf->top__DOT__vgc_inst__DOT__sprite_inst__DOT__unnamedblk3__DOT__color = VL_SCOPED_RAND_RESET_I(4, __VscopeHash, 10219578349838606127ull);
    vlSelf->top__DOT__vgc_inst__DOT__sprite_inst__DOT__unnamedblk3__DOT__screen_x = VL_SCOPED_RAND_RESET_I(9, __VscopeHash, 17355996989879729256ull);
    Vtop_sid_chip___eval_static__TOP__top__DOT__sid1_inst((&vlSymsp->TOP__top__DOT__sid1_inst));
    Vtop_sid_chip___eval_static__TOP__top__DOT__sid1_inst((&vlSymsp->TOP__top__DOT__sid2_inst));
    vlSelfRef.__Vtrigprevexpr___TOP__top__DOT__sid1_inst____PVT__v1__DOT__waveform_dac__DOT__bitsum__0 
        = vlSymsp->TOP__top__DOT__sid1_inst.__PVT__v1__DOT__waveform_dac__DOT__bitsum;
    vlSelfRef.__Vtrigprevexpr___TOP__top__DOT__sid1_inst____PVT__v1__DOT__envelope_dac__DOT__bitsum__0 
        = vlSymsp->TOP__top__DOT__sid1_inst.__PVT__v1__DOT__envelope_dac__DOT__bitsum;
    vlSelfRef.__Vtrigprevexpr___TOP__top__DOT__sid1_inst____PVT__v2__DOT__waveform_dac__DOT__bitsum__0 
        = vlSymsp->TOP__top__DOT__sid1_inst.__PVT__v2__DOT__waveform_dac__DOT__bitsum;
    vlSelfRef.__Vtrigprevexpr___TOP__top__DOT__sid1_inst____PVT__v2__DOT__envelope_dac__DOT__bitsum__0 
        = vlSymsp->TOP__top__DOT__sid1_inst.__PVT__v2__DOT__envelope_dac__DOT__bitsum;
    vlSelfRef.__Vtrigprevexpr___TOP__top__DOT__sid1_inst____PVT__v3__DOT__waveform_dac__DOT__bitsum__0 
        = vlSymsp->TOP__top__DOT__sid1_inst.__PVT__v3__DOT__waveform_dac__DOT__bitsum;
    vlSelfRef.__Vtrigprevexpr___TOP__top__DOT__sid1_inst____PVT__v3__DOT__envelope_dac__DOT__bitsum__0 
        = vlSymsp->TOP__top__DOT__sid1_inst.__PVT__v3__DOT__envelope_dac__DOT__bitsum;
    vlSelfRef.__Vtrigprevexpr___TOP__top__DOT__sid2_inst____PVT__v1__DOT__waveform_dac__DOT__bitsum__0 
        = vlSymsp->TOP__top__DOT__sid2_inst.__PVT__v1__DOT__waveform_dac__DOT__bitsum;
    vlSelfRef.__Vtrigprevexpr___TOP__top__DOT__sid2_inst____PVT__v1__DOT__envelope_dac__DOT__bitsum__0 
        = vlSymsp->TOP__top__DOT__sid2_inst.__PVT__v1__DOT__envelope_dac__DOT__bitsum;
    vlSelfRef.__Vtrigprevexpr___TOP__top__DOT__sid2_inst____PVT__v2__DOT__waveform_dac__DOT__bitsum__0 
        = vlSymsp->TOP__top__DOT__sid2_inst.__PVT__v2__DOT__waveform_dac__DOT__bitsum;
    vlSelfRef.__Vtrigprevexpr___TOP__top__DOT__sid2_inst____PVT__v2__DOT__envelope_dac__DOT__bitsum__0 
        = vlSymsp->TOP__top__DOT__sid2_inst.__PVT__v2__DOT__envelope_dac__DOT__bitsum;
    vlSelfRef.__Vtrigprevexpr___TOP__top__DOT__sid2_inst____PVT__v3__DOT__waveform_dac__DOT__bitsum__0 
        = vlSymsp->TOP__top__DOT__sid2_inst.__PVT__v3__DOT__waveform_dac__DOT__bitsum;
    vlSelfRef.__Vtrigprevexpr___TOP__top__DOT__sid2_inst____PVT__v3__DOT__envelope_dac__DOT__bitsum__0 
        = vlSymsp->TOP__top__DOT__sid2_inst.__PVT__v3__DOT__envelope_dac__DOT__bitsum;
    vlSelfRef.__Vtrigprevexpr___TOP__top__DOT__sid1_inst____PVT__v1__DOT__waveform_dac__DOT__bitsum__1 
        = vlSymsp->TOP__top__DOT__sid1_inst.__PVT__v1__DOT__waveform_dac__DOT__bitsum;
    vlSelfRef.__Vtrigprevexpr___TOP__top__DOT__sid1_inst____PVT__v1__DOT__envelope_dac__DOT__bitsum__1 
        = vlSymsp->TOP__top__DOT__sid1_inst.__PVT__v1__DOT__envelope_dac__DOT__bitsum;
    vlSelfRef.__Vtrigprevexpr___TOP__top__DOT__sid1_inst____PVT__v2__DOT__waveform_dac__DOT__bitsum__1 
        = vlSymsp->TOP__top__DOT__sid1_inst.__PVT__v2__DOT__waveform_dac__DOT__bitsum;
    vlSelfRef.__Vtrigprevexpr___TOP__top__DOT__sid1_inst____PVT__v2__DOT__envelope_dac__DOT__bitsum__1 
        = vlSymsp->TOP__top__DOT__sid1_inst.__PVT__v2__DOT__envelope_dac__DOT__bitsum;
    vlSelfRef.__Vtrigprevexpr___TOP__top__DOT__sid1_inst____PVT__v3__DOT__waveform_dac__DOT__bitsum__1 
        = vlSymsp->TOP__top__DOT__sid1_inst.__PVT__v3__DOT__waveform_dac__DOT__bitsum;
    vlSelfRef.__Vtrigprevexpr___TOP__top__DOT__sid1_inst____PVT__v3__DOT__envelope_dac__DOT__bitsum__1 
        = vlSymsp->TOP__top__DOT__sid1_inst.__PVT__v3__DOT__envelope_dac__DOT__bitsum;
    vlSelfRef.__Vtrigprevexpr___TOP__top__DOT__sid2_inst____PVT__v1__DOT__waveform_dac__DOT__bitsum__1 
        = vlSymsp->TOP__top__DOT__sid2_inst.__PVT__v1__DOT__waveform_dac__DOT__bitsum;
    vlSelfRef.__Vtrigprevexpr___TOP__top__DOT__sid2_inst____PVT__v1__DOT__envelope_dac__DOT__bitsum__1 
        = vlSymsp->TOP__top__DOT__sid2_inst.__PVT__v1__DOT__envelope_dac__DOT__bitsum;
    vlSelfRef.__Vtrigprevexpr___TOP__top__DOT__sid2_inst____PVT__v2__DOT__waveform_dac__DOT__bitsum__1 
        = vlSymsp->TOP__top__DOT__sid2_inst.__PVT__v2__DOT__waveform_dac__DOT__bitsum;
    vlSelfRef.__Vtrigprevexpr___TOP__top__DOT__sid2_inst____PVT__v2__DOT__envelope_dac__DOT__bitsum__1 
        = vlSymsp->TOP__top__DOT__sid2_inst.__PVT__v2__DOT__envelope_dac__DOT__bitsum;
    vlSelfRef.__Vtrigprevexpr___TOP__top__DOT__sid2_inst____PVT__v3__DOT__waveform_dac__DOT__bitsum__1 
        = vlSymsp->TOP__top__DOT__sid2_inst.__PVT__v3__DOT__waveform_dac__DOT__bitsum;
    vlSelfRef.__Vtrigprevexpr___TOP__top__DOT__sid2_inst____PVT__v3__DOT__envelope_dac__DOT__bitsum__1 
        = vlSymsp->TOP__top__DOT__sid2_inst.__PVT__v3__DOT__envelope_dac__DOT__bitsum;
    vlSelfRef.__Vtrigprevexpr___TOP__clk__0 = vlSelfRef.clk;
    vlSelfRef.__Vtrigprevexpr___TOP__rst__0 = vlSelfRef.rst;
}

VL_ATTR_COLD void Vtop___024root___eval_static__TOP(Vtop___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vtop___024root___eval_static__TOP\n"); );
    Vtop__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
    // Body
    vlSelfRef.top__DOT__cpu_inst__DOT__C = 0U;
    vlSelfRef.top__DOT__cpu_inst__DOT__Z = 0U;
    vlSelfRef.top__DOT__cpu_inst__DOT__I = 0U;
    vlSelfRef.top__DOT__cpu_inst__DOT__D = 0U;
    vlSelfRef.top__DOT__cpu_inst__DOT__V = 0U;
    vlSelfRef.top__DOT__cpu_inst__DOT__N = 0U;
    vlSelfRef.top__DOT__cpu_inst__DOT__NMI_edge = 0U;
    vlSelfRef.top__DOT__cpu_inst__DOT__NMI_1 = 0U;
    const uint64_t __VscopeHash = VL_MURMUR64_HASH(vlSelf->vlNamep);
    vlSelf->top__DOT__vgc_inst__DOT__sprite_inst__DOT__unnamedblk2__DOT__fy = VL_SCOPED_RAND_RESET_I(4, __VscopeHash, 13807607874777985512ull);
    vlSelf->top__DOT__vgc_inst__DOT__sprite_inst__DOT__unnamedblk3__DOT__fx = VL_SCOPED_RAND_RESET_I(4, __VscopeHash, 8371614299050595205ull);
    vlSelf->top__DOT__vgc_inst__DOT__sprite_inst__DOT__unnamedblk3__DOT__color = VL_SCOPED_RAND_RESET_I(4, __VscopeHash, 10219578349838606127ull);
    vlSelf->top__DOT__vgc_inst__DOT__sprite_inst__DOT__unnamedblk3__DOT__screen_x = VL_SCOPED_RAND_RESET_I(9, __VscopeHash, 17355996989879729256ull);
}

VL_ATTR_COLD void Vtop___024root___eval_initial__TOP(Vtop___024root* vlSelf);
VL_ATTR_COLD void Vtop_sid_chip___eval_initial__TOP__top__DOT__sid1_inst(Vtop_sid_chip* vlSelf);

VL_ATTR_COLD void Vtop___024root___eval_initial(Vtop___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vtop___024root___eval_initial\n"); );
    Vtop__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
    // Body
    Vtop___024root___eval_initial__TOP(vlSelf);
    Vtop_sid_chip___eval_initial__TOP__top__DOT__sid1_inst((&vlSymsp->TOP__top__DOT__sid1_inst));
    Vtop_sid_chip___eval_initial__TOP__top__DOT__sid1_inst((&vlSymsp->TOP__top__DOT__sid2_inst));
}

VL_ATTR_COLD void Vtop___024root___eval_initial__TOP(Vtop___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vtop___024root___eval_initial__TOP\n"); );
    Vtop__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
    // Locals
    IData/*31:0*/ top__DOT__unnamedblk1__DOT__i;
    top__DOT__unnamedblk1__DOT__i = 0;
    IData/*31:0*/ top__DOT__unnamedblk2__DOT__i;
    top__DOT__unnamedblk2__DOT__i = 0;
    IData/*31:0*/ top__DOT__unnamedblk3__DOT__i;
    top__DOT__unnamedblk3__DOT__i = 0;
    IData/*31:0*/ top__DOT__vgc_inst__DOT__unnamedblk6__DOT__i;
    top__DOT__vgc_inst__DOT__unnamedblk6__DOT__i = 0;
    IData/*31:0*/ top__DOT__vgc_inst__DOT__unnamedblk7__DOT__i;
    top__DOT__vgc_inst__DOT__unnamedblk7__DOT__i = 0;
    IData/*31:0*/ top__DOT__vgc_inst__DOT__text_inst__DOT__char_mem__DOT__gen_init_zero__DOT__i;
    top__DOT__vgc_inst__DOT__text_inst__DOT__char_mem__DOT__gen_init_zero__DOT__i = 0;
    IData/*31:0*/ top__DOT__vgc_inst__DOT__text_inst__DOT__color_mem__DOT__gen_init_zero__DOT__i;
    top__DOT__vgc_inst__DOT__text_inst__DOT__color_mem__DOT__gen_init_zero__DOT__i = 0;
    IData/*31:0*/ top__DOT__vgc_inst__DOT__gfx_inst__DOT__gfx_mem__DOT__gen_init_zero__DOT__i;
    top__DOT__vgc_inst__DOT__gfx_inst__DOT__gfx_mem__DOT__gen_init_zero__DOT__i = 0;
    IData/*31:0*/ top__DOT__vgc_inst__DOT__artist_inst__DOT__fill_stack_mem__DOT__gen_init_zero__DOT__i;
    top__DOT__vgc_inst__DOT__artist_inst__DOT__fill_stack_mem__DOT__gen_init_zero__DOT__i = 0;
    IData/*31:0*/ top__DOT__vgc_inst__DOT__sprite_inst__DOT__spr_mem__DOT__gen_init_zero__DOT__i;
    top__DOT__vgc_inst__DOT__sprite_inst__DOT__spr_mem__DOT__gen_init_zero__DOT__i = 0;
    IData/*31:0*/ top__DOT__vgc_inst__DOT__sprite_inst__DOT__scanline_buf__DOT__gen_init_zero__DOT__i;
    top__DOT__vgc_inst__DOT__sprite_inst__DOT__scanline_buf__DOT__gen_init_zero__DOT__i = 0;
    IData/*31:0*/ top__DOT__vgc_inst__DOT__tile_inst__DOT__tile_data_ram__DOT__gen_init_zero__DOT__i;
    top__DOT__vgc_inst__DOT__tile_inst__DOT__tile_data_ram__DOT__gen_init_zero__DOT__i = 0;
    IData/*31:0*/ top__DOT__vgc_inst__DOT__tile_inst__DOT__nametable_ram__DOT__gen_init_zero__DOT__i;
    top__DOT__vgc_inst__DOT__tile_inst__DOT__nametable_ram__DOT__gen_init_zero__DOT__i = 0;
    IData/*31:0*/ top__DOT__vgc_inst__DOT__tile_inst__DOT__attr_table_ram__DOT__gen_init_zero__DOT__i;
    top__DOT__vgc_inst__DOT__tile_inst__DOT__attr_table_ram__DOT__gen_init_zero__DOT__i = 0;
    IData/*31:0*/ __Vilp1;
    IData/*31:0*/ __Vilp2;
    // Body
    top__DOT__unnamedblk1__DOT__i = 0U;
    while (VL_GTS_III(32, 0x00010000U, top__DOT__unnamedblk1__DOT__i)) {
        vlSelfRef.top__DOT__ram[(0x0000ffffU & top__DOT__unnamedblk1__DOT__i)] = 0U;
        top__DOT__unnamedblk1__DOT__i = ((IData)(1U) 
                                         + top__DOT__unnamedblk1__DOT__i);
    }
    top__DOT__unnamedblk2__DOT__i = 0U;
    while (VL_GTS_III(32, 0x00004000U, top__DOT__unnamedblk2__DOT__i)) {
        vlSelfRef.top__DOT__basic_rom[(0x00003fffU 
                                       & top__DOT__unnamedblk2__DOT__i)] = 0U;
        vlSelfRef.top__DOT__ext_rom[(0x00003fffU & top__DOT__unnamedblk2__DOT__i)] = 0U;
        top__DOT__unnamedblk2__DOT__i = ((IData)(1U) 
                                         + top__DOT__unnamedblk2__DOT__i);
    }
    VL_READMEM_N(true, 8, 16384, 0, "rom/ehbasic.hex"s
                 ,  &(vlSelfRef.top__DOT__basic_rom)
                 , 0U, 0x3fffU);
    VL_READMEM_N(true, 8, 16384, 0, "rom/extension.hex"s
                 ,  &(vlSelfRef.top__DOT__ext_rom), 0U
                 , 0x3fffU);
    vlSelfRef.top__DOT__ext_rom_active = 0U;
    top__DOT__unnamedblk3__DOT__i = 0U;
    while (VL_GTS_III(32, 0x00080000U, top__DOT__unnamedblk3__DOT__i)) {
        vlSelfRef.top__DOT__xram[(0x0007ffffU & top__DOT__unnamedblk3__DOT__i)] = 0U;
        top__DOT__unnamedblk3__DOT__i = ((IData)(1U) 
                                         + top__DOT__unnamedblk3__DOT__i);
    }
    __Vilp1 = 0U;
    while ((__Vilp1 <= 0x0000003fU)) {
        vlSelfRef.top__DOT__xmc_regs[__Vilp1] = 0U;
        __Vilp1 = ((IData)(1U) + __Vilp1);
    }
    vlSelfRef.top__DOT__xmc_regs[13U] = 8U;
    vlSelfRef.top__DOT__xmc_regs[22U] = 0x0fU;
    vlSelfRef.top__DOT__blt_inst__DOT__regs[0U] = 0U;
    vlSelfRef.top__DOT__blt_inst__DOT__regs[1U] = 0U;
    vlSelfRef.top__DOT__blt_inst__DOT__regs[2U] = 0U;
    vlSelfRef.top__DOT__blt_inst__DOT__regs[3U] = 0U;
    vlSelfRef.top__DOT__blt_inst__DOT__regs[4U] = 0U;
    vlSelfRef.top__DOT__blt_inst__DOT__regs[5U] = 0U;
    vlSelfRef.top__DOT__blt_inst__DOT__regs[6U] = 0U;
    vlSelfRef.top__DOT__blt_inst__DOT__regs[7U] = 0U;
    vlSelfRef.top__DOT__blt_inst__DOT__regs[8U] = 0U;
    vlSelfRef.top__DOT__blt_inst__DOT__regs[9U] = 0U;
    vlSelfRef.top__DOT__blt_inst__DOT__regs[10U] = 0U;
    vlSelfRef.top__DOT__blt_inst__DOT__regs[11U] = 0U;
    vlSelfRef.top__DOT__blt_inst__DOT__regs[12U] = 0U;
    vlSelfRef.top__DOT__blt_inst__DOT__regs[13U] = 0U;
    vlSelfRef.top__DOT__blt_inst__DOT__regs[14U] = 0U;
    vlSelfRef.top__DOT__blt_inst__DOT__regs[15U] = 0U;
    vlSelfRef.top__DOT__blt_inst__DOT__regs[16U] = 0U;
    vlSelfRef.top__DOT__blt_inst__DOT__regs[17U] = 0U;
    vlSelfRef.top__DOT__blt_inst__DOT__regs[18U] = 0U;
    vlSelfRef.top__DOT__blt_inst__DOT__regs[19U] = 0U;
    vlSelfRef.top__DOT__blt_inst__DOT__regs[20U] = 0U;
    vlSelfRef.top__DOT__blt_inst__DOT__regs[21U] = 0U;
    vlSelfRef.top__DOT__blt_inst__DOT__regs[22U] = 0U;
    vlSelfRef.top__DOT__blt_inst__DOT__regs[23U] = 0U;
    vlSelfRef.top__DOT__blt_inst__DOT__regs[24U] = 0U;
    vlSelfRef.top__DOT__blt_inst__DOT__regs[1U] = 0U;
    vlSelfRef.top__DOT__blt_inst__DOT__state = 0U;
    vlSelfRef.top__DOT__blt_inst__DOT__fill_mode = 0U;
    vlSelfRef.top__DOT__blt_inst__DOT__colorkey_mode = 0U;
    vlSelfRef.top__DOT__blt_inst__DOT__use_row_buffer = 0U;
    vlSelfRef.top__DOT__blt_inst__DOT__src_space = 0U;
    vlSelfRef.top__DOT__blt_inst__DOT__dst_space = 0U;
    vlSelfRef.top__DOT__blt_inst__DOT__src_base = 0U;
    vlSelfRef.top__DOT__blt_inst__DOT__dst_base = 0U;
    vlSelfRef.top__DOT__blt_inst__DOT__width = 0U;
    vlSelfRef.top__DOT__blt_inst__DOT__height = 0U;
    vlSelfRef.top__DOT__blt_inst__DOT__src_stride = 0U;
    vlSelfRef.top__DOT__blt_inst__DOT__dst_stride = 0U;
    vlSelfRef.top__DOT__blt_inst__DOT__fill_value = 0U;
    vlSelfRef.top__DOT__blt_inst__DOT__color_key = 0U;
    vlSelfRef.top__DOT__blt_inst__DOT__row = 0U;
    vlSelfRef.top__DOT__blt_inst__DOT__col = 0U;
    vlSelfRef.top__DOT__blt_inst__DOT__wrote_count = 0U;
    vlSelfRef.top__DOT__blt_inst__DOT__read_byte = 0U;
    vlSelfRef.top__DOT__blt_inst__DOT__read_valid = 0U;
    vlSelfRef.top__DOT__blt_inst__DOT__buf_idx = 0U;
    vlSelfRef.top__DOT__vgc_inst__DOT__palette[0U] = 0U;
    vlSelfRef.top__DOT__vgc_inst__DOT__palette[1U] = 0x0fffU;
    vlSelfRef.top__DOT__vgc_inst__DOT__palette[2U] = 0x0800U;
    vlSelfRef.top__DOT__vgc_inst__DOT__palette[3U] = 0x0afeU;
    vlSelfRef.top__DOT__vgc_inst__DOT__palette[4U] = 0x0c4cU;
    vlSelfRef.top__DOT__vgc_inst__DOT__palette[5U] = 0x00c5U;
    vlSelfRef.top__DOT__vgc_inst__DOT__palette[6U] = 0x000aU;
    vlSelfRef.top__DOT__vgc_inst__DOT__palette[7U] = 0x0ee7U;
    vlSelfRef.top__DOT__vgc_inst__DOT__palette[8U] = 0x0d85U;
    vlSelfRef.top__DOT__vgc_inst__DOT__palette[9U] = 0x0640U;
    vlSelfRef.top__DOT__vgc_inst__DOT__palette[10U] = 0x0f77U;
    vlSelfRef.top__DOT__vgc_inst__DOT__palette[11U] = 0x0333U;
    vlSelfRef.top__DOT__vgc_inst__DOT__palette[12U] = 0x0777U;
    vlSelfRef.top__DOT__vgc_inst__DOT__palette[13U] = 0x08f6U;
    vlSelfRef.top__DOT__vgc_inst__DOT__palette[14U] = 0x008fU;
    vlSelfRef.top__DOT__vgc_inst__DOT__palette[15U] = 0x0bbbU;
    vlSelfRef.top__DOT__vgc_inst__DOT__spr_x[0U] = 0U;
    vlSelfRef.top__DOT__vgc_inst__DOT__spr_y[0U] = 0U;
    vlSelfRef.top__DOT__vgc_inst__DOT__spr_enable[0U] = 0U;
    vlSelfRef.top__DOT__vgc_inst__DOT__spr_flip_h[0U] = 0U;
    vlSelfRef.top__DOT__vgc_inst__DOT__spr_flip_v[0U] = 0U;
    vlSelfRef.top__DOT__vgc_inst__DOT__spr_pri[0U] = 1U;
    vlSelfRef.top__DOT__vgc_inst__DOT__spr_shape[0U] = 0U;
    vlSelfRef.top__DOT__vgc_inst__DOT__spr_x[1U] = 0U;
    vlSelfRef.top__DOT__vgc_inst__DOT__spr_y[1U] = 0U;
    vlSelfRef.top__DOT__vgc_inst__DOT__spr_enable[1U] = 0U;
    vlSelfRef.top__DOT__vgc_inst__DOT__spr_flip_h[1U] = 0U;
    vlSelfRef.top__DOT__vgc_inst__DOT__spr_flip_v[1U] = 0U;
    vlSelfRef.top__DOT__vgc_inst__DOT__spr_pri[1U] = 1U;
    vlSelfRef.top__DOT__vgc_inst__DOT__spr_shape[1U] = 1U;
    vlSelfRef.top__DOT__vgc_inst__DOT__spr_x[2U] = 0U;
    vlSelfRef.top__DOT__vgc_inst__DOT__spr_y[2U] = 0U;
    vlSelfRef.top__DOT__vgc_inst__DOT__spr_enable[2U] = 0U;
    vlSelfRef.top__DOT__vgc_inst__DOT__spr_flip_h[2U] = 0U;
    vlSelfRef.top__DOT__vgc_inst__DOT__spr_flip_v[2U] = 0U;
    vlSelfRef.top__DOT__vgc_inst__DOT__spr_pri[2U] = 1U;
    vlSelfRef.top__DOT__vgc_inst__DOT__spr_shape[2U] = 2U;
    vlSelfRef.top__DOT__vgc_inst__DOT__spr_x[3U] = 0U;
    vlSelfRef.top__DOT__vgc_inst__DOT__spr_y[3U] = 0U;
    vlSelfRef.top__DOT__vgc_inst__DOT__spr_enable[3U] = 0U;
    vlSelfRef.top__DOT__vgc_inst__DOT__spr_flip_h[3U] = 0U;
    vlSelfRef.top__DOT__vgc_inst__DOT__spr_flip_v[3U] = 0U;
    vlSelfRef.top__DOT__vgc_inst__DOT__spr_pri[3U] = 1U;
    vlSelfRef.top__DOT__vgc_inst__DOT__spr_shape[3U] = 3U;
    vlSelfRef.top__DOT__vgc_inst__DOT__spr_x[4U] = 0U;
    vlSelfRef.top__DOT__vgc_inst__DOT__spr_y[4U] = 0U;
    vlSelfRef.top__DOT__vgc_inst__DOT__spr_enable[4U] = 0U;
    vlSelfRef.top__DOT__vgc_inst__DOT__spr_flip_h[4U] = 0U;
    vlSelfRef.top__DOT__vgc_inst__DOT__spr_flip_v[4U] = 0U;
    vlSelfRef.top__DOT__vgc_inst__DOT__spr_pri[4U] = 1U;
    vlSelfRef.top__DOT__vgc_inst__DOT__spr_shape[4U] = 4U;
    vlSelfRef.top__DOT__vgc_inst__DOT__spr_x[5U] = 0U;
    vlSelfRef.top__DOT__vgc_inst__DOT__spr_y[5U] = 0U;
    vlSelfRef.top__DOT__vgc_inst__DOT__spr_enable[5U] = 0U;
    vlSelfRef.top__DOT__vgc_inst__DOT__spr_flip_h[5U] = 0U;
    vlSelfRef.top__DOT__vgc_inst__DOT__spr_flip_v[5U] = 0U;
    vlSelfRef.top__DOT__vgc_inst__DOT__spr_pri[5U] = 1U;
    vlSelfRef.top__DOT__vgc_inst__DOT__spr_shape[5U] = 5U;
    vlSelfRef.top__DOT__vgc_inst__DOT__spr_x[6U] = 0U;
    vlSelfRef.top__DOT__vgc_inst__DOT__spr_y[6U] = 0U;
    vlSelfRef.top__DOT__vgc_inst__DOT__spr_enable[6U] = 0U;
    vlSelfRef.top__DOT__vgc_inst__DOT__spr_flip_h[6U] = 0U;
    vlSelfRef.top__DOT__vgc_inst__DOT__spr_flip_v[6U] = 0U;
    vlSelfRef.top__DOT__vgc_inst__DOT__spr_pri[6U] = 1U;
    vlSelfRef.top__DOT__vgc_inst__DOT__spr_shape[6U] = 6U;
    vlSelfRef.top__DOT__vgc_inst__DOT__spr_x[7U] = 0U;
    vlSelfRef.top__DOT__vgc_inst__DOT__spr_y[7U] = 0U;
    vlSelfRef.top__DOT__vgc_inst__DOT__spr_enable[7U] = 0U;
    vlSelfRef.top__DOT__vgc_inst__DOT__spr_flip_h[7U] = 0U;
    vlSelfRef.top__DOT__vgc_inst__DOT__spr_flip_v[7U] = 0U;
    vlSelfRef.top__DOT__vgc_inst__DOT__spr_pri[7U] = 1U;
    vlSelfRef.top__DOT__vgc_inst__DOT__spr_shape[7U] = 7U;
    vlSelfRef.top__DOT__vgc_inst__DOT__spr_x[8U] = 0U;
    vlSelfRef.top__DOT__vgc_inst__DOT__spr_y[8U] = 0U;
    vlSelfRef.top__DOT__vgc_inst__DOT__spr_enable[8U] = 0U;
    vlSelfRef.top__DOT__vgc_inst__DOT__spr_flip_h[8U] = 0U;
    vlSelfRef.top__DOT__vgc_inst__DOT__spr_flip_v[8U] = 0U;
    vlSelfRef.top__DOT__vgc_inst__DOT__spr_pri[8U] = 1U;
    vlSelfRef.top__DOT__vgc_inst__DOT__spr_shape[8U] = 8U;
    vlSelfRef.top__DOT__vgc_inst__DOT__spr_x[9U] = 0U;
    vlSelfRef.top__DOT__vgc_inst__DOT__spr_y[9U] = 0U;
    vlSelfRef.top__DOT__vgc_inst__DOT__spr_enable[9U] = 0U;
    vlSelfRef.top__DOT__vgc_inst__DOT__spr_flip_h[9U] = 0U;
    vlSelfRef.top__DOT__vgc_inst__DOT__spr_flip_v[9U] = 0U;
    vlSelfRef.top__DOT__vgc_inst__DOT__spr_pri[9U] = 1U;
    vlSelfRef.top__DOT__vgc_inst__DOT__spr_shape[9U] = 9U;
    vlSelfRef.top__DOT__vgc_inst__DOT__spr_x[10U] = 0U;
    vlSelfRef.top__DOT__vgc_inst__DOT__spr_y[10U] = 0U;
    vlSelfRef.top__DOT__vgc_inst__DOT__spr_enable[10U] = 0U;
    vlSelfRef.top__DOT__vgc_inst__DOT__spr_flip_h[10U] = 0U;
    vlSelfRef.top__DOT__vgc_inst__DOT__spr_flip_v[10U] = 0U;
    vlSelfRef.top__DOT__vgc_inst__DOT__spr_pri[10U] = 1U;
    vlSelfRef.top__DOT__vgc_inst__DOT__spr_shape[10U] = 0x0aU;
    vlSelfRef.top__DOT__vgc_inst__DOT__spr_x[11U] = 0U;
    vlSelfRef.top__DOT__vgc_inst__DOT__spr_y[11U] = 0U;
    vlSelfRef.top__DOT__vgc_inst__DOT__spr_enable[11U] = 0U;
    vlSelfRef.top__DOT__vgc_inst__DOT__spr_flip_h[11U] = 0U;
    vlSelfRef.top__DOT__vgc_inst__DOT__spr_flip_v[11U] = 0U;
    vlSelfRef.top__DOT__vgc_inst__DOT__spr_pri[11U] = 1U;
    vlSelfRef.top__DOT__vgc_inst__DOT__spr_shape[11U] = 0x0bU;
    vlSelfRef.top__DOT__vgc_inst__DOT__spr_x[12U] = 0U;
    vlSelfRef.top__DOT__vgc_inst__DOT__spr_y[12U] = 0U;
    vlSelfRef.top__DOT__vgc_inst__DOT__spr_enable[12U] = 0U;
    vlSelfRef.top__DOT__vgc_inst__DOT__spr_flip_h[12U] = 0U;
    vlSelfRef.top__DOT__vgc_inst__DOT__spr_flip_v[12U] = 0U;
    vlSelfRef.top__DOT__vgc_inst__DOT__spr_pri[12U] = 1U;
    vlSelfRef.top__DOT__vgc_inst__DOT__spr_shape[12U] = 0x0cU;
    vlSelfRef.top__DOT__vgc_inst__DOT__spr_x[13U] = 0U;
    vlSelfRef.top__DOT__vgc_inst__DOT__spr_y[13U] = 0U;
    vlSelfRef.top__DOT__vgc_inst__DOT__spr_enable[13U] = 0U;
    vlSelfRef.top__DOT__vgc_inst__DOT__spr_flip_h[13U] = 0U;
    vlSelfRef.top__DOT__vgc_inst__DOT__spr_flip_v[13U] = 0U;
    vlSelfRef.top__DOT__vgc_inst__DOT__spr_pri[13U] = 1U;
    vlSelfRef.top__DOT__vgc_inst__DOT__spr_shape[13U] = 0x0dU;
    vlSelfRef.top__DOT__vgc_inst__DOT__spr_x[14U] = 0U;
    vlSelfRef.top__DOT__vgc_inst__DOT__spr_y[14U] = 0U;
    vlSelfRef.top__DOT__vgc_inst__DOT__spr_enable[14U] = 0U;
    vlSelfRef.top__DOT__vgc_inst__DOT__spr_flip_h[14U] = 0U;
    vlSelfRef.top__DOT__vgc_inst__DOT__spr_flip_v[14U] = 0U;
    vlSelfRef.top__DOT__vgc_inst__DOT__spr_pri[14U] = 1U;
    vlSelfRef.top__DOT__vgc_inst__DOT__spr_shape[14U] = 0x0eU;
    vlSelfRef.top__DOT__vgc_inst__DOT__spr_x[15U] = 0U;
    vlSelfRef.top__DOT__vgc_inst__DOT__spr_y[15U] = 0U;
    vlSelfRef.top__DOT__vgc_inst__DOT__spr_enable[15U] = 0U;
    vlSelfRef.top__DOT__vgc_inst__DOT__spr_flip_h[15U] = 0U;
    vlSelfRef.top__DOT__vgc_inst__DOT__spr_flip_v[15U] = 0U;
    vlSelfRef.top__DOT__vgc_inst__DOT__spr_pri[15U] = 1U;
    vlSelfRef.top__DOT__vgc_inst__DOT__spr_shape[15U] = 0x0fU;
    vlSelfRef.top__DOT__vgc_inst__DOT__copper_pos[0U] = 0U;
    vlSelfRef.top__DOT__vgc_inst__DOT__copper_reg[0U] = 0U;
    vlSelfRef.top__DOT__vgc_inst__DOT__copper_val[0U] = 0U;
    vlSelfRef.top__DOT__vgc_inst__DOT__copper_pos[1U] = 0U;
    vlSelfRef.top__DOT__vgc_inst__DOT__copper_reg[1U] = 0U;
    vlSelfRef.top__DOT__vgc_inst__DOT__copper_val[1U] = 0U;
    vlSelfRef.top__DOT__vgc_inst__DOT__copper_pos[2U] = 0U;
    vlSelfRef.top__DOT__vgc_inst__DOT__copper_reg[2U] = 0U;
    vlSelfRef.top__DOT__vgc_inst__DOT__copper_val[2U] = 0U;
    vlSelfRef.top__DOT__vgc_inst__DOT__copper_pos[3U] = 0U;
    vlSelfRef.top__DOT__vgc_inst__DOT__copper_reg[3U] = 0U;
    vlSelfRef.top__DOT__vgc_inst__DOT__copper_val[3U] = 0U;
    vlSelfRef.top__DOT__vgc_inst__DOT__copper_pos[4U] = 0U;
    vlSelfRef.top__DOT__vgc_inst__DOT__copper_reg[4U] = 0U;
    vlSelfRef.top__DOT__vgc_inst__DOT__copper_val[4U] = 0U;
    vlSelfRef.top__DOT__vgc_inst__DOT__copper_pos[5U] = 0U;
    vlSelfRef.top__DOT__vgc_inst__DOT__copper_reg[5U] = 0U;
    vlSelfRef.top__DOT__vgc_inst__DOT__copper_val[5U] = 0U;
    vlSelfRef.top__DOT__vgc_inst__DOT__copper_pos[6U] = 0U;
    vlSelfRef.top__DOT__vgc_inst__DOT__copper_reg[6U] = 0U;
    vlSelfRef.top__DOT__vgc_inst__DOT__copper_val[6U] = 0U;
    vlSelfRef.top__DOT__vgc_inst__DOT__copper_pos[7U] = 0U;
    vlSelfRef.top__DOT__vgc_inst__DOT__copper_reg[7U] = 0U;
    vlSelfRef.top__DOT__vgc_inst__DOT__copper_val[7U] = 0U;
    vlSelfRef.top__DOT__vgc_inst__DOT__copper_pos[8U] = 0U;
    vlSelfRef.top__DOT__vgc_inst__DOT__copper_reg[8U] = 0U;
    vlSelfRef.top__DOT__vgc_inst__DOT__copper_val[8U] = 0U;
    vlSelfRef.top__DOT__vgc_inst__DOT__copper_pos[9U] = 0U;
    vlSelfRef.top__DOT__vgc_inst__DOT__copper_reg[9U] = 0U;
    vlSelfRef.top__DOT__vgc_inst__DOT__copper_val[9U] = 0U;
    vlSelfRef.top__DOT__vgc_inst__DOT__copper_pos[10U] = 0U;
    vlSelfRef.top__DOT__vgc_inst__DOT__copper_reg[10U] = 0U;
    vlSelfRef.top__DOT__vgc_inst__DOT__copper_val[10U] = 0U;
    vlSelfRef.top__DOT__vgc_inst__DOT__copper_pos[11U] = 0U;
    vlSelfRef.top__DOT__vgc_inst__DOT__copper_reg[11U] = 0U;
    vlSelfRef.top__DOT__vgc_inst__DOT__copper_val[11U] = 0U;
    vlSelfRef.top__DOT__vgc_inst__DOT__copper_pos[12U] = 0U;
    vlSelfRef.top__DOT__vgc_inst__DOT__copper_reg[12U] = 0U;
    vlSelfRef.top__DOT__vgc_inst__DOT__copper_val[12U] = 0U;
    vlSelfRef.top__DOT__vgc_inst__DOT__copper_pos[13U] = 0U;
    vlSelfRef.top__DOT__vgc_inst__DOT__copper_reg[13U] = 0U;
    vlSelfRef.top__DOT__vgc_inst__DOT__copper_val[13U] = 0U;
    vlSelfRef.top__DOT__vgc_inst__DOT__copper_pos[14U] = 0U;
    vlSelfRef.top__DOT__vgc_inst__DOT__copper_reg[14U] = 0U;
    vlSelfRef.top__DOT__vgc_inst__DOT__copper_val[14U] = 0U;
    vlSelfRef.top__DOT__vgc_inst__DOT__copper_pos[15U] = 0U;
    vlSelfRef.top__DOT__vgc_inst__DOT__copper_reg[15U] = 0U;
    vlSelfRef.top__DOT__vgc_inst__DOT__copper_val[15U] = 0U;
    vlSelfRef.top__DOT__vgc_inst__DOT__copper_pos[16U] = 0U;
    vlSelfRef.top__DOT__vgc_inst__DOT__copper_reg[16U] = 0U;
    vlSelfRef.top__DOT__vgc_inst__DOT__copper_val[16U] = 0U;
    vlSelfRef.top__DOT__vgc_inst__DOT__copper_pos[17U] = 0U;
    vlSelfRef.top__DOT__vgc_inst__DOT__copper_reg[17U] = 0U;
    vlSelfRef.top__DOT__vgc_inst__DOT__copper_val[17U] = 0U;
    vlSelfRef.top__DOT__vgc_inst__DOT__copper_pos[18U] = 0U;
    vlSelfRef.top__DOT__vgc_inst__DOT__copper_reg[18U] = 0U;
    vlSelfRef.top__DOT__vgc_inst__DOT__copper_val[18U] = 0U;
    vlSelfRef.top__DOT__vgc_inst__DOT__copper_pos[19U] = 0U;
    vlSelfRef.top__DOT__vgc_inst__DOT__copper_reg[19U] = 0U;
    vlSelfRef.top__DOT__vgc_inst__DOT__copper_val[19U] = 0U;
    vlSelfRef.top__DOT__vgc_inst__DOT__copper_pos[20U] = 0U;
    vlSelfRef.top__DOT__vgc_inst__DOT__copper_reg[20U] = 0U;
    vlSelfRef.top__DOT__vgc_inst__DOT__copper_val[20U] = 0U;
    vlSelfRef.top__DOT__vgc_inst__DOT__copper_pos[21U] = 0U;
    vlSelfRef.top__DOT__vgc_inst__DOT__copper_reg[21U] = 0U;
    vlSelfRef.top__DOT__vgc_inst__DOT__copper_val[21U] = 0U;
    vlSelfRef.top__DOT__vgc_inst__DOT__copper_pos[22U] = 0U;
    vlSelfRef.top__DOT__vgc_inst__DOT__copper_reg[22U] = 0U;
    vlSelfRef.top__DOT__vgc_inst__DOT__copper_val[22U] = 0U;
    vlSelfRef.top__DOT__vgc_inst__DOT__copper_pos[23U] = 0U;
    vlSelfRef.top__DOT__vgc_inst__DOT__copper_reg[23U] = 0U;
    vlSelfRef.top__DOT__vgc_inst__DOT__copper_val[23U] = 0U;
    vlSelfRef.top__DOT__vgc_inst__DOT__copper_pos[24U] = 0U;
    vlSelfRef.top__DOT__vgc_inst__DOT__copper_reg[24U] = 0U;
    vlSelfRef.top__DOT__vgc_inst__DOT__copper_val[24U] = 0U;
    vlSelfRef.top__DOT__vgc_inst__DOT__copper_pos[25U] = 0U;
    vlSelfRef.top__DOT__vgc_inst__DOT__copper_reg[25U] = 0U;
    vlSelfRef.top__DOT__vgc_inst__DOT__copper_val[25U] = 0U;
    vlSelfRef.top__DOT__vgc_inst__DOT__copper_pos[26U] = 0U;
    vlSelfRef.top__DOT__vgc_inst__DOT__copper_reg[26U] = 0U;
    vlSelfRef.top__DOT__vgc_inst__DOT__copper_val[26U] = 0U;
    vlSelfRef.top__DOT__vgc_inst__DOT__copper_pos[27U] = 0U;
    vlSelfRef.top__DOT__vgc_inst__DOT__copper_reg[27U] = 0U;
    vlSelfRef.top__DOT__vgc_inst__DOT__copper_val[27U] = 0U;
    vlSelfRef.top__DOT__vgc_inst__DOT__copper_pos[28U] = 0U;
    vlSelfRef.top__DOT__vgc_inst__DOT__copper_reg[28U] = 0U;
    vlSelfRef.top__DOT__vgc_inst__DOT__copper_val[28U] = 0U;
    vlSelfRef.top__DOT__vgc_inst__DOT__copper_pos[29U] = 0U;
    vlSelfRef.top__DOT__vgc_inst__DOT__copper_reg[29U] = 0U;
    vlSelfRef.top__DOT__vgc_inst__DOT__copper_val[29U] = 0U;
    vlSelfRef.top__DOT__vgc_inst__DOT__copper_pos[30U] = 0U;
    vlSelfRef.top__DOT__vgc_inst__DOT__copper_reg[30U] = 0U;
    vlSelfRef.top__DOT__vgc_inst__DOT__copper_val[30U] = 0U;
    vlSelfRef.top__DOT__vgc_inst__DOT__copper_pos[31U] = 0U;
    vlSelfRef.top__DOT__vgc_inst__DOT__copper_reg[31U] = 0U;
    vlSelfRef.top__DOT__vgc_inst__DOT__copper_val[31U] = 0U;
    top__DOT__vgc_inst__DOT__unnamedblk6__DOT__i = 0U;
    while (VL_GTS_III(32, 0x00001000U, top__DOT__vgc_inst__DOT__unnamedblk6__DOT__i)) {
        vlSelfRef.top__DOT__vgc_inst__DOT__copper_list_pos[(0x00000fffU 
                                                            & top__DOT__vgc_inst__DOT__unnamedblk6__DOT__i)] = 0U;
        vlSelfRef.top__DOT__vgc_inst__DOT__copper_list_reg[(0x00000fffU 
                                                            & top__DOT__vgc_inst__DOT__unnamedblk6__DOT__i)] = 0U;
        vlSelfRef.top__DOT__vgc_inst__DOT__copper_list_val[(0x00000fffU 
                                                            & top__DOT__vgc_inst__DOT__unnamedblk6__DOT__i)] = 0U;
        top__DOT__vgc_inst__DOT__unnamedblk6__DOT__i 
            = ((IData)(1U) + top__DOT__vgc_inst__DOT__unnamedblk6__DOT__i);
    }
    top__DOT__vgc_inst__DOT__unnamedblk7__DOT__i = 0U;
    while (VL_GTS_III(32, 0x00000080U, top__DOT__vgc_inst__DOT__unnamedblk7__DOT__i)) {
        vlSelfRef.top__DOT__vgc_inst__DOT__copper_list_count[(0x0000007fU 
                                                              & top__DOT__vgc_inst__DOT__unnamedblk7__DOT__i)] = 0U;
        top__DOT__vgc_inst__DOT__unnamedblk7__DOT__i 
            = ((IData)(1U) + top__DOT__vgc_inst__DOT__unnamedblk7__DOT__i);
    }
    vlSelfRef.top__DOT__vgc_inst__DOT__copper_count = 0U;
    vlSelfRef.top__DOT__vgc_inst__DOT__copper_enabled = 0U;
    vlSelfRef.top__DOT__vgc_inst__DOT__copper_target_list = 0U;
    vlSelfRef.top__DOT__vgc_inst__DOT__copper_active_list = 0U;
    vlSelfRef.top__DOT__vgc_inst__DOT__copper_pending_list = 0U;
    vlSelfRef.top__DOT__vgc_inst__DOT__copper_loading = 0U;
    vlSelfRef.top__DOT__vgc_inst__DOT__copper_load_idx = 0U;
    vlSelfRef.top__DOT__vgc_inst__DOT__copper_load_src = 0U;
    __Vilp2 = 0U;
    while ((__Vilp2 <= 0x0000003fU)) {
        vlSelfRef.top__DOT__vgc_inst__DOT__fio_name[__Vilp2] = 0U;
        __Vilp2 = ((IData)(1U) + __Vilp2);
    }
    vlSelfRef.top__DOT__vgc_inst__DOT__fio_name_len = 0U;
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
    vlSelfRef.top__DOT__vgc_inst__DOT__cursor_x = 0U;
    vlSelfRef.top__DOT__vgc_inst__DOT__cursor_y = 0U;
    vlSelfRef.top__DOT__vgc_inst__DOT__fg_color = 1U;
    vlSelfRef.top__DOT__vgc_inst__DOT__bg_color = 6U;
    vlSelfRef.top__DOT__vgc_inst__DOT__border_color = 6U;
    vlSelfRef.top__DOT__vgc_inst__DOT__gfx_color = 1U;
    vlSelfRef.top__DOT__vgc_inst__DOT__mode = 0U;
    vlSelfRef.top__DOT__vgc_inst__DOT__cursor_enable = 1U;
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
    vlSelfRef.top__DOT__vgc_inst__DOT__scroll_offset = 0U;
    vlSelfRef.top__DOT__vgc_inst__DOT__scroll_pending = 0U;
    vlSelfRef.top__DOT__vgc_inst__DOT__scroll_col = 0U;
    vlSelfRef.top__DOT__vgc_inst__DOT__char_in_reg = 0U;
    vlSelfRef.top__DOT__vgc_inst__DOT__cmd_busy = 0U;
    vlSelfRef.top__DOT__vgc_inst__DOT__font_slot = 0U;
    vlSelfRef.top__DOT__vgc_inst__DOT__sprrow_count = 0U;
    vlSelfRef.top__DOT__vgc_inst__DOT__sprcopy_phase = 0U;
    vlSelfRef.top__DOT__vgc_inst__DOT__memread_pending = 0U;
    vlSelfRef.top__DOT__vgc_inst__DOT__vgc_tile_addr = 0U;
    vlSelfRef.top__DOT__vgc_inst__DOT__vgc_tile_wdata = 0U;
    vlSelfRef.top__DOT__vgc_inst__DOT__vgc_tile_we = 0U;
    vlSelfRef.top__DOT__vgc_inst__DOT__vgc_tile_re = 0U;
    vlSelfRef.top__DOT__vgc_inst__DOT__artist_cmd_valid = 0U;
    top__DOT__vgc_inst__DOT__text_inst__DOT__char_mem__DOT__gen_init_zero__DOT__i = 0U;
    while (VL_GTS_III(32, 0x000007d0U, top__DOT__vgc_inst__DOT__text_inst__DOT__char_mem__DOT__gen_init_zero__DOT__i)) {
        vlSelfRef.top__DOT__vgc_inst__DOT__text_inst__DOT__char_mem__DOT____Vlvbound_h3482bd38__0 = 0U;
        if (VL_LIKELY(((0x07cfU >= (0x000007ffU & top__DOT__vgc_inst__DOT__text_inst__DOT__char_mem__DOT__gen_init_zero__DOT__i))))) {
            vlSelfRef.top__DOT__vgc_inst__DOT__text_inst__DOT__char_mem__DOT__mem[(0x000007ffU 
                                                                                & top__DOT__vgc_inst__DOT__text_inst__DOT__char_mem__DOT__gen_init_zero__DOT__i)] 
                = vlSelfRef.top__DOT__vgc_inst__DOT__text_inst__DOT__char_mem__DOT____Vlvbound_h3482bd38__0;
        }
        top__DOT__vgc_inst__DOT__text_inst__DOT__char_mem__DOT__gen_init_zero__DOT__i 
            = ((IData)(1U) + top__DOT__vgc_inst__DOT__text_inst__DOT__char_mem__DOT__gen_init_zero__DOT__i);
    }
    top__DOT__vgc_inst__DOT__text_inst__DOT__color_mem__DOT__gen_init_zero__DOT__i = 0U;
    while (VL_GTS_III(32, 0x000007d0U, top__DOT__vgc_inst__DOT__text_inst__DOT__color_mem__DOT__gen_init_zero__DOT__i)) {
        vlSelfRef.top__DOT__vgc_inst__DOT__text_inst__DOT__color_mem__DOT____Vlvbound_h3482bd38__0 = 0U;
        if (VL_LIKELY(((0x07cfU >= (0x000007ffU & top__DOT__vgc_inst__DOT__text_inst__DOT__color_mem__DOT__gen_init_zero__DOT__i))))) {
            vlSelfRef.top__DOT__vgc_inst__DOT__text_inst__DOT__color_mem__DOT__mem[(0x000007ffU 
                                                                                & top__DOT__vgc_inst__DOT__text_inst__DOT__color_mem__DOT__gen_init_zero__DOT__i)] 
                = vlSelfRef.top__DOT__vgc_inst__DOT__text_inst__DOT__color_mem__DOT____Vlvbound_h3482bd38__0;
        }
        top__DOT__vgc_inst__DOT__text_inst__DOT__color_mem__DOT__gen_init_zero__DOT__i 
            = ((IData)(1U) + top__DOT__vgc_inst__DOT__text_inst__DOT__color_mem__DOT__gen_init_zero__DOT__i);
    }
    VL_READMEM_N(true, 8, 2048, 0, "rom/cp437.hex"s
                 ,  &(vlSelfRef.top__DOT__vgc_inst__DOT__text_inst__DOT__font_mem__DOT__mem)
                 , 0, ~0ULL);
    top__DOT__vgc_inst__DOT__gfx_inst__DOT__gfx_mem__DOT__gen_init_zero__DOT__i = 0U;
    while (VL_GTS_III(32, 0x0000fa00U, top__DOT__vgc_inst__DOT__gfx_inst__DOT__gfx_mem__DOT__gen_init_zero__DOT__i)) {
        vlSelfRef.top__DOT__vgc_inst__DOT__gfx_inst__DOT__gfx_mem__DOT____Vlvbound_h75f189da__0 = 0U;
        if (VL_LIKELY(((0xf9ffU >= (0x0000ffffU & top__DOT__vgc_inst__DOT__gfx_inst__DOT__gfx_mem__DOT__gen_init_zero__DOT__i))))) {
            vlSelfRef.top__DOT__vgc_inst__DOT__gfx_inst__DOT__gfx_mem__DOT__mem[(0x0000ffffU 
                                                                                & top__DOT__vgc_inst__DOT__gfx_inst__DOT__gfx_mem__DOT__gen_init_zero__DOT__i)] 
                = vlSelfRef.top__DOT__vgc_inst__DOT__gfx_inst__DOT__gfx_mem__DOT____Vlvbound_h75f189da__0;
        }
        top__DOT__vgc_inst__DOT__gfx_inst__DOT__gfx_mem__DOT__gen_init_zero__DOT__i 
            = ((IData)(1U) + top__DOT__vgc_inst__DOT__gfx_inst__DOT__gfx_mem__DOT__gen_init_zero__DOT__i);
    }
    vlSelfRef.top__DOT__vgc_inst__DOT__artist_busy = 0U;
    vlSelfRef.top__DOT__vgc_inst__DOT__artist_inst__DOT__op = 0U;
    vlSelfRef.top__DOT__vgc_inst__DOT__artist_inst__DOT__fill_sp = 0U;
    vlSelfRef.top__DOT__vgc_inst__DOT__artist_inst__DOT__octant_count = 0U;
    vlSelfRef.top__DOT__vgc_inst__DOT__artist_inst__DOT__paint_phase = 0U;
    vlSelfRef.top__DOT__vgc_inst__DOT__artist_inst__DOT__gt_font_pending = 0U;
    vlSelfRef.top__DOT__vgc_inst__DOT__artist_inst__DOT__paint_init = 0U;
    top__DOT__vgc_inst__DOT__artist_inst__DOT__fill_stack_mem__DOT__gen_init_zero__DOT__i = 0U;
    while (VL_GTS_III(32, 0x00000200U, top__DOT__vgc_inst__DOT__artist_inst__DOT__fill_stack_mem__DOT__gen_init_zero__DOT__i)) {
        vlSelfRef.top__DOT__vgc_inst__DOT__artist_inst__DOT__fill_stack_mem__DOT__mem[(0x000001ffU 
                                                                                & top__DOT__vgc_inst__DOT__artist_inst__DOT__fill_stack_mem__DOT__gen_init_zero__DOT__i)] = 0U;
        top__DOT__vgc_inst__DOT__artist_inst__DOT__fill_stack_mem__DOT__gen_init_zero__DOT__i 
            = ((IData)(1U) + top__DOT__vgc_inst__DOT__artist_inst__DOT__fill_stack_mem__DOT__gen_init_zero__DOT__i);
    }
    vlSelfRef.top__DOT__vgc_inst__DOT__sprite_inst__DOT__spr_eval_state = 0U;
    vlSelfRef.top__DOT__vgc_inst__DOT__sprite_inst__DOT__spr_eval_idx = 0U;
    vlSelfRef.top__DOT__vgc_inst__DOT__sprite_inst__DOT__spr_clear_x = 0U;
    top__DOT__vgc_inst__DOT__sprite_inst__DOT__spr_mem__DOT__gen_init_zero__DOT__i = 0U;
    while (VL_GTS_III(32, 0x00000800U, top__DOT__vgc_inst__DOT__sprite_inst__DOT__spr_mem__DOT__gen_init_zero__DOT__i)) {
        vlSelfRef.top__DOT__vgc_inst__DOT__sprite_inst__DOT__spr_mem__DOT__mem[(0x000007ffU 
                                                                                & top__DOT__vgc_inst__DOT__sprite_inst__DOT__spr_mem__DOT__gen_init_zero__DOT__i)] = 0U;
        top__DOT__vgc_inst__DOT__sprite_inst__DOT__spr_mem__DOT__gen_init_zero__DOT__i 
            = ((IData)(1U) + top__DOT__vgc_inst__DOT__sprite_inst__DOT__spr_mem__DOT__gen_init_zero__DOT__i);
    }
    top__DOT__vgc_inst__DOT__sprite_inst__DOT__scanline_buf__DOT__gen_init_zero__DOT__i = 0U;
    while (VL_GTS_III(32, 0x00000140U, top__DOT__vgc_inst__DOT__sprite_inst__DOT__scanline_buf__DOT__gen_init_zero__DOT__i)) {
        vlSelfRef.top__DOT__vgc_inst__DOT__sprite_inst__DOT__scanline_buf__DOT____Vlvbound_he4591f03__0 = 0U;
        if (VL_LIKELY(((0x013fU >= (0x000001ffU & top__DOT__vgc_inst__DOT__sprite_inst__DOT__scanline_buf__DOT__gen_init_zero__DOT__i))))) {
            vlSelfRef.top__DOT__vgc_inst__DOT__sprite_inst__DOT__scanline_buf__DOT__mem[(0x000001ffU 
                                                                                & top__DOT__vgc_inst__DOT__sprite_inst__DOT__scanline_buf__DOT__gen_init_zero__DOT__i)] 
                = vlSelfRef.top__DOT__vgc_inst__DOT__sprite_inst__DOT__scanline_buf__DOT____Vlvbound_he4591f03__0;
        }
        top__DOT__vgc_inst__DOT__sprite_inst__DOT__scanline_buf__DOT__gen_init_zero__DOT__i 
            = ((IData)(1U) + top__DOT__vgc_inst__DOT__sprite_inst__DOT__scanline_buf__DOT__gen_init_zero__DOT__i);
    }
    vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__tregs[0U] = 0U;
    vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__tregs[1U] = 0U;
    vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__tregs[2U] = 0U;
    vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__tregs[3U] = 0U;
    vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__tregs[4U] = 0U;
    vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__tregs[5U] = 0U;
    vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__tregs[6U] = 0U;
    vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__tregs[7U] = 0U;
    vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__tregs[8U] = 0U;
    vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__tregs[9U] = 0U;
    vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__tregs[10U] = 0U;
    vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__tregs[11U] = 0U;
    vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__tregs[12U] = 0U;
    vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__tregs[13U] = 0U;
    vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__tregs[14U] = 0U;
    vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__tregs[15U] = 0U;
    vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__tregs[16U] = 0U;
    vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__tregs[17U] = 0U;
    vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__tregs[18U] = 0U;
    vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__tregs[19U] = 0U;
    vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__tregs[20U] = 0U;
    vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__tregs[21U] = 0U;
    vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__tregs[22U] = 0U;
    vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__tregs[23U] = 0U;
    vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__tregs[24U] = 0U;
    vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__tregs[25U] = 0U;
    vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__tregs[26U] = 0U;
    vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__tregs[27U] = 0U;
    vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__tregs[28U] = 0U;
    vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__tregs[29U] = 0U;
    vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__tregs[30U] = 0U;
    vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__tregs[31U] = 0U;
    vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__col_buffer[0U] = 0U;
    vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__col_buffer[1U] = 0U;
    vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__col_buffer[2U] = 0U;
    vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__col_buffer[3U] = 0U;
    vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__col_buffer[4U] = 0U;
    vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__col_buffer[5U] = 0U;
    vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__col_buffer[6U] = 0U;
    vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__col_buffer[7U] = 0U;
    vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__col_buffer[8U] = 0U;
    vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__col_buffer[9U] = 0U;
    vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__col_buffer[10U] = 0U;
    vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__col_buffer[11U] = 0U;
    vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__col_buffer[12U] = 0U;
    vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__col_buffer[13U] = 0U;
    vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__col_buffer[14U] = 0U;
    vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__col_buffer[15U] = 0U;
    vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__col_buffer[16U] = 0U;
    vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__col_buffer[17U] = 0U;
    vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__col_buffer[18U] = 0U;
    vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__col_buffer[19U] = 0U;
    vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__col_buffer[20U] = 0U;
    vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__col_buffer[21U] = 0U;
    vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__col_buffer[22U] = 0U;
    vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__col_buffer[23U] = 0U;
    vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__col_buffer[24U] = 0U;
    vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__tile_size16 = 0U;
    vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__mirror_mode = 0U;
    vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__trans_color = 0U;
    vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__scroll_x = 0U;
    vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__scroll_y = 0U;
    vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__dma_state = 0U;
    vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__dma_src = 0U;
    vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__dma_count = 0U;
    vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__dma_dst = 0U;
    vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__dma_nt = 0U;
    vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__dma_col_row = 0U;
    vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__dma_col_idx = 0U;
    vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__dma_data_valid = 0U;
    vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__dma_pal_phase = 0U;
    vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__dma_pal_r = 0U;
    vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__dma_pal_g = 0U;
    vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__dma_pal_sub = 0U;
    vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__dma_pal_cidx = 0U;
    vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__dma_fill_val = 0U;
    vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__dma_fill_addr = 0U;
    top__DOT__vgc_inst__DOT__tile_inst__DOT__tile_data_ram__DOT__gen_init_zero__DOT__i = 0U;
    while (VL_GTS_III(32, 0x00008000U, top__DOT__vgc_inst__DOT__tile_inst__DOT__tile_data_ram__DOT__gen_init_zero__DOT__i)) {
        vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__tile_data_ram__DOT__mem[(0x00007fffU 
                                                                                & top__DOT__vgc_inst__DOT__tile_inst__DOT__tile_data_ram__DOT__gen_init_zero__DOT__i)] = 0U;
        top__DOT__vgc_inst__DOT__tile_inst__DOT__tile_data_ram__DOT__gen_init_zero__DOT__i 
            = ((IData)(1U) + top__DOT__vgc_inst__DOT__tile_inst__DOT__tile_data_ram__DOT__gen_init_zero__DOT__i);
    }
    top__DOT__vgc_inst__DOT__tile_inst__DOT__nametable_ram__DOT__gen_init_zero__DOT__i = 0U;
    while (VL_GTS_III(32, 0x00001000U, top__DOT__vgc_inst__DOT__tile_inst__DOT__nametable_ram__DOT__gen_init_zero__DOT__i)) {
        vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__nametable_ram__DOT__mem[(0x00000fffU 
                                                                                & top__DOT__vgc_inst__DOT__tile_inst__DOT__nametable_ram__DOT__gen_init_zero__DOT__i)] = 0U;
        top__DOT__vgc_inst__DOT__tile_inst__DOT__nametable_ram__DOT__gen_init_zero__DOT__i 
            = ((IData)(1U) + top__DOT__vgc_inst__DOT__tile_inst__DOT__nametable_ram__DOT__gen_init_zero__DOT__i);
    }
    top__DOT__vgc_inst__DOT__tile_inst__DOT__attr_table_ram__DOT__gen_init_zero__DOT__i = 0U;
    while (VL_GTS_III(32, 0x00001000U, top__DOT__vgc_inst__DOT__tile_inst__DOT__attr_table_ram__DOT__gen_init_zero__DOT__i)) {
        vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__attr_table_ram__DOT__mem[(0x00000fffU 
                                                                                & top__DOT__vgc_inst__DOT__tile_inst__DOT__attr_table_ram__DOT__gen_init_zero__DOT__i)] = 0U;
        top__DOT__vgc_inst__DOT__tile_inst__DOT__attr_table_ram__DOT__gen_init_zero__DOT__i 
            = ((IData)(1U) + top__DOT__vgc_inst__DOT__tile_inst__DOT__attr_table_ram__DOT__gen_init_zero__DOT__i);
    }
    VL_READMEM_N(true, 12, 256, 0, "rom/tile_pal0.hex"s
                 ,  &(vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__pal_ram_inst__DOT__mem)
                 , 0, ~0ULL);
}

VL_ATTR_COLD void Vtop___024root___eval_final(Vtop___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vtop___024root___eval_final\n"); );
    Vtop__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
}

#ifdef VL_DEBUG
VL_ATTR_COLD void Vtop___024root___dump_triggers__stl(const VlUnpacked<QData/*63:0*/, 2> &triggers, const std::string &tag);
#endif  // VL_DEBUG
VL_ATTR_COLD bool Vtop___024root___eval_phase__stl(Vtop___024root* vlSelf);

VL_ATTR_COLD void Vtop___024root___eval_settle(Vtop___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vtop___024root___eval_settle\n"); );
    Vtop__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
    // Locals
    IData/*31:0*/ __VstlIterCount;
    // Body
    __VstlIterCount = 0U;
    vlSelfRef.__VstlFirstIteration = 1U;
    do {
        if (VL_UNLIKELY(((0x00000064U < __VstlIterCount)))) {
#ifdef VL_DEBUG
            Vtop___024root___dump_triggers__stl(vlSelfRef.__VstlTriggered, "stl"s);
#endif
            VL_FATAL_MT("rtl/top.sv", 9, "", "DIDNOTCONVERGE: Settle region did not converge after '--converge-limit' of 100 tries");
        }
        __VstlIterCount = ((IData)(1U) + __VstlIterCount);
        vlSelfRef.__VstlPhaseResult = Vtop___024root___eval_phase__stl(vlSelf);
        vlSelfRef.__VstlFirstIteration = 0U;
    } while (vlSelfRef.__VstlPhaseResult);
}

VL_ATTR_COLD void Vtop___024root___eval_triggers_vec__stl(Vtop___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vtop___024root___eval_triggers_vec__stl\n"); );
    Vtop__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
    // Body
    vlSelfRef.__VstlTriggered[1U] = ((0xfffffffffffffffeULL 
                                      & vlSelfRef.__VstlTriggered[1U]) 
                                     | (IData)((IData)(vlSelfRef.__VstlFirstIteration)));
    vlSelfRef.__VstlTriggered[0U] = (QData)((IData)(
                                                    (((((vlSelfRef.__Vtrigprevexpr___TOP__top__DOT__sid2_inst____PVT__v3__DOT__envelope_dac__DOT__bitsum__0.neq(vlSymsp->TOP__top__DOT__sid2_inst.__PVT__v3__DOT__envelope_dac__DOT__bitsum) 
                                                         << 3U) 
                                                        | (vlSelfRef.__Vtrigprevexpr___TOP__top__DOT__sid2_inst____PVT__v3__DOT__waveform_dac__DOT__bitsum__0.neq(vlSymsp->TOP__top__DOT__sid2_inst.__PVT__v3__DOT__waveform_dac__DOT__bitsum) 
                                                           << 2U)) 
                                                       | ((vlSelfRef.__Vtrigprevexpr___TOP__top__DOT__sid2_inst____PVT__v2__DOT__envelope_dac__DOT__bitsum__0.neq(vlSymsp->TOP__top__DOT__sid2_inst.__PVT__v2__DOT__envelope_dac__DOT__bitsum) 
                                                           << 1U) 
                                                          | vlSelfRef.__Vtrigprevexpr___TOP__top__DOT__sid2_inst____PVT__v2__DOT__waveform_dac__DOT__bitsum__0.neq(vlSymsp->TOP__top__DOT__sid2_inst.__PVT__v2__DOT__waveform_dac__DOT__bitsum))) 
                                                      << 8U) 
                                                     | (((((vlSelfRef.__Vtrigprevexpr___TOP__top__DOT__sid2_inst____PVT__v1__DOT__envelope_dac__DOT__bitsum__0.neq(vlSymsp->TOP__top__DOT__sid2_inst.__PVT__v1__DOT__envelope_dac__DOT__bitsum) 
                                                            << 3U) 
                                                           | (vlSelfRef.__Vtrigprevexpr___TOP__top__DOT__sid2_inst____PVT__v1__DOT__waveform_dac__DOT__bitsum__0.neq(vlSymsp->TOP__top__DOT__sid2_inst.__PVT__v1__DOT__waveform_dac__DOT__bitsum) 
                                                              << 2U)) 
                                                          | ((vlSelfRef.__Vtrigprevexpr___TOP__top__DOT__sid1_inst____PVT__v3__DOT__envelope_dac__DOT__bitsum__0.neq(vlSymsp->TOP__top__DOT__sid1_inst.__PVT__v3__DOT__envelope_dac__DOT__bitsum) 
                                                              << 1U) 
                                                             | vlSelfRef.__Vtrigprevexpr___TOP__top__DOT__sid1_inst____PVT__v3__DOT__waveform_dac__DOT__bitsum__0.neq(vlSymsp->TOP__top__DOT__sid1_inst.__PVT__v3__DOT__waveform_dac__DOT__bitsum))) 
                                                         << 4U) 
                                                        | (((vlSelfRef.__Vtrigprevexpr___TOP__top__DOT__sid1_inst____PVT__v2__DOT__envelope_dac__DOT__bitsum__0.neq(vlSymsp->TOP__top__DOT__sid1_inst.__PVT__v2__DOT__envelope_dac__DOT__bitsum) 
                                                             << 3U) 
                                                            | (vlSelfRef.__Vtrigprevexpr___TOP__top__DOT__sid1_inst____PVT__v2__DOT__waveform_dac__DOT__bitsum__0.neq(vlSymsp->TOP__top__DOT__sid1_inst.__PVT__v2__DOT__waveform_dac__DOT__bitsum) 
                                                               << 2U)) 
                                                           | ((vlSelfRef.__Vtrigprevexpr___TOP__top__DOT__sid1_inst____PVT__v1__DOT__envelope_dac__DOT__bitsum__0.neq(vlSymsp->TOP__top__DOT__sid1_inst.__PVT__v1__DOT__envelope_dac__DOT__bitsum) 
                                                               << 1U) 
                                                              | vlSelfRef.__Vtrigprevexpr___TOP__top__DOT__sid1_inst____PVT__v1__DOT__waveform_dac__DOT__bitsum__0.neq(vlSymsp->TOP__top__DOT__sid1_inst.__PVT__v1__DOT__waveform_dac__DOT__bitsum)))))));
    vlSelfRef.__Vtrigprevexpr___TOP__top__DOT__sid1_inst____PVT__v1__DOT__waveform_dac__DOT__bitsum__0.assign(vlSymsp->TOP__top__DOT__sid1_inst.__PVT__v1__DOT__waveform_dac__DOT__bitsum);
    vlSelfRef.__Vtrigprevexpr___TOP__top__DOT__sid1_inst____PVT__v1__DOT__envelope_dac__DOT__bitsum__0.assign(vlSymsp->TOP__top__DOT__sid1_inst.__PVT__v1__DOT__envelope_dac__DOT__bitsum);
    vlSelfRef.__Vtrigprevexpr___TOP__top__DOT__sid1_inst____PVT__v2__DOT__waveform_dac__DOT__bitsum__0.assign(vlSymsp->TOP__top__DOT__sid1_inst.__PVT__v2__DOT__waveform_dac__DOT__bitsum);
    vlSelfRef.__Vtrigprevexpr___TOP__top__DOT__sid1_inst____PVT__v2__DOT__envelope_dac__DOT__bitsum__0.assign(vlSymsp->TOP__top__DOT__sid1_inst.__PVT__v2__DOT__envelope_dac__DOT__bitsum);
    vlSelfRef.__Vtrigprevexpr___TOP__top__DOT__sid1_inst____PVT__v3__DOT__waveform_dac__DOT__bitsum__0.assign(vlSymsp->TOP__top__DOT__sid1_inst.__PVT__v3__DOT__waveform_dac__DOT__bitsum);
    vlSelfRef.__Vtrigprevexpr___TOP__top__DOT__sid1_inst____PVT__v3__DOT__envelope_dac__DOT__bitsum__0.assign(vlSymsp->TOP__top__DOT__sid1_inst.__PVT__v3__DOT__envelope_dac__DOT__bitsum);
    vlSelfRef.__Vtrigprevexpr___TOP__top__DOT__sid2_inst____PVT__v1__DOT__waveform_dac__DOT__bitsum__0.assign(vlSymsp->TOP__top__DOT__sid2_inst.__PVT__v1__DOT__waveform_dac__DOT__bitsum);
    vlSelfRef.__Vtrigprevexpr___TOP__top__DOT__sid2_inst____PVT__v1__DOT__envelope_dac__DOT__bitsum__0.assign(vlSymsp->TOP__top__DOT__sid2_inst.__PVT__v1__DOT__envelope_dac__DOT__bitsum);
    vlSelfRef.__Vtrigprevexpr___TOP__top__DOT__sid2_inst____PVT__v2__DOT__waveform_dac__DOT__bitsum__0.assign(vlSymsp->TOP__top__DOT__sid2_inst.__PVT__v2__DOT__waveform_dac__DOT__bitsum);
    vlSelfRef.__Vtrigprevexpr___TOP__top__DOT__sid2_inst____PVT__v2__DOT__envelope_dac__DOT__bitsum__0.assign(vlSymsp->TOP__top__DOT__sid2_inst.__PVT__v2__DOT__envelope_dac__DOT__bitsum);
    vlSelfRef.__Vtrigprevexpr___TOP__top__DOT__sid2_inst____PVT__v3__DOT__waveform_dac__DOT__bitsum__0.assign(vlSymsp->TOP__top__DOT__sid2_inst.__PVT__v3__DOT__waveform_dac__DOT__bitsum);
    vlSelfRef.__Vtrigprevexpr___TOP__top__DOT__sid2_inst____PVT__v3__DOT__envelope_dac__DOT__bitsum__0.assign(vlSymsp->TOP__top__DOT__sid2_inst.__PVT__v3__DOT__envelope_dac__DOT__bitsum);
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
        vlSelfRef.__VstlTriggered[0U] = (0x0000000000000040ULL 
                                         | vlSelfRef.__VstlTriggered[0U]);
        vlSelfRef.__VstlTriggered[0U] = (0x0000000000000080ULL 
                                         | vlSelfRef.__VstlTriggered[0U]);
        vlSelfRef.__VstlTriggered[0U] = (0x0000000000000100ULL 
                                         | vlSelfRef.__VstlTriggered[0U]);
        vlSelfRef.__VstlTriggered[0U] = (0x0000000000000200ULL 
                                         | vlSelfRef.__VstlTriggered[0U]);
        vlSelfRef.__VstlTriggered[0U] = (0x0000000000000400ULL 
                                         | vlSelfRef.__VstlTriggered[0U]);
        vlSelfRef.__VstlTriggered[0U] = (0x0000000000000800ULL 
                                         | vlSelfRef.__VstlTriggered[0U]);
    }
}

VL_ATTR_COLD bool Vtop___024root___trigger_anySet__stl(const VlUnpacked<QData/*63:0*/, 2> &in);

#ifdef VL_DEBUG
VL_ATTR_COLD void Vtop___024root___dump_triggers__stl(const VlUnpacked<QData/*63:0*/, 2> &triggers, const std::string &tag) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vtop___024root___dump_triggers__stl\n"); );
    // Body
    if ((1U & (~ (IData)(Vtop___024root___trigger_anySet__stl(triggers))))) {
        VL_DBG_MSGS("         No '" + tag + "' region triggers active\n");
    }
    if ((1U & (IData)(triggers[0U]))) {
        VL_DBG_MSGS("         '" + tag + "' region trigger index 0 is active: @([hybrid] top.sid1_inst.v1.waveform_dac.bitsum)\n");
    }
    if ((1U & (IData)((triggers[0U] >> 1U)))) {
        VL_DBG_MSGS("         '" + tag + "' region trigger index 1 is active: @([hybrid] top.sid1_inst.v1.envelope_dac.bitsum)\n");
    }
    if ((1U & (IData)((triggers[0U] >> 2U)))) {
        VL_DBG_MSGS("         '" + tag + "' region trigger index 2 is active: @([hybrid] top.sid1_inst.v2.waveform_dac.bitsum)\n");
    }
    if ((1U & (IData)((triggers[0U] >> 3U)))) {
        VL_DBG_MSGS("         '" + tag + "' region trigger index 3 is active: @([hybrid] top.sid1_inst.v2.envelope_dac.bitsum)\n");
    }
    if ((1U & (IData)((triggers[0U] >> 4U)))) {
        VL_DBG_MSGS("         '" + tag + "' region trigger index 4 is active: @([hybrid] top.sid1_inst.v3.waveform_dac.bitsum)\n");
    }
    if ((1U & (IData)((triggers[0U] >> 5U)))) {
        VL_DBG_MSGS("         '" + tag + "' region trigger index 5 is active: @([hybrid] top.sid1_inst.v3.envelope_dac.bitsum)\n");
    }
    if ((1U & (IData)((triggers[0U] >> 6U)))) {
        VL_DBG_MSGS("         '" + tag + "' region trigger index 6 is active: @([hybrid] top.sid2_inst.v1.waveform_dac.bitsum)\n");
    }
    if ((1U & (IData)((triggers[0U] >> 7U)))) {
        VL_DBG_MSGS("         '" + tag + "' region trigger index 7 is active: @([hybrid] top.sid2_inst.v1.envelope_dac.bitsum)\n");
    }
    if ((1U & (IData)((triggers[0U] >> 8U)))) {
        VL_DBG_MSGS("         '" + tag + "' region trigger index 8 is active: @([hybrid] top.sid2_inst.v2.waveform_dac.bitsum)\n");
    }
    if ((1U & (IData)((triggers[0U] >> 9U)))) {
        VL_DBG_MSGS("         '" + tag + "' region trigger index 9 is active: @([hybrid] top.sid2_inst.v2.envelope_dac.bitsum)\n");
    }
    if ((1U & (IData)((triggers[0U] >> 0x0000000aU)))) {
        VL_DBG_MSGS("         '" + tag + "' region trigger index 10 is active: @([hybrid] top.sid2_inst.v3.waveform_dac.bitsum)\n");
    }
    if ((1U & (IData)((triggers[0U] >> 0x0000000bU)))) {
        VL_DBG_MSGS("         '" + tag + "' region trigger index 11 is active: @([hybrid] top.sid2_inst.v3.envelope_dac.bitsum)\n");
    }
    if ((1U & (IData)(triggers[1U]))) {
        VL_DBG_MSGS("         '" + tag + "' region trigger index 64 is active: Internal 'stl' trigger - first iteration\n");
    }
}
#endif  // VL_DEBUG

VL_ATTR_COLD bool Vtop___024root___trigger_anySet__stl(const VlUnpacked<QData/*63:0*/, 2> &in) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vtop___024root___trigger_anySet__stl\n"); );
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

extern const VlUnpacked<CData/*0:0*/, 256> Vtop__ConstPool__TABLE_h67de06b7_0;
extern const VlUnpacked<CData/*0:0*/, 128> Vtop__ConstPool__TABLE_hdc38b0ff_0;
extern const VlUnpacked<CData/*0:0*/, 2048> Vtop__ConstPool__TABLE_h32401d32_0;
extern const VlUnpacked<CData/*0:0*/, 128> Vtop__ConstPool__TABLE_h30aa0085_0;
extern const VlUnpacked<CData/*0:0*/, 8192> Vtop__ConstPool__TABLE_hc7a084b0_0;
extern const VlUnpacked<CData/*3:0*/, 2048> Vtop__ConstPool__TABLE_hf67012e5_0;
extern const VlUnpacked<CData/*1:0*/, 2048> Vtop__ConstPool__TABLE_hd7a914d5_0;

VL_ATTR_COLD void Vtop___024root___stl_sequent__TOP__0(Vtop___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vtop___024root___stl_sequent__TOP__0\n"); );
    Vtop__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
    // Locals
    CData/*7:0*/ top__DOT__blt_vgc_wdata;
    top__DOT__blt_vgc_wdata = 0;
    CData/*5:0*/ top__DOT____VdfgRegularize_hbded3b40_0_0;
    top__DOT____VdfgRegularize_hbded3b40_0_0 = 0;
    IData/*23:0*/ top__DOT__blt_inst__DOT__src_addr;
    top__DOT__blt_inst__DOT__src_addr = 0;
    IData/*23:0*/ top__DOT__blt_inst__DOT__dst_addr;
    top__DOT__blt_inst__DOT__dst_addr = 0;
    CData/*7:0*/ top__DOT__cpu_inst__DOT__BI;
    top__DOT__cpu_inst__DOT__BI = 0;
    SData/*8:0*/ top__DOT__cpu_inst__DOT__ALU__DOT__temp_logic;
    top__DOT__cpu_inst__DOT__ALU__DOT__temp_logic = 0;
    CData/*4:0*/ top__DOT__cpu_inst__DOT__ALU__DOT__temp_l;
    top__DOT__cpu_inst__DOT__ALU__DOT__temp_l = 0;
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
    CData/*6:0*/ __Vtableidx7;
    __Vtableidx7 = 0;
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
    CData/*1:0*/ __VdfgRegularize_hebeb780c_0_23;
    __VdfgRegularize_hebeb780c_0_23 = 0;
    // Body
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
    vlSelfRef.dbg_cpu_pc = vlSelfRef.top__DOT__cpu_inst__DOT__PC;
    vlSelfRef.dbg_cpu_a = vlSelfRef.top__DOT__cpu_inst__DOT__AXYS[0U];
    vlSelfRef.dbg_cpu_x = vlSelfRef.top__DOT__cpu_inst__DOT__AXYS[1U];
    vlSelfRef.dbg_cpu_y = vlSelfRef.top__DOT__cpu_inst__DOT__AXYS[2U];
    vlSelfRef.dbg_cpu_sp = vlSelfRef.top__DOT__cpu_inst__DOT__AXYS[3U];
    vlSelfRef.audio_l = vlSymsp->TOP__top__DOT__sid1_inst.audio_out;
    vlSelfRef.audio_r = vlSymsp->TOP__top__DOT__sid2_inst.audio_out;
    vlSelfRef.top__DOT__vgc_inst__DOT____Vcellinp__artist_inst__cmd_x0 
        = ((0x00000100U & (vlSelfRef.top__DOT__vgc_inst__DOT__regs[18U] 
                           << 8U)) | vlSelfRef.top__DOT__vgc_inst__DOT__regs[17U]);
    vlSelfRef.top__DOT__vgc_inst__DOT____Vcellinp__artist_inst__cmd_x1 
        = ((0x00000100U & (vlSelfRef.top__DOT__vgc_inst__DOT__regs[22U] 
                           << 8U)) | vlSelfRef.top__DOT__vgc_inst__DOT__regs[21U]);
    vlSelfRef.top__DOT__vgc_inst__DOT__font_a_addr 
        = ((IData)(vlSelfRef.top__DOT__vgc_inst__DOT__artist_font_re)
            ? (IData)(vlSelfRef.top__DOT__vgc_inst__DOT__artist_font_addr)
            : 0U);
    vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__s2_color_idx 
        = (0x0000000fU & ((IData)(vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__s2_sample_x_lsb)
                           ? (IData)(vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__td_dout_b)
                           : ((IData)(vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__td_dout_b) 
                              >> 4U)));
    vlSelfRef.top__DOT__vgc_inst__DOT__tile_blt_rdata 
        = ((IData)(vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__blt_tile_rd_pending)
            ? (IData)(vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__td_dout_a)
            : (IData)(vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__blt_tile_rd_latch));
    vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__s1_sample_x 
        = (0x0000000fU & ((0x00000040U & (IData)(vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__at_dout_b))
                           ? ((IData)(vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__s1_tile_w) 
                              - (IData)(vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__s1_pix_in_x))
                           : (IData)(vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__s1_pix_in_x)));
    vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__s1_sample_y 
        = (0x0000000fU & ((0x00000080U & (IData)(vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__at_dout_b))
                           ? ((IData)(vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__s1_tile_w) 
                              - (IData)(vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__s1_pix_in_y))
                           : (IData)(vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__s1_pix_in_y)));
    vlSelfRef.__VdfgRegularize_hebeb780c_0_25 = ((IData)(vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__s3_pixel_valid) 
                                                 & ((IData)(vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__s3_color_idx) 
                                                    != (IData)(vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__s3_trans_color)));
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
    vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__at_we_a = 0U;
    vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__pr_we_a = 0U;
    __Vtableidx3 = (((IData)(vlSelfRef.top__DOT__cpu_inst__DOT__plp) 
                     << 7U) | (((IData)(vlSelfRef.top__DOT__cpu_inst__DOT__load_reg) 
                                << 6U) | (IData)(vlSelfRef.top__DOT__cpu_inst__DOT__state)));
    vlSelfRef.top__DOT__cpu_inst__DOT__write_register 
        = Vtop__ConstPool__TABLE_h67de06b7_0[__Vtableidx3];
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
    vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__nt_we_a = 0U;
    vlSelfRef.top__DOT__vgc_inst__DOT__visible = ((0x0280U 
                                                   > (IData)(vlSelfRef.top__DOT__vgc_inst__DOT__h_count)) 
                                                  & (0x01e0U 
                                                     > (IData)(vlSelfRef.top__DOT__vgc_inst__DOT__v_count)));
    vlSelfRef.top__DOT__vgc_inst__DOT__cursor_here 
        = ((IData)(vlSelfRef.top__DOT__vgc_inst__DOT__cursor_enable) 
           & ((IData)(vlSelfRef.top__DOT__vgc_inst__DOT__in_text_area_d2) 
              & (((IData)(vlSelfRef.top__DOT__vgc_inst__DOT__cursor_x) 
                  == (IData)(vlSelfRef.top__DOT__vgc_inst__DOT__text_col_d2)) 
                 & ((IData)(vlSelfRef.top__DOT__vgc_inst__DOT__cursor_y) 
                    == (IData)(vlSelfRef.top__DOT__vgc_inst__DOT__text_row_d2)))));
    vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__pr_addr_a = 0U;
    vlSelfRef.top__DOT__blt_ram_we = 0U;
    vlSelfRef.top__DOT__blt_ram_wdata = 0U;
    vlSelfRef.top__DOT__blt_xram_we = 0U;
    vlSelfRef.top__DOT__blt_xram_wdata = 0U;
    __Vtableidx7 = ((((IData)(vlSelfRef.top__DOT__cpu_inst__DOT__N) 
                      << 6U) | ((IData)(vlSelfRef.top__DOT__cpu_inst__DOT__V) 
                                << 5U)) | (((IData)(vlSelfRef.top__DOT__cpu_inst__DOT__C) 
                                            << 4U) 
                                           | (((IData)(vlSelfRef.top__DOT__cpu_inst__DOT__Z) 
                                               << 3U) 
                                              | (IData)(vlSelfRef.top__DOT__cpu_inst__DOT__cond_code))));
    vlSelfRef.top__DOT__cpu_inst__DOT__cond_true = Vtop__ConstPool__TABLE_hdc38b0ff_0
        [__Vtableidx7];
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
    vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__at_addr_a = 0U;
    vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__nt_addr_a = 0U;
    __Vtableidx2 = (((IData)(vlSelfRef.top__DOT__cpu_inst__DOT__store) 
                     << 6U) | (IData)(vlSelfRef.top__DOT__cpu_inst__DOT__state));
    vlSelfRef.top__DOT__cpu_we = Vtop__ConstPool__TABLE_h30aa0085_0
        [__Vtableidx2];
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
    vlSelfRef.top__DOT__cpu_inst__DOT____Vcellinp__ALU__BCD 
        = ((IData)(vlSelfRef.top__DOT__cpu_inst__DOT__adc_bcd) 
           & (0x0dU == (IData)(vlSelfRef.top__DOT__cpu_inst__DOT__state)));
    vlSelfRef.__VdfgRegularize_hebeb780c_0_3 = (1U 
                                                & (((~ (IData)(vlSelfRef.top__DOT__cpu_inst__DOT__I)) 
                                                    & (~ (IData)(vlSelfRef.irq_n))) 
                                                   | (IData)(vlSelfRef.top__DOT__cpu_inst__DOT__NMI_edge)));
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
    vlSelfRef.__VdfgRegularize_hebeb780c_0_19 = ((((IData)(vlSelfRef.top__DOT__cpu_inst__DOT__D) 
                                                   << 3U) 
                                                  | ((IData)(vlSelfRef.top__DOT__cpu_inst__DOT__I) 
                                                     << 2U)) 
                                                 | (((IData)(vlSelfRef.top__DOT__cpu_inst__DOT__Z) 
                                                     << 1U) 
                                                    | (IData)(vlSelfRef.top__DOT__cpu_inst__DOT__C)));
    vlSelfRef.top__DOT__tile_dma_data = vlSelfRef.top__DOT__ram
        [vlSelfRef.top__DOT__tile_dma_addr];
    vlSelfRef.top__DOT__vgc_inst__DOT__dbg_vgc_sel 
        = ((0xa000U <= (IData)(vlSelfRef.dbg_peek_addr)) 
           & (0xa01fU >= (IData)(vlSelfRef.dbg_peek_addr)));
    top__DOT__blt_vgc_wdata = 0U;
    vlSelfRef.top__DOT__vgc_inst__DOT__dbg_char_sel 
        = ((0xaa00U <= (IData)(vlSelfRef.dbg_peek_addr)) 
           & (0xb1cfU >= (IData)(vlSelfRef.dbg_peek_addr)));
    vlSelfRef.top__DOT__vgc_inst__DOT__dbg_color_sel 
        = ((0xb1d0U <= (IData)(vlSelfRef.dbg_peek_addr)) 
           & (0xb99fU >= (IData)(vlSelfRef.dbg_peek_addr)));
    vlSelfRef.top__DOT__blt_vgc_re = 0U;
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
    vlSelfRef.top__DOT__blt_vgc_we = 0U;
    vlSelfRef.__VdfgRegularize_hebeb780c_0_10 = (((IData)(vlSelfRef.top__DOT__cpu_inst__DOT__ADD) 
                                                  << 8U) 
                                                 | (IData)(vlSelfRef.top__DOT__cpu_inst__DOT__ABL));
    vlSelfRef.__VdfgRegularize_hebeb780c_0_4 = (((IData)(vlSelfRef.top__DOT__cpu_inst__DOT__ABH) 
                                                 << 8U) 
                                                | (IData)(vlSelfRef.top__DOT__cpu_inst__DOT__ABL));
    vlSelfRef.__VdfgRegularize_hebeb780c_0_5 = (((IData)(vlSelfRef.top__DOT__cpu_inst__DOT__ABH) 
                                                 << 8U) 
                                                | (IData)(vlSelfRef.top__DOT__cpu_inst__DOT__ADD));
    vlSelfRef.top__DOT__blt_vgc_space = 0U;
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
    vlSelfRef.top__DOT____Vcellinp__cpu_inst__RDY = 
        ((~ (IData)(vlSelfRef.dbg_pause)) & ((0U == (IData)(vlSelfRef.top__DOT__blt_inst__DOT__state)) 
                                             | (6U 
                                                == (IData)(vlSelfRef.top__DOT__blt_inst__DOT__state))));
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
    vlSelfRef.top__DOT__vgc_inst__DOT__in_text_area 
        = ((IData)(vlSelfRef.top__DOT__vgc_inst__DOT__visible) 
           & ((0x0028U <= (IData)(vlSelfRef.top__DOT__vgc_inst__DOT__v_count)) 
              & (0x01b8U > (IData)(vlSelfRef.top__DOT__vgc_inst__DOT__v_count))));
    vlSelfRef.top__DOT__vgc_inst__DOT__text_inst__DOT__char_b_addr 
        = (0x000007ffU & (((IData)(0x00000050U) * (IData)(vlSelfRef.top__DOT__vgc_inst__DOT__real_row)) 
                          + (0x0000007fU & ((IData)(vlSelfRef.top__DOT__vgc_inst__DOT__h_count) 
                                            >> 3U))));
    vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT____VdfgRegularize_hb33eefcd_0_3 
        = (0x000003ffU & ((0x00c8U <= (IData)(vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__world_y))
                           ? ((IData)(vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__world_y) 
                              - (IData)(0x00c8U)) : 
                          (0x000001ffU & (IData)(vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__world_y))));
    vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT____VdfgRegularize_hb33eefcd_0_2 
        = (0x000007ffU & ((0x0140U <= (0x000007ffU 
                                       & (IData)(vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT____VdfgRegularize_hb33eefcd_0_1)))
                           ? ((IData)(vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT____VdfgRegularize_hb33eefcd_0_1) 
                              - (IData)(0x0140U)) : 
                          (0x000003ffU & (IData)(vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT____VdfgRegularize_hb33eefcd_0_1))));
    __VdfgRegularize_hebeb780c_0_23 = (((0x00c8U <= (IData)(vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__world_y)) 
                                        << 1U) | (0x0140U 
                                                  <= 
                                                  (0x000007ffU 
                                                   & (IData)(vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT____VdfgRegularize_hb33eefcd_0_1))));
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
    vlSelfRef.top__DOT__cpu_inst__DOT__P = (0x00000030U 
                                            | (((IData)(vlSelfRef.top__DOT__cpu_inst__DOT__N) 
                                                << 7U) 
                                               | (((IData)(vlSelfRef.top__DOT__cpu_inst__DOT__V) 
                                                   << 6U) 
                                                  | (IData)(vlSelfRef.__VdfgRegularize_hebeb780c_0_19))));
    vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__at_din_a = 0U;
    vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__pr_din_a = 0U;
    vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__nt_din_a = 0U;
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
            }
        }
    }
    top__DOT__vgc_inst__DOT__tile_blt_wdata = ((IData)(vlSelfRef.top__DOT__vgc_inst__DOT__vgc_tile_we)
                                                ? (IData)(vlSelfRef.top__DOT__vgc_inst__DOT__vgc_tile_wdata)
                                                : (IData)(top__DOT__blt_vgc_wdata));
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
    vlSelfRef.top__DOT__blt_ram_addr = 0U;
    vlSelfRef.top__DOT__blt_xram_addr = 0U;
    if (((2U == (IData)(vlSelfRef.top__DOT__blt_inst__DOT__state)) 
         | (4U == (IData)(vlSelfRef.top__DOT__blt_inst__DOT__state)))) {
        vlSelfRef.top__DOT__blt_vgc_addr = 0U;
        if ((1U & (~ (IData)(vlSelfRef.top__DOT__blt_inst__DOT__fill_mode)))) {
            if ((0U != (IData)(vlSelfRef.top__DOT__blt_inst__DOT__src_space))) {
                if ((5U != (IData)(vlSelfRef.top__DOT__blt_inst__DOT__src_space))) {
                    vlSelfRef.top__DOT__blt_vgc_re = 1U;
                    vlSelfRef.top__DOT__blt_vgc_space 
                        = vlSelfRef.top__DOT__blt_inst__DOT__src_space;
                    vlSelfRef.top__DOT__blt_vgc_addr 
                        = (0x0000ffffU & top__DOT__blt_inst__DOT__src_addr);
                }
                if ((5U == (IData)(vlSelfRef.top__DOT__blt_inst__DOT__src_space))) {
                    vlSelfRef.top__DOT__blt_xram_addr 
                        = (0x0007ffffU & top__DOT__blt_inst__DOT__src_addr);
                }
            }
            if ((0U == (IData)(vlSelfRef.top__DOT__blt_inst__DOT__src_space))) {
                vlSelfRef.top__DOT__blt_ram_addr = 
                    (0x0000ffffU & top__DOT__blt_inst__DOT__src_addr);
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
        if ((0U != (IData)(vlSelfRef.top__DOT__blt_inst__DOT__dst_space))) {
            if ((5U != (IData)(vlSelfRef.top__DOT__blt_inst__DOT__dst_space))) {
                vlSelfRef.top__DOT__blt_vgc_we = 1U;
                vlSelfRef.top__DOT__blt_vgc_space = vlSelfRef.top__DOT__blt_inst__DOT__dst_space;
                vlSelfRef.top__DOT__blt_vgc_addr = 
                    (0x0000ffffU & top__DOT__blt_inst__DOT__dst_addr);
            }
            if ((5U == (IData)(vlSelfRef.top__DOT__blt_inst__DOT__dst_space))) {
                vlSelfRef.top__DOT__blt_xram_addr = 
                    (0x0007ffffU & top__DOT__blt_inst__DOT__dst_addr);
            }
        }
        if ((0U == (IData)(vlSelfRef.top__DOT__blt_inst__DOT__dst_space))) {
            vlSelfRef.top__DOT__blt_ram_addr = (0x0000ffffU 
                                                & top__DOT__blt_inst__DOT__dst_addr);
        }
    }
    vlSelfRef.top__DOT__vgc_inst__DOT__char_a_we = 0U;
    vlSelfRef.top__DOT__vgc_inst__DOT__color_a_we = 0U;
    vlSelfRef.top__DOT__vgc_inst__DOT__spr_a_we = 0U;
    vlSelfRef.top__DOT__vgc_inst__DOT__gfx_a_we = 0U;
    vlSelfRef.top__DOT__vgc_inst__DOT__spr_a_din = 0U;
    vlSelfRef.top__DOT__vgc_inst__DOT__char_a_din = 0U;
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
    vlSelfRef.top__DOT__vgc_inst__DOT__color_a_din = 0U;
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
    vlSelfRef.top__DOT__vgc_inst__DOT__gfx_a_din = 0U;
    vlSelfRef.top__DOT__vgc_inst__DOT__tile_blt_re 
        = ((IData)(vlSelfRef.top__DOT__vgc_inst__DOT__vgc_tile_re) 
           | ((IData)(vlSelfRef.top__DOT__blt_vgc_re) 
              & (6U == (IData)(vlSelfRef.top__DOT__blt_vgc_space))));
    vlSelfRef.top__DOT__vgc_inst__DOT__tile_blt_we 
        = ((IData)(vlSelfRef.top__DOT__vgc_inst__DOT__vgc_tile_we) 
           | ((IData)(vlSelfRef.top__DOT__blt_vgc_we) 
              & (6U == (IData)(vlSelfRef.top__DOT__blt_vgc_space))));
    vlSelfRef.top__DOT__cpu_inst__DOT__regfile = vlSelfRef.top__DOT__cpu_inst__DOT__AXYS
        [vlSelfRef.top__DOT__cpu_inst__DOT__regsel];
    vlSelfRef.top__DOT__cpu_inst__DOT__DIMUX = ((IData)(vlSelfRef.top__DOT____Vcellinp__cpu_inst__RDY)
                                                 ? (IData)(vlSelfRef.top__DOT__cpu_din)
                                                 : (IData)(vlSelfRef.top__DOT__cpu_inst__DOT__DIHOLD));
    vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__nt_addr_b 
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
                          + (((IData)(0x00000028U) 
                              * (0x0000001fU & ((IData)(vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__tile_size16)
                                                 ? 
                                                (0x0000001fU 
                                                 & ((IData)(vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT____VdfgRegularize_hb33eefcd_0_3) 
                                                    >> 4U))
                                                 : 
                                                ((IData)(vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT____VdfgRegularize_hb33eefcd_0_3) 
                                                 >> 3U)))) 
                             + (0x0000003fU & ((IData)(vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__tile_size16)
                                                ? (0x0000003fU 
                                                   & ((IData)(vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT____VdfgRegularize_hb33eefcd_0_2) 
                                                      >> 4U))
                                                : ((IData)(vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT____VdfgRegularize_hb33eefcd_0_2) 
                                                   >> 3U))))));
    vlSelfRef.dbg_cpu_flags = vlSelfRef.top__DOT__cpu_inst__DOT__P;
    vlSelfRef.top__DOT__blt_inst__DOT__mem_read_data 
        = ((0U == (IData)(vlSelfRef.top__DOT__blt_inst__DOT__src_space))
            ? vlSelfRef.top__DOT__ram[vlSelfRef.top__DOT__blt_ram_addr]
            : ((5U == (IData)(vlSelfRef.top__DOT__blt_inst__DOT__src_space))
                ? vlSelfRef.top__DOT__xram[vlSelfRef.top__DOT__blt_xram_addr]
                : (IData)(vlSelfRef.top__DOT__vgc_inst__DOT__blt_rd_latch)));
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
    top__DOT__vgc_inst__DOT__tile_blt_addr = (0x00007fffU 
                                              & (((IData)(vlSelfRef.top__DOT__vgc_inst__DOT__vgc_tile_re) 
                                                  | (IData)(vlSelfRef.top__DOT__vgc_inst__DOT__vgc_tile_we))
                                                  ? (IData)(vlSelfRef.top__DOT__vgc_inst__DOT__vgc_tile_addr)
                                                  : (IData)(vlSelfRef.top__DOT__blt_vgc_addr)));
    vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__td_we_a = 0U;
    vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__td_din_a = 0U;
    vlSelfRef.top__DOT__cpu_inst__DOT__IR = ((IData)(vlSelfRef.__VdfgRegularize_hebeb780c_0_3)
                                              ? 0U : 
                                             ((IData)(vlSelfRef.top__DOT__cpu_inst__DOT__IRHOLD_valid)
                                               ? (IData)(vlSelfRef.top__DOT__cpu_inst__DOT__IRHOLD)
                                               : (IData)(vlSelfRef.top__DOT__cpu_inst__DOT__DIMUX)));
    if ((1U & (IData)(vlSelfRef.top__DOT__cpu_inst__DOT__state))) {
        vlSelfRef.__VdfgRegularize_hebeb780c_0_11 = 
            (0x000000ffU & (IData)(vlSelfRef.top__DOT__cpu_inst__DOT__PC));
        vlSelfRef.__VdfgRegularize_hebeb780c_0_6 = vlSelfRef.top__DOT__cpu_inst__DOT__ADD;
        vlSelfRef.__VdfgRegularize_hebeb780c_0_8 = vlSelfRef.top__DOT__cpu_inst__DOT__PC;
        vlSelfRef.__VdfgRegularize_hebeb780c_0_12 = 0U;
        vlSelfRef.__VdfgRegularize_hebeb780c_0_15 = vlSelfRef.top__DOT__cpu_inst__DOT__regfile;
        vlSelfRef.__VdfgRegularize_hebeb780c_0_9 = 
            (0x00000100U | (IData)(vlSelfRef.top__DOT__cpu_inst__DOT__ADD));
        vlSelfRef.__VdfgRegularize_hebeb780c_0_13 = vlSelfRef.top__DOT__cpu_inst__DOT__ADD;
        __VdfgRegularize_hebeb780c_0_18 = 0U;
        __VdfgRegularize_hebeb780c_0_16 = vlSelfRef.top__DOT__cpu_inst__DOT__DIMUX;
    } else {
        vlSelfRef.__VdfgRegularize_hebeb780c_0_11 = 
            (0x000000ffU & ((IData)(vlSelfRef.top__DOT__cpu_inst__DOT__PC) 
                            >> 8U));
        vlSelfRef.__VdfgRegularize_hebeb780c_0_6 = vlSelfRef.top__DOT__cpu_inst__DOT__PC;
        vlSelfRef.__VdfgRegularize_hebeb780c_0_8 = 
            (0x00000100U | (IData)(vlSelfRef.top__DOT__cpu_inst__DOT__ADD));
        vlSelfRef.__VdfgRegularize_hebeb780c_0_12 = vlSelfRef.top__DOT__cpu_inst__DOT__regfile;
        vlSelfRef.__VdfgRegularize_hebeb780c_0_15 = 0U;
        vlSelfRef.__VdfgRegularize_hebeb780c_0_9 = 
            (0x00000100U | (IData)(vlSelfRef.top__DOT__cpu_inst__DOT__regfile));
        vlSelfRef.__VdfgRegularize_hebeb780c_0_13 = vlSelfRef.top__DOT__cpu_inst__DOT__regfile;
        __VdfgRegularize_hebeb780c_0_18 = vlSelfRef.top__DOT__cpu_inst__DOT__DIMUX;
        __VdfgRegularize_hebeb780c_0_16 = 0U;
    }
    if ((2U & (IData)(vlSelfRef.top__DOT__cpu_inst__DOT__state))) {
        __VdfgRegularize_hebeb780c_0_17 = 0U;
        vlSelfRef.__VdfgRegularize_hebeb780c_0_14 = vlSelfRef.__VdfgRegularize_hebeb780c_0_13;
    } else {
        __VdfgRegularize_hebeb780c_0_17 = vlSelfRef.top__DOT__cpu_inst__DOT__DIMUX;
        vlSelfRef.__VdfgRegularize_hebeb780c_0_14 = 0U;
    }
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
            top__DOT__cpu_inst__DOT__BI = (0x000000ffU 
                                           & (IData)(vlSelfRef.top__DOT__cpu_inst__DOT__DIMUX));
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
                top__DOT__cpu_inst__DOT__BI = (0x000000ffU 
                                               & (IData)(vlSelfRef.top__DOT__cpu_inst__DOT__DIMUX));
                vlSelfRef.top__DOT__cpu_inst__DOT__AI = 0U;
            } else {
                vlSelfRef.top__DOT__cpu_dout = vlSelfRef.top__DOT__cpu_inst__DOT__regfile;
                top__DOT__cpu_inst__DOT__BI = (0x000000ffU 
                                               & (IData)(__VdfgRegularize_hebeb780c_0_17));
                vlSelfRef.top__DOT__cpu_inst__DOT__AI 
                    = vlSelfRef.__VdfgRegularize_hebeb780c_0_14;
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
                top__DOT__cpu_inst__DOT__BI = (0x000000ffU 
                                               & 0U);
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
                top__DOT__cpu_inst__DOT__BI = (0x000000ffU 
                                               & (IData)(__VdfgRegularize_hebeb780c_0_18));
                vlSelfRef.top__DOT__cpu_inst__DOT__AI 
                    = vlSelfRef.__VdfgRegularize_hebeb780c_0_15;
                vlSelfRef.top__DOT__cpu_addr = vlSelfRef.top__DOT__cpu_inst__DOT__PC;
            }
            vlSelfRef.top__DOT__cpu_inst__DOT__PC_temp 
                = vlSelfRef.top__DOT__cpu_inst__DOT__PC;
        }
    } else if ((0x00000010U & (IData)(vlSelfRef.top__DOT__cpu_inst__DOT__state))) {
        if ((8U & (IData)(vlSelfRef.top__DOT__cpu_inst__DOT__state))) {
            if ((4U & (IData)(vlSelfRef.top__DOT__cpu_inst__DOT__state))) {
                vlSelfRef.top__DOT__cpu_dout = vlSelfRef.top__DOT__cpu_inst__DOT__regfile;
                top__DOT__cpu_inst__DOT__BI = (0x000000ffU 
                                               & (IData)(__VdfgRegularize_hebeb780c_0_16));
                vlSelfRef.top__DOT__cpu_inst__DOT__AI 
                    = vlSelfRef.__VdfgRegularize_hebeb780c_0_12;
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
                top__DOT__cpu_inst__DOT__BI = (0x000000ffU 
                                               & (IData)(__VdfgRegularize_hebeb780c_0_17));
                vlSelfRef.top__DOT__cpu_inst__DOT__AI 
                    = vlSelfRef.__VdfgRegularize_hebeb780c_0_14;
            }
            vlSelfRef.top__DOT__cpu_inst__DOT__PC_temp 
                = __VdfgRegularize_hebeb780c_0_2;
        } else {
            vlSelfRef.top__DOT__cpu_dout = vlSelfRef.top__DOT__cpu_inst__DOT__regfile;
            top__DOT__cpu_inst__DOT__BI = (0x000000ffU 
                                           & (IData)(vlSelfRef.top__DOT__cpu_inst__DOT__DIMUX));
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
        }
    } else if ((8U & (IData)(vlSelfRef.top__DOT__cpu_inst__DOT__state))) {
        if ((4U & (IData)(vlSelfRef.top__DOT__cpu_inst__DOT__state))) {
            vlSelfRef.top__DOT__cpu_dout = vlSelfRef.top__DOT__cpu_inst__DOT__regfile;
            if ((2U & (IData)(vlSelfRef.top__DOT__cpu_inst__DOT__state))) {
                top__DOT__cpu_inst__DOT__BI = (0x000000ffU 
                                               & (IData)(__VdfgRegularize_hebeb780c_0_18));
                vlSelfRef.top__DOT__cpu_inst__DOT__AI 
                    = vlSelfRef.__VdfgRegularize_hebeb780c_0_13;
                vlSelfRef.top__DOT__cpu_addr = vlSelfRef.__VdfgRegularize_hebeb780c_0_6;
                vlSelfRef.top__DOT__cpu_inst__DOT__PC_temp 
                    = vlSelfRef.top__DOT__cpu_inst__DOT__PC;
            } else {
                top__DOT__cpu_inst__DOT__BI = (0x000000ffU 
                                               & (IData)(__VdfgRegularize_hebeb780c_0_16));
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
            top__DOT__cpu_inst__DOT__BI = (0x000000ffU 
                                           & 0U);
            vlSelfRef.top__DOT__cpu_inst__DOT__AI = vlSelfRef.__VdfgRegularize_hebeb780c_0_13;
            vlSelfRef.top__DOT__cpu_addr = vlSelfRef.__VdfgRegularize_hebeb780c_0_9;
            vlSelfRef.top__DOT__cpu_inst__DOT__PC_temp 
                = vlSelfRef.top__DOT__cpu_inst__DOT__PC;
        }
    } else {
        vlSelfRef.top__DOT__cpu_dout = vlSelfRef.top__DOT__cpu_inst__DOT__regfile;
        if ((4U & (IData)(vlSelfRef.top__DOT__cpu_inst__DOT__state))) {
            if ((2U & (IData)(vlSelfRef.top__DOT__cpu_inst__DOT__state))) {
                top__DOT__cpu_inst__DOT__BI = (0x000000ffU 
                                               & (IData)(__VdfgRegularize_hebeb780c_0_16));
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
                vlSelfRef.top__DOT__cpu_inst__DOT__AI 
                    = __VdfgRegularize_hebeb780c_0_16;
                vlSelfRef.top__DOT__cpu_inst__DOT__PC_temp 
                    = vlSelfRef.top__DOT__cpu_inst__DOT__PC;
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
            vlSelfRef.top__DOT__cpu_addr = __VdfgRegularize_hebeb780c_0_1;
            vlSelfRef.top__DOT__cpu_inst__DOT__PC_temp 
                = vlSelfRef.top__DOT__cpu_inst__DOT__PC;
        }
    }
    vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__td_addr_a = 0U;
    if ((0x00000010U & (IData)(vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__dma_state))) {
        if (vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__cmd_at_we) {
            vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__at_we_a = 1U;
            vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__at_addr_a 
                = vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__cmd_at_addr;
            vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__at_din_a 
                = vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__cmd_at_din;
        }
        if (vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__cmd_pal_we) {
            vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__pr_we_a = 1U;
            vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__pr_addr_a 
                = vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__cmd_pal_addr_reg;
            vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__pr_din_a 
                = vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__cmd_pal_din;
        }
        if (vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__cmd_nt_we) {
            vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__nt_we_a = 1U;
            vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__nt_addr_a 
                = vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__cmd_nt_addr;
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
                        vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__at_we_a = 1U;
                        vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__at_addr_a 
                            = vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__cmd_at_addr;
                        vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__at_din_a 
                            = vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__cmd_at_din;
                    }
                    if (vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__cmd_pal_we) {
                        vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__pr_we_a = 1U;
                        vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__pr_addr_a 
                            = vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__cmd_pal_addr_reg;
                        vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__pr_din_a 
                            = vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__cmd_pal_din;
                    }
                    if (vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__cmd_nt_we) {
                        vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__nt_we_a = 1U;
                        vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__nt_addr_a 
                            = vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__cmd_nt_addr;
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
                        vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__at_din_a = 0U;
                    }
                } else if (vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__dma_data_valid) {
                    vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__at_we_a = 1U;
                    vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__at_addr_a 
                        = (0x00000fffU & ((((IData)(0x000003e8U) 
                                            * (IData)(vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__dma_nt)) 
                                           + ((IData)(0x0028U) 
                                              * (IData)(vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__dma_col_idx))) 
                                          + (IData)(vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__dma_col_row)));
                    vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__at_din_a 
                        = vlSelfRef.top__DOT__tile_dma_data;
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
                        vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__nt_din_a 
                            = ((0x18U >= (IData)(vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__dma_col_idx))
                                ? vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__col_buffer
                               [vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__dma_col_idx]
                                : 0U);
                    }
                } else if ((0x03e8U > (IData)(vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__dma_count))) {
                    vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__nt_we_a = 1U;
                    vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__nt_addr_a 
                        = (0x00000fffU & (((IData)(0x000003e8U) 
                                           * (IData)(vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__dma_nt)) 
                                          + (IData)(vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__dma_count)));
                    vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__nt_din_a 
                        = vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__dma_fill_val;
                }
            } else if ((1U & (IData)(vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__dma_state))) {
                if ((0x0fa0U > (IData)(vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__dma_fill_addr))) {
                    vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__nt_we_a = 1U;
                    vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__nt_addr_a 
                        = vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__dma_fill_addr;
                    vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__nt_din_a = 0U;
                }
            }
        }
    } else {
        if ((4U & (IData)(vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__dma_state))) {
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
                        vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__at_din_a 
                            = vlSelfRef.top__DOT__tile_dma_data;
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
                        vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__nt_din_a 
                            = vlSelfRef.top__DOT__tile_dma_data;
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
                    vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__nt_din_a 
                        = vlSelfRef.top__DOT__tile_dma_data;
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
                        vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__at_we_a = 1U;
                        vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__at_addr_a 
                            = (0x00000fffU & (((IData)(0x000003e8U) 
                                               * (IData)(vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__dma_nt)) 
                                              + (0x000003ffU 
                                                 & (IData)(vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__dma_dst))));
                        vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__at_din_a 
                            = vlSelfRef.top__DOT__tile_dma_data;
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
                        vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__nt_din_a 
                            = vlSelfRef.top__DOT__tile_dma_data;
                    }
                }
            } else if ((1U & (~ (IData)(vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__dma_state)))) {
                if (vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__cmd_at_we) {
                    vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__at_we_a = 1U;
                    vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__at_addr_a 
                        = vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__cmd_at_addr;
                    vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__at_din_a 
                        = vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__cmd_at_din;
                }
                if (vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__cmd_nt_we) {
                    vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__nt_we_a = 1U;
                    vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__nt_addr_a 
                        = vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__cmd_nt_addr;
                    vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__nt_din_a 
                        = vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__cmd_nt_din;
                }
            }
            if ((1U & (~ ((IData)(vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__dma_state) 
                          >> 1U)))) {
                if ((1U & (~ (IData)(vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__dma_state)))) {
                    if (vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__cmd_pal_we) {
                        vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__pr_we_a = 1U;
                        vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__pr_addr_a 
                            = vlSelfRef.top__DOT__vgc_inst__DOT__tile_inst__DOT__cmd_pal_addr_reg;
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

VL_ATTR_COLD void Vtop_sid_chip___stl_sequent__TOP__top__DOT__sid1_inst__0(Vtop_sid_chip* vlSelf);
VL_ATTR_COLD void Vtop_sid_chip___stl_sequent__TOP__top__DOT__sid2_inst__0(Vtop_sid_chip* vlSelf);
void Vtop___024root___ico_sequent__TOP__1(Vtop___024root* vlSelf);
void Vtop_sid_chip___act_sequent__TOP__top__DOT__sid1_inst__0(Vtop_sid_chip* vlSelf);
void Vtop_sid_chip___act_sequent__TOP__top__DOT__sid1_inst__1(Vtop_sid_chip* vlSelf);
void Vtop_sid_chip___act_sequent__TOP__top__DOT__sid1_inst__2(Vtop_sid_chip* vlSelf);
void Vtop_sid_chip___act_sequent__TOP__top__DOT__sid1_inst__3(Vtop_sid_chip* vlSelf);
void Vtop_sid_chip___act_sequent__TOP__top__DOT__sid1_inst__4(Vtop_sid_chip* vlSelf);
void Vtop_sid_chip___act_sequent__TOP__top__DOT__sid1_inst__5(Vtop_sid_chip* vlSelf);

VL_ATTR_COLD void Vtop___024root___eval_stl(Vtop___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vtop___024root___eval_stl\n"); );
    Vtop__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
    // Body
    if ((1ULL & vlSelfRef.__VstlTriggered[1U])) {
        Vtop___024root___stl_sequent__TOP__0(vlSelf);
        Vtop_sid_chip___stl_sequent__TOP__top__DOT__sid1_inst__0((&vlSymsp->TOP__top__DOT__sid1_inst));
        Vtop_sid_chip___stl_sequent__TOP__top__DOT__sid2_inst__0((&vlSymsp->TOP__top__DOT__sid2_inst));
        Vtop___024root___ico_sequent__TOP__1(vlSelf);
    }
    if (((1ULL & vlSelfRef.__VstlTriggered[1U]) | (2ULL 
                                                   & vlSelfRef.__VstlTriggered[0U]))) {
        Vtop_sid_chip___act_sequent__TOP__top__DOT__sid1_inst__0((&vlSymsp->TOP__top__DOT__sid1_inst));
    }
    if (((1ULL & vlSelfRef.__VstlTriggered[1U]) | (8ULL 
                                                   & vlSelfRef.__VstlTriggered[0U]))) {
        Vtop_sid_chip___act_sequent__TOP__top__DOT__sid1_inst__1((&vlSymsp->TOP__top__DOT__sid1_inst));
    }
    if (((1ULL & vlSelfRef.__VstlTriggered[1U]) | (0x0000000000000020ULL 
                                                   & vlSelfRef.__VstlTriggered[0U]))) {
        Vtop_sid_chip___act_sequent__TOP__top__DOT__sid1_inst__2((&vlSymsp->TOP__top__DOT__sid1_inst));
    }
    if (((1ULL & vlSelfRef.__VstlTriggered[1U]) | (0x0000000000000080ULL 
                                                   & vlSelfRef.__VstlTriggered[0U]))) {
        Vtop_sid_chip___act_sequent__TOP__top__DOT__sid1_inst__0((&vlSymsp->TOP__top__DOT__sid2_inst));
    }
    if (((1ULL & vlSelfRef.__VstlTriggered[1U]) | (0x0000000000000200ULL 
                                                   & vlSelfRef.__VstlTriggered[0U]))) {
        Vtop_sid_chip___act_sequent__TOP__top__DOT__sid1_inst__1((&vlSymsp->TOP__top__DOT__sid2_inst));
    }
    if (((1ULL & vlSelfRef.__VstlTriggered[1U]) | (0x0000000000000800ULL 
                                                   & vlSelfRef.__VstlTriggered[0U]))) {
        Vtop_sid_chip___act_sequent__TOP__top__DOT__sid1_inst__2((&vlSymsp->TOP__top__DOT__sid2_inst));
    }
    if ((1ULL & (vlSelfRef.__VstlTriggered[1U] | vlSelfRef.__VstlTriggered[0U]))) {
        Vtop_sid_chip___act_sequent__TOP__top__DOT__sid1_inst__3((&vlSymsp->TOP__top__DOT__sid1_inst));
    }
    if (((1ULL & vlSelfRef.__VstlTriggered[1U]) | (4ULL 
                                                   & vlSelfRef.__VstlTriggered[0U]))) {
        Vtop_sid_chip___act_sequent__TOP__top__DOT__sid1_inst__4((&vlSymsp->TOP__top__DOT__sid1_inst));
    }
    if (((1ULL & vlSelfRef.__VstlTriggered[1U]) | (0x0000000000000010ULL 
                                                   & vlSelfRef.__VstlTriggered[0U]))) {
        Vtop_sid_chip___act_sequent__TOP__top__DOT__sid1_inst__5((&vlSymsp->TOP__top__DOT__sid1_inst));
    }
    if (((1ULL & vlSelfRef.__VstlTriggered[1U]) | (0x0000000000000040ULL 
                                                   & vlSelfRef.__VstlTriggered[0U]))) {
        Vtop_sid_chip___act_sequent__TOP__top__DOT__sid1_inst__3((&vlSymsp->TOP__top__DOT__sid2_inst));
    }
    if (((1ULL & vlSelfRef.__VstlTriggered[1U]) | (0x0000000000000100ULL 
                                                   & vlSelfRef.__VstlTriggered[0U]))) {
        Vtop_sid_chip___act_sequent__TOP__top__DOT__sid1_inst__4((&vlSymsp->TOP__top__DOT__sid2_inst));
    }
    if (((1ULL & vlSelfRef.__VstlTriggered[1U]) | (0x0000000000000400ULL 
                                                   & vlSelfRef.__VstlTriggered[0U]))) {
        Vtop_sid_chip___act_sequent__TOP__top__DOT__sid1_inst__5((&vlSymsp->TOP__top__DOT__sid2_inst));
    }
}

VL_ATTR_COLD bool Vtop___024root___eval_phase__stl(Vtop___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vtop___024root___eval_phase__stl\n"); );
    Vtop__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
    // Locals
    CData/*0:0*/ __VstlExecute;
    // Body
    Vtop___024root___eval_triggers_vec__stl(vlSelf);
#ifdef VL_DEBUG
    if (VL_UNLIKELY(vlSymsp->_vm_contextp__->debug())) {
        Vtop___024root___dump_triggers__stl(vlSelfRef.__VstlTriggered, "stl"s);
    }
#endif
    __VstlExecute = Vtop___024root___trigger_anySet__stl(vlSelfRef.__VstlTriggered);
    if (__VstlExecute) {
        Vtop___024root___eval_stl(vlSelf);
    }
    return (__VstlExecute);
}

bool Vtop___024root___trigger_anySet__ico(const VlUnpacked<QData/*63:0*/, 1> &in);

#ifdef VL_DEBUG
VL_ATTR_COLD void Vtop___024root___dump_triggers__ico(const VlUnpacked<QData/*63:0*/, 1> &triggers, const std::string &tag) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vtop___024root___dump_triggers__ico\n"); );
    // Body
    if ((1U & (~ (IData)(Vtop___024root___trigger_anySet__ico(triggers))))) {
        VL_DBG_MSGS("         No '" + tag + "' region triggers active\n");
    }
    if ((1U & (IData)(triggers[0U]))) {
        VL_DBG_MSGS("         '" + tag + "' region trigger index 0 is active: Internal 'ico' trigger - first iteration\n");
    }
}
#endif  // VL_DEBUG

bool Vtop___024root___trigger_anySet__act(const VlUnpacked<QData/*63:0*/, 1> &in);

#ifdef VL_DEBUG
VL_ATTR_COLD void Vtop___024root___dump_triggers__act(const VlUnpacked<QData/*63:0*/, 1> &triggers, const std::string &tag) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vtop___024root___dump_triggers__act\n"); );
    // Body
    if ((1U & (~ (IData)(Vtop___024root___trigger_anySet__act(triggers))))) {
        VL_DBG_MSGS("         No '" + tag + "' region triggers active\n");
    }
    if ((1U & (IData)(triggers[0U]))) {
        VL_DBG_MSGS("         '" + tag + "' region trigger index 0 is active: @([hybrid] top.sid1_inst.v1.waveform_dac.bitsum)\n");
    }
    if ((1U & (IData)((triggers[0U] >> 1U)))) {
        VL_DBG_MSGS("         '" + tag + "' region trigger index 1 is active: @([hybrid] top.sid1_inst.v1.envelope_dac.bitsum)\n");
    }
    if ((1U & (IData)((triggers[0U] >> 2U)))) {
        VL_DBG_MSGS("         '" + tag + "' region trigger index 2 is active: @([hybrid] top.sid1_inst.v2.waveform_dac.bitsum)\n");
    }
    if ((1U & (IData)((triggers[0U] >> 3U)))) {
        VL_DBG_MSGS("         '" + tag + "' region trigger index 3 is active: @([hybrid] top.sid1_inst.v2.envelope_dac.bitsum)\n");
    }
    if ((1U & (IData)((triggers[0U] >> 4U)))) {
        VL_DBG_MSGS("         '" + tag + "' region trigger index 4 is active: @([hybrid] top.sid1_inst.v3.waveform_dac.bitsum)\n");
    }
    if ((1U & (IData)((triggers[0U] >> 5U)))) {
        VL_DBG_MSGS("         '" + tag + "' region trigger index 5 is active: @([hybrid] top.sid1_inst.v3.envelope_dac.bitsum)\n");
    }
    if ((1U & (IData)((triggers[0U] >> 6U)))) {
        VL_DBG_MSGS("         '" + tag + "' region trigger index 6 is active: @([hybrid] top.sid2_inst.v1.waveform_dac.bitsum)\n");
    }
    if ((1U & (IData)((triggers[0U] >> 7U)))) {
        VL_DBG_MSGS("         '" + tag + "' region trigger index 7 is active: @([hybrid] top.sid2_inst.v1.envelope_dac.bitsum)\n");
    }
    if ((1U & (IData)((triggers[0U] >> 8U)))) {
        VL_DBG_MSGS("         '" + tag + "' region trigger index 8 is active: @([hybrid] top.sid2_inst.v2.waveform_dac.bitsum)\n");
    }
    if ((1U & (IData)((triggers[0U] >> 9U)))) {
        VL_DBG_MSGS("         '" + tag + "' region trigger index 9 is active: @([hybrid] top.sid2_inst.v2.envelope_dac.bitsum)\n");
    }
    if ((1U & (IData)((triggers[0U] >> 0x0000000aU)))) {
        VL_DBG_MSGS("         '" + tag + "' region trigger index 10 is active: @([hybrid] top.sid2_inst.v3.waveform_dac.bitsum)\n");
    }
    if ((1U & (IData)((triggers[0U] >> 0x0000000bU)))) {
        VL_DBG_MSGS("         '" + tag + "' region trigger index 11 is active: @([hybrid] top.sid2_inst.v3.envelope_dac.bitsum)\n");
    }
    if ((1U & (IData)((triggers[0U] >> 0x0000000cU)))) {
        VL_DBG_MSGS("         '" + tag + "' region trigger index 12 is active: @(posedge clk)\n");
    }
    if ((1U & (IData)((triggers[0U] >> 0x0000000dU)))) {
        VL_DBG_MSGS("         '" + tag + "' region trigger index 13 is active: @(posedge rst)\n");
    }
}
#endif  // VL_DEBUG

VL_ATTR_COLD void Vtop___024root___ctor_var_reset(Vtop___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vtop___024root___ctor_var_reset\n"); );
    Vtop__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
    // Body
    const uint64_t __VscopeHash = VL_MURMUR64_HASH(vlSelf->vlNamep);
    vlSelf->clk = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 16707436170211756652ull);
    vlSelf->rst = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 18209466448985614591ull);
    vlSelf->key_valid = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 11641391107834527882ull);
    vlSelf->key_data = VL_SCOPED_RAND_RESET_I(8, __VscopeHash, 16169336856313411461ull);
    vlSelf->irq_n = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 10901006744231566505ull);
    vlSelf->nmi_n = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 809203393953361429ull);
    vlSelf->vid_r = VL_SCOPED_RAND_RESET_I(4, __VscopeHash, 4640268748951543978ull);
    vlSelf->vid_g = VL_SCOPED_RAND_RESET_I(4, __VscopeHash, 16072396866014922634ull);
    vlSelf->vid_b = VL_SCOPED_RAND_RESET_I(4, __VscopeHash, 14561862645762128483ull);
    vlSelf->vid_hsync = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 9848722673327594505ull);
    vlSelf->vid_vsync = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 5714281414838969621ull);
    vlSelf->vid_de = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 8410564023838534647ull);
    vlSelf->audio_l = VL_SCOPED_RAND_RESET_I(18, __VscopeHash, 5668020072184496527ull);
    vlSelf->audio_r = VL_SCOPED_RAND_RESET_I(18, __VscopeHash, 10688841200571485478ull);
    vlSelf->dbg_peek_en = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 244272040184725475ull);
    vlSelf->dbg_peek_addr = VL_SCOPED_RAND_RESET_I(16, __VscopeHash, 15295359251860161495ull);
    vlSelf->dbg_peek_data = VL_SCOPED_RAND_RESET_I(8, __VscopeHash, 12942318061505056156ull);
    vlSelf->dbg_poke_en = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 16868296657042138612ull);
    vlSelf->dbg_poke_addr = VL_SCOPED_RAND_RESET_I(16, __VscopeHash, 17846633060601417214ull);
    vlSelf->dbg_poke_data = VL_SCOPED_RAND_RESET_I(8, __VscopeHash, 16464346550525629492ull);
    vlSelf->dbg_pause = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 16983694524354527678ull);
    vlSelf->dbg_cpu_pc = VL_SCOPED_RAND_RESET_I(16, __VscopeHash, 13788619681378170624ull);
    vlSelf->dbg_cpu_a = VL_SCOPED_RAND_RESET_I(8, __VscopeHash, 7052850545315918333ull);
    vlSelf->dbg_cpu_x = VL_SCOPED_RAND_RESET_I(8, __VscopeHash, 842755296699267530ull);
    vlSelf->dbg_cpu_y = VL_SCOPED_RAND_RESET_I(8, __VscopeHash, 73451268135057667ull);
    vlSelf->dbg_cpu_sp = VL_SCOPED_RAND_RESET_I(8, __VscopeHash, 2143822731886580633ull);
    vlSelf->dbg_cpu_flags = VL_SCOPED_RAND_RESET_I(8, __VscopeHash, 10100854060673285788ull);
    vlSelf->top__DOT__cpu_addr = VL_SCOPED_RAND_RESET_I(16, __VscopeHash, 14062992931533076123ull);
    vlSelf->top__DOT__cpu_dout = VL_SCOPED_RAND_RESET_I(8, __VscopeHash, 444429073127247956ull);
    vlSelf->top__DOT__cpu_we = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 11970622726319456214ull);
    vlSelf->top__DOT__cpu_din = VL_SCOPED_RAND_RESET_I(8, __VscopeHash, 4375516561482716724ull);
    for (int __Vi0 = 0; __Vi0 < 65536; ++__Vi0) {
        vlSelf->top__DOT__ram[__Vi0] = VL_SCOPED_RAND_RESET_I(8, __VscopeHash, 2614412921040765590ull);
    }
    for (int __Vi0 = 0; __Vi0 < 16384; ++__Vi0) {
        vlSelf->top__DOT__basic_rom[__Vi0] = VL_SCOPED_RAND_RESET_I(8, __VscopeHash, 11043186321196759923ull);
    }
    for (int __Vi0 = 0; __Vi0 < 16384; ++__Vi0) {
        vlSelf->top__DOT__ext_rom[__Vi0] = VL_SCOPED_RAND_RESET_I(8, __VscopeHash, 5165524949639356115ull);
    }
    vlSelf->top__DOT__ext_rom_active = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 2960931826611579899ull);
    for (int __Vi0 = 0; __Vi0 < 524288; ++__Vi0) {
        vlSelf->top__DOT__xram[__Vi0] = VL_SCOPED_RAND_RESET_I(8, __VscopeHash, 11801498153876470018ull);
    }
    for (int __Vi0 = 0; __Vi0 < 64; ++__Vi0) {
        vlSelf->top__DOT__xmc_regs[__Vi0] = VL_SCOPED_RAND_RESET_I(8, __VscopeHash, 668427059981155529ull);
    }
    vlSelf->top__DOT__xmc_reg_sel = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 9179279545183214600ull);
    vlSelf->top__DOT__xmc_win_sel = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 7338878830744560571ull);
    vlSelf->top__DOT__xmc_addr = VL_SCOPED_RAND_RESET_I(19, __VscopeHash, 6642094466747786515ull);
    vlSelf->top__DOT__xmc_win_enabled = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 1841433208277389150ull);
    vlSelf->top__DOT__blt_reg_sel = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 7946572707348209205ull);
    vlSelf->top__DOT__sid1_reg_sel = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 16274306454749511209ull);
    vlSelf->top__DOT__sid2_reg_sel = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 5335752436860021003ull);
    vlSelf->top__DOT__clear_input = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 11367160002665639816ull);
    vlSelf->top__DOT__tile_dma_data = VL_SCOPED_RAND_RESET_I(8, __VscopeHash, 5913762659468986911ull);
    vlSelf->top__DOT__blt_ram_addr = VL_SCOPED_RAND_RESET_I(16, __VscopeHash, 15220515095603648651ull);
    vlSelf->top__DOT__blt_ram_wdata = VL_SCOPED_RAND_RESET_I(8, __VscopeHash, 14714805443437309559ull);
    vlSelf->top__DOT__blt_ram_we = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 14915426918176470070ull);
    vlSelf->top__DOT__blt_xram_addr = VL_SCOPED_RAND_RESET_I(19, __VscopeHash, 5359636542878184612ull);
    vlSelf->top__DOT__blt_xram_wdata = VL_SCOPED_RAND_RESET_I(8, __VscopeHash, 3205237527378164469ull);
    vlSelf->top__DOT__blt_xram_we = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 13314375415681483190ull);
    vlSelf->top__DOT__blt_vgc_space = VL_SCOPED_RAND_RESET_I(3, __VscopeHash, 13650049987070813097ull);
    vlSelf->top__DOT__blt_vgc_addr = VL_SCOPED_RAND_RESET_I(16, __VscopeHash, 16368854210064407662ull);
    vlSelf->top__DOT__blt_vgc_we = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 1831237465086001060ull);
    vlSelf->top__DOT__blt_vgc_re = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 14198989992373416130ull);
    vlSelf->top__DOT__blt_cpu_rdata = VL_SCOPED_RAND_RESET_I(8, __VscopeHash, 15579734964006782715ull);
    vlSelf->top__DOT__sid_clk_div = VL_SCOPED_RAND_RESET_I(5, __VscopeHash, 9732289825198143397ull);
    vlSelf->top__DOT__sid_ce_1m = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 7415519373683493794ull);
    vlSelf->top__DOT__sid_cfg_reg = VL_SCOPED_RAND_RESET_I(8, __VscopeHash, 6089664830302405009ull);
    vlSelf->top__DOT____Vcellinp__cpu_inst__RDY = 0;
    vlSelf->top__DOT__vgc_dbg_rdata = VL_SCOPED_RAND_RESET_I(8, __VscopeHash, 7010089783760776327ull);
    vlSelf->top__DOT__tile_dma_addr = VL_SCOPED_RAND_RESET_I(16, __VscopeHash, 14225546897381597088ull);
    vlSelf->top__DOT__blt_inst__DOT____Vlvbound_hf31f73b8__0 = 0;
    for (int __Vi0 = 0; __Vi0 < 25; ++__Vi0) {
        vlSelf->top__DOT__blt_inst__DOT__regs[__Vi0] = VL_SCOPED_RAND_RESET_I(8, __VscopeHash, 10615353747989713705ull);
    }
    vlSelf->top__DOT__blt_inst__DOT__state = VL_SCOPED_RAND_RESET_I(4, __VscopeHash, 3621452129638494694ull);
    vlSelf->top__DOT__blt_inst__DOT__fill_mode = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 14604537939961932011ull);
    vlSelf->top__DOT__blt_inst__DOT__colorkey_mode = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 6952143994682428174ull);
    vlSelf->top__DOT__blt_inst__DOT__use_row_buffer = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 3183893000724161879ull);
    vlSelf->top__DOT__blt_inst__DOT__src_space = VL_SCOPED_RAND_RESET_I(3, __VscopeHash, 14425693829985727049ull);
    vlSelf->top__DOT__blt_inst__DOT__dst_space = VL_SCOPED_RAND_RESET_I(3, __VscopeHash, 2414262048605809721ull);
    vlSelf->top__DOT__blt_inst__DOT__src_base = VL_SCOPED_RAND_RESET_I(24, __VscopeHash, 14021839725676378438ull);
    vlSelf->top__DOT__blt_inst__DOT__dst_base = VL_SCOPED_RAND_RESET_I(24, __VscopeHash, 12758082617344892450ull);
    vlSelf->top__DOT__blt_inst__DOT__width = VL_SCOPED_RAND_RESET_I(16, __VscopeHash, 4005018124557909205ull);
    vlSelf->top__DOT__blt_inst__DOT__height = VL_SCOPED_RAND_RESET_I(16, __VscopeHash, 6655388565015754821ull);
    vlSelf->top__DOT__blt_inst__DOT__src_stride = VL_SCOPED_RAND_RESET_I(16, __VscopeHash, 10424210872337532402ull);
    vlSelf->top__DOT__blt_inst__DOT__dst_stride = VL_SCOPED_RAND_RESET_I(16, __VscopeHash, 18123801988395345917ull);
    vlSelf->top__DOT__blt_inst__DOT__fill_value = VL_SCOPED_RAND_RESET_I(8, __VscopeHash, 15013190378298026637ull);
    vlSelf->top__DOT__blt_inst__DOT__color_key = VL_SCOPED_RAND_RESET_I(8, __VscopeHash, 2680567662511816626ull);
    vlSelf->top__DOT__blt_inst__DOT__row = VL_SCOPED_RAND_RESET_I(16, __VscopeHash, 11733782728399274466ull);
    vlSelf->top__DOT__blt_inst__DOT__col = VL_SCOPED_RAND_RESET_I(16, __VscopeHash, 13468467093051691444ull);
    vlSelf->top__DOT__blt_inst__DOT__wrote_count = VL_SCOPED_RAND_RESET_I(24, __VscopeHash, 15186606614640862608ull);
    vlSelf->top__DOT__blt_inst__DOT__read_byte = VL_SCOPED_RAND_RESET_I(8, __VscopeHash, 8549441478472208387ull);
    vlSelf->top__DOT__blt_inst__DOT__read_valid = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 12566023935232343193ull);
    for (int __Vi0 = 0; __Vi0 < 1024; ++__Vi0) {
        vlSelf->top__DOT__blt_inst__DOT__row_buf[__Vi0] = VL_SCOPED_RAND_RESET_I(8, __VscopeHash, 7715010631369937249ull);
    }
    vlSelf->top__DOT__blt_inst__DOT__buf_idx = VL_SCOPED_RAND_RESET_I(10, __VscopeHash, 10093097965691303302ull);
    vlSelf->top__DOT__blt_inst__DOT__mem_read_data = VL_SCOPED_RAND_RESET_I(8, __VscopeHash, 11755479944786947581ull);
    vlSelf->top__DOT__cpu_inst__DOT__PC = VL_SCOPED_RAND_RESET_I(16, __VscopeHash, 14269910086237652175ull);
    vlSelf->top__DOT__cpu_inst__DOT__ABL = VL_SCOPED_RAND_RESET_I(8, __VscopeHash, 18179433189645338737ull);
    vlSelf->top__DOT__cpu_inst__DOT__ABH = VL_SCOPED_RAND_RESET_I(8, __VscopeHash, 13220569999968142928ull);
    vlSelf->top__DOT__cpu_inst__DOT__ADD = VL_SCOPED_RAND_RESET_I(8, __VscopeHash, 15518772865053114549ull);
    vlSelf->top__DOT__cpu_inst__DOT__DIHOLD = VL_SCOPED_RAND_RESET_I(8, __VscopeHash, 14818661888454435886ull);
    vlSelf->top__DOT__cpu_inst__DOT__DIMUX = VL_SCOPED_RAND_RESET_I(8, __VscopeHash, 6555060123129795683ull);
    vlSelf->top__DOT__cpu_inst__DOT__IRHOLD = VL_SCOPED_RAND_RESET_I(8, __VscopeHash, 1595376696550913710ull);
    vlSelf->top__DOT__cpu_inst__DOT__IRHOLD_valid = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 18188728206591177384ull);
    for (int __Vi0 = 0; __Vi0 < 4; ++__Vi0) {
        vlSelf->top__DOT__cpu_inst__DOT__AXYS[__Vi0] = VL_SCOPED_RAND_RESET_I(8, __VscopeHash, 9255314609532243383ull);
    }
    vlSelf->top__DOT__cpu_inst__DOT__AN = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 9600171164098206906ull);
    vlSelf->top__DOT__cpu_inst__DOT__HC = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 16543910592533678532ull);
    vlSelf->top__DOT__cpu_inst__DOT__AI = VL_SCOPED_RAND_RESET_I(8, __VscopeHash, 5650390190020284061ull);
    vlSelf->top__DOT__cpu_inst__DOT__IR = VL_SCOPED_RAND_RESET_I(8, __VscopeHash, 3802907230601892423ull);
    vlSelf->top__DOT__cpu_inst__DOT__CI = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 1887116244910660575ull);
    vlSelf->top__DOT__cpu_inst__DOT__CO = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 11889451382013411164ull);
    vlSelf->top__DOT__cpu_inst__DOT__regsel = VL_SCOPED_RAND_RESET_I(2, __VscopeHash, 7838246373902931601ull);
    vlSelf->top__DOT__cpu_inst__DOT__regfile = VL_SCOPED_RAND_RESET_I(8, __VscopeHash, 7611307552824499813ull);
    vlSelf->top__DOT__cpu_inst__DOT__P = VL_SCOPED_RAND_RESET_I(8, __VscopeHash, 4379680362878439054ull);
    vlSelf->top__DOT__cpu_inst__DOT__state = VL_SCOPED_RAND_RESET_I(6, __VscopeHash, 4831566915033447229ull);
    vlSelf->top__DOT__cpu_inst__DOT__PC_inc = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 15230384814563280195ull);
    vlSelf->top__DOT__cpu_inst__DOT__PC_temp = VL_SCOPED_RAND_RESET_I(16, __VscopeHash, 17612171515425806060ull);
    vlSelf->top__DOT__cpu_inst__DOT__src_reg = VL_SCOPED_RAND_RESET_I(2, __VscopeHash, 11363332445740795812ull);
    vlSelf->top__DOT__cpu_inst__DOT__dst_reg = VL_SCOPED_RAND_RESET_I(2, __VscopeHash, 771961669877902700ull);
    vlSelf->top__DOT__cpu_inst__DOT__index_y = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 13154849637610631028ull);
    vlSelf->top__DOT__cpu_inst__DOT__load_reg = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 4234312489347983380ull);
    vlSelf->top__DOT__cpu_inst__DOT__inc = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 13672967476129863391ull);
    vlSelf->top__DOT__cpu_inst__DOT__write_back = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 3501206622915590587ull);
    vlSelf->top__DOT__cpu_inst__DOT__load_only = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 9335111899031972341ull);
    vlSelf->top__DOT__cpu_inst__DOT__store = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 3861193402586961545ull);
    vlSelf->top__DOT__cpu_inst__DOT__adc_sbc = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 11012220920010841010ull);
    vlSelf->top__DOT__cpu_inst__DOT__compare = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 11995092037005419023ull);
    vlSelf->top__DOT__cpu_inst__DOT__shift = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 10853005806602841175ull);
    vlSelf->top__DOT__cpu_inst__DOT__rotate = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 5643260013987046180ull);
    vlSelf->top__DOT__cpu_inst__DOT__backwards = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 16686328707291985893ull);
    vlSelf->top__DOT__cpu_inst__DOT__cond_true = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 3565754102815746669ull);
    vlSelf->top__DOT__cpu_inst__DOT__cond_code = VL_SCOPED_RAND_RESET_I(3, __VscopeHash, 11208018378703319351ull);
    vlSelf->top__DOT__cpu_inst__DOT__shift_right = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 15761215316816912860ull);
    vlSelf->top__DOT__cpu_inst__DOT__alu_shift_right = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 3905987511652885668ull);
    vlSelf->top__DOT__cpu_inst__DOT__op = VL_SCOPED_RAND_RESET_I(4, __VscopeHash, 13088739121448865546ull);
    vlSelf->top__DOT__cpu_inst__DOT__alu_op = VL_SCOPED_RAND_RESET_I(4, __VscopeHash, 2252645933390949528ull);
    vlSelf->top__DOT__cpu_inst__DOT__adc_bcd = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 14071717265758327586ull);
    vlSelf->top__DOT__cpu_inst__DOT__adj_bcd = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 18127576558896255852ull);
    vlSelf->top__DOT__cpu_inst__DOT__bit_ins = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 15202495604571443732ull);
    vlSelf->top__DOT__cpu_inst__DOT__plp = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 3782470103023875918ull);
    vlSelf->top__DOT__cpu_inst__DOT__php = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 3883360762843877700ull);
    vlSelf->top__DOT__cpu_inst__DOT__clc = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 12447630738140975185ull);
    vlSelf->top__DOT__cpu_inst__DOT__sec = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 11632116232281006163ull);
    vlSelf->top__DOT__cpu_inst__DOT__cld = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 2380044781392245825ull);
    vlSelf->top__DOT__cpu_inst__DOT__sed = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 17354184654259548810ull);
    vlSelf->top__DOT__cpu_inst__DOT__cli = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 14743546793483739970ull);
    vlSelf->top__DOT__cpu_inst__DOT__sei = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 16285526021408168652ull);
    vlSelf->top__DOT__cpu_inst__DOT__clv = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 17809889738935702673ull);
    vlSelf->top__DOT__cpu_inst__DOT__res = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 13793966882550452310ull);
    vlSelf->top__DOT__cpu_inst__DOT__write_register = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 12392953634098252799ull);
    vlSelf->top__DOT__cpu_inst__DOT____Vcellinp__ALU__BCD = 0;
    vlSelf->top__DOT__cpu_inst__DOT__ALU__DOT__AI7 = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 13571103649231066904ull);
    vlSelf->top__DOT__cpu_inst__DOT__ALU__DOT__BI7 = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 17063848402528385936ull);
    vlSelf->top__DOT__cpu_inst__DOT__ALU__DOT__temp_BI = VL_SCOPED_RAND_RESET_I(8, __VscopeHash, 3312840931277776077ull);
    vlSelf->top__DOT__cpu_inst__DOT__ALU__DOT__temp_h = VL_SCOPED_RAND_RESET_I(5, __VscopeHash, 15410355837646859569ull);
    vlSelf->top__DOT__cpu_inst__DOT__ALU__DOT__temp = VL_SCOPED_RAND_RESET_I(9, __VscopeHash, 12374956371556617101ull);
    vlSelf->top__DOT__cpu_inst__DOT__ALU__DOT__temp_HC = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 13438428945788262813ull);
    vlSelf->top__DOT__vgc_inst__DOT__h_count = VL_SCOPED_RAND_RESET_I(10, __VscopeHash, 11805546863561209031ull);
    vlSelf->top__DOT__vgc_inst__DOT__v_count = VL_SCOPED_RAND_RESET_I(10, __VscopeHash, 4453295177809797979ull);
    vlSelf->top__DOT__vgc_inst__DOT__visible = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 5159027323273974455ull);
    vlSelf->top__DOT__vgc_inst__DOT__in_text_area = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 1709526822613847052ull);
    vlSelf->top__DOT__vgc_inst__DOT__visible_d1 = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 3368610048829115066ull);
    vlSelf->top__DOT__vgc_inst__DOT__visible_d2 = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 13643443695618079937ull);
    vlSelf->top__DOT__vgc_inst__DOT__in_text_area_d1 = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 15381436765347590331ull);
    vlSelf->top__DOT__vgc_inst__DOT__in_text_area_d2 = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 3682653434966715022ull);
    vlSelf->top__DOT__vgc_inst__DOT__h_sync_area_d1 = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 17544493835827973679ull);
    vlSelf->top__DOT__vgc_inst__DOT__h_sync_area_d2 = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 6041120481661357050ull);
    vlSelf->top__DOT__vgc_inst__DOT__v_sync_area_d1 = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 17809167288931480635ull);
    vlSelf->top__DOT__vgc_inst__DOT__v_sync_area_d2 = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 14202119569054947299ull);
    vlSelf->top__DOT__vgc_inst__DOT__text_row = VL_SCOPED_RAND_RESET_I(5, __VscopeHash, 13389520431090633350ull);
    vlSelf->top__DOT__vgc_inst__DOT__real_row = VL_SCOPED_RAND_RESET_I(5, __VscopeHash, 7302056808584412249ull);
    vlSelf->top__DOT__vgc_inst__DOT__font_line = VL_SCOPED_RAND_RESET_I(3, __VscopeHash, 6090671473237101265ull);
    vlSelf->top__DOT__vgc_inst__DOT__gfx_y = VL_SCOPED_RAND_RESET_I(8, __VscopeHash, 15900382798927743579ull);
    vlSelf->top__DOT__vgc_inst__DOT__text_col_d1 = VL_SCOPED_RAND_RESET_I(7, __VscopeHash, 4722840637796391101ull);
    vlSelf->top__DOT__vgc_inst__DOT__text_col_d2 = VL_SCOPED_RAND_RESET_I(7, __VscopeHash, 9459398846198723332ull);
    vlSelf->top__DOT__vgc_inst__DOT__text_row_d1 = VL_SCOPED_RAND_RESET_I(5, __VscopeHash, 3601926593540287247ull);
    vlSelf->top__DOT__vgc_inst__DOT__text_row_d2 = VL_SCOPED_RAND_RESET_I(5, __VscopeHash, 2143928913919262936ull);
    vlSelf->top__DOT__vgc_inst__DOT__font_pixel_d1 = VL_SCOPED_RAND_RESET_I(3, __VscopeHash, 6897380327875258526ull);
    vlSelf->top__DOT__vgc_inst__DOT__font_pixel_d2 = VL_SCOPED_RAND_RESET_I(3, __VscopeHash, 4376953757791658242ull);
    vlSelf->top__DOT__vgc_inst__DOT__gfx_x_d1 = VL_SCOPED_RAND_RESET_I(9, __VscopeHash, 14397124288206199523ull);
    vlSelf->top__DOT__vgc_inst__DOT__gfx_x_d2 = VL_SCOPED_RAND_RESET_I(9, __VscopeHash, 10882738839612128231ull);
    vlSelf->top__DOT__vgc_inst__DOT__scroll_offset = VL_SCOPED_RAND_RESET_I(5, __VscopeHash, 12672702492182880445ull);
    for (int __Vi0 = 0; __Vi0 < 16; ++__Vi0) {
        vlSelf->top__DOT__vgc_inst__DOT__palette[__Vi0] = VL_SCOPED_RAND_RESET_I(12, __VscopeHash, 10498909240792947834ull);
    }
    vlSelf->top__DOT__vgc_inst__DOT__char_a_addr = VL_SCOPED_RAND_RESET_I(11, __VscopeHash, 1329795956477004815ull);
    vlSelf->top__DOT__vgc_inst__DOT__char_a_din = VL_SCOPED_RAND_RESET_I(8, __VscopeHash, 7467364520335578426ull);
    vlSelf->top__DOT__vgc_inst__DOT__char_a_we = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 5871338839350102588ull);
    vlSelf->top__DOT__vgc_inst__DOT__char_a_dout = VL_SCOPED_RAND_RESET_I(8, __VscopeHash, 14296877360434001632ull);
    vlSelf->top__DOT__vgc_inst__DOT__color_a_addr = VL_SCOPED_RAND_RESET_I(11, __VscopeHash, 848372372307534277ull);
    vlSelf->top__DOT__vgc_inst__DOT__color_a_din = VL_SCOPED_RAND_RESET_I(8, __VscopeHash, 15253097928878990278ull);
    vlSelf->top__DOT__vgc_inst__DOT__color_a_we = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 7855002795864510796ull);
    vlSelf->top__DOT__vgc_inst__DOT__color_a_dout = VL_SCOPED_RAND_RESET_I(8, __VscopeHash, 1763967950530126492ull);
    vlSelf->top__DOT__vgc_inst__DOT__font_a_addr = VL_SCOPED_RAND_RESET_I(11, __VscopeHash, 6553104218726064028ull);
    vlSelf->top__DOT__vgc_inst__DOT__font_a_dout = VL_SCOPED_RAND_RESET_I(8, __VscopeHash, 6499564535600757498ull);
    vlSelf->top__DOT__vgc_inst__DOT__char_b_dout = VL_SCOPED_RAND_RESET_I(8, __VscopeHash, 17244714293121842373ull);
    vlSelf->top__DOT__vgc_inst__DOT__color_b_dout = VL_SCOPED_RAND_RESET_I(8, __VscopeHash, 5229553048442855299ull);
    vlSelf->top__DOT__vgc_inst__DOT__font_b_dout = VL_SCOPED_RAND_RESET_I(8, __VscopeHash, 2867176083759603025ull);
    vlSelf->top__DOT__vgc_inst__DOT__gfx_a_addr = VL_SCOPED_RAND_RESET_I(16, __VscopeHash, 4945152106088475764ull);
    vlSelf->top__DOT__vgc_inst__DOT__gfx_a_din = VL_SCOPED_RAND_RESET_I(4, __VscopeHash, 18000709607282360042ull);
    vlSelf->top__DOT__vgc_inst__DOT__gfx_a_we = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 7292908414608541797ull);
    vlSelf->top__DOT__vgc_inst__DOT__gfx_a_dout = VL_SCOPED_RAND_RESET_I(4, __VscopeHash, 2794251248433243298ull);
    vlSelf->top__DOT__vgc_inst__DOT__gfx_b_dout = VL_SCOPED_RAND_RESET_I(4, __VscopeHash, 12453561196351551382ull);
    VL_SCOPED_RAND_RESET_W(512, vlSelf->top__DOT__vgc_inst__DOT__fio_name_flat, __VscopeHash, 13635742595777826841ull);
    vlSelf->top__DOT__vgc_inst__DOT____Vcellinp__artist_inst__cmd_x1 = 0;
    vlSelf->top__DOT__vgc_inst__DOT____Vcellinp__artist_inst__cmd_x0 = 0;
    vlSelf->top__DOT__vgc_inst__DOT__spr_a_addr = VL_SCOPED_RAND_RESET_I(11, __VscopeHash, 17229925462687911458ull);
    vlSelf->top__DOT__vgc_inst__DOT__spr_a_din = VL_SCOPED_RAND_RESET_I(8, __VscopeHash, 4941272075569249590ull);
    vlSelf->top__DOT__vgc_inst__DOT__spr_a_we = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 11508858112329708732ull);
    vlSelf->top__DOT__vgc_inst__DOT__spr_a_dout = VL_SCOPED_RAND_RESET_I(8, __VscopeHash, 11568894501849305584ull);
    for (int __Vi0 = 0; __Vi0 < 16; ++__Vi0) {
        vlSelf->top__DOT__vgc_inst__DOT__spr_x[__Vi0] = VL_SCOPED_RAND_RESET_I(9, __VscopeHash, 3525630943627754323ull);
    }
    for (int __Vi0 = 0; __Vi0 < 16; ++__Vi0) {
        vlSelf->top__DOT__vgc_inst__DOT__spr_y[__Vi0] = VL_SCOPED_RAND_RESET_I(8, __VscopeHash, 5796008776110150920ull);
    }
    for (int __Vi0 = 0; __Vi0 < 16; ++__Vi0) {
        vlSelf->top__DOT__vgc_inst__DOT__spr_enable[__Vi0] = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 5450130358760532424ull);
    }
    for (int __Vi0 = 0; __Vi0 < 16; ++__Vi0) {
        vlSelf->top__DOT__vgc_inst__DOT__spr_flip_h[__Vi0] = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 2053801081366311313ull);
    }
    for (int __Vi0 = 0; __Vi0 < 16; ++__Vi0) {
        vlSelf->top__DOT__vgc_inst__DOT__spr_flip_v[__Vi0] = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 9756698892608598665ull);
    }
    for (int __Vi0 = 0; __Vi0 < 16; ++__Vi0) {
        vlSelf->top__DOT__vgc_inst__DOT__spr_pri[__Vi0] = VL_SCOPED_RAND_RESET_I(2, __VscopeHash, 10625968600645325770ull);
    }
    for (int __Vi0 = 0; __Vi0 < 16; ++__Vi0) {
        vlSelf->top__DOT__vgc_inst__DOT__spr_shape[__Vi0] = VL_SCOPED_RAND_RESET_I(4, __VscopeHash, 11766514090340350603ull);
    }
    for (int __Vi0 = 0; __Vi0 < 16; ++__Vi0) {
        vlSelf->top__DOT__vgc_inst__DOT__spr_trans[__Vi0] = VL_SCOPED_RAND_RESET_I(4, __VscopeHash, 3063335376297485947ull);
    }
    VL_SCOPED_RAND_RESET_W(144, vlSelf->top__DOT__vgc_inst__DOT__spr_x_flat, __VscopeHash, 12888696106673753061ull);
    VL_SCOPED_RAND_RESET_W(128, vlSelf->top__DOT__vgc_inst__DOT__spr_y_flat, __VscopeHash, 4053977688923638790ull);
    vlSelf->top__DOT__vgc_inst__DOT__spr_enable_flat = VL_SCOPED_RAND_RESET_I(16, __VscopeHash, 14447771685646889290ull);
    vlSelf->top__DOT__vgc_inst__DOT__spr_flip_h_flat = VL_SCOPED_RAND_RESET_I(16, __VscopeHash, 10205258146289882581ull);
    vlSelf->top__DOT__vgc_inst__DOT__spr_flip_v_flat = VL_SCOPED_RAND_RESET_I(16, __VscopeHash, 11226596205401130975ull);
    vlSelf->top__DOT__vgc_inst__DOT__spr_pri_flat = VL_SCOPED_RAND_RESET_I(32, __VscopeHash, 7727561446582165707ull);
    vlSelf->top__DOT__vgc_inst__DOT__spr_shape_flat = VL_SCOPED_RAND_RESET_Q(64, __VscopeHash, 16916748059905668455ull);
    vlSelf->top__DOT__vgc_inst__DOT__spr_trans_flat = VL_SCOPED_RAND_RESET_Q(64, __VscopeHash, 12196438112702161272ull);
    for (int __Vi0 = 0; __Vi0 < 32; ++__Vi0) {
        vlSelf->top__DOT__vgc_inst__DOT__copper_pos[__Vi0] = VL_SCOPED_RAND_RESET_I(17, __VscopeHash, 6273658580735665441ull);
    }
    for (int __Vi0 = 0; __Vi0 < 32; ++__Vi0) {
        vlSelf->top__DOT__vgc_inst__DOT__copper_reg[__Vi0] = VL_SCOPED_RAND_RESET_I(8, __VscopeHash, 11945446827575079795ull);
    }
    for (int __Vi0 = 0; __Vi0 < 32; ++__Vi0) {
        vlSelf->top__DOT__vgc_inst__DOT__copper_val[__Vi0] = VL_SCOPED_RAND_RESET_I(8, __VscopeHash, 11484039803331668658ull);
    }
    vlSelf->top__DOT__vgc_inst__DOT__copper_count = VL_SCOPED_RAND_RESET_I(9, __VscopeHash, 14902926815119172938ull);
    vlSelf->top__DOT__vgc_inst__DOT__copper_index = VL_SCOPED_RAND_RESET_I(9, __VscopeHash, 17230886302175728807ull);
    vlSelf->top__DOT__vgc_inst__DOT__copper_enabled = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 14356838651262047076ull);
    for (int __Vi0 = 0; __Vi0 < 4096; ++__Vi0) {
        vlSelf->top__DOT__vgc_inst__DOT__copper_list_pos[__Vi0] = VL_SCOPED_RAND_RESET_I(17, __VscopeHash, 10987291808973769782ull);
    }
    for (int __Vi0 = 0; __Vi0 < 4096; ++__Vi0) {
        vlSelf->top__DOT__vgc_inst__DOT__copper_list_reg[__Vi0] = VL_SCOPED_RAND_RESET_I(8, __VscopeHash, 2340796189252858449ull);
    }
    for (int __Vi0 = 0; __Vi0 < 4096; ++__Vi0) {
        vlSelf->top__DOT__vgc_inst__DOT__copper_list_val[__Vi0] = VL_SCOPED_RAND_RESET_I(8, __VscopeHash, 9818969500871100024ull);
    }
    for (int __Vi0 = 0; __Vi0 < 128; ++__Vi0) {
        vlSelf->top__DOT__vgc_inst__DOT__copper_list_count[__Vi0] = VL_SCOPED_RAND_RESET_I(9, __VscopeHash, 8924829141550956560ull);
    }
    vlSelf->top__DOT__vgc_inst__DOT__copper_target_list = VL_SCOPED_RAND_RESET_I(7, __VscopeHash, 292732304025908135ull);
    vlSelf->top__DOT__vgc_inst__DOT__copper_active_list = VL_SCOPED_RAND_RESET_I(7, __VscopeHash, 7505171416879597094ull);
    vlSelf->top__DOT__vgc_inst__DOT__copper_pending_list = VL_SCOPED_RAND_RESET_I(7, __VscopeHash, 2042539641801815459ull);
    vlSelf->top__DOT__vgc_inst__DOT__copper_loading = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 15950090564233184386ull);
    vlSelf->top__DOT__vgc_inst__DOT__copper_load_idx = VL_SCOPED_RAND_RESET_I(9, __VscopeHash, 8174089754523841874ull);
    vlSelf->top__DOT__vgc_inst__DOT__copper_load_src = VL_SCOPED_RAND_RESET_I(7, __VscopeHash, 15978874466810526593ull);
    vlSelf->top__DOT__vgc_inst__DOT__copper_fire = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 18059199201016299732ull);
    vlSelf->top__DOT__vgc_inst__DOT__copper_fire_reg = VL_SCOPED_RAND_RESET_I(8, __VscopeHash, 13678372512039214229ull);
    vlSelf->top__DOT__vgc_inst__DOT__copper_fire_val = VL_SCOPED_RAND_RESET_I(8, __VscopeHash, 2178847549667406432ull);
    VL_SCOPED_RAND_RESET_W(544, vlSelf->top__DOT__vgc_inst__DOT__copper_pos_flat, __VscopeHash, 10036896197879600810ull);
    VL_SCOPED_RAND_RESET_W(256, vlSelf->top__DOT__vgc_inst__DOT__copper_reg_flat, __VscopeHash, 16041708979782664992ull);
    VL_SCOPED_RAND_RESET_W(256, vlSelf->top__DOT__vgc_inst__DOT__copper_val_flat, __VscopeHash, 2028372798500461372ull);
    for (int __Vi0 = 0; __Vi0 < 32; ++__Vi0) {
        vlSelf->top__DOT__vgc_inst__DOT__regs[__Vi0] = VL_SCOPED_RAND_RESET_I(8, __VscopeHash, 11003884235172198755ull);
    }
    vlSelf->top__DOT__vgc_inst__DOT__cursor_x = VL_SCOPED_RAND_RESET_I(7, __VscopeHash, 4922292800445943068ull);
    vlSelf->top__DOT__vgc_inst__DOT__cursor_y = VL_SCOPED_RAND_RESET_I(5, __VscopeHash, 2894972859692547860ull);
    vlSelf->top__DOT__vgc_inst__DOT__fg_color = VL_SCOPED_RAND_RESET_I(4, __VscopeHash, 13844641643093747749ull);
    vlSelf->top__DOT__vgc_inst__DOT__bg_color = VL_SCOPED_RAND_RESET_I(4, __VscopeHash, 1553403682278150336ull);
    vlSelf->top__DOT__vgc_inst__DOT__border_color = VL_SCOPED_RAND_RESET_I(4, __VscopeHash, 5179612022770402341ull);
    vlSelf->top__DOT__vgc_inst__DOT__gfx_color = VL_SCOPED_RAND_RESET_I(4, __VscopeHash, 16775930302260237695ull);
    vlSelf->top__DOT__vgc_inst__DOT__mode = VL_SCOPED_RAND_RESET_I(3, __VscopeHash, 10470405726398609093ull);
    vlSelf->top__DOT__vgc_inst__DOT__cursor_enable = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 11858595983984632465ull);
    vlSelf->top__DOT__vgc_inst__DOT__char_in_reg = VL_SCOPED_RAND_RESET_I(8, __VscopeHash, 13506278984315242004ull);
    vlSelf->top__DOT__vgc_inst__DOT__font_slot = VL_SCOPED_RAND_RESET_I(3, __VscopeHash, 6908780534930202618ull);
    vlSelf->top__DOT__vgc_inst__DOT__scroll_pending = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 2913400672515789644ull);
    vlSelf->top__DOT__vgc_inst__DOT__scroll_col = VL_SCOPED_RAND_RESET_I(7, __VscopeHash, 3703413112585759979ull);
    for (int __Vi0 = 0; __Vi0 < 64; ++__Vi0) {
        vlSelf->top__DOT__vgc_inst__DOT__fio_name[__Vi0] = VL_SCOPED_RAND_RESET_I(8, __VscopeHash, 11697907039282495248ull);
    }
    vlSelf->top__DOT__vgc_inst__DOT__fio_name_len = VL_SCOPED_RAND_RESET_I(6, __VscopeHash, 6634993440007203268ull);
    vlSelf->top__DOT__vgc_inst__DOT__cmd_busy = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 14426744459723185369ull);
    vlSelf->top__DOT__vgc_inst__DOT__cmd_op = VL_SCOPED_RAND_RESET_I(8, __VscopeHash, 12044860386946464373ull);
    vlSelf->top__DOT__vgc_inst__DOT__cmd_x = VL_SCOPED_RAND_RESET_I(10, __VscopeHash, 3720964225446871267ull);
    vlSelf->top__DOT__vgc_inst__DOT__cmd_y = VL_SCOPED_RAND_RESET_I(10, __VscopeHash, 5828144132756238096ull);
    vlSelf->top__DOT__vgc_inst__DOT__cmd_cx = VL_SCOPED_RAND_RESET_I(9, __VscopeHash, 1643995001836955740ull);
    vlSelf->top__DOT__vgc_inst__DOT__cmd_cy = VL_SCOPED_RAND_RESET_I(8, __VscopeHash, 15333978331593976762ull);
    vlSelf->top__DOT__vgc_inst__DOT__sprrow_count = VL_SCOPED_RAND_RESET_I(3, __VscopeHash, 16662545896393356064ull);
    for (int __Vi0 = 0; __Vi0 < 8; ++__Vi0) {
        vlSelf->top__DOT__vgc_inst__DOT__sprrow_data[__Vi0] = VL_SCOPED_RAND_RESET_I(8, __VscopeHash, 604830978026147186ull);
    }
    vlSelf->top__DOT__vgc_inst__DOT__sprrow_spr = VL_SCOPED_RAND_RESET_I(4, __VscopeHash, 6314005667588616723ull);
    vlSelf->top__DOT__vgc_inst__DOT__sprrow_row = VL_SCOPED_RAND_RESET_I(4, __VscopeHash, 14314119657771325322ull);
    vlSelf->top__DOT__vgc_inst__DOT__sprcopy_phase = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 14922975223422643933ull);
    vlSelf->top__DOT__vgc_inst__DOT__memread_pending = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 9473167400625733032ull);
    vlSelf->top__DOT__vgc_inst__DOT__memread_space = VL_SCOPED_RAND_RESET_I(3, __VscopeHash, 16664595760517234438ull);
    vlSelf->top__DOT__vgc_inst__DOT__cmd_char_addr = VL_SCOPED_RAND_RESET_I(11, __VscopeHash, 15460989922248672553ull);
    vlSelf->top__DOT__vgc_inst__DOT__cmd_char_din = VL_SCOPED_RAND_RESET_I(8, __VscopeHash, 13274401642933606462ull);
    vlSelf->top__DOT__vgc_inst__DOT__cmd_char_we = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 15931951818772575868ull);
    vlSelf->top__DOT__vgc_inst__DOT__cmd_color_addr = VL_SCOPED_RAND_RESET_I(11, __VscopeHash, 8966641566466105452ull);
    vlSelf->top__DOT__vgc_inst__DOT__cmd_color_din = VL_SCOPED_RAND_RESET_I(8, __VscopeHash, 410890156314966910ull);
    vlSelf->top__DOT__vgc_inst__DOT__cmd_color_we = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 6657339085784938522ull);
    vlSelf->top__DOT__vgc_inst__DOT__cmd_gfx_addr = VL_SCOPED_RAND_RESET_I(16, __VscopeHash, 4994903704657317823ull);
    vlSelf->top__DOT__vgc_inst__DOT__cmd_gfx_din = VL_SCOPED_RAND_RESET_I(4, __VscopeHash, 273355153901695558ull);
    vlSelf->top__DOT__vgc_inst__DOT__cmd_gfx_we = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 3044740705530994662ull);
    vlSelf->top__DOT__vgc_inst__DOT__cmd_gfx_re = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 5741938000248121370ull);
    vlSelf->top__DOT__vgc_inst__DOT__cmd_spr_addr = VL_SCOPED_RAND_RESET_I(11, __VscopeHash, 888973537386143219ull);
    vlSelf->top__DOT__vgc_inst__DOT__cmd_spr_din = VL_SCOPED_RAND_RESET_I(8, __VscopeHash, 14753903937277422032ull);
    vlSelf->top__DOT__vgc_inst__DOT__cmd_spr_we = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 2318381443488016701ull);
    vlSelf->top__DOT__vgc_inst__DOT__cmd_spr_re = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 7698768496888752363ull);
    vlSelf->top__DOT__vgc_inst__DOT__artist_cmd_valid = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 7736126444901957484ull);
    vlSelf->top__DOT__vgc_inst__DOT__artist_cmd_code = VL_SCOPED_RAND_RESET_I(8, __VscopeHash, 14761074978268165020ull);
    vlSelf->top__DOT__vgc_inst__DOT__artist_gfx_addr = VL_SCOPED_RAND_RESET_I(16, __VscopeHash, 17500176552923166564ull);
    vlSelf->top__DOT__vgc_inst__DOT__artist_gfx_wdata = VL_SCOPED_RAND_RESET_I(4, __VscopeHash, 17251978692619753798ull);
    vlSelf->top__DOT__vgc_inst__DOT__artist_gfx_we = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 10605670003383739056ull);
    vlSelf->top__DOT__vgc_inst__DOT__artist_gfx_raddr = VL_SCOPED_RAND_RESET_I(16, __VscopeHash, 11103610023898256303ull);
    vlSelf->top__DOT__vgc_inst__DOT__artist_gfx_re = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 16611193647141729665ull);
    vlSelf->top__DOT__vgc_inst__DOT__artist_font_addr = VL_SCOPED_RAND_RESET_I(11, __VscopeHash, 15965629309261733347ull);
    vlSelf->top__DOT__vgc_inst__DOT__artist_font_re = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 669480287738718755ull);
    vlSelf->top__DOT__vgc_inst__DOT__artist_busy = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 14963199831128233931ull);
    vlSelf->top__DOT__vgc_inst__DOT__vgc_reg_sel = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 16940596785735907027ull);
    vlSelf->top__DOT__vgc_inst__DOT__spr_reg_sel = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 14544170901387152661ull);
    vlSelf->top__DOT__vgc_inst__DOT__char_ram_sel = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 13344466941136171116ull);
    vlSelf->top__DOT__vgc_inst__DOT__color_ram_sel = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 9819832744737651264ull);
    vlSelf->top__DOT__vgc_inst__DOT__tile_reg_sel = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 2305656673107131713ull);
    vlSelf->top__DOT__vgc_inst__DOT__dbg_char_sel = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 6134542423536004730ull);
    vlSelf->top__DOT__vgc_inst__DOT__dbg_color_sel = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 17933616731144665550ull);
    vlSelf->top__DOT__vgc_inst__DOT__dbg_vgc_sel = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 16096157615218818504ull);
    vlSelf->top__DOT__vgc_inst__DOT__dbg_rd_pending = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 3397197407770880655ull);
    vlSelf->top__DOT__vgc_inst__DOT__dbg_rd_source = VL_SCOPED_RAND_RESET_I(2, __VscopeHash, 3771125736445697252ull);
    vlSelf->top__DOT__vgc_inst__DOT__dbg_rd_latch = VL_SCOPED_RAND_RESET_I(8, __VscopeHash, 4626402248401355374ull);
    vlSelf->top__DOT__vgc_inst__DOT__blt_rd_pending = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 8491118014287199687ull);
    vlSelf->top__DOT__vgc_inst__DOT__blt_rd_space = VL_SCOPED_RAND_RESET_I(3, __VscopeHash, 4042605049953721698ull);
    vlSelf->top__DOT__vgc_inst__DOT__blt_rd_latch = VL_SCOPED_RAND_RESET_I(8, __VscopeHash, 2715704092618707192ull);
    vlSelf->top__DOT__vgc_inst__DOT__tile_blt_rdata = VL_SCOPED_RAND_RESET_I(8, __VscopeHash, 17990333416369930646ull);
    vlSelf->top__DOT__vgc_inst__DOT__vgc_tile_addr = VL_SCOPED_RAND_RESET_I(15, __VscopeHash, 17950630997073558319ull);
    vlSelf->top__DOT__vgc_inst__DOT__vgc_tile_wdata = VL_SCOPED_RAND_RESET_I(8, __VscopeHash, 13961391969759483787ull);
    vlSelf->top__DOT__vgc_inst__DOT__vgc_tile_we = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 1990674681049679603ull);
    vlSelf->top__DOT__vgc_inst__DOT__vgc_tile_re = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 308087680378802699ull);
    vlSelf->top__DOT__vgc_inst__DOT__tile_blt_we = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 682718729040305023ull);
    vlSelf->top__DOT__vgc_inst__DOT__tile_blt_re = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 4829084748099158467ull);
    vlSelf->top__DOT__vgc_inst__DOT__pixel_color = VL_SCOPED_RAND_RESET_I(12, __VscopeHash, 12578114064701120044ull);
    vlSelf->top__DOT__vgc_inst__DOT__tile_rgb_d1 = VL_SCOPED_RAND_RESET_I(12, __VscopeHash, 11211761460720262913ull);
    vlSelf->top__DOT__vgc_inst__DOT__tile_rgb_d2 = VL_SCOPED_RAND_RESET_I(12, __VscopeHash, 385548907961394974ull);
    vlSelf->top__DOT__vgc_inst__DOT__tile_opaque_d1 = VL_SCOPED_RAND_RESET_I(2, __VscopeHash, 2038544467818138457ull);
    vlSelf->top__DOT__vgc_inst__DOT__tile_opaque_d2 = VL_SCOPED_RAND_RESET_I(2, __VscopeHash, 11186890319231960270ull);
    vlSelf->top__DOT__vgc_inst__DOT__blink_count = VL_SCOPED_RAND_RESET_I(5, __VscopeHash, 998079296910595332ull);
    vlSelf->top__DOT__vgc_inst__DOT__cursor_blink = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 16237730840550051681ull);
    vlSelf->top__DOT__vgc_inst__DOT__cursor_here = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 15751776894645595649ull);
    vlSelf->top__DOT__vgc_inst__DOT__unnamedblk14__DOT__i = 0;
    vlSelf->top__DOT__vgc_inst__DOT__text_inst__DOT__char_b_addr = VL_SCOPED_RAND_RESET_I(11, __VscopeHash, 12561362461102742392ull);
    vlSelf->top__DOT__vgc_inst__DOT__text_inst__DOT__char_mem__DOT____Vlvbound_hcad4ec09__0 = 0;
    vlSelf->top__DOT__vgc_inst__DOT__text_inst__DOT__char_mem__DOT____Vlvbound_h3482bd38__0 = 0;
    for (int __Vi0 = 0; __Vi0 < 2000; ++__Vi0) {
        vlSelf->top__DOT__vgc_inst__DOT__text_inst__DOT__char_mem__DOT__mem[__Vi0] = VL_SCOPED_RAND_RESET_I(8, __VscopeHash, 11373356693727126544ull);
    }
    vlSelf->top__DOT__vgc_inst__DOT__text_inst__DOT__color_mem__DOT____Vlvbound_hcad4ec09__0 = 0;
    vlSelf->top__DOT__vgc_inst__DOT__text_inst__DOT__color_mem__DOT____Vlvbound_h3482bd38__0 = 0;
    for (int __Vi0 = 0; __Vi0 < 2000; ++__Vi0) {
        vlSelf->top__DOT__vgc_inst__DOT__text_inst__DOT__color_mem__DOT__mem[__Vi0] = VL_SCOPED_RAND_RESET_I(8, __VscopeHash, 11842015586005082850ull);
    }
    for (int __Vi0 = 0; __Vi0 < 2048; ++__Vi0) {
        vlSelf->top__DOT__vgc_inst__DOT__text_inst__DOT__font_mem__DOT__mem[__Vi0] = VL_SCOPED_RAND_RESET_I(8, __VscopeHash, 11175090545138279814ull);
    }
    vlSelf->top__DOT__vgc_inst__DOT__gfx_inst__DOT__gfx_b_addr = VL_SCOPED_RAND_RESET_I(16, __VscopeHash, 4618343915872205366ull);
    vlSelf->top__DOT__vgc_inst__DOT__gfx_inst__DOT__gfx_mem__DOT____Vlvbound_h0a4ce44c__0 = 0;
    vlSelf->top__DOT__vgc_inst__DOT__gfx_inst__DOT__gfx_mem__DOT____Vlvbound_h75f189da__0 = 0;
    for (int __Vi0 = 0; __Vi0 < 64000; ++__Vi0) {
        vlSelf->top__DOT__vgc_inst__DOT__gfx_inst__DOT__gfx_mem__DOT__mem[__Vi0] = VL_SCOPED_RAND_RESET_I(4, __VscopeHash, 16109264844054766091ull);
    }
    vlSelf->top__DOT__vgc_inst__DOT__artist_inst__DOT__op = VL_SCOPED_RAND_RESET_I(8, __VscopeHash, 15502653448327414633ull);
    vlSelf->top__DOT__vgc_inst__DOT__artist_inst__DOT__x = VL_SCOPED_RAND_RESET_I(10, __VscopeHash, 7555362913801053027ull);
    vlSelf->top__DOT__vgc_inst__DOT__artist_inst__DOT__y = VL_SCOPED_RAND_RESET_I(10, __VscopeHash, 11399007874015029200ull);
    vlSelf->top__DOT__vgc_inst__DOT__artist_inst__DOT__x2 = VL_SCOPED_RAND_RESET_I(10, __VscopeHash, 16054362866412253621ull);
    vlSelf->top__DOT__vgc_inst__DOT__artist_inst__DOT__y2 = VL_SCOPED_RAND_RESET_I(10, __VscopeHash, 13069525319091055676ull);
    vlSelf->top__DOT__vgc_inst__DOT__artist_inst__DOT__dx = VL_SCOPED_RAND_RESET_I(10, __VscopeHash, 8347105153158947136ull);
    vlSelf->top__DOT__vgc_inst__DOT__artist_inst__DOT__dy = VL_SCOPED_RAND_RESET_I(10, __VscopeHash, 3459305269091724345ull);
    vlSelf->top__DOT__vgc_inst__DOT__artist_inst__DOT__err = VL_SCOPED_RAND_RESET_I(10, __VscopeHash, 7402295148181317797ull);
    vlSelf->top__DOT__vgc_inst__DOT__artist_inst__DOT__sx = VL_SCOPED_RAND_RESET_I(10, __VscopeHash, 16538348813252117161ull);
    vlSelf->top__DOT__vgc_inst__DOT__artist_inst__DOT__sy = VL_SCOPED_RAND_RESET_I(10, __VscopeHash, 4523836305091265889ull);
    vlSelf->top__DOT__vgc_inst__DOT__artist_inst__DOT__cx = VL_SCOPED_RAND_RESET_I(9, __VscopeHash, 18410820144779398875ull);
    vlSelf->top__DOT__vgc_inst__DOT__artist_inst__DOT__cy = VL_SCOPED_RAND_RESET_I(8, __VscopeHash, 5558831181849454433ull);
    vlSelf->top__DOT__vgc_inst__DOT__artist_inst__DOT__color = VL_SCOPED_RAND_RESET_I(4, __VscopeHash, 8457449048200485307ull);
    vlSelf->top__DOT__vgc_inst__DOT__artist_inst__DOT__octant_count = VL_SCOPED_RAND_RESET_I(3, __VscopeHash, 17588164767413792441ull);
    vlSelf->top__DOT__vgc_inst__DOT__artist_inst__DOT__fs_a_addr = VL_SCOPED_RAND_RESET_I(9, __VscopeHash, 3592695942114520672ull);
    vlSelf->top__DOT__vgc_inst__DOT__artist_inst__DOT__fs_a_din = VL_SCOPED_RAND_RESET_I(17, __VscopeHash, 16160204151863728858ull);
    vlSelf->top__DOT__vgc_inst__DOT__artist_inst__DOT__fs_a_we = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 17532154459570411376ull);
    vlSelf->top__DOT__vgc_inst__DOT__artist_inst__DOT__fs_b_addr = VL_SCOPED_RAND_RESET_I(9, __VscopeHash, 12374255773748599210ull);
    vlSelf->top__DOT__vgc_inst__DOT__artist_inst__DOT__fs_b_dout = VL_SCOPED_RAND_RESET_I(17, __VscopeHash, 7256837600593685552ull);
    vlSelf->top__DOT__vgc_inst__DOT__artist_inst__DOT__fill_sp = VL_SCOPED_RAND_RESET_I(10, __VscopeHash, 5919515605743233982ull);
    vlSelf->top__DOT__vgc_inst__DOT__artist_inst__DOT__fill_target = VL_SCOPED_RAND_RESET_I(4, __VscopeHash, 2680460476161381236ull);
    vlSelf->top__DOT__vgc_inst__DOT__artist_inst__DOT__paint_phase = VL_SCOPED_RAND_RESET_I(3, __VscopeHash, 11721612562569401722ull);
    vlSelf->top__DOT__vgc_inst__DOT__artist_inst__DOT__paint_init = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 11814347283578677441ull);
    vlSelf->top__DOT__vgc_inst__DOT__artist_inst__DOT__push_count = VL_SCOPED_RAND_RESET_I(2, __VscopeHash, 2228952425143819219ull);
    for (int __Vi0 = 0; __Vi0 < 4; ++__Vi0) {
        vlSelf->top__DOT__vgc_inst__DOT__artist_inst__DOT__push_neighbors[__Vi0] = VL_SCOPED_RAND_RESET_I(17, __VscopeHash, 6610937749308406277ull);
    }
    vlSelf->top__DOT__vgc_inst__DOT__artist_inst__DOT__gt_char_idx = VL_SCOPED_RAND_RESET_I(6, __VscopeHash, 9588105791377982067ull);
    vlSelf->top__DOT__vgc_inst__DOT__artist_inst__DOT__gt_len = VL_SCOPED_RAND_RESET_I(6, __VscopeHash, 14896283518978653628ull);
    vlSelf->top__DOT__vgc_inst__DOT__artist_inst__DOT__gt_font_row = VL_SCOPED_RAND_RESET_I(3, __VscopeHash, 7466847306228118856ull);
    vlSelf->top__DOT__vgc_inst__DOT__artist_inst__DOT__gt_font_col = VL_SCOPED_RAND_RESET_I(3, __VscopeHash, 16003535665835751547ull);
    vlSelf->top__DOT__vgc_inst__DOT__artist_inst__DOT__gt_scale = VL_SCOPED_RAND_RESET_I(8, __VscopeHash, 6210443102662527300ull);
    vlSelf->top__DOT__vgc_inst__DOT__artist_inst__DOT__gt_scale_x = VL_SCOPED_RAND_RESET_I(8, __VscopeHash, 2891549219643733387ull);
    vlSelf->top__DOT__vgc_inst__DOT__artist_inst__DOT__gt_scale_y = VL_SCOPED_RAND_RESET_I(8, __VscopeHash, 9215712611901008488ull);
    vlSelf->top__DOT__vgc_inst__DOT__artist_inst__DOT__gt_pen_x = VL_SCOPED_RAND_RESET_I(9, __VscopeHash, 13841114331696981734ull);
    vlSelf->top__DOT__vgc_inst__DOT__artist_inst__DOT__gt_start_y = VL_SCOPED_RAND_RESET_I(8, __VscopeHash, 1816453929159996589ull);
    vlSelf->top__DOT__vgc_inst__DOT__artist_inst__DOT__gt_cur_char = VL_SCOPED_RAND_RESET_I(8, __VscopeHash, 9829927504870956379ull);
    vlSelf->top__DOT__vgc_inst__DOT__artist_inst__DOT__gt_font_byte = VL_SCOPED_RAND_RESET_I(8, __VscopeHash, 2157042414475681321ull);
    vlSelf->top__DOT__vgc_inst__DOT__artist_inst__DOT__gt_font_pending = VL_SCOPED_RAND_RESET_I(2, __VscopeHash, 2540973748780031630ull);
    for (int __Vi0 = 0; __Vi0 < 512; ++__Vi0) {
        vlSelf->top__DOT__vgc_inst__DOT__artist_inst__DOT__fill_stack_mem__DOT__mem[__Vi0] = VL_SCOPED_RAND_RESET_I(17, __VscopeHash, 14441317695613214766ull);
    }
    for (int __Vi0 = 0; __Vi0 < 16; ++__Vi0) {
        vlSelf->top__DOT__vgc_inst__DOT__sprite_inst__DOT__spr_x[__Vi0] = VL_SCOPED_RAND_RESET_I(9, __VscopeHash, 17081936418927663102ull);
    }
    for (int __Vi0 = 0; __Vi0 < 16; ++__Vi0) {
        vlSelf->top__DOT__vgc_inst__DOT__sprite_inst__DOT__spr_y[__Vi0] = VL_SCOPED_RAND_RESET_I(8, __VscopeHash, 14036676292949921591ull);
    }
    for (int __Vi0 = 0; __Vi0 < 16; ++__Vi0) {
        vlSelf->top__DOT__vgc_inst__DOT__sprite_inst__DOT__spr_enable[__Vi0] = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 4484230244168095424ull);
    }
    for (int __Vi0 = 0; __Vi0 < 16; ++__Vi0) {
        vlSelf->top__DOT__vgc_inst__DOT__sprite_inst__DOT__spr_flip_h[__Vi0] = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 8155109596430938228ull);
    }
    for (int __Vi0 = 0; __Vi0 < 16; ++__Vi0) {
        vlSelf->top__DOT__vgc_inst__DOT__sprite_inst__DOT__spr_flip_v[__Vi0] = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 3763382554802703654ull);
    }
    for (int __Vi0 = 0; __Vi0 < 16; ++__Vi0) {
        vlSelf->top__DOT__vgc_inst__DOT__sprite_inst__DOT__spr_pri[__Vi0] = VL_SCOPED_RAND_RESET_I(2, __VscopeHash, 14551014822489943815ull);
    }
    for (int __Vi0 = 0; __Vi0 < 16; ++__Vi0) {
        vlSelf->top__DOT__vgc_inst__DOT__sprite_inst__DOT__spr_shape[__Vi0] = VL_SCOPED_RAND_RESET_I(4, __VscopeHash, 15501489280574745570ull);
    }
    for (int __Vi0 = 0; __Vi0 < 16; ++__Vi0) {
        vlSelf->top__DOT__vgc_inst__DOT__sprite_inst__DOT__spr_trans[__Vi0] = VL_SCOPED_RAND_RESET_I(4, __VscopeHash, 9197522743716509346ull);
    }
    vlSelf->top__DOT__vgc_inst__DOT__sprite_inst__DOT__spr_b_addr = VL_SCOPED_RAND_RESET_I(11, __VscopeHash, 8950710499516000322ull);
    vlSelf->top__DOT__vgc_inst__DOT__sprite_inst__DOT__spr_b_dout = VL_SCOPED_RAND_RESET_I(8, __VscopeHash, 14340781037573392276ull);
    vlSelf->top__DOT__vgc_inst__DOT__sprite_inst__DOT__slb_a_addr = VL_SCOPED_RAND_RESET_I(9, __VscopeHash, 14599121677829492191ull);
    vlSelf->top__DOT__vgc_inst__DOT__sprite_inst__DOT__slb_a_din = VL_SCOPED_RAND_RESET_I(7, __VscopeHash, 18034729647714800345ull);
    vlSelf->top__DOT__vgc_inst__DOT__sprite_inst__DOT__slb_a_we = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 2417619510496661378ull);
    vlSelf->top__DOT__vgc_inst__DOT__sprite_inst__DOT__slb_b_dout = VL_SCOPED_RAND_RESET_I(7, __VscopeHash, 6709289418850269843ull);
    vlSelf->top__DOT__vgc_inst__DOT__sprite_inst__DOT__spr_eval_state = VL_SCOPED_RAND_RESET_I(3, __VscopeHash, 1277517206028420814ull);
    vlSelf->top__DOT__vgc_inst__DOT__sprite_inst__DOT__spr_eval_idx = VL_SCOPED_RAND_RESET_I(4, __VscopeHash, 14665655202930693355ull);
    vlSelf->top__DOT__vgc_inst__DOT__sprite_inst__DOT__spr_clear_x = VL_SCOPED_RAND_RESET_I(9, __VscopeHash, 10686490541055778065ull);
    vlSelf->top__DOT__vgc_inst__DOT__sprite_inst__DOT__spr_read_byte = VL_SCOPED_RAND_RESET_I(3, __VscopeHash, 7305092435786259870ull);
    for (int __Vi0 = 0; __Vi0 < 8; ++__Vi0) {
        vlSelf->top__DOT__vgc_inst__DOT__sprite_inst__DOT__spr_row_data[__Vi0] = VL_SCOPED_RAND_RESET_I(8, __VscopeHash, 8752568706077893970ull);
    }
    vlSelf->top__DOT__vgc_inst__DOT__sprite_inst__DOT__spr_eval_y_line = VL_SCOPED_RAND_RESET_I(8, __VscopeHash, 9326626050393613953ull);
    vlSelf->top__DOT__vgc_inst__DOT__sprite_inst__DOT__spr_eval_flip_h = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 6318072119774671338ull);
    vlSelf->top__DOT__vgc_inst__DOT__sprite_inst__DOT__spr_eval_flip_v = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 9918767673660377001ull);
    vlSelf->top__DOT__vgc_inst__DOT__sprite_inst__DOT__spr_eval_pri_r = VL_SCOPED_RAND_RESET_I(2, __VscopeHash, 10840021611081670635ull);
    vlSelf->top__DOT__vgc_inst__DOT__sprite_inst__DOT__spr_eval_x_r = VL_SCOPED_RAND_RESET_I(9, __VscopeHash, 14545728613609204015ull);
    vlSelf->top__DOT__vgc_inst__DOT__sprite_inst__DOT__spr_eval_shape_r = VL_SCOPED_RAND_RESET_I(4, __VscopeHash, 4614576084197544862ull);
    vlSelf->top__DOT__vgc_inst__DOT__sprite_inst__DOT__spr_eval_trans_r = VL_SCOPED_RAND_RESET_I(4, __VscopeHash, 8893953750493089012ull);
    vlSelf->top__DOT__vgc_inst__DOT__sprite_inst__DOT__spr_next_scanline = VL_SCOPED_RAND_RESET_I(8, __VscopeHash, 1138691567822274869ull);
    vlSelf->top__DOT__vgc_inst__DOT__sprite_inst__DOT__spr_write_px = VL_SCOPED_RAND_RESET_I(5, __VscopeHash, 8388783852666958067ull);
    for (int __Vi0 = 0; __Vi0 < 2048; ++__Vi0) {
        vlSelf->top__DOT__vgc_inst__DOT__sprite_inst__DOT__spr_mem__DOT__mem[__Vi0] = VL_SCOPED_RAND_RESET_I(8, __VscopeHash, 8448544654448003864ull);
    }
    vlSelf->top__DOT__vgc_inst__DOT__sprite_inst__DOT__scanline_buf__DOT____Vlvbound_h7b43b104__0 = 0;
    vlSelf->top__DOT__vgc_inst__DOT__sprite_inst__DOT__scanline_buf__DOT____Vlvbound_he4591f03__0 = 0;
    for (int __Vi0 = 0; __Vi0 < 320; ++__Vi0) {
        vlSelf->top__DOT__vgc_inst__DOT__sprite_inst__DOT__scanline_buf__DOT__mem[__Vi0] = VL_SCOPED_RAND_RESET_I(7, __VscopeHash, 13867094749067534178ull);
    }
    for (int __Vi0 = 0; __Vi0 < 32; ++__Vi0) {
        vlSelf->top__DOT__vgc_inst__DOT__copper_inst__DOT__copper_pos[__Vi0] = VL_SCOPED_RAND_RESET_I(17, __VscopeHash, 3348886926486375717ull);
    }
    for (int __Vi0 = 0; __Vi0 < 32; ++__Vi0) {
        vlSelf->top__DOT__vgc_inst__DOT__copper_inst__DOT__copper_reg[__Vi0] = VL_SCOPED_RAND_RESET_I(8, __VscopeHash, 4571583721583468171ull);
    }
    for (int __Vi0 = 0; __Vi0 < 32; ++__Vi0) {
        vlSelf->top__DOT__vgc_inst__DOT__copper_inst__DOT__copper_val[__Vi0] = VL_SCOPED_RAND_RESET_I(8, __VscopeHash, 3814702806909130224ull);
    }
    vlSelf->top__DOT__vgc_inst__DOT__tile_inst__DOT____Vlvbound_ha768cd16__0 = 0;
    vlSelf->top__DOT__vgc_inst__DOT__tile_inst__DOT____Vlvbound_h67f0dfac__0 = 0;
    vlSelf->top__DOT__vgc_inst__DOT__tile_inst__DOT____Vlvbound_hbbfa6b2d__0 = 0;
    for (int __Vi0 = 0; __Vi0 < 32; ++__Vi0) {
        vlSelf->top__DOT__vgc_inst__DOT__tile_inst__DOT__tregs[__Vi0] = VL_SCOPED_RAND_RESET_I(8, __VscopeHash, 475624060206381906ull);
    }
    vlSelf->top__DOT__vgc_inst__DOT__tile_inst__DOT__tile_size16 = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 2110614880053610344ull);
    vlSelf->top__DOT__vgc_inst__DOT__tile_inst__DOT__mirror_mode = VL_SCOPED_RAND_RESET_I(2, __VscopeHash, 12588063867732978501ull);
    vlSelf->top__DOT__vgc_inst__DOT__tile_inst__DOT__trans_color = VL_SCOPED_RAND_RESET_I(4, __VscopeHash, 13533350438607619348ull);
    vlSelf->top__DOT__vgc_inst__DOT__tile_inst__DOT__scroll_x = VL_SCOPED_RAND_RESET_I(16, __VscopeHash, 18163330331616261658ull);
    vlSelf->top__DOT__vgc_inst__DOT__tile_inst__DOT__scroll_y = VL_SCOPED_RAND_RESET_I(16, __VscopeHash, 4788256062563577056ull);
    for (int __Vi0 = 0; __Vi0 < 25; ++__Vi0) {
        vlSelf->top__DOT__vgc_inst__DOT__tile_inst__DOT__col_buffer[__Vi0] = VL_SCOPED_RAND_RESET_I(8, __VscopeHash, 18363267046953132205ull);
    }
    vlSelf->top__DOT__vgc_inst__DOT__tile_inst__DOT__td_addr_a = VL_SCOPED_RAND_RESET_I(15, __VscopeHash, 8382306000872722624ull);
    vlSelf->top__DOT__vgc_inst__DOT__tile_inst__DOT__td_din_a = VL_SCOPED_RAND_RESET_I(8, __VscopeHash, 1805843011321880737ull);
    vlSelf->top__DOT__vgc_inst__DOT__tile_inst__DOT__td_we_a = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 11067633778318281048ull);
    vlSelf->top__DOT__vgc_inst__DOT__tile_inst__DOT__td_dout_a = VL_SCOPED_RAND_RESET_I(8, __VscopeHash, 1044477563585518267ull);
    vlSelf->top__DOT__vgc_inst__DOT__tile_inst__DOT__td_dout_b = VL_SCOPED_RAND_RESET_I(8, __VscopeHash, 8993565326544630772ull);
    vlSelf->top__DOT__vgc_inst__DOT__tile_inst__DOT__nt_addr_a = VL_SCOPED_RAND_RESET_I(12, __VscopeHash, 4840342496738521626ull);
    vlSelf->top__DOT__vgc_inst__DOT__tile_inst__DOT__nt_din_a = VL_SCOPED_RAND_RESET_I(8, __VscopeHash, 4122197320584415602ull);
    vlSelf->top__DOT__vgc_inst__DOT__tile_inst__DOT__nt_we_a = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 6088860960688102816ull);
    vlSelf->top__DOT__vgc_inst__DOT__tile_inst__DOT__nt_addr_b = VL_SCOPED_RAND_RESET_I(12, __VscopeHash, 10443221745212283885ull);
    vlSelf->top__DOT__vgc_inst__DOT__tile_inst__DOT__nt_dout_b = VL_SCOPED_RAND_RESET_I(8, __VscopeHash, 18030504754410556922ull);
    vlSelf->top__DOT__vgc_inst__DOT__tile_inst__DOT__at_addr_a = VL_SCOPED_RAND_RESET_I(12, __VscopeHash, 7756396891881611223ull);
    vlSelf->top__DOT__vgc_inst__DOT__tile_inst__DOT__at_din_a = VL_SCOPED_RAND_RESET_I(8, __VscopeHash, 16153768245435908444ull);
    vlSelf->top__DOT__vgc_inst__DOT__tile_inst__DOT__at_we_a = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 7860370514466620850ull);
    vlSelf->top__DOT__vgc_inst__DOT__tile_inst__DOT__at_dout_b = VL_SCOPED_RAND_RESET_I(8, __VscopeHash, 8376901819424725933ull);
    vlSelf->top__DOT__vgc_inst__DOT__tile_inst__DOT__pr_addr_a = VL_SCOPED_RAND_RESET_I(8, __VscopeHash, 9756279674319895570ull);
    vlSelf->top__DOT__vgc_inst__DOT__tile_inst__DOT__pr_din_a = VL_SCOPED_RAND_RESET_I(12, __VscopeHash, 13075214780260645759ull);
    vlSelf->top__DOT__vgc_inst__DOT__tile_inst__DOT__pr_we_a = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 534601164984468305ull);
    vlSelf->top__DOT__vgc_inst__DOT__tile_inst__DOT__pr_dout_b = VL_SCOPED_RAND_RESET_I(12, __VscopeHash, 17425875872516092908ull);
    vlSelf->top__DOT__vgc_inst__DOT__tile_inst__DOT__dma_state = VL_SCOPED_RAND_RESET_I(5, __VscopeHash, 3851128621443153532ull);
    vlSelf->top__DOT__vgc_inst__DOT__tile_inst__DOT__dma_src = VL_SCOPED_RAND_RESET_I(16, __VscopeHash, 782760209955877009ull);
    vlSelf->top__DOT__vgc_inst__DOT__tile_inst__DOT__dma_count = VL_SCOPED_RAND_RESET_I(16, __VscopeHash, 6129509713003057518ull);
    vlSelf->top__DOT__vgc_inst__DOT__tile_inst__DOT__dma_dst = VL_SCOPED_RAND_RESET_I(16, __VscopeHash, 10041212104300692835ull);
    vlSelf->top__DOT__vgc_inst__DOT__tile_inst__DOT__dma_nt = VL_SCOPED_RAND_RESET_I(2, __VscopeHash, 4247030028071847542ull);
    vlSelf->top__DOT__vgc_inst__DOT__tile_inst__DOT__dma_col_row = VL_SCOPED_RAND_RESET_I(6, __VscopeHash, 9254692316727579471ull);
    vlSelf->top__DOT__vgc_inst__DOT__tile_inst__DOT__dma_col_idx = VL_SCOPED_RAND_RESET_I(5, __VscopeHash, 12830291992961023920ull);
    vlSelf->top__DOT__vgc_inst__DOT__tile_inst__DOT__dma_data_valid = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 5073711299891437012ull);
    vlSelf->top__DOT__vgc_inst__DOT__tile_inst__DOT__dma_pal_phase = VL_SCOPED_RAND_RESET_I(8, __VscopeHash, 7894155145523769630ull);
    vlSelf->top__DOT__vgc_inst__DOT__tile_inst__DOT__dma_pal_r = VL_SCOPED_RAND_RESET_I(8, __VscopeHash, 7634965297911791415ull);
    vlSelf->top__DOT__vgc_inst__DOT__tile_inst__DOT__dma_pal_g = VL_SCOPED_RAND_RESET_I(8, __VscopeHash, 1399843149186698177ull);
    vlSelf->top__DOT__vgc_inst__DOT__tile_inst__DOT__dma_pal_sub = VL_SCOPED_RAND_RESET_I(4, __VscopeHash, 12628950741653233472ull);
    vlSelf->top__DOT__vgc_inst__DOT__tile_inst__DOT__dma_pal_cidx = VL_SCOPED_RAND_RESET_I(4, __VscopeHash, 632250123168251287ull);
    vlSelf->top__DOT__vgc_inst__DOT__tile_inst__DOT__dma_fill_val = VL_SCOPED_RAND_RESET_I(8, __VscopeHash, 17603324644241459411ull);
    vlSelf->top__DOT__vgc_inst__DOT__tile_inst__DOT__dma_fill_addr = VL_SCOPED_RAND_RESET_I(12, __VscopeHash, 592155201086315660ull);
    vlSelf->top__DOT__vgc_inst__DOT__tile_inst__DOT__cmd_nt_we = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 10700737906566824335ull);
    vlSelf->top__DOT__vgc_inst__DOT__tile_inst__DOT__cmd_at_we = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 3477798842704476615ull);
    vlSelf->top__DOT__vgc_inst__DOT__tile_inst__DOT__cmd_pal_we = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 10084862975220322239ull);
    vlSelf->top__DOT__vgc_inst__DOT__tile_inst__DOT__cmd_nt_addr = VL_SCOPED_RAND_RESET_I(12, __VscopeHash, 2132488266276418577ull);
    vlSelf->top__DOT__vgc_inst__DOT__tile_inst__DOT__cmd_at_addr = VL_SCOPED_RAND_RESET_I(12, __VscopeHash, 14137565378615498391ull);
    vlSelf->top__DOT__vgc_inst__DOT__tile_inst__DOT__cmd_nt_din = VL_SCOPED_RAND_RESET_I(8, __VscopeHash, 5113014320508164993ull);
    vlSelf->top__DOT__vgc_inst__DOT__tile_inst__DOT__cmd_at_din = VL_SCOPED_RAND_RESET_I(8, __VscopeHash, 8070533148860467576ull);
    vlSelf->top__DOT__vgc_inst__DOT__tile_inst__DOT__cmd_pal_addr_reg = VL_SCOPED_RAND_RESET_I(8, __VscopeHash, 7030323352088782018ull);
    vlSelf->top__DOT__vgc_inst__DOT__tile_inst__DOT__cmd_pal_din = VL_SCOPED_RAND_RESET_I(12, __VscopeHash, 8440012106467947134ull);
    vlSelf->top__DOT__vgc_inst__DOT__tile_inst__DOT__blt_tile_rd_pending = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 2361033918321119133ull);
    vlSelf->top__DOT__vgc_inst__DOT__tile_inst__DOT__blt_tile_rd_latch = VL_SCOPED_RAND_RESET_I(8, __VscopeHash, 12753724397765495169ull);
    vlSelf->top__DOT__vgc_inst__DOT__tile_inst__DOT__world_y = VL_SCOPED_RAND_RESET_I(10, __VscopeHash, 16623484178495267749ull);
    vlSelf->top__DOT__vgc_inst__DOT__tile_inst__DOT__s1_pix_in_x = VL_SCOPED_RAND_RESET_I(4, __VscopeHash, 1928972005779562601ull);
    vlSelf->top__DOT__vgc_inst__DOT__tile_inst__DOT__s1_pix_in_y = VL_SCOPED_RAND_RESET_I(4, __VscopeHash, 16501396472136322495ull);
    vlSelf->top__DOT__vgc_inst__DOT__tile_inst__DOT__s1_tile_size16 = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 2084761017727018947ull);
    vlSelf->top__DOT__vgc_inst__DOT__tile_inst__DOT__s1_tile_w = VL_SCOPED_RAND_RESET_I(4, __VscopeHash, 6876677669055960357ull);
    vlSelf->top__DOT__vgc_inst__DOT__tile_inst__DOT__s1_sample_x = VL_SCOPED_RAND_RESET_I(4, __VscopeHash, 17738661916353261069ull);
    vlSelf->top__DOT__vgc_inst__DOT__tile_inst__DOT__s1_sample_y = VL_SCOPED_RAND_RESET_I(4, __VscopeHash, 12875224018026327966ull);
    vlSelf->top__DOT__vgc_inst__DOT__tile_inst__DOT__s2_sub_pal = VL_SCOPED_RAND_RESET_I(4, __VscopeHash, 17524755658363798695ull);
    vlSelf->top__DOT__vgc_inst__DOT__tile_inst__DOT__s2_sample_x_lsb = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 18062442630143331417ull);
    vlSelf->top__DOT__vgc_inst__DOT__tile_inst__DOT__s2_tile_pri = VL_SCOPED_RAND_RESET_I(2, __VscopeHash, 9400508896510145702ull);
    vlSelf->top__DOT__vgc_inst__DOT__tile_inst__DOT__s2_color_idx = VL_SCOPED_RAND_RESET_I(4, __VscopeHash, 1136272894313071577ull);
    vlSelf->top__DOT__vgc_inst__DOT__tile_inst__DOT__s3_pixel_valid = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 8431359440301845407ull);
    vlSelf->top__DOT__vgc_inst__DOT__tile_inst__DOT__s3_color_idx = VL_SCOPED_RAND_RESET_I(4, __VscopeHash, 6608848136949748290ull);
    vlSelf->top__DOT__vgc_inst__DOT__tile_inst__DOT__s3_tile_pri = VL_SCOPED_RAND_RESET_I(2, __VscopeHash, 2606840384849026096ull);
    vlSelf->top__DOT__vgc_inst__DOT__tile_inst__DOT__s3_trans_color = VL_SCOPED_RAND_RESET_I(4, __VscopeHash, 3323248488967517185ull);
    vlSelf->top__DOT__vgc_inst__DOT__tile_inst__DOT____VdfgRegularize_hb33eefcd_0_1 = 0;
    vlSelf->top__DOT__vgc_inst__DOT__tile_inst__DOT____VdfgRegularize_hb33eefcd_0_2 = 0;
    vlSelf->top__DOT__vgc_inst__DOT__tile_inst__DOT____VdfgRegularize_hb33eefcd_0_3 = 0;
    for (int __Vi0 = 0; __Vi0 < 32768; ++__Vi0) {
        vlSelf->top__DOT__vgc_inst__DOT__tile_inst__DOT__tile_data_ram__DOT__mem[__Vi0] = VL_SCOPED_RAND_RESET_I(8, __VscopeHash, 16170665793896305714ull);
    }
    for (int __Vi0 = 0; __Vi0 < 4096; ++__Vi0) {
        vlSelf->top__DOT__vgc_inst__DOT__tile_inst__DOT__nametable_ram__DOT__mem[__Vi0] = VL_SCOPED_RAND_RESET_I(8, __VscopeHash, 1789495902756687216ull);
    }
    for (int __Vi0 = 0; __Vi0 < 4096; ++__Vi0) {
        vlSelf->top__DOT__vgc_inst__DOT__tile_inst__DOT__attr_table_ram__DOT__mem[__Vi0] = VL_SCOPED_RAND_RESET_I(8, __VscopeHash, 2877447275704871997ull);
    }
    for (int __Vi0 = 0; __Vi0 < 256; ++__Vi0) {
        vlSelf->top__DOT__vgc_inst__DOT__tile_inst__DOT__pal_ram_inst__DOT__mem[__Vi0] = VL_SCOPED_RAND_RESET_I(12, __VscopeHash, 16637157683192219654ull);
    }
    vlSelf->__VdfgRegularize_hebeb780c_0_0 = 0;
    vlSelf->__VdfgRegularize_hebeb780c_0_3 = 0;
    vlSelf->__VdfgRegularize_hebeb780c_0_4 = 0;
    vlSelf->__VdfgRegularize_hebeb780c_0_5 = 0;
    vlSelf->__VdfgRegularize_hebeb780c_0_6 = 0;
    vlSelf->__VdfgRegularize_hebeb780c_0_8 = 0;
    vlSelf->__VdfgRegularize_hebeb780c_0_9 = 0;
    vlSelf->__VdfgRegularize_hebeb780c_0_10 = 0;
    vlSelf->__VdfgRegularize_hebeb780c_0_11 = 0;
    vlSelf->__VdfgRegularize_hebeb780c_0_12 = 0;
    vlSelf->__VdfgRegularize_hebeb780c_0_13 = 0;
    vlSelf->__VdfgRegularize_hebeb780c_0_14 = 0;
    vlSelf->__VdfgRegularize_hebeb780c_0_15 = 0;
    vlSelf->__VdfgRegularize_hebeb780c_0_19 = 0;
    vlSelf->__VdfgRegularize_hebeb780c_0_25 = 0;
    vlSelf->__Vdly__top__DOT__sid_clk_div = 0;
    vlSelf->__Vdly__top__DOT__cpu_inst__DOT__state = 0;
    vlSelf->__VdlyVal__top__DOT__vgc_inst__DOT__tile_inst__DOT__attr_table_ram__DOT__mem__v0 = 0;
    vlSelf->__VdlyDim0__top__DOT__vgc_inst__DOT__tile_inst__DOT__attr_table_ram__DOT__mem__v0 = 0;
    vlSelf->__VdlySet__top__DOT__vgc_inst__DOT__tile_inst__DOT__attr_table_ram__DOT__mem__v0 = 0;
    for (int __Vi0 = 0; __Vi0 < 2; ++__Vi0) {
        vlSelf->__VstlTriggered[__Vi0] = 0;
    }
    for (int __Vi0 = 0; __Vi0 < 12; ++__Vi0) {
        vlSelf->__Vtrigprevexpr___TOP__top__DOT__sid1_inst____PVT__v1__DOT__waveform_dac__DOT__bitsum__0[__Vi0] = 0;
    }
    for (int __Vi0 = 0; __Vi0 < 8; ++__Vi0) {
        vlSelf->__Vtrigprevexpr___TOP__top__DOT__sid1_inst____PVT__v1__DOT__envelope_dac__DOT__bitsum__0[__Vi0] = 0;
    }
    for (int __Vi0 = 0; __Vi0 < 12; ++__Vi0) {
        vlSelf->__Vtrigprevexpr___TOP__top__DOT__sid1_inst____PVT__v2__DOT__waveform_dac__DOT__bitsum__0[__Vi0] = 0;
    }
    for (int __Vi0 = 0; __Vi0 < 8; ++__Vi0) {
        vlSelf->__Vtrigprevexpr___TOP__top__DOT__sid1_inst____PVT__v2__DOT__envelope_dac__DOT__bitsum__0[__Vi0] = 0;
    }
    for (int __Vi0 = 0; __Vi0 < 12; ++__Vi0) {
        vlSelf->__Vtrigprevexpr___TOP__top__DOT__sid1_inst____PVT__v3__DOT__waveform_dac__DOT__bitsum__0[__Vi0] = 0;
    }
    for (int __Vi0 = 0; __Vi0 < 8; ++__Vi0) {
        vlSelf->__Vtrigprevexpr___TOP__top__DOT__sid1_inst____PVT__v3__DOT__envelope_dac__DOT__bitsum__0[__Vi0] = 0;
    }
    for (int __Vi0 = 0; __Vi0 < 12; ++__Vi0) {
        vlSelf->__Vtrigprevexpr___TOP__top__DOT__sid2_inst____PVT__v1__DOT__waveform_dac__DOT__bitsum__0[__Vi0] = 0;
    }
    for (int __Vi0 = 0; __Vi0 < 8; ++__Vi0) {
        vlSelf->__Vtrigprevexpr___TOP__top__DOT__sid2_inst____PVT__v1__DOT__envelope_dac__DOT__bitsum__0[__Vi0] = 0;
    }
    for (int __Vi0 = 0; __Vi0 < 12; ++__Vi0) {
        vlSelf->__Vtrigprevexpr___TOP__top__DOT__sid2_inst____PVT__v2__DOT__waveform_dac__DOT__bitsum__0[__Vi0] = 0;
    }
    for (int __Vi0 = 0; __Vi0 < 8; ++__Vi0) {
        vlSelf->__Vtrigprevexpr___TOP__top__DOT__sid2_inst____PVT__v2__DOT__envelope_dac__DOT__bitsum__0[__Vi0] = 0;
    }
    for (int __Vi0 = 0; __Vi0 < 12; ++__Vi0) {
        vlSelf->__Vtrigprevexpr___TOP__top__DOT__sid2_inst____PVT__v3__DOT__waveform_dac__DOT__bitsum__0[__Vi0] = 0;
    }
    for (int __Vi0 = 0; __Vi0 < 8; ++__Vi0) {
        vlSelf->__Vtrigprevexpr___TOP__top__DOT__sid2_inst____PVT__v3__DOT__envelope_dac__DOT__bitsum__0[__Vi0] = 0;
    }
    vlSelf->__VstlDidInit = 0;
    for (int __Vi0 = 0; __Vi0 < 1; ++__Vi0) {
        vlSelf->__VicoTriggered[__Vi0] = 0;
    }
    for (int __Vi0 = 0; __Vi0 < 1; ++__Vi0) {
        vlSelf->__VactTriggered[__Vi0] = 0;
    }
    for (int __Vi0 = 0; __Vi0 < 12; ++__Vi0) {
        vlSelf->__Vtrigprevexpr___TOP__top__DOT__sid1_inst____PVT__v1__DOT__waveform_dac__DOT__bitsum__1[__Vi0] = 0;
    }
    for (int __Vi0 = 0; __Vi0 < 8; ++__Vi0) {
        vlSelf->__Vtrigprevexpr___TOP__top__DOT__sid1_inst____PVT__v1__DOT__envelope_dac__DOT__bitsum__1[__Vi0] = 0;
    }
    for (int __Vi0 = 0; __Vi0 < 12; ++__Vi0) {
        vlSelf->__Vtrigprevexpr___TOP__top__DOT__sid1_inst____PVT__v2__DOT__waveform_dac__DOT__bitsum__1[__Vi0] = 0;
    }
    for (int __Vi0 = 0; __Vi0 < 8; ++__Vi0) {
        vlSelf->__Vtrigprevexpr___TOP__top__DOT__sid1_inst____PVT__v2__DOT__envelope_dac__DOT__bitsum__1[__Vi0] = 0;
    }
    for (int __Vi0 = 0; __Vi0 < 12; ++__Vi0) {
        vlSelf->__Vtrigprevexpr___TOP__top__DOT__sid1_inst____PVT__v3__DOT__waveform_dac__DOT__bitsum__1[__Vi0] = 0;
    }
    for (int __Vi0 = 0; __Vi0 < 8; ++__Vi0) {
        vlSelf->__Vtrigprevexpr___TOP__top__DOT__sid1_inst____PVT__v3__DOT__envelope_dac__DOT__bitsum__1[__Vi0] = 0;
    }
    for (int __Vi0 = 0; __Vi0 < 12; ++__Vi0) {
        vlSelf->__Vtrigprevexpr___TOP__top__DOT__sid2_inst____PVT__v1__DOT__waveform_dac__DOT__bitsum__1[__Vi0] = 0;
    }
    for (int __Vi0 = 0; __Vi0 < 8; ++__Vi0) {
        vlSelf->__Vtrigprevexpr___TOP__top__DOT__sid2_inst____PVT__v1__DOT__envelope_dac__DOT__bitsum__1[__Vi0] = 0;
    }
    for (int __Vi0 = 0; __Vi0 < 12; ++__Vi0) {
        vlSelf->__Vtrigprevexpr___TOP__top__DOT__sid2_inst____PVT__v2__DOT__waveform_dac__DOT__bitsum__1[__Vi0] = 0;
    }
    for (int __Vi0 = 0; __Vi0 < 8; ++__Vi0) {
        vlSelf->__Vtrigprevexpr___TOP__top__DOT__sid2_inst____PVT__v2__DOT__envelope_dac__DOT__bitsum__1[__Vi0] = 0;
    }
    for (int __Vi0 = 0; __Vi0 < 12; ++__Vi0) {
        vlSelf->__Vtrigprevexpr___TOP__top__DOT__sid2_inst____PVT__v3__DOT__waveform_dac__DOT__bitsum__1[__Vi0] = 0;
    }
    for (int __Vi0 = 0; __Vi0 < 8; ++__Vi0) {
        vlSelf->__Vtrigprevexpr___TOP__top__DOT__sid2_inst____PVT__v3__DOT__envelope_dac__DOT__bitsum__1[__Vi0] = 0;
    }
    vlSelf->__Vtrigprevexpr___TOP__clk__0 = 0;
    vlSelf->__Vtrigprevexpr___TOP__rst__0 = 0;
    vlSelf->__VactDidInit = 0;
    for (int __Vi0 = 0; __Vi0 < 1; ++__Vi0) {
        vlSelf->__VnbaTriggered[__Vi0] = 0;
    }
}
