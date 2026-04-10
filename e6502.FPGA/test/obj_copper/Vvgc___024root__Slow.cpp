// Verilated -*- C++ -*-
// DESCRIPTION: Verilator output: Design implementation internals
// See Vvgc.h for the primary calling header

#include "Vvgc__pch.h"

// Parameter definitions for Vvgc___024root
constexpr CData/*7:0*/ Vvgc___024root::vgc__DOT__CMD_PLOT;
constexpr CData/*7:0*/ Vvgc___024root::vgc__DOT__CMD_UNPLOT;
constexpr CData/*7:0*/ Vvgc___024root::vgc__DOT__CMD_LINE;
constexpr CData/*7:0*/ Vvgc___024root::vgc__DOT__CMD_CIRCLE;
constexpr CData/*7:0*/ Vvgc___024root::vgc__DOT__CMD_RECT;
constexpr CData/*7:0*/ Vvgc___024root::vgc__DOT__CMD_FILL;
constexpr CData/*7:0*/ Vvgc___024root::vgc__DOT__CMD_GCLS;
constexpr CData/*7:0*/ Vvgc___024root::vgc__DOT__CMD_GCOLOR;
constexpr CData/*7:0*/ Vvgc___024root::vgc__DOT__CMD_PAINT;
constexpr CData/*7:0*/ Vvgc___024root::vgc__DOT__CMD_SPRDEF;
constexpr CData/*7:0*/ Vvgc___024root::vgc__DOT__CMD_SPRROW;
constexpr CData/*7:0*/ Vvgc___024root::vgc__DOT__CMD_SPRCLR;
constexpr CData/*7:0*/ Vvgc___024root::vgc__DOT__CMD_SPRCOPY;
constexpr CData/*7:0*/ Vvgc___024root::vgc__DOT__CMD_SPRPOS;
constexpr CData/*7:0*/ Vvgc___024root::vgc__DOT__CMD_SPRENA;
constexpr CData/*7:0*/ Vvgc___024root::vgc__DOT__CMD_SPRDIS;
constexpr CData/*7:0*/ Vvgc___024root::vgc__DOT__CMD_SPRFLIP;
constexpr CData/*7:0*/ Vvgc___024root::vgc__DOT__CMD_SPRPRI;
constexpr SData/*15:0*/ Vvgc___024root::vgc__DOT__VGC_BASE;
constexpr SData/*15:0*/ Vvgc___024root::vgc__DOT__VGC_REGS_END;
constexpr SData/*15:0*/ Vvgc___024root::vgc__DOT__SPR_REG_BASE;
constexpr SData/*15:0*/ Vvgc___024root::vgc__DOT__SPR_REG_END;
constexpr SData/*15:0*/ Vvgc___024root::vgc__DOT__CHAR_RAM_BASE;
constexpr SData/*15:0*/ Vvgc___024root::vgc__DOT__CHAR_RAM_END;
constexpr SData/*15:0*/ Vvgc___024root::vgc__DOT__COLOR_RAM_BASE;
constexpr SData/*15:0*/ Vvgc___024root::vgc__DOT__COLOR_RAM_END;
constexpr IData/*31:0*/ Vvgc___024root::vgc__DOT__H_ACTIVE;
constexpr IData/*31:0*/ Vvgc___024root::vgc__DOT__H_FRONT;
constexpr IData/*31:0*/ Vvgc___024root::vgc__DOT__H_SYNC;
constexpr IData/*31:0*/ Vvgc___024root::vgc__DOT__H_BACK;
constexpr IData/*31:0*/ Vvgc___024root::vgc__DOT__H_TOTAL;
constexpr IData/*31:0*/ Vvgc___024root::vgc__DOT__V_ACTIVE;
constexpr IData/*31:0*/ Vvgc___024root::vgc__DOT__V_FRONT;
constexpr IData/*31:0*/ Vvgc___024root::vgc__DOT__V_SYNC;
constexpr IData/*31:0*/ Vvgc___024root::vgc__DOT__V_BACK;
constexpr IData/*31:0*/ Vvgc___024root::vgc__DOT__V_TOTAL;
constexpr IData/*31:0*/ Vvgc___024root::vgc__DOT__COLS;
constexpr IData/*31:0*/ Vvgc___024root::vgc__DOT__ROWS;
constexpr IData/*31:0*/ Vvgc___024root::vgc__DOT__CHAR_W;
constexpr IData/*31:0*/ Vvgc___024root::vgc__DOT__CHAR_H;
constexpr IData/*31:0*/ Vvgc___024root::vgc__DOT__TEXT_H;
constexpr IData/*31:0*/ Vvgc___024root::vgc__DOT__V_BORDER;
constexpr IData/*31:0*/ Vvgc___024root::vgc__DOT__GFX_W;
constexpr IData/*31:0*/ Vvgc___024root::vgc__DOT__GFX_H;
constexpr IData/*31:0*/ Vvgc___024root::vgc__DOT__GFX_SIZE;
constexpr IData/*31:0*/ Vvgc___024root::vgc__DOT__REG_MODE;
constexpr IData/*31:0*/ Vvgc___024root::vgc__DOT__REG_BGCOL;
constexpr IData/*31:0*/ Vvgc___024root::vgc__DOT__REG_FGCOL;
constexpr IData/*31:0*/ Vvgc___024root::vgc__DOT__REG_CURSORX;
constexpr IData/*31:0*/ Vvgc___024root::vgc__DOT__REG_CURSORY;
constexpr IData/*31:0*/ Vvgc___024root::vgc__DOT__REG_BORDER;
constexpr IData/*31:0*/ Vvgc___024root::vgc__DOT__REG_CHAROUT;
constexpr IData/*31:0*/ Vvgc___024root::vgc__DOT__REG_CHARIN;
constexpr IData/*31:0*/ Vvgc___024root::vgc__DOT__REG_CMD;
constexpr IData/*31:0*/ Vvgc___024root::vgc__DOT__NUM_SPRITES;
constexpr IData/*31:0*/ Vvgc___024root::vgc__DOT__SPR_W;
constexpr IData/*31:0*/ Vvgc___024root::vgc__DOT__SPR_H;
constexpr IData/*31:0*/ Vvgc___024root::vgc__DOT__COPPER_MAX;


void Vvgc___024root___ctor_var_reset(Vvgc___024root* vlSelf);

Vvgc___024root::Vvgc___024root(Vvgc__Syms* symsp, const char* namep)
 {
    vlSymsp = symsp;
    vlNamep = strdup(namep);
    // Reset structure values
    Vvgc___024root___ctor_var_reset(this);
}

void Vvgc___024root::__Vconfigure(bool first) {
    (void)first;  // Prevent unused variable warning
}

Vvgc___024root::~Vvgc___024root() {
    VL_DO_DANGLING(std::free(const_cast<char*>(vlNamep)), vlNamep);
}
