// tb_top.sv — boot NovaVM SoC (top.sv) in iverilog, trace the 6502 to the
// crash in the BASIC input loop. ROM auto-loads from rom/ehbasic.hex.
// Console captured via cpu_mon writes to $A00E. Crash = PC in garbage RAM.
`timescale 1ns/1ps
module tb_top;
    reg clk = 0, rst = 1;
    reg key_valid = 0; reg [7:0] key_data = 0; wire key_ready;

    wire [15:0] dcpu_pc, dcpu_addr;
    wire [7:0]  dcpu_a, dcpu_x, dcpu_y, dcpu_sp, dcpu_flags, dcpu_ir, dcpu_din, dcpu_dout;
    wire [5:0]  dcpu_state;
    wire        dcpu_we, dcpu_rdy, dcpu_irq, dcpu_nmi, dcpu_waiting, dcpu_stopped;
    wire [15:0] cpu_mon_addr; wire [7:0] cpu_mon_wdata; wire cpu_mon_we;
    wire fio_event, nic_event;
    wire [24:0] sdram_addrA, sdram_addrB, sdram_stream_addr;
    wire [7:0]  sdram_dinA, sdram_dinB; wire sdram_weA, sdram_oeA, sdram_weB, sdram_oeB;
    wire [13:0] sdram_stream_words; wire sdram_stream_req, sdram_stream_ready;
    wire signed [17:0] sid1o, sid2o;
    reg sdram_doneA = 0;
    always @(posedge clk) sdram_doneA <= sdram_oeA | sdram_weA;

    top dut (
        .clk(clk), .rst(rst),
        .key_valid(key_valid), .key_data(key_data), .key_ready(key_ready),
        .board_buttons(8'd0), .board_switches(8'd0),
        .usb_hid_status(8'd0), .usb_hid_device_type(8'd0), .usb_hid_last_scan(8'd0),
        .usb_hid_last_ascii(8'd0), .usb_hid_report_count(8'd0), .usb_hid_key_count(8'd0),
        .usb_hid_core_status(8'd0), .usb_hid_regs(64'd0),
        .irq_n(1'b1), .nmi_n(1'b1),
        .vid_r(), .vid_g(), .vid_b(), .vid_hsync(), .vid_vsync(), .vid_de(),
        .audio_l(), .audio_r(),
        .dbg_peek_en(1'b0), .dbg_peek_addr(16'd0), .dbg_peek_data(),
        .dbg_poke_en(1'b0), .dbg_poke_addr(16'd0), .dbg_poke_data(8'd0),
        .dbg_pause(1'b0),
        .dbg_nic_buf_we(1'b0), .dbg_nic_buf_re(1'b0), .dbg_nic_buf_sel(1'b0),
        .dbg_nic_buf_addr(8'd0), .dbg_nic_buf_data(8'd0), .dbg_nic_buf_rdata(),
        .dbg_vmem_we(1'b0), .dbg_vmem_re(1'b0), .dbg_vmem_space(3'd0),
        .dbg_vmem_addr(17'd0), .dbg_vmem_data(8'd0), .dbg_vmem_rdata(),
        .dbg_rom_we(1'b0), .dbg_rom_idx(1'b0), .dbg_rom_addr(14'd0), .dbg_rom_data(8'd0),
        .dbg_cpu_reset(1'b0), .dbg_system_reset(1'b0), .dbg_cpu_resume(1'b0),
        .brg_sdram_b_we(1'b0), .brg_sdram_b_oe(1'b0), .brg_sdram_b_addr(25'd0), .brg_sdram_b_din(8'd0),
        .host_wts_event_we(1'b0), .host_wts_event_data(8'd0), .host_wts_event_ready(),
        .dbg_cpu_pc(dcpu_pc), .dbg_cpu_a(dcpu_a), .dbg_cpu_x(dcpu_x), .dbg_cpu_y(dcpu_y),
        .dbg_cpu_sp(dcpu_sp), .dbg_cpu_flags(dcpu_flags), .dbg_cpu_state(dcpu_state),
        .dbg_cpu_ir(dcpu_ir), .dbg_cpu_addr(dcpu_addr), .dbg_cpu_din(dcpu_din),
        .dbg_cpu_dout(dcpu_dout), .dbg_cpu_we(dcpu_we), .dbg_cpu_rdy(dcpu_rdy),
        .dbg_cpu_irq(dcpu_irq), .dbg_cpu_nmi(dcpu_nmi),
        .dbg_cpu_waiting(dcpu_waiting), .dbg_cpu_stopped(dcpu_stopped),
        .sdram_clk(clk),
        .sdram_addrA(sdram_addrA), .sdram_dinA(sdram_dinA), .sdram_weA(sdram_weA),
        .sdram_oeA(sdram_oeA), .sdram_doutA(8'd0), .sdram_doneA(sdram_doneA),
        .sdram_addrB(sdram_addrB), .sdram_dinB(sdram_dinB), .sdram_weB(sdram_weB),
        .sdram_oeB(sdram_oeB), .sdram_doutB(8'd0),
        .sdram_stream_req(sdram_stream_req), .sdram_stream_addr(sdram_stream_addr),
        .sdram_stream_words(sdram_stream_words), .sdram_stream_ready(sdram_stream_ready),
        .sdram_stream_dout(16'd0), .sdram_stream_valid(1'b0), .sdram_stream_busy(1'b0),
        .sdram_stream_done(1'b1),
        .fio_event(fio_event), .nic_event(nic_event),
        .cpu_mon_addr(cpu_mon_addr), .cpu_mon_wdata(cpu_mon_wdata), .cpu_mon_we(cpu_mon_we),
        .sid1_audio_out(sid1o), .sid2_audio_out(sid2o)
    );

    always #18 clk = ~clk;     // ~27.7 MHz
    initial begin rst = 1; repeat (40) @(posedge clk); rst = 0; end

    // ---- console capture ($A00E) + "Ready" detect via 5-char shift ----
    integer cyc = 0; always @(posedge clk) cyc = cyc + 1;
    always @(posedge clk) if (cyc % 50000 == 0 && cyc > 0)
        $display("[t] cyc=%0d PC=%04x A=%02x daddr=%04x ce=%b xstall=%b xmc=%b xbusy=%b oeA=%b weA=%b", cyc,
                 dcpu_pc, dcpu_a, dcpu_addr, dut.cpu_ce, dut.xram_stall, dut.xmc_access,
                 dut.xram_busy, sdram_oeA, sdram_weA);
    reg [7:0] r0,r1,r2,r3,r4; reg ready_seen = 0; integer ready_at = 0;
    always @(posedge clk) begin
        if (cpu_mon_we && cpu_mon_addr == 16'hA00E) begin
            if (cpu_mon_wdata >= 8'h20 && cpu_mon_wdata < 8'h7F) $write("%c", cpu_mon_wdata);
            else if (cpu_mon_wdata == 8'h0D) $write("\n");
            r0<=cpu_mon_wdata; r1<=r0; r2<=r1; r3<=r2; r4<=r3;
            if (!ready_seen && r4=="R" && r3=="e" && r2=="a" && r1=="d" && r0=="y") begin end
            if (!ready_seen && r3=="R" && r2=="e" && r1=="a" && r0=="d" && cpu_mon_wdata=="y") begin
                ready_seen<=1; ready_at<=cyc; $display("\n=== READY at cycle %0d ===", cyc);
            end
        end
    end

    // ---- rolling instruction buffer + crash detect ----
    reg [15:0] pcb[0:127]; reg [7:0] irb[0:127], ab[0:127], xb[0:127], yb[0:127], spb[0:127], pb[0:127];
    integer bi = 0; reg [15:0] lastpc = 16'hFFFF;
    integer k;
    always @(posedge clk) begin
        if (!rst && dcpu_pc !== lastpc) begin
            lastpc <= dcpu_pc;
            pcb[bi%128]=dcpu_pc; irb[bi%128]=dcpu_ir; ab[bi%128]=dcpu_a; xb[bi%128]=dcpu_x;
            yb[bi%128]=dcpu_y; spb[bi%128]=dcpu_sp; pb[bi%128]=dcpu_flags; bi=bi+1;
            if (dcpu_pc >= 16'h0500 && dcpu_pc < 16'hC000) begin
                $display("\n*** CRASH: PC=%04x at cycle %0d (%0d after Ready, ready_seen=%0d) ***",
                         dcpu_pc, cyc, cyc-ready_at, ready_seen);
                $display("--- last 128 instructions (oldest first) ---");
                for (k=0;k<128;k=k+1) begin
                    integer idx; idx=(bi+k)%128;
                    $display(" PC=%04x IR=%02x A=%02x X=%02x Y=%02x SP=%02x P=%02x",
                             pcb[idx],irb[idx],ab[idx],xb[idx],yb[idx],spb[idx],pb[idx]);
                end
                $finish;
            end
        end
    end

    initial begin
        #120_000_000;
        $display("\n=== TIMEOUT cycle %0d ready_seen=%0d PC=%04x ===", cyc, ready_seen, dcpu_pc);
        $finish;
    end
endmodule
