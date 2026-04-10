// Verilated -*- C++ -*-
// DESCRIPTION: Verilator output: Symbol table implementation internals

#include "Vvgc__pch.h"

Vvgc__Syms::Vvgc__Syms(VerilatedContext* contextp, const char* namep, Vvgc* modelp)
    : VerilatedSyms{contextp}
    // Setup internal state of the Syms class
    , __Vm_modelp{modelp}
    // Setup top module instance
    , TOP{this, namep}
{
    // Check resources
    Verilated::stackCheck(670);
    // Setup sub module instances
    // Configure time unit / time precision
    _vm_contextp__->timeunit(-12);
    _vm_contextp__->timeprecision(-12);
    // Setup each module's pointers to their submodules
    // Setup each module's pointer back to symbol table (for public functions)
    TOP.__Vconfigure(true);
    // Setup scopes
    __Vscopep_TOP = new VerilatedScope{this, "TOP", "TOP", "<null>", 0, VerilatedScope::SCOPE_OTHER};
    __Vscopep_vgc = new VerilatedScope{this, "vgc", "vgc", "<null>", 0, VerilatedScope::SCOPE_OTHER};
    __Vscopep_vgc__unnamedblk1 = new VerilatedScope{this, "vgc.unnamedblk1", "unnamedblk1", "<null>", 0, VerilatedScope::SCOPE_OTHER};
    __Vscopep_vgc__unnamedblk2 = new VerilatedScope{this, "vgc.unnamedblk2", "unnamedblk2", "<null>", 0, VerilatedScope::SCOPE_OTHER};
    __Vscopep_vgc__unnamedblk3 = new VerilatedScope{this, "vgc.unnamedblk3", "unnamedblk3", "<null>", 0, VerilatedScope::SCOPE_OTHER};
    __Vscopep_vgc__unnamedblk4 = new VerilatedScope{this, "vgc.unnamedblk4", "unnamedblk4", "<null>", 0, VerilatedScope::SCOPE_OTHER};
    __Vscopep_vgc__unnamedblk5 = new VerilatedScope{this, "vgc.unnamedblk5", "unnamedblk5", "<null>", 0, VerilatedScope::SCOPE_OTHER};
    __Vscopep_vgc__unnamedblk6 = new VerilatedScope{this, "vgc.unnamedblk6", "unnamedblk6", "<null>", 0, VerilatedScope::SCOPE_OTHER};
    __Vscopep_vgc__unnamedblk7 = new VerilatedScope{this, "vgc.unnamedblk7", "unnamedblk7", "<null>", 0, VerilatedScope::SCOPE_OTHER};
    __Vscopep_vgc__unnamedblk8 = new VerilatedScope{this, "vgc.unnamedblk8", "unnamedblk8", "<null>", 0, VerilatedScope::SCOPE_OTHER};
    // Setup export functions - final: 0
    // Setup export functions - final: 1
    // Setup public variables
    __Vscopep_TOP->varInsert("clk", &(TOP.clk), false, VLVT_UINT8, VLVD_IN|VLVF_PUB_RW, 0, 0);
    __Vscopep_TOP->varInsert("cpu_addr", &(TOP.cpu_addr), false, VLVT_UINT16, VLVD_IN|VLVF_PUB_RW, 0, 1 ,15,0);
    __Vscopep_TOP->varInsert("cpu_ce", &(TOP.cpu_ce), false, VLVT_UINT8, VLVD_IN|VLVF_PUB_RW, 0, 0);
    __Vscopep_TOP->varInsert("cpu_rdata", &(TOP.cpu_rdata), false, VLVT_UINT8, VLVD_OUT|VLVF_PUB_RW, 0, 1 ,7,0);
    __Vscopep_TOP->varInsert("cpu_re", &(TOP.cpu_re), false, VLVT_UINT8, VLVD_IN|VLVF_PUB_RW, 0, 0);
    __Vscopep_TOP->varInsert("cpu_wdata", &(TOP.cpu_wdata), false, VLVT_UINT8, VLVD_IN|VLVF_PUB_RW, 0, 1 ,7,0);
    __Vscopep_TOP->varInsert("cpu_we", &(TOP.cpu_we), false, VLVT_UINT8, VLVD_IN|VLVF_PUB_RW, 0, 0);
    __Vscopep_TOP->varInsert("key_data", &(TOP.key_data), false, VLVT_UINT8, VLVD_IN|VLVF_PUB_RW, 0, 1 ,7,0);
    __Vscopep_TOP->varInsert("key_valid", &(TOP.key_valid), false, VLVT_UINT8, VLVD_IN|VLVF_PUB_RW, 0, 0);
    __Vscopep_TOP->varInsert("rst", &(TOP.rst), false, VLVT_UINT8, VLVD_IN|VLVF_PUB_RW, 0, 0);
    __Vscopep_TOP->varInsert("vid_b", &(TOP.vid_b), false, VLVT_UINT8, VLVD_OUT|VLVF_PUB_RW, 0, 1 ,3,0);
    __Vscopep_TOP->varInsert("vid_de", &(TOP.vid_de), false, VLVT_UINT8, VLVD_OUT|VLVF_PUB_RW, 0, 0);
    __Vscopep_TOP->varInsert("vid_g", &(TOP.vid_g), false, VLVT_UINT8, VLVD_OUT|VLVF_PUB_RW, 0, 1 ,3,0);
    __Vscopep_TOP->varInsert("vid_hsync", &(TOP.vid_hsync), false, VLVT_UINT8, VLVD_OUT|VLVF_PUB_RW, 0, 0);
    __Vscopep_TOP->varInsert("vid_r", &(TOP.vid_r), false, VLVT_UINT8, VLVD_OUT|VLVF_PUB_RW, 0, 1 ,3,0);
    __Vscopep_TOP->varInsert("vid_vsync", &(TOP.vid_vsync), false, VLVT_UINT8, VLVD_OUT|VLVF_PUB_RW, 0, 0);
    __Vscopep_vgc->varInsert("CHAR_H", const_cast<void*>(static_cast<const void*>(&(TOP.vgc__DOT__CHAR_H))), true, VLVT_UINT32, VLVD_NODIR|VLVF_PUB_RW|VLVF_SIGNED, 0, 1 ,31,0);
    __Vscopep_vgc->varInsert("CHAR_RAM_BASE", const_cast<void*>(static_cast<const void*>(&(TOP.vgc__DOT__CHAR_RAM_BASE))), true, VLVT_UINT16, VLVD_NODIR|VLVF_PUB_RW, 0, 1 ,15,0);
    __Vscopep_vgc->varInsert("CHAR_RAM_END", const_cast<void*>(static_cast<const void*>(&(TOP.vgc__DOT__CHAR_RAM_END))), true, VLVT_UINT16, VLVD_NODIR|VLVF_PUB_RW, 0, 1 ,15,0);
    __Vscopep_vgc->varInsert("CHAR_W", const_cast<void*>(static_cast<const void*>(&(TOP.vgc__DOT__CHAR_W))), true, VLVT_UINT32, VLVD_NODIR|VLVF_PUB_RW|VLVF_SIGNED, 0, 1 ,31,0);
    __Vscopep_vgc->varInsert("CMD_CIRCLE", const_cast<void*>(static_cast<const void*>(&(TOP.vgc__DOT__CMD_CIRCLE))), true, VLVT_UINT8, VLVD_NODIR|VLVF_PUB_RW, 0, 1 ,7,0);
    __Vscopep_vgc->varInsert("CMD_FILL", const_cast<void*>(static_cast<const void*>(&(TOP.vgc__DOT__CMD_FILL))), true, VLVT_UINT8, VLVD_NODIR|VLVF_PUB_RW, 0, 1 ,7,0);
    __Vscopep_vgc->varInsert("CMD_GCLS", const_cast<void*>(static_cast<const void*>(&(TOP.vgc__DOT__CMD_GCLS))), true, VLVT_UINT8, VLVD_NODIR|VLVF_PUB_RW, 0, 1 ,7,0);
    __Vscopep_vgc->varInsert("CMD_GCOLOR", const_cast<void*>(static_cast<const void*>(&(TOP.vgc__DOT__CMD_GCOLOR))), true, VLVT_UINT8, VLVD_NODIR|VLVF_PUB_RW, 0, 1 ,7,0);
    __Vscopep_vgc->varInsert("CMD_LINE", const_cast<void*>(static_cast<const void*>(&(TOP.vgc__DOT__CMD_LINE))), true, VLVT_UINT8, VLVD_NODIR|VLVF_PUB_RW, 0, 1 ,7,0);
    __Vscopep_vgc->varInsert("CMD_PAINT", const_cast<void*>(static_cast<const void*>(&(TOP.vgc__DOT__CMD_PAINT))), true, VLVT_UINT8, VLVD_NODIR|VLVF_PUB_RW, 0, 1 ,7,0);
    __Vscopep_vgc->varInsert("CMD_PLOT", const_cast<void*>(static_cast<const void*>(&(TOP.vgc__DOT__CMD_PLOT))), true, VLVT_UINT8, VLVD_NODIR|VLVF_PUB_RW, 0, 1 ,7,0);
    __Vscopep_vgc->varInsert("CMD_RECT", const_cast<void*>(static_cast<const void*>(&(TOP.vgc__DOT__CMD_RECT))), true, VLVT_UINT8, VLVD_NODIR|VLVF_PUB_RW, 0, 1 ,7,0);
    __Vscopep_vgc->varInsert("CMD_SPRCLR", const_cast<void*>(static_cast<const void*>(&(TOP.vgc__DOT__CMD_SPRCLR))), true, VLVT_UINT8, VLVD_NODIR|VLVF_PUB_RW, 0, 1 ,7,0);
    __Vscopep_vgc->varInsert("CMD_SPRCOPY", const_cast<void*>(static_cast<const void*>(&(TOP.vgc__DOT__CMD_SPRCOPY))), true, VLVT_UINT8, VLVD_NODIR|VLVF_PUB_RW, 0, 1 ,7,0);
    __Vscopep_vgc->varInsert("CMD_SPRDEF", const_cast<void*>(static_cast<const void*>(&(TOP.vgc__DOT__CMD_SPRDEF))), true, VLVT_UINT8, VLVD_NODIR|VLVF_PUB_RW, 0, 1 ,7,0);
    __Vscopep_vgc->varInsert("CMD_SPRDIS", const_cast<void*>(static_cast<const void*>(&(TOP.vgc__DOT__CMD_SPRDIS))), true, VLVT_UINT8, VLVD_NODIR|VLVF_PUB_RW, 0, 1 ,7,0);
    __Vscopep_vgc->varInsert("CMD_SPRENA", const_cast<void*>(static_cast<const void*>(&(TOP.vgc__DOT__CMD_SPRENA))), true, VLVT_UINT8, VLVD_NODIR|VLVF_PUB_RW, 0, 1 ,7,0);
    __Vscopep_vgc->varInsert("CMD_SPRFLIP", const_cast<void*>(static_cast<const void*>(&(TOP.vgc__DOT__CMD_SPRFLIP))), true, VLVT_UINT8, VLVD_NODIR|VLVF_PUB_RW, 0, 1 ,7,0);
    __Vscopep_vgc->varInsert("CMD_SPRPOS", const_cast<void*>(static_cast<const void*>(&(TOP.vgc__DOT__CMD_SPRPOS))), true, VLVT_UINT8, VLVD_NODIR|VLVF_PUB_RW, 0, 1 ,7,0);
    __Vscopep_vgc->varInsert("CMD_SPRPRI", const_cast<void*>(static_cast<const void*>(&(TOP.vgc__DOT__CMD_SPRPRI))), true, VLVT_UINT8, VLVD_NODIR|VLVF_PUB_RW, 0, 1 ,7,0);
    __Vscopep_vgc->varInsert("CMD_SPRROW", const_cast<void*>(static_cast<const void*>(&(TOP.vgc__DOT__CMD_SPRROW))), true, VLVT_UINT8, VLVD_NODIR|VLVF_PUB_RW, 0, 1 ,7,0);
    __Vscopep_vgc->varInsert("CMD_UNPLOT", const_cast<void*>(static_cast<const void*>(&(TOP.vgc__DOT__CMD_UNPLOT))), true, VLVT_UINT8, VLVD_NODIR|VLVF_PUB_RW, 0, 1 ,7,0);
    __Vscopep_vgc->varInsert("COLOR_RAM_BASE", const_cast<void*>(static_cast<const void*>(&(TOP.vgc__DOT__COLOR_RAM_BASE))), true, VLVT_UINT16, VLVD_NODIR|VLVF_PUB_RW, 0, 1 ,15,0);
    __Vscopep_vgc->varInsert("COLOR_RAM_END", const_cast<void*>(static_cast<const void*>(&(TOP.vgc__DOT__COLOR_RAM_END))), true, VLVT_UINT16, VLVD_NODIR|VLVF_PUB_RW, 0, 1 ,15,0);
    __Vscopep_vgc->varInsert("COLS", const_cast<void*>(static_cast<const void*>(&(TOP.vgc__DOT__COLS))), true, VLVT_UINT32, VLVD_NODIR|VLVF_PUB_RW|VLVF_SIGNED, 0, 1 ,31,0);
    __Vscopep_vgc->varInsert("COPPER_MAX", const_cast<void*>(static_cast<const void*>(&(TOP.vgc__DOT__COPPER_MAX))), true, VLVT_UINT32, VLVD_NODIR|VLVF_PUB_RW|VLVF_SIGNED, 0, 1 ,31,0);
    __Vscopep_vgc->varInsert("GFX_H", const_cast<void*>(static_cast<const void*>(&(TOP.vgc__DOT__GFX_H))), true, VLVT_UINT32, VLVD_NODIR|VLVF_PUB_RW|VLVF_SIGNED, 0, 1 ,31,0);
    __Vscopep_vgc->varInsert("GFX_SIZE", const_cast<void*>(static_cast<const void*>(&(TOP.vgc__DOT__GFX_SIZE))), true, VLVT_UINT32, VLVD_NODIR|VLVF_PUB_RW|VLVF_SIGNED, 0, 1 ,31,0);
    __Vscopep_vgc->varInsert("GFX_W", const_cast<void*>(static_cast<const void*>(&(TOP.vgc__DOT__GFX_W))), true, VLVT_UINT32, VLVD_NODIR|VLVF_PUB_RW|VLVF_SIGNED, 0, 1 ,31,0);
    __Vscopep_vgc->varInsert("H_ACTIVE", const_cast<void*>(static_cast<const void*>(&(TOP.vgc__DOT__H_ACTIVE))), true, VLVT_UINT32, VLVD_NODIR|VLVF_PUB_RW|VLVF_SIGNED, 0, 1 ,31,0);
    __Vscopep_vgc->varInsert("H_BACK", const_cast<void*>(static_cast<const void*>(&(TOP.vgc__DOT__H_BACK))), true, VLVT_UINT32, VLVD_NODIR|VLVF_PUB_RW|VLVF_SIGNED, 0, 1 ,31,0);
    __Vscopep_vgc->varInsert("H_FRONT", const_cast<void*>(static_cast<const void*>(&(TOP.vgc__DOT__H_FRONT))), true, VLVT_UINT32, VLVD_NODIR|VLVF_PUB_RW|VLVF_SIGNED, 0, 1 ,31,0);
    __Vscopep_vgc->varInsert("H_SYNC", const_cast<void*>(static_cast<const void*>(&(TOP.vgc__DOT__H_SYNC))), true, VLVT_UINT32, VLVD_NODIR|VLVF_PUB_RW|VLVF_SIGNED, 0, 1 ,31,0);
    __Vscopep_vgc->varInsert("H_TOTAL", const_cast<void*>(static_cast<const void*>(&(TOP.vgc__DOT__H_TOTAL))), true, VLVT_UINT32, VLVD_NODIR|VLVF_PUB_RW|VLVF_SIGNED, 0, 1 ,31,0);
    __Vscopep_vgc->varInsert("NUM_SPRITES", const_cast<void*>(static_cast<const void*>(&(TOP.vgc__DOT__NUM_SPRITES))), true, VLVT_UINT32, VLVD_NODIR|VLVF_PUB_RW|VLVF_SIGNED, 0, 1 ,31,0);
    __Vscopep_vgc->varInsert("REG_BGCOL", const_cast<void*>(static_cast<const void*>(&(TOP.vgc__DOT__REG_BGCOL))), true, VLVT_UINT32, VLVD_NODIR|VLVF_PUB_RW|VLVF_SIGNED, 0, 1 ,31,0);
    __Vscopep_vgc->varInsert("REG_BORDER", const_cast<void*>(static_cast<const void*>(&(TOP.vgc__DOT__REG_BORDER))), true, VLVT_UINT32, VLVD_NODIR|VLVF_PUB_RW|VLVF_SIGNED, 0, 1 ,31,0);
    __Vscopep_vgc->varInsert("REG_CHARIN", const_cast<void*>(static_cast<const void*>(&(TOP.vgc__DOT__REG_CHARIN))), true, VLVT_UINT32, VLVD_NODIR|VLVF_PUB_RW|VLVF_SIGNED, 0, 1 ,31,0);
    __Vscopep_vgc->varInsert("REG_CHAROUT", const_cast<void*>(static_cast<const void*>(&(TOP.vgc__DOT__REG_CHAROUT))), true, VLVT_UINT32, VLVD_NODIR|VLVF_PUB_RW|VLVF_SIGNED, 0, 1 ,31,0);
    __Vscopep_vgc->varInsert("REG_CMD", const_cast<void*>(static_cast<const void*>(&(TOP.vgc__DOT__REG_CMD))), true, VLVT_UINT32, VLVD_NODIR|VLVF_PUB_RW|VLVF_SIGNED, 0, 1 ,31,0);
    __Vscopep_vgc->varInsert("REG_CURSORX", const_cast<void*>(static_cast<const void*>(&(TOP.vgc__DOT__REG_CURSORX))), true, VLVT_UINT32, VLVD_NODIR|VLVF_PUB_RW|VLVF_SIGNED, 0, 1 ,31,0);
    __Vscopep_vgc->varInsert("REG_CURSORY", const_cast<void*>(static_cast<const void*>(&(TOP.vgc__DOT__REG_CURSORY))), true, VLVT_UINT32, VLVD_NODIR|VLVF_PUB_RW|VLVF_SIGNED, 0, 1 ,31,0);
    __Vscopep_vgc->varInsert("REG_FGCOL", const_cast<void*>(static_cast<const void*>(&(TOP.vgc__DOT__REG_FGCOL))), true, VLVT_UINT32, VLVD_NODIR|VLVF_PUB_RW|VLVF_SIGNED, 0, 1 ,31,0);
    __Vscopep_vgc->varInsert("REG_MODE", const_cast<void*>(static_cast<const void*>(&(TOP.vgc__DOT__REG_MODE))), true, VLVT_UINT32, VLVD_NODIR|VLVF_PUB_RW|VLVF_SIGNED, 0, 1 ,31,0);
    __Vscopep_vgc->varInsert("ROWS", const_cast<void*>(static_cast<const void*>(&(TOP.vgc__DOT__ROWS))), true, VLVT_UINT32, VLVD_NODIR|VLVF_PUB_RW|VLVF_SIGNED, 0, 1 ,31,0);
    __Vscopep_vgc->varInsert("SPR_H", const_cast<void*>(static_cast<const void*>(&(TOP.vgc__DOT__SPR_H))), true, VLVT_UINT32, VLVD_NODIR|VLVF_PUB_RW|VLVF_SIGNED, 0, 1 ,31,0);
    __Vscopep_vgc->varInsert("SPR_REG_BASE", const_cast<void*>(static_cast<const void*>(&(TOP.vgc__DOT__SPR_REG_BASE))), true, VLVT_UINT16, VLVD_NODIR|VLVF_PUB_RW, 0, 1 ,15,0);
    __Vscopep_vgc->varInsert("SPR_REG_END", const_cast<void*>(static_cast<const void*>(&(TOP.vgc__DOT__SPR_REG_END))), true, VLVT_UINT16, VLVD_NODIR|VLVF_PUB_RW, 0, 1 ,15,0);
    __Vscopep_vgc->varInsert("SPR_W", const_cast<void*>(static_cast<const void*>(&(TOP.vgc__DOT__SPR_W))), true, VLVT_UINT32, VLVD_NODIR|VLVF_PUB_RW|VLVF_SIGNED, 0, 1 ,31,0);
    __Vscopep_vgc->varInsert("TEXT_H", const_cast<void*>(static_cast<const void*>(&(TOP.vgc__DOT__TEXT_H))), true, VLVT_UINT32, VLVD_NODIR|VLVF_PUB_RW|VLVF_SIGNED, 0, 1 ,31,0);
    __Vscopep_vgc->varInsert("VGC_BASE", const_cast<void*>(static_cast<const void*>(&(TOP.vgc__DOT__VGC_BASE))), true, VLVT_UINT16, VLVD_NODIR|VLVF_PUB_RW, 0, 1 ,15,0);
    __Vscopep_vgc->varInsert("VGC_REGS_END", const_cast<void*>(static_cast<const void*>(&(TOP.vgc__DOT__VGC_REGS_END))), true, VLVT_UINT16, VLVD_NODIR|VLVF_PUB_RW, 0, 1 ,15,0);
    __Vscopep_vgc->varInsert("V_ACTIVE", const_cast<void*>(static_cast<const void*>(&(TOP.vgc__DOT__V_ACTIVE))), true, VLVT_UINT32, VLVD_NODIR|VLVF_PUB_RW|VLVF_SIGNED, 0, 1 ,31,0);
    __Vscopep_vgc->varInsert("V_BACK", const_cast<void*>(static_cast<const void*>(&(TOP.vgc__DOT__V_BACK))), true, VLVT_UINT32, VLVD_NODIR|VLVF_PUB_RW|VLVF_SIGNED, 0, 1 ,31,0);
    __Vscopep_vgc->varInsert("V_BORDER", const_cast<void*>(static_cast<const void*>(&(TOP.vgc__DOT__V_BORDER))), true, VLVT_UINT32, VLVD_NODIR|VLVF_PUB_RW|VLVF_SIGNED, 0, 1 ,31,0);
    __Vscopep_vgc->varInsert("V_FRONT", const_cast<void*>(static_cast<const void*>(&(TOP.vgc__DOT__V_FRONT))), true, VLVT_UINT32, VLVD_NODIR|VLVF_PUB_RW|VLVF_SIGNED, 0, 1 ,31,0);
    __Vscopep_vgc->varInsert("V_SYNC", const_cast<void*>(static_cast<const void*>(&(TOP.vgc__DOT__V_SYNC))), true, VLVT_UINT32, VLVD_NODIR|VLVF_PUB_RW|VLVF_SIGNED, 0, 1 ,31,0);
    __Vscopep_vgc->varInsert("V_TOTAL", const_cast<void*>(static_cast<const void*>(&(TOP.vgc__DOT__V_TOTAL))), true, VLVT_UINT32, VLVD_NODIR|VLVF_PUB_RW|VLVF_SIGNED, 0, 1 ,31,0);
    __Vscopep_vgc->varInsert("beam_pos", &(TOP.vgc__DOT__beam_pos), false, VLVT_UINT32, VLVD_NODIR|VLVF_PUB_RW|VLVF_CONTINUOUSLY, 0, 1 ,16,0);
    __Vscopep_vgc->varInsert("bg_color", &(TOP.vgc__DOT__bg_color), false, VLVT_UINT8, VLVD_NODIR|VLVF_PUB_RW, 0, 1 ,3,0);
    __Vscopep_vgc->varInsert("blink_count", &(TOP.vgc__DOT__blink_count), false, VLVT_UINT8, VLVD_NODIR|VLVF_PUB_RW, 0, 1 ,4,0);
    __Vscopep_vgc->varInsert("border_color", &(TOP.vgc__DOT__border_color), false, VLVT_UINT8, VLVD_NODIR|VLVF_PUB_RW, 0, 1 ,3,0);
    __Vscopep_vgc->varInsert("char_in_reg", &(TOP.vgc__DOT__char_in_reg), false, VLVT_UINT8, VLVD_NODIR|VLVF_PUB_RW, 0, 1 ,7,0);
    __Vscopep_vgc->varInsert("char_offset", &(TOP.vgc__DOT__char_offset), false, VLVT_UINT16, VLVD_NODIR|VLVF_PUB_RW|VLVF_CONTINUOUSLY, 0, 1 ,10,0);
    __Vscopep_vgc->varInsert("char_ram", &(TOP.vgc__DOT__char_ram), false, VLVT_UINT8, VLVD_NODIR|VLVF_PUB_RW, 1, 1 ,0,1999 ,7,0);
    __Vscopep_vgc->varInsert("char_ram_sel", &(TOP.vgc__DOT__char_ram_sel), false, VLVT_UINT8, VLVD_NODIR|VLVF_PUB_RW|VLVF_CONTINUOUSLY, 0, 0);
    __Vscopep_vgc->varInsert("clk", &(TOP.vgc__DOT__clk), false, VLVT_UINT8, VLVD_NODIR|VLVF_PUB_RW, 0, 0);
    __Vscopep_vgc->varInsert("cmd_busy", &(TOP.vgc__DOT__cmd_busy), false, VLVT_UINT8, VLVD_NODIR|VLVF_PUB_RW, 0, 0);
    __Vscopep_vgc->varInsert("cmd_cx", &(TOP.vgc__DOT__cmd_cx), false, VLVT_UINT16, VLVD_NODIR|VLVF_PUB_RW, 0, 1 ,8,0);
    __Vscopep_vgc->varInsert("cmd_cy", &(TOP.vgc__DOT__cmd_cy), false, VLVT_UINT8, VLVD_NODIR|VLVF_PUB_RW, 0, 1 ,7,0);
    __Vscopep_vgc->varInsert("cmd_dx", &(TOP.vgc__DOT__cmd_dx), false, VLVT_UINT16, VLVD_NODIR|VLVF_PUB_RW|VLVF_SIGNED, 0, 1 ,9,0);
    __Vscopep_vgc->varInsert("cmd_dy", &(TOP.vgc__DOT__cmd_dy), false, VLVT_UINT16, VLVD_NODIR|VLVF_PUB_RW|VLVF_SIGNED, 0, 1 ,9,0);
    __Vscopep_vgc->varInsert("cmd_err", &(TOP.vgc__DOT__cmd_err), false, VLVT_UINT16, VLVD_NODIR|VLVF_PUB_RW|VLVF_SIGNED, 0, 1 ,9,0);
    __Vscopep_vgc->varInsert("cmd_op", &(TOP.vgc__DOT__cmd_op), false, VLVT_UINT8, VLVD_NODIR|VLVF_PUB_RW, 0, 1 ,7,0);
    __Vscopep_vgc->varInsert("cmd_sx", &(TOP.vgc__DOT__cmd_sx), false, VLVT_UINT16, VLVD_NODIR|VLVF_PUB_RW|VLVF_SIGNED, 0, 1 ,9,0);
    __Vscopep_vgc->varInsert("cmd_sy", &(TOP.vgc__DOT__cmd_sy), false, VLVT_UINT16, VLVD_NODIR|VLVF_PUB_RW|VLVF_SIGNED, 0, 1 ,9,0);
    __Vscopep_vgc->varInsert("cmd_x", &(TOP.vgc__DOT__cmd_x), false, VLVT_UINT16, VLVD_NODIR|VLVF_PUB_RW|VLVF_SIGNED, 0, 1 ,9,0);
    __Vscopep_vgc->varInsert("cmd_x2", &(TOP.vgc__DOT__cmd_x2), false, VLVT_UINT16, VLVD_NODIR|VLVF_PUB_RW|VLVF_SIGNED, 0, 1 ,9,0);
    __Vscopep_vgc->varInsert("cmd_y", &(TOP.vgc__DOT__cmd_y), false, VLVT_UINT16, VLVD_NODIR|VLVF_PUB_RW|VLVF_SIGNED, 0, 1 ,9,0);
    __Vscopep_vgc->varInsert("cmd_y2", &(TOP.vgc__DOT__cmd_y2), false, VLVT_UINT16, VLVD_NODIR|VLVF_PUB_RW|VLVF_SIGNED, 0, 1 ,9,0);
    __Vscopep_vgc->varInsert("color_offset", &(TOP.vgc__DOT__color_offset), false, VLVT_UINT16, VLVD_NODIR|VLVF_PUB_RW|VLVF_CONTINUOUSLY, 0, 1 ,10,0);
    __Vscopep_vgc->varInsert("color_ram", &(TOP.vgc__DOT__color_ram), false, VLVT_UINT8, VLVD_NODIR|VLVF_PUB_RW, 1, 1 ,0,1999 ,7,0);
    __Vscopep_vgc->varInsert("color_ram_sel", &(TOP.vgc__DOT__color_ram_sel), false, VLVT_UINT8, VLVD_NODIR|VLVF_PUB_RW|VLVF_CONTINUOUSLY, 0, 0);
    __Vscopep_vgc->varInsert("copper_count", &(TOP.vgc__DOT__copper_count), false, VLVT_UINT16, VLVD_NODIR|VLVF_PUB_RW, 0, 1 ,8,0);
    __Vscopep_vgc->varInsert("copper_enabled", &(TOP.vgc__DOT__copper_enabled), false, VLVT_UINT8, VLVD_NODIR|VLVF_PUB_RW, 0, 0);
    __Vscopep_vgc->varInsert("copper_index", &(TOP.vgc__DOT__copper_index), false, VLVT_UINT16, VLVD_NODIR|VLVF_PUB_RW, 0, 1 ,8,0);
    __Vscopep_vgc->varInsert("copper_pos", &(TOP.vgc__DOT__copper_pos), false, VLVT_UINT32, VLVD_NODIR|VLVF_PUB_RW, 1, 1 ,0,255 ,16,0);
    __Vscopep_vgc->varInsert("copper_reg", &(TOP.vgc__DOT__copper_reg), false, VLVT_UINT8, VLVD_NODIR|VLVF_PUB_RW, 1, 1 ,0,255 ,7,0);
    __Vscopep_vgc->varInsert("copper_val", &(TOP.vgc__DOT__copper_val), false, VLVT_UINT8, VLVD_NODIR|VLVF_PUB_RW, 1, 1 ,0,255 ,7,0);
    __Vscopep_vgc->varInsert("cpu_addr", &(TOP.vgc__DOT__cpu_addr), false, VLVT_UINT16, VLVD_NODIR|VLVF_PUB_RW, 0, 1 ,15,0);
    __Vscopep_vgc->varInsert("cpu_ce", &(TOP.vgc__DOT__cpu_ce), false, VLVT_UINT8, VLVD_NODIR|VLVF_PUB_RW, 0, 0);
    __Vscopep_vgc->varInsert("cpu_rdata", &(TOP.vgc__DOT__cpu_rdata), false, VLVT_UINT8, VLVD_NODIR|VLVF_PUB_RW, 0, 1 ,7,0);
    __Vscopep_vgc->varInsert("cpu_re", &(TOP.vgc__DOT__cpu_re), false, VLVT_UINT8, VLVD_NODIR|VLVF_PUB_RW, 0, 0);
    __Vscopep_vgc->varInsert("cpu_wdata", &(TOP.vgc__DOT__cpu_wdata), false, VLVT_UINT8, VLVD_NODIR|VLVF_PUB_RW, 0, 1 ,7,0);
    __Vscopep_vgc->varInsert("cpu_we", &(TOP.vgc__DOT__cpu_we), false, VLVT_UINT8, VLVD_NODIR|VLVF_PUB_RW, 0, 0);
    __Vscopep_vgc->varInsert("cur_char", &(TOP.vgc__DOT__cur_char), false, VLVT_UINT8, VLVD_NODIR|VLVF_PUB_RW, 0, 1 ,7,0);
    __Vscopep_vgc->varInsert("cur_fg", &(TOP.vgc__DOT__cur_fg), false, VLVT_UINT8, VLVD_NODIR|VLVF_PUB_RW, 0, 1 ,3,0);
    __Vscopep_vgc->varInsert("cur_gfx", &(TOP.vgc__DOT__cur_gfx), false, VLVT_UINT8, VLVD_NODIR|VLVF_PUB_RW, 0, 1 ,3,0);
    __Vscopep_vgc->varInsert("cursor_blink", &(TOP.vgc__DOT__cursor_blink), false, VLVT_UINT8, VLVD_NODIR|VLVF_PUB_RW, 0, 0);
    __Vscopep_vgc->varInsert("cursor_here", &(TOP.vgc__DOT__cursor_here), false, VLVT_UINT8, VLVD_NODIR|VLVF_PUB_RW|VLVF_CONTINUOUSLY, 0, 0);
    __Vscopep_vgc->varInsert("cursor_x", &(TOP.vgc__DOT__cursor_x), false, VLVT_UINT8, VLVD_NODIR|VLVF_PUB_RW, 0, 1 ,6,0);
    __Vscopep_vgc->varInsert("cursor_y", &(TOP.vgc__DOT__cursor_y), false, VLVT_UINT8, VLVD_NODIR|VLVF_PUB_RW, 0, 1 ,4,0);
    __Vscopep_vgc->varInsert("fg_color", &(TOP.vgc__DOT__fg_color), false, VLVT_UINT8, VLVD_NODIR|VLVF_PUB_RW, 0, 1 ,3,0);
    __Vscopep_vgc->varInsert("font_byte", &(TOP.vgc__DOT__font_byte), false, VLVT_UINT8, VLVD_NODIR|VLVF_PUB_RW, 0, 1 ,7,0);
    __Vscopep_vgc->varInsert("font_line", &(TOP.vgc__DOT__font_line), false, VLVT_UINT8, VLVD_NODIR|VLVF_PUB_RW, 0, 1 ,2,0);
    __Vscopep_vgc->varInsert("font_pixel", &(TOP.vgc__DOT__font_pixel), false, VLVT_UINT8, VLVD_NODIR|VLVF_PUB_RW, 0, 1 ,2,0);
    __Vscopep_vgc->varInsert("font_rom", &(TOP.vgc__DOT__font_rom), false, VLVT_UINT8, VLVD_NODIR|VLVF_PUB_RW, 1, 1 ,0,2047 ,7,0);
    __Vscopep_vgc->varInsert("gfx_color", &(TOP.vgc__DOT__gfx_color), false, VLVT_UINT8, VLVD_NODIR|VLVF_PUB_RW, 0, 1 ,3,0);
    __Vscopep_vgc->varInsert("gfx_pixel", &(TOP.vgc__DOT__gfx_pixel), false, VLVT_UINT16, VLVD_NODIR|VLVF_PUB_RW, 0, 1 ,11,0);
    __Vscopep_vgc->varInsert("gfx_ram", &(TOP.vgc__DOT__gfx_ram), false, VLVT_UINT8, VLVD_NODIR|VLVF_PUB_RW, 1, 1 ,0,63999 ,3,0);
    __Vscopep_vgc->varInsert("gfx_x", &(TOP.vgc__DOT__gfx_x), false, VLVT_UINT16, VLVD_NODIR|VLVF_PUB_RW, 0, 1 ,8,0);
    __Vscopep_vgc->varInsert("gfx_y", &(TOP.vgc__DOT__gfx_y), false, VLVT_UINT8, VLVD_NODIR|VLVF_PUB_RW, 0, 1 ,7,0);
    __Vscopep_vgc->varInsert("h_count", &(TOP.vgc__DOT__h_count), false, VLVT_UINT16, VLVD_NODIR|VLVF_PUB_RW, 0, 1 ,9,0);
    __Vscopep_vgc->varInsert("h_sync_area", &(TOP.vgc__DOT__h_sync_area), false, VLVT_UINT8, VLVD_NODIR|VLVF_PUB_RW|VLVF_CONTINUOUSLY, 0, 0);
    __Vscopep_vgc->varInsert("h_visible", &(TOP.vgc__DOT__h_visible), false, VLVT_UINT8, VLVD_NODIR|VLVF_PUB_RW|VLVF_CONTINUOUSLY, 0, 0);
    __Vscopep_vgc->varInsert("in_text_area", &(TOP.vgc__DOT__in_text_area), false, VLVT_UINT8, VLVD_NODIR|VLVF_PUB_RW|VLVF_CONTINUOUSLY, 0, 0);
    __Vscopep_vgc->varInsert("key_data", &(TOP.vgc__DOT__key_data), false, VLVT_UINT8, VLVD_NODIR|VLVF_PUB_RW, 0, 1 ,7,0);
    __Vscopep_vgc->varInsert("key_valid", &(TOP.vgc__DOT__key_valid), false, VLVT_UINT8, VLVD_NODIR|VLVF_PUB_RW, 0, 0);
    __Vscopep_vgc->varInsert("mode", &(TOP.vgc__DOT__mode), false, VLVT_UINT8, VLVD_NODIR|VLVF_PUB_RW, 0, 1 ,2,0);
    __Vscopep_vgc->varInsert("palette", &(TOP.vgc__DOT__palette), false, VLVT_UINT16, VLVD_NODIR|VLVF_PUB_RW, 1, 1 ,0,15 ,11,0);
    __Vscopep_vgc->varInsert("pixel_color", &(TOP.vgc__DOT__pixel_color), false, VLVT_UINT16, VLVD_NODIR|VLVF_PUB_RW, 0, 1 ,11,0);
    __Vscopep_vgc->varInsert("pixel_on", &(TOP.vgc__DOT__pixel_on), false, VLVT_UINT8, VLVD_NODIR|VLVF_PUB_RW, 0, 0);
    __Vscopep_vgc->varInsert("real_row", &(TOP.vgc__DOT__real_row), false, VLVT_UINT8, VLVD_NODIR|VLVF_PUB_RW, 0, 1 ,4,0);
    __Vscopep_vgc->varInsert("reg_offset", &(TOP.vgc__DOT__reg_offset), false, VLVT_UINT8, VLVD_NODIR|VLVF_PUB_RW|VLVF_CONTINUOUSLY, 0, 1 ,4,0);
    __Vscopep_vgc->varInsert("regs", &(TOP.vgc__DOT__regs), false, VLVT_UINT8, VLVD_NODIR|VLVF_PUB_RW, 1, 1 ,0,31 ,7,0);
    __Vscopep_vgc->varInsert("rst", &(TOP.vgc__DOT__rst), false, VLVT_UINT8, VLVD_NODIR|VLVF_PUB_RW, 0, 0);
    __Vscopep_vgc->varInsert("scroll_col", &(TOP.vgc__DOT__scroll_col), false, VLVT_UINT8, VLVD_NODIR|VLVF_PUB_RW, 0, 1 ,6,0);
    __Vscopep_vgc->varInsert("scroll_offset", &(TOP.vgc__DOT__scroll_offset), false, VLVT_UINT8, VLVD_NODIR|VLVF_PUB_RW, 0, 1 ,4,0);
    __Vscopep_vgc->varInsert("scroll_pending", &(TOP.vgc__DOT__scroll_pending), false, VLVT_UINT8, VLVD_NODIR|VLVF_PUB_RW, 0, 0);
    __Vscopep_vgc->varInsert("spr_dx", &(TOP.vgc__DOT__spr_dx), false, VLVT_UINT16, VLVD_NODIR|VLVF_PUB_RW|VLVF_SIGNED, 1, 1 ,0,15 ,9,0);
    __Vscopep_vgc->varInsert("spr_dy", &(TOP.vgc__DOT__spr_dy), false, VLVT_UINT16, VLVD_NODIR|VLVF_PUB_RW|VLVF_SIGNED, 1, 1 ,0,15 ,9,0);
    __Vscopep_vgc->varInsert("spr_enable", &(TOP.vgc__DOT__spr_enable), false, VLVT_UINT8, VLVD_NODIR|VLVF_PUB_RW, 1, 0 ,0,15);
    __Vscopep_vgc->varInsert("spr_field", &(TOP.vgc__DOT__spr_field), false, VLVT_UINT8, VLVD_NODIR|VLVF_PUB_RW|VLVF_CONTINUOUSLY, 0, 1 ,2,0);
    __Vscopep_vgc->varInsert("spr_flip_h", &(TOP.vgc__DOT__spr_flip_h), false, VLVT_UINT8, VLVD_NODIR|VLVF_PUB_RW, 1, 0 ,0,15);
    __Vscopep_vgc->varInsert("spr_flip_v", &(TOP.vgc__DOT__spr_flip_v), false, VLVT_UINT8, VLVD_NODIR|VLVF_PUB_RW, 1, 0 ,0,15);
    __Vscopep_vgc->varInsert("spr_fx", &(TOP.vgc__DOT__spr_fx), false, VLVT_UINT8, VLVD_NODIR|VLVF_PUB_RW, 1, 1 ,0,15 ,3,0);
    __Vscopep_vgc->varInsert("spr_fy", &(TOP.vgc__DOT__spr_fy), false, VLVT_UINT8, VLVD_NODIR|VLVF_PUB_RW, 1, 1 ,0,15 ,3,0);
    __Vscopep_vgc->varInsert("spr_index", &(TOP.vgc__DOT__spr_index), false, VLVT_UINT8, VLVD_NODIR|VLVF_PUB_RW|VLVF_CONTINUOUSLY, 0, 1 ,3,0);
    __Vscopep_vgc->varInsert("spr_pix", &(TOP.vgc__DOT__spr_pix), false, VLVT_UINT8, VLVD_NODIR|VLVF_PUB_RW, 1, 1 ,0,15 ,3,0);
    __Vscopep_vgc->varInsert("spr_pixel", &(TOP.vgc__DOT__spr_pixel), false, VLVT_UINT8, VLVD_NODIR|VLVF_PUB_RW, 0, 1 ,3,0);
    __Vscopep_vgc->varInsert("spr_pixel_hit", &(TOP.vgc__DOT__spr_pixel_hit), false, VLVT_UINT8, VLVD_NODIR|VLVF_PUB_RW, 0, 0);
    __Vscopep_vgc->varInsert("spr_pixel_pri", &(TOP.vgc__DOT__spr_pixel_pri), false, VLVT_UINT8, VLVD_NODIR|VLVF_PUB_RW, 0, 1 ,1,0);
    __Vscopep_vgc->varInsert("spr_pri", &(TOP.vgc__DOT__spr_pri), false, VLVT_UINT8, VLVD_NODIR|VLVF_PUB_RW, 1, 1 ,0,15 ,1,0);
    __Vscopep_vgc->varInsert("spr_reg_sel", &(TOP.vgc__DOT__spr_reg_sel), false, VLVT_UINT8, VLVD_NODIR|VLVF_PUB_RW|VLVF_CONTINUOUSLY, 0, 0);
    __Vscopep_vgc->varInsert("spr_sa", &(TOP.vgc__DOT__spr_sa), false, VLVT_UINT16, VLVD_NODIR|VLVF_PUB_RW, 1, 1 ,0,15 ,10,0);
    __Vscopep_vgc->varInsert("spr_shape", &(TOP.vgc__DOT__spr_shape), false, VLVT_UINT8, VLVD_NODIR|VLVF_PUB_RW, 1, 1 ,0,15 ,3,0);
    __Vscopep_vgc->varInsert("spr_vis", &(TOP.vgc__DOT__spr_vis), false, VLVT_UINT8, VLVD_NODIR|VLVF_PUB_RW, 1, 0 ,0,15);
    __Vscopep_vgc->varInsert("spr_x", &(TOP.vgc__DOT__spr_x), false, VLVT_UINT16, VLVD_NODIR|VLVF_PUB_RW, 1, 1 ,0,15 ,8,0);
    __Vscopep_vgc->varInsert("spr_y", &(TOP.vgc__DOT__spr_y), false, VLVT_UINT8, VLVD_NODIR|VLVF_PUB_RW, 1, 1 ,0,15 ,7,0);
    __Vscopep_vgc->varInsert("sprite_shapes", &(TOP.vgc__DOT__sprite_shapes), false, VLVT_UINT8, VLVD_NODIR|VLVF_PUB_RW, 1, 1 ,0,2047 ,7,0);
    __Vscopep_vgc->varInsert("text_col", &(TOP.vgc__DOT__text_col), false, VLVT_UINT8, VLVD_NODIR|VLVF_PUB_RW, 0, 1 ,6,0);
    __Vscopep_vgc->varInsert("text_line", &(TOP.vgc__DOT__text_line), false, VLVT_UINT16, VLVD_NODIR|VLVF_PUB_RW, 0, 1 ,9,0);
    __Vscopep_vgc->varInsert("text_pixel", &(TOP.vgc__DOT__text_pixel), false, VLVT_UINT16, VLVD_NODIR|VLVF_PUB_RW, 0, 1 ,11,0);
    __Vscopep_vgc->varInsert("text_row", &(TOP.vgc__DOT__text_row), false, VLVT_UINT8, VLVD_NODIR|VLVF_PUB_RW, 0, 1 ,4,0);
    __Vscopep_vgc->varInsert("v_count", &(TOP.vgc__DOT__v_count), false, VLVT_UINT16, VLVD_NODIR|VLVF_PUB_RW, 0, 1 ,9,0);
    __Vscopep_vgc->varInsert("v_sync_area", &(TOP.vgc__DOT__v_sync_area), false, VLVT_UINT8, VLVD_NODIR|VLVF_PUB_RW|VLVF_CONTINUOUSLY, 0, 0);
    __Vscopep_vgc->varInsert("v_visible", &(TOP.vgc__DOT__v_visible), false, VLVT_UINT8, VLVD_NODIR|VLVF_PUB_RW|VLVF_CONTINUOUSLY, 0, 0);
    __Vscopep_vgc->varInsert("vgc_reg_sel", &(TOP.vgc__DOT__vgc_reg_sel), false, VLVT_UINT8, VLVD_NODIR|VLVF_PUB_RW|VLVF_CONTINUOUSLY, 0, 0);
    __Vscopep_vgc->varInsert("vid_b", &(TOP.vgc__DOT__vid_b), false, VLVT_UINT8, VLVD_NODIR|VLVF_PUB_RW, 0, 1 ,3,0);
    __Vscopep_vgc->varInsert("vid_de", &(TOP.vgc__DOT__vid_de), false, VLVT_UINT8, VLVD_NODIR|VLVF_PUB_RW, 0, 0);
    __Vscopep_vgc->varInsert("vid_g", &(TOP.vgc__DOT__vid_g), false, VLVT_UINT8, VLVD_NODIR|VLVF_PUB_RW, 0, 1 ,3,0);
    __Vscopep_vgc->varInsert("vid_hsync", &(TOP.vgc__DOT__vid_hsync), false, VLVT_UINT8, VLVD_NODIR|VLVF_PUB_RW, 0, 0);
    __Vscopep_vgc->varInsert("vid_r", &(TOP.vgc__DOT__vid_r), false, VLVT_UINT8, VLVD_NODIR|VLVF_PUB_RW, 0, 1 ,3,0);
    __Vscopep_vgc->varInsert("vid_vsync", &(TOP.vgc__DOT__vid_vsync), false, VLVT_UINT8, VLVD_NODIR|VLVF_PUB_RW, 0, 0);
    __Vscopep_vgc->varInsert("visible", &(TOP.vgc__DOT__visible), false, VLVT_UINT8, VLVD_NODIR|VLVF_PUB_RW|VLVF_CONTINUOUSLY, 0, 0);
    __Vscopep_vgc__unnamedblk1->varInsert("i", &(TOP.vgc__DOT__unnamedblk1__DOT__i), false, VLVT_UINT32, VLVD_NODIR|VLVF_PUB_RW|VLVF_DPI_CLAY|VLVF_SIGNED, 0, 1 ,31,0);
    __Vscopep_vgc__unnamedblk2->varInsert("i", &(TOP.vgc__DOT__unnamedblk2__DOT__i), false, VLVT_UINT32, VLVD_NODIR|VLVF_PUB_RW|VLVF_DPI_CLAY|VLVF_SIGNED, 0, 1 ,31,0);
    __Vscopep_vgc__unnamedblk3->varInsert("i", &(TOP.vgc__DOT__unnamedblk3__DOT__i), false, VLVT_UINT32, VLVD_NODIR|VLVF_PUB_RW|VLVF_DPI_CLAY|VLVF_SIGNED, 0, 1 ,31,0);
    __Vscopep_vgc__unnamedblk4->varInsert("i", &(TOP.vgc__DOT__unnamedblk4__DOT__i), false, VLVT_UINT32, VLVD_NODIR|VLVF_PUB_RW|VLVF_DPI_CLAY|VLVF_SIGNED, 0, 1 ,31,0);
    __Vscopep_vgc__unnamedblk5->varInsert("i", &(TOP.vgc__DOT__unnamedblk5__DOT__i), false, VLVT_UINT32, VLVD_NODIR|VLVF_PUB_RW|VLVF_DPI_CLAY|VLVF_SIGNED, 0, 1 ,31,0);
    __Vscopep_vgc__unnamedblk6->varInsert("i", &(TOP.vgc__DOT__unnamedblk6__DOT__i), false, VLVT_UINT32, VLVD_NODIR|VLVF_PUB_RW|VLVF_DPI_CLAY|VLVF_SIGNED, 0, 1 ,31,0);
    __Vscopep_vgc__unnamedblk7->varInsert("s", &(TOP.vgc__DOT__unnamedblk7__DOT__s), false, VLVT_UINT32, VLVD_NODIR|VLVF_PUB_RW|VLVF_DPI_CLAY|VLVF_SIGNED, 0, 1 ,31,0);
    __Vscopep_vgc__unnamedblk8->varInsert("s", &(TOP.vgc__DOT__unnamedblk8__DOT__s), false, VLVT_UINT32, VLVD_NODIR|VLVF_PUB_RW|VLVF_DPI_CLAY|VLVF_SIGNED, 0, 1 ,31,0);
}

Vvgc__Syms::~Vvgc__Syms() {
    // Tear down scopes
    VL_DO_CLEAR(delete __Vscopep_TOP, __Vscopep_TOP = nullptr);
    VL_DO_CLEAR(delete __Vscopep_vgc, __Vscopep_vgc = nullptr);
    VL_DO_CLEAR(delete __Vscopep_vgc__unnamedblk1, __Vscopep_vgc__unnamedblk1 = nullptr);
    VL_DO_CLEAR(delete __Vscopep_vgc__unnamedblk2, __Vscopep_vgc__unnamedblk2 = nullptr);
    VL_DO_CLEAR(delete __Vscopep_vgc__unnamedblk3, __Vscopep_vgc__unnamedblk3 = nullptr);
    VL_DO_CLEAR(delete __Vscopep_vgc__unnamedblk4, __Vscopep_vgc__unnamedblk4 = nullptr);
    VL_DO_CLEAR(delete __Vscopep_vgc__unnamedblk5, __Vscopep_vgc__unnamedblk5 = nullptr);
    VL_DO_CLEAR(delete __Vscopep_vgc__unnamedblk6, __Vscopep_vgc__unnamedblk6 = nullptr);
    VL_DO_CLEAR(delete __Vscopep_vgc__unnamedblk7, __Vscopep_vgc__unnamedblk7 = nullptr);
    VL_DO_CLEAR(delete __Vscopep_vgc__unnamedblk8, __Vscopep_vgc__unnamedblk8 = nullptr);
    // Tear down sub module instances
}
