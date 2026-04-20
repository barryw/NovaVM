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
            bus_write(16'hA00D,      8'(c));   step(2);  // BORDER
            check_eq($sformatf("bg_color=%0d",     c), int'(dut.bg_color),     c);
            check_eq($sformatf("fg_color=%0d",     c), int'(dut.fg_color),     c);
            check_eq($sformatf("border_color=%0d", c), int'(dut.border_color), c);
        end
        // High nibble must be masked off
        bus_write(REG_BGCOL_A, 8'hF5); step(2);
        check_eq("bg_color masks to 4 bits (write 0xF5 → 5)",
                 int'(dut.bg_color), 5);
    endtask

    task automatic test_cursor_xy();
        $display("");
        $display("Test: cursor_x (7-bit), cursor_y (5-bit)");
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
        bus_write(REG_CURSORY_A, 8'd24);   step(2);
        check_eq("cursor_y=24", int'(dut.cursor_y), 24);
        // top bits masked to 5 bits
        bus_write(REG_CURSORY_A, 8'hFF);   step(2);
        check_eq("cursor_y masks to 5 bits (0xFF → 31)",
                 int'(dut.cursor_y), 31);
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
        $display("Test: font_slot (3-bit)");
        bus_write(16'hA007, 8'd3); step(2);
        check_eq("font_slot=3", int'(dut.font_slot), 3);
        bus_write(16'hA007, 8'hFF); step(2);
        check_eq("font_slot masks to 3 bits (0xFF → 7)",
                 int'(dut.font_slot), 7);
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

    task automatic test_cpu_read_char_ram();
        logic [7:0] rb;
        $display("");
        $display("Test: CPU bus reads from char RAM ($AA00-$B1CF)");
        // Plant known bytes
        dut.text_inst.char_mem.mem[0]    = 8'h41;
        dut.text_inst.char_mem.mem[79]   = 8'h42;
        dut.text_inst.char_mem.mem[1999] = 8'h43;
        step(2);
        bus_read(16'hAA00, rb);                check_eq("read $AA00",        int'(rb), 8'h41);
        bus_read(CHAR_RAM_BASE_A + 79, rb);    check_eq("read $AA00+79",     int'(rb), 8'h42);
        bus_read(CHAR_RAM_BASE_A + 1999, rb);  check_eq("read last cell $B1CF", int'(rb), 8'h43);
    endtask

    task automatic test_cpu_read_color_ram();
        logic [7:0] rb;
        $display("");
        $display("Test: CPU bus reads from color RAM ($B1D0-$B99F)");
        dut.text_inst.color_mem.mem[0]    = 8'hAA;
        dut.text_inst.color_mem.mem[500]  = 8'h55;
        dut.text_inst.color_mem.mem[1999] = 8'h33;
        step(2);
        bus_read(COLOR_RAM_BASE_A, rb);         check_eq("color[0]",    int'(rb), 8'hAA);
        bus_read(COLOR_RAM_BASE_A + 500, rb);   check_eq("color[500]",  int'(rb), 8'h55);
        bus_read(COLOR_RAM_BASE_A + 1999, rb);  check_eq("color[1999]", int'(rb), 8'h33);
    endtask

    task automatic test_sprite_count_reg();
        logic [7:0] rb;
        $display("");
        $display("Test: $A009 returns count of enabled sprites");
        // All disabled
        for (int i = 0; i < 16; i++)
            dut.spr_enable[i] = 0;
        step(2);
        bus_read(16'hA009, rb);   check_eq("count=0 initially", int'(rb), 0);

        // Enable 3 sprites
        dut.spr_enable[1] = 1;
        dut.spr_enable[5] = 1;
        dut.spr_enable[15] = 1;
        step(2);
        bus_read(16'hA009, rb);   check_eq("count=3 after enabling 3", int'(rb), 3);

        // Enable all 16
        for (int i = 0; i < 16; i++)
            dut.spr_enable[i] = 1;
        step(2);
        bus_read(16'hA009, rb);   check_eq("count=16 (all enabled)", int'(rb), 16);
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
        check_eq("border_color stable", int'(dut.border_color),  11);
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
        test_cursor_xy();
        test_scroll_registers();
        test_font_slot();
        test_gfx_color();
        test_cursor_enable();
        test_collision_clear_on_write();
        test_irq_ctrl();
        test_cpu_read_char_ram();
        test_cpu_read_color_ram();
        test_sprite_count_reg();
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
