// NovaLogo DRAW top-level regression.
//
// Loads the real NovaLogo ROM pair through the synthesized top.sv debug-ROM
// path, boots to the prompt, types DRAW, and verifies the NDK virtual-sprite
// turtle remains centered with no large graphics-plane corruption. This is the
// simulation guard for the hardware symptom where DRAW stayed mapped to the
// extension ROM after writing ROMSWAP_LOGO and then executed garbage.

`timescale 1ns/1ps

module test_novalogo_draw_top;

    logic clk = 0;
    always #20 clk = ~clk;

    logic rst;
    logic        key_valid = 0;
    logic [7:0]  key_data  = 0;
    logic        irq_n = 1, nmi_n = 1;

    wire  [3:0]  vid_r, vid_g, vid_b;
    wire         vid_hsync, vid_vsync, vid_de;
    wire signed [17:0] audio_l, audio_r;

    logic        dbg_peek_en;
    logic [15:0] dbg_peek_addr;
    wire  [7:0]  dbg_peek_data;
    logic        dbg_poke_en;
    logic [15:0] dbg_poke_addr;
    logic [7:0]  dbg_poke_data;
    logic        dbg_pause;

    logic        dbg_rom_we;
    logic        dbg_rom_idx;
    logic [13:0] dbg_rom_addr;
    logic [7:0]  dbg_rom_data;
    logic        dbg_cpu_reset;
    logic        dbg_cpu_resume = 1'b0;

    wire  [15:0] dbg_cpu_pc;
    wire  [7:0]  dbg_cpu_a, dbg_cpu_x, dbg_cpu_y, dbg_cpu_sp, dbg_cpu_flags;
    wire  [5:0]  dbg_cpu_state;
    wire  [7:0]  dbg_cpu_ir;
    wire  [15:0] dbg_cpu_addr;
    wire  [7:0]  dbg_cpu_din, dbg_cpu_dout;
    wire         dbg_cpu_we, dbg_cpu_rdy, dbg_cpu_irq, dbg_cpu_nmi;
    wire         dbg_cpu_waiting, dbg_cpu_stopped;

    top dut (
        .clk(clk), .rst(rst),
        .key_valid(key_valid), .key_data(key_data), .key_ready(),
        .board_buttons(8'h00),
        .board_switches(8'h00),
        .usb_hid_status(8'h00),
        .usb_hid_device_type(8'h00),
        .usb_hid_last_scan(8'h00),
        .usb_hid_last_ascii(8'h00),
        .usb_hid_report_count(8'h00),
        .usb_hid_key_count(8'h00),
        .usb_hid_core_status(8'h00),
        .usb_hid_regs(64'h0),
        .irq_n(irq_n), .nmi_n(nmi_n),
        .vid_r(vid_r), .vid_g(vid_g), .vid_b(vid_b),
        .vid_hsync(vid_hsync), .vid_vsync(vid_vsync), .vid_de(vid_de),
        .audio_l(audio_l), .audio_r(audio_r),
        .dbg_peek_en(dbg_peek_en), .dbg_peek_addr(dbg_peek_addr),
        .dbg_peek_data(dbg_peek_data),
        .dbg_poke_en(dbg_poke_en), .dbg_poke_addr(dbg_poke_addr),
        .dbg_poke_data(dbg_poke_data),
        .dbg_pause(dbg_pause),
        .dbg_nic_buf_we(1'b0), .dbg_nic_buf_re(1'b0), .dbg_nic_buf_sel(1'b0),
        .dbg_nic_buf_addr(8'd0), .dbg_nic_buf_data(8'd0), .dbg_nic_buf_rdata(),
        .dbg_vmem_we(1'b0), .dbg_vmem_re(1'b0), .dbg_vmem_space(3'd0),
        .dbg_vmem_addr(17'd0), .dbg_vmem_data(8'd0), .dbg_vmem_rdata(),
        .dbg_rom_we(dbg_rom_we), .dbg_rom_idx(dbg_rom_idx),
        .dbg_rom_addr(dbg_rom_addr), .dbg_rom_data(dbg_rom_data),
        .dbg_cpu_reset(dbg_cpu_reset),
        .dbg_system_reset(1'b0),
        .dbg_cpu_resume(dbg_cpu_resume),
        .brg_sdram_b_we(1'b0), .brg_sdram_b_oe(1'b0),
        .brg_sdram_b_addr(25'd0), .brg_sdram_b_din(8'd0),
        .host_wts_event_we(1'b0), .host_wts_event_data(8'd0),
        .host_wts_event_ready(),
        .dbg_cpu_pc(dbg_cpu_pc),
        .dbg_cpu_a(dbg_cpu_a), .dbg_cpu_x(dbg_cpu_x),
        .dbg_cpu_y(dbg_cpu_y), .dbg_cpu_sp(dbg_cpu_sp),
        .dbg_cpu_flags(dbg_cpu_flags),
        .dbg_cpu_state(dbg_cpu_state),
        .dbg_cpu_ir(dbg_cpu_ir),
        .dbg_cpu_addr(dbg_cpu_addr),
        .dbg_cpu_din(dbg_cpu_din),
        .dbg_cpu_dout(dbg_cpu_dout),
        .dbg_cpu_we(dbg_cpu_we),
        .dbg_cpu_rdy(dbg_cpu_rdy),
        .dbg_cpu_irq(dbg_cpu_irq),
        .dbg_cpu_nmi(dbg_cpu_nmi),
        .dbg_cpu_waiting(dbg_cpu_waiting),
        .dbg_cpu_stopped(dbg_cpu_stopped),
        .sdram_clk(clk),
        .sdram_addrA(), .sdram_dinA(), .sdram_weA(), .sdram_oeA(),
        .sdram_doutA(8'h00), .sdram_doneA(1'b0),
        .sdram_addrB(), .sdram_dinB(), .sdram_weB(), .sdram_oeB(),
        .sdram_doutB(8'h00),
        .fio_event(),
        .nic_event()
    );

    localparam int ROM_SIZE = 16384;
    localparam int COLS = 80;
    localparam int ROWS = 45;
    localparam int GFX_WIDTH = 320;
    localparam int SPLIT_GFX_ROWS = 160;
    localparam int TURTLE_STATE = 16'h9F00;
    localparam int GLOBAL_TIMEOUT_CYCLES = 80000000;

    byte unsigned logo_rom [0:ROM_SIZE-1];
    byte unsigned logo_ext_rom [0:ROM_SIZE-1];

    int pass_count = 0;
    int fail_count = 0;
    int test_num = 0;

    task automatic check(input string name, input logic condition);
        test_num++;
        if (condition) begin
            $display("  PASS [%0d] %s", test_num, name);
            pass_count++;
        end else begin
            $display("  FAIL [%0d] %s", test_num, name);
            fail_count++;
        end
    endtask

    task automatic check_eq8(input string name, input logic [7:0] actual,
                             input logic [7:0] expected);
        test_num++;
        if (actual === expected) begin
            $display("  PASS [%0d] %s (=0x%02X)", test_num, name, actual);
            pass_count++;
        end else begin
            $display("  FAIL [%0d] %s (got 0x%02X, want 0x%02X)",
                     test_num, name, actual, expected);
            fail_count++;
        end
    endtask

    task automatic rom_write(input logic idx, input logic [13:0] addr,
                             input logic [7:0] data);
        @(posedge clk);
        dbg_rom_idx  <= idx;
        dbg_rom_addr <= addr;
        dbg_rom_data <= data;
        dbg_rom_we   <= 1;
        @(posedge clk);
        dbg_rom_we   <= 0;
    endtask

    task automatic load_rom_file(input string path, ref byte unsigned dest [0:ROM_SIZE-1]);
        int fd;
        int count;
        for (int i = 0; i < ROM_SIZE; i++)
            dest[i] = 8'h00;
        fd = $fopen(path, "rb");
        if (fd == 0) begin
            $display("FAIL: could not open %s", path);
            $finish(1);
        end
        count = $fread(dest, fd);
        $fclose(fd);
        if (count <= 0) begin
            $display("FAIL: could not read %s", path);
            $finish(1);
        end
        $display("Loaded %0d bytes from %s", count, path);
    endtask

    task automatic load_rom_bank(input logic idx, ref byte unsigned data [0:ROM_SIZE-1]);
        for (int i = 0; i < ROM_SIZE; i++)
            rom_write(idx, 14'(i), data[i]);
    endtask

    task automatic wait_vgc_ready();
        while (dut.vgc_rdy !== 1'b1) @(posedge clk);
        repeat(4) @(posedge clk);
    endtask

    task automatic inject_key(input logic [7:0] ch);
        @(posedge clk);
        key_data  <= ch;
        key_valid <= 1;
        @(posedge clk);
        key_valid <= 0;
        repeat(5000) @(posedge clk);
    endtask

    task automatic type_draw();
        type_line("DRAW");
    endtask

    task automatic type_line(input string line);
        for (int i = 0; i < line.len(); i++)
            inject_key(line.getc(i));
        inject_key(8'h0D);
        wait_for_command_prompt(line, 12000000);
    endtask

    function automatic logic [7:0] text_at(input int row, input int col);
        text_at = dut.vgc_inst.text_inst.char_mem.mem[row * COLS + col];
    endfunction

    function automatic bit screen_contains_prompt();
        screen_contains_prompt = 0;
        for (int row = 0; row < ROWS; row++) begin
            for (int col = 0; col < COLS - 1; col++) begin
                if (text_at(row, col) == "?" && text_at(row, col + 1) == " ")
                    screen_contains_prompt = 1;
            end
        end
    endfunction

    function automatic int prompt_count();
        int count;
        count = 0;
        for (int row = 0; row < ROWS; row++) begin
            for (int col = 0; col < COLS - 1; col++) begin
                if (text_at(row, col) == "?" && text_at(row, col + 1) == " ")
                    count++;
            end
        end
        prompt_count = count;
    endfunction

    function automatic bit screen_contains_text(input string needle);
        screen_contains_text = 0;
        if (needle.len() == 0)
            return 1;
        for (int row = 0; row < ROWS; row++) begin
            for (int col = 0; col <= COLS - needle.len(); col++) begin
                bit matched;
                matched = 1;
                for (int i = 0; i < needle.len(); i++) begin
                    if (text_at(row, col + i) != needle.getc(i))
                        matched = 0;
                end
                if (matched)
                    screen_contains_text = 1;
            end
        end
    endfunction

    task automatic type_proc_definition(input string name, input string body);
        string header;
        header = {"TO ", name};
        for (int i = 0; i < header.len(); i++)
            inject_key(header.getc(i));
        inject_key(8'h0D);
        repeat(1500000) @(posedge clk);
        for (int i = 0; i < body.len(); i++)
            inject_key(body.getc(i));
        inject_key(8'h0D);
        inject_key(8'h13);       // Ctrl-S saves
        repeat(1000000) @(posedge clk);
        inject_key(8'h11);       // Ctrl-Q exits
        wait_for_command_prompt("TO editor", 12000000);
    endtask

    task automatic wait_for_prompt(input int max_cycles);
        int cycles;
        cycles = 0;
        while (!screen_contains_prompt() && cycles < max_cycles) begin
            @(posedge clk);
            cycles++;
        end
        check("NovaLogo prompt appeared", screen_contains_prompt());
        $display("Prompt wait cycles: %0d", cycles);
    endtask

    task automatic wait_for_prompt_count(input int minimum, input int max_cycles,
                                         input string label);
        int cycles;
        cycles = 0;
        while (prompt_count() < minimum && cycles < max_cycles) begin
            @(posedge clk);
            cycles++;
        end
        check({"fresh prompt after ", label}, prompt_count() >= minimum);
    endtask

    task automatic wait_for_command_prompt(input string label, input int max_cycles);
        int cycles;
        bit saw_cursor_off;
        cycles = 0;
        saw_cursor_off = 0;
        while (cycles < max_cycles) begin
            @(posedge clk);
            cycles++;
            if (dut.vgc_inst.cursor_enable == 1'b0)
                saw_cursor_off = 1;
            if (saw_cursor_off && dut.vgc_inst.cursor_enable == 1'b1)
                break;
        end
        check({"command returned to prompt after ", label},
              saw_cursor_off && dut.vgc_inst.cursor_enable == 1'b1);
    endtask

    task automatic wait_for_draw_idle(input int max_cycles);
        int cycles;
        cycles = 0;
        while (dut.blt_inst.regs[1] == 8'h01 && cycles < max_cycles) begin
            @(posedge clk);
            cycles++;
        end
        repeat(1500000) @(posedge clk);
    endtask

    function automatic int count_gfx_nonzero(input int x0, input int y0,
                                             input int width, input int height);
        int total;
        int addr;
        total = 0;
        for (int y = y0; y < y0 + height; y++) begin
            for (int x = x0; x < x0 + width; x++) begin
                addr = y * GFX_WIDTH + x;
                if (dut.vgc_inst.gfx_inst.gfx_mem.mem[addr] != 0)
                    total++;
            end
        end
        count_gfx_nonzero = total;
    endfunction

    initial begin
        int center_pixels;
        int total_pixels;
        int turn_x;
        int turn_y;
        int turn_heading;
        int diagonal_pixels;
        int old_center_pixels;
        int square_top_pixels;
        int square_right_pixels;
        int editor_mode_before;
        int editor_mode_after;
        int editor_pixels_before;
        int editor_pixels_after;

        $display("=== NovaLogo DRAW top-level regression ===");

        rst           = 1;
        dbg_pause     = 1;
        dbg_cpu_reset = 1;
        key_valid     = 0;
        key_data      = 0;
        dbg_peek_en   = 0;
        dbg_peek_addr = 0;
        dbg_poke_en   = 0;
        dbg_poke_addr = 0;
        dbg_poke_data = 0;
        dbg_rom_we    = 0;
        dbg_rom_idx   = 0;
        dbg_rom_addr  = 0;
        dbg_rom_data  = 0;

        load_rom_file("../novalogo/novalogo.bin", logo_rom);
        load_rom_file("../novalogo/novalogo_ext.bin", logo_ext_rom);

        repeat(20) @(posedge clk);
        rst = 0;
        repeat(10) @(posedge clk);

        load_rom_bank(1'b0, logo_rom);
        load_rom_bank(1'b1, logo_ext_rom);

        repeat(4) @(posedge clk);
        wait_vgc_ready();

        dbg_cpu_reset = 0;
        repeat(4) @(posedge clk);
        dbg_pause = 0;

        wait_for_prompt(8000000);
        type_draw();
        wait_for_draw_idle(8000000);

        $display("CPU after DRAW: PC=0x%04X IR=0x%02X state=%0d addr=0x%04X din=0x%02X A=0x%02X SP=0x%02X flags=0x%02X",
                 dbg_cpu_pc, dbg_cpu_ir, dbg_cpu_state, dbg_cpu_addr,
                 dbg_cpu_din, dbg_cpu_a, dbg_cpu_sp, dbg_cpu_flags);

        check_eq8("turtle x frac", dut.main_ram.mem[TURTLE_STATE + 0], 8'h00);
        check_eq8("turtle x lo", dut.main_ram.mem[TURTLE_STATE + 1], 8'd160);
        check_eq8("turtle x hi", dut.main_ram.mem[TURTLE_STATE + 2], 8'h00);
        check_eq8("turtle y frac", dut.main_ram.mem[TURTLE_STATE + 3], 8'h00);
        check_eq8("turtle y lo", dut.main_ram.mem[TURTLE_STATE + 4], 8'd80);
        check_eq8("turtle y hi", dut.main_ram.mem[TURTLE_STATE + 5], 8'h00);
        check_eq8("turtle heading lo", dut.main_ram.mem[TURTLE_STATE + 6], 8'h00);
        check_eq8("turtle heading hi", dut.main_ram.mem[TURTLE_STATE + 7], 8'h00);
        check_eq8("turtle pen is up after DRAW", dut.main_ram.mem[TURTLE_STATE + 8], 8'h00);
        check_eq8("turtle shown", dut.main_ram.mem[TURTLE_STATE + 9], 8'h01);
        check_eq8("turtle color", dut.main_ram.mem[TURTLE_STATE + 10], 8'h01);
        check_eq8("turtle initialized", dut.main_ram.mem[TURTLE_STATE + 12], 8'h01);
        check("DRAW returned to the primary LOGO ROM", dut.ext_rom_active == 1'b0);

        center_pixels = count_gfx_nonzero(152, 72, 16, 16);
        total_pixels = count_gfx_nonzero(0, 0, GFX_WIDTH, SPLIT_GFX_ROWS);
        $display("DRAW graphics pixels: center=%0d total=%0d", center_pixels, total_pixels);
        check("small centered turtle was drawn", center_pixels >= 20 && center_pixels <= 80);
        check("graphics plane contains only the turtle", total_pixels >= 20 && total_pixels <= 140);
        check("hardware sprite slot 15 is unused",
              dut.vgc_inst.spr_next_enable[15] == 1'b0 && dut.vgc_inst.spr_enable[15] == 1'b0);

        type_line("PU");
        wait_for_draw_idle(8000000);
        type_line("FD 20");
        wait_for_draw_idle(8000000);
        type_line("PD");
        wait_for_draw_idle(8000000);
        type_line("RT 45");
        wait_for_draw_idle(8000000);
        type_line("FD 20");
        wait_for_draw_idle(8000000);

        turn_x = dut.main_ram.mem[TURTLE_STATE + 1] |
                 (dut.main_ram.mem[TURTLE_STATE + 2] << 8);
        turn_y = dut.main_ram.mem[TURTLE_STATE + 4] |
                 (dut.main_ram.mem[TURTLE_STATE + 5] << 8);
        turn_heading = dut.main_ram.mem[TURTLE_STATE + 6] |
                       (dut.main_ram.mem[TURTLE_STATE + 7] << 8);
        diagonal_pixels = count_gfx_nonzero(161, 53, 7, 8);
        old_center_pixels = count_gfx_nonzero(152, 72, 16, 16);
        $display("Turn/forward turtle state: x=%0d y=%0d heading=%0d diagonal=%0d old_center=%0d",
                 turn_x, turn_y, turn_heading, diagonal_pixels, old_center_pixels);
        check("RT 45 leaves heading at 45 degrees", turn_heading == 45);
        check("FD after RT 45 advances east from the current turtle position",
              turn_x >= 172 && turn_x <= 175);
        check("FD after RT 45 advances north on the current heading",
              turn_y >= 44 && turn_y <= 47);
        check("FD after RT 45 draws the diagonal from the current position",
              diagonal_pixels >= 4);
        check("isolated pen-down move does not redraw from original center",
              old_center_pixels == 0);

        type_line("CLEARSCREEN");
        wait_for_draw_idle(8000000);
        check_eq8("CLEARSCREEN alias resets turtle x lo", dut.main_ram.mem[TURTLE_STATE + 1], 8'd160);
        check_eq8("CLEARSCREEN alias resets turtle y lo", dut.main_ram.mem[TURTLE_STATE + 4], 8'd80);
        check_eq8("CLEARSCREEN alias resets heading lo", dut.main_ram.mem[TURTLE_STATE + 6], 8'h00);

        type_line("PENUP");
        wait_for_draw_idle(8000000);
        type_line("FORWARD 20");
        wait_for_draw_idle(8000000);
        type_line("PENDOWN");
        wait_for_draw_idle(8000000);
        type_line("RIGHT 45");
        wait_for_draw_idle(8000000);
        type_line("FORWARD 20");
        wait_for_draw_idle(8000000);

        turn_x = dut.main_ram.mem[TURTLE_STATE + 1] |
                 (dut.main_ram.mem[TURTLE_STATE + 2] << 8);
        turn_y = dut.main_ram.mem[TURTLE_STATE + 4] |
                 (dut.main_ram.mem[TURTLE_STATE + 5] << 8);
        turn_heading = dut.main_ram.mem[TURTLE_STATE + 6] |
                       (dut.main_ram.mem[TURTLE_STATE + 7] << 8);
        diagonal_pixels = count_gfx_nonzero(161, 53, 7, 8);
        old_center_pixels = count_gfx_nonzero(152, 72, 16, 16);
        $display("Long-form turn/forward turtle state: x=%0d y=%0d heading=%0d diagonal=%0d old_center=%0d",
                 turn_x, turn_y, turn_heading, diagonal_pixels, old_center_pixels);
        check("RIGHT 45 leaves heading at 45 degrees", turn_heading == 45);
        check("FORWARD after RIGHT 45 advances east from the current turtle position",
              turn_x >= 172 && turn_x <= 175);
        check("FORWARD after RIGHT 45 advances north on the current heading",
              turn_y >= 44 && turn_y <= 47);
        check("long-form FORWARD after RIGHT 45 draws the diagonal from the current position",
              diagonal_pixels >= 4);
        check("long-form isolated pen-down move does not redraw from original center",
              old_center_pixels == 0);

        type_line("CLEARSCREEN");
        wait_for_draw_idle(8000000);
        type_line("PENUP");
        wait_for_draw_idle(8000000);
        type_line("FD 50");
        wait_for_draw_idle(8000000);
        type_line("RT 90");
        wait_for_draw_idle(8000000);
        type_line("FD 50");
        wait_for_draw_idle(8000000);
        type_line("RT 90");
        wait_for_draw_idle(8000000);
        type_line("FD 50");
        wait_for_draw_idle(8000000);

        turn_x = dut.main_ram.mem[TURTLE_STATE + 1] |
                 (dut.main_ram.mem[TURTLE_STATE + 2] << 8);
        turn_y = dut.main_ram.mem[TURTLE_STATE + 4] |
                 (dut.main_ram.mem[TURTLE_STATE + 5] << 8);
        turn_heading = dut.main_ram.mem[TURTLE_STATE + 6] |
                       (dut.main_ram.mem[TURTLE_STATE + 7] << 8);
        $display("Cardinal relative turn state: x=%0d y=%0d heading=%0d",
                 turn_x, turn_y, turn_heading);
        check("RT 90 is relative and two turns leave heading at 180 degrees",
              turn_heading == 180);
        check("FD at cardinal heading 90 stays on the horizontal row",
              turn_x == 210);
        check("FD after the second relative RT 90 moves south with no x drift",
              turn_y == 80);

        type_line("CLEARSCREEN");
        wait_for_draw_idle(8000000);
        type_line("REPEAT 4 [FD 50 RT 90]");
        wait_for_draw_idle(16000000);
        turn_x = dut.main_ram.mem[TURTLE_STATE + 1] |
                 (dut.main_ram.mem[TURTLE_STATE + 2] << 8);
        turn_y = dut.main_ram.mem[TURTLE_STATE + 4] |
                 (dut.main_ram.mem[TURTLE_STATE + 5] << 8);
        turn_heading = dut.main_ram.mem[TURTLE_STATE + 6] |
                       (dut.main_ram.mem[TURTLE_STATE + 7] << 8);
        square_top_pixels = count_gfx_nonzero(158, 28, 56, 5);
        square_right_pixels = count_gfx_nonzero(208, 28, 5, 56);
        $display("REPEAT square state: x=%0d y=%0d heading=%0d top=%0d right=%0d",
                 turn_x, turn_y, turn_heading, square_top_pixels, square_right_pixels);
        check("REPEAT 4 [FD 50 RT 90] closes back at the starting x",
              turn_x == 160);
        check("REPEAT 4 [FD 50 RT 90] closes back at the starting y",
              turn_y == 80);
        check("REPEAT 4 [FD 50 RT 90] leaves heading restored to 0 degrees",
              turn_heading == 0);
        check("REPEAT 4 [FD 50 RT 90] draws the top edge",
              square_top_pixels >= 30);
        check("REPEAT 4 [FD 50 RT 90] draws the right edge",
              square_right_pixels >= 30);

        type_line("CLEARSCREEN");
        wait_for_draw_idle(8000000);
        type_line("PENUP");
        wait_for_draw_idle(8000000);
        type_line("SETPOS [100 50]");
        wait_for_draw_idle(8000000);
        turn_x = dut.main_ram.mem[TURTLE_STATE + 1] |
                 (dut.main_ram.mem[TURTLE_STATE + 2] << 8);
        turn_y = dut.main_ram.mem[TURTLE_STATE + 4] |
                 (dut.main_ram.mem[TURTLE_STATE + 5] << 8);
        $display("SETPOS turtle state: x=%0d y=%0d", turn_x, turn_y);
        check("SETPOS [100 50] moves turtle to x=100", turn_x == 100);
        check("SETPOS [100 50] moves turtle to y=50", turn_y == 50);

        type_line("CLEARSCREEN");
        wait_for_draw_idle(8000000);
        editor_mode_before = dut.vgc_inst.mode;
        editor_pixels_before = count_gfx_nonzero(152, 72, 16, 16);
        type_proc_definition("VPROC", "PRINT 6060");
        editor_mode_after = dut.vgc_inst.mode;
        editor_pixels_after = count_gfx_nonzero(152, 72, 16, 16);
        check("TO editor restores display mode after Ctrl-S/Ctrl-Q",
              editor_mode_after == editor_mode_before);
        check("TO editor does not clear the graphics turtle",
              editor_pixels_after >= editor_pixels_before);
        type_line("VPROC");
        wait_for_draw_idle(8000000);
        check("TO editor saved procedure runs after exit",
              screen_contains_text("6060"));

        $display("");
        $display("=== Results: %0d passed, %0d failed ===", pass_count, fail_count);
        if (fail_count == 0) $display("ALL TESTS PASSED");
        else                 $display("SOME TESTS FAILED");
        $finish(fail_count);
    end

    initial begin
        repeat (GLOBAL_TIMEOUT_CYCLES) @(posedge clk);
        $display("FAIL: global timeout hit, PC=0x%04X IR=0x%02X state=%0d addr=0x%04X din=0x%02X",
                 dbg_cpu_pc, dbg_cpu_ir, dbg_cpu_state, dbg_cpu_addr, dbg_cpu_din);
        $finish(1);
    end

endmodule
