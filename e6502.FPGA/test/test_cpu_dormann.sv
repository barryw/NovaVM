// Klaus Dormann CPU acceptance harness for the FPGA 6502 core.
//
// This intentionally instantiates only Arlet's CPU plus a simple synchronous
// 64K RAM. It tests the core contract without NovaVM MMIO, VGC, SID, or top.sv
// bus behavior in the way.

`timescale 1ns/1ps

module test_cpu_dormann;

    logic clk = 0;
    always #20 clk = ~clk;

    logic reset = 1;
    logic rdy   = 1;
    logic irq   = 0;
    logic nmi   = 0;

    wire [15:0] ab;
    logic [7:0] di = 8'h00;
    wire [7:0] dout;
    wire       we;

    wire [15:0] dbg_pc;
    wire [7:0]  dbg_a, dbg_x, dbg_y, dbg_s, dbg_flags;
    wire        dbg_waiting, dbg_stopped;

    byte unsigned mem [0:65535];

    cpu dut (
        .clk(clk),
        .reset(reset),
        .AB(ab),
        .DI(di),
        .DO(dout),
        .WE(we),
        .IRQ(irq),
        .NMI(nmi),
        .RDY(rdy),
        .dbg_resume(1'b0),
        .dbg_waiting(dbg_waiting),
        .dbg_stopped(dbg_stopped),
        .dbg_pc(dbg_pc),
        .dbg_a(dbg_a),
        .dbg_x(dbg_x),
        .dbg_y(dbg_y),
        .dbg_s(dbg_s),
        .dbg_flags(dbg_flags)
    );

    // Arlet's core expects synchronous memory: data appears one cycle after AB.
    always_ff @(posedge clk) begin
        if (we)
            mem[ab] <= dout;
        di <= mem[ab];
    end

    string bin_path;
    string test_name;
    int fd;
    int bytes_read;
    int load_addr;
    int start_pc;
    int pass_pc;
    int max_cycles;
    int pass_seen;
    int cycles;
    int enable_interrupts;
    int trace_from;
    int trace_to;
    int trace_limit;
    int trace_count;
    int cmos_d_clear;
    int patched_masks;

    logic [5:0] irq_fired = 6'b0;
    logic [5:0] nmi_fired = 6'b0;
    logic irq_pending = 0;
    int nmi_hold = 0;

    always_ff @(posedge clk) begin
        if (cmos_d_clear != 0 && we &&
            (ab == 16'h000c || ab == 16'h000f) && dout[3])
            $fatal(1, "CMOS interrupt mode expected D clear in captured flags: addr=$%04X flags=$%02X",
                   ab, dout);
    end

    function automatic int irq_trigger_index(input logic [15:0] pc);
        case (pc)
            16'h0434: return 0;
            16'h0464: return 1;
            16'h04A3: return 2;
            16'h04DE: return 3;
            16'h06A0: return 4;
            16'h06DB: return 5;
            default:  return -1;
        endcase
    endfunction

    function automatic int nmi_trigger_index(input logic [15:0] pc);
        case (pc)
            16'h05C8: return 0;
            16'h05F8: return 1;
            16'h0637: return 2;
            16'h0672: return 3;
            16'h06A0: return 4;
            16'h06DB: return 5;
            default:  return -1;
        endcase
    endfunction

    task automatic maybe_fire_interrupts;
        int idx;
        if (enable_interrupts != 0) begin
            idx = irq_trigger_index(dbg_pc);
            if (idx >= 0 && !irq_fired[idx]) begin
                irq_fired[idx] = 1'b1;
                irq_pending = 1'b1;
                $display("  IRQ trigger at PC=$%04X", dbg_pc);
            end

            idx = nmi_trigger_index(dbg_pc);
            if (idx >= 0 && !nmi_fired[idx]) begin
                nmi_fired[idx] = 1'b1;
                nmi_hold = 1;
                $display("  NMI trigger at PC=$%04X", dbg_pc);
            end
        end
    endtask

    task automatic update_interrupt_lines;
        irq = irq_pending;
        nmi = (nmi_hold > 0);
        // Model the C# harness' IrqWaiting behavior: IRQ is level-sticky until
        // the CPU accepts the vector. This matters for the combined NMI+IRQ
        // tests, where IRQ must survive while NMI runs and until CLI.
        if (irq_pending && dbg_pc >= 16'h0774 && dbg_pc <= 16'h0776)
            irq_pending = 1'b0;
        if (nmi_hold > 0)
            nmi_hold--;
    endtask

    initial begin
        bin_path = "../e6502UnitTests/Resources/6502_functional_test.bin";
        test_name = "6502_functional";
        load_addr = 0;
        start_pc = 16'h0400;
        pass_pc = 16'h3399;
        max_cycles = 50_000_000;
        enable_interrupts = 0;
        trace_from = 0;
        trace_to = 0;
        trace_limit = 256;
        trace_count = 0;
        cmos_d_clear = 0;
        patched_masks = 0;

        void'($value$plusargs("BIN=%s", bin_path));
        void'($value$plusargs("NAME=%s", test_name));
        void'($value$plusargs("LOAD=%h", load_addr));
        void'($value$plusargs("START=%h", start_pc));
        void'($value$plusargs("PASS=%h", pass_pc));
        void'($value$plusargs("MAX_CYCLES=%d", max_cycles));
        void'($value$plusargs("INTERRUPTS=%d", enable_interrupts));
        void'($value$plusargs("TRACE_FROM=%h", trace_from));
        void'($value$plusargs("TRACE_TO=%h", trace_to));
        void'($value$plusargs("TRACE_LIMIT=%d", trace_limit));
        void'($value$plusargs("CMOS_D_CLEAR=%d", cmos_d_clear));

        $display("=== Dormann CPU harness: %s ===", test_name);
        $display("  bin=%s load=$%04X start=$%04X pass=$%04X max_cycles=%0d interrupts=%0d",
                 bin_path, load_addr[15:0], start_pc[15:0], pass_pc[15:0],
                 max_cycles, enable_interrupts);

        for (int i = 0; i < 65536; i++)
            mem[i] = 8'h00;

        fd = $fopen(bin_path, "rb");
        if (fd == 0)
            $fatal(1, "Could not open binary: %s", bin_path);
        bytes_read = $fread(mem, fd, load_addr);
        $fclose(fd);
        $display("  loaded %0d bytes", bytes_read);

        if (cmos_d_clear != 0) begin
            // The bundled interrupt binary is assembled for NMOS D-preserve
            // behavior. In CMOS mode, ignore the D bit in its unchanged-flags
            // masks, then assert above that interrupt handlers actually saw D=0.
            for (int i = load_addr; i < load_addr + bytes_read - 1; i++) begin
                if (mem[i] == 8'h29 && mem[i + 1] == 8'hcb) begin
                    mem[i + 1] = 8'hc3;
                    patched_masks++;
                end
            end
            $display("  CMOS D-clear mode: patched %0d NMOS flag masks", patched_masks);
        end

        // C# tests boot by forcing PC. Arlet boots from the reset vector, so
        // patch the vector to the same start address for equivalent behavior.
        mem[16'hFFFC] = start_pc[7:0];
        mem[16'hFFFD] = start_pc[15:8];

        repeat (10) @(posedge clk);
        reset = 0;

        pass_seen = 0;
        cycles = 0;
        forever begin
            @(posedge clk);
            cycles++;

            maybe_fire_interrupts();
            update_interrupt_lines();

            if (trace_from != trace_to &&
                dbg_pc >= trace_from[15:0] && dbg_pc <= trace_to[15:0] &&
                trace_count < trace_limit) begin
                trace_count++;
                $display("  trace[%0d] cyc=%0d pc=$%04X ab=$%04X di=$%02X do=$%02X we=%0d state=%s ir=$%02X dimux=$%02X add=$%02X A=$%02X X=$%02X Y=$%02X S=$%02X P=$%02X",
                         trace_count, cycles, dbg_pc, ab, di, dout, we,
                         dut.statename, dut.IR, dut.DIMUX, dut.ADD,
                         dbg_a, dbg_x, dbg_y, dbg_s, dbg_flags);
            end

            if (dbg_pc >= pass_pc[15:0] && dbg_pc <= (pass_pc[15:0] + 16'd2)) begin
                pass_seen++;
                if (pass_seen == 16) begin
                    $display("  PASS: observed pass loop $%04X-$%04X repeatedly after %0d cycles",
                             pass_pc[15:0], pass_pc[15:0] + 16'd2, cycles);
                    $display("  final A=$%02X X=$%02X Y=$%02X S=$%02X P=$%02X",
                             dbg_a, dbg_x, dbg_y, dbg_s, dbg_flags);
                    $finish;
                end
            end

            if (cycles >= max_cycles) begin
                $display("  FAIL: timeout after %0d cycles", cycles);
                $display("  final PC=$%04X AB=$%04X DI=$%02X DO=$%02X WE=%0d A=$%02X X=$%02X Y=$%02X S=$%02X P=$%02X",
                         dbg_pc, ab, di, dout, we, dbg_a, dbg_x, dbg_y, dbg_s, dbg_flags);
                $display("  zp: ad1[$0D]=$%02X ad2[$0E]=$%02X adrl[$0F]=$%02X adrh[$10]=$%02X adrf[$11]=$%02X",
                         mem[16'h000D], mem[16'h000E], mem[16'h000F], mem[16'h0010], mem[16'h0011]);
                $display("  stack: [$01FC]=$%02X [$01FD]=$%02X [$01FE]=$%02X [$01FF]=$%02X",
                         mem[16'h01FC], mem[16'h01FD], mem[16'h01FE], mem[16'h01FF]);
                $fatal(1, "Dormann CPU harness timeout");
            end
        end
    end

endmodule
