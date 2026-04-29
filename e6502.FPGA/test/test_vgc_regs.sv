// VGC register-level tests — exercises every writable register in the
// $A000-$A01F core register file and verifies the write goes to the right
// internal state with correct bit masking. Also exercises readback via
// cpu bus and debug port to ensure mirror reads agree with the DUT regs.

`timescale 1ns/1ps

module test_vgc_regs;

    `include "vgc_tb.svh"

    // -----------------------------------------------------------------------
    // Tests
    // -----------------------------------------------------------------------

    task automatic test_mode_register();
        logic [7:0] rb;
        $display("");
        $display("Test: MODE register ($A000) — 3-bit value, masked on write");
        for (int m = 0; m < 5; m++) begin
            bus_write(REG_MODE_A, 8'(m));
            step(2);
            check_eq($sformatf("mode=%0d register holds value", m),
                     int'(dut.mode), m);
            bus_read(REG_MODE_A, rb);
            check_eq($sformatf("mode=%0d readback via cpu bus",   m),
                     int'(rb), m);
        end
        // Write bits outside the mask and verify only the low 3 bits latch
        bus_write(REG_MODE_A, 8'hF7);   // low 3 bits = 0b111 = 7
        step(2);
        check_eq("mode register masks to 3 bits (write 0xF7 → 7)",
                 int'(dut.mode), 7);
    endtask

    task automatic test_color_registers();
        logic [7:0] rb;
        $display("");
        $display("Test: BG/FG/BORDER color registers — 4-bit");
        for (int c = 0; c < 16; c++) begin
            bus_write(REG_BGCOL_A,  8'(c));   step(2);
            bus_write(REG_FGCOL_A,  8'(c));   step(2);
            bus_write(16'hA00D,      8'(c));   step(2);
            check_eq($sformatf("bg_color=%0d",     c), int'(dut.bg_color),     c);
            check_eq($sformatf("fg_color=%0d",     c), int'(dut.fg_color),     c);
            check_eq($sformatf("border_color=%0d", c), int'(dut.border_color), c);
            bus_read(16'hA00D, rb); step(2);
            check_eq($sformatf("$A00D reads border color (c=%0d)", c), int'(rb), c);
        end
        // High nibble must be masked off
        bus_write(REG_BGCOL_A, 8'hF5); step(2);
        check_eq("bg_color masks to 4 bits (write 0xF5 → 5)",
                 int'(dut.bg_color), 5);
    endtask

    task automatic test_border_masks_sprites();
        $display("");
        $display("Test: border compositor masks sprite pixels outside Nova canvas");

        force dut.visible_d2 = 1'b1;
        force dut.in_text_area_d2 = 1'b0;
        force dut.border_color = 4'd14;
        force dut.mode = 3'd3;
        force dut.spr_pixel_hit = 1'b1;
        force dut.spr_pixel = 4'd2;
        force dut.spr_pixel_pri = 2'd2;
        #1;
        check_eq("border pixel remains border color when sprite is behind it",
                 int'(dut.pixel_color), 12'h08F);

        force dut.in_text_area_d2 = 1'b1;
        #1;
        check_eq("canvas pixel still shows front-priority sprite",
                 int'(dut.pixel_color), 12'h800);

        release dut.visible_d2;
        release dut.in_text_area_d2;
        release dut.border_color;
        release dut.mode;
        release dut.spr_pixel_hit;
        release dut.spr_pixel;
        release dut.spr_pixel_pri;
    endtask

    task automatic test_sprite_scanline_read_prefetch();
        $display("");
        $display("Test: sprite scanline reader prefetches one physical pixel");

        force dut.h_count_d2 = 10'd39;
        #1;
        check_eq("pixel before canvas prefetches sprite x=20",
                 int'(dut.sprite_inst.slb_b_addr), 20);

        force dut.h_count_d2 = 10'd40;
        #1;
        check_eq("first canvas pixel keeps sprite x=20",
                 int'(dut.sprite_inst.slb_b_addr), 20);

        force dut.h_count_d2 = 10'd41;
        #1;
        check_eq("second canvas pixel prefetches next sprite x=21",
                 int'(dut.sprite_inst.slb_b_addr), 21);

        release dut.h_count_d2;
    endtask

    task automatic test_cursor_xy();
        $display("");
        $display("Test: cursor_x (7-bit), cursor_y clamps to visible 50-row text area");
        // cursor_x range 0..127
        bus_write(REG_CURSORX_A, 8'd0);    step(2);
        check_eq("cursor_x=0",  int'(dut.cursor_x), 0);
        bus_write(REG_CURSORX_A, 8'd79);   step(2);
        check_eq("cursor_x=79", int'(dut.cursor_x), 79);
        // top bit should be masked
        bus_write(REG_CURSORX_A, 8'hFF);   step(2);
        check_eq("cursor_x masks to 7 bits (0xFF → 127)",
                 int'(dut.cursor_x), 127);

        bus_write(REG_CURSORY_A, 8'd0);    step(2);
        check_eq("cursor_y=0",  int'(dut.cursor_y), 0);
        bus_write(REG_CURSORY_A, 8'd49);   step(2);
        check_eq("cursor_y=49 (max valid row)", int'(dut.cursor_y), 49);
        bus_write(REG_CURSORY_A, 8'd50);   step(2);
        check_eq("cursor_y clamps 50 to 49", int'(dut.cursor_y), 49);
        // Top bits masked to 6 bits, then clamped to the 50-row text area.
        bus_write(REG_CURSORY_A, 8'hFF);   step(2);
        check_eq("cursor_y masks/clamps 0xFF to 49",
                 int'(dut.cursor_y), 49);
    endtask

    task automatic test_scroll_registers();
        $display("");
        $display("Test: scroll_x, scroll_y — full 8 bits");
        bus_write(16'hA005, 8'hAB); step(2);
        check_eq("scroll_x = 0xAB", int'(dut.scroll_x), 8'hAB);
        bus_write(16'hA006, 8'h5C); step(2);
        check_eq("scroll_y = 0x5C", int'(dut.scroll_y), 8'h5C);
    endtask

    task automatic test_font_slot();
        $display("");
        $display("Test: font_slot (valid 0..2, clamps 3..7 to 0)");
        bus_write(16'hA007, 8'd2); step(2);
        check_eq("font_slot=2 (valid)", int'(dut.font_slot), 2);
        bus_write(16'hA007, 8'd3); step(2);
        check_eq("font_slot=3 clamps to 0", int'(dut.font_slot), 0);
        bus_write(16'hA007, 8'hFF); step(2);
        check_eq("font_slot=0xFF clamps to 0 (was 7)",
                 int'(dut.font_slot), 0);
    endtask

    task automatic test_gfx_color();
        $display("");
        $display("Test: gfx_color (4-bit)");
        bus_write(16'hA008, 8'd12); step(2);
        check_eq("gfx_color=12", int'(dut.gfx_color), 12);
        bus_write(16'hA008, 8'hF3); step(2);
        check_eq("gfx_color masks to 4 bits (0xF3 → 3)",
                 int'(dut.gfx_color), 3);
    endtask

    task automatic test_cursor_enable();
        $display("");
        $display("Test: cursor_enable at $A00A (1-bit)");
        bus_write(16'hA00A, 8'd0); step(2);
        check_eq("cursor_enable=0", int'(dut.cursor_enable), 0);
        bus_write(16'hA00A, 8'd1); step(2);
        check_eq("cursor_enable=1", int'(dut.cursor_enable), 1);
        // Any non-zero write of only bit 0 enables
        bus_write(16'hA00A, 8'hFE); step(2);
        check_eq("cursor_enable bit 0 only (0xFE → 0)",
                 int'(dut.cursor_enable), 0);
    endtask

    task automatic test_collision_clear_on_write();
        $display("");
        $display("Test: collision_ss / collision_bg cleared on write");
        // Force some bits into the collision registers
        dut.collision_ss = 8'hAA;
        dut.collision_bg = 8'h55;
        step(2);
        check_eq("pre-write: collision_ss=0xAA", int'(dut.collision_ss), 8'hAA);
        check_eq("pre-write: collision_bg=0x55", int'(dut.collision_bg), 8'h55);
        bus_write(16'hA00B, 8'h00); step(2);
        check_eq("after write $A00B: collision_ss cleared",
                 int'(dut.collision_ss), 0);
        bus_write(16'hA00C, 8'h00); step(2);
        check_eq("after write $A00C: collision_bg cleared",
                 int'(dut.collision_bg), 0);
    endtask

    task automatic test_irq_ctrl();
        $display("");
        $display("Test: irq_ctrl at $A01F — full 8 bits");
        bus_write(16'hA01F, 8'hA5); step(2);
        check_eq("irq_ctrl = 0xA5", int'(dut.irq_ctrl), 8'hA5);
    endtask

    // CMD_MEMREAD = $19. Params: regs[17]=space, regs[18]=addr_lo, regs[19]=addr_hi.
    // Result lands in regs[20] ($A014). Spaces match the VDC-style VRAM port:
    // 1=char, 2=color, 3=gfx, 4=sprite, 6=tile.
    task automatic test_memread_char();
        logic [7:0] rb;
        $display("");
        $display("Test: CMD_MEMREAD (space 1 = char RAM)");
        dut.text_inst.char_mem.mem[16'h0123] = 8'h5A;
        step(2);
        write_param(0, VPLANE_CHAR_A);  // space = char
        write_param(1, 8'h23);          // addr_lo
        write_param(2, 8'h01);          // addr_hi → 0x0123
        write_cmd(8'h19);               // MemRead
        wait_cmd_done();
        bus_read(16'hA014, rb);         // regs[20]
        check_eq("MemRead char[0x0123] → regs[20] = 0x5A", int'(rb), 8'h5A);
    endtask

    task automatic test_memread_color();
        logic [7:0] rb;
        $display("");
        $display("Test: CMD_MEMREAD (space 2 = color RAM)");
        dut.text_inst.color_mem.mem[16'h0456] = 8'h3C;
        step(2);
        write_param(0, VPLANE_COLOR_A); // space = color
        write_param(1, 8'h56);
        write_param(2, 8'h04);
        write_cmd(8'h19);
        wait_cmd_done();
        bus_read(16'hA014, rb);
        check_eq("MemRead color[0x0456] → regs[20] = 0x3C", int'(rb), 8'h3C);
    endtask

    task automatic test_memread_gfx();
        logic [7:0] rb;
        $display("");
        $display("Test: CMD_MEMREAD (space 3 = gfx RAM, 4-bit)");
        dut.gfx_inst.gfx_mem.mem[16'h1234] = 4'hB;
        step(2);
        write_param(0, VPLANE_GFX_A);
        write_param(1, 8'h34);
        write_param(2, 8'h12);
        write_cmd(8'h19);
        wait_cmd_done();
        bus_read(16'hA014, rb);
        check_eq("MemRead gfx[0x1234] → regs[20] = 0x0B", int'(rb), 8'h0B);
    endtask

    task automatic test_memread_sprite();
        logic [7:0] rb;
        $display("");
        $display("Test: CMD_MEMREAD (space 4 = sprite RAM)");
        poke_spr_pending_addr(11'h1AB, 8'hDE);
        step(2);
        write_param(0, VPLANE_SPRITE_A);
        write_param(1, 8'hAB);
        write_param(2, 8'h01);
        write_cmd(8'h19);
        wait_cmd_done();
        bus_read(16'hA014, rb);
        check_eq("MemRead sprite[0x01AB] → regs[20] = 0xDE", int'(rb), 8'hDE);
    endtask

    task automatic test_cpu_read_char_ram();
        logic [7:0] rb;
        $display("");
        $display("Test: CPU bus reads char RAM through VDATA");
        // Plant known bytes
        dut.text_inst.char_mem.mem[0]    = 8'h41;
        dut.text_inst.char_mem.mem[79]   = 8'h42;
        dut.text_inst.char_mem.mem[3999] = 8'h43;
        step(2);
        vram_read(VPLANE_CHAR_A, 0, rb);       check_eq("read char[0]",       int'(rb), 8'h41);
        vram_read(VPLANE_CHAR_A, 79, rb);      check_eq("read char[79]",      int'(rb), 8'h42);
        vram_read(VPLANE_CHAR_A, 3999, rb);    check_eq("read char[3999]",    int'(rb), 8'h43);
    endtask

    task automatic test_cpu_read_color_ram();
        logic [7:0] rb;
        $display("");
        $display("Test: CPU bus reads color RAM through VDATA");
        dut.text_inst.color_mem.mem[0]    = 8'hAA;
        dut.text_inst.color_mem.mem[500]  = 8'h55;
        dut.text_inst.color_mem.mem[3999] = 8'h33;
        step(2);
        vram_read(VPLANE_COLOR_A, 0, rb);       check_eq("color[0]",    int'(rb), 8'hAA);
        vram_read(VPLANE_COLOR_A, 500, rb);     check_eq("color[500]",  int'(rb), 8'h55);
        vram_read(VPLANE_COLOR_A, 3999, rb);    check_eq("color[3999]", int'(rb), 8'h33);
    endtask

    // Sprite register readback — $A040-$A0BF was write-only before this test.
    // Each sprite has 8 bytes: X lo, X hi, Y lo, reserved, shape, flags, pri, trans.
    // Flags byte: bit 7 = enable, bit 1 = flip_v, bit 0 = flip_h.
    task automatic test_sprite_register_readback();
        logic [7:0] rb;
        logic [15:0] base;
        $display("");
        $display("Test: sprite register readback ($A040-$A0BF)");

        // Sprite 0 — X=$1234, Y=$78, shape=7, enable+flip_h, pri=2, trans=$A.
        // The former Y-high byte is reserved and must read back as zero.
        base = 16'hA040;
        bus_write(base + 0, 8'h34); step(2);
        bus_write(base + 1, 8'h12); step(2);
        bus_write(base + 2, 8'h78); step(2);
        bus_write(base + 3, 8'h56); step(2);
        bus_write(base + 4, 8'h07); step(2);
        bus_write(base + 5, 8'h81); step(2);  // enable+flip_h
        bus_write(base + 6, 8'h02); step(2);
        bus_write(base + 7, 8'h0A); step(2);
        bus_read (base + 0, rb); check_eq("spr0 X lo  = 0x34", int'(rb), 8'h34);
        bus_read (base + 1, rb); check_eq("spr0 X hi  = 0x12", int'(rb), 8'h12);
        bus_read (base + 2, rb); check_eq("spr0 Y lo  = 0x78", int'(rb), 8'h78);
        bus_read (base + 3, rb); check_eq("spr0 Y hi reserved = 0x00", int'(rb), 8'h00);
        bus_read (base + 4, rb); check_eq("spr0 shape = 0x07", int'(rb), 8'h07);
        bus_read (base + 5, rb); check_eq("spr0 flags = 0x81", int'(rb), 8'h81);
        bus_read (base + 6, rb); check_eq("spr0 pri   = 0x02", int'(rb), 8'h02);
        bus_read (base + 7, rb); check_eq("spr0 trans = 0x0A", int'(rb), 8'h0A);

        // Sprite 15 — distinct values, confirms address decode at upper bound
        base = 16'hA040 + 16'd120;  // 15*8 = 120
        bus_write(base + 0, 8'hAB); step(2);
        bus_write(base + 1, 8'hCD); step(2);
        bus_write(base + 3, 8'hEF); step(2);
        bus_write(base + 5, 8'h82); step(2);  // enable+flip_v
        bus_read (base + 0, rb); check_eq("spr15 X lo  = 0xAB", int'(rb), 8'hAB);
        bus_read (base + 1, rb); check_eq("spr15 X hi  = 0xCD", int'(rb), 8'hCD);
        bus_read (base + 3, rb); check_eq("spr15 Y hi reserved = 0x00", int'(rb), 8'h00);
        bus_read (base + 5, rb); check_eq("spr15 flags = 0x82", int'(rb), 8'h82);

        // Spr0 must be independent of spr15 writes
        base = 16'hA040;
        bus_read (base + 0, rb); check_eq("spr0 X lo independent", int'(rb), 8'h34);
    endtask

    task automatic test_sprite_count_reg();
        logic [7:0] rb;
        $display("");
        $display("Test: $A009 returns count of enabled sprites");
        // All disabled
        for (int i = 0; i < 16; i++)
            dut.spr_next_enable[i] = 0;
        step(2);
        bus_read(16'hA009, rb);   check_eq("count=0 initially", int'(rb), 0);

        // Enable 3 sprites
        dut.spr_next_enable[1] = 1;
        dut.spr_next_enable[5] = 1;
        dut.spr_next_enable[15] = 1;
        step(2);
        bus_read(16'hA009, rb);   check_eq("count=3 after enabling 3", int'(rb), 3);

        // Enable all 16
        for (int i = 0; i < 16; i++)
            dut.spr_next_enable[i] = 1;
        step(2);
        bus_read(16'hA009, rb);   check_eq("count=16 (all enabled)", int'(rb), 16);
    endtask

    // Drive the VGC's blitter-side read port, sample blt_rdata after
    // dpram latency settles.
    task automatic blt_read(input logic [2:0] space, input logic [15:0] addr,
                             output logic [7:0] data);
        @(posedge clk);
        tb_blt_space <= space;
        tb_blt_addr  <= addr;
        tb_blt_re    <= 1;
        @(posedge clk);
        tb_blt_re    <= 0;
        // Data path: cycle N re=1 → addr latched on cycle N+1 posedge (by
        // blt_rd_pending always_ff) → blt_rd_latch valid at end of N+1 →
        // blt_rdata (combinational from latch) visible at start of N+2.
        repeat(3) @(posedge clk);
        data = tb_blt_rdata;
    endtask

    task automatic test_blt_read_char();
        logic [7:0] d;
        $display("");
        $display("Test: blitter-port read of char RAM");
        dut.text_inst.char_mem.mem[11'h0AB] = 8'hC3;
        step(4);
        blt_read(3'd1, 16'h00AB, d);
        check_eq("blt read char[0x0AB]", int'(d), 8'hC3);
    endtask

    task automatic test_blt_read_color();
        logic [7:0] d;
        $display("");
        $display("Test: blitter-port read of color RAM");
        dut.text_inst.color_mem.mem[11'h012] = 8'h7E;
        step(4);
        blt_read(3'd2, 16'h0012, d);
        check_eq("blt read color[0x012]", int'(d), 8'h7E);
    endtask

    task automatic test_blt_read_gfx();
        logic [7:0] d;
        $display("");
        $display("Test: blitter-port read of gfx RAM");
        dut.gfx_inst.gfx_mem.mem[16'h1F0] = 4'hA;
        step(4);
        blt_read(3'd3, 16'h01F0, d);
        check_eq("blt read gfx[0x1F0]", int'(d), 8'h0A);
    endtask

    task automatic test_blt_read_sprite();
        logic [7:0] d;
        $display("");
        $display("Test: blitter-port read of sprite RAM");
        poke_spr_pending_addr(11'h200, 8'h91);
        step(4);
        blt_read(3'd4, 16'h0200, d);
        check_eq("blt read sprite[0x200]", int'(d), 8'h91);
    endtask

    task automatic test_independent_registers();
        $display("");
        $display("Test: writing each register does not disturb others");
        // Set a recognizable fingerprint
        bus_write(REG_MODE_A,     8'd2);   step(2);
        bus_write(REG_BGCOL_A,    8'd3);   step(2);
        bus_write(REG_FGCOL_A,    8'd4);   step(2);
        bus_write(REG_CURSORX_A,  8'd5);   step(2);
        bus_write(REG_CURSORY_A,  8'd6);   step(2);
        bus_write(16'hA005,       8'd7);   step(2);
        bus_write(16'hA006,       8'd8);   step(2);
        bus_write(16'hA007,       8'd1);   step(2);
        bus_write(16'hA008,       8'd9);   step(2);
        bus_write(16'hA00A,       8'd1);   step(2);
        // $A00D write accepted but no-op — border removed, nothing to assert
        bus_write(16'hA00D,       8'd11);  step(2);
        bus_write(16'hA01F,       8'hC3);  step(2);

        check_eq("mode stable",         int'(dut.mode),          2);
        check_eq("bg_color stable",     int'(dut.bg_color),      3);
        check_eq("fg_color stable",     int'(dut.fg_color),      4);
        check_eq("cursor_x stable",     int'(dut.cursor_x),      5);
        check_eq("cursor_y stable",     int'(dut.cursor_y),      6);
        check_eq("scroll_x stable",     int'(dut.scroll_x),      7);
        check_eq("scroll_y stable",     int'(dut.scroll_y),      8);
        check_eq("font_slot stable",    int'(dut.font_slot),     1);
        check_eq("gfx_color stable",    int'(dut.gfx_color),     9);
        check_eq("cursor_enable stable", int'(dut.cursor_enable), 1);
        check_eq("irq_ctrl stable",     int'(dut.irq_ctrl),      8'hC3);
    endtask

    // -----------------------------------------------------------------------
    // Runner
    // -----------------------------------------------------------------------
    initial begin
        $display("=== VGC Register-Level Tests ===");
        do_reset();

        test_mode_register();
        test_color_registers();
        test_border_masks_sprites();
        test_sprite_scanline_read_prefetch();
        test_cursor_xy();
        test_scroll_registers();
        test_font_slot();
        test_gfx_color();
        test_cursor_enable();
        test_collision_clear_on_write();
        test_irq_ctrl();
        test_memread_char();
        test_memread_color();
        test_memread_gfx();
        test_memread_sprite();
        test_cpu_read_char_ram();
        test_cpu_read_color_ram();
        test_sprite_register_readback();
        test_sprite_count_reg();
        test_blt_read_char();
        test_blt_read_color();
        test_blt_read_gfx();
        test_blt_read_sprite();
        test_independent_registers();

        summary();
        $finish;
    end

    initial begin
        #2000000000;
        $display("FAIL: global timeout");
        $finish;
    end

endmodule
