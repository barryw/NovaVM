// Top-level custom-chip reset regression.
//
// Verifies dbg_system_reset reaches custom devices, not just the CPU. This
// guards against regressions where reset returns BASIC to a prompt while VGC/FIO
// state still contains whatever the last graphics test or splash screen left.

`timescale 1ns/1ps

module test_custom_reset_top;

    logic clk = 0;
    always #20 clk = ~clk;  // ~25 MHz

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

    logic        dbg_vmem_we;
    logic        dbg_vmem_re;
    logic [2:0]  dbg_vmem_space;
    logic [16:0] dbg_vmem_addr;
    logic [7:0]  dbg_vmem_data;
    wire  [7:0]  dbg_vmem_rdata;

    logic        dbg_rom_we = 0;
    logic        dbg_rom_idx = 0;
    logic [13:0] dbg_rom_addr = 0;
    logic [7:0]  dbg_rom_data = 0;
    logic        dbg_cpu_reset;
    logic        dbg_system_reset;
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
        .key_valid(key_valid), .key_data(key_data),
        .irq_n(irq_n), .nmi_n(nmi_n),
        .vid_r(vid_r), .vid_g(vid_g), .vid_b(vid_b),
        .vid_hsync(vid_hsync), .vid_vsync(vid_vsync), .vid_de(vid_de),
        .audio_l(audio_l), .audio_r(audio_r),
        .dbg_peek_en(dbg_peek_en), .dbg_peek_addr(dbg_peek_addr),
        .dbg_peek_data(dbg_peek_data),
        .dbg_poke_en(dbg_poke_en), .dbg_poke_addr(dbg_poke_addr),
        .dbg_poke_data(dbg_poke_data),
        .dbg_pause(dbg_pause),
        .dbg_vmem_we(dbg_vmem_we), .dbg_vmem_re(dbg_vmem_re),
        .dbg_vmem_space(dbg_vmem_space),
        .dbg_vmem_addr(dbg_vmem_addr), .dbg_vmem_data(dbg_vmem_data),
        .dbg_vmem_rdata(dbg_vmem_rdata),
        .dbg_rom_we(dbg_rom_we), .dbg_rom_idx(dbg_rom_idx),
        .dbg_rom_addr(dbg_rom_addr), .dbg_rom_data(dbg_rom_data),
        .dbg_cpu_reset(dbg_cpu_reset),
        .dbg_system_reset(dbg_system_reset),
        .dbg_cpu_resume(dbg_cpu_resume),
        .brg_sdram_b_we(1'b0), .brg_sdram_b_oe(1'b0), .brg_sdram_b_addr(25'd0),
        .brg_sdram_b_din(8'd0),
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
        .dbg_cpu_stopped(dbg_cpu_stopped)
    );

    localparam SPACE_CHAR   = 3'd1;
    localparam SPACE_COLOR  = 3'd2;
    localparam SPACE_GFX    = 3'd3;
    localparam SPACE_SPRITE = 3'd4;
    localparam SPACE_TILE   = 3'd6;

    int pass_count = 0;
    int fail_count = 0;
    int test_num   = 0;

    task automatic check_eq8(input string name, input logic [7:0] actual, input logic [7:0] expected);
        test_num++;
        if (actual === expected) begin
            $display("  PASS [%0d] %s (=0x%02X)", test_num, name, actual);
            pass_count++;
        end else begin
            $display("  FAIL [%0d] %s (got 0x%02X, want 0x%02X)", test_num, name, actual, expected);
            fail_count++;
        end
    endtask

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

    task automatic rom_write(input logic idx, input logic [13:0] addr, input logic [7:0] data);
        @(posedge clk);
        dbg_rom_idx  <= idx;
        dbg_rom_addr <= addr;
        dbg_rom_data <= data;
        dbg_rom_we   <= 1;
        @(posedge clk);
        dbg_rom_we   <= 0;
    endtask

    task automatic dbg_poke(input logic [15:0] addr, input logic [7:0] data);
        @(posedge clk);
        dbg_poke_addr <= addr;
        dbg_poke_data <= data;
        dbg_poke_en   <= 1;
        @(posedge clk);
        dbg_poke_en   <= 0;
        repeat(4) @(posedge clk);
    endtask

    task automatic dbg_peek(input logic [15:0] addr, output logic [7:0] data);
        @(posedge clk);
        dbg_peek_addr <= addr;
        dbg_peek_en   <= 1;
        repeat(4) @(posedge clk);
        data = dbg_peek_data;
        @(posedge clk);
        dbg_peek_en   <= 0;
    endtask

    task automatic dbg_vmem_write(input logic [2:0] space, input logic [16:0] addr, input logic [7:0] data);
        @(posedge clk);
        dbg_vmem_space <= space;
        dbg_vmem_addr  <= addr;
        dbg_vmem_data  <= data;
        dbg_vmem_we    <= 1;
        @(posedge clk);
        dbg_vmem_we    <= 0;
        repeat(4) @(posedge clk);
    endtask

    task automatic wait_vgc_ready();
        while (dut.vgc_rdy !== 1'b1) @(posedge clk);
        repeat(4) @(posedge clk);
    endtask

    localparam int PROG_LEN = 68;
    byte unsigned prog [PROG_LEN] = '{
        8'hA9, 8'h05, 8'h8D, 8'h01, 8'hA0,  // STA $A001 VGC bg
        8'hA9, 8'h03, 8'h8D, 8'h02, 8'hA0,  // STA $A002 VGC fg
        8'hA9, 8'h42, 8'h8D, 8'hA0, 8'hB9,  // STA $B9A0 FIO cmd
        8'hA9, 8'h56, 8'h8D, 8'h00, 8'hBA,  // STA $BA00 XMC reg 0
        8'hA9, 8'h05, 8'h8D, 8'h66, 8'hBA,  // STA $BA66 DMA srcspace
        8'hA9, 8'h06, 8'h8D, 8'h86, 8'hBA,  // STA $BA86 blitter srcspace
        8'hA9, 8'h07, 8'h8D, 8'hC0, 8'hA0,  // STA $A0C0 tile config
        8'hA9, 8'h09, 8'h8D, 8'hC1, 8'hA0,  // STA $A0C1 tile transparent color
        8'hA9, 8'h1B, 8'h8D, 8'h00, 8'hD4,  // STA $D400 SID1 freq lo
        8'hA9, 8'h2C, 8'h8D, 8'h01, 8'hD4,  // STA $D401 SID1 freq hi
        8'hA9, 8'h07, 8'h8D, 8'h1D, 8'hD4,  // STA $D41D SID1 voice volume
        8'hA9, 8'h3D, 8'h8D, 8'h20, 8'hD4,  // STA $D420 SID2 freq lo
        8'hA9, 8'h03, 8'h8D, 8'h40, 8'hD4,  // STA $D440 SID config
        8'h4C, 8'h41, 8'hC0                   // JMP $C041
    };

    localparam int SYSRESET_PROG_LEN = 31;
    byte unsigned sysreset_prog [SYSRESET_PROG_LEN] = '{
        8'hAD, 8'h00, 8'h04,        // $C000: LDA $0400
        8'hD0, 8'h12,              //        BNE $C017
        8'hA9, 8'h01,              //        LDA #$01
        8'h8D, 8'h00, 8'h04,        //        STA $0400
        8'hA9, 8'h1F,              //        LDA #VCMD_SYSRESET
        8'h8D, 8'h10, 8'hA0,        //        STA $A010
        8'hA9, 8'hEE,              //        LDA #$EE
        8'h8D, 8'h01, 8'h04,        //        STA $0401 (must not commit)
        8'h4C, 8'h14, 8'hC0,        //        JMP $C014
        8'hA9, 8'hAA,              // $C017: LDA #$AA
        8'h8D, 8'h02, 8'h04,        //        STA $0402
        8'h4C, 8'h1C, 8'hC0         //        JMP $C01C
    };

    initial begin
        logic [7:0] d;

        $display("=== Top-level custom reset regression test ===");

        rst              = 1;
        dbg_pause        = 1;
        dbg_cpu_reset    = 1;
        dbg_system_reset = 0;
        dbg_peek_en      = 0;
        dbg_peek_addr    = 0;
        dbg_poke_en      = 0;
        dbg_poke_addr    = 0;
        dbg_poke_data    = 0;
        dbg_vmem_we      = 0;
        dbg_vmem_re      = 0;
        dbg_vmem_space   = 0;
        dbg_vmem_addr    = 0;
        dbg_vmem_data    = 0;

        repeat(20) @(posedge clk);
        rst = 0;
        wait_vgc_ready();
        while (dut.vgc_inst.reset_display_active !== 1'b0) @(posedge clk);

        // A custom/system reset must not restart the HDMI scan counters or
        // blank output immediately. VGC applies soft reset at frame_start so
        // sinks never see a split frame.
        while (!(dut.vgc_inst.v_count == 10'd100 &&
                 dut.vgc_inst.h_count == 10'd100)) @(posedge clk);
        dbg_system_reset = 1;
        @(posedge clk);
        check("system reset leaves VGC timing running mid-frame",
              !(dut.vgc_inst.h_count == 10'd0 &&
                dut.vgc_inst.v_count == 10'd0));
        check("system reset does not blank mid-frame",
              dut.vgc_inst.reset_display_active == 1'b0);
        dbg_system_reset = 0;
        wait_vgc_ready();
        while (dut.vgc_inst.reset_display_active !== 1'b0) @(posedge clk);

        // Debug writes to owned MMIO ranges must not also write the main-RAM
        // mirror. This is the regression that made reset appear broken:
        // device state cleared, but debug peeks fell through to stale RAM.
        dbg_poke(16'hA001, 8'h77);
        dbg_poke(16'hB9A0, 8'h88);
        check_eq8("debug VGC poke does not shadow-write RAM",
                  dut.main_ram.mem[16'hA001], 8'h00);
        check_eq8("debug FIO poke does not shadow-write RAM",
                  dut.main_ram.mem[16'hB9A0], 8'h00);

        for (int i = 0; i < PROG_LEN; i++)
            rom_write(1'b0, 14'(i), prog[i]);
        rom_write(1'b0, 14'h3FFC, 8'h00);
        rom_write(1'b0, 14'h3FFD, 8'hC0);

        dbg_cpu_reset = 0;
        repeat(4) @(posedge clk);
        dbg_pause = 0;
        repeat(20000) @(posedge clk);
        dbg_pause = 1;
        repeat(20) @(posedge clk);

        dbg_peek(16'hB9A0, d);
        check_eq8("CPU mutated FIO before reset", d, 8'h42);
        dbg_peek(16'hA001, d);
        check_eq8("CPU mutated VGC bg before reset", d, 8'h05);
        check_eq8("CPU mutated XMC before reset", dut.xmc_regs[6'h00], 8'h56);
        check_eq8("CPU mutated DMA before reset", dut.dma_inst.regs[3], 8'h05);
        check_eq8("CPU mutated blitter before reset", dut.blt_inst.regs[3], 8'h06);
        check("CPU mutated tile config before reset",
              dut.vgc_inst.tile_inst.tile_size16 == 1 && dut.vgc_inst.tile_inst.mirror_mode == 2'd3);
        check_eq8("CPU mutated tile trans before reset",
                  {4'h0, dut.vgc_inst.tile_inst.trans_color}, 8'h09);
        check_eq8("CPU mutated SID1 before reset",
                  dut.sid1_inst.voice_freq[0][7:0], 8'h1B);
        check_eq8("CPU mutated SID2 before reset",
                  dut.sid2_inst.voice_freq[0][7:0], 8'h3D);
        check_eq8("CPU mutated SID config before reset", dut.sid_cfg_reg, 8'h03);

        check_eq8("CPU VGC write does not shadow-write RAM",
                  dut.main_ram.mem[16'hA001], 8'h00);
        check_eq8("CPU FIO write does not shadow-write RAM",
                  dut.main_ram.mem[16'hB9A0], 8'h00);
        check_eq8("CPU XMC write does not shadow-write RAM",
                  dut.main_ram.mem[16'hBA00], 8'h00);
        check_eq8("CPU DMA write does not shadow-write RAM",
                  dut.main_ram.mem[16'hBA66], 8'h00);
        check_eq8("CPU blitter write does not shadow-write RAM",
                  dut.main_ram.mem[16'hBA86], 8'h00);

        dbg_poke(16'hB9A0, 8'h42);
        dbg_poke(16'hA001, 8'h05);
        dbg_poke(16'hA002, 8'h03);
        dbg_vmem_write(SPACE_CHAR,   17'd0,    8'h58);
        dbg_vmem_write(SPACE_COLOR,  17'd0,    8'h04);
        dbg_vmem_write(SPACE_GFX,    17'd1234, 8'h0F);
        dbg_vmem_write(SPACE_SPRITE, 17'd0,    8'hFF);
        dbg_vmem_write(SPACE_TILE,   17'd100,  8'hAB);

        dbg_peek(16'hB9A0, d);
        check_eq8("FIO mutated before system reset", d, 8'h42);
        dbg_peek(16'hA001, d);
        check_eq8("VGC bg mutated before system reset", d, 8'h05);
        check_eq8("char RAM mutated before system reset",
                  dut.vgc_inst.text_inst.char_mem.mem[0], 8'h58);
        check_eq8("gfx RAM mutated before system reset",
                  {4'h0, dut.vgc_inst.gfx_inst.gfx_mem.mem[1234]}, 8'h0F);
        check_eq8("tile RAM mutated before system reset",
                  dut.vgc_inst.tile_inst.tile_data_ram.mem[100], 8'hAB);

        dbg_system_reset = 1;
        repeat(8) @(posedge clk);
        dbg_system_reset = 0;
        wait_vgc_ready();

        dbg_peek(16'hB9A0, d);
        check_eq8("FIO command reset to zero", d, 8'h00);
        dbg_peek(16'hA001, d);
        check_eq8("VGC bg reset to black", d, 8'h00);
        dbg_peek(16'hA002, d);
        check_eq8("VGC fg reset to white", d, 8'h0F);
        check_eq8("XMC reg 0 reset to zero", dut.xmc_regs[6'h00], 8'h00);
        check_eq8("XMC banks reset to 8", dut.xmc_regs[6'h0D], 8'd8);
        check_eq8("XMC window control reset enabled", dut.xmc_regs[6'h16], 8'h0F);
        check_eq8("DMA srcspace reset", dut.dma_inst.regs[3], 8'h00);
        check_eq8("blitter srcspace reset", dut.blt_inst.regs[3], 8'h00);
        check_eq8("SID config reset", dut.sid_cfg_reg, 8'h00);
        check_eq8("SID1 freq reset", dut.sid1_inst.voice_freq[0][7:0], 8'h00);
        check_eq8("SID1 voice volume default", {4'h0, dut.sid1_inst.voice_vol[0]}, 8'h0F);
        check_eq8("SID2 freq reset", dut.sid2_inst.voice_freq[0][7:0], 8'h00);
        check("tile config reset",
              dut.vgc_inst.tile_inst.tile_size16 == 0 && dut.vgc_inst.tile_inst.mirror_mode == 0);
        check_eq8("tile trans reset", {4'h0, dut.vgc_inst.tile_inst.trans_color}, 8'h00);
        check_eq8("char RAM reset to space",
                  dut.vgc_inst.text_inst.char_mem.mem[0], 8'h20);
        check_eq8("color RAM reset to default fg",
                  dut.vgc_inst.text_inst.color_mem.mem[0], 8'h0F);
        check_eq8("gfx RAM reset to transparent/zero",
                  {4'h0, dut.vgc_inst.gfx_inst.gfx_mem.mem[1234]}, 8'h00);
        check_eq8("sprite shape RAM bank 0 reset",
                  dut.vgc_inst.sprite_inst.spr_mem0.mem[0], 8'h00);
        check_eq8("sprite shape RAM bank 1 reset",
                  dut.vgc_inst.sprite_inst.spr_mem1.mem[0], 8'h00);
        check_eq8("tile RAM reset",
                  dut.vgc_inst.tile_inst.tile_data_ram.mem[100], 8'h00);

        // CPU-issued VGC SYSRESET must reset the CPU too. This catches the
        // BASIC `RESET` regression where the custom chips reset underneath
        // the interpreter and execution fell through into a syntax error.
        dbg_pause = 1;
        dbg_cpu_reset = 1;
        repeat(8) @(posedge clk);
        for (int i = 0; i < SYSRESET_PROG_LEN; i++)
            rom_write(1'b0, 14'(i), sysreset_prog[i]);
        rom_write(1'b0, 14'h3FFC, 8'h00);
        rom_write(1'b0, 14'h3FFD, 8'hC0);
        dbg_poke(16'h0400, 8'h00);
        dbg_poke(16'h0401, 8'h00);
        dbg_poke(16'h0402, 8'h00);

        dbg_cpu_reset = 0;
        repeat(4) @(posedge clk);
        dbg_pause = 0;
        begin
            int reset_wait;
            int restart_wait;
            reset_wait = 0;
            while (dut.vgc_sys_reset_active !== 1'b1 && reset_wait < 20000) begin
                reset_wait++;
                @(posedge clk);
            end
            check("SYSRESET command asserted machine reset",
                  dut.vgc_sys_reset_active === 1'b1);
            while (dut.vgc_sys_reset_active === 1'b1) @(posedge clk);
            wait_vgc_ready();
            restart_wait = 0;
            while (dut.main_ram.mem[16'h0402] !== 8'hAA && restart_wait < 20000) begin
                restart_wait++;
                @(posedge clk);
            end
        end
        dbg_pause = 1;
        repeat(20) @(posedge clk);

        dbg_peek(16'h0400, d);
        check_eq8("SYSRESET program set first-run flag", d, 8'h01);
        dbg_peek(16'h0401, d);
        check_eq8("SYSRESET prevented post-command instruction", d, 8'h00);
        dbg_peek(16'h0402, d);
        check_eq8("SYSRESET restarted CPU at reset vector", d, 8'hAA);

        $display("");
        $display("=== Results: %0d passed, %0d failed ===", pass_count, fail_count);
        if (fail_count == 0) $display("ALL TESTS PASSED");
        else                 $display("SOME TESTS FAILED");
        $finish;
    end

    initial begin
        #200000000;
        $display("FAIL: global timeout hit");
        $finish;
    end

endmodule
